//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfImplementationHeader1_HeaderUniversalEnveloping
#define vpfImplementationHeader1_HeaderUniversalEnveloping
#include "vpfHeader2Math15_UniversalEnveloping.h"
static ProjectInformationInstance ProjectInfovpfImplementationHeader1_HeaderUniversalEnveloping(__FILE__, "Implementation header, universal enveloping algebras. ");

template <class coefficient>
coefficient SemisimpleLieAlgebra::GetKillingFormProductWRTLevi
(const ElementSemisimpleLieAlgebra<coefficient>& left, const ElementSemisimpleLieAlgebra<coefficient>& right, const Selection& rootsNotInLevi)
{ MacroRegisterFunctionWithName("SemisimpleLieAlgebra::GetKillingFormWRTLevi");
  coefficient result = 0;
  ElementSemisimpleLieAlgebra<coefficient> adadAppliedToMon, tempElt;
  ChevalleyGenerator baseGen;
  Vector<Rational> rootsNotInLeviVectorForm = rootsNotInLevi;
  Vector<Rational> theWeight;
  for (int i = 0; i < this->GetNumGenerators(); i ++)
  { theWeight = this->GetWeightOfGenerator(i);
    if (theWeight.ScalarEuclidean(rootsNotInLeviVectorForm) != 0)
      continue;
    baseGen.MakeGenerator(*this, i);
    adadAppliedToMon.MakeZero();
    adadAppliedToMon.AddMonomial(baseGen, 1);
    this->LieBracket(right, adadAppliedToMon, tempElt);
    this->LieBracket(left, tempElt, adadAppliedToMon);
    result += adadAppliedToMon.GetMonomialCoefficient(baseGen);
  }
  return result;
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::MakeCasimirWRTLeviParabolic(SemisimpleLieAlgebra& theOwner, const Selection& theLeviRoots)
{ MacroRegisterFunctionWithName("ElementUniversalEnveloping::MakeCasimirWRTLeviParabolic");
  if (theLeviRoots.CardinalitySelection == 0)
  { this->MakeZero(theOwner);
    return;
  }
  coefficient result = 0;
  ElementSemisimpleLieAlgebra<Rational> leftE, rightE;
  ChevalleyGenerator baseGen;
  Selection rootsNotInLEvi = theLeviRoots;
  rootsNotInLEvi.InvertSelection();
  Vector<Rational> rootsNotInLeviVectorForm = rootsNotInLEvi;
  Vector<Rational> theWeightLeft, theWeightRight;
  this->MakeZero(theOwner);
  MonomialUniversalEnveloping<coefficient> theMon;
  Rational theCF;
  //coefficient theCFconverted;
  for (int i = 0; i < theOwner.GetNumGenerators(); i ++)
  { theWeightLeft = theOwner.GetWeightOfGenerator(i);
    if (theWeightLeft.ScalarEuclidean(rootsNotInLeviVectorForm) != 0)
      continue;
    if (theWeightLeft.IsEqualToZero())
      continue;
    theMon.MakeOne(theOwner);
    int indexOpposite = theOwner.GetGeneratorFromRoot(- theWeightLeft);
    theMon.generatorsIndices.AddOnTop(i);
    theMon.generatorsIndices.AddOnTop(indexOpposite);
    theMon.Powers.AddOnTop(1);
    theMon.Powers.AddOnTop(1);
    leftE.MakeGenerator(i, theOwner);
    rightE.MakeGenerator(indexOpposite, theOwner);
    theCF = theOwner.GetKillingFormProductWRTLevi(leftE, rightE, rootsNotInLEvi);
//    stOutput << "<hr>Killing product: " << leftE.ToString() << " and " << rightE.ToString() << " = " << theCF.ToString();
    theCF.Invert();
//    theCFconverted=theCF;
    this->AddMonomial(theMon, theCF);
  }
  Matrix<Rational> killingRestrictedToCartan;
  killingRestrictedToCartan.init(theLeviRoots.CardinalitySelection, theLeviRoots.CardinalitySelection);
  for (int i = 0; i < theLeviRoots.CardinalitySelection; i ++)
    for (int j = i; j < theLeviRoots.CardinalitySelection; j ++)
    { theWeightLeft.MakeEi(theOwner.GetRank(), theLeviRoots.elements[i]);
      theWeightRight.MakeEi(theOwner.GetRank(), theLeviRoots.elements[j]);
      leftE.MakeHgenerator(theWeightLeft, theOwner);
      rightE.MakeHgenerator(theWeightRight, theOwner);
      killingRestrictedToCartan(i, j) = theOwner.GetKillingFormProductWRTLevi(leftE, rightE, rootsNotInLEvi);
      killingRestrictedToCartan(j, i) = killingRestrictedToCartan(i, j);
    }
//  stOutput << "<br>The killing restricted to Cartan: " << killingRestrictedToCartan.ToString();
  killingRestrictedToCartan.Invert();
  ElementUniversalEnveloping<coefficient> leftUE, rightUE;
  Vector<Rational> currentEj;
  for (int i = 0; i < theLeviRoots.CardinalitySelection; i ++)
  { theWeightLeft.MakeEi(theOwner.GetRank(), theLeviRoots.elements[i]);
    theWeightRight.MakeZero(theOwner.GetRank());
    for (int j = 0; j < theLeviRoots.CardinalitySelection; j ++)
    { currentEj.MakeEi(theOwner.GetRank(), theLeviRoots.elements[j]);
      theWeightRight += currentEj * killingRestrictedToCartan(i, j);
    }
    leftUE.MakeHgenerator(theWeightLeft, theOwner);
    rightUE.MakeHgenerator(theWeightRight, theOwner);
    leftUE *= rightUE;
    *this += leftUE;
  }
  this->Simplify();
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::ModOutVermaRelations
(const Vector<coefficient>* subHiGoesToIthElement, const coefficient& theRingUnit, const coefficient& theRingZero)
{ MonomialUniversalEnveloping<coefficient> tempMon;
  ElementUniversalEnveloping<coefficient> output;
  output.MakeZero(*this->owner);
  coefficient acquiredCoeff;
  for (int i = 0; i < this->size(); i ++)
  { tempMon = (*this)[i];
    tempMon.ModOutVermaRelations(acquiredCoeff, subHiGoesToIthElement, theRingUnit, theRingZero);
    acquiredCoeff *= this->theCoeffs[i];
//    stOutput << "<hr><hr>Adding " << tempMon.ToString() << " times " << acquiredCoeff.ToString() << " to " << output.ToString();
    output.AddMonomial(tempMon, acquiredCoeff);
//    stOutput <<"<br> to obtain " << output.ToString();
  }
  this->operator=(output);
}

template<class coefficient>
void ElementUniversalEnveloping<coefficient>::LieBracketOnTheLeft(const ElementSemisimpleLieAlgebra<Rational>& left)
{ if (this->IsEqualToZero())
  { this->MakeZero(*this->owner);
    return;
  }
  ElementUniversalEnveloping<coefficient> tempElt1, tempElt2;
  tempElt1.AssignElementLieAlgebra
  (left, *this->owner, this->theCoeffs[0].GetOne());
  tempElt2 = *this;
  tempElt2.LieBracketOnTheRight(tempElt1, *this);
}

template<class coefficient>
bool MonomialUniversalEnveloping<coefficient>::AdjointRepresentationAction
(const ElementUniversalEnveloping<coefficient>& input, ElementUniversalEnveloping<coefficient>& output)const
{ output.MakeZero(*this->owner);
  ElementSemisimpleLieAlgebra<Rational> tempElt;
  output = input;
  for (int i = this->generatorsIndices.size - 1; i >= 0; i --)
  { int nextCycleSize;
    if (!this->Powers[i].IsSmallInteger(&nextCycleSize))
      return false;
    for (int j = 0; j < nextCycleSize; j ++)
    { tempElt.MakeGenerator(this->generatorsIndices[i], *this->owner) ;
      output.LieBracketOnTheLeft(tempElt);
    }
  }
  return true;
}

template<class coefficient>
bool ElementUniversalEnveloping<coefficient>::AdjointRepresentationAction
(const ElementUniversalEnveloping<coefficient>& input, ElementUniversalEnveloping<coefficient>& output)const
{ if (&input==&output)
    crash << crash;
  output.MakeZero(*this->owner);
  ElementUniversalEnveloping<coefficient> summand;
  for (int i = 0; i < this->size(); i ++)
  { if (!(*this)[i].AdjointRepresentationAction(input, summand))
      return false;
    summand *= this->theCoeffs[i];
    output += summand;
  }
  return true;
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::Simplify(const coefficient& theRingUnit)
{ ElementUniversalEnveloping<coefficient> buffer;
  ElementUniversalEnveloping<coefficient> outpuT;
  MonomialUniversalEnveloping<coefficient> tempMon;
  coefficient currentCoeff;
  outpuT.MakeZero(*this->owner);
  for (; this->size() > 0;)
  {// FormatExpressions tempFormat;
    //tempFormat.MakeAlphabetArbitraryWithIndex("g", "h");
    //stOutput << "<hr>(At the start of reduction cycle) *this+output - (At the end of reduction cycle)(*this+output)=<br>" << (*this+outpuT).ToString(&tempFormat);
    this->PopMonomial(this->size() - 1, tempMon, currentCoeff);
    bool reductionOccurred = false;
    for (int i = 0; i < tempMon.generatorsIndices.size - 1; i ++)
      if (!this->GetOwner().AreOrderedProperly(tempMon.generatorsIndices[i], tempMon.generatorsIndices[i + 1]))
      { if (tempMon.SwitchConsecutiveIndicesIfTheyCommute(i))
        { this->AddMonomial(tempMon, currentCoeff);
          reductionOccurred = true;
          break;
        }
        if (tempMon.CommutingAnBtoBAnPlusLowerOrderAllowed(tempMon.Powers[i], tempMon.generatorsIndices[i], tempMon.Powers[i+1], tempMon.generatorsIndices[i+1]))
        { tempMon.CommuteAnBtoBAnPlusLowerOrder(i, buffer, theRingUnit);
          buffer *= currentCoeff;
          *this += buffer;
          reductionOccurred = true;
          break;
        }
        if (tempMon.CommutingABntoBnAPlusLowerOrderAllowed(tempMon.Powers[i], tempMon.generatorsIndices[i], tempMon.Powers[i+1], tempMon.generatorsIndices[i+1]))
        { tempMon.CommuteABntoBnAPlusLowerOrder(i, buffer, theRingUnit);
          buffer *= currentCoeff;
          *this += buffer;
          reductionOccurred = true;
          break;
        }
      }
    if(!reductionOccurred)
      outpuT.AddMonomial(tempMon, currentCoeff);
//    stOutput << "-<br>(" << (*this+outpuT).ToString() << ")<br>(this should simplify to zero).";
  }
  *this = outpuT;
}

template <class coefficient>
bool MonomialUniversalEnveloping<coefficient>::CommutingAnBtoBAnPlusLowerOrderAllowed(coefficient& theLeftPower, int leftGeneratorIndex,coefficient& theRightPower, int rightGeneratorIndex)
{ return this->CommutingABntoBnAPlusLowerOrderAllowed(theRightPower, rightGeneratorIndex, theLeftPower, leftGeneratorIndex);
}

template <class coefficient>
bool MonomialUniversalEnveloping<coefficient>::CommutingABntoBnAPlusLowerOrderAllowed(coefficient& theLeftPower, int leftGeneratorIndex, coefficient& theRightPower, int rightGeneratorIndex)
{ int leftPower, rightPower;
  if (!theLeftPower.IsSmallInteger(&leftPower))
    return false;
  if (leftPower < 0)
    return false;
  if(theRightPower.IsSmallInteger(&rightPower))
    if (rightPower >= 0)
      return true;
  int numPosRoots = this->GetOwner().theWeyl.RootsOfBorel.size;
  int theDimension = this->GetOwner().theWeyl.CartanSymmetric.NumRows;
  if (rightGeneratorIndex >= numPosRoots && rightGeneratorIndex < numPosRoots + theDimension)
    return this->GetOwner().theLiebrackets.elements[leftGeneratorIndex][rightGeneratorIndex].IsEqualToZero();
  return true;
}

template <class coefficient>
bool MonomialUniversalEnveloping<coefficient>::SwitchConsecutiveIndicesIfTheyCommute(int theLeftIndex)
{ if (theLeftIndex >= this->generatorsIndices.size - 1)
    return false;
  int leftGenerator = this->generatorsIndices[theLeftIndex];
  int rightGenerator = this->generatorsIndices[theLeftIndex + 1];
  if (!this->GetOwner().theLiebrackets.elements[leftGenerator][rightGenerator].IsEqualToZero())
    return false;
  this->generatorsIndices.SwapTwoIndices(theLeftIndex, theLeftIndex + 1);
  this->Powers.SwapTwoIndices(theLeftIndex, theLeftIndex + 1);
  this->SimplifyEqualConsecutiveGenerators(theLeftIndex - 1);
  return true;
}

template <class coefficient>
void MonomialUniversalEnveloping<coefficient>::CommuteAnBtoBAnPlusLowerOrder
(int indexA, ElementUniversalEnveloping<coefficient>& output, const coefficient& theRingUnit)
{ if (indexA == this->generatorsIndices.size - 1)
    return;
  output.MakeZero(*this->owner);
  MonomialUniversalEnveloping<coefficient> tempMon;
  tempMon.MakeOne(*this->owner);
  tempMon.Powers.SetExpectedSize(this->generatorsIndices.size + 2);
  tempMon.generatorsIndices.SetExpectedSize(this->generatorsIndices.size + 2);
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
  for (int i = 0; i < indexA; i ++)
    tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices[i], this->Powers[i]);
  MonomialUniversalEnveloping<coefficient> startMon;
  startMon = tempMon;
  ElementSemisimpleLieAlgebra<Rational> adAToTheIthOfB, aElt;
  adAToTheIthOfB.MakeGenerator(rightGeneratorIndeX, *this->owner);
  aElt.MakeGenerator(leftGeneratorIndeX, *this->owner);
  //Formula realized:
  //a^n b =\sum_{i=0}^\infty \binom{n}{i} (\ad a)^i (b)a^{n-i}
  //Proof (Dixmier): let L_x stand for left multiplication by x and R_x stand for right multiplication.
  //Then L_x and R_x commute and L_x-R_x=ad_x, i.e.
  //(L_a)^n=(R_a+ad_a)^n.
  do
  { for (int i = 0; i < adAToTheIthOfB.size(); i ++)
    { int theNewGeneratorIndex = adAToTheIthOfB[i].theGeneratorIndex;
      tempMon = startMon;
      incomingAcquiredCoefficienT = acquiredCoefficienT;
      incomingAcquiredCoefficienT *= adAToTheIthOfB.theCoeffs[i];
      tempMon.MultiplyByGeneratorPowerOnTheRight(theNewGeneratorIndex, theRingUnit);
      tempMon.MultiplyByGeneratorPowerOnTheRight(leftGeneratorIndeX, theLeftPoweR);
      tempMon.MultiplyByGeneratorPowerOnTheRight(rightGeneratorIndeX, theRightPoweR);
      for (int i = indexA + 2; i < this->generatorsIndices.size; i ++)
        tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices[i], this->Powers[i]);
      //tempMon.ComputeDebugString();
      output.AddMonomial(tempMon, incomingAcquiredCoefficienT);
    }
    powerDroP ++;
    acquiredCoefficienT *= theLeftPoweR;
    acquiredCoefficienT /= powerDroP;
    theLeftPoweR -= 1;
//    stOutput <<"<hr>(ad_a)(" << adAToTheIthOfB.ToString(0) << ") =";
//    int commentwhendone;
//    aElt.checkConsistency();
//    adAToTheIthOfB.checkConsistency();
    this->GetOwner().LieBracket(aElt, adAToTheIthOfB, adAToTheIthOfB);
//    aElt.checkConsistency();
//    adAToTheIthOfB.checkConsistency();
//    stOutput << adAToTheIthOfB.ToString(0);
  } while(!adAToTheIthOfB.IsEqualToZero() && !acquiredCoefficienT.IsEqualToZero());
//  stOutput << "<hr>final output: " << this->ToString() << " = " << output.ToString();
}

template <class coefficient>
void MonomialUniversalEnveloping<coefficient>::CommuteABntoBnAPlusLowerOrder
(int theIndeX, ElementUniversalEnveloping<coefficient>& output, const coefficient& theRingUnit)
{ if (theIndeX == this->generatorsIndices.size - 1)
    return;
  output.MakeZero(*this->owner);
  MonomialUniversalEnveloping<coefficient> tempMon;
  tempMon.MakeOne(*this->owner);
  tempMon.Powers.SetExpectedSize(this->generatorsIndices.size + 2);
  tempMon.generatorsIndices.SetExpectedSize(this->generatorsIndices.size + 2);
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
  for (int i = 0; i < theIndeX; i ++)
    tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices[i], this->Powers[i]);
  tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices[theIndeX], theLeftPower);
  MonomialUniversalEnveloping<coefficient> startMon;
  startMon = tempMon;
  ElementSemisimpleLieAlgebra<Rational> adResult, tempElt, rightGeneratorElt;
  adResult.MakeGenerator(leftGeneratorIndex, *this->owner);
  rightGeneratorElt.MakeGenerator(rightGeneratorIndex, *this->owner);
//  stOutput << this->ToString() << "=";
  //Formula realized:
  //a b^n  &=&\sum_{i=0}^\infty b^{n-i}(-\ad b)^i (a) \binom{n}{i} .
  //Proof (Dixmier): let L_x stand for left multiplication by x and R_x stand for right multiplication.
  //Then L_x and R_x commute and L_x-R_x=ad_x, i.e.
  //(L_b-ad_b)^n=R_b^n.
  do
  { for (int i = 0; i < adResult.size(); i ++)
    { int theNewGeneratorIndex = adResult[i].theGeneratorIndex;
      tempMon = startMon;
      tempMon.MultiplyByGeneratorPowerOnTheRight(rightGeneratorIndex, theRightPower);
      incomingAcquiredCoefficient = acquiredCoefficient;
      incomingAcquiredCoefficient *= adResult.theCoeffs[i];
      tempMon.MultiplyByGeneratorPowerOnTheRight(theNewGeneratorIndex, theRingUnit);
      for (int i = theIndeX + 2; i < this->generatorsIndices.size; i ++)
        tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices[i], this->Powers[i]);
      output.AddMonomial(tempMon, incomingAcquiredCoefficient);
//      stOutput << "+(" << incomingAcquiredCoefficient.ToString() << ")" << tempMon.ToString();
    }
    acquiredCoefficient *= theRightPower;
    theRightPower -= 1;
    this->GetOwner().LieBracket(adResult, rightGeneratorElt, tempElt);
    adResult = tempElt;
    powerDrop ++;
    acquiredCoefficient /= powerDrop;
  } while (!adResult.IsEqualToZero() && !acquiredCoefficient.IsEqualToZero());
}

template <class coefficient>
bool MonomialUniversalEnveloping<coefficient>::HWTAAbilinearForm
(const MonomialUniversalEnveloping<coefficient>&right, coefficient& output, const Vector<coefficient>* subHiGoesToIthElement,
 GlobalVariables& theGlobalVariables, const coefficient& theRingUnit, const coefficient& theRingZero, std::stringstream* logStream)
{ ElementUniversalEnveloping<coefficient> tempElt1, tempElt2;
  tempElt1.MakeZero(*this->owner);
  tempElt1.AddMonomial(*this, theRingUnit);
  tempElt2.MakeZero(*this->owner);
  tempElt2.AddMonomial(right, theRingUnit);
  return tempElt1.HWTAAbilinearForm(tempElt2, output, subHiGoesToIthElement, theGlobalVariables, theRingUnit, theRingZero, logStream);
}

template<class coefficient>
bool ElementUniversalEnveloping<coefficient>::ApplyTransposeAntiAutoOnMe()
{ MonomialUniversalEnveloping<coefficient> tempMon;
  ElementUniversalEnveloping<coefficient> result;
  result.MakeZero(*this->owner);
  int numPosRoots = this->GetOwner().GetNumPosRoots();
  int theRank = this->GetOwner().GetRank();
  coefficient theCoeff;
  this->CheckNumCoeffsConsistency();
  for (int i = 0; i < this->size(); i ++)
  { const MonomialUniversalEnveloping<coefficient>& currentMon = (*this)[i];
    theCoeff = this->theCoeffs[i];
    tempMon.owner = currentMon.owner;
    tempMon.Powers.size = 0;
    tempMon.generatorsIndices.size = 0;
//    for (int j=0; j<currentMon.Powers.size; j++)
    for (int j = currentMon.Powers.size - 1; j >= 0; j --)
    { int thePower;
      if (!currentMon.Powers[j].IsSmallInteger(&thePower))
        return false;
      int theGenerator = currentMon.generatorsIndices[j];
      if (theGenerator < numPosRoots)
        theGenerator = 2 * numPosRoots + theRank - 1 - theGenerator;
      else if (theGenerator >= numPosRoots + theRank)
        theGenerator = - theGenerator + 2 * numPosRoots + theRank - 1;
      tempMon.MultiplyByGeneratorPowerOnTheRight(theGenerator, currentMon.Powers[j]);
//      if (thePower%2==1)
//        tempMon.Coefficient*=-1;
//      ;
    }
    result.AddMonomial(tempMon, theCoeff);
  }
  *this = result;
  return true;
}

template <class coefficient>
bool ElementUniversalEnveloping<coefficient>::HWTAAbilinearForm
(const ElementUniversalEnveloping<coefficient>& right, coefficient& output, const Vector<coefficient>* subHiGoesToIthElement,
 const coefficient& theRingUnit, const coefficient& theRingZero, std::stringstream* logStream)const
{ output = theRingZero;
  ElementUniversalEnveloping<coefficient> TAleft;
//  this->CheckNumCoeffsConsistency(__FILE__, __LINE__);
  TAleft = *this;
//  TAleft.CheckNumCoeffsConsistency(__FILE__, __LINE__);
//  stOutput << "<hr>TAleft: " << TAleft.ToString(&theGlobalVariables.theDefaultLieFormat);
//  stOutput << "<br>right:" << right.ToString(&theGlobalVariables.theDefaultLieFormat);
  if (!TAleft.ApplyTransposeAntiAutoOnMe())
    return false;
  ElementUniversalEnveloping<coefficient> Accum, intermediateAccum, tempElt, startingElt;
  List<int> oldOrder = this->GetOwner().UEGeneratorOrderIncludingCartanElts;
  int numPosRoots = this->GetOwner().GetNumPosRoots();
  for (int i = 0; i < numPosRoots; i ++)
    this->GetOwner().UEGeneratorOrderIncludingCartanElts[i] = - 1;
  Accum.MakeZero(this->GetOwner());
  MonomialUniversalEnveloping<coefficient> constMon;
  constMon.MakeOne(this->GetOwner());
//    stOutput << "<hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr>"
//  << "hw: " << subHiGoesToIthElement->ToString()
//  << "<hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr>"
 // << "(" << this->ElementToStringCalculatorFormat(theGlobalVariables, tempFormat)
 // << "," << right.ElementToStringCalculatorFormat(theGlobalVariables, tempFormat) << ")";
  if (logStream != 0)
  { *logStream << "left eltement transposed: " << TAleft.ToString(&theGlobalVariables.theDefaultFormat.GetElement()) << "<br>";
    *logStream << "right element: " << right.ToString(&theGlobalVariables.theDefaultFormat.GetElement()) << "<br>";
  }
  startingElt = right;
  startingElt.Simplify(theRingUnit);
  if (logStream != 0)
    *logStream << "right element after simplification: " << startingElt.ToString(&theGlobalVariables.theDefaultFormat.GetElement()) << "<br>";
  startingElt.ModOutVermaRelations(subHiGoesToIthElement, theRingUnit);
  if (logStream != 0)
    *logStream << "right element after Verma rels: " << startingElt.ToString(&theGlobalVariables.theDefaultFormat.GetElement()) << "<br>";
  coefficient leftMonCoeff;
  for (int j = 0; j < TAleft.size(); j ++)
  { intermediateAccum = startingElt;
    const MonomialUniversalEnveloping<coefficient>& leftMon = TAleft[j];
    leftMonCoeff = TAleft.theCoeffs[j];
    int thePower;
    for (int i = leftMon.Powers.size - 1; i >= 0; i --)
      if (leftMon.Powers[i].IsSmallInteger(&thePower))
        for (int k = 0; k < thePower; k ++)
        { tempElt.MakeOneGenerator(leftMon.generatorsIndices[i], this->GetOwner(), theRingUnit);
          MathRoutines::swap(tempElt, intermediateAccum);
          if (logStream!=0)
          { //*logStream << "tempElt before mult: " << tempElt.ToString(theGlobalVariables, tempFormat) << "<br>";
            *logStream << "intermediate before mult: " << intermediateAccum.ToString(&theGlobalVariables.theDefaultFormat.GetElement()) << "<br>";
          }
          intermediateAccum*=(tempElt);
          if (logStream!=0)
            *logStream << "intermediate before simplification: " << intermediateAccum.ToString(&theGlobalVariables.theDefaultFormat.GetElement()) << "<br>";
          intermediateAccum.Simplify(theRingUnit);
          if (logStream!=0)
            *logStream << "intermediate after simplification: " << intermediateAccum.ToString(&theGlobalVariables.theDefaultFormat.GetElement()) << "<br>";
          intermediateAccum.ModOutVermaRelations(subHiGoesToIthElement, theRingUnit, theRingZero);
          if (logStream!=0)
            *logStream << "intermediate after Verma rels: " << intermediateAccum.ToString(&theGlobalVariables.theDefaultFormat.GetElement()) << "<br>";
        }
      else
      { this->GetOwner().UEGeneratorOrderIncludingCartanElts=oldOrder;
        return false;
      }
    intermediateAccum*=leftMonCoeff;
    Accum+=intermediateAccum;
    int theIndex= intermediateAccum.theMonomials.GetIndex(constMon);
    if (theIndex!=-1)
      output+=intermediateAccum.theCoeffs[theIndex];
  }
  if (logStream!=0)
    *logStream << "final UE element: " << Accum.ToString(&theGlobalVariables.theDefaultFormat.GetElement());
  this->GetOwner().UEGeneratorOrderIncludingCartanElts=oldOrder;
//  if (logStream!=0)
//    stOutput << "<hr>" << logStream->str();
  return true;
}

template <class coefficient>
void MonomialUniversalEnveloping<coefficient>::ModOutVermaRelations
(coefficient& outputCoeff, const Vector<coefficient>* subHiGoesToIthElement, const coefficient& theRingUnit, const coefficient& theRingZero)
{ int numPosRoots=this->GetOwner().GetNumPosRoots();
  int theDimension=this->GetOwner().GetRank();
  outputCoeff=theRingUnit;
//  if (subHiGoesToIthElement!=0)
//    stOutput << "<br>The subHiGoesToIthElement: " << subHiGoesToIthElement->ToString();
  for (int i=this->generatorsIndices.size-1; i>=0; i--)
  { int IndexCurrentGenerator=this->generatorsIndices[i];
    if (IndexCurrentGenerator>=numPosRoots+theDimension)
    { this->MakeOne(*this->owner);
      outputCoeff=theRingZero;
      return;
    }
    if (IndexCurrentGenerator<numPosRoots)
      return;
    if (IndexCurrentGenerator>=numPosRoots && IndexCurrentGenerator<numPosRoots+theDimension)
    { if (subHiGoesToIthElement==0)
      { this->MakeOne(*this->owner);
        outputCoeff=theRingZero;
        return;
      }
      int theDegree;
      if (!this->Powers[i].IsSmallInteger(&theDegree))
        return;
      int hIndex=IndexCurrentGenerator-numPosRoots;
      coefficient theSubbedH;
      theSubbedH=(*subHiGoesToIthElement)[hIndex];
      MathRoutines::RaiseToPower(theSubbedH, theDegree, theRingUnit);
      outputCoeff*=(theSubbedH);
//      stOutput << "&nbsp outputCoeff=" << outputCoeff.ToString();
      this->generatorsIndices.size--;
      this->Powers.size--;
    }
  }
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::Substitution(const PolynomialSubstitution<Rational>& theSub)
{ ElementUniversalEnveloping<coefficient> output;
  output.MakeZero(*this->owner);
  MonomialUniversalEnveloping<coefficient> theMon;
  coefficient tempCF;
  for (int i=0; i<this->size(); i++)
  { theMon=(*this)[i];
    theMon.Substitution(theSub);
    tempCF=this->theCoeffs[i];
    tempCF.Substitution(theSub);
    output.AddMonomial(theMon, tempCF);
  }
  *this=output;
}

template <class coefficient>
void MonomialUniversalEnveloping<coefficient>::Substitution(const PolynomialSubstitution<Rational>& theSub)
{ for (int i=0; i<this->generatorsIndices.size; i++)
    this->Powers[i].Substitution(theSub);
  this->SimplifyEqualConsecutiveGenerators(0);
}

template <class coefficient>
void MonomialUniversalEnveloping<coefficient>::SetNumVariables(int newNumVars)
{//the below code is wrong messed up with substitutions!
  //Left in comments to remind you of what you shouldnt do.
  // if (this->Coefficient.NumVars==newNumVars)
  //  return;
  for(int i=0; i<this->generatorsIndices.size; i++)
    this->Powers.TheObjects[i].SetNumVariablesSubDeletedVarsByOne(newNumVars);
}

template <class coefficient>
std::string MonomialUniversalEnveloping<coefficient>::ToString(FormatExpressions* theFormat)const
{ std::stringstream out;
  std::string tempS;
  if (this->owner==0)
    return "(Error:Programming:NonInitializedMonomial)";
  if (this->generatorsIndices.size==0)
    return "1";
  for (int i=0; i<this->generatorsIndices.size; i++)
  { coefficient& thePower=this->Powers[i];
    int theIndex=this->generatorsIndices[i];
    tempS=this->GetOwner().GetStringFromChevalleyGenerator(theIndex, theFormat);
    //if (thePower>1)
    //  out << "(";
    out << tempS;
    if (!thePower.IsEqualToOne())
    { out << "^";
     // if (useRootIndices)
      out << "{";
      out << thePower.ToString(theFormat);
      //if (useRootIndices)
      out << "}";
    }
    //if (thePower>1)
    //  out << ")";
  }
  return out.str();
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::MakeCasimir(SemisimpleLieAlgebra& theOwner)
{ //std::stringstream out;
  this->MakeZero(theOwner);
  WeylGroupData& theWeyl= this->GetOwner().theWeyl;
  int theDimension=theWeyl.CartanSymmetric.NumRows;
  Vector<Rational> tempRoot1, tempRoot2;
//  Matrix<Rational> killingForm;
//  killingForm.init(theDimension, theDimension);
//  for (int i=0; i<theDimension; i++)
//  { tempRoot1.MakeEi(theDimension, i);
//    for (int j=0; j<theDimension; j++)
//    { killingForm.elements[i][j]=0;
//      tempRoot2.MakeEi(theDimension, j);
//      for (int k=0; k<theWeyl.RootSystem.size; k++)
//        killingForm.elements[i][j]+= theWeyl.RootScalarCartanRoot(tempRoot1, theWeyl.RootSystem.TheObjects[k])* theWeyl.RootScalarCartanRoot(tempRoot2, theWeyl.RootSystem.TheObjects[k]);
//    }
//  }
//  stOutput << killingForm.ToString(true, false);
//  killingForm.Invert(theGlobalVariables);
//  killingForm.ComputeDebugString();
//  out << killingForm.ToString(true, false);
//  stOutput << killingForm.ToString(true, false);


  ElementUniversalEnveloping<coefficient> tempElt1, tempElt2;
//this code is to check a math formula:
//  ElementUniversalEnveloping checkElement;
//  checkElement.MakeZero(theOwner);
  Matrix<Rational> invertedSymCartan;
  invertedSymCartan=theWeyl.CartanSymmetric;
  invertedSymCartan.Invert();
////////////////////////////////////////////////////////////////////////
  for (int i=0; i<theDimension; i++)
  { tempRoot1.MakeEi(theDimension, i);
  //implementation without the ninja formula:
//    killingForm.ActOnVectorColumn(tempRoot1, tempRoot2);
//    tempElt1.MakeHgenerator(tempRoot1, numVars, theOwner);
//    tempElt2.MakeHgenerator(tempRoot2, numVars, theOwner);
//    tempElt1*=tempElt2;
//    *this+=tempElt1;
// Alternative implementation using a ninja formula I cooked up after looking at the printouts:
    invertedSymCartan.ActOnVectorColumn(tempRoot1, tempRoot2);
    tempElt1.MakeHgenerator(tempRoot1, theOwner);
    tempElt2.MakeHgenerator(tempRoot2, theOwner);
    tempElt1*=tempElt2;
    *this+=tempElt1;
  }
  Rational tempRat;
  Vector<Rational> theSum;
  for (int i=0; i<theWeyl.RootSystem.size; i++)
  { //Implementation without the ninja formula:
//    tempRat=0;
//    Vector<Rational> & theRoot=theWeyl.RootSystem.TheObjects[i];
//    int indexOfOpposite=theWeyl.RootSystem.GetIndex(-theRoot);
//    Vector<Rational> & theOpposite= theWeyl.RootSystem.TheObjects[indexOfOpposite];
//    for (int j=0; j<theWeyl.RootSystem.size; j++)
//    { Vector<Rational> & current=theWeyl.RootSystem.TheObjects[j];
//      if (current==theOpposite)
//        tempRat+=2;
//       else
//       { int indexOfSum=theWeyl.RootSystem.GetIndex(current+theRoot);
//         if (indexOfSum!=-1)
//           tempRat+=(theOwner.ChevalleyConstants.elements[i][j]*theOwner.ChevalleyConstants.elements[indexOfOpposite][indexOfSum]);
//       }
//     }
//     tempRat+=2;
//     tempRat= 1/tempRat;
//     tempElt2.MakeOneGeneratorCoeffOne(theOpposite, numVars, theOwner);
//     tempElt1.MakeOneGeneratorCoeffOne(theRoot, numVars, theOwner);
//     tempElt2*=tempElt1;
//
//     tempElt2*=tempRat;
//     *this+=tempElt2;
    //The ninja formula alternative implementation:
    Vector<Rational>& theRoot=theWeyl.RootSystem.TheObjects[i];
    tempElt2.MakeOneGeneratorCoeffOne(-theRoot, theOwner);
    tempElt1.MakeOneGeneratorCoeffOne(theRoot, theOwner);
    tempElt2*=tempElt1;
    tempElt2*=theWeyl.RootScalarCartanRoot(theWeyl.RootSystem[i], theWeyl.RootSystem[i])/2;
    *this+=tempElt2;
  }
  *this/=theWeyl.GetKillingDivTraceRatio();
// Check that the ninja formula is correct:
//  FormatExpressions tempPolyFormat;
//  tempPolyFormat.MakeAlphabetArbitraryWithIndex("g", "h");
//  stOutput << "Killing divided by trace ratio:" << theWeyl.GetKillingDivTraceRatio().ToString();
//  stOutput << "<br>casimir: " << this->ToString(false, false, theGlobalVariables, tempPolyFormat);
//  stOutput << "<br>check element: " << checkElement.ToString(false, false, theGlobalVariables, tempPolyFormat);

//  stOutput << "<br> check element minus casimir=" << checkElement.ToString(false, false, theGlobalVariables, tempPolyFormat);
  //this->DebugString=out.str();
//  Vector<Rational> tempRoot;
//  for (int i=0; i<theDimension; i++)
//  { tempRoot.MakeEi(theDimension, i);
//    if (!length1Explored)
//    { length1= theWeyl.RootScalarCartanRoot(tempRoot, tempRoot);
//      length1Explored=true;
//      coefficient1=0;
//      for (int j=0; j<theWeyl.RootsOfBorel.size; j++)
//      { coefficient1+=theWeyl.RootScalarCartanRoot(tempRoot, theWeyl.RootsOfBorel.TheObjects[j])*theWeyl.RootScalarCartanRoot(tempRoot, theWeyl.RootsOfBorel.TheObjects[j]);
//        coef
//      }
//    }
//  }
  this->Simplify(0);
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::SubstitutionCoefficients
(PolynomialSubstitution<Rational>& theSub, GlobalVariables* theContext, const coefficient& theRingUnit, const coefficient& theRingZero)
{ ElementUniversalEnveloping<coefficient> endResult;
  MonomialUniversalEnveloping<coefficient> currentMon;
  endResult.MakeZero(*this->owner);
  coefficient tempCF;
  for (int i=0; i<this->size; i++)
  { currentMon=this->TheObjects[i];
    this->theCoeffs[i].Substitution(theSub);
    endResult.AddMonomial(currentMon, tempCF);
  }
  if (theContext!=0)
    endResult.Simplify(*theContext, theRingUnit, theRingZero);
  else
  { GlobalVariables theGlobalVariables;
    endResult.Simplify(theGlobalVariables, theRingUnit, theRingZero);
  }
  this->operator=(endResult);
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::LieBracketOnTheRight(const ElementUniversalEnveloping<coefficient>& right, ElementUniversalEnveloping<coefficient>& output)const
{ ElementUniversalEnveloping<coefficient> tempElt, tempElt2;
  tempElt=*this;
  tempElt*=right;
//  stOutput  << "this: " << this->ToString()
//  << " right: " << right.ToString() << " this*right: " << tempElt.ToString();
  tempElt2=right;
  tempElt2*=*this;
  output=tempElt;
  output-=tempElt2;
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::CleanUpZeroCoeff()
{ for (int i=0; i<this->size; i++)
    if (this->TheObjects[i].Coefficient.IsEqualToZero())
    { this->RemoveIndexSwapWithLast(i);
      i--;
    }
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::MakeOneGenerator(int theIndex, SemisimpleLieAlgebra& inputOwner, const coefficient& theRingUnit)
{ this->MakeZero(inputOwner);
  MonomialUniversalEnveloping<coefficient> tempMon;
  tempMon.MakeOne(inputOwner);
  tempMon.MultiplyByGeneratorPowerOnTheRight(theIndex, theRingUnit);
  this->AddMonomial(tempMon, theRingUnit);
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::MakeOneGeneratorCoeffOne
(int theIndex, SemisimpleLieAlgebra& inputOwner, const coefficient& theRingUnit)
{ this->MakeZero(inputOwner);
  MonomialUniversalEnveloping<coefficient> tempMon;
  tempMon.MakeOne(inputOwner);
  tempMon.MultiplyByGeneratorPowerOnTheRight(theIndex, theRingUnit);
  this->AddMonomial(tempMon, theRingUnit);
}

template <class coefficient>
bool ElementUniversalEnveloping<coefficient>::GetLieAlgebraElementIfPossible(ElementSemisimpleLieAlgebra<Rational>& output)const
{ output.MakeZero();
//  SemisimpleLieAlgebra& theOwner=this->owners->TheObjects[this->indexInOwners];
//  int numPosRoots=theOwner.theWeyl.RootsOfBorel.size;
  Rational theConst=0;
  coefficient theExponent;
  ChevalleyGenerator tempChevalley;
  for (int i=0; i<this->size(); i++)
  { const MonomialUniversalEnveloping<coefficient>& tempMon= (*this)[i];
    tempMon.GetDegree(theExponent);
    if (!theExponent.IsEqualToOne())
      return false;
    Rational theConst;
    if (!this->theCoeffs[i].IsConstant(&theConst))
      return false;
    tempChevalley.MakeGenerator(*this->owner, tempMon.generatorsIndices[0]);
    output.AddMonomial(tempChevalley, theConst);
  }
  return true;
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::AssignElementLieAlgebra
(const ElementSemisimpleLieAlgebra<Rational>& input, SemisimpleLieAlgebra& inputOwner, const coefficient& theRingUnit)
{ this->MakeZero(inputOwner);
  MonomialUniversalEnveloping<coefficient> tempMon;
  tempMon.MakeOne(inputOwner);
  tempMon.generatorsIndices.SetSize(1);
  tempMon.Powers.SetSize(1);
  tempMon.Powers[0]=theRingUnit;
  coefficient tempCF;
  for (int i=0; i<input.size(); i++)
  { tempCF=theRingUnit; //<- to facilitate implicit type conversion: theRingUnit does not have to be of type Rational
    tempCF*=input.theCoeffs[i];//<- to facilitate implicit type conversion: theRingUnit does not have to be of type Rational
    tempMon.generatorsIndices[0]=input[i].theGeneratorIndex;
    this->AddMonomial(tempMon, tempCF);
  }
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::GetCoordinateFormOfSpanOfElements
(List<ElementUniversalEnveloping<coefficient> >& theElements, Vectors<coefficient>& outputCoordinates, ElementUniversalEnveloping<coefficient>& outputCorrespondingMonomials,
 GlobalVariables& theGlobalVariables)
{ if (theElements.size==0)
    return;
  outputCorrespondingMonomials.MakeZero(*theElements[0].owners, theElements[0].indexInOwners);
  MonomialUniversalEnveloping<coefficient> tempMon;
  for (int i=0; i<theElements.size; i++)
    for (int j=0; j<theElements[i].size; j++)
      outputCorrespondingMonomials.AddOnTopNoRepetition(theElements[i][j]);
  outputCoordinates.SetSize(theElements.size);
  Polynomial<Rational> ZeroPoly;
  ZeroPoly.MakeZero();
  for (int i=0; i<theElements.size; i++)
  { Vector<coefficient>& current=outputCoordinates[i];
    current.initFillInObject(outputCorrespondingMonomials.size, ZeroPoly);
    ElementUniversalEnveloping& currentElt=theElements[i];
    for (int j=0; j<currentElt.size; j++)
    { MonomialUniversalEnveloping<coefficient>& currentMon=currentElt.TheObjects[j];
      current.TheObjects[outputCorrespondingMonomials.GetIndex(currentMon)]=currentElt.theCoeffs[j];
    }
  }
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::MakeHgenerator(const Vector<Rational>& input, SemisimpleLieAlgebra& inputOwner)
{ MonomialUniversalEnveloping<coefficient> tempMon;
  this->MakeZero(inputOwner);
  tempMon.MakeOne(inputOwner);
  int theDimension= this->GetOwner().theWeyl.CartanSymmetric.NumRows;
  int numPosRoots=this->GetOwner().theWeyl.RootsOfBorel.size;
  tempMon.generatorsIndices.SetSize(1);
  tempMon.Powers.SetSize(1);
  coefficient tempCF;
  for (int i=0; i<theDimension; i++)
    if (!input[i].IsEqualToZero())
    { (*tempMon.generatorsIndices.LastObject())=i+numPosRoots;
      *tempMon.Powers.LastObject()=1;
      tempCF=1;               //<- to facilitate type conversion
      tempCF*=input[i]; //<- to facilitate type conversion we call extra assignment
      this->AddMonomial(tempMon, tempCF);
    }
}

template <class coefficient>
void MonomialUniversalEnveloping<coefficient>::Simplify
(ElementUniversalEnveloping<coefficient>& output, GlobalVariables& theGlobalVariables, const coefficient& theRingUnit)
{ output.MakeZero(*this->owner);
  output.AddOnTop(*this);
  this->SimplifyAccumulateInOutputNoOutputInit(output, theGlobalVariables, theRingUnit);
}

template <class coefficient>
bool ElementUniversalEnvelopingOrdered<coefficient>::GetElementUniversalEnveloping(ElementUniversalEnveloping<coefficient>& output, SemisimpleLieAlgebra& inputOwner)
{ ElementUniversalEnveloping<coefficient> Accum, tempElt;
  Accum.MakeZero(inputOwner);
  for (int i=0; i<this->size; i++)
    if (!this->TheObjects[i].GetElementUniversalEnveloping(tempElt, owner))
      return false;
    else
      Accum+=tempElt;
  output=Accum;
  return true;
}

template <class coefficient>
bool MonomialUniversalEnvelopingOrdered<coefficient>::GetElementUniversalEnveloping(ElementUniversalEnveloping<coefficient>& output, SemisimpleLieAlgebraOrdered &inputOwner)
{ ElementUniversalEnveloping<coefficient> Accum;
  ElementUniversalEnveloping<coefficient> tempMon;
  int theIndex;
  int theDegree;
  Accum.MakeConst(this->Coefficient, inputOwner);
  for (int i=0; i<this->generatorsIndices.size; i++)
    if (this->Powers[i].IsSmallInteger(&theDegree))
    { tempMon.AssignElementLieAlgebra(this->owner->theOrder[this->generatorsIndices[i]], inputOwner, this->Coefficient.GetOne(), this->Coefficient.GetZero());
      tempMon.RaiseToPower(theDegree);
      Accum*=(tempMon);
    } else
      if (this->owner->theOrder[this->generatorsIndices[i]].IsACoeffOneChevalleyGenerator(theIndex, *inputOwner.theOwner))
      { tempMon.MakeOneGeneratorCoeffOne(theIndex, inputOwner, this->Coefficient.GetOne(), this->Coefficient.GetZero());
        tempMon[0].Powers[0]=this->Powers[i];
        Accum*=(tempMon);
      } else
        return false;
  output.operator=(Accum);
  return true;
}

template <class coefficient>
void MonomialUniversalEnvelopingOrdered<coefficient>::SetNumVariables(int newNumVars)
{ //the below commented out code causes problems in substitution code!
  //if (this->Coefficient.NumVars==newNumVars)
  //  return;
  this->Coefficient.SetNumVariablesSubDeletedVarsByOne((int)newNumVars);
  for(int i=0; i<this->generatorsIndices.size; i++)
    this->Powers.TheObjects[i].SetNumVariablesSubDeletedVarsByOne((int)newNumVars);
}

template <class coefficient>
bool ElementUniversalEnvelopingOrdered<coefficient>::GetCoordsInBasis
(List<ElementUniversalEnvelopingOrdered<coefficient> >& theBasis, Vector<coefficient>& output,
 const coefficient& theRingUnit, const coefficient& theRingZero)const
{ List<ElementUniversalEnvelopingOrdered<coefficient> > tempBasis, tempElts;
  tempBasis=theBasis;
  tempBasis.AddOnTop(*this);
  Vectors<coefficient> tempCoords;
  this->GetBasisFromSpanOfElements(tempBasis, tempCoords, tempElts, theRingUnit, theRingZero, theGlobalVariables);
  Vector<coefficient> tempRoot;
  tempRoot=*tempCoords.LastObject();
  tempCoords.SetSize(theBasis.size);
  return tempRoot.GetCoordsInBasiS(tempCoords, output, theRingUnit, theRingZero);
}

template <class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::GetCoordinateFormOfSpanOfElements
(int numVars, List<ElementUniversalEnvelopingOrdered>& theElements, Vectors<Polynomial<coefficient> >& outputCoordinates,
 ElementUniversalEnvelopingOrdered& outputCorrespondingMonomials)
{ if (theElements.size==0)
    return;
  outputCorrespondingMonomials.MakeZero(*theElements[0].owner);
  MonomialUniversalEnveloping<coefficient> tempMon;
  for (int i=0; i<theElements.size; i++)
    for (int j=0; j<theElements[i].size; j++)
      outputCorrespondingMonomials.AddOnTopNoRepetition(theElements[i][j]);
  outputCoordinates.SetSize(theElements.size);
  Polynomial<Rational>  ZeroPoly;
  ZeroPoly.MakeZero();
  for (int i=0; i<theElements.size; i++)
  { Vector<Polynomial<coefficient> >& current=outputCoordinates[i];
    current.initFillInObject(outputCorrespondingMonomials.size, ZeroPoly);
    ElementUniversalEnvelopingOrdered& currentElt=theElements[i];
    for (int j=0; j<currentElt.size; j++)
    { MonomialUniversalEnvelopingOrdered<coefficient>& currentMon=currentElt[j];
      current.TheObjects[outputCorrespondingMonomials.GetIndex(currentMon)]=currentMon.Coefficient;
    }
  }
}

template <class coefficient>
void MonomialUniversalEnvelopingOrdered<coefficient>::SubstitutionCoefficients(PolynomialSubstitution<Rational>& theSub)
{ if (theSub.size<1)
    return;
  this->Coefficient.Substitution(theSub);
}

template <class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::SubstitutionCoefficients(PolynomialSubstitution<Rational>& theSub, GlobalVariables* theContext)
{ ElementUniversalEnvelopingOrdered<coefficient> endResult;
  MonomialUniversalEnvelopingOrdered<coefficient> currentMon;
  endResult.MakeZero(*this->owner);
  for (int i=0; i<this->size; i++)
  { currentMon=this->TheObjects[i];
    currentMon.SubstitutionCoefficients(theSub);
    endResult.AddMonomial(currentMon);
  }
//  endResult.Simplify(theContext);
  this->operator=(endResult);
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::MakeConst(const Rational& coeff, int numVars, SemisimpleLieAlgebra& inputOwner)
{ MonomialUniversalEnveloping<coefficient> tempMon;
  this->MakeZero(inputOwner);
  if (coeff.IsEqualToZero())
    return;
  coefficient tempP;
  tempP= coeff;
  tempMon.MakeConst(inputOwner);
  this->AddMonomial(tempMon, tempP);
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::MakeZero(SemisimpleLieAlgebra& inputOwner)
{ this->::MonomialCollection<MonomialUniversalEnveloping<coefficient>, coefficient >::MakeZero();
  this->owner=&inputOwner;
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::MultiplyBy(const MonomialUniversalEnveloping<coefficient>& standsOnTheRight, const coefficient& theCoeff)
{ if (standsOnTheRight.generatorsIndices.size==0)
    return;
  ElementUniversalEnveloping<coefficient> output;
  output.SetExpectedSize(this->size);
  output.MakeZero(*this->owner);
  MonomialUniversalEnveloping<coefficient> tempMon;
  coefficient newCoeff;
  for (int i=0; i<this->size; i++)
  { tempMon=this->TheObjects[i];
    tempMon*=standsOnTheRight;
    newCoeff=this->theCoeffs[i];
    newCoeff*=theCoeff;
    output.AddMonomial(tempMon, newCoeff);
  }
  (*this)=output;
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::operator*=(const ElementUniversalEnveloping& standsOnTheRight)
{ this->::ElementMonomialAlgebra<MonomialUniversalEnveloping<coefficient>, coefficient>
  ::operator*=( (ElementMonomialAlgebra<MonomialUniversalEnveloping<coefficient>, coefficient>)standsOnTheRight);
}

template <class coefficient>
void ElementUniversalEnveloping<coefficient>::RaiseToPower(int thePower)
{ ElementUniversalEnveloping<coefficient> buffer;
  buffer=*this;
  if (this->size()==0)
    return;
  this->MakeConst(this->theCoeffs[0].GetOne(), *this->owner);
  for (int i=0; i<thePower; i++)
    this->operator*=(buffer);
}

template <class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::MakeOneGenerator
(int theIndex, const coefficient& theCoeff, SemisimpleLieAlgebraOrdered& owner, GlobalVariables* theContext)
{ this->MakeZero(owner);
  MonomialUniversalEnvelopingOrdered<coefficient> tempMon;
//  tempMon.MakeZero(theCoeff.NumVars, owner, theContext);
  tempMon.Coefficient=theCoeff;
  tempMon.MultiplyByGeneratorPowerOnTheRight(theIndex, tempMon.Coefficient);
  this->AddOnTop(tempMon);
}

template<class coefficient>
void ElementVermaModuleOrdered<coefficient>::GetBasisFromSpanOfElements
(List<ElementVermaModuleOrdered>& theElements, Vectors<RationalFunctionOld>& outputCoordinates, List<ElementVermaModuleOrdered>& outputTheBasis,
 const RationalFunctionOld& RFOne, const RationalFunctionOld& RFZero)
{ List<ElementUniversalEnvelopingOrdered<coefficient> > theEltsUEform;
  theEltsUEform.SetSize(theElements.size);
  for (int i=0; i<theElements.size; i++)
    theEltsUEform.TheObjects[i]=theElements.TheObjects[i].theElT;
  List<ElementUniversalEnvelopingOrdered<coefficient> > theBasisUEform;
  ElementUniversalEnvelopingOrdered<coefficient>::GetBasisFromSpanOfElements(theEltsUEform, outputCoordinates, theBasisUEform, RFOne, RFZero, theGlobalVariables);
  outputTheBasis.SetSize(theBasisUEform.size);
  for (int i=0; i<theBasisUEform.size; i++)
  { outputTheBasis.TheObjects[i].theElT=theBasisUEform.TheObjects[i];
    outputTheBasis.TheObjects[i].theSubNthElementIsImageNthCoordSimpleBasis=theElements.TheObjects[0].theSubNthElementIsImageNthCoordSimpleBasis;
  }
}

template<class coefficient>
bool ElementVermaModuleOrdered<coefficient>::GetCoordsInBasis
(const List<ElementVermaModuleOrdered<coefficient> >& theBasis, Vector<coefficient>& output, const coefficient& theRingUnit, const coefficient& theRingZero)const
{ List<ElementUniversalEnvelopingOrdered<coefficient> > theEltsUEform;
  theEltsUEform.SetSize(theBasis.size);
  for (int i = 0; i < theBasis.size; i ++)
  { theEltsUEform.TheObjects[i] = theBasis.TheObjects[i].theElT;
  }
  return this->theElT.GetCoordsInBasis(theEltsUEform, output, theRingUnit, theRingZero, theGlobalVariables);
}

template<class coefficient>
bool ElementVermaModuleOrdered<coefficient>::NeedsParenthesisForMultiplication()const
{ return this->theElT.NeedsParenthesisForMultiplication();
}

template <class coefficient>
void ElementVermaModuleOrdered<coefficient>::AssignElementUniversalEnvelopingOrderedTimesHighestWeightVector
(ElementUniversalEnvelopingOrdered<coefficient>& input, const ElementVermaModuleOrdered<coefficient>& theRingZero, GlobalVariables* theContext, const coefficient& theRingUnit)
{ this->theElT.operator=(input);
  if (theRingZero.theSubNthElementIsImageNthCoordSimpleBasis.size != 3)
    crash << crash;
  this->theSubNthElementIsImageNthCoordSimpleBasis = theRingZero.theSubNthElementIsImageNthCoordSimpleBasis;
  this->theElT.ModOutVermaRelationSOld(false, this->theSubNthElementIsImageNthCoordSimpleBasis, theContext, theRingUnit);
}

template <class coefficient>
void ElementVermaModuleOrdered<coefficient>::MultiplyOnTheLeft
(const ElementSemisimpleLieAlgebra<Rational>& other, ElementVermaModuleOrdered<coefficient>& output, const coefficient& theRingUnit,
 const coefficient& theRingZero, GlobalVariables* theContext)
{ ElementUniversalEnvelopingOrdered<coefficient> tempElt;
  tempElt.AssignElementLieAlgebra(other, theRingUnit, theRingZero, *this->theElT.owner);
  //stOutput << "<br>multiplying " << tempElt.ToString() << " times " << this->ToString();
  tempElt *= this->theElT;
  output.theElT = tempElt;
  //stOutput << "<br> ... and the result before simplifying is: " << output.theElT.ToString();
  output.theElT.Simplify(theContext, theRingUnit, theRingZero);
  //stOutput << "<br> before modding out we get: " << output.theElT.ToString();
  output.theElT.ModOutVermaRelationSOld(false, this->theSubNthElementIsImageNthCoordSimpleBasis, theContext, theRingUnit);
  //stOutput << "<br> finally we get: " << output.theElT.ToString();
}

template <class coefficient>
std::string ElementVermaModuleOrdered<coefficient>::ToString(const FormatExpressions& theFormat)const
{ std::stringstream out;
  std::string tempS = MathRoutines::ElementToStringBrackets(this->theElT, theFormat);
  if (tempS.size() > 1)
    out << "(";
  if (tempS != "1")
    out << tempS;
  if (tempS.size() > 1)
    out << ")";
  if (tempS != "0")
    out << " v";
  return out.str();
}

template<class coefficient>
void ElementVermaModuleOrdered<coefficient>::ActOnMe
(const ElementSemisimpleLieAlgebra<Rational>& actingElt, ElementVermaModuleOrdered<coefficient>& output, SemisimpleLieAlgebra& owner,
 const coefficient& theRingUnit, const coefficient& theRingZero, GlobalVariables* theContext)const
{ ElementUniversalEnvelopingOrdered<coefficient> tempElt;
  tempElt.AssignElementLieAlgebra(actingElt, theRingUnit, theRingZero, *this->theElT.owner);
//  stOutput << "<br>" << actingElt.ToString() << " acts on " << this->ToString();
  tempElt.LieBracketOnTheRight(this->theElT, output.theElT);
  output.theElT.Simplify(theContext, theRingUnit, theRingZero);
  output.theSubNthElementIsImageNthCoordSimpleBasis = this->theSubNthElementIsImageNthCoordSimpleBasis;
//  stOutput << "<br>and the result before modding out is: " << output.ToString();
//  int numVars=output.theElt.GetNumVars();
  output.theElT.ModOutVermaRelationSOld(false, this->theSubNthElementIsImageNthCoordSimpleBasis, theContext, theRingUnit);
//  stOutput << "<br>and after modding out we get: " << output.ToString();
}

template<class coefficient>
template<class CoefficientTypeQuotientField>
void ElementUniversalEnvelopingOrdered<coefficient>::GetBasisFromSpanOfElements
(List<ElementUniversalEnvelopingOrdered<coefficient> >& theElements, Vectors<CoefficientTypeQuotientField>& outputCoords, List<ElementUniversalEnvelopingOrdered<coefficient> >& outputTheBasis,
 const CoefficientTypeQuotientField& theFieldUnit, const CoefficientTypeQuotientField& theFieldZero)
{ if (theElements.size == 0)
    return;
  ElementUniversalEnvelopingOrdered<coefficient> outputCorrespondingMonomials;
  outputCorrespondingMonomials.MakeZero(*theElements[0].owner);
  Vectors<CoefficientTypeQuotientField> outputCoordsBeforeReduction;
  for (int i = 0; i < theElements.size; i ++)
    for (int j = 0; j < theElements[i].size; j ++)
      outputCorrespondingMonomials.AddOnTopNoRepetition(theElements[i][j]);
  outputCoordsBeforeReduction.SetSize(theElements.size);
  for (int i = 0; i < theElements.size; i ++)
  { Vector<CoefficientTypeQuotientField>& currentList = outputCoordsBeforeReduction[i];
    currentList.MakeZero(outputCorrespondingMonomials.size, theFieldZero);
    ElementUniversalEnvelopingOrdered<coefficient>& currentElt = theElements[i];
    for (int j = 0; j < currentElt.size; j ++)
    { MonomialUniversalEnvelopingOrdered<coefficient>& currentMon = currentElt[j];
      currentList.TheObjects[outputCorrespondingMonomials.GetIndex(currentMon)] = currentMon.Coefficient;
    }
  }
  outputTheBasis.size = 0;
  outputTheBasis.Reserve(theElements.size);
  Vectors<CoefficientTypeQuotientField> basisCoordForm;
  basisCoordForm.Reserve(theElements.size);
  Selection selectedBasis;
  outputCoordsBeforeReduction.ComputeDebugString();
  outputCoordsBeforeReduction.SelectABasis(basisCoordForm, theFieldZero, selectedBasis, theGlobalVariables);
  for (int i = 0; i < selectedBasis.CardinalitySelection; i ++)
    outputTheBasis.AddOnTop(theElements[selectedBasis.elements[i]]);
  Matrix<coefficient> bufferMat;
  Vectors<coefficient> bufferVectors;
  outputCoordsBeforeReduction.GetCoordsInBasis(basisCoordForm, outputCoords, bufferVectors, bufferMat, theFieldUnit, theFieldZero);
}

template <class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::Simplify(GlobalVariables* theContext, const coefficient& theRingUnit, const coefficient& theRingZero)
{ ElementUniversalEnvelopingOrdered buffer;
  ElementUniversalEnvelopingOrdered output;
  //this->ComputeDebugString();
  output.MakeZero(*this->owner);
  for (int i = 0; i < this->size; i ++)
  { this->TheObjects[i].Simplify(buffer, theContext, theRingUnit, theRingZero);
    output += buffer;
    //output.ComputeDebugString();
  }
  *this = output;
}

template <class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::MakeZero(SemisimpleLieAlgebraOrdered& theOwner)
{ this->Clear();
  this->owner = &theOwner;
}

template <class coefficient>
void MonomialUniversalEnvelopingOrdered<coefficient>::Simplify
(ElementUniversalEnvelopingOrdered<coefficient>& output, GlobalVariables* theContext, const coefficient& theRingUnit,  const coefficient& theRingZero)
{ output.MakeZero(*this->owner);
  output.AddOnTop(*this);
  this->SimplifyAccumulateInOutputNoOutputInit(output, theContext, theRingUnit, theRingZero);
}

template <class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::CleanUpZeroCoeff()
{ for (int i = 0; i < this->size; i ++)
    if (this->TheObjects[i].Coefficient.IsEqualToZero())
    { this->RemoveIndexSwapWithLast(i);
      i --;
    }
}

template <class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::operator+=(const ElementUniversalEnvelopingOrdered<coefficient>& other)
{ this->Reserve(this->size + other.size);
  for (int i = 0; i < other.size; i ++)
    this->AddMonomialNoCleanUpZeroCoeff(other[i]);
  this->CleanUpZeroCoeff();
}

template <class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::operator-=(const ElementUniversalEnvelopingOrdered<coefficient>& other)
{ this->Reserve(this->size+other.size);
  MonomialUniversalEnvelopingOrdered<coefficient> tempMon;
  for (int i = 0; i < other.size; i ++)
  { tempMon = other.TheObjects[i];
    tempMon.Coefficient *= - 1;
    this->AddMonomialNoCleanUpZeroCoeff(tempMon);
  }
  this->CleanUpZeroCoeff();
}

template <class coefficient>
template <class otherType>
void ElementUniversalEnvelopingOrdered<coefficient>::operator*=(const otherType& other)
{ if (other.IsEqualToZero())
  { this->MakeZero(*this->owner);
    return;
  }
  for (int i = 0; i < this->size; i ++)
    this->TheObjects[i].Coefficient.operator*=(other);
}

template <class coefficient>
template <class SecondType>
void ElementUniversalEnvelopingOrdered<coefficient>::operator/=(const SecondType& other)
{ for (int i = 0; i < this->size; i ++)
    this->TheObjects[i].Coefficient.operator/=(other);
}

template <class coefficient>
void MonomialUniversalEnvelopingOrdered<coefficient>::MultiplyByNoSimplify(const MonomialUniversalEnvelopingOrdered& other)
{ if (this == &other)
    crash << crash;
  this->generatorsIndices.Reserve(other.generatorsIndices.size + this->generatorsIndices.size);
  this->Powers.Reserve(other.generatorsIndices.size + this->generatorsIndices.size);
  this->Coefficient.MultiplyBy(other.Coefficient);
  if (other.generatorsIndices.size == 0)
    return;
  int firstIndex = other.generatorsIndices[0];
  int i = 0;
//  std::string tempS;
//  tempS=other.ToString();
  if (this->generatorsIndices.size > 0)
    if (firstIndex == (*this->generatorsIndices.LastObject()))
    { //this->ComputeDebugString();
      *this->Powers.LastObject() += other.Powers[0];
      i = 1;
    }
  for (; i < other.generatorsIndices.size; i ++)
  { this->Powers.AddOnTop(other.Powers[i]);
    this->generatorsIndices.AddOnTop(other.generatorsIndices[i]);
  }
}

template <class coefficient>
bool ElementUniversalEnvelopingOrdered<coefficient>::IsProportionalTo
(const ElementUniversalEnvelopingOrdered<coefficient>& other, coefficient& outputTimesMeEqualsOther, const coefficient& theRingZero)const
{ if (this->IsEqualToZero())
  { if (other.IsEqualToZero())
      return true;
    return false;
  }
  if (other.IsEqualToZero())
  { outputTimesMeEqualsOther = theRingZero;
    return true;
  }
  if (other.size != this->size)
    return false;
  MonomialUniversalEnvelopingOrdered<coefficient>& theMon = (*this)[0];
  int theIndex = other.GetIndex(theMon);
  if (theIndex == - 1)
    return false;
  MonomialUniversalEnvelopingOrdered<coefficient>& otherMon = other[theIndex];
  theMon.ComputeDebugString();
  otherMon.ComputeDebugString();
  outputTimesMeEqualsOther = otherMon.Coefficient;
  outputTimesMeEqualsOther /= theMon.Coefficient;
  ElementUniversalEnvelopingOrdered<coefficient> tempElt;

  tempElt = *this;
  tempElt *= outputTimesMeEqualsOther;
  tempElt -= other;
  return tempElt.IsEqualToZero();
}

template<class coefficient>
void MonomialUniversalEnvelopingOrdered<coefficient>::operator*=(const MonomialUniversalEnvelopingOrdered& other)
{ if (this == &other)
    crash << "Output and input are the same. " << crash;
  this->Coefficient *= other.Coefficient;
  for (int i = 0; i < other.generatorsIndices.size; i ++)
    this->MultiplyByGeneratorPowerOnTheRight(other.generatorsIndices[i], other.Powers[i]);
}

template <class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::operator*=(const ElementUniversalEnvelopingOrdered<coefficient>& other)
{ ElementUniversalEnvelopingOrdered output;
  output.MakeZero(*this->owner);
  output.Reserve(this->size * other.size);
  MonomialUniversalEnvelopingOrdered<coefficient> tempMon;
  for (int i = 0; i < this->size; i ++)
    for (int j = 0; j < other.size; j ++)
    { tempMon = (*this)[i];
      tempMon *= other[j];
      output.AddMonomial(tempMon);
    }
  *this = output;
}

template <class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::AddMonomialNoCleanUpZeroCoeff(const MonomialUniversalEnvelopingOrdered<coefficient>& input)
{ //MonomialUniversalEnvelopingOrdered<coefficient> tempMon;
  //tempMon=input;
  //tempMon.ComputeDebugString();
  //this->ComputeDebugString();
  int theIndex = this->GetIndex(input);
  if (theIndex == - 1)
    this->AddOnTop(input);
  else
    this->TheObjects[theIndex].Coefficient += input.Coefficient;
}

template <class coefficient>
void MonomialUniversalEnvelopingOrdered<coefficient>::SimplifyAccumulateInOutputNoOutputInit
(ElementUniversalEnvelopingOrdered<coefficient>& output, GlobalVariables* theContext, const coefficient& theRingUnit, const coefficient& theRingZero)
{ int IndexlowestNonSimplified = 0;
  ElementUniversalEnvelopingOrdered<coefficient> buffer2;
  MonomialUniversalEnvelopingOrdered<coefficient> tempMon;
  //simplified order is descending order
  while (IndexlowestNonSimplified < output.size)
  { bool reductionOccurred = false;
    if (output[IndexlowestNonSimplified].Coefficient.IsEqualToZero())
      reductionOccurred = true;
    else
      for (int i = 0; i < output[IndexlowestNonSimplified].generatorsIndices.size - 1; i ++)
        if (output[IndexlowestNonSimplified].generatorsIndices[i] > output[IndexlowestNonSimplified].generatorsIndices[i + 1])
        { if (output[IndexlowestNonSimplified].SwitchConsecutiveIndicesIfTheyCommute(i, tempMon, theContext, theRingZero))
          { output.AddMonomialNoCleanUpZeroCoeff(tempMon);
//            tempMon.ComputeDebugString();
            reductionOccurred = true;
            break;
          }
          if (this->CommutingRightIndexAroundLeftIndexAllowed(output[IndexlowestNonSimplified].Powers[i], output[IndexlowestNonSimplified].generatorsIndices[i], output[IndexlowestNonSimplified].Powers[i + 1], output[IndexlowestNonSimplified].generatorsIndices[i + 1]))
          { output.TheObjects[IndexlowestNonSimplified].CommuteConsecutiveIndicesRightIndexAroundLeft(i, buffer2, theContext, theRingUnit, theRingZero);
            for (int j = 0; j < buffer2.size; j ++)
              output.AddMonomialNoCleanUpZeroCoeff(buffer2[j]);
//            output.ComputeDebugString();
            reductionOccurred = true;
            break;
          }
          if (this->CommutingLeftIndexAroundRightIndexAllowed(output[IndexlowestNonSimplified].Powers[i], output[IndexlowestNonSimplified].generatorsIndices[i], output[IndexlowestNonSimplified].Powers[i + 1], output.TheObjects[IndexlowestNonSimplified].generatorsIndices[i + 1]))
          { output[IndexlowestNonSimplified].CommuteConsecutiveIndicesLeftIndexAroundRight(i, buffer2, theContext, theRingUnit, theRingZero);
            for (int j = 0; j < buffer2.size; j ++)
              output.AddMonomialNoCleanUpZeroCoeff(buffer2[j]);
//            output.ComputeDebugString();
            reductionOccurred=true;
            break;
          }
        }
    if (reductionOccurred)
      output.RemoveIndexSwapWithLast(IndexlowestNonSimplified);
    else
      IndexlowestNonSimplified ++;
//    output.ComputeDebugString();
  }
  output.CleanUpZeroCoeff();
}

template <class coefficient>
bool MonomialUniversalEnvelopingOrdered<coefficient>::SwitchConsecutiveIndicesIfTheyCommute
(int theLeftIndex, MonomialUniversalEnvelopingOrdered<coefficient>& output, GlobalVariables* theContext, const coefficient& theRingZero)
{ if (theLeftIndex >= this->generatorsIndices.size - 1)
    return false;
  int theLeftGeneratorIndex = this->generatorsIndices[theLeftIndex];
  int theRightGeneratorIndex = this->generatorsIndices[theLeftIndex + 1];
  ElementSemisimpleLieAlgebra<Rational> tempElt;
  this->owner->theOwner->LieBracket(this->owner->theOrder[theLeftGeneratorIndex], this->owner->theOrder[theRightGeneratorIndex], tempElt);
  if (tempElt.IsEqualToZero())
  { output.generatorsIndices.Reserve(this->generatorsIndices.size);
    output.Powers.Reserve(this->generatorsIndices.size);
    output.MakeZero(theRingZero, *this->owner);
    output.Coefficient = this->Coefficient;
    //output.ComputeDebugString();
    for (int i = 0; i < theLeftIndex; i ++)
      output.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
    output.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[theLeftIndex + 1], this->Powers.TheObjects[theLeftIndex + 1]);
    output.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[theLeftIndex], this->Powers.TheObjects[theLeftIndex]);
    for (int i = theLeftIndex + 2; i < this->generatorsIndices.size; i ++)
      output.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
    return true;
  }
  return false;
}

template <class coefficient>
bool MonomialUniversalEnvelopingOrdered<coefficient>::CommutingRightIndexAroundLeftIndexAllowed(coefficient& theLeftPower, int leftGeneratorIndex, coefficient& theRightPower, int rightGeneratorIndex)
{ return this->CommutingLeftIndexAroundRightIndexAllowed(theRightPower, rightGeneratorIndex, theLeftPower, leftGeneratorIndex);
}

template <class coefficient>
void MonomialUniversalEnvelopingOrdered<coefficient>::CommuteConsecutiveIndicesRightIndexAroundLeft
(int theIndeX, ElementUniversalEnvelopingOrdered<coefficient>& output, GlobalVariables* theContext, const coefficient& theRingUnit, const coefficient& theRingZero)
{ if (theIndeX == this->generatorsIndices.size - 1)
    return;
  output.MakeZero(*this->owner);
  MonomialUniversalEnvelopingOrdered tempMon;
  tempMon.MakeZero(theRingZero, *this->owner);
  tempMon.Powers.Reserve(this->generatorsIndices.size + 2);
  tempMon.generatorsIndices.Reserve(this->generatorsIndices.size + 2);
  tempMon.Powers.size = 0;
  tempMon.generatorsIndices.size = 0;
  int rightGeneratorIndeX = this->generatorsIndices.TheObjects[theIndeX + 1];
  int leftGeneratorIndeX = this->generatorsIndices.TheObjects[theIndeX];
  coefficient theRightPoweR, theLeftPoweR;
  theRightPoweR = this->Powers.TheObjects[theIndeX + 1];
  theLeftPoweR = this->Powers.TheObjects[theIndeX];
  theRightPoweR -= 1;
  int powerDroP = 0;
//  if (this->flagAnErrorHasOccurredTimeToPanic)
//  if (this->ToString()=="2f_{5}f_{-5}f_{-4}" || this->ToString()=="2f_{11}f_{-4}")
//  { stOutput << "here we are!";
//    this->flagAnErrorHasOccurredTimeToPanic=true;
//  }

  coefficient acquiredCoefficienT;
  acquiredCoefficienT = this->Coefficient;
  tempMon.Coefficient = this->Coefficient;
  for (int i = 0; i < theIndeX; i ++)
    tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices[i], this->Powers[i]);
  MonomialUniversalEnvelopingOrdered startMon;
  startMon = tempMon;
  ElementSemisimpleLieAlgebra<Rational> adResulT, tempElT, theLeftElt;
  this->owner->AssignGeneratorCoeffOne(rightGeneratorIndeX, adResulT);
  this->owner->AssignGeneratorCoeffOne(leftGeneratorIndeX, theLeftElt);
  //tempLefttElt.ComputeDebugString(*this->owner, false, false);
  Vector<Rational> theCoeffs;
  do
  { this->owner->GetLinearCombinationFrom(adResulT, theCoeffs);
    for (int i = 0; i < theCoeffs.size; i ++)
      if (theCoeffs[i] != 0)
      { int theNewGeneratorIndex = i;
        tempMon = startMon;
        if (this->flagAnErrorHasOccurredTimeToPanic)
        { tempMon.ComputeDebugString();
          this->ComputeDebugString();
        }
        tempMon.Coefficient = acquiredCoefficienT;
        if (this->flagAnErrorHasOccurredTimeToPanic)
        { tempMon.ComputeDebugString();
          this->ComputeDebugString();
        }
        tempMon.Coefficient *= theCoeffs[i];
        if (this->flagAnErrorHasOccurredTimeToPanic)
        { tempMon.ComputeDebugString();
          this->ComputeDebugString();
        }
        tempMon.MultiplyByGeneratorPowerOnTheRight(theNewGeneratorIndex, theRingUnit);
        tempMon.MultiplyByGeneratorPowerOnTheRight(leftGeneratorIndeX, theLeftPoweR);
        tempMon.MultiplyByGeneratorPowerOnTheRight(rightGeneratorIndeX, theRightPoweR);
        for (int i = theIndeX + 2; i < this->generatorsIndices.size; i ++)
          tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
        if (this->flagAnErrorHasOccurredTimeToPanic)
        { tempMon.ComputeDebugString();
          this->ComputeDebugString();
        }
        output.AddOnTop(tempMon);
      }
    acquiredCoefficienT *= theLeftPoweR;
    theLeftPoweR -= 1;
    this->owner->theOwner->LieBracket(theLeftElt, adResulT, tempElT);
    adResulT = tempElT;
    powerDroP ++;
    acquiredCoefficienT /= powerDroP;
//    if (this->flagAnErrorHasOccurredTimeToPanic)
//      adResulT.ComputeDebugString(false, false);
  } while(!adResulT.IsEqualToZero() && !acquiredCoefficienT.IsEqualToZero());
}

template <class coefficient>
void MonomialUniversalEnvelopingOrdered<coefficient>::CommuteConsecutiveIndicesLeftIndexAroundRight
(int theIndeX, ElementUniversalEnvelopingOrdered<coefficient>& output, GlobalVariables* theContext, const coefficient& theRingUnit, const coefficient& theRingZero)
{ if (theIndeX == this->generatorsIndices.size - 1)
    return;
  output.MakeZero(*this->owner);
  MonomialUniversalEnvelopingOrdered tempMon;
  tempMon.MakeZero(theRingZero, *this->owner);
  tempMon.Powers.SetExpectedSize(this->generatorsIndices.size + 2);
  tempMon.generatorsIndices.SetExpectedSize(this->generatorsIndices.size + 2);
  tempMon.Powers.size = 0;
  tempMon.generatorsIndices.size = 0;
  int rightGeneratorIndex = this->generatorsIndices.TheObjects[theIndeX + 1];
  int leftGeneratorIndex = this->generatorsIndices.TheObjects[theIndeX];
  coefficient theRightPower, theLeftPower;
  theRightPower = this->Powers.TheObjects[theIndeX + 1];
  theLeftPower = this->Powers.TheObjects[theIndeX];
  theLeftPower -= 1;
  int powerDrop = 0;
 /* if (this->ToString()=="2f_{5}f_{-5}f_{-4}" || this->ToString()=="2f_{11}f_{-4}")
  { stOutput << "here we are!";
    this->flagAnErrorHasOccurredTimeToPanic=true;
  }*/

  coefficient acquiredCoefficient;
  acquiredCoefficient = this->Coefficient;
  tempMon.Coefficient = this->Coefficient;
  for (int i = 0; i < theIndeX; i ++)
    tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[i], this->Powers.TheObjects[i]);
  tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices.TheObjects[theIndeX], theLeftPower);
  MonomialUniversalEnvelopingOrdered startMon, tempMon2;
  startMon=tempMon;
  ElementSemisimpleLieAlgebra<Rational> adResult, tempElt, tempRightElt;
  this->owner->AssignGeneratorCoeffOne(leftGeneratorIndex, adResult);
  this->owner->AssignGeneratorCoeffOne(rightGeneratorIndex, tempRightElt);
//  tempRightElt.ComputeDebugString(*this->owner, false, false);
  Vector<Rational> theCoeffs;
  do
  { //acquiredCoefficient.ComputeDebugString();
    //theRightPower.ComputeDebugString();
    //adResult.ComputeDebugString(*this->owner, false, false);
    //tempMon.ComputeDebugString();
    //tempMon.ComputeDebugString();
    this->owner->GetLinearCombinationFrom(adResult, theCoeffs);
    for (int i = 0; i < theCoeffs.size; i ++)
      if (theCoeffs[i] != 0)
      { int theNewGeneratorIndex = i;
        tempMon = startMon;
        tempMon.MultiplyByGeneratorPowerOnTheRight(rightGeneratorIndex, theRightPower);
        tempMon.Coefficient = acquiredCoefficient;
        tempMon.Coefficient *= theCoeffs[i];
        tempMon.MultiplyByGeneratorPowerOnTheRight(theNewGeneratorIndex, theRingUnit);
        for (int i = theIndeX + 2; i < this->generatorsIndices.size; i ++)
          tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices[i], this->Powers[i]);
        output.AddOnTop(tempMon);
      }
    acquiredCoefficient *= theRightPower;
    theRightPower -= 1;
    this->owner->theOwner->LieBracket(adResult, tempRightElt, tempElt);
    adResult = tempElt;
    powerDrop ++;
    acquiredCoefficient /= powerDrop;
    //adResult.ComputeDebugString(*this->owner, false, false);
  } while(!adResult.IsEqualToZero() && !acquiredCoefficient.IsEqualToZero());
}

template <class coefficient>
bool MonomialUniversalEnvelopingOrdered<coefficient>::CommutingLeftIndexAroundRightIndexAllowed(coefficient& theLeftPower, int leftGeneratorIndex, coefficient& theRightPower, int rightGeneratorIndex)
{ int tempInt;
  if (theLeftPower.IsSmallInteger(&tempInt))
  { if (theRightPower.IsSmallInteger(&tempInt))
      return true;
    int numPosRoots = this->owner->theOwner->theWeyl.RootsOfBorel.size;
    int theDimension = this->owner->theOwner->theWeyl.CartanSymmetric.NumRows;
    if (rightGeneratorIndex >= numPosRoots && rightGeneratorIndex < numPosRoots + theDimension)
    { ElementSemisimpleLieAlgebra<Rational> tempElt;
      this->owner->theOwner->LieBracket(this->owner->theOrder[leftGeneratorIndex], this->owner->theOrder[rightGeneratorIndex], tempElt);
      if (tempElt.IsEqualToZero())
        return true;
      else
        return false;
    } else
      return true;
  }
  return false;
}

template <class coefficient>
void MonomialUniversalEnvelopingOrdered<coefficient>::MakeZero(int numVars, SemisimpleLieAlgebraOrdered& theOwner, GlobalVariables* theContext)
{ this->Coefficient.MakeZero((int) numVars, theContext);
  this->owner = &theOwner;
  this->generatorsIndices.size = 0;
  this->Powers.size = 0;
}

template <class coefficient>
void MonomialUniversalEnvelopingOrdered<coefficient>::MakeZero(const coefficient& theRingZero, SemisimpleLieAlgebraOrdered& theOwner)
{ this->Coefficient = theRingZero;
  this->owner = &theOwner;
  this->generatorsIndices.size = 0;
  this->Powers.size = 0;
}

template <class coefficient>
unsigned int MonomialUniversalEnvelopingOrdered<coefficient>::HashFunction() const
{ int top = MathRoutines::Minimum(SomeRandomPrimesSize, this->generatorsIndices.size);
  int result = 0;
  for (int i = 0; i < top; i ++)
    result += SomeRandomPrimes[i] * this->generatorsIndices.TheObjects[i];
  return result;
}

template <class coefficient>
void MonomialUniversalEnvelopingOrdered<coefficient>::MultiplyByGeneratorPowerOnTheRight(int theGeneratorIndex, int thePower)
{ if (thePower == 0)
    return;
  Polynomial<Rational> tempP;
  tempP.MakeConst(thePower);
  this->MultiplyByGeneratorPowerOnTheRight(theGeneratorIndex, tempP);
}

template <class coefficient>
void MonomialUniversalEnvelopingOrdered<coefficient>::MultiplyByGeneratorPowerOnTheRight(int theGeneratorIndex, const coefficient& thePower)
{ if (thePower.IsEqualToZero())
    return;
  if (this->generatorsIndices.size > 0)
    if (*this->generatorsIndices.LastObject() == theGeneratorIndex)
    { (*this->Powers.LastObject()) += thePower;
      return;
    }
  this->Powers.AddOnTop(thePower);
  this->generatorsIndices.AddOnTop(theGeneratorIndex);
}

template <class coefficient>
std::string MonomialUniversalEnvelopingOrdered<coefficient>::ToString(bool useLatex, bool useCalculatorFormat, FormatExpressions* PolyFormatLocal)const
{ if (this->owner == 0)
    return "faulty monomial non-initialized owner. Slap the programmer.";
  if (this->IsEqualToZero())
    return "0";
  std::stringstream out;
  std::string tempS;
  if (this->generatorsIndices.size > 0)
  { tempS = MathRoutines::ElementToStringBrackets(this->Coefficient, PolyFormatLocal);
    if (tempS == "1")
      tempS = "";
    if (tempS == "-1")
      tempS = "-";
  } else
    tempS = this->Coefficient.ToString(PolyFormatLocal);
  out << tempS;
  for (int i = 0; i < this->generatorsIndices.size; i++)
  { coefficient& thePower = this->Powers[i];
    int theIndex = this->generatorsIndices[i];
    bool usebrackets = false;
    tempS = this->owner->theOwner->GetStringFromChevalleyGenerator(theIndex, PolyFormatLocal);
    if (usebrackets)
      out << "(";
    out << tempS;
    if (usebrackets)
      out << ")";
    tempS = thePower.ToString(PolyFormatLocal);
    if (tempS != "1")
    { out << "^";
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
void ElementUniversalEnvelopingOrdered<coefficient>::ToString
(std::string& output, bool useLatex, bool useCalculatorFormat, FormatExpressions& PolyFormatLocal)const
{ std::stringstream out;
  std::string tempS;
  if (this->size == 0)
    out << "0";
  int IndexCharAtLastLineBreak = 0;
  for (int i = 0; i < this->size; i ++)
  { MonomialUniversalEnvelopingOrdered<coefficient>& current = this->TheObjects[i];
    tempS = current.ToString(false, useCalculatorFormat, &PolyFormatLocal, theGlobalVariables);
    if (i != 0)
      if (tempS.size() > 0)
        if (tempS[0] != '-')
          out << '+';
    out << tempS;
    if (((int) out.tellp())- IndexCharAtLastLineBreak > 150)
    { IndexCharAtLastLineBreak = out.tellp();
      out << "\\\\&&";
    }
  }
  output = out.str();
}

template <class coefficient>
bool ElementUniversalEnvelopingOrdered<coefficient>::AssignElementUniversalEnveloping
(ElementUniversalEnveloping<coefficient>& input, SemisimpleLieAlgebraOrdered& owner, const coefficient& theRingUnit, const coefficient& theRingZero, GlobalVariables* theContext)
{ ElementUniversalEnvelopingOrdered<coefficient> tempElt;
  this->MakeZero(owner);
  for (int i = 0; i < input.size; i ++)
  { if(!tempElt.AssignMonomialUniversalEnveloping
        (input.TheObjects[i], input.theCoeffs[i], owner, theRingUnit, theRingZero, theContext))
      return false;
    this->operator+=(tempElt);
  }
  this->Simplify(theContext, theRingUnit, theRingZero);
  return true;
}

template <class coefficient>
bool ElementUniversalEnvelopingOrdered<coefficient>::AssignMonomialUniversalEnveloping
(MonomialUniversalEnveloping<coefficient>& input, const coefficient& inputCoeff, SemisimpleLieAlgebraOrdered& owner, const coefficient& theRingUnit,
 const coefficient& theRingZero, GlobalVariables* theContext)
{ ElementUniversalEnvelopingOrdered theMon;
  ElementSemisimpleLieAlgebra<Rational> tempElt;
  coefficient theCoeff;
  theCoeff = inputCoeff;
  this->MakeConst(theCoeff, owner);
  //stOutput << "<br>after initialization with constant I am " << this->ToString();
  for (int i = 0; i < input.generatorsIndices.size; i ++)
  { int thePower;
    bool isASmallInt = input.Powers.TheObjects[i].IsSmallInteger(&thePower);
    if (isASmallInt)
    { tempElt.MakeGenerator
      (input.generatorsIndices.TheObjects[i], *input.owners, input.indexInOwners);
      theMon.AssignElementLieAlgebra(tempElt, theRingUnit, theRingZero, owner);
      //stOutput << "<br>raising " << theMon.ToString() << " to power " << thePower;
      theMon.RaiseToPower(thePower, theRingUnit);
      //stOutput << " to obtain " << theMon.ToString();
    }
    else
      return false;
    this->MultiplyBy(theMon);

  }
  //this->Simplify(theContext);
  return true;
}

template <class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::AssignElementLieAlgebra
(const ElementSemisimpleLieAlgebra<Rational>& input, const coefficient& theRingUnit, const coefficient& theRingZero, SemisimpleLieAlgebraOrdered& theOwner)
{ this->MakeZero(theOwner);
  Vector<Rational> ElementRootForm;
  input.ElementToVectorNegativeRootSpacesFirst(ElementRootForm);
  theOwner.ChevalleyGeneratorsInCurrentCoords.ActOnVectorColumn(ElementRootForm);
  MonomialUniversalEnvelopingOrdered<coefficient> tempMon;
  tempMon.MakeZero(theRingZero, theOwner);
  tempMon.generatorsIndices.SetSize(1);
  tempMon.Powers.SetSize(1);
  tempMon.Powers.TheObjects[0] = theRingUnit;
  for (int theIndex = 0; theIndex < ElementRootForm.size; theIndex ++)
    if (ElementRootForm.TheObjects[theIndex] != 0)
    { tempMon.Coefficient = theRingUnit;
      tempMon.Coefficient *= ElementRootForm.TheObjects[theIndex];
      tempMon.generatorsIndices[0] = theIndex;
      this->AddOnTop(tempMon);
    }
}

template <class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::RaiseToPower(int thePower, const coefficient& theRingUnit)
{ if (this->size == 0)
    return;
  ElementUniversalEnvelopingOrdered<coefficient> buffer;
  buffer.operator=(*this);
  this->MakeConst(theRingUnit, *this->owner);
  //stOutput << "<br>raising " <<buffer.ToString() << " to power " << thePower;
  for (int i = 0; i < thePower; i ++)
    this->operator*=(buffer);
  //stOutput << "<br> and the result is " << this->ToString();
}

template <class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::LieBracketOnTheRight(const ElementSemisimpleLieAlgebra<Rational>& right, const coefficient& ringUnit, const coefficient& ringZero)
{ ElementUniversalEnvelopingOrdered<coefficient> tempElt;
  tempElt.AssignElementLieAlgebra(right, ringUnit, ringZero, *this->owner);
  this->LieBracketOnTheRight(tempElt, *this);
}

template <class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::LieBracketOnTheRight(const ElementUniversalEnvelopingOrdered& right, ElementUniversalEnvelopingOrdered& output)
{ ElementUniversalEnvelopingOrdered tempElt, tempElt2;
  tempElt = *this;
  tempElt *= right;
  tempElt2 = right;
  tempElt2 *= *this;
  output = tempElt;
  output -= tempElt2;
}

template<class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::AddMonomial(const MonomialUniversalEnvelopingOrdered<coefficient>& input)
{ if (input.IsEqualToZero())
    return;
  int theIndex = this->GetIndex(input);
  if (theIndex == - 1)
    this->AddOnTop(input);
  else
  { this->TheObjects[theIndex].Coefficient += input.Coefficient;
    if (this->TheObjects[theIndex].Coefficient.IsEqualToZero())
      this->RemoveIndexSwapWithLast(theIndex);
  }
}

template <class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::ModOutVermaRelationSOld
(bool SubHighestWeightWithZeroes, const PolynomialSubstitution<Rational>& highestWeightSub, GlobalVariables* theContext, const coefficient& theRingUnit)
{ MonomialUniversalEnvelopingOrdered<coefficient> tempMon;
  ElementUniversalEnvelopingOrdered<coefficient> output;
  output.MakeZero(*this->owner);
  for (int i = 0; i < this->size; i ++)
  { tempMon = this->TheObjects[i];
//    tempMon.ComputeDebugString();
    tempMon.ModOutVermaRelationSOld(SubHighestWeightWithZeroes, highestWeightSub, theContext, theRingUnit);
//    tempMon.ComputeDebugString();
    output.AddMonomial(tempMon);
  }
  this->operator=(output);
}

template <class coefficient>
void ElementUniversalEnvelopingOrdered<coefficient>::ModOutVermaRelations
(GlobalVariables* theContext, const List<coefficient>* subHiGoesToIthElement, const coefficient& theRingUnit, const coefficient& theRingZero)
{ MonomialUniversalEnvelopingOrdered<coefficient> tempMon;
  ElementUniversalEnvelopingOrdered<coefficient> output;
  output.MakeZero(*this->owner);
  for (int i = 0; i < this->size; i ++)
  { tempMon = this->TheObjects[i];
//    tempMon.ComputeDebugString();
    tempMon.ModOutVermaRelations(theContext, subHiGoesToIthElement, theRingUnit, theRingZero);
//    tempMon.ComputeDebugString();
    output.AddMonomial(tempMon);
  }
  this->operator=(output);
}

template <class coefficient>
void MonomialUniversalEnvelopingOrdered<coefficient>::ModOutVermaRelations
(GlobalVariables* theContext, const List<coefficient>* subHiGoesToIthElement, const coefficient& theRingUnit, const coefficient& theRingZero)
{ int numPosRoots = this->owner->theOwner->GetNumPosRoots();
  int theDimension = this->owner->theOwner->GetRank();
  for (int i = this->generatorsIndices.size - 1; i >= 0; i --)
  { int IndexCurrentGenerator = this->generatorsIndices[i];
    if (IndexCurrentGenerator >= numPosRoots + theDimension)
    { this->MakeZero(theRingZero, *this->owner);
      return;
    }
    if (IndexCurrentGenerator < numPosRoots)
      return;
    if (IndexCurrentGenerator >= numPosRoots &&  IndexCurrentGenerator < numPosRoots + theDimension)
    { int theDegree;
      if (!this->Powers[i].IsSmallInteger(theDegree))
        return;
      if (subHiGoesToIthElement == 0)
      { this->MakeZero(theRingZero, *this->owner);
        return;
      }
      coefficient theSubbedH = theRingZero;
      Vector<Rational> currentH = this->owner->theOrder[IndexCurrentGenerator].GetCartanPart();
      for (int j = 0; j < currentH.size; j ++)
        theSubbedH += (*subHiGoesToIthElement)[j] * currentH[j];
      MathRoutines::RaiseToPower(theSubbedH, theDegree, theRingUnit);
//      this->owner->theOrder.TheObjects[IndexCurrentGenerator].Hcomponent.ComputeDebugString();
      this->Coefficient *= theSubbedH;
      this->generatorsIndices.size --;
      this->Powers.size --;
    }
  }
}
#endif
