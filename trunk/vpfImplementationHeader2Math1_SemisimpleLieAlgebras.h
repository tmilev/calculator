//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfImplementationHeaderSemisimpleLieAlgebrasIncluded
#define vpfImplementationHeaderSemisimpleLieAlgebrasIncluded

#include "vpfHeader2Math1_SemisimpleLieAlgebras.h"
static ProjectInformationInstance ProjectInfovpfImplementationHeaderSemisimpleLieAlgebras(__FILE__, "Implementation header, semisimple Lie algebra routines. ");

template <class coefficient>
std::string Weight<coefficient>::ToString(FormatExpressions* theFormat)const
{ std::stringstream out;
  bool formatWeightAsIndexVectorSpace=theFormat==0 ? true : theFormat->flagFormatWeightAsVectorSpaceIndex;
  if (!formatWeightAsIndexVectorSpace)
  { if (this->owner==0)
      return this->weightFundamentalCoordS.ToStringLetterFormat("\\psi", theFormat);
    Vector<coefficient> weightEpsCoords, weightSimpleCoords;
    weightSimpleCoords=this->owner->theWeyl.GetSimpleCoordinatesFromFundamental(this->weightFundamentalCoordS);
    this->owner->theWeyl.GetEpsilonCoords(weightSimpleCoords, weightEpsCoords);
    return weightEpsCoords.ToStringLetterFormat("\\varepsilon", theFormat);
  }
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
void Weight<coefficient>::AccountSingleWeight
(const Vector<Rational>& currentWeightSimpleCoords, const Vector<Rational>& otherHighestWeightSimpleCoords,
 Rational& theMult, charSSAlgMod<coefficient>& outputAccum)const
{ //This is the Brauer-Klimyk formula. Reference:
  //Humphreys J., Introduction to Lie algebras and representation theory
  //page 142, exercise 9.
  //stOutput << "<hr>Accounting " << currentWeightSimpleCoords.ToString()
  //<< " with coefficient " << finalCoeff.ToString();
  MacroRegisterFunctionWithName("Weight_CoefficientType::AccountSingleWeight");
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
//  stOutput << "<br> Before raising to dominant, in simple coords: " << dominant.ToString() << "<br>";
  theWeyl.RaiseToDominantWeight(dominant, &sign);
//  stOutput << "After raising to dominant: " << dominant.ToString() << "<br>";
  dominant-=theWeyl.rho;
  if (!theWeyl.IsDominantWeight(dominant))
    return;
  Weight<Rational> tempMon;
  tempMon.owner=this->owner;
  tempMon.weightFundamentalCoordS=theWeyl.GetFundamentalCoordinatesFromSimple(dominant);
  coefficient coeffChange;
  coeffChange= theMult;
  coeffChange*=sign;
//  stOutput << "; final contribution: " << tempMon.Coefficient.ToString()
//  << "*" << tempMon.weightFundamentalCoords.ToString() << "<br>";
  outputAccum.AddMonomial(tempMon, coeffChange);
}

template <class coefficient>
std::string Weight<coefficient>::TensorAndDecompose
(const Weight<coefficient>& other, charSSAlgMod<coefficient>& output, GlobalVariables& theGlobalVariables)const
{ //This is the Brauer-Klimyk formula. Reference:
  //Humphreys J. Introduction to Lie algebras and representation theory
  //page 142, exercise 9.
  MacroRegisterFunctionWithName("Weight_CoefficientType::TensorAndDecompose");
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
//  stOutput << "weights of smaller module: " << weightsLeftSimpleCoords.ToString();
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
//  stOutput << "<hr><hr><hr><hr>";
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
  Weight<coefficient> tempMon;
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
  Weight<coefficient> tempM;
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
  Weight<coefficient> theMon;
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
  Weight<coefficient> tempMon;
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
  output.MakeZero();
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
//  stOutput << "<br>Common ad: " << commonAd.ToString();
//  stOutput << "<br>Eigenvectors: " << outputV.ToString();
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
//  stOutput << "<br>Zero vector in GetCartanPart is: " << result.ToString();
  if (theRank<=0 || owner==0)
    crash << "This is a programming error: the owner of a semisimple Lie algebra element is non-present or corrupted. " << crash;
  for (int i=0; i<theRank; i++)
  { tempGen.MakeGenerator(*owner, i+numPosRoots);
    int currentIndex=this->theMonomials.GetIndex(tempGen);
    if (currentIndex!=-1)
      result[i]+=this->theCoeffs[currentIndex];
  }
//  stOutput << "<br>GetCartanPart is returning vector: " << result.ToString();
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
  { const Weight<coefficient>& currentMon=CharCartan[i];
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
void charSSAlgMod<coefficient>::MakeTrivial(SemisimpleLieAlgebra& inputOwner)
{ this->MakeZero();
  Weight<Rational> tempMon;
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
    { const Weight<Rational>& left = (*this)[i];
      const Weight<Rational>& right=other[j];
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
