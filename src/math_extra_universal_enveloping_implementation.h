// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfImplementationHeader1_HeaderUniversalEnveloping
#define vpfImplementationHeader1_HeaderUniversalEnveloping
#include "math_extra_universal_enveloping.h"

template <class coefficient>
coefficient SemisimpleLieAlgebra::GetKillingFormProductWRTLevi(
  const ElementSemisimpleLieAlgebra<coefficient>& left,
  const ElementSemisimpleLieAlgebra<coefficient>& right,
  const Selection& rootsNotInLevi
) {
  MacroRegisterFunctionWithName("SemisimpleLieAlgebra::GetKillingFormWRTLevi");
  coefficient result = 0;
  ElementSemisimpleLieAlgebra<coefficient> adadAppliedToMon, tempElt;
  ChevalleyGenerator baseGen;
  Vector<Rational> rootsNotInLeviVectorForm = rootsNotInLevi;
  Vector<Rational> theWeight;
  for (int i = 0; i < this->GetNumGenerators(); i ++) {
    theWeight = this->GetWeightOfGenerator(i);
    if (theWeight.ScalarEuclidean(rootsNotInLeviVectorForm) != 0) {
      continue;
    }
    baseGen.MakeGenerator(*this, i);
    adadAppliedToMon.makeZero();
    adadAppliedToMon.AddMonomial(baseGen, 1);
    this->LieBracket(right, adadAppliedToMon, tempElt);
    this->LieBracket(left, tempElt, adadAppliedToMon);
    result += adadAppliedToMon.GetMonomialCoefficient(baseGen);
  }
  return result;
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::MakeCasimirWRTLeviParabolic(
  SemisimpleLieAlgebra& theOwner, const Selection& theLeviRoots
) {
  MacroRegisterFunctionWithName("ElementUniversalEnveloping::MakeCasimirWRTLeviParabolic");
  if (theLeviRoots.CardinalitySelection == 0) {
    this->makeZero(theOwner);
    return;
  }
  coefficient result = 0;
  ElementSemisimpleLieAlgebra<Rational> leftE, rightE;
  ChevalleyGenerator baseGen;
  Selection rootsNotInLEvi = theLeviRoots;
  rootsNotInLEvi.InvertSelection();
  Vector<Rational> rootsNotInLeviVectorForm = rootsNotInLEvi;
  Vector<Rational> theWeightLeft, theWeightRight;
  this->makeZero(theOwner);
  MonomialUniversalEnveloping<coefficient> theMon;
  Rational theCF;
  //coefficient theCFconverted;
  for (int i = 0; i < theOwner.GetNumGenerators(); i ++) {
    theWeightLeft = theOwner.GetWeightOfGenerator(i);
    if (theWeightLeft.ScalarEuclidean(rootsNotInLeviVectorForm) != 0) {
      continue;
    }
    if (theWeightLeft.isEqualToZero()) {
      continue;
    }
    theMon.makeOne(theOwner);
    int indexOpposite = theOwner.GetGeneratorFromRoot(- theWeightLeft);
    theMon.generatorsIndices.addOnTop(i);
    theMon.generatorsIndices.addOnTop(indexOpposite);
    theMon.Powers.addOnTop(1);
    theMon.Powers.addOnTop(1);
    leftE.MakeGenerator(i, theOwner);
    rightE.MakeGenerator(indexOpposite, theOwner);
    theCF = theOwner.GetKillingFormProductWRTLevi(leftE, rightE, rootsNotInLEvi);
    theCF.invert();
    this->AddMonomial(theMon, theCF);
  }
  Matrix<Rational> killingRestrictedToCartan;
  killingRestrictedToCartan.init(theLeviRoots.CardinalitySelection, theLeviRoots.CardinalitySelection);
  for (int i = 0; i < theLeviRoots.CardinalitySelection; i ++) {
    for (int j = i; j < theLeviRoots.CardinalitySelection; j ++) {
      theWeightLeft.MakeEi(theOwner.GetRank(), theLeviRoots.elements[i]);
      theWeightRight.MakeEi(theOwner.GetRank(), theLeviRoots.elements[j]);
      leftE.MakeHgenerator(theWeightLeft, theOwner);
      rightE.MakeHgenerator(theWeightRight, theOwner);
      killingRestrictedToCartan(i, j) = theOwner.GetKillingFormProductWRTLevi(leftE, rightE, rootsNotInLEvi);
      killingRestrictedToCartan(j, i) = killingRestrictedToCartan(i, j);
    }
  }
  killingRestrictedToCartan.Invert();
  ElementUniversalEnveloping<coefficient> leftUE, rightUE;
  Vector<Rational> currentEj;
  for (int i = 0; i < theLeviRoots.CardinalitySelection; i ++) {
    theWeightLeft.MakeEi(theOwner.GetRank(), theLeviRoots.elements[i]);
    theWeightRight.makeZero(theOwner.GetRank());
    for (int j = 0; j < theLeviRoots.CardinalitySelection; j ++) {
      currentEj.MakeEi(theOwner.GetRank(), theLeviRoots.elements[j]);
      theWeightRight += currentEj * killingRestrictedToCartan(i, j);
    }
    leftUE.MakeHgenerator(theWeightLeft, theOwner);
    rightUE.MakeHgenerator(theWeightRight, theOwner);
    leftUE *= rightUE;
    *this += leftUE;
  }
  this->simplify();
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::ModOutVermaRelations(
  const Vector<coefficient>* subHiGoesToIthElement, const coefficient& theRingUnit, const coefficient& theRingZero
) {
  MonomialUniversalEnveloping<coefficient> tempMon;
  ElementUniversalEnveloping<coefficient> output;
  output.makeZero(*this->owner);
  coefficient acquiredCoeff;
  for (int i = 0; i < this->size(); i ++) {
    tempMon = (*this)[i];
    tempMon.ModOutVermaRelations(acquiredCoeff, subHiGoesToIthElement, theRingUnit, theRingZero);
    acquiredCoeff *= this->coefficients[i];
    output.AddMonomial(tempMon, acquiredCoeff);
  }
  this->operator=(output);
}

template<class coefficient>
void ElementUniversalEnveloping<coefficient>::LieBracketOnTheLeft(const ElementSemisimpleLieAlgebra<Rational>& left) {
  if (this->isEqualToZero()) {
    this->makeZero(*this->owner);
    return;
  }
  ElementUniversalEnveloping<coefficient> tempElt1, tempElt2;
  tempElt1.AssignElementLieAlgebra(left, *this->owner, this->coefficients[0].one());
  tempElt2 = *this;
  tempElt2.LieBracketOnTheRight(tempElt1, *this);
}

template<class coefficient>
bool MonomialUniversalEnveloping<coefficient>::AdjointRepresentationAction(
  const ElementUniversalEnveloping<coefficient>& input, ElementUniversalEnveloping<coefficient>& output
) const {
  output.makeZero(*this->owner);
  ElementSemisimpleLieAlgebra<Rational> tempElt;
  output = input;
  for (int i = this->generatorsIndices.size - 1; i >= 0; i --) {
    int nextCycleSize;
    if (!this->Powers[i].IsSmallInteger(&nextCycleSize)) {
      return false;
    }
    for (int j = 0; j < nextCycleSize; j ++) {
      tempElt.MakeGenerator(this->generatorsIndices[i], *this->owner) ;
      output.LieBracketOnTheLeft(tempElt);
    }
  }
  return true;
}

template<class coefficient>
bool ElementUniversalEnveloping<coefficient>::AdjointRepresentationAction(
  const ElementUniversalEnveloping<coefficient>& input,
  ElementUniversalEnveloping<coefficient>& output
) const {
  if (&input == &output) {
    global.fatal << "Input not allowed to coincide with output. " << global.fatal;
  }
  output.makeZero(*this->owner);
  ElementUniversalEnveloping<coefficient> summand;
  for (int i = 0; i < this->size(); i ++) {
    if (!(*this)[i].AdjointRepresentationAction(input, summand)) {
      return false;
    }
    summand *= this->coefficients[i];
    output += summand;
  }
  return true;
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::simplify(const coefficient& theRingUnit) {
  ElementUniversalEnveloping<coefficient> buffer;
  ElementUniversalEnveloping<coefficient> outpuT;
  MonomialUniversalEnveloping<coefficient> tempMon;
  coefficient currentCoeff;
  outpuT.makeZero(*this->owner);
  for (; this->size() > 0;) {
    // FormatExpressions tempFormat;
    //tempFormat.MakeAlphabetArbitraryWithIndex("g", "h");
    this->PopMonomial(this->size() - 1, tempMon, currentCoeff);
    bool reductionOccurred = false;
    for (int i = 0; i < tempMon.generatorsIndices.size - 1; i ++) {
      if (!this->GetOwner().AreOrderedProperly(tempMon.generatorsIndices[i], tempMon.generatorsIndices[i + 1])) {
        if (tempMon.SwitchConsecutiveIndicesIfTheyCommute(i)) {
          this->AddMonomial(tempMon, currentCoeff);
          reductionOccurred = true;
          break;
        }
        if (tempMon.CommutingAnBtoBAnPlusLowerOrderAllowed(
          tempMon.Powers[i], tempMon.generatorsIndices[i], tempMon.Powers[i + 1], tempMon.generatorsIndices[i + 1]
        )) {
          tempMon.CommuteAnBtoBAnPlusLowerOrder(i, buffer, theRingUnit);
          buffer *= currentCoeff;
          *this += buffer;
          reductionOccurred = true;
          break;
        }
        if (tempMon.CommutingABntoBnAPlusLowerOrderAllowed(
          tempMon.Powers[i], tempMon.generatorsIndices[i], tempMon.Powers[i + 1], tempMon.generatorsIndices[i + 1]
        )) {
          tempMon.CommuteABntoBnAPlusLowerOrder(i, buffer, theRingUnit);
          buffer *= currentCoeff;
          *this += buffer;
          reductionOccurred = true;
          break;
        }
      }
    }
    if (!reductionOccurred) {
      outpuT.AddMonomial(tempMon, currentCoeff);
    }
  }
  *this = outpuT;
}

template <class coefficient>
bool MonomialUniversalEnveloping<coefficient>::CommutingAnBtoBAnPlusLowerOrderAllowed(
  coefficient& theLeftPower, int leftGeneratorIndex,coefficient& theRightPower, int rightGeneratorIndex
) {
  return this->CommutingABntoBnAPlusLowerOrderAllowed(theRightPower, rightGeneratorIndex, theLeftPower, leftGeneratorIndex);
}

template <class coefficient>
bool MonomialUniversalEnveloping<coefficient>::CommutingABntoBnAPlusLowerOrderAllowed(
  coefficient& theLeftPower, int leftGeneratorIndex, coefficient& theRightPower, int rightGeneratorIndex
) {
  int leftPower, rightPower;
  if (!theLeftPower.IsSmallInteger(&leftPower)) {
    return false;
  }
  if (leftPower < 0) {
    return false;
  }
  if (theRightPower.IsSmallInteger(&rightPower)) {
    if (rightPower >= 0) {
      return true;
    }
  }
  int numPosRoots = this->GetOwner().theWeyl.RootsOfBorel.size;
  int theDimension = this->GetOwner().theWeyl.CartanSymmetric.NumRows;
  if (rightGeneratorIndex >= numPosRoots && rightGeneratorIndex < numPosRoots + theDimension) {
    return this->GetOwner().theLiebrackets.elements[leftGeneratorIndex][rightGeneratorIndex].isEqualToZero();
  }
  return true;
}

template <class coefficient>
bool MonomialUniversalEnveloping<coefficient>::SwitchConsecutiveIndicesIfTheyCommute(int theLeftIndex) {
  if (theLeftIndex >= this->generatorsIndices.size - 1) {
    return false;
  }
  int leftGenerator = this->generatorsIndices[theLeftIndex];
  int rightGenerator = this->generatorsIndices[theLeftIndex + 1];
  if (!this->GetOwner().theLiebrackets.elements[leftGenerator][rightGenerator].isEqualToZero()) {
    return false;
  }
  this->generatorsIndices.swapTwoIndices(theLeftIndex, theLeftIndex + 1);
  this->Powers.swapTwoIndices(theLeftIndex, theLeftIndex + 1);
  this->SimplifyEqualConsecutiveGenerators(theLeftIndex - 1);
  return true;
}

template <class coefficient>
void MonomialUniversalEnveloping<coefficient>::CommuteAnBtoBAnPlusLowerOrder(
  int indexA, ElementUniversalEnveloping<coefficient>& output, const coefficient& theRingUnit
) {
  if (indexA == this->generatorsIndices.size - 1) {
    return;
  }
  output.makeZero(*this->owner);
  MonomialUniversalEnveloping<coefficient> tempMon;
  tempMon.makeOne(*this->owner);
  tempMon.Powers.setExpectedSize(this->generatorsIndices.size + 2);
  tempMon.generatorsIndices.setExpectedSize(this->generatorsIndices.size + 2);
  tempMon.Powers.size = 0;
  tempMon.generatorsIndices.size = 0;
  int rightGeneratorIndeX = this->generatorsIndices[indexA + 1];
  int leftGeneratorIndeX = this->generatorsIndices[indexA];
  coefficient theRightPoweR, theLeftPoweR;
  theRightPoweR = this->Powers[indexA + 1];
  theLeftPoweR = this->Powers[indexA];
  theRightPoweR -= 1;
  int powerDroP = 0;
  coefficient acquiredCoefficienT, incomingAcquiredCoefficienT;
  acquiredCoefficienT = theRingUnit;
  for (int i = 0; i < indexA; i ++) {
    tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices[i], this->Powers[i]);
  }
  MonomialUniversalEnveloping<coefficient> startMon;
  startMon = tempMon;
  ElementSemisimpleLieAlgebra<Rational> adAToTheIthOfB, aElt;
  adAToTheIthOfB.MakeGenerator(rightGeneratorIndeX, *this->owner);
  aElt.MakeGenerator(leftGeneratorIndeX, *this->owner);
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
      tempMon.MultiplyByGeneratorPowerOnTheRight(theNewGeneratorIndex, theRingUnit);
      tempMon.MultiplyByGeneratorPowerOnTheRight(leftGeneratorIndeX, theLeftPoweR);
      tempMon.MultiplyByGeneratorPowerOnTheRight(rightGeneratorIndeX, theRightPoweR);
      for (int i = indexA + 2; i < this->generatorsIndices.size; i ++) {
        tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices[i], this->Powers[i]);
      }
      output.AddMonomial(tempMon, incomingAcquiredCoefficienT);
    }
    powerDroP ++;
    acquiredCoefficienT *= theLeftPoweR;
    acquiredCoefficienT /= powerDroP;
    theLeftPoweR -= 1;
    this->GetOwner().LieBracket(aElt, adAToTheIthOfB, adAToTheIthOfB);
  } while (!adAToTheIthOfB.isEqualToZero() && !acquiredCoefficienT.isEqualToZero());
}

template <class coefficient>
void MonomialUniversalEnveloping<coefficient>::CommuteABntoBnAPlusLowerOrder(
  int theIndeX,
  ElementUniversalEnveloping<coefficient>& output,
  const coefficient& theRingUnit
) {
  if (theIndeX == this->generatorsIndices.size - 1)
    return;
  output.makeZero(*this->owner);
  MonomialUniversalEnveloping<coefficient> tempMon;
  tempMon.makeOne(*this->owner);
  tempMon.Powers.setExpectedSize(this->generatorsIndices.size + 2);
  tempMon.generatorsIndices.setExpectedSize(this->generatorsIndices.size + 2);
  tempMon.Powers.size = 0;
  tempMon.generatorsIndices.size = 0;
  int rightGeneratorIndex = this->generatorsIndices[theIndeX + 1];
  int leftGeneratorIndex = this->generatorsIndices[theIndeX];
  coefficient theRightPower, theLeftPower;
  theRightPower = this->Powers[theIndeX + 1];
  theLeftPower = this->Powers[theIndeX];
  theLeftPower -= 1;
  int powerDrop = 0;
  coefficient acquiredCoefficient, incomingAcquiredCoefficient;
  acquiredCoefficient = theRingUnit;
  for (int i = 0; i < theIndeX; i ++) {
    tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices[i], this->Powers[i]);
  }
  tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices[theIndeX], theLeftPower);
  MonomialUniversalEnveloping<coefficient> startMon;
  startMon = tempMon;
  ElementSemisimpleLieAlgebra<Rational> adResult, tempElt, rightGeneratorElt;
  adResult.MakeGenerator(leftGeneratorIndex, *this->owner);
  rightGeneratorElt.MakeGenerator(rightGeneratorIndex, *this->owner);
  //Formula realized:
  //a b^n  &= &\sum_{i = 0}^\infty b^{n-i}(-\ad b)^i (a) \binom{n}{i} .
  //Proof (Dixmier): let L_x stand for left multiplication by x and R_x stand for right multiplication.
  //Then L_x and R_x commute and L_x-R_x =ad_x, i.e.
  //(L_b-ad_b)^n =R_b^n.
  do {
    for (int i = 0; i < adResult.size(); i ++) {
      int theNewGeneratorIndex = adResult[i].theGeneratorIndex;
      tempMon = startMon;
      tempMon.MultiplyByGeneratorPowerOnTheRight(rightGeneratorIndex, theRightPower);
      incomingAcquiredCoefficient = acquiredCoefficient;
      incomingAcquiredCoefficient *= adResult.coefficients[i];
      tempMon.MultiplyByGeneratorPowerOnTheRight(theNewGeneratorIndex, theRingUnit);
      for (int i = theIndeX + 2; i < this->generatorsIndices.size; i ++) {
        tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices[i], this->Powers[i]);
      }
      output.AddMonomial(tempMon, incomingAcquiredCoefficient);
    }
    acquiredCoefficient *= theRightPower;
    theRightPower -= 1;
    this->GetOwner().LieBracket(adResult, rightGeneratorElt, tempElt);
    adResult = tempElt;
    powerDrop ++;
    acquiredCoefficient /= powerDrop;
  } while (!adResult.isEqualToZero() && !acquiredCoefficient.isEqualToZero());
}

template <class coefficient>
bool MonomialUniversalEnveloping<coefficient>::HWTAAbilinearForm(
  const MonomialUniversalEnveloping<coefficient>& right,
  coefficient& output,
  const Vector<coefficient>* subHiGoesToIthElement,
  const coefficient& theRingUnit,
  const coefficient& theRingZero,
  std::stringstream* logStream
) {
  ElementUniversalEnveloping<coefficient> tempElt1, tempElt2;
  tempElt1.makeZero(*this->owner);
  tempElt1.AddMonomial(*this, theRingUnit);
  tempElt2.makeZero(*this->owner);
  tempElt2.AddMonomial(right, theRingUnit);
  return tempElt1.HWTAAbilinearForm(tempElt2, output, subHiGoesToIthElement, theRingUnit, theRingZero, logStream);
}

template<class coefficient>
bool ElementUniversalEnveloping<coefficient>::ApplyTransposeAntiAutoOnMe() {
  MonomialUniversalEnveloping<coefficient> tempMon;
  ElementUniversalEnveloping<coefficient> result;
  result.makeZero(*this->owner);
  int numPosRoots = this->GetOwner().GetNumPosRoots();
  int theRank = this->GetOwner().GetRank();
  coefficient theCoeff;
  this->CheckNumCoeffsConsistency();
  for (int i = 0; i < this->size(); i ++) {
    const MonomialUniversalEnveloping<coefficient>& currentMon = (*this)[i];
    theCoeff = this->coefficients[i];
    tempMon.owner = currentMon.owner;
    tempMon.Powers.size = 0;
    tempMon.generatorsIndices.size = 0;
    for (int j = currentMon.Powers.size - 1; j >= 0; j --) {
      int thePower;
      if (!currentMon.Powers[j].IsSmallInteger(&thePower)) {
        return false;
      }
      int theGenerator = currentMon.generatorsIndices[j];
      if (theGenerator < numPosRoots) {
        theGenerator = 2 * numPosRoots + theRank - 1 - theGenerator;
      } else if (theGenerator >= numPosRoots + theRank) {
        theGenerator = - theGenerator + 2 * numPosRoots + theRank - 1;
      }
      tempMon.MultiplyByGeneratorPowerOnTheRight(theGenerator, currentMon.Powers[j]);
    }
    result.AddMonomial(tempMon, theCoeff);
  }
  *this = result;
  return true;
}

template <class coefficient>
bool ElementUniversalEnveloping<coefficient>::HWTAAbilinearForm(
  const ElementUniversalEnveloping<coefficient>& right,
  coefficient& output,
  const Vector<coefficient>* subHiGoesToIthElement,
  const coefficient& theRingUnit,
  const coefficient& theRingZero,
  std::stringstream* logStream
) const {
  output = theRingZero;
  ElementUniversalEnveloping<coefficient> TAleft;
  TAleft = *this;
  if (!TAleft.ApplyTransposeAntiAutoOnMe()) {
    return false;
  }
  ElementUniversalEnveloping<coefficient> Accum, intermediateAccum, tempElt, startingElt;
  List<int> oldOrder = this->GetOwner().UEGeneratorOrderIncludingCartanElts;
  int numPosRoots = this->GetOwner().GetNumPosRoots();
  for (int i = 0; i < numPosRoots; i ++) {
    this->GetOwner().UEGeneratorOrderIncludingCartanElts[i] = - 1;
  }
  Accum.makeZero(this->GetOwner());
  MonomialUniversalEnveloping<coefficient> constMon;
  constMon.makeOne(this->GetOwner());
  if (logStream != nullptr) {
    *logStream << "left eltement transposed: "
    << TAleft.toString(&global.theDefaultFormat.getElement()) << "<br>";
    *logStream << "right element: " << right.toString(&global.theDefaultFormat.getElement()) << "<br>";
  }
  startingElt = right;
  startingElt.simplify(theRingUnit);
  if (logStream != nullptr) {
    *logStream << "right element after simplification: "
    << startingElt.toString(&global.theDefaultFormat.getElement()) << "<br>";
  }
  startingElt.ModOutVermaRelations(subHiGoesToIthElement, theRingUnit);
  if (logStream != nullptr) {
    *logStream << "right element after Verma rels: "
    << startingElt.toString(&global.theDefaultFormat.getElement()) << "<br>";
  }
  coefficient leftMonCoeff;
  for (int j = 0; j < TAleft.size(); j ++) {
    intermediateAccum = startingElt;
    const MonomialUniversalEnveloping<coefficient>& leftMon = TAleft[j];
    leftMonCoeff = TAleft.coefficients[j];
    int thePower;
    for (int i = leftMon.Powers.size - 1; i >= 0; i --) {
      if (leftMon.Powers[i].IsSmallInteger(&thePower)) {
        for (int k = 0; k < thePower; k ++) {
          tempElt.MakeOneGenerator(leftMon.generatorsIndices[i], this->GetOwner(), theRingUnit);
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
          intermediateAccum.simplify(theRingUnit);
          if (logStream != nullptr) {
            *logStream << "intermediate after simplification: "
            << intermediateAccum.toString(&global.theDefaultFormat.getElement()) << "<br>";
          }
          intermediateAccum.ModOutVermaRelations(subHiGoesToIthElement, theRingUnit, theRingZero);
          if (logStream != nullptr) {
            *logStream << "intermediate after Verma rels: "
            << intermediateAccum.toString(&global.theDefaultFormat.getElement()) << "<br>";
          }
        }
      } else {
        this->GetOwner().UEGeneratorOrderIncludingCartanElts = oldOrder;
        return false;
      }
    }
    intermediateAccum *= leftMonCoeff;
    Accum += intermediateAccum;
    int theIndex = intermediateAccum.theMonomials.getIndex(constMon);
    if (theIndex != - 1) {
      output += intermediateAccum.coefficients[theIndex];
    }
  }
  if (logStream != nullptr) {
    *logStream << "final UE element: " << Accum.toString(&global.theDefaultFormat.getElement());
  }
  this->GetOwner().UEGeneratorOrderIncludingCartanElts = oldOrder;
  return true;
}

template <class coefficient>
void MonomialUniversalEnveloping<coefficient>::ModOutVermaRelations(
  coefficient& outputCoeff,
  const Vector<coefficient>* subHiGoesToIthElement,
  const coefficient& theRingUnit,
  const coefficient& theRingZero
) {
  int numPosRoots = this->GetOwner().GetNumPosRoots();
  int theDimension = this->GetOwner().GetRank();
  outputCoeff = theRingUnit;
  for (int i = this->generatorsIndices.size - 1; i >= 0; i --) {
    int IndexCurrentGenerator = this->generatorsIndices[i];
    if (IndexCurrentGenerator >= numPosRoots + theDimension) {
      this->makeOne(*this->owner);
      outputCoeff = theRingZero;
      return;
    }
    if (IndexCurrentGenerator < numPosRoots) {
      return;
    }
    if (IndexCurrentGenerator >= numPosRoots && IndexCurrentGenerator < numPosRoots + theDimension) {
      if (subHiGoesToIthElement == 0) {
        this->makeOne(*this->owner);
        outputCoeff = theRingZero;
        return;
      }
      int theDegree;
      if (!this->Powers[i].IsSmallInteger(&theDegree)) {
        return;
      }
      int hIndex = IndexCurrentGenerator - numPosRoots;
      coefficient theSubbedH;
      theSubbedH = (*subHiGoesToIthElement)[hIndex];
      MathRoutines::RaiseToPower(theSubbedH, theDegree, theRingUnit);
      outputCoeff *= theSubbedH;
      this->generatorsIndices.size --;
      this->Powers.size --;
    }
  }
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::Substitution(const PolynomialSubstitution<Rational>& theSub) {
  ElementUniversalEnveloping<coefficient> output;
  output.makeZero(*this->owner);
  MonomialUniversalEnveloping<coefficient> theMon;
  coefficient tempCF;
  for (int i = 0; i < this->size(); i ++) {
    theMon = (*this)[i];
    theMon.Substitution(theSub);
    tempCF = this->coefficients[i];
    tempCF.Substitution(theSub);
    output.AddMonomial(theMon, tempCF);
  }
  *this = output;
}

template <class coefficient>
void MonomialUniversalEnveloping<coefficient>::Substitution(const PolynomialSubstitution<Rational>& theSub) {
  for (int i = 0; i < this->generatorsIndices.size; i ++) {
    this->Powers[i].Substitution(theSub);
  }
  this->SimplifyEqualConsecutiveGenerators(0);
}

template <class coefficient>
void MonomialUniversalEnveloping<coefficient>::SetNumVariables(int newNumVars) {
 //the below code is wrong messed up with substitutions!
  //Left in comments to remind you of what you shouldnt do.
  // if (this->Coefficient.NumVars == newNumVars)
  //  return;
  for (int i = 0; i < this->generatorsIndices.size; i ++) {
    this->Powers.theObjects[i].SetNumVariablesSubDeletedVarsByOne(newNumVars);
  }
}

template <class coefficient>
std::string MonomialUniversalEnveloping<coefficient>::toString(FormatExpressions* theFormat) const {
  std::stringstream out;
  std::string tempS;
  if (this->owner == nullptr) {
    return "(Error:Programming:NonInitializedMonomial)";
  }
  if (this->generatorsIndices.size == 0) {
    return "1";
  }
  for (int i = 0; i < this->generatorsIndices.size; i ++) {
    coefficient& thePower = this->Powers[i];
    int theIndex = this->generatorsIndices[i];
    tempS = this->GetOwner().GetStringFromChevalleyGenerator(theIndex, theFormat);
    out << tempS;
    if (!thePower.IsEqualToOne()) {
      out << "^";
      out << "{";
      out << thePower.toString(theFormat);
      out << "}";
    }
  }
  return out.str();
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::MakeCasimir(SemisimpleLieAlgebra& theOwner) {
  //std::stringstream out;
  this->makeZero(theOwner);
  WeylGroupData& theWeyl = this->GetOwner().theWeyl;
  int theDimension = theWeyl.CartanSymmetric.NumRows;
  Vector<Rational> tempRoot1, tempRoot2;
//  Matrix<Rational> killingForm;
//  killingForm.init(theDimension, theDimension);
//  for (int i = 0; i < theDimension; i ++)
//  { tempRoot1.MakeEi(theDimension, i);
//    for (int j = 0; j < theDimension; j ++)
//    { killingForm.elements[i][j] = 0;
//      tempRoot2.MakeEi(theDimension, j);
//      for (int k = 0; k<theWeyl.RootSystem.size; k++)
//        killingForm.elements[i][j] += theWeyl.RootScalarCartanRoot(tempRoot1, theWeyl.RootSystem.theObjects[k])* theWeyl.RootScalarCartanRoot(tempRoot2, theWeyl.RootSystem.theObjects[k]);
//    }
//  }
//  killingForm.Invert(global);
//  killingForm.ComputeDebugString();
//  out << killingForm.toString(true, false);


  ElementUniversalEnveloping<coefficient> tempElt1, tempElt2;
//this code is to check a math formula:
//  ElementUniversalEnveloping checkElement;
//  checkElement.makeZero(theOwner);
  Matrix<Rational> invertedSymCartan;
  invertedSymCartan = theWeyl.CartanSymmetric;
  invertedSymCartan.Invert();
////////////////////////////////////////////////////////////////////////
  for (int i = 0; i < theDimension; i ++) {
    tempRoot1.MakeEi(theDimension, i);
  //implementation without the ninja formula:
//    killingForm.ActOnVectorColumn(tempRoot1, tempRoot2);
//    tempElt1.MakeHgenerator(tempRoot1, numVars, theOwner);
//    tempElt2.MakeHgenerator(tempRoot2, numVars, theOwner);
//    tempElt1*= tempElt2;
//    *this+= tempElt1;
// Alternative implementation using a ninja formula I cooked up after looking at the printouts:
    invertedSymCartan.ActOnVectorColumn(tempRoot1, tempRoot2);
    tempElt1.MakeHgenerator(tempRoot1, theOwner);
    tempElt2.MakeHgenerator(tempRoot2, theOwner);
    tempElt1 *= tempElt2;
    *this += tempElt1;
  }
  //Rational tempRat;
  //Vector<Rational> theSum;
  for (int i = 0; i < theWeyl.RootSystem.size; i ++) {
    //Implementation without the ninja formula:
//    tempRat = 0;
//    Vector<Rational> & theRoot = theWeyl.RootSystem.theObjects[i];
//    int indexOfOpposite = theWeyl.RootSystem.getIndex(-theRoot);
//    Vector<Rational> & theOpposite = theWeyl.RootSystem.theObjects[indexOfOpposite];
//    for (int j = 0; j < theWeyl.RootSystem.size; j ++)
//    { Vector<Rational> & current = theWeyl.RootSystem.theObjects[j];
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
//     tempElt2.MakeOneGeneratorCoeffOne(theOpposite, numVars, theOwner);
//     tempElt1.MakeOneGeneratorCoeffOne(theRoot, numVars, theOwner);
//     tempElt2*= tempElt1;
//
//     tempElt2*= tempRat;
//     *this+= tempElt2;
    //The ninja formula alternative implementation:
    const Vector<Rational>& theRoot = theWeyl.RootSystem[i];
    tempElt2.MakeOneGeneratorCoeffOne(- theRoot, theOwner);
    tempElt1.MakeOneGeneratorCoeffOne(theRoot, theOwner);
    tempElt2 *= tempElt1;
    tempElt2 *= theWeyl.RootScalarCartanRoot(theWeyl.RootSystem[i], theWeyl.RootSystem[i]) / 2;
    *this += tempElt2;
  }
  *this /= theWeyl.GetKillingDivTraceRatio();
// Check that the ninja formula is correct:
//  FormatExpressions tempPolyFormat;
//  tempPolyFormat.MakeAlphabetArbitraryWithIndex("g", "h");
  //this->DebugString= out.str();
//  Vector<Rational> tempRoot;
//  for (int i = 0; i < theDimension; i ++)
//  { tempRoot.MakeEi(theDimension, i);
//    if (!length1Explored)
//    { length1= theWeyl.RootScalarCartanRoot(tempRoot, tempRoot);
//      length1Explored = true;
//      coefficient1= 0;
//      for (int j = 0; j < theWeyl.RootsOfBorel.size; j ++)
//      { coefficient1+= theWeyl.RootScalarCartanRoot(tempRoot, theWeyl.RootsOfBorel.theObjects[j])*theWeyl.RootScalarCartanRoot(tempRoot, theWeyl.RootsOfBorel.theObjects[j]);
//        coef
//      }
//    }
//  }
  this->simplify(0);
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::SubstitutionCoefficients(
  PolynomialSubstitution<Rational>& theSub, const coefficient& theRingUnit, const coefficient& theRingZero
) {
  ElementUniversalEnveloping<coefficient> endResult;
  MonomialUniversalEnveloping<coefficient> currentMon;
  endResult.makeZero(*this->owner);
  coefficient tempCF;
  for (int i = 0; i < this->size; i ++) {
    currentMon = this->theObjects[i];
    this->coefficients[i].Substitution(theSub);
    endResult.AddMonomial(currentMon, tempCF);
  }
  endResult.simplify(theRingUnit, theRingZero);
  this->operator=(endResult);
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::LieBracketOnTheRight(
  const ElementUniversalEnveloping<coefficient>& right,
  ElementUniversalEnveloping<coefficient>& output
) const {
  ElementUniversalEnveloping<coefficient> tempElt, tempElt2;
  tempElt = *this;
  tempElt *= right;
  tempElt2 = right;
  tempElt2 *= *this;
  output = tempElt;
  output -= tempElt2;
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::CleanUpZeroCoeff() {
  for (int i = 0; i < this->size; i ++) {
    if ((*this)[i].Coefficient.isEqualToZero()) {
      this->RemoveIndexSwapWithLast(i);
      i --;
    }
  }
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::MakeOneGenerator(
  int theIndex, SemisimpleLieAlgebra& inputOwner, const coefficient& theRingUnit
) {
  this->makeZero(inputOwner);
  MonomialUniversalEnveloping<coefficient> tempMon;
  tempMon.makeOne(inputOwner);
  tempMon.MultiplyByGeneratorPowerOnTheRight(theIndex, theRingUnit);
  this->AddMonomial(tempMon, theRingUnit);
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::MakeOneGeneratorCoeffOne(
  int theIndex, SemisimpleLieAlgebra& inputOwner, const coefficient& theRingUnit
) {
  this->makeZero(inputOwner);
  MonomialUniversalEnveloping<coefficient> tempMon;
  tempMon.makeOne(inputOwner);
  tempMon.MultiplyByGeneratorPowerOnTheRight(theIndex, theRingUnit);
  this->AddMonomial(tempMon, theRingUnit);
}

template <class coefficient>
bool ElementUniversalEnveloping<coefficient>::GetLieAlgebraElementIfPossible(
  ElementSemisimpleLieAlgebra<Rational>& output
) const {
  output.makeZero();
//  SemisimpleLieAlgebra& theOwner = this->owners->theObjects[this->indexInOwners];
//  int numPosRoots = theOwner.theWeyl.RootsOfBorel.size;
  coefficient theExponent;
  ChevalleyGenerator tempChevalley;
  for (int i = 0; i < this->size(); i ++) {
    const MonomialUniversalEnveloping<coefficient>& tempMon = (*this)[i];
    tempMon.GetDegree(theExponent);
    if (!theExponent.IsEqualToOne()) {
      return false;
    }
    Rational theConst;
    if (!this->coefficients[i].IsConstant(&theConst)) {
      return false;
    }
    tempChevalley.MakeGenerator(*this->owner, tempMon.generatorsIndices[0]);
    output.AddMonomial(tempChevalley, theConst);
  }
  return true;
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::AssignElementLieAlgebra(
  const ElementSemisimpleLieAlgebra<Rational>& input, SemisimpleLieAlgebra& inputOwner, const coefficient& theRingUnit
) {
  this->makeZero(inputOwner);
  MonomialUniversalEnveloping<coefficient> tempMon;
  tempMon.makeOne(inputOwner);
  tempMon.generatorsIndices.setSize(1);
  tempMon.Powers.setSize(1);
  tempMon.Powers[0] = theRingUnit;
  coefficient tempCF;
  for (int i = 0; i < input.size(); i ++) {
    tempCF = theRingUnit; //<- to facilitate implicit type conversion: theRingUnit does not have to be of type Rational
    tempCF *= input.coefficients[i];//<- to facilitate implicit type conversion: theRingUnit does not have to be of type Rational
    tempMon.generatorsIndices[0] = input[i].theGeneratorIndex;
    this->AddMonomial(tempMon, tempCF);
  }
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::GetCoordinateFormOfSpanOfElements(
  List<ElementUniversalEnveloping<coefficient> >& theElements,
  Vectors<coefficient>& outputCoordinates,
  ElementUniversalEnveloping<coefficient>& outputCorrespondingMonomials
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
    Vector<coefficient>& current = outputCoordinates[i];
    current.initializeFillInObject(outputCorrespondingMonomials.size, ZeroPoly);
    ElementUniversalEnveloping& currentElt = theElements[i];
    for (int j = 0; j < currentElt.size; j ++) {
      MonomialUniversalEnveloping<coefficient>& currentMon = currentElt[j];
      current[outputCorrespondingMonomials.getIndex(currentMon)] = currentElt.coefficients[j];
    }
  }
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::MakeHgenerator(
  const Vector<Rational>& input, SemisimpleLieAlgebra& inputOwner
) {
  MonomialUniversalEnveloping<coefficient> tempMon;
  this->makeZero(inputOwner);
  tempMon.makeOne(inputOwner);
  int theDimension = this->GetOwner().theWeyl.CartanSymmetric.NumRows;
  int numPosRoots = this->GetOwner().theWeyl.RootsOfBorel.size;
  tempMon.generatorsIndices.setSize(1);
  tempMon.Powers.setSize(1);
  coefficient tempCF;
  for (int i = 0; i < theDimension; i ++) {
    if (!input[i].isEqualToZero()) {
      (*tempMon.generatorsIndices.LastObject()) = i + numPosRoots;
      *tempMon.Powers.LastObject() = 1;
      tempCF = 1;               //<- to facilitate type conversion
      tempCF *= input[i]; //<- to facilitate type conversion we call extra assignment
      this->AddMonomial(tempMon, tempCF);
    }
  }
}

template <class coefficient>
void MonomialUniversalEnveloping<coefficient>::simplify(
  ElementUniversalEnveloping<coefficient>& output,
  const coefficient& theRingUnit
) {
  output.makeZero(*this->owner);
  output.addOnTop(*this);
  this->SimplifyAccumulateInOutputNoOutputInit(output, theRingUnit);
}

template <class coefficient>
bool ElementUniversalEnvelopingOrdered<coefficient>::GetElementUniversalEnveloping(
  ElementUniversalEnveloping<coefficient>& output, SemisimpleLieAlgebra& inputOwner
) {
  ElementUniversalEnveloping<coefficient> Accum, tempElt;
  Accum.makeZero(inputOwner);
  for (int i = 0; i < this->size; i ++) {
    if (!this->theObjects[i].GetElementUniversalEnveloping(tempElt, owner)) {
      return false;
    } else {
      Accum += tempElt;
    }
  }
  output = Accum;
  return true;
}

template <class coefficient>
bool MonomialUniversalEnvelopingOrdered<coefficient>::GetElementUniversalEnveloping(
  ElementUniversalEnveloping<coefficient>& output, SemisimpleLieAlgebraOrdered& inputOwner
) {
  ElementUniversalEnveloping<coefficient> Accum;
  ElementUniversalEnveloping<coefficient> tempMon;
  int theIndex;
  int theDegree;
  Accum.MakeConst(this->Coefficient, inputOwner);
  for (int i = 0; i < this->generatorsIndices.size; i ++) {
    if (this->Powers[i].IsSmallInteger(&theDegree)) {
      tempMon.AssignElementLieAlgebra(
        this->owner->theOrder[this->generatorsIndices[i]],
        inputOwner,
        this->Coefficient.GetOne(),
        this->Coefficient.GetZero()
      );
      tempMon.RaiseToPower(theDegree);
      Accum *= tempMon;
    } else {
      if (this->owner->theOrder[this->generatorsIndices[i]].IsACoeffOneChevalleyGenerator()) {
        tempMon.MakeOneGeneratorCoeffOne(theIndex, inputOwner, this->Coefficient.GetOne(), this->Coefficient.GetZero());
        tempMon[0].Powers[0] = this->Powers[i];
        Accum *= tempMon;
      } else {
        return false;
      }
    }
  }
  output.operator=(Accum);
  return true;
}

template <class coefficient>
void MonomialUniversalEnvelopingOrdered<coefficient>::SetNumVariables(int newNumVars) {
  this->Coefficient.SetNumVariablesSubDeletedVarsByOne(newNumVars);
  for (int i = 0; i < this->generatorsIndices.size; i ++) {
    this->Powers[i].SetNumVariablesSubDeletedVarsByOne(newNumVars);
  }
}

template <class coefficient>
bool ElementUniversalEnvelopingOrdered<coefficient>::GetCoordsInBasis(
  List<ElementUniversalEnvelopingOrdered<coefficient> >& theBasis,
  Vector<coefficient>& output,
  const coefficient& theRingUnit,
  const coefficient& theRingZero
) const {
  List<ElementUniversalEnvelopingOrdered<coefficient> > tempBasis, tempElts;
  tempBasis = theBasis;
  tempBasis.addOnTop(*this);
  Vectors<coefficient> tempCoords;
  this->GetBasisFromSpanOfElements(tempBasis, tempCoords, tempElts, theRingUnit, theRingZero, global);
  Vector<coefficient> tempRoot;
  tempRoot = *tempCoords.LastObject();
  tempCoords.setSize(theBasis.size);
  return tempRoot.getCoordinatesInBasis(tempCoords, output, theRingUnit, theRingZero);
}

template <class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::GetCoordinateFormOfSpanOfElements(
  List<ElementUniversalEnvelopingOrdered>& theElements,
  Vectors<Polynomial<coefficient> >& outputCoordinates,
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
    Vector<Polynomial<coefficient> >& current = outputCoordinates[i];
    current.initializeFillInObject(outputCorrespondingMonomials.size, ZeroPoly);
    ElementUniversalEnvelopingOrdered& currentElt = theElements[i];
    for (int j = 0; j < currentElt.size; j ++) {
      MonomialUniversalEnvelopingOrdered<coefficient>& currentMon = currentElt[j];
      current.theObjects[outputCorrespondingMonomials.getIndex(currentMon)] = currentMon.Coefficient;
    }
  }
}

template <class coefficient>
void MonomialUniversalEnvelopingOrdered<coefficient>::SubstitutionCoefficients(PolynomialSubstitution<Rational>& theSub) {
  if (theSub.size < 1)
    return;
  this->Coefficient.Substitution(theSub);
}

template <class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::SubstitutionCoefficients(PolynomialSubstitution<Rational>& theSub) {
  ElementUniversalEnvelopingOrdered<coefficient> endResult;
  MonomialUniversalEnvelopingOrdered<coefficient> currentMon;
  endResult.makeZero(*this->owner);
  for (int i = 0; i < this->size; i ++) {
    currentMon = (*this)[i];
    currentMon.SubstitutionCoefficients(theSub);
    endResult.AddMonomial(currentMon);
  }
//  endResult.simplify(theContext);
  this->operator=(endResult);
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::makeZero(SemisimpleLieAlgebra& inputOwner) {
  this->::LinearCombination<MonomialUniversalEnveloping<coefficient>, coefficient >::makeZero();
  this->owner = &inputOwner;
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::MultiplyBy(
  const MonomialUniversalEnveloping<coefficient>& standsOnTheRight, const coefficient& theCoeff
) {
  if (standsOnTheRight.generatorsIndices.size == 0) {
    return;
  }
  ElementUniversalEnveloping<coefficient> output;
  output.setExpectedSize(this->size);
  output.makeZero(*this->owner);
  MonomialUniversalEnveloping<coefficient> tempMon;
  coefficient newCoeff;
  for (int i = 0; i < this->size; i ++) {
    tempMon = (*this)[i];
    tempMon *= standsOnTheRight;
    newCoeff = this->coefficients[i];
    newCoeff *= theCoeff;
    output.AddMonomial(tempMon, newCoeff);
  }
  *this = output;
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::operator*=(const ElementUniversalEnveloping& standsOnTheRight) {
  this->::ElementMonomialAlgebra<MonomialUniversalEnveloping<coefficient>, coefficient>
  ::operator*=(static_cast<ElementMonomialAlgebra<MonomialUniversalEnveloping<coefficient>, coefficient> >(standsOnTheRight));
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::RaiseToPower(int thePower) {
  ElementUniversalEnveloping<coefficient> buffer;
  buffer = *this;
  if (this->size() == 0) {
    return;
  }
  this->MakeConst(this->coefficients[0].one(), *this->owner);
  for (int i = 0; i < thePower; i ++) {
    this->operator*=(buffer);
  }
}

template <class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::MakeOneGenerator(
  int theIndex, const coefficient& theCoeff, SemisimpleLieAlgebraOrdered& owner
) {
  this->makeZero(owner);
  MonomialUniversalEnvelopingOrdered<coefficient> tempMon;
//  tempMon.makeZero(theCoeff.NumVars, owner, theContext);
  tempMon.Coefficient = theCoeff;
  tempMon.MultiplyByGeneratorPowerOnTheRight(theIndex, tempMon.Coefficient);
  this->addOnTop(tempMon);
}

template<class coefficient>
void ElementVermaModuleOrdered<coefficient>::GetBasisFromSpanOfElements(
  List<ElementVermaModuleOrdered>& theElements,
  Vectors<RationalFunction>& outputCoordinates,
  List<ElementVermaModuleOrdered>& outputTheBasis,
  const RationalFunction& RFOne,
  const RationalFunction& RFZero
) {
  List<ElementUniversalEnvelopingOrdered<coefficient> > theEltsUEform;
  theEltsUEform.setSize(theElements.size);
  for (int i = 0; i < theElements.size; i ++) {
    theEltsUEform[i] = theElements[i].theElT;
  }
  List<ElementUniversalEnvelopingOrdered<coefficient> > theBasisUEform;
  ElementUniversalEnvelopingOrdered<coefficient>::GetBasisFromSpanOfElements(
    theEltsUEform, outputCoordinates, theBasisUEform, RFOne, RFZero, global
  );
  outputTheBasis.setSize(theBasisUEform.size);
  for (int i = 0; i < theBasisUEform.size; i ++) {
    outputTheBasis[i].theElT = theBasisUEform[i];
    outputTheBasis[i].theSubNthElementIsImageNthCoordSimpleBasis = theElements[0].theSubNthElementIsImageNthCoordSimpleBasis;
  }
}

template<class coefficient>
bool ElementVermaModuleOrdered<coefficient>::GetCoordsInBasis(
  const List<ElementVermaModuleOrdered<coefficient> >& theBasis,
  Vector<coefficient>& output,
  const coefficient& theRingUnit,
  const coefficient& theRingZero
) const {
  List<ElementUniversalEnvelopingOrdered<coefficient> > theEltsUEform;
  theEltsUEform.setSize(theBasis.size);
  for (int i = 0; i < theBasis.size; i ++) {
    theEltsUEform.theObjects[i] = theBasis.theObjects[i].theElT;
  }
  return this->theElT.GetCoordsInBasis(theEltsUEform, output, theRingUnit, theRingZero);
}

template<class coefficient>
bool ElementVermaModuleOrdered<coefficient>::needsParenthesisForMultiplication() const {
  return this->theElT.needsParenthesisForMultiplication();
}

template <class coefficient>
void ElementVermaModuleOrdered<coefficient>::AssignElementUniversalEnvelopingOrderedTimesHighestWeightVector(
  ElementUniversalEnvelopingOrdered<coefficient>& input,
  const ElementVermaModuleOrdered<coefficient>& theRingZero,
  const coefficient& theRingUnit
) {
  this->theElT.operator=(input);
  if (theRingZero.theSubNthElementIsImageNthCoordSimpleBasis.size != 3) {
    global.fatal << "theRingZero.theSubNthElementIsImageNthCoordSimpleBasis.size is not equal to 3 as expected. " << global.fatal;
  }
  this->theSubNthElementIsImageNthCoordSimpleBasis = theRingZero.theSubNthElementIsImageNthCoordSimpleBasis;
  this->theElT.ModOutVermaRelationSOld(false, this->theSubNthElementIsImageNthCoordSimpleBasis, theRingUnit);
}

template <class coefficient>
void ElementVermaModuleOrdered<coefficient>::MultiplyOnTheLeft(
  const ElementSemisimpleLieAlgebra<Rational>& other,
  ElementVermaModuleOrdered<coefficient>& output,
  const coefficient& theRingUnit,
  const coefficient& theRingZero
) {
  ElementUniversalEnvelopingOrdered<coefficient> tempElt;
  tempElt.AssignElementLieAlgebra(other, theRingUnit, theRingZero, *this->theElT.owner);
  tempElt *= this->theElT;
  output.theElT = tempElt;
  output.theElT.simplify(theRingUnit, theRingZero);
  output.theElT.ModOutVermaRelationSOld(false, this->theSubNthElementIsImageNthCoordSimpleBasis, theRingUnit);
}

template <class coefficient>
std::string ElementVermaModuleOrdered<coefficient>::toString(const FormatExpressions& theFormat) const {
  std::stringstream out;
  std::string tempS = MathRoutines::ElementToStringBrackets(this->theElT, theFormat);
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

template<class coefficient>
void ElementVermaModuleOrdered<coefficient>::ActOnMe(
  const ElementSemisimpleLieAlgebra<Rational>& actingElt,
  ElementVermaModuleOrdered<coefficient>& output,
  const coefficient& theRingUnit,
  const coefficient& theRingZero
) const {
  ElementUniversalEnvelopingOrdered<coefficient> tempElt;
  tempElt.AssignElementLieAlgebra(actingElt, theRingUnit, theRingZero, *this->theElT.owner);
  tempElt.LieBracketOnTheRight(this->theElT, output.theElT);
  output.theElT.simplify(theRingUnit, theRingZero);
  output.theSubNthElementIsImageNthCoordSimpleBasis = this->theSubNthElementIsImageNthCoordSimpleBasis;
  output.theElT.ModOutVermaRelationSOld(false, this->theSubNthElementIsImageNthCoordSimpleBasis, theRingUnit);
}

template<class coefficient>
template<class CoefficientTypeQuotientField>
void ElementUniversalEnvelopingOrdered<coefficient>::GetBasisFromSpanOfElements(
  List<ElementUniversalEnvelopingOrdered<coefficient> >& theElements,
  Vectors<CoefficientTypeQuotientField>& outputCoords,
  List<ElementUniversalEnvelopingOrdered<coefficient> >& outputTheBasis,
  const CoefficientTypeQuotientField& theFieldUnit,
  const CoefficientTypeQuotientField& theFieldZero
) {
  if (theElements.size == 0) {
    return;
  }
  ElementUniversalEnvelopingOrdered<coefficient> outputCorrespondingMonomials;
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
    ElementUniversalEnvelopingOrdered<coefficient>& currentElt = theElements[i];
    for (int j = 0; j < currentElt.size; j ++) {
      MonomialUniversalEnvelopingOrdered<coefficient>& currentMon = currentElt[j];
      currentList.theObjects[outputCorrespondingMonomials.getIndex(currentMon)] = currentMon.Coefficient;
    }
  }
  outputTheBasis.size = 0;
  outputTheBasis.Reserve(theElements.size);
  Vectors<CoefficientTypeQuotientField> basisCoordForm;
  basisCoordForm.Reserve(theElements.size);
  Selection selectedBasis;
  outputCoordsBeforeReduction.ComputeDebugString();
  outputCoordsBeforeReduction.SelectABasis(basisCoordForm, theFieldZero, selectedBasis);
  for (int i = 0; i < selectedBasis.CardinalitySelection; i ++) {
    outputTheBasis.addOnTop(theElements[selectedBasis.elements[i]]);
  }
  Matrix<coefficient> bufferMat;
  Vectors<coefficient> bufferVectors;
  outputCoordsBeforeReduction.GetCoordsInBasis(basisCoordForm, outputCoords, bufferVectors, bufferMat, theFieldUnit, theFieldZero);
}

template <class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::simplify(
  const coefficient& theRingUnit,
  const coefficient& theRingZero
) {
  ElementUniversalEnvelopingOrdered buffer;
  ElementUniversalEnvelopingOrdered output;
  output.makeZero(*this->owner);
  for (int i = 0; i < this->size; i ++) {
    this->theObjects[i].simplify(buffer, theRingUnit, theRingZero);
    output += buffer;
  }
  *this = output;
}

template <class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::makeZero(SemisimpleLieAlgebraOrdered& theOwner) {
  this->clear();
  this->owner = &theOwner;
}

template <class coefficient>
void MonomialUniversalEnvelopingOrdered<coefficient>::simplify(
  ElementUniversalEnvelopingOrdered<coefficient>& output,
  const coefficient& theRingUnit,
  const coefficient& theRingZero
) {
  output.makeZero(*this->owner);
  output.addOnTop(*this);
  this->SimplifyAccumulateInOutputNoOutputInit(output, theRingUnit, theRingZero);
}

template <class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::CleanUpZeroCoeff() {
  for (int i = 0; i < this->size; i ++) {
    if (this->theObjects[i].Coefficient.isEqualToZero()) {
      this->RemoveIndexSwapWithLast(i);
      i --;
    }
  }
}

template <class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::operator+=(const ElementUniversalEnvelopingOrdered<coefficient>& other) {
  this->Reserve(this->size + other.size);
  for (int i = 0; i < other.size; i ++)
    this->AddMonomialNoCleanUpZeroCoeff(other[i]);
  this->CleanUpZeroCoeff();
}

template <class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::operator-=(const ElementUniversalEnvelopingOrdered<coefficient>& other) {
  this->Reserve(this->size +other.size);
  MonomialUniversalEnvelopingOrdered<coefficient> tempMon;
  for (int i = 0; i < other.size; i ++) {
    tempMon = other.theObjects[i];
    tempMon.Coefficient *= - 1;
    this->AddMonomialNoCleanUpZeroCoeff(tempMon);
  }
  this->CleanUpZeroCoeff();
}

template <class coefficient>
template <class otherType>
void ElementUniversalEnvelopingOrdered<coefficient>::operator*=(const otherType& other) {
  if (other.isEqualToZero()) {
    this->makeZero(*this->owner);
    return;
  }
  for (int i = 0; i < this->size; i ++) {
    this->theObjects[i].Coefficient.operator*=(other);
  }
}

template <class coefficient>
template <class SecondType>
void ElementUniversalEnvelopingOrdered<coefficient>::operator/=(const SecondType& other) {
  for (int i = 0; i < this->size; i ++) {
    this->theObjects[i].Coefficient.operator/=(other);
  }
}

template <class coefficient>
void MonomialUniversalEnvelopingOrdered<coefficient>::MultiplyByNoSimplify(const MonomialUniversalEnvelopingOrdered& other) {
  if (this == &other) {
    global.fatal << "Output not allowed to be equal to this object. " << global.fatal;
  }
  this->generatorsIndices.Reserve(other.generatorsIndices.size + this->generatorsIndices.size);
  this->Powers.Reserve(other.generatorsIndices.size + this->generatorsIndices.size);
  this->Coefficient.MultiplyBy(other.Coefficient);
  if (other.generatorsIndices.size == 0) {
    return;
  }
  int firstIndex = other.generatorsIndices[0];
  int i = 0;
  if (this->generatorsIndices.size > 0) {
    if (firstIndex == (*this->generatorsIndices.LastObject())) {
      *this->Powers.LastObject() += other.Powers[0];
      i = 1;
    }
  }
  for (; i < other.generatorsIndices.size; i ++) {
    this->Powers.addOnTop(other.Powers[i]);
    this->generatorsIndices.addOnTop(other.generatorsIndices[i]);
  }
}

template <class coefficient>
bool ElementUniversalEnvelopingOrdered<coefficient>::IsProportionalTo(
  const ElementUniversalEnvelopingOrdered<coefficient>& other,
  coefficient& outputTimesMeEqualsOther,
  const coefficient& theRingZero
) const {
  if (this->isEqualToZero()) {
    if (other.isEqualToZero()) {
      return true;
    }
    return false;
  }
  if (other.isEqualToZero()) {
    outputTimesMeEqualsOther = theRingZero;
    return true;
  }
  if (other.size != this->size) {
    return false;
  }
  MonomialUniversalEnvelopingOrdered<coefficient>& theMon = (*this)[0];
  int theIndex = other.getIndex(theMon);
  if (theIndex == - 1) {
    return false;
  }
  MonomialUniversalEnvelopingOrdered<coefficient>& otherMon = other[theIndex];
  theMon.ComputeDebugString();
  otherMon.ComputeDebugString();
  outputTimesMeEqualsOther = otherMon.Coefficient;
  outputTimesMeEqualsOther /= theMon.Coefficient;
  ElementUniversalEnvelopingOrdered<coefficient> tempElt;

  tempElt = *this;
  tempElt *= outputTimesMeEqualsOther;
  tempElt -= other;
  return tempElt.isEqualToZero();
}

template<class coefficient>
void MonomialUniversalEnvelopingOrdered<coefficient>::operator*=(const MonomialUniversalEnvelopingOrdered& other) {
  if (this == &other) {
    global.fatal << "Output and input are the same. " << global.fatal;
  }
  this->Coefficient *= other.Coefficient;
  for (int i = 0; i < other.generatorsIndices.size; i ++) {
    this->MultiplyByGeneratorPowerOnTheRight(other.generatorsIndices[i], other.Powers[i]);
  }
}

template <class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::operator*=(
  const ElementUniversalEnvelopingOrdered<coefficient>& other
) {
  ElementUniversalEnvelopingOrdered output;
  output.makeZero(*this->owner);
  output.Reserve(this->size * other.size);
  MonomialUniversalEnvelopingOrdered<coefficient> tempMon;
  for (int i = 0; i < this->size; i ++) {
    for (int j = 0; j < other.size; j ++) {
      tempMon = (*this)[i];
      tempMon *= other[j];
      output.AddMonomial(tempMon);
    }
  }
  *this = output;
}

template <class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::AddMonomialNoCleanUpZeroCoeff(
  const MonomialUniversalEnvelopingOrdered<coefficient>& input
) {
  int theIndex = this->getIndex(input);
  if (theIndex == - 1) {
    this->addOnTop(input);
  } else {
    this->theObjects[theIndex].Coefficient += input.Coefficient;
  }
}

template <class coefficient>
void MonomialUniversalEnvelopingOrdered<coefficient>::SimplifyAccumulateInOutputNoOutputInit(
  ElementUniversalEnvelopingOrdered<coefficient>& output,
  const coefficient& theRingUnit,
  const coefficient& theRingZero
) {
  int IndexlowestNonSimplified = 0;
  ElementUniversalEnvelopingOrdered<coefficient> buffer2;
  MonomialUniversalEnvelopingOrdered<coefficient> tempMon;
  //simplified order is descending order
  while (IndexlowestNonSimplified < output.size) {
    bool reductionOccurred = false;
    if (output[IndexlowestNonSimplified].Coefficient.isEqualToZero()) {
      reductionOccurred = true;
    } else {
      for (int i = 0; i < output[IndexlowestNonSimplified].generatorsIndices.size - 1; i ++) {
        if (
          output[IndexlowestNonSimplified].generatorsIndices[i] >
          output[IndexlowestNonSimplified].generatorsIndices[i + 1]
        ) {
          if (output[IndexlowestNonSimplified].SwitchConsecutiveIndicesIfTheyCommute(
            i, tempMon, theRingZero
          )) {
            output.AddMonomialNoCleanUpZeroCoeff(tempMon);
            reductionOccurred = true;
            break;
          }
          if (this->CommutingRightIndexAroundLeftIndexAllowed(
            output[IndexlowestNonSimplified].Powers[i],
            output[IndexlowestNonSimplified].generatorsIndices[i],
            output[IndexlowestNonSimplified].Powers[i + 1],
            output[IndexlowestNonSimplified].generatorsIndices[i + 1]
          )) {
            output[IndexlowestNonSimplified].CommuteConsecutiveIndicesRightIndexAroundLeft(
              i, buffer2, theRingUnit, theRingZero
            );
            for (int j = 0; j < buffer2.size; j ++) {
              output.AddMonomialNoCleanUpZeroCoeff(buffer2[j]);
            }
            reductionOccurred = true;
            break;
          }
          if (this->CommutingLeftIndexAroundRightIndexAllowed(
            output[IndexlowestNonSimplified].Powers[i],
            output[IndexlowestNonSimplified].generatorsIndices[i],
            output[IndexlowestNonSimplified].Powers[i + 1],
            output[IndexlowestNonSimplified].generatorsIndices[i + 1]
          )) {
            output[IndexlowestNonSimplified].CommuteConsecutiveIndicesLeftIndexAroundRight(
              i, buffer2, theRingUnit, theRingZero
            );
            for (int j = 0; j < buffer2.size; j ++) {
              output.AddMonomialNoCleanUpZeroCoeff(buffer2[j]);
            }
            reductionOccurred = true;
            break;
          }
        }
      }
    }
    if (reductionOccurred) {
      output.RemoveIndexSwapWithLast(IndexlowestNonSimplified);
    } else {
      IndexlowestNonSimplified ++;
    }
  }
  output.CleanUpZeroCoeff();
}

template <class coefficient>
bool MonomialUniversalEnvelopingOrdered<coefficient>::SwitchConsecutiveIndicesIfTheyCommute(
  int theLeftIndex,
  MonomialUniversalEnvelopingOrdered<coefficient>& output,
  const coefficient& theRingZero
) {
  if (theLeftIndex >= this->generatorsIndices.size - 1) {
    return false;
  }
  int theLeftGeneratorIndex = this->generatorsIndices[theLeftIndex];
  int theRightGeneratorIndex = this->generatorsIndices[theLeftIndex + 1];
  ElementSemisimpleLieAlgebra<Rational> tempElt;
  this->owner->theOwner->LieBracket(
    this->owner->theOrder[theLeftGeneratorIndex], this->owner->theOrder[theRightGeneratorIndex], tempElt
  );
  if (tempElt.isEqualToZero()) {
    output.generatorsIndices.Reserve(this->generatorsIndices.size);
    output.Powers.Reserve(this->generatorsIndices.size);
    output.makeZero(theRingZero, *this->owner);
    output.Coefficient = this->Coefficient;
    //output.ComputeDebugString();
    for (int i = 0; i < theLeftIndex; i ++) {
      output.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices[i], this->Powers[i]);
    }
    output.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices[theLeftIndex + 1], this->Powers[theLeftIndex + 1]);
    output.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices[theLeftIndex], this->Powers[theLeftIndex]);
    for (int i = theLeftIndex + 2; i < this->generatorsIndices.size; i ++) {
      output.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices[i], this->Powers[i]);
    }
    return true;
  }
  return false;
}

template <class coefficient>
bool MonomialUniversalEnvelopingOrdered<coefficient>::CommutingRightIndexAroundLeftIndexAllowed(
  coefficient& theLeftPower, int leftGeneratorIndex, coefficient& theRightPower, int rightGeneratorIndex
) {
  return this->CommutingLeftIndexAroundRightIndexAllowed(theRightPower, rightGeneratorIndex, theLeftPower, leftGeneratorIndex);
}

template <class coefficient>
void MonomialUniversalEnvelopingOrdered<coefficient>::CommuteConsecutiveIndicesRightIndexAroundLeft(
  int theIndeX,
  ElementUniversalEnvelopingOrdered<coefficient>& output,
  const coefficient& theRingUnit,
  const coefficient& theRingZero
) {
  if (theIndeX == this->generatorsIndices.size - 1) {
    return;
  }
  output.makeZero(*this->owner);
  MonomialUniversalEnvelopingOrdered tempMon;
  tempMon.makeZero(theRingZero, *this->owner);
  tempMon.Powers.Reserve(this->generatorsIndices.size + 2);
  tempMon.generatorsIndices.Reserve(this->generatorsIndices.size + 2);
  tempMon.Powers.size = 0;
  tempMon.generatorsIndices.size = 0;
  int rightGeneratorIndeX = this->generatorsIndices.theObjects[theIndeX + 1];
  int leftGeneratorIndeX = this->generatorsIndices.theObjects[theIndeX];
  coefficient theRightPoweR, theLeftPoweR;
  theRightPoweR = this->Powers.theObjects[theIndeX + 1];
  theLeftPoweR = this->Powers.theObjects[theIndeX];
  theRightPoweR -= 1;
  int powerDroP = 0;
//  if (this->flagAnErrorHasOccurredTimeToPanic)
//  if (this->toString() == "2f_{5}f_{-5}f_{-4}" || this->toString() == "2f_{11}f_{-4}")
//  { this->flagAnErrorHasOccurredTimeToPanic = true;
//  }

  coefficient acquiredCoefficienT;
  acquiredCoefficienT = this->Coefficient;
  tempMon.Coefficient = this->Coefficient;
  for (int i = 0; i < theIndeX; i ++) {
    tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices[i], this->Powers[i]);
  }
  MonomialUniversalEnvelopingOrdered startMon;
  startMon = tempMon;
  ElementSemisimpleLieAlgebra<Rational> adResulT, tempElT, theLeftElt;
  this->owner->AssignGeneratorCoeffOne(rightGeneratorIndeX, adResulT);
  this->owner->AssignGeneratorCoeffOne(leftGeneratorIndeX, theLeftElt);
  //tempLefttElt.ComputeDebugString(*this->owner, false, false);
  Vector<Rational> theCoeffs;
  do {
    this->owner->GetLinearCombinationFrom(adResulT, theCoeffs);
    for (int i = 0; i < theCoeffs.size; i ++) {
      if (theCoeffs[i] != 0) {
        int theNewGeneratorIndex = i;
        tempMon = startMon;
        if (this->flagAnErrorHasOccurredTimeToPanic) {
          tempMon.ComputeDebugString();
          this->ComputeDebugString();
        }
        tempMon.Coefficient = acquiredCoefficienT;
        if (this->flagAnErrorHasOccurredTimeToPanic) {
          tempMon.ComputeDebugString();
          this->ComputeDebugString();
        }
        tempMon.Coefficient *= theCoeffs[i];
        if (this->flagAnErrorHasOccurredTimeToPanic) {
          tempMon.ComputeDebugString();
          this->ComputeDebugString();
        }
        tempMon.MultiplyByGeneratorPowerOnTheRight(theNewGeneratorIndex, theRingUnit);
        tempMon.MultiplyByGeneratorPowerOnTheRight(leftGeneratorIndeX, theLeftPoweR);
        tempMon.MultiplyByGeneratorPowerOnTheRight(rightGeneratorIndeX, theRightPoweR);
        for (int i = theIndeX + 2; i < this->generatorsIndices.size; i ++) {
          tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.theObjects[i], this->Powers.theObjects[i]);
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
    this->owner->theOwner->LieBracket(theLeftElt, adResulT, tempElT);
    adResulT = tempElT;
    powerDroP ++;
    acquiredCoefficienT /= powerDroP;
  } while (!adResulT.isEqualToZero() && !acquiredCoefficienT.isEqualToZero());
}

template <class coefficient>
void MonomialUniversalEnvelopingOrdered<coefficient>::CommuteConsecutiveIndicesLeftIndexAroundRight(
  int theIndeX,
  ElementUniversalEnvelopingOrdered<coefficient>& output,
  const coefficient& theRingUnit,
  const coefficient& theRingZero
) {
  if (theIndeX == this->generatorsIndices.size - 1) {
    return;
  }
  output.makeZero(*this->owner);
  MonomialUniversalEnvelopingOrdered tempMon;
  tempMon.makeZero(theRingZero, *this->owner);
  tempMon.Powers.setExpectedSize(this->generatorsIndices.size + 2);
  tempMon.generatorsIndices.setExpectedSize(this->generatorsIndices.size + 2);
  tempMon.Powers.size = 0;
  tempMon.generatorsIndices.size = 0;
  int rightGeneratorIndex = this->generatorsIndices.theObjects[theIndeX + 1];
  int leftGeneratorIndex = this->generatorsIndices.theObjects[theIndeX];
  coefficient theRightPower, theLeftPower;
  theRightPower = this->Powers.theObjects[theIndeX + 1];
  theLeftPower = this->Powers.theObjects[theIndeX];
  theLeftPower -= 1;
  int powerDrop = 0;

  coefficient acquiredCoefficient;
  acquiredCoefficient = this->Coefficient;
  tempMon.Coefficient = this->Coefficient;
  for (int i = 0; i < theIndeX; i ++) {
    tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.theObjects[i], this->Powers.theObjects[i]);
  }
  tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.theObjects[theIndeX], theLeftPower);
  MonomialUniversalEnvelopingOrdered startMon, tempMon2;
  startMon = tempMon;
  ElementSemisimpleLieAlgebra<Rational> adResult, tempElt, tempRightElt;
  this->owner->AssignGeneratorCoeffOne(leftGeneratorIndex, adResult);
  this->owner->AssignGeneratorCoeffOne(rightGeneratorIndex, tempRightElt);
//  tempRightElt.ComputeDebugString(*this->owner, false, false);
  Vector<Rational> theCoeffs;
  do {
    this->owner->GetLinearCombinationFrom(adResult, theCoeffs);
    for (int i = 0; i < theCoeffs.size; i ++) {
      if (theCoeffs[i] != 0) {
        int theNewGeneratorIndex = i;
        tempMon = startMon;
        tempMon.MultiplyByGeneratorPowerOnTheRight(rightGeneratorIndex, theRightPower);
        tempMon.Coefficient = acquiredCoefficient;
        tempMon.Coefficient *= theCoeffs[i];
        tempMon.MultiplyByGeneratorPowerOnTheRight(theNewGeneratorIndex, theRingUnit);
        for (int i = theIndeX + 2; i < this->generatorsIndices.size; i ++) {
          tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices[i], this->Powers[i]);
        }
        output.addOnTop(tempMon);
      }
    }
    acquiredCoefficient *= theRightPower;
    theRightPower -= 1;
    this->owner->theOwner->LieBracket(adResult, tempRightElt, tempElt);
    adResult = tempElt;
    powerDrop ++;
    acquiredCoefficient /= powerDrop;
    //adResult.ComputeDebugString(*this->owner, false, false);
  } while (!adResult.isEqualToZero() && !acquiredCoefficient.isEqualToZero());
}

template <class coefficient>
bool MonomialUniversalEnvelopingOrdered<coefficient>::CommutingLeftIndexAroundRightIndexAllowed(
  coefficient& theLeftPower, int leftGeneratorIndex, coefficient& theRightPower, int rightGeneratorIndex
) {
  int tempInt;
  if (theLeftPower.IsSmallInteger(&tempInt)) {
    if (theRightPower.IsSmallInteger(&tempInt)) {
      return true;
    }
    int numPosRoots = this->owner->theOwner->theWeyl.RootsOfBorel.size;
    int theDimension = this->owner->theOwner->theWeyl.CartanSymmetric.NumRows;
    if (rightGeneratorIndex >= numPosRoots && rightGeneratorIndex < numPosRoots + theDimension) {
      ElementSemisimpleLieAlgebra<Rational> tempElt;
      this->owner->theOwner->LieBracket(
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

template <class coefficient>
void MonomialUniversalEnvelopingOrdered<coefficient>::makeZero(int numVars, SemisimpleLieAlgebraOrdered& theOwner) {
  this->Coefficient.makeZero(numVars);
  this->owner = &theOwner;
  this->generatorsIndices.size = 0;
  this->Powers.size = 0;
}

template <class coefficient>
void MonomialUniversalEnvelopingOrdered<coefficient>::makeZero(
  const coefficient& theRingZero, SemisimpleLieAlgebraOrdered& theOwner
) {
  this->Coefficient = theRingZero;
  this->owner = &theOwner;
  this->generatorsIndices.size = 0;
  this->Powers.size = 0;
}

template <class coefficient>
unsigned int MonomialUniversalEnvelopingOrdered<coefficient>::hashFunction() const {
  int top = MathRoutines::Minimum(SomeRandomPrimesSize, this->generatorsIndices.size);
  unsigned int result = 0;
  for (int i = 0; i < top; i ++) {
    result += SomeRandomPrimes[i] * this->generatorsIndices.theObjects[i];
  }
  return result;
}

template <class coefficient>
void MonomialUniversalEnvelopingOrdered<coefficient>::MultiplyByGeneratorPowerOnTheRight(int theGeneratorIndex, int thePower) {
  if (thePower == 0) {
    return;
  }
  Polynomial<Rational> tempP;
  tempP.MakeConst(thePower);
  this->MultiplyByGeneratorPowerOnTheRight(theGeneratorIndex, tempP);
}

template <class coefficient>
void MonomialUniversalEnvelopingOrdered<coefficient>::MultiplyByGeneratorPowerOnTheRight(
  int theGeneratorIndex, const coefficient& thePower
) {
  if (thePower.isEqualToZero()) {
    return;
  }
  if (this->generatorsIndices.size > 0) {
    if (*this->generatorsIndices.LastObject() == theGeneratorIndex) {
      (*this->Powers.LastObject()) += thePower;
      return;
    }
  }
  this->Powers.addOnTop(thePower);
  this->generatorsIndices.addOnTop(theGeneratorIndex);
}

template <class coefficient>
std::string MonomialUniversalEnvelopingOrdered<coefficient>::toString(
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
    tempS = MathRoutines::ElementToStringBrackets(this->Coefficient, PolyFormatLocal);
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
    coefficient& thePower = this->Powers[i];
    int theIndex = this->generatorsIndices[i];
    bool usebrackets = false;
    tempS = this->owner->theOwner->GetStringFromChevalleyGenerator(theIndex, PolyFormatLocal);
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

template <class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::toString(
  std::string& output, FormatExpressions* PolyFormatLocal
) const {
  std::stringstream out;
  std::string tempS;
  if (this->size == 0) {
    out << "0";
  }
  int IndexCharAtLastLineBreak = 0;
  for (int i = 0; i < this->size; i ++) {
    MonomialUniversalEnvelopingOrdered<coefficient>& current = this->theObjects[i];
    tempS = current.toString(PolyFormatLocal);
    if (i != 0) {
      if (tempS.size() > 0) {
        if (tempS[0] != '-') {
          out << '+';
        }
      }
    }
    out << tempS;
    if ((static_cast<int>(out.tellp())) - IndexCharAtLastLineBreak > 150) {
      IndexCharAtLastLineBreak = out.tellp();
      out << "\\\\&&";
    }
  }
  output = out.str();
}

template <class coefficient>
bool ElementUniversalEnvelopingOrdered<coefficient>::AssignElementUniversalEnveloping(
  ElementUniversalEnveloping<coefficient>& input,
  SemisimpleLieAlgebraOrdered& owner,
  const coefficient& theRingUnit,
  const coefficient& theRingZero
) {
  ElementUniversalEnvelopingOrdered<coefficient> tempElt;
  this->makeZero(owner);
  for (int i = 0; i < input.size; i ++) {
    if (!tempElt.AssignMonomialUniversalEnveloping(
      input.theObjects[i], input.coefficients[i], owner, theRingUnit, theRingZero
    )) {
      return false;
    }
    this->operator+=(tempElt);
  }
  this->simplify(theRingUnit, theRingZero);
  return true;
}

template <class coefficient>
bool ElementUniversalEnvelopingOrdered<coefficient>::AssignMonomialUniversalEnveloping(
  MonomialUniversalEnveloping<coefficient>& input,
  const coefficient& inputCoeff,
  SemisimpleLieAlgebraOrdered& owner,
  const coefficient& theRingUnit,
  const coefficient& theRingZero
) {
  ElementUniversalEnvelopingOrdered theMon;
  ElementSemisimpleLieAlgebra<Rational> tempElt;
  coefficient theCoeff;
  theCoeff = inputCoeff;
  this->MakeConst(theCoeff, owner);
  for (int i = 0; i < input.generatorsIndices.size; i ++) {
    int thePower;
    bool isASmallInt = input.Powers.theObjects[i].IsSmallInteger(&thePower);
    if (isASmallInt) {
      tempElt.MakeGenerator(
        input.generatorsIndices.theObjects[i], *input.owners, input.indexInOwners
      );
      theMon.AssignElementLieAlgebra(tempElt, theRingUnit, theRingZero, owner);
      theMon.RaiseToPower(thePower, theRingUnit);
    } else {
      return false;
    }
    this->MultiplyBy(theMon);
  }
  return true;
}

template <class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::AssignElementLieAlgebra(
  const ElementSemisimpleLieAlgebra<Rational>& input,
  const coefficient& theRingUnit,
  const coefficient& theRingZero,
  SemisimpleLieAlgebraOrdered& theOwner
) {
  this->makeZero(theOwner);
  Vector<Rational> ElementRootForm;
  input.ElementToVectorNegativeRootSpacesFirst(ElementRootForm);
  theOwner.ChevalleyGeneratorsInCurrentCoords.ActOnVectorColumn(ElementRootForm);
  MonomialUniversalEnvelopingOrdered<coefficient> tempMon;
  tempMon.makeZero(theRingZero, theOwner);
  tempMon.generatorsIndices.setSize(1);
  tempMon.Powers.setSize(1);
  tempMon.Powers.theObjects[0] = theRingUnit;
  for (int theIndex = 0; theIndex < ElementRootForm.size; theIndex ++) {
    if (ElementRootForm.theObjects[theIndex] != 0) {
      tempMon.Coefficient = theRingUnit;
      tempMon.Coefficient *= ElementRootForm.theObjects[theIndex];
      tempMon.generatorsIndices[0] = theIndex;
      this->addOnTop(tempMon);
    }
  }
}

template <class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::RaiseToPower(int thePower, const coefficient& theRingUnit) {
  if (this->size == 0) {
    return;
  }
  ElementUniversalEnvelopingOrdered<coefficient> buffer;
  buffer.operator=(*this);
  this->MakeConst(theRingUnit, *this->owner);
  for (int i = 0; i < thePower; i ++) {
    this->operator*=(buffer);
  }
}

template <class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::LieBracketOnTheRight(
  const ElementSemisimpleLieAlgebra<Rational>& right, const coefficient& ringUnit, const coefficient& ringZero
) {
  ElementUniversalEnvelopingOrdered<coefficient> tempElt;
  tempElt.AssignElementLieAlgebra(right, ringUnit, ringZero, *this->owner);
  this->LieBracketOnTheRight(tempElt, *this);
}

template <class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::LieBracketOnTheRight(
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

template<class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::AddMonomial(
  const MonomialUniversalEnvelopingOrdered<coefficient>& input
) {
  if (input.isEqualToZero()) {
    return;
  }
  int theIndex = this->getIndex(input);
  if (theIndex == - 1) {
    this->addOnTop(input);
  } else {
    this->theObjects[theIndex].Coefficient += input.Coefficient;
    if (this->theObjects[theIndex].Coefficient.isEqualToZero()) {
      this->RemoveIndexSwapWithLast(theIndex);
    }
  }
}

template <class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::ModOutVermaRelationSOld(
  bool SubHighestWeightWithZeroes,
  const PolynomialSubstitution<Rational>& highestWeightSub,
  const coefficient& theRingUnit
) {
  MonomialUniversalEnvelopingOrdered<coefficient> tempMon;
  ElementUniversalEnvelopingOrdered<coefficient> output;
  output.makeZero(*this->owner);
  for (int i = 0; i < this->size; i ++) {
    tempMon = this->theObjects[i];
    tempMon.ModOutVermaRelationSOld(SubHighestWeightWithZeroes, highestWeightSub, theRingUnit);
    output.AddMonomial(tempMon);
  }
  this->operator=(output);
}

template <class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::ModOutVermaRelations(
  const List<coefficient>* subHiGoesToIthElement,
  const coefficient& theRingUnit,
  const coefficient& theRingZero
) {
  MonomialUniversalEnvelopingOrdered<coefficient> tempMon;
  ElementUniversalEnvelopingOrdered<coefficient> output;
  output.makeZero(*this->owner);
  for (int i = 0; i < this->size; i ++) {
    tempMon = this->theObjects[i];
    tempMon.ModOutVermaRelations(subHiGoesToIthElement, theRingUnit, theRingZero);
    output.AddMonomial(tempMon);
  }
  this->operator=(output);
}

template <class coefficient>
void MonomialUniversalEnvelopingOrdered<coefficient>::ModOutVermaRelations(
  const List<coefficient>* subHiGoesToIthElement,
  const coefficient& theRingUnit,
  const coefficient& theRingZero
) {
  int numPosRoots = this->owner->theOwner->GetNumPosRoots();
  int theDimension = this->owner->theOwner->GetRank();
  for (int i = this->generatorsIndices.size - 1; i >= 0; i --) {
    int IndexCurrentGenerator = this->generatorsIndices[i];
    if (IndexCurrentGenerator >= numPosRoots + theDimension) {
      this->makeZero(theRingZero, *this->owner);
      return;
    }
    if (IndexCurrentGenerator < numPosRoots) {
      return;
    }
    if (IndexCurrentGenerator >= numPosRoots &&  IndexCurrentGenerator < numPosRoots + theDimension) {
      int theDegree;
      if (!this->Powers[i].IsSmallInteger(theDegree)) {
        return;
      }
      if (subHiGoesToIthElement == 0) {
        this->makeZero(theRingZero, *this->owner);
        return;
      }
      coefficient theSubbedH = theRingZero;
      Vector<Rational> currentH = this->owner->theOrder[IndexCurrentGenerator].GetCartanPart();
      for (int j = 0; j < currentH.size; j ++) {
        theSubbedH += (*subHiGoesToIthElement)[j] * currentH[j];
      }
      MathRoutines::RaiseToPower(theSubbedH, theDegree, theRingUnit);
      this->Coefficient *= theSubbedH;
      this->generatorsIndices.size --;
      this->Powers.size --;
    }
  }
}
#endif
