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

template <>
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

template <>
void WeylGroup::RaiseToDominantWeight(Vector<RationalFunction>& theWeight, int* sign, bool* stabilizerFound)
{ if (sign!=0)
    *sign=1;
  if (stabilizerFound!=0)
    *stabilizerFound=false;
  RationalFunction theScalarProd;
  int theDim=this->GetDim();
  for (bool found = true; found; )
  { found=false;
    for (int i=0; i<theDim; i++)
    { theScalarProd=this->GetScalarProdSimpleRoot(theWeight, i);
      if (theScalarProd.expressionType==theScalarProd.typeRational)
        if (theScalarProd.ratValue.IsNegative())
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
  charSSAlgMod<Rational>& output=theNode.theChar.GetElement();

  output.MakeFromWeight
  (theNode.owner->theHmm.theRange().theWeyl.GetSimpleCoordinatesFromFundamental(theWeight),
   theNode.owner->theAlgebras, 0);
  theNode.ExpressionType=theNode.typeCharSSFDMod;
  return theNode.errorNoError;
}

template <class CoefficientType>
void charSSAlgMod<CoefficientType>::MakeFromWeight
(const Vector<CoefficientType>& inputWeightSimpleCoords, List<SemisimpleLieAlgebra>& inputOwners,
 int inputIndexInOwner)
{ this->MakeZero(inputOwners, inputIndexInOwner);
  assert(inputWeightSimpleCoords.size==this->listOwners->TheObjects[this->indexInOwners].GetRank());
  MonomialChar<CoefficientType> theMon;
  theMon.weightFundamentalCoords=
  this->listOwners->TheObjects[this->indexInOwners].theWeyl.GetFundamentalCoordinatesFromSimple(inputWeightSimpleCoords);
  this->AddMonomial(theMon,1);
}

template <class CoefficientType>
std::string MonomialChar<CoefficientType>::TensorAndDecompose
(MonomialChar<CoefficientType>& other, List<SemisimpleLieAlgebra>& inputOwners, int inputIndexInOwners,
 charSSAlgMod<CoefficientType>& output, GlobalVariables& theGlobalVariables)
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

  Rational leftTotalDim= theWeyl.WeylDimFormulaFundamentalCoords(leftHWFundCoords);
  Rational rightTotalDim= theWeyl.WeylDimFormulaFundamentalCoords(rightHWFundCoords);
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
    theWeyl.GenerateOrbit(tempRoots, false, currentOrbit, false, 0, 0, OrbitSizeHardLimit);
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
 Rational& theMult, CoefficientType& theCoeff, charSSAlgMod<CoefficientType>& outputAccum, Rational& finalCoeff
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
  CoefficientType coeffChange;
  coeffChange=theCoeff;
  coeffChange*= theMult*finalCoeff*sign;
//  std::cout << "; final contribution: " << tempMon.Coefficient.ElementToString()
//  << "*" << tempMon.weightFundamentalCoords.ElementToString() << "<br>";
  outputAccum.AddMonomial(tempMon, coeffChange);
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
    { tempS=this->TheObjects[i].ElementToString();
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
      if (this->TheObjects[i].NeedsBrackets())
        out << "(" << tempS << ")";
      else
        out << tempS;
      out << inputLetter << "_{" << i+1<< "}";
    }
  return out.str();
}

template <class CoefficientType>
std::string charSSAlgMod<CoefficientType>::operator*=(const charSSAlgMod& other)
{ GlobalVariables theGlobalVariables;
  return this->MultiplyBy(other, theGlobalVariables);
}

template <class CoefficientType>
std::string charSSAlgMod<CoefficientType>::MultiplyBy(const charSSAlgMod& other, GlobalVariables& theGlobalVariables)
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

template <class CoefficientType>
std::string charSSAlgMod<CoefficientType>::ElementToStringCharacter
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
    theMod.AddMonomial(currentSummand, theMults[i]);
  }
  out << CGI::GetHtmlMathSpanFromLatexFormulaAddBeginArrayRCL(theMod.ElementToString());
  return out.str();
}

int ParserNode::EvaluateFreudenthal(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  out << "Freudenthal formula: ";
  charSSAlgMod<Rational>& ch=theNode.owner->TheObjects[theArgumentList[0]].theChar.GetElement();
  charSSAlgMod<Rational> finalChar;
  std::string localDetailsString;
  if (ch.FreudenthalEvalMe(finalChar, localDetailsString, theGlobalVariables, 1000000))
  { out << "resulting character in fundamental coordinates: <br>";
    out << CGI::GetHtmlMathSpanFromLatexFormulaAddBeginArrayRCL(finalChar.ElementToString());
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
  << theWeyl.WeylDimFormulaSimpleCoords(*thePath.Waypoints.LastObject()).ElementToString();
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
  (Vector<CoefficientType>& HWDualCoordS, GlobalVariables& theGlobalVariables,
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
      { //std::cout << "<br>word " << i+1 << ": " << currentWordList[i].ElementToString(&theGlobalVariables.theDefaultLieFormat);
        //std::cout << "<br>word " << j+1 << ": " << currentWordList[j].ElementToString(&theGlobalVariables.theDefaultLieFormat);
        currentWordList[i].HWTAAbilinearForm
        (currentWordList[j], currentBF.elements[i][j], &HWDualCoordS, theGlobalVariables, theRingUnit, theRingZero, 0)
        ;
//        std::cout << "[" << currentWordList[i].ElementToString() << ", " << currentWordList[j].ElementToString() << "]=" <<currentBF.elements[i][j].ElementToString();
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
   int indexInputBasis, const Vector<CoefficientType>& subHiGoesToIthElement,
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
  this->theHWSimpleCoordSBaseField=theWeyl.GetSimpleCoordinatesFromFundamental(this->theHWFundamentalCoordsBaseField);

/*  std::cout << "<br>input fund coords base field: " << HWFundCoords.ElementToString();
  std::cout << "<br>dual coords no base field: " << this->theHWDualCoordS.ElementToStringGeneric();
  std::cout << "<br>dual coords: " << this->theHWDualCoordsBaseField.ElementToStringGeneric();
  std::cout << "<br>fund coords no base field: " << this->theHWFundamentalCoordS.ElementToString();
  std::cout << "<br>fund coords: " << this->theHWFundamentalCoordsBaseField.ElementToString();
  std::cout << "<br>simple coords no base field: " << this->theHWSimpleCoordS.ElementToString();
*/
  this->theChaR.MakeFromWeight(this->theHWSimpleCoordSBaseField, *this->theAlgebras, this->indexAlgebra);

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
  MonomialChar<CoefficientType> tempCharMon;
  this->theCharOverH.Reset();
  for (int i=0; i<thePaths.size; i++)
  { this->theModuleWeightsSimpleCoords.AddNoRepetition(*thePaths[i].Waypoints.LastObject());
    tempCharMon.weightFundamentalCoords= theWeyl.GetFundamentalCoordinatesFromSimple(*thePaths[i].Waypoints.LastObject());
    this->theCharOverH.AddMonomial(tempCharMon,1);
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
        << currentList[j].ElementToString(&theGlobalVariables.theDefaultLieFormat) << "  v( "
        << this->theHWFundamentalCoordsBaseField.ElementToString() << ", "
        << Vector<Rational>(this->parabolicSelectionNonSelectedAreElementsLevi).ElementToString() << ")";
;
      }
    }
  }
  this->IntermediateStepForMakeFromHW(this->theHWDualCoordsBaseField, theGlobalVariables, theRingUnit, theRingZero);
  bool isBad=false;
  if (outputReport!=0)
    for (int k=0; k<this->theBilinearFormsAtEachWeightLevel.size; k++)
    { Matrix<CoefficientType>& theBF=this->theBilinearFormsAtEachWeightLevel[k];
      Matrix<CoefficientType>& theBFinverted= this->theBilinearFormsInverted[k];
      if (outputReport!=0)
      { monomialDetailStream << "<hr>weight in simple coords: " << this->theModuleWeightsSimpleCoords[k].ElementToString();
        List<MonomialUniversalEnveloping<CoefficientType> >& currentList=this->theGeneratingWordsGrouppedByWeight[k];
        for (int i=0; i<currentList.size; i++)
        { MonomialUniversalEnveloping<CoefficientType>& currentElt=currentList[i];
          monomialDetailStream << "<br>monomial " << i+1 << ": " << currentElt.ElementToString(&theGlobalVariables.theDefaultLieFormat);
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
          out2 << "<hr>Simple generator: " << theSimpleGenerator.ElementToString(&theGlobalVariables.theDefaultLieFormat);
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
      [this->parabolicSelectionSelectedAreElementsLevi.elements[i]+ this->GetOwner().GetRank()]
      .ElementToString(&theGlobalVariables.theDefaultLieFormat)
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
        std::string theMonString=this->theGeneratingWordsGrouppedByWeight[i][j].ElementToString(&theGlobalVariables.theDefaultLieFormat);
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
      for (int j=0; j<currentH.size; j++)
        theSubbedH+=(*subHiGoesToIthElement)[j]*currentH[j];
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
void MonomialUniversalEnveloping<CoefficientType>::ModOutVermaRelations
(CoefficientType& outputCoeff, GlobalVariables* theContext, const Vector<CoefficientType>* subHiGoesToIthElement, const CoefficientType& theRingUnit,
 const CoefficientType& theRingZero)
{ int numPosRoots=this->GetOwner().GetNumPosRoots();
  int theDimension=this->GetOwner().GetRank();
  outputCoeff=theRingUnit;
//  if (subHiGoesToIthElement!=0)
//    std::cout << "<br>The subHiGoesToIthElement: " << subHiGoesToIthElement->ElementToString();
  for (int i=this->generatorsIndices.size-1; i>=0; i--)
  { int IndexCurrentGenerator=this->generatorsIndices[i];
    if (IndexCurrentGenerator>=numPosRoots+theDimension)
    { this->MakeZero(theRingZero, *this->owners, this->indexInOwners);
      outputCoeff=theRingZero;
      return;
    }
    if (IndexCurrentGenerator<numPosRoots)
      return;
    if (IndexCurrentGenerator>=numPosRoots && IndexCurrentGenerator<numPosRoots+theDimension)
    { if (subHiGoesToIthElement==0)
      { this->MakeZero(theRingZero, *this->owners, this->indexInOwners);
        outputCoeff=theRingZero;
        return;
      }
      int theDegree;
      if (!this->Powers[i].IsSmallInteger(theDegree))
        return;
      int hIndex=IndexCurrentGenerator-numPosRoots;
      CoefficientType theSubbedH;
      theSubbedH=(*subHiGoesToIthElement)[hIndex];
      MathRoutines::RaiseToPower(theSubbedH, theDegree, theRingUnit);
      outputCoeff*=(theSubbedH);
//      std::cout << "&nbsp outputCoeff=" << outputCoeff.ElementToString();
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
  this->CheckNumCoeffsConsistency(__FILE__, __LINE__);
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
  Vector<Polynomial<Rational> > theHW;
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
{
  return theNode.errorNoError;
}

template <class CoefficientType>
bool ElementUniversalEnveloping<CoefficientType>::HWMTAbilinearForm
  (const ElementUniversalEnveloping<CoefficientType>&right, CoefficientType& output,
   const Vector<CoefficientType>* subHiGoesToIthElement, GlobalVariables& theGlobalVariables,
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
  if (logStream!=0)
  { *logStream << "backtraced elt: " << MTright.ElementToString(&theGlobalVariables.theDefaultLieFormat) << "<br>";
    *logStream << "this element: " << this->ElementToString(&theGlobalVariables.theDefaultLieFormat) << "<br>";
   // for (int i=0; i<subHiGoesToIthElement->size; i++)
   // { *logStream << subHiGoesToIthElement->TheObjects[i].ElementToString();
   // }
  }
  for (int j=0; j<right.size; j++)
  { intermediateAccum=*this;
    intermediateAccum.Simplify(theGlobalVariables, theRingUnit, theRingZero);
    if (logStream!=0)
      *logStream << "intermediate after simplification: " << intermediateAccum.ElementToString(&theGlobalVariables.theDefaultLieFormat)
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
          { *logStream << "tempElt before mult: " << tempElt.ElementToString(&theGlobalVariables.theDefaultLieFormat) << "<br>";
            *logStream << "intermediate before mult: " << intermediateAccum.ElementToString(&theGlobalVariables.theDefaultLieFormat) << "<br>";
          }
          intermediateAccum.MultiplyBy(tempElt);
          if (logStream!=0)
            *logStream << "intermediate before simplification: " << intermediateAccum.ElementToString(&theGlobalVariables.theDefaultLieFormat) << "<br>";
          intermediateAccum.Simplify(theGlobalVariables, theRingUnit, theRingZero);
          if (logStream!=0)
            *logStream << "intermediate after simplification: " << intermediateAccum.ElementToString(&theGlobalVariables.theDefaultLieFormat) << "<br>";
          intermediateAccum.ModOutVermaRelations(&theGlobalVariables, subHiGoesToIthElement, theRingUnit, theRingZero);
          if (logStream!=0)
            *logStream << "intermediate after Verma rels: " << intermediateAccum.ElementToString(&theGlobalVariables.theDefaultLieFormat) << "<br>";
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
    *logStream << "final UE element: " << Accum.ElementToString(&theGlobalVariables.theDefaultLieFormat);
  return true;
}

template <class CoefficientType>
bool MonomialUniversalEnveloping<CoefficientType>::HWTAAbilinearForm
(const MonomialUniversalEnveloping<CoefficientType>&right, CoefficientType& output,
 const Vector<CoefficientType>* subHiGoesToIthElement, GlobalVariables& theGlobalVariables,
 const CoefficientType& theRingUnit, const CoefficientType& theRingZero, std::stringstream* logStream)
{ ElementUniversalEnveloping<CoefficientType> tempElt1, tempElt2;
  tempElt1.MakeZero(*this->owners, this->indexInOwners);
  tempElt1.AddMonomial(*this, theRingUnit);
  tempElt2.MakeZero(*this->owners, this->indexInOwners);
  tempElt2.AddMonomial(right, theRingUnit);
  return tempElt1.HWTAAbilinearForm
  (tempElt2, output, subHiGoesToIthElement, theGlobalVariables, theRingUnit, theRingZero, logStream);
}

template <class CoefficientType>
bool ElementUniversalEnveloping<CoefficientType>::HWTAAbilinearForm
  (const ElementUniversalEnveloping<CoefficientType>&right, CoefficientType& output,
   const Vector<CoefficientType>* subHiGoesToIthElement, GlobalVariables& theGlobalVariables,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero, std::stringstream* logStream)const
{ output=theRingZero;
  CoefficientType tempCF;
  ElementUniversalEnveloping<CoefficientType> TAleft;
  this->CheckNumCoeffsConsistency(__FILE__, __LINE__);
  TAleft=*this;
  TAleft.CheckNumCoeffsConsistency(__FILE__, __LINE__);
//  std::cout << "<hr>TAleft: " << TAleft.ElementToString(&theGlobalVariables.theDefaultLieFormat);
//  std::cout << "<br>right:" << right.ElementToString(&theGlobalVariables.theDefaultLieFormat);
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
//    std::cout << "<hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr>"
//  << "hw: " << subHiGoesToIthElement->ElementToString()
//  << "<hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr><hr>"
 // << "(" << this->ElementToStringCalculatorFormat(theGlobalVariables, tempFormat)
 // << "," << right.ElementToStringCalculatorFormat(theGlobalVariables, tempFormat) << ")";

  if (logStream!=0)
  { *logStream << "left eltement transposed: " << TAleft.ElementToString(&theGlobalVariables.theDefaultLieFormat) << "<br>";
    *logStream << "right element: " << right.ElementToString(&theGlobalVariables.theDefaultLieFormat) << "<br>";
  }
  startingElt=right;
  startingElt.Simplify(theGlobalVariables, theRingUnit, theRingZero);
  if (logStream!=0)
    *logStream << "right element after simplification: "
    << startingElt.ElementToString(&theGlobalVariables.theDefaultLieFormat) << "<br>";
  startingElt.ModOutVermaRelations(&theGlobalVariables, subHiGoesToIthElement, theRingUnit, theRingZero);
  if (logStream!=0)
    *logStream << "right element after Verma rels: "
    << startingElt.ElementToString(&theGlobalVariables.theDefaultLieFormat) << "<br>";
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
            *logStream << "intermediate before mult: " << intermediateAccum.ElementToString(&theGlobalVariables.theDefaultLieFormat) << "<br>";
          }
          intermediateAccum.MultiplyBy(tempElt);
          if (logStream!=0)
            *logStream << "intermediate before simplification: " << intermediateAccum.ElementToString(&theGlobalVariables.theDefaultLieFormat) << "<br>";
          intermediateAccum.Simplify(theGlobalVariables, theRingUnit, theRingZero);
          if (logStream!=0)
            *logStream << "intermediate after simplification: " << intermediateAccum.ElementToString(&theGlobalVariables.theDefaultLieFormat) << "<br>";
          intermediateAccum.ModOutVermaRelations(&theGlobalVariables, subHiGoesToIthElement, theRingUnit, theRingZero);
          if (logStream!=0)
            *logStream << "intermediate after Verma rels: " << intermediateAccum.ElementToString(&theGlobalVariables.theDefaultLieFormat) << "<br>";
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
    *logStream << "final UE element: " << Accum.ElementToString(&theGlobalVariables.theDefaultLieFormat);
  this->GetOwner().UEGeneratorOrderIncludingCartanElts=oldOrder;
//  if (logStream!=0)
//    std::cout << "<hr>" << logStream->str();
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
  Vector<Polynomial<Rational> > theHW;
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
(const Vector<CoefficientType>* subHiGoesToIthElement, GlobalVariables& theGlobalVariables, const CoefficientType& theRingUnit,
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
  for (int i=0; i< theOrbit.size; i++)
  { ElementUniversalEnveloping<CoefficientType>& current=theOrbit[i];
    out << "<br>" << current.ElementToString(&theGlobalVariables.theDefaultLieFormat);
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
  charSSAlgMod<Rational>& theChar=charNode.theChar.GetElement();
  std::string report;
  charSSAlgMod<Rational> tempChar;
  ReflectionSubgroupWeylGroup subWeyl;
  Vector<Rational> emptySel;
  emptySel.MakeZero(theDim);
  theChar.SplitCharOverLevi(&report,  tempChar, parSel, emptySel, subWeyl, theGlobalVariables);
  out << report;
  theNode.outputString=out.str();
  theNode.ExpressionType=theNode.typeString;
  return theNode.errorNoError;
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
  if (!CGI::FileExists(PhysicalNameLieBracketFullPathNoEnding+".png") )
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
    lieBracketFile1 << "\\documentclass{article}\\usepackage{longtable}\n\\begin{document}\\pagestyle{empty}\n"
    << this->theHmm.theRange().ElementToString(&theGlobalVariables.theDefaultLieFormat) << "\n\\end{document}";
    this->SystemCommands.AddOnTop("latex  -output-directory=" + PhysicalPathCurrentType + " " + PhysicalNameLieBracketFullPathNoEnding +".tex");
    this->SystemCommands.AddOnTop("dvipng " + PhysicalNameLieBracketFullPathNoEnding + ".dvi -o " + PhysicalNameLieBracketFullPathNoEnding + ".png -T tight");
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
//  FormatExpressions theFormat;
//  theFormat.MakeAlphabetArbitraryWithIndex("g", "h");
//  std::cout << "<br>Acting on " << theNode.theGenVermaElt.GetElement().ElementToString(theGlobalVariables)
//  << " by " << tempElt.ElementToString(theGlobalVariables, theFormat);
//  theNode.theGenVermaElt.GetElement().MultiplyMeByUEEltOnTheLeft
//  (theNode.owner->theModulePolys, tempElt, theGlobalVariables, RFone, RFZero)
//  ;
  theNode.ExpressionType=theNode.typeGenVermaElt;
  return theNode.errorNoError;
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
std::string ModuleSSalgebraNew<CoefficientType>::ElementToString()const
{ std::stringstream out;
  GlobalVariables theGlobalVariables; FormatExpressions theformat;
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

template <class CoefficientType>
void ElementUniversalEnveloping<CoefficientType>::ModOutVermaRelations
  (GlobalVariables* theContext, const Vector<CoefficientType>* subHiGoesToIthElement,
   const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{ MonomialUniversalEnveloping<CoefficientType> tempMon;
  ElementUniversalEnveloping<CoefficientType> output;
  output.MakeZero(*this->owners, this->indexInOwners);
  CoefficientType acquiredCoeff;
  for (int i=0; i<this->size; i++)
  { tempMon= this->TheObjects[i];
    tempMon.ModOutVermaRelations(acquiredCoeff, theContext, subHiGoesToIthElement, theRingUnit, theRingZero);
    acquiredCoeff*=this->theCoeffs[i];
//    std::cout << "<hr><hr>Adding " << tempMon.ElementToString() << " times " << acquiredCoeff.ElementToString() << " to " << output.ElementToString();
    output.AddMonomial(tempMon, acquiredCoeff);
//    std::cout <<"<br> to obtain " << output.ElementToString();
  }
  this->operator=(output);
}

template <class CoefficientType>
void ElementUniversalEnveloping<CoefficientType>::Simplify
(GlobalVariables& theGlobalVariables, const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{ ElementUniversalEnveloping<CoefficientType> buffer;
  ElementUniversalEnveloping<CoefficientType> outpuT;
  MonomialUniversalEnveloping<CoefficientType> tempMon;
  CoefficientType currentCoeff;
  outpuT.MakeZero(*this->owners, this->indexInOwners);
  for (; this->size>0; )
  {// FormatExpressions tempFormat;
    //tempFormat.MakeAlphabetArbitraryWithIndex("g", "h");
    //std::cout << "<hr>(At the start of reduction cycle) *this+output - (At the end of reduction cycle)(*this+output)=<br>" << (*this+outpuT).ElementToString(&tempFormat);
    this->PopMonomial(this->size-1, tempMon, currentCoeff);
    bool reductionOccurred=false;
    for (int i=0; i<tempMon.generatorsIndices.size-1; i++)
      if (!this->GetOwner().AreOrderedProperly(tempMon.generatorsIndices[i], tempMon.generatorsIndices[i+1]))
      { if (tempMon.SwitchConsecutiveIndicesIfTheyCommute(i))
        { this->AddMonomial(tempMon, currentCoeff);
          reductionOccurred=true;
          break;
        }
        if (tempMon.CommutingAnBtoBAnPlusLowerOrderAllowed
            (tempMon.Powers[i], tempMon.generatorsIndices[i], tempMon.Powers[i+1], tempMon.generatorsIndices[i+1]))
        { tempMon.CommuteAnBtoBAnPlusLowerOrder(i, buffer, theRingUnit, theRingZero);
          buffer*=currentCoeff;
          *this+=buffer;
          reductionOccurred=true;
          break;
        }
        if (tempMon.CommutingABntoBnAPlusLowerOrderAllowed
            (tempMon.Powers[i], tempMon.generatorsIndices[i], tempMon.Powers[i+1], tempMon.generatorsIndices[i+1]))
        { tempMon.CommuteABntoBnAPlusLowerOrder(i, buffer, theRingUnit, theRingZero);
          buffer*=currentCoeff;
          *this+=buffer;
          reductionOccurred=true;
          break;
        }
      }
    if(!reductionOccurred)
      outpuT.AddMonomial(tempMon, currentCoeff);
//    std::cout << "-<br>(" << (*this+outpuT).ElementToString(&tempFormat) << ")<br>(this should simplify to zero).";
  }
  *this=outpuT;
}

template <class CoefficientType>
bool MonomialUniversalEnveloping<CoefficientType>::CommutingAnBtoBAnPlusLowerOrderAllowed
(CoefficientType& theLeftPower, int leftGeneratorIndex,
 CoefficientType& theRightPower, int rightGeneratorIndex)
{ return this->CommutingABntoBnAPlusLowerOrderAllowed
(theRightPower, rightGeneratorIndex, theLeftPower, leftGeneratorIndex);
}

template <class CoefficientType>
bool MonomialUniversalEnveloping<CoefficientType>::CommutingABntoBnAPlusLowerOrderAllowed
(CoefficientType& theLeftPower, int leftGeneratorIndex, CoefficientType& theRightPower, int rightGeneratorIndex)
{ if (!theLeftPower.IsSmallInteger())
    return false;
  if(theRightPower.IsSmallInteger())
    return true;
  int numPosRoots=this->GetOwner().theWeyl.RootsOfBorel.size;
  int theDimension= this->GetOwner().theWeyl.CartanSymmetric.NumRows;
  if(rightGeneratorIndex>= numPosRoots && rightGeneratorIndex<numPosRoots+theDimension)
    return this->GetOwner().theLiebrackets.elements[leftGeneratorIndex][rightGeneratorIndex].IsEqualToZero();
  return true;
}

template <class CoefficientType>
bool MonomialUniversalEnveloping<CoefficientType>::SwitchConsecutiveIndicesIfTheyCommute
(int theLeftIndex)
{ if (theLeftIndex>= this->generatorsIndices.size-1)
    return false;
  int leftGenerator=this->generatorsIndices[theLeftIndex];
  int rightGenerator=this->generatorsIndices[theLeftIndex+1];
  if (!this->GetOwner().theLiebrackets.elements[leftGenerator][rightGenerator].IsEqualToZero())
    return false;
  this->generatorsIndices.SwapTwoIndices(theLeftIndex, theLeftIndex+1);
  this->Powers.SwapTwoIndices(theLeftIndex, theLeftIndex+1);
  this->SimplifyEqualConsecutiveGenerators(theLeftIndex-1);
  return true;
}

template <class CoefficientType>
bool MonomialUniversalEnveloping<CoefficientType>::SimplifyEqualConsecutiveGenerators(int lowestNonReducedIndex)
{ if (this->generatorsIndices.size<1)
    return false;
  if (lowestNonReducedIndex<0)
    lowestNonReducedIndex=0;
  bool result=false;
  for (int next=lowestNonReducedIndex+1; next<this->generatorsIndices.size; next++)
    if (this->generatorsIndices[lowestNonReducedIndex]==this->generatorsIndices[next])
    { result=true;
      this->Powers[lowestNonReducedIndex]+=this->Powers[next];
    }
    else
    { lowestNonReducedIndex++;
      this->Powers[lowestNonReducedIndex]=this->Powers[next];
      this->generatorsIndices[lowestNonReducedIndex]=this->generatorsIndices[next];
    }
  this->generatorsIndices.SetSize(lowestNonReducedIndex+1);
  this->Powers.SetSize(lowestNonReducedIndex+1);
  return result;
}

template <class CoefficientType>
void MonomialUniversalEnveloping<CoefficientType>::CommuteAnBtoBAnPlusLowerOrder
(int indexA, ElementUniversalEnveloping<CoefficientType>& output, const CoefficientType& theRingUnit,
 const CoefficientType& theRingZero)
{ if (indexA==this->generatorsIndices.size-1)
    return;
  output.MakeZero(*this->owners, this->indexInOwners);
  MonomialUniversalEnveloping<CoefficientType> tempMon;
  tempMon.init(*this->owners, this->indexInOwners);
  tempMon.Powers.Reserve(this->generatorsIndices.size+2);
  tempMon.generatorsIndices.Reserve(this->generatorsIndices.size+2);
  tempMon.Powers.size=0;
  tempMon.generatorsIndices.size=0;
  int rightGeneratorIndeX= this->generatorsIndices[indexA+1];
  int leftGeneratorIndeX=this->generatorsIndices[indexA];
  CoefficientType theRightPoweR, theLeftPoweR;
  theRightPoweR= this->Powers[indexA+1];
  theLeftPoweR= this->Powers[indexA];
  theRightPoweR-=1;
  int powerDroP=0;
  CoefficientType acquiredCoefficienT, incomingAcquiredCoefficienT;
  acquiredCoefficienT=theRingUnit;
  for (int i=0; i<indexA; i++)
    tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices[i], this->Powers[i]);
  MonomialUniversalEnveloping<CoefficientType> startMon;
  startMon=tempMon;
  ElementSemisimpleLieAlgebra adAToTheIthOfB, aElt;
  adAToTheIthOfB.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE
  (rightGeneratorIndeX, *this->owners, this->indexInOwners);
  aElt.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE
  (leftGeneratorIndeX, *this->owners, this->indexInOwners);
  //Formula realized:
  //a^n b =\sum_{i=0}^\infty \binom{n}{i} (\ad a)^i (b)a^{n-i}
  //Proof (Dixmier): let L_x stand for left multiplication by x and R_x stand for right multiplication.
  //Then L_x and R_x commute and L_x-R_x=ad_x, i.e.
  //(L_a)^n=(R_a+ad_a)^n.
  do
  { for (int i=0; i<adAToTheIthOfB.size; i++)
    { int theNewGeneratorIndex=adAToTheIthOfB[i].theGeneratorIndex;
      tempMon=startMon;
      incomingAcquiredCoefficienT=acquiredCoefficienT;
      incomingAcquiredCoefficienT*=adAToTheIthOfB.theCoeffs[i];
      tempMon.MultiplyByGeneratorPowerOnTheRight(theNewGeneratorIndex, theRingUnit);
      tempMon.MultiplyByGeneratorPowerOnTheRight(leftGeneratorIndeX, theLeftPoweR);
      tempMon.MultiplyByGeneratorPowerOnTheRight(rightGeneratorIndeX, theRightPoweR);
      for (int i=indexA+2; i<this->generatorsIndices.size; i++)
        tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices[i], this->Powers[i]);
      //tempMon.ComputeDebugString();
      output.AddMonomial(tempMon, incomingAcquiredCoefficienT);
    }
    powerDroP++;
    acquiredCoefficienT*=(theLeftPoweR);
    acquiredCoefficienT/=(powerDroP);
    theLeftPoweR-=1;
//    std::cout <<"<hr>(ad_a)(" << adAToTheIthOfB.ElementToString(0) << ") =";
    this->GetOwner().LieBracket(aElt, adAToTheIthOfB, adAToTheIthOfB);
//    std::cout << adAToTheIthOfB.ElementToString(0);
  }while(!adAToTheIthOfB.IsEqualToZero() && !acquiredCoefficienT.IsEqualToZero());
//  std::cout << "<hr>final output: " << this->ElementToString() << " = " << output.ElementToString();
}

template <class CoefficientType>
void MonomialUniversalEnveloping<CoefficientType>::CommuteABntoBnAPlusLowerOrder
(int theIndeX, ElementUniversalEnveloping<CoefficientType>& output,
 const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{ if (theIndeX==this->generatorsIndices.size-1)
    return;
  output.MakeZero(*this->owners, this->indexInOwners);
  MonomialUniversalEnveloping<CoefficientType> tempMon;
  tempMon.init(*this->owners, this->indexInOwners);
  tempMon.Powers.Reserve(this->generatorsIndices.size+2);
  tempMon.generatorsIndices.Reserve(this->generatorsIndices.size+2);
  tempMon.Powers.size=0;
  tempMon.generatorsIndices.size=0;
  int rightGeneratorIndex= this->generatorsIndices[theIndeX+1];
  int leftGeneratorIndex=this->generatorsIndices[theIndeX];
  CoefficientType theRightPower, theLeftPower;
  theRightPower= this->Powers[theIndeX+1];
  theLeftPower= this->Powers[theIndeX];
  theLeftPower-=1;
  int powerDrop=0;
  CoefficientType acquiredCoefficient, incomingAcquiredCoefficient;
  acquiredCoefficient=theRingUnit;
  for (int i=0; i<theIndeX; i++)
    tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices[i], this->Powers[i]);
  tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices[theIndeX], theLeftPower);
  MonomialUniversalEnveloping<CoefficientType> startMon, tempMon2;
  startMon=tempMon;
  ElementSemisimpleLieAlgebra adResult, tempElt, rightGeneratorElt;
  adResult.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE
  (leftGeneratorIndex, *this->owners, this->indexInOwners);
  rightGeneratorElt.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE
  (rightGeneratorIndex, *this->owners, this->indexInOwners);
  std::cout << this->ElementToString() << "=";
  //Formula realized:
  //a b^n  &=&\sum_{i=0}^\infty b^{n-i}(-\ad b)^i (a) \binom{n}{i} .
  //Proof (Dixmier): let L_x stand for left multiplication by x and R_x stand for right multiplication.
  //Then L_x and R_x commute and L_x-R_x=ad_x, i.e.
  //(L_b-ad_b)^n=R_b^n.
  do
  { for (int i=0; i<adResult.size; i++)
    { int theNewGeneratorIndex= adResult[i].theGeneratorIndex;
      tempMon=startMon;
      tempMon.MultiplyByGeneratorPowerOnTheRight(rightGeneratorIndex, theRightPower);
      incomingAcquiredCoefficient=acquiredCoefficient;
      incomingAcquiredCoefficient*=(adResult.theCoeffs[i]);
      tempMon.MultiplyByGeneratorPowerOnTheRight(theNewGeneratorIndex, theRingUnit);
      for (int i=theIndeX+2; i<this->generatorsIndices.size; i++)
        tempMon.MultiplyByGeneratorPowerOnTheRight(this->generatorsIndices[i], this->Powers[i]);
      output.AddMonomial(tempMon, incomingAcquiredCoefficient);
      std::cout << "+(" << incomingAcquiredCoefficient.ElementToString() << ")" << tempMon.ElementToString();
    }
    acquiredCoefficient*=(theRightPower);
    theRightPower-=1;
    this->GetOwner().LieBracket(adResult, rightGeneratorElt, tempElt);
    adResult=tempElt;
    powerDrop++;
    acquiredCoefficient/=(powerDrop);
  }while(!adResult.IsEqualToZero() && !acquiredCoefficient.IsEqualToZero());
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
