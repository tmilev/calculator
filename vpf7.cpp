//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader1.h"
#include "vpfHeader1_2.h"

ProjectInformationInstance ProjectInfoVpf7cpp
(__FILE__, "Implementation file intended for small fast changes. ");

void SemisimpleLieAlgebra::GetChevalleyGeneratorAsLieBracketsSimpleGens
  (int generatorIndex, List<int>& outputIndicesFormatAd0Ad1Ad2etc,
   Rational& outputMultiplyLieBracketsToGetGenerator)
{ MacroRegisterFunctionWithName("SemisimpleLieAlgebra::GetChevalleyGeneratorAsLieBracketsSimpleGens");
  outputIndicesFormatAd0Ad1Ad2etc.size=0;
  if (this->IsGeneratorFromCartan(generatorIndex))
  { int simpleIndex=generatorIndex-this->GetNumPosRoots();
    outputIndicesFormatAd0Ad1Ad2etc.AddOnTop(generatorIndex+this->GetRank());
    outputIndicesFormatAd0Ad1Ad2etc.AddOnTop( 2*this->GetNumPosRoots()-1-generatorIndex);
    outputMultiplyLieBracketsToGetGenerator=this->theWeyl.CartanSymmetric.elements[simpleIndex][simpleIndex]/2;
    return;
  }
  Vector<Rational> theWeight=this->GetWeightOfGenerator(generatorIndex);
  outputMultiplyLieBracketsToGetGenerator=1;
  Vector<Rational> genWeight, newWeight;
  while (!theWeight.IsEqualToZero())
    for (int i=0; i<this->GetRank(); i++)
    { genWeight.MakeEi(this->GetRank(), i);
      if (theWeight.IsPositive())
        genWeight.Minus();
      std::string debugString=theWeight.ToString();
      newWeight=theWeight+genWeight;
      if (newWeight.IsEqualToZero() || this->theWeyl.IsARoot(newWeight))
      { theWeight=newWeight;
        int theIndex=this->GetGeneratorFromRoot(-genWeight);
        outputIndicesFormatAd0Ad1Ad2etc.AddOnTop(theIndex);
        if (!theWeight.IsEqualToZero())
        { int currentIndex=this->theWeyl.RootSystem.GetIndex(theWeight);
          theIndex=this->GetRootIndexFromGenerator(theIndex);
          if (!this->Computed.elements[theIndex][currentIndex])
          { std::cout << "This is a programming error. For some reason I am not computed. "
            << " Here is me: "
            << this->ToString() << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
            assert(false);
          }
          outputMultiplyLieBracketsToGetGenerator/=this->ChevalleyConstants.elements[theIndex][currentIndex];
        }
        break;
      }
    }
}

template<class CoefficientType>
void ModuleSSalgebra<CoefficientType>::TestConsistency(GlobalVariables& theGlobalVariables)
{ MacroRegisterFunctionWithName("ModuleSSalgebra<CoefficientType>::TestConsistency");
  ProgressReport theReport(&theGlobalVariables);
  MatrixTensor<CoefficientType> left, right, output, otherOutput, tempMat, diffMat;
  for (int i=0; i<this->GetOwner().GetNumGenerators(); i++)
    for (int j=0; j<this->GetOwner().GetNumGenerators(); j++)
    { left = this->GetActionGeneratorIndeX(i, theGlobalVariables);
      right = this->GetActionGeneratorIndeX(j, theGlobalVariables);
      output=right;
      output.LieBracketOnTheLeft(left);
      ElementSemisimpleLieAlgebra<Rational> leftGen, rightGen, outputGen;
      leftGen.MakeGenerator(i, *this->theAlgebras, this->indexAlgebra);
      rightGen.MakeGenerator(j, *this->theAlgebras, this->indexAlgebra);
      this->GetOwner().LieBracket(leftGen, rightGen, outputGen);
      otherOutput.MakeZero();
      for (int k=0; k<outputGen.size; k++)
      { tempMat=this->GetActionGeneratorIndeX(outputGen[k].theGeneratorIndex, theGlobalVariables);
        tempMat*=outputGen.theCoeffs[k];
        otherOutput+=tempMat;
      }
      diffMat=otherOutput;
      diffMat-=output;
      if(!diffMat.IsEqualToZero())
      { std::cout << "<br>[" << left.ToString() << ", " << right.ToString() << "] = " << output.ToString();
        std::cout << "<br> [" << leftGen.ToString() << ", " << rightGen.ToString() << "] = " << outputGen.ToString();
        std::cout << "<br> " << outputGen.ToString() << "->" << otherOutput.ToString();
        std::cout << "<br>This is a programming error: something is wrong with "
        << " the algorithm, a check fails! " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
        assert(false);
      } else
      { std::stringstream tempStream;
        tempStream << "tested index" << i+1 << " out of "
        << this->GetOwner().GetNumGenerators() << ", " << j+1 << " out of "
        << this->GetOwner().GetNumGenerators();
        theReport.Report(tempStream.str());
      }
    }
  for (int i=0; i<this->GetOwner().GetNumPosRoots(); i++)
  { left=this->GetActionGeneratorIndeX(i, theGlobalVariables);
    right=this->GetActionGeneratorIndeX(this->GetOwner().GetNumGenerators()-1-i, theGlobalVariables);
    left.Transpose();
    left-=right;
//    std::cout << "<br>left minus right: " << left.ToString();
    left=this->GetActionGeneratorIndeX(i, theGlobalVariables);
    right.LieBracketOnTheLeft(left);
//    std::cout << "<br> [left,right]" << right.ToString();

  }
  std::cout << "Consistency check passed successfully!";
}


std::string ReflectionSubgroupWeylGroup::ElementToStringCosetGraph()
{ if (this->size<1)
    return "Error, non-initialized group";
  if (this->size==1)
    return "id";
  List<List<List<int> > > arrows;
  List<List<int> > Layers;
  Vector<Rational> tempRoot;
  Layers.ReservE(this->RepresentativesQuotientAmbientOrder.size);
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
  this->RepresentativesQuotientAmbientOrder.ReservE(this->AmbientWeyl.size);
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
      << tempCone.Vertices[i].ToString() << " as well as its opposite vector "
      << (-tempCone.Vertices[i]).ToString()
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

void LittelmannPath::ActByEFDisplayIndex(int displayIndex)
{ if(this->owner==0)
  { std::cout << " This is a programming error: LS path without initialized owner is begin "
    << "acted upon. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  if (displayIndex>0)
    this->ActByEalpha(displayIndex-1);
  else
    this->ActByFalpha(-displayIndex-1);
}

void LittelmannPath::ActByEalpha(int indexAlpha)
{ if(this->owner==0)
  { std::cout << " This is a programming error: LS path without initialized owner "
    << "is begin acted upon. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  if (indexAlpha<0 || indexAlpha>=this->owner->GetDim())
  { std::cout << " This is a programming error: index of Littelmann root operator out of range. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
 if (this->Waypoints.size==0)
    return;
  Rational theMin=0;
  int minIndex=-1;
  assert (this->owner!=0);
  WeylGroup& theWeyl=*this->owner;
  theWeyl.ComputeRho(true);
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
//  std::cout << "<hr>Starting path: " << this->ToString();
  if (!this->MinimaAreIntegral())
    std::cout << "<br>WTF, starting path is BAD!";
//  std::cout << "<br>Min waypoint:" << this->Waypoints[minIndex].ToString();
//  std::cout << " with minimum: " << theMin.ToString();
  if (s2>theMin+1)
  { this->Waypoints.SetSize(this->Waypoints.size+1);
    for (int i=this->Waypoints.size-1; i>=precedingIndex+2; i--)
      this->Waypoints[i]=this->Waypoints[i-1];
    precedingIndex++;
    minIndex++;
//    std::cout << "<br>fake waypoint added: " << this->ToString();
    Rational scalarNext=theWeyl.RootScalarCartanRoot(this->Waypoints[precedingIndex],alphaScaled);
    Vector<Rational>& r1=this->Waypoints[precedingIndex];
    Vector<Rational>& r2=this->Waypoints[precedingIndex-1];
    Rational s1=theWeyl.RootScalarCartanRoot(r1, alphaScaled);
    Rational x= (theMin+1-s2)/(s1-s2);
    this->Waypoints[precedingIndex]= (r1-r2)*x+ r2;
//    std::cout << "<br> fake waypoint corrected: " << this->ToString();
  }
//  std::cout << "<br>Min waypoint:" << this->Waypoints[minIndex].ToString();
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
//  std::cout << "<br> result before simplification: " << this->ToString();
  this->Simplify();
//  std::cout << "<br> final: " << this->ToString();
}

void LittelmannPath::ActByFalpha(int indexAlpha)
{ if (this->Waypoints.size==0)
    return;
  if(this->owner==0)
  { std::cout << " This is a programming error: LS path without initialized "
    << "owner is begin acted upon. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  if (indexAlpha<0 || indexAlpha>=this->owner->GetDim())
  { std::cout << " This is a programming error: index of Littelmann root operator out of range. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
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
//  Vectors<Rational> differences;
  Vector<Rational> diff, oldWayPoint;
  oldWayPoint=this->Waypoints[minIndex];
  Rational currentDist=0;
  for (int i=0; i<succeedingIndex-minIndex; i++)
  { diff=this->Waypoints[i+minIndex+1]-oldWayPoint;
    currentDist+=theWeyl.RootScalarCartanRoot(diff, alphaScaled);
    if (currentDist>0)
    { theWeyl.SimpleReflection(indexAlpha, diff, false, false);
      currentDist=0;
    }
    oldWayPoint=this->Waypoints[i+minIndex+1];
    this->Waypoints[i+minIndex+1]=this->Waypoints[i+minIndex]+diff;
  }
  for (int i=succeedingIndex+1; i<this->Waypoints.size; i++)
    this->Waypoints[i]-=alpha;
  this->Simplify();
}

void LittelmannPath::Simplify()
{ if (this->Waypoints.size==0)
    return;
  Rational strech;
  Vector<Rational> d1, d2;
  Rational d11, d12, d22;
  int leftIndex=0;
  int rightIndex=2;
//  std::stringstream tempStream;
//  tempStream <<"<br>" << this->ToString();
  while (rightIndex<this->Waypoints.size)
  { Vector<Rational>& left=this->Waypoints[leftIndex];
    Vector<Rational>& middle=this->Waypoints[rightIndex-1];
    Vector<Rational>& right=this->Waypoints[rightIndex];
    d1=left-middle;
    d2=right-middle;
    d11=d1.ScalarEuclidean(d1);
    d12=d1.ScalarEuclidean(d2);
    d22=d2.ScalarEuclidean(d2);
    bool isBad=((d11*d22-d12*d12).IsEqualToZero() && (d12<=0));
    if (!isBad)
    { leftIndex++;
      this->Waypoints[leftIndex]=middle;
    }
    rightIndex++;
  }
  leftIndex++;
  this->Waypoints[leftIndex]=*this->Waypoints.LastObject();
/*  if (leftIndex+1<this->Waypoints.size)
  { this->Waypoints.SetSize(leftIndex+1);
    tempStream << " reduced to " << this->ToString();
    std::cout << tempStream.str();
  }*/
  this->Waypoints.SetSize(leftIndex+1);
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

void LittelmannPath::MakeFromWeightInSimpleCoords
  (const Vector<Rational>& weightInSimpleCoords, WeylGroup& theOwner)
{ this->owner=& theOwner;
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
  (*inputStartingPath.Waypoints.LastObject() ).ToString();
  for (int i=0; i<input.size; i++)
    out << " ) ";
  return out.str();
}

bool LittelmannPath::GenerateOrbit
(List<LittelmannPath>& output, List<List<int> >& outputOperators,
 GlobalVariables& theGlobalVariables, int UpperBoundNumElts,
 Selection* parabolicNonSelectedAreInLeviPart)
{ HashedList<LittelmannPath> hashedOutput;
  hashedOutput.AddOnTop(*this);
  int theDim=this->owner->GetDim();
  outputOperators.SetSize(1);
  outputOperators[0].SetSize(0);
  List<int> currentSequence;
  if (UpperBoundNumElts>0)
    currentSequence.ReservE(UpperBoundNumElts);
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
  //std::cout << "<br>parabolicSelectionSelectedAreInLeviPart = " << parabolicSelectionSelectedAreInLeviPart.ToString();
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
  //          hashedOutput.AddOnTopNoRepetition(currentPath);
            if (hashedOutput.AddOnTopNoRepetition(currentPath))
            { found=true;
              currentSequence.AddOnTop(theIndex);
              outputOperators.AddOnTop(currentSequence);
              if (!currentPath.MinimaAreIntegral())
              { std::cout << "<hr>Found a bad path:<br> ";
                std::cout << " = " << currentPath.ToString();
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
            if (hashedOutput.AddOnTopNoRepetition(currentPath))
            { found=true;
              currentSequence.AddOnTop(-theIndex-1);
              outputOperators.AddOnTop(currentSequence);
              if (!currentPath.MinimaAreIntegral())
              { std::cout << "<hr>Found a bad path:<br> ";
                std::cout << " = " << currentPath.ToString();
              }
            }
        }
      }
  output=(hashedOutput);
  return result;
}

std::string LittelmannPath:: ElementToStringOperatorSequenceStartingOnMe
(List<int>& input)
{ MonomialTensor<Rational> tempMon;
  tempMon=input;
  tempMon.generatorsIndices.ReverseOrderElements();
  tempMon.Powers.ReverseOrderElements();
  return tempMon.ToString();
}

template <class CoefficientType>
void ModuleSSalgebra<CoefficientType>::ExpressAsLinearCombinationHomogenousElement
  (ElementUniversalEnveloping<CoefficientType>& inputHomogeneous,
   ElementUniversalEnveloping<CoefficientType>& outputHomogeneous,
   int indexInputBasis, const Vector<CoefficientType>& subHiGoesToIthElement,
   GlobalVariables& theGlobalVariables, const CoefficientType& theRingUnit, const CoefficientType& theRingZero
   )
{ Vector <CoefficientType> theScalarProducts;
  List<MonomialUniversalEnveloping<CoefficientType> >& inputBasis =
  this->theGeneratingWordsGrouppedByWeight[indexInputBasis];
  theScalarProducts.SetSize(inputBasis.size);
//  std::cout << "<hr>Expressing " << inputHomogeneous.ToString
//  (theGlobalVariables, theGlobalVariables.theDefaultFormat);

  for (int i=0; i<inputBasis.size; i++)
  { std::stringstream tempStream;
//    std::cout << "<br>(" << inputHomogeneous.ToString(false, false, theGlobalVariables, theGlobalVariables.theDefaultFormat)
//    << "," << inputBasis[i].ToString(false, false, theGlobalVariables, theGlobalVariables.theDefaultFormat)
//    << " )=";
//    std::cout << theScalarProducts[i].ToString() ;

    inputHomogeneous.HWTAAbilinearForm
    (inputBasis[i], theScalarProducts[i], &subHiGoesToIthElement, theGlobalVariables, theRingUnit, theRingZero, &tempStream);
/*    std::cout << "<br>Computation details: " << tempStream.str();
    if ( inputHomogeneous.ToString(false, false, theGlobalVariables, theGlobalVariables.theDefaultFormat)=="x_{-1}"
        &&  theScalarProducts[i].ToString()=="0"
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
void ModuleSSalgebra<CoefficientType>::GetAdActionHomogenousElT
  (ElementUniversalEnveloping<CoefficientType>& inputHomogeneous, Vector<Rational>& weightUEEltSimpleCoords,
   List<List<ElementUniversalEnveloping<CoefficientType> > >& outputSortedByArgumentWeight,
   GlobalVariables& theGlobalVariables, const CoefficientType& theRingUnit, const CoefficientType& theRingZero)
{ Vector<Rational> targetWeight;
  outputSortedByArgumentWeight.SetSize(this->theGeneratingWordsGrouppedByWeight.size);
  ElementUniversalEnveloping<CoefficientType> theElt;
  std::string generatorString=inputHomogeneous.ToString();
  ProgressReport theReport(&theGlobalVariables);
  for (int i=0; i<this->theGeneratingWordsGrouppedByWeight.size; i++)
  { List<MonomialUniversalEnveloping<CoefficientType> >& currentWordList=
    this->theGeneratingWordsGrouppedByWeight[i];
    List<ElementUniversalEnveloping<CoefficientType> >& outputCurrentList=
    outputSortedByArgumentWeight[i];
    outputCurrentList.SetSize(currentWordList.size);
    Vector<Rational>& currentWeight=this->theModuleWeightsSimpleCoords[i];
    targetWeight=currentWeight+weightUEEltSimpleCoords;
//    std::cout << "<br>target weight: " << targetWeight.ToString() << "="
//    << currentWeight.ToString() << "+" << inputWeight.ToString();
    int theIndex=this->theModuleWeightsSimpleCoords.GetIndex(targetWeight);
    for (int j=0; j<currentWordList.size; j++)
    { std::stringstream progressStream;
      progressStream << "Computing action of " << generatorString << " on weight layer "
      << i+1 << " out of " << this->theGeneratingWordsGrouppedByWeight.size << ", word "
      << j+1 << " out of " << currentWordList.size
      << "...";
      theReport.Report(progressStream.str());
      ElementUniversalEnveloping<CoefficientType>& currentOutputWord=outputCurrentList[j];
      if (theIndex==-1)
        currentOutputWord.MakeZero(*this->theAlgebras, this->indexAlgebra);
      else
      { theElt=inputHomogeneous;
        theElt.MultiplyBy(currentWordList[j], theRingUnit);
        this->ExpressAsLinearCombinationHomogenousElement
        (theElt, currentOutputWord, theIndex, this->theHWDualCoordsBaseFielD, theGlobalVariables, theRingUnit, theRingZero);
      }
      progressStream << " done!";
      theReport.Report(progressStream.str());
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
    ElementSemisimpleLieAlgebra<Rational>& currentElt=this->owner->theOrder[IndexCurrentGenerator];
    if (!currentElt.GetCartanPart().IsEqualToZero() || currentElt.size>1)
      return false;
    int thePower=0;
    if (!this->Powers[k].IsSmallInteger(thePower))
      return false;
    int rootIndex= this->owner->theOwner.GetRootIndexFromGenerator(currentElt[0].theGeneratorIndex);
    Vector<Rational>& currentRoot=theWeyl.RootSystem[rootIndex];
//    std::cout << "<hr>The power: " << thePower;
    for (int j=0; j<thePower; j++)
    { currentWeight+=currentRoot;
//      std::cout << "<br>current weight is: " << currentWeight.ToString();
      testWeight=currentWeight;
      theWeyl.RaiseToDominantWeight(testWeight);
//      std::cout << "; raised to highest: " << testWeight.ToString();
//      std::cout << "<br>theHWsimpleCoordsTrue-currentWeight raised to highest = "
//      << (theHWsimpleCoordsTrue-testWeight).ToString();
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
      outputCorrespondingMonomials.AddOnTopNoRepetition(theElements.TheObjects[i].TheObjects[j]);
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
  result.MakeZero(*this->owneR);
  int numPosRoots=this->GetOwner().GetNumPosRoots();
  int theRank=this->GetOwner().GetRank();
  CoefficientType theCoeff;
  for (int i=0; i<this->size; i++)
  { MonomialUniversalEnveloping<CoefficientType>& currentMon=this->TheObjects[i];
    theCoeff=this->theCoeffs[i];
    tempMon.owneR=currentMon.owneR;
    tempMon.Powers.size=0;
    tempMon.generatorsIndices.size=0;
    for (int j=0; j<currentMon.Powers.size; j++)
 //   for (int j=currentMon.Powers.size-1; j>=0; j--)
    { int thePower;
      if (!currentMon.Powers[j].IsSmallInteger(&thePower))
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

template <class CoefficientType>
bool ElementUniversalEnveloping<CoefficientType>::HWMTAbilinearForm
  (const ElementUniversalEnveloping<CoefficientType>& right, CoefficientType& output,
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
  { *logStream << "backtraced elt: " << MTright.ToString(&theGlobalVariables.theDefaultFormat) << "<br>";
    *logStream << "this element: " << this->ToString(&theGlobalVariables.theDefaultFormat) << "<br>";
   // for (int i=0; i<subHiGoesToIthElement->size; i++)
   // { *logStream << subHiGoesToIthElement->TheObjects[i].ToString();
   // }
  }
  for (int j=0; j<right.size; j++)
  { intermediateAccum=*this;
    intermediateAccum.Simplify(theGlobalVariables, theRingUnit, theRingZero);
    if (logStream!=0)
      *logStream << "intermediate after simplification: "
      << intermediateAccum.ToString(&theGlobalVariables.theDefaultFormat)
      << "<br>";
    intermediateAccum.ModOutVermaRelations(&theGlobalVariables, subHiGoesToIthElement, theRingUnit, theRingZero);
    MonomialUniversalEnveloping<CoefficientType>& rightMon=MTright[j];
    CoefficientType& rightMonCoeff=MTright.theCoeffs[j];
    int thePower;
    for (int i=rightMon.Powers.size-1; i>=0; i--)
      if (rightMon.Powers[i].IsSmallInteger(&thePower))
        for (int k=0; k<thePower; k++)
        { tempElt.MakeOneGenerator(rightMon.generatorsIndices[i], *this->owners, this->indexInOwners, theRingUnit);
          MathRoutines::swap(tempElt, intermediateAccum);
          if (logStream!=0)
          { *logStream << "tempElt before mult: " << tempElt.ToString(&theGlobalVariables.theDefaultFormat) << "<br>";
            *logStream << "intermediate before mult: " << intermediateAccum.ToString(&theGlobalVariables.theDefaultFormat) << "<br>";
          }
          intermediateAccum*=(tempElt);
          if (logStream!=0)
            *logStream << "intermediate before simplification: " << intermediateAccum.ToString(&theGlobalVariables.theDefaultFormat) << "<br>";
          intermediateAccum.Simplify(theGlobalVariables, theRingUnit, theRingZero);
          if (logStream!=0)
            *logStream << "intermediate after simplification: " << intermediateAccum.ToString(&theGlobalVariables.theDefaultFormat) << "<br>";
          intermediateAccum.ModOutVermaRelations(&theGlobalVariables, subHiGoesToIthElement, theRingUnit, theRingZero);
          if (logStream!=0)
            *logStream << "intermediate after Verma rels: " << intermediateAccum.ToString(&theGlobalVariables.theDefaultFormat) << "<br>";
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
    *logStream << "final UE element: " << Accum.ToString(&theGlobalVariables.theDefaultFormat);
  return true;
}

template <class CoefficientType>
std::string ElementUniversalEnveloping<CoefficientType>::IsInProperSubmodule
(const Vector<CoefficientType>* subHiGoesToIthElement, GlobalVariables& theGlobalVariables, const CoefficientType& theRingUnit,
 const CoefficientType& theRingZero)
{ std::stringstream out;
  List<ElementUniversalEnveloping<CoefficientType> > theOrbit;
  theOrbit.ReservE(1000);
  ElementUniversalEnveloping<CoefficientType> theElt;
  int theDim=this->GetOwner().GetRank();
  int numPosRoots=this->GetOwner().GetNumPosRoots();
  theOrbit.AddOnTop(*this);
  for (int i=0; i<theOrbit.size; i++)
    for (int j=0; j<theDim; j++)
    { theElt.MakeOneGenerator(j+numPosRoots+theDim, *this->owneR, theRingUnit);
      theElt*=(theOrbit[i]);
      theElt.Simplify(theGlobalVariables, theRingUnit, theRingZero);
      theElt.ModOutVermaRelations(&theGlobalVariables, subHiGoesToIthElement, theRingUnit, theRingZero);
//      theElt.Simplify(theGlobalVariables, theRingUnit, theRingZero);
      if (!theElt.IsEqualToZero())
        theOrbit.AddOnTop(theElt);
    }
  for (int i=0; i< theOrbit.size; i++)
  { ElementUniversalEnveloping<CoefficientType>& current=theOrbit[i];
    out << "<br>" << current.ToString(&theGlobalVariables.theDefaultFormat);
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
      if (theOrbit.AddOnTopNoRepetition(tempRoot))
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

void ProjectInformation::AddProjectInfo(const std::string& fileName, const std::string& fileDescription)
{ if (this->FileNames.Contains(fileName))
    return;
  this->FileNames.AddOnTop(fileName);
  this->FileDescriptions.AddOnTop(fileDescription);
}

std::string CGI::GetHtmlLinkFromProjectFileName(const std::string& fileName, const std::string& fileDesc)
{ std::stringstream out;
  out << " <a href=\"https://sourceforge.net/p/vectorpartition/code/HEAD"
  << "/tree/trunk/"
  << CGI::RemovePathFromFileName(fileName) << "\">"
  << CGI::RemovePathFromFileName(fileName);
  if (fileDesc!="")
    out << " (" << fileDesc << ")";
  out << "</a>\n";
  return out.str();
}

std::string ProjectInformation::ToString()
{ std::stringstream out;
  out << "<button " << CGI::GetStyleButtonLikeHtml()
  << " onclick=\"switchMenu('sourceDetails');\" >C++ source of the calculator "
  << "(expand/collapse)</button>";
  out << "<div id=\"sourceDetails\" style=\"display: none\">";
  for (int i=0; i<this->FileNames.size; i++)
  { out << " <br>\n";
    out << CGI::GetHtmlLinkFromProjectFileName
    (this->FileNames[i], this->FileDescriptions[i]);
  }
  out << "<br>The calculator is a simple console application (like the C++ \"Hello world!\")."
  << " It is managed by an <a href=\"http://httpd.apache.org/\">Apache web server</a>. ";
  out << " <br>The calculator errors get caught either by 1) in-line asserts() (blank screen), "
  << "or 2) by Apache/the system (internal server error)."
  << "  \n  <br> The file input/output is done via std::fstream. "
  << "<br>The LaTeX'ing is called using std::system() "
  << " calls. The LaTeX logs can be found by viewing the calculator page's source. <br> "
  << "The html output is hardcoded. ";
  out << " ";
  out << " \n";
  out << "<hr><b>Installing the calculator on your machine from c++ source. </b><br> "
  << "In order to get the calculator running on your machine you need to do the following. "
  << " Simplifying the installation procedure is on our to-do list. "
  << "<br>0) You need a Linux machine. Tested it only on Ubuntu and OpenSUSE. "
  << "If you are interested in making the system run on Windows please write us an email. "
  << "<br>1) Download the c++ files in the links above. Alternatively you "
  << " can check out the program's "
  << " svn repository. The svn check out command is  &nbsp&nbsp"
  << " svn checkout svn://svn.code.sf.net/p/vectorpartition/code/trunk vectorpartition-code"
  << " &nbsp&nbsp."
  << "<br>2) Put them in a c++ project and make sure the following includes work:"
  << " #include &lt;sys/time.h&gt; #include &lt;unistd.h&gt; #include &lt;pthread.h&gt;. "
  << "They should work by default on almost any Linux distro. "
  << "<br>3) Build the project to a console application named  &nbsp&nbsp "
  << "calculator &nbsp&nbsp with default console application settings.  "
  << "<br>4) Create a folder to contain the server files; assume without loss of "
  << "generality the so created folder is called "
  << "&nbsp&nbsp ServerBase/    &nbsp&nbsp."
  << "<br>5) Create folders &nbsp&nbsp ServerBase/cgi-bin  "
  << "&nbsp&nbsp and &nbsp&nbsp ServerBase/output &nbsp&nbsp. "
  << "<br>6) Enable full read/write access for every user in the folder "
  << "&nbsp&nbsp ServerBase/output  &nbsp&nbsp. "
  << "<br>7) Copy the file &nbsp&nbsp calculator  &nbsp&nbsp into &nbsp&nbsp "
  << "ServerBase/cgi-bin/ &nbsp&nbsp and allow read/execute access to every user."
  << "<br> 8) Install an <a href=\"http://httpd.apache.org/\">Apache web server</a> "
  << "and enable cgi scripts "
  << "from folder &nbsp&nbsp ServerBase/cgi-bin/ &nbsp&nbsp."
  << "<br>9) Configure the Apache server so the address of physical folder "
  << "&nbsp&nbsp ServerBase/ &nbsp&nbsp is displayed as &nbsp&nbsp /ServerBase/ &nbsp&nbsp."
  << "<br>10) The basic installation is now complete; test the calculator by "
  << "running it through your web browser."
  << "<br>11) To finish the installation install the jsmath in folder &nbsp&nbsp "
  << "ServerBase/jsmath/ &nbsp&nbsp.";
  out <<	"</div>";

  return out.str();
}

std::string CGI::GetLatexEmbeddableLinkFromCalculatorInput
(const std::string& address, const std::string& display)
{ std::stringstream out;
  out << "\\href{http://vector-partition.jacobs-university.de/vpf/cgi-bin/calculator?";
  for (unsigned i=0; i<address.size(); i++)
  { //if (input[i]=='&')
     // out << "\\&";
    //else
    if (address[i]=='%')
      out << "\\%";
    else if (address[i]=='_')
      out << "\\_";
    else
      out << address[i];
  }
  out << "}{";
  for (unsigned i=0; i<display.size(); i++)
  { //if (input[i]=='&')
     // out << "\\&";
    //else
    if (display[i]=='%')
      out << "\\%";
    else if (display[i]=='_')
      out << "\\_";
    else if (display[i]=='{')
      out << "\\{";
    else if (display[i]=='}')
      out << "\\}";
    else
      out << display[i];
  }
  out << "}";
  return out.str();
}

std::string CGI::GetHtmlMathSpanNoButtonAddBeginArrayL(const std::string& input)
{ std::stringstream out;
  out << "<span class=\"math\">\\begin{array}{l}" << input << "\\end{array} </span>";
  return out.str();
}

std::string CGI::GetCalculatorLink
(const std::string& DisplayNameCalculator, const std::string& input)
{ std::stringstream out;
  out << "<a href=\"" << DisplayNameCalculator
  << "?textInput=" << CGI::UnCivilizeStringCGI(input)
  << "\"> " << input << "</a>";
  return out.str();
}

std::string CGI::GetHtmlMathSpanPure(const std::string& input, int upperNumChars)
{ std::stringstream out;
  if (input.size()< (unsigned) upperNumChars)
    out << "<span class=\"math\">" << input << "</span>";
  else
    out << "<b>LaTeX output is long and I dare not use jsmath. "
    << "Here is the output as plain LaTeX.</b><br> " << input;
  return out.str();
}

void branchingData::initAssumingParSelAndHmmInittedPart1NoSubgroups
(GlobalVariables& theGlobalVariables)
{ MacroRegisterFunctionWithName("branchingData::initAssumingParSelAndHmmInittedPart1NoSubgroups");
  this->WeylFDSmallAsSubInLarge.AmbientWeyl=this->theHmm.theRange().theWeyl;
  this->WeylFDSmall.AmbientWeyl=this->theHmm.theDomain().theWeyl;
  this->WeylFD.AmbientWeyl=this->theHmm.theRange().theWeyl;
  this->selSmallParSel.init(WeylFDSmall.AmbientWeyl.GetDim());
  for (int i=0; i<this->theHmm.ImagesCartanDomain.size; i++)
  { Vector<Rational>& currentV=this->theHmm.ImagesCartanDomain[i];
    this->generatorsSmallSub.AddOnTop(currentV);
    for (int j=0; j<currentV.size; j++)
      if (!currentV[j].IsEqualToZero() && this->selInducing.selected[j])
      { this->generatorsSmallSub.RemoveLastObject();
        this->selSmallParSel.AddSelectionAppendNewIndex(i);
        break;
      }
  }
  this->NilModPreNil.SetSize(0);
  this->nilradicalSmall.SetSize(0);
  this->nilradicalLarge.SetSize(0);
  this->weightsNilradicalLarge.SetSize(0);
  this->weightsNilradicalSmall.SetSize(0);
  this->weightsNilModPreNil.SetSize(0);
  this->indicesNilradicalLarge.SetSize(0);
  this->indicesNilradicalSmall.SetSize(0);
  ElementSemisimpleLieAlgebra<Rational> tempElt;
  WeylGroup& theLargeWeyl=this->theHmm.theRange().theWeyl;
  WeylGroup& theSmallWeyl=this->theHmm.theDomain().theWeyl;
  int numB3NegGenerators=this->theHmm.theRange().GetNumPosRoots();
  int numG2NegGenerators=this->theHmm.theDomain().GetNumPosRoots();
  for (int i=0; i<numB3NegGenerators; i++)
  { Vector<Rational>& currentWeight=theLargeWeyl.RootSystem[i];
    bool isInNilradical=false;
    for (int k=0; k<this->selInducing.CardinalitySelection; k++)
      if (!currentWeight[this->selInducing.elements[k]].IsEqualToZero())
      { isInNilradical=true;
        break;
      }
    if (isInNilradical)
    { this->weightsNilradicalLarge.AddOnTop(currentWeight);
      tempElt.MakeGenerator(i, this->theHmm.theRange());
      this->nilradicalLarge.AddOnTop(tempElt);
      this->indicesNilradicalLarge.AddOnTop(i);
    }
  }
  for (int i=0; i<numG2NegGenerators; i++)
  { Vector<Rational>& currentWeight=theSmallWeyl.RootSystem[i];
    bool isInNilradical=false;
    for (int k=0; k<this->selSmallParSel.CardinalitySelection; k++)
      if (!currentWeight[this->selSmallParSel.elements[k]].IsEqualToZero())
      { isInNilradical=true;
        break;
      }
    if (isInNilradical)
    { this->weightsNilradicalSmall.AddOnTop(currentWeight);
      tempElt.MakeGenerator(i, this->theHmm.theDomain());
      this->nilradicalSmall.AddOnTop(tempElt);
      this->indicesNilradicalSmall.AddOnTop(i);
    }
  }
//  std::cout << "<br>call stack look who is callng me: "
//  << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  this->NilModPreNil=this->nilradicalLarge;
  this->weightsNilModPreNil=this->weightsNilradicalLarge;
  Vector<Rational> proj;
  for (int i=0; i<this->nilradicalSmall.size; i++)
  { ElementSemisimpleLieAlgebra<Rational>& eltImage=
    this->theHmm.imagesAllChevalleyGenerators[this->indicesNilradicalSmall[i]];
    int theIndex=this->NilModPreNil.GetIndex(eltImage);
    if (theIndex!=-1)
    { this->NilModPreNil.RemoveIndexSwapWithLast(theIndex);
      this->weightsNilModPreNil.RemoveIndexSwapWithLast(theIndex);
      continue;
    }
    bool isGood=false;
    for (int j=0; j<this->weightsNilModPreNil.size; j++)
    { proj=this->ProjectWeight(this->weightsNilModPreNil[j]);
      if (proj==this->weightsNilradicalSmall[i])
      { isGood=true;
        this->NilModPreNil.RemoveIndexSwapWithLast(j);
        this->weightsNilModPreNil.RemoveIndexSwapWithLast(j);
        break;
      }
    }
    if (!isGood)
    { std::cout << "This is either a programming error, or Lemma 3.3, "
      << "T. Milev, P. Somberg, \"On branching...\""
      << " is wrong. The question is, which is the more desirable case... "
      << "The bad apple is element "
      << this->nilradicalSmall[i].ToString() << " of weight "
      << this->weightsNilradicalSmall[i].ToString()
      << ". " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
  }
/*  std::cout << "<br>large nilradical: " << this->nilradicalLarge.ToString();
  std::cout  << "<br>large nilradical weights: " << this->weightsNilradicalLarge.ToString();
  std::cout << "<br>small nilradical: " << this->nilradicalSmall.ToString();
  std::cout  << "<br>small nilradical weights: " << this->weightsNilradicalSmall.ToString();
  std::cout << "<br>Nil mod pre-nil (Lemma 3.3): " << this->NilModPreNil.ToString();
  std::cout  << "<br>Nil mod pre-nil weights: " << this->weightsNilModPreNil.ToString();*/
}

void branchingData::initAssumingParSelAndHmmInittedPart2Subgroups(GlobalVariables& theGlobalVariables)
{ List<Vectors<Rational> > emptyList;
  this->WeylFDSmallAsSubInLarge.ComputeSubGroupFromGeneratingReflections
  (&this->generatorsSmallSub, &emptyList, &theGlobalVariables, 1000, true);
  this->WeylFDSmall.MakeParabolicFromSelectionSimpleRoots
  (this->WeylFDSmall.AmbientWeyl, this->selSmallParSel, theGlobalVariables, 1000);
  this->WeylFD.MakeParabolicFromSelectionSimpleRoots
  (this->theHmm.theRange().theWeyl, this->selInducing, theGlobalVariables, 1000);

  //  std::cout << "Splitting parabolic selection: " << splittingParSel.ToString();
  //outputWeylSub.outputFDactingWeyl(this->GetOwner().theWeyl, splittingParSel, theGlobalVariables,1);
  this->WeylFD.ComputeRootSubsystem();
  this->WeylFDSmallAsSubInLarge.ComputeRootSubsystem();
  this->WeylFDSmall.ComputeRootSubsystem();
}

std::string branchingData::GetStringCasimirProjector(int theIndex, const Rational& additionalMultiple)
{ Vector<RationalFunctionOld> weightDifference;
  std::stringstream formulaStream1;
  HashedList<Vector<RationalFunctionOld> > accountedDiffs;
  accountedDiffs.SetExpectedSize(this->g2Weights.size);
  bool found=false;
  for (int i=0; i<this->g2Weights.size; i++)
  { weightDifference= this->g2Weights[i]-this->g2Weights[theIndex];
    if (weightDifference.IsPositive() && !accountedDiffs.Contains(weightDifference))
    { accountedDiffs.AddOnTop(weightDifference);
      if (additionalMultiple!=1)
        formulaStream1 << additionalMultiple.ToString(&this->theFormat);
      formulaStream1 << "(i(\\bar c) - (" << this->theChars[i].ToString(&this->theFormat) << "))";
      found=true;
    }
  }
  if (!found)
    formulaStream1 << "id";
  return formulaStream1.str();
}

bool LittelmannPath::IsAdaptedString
  (MonomialTensor<int, MathRoutines::IntUnsignIdentity>& theString)
{ LittelmannPath tempPath=*this;
  LittelmannPath tempPath2;
//  std::cout << "<hr>";
  for (int i=0; i<theString.generatorsIndices.size; i++)
  { //std::cout << "e_" << -theString.generatorsIndices[i] << "^"
    //<< theString.Powers[i] << "(" << tempPath.ToString() << ") =";
    for (int k=0; k<theString.Powers[i]; k++)
      tempPath.ActByEalpha(-theString.generatorsIndices[i]-1);
    if (tempPath.IsEqualToZero())
      return false;
    tempPath2=tempPath;
    tempPath2.ActByEalpha(-theString.generatorsIndices[i]-1);
    //std::cout << tempPath.ToString();
    if (!tempPath2.IsEqualToZero())
      return false;
    //if (i!=theString.generatorsIndices.size-1)
    //  std::cout << "<br>";
  }
  return true;
}

void ReflectionSubgroupWeylGroup::GetGroupElementsIndexedAsAmbientGroup
(List<ElementWeylGroup>& output)
{ if (this->ExternalAutomorphisms.size>0)
  { std::cout << "This is  a programming error: a function meant for subgroups that "
    << " are Weyl groups of Levi parts of parabolics is called on a subgroup that "
    << "is not of that type. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  output.ReservE(this->size);
  output.SetSize(0);
  ElementWeylGroup tempElt;
  Vector<int> indexShifts;
  indexShifts.SetSize(this->simpleGenerators.size);
  for (int i=0; i<this->simpleGenerators.size; i++)
    indexShifts[i]=this->simpleGenerators[i].GetIndexFirstNonZeroCoordinate();
  for (int i=0; i<this->size; i++)
  { tempElt=(*this)[i];
    for (int j=0; j<tempElt.size; j++)
      tempElt[j]= indexShifts[tempElt[j]];
    output.AddOnTop(tempElt);
  }
}

std::string LittelmannPath::ToString(bool useSimpleCoords, bool useArrows, bool includeDominance)const
{ if (this->Waypoints.size==0)
    return "0";
  std::stringstream out;
  for (int i=0; i<this->Waypoints.size; i++)
  { if (useSimpleCoords)
      out << this->Waypoints[i].ToString();
    else
      out << this->owner->GetFundamentalCoordinatesFromSimple(this->Waypoints[i]).ToString();
    if (i!=this->Waypoints.size-1)
    { if (useArrows)
        out << "->";
      else
        out << ",";
    }
  }
  if (includeDominance)
  { List<int> dominantIndices;
    Vector<Rational> tempV;
    out << " ";
    for (int i=0; i<this->owner->GetDim(); i++)
    { LittelmannPath tempP=*this;
      tempP.ActByEFDisplayIndex(i+1);
      if (!tempP.IsEqualToZero())
        out << "e_{" << i+1 << "}";
      tempP=*this;
      tempP.ActByEFDisplayIndex(-i-1);
      if (!tempP.IsEqualToZero())
        out << "e_{" << -i-1 << "},";
    }
  }
  return out.str();
}
