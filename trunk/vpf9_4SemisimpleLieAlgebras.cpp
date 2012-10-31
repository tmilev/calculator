//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader1_4SemisimpleLieAlgebras.h"
ProjectInformationInstance ProjectInfoVpf9_4cpp
(__FILE__, "Implementation of semisimple subalgebra routines. ");


void  SemisimpleSubalgebras::FindHCandidatesWithOneExtraHContaining
(Vectors<Rational>& inpuT, GlobalVariables* theGlobalVariables)
{ int theDimension= this->GetSSowner().theWeyl.CartanSymmetric.NumRows;
  SelectionWithMaxMultiplicity theSel;
  theSel.initMaxMultiplicity(theDimension, 2);
  int theCounter=MathRoutines::KToTheNth(3, theDimension);
  Vector<Rational> theRoot;
  WeylGroup& theWeyl=this->GetSSowner().theWeyl;
  Matrix<Rational> invertedCartan=this->GetSSowner().theWeyl.CartanSymmetric;
  invertedCartan.Invert();
  Vectors<Rational> tempRoots;
  tempRoots=inpuT;
  Vectors<Rational> inputCopy;
  inputCopy=inpuT;
  ProgressReport theReport(theGlobalVariables);
  for (int i=0; i<theCounter; i++, theSel.IncrementSubset())
  { //slTwo& currentSl2=this->theSl2s.TheObjects[i];
    theRoot=theSel;
    invertedCartan.ActOnVectorColumn(theRoot, theRoot);
    bool isGood=true;
    if (!inputCopy.LinSpanContainsRoot(theRoot))
    { for (int j=0; j<inputCopy.size; j++)
        if (theWeyl.RootScalarCartanRoot(inputCopy.TheObjects[j], theRoot).IsPositive())
        { isGood=false;
          break;
        }
      if (isGood)
      { tempRoots= inputCopy;
        tempRoots.AddOnTop(theRoot);
        for (int k=tempRoots.size-1; k>0; k--)
          if (tempRoots[k]< tempRoots[k-1])
            tempRoots.SwapTwoIndices(k, k-1);
        this->theHcandidates.AddOnTopNoRepetition(tempRoots);
        //this->th
      }
      if (theGlobalVariables!=0)
      { std::stringstream out;
        out << "index lowest non explored: " << this->indexLowestUnexplored+1 << " Total number found: " << this->theHcandidates.size;
        theReport.Report(out.str());
      }
    }
  }
}

void SemisimpleSubalgebras::FindHCandidates(GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("SemisimpleSubalgebras::FindHCandidates");
  Vectors<Rational> emptyStart;
//  List<List<Vector<Rational>> >::ListActualSizeIncrement=10000;
  MemorySaving<GlobalVariables> tempGV;
  if (theGlobalVariables==0)
    theGlobalVariables=&tempGV.GetElement();
  this->GetSSowner().FindSl2Subalgebras
  (*this->owners, this->indexInOwners, this->theSl2s, *theGlobalVariables);
  std::cout << this->theSl2s.ToString();
  this->theHcandidates.size=0;
  this->theHcandidates.AddOnTop(emptyStart);
  for (this->indexLowestUnexplored=0; this->indexLowestUnexplored<this->theHcandidates.size; this->indexLowestUnexplored++)
    this->FindHCandidatesWithOneExtraHContaining
    (this->theHcandidates[this->indexLowestUnexplored], theGlobalVariables);
}

std::string SemisimpleSubalgebras::ToString(FormatExpressions* theFormat)
{ std::stringstream out;
  out << this->theHcandidates.ToString();
  out << this->ElementToStringCandidatePrincipalSl2s(theFormat);
  out << this->theSl2s.ToString(theFormat);
  return out.str();
}

void SemisimpleSubalgebras::FindTheSSSubalgebras
(List<SemisimpleLieAlgebra>* newOwner, int newIndexInOwner, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("SemisimpleSubalgebras::FindTheSSSubalgebras");
  this->owners=newOwner;
  this->indexInOwners=newIndexInOwner;
//  int theRank=this->GetSSowner().GetRank();
  this->GetSSowner().FindSl2Subalgebras
  (*this->owners, this->indexInOwners, this->theSl2s, *theGlobalVariables);
//  this->theSl2s.ComputeModuleDecompositionsOfMinimalContainingRegularSAs(*theGlobalVariables);
//  this->GenerateModuleDecompositionsPrincipalSl2s(theRank, *theGlobalVariables);
//  this->MatchActualSl2sFixedRootSAToPartitionSl2s(*theGlobalVariables);
//  this->MakeSelectionBasedOnPrincipalSl2s(*theGlobalVariables);
  this->FindTheSSSubalgebrasPart2(theGlobalVariables);
}

void SemisimpleSubalgebras::FindTheSSSubalgebrasPart2
(GlobalVariables* theGlobalVariables)
{

}

void SemisimpleSubalgebras::MakeSelectionBasedOnPrincipalSl2s(GlobalVariables& theGlobalVariables)
{ this->RemainingCandidates.init(this->theTypes.size);
  for (int i=0; i<this->theTypes.size; i++)
    if (this->IndicesMatchingActualSl2s[i].size>0)
    { SltwoSubalgebras& theCurrentAlgebrasSl2s= this->theCandidateSubAlgebras[i];
      SemisimpleLieAlgebra& theSSLieAlgebra= this->GetSSowner();
      theSSLieAlgebra.FindSl2Subalgebras(*this->owners, this->indexInOwners, theCurrentAlgebrasSl2s, theGlobalVariables);
      bool DoesFit=true;
      for (int j=0; j<theCurrentAlgebrasSl2s.size; j++)
      { slTwo& currentSl2= theCurrentAlgebrasSl2s[j];
        bool currentSl2Fits=false;
        for (int k=0; k<this->theSl2s.size; k++)
        { slTwo& other= this->theSl2s[k];
          currentSl2Fits=currentSl2.ModuleDecompositionFitsInto
          (currentSl2.highestWeights, currentSl2.multiplicitiesHighestWeights,
           other.highestWeights, other.multiplicitiesHighestWeights);
          if (currentSl2Fits)
            break;
        }
        if (!currentSl2Fits)
        { DoesFit=false;
          break;
        }
      }
      if (DoesFit)
        this->RemainingCandidates.AddSelectionAppendNewIndex(i);
    }
}

void SemisimpleSubalgebras::GenerateModuleDecompositionsPrincipalSl2s
(int theRank, GlobalVariables& theGlobalVariables)
{ this->EnumerateAllPossibleDynkinDiagramsOfRankUpTo(theRank);
  slTwo tempSl2;
  this->CandidatesPrincipalSl2ofSubalgebra.ReservE(this->theTypes.size);
  this->theCandidateSubAlgebras.SetSize(this->theTypes.size);
  List<char> theLetters;
  List<int> theRanks;
  List<int> theMults;
  for (int i=0; i<this->theTypes.size; i++)
  { this->theTypes[i].GetLettersTypesMults(theLetters, theRanks, theMults);
    this->theCandidateSubAlgebras[i].theRootSAs.GetOwnerWeyl().MakeFromDynkinType
    (theLetters, theRanks, &theMults);

    this->theCandidateSubAlgebras[i].theRootSAs.GetOwnerWeyl().GenerateRootSystemFromKillingFormMatrix();
    (*this->theCandidateSubAlgebras[i].owners)[0].theWeyl=
    (this->theCandidateSubAlgebras[i].theRootSAs.GetOwnerWeyl());
    int theDimension = this->theCandidateSubAlgebras[i].theRootSAs.GetOwnerWeyl().GetDim();
    tempSl2.hCharacteristic.initFillInObject(theDimension, 2);
    tempSl2.preferredAmbientSimpleBasis.MakeEiBasis(theDimension);
    tempSl2.owners=this->owners;
    tempSl2.indexOwnerAlgebra=this->indexInOwners;
    tempSl2.ComputeModuleDecompositionAmbientLieAlgebra(theGlobalVariables);
    this->CandidatesPrincipalSl2ofSubalgebra.AddOnTop(tempSl2);
  }
}

void SemisimpleSubalgebras::EnumerateAllPossibleDynkinDiagramsOfRankUpTo(int theRank)
{ this->GenerateAllPartitionsUpTo(theRank);
  this->theTypes.size=0;
  List<int> ranksBuffer, multiplicitiesBuffer;
  List<char> lettersBuffer;
  for (int i=0; i<this->thePartitionMultiplicities.size; i++)
    this->GenerateAllDiagramsForPartitionRecursive
    (i, 0, ranksBuffer, multiplicitiesBuffer, lettersBuffer);
}

void SemisimpleSubalgebras::GenerateAllDiagramsForPartitionRecursive
(int indexPartition, int indexInPartition,
 List<int>& ranksBuffer, List<int>& multiplicitiesBuffer, List<char>& lettersBuffer)
{ List<int>& partitionValues= this->thePartitionValues[indexPartition];
  List<int>& partitionMults= this->thePartitionMultiplicities[indexPartition];
  if (indexInPartition>= partitionValues.size)
  { DynkinSimpleType currentType;
    DynkinType outputType;
    outputType.MakeZero();
    for (int i=0; i<ranksBuffer.size; i++)
    { currentType.theLetter=lettersBuffer[i];
      currentType.theRank=ranksBuffer[i];
      outputType.AddMonomial(currentType, multiplicitiesBuffer[i]);
    }
    this->theTypes.AddOnTop(outputType);
    return;
  }
  Selection DistributionBetweenTheFourLetters;
  int theMult = partitionMults[indexInPartition];
  int theRank = partitionValues[indexInPartition];
  int numLetters;
  List<char> lettersAvailable; lettersAvailable.SetSize(5);
  lettersAvailable[0]='A';
  lettersAvailable[1]='B';
  lettersAvailable[2]='C';
  lettersAvailable[3]='D';
  switch(theRank)
  { case 1: numLetters=1; break;
    case 2: numLetters=3; lettersAvailable[2]='G'; break;
    case 3: numLetters=3; break;
    case 4: numLetters=5; lettersAvailable[4]='F'; break;
    case 6: numLetters=5; lettersAvailable[4]='E'; break;
    case 7: numLetters=5; lettersAvailable[4]='E'; break;
    case 8: numLetters=5; lettersAvailable[4]='E'; break;
    default: numLetters=4; break;
  }
  int numBars=numLetters-1;
  DistributionBetweenTheFourLetters.init(numBars+theMult);
  //there are numLetters letters, therefore we need numLetters-1 barriers
  //in numLetters-1+theMult cells to record a partition of mult into 7 letters.
  int numCycles= MathRoutines::NChooseK(DistributionBetweenTheFourLetters.MaxSize, numBars);
  int oldsize= ranksBuffer.size;
  for (int i=0; i<numCycles; i++)
  { DistributionBetweenTheFourLetters.incrementSelectionFixedCardinality(numBars);
    int startIndex=-1;
    int endIndex;
    for (int k=0; k< numLetters; k++)
    { if (k!=0)
        startIndex= DistributionBetweenTheFourLetters.elements[k-1];
      if (k!=numBars)
        endIndex= DistributionBetweenTheFourLetters.elements[k];
      else
        endIndex= DistributionBetweenTheFourLetters.MaxSize;
      int numIsotypic= endIndex-startIndex-1;
      if (numIsotypic!=0)
      { ranksBuffer.AddOnTop(theRank);
        lettersBuffer.AddOnTop(lettersAvailable.TheObjects[k]);
        multiplicitiesBuffer.AddOnTop(numIsotypic);
      }
    }
    this->GenerateAllDiagramsForPartitionRecursive(indexPartition, indexInPartition+1, ranksBuffer, multiplicitiesBuffer, lettersBuffer);
    ranksBuffer.size=oldsize;
    multiplicitiesBuffer.size=oldsize;
    lettersBuffer.size=oldsize;
  }
}

void SemisimpleSubalgebras::MatchRealSl2sToPartitionSl2s()
{ this->IndicesMatchingSl2s.SetSize(this->theTypes.size);
  for (int i=0; i<this->CandidatesPrincipalSl2ofSubalgebra.size; i++)
  { this->IndicesMatchingSl2s[i].size=0;
    for (int j=0; j<this->theSl2s.size; j++)
      if (this->CandidatesPrincipalSl2ofSubalgebra[i].ModuleDecompositionFitsInto(this->theSl2s[j]))
        this->IndicesMatchingSl2s[i].AddOnTop(j);
  }
}

void SemisimpleSubalgebras::MatchActualSl2sFixedRootSAToPartitionSl2s
(GlobalVariables& theGlobalVariables)
{ this->theSl2s.ComputeModuleDecompositionsOfMinimalContainingRegularSAs(theGlobalVariables);
  List<int> tempL;
  this->IndicesMatchingActualSl2s.initFillInObject(this->theTypes.size, tempL);
  this->IndicesMatchingPartitionSl2s.initFillInObject(this->theSl2s.size, tempL);
  for (int i=0; i<this->theSl2s.size; i++)
  { slTwo& theSl2= this->theSl2s[i];
    for (int j=0; j<theSl2.IndicesMinimalContainingRootSA.size; j++)
      for (int k=0; k<this->CandidatesPrincipalSl2ofSubalgebra.size; k++)
      { slTwo& candidateSl2= this->CandidatesPrincipalSl2ofSubalgebra[k];
        if (theSl2.ModuleDecompositionFitsInto
            (candidateSl2.highestWeights, candidateSl2.multiplicitiesHighestWeights,
             theSl2.HighestWeightsDecompositionMinimalContainingRootSA[j],
             theSl2.MultiplicitiesDecompositionMinimalContainingRootSA[j]))
        { this->IndicesMatchingActualSl2s[k].AddOnTop(i);
          this->IndicesMatchingPartitionSl2s[i].AddOnTop(k);
        }
      }
  }
}


void SemisimpleSubalgebras::GenerateAllPartitionsRecursive
(int remainingToBePartitioned, int CurrentValue, List<int>& Multiplicities, List<int>& Values)
{ if (remainingToBePartitioned==0)
  { this->thePartitionMultiplicities.AddOnTop(Multiplicities);
    this->thePartitionValues.AddOnTop(Values);
    return;
  }
  int possibleMults=(remainingToBePartitioned/CurrentValue)+1;
  for(int i=0; i<possibleMults; i++)
  { if (i>0)
    { Multiplicities.AddOnTop(i);
      Values.AddOnTop(CurrentValue);
    }
    int newRemainder = remainingToBePartitioned - i*CurrentValue;
    int newCurrentValue = MathRoutines::Minimum(CurrentValue-1, newRemainder);
    if (newCurrentValue!=0 || newRemainder==0)
      this->GenerateAllPartitionsRecursive(newRemainder, newCurrentValue, Multiplicities, Values);
    if (i>0)
    { Multiplicities.size--;
      Values.size--;
    }
  }
}

void SemisimpleSubalgebras::GenerateAllPartitionsUpTo(int theRank)
{ this->thePartitionMultiplicities.size=0;
  this->thePartitionValues.size=0;
  for (int i=1; i<=theRank; i++)
    this->GenerateAllPartitionsDontInit(i);
}

void SemisimpleSubalgebras::GenerateAllPartitionsDontInit(int theRank)
{ int upperLimit= MathRoutines::TwoToTheNth(theRank);
  this->thePartitionMultiplicities.ReservE(upperLimit);
  this->thePartitionValues.ReservE(upperLimit);
  List<int> buffer1, buffer2;
  this->GenerateAllPartitionsRecursive(theRank, theRank, buffer1, buffer2);
}

std::string SemisimpleSubalgebras::ElementToStringCandidatePrincipalSl2s
(FormatExpressions* theFormat)
{ std::stringstream out;
  std::string tempS;
  out << "the candidate mults: "
  << this->thePartitionMultiplicities;
  out << this->theTypes.size << " candidate subalgebras: ";
  for (int i=0; i<this->theTypes.size; i++)
    out << "<br>" << this->theTypes[i];
  for (int i =0; i<this->thePartitionValues.size; i++)
  { for (int j=0; j< this->thePartitionValues[i].size; j++)
    { int mult= this->thePartitionMultiplicities[i][j];
      int val= this->thePartitionValues[i][j];
      if (mult!=1)
        out << mult << " x ";
      out << val << " ";
      if (j!=this->thePartitionValues[i].size-1)
        out << "+ ";
    }
    out << "\n<br>";
    out << "\n";
  }
  for (int i=0; i<this->theTypes.size; i++)
  { out << this->theTypes[i] << "     Module decomposition: ";
    slTwo& theSl2= this->CandidatesPrincipalSl2ofSubalgebra[i];
    theSl2.ElementToStringModuleDecomposition(false, true, tempS);
    out << tempS;
    if (this->IndicesMatchingActualSl2s.size>0)
    { out << " Matching actual sl(2)'s: ";
      for (int j=0; j<this->IndicesMatchingActualSl2s[i].size; j++)
      { int tempI= this->IndicesMatchingActualSl2s[i][j];
        out << this->theSl2s[tempI].hCharacteristic.ToString() << ", ";
      }
    }
    out << "\n<br>";
    out << "\n";
  }
  out << "Remaining candidates:\n" << this->RemainingCandidates.ToString();
  out << "<br>The candidate details follow.<br>\n";
  for (int i=0; i<this->theCandidateSubAlgebras.size; i++)
  { SltwoSubalgebras& currentSl2s= this->theCandidateSubAlgebras[i];
    if (currentSl2s.size>0)
    { out << this->theTypes[i];
      if (this->RemainingCandidates.selected[i])
        out << " orbits fit\n";
      else
        out << " orbits do not fit, embedding impossible\n";
      out << "<br>\n";
      out << currentSl2s.ElementToStringNoGenerators(theFormat);
    }
  }
  return out.str();
}

void slTwo::ElementToStringModuleDecompositionMinimalContainingRegularSAs(bool useLatex, bool useHtml, SltwoSubalgebras& owner, std::string& output)
{ std::stringstream out;
  std::string tempS;
  if (useLatex)
    out << "$";
  if (useHtml)
  { out << "<table><tr><td align=\"center\">Char.</td>";
    for (int i=0; i<this->IndicesMinimalContainingRootSA.size; i++)
    { rootSubalgebra& theSA= owner.theRootSAs.TheObjects[this->IndicesMinimalContainingRootSA.TheObjects[i]];
      CGI::clearDollarSigns(theSA.theDynkinDiagram.DynkinStrinG, tempS);
      out << "<td align=\"center\">Decomp. " << tempS << "</td>";
    }
    out << "</tr>\n";
  }
  out << "<tr><td align=\"center\"> " << this->hCharacteristic.ToString() << "</td>";
  for (int k=0; k<this->IndicesMinimalContainingRootSA.size; k++)
  { rootSubalgebra& theSA= owner.theRootSAs.TheObjects[this->IndicesMinimalContainingRootSA.TheObjects[k]];
    CGI::clearDollarSigns(theSA.theDynkinDiagram.DynkinStrinG, tempS);
    if (useHtml)
      out << "<td align=\"center\">";
    for (int i=0; i<this->HighestWeightsDecompositionMinimalContainingRootSA.TheObjects[k].size; i++)
    { if (this->MultiplicitiesDecompositionMinimalContainingRootSA.TheObjects[k].TheObjects[i]>1)
        out << this->MultiplicitiesDecompositionMinimalContainingRootSA.TheObjects[k].TheObjects[i];
      out << "V_{"<<this->HighestWeightsDecompositionMinimalContainingRootSA.TheObjects[k].TheObjects[i] << "}";
      if (i!=this->HighestWeightsDecompositionMinimalContainingRootSA.TheObjects[k].size-1)
        out << "+";
    }
    if (useHtml)
      out << "</td>";
    out << "\n";
  }
  if (useHtml)
    out << "</tr></table>";
  if (useLatex)
    out << "$";
  output=out.str();
}

void slTwo::ElementToHtmlCreateFormulaOutputReference(const std::string& formulaTex, std::stringstream& output, bool usePNG, bool useHtml, SltwoSubalgebras& container, std::string* physicalPath, std::string* htmlPathServer)
{ if (!usePNG)
  { output << formulaTex;
    //if (useHtml)
      //output<<"\n<br>\n";
    return;
  }
  std::stringstream tempStream;
  container.texFileNamesForPNG.SetSize(container.texFileNamesForPNG.size+1);
  container.texStringsEachFile.SetSize(container.texFileNamesForPNG.size);
  (*container.texStringsEachFile.LastObject())=formulaTex;
  tempStream << (*physicalPath) << "fla";
  tempStream << container.texFileNamesForPNG.size << ".tex";
  container.texFileNamesForPNG.TheObjects[container.texFileNamesForPNG.size-1]=tempStream.str();
  output << "<img src=\"" << (*htmlPathServer) << "fla" << container.texFileNamesForPNG.size
  << ".png\">";
  if (useHtml)
    output << "\n<br>\n";
}

WeylGroup& slTwo::GetOwnerWeyl()
{ return this->GetOwnerSSAlgebra().theWeyl;
}

bool slTwo::operator==(const slTwo& right)const
{// See Dynkin, Semisimple Lie subalgebras of semisimple Lie algebras, chapter 7-10
  if (this->owners!=right.owners || this->indexOwnerAlgebra!=right.indexOwnerAlgebra)
  { std::cout << "This is a programming error: comparing sl(2) subalgebras"
    << "that have different ambient Lie algebras. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return this->hCharacteristic==(right.hCharacteristic);
}


std::string slTwo::ToString(FormatExpressions* theFormat)
{ if (this->container==0)
    return "sl(2) subalgebra not initialized.";
  std::stringstream out;  std::string tempS;
  out << "<a name=\"sl2index" << indexInContainer << "\">h-characteristic: "
  << this->hCharacteristic.ToString() << "</a>";
  tempS=this->preferredAmbientSimpleBasis.ToString();
  std::string physicalPath="";
  std::string htmlPathServer="";
  bool usePNG=true;
  bool useHtml=true;
  bool useLatex=false;
  if (theFormat!=0)
    if (theFormat->physicalPath!="")
    { physicalPath=theFormat->physicalPath + "sl2s/";
      htmlPathServer=theFormat->htmlPathServer + "sl2s/";
    }
  if (physicalPath=="" || htmlPathServer=="")
  { usePNG=false;
    useHtml=false;
  }
  if (useHtml)
    out << "<br>";
  out << "\nSimple basis ambient algebra w.r.t defining h: " << tempS;
  Matrix<Rational> tempMat;
  if (useHtml)
    out << "<br>";
  if (this->IndicesContainingRootSAs.size>1)
  { out << "Number of containing regular semisimple subalgebras: "
    << this->IndicesContainingRootSAs.size;
    if (useHtml)
      out << "<br>";
  }
  FormatExpressions localFormat, latexFormat;
  localFormat.flagUseHTML=useHtml;
  localFormat.flagUseLatex=useLatex;
  latexFormat.flagUseHTML=false;
  latexFormat.flagUseLatex=true;
  for (int i=0; i<this->IndicesContainingRootSAs.size; i++)
  { out << "\nContaining regular semisimple subalgebra number " << i+1 << ": ";
    rootSubalgebra& currentSA= this->container->theRootSAs[this->IndicesContainingRootSAs[i]];
    if (useHtml)
    { out << "<a href=\"" << htmlPathServer << "../rootHtml_rootSA"
      << this->IndicesContainingRootSAs[i] << ".html\">";
      currentSA.theDynkinDiagram.ElementToStrinG(tempS, useLatex, true);
      CGI::clearDollarSigns(tempS, tempS);
    }
    out << tempS;
    if (useHtml)
      out << "</a>";
  }
  if (useHtml)
    out << "<br>";
  out << "\nsl(2)-module decomposition of the ambient Lie algebra: ";
  this->ElementToStringModuleDecomposition(useLatex || usePNG, useHtml, tempS);
  this->ElementToHtmlCreateFormulaOutputReference
  (tempS, out, usePNG, useHtml, *this->container, &physicalPath, &htmlPathServer);
  this->container->IndicesSl2decompositionFlas.SetSize(this->container->size);
  this->container->IndicesSl2decompositionFlas[indexInContainer]=
  this->container->texFileNamesForPNG.size-1;
  out << "\nThe below list one possible realization of the sl(2) subalgebra.";
  if (useHtml)
    out << "\n<br>\n";
  std::stringstream tempStreamH, tempStreamE, tempStreamF, tempStreamHE, tempStreamHF, tempStreamEF;
  tempS=this->theH.ToString(theFormat);
  tempStreamH << "\n$h=$ $" << tempS << "$";
  tempS= tempStreamH.str();
  this->ElementToHtmlCreateFormulaOutputReference
  (tempS, out, usePNG, useHtml, *this->container, &physicalPath, &htmlPathServer);
  tempStreamE << "\n$e=$ $" << this->theE.ToString(theFormat) << "$";
  tempS= tempStreamE.str();
  this->ElementToHtmlCreateFormulaOutputReference
  (tempS, out, usePNG, useHtml, *this->container, &physicalPath, &htmlPathServer);
  tempStreamF << "\n$f=$ $" << this->theF.ToString(theFormat) << "$";
  tempS= tempStreamF.str();
  this->ElementToHtmlCreateFormulaOutputReference
  (tempS, out, usePNG, useHtml, *this->container, &physicalPath, &htmlPathServer);
  out << "\n\nThe below are the Lie brackets of the above elements. Printed for debugging.";
  if (useHtml)
    out << "\n<br>\n";
  tempStreamEF << "\n$[e, f]=$ $" <<  this->bufferEbracketF.ToString(theFormat) << "$";
  tempS= tempStreamEF.str();
  this->ElementToHtmlCreateFormulaOutputReference
  (tempS, out, usePNG, useHtml, *this->container, &physicalPath, &htmlPathServer);
  tempStreamHE << "\n$[h, e]=$ $" << this->bufferHbracketE.ToString(theFormat) << "$";
  tempS= tempStreamHE.str();
  this->ElementToHtmlCreateFormulaOutputReference
  (tempS, out, usePNG, useHtml, *this->container, &physicalPath, &htmlPathServer);
  tempStreamHF << "\n$[h, f]=$ $" << this->bufferHbracketF.ToString(theFormat) << "$";
  tempS= tempStreamHF.str();
  this->ElementToHtmlCreateFormulaOutputReference
  (tempS, out, usePNG, useHtml, *this->container, &physicalPath, &htmlPathServer);
  //this->theSystemMatrixForm.ToString(tempS);
  //out <<"\nSystem matrix form we try to solve:\n"<< tempS;
  //this->theSystemColumnVector.ToString(tempS);
  //out <<"\nColumn vector of the system:\n"<<tempS;
  std::stringstream tempStreamActual;
  for (int i=0; i<this->theSystemToBeSolved.size; i++)
  { tempS=this->theSystemToBeSolved[i].ToString(theFormat);
    if (tempS=="")
    { if (useLatex || usePNG)
        tempStreamActual << "~\\\\";
      else
        tempStreamActual << "\n\n";
    }
    else
      tempStreamActual << "\\noindent \\begin{eqnarray*}&& " << tempS << "=0\\end{eqnarray*}\n\n";
  }
  out << "\nThe system we need to solve:\n";
  if (useHtml)
    out << "\n<br>\n";
  tempS= tempStreamActual.str();
  this->ElementToHtmlCreateFormulaOutputReference
  (tempS, out, usePNG, useHtml, *this->container, &physicalPath, &htmlPathServer);
  return out.str();
}

void slTwo::ElementToHtml(std::string& filePath)
{ std::fstream theFile;
  std::string theFileName=filePath;
  theFileName.append("theSlTwo.txt");
  CGI::OpenFileCreateIfNotPresent(theFile, filePath, false, true, false);
}

void SltwoSubalgebras::init(List<SemisimpleLieAlgebra>* inputOwners, int inputIndexInOwner)
{ this->owners=inputOwners;
  this->IndexInOwners=inputIndexInOwner;
  this->theRootSAs.ownerArray=inputOwners;
  this->theRootSAs.indexInOwner=inputIndexInOwner;
}

void SemisimpleLieAlgebra::FindSl2Subalgebras
  (List<SemisimpleLieAlgebra>& inputOwner, int inputIndexInOwner,
   SltwoSubalgebras& output, GlobalVariables& theGlobalVariables)
{ MacroRegisterFunctionWithName("SemisimpleLieAlgebra::FindSl2Subalgebras");
  output.init(&inputOwner, inputIndexInOwner);
  output.theRootSAs.GenerateAllReductiveRootSubalgebrasUpToIsomorphism
  (theGlobalVariables, true, true);
  //output.theRootSAs.ComputeDebugString(false, false, false, 0, 0, theGlobalVariables);
  output.IndicesSl2sContainedInRootSA.SetSize(output.theRootSAs.size);
  output.IndicesSl2sContainedInRootSA.ReservE(output.theRootSAs.size*2);
  for (int i=0; i<output.IndicesSl2sContainedInRootSA.size; i++)
    output.IndicesSl2sContainedInRootSA[i].size=0;
  ProgressReport theReport(&theGlobalVariables);
  for (int i=0; i<output.theRootSAs.size-1; i++)
  { output.theRootSAs[i].GetSsl2SubalgebrasAppendListNoRepetition
    (output, i, theGlobalVariables);
    std::stringstream tempStream;
    tempStream << "Exploring root subalgebra number " << (i+1)
    << " out of " << output.theRootSAs.size-1 << " non-trivial";
    theReport.Report(tempStream.str());
  }
  for (int i=0; i<output.size; i++)
  { slTwo& theSl2= output[i];
    theSl2.IndicesMinimalContainingRootSA.ReservE(theSl2.IndicesContainingRootSAs.size);
    theSl2.IndicesMinimalContainingRootSA.size=0;
    for (int j=0; j<theSl2.IndicesContainingRootSAs.size; j++)
    { bool isMinimalContaining=true;
//      rootSubalgebra& currentRootSA = output.theRootSAs.TheObjects[];
      for (int k=0; k<theSl2.IndicesContainingRootSAs.size; k++)
      { rootSubalgebra& theOtherRootSA =
        output.theRootSAs[theSl2.IndicesContainingRootSAs[k]];
        if (theOtherRootSA.indicesSubalgebrasContainingK.ContainsObject(theSl2.IndicesContainingRootSAs[j]))
        { isMinimalContaining=false;
          break;
        }
      }
      if (isMinimalContaining)
        theSl2.IndicesMinimalContainingRootSA.AddOnTopNoRepetition(theSl2.IndicesContainingRootSAs[j]);
    }
    output.ComputeModuleDecompositionsOfAmbientLieAlgebra(theGlobalVariables);
  }
//  tempRootSA.GetSsl2Subalgebras(tempSl2s, theGlobalVariables, *this);
}

std::string rootSubalgebras::ToString()
{ std::stringstream out;
  for (int i=0; i<this->size; i++)
    out << (*this)[i].ToString();
  return out.str();
}

WeylGroup& rootSubalgebras::GetOwnerWeyl()
{ return this->GetOwnerSSalgebra().theWeyl;
}

SemisimpleLieAlgebra& rootSubalgebras::GetOwnerSSalgebra()
{ if (this->ownerArray==0 || this->indexInOwner<0)
  { std::cout << "This is a programming error. Attempting to access the "
    << " ambient Lie algebra of a non-initialized collection of root subalgebras. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return (*this->ownerArray)[this->indexInOwner];
}

void rootSubalgebras::GenerateAllReductiveRootSubalgebrasUpToIsomorphism
(GlobalVariables& theGlobalVariables, bool sort, bool computeEpsCoords)
{ MacroRegisterFunctionWithName("rootSubalgebras::GenerateAllReductiveRootSubalgebrasUpToIsomorphism");
  this->size=0;
  this->GetOwnerWeyl().ComputeRho(true);
  //this->initDynkinDiagramsNonDecided(this->AmbientWeyl, WeylLetter, WeylRank);
  rootSubalgebras rootSAsGenerateAll;
  rootSAsGenerateAll.SetSize(this->GetOwnerSSalgebra().GetRank()*2+1);
  rootSAsGenerateAll[0].genK.size=0;
  rootSAsGenerateAll[0].owners=this->ownerArray;
  rootSAsGenerateAll[0].indexInOwners=this->indexInOwner;
  rootSAsGenerateAll[0].ComputeAll();
  this->GenerateAllReductiveRootSubalgebrasContainingInputUpToIsomorphism
  (rootSAsGenerateAll, 1, theGlobalVariables)
  ;
//  std::cout << this->ToString();
  if (sort)
    this->SortDescendingOrderBySSRank();
  if(computeEpsCoords)
    for(int i=0; i<this->size; i++)
      (*this)[i].ComputeEpsCoordsWRTk(theGlobalVariables);
}

void rootSubalgebra::GetSsl2SubalgebrasAppendListNoRepetition
(SltwoSubalgebras& output, int indexInContainer, GlobalVariables& theGlobalVariables)
{ MacroRegisterFunctionWithName("rootSubalgebra::GetSsl2SubalgebrasAppendListNoRepetition");
  //reference: Dynkin, semisimple Lie algebras of simple lie algebras, theorems 10.1-10.4
  Selection theRootsWithZeroCharacteristic;
  Vectors<Rational> RootsWithCharacteristic2;
  Vectors<Rational> reflectedSimpleBasisK;
  RootsWithCharacteristic2.ReservE(this->PositiveRootsK.size);
  ElementWeylGroup raisingElt;
  DynkinDiagramRootSubalgebra tempDiagram;
  int theRelativeDimension= this->SimpleBasisK.size;
  theRootsWithZeroCharacteristic.init(theRelativeDimension);
  Matrix<Rational> InvertedRelativeKillingForm;
  InvertedRelativeKillingForm.init(theRelativeDimension, theRelativeDimension);
  for (int k=0; k<theRelativeDimension; k++)
    for (int j=0; j<theRelativeDimension; j++)
      InvertedRelativeKillingForm.elements[k][j]=this->GetAmbientWeyl().RootScalarCartanRoot
      (this->SimpleBasisK[k], this->SimpleBasisK[j]);
  InvertedRelativeKillingForm.Invert();
  this->GetOwnerSSalg().ComputeChevalleyConstantS(theGlobalVariables);
  int numCycles= MathRoutines::TwoToTheNth(theRootsWithZeroCharacteristic.MaxSize);
  ProgressReport theReport(&theGlobalVariables);
  Vectors<Rational> tempRoots;
  tempRoots.ReservE(theRootsWithZeroCharacteristic.MaxSize);
  Vectors<Rational> relativeRootSystem, bufferVectors;
  Matrix<Rational> tempMat;
//  Selection tempSel;
  this->PositiveRootsK.GetCoordsInBasis
  (this->SimpleBasisK, relativeRootSystem, bufferVectors, tempMat);
  slTwo theSl2;
  theSl2.container=&output;
  theSl2.owners=this->owners;
  theSl2.indexOwnerAlgebra=this->indexInOwners;
  SemisimpleLieAlgebra& theLieAlgebra= this->GetOwnerSSalg();
  for (int i=0; i<numCycles; i++, theRootsWithZeroCharacteristic.incrementSelection())
  { tempRoots.size=0;
    for (int j=0; j<theRootsWithZeroCharacteristic.CardinalitySelection; j++)
      tempRoots.AddOnTop(this->SimpleBasisK[theRootsWithZeroCharacteristic.elements[j]]);
    tempDiagram.ComputeDiagramTypeModifyInput(tempRoots, this->GetAmbientWeyl());
    int theSlack=0;
    RootsWithCharacteristic2.size=0;
    for (int j=0; j<relativeRootSystem.size; j++)
    { Rational DimTwoSpace=0;
      for (int k=0; k<theRelativeDimension; k++)
        if (!theRootsWithZeroCharacteristic.selected[k])
        { DimTwoSpace+=relativeRootSystem[j][k];
          if (DimTwoSpace>1)
            break;
        }
      if (DimTwoSpace==1)
      { theSlack++;
        RootsWithCharacteristic2.AddOnTop(this->PositiveRootsK[j]);
      }
    }
    int theDynkinEpsilon =
    tempDiagram.NumRootsGeneratedByDiagram() + theRelativeDimension - theSlack;
    //if Dynkin's epsilon is not zero the subalgebra cannot be an S sl(2) subalgebra.
    //otherwise, as far as I understand, it always is (but generators still have to be found)
    //this is done in the below code.
//    std::cout << "Simple basis k: " << this->SimpleBasisK.ToString();
    if (theDynkinEpsilon==0)
    { Vector<Rational> tempRoot, tempRoot2;
      tempRoot.MakeZero(theRelativeDimension);
      for (int k=0; k<theRelativeDimension; k++)
        if(!theRootsWithZeroCharacteristic.selected[k])
          tempRoot[k]=2;
      InvertedRelativeKillingForm.ActOnVectorColumn(tempRoot, tempRoot2);
      Vector<Rational> characteristicH;
      characteristicH.MakeZero(theLieAlgebra.GetRank());
      for(int j=0; j<theRelativeDimension; j++)
        characteristicH+=this->SimpleBasisK[j]*tempRoot2[j];
      this->GetAmbientWeyl().RaiseToDominantWeight(characteristicH, 0, 0, &raisingElt);
      reflectedSimpleBasisK=this->SimpleBasisK;
      for (int k=0; k<reflectedSimpleBasisK.size; k++)
        this->GetAmbientWeyl().ActOn(raisingElt, reflectedSimpleBasisK[k], false, false);
      theSl2.theH.MakeHgenerator
      (characteristicH, *theLieAlgebra.owner, theLieAlgebra.indexInOwner);
      theSl2.theE.MakeZero(*theLieAlgebra.owner, theLieAlgebra.indexInOwner);
      theSl2.theF.MakeZero(*theLieAlgebra.owner, theLieAlgebra.indexInOwner);
      //theSl2.ComputeDebugString(false, false, theGlobalVariables);
//      std::cout << "<br>accounting " << characteristicH.ToString();
      if(theLieAlgebra.AttemptExtendingHEtoHEFWRTSubalgebra
         (RootsWithCharacteristic2, theRootsWithZeroCharacteristic,
          this->SimpleBasisK,
          characteristicH, theSl2.theE, theSl2.theF, theSl2.theSystemMatrixForm,
          theSl2.theSystemToBeSolved, theSl2.theSystemColumnVector, theGlobalVariables))
      { int indexIsoSl2;
        theSl2.MakeReportPrecomputations
        (theGlobalVariables, output, output.size, indexInContainer, *this);
        if(output.ContainsSl2WithGivenHCharacteristic(theSl2.hCharacteristic, &indexIsoSl2))
        { output[indexIsoSl2].IndicesContainingRootSAs.AddOnTop(indexInContainer);
          output.IndicesSl2sContainedInRootSA[indexInContainer].AddOnTop(indexIsoSl2);
        } else
        { output.IndicesSl2sContainedInRootSA[indexInContainer].AddOnTop(output.size);
          theSl2.indexInContainer=output.size;
          output.AddOnTop(theSl2);
        }
      }
      else
        output.BadHCharacteristics.AddOnTop(characteristicH);
    }
    std::stringstream out;
    out << "Exploring Dynkin characteristics case " << i+1 << " out of " << numCycles;
//    std::cout << "<br>" << out.str();
    theReport.Report(out.str());
  }
//  std::cout << "Bad chracteristics: " << output.BadHCharacteristics.ToString();
}

bool SltwoSubalgebras::ContainsSl2WithGivenH(Vector<Rational>& theH, int* outputIndex)
{ if (outputIndex!=0)
    *outputIndex=-1;
  ElementSemisimpleLieAlgebra tempH;
  this->CheckForCorrectInitializationCrashIfNot();
  tempH.MakeHgenerator(theH, *this->owners, this->IndexInOwners);
  for (int i=0; i<this->size; i++)
    if (this->TheObjects[i].theH==tempH)
    { if (outputIndex!=0)
        *outputIndex=i;
      return true;
    }
  return false;
}

bool SltwoSubalgebras::ContainsSl2WithGivenHCharacteristic
(Vector<Rational>& theHCharacteristic, int* outputIndex)
{ if (outputIndex!=0)
    *outputIndex=-1;
  for (int i=0; i<this->size; i++)
    if ((*this)[i].hCharacteristic==theHCharacteristic)
    { if (outputIndex!=0)
        *outputIndex=i;
      return true;
    }
  return false;
}

void slTwo::ComputeModuleDecompositionAmbientLieAlgebra(GlobalVariables& theGlobalVariables)
{ this->ComputeModuleDecomposition
  (this->GetOwnerWeyl().RootsOfBorel, this->GetOwnerWeyl().CartanSymmetric.NumRows,
   this->highestWeights, this->multiplicitiesHighestWeights, this->weightSpaceDimensions,
   theGlobalVariables);
}

void slTwo::ComputeModuleDecompositionOfMinimalContainingRegularSAs(SltwoSubalgebras& owner, int IndexInOwner, GlobalVariables& theGlobalVariables)
{ this->MultiplicitiesDecompositionMinimalContainingRootSA.SetSize
  (this->IndicesMinimalContainingRootSA.size);
  this->HighestWeightsDecompositionMinimalContainingRootSA.SetSize
  (this->IndicesMinimalContainingRootSA.size);
  List<int> buffer;
  for (int i=0; i<this->IndicesMinimalContainingRootSA.size; i++)
  { rootSubalgebra& theSA= owner.theRootSAs[this->IndicesMinimalContainingRootSA[i]];
    this->ComputeModuleDecomposition
    (theSA.PositiveRootsK, theSA.SimpleBasisK.size,
     this->HighestWeightsDecompositionMinimalContainingRootSA[i],
     this->MultiplicitiesDecompositionMinimalContainingRootSA[i], buffer, theGlobalVariables);
  }
}

//The below code is related to sl(2) subalgebras of simple Lie algebras
void slTwo::ComputeModuleDecomposition
(Vectors<Rational>& positiveRootsContainingRegularSA, int dimensionContainingRegularSA,
 List<int>& outputHighestWeights, List<int>& outputMultiplicitiesHighestWeights,
 List<int>& outputWeightSpaceDimensions, GlobalVariables& theGlobalVariables)
{ int IndexZeroWeight=positiveRootsContainingRegularSA.size*2;
  outputWeightSpaceDimensions.initFillInObject(4*positiveRootsContainingRegularSA.size+1, 0);
  outputWeightSpaceDimensions[IndexZeroWeight]=dimensionContainingRegularSA;
  List<int> BufferHighestWeights;
  Rational tempRat;
  Vectors<Rational> coordsInPreferredSimpleBasis, tempRoots2;
  Matrix<Rational> tempMat;
  positiveRootsContainingRegularSA.GetCoordsInBasis
  (this->preferredAmbientSimpleBasis, coordsInPreferredSimpleBasis, tempRoots2, tempMat);
  for (int k=0; k<positiveRootsContainingRegularSA.size; k++)
  { tempRat=Vector<Rational>::ScalarEuclidean
    (this->hCharacteristic, coordsInPreferredSimpleBasis[k]);
    assert(tempRat.DenShort==1);
    if (tempRat>positiveRootsContainingRegularSA.size*2)
    { std::cout << "This is a programming error. The scalar product of the h-Characteristic "
      << this->hCharacteristic.ToString()
      << " with the simple root " << coordsInPreferredSimpleBasis[k].ToString()
      << " is larger than " << positiveRootsContainingRegularSA.size*2
      << ". The affected sl(2) subalgebra is " << this->ToString()
      << ". " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
      break;
    }
    outputWeightSpaceDimensions[IndexZeroWeight+tempRat.NumShort]++;
    outputWeightSpaceDimensions[IndexZeroWeight-tempRat.NumShort]++;
  }
  BufferHighestWeights=(outputWeightSpaceDimensions);
  outputHighestWeights.ReservE(positiveRootsContainingRegularSA.size*2);
  outputMultiplicitiesHighestWeights.ReservE(positiveRootsContainingRegularSA.size*2);
  outputHighestWeights.size=0;
  outputMultiplicitiesHighestWeights.size=0;
//  this->hCharacteristic.ComputeDebugString();
//  std::cout << "Starting weights:  " << BufferHighestWeights;
  for (int j=BufferHighestWeights.size-1; j>=IndexZeroWeight; j--)
  { int topMult = BufferHighestWeights[j];
    if (topMult<0)
    { std::cout << "This is a programming error: "
      << " the sl(2)-module decomposition "
      << " shows an sl(2)-module with highest weight "
      << topMult << " which is impossible. Here is the sl(2) subalgebra. "
      << this->ToString() << "."
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    if (topMult>0)
    { outputHighestWeights.AddOnTop(j-IndexZeroWeight);
      outputMultiplicitiesHighestWeights.AddOnTop(topMult);
      if (j!=IndexZeroWeight)
        BufferHighestWeights[IndexZeroWeight*2-j]-=topMult;
      for (int k=j-2; k>=IndexZeroWeight; k-=2)
      { BufferHighestWeights[k]-=topMult;
        if (k!=IndexZeroWeight)
          BufferHighestWeights[IndexZeroWeight*2-k]-=topMult;
        if(BufferHighestWeights[k]<0 ||
           !(BufferHighestWeights[k]==BufferHighestWeights[IndexZeroWeight*2-k]))
        { std::cout << " This is a programming error: an error check has failed. "
          << " While trying to decompose with respect to  h-characteristic <br> "
          << this->hCharacteristic.ToString()
          << ". The positive root system of the containing root subalgebra is <br>"
          << positiveRootsContainingRegularSA.ToString() << ". "
          << "<br>The preferred simple basis is <br>"
          << this->preferredAmbientSimpleBasis.ToString()
          << ". The coordinates relative to the preferred simple basis are<br>"
          << coordsInPreferredSimpleBasis.ToString()
          << " The starting weights list is <br>"
          << outputWeightSpaceDimensions << ". "
          << " I got that the root space of index  "
          <<  k+1 << " has negative dimension. Something is wrong. "
          << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
          assert(false);
        }
      }
    }
  }
}

void SltwoSubalgebras::ElementToStringModuleDecompositionMinimalContainingRegularSAs(std::string& output, bool useLatex, bool useHtml)
{ std::string tempS;
  std::stringstream out;
  for (int i=0; i<this->size; i++)
  { (*this)[i].ElementToStringModuleDecompositionMinimalContainingRegularSAs(useLatex, useHtml, *this, tempS);
    out << tempS;
    if (useHtml)
      out << "\n<br>";
    out << "\n";
  }
  output=out.str();
}

std::string SltwoSubalgebras::ElementToStringNoGenerators(FormatExpressions* theFormat)
{ std::string tempS; std::stringstream out;
  std::string tooltipHchar="Let h be in the Cartan s.a. Let \\alpha_1, ..., \\alpha_n be simple Vectors<Rational> w.r.t. h. Then the h-characteristic is the n-tuple (\\alpha_1(h), ..., \\alpha_n(h))";
  std::string tooltipVDecomposition= "The sl(2) submodules of g are parametrized by their highest weight w.r.t. h. V_l is l+1 dimensional";
  std::string tooltipContainingRegular="A regular semisimple subalgebra might contain an sl(2) such that the sl(2) has no centralizer in the regular semisimple subalgebra, but the regular semisimple subalgebra might fail to be minimal containing. This happens when another minimal containing regular semisimple subalgebra of equal rank nests as a Vector<Rational> subalgebra in the containing SA. See Dynkin, Semisimple Lie subalgebras of semisimple Lie algebras, remark before Theorem 10.4.";
  std::string tooltipHvalue="The actual realization of h. The coordinates of h are given with respect to the fixed original simple basis. Note that the characteristic of h is given *with respect to another basis* (namely, with respect to an h-positive simple basis). I will fix this in the future (email me if you want that done sooner).";
  bool usePNG=true;
  bool useHtml=theFormat==0 ? true : theFormat->flagUseHTML;
  bool useLatex=theFormat==0 ? true : theFormat->flagUseLatex;
  out << "Number of sl(2) subalgebras "<< this->size<<"\n";
  if (this->IndicesSl2decompositionFlas.size < this->size)
    usePNG = false;
  if(useHtml)
    out << "<br><br><table><tr><td style=\"padding-right:20px\">"
    << CGI::ElementToStringTooltip("Characteristic", tooltipHchar)
    << "</td><td align=\"center\" title=\"" << tooltipHvalue
    << "\"> h</td><td style=\"padding-left:20px\" title=\""
    << tooltipVDecomposition
    << "\"> Decomposition of ambient Lie algebra</td>"
    << "<td>Minimal containing regular semisimple SAs</td><td title=\""
    << tooltipContainingRegular << "\">Containing regular semisimple SAs in "
    << "which the sl(2) has no centralizer</td> </tr>";
  if (this->BadHCharacteristics.size>0)
  { if (useHtml)
      out << "<tr><td><b>Bad values of h</b></td><td>";
    tempS= this->BadHCharacteristics.ToString();
    out << tempS;
    if (useHtml)
      out << "</td></tr>";
  }
  for (int i=0; i<this->size; i++)
  { slTwo& theSl2= (*this)[i];
    if (useHtml)
      out << "<tr><td style=\"padding-right:20px\"><a href=\"./sl2s.html#sl2index"
      << i << "\"title=\"" << tooltipHchar << "\" >";
    out << theSl2.hCharacteristic.ToString();
    if (useHtml)
      out << "</a></td><td title=\"" << tooltipHvalue << "\">";
    out << theSl2.theH.GetCartanPart().ToString();
    if(!this->GetOwnerWeyl().IsDominantWeight(theSl2.theH.GetCartanPart()))
      out << "<b>This is wrong!!!!!!!!!!!!! The h is not dual to a dominant weight... </b>";
    if (useHtml)
      out << "</td><td style=\"padding-left:20px\" title=\"" << tooltipVDecomposition << "\">";
    if (useHtml && usePNG)
      out << "<img src=\"./fla" << this->IndicesSl2decompositionFlas[i]+1 << ".png\"></td><td>";
    else
    { theSl2.ElementToStringModuleDecomposition(useLatex, useHtml, tempS);
      out << tempS;
      if (useHtml)
        out << "</td><td>";
    }
    for (int j=0; j<theSl2.IndicesMinimalContainingRootSA.size; j++)
    { rootSubalgebra& currentSA= this->theRootSAs[theSl2.IndicesMinimalContainingRootSA[j]];
      CGI::clearDollarSigns(currentSA.theDynkinDiagram.DynkinStrinG, tempS);
      out << "<a href=\"../rootHtml_rootSA" << theSl2.IndicesMinimalContainingRootSA[j]
      << ".html\">" << tempS << "</a>" << ";  ";
    }
    if (useHtml)
      out << "</td><td title=\"" << tooltipContainingRegular << "\">";
    for (int j=0; j<theSl2.IndicesContainingRootSAs.size; j++)
    { rootSubalgebra& currentSA= this->theRootSAs[theSl2.IndicesContainingRootSAs[j]];
      CGI::clearDollarSigns(currentSA.theDynkinDiagram.DynkinStrinG, tempS);
      out << "<a href=\"../rootHtml_rootSA" << theSl2.IndicesContainingRootSAs[j] << ".html\">"
      << tempS << "</a>" << ";  ";
    }
    if (useHtml)
      out << "</td></tr>\n";
  }
  if (useHtml)
    out << "</table><HR width=\"100%\">";
  return out.str();
}

std::string SltwoSubalgebras::ToString(FormatExpressions* theFormat)
{ std::string tempS; std::stringstream out; std::stringstream body;
  bool useHtml= theFormat==0 ? true : theFormat->flagUseHTML;
  for (int i=0; i<this->size; i++)
  { tempS=(*this)[i].ToString(theFormat);
  //  body<< "Index "<< i<<": ";
    if(useHtml)
      body << "<br>";
    body << tempS;
    if(useHtml)
      body << "<HR width=\"100%\">";
  }
  if(useHtml)
    out << "<br>";
  out << this->ElementToStringNoGenerators(theFormat);
  out << body.str();
  return out.str();
}

void SltwoSubalgebras::ElementToHtml
(FormatExpressions* theFormat, GlobalVariables* theGlobalVariables)
{ std::string physicalPathSAs= theFormat==0 ? "": theFormat->physicalPath;
  std::string htmlPathServerSAs= theFormat==0 ? "": theFormat->htmlPathServer;
  std::string physicalPathSl2s= theFormat==0 ? "": theFormat->physicalPath+"sl2s/";
  std::string htmlPathServerSl2s= theFormat==0 ? "": theFormat->htmlPathServer+"sl2s/";

  ProgressReport theReport(theGlobalVariables);
  theReport.Report("Preparing html pages for sl(2) subalgebras. This might take a while.");
  std::string tempS;
  std::string DisplayNameCalculator= theFormat==0 ? "" : theFormat->DisplayNameCalculator;
  this->theRootSAs.ElementToHtml
  (tempS, physicalPathSAs, htmlPathServerSAs, this, theGlobalVariables);
  bool usePNG=true;
  if (physicalPathSAs=="")
    usePNG=false;
  if(usePNG)
  { int numExpectedFiles= this->size*8;
    this->texFileNamesForPNG.ReservE(numExpectedFiles);
    this->texStringsEachFile.ReservE(numExpectedFiles);
    this->listSystemCommandsLatex.ReservE(numExpectedFiles);
    this->listSystemCommandsDVIPNG.ReservE(numExpectedFiles);
  }
  this->texFileNamesForPNG.size=0;
  this->texStringsEachFile.size=0;
  this->listSystemCommandsLatex.size=0;
  this->listSystemCommandsDVIPNG.size=0;
  std::stringstream out, outNotation, outNotationCommand;
  std::string fileName;
  std::fstream theFile, fileFlas;
  outNotationCommand << "printSemisimpleLieAlgebra{}" << this->GetOwnerWeyl().WeylLetter
  << "_" << this->GetOwnerWeyl().GetDim();
  outNotation
  << "Notation, structure constants and Weyl group info: "
  << CGI::GetCalculatorLink(DisplayNameCalculator, outNotationCommand.str())
  << "<br> <a href=\""<< DisplayNameCalculator << "\"> Calculator main page</a><br><a href=\"../rootHtml.html\">Root subsystem table</a><br>";
  std::string notation= outNotation.str();
  out << this->ToString(theFormat);
  if(usePNG)
  { fileName= physicalPathSl2s;
    fileName.append("sl2s.html");
    CGI::OpenFileCreateIfNotPresent(theFile, fileName, false, true, false);
    tempS= out.str();
    theFile << "<HMTL>"
    << "<title>sl(2)-subalgebras of "
    << this->theRootSAs[0].theDynkinDiagram.ElementToStrinG(false, true) << "</title>";
    theFile << "<meta name=\"keywords\" content=\""
    <<  this->theRootSAs[0].theDynkinDiagram.ElementToStrinG(false, true)
    << " sl(2)-triples, sl(2)-subalgebras, nilpotent orbits simple Lie algebras,"
    << " nilpotent orbits of "
    <<  this->theRootSAs[0].theDynkinDiagram.ElementToStrinG(false, true)
    << ", sl(2)-triples of "
    << this->theRootSAs[0].theDynkinDiagram.ElementToStrinG(false, true)
    << " \">";
    theFile << "<BODY>" << notation << "<a href=\"" << htmlPathServerSl2s
    << "sl2s_nopng.html\"> plain html for your copy+paste convenience</a><br>\n"
    << tempS << "</HTML></BODY>";
    theFile.close();
  }
  fileName= physicalPathSl2s;
  fileName.append("sl2s_nopng.html");
  tempS = this->ToString(theFormat);
  CGI::OpenFileCreateIfNotPresent(theFile, fileName, false, true, false);
  theFile << "<HMTL><BODY>" << notation << "<a href=\""
  << htmlPathServerSl2s << "sl2s.html\"> "
  << ".png rich html for your viewing pleasure</a><br>\n" << tempS << "</HTML></BODY>";
  theFile.close();
  if (usePNG)
  { this->listSystemCommandsLatex.SetSize(this->texFileNamesForPNG.size);
    this->listSystemCommandsDVIPNG.SetSize(this->texFileNamesForPNG.size);
    for (int i=0; i<this->texFileNamesForPNG.size; i++)
    { CGI::OpenFileCreateIfNotPresent(fileFlas, this->texFileNamesForPNG[i], false, true, false);
      fileFlas << "\\documentclass{article}\\begin{document}\\pagestyle{empty}\n"
      << this->texStringsEachFile.TheObjects[i] << "\n\\end{document}";
      std::stringstream tempStreamLatex, tempStreamPNG;
      tempStreamLatex << "latex " << " -output-directory="
      << physicalPathSl2s << " " << this->texFileNamesForPNG[i];
      tempS= this->texFileNamesForPNG[i];
      tempS.resize(tempS.size()-4);
      tempStreamPNG << "dvipng " << tempS << ".dvi -o " << tempS << ".png -T tight";
      this->listSystemCommandsLatex[i]= tempStreamLatex.str();
      this->listSystemCommandsDVIPNG[i]=tempStreamPNG.str();
      fileFlas.close();
    }
  }
}
