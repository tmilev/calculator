// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfImplementationHeader1_HeaderUniversalEnveloping
#define vpfImplementationHeader1_HeaderUniversalEnveloping
#include "math_extra_universal_enveloping.h"

template <class Coefficient>
Coefficient SemisimpleLieAlgebra::getKillingFormProductWRTLevi(
  const ElementSemisimpleLieAlgebra<Coefficient>& left,
  const ElementSemisimpleLieAlgebra<Coefficient>& right,
  const Selection& rootsNotInLevi
) {
  MacroRegisterFunctionWithName("SemisimpleLieAlgebra::getKillingFormProductWRTLevi");
  Coefficient result = 0;
  ElementSemisimpleLieAlgebra<Coefficient> adadAppliedToMon, tempElt;
  ChevalleyGenerator baseGen;
  Vector<Rational> rootsNotInLeviVectorForm = rootsNotInLevi;
  Vector<Rational> theWeight;
  for (int i = 0; i < this->getNumberOfGenerators(); i ++) {
    theWeight = this->getWeightOfGenerator(i);
    if (theWeight.scalarEuclidean(rootsNotInLeviVectorForm) != 0) {
      continue;
    }
    baseGen.makeGenerator(*this, i);
    adadAppliedToMon.makeZero();
    adadAppliedToMon.addMonomial(baseGen, 1);
    this->lieBracket(right, adadAppliedToMon, tempElt);
    this->lieBracket(left, tempElt, adadAppliedToMon);
    result += adadAppliedToMon.getCoefficientOf(baseGen);
  }
  return result;
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::makeCasimirWRTLeviParabolic(
  SemisimpleLieAlgebra& theOwner, const Selection& theLeviRoots
) {
  MacroRegisterFunctionWithName("ElementUniversalEnveloping::makeCasimirWRTLeviParabolic");
  if (theLeviRoots.cardinalitySelection == 0) {
    this->makeZero(theOwner);
    return;
  }
  Coefficient result = 0;
  ElementSemisimpleLieAlgebra<Rational> leftE, rightE;
  ChevalleyGenerator baseGen;
  Selection rootsNotInLEvi = theLeviRoots;
  rootsNotInLEvi.invertSelection();
  Vector<Rational> rootsNotInLeviVectorForm = rootsNotInLEvi;
  Vector<Rational> theWeightLeft, theWeightRight;
  this->makeZero(theOwner);
  MonomialUniversalEnveloping<Coefficient> theMon;
  Rational theCF;
  //Coefficient theCFconverted;
  for (int i = 0; i < theOwner.getNumberOfGenerators(); i ++) {
    theWeightLeft = theOwner.getWeightOfGenerator(i);
    if (theWeightLeft.scalarEuclidean(rootsNotInLeviVectorForm) != 0) {
      continue;
    }
    if (theWeightLeft.isEqualToZero()) {
      continue;
    }
    theMon.makeOne(theOwner);
    int indexOpposite = theOwner.getGeneratorFromRoot(- theWeightLeft);
    theMon.generatorsIndices.addOnTop(i);
    theMon.generatorsIndices.addOnTop(indexOpposite);
    theMon.powers.addOnTop(1);
    theMon.powers.addOnTop(1);
    leftE.makeGenerator(i, theOwner);
    rightE.makeGenerator(indexOpposite, theOwner);
    theCF = theOwner.getKillingFormProductWRTLevi(leftE, rightE, rootsNotInLEvi);
    theCF.invert();
    this->addMonomial(theMon, theCF);
  }
  Matrix<Rational> killingRestrictedToCartan;
  killingRestrictedToCartan.initialize(theLeviRoots.cardinalitySelection, theLeviRoots.cardinalitySelection);
  for (int i = 0; i < theLeviRoots.cardinalitySelection; i ++) {
    for (int j = i; j < theLeviRoots.cardinalitySelection; j ++) {
      theWeightLeft.makeEi(theOwner.getRank(), theLeviRoots.elements[i]);
      theWeightRight.makeEi(theOwner.getRank(), theLeviRoots.elements[j]);
      leftE.makeCartanGenerator(theWeightLeft, theOwner);
      rightE.makeCartanGenerator(theWeightRight, theOwner);
      killingRestrictedToCartan(i, j) = theOwner.getKillingFormProductWRTLevi(leftE, rightE, rootsNotInLEvi);
      killingRestrictedToCartan(j, i) = killingRestrictedToCartan(i, j);
    }
  }
  killingRestrictedToCartan.invert();
  ElementUniversalEnveloping<Coefficient> leftUE, rightUE;
  Vector<Rational> currentEj;
  for (int i = 0; i < theLeviRoots.cardinalitySelection; i ++) {
    theWeightLeft.makeEi(theOwner.getRank(), theLeviRoots.elements[i]);
    theWeightRight.makeZero(theOwner.getRank());
    for (int j = 0; j < theLeviRoots.cardinalitySelection; j ++) {
      currentEj.makeEi(theOwner.getRank(), theLeviRoots.elements[j]);
      theWeightRight += currentEj * killingRestrictedToCartan(i, j);
    }
    leftUE.makeCartanGenerator(theWeightLeft, theOwner);
    rightUE.makeCartanGenerator(theWeightRight, theOwner);
    leftUE *= rightUE;
    *this += leftUE;
  }
  this->simplify();
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::modOutVermaRelations(
  const Vector<Coefficient>* substitutionHiGoesToIthElement, const Coefficient& ringUnit, const Coefficient& ringZero
) {
  MonomialUniversalEnveloping<Coefficient> tempMon;
  ElementUniversalEnveloping<Coefficient> output;
  output.makeZero(*this->owner);
  Coefficient acquiredCoeff;
  for (int i = 0; i < this->size(); i ++) {
    tempMon = (*this)[i];
    tempMon.modOutVermaRelations(acquiredCoeff, substitutionHiGoesToIthElement, ringUnit, ringZero);
    acquiredCoeff *= this->coefficients[i];
    output.addMonomial(tempMon, acquiredCoeff);
  }
  this->operator=(output);
}

template<class Coefficient>
void ElementUniversalEnveloping<Coefficient>::lieBracketOnTheLeft(const ElementSemisimpleLieAlgebra<Rational>& left) {
  if (this->isEqualToZero()) {
    this->makeZero(*this->owner);
    return;
  }
  ElementUniversalEnveloping<Coefficient> tempElt1, tempElt2;
  tempElt1.assignElementLieAlgebra(left, *this->owner, this->coefficients[0].one());
  tempElt2 = *this;
  tempElt2.lieBracketOnTheRight(tempElt1, *this);
}

template<class Coefficient>
bool MonomialUniversalEnveloping<Coefficient>::adjointRepresentationAction(
  const ElementUniversalEnveloping<Coefficient>& input, ElementUniversalEnveloping<Coefficient>& output
) const {
  output.makeZero(*this->owner);
  ElementSemisimpleLieAlgebra<Rational> tempElt;
  output = input;
  for (int i = this->generatorsIndices.size - 1; i >= 0; i --) {
    int nextCycleSize;
    if (!this->powers[i].isSmallInteger(&nextCycleSize)) {
      return false;
    }
    for (int j = 0; j < nextCycleSize; j ++) {
      tempElt.makeGenerator(this->generatorsIndices[i], *this->owner) ;
      output.lieBracketOnTheLeft(tempElt);
    }
  }
  return true;
}

template<class Coefficient>
bool ElementUniversalEnveloping<Coefficient>::adjointRepresentationAction(
  const ElementUniversalEnveloping<Coefficient>& input,
  ElementUniversalEnveloping<Coefficient>& output
) const {
  if (&input == &output) {
    global.fatal << "Input not allowed to coincide with output. " << global.fatal;
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
void ElementUniversalEnveloping<Coefficient>::simplify(const Coefficient& ringUnit) {
  ElementUniversalEnveloping<Coefficient> buffer;
  ElementUniversalEnveloping<Coefficient> outpuT;
  MonomialUniversalEnveloping<Coefficient> tempMon;
  Coefficient currentCoeff;
  outpuT.makeZero(*this->owner);
  for (; this->size() > 0;) {
    // FormatExpressions tempFormat;
    //tempFormat.MakeAlphabetArbitraryWithIndex("g", "h");
    this->popMonomial(this->size() - 1, tempMon, currentCoeff);
    bool reductionOccurred = false;
    for (int i = 0; i < tempMon.generatorsIndices.size - 1; i ++) {
      if (!this->getOwner().areOrderedProperly(tempMon.generatorsIndices[i], tempMon.generatorsIndices[i + 1])) {
        if (tempMon.switchConsecutiveIndicesIfTheyCommute(i)) {
          this->addMonomial(tempMon, currentCoeff);
          reductionOccurred = true;
          break;
        }
        if (tempMon.commutingAnBtoBAnPlusLowerOrderAllowed(
          tempMon.powers[i], tempMon.generatorsIndices[i], tempMon.powers[i + 1], tempMon.generatorsIndices[i + 1]
        )) {
          tempMon.commuteAnBtoBAnPlusLowerOrder(i, buffer, ringUnit);
          buffer *= currentCoeff;
          *this += buffer;
          reductionOccurred = true;
          break;
        }
        if (tempMon.commutingABntoBnAPlusLowerOrderAllowed(
          tempMon.powers[i], tempMon.generatorsIndices[i], tempMon.powers[i + 1], tempMon.generatorsIndices[i + 1]
        )) {
          tempMon.commuteABntoBnAPlusLowerOrder(i, buffer, ringUnit);
          buffer *= currentCoeff;
          *this += buffer;
          reductionOccurred = true;
          break;
        }
      }
    }
    if (!reductionOccurred) {
      outpuT.addMonomial(tempMon, currentCoeff);
    }
  }
  *this = outpuT;
}

template <class Coefficient>
bool MonomialUniversalEnveloping<Coefficient>::commutingAnBtoBAnPlusLowerOrderAllowed(
  Coefficient& theLeftPower, int leftGeneratorIndex,Coefficient& theRightPower, int rightGeneratorIndex
) {
  return this->commutingABntoBnAPlusLowerOrderAllowed(theRightPower, rightGeneratorIndex, theLeftPower, leftGeneratorIndex);
}

template <class Coefficient>
bool MonomialUniversalEnveloping<Coefficient>::commutingABntoBnAPlusLowerOrderAllowed(
  Coefficient& theLeftPower, int leftGeneratorIndex, Coefficient& theRightPower, int rightGeneratorIndex
) {
  int leftPower, rightPower;
  if (!theLeftPower.isSmallInteger(&leftPower)) {
    return false;
  }
  if (leftPower < 0) {
    return false;
  }
  if (theRightPower.isSmallInteger(&rightPower)) {
    if (rightPower >= 0) {
      return true;
    }
  }
  int numPosRoots = this->getOwner().theWeyl.rootsOfBorel.size;
  int theDimension = this->getOwner().theWeyl.cartanSymmetric.numberOfRows;
  if (rightGeneratorIndex >= numPosRoots && rightGeneratorIndex < numPosRoots + theDimension) {
    return this->getOwner().theLiebrackets.elements[leftGeneratorIndex][rightGeneratorIndex].isEqualToZero();
  }
  return true;
}

template <class Coefficient>
bool MonomialUniversalEnveloping<Coefficient>::switchConsecutiveIndicesIfTheyCommute(int theLeftIndex) {
  if (theLeftIndex >= this->generatorsIndices.size - 1) {
    return false;
  }
  int leftGenerator = this->generatorsIndices[theLeftIndex];
  int rightGenerator = this->generatorsIndices[theLeftIndex + 1];
  if (!this->getOwner().theLiebrackets.elements[leftGenerator][rightGenerator].isEqualToZero()) {
    return false;
  }
  this->generatorsIndices.swapTwoIndices(theLeftIndex, theLeftIndex + 1);
  this->powers.swapTwoIndices(theLeftIndex, theLeftIndex + 1);
  this->simplifyEqualConsecutiveGenerators(theLeftIndex - 1);
  return true;
}

template <class Coefficient>
void MonomialUniversalEnveloping<Coefficient>::commuteAnBtoBAnPlusLowerOrder(
  int indexA, ElementUniversalEnveloping<Coefficient>& output, const Coefficient& ringUnit
) {
  if (indexA == this->generatorsIndices.size - 1) {
    return;
  }
  output.makeZero(*this->owner);
  MonomialUniversalEnveloping<Coefficient> tempMon;
  tempMon.makeOne(*this->owner);
  tempMon.powers.setExpectedSize(this->generatorsIndices.size + 2);
  tempMon.generatorsIndices.setExpectedSize(this->generatorsIndices.size + 2);
  tempMon.powers.size = 0;
  tempMon.generatorsIndices.size = 0;
  int rightGeneratorIndeX = this->generatorsIndices[indexA + 1];
  int leftGeneratorIndeX = this->generatorsIndices[indexA];
  Coefficient theRightPoweR, theLeftPoweR;
  theRightPoweR = this->powers[indexA + 1];
  theLeftPoweR = this->powers[indexA];
  theRightPoweR -= 1;
  int powerDroP = 0;
  Coefficient acquiredCoefficienT, incomingAcquiredCoefficienT;
  acquiredCoefficienT = ringUnit;
  for (int i = 0; i < indexA; i ++) {
    tempMon.multiplyByGeneratorPowerOnTheRight(this->generatorsIndices[i], this->powers[i]);
  }
  MonomialUniversalEnveloping<Coefficient> startMon;
  startMon = tempMon;
  ElementSemisimpleLieAlgebra<Rational> adAToTheIthOfB, aElt;
  adAToTheIthOfB.makeGenerator(rightGeneratorIndeX, *this->owner);
  aElt.makeGenerator(leftGeneratorIndeX, *this->owner);
  //Formula realized:
  //a^n b =\sum_{i = 0}^\infty \binom{n}{i} (\ad a)^i (b)a^{n-i}
  //Proof (Dixmier): let L_x stand for left multiplication by x and R_x stand for right multiplication.
  //Then L_x and R_x commute and L_x-R_x =ad_x, i.e.
  //(L_a)^n =(R_a +ad_a)^n.
  do {
    for (int i = 0; i < adAToTheIthOfB.size(); i ++) {
      int theNewGeneratorIndex = adAToTheIthOfB[i].theGeneratorIndex;
      tempMon = startMon;
      incomingAcquiredCoefficienT = acquiredCoefficienT;
      incomingAcquiredCoefficienT *= adAToTheIthOfB.coefficients[i];
      tempMon.multiplyByGeneratorPowerOnTheRight(theNewGeneratorIndex, ringUnit);
      tempMon.multiplyByGeneratorPowerOnTheRight(leftGeneratorIndeX, theLeftPoweR);
      tempMon.multiplyByGeneratorPowerOnTheRight(rightGeneratorIndeX, theRightPoweR);
      for (int i = indexA + 2; i < this->generatorsIndices.size; i ++) {
        tempMon.multiplyByGeneratorPowerOnTheRight(this->generatorsIndices[i], this->powers[i]);
      }
      output.addMonomial(tempMon, incomingAcquiredCoefficienT);
    }
    powerDroP ++;
    acquiredCoefficienT *= theLeftPoweR;
    acquiredCoefficienT /= powerDroP;
    theLeftPoweR -= 1;
    this->getOwner().lieBracket(aElt, adAToTheIthOfB, adAToTheIthOfB);
  } while (!adAToTheIthOfB.isEqualToZero() && !acquiredCoefficienT.isEqualToZero());
}

template <class Coefficient>
void MonomialUniversalEnveloping<Coefficient>::commuteABntoBnAPlusLowerOrder(
  int theIndeX,
  ElementUniversalEnveloping<Coefficient>& output,
  const Coefficient& ringUnit
) {
  if (theIndeX == this->generatorsIndices.size - 1)
    return;
  output.makeZero(*this->owner);
  MonomialUniversalEnveloping<Coefficient> tempMon;
  tempMon.makeOne(*this->owner);
  tempMon.powers.setExpectedSize(this->generatorsIndices.size + 2);
  tempMon.generatorsIndices.setExpectedSize(this->generatorsIndices.size + 2);
  tempMon.powers.size = 0;
  tempMon.generatorsIndices.size = 0;
  int rightGeneratorIndex = this->generatorsIndices[theIndeX + 1];
  int leftGeneratorIndex = this->generatorsIndices[theIndeX];
  Coefficient theRightPower, theLeftPower;
  theRightPower = this->powers[theIndeX + 1];
  theLeftPower = this->powers[theIndeX];
  theLeftPower -= 1;
  int powerDrop = 0;
  Coefficient acquiredCoefficient, incomingAcquiredCoefficient;
  acquiredCoefficient = ringUnit;
  for (int i = 0; i < theIndeX; i ++) {
    tempMon.multiplyByGeneratorPowerOnTheRight(this->generatorsIndices[i], this->powers[i]);
  }
  tempMon.multiplyByGeneratorPowerOnTheRight(this->generatorsIndices[theIndeX], theLeftPower);
  MonomialUniversalEnveloping<Coefficient> startMon;
  startMon = tempMon;
  ElementSemisimpleLieAlgebra<Rational> adResult, tempElt, rightGeneratorElt;
  adResult.makeGenerator(leftGeneratorIndex, *this->owner);
  rightGeneratorElt.makeGenerator(rightGeneratorIndex, *this->owner);
  //Formula realized:
  //a b^n  &= &\sum_{i = 0}^\infty b^{n-i}(-\ad b)^i (a) \binom{n}{i} .
  //Proof (Dixmier): let L_x stand for left multiplication by x and R_x stand for right multiplication.
  //Then L_x and R_x commute and L_x-R_x =ad_x, i.e.
  //(L_b-ad_b)^n =R_b^n.
  do {
    for (int i = 0; i < adResult.size(); i ++) {
      int theNewGeneratorIndex = adResult[i].theGeneratorIndex;
      tempMon = startMon;
      tempMon.multiplyByGeneratorPowerOnTheRight(rightGeneratorIndex, theRightPower);
      incomingAcquiredCoefficient = acquiredCoefficient;
      incomingAcquiredCoefficient *= adResult.coefficients[i];
      tempMon.multiplyByGeneratorPowerOnTheRight(theNewGeneratorIndex, ringUnit);
      for (int i = theIndeX + 2; i < this->generatorsIndices.size; i ++) {
        tempMon.multiplyByGeneratorPowerOnTheRight(this->generatorsIndices[i], this->powers[i]);
      }
      output.addMonomial(tempMon, incomingAcquiredCoefficient);
    }
    acquiredCoefficient *= theRightPower;
    theRightPower -= 1;
    this->getOwner().lieBracket(adResult, rightGeneratorElt, tempElt);
    adResult = tempElt;
    powerDrop ++;
    acquiredCoefficient /= powerDrop;
  } while (!adResult.isEqualToZero() && !acquiredCoefficient.isEqualToZero());
}

template <class Coefficient>
bool MonomialUniversalEnveloping<Coefficient>::highestWeightTransposeAntiAutomorphismBilinearForm(
  const MonomialUniversalEnveloping<Coefficient>& right,
  Coefficient& output,
  const Vector<Coefficient>* substitutionHiGoesToIthElement,
  const Coefficient& ringUnit,
  const Coefficient& ringZero,
  std::stringstream* logStream
) {
  ElementUniversalEnveloping<Coefficient> tempElt1, tempElt2;
  tempElt1.makeZero(*this->owner);
  tempElt1.addMonomial(*this, ringUnit);
  tempElt2.makeZero(*this->owner);
  tempElt2.addMonomial(right, ringUnit);
  return tempElt1.highestWeightTransposeAntiAutomorphismBilinearForm(tempElt2, output, substitutionHiGoesToIthElement, ringUnit, ringZero, logStream);
}

template<class Coefficient>
bool ElementUniversalEnveloping<Coefficient>::applyTransposeAntiAutoOnMe() {
  MonomialUniversalEnveloping<Coefficient> tempMon;
  ElementUniversalEnveloping<Coefficient> result;
  result.makeZero(*this->owner);
  int numPosRoots = this->getOwner().getNumberOfPositiveRoots();
  int theRank = this->getOwner().getRank();
  Coefficient theCoeff;
  this->checkNumberOfCoefficientsConsistency();
  for (int i = 0; i < this->size(); i ++) {
    const MonomialUniversalEnveloping<Coefficient>& currentMon = (*this)[i];
    theCoeff = this->coefficients[i];
    tempMon.owner = currentMon.owner;
    tempMon.powers.size = 0;
    tempMon.generatorsIndices.size = 0;
    for (int j = currentMon.powers.size - 1; j >= 0; j --) {
      int thePower;
      if (!currentMon.powers[j].isSmallInteger(&thePower)) {
        return false;
      }
      int theGenerator = currentMon.generatorsIndices[j];
      if (theGenerator < numPosRoots) {
        theGenerator = 2 * numPosRoots + theRank - 1 - theGenerator;
      } else if (theGenerator >= numPosRoots + theRank) {
        theGenerator = - theGenerator + 2 * numPosRoots + theRank - 1;
      }
      tempMon.multiplyByGeneratorPowerOnTheRight(theGenerator, currentMon.powers[j]);
    }
    result.addMonomial(tempMon, theCoeff);
  }
  *this = result;
  return true;
}

template <class Coefficient>
bool ElementUniversalEnveloping<Coefficient>::highestWeightTransposeAntiAutomorphismBilinearForm(
  const ElementUniversalEnveloping<Coefficient>& right,
  Coefficient& output,
  const Vector<Coefficient>* substitutionHiGoesToIthElement,
  const Coefficient& ringUnit,
  const Coefficient& ringZero,
  std::stringstream* logStream
) const {
  output = ringZero;
  ElementUniversalEnveloping<Coefficient> TAleft;
  TAleft = *this;
  if (!TAleft.applyTransposeAntiAutoOnMe()) {
    return false;
  }
  ElementUniversalEnveloping<Coefficient> Accum, intermediateAccum, tempElt, startingElt;
  List<int> oldOrder = this->getOwner().UEGeneratorOrderIncludingCartanElts;
  int numPosRoots = this->getOwner().getNumberOfPositiveRoots();
  for (int i = 0; i < numPosRoots; i ++) {
    this->getOwner().UEGeneratorOrderIncludingCartanElts[i] = - 1;
  }
  Accum.makeZero(this->getOwner());
  MonomialUniversalEnveloping<Coefficient> constMon;
  constMon.makeOne(this->getOwner());
  if (logStream != nullptr) {
    *logStream << "left eltement transposed: "
    << TAleft.toString(&global.theDefaultFormat.getElement()) << "<br>";
    *logStream << "right element: " << right.toString(&global.theDefaultFormat.getElement()) << "<br>";
  }
  startingElt = right;
  startingElt.simplify(ringUnit);
  if (logStream != nullptr) {
    *logStream << "right element after simplification: "
    << startingElt.toString(&global.theDefaultFormat.getElement()) << "<br>";
  }
  startingElt.modOutVermaRelations(substitutionHiGoesToIthElement, ringUnit);
  if (logStream != nullptr) {
    *logStream << "right element after Verma rels: "
    << startingElt.toString(&global.theDefaultFormat.getElement()) << "<br>";
  }
  Coefficient leftMonCoeff;
  for (int j = 0; j < TAleft.size(); j ++) {
    intermediateAccum = startingElt;
    const MonomialUniversalEnveloping<Coefficient>& leftMon = TAleft[j];
    leftMonCoeff = TAleft.coefficients[j];
    int thePower;
    for (int i = leftMon.powers.size - 1; i >= 0; i --) {
      if (leftMon.powers[i].isSmallInteger(&thePower)) {
        for (int k = 0; k < thePower; k ++) {
          tempElt.makeOneGenerator(leftMon.generatorsIndices[i], this->getOwner(), ringUnit);
          MathRoutines::swap(tempElt, intermediateAccum);
          if (logStream != nullptr) {
            //*logStream << "tempElt before mult: " << tempElt.toString(global, tempFormat) << "<br>";
            *logStream << "intermediate before mult: "
            << intermediateAccum.toString(&global.theDefaultFormat.getElement()) << "<br>";
          }
          intermediateAccum *= tempElt;
          if (logStream != nullptr) {
            *logStream << "intermediate before simplification: "
            << intermediateAccum.toString(&global.theDefaultFormat.getElement()) << "<br>";
          }
          intermediateAccum.simplify(ringUnit);
          if (logStream != nullptr) {
            *logStream << "intermediate after simplification: "
            << intermediateAccum.toString(&global.theDefaultFormat.getElement()) << "<br>";
          }
          intermediateAccum.modOutVermaRelations(substitutionHiGoesToIthElement, ringUnit, ringZero);
          if (logStream != nullptr) {
            *logStream << "intermediate after Verma rels: "
            << intermediateAccum.toString(&global.theDefaultFormat.getElement()) << "<br>";
          }
        }
      } else {
        this->getOwner().UEGeneratorOrderIncludingCartanElts = oldOrder;
        return false;
      }
    }
    intermediateAccum *= leftMonCoeff;
    Accum += intermediateAccum;
    int theIndex = intermediateAccum.monomials.getIndex(constMon);
    if (theIndex != - 1) {
      output += intermediateAccum.coefficients[theIndex];
    }
  }
  if (logStream != nullptr) {
    *logStream << "final UE element: " << Accum.toString(&global.theDefaultFormat.getElement());
  }
  this->getOwner().UEGeneratorOrderIncludingCartanElts = oldOrder;
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
  int theDimension = this->getOwner().getRank();
  outputCoeff = ringUnit;
  for (int i = this->generatorsIndices.size - 1; i >= 0; i --) {
    int indexCurrentGenerator = this->generatorsIndices[i];
    if (indexCurrentGenerator >= numPosRoots + theDimension) {
      this->makeOne(*this->owner);
      outputCoeff = ringZero;
      return;
    }
    if (indexCurrentGenerator < numPosRoots) {
      return;
    }
    if (indexCurrentGenerator >= numPosRoots && indexCurrentGenerator < numPosRoots + theDimension) {
      if (substitutionHiGoesToIthElement == 0) {
        this->makeOne(*this->owner);
        outputCoeff = ringZero;
        return;
      }
      int theDegree;
      if (!this->powers[i].isSmallInteger(&theDegree)) {
        return;
      }
      int hIndex = indexCurrentGenerator - numPosRoots;
      Coefficient theSubbedH;
      theSubbedH = (*substitutionHiGoesToIthElement)[hIndex];
      MathRoutines::raiseToPower(theSubbedH, theDegree, ringUnit);
      outputCoeff *= theSubbedH;
      this->generatorsIndices.size --;
      this->powers.size --;
    }
  }
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::substitution(const PolynomialSubstitution<Rational>& theSub) {
  ElementUniversalEnveloping<Coefficient> output;
  output.makeZero(*this->owner);
  MonomialUniversalEnveloping<Coefficient> theMon;
  Coefficient tempCF;
  for (int i = 0; i < this->size(); i ++) {
    theMon = (*this)[i];
    theMon.substitution(theSub);
    tempCF = this->coefficients[i];
    tempCF.substitution(theSub);
    output.addMonomial(theMon, tempCF);
  }
  *this = output;
}

template <class Coefficient>
void MonomialUniversalEnveloping<Coefficient>::substitution(const PolynomialSubstitution<Rational>& theSub) {
  for (int i = 0; i < this->generatorsIndices.size; i ++) {
    this->powers[i].substitution(theSub);
  }
  this->simplifyEqualConsecutiveGenerators(0);
}

template <class Coefficient>
void MonomialUniversalEnveloping<Coefficient>::setNumberOfVariables(int newNumVars) {
 //the below code is wrong messed up with substitutions!
  //Left in comments to remind you of what you shouldnt do.
  // if (this->Coefficient.NumVars == newNumVars)
  //  return;
  for (int i = 0; i < this->generatorsIndices.size; i ++) {
    this->powers.objects[i].setNumberOfVariablesSubstituteDeletedByOne(newNumVars);
  }
}

template <class Coefficient>
std::string MonomialUniversalEnveloping<Coefficient>::toString(FormatExpressions* theFormat) const {
  std::stringstream out;
  std::string tempS;
  if (this->owner == nullptr) {
    return "(Error:Programming:NonInitializedMonomial)";
  }
  if (this->generatorsIndices.size == 0) {
    return "1";
  }
  for (int i = 0; i < this->generatorsIndices.size; i ++) {
    Coefficient& thePower = this->powers[i];
    int theIndex = this->generatorsIndices[i];
    tempS = this->getOwner().getStringFromChevalleyGenerator(theIndex, theFormat);
    out << tempS;
    if (!thePower.isEqualToOne()) {
      out << "^";
      out << "{";
      out << thePower.toString(theFormat);
      out << "}";
    }
  }
  return out.str();
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::makeCasimir(SemisimpleLieAlgebra& theOwner) {
  //std::stringstream out;
  this->makeZero(theOwner);
  WeylGroupData& theWeyl = this->getOwner().theWeyl;
  int theDimension = theWeyl.cartanSymmetric.numberOfRows;
  Vector<Rational> tempRoot1, tempRoot2;
//  Matrix<Rational> killingForm;
//  killingForm.initialize(theDimension, theDimension);
//  for (int i = 0; i < theDimension; i ++)
//  { tempRoot1.makeEi(theDimension, i);
//    for (int j = 0; j < theDimension; j ++)
//    { killingForm.elements[i][j] = 0;
//      tempRoot2.makeEi(theDimension, j);
//      for (int k = 0; k<theWeyl.RootSystem.size; k++)
//        killingForm.elements[i][j] += theWeyl.rootScalarCartanRoot(tempRoot1, theWeyl.RootSystem.objects[k])* theWeyl.rootScalarCartanRoot(tempRoot2, theWeyl.RootSystem.objects[k]);
//    }
//  }
//  killingForm.invert(global);
//  killingForm.ComputeDebugString();
//  out << killingForm.toString(true, false);


  ElementUniversalEnveloping<Coefficient> tempElt1, tempElt2;
//this code is to check a math formula:
//  ElementUniversalEnveloping checkElement;
//  checkElement.makeZero(theOwner);
  Matrix<Rational> invertedSymCartan;
  invertedSymCartan = theWeyl.cartanSymmetric;
  invertedSymCartan.invert();
////////////////////////////////////////////////////////////////////////
  for (int i = 0; i < theDimension; i ++) {
    tempRoot1.makeEi(theDimension, i);
  //implementation without the ninja formula:
//    killingForm.actOnVectorColumn(tempRoot1, tempRoot2);
//    tempElt1.makeCartanGenerator(tempRoot1, numVars, theOwner);
//    tempElt2.makeCartanGenerator(tempRoot2, numVars, theOwner);
//    tempElt1*= tempElt2;
//    *this+= tempElt1;
// Alternative implementation using a ninja formula I cooked up after looking at the printouts:
    invertedSymCartan.actOnVectorColumn(tempRoot1, tempRoot2);
    tempElt1.makeCartanGenerator(tempRoot1, theOwner);
    tempElt2.makeCartanGenerator(tempRoot2, theOwner);
    tempElt1 *= tempElt2;
    *this += tempElt1;
  }
  //Rational tempRat;
  //Vector<Rational> theSum;
  for (int i = 0; i < theWeyl.rootSystem.size; i ++) {
    //Implementation without the ninja formula:
//    tempRat = 0;
//    Vector<Rational> & theRoot = theWeyl.RootSystem.objects[i];
//    int indexOfOpposite = theWeyl.RootSystem.getIndex(-theRoot);
//    Vector<Rational> & theOpposite = theWeyl.RootSystem.objects[indexOfOpposite];
//    for (int j = 0; j < theWeyl.RootSystem.size; j ++)
//    { Vector<Rational> & current = theWeyl.RootSystem.objects[j];
//      if (current == theOpposite)
//        tempRat +=2;
//       else
//       { int indexOfSum= theWeyl.RootSystem.getIndex(current +theRoot);
//         if (indexOfSum!= - 1)
//           tempRat +=(theOwner.ChevalleyConstants.elements[i][j]*theOwner.ChevalleyConstants.elements[indexOfOpposite][indexOfSum]);
//       }
//     }
//     tempRat +=2;
//     tempRat = 1/tempRat;
//     tempElt2.makeOneGeneratorCoefficientOne(theOpposite, numVars, theOwner);
//     tempElt1.makeOneGeneratorCoefficientOne(theRoot, numVars, theOwner);
//     tempElt2*= tempElt1;
//
//     tempElt2*= tempRat;
//     *this+= tempElt2;
    //The ninja formula alternative implementation:
    const Vector<Rational>& theRoot = theWeyl.rootSystem[i];
    tempElt2.makeOneGeneratorCoefficientOne(- theRoot, theOwner);
    tempElt1.makeOneGeneratorCoefficientOne(theRoot, theOwner);
    tempElt2 *= tempElt1;
    tempElt2 *= theWeyl.rootScalarCartanRoot(theWeyl.rootSystem[i], theWeyl.rootSystem[i]) / 2;
    *this += tempElt2;
  }
  *this /= theWeyl.getKillingDividedByTraceRatio();
// check that the ninja formula is correct:
//  FormatExpressions tempPolyFormat;
//  tempPolyFormat.MakeAlphabetArbitraryWithIndex("g", "h");
  //this->DebugString= out.str();
//  Vector<Rational> tempRoot;
//  for (int i = 0; i < theDimension; i ++)
//  { tempRoot.makeEi(theDimension, i);
//    if (!length1Explored)
//    { length1= theWeyl.rootScalarCartanRoot(tempRoot, tempRoot);
//      length1Explored = true;
//      coefficient1= 0;
//      for (int j = 0; j < theWeyl.RootsOfBorel.size; j ++)
//      { coefficient1+= theWeyl.rootScalarCartanRoot(tempRoot, theWeyl.RootsOfBorel.objects[j])*theWeyl.rootScalarCartanRoot(tempRoot, theWeyl.RootsOfBorel.objects[j]);
//        coef
//      }
//    }
//  }
  this->simplify(0);
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::substitutionCoefficients(
  PolynomialSubstitution<Rational>& theSub, const Coefficient& ringUnit, const Coefficient& ringZero
) {
  ElementUniversalEnveloping<Coefficient> endResult;
  MonomialUniversalEnveloping<Coefficient> currentMon;
  endResult.makeZero(*this->owner);
  Coefficient tempCF;
  for (int i = 0; i < this->size; i ++) {
    currentMon = this->objects[i];
    this->coefficients[i].substitution(theSub);
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
  ElementUniversalEnveloping<Coefficient> tempElt, tempElt2;
  tempElt = *this;
  tempElt *= right;
  tempElt2 = right;
  tempElt2 *= *this;
  output = tempElt;
  output -= tempElt2;
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::cleanUpZeroCoefficient() {
  for (int i = 0; i < this->size; i ++) {
    if ((*this)[i].theCoefficient.isEqualToZero()) {
      this->removeIndexSwapWithLast(i);
      i --;
    }
  }
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::makeOneGenerator(
  int theIndex, SemisimpleLieAlgebra& inputOwner, const Coefficient& ringUnit
) {
  this->makeZero(inputOwner);
  MonomialUniversalEnveloping<Coefficient> tempMon;
  tempMon.makeOne(inputOwner);
  tempMon.multiplyByGeneratorPowerOnTheRight(theIndex, ringUnit);
  this->addMonomial(tempMon, ringUnit);
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::makeOneGeneratorCoefficientOne(
  int theIndex, SemisimpleLieAlgebra& inputOwner, const Coefficient& ringUnit
) {
  this->makeZero(inputOwner);
  MonomialUniversalEnveloping<Coefficient> tempMon;
  tempMon.makeOne(inputOwner);
  tempMon.multiplyByGeneratorPowerOnTheRight(theIndex, ringUnit);
  this->addMonomial(tempMon, ringUnit);
}

template <class Coefficient>
bool ElementUniversalEnveloping<Coefficient>::getLieAlgebraElementIfPossible(
  ElementSemisimpleLieAlgebra<Rational>& output
) const {
  output.makeZero();
//  SemisimpleLieAlgebra& theOwner = this->owners->objects[this->indexInOwners];
//  int numPosRoots = theOwner.theWeyl.RootsOfBorel.size;
  Coefficient theExponent;
  ChevalleyGenerator tempChevalley;
  for (int i = 0; i < this->size(); i ++) {
    const MonomialUniversalEnveloping<Coefficient>& tempMon = (*this)[i];
    tempMon.getDegree(theExponent);
    if (!theExponent.isEqualToOne()) {
      return false;
    }
    Rational theConst;
    if (!this->coefficients[i].isConstant(&theConst)) {
      return false;
    }
    tempChevalley.makeGenerator(*this->owner, tempMon.generatorsIndices[0]);
    output.addMonomial(tempChevalley, theConst);
  }
  return true;
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::assignElementLieAlgebra(
  const ElementSemisimpleLieAlgebra<Rational>& input, SemisimpleLieAlgebra& inputOwner, const Coefficient& ringUnit
) {
  this->makeZero(inputOwner);
  MonomialUniversalEnveloping<Coefficient> tempMon;
  tempMon.makeOne(inputOwner);
  tempMon.generatorsIndices.setSize(1);
  tempMon.powers.setSize(1);
  tempMon.powers[0] = ringUnit;
  Coefficient tempCF;
  for (int i = 0; i < input.size(); i ++) {
    tempCF = ringUnit; //<- to facilitate implicit type conversion: ringUnit does not have to be of type Rational
    tempCF *= input.coefficients[i];//<- to facilitate implicit type conversion: ringUnit does not have to be of type Rational
    tempMon.generatorsIndices[0] = input[i].theGeneratorIndex;
    this->addMonomial(tempMon, tempCF);
  }
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::getCoordinateFormOfSpanOfElements(
  List<ElementUniversalEnveloping<Coefficient> >& theElements,
  Vectors<Coefficient>& outputCoordinates,
  ElementUniversalEnveloping<Coefficient>& outputCorrespondingMonomials
) {
  if (theElements.size == 0) {
    return;
  }
  outputCorrespondingMonomials.makeZero(*theElements[0].owners, theElements[0].indexInOwners);
  for (int i = 0; i < theElements.size; i ++) {
    for (int j = 0; j < theElements[i].size; j ++) {
      outputCorrespondingMonomials.addOnTopNoRepetition(theElements[i][j]);
    }
  }
  outputCoordinates.setSize(theElements.size);
  Polynomial<Rational> ZeroPoly;
  ZeroPoly.makeZero();
  for (int i = 0; i < theElements.size; i ++) {
    Vector<Coefficient>& current = outputCoordinates[i];
    current.initializeFillInObject(outputCorrespondingMonomials.size, ZeroPoly);
    ElementUniversalEnveloping& currentElt = theElements[i];
    for (int j = 0; j < currentElt.size; j ++) {
      MonomialUniversalEnveloping<Coefficient>& currentMon = currentElt[j];
      current[outputCorrespondingMonomials.getIndex(currentMon)] = currentElt.theCoefficients[j];
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
  int theDimension = this->getOwner().theWeyl.cartanSymmetric.numberOfRows;
  int numPosRoots = this->getOwner().theWeyl.rootsOfBorel.size;
  tempMon.generatorsIndices.setSize(1);
  tempMon.powers.setSize(1);
  Coefficient tempCF;
  for (int i = 0; i < theDimension; i ++) {
    if (!input[i].isEqualToZero()) {
      (*tempMon.generatorsIndices.lastObject()) = i + numPosRoots;
      *tempMon.powers.lastObject() = 1;
      tempCF = 1;               //<- to facilitate type conversion
      tempCF *= input[i]; //<- to facilitate type conversion we call extra assignment
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
bool ElementUniversalEnvelopingOrdered<Coefficient>::getElementUniversalEnveloping(
  ElementUniversalEnveloping<Coefficient>& output, SemisimpleLieAlgebra& inputOwner
) {
  ElementUniversalEnveloping<Coefficient> Accum, tempElt;
  Accum.makeZero(inputOwner);
  for (int i = 0; i < this->size; i ++) {
    if (!this->objects[i].getElementUniversalEnveloping(tempElt, owner)) {
      return false;
    } else {
      Accum += tempElt;
    }
  }
  output = Accum;
  return true;
}

template <class Coefficient>
bool MonomialUniversalEnvelopingOrdered<Coefficient>::getElementUniversalEnveloping(
  ElementUniversalEnveloping<Coefficient>& output, SemisimpleLieAlgebraOrdered& inputOwner
) {
  ElementUniversalEnveloping<Coefficient> Accum;
  ElementUniversalEnveloping<Coefficient> tempMon;
  int theIndex;
  int theDegree;
  Accum.makeConstant(this->Coefficient, inputOwner);
  for (int i = 0; i < this->generatorsIndices.size; i ++) {
    if (this->powers[i].isSmallInteger(&theDegree)) {
      tempMon.assignElementLieAlgebra(
        this->owner->theOrder[this->generatorsIndices[i]],
        inputOwner,
        this->Coefficient.GetOne(),
        this->Coefficient.GetZero()
      );
      tempMon.raiseToPower(theDegree);
      Accum *= tempMon;
    } else {
      if (this->owner->theOrder[this->generatorsIndices[i]].isCoefficientOneChevalleyGenerator()) {
        tempMon.makeOneGeneratorCoefficientOne(theIndex, inputOwner, this->Coefficient.GetOne(), this->Coefficient.GetZero());
        tempMon[0].powers[0] = this->powers[i];
        Accum *= tempMon;
      } else {
        return false;
      }
    }
  }
  output.operator=(Accum);
  return true;
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::setNumberOfVariables(int newNumVars) {
  this->Coefficient.setNumberOfVariablesSubstituteDeletedByOne(newNumVars);
  for (int i = 0; i < this->generatorsIndices.size; i ++) {
    this->powers[i].setNumberOfVariablesSubstituteDeletedByOne(newNumVars);
  }
}

template <class Coefficient>
bool ElementUniversalEnvelopingOrdered<Coefficient>::getCoordinatesInBasis(
  List<ElementUniversalEnvelopingOrdered<Coefficient> >& theBasis,
  Vector<Coefficient>& output,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) const {
  List<ElementUniversalEnvelopingOrdered<Coefficient> > tempBasis, tempElts;
  tempBasis = theBasis;
  tempBasis.addOnTop(*this);
  Vectors<Coefficient> tempCoords;
  this->getBasisFromSpanOfElements(tempBasis, tempCoords, tempElts, ringUnit, ringZero, global);
  Vector<Coefficient> tempRoot;
  tempRoot = *tempCoords.lastObject();
  tempCoords.setSize(theBasis.size);
  return tempRoot.getCoordinatesInBasis(tempCoords, output, ringUnit, ringZero);
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::getCoordinateFormOfSpanOfElements(
  List<ElementUniversalEnvelopingOrdered>& theElements,
  Vectors<Polynomial<Coefficient> >& outputCoordinates,
  ElementUniversalEnvelopingOrdered& outputCorrespondingMonomials
) {
  if (theElements.size == 0) {
    return;
  }
  outputCorrespondingMonomials.makeZero(*theElements[0].owner);
  for (int i = 0; i < theElements.size; i ++) {
    for (int j = 0; j < theElements[i].size; j ++) {
      outputCorrespondingMonomials.addOnTopNoRepetition(theElements[i][j]);
    }
  }
  outputCoordinates.setSize(theElements.size);
  Polynomial<Rational> ZeroPoly;
  ZeroPoly.makeZero();
  for (int i = 0; i < theElements.size; i ++) {
    Vector<Polynomial<Coefficient> >& current = outputCoordinates[i];
    current.initializeFillInObject(outputCorrespondingMonomials.size, ZeroPoly);
    ElementUniversalEnvelopingOrdered& currentElt = theElements[i];
    for (int j = 0; j < currentElt.size; j ++) {
      MonomialUniversalEnvelopingOrdered<Coefficient>& currentMon = currentElt[j];
      current.objects[outputCorrespondingMonomials.getIndex(currentMon)] = currentMon.theCoefficient;
    }
  }
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::substitutionCoefficients(PolynomialSubstitution<Rational>& theSub) {
  if (theSub.size < 1)
    return;
  this->Coefficient.substitution(theSub);
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::substitutionCoefficients(PolynomialSubstitution<Rational>& theSub) {
  ElementUniversalEnvelopingOrdered<Coefficient> endResult;
  MonomialUniversalEnvelopingOrdered<Coefficient> currentMon;
  endResult.makeZero(*this->owner);
  for (int i = 0; i < this->size; i ++) {
    currentMon = (*this)[i];
    currentMon.substitutionCoefficients(theSub);
    endResult.addMonomial(currentMon);
  }
//  endResult.simplify(theContext);
  this->operator=(endResult);
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::makeZero(SemisimpleLieAlgebra& inputOwner) {
  this->::LinearCombination<MonomialUniversalEnveloping<Coefficient>, Coefficient>::makeZero();
  this->owner = &inputOwner;
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::multiplyBy(
  const MonomialUniversalEnveloping<Coefficient>& standsOnTheRight, const Coefficient& theCoeff
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
    newCoeff *= theCoeff;
    output.addMonomial(tempMon, newCoeff);
  }
  *this = output;
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::operator*=(const ElementUniversalEnveloping& standsOnTheRight) {
  this->::ElementMonomialAlgebra<MonomialUniversalEnveloping<Coefficient>, Coefficient>
  ::operator*=(static_cast<ElementMonomialAlgebra<MonomialUniversalEnveloping<Coefficient>, Coefficient> >(standsOnTheRight));
}

template <class Coefficient>
void ElementUniversalEnveloping<Coefficient>::raiseToPower(int thePower) {
  ElementUniversalEnveloping<Coefficient> buffer;
  buffer = *this;
  if (this->size() == 0) {
    return;
  }
  this->makeConstant(this->coefficients[0].one(), *this->owner);
  for (int i = 0; i < thePower; i ++) {
    this->operator*=(buffer);
  }
}

template<class Coefficient>
void ElementVermaModuleOrdered<Coefficient>::getBasisFromSpanOfElements(
  List<ElementVermaModuleOrdered>& theElements,
  Vectors<RationalFunction<Rational> >& outputCoordinates,
  List<ElementVermaModuleOrdered>& outputTheBasis,
  const RationalFunction<Rational>& RFOne,
  const RationalFunction<Rational>& RFZero
) {
  List<ElementUniversalEnvelopingOrdered<Coefficient> > theEltsUEform;
  theEltsUEform.setSize(theElements.size);
  for (int i = 0; i < theElements.size; i ++) {
    theEltsUEform[i] = theElements[i].theElT;
  }
  List<ElementUniversalEnvelopingOrdered<Coefficient> > theBasisUEform;
  ElementUniversalEnvelopingOrdered<Coefficient>::getBasisFromSpanOfElements(
    theEltsUEform, outputCoordinates, theBasisUEform, RFOne, RFZero, global
  );
  outputTheBasis.setSize(theBasisUEform.size);
  for (int i = 0; i < theBasisUEform.size; i ++) {
    outputTheBasis[i].theElT = theBasisUEform[i];
    outputTheBasis[i].theSubNthElementIsImageNthCoordSimpleBasis = theElements[0].theSubNthElementIsImageNthCoordSimpleBasis;
  }
}

template<class Coefficient>
bool ElementVermaModuleOrdered<Coefficient>::getCoordinatesInBasis(
  const List<ElementVermaModuleOrdered<Coefficient> >& theBasis,
  Vector<Coefficient>& output,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) const {
  List<ElementUniversalEnvelopingOrdered<Coefficient> > theEltsUEform;
  theEltsUEform.setSize(theBasis.size);
  for (int i = 0; i < theBasis.size; i ++) {
    theEltsUEform.objects[i] = theBasis.objects[i].theElT;
  }
  return this->theElT.getCoordinatesInBasis(theEltsUEform, output, ringUnit, ringZero);
}

template<class Coefficient>
bool ElementVermaModuleOrdered<Coefficient>::needsParenthesisForMultiplication() const {
  return this->theElT.needsParenthesisForMultiplication();
}

template <class Coefficient>
void ElementVermaModuleOrdered<Coefficient>::assignElementUniversalEnvelopingOrderedTimesHighestWeightVector(
  ElementUniversalEnvelopingOrdered<Coefficient>& input,
  const ElementVermaModuleOrdered<Coefficient>& ringZero,
  const Coefficient& ringUnit
) {
  this->theElT.operator=(input);
  if (ringZero.theSubNthElementIsImageNthCoordSimpleBasis.size != 3) {
    global.fatal << "ringZero.theSubNthElementIsImageNthCoordSimpleBasis.size is not equal to 3 as expected. " << global.fatal;
  }
  this->theSubNthElementIsImageNthCoordSimpleBasis = ringZero.theSubNthElementIsImageNthCoordSimpleBasis;
  this->theElT.modOutVermaRelationsOld(false, this->theSubNthElementIsImageNthCoordSimpleBasis, ringUnit);
}

template <class Coefficient>
void ElementVermaModuleOrdered<Coefficient>::multiplyOnTheLeft(
  const ElementSemisimpleLieAlgebra<Rational>& other,
  ElementVermaModuleOrdered<Coefficient>& output,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  ElementUniversalEnvelopingOrdered<Coefficient> tempElt;
  tempElt.assignElementLieAlgebra(other, ringUnit, ringZero, *this->theElT.owner);
  tempElt *= this->theElT;
  output.theElT = tempElt;
  output.theElT.simplify(ringUnit, ringZero);
  output.theElT.modOutVermaRelationsOld(false, this->theSubNthElementIsImageNthCoordSimpleBasis, ringUnit);
}

template <class Coefficient>
std::string ElementVermaModuleOrdered<Coefficient>::toString(const FormatExpressions& theFormat) const {
  std::stringstream out;
  std::string tempS = MathRoutines::toStringBrackets(this->theElT, theFormat);
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

template<class Coefficient>
void ElementVermaModuleOrdered<Coefficient>::actOnMe(
  const ElementSemisimpleLieAlgebra<Rational>& actingElt,
  ElementVermaModuleOrdered<Coefficient>& output,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) const {
  ElementUniversalEnvelopingOrdered<Coefficient> tempElt;
  tempElt.assignElementLieAlgebra(actingElt, ringUnit, ringZero, *this->theElT.owner);
  tempElt.lieBracketOnTheRight(this->theElT, output.theElT);
  output.theElT.simplify(ringUnit, ringZero);
  output.theSubNthElementIsImageNthCoordSimpleBasis = this->theSubNthElementIsImageNthCoordSimpleBasis;
  output.theElT.modOutVermaRelationsOld(false, this->theSubNthElementIsImageNthCoordSimpleBasis, ringUnit);
}

template<class Coefficient>
template<class CoefficientTypeQuotientField>
void ElementUniversalEnvelopingOrdered<Coefficient>::getBasisFromSpanOfElements(
  List<ElementUniversalEnvelopingOrdered<Coefficient> >& theElements,
  Vectors<CoefficientTypeQuotientField>& outputCoords,
  List<ElementUniversalEnvelopingOrdered<Coefficient> >& outputTheBasis,
  const CoefficientTypeQuotientField& theFieldUnit,
  const CoefficientTypeQuotientField& theFieldZero
) {
  if (theElements.size == 0) {
    return;
  }
  ElementUniversalEnvelopingOrdered<Coefficient> outputCorrespondingMonomials;
  outputCorrespondingMonomials.makeZero(*theElements[0].owner);
  Vectors<CoefficientTypeQuotientField> outputCoordsBeforeReduction;
  for (int i = 0; i < theElements.size; i ++) {
    for (int j = 0; j < theElements[i].size; j ++) {
      outputCorrespondingMonomials.addOnTopNoRepetition(theElements[i][j]);
    }
  }
  outputCoordsBeforeReduction.setSize(theElements.size);
  for (int i = 0; i < theElements.size; i ++) {
    Vector<CoefficientTypeQuotientField>& currentList = outputCoordsBeforeReduction[i];
    currentList.makeZero(outputCorrespondingMonomials.size, theFieldZero);
    ElementUniversalEnvelopingOrdered<Coefficient>& currentElt = theElements[i];
    for (int j = 0; j < currentElt.size; j ++) {
      MonomialUniversalEnvelopingOrdered<Coefficient>& currentMon = currentElt[j];
      currentList.objects[outputCorrespondingMonomials.getIndex(currentMon)] = currentMon.theCoefficient;
    }
  }
  outputTheBasis.size = 0;
  outputTheBasis.reserve(theElements.size);
  Vectors<CoefficientTypeQuotientField> basisCoordForm;
  basisCoordForm.reserve(theElements.size);
  Selection selectedBasis;
  outputCoordsBeforeReduction.ComputeDebugString();
  outputCoordsBeforeReduction.SelectABasis(basisCoordForm, theFieldZero, selectedBasis);
  for (int i = 0; i < selectedBasis.cardinalitySelection; i ++) {
    outputTheBasis.addOnTop(theElements[selectedBasis.elements[i]]);
  }
  Matrix<Coefficient> bufferMat;
  Vectors<Coefficient> bufferVectors;
  outputCoordsBeforeReduction.getCoordinatesInBasis(basisCoordForm, outputCoords, bufferVectors, bufferMat, theFieldUnit, theFieldZero);
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::simplify(
  const Coefficient& ringUnit,
  const Coefficient& ringZero
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
void ElementUniversalEnvelopingOrdered<Coefficient>::makeZero(SemisimpleLieAlgebraOrdered& theOwner) {
  this->clear();
  this->owner = &theOwner;
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
    if (this->objects[i].theCoefficient.isEqualToZero()) {
      this->removeIndexSwapWithLast(i);
      i --;
    }
  }
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::operator+=(const ElementUniversalEnvelopingOrdered<Coefficient>& other) {
  this->reserve(this->size + other.size);
  for (int i = 0; i < other.size; i ++)
    this->addMonomialNoCleanUpZeroCoefficient(other[i]);
  this->cleanUpZeroCoefficient();
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::operator-=(const ElementUniversalEnvelopingOrdered<Coefficient>& other) {
  this->reserve(this->size +other.size);
  MonomialUniversalEnvelopingOrdered<Coefficient> tempMon;
  for (int i = 0; i < other.size; i ++) {
    tempMon = other.objects[i];
    tempMon.theCoefficient *= - 1;
    this->addMonomialNoCleanUpZeroCoefficient(tempMon);
  }
  this->cleanUpZeroCoefficient();
}

template <class Coefficient>
template <class otherType>
void ElementUniversalEnvelopingOrdered<Coefficient>::operator*=(const otherType& other) {
  if (other.isEqualToZero()) {
    this->makeZero(*this->owner);
    return;
  }
  for (int i = 0; i < this->size; i ++) {
    this->objects[i].theCoefficient.operator*=(other);
  }
}

template <class Coefficient>
template <class SecondType>
void ElementUniversalEnvelopingOrdered<Coefficient>::operator/=(const SecondType& other) {
  for (int i = 0; i < this->size; i ++) {
    this->objects[i].theCoefficient.operator/=(other);
  }
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::multiplyByNoSimplify(const MonomialUniversalEnvelopingOrdered& other) {
  if (this == &other) {
    global.fatal << "Output not allowed to be equal to this object. " << global.fatal;
  }
  this->generatorsIndices.reserve(other.generatorsIndices.size + this->generatorsIndices.size);
  this->powers.reserve(other.generatorsIndices.size + this->generatorsIndices.size);
  this->Coefficient.multiplyBy(other.theCoefficient);
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
  MonomialUniversalEnvelopingOrdered<Coefficient>& theMon = (*this)[0];
  int theIndex = other.getIndex(theMon);
  if (theIndex == - 1) {
    return false;
  }
  MonomialUniversalEnvelopingOrdered<Coefficient>& otherMon = other[theIndex];
  theMon.ComputeDebugString();
  otherMon.ComputeDebugString();
  outputTimesMeEqualsOther = otherMon.theCoefficient;
  outputTimesMeEqualsOther /= theMon.theCoefficient;
  ElementUniversalEnvelopingOrdered<Coefficient> tempElt;

  tempElt = *this;
  tempElt *= outputTimesMeEqualsOther;
  tempElt -= other;
  return tempElt.isEqualToZero();
}

template<class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::operator*=(const MonomialUniversalEnvelopingOrdered& other) {
  if (this == &other) {
    global.fatal << "Output and input are the same. " << global.fatal;
  }
  this->Coefficient *= other.theCoefficient;
  for (int i = 0; i < other.generatorsIndices.size; i ++) {
    this->multiplyByGeneratorPowerOnTheRight(other.generatorsIndices[i], other.powers[i]);
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
void ElementUniversalEnvelopingOrdered<Coefficient>::addMonomialNoCleanUpZeroCoefficient(
  const MonomialUniversalEnvelopingOrdered<Coefficient>& input
) {
  int theIndex = this->getIndex(input);
  if (theIndex == - 1) {
    this->addOnTop(input);
  } else {
    this->objects[theIndex].theCoefficient += input.theCoefficient;
  }
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::simplifyAccumulateInOutputNoOutputInit(
  ElementUniversalEnvelopingOrdered<Coefficient>& output,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  int IndexlowestNonSimplified = 0;
  ElementUniversalEnvelopingOrdered<Coefficient> buffer2;
  MonomialUniversalEnvelopingOrdered<Coefficient> tempMon;
  //simplified order is descending order
  while (IndexlowestNonSimplified < output.size) {
    bool reductionOccurred = false;
    if (output[IndexlowestNonSimplified].theCoefficient.isEqualToZero()) {
      reductionOccurred = true;
    } else {
      for (int i = 0; i < output[IndexlowestNonSimplified].generatorsIndices.size - 1; i ++) {
        if (
          output[IndexlowestNonSimplified].generatorsIndices[i] >
          output[IndexlowestNonSimplified].generatorsIndices[i + 1]
        ) {
          if (output[IndexlowestNonSimplified].switchConsecutiveIndicesIfTheyCommute(
            i, tempMon, ringZero
          )) {
            output.addMonomialNoCleanUpZeroCoefficient(tempMon);
            reductionOccurred = true;
            break;
          }
          if (this->commutingRightIndexAroundLeftIndexAllowed(
            output[IndexlowestNonSimplified].powers[i],
            output[IndexlowestNonSimplified].generatorsIndices[i],
            output[IndexlowestNonSimplified].powers[i + 1],
            output[IndexlowestNonSimplified].generatorsIndices[i + 1]
          )) {
            output[IndexlowestNonSimplified].commuteConsecutiveIndicesRightIndexAroundLeft(
              i, buffer2, ringUnit, ringZero
            );
            for (int j = 0; j < buffer2.size; j ++) {
              output.addMonomialNoCleanUpZeroCoefficient(buffer2[j]);
            }
            reductionOccurred = true;
            break;
          }
          if (this->commutingLeftIndexAroundRightIndexAllowed(
            output[IndexlowestNonSimplified].powers[i],
            output[IndexlowestNonSimplified].generatorsIndices[i],
            output[IndexlowestNonSimplified].powers[i + 1],
            output[IndexlowestNonSimplified].generatorsIndices[i + 1]
          )) {
            output[IndexlowestNonSimplified].commuteConsecutiveIndicesLeftIndexAroundRight(
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
      output.removeIndexSwapWithLast(IndexlowestNonSimplified);
    } else {
      IndexlowestNonSimplified ++;
    }
  }
  output.cleanUpZeroCoefficient();
}

template <class Coefficient>
bool MonomialUniversalEnvelopingOrdered<Coefficient>::switchConsecutiveIndicesIfTheyCommute(
  int theLeftIndex,
  MonomialUniversalEnvelopingOrdered<Coefficient>& output,
  const Coefficient& ringZero
) {
  if (theLeftIndex >= this->generatorsIndices.size - 1) {
    return false;
  }
  int theLeftGeneratorIndex = this->generatorsIndices[theLeftIndex];
  int theRightGeneratorIndex = this->generatorsIndices[theLeftIndex + 1];
  ElementSemisimpleLieAlgebra<Rational> tempElt;
  this->owner->theOwner->lieBracket(
    this->owner->theOrder[theLeftGeneratorIndex], this->owner->theOrder[theRightGeneratorIndex], tempElt
  );
  if (tempElt.isEqualToZero()) {
    output.generatorsIndices.reserve(this->generatorsIndices.size);
    output.powers.reserve(this->generatorsIndices.size);
    output.makeZero(ringZero, *this->owner);
    output.theCoefficient = this->Coefficient;
    //output.ComputeDebugString();
    for (int i = 0; i < theLeftIndex; i ++) {
      output.multiplyByGeneratorPowerOnTheRight(this->generatorsIndices[i], this->powers[i]);
    }
    output.multiplyByGeneratorPowerOnTheRight(this->generatorsIndices[theLeftIndex + 1], this->powers[theLeftIndex + 1]);
    output.multiplyByGeneratorPowerOnTheRight(this->generatorsIndices[theLeftIndex], this->powers[theLeftIndex]);
    for (int i = theLeftIndex + 2; i < this->generatorsIndices.size; i ++) {
      output.multiplyByGeneratorPowerOnTheRight(this->generatorsIndices[i], this->powers[i]);
    }
    return true;
  }
  return false;
}

template <class Coefficient>
bool MonomialUniversalEnvelopingOrdered<Coefficient>::commutingRightIndexAroundLeftIndexAllowed(
  Coefficient& theLeftPower, int leftGeneratorIndex, Coefficient& theRightPower, int rightGeneratorIndex
) {
  return this->commutingLeftIndexAroundRightIndexAllowed(theRightPower, rightGeneratorIndex, theLeftPower, leftGeneratorIndex);
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::commuteConsecutiveIndicesRightIndexAroundLeft(
  int theIndeX,
  ElementUniversalEnvelopingOrdered<Coefficient>& output,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  if (theIndeX == this->generatorsIndices.size - 1) {
    return;
  }
  output.makeZero(*this->owner);
  MonomialUniversalEnvelopingOrdered tempMon;
  tempMon.makeZero(ringZero, *this->owner);
  tempMon.powers.reserve(this->generatorsIndices.size + 2);
  tempMon.generatorsIndices.reserve(this->generatorsIndices.size + 2);
  tempMon.powers.size = 0;
  tempMon.generatorsIndices.size = 0;
  int rightGeneratorIndeX = this->generatorsIndices.objects[theIndeX + 1];
  int leftGeneratorIndeX = this->generatorsIndices.objects[theIndeX];
  Coefficient theRightPoweR, theLeftPoweR;
  theRightPoweR = this->powers.objects[theIndeX + 1];
  theLeftPoweR = this->powers.objects[theIndeX];
  theRightPoweR -= 1;
  int powerDroP = 0;
//  if (this->flagAnErrorHasOccurredTimeToPanic)
//  if (this->toString() == "2f_{5}f_{-5}f_{-4}" || this->toString() == "2f_{11}f_{-4}")
//  { this->flagAnErrorHasOccurredTimeToPanic = true;
//  }

  Coefficient acquiredCoefficienT;
  acquiredCoefficienT = this->Coefficient;
  tempMon.theCoefficient = this->Coefficient;
  for (int i = 0; i < theIndeX; i ++) {
    tempMon.multiplyByGeneratorPowerOnTheRight(this->generatorsIndices[i], this->powers[i]);
  }
  MonomialUniversalEnvelopingOrdered startMon;
  startMon = tempMon;
  ElementSemisimpleLieAlgebra<Rational> adResulT, tempElT, theLeftElt;
  this->owner->assignGeneratorCoefficientOne(rightGeneratorIndeX, adResulT);
  this->owner->assignGeneratorCoefficientOne(leftGeneratorIndeX, theLeftElt);
  //tempLefttElt.ComputeDebugString(*this->owner, false, false);
  Vector<Rational> theCoeffs;
  do {
    this->owner->getLinearCombinationFrom(adResulT, theCoeffs);
    for (int i = 0; i < theCoeffs.size; i ++) {
      if (theCoeffs[i] != 0) {
        int theNewGeneratorIndex = i;
        tempMon = startMon;
        if (this->flagAnErrorHasOccurredTimeToPanic) {
          tempMon.ComputeDebugString();
          this->ComputeDebugString();
        }
        tempMon.theCoefficient = acquiredCoefficienT;
        if (this->flagAnErrorHasOccurredTimeToPanic) {
          tempMon.ComputeDebugString();
          this->ComputeDebugString();
        }
        tempMon.theCoefficient *= theCoeffs[i];
        if (this->flagAnErrorHasOccurredTimeToPanic) {
          tempMon.ComputeDebugString();
          this->ComputeDebugString();
        }
        tempMon.multiplyByGeneratorPowerOnTheRight(theNewGeneratorIndex, ringUnit);
        tempMon.multiplyByGeneratorPowerOnTheRight(leftGeneratorIndeX, theLeftPoweR);
        tempMon.multiplyByGeneratorPowerOnTheRight(rightGeneratorIndeX, theRightPoweR);
        for (int i = theIndeX + 2; i < this->generatorsIndices.size; i ++) {
          tempMon.multiplyByGeneratorPowerOnTheRight(this->generatorsIndices.objects[i], this->powers.objects[i]);
        }
        if (this->flagAnErrorHasOccurredTimeToPanic) {
          tempMon.ComputeDebugString();
          this->ComputeDebugString();
        }
        output.addOnTop(tempMon);
      }
    }
    acquiredCoefficienT *= theLeftPoweR;
    theLeftPoweR -= 1;
    this->owner->theOwner->lieBracket(theLeftElt, adResulT, tempElT);
    adResulT = tempElT;
    powerDroP ++;
    acquiredCoefficienT /= powerDroP;
  } while (!adResulT.isEqualToZero() && !acquiredCoefficienT.isEqualToZero());
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::commuteConsecutiveIndicesLeftIndexAroundRight(
  int theIndeX,
  ElementUniversalEnvelopingOrdered<Coefficient>& output,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  if (theIndeX == this->generatorsIndices.size - 1) {
    return;
  }
  output.makeZero(*this->owner);
  MonomialUniversalEnvelopingOrdered tempMon;
  tempMon.makeZero(ringZero, *this->owner);
  tempMon.powers.setExpectedSize(this->generatorsIndices.size + 2);
  tempMon.generatorsIndices.setExpectedSize(this->generatorsIndices.size + 2);
  tempMon.powers.size = 0;
  tempMon.generatorsIndices.size = 0;
  int rightGeneratorIndex = this->generatorsIndices.objects[theIndeX + 1];
  int leftGeneratorIndex = this->generatorsIndices.objects[theIndeX];
  Coefficient theRightPower, theLeftPower;
  theRightPower = this->powers.objects[theIndeX + 1];
  theLeftPower = this->powers.objects[theIndeX];
  theLeftPower -= 1;
  int powerDrop = 0;

  Coefficient acquiredCoefficient;
  acquiredCoefficient = this->Coefficient;
  tempMon.theCoefficient = this->Coefficient;
  for (int i = 0; i < theIndeX; i ++) {
    tempMon.multiplyByGeneratorPowerOnTheRight(this->generatorsIndices.objects[i], this->powers.objects[i]);
  }
  tempMon.multiplyByGeneratorPowerOnTheRight(this->generatorsIndices.objects[theIndeX], theLeftPower);
  MonomialUniversalEnvelopingOrdered startMon, tempMon2;
  startMon = tempMon;
  ElementSemisimpleLieAlgebra<Rational> adResult, tempElt, tempRightElt;
  this->owner->assignGeneratorCoefficientOne(leftGeneratorIndex, adResult);
  this->owner->assignGeneratorCoefficientOne(rightGeneratorIndex, tempRightElt);
//  tempRightElt.ComputeDebugString(*this->owner, false, false);
  Vector<Rational> theCoeffs;
  do {
    this->owner->getLinearCombinationFrom(adResult, theCoeffs);
    for (int i = 0; i < theCoeffs.size; i ++) {
      if (theCoeffs[i] != 0) {
        int theNewGeneratorIndex = i;
        tempMon = startMon;
        tempMon.multiplyByGeneratorPowerOnTheRight(rightGeneratorIndex, theRightPower);
        tempMon.theCoefficient = acquiredCoefficient;
        tempMon.theCoefficient *= theCoeffs[i];
        tempMon.multiplyByGeneratorPowerOnTheRight(theNewGeneratorIndex, ringUnit);
        for (int i = theIndeX + 2; i < this->generatorsIndices.size; i ++) {
          tempMon.multiplyByGeneratorPowerOnTheRight(this->generatorsIndices[i], this->powers[i]);
        }
        output.addOnTop(tempMon);
      }
    }
    acquiredCoefficient *= theRightPower;
    theRightPower -= 1;
    this->owner->theOwner->lieBracket(adResult, tempRightElt, tempElt);
    adResult = tempElt;
    powerDrop ++;
    acquiredCoefficient /= powerDrop;
    //adResult.ComputeDebugString(*this->owner, false, false);
  } while (!adResult.isEqualToZero() && !acquiredCoefficient.isEqualToZero());
}

template <class Coefficient>
bool MonomialUniversalEnvelopingOrdered<Coefficient>::commutingLeftIndexAroundRightIndexAllowed(
  Coefficient& theLeftPower, int leftGeneratorIndex, Coefficient& theRightPower, int rightGeneratorIndex
) {
  int tempInt;
  if (theLeftPower.isSmallInteger(&tempInt)) {
    if (theRightPower.isSmallInteger(&tempInt)) {
      return true;
    }
    int numPosRoots = this->owner->theOwner->theWeyl.rootsOfBorel.size;
    int theDimension = this->owner->theOwner->theWeyl.cartanSymmetric.numberOfRows;
    if (rightGeneratorIndex >= numPosRoots && rightGeneratorIndex < numPosRoots + theDimension) {
      ElementSemisimpleLieAlgebra<Rational> tempElt;
      this->owner->theOwner->lieBracket(
        this->owner->theOrder[leftGeneratorIndex], this->owner->theOrder[rightGeneratorIndex], tempElt
      );
      if (tempElt.isEqualToZero()) {
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
void MonomialUniversalEnvelopingOrdered<Coefficient>::makeZero(int numVars, SemisimpleLieAlgebraOrdered& theOwner) {
  this->Coefficient.makeZero(numVars);
  this->owner = &theOwner;
  this->generatorsIndices.size = 0;
  this->powers.size = 0;
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::makeZero(
  const Coefficient& ringZero, SemisimpleLieAlgebraOrdered& theOwner
) {
  this->theCoefficient = ringZero;
  this->owner = &theOwner;
  this->generatorsIndices.size = 0;
  this->powers.size = 0;
}

template <class Coefficient>
unsigned int MonomialUniversalEnvelopingOrdered<Coefficient>::hashFunction() const {
  int top = MathRoutines::minimum(someRandomPrimesSize, this->generatorsIndices.size);
  unsigned int result = 0;
  for (int i = 0; i < top; i ++) {
    result += someRandomPrimes[i] * this->generatorsIndices.objects[i];
  }
  return result;
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::multiplyByGeneratorPowerOnTheRight(int theGeneratorIndex, int thePower) {
  if (thePower == 0) {
    return;
  }
  Polynomial<Rational> tempP;
  tempP.makeConstant(thePower);
  this->multiplyByGeneratorPowerOnTheRight(theGeneratorIndex, tempP);
}

template <class Coefficient>
void MonomialUniversalEnvelopingOrdered<Coefficient>::multiplyByGeneratorPowerOnTheRight(
  int theGeneratorIndex, const Coefficient& thePower
) {
  if (thePower.isEqualToZero()) {
    return;
  }
  if (this->generatorsIndices.size > 0) {
    if (*this->generatorsIndices.lastObject() == theGeneratorIndex) {
      (*this->powers.lastObject()) += thePower;
      return;
    }
  }
  this->powers.addOnTop(thePower);
  this->generatorsIndices.addOnTop(theGeneratorIndex);
}

template <class Coefficient>
std::string MonomialUniversalEnvelopingOrdered<Coefficient>::toString(
  FormatExpressions* PolyFormatLocal
) const {

  if (this->owner == nullptr) {
    return "faulty monomial non-initialized owner. Slap the programmer.";
  }
  if (this->isEqualToZero()) {
    return "0";
  }
  std::stringstream out;
  std::string tempS;
  if (this->generatorsIndices.size > 0) {
    tempS = MathRoutines::toStringBrackets(this->Coefficient, PolyFormatLocal);
    if (tempS == "1") {
      tempS = "";
    }
    if (tempS == "- 1" || tempS == "-1") {
      tempS = "-";
    }
  } else {
    tempS = this->Coefficient.toString(PolyFormatLocal);
  }
  out << tempS;
  for (int i = 0; i < this->generatorsIndices.size; i ++) {
    Coefficient& thePower = this->powers[i];
    int theIndex = this->generatorsIndices[i];
    bool usebrackets = false;
    tempS = this->owner->theOwner->getStringFromChevalleyGenerator(theIndex, PolyFormatLocal);
    if (usebrackets) {
      out << "(";
    }
    out << tempS;
    if (usebrackets) {
      out << ")";
    }
    tempS = thePower.toString(PolyFormatLocal);
    if (tempS != "1") {
      out << "^";
     // if (useLatex)
      out << "{";
      out << tempS;
      //if (useLatex)
      out << "}";
    }
    //if (thePower>1)
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
    MonomialUniversalEnvelopingOrdered<Coefficient>& current = this->objects[i];
    tempS = current.toString(PolyFormatLocal);
    if (i != 0) {
      if (tempS.size() > 0) {
        if (tempS[0] != '-') {
          out << '+';
        }
      }
    }
    out << tempS;
    if ((static_cast<int>(out.tellp())) - indexCharacterAtLastLineBreak > 150) {
      indexCharacterAtLastLineBreak = out.tellp();
      out << "\\\\&&";
    }
  }
  output = out.str();
}

template <class Coefficient>
bool ElementUniversalEnvelopingOrdered<Coefficient>::assignElementUniversalEnveloping(
  ElementUniversalEnveloping<Coefficient>& input,
  SemisimpleLieAlgebraOrdered& owner,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  ElementUniversalEnvelopingOrdered<Coefficient> tempElt;
  this->makeZero(owner);
  for (int i = 0; i < input.size; i ++) {
    if (!tempElt.assignMonomialUniversalEnveloping(
      input.objects[i], input.theCoefficients[i], owner, ringUnit, ringZero
    )) {
      return false;
    }
    this->operator+=(tempElt);
  }
  this->simplify(ringUnit, ringZero);
  return true;
}

template <class Coefficient>
bool ElementUniversalEnvelopingOrdered<Coefficient>::assignMonomialUniversalEnveloping(
  MonomialUniversalEnveloping<Coefficient>& input,
  const Coefficient& inputCoeff,
  SemisimpleLieAlgebraOrdered& owner,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  ElementUniversalEnvelopingOrdered theMon;
  ElementSemisimpleLieAlgebra<Rational> tempElt;
  Coefficient theCoeff;
  theCoeff = inputCoeff;
  this->makeConstant(theCoeff, owner);
  for (int i = 0; i < input.generatorsIndices.size; i ++) {
    int thePower;
    bool isASmallInt = input.powers.objects[i].isSmallInteger(&thePower);
    if (isASmallInt) {
      tempElt.makeGenerator(
        input.generatorsIndices.objects[i], *input.owners, input.indexInOwners
      );
      theMon.assignElementLieAlgebra(tempElt, ringUnit, ringZero, owner);
      theMon.raiseToPower(thePower, ringUnit);
    } else {
      return false;
    }
    this->multiplyBy(theMon);
  }
  return true;
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::assignElementLieAlgebra(
  const ElementSemisimpleLieAlgebra<Rational>& input,
  const Coefficient& ringUnit,
  const Coefficient& ringZero,
  SemisimpleLieAlgebraOrdered& theOwner
) {
  this->makeZero(theOwner);
  Vector<Rational> elementRootForm;
  input.elementToVectorNegativeRootSpacesFirst(elementRootForm);
  theOwner.chevalleyGeneratorsInCurrentCoordinates.actOnVectorColumn(elementRootForm);
  MonomialUniversalEnvelopingOrdered<Coefficient> tempMon;
  tempMon.makeZero(ringZero, theOwner);
  tempMon.generatorsIndices.setSize(1);
  tempMon.powers.setSize(1);
  tempMon.powers.objects[0] = ringUnit;
  for (int theIndex = 0; theIndex < elementRootForm.size; theIndex ++) {
    if (elementRootForm.objects[theIndex] != 0) {
      tempMon.theCoefficient = ringUnit;
      tempMon.theCoefficient *= elementRootForm.objects[theIndex];
      tempMon.generatorsIndices[0] = theIndex;
      this->addOnTop(tempMon);
    }
  }
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::raiseToPower(int thePower, const Coefficient& ringUnit) {
  if (this->size == 0) {
    return;
  }
  ElementUniversalEnvelopingOrdered<Coefficient> buffer;
  buffer.operator=(*this);
  this->makeConstant(ringUnit, *this->owner);
  for (int i = 0; i < thePower; i ++) {
    this->operator*=(buffer);
  }
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::lieBracketOnTheRight(
  const ElementSemisimpleLieAlgebra<Rational>& right,
  const Coefficient& ringUnit,
  const Coefficient& ringZero
) {
  ElementUniversalEnvelopingOrdered<Coefficient> tempElt;
  tempElt.assignElementLieAlgebra(right, ringUnit, ringZero, *this->owner);
  this->lieBracketOnTheRight(tempElt, *this);
}

template <class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::lieBracketOnTheRight(
  const ElementUniversalEnvelopingOrdered& right, ElementUniversalEnvelopingOrdered& output
) {
  ElementUniversalEnvelopingOrdered tempElt, tempElt2;
  tempElt = *this;
  tempElt *= right;
  tempElt2 = right;
  tempElt2 *= *this;
  output = tempElt;
  output -= tempElt2;
}

template<class Coefficient>
void ElementUniversalEnvelopingOrdered<Coefficient>::addMonomial(
  const MonomialUniversalEnvelopingOrdered<Coefficient>& input
) {
  if (input.isEqualToZero()) {
    return;
  }
  int theIndex = this->getIndex(input);
  if (theIndex == - 1) {
    this->addOnTop(input);
  } else {
    this->objects[theIndex].theCoefficient += input.theCoefficient;
    if (this->objects[theIndex].theCoefficient.isEqualToZero()) {
      this->removeIndexSwapWithLast(theIndex);
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
    tempMon.modOutVermaRelationsOld(substitutionHighestWeightWithZeroes, highestWeightSub, ringUnit);
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
    tempMon.modOutVermaRelations(substitutionHiGoesToIthElement, ringUnit, ringZero);
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
  int numPosRoots = this->owner->theOwner->getNumberOfPositiveRoots();
  int theDimension = this->owner->theOwner->getRank();
  for (int i = this->generatorsIndices.size - 1; i >= 0; i --) {
    int indexCurrentGenerator = this->generatorsIndices[i];
    if (indexCurrentGenerator >= numPosRoots + theDimension) {
      this->makeZero(ringZero, *this->owner);
      return;
    }
    if (indexCurrentGenerator < numPosRoots) {
      return;
    }
    if (indexCurrentGenerator >= numPosRoots &&  indexCurrentGenerator < numPosRoots + theDimension) {
      int theDegree;
      if (!this->powers[i].isSmallInteger(theDegree)) {
        return;
      }
      if (substitutionHiGoesToIthElement == 0) {
        this->makeZero(ringZero, *this->owner);
        return;
      }
      Coefficient theSubbedH = ringZero;
      Vector<Rational> currentH = this->owner->theOrder[indexCurrentGenerator].getCartanPart();
      for (int j = 0; j < currentH.size; j ++) {
        theSubbedH += (*substitutionHiGoesToIthElement)[j] * currentH[j];
      }
      MathRoutines::raiseToPower(theSubbedH, theDegree, ringUnit);
      this->theCoefficient *= theSubbedH;
      this->generatorsIndices.size --;
      this->powers.size --;
    }
  }
}
#endif
