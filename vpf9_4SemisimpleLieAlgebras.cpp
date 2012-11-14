//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader1_4SemisimpleLieAlgebras.h"
ProjectInformationInstance ProjectInfoVpf9_4cpp
(__FILE__, "Implementation of semisimple subalgebra routines. ");

std::string SemisimpleSubalgebras::ToString(FormatExpressions* theFormat)
{ std::stringstream out;
  out << "There are " << this->Hcandidates.size << " candidates total. ";
  for (int i=0; i<this->Hcandidates.size; i++)
    out << "<br>" << this->Hcandidates[i].ToString(theFormat);
  out << this->theSl2s.ToString(theFormat);
  return out.str();
}

void SemisimpleSubalgebras::FindTheSSSubalgebras
(List<SemisimpleLieAlgebra>* newOwner, int newIndexInOwner, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("SemisimpleSubalgebras::FindTheSSSubalgebras");
  this->owners=newOwner;
  this->indexInOwners=newIndexInOwner;
  this->GetSSowner().FindSl2Subalgebras
  (*this->owners, this->indexInOwners, this->theSl2s, *theGlobalVariables);
  this->FindTheSSSubalgebrasPart2(theGlobalVariables);
}

void SemisimpleSubalgebras::FindTheSSSubalgebrasPart2
(GlobalVariables* theGlobalVariables)
{ CandidateSSSubalgebra emptyCandidate;
  this->ExtendCandidatesRecursive(emptyCandidate, theGlobalVariables);
}

void SemisimpleSubalgebras::ExtendOneComponentRecursive
(const CandidateSSSubalgebra& baseCandidate,
GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("SemisimpleSubalgebras::ExtendOneComponentRecursive");
  RecursionDepthCounter theCounter(&this->theRecursionCounter);
  int numVectorsFound=baseCandidate.CartanSAsByComponent.LastObject()->size;
  CandidateSSSubalgebra newCandidate;
  DynkinSimpleType& theNewTypE=*baseCandidate.theTypes.LastObject();
  if (numVectorsFound==theNewTypE.theRank)
  { newCandidate=baseCandidate;
    if (!newCandidate.ComputeChar(this->GetSSowner().theWeyl, *this->owners, this->indexInOwners))
      return;
    this->Hcandidates.AddOnTop(newCandidate);
    this->ExtendCandidatesRecursive(newCandidate, theGlobalVariables);
    return;
  }
  ProgressReport theReport(theGlobalVariables);
  ProgressReport theReport2(theGlobalVariables);
  ProgressReport theReport0(theGlobalVariables);
  int indexCurrentWeight=
  baseCandidate.theSymmetricCartanScaled.NumRows- theNewTypE.theRank+numVectorsFound;
  Rational desiredLengthSquared=
  baseCandidate.theSymmetricCartanScaled(indexCurrentWeight, indexCurrentWeight);
  Vectors<Rational> startingVector;
  HashedList<Vector<Rational> > theOrbit;
  startingVector.SetSize(1);
  std::stringstream out0;
  if (theGlobalVariables!=0)
  { out0 << "Attempting to complete component " << theNewTypE.ToString()
    << ", root " << baseCandidate.CartanSAsByComponent.LastObject()->size+1
    << " out of " << theNewTypE.theRank << ".";
    theReport0.Report(out0.str());
  }
  for (int i=0; i<this->theSl2s.size; i++)
  { if (this->theSl2s[i].LengthHsquared!=desiredLengthSquared)
      continue;
    startingVector[0]=this->theSl2s[i].theH.GetCartanPart();
    std::stringstream out;
    if (theGlobalVariables!=0)
    { out << "Generating orbit of " << startingVector[0].ToString() << "...";
      theReport.Report(out.str());
    }
    if (indexCurrentWeight!=0)
      this->GetSSowner().theWeyl.GenerateOrbit
      (startingVector, false, theOrbit, false, 1152, 0, 10000);
    else
      theOrbit=startingVector;
    if (theGlobalVariables!=0)
    { out << " done. The size of the orbit is " << theOrbit.size;
      theReport.Report(out.str());
    }
    for (int j=0; j<theOrbit.size; j++)
    { std::stringstream out2;
      if (theGlobalVariables!=0)
      { out2 << "Exploring orbit element of index  " << j+1 << " out of "
        << theOrbit.size << ".";
        theReport2.Report(out2.str());
      }
      if (baseCandidate.isGoodForTheTop(this->GetSSowner().theWeyl, theOrbit[j]))
      { if (theGlobalVariables!=0)
        { out2 << " the candidate is good. Attempting to extend it by recursion. ";
          theReport2.Report(out2.str());
        }
        //std::cout << "Is good!";
        newCandidate=baseCandidate;
        newCandidate.AddHincomplete(this->GetSSowner().theWeyl, theOrbit[j]);
        this->ExtendOneComponentRecursive(newCandidate, theGlobalVariables);
      } else
      { if (theGlobalVariables!=0)
        { out2 << " the candidate is no good. ";
          theReport2.Report(out2.str());
        }
      }
    }
  }
}

void CandidateSSSubalgebra::AddHincomplete(WeylGroup& ownerWeyl, const Vector<Rational>& theH)
{ if (this->CartanSAsByComponent.size==1)
    if (this->CartanSAsByComponent.LastObject()->size==0)
      this->PosRootsPerpendicularPrecedingWeights=ownerWeyl.RootsOfBorel;
  for (int i=0; i<this->PosRootsPerpendicularPrecedingWeights.size; i++)
    if (ownerWeyl.RootScalarCartanRoot(this->PosRootsPerpendicularPrecedingWeights[i], theH)!=0)
    { this->PosRootsPerpendicularPrecedingWeights.PopIndexShiftDown(i);
      i--;
    }
  this->CartanSAsByComponent.LastObject()->AddOnTop(theH);
}

void CandidateSSSubalgebra::AddTypeIncomplete(const DynkinSimpleType& theNewType)
{ if (theNewType.theRank<=0)
  { std::cout << "This is a programming error: I am given a simple Dynkin type of non-positive "
    << " rank. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  WeylGroup tempWeyl;
  tempWeyl.MakeArbitrary(theNewType.theLetter, theNewType.theRank);
  Rational scale=theNewType.lengthFirstSimpleRootSquared/tempWeyl.CartanSymmetric(0,0);
  tempWeyl.CartanSymmetric*=scale;
  this->theSymmetricCartanScaled.DirectSumWith(tempWeyl.CartanSymmetric);
  this->CartanSAsByComponent.SetSize(this->CartanSAsByComponent.size+1);
  this->CartanSAsByComponent.LastObject()->size=0;
  this->theTypes.AddOnTop(theNewType);
}

bool CandidateSSSubalgebra::isGoodForTheTop
  (WeylGroup& ownerWeyl, const Vector<Rational>& Hnew)const
{ MacroRegisterFunctionWithName("CandidateSSSubalgebra::isGoodForTheTop");
  Rational theScalarProd;
  int counter=-1;
  int indexHnew= this->theSymmetricCartanScaled.NumRows-
  this->theTypes.LastObject()->theRank+this->CartanSAsByComponent.LastObject()->size;
  for (int k=0; k<this->CartanSAsByComponent.size; k++)
  { Vectors<Rational>& currentComponent=this->CartanSAsByComponent[k];
    for (int l=0; l<currentComponent.size; l++)
    { counter++;
      theScalarProd= ownerWeyl.RootScalarCartanRoot
      (Hnew, currentComponent[l]);
      if (theScalarProd!=this->theSymmetricCartanScaled(indexHnew, counter))
        return false;
    }
  }
  for (int i=0; i<this->PosRootsPerpendicularPrecedingWeights.size; i++)
    if (ownerWeyl.RootScalarCartanRoot(Hnew, this->PosRootsPerpendicularPrecedingWeights[i])<0)
      return false;
  for (int i=0; i<this->CartanSAsByComponent.size; i++)
    if (this->CartanSAsByComponent[i].Contains(Hnew))
    { std::cout << "This is a programming error: I am told that "
      << Hnew.ToString() << " is an OK weight to extend the weight subsystem, "
      << " but the weight subsystem contains that weight already: "
      << this->CartanSAsByComponent[i].ToString()
      << ". "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
  if (indexHnew==0)
    return ownerWeyl.IsDominantWeight(Hnew);
  return true;
}

bool CandidateSSSubalgebra::ComputeChar
(WeylGroup& ownerWeyl, List<SemisimpleLieAlgebra>& owners, int indexInOwners)
{ this->theWeylSubgroup.AmbientWeyl=ownerWeyl;
  this->theWeylSubgroup.simpleGenerators.size=0;
  for (int i=0; i<this->CartanSAsByComponent.size; i++)
    this->theWeylSubgroup.simpleGenerators.AddListOnTop(this->CartanSAsByComponent[i]);
  this->theWeylSubgroup.ComputeRootSubsystem();
  this->theCharFundamentalCoords.MakeZero(this->theSymmetricCartanScaled.NumRows);
  Polynomial<Rational> startingChar;
  MonomialP tempMon;
  tempMon.MakeConst(this->theSymmetricCartanScaled.NumRows);
  startingChar.AddMonomial(tempMon, ownerWeyl.GetDim());
  for (int i=0; i<ownerWeyl.RootSystem.size; i++)
    for (int j=0; j<ownerWeyl.CartanSymmetric.NumRows; j++)
    {
    }
  return true;
}

void SemisimpleSubalgebras::ExtendCandidatesRecursive
  (const CandidateSSSubalgebra& baseCandidate, GlobalVariables* theGlobalVariables)
{ RecursionDepthCounter theCounter(&this->theRecursionCounter);
  MacroRegisterFunctionWithName("SemisimpleSubalgebras::ExtendCandidatesRecursive");
  DynkinSimpleType theType;
  //std::cout << "The types to be tested: ";
  CandidateSSSubalgebra theCandidate;
  SemisimpleLieAlgebra tempAlgebra;
  ProgressReport theProgressReport1(theGlobalVariables);
  ProgressReport theProgressReport2(theGlobalVariables);
  ProgressReport theProgressReport3(theGlobalVariables);
  if(theGlobalVariables!=0)
  { std::stringstream out;
    out << "\nExploring extensions of (" << baseCandidate.ToString() << ") via: ";
    out << "\n";
    theProgressReport1.Report(out.str());
    //std::cout << "Exploring extensions of " << baseCandidate.ToString() << " by: ";
  }
  DynkinSimpleType myType;
  myType.MakeAone();
  myType.theLetter=this->GetSSowner().theWeyl.WeylLetter;
  myType.theRank=this->GetSSowner().GetRank();
  myType.lengthFirstSimpleRootSquared=this->theSl2s.LastObject()->LengthHsquared;
  //std::cout << myType.ToString();
  for (theType.MakeAone(); theType<myType; theType++)
    for (int k=0; k<this->theSl2s.size; k++)
    { theType.lengthFirstSimpleRootSquared=this->theSl2s[k].LengthHsquared;
      if (!(theType<myType))
        break;
      //std::cout << theType.ToString() << ", ";
      theCandidate=baseCandidate;
      tempAlgebra.theWeyl.MakeArbitrary(theType.theLetter, theType.theRank);
      int indexSubalgebra=this->SimpleComponentsSubalgebras.IndexOfObject(tempAlgebra);
      bool mustComputeSSalgebra=(indexSubalgebra==-1);
      if (mustComputeSSalgebra)
      { indexSubalgebra=this->SimpleComponentsSubalgebras.size;
        this->SimpleComponentsSubalgebras.AddOnTop(tempAlgebra);
        this->theSl2sOfSubalgebras.SetSize(this->theSl2sOfSubalgebras.size+1);
      }
      SemisimpleLieAlgebra& theSmallAlgebra=this->SimpleComponentsSubalgebras[indexSubalgebra];
      SltwoSubalgebras& theSmallSl2s=this->theSl2sOfSubalgebras[indexSubalgebra];
      if (mustComputeSSalgebra)
      { std::stringstream tempStream;
        tempStream << "\nGenerating simple Lie algebra "
        << SemisimpleLieAlgebra::GetLieAlgebraName(theType.theLetter, theType.theRank)
        << " (total " << this->SimpleComponentsSubalgebras.size << ")...";
        theProgressReport2.Report(tempStream.str());
        theSmallAlgebra.init
        (this->SimpleComponentsSubalgebras, indexSubalgebra, theType.theLetter, theType.theRank);
        theSmallAlgebra.ComputeChevalleyConstantS(theGlobalVariables);
        theSmallAlgebra.FindSl2Subalgebras
        (this->SimpleComponentsSubalgebras, theSmallAlgebra.indexInOwner,
         theSmallSl2s, *theGlobalVariables);
        tempStream << " done.";
        theProgressReport2.Report(tempStream.str());
      }
      Rational desiredScale=
      theType.lengthFirstSimpleRootSquared/theSmallAlgebra.theWeyl.CartanSymmetric(0,0);
      bool isGood=true;
      for (int i=0; i<theSmallSl2s.size; i++)
      { isGood=false;
        Rational impliedRootLength= desiredScale * theSmallSl2s[i].LengthHsquared;
        for (int j=0; j<this->theSl2s.size; j++)
        { if (!theSmallSl2s[i].ModuleDecompositionFitsInto(this->theSl2s[j]) ||
              !(impliedRootLength==this->theSl2s[j].LengthHsquared))
            continue;
          isGood=true;
          break;
        }
        if (!isGood)
          break;
      }
      if (theGlobalVariables!=0)
      { std::stringstream out;
        out << " \n" << theType.ToString();
        if (isGood)
          out << " has fitting sl(2) decompositions.";
        else
          out << " does not have fitting sl(2) decompositions.";
        theProgressReport3.Report(out.str());
      }
      if (isGood)
      { theCandidate.AddTypeIncomplete(theType);
        this->ExtendOneComponentRecursive(theCandidate, theGlobalVariables);
      }
    }
}

void slTwo::ElementToStringModuleDecompositionMinimalContainingRegularSAs
(bool useLatex, bool useHtml, SltwoSubalgebras& owner, std::string& output)
{ std::stringstream out;
  std::string tempS;
  if (useLatex)
    out << "$";
  if (useHtml)
  { out << "<table><tr><td align=\"center\">Char.</td>";
    for (int i=0; i<this->IndicesMinimalContainingRootSA.size; i++)
    { rootSubalgebra& theSA= owner.theRootSAs[this->IndicesMinimalContainingRootSA[i]];
      CGI::clearDollarSigns(theSA.theDynkinDiagram.DynkinStrinG, tempS);
      out << "<td align=\"center\">Decomp. " << tempS << "</td>";
    }
    out << "</tr>\n";
  }
  out << "<tr><td align=\"center\"> " << this->hCharacteristic.ToString() << "</td>";
  for (int k=0; k<this->IndicesMinimalContainingRootSA.size; k++)
  { rootSubalgebra& theSA= owner.theRootSAs[this->IndicesMinimalContainingRootSA[k]];
    CGI::clearDollarSigns(theSA.theDynkinDiagram.DynkinStrinG, tempS);
    if (useHtml)
      out << "<td align=\"center\">";
    for (int i=0; i<this->HighestWeightsDecompositionMinimalContainingRootSA[k].size; i++)
    { if (this->MultiplicitiesDecompositionMinimalContainingRootSA[k][i]>1)
        out << this->MultiplicitiesDecompositionMinimalContainingRootSA[k][i];
      out << "V_{"<<this->HighestWeightsDecompositionMinimalContainingRootSA[k][i] << "}";
      if (i!=this->HighestWeightsDecompositionMinimalContainingRootSA[k].size-1)
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

void slTwo::ElementToHtmlCreateFormulaOutputReference
(const std::string& formulaTex, std::stringstream& output, bool usePNG, bool useHtml,
 SltwoSubalgebras& container, std::string* physicalPath, std::string* htmlPathServer)
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
  out << "<br>Length of the weight dual to h: "
  << this->LengthHsquared;
  tempS=this->preferredAmbientSimpleBasis.ToString();
  std::string physicalPath="";
  std::string htmlPathServer="";
  bool usePNG=false;
  bool useHtml=true;
  bool useLatex=false;
  if (theFormat!=0)
    if (theFormat->physicalPath!="")
    { physicalPath=theFormat->physicalPath + "sl2s/";
      htmlPathServer=theFormat->htmlPathServer + "sl2s/";
      usePNG=theFormat->flagUsePNG;
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
      currentSA.theDynkinDiagram.ElementToStrinG(tempS, true);
    }
    out << tempS;
    if (useHtml)
      out << "</a>";
  }
  if (useHtml)
    out << "<br>";
  out << "\nsl(2)-module decomposition of the ambient Lie algebra: ";
  this->ElementToStringModuleDecomposition(useLatex, useHtml, tempS);
  this->ElementToHtmlCreateFormulaOutputReference
  ("$"+tempS+"$", out, usePNG, useHtml, *this->container, &physicalPath, &htmlPathServer);
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

void slTwo::ComputeDynkinsEpsilon(WeylGroup& theWeyl)
{//outdates, must be erased as soon as I implement an equivalent
  this->DynkinsEpsilon = this->DiagramM.NumRootsGeneratedByDiagram()+this->DiagramM.RankTotal();
  int r=0;
  for (int i=0; i<this->hCharacteristic.size; i++)
    if (!this->hCharacteristic[i].IsEqualToZero())
      r++;
  this->DynkinsEpsilon+= r;
  for (int i=0; i<theWeyl.RootSystem.size; i++)
  { int NumNonZeroFound=0;
    for (int j=0; j<this->hCharacteristic.size; j++)
    { if (theWeyl.RootSystem[i][j]==1)
        NumNonZeroFound++;
      if(NumNonZeroFound>1)
        break;
    }
    if (NumNonZeroFound==1)
      this->DynkinsEpsilon--;
  }
  this->DynkinsEpsilon=0;
  for (int i=0; i<this->hCharacteristic.size; i++)
    if (this->hCharacteristic[i]==1)
      this->DynkinsEpsilon=-1;
  this->DynkinsEpsilon=0;
}

bool slTwo::ModuleDecompositionFitsInto(const slTwo& other)
{ return this->ModuleDecompositionFitsInto(this->highestWeights, this->multiplicitiesHighestWeights, other.highestWeights, other.multiplicitiesHighestWeights);
}

bool slTwo::ModuleDecompositionFitsInto(const List<int>& highestWeightsLeft, const List<int>& multiplicitiesHighestWeightsLeft, const List<int>& highestWeightsRight, const List<int>& multiplicitiesHighestWeightsRight)
{ for (int i=0; i<highestWeightsLeft.size; i++)
  { int theIndex= highestWeightsRight.IndexOfObject(highestWeightsLeft[i]);
    if (theIndex==-1)
      return false;
    else
      if (multiplicitiesHighestWeightsLeft[i]>multiplicitiesHighestWeightsRight[theIndex])
        return false;
  }
  return true;
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
  output.GetOwner().ComputeChevalleyConstantS(&theGlobalVariables);
  output.theRootSAs.GenerateAllReductiveRootSubalgebrasUpToIsomorphism
  (theGlobalVariables, true, true);
  //output.theRootSAs.ComputeDebugString(false, false, false, 0, 0, theGlobalVariables);
  output.IndicesSl2sContainedInRootSA.SetSize(output.theRootSAs.size);
  output.IndicesSl2sContainedInRootSA.ReservE(output.theRootSAs.size*2);
  for (int i=0; i<output.IndicesSl2sContainedInRootSA.size; i++)
    output.IndicesSl2sContainedInRootSA[i].size=0;
  ProgressReport theReport(&theGlobalVariables);
  for (int i=0; i<output.theRootSAs.size-1; i++)
  { std::stringstream tempStream;
    tempStream << "\nExploring root subalgebra "
    << output.theRootSAs[i].theDynkinDiagram.ElementToStrinG() << "(" << (i+1)
    << " out of " << output.theRootSAs.size-1 << " non-trivial)\n";
    theReport.Report(tempStream.str());
    output.theRootSAs[i].GetSsl2SubalgebrasAppendListNoRepetition
    (output, i, theGlobalVariables);
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
        if (theOtherRootSA.indicesSubalgebrasContainingK.Contains(theSl2.IndicesContainingRootSAs[j]))
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
  Selection simpleRootsChar2;
  Vectors<Rational> RootsWithCharacteristic2;
  Vectors<Rational> reflectedRootsWithCharacteristic2;
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
    simpleRootsChar2=theRootsWithZeroCharacteristic;
    simpleRootsChar2.InvertSelection();
    Vector<Rational> simpleRootsChar2Vect;
    simpleRootsChar2Vect=simpleRootsChar2;
    for (int j=0; j<relativeRootSystem.size; j++)
      if (simpleRootsChar2Vect.ScalarEuclidean(relativeRootSystem[j])==1)
      { theSlack++;
        RootsWithCharacteristic2.AddOnTop(this->PositiveRootsK[j]);
      }
    int theDynkinEpsilon =
    tempDiagram.NumRootsGeneratedByDiagram() + theRelativeDimension - theSlack;
    //if Dynkin's epsilon is not zero the subalgebra cannot be an S sl(2) subalgebra.
    //otherwise, as far as I understand, it always is //
    //except for G_2 (go figure!).
    //(but generatorootsWithZeroCharVectorrs still have to be found)
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
      ////////////////////
      reflectedSimpleBasisK=this->SimpleBasisK;
      for (int k=0; k<reflectedSimpleBasisK.size; k++)
        this->GetAmbientWeyl().ActOn(raisingElt, reflectedSimpleBasisK[k], false, false);
      ////////////////////
      reflectedRootsWithCharacteristic2=RootsWithCharacteristic2;
      for (int k=0; k<reflectedRootsWithCharacteristic2.size; k++)
        this->GetAmbientWeyl().ActOn(raisingElt, reflectedRootsWithCharacteristic2[k], false, false);

      theSl2.theH.MakeHgenerator
      (characteristicH, *theLieAlgebra.owner, theLieAlgebra.indexInOwner);
      theSl2.theE.MakeZero(*theLieAlgebra.owner, theLieAlgebra.indexInOwner);
      theSl2.theF.MakeZero(*theLieAlgebra.owner, theLieAlgebra.indexInOwner);
      //theSl2.ComputeDebugString(false, false, theGlobalVariables);
//      std::cout << "<br>accounting " << characteristicH.ToString();
      if(theLieAlgebra.AttemptExtendingHEtoHEFWRTSubalgebra
         (reflectedRootsWithCharacteristic2, theRootsWithZeroCharacteristic,
          reflectedSimpleBasisK,
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
      { output.BadHCharacteristics.AddOnTop(characteristicH);
/*        std::cout << "<br>obtained bad characteristic "
        << characteristicH.ToString() << ". The zero diagram is "
        << tempDiagram.ElementToStrinG()
        << "; the Dynkin epsilon is " << theDynkinEpsilon
        << "= the num roots generated by diagram " << tempDiagram.NumRootsGeneratedByDiagram()
        << " + the relative dimension " << theRelativeDimension
        << " - the slack " << theSlack
        << "<br>The relative root system is: "
        << relativeRootSystem.ToString();
        std::cout << "<br> I was exploring " << this->ToString();*/
      }
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

void slTwo::ElementToStringModuleDecomposition(bool useLatex, bool useHtml, std::string& output)
{ std::stringstream out;
  for (int i=0; i<this->highestWeights.size; i++)
  { if (this->multiplicitiesHighestWeights[i]>1)
      out << this->multiplicitiesHighestWeights[i];
    out << "V_{" << this->highestWeights[i] << "}";
    if (i!=this->highestWeights.size-1)
      out << "+";
  }
  output=out.str();
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

void slTwo::MakeReportPrecomputations
(GlobalVariables& theGlobalVariables, SltwoSubalgebras& container, int indexInContainer,
 int indexMinimalContainingRegularSA, rootSubalgebra& MinimalContainingRegularSubalgebra)
{ MacroRegisterFunctionWithName("slTwo::MakeReportPrecomputations");
  int theDimension=this->GetOwnerSSAlgebra().GetRank();
  this->IndicesContainingRootSAs.size=0;
  Vectors<Rational> tempRoots;
  tempRoots=(MinimalContainingRegularSubalgebra.SimpleBasisK);
  this->GetOwnerSSAlgebra().theWeyl.TransformToSimpleBasisGeneratorsWRTh
  (tempRoots, this->theH.GetCartanPart());
  DynkinDiagramRootSubalgebra theDiagram;
  theDiagram.ComputeDiagramTypeKeepInput(tempRoots, this->GetOwnerSSAlgebra().theWeyl);
  this->IndicesContainingRootSAs.AddOnTop(indexMinimalContainingRegularSA);
  tempRoots.MakeEiBasis(theDimension);
  this->GetOwnerSSAlgebra().theWeyl.TransformToSimpleBasisGeneratorsWRTh
  (tempRoots, this->theH.GetCartanPart());
  DynkinDiagramRootSubalgebra tempDiagram;
  tempDiagram.ComputeDiagramTypeKeepInput(tempRoots, this->GetOwnerSSAlgebra().theWeyl);
  this->preferredAmbientSimpleBasis=tempRoots;
  this->hCharacteristic.SetSize(theDimension);
  for (int i=0; i<theDimension; i++)
    this->hCharacteristic[i]=
    this->GetOwnerSSAlgebra().theWeyl.RootScalarCartanRoot
    (this->theH.GetCartanPart(), this->preferredAmbientSimpleBasis[i]);
  this->LengthHsquared=
  this->GetOwnerSSAlgebra().theWeyl.RootScalarCartanRoot
  (this->theH.GetCartanPart(), this->theH.GetCartanPart());
  //this->hCharacteristic.ComputeDebugString();
//  if (this->theE.NonZeroElements.MaxSize==this->owner->theWeyl.RootSystem.size
//      && this->theF.NonZeroElements.MaxSize==this->owner->theWeyl.RootSystem.size
//      && this->theH.NonZeroElements.MaxSize==this->owner->theWeyl.RootSystem.size)
  this->GetOwnerSSAlgebra().LieBracket(this->theE, this->theF, this->bufferEbracketF);
//  std:: cout << "[" << this->theE.ToString() << ", " << this->theF.ToString() << "]="
//  << this->bufferEbracketF.ToString();
  this->GetOwnerSSAlgebra().LieBracket(this->theH, this->theE, this->bufferHbracketE);
  this->GetOwnerSSAlgebra().LieBracket(this->theH, this->theF, this->bufferHbracketF);

  //theSl2.hCharacteristic.ComputeDebugString();
//  this->ComputeModuleDecomposition();
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
  bool usePNG=theFormat==0? false : theFormat->flagUsePNG;
  bool useHtml=theFormat==0 ? true : theFormat->flagUseHTML;
  bool useLatex=theFormat==0 ? true : theFormat->flagUseLatex;
  std::string physicalPath, displayPath;
  physicalPath=theFormat==0 ? "../" : theFormat->physicalPath;
  displayPath=theFormat==0 ? "../" : theFormat->htmlPathServer;
  out << "Number of sl(2) subalgebras " << this->size << ".\n";
  if (this->IndicesSl2decompositionFlas.size < this->size)
    usePNG = false;
  std::stringstream out2;
  out2
    << "<br> Given a root subsystem P, and a root subsubsystem P_0, in (10.2) "
    << " of Semisimple subalgebras of semisimple Lie algebras, E. Dynkin defines "
    << " a numerical constant e(P, P_0) (which we call Dynkin epsilon). "
    << " <br>In Theorem 10.3, Dynkin proves that if an sl(2) is an S-subalgebra in "
    << " the root subalgebra generated by P, such that it has characteristic 2 "
    << " for all simple roots of P lying in P_0, then e(P, P_0)=0. ";
  if (this->BadHCharacteristics.size>0)
  { bool allbadAreGoodInTheirBadness=true;
    for (int i=0; i<this->BadHCharacteristics.size; i++)
    { bool isGoodInItsbaddness=false;
      for (int j=0; j<this->size; j++)
        if (this->BadHCharacteristics[i]==(*this)[j].theH.GetCartanPart())
        { isGoodInItsbaddness=true;
          break;
        }
      if (!isGoodInItsbaddness)
      { allbadAreGoodInTheirBadness=false;
        out << "<b>Bad characteristic: " << this->BadHCharacteristics[i]
        << "</b>";
      }
      else
      { out2
        << "<br><b>It turns out that in the current case of Cartan element h = "
        << this->BadHCharacteristics[i] << " we have that, for a certain P, "
        << " e(P, P_0) equals 0, but I failed to realize the corresponding sl(2) "
        << " as a subalgebra of that P. However, it turns out that h "
        << " is indeed an S-subalgebra of a smaller root subalgebra P'.</b>"
        ;
      }
    }
    if (!allbadAreGoodInTheirBadness)
      out << "<b>found bad characteristics!</b>";
  } else
  { out2 << "It turns out by direct computation, that in current case of "
    << this->GetOwner().GetLieAlgebraName()
    << ",  e(P,P_0)=0 implies that an S-sl(2) subalgebra of the root subalgebra generated by "
    << " P with characteristic with 2's in the simple roots of P_0 "
    << " always exists. Note that Theorem 10.3 is stated in one "
    << " direction only.";
  }
  if(useHtml)
    out << "<br><br><table><tr><td style=\"padding-right:20px\">"
    << CGI::ElementToStringTooltip("Characteristic", tooltipHchar)
    << "</td><td align=\"center\" title=\"" << tooltipHvalue
    << "\"> h</td><td style=\"padding-left:20px\" title=\""
    << tooltipVDecomposition
    << "\"> Decomposition of ambient Lie algebra</td>"
    << "<td>The square of the length of the weight dual to h.</td>"
    << "<td>Dynkin index = (previous column) *(long root length ambient algebra)^2/4</td>"
    << "<td>Minimal containing regular semisimple SAs</td><td title=\""
    << tooltipContainingRegular << "\">Containing regular semisimple SAs in "
    << "which the sl(2) has no centralizer</td> </tr>";

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
      out << "<img src=\"./fla"
      << this->IndicesSl2decompositionFlas[i]+1 << ".png\"></td><td>";
    else
    { theSl2.ElementToStringModuleDecomposition(useLatex, useHtml, tempS);
      out << tempS;
      if (useHtml)
        out << "</td><td>";
    }
    out << theSl2.LengthHsquared;
    if (useHtml)
      out << "</td><td>";
    out << theSl2.LengthHsquared * this->GetOwnerWeyl().GetLongestRootLengthSquared()/4;
    if (useHtml)
      out << "</td><td>";
    for (int j=0; j<theSl2.IndicesMinimalContainingRootSA.size; j++)
    { rootSubalgebra& currentSA= this->theRootSAs[theSl2.IndicesMinimalContainingRootSA[j]];
      CGI::clearDollarSigns(currentSA.theDynkinDiagram.DynkinStrinG, tempS);
      out << "<a href=\""
      << displayPath
      << "rootHtml_rootSA" << theSl2.IndicesMinimalContainingRootSA[j]
      << ".html\">" << tempS << "</a>" << ";  ";
    }
    if (useHtml)
      out << "</td><td title=\"" << tooltipContainingRegular << "\">";
    for (int j=0; j<theSl2.IndicesContainingRootSAs.size; j++)
    { rootSubalgebra& currentSA= this->theRootSAs[theSl2.IndicesContainingRootSAs[j]];
      CGI::clearDollarSigns(currentSA.theDynkinDiagram.DynkinStrinG, tempS);
      out << "<a href=\""
      <<  displayPath
      << "rootHtml_rootSA" << theSl2.IndicesContainingRootSAs[j] << ".html\">"
      << tempS << "</a>" << ";  ";
    }
    if (useHtml)
      out << "</td></tr>\n";
  }
  if (useHtml)
    out << "</table><HR width=\"100%\">";
  out << out2.str() << "<hr>";
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
  << "<br> <a href=\""<< DisplayNameCalculator
  << "\"> Calculator main page</a><br><a href=\"../rootHtml.html\">Root subsystem table</a><br>";
  std::string notation= outNotation.str();
  out << this->ToString(theFormat);
  if(usePNG)
  { fileName= physicalPathSl2s;
    fileName.append("sl2s.html");
    CGI::OpenFileCreateIfNotPresent(theFile, fileName, false, true, false);
    tempS= out.str();
    theFile << "<HMTL>"
    << "<title>sl(2)-subalgebras of "
    << this->theRootSAs[0].theDynkinDiagram.ElementToStrinG(true) << "</title>";
    theFile << "<meta name=\"keywords\" content=\""
    <<  this->theRootSAs[0].theDynkinDiagram.ElementToStrinG(true)
    << " sl(2)-triples, sl(2)-subalgebras, nilpotent orbits simple Lie algebras,"
    << " nilpotent orbits of "
    <<  this->theRootSAs[0].theDynkinDiagram.ElementToStrinG(true)
    << ", sl(2)-triples of "
    << this->theRootSAs[0].theDynkinDiagram.ElementToStrinG(true)
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
      << this->texStringsEachFile[i] << "\n\\end{document}";
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

void rootSubalgebra::ToString
(std::string& output, SltwoSubalgebras* sl2s, int indexInOwner, bool useLatex,
 bool useHtml, bool includeKEpsCoords, GlobalVariables* theGlobalVariables)
{ std::stringstream out;
  std::string tempS;
  std::string latexFooter, latexHeader;
  if (this->SimpleBasisgEpsCoords.size!=this->SimpleBasisK.size || this->SimpleBasisKEpsCoords.size!= this->SimpleBasisK.size || this->kModulesgEpsCoords.size!= this->kModules.size || this->kModulesKepsCoords.size!= this->kModules.size)
    includeKEpsCoords=false;
  int LatexLineCounter=0;
  this->ElementToStringHeaderFooter(latexHeader, latexFooter, useLatex, useHtml, includeKEpsCoords);
  this->theDynkinDiagram.ElementToStrinG(tempS, true);
  if (useLatex)
    out << "\\noindent$\\mathfrak{k}_{ss}:$ ";
  else
    out << "k_{ss}: ";
  out << tempS;
  if (sl2s!=0)
  { out <<" &nbsp&nbsp&nbsp Contained in: ";
    for (int i=0; i<this->indicesSubalgebrasContainingK.size; i++)
    { if (useHtml)
        out << "<a href=\"./rootHtml_rootSA"
        << this->indicesSubalgebrasContainingK[i] << ".html\">";
      out << tempS;
      if (useHtml)
        out << "</a>, ";
    }
    if (useHtml)
      out << "<br> <a href=\"./rootHtml.html\">Back to root subsystem table </a> ";
  }
  tempS=this->SimpleBasisK.ToString();
  if (useHtml)
    out << "\n<br>\n";
  if (useLatex)
    out << "\n\\noindent";
  out << " Simple basis: "<< tempS;
  tempS=this->SimpleBasisgEpsCoords.ElementToStringEpsilonForm(useLatex, useHtml, false);
  if (useHtml)
    out << "\n<br>\nSimple basis epsilon form: " << tempS;
  tempS=this->SimpleBasisKEpsCoords.ElementToStringEpsilonForm(useLatex, useHtml, false);
  if (useHtml)
    out << "\n<br>\nSimple basis epsilon form with respect to k: " << tempS;
  this->theCentralizerDiagram.ElementToStrinG(tempS, true);
  if(!useLatex)
    CGI::clearDollarSigns(tempS, tempS);
  if (useLatex)
    out << "\n\n\\noindent ";
  if (useHtml)
    out << "<br>\n";
  if (useLatex)
    out << "$C(\\mathfrak{k_{ss}})_{ss}$: ";
  else
    out << "C(k_{ss})_{ss}: ";
  out << tempS;
  //int CartanPieceSize=
    //this->AmbientWeyl.CartanSymmetric.NumRows- this->SimpleBasisCentralizerRoots.size-
    //  this->SimpleBasisK.size;
  //if (CartanPieceSize!=0)
  //{  if (useLatex)
  //    out << "$\\oplus\\mathfrak{h}_" << CartanPieceSize<<"$";
  //  if (useHtml)
  //    out <<"+h_"<<CartanPieceSize;
  //}
  if (useHtml)
    out << "<br>\n simple basis centralizer: ";
  if (useLatex)
    out << "; simple basis centralizer: ";
  tempS=this->SimpleBasisCentralizerRoots.ToString();
  out << tempS;
  if (sl2s!=0)
  { if (useHtml)
      out << "\n<hr>\n";
    List<int> hCharacteristics_S_subalgebras;
    //this->ComputeIndicesSl2s(indexInOwner, *sl2s, hCharacteristics_S_subalgebras);
    hCharacteristics_S_subalgebras.size=0;
    out << "\nCharacteristics of sl(2) subalgebras that have no centralizer in k (total "
    << sl2s->IndicesSl2sContainedInRootSA[indexInOwner].size << "): ";
    for (int i=0; i<sl2s->IndicesSl2sContainedInRootSA[indexInOwner].size; i++)
    { int theSl2index=sl2s->IndicesSl2sContainedInRootSA[indexInOwner][i];
      slTwo& theSl2 = (*sl2s)[theSl2index];
      if (useHtml)
        out << "<a href=\"./sl2s/sl2s.html#sl2index" << theSl2index << "\">";
      out << theSl2.hCharacteristic.ToString() << ", ";
      if (useHtml)
        out << "</a>";
      bool isS_subalgebra=true;
//      theSl2.hCharacteristic.ComputeDebugString();
      for (int j=0; j<theSl2.IndicesContainingRootSAs.size; j++)
      { int indexComparison= theSl2.IndicesContainingRootSAs[j];
        if (indexComparison!=indexInOwner && sl2s->theRootSAs[indexComparison].indicesSubalgebrasContainingK.Contains(indexInOwner))
        { isS_subalgebra=false;
          break;
        }
      }
      if (isS_subalgebra)
        hCharacteristics_S_subalgebras.AddOnTop(sl2s->IndicesSl2sContainedInRootSA[indexInOwner][i]);
    }
    if (useHtml)
      out << "\n<br>\n";
    out << "\nS-sl(2) subalgebras in k (total " << hCharacteristics_S_subalgebras.size << "): ";
    for (int i=0; i<hCharacteristics_S_subalgebras.size; i++)
      out << sl2s->TheObjects[hCharacteristics_S_subalgebras[i]].hCharacteristic.ToString() << ", ";
  }
  if (useHtml)
  { out << "<hr>\n Number of k-submodules of g/k: " << this->HighestWeightsGmodK.size;
    out << "<br>Module decomposition over k follows. The decomposition is given in 1) epsilon coordinates w.r.t. g 2) simple coordinates w.r.t. g <br> ";
    std::stringstream //tempStream1,
    tempStream2, tempStream3;
    for(int i=0; i<this->HighestWeightsGmodK.size; i++)
    { //tempStream1 << "\\underbrace{V_{";
      tempStream2 << "\\underbrace{V_{";
      tempStream3 << "\\underbrace{V_{";
      //tempStream1
      //<< this->AmbientWeyl.GetFundamentalCoordinatesFromSimple
      //(this->HighestWeightsGmodK[i]).ElementToStringLetterFormat("\\omega", true, false);
      tempStream2
      << this->kModulesgEpsCoords[i][0].ToStringLetterFormat("\\epsilon");
      tempStream3
      << this->HighestWeightsGmodK[i].ToStringLetterFormat("\\alpha");
      //tempStream1 << "}}_{dim= " << this->kModules[i].size << "} ";
      tempStream2 << "}}_{dim= " << this->kModules[i].size << "} ";
      tempStream3 << "}}_{dim= " << this->kModules[i].size << "} ";
      if (i!=this->HighestWeightsGmodK.size-1)
      { //tempStream1 << "\\oplus";
        tempStream2 << "\\oplus";
        tempStream3 << "\\oplus";
      }
    }
//    out << "\n<hr>\n" << CGI::GetHtmlMathSpanFromLatexFormula(tempStream1.str()) << "\n";
    out << "\n<hr>\n" << CGI::GetHtmlMathSpanFromLatexFormula(tempStream2.str()) << "\n";
    out << "\n<hr>\n" << CGI::GetHtmlMathSpanFromLatexFormula(tempStream3.str()) << "\n<hr>\n";
  }
  if (useLatex)
    out << "\n\n\\noindent Number $\\mathfrak{g}/\\mathfrak{k}$ $\\mathfrak{k}$-submodules: ";
  if (!useHtml)
    out << this->LowestWeightsGmodK.size ;
  if (useHtml)
    out << "<br>\n";
  if (useLatex)
    out << "\n\n";
  out << latexHeader;
  this->kModulesgEpsCoords.SetSize(this->kModules.size);
  for (int i=0; i<this->kModules.size; i++)
  { tempS=this->LowestWeightsGmodK[i].ToString();
    if (useHtml)
      out << "\n<tr><td>";
    if (useLatex)
      out << "\\hline ";
    out << i;
    if (useHtml)
      out << "</td><td>";
    if (useLatex)
      out << " & ";
    out << this->kModules[i].size;
    if (useHtml)
      out << "</td><td>";
    if (useLatex)
      out << " & ";
    out << tempS;
    tempS=this->HighestWeightsGmodK[i].ToString();
    if (useHtml)
      out << "</td><td>";
    if (useLatex)
      out << " & ";
    out  << tempS;
    if (useHtml)
      out << "</td><td>";
    if (useLatex)
      out << " & \n";
    out << this->kModules[i].ToString();
    if (useHtml)
      out << "</td><td>";
    if (i>=this->kModulesgEpsCoords.size)
      this->GetAmbientWeyl().GetEpsilonCoords(this->kModules[i], this->kModulesgEpsCoords[i]);
    out << this->kModulesgEpsCoords[i].ElementToStringEpsilonForm(useLatex, useHtml, true);
    if (useLatex)
      out << " & \n";
    if (useHtml)
      out << "</td>";
    if (includeKEpsCoords)
    { if (useHtml)
        out << "<td>";
      if (useLatex)
        out << " & ";
      out << tempS << this->kModulesKepsCoords[i].ElementToStringEpsilonForm(useLatex, useHtml, true);
      if (useHtml)
        out << "</td>";
      if (useLatex)
        out << "\\\\\n";
    }
    if (useHtml)
      out << "</tr>";
    if (LatexLineCounter>this->NumGmodKtableRowsAllowedLatex)
    { LatexLineCounter=0;
      out << latexFooter << latexHeader;
    }
    if (i!=this->kModules.size-1)
    { LatexLineCounter+=this->kModules[i].size;
      if (useLatex)
       if ((LatexLineCounter>this->NumGmodKtableRowsAllowedLatex) && (LatexLineCounter!=this->kModules.TheObjects[i].size))
        { out << latexFooter << latexHeader;
          LatexLineCounter = this->kModules[i].size;
        }
    }
  }
  if (useHtml)
    out << "</table>";
  if (useLatex)
    out << latexFooter;
  if ((useLatex|| useHtml)&& this->theMultTable.size==0 && this->kModules.size!=0)
    this->GenerateKmodMultTable(this->theMultTable, this->theOppositeKmods, theGlobalVariables);
  if (this->theMultTable.size!=0)
  { if (useHtml)
      out << "\n\n Pairing table:\n\n";
    if (useLatex)
      out << "\n\n\\noindent Pairing table:\n\n\\noindent";
    this->theMultTable.ToString(tempS, useLatex, useHtml, *this);
    out << tempS << "\n";
  }
  output=out.str();
}

std::string CandidateSSSubalgebra::ToString(FormatExpressions* theFormat)const
{ DynkinType accumType;
  accumType.MakeZero();
  for (int i=0; i<theTypes.size; i++)
    accumType.AddMonomial(this->theTypes[i], 1);
  std::stringstream out;
  out << accumType;
  out << ". Hcandidates: ";
  for (int i=0; i<this->CartanSAsByComponent.size; i++)
    out << this->CartanSAsByComponent[i].ToString() << ", ";
  out << "Positive weight subsystem: " << this->theWeylSubgroup.RootsOfBorel.ToString();
  if (this->PosRootsPerpendicularPrecedingWeights.size>0)
    out << "Positive roots that commute with the weight subsystem: "
    << this->PosRootsPerpendicularPrecedingWeights.ToString();
  out << "Symmetric Cartan matrix scaled: "
  << this->theSymmetricCartanScaled.ToString(theFormat);
  out << "Character ambient Lie algebra: " << this->theCharFundamentalCoords.ToString();
  return out.str();
}
