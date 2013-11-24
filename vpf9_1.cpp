//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader2Math0_General.h"
#include <assert.h>

ProjectInformationInstance ProjectInfoVpf9_1cpp(__FILE__, "Math routines implementation. ");

Crasher crash;

void Crasher::FirstRun()
{ if (this->flagFirstRun)
    this->theCrashReport << "\n<hr>\n\n";
  this->flagFirstRun=false;
}

Crasher& Crasher::operator<<(const std::string& input)
{ this->FirstRun();
  this->theCrashReport << input;
  return *this;
}

Crasher& Crasher::operator<<(int x)
{ this->FirstRun();
  this->theCrashReport << x;
  return *this;
}

Crasher& Crasher::operator<<(const Crasher& dummyCrasherSignalsActualCrash)
{ this->FirstRun();
  this->theCrashReport << "<hr>This is a program crash";
  if (this->theGlobalVariables!=0)
    this->theCrashReport << " " << this->theGlobalVariables->GetElapsedSeconds() << " second(s) from the start. ";
  else
    this->theCrashReport << ". ";
  if (this->userInputStringIfAvailable!="")
    this->theCrashReport << " The user input that caused the crash was: <hr> " << this->userInputStringIfAvailable << "<hr>";
  this->theCrashReport << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  if (this->theGlobalVariables!=0)
    if (this->theGlobalVariables->theIndicatorVariables.ProgressReportStringS.size>0)
    { this->theCrashReport << "<hr>In addition, I have an account of the computation progress report strings, attached below.<hr>";
      for (int i=this->theGlobalVariables->theIndicatorVariables.ProgressReportStringS.size-1; i>=0; i--)
        this->theCrashReport << this->theGlobalVariables->theIndicatorVariables.ProgressReportStringS[i] << "<br>";
    }
  std::cout << this->theCrashReport.str();
  std::fstream theFile;
  std::string theFileName="../output/crashdump.txt";
  bool succeededToDump=true;
  if (!XML::OpenFileCreateIfNotPresent(theFile, theFileName, false, true, false))
  { theFileName="../output/crashdump2.txt";
    succeededToDump=XML::OpenFileCreateIfNotPresent(theFile, theFileName, false, true, false);
  }
  if (succeededToDump)
    std::cout << "<hr>Crash dumped in file " << theFileName;
  else
    std::cout << "<hr>Failed to dump crash: check if folder exists and the executable has file permissions for file " << theFileName << ".";
  std::cout.flush();
  theFile << this->theCrashReport.str();
  theFile.close();
  assert(false);
  return *this;
}

void DynkinDiagramRootSubalgebra::SwapDynkinStrings(int i, int j)
{ this->SimpleComponentTypes.SwapTwoIndices(i, j);
  this->SimpleBasesConnectedComponents.SwapTwoIndices(i, j);
  this->indicesThreeNodes.SwapTwoIndices(i, j);
  this->indicesEnds.SwapTwoIndices(i, j);
}

void DynkinDiagramRootSubalgebra::Sort()
{ //doing bubble sort
  for (int i=0; i<this->SimpleBasesConnectedComponents.size; i++)
    for (int j=i+1; j<this->SimpleBasesConnectedComponents.size; j++)
    { bool tempBool=false;
      if (this->SimpleBasesConnectedComponents[i].size<this->SimpleBasesConnectedComponents[j].size)
        tempBool=true;
      if (this->SimpleBasesConnectedComponents[i].size==this->SimpleBasesConnectedComponents[j].size)
        tempBool=((this->SimpleComponentTypes[i])<(this->SimpleComponentTypes[j]));
      if (tempBool)
        this->SwapDynkinStrings(i, j);
    }
  this->sameTypeComponents.size=0;
  this->indexInUniComponent.SetSize(this->SimpleBasesConnectedComponents.size);
  this->indexUniComponent.SetSize(this->SimpleBasesConnectedComponents.size);
  this->sameTypeComponents.ReservE(this->SimpleBasesConnectedComponents.size);
  DynkinSimpleType tempType;
  for (int i=0; i<this->SimpleBasesConnectedComponents.size; i++)
  { if (!(this->SimpleComponentTypes[i]==tempType))
    { this->sameTypeComponents.SetSize(this->sameTypeComponents.size+1);
      this->sameTypeComponents.LastObject()->size=0;
      tempType=this->SimpleComponentTypes[i];
    }
    this->sameTypeComponents.LastObject()->AddOnTop(i);
    this->indexUniComponent[i]=this->sameTypeComponents.size-1;
    this->indexInUniComponent[i]=this->sameTypeComponents.LastObject()->size-1;
  }
}

void DynkinDiagramRootSubalgebra::ComputeDiagramTypeKeepInput(const Vectors<Rational>& simpleBasisInput, const Matrix<Rational>& theBilinearForm)
{ MacroRegisterFunctionWithName("DynkinDiagramRootSubalgebra::ComputeDiagramTypeKeepInput");
  this->SimpleBasesConnectedComponents.size=0;
  this->SimpleBasesConnectedComponents.ReservE(simpleBasisInput.size);
  for (int i=0; i<simpleBasisInput.size; i++)
  { int indexFirstComponentConnectedToRoot=-1;
    for (int j=0; j<this->SimpleBasesConnectedComponents.size; j++)
      if (this->SimpleBasesConnectedComponents[j].ContainsARootNonPerpendicularTo(simpleBasisInput[i], theBilinearForm))
      { if (indexFirstComponentConnectedToRoot==-1)
        { indexFirstComponentConnectedToRoot=j;
          this->SimpleBasesConnectedComponents[j].AddOnTop(simpleBasisInput[i]);
        } else
        { this->SimpleBasesConnectedComponents[indexFirstComponentConnectedToRoot].AddListOnTop(this->SimpleBasesConnectedComponents[j]);
          this->SimpleBasesConnectedComponents.RemoveIndexSwapWithLast(j);
          j--;
        }
      }
    if (indexFirstComponentConnectedToRoot==-1)
    { this->SimpleBasesConnectedComponents.SetSize(this->SimpleBasesConnectedComponents.size+1);
      this->SimpleBasesConnectedComponents.LastObject()->size=0;
      this->SimpleBasesConnectedComponents.LastObject()->AddOnTop(simpleBasisInput[i]);
    }
  }
  this->ComputeDynkinStrings(theBilinearForm);
  this->Sort();
  this->ComputeDynkinStrings(theBilinearForm);
}

bool DynkinDiagramRootSubalgebra::LetterIsDynkinGreaterThanLetter(char letter1, char letter2)
{ if( (letter1=='B' || letter1=='D') && (letter2=='B' || letter2=='D') )
  { if (letter1==letter2)
      return false;
    if (letter1=='B')
      return true;
    else
      return false;
  }
  return letter1>letter2;
}

bool DynkinDiagramRootSubalgebra::IsGreaterThan(DynkinDiagramRootSubalgebra& right)
{ if (this->RankTotal()>right.RankTotal())
    return true;
  if (this->RankTotal()<right.RankTotal())
    return false;
  if(this->SimpleComponentTypes.size!=this->SimpleBasesConnectedComponents.size)
    crash << crash;
  for (int i=0; i<this->SimpleComponentTypes.size; i++)
  { if (this->SimpleBasesConnectedComponents[i].size>right.SimpleBasesConnectedComponents[i].size)
      return true;
     if (right.SimpleBasesConnectedComponents[i].size>this->SimpleBasesConnectedComponents[i].size)
      return false;
    if (this->SimpleComponentTypes[i]>right.SimpleComponentTypes[i])
      return true;
    if (right.SimpleComponentTypes[i]>this->SimpleComponentTypes[i])
      return false;
  }
  return false;
}

void DynkinDiagramRootSubalgebra::GetMapFromPermutation(Vectors<Rational>& domain, Vectors<Rational>& range, List<int>& thePerm, List<List<List<int > > >& theAutos, SelectionWithDifferentMaxMultiplicities& theAutosPerm, DynkinDiagramRootSubalgebra& right)
{ for (int i=0; i<this->SimpleBasesConnectedComponents.size; i++)
    for (int j=0; j<this->SimpleBasesConnectedComponents[i].size; j++)
    { if(this->SimpleBasesConnectedComponents[i].size!=right.SimpleBasesConnectedComponents[thePerm[i]].size)
        crash << crash;
      domain.AddOnTop( this->SimpleBasesConnectedComponents[i][j]);
      int indexTargetComponent=thePerm[i];
      int indexAutomorphismInComponent=theAutosPerm.Multiplicities[i];
      int indexRoot=theAutos[i][indexAutomorphismInComponent][j];
      range.AddOnTop(right.SimpleBasesConnectedComponents[indexTargetComponent][indexRoot]);
    }
}

void DynkinDiagramRootSubalgebra::ComputeDynkinStrings(const Matrix<Rational>& theBilinearForm)
{ MacroRegisterFunctionWithName("DynkinDiagramRootSubalgebra::ComputeDynkinStrings");
  this->indicesThreeNodes.SetSize(this->SimpleBasesConnectedComponents.size);
  this->SimpleComponentTypes.SetSize(this->SimpleBasesConnectedComponents.size);
  this->indicesEnds.SetSize(this->SimpleBasesConnectedComponents.size);
  for (int i=0; i<this->SimpleBasesConnectedComponents.size; i++)
    this->ComputeDynkinString(i, theBilinearForm);
}

bool DynkinDiagramRootSubalgebra::operator==(const DynkinDiagramRootSubalgebra& right) const
{ if (right.SimpleBasesConnectedComponents.size!= this->SimpleBasesConnectedComponents.size)
    return false;
  for (int i=0; i<this->SimpleBasesConnectedComponents.size; i++)
  { bool tempBool=
    ((this->SimpleBasesConnectedComponents[i].size ==right.SimpleBasesConnectedComponents[i].size) && (this->SimpleComponentTypes[i]==right.SimpleComponentTypes[i]));
    if (!tempBool)
      return false;
  }
  return true;
}

void DynkinDiagramRootSubalgebra::GetDynkinType(DynkinType& output)const
{ output.MakeZero();
  output.SetExpectedSize(this->SimpleComponentTypes.size);
  for (int i=0; i<this->SimpleComponentTypes.size; i++)
    output.AddMonomial(this->SimpleComponentTypes[i],1);
}

void DynkinDiagramRootSubalgebra::GetAutomorphism(List<List<int> >& output, int index)
{ Vectors<Rational>& currentComponent= this->SimpleBasesConnectedComponents[index];
  DynkinSimpleType& currentStrinG=this->SimpleComponentTypes[index];
  List<int> thePermutation;
  thePermutation.SetSize(currentComponent.size);
  output.size=0;
  for (int i=0; i<currentComponent.size; i++)
    thePermutation[i]=i;
  output.AddOnTop(thePermutation);
  if (currentStrinG.theLetter=='A' && currentComponent.size!=1)
  { thePermutation.ReverseOrderElements();
    output.AddOnTop(thePermutation);
  }
  if (currentStrinG.theLetter=='D')
  { if (currentComponent.size==4)
    {//the automorphism group of the Dynkin Diagram is S3
      thePermutation[1]=2; thePermutation[2]=3; thePermutation[3]=1;
      output.AddOnTop(thePermutation);
      thePermutation[1]=1; thePermutation[2]=3; thePermutation[3]=2;
      output.AddOnTop(thePermutation);
      thePermutation[1]=2; thePermutation[2]=1; thePermutation[3]=3;
      output.AddOnTop(thePermutation);
      thePermutation[1]=3; thePermutation[2]=1; thePermutation[3]=2;
      output.AddOnTop(thePermutation);
      thePermutation[1]=3; thePermutation[2]=2; thePermutation[3]=1;
      output.AddOnTop(thePermutation);
    } else
    { thePermutation[currentComponent.size-2]=currentComponent.size-1;
      thePermutation[currentComponent.size-1]=currentComponent.size-2;
      output.AddOnTop(thePermutation);
    }
  }
  if (currentStrinG.theLetter=='E' && currentStrinG.theRank==6)
  { thePermutation[1]=3; thePermutation[2]=4;
    thePermutation[3]=1; thePermutation[4]=2;
    output.AddOnTop(thePermutation);
  }
}

void DynkinDiagramRootSubalgebra::GetAutomorphisms(List<List<List<int> > >& output)
{ output.SetSize(this->SimpleBasesConnectedComponents.size);
  for (int i=0; i<this->SimpleBasesConnectedComponents.size; i++)
    this->GetAutomorphism(output[i], i);
}

int DynkinDiagramRootSubalgebra::RankTotal()
{ int result=0;
  for (int i=0; i<this->SimpleBasesConnectedComponents.size; i++)
    result+=this->SimpleBasesConnectedComponents[i].size;
  return result;
}

int DynkinDiagramRootSubalgebra::NumRootsGeneratedByDiagram()
{ int result=0;
  if(this->SimpleBasesConnectedComponents.size!=this->SimpleComponentTypes.size)
    crash << crash;
  for (int i=0; i<this->SimpleComponentTypes.size; i++)
  { int Rank=this->SimpleBasesConnectedComponents[i].size;
    if (this->SimpleComponentTypes[i].theLetter=='A')
      result+=Rank*(Rank+1);
    if (this->SimpleComponentTypes[i].theLetter=='B'|| this->SimpleComponentTypes[i].theLetter=='C')
      result+=Rank*Rank*2;
    if (this->SimpleComponentTypes[i].theLetter=='D')
      result+=Rank*(Rank-1)*2;
    if (this->SimpleComponentTypes[i].theLetter=='E')
    { if (Rank==6)
        result+=72;
      if (Rank==7)
        result+=126;
      if (Rank==8)
        result+=240;
    }
    if (this->SimpleComponentTypes[i].theLetter=='F')
      result+=48;
    if (this->SimpleComponentTypes[i].theLetter=='G')
      result+=12;
  }
  return result;
}

int DynkinDiagramRootSubalgebra::numberOfThreeValencyNodes(int indexComponent, const Matrix<Rational>& theBilinearForm)
{ Vectors<Rational>& currentComponent = this->SimpleBasesConnectedComponents[indexComponent];
  int numEnds=0; int result=0;
  this->indicesThreeNodes[indexComponent] =-1;
  this->indicesEnds[indexComponent].size=0;
  for (int i=0; i<currentComponent.size; i++)
  { int counter=0;
    for (int j=0; j<currentComponent.size; j++)
      if (currentComponent[i].ScalarProduct(currentComponent[j], theBilinearForm).IsNegative())
        counter++;
    if (counter>3)
      crash  << "This is a programming error: corrupt simple basis corresponding to Dynkin diagram: the Dynkin diagram should have nodes with"
      << " valency at most 3, but this diagram has node with valency " << counter << ". " << crash;
    if (counter==3)
    { result++;
      this->indicesThreeNodes[indexComponent]=i;
    }
    if (counter<=1)
    { numEnds++;
      this->indicesEnds[indexComponent].AddOnTop(i);
    }
  }
  if (result>1)
    crash << crash;
  if (result==1)
  { if(numEnds!=3)
      crash << crash;
  }
  else
  { if(numEnds>2)
      crash << crash;
  }
  return result;
}

void IndicatorWindowVariables::Assign(IndicatorWindowVariables& right)
{ this->ProgressReportStringS=right.ProgressReportStringS;
  this->flagRootIsModified= right.flagRootIsModified;
  this->modifiedRoot=right.modifiedRoot;
}

bool affineCone::SplitByAffineHyperplane(affineHyperplane<Rational>& theKillerPlane, affineCones& output)
{ return true;
}

bool affineCone::WallIsInternalInCone(affineHyperplane<Rational>& theKillerCandidate)
{ return true;
}

inline int affineCone::GetDimension()
{ if (this->theWalls.size==0)
    return 0;
  return this->theWalls.TheObjects[0].affinePoint.size;
}

inline unsigned int affineCone::HashFunction() const
{ int tempMin=MathRoutines::Minimum(this->theWalls.size, ::SomeRandomPrimesSize);
  unsigned int result=0;
  for (int i=0; i<tempMin; i++)
    result+= this->theWalls[i].HashFunction()*::SomeRandomPrimes[i];
  return result;
}

void affineHyperplanes::ToString(std::string& output)
{ std::stringstream out;
  for (int i=0; i<this->size; i++)
  { std::string tempS;
    this->TheObjects[i].ToString(tempS);
    out << "index: " << i << " " << tempS << "\n";
  }
  output= out.str();
}

void permutation::initPermutation(int n)
{ this->initIncomplete(n);
  for(int i=0; i<n; i++)
  { this->MaxMultiplicities[i]=n-i-1;
    this->Multiplicities[i]=0;
  }
}

void permutation::initPermutation(List<int>& disjointSubsets, int TotalNumElements)
{ this->initIncomplete(TotalNumElements);
  int counter=0;
  for(int i=0; i<disjointSubsets.size; i++)
  { for (int j=0; j<disjointSubsets[i]; j++)
    { this->MaxMultiplicities[counter]=disjointSubsets[i]-j-1;
      this->Multiplicities[counter]=0;
      counter++;
    }
    TotalNumElements-=disjointSubsets[i];
  }
  if(TotalNumElements!=0)
    crash << crash;
}

void permutation::incrementAndGetPermutation(List<int>& output)
{ this->IncrementSubset();
  this->GetPermutationLthElementIsTheImageofLthIndex(output);
}

void permutation::GetPermutationLthElementIsTheImageofLthIndex(List<int>& output)
{ int numElements=this->Multiplicities.size;
  output.SetSize(numElements);
  for (int i=0; i<numElements; i++)
    output[i]=i;
  for (int i=0; i<numElements; i++)
    MathRoutines::swap(output[i], output[i+this->Multiplicities[i]]);
}

bool WeylGroup::AreMaximallyDominant(List<Vector<Rational> >& theWeights, bool useOuterAutos)
{ MacroRegisterFunctionWithName("WeylGroup::AreMaximallyDominant");
  MemorySaving<Vectors<Rational> > theWeightsCopy;
  Vector<Rational> zeroWeight;
  if (useOuterAutos)
  { this->ComputeExternalAutos();
    zeroWeight.MakeZero(this->GetDim());
  }
  for (int i=0; i<theWeights.size; i++)
  { for (int j=0; j<this->RootsOfBorel.size; j++)
      if (this->RootScalarCartanRoot(this->RootsOfBorel[j], theWeights[i])<0)
      { bool reflectionDoesRaise=true;
        for (int k=0; k<i; k++)
          if (this->RootScalarCartanRoot(this->RootsOfBorel[j], theWeights[k])>0)
          { reflectionDoesRaise=false;
            break;
          }
        if (reflectionDoesRaise)
          return false;
      }
    if (!useOuterAutos)
      continue;
    for (int j=0; j<this->OuterAutomorphisms.size; j++)
    { theWeightsCopy.GetElement()=theWeights;
      this->OuterAutomorphisms[j].ActOnVectorsColumn(theWeightsCopy.GetElement());
      for (int k=0; k<i; k++)
        if (!(theWeightsCopy.GetElement()[k]-theWeights[k]).IsPositiveOrZero())
          continue;
      if (!(theWeightsCopy.GetElement()[i]-theWeights[i]).isGreaterThanLexicographic(zeroWeight))
        continue;
      return false;
    }
  }
  return true;
}

void SemisimpleLieAlgebra::ComputeChevalleyConstants(GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("SemisimpleLieAlgebra::ComputeChevalleyConstants");
  this->theWeyl.ComputeRho(true);
  this->ChevalleyConstants.init(this->theWeyl.RootSystem.size, this->theWeyl.RootSystem.size);
  this->Computed.init(this->theWeyl.RootSystem.size, this->theWeyl.RootSystem.size);
  this->Computed.NullifyAll(false);
  Selection nonExploredRoots;
  this->flagAnErrorHasOccurredTimeToPanic=false;
  Vectors<Rational>& posRoots=this->theWeyl.RootsOfBorel;
  nonExploredRoots.MakeFullSelection(posRoots.size);
  Vector<Rational> tempRoot;
  std::stringstream out;
  ProgressReport theReport(theGlobalVariables);
  double startTimer=-1;
  if (theGlobalVariables!=0)
  { out << "Initializing matrix for structure constant computation of " << this->GetLieAlgebraName() << "... ";
    startTimer= theGlobalVariables->GetElapsedSeconds();
    theReport.Report(out.str());
  }
  for (int i=0; i<this->theWeyl.RootSystem.size; i++)
    for(int j=i; j<this->theWeyl.RootSystem.size; j++)
    { tempRoot=this->theWeyl.RootSystem.TheObjects[i]+this->theWeyl.RootSystem.TheObjects[j];
      if(!tempRoot.IsEqualToZero())
        if(!this->theWeyl.IsARoot(tempRoot))
        { this->Computed.elements[i][j]=true;
          this->ChevalleyConstants.elements[i][j].MakeZero();
          this->Computed.elements[j][i]=true;
          this->ChevalleyConstants.elements[j][i].MakeZero();
        }
    }
  double startStructureConstantComputation=-1;
  if (theGlobalVariables!=0)
  { out << "done in " << theGlobalVariables->GetElapsedSeconds()- startTimer << " seconds.<br> " << "Computing structure constants...";
    theReport.Report(out.str());
    startStructureConstantComputation= theGlobalVariables->GetElapsedSeconds();
  }
  Rational tempRat;
  while (nonExploredRoots.CardinalitySelection>0)
  { //this->ComputeDebugString();
    //nonExploredRoots.ComputeDebugString();
    int theBorelIndex=nonExploredRoots.elements[0];
    Rational theHeight= posRoots.TheObjects[theBorelIndex].SumCoords();
    for (int i=1; i<nonExploredRoots.CardinalitySelection; i++)
    { tempRat= posRoots.TheObjects[nonExploredRoots.elements[i]].SumCoords();
      if (theHeight.IsGreaterThan(tempRat))
      { theHeight.Assign(tempRat);
        theBorelIndex=nonExploredRoots.elements[i];
      }
    }
    Vector<Rational>& theRoot= posRoots.TheObjects[theBorelIndex];
    int theIndex=this->theWeyl.RootSystem.GetIndex(theRoot);
    Vector<Rational> smallRoot2;
    int FirstIndexFirstPosChoice=-1;
//    int FirstIndexFirstNegChoice;
    int SecondIndexFirstPosChoice=-1;
//    int SecondIndexFirstNegChoice;
    Rational CurrentHeight;
    for (int i=0; i<this->theWeyl.RootsOfBorel.size; i++)
    { Vector<Rational>& smallRoot1=this->theWeyl.RootsOfBorel.TheObjects[i];
      CurrentHeight=smallRoot1.SumCoords();
      int FirstPosIndex= this->theWeyl.RootSystem.GetIndex(smallRoot1);
      int FirstNegIndex= this->theWeyl.RootSystem.GetIndex(-smallRoot1);
      if (theHeight.IsGreaterThan(CurrentHeight))
      { smallRoot2=theRoot-smallRoot1;
        int SecondPosIndex=this->theWeyl.RootSystem.GetIndex(smallRoot2);
        if (FirstPosIndex<SecondPosIndex)
          if (SecondPosIndex!=-1)
          { int SecondNegIndex=this->theWeyl.RootSystem.GetIndex(-smallRoot2);
            if (FirstIndexFirstPosChoice==-1)
            { FirstIndexFirstPosChoice=FirstPosIndex;
              SecondIndexFirstPosChoice=SecondPosIndex;
//              FirstIndexFirstNegChoice=FirstNegIndex;
//              SecondIndexFirstNegChoice= SecondNegIndex;
              int thePower;
              this->GetMaxQForWhichBetaMinusQAlphaIsARoot(smallRoot1, smallRoot2, thePower);
              this->ChevalleyConstants.elements[FirstNegIndex][SecondNegIndex]=-1-thePower;
              this->Computed.elements[FirstNegIndex][SecondNegIndex]=true;
            } else
              this->ComputeOneChevalleyConstant(FirstIndexFirstPosChoice, SecondIndexFirstPosChoice, FirstNegIndex, SecondNegIndex, theIndex);
            //this->ComputeDebugString();
            this->ExploitSymmetryAndCyclicityChevalleyConstants(FirstNegIndex, SecondNegIndex);
            //this->ComputeDebugString();
          }
      }
//      if (this->flagAnErrorHasOccurredTimeToPanic)
//        this->ComputeDebugString(false, false, theGlobalVariables);
    }
    nonExploredRoots.selected[theBorelIndex]=false;
    nonExploredRoots.ComputeIndicesFromSelection();
  }
  double startMultTable=-1;
  if (theGlobalVariables!=0)
  { out << "done in " << theGlobalVariables->GetElapsedSeconds() - startStructureConstantComputation
    << " seconds.<br> Computing Lie bracket pairing (``multiplication'') table...";
    theReport.Report(out.str());
    startMultTable=theGlobalVariables->GetElapsedSeconds();
  }
  this->ComputeMultTable(*theGlobalVariables);
  if (theGlobalVariables!=0)
  { out << " done in " << theGlobalVariables->GetElapsedSeconds()-startMultTable << " seconds. Total structure constant computation time: "
    << theGlobalVariables->GetElapsedSeconds()- startTimer << " seconds. ";
    theReport.Report(out.str());
  }
  if (this->GetNumPosRoots()<=0)
    crash << "This is a programming error: number of positive roots of a semisimple Lie algebra is reported to be zero. " << crash;
//  this->TestForConsistency(theGlobalVariables);
}

void SemisimpleLieAlgebra::ComputeMultTable(GlobalVariables& theGlobalVariables)
{ int numPosRoots=this->theWeyl.RootsOfBorel.size;
  int theRank= this->theWeyl.CartanSymmetric.NumRows;
  int numRoots = numPosRoots*2;
  int numGenerators=numRoots+theRank;
  Vector<Rational> tempRoot;
  this->theLiebrackets.init(numGenerators, numGenerators);
//  this->theLiebracketPairingIndices.init(numGenerators, numGenerators);
  this->UEGeneratorOrderIncludingCartanElts.initFillInObject(numGenerators, -1);
//  this->theLiebracketPairingIndices.NullifyAll(-1);
//  this->OppositeRootSpaces.initFillInObject(numRoots+theDimension, -1);
  Vector<Rational> leftWeight, rightWeight, hRoot;
  for (int i=0; i<numGenerators; i++)
  { leftWeight=this->GetWeightOfGenerator(i);
    for (int j=i; j<numGenerators; j++)
    { rightWeight=this->GetWeightOfGenerator(j);
      if (leftWeight.IsEqualToZero() && rightWeight.IsEqualToZero())
      { this->theLiebrackets.elements[i][j].MakeZero();
        continue;
      }
      if (leftWeight.IsEqualToZero() && !rightWeight.IsEqualToZero())
      { this->theLiebrackets.elements[i][j].MakeGenerator(j, *this);
        hRoot.MakeEi(theRank, i-numPosRoots);
        this->theLiebrackets.elements[i][j]*=Vector<Rational>::ScalarProduct(hRoot, rightWeight, this->theWeyl.CartanSymmetric);
        continue;
      }
      if (!leftWeight.IsEqualToZero() && rightWeight.IsEqualToZero())
      { this->theLiebrackets.elements[i][j].MakeGenerator(i, *this);
        hRoot.MakeEi(theRank, j-numPosRoots);
        this->theLiebrackets.elements[i][j]*=-Vector<Rational>::ScalarProduct(hRoot, leftWeight, this->theWeyl.CartanSymmetric);
        continue;
      }
      if (!leftWeight.IsEqualToZero() && !rightWeight.IsEqualToZero())
      { int newIndex=this->GetGeneratorFromRoot(leftWeight+rightWeight);
        if (newIndex!=-1)
        { this->theLiebrackets.elements[i][j].MakeGenerator(newIndex, *this);
          int leftIndex=this->GetRootIndexFromGenerator(i);
          int rightIndex=this->GetRootIndexFromGenerator(j);
          this->theLiebrackets.elements[i][j]*=this->ChevalleyConstants.elements[leftIndex][rightIndex];
        } else
        { if (!(leftWeight+rightWeight).IsEqualToZero())
            this->theLiebrackets.elements[i][j].MakeZero();
          else
          { ElementSemisimpleLieAlgebra<Rational>& current=this->theLiebrackets.elements[i][j];
            current.MakeHgenerator(leftWeight*2/(this->theWeyl.RootScalarCartanRoot(leftWeight, leftWeight)), *this);
          }
        }
        continue;
      }
    }
  }
  for (int i=0; i<numGenerators; i++)
    for (int j=i+1; j<numGenerators; j++)
    { this->theLiebrackets.elements[j][i]=this->theLiebrackets.elements[i][j];
      this->theLiebrackets.elements[j][i]*=-1;
    }
  for (int i=0; i<numGenerators; i++)
    this->UEGeneratorOrderIncludingCartanElts[i]=i;
}

void SemisimpleLieAlgebra::ExploitSymmetryAndCyclicityChevalleyConstants(int indexI, int indexJ)
{ Vector<Rational>& rootI= this->theWeyl.RootSystem.TheObjects[indexI];
  Vector<Rational>& rootJ= this->theWeyl.RootSystem.TheObjects[indexJ];
  if((rootI+rootJ).IsEqualToZero())
    crash << crash;
  //int indexMinusI= this->theWeyl.RootSystem.GetIndex(-rootI);
  //int indexMinusJ= this->theWeyl.RootSystem.GetIndex(-rootJ);
  //this->ComputeDebugString();
  this->ExploitSymmetryChevalleyConstants(indexI, indexJ);
  //this->ComputeDebugString();
  //int indexRootIPlusRootJ=this->theWeyl.RootSystem.GetIndex(rootI+rootJ);
  int indexMinusIMinusJ=this->theWeyl.RootSystem.GetIndex(-rootI-rootJ);
  this->ExploitTheCyclicTrick(indexI, indexJ, indexMinusIMinusJ);
  //this->ComputeDebugString();
  //this->ExploitTheCyclicTrick(indexMinusI, indexMinusJ, indexRootIPlusRootJ);
  //this->ComputeDebugString();
}

void SemisimpleLieAlgebra::ExploitSymmetryChevalleyConstants(int indexI, int indexJ)
{ Vector<Rational>& rootI= this->theWeyl.RootSystem.TheObjects[indexI];
  Vector<Rational>& rootJ= this->theWeyl.RootSystem.TheObjects[indexJ];
  if(!this->Computed.elements[indexI][indexJ])
    crash << crash;
  int indexMinusI = this->theWeyl.RootSystem.GetIndex(-rootI);
  int indexMinusJ = this->theWeyl.RootSystem.GetIndex(-rootJ);
  this->ChevalleyConstants.elements[indexJ][indexI].Assign(this->ChevalleyConstants.elements[indexI][indexJ]*(-1));
  //this->ComputeDebugString();
  this->Computed.elements[indexJ][indexI]=true;
  if((rootI+rootJ).IsEqualToZero())
    crash << crash;
  int thePower;
  this->GetMaxQForWhichBetaMinusQAlphaIsARoot
  (this->theWeyl.RootSystem.TheObjects[indexMinusI], this->theWeyl.RootSystem.TheObjects[indexMinusJ], thePower);
  int i=1+thePower;
  this->ChevalleyConstants.elements[indexMinusI][indexMinusJ].AssignInteger(-i*i);
  this->ChevalleyConstants.elements[indexMinusI][indexMinusJ].DivideBy(this->ChevalleyConstants.elements[indexI][indexJ]);
  this->Computed.elements[indexMinusI][indexMinusJ]=true;
  //this->ComputeDebugString();
  this->ChevalleyConstants.elements[indexMinusJ][indexMinusI].Assign(this->ChevalleyConstants.elements[indexMinusI][indexMinusJ]*(-1));
  this->Computed.elements[indexMinusJ][indexMinusI]=true;
  //this->ComputeDebugString();
}

void SemisimpleLieAlgebra::ExploitTheCyclicTrick(int i, int j, int k)
{ Vector<Rational>& rootI= this->theWeyl.RootSystem.TheObjects[i];
  Vector<Rational>& rootK= this->theWeyl.RootSystem.TheObjects[k];
  Vector<Rational>& rootJ= this->theWeyl.RootSystem.TheObjects[j];
  //the following three checks can be commented out to increase speed. They have never failed so far.
  if(!(rootI+rootK+rootJ).IsEqualToZero())
    crash << crash;
  if((rootI+rootJ).IsEqualToZero() || (rootK+rootK).IsEqualToZero() || (rootJ+rootI).IsEqualToZero())
    crash << crash;
  if(!this->Computed.elements[i][j])
    crash << crash;
  /////////////////////////////////////////////////////////////////
  Rational& tempRat= this->ChevalleyConstants.elements[i][j];
  Rational tempRat2= this->theWeyl.RootScalarCartanRoot(rootK, rootK);
  this->ChevalleyConstants.elements[j][k]=(tempRat*this->theWeyl.RootScalarCartanRoot(rootI, rootI))/tempRat2;
  this->Computed.elements[j][k]=true;
  this->ChevalleyConstants.elements[k][i] = (tempRat*this->theWeyl.RootScalarCartanRoot(rootJ, rootJ))/tempRat2;
  this->Computed.elements[k][i]=true;
  this->ExploitSymmetryChevalleyConstants(j, k);
  this->ExploitSymmetryChevalleyConstants(k, i);
}

bool SemisimpleLieAlgebra::GetMaxQForWhichBetaMinusQAlphaIsARoot
(const Vector<Rational>& alpha, const Vector<Rational>& beta, int& output)const
{ output=-1;
  Vector<Rational> tempRoot=beta;
  if (alpha.IsEqualToZero())
    crash << "This is a programming error: calling function GetMaxQForWhichBetaMinusQAlphaIsARoot with zero value for alpha is not allowed. " << crash;
  bool foundRoot=false;
  while (this->theWeyl.IsARoot(tempRoot))
  { output++;
    tempRoot-=(alpha);
    foundRoot=true;
  }
  return foundRoot;
}

void SemisimpleLieAlgebra::ComputeOneChevalleyConstant
(int indexGamma, int indexDelta, int indexMinusEpsilon, int indexMinusZeta, int indexEta)
{//using formula (**), 2.9, page 49, Samelson, Notes on Lie algebras, 1989
  Vector<Rational>& gamma= this->theWeyl.RootSystem.TheObjects[indexGamma];
  Vector<Rational>& delta= this->theWeyl.RootSystem.TheObjects[indexDelta];
  Vector<Rational>& minusEpsilon= this->theWeyl.RootSystem.TheObjects[indexMinusEpsilon];
  Vector<Rational>& eta= this->theWeyl.RootSystem.TheObjects[indexEta];
  Vector<Rational>& minusZeta=this->theWeyl.RootSystem.TheObjects[indexMinusZeta];
  if(eta!=gamma+delta)
    crash << crash;
  if(!this->theWeyl.IsARoot(eta+minusEpsilon))
    crash << crash;
  if(!this->Computed.elements[indexDelta][indexMinusEpsilon] || !this->Computed.elements[indexMinusEpsilon][indexGamma] || !this->Computed.elements[indexGamma][indexDelta] )
   crash << crash;
  if(this->ChevalleyConstants.elements[indexGamma][indexDelta].IsEqualToZero())
    crash << crash;
  int indexDeltaMinusEpsilon= this->theWeyl.RootSystem.GetIndex(delta+minusEpsilon);
  int indexGammaMinusEpsilon= this->theWeyl.RootSystem.GetIndex(gamma+minusEpsilon);
  Rational FirstSummand, SecondSummand;
  if (indexDeltaMinusEpsilon!=-1)
  { if(!this->Computed.elements[indexGamma][indexDeltaMinusEpsilon] || !this->Computed.elements[indexDelta][indexMinusEpsilon])
      crash << crash;
    FirstSummand=this->ChevalleyConstants.elements[indexGamma][indexDeltaMinusEpsilon]*this->ChevalleyConstants.elements[indexDelta][indexMinusEpsilon];
  } else
    FirstSummand.MakeZero();
  if (indexGammaMinusEpsilon!=-1)
  { if(!this->Computed.elements[indexDelta][indexGammaMinusEpsilon] || !this->Computed.elements[indexMinusEpsilon][indexGamma])
      crash << crash;
    SecondSummand =this->ChevalleyConstants.elements[indexDelta][indexGammaMinusEpsilon]*this->ChevalleyConstants.elements[indexMinusEpsilon][indexGamma];
  } else
    SecondSummand.MakeZero();
  this->ChevalleyConstants.elements[indexMinusEpsilon][indexMinusZeta]=(this->theWeyl.RootScalarCartanRoot(eta, eta)/ this->theWeyl.RootScalarCartanRoot(minusZeta, minusZeta))*( FirstSummand+SecondSummand)/ this->ChevalleyConstants.elements[indexGamma][indexDelta];
  this->Computed.elements[indexMinusEpsilon][indexMinusZeta]=true;
}

bool SemisimpleLieAlgebra::TestForConsistency(GlobalVariables& theGlobalVariables)
{ //HashedList<Vector<Rational> >& theRoots=this->theWeyl.RootSystem;
  FormatExpressions& theFormat=theGlobalVariables.theDefaultFormat;
  ElementSemisimpleLieAlgebra<Rational> g1, g2, g3, g23, g31, g12, g123, g231, g312, temp;
  //this->ComputeDebugString(false, false, theGlobalVariables);
  for (int i=0; i<this->GetNumGenerators(); i++)
  { g1.MakeGenerator(i, *this);
    for (int j=0; j<this->GetNumGenerators(); j++)
    { g2.MakeGenerator(j, *this);
      for (int k=0; k<this->GetNumGenerators(); k++)
      { g3.MakeGenerator(k, *this);
        this->LieBracket(g2, g3, g23); this->LieBracket(g1, g23, g123);
        this->LieBracket(g3, g1, g31); this->LieBracket(g2, g31, g231);
        this->LieBracket(g1, g2, g12); this->LieBracket(g3, g12, g312);
        temp=g123;
        temp+=g231;
        temp+=g312;
        if (!temp.IsEqualToZero())
        { crash << "This is a programming error. The computed structure constants are wrong: the Jacobi identity fails. More precisely, I get that "
          << "<br>[" << g1.ToString(&theFormat) << ", " << g2.ToString(&theFormat) << "]=" << g12.ToString(&theFormat)
          << "<br>[" << g2.ToString(&theFormat) << ", " << g3.ToString(&theFormat) << "]=" << g23.ToString(&theFormat)
          << "<br>[" << g3.ToString(&theFormat) << ", " << g1.ToString(&theFormat) << "]=" << g31.ToString(&theFormat)
          << "<br>g123= " << g123.ToString(&theFormat) << "<br>g231=" << g231.ToString(&theFormat) << "<br>g312=" << g312.ToString(&theFormat) << "<br>"
          << crash;
          return false;
        }
        this->MakeChevalleyTestReport(i, j, k, this->GetNumGenerators(), theGlobalVariables);
      }
    }
  }
  return true;
}

Rational SemisimpleLieAlgebra::GetConstant(const Vector<Rational> &root1, const  Vector<Rational> &root2)
{ int index1=this->theWeyl.RootSystem.GetIndex(root1);
  int index2= this->theWeyl.RootSystem.GetIndex(root2);
  Rational tempRat;
  if (index1==-1 || index2==-1)
  { tempRat.MakeZero();
    return tempRat;
  }
  tempRat.Assign(this->ChevalleyConstants.elements[index1][index2]);
  return tempRat;
}

bool SemisimpleLieAlgebra::GetConstantOrHElement
(const Vector<Rational>& root1, const Vector<Rational>& root2, Rational& outputRat, Vector<Rational>& outputH)
{ if (!(root1+root2).IsEqualToZero())
  { outputRat=this->GetConstant(root1, root2);
    return true;
  }
  if (this->theWeyl.RootSystem.GetIndex(root1)==-1 || this->theWeyl.RootSystem.GetIndex(root2)==-1)
  { outputRat.MakeZero();
    return true;
  }
  outputH=(root1*2)/Vector<Rational>::ScalarProduct(root1, root1, this->theWeyl.CartanSymmetric);
  return false;
}

void SemisimpleLieAlgebra::MakeChevalleyTestReport(int i, int j, int k, int Total, GlobalVariables& theGlobalVariables)
{ if (theGlobalVariables.GetFeedDataToIndicatorWindowDefault()==0)
    return;
  std::stringstream out2, out3;
  int x=(i*Total*Total+j*Total+k+1);
  out2 << "i: " << i+1 << " of " << Total << " j: " << j+1 << " of " << Total << " k: " << k+1 << " of " << Total;
  out3 << "Total progress: " << x << " out of " << (Total*Total*Total);
  ProgressReport theReport(&theGlobalVariables);
  theReport.Report(out2.str()+out3.str());
}

void WeylGroup::GenerateRootSubsystem(Vectors<Rational>& theRoots)
{ Vector<Rational> tempRoot;
  int oldsize=theRoots.size;
  for (int i=0; i<oldsize; i++)
    theRoots.AddOnTopNoRepetition(-theRoots[i]);
  for (int i =0; i<theRoots.size; i++)
    for (int j=0; j<theRoots.size; j++)
    { tempRoot= theRoots[i]+theRoots[j];
      if (this->IsARoot(tempRoot))
        theRoots.AddOnTopNoRepetition(tempRoot);
    }
}
