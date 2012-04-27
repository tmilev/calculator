//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader1.h"
#include "vpfHeader1_2.h"

ProjectInformationInstance ProjectInfoVpf7cpp(__FILE__, "Implementation file intended for small fast changes. ");


class Qsqrt2;

template < > int List<LittelmannPath>::ListActualSizeIncrement=1000;
template < > int List<MonomialUniversalEnvelopingOrdered<Rational> >::ListActualSizeIncrement=100;
template < > int List<ElementUniversalEnvelopingOrdered<Rational> >::ListActualSizeIncrement=100;
//template < > int List<Matrix<Qsqrt2> >::ListActualSizeIncrement=100;
//template < > int List<Vector<Qsqrt2> >::ListActualSizeIncrement=100;
//template < > int HashedList<MonomialUniversalEnveloping<Qsqrt2> >::PreferredHashSize=100;
template < > int HashedListB<MonomialUniversalEnvelopingOrdered<Rational>, MonomialUniversalEnvelopingOrdered<Rational>::HashFunction>::PreferredHashSize=100;
template < > int HashedListB<LittelmannPath, LittelmannPath::HashFunction>::PreferredHashSize=1000;
template < > int HashedListB<MonomialUniversalEnveloping<Qsqrt2>, MonomialUniversalEnveloping<Qsqrt2>::HashFunction>::PreferredHashSize=30;

int ParserNode::EvaluateInvariantsExteriorPowerFundamentalRepsPlusTrivialReps
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  if (theNode.owner->DefaultWeylLetter!='B')
  { out << "My lazy master hasn't taught me how to compute that! Email him to fix that!";
    theNode.ExpressionType=theNode.typeString;
    return theNode.errorNoError;
  }
  out << "Function not implemented. This piece of code is waiting for deletion.";
  ModuleSSalgebraNew<Qsqrt2> theModule;
//  theModule.MakeFundamentalRep('B', 2, theGlobalVariables);
  theNode.outputString=out.str();
  theNode.ExpressionType=theNode.typeString;
  return theNode.errorNoError;
}

std::string ReflectionSubgroupWeylGroup::ElementToStringCosetGraph()
{ if (this->size<1)
    return "Error, non-initialized group";
  if (this->size==1)
    return "id";
  List<List<List<int> > > arrows;
  List<List<int> > Layers;
  Vector<Rational> tempRoot;
  Layers.Reserve(this->RepresentativesQuotientAmbientOrder.size);
  int GraphWidth=1;
  int oldLayerElementLength=-1;
  for (int i=0; i< this->RepresentativesQuotientAmbientOrder.size; i++)
  { if (this->RepresentativesQuotientAmbientOrder[i].size!=oldLayerElementLength)
    { Layers.SetSize(Layers.size+1);
      oldLayerElementLength=this->RepresentativesQuotientAmbientOrder[i].size;
    }
    Layers.LastObject()->AddOnTop(i);
    GraphWidth=MathRoutines::Maximum(GraphWidth, Layers.LastObject()->size);
  }
//  HashedList<Vector<Rational>> orbit;
//  orbit.Reserve(this->RepresentativesQuotientAmbientOrder.size);
  for (int i=0; i<this->RepresentativesQuotientAmbientOrder.size; i++)
  { tempRoot=this->AmbientWeyl.rho;
    this->AmbientWeyl.ActOnRootByGroupElement
  (this->AmbientWeyl.GetIndex(this->RepresentativesQuotientAmbientOrder[i]), tempRoot, false, false);
//    orbit.AddOnTop(tempRoot);
  }
  arrows.SetSize(Layers.size);
  for (int i=0; i< Layers.size; i++)
  { arrows[i].SetSize(Layers[i].size);
    for (int j=0; j<Layers[i].size; j++)
      for (int k=0; k<this->RepresentativesQuotientAmbientOrder.size; k++)
        if (this->AmbientWeyl.LeftIsHigherInBruhatOrderThanRight
        ( this->RepresentativesQuotientAmbientOrder[k], this->RepresentativesQuotientAmbientOrder[Layers[i][j]]))
          if (this->RepresentativesQuotientAmbientOrder[Layers[i][j]].size
              ==this->RepresentativesQuotientAmbientOrder[k].size-1)
            arrows[i][j].AddOnTop(k);
  }
  return this->ElementToStringFromLayersAndArrows(arrows, Layers, GraphWidth, true);
}

bool WeylGroup::LeftIsHigherInBruhatOrderThanRight(ElementWeylGroup& left, ElementWeylGroup& right)
{ Vector<Rational> leftImage; leftImage=this->rho;
  Vector<Rational> rightImage; rightImage=this->rho;
  this->ActOn(left, leftImage, false, false, (Rational) 0);
  this->ActOn(right, rightImage, false, false, (Rational) 0);
  return (rightImage-leftImage).IsPositiveOrZero() && !(rightImage-leftImage).IsEqualToZero();
}

void ReflectionSubgroupWeylGroup::FindQuotientRepresentatives(int UpperLimit)
{ this->AmbientWeyl.ComputeWeylGroup(UpperLimit);
  Vector<Rational> image1;
  this->RepresentativesQuotientAmbientOrder.size=0;
  this->RepresentativesQuotientAmbientOrder.Reserve(this->AmbientWeyl.size);
  for (int i=0; i<this->AmbientWeyl.size; i++)
  { image1=this->AmbientWeyl.rho;
    this->AmbientWeyl.ActOnRootByGroupElement(i, image1, false, false);
    bool isGood=true;
    for (int j=0; j<this->simpleGenerators.size; j++)
      if (this->AmbientWeyl.RootScalarCartanRoot(image1, this->simpleGenerators[j]).IsNegative())
      { isGood=false;
        break;
      }
    if (isGood)
      this->RepresentativesQuotientAmbientOrder.AddOnTop(this->AmbientWeyl[i]);
  }
}

int ParserNode::EvaluateChar
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ int theDim=theNode.owner->theHmm.theRange().GetRank();
  Vector<Rational> theWeight;
  if (! theNode.GetRootRationalFromFunctionArguments(theWeight, theGlobalVariables) )
    return theNode.SetError(theNode.errorBadOrNoArgument);
  if (theWeight.size!=theDim)
    return theNode.SetError(theNode.errorDimensionProblem);
  charSSAlgMod& output=theNode.theChar.GetElement();

  output.MakeFromWeight
  (theNode.owner->theHmm.theRange().theWeyl.GetSimpleCoordinatesFromFundamental(theWeight),
   theNode.owner->theAlgebras, 0);
  theNode.ExpressionType=theNode.typeCharSSFDMod;
  return theNode.errorNoError;
}

bool charSSAlgMod::DrawMe
(std::string& outputDetails, GlobalVariables& theGlobalVariables,
 DrawingVariables& theDrawingVars, int upperBoundWeights, bool useMults)
{ charSSAlgMod CharCartan;
  bool result= this->FreudenthalEvalMe
  (CharCartan, outputDetails, theGlobalVariables, upperBoundWeights);
  std::stringstream out;
  Vectors<Rational> currentOrbit;
  WeylGroup& theWeyl=this->listOwners->TheObjects[this->indexInOwners].theWeyl;
  theWeyl.DrawRootSystem(theDrawingVars, false, theGlobalVariables, true);
  int totalNumWeights=0;
  Vectors<Rational> dominantWeightsNonHashed;
  HashedList<Vector<Rational> > finalWeights;
  for (int i=0; i< CharCartan.size; i++)
  { MonomialChar<Rational>& currentMon=CharCartan[i];
    dominantWeightsNonHashed.size=0;
    dominantWeightsNonHashed.AddOnTop
    (theWeyl.GetSimpleCoordinatesFromFundamental(currentMon.weightFundamentalCoords));
    bool isTrimmed=!theWeyl.GenerateOrbit
    (dominantWeightsNonHashed, false, finalWeights, false, 0, upperBoundWeights);
    totalNumWeights+=finalWeights.size;
    if (isTrimmed || totalNumWeights>upperBoundWeights)
    { out << "Did not generate all weights of the module due to RAM limits. ";
      result=false;
      break;
    }
    for (int j=0; j<finalWeights.size; j++)
    { theDrawingVars.drawCircleAtVectorBuffer(finalWeights[j], 5, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(0,0,0));
      if (useMults)
        theDrawingVars.drawTextAtVectorBuffer
          (finalWeights[j], currentMon.Coefficient.ElementToString(), CGI::RedGreenBlue(0,0,0), theDrawingVars.PenStyleNormal, 0);
    }
  }
  out << "<br>Number of computed weights: " << totalNumWeights << ". ";
  if (result && totalNumWeights<upperBoundWeights)
    out << "<br>All weights were computed and are drawn. <br>";
  else
    out << "<br><b> Not all weights were computed. </b>";
  outputDetails=out.str();
//  outputWeights.CopyFromBase(finalWeights);
  return result;
}

void charSSAlgMod::MakeFromWeight
(const Vector<Rational>& inputWeightSimpleCoords, List<SemisimpleLieAlgebra>& inputOwners,
 int inputIndexInOwner)
{ this->MakeZero(inputOwners, inputIndexInOwner);
  assert(inputWeightSimpleCoords.size==this->listOwners->TheObjects[this->indexInOwners].GetRank());
  MonomialChar<Rational> theMon;
  theMon.weightFundamentalCoords=
  this->listOwners->TheObjects[this->indexInOwners].theWeyl.GetFundamentalCoordinatesFromSimple(inputWeightSimpleCoords);
  theMon.Coefficient=1;
  this->AddOnTop(theMon);
}

template <class CoefficientType>
std::string MonomialChar<CoefficientType>::TensorAndDecompose
(MonomialChar<CoefficientType>& other, List<SemisimpleLieAlgebra>& inputOwners, int inputIndexInOwners, charSSAlgMod& output,
 GlobalVariables& theGlobalVariables)
{ std::stringstream errorLog;
  std::string tempS;
  output.MakeZero(inputOwners, inputIndexInOwners);
  Rational finalCoefficient;
  finalCoefficient=this->Coefficient*other.Coefficient;
  if (finalCoefficient==0)
    return "";
  WeylGroup& theWeyl=inputOwners.TheObjects[inputIndexInOwners].theWeyl;
  Vector<Rational> leftHWFundCoords;
  leftHWFundCoords=this->weightFundamentalCoords;
  Vector<Rational> rightHWFundCoords;
  rightHWFundCoords =other.weightFundamentalCoords;

  Rational leftTotalDim= theWeyl.WeylDimFormula(leftHWFundCoords);
  Rational rightTotalDim= theWeyl.WeylDimFormula(rightHWFundCoords);
  if (leftTotalDim>rightTotalDim)
  { MathRoutines::swap(leftTotalDim, rightTotalDim);
    MathRoutines::swap(leftHWFundCoords, rightHWFundCoords);
  }
  HashedList<Vector<Rational> > weightsLeftSimpleCoords;
  List<Rational> multsLeft;
  if (!theWeyl.FreudenthalEval
      (leftHWFundCoords, weightsLeftSimpleCoords, multsLeft, tempS, theGlobalVariables, 1000000))
  { errorLog << "Freudenthal formula generated error: " << tempS;
    return errorLog.str();
  }
  HashedList<Vector<Rational> > currentOrbit;
  const int OrbitSizeHardLimit=10000000;
//  int theRank=theWeyl.GetDim();
  Vector<Rational> rightHWSimpleCoords=theWeyl.GetSimpleCoordinatesFromFundamental(rightHWFundCoords);
  Vectors<Rational> tempRoots;
  tempRoots.SetSize(1);
//  std::cout << "weights of smaller module: " << weightsLeftSimpleCoords.ElementToString();
  for (int i=0; i<weightsLeftSimpleCoords.size; i++)
  { tempRoots[0]=weightsLeftSimpleCoords[i];
    theWeyl.GenerateOrbit(tempRoots, false, currentOrbit, false, 0, OrbitSizeHardLimit);
    if (currentOrbit.size>=OrbitSizeHardLimit)
    { errorLog << "Error: orbit layer size exceeded hard-coded limit of " << OrbitSizeHardLimit << ".";
      return errorLog.str();
    }
    for (int j=0; j<currentOrbit.size; j++)
      this->AccountSingleWeight
        (currentOrbit[j], rightHWSimpleCoords, theWeyl, multsLeft[i], output, finalCoefficient);
  }
//  std::cout << "<hr><hr><hr><hr>";
  return errorLog.str();
}

template <class CoefficientType>
void MonomialChar<CoefficientType>::AccountSingleWeight
(Vector<Rational>& currentWeightSimpleCoords, Vector<Rational>& otherHighestWeightSimpleCoords, WeylGroup& theWeyl,
 Rational& theMult, charSSAlgMod& outputAccum, Rational& finalCoeff
 )
{ //std::cout << "<hr>Accounting " << currentWeightSimpleCoords.ElementToString()
  //<< " with coefficient " << finalCoeff.ElementToString();

  Vector<Rational> dominant=currentWeightSimpleCoords;
  dominant+=otherHighestWeightSimpleCoords;
  dominant+=theWeyl.rho;
  int sign;
  bool certainlyHasStabilizer;
//  std::cout << "<br> Before raising to dominant, in simple coords: " << dominant.ElementToString() << "<br>";
  theWeyl.RaiseToDominantWeight(dominant, &sign, &certainlyHasStabilizer);
//  std::cout << "After raising to dominant: " << dominant.ElementToString() << "<br>";
  dominant-=theWeyl.rho;
  if (!theWeyl.IsDominantWeight(dominant))
    return;
  MonomialChar<Rational> tempMon;
  tempMon.weightFundamentalCoords=theWeyl.GetFundamentalCoordinatesFromSimple(dominant);
  tempMon.Coefficient=theMult*finalCoeff*sign;
//  std::cout << "; final contribution: " << tempMon.Coefficient.ElementToString()
//  << "*" << tempMon.weightFundamentalCoords.ElementToString() << "<br>";
  outputAccum+=tempMon;
}

template <class CoefficientType>
std::string Vector<CoefficientType>::ElementToStringLetterFormat
(const std::string& inputLetter, bool useLatex, bool DontIncludeLastVar)
{ if (this->IsEqualToZero())
    return "0";
  std::stringstream out;
  std::string tempS;
  bool found=false;
  int NumVars= DontIncludeLastVar ? this->size-1 : this->size;
  for(int i=0; i<NumVars; i++)
    if (!this->TheObjects[i].IsEqualToZero())
    { this->TheObjects[i].ElementToString(tempS);
      if (tempS=="1")
        tempS="";
      if (tempS=="-1")
        tempS="-";
      if (found)
      { if (tempS.size()>0)
        { if (tempS[0]!='-')
            out << "+";
        } else
          out << "+";
      }
      found=true;
      out << tempS;
      out << inputLetter << "_{" << i+1<< "}";
    }
  return out.str();
}

void charSSAlgMod::operator+=(const MonomialChar<Rational>& other)
{ if (other.Coefficient.IsEqualToZero())
    return;
  int index=this->GetIndex(other);
  if (index==-1)
  { this->AddOnTop(other);
    return;
  }
  this->TheObjects[index].Coefficient+=other.Coefficient;
  if (this->TheObjects[index].Coefficient.IsEqualToZero())
    this->PopIndexSwapWithLast(index);
}

void charSSAlgMod::operator-=(const MonomialChar<Rational>& other)
{ if (other.Coefficient.IsEqualToZero())
    return;
  int index=this->GetIndex(other);
  if (index==-1)
  { this->AddOnTop(other);
    this->LastObject()->Coefficient*=-1;
    return;
  }
  this->TheObjects[index].Coefficient-=other.Coefficient;
  if (this->TheObjects[index].Coefficient.IsEqualToZero())
    this->PopIndexSwapWithLast(index);
}

void charSSAlgMod::operator+=(const charSSAlgMod& other)
{ this->Reserve(other.size+this->size);
  if ( (other.size+this->size)/this->HashSize>5)
    this->SetHashSizE(other.size+this->size);
  for (int i=0; i<other.size; i++)
    this->operator+=(other[i]);
}

void charSSAlgMod::operator-=(const charSSAlgMod& other)
{ this->Reserve(other.size+this->size);
  if ( (other.size+this->size)/this->HashSize>5)
    this->SetHashSizE(other.size+this->size);
  for (int i=0; i<other.size; i++)
    this->operator-=(other[i]);
}

std::string charSSAlgMod::operator*=(const charSSAlgMod& other)
{ GlobalVariables theGlobalVariables;
  return this->MultiplyBy(other, theGlobalVariables);
}

std::string charSSAlgMod::MultiplyBy(const charSSAlgMod& other, GlobalVariables& theGlobalVariables)
{ assert(this->listOwners==other.listOwners&& this->indexInOwners==other.indexInOwners && this->indexInOwners!=-1);
  this->Reserve(other.size+this->size);
  charSSAlgMod result, summand;
  result.MakeZero(*this->listOwners, this->indexInOwners);
  std::string potentialError;
  for (int i=0; i<this->size; i++)
    for (int j=0; j<other.size; j++)
    { MonomialChar<Rational>& left = this->TheObjects[i];
      MonomialChar<Rational>& right=other[j];
      potentialError=left.TensorAndDecompose(right, *this->listOwners, this->indexInOwners, summand, theGlobalVariables);
      if (potentialError!="")
        return potentialError;
      result+=summand;
    }
  this->operator=(result);
  return "";
}

void charSSAlgMod::MakeTrivial(List<SemisimpleLieAlgebra>& inputOwners, int inputIndexInOwners)
{ this->MakeZero(inputOwners, inputIndexInOwners);
  MonomialChar<Rational> tempMon;
  tempMon.Coefficient=1;
  tempMon.weightFundamentalCoords.MakeZero(inputOwners[inputIndexInOwners].GetRank());
  this->operator+=(tempMon);
}

template <class CoefficientType>
std::string MonomialChar<CoefficientType>::ElementToString
  (const std::string& theVectorSpaceLetter, const std::string& theWeightLetter, bool useBrackets)
{ std::stringstream out;
  std::string coeffString=this->Coefficient.ElementToString();
  if (coeffString=="1")
    coeffString="";
  if (coeffString=="-1")
    coeffString="-";
  out << coeffString << theVectorSpaceLetter;
  if (!useBrackets)
    out << "_{" << this->weightFundamentalCoords.ElementToStringLetterFormat(theWeightLetter,  true, false)
    << "}";
  else
    out << weightFundamentalCoords.ElementToString();
  return out.str();
}

std::string charSSAlgMod::ElementToString
(const std::string& theVectorSpaceLetter, const std::string& theWeightLetter, bool useBrackets)
{ std::stringstream out;
  if (this->size==0)
    return "0";
  std::string tempS;
  List<MonomialChar<Rational> > sortedMons;
  sortedMons=*this;
  sortedMons.QuickSortDescending();
  int lastCutOff=0;
  for (int i=0; i<this->size; i++)
  { MonomialChar<Rational>& current=sortedMons[i];
    tempS=current.ElementToString(theVectorSpaceLetter, theWeightLetter, useBrackets);
    if (i!=0)
      if (tempS!="")
        if (tempS[0]!='-')
          out << "+";
    out << tempS;
    int sizeSoFar=out.str().size();
    if (sizeSoFar-lastCutOff>100)
    { lastCutOff = sizeSoFar;
      out << "\\\\&&";
    }
  }
  return out.str();
}
/*
Rational WeylGroup::EstimateNumDominantWeightsBelow
(Vector<Rational>& inputHWsimpleCoords, GlobalVariables& theGlobalVariables)
{ Cone theWeylChamber;
  this->GetWeylChamber(theWeylChamber, theGlobalVariables);
  int theDim=this->GetDim();
  Vectors<Rational> strechedVertices=theWeylChamber.Vertices;
  std::string tempS;
  PolynomialOutputFormat tempFormat;
  tempS=theWeylChamber.ElementToString(tempFormat);
  assert(strechedVertices.size==theDim);
  Rational result=1;
  for (int i=0; i<theDim; i++)
  { //Vector<Rational>& currentEdge=strechedVertices[i];
    //Rational tempRat=currentEdge.SumCoords();
    //currentEdge/=tempRat;
    //Rational strech= (inputHWsimpleCoords.SumCoords()/currentEdge.SumCoords())+2;
    Rational strech= inputHWsimpleCoords.SumCoords()+2;
    result*=strech*theDim;
  }
  result/=Rational::Factorial(theDim);
  result.AssignAbsoluteValue();
  result+=1;
  assert((result-1).IsPositive());
  return result;
}*/

bool WeylGroup::GetAlLDominantWeightsHWFDIM
(Vector<Rational>& highestWeightSimpleCoords, HashedList<Vector<Rational> >& outputWeightsSimpleCoords,
 int upperBoundDominantWeights, std::string& outputDetails, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
//  double startTime=theGlobalVariables.GetElapsedSeconds();
//  std::cout << "<br>time elapsed: " << theGlobalVariables.GetElapsedSeconds()-startTime;
  Vector<Rational> highestWeightTrue=highestWeightSimpleCoords;
  this->RaiseToDominantWeight(highestWeightTrue);
  Vector<Rational> highestWeightFundCoords=this->GetFundamentalCoordinatesFromSimple(highestWeightTrue);
  if (!highestWeightFundCoords.SumCoords().IsSmallInteger())
  { out << "<hr> The highest weight you gave in simple coordinates: " << highestWeightSimpleCoords.ElementToString()
    << " which equals " << highestWeightFundCoords.ElementToString() << "  in fundamental coordinates "
    << " is not integral dominant.<br>";
    outputDetails=out.str();
    return false;
  }
  int theTopHeightSimpleCoords=(int) highestWeightSimpleCoords.SumCoords().DoubleValue()+1;
//  int theTopHeightFundCoords=(int) highestWeightFundCoords.SumCoords().DoubleValue();
  if (theTopHeightSimpleCoords<0)
    theTopHeightSimpleCoords=0;
  List<HashedList<Vector<Rational> > > outputWeightsByHeight;
  int topHeightRootSystem=this->RootsOfBorel.LastObject()->SumCoords().NumShort;
  int topHeightRootSystemPlusOne=topHeightRootSystem+1;
  outputWeightsByHeight.SetSize(topHeightRootSystemPlusOne);
  int finalHashSize=100;
  for (int i=0; i<topHeightRootSystemPlusOne; i++)
    outputWeightsByHeight[i].SetHashSizE(finalHashSize);
  outputWeightsSimpleCoords.Clear();
  outputWeightsSimpleCoords.SetHashSizE(10000);
  outputWeightsByHeight[0].AddOnTop(highestWeightTrue);
  int numTotalWeightsFound=0;
  int numPosRoots=this->RootsOfBorel.size;
  Vector<Rational> currentWeight;
//  std::cout << "<br>time spend before working cycle: " << theGlobalVariables.GetElapsedSeconds()-startTime;
  for (int lowestUnexploredHeightDiff=0; lowestUnexploredHeightDiff<=theTopHeightSimpleCoords;
  lowestUnexploredHeightDiff++)
  { //double startCycleTime=theGlobalVariables.GetElapsedSeconds();
    if (upperBoundDominantWeights>0 && numTotalWeightsFound>upperBoundDominantWeights)
      break;
    int bufferIndexShift=lowestUnexploredHeightDiff%topHeightRootSystemPlusOne;
    HashedList<Vector<Rational> >& currentHashes=outputWeightsByHeight[bufferIndexShift];
    for (int lowest=0; lowest<currentHashes.size; lowest++)
      for (int i=0; i<numPosRoots; i++)
      { currentWeight=currentHashes[lowest];
        currentWeight-=this->RootsOfBorel[i];
        if (this->IsDominantWeight(currentWeight))
        { int currentIndexShift=this->RootsOfBorel[i].SumCoords().NumShort;
          currentIndexShift=(currentIndexShift+bufferIndexShift)%topHeightRootSystemPlusOne;
          if (outputWeightsByHeight[currentIndexShift].AddNoRepetition(currentWeight))
          { numTotalWeightsFound++;
            outputWeightsByHeight[currentIndexShift].AdjustHashes();
          }
        }
      }
//    std::cout << "<br>time spent before accounting at height level " << lowestUnexploredHeightDiff
//    << ": " << theGlobalVariables.GetElapsedSeconds()-startCycleTime;
//    std::cout << " Size of current level: " << currentHashes.size;
    outputWeightsSimpleCoords.AddOnTop(currentHashes);
//    std::cout << ". Time spent after accounting at height level " << lowestUnexploredHeightDiff
//    << ": " << theGlobalVariables.GetElapsedSeconds()-startCycleTime;
//    startCycleTime=theGlobalVariables.GetElapsedSeconds();
    outputWeightsSimpleCoords.AdjustHashes();
    currentHashes.Clear();
//    std::cout << ". Time spent clearing up buffer at height level " << lowestUnexploredHeightDiff
//    << ": " << theGlobalVariables.GetElapsedSeconds()-startCycleTime;
  }
  out << " Total number of dominant weights: " << outputWeightsSimpleCoords.size;
  if (numTotalWeightsFound>=upperBoundDominantWeights)
    out << "<hr>This message is generated either because the number of weights has "
    << "exceeded the hard-coded RAM memory limits, or because "
    << " a priori bound for the number of weights is WRONG. If the latter "
    << " is the case, make sure to send an angry email to the author(s).";
  outputDetails=out.str();
  //std::cout << "<hr><hr>Total time spent generating weights: " << -startTime+theGlobalVariables.GetElapsedSeconds();
  return (numTotalWeightsFound<=upperBoundDominantWeights);
}

void WeylGroup::RaiseToDominantWeight(Vector<Rational>& theWeight, int* sign, bool* stabilizerFound)
{ if (sign!=0)
    *sign=1;
  if (stabilizerFound!=0)
    *stabilizerFound=false;
  Rational theScalarProd;
  int theDim=this->GetDim();
  for (bool found = true; found; )
  { found=false;
    for (int i=0; i<theDim; i++)
    { theScalarProd=this->GetScalarProdSimpleRoot(theWeight, i);
      if (theScalarProd.IsNegative())
      { found=true;
        theScalarProd*=2;
        theScalarProd/=this->CartanSymmetric.elements[i][i];
        theWeight[i]-=theScalarProd;
        if (sign!=0)
          *sign*=-1;
      }
      if (stabilizerFound!=0)
        if (theScalarProd.IsEqualToZero())
          *stabilizerFound=true;
    }
  }
//  std::cout << "<hr># simple reflections applied total: " << numTimesReflectionWasApplied;
}

bool ReflectionSubgroupWeylGroup::FreudenthalEvalIrrepIsWRTLeviPart
(Vector<Rational>& inputHWfundamentalCoords, HashedList<Vector<Rational> >& outputDominantWeightsSimpleCoords,
 List<Rational>& outputMultsSimpleCoords, std::string& outputDetails,
 GlobalVariables& theGlobalVariables, int UpperBoundFreudenthal)
{ //double startTimer=theGlobalVariables.GetElapsedSeconds();
  this->ComputeRootSubsystem();
  Vectors<Rational> EiBasis;
  EiBasis.MakeEiBasis(this->AmbientWeyl.GetDim());
  List<bool> Explored;
  Vector<Rational> hwSimpleCoords=this->AmbientWeyl.GetSimpleCoordinatesFromFundamental(inputHWfundamentalCoords);
  if (!this->GetAlLDominantWeightsHWFDIM
      (hwSimpleCoords, outputDominantWeightsSimpleCoords, UpperBoundFreudenthal, outputDetails, theGlobalVariables))
  { std::stringstream errorLog;
    errorLog << "Error: the number of dominant weights exceeded hard-coded limit of "
    << UpperBoundFreudenthal << ". Please check out whether LiE's implementation of the Freudenthal "
    << " formula can do your computation.";
    outputDetails=errorLog.str();
    return false;
  }
//  std::cout << "Highest weight: " << hwSimpleCoords.ElementToString() << "<br>Dominant weights wrt. levi part("
//  << outputDominantWeightsSimpleCoords.size << "):<br> ";
//  for (int i=0; i<outputDominantWeightsSimpleCoords.size; i++)
//    std::cout << "<br>" << outputDominantWeightsSimpleCoords[i].ElementToString();
  Explored.initFillInObject(outputDominantWeightsSimpleCoords.size, false);
  outputMultsSimpleCoords.SetSize(outputDominantWeightsSimpleCoords.size);
  Vector<Rational> currentWeight, currentDominantRepresentative;
  Vector<Rational> Rho=this->GetRho();
//  std::cout << "<br> Rho equals: " << Rho.ElementToString();
  //out << "<br> Rho equals: " << Rho.ElementToString();
  Rational hwPlusRhoSquared=this->AmbientWeyl.RootScalarCartanRoot(hwSimpleCoords+Rho, hwSimpleCoords+Rho);
  Explored[0]=true;
  outputMultsSimpleCoords[0]=1;
//  std::cout << "<br>time for generating weights and initializations: " << theGlobalVariables.GetElapsedSeconds()-startTimer;
  //static double totalTimeSpentOnHashIndexing=0;
//  static double timeSpentRaisingWeights=0;
  for (int k=1; k< outputDominantWeightsSimpleCoords.size; k++)
  { Explored[k]=true;
    Rational& currentAccum=outputMultsSimpleCoords[k];
    currentAccum=0;
    for (int j=0; j<this->RootsOfBorel.size; j++)
      for (int i=1; ; i++)
      { currentWeight= outputDominantWeightsSimpleCoords[k]+this->RootsOfBorel[j]*i;
        currentDominantRepresentative=currentWeight;
//        double startLocal=theGlobalVariables.GetElapsedSeconds();
        this->RaiseToDominantWeight(currentDominantRepresentative);
//        std::cout << "<br>currentDominant representative: " << currentDominantRepresentative.ElementToString();
        int theIndex=outputDominantWeightsSimpleCoords.GetIndex(currentDominantRepresentative);
//        std::cout << "<br>index of currentDomain rep: " << theIndex;
        //totalTimeSpentOnHashIndexing+=theGlobalVariables.GetElapsedSeconds()-beforeHash;
        if (theIndex==-1)
          break;
//        std::cout << "<br> summing over weight: " << currentWeight.ElementToString();
        if (!Explored[theIndex])
        { std::stringstream errorLog;
          errorLog << "This is an internal error check. If you see it, that means " << " that the Freudenthal algorithm implementation is "
          << " wrong (because the author of the implementation was dumb enough to try to write less code than what is "
          << " suggested by LiE).";
          outputDetails=errorLog.str();
          return false;
        }
        currentAccum+=this->AmbientWeyl.RootScalarCartanRoot(currentWeight, this->RootsOfBorel[j])
        *outputMultsSimpleCoords[theIndex];
//        std::cout << "<hr>current weight: " << currentWeight.ElementToString();
//        std::cout << "<br>current dominant representative " << currentDominantRepresentative.ElementToString();
      }
    currentAccum*=2;
//    std::cout << "<br>hwPlusRhoSquared: " << hwPlusRhoSquared.ElementToString();
//    std::cout << "<br>Coeff we divide by: " << (hwPlusRhoSquared-this->AmbientWeyl.RootScalarCartanRoot
//   (outputDominantWeightsSimpleCoords[k]+Rho, outputDominantWeightsSimpleCoords[k]+Rho))
//    .ElementToString()
    ;
    Rational tempDen= hwPlusRhoSquared- this->AmbientWeyl.RootScalarCartanRoot
    (outputDominantWeightsSimpleCoords[k]+Rho, outputDominantWeightsSimpleCoords[k]+Rho);
    assert(tempDen!=0);
    currentAccum/=tempDen;
    std::stringstream out;
    out << " Computed the multiplicities of " << k+1 << " out of "
    << outputDominantWeightsSimpleCoords.size << " dominant weights in the support.";
    theGlobalVariables.MakeStatusReport(out.str());
//    std::cout
//    << "<hr> Computed the multiplicities of " << k+1 << " out of " << outputDominantWeightsSimpleCoords.size << " dominant weights in the support.";
 //   theGlobalVariables.MakeStatusReport(out.str());
//    std::cout << "<br>time so far: " << theGlobalVariables.GetElapsedSeconds()-startTimer;
//    std::cout << " of which " << totalTimeSpentOnHashIndexing << " used for hash routines";
//    std::cout << " of which " << timeSpentRaisingWeights << " used to raise weights";
  }
//  std::cout << "<br>Total freudenthal running time: " << theGlobalVariables.GetElapsedSeconds()-startTimer;
  return true;
}

bool WeylGroup::FreudenthalEval
(Vector<Rational>& inputHWfundamentalCoords, HashedList<Vector<Rational> >& outputDominantWeightsSimpleCoords,
 List<Rational>& outputMultsSimpleCoords, std::string& outputDetails,
 GlobalVariables& theGlobalVariables, int UpperBoundFreudenthal)
{ //double startTimer=theGlobalVariables.GetElapsedSeconds();
  this->ComputeRho(true);
  Vectors<Rational> EiBasis;
  EiBasis.MakeEiBasis(this->GetDim());
  List<bool> Explored;
  Vector<Rational> hwSimpleCoords=this->GetSimpleCoordinatesFromFundamental(inputHWfundamentalCoords);
  if (!this->GetAlLDominantWeightsHWFDIM
      (hwSimpleCoords, outputDominantWeightsSimpleCoords, UpperBoundFreudenthal, outputDetails, theGlobalVariables))
  { std::stringstream errorLog;
    errorLog << "Error: the number of dominant weights exceeded hard-coded limit of "
    << UpperBoundFreudenthal << ". Please check out whether LiE's implementation of the Freudenthal "
    << " formula can do your computation.";
    outputDetails=errorLog.str();
    return false;
  }
  Explored.initFillInObject(outputDominantWeightsSimpleCoords.size, false);
  outputMultsSimpleCoords.SetSize(outputDominantWeightsSimpleCoords.size);
  Vector<Rational> currentWeight, currentDominantRepresentative;
  Rational hwPlusRhoSquared=this->RootScalarCartanRoot
  (hwSimpleCoords+this->rho, hwSimpleCoords+this->rho);
  outputMultsSimpleCoords[0]=1;
  Explored[0]=true;
//  std::cout << "<br>time for generating weights and initializations: " << theGlobalVariables.GetElapsedSeconds()-startTimer;
  //static double totalTimeSpentOnHashIndexing=0;
//  static double timeSpentRaisingWeights=0;
  for (int k=1; k< outputDominantWeightsSimpleCoords.size; k++)
  { Explored[k]=true;
    Rational& currentAccum=outputMultsSimpleCoords[k];
    currentAccum=0;
    for (int j=0; j<this->RootsOfBorel.size; j++)
      for (int i=1; ; i++)
      { currentWeight= outputDominantWeightsSimpleCoords[k]+this->RootsOfBorel[j]*i;
        currentDominantRepresentative=currentWeight;
//        double startLocal=theGlobalVariables.GetElapsedSeconds();
        this->RaiseToDominantWeight(currentDominantRepresentative);
//        timeSpentRaisingWeights+=theGlobalVariables.GetElapsedSeconds()-startLocal;
        //double beforeHash=theGlobalVariables.GetElapsedSeconds();
        int theIndex=outputDominantWeightsSimpleCoords.GetIndex(currentDominantRepresentative);
        //totalTimeSpentOnHashIndexing+=theGlobalVariables.GetElapsedSeconds()-beforeHash;
        if (theIndex==-1)
          break;
//        std::cout << "<br> summing over weight: " << currentWeight.ElementToString();
        if (!Explored[theIndex])
        { std::stringstream errorLog;
          errorLog << "This is an internal error check. If you see it, that means "
          << " that the Freudenthal algorithm implementation is "
          << " wrong (because the author of the implementation was dumb enough to try to write less code than what is "
          << " suggested by LiE).";
          outputDetails=errorLog.str();
          return false;
        }
        currentAccum+=this->RootScalarCartanRoot(currentWeight, this->RootsOfBorel[j])
        *outputMultsSimpleCoords[theIndex];
      }
    currentAccum*=2;
    currentAccum/= hwPlusRhoSquared-
    this->RootScalarCartanRoot
    (outputDominantWeightsSimpleCoords[k]+this->rho, outputDominantWeightsSimpleCoords[k]+this->rho);
//    std::cout << "<br>Coeff we divide by: " << (hwPlusRhoSquared-this->RootScalarCartanRoot
 //   (outputDominantWeightsSimpleCoords[k]+this->rho, outputDominantWeightsSimpleCoords[k]+this->rho))
  //  .ElementToString()
   // ;
    std::stringstream out;
    out << " Computed the multiplicities of " << k+1 << " out of "
    << outputDominantWeightsSimpleCoords.size << " dominant weights in the support.";
    theGlobalVariables.MakeStatusReport(out.str());
//    std::cout
//    << "<hr> Computed the multiplicities of " << k+1 << " out of " << outputDominantWeightsSimpleCoords.size << " dominant weights in the support.";
    theGlobalVariables.MakeStatusReport(out.str());
//    std::cout << "<br>time so far: " << theGlobalVariables.GetElapsedSeconds()-startTimer;
//    std::cout << " of which " << totalTimeSpentOnHashIndexing << " used for hash routines";
//    std::cout << " of which " << timeSpentRaisingWeights << " used to raise weights";
  }
//  std::cout << "<br>Total freudenthal running time: " << theGlobalVariables.GetElapsedSeconds()-startTimer;
  return true;
}

std::string charSSAlgMod::ElementToStringCharacter
(List<Vector<Rational> >& theWeights, List<Rational>& theMults)
{ std::stringstream out;
  MonomialChar<Rational> currentSummand;
  charSSAlgMod theMod;
  theMod.MakeZero(*this->listOwners, this->indexInOwners);
  theMod.Reserve(theWeights.size);
  theMod.SetHashSizE(theWeights.size);
  assert(theMults.size==theWeights.size);
  for (int i=0; i<theWeights.size; i++)
  { currentSummand.weightFundamentalCoords=
    this->listOwners->TheObjects[this->indexInOwners].theWeyl.GetFundamentalCoordinatesFromSimple(theWeights[i]);
    currentSummand.Coefficient=theMults[i];
    theMod+=currentSummand;
  }
  out << CGI::GetHtmlMathSpanFromLatexFormulaAddBeginArrayRCL(theMod.ElementToString());
  return out.str();
}

bool charSSAlgMod::FreudenthalEvalMe
 (charSSAlgMod& outputCharOwnerSetToZero, std::string& outputDetails,
  GlobalVariables& theGlobalVariables, int upperBoundNumDominantWeights)
{ assert(&outputCharOwnerSetToZero!=this);
  outputCharOwnerSetToZero.Reset();
  Vector<Rational> currentWeightFundCoords;
  List<Rational> currentMults;
  HashedList<Vector<Rational> > currentWeights;
  std::stringstream localErrors, localDetails;
  std::string localDetail;
  MonomialChar<Rational> tempMon;
  for (int i=0; i<this->size; i++)
  { currentWeightFundCoords=this->TheObjects[i].weightFundamentalCoords;
    if (!this->GetOwner().theWeyl.FreudenthalEval
    (currentWeightFundCoords, currentWeights, currentMults, localDetail, theGlobalVariables, upperBoundNumDominantWeights))
    { localErrors << "Encountered error while evaluating freudenthal formula. Error details: " << localDetail
      << "<br> Further computation detail: " << localDetails.str();
      outputDetails=localErrors.str();
      return false;
    }
    if (localDetail!="")
      localDetails << "<br>MonomialP " << i+1 << " computation details: " << localDetail;
    for (int j=0; j<currentWeights.size; j++)
    { tempMon.weightFundamentalCoords=this->GetOwner().theWeyl.GetFundamentalCoordinatesFromSimple(currentWeights[j]);
      tempMon.Coefficient=currentMults[j]* this->TheObjects[i].Coefficient;
      outputCharOwnerSetToZero+=tempMon;
    }
  }
  outputDetails=localDetails.str();
  return true;
}

int ParserNode::EvaluateFreudenthal(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  out << "Freudenthal formula: ";
  charSSAlgMod& ch=theNode.owner->TheObjects[theArgumentList[0]].theChar.GetElement();
  charSSAlgMod finalChar;
  std::string localDetailsString;
  if (ch.FreudenthalEvalMe(finalChar, localDetailsString, theGlobalVariables, 1000000))
  { out << "resulting character in fundamental coordinates: <br>";
    out << CGI::GetHtmlMathSpanFromLatexFormulaAddBeginArrayRCL(finalChar.ElementToString("V", "\\omega", false));
  }
  out << "<br>" << localDetailsString;
  theNode.outputString=out.str();
  theNode.ExpressionType=theNode.typeString;
  return theNode.errorNoError;
}

int Parser::GetNumVarsModulePolys()
{ int result=0;
  for (int i=0; i<this->theModulePolys.size; i++)
    result=MathRoutines::Maximum(this->theModulePolys[i].GetNumVars(), result);
  return result;
}

void Parser::SetNumVarsModulePolys(int NumVars)
{ for (int i=0; i<this->theModulePolys.size; i++)
    this->theModulePolys[i].SetNumVariables(NumVars);
  for (int i=0; i<this->size; i++)
  { ParserNode& current=this->TheObjects[i];
    if (current.ExpressionType==current.typeGenVermaElt)
      current.theGenVermaElt.GetElement().SetNumVariables(NumVars);
  }
}

SemisimpleLieAlgebra& ParserNode::GetContextLieAlgebra()
{ return this->owner->theAlgebras[this->IndexContextLieAlgebra];
}

int ParserNode::EvaluateHWV(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  ParserNode& leftNode=theNode.owner->TheObjects[theArgumentList[0]];
  ParserNode& rightNode=theNode.owner->TheObjects[theArgumentList[1]];
  Vector<Rational> parSel;
  List<RationalFunction> weight;
  theNode.impliedNumVars=MathRoutines::Maximum(leftNode.impliedNumVars, rightNode.impliedNumVars);
  theNode.impliedNumVars=MathRoutines::Maximum(theNode.impliedNumVars, theNode.owner->MaxFoundVars);
  theNode.owner->SetNumVarsModulePolys(theNode.impliedNumVars);

  if (!leftNode.GetListDontUseForFunctionArguments<RationalFunction>
      (weight, theGlobalVariables, theNode.typeRationalFunction, theNode.impliedNumVars))
    return theNode.SetError(theNode.errorBadOrNoArgument);
  if (!rightNode.GetRootRationalDontUseForFunctionArguments(parSel, theGlobalVariables))
    return theNode.SetError(theNode.errorBadOrNoArgument);
  int theDim=theNode.GetContextLieAlgebra().GetRank();
  if (parSel.size!=theDim || weight.size!=theDim)
    return theNode.SetError(theNode.errorDimensionProblem);
  Vector<RationalFunction> theWeight;
  theWeight=weight;
  RationalFunction RFOne, RFZero;
  RFOne.MakeConst(theNode.impliedNumVars, 1, & theGlobalVariables);
  RFZero.MakeZero(theNode.impliedNumVars, & theGlobalVariables);
  std::string report;
  ElementTensorsGeneralizedVermas<RationalFunction>& theElt=theNode.theGenVermaElt.GetElement();
  List<ModuleSSalgebraNew<RationalFunction> >& theMods=theNode.owner->theModulePolys;
  int indexOfModule=-1;
  Selection selectionParSel;
  selectionParSel=parSel;
  for (int i=0; i<theMods.size; i++)
  { ModuleSSalgebraNew<RationalFunction>& currentMod=theMods[i];
    if (theWeight==currentMod.theHWFundamentalCoordsBaseField &&
          selectionParSel==currentMod.parabolicSelectionNonSelectedAreElementsLevi )
    { indexOfModule=i;
      break;
    }
  }
  if (indexOfModule==-1)
  { indexOfModule=theMods.size;
    theMods.SetSize(theMods.size+1);
  }
  ModuleSSalgebraNew<RationalFunction>& theMod=theMods[indexOfModule];
  if (!theMod.flagIsInitialized)
  { assert(theWeight[0].NumVars==RFOne.NumVars);
    bool isGood=theMod.MakeFromHW
    (theNode.owner->theAlgebras, 0, theWeight, parSel, theGlobalVariables, RFOne, RFZero, &report);

    out << report;
    if (!isGood)
      return theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);
  }
  theElt.MakeHWV(theMods, indexOfModule, RFOne);
  theNode.outputString=out.str();
  theNode.ExpressionType=theNode.typeGenVermaElt;
  return theNode.errorNoError;
}

bool partFractions::ArgumentsAllowed
  (Vectors<Rational>& theArguments, std::string& outputWhatWentWrong, GlobalVariables& theGlobalVariables)
{ if (theArguments.size<1)
    return false;
  Cone tempCone;
  bool result=tempCone.CreateFromVertices(theArguments, theGlobalVariables);
  if (tempCone.IsTheEntireSpace())
  { outputWhatWentWrong="Error: the vectors you gave as input span the entire space.";
    return false;
  }
  for (int i=0; i<tempCone.Vertices.size; i++)
    if (tempCone.IsInCone(tempCone.Vertices[i]) && tempCone.IsInCone(-tempCone.Vertices[i]))
    { std::stringstream out;
      out << "Error: the Q_{>0} span of vectors you gave as input contains zero (as it contains the vector "
      << tempCone.Vertices[i].ElementToString() << " as well as its opposite vector "
      << (-tempCone.Vertices[i]).ElementToString()
      << "), hence the vector partition function is " << "can only take values infinity or zero. ";
      outputWhatWentWrong=out.str();
      return false;
    }
  return result;
}

void Lattice::IntersectWithLineGivenBy(Vector<Rational>& inputLine, Vector<Rational>& outputGenerator)
{ Vectors<Rational> tempRoots;
  tempRoots.AddOnTop(inputLine);
  this->IntersectWithLinearSubspaceSpannedBy(tempRoots);
  assert(this->basisRationalForm.NumRows<=1);
  if (this->basisRationalForm.NumRows==0)
    outputGenerator.MakeZero(inputLine.size);
  else
    this->basisRationalForm.RowToRoot(0, outputGenerator);
}

void LittelmannPath::ActByEalpha(int indexAlpha)
{ Rational theMin=0;
  int minIndex=-1;
  WeylGroup& theWeyl=*this->owner;
  Vector<Rational>& alpha=theWeyl.RootsOfBorel[indexAlpha];
  Rational LengthAlpha=theWeyl.RootScalarCartanRoot(alpha, alpha);
  Vector<Rational> alphaScaled=alpha*2/LengthAlpha;
  for (int i=0; i<this->Waypoints.size; i++)
  { Rational tempRat=this->owner->RootScalarCartanRoot(this->Waypoints[i], alphaScaled);
    if (tempRat<=theMin)
    { theMin=tempRat;
      minIndex=i;
    }
  }
  if (minIndex<=0 || theMin>-1)
  { this->Waypoints.size=0;
    return;
  }
  int precedingIndex=0;
  for (int i=0; i<=minIndex; i++)
  { Rational tempScalar=theWeyl.RootScalarCartanRoot(this->Waypoints[i], alphaScaled);
    if (tempScalar>=theMin+1)
      precedingIndex=i;
    if (tempScalar<theMin+1)
      break;
  }
  Rational s2= this->owner->RootScalarCartanRoot(this->Waypoints[precedingIndex], alphaScaled);
//  std::cout << "<hr>Starting path: " << this->ElementToString();
  if (!this->MinimaAreIntegral())
    std::cout << "<br>WTF, starting path is BAD!";
//  std::cout << "<br>Min waypoint:" << this->Waypoints[minIndex].ElementToString();
//  std::cout << " with minimum: " << theMin.ElementToString();
  if (s2>theMin+1)
  { this->Waypoints.SetSize(this->Waypoints.size+1);
    for (int i=this->Waypoints.size-1; i>=precedingIndex+2; i--)
      this->Waypoints[i]=this->Waypoints[i-1];
    precedingIndex++;
    minIndex++;
//    std::cout << "<br>fake waypoint added: " << this->ElementToString();
    Rational scalarNext=theWeyl.RootScalarCartanRoot(this->Waypoints[precedingIndex],alphaScaled);
    Vector<Rational>& r1=this->Waypoints[precedingIndex];
    Vector<Rational>& r2=this->Waypoints[precedingIndex-1];
    Rational s1=theWeyl.RootScalarCartanRoot(r1, alphaScaled);
    Rational x= (theMin+1-s2)/(s1-s2);
    this->Waypoints[precedingIndex]= (r1-r2)*x+ r2;
//    std::cout << "<br> fake waypoint corrected: " << this->ElementToString();
  }
//  std::cout << "<br>Min waypoint:" << this->Waypoints[minIndex].ElementToString();
  Vectors<Rational> differences;
  differences.SetSize(minIndex-precedingIndex);
  Rational currentDist=0;
  Rational minDist=0;
  for (int i=0; i<differences.size; i++)
  { differences[i]=this->Waypoints[i+precedingIndex+1]-this->Waypoints[i+precedingIndex];
    currentDist+=theWeyl.RootScalarCartanRoot(differences[i], alphaScaled);
    if (currentDist<minDist)
    { theWeyl.SimpleReflection(indexAlpha, differences[i], false, false);
      minDist=currentDist;
    }
  }
  for (int i=0; i<differences.size; i++)
    this->Waypoints[i+precedingIndex+1]=this->Waypoints[i+precedingIndex]+differences[i];
  for (int i=minIndex+1; i<this->Waypoints.size; i++)
    this->Waypoints[i]+=alpha;
//  std::cout << "<br> result before simplification: " << this->ElementToString();
  this->Simplify();
//  std::cout << "<br> final: " << this->ElementToString();
}

void LittelmannPath::ActByFalpha(int indexAlpha)
{ if (this->Waypoints.size==0)
    return;
  Rational theMin=0;
  int minIndex=-1;
  WeylGroup& theWeyl=*this->owner;
  Vector<Rational>& alpha=theWeyl.RootsOfBorel[indexAlpha];
  Rational LengthAlpha=this->owner->RootScalarCartanRoot(alpha, alpha);
  Vector<Rational> alphaScaled=alpha*2/LengthAlpha;
  for (int i=0; i<this->Waypoints.size; i++)
  { Rational tempRat=this->owner->RootScalarCartanRoot(this->Waypoints[i], alphaScaled);
    if (tempRat<=theMin)
    { theMin=tempRat;
      minIndex=i;
    }
  }
  Rational lastScalar=this->owner->RootScalarCartanRoot(*this->Waypoints.LastObject(), alphaScaled);
  if (minIndex<0 || lastScalar-theMin<1)
  { this->Waypoints.size=0;
    return;
  }
  int succeedingIndex=0;
  for (int i=this->Waypoints.size-1; i>=minIndex; i--)
  { Rational tempScalar=theWeyl.RootScalarCartanRoot(alphaScaled, this->Waypoints[i]);
    if (tempScalar>=theMin+1)
      succeedingIndex=i;
    if (tempScalar<theMin+1)
      break;
  }
  Rational s1= this->owner->RootScalarCartanRoot(this->Waypoints[succeedingIndex], alphaScaled);
  if (s1>theMin+1)
  { this->Waypoints.SetSize(this->Waypoints.size+1);
    for (int i=this->Waypoints.size-1; i>=succeedingIndex+1; i--)
      this->Waypoints[i]=this->Waypoints[i-1];
    Rational scalarNext=theWeyl.RootScalarCartanRoot(this->Waypoints[succeedingIndex], alphaScaled);
    Vector<Rational>& r1=this->Waypoints[succeedingIndex];
    Vector<Rational>& r2=this->Waypoints[succeedingIndex-1];
    Rational s2=theWeyl.RootScalarCartanRoot(r2, alphaScaled);
    Rational x= (theMin+1-s2)/(s1-s2);
    this->Waypoints[succeedingIndex]= (r1-r2)*x+ r2;
  }
  Vectors<Rational> differences;
  differences.SetSize(succeedingIndex-minIndex);
  Rational distMax=0;
  Rational currentDist=0;
  for (int i=0; i<differences.size; i++)
  { differences[i]=this->Waypoints[i+minIndex+1]-this->Waypoints[i+minIndex];
    currentDist+=theWeyl.RootScalarCartanRoot(differences[i], alphaScaled);
    if (currentDist>distMax)
    { theWeyl.SimpleReflection(indexAlpha, differences[i], false, false);
      currentDist=distMax;
    }
  }
  for (int i=0; i<differences.size; i++)
    this->Waypoints[i+minIndex+1]=this->Waypoints[i+minIndex]+differences[i];
  for (int i=succeedingIndex+1; i<this->Waypoints.size; i++)
    this->Waypoints[i]-=alpha;
  this->Simplify();
}

void LittelmannPath::Simplify()
{ Rational strech;
  Vectors<Rational> output;
  Vector<Rational> tempRoot1, tempRoot2;
  output.Reserve(this->Waypoints.size);
  for (int i=0; i<this->Waypoints.size; i++)
  { Vector<Rational>& current=this->Waypoints[i];
    if (output.size==0)
      output.AddOnTop(current);
    else if (output.size==1)
    { if (current!=output[0])
        output.AddOnTop(current);
    } else if (output.size>1)
    { Vector<Rational>& preceding =*output.LastObject();
      Vector<Rational>& precedingThePreceding=output[output.size-2];
      tempRoot1=preceding-precedingThePreceding;
      tempRoot2=current-preceding;
//      std::cout << "preceding- preceding the preceding: " << tempRoot1.ElementToString();
//      std::cout << "<br>current- preceding : " << tempRoot2.ElementToString() << "<hr>";
      if ( tempRoot1.IsProportionalTo(tempRoot2, strech))
      { if (strech.IsNegative())
          output.AddOnTop(current);
        else
          *output.LastObject()=current;
      } else
       output.AddOnTop(current);
    }
  }
  this->Waypoints=output;
}

bool LittelmannPath::MinimaAreIntegral()
{ if (this->Waypoints.size==0)
    return true;
  List<Rational> theMinima;
  WeylGroup& theWeyl= *this->owner;
  int theDim=theWeyl.GetDim();
  theMinima.SetSize(theDim);
  for (int i=0; i<theDim; i++)
    theMinima[i]=
    theWeyl.GetScalarProdSimpleRoot
    (this->Waypoints[0], i)*2/theWeyl.CartanSymmetric.elements[i][i];
  for (int i=1; i<this->Waypoints.size; i++)
    for (int j=0; j<theDim; j++)
      theMinima[j]=MathRoutines::Minimum
      (theWeyl.GetScalarProdSimpleRoot
      (this->Waypoints[i],j)*2/theWeyl.CartanSymmetric.elements[j][j], theMinima[j]);
  for (int i=0; i<theDim; i++)
    if (!theMinima[i].IsSmallInteger())
      return false;
  return true;
}

int ParserNode::EvaluateLittelmannPaths
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ //std::stringstream out;
  Vector<Rational> theWeight;
  if (!theNode.GetRootRationalFromFunctionArguments(theWeight, theGlobalVariables))
    return theNode.SetError(theNode.errorBadOrNoArgument);
  WeylGroup& theWeyl=theNode.owner->theHmm.theRange().theWeyl;
  if (theWeight.size!=theWeyl.GetDim())
    return theNode.SetError(theNode.errorDimensionProblem);
  Vector<Rational> theWeightInSimpleCoords= theWeyl.GetSimpleCoordinatesFromFundamental(theWeight);
  LittelmannPath& thePath=theNode.theLittelmann.GetElement();
  thePath.MakeFromWeightInSimpleCoords(theWeightInSimpleCoords, theWeyl);
//  theNode.outputString=out.str();
  theNode.ExpressionType=theNode.typeLittelman;
  return theNode.errorNoError;
}

int ParserNode::EvaluateLittelmannPathFromWayPoints
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ //std::stringstream out;
  Vectors<Rational> theWeights;
  int theDim;
  if (!theNode.GetRootsEqualDimNoConversionNoEmptyArgument(theArgumentList, theWeights, theDim))
    return theNode.SetError(theNode.errorBadOrNoArgument);
  WeylGroup& theWeyl=theNode.owner->theHmm.theRange().theWeyl;
  if (theDim!=theWeyl.GetDim())
    return theNode.SetError(theNode.errorDimensionProblem);
//  for (int i=0; i<theWeights.size; i++)
//    theWeights[i]= theWeyl.GetSimpleCoordinatesFromFundamental(theWeights[i]);
  LittelmannPath& thePath=theNode.theLittelmann.GetElement();
  thePath.MakeFromWaypoints(theWeights, theWeyl);
//  theNode.outputString=out.str();
  theNode.ExpressionType=theNode.typeLittelman;
  return theNode.errorNoError;
}

int ParserNode::EvaluateLittelmannEAlpha
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ WeylGroup& theWeyl=theNode.owner->theHmm.theRange().theWeyl;
  int theIndex=theNode.owner->TheObjects[theArgumentList[0]].intValue;
  if (theIndex>theWeyl.GetDim() || theIndex==0 || theIndex< - theWeyl.GetDim())
    return theNode.SetError(theNode.errorBadIndex);
  LittelmannPath& thePath=theNode.theLittelmann.GetElement();
  thePath=theNode.owner->TheObjects[theArgumentList[1]].theLittelmann.GetElement();
  if (theIndex<0)
    thePath.ActByFalpha(-theIndex-1);
  else
    thePath.ActByEalpha(theIndex-1);
  theNode.ExpressionType=theNode.typeLittelman;
  return theNode.errorNoError;
}

int ParserNode::EvaluateAllLittelmannPaths
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ WeylGroup& theWeyl=theNode.owner->theHmm.theRange().theWeyl;
  theNode.EvaluateLittelmannPaths(theNode, theArgumentList, theGlobalVariables);
  if (theNode.ErrorType!=theNode.errorNoError)
    return theNode.ErrorType;
  LittelmannPath& thePath=theNode.theLittelmann.GetElement();
  List<LittelmannPath> allPaths;
  std::stringstream out;
  List<List<int> > outputIndices;
  if (thePath.GenerateOrbit(allPaths, outputIndices, theGlobalVariables, 1000))
    out << "Number of paths: " << allPaths.size;
  else
    out << "<b>Number of paths has been trimmed to " << allPaths.size
    << " (the hard-coded limit for a path's orbit size is " << 1000 << ")</b>";
  out << "; Weyl dimension formula: "
  << theWeyl.WeylDimFormulaFromSimpleCoords(*thePath.Waypoints.LastObject()).ElementToString();
  out << ".<br> A printout of the paths follows. The waypoints of the Littelmann paths are given in simple coordinates.";
  for (int i=0; i<allPaths.size; i++)
  { out << "\n<br>\n" << allPaths[i].ElementToString();
    out << " corresponds to sequence "
    << allPaths[0].ElementToStringOperatorSequenceStartingOnMe(outputIndices[i]);
  }
  theNode.outputString=out.str();
  return theNode.errorNoError;
}

void LittelmannPath::MakeFromWeightInSimpleCoords
  (Vector<Rational>& weightInSimpleCoords, WeylGroup& theOwner)
{ this->owner=& theOwner;
  if (weightInSimpleCoords.IsEqualToZero())
  { this->Waypoints.SetSize(1);
    this->Waypoints[0]=weightInSimpleCoords;
    return;
  }
  this->Waypoints.SetSize(2);
  this->Waypoints[0].MakeZero(theOwner.GetDim());
  this->Waypoints[1]=weightInSimpleCoords;
  this->Simplify();
}

std::string LittelmannPath::ElementToStringIndicesToCalculatorOutput
(LittelmannPath& inputStartingPath, List<int> & input)
{ std::stringstream out;
  for (int i=input.size-1; i>=0; i--)
  { int displayIndex= input[i];
    if (displayIndex>=0)
      displayIndex++;
    out << "eAlpha(" << displayIndex << ", ";
  }
  out << "littelmann"
  << inputStartingPath.owner->GetFundamentalCoordinatesFromSimple
  (*inputStartingPath.Waypoints.LastObject() ).ElementToString();
  for (int i=0; i<input.size; i++)
    out << " ) ";
  return out.str();
}

bool LittelmannPath::GenerateOrbit
(List<LittelmannPath>& output, List<List<int> >& outputOperators, GlobalVariables& theGlobalVariables, int UpperBoundNumElts,
 Selection* parabolicNonSelectedAreInLeviPart)
{ HashedList<LittelmannPath> hashedOutput;
  hashedOutput.AddOnTop(*this);
  int theDim=this->owner->GetDim();
  outputOperators.SetSize(1);
  outputOperators[0].SetSize(0);
  List<int> currentSequence;
  if (UpperBoundNumElts>0)
    currentSequence.Reserve(UpperBoundNumElts);
  LittelmannPath currentPath;
  bool result=true;
  Selection parabolicSelectionSelectedAreInLeviPart;
  parabolicSelectionSelectedAreInLeviPart.init(theDim);
  if (parabolicNonSelectedAreInLeviPart!=0)
  { parabolicSelectionSelectedAreInLeviPart=*parabolicNonSelectedAreInLeviPart;
    parabolicSelectionSelectedAreInLeviPart.InvertSelection();
  }
  else
    parabolicSelectionSelectedAreInLeviPart.MakeFullSelection();
  for (int lowestNonExplored=0; lowestNonExplored<hashedOutput.size; lowestNonExplored++)
    if (UpperBoundNumElts>0 && UpperBoundNumElts< hashedOutput.size)
    { result=false;
      break;
    }
    else
      for (int j=0; j<parabolicSelectionSelectedAreInLeviPart.CardinalitySelection; j++)
      { bool found=true;
        currentPath=hashedOutput[lowestNonExplored];
        currentSequence=outputOperators[lowestNonExplored];
        int theIndex=parabolicSelectionSelectedAreInLeviPart.elements[j];
        while (found)
        { found=false;
          currentPath.ActByEalpha(theIndex);
          if (!currentPath.IsEqualToZero())
  //          hashedOutput.AddNoRepetition(currentPath);
            if (hashedOutput.AddNoRepetition(currentPath))
            { found=true;
              currentSequence.AddOnTop(theIndex);
              outputOperators.AddOnTop(currentSequence);
              if (!currentPath.MinimaAreIntegral())
              { std::cout << "<hr>Found a bad path:<br> ";
                std::cout << " = " << currentPath.ElementToString();
              }
            }
        }
        found=true;
        currentPath=hashedOutput[lowestNonExplored];
        currentSequence=outputOperators[lowestNonExplored];
        while (found)
        { found=false;
          currentPath.ActByFalpha(theIndex);
          if (!currentPath.IsEqualToZero())
            //hashedOutput.AddNoRepetition(currentPath);
            if (hashedOutput.AddNoRepetition(currentPath))
            { found=true;
              currentSequence.AddOnTop(-theIndex-1);
              outputOperators.AddOnTop(currentSequence);
              if (!currentPath.MinimaAreIntegral())
              { std::cout << "<hr>Found a bad path:<br> ";
                std::cout << " = " << currentPath.ElementToString();
              }
            }
        }
      }
  output.CopyFromBase(hashedOutput);
  return result;
}

std::string LittelmannPath:: ElementToStringOperatorSequenceStartingOnMe
(List<int> & input)
{ std::stringstream out;
  int currentCount=0;
  for (int i=input.size-1; i>=0; i--)
  { int displayIndexNext=0;
    if (i>0)
      displayIndexNext=(input[i-1]<0) ? input[i-1]: input[i]+1;
    int displayIndexCurrent= (input[i]<0) ? input[i] : input[i]+1;
    currentCount++;
    if (displayIndexNext!=displayIndexCurrent)
    { out << "g_{" << displayIndexCurrent << "}";
      if (currentCount>1)
        out << "^{" << currentCount << "}";
      currentCount=0;
    }
  }
  return out.str();
}

int ParserNode::EvaluateRepresentationFromHWFundCoords
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ Vector<Rational> theWeight;
  WeylGroup& theWeyl=theNode.owner->theHmm.theRange().theWeyl;
  if (!theNode.GetRootRationalFromFunctionArguments(theWeight, theGlobalVariables))
    return theNode.SetError(theNode.errorBadOrNoArgument);
  if (theWeight.size!=theWeyl.GetDim())
  { //std::cout << "read dim: " << theWeight.size << " not equal to " << theWeyl.GetDim();
    return theNode.SetError(theNode.errorDimensionProblem);
  }
  std::stringstream out;
  std::string report;
  ModuleSSalgebraNew<Rational> theModule;
  Vector<Rational> fullParSel;
  fullParSel.MakeZero(theWeight.size);
  if (!theModule.MakeFromHW
      (theNode.owner->theAlgebras, 0, theWeight, fullParSel, theGlobalVariables, 1, 0, &report))
  { theNode.outputString=report;
    return theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);
  }
  out << report;
  theNode.outputString=out.str();
  theNode.ExpressionType=theNode.typeString;
  return theNode.errorNoError;
}


template<class CoefficientType>
void ModuleSSalgebraNew<CoefficientType>::IntermediateStepForMakeFromHW
  (List<CoefficientType>& HWDualCoordS, GlobalVariables& theGlobalVariables,
    const CoefficientType& theRingUnit, const CoefficientType& theRingZero
   )
{ WeylGroup& theWeyl=this->GetOwner().theWeyl;
  int theDim=theWeyl.GetDim();
  int numPosRoots=theWeyl.RootsOfBorel.size;
  Vector<Rational> targetWeight;
  this->theSimpleGens.SetSize(theDim*2);
  this->weightsSimpleGens.SetSize(theDim*2);
  for (int k=0; k<2; k++)
    for (int i=0; i<theDim; i++)
    { int genChevIndex=(k==0) ? numPosRoots-i-1 : numPosRoots+theDim+i;
      ElementUniversalEnveloping<CoefficientType>& theSimpleGenerator= this->theSimpleGens[k*theDim+i];
      Vector<Rational>& simpleGeneratorWeight=this->weightsSimpleGens[k*theDim+i];
      simpleGeneratorWeight=theWeyl.RootsOfBorel[i];
      if (k==0)
        simpleGeneratorWeight.Minus();
      theSimpleGenerator.MakeOneGenerator(genChevIndex, *this->theAlgebras, this->indexAlgebra, theRingUnit);
    }
  this->theBilinearFormsAtEachWeightLevel.SetSize(this->theGeneratingWordsGrouppedByWeight.size);
  this->theBilinearFormsInverted.SetSize(this->theGeneratingWordsGrouppedByWeight.size);
  for (int l=0; l<this->theGeneratingWordsGrouppedByWeight.size; l++)
  { Matrix<CoefficientType>& currentBF=this->theBilinearFormsAtEachWeightLevel[l];
    List<MonomialUniversalEnveloping<CoefficientType> >& currentWordList=
    this->theGeneratingWordsGrouppedByWeight[l];
    currentBF.init(currentWordList.size, currentWordList.size);
    for (int i=0; i<currentWordList.size; i++)
      for (int j=i; j<currentWordList.size; j++)
      { currentWordList[i].HWTAAbilinearForm
        (currentWordList[j], currentBF.elements[i][j], &HWDualCoordS, theGlobalVariables, theRingUnit, theRingZero, 0)
        ;
        if (i!=j)
          currentBF.elements[j][i]=currentBF.elements[i][j];
      }
    Matrix<CoefficientType> tempMat;
    tempMat=(currentBF);
    CoefficientType tempRat;
    tempMat.ComputeDeterminantOverwriteMatrix(tempRat, theRingUnit, theRingZero);
    if (!tempRat.IsEqualToZero())
    { this->theBilinearFormsInverted[l]=currentBF;
      this->theBilinearFormsInverted[l].Invert(theRingUnit, theRingZero);
    } else
      this->theBilinearFormsInverted[l].init(0,0);
  }
}

template <class CoefficientType>
void ModuleSSalgebraNew<CoefficientType>::ExpressAsLinearCombinationHomogenousElement
  (ElementUniversalEnveloping<CoefficientType>& inputHomogeneous,
   ElementUniversalEnveloping<CoefficientType>& outputHomogeneous,
   int indexInputBasis, const List<CoefficientType>& subHiGoesToIthElement,
   GlobalVariables& theGlobalVariables, const CoefficientType& theRingUnit, const CoefficientType& theRingZero
   )
{ Vector <CoefficientType> theScalarProducts;
  List<MonomialUniversalEnveloping<CoefficientType> >& inputBasis =
  this->theGeneratingWordsGrouppedByWeight[indexInputBasis];
  theScalarProducts.SetSize(inputBasis.size);
//  std::cout << "<hr>Expressing " << inputHomogeneous.ElementToString
//  (theGlobalVariables, theGlobalVariables.theDefaultFormat);

  for (int i=0; i<inputBasis.size; i++)
  { std::stringstream tempStream;
//    std::cout << "<br>(" << inputHomogeneous.ElementToString(false, false, theGlobalVariables, theGlobalVariables.theDefaultFormat)
//    << "," << inputBasis[i].ElementToString(false, false, theGlobalVariables, theGlobalVariables.theDefaultFormat)
//    << " )=";
//    std::cout << theScalarProducts[i].ElementToString() ;

    inputHomogeneous.HWTAAbilinearForm
    (inputBasis[i], theScalarProducts[i], &subHiGoesToIthElement, theGlobalVariables, theRingUnit, theRingZero, &tempStream);
/*    std::cout << "<br>Computation details: " << tempStream.str();
    if ( inputHomogeneous.ElementToString(false, false, theGlobalVariables, theGlobalVariables.theDefaultFormat)=="x_{-1}"
        &&  theScalarProducts[i].ElementToString()=="0"
        )
    { std::cout << "<b>Problem here!</b>";
      inputHomogeneous.HWTAAbilinearForm
      (inputBasis[i], theScalarProducts[i], &subHiGoesToIthElement, theGlobalVariables, theRingUnit, theRingZero, &tempStream);
    }*/

  }
  this->theBilinearFormsInverted[indexInputBasis].ActOnVectorColumn(theScalarProducts, theRingZero);
  outputHomogeneous.MakeZero(*this->theAlgebras, this->indexAlgebra);
  ElementUniversalEnveloping<CoefficientType> tempElt;
  for (int i=0; i<theScalarProducts.size; i++)
  { tempElt.MakeZero(*this->theAlgebras, this->indexAlgebra);
    tempElt.AddMonomial(this->theGeneratingWordsGrouppedByWeight[indexInputBasis][i], theRingUnit);
    tempElt*=theScalarProducts[i];
    outputHomogeneous+=tempElt;
  }
}

template <class CoefficientType>
void ModuleSSalgebraNew<CoefficientType>::GetAdActionHomogenousElT
  (ElementUniversalEnveloping<CoefficientType>& inputHomogeneous, Vector<Rational>& weightUEEltSimpleCoords,
   List<List<ElementUniversalEnveloping<CoefficientType> > >& outputSortedByArgumentWeight,
   GlobalVariables& theGlobalVariables, const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{ Vector<Rational> targetWeight;
  outputSortedByArgumentWeight.SetSize(this->theGeneratingWordsGrouppedByWeight.size);
  ElementUniversalEnveloping<CoefficientType> theElt;
  for (int i=0; i<this->theGeneratingWordsGrouppedByWeight.size; i++)
  { List<MonomialUniversalEnveloping<CoefficientType> >& currentWordList=
    this->theGeneratingWordsGrouppedByWeight[i];
    List<ElementUniversalEnveloping<CoefficientType> >& outputCurrentList=
    outputSortedByArgumentWeight[i];
    outputCurrentList.SetSize(currentWordList.size);
    Vector<Rational>& currentWeight=this->theModuleWeightsSimpleCoords[i];
    targetWeight=currentWeight+weightUEEltSimpleCoords;
//    std::cout << "<br>target weight: " << targetWeight.ElementToString() << "="
//    << currentWeight.ElementToString() << "+" << inputWeight.ElementToString();
    int theIndex=this->theModuleWeightsSimpleCoords.GetIndex(targetWeight);
    for (int j=0; j<currentWordList.size; j++)
    { ElementUniversalEnveloping<CoefficientType>& currentOutputWord=outputCurrentList[j];
      if (theIndex==-1)
        currentOutputWord.MakeZero(*this->theAlgebras, this->indexAlgebra);
      else
      { theElt=inputHomogeneous;
        theElt.MultiplyBy(currentWordList[j], theRingUnit);
        this->ExpressAsLinearCombinationHomogenousElement
        (theElt, currentOutputWord, theIndex, this->theHWDualCoordsBaseField, theGlobalVariables, theRingUnit, theRingZero);
      }
    }
  }
}

template<class CoefficientType>
bool ModuleSSalgebraNew<CoefficientType>::MakeFromHW
(List<SemisimpleLieAlgebra>& inputAlgebras, int inputIndexAlgebra, Vector<CoefficientType>& HWFundCoords,
 const Selection& selNonSelectedAreElementsLevi, GlobalVariables& theGlobalVariables,
const CoefficientType& theRingUnit, const CoefficientType& theRingZero,
 std::string* outputReport)
{ this->theAlgebras=&inputAlgebras;
  this->indexAlgebra=inputIndexAlgebra;
  SemisimpleLieAlgebra& theAlgebrA=inputAlgebras[this->indexAlgebra];

  int theRank=theAlgebrA.GetRank();
  assert(HWFundCoords.size==theRank && selNonSelectedAreElementsLevi.MaxSize==theRank);
  WeylGroup& theWeyl=theAlgebrA.theWeyl;

  this->parabolicSelectionNonSelectedAreElementsLevi=selNonSelectedAreElementsLevi;
  this->parabolicSelectionSelectedAreElementsLevi=this->parabolicSelectionNonSelectedAreElementsLevi;
  this->parabolicSelectionSelectedAreElementsLevi.InvertSelection();

  this->theHWFundamentalCoordsBaseField=HWFundCoords;
  this->theHWDualCoordsBaseField.SetSize(theRank);
  this->theHWFundamentalCoordS.SetSize(theRank);

  for (int i=0; i<theRank; i++)
  { this->theHWFundamentalCoordS[i]=0;
    if (this->parabolicSelectionSelectedAreElementsLevi.selected[i])
    { int theCoord;
      if (!this->theHWFundamentalCoordsBaseField[i].IsSmallInteger(theCoord))
      { if (outputReport!=0)
          *outputReport="The given module over the Levi part is not finite dimensional";
        return false;
      }
      this->theHWFundamentalCoordS[i]=theCoord;
    }
    this->theHWDualCoordsBaseField[i]=this->theHWFundamentalCoordsBaseField[i];
    this->theHWDualCoordsBaseField[i]*=theWeyl.CartanSymmetric.elements[i][i]/2;
  }

  this->theHWSimpleCoordS=theWeyl.GetSimpleCoordinatesFromFundamental(this->theHWFundamentalCoordS);
  this->theHWDualCoordS= theWeyl.GetDualCoordinatesFromFundamental(this->theHWFundamentalCoordS);

/*  std::cout << "<br>input fund coords base field: " << HWFundCoords.ElementToString();
  std::cout << "<br>dual coords no base field: " << this->theHWDualCoordS.ElementToStringGeneric();
  std::cout << "<br>dual coords: " << this->theHWDualCoordsBaseField.ElementToStringGeneric();
  std::cout << "<br>fund coords no base field: " << this->theHWFundamentalCoordS.ElementToString();
  std::cout << "<br>fund coords: " << this->theHWFundamentalCoordsBaseField.ElementToString();
  std::cout << "<br>simple coords no base field: " << this->theHWSimpleCoordS.ElementToString();
*/
  this->theChaR.MakeFromWeight(this->theHWSimpleCoordS, *this->theAlgebras, this->indexAlgebra);

  LittelmannPath startingPath;
  startingPath.MakeFromWeightInSimpleCoords(this->theHWSimpleCoordS, theWeyl);
  List<LittelmannPath> thePaths;
  List<List<int> > generatorsIndices;
  if (!startingPath.GenerateOrbit
      (thePaths, generatorsIndices, theGlobalVariables, 1000, & this->parabolicSelectionNonSelectedAreElementsLevi))
  { if (outputReport!=0)
      *outputReport = "Error: number of Littelmann paths exceeded allowed limit of 1000.";
    return false;
  }
  this->theModuleWeightsSimpleCoords.Clear();
  MonomialChar<Rational> tempCharMon;
  tempCharMon.Coefficient=1;
  this->theCharOverH.Reset();
  for (int i=0; i<thePaths.size; i++)
  { this->theModuleWeightsSimpleCoords.AddNoRepetition(*thePaths[i].Waypoints.LastObject());
    tempCharMon.weightFundamentalCoords= theWeyl.GetFundamentalCoordinatesFromSimple(*thePaths[i].Waypoints.LastObject());
    this->theCharOverH+=tempCharMon;
  }
  this->theModuleWeightsSimpleCoords.QuickSortAscending();
  std::stringstream out2, monomialDetailStream;
  if (outputReport!=0)
  { monomialDetailStream << "Number of Littelmann paths: " << thePaths.size;
    monomialDetailStream << "<br>Let v denote the highest weight vector of highest weight in simple coordinates "
    << this->theHWSimpleCoordS.ElementToString();
    std::string tempS;
    DrawingVariables theDV;
    this->theCharOverH.DrawMeAssumeCharIsOverCartan(theWeyl, theGlobalVariables, theDV);
    out2 << " A picture of the weight support follows. "
    << theDV.GetHtmlFromDrawOperationsCreateDivWithUniqueName(theWeyl.GetDim());
    monomialDetailStream << "<br>Then the elements corresponding to the Littelmann paths are as follows. ";
  }
  ElementUniversalEnveloping<CoefficientType> tempElt;
  PolynomialOutputFormat tempFormat;
  tempFormat.MakeAlphabetArbitraryWithIndex("g", "h");

  this->theGeneratingWordsGrouppedByWeight.SetSize(this->theModuleWeightsSimpleCoords.size);
  for (int i=0; i<this->theGeneratingWordsGrouppedByWeight.size; i++)
    this->theGeneratingWordsGrouppedByWeight[i].size=0;
  MonomialUniversalEnveloping<CoefficientType> currentNonReducedElement;
  for (int i=0; i<thePaths.size; i++)
  { List<int>& currentPath= generatorsIndices[i];
    currentNonReducedElement.MakeConst(*this->theAlgebras, this->indexAlgebra);
    for (int j=currentPath.size-1; j>=0; j--)
    { int theIndex=currentPath[j];
      if (theIndex>0)
        theIndex++;
      currentNonReducedElement.MultiplyByGeneratorPowerOnTheRight
      (this->GetOwner().DisplayIndexToChevalleyGeneratorIndex(theIndex), theRingUnit);
    }
    Vector<Rational>& hwCurrent=*thePaths[i].Waypoints.LastObject();
    int theIndex=this->theModuleWeightsSimpleCoords.GetIndex(hwCurrent);
    if (theIndex==-1)
    { std::cout << "couldn't find weight : " << hwCurrent.ElementToString() << " in " << this->theModuleWeightsSimpleCoords.ElementToString();
      out2 << "Error: could not generate all weights in the weight support. Maybe they are too many? Allowed "
      << " # of weights is 10000";
      if (outputReport!=0)
        *outputReport=out2.str() + monomialDetailStream.str();
      return false;
    }
    this->theGeneratingWordsGrouppedByWeight[theIndex].AddOnTop(currentNonReducedElement);
  }
  this->theGeneratingWordsNonReduced.Clear();
  this->theGeneratingWordsNonReduced.SetExpectedSize(thePaths.size);
  this->theGeneratingWordsNonReduced.size=0;
  this->theGeneratingWordsNonReducedWeights.SetSize(0);
  this->theGeneratingWordsNonReducedWeights.Reserve(thePaths.size);
  int wordCounter=-1;
  for (int i=0; i<this->theGeneratingWordsGrouppedByWeight.size; i++)
  { List<MonomialUniversalEnveloping<CoefficientType> >& currentList=this->theGeneratingWordsGrouppedByWeight[i];
    currentList.QuickSortDescending();
    for (int j=0; j<currentList.size; j++)
    { wordCounter++;
      this->theGeneratingWordsNonReduced.AddOnTop(currentList[j]);
      this->theGeneratingWordsNonReducedWeights.AddOnTop(this->theModuleWeightsSimpleCoords[i]);
      if (outputReport!=0)
      { monomialDetailStream << "<br>m_{ " << this->theGeneratingWordsNonReduced.size << "} := "
        << currentList[j].ElementToString(false, false, theGlobalVariables, tempFormat) << "  v( "
        << this->theHWFundamentalCoordsBaseField.ElementToString() << ", "
        << Vector<Rational>(this->parabolicSelectionNonSelectedAreElementsLevi).ElementToString() << ")";
;
      }
    }
  }
  this->IntermediateStepForMakeFromHW(this->theHWDualCoordsBaseField, theGlobalVariables, theRingUnit, theRingZero);
  bool isBad=false;
  if (outputReport!=0)
    for (int i=0; i<this->theBilinearFormsAtEachWeightLevel.size; i++)
    { Matrix<CoefficientType>& theBF=this->theBilinearFormsAtEachWeightLevel[i];
      Matrix<CoefficientType>& theBFinverted= this->theBilinearFormsInverted[i];
      if (outputReport!=0)
      { monomialDetailStream << "<hr>weight in simple coords: " << this->theModuleWeightsSimpleCoords[i].ElementToString();
        List<MonomialUniversalEnveloping<CoefficientType> >& currentList=this->theGeneratingWordsGrouppedByWeight[i];
        for (int i=0; i<currentList.size; i++)
        { MonomialUniversalEnveloping<CoefficientType>& currentElt=currentList[i];
          monomialDetailStream << "<br>monomial " << i+1 << ": "
          << currentElt.ElementToString(false, false, theGlobalVariables, tempFormat);
        }
        monomialDetailStream << "; Matrix of Shapovalov form associated to current weight level: <br> "
        << theBF.ElementToString(true, false) << " corresonding inverted matrix:<br>";
      }
      if (theBFinverted.NumRows>0)
      { if (outputReport!=0)
          monomialDetailStream << theBFinverted.ElementToString(true, false);
      }
      else
      { if (outputReport!=0)
          monomialDetailStream << "<b>The matrix of the bilinear form is not invertible!</b>";
        isBad=true;
      }
    }
  if (isBad)
  { if(outputReport!=0)
      monomialDetailStream << "<br>Error: the Littelmann-path induced monomials do not give a monomial basis. ";
    *outputReport=out2.str()+monomialDetailStream.str();
    return false;
  }
  this->actionsSimpleGens.SetSize(this->GetOwner().GetRank()*2);
  this->actionsSimpleGensMatrixForM.SetSize(this->GetOwner().GetRank()*2);
  for (int k=0; k<2; k++)
    for (int j=0; j<this->GetOwner().GetRank(); j++)
      if (this->parabolicSelectionSelectedAreElementsLevi.selected[j])
      { int theIndex=j+k*this->GetOwner().GetRank();
        ElementUniversalEnveloping<CoefficientType>& theSimpleGenerator=this->theSimpleGens[theIndex];
        Vector<Rational>& simpleWeight=this->weightsSimpleGens[theIndex];
        List<List<ElementUniversalEnveloping<CoefficientType> > >& currentAction= this->actionsSimpleGens[theIndex];
        if (outputReport!=0)
          out2 << "<hr>Simple generator: " << theSimpleGenerator.ElementToString(theGlobalVariables, tempFormat);
        Matrix<CoefficientType>& theMatrix=this->actionsSimpleGensMatrixForM[theIndex];
        this->GetMatrixHomogenousElt
        (theSimpleGenerator, currentAction, simpleWeight, theMatrix, theGlobalVariables,
         theRingUnit, theRingZero);
        if (outputReport!=0)
          out2 << "<br>Matrix of elemenent in the m_i basis:<br>"
          << CGI::GetHtmlMathSpanFromLatexFormula(theMatrix.ElementToString(false, true));
    /*    for (int j=0; j<this->actionsSimpleGens[i].size; j++)
          for (int k=0; k<this->actionsSimpleGens[i][j].size; k++)
          { out << "<br>" << theSimpleGenerator.ElementToString(theGlobalVariables, tempFormat) << "\\cdot "
            << this->theGeneratingWordsGrouppedByWeight[j][k].ElementToString(false, false, theGlobalVariables, tempFormat)
            << "\\cdot v=" << this->actionsSimpleGens[i][j][k].ElementToString(theGlobalVariables, tempFormat)
            << "\\cdot v"
            ;
          }*/
      }
    else
      this->actionsSimpleGensMatrixForM[j+k*this->GetOwner().GetRank()].init(0,0);
  this->ComputedGeneratorActions.init(this->GetOwner().GetNumGenerators());
  this->actionsGeneratorS.SetSize(this->GetOwner().GetNumGenerators());
  this->actionsGeneratorsMaT.SetSize(this->GetOwner().GetNumGenerators());
  if (outputReport!=0)
  { std::stringstream latexTableStream;
    Vector<CoefficientType> tempV;
    tempV=this->theHWFundamentalCoordsBaseField;
    for (int i=0; i<tempV.size; i++)
      tempV[i]-=this->theHWFundamentalCoordS[i];
    latexTableStream << "<hr>Ready copy +paste for your .tex file:<br> <br>"
    << "\\begin{tabular}{lll";
    for (int i=0; i<this->parabolicSelectionSelectedAreElementsLevi.CardinalitySelection; i++)
      latexTableStream <<"l";
    latexTableStream << "} \n\\hline\\hline \\multicolumn{"
    << this->parabolicSelectionSelectedAreElementsLevi.CardinalitySelection+3
    << "}{|c|}{ Highest weight $\\lambda="
    << tempV.ElementToStringLetterFormat("\\omega", false, false)
    << "+" << theWeyl.GetEpsilonCoords(this->theHWSimpleCoordS, theGlobalVariables).ElementToStringEpsilonForm(true, false)
    << "$}\\\\\\hline Element& weight & monomial expression";
    for (int i=0; i<this->parabolicSelectionSelectedAreElementsLevi.CardinalitySelection; i++)
    { latexTableStream << "&action of $"
      << this->theSimpleGens
      [this->parabolicSelectionSelectedAreElementsLevi.elements[i]+ this->GetOwner().GetRank()].ElementToString()
      << "$";
    }
    latexTableStream << "\\\\\n";
    int monCounter=0;
    for (int i=0; i<this->theGeneratingWordsGrouppedByWeight.size; i++)
      for (int j=0; j<this->theGeneratingWordsGrouppedByWeight[i].size; j++)
      { monCounter++;
        latexTableStream << "$m_{" << monCounter << "}$&";
        std::string stringWeightTemp=theWeyl.GetEpsilonCoords
        (this->theModuleWeightsSimpleCoords[i], theGlobalVariables).ElementToStringEpsilonForm(true, false);
        latexTableStream << "\n$" << tempV.ElementToStringLetterFormat("\\omega", false, false);
        if (stringWeightTemp!="0")
        { if (stringWeightTemp[0]!='-')
            latexTableStream << "+";
          latexTableStream << stringWeightTemp;
        }
        std::string theMonString=this->theGeneratingWordsGrouppedByWeight[i][j].ElementToString
        (false, false, theGlobalVariables, tempFormat);
        if (theMonString=="1")
          theMonString="";
        latexTableStream << "$&$" << theMonString << "  v_\\lambda$";
        for (int s=0; s< this->parabolicSelectionSelectedAreElementsLevi.CardinalitySelection; s++)
        { Matrix<CoefficientType>& theMat=this->actionsSimpleGensMatrixForM
          [this->parabolicSelectionSelectedAreElementsLevi.elements[s] +this->GetOwner().GetRank()];
          bool foundMon=false;
          latexTableStream << "&$";
          for (int l=0; l< this->GetDim(); l++)
            if (!theMat.elements[l] [monCounter-1].IsEqualToZero())
            { std::string tempS1;
              tempS1= theMat.elements[l][monCounter-1].ElementToString();
              if (tempS1=="1")
                tempS1="";
              if (tempS1=="-1")
                tempS1="-";
              std::stringstream tempStream;
              tempStream << "m_{" << l+1 << "}";
              tempS1+=tempStream.str();
              if (foundMon && tempS1[0]!='-')
                latexTableStream << "+";
              latexTableStream << tempS1;
              foundMon=true;
            }
          if (!foundMon)
            latexTableStream << "0";
          latexTableStream << "$";
        }
        latexTableStream << "\\\\\n";
      }
    latexTableStream << "\\end{tabular}<hr>";
    monomialDetailStream << (latexTableStream.str());
  }
  if (outputReport!=0)
    *outputReport= out2.str()+monomialDetailStream.str();
  this->flagIsInitialized=true;
//  std::cout << "<hr>MakeHW result: <br>" << this->ElementToString();

  return true;
}

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::ModOutVermaRelations
  (GlobalVariables* theContext, const List<CoefficientType>* subHiGoesToIthElement,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{ MonomialUniversalEnvelopingOrdered<CoefficientType> tempMon;
  ElementUniversalEnvelopingOrdered<CoefficientType> output;
  output.MakeZero(*this->owner);
  for (int i=0; i<this->size; i++)
  { tempMon= this->TheObjects[i];
//    tempMon.ComputeDebugString();
    tempMon.ModOutVermaRelations(theContext, subHiGoesToIthElement, theRingUnit, theRingZero);
//    tempMon.ComputeDebugString();
    output.AddMonomial(tempMon);
  }
  this->operator=(output);
}

template <class CoefficientType>
void MonomialUniversalEnvelopingOrdered<CoefficientType>::ModOutVermaRelations
(GlobalVariables* theContext, const List<CoefficientType>* subHiGoesToIthElement,
 const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{ int numPosRoots=this->owner->theOwner.GetNumPosRoots();
  int theDimension=this->owner->theOwner.GetRank();
  for (int i=this->generatorsIndices.size-1; i>=0; i--)
  { int IndexCurrentGenerator=this->generatorsIndices.TheObjects[i];
    if (IndexCurrentGenerator>=numPosRoots+theDimension)
    { this->MakeZero(theRingZero, *this->owner);
      return;
    }
    if (IndexCurrentGenerator<numPosRoots)
      return;
    if (IndexCurrentGenerator>=numPosRoots &&  IndexCurrentGenerator<numPosRoots+theDimension)
    { int theDegree;
      if (!this->Powers.TheObjects[i].IsSmallInteger(theDegree))
        return;
      if (subHiGoesToIthElement==0)
      { this->MakeZero(theRingZero, *this->owner);
        return;
      }
      CoefficientType theSubbedH=theRingZero;
      Vector<Rational>& currentH=this->owner->theOrder.TheObjects[IndexCurrentGenerator].Hcomponent;
      for (int i=0; i<currentH.size; i++)
        theSubbedH+=(*subHiGoesToIthElement)[i]*currentH[i];
      MathRoutines::RaiseToPower(theSubbedH, theDegree, theRingUnit);
//      this->owner->theOrder.TheObjects[IndexCurrentGenerator].Hcomponent.ComputeDebugString();
      //assert(this->Coefficient.checkConsistency());
      this->Coefficient.operator*=(theSubbedH);
      //assert(this->Coefficient.checkConsistency());
      this->generatorsIndices.size--;
      this->Powers.size--;
    }
  }
}

template <class CoefficientType>
bool MonomialUniversalEnvelopingOrdered<CoefficientType>::ModOutFDRelationsExperimental
(GlobalVariables* theContext, const Vector<Rational>& theHWsimpleCoords, const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{ WeylGroup& theWeyl=this->owner->theOwner.theWeyl;
  Vector<Rational> theHWsimpleCoordsTrue=theHWsimpleCoords;
  theWeyl.RaiseToDominantWeight(theHWsimpleCoordsTrue);
  Vector<Rational> theHWdualCoords=theWeyl.GetDualCoordinatesFromFundamental
  (theWeyl.GetFundamentalCoordinatesFromSimple(theHWsimpleCoordsTrue));
  List<CoefficientType> theSub;
  theSub.SetSize(theHWdualCoords.size);
  for (int i=0; i<theHWdualCoords.size; i++)
    theSub[i]=theHWdualCoords[i];
  this->ModOutVermaRelations(theContext, &theSub, theRingUnit, theRingZero);
  int numPosRoots=this->owner->theOwner.GetNumPosRoots();
  //int theDimension=this->owner->theOwner.GetRank();
  Vector<Rational> currentWeight=theHWsimpleCoordsTrue;
  Vector<Rational> testWeight;
  for (int k=this->generatorsIndices.size-1; k>=0; k--)
  { int IndexCurrentGenerator=this->generatorsIndices[k];
    if (IndexCurrentGenerator>=numPosRoots)
      return false;
    ElementSemisimpleLieAlgebra& currentElt=this->owner->theOrder[IndexCurrentGenerator];
    if (!currentElt.GetCartanPart().IsEqualToZero() || currentElt.size>1)
      return false;
    int thePower=0;
    if (!this->Powers[k].IsSmallInteger(thePower))
      return false;
    int rootIndex= this->owner->theOwner.ChevalleyGeneratorIndexToRootIndex(currentElt[0].theGeneratorIndex);
    Vector<Rational>& currentRoot=theWeyl.RootSystem[rootIndex];
//    std::cout << "<hr>The power: " << thePower;
    for (int j=0; j<thePower; j++)
    { currentWeight+=currentRoot;
//      std::cout << "<br>current weight is: " << currentWeight.ElementToString();
      testWeight=currentWeight;
      theWeyl.RaiseToDominantWeight(testWeight);
//      std::cout << "; raised to highest: " << testWeight.ElementToString();
//      std::cout << "<br>theHWsimpleCoordsTrue-currentWeight raised to highest = "
//      << (theHWsimpleCoordsTrue-testWeight).ElementToString();
      if (!(theHWsimpleCoordsTrue-testWeight).IsPositiveOrZero())
      { this->MakeZero(theRingZero, *this->owner);
        return true;
      }
    }
  }
  return true;
}

template <class CoefficientType>
bool ElementUniversalEnvelopingOrdered<CoefficientType>::ModOutFDRelationsExperimental
(GlobalVariables* theContext, const Vector<Rational>& theHWsimpleCoords, const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{ MonomialUniversalEnvelopingOrdered<CoefficientType> tempMon;
  ElementUniversalEnvelopingOrdered<CoefficientType> output;
  output.MakeZero(*this->owner);
  bool result=true;
  for (int i=0; i<this->size; i++)
  { tempMon= this->TheObjects[i];
//    tempMon.ComputeDebugString();
    if (!tempMon.ModOutFDRelationsExperimental(theContext, theHWsimpleCoords, theRingUnit, theRingZero))
      result=false;
//    tempMon.ComputeDebugString();
    output.AddMonomial(tempMon);
  }
  this->operator=(output);
  return result;
}

template <class CoefficientType>
bool ElementUniversalEnveloping<CoefficientType>::ModOutFDRelationsExperimental
(GlobalVariables* theContext, const Vector<Rational>& theHWsimpleCoords, const CoefficientType& theRingUnit,
 const CoefficientType& theRingZero)
{ MonomialUniversalEnveloping<CoefficientType> tempMon;
  ElementUniversalEnveloping<CoefficientType> output;
  output.MakeZero(*this->owner);
  bool result=false;
  for (int i=0; i<this->size; i++)
  { tempMon= this->TheObjects[i];
//    tempMon.ComputeDebugString();
    if (tempMon.ModOutFDRelationsExperimental(theContext, theHWsimpleCoords, theRingUnit, theRingZero))
      result=true;
//    tempMon.ComputeDebugString();
    output.AddMonomial(tempMon);
  }
  this->operator=(output);
  return result;
}

template <class CoefficientType>
bool ElementUniversalEnveloping<CoefficientType>::GetCoordsInBasis
  (List<ElementUniversalEnveloping<CoefficientType> >& theBasis, Vector<CoefficientType>& output,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero, GlobalVariables& theGlobalVariables)const
{ List<ElementUniversalEnveloping<CoefficientType> > tempBasis, tempElts;
  tempBasis=theBasis;
  tempBasis.AddOnTop(*this);
  Vectors<CoefficientType> tempCoords;
  if (!this->GetBasisFromSpanOfElements(tempBasis, tempCoords, tempElts, theRingUnit, theRingZero, theGlobalVariables))
    return false;
  Vector<CoefficientType> tempRoot;
  tempRoot=*tempCoords.LastObject();
  tempCoords.SetSize(theBasis.size);
  return tempRoot.GetCoordsInBasiS(tempCoords, output, theRingUnit, theRingZero);
}

template<class CoefficientType>
template<class CoefficientTypeQuotientField>
bool ElementUniversalEnveloping<CoefficientType>::GetBasisFromSpanOfElements
  (List<ElementUniversalEnveloping<CoefficientType> >& theElements, Vectors<CoefficientTypeQuotientField>& outputCoords,
   List<ElementUniversalEnveloping<CoefficientType> >& outputTheBasis, const CoefficientTypeQuotientField& theFieldUnit,
   const CoefficientTypeQuotientField& theFieldZero, GlobalVariables& theGlobalVariables)
{ if (theElements.size==0)
    return false;
  ElementUniversalEnveloping<CoefficientType> outputCorrespondingMonomials;
  outputCorrespondingMonomials.MakeZero(*theElements.TheObjects[0].owner);
  Vectors<CoefficientTypeQuotientField> outputCoordsBeforeReduction;
  for (int i=0; i<theElements.size; i++)
    for (int j=0; j<theElements.TheObjects[i].size; j++)
      outputCorrespondingMonomials.AddNoRepetition(theElements.TheObjects[i].TheObjects[j]);
  outputCoordsBeforeReduction.SetSize(theElements.size);
  for (int i=0; i<theElements.size; i++)
  { Vector<CoefficientTypeQuotientField>& currentList=outputCoordsBeforeReduction.TheObjects[i];
    currentList.MakeZero(outputCorrespondingMonomials.size, theFieldZero);
    ElementUniversalEnveloping<CoefficientType>& currentElt=theElements.TheObjects[i];
    for (int j=0; j<currentElt.size; j++)
    { MonomialUniversalEnveloping<CoefficientType>& currentMon=currentElt.TheObjects[j];
      currentList.TheObjects[outputCorrespondingMonomials.GetIndex(currentMon)]=currentMon.Coefficient;
    }
  }
  outputTheBasis.size=0;
  outputTheBasis.Reserve(theElements.size);
  Vectors<CoefficientTypeQuotientField> basisCoordForm;
  basisCoordForm.Reserve(theElements.size);
  Selection selectedBasis;
//  outputCoordsBeforeReduction.ComputeDebugString();
  outputCoordsBeforeReduction.SelectABasis(basisCoordForm, theFieldZero, selectedBasis, theGlobalVariables);
  for (int i=0; i<selectedBasis.CardinalitySelection; i++)
    outputTheBasis.AddOnTop(theElements.TheObjects[selectedBasis.elements[i]]);
  Matrix<CoefficientType> bufferMat;
  Vectors<CoefficientType> bufferVectors;
  outputCoordsBeforeReduction.GetCoordsInBasis
  (basisCoordForm, outputCoords, bufferVectors, bufferMat, theFieldUnit, theFieldZero);
  return true;
}

template <class CoefficientType>
bool MonomialUniversalEnveloping<CoefficientType>::ModOutFDRelationsExperimental
  (GlobalVariables* theContext, const Vector<Rational>& theHWsimpleCoords, const CoefficientType& theRingUnit,
   const CoefficientType& theRingZero)
{ WeylGroup& theWeyl=this->owner->theWeyl;
  Vector<Rational> theHWsimpleCoordsTrue=theHWsimpleCoords;
  theWeyl.RaiseToDominantWeight(theHWsimpleCoordsTrue);
  Vector<Rational> theHWdualCoords=theWeyl.GetDualCoordinatesFromFundamental
  (theWeyl.GetFundamentalCoordinatesFromSimple(theHWsimpleCoordsTrue));
  List<CoefficientType> theSub;
  theSub.SetSize(theHWdualCoords.size);
  for (int i=0; i<theHWdualCoords.size; i++)
    theSub[i]=theHWdualCoords[i];
  this->ModOutVermaRelations(theContext, &theSub, theRingUnit, theRingZero);
  int numPosRoots=this->GetOwner().GetNumPosRoots();
  //int theDimension=this->owner->theOwner.GetRank();
  Vector<Rational> currentWeight=theHWsimpleCoordsTrue;
  Vector<Rational> testWeight;
  ElementSemisimpleLieAlgebra currentElt;
  for (int k=this->generatorsIndices.size-1; k>=0; k--)
  { int IndexCurrentGenerator=this->generatorsIndices[k];
    if (IndexCurrentGenerator>=numPosRoots)
      return false;
    currentElt.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE
    (IndexCurrentGenerator, *this->owner);
    if (!currentElt.GetCartanPart().IsEqualToZero() || currentElt.size>1)
      return false;
    int thePower=0;
    if (!this->Powers[k].IsSmallInteger(thePower))
      return false;
    int rootIndex= this->GetOwner().ChevalleyGeneratorIndexToRootIndex(currentElt[0].theGeneratorIndex);
    Vector<Rational>& currentRoot=theWeyl.RootSystem[rootIndex];
//    std::cout << "<hr>The power: " << thePower;
    for (int j=0; j<thePower; j++)
    { currentWeight+=currentRoot;
//      std::cout << "<br>current weight is: " << currentWeight.ElementToString();
      testWeight=currentWeight;
      theWeyl.RaiseToDominantWeight(testWeight);
//      std::cout << "; raised to highest: " << testWeight.ElementToString();
//      std::cout << "<br>theHWsimpleCoordsTrue-currentWeight raised to highest = "
//      << (theHWsimpleCoordsTrue-testWeight).ElementToString();
      if (!(theHWsimpleCoordsTrue-testWeight).IsPositiveOrZero())
      { this->MakeZero(theRingZero, *this->owner);
        return true;
      }
    }
  }
  return false;
}

template <class CoefficientType>
void MonomialUniversalEnveloping<CoefficientType>::ModOutVermaRelations
  (CoefficientType& outputCoeff, GlobalVariables* theContext, const List<CoefficientType>* subHiGoesToIthElement, const CoefficientType& theRingUnit,
   const CoefficientType& theRingZero)
{ int numPosRoots=this->GetOwner().GetNumPosRoots();
  int theDimension=this->GetOwner().GetRank();
  ElementSemisimpleLieAlgebra currentElt;
  CoefficientType tempCF;
  outputCoeff=theRingUnit;
  for (int i=this->generatorsIndices.size-1; i>=0; i--)
  { int IndexCurrentGenerator=this->generatorsIndices.TheObjects[i];
    if (IndexCurrentGenerator>=numPosRoots+theDimension)
    { this->MakeZero(theRingZero, *this->owners, this->indexInOwners);
      return;
    }
    if (IndexCurrentGenerator<numPosRoots)
      return;
    if (IndexCurrentGenerator>=numPosRoots &&  IndexCurrentGenerator<numPosRoots+theDimension)
    { int theDegree;
      if (!this->Powers.TheObjects[i].IsSmallInteger(theDegree))
        return;
      if (subHiGoesToIthElement==0)
      { this->MakeZero(theRingZero, *this->owners, this->indexInOwners);
        return;
      }
      CoefficientType theSubbedH;
      theSubbedH=theRingZero;
      currentElt.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE
      (IndexCurrentGenerator, *this->owners, this->indexInOwners);
      Vector<Rational> currentH=currentElt.GetCartanPart();
      for (int i=0; i<currentH.size; i++)
      { tempCF=(*subHiGoesToIthElement)[i];
        tempCF*=currentH[i];
        theSubbedH+=tempCF;
      }
      MathRoutines::RaiseToPower(theSubbedH, theDegree, theRingUnit);
//      this->owner->theOrder.TheObjects[IndexCurrentGenerator].Hcomponent.ComputeDebugString();
      //assert(this->Coefficient.checkConsistency());
      outputCoeff*=(theSubbedH);
      //assert(this->Coefficient.checkConsistency());
      this->generatorsIndices.size--;
      this->Powers.size--;
    }
  }
}

template<class CoefficientType>
void ElementUniversalEnveloping<CoefficientType>::ModToMinDegreeFormFDRels
(const Vector<Rational>& theHWinSimpleCoords, GlobalVariables& theGlobalVariables, const CoefficientType& theRingUnit,
 const CoefficientType& theRingZero)
{ ElementUniversalEnveloping<CoefficientType> result, temp;
  result.MakeZero(*this->owner);
  bool Found=true;
  int numPosRoots=this->owner->GetNumPosRoots();
  while (Found)
  { Found=false;
    for (int j=numPosRoots-1; j>=0; j--)
    { this->owner->UEGeneratorOrderIncludingCartanElts.SwapTwoIndices(j,numPosRoots-1);
      this->Simplify(theGlobalVariables, theRingUnit, theRingZero);
      this->owner->UEGeneratorOrderIncludingCartanElts.SwapTwoIndices(j,numPosRoots-1);
      if (this->ModOutFDRelationsExperimental(&theGlobalVariables, theHWinSimpleCoords, theRingUnit, theRingZero))
        Found=true;
    }
  }
  this->Simplify(theGlobalVariables, theRingUnit, theRingZero);
}

template<class CoefficientType>
bool ElementUniversalEnveloping<CoefficientType>::ApplyMinusTransposeAutoOnMe()
{ MonomialUniversalEnveloping<CoefficientType> tempMon;
  ElementUniversalEnveloping<CoefficientType> result;
  result.MakeZero(*this->owners, this->indexInOwners);
  int numPosRoots=this->GetOwner().GetNumPosRoots();
  int theRank=this->GetOwner().GetRank();
  CoefficientType theCoeff;
  for (int i=0; i<this->size; i++)
  { MonomialUniversalEnveloping<CoefficientType>& currentMon=this->TheObjects[i];
    theCoeff=this->theCoeffs[i];
    tempMon.owners=currentMon.owners;
    tempMon.indexInOwners=currentMon.indexInOwners;
    tempMon.Powers.size=0;
    tempMon.generatorsIndices.size=0;
    for (int j=0; j<currentMon.Powers.size; j++)
 //   for (int j=currentMon.Powers.size-1; j>=0; j--)
    { int thePower;
      if (!currentMon.Powers[j].IsSmallInteger(thePower))
        return false;
      int theGenerator=currentMon.generatorsIndices[j];
      if (theGenerator<numPosRoots)
        theGenerator=2*numPosRoots+theRank-1-theGenerator;
      else if (theGenerator>=numPosRoots +theRank)
        theGenerator=-theGenerator+2*numPosRoots+theRank-1;
      tempMon.MultiplyByGeneratorPowerOnTheRight(theGenerator, currentMon.Powers[j]);
      if (thePower%2==1)
        theCoeff*=-1;
//      ;
    }
    result.AddMonomial(tempMon, theCoeff);
  }
  *this=result;
  return true;
}

template<class CoefficientType>
bool ElementUniversalEnveloping<CoefficientType>::ApplyTransposeAntiAutoOnMe()
{ MonomialUniversalEnveloping<CoefficientType> tempMon;
  ElementUniversalEnveloping<CoefficientType> result;
  result.MakeZero(*this->owners, this->indexInOwners);
  int numPosRoots=this->GetOwner().GetNumPosRoots();
  int theRank=this->GetOwner().GetRank();
  CoefficientType theCoeff;
  for (int i=0; i<this->size; i++)
  { MonomialUniversalEnveloping<CoefficientType>& currentMon=this->TheObjects[i];
    theCoeff=this->theCoeffs[i];
    tempMon.owners=currentMon.owners;
    tempMon.indexInOwners=currentMon.indexInOwners;
    tempMon.Powers.size=0;
    tempMon.generatorsIndices.size=0;
//    for (int j=0; j<currentMon.Powers.size; j++)
    for (int j=currentMon.Powers.size-1; j>=0; j--)
    { int thePower;
      if (!currentMon.Powers[j].IsSmallInteger(thePower))
        return false;
      int theGenerator=currentMon.generatorsIndices[j];
      if (theGenerator<numPosRoots)
        theGenerator=2*numPosRoots+theRank-1-theGenerator;
      else if (theGenerator>=numPosRoots +theRank)
        theGenerator=-theGenerator+2*numPosRoots+theRank-1;
      tempMon.MultiplyByGeneratorPowerOnTheRight(theGenerator, currentMon.Powers[j]);
//      if (thePower%2==1)
//        tempMon.Coefficient*=-1;
//      ;
    }
    result.AddMonomial(tempMon, theCoeff);
  }
  *this=result;
  return true;
}

int ParserNode::EvaluateMinusTransposeAuto
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ ElementUniversalEnveloping<Polynomial<Rational> >& theElt= theNode.UEElement.GetElement();
  theElt=theNode.owner->TheObjects[theArgumentList[0]].UEElement.GetElement();
  if (!theElt.ApplyMinusTransposeAutoOnMe())
    return theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);
  theNode.ExpressionType=theNode.typeUEelement;
  return theNode.errorNoError;
}

int ParserNode::EvaluateTransposeAntiAuto
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ ElementUniversalEnveloping<Polynomial<Rational> >& theElt= theNode.UEElement.GetElement();
  theElt=theNode.owner->TheObjects[theArgumentList[0]].UEElement.GetElement();
  if (!theElt.ApplyTransposeAntiAutoOnMe())
    return theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);
  theNode.ExpressionType=theNode.typeUEelement;
  return theNode.errorNoError;
}

int ParserNode::EvaluateHWMTABilinearForm
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ Vector<Rational> weight;
  ParserNode& leftNode=theNode.owner->TheObjects[theArgumentList[0]];
  ParserNode& rightNode=theNode.owner->TheObjects[theArgumentList[1]];
  ParserNode& weightNode=theNode.owner->TheObjects[theArgumentList[2]];
  if (!weightNode.GetRootRationalDontUseForFunctionArguments(weight, theGlobalVariables))
    return theNode.SetError(theNode.errorBadOrNoArgument);
  SemisimpleLieAlgebra& theSSalgebra=theNode.owner->theHmm.theRange();
  if (weight.size!=theSSalgebra.GetRank())
    return theNode.SetError(theNode.errorDimensionProblem);
  ElementUniversalEnveloping<Polynomial<Rational> >& leftElt=leftNode.UEElement.GetElement();
  ElementUniversalEnveloping<Polynomial<Rational> >& rightElt=rightNode.UEElement.GetElement();
  Polynomial<Rational> theRingZero, theRingUnit;
  theNode.impliedNumVars=MathRoutines::Maximum(leftNode.impliedNumVars, rightNode.impliedNumVars);
  int& numVars= theNode.impliedNumVars;
  theRingZero.MakeZero(numVars);
  theRingUnit.MakeOne(numVars);
  List<Polynomial<Rational> > theHW;
  WeylGroup& theWeyl=theSSalgebra.theWeyl;
  weight=theWeyl.GetDualCoordinatesFromFundamental(weight);
  std::stringstream out;
  out << "Highest weight in dual coords: " << weight.ElementToString() << "<br>";
  theHW.SetSize(weight.size);
  for (int i=0; i<weight.size; i++)
    theHW[i].MakeConst(numVars, weight[i]);
  leftElt.GetOwner().OrderSSalgebraForHWbfComputation();
  if(!leftElt.HWMTAbilinearForm
     (rightElt, theNode.polyValue.GetElement(), &theHW, theGlobalVariables, theRingUnit, theRingZero, &out))
    return theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);
  Polynomial<Rational>  symmTerm;
  if(!rightElt.HWMTAbilinearForm
     (leftElt, symmTerm, &theHW, theGlobalVariables, theRingUnit, theRingZero, &out))
    return theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);
  leftElt.GetOwner().OrderSSLieAlgebraStandard();
  theNode.polyValue.GetElement()+=symmTerm;
  theNode.ExpressionType=theNode.typePoly;
  theNode.outputString=out.str();
  return theNode.errorNoError;
}

int ParserNode::EvaluateHWTAABilinearForm
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ List<Polynomial<Rational> > weight;
  ParserNode& leftNode=theNode.owner->TheObjects[theArgumentList[0]];
  ParserNode& rightNode=theNode.owner->TheObjects[theArgumentList[1]];
  ParserNode& weightNode=theNode.owner->TheObjects[theArgumentList[2]];
  theNode.impliedNumVars=theNode.owner->MaxFoundVars;
  if (!weightNode.GetListDontUseForFunctionArguments<Polynomial<Rational> >
      (weight, theGlobalVariables, theNode.typePoly, theNode.impliedNumVars))
    return theNode.SetError(theNode.errorBadOrNoArgument);
  SemisimpleLieAlgebra& theSSalgebra=theNode.owner->theHmm.theRange();
  if (weight.size!=theSSalgebra.GetRank())
    return theNode.SetError(theNode.errorDimensionProblem);
  ElementUniversalEnveloping<Polynomial<Rational> >& leftElt=leftNode.UEElement.GetElement();
  ElementUniversalEnveloping<Polynomial<Rational> >& rightElt=rightNode.UEElement.GetElement();
  leftElt.SetNumVariables(theNode.impliedNumVars);
  rightElt.SetNumVariables(theNode.impliedNumVars);
  Polynomial<Rational>  theRingZero, theRingUnit;
  int& numVars= theNode.impliedNumVars;
  theRingZero.MakeZero(numVars);
  theRingUnit.MakeOne(numVars);
  List<Polynomial<Rational> > theHW;
  WeylGroup& theWeyl=theSSalgebra.theWeyl;
  std::stringstream out;
  out << "Highest weight in fundamental coords: " << weight.ElementToString() << "<br>";
  theHW.SetSize(weight.size);
  leftElt.GetOwner().OrderSSalgebraForHWbfComputation();
  for (int i=0; i<weight.size; i++)
  { theHW[i]=weight[i];
    theHW[i]*=theWeyl.CartanSymmetric.elements[i][i]/2;
  }
  if(!leftElt.HWTAAbilinearForm(rightElt, theNode.polyValue.GetElement(), &theHW, theGlobalVariables, theRingUnit, theRingZero, &out))
    return theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);
//  Polynomial<Rational>  symmTerm;
//  if(!rightElt.HWTAAbilinearForm
//     (leftElt, symmTerm, &theHW, theGlobalVariables, theRingUnit, theRingZero, &out))
//  return theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);
  leftElt.GetOwner().OrderSSLieAlgebraStandard();
//  theNode.polyValue.GetElement()+=symmTerm;
  theNode.ExpressionType=theNode.typePoly;
  theNode.outputString=out.str();
  return theNode.errorNoError;
}

template <class CoefficientType>
bool ElementUniversalEnveloping<CoefficientType>::HWMTAbilinearForm
  (const ElementUniversalEnveloping<CoefficientType>&right, CoefficientType& output,
   const List<CoefficientType>* subHiGoesToIthElement, GlobalVariables& theGlobalVariables,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero, std::stringstream* logStream)
{ output=theRingZero;
  CoefficientType tempCF;
  ElementUniversalEnveloping<CoefficientType> MTright;
  MTright=right;
  if (!MTright.ApplyMinusTransposeAutoOnMe())
    return false;
  ElementUniversalEnveloping<CoefficientType> Accum, intermediateAccum, tempElt;
  Accum.MakeZero(*this->owners, this->indexInOwners);
  MonomialUniversalEnveloping<CoefficientType> constMon;
  constMon.MakeConst(*this->owners, this->indexInOwners);
  PolynomialOutputFormat tempFormat;
  tempFormat.MakeAlphabetArbitraryWithIndex("g", "h");
  if (logStream!=0)
  { *logStream << "backtraced elt: " << MTright.ElementToString(theGlobalVariables, tempFormat) << "<br>";
    *logStream << "this element: " << this->ElementToString(theGlobalVariables, tempFormat) << "<br>";
   // for (int i=0; i<subHiGoesToIthElement->size; i++)
   // { *logStream << subHiGoesToIthElement->TheObjects[i].ElementToString();
   // }
  }
  for (int j=0; j<right.size; j++)
  { intermediateAccum=*this;
    intermediateAccum.Simplify(theGlobalVariables, theRingUnit, theRingZero);
    if (logStream!=0)
      *logStream << "intermediate after simplification: " << intermediateAccum.ElementToString(theGlobalVariables, tempFormat)
      << "<br>";
    intermediateAccum.ModOutVermaRelations(&theGlobalVariables, subHiGoesToIthElement, theRingUnit, theRingZero);
    MonomialUniversalEnveloping<CoefficientType>& rightMon=MTright[j];
    CoefficientType& rightMonCoeff=MTright.theCoeffs[j];
    int thePower;
    for (int i=rightMon.Powers.size-1; i>=0; i--)
      if (rightMon.Powers[i].IsSmallInteger(thePower))
        for (int k=0; k<thePower; k++)
        { tempElt.MakeOneGenerator(rightMon.generatorsIndices[i], *this->owners, this->indexInOwners, theRingUnit);
          MathRoutines::swap(tempElt, intermediateAccum);
          if (logStream!=0)
          { *logStream << "tempElt before mult: " << tempElt.ElementToString(theGlobalVariables, tempFormat) << "<br>";
            *logStream << "intermediate before mult: " << intermediateAccum.ElementToString(theGlobalVariables, tempFormat) << "<br>";
          }
          intermediateAccum.MultiplyBy(tempElt);
          if (logStream!=0)
            *logStream << "intermediate before simplification: " << intermediateAccum.ElementToString(theGlobalVariables, tempFormat) << "<br>";
          intermediateAccum.Simplify(theGlobalVariables, theRingUnit, theRingZero);
          if (logStream!=0)
            *logStream << "intermediate after simplification: " << intermediateAccum.ElementToString(theGlobalVariables, tempFormat) << "<br>";
          intermediateAccum.ModOutVermaRelations(&theGlobalVariables, subHiGoesToIthElement, theRingUnit, theRingZero);
          if (logStream!=0)
            *logStream << "intermediate after Verma rels: " << intermediateAccum.ElementToString(theGlobalVariables, tempFormat) << "<br>";
        }
      else
        return false;
    intermediateAccum*=rightMonCoeff;
    Accum+=intermediateAccum;
    int theIndex= intermediateAccum.GetIndex(constMon);
    if (theIndex!=-1)
      output+=intermediateAccum.theCoeffs[theIndex];
  }
  if (logStream!=0)
    *logStream << "final UE element: " << Accum.ElementToString(false, theGlobalVariables, tempFormat);
  return true;
}

template <class CoefficientType>
bool MonomialUniversalEnveloping<CoefficientType>::HWTAAbilinearForm
(const MonomialUniversalEnveloping<CoefficientType>&right, CoefficientType& output,
 const List<CoefficientType>* subHiGoesToIthElement, GlobalVariables& theGlobalVariables,
 const CoefficientType& theRingUnit, const CoefficientType& theRingZero, std::stringstream* logStream)
{ ElementUniversalEnveloping<CoefficientType> tempElt1, tempElt2;
  tempElt1.MakeZero(*this->owners, this->indexInOwners);
  tempElt1.AddMonomial(*this, theRingUnit);
  tempElt2.MakeZero(*this->owners, this->indexInOwners);
  tempElt2.AddMonomial(*this, theRingUnit);
  return tempElt1.HWTAAbilinearForm
  (tempElt2, output, subHiGoesToIthElement, theGlobalVariables, theRingUnit, theRingZero, logStream);
}

template <class CoefficientType>
bool ElementUniversalEnveloping<CoefficientType>::HWTAAbilinearForm
  (const ElementUniversalEnveloping<CoefficientType>&right, CoefficientType& output,
   const List<CoefficientType>* subHiGoesToIthElement, GlobalVariables& theGlobalVariables,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero, std::stringstream* logStream)
{ output=theRingZero;
  CoefficientType tempCF;
  ElementUniversalEnveloping<CoefficientType> TAleft;
  TAleft=*this;
  if (!TAleft.ApplyTransposeAntiAutoOnMe())
    return false;
  ElementUniversalEnveloping<CoefficientType> Accum, intermediateAccum, tempElt, startingElt;
  List<int> oldOrder=this->GetOwner().UEGeneratorOrderIncludingCartanElts;
  int numPosRoots=this->GetOwner().GetNumPosRoots();
  for (int i=0; i<numPosRoots; i++)
    this->GetOwner().UEGeneratorOrderIncludingCartanElts[i]=-1;
  Accum.MakeZero(*this->owners, this->indexInOwners);
  MonomialUniversalEnveloping<CoefficientType> constMon;
  constMon.MakeConst(*this->owners, this->indexInOwners);
  PolynomialOutputFormat tempFormat;
  tempFormat.MakeAlphabetArbitraryWithIndex("g", "h");
//    std::cout << "<hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr>"
//  << "hw: " << subHiGoesToIthElement->ElementToString()
//  << "<hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr>"
 // << "(" << this->ElementToStringCalculatorFormat(theGlobalVariables, tempFormat)
 // << "," << right.ElementToStringCalculatorFormat(theGlobalVariables, tempFormat) << ")";

  if (logStream!=0)
  { *logStream << "left eltement transposed: " << TAleft.ElementToString(theGlobalVariables, tempFormat) << "<br>";
    *logStream << "right element: " << right.ElementToString(theGlobalVariables, tempFormat) << "<br>";
  }
  startingElt=right;
  startingElt.Simplify(theGlobalVariables, theRingUnit, theRingZero);
  if (logStream!=0)
    *logStream << "right element after simplification: "
    << startingElt.ElementToString(theGlobalVariables, tempFormat) << "<br>";
  startingElt.ModOutVermaRelations(&theGlobalVariables, subHiGoesToIthElement, theRingUnit, theRingZero);
  if (logStream!=0)
    *logStream << "right element after Verma rels: "
    << startingElt.ElementToString(theGlobalVariables, tempFormat) << "<br>";
  CoefficientType leftMonCoeff;
  for (int j=0; j<TAleft.size; j++)
  { intermediateAccum=startingElt;
    MonomialUniversalEnveloping<CoefficientType>& leftMon=TAleft[j];
    leftMonCoeff=TAleft.theCoeffs[j];
    int thePower;
    for (int i=leftMon.Powers.size-1; i>=0; i--)
      if (leftMon.Powers[i].IsSmallInteger(thePower))
        for (int k=0; k<thePower; k++)
        { tempElt.MakeOneGenerator(leftMon.generatorsIndices[i], *this->owners, this->indexInOwners, theRingUnit);
          MathRoutines::swap(tempElt, intermediateAccum);
          if (logStream!=0)
          { //*logStream << "tempElt before mult: " << tempElt.ElementToString(theGlobalVariables, tempFormat) << "<br>";
            *logStream << "intermediate before mult: " << intermediateAccum.ElementToString(theGlobalVariables, tempFormat) << "<br>";
          }
          intermediateAccum.MultiplyBy(tempElt);
          if (logStream!=0)
            *logStream << "intermediate before simplification: " << intermediateAccum.ElementToString(theGlobalVariables, tempFormat) << "<br>";
          intermediateAccum.Simplify(theGlobalVariables, theRingUnit, theRingZero);
          if (logStream!=0)
            *logStream << "intermediate after simplification: " << intermediateAccum.ElementToString(theGlobalVariables, tempFormat) << "<br>";
          intermediateAccum.ModOutVermaRelations(&theGlobalVariables, subHiGoesToIthElement, theRingUnit, theRingZero);
          if (logStream!=0)
            *logStream << "intermediate after Verma rels: " << intermediateAccum.ElementToString(theGlobalVariables, tempFormat) << "<br>";
        }
      else
      { this->GetOwner().UEGeneratorOrderIncludingCartanElts=oldOrder;
        return false;
      }
    intermediateAccum*=leftMonCoeff;
    Accum+=intermediateAccum;
    int theIndex= intermediateAccum.GetIndex(constMon);
    if (theIndex!=-1)
      output+=intermediateAccum.theCoeffs[theIndex];
  }
  if (logStream!=0)
    *logStream << "final UE element: " << Accum.ElementToString(false, theGlobalVariables, tempFormat);
  this->GetOwner().UEGeneratorOrderIncludingCartanElts=oldOrder;
  return true;
}

int ParserNode::EvaluateIsInProperSubmoduleVermaModule
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ Vector<Rational> weight;
  ParserNode& ueNode=theNode.owner->TheObjects[theArgumentList[0]];
  ParserNode& weightNode=theNode.owner->TheObjects[theArgumentList[1]];
  if (!weightNode.GetRootRationalDontUseForFunctionArguments(weight, theGlobalVariables))
    return theNode.SetError(theNode.errorBadOrNoArgument);
  SemisimpleLieAlgebra& theSSalgebra=theNode.owner->theHmm.theRange();
  if (weight.size!=theSSalgebra.GetRank())
    return theNode.SetError(theNode.errorDimensionProblem);
  ElementUniversalEnveloping<Polynomial<Rational> >& theUE=ueNode.UEElement.GetElement();
  Polynomial<Rational>  theRingZero, theRingUnit;
  theNode.impliedNumVars=ueNode.impliedNumVars;
  int& numVars= theNode.impliedNumVars;
  theRingZero.MakeZero(numVars);
  theRingUnit.MakeOne(numVars);
  List<Polynomial<Rational> > theHW;
  WeylGroup& theWeyl=theSSalgebra.theWeyl;
  weight=theWeyl.GetDualCoordinatesFromFundamental(weight);
  std::stringstream out;
  theGlobalVariables.MaxAllowedComputationTimeInSeconds=30;
  out << "Highest weight in dual coords: " << weight.ElementToString() << "<br>";
  theHW.SetSize(weight.size);
  for (int i=0; i<weight.size; i++)
    theHW[i].MakeConst(numVars, weight[i]);
  theUE.GetOwner().OrderSSalgebraForHWbfComputation();
  out << theUE.IsInProperSubmodule(&theHW, theGlobalVariables, theRingUnit, theRingZero);
  theUE.GetOwner().OrderSSLieAlgebraStandard();
  theNode.ExpressionType=theNode.typeString;
  theNode.outputString=out.str();
  return theNode.errorNoError;
}

template <class CoefficientType>
std::string ElementUniversalEnveloping<CoefficientType>::IsInProperSubmodule
(const List<CoefficientType>* subHiGoesToIthElement, GlobalVariables& theGlobalVariables, const CoefficientType& theRingUnit,
 const CoefficientType& theRingZero)
{ std::stringstream out;
  List<ElementUniversalEnveloping<CoefficientType> > theOrbit;
  theOrbit.Reserve(1000);
  ElementUniversalEnveloping<CoefficientType> theElt;
  int theDim=this->GetOwner().GetRank();
  int numPosRoots=this->GetOwner().GetNumPosRoots();
  theOrbit.AddOnTop(*this);
  for (int i=0; i<theOrbit.size; i++)
    for (int j=0; j<theDim; j++)
    { theElt.MakeOneGenerator(j+numPosRoots+theDim, *this->owners, this->indexInOwners, theRingUnit);
      theElt.MultiplyBy(theOrbit[i]);
      theElt.Simplify(theGlobalVariables, theRingUnit, theRingZero);
      theElt.ModOutVermaRelations(&theGlobalVariables, subHiGoesToIthElement, theRingUnit, theRingZero);
//      theElt.Simplify(theGlobalVariables, theRingUnit, theRingZero);
      if (!theElt.IsEqualToZero())
        theOrbit.AddOnTop(theElt);
    }
  PolynomialOutputFormat theFormat;
  theFormat.MakeAlphabetArbitraryWithIndex("g", "h");
  for (int i=0; i< theOrbit.size; i++)
  { ElementUniversalEnveloping<CoefficientType>& current=theOrbit[i];
    out << "<br>" << current.ElementToString(theGlobalVariables, theFormat);
  }
  return out.str();
}

int ParserNode::EvaluateSplitCharOverLeviParabolic
(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ Vector<Rational> parSel;
  ParserNode& charNode=theNode.owner->TheObjects[theArgumentList[0]];
  ParserNode& selNode=theNode.owner->TheObjects[theArgumentList[1]];
//  WeylGroup& theWeyl= theNode.owner->theHmm.theRange.theWeyl;
  int theDim=theNode.owner->theHmm.theRange().GetRank();
  if (! selNode.GetRootRationalDontUseForFunctionArguments(parSel, theGlobalVariables))
    return theNode.SetError(theNode.errorBadOrNoArgument);
  if (parSel.size!=theDim)
    return theNode.SetError(theNode.errorDimensionProblem);
  std::stringstream out;
  out << "Parabolic subalgebra selection: " << parSel.ElementToString() << ".";
  charSSAlgMod& theChar=charNode.theChar.GetElement();
  std::string report;
  charSSAlgMod tempChar;
  ReflectionSubgroupWeylGroup subWeyl;
  Vector<Rational> emptySel;
  emptySel.MakeZero(theDim);
  theChar.SplitCharOverLevi(&report,  tempChar, parSel, emptySel, subWeyl, theGlobalVariables);
  out << report;
  theNode.outputString=out.str();
  theNode.ExpressionType=theNode.typeString;
  return theNode.errorNoError;
}

int ParserNode::EvaluateSplitIrrepOverLeviParabolic
(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ Vector<Rational> theWeightFundCoords, parSel;
  ParserNode& weightNode=theNode.owner->TheObjects[theArgumentList[0]];
  ParserNode& selNode=theNode.owner->TheObjects[theArgumentList[1]];
  WeylGroup& theWeyl=theNode.GetContextLieAlgebra().theWeyl;
  int theDim=theNode.owner->theHmm.theRange().GetRank();
  if (! weightNode.GetRootRationalDontUseForFunctionArguments(theWeightFundCoords, theGlobalVariables))
    return theNode.SetError(theNode.errorBadOrNoArgument);
  if (! selNode.GetRootRationalDontUseForFunctionArguments(parSel, theGlobalVariables))
    return theNode.SetError(theNode.errorBadOrNoArgument);
  if (theWeightFundCoords.size!=theDim || parSel.size!=theDim)
    return theNode.SetError(theNode.errorDimensionProblem);
  std::stringstream out;
  out << "Your input weight in fundamental coordinates: "
  << theWeyl.GetFundamentalCoordinatesFromSimple(theWeyl.GetSimpleCoordinatesFromFundamental(theWeightFundCoords)  )
  .ElementToString();
  out << ". <br>Your input weight in simple coordinates: " << theWeyl.GetSimpleCoordinatesFromFundamental(theWeightFundCoords).ElementToString();
  out << ".<br>Your parabolic subalgebra selection: " << parSel.ElementToString() << ".";
  ModuleSSalgebraNew<Rational> theMod;

  Selection emptySel;
  emptySel.init(theDim);
  theMod.MakeFromHW
  (theNode.owner->theAlgebras, 0, theWeightFundCoords, emptySel, theGlobalVariables, 1, 0, 0);
  std::string report;
  theMod.SplitOverLevi(& report, parSel, theGlobalVariables, 1, 0);
  out << "<br>" << report;

  theNode.ExpressionType=theNode.typeString;
  theNode.outputString=out.str();
  return theNode.errorNoError;
}

int ParserNode::EvaluateSplitFDPartGenVermaOverLeviParabolic
(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ Vector<Rational> theWeightFundCoords, inducingParSel, splittingParSel;
  ParserNode& weightNode=theNode.owner->TheObjects[theArgumentList[0]];
  ParserNode& inducingParNode=theNode.owner->TheObjects[theArgumentList[1]];
  ParserNode& splittingParNode=theNode.owner->TheObjects[theArgumentList[2]];
  WeylGroup& theWeyl=theNode.GetContextLieAlgebra().theWeyl;
  int theDim=theNode.owner->theHmm.theRange().GetRank();
  if (! weightNode.GetRootRationalDontUseForFunctionArguments(theWeightFundCoords, theGlobalVariables))
    return theNode.SetError(theNode.errorBadOrNoArgument);
  if (! inducingParNode.GetRootRationalDontUseForFunctionArguments(inducingParSel, theGlobalVariables))
    return theNode.SetError(theNode.errorBadOrNoArgument);
  if (! splittingParNode.GetRootRationalDontUseForFunctionArguments(splittingParSel, theGlobalVariables))
    return theNode.SetError(theNode.errorBadOrNoArgument);
  if (theWeightFundCoords.size!=theDim || inducingParSel.size!=theDim || splittingParSel.size!=theDim)
    return theNode.SetError(theNode.errorDimensionProblem);
  std::stringstream out;
  out << "Your input weight in fundamental coordinates: "
  << theWeyl.GetFundamentalCoordinatesFromSimple(theWeyl.GetSimpleCoordinatesFromFundamental(theWeightFundCoords)  )
  .ElementToString();
  out << ". <br>Your input weight in simple coordinates: " << theWeyl.GetSimpleCoordinatesFromFundamental(theWeightFundCoords).ElementToString();
  out << ".<br>Your inducing parabolic subalgebra: " << inducingParSel.ElementToString() << ".";
  out << ".<br>The parabolic subalgebra I should split over: " << splittingParSel.ElementToString() << ".";
  ModuleSSalgebraNew<Rational> theMod;
  Selection selInducing= inducingParSel;
  theMod.MakeFromHW
  (theNode.owner->theAlgebras, 0, theWeightFundCoords, selInducing, theGlobalVariables, 1, 0, 0);
  std::string report;
  theMod.SplitOverLevi(& report, splittingParSel, theGlobalVariables, 1, 0);
  out << "<br>" << report;

  theNode.ExpressionType=theNode.typeString;
  theNode.outputString=out.str();
  return theNode.errorNoError;
}

template<class CoefficientType>
void ModuleSSalgebraNew<CoefficientType>::SplitOverLevi
  (std::string* Report, Vector<Rational>& splittingParSel, GlobalVariables& theGlobalVariables, const CoefficientType& theRingUnit,
   const CoefficientType& theRingZero)
{ if (this->theChaR.size!=1)
  { if (Report!=0)
      *Report="I have been instructed only to split modules that are irreducible over the ambient Lie algebra";
    return;
  }
  PolynomialOutputFormat theFormat;
  theFormat.MakeAlphabetArbitraryWithIndex("g", "h");
  ReflectionSubgroupWeylGroup subWeyl;
  charSSAlgMod charWRTsubalgebra;
  this->theChaR.SplitCharOverLevi
  (Report, charWRTsubalgebra, splittingParSel, this->parabolicSelectionNonSelectedAreElementsLevi,
   subWeyl, theGlobalVariables);
  Vector<Rational> theHWsimpleCoords, theHWfundCoords;
  std::stringstream out;
  if(Report!=0)
    out << *Report;
  Selection splittingParSelectedInLevi;
  splittingParSelectedInLevi=splittingParSel;
  splittingParSelectedInLevi.InvertSelection();
  if (!splittingParSelectedInLevi.IsSubset(this->parabolicSelectionSelectedAreElementsLevi))
  { out << "The parabolic subalgebra you selected is not a subalgebra of the ambient parabolic subalgebra."
    << " The parabolic has Vectors<Rational> of Levi given by " << splittingParSel.ElementToString()
    <<" while the ambient parabolic subalgebra has Vectors<Rational> of Levi given by "
    << this->parabolicSelectionNonSelectedAreElementsLevi.ElementToString();
    if (Report!=0)
      *Report=out.str();
    return;
  }
  out << "<br>Parabolic selection: " << splittingParSel.ElementToString();
  List<List<List<Rational> > > eigenSpacesPerSimpleGenerator;
 // if (false)
  eigenSpacesPerSimpleGenerator.SetSize(splittingParSelectedInLevi.CardinalitySelection);
  Vectors<Rational> theFinalEigenSpace, tempSpace1, tempSpace2;
//  WeylGroup& theWeyL=this->theAlgebra.theWeyl;
  for (int i=0; i<splittingParSelectedInLevi.CardinalitySelection; i++)
  { int theGenIndex=splittingParSelectedInLevi.elements[i]+this->GetOwner().GetRank();
    Matrix<Rational>& currentOp=this->actionsSimpleGensMatrixForM[theGenIndex];
    currentOp.FindZeroEigenSpacE(eigenSpacesPerSimpleGenerator[i], 1, -1, 0, theGlobalVariables);
    if (i==0)
      theFinalEigenSpace.AssignListListCoefficientType(eigenSpacesPerSimpleGenerator[i]);
    else
    { tempSpace1=theFinalEigenSpace;
      tempSpace2.AssignListListCoefficientType(eigenSpacesPerSimpleGenerator[i]);
      theFinalEigenSpace.IntersectTwoLinSpaces(tempSpace1, tempSpace2, theFinalEigenSpace, theGlobalVariables);
    }
  }
  out << "<br>Eigenvectors:<table> ";
//  Vector<Rational> zeroRoot;
//  zeroRoot.MakeZero(this->theAlgebra->GetRank());
  std::stringstream readyForLatexComsumption;
  readyForLatexComsumption << "\\begin{tabular}{|lll|}\n <br>";
  std::stringstream extraHWAddition;
  for (int i=0; i<this->parabolicSelectionNonSelectedAreElementsLevi.CardinalitySelection; i++)
    extraHWAddition << "x_{" << this->parabolicSelectionNonSelectedAreElementsLevi.elements[i]+1
    << "}\\omega_{" << this->parabolicSelectionNonSelectedAreElementsLevi.elements[i]+1 << "}+";
  readyForLatexComsumption << "\\hline\\multicolumn{3}{|c|}{Highest weight $" << extraHWAddition.str();
  readyForLatexComsumption
  << this->GetOwner().theWeyl.GetEpsilonCoords(this->theHWSimpleCoordS, theGlobalVariables)
  .ElementToStringEpsilonForm(true, false)
  << "$}\\\\\n<br>";
  readyForLatexComsumption << "weight fund. coord.& weight& singular vector \\\\\\hline\n<br>";
  for (int j=0; j<theFinalEigenSpace.size; j++)
  { out << "<tr><td>";
    ElementUniversalEnveloping<Rational> currentElt, tempElt;
    currentElt.MakeZero(*this->theAlgebras, this->indexAlgebra);
    Vector<Rational>& currentVect= theFinalEigenSpace[j];
    int lastNonZeroIndex=-1;
    for (int i=0; i<currentVect.size; i++)
      if (currentVect[i]!=0)
      { tempElt.MakeZero(*this->theAlgebras, this->indexAlgebra);
        tempElt.AddMonomial(this->theGeneratingWordsNonReduced[i], 1);
        tempElt*=currentVect[i];
        currentElt+=tempElt;
        lastNonZeroIndex=i;
      }
    Vector<Rational>& currentWeight=this->theGeneratingWordsNonReducedWeights[lastNonZeroIndex];
    readyForLatexComsumption << "$" << extraHWAddition.str()
    << this->GetOwner().theWeyl.GetFundamentalCoordinatesFromSimple(currentWeight)
    .ElementToStringLetterFormat("\\omega", false, false)
    << "$&$" << extraHWAddition.str()
    << this->GetOwner().theWeyl.GetEpsilonCoords(currentWeight, theGlobalVariables)
    .Vector<Rational>::ElementToStringEpsilonForm(true, false)
    << "$&$" << currentVect.ElementToStringLetterFormat("m", true, false) << "$";

    if (currentElt.size>1)
      out << "(";
    out << currentElt.ElementToString(false, theGlobalVariables, theFormat);
    if (currentElt.size>1)
      out << ")";
    out << " v(" << this->theHWFundamentalCoordS.ElementToString() << "," << ((Vector<Rational>)this->parabolicSelectionNonSelectedAreElementsLevi).ElementToString() << ")" ;
    out << "</td><td>(weight: "
    << currentWeight.ElementToString() << ")</td></tr>";
    readyForLatexComsumption << "\\\\\n<br>";
  }
  out << "</table>";
  readyForLatexComsumption << "\\hline \n<br> \\end{tabular}";
  out << "<br>Your ready for LaTeX consumption text follows.<br>";
  out << readyForLatexComsumption.str();
  if (Report!=0)
    *Report=out.str();
}

int ParserNode::EvaluateMakeWeylFromParSel
(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ Vector<Rational> parSel;
  int theDim=theNode.owner->theHmm.theRange().GetRank();
  if (! theNode.GetRootRationalFromFunctionArguments(parSel, theGlobalVariables))
    return theNode.SetError(theNode.errorBadOrNoArgument);
  if (parSel.size!=theDim)
    return theNode.SetError(theNode.errorDimensionProblem);
  std::stringstream out;
  ReflectionSubgroupWeylGroup tempWeyl;
  Selection tempSel;
  tempSel=parSel;
  tempWeyl.MakeParabolicFromSelectionSimpleRoots
  (theNode.owner->theHmm.theRange().theWeyl, tempSel, theGlobalVariables, 1)
  ;
  tempWeyl.ComputeRootSubsystem();
  out << tempWeyl.ElementToString(false);
  theNode.outputString=out.str();
  theNode.ExpressionType=theNode.typeString;
  return theNode.errorNoError;
}

bool charSSAlgMod::SplitCharOverLevi
(std::string* Report, charSSAlgMod& output, Vector<Rational>& splittingParSel, const Vector<Rational>& ParSelFDInducingPart,
 ReflectionSubgroupWeylGroup& outputWeylSub, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  std::string tempS;
//  std::cout << "Splitting parabolic selection: " << splittingParSel.ElementToString();
  outputWeylSub.MakeParabolicFromSelectionSimpleRoots(this->GetOwner().theWeyl, splittingParSel, theGlobalVariables,1);
  outputWeylSub.ComputeRootSubsystem();
//  ReflectionSubgroupWeylGroup complementGroup;
//  complementGroup.MakeParabolicFromSelectionSimpleRoots(this->theBoss->theWeyl, invertedSel, theGlobalVariables,1);
//  complementGroup.ComputeRootSubsystem();
  out << outputWeylSub.ElementToString(false);
//  std::cout << "<hr> the Weyl subgroup: " << outputWeylSub.ElementToString(false);
//  std::cout << this->ElementToString();
//  std::cout << out.str();
  charSSAlgMod charAmbientFDWeyl, remainingCharDominantLevi;
  ReflectionSubgroupWeylGroup theFDWeyl;
  theFDWeyl.MakeParabolicFromSelectionSimpleRoots
  (this->GetOwner().theWeyl, ParSelFDInducingPart, theGlobalVariables, 1);
  MonomialChar<Rational> tempMon, localHighest;
  List<Rational> tempMults;
  HashedList<Vector<Rational> > tempHashedRoots;
  WeylGroup& theWeyL=this->GetOwner().theWeyl;
  charAmbientFDWeyl.Reset();
  for (int i=0; i<this->size; i++)
  { MonomialChar<Rational>& currentMon=this->TheObjects[i];
    if (!theFDWeyl.FreudenthalEvalIrrepIsWRTLeviPart
        (currentMon.weightFundamentalCoords, tempHashedRoots, tempMults, tempS, theGlobalVariables, 10000))
    { if (Report!=0)
        *Report=tempS;
      return false;
    }
    for (int j=0; j<tempHashedRoots.size; j++)
    { tempMon.Coefficient=currentMon.Coefficient;
      tempMon.weightFundamentalCoords=theWeyL.GetFundamentalCoordinatesFromSimple(tempHashedRoots[j]);
      tempMon.Coefficient*=tempMults[j];
      charAmbientFDWeyl+=tempMon;
    }
  }
//  std::cout << "<hr>" << tempS;
  remainingCharDominantLevi.Reset();
  Vectors<Rational> tempRootS;
  Vectors<Rational> orbitDom;
  for (int i=0; i<charAmbientFDWeyl.size; i++)
  { orbitDom.SetSize(0);
    if (!theFDWeyl.GenerateOrbitReturnFalseIfTruncated
        (theWeyL.GetSimpleCoordinatesFromFundamental(charAmbientFDWeyl[i].weightFundamentalCoords),
         orbitDom, 10000))
    { out << "failed to generate the complement-sub-Weyl-orbit of weight "
      << this->GetOwner().theWeyl.GetSimpleCoordinatesFromFundamental
      (charAmbientFDWeyl[i].weightFundamentalCoords).ElementToString();
      if (Report!=0)
        *Report=out.str();
      return false;
    }
    for (int k=0; k<orbitDom.size; k++)
      if (outputWeylSub.IsDominantWeight(orbitDom[k]))
      { tempMon.Coefficient=charAmbientFDWeyl[i].Coefficient;
        tempMon.weightFundamentalCoords=theWeyL.GetFundamentalCoordinatesFromSimple(orbitDom[k]);
        remainingCharDominantLevi+=tempMon;
      }
  }
  output.Reset();
//  out << "<br>Character w.r.t Levi part: " << CGI::GetHtmlMathDivFromLatexAddBeginARCL
//  (remainingCharDominantLevi.ElementToString("V", "\\omega", false));
//  std::cout << "Character w.r.t Levi part: " << CGI::GetHtmlMathDivFromLatexAddBeginARCL
//  (remainingCharDominantLevi.ElementToString("V", "\\omega", false));


  while(!remainingCharDominantLevi.IsEqualToZero())
  { localHighest=*remainingCharDominantLevi.LastObject();
    for (bool Found=true; Found; )
    { Found=false;
      for (int i=0; i<outputWeylSub.simpleGenerators.size; i++)
      { tempMon=localHighest;
        tempMon.weightFundamentalCoords+=this->GetOwner().theWeyl.GetFundamentalCoordinatesFromSimple
        (outputWeylSub.simpleGenerators[i]);
        if (remainingCharDominantLevi.Contains(tempMon))
        { localHighest=tempMon;
          Found=true;
        }
      }
    }
    localHighest=remainingCharDominantLevi[remainingCharDominantLevi.GetIndex(localHighest)];
    output+=localHighest;
    if (!outputWeylSub.FreudenthalEvalIrrepIsWRTLeviPart
        (localHighest.weightFundamentalCoords, tempHashedRoots, tempMults, tempS, theGlobalVariables, 10000))
    { if (Report!=0)
        *Report=tempS;
      return false;
    }
    for (int i=0; i<tempHashedRoots.size; i++)
    { tempMon.weightFundamentalCoords=theWeyL.GetFundamentalCoordinatesFromSimple(tempHashedRoots[i]);
      tempMon.Coefficient=tempMults[i]* localHighest.Coefficient;
      remainingCharDominantLevi-=tempMon;
    }
  }
  out << "<br>Character w.r.t Levi part: " << CGI::GetHtmlMathDivFromLatexAddBeginARCL
  (output.ElementToString("V", "\\omega", false))
  ;

  if (Report!=0)
  { //out << "<hr>"  << "The split character is: " << output.ElementToString("V", "\\omega", false);
    DrawingVariables theDV;
    std::string tempS;
    this->DrawMeNoMults(tempS, theGlobalVariables, theDV, 10000);
    Vector<Rational> tempRoot;
    out << "<hr>In the following weight visualization, a yellow line is drawn if the corresponding weights are "
    << " simple reflections of one another, with respect to a simple Vector<Rational> of the Levi part of the parabolic subalgebra. ";
    for (int i=0; i<output.size; i++)
    { tempRoot=theWeyL.GetSimpleCoordinatesFromFundamental(output.TheObjects[i].weightFundamentalCoords);
      outputWeylSub.DrawContour
      (tempRoot, theDV, theGlobalVariables, CGI::RedGreenBlue(200, 200, 0), 1000);
      std::stringstream tempStream;
      tempStream << output[i].Coefficient.ElementToString();
      theDV.drawTextAtVectorBuffer(tempRoot, tempStream.str(), 0, DrawingVariables::PenStyleNormal, 0);
    }
    out << "<hr>" << theDV.GetHtmlFromDrawOperationsCreateDivWithUniqueName(theWeyL.GetDim());
    *Report=out.str();
  }
  return true;
}

bool ReflectionSubgroupWeylGroup::IsDominantWeight(Vector<Rational>& theWeight)
{ for (int i=0; i<this->simpleGenerators.size; i++)
    if (this->AmbientWeyl.RootScalarCartanRoot(theWeight, this->simpleGenerators[i]).IsNegative())
      return false;
  return true;
}

bool ReflectionSubgroupWeylGroup::GetAlLDominantWeightsHWFDIMwithRespectToAmbientAlgebra
(Vector<Rational>& highestWeightSimpleCoords, HashedList<Vector<Rational> >& outputWeightsSimpleCoords,
 int upperBoundDominantWeights, std::string& outputDetails, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
//  double startTime=theGlobalVariables.GetElapsedSeconds();
//  std::cout << "<br>time elapsed: " << theGlobalVariables.GetElapsedSeconds()-startTime;
  Vector<Rational> highestWeightTrue=highestWeightSimpleCoords;
  Vectors<Rational> basisEi;
  int theDim=this->AmbientWeyl.GetDim();
  basisEi.MakeEiBasis(theDim);
  this->RaiseToDominantWeight(highestWeightTrue);
  Vector<Rational> highestWeightFundCoords=this->AmbientWeyl.GetFundamentalCoordinatesFromSimple(highestWeightTrue);
  if (!highestWeightFundCoords.SumCoords().IsSmallInteger())
    return false;
  int theTopHeightSimpleCoords=(int) highestWeightSimpleCoords.SumCoords().DoubleValue()+1;
//  int theTopHeightFundCoords=(int) highestWeightFundCoords.SumCoords().DoubleValue();
  if (theTopHeightSimpleCoords<0)
    theTopHeightSimpleCoords=0;
  List<HashedList<Vector<Rational> > > outputWeightsByHeight;
  int topHeightRootSystem=this->AmbientWeyl.RootsOfBorel.LastObject()->SumCoords().NumShort;
  int topHeightRootSystemPlusOne=topHeightRootSystem+1;
  outputWeightsByHeight.SetSize(topHeightRootSystemPlusOne);
  int finalHashSize=100;
  for (int i=0; i<topHeightRootSystemPlusOne; i++)
    outputWeightsByHeight[i].SetHashSizE(finalHashSize);
  outputWeightsSimpleCoords.Clear();
  outputWeightsByHeight[0].AddOnTop(highestWeightTrue);
  int numTotalWeightsFound=0;
  int numPosRoots=this->AmbientWeyl.RootsOfBorel.size;
  Vector<Rational> currentWeight, currentWeightRaisedToDominantWRTAmbientAlgebra;
//  std::cout << "<br>time spend before working cycle: " << theGlobalVariables.GetElapsedSeconds()-startTime;
  for (int lowestUnexploredHeightDiff=0; lowestUnexploredHeightDiff<=theTopHeightSimpleCoords;
  lowestUnexploredHeightDiff++)
  { //double startCycleTime=theGlobalVariables.GetElapsedSeconds();
    if (upperBoundDominantWeights>0 && numTotalWeightsFound>upperBoundDominantWeights)
      break;
    int bufferIndexShift=lowestUnexploredHeightDiff%topHeightRootSystemPlusOne;
    HashedList<Vector<Rational> >& currentHashes=outputWeightsByHeight[bufferIndexShift];
    for (int lowest=0; lowest<currentHashes.size; lowest++)
      for (int i=0; i<numPosRoots; i++)
      { currentWeight=currentHashes[lowest];
        currentWeight-=this->AmbientWeyl.RootsOfBorel[i];
        if (this->IsDominantWeight(currentWeight))
        { currentWeightRaisedToDominantWRTAmbientAlgebra=currentWeight;
          this->AmbientWeyl.RaiseToDominantWeight(currentWeightRaisedToDominantWRTAmbientAlgebra);
          currentWeightRaisedToDominantWRTAmbientAlgebra-=highestWeightTrue;
          if (currentWeightRaisedToDominantWRTAmbientAlgebra.IsNegativeOrZero())
          { int currentIndexShift=this->AmbientWeyl.RootsOfBorel[i].SumCoords().NumShort;
            currentIndexShift=(currentIndexShift+bufferIndexShift)%topHeightRootSystemPlusOne;
            if (outputWeightsByHeight[currentIndexShift].AddNoRepetition(currentWeight))
            { numTotalWeightsFound++;
              outputWeightsByHeight[currentIndexShift].AdjustHashes();
            }
          }
        }
      }
//    std::cout << "<br>time spent before accounting at height level " << lowestUnexploredHeightDiff
//    << ": " << theGlobalVariables.GetElapsedSeconds()-startCycleTime;
//    std::cout << " Size of current level: " << currentHashes.size;
    outputWeightsSimpleCoords.AddOnTop(currentHashes);
//    std::cout << ". Time spent after accounting at height level " << lowestUnexploredHeightDiff
//    << ": " << theGlobalVariables.GetElapsedSeconds()-startCycleTime;
//    startCycleTime=theGlobalVariables.GetElapsedSeconds();
    outputWeightsSimpleCoords.AdjustHashes();
    currentHashes.Clear();
//    std::cout << ". Time spent clearing up buffer at height level " << lowestUnexploredHeightDiff
//    << ": " << theGlobalVariables.GetElapsedSeconds()-startCycleTime;
  }
  out << " Total number of dominant weights: " << outputWeightsSimpleCoords.size;
  if (numTotalWeightsFound>=upperBoundDominantWeights)
    out << "<hr>This message is generated either because the number of weights has "
    << "exceeded the hard-coded RAM memory limits, or because "
    << " a priori bound for the number of weights is WRONG. If the latter "
    << " is the case, make sure to send an angry email to the author(s).";
  outputDetails=out.str();
  //std::cout << "<hr><hr>Total time spent generating weights: " << -startTime+theGlobalVariables.GetElapsedSeconds();
  return (numTotalWeightsFound<=upperBoundDominantWeights);
}

bool ReflectionSubgroupWeylGroup::GetAlLDominantWeightsHWFDIM
(Vector<Rational>& highestWeightSimpleCoords, HashedList<Vector<Rational> >& outputWeightsSimpleCoords,
 int upperBoundDominantWeights, std::string& outputDetails, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  this->ComputeRootSubsystem();
//  double startTime=theGlobalVariables.GetElapsedSeconds();
//  std::cout << "<br>time elapsed: " << theGlobalVariables.GetElapsedSeconds()-startTime;
  Vector<Rational> highestWeightTrue=highestWeightSimpleCoords;
  Vectors<Rational> basisEi;
  int theDim=this->AmbientWeyl.GetDim();
  basisEi.MakeEiBasis(theDim);
  this->RaiseToDominantWeight(highestWeightTrue);
  Vector<Rational> highestWeightFundCoords=this->AmbientWeyl.GetFundamentalCoordinatesFromSimple(highestWeightTrue);
  if (!highestWeightFundCoords.SumCoords().IsSmallInteger())
    return false;
  int theTopHeightSimpleCoords=(int) highestWeightSimpleCoords.SumCoords().DoubleValue()+1;
//  int theTopHeightFundCoords=(int) highestWeightFundCoords.SumCoords().DoubleValue();
  if (theTopHeightSimpleCoords<0)
    theTopHeightSimpleCoords=0;
  List<HashedList<Vector<Rational> > > outputWeightsByHeight;
  int topHeightRootSystem=this->AmbientWeyl.RootsOfBorel.LastObject()->SumCoords().NumShort;
  int topHeightRootSystemPlusOne=topHeightRootSystem+1;
  outputWeightsByHeight.SetSize(topHeightRootSystemPlusOne);
  int finalHashSize=100;
  for (int i=0; i<topHeightRootSystemPlusOne; i++)
    outputWeightsByHeight[i].SetHashSizE(finalHashSize);
  outputWeightsSimpleCoords.Clear();
  outputWeightsByHeight[0].AddOnTop(highestWeightTrue);
  int numTotalWeightsFound=0;
  Vector<Rational> currentWeight, currentWeightRaisedToDominant;
//  std::cout << "<br>time spend before working cycle: " << theGlobalVariables.GetElapsedSeconds()-startTime;
  for (int lowestUnexploredHeightDiff=0; lowestUnexploredHeightDiff<=theTopHeightSimpleCoords;
  lowestUnexploredHeightDiff++)
  { //double startCycleTime=theGlobalVariables.GetElapsedSeconds();
    if (upperBoundDominantWeights>0 && numTotalWeightsFound>upperBoundDominantWeights)
      break;
    int bufferIndexShift=lowestUnexploredHeightDiff%topHeightRootSystemPlusOne;
    HashedList<Vector<Rational> >& currentHashes=outputWeightsByHeight[bufferIndexShift];
    for (int lowest=0; lowest<currentHashes.size; lowest++)
      for (int i=0; i<this->RootsOfBorel.size; i++)
      { currentWeight=currentHashes[lowest];
        currentWeight-=this->RootsOfBorel[i];
        if (this->IsDominantWeight(currentWeight))
        { int currentIndexShift=this->RootsOfBorel[i].SumCoords().NumShort;
          currentIndexShift=(currentIndexShift+bufferIndexShift)%topHeightRootSystemPlusOne;
          if (outputWeightsByHeight[currentIndexShift].AddNoRepetition(currentWeight))
          { numTotalWeightsFound++;
            outputWeightsByHeight[currentIndexShift].AdjustHashes();
          }
        }
      }
//    std::cout << "<br>time spent before accounting at height level " << lowestUnexploredHeightDiff
//    << ": " << theGlobalVariables.GetElapsedSeconds()-startCycleTime;
//    std::cout << " Size of current level: " << currentHashes.size;
    outputWeightsSimpleCoords.AddOnTop(currentHashes);
//    std::cout << ". Time spent after accounting at height level " << lowestUnexploredHeightDiff
//    << ": " << theGlobalVariables.GetElapsedSeconds()-startCycleTime;
//    startCycleTime=theGlobalVariables.GetElapsedSeconds();
    outputWeightsSimpleCoords.AdjustHashes();
    currentHashes.Clear();
//    std::cout << ". Time spent clearing up buffer at height level " << lowestUnexploredHeightDiff
//    << ": " << theGlobalVariables.GetElapsedSeconds()-startCycleTime;
  }
  out << " Total number of dominant weights: " << outputWeightsSimpleCoords.size;
  if (numTotalWeightsFound>=upperBoundDominantWeights)
    out << "<hr>This message is generated either because the number of weights has "
    << "exceeded the hard-coded RAM memory limits, or because "
    << " a priori bound for the number of weights is WRONG. If the latter "
    << " is the case, make sure to send an angry email to the author(s).";
  outputDetails=out.str();
  //std::cout << "<hr><hr>Total time spent generating weights: " << -startTime+theGlobalVariables.GetElapsedSeconds();
  return (numTotalWeightsFound<=upperBoundDominantWeights);
}

void ReflectionSubgroupWeylGroup::RaiseToDominantWeight(Vector<Rational>& theWeight, int* sign, bool* stabilizerFound)
{ if (sign!=0)
    *sign=1;
  if (stabilizerFound!=0)
    *stabilizerFound=false;
  Rational theScalarProd;
//  int theDim=this->AmbientWeyl.GetDim();
  for (bool found = true; found; )
  { found=false;
    for (int i=0; i<this->simpleGenerators.size; i++)
    { theScalarProd=this->AmbientWeyl.RootScalarCartanRoot(theWeight, this->simpleGenerators[i]);
      if (theScalarProd.IsNegative())
      { found=true;
        this->AmbientWeyl.ReflectBetaWRTAlpha(this->simpleGenerators[i], theWeight, false, theWeight);
        if (sign!=0)
          *sign*=-1;
      }
      if (stabilizerFound!=0)
        if (theScalarProd.IsEqualToZero())
          *stabilizerFound=true;
    }
  }
//  std::cout << "<hr># simple reflections applied total: " << numTimesReflectionWasApplied;

}

void Parser::initDefaultFolderAndFileNames
  (const std::string& inputPathBinaryBaseIsFolderBelow, const std::string& inputDisplayPathBase, const std::string& scrambledIP)
{ this->PhysicalPathServerBase=inputPathBinaryBaseIsFolderBelow+"../";
  this->DisplayPathServerBase=inputDisplayPathBase;

  this->PhysicalPathOutputFolder=this->PhysicalPathServerBase+"output/";
  this->DisplayPathOutputFolder= this->DisplayPathServerBase + "output/";

  this->userLabel=scrambledIP;

  this->PhysicalNameDefaultOutput=this->PhysicalPathOutputFolder+"default"+this->userLabel+"output";
  this->DisplayNameDefaultOutputNoPath="default"+this->userLabel+"output";
  this->DisplayNameDefaultOutput=this->DisplayPathOutputFolder+this->DisplayNameDefaultOutputNoPath;

  this->indicatorFileName=this->PhysicalPathOutputFolder + "indicator" + this->userLabel + ".html" ;
  this->indicatorFileNameDisplay=this->DisplayPathOutputFolder +"indicator" + this->userLabel+ ".html" ;
  this->indicatorReportFileName=this->PhysicalPathOutputFolder +"report"+ this->userLabel+ ".txt" ;
  this->indicatorReportFileNameDisplay=this->DisplayPathOutputFolder+"report"+this->userLabel + ".txt" ;
}

std::string Parser::CreateBasicStructureConstantInfoIfItDoesntExist
(GlobalVariables& theGlobalVariables)
{ std::stringstream stream1, stream2, stream3;
  std::stringstream out;

  stream1 << this->PhysicalPathOutputFolder << this->DefaultWeylLetter << this->DefaultWeylRank << "/";
  std::string PhysicalPathCurrentType=stream1.str();

  stream2 << this->DisplayPathOutputFolder << this->DefaultWeylLetter << this->DefaultWeylRank << "/" << this->DefaultWeylLetter << this->DefaultWeylRank << "table";
  std::string DisplayNameLieBracketNoEnding= stream2.str();

  stream3 << PhysicalPathCurrentType << this->DefaultWeylLetter << this->DefaultWeylRank << "table";
  std::string PhysicalNameLieBracketFullPathNoEnding=stream3.str();

  out << " <hr><b>Chevalley-Weyl basis of simple Lie algebra of type " << this->DefaultWeylLetter << this->DefaultWeylRank
  << "(" << SemisimpleLieAlgebra::GetLieAlgebraName(this->DefaultWeylLetter, this->DefaultWeylRank, true) << ").</b><br>";
  out << "Notation formats: <button " << CGI::GetStyleButtonLikeHtml()
  << " onclick=\"showItem('ChevalleyWeylBasis'); hideItem('ChevalleyWeylBasisRootFormat'); "
  << "hideItem('ChevalleyWeylBasisEpsFormat'); \">Calculator format</button> ";
  out << "<button " << CGI::GetStyleButtonLikeHtml()
  << " onclick=\"hideItem('ChevalleyWeylBasis'); showItem('ChevalleyWeylBasisRootFormat'); "
  << "hideItem('ChevalleyWeylBasisEpsFormat'); \">Simple basis format</button> ";
  out << "<button " << CGI::GetStyleButtonLikeHtml()
  << " onclick=\"hideItem('ChevalleyWeylBasis'); hideItem('ChevalleyWeylBasisRootFormat'); "
  << "showItem('ChevalleyWeylBasisEpsFormat'); \">Epsilon format</button> ";

  out << "<div id=\"ChevalleyWeylBasis\" ><a href=\""
  << DisplayNameLieBracketNoEnding << ".tex\">Latex source</a>. <br>\n<img src=\""
  << DisplayNameLieBracketNoEnding << ".png\"></img></div>";

  out << "<div id=\"ChevalleyWeylBasisRootFormat\" style=\"display: none\"><a href=\""
  << DisplayNameLieBracketNoEnding << "RootFormat.tex\">Latex source</a>. <br>\n<img src=\""
  << DisplayNameLieBracketNoEnding << "RootFormat.png\"></img></div>";

  out << "<div id=\"ChevalleyWeylBasisEpsFormat\" style=\"display: none\"><a href=\""
  << DisplayNameLieBracketNoEnding << "EpsFormat.tex\">Latex source</a>. <br>\n<img src=\""
  << DisplayNameLieBracketNoEnding << "EpsFormat.png\"></img></div>";

  std::string latexCommandTemp;
  if (!CGI::FileExists(PhysicalNameLieBracketFullPathNoEnding+".png") ||
      !CGI::FileExists(PhysicalNameLieBracketFullPathNoEnding+"RootFormat.png" ) ||
      !CGI::FileExists(PhysicalNameLieBracketFullPathNoEnding+"EpsFormat.png")
      )
  { out << "<br>the file: " << PhysicalNameLieBracketFullPathNoEnding << ".png" << " was just created<br>";
    std::fstream lieBracketFile1, lieBracketFile2, lieBracketFile3;
    std::stringstream tempCommand;
    tempCommand << "mkdir " << PhysicalPathCurrentType;
    std::string tempS;
    tempS=tempCommand.str();
    system(tempS.c_str());
    CGI::OpenFileCreateIfNotPresent(lieBracketFile1, PhysicalNameLieBracketFullPathNoEnding+".tex", false, true, false);
    CGI::OpenFileCreateIfNotPresent(lieBracketFile2, PhysicalNameLieBracketFullPathNoEnding+"RootFormat.tex", false, true, false);
    CGI::OpenFileCreateIfNotPresent(lieBracketFile3, PhysicalNameLieBracketFullPathNoEnding+"EpsFormat.tex", false, true, false);
    PolynomialOutputFormat tempFormat;
    tempFormat.MakeAlphabetArbitraryWithIndex("g", "h");
    this->theHmm.theRange().ElementToStringNegativeRootSpacesFirst(tempS, false, false, false, true, true, tempFormat, theGlobalVariables);
    lieBracketFile1 << "\\documentclass{article}\\usepackage{longtable}\n\\begin{document}\\pagestyle{empty}\n" << tempS << "\n\\end{document}";
    this->theHmm.theRange().ElementToStringNegativeRootSpacesFirst(tempS, true, false, false, true, true, tempFormat, theGlobalVariables);
    lieBracketFile2 << "\\documentclass{article}\\usepackage{longtable}\\begin{document}\\pagestyle{empty}\n" << tempS << "\n\\end{document}";
    this->theHmm.theRange().ElementToStringNegativeRootSpacesFirst(tempS, true, true, false, true, true, tempFormat, theGlobalVariables);
    lieBracketFile3 << "\\documentclass{article}\\usepackage{longtable}\n\\begin{document}\\pagestyle{empty}\n" << tempS << "\n\\end{document}";
    this->SystemCommands.AddOnTop("latex  -output-directory=" + PhysicalPathCurrentType + " " + PhysicalNameLieBracketFullPathNoEnding +".tex");
    this->SystemCommands.AddOnTop("latex  -output-directory=" + PhysicalPathCurrentType + " " + PhysicalNameLieBracketFullPathNoEnding +"RootFormat.tex");
    this->SystemCommands.AddOnTop("latex  -output-directory=" + PhysicalPathCurrentType + " " + PhysicalNameLieBracketFullPathNoEnding +"EpsFormat.tex");
    this->SystemCommands.AddOnTop("dvipng " + PhysicalNameLieBracketFullPathNoEnding + ".dvi -o " + PhysicalNameLieBracketFullPathNoEnding + ".png -T tight");
    this->SystemCommands.AddOnTop("dvipng " + PhysicalNameLieBracketFullPathNoEnding + "RootFormat.dvi -o " + PhysicalNameLieBracketFullPathNoEnding + "RootFormat.png -T tight");
    this->SystemCommands.AddOnTop("dvipng " + PhysicalNameLieBracketFullPathNoEnding + "EpsFormat.dvi -o " + PhysicalNameLieBracketFullPathNoEnding + "EpsFormat.png -T tight");
  }
  return out.str();
}

bool ReflectionSubgroupWeylGroup::DrawContour
(const Vector<Rational>& highestWeightSimpleCoord, DrawingVariables& theDV, GlobalVariables& theGlobalVariables, int theColor,
 int UpperBoundVertices)
{ HashedList<Vector<Rational> > theOrbit;
  theOrbit.AddOnTop(highestWeightSimpleCoord);
  WeylGroup& theWeyl=this->AmbientWeyl;
  Vector<Rational> tempRoot;
  for (int i=0; i<theOrbit.size; i++)
    for (int j=0; j<this->simpleGenerators.size; j++)
    { tempRoot=theOrbit[i];
      theWeyl.ReflectBetaWRTAlpha(this->simpleGenerators[j], tempRoot, false, tempRoot);
      if (theOrbit.AddNoRepetition(tempRoot))
        theDV.drawLineBetweenTwoVectorsBuffer(theOrbit[i], tempRoot, DrawingVariables::PenStyleNormal, theColor);
      if (theOrbit.size>UpperBoundVertices)
        return false;
    }
  return true;
}

template <class CoefficientType>
bool ElementUniversalEnveloping<CoefficientType>::ConvertToRationalCoeff
(ElementUniversalEnveloping<Rational>& output)
{ output.MakeZero(*this->owner);
  MonomialUniversalEnveloping<Rational> tempMon;
  Rational theCoeff;
  for (int i=0; i<this->size; i++)
  { MonomialUniversalEnveloping<CoefficientType>& currentMon=this->TheObjects[i];
    tempMon.MakeZero(0,*this->owner);
    if (!this->theCoeffs[i].IsAConstant(theCoeff))
      return false;
    for (int j=0; j<currentMon.Powers.size; j++)
    { Rational tempRat;
      if (!currentMon.Powers[j].IsAConstant(tempRat))
        return false;
      tempMon.MultiplyByGeneratorPowerOnTheRight(currentMon.generatorsIndices[j], tempRat);
    }
    output+=tempMon;
  }
  return true;
}

int ParserNode::EvaluateMultiplyEltGenVermaOnTheRight
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ if (theArgumentList.size!=2)
    return theNode.SetError(theNode.errorBadOrNoArgument);
  ParserNode& left=theNode.owner->TheObjects[theArgumentList[0]];
  ParserNode& right=theNode.owner->TheObjects[theArgumentList[1]];
  theNode.impliedNumVars=MathRoutines::Maximum(left.impliedNumVars, right.impliedNumVars);
  theNode.impliedNumVars=MathRoutines::Maximum(theNode.impliedNumVars, theNode.owner->MaxFoundVars);
  theNode.owner->SetNumVarsModulePolys(theNode.impliedNumVars);
  if (!right.ConvertToType(theNode.typeGenVermaElt, theNode.impliedNumVars, theGlobalVariables))
    return theNode.SetError(theNode.errorConversionError);
  if (!left.ConvertToType(theNode.typeUEelement, theNode.impliedNumVars, theGlobalVariables))
  { if (!left.ConvertToType(theNode.typeGenVermaElt, theNode.impliedNumVars, theGlobalVariables))
      return theNode.SetError(theNode.errorConversionError);
    theNode.theGenVermaElt=left.theGenVermaElt;
//    theNode.theGenVermaElt.GetElement().MultiplyBy(right.theGenVermaElt.GetElement(), theGlobalVariables);
    theNode.ExpressionType=theNode.typeGenVermaElt;
    return theNode.errorNoError;
  }
  theNode.theGenVermaElt=right.theGenVermaElt;
  ElementUniversalEnveloping<RationalFunction> tempElt;
  tempElt.Assign(left.UEElement.GetElement());
  RationalFunction RFone, RFZero;
  RFone.MakeConst(theNode.impliedNumVars, 1, &theGlobalVariables);
  RFZero.MakeConst(theNode.impliedNumVars, 0, &theGlobalVariables);
//  PolynomialOutputFormat theFormat;
//  theFormat.MakeAlphabetArbitraryWithIndex("g", "h");
//  std::cout << "<br>Acting on " << theNode.theGenVermaElt.GetElement().ElementToString(theGlobalVariables)
//  << " by " << tempElt.ElementToString(theGlobalVariables, theFormat);
  theNode.theGenVermaElt.GetElement().MultiplyMeByUEEltOnTheLeft
  (theNode.owner->theModulePolys, tempElt, theGlobalVariables, RFone, RFZero)
  ;
  theNode.ExpressionType=theNode.typeGenVermaElt;
  return theNode.errorNoError;
}

template <class CoefficientType>
void MonomialGeneralizedVerma<CoefficientType>::MultiplyMeByUEEltOnTheLeft
(const CoefficientType& theCoeff, ElementUniversalEnveloping<CoefficientType>& theUE, ElementSumGeneralizedVermas<CoefficientType>& outputAccum,
GlobalVariables& theGlobalVariables, const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{ MonomialGeneralizedVerma<CoefficientType> currentMon;
  CoefficientType currentCoeff;
  for (int j=0; j<theUE.size; j++)
  { currentMon.theMonCoeffOne=theUE[j];
    currentMon.theMonCoeffOne*=this->theMonCoeffOne;
    currentMon.owneR=this->owneR;
    currentMon.indexFDVector=this->indexFDVector;
    currentMon.indexInOwner=this->indexInOwner;
    currentCoeff=theCoeff;
    currentCoeff*=theUE.theCoeffs[j];
    outputAccum.ReduceMonAndAddToMe(currentMon, currentCoeff, theGlobalVariables, theRingUnit, theRingZero);
  }
//  std::cout << "<hr>result: " << this->ElementToString(theGlobalVariables);
}


template <class CoefficientType>
void ElementSumGeneralizedVermas<CoefficientType>::MultiplyMeByUEEltOnTheLeft
  (ElementUniversalEnveloping<CoefficientType>& theUE, GlobalVariables& theGlobalVariables,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{ ElementSumGeneralizedVermas<CoefficientType> output;
  MonomialGeneralizedVerma<CoefficientType> currentMon;
  output.MakeZero(*this->owneR);
  for (int i=0; i<this->size; i++)
    this->TheObjects[i].MultiplyMeByUEEltOnTheLeft(this->theCoeffs[i], theUE, output, theGlobalVariables, theRingUnit, theRingZero);
  *this=output;
//  std::cout << "<hr>result: " << this->ElementToString(theGlobalVariables);
}

template <class CoefficientType>
void ElementTensorsGeneralizedVermas<CoefficientType>::MultiplyMeByUEEltOnTheLeft
  (List<ModuleSSalgebraNew<CoefficientType> >& theOwner, ElementUniversalEnveloping<CoefficientType>& theUE,
   GlobalVariables& theGlobalVariables, const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{ ElementTensorsGeneralizedVermas<CoefficientType> output, tempET;
  output.MakeZero();
  for (int i=0; i<theUE.size; i++)
  { tempET=*this;
    tempET.MultiplyMeByUEEltOnTheLeft(theOwner, theUE[i], theUE.theCoeffs[i], theGlobalVariables, theRingUnit, theRingZero);
    output+=tempET;
  }
  *this=output;
  //for (int i=0; i<theOwner.size; i++)
  //{ //std::cout << "<hr><hr>Module" << i+1 << "<br>" << theOwner[i].ElementToString();
  //}
}

template <class CoefficientType>
void ElementTensorsGeneralizedVermas<CoefficientType>::MultiplyMeByUEEltOnTheLeft
  (List<ModuleSSalgebraNew<CoefficientType> >& theOwner, MonomialUniversalEnveloping<CoefficientType>& theUE,
   const CoefficientType& theCoeff,
   GlobalVariables& theGlobalVariables, const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{ if (theCoeff.IsEqualToZero())
  { this->MakeZero();
    return;
  }
  this->operator*=(theCoeff);
  for (int i=theUE.Powers.size-1; i>=0; i--)
  { int thePower;
    if (!theUE.Powers[i].IsSmallInteger(thePower))
      break;
    int theIndex=theUE.generatorsIndices[i];
    for (int j=0; j<thePower; j++)
    //{ //std::cout <<"<hr>Acting by generator index " << theIndex << " on " << this->ElementToString(theGlobalVariables);
      this->MultiplyMeByElementLieAlg(theOwner, theIndex, theGlobalVariables, theRingUnit, theRingZero);
      //std::cout <<"<br>to get: " << this->ElementToString(theGlobalVariables);
    //}
  }
}

template <class CoefficientType>
void MonomialGeneralizedVerma<CoefficientType>::ReduceMe
(const CoefficientType& myCoeff, ElementSumGeneralizedVermas<CoefficientType>& outputAccum, GlobalVariables& theGlobalVariables,
  const CoefficientType& theRingUnit, const CoefficientType& theRingZero)const
{ if (myCoeff.IsEqualToZero())
  { std::cout << "Warning: this is likely a programming error: if it is not, this message needs to be removed. "
    << " MonomialGeneralizedVerma<CoefficientType>::ReduceMe is called with a zero coefficient input. "
    << " At the moment I programmed this function, I did not foresee any uses of ReduceMe with zero coefficient input, "
    << " however I was not able to affirm that will never happen. "
    << "Hence I do not crash the calculator, but display this benign warning message.";
    return;
  }
//  PolynomialOutputFormat theFormat;
//  theFormat.MakeAlphabetArbitraryWithIndex("g", "h");
//  std::cout << "<br>Reducing  " << theMon.ElementToString( theGlobalVariables, theFormat);
  ModuleSSalgebraNew<CoefficientType>& theMod=this->owneR->TheObjects[this->indexInOwner];
  theMod.GetOwner().OrderSetNilradicalNegativeMost(theMod.parabolicSelectionNonSelectedAreElementsLevi);
//  std::cout << "<br>";
  //for (int i=0; i<theMod.theAlgebra->UEGeneratorOrderIncludingCartanElts.size; i++)
  //{ std::cout << "<br>generator index " << i << " has order " << theMod.theAlgebra->UEGeneratorOrderIncludingCartanElts[i];
  //}
  ElementUniversalEnveloping<CoefficientType> theUEelt;
  theUEelt.MakeZero(*this->GetOwner().theAlgebras, this->GetOwner().indexAlgebra);
  theUEelt.AddMonomial(this->theMonCoeffOne, myCoeff);
//  std::cout << " <br>the UE elt before simplifying: " << theUEelt.ElementToString();
  theUEelt.Simplify(theGlobalVariables, theRingUnit, theRingZero);
//  std::cout << " <br>the UE elt after simplifying: " << theUEelt.ElementToString();
  MonomialUniversalEnveloping<CoefficientType> currentMon;
  MonomialGeneralizedVerma<CoefficientType> newMon;
  CoefficientType theCoeff;
  Matrix<CoefficientType> tempMat1, tempMat2;
//  std::cout << theMod.ElementToString();
//  std::cout << "<br>theMod.theModuleWeightsSimpleCoords.size: "
//  << theMod.theModuleWeightsSimpleCoords.size;
  for (int l=0; l<theUEelt.size; l++)
  { currentMon=theUEelt[l];
//    std::cout << "<br> Processing monomial " << currentMon.ElementToString(false, false, theGlobalVariables, theFormat);
    tempMat1.MakeIdMatrix(theMod.theGeneratingWordsNonReduced.size, theRingUnit, theRingZero);
    for (int k=currentMon.Powers.size-1; k>=0; k--)
    { int thePower;
      if (!currentMon.Powers[k].IsSmallInteger(thePower))
        break;
      int theIndex=currentMon.generatorsIndices[k];
      tempMat2=theMod.GetActionGeneratorIndex(theIndex, theGlobalVariables, theRingUnit, theRingZero);
//      std::cout << "<hr>Action generator " << theIndex << ":<br>"
//      << tempMat2.ElementToString(true, false);
      if (tempMat2.NumRows==0)
      { if (theIndex>=theMod.GetOwner().GetRank()+theMod.GetOwner().GetNumPosRoots())
//        { std::cout << "<br>Error! Accum: " << this->ElementToString(theGlobalVariables);
          return;
//        }
        break;
      }
      for (int s=0; s<thePower; s++)
        tempMat1.MultiplyOnTheLeft(tempMat2, theRingZero);
      currentMon.Powers.size--;
      currentMon.generatorsIndices.size--;
    }
//      std::cout << "<br> Action is the " << currentMon.ElementToString(false, false, theGlobalVariables, theFormat)
//      << " free action plus <br>"
//      << tempMat1.ElementToString(true, false);
    newMon.owneR=this->owneR;
    newMon.indexInOwner=this->indexInOwner;
//    CoefficientType newCoeff;
    for (int i=0; i<tempMat1.NumRows; i++)
      if (!tempMat1.elements[i][this->indexFDVector].IsEqualToZero())
      { newMon.theMonCoeffOne=currentMon;
        newMon.indexFDVector=i;
        outputAccum.AddMonomial(newMon, tempMat1.elements[i][this->indexFDVector]);
      }
  }
//  std::cout << "<br>Matrix of the action: " << tempMat1.ElementToString(true, false);
//  std::cout << "<br> Accum: " << this->ElementToString(theGlobalVariables);
  theMod.GetOwner().OrderSSLieAlgebraStandard();
}


template <class CoefficientType>
void ElementSumGeneralizedVermas<CoefficientType>::ReduceMonAndAddToMe
  (const MonomialGeneralizedVerma<CoefficientType>& theMon, const CoefficientType& theMonCoeff, GlobalVariables& theGlobalVariables,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{ theMon.ReduceMe(theMonCoeff, *this, theGlobalVariables, theRingUnit, theRingZero);
}

template  <class CoefficientType>
Matrix<CoefficientType>& ModuleSSalgebraNew<CoefficientType>::GetActionGeneratorIndex
(int generatorIndex, GlobalVariables& theGlobalVariables,
 const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{ int numGenerators=this->GetOwner().GetNumGenerators();
  int theDim=this->GetOwner().GetRank();
  int numPosRoots=this->GetOwner().GetNumPosRoots();
  assert(generatorIndex>=0 && generatorIndex<numGenerators);
  if (this->ComputedGeneratorActions.selected[generatorIndex])
  { //std::cout << "<br>generator index " << generatorIndex << " is precomputed: "
    //<< this->actionsGeneratorsMaT[generatorIndex].ElementToString(true, false);
    return this->actionsGeneratorsMaT[generatorIndex];
  }
  this->ComputedGeneratorActions.AddSelectionAppendNewIndex(generatorIndex);
  ElementUniversalEnveloping<CoefficientType> tempElt;
  Vector<Rational> theWeight;
  tempElt.MakeOneGenerator(generatorIndex,*this->theAlgebras, this->indexAlgebra, theRingUnit);
  theWeight.MakeZero(theDim);
  if (generatorIndex<numPosRoots)
    theWeight=this->GetOwner().theWeyl.RootSystem[generatorIndex];
  if(generatorIndex>=numPosRoots+theDim)
    theWeight=this->GetOwner().theWeyl.RootSystem[generatorIndex-theDim];
  for (int i=0; i<this->parabolicSelectionNonSelectedAreElementsLevi.CardinalitySelection; i++)
    if (theWeight[this->parabolicSelectionNonSelectedAreElementsLevi.elements[i]]!=0)
    { if (theWeight[this->parabolicSelectionNonSelectedAreElementsLevi.elements[i]]<0)
      { this->actionsGeneratorsMaT[generatorIndex].init(0,0);
//        std::cout << "<br>generator index " << generatorIndex << " has free action. ";
        return this->actionsGeneratorsMaT[generatorIndex];
      } else
      { this->actionsGeneratorsMaT[generatorIndex].init
        (this->theGeneratingWordsNonReduced.size,this->theGeneratingWordsNonReduced.size);
        this->actionsGeneratorsMaT[generatorIndex].NullifyAll(theRingZero);
//        std::cout << "<br>generator index " << generatorIndex << " has ZERO action. ";
        return this->actionsGeneratorsMaT[generatorIndex];
      }
    }

  this->GetMatrixHomogenousElt
  (tempElt, this->actionsGeneratorS[generatorIndex], theWeight,
   this->actionsGeneratorsMaT[generatorIndex], theGlobalVariables, theRingUnit, theRingZero);
//  std::cout << "<br>generator index " << generatorIndex << " has been computed to be: "
//  << this->actionsGeneratorsMaT[generatorIndex].ElementToString(true, false);
  return this->actionsGeneratorsMaT[generatorIndex];
}

template  <class CoefficientType>
void ModuleSSalgebraNew<CoefficientType>::
GetMatrixHomogenousElt
(ElementUniversalEnveloping<CoefficientType>& inputHomogeneous,
  List<List<ElementUniversalEnveloping<CoefficientType> > >& outputSortedByArgumentWeight,
  Vector<Rational>& weightUEEltSimpleCoords, Matrix<CoefficientType>& output,
  GlobalVariables& theGlobalVariables, const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{//  std::cout << "<hr>status of the module @ start GetMatrixHomogenousElt" << this->ElementToString();
  this->GetAdActionHomogenousElT
  (inputHomogeneous, weightUEEltSimpleCoords, outputSortedByArgumentWeight, theGlobalVariables, theRingUnit, theRingZero)
  ;
  output.init(this->theGeneratingWordsNonReduced.size, this->theGeneratingWordsNonReduced.size);
  output.NullifyAll(theRingZero);
  for (int j=0; j<outputSortedByArgumentWeight.size; j++)
    for (int k=0; k<outputSortedByArgumentWeight[j].size; k++)
    { MonomialUniversalEnveloping<CoefficientType>& currentMon=this->theGeneratingWordsGrouppedByWeight[j][k];
      ElementUniversalEnveloping<CoefficientType>& imageCurrentMon=outputSortedByArgumentWeight[j][k];
      int indexColumn=this->theGeneratingWordsNonReduced.GetIndex(currentMon);
      assert(indexColumn!=-1);
      for (int l=0; l< imageCurrentMon.size; l++)
      { int indexRow=this->theGeneratingWordsNonReduced.GetIndex(imageCurrentMon[l]);
        assert(indexRow!=-1);
        output.elements[indexRow][indexColumn]=imageCurrentMon.theCoeffs[l];
//        std::cout <<"<br> Index row: " << indexRow << "; index column: " << indexColumn;
      }
    }
  if (!this->flagIsInitialized)
    return;
//  std::cout << "<hr><hr><hr><hr><hr>input GetMatrixHomogenousElt: " << inputHomogeneous.ElementToString();
//  std::cout << "<hr>status of the module @ GetMatrixHomogenousElt" << this->ElementToString();
//  std::cout << "<hr>output GetMatrixHomogenousElt: " << output.ElementToString(true, false);
}

void charSSAlgMod::DrawMeAssumeCharIsOverCartan
(WeylGroup& actualAmbientWeyl, GlobalVariables& theGlobalVariables, DrawingVariables& theDrawingVars)
{ if (actualAmbientWeyl.GetDim()<2)
    return;
  Vector<Rational> actualWeight;
  actualAmbientWeyl.DrawRootSystem(theDrawingVars, true, theGlobalVariables, false, 0, false);
  for (int j=0; j<this->size; j++)
  { actualWeight=actualAmbientWeyl.GetSimpleCoordinatesFromFundamental(this->TheObjects[j].weightFundamentalCoords);
    theDrawingVars.drawCircleAtVectorBuffer(actualWeight, 5, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(0,0,0));
    theDrawingVars.drawTextAtVectorBuffer
    (actualWeight, this->TheObjects[j].Coefficient.ElementToString(), CGI::RedGreenBlue(0,0,0), theDrawingVars.PenStyleNormal, 0);
  }
}

template <class CoefficientType>
void ElementSumGeneralizedVermas<CoefficientType>::MakeHWV
  (List<ModuleSSalgebraNew<CoefficientType> >& theOwner, int TheIndexInOwner, const CoefficientType& theRingUnit)
{ this->owneR=&theOwner;
  MonomialGeneralizedVerma<CoefficientType> theMon;
  theMon.indexInOwner=TheIndexInOwner;
  ModuleSSalgebraNew<CoefficientType>& theMod=this->owneR->TheObjects[TheIndexInOwner];
  theMon.indexFDVector=theMod.theGeneratingWordsNonReduced.size-1;
  theMon.Coefficient.MakeConst(theRingUnit, theMod.theAlgebra);
  theMon.owneR=&theOwner;
  assert(TheIndexInOwner<theOwner.size);
  this->Clear();
  this->AddOnTop(theMon);
}

template <class CoefficientType>
void ElementTensorsGeneralizedVermas<CoefficientType>::MakeHWV
  (List<ModuleSSalgebraNew<CoefficientType> >& theOwner, int TheIndexInOwner, const CoefficientType& theRingUnit)
{ assert(TheIndexInOwner<theOwner.size);
  MonomialTensorGeneralizedVermas<CoefficientType> tensorMon;
  CoefficientType currentCoeff;
  currentCoeff=theRingUnit;
  tensorMon.theMons.SetSize(1);
  MonomialGeneralizedVerma<CoefficientType>& theMon=tensorMon.theMons[0];
  theMon.owneR=&theOwner;
  theMon.indexInOwner=TheIndexInOwner;
  ModuleSSalgebraNew<CoefficientType>& theMod=theOwner.TheObjects[TheIndexInOwner];
  theMon.indexFDVector=theMod.theGeneratingWordsNonReduced.size-1;
  theMon.MakeConst(*theMod.theAlgebras, theMod.indexAlgebra);

  this->Clear();
  this->AddMonomial(tensorMon, theRingUnit);
}

template <class CoefficientType>
void ElementTensorsGeneralizedVermas<CoefficientType>::MultiplyMeByElementLieAlg
  (List<ModuleSSalgebraNew<CoefficientType> >& theOwner, int indexGenerator, GlobalVariables& theGlobalVariables,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{ if (theOwner.size<=0)
    return;
  ElementTensorsGeneralizedVermas<CoefficientType> output;
  output.MakeZero();
  MonomialTensorGeneralizedVermas<CoefficientType> accumMon, monActedOn;
  ElementSumGeneralizedVermas<CoefficientType> tempElt;
  ElementUniversalEnveloping<CoefficientType> theGenerator;
  theGenerator.MakeOneGenerator
  (indexGenerator, *theOwner[0].theAlgebras, theOwner[0].indexAlgebra, theRingUnit);
  ;
//  PolynomialOutputFormat tempFormat;
//  tempFormat.MakeAlphabetArbitraryWithIndex("g", "h");
  CoefficientType currentCoeff;
  for (int i=0; i<this->size; i++)
  { MonomialTensorGeneralizedVermas<CoefficientType>& currentMon=this->TheObjects[i];
    accumMon.theMons.SetSize(0);
    for (int j=0; j<currentMon.theMons.size; j++)
    { tempElt.MakeZero(theOwner);
      tempElt.AddMonomial(currentMon.theMons[j], theRingUnit);
//      std::cout << "<hr> Acting by " << theGenerator.ElementToString(true, theGlobalVariables, tempFormat)
//      << " on " << tempElt.ElementToString(theGlobalVariables) << "<br>";
      tempElt.MultiplyMeByUEEltOnTheLeft(theGenerator, theGlobalVariables, theRingUnit, theRingZero);
//      std::cout << "<br> result: " << tempElt.ElementToString(theGlobalVariables);
      for (int k=0; k<tempElt.size; k++)
      { currentCoeff=this->theCoeffs[i];
        currentCoeff*=tempElt.theCoeffs[k];
        monActedOn=accumMon;
        monActedOn*=(tempElt[k]);
        for (int l=j+1; l<currentMon.theMons.size; l++)
          monActedOn*=currentMon.theMons[l];
        output.AddMonomial(monActedOn, currentCoeff);
//        std::cout << "<br>accounted: " << monActedOn.ElementToString(theGlobalVariables, tempFormat);
      }
      accumMon*=currentMon.theMons[j];
    }
  }
  *this=output;
}

template <class CoefficientType>
std::string ModuleSSalgebraNew<CoefficientType>::ElementToString()const
{ std::stringstream out;
  GlobalVariables theGlobalVariables; PolynomialOutputFormat theformat;
  out << "<br><b>Highest weight fund coords: " << this->theHWFundamentalCoordsBaseField.ElementToString() << "</b>";
  out << "<br>Parabolic selection: " << this->parabolicSelectionNonSelectedAreElementsLevi.ElementToString();
  out << "<br>Actions generators matrix form (" << this->actionsGeneratorsMaT.size << "): ";
  for (int i=0; i<this->actionsGeneratorsMaT.size; i++)
    out << this->actionsGeneratorsMaT[i].ElementToString(true, false) << "";
  out << "<br>Actions generators UE form: " << this->actionsGeneratorS.ElementToStringGeneric();
  out << "<br>Computed generators: " << this->ComputedGeneratorActions.ElementToString();
  out << "<br>" << this->theGeneratingWordsNonReduced.size << " Generating words: ";
  for (int i=0; i<this->theGeneratingWordsNonReduced.size; i++)
  { out << this->theGeneratingWordsNonReduced[i].ElementToString(false, false, theGlobalVariables, theformat) << ", ";
    if ( this->theGeneratingWordsNonReduced[i].ElementToString(false, false, theGlobalVariables, theformat) =="x_{-830168935}")
    { std::cout << "stop";
    this->theGeneratingWordsNonReduced[i].ElementToString(false, false, theGlobalVariables, theformat) ;
    }
  }
/*out <<     this->theAlgebra=other.theAlgebra;
out <<     this->theGeneratingWordsGrouppedByWeight= other.theGeneratingWordsGrouppedByWeight;
out <<     this->theSimpleGens=other.theSimpleGens;
out <<     this->actionsSimpleGens=other.actionsSimpleGens;
out <<     this->actionsSimpleGensMatrixForM= other.actionsSimpleGensMatrixForM;
    this->theBilinearFormsAtEachWeightLevel=other.theBilinearFormsAtEachWeightLevel;
    this->theBilinearFormsInverted=other.theBilinearFormsInverted;
    this->weightsSimpleGens=other.weightsSimpleGens;
    this->theHWDualCoordsBaseField=other.theHWDualCoordsBaseField;
    this->theHWDualCoordS = other.theHWDualCoordS;
    this->theHWSimpleCoordS=other.theHWSimpleCoordS;
    this->theHWFundamentalCoordS=other.theHWFundamentalCoordS;
    this->theHWFundamentalCoordsBaseField= other.theHWFundamentalCoordsBaseField;
    this->theModuleWeightsSimpleCoords=other.theModuleWeightsSimpleCoords;
    this->theCharOverH=other.theCharOverH;
    this->theChaR=other.theChaR;
    this->parabolicSelectionNonSelectedAreElementsLevi=other.parabolicSelectionNonSelectedAreElementsLevi;
    this->parabolicSelectionSelectedAreElementsLevi=other.parabolicSelectionSelectedAreElementsLevi;
    this->flagIsInitialized=other.flagIsInitialized;*/
  return out.str();
}

void ProjectInformation::AddProjectInfo(const std::string& fileName, const std::string& fileDescription)
{ if (this->FileNames.ContainsObject(fileName))
    return;
  this->FileNames.AddOnTop(fileName);
  this->FileDescriptions.AddOnTop(fileDescription);
}

std::string CGI::GetHtmlLinkFromFileName(const std::string& fileName, const std::string& fileDesc)
{ std::stringstream out;
  out << " <a href=\"http://vectorpartition.svn.sourceforge.net/viewvc/vectorpartition/trunk/"
  << CGI::RemovePathFromFileName(fileName) << "?view=markup\">"
  << CGI::RemovePathFromFileName(fileName);
  if (fileDesc!="")
    out << " (" << fileDesc << ")";
  out << "</a>\n";
  return out.str();
}

std::string ProjectInformation::ElementToString()
{ std::stringstream out;
  out << "<button " << CGI::GetStyleButtonLikeHtml() << " onclick=\"switchMenu('sourceDetails');\" >C++ source of the calculator</button>";
  out << "<div id=\"sourceDetails\" style=\"display: none\">";
  for (int i=0; i<this->FileNames.size; i++)
  { out << " <br>\n";
    out << CGI::GetHtmlLinkFromFileName(this->FileNames[i], this->FileDescriptions[i]);
  }
  out << "<br>The calculator is a simple console application (like the C++ \"Hello world!\"). It is managed by an <a href=\"http://httpd.apache.org/\">Apache web server</a>. ";
  out << " <br>The calculator errors get caught either by 1) in-line asserts() (blank screen), or 2) by Apache/the system (internal server error)."
  << "  \n  <br> The file input/output is done via std::fstream. <br>The LaTeX'ing is called using std::system() \
  calls. The LaTeX logs can be found by viewing the calculator page's source. <br> \
  The html output is hardcoded: either by hand or transformed from a separate .html file using a \
  micro-tool written for the purpose. ";
  out << " ";
  out << " \n";
  out << "<hr><b>Installing the calculator on your machine from c++ source. </b><br> \
  In order to get the calculator running on your machine you need to do the following. \
  Simplifying the installation procedure is on our to-do list. \
  <br>0) You need a Linux machine. Tested it only on Ubuntu and OpenSUSE. If you are interested in making the system run on Windows please write us an email. \
  <br>1) Download the c++ files in the links above. Alternatively you can check out the program's "
  << " svn repository. The svn check out command is  &nbsp&nbsp  svn co https://vectorpartition.svn.sourceforge.net/svnroot/vectorpartition vectorpartition &nbsp&nbsp."
  << "<br>2) Put them in a c++ project and make sure the following includes work:"
  << " #include &lt;sys/time.h&gt; #include &lt;unistd.h&gt; #include &lt;pthread.h&gt;. "
  << "They should work by default on almost any Linux distro. "
  << "<br>3) Build the project to a console application named  &nbsp&nbsp calculator &nbsp&nbsp with default console application settings.  "
  << "<br>4) Create a folder to contain the server files; assume without loss of generality the so created folder is called "
  << "&nbsp&nbsp ServerBase/    &nbsp&nbsp."
  << "<br>5) Create folders &nbsp&nbsp ServerBase/cgi-bin  &nbsp&nbsp and &nbsp&nbsp ServerBase/output &nbsp&nbsp. "
  << "<br>6) Enable full read/write access for every user in the folder &nbsp&nbsp ServerBase/output  &nbsp&nbsp. "
  << "<br>7) Copy the file &nbsp&nbsp calculator  &nbsp&nbsp into &nbsp&nbsp ServerBase/cgi-bin/ &nbsp&nbsp and allow read/execute access to every user."
  << "<br> 8) Install an <a href=\"http://httpd.apache.org/\">Apache web server</a> and enable cgi scripts "
  << "from folder &nbsp&nbsp ServerBase/cgi-bin/ &nbsp&nbsp."
  << "<br>9) Configure the Apache server so the adress of physical folder &nbsp&nbsp ServerBase/ &nbsp&nbsp is displayed as &nbsp&nbsp /vpf/ &nbsp&nbsp."
  << "<br>10) The basic installation is now complete; test the calculator by running it through your web browser."
  << "<br>11) To finish the installation install the jsmath in folder &nbsp&nbsp ServerBase/jsmath/ &nbsp&nbsp.";
  out <<	"</div>";

  return out.str();
}
