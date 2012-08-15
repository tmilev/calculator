//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader1.h"
#include "vpfHeader1_2.h"

ProjectInformationInstance ProjectInfoVpf7cpp(__FILE__, "Implementation file intended for small fast changes. ");

template<class CoefficientType>
Rational ModuleSSalgebra<CoefficientType>::hwTrace
(const Pair
 <TensorMonomial<int, MathRoutines::IntUnsignIdentity>,
 TensorMonomial<int, MathRoutines::IntUnsignIdentity> >
 & thePair
   )
{ MacroRegisterFunctionWithName("ModuleSSalgebra<CoefficientType>::hwTrace");
  int indexInCache=this->cachedPairs.GetIndex(thePair);
  if (indexInCache!=-1)
    return this->cachedTraces[indexInCache];
  if (thePair.Object1.generatorsIndices.size==0)
    return 1;
  Pair<TensorMonomial<int, MathRoutines::IntUnsignIdentity>,
  TensorMonomial<int, MathRoutines::IntUnsignIdentity> > newPair;
  TensorMonomial<int, MathRoutines::IntUnsignIdentity>& newLeft=newPair.Object1;
  TensorMonomial<int, MathRoutines::IntUnsignIdentity>& newRight=newPair.Object2;
  const TensorMonomial<int, MathRoutines::IntUnsignIdentity>& oldRight=thePair.Object2;
  newLeft=thePair.Object1;
  (*newLeft.Powers.LastObject())-=1;
  int theIndex= *newLeft.generatorsIndices.LastObject();
  if (*newLeft.Powers.LastObject()==0)
  { newLeft.generatorsIndices.size--;
    newLeft.Powers.size--;
  }
  int theIndexMinus=
  2*this->GetOwner().GetNumPosRoots() + this->GetOwner().GetRank()-theIndex-1;
  int theSimpleIndex= theIndex-this->GetOwner().GetNumPosRoots()-this->GetOwner().GetRank();
  TensorMonomial<int, MathRoutines::IntUnsignIdentity> Accum;
  Accum.Powers.ReservE(oldRight.Powers.size);
  Accum.generatorsIndices.ReservE(oldRight.generatorsIndices.size);
  Vector<Rational> RemainingWeight;
  Rational result=0;
  Rational summand;
  WeylGroup& theWeyl=this->GetOwner().theWeyl;
//  std::stringstream tempStr;
//  tempStr << thePair.Object2;
//  if (tempStr.str()=="g_{1}^{1}g_{2}^{1}")
//    std::cout << "<hr><hr>";
//  std::cout << "<br>Computing (" << thePair.Object1 << ", " << thePair.Object2 << ")";

  for (int i=0; i<oldRight.generatorsIndices.size; i++)
  { if (oldRight.generatorsIndices[i]==theIndexMinus)
    { summand=0;
      newRight=Accum;
      newRight.MultiplyByGeneratorPowerOnTheRight
      (oldRight.generatorsIndices[i], oldRight.Powers[i]-1);
      RemainingWeight.MakeZero(theWeyl.GetDim());
      for (int j=i+1; j<oldRight.generatorsIndices.size; j++)
      { newRight.MultiplyByGeneratorPowerOnTheRight(oldRight.generatorsIndices[j], oldRight.Powers[j]);
        RemainingWeight+=theWeyl.RootSystem[oldRight.generatorsIndices[j]]* oldRight.Powers[j];
      }
      RemainingWeight+=this->theHWSimpleCoordS;
//      std::cout << "<br>Remaining weight: " << RemainingWeight.ToString();
      summand+=theWeyl.GetScalarProdSimpleRoot(RemainingWeight, theSimpleIndex);
      summand*=2;
      summand/=theWeyl.CartanSymmetric.elements[theSimpleIndex][theSimpleIndex];
//      std::cout << "<br>The scalar product: " << summand.ToString();
      summand+=1;
      summand-=oldRight.Powers[i];
 //     std::cout << "<br>summand: " << summand.ToString();
      if (!summand.IsEqualToZero())
        summand*=this->hwTrace(newPair);
//      std::cout << "<br>summand after recursion: " << summand.ToString();
      summand*=oldRight.Powers[i];
      result+=summand;
    }
    Accum.generatorsIndices.AddOnTop(oldRight.generatorsIndices[i]);
    Accum.Powers.AddOnTop(oldRight.Powers[i]);
  }
  this->cachedPairs.AddOnTop(thePair);
  this->cachedTraces.AddOnTop(result);
//  std::cout << "<br>Computed (" << thePair.Object1 << ", "
//  << thePair.Object2 << ")=" << result.ToString();
//  if (tempStr.str()=="g_{1}^{1}g_{2}^{1}")
//    std::cout << "<hr><hr>";

  return result;
}

template<class CoefficientType>
void ModuleSSalgebra<CoefficientType>::ApplyTAA
(TensorMonomial<int, MathRoutines::IntUnsignIdentity>& theMon)
{ for (int i=0; i<theMon.generatorsIndices.size; i++)
    theMon.generatorsIndices[i]=
    this->GetOwner().GetNumPosRoots()*2+
    this->GetOwner().GetRank()-theMon.generatorsIndices[i]-1;
  theMon.Powers.ReverseOrderElements();
  theMon.generatorsIndices.ReverseOrderElements();
}

template<class CoefficientType>
Rational ModuleSSalgebra<CoefficientType>::hwtaabfSimpleGensOnly
  (const TensorMonomial<int, MathRoutines::IntUnsignIdentity>& leftMon,
   const TensorMonomial<int, MathRoutines::IntUnsignIdentity>& rightMon)
{ MacroRegisterFunctionWithName("ModuleSSalgebra<CoefficientType>::hwtaabfSimpleGensOnly");
  const TensorMonomial<int, MathRoutines::IntUnsignIdentity>* left=&leftMon;
  const TensorMonomial<int, MathRoutines::IntUnsignIdentity>* right=&rightMon;
  if (leftMon>rightMon)
    MathRoutines::swap(left, right);
  Pair<TensorMonomial<int, MathRoutines::IntUnsignIdentity>,
  TensorMonomial<int, MathRoutines::IntUnsignIdentity> > thePair;
  thePair.Object1=*left;
  thePair.Object2=*right;
//  std::cout << "<br>Computing " << thePair.Object1 << " times " << thePair.Object2 << "<br>";
  this->ApplyTAA(thePair.Object1);
  return this->hwTrace(thePair);
}

void SemisimpleLieAlgebra::GetChevalleyGeneratorAsLieBracketsSimpleGens
  (int generatorIndex, List<int>& outputIndicesFormatAd0Ad1Ad2etc,
   Rational& outputMultiplyLieBracketsToGetGenerator)
{ outputIndicesFormatAd0Ad1Ad2etc.size=0;
  if (this->IsGeneratorFromCartan(generatorIndex))
  { int simpleIndex=generatorIndex-this->GetNumPosRoots();
    outputIndicesFormatAd0Ad1Ad2etc.AddOnTop(generatorIndex+this->GetRank());
    outputIndicesFormatAd0Ad1Ad2etc.AddOnTop( 2*this->GetNumPosRoots()-1-generatorIndex);
    outputMultiplyLieBracketsToGetGenerator=this->theWeyl.CartanSymmetric.elements[simpleIndex][simpleIndex]/2;
    return;
  }
  Vector<Rational> theWeight=this->GetWeightOfGenerator(generatorIndex);
  outputMultiplyLieBracketsToGetGenerator=1;
  int theIndex=-1;
  while (!theWeight.IsEqualToZero())
    for (int i=0; i<this->GetRank(); i++)
    { bool isChanged=false;
      if (theWeight.IsPositive())
        if (this->theWeyl.IsARoot(theWeight-this->theWeyl.RootsOfBorel[i]))
        { theWeight-=this->theWeyl.RootsOfBorel[i];
          theIndex=i+this->GetNumPosRoots()+this->GetRank();
          isChanged=true;
        }
      if (theWeight.IsNegative())
        if (this->theWeyl.IsARoot(theWeight-this->theWeyl.RootsOfBorel[i]))
        { theWeight+=this->theWeyl.RootsOfBorel[i];
          theIndex=-i-1+this->GetNumPosRoots();
          isChanged=true;
        }
      if (isChanged)
      { int currentIndex=this->GetChevalleyGeneratorIndexCorrespondingToNonZeroRootSpace(theWeight);
        outputIndicesFormatAd0Ad1Ad2etc.AddOnTop(theIndex);
        outputMultiplyLieBracketsToGetGenerator/=this->ChevalleyConstants.elements[theIndex][currentIndex];
      }
    }
}

template<class CoefficientType>
void ModuleSSalgebra<CoefficientType>::TestConsistency(GlobalVariables& theGlobalVariables)
{ MatrixTensor<CoefficientType> left, right, output;
  for (int i=0; i<this->GetOwner().GetNumPosRoots(); i++)
  { left = this->GetActionGeneratorIndeX
    (this->GetOwner().DisplayIndexToChevalleyGeneratorIndex(i+1) , theGlobalVariables);
    right = this->GetActionGeneratorIndeX
    (this->GetOwner().DisplayIndexToChevalleyGeneratorIndex(-i-1) , theGlobalVariables);
    right.LieBracketOnTheLeft(left);
    output=left;
    output.LieBracketOnTheLeft(right);
    output/=2;
    output-=left;
    if(output.IsEqualToZero())
    { std::cout << "<br>index " << i << " is good ";
    } else
    { std::cout << "<br>index " << i << " is bad.";
    }
  }
}

template<class CoefficientType>
bool ModuleSSalgebra<CoefficientType>::MakeFromHW
(List<SemisimpleLieAlgebra>& inputAlgebras, int inputIndexAlgebra, Vector<CoefficientType>& HWFundCoords,
 const Selection& selNonSelectedAreElementsLevi, GlobalVariables& theGlobalVariables,
const CoefficientType& theRingUnit, const CoefficientType& theRingZero,
 std::string* outputReport, bool computeSimpleGens)
{ MacroRegisterFunctionWithName("ModuleSSalgebra<CoefficientType>::MakeFromHW");

  this->theAlgebras=&inputAlgebras;
  this->indexAlgebra=inputIndexAlgebra;
  SemisimpleLieAlgebra& theAlgebrA=inputAlgebras[this->indexAlgebra];

  int theRank=theAlgebrA.GetRank();
  if (HWFundCoords.size!=theRank || selNonSelectedAreElementsLevi.MaxSize!=theRank)
  { std::cout << "This is a programming error. I am asked to create a generalized Verma module "
    << " with a semisimple Lie algebra of rank " << theRank << " but the input highest weight, "
    << HWFundCoords.ToString() << ", has " << HWFundCoords.size << " coordinates and "
    << " the parabolic section indicates rank of " << selNonSelectedAreElementsLevi.MaxSize
    << ". " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  WeylGroup& theWeyl=theAlgebrA.theWeyl;

  this->parabolicSelectionNonSelectedAreElementsLevi=selNonSelectedAreElementsLevi;
  this->parabolicSelectionSelectedAreElementsLevi=this->parabolicSelectionNonSelectedAreElementsLevi;
  this->parabolicSelectionSelectedAreElementsLevi.InvertSelection();

  this->theHWFundamentalCoordsBaseField=HWFundCoords;
  this->theHWDualCoordsBaseFielD.SetSize(theRank);
  this->theHWFundamentalCoordS.SetSize(theRank);

  for (int i=0; i<theRank; i++)
  { this->theHWFundamentalCoordS[i]=0;
    if (this->parabolicSelectionSelectedAreElementsLevi.selected[i])
    { int theCoord;
      if (!this->theHWFundamentalCoordsBaseField[i].IsSmallInteger(&theCoord))
      { if (outputReport!=0)
          *outputReport="The given module over the Levi part is not finite dimensional";
        return false;
      }
      this->theHWFundamentalCoordS[i]=theCoord;
    }
    this->theHWDualCoordsBaseFielD[i]=this->theHWFundamentalCoordsBaseField[i];
    this->theHWDualCoordsBaseFielD[i]*=theWeyl.CartanSymmetric.elements[i][i]/2;
  }

  this->theHWSimpleCoordS=theWeyl.GetSimpleCoordinatesFromFundamental(this->theHWFundamentalCoordS);
  this->theHWDualCoords= theWeyl.GetDualCoordinatesFromFundamental(this->theHWFundamentalCoordS);
  this->theHWSimpleCoordSBaseField=theWeyl.GetSimpleCoordinatesFromFundamental(this->theHWFundamentalCoordsBaseField);
  this->theChaR.MakeFromWeight(this->theHWSimpleCoordSBaseField, *this->theAlgebras, this->indexAlgebra);

 // std::cout << "<br>input fund coords base field: " << HWFundCoords.ToString();
 // std::cout << "<br>dual coords no base field: " << this->theHWDualCoordS.ToString();
 // std::cout << "<br>dual coords: " << this->theHWDualCoordsBaseField.ToString();
 // std::cout << "<br>fund coords no base field: " << this->theHWFundamentalCoordS.ToString();
 // std::cout << "<br>fund coords: " << this->theHWFundamentalCoordsBaseField.ToString();
 // std::cout << "<br>simple coords no base field: " << this->theHWSimpleCoordS.ToString();
 // std::cout << "<br>parabolicSelectionNonSelectedAreElementsLevi: " << this->parabolicSelectionNonSelectedAreElementsLevi.ToString();
 // std::cout << "<br>parabolicSelectionSelectedAreElementsLevi: " << this->parabolicSelectionSelectedAreElementsLevi.ToString();

  LittelmannPath startingPath;
  startingPath.MakeFromWeightInSimpleCoords(this->theHWSimpleCoordS, theWeyl);
//  std::cout << "<br>starting littelmann path: " << startingPath.ToString() << this->parabolicSelectionNonSelectedAreElementsLevi.ToString();
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
//  std::cout << "<br>character over h (i.e. the set of weights with mults): " << this->theCharOverH.ToString();
//  std::cout << "<br>character: " << this->theChaR.ToString();
  this->theModuleWeightsSimpleCoords.QuickSortAscending();
  std::stringstream out2, monomialDetailStream;
  if (outputReport!=0)
  { monomialDetailStream << "<br>Number of Littelmann paths: " << thePaths.size;
    monomialDetailStream << "<br>Let v denote the highest weight vector of highest weight in simple coordinates "
    << this->theHWSimpleCoordS.ToString();
    std::string tempS;
    DrawingVariables theDV;
    this->theCharOverH.DrawMeAssumeCharIsOverCartan(theWeyl, theGlobalVariables, theDV);
    out2 << " A picture of the weight support follows. "
    << theDV.GetHtmlFromDrawOperationsCreateDivWithUniqueName(theWeyl.GetDim());
    monomialDetailStream << "<br>Then the elements corresponding to the Littelmann paths are as follows. ";
  }
  ElementUniversalEnveloping<CoefficientType> tempElt;
  this->theGeneratingWordsGrouppedByWeight.SetSize(this->theModuleWeightsSimpleCoords.size);
  this->theGeneratingWordsIntGrouppedByWeight.SetSize(this->theModuleWeightsSimpleCoords.size);
  for (int i=0; i<this->theGeneratingWordsGrouppedByWeight.size; i++)
  { this->theGeneratingWordsGrouppedByWeight[i].size=0;
    this->theGeneratingWordsIntGrouppedByWeight[i].size=0;
  }
  MonomialUniversalEnveloping<CoefficientType> currentNonReducedElement;
  TensorMonomial<int, MathRoutines::IntUnsignIdentity> tempMonInt;

  for (int i=0; i<thePaths.size; i++)
  { List<int>& currentPath= generatorsIndices[i];
    currentNonReducedElement.MakeConst(*this->theAlgebras, this->indexAlgebra);
    tempMonInt.MakeConst();
    for (int j=currentPath.size-1; j>=0; j--)
    { int theIndex=currentPath[j];
      if (theIndex>0)
        theIndex++;
      currentNonReducedElement.MultiplyByGeneratorPowerOnTheRight
      (this->GetOwner().DisplayIndexToChevalleyGeneratorIndex(theIndex), theRingUnit);
      tempMonInt.MultiplyByGeneratorPowerOnTheRight
      (this->GetOwner().DisplayIndexToChevalleyGeneratorIndex(theIndex), 1);
    }
    Vector<Rational>& hwCurrent=*thePaths[i].Waypoints.LastObject();
    int theIndex=this->theModuleWeightsSimpleCoords.GetIndex(hwCurrent);
    if (theIndex==-1)
    { //std::cout << "couldn't find weight : " << hwCurrent.ToString() << " in " << this->theModuleWeightsSimpleCoords.ToString();
      out2 << "Error: could not generate all weights in the weight support. Maybe they are too many? Allowed "
      << " # of weights is 10000";
      if (outputReport!=0)
        *outputReport=out2.str() + monomialDetailStream.str();
      return false;
    }
    this->theGeneratingWordsGrouppedByWeight[theIndex].AddOnTop(currentNonReducedElement);
    this->theGeneratingWordsIntGrouppedByWeight[theIndex].AddOnTop(tempMonInt);
  }
  this->theGeneratingWordsNonReduced.Clear();
  this->theGeneratingWordsNonReduced.SetExpectedSize(thePaths.size);
  this->theGeneratingWordsNonReducedInt.Clear();
  this->theGeneratingWordsNonReducedInt.SetExpectedSize(thePaths.size);
  this->theGeneratingWordsNonReduced.size=0;
  this->theGeneratingWordsNonReducedWeights.SetSize(0);
  this->theGeneratingWordsNonReducedWeights.ReservE(thePaths.size);
  int wordCounter=-1;
  for (int i=0; i<this->theGeneratingWordsGrouppedByWeight.size; i++)
  { List<MonomialUniversalEnveloping<CoefficientType> >& currentList=
    this->theGeneratingWordsGrouppedByWeight[i];
    List<TensorMonomial<int, MathRoutines::IntUnsignIdentity> >& currentListInt=
    this->theGeneratingWordsIntGrouppedByWeight[i];
    currentList.QuickSortDescending();
    currentListInt.QuickSortDescending();
    for (int j=0; j<currentList.size; j++)
    { wordCounter++;
      this->theGeneratingWordsNonReduced.AddOnTop(currentList[j]);
      this->theGeneratingWordsNonReducedInt.AddOnTop(currentListInt[j]);
      this->theGeneratingWordsNonReducedWeights.AddOnTop(this->theModuleWeightsSimpleCoords[i]);
      if (outputReport!=0)
      { monomialDetailStream << "<br>m_{ " << this->theGeneratingWordsNonReduced.size << "} := "
        << currentList[j].ToString(&theGlobalVariables.theDefaultFormat) << "  v_\\lambda";
      }
    }
  }
  this->IntermediateStepForMakeFromHW
  (this->theHWDualCoordsBaseFielD, theGlobalVariables, theRingUnit, theRingZero);
  bool isBad=false;
  if (outputReport!=0)
    for (int k=0; k<this->theBilinearFormsAtEachWeightLevel.size; k++)
    { Matrix<CoefficientType>& theBF=this->theBilinearFormsAtEachWeightLevel[k];
      Matrix<CoefficientType>& theBFinverted= this->theBilinearFormsInverted[k];
      if (outputReport!=0)
      { monomialDetailStream << "<hr>weight in simple coords: "
        << this->theModuleWeightsSimpleCoords[k].ToString();
        List<MonomialUniversalEnveloping<CoefficientType> >& currentList=this->theGeneratingWordsGrouppedByWeight[k];
        for (int i=0; i<currentList.size; i++)
        { MonomialUniversalEnveloping<CoefficientType>& currentElt=currentList[i];
          monomialDetailStream << "<br>monomial " << i+1 << ": "
          << currentElt.ToString(&theGlobalVariables.theDefaultFormat);
        }
        monomialDetailStream << "; Matrix of Shapovalov form associated to current weight level: <br> "
        << theBF.ToString(true, false) << " corresonding inverted matrix:<br>";
      }
      if (theBFinverted.NumRows>0)
      { if (outputReport!=0)
          monomialDetailStream << theBFinverted.ToString(true, false);
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
  ElementSemisimpleLieAlgebra tempSSElt;
  if (computeSimpleGens)
    for (int k=0; k<2; k++)
      for (int j=0; j<this->GetOwner().GetRank(); j++)
        if (this->parabolicSelectionSelectedAreElementsLevi.selected[j])
        { int theIndex=this->GetOwner().GetNumPosRoots()-j-1;
          if (k==1)
            theIndex=this->GetOwner().GetNumPosRoots()+this->GetOwner().GetRank()+j;
          tempSSElt.MakeGenerator(theIndex, *this->theAlgebras, this->indexAlgebra);
          if (outputReport!=0)
            out2 << "<hr>Simple generator: " << tempSSElt.ToString(&theGlobalVariables.theDefaultFormat);
          MatrixTensor<CoefficientType>& theMatrix
          =this->GetActionGeneratorIndeX(theIndex, theGlobalVariables, theRingUnit, theRingZero);
          std::stringstream tempStream;
          tempStream << "computing action simple generator index " << (2*k-1)*(j+1) << " ... ";
          theGlobalVariables.MakeProgressReport(tempStream.str(), 2);
          tempStream << " done!";
          theGlobalVariables.MakeProgressReport(tempStream.str(), 2);

          if (outputReport!=0)
            out2 << "<br>Matrix of elemenent in the m_i basis:<br>"
            << CGI::GetHtmlMathSpanFromLatexFormula(theMatrix.ToString());
      /*    for (int j=0; j<this->actionsSimpleGens[i].size; j++)
            for (int k=0; k<this->actionsSimpleGens[i][j].size; k++)
            { out << "<br>" << theSimpleGenerator.ToString(theGlobalVariables, tempFormat) << "\\cdot "
              << this->theGeneratingWordsGrouppedByWeight[j][k].ToString(false, false, theGlobalVariables, tempFormat)
              << "\\cdot v=" << this->actionsSimpleGens[i][j][k].ToString(theGlobalVariables, tempFormat)
              << "\\cdot v"
              ;
            }*/
        }
  Vector<CoefficientType> currentWeightFundCoords;
  Vector<CoefficientType> hwFundCoordsTrimmedBaseField;
  hwFundCoordsTrimmedBaseField=this->theHWFundamentalCoordS;
  ElementSemisimpleLieAlgebra tempSSelt;
  if (outputReport!=0)
  { std::stringstream latexTableStream;
    latexTableStream << "<hr>Ready copy +paste for your .tex file:<br> <br>"
    << "\\begin{tabular}{lll";
    for (int i=0; i<this->parabolicSelectionSelectedAreElementsLevi.CardinalitySelection; i++)
      latexTableStream <<"l";
    latexTableStream << "} \n\\hline\\hline \\multicolumn{"
    << this->parabolicSelectionSelectedAreElementsLevi.CardinalitySelection+3
    << "}{|c|}{ Highest weight $\\lambda="
    << this->theHWFundamentalCoordsBaseField.ToStringLetterFormat("\\omega")
    << "$}\\\\\\hline Element& weight & monomial expression";
    for (int i=0; i<this->parabolicSelectionSelectedAreElementsLevi.CardinalitySelection; i++)
    { int theIndex=this->parabolicSelectionSelectedAreElementsLevi.elements[i]
      + this->GetOwner().GetRank()+this->GetOwner().GetNumPosRoots();
      tempSSelt.MakeGenerator(theIndex, *this->theAlgebras, this->indexAlgebra);
      latexTableStream << "&action of $" << tempSSElt.ToString() << "$";
    }
    latexTableStream << "\\\\\n";
    int monCounter=0;
    for (int i=0; i<this->theGeneratingWordsGrouppedByWeight.size; i++)
      for (int j=0; j<this->theGeneratingWordsGrouppedByWeight[i].size; j++)
      { monCounter++;
        latexTableStream << "$m_{" << monCounter << "}$&";
        currentWeightFundCoords=theWeyl.GetFundamentalCoordinatesFromSimple(this->theModuleWeightsSimpleCoords[i]);
        latexTableStream << "\n$"
        << (this->theHWFundamentalCoordsBaseField
        +currentWeightFundCoords-hwFundCoordsTrimmedBaseField).ToStringLetterFormat("\\omega");
        std::string theMonString=this->theGeneratingWordsGrouppedByWeight[i][j].ToString(&theGlobalVariables.theDefaultFormat);
        if (theMonString=="1")
          theMonString="";
        latexTableStream << "$&$" << theMonString << "  v_\\lambda$";
        for (int s=0; s< this->parabolicSelectionSelectedAreElementsLevi.CardinalitySelection; s++)
        { int currentIndex=this->parabolicSelectionSelectedAreElementsLevi.elements[s] +this->GetOwner().GetRank()+this->GetOwner().GetNumPosRoots();
          MatrixTensor<CoefficientType>& theMat=this->GetActionGeneratorIndeX(currentIndex, theGlobalVariables, theRingUnit, theRingZero);
          bool foundMon=false;
          latexTableStream << "&$";
          for (int l=0; l< theMat.size; l++)
            if (theMat[l].dualIndex==monCounter)
            { std::string tempS1;
              tempS1= theMat.theCoeffs[l].ToString();
              if (tempS1=="1")
                tempS1="";
              if (tempS1=="-1")
                tempS1="-";
              std::stringstream tempStream;
              tempStream << "m_{" << theMat[l].vIndex+1 << "}";
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
//  std::cout << "<hr>MakeHW result: <br>" << this->ToString();
  this->TestConsistency(theGlobalVariables);
  theGlobalVariables.MakeStatusReport("Done with module generation");
  return true;
}

template<class CoefficientType>
void ModuleSSalgebra<CoefficientType>::IntermediateStepForMakeFromHW
  (Vector<CoefficientType>& HWDualCoordS, GlobalVariables& theGlobalVariables,
    const CoefficientType& theRingUnit, const CoefficientType& theRingZero
   )
{ MacroRegisterFunctionWithName("ModuleSSalgebra<CoefficientType>::IntermediateStepForMakeFromHW");
  Vector<Rational> targetWeight;
  this->theBilinearFormsAtEachWeightLevel.SetSize(this->theGeneratingWordsGrouppedByWeight.size);
  this->theBilinearFormsInverted.SetSize(this->theGeneratingWordsGrouppedByWeight.size);
  this->ComputedGeneratorActions.init(this->GetOwner().GetNumGenerators());
  this->actionsGeneratorS.SetSize(this->GetOwner().GetNumGenerators());
  this->actionsGeneratorsMaT.SetSize(this->GetOwner().GetNumGenerators());
  int numScalarProducts=0;
  for (int l=0; l<this->theGeneratingWordsGrouppedByWeight.size; l++)
  { Matrix<CoefficientType>& currentBF=this->theBilinearFormsAtEachWeightLevel[l];
    List<MonomialUniversalEnveloping<CoefficientType> >& currentWordList=
    this->theGeneratingWordsGrouppedByWeight[l];
    List<TensorMonomial<int, MathRoutines::IntUnsignIdentity> >& currentWordListInt=
    this->theGeneratingWordsIntGrouppedByWeight[l];
    currentBF.init(currentWordList.size, currentWordList.size);
    for (int i=0; i<currentWordList.size; i++)
      for (int j=i; j<currentWordList.size; j++)
      { //std::cout << "<br>word " << i+1 << ": " << currentWordList[i].ToString(&theGlobalVariables.theDefaultLieFormat);
        //std::cout << "<br>word " << j+1 << ": " << currentWordList[j].ToString(&theGlobalVariables.theDefaultLieFormat);
          std::stringstream tempStream;
          tempStream << " Computing Shapovalov form layer " << l << " out of " << this->theGeneratingWordsGrouppedByWeight.size
          << " between indices " << i +1 << " and " << j+1 << " out of " << currentWordList.size;
        //currentWordList[i].HWTAAbilinearForm
        //(currentWordList[j], currentBF.elements[i][j], &HWDualCoordS, theGlobalVariables, theRingUnit, theRingZero, 0)
        //;
        numScalarProducts++;
        currentBF.elements[i][j]= this->hwtaabfSimpleGensOnly(currentWordListInt[i], currentWordListInt[j]);
//        std::cout << "<br> (" << currentWordList[i].ToString()
//        << ", " << currentWordList[j].ToString() << ")=  Old: " << currentBF.elements[i][j].ToString()
//        << "&nbsp    New: " << tempI.ToString();
          theGlobalVariables.MakeProgressReport(tempStream.str(), 1);
//        std::cout << "[" << currentWordList[i].ToString() << ", " << currentWordList[j].ToString() << "]=" <<currentBF.elements[i][j].ToString();
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
  std::cout << "<br>Number of shap. products cached: " << this->cachedPairs.size << "; dimension : "
  << this->GetDim() << " total scalar products computed: " << numScalarProducts;
  ;
  for (int i=0; i<this->cachedPairs.size; i++)
  { std::cout << "<br>(" << this->cachedPairs[i].Object1 << ", " << this->cachedPairs[i].Object2
    << ") = " << this->cachedTraces[i];
  }
}

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
  if (inputWeightSimpleCoords.size!=this->GetOwner().GetRank())
  { std::cout << "This is a programming error: attempting to create a character from highest weight in simple coords "
    << inputWeightSimpleCoords.ToString() << "(" << inputWeightSimpleCoords.size << " coordinates) while the owner semisimple "
    << " Lie algebra is of rank " << this->GetOwner().GetRank()
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
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
//  std::cout << "weights of smaller module: " << weightsLeftSimpleCoords.ToString();
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
{ //std::cout << "<hr>Accounting " << currentWeightSimpleCoords.ToString()
  //<< " with coefficient " << finalCoeff.ToString();

  Vector<Rational> dominant=currentWeightSimpleCoords;
  dominant+=otherHighestWeightSimpleCoords;
  dominant+=theWeyl.rho;
  int sign;
  bool certainlyHasStabilizer;
//  std::cout << "<br> Before raising to dominant, in simple coords: " << dominant.ToString() << "<br>";
  theWeyl.RaiseToDominantWeight(dominant, &sign, &certainlyHasStabilizer);
//  std::cout << "After raising to dominant: " << dominant.ToString() << "<br>";
  dominant-=theWeyl.rho;
  if (!theWeyl.IsDominantWeight(dominant))
    return;
  MonomialChar<Rational> tempMon;
  tempMon.weightFundamentalCoords=theWeyl.GetFundamentalCoordinatesFromSimple(dominant);
  CoefficientType coeffChange;
  coeffChange=theCoeff;
  coeffChange*= theMult*finalCoeff*sign;
//  std::cout << "; final contribution: " << tempMon.Coefficient.ToString()
//  << "*" << tempMon.weightFundamentalCoords.ToString() << "<br>";
  outputAccum.AddMonomial(tempMon, coeffChange);
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
  out << CGI::GetHtmlMathSpanFromLatexFormulaAddBeginArrayL(theMod.ToString());
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
    out << CGI::GetHtmlMathSpanFromLatexFormulaAddBeginArrayL(finalChar.ToString());
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
  List<ModuleSSalgebra<RationalFunction> >& theMods=theNode.owner->theModulePolys;
  int indexOfModule=-1;
  Selection selectionParSel;
  selectionParSel=parSel;
  for (int i=0; i<theMods.size; i++)
  { ModuleSSalgebra<RationalFunction>& currentMod=theMods[i];
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
  ModuleSSalgebra<RationalFunction>& theMod=theMods[indexOfModule];
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

void LittelmannPath::ActByEalpha(int indexAlpha)
{ Rational theMin=0;
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
  output.ReservE(this->Waypoints.size);
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
//      std::cout << "preceding- preceding the preceding: " << tempRoot1.ToString();
//      std::cout << "<br>current- preceding : " << tempRoot2.ToString() << "<hr>";
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
  << theWeyl.WeylDimFormulaSimpleCoords(*thePath.Waypoints.LastObject()).ToString();
  out << ".<br> A printout of the paths follows. The waypoints of the Littelmann paths are given in simple coordinates.";
  for (int i=0; i<allPaths.size; i++)
  { out << "\n<br>\n" << allPaths[i].ToString();
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
  (*inputStartingPath.Waypoints.LastObject() ).ToString();
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
  //          hashedOutput.AddNoRepetition(currentPath);
            if (hashedOutput.AddNoRepetition(currentPath))
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
            //hashedOutput.AddNoRepetition(currentPath);
            if (hashedOutput.AddNoRepetition(currentPath))
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
  ModuleSSalgebra<Rational> theModule;
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
      << i+1 << " out of " << this->theGeneratingWordsGrouppedByWeight.size << ", word " << j+1 << " out of " << currentWordList.size
      << "...";
      theGlobalVariables.MakeProgressReport(progressStream.str(), 2);
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
      theGlobalVariables.MakeProgressReport(progressStream.str(), 2);
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
  out << "Highest weight in dual coords: " << weight.ToString() << "<br>";
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
      *logStream << "intermediate after simplification: " << intermediateAccum.ToString(&theGlobalVariables.theDefaultFormat)
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

/*template <class CoefficientType>
bool ElementUniversalEnveloping<CoefficientType>::
GetWithSimpleGeneratorsOnly(MonomialCollection<TensorMonomial<CoefficientType> >& output)
{ output.MakeZero();
  MonomialCollection<TensorMonomial<CoefficientType> > pbwmon, singlemon;
  TensorMonomial tempMon;
  for (int i=0; i<this->size; i++)
  { pbwmon.MakeOne();
    for (int j=0; j<(*this)[i].generatorsIndices.size; j++)
    { singlemon.
    }
  }
}*/

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
  out << "Highest weight in dual coords: " << weight.ToString() << "<br>";
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
  theOrbit.ReservE(1000);
  ElementUniversalEnveloping<CoefficientType> theElt;
  int theDim=this->GetOwner().GetRank();
  int numPosRoots=this->GetOwner().GetNumPosRoots();
  theOrbit.AddOnTop(*this);
  for (int i=0; i<theOrbit.size; i++)
    for (int j=0; j<theDim; j++)
    { theElt.MakeOneGenerator(j+numPosRoots+theDim, *this->owners, this->indexInOwners, theRingUnit);
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
  out << "Parabolic subalgebra selection: " << parSel.ToString() << ".";
  charSSAlgMod<Rational>& theChar=charNode.theChar.GetElement();
  std::string report;
  charSSAlgMod<Rational> tempChar;
  ReflectionSubgroupWeylGroup subWeyl;
  Vector<Rational> emptySel;
  emptySel.MakeZero(theDim);
  theChar.SplitOverLeviMonsEncodeHIGHESTWeight(&report,  tempChar, parSel, emptySel, subWeyl, theGlobalVariables);
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
  out << tempWeyl.ToString(false);
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
    << this->theHmm.theRange().ToString(&theGlobalVariables.theDefaultFormat) << "\n\\end{document}";
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
//  std::cout << "<br>Acting on " << theNode.theGenVermaElt.GetElement().ToString(theGlobalVariables)
//  << " by " << tempElt.ToString(theGlobalVariables, theFormat);
//  theNode.theGenVermaElt.GetElement().MultiplyMeByUEEltOnTheLeft
//  (theNode.owner->theModulePolys, tempElt, theGlobalVariables, RFone, RFZero)
//  ;
  theNode.ExpressionType=theNode.typeGenVermaElt;
  return theNode.errorNoError;
}

template <class CoefficientType>
void ElementTensorsGeneralizedVermas<CoefficientType>::MakeHWV
  (List<ModuleSSalgebra<CoefficientType> >& theOwner, int TheIndexInOwner, const CoefficientType& theRingUnit)
{ assert(TheIndexInOwner<theOwner.size);
  MonomialTensorGeneralizedVermas<CoefficientType> tensorMon;
  CoefficientType currentCoeff;
  currentCoeff=theRingUnit;
  tensorMon.theMons.SetSize(1);
  MonomialGeneralizedVerma<CoefficientType>& theMon=tensorMon.theMons[0];
  ModuleSSalgebra<CoefficientType>& theMod=theOwner.TheObjects[TheIndexInOwner];
  theMon.indexFDVector=theMod.theGeneratingWordsNonReduced.size-1;
  theMon.MakeConst(theOwner, TheIndexInOwner);
  this->MakeZero();
  this->AddMonomial(tensorMon, theRingUnit);
}

template <class CoefficientType>
std::string ModuleSSalgebra<CoefficientType>::ToString()const
{ std::stringstream out;
  GlobalVariables theGlobalVariables; FormatExpressions theformat;
  out << "<br>Highest weight fund coords: " << this->theHWFundamentalCoordsBaseField.ToString();
  out << "<br>in simple coordinates: " << this->theHWSimpleCoordSBaseField.ToString();
  out << "<br>Rational part highest weight fund. coords:" << this->theHWDualCoordS.ToString();
  out << "<br>Rational part highest weight simple coords: " << this->theHWSimpleCoordS.ToString();
  out << "<br>Type semisimple Lie algebra: " << this->theAlgebras->TheObjects[this->indexAlgebra].GetLieAlgebraName();
  out << "<br>Parabolic selection: " << this->parabolicSelectionNonSelectedAreElementsLevi.ToString();
  out << "<br>Character over H: " << this->theCharOverH.ToString();
  out << "<br>Character: " << this->theChaR.ToString();
  out << "<br>Actions generators matrix form (" << this->actionsGeneratorsMaT.size << "): ";
  for (int i=0; i<this->actionsGeneratorsMaT.size; i++)
    out << this->actionsGeneratorsMaT[i].ToString(true, false) << "";
  out << "<br>Actions generators UE form: " << this->actionsGeneratorS.ElementToStringGeneric();
  out << "<br>Computed generators: " << this->ComputedGeneratorActions.ToString();
  out << "<br>" << this->theGeneratingWordsNonReduced.size << " Generating words: ";
  for (int i=0; i<this->theGeneratingWordsNonReduced.size; i++)
  { out << this->theGeneratingWordsNonReduced[i].ToString(&theformat) << ", ";
  }
  /*out << this->theGeneratingWordsGrouppedByWeight;
  out << this->theSimpleGens;
  out << this->actionsSimpleGens;
  out << this->actionsSimpleGensMatrixForM;
  this->theBilinearFormsAtEachWeightLevel=other.theBilinearFormsAtEachWeightLevel;
  this->theBilinearFormsInverted=other.theBilinearFormsInverted;
  this->weightsSimpleGens=other.weightsSimpleGens;
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

std::string ProjectInformation::ToString()
{ std::stringstream out;
  out << "<button " << CGI::GetStyleButtonLikeHtml() << " onclick=\"switchMenu('sourceDetails');\" >C++ source of the calculator (expand/collapse)</button>";
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

std::string CGI::GetLatexEmbeddableLinkFromCalculatorInput(const std::string& input)
{ std::stringstream out;
  out << "\\url{http://vector-partition.jacobs-university.de/vpf/cgi-bin/calculator?";
  for (unsigned i=0; i<input.size(); i++)
  { //if (input[i]=='&')
     // out << "\\&";
    //else
    if (input[i]=='%')
      out << "\\%";
    else if (input[i]=='_')
      out << "\\_";
    else
      out << input[i];
  }
  out << "}";
  return out.str();
}

std::string CGI::GetHtmlMathSpanNoButtonAddBeginArrayL(const std::string& input)
{ std::stringstream out;
  out << "<span class=\"math\">\\begin{array}{l}" << input << "\\end{array} </span>";
  return out.str();
}

std::string CGI::GetHtmlMathSpanPure(const std::string& input)
{ std::stringstream out;
  out << "<span class=\"math\">" << input << "</span>";
  return out.str();
}

void branchingData::initAssumingParSelAndHmmInittedPart1NoSubgroups(GlobalVariables& theGlobalVariables)
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
      { this->generatorsSmallSub.PopLastObject();
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
  ElementSemisimpleLieAlgebra tempElt;
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
      tempElt.MakeGenerator
      (i, *this->theHmm.owners, this->theHmm.indexRange);
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
      tempElt.MakeGenerator
      (i, *this->theHmm.owners, this->theHmm.indexDomain);
      this->nilradicalSmall.AddOnTop(tempElt);
      this->indicesNilradicalSmall.AddOnTop(i);
    }
  }
//  std::cout << "<br>call stack look who is callng me: " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  this->NilModPreNil=this->nilradicalLarge;
  this->weightsNilModPreNil=this->weightsNilradicalLarge;
  Vector<Rational> proj;
  for (int i=0; i<this->nilradicalSmall.size; i++)
  { ElementSemisimpleLieAlgebra& eltImage=
    this->theHmm.imagesAllChevalleyGenerators[this->indicesNilradicalSmall[i]];
    int theIndex=this->NilModPreNil.IndexOfObject(eltImage);
    if (theIndex!=-1)
    { this->NilModPreNil.PopIndexSwapWithLast(theIndex);
      this->weightsNilModPreNil.PopIndexSwapWithLast(theIndex);
      continue;
    }
    bool isGood=false;
    for (int j=0; j<this->weightsNilModPreNil.size; j++)
    { proj=this->ProjectWeight(this->weightsNilModPreNil[j]);
      if (proj==this->weightsNilradicalSmall[i])
      { isGood=true;
        this->NilModPreNil.PopIndexSwapWithLast(j);
        this->weightsNilModPreNil.PopIndexSwapWithLast(j);
        break;
      }
    }
    if (!isGood)
    { std::cout << "This is either or a programming error, or Lemma 3.3, T. Milev, P. Somberg, \"On branching...\""
      << " is wrong. The question is, which is the more desirable case... The bad apple is element "
      << this->nilradicalSmall[i].ToString() << " of weight " << this->weightsNilradicalSmall[i].ToString()
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
  (this->generatorsSmallSub, emptyList, theGlobalVariables, 1000, true);
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
{ Vector<RationalFunction> weightDifference;
  std::stringstream formulaStream1;
  HashedList<Vector<RationalFunction> > accountedDiffs;
  accountedDiffs.SetExpectedSize(this->g2Weights.size);
  bool found=false;
  for (int i=0; i<this->g2Weights.size; i++)
  { weightDifference= this->g2Weights[i]-this->g2Weights[theIndex];
    if (weightDifference.IsPositive() && !accountedDiffs.Contains(weightDifference))
    { accountedDiffs.AddOnTop(weightDifference);
      if (additionalMultiple!=1)
        formulaStream1 << additionalMultiple.ToString(&this->theFormat);
      formulaStream1 << "(i(\\bar c) - (" << this->theChars[i].ToString(&this->theFormat) <<  "))";
      found=true;
    }
  }
  if (!found)
    formulaStream1 << "id";
  return formulaStream1.str();
}
