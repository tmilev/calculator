#include "polyhedra.h"

template < > int HashedList<LittelmannPath >::PreferredHashSize=1000;
template < > int List<LittelmannPath>::ListActualSizeIncrement=1000;
template < > int List<MonomialUniversalEnvelopingOrdered<Rational> >::ListActualSizeIncrement=100;
template < > int HashedList<MonomialUniversalEnvelopingOrdered<Rational> >::PreferredHashSize=100;
template < > int List<ElementUniversalEnvelopingOrdered<Rational> >::ListActualSizeIncrement=100;

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

template <class CoefficientType>
class ElementModuleSSalgebra;

template <class CoefficientType>
class ModuleSSalgebraNew
{
public:
  SemisimpleLieAlgebra theAlgebra;
  List<MonomialUniversalEnveloping<CoefficientType> > theGeneratingWordsNonReduced;
  List<List<MonomialUniversalEnveloping<CoefficientType> > > theGeneratingWordsGrouppedByWeight;
  List<ElementUniversalEnveloping<CoefficientType> > theSimpleGens;
  List<List<List<ElementUniversalEnveloping<CoefficientType> > > > actionsSimpleGens;
  List<Matrix<CoefficientType> > theBilinearFormsAtEachWeightLevel;
  List<Matrix<CoefficientType> > theBilinearFormsInverted;
  roots weightsSimpleGens;
//  List<List<Matrix<CoefficientType> > >
  HashedList<root> generatingWordsWeights;
//  List<ElementUniversalEnveloping<CoefficientType> > theGeneratingWordsLittelmannForm;
//  HashedList<MonomialUniversalEnveloping<CoefficientType> > theGeneratingMonsPBWform;
//  List
//  List<Matrix<CoefficientType> > ActionsChevalleyGenerators;
  void MakeFundamentalRep
(char WeylLetter, int theRank, GlobalVariables& theGlobalVariables)
  ;
  void IntermediateStepForMakeFromHW
    ( List<Rational>& theHWDualCoords, GlobalVariables& theGlobalVariables,
    const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
  ;
  bool MakeFromHW
(char WeylLetter, int theRank, root& theHWFundCoords, GlobalVariables& theGlobalVariables,
const CoefficientType& theRingUnit, const CoefficientType& theRingZero,
 std::string* outputReport)
  ;
  void GetAdActionHomogenousElt
  (ElementUniversalEnveloping<CoefficientType>& inputHomogeneous, root& inputWeight,
   List<List<ElementUniversalEnveloping<CoefficientType> > >& outputSortedByArgumentWeight,
   const List<CoefficientType>& subHiGoesToIthElement,
   GlobalVariables& theGlobalVariables, const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
   ;
  void ExpressAsLinearCombinationHomogenousElement
  (ElementUniversalEnveloping<CoefficientType>& inputHomogeneous,
   ElementUniversalEnveloping<CoefficientType>& outputHomogeneous,
   int indexInputBasis,
   const List<CoefficientType>& subHiGoesToIthElement,
   GlobalVariables& theGlobalVariables, const CoefficientType& theRingUnit, const CoefficientType& theRingZero
   )
  ;
  void Reduce
  (ElementModuleSSalgebra<CoefficientType>& inputOutput, GlobalVariables& theGlobalVariables)
  ;
};

template <class CoefficientType>
class ElementModuleSSalgebra
{
public:
  ModuleSSalgebraNew<CoefficientType>* owner;
  ElementUniversalEnvelopingOrdered<CoefficientType> theWord;
};

template < > int List<Matrix<Qsqrt2> >::ListActualSizeIncrement=100;
template < > int List<Vector<Qsqrt2> >::ListActualSizeIncrement=100;
template < > int HashedList<MonomialUniversalEnveloping<Qsqrt2> >::PreferredHashSize=100;

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
  Layers.MakeActualSizeAtLeastExpandOnTop(this->RepresentativesQuotientAmbientOrder.size);
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
//  orbit.MakeActualSizeAtLeastExpandOnTop(this->RepresentativesQuotientAmbientOrder.size);
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
  this->RepresentativesQuotientAmbientOrder.MakeActualSizeAtLeastExpandOnTop(this->AmbientWeyl.size);
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
  output.MakeFromWeight(theWeight, &theNode.owner->theHmm.theRange);
  theNode.ExpressionType=theNode.typeCharSSFDMod;
  return theNode.errorNoError;
}

void charSSAlgMod::MakeFromWeight(Vector<Rational>& inputWeightSimpleCoords, SemisimpleLieAlgebra* owner)
{ this->Nullify(owner);
  assert(inputWeightSimpleCoords.size=this->theBoss->GetRank());
  MonomialChar<Rational> theMon;
  theMon.weightFundamentalCoords=inputWeightSimpleCoords;
  theMon.Coefficient=1;
  this->AddOnTopHash(theMon);
}

template <class CoefficientType>
std::string MonomialChar<CoefficientType>::TensorAndDecompose
(MonomialChar<CoefficientType>& other, SemisimpleLieAlgebra& owner, charSSAlgMod& output,
 GlobalVariables& theGlobalVariables)
{ std::stringstream errorLog;
  std::string tempS, tempS2;
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
  if (!theWeyl.FreudenthalEval(leftHWFundCoords, weightsLeftSimpleCoords, multsLeft, tempS, tempS2, theGlobalVariables))
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
{ this->MakeActualSizeAtLeastExpandOnTop(other.size+this->size);
  if ( (other.size+this->size)/this->HashSize>5)
    this->SetHashSizE(other.size+this->size);
  for (int i=0; i<other.size; i++)
    this->operator+=(other[i]);
}

void charSSAlgMod::operator-=(const charSSAlgMod& other)
{ this->MakeActualSizeAtLeastExpandOnTop(other.size+this->size);
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
  this->MakeActualSizeAtLeastExpandOnTop(other.size+this->size);
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

void charSSAlgMod::MakeTrivial(SemisimpleLieAlgebra* owner)
{ this->Nullify(owner);
  MonomialChar<Rational> tempMon;
  tempMon.Coefficient=1;
  tempMon.weightFundamentalCoords.MakeZero(owner->GetRank());
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
  roots basisEi;
  basisEi.MakeEiBasis(this->GetDim());
  this->RaiseToDominantWeight(highestWeightTrue);
  root highestWeightFundCoords=this->GetFundamentalCoordinatesFromSimple(highestWeightTrue);
  if (!highestWeightFundCoords.SumCoordinates().IsSmallInteger())
    return false;
  int theTopHeightSimpleCoords=(int) highestWeightSimpleCoords.SumCoordinates().DoubleValue()+1;
//  int theTopHeightFundCoords=(int) highestWeightFundCoords.SumCoordinates().DoubleValue();
  if (theTopHeightSimpleCoords<0)
    theTopHeightSimpleCoords=0;
  List<HashedList<root> > outputWeightsByHeight;
  int topHeightRootSystem=this->RootsOfBorel.LastObject()->SumCoordinates().NumShort;
  int topHeightRootSystemPlusOne=topHeightRootSystem+1;
  outputWeightsByHeight.SetSize(topHeightRootSystemPlusOne);

//  Rational UBDWeightsRat=this->EstimateNumDominantWeightsBelow(highestWeightTrue, theGlobalVariables);
//  out << "Computed a priori bound for the number of dominant weights " << UBDWeightsRat.ElementToString();
//  std::cout << "Computed a priori bound for the number of dominant weights " << UBDWeightsRat.ElementToString();
//  if (!UBDWeightsRat.IsInteger())
//  { out << " (~" << UBDWeightsRat.DoubleValue() << ")";
//    std::cout << " (~" << UBDWeightsRat.DoubleValue() << ")";
//  }
//  out << " (the bound is used to preallocate RAM to minimize the memory allocation routines called).";
//  std::cout << " (the bound is used to preallocate RAM to minimize the memory allocation routines called).";
//  assert((int)UBDWeightsRat.DoubleValue()>=1);
//  upperBoundDominantWeights=
//  MathRoutines::Minimum(upperBoundDominantWeights, (int)UBDWeightsRat.DoubleValue()+1);
  int finalHashSize=100;//MathRoutines::Maximum  (100, (int)UBDWeightsRat.DoubleValue() /(theTopHeightSimpleCoords+1));

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

void WeylGroup::RaiseToDominantWeight
  (root& theWeight, int* sign, bool* stabilizerFound)
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

bool WeylGroup::FreudenthalEval
  (root& inputHWfundamentalCoords, HashedList<root>& outputDominantWeightsSimpleCoords,
   List<Rational>& outputMultsSimpleCoords, std::string& errorMessage, std::string& outputDetails, GlobalVariables& theGlobalVariables)
{ //double startTimer=theGlobalVariables.GetElapsedSeconds();
  this->ComputeRho(true);
  roots EiBasis;
  EiBasis.MakeEiBasis(this->GetDim());
  List<bool> Explored;
  root hwSimpleCoords=this->GetSimpleCoordinatesFromFundamental(inputHWfundamentalCoords);
  const int UpperBoundFreudenthal=10000000;
  if (!this->GetAlLDominantWeightsHWFDIM
      (hwSimpleCoords, outputDominantWeightsSimpleCoords, UpperBoundFreudenthal, outputDetails, theGlobalVariables))
  { std::stringstream errorLog;
    errorLog << "Error: the number of dominant weights exceeded hard-coded limit of "
    << UpperBoundFreudenthal << ". Please check out whether LiE's implementation of the Freudenthal "
    << " formula can do your computation.";
    errorMessage=errorLog.str();
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
          errorMessage=errorLog.str();
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
  theMod.MakeActualSizeAtLeastExpandOnTop(theWeights.size);
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
 (charSSAlgMod& outputCharOwnerSetToZero, std::string& errorMessage, std::string& outputDetails,
  GlobalVariables& theGlobalVariables)
{ assert(&outputCharOwnerSetToZero!=this);
  outputCharOwnerSetToZero.Nullify(0);
  root currentWeightFundCoords;
  List<Rational> currentMults;
  HashedList<root> currentWeights;
  std::stringstream localErrors, localDetails;
  std::string localError, localDetail;
  errorMessage="";
  MonomialChar<Rational> tempMon;
  for (int i=0; i<this->size; i++)
  { currentWeightFundCoords=this->TheObjects[i].weightFundamentalCoords;
    if (!this->theBoss->theWeyl.FreudenthalEval
    (currentWeightFundCoords, currentWeights, currentMults, localError, localDetail, theGlobalVariables))
    { localErrors << "Encountered error while evaluating freudenthal formula. Error details: " << localError;
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

int ParserNode::EvaluateFreudenthal
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  out << "Freudenthal formula: ";
  charSSAlgMod& ch=theNode.owner->TheObjects[theArgumentList[0]].theChar.GetElement();
  charSSAlgMod finalChar;
  std::string localErrorString, localDetailsString;
  if (!ch.FreudenthalEvalMe(finalChar, localErrorString, localDetailsString, theGlobalVariables))
    out << "<br>" << localErrorString;
  else
  { out << "resulting character in fundamental coordinates: <br>";
    out << CGI::GetHtmlMathSpanFromLatexFormulaAddBeginArrayRCL(finalChar.ElementToString("V", "\\omega", false));
  }
  out << "<br>" << localDetailsString;
  theNode.outputString=out.str();
  theNode.ExpressionType=theNode.typeString;
  return theNode.errorNoError;
}

int ParserNode::EvaluateHWV
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  out << "Function not programmed yet. ";
  theNode.outputString=out.str();
  theNode.ExpressionType=theNode.typeString;
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
      << "), hence the vector partition function is "
      << "can only take values infinity or zero. ";
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
  output.MakeActualSizeAtLeastExpandOnTop(this->Waypoints.size);
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
(List<LittelmannPath>& output, List<List<int> >& outputOperators,
 GlobalVariables& theGlobalVariables, int UpperBoundNumElts)
{ HashedList<LittelmannPath> hashedOutput;
  hashedOutput.AddOnTopHash(*this);
  int theDim=this->owner->GetDim();
  outputOperators.SetSize(1);
  outputOperators[0].SetSize(0);
  List<int> currentSequence;
  if (UpperBoundNumElts>0)
    currentSequence.MakeActualSizeAtLeastExpandOnTop(UpperBoundNumElts);
  LittelmannPath currentPath;
  bool result=true;
  for (int lowestNonExplored=0; lowestNonExplored<hashedOutput.size; lowestNonExplored++)
    if (UpperBoundNumElts>0 && UpperBoundNumElts< hashedOutput.size)
    { result=false;
      break;
    }
    else
      for (int i=0; i<theDim; i++)
      { bool found=true;
        currentPath=hashedOutput[lowestNonExplored];
        currentSequence=outputOperators[lowestNonExplored];
        while (found)
        { found=false;
          currentPath.ActByEalpha(i);
          if (!currentPath.IsEqualToZero())
  //          hashedOutput.AddOnTopNoRepetitionHash(currentPath);
            if (hashedOutput.AddOnTopNoRepetitionHash(currentPath))
            { found=true;
              currentSequence.AddOnTop(i);
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
          currentPath.ActByFalpha(i);
          if (!currentPath.IsEqualToZero())
            //hashedOutput.AddOnTopNoRepetitionHash(currentPath);
            if (hashedOutput.AddOnTopNoRepetitionHash(currentPath))
            {  found=true;
              currentSequence.AddOnTop(-i-1);
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
  if (!theModule.MakeFromHW
      (theWeyl.WeylLetter, theWeyl.GetDim(), theWeight, theGlobalVariables, 1, 0, &report))
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
  ( List<Rational>& theHWDualCoords, GlobalVariables& theGlobalVariables,
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
void ModuleSSalgebraNew<CoefficientType>::GetAdActionHomogenousElt
( ElementUniversalEnveloping<CoefficientType>& inputHomogeneous, root& inputWeight,
  List<List<ElementUniversalEnveloping<CoefficientType> > >& outputSortedByArgumentWeight,
  const List<CoefficientType>& subHiGoesToIthElement,
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
    root& currentWeight=this->generatingWordsWeights[i];
    targetWeight=currentWeight+inputWeight;
//    std::cout << "<br>target weight: " << targetWeight.ElementToString() << "="
//    << currentWeight.ElementToString() << "+" << inputWeight.ElementToString();
    int theIndex=this->generatingWordsWeights.IndexOfObjectHash(targetWeight);
    for (int j=0; j<currentWordList.size; j++)
    { ElementUniversalEnveloping<CoefficientType>& currentOutputWord=outputCurrentList[j];
      if (theIndex==-1)
        currentOutputWord.Nullify(this->theAlgebra);
      else
      { theElt=inputHomogeneous;
        theElt*=(currentWordList[j]);
        this->ExpressAsLinearCombinationHomogenousElement
        (theElt, currentOutputWord, theIndex, subHiGoesToIthElement, theGlobalVariables, theRingUnit, theRingZero);
      }
    }
  }
}

template<class CoefficientType>
bool ModuleSSalgebraNew<CoefficientType>::MakeFromHW
(char WeylLetter, int theRank, root& theHWFundCoords, GlobalVariables& theGlobalVariables,
const CoefficientType& theRingUnit, const CoefficientType& theRingZero,
 std::string* outputReport)
{ this->theAlgebra.ComputeChevalleyConstants(WeylLetter, theRank, theGlobalVariables);
  LittelmannPath startingPath;
  WeylGroup& theWeyl=this->theAlgebra.theWeyl;
  root theHWsimpleCoords=theWeyl.GetSimpleCoordinatesFromFundamental(theHWFundCoords);
  startingPath.MakeFromWeightInSimpleCoords(theHWsimpleCoords, theWeyl);
  List<LittelmannPath> thePaths;
  List<List<int> > generatorsIndices;
  if (!startingPath.GenerateOrbit(thePaths, generatorsIndices, theGlobalVariables, 1000))
  { if (outputReport!=0)
      *outputReport = "Error: number of Littelmann paths exceeded allowed limit of 1000.";
    return false;
  }
  std::stringstream out;
  if (outputReport!=0)
    out << "Number of Littelmann paths: " << thePaths.size;
  out << "<br>Let v denote the highest weight vector of highest weight in simple coordinates "
  << theHWsimpleCoords.ElementToString();
  out << "<br>Then the elements corresponding to the Littelmann paths are as follows. ";
  this->theGeneratingWordsNonReduced.SetSize(thePaths.size);
//  ElementUniversalEnvelopingOrdered<CoefficientType> theElt;
  ElementUniversalEnveloping<CoefficientType> tempElt;
  PolynomialOutputFormat tempFormat;
  tempFormat.MakeAlphabetArbitraryWithIndex("g", "h");
  List<Rational> theHWDualCoords= theWeyl.GetDualCoordinatesFromFundamental(theHWFundCoords);
//  this->theGeneratingWordsLittelmannForm.SetSize(theGeneratingWordsNonReduced.size);
  this->theGeneratingWordsNonReduced.SetSize(theGeneratingWordsNonReduced.size);
  roots tempRoots;
  theWeyl.GenerateWeightSupportMethoD1
  (theHWsimpleCoords, tempRoots, 10000, theGlobalVariables);
  tempRoots.QuickSortAscending();
  this->generatingWordsWeights.AssignList(tempRoots);
  this->theGeneratingWordsGrouppedByWeight.SetSize(this->generatingWordsWeights.size);
  for (int i=0; i<this->theGeneratingWordsGrouppedByWeight.size; i++)
    this->theGeneratingWordsGrouppedByWeight[i].size=0;
  for (int i=0; i<theGeneratingWordsNonReduced.size; i++)
  { List<int>& currentPath= generatorsIndices[i];
    MonomialUniversalEnveloping<CoefficientType>& currentElt = this->theGeneratingWordsNonReduced[i];
//    ElementUniversalEnveloping<CoefficientType>& currentReducedElt = this->theGeneratingWordsLittelmannForm[i];
    currentElt.MakeConst(1, this->theAlgebra);
    for (int j=currentPath.size-1; j>=0; j--)
    { int theIndex=currentPath[j];
      if (theIndex>0)
        theIndex++;
      currentElt.MultiplyByGeneratorPowerOnTheRight
      (this->theAlgebra.DisplayIndexToChevalleyGeneratorIndex(theIndex),1);
    }
    out << "<br>m_{ " << i+1 << "} := "
    << currentElt.ElementToString(false, false, theGlobalVariables, tempFormat) << " \\cdot v ";
    root& hwCurrent=*thePaths[i].Waypoints.LastObject();
    int theIndex=this->generatingWordsWeights.IndexOfObjectHash(hwCurrent);
    if (theIndex==-1)
    { out << "Error: could not generate all weights in the weight support. Maybe they are too many? Allowed "
      << " weight # limit is 10000";
      if (outputReport!=0)
        *outputReport=out.str();
      return false;
    }
    this->theGeneratingWordsGrouppedByWeight[theIndex].AddOnTop(currentElt);
//    currentReducedElt=currentElt;
//    currentReducedElt.Simplify(theGlobalVariables, theRingUnit, theRingZero);
//    out << currentElt.ElementToString(tempFormat) << " \\cdot v = ";
//    std::cout  << "<hr><hr>Element " << i+1 << " here: ";
//    currentReducedElt.ModToMinDegreeFormFDRels
//    (theHWsimpleCoords, theGlobalVariables, 1, 0)
//    ;
//    out << currentReducedElt.ElementToStringCalculatorFormat(theGlobalVariables, tempFormat);
//    int numNewFoundMons=0;
//    for (int l=0; l<currentReducedElt.size; l++)
//      if (this->theGeneratingMonsPBWform.AddOnTopNoRepetitionHash(currentReducedElt[l]))
//        numNewFoundMons++;
//    if (numNewFoundMons>1)
//      std::cout << "<hr>Houston, we don't have problem @ element " << i+1 << ": "
//      << currentElt.ElementToStringCalculatorFormat(theGlobalVariables, tempFormat);
  }
  for (int i=0; i<this->theGeneratingWordsGrouppedByWeight.size; i++)
  { List<MonomialUniversalEnveloping<CoefficientType> >& currentList= this->theGeneratingWordsGrouppedByWeight[i];
    currentList.QuickSortAscending();
  }
  this->IntermediateStepForMakeFromHW(theHWDualCoords, theGlobalVariables, theRingUnit, theRingZero);
  if (outputReport!=0)
    for (int i=0; i<this->theBilinearFormsAtEachWeightLevel.size; i++)
    { Matrix<CoefficientType>& theBF=this->theBilinearFormsAtEachWeightLevel[i];
      Matrix<CoefficientType>& theBFinverted= this->theBilinearFormsInverted[i];
      out << "<hr>weight " << this->generatingWordsWeights[i].ElementToString();
      List<MonomialUniversalEnveloping<CoefficientType> >& currentList=this->theGeneratingWordsGrouppedByWeight[i];
      for (int i=0; i<currentList.size; i++)
      { MonomialUniversalEnveloping<CoefficientType>& currentElt=currentList[i];
        out << "<br>mon " << i+1 << ": " << currentElt.ElementToString(false, false, theGlobalVariables, tempFormat);
      }
      out << "; corresponding form:<br> " << theBF.ElementToString(true, false)
      << " corresonding inverted matrix:<br>";
      if (theBFinverted.NumRows>0)
        out << theBFinverted.ElementToString(true, false);
      else
        out << "<b>The matrix of the bilinear form is not invertible!</b>";
    }
  this->actionsSimpleGens.SetSize(this->theAlgebra.GetRank()*2);
  for (int i=0; i<this->theSimpleGens.size; i++)
  { ElementUniversalEnveloping<Rational>& theSimpleGenerator=this->theSimpleGens[i];
    root& simpleWeight=this->weightsSimpleGens[i];
    this->GetAdActionHomogenousElt
    (theSimpleGenerator, simpleWeight, this->actionsSimpleGens[i], theHWDualCoords, theGlobalVariables,
     theRingUnit, theRingZero);
    out << "<hr>Simple generator: " << theSimpleGenerator.ElementToString(theGlobalVariables, tempFormat);
    for (int j=0; j<this->actionsSimpleGens[i].size; j++)
      for (int k=0; k<this->actionsSimpleGens[i][j].size; k++)
      { out << "<br>" << theSimpleGenerator.ElementToString(theGlobalVariables, tempFormat) << "\\cdot "
        << this->theGeneratingWordsGrouppedByWeight[j][k].ElementToString(false, false, theGlobalVariables, tempFormat)
        << "\\cdot v=" << this->actionsSimpleGens[i][j][k].ElementToString(theGlobalVariables, tempFormat)
        << "\\cdot v"
        ;
      }
  }
  if (outputReport!=0)
    *outputReport=out.str();
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
  (GlobalVariables* theContext, const root& theHWsimpleCoords,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
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
  (GlobalVariables* theContext, const root& theHWsimpleCoords,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
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
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero,
   GlobalVariables& theGlobalVariables)const
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
  (List<ElementUniversalEnveloping<CoefficientType> >& theElements,
   Vectors<CoefficientTypeQuotientField>& outputCoords,
   List<ElementUniversalEnveloping<CoefficientType> >& outputTheBasis,
   const CoefficientTypeQuotientField& theFieldUnit, const CoefficientTypeQuotientField& theFieldZero,
   GlobalVariables& theGlobalVariables)
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
  outputTheBasis.MakeActualSizeAtLeastExpandOnTop(theElements.size);
  Vectors<CoefficientTypeQuotientField> basisCoordForm;
  basisCoordForm.MakeActualSizeAtLeastExpandOnTop(theElements.size);
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
  (GlobalVariables* theContext, const root& theHWsimpleCoords,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
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
  (GlobalVariables* theContext, const List<CoefficientType>* subHiGoesToIthElement,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
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
  leftElt.PrepareOrderSSalgebraForHWbfComputation();
  if(!leftElt.HWMTAbilinearForm
     (rightElt, theNode.polyValue.GetElement(), &theHW, theGlobalVariables, theRingUnit, theRingZero, &out))
    return theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);
  PolynomialRationalCoeff symmTerm;
  if(!rightElt.HWMTAbilinearForm
     (leftElt, symmTerm, &theHW, theGlobalVariables, theRingUnit, theRingZero, &out))
    return theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);
  leftElt.RestoreOrderSSLieAlgebra();
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
  leftElt.PrepareOrderSSalgebraForHWbfComputation();
  for (int i=0; i<weight.size; i++)
    theHW[i].MakeNVarConst(numVars, weight[i]);
  if(!leftElt.HWTAAbilinearForm(rightElt, theNode.polyValue.GetElement(), &theHW, theGlobalVariables, theRingUnit, theRingZero, &out))
    return theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);
//  PolynomialRationalCoeff symmTerm;
//  if(!rightElt.HWTAAbilinearForm
//     (leftElt, symmTerm, &theHW, theGlobalVariables, theRingUnit, theRingZero, &out))
//  return theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);
  leftElt.RestoreOrderSSLieAlgebra();
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
  theUE.PrepareOrderSSalgebraForHWbfComputation();
  out << theUE.IsInProperSubmodule(&theHW, theGlobalVariables, theRingUnit, theRingZero);
  theUE.RestoreOrderSSLieAlgebra();
  theNode.ExpressionType=theNode.typeString;
  theNode.outputString=out.str();
  return theNode.errorNoError;
}

template <class CoefficientType>
std::string ElementUniversalEnveloping<CoefficientType>::IsInProperSubmodule
(const List<CoefficientType>* subHiGoesToIthElement, GlobalVariables& theGlobalVariables,
  const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{ std::stringstream out;
  List<ElementUniversalEnveloping<CoefficientType> > theOrbit;
  theOrbit.MakeActualSizeAtLeastExpandOnTop(1000);
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
{ root theWeight, parSel;
  ParserNode& weightNode=theNode.owner->TheObjects[theArgumentList[0]];
  ParserNode& selNode=theNode.owner->TheObjects[theArgumentList[1]];
  int theDim=theNode.owner->theHmm.theRange.GetRank();
  if (! weightNode.GetRootRationalDontUseForFunctionArguments(theWeight, theGlobalVariables))
    return theNode.SetError(theNode.errorBadOrNoArgument);
  if (! selNode.GetRootRationalDontUseForFunctionArguments(parSel, theGlobalVariables))
    return theNode.SetError(theNode.errorBadOrNoArgument);
  if (theWeight.size!=theDim || parSel.size!=theDim)
    return theNode.SetError(theNode.errorDimensionProblem);
  std::stringstream out;
  out << "Not implemented yet. Your input weight: " << theWeight.ElementToString();
  out << "; your parabolic subalgebra selection: " << parSel.ElementToString() << ".";
  theNode.ExpressionType=theNode.typeString;
  return theNode.errorNoError;
}
