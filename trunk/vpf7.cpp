#include "vpf.h"
//The files vpf*.cpp are under the same license. The license is included in the last (in alphabetical order)
//vpf*.cpp file. At the moment, this is the file vpf9.cpp. The vpf*.cpp files are considered to be inseparable
//pieces of the same body code; they are separated only to facilitate faster compilation time.

class Qsqrt2;

template < > int List<LittelmannPath>::ListActualSizeIncrement=1000;
template < > int List<MonomialUniversalEnvelopingOrdered<Rational> >::ListActualSizeIncrement=100;
template < > int List<ElementUniversalEnvelopingOrdered<Rational> >::ListActualSizeIncrement=100;
template < > int List<Matrix<Qsqrt2> >::ListActualSizeIncrement=100;
template < > int List<Vector<Qsqrt2> >::ListActualSizeIncrement=100;
template < > int HashedList<MonomialUniversalEnveloping<Qsqrt2> >::PreferredHashSize=100;
template < > int HashedList<MonomialUniversalEnvelopingOrdered<Rational> >::PreferredHashSize=100;
template < > int HashedList<LittelmannPath >::PreferredHashSize=1000;


class Qsqrt2
{
public:
  Rational a;
  Rational b;
  friend std::iostream& operator<< (std::iostream& output, const Qsqrt2& theElt)
  { output << theElt.ElementToString();
    return output;
  }
  void operator =(const Qsqrt2& other)
  { this->a=other.a;
    this->b=other.b;
  }
  void operator =(int x)
  { this->a=x;
    this->b.MakeZero();
  }
  Qsqrt2(const Qsqrt2& other)
  { this->operator=(other);
  }
  Qsqrt2()
  { this->a.MakeZero();
    this->b.MakeZero();
  }
  void MakeAplusSqrt2B(int theA, int theB){ this->a=theA; this->b=theB;}
  Qsqrt2(int x)
  { this->operator=(x);
  }
  bool IsEqualToZero()const
  { return this->a.IsEqualToZero() && this->b.IsEqualToZero();
  }
  void Assign(const Qsqrt2& other)
  { this->operator=(other);
  }
  void ElementToString(std::string& output)
  { output=this->ElementToString();
  }
  std::string ElementToString()const
  { if (this->IsEqualToZero())
      return "0";
    std::stringstream out;
    if (!this->a.IsEqualToZero())
    { out << this->a.ElementToString();
      if (this->b.IsPositive())
        out << "+";
    }
    if (this->b.IsEqualToZero())
      return out.str();
    else
    { Rational tempRat=this->b;
      if (tempRat<0)
      { out << "-";
        tempRat.Minus();
      }
      if (tempRat!=1)
        out << tempRat.ElementToString();
      out << "\\sqrt{2}";
    }
    return out.str();
  }
  void operator+=(const Qsqrt2& other)
  { this->a+=other.a;
    this->b+=other.b;
  }
  void operator-=(const Qsqrt2& other)
  { this->a-=other.a;
    this->b-=other.b;
  }
  void operator*=(const Qsqrt2& other)
  { Qsqrt2 result;
    result.a=this->a*other.a+this->b*other.b*2;
    result.b=other.b*this->a+this->b*other.a;
    this->operator=(result);
  }
  void operator/=(const Rational& other)
  { this->a/=other;
    this->b/=other;
  }
  void operator/=(const Qsqrt2& otheR)
  { Qsqrt2 result, conjugate;
    conjugate=otheR;
    conjugate.b.Minus();
    result.operator=(*this);
    result*=conjugate;
    result/=(otheR.a*otheR.a-otheR.b*otheR.b*2);
    Qsqrt2 old=*this, oldOther=otheR;
    this->operator=(result);
    result*=oldOther;
    result-=old;
    if (!result.IsEqualToZero())
    { std::cout << "  start " << old.ElementToString() << "; oldOther: " << otheR.ElementToString();
      std::cout << "<hr>" << (otheR.a*otheR.a+otheR.b*otheR.b*2).ElementToString();
      old/=oldOther;
    }
    assert(result.IsEqualToZero());
  }
  void Minus()
  { this->a.Minus();
    this->b.Minus();
  }
  void Invert()
  { Qsqrt2 result;
    result=1;
    result/=*this;
    this->operator=(result);
  }
  void MultiplyBy(const Qsqrt2& other)
  { this->operator*=(other);
  }
  void Subtract(const Qsqrt2& other)
  { this->operator-=(other);
  }
};


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
  root tempRoot;
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
//  HashedList<root> orbit;
//  orbit.Reserve(this->RepresentativesQuotientAmbientOrder.size);
  for (int i=0; i<this->RepresentativesQuotientAmbientOrder.size; i++)
  { tempRoot=this->AmbientWeyl.rho;
    this->AmbientWeyl.ActOnRootByGroupElement
  (this->AmbientWeyl.IndexOfObjectHash(this->RepresentativesQuotientAmbientOrder[i]), tempRoot, false, false);
//    orbit.AddOnTopHash(tempRoot);
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
{ root leftImage; leftImage=this->rho;
  root rightImage; rightImage=this->rho;
  this->ActOn(left, leftImage, false, false, (Rational) 0);
  this->ActOn(right, rightImage, false, false, (Rational) 0);
  return (rightImage-leftImage).IsNonNegative() && !(rightImage-leftImage).IsEqualToZero();
}

void ReflectionSubgroupWeylGroup::FindQuotientRepresentatives(int UpperLimit)
{ this->AmbientWeyl.ComputeWeylGroup(UpperLimit);
  root image1;
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
{ int theDim=theNode.owner->theHmm.theRange.GetRank();
  root theWeight;
  if (! theNode.GetRootRationalFromFunctionArguments(theWeight, theGlobalVariables) )
    return theNode.SetError(theNode.errorBadOrNoArgument);
  if (theWeight.size!=theDim)
    return theNode.SetError(theNode.errorDimensionProblem);
  charSSAlgMod& output=theNode.theChar.GetElement();

  output.MakeFromWeight
  (theNode.owner->theHmm.theRange.theWeyl.GetSimpleCoordinatesFromFundamental(theWeight),
   &theNode.owner->theHmm.theRange);
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
  roots currentOrbit;
  WeylGroup& theWeyl=this->theBoss->theWeyl;
  theWeyl.DrawRootSystem(theDrawingVars, false, theGlobalVariables, true);
  int totalNumWeights=0;
  roots dominantWeightsNonHashed;
  HashedList<root> finalWeights;
  for (int i=0; i< CharCartan.size; i++)
  { MonomialChar<Rational>& currentMon=CharCartan[i];
    dominantWeightsNonHashed.size=0;
    dominantWeightsNonHashed.AddOnTop
    (theWeyl.GetSimpleCoordinatesFromFundamental(currentMon.weightFundamentalCoords));
    bool isTrimmed=!theWeyl.GenerateOrbit(dominantWeightsNonHashed, false, finalWeights, false, upperBoundWeights);
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

void charSSAlgMod::MakeFromWeight(const Vector<Rational>& inputWeightSimpleCoords, SemisimpleLieAlgebra* owner)
{ this->Nullify(owner);
  assert(inputWeightSimpleCoords.size==this->theBoss->GetRank());
  MonomialChar<Rational> theMon;
  theMon.weightFundamentalCoords=
  owner->theWeyl.GetFundamentalCoordinatesFromSimple(inputWeightSimpleCoords);
  theMon.Coefficient=1;
  this->AddOnTopHash(theMon);
}

template <class CoefficientType>
std::string MonomialChar<CoefficientType>::TensorAndDecompose
(MonomialChar<CoefficientType>& other, SemisimpleLieAlgebra& owner, charSSAlgMod& output,
 GlobalVariables& theGlobalVariables)
{ std::stringstream errorLog;
  std::string tempS;
  output.Nullify(&owner);
  Rational finalCoefficient;
  finalCoefficient=this->Coefficient*other.Coefficient;
  if (finalCoefficient==0)
    return "";
  WeylGroup& theWeyl=owner.theWeyl;
  root leftHWFundCoords;
  leftHWFundCoords=this->weightFundamentalCoords;
  root rightHWFundCoords;
  rightHWFundCoords =other.weightFundamentalCoords;

  Rational leftTotalDim= theWeyl.WeylDimFormula(leftHWFundCoords);
  Rational rightTotalDim= theWeyl.WeylDimFormula(rightHWFundCoords);
  if (leftTotalDim>rightTotalDim)
  { MathRoutines::swap(leftTotalDim, rightTotalDim);
    MathRoutines::swap(leftHWFundCoords, rightHWFundCoords);
  }
  HashedList<root> weightsLeftSimpleCoords;
  List<Rational> multsLeft;
  if (!theWeyl.FreudenthalEval
      (leftHWFundCoords, weightsLeftSimpleCoords, multsLeft, tempS, theGlobalVariables, 1000000))
  { errorLog << "Freudenthal formula generated error: " << tempS;
    return errorLog.str();
  }
  HashedList<root> currentOrbit;
  const int OrbitSizeHardLimit=10000000;
//  int theRank=theWeyl.GetDim();
  root rightHWSimpleCoords=theWeyl.GetSimpleCoordinatesFromFundamental(rightHWFundCoords);
  roots tempRoots;
  tempRoots.SetSize(1);
//  std::cout << "weights of smaller module: " << weightsLeftSimpleCoords.ElementToString();
  for (int i=0; i<weightsLeftSimpleCoords.size; i++)
  { tempRoots[0]=weightsLeftSimpleCoords[i];
    theWeyl.GenerateOrbit(tempRoots, false, currentOrbit, false, OrbitSizeHardLimit);
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
(root& currentWeightSimpleCoords, root& otherHighestWeightSimpleCoords, WeylGroup& theWeyl,
 Rational& theMult, charSSAlgMod& outputAccum, Rational& finalCoeff
 )
{ //std::cout << "<hr>Accounting " << currentWeightSimpleCoords.ElementToString()
  //<< " with coefficient " << finalCoeff.ElementToString();

  root dominant=currentWeightSimpleCoords;
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
  int index=this->IndexOfObjectHash(other);
  if (index==-1)
  { this->AddOnTopHash(other);
    return;
  }
  this->TheObjects[index].Coefficient+=other.Coefficient;
  if (this->TheObjects[index].Coefficient.IsEqualToZero())
    this->PopIndexSwapWithLastHash(index);
}

void charSSAlgMod::operator-=(const MonomialChar<Rational>& other)
{ if (other.Coefficient.IsEqualToZero())
    return;
  int index=this->IndexOfObjectHash(other);
  if (index==-1)
  { this->AddOnTopHash(other);
    this->LastObject()->Coefficient*=-1;
    return;
  }
  this->TheObjects[index].Coefficient-=other.Coefficient;
  if (this->TheObjects[index].Coefficient.IsEqualToZero())
    this->PopIndexSwapWithLastHash(index);
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
{ assert(this->theBoss==other.theBoss && this->theBoss!=0);
  this->Reserve(other.size+this->size);
  charSSAlgMod result, summand;
  result.Nullify(this->theBoss);
  std::string potentialError;
  for (int i=0; i<this->size; i++)
    for (int j=0; j<other.size; j++)
    { MonomialChar<Rational>& left = this->TheObjects[i];
      MonomialChar<Rational>& right=other[j];
      potentialError=left.TensorAndDecompose(right, *this->theBoss, summand, theGlobalVariables);
      if (potentialError!="")
        return potentialError;
      result+=summand;
    }
  this->operator=(result);
  return "";
}

void charSSAlgMod::MakeTrivial(SemisimpleLieAlgebra& owner)
{ this->Nullify(&owner);
  MonomialChar<Rational> tempMon;
  tempMon.Coefficient=1;
  tempMon.weightFundamentalCoords.MakeZero(owner.GetRank());
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
(root& inputHWsimpleCoords, GlobalVariables& theGlobalVariables)
{ Cone theWeylChamber;
  this->GetWeylChamber(theWeylChamber, theGlobalVariables);
  int theDim=this->GetDim();
  roots strechedVertices=theWeylChamber.Vertices;
  std::string tempS;
  PolynomialOutputFormat tempFormat;
  tempS=theWeylChamber.ElementToString(tempFormat);
  assert(strechedVertices.size==theDim);
  Rational result=1;
  for (int i=0; i<theDim; i++)
  { //root& currentEdge=strechedVertices[i];
    //Rational tempRat=currentEdge.SumCoordinates();
    //currentEdge/=tempRat;
    //Rational strech= (inputHWsimpleCoords.SumCoordinates()/currentEdge.SumCoordinates())+2;
    Rational strech= inputHWsimpleCoords.SumCoordinates()+2;
    result*=strech*theDim;
  }
  result/=Rational::Factorial(theDim);
  result.AssignAbsoluteValue();
  result+=1;
  assert((result-1).IsPositive());
  return result;
}*/

bool WeylGroup::GetAlLDominantWeightsHWFDIM
(root& highestWeightSimpleCoords, HashedList<root>& outputWeightsSimpleCoords,
 int upperBoundDominantWeights, std::string& outputDetails, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
//  double startTime=theGlobalVariables.GetElapsedSeconds();
//  std::cout << "<br>time elapsed: " << theGlobalVariables.GetElapsedSeconds()-startTime;
  root highestWeightTrue=highestWeightSimpleCoords;
  this->RaiseToDominantWeight(highestWeightTrue);
  root highestWeightFundCoords=this->GetFundamentalCoordinatesFromSimple(highestWeightTrue);
  if (!highestWeightFundCoords.SumCoordinates().IsSmallInteger())
  { out << "<hr> The highest weight you gave in simple coordinates: " << highestWeightSimpleCoords.ElementToString()
    << " which equals " << highestWeightFundCoords.ElementToString() << "  in fundamental coordinates "
    << " is not integral dominant.<br>";
    outputDetails=out.str();
    return false;
  }
  int theTopHeightSimpleCoords=(int) highestWeightSimpleCoords.SumCoordinates().DoubleValue()+1;
//  int theTopHeightFundCoords=(int) highestWeightFundCoords.SumCoordinates().DoubleValue();
  if (theTopHeightSimpleCoords<0)
    theTopHeightSimpleCoords=0;
  List<HashedList<root> > outputWeightsByHeight;
  int topHeightRootSystem=this->RootsOfBorel.LastObject()->SumCoordinates().NumShort;
  int topHeightRootSystemPlusOne=topHeightRootSystem+1;
  outputWeightsByHeight.SetSize(topHeightRootSystemPlusOne);
  int finalHashSize=100;
  for (int i=0; i<topHeightRootSystemPlusOne; i++)
    outputWeightsByHeight[i].SetHashSizE(finalHashSize);
  outputWeightsSimpleCoords.ClearTheObjects();
  outputWeightsSimpleCoords.SetHashSizE(10000);
  outputWeightsByHeight[0].AddOnTopHash(highestWeightTrue);
  int numTotalWeightsFound=0;
  int numPosRoots=this->RootsOfBorel.size;
  root currentWeight;
//  std::cout << "<br>time spend before working cycle: " << theGlobalVariables.GetElapsedSeconds()-startTime;
  for (int lowestUnexploredHeightDiff=0; lowestUnexploredHeightDiff<=theTopHeightSimpleCoords;
  lowestUnexploredHeightDiff++)
  { //double startCycleTime=theGlobalVariables.GetElapsedSeconds();
    if (upperBoundDominantWeights>0 && numTotalWeightsFound>upperBoundDominantWeights)
      break;
    int bufferIndexShift=lowestUnexploredHeightDiff%topHeightRootSystemPlusOne;
    HashedList<root>& currentHashes=outputWeightsByHeight[bufferIndexShift];
    for (int lowest=0; lowest<currentHashes.size; lowest++)
      for (int i=0; i<numPosRoots; i++)
      { currentWeight=currentHashes[lowest];
        currentWeight-=this->RootsOfBorel[i];
        if (this->IsDominantWeight(currentWeight))
        { int currentIndexShift=this->RootsOfBorel[i].SumCoordinates().NumShort;
          currentIndexShift=(currentIndexShift+bufferIndexShift)%topHeightRootSystemPlusOne;
          if (outputWeightsByHeight[currentIndexShift].AddOnTopNoRepetitionHash(currentWeight))
          { numTotalWeightsFound++;
            outputWeightsByHeight[currentIndexShift].AdjustHashes();
          }
        }
      }
//    std::cout << "<br>time spent before accounting at height level " << lowestUnexploredHeightDiff
//    << ": " << theGlobalVariables.GetElapsedSeconds()-startCycleTime;
//    std::cout << " Size of current level: " << currentHashes.size;
    outputWeightsSimpleCoords.AddOnTopHash(currentHashes);
//    std::cout << ". Time spent after accounting at height level " << lowestUnexploredHeightDiff
//    << ": " << theGlobalVariables.GetElapsedSeconds()-startCycleTime;
//    startCycleTime=theGlobalVariables.GetElapsedSeconds();
    outputWeightsSimpleCoords.AdjustHashes();
    currentHashes.ClearTheObjects();
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

void WeylGroup::RaiseToDominantWeight(root& theWeight, int* sign, bool* stabilizerFound)
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
(Vector<Rational>& inputHWfundamentalCoords, HashedList<root>& outputDominantWeightsSimpleCoords,
 List<Rational>& outputMultsSimpleCoords, std::string& outputDetails,
 GlobalVariables& theGlobalVariables, int UpperBoundFreudenthal)
{ //double startTimer=theGlobalVariables.GetElapsedSeconds();
  this->ComputeRootSubsystem();
  roots EiBasis;
  EiBasis.MakeEiBasis(this->AmbientWeyl.GetDim());
  List<bool> Explored;
  root hwSimpleCoords=this->AmbientWeyl.GetSimpleCoordinatesFromFundamental(inputHWfundamentalCoords);
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
  root currentWeight, currentDominantRepresentative;
  root Rho=this->GetRho();
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
        int theIndex=outputDominantWeightsSimpleCoords.IndexOfObjectHash(currentDominantRepresentative);
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
(Vector<Rational>& inputHWfundamentalCoords, HashedList<root>& outputDominantWeightsSimpleCoords,
 List<Rational>& outputMultsSimpleCoords, std::string& outputDetails,
 GlobalVariables& theGlobalVariables, int UpperBoundFreudenthal)
{ //double startTimer=theGlobalVariables.GetElapsedSeconds();
  this->ComputeRho(true);
  roots EiBasis;
  EiBasis.MakeEiBasis(this->GetDim());
  List<bool> Explored;
  root hwSimpleCoords=this->GetSimpleCoordinatesFromFundamental(inputHWfundamentalCoords);
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
  root currentWeight, currentDominantRepresentative;
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
        int theIndex=outputDominantWeightsSimpleCoords.IndexOfObjectHash(currentDominantRepresentative);
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
(List<root>& theWeights, List<Rational>& theMults)
{ std::stringstream out;
  MonomialChar<Rational> currentSummand;
  charSSAlgMod theMod;
  theMod.Nullify(this->theBoss);
  theMod.Reserve(theWeights.size);
  theMod.SetHashSizE(theWeights.size);
  assert(theMults.size==theWeights.size);
  for (int i=0; i<theWeights.size; i++)
  { currentSummand.weightFundamentalCoords=
    this->theBoss->theWeyl.GetFundamentalCoordinatesFromSimple(theWeights[i]);
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
  outputCharOwnerSetToZero.Nullify(0);
  root currentWeightFundCoords;
  List<Rational> currentMults;
  HashedList<root> currentWeights;
  std::stringstream localErrors, localDetails;
  std::string localDetail;
  MonomialChar<Rational> tempMon;
  for (int i=0; i<this->size; i++)
  { currentWeightFundCoords=this->TheObjects[i].weightFundamentalCoords;
    if (!this->theBoss->theWeyl.FreudenthalEval
    (currentWeightFundCoords, currentWeights, currentMults, localDetail, theGlobalVariables, upperBoundNumDominantWeights))
    { localErrors << "Encountered error while evaluating freudenthal formula. Error details: " << localDetail
      << "<br> Further computation detail: " << localDetails.str();
      outputDetails=localErrors.str();
      return false;
    }
    if (localDetail!="")
      localDetails << "<br>Monomial " << i+1 << " computation details: " << localDetail;
    for (int j=0; j<currentWeights.size; j++)
    { tempMon.weightFundamentalCoords=this->theBoss->theWeyl.GetFundamentalCoordinatesFromSimple(currentWeights[j]);
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

int ParserNode::EvaluateHWV(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  ParserNode& leftNode=theNode.owner->TheObjects[theArgumentList[0]];
  ParserNode& rightNode=theNode.owner->TheObjects[theArgumentList[1]];
  root parSel, weight;
  if (!leftNode.GetRootRationalDontUseForFunctionArguments(weight, theGlobalVariables))
    return theNode.SetError(theNode.errorBadOrNoArgument);
  if (!rightNode.GetRootRationalDontUseForFunctionArguments(parSel, theGlobalVariables))
    return theNode.SetError(theNode.errorBadOrNoArgument);
  int theDim=theNode.ContextLieAlgebra->GetRank();
  if (parSel.size!=theDim || weight.size!=theDim)
    return theNode.SetError(theNode.errorDimensionProblem);
  ElementGeneralizedVerma<Rational>& theElt=theNode.theGenVermaElt.GetElement();
  ModuleSSalgebraNew<Rational>& theMod=theNode.owner->theModule;
  std::string report;
  if (!theMod.flagIsInitialized)
  { theMod.MakeFromHW
    (theNode.owner->DefaultWeylLetter, theNode.owner->DefaultWeylRank, weight, parSel, theGlobalVariables, 1, 0, &report);
    out << report;
  }
  theElt.MakeHWV(theMod, 1);
  theNode.outputString=out.str();
  theNode.ExpressionType=theNode.typeGenVermaElt;
  return theNode.errorNoError;
}

bool partFractions::ArgumentsAllowed
  (roots& theArguments, std::string& outputWhatWentWrong, GlobalVariables& theGlobalVariables)
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

void Lattice::IntersectWithLineGivenBy(root& inputLine, root& outputGenerator)
{ roots tempRoots;
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
  root& alpha=theWeyl.RootsOfBorel[indexAlpha];
  Rational LengthAlpha=theWeyl.RootScalarCartanRoot(alpha, alpha);
  root alphaScaled=alpha*2/LengthAlpha;
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
    root& r1=this->Waypoints[precedingIndex];
    root& r2=this->Waypoints[precedingIndex-1];
    Rational s1=theWeyl.RootScalarCartanRoot(r1, alphaScaled);
    Rational x= (theMin+1-s2)/(s1-s2);
    this->Waypoints[precedingIndex]= (r1-r2)*x+ r2;
//    std::cout << "<br> fake waypoint corrected: " << this->ElementToString();
  }
//  std::cout << "<br>Min waypoint:" << this->Waypoints[minIndex].ElementToString();
  roots differences;
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
  root& alpha=theWeyl.RootsOfBorel[indexAlpha];
  Rational LengthAlpha=this->owner->RootScalarCartanRoot(alpha, alpha);
  root alphaScaled=alpha*2/LengthAlpha;
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
    root& r1=this->Waypoints[succeedingIndex];
    root& r2=this->Waypoints[succeedingIndex-1];
    Rational s2=theWeyl.RootScalarCartanRoot(r2, alphaScaled);
    Rational x= (theMin+1-s2)/(s1-s2);
    this->Waypoints[succeedingIndex]= (r1-r2)*x+ r2;
  }
  roots differences;
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
  roots output;
  root tempRoot1, tempRoot2;
  output.Reserve(this->Waypoints.size);
  for (int i=0; i<this->Waypoints.size; i++)
  { root& current=this->Waypoints[i];
    if (output.size==0)
      output.AddOnTop(current);
    else if (output.size==1)
    { if (current!=output[0])
        output.AddOnTop(current);
    } else if (output.size>1)
    { root& preceding =*output.LastObject();
      root& precedingThePreceding=output[output.size-2];
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
  root theWeight;
  if (!theNode.GetRootRationalFromFunctionArguments(theWeight, theGlobalVariables))
    return theNode.SetError(theNode.errorBadOrNoArgument);
  WeylGroup& theWeyl=theNode.owner->theHmm.theRange.theWeyl;
  if (theWeight.size!=theWeyl.GetDim())
    return theNode.SetError(theNode.errorDimensionProblem);
  root theWeightInSimpleCoords= theWeyl.GetSimpleCoordinatesFromFundamental(theWeight);
  LittelmannPath& thePath=theNode.theLittelmann.GetElement();
  thePath.MakeFromWeightInSimpleCoords(theWeightInSimpleCoords, theWeyl);
//  theNode.outputString=out.str();
  theNode.ExpressionType=theNode.typeLittelman;
  return theNode.errorNoError;
}

int ParserNode::EvaluateLittelmannPathFromWayPoints
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ //std::stringstream out;
  roots theWeights;
  int theDim;
  if (!theNode.GetRootsEqualDimNoConversionNoEmptyArgument(theArgumentList, theWeights, theDim))
    return theNode.SetError(theNode.errorBadOrNoArgument);
  WeylGroup& theWeyl=theNode.owner->theHmm.theRange.theWeyl;
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
{ WeylGroup& theWeyl=theNode.owner->theHmm.theRange.theWeyl;
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
{ WeylGroup& theWeyl=theNode.owner->theHmm.theRange.theWeyl;
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
  (root& weightInSimpleCoords, WeylGroup& theOwner)
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
  hashedOutput.AddOnTopHash(*this);
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
  //          hashedOutput.AddOnTopNoRepetitionHash(currentPath);
            if (hashedOutput.AddOnTopNoRepetitionHash(currentPath))
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
            //hashedOutput.AddOnTopNoRepetitionHash(currentPath);
            if (hashedOutput.AddOnTopNoRepetitionHash(currentPath))
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
{ root theWeight;
  WeylGroup& theWeyl=theNode.owner->theHmm.theRange.theWeyl;
  if (!theNode.GetRootRationalFromFunctionArguments(theWeight, theGlobalVariables))
    return theNode.SetError(theNode.errorBadOrNoArgument);
  if (theWeight.size!=theWeyl.GetDim())
  { //std::cout << "read dim: " << theWeight.size << " not equal to " << theWeyl.GetDim();
    return theNode.SetError(theNode.errorDimensionProblem);
  }
  std::stringstream out;
  std::string report;
  ModuleSSalgebraNew<Rational> theModule;
  root fullParSel;
  fullParSel.MakeZero(theWeight.size);
  if (!theModule.MakeFromHW
      (theWeyl.WeylLetter, theWeyl.GetDim(), theWeight, fullParSel, theGlobalVariables, 1, 0, &report))
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
  (List<Rational>& theHWDualCoords, GlobalVariables& theGlobalVariables,
    const CoefficientType& theRingUnit, const CoefficientType& theRingZero
   )
{ WeylGroup& theWeyl=this->theAlgebra.theWeyl;
  int theDim=theWeyl.GetDim();
  int numPosRoots=theWeyl.RootsOfBorel.size;
  root targetWeight;
  this->theSimpleGens.SetSize(theDim*2);
  this->weightsSimpleGens.SetSize(theDim*2);
  for (int k=0; k<2; k++)
    for (int i=0; i<theDim; i++)
    { int genChevIndex=(k==0) ? numPosRoots-i-1 : numPosRoots+theDim+i;
      ElementUniversalEnveloping<Rational>& theSimpleGenerator= this->theSimpleGens[k*theDim+i];
      root& simpleGeneratorWeight=this->weightsSimpleGens[k*theDim+i];
      simpleGeneratorWeight=theWeyl.RootsOfBorel[i];
      if (k==0)
        simpleGeneratorWeight.MinusRoot();
      theSimpleGenerator.MakeOneGenerator(genChevIndex, 1, this->theAlgebra);
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
        (currentWordList[j], currentBF.elements[i][j], &theHWDualCoords, theGlobalVariables, theRingUnit, theRingZero, 0)
        ;
        if (i!=j)
          currentBF.elements[j][i]=currentBF.elements[i][j];
      }
    MatrixLargeRational tempMat;
    tempMat.Assign(currentBF);
    Rational tempRat;
    tempMat.ComputeDeterminantOverwriteMatrix(tempRat);
    if (tempRat!=0)
    { this->theBilinearFormsInverted[l]=currentBF;
      this->theBilinearFormsInverted[l].Invert();
    } else
      this->theBilinearFormsInverted[l].init(0,0);
  }
}

template <class CoefficientType>
void ModuleSSalgebraNew<CoefficientType>::ExpressAsLinearCombinationHomogenousElement
  (ElementUniversalEnveloping<CoefficientType>& inputHomogeneous,
   ElementUniversalEnveloping<CoefficientType>& outputHomogeneous,
   int indexInputBasis,
   const List<CoefficientType>& subHiGoesToIthElement,
   GlobalVariables& theGlobalVariables, const CoefficientType& theRingUnit, const CoefficientType& theRingZero
   )
{ Vector <CoefficientType> theScalarProducts;
  List<MonomialUniversalEnveloping<CoefficientType> >& inputBasis =
  this->theGeneratingWordsGrouppedByWeight[indexInputBasis];
  theScalarProducts.SetSize(inputBasis.size);
//  std::cout << "<hr>Expressing " << inputHomogeneous.ElementToString
//  (theGlobalVariables, theGlobalVariables.theDefaultFormat);

  for (int i=0; i<inputBasis.size; i++)
  { inputHomogeneous.HWTAAbilinearForm
    (inputBasis[i], theScalarProducts[i], &subHiGoesToIthElement, theGlobalVariables, theRingUnit, theRingZero, 0);
//    std::cout << "<br>(" << inputHomogeneous.ElementToString(theGlobalVariables, theGlobalVariables.theDefaultFormat)
//    << "," << inputBasis[i].ElementToString(theGlobalVariables, theGlobalVariables.theDefaultFormat)
//    << " )=" << theScalarProducts[i].ElementToString();
  }
  this->theBilinearFormsInverted[indexInputBasis].ActOnVectorColumn(theScalarProducts, theRingZero);
  outputHomogeneous.Nullify(this->theAlgebra);
  ElementUniversalEnveloping<CoefficientType> tempElt;
  for (int i=0; i<theScalarProducts.size; i++)
  { tempElt=this->theGeneratingWordsGrouppedByWeight[indexInputBasis][i];
    tempElt*=theScalarProducts[i];
    outputHomogeneous+=tempElt;
  }
}

template <class CoefficientType>
void ModuleSSalgebraNew<CoefficientType>::GetAdActionHomogenousElT
  (ElementUniversalEnveloping<CoefficientType>& inputHomogeneous, root& weightUEEltSimpleCoords,
   List<List<ElementUniversalEnveloping<CoefficientType> > >& outputSortedByArgumentWeight,
   GlobalVariables& theGlobalVariables, const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{ root targetWeight;
  outputSortedByArgumentWeight.SetSize(this->theGeneratingWordsGrouppedByWeight.size);
  ElementUniversalEnveloping<CoefficientType> theElt;
  for (int i=0; i<this->theGeneratingWordsGrouppedByWeight.size; i++)
  { List<MonomialUniversalEnveloping<CoefficientType> >& currentWordList=
    this->theGeneratingWordsGrouppedByWeight[i];
    List<ElementUniversalEnveloping<CoefficientType> >& outputCurrentList=
    outputSortedByArgumentWeight[i];
    outputCurrentList.SetSize(currentWordList.size);
    root& currentWeight=this->theGeneratingWordsWeightsSimpleCoords[i];
    targetWeight=currentWeight+weightUEEltSimpleCoords;
//    std::cout << "<br>target weight: " << targetWeight.ElementToString() << "="
//    << currentWeight.ElementToString() << "+" << inputWeight.ElementToString();
    int theIndex=this->theGeneratingWordsWeightsSimpleCoords.IndexOfObjectHash(targetWeight);
    for (int j=0; j<currentWordList.size; j++)
    { ElementUniversalEnveloping<CoefficientType>& currentOutputWord=outputCurrentList[j];
      if (theIndex==-1)
        currentOutputWord.Nullify(this->theAlgebra);
      else
      { theElt=inputHomogeneous;
        theElt*=(currentWordList[j]);
        this->ExpressAsLinearCombinationHomogenousElement
        (theElt, currentOutputWord, theIndex, this->theHWDualCoords, theGlobalVariables, theRingUnit, theRingZero);
      }
    }
  }
}

template<class CoefficientType>
bool ModuleSSalgebraNew<CoefficientType>::MakeFromHW
(char WeylLetter, int theRank, root& HWFundCoords, const Selection& selNonSelectedAreElementsLevi,
 GlobalVariables& theGlobalVariables,
const CoefficientType& theRingUnit, const CoefficientType& theRingZero,
 std::string* outputReport)
{ assert(HWFundCoords.size==theRank && selNonSelectedAreElementsLevi.MaxSize==theRank);

  this->theAlgebra.ComputeChevalleyConstants(WeylLetter, theRank, theGlobalVariables);
  WeylGroup& theWeyl=this->theAlgebra.theWeyl;

  this->theHWFundamentalCoords=HWFundCoords;
  this->theHWSimpleCoordS=theWeyl.GetSimpleCoordinatesFromFundamental(this->theHWFundamentalCoords);
  this->theHWDualCoords= theWeyl.GetDualCoordinatesFromFundamental(this->theHWFundamentalCoords);

  this->theChaR.MakeFromWeight(this->theHWSimpleCoordS, &this->theAlgebra);
  this->parabolicSelectionNonSelectedAreElementsLevi=selNonSelectedAreElementsLevi;
  this->parabolicSelectionSelectedAreElementsLevi=this->parabolicSelectionNonSelectedAreElementsLevi;
  this->parabolicSelectionSelectedAreElementsLevi.InvertSelection();

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
  this->theGeneratingWordsWeightsSimpleCoords.ClearTheObjects();
  MonomialChar<Rational> tempCharMon;
  tempCharMon.Coefficient=1;
  this->theCharOverH.Nullify(0);
  for (int i=0; i<thePaths.size; i++)
  { this->theGeneratingWordsWeightsSimpleCoords.AddOnTopNoRepetitionHash(*thePaths[i].Waypoints.LastObject());
    tempCharMon.weightFundamentalCoords= theWeyl.GetFundamentalCoordinatesFromSimple(*thePaths[i].Waypoints.LastObject());
    this->theCharOverH+=tempCharMon;
  }
  this->theGeneratingWordsWeightsSimpleCoords.QuickSortAscending();
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

  this->theGeneratingWordsGrouppedByWeight.SetSize(this->theGeneratingWordsWeightsSimpleCoords.size);
  for (int i=0; i<this->theGeneratingWordsGrouppedByWeight.size; i++)
    this->theGeneratingWordsGrouppedByWeight[i].size=0;
  MonomialUniversalEnveloping<CoefficientType> currentNonReducedElement;
  for (int i=0; i<thePaths.size; i++)
  { List<int>& currentPath= generatorsIndices[i];
    currentNonReducedElement.MakeConst(1, this->theAlgebra);
    for (int j=currentPath.size-1; j>=0; j--)
    { int theIndex=currentPath[j];
      if (theIndex>0)
        theIndex++;
      currentNonReducedElement.MultiplyByGeneratorPowerOnTheRight
      (this->theAlgebra.DisplayIndexToChevalleyGeneratorIndex(theIndex),1);
    }
    root& hwCurrent=*thePaths[i].Waypoints.LastObject();
    int theIndex=this->theGeneratingWordsWeightsSimpleCoords.IndexOfObjectHash(hwCurrent);
    if (theIndex==-1)
    { std::cout << "couldn't find weight : " << hwCurrent.ElementToString() << " in " << this->theGeneratingWordsWeightsSimpleCoords.ElementToString();
      out2 << "Error: could not generate all weights in the weight support. Maybe they are too many? Allowed "
      << " # of weights is 10000";
      if (outputReport!=0)
        *outputReport=out2.str() + monomialDetailStream.str();
      return false;
    }
    this->theGeneratingWordsGrouppedByWeight[theIndex].AddOnTop(currentNonReducedElement);
  }
  this->theGeneratingWordsNonReduced.ClearTheObjects();
  this->theGeneratingWordsNonReduced.SetExpectedSize(thePaths.size);
  this->theGeneratingWordsNonReduced.size=0;
  for (int i=0; i<this->theGeneratingWordsGrouppedByWeight.size; i++)
  { List<MonomialUniversalEnveloping<CoefficientType> >& currentList=this->theGeneratingWordsGrouppedByWeight[i];
    currentList.QuickSortDescending();
    for (int j=0; j<currentList.size; j++)
    { this->theGeneratingWordsNonReduced.AddOnTopHash(currentList[j]);
      if (outputReport!=0)
        monomialDetailStream << "<br>m_{ " << this->theGeneratingWordsNonReduced.size << "} := "
        << currentList[j].ElementToString(false, false, theGlobalVariables, tempFormat) << " \\cdot v ";
    }
  }
  this->IntermediateStepForMakeFromHW(this->theHWDualCoords, theGlobalVariables, theRingUnit, theRingZero);
  bool isBad=false;
  if (outputReport!=0)
    for (int i=0; i<this->theBilinearFormsAtEachWeightLevel.size; i++)
    { Matrix<CoefficientType>& theBF=this->theBilinearFormsAtEachWeightLevel[i];
      Matrix<CoefficientType>& theBFinverted= this->theBilinearFormsInverted[i];
      if (outputReport!=0)
      { monomialDetailStream << "<hr>weight in simple coords: " << this->theGeneratingWordsWeightsSimpleCoords[i].ElementToString();
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
  this->actionsSimpleGens.SetSize(this->theAlgebra.GetRank()*2);
  this->actionsSimpleGensMatrixForM.SetSize(this->theAlgebra.GetRank()*2);
  for (int k=0; k<2; k++)
    for (int j=0; j<this->theAlgebra.GetRank(); j++)
      if (this->parabolicSelectionSelectedAreElementsLevi.selected[j])
      { int theIndex=j+k*this->theAlgebra.GetRank();
        ElementUniversalEnveloping<Rational>& theSimpleGenerator=this->theSimpleGens[theIndex];
        root& simpleWeight=this->weightsSimpleGens[theIndex];
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
      this->actionsSimpleGensMatrixForM[j+k*this->theAlgebra.GetRank()].init(0,0);
  this->ComputedGeneratorActions.init(this->theAlgebra.GetNumGenerators());
  this->actionsGeneratorS.SetSize(this->theAlgebra.GetNumGenerators());
  this->actionsGeneratorsMaT.SetSize(this->theAlgebra.GetNumGenerators());

  if (outputReport!=0)
    *outputReport= out2.str()+monomialDetailStream.str();
  return true;
}

template <class CoefficientType>
void ElementUniversalEnvelopingOrdered<CoefficientType>::ModOutVermaRelations
  (GlobalVariables* theContext, const List<CoefficientType>* subHiGoesToIthElement,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{ MonomialUniversalEnvelopingOrdered<CoefficientType> tempMon;
  ElementUniversalEnvelopingOrdered<CoefficientType> output;
  output.Nullify(*this->owner);
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
    { this->Nullify(theRingZero, *this->owner);
      return;
    }
    if (IndexCurrentGenerator<numPosRoots)
      return;
    if (IndexCurrentGenerator>=numPosRoots &&  IndexCurrentGenerator<numPosRoots+theDimension)
    { int theDegree;
      if (!this->Powers.TheObjects[i].IsSmallInteger(theDegree))
        return;
      if (subHiGoesToIthElement==0)
      { this->Nullify(theRingZero, *this->owner);
        return;
      }
      CoefficientType theSubbedH=theRingZero;
      root& currentH=this->owner->theOrder.TheObjects[IndexCurrentGenerator].Hcomponent;
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
(GlobalVariables* theContext, const root& theHWsimpleCoords, const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{ WeylGroup& theWeyl=this->owner->theOwner.theWeyl;
  root theHWsimpleCoordsTrue=theHWsimpleCoords;
  theWeyl.RaiseToDominantWeight(theHWsimpleCoordsTrue);
  root theHWdualCoords=theWeyl.GetDualCoordinatesFromFundamental
  (theWeyl.GetFundamentalCoordinatesFromSimple(theHWsimpleCoordsTrue));
  List<CoefficientType> theSub;
  theSub.SetSize(theHWdualCoords.size);
  for (int i=0; i<theHWdualCoords.size; i++)
    theSub[i]=theHWdualCoords[i];
  this->ModOutVermaRelations(theContext, &theSub, theRingUnit, theRingZero);
  int numPosRoots=this->owner->theOwner.GetNumPosRoots();
  //int theDimension=this->owner->theOwner.GetRank();
  root currentWeight=theHWsimpleCoordsTrue;
  root testWeight;
  for (int k=this->generatorsIndices.size-1; k>=0; k--)
  { int IndexCurrentGenerator=this->generatorsIndices[k];
    if (IndexCurrentGenerator>=numPosRoots)
      return false;
    ElementSimpleLieAlgebra& currentElt=this->owner->theOrder[IndexCurrentGenerator];
    if (!currentElt.Hcomponent.IsEqualToZero() || currentElt.NonZeroElements.CardinalitySelection!=1)
      return false;
    int thePower=0;
    if (!this->Powers[k].IsSmallInteger(thePower))
      return false;
    int rootIndex= currentElt.NonZeroElements.elements[0];
    root& currentRoot=theWeyl.RootSystem[rootIndex];
//    std::cout << "<hr>The power: " << thePower;
    for (int j=0; j<thePower; j++)
    { currentWeight+=currentRoot;
//      std::cout << "<br>current weight is: " << currentWeight.ElementToString();
      testWeight=currentWeight;
      theWeyl.RaiseToDominantWeight(testWeight);
//      std::cout << "; raised to highest: " << testWeight.ElementToString();
//      std::cout << "<br>theHWsimpleCoordsTrue-currentWeight raised to highest = "
//      << (theHWsimpleCoordsTrue-testWeight).ElementToString();
      if (!(theHWsimpleCoordsTrue-testWeight).IsNonNegative())
      { this->Nullify(theRingZero, *this->owner);
        return true;
      }
    }
  }
  return true;
}

template <class CoefficientType>
bool ElementUniversalEnvelopingOrdered<CoefficientType>::ModOutFDRelationsExperimental
(GlobalVariables* theContext, const root& theHWsimpleCoords, const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{ MonomialUniversalEnvelopingOrdered<CoefficientType> tempMon;
  ElementUniversalEnvelopingOrdered<CoefficientType> output;
  output.Nullify(*this->owner);
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
(GlobalVariables* theContext, const root& theHWsimpleCoords, const CoefficientType& theRingUnit,
 const CoefficientType& theRingZero)
{ MonomialUniversalEnveloping<CoefficientType> tempMon;
  ElementUniversalEnveloping<CoefficientType> output;
  output.Nullify(*this->owner);
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
  outputCorrespondingMonomials.Nullify(*theElements.TheObjects[0].owner);
  Vectors<CoefficientTypeQuotientField> outputCoordsBeforeReduction;
  for (int i=0; i<theElements.size; i++)
    for (int j=0; j<theElements.TheObjects[i].size; j++)
      outputCorrespondingMonomials.AddOnTopNoRepetitionHash(theElements.TheObjects[i].TheObjects[j]);
  outputCoordsBeforeReduction.SetSize(theElements.size);
  for (int i=0; i<theElements.size; i++)
  { Vector<CoefficientTypeQuotientField>& currentList=outputCoordsBeforeReduction.TheObjects[i];
    currentList.MakeZero(outputCorrespondingMonomials.size, theFieldZero);
    ElementUniversalEnveloping<CoefficientType>& currentElt=theElements.TheObjects[i];
    for (int j=0; j<currentElt.size; j++)
    { MonomialUniversalEnveloping<CoefficientType>& currentMon=currentElt.TheObjects[j];
      currentList.TheObjects[outputCorrespondingMonomials.IndexOfObjectHash(currentMon)]=currentMon.Coefficient;
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
  (GlobalVariables* theContext, const root& theHWsimpleCoords, const CoefficientType& theRingUnit,
   const CoefficientType& theRingZero)
{ WeylGroup& theWeyl=this->owner->theWeyl;
  root theHWsimpleCoordsTrue=theHWsimpleCoords;
  theWeyl.RaiseToDominantWeight(theHWsimpleCoordsTrue);
  root theHWdualCoords=theWeyl.GetDualCoordinatesFromFundamental
  (theWeyl.GetFundamentalCoordinatesFromSimple(theHWsimpleCoordsTrue));
  List<CoefficientType> theSub;
  theSub.SetSize(theHWdualCoords.size);
  for (int i=0; i<theHWdualCoords.size; i++)
    theSub[i]=theHWdualCoords[i];
  this->ModOutVermaRelations(theContext, &theSub, theRingUnit, theRingZero);
  int numPosRoots=this->owner->GetNumPosRoots();
  //int theDimension=this->owner->theOwner.GetRank();
  root currentWeight=theHWsimpleCoordsTrue;
  root testWeight;
  ElementSimpleLieAlgebra currentElt;
  for (int k=this->generatorsIndices.size-1; k>=0; k--)
  { int IndexCurrentGenerator=this->generatorsIndices[k];
    if (IndexCurrentGenerator>=numPosRoots)
      return false;
    currentElt.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE
    (IndexCurrentGenerator, *this->owner);
    if (!currentElt.Hcomponent.IsEqualToZero() || currentElt.NonZeroElements.CardinalitySelection!=1)
      return false;
    int thePower=0;
    if (!this->Powers[k].IsSmallInteger(thePower))
      return false;
    int rootIndex= currentElt.NonZeroElements.elements[0];
    root& currentRoot=theWeyl.RootSystem[rootIndex];
//    std::cout << "<hr>The power: " << thePower;
    for (int j=0; j<thePower; j++)
    { currentWeight+=currentRoot;
//      std::cout << "<br>current weight is: " << currentWeight.ElementToString();
      testWeight=currentWeight;
      theWeyl.RaiseToDominantWeight(testWeight);
//      std::cout << "; raised to highest: " << testWeight.ElementToString();
//      std::cout << "<br>theHWsimpleCoordsTrue-currentWeight raised to highest = "
//      << (theHWsimpleCoordsTrue-testWeight).ElementToString();
      if (!(theHWsimpleCoordsTrue-testWeight).IsNonNegative())
      { this->Nullify(theRingZero, *this->owner);
        return true;
      }
    }
  }
  return false;
}

template <class CoefficientType>
void MonomialUniversalEnveloping<CoefficientType>::ModOutVermaRelations
  (GlobalVariables* theContext, const List<CoefficientType>* subHiGoesToIthElement, const CoefficientType& theRingUnit,
   const CoefficientType& theRingZero)
{ int numPosRoots=this->owner->GetNumPosRoots();
  int theDimension=this->owner->GetRank();
  ElementSimpleLieAlgebra currentElt;
  CoefficientType tempCF;
  for (int i=this->generatorsIndices.size-1; i>=0; i--)
  { int IndexCurrentGenerator=this->generatorsIndices.TheObjects[i];
    if (IndexCurrentGenerator>=numPosRoots+theDimension)
    { this->Nullify(theRingZero, *this->owner);
      return;
    }
    if (IndexCurrentGenerator<numPosRoots)
      return;
    if (IndexCurrentGenerator>=numPosRoots &&  IndexCurrentGenerator<numPosRoots+theDimension)
    { int theDegree;
      if (!this->Powers.TheObjects[i].IsSmallInteger(theDegree))
        return;
      if (subHiGoesToIthElement==0)
      { this->Nullify(theRingZero, *this->owner);
        return;
      }
      CoefficientType theSubbedH=theRingZero;
      currentElt.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE
      (IndexCurrentGenerator, *this->owner);
      root& currentH=currentElt.Hcomponent;
      for (int i=0; i<currentH.size; i++)
      { tempCF=(*subHiGoesToIthElement)[i];
        tempCF*=currentH[i];
        theSubbedH+=tempCF;
      }
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

template<class CoefficientType>
void ElementUniversalEnveloping<CoefficientType>::ModToMinDegreeFormFDRels
(const root& theHWinSimpleCoords, GlobalVariables& theGlobalVariables, const CoefficientType& theRingUnit,
 const CoefficientType& theRingZero)
{ ElementUniversalEnveloping<CoefficientType> result, temp;
  result.Nullify(*this->owner);
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
  result.Nullify(*this->owner);
  int numPosRoots=this->owner->GetNumPosRoots();
  int theRank=this->owner->GetRank();
  for (int i=0; i<this->size; i++)
  { MonomialUniversalEnveloping<CoefficientType>& currentMon=this->TheObjects[i];
    tempMon.Coefficient=currentMon.Coefficient;
    tempMon.owner=currentMon.owner;
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
        tempMon.Coefficient*=-1;
//      ;
    }
    result+=tempMon;
  }
  *this=result;
  return true;
}

template<class CoefficientType>
bool ElementUniversalEnveloping<CoefficientType>::ApplyTransposeAntiAutoOnMe()
{ MonomialUniversalEnveloping<CoefficientType> tempMon;
  ElementUniversalEnveloping<CoefficientType> result;
  result.Nullify(*this->owner);
  int numPosRoots=this->owner->GetNumPosRoots();
  int theRank=this->owner->GetRank();
  for (int i=0; i<this->size; i++)
  { MonomialUniversalEnveloping<CoefficientType>& currentMon=this->TheObjects[i];
    tempMon.Coefficient=currentMon.Coefficient;
    tempMon.owner=currentMon.owner;
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
    result+=tempMon;
  }
  *this=result;
  return true;
}

int ParserNode::EvaluateMinusTransposeAuto
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ ElementUniversalEnveloping<PolynomialRationalCoeff>& theElt= theNode.UEElement.GetElement();
  theElt=theNode.owner->TheObjects[theArgumentList[0]].UEElement.GetElement();
  if (!theElt.ApplyMinusTransposeAutoOnMe())
    return theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);
  theNode.ExpressionType=theNode.typeUEelement;
  return theNode.errorNoError;
}

int ParserNode::EvaluateTransposeAntiAuto
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ ElementUniversalEnveloping<PolynomialRationalCoeff>& theElt= theNode.UEElement.GetElement();
  theElt=theNode.owner->TheObjects[theArgumentList[0]].UEElement.GetElement();
  if (!theElt.ApplyTransposeAntiAutoOnMe())
    return theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);
  theNode.ExpressionType=theNode.typeUEelement;
  return theNode.errorNoError;
}

int ParserNode::EvaluateHWMTABilinearForm
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ root weight;
  ParserNode& leftNode=theNode.owner->TheObjects[theArgumentList[0]];
  ParserNode& rightNode=theNode.owner->TheObjects[theArgumentList[1]];
  ParserNode& weightNode=theNode.owner->TheObjects[theArgumentList[2]];
  if (!weightNode.GetRootRationalDontUseForFunctionArguments(weight, theGlobalVariables))
    return theNode.SetError(theNode.errorBadOrNoArgument);
  SemisimpleLieAlgebra& theSSalgebra=theNode.owner->theHmm.theRange;
  if (weight.size!=theSSalgebra.GetRank())
    return theNode.SetError(theNode.errorDimensionProblem);
  ElementUniversalEnveloping<PolynomialRationalCoeff>& leftElt=leftNode.UEElement.GetElement();
  ElementUniversalEnveloping<PolynomialRationalCoeff>& rightElt=rightNode.UEElement.GetElement();
  PolynomialRationalCoeff theRingZero, theRingUnit;
  theNode.impliedNumVars=MathRoutines::Maximum(leftNode.impliedNumVars, rightNode.impliedNumVars);
  int& numVars= theNode.impliedNumVars;
  theRingZero.Nullify(numVars);
  theRingUnit.MakeOne(numVars);
  List<PolynomialRationalCoeff> theHW;
  WeylGroup& theWeyl=theSSalgebra.theWeyl;
  weight=theWeyl.GetDualCoordinatesFromFundamental(weight);
  std::stringstream out;
  out << "Highest weight in dual coords: " << weight.ElementToString() << "<br>";
  theHW.SetSize(weight.size);
  for (int i=0; i<weight.size; i++)
    theHW[i].MakeNVarConst(numVars, weight[i]);
  leftElt.owner->OrderSSalgebraForHWbfComputation();
  if(!leftElt.HWMTAbilinearForm
     (rightElt, theNode.polyValue.GetElement(), &theHW, theGlobalVariables, theRingUnit, theRingZero, &out))
    return theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);
  PolynomialRationalCoeff symmTerm;
  if(!rightElt.HWMTAbilinearForm
     (leftElt, symmTerm, &theHW, theGlobalVariables, theRingUnit, theRingZero, &out))
    return theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);
  leftElt.owner->OrderSSLieAlgebraStandard();
  theNode.polyValue.GetElement()+=symmTerm;
  theNode.ExpressionType=theNode.typePoly;
  theNode.outputString=out.str();
  return theNode.errorNoError;
}

int ParserNode::EvaluateHWTAABilinearForm
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ root weight;
  ParserNode& leftNode=theNode.owner->TheObjects[theArgumentList[0]];
  ParserNode& rightNode=theNode.owner->TheObjects[theArgumentList[1]];
  ParserNode& weightNode=theNode.owner->TheObjects[theArgumentList[2]];
  if (!weightNode.GetRootRationalDontUseForFunctionArguments(weight, theGlobalVariables))
    return theNode.SetError(theNode.errorBadOrNoArgument);
  SemisimpleLieAlgebra& theSSalgebra=theNode.owner->theHmm.theRange;
  if (weight.size!=theSSalgebra.GetRank())
    return theNode.SetError(theNode.errorDimensionProblem);
  ElementUniversalEnveloping<PolynomialRationalCoeff>& leftElt=leftNode.UEElement.GetElement();
  ElementUniversalEnveloping<PolynomialRationalCoeff>& rightElt=rightNode.UEElement.GetElement();
  PolynomialRationalCoeff theRingZero, theRingUnit;
  theNode.impliedNumVars=MathRoutines::Maximum(leftNode.impliedNumVars, rightNode.impliedNumVars);
  int& numVars= theNode.impliedNumVars;
  theRingZero.Nullify(numVars);
  theRingUnit.MakeOne(numVars);
  List<PolynomialRationalCoeff> theHW;
  WeylGroup& theWeyl=theSSalgebra.theWeyl;
  weight=theWeyl.GetDualCoordinatesFromFundamental(weight);
  std::stringstream out;
  out << "Highest weight in fundamental coords: " << weight.ElementToString() << "<br>";
  theHW.SetSize(weight.size);
  leftElt.owner->OrderSSalgebraForHWbfComputation();
  for (int i=0; i<weight.size; i++)
    theHW[i].MakeNVarConst(numVars, weight[i]);
  if(!leftElt.HWTAAbilinearForm(rightElt, theNode.polyValue.GetElement(), &theHW, theGlobalVariables, theRingUnit, theRingZero, &out))
    return theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);
//  PolynomialRationalCoeff symmTerm;
//  if(!rightElt.HWTAAbilinearForm
//     (leftElt, symmTerm, &theHW, theGlobalVariables, theRingUnit, theRingZero, &out))
//  return theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);
  leftElt.owner->OrderSSLieAlgebraStandard();
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
  Accum.Nullify(*this->owner);
  MonomialUniversalEnveloping<CoefficientType> constMon;
  constMon.MakeConst(theRingUnit, *this->owner);
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
      *logStream << "intermediate after simplification: " << intermediateAccum.ElementToString(theGlobalVariables, tempFormat) << "<br>";
          intermediateAccum.ModOutVermaRelations(&theGlobalVariables, subHiGoesToIthElement, theRingUnit, theRingZero);
    MonomialUniversalEnveloping<CoefficientType>& rightMon=MTright[j];
    int thePower;
    for (int i=rightMon.Powers.size-1; i>=0; i--)
      if (rightMon.Powers[i].IsSmallInteger(thePower))
        for (int k=0; k<thePower; k++)
        { tempElt.MakeOneGenerator(rightMon.generatorsIndices[i], theRingUnit, *this->owner);
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
    intermediateAccum*=rightMon.Coefficient;
    Accum+=intermediateAccum;
    int theIndex= intermediateAccum.IndexOfObjectHash(constMon);
    if (theIndex!=-1)
      output+=intermediateAccum[theIndex].Coefficient;
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
{ if (this->Coefficient.IsEqualToZero())
  { output=theRingZero;
    return true;
  }
  ElementUniversalEnveloping<CoefficientType> tempElt1, tempElt2;
  tempElt1.Nullify(*this->owner);
  tempElt1+=*this;
  tempElt2.Nullify(*this->owner);
  tempElt2+=right;
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
  List<int> oldOrder=this->owner->UEGeneratorOrderIncludingCartanElts;
  int numPosRoots=this->owner->GetNumPosRoots();
  for (int i=0; i<numPosRoots; i++)
    this->owner->UEGeneratorOrderIncludingCartanElts[i]=-1;
  Accum.Nullify(*this->owner);
  MonomialUniversalEnveloping<CoefficientType> constMon;
  constMon.MakeConst(theRingUnit, *this->owner);
  PolynomialOutputFormat tempFormat;
  tempFormat.MakeAlphabetArbitraryWithIndex("g", "h");
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
  for (int j=0; j<TAleft.size; j++)
  { intermediateAccum=startingElt;
    MonomialUniversalEnveloping<CoefficientType>& leftMon=TAleft[j];
    int thePower;
    for (int i=leftMon.Powers.size-1; i>=0; i--)
      if (leftMon.Powers[i].IsSmallInteger(thePower))
        for (int k=0; k<thePower; k++)
        { tempElt.MakeOneGenerator(leftMon.generatorsIndices[i], theRingUnit, *this->owner);
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
        return false;
    intermediateAccum*=leftMon.Coefficient;
    Accum+=intermediateAccum;
    int theIndex= intermediateAccum.IndexOfObjectHash(constMon);
    if (theIndex!=-1)
      output+=intermediateAccum[theIndex].Coefficient;
  }
  if (logStream!=0)
    *logStream << "final UE element: " << Accum.ElementToString(false, theGlobalVariables, tempFormat);
  return true;
}

int ParserNode::EvaluateIsInProperSubmoduleVermaModule
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ root weight;
  ParserNode& ueNode=theNode.owner->TheObjects[theArgumentList[0]];
  ParserNode& weightNode=theNode.owner->TheObjects[theArgumentList[1]];
  if (!weightNode.GetRootRationalDontUseForFunctionArguments(weight, theGlobalVariables))
    return theNode.SetError(theNode.errorBadOrNoArgument);
  SemisimpleLieAlgebra& theSSalgebra=theNode.owner->theHmm.theRange;
  if (weight.size!=theSSalgebra.GetRank())
    return theNode.SetError(theNode.errorDimensionProblem);
  ElementUniversalEnveloping<PolynomialRationalCoeff>& theUE=ueNode.UEElement.GetElement();
  PolynomialRationalCoeff theRingZero, theRingUnit;
  theNode.impliedNumVars=ueNode.impliedNumVars;
  int& numVars= theNode.impliedNumVars;
  theRingZero.Nullify(numVars);
  theRingUnit.MakeOne(numVars);
  List<PolynomialRationalCoeff> theHW;
  WeylGroup& theWeyl=theSSalgebra.theWeyl;
  weight=theWeyl.GetDualCoordinatesFromFundamental(weight);
  std::stringstream out;
  theGlobalVariables.MaxAllowedComputationTimeInSeconds=30;
  out << "Highest weight in dual coords: " << weight.ElementToString() << "<br>";
  theHW.SetSize(weight.size);
  for (int i=0; i<weight.size; i++)
    theHW[i].MakeNVarConst(numVars, weight[i]);
  theUE.owner->OrderSSalgebraForHWbfComputation();
  out << theUE.IsInProperSubmodule(&theHW, theGlobalVariables, theRingUnit, theRingZero);
  theUE.owner->OrderSSLieAlgebraStandard();
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
  int theDim=this->owner->GetRank();
  int numPosRoots=this->owner->GetNumPosRoots();
  theOrbit.AddOnTop(*this);
  for (int i=0; i<theOrbit.size; i++)
    for (int j=0; j<theDim; j++)
    { theElt.MakeOneGenerator(j+numPosRoots+theDim, theRingUnit, *this->owner);
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

int ParserNode::EvaluateSplitIrrepOverLeviParabolic
(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ root theWeightFundCoords, parSel;
  ParserNode& weightNode=theNode.owner->TheObjects[theArgumentList[0]];
  ParserNode& selNode=theNode.owner->TheObjects[theArgumentList[1]];
  WeylGroup& theWeyl=theNode.ContextLieAlgebra->theWeyl;
  int theDim=theNode.owner->theHmm.theRange.GetRank();
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

  theMod.MakeFromHW
  (theWeyl.WeylLetter, theWeyl.GetDim(), theWeightFundCoords, parSel, theGlobalVariables, 1, 0, 0);
  std::string report;
  theMod.SplitOverLevi(& report, parSel, theGlobalVariables, 1, 0);
  out << "<br>" << report;

  theNode.ExpressionType=theNode.typeString;
  theNode.outputString=out.str();
  return theNode.errorNoError;
}

template<class CoefficientType>
void ModuleSSalgebraNew<CoefficientType>::SplitOverLevi
  (std::string* Report, root& parSelection, GlobalVariables& theGlobalVariables, const CoefficientType& theRingUnit,
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
  this->theChaR.SplitCharOverLevi(Report, charWRTsubalgebra, parSelection, subWeyl, theGlobalVariables);
  root theHWsimpleCoords, theHWfundCoords;
  std::stringstream out;
  if(Report!=0)
    out << *Report;
  ModuleSSalgebraNew<Rational>  theIrrep;
  WeylGroup& theWeyl=this->theAlgebra.theWeyl;
  theHWfundCoords=this->theChaR[0].weightFundamentalCoords;
  theHWsimpleCoords=theWeyl.GetSimpleCoordinatesFromFundamental(theHWfundCoords);
  std::string irrepReport;
  root fullPar;
  fullPar.MakeZero(theWeyl.GetDim());
  if (! theIrrep.MakeFromHW(theWeyl.WeylLetter, theWeyl.GetDim(), theHWfundCoords, fullPar, theGlobalVariables, 1, 0, &irrepReport))
  { out << "<hr><b>Failed to create the irreducible representation (most probably the computation requested was too large).</b> "
    << "Below is the report obtained when trying to create the irrep you requested.<br> " << irrepReport;
    if (Report!=0)
      *Report=out.str();
    return;
  }
  Selection parSelSelectedRootsAreInLeviPart;
  parSelSelectedRootsAreInLeviPart=parSelection;
  parSelSelectedRootsAreInLeviPart.InvertSelection();
  if (!parSelSelectedRootsAreInLeviPart.IsSubset(this->parabolicSelectionSelectedAreElementsLevi))
  { out << "The parabolic subalgebra you selected is not a subalgebra of the ambient parabolic subalgebra."
    << " The parabolic has roots of Levi given by " << parSelSelectedRootsAreInLeviPart.ElementToString()
    <<" while the ambient parabolic subalgebra has roots of Levi given by "
    << this->parabolicSelectionSelectedAreElementsLevi.ElementToString();
    if (Report!=0)
      *Report=out.str();
    return;
  }
  out << "<br>Parabolic selection 1 stands for levi part: " << parSelSelectedRootsAreInLeviPart.ElementToString();
  List<List<List<Rational> > > eigenSpacesPerSimpleGenerator;
 // if (false)
  eigenSpacesPerSimpleGenerator.SetSize(parSelSelectedRootsAreInLeviPart.CardinalitySelection);
  roots theFinalEigenSpace, tempSpace1, tempSpace2;
  for (int i=0; i<parSelSelectedRootsAreInLeviPart.CardinalitySelection; i++)
  { int theGenIndex=parSelSelectedRootsAreInLeviPart.elements[i]+theWeyl.GetDim();
    Matrix<Rational>& currentOp=theIrrep.actionsSimpleGensMatrixForM[theGenIndex];
    currentOp.FindZeroEigenSpacE(eigenSpacesPerSimpleGenerator[i], 1, -1, 0, theGlobalVariables);
    if (i==0)
      theFinalEigenSpace.AssignListListRational(eigenSpacesPerSimpleGenerator[i]);
    else
    { tempSpace1=theFinalEigenSpace;
      tempSpace2.AssignListListRational(eigenSpacesPerSimpleGenerator[i]);
      theFinalEigenSpace.IntersectTwoLinSpaces(tempSpace1, tempSpace2, theFinalEigenSpace, theGlobalVariables);
    }
  }
  out << "<br> Eigenvectors: ";
  for (int j=0; j<theFinalEigenSpace.size; j++)
  { out << "<hr>Weight in simple coords: "
    << theIrrep.theGeneratingWordsWeightsSimpleCoords[j].ElementToString() << "<br>Eigenvector:<br>";
    ElementUniversalEnveloping<Rational> currentElt, tempElt;
    currentElt.Nullify(this->theAlgebra);
    root& currentVect= theFinalEigenSpace[j];
    for (int i=0; i<currentVect.size; i++)
    { tempElt=theIrrep.theGeneratingWordsNonReduced[i];
      tempElt*=currentVect[i];
      currentElt+=tempElt;
    }
    if (currentElt.size>1)
      out << "(";
    out << currentElt.ElementToString(false, theGlobalVariables, theFormat);
    if (currentElt.size>1)
      out << ")";
    out << "\\cdot v";
    if (j!=theFinalEigenSpace.size-1)
      out << ", ";
  }
  if (Report!=0)
    *Report=out.str();
}

int ParserNode::EvaluateSplitCharOverLeviParabolic
(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ root parSel;
  ParserNode& charNode=theNode.owner->TheObjects[theArgumentList[0]];
  ParserNode& selNode=theNode.owner->TheObjects[theArgumentList[1]];
//  WeylGroup& theWeyl= theNode.owner->theHmm.theRange.theWeyl;
  int theDim=theNode.owner->theHmm.theRange.GetRank();
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
  theChar.SplitCharOverLevi(&report,  tempChar, parSel, subWeyl, theGlobalVariables);
  out << report;
  theNode.outputString=out.str();
  theNode.ExpressionType=theNode.typeString;
  return theNode.errorNoError;
}

int ParserNode::EvaluateMakeWeylFromParSel
(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ root parSel;
  int theDim=theNode.owner->theHmm.theRange.GetRank();
  if (! theNode.GetRootRationalFromFunctionArguments(parSel, theGlobalVariables))
    return theNode.SetError(theNode.errorBadOrNoArgument);
  if (parSel.size!=theDim)
    return theNode.SetError(theNode.errorDimensionProblem);
  std::stringstream out;
  ReflectionSubgroupWeylGroup tempWeyl;
  Selection tempSel;
  tempSel=parSel;
  tempWeyl.MakeParabolicFromSelectionSimpleRoots
  (theNode.owner->theHmm.theRange.theWeyl, tempSel, theGlobalVariables, 1)
  ;
  tempWeyl.ComputeRootSubsystem();
  out << tempWeyl.ElementToString(false);
  theNode.outputString=out.str();
  theNode.ExpressionType=theNode.typeString;
  return theNode.errorNoError;
}

bool charSSAlgMod::SplitCharOverLevi
(std::string* Report, charSSAlgMod& output, root& parabolicSel, ReflectionSubgroupWeylGroup& outputWeylSub,
 GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  std::string tempS;
  root invertedSel;
  invertedSel.MakeZero(parabolicSel.size);
  for (int i=0; i<parabolicSel.size; i++)
    if (parabolicSel[i].IsEqualToZero())
      invertedSel[i]=1;
  outputWeylSub.MakeParabolicFromSelectionSimpleRoots(this->theBoss->theWeyl, parabolicSel, theGlobalVariables,1);
  outputWeylSub.ComputeRootSubsystem();
//  ReflectionSubgroupWeylGroup complementGroup;
//  complementGroup.MakeParabolicFromSelectionSimpleRoots(this->theBoss->theWeyl, invertedSel, theGlobalVariables,1);
//  complementGroup.ComputeRootSubsystem();
  out << outputWeylSub.ElementToString(false);
  //std::cout << out.str();
  charSSAlgMod dominantCharAmbient, remainingCharDominantLevi;
  this->FreudenthalEvalMe(dominantCharAmbient, tempS, theGlobalVariables, 10000);
  //std::cout << "<hr>" << tempS;
  remainingCharDominantLevi.Nullify(0);
  roots tempRootS;
  HashedList<root> orbitDom, tempHashedRoots;
  MonomialChar<Rational> tempMon, localHighest;
  WeylGroup& theWeyl=this->theBoss->theWeyl;
  for (int i=0; i<dominantCharAmbient.size; i++)
  { tempRootS.size=0;
    tempRootS.AddOnTop(theWeyl.GetSimpleCoordinatesFromFundamental(dominantCharAmbient[i].weightFundamentalCoords));
    if (!theWeyl.GenerateOrbit(tempRootS, false, orbitDom, false, 10000))
    { out << "failed to generate the complement-sub-Weyl-orbit of weight "
      << this->theBoss->theWeyl.GetSimpleCoordinatesFromFundamental
      (dominantCharAmbient[i].weightFundamentalCoords).ElementToString();
      if (Report!=0)
        *Report=out.str();
      return false;
    }
    for (int k=0; k<orbitDom.size; k++)
      if (outputWeylSub.IsDominantWeight(orbitDom[k]))
      { tempMon.Coefficient=dominantCharAmbient[i].Coefficient;
        tempMon.weightFundamentalCoords=theWeyl.GetFundamentalCoordinatesFromSimple(orbitDom[k]);
        remainingCharDominantLevi+=tempMon;
      }
  }
  output.Nullify(0);
  List<Rational> tempMults;
  out << "Character w.r.t Levi part: " << CGI::GetHtmlMathDivFromLatexAddBeginARCL
  (remainingCharDominantLevi.ElementToString("V", "\\omega", false));
  //std::cout << "Character w.r.t Levi part: " << CGI::GetHtmlMathDivFromLatexAddBeginARCL
//  (remainingCharDominantLevi.ElementToString("V", "\\omega", false));
  while(!remainingCharDominantLevi.IsEqualToZero())
  { localHighest=*remainingCharDominantLevi.LastObject();
    for (bool Found=true; Found; )
    { Found=false;
      for (int i=0; i<outputWeylSub.simpleGenerators.size; i++)
      { tempMon=localHighest;
        tempMon.weightFundamentalCoords+=this->theBoss->theWeyl.GetFundamentalCoordinatesFromSimple
        (outputWeylSub.simpleGenerators[i]);
        if (remainingCharDominantLevi.ContainsObjectHash(tempMon))
        { localHighest=tempMon;
          Found=true;
        }
      }
    }
    localHighest=remainingCharDominantLevi[remainingCharDominantLevi.IndexOfObjectHash(localHighest)];
    output+=localHighest;
    if (!outputWeylSub.FreudenthalEvalIrrepIsWRTLeviPart
        (localHighest.weightFundamentalCoords, tempHashedRoots, tempMults, tempS, theGlobalVariables, 10000))
    { if (Report!=0)
        *Report=tempS;
      return false;
    }
    for (int i=0; i<tempHashedRoots.size; i++)
    { tempMon.weightFundamentalCoords=theWeyl.GetFundamentalCoordinatesFromSimple(tempHashedRoots[i]);
      tempMon.Coefficient=tempMults[i]* localHighest.Coefficient;
      remainingCharDominantLevi-=tempMon;
    }
  }
  if (Report!=0)
  { //out << "<hr>"  << "The split character is: " << output.ElementToString("V", "\\omega", false);
    DrawingVariables theDV;
    std::string tempS;
    this->DrawMeNoMults(tempS, theGlobalVariables, theDV, 10000);
    root tempRoot;
    out << "<hr>In the following weight visualization, a yellow line is drawn if the corresponding weights are "
    << " simple reflections of one another, with respect to a simple root of the Levi part of the parabolic subalgebra. ";
    for (int i=0; i<output.size; i++)
    { tempRoot=theWeyl.GetSimpleCoordinatesFromFundamental(output.TheObjects[i].weightFundamentalCoords);
      outputWeylSub.DrawContour
      (tempRoot, theDV, theGlobalVariables, CGI::RedGreenBlue(200, 200, 0), 1000);
      std::stringstream tempStream;
      tempStream << output[i].Coefficient.ElementToString();
      theDV.drawTextAtVectorBuffer(tempRoot, tempStream.str(), 0, DrawingVariables::PenStyleNormal, 0);
    }
    out << "<hr>" << theDV.GetHtmlFromDrawOperationsCreateDivWithUniqueName(theWeyl.GetDim());
    *Report=out.str();
  }
  return true;
}

bool ReflectionSubgroupWeylGroup::IsDominantWeight(root& theWeight)
{ for (int i=0; i<this->simpleGenerators.size; i++)
    if (this->AmbientWeyl.RootScalarCartanRoot(theWeight, this->simpleGenerators[i]).IsNegative())
      return false;
  return true;
}

bool ReflectionSubgroupWeylGroup::GetAlLDominantWeightsHWFDIMwithRespectToAmbientAlgebra
(root& highestWeightSimpleCoords, HashedList<root>& outputWeightsSimpleCoords,
 int upperBoundDominantWeights, std::string& outputDetails, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
//  double startTime=theGlobalVariables.GetElapsedSeconds();
//  std::cout << "<br>time elapsed: " << theGlobalVariables.GetElapsedSeconds()-startTime;
  root highestWeightTrue=highestWeightSimpleCoords;
  roots basisEi;
  int theDim=this->AmbientWeyl.GetDim();
  basisEi.MakeEiBasis(theDim);
  this->RaiseToDominantWeight(highestWeightTrue);
  root highestWeightFundCoords=this->AmbientWeyl.GetFundamentalCoordinatesFromSimple(highestWeightTrue);
  if (!highestWeightFundCoords.SumCoordinates().IsSmallInteger())
    return false;
  int theTopHeightSimpleCoords=(int) highestWeightSimpleCoords.SumCoordinates().DoubleValue()+1;
//  int theTopHeightFundCoords=(int) highestWeightFundCoords.SumCoordinates().DoubleValue();
  if (theTopHeightSimpleCoords<0)
    theTopHeightSimpleCoords=0;
  List<HashedList<root> > outputWeightsByHeight;
  int topHeightRootSystem=this->AmbientWeyl.RootsOfBorel.LastObject()->SumCoordinates().NumShort;
  int topHeightRootSystemPlusOne=topHeightRootSystem+1;
  outputWeightsByHeight.SetSize(topHeightRootSystemPlusOne);
  int finalHashSize=100;
  for (int i=0; i<topHeightRootSystemPlusOne; i++)
    outputWeightsByHeight[i].SetHashSizE(finalHashSize);
  outputWeightsSimpleCoords.ClearTheObjects();
  outputWeightsByHeight[0].AddOnTopHash(highestWeightTrue);
  int numTotalWeightsFound=0;
  int numPosRoots=this->AmbientWeyl.RootsOfBorel.size;
  root currentWeight, currentWeightRaisedToDominantWRTAmbientAlgebra;
//  std::cout << "<br>time spend before working cycle: " << theGlobalVariables.GetElapsedSeconds()-startTime;
  for (int lowestUnexploredHeightDiff=0; lowestUnexploredHeightDiff<=theTopHeightSimpleCoords;
  lowestUnexploredHeightDiff++)
  { //double startCycleTime=theGlobalVariables.GetElapsedSeconds();
    if (upperBoundDominantWeights>0 && numTotalWeightsFound>upperBoundDominantWeights)
      break;
    int bufferIndexShift=lowestUnexploredHeightDiff%topHeightRootSystemPlusOne;
    HashedList<root>& currentHashes=outputWeightsByHeight[bufferIndexShift];
    for (int lowest=0; lowest<currentHashes.size; lowest++)
      for (int i=0; i<numPosRoots; i++)
      { currentWeight=currentHashes[lowest];
        currentWeight-=this->AmbientWeyl.RootsOfBorel[i];
        if (this->IsDominantWeight(currentWeight))
        { currentWeightRaisedToDominantWRTAmbientAlgebra=currentWeight;
          this->AmbientWeyl.RaiseToDominantWeight(currentWeightRaisedToDominantWRTAmbientAlgebra);
          currentWeightRaisedToDominantWRTAmbientAlgebra-=highestWeightTrue;
          if (currentWeightRaisedToDominantWRTAmbientAlgebra.IsNegativeOrZero())
          { int currentIndexShift=this->AmbientWeyl.RootsOfBorel[i].SumCoordinates().NumShort;
            currentIndexShift=(currentIndexShift+bufferIndexShift)%topHeightRootSystemPlusOne;
            if (outputWeightsByHeight[currentIndexShift].AddOnTopNoRepetitionHash(currentWeight))
            { numTotalWeightsFound++;
              outputWeightsByHeight[currentIndexShift].AdjustHashes();
            }
          }
        }
      }
//    std::cout << "<br>time spent before accounting at height level " << lowestUnexploredHeightDiff
//    << ": " << theGlobalVariables.GetElapsedSeconds()-startCycleTime;
//    std::cout << " Size of current level: " << currentHashes.size;
    outputWeightsSimpleCoords.AddOnTopHash(currentHashes);
//    std::cout << ". Time spent after accounting at height level " << lowestUnexploredHeightDiff
//    << ": " << theGlobalVariables.GetElapsedSeconds()-startCycleTime;
//    startCycleTime=theGlobalVariables.GetElapsedSeconds();
    outputWeightsSimpleCoords.AdjustHashes();
    currentHashes.ClearTheObjects();
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
(root& highestWeightSimpleCoords, HashedList<root>& outputWeightsSimpleCoords,
 int upperBoundDominantWeights, std::string& outputDetails, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  this->ComputeRootSubsystem();
//  double startTime=theGlobalVariables.GetElapsedSeconds();
//  std::cout << "<br>time elapsed: " << theGlobalVariables.GetElapsedSeconds()-startTime;
  root highestWeightTrue=highestWeightSimpleCoords;
  roots basisEi;
  int theDim=this->AmbientWeyl.GetDim();
  basisEi.MakeEiBasis(theDim);
  this->RaiseToDominantWeight(highestWeightTrue);
  root highestWeightFundCoords=this->AmbientWeyl.GetFundamentalCoordinatesFromSimple(highestWeightTrue);
  if (!highestWeightFundCoords.SumCoordinates().IsSmallInteger())
    return false;
  int theTopHeightSimpleCoords=(int) highestWeightSimpleCoords.SumCoordinates().DoubleValue()+1;
//  int theTopHeightFundCoords=(int) highestWeightFundCoords.SumCoordinates().DoubleValue();
  if (theTopHeightSimpleCoords<0)
    theTopHeightSimpleCoords=0;
  List<HashedList<root> > outputWeightsByHeight;
  int topHeightRootSystem=this->AmbientWeyl.RootsOfBorel.LastObject()->SumCoordinates().NumShort;
  int topHeightRootSystemPlusOne=topHeightRootSystem+1;
  outputWeightsByHeight.SetSize(topHeightRootSystemPlusOne);
  int finalHashSize=100;
  for (int i=0; i<topHeightRootSystemPlusOne; i++)
    outputWeightsByHeight[i].SetHashSizE(finalHashSize);
  outputWeightsSimpleCoords.ClearTheObjects();
  outputWeightsByHeight[0].AddOnTopHash(highestWeightTrue);
  int numTotalWeightsFound=0;
  root currentWeight, currentWeightRaisedToDominant;
//  std::cout << "<br>time spend before working cycle: " << theGlobalVariables.GetElapsedSeconds()-startTime;
  for (int lowestUnexploredHeightDiff=0; lowestUnexploredHeightDiff<=theTopHeightSimpleCoords;
  lowestUnexploredHeightDiff++)
  { //double startCycleTime=theGlobalVariables.GetElapsedSeconds();
    if (upperBoundDominantWeights>0 && numTotalWeightsFound>upperBoundDominantWeights)
      break;
    int bufferIndexShift=lowestUnexploredHeightDiff%topHeightRootSystemPlusOne;
    HashedList<root>& currentHashes=outputWeightsByHeight[bufferIndexShift];
    for (int lowest=0; lowest<currentHashes.size; lowest++)
      for (int i=0; i<this->RootsOfBorel.size; i++)
      { currentWeight=currentHashes[lowest];
        currentWeight-=this->RootsOfBorel[i];
        if (this->IsDominantWeight(currentWeight))
        { int currentIndexShift=this->RootsOfBorel[i].SumCoordinates().NumShort;
          currentIndexShift=(currentIndexShift+bufferIndexShift)%topHeightRootSystemPlusOne;
          if (outputWeightsByHeight[currentIndexShift].AddOnTopNoRepetitionHash(currentWeight))
          { numTotalWeightsFound++;
            outputWeightsByHeight[currentIndexShift].AdjustHashes();
          }
        }
      }
//    std::cout << "<br>time spent before accounting at height level " << lowestUnexploredHeightDiff
//    << ": " << theGlobalVariables.GetElapsedSeconds()-startCycleTime;
//    std::cout << " Size of current level: " << currentHashes.size;
    outputWeightsSimpleCoords.AddOnTopHash(currentHashes);
//    std::cout << ". Time spent after accounting at height level " << lowestUnexploredHeightDiff
//    << ": " << theGlobalVariables.GetElapsedSeconds()-startCycleTime;
//    startCycleTime=theGlobalVariables.GetElapsedSeconds();
    outputWeightsSimpleCoords.AdjustHashes();
    currentHashes.ClearTheObjects();
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

void ReflectionSubgroupWeylGroup::RaiseToDominantWeight(root& theWeight, int* sign, bool* stabilizerFound)
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
  << "(" << SemisimpleLieAlgebra::GetLieAlgebraName(this->DefaultWeylLetter, this->DefaultWeylRank) << ").</b><br>";
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
    this->theHmm.theRange.ElementToStringNegativeRootSpacesFirst(tempS, false, false, false, true, true, tempFormat, theGlobalVariables);
    lieBracketFile1 << "\\documentclass{article}\\usepackage{longtable}\n\\begin{document}\\pagestyle{empty}\n" << tempS << "\n\\end{document}";
    this->theHmm.theRange.ElementToStringNegativeRootSpacesFirst(tempS, true, false, false, true, true, tempFormat, theGlobalVariables);
    lieBracketFile2 << "\\documentclass{article}\\usepackage{longtable}\\begin{document}\\pagestyle{empty}\n" << tempS << "\n\\end{document}";
    this->theHmm.theRange.ElementToStringNegativeRootSpacesFirst(tempS, true, true, false, true, true, tempFormat, theGlobalVariables);
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
(const root& highestWeightSimpleCoord, DrawingVariables& theDV, GlobalVariables& theGlobalVariables, int theColor,
 int UpperBoundVertices)
{ HashedList<root> theOrbit;
  theOrbit.AddOnTopHash(highestWeightSimpleCoord);
  WeylGroup& theWeyl=this->AmbientWeyl;
  root tempRoot;
  for (int i=0; i<theOrbit.size; i++)
    for (int j=0; j<this->simpleGenerators.size; j++)
    { tempRoot=theOrbit[i];
      theWeyl.ReflectBetaWRTAlpha(this->simpleGenerators[j], tempRoot, false, tempRoot);
      if (theOrbit.AddOnTopNoRepetitionHash(tempRoot))
        theDV.drawLineBetweenTwoVectorsBuffer(theOrbit[i], tempRoot, DrawingVariables::PenStyleNormal, theColor);
      if (theOrbit.size>UpperBoundVertices)
        return false;
    }
  return true;
}

template <class CoefficientType>
bool ElementUniversalEnveloping<CoefficientType>::ConvertToRationalCoeff
(ElementUniversalEnveloping<Rational>& output)
{ output.Nullify(*this->owner);
  MonomialUniversalEnveloping<Rational> tempMon;
  for (int i=0; i<this->size; i++)
  { MonomialUniversalEnveloping<CoefficientType>& currentMon=this->TheObjects[i];
    tempMon.Nullify((Rational)0,*this->owner);
    if (!currentMon.Coefficient.IsAConstant(tempMon.Coefficient))
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

int ParserNode::EvaluateActByUEonEltGenVerma
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ if (theArgumentList.size!=2)
    return theNode.SetError(theNode.errorBadOrNoArgument);
  ParserNode& left=theNode.owner->TheObjects[theArgumentList[0]];
  ParserNode& right=theNode.owner->TheObjects[theArgumentList[1]];
  theNode.impliedNumVars=MathRoutines::Maximum(left.impliedNumVars, right.impliedNumVars);
  if (!right.ConvertToType(theNode.typeGenVermaElt, theNode.impliedNumVars, theGlobalVariables))
    return theNode.SetError(theNode.errorConversionError);
  if (!left.ConvertToType(theNode.typeUEelement, theNode.impliedNumVars, theGlobalVariables))
    return theNode.SetError(theNode.errorConversionError);
  theNode.theGenVermaElt=right.theGenVermaElt;
  ElementUniversalEnveloping<Rational> tempElt;
  if (!left.UEElement.GetElement().ConvertToRationalCoeff(tempElt))
    return theNode.SetError(theNode.errorBadOrNoArgument);
  theNode.theGenVermaElt.GetElement().MultiplyMeByUEEltOnTheLeft(tempElt, theGlobalVariables, 1, 0);
  theNode.ExpressionType=theNode.typeGenVermaElt;
  return theNode.errorNoError;
}

template <class CoefficientType>
void ElementGeneralizedVerma<CoefficientType>::MultiplyMeByUEEltOnTheLeft
  (ElementUniversalEnveloping<CoefficientType>& theUE, GlobalVariables& theGlobalVariables,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{ ElementUniversalEnveloping<CoefficientType> nilPart;
  this->owner->theAlgebra.OrderSetNilradicalNegativeMost(this->owner->parabolicSelectionNonSelectedAreElementsLevi);
  ElementGeneralizedVerma<CoefficientType> output;
  MonomialGeneralizedVerma<CoefficientType> currentMon;
  output.owner=this->owner;
  for (int i=0; i<this->size; i++)
  { currentMon.Coefficient=theUE;
    currentMon.indexFDVector=this->TheObjects[i].indexFDVector;
    currentMon.Coefficient*=this->TheObjects[i].Coefficient;
    currentMon.Coefficient.Simplify(theGlobalVariables, theRingUnit, theRingZero);
    for (int j=0; j<currentMon.Coefficient.size; j++)
      output.ReduceMonAndAddToMe(currentMon.Coefficient[j], currentMon.indexFDVector, theGlobalVariables, theRingUnit, theRingZero);
  }
  *this=output;
  this->owner->theAlgebra.OrderSSLieAlgebraStandard();
}

template <class CoefficientType>
void ElementGeneralizedVerma<CoefficientType>::ReduceMonAndAddToMe
  (MonomialUniversalEnveloping<CoefficientType>& theMon, int theIndexOfFDVector, GlobalVariables& theGlobalVariables,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{ Matrix<CoefficientType> tempMat1, tempMat2;
  if (theMon.Coefficient.IsEqualToZero())
    return;
  tempMat1.MakeIdMatrix(this->owner->theGeneratingWordsWeightsSimpleCoords.size, theRingUnit, theRingZero);
  for (int i=theMon.Powers.size-1; i>=0; i--)
  { int thePower;
    if (!theMon.Powers[i].IsSmallInteger(thePower))
      break;
    int theIndex=theMon.generatorsIndices[i];
    tempMat2=this->owner->GetActionGeneratorIndex(theIndex, theGlobalVariables, theRingUnit, theRingZero);
    if (tempMat2.NumRows==0)
    { if (theIndex>=this->owner->theAlgebra.GetRank()+this->owner->theAlgebra.GetNumPosRoots())
        return;
      break;
    }
    for (int j=0; j<thePower; j++)
      tempMat1.MultiplyOnTheLeft(tempMat2);
    theMon.Powers.size--;
    theMon.generatorsIndices.size--;
  }
  MonomialGeneralizedVerma<CoefficientType> tempMon;
  tempMon.Coefficient.Nullify(*theMon.owner);
  tempMon.owner=this->owner;
  tempMon.Coefficient+=theMon;
  for (int i=0; i<tempMat1.NumRows; i++)
    if (tempMat1.elements[theIndexOfFDVector][i]!=0)
    { tempMon.indexFDVector=i;
      tempMon.Coefficient*=tempMat1.elements[theIndexOfFDVector][i];
      this->operator+=(tempMon);
      tempMon.Coefficient/=tempMat1.elements[theIndexOfFDVector][i];

    }
}

template  <class CoefficientType>
Matrix<CoefficientType>& ModuleSSalgebraNew<CoefficientType>::GetActionGeneratorIndex
(int generatorIndex, GlobalVariables& theGlobalVariables,
 const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{ int numGenerators=this->theAlgebra.GetNumGenerators();
  int theDim=this->theAlgebra.GetRank();
  int numPosRoots=this->theAlgebra.GetNumPosRoots();
  assert(generatorIndex>=0 && generatorIndex<numGenerators);
  if (this->ComputedGeneratorActions.selected[generatorIndex])
    return this->actionsGeneratorsMaT[generatorIndex];
  this->ComputedGeneratorActions.AddSelectionAppendNewIndex(generatorIndex);
  ElementUniversalEnveloping<CoefficientType> tempElt;
  root theWeight;
  tempElt.MakeOneGenerator(generatorIndex, 1, this->theAlgebra);
  theWeight.MakeZero(theDim);
  if (generatorIndex<numPosRoots)
    theWeight=this->theAlgebra.theWeyl.RootSystem[generatorIndex];
  if(generatorIndex>=numPosRoots+theDim)
    theWeight=this->theAlgebra.theWeyl.RootSystem[generatorIndex-theDim];
  for (int i=0; i<this->parabolicSelectionNonSelectedAreElementsLevi.CardinalitySelection; i++)
    if (theWeight[this->parabolicSelectionNonSelectedAreElementsLevi.elements[i]]!=0)
    { this->actionsGeneratorsMaT[generatorIndex].init(0,0);
      return this->actionsGeneratorsMaT[generatorIndex];
    }
  this->GetMatrixHomogenousElt
  (tempElt, this->actionsGeneratorS[generatorIndex], theWeight,
   this->actionsGeneratorsMaT[generatorIndex], theGlobalVariables, theRingUnit, theRingZero);
  return this->actionsGeneratorsMaT[generatorIndex];
}

template  <class CoefficientType>
void ModuleSSalgebraNew<CoefficientType>::
 GetMatrixHomogenousElt
  (ElementUniversalEnveloping<CoefficientType>& inputHomogeneous,
   List<List<ElementUniversalEnveloping<CoefficientType> > >& outputSortedByArgumentWeight,
    root& weightUEEltSimpleCoords, Matrix<CoefficientType>& output,
   GlobalVariables& theGlobalVariables, const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{ this->GetAdActionHomogenousElT
  (inputHomogeneous, weightUEEltSimpleCoords, outputSortedByArgumentWeight, theGlobalVariables, theRingUnit, theRingZero)
  ;
  output.init(this->theGeneratingWordsNonReduced.size, this->theGeneratingWordsNonReduced.size);
  output.NullifyAll(theRingZero);
  for (int j=0; j<outputSortedByArgumentWeight.size; j++)
    for (int k=0; k<outputSortedByArgumentWeight[j].size; k++)
    { MonomialUniversalEnveloping<CoefficientType>& currentMon=this->theGeneratingWordsGrouppedByWeight[j][k];
      ElementUniversalEnveloping<CoefficientType>& imageCurrentMon=outputSortedByArgumentWeight[j][k];
      int indexColumn=this->theGeneratingWordsNonReduced.IndexOfObjectHash(currentMon);
      assert(indexColumn!=-1);
      for (int l=0; l< imageCurrentMon.size; l++)
      { int indexRow=this->theGeneratingWordsNonReduced.IndexOfObjectHash(imageCurrentMon[l]);
        assert(indexRow!=-1);
        output.elements[indexRow][indexColumn]=imageCurrentMon[l].Coefficient;
//        std::cout <<"<br> Index row: " << indexRow << "; index column: " << indexColumn;
      }
    }
}

void charSSAlgMod::DrawMeAssumeCharIsOverCartan
(WeylGroup& actualAmbientWeyl, GlobalVariables& theGlobalVariables, DrawingVariables& theDrawingVars)
{ root actualWeight;
  actualAmbientWeyl.DrawRootSystem(theDrawingVars, true, theGlobalVariables, false, 0, false);
  for (int j=0; j<this->size; j++)
  { actualWeight=actualAmbientWeyl.GetSimpleCoordinatesFromFundamental(this->TheObjects[j].weightFundamentalCoords);
    theDrawingVars.drawCircleAtVectorBuffer(actualWeight, 5, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(0,0,0));
    theDrawingVars.drawTextAtVectorBuffer
          (actualWeight, this->TheObjects[j].Coefficient.ElementToString(), CGI::RedGreenBlue(0,0,0), theDrawingVars.PenStyleNormal, 0);
  }
}

template <class CoefficientType>
void ElementGeneralizedVerma<CoefficientType>::MakeHWV
  (ModuleSSalgebraNew<CoefficientType>& theOwner, const CoefficientType& theRingUnit)
{ this->owner=&theOwner;
  MonomialGeneralizedVerma<CoefficientType> theMon;
  theMon.indexFDVector=this->owner->theGeneratingWordsWeightsSimpleCoords.size-1;
  theMon.Coefficient.MakeConst(theRingUnit, this->owner->theAlgebra);
  this->ClearTheObjects();
  this->AddOnTopHash(theMon);
}
