//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfImplementationHeaderSemisimpleLieAlgebrasIncluded
#define vpfImplementationHeaderSemisimpleLieAlgebrasIncluded

#include "vpfHeader2Math1_SemisimpleLieAlgebras.h"
static ProjectInformationInstance ProjectInfovpfImplementationHeaderSemisimpleLieAlgebras(__FILE__, "Implementation header, semisimple Lie algebra routines. ");

template <class coefficient>
std::string MonomialChar<coefficient>::ToString(FormatExpressions* theFormat)const
{ std::stringstream out;
  bool useOmega=true;
  std::string oldCustomPlus;
  std::string VectorSpaceLetter="V";
  if (theFormat!=0)
  { useOmega=(theFormat->fundamentalWeightLetter=="");
    oldCustomPlus=theFormat->CustomPlusSign;
    theFormat->CustomPlusSign="";
    VectorSpaceLetter=theFormat->FDrepLetter;
  }
  if (useOmega)
    out << VectorSpaceLetter << "_{" << this->weightFundamentalCoordS.ToStringLetterFormat("\\omega", theFormat) << "}";
  else
    out << VectorSpaceLetter << "_{"
    << this->weightFundamentalCoordS.ToStringLetterFormat(theFormat->fundamentalWeightLetter, theFormat) << "}";
  if (theFormat!=0)
    theFormat->CustomPlusSign=oldCustomPlus;
  return out.str();
}

template <class coefficient>
void MonomialChar<coefficient>::AccountSingleWeight
(const Vector<Rational>& currentWeightSimpleCoords, const Vector<Rational>& otherHighestWeightSimpleCoords,
 Rational& theMult, charSSAlgMod<coefficient>& outputAccum)const
{ //This is the Brauer-Klimyk formula. Reference:
  //Humphreys J., Introduction to Lie algebras and representation theory
  //page 142, exercise 9.
  //std::cout << "<hr>Accounting " << currentWeightSimpleCoords.ToString()
  //<< " with coefficient " << finalCoeff.ToString();
  MacroRegisterFunctionWithName("MonomialChar_CoefficientType::AccountSingleWeight");
  this->CheckNonZeroOwner();
  Vector<Rational> dominant=currentWeightSimpleCoords;
  dominant+=otherHighestWeightSimpleCoords;
  WeylGroup& theWeyl = this->owner->theWeyl;
  dominant+=theWeyl.rho;
  int sign;
  // a weight has no stabilizer if and only if it is not stabilized by all root reflections.
  for (int i=0; i<theWeyl.RootsOfBorel.size; i++)
    if (theWeyl.RootScalarCartanRoot(dominant, theWeyl.RootsOfBorel[i]).IsEqualToZero())
      return;
//  std::cout << "<br> Before raising to dominant, in simple coords: " << dominant.ToString() << "<br>";
  theWeyl.RaiseToDominantWeight(dominant, &sign);
//  std::cout << "After raising to dominant: " << dominant.ToString() << "<br>";
  dominant-=theWeyl.rho;
  if (!theWeyl.IsDominantWeight(dominant))
    return;
  MonomialChar<Rational> tempMon;
  tempMon.owner=this->owner;
  tempMon.weightFundamentalCoordS=theWeyl.GetFundamentalCoordinatesFromSimple(dominant);
  coefficient coeffChange;
  coeffChange= theMult;
  coeffChange*=sign;
//  std::cout << "; final contribution: " << tempMon.Coefficient.ToString()
//  << "*" << tempMon.weightFundamentalCoords.ToString() << "<br>";
  outputAccum.AddMonomial(tempMon, coeffChange);
}

template <class coefficient>
std::string MonomialChar<coefficient>::TensorAndDecompose
(const MonomialChar<coefficient>& other, charSSAlgMod<coefficient>& output, GlobalVariables& theGlobalVariables)const
{ //This is the Brauer-Klimyk formula. Reference:
  //Humphreys J. Introduction to Lie algebras and representation theory
  //page 142, exercise 9.
  MacroRegisterFunctionWithName("MonomialChar_CoefficientType::TensorAndDecompose");
  this->CheckNonZeroOwner();
  std::stringstream errorLog;
  std::string tempS;
  output.MakeZero();
  WeylGroup& theWeyl=this->owner->theWeyl;
  Vector<Rational> leftHWFundCoords;
  leftHWFundCoords=this->weightFundamentalCoordS;
  Vector<Rational> rightHWFundCoords;
  rightHWFundCoords =other.weightFundamentalCoordS;

  Rational leftTotalDim= theWeyl.WeylDimFormulaFundamentalCoords(leftHWFundCoords);
  Rational rightTotalDim= theWeyl.WeylDimFormulaFundamentalCoords(rightHWFundCoords);
  if (leftTotalDim>rightTotalDim)
  { MathRoutines::swap(leftTotalDim, rightTotalDim);
    MathRoutines::swap(leftHWFundCoords, rightHWFundCoords);
  }
  HashedList<Vector<Rational> > weightsLeftSimpleCoords;
  List<Rational> multsLeft;
  if (!theWeyl.FreudenthalEval(leftHWFundCoords, weightsLeftSimpleCoords, multsLeft, &tempS, &theGlobalVariables, 1000000))
  { errorLog << "Freudenthal formula generated error: " << tempS;
    return errorLog.str();
  }
  HashedList<Vector<Rational> > currentOrbit;
  const int OrbitSizeHardLimit=10000000;
//  int theRank=theWeyl.GetDim();
  Vector<Rational> rightHWSimpleCoords=theWeyl.GetSimpleCoordinatesFromFundamental(rightHWFundCoords);
  Vectors<Rational> tempRoots;
  tempRoots.SetSize(1);
//  std::cout << "weights of smaller module: " << weightsLeftSimpleCoords.ToString();
  for (int i=0; i<weightsLeftSimpleCoords.size; i++)
  { tempRoots[0]=weightsLeftSimpleCoords[i];
    theWeyl.GenerateOrbit(tempRoots, false, currentOrbit, false, 0, 0, OrbitSizeHardLimit);
    if (currentOrbit.size>=OrbitSizeHardLimit)
    { errorLog << "Error: orbit layer size exceeded hard-coded limit of " << OrbitSizeHardLimit << ".";
      return errorLog.str();
    }
    for (int j=0; j<currentOrbit.size; j++)
      this->AccountSingleWeight(currentOrbit[j], rightHWSimpleCoords, multsLeft[i], output);
  }
//  std::cout << "<hr><hr><hr><hr>";
  return errorLog.str();
}

template <class coefficient>
bool charSSAlgMod<coefficient>::FreudenthalEvalMeFullCharacter
(charSSAlgMod<coefficient>& outputCharOwnerSetToZero, int upperBoundNumDominantWeights, std::string* outputDetails, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("charSSAlgMod_CoefficientType::FreudenthalEvalMeFullCharacter");
  this->CheckNonZeroOwner();
  charSSAlgMod<coefficient> domChar;
  if (!this->FreudenthalEvalMeDominantWeightsOnly(domChar, upperBoundNumDominantWeights, outputDetails, theGlobalVariables))
    return false;
  outputCharOwnerSetToZero.MakeZero();
  Vectors<Rational> theVect;
  HashedList<Vector<Rational> > theOrbit;
  theVect.SetSize(1);
  MonomialChar<coefficient> tempMon;
  tempMon.owner=0;
  for (int i=0; i<domChar.size(); i++)
  { theVect[0]=this->GetOwner()->theWeyl.GetSimpleCoordinatesFromFundamental(domChar[i].weightFundamentalCoordS);
    if (!(this->GetOwner()->theWeyl.GenerateOrbit(theVect, false, theOrbit, false, -1, 0, upperBoundNumDominantWeights)))
    { if (outputDetails!=0)
        *outputDetails= "failed to generate orbit (possibly too large?)";
      return false;
    }
    for (int j=0; j<theOrbit.size; j++)
    { tempMon.weightFundamentalCoordS=this->GetOwner()->theWeyl.GetFundamentalCoordinatesFromSimple(theOrbit[j]);
      outputCharOwnerSetToZero.AddMonomial(tempMon, domChar.theCoeffs[i]);
    }
  }
  return true;
}

template <class coefficient>
void charSSAlgMod<coefficient>::GetDual(charSSAlgMod<coefficient>& output)const
{ if (&output==this)
  { charSSAlgMod<coefficient> thisCopy=*this;
    thisCopy.GetDual(output);
    return;
  }
  MonomialChar<coefficient> tempM;
  output.MakeZero();
  for (int i=0; i<this->size(); i++)
  { tempM=(*this)[i];
    tempM.weightFundamentalCoordS.Minus();
    output.AddMonomial(tempM, this->theCoeffs[i]);
  }
}

template <class coefficient>
void charSSAlgMod<coefficient>::MakeFromWeight(const Vector<coefficient>& inputWeightSimpleCoords, SemisimpleLieAlgebra* inputOwner)
{ this->MakeZero();
  if (inputWeightSimpleCoords.size!=inputOwner->GetRank())
    crash << "This is a programming error: attempting to create a character from highest weight in simple coords "
    << inputWeightSimpleCoords.ToString() << "(" << inputWeightSimpleCoords.size << " coordinates) while the owner semisimple "
    << " Lie algebra is of rank " << (inputOwner->GetRank()) << crash;
  MonomialChar<coefficient> theMon;
  theMon.owner=inputOwner;
  theMon.weightFundamentalCoordS=
  inputOwner->theWeyl.GetFundamentalCoordinatesFromSimple(inputWeightSimpleCoords);
  this->AddMonomial(theMon, 1);
}

template <class coefficient>
bool charSSAlgMod<coefficient>::FreudenthalEvalMeDominantWeightsOnly
(charSSAlgMod<coefficient>& outputCharOwnerSetToZero, int upperBoundNumDominantWeights, std::string* outputDetails, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("charSSAlgMod_CoefficientType::FreudenthalEvalMeDominantWeightsOnly");
  if (&outputCharOwnerSetToZero==this)
  { charSSAlgMod<coefficient> thisCopy=*this;
    return thisCopy.FreudenthalEvalMeDominantWeightsOnly
    (outputCharOwnerSetToZero, upperBoundNumDominantWeights, outputDetails, theGlobalVariables);
  }
  this->CheckNonZeroOwner();
  outputCharOwnerSetToZero.MakeZero();
  Vector<coefficient> currentWeightFundCoords;
  List<coefficient> currentMults;
  HashedList<Vector<coefficient> > currentWeights;
  std::stringstream localErrors, localDetails;
  std::string localDetail;
  MonomialChar<coefficient> tempMon;
  tempMon.owner=0;
  coefficient bufferCoeff;
  for (int i=0; i<this->size(); i++)
  { currentWeightFundCoords=(*this)[i].weightFundamentalCoordS;
    if (!this->GetOwner()->theWeyl.FreudenthalEval
    (currentWeightFundCoords, currentWeights, currentMults, &localDetail, theGlobalVariables, upperBoundNumDominantWeights))
    { if (outputDetails!=0)
      { localErrors << "Encountered error while evaluating freudenthal formula. Error details: " << localDetail
        << "<br> Further computation detail: " << localDetails.str();
        *outputDetails=localErrors.str();
      }
      return false;
    }
    if (localDetail!="")
      localDetails << "<br>MonomialP " << i+1 << " computation details: " << localDetail;
    for (int j=0; j<currentWeights.size; j++)
    { tempMon.weightFundamentalCoordS=this->GetOwner()->theWeyl.GetFundamentalCoordinatesFromSimple(currentWeights[j]);
      bufferCoeff=this->theCoeffs[i];
      bufferCoeff*=currentMults[j];
      outputCharOwnerSetToZero.AddMonomial(tempMon, bufferCoeff);
    }
  }
  if (outputDetails!=0)
    *outputDetails=localDetails.str();
  return true;
}

template <class coefficient>
void SemisimpleLieAlgebra::GetAd(Matrix<coefficient>& output, ElementSemisimpleLieAlgebra<coefficient>& e)
{ int NumGenerators=this->GetNumGenerators();
  output.init(NumGenerators, NumGenerators);
  output.NullifyAll();
  ElementSemisimpleLieAlgebra<coefficient> theGen, theResult;
  for (int i=0; i<NumGenerators; i++)
  { theGen.MakeGenerator(i, *this);
    this->LieBracket(e, theGen, theResult);
    for (int j=0; j<theResult.size(); j++)
      output(theResult[j].theGeneratorIndex, i)=theResult.theCoeffs[j];
  }
}

template <class coefficient>
void SemisimpleLieAlgebra::GetCommonCentralizer
(const List<ElementSemisimpleLieAlgebra<coefficient> >& inputElementsToCentralize, List<ElementSemisimpleLieAlgebra<coefficient> >& outputCentralizingElements)
{ Matrix<coefficient> tempAd, commonAd;
  for (int i=0; i<inputElementsToCentralize.size; i++)
  { this->GetAd(tempAd, inputElementsToCentralize[i]);
    //tempAd.Transpose();
    commonAd.AppendMatrixToTheBottom(tempAd);
  }
  Vectors<coefficient> outputV;
  commonAd.GetZeroEigenSpace(outputV);
//  std::cout << "<br>Common ad: " << commonAd.ToString();
//  std::cout << "<br>Eigenvectors: " << outputV.ToString();
  outputCentralizingElements.SetSize(outputV.size);
  for (int i=0; i<outputV.size; i++)
  { ElementSemisimpleLieAlgebra<coefficient>& currentElt=outputCentralizingElements[i];
    currentElt.AssignVectorNegRootSpacesCartanPosRootSpaces(outputV[i], *this);
  }
}

template <class coefficient>
void SemisimpleLieAlgebra::LieBracket
(const ElementSemisimpleLieAlgebra<coefficient>& g1, const ElementSemisimpleLieAlgebra<coefficient>& g2, ElementSemisimpleLieAlgebra<coefficient>& output)
{ if (&output==&g1 || &output==&g2)
  { ElementSemisimpleLieAlgebra<coefficient> outputNew;
    this->LieBracket(g1, g2, outputNew);
    output=outputNew;
    return;
  }
  output.MakeZero();
  if (g1.IsEqualToZero() || g2.IsEqualToZero())
    return;
  int maxNumMonsFinal=g1.size()*g2.size();
  output.SetExpectedSize(maxNumMonsFinal);
  coefficient theCoeff;
  ElementSemisimpleLieAlgebra<coefficient> buffer;
  for (int i=0; i<g1.size(); i++)
    for (int j=0; j<g2.size(); j++)
    { buffer=this->theLiebrackets.elements[g1[i].theGeneratorIndex][g2[j].theGeneratorIndex];
      theCoeff=g1.theCoeffs[i];
      theCoeff*=g2.theCoeffs[j];
      buffer*=theCoeff;
      output+=buffer;
    }
}

template <class coefficient>
void ElementSemisimpleLieAlgebra<coefficient>::ActOnMe(const ElementSemisimpleLieAlgebra<coefficient>& theElt, ElementSemisimpleLieAlgebra<coefficient>& output, SemisimpleLieAlgebra& owner)
{ owner.LieBracket(theElt, *this, output);
}

template <class coefficient>
void ElementSemisimpleLieAlgebra<coefficient>::ActOnMe
(const ElementSemisimpleLieAlgebra<coefficient>& theElt, ElementSemisimpleLieAlgebra<coefficient>& output, SemisimpleLieAlgebra& owner,
 const RationalFunctionOld& theRingUnit, const RationalFunctionOld& theRingZero, GlobalVariables* theGlobalVariables)
{ owner.LieBracket(theElt, *this, output);
}

template <class coefficient>
Vector<coefficient> ElementSemisimpleLieAlgebra<coefficient>::GetCartanPart()const
{ Vector<coefficient> result;
  if (this->IsEqualToZero())
  { result.MakeZero(0);
    return result;
  }
  ChevalleyGenerator tempGen;
  SemisimpleLieAlgebra* owner=this->GetOwner();
  int theRank=owner->GetRank();
  int numPosRoots=owner->GetNumPosRoots();
  result.MakeZero(theRank);
//  std::cout << "<br>Zero vector in GetCartanPart is: " << result.ToString();
  if (theRank<=0 || owner==0)
    crash << "This is a programming error: the owner of a semisimple Lie algebra element is non-present or corrupted. " << crash;
  for (int i=0; i<theRank; i++)
  { tempGen.MakeGenerator(*owner, i+numPosRoots);
    int currentIndex=this->theMonomials.GetIndex(tempGen);
    if (currentIndex!=-1)
      result[i]+=this->theCoeffs[currentIndex];
  }
//  std::cout << "<br>GetCartanPart is returning vector: " << result.ToString();
  return result;
}

template <class coefficient>
void ElementSemisimpleLieAlgebra<coefficient>::ElementToVectorNegativeRootSpacesFirst(Vector<coefficient>& output)const
{ if (this->IsEqualToZero())
  { output.MakeZero(0);
    return;
  }
  output.MakeZero(this->GetOwner()->GetNumGenerators());
  for (int i=0; i<this->size(); i++)
    output[(*this)[i].theGeneratorIndex]=this->theCoeffs[i];
}

template <class coefficient>
void ElementSemisimpleLieAlgebra<coefficient>::MakeGGenerator(const Vector<Rational>& theRoot, SemisimpleLieAlgebra& inputOwner)
{ this->MakeGenerator(inputOwner.GetGeneratorFromRoot(theRoot), inputOwner);
}

template <class coefficient>
void ElementSemisimpleLieAlgebra<coefficient>::AssignVectorNegRootSpacesCartanPosRootSpaces(const Vector<coefficient>& input, SemisimpleLieAlgebra& owner)
{ //Changing RootSystem order invalidates this function!
  this->MakeZero();
  ChevalleyGenerator tempGenerator;
  for (int i=0; i<input.size; i++)
    if (input[i]!=0)
    { tempGenerator.MakeGenerator(owner, i);
      this->AddMonomial(tempGenerator, input[i]);
    }
}

template<class coefficient>
bool ElementSemisimpleLieAlgebra<coefficient>::IsElementCartan()const
{ if (this->size()==0)
    return true;
  SemisimpleLieAlgebra* owner= (*this)[0].owneR;
  for (int i=0; i<this->size(); i++)
    if (!owner->IsGeneratorFromCartan((*this)[i].theGeneratorIndex))
      return false;
  return true;
}

template <class coefficient>
void ElementSemisimpleLieAlgebra<coefficient>::MakeHgenerator(const Vector<coefficient>& theH, SemisimpleLieAlgebra& inputOwner)
{ ChevalleyGenerator tempGen;
  this->MakeZero();
  for (int i=0; i<theH.size; i++)
  { tempGen.MakeGenerator(inputOwner, inputOwner.GetCartanIndexFromGenerator(i));
    this->AddMonomial(tempGen, theH[i]);
  }
}

template<class coefficient>
bool charSSAlgMod<coefficient>::DrawMe
(std::string& outputDetails, GlobalVariables& theGlobalVariables, DrawingVariables& theDrawingVars, int upperBoundWeights, bool useMults)
{ MacroRegisterFunctionWithName("charSSAlgMod::DrawMe");
  this->CheckNonZeroOwner();
  charSSAlgMod<coefficient> CharCartan;
  bool result= this->FreudenthalEvalMeDominantWeightsOnly(CharCartan, upperBoundWeights, &outputDetails, &theGlobalVariables);
  std::stringstream out;
  Vectors<Rational> currentOrbit;
  WeylGroup& theWeyl=this->GetOwner()->theWeyl;
  theWeyl.DrawRootSystem(theDrawingVars, false, theGlobalVariables, true);
  int totalNumWeights=0;
  Vectors<coefficient> dominantWeightsNonHashed;
  HashedList<Vector<coefficient> > finalWeights;
  Vector<Rational> convertor;
  for (int i=0; i< CharCartan.size(); i++)
  { const MonomialChar<coefficient>& currentMon=CharCartan[i];
    dominantWeightsNonHashed.size=0;
    dominantWeightsNonHashed.AddOnTop(theWeyl.GetSimpleCoordinatesFromFundamental(currentMon.weightFundamentalCoordS));
    bool isTrimmed=!theWeyl.GenerateOrbit(dominantWeightsNonHashed, false, finalWeights, false, 0,  0, upperBoundWeights);
    totalNumWeights+=finalWeights.size;
    if (isTrimmed || totalNumWeights>upperBoundWeights)
    { out << "Did not generate all weights of the module due to RAM limits. ";
      result=false;
      break;
    }
    for (int j=0; j<finalWeights.size; j++)
    { convertor=finalWeights[j].GetVectorRational();
      theDrawingVars.drawCircleAtVectorBuffer(convertor, 3, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(0,0,0));
      if (useMults)
        theDrawingVars.drawTextAtVectorBuffer
        (convertor, CharCartan.theCoeffs[i].ToString(), CGI::RedGreenBlue(0,0,0), theDrawingVars.PenStyleNormal, 0);
    }
  }
  out << "<br>Number of computed weights: " << totalNumWeights << ". ";
  if (result && totalNumWeights<upperBoundWeights)
    out << "<br>All weights were computed and are drawn. <br>";
  else
    out << "<br><b> Not all weights were computed. </b>";
  outputDetails=out.str();
  return result;
}

template <class coefficient>
void charSSAlgMod<coefficient>::DrawMeAssumeCharIsOverCartan(WeylGroup& actualAmbientWeyl, GlobalVariables& theGlobalVariables, DrawingVariables& theDrawingVars)const
{ if (actualAmbientWeyl.GetDim()<2)
    return;
  Vector<coefficient> actualWeight;
  Vector<Rational> actualWeightRationalPart;

  actualAmbientWeyl.DrawRootSystem(theDrawingVars, true, theGlobalVariables, false, 0, false);
  for (int j=0; j<this->size(); j++)
  { actualWeight=actualAmbientWeyl.GetSimpleCoordinatesFromFundamental((*this)[j].weightFundamentalCoordS);
    actualWeightRationalPart=actualWeight.GetVectorRational(); // <-type conversion here!
    theDrawingVars.drawCircleAtVectorBuffer(actualWeightRationalPart, 5, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(0,0,0));
    theDrawingVars.drawTextAtVectorBuffer(actualWeightRationalPart, this->theCoeffs[j].ToString(), CGI::RedGreenBlue(0,0,0), theDrawingVars.PenStyleNormal, 0);
  }
}

template <class coefficient>
bool charSSAlgMod<coefficient>::SplitOverLeviMonsEncodeHIGHESTWeight
(std::string* Report, charSSAlgMod& output, const Selection& splittingParSel, const Selection& ParSelFDInducingPart,
 SubgroupWeylGroupOLD& outputWeylSub, GlobalVariables& theGlobalVariables)
{ MacroRegisterFunctionWithName("charSSAlgMod<coefficient>::SplitOverLeviMonsEncodeHIGHESTWeight");
  this->CheckNonZeroOwner();
  std::stringstream out;
  std::string tempS;
//  std::cout << "Splitting parabolic selection: " << splittingParSel.ToString();
  if (this->GetOwner()->GetRank()!=splittingParSel.MaxSize)
    crash << "This is a programming error: parabolic selection selects out of " << splittingParSel.MaxSize
    << " elements while the weyl group is of rank " << this->GetOwner()->GetRank() << ". " << crash;
  outputWeylSub.MakeParabolicFromSelectionSimpleRoots(this->GetOwner()->theWeyl, splittingParSel, theGlobalVariables, 1);
  outputWeylSub.ComputeRootSubsystem();
  SubgroupWeylGroupOLD complementGroup;
  Selection invertedSel;
  invertedSel=splittingParSel;
  invertedSel.InvertSelection();
  complementGroup.MakeParabolicFromSelectionSimpleRoots(this->GetOwner()->theWeyl, invertedSel, theGlobalVariables, 1);
  complementGroup.ComputeRootSubsystem();
  out << outputWeylSub.ToString(false);
//  std::cout << "<hr> the Weyl subgroup: " << outputWeylSub.ToString(false);
//  std::cout << this->ToString();
//  std::cout << out.str();
  charSSAlgMod charAmbientFDWeyl, remainingCharDominantLevi;
  SubgroupWeylGroupOLD theFDWeyl;
  theFDWeyl.MakeParabolicFromSelectionSimpleRoots(this->GetOwner()->theWeyl, ParSelFDInducingPart, theGlobalVariables, 1);
  MonomialChar<coefficient> tempMon, localHighest;
  List<coefficient> tempMults;
  HashedList<Vector<coefficient> > tempHashedRoots;
  WeylGroup& theWeyL=this->GetOwner()->theWeyl;
  charAmbientFDWeyl.MakeZero();
  coefficient bufferCoeff, highestCoeff;
  out << "Starting character: " << this->ToString();
  tempMon.owner=this->GetOwner();
  for (int i=0; i<this->size(); i++)
  { const MonomialChar<coefficient>& currentMon=(*this)[i];
    if (!theFDWeyl.FreudenthalEvalIrrepIsWRTLeviPart(currentMon.weightFundamentalCoordS, tempHashedRoots, tempMults, tempS, theGlobalVariables, 10000))
    { if (Report!=0)
        *Report=tempS;
      return false;
    }
//    std::cout << "<hr>FreudenthalEval on " << currentMon.ToString() << " generated the following report: "
//    << tempS << "<hr>";
    for (int j=0; j<tempHashedRoots.size; j++)
    { bufferCoeff=this->theCoeffs[i];
      tempMon.weightFundamentalCoordS=theWeyL.GetFundamentalCoordinatesFromSimple(tempHashedRoots[j]);
      bufferCoeff*=tempMults[j];
      charAmbientFDWeyl.AddMonomial(tempMon, bufferCoeff);
    }
  }
//  std::cout << "<hr>" << tempS;
//  std::cout << "<hr>Freudenthal eval ends up being: " << charAmbientFDWeyl.ToString();

  remainingCharDominantLevi.MakeZero();
  Vectors<coefficient> orbitDom;
  tempMon.owner=this->GetOwner();
  for (int i=0; i<charAmbientFDWeyl.size(); i++)
  { orbitDom.SetSize(0);
    if (!theFDWeyl.GenerateOrbitReturnFalseIfTruncated
        (theWeyL.GetSimpleCoordinatesFromFundamental(charAmbientFDWeyl[i].weightFundamentalCoordS), orbitDom, 10000))
    { out << "failed to generate the complement-sub-Weyl-orbit of weight "
      << this->GetOwner()->theWeyl.GetSimpleCoordinatesFromFundamental(charAmbientFDWeyl[i].weightFundamentalCoordS).ToString();
      if (Report!=0)
        *Report=out.str();
      return false;
    }
//    std::cout << "<hr>the orbit with highest weight "
//    << theWeyL.GetSimpleCoordinatesFromFundamental(charAmbientFDWeyl[i].weightFundamentalCoords).ToString()
//    << " is: ";
//    for (int l=0; l<orbitDom.size; l++)
//      std::cout <<"<br>" << orbitDom[l].ToString();
//    std::cout << "<hr>of them dominant are: <br>";
    for (int k=0; k<orbitDom.size; k++)
      if (outputWeylSub.IsDominantWeight(orbitDom[k]))
      { tempMon.weightFundamentalCoordS=theWeyL.GetFundamentalCoordinatesFromSimple(orbitDom[k]);
        remainingCharDominantLevi.AddMonomial(tempMon, charAmbientFDWeyl.theCoeffs[i]);
//        std::cout << "<br>" << orbitDom[k].ToString() << " with coeff " << charAmbientFDWeyl.theCoeffs[i].ToString();
      }
  }
  output.MakeZero();
  out << "<br>Character w.r.t Levi part: " << CGI::GetMathMouseHover(remainingCharDominantLevi.ToString());
//  std::cout << "<br>Character w.r.t Levi part: " << CGI::GetHtmlMathDivFromLatexAddBeginArrayL
// (remainingCharDominantLevi.ToString());

  Vector<coefficient> simpleGeneratorBaseField;
  while(!remainingCharDominantLevi.IsEqualToZero())
  { localHighest=*remainingCharDominantLevi.theMonomials.LastObject();
    for (bool Found=true; Found; )
    { Found=false;
      for (int i=0; i<outputWeylSub.simpleGenerators.size; i++)
      { tempMon=localHighest;
        simpleGeneratorBaseField=outputWeylSub.simpleGenerators[i]; // <- implicit type conversion here!
        tempMon.weightFundamentalCoordS+=this->GetOwner()->theWeyl.GetFundamentalCoordinatesFromSimple(simpleGeneratorBaseField);
        if (remainingCharDominantLevi.theMonomials.Contains(tempMon))
        { localHighest=tempMon;
          Found=true;
        }
      }
    }
    highestCoeff=remainingCharDominantLevi.theCoeffs[remainingCharDominantLevi.theMonomials.GetIndex(localHighest)];
    output.AddMonomial(localHighest, highestCoeff);
    if (!outputWeylSub.FreudenthalEvalIrrepIsWRTLeviPart
        (localHighest.weightFundamentalCoordS, tempHashedRoots, tempMults, tempS, theGlobalVariables, 10000))
    { if (Report!=0)
        *Report=tempS;
      return false;
    }
//    std::cout << "<hr>accounting " << localHighest.ToString() << " with coeff "
//    << highestCoeff.ToString() << "<br>"
//    << remainingCharDominantLevi.ToString();
    for (int i=0; i<tempHashedRoots.size; i++)
    { tempMon.owner=this->GetOwner();
      tempMon.weightFundamentalCoordS=theWeyL.GetFundamentalCoordinatesFromSimple(tempHashedRoots[i]);
      bufferCoeff=tempMults[i];
      bufferCoeff*=highestCoeff;
      remainingCharDominantLevi.SubtractMonomial(tempMon, bufferCoeff);
//      std::cout << "<br>-(" << bufferCoeff.ToString() << ")" << tempMon.ToString();
    }
//    std::cout << "<br>remaining character after accounting:<br>" << remainingCharDominantLevi.ToString();
  }
  out << "<br>Character w.r.t Levi part: " << CGI::GetMathMouseHover(output.ToString());
  if (Report!=0)
  { //out << "<hr>"  << "The split character is: " << output.ToString("V", "\\omega", false);
    DrawingVariables theDV;
    std::string tempS;
    this->DrawMeNoMults(tempS, theGlobalVariables, theDV, 10000);
    Vector<Rational> tempRoot;
    out << "<hr>In the following weight visualization, a yellow line is drawn if the corresponding weights are "
    << " simple reflections of one another, with respect to a simple root of the Levi part of the parabolic subalgebra. ";
    for (int i=0; i<output.size(); i++)
    { tempRoot=theWeyL.GetSimpleCoordinatesFromFundamental(output[i].weightFundamentalCoordS).GetVectorRational();
      outputWeylSub.DrawContour(tempRoot, theDV, theGlobalVariables, CGI::RedGreenBlue(200, 200, 0), 1000);
      std::stringstream tempStream;
      tempStream << output.theCoeffs[i].ToString();
      theDV.drawTextAtVectorBuffer(tempRoot, tempStream.str(), 0, DrawingVariables::PenStyleNormal, 0);
    }
    out << "<hr>" << theDV.GetHtmlFromDrawOperationsCreateDivWithUniqueName(theWeyL.GetDim());
    *Report=out.str();
  }
  return true;
}

template <class coefficient>
void charSSAlgMod<coefficient>::MakeTrivial(SemisimpleLieAlgebra& inputOwner)
{ this->MakeZero();
  MonomialChar<Rational> tempMon;
  tempMon.owner=&inputOwner;
  tempMon.weightFundamentalCoordS.MakeZero(inputOwner.GetRank());
  this->AddMonomial(tempMon, 1);
}

template <class coefficient>
std::string charSSAlgMod<coefficient>::operator*=(const charSSAlgMod& other)
{ GlobalVariables theGlobalVariables;
  return this->MultiplyBy(other, theGlobalVariables);
}

template <class coefficient>
std::string charSSAlgMod<coefficient>::MultiplyBy(const charSSAlgMod& other, GlobalVariables& theGlobalVariables)
{ if (this->GetOwner()!=other.GetOwner() || this->GetOwner()==0)
    crash << "This is a programming error: attempting to multiply characters of different or non-initialized semisimple Lie algebras." << crash;
  this->SetExpectedSize(other.size()+this->size());
  charSSAlgMod result, summand;
  result.MakeZero();
  std::string potentialError;
  coefficient theCF;
  for (int i=0; i<this->size(); i++)
    for (int j=0; j<other.size(); j++)
    { const MonomialChar<Rational>& left = (*this)[i];
      const MonomialChar<Rational>& right=other[j];
      potentialError=left.TensorAndDecompose(right, summand, theGlobalVariables);
      if (potentialError!="")
        return potentialError;
      theCF=this->theCoeffs[i];
      theCF*=other.theCoeffs[j];
      summand*=theCF;
      result+=summand;
    }
  this->operator=(result);
  return "";
}

#endif
