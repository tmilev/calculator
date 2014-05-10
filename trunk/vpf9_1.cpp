//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader2Math0_General.h"
#include "vpfHeader2Math1_LieTheoryExtras.h"
#include <assert.h>

ProjectInformationInstance ProjectInfoVpf9_1cpp(__FILE__, "Math routines implementation. ");

Crasher crash;
StdoutClass stOutput;

Crasher::Crasher()
{ this->theGlobalVariables=0;
  this->flagFirstRun=true;
  this->CleanUpFunction=0;
}

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
  this->theCrashReport << Crasher::GetStackTraceEtcErrorMessage();
  if (this->theGlobalVariables!=0)
    if (this->theGlobalVariables->ProgressReportStringS.size>0)
    { this->theCrashReport << "<hr>In addition, I have an account of the computation progress report strings, attached below.<hr>";
      for (int i=this->theGlobalVariables->ProgressReportStringS.size-1; i>=0; i--)
        this->theCrashReport << this->theGlobalVariables->ProgressReportStringS[i] << "<br>";
    }
  stOutput << this->theCrashReport.str();
  std::fstream theFile;
  std::string theFileName="../output/crashdump.txt";
  bool succeededToDump=true;
  if (!FileOperations::OpenFileCreateIfNotPresent(theFile, theFileName, false, true, false))
  { theFileName="../output/crashdump2.txt";
    succeededToDump=FileOperations::OpenFileCreateIfNotPresent(theFile, theFileName, false, true, false);
  }
  if (stOutput.theOutputFunction!=0)
    std::cout << this->theCrashReport.str();
  if (succeededToDump)
    stOutput << "<hr>Crash dumped in file " << theFileName;
  else
    stOutput << "<hr>Failed to dump crash: check if folder exists and the executable has file permissions for file " << theFileName << ".";
  theFile << this->theCrashReport.str();
  theFile.close();
  stOutput.Flush();
  if (this->CleanUpFunction!=0)
    this->CleanUpFunction();
  assert(false);
  return *this;
}

std::string Crasher::GetStackTraceEtcErrorMessage()
{ std::stringstream out;
  out << "A partial stack trace follows (function calls not explicitly logged not included).";
  out << "<table><tr><td>file</td><td>line</td><td>function name (if known)</td></tr>";
  out << ProjectInformation::GetMainProjectInfo().GetStackTraceReport();
  out << "</table>";
  return out.str();
}

std::string GlobalVariables::ToStringFolderInfo()const
{ std::stringstream out;
  out << "<br>Physical path server base: " << this->PhysicalPathServerBase;
  out << "<br>Display name calculator with path: " << this->DisplayNameCalculatorWithPath;
  out << "<br>Physical name folder below executable: " << this->PhysicalNameFolderBelowExecutable;
  out << "<br>Display path server base: " << this->DisplayPathServerBase;
  out << "<br>Display name calculator with path: " << this->DisplayNameExecutableWithPath;
  out << "<br>Physical path output folder: " << this->PhysicalPathOutputFolder;
  out << "<br>Display path output folder: " << this->DisplayPathOutputFolder;
  return out.str();
}

void GlobalVariables::initDefaultFolderAndFileNames
(const std::string& inputPhysicalExecutableWithPathServerBaseIsFolderBelow,
 const std::string& scrambledIP)
{ this->PhysicalNameFolderBelowExecutable="";
  this->PhysicalNameExecutableNoPath="";
  this->PhysicalPathServerBase="";
  this->DisplayPathServerBase="";
  for (unsigned i=0; i<inputPhysicalExecutableWithPathServerBaseIsFolderBelow.size(); i++)
  { this->PhysicalNameExecutableNoPath.push_back(inputPhysicalExecutableWithPathServerBaseIsFolderBelow[i]);
    if (inputPhysicalExecutableWithPathServerBaseIsFolderBelow[i]=='/')
    { this->PhysicalPathServerBase+=this->PhysicalNameFolderBelowExecutable;
      this->DisplayPathServerBase=this->PhysicalNameFolderBelowExecutable;
      this->PhysicalNameFolderBelowExecutable=this->PhysicalNameExecutableNoPath;
      this->PhysicalNameExecutableNoPath="";
    }
  }
  this->PhysicalNameExecutableWithPath=this->PhysicalNameFolderBelowExecutable+this->PhysicalNameExecutableNoPath;
  if (this->PhysicalPathServerBase=="")
  { this->PhysicalPathServerBase="./../";
    this->DisplayPathServerBase="trunk/";
  }
  this->DisplayPathServerBase="/"+this->DisplayPathServerBase;
  this->PhysicalPathOutputFolder = this->PhysicalPathServerBase + "output/";
  this->DisplayPathOutputFolder = this->DisplayPathServerBase + "output/";

  this->defaultUserLabel=scrambledIP;
  this->PhysicalNameDefaultOutputNoPatH="default" + this->defaultUserLabel + "output";
  this->PhysicalNameDefaultOutputWithPath = this->PhysicalPathOutputFolder + this->PhysicalNameDefaultOutputNoPatH ;
  this->DisplayNameDefaultOutputNoPath = "default" + this->defaultUserLabel + "output";
  this->DisplayNameDefaultOutputWithPath = this->DisplayPathOutputFolder + this->DisplayNameDefaultOutputNoPath;

  this->PhysicalNameIndicatorWithPath = this->PhysicalPathOutputFolder + "indicator" + this->defaultUserLabel + ".html" ;
  this->DisplayNameIndicatorWithPath = this->DisplayPathOutputFolder + "indicator" + this->defaultUserLabel + ".html" ;
  this->DisplayNameCalculatorWithPath =
  this->DisplayPathServerBase +"cgi-bin/calculator";

}


template<>
typename List<Weight<RationalFunctionOld> >::OrderLeftGreaterThanRight
FormatExpressions::GetMonOrder<Weight<RationalFunctionOld> >()
{ return 0;
}

template<>
typename List<Weight<Rational> >::OrderLeftGreaterThanRight
FormatExpressions::GetMonOrder<Weight<Rational> >()
{ return 0;
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

void DynkinDiagramRootSubalgebra::ComputeDynkinString(int indexComponent, const Matrix<Rational>& theBilinearForm)
{ MacroRegisterFunctionWithName("DynkinDiagramRootSubalgebra::ComputeDynkinString");
  if(indexComponent>=this->SimpleBasesConnectedComponents.size)
    crash << crash;
  DynkinSimpleType& outputType=this->SimpleComponentTypes[indexComponent];
  Vectors<Rational>& currentComponent= this->SimpleBasesConnectedComponents[indexComponent];
  //stOutput << "<hr><hr>Computing dynkin string from " << currentComponent.ToString();
  List<int>& currentEnds=this->indicesEnds[indexComponent];
  if (currentComponent.size<1)
    crash << "This is a programming error: currentComponent is empty which is impossible. " << crash;
//  stOutput << "<hr> Extracting component from " << currentComponent.ToString() << " with bilinear form "
//  << theBilinearForm.ToString();
  if (this->numberOfThreeValencyNodes(indexComponent, theBilinearForm)==1)
  { //type D or E
    //in type D first comes the triple node, then the long string, then the one-root strings
    //the long string is oriented with the end that is connected to the triple node having
    //smaller index
    //in type E similarly the longest string comes first oriented with the root that is
    //linked to the triple node having smaller index
    // then comes the second longest string (oriented in the same fashion)
    // and last the one-root string
    Vector<Rational> tripleNode;
    tripleNode=currentComponent[this->indicesThreeNodes[indexComponent]];
    Vectors<Rational> rootsWithoutTripleNode=currentComponent;
    rootsWithoutTripleNode.RemoveIndexSwapWithLast(this->indicesThreeNodes[indexComponent]);
    DynkinDiagramRootSubalgebra diagramWithoutTripleNode;
//    stOutput << "<br>Computing helper Dynkin diagram from: " << rootsWithoutTripleNode.ToString();
    diagramWithoutTripleNode.ComputeDiagramInputIsSimple(rootsWithoutTripleNode, theBilinearForm);
//    stOutput << " ... to get: " << diagramWithoutTripleNode.ToStringRelativeToAmbientType(DynkinSimpleType('A',1));
    if (diagramWithoutTripleNode.SimpleBasesConnectedComponents.size!=3)
      crash << "This is a programming error: Dynkin diagram has a triple node whose removal does not yield 3 connected components. " << crash;
    for (int i=0; i<3; i++)
      if (diagramWithoutTripleNode.SimpleBasesConnectedComponents[i][0].ScalarProduct(tripleNode, theBilinearForm)==0)
        diagramWithoutTripleNode.SimpleBasesConnectedComponents[i].ReverseOrderElements();
    for(int i=0; i<3; i++)
      for(int j=i+1; j<3; j++)
        if (diagramWithoutTripleNode.SimpleBasesConnectedComponents[i].size<diagramWithoutTripleNode.SimpleBasesConnectedComponents[j].size)
          diagramWithoutTripleNode.SwapDynkinStrings(i,j);
    Rational theScale=2/tripleNode.ScalarProduct(tripleNode, theBilinearForm);
    currentComponent.SetSize(0);
    if (diagramWithoutTripleNode.SimpleBasesConnectedComponents[1].size==1)
    { //<- components are sorted by length, therefore the second and third component are of length 1,
      //therefore we have type D_n
      currentComponent.AddListOnTop(diagramWithoutTripleNode.SimpleBasesConnectedComponents[0]);//<-first long component
      if (!tripleNode.ScalarProduct(currentComponent[0], theBilinearForm).IsEqualToZero())
        currentComponent.ReverseOrderElements();
      currentComponent.AddOnTop(tripleNode);//<-then triple node
      currentComponent.AddListOnTop(diagramWithoutTripleNode.SimpleBasesConnectedComponents[1]);//<-last two vectors
      currentComponent.AddListOnTop(diagramWithoutTripleNode.SimpleBasesConnectedComponents[2]);//<-last two vectors
      outputType.MakeArbitrary('D', currentComponent.size, theScale);
    } else
    { //the second largest component has more than one element, hence we are in type E_n.
      if (diagramWithoutTripleNode.SimpleBasesConnectedComponents[1].size!=2)
        crash << "This is a programming error: the dynkin diagram has two components of length larger than 2 linked to the triple node."
        << crash;
      if (!tripleNode.ScalarProduct(diagramWithoutTripleNode.SimpleBasesConnectedComponents[1][0], theBilinearForm).IsEqualToZero())
        diagramWithoutTripleNode.SimpleBasesConnectedComponents[1].ReverseOrderElements(); //<-the 2-root component has the first root perpendicular to the triple node
      if (tripleNode.ScalarProduct(diagramWithoutTripleNode.SimpleBasesConnectedComponents[0][0], theBilinearForm).IsEqualToZero())
        diagramWithoutTripleNode.SimpleBasesConnectedComponents[0].ReverseOrderElements(); //<-the largest component has the first root non-perpendicular to the triple node
      currentComponent.AddOnTop(diagramWithoutTripleNode.SimpleBasesConnectedComponents[1][0]); //<-first root from 2-element component
      currentComponent.AddOnTop(diagramWithoutTripleNode.SimpleBasesConnectedComponents[2][0]); //<-then the small sticky part of the Dynkin diagram
      currentComponent.AddOnTop(diagramWithoutTripleNode.SimpleBasesConnectedComponents[1][1]); //<-next the second root from 2-element component
      currentComponent.AddOnTop(tripleNode); //<- next the triple node
      currentComponent.AddListOnTop(diagramWithoutTripleNode.SimpleBasesConnectedComponents[0]); //<-finally the longest component. Conventions, conventions...
      outputType.MakeArbitrary('E', currentComponent.size, theScale);
    }
   return;
  }
  Rational length1, length2;
  length1=currentComponent[0].ScalarProduct(currentComponent[0], theBilinearForm);
  int numLength1=1;
  int numLength2=0;
  for(int i=1; i<currentComponent.size; i++)
    if (currentComponent[i].ScalarProduct(currentComponent[i], theBilinearForm)==length1)
      numLength1++;
    else
    { numLength2++;
      length2=currentComponent[i].ScalarProduct(currentComponent[i], theBilinearForm);
    }
  if (numLength2==0)
  { //type A
    outputType.MakeArbitrary('A', numLength1, 2/length1);
  } else
  { if (length1<length2)
    { MathRoutines::swap(length1, length2);
      MathRoutines::swap(numLength1, numLength2);
      currentEnds.SwapTwoIndices(0,1);
    }//<-so far we made sure the first length is long
    if (numLength1==numLength2)
    {//B2, C2, F4 or G2
      if (numLength1==2)
        outputType.MakeArbitrary('F', 4, 2/length1);
      else if (length1/length2==3)
        outputType.MakeArbitrary('G', 2, 2/length2);
      else
        outputType.MakeArbitrary('B', 2, 2/length1);
    } else
    { if (numLength1>numLength2)
        outputType.MakeArbitrary('B', currentComponent.size, 2/length1);
      else
        outputType.MakeArbitrary('C', currentComponent.size, 2/length2);
    }
  }
//  stOutput << "<hr><hr>before swapping:" << currentComponent.ToString();
  currentComponent.SwapTwoIndices(0, currentEnds[0]);
  for (int i=0; i<currentComponent.size; i++)
    for (int j=i+1; j<currentComponent.size; j++)
      if (!currentComponent[i].ScalarProduct(currentComponent[j], theBilinearForm).IsEqualToZero())
      { currentComponent.SwapTwoIndices(i+1, j);
        break;
      }
//  stOutput << "<br>after swapping:" << currentComponent.ToString();
  //so far we made sure the entire component is one properly ordered string, starting with the long root.
  if (outputType.theLetter=='G' || outputType.theLetter=='C' )
    currentComponent.ReverseOrderElements();//<-in G_2 and C_n the short root comes first so we need to reverse elements.
}

std::string DynkinDiagramRootSubalgebra::ToStringRelativeToAmbientType(const DynkinSimpleType& ambientType)const
{ std::stringstream out;
  DynkinType theType;
  theType.MakeZero();
  for (int j=0; j<this->SimpleComponentTypes.size; j++)
    theType.AddMonomial(this->SimpleComponentTypes[j], 1);
  return theType.ToStringRelativeToAmbientType(ambientType);
}

void DynkinDiagramRootSubalgebra::ComputeDiagramInputIsSimple(const Vectors<Rational>& simpleBasisInput, const Matrix<Rational>& theBilinearForm)
{ MacroRegisterFunctionWithName("DynkinDiagramRootSubalgebra::ComputeDiagramInputIsSimple");
  //stOutput << "<br>Computing diagram from " << simpleBasisInput.ToString();
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
//  stOutput << "<br>before the horrible sort, the roots be: " << this->SimpleBasesConnectedComponents.ToString();
  this->Sort();
//  stOutput << "<br>'n after  the horrible sort, the roots be: " << this->SimpleBasesConnectedComponents.ToString();

//  stOutput << "...after the horrible sort, computing again...";
  this->ComputeDynkinStrings(theBilinearForm);
  DynkinType tempType;
  this->GetDynkinType(tempType);
  if (tempType.IsEqualToZero() && simpleBasisInput.size!=0)
    crash << "Dynkin type of zero but the roots generating the type are: " << simpleBasisInput.ToString() << crash;

//  stOutput << "...the very final complete absolute computation is done!";
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

Rational DynkinDiagramRootSubalgebra::GetSizeCorrespondingWeylGroupByFormula()
{ Rational output=1;
  for (int i=0; i<this->SimpleBasesConnectedComponents.size; i++)
    output*=WeylGroup::GetGroupSizeByFormula(this->SimpleComponentTypes[i].theLetter, this->SimpleComponentTypes[i].theRank);
  return output;
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

void DynkinDiagramRootSubalgebra::ComputeDiagramTypeModifyInput(Vectors<Rational>& inputRoots, WeylGroup& theWeyl)
{ MacroRegisterFunctionWithName("DynkinDiagramRootSubalgebra::ComputeDiagramTypeModifyInput");
  theWeyl.TransformToSimpleBasisGenerators(inputRoots, theWeyl.RootSystem);
  this->ComputeDiagramInputIsSimple(inputRoots, theWeyl.CartanSymmetric);
}

void DynkinDiagramRootSubalgebra::ComputeDiagramTypeModifyInputRelative
(Vectors<Rational>& inputOutputSimpleWeightSystem, const HashedList<Vector<Rational> >& weightSystem, const Matrix<Rational>& theBilinearForm)
{ MacroRegisterFunctionWithName("DynkinDiagramRootSubalgebra::ComputeDiagramTypeModifyInputRelative");
  WeylGroup::TransformToSimpleBasisGeneratorsArbitraryCoords(inputOutputSimpleWeightSystem, weightSystem);
  this->ComputeDiagramInputIsSimple(inputOutputSimpleWeightSystem, theBilinearForm);
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
{ MacroRegisterFunctionWithName("DynkinDiagramRootSubalgebra::numberOfThreeValencyNodes");
  Vectors<Rational>& currentComponent = this->SimpleBasesConnectedComponents[indexComponent];
  int numEnds=0;
  int result=0;
  this->indicesThreeNodes[indexComponent] =-1;
  this->indicesEnds[indexComponent].size=0;
  for (int i=0; i<currentComponent.size; i++)
  { int counter=0;
    for (int j=0; j<currentComponent.size; j++)
      if (currentComponent[i].ScalarProduct(currentComponent[j], theBilinearForm).IsNegative())
        counter++;
    if (counter>3)
    { Matrix<Rational> theGram;
      currentComponent.GetGramMatrix(theGram, &theBilinearForm);
      crash  << "This is a programming error: corrupt simple basis corresponding to Dynkin diagram: the Dynkin diagram should have nodes with"
      << " valency at most 3, but this diagram has node with valency " << counter << ". The current component is: "
      << currentComponent.ToString() << ". The corresponding Symmetric Cartan is: "
      << theGram.ToString() << ". " << crash;
    }
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
  } else
    if(numEnds>2)
      crash << crash;
  return result;
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
  { this->ComputeOuterAutos();
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
    for (int j=0; j<this->theOuterAutos.GetElement().theElements.size; j++)
    { theWeightsCopy.GetElement()=theWeights;
      this->theOuterAutos.GetElement().theElements[j].ActOnVectorsColumn(theWeightsCopy.GetElement());
      bool isGood=true;
      for (int k=0; k<i; k++)
        if (!(theWeightsCopy.GetElement()[k]-theWeights[k]).IsPositiveOrZero())
        { isGood=false;
          break;
        }
      if (!isGood)
        continue;
      if (!(theWeightsCopy.GetElement()[i]-theWeights[i]).IsGreaterThanLexicographic(zeroWeight))
        continue;
      return false;
    }
  }
  return true;
}

std::string SemisimpleLieAlgebra::ToString(FormatExpressions* theFormat)
{ MacroRegisterFunctionWithName("SemisimpleLieAlgebra::ToString");
  std::stringstream out;
  std::string tempS;
  Vector<Rational> tempRoot, tempRoot2;
  int numRoots=this->theWeyl.RootSystem.size;
  int theDimension = this->theWeyl.CartanSymmetric.NumRows;
  ElementSemisimpleLieAlgebra<Rational> tempElt1, tempElt2, tempElt3;
//  out << beginMath << "\\begin{array}{ccc}a& a&a\\\\a&a&a\\end{array}";
  std::string hLetter="h";
  std::string gLetter="g";
  if (theFormat!=0)
  { hLetter=theFormat->chevalleyHgeneratorLetter;
    gLetter=theFormat->chevalleyGgeneratorLetter;
  }
  out << "Type " << this->theWeyl.theDynkinType.ToString() << ".";
  out << "The letter " << CGI::GetMathSpanPure(hLetter) << " stands for elements of the Cartan subalgebra, <br>"
  << " the letter " << CGI::GetMathSpanPure(gLetter) << " stands for the Chevalley (root space) generators of non-zero weight. <br>"
  << " The generator " << CGI::GetMathSpanPure(hLetter+"_i") << " is the element of the Cartan subalgebra dual to the <br>"
  << "i^th simple root, that is, " << CGI::GetMathSpanPure("[" + hLetter + "_i, g]=\\langle \\alpha_i , \\gamma\\rangle g")
  << ", <br> where g is a Chevalley generator, " << CGI::GetMathSpanPure("\\gamma") << " is its weight, and <br>"
  << CGI::GetMathSpanPure("\\alpha_i") << " is the i^th simple root. ";
  std::stringstream theTableLateXStream, theHtmlStream;
  theHtmlStream << "<table><tr><td> roots simple coords </td><td>epsilon coordinates</td>"
  << "<td>[,]</td>";
  theTableLateXStream << "\\begin{array}{cc|";
  for (int i=0; i<this->GetNumGenerators()+1; i++)
    theTableLateXStream << "c";
  theTableLateXStream << "}\n";
  theTableLateXStream << "\\mathrm{roots~simple~coords}&\\varepsilon-\\mathrm{root~notation}&" << "[\\bullet, \\bullet]\n";
  for (int i=0; i<numRoots+theDimension; i++)
  { tempElt1.MakeGenerator(i, *this);
    tempS=tempElt1.ToString(theFormat);
    theHtmlStream << "<td>" << tempS << "</td>";
    theTableLateXStream << " & ";
    theTableLateXStream << tempS;
  }
  theTableLateXStream << "\\\\\n";
  theHtmlStream << "</tr>";
  Rational tempRat;
  //int lineCounter=0;
  for (int i=0; i<theDimension+numRoots; i++)
  { tempRoot=this->GetWeightOfGenerator(i);
    theTableLateXStream << tempRoot.ToString() << "&";
    theHtmlStream << "<tr><td>" << tempRoot.ToString() << "</td>";
    this->theWeyl.GetEpsilonCoords(tempRoot, tempRoot2);
    theTableLateXStream << tempRoot2.ToStringLetterFormat("\\varepsilon") << "&";
    theHtmlStream << "<td>" << tempRoot2.ToStringLetterFormat("e") << "</td>";
    tempElt1.MakeGenerator(i, *this);
    tempS=tempElt1.ToString(theFormat);
    theTableLateXStream << tempS;
    theHtmlStream << "<td>" << tempS << "</td>";
    for (int j=0; j<numRoots+theDimension; j++)
    { tempElt2.MakeGenerator(j, *this);
      this->LieBracket(tempElt1, tempElt2, tempElt3);
      tempS=tempElt3.ToString(theFormat);
      theTableLateXStream << "& ";
      theTableLateXStream << tempS;
      theHtmlStream << "<td>" << tempS << "</td>";
    }
    theHtmlStream << "</tr>";
    theTableLateXStream << "\\\\\n";
  }
  theHtmlStream << "</table>";
  theTableLateXStream << "\\end{array}";
  if (this->GetNumGenerators()<22)
  { out << "<div class=\"math\">" << theTableLateXStream.str() << "</div>";
    return out.str();
  }
  out << "<br><b> The Lie bracket table is too large to be rendered in LaTeX, displaying in"
  << " html format instead.</b> "
  << " Below you can find the same table in pure LaTeX, "
  << " which you can render in a separate LaTeX session, should "
  << " wish to do so on your own. " <<  theHtmlStream.str()
  << "The above table in LaTex format follows. <hr>"
  << theTableLateXStream.str() << "<hr>";
  return out.str();
}

void SemisimpleLieAlgebra::ComputeChevalleyConstants(GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("SemisimpleLieAlgebra::ComputeChevalleyConstants");
  this->theWeyl.ComputeRho(true);
  this->ChevalleyConstants.init(this->theWeyl.RootSystem.size, this->theWeyl.RootSystem.size);
  this->Computed.init(this->theWeyl.RootSystem.size, this->theWeyl.RootSystem.size);
  this->Computed.MakeZero(false);
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
    Rational theHeight= posRoots[theBorelIndex].SumCoords();
    for (int i=1; i<nonExploredRoots.CardinalitySelection; i++)
    { tempRat= posRoots[nonExploredRoots.elements[i]].SumCoords();
      if (theHeight.IsGreaterThan(tempRat))
      { theHeight=(tempRat);
        theBorelIndex=nonExploredRoots.elements[i];
      }
    }
    Vector<Rational>& theRoot= posRoots[theBorelIndex];
    int theIndex=this->theWeyl.RootSystem.GetIndex(theRoot);
    Vector<Rational> smallRoot2;
    int FirstIndexFirstPosChoice=-1;
//    int FirstIndexFirstNegChoice;
    int SecondIndexFirstPosChoice=-1;
//    int SecondIndexFirstNegChoice;
    Rational CurrentHeight;
    for (int i=0; i<this->theWeyl.RootsOfBorel.size; i++)
    { Vector<Rational>& smallRoot1=this->theWeyl.RootsOfBorel[i];
      CurrentHeight=smallRoot1.SumCoords();
      int FirstPosIndex= this->theWeyl.RootSystem.GetIndex(smallRoot1);
      int FirstNegIndex= this->theWeyl.RootSystem.GetIndex(-smallRoot1);
      if (theHeight.IsGreaterThan(CurrentHeight))
      { smallRoot2=theRoot-smallRoot1;
        int SecondPosIndex=this->theWeyl.RootSystem.GetIndex(smallRoot2);
        if (FirstPosIndex<SecondPosIndex)
        { int SecondNegIndex=this->theWeyl.RootSystem.GetIndex(-smallRoot2);
          if (FirstIndexFirstPosChoice==-1)
          { FirstIndexFirstPosChoice=FirstPosIndex;
            SecondIndexFirstPosChoice=SecondPosIndex;
//             FirstIndexFirstNegChoice=FirstNegIndex;
//             SecondIndexFirstNegChoice= SecondNegIndex;
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
//  this->theLiebracketPairingIndices.MakeZero(-1);
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
{ const Vector<Rational>& rootI= this->theWeyl.RootSystem[indexI];
  const Vector<Rational>& rootJ= this->theWeyl.RootSystem[indexJ];
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
{ const Vector<Rational>& rootI= this->theWeyl.RootSystem[indexI];
  const Vector<Rational>& rootJ= this->theWeyl.RootSystem[indexJ];
  if(!this->Computed.elements[indexI][indexJ])
    crash << crash;
  int indexMinusI = this->theWeyl.RootSystem.GetIndex(-rootI);
  int indexMinusJ = this->theWeyl.RootSystem.GetIndex(-rootJ);
  this->ChevalleyConstants.elements[indexJ][indexI]=(this->ChevalleyConstants.elements[indexI][indexJ]*(-1));
  //this->ComputeDebugString();
  this->Computed.elements[indexJ][indexI]=true;
  if((rootI+rootJ).IsEqualToZero())
    crash << crash;
  int thePower;
  this->GetMaxQForWhichBetaMinusQAlphaIsARoot
  (this->theWeyl.RootSystem[indexMinusI], this->theWeyl.RootSystem[indexMinusJ], thePower);
  int i=1+thePower;
  this->ChevalleyConstants.elements[indexMinusI][indexMinusJ]=-i*i;
  this->ChevalleyConstants.elements[indexMinusI][indexMinusJ]/=this->ChevalleyConstants.elements[indexI][indexJ];
  this->Computed.elements[indexMinusI][indexMinusJ]=true;
  //this->ComputeDebugString();
  this->ChevalleyConstants.elements[indexMinusJ][indexMinusI]=this->ChevalleyConstants.elements[indexMinusI][indexMinusJ]*(-1);
  this->Computed.elements[indexMinusJ][indexMinusI]=true;
  //this->ComputeDebugString();
}

void SemisimpleLieAlgebra::ExploitTheCyclicTrick(int i, int j, int k)
{ const Vector<Rational>& rootI= this->theWeyl.RootSystem[i];
  const Vector<Rational>& rootK= this->theWeyl.RootSystem[k];
  const Vector<Rational>& rootJ= this->theWeyl.RootSystem[j];
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
  const Vector<Rational>& gamma= this->theWeyl.RootSystem[indexGamma];
  const Vector<Rational>& delta= this->theWeyl.RootSystem[indexDelta];
  const Vector<Rational>& minusEpsilon= this->theWeyl.RootSystem[indexMinusEpsilon];
  const Vector<Rational>& eta= this->theWeyl.RootSystem[indexEta];
  const Vector<Rational>& minusZeta=this->theWeyl.RootSystem[indexMinusZeta];
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
{ if (theGlobalVariables.StandardStringOutputFunction==0)
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

void GeneralizedVermaModuleCharacters::ComputeQPsFromChamberComplex(GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  FormatExpressions theFormat;
  Vector<Rational> tempRoot;
  FileOperations::OpenFileCreateIfNotPresent(this->theMultiplicitiesMaxOutputReport2, "/home/todor/math/vectorpartition/trunk/ExtremaPolys.txt", false, true, false);
  this->thePfs.initFromRoots(this->GmodKNegWeightsBasisChanged, theGlobalVariables);
  this->thePfs.ComputeDebugString(theGlobalVariables);
  out << this->thePfs.DebugString;
  this->thePfs.split(theGlobalVariables, 0);
  this->thePfs.ComputeDebugString(theGlobalVariables);
  out << "=" << this->thePfs.DebugString;
//  int totalDim=this->theTranslationS[0].size+this->theTranslationsProjecteD[0].size;
  this->theQPsSubstituted.SetSize(this->projectivizedChambeR.size);
  crash << crash;
//  this->thePfs.theChambersOld.init();
//  this->thePfs.theChambersOld.theDirections=this->GmodKNegWeightsBasisChanged;
//  this->thePfs.theChambersOld.SliceTheEuclideanSpace(theGlobalVariables, false);
//  this->theQPsNonSubstituted.SetSize(this->thePfs.theChambersOld.size);
//  this->theQPsSubstituted.SetSize(this->thePfs.theChambersOld.size);
  out << "\n\nThe vector partition functions in each chamber follow.";
  crash << crash;
/*
  for (int i=0; i<this->thePfs.theChambersOld.size; i++)
    if (this->thePfs.theChambersOld.TheObjects[i]!=0)
    { QuasiPolynomial& currentQPNoSub= this->theQPsNonSubstituted.TheObjects[i];
      this->theQPsSubstituted.TheObjects[i].SetSize(this->theLinearOperators.size);
      this->thePfs.GetVectorPartitionFunction(currentQPNoSub, this->thePfs.theChambersOld.TheObjects[i]->InternalPoint, theGlobalVariables);
      out << "\nChamber " << i+1 << " with internal point " << this->thePfs.theChambersOld.TheObjects[i]->InternalPoint.ToString() << " the quasipoly is: " << currentQPNoSub.ToString(false, false);
      for (int k=0; k<this->theLinearOperators.size; k++)
      { QuasiPolynomial& currentQPSub=this->theQPsSubstituted.TheObjects[i].TheObjects[k];
        std::stringstream tempStream;
        tempStream << "Processing chamber " << i+1 << " linear operator " << k+1;
        theGlobalVariables.theIndicatorVariables.ProgressReportStrings[0]= tempStream.str();
        theGlobalVariables.MakeReport();
        currentQPNoSub.Substitution(this->theLinearOperatorsExtended.TheObjects[k], this->theTranslationsProjectedBasisChanged[k], this->theExtendedIntegralLatticeMatForM, currentQPSub, theGlobalVariables);
        out << "; after substitution we get: " << currentQPSub.ToString(false, false);
      }
    }
    */
//  this->theParser.theHmm.theRange.theWeyl.GetIntegralLatticeInSimpleCoordinates(integralLattice);
  //out << "\nThe integral lattice:\n" << integralLattice.ToString(false, false);
  //this->theMultiplicitiesMaxOutputReport2.flush();
  QuasiPolynomial tempQP;
  this->theMultiplicities.SetSize(this->projectivizedChambeR.size);
  this->numNonZeroMults=0;
  ProgressReport theReport(&theGlobalVariables);
  ProgressReport theReport2(&theGlobalVariables);
  for (int i=0; i<this->projectivizedChambeR.size; i++)
  { QuasiPolynomial& currentSum=this->theMultiplicities.TheObjects[i];
    currentSum.MakeZeroOverLattice(this->theExtendedIntegralLatticeMatForM);
    for (int k=0; k<this->theLinearOperators.size; k++)
    { this->GetProjection(k, this->projectivizedChambeR.TheObjects[i].GetInternalPoint(), tempRoot);
      tempRoot-=this->NonIntegralOriginModificationBasisChanged;
      crash << crash ;
      int theIndex=-1;//= this->thePfs.theChambersOld.GetFirstChamberIndexContainingPoint(tempRoot);
      if (theIndex!=-1)
      { tempQP=this->theQPsSubstituted[theIndex][k];
        tempQP*=this->theCoeffs[k];
        currentSum+=tempQP;
      }
      std::stringstream tempStream;
      tempStream << " Chamber " << i+1 << " translation " << k+1;
      theReport.Report(tempStream.str());
    }
    if (!currentSum.IsEqualToZero())
      this->numNonZeroMults++;
    std::stringstream tempStream;
    tempStream << " So far " << i+1 << " out of " << this->projectivizedChambeR.size << " processed " << this->numNonZeroMults
    << " non-zero total.";
    theReport2.Report(tempStream.str());
    out << "\nChamber " << i+1 << ": the quasipolynomial is: " << currentSum.ToString(false, false);
    out << "\nThe chamber is: " << this->projectivizedChambeR[i].ToString(&theFormat);
  }
//  this->projectivizedChamber.ComputeDebugString();
//  out << "\n\n" << this->projectivizedChamber.DebugString;
  theReport.Report(out.str());
  this->theMultiplicitiesMaxOutputReport2 << out.str();
}

std::string GeneralizedVermaModuleCharacters::ComputeMultsLargerAlgebraHighestWeight
(Vector<Rational>& highestWeightLargerAlgebraFundamentalCoords, Vector<Rational>& parabolicSel, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  WeylGroup& LargerWeyl=this->theHmm.theRange().theWeyl;
  WeylGroup& SmallerWeyl=this->theHmm.theDomain().theWeyl;
  if (!LargerWeyl.IsOfSimpleType('B', 3))
    return "Error: algebra is not so(7).";
  this->initFromHomomorphism(parabolicSel, this->theHmm, theGlobalVariables);
  this->TransformToWeylProjectiveStep1(theGlobalVariables);
  this->TransformToWeylProjectiveStep2(theGlobalVariables);
  Vector<Rational> highestWeightLargerAlgSimpleCoords;
  highestWeightLargerAlgSimpleCoords=LargerWeyl.GetSimpleCoordinatesFromFundamental(highestWeightLargerAlgebraFundamentalCoords);

  Matrix<Rational>  tempMat;


  Vector<Rational> tempRoot, ZeroRoot;
  DrawingVariables& drawOps=theGlobalVariables.theDrawingVariables;
  int theSmallDim=SmallerWeyl.CartanSymmetric.NumRows;
//  drawOps.theBuffer.initDimensions(theSmallDim, 1);
  Vectors<double> theDraggableBasis;
  theDraggableBasis.MakeEiBasis(theSmallDim, 1, 0);
  WeylGroup tmpWeyl;
  tmpWeyl.MakeArbitrarySimple('A',2);
  drawOps.theBuffer.initDimensions(tmpWeyl.CartanSymmetric, theDraggableBasis, theDraggableBasis, 1);
  FormatExpressions theFormat;
  drawOps.theBuffer.BasisProjectionPlane[0][0][0]=1; drawOps.theBuffer.BasisProjectionPlane[0][0][1]=0;
  drawOps.theBuffer.BasisProjectionPlane[0][1][0]=1;  drawOps.theBuffer.BasisProjectionPlane[0][1][1]=1;
  drawOps.theBuffer.ModifyToOrthonormalNoShiftSecond
  (drawOps.theBuffer.BasisProjectionPlane[0][1], drawOps.theBuffer.BasisProjectionPlane[0][0]);
  drawOps.theBuffer.GraphicsUnit[0]=50;
  PiecewiseQuasipolynomial theStartingPoly(theGlobalVariables),
  theSubbedPoly(theGlobalVariables), Accum(theGlobalVariables);
  //stOutput << "<hr>" << this->GmodKNegWeightsBasisChanged.ToString() << "<hr>";
  std::string tempS;
  theStartingPoly.MakeVPF(this->GmodKNegWeightsBasisChanged, tempS, theGlobalVariables);
  Vectors<Rational> translationsProjectedFinal;
  translationsProjectedFinal.SetSize(this->theLinearOperators.size);
  this->theLinearOperators[0].ActOnVectorColumn(highestWeightLargerAlgSimpleCoords, translationsProjectedFinal[0]);
//  translationsProjectedFinal[0].Minus();
  out << "<br>Input so(7)-highest weight: " << highestWeightLargerAlgSimpleCoords.ToString();
  out << "<br>Input parabolics selections: " << parabolicSel.ToString();
  out << "<br>the argument translations: " << this->theTranslationsProjectedBasisChanged.ToString();
  out << "<br>Element u_w: projection, multiplication by -1, and basis change of so(7)-highest weight to G_2: "
  << translationsProjectedFinal[0].ToString();
  theStartingPoly.MakeVPF(this->GmodKNegWeightsBasisChanged, tempS, theGlobalVariables);
  //stOutput << theStartingPoly.ToString(false, true);
  drawOps.drawCoordSystemBuffer(drawOps, 2, 0);
  //out << this->log.str();
  Cone smallWeylChamber;
  tempMat=SmallerWeyl.CartanSymmetric;
  tempMat.Invert();
  Vectors<Rational> tempVertices;
  Vector<Rational> tMpRt;
  tMpRt=this->ParabolicSelectionSmallerAlgebra;
//  stOutput << "<br>sel smaller: " << tMpRt.ToString();
  for (int i=0; i<this->ParabolicSelectionSmallerAlgebra.MaxSize; i++)
  { tempMat.GetVectorFromRow(i, tempRoot);
    tempVertices.AddOnTop(tempRoot);
    if(this->ParabolicSelectionSmallerAlgebra.selected[i])
      tempVertices.AddOnTop(-tempRoot);
  }
  smallWeylChamber.CreateFromVertices(tempVertices, &theGlobalVariables);
  tempMat.init(2,2);
  tempMat.elements[0][0]=1; tempMat.elements[0][1]=0;
  tempMat.elements[1][0]=1; tempMat.elements[1][1]=1;

//  stOutput << smallWeylChamber.ToString(false, true, theFormat);
  tempMat.Transpose();
  smallWeylChamber.ChangeBasis
  (tempMat, theGlobalVariables)
  ;
//  stOutput << "<br> after the basis change: " << smallWeylChamber.ToString(false, true, theFormat);
  out << "<br> The small Weyl chamber: " << smallWeylChamber.ToString(&theFormat);
  Vector<Rational> highestWeightSmallAlgBasisChanged= -translationsProjectedFinal[0];
//  stOutput << highestWeightSmallAlgBasisChanged.ToString();
  theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit=100;
  for (int i=0; i<this->theLinearOperators.size; i++)
  { this->theLinearOperators[i].ActOnVectorColumn(highestWeightLargerAlgSimpleCoords, translationsProjectedFinal[i]);
    translationsProjectedFinal[i]+=this->theTranslationsProjectedBasisChanged[i];
    drawOps.drawCircleAtVectorBuffer(-translationsProjectedFinal[i], 3, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(250,0,0));
  }
  out << "<br>the translations projected final: " << translationsProjectedFinal.ToString();
  Accum.MakeZero(theStartingPoly.NumVariables, theGlobalVariables);
  for (int i=0; i<this->theLinearOperators.size; i++)
  { theSubbedPoly=theStartingPoly;
    theSubbedPoly*=this->theCoeffs[i];
    theSubbedPoly.TranslateArgument(translationsProjectedFinal[i], theGlobalVariables);
    //theSubbedPoly.DrawMe(tempVars);
   /* if (i==2)
    { DrawingVariables tempDV, tempDV2;
      tempDV.NumHtmlGraphics=100;
      tempDV2.NumHtmlGraphics=109;
      stOutput << "<hr><hr>first point of failure: <hr>accum: ";
      Accum.DrawMe(tempDV);
      tempDV.drawCoordSystemBuffer(tempDV, 2, 0);
      stOutput << tempDV.GetHtmlFromDrawOperationsCreateDivWithUniqueName(2);
      stOutput << "<hr>subbed poly: ";
      theSubbedPoly.DrawMe(tempDV2);
      tempDV.drawCoordSystemBuffer(tempDV2, 2, 0);
      stOutput << tempDV2.GetHtmlFromDrawOperationsCreateDivWithUniqueName(2);
    }*/
    Accum+=theSubbedPoly;
/*    if (i==2)
    { DrawingVariables tempDrawOps;
      tempDrawOps.NumHtmlGraphics=500;
      stOutput << "<hr><hr> <b>Index: " << i+1 << " out of " << this->theLinearOperators.size << "</b> <hr>";
      Accum.DrawMe(tempDrawOps);
      stOutput << tempDrawOps.GetHtmlFromDrawOperationsCreateDivWithUniqueName(2);
    }
    */
  }
//  stOutput << "<hr>so far so good!";
  drawOps.theBuffer.theDrawCircleAtVectorOperations.ReservE(2500);
  Accum.DrawMe(drawOps, 10, &smallWeylChamber, &highestWeightSmallAlgBasisChanged);
//  smallWeylChamber.DrawMeProjective(0, false, drawOps, theFormat);
//  out << tempVars.GetHtmlFromDrawOperationsCreateDivWithUniqueName(2);
  out << drawOps.GetHtmlFromDrawOperationsCreateDivWithUniqueName(2);
  out << Accum.ToString(false, true);
/*  out << refinedProjectivized.ToString(false, true);*/
/*
  for (int i=0; i<this->theLinearOperators.size; i++)
  { this->theLinearOperators[i].ActOnVectorColumn(highestWeightLargerAlg, tempRoot);
    tempRoot.Minus();
    tempRoot-=this->theTranslationsProjecteD[i];
    drawOps.
    drawCircleAtVectorBuffer(tempRoot, 2, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(0,0,100));
    drawOps.drawLineBetweenTwoVectorsBuffer(tempRoot, ZeroRoot, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(240, 240, 240));
    tempComplex.DrawMeProjective(&tempRoot, false, drawOps, tempFormat);
    out << tempRoot.ToString();
  }
  drawOps.drawCoordSystemBuffer(drawOps, 3, 0);
  out << drawOps.GetHtmlFromDrawOperationsCreateDivWithUniqueName(2);*/

  return out.str();
}

void GeneralizedVermaModuleCharacters::SortMultiplicities(GlobalVariables& theGlobalVariables)
{ List<Cone> tempList;
  tempList=this->projectivizedChambeR;
  tempList.QuickSortAscending();
  List<QuasiPolynomial> tempQPlist;
  tempQPlist.SetSize(this->theMultiplicities.size);
  for (int i=0; i<this->theMultiplicities.size; i++)
    tempQPlist[i]=this->theMultiplicities[this->projectivizedChambeR.GetIndex(tempList[i])];
  this->theMultiplicities=tempQPlist;
  this->projectivizedChambeR.Clear();
  for (int i=0; i<tempList.size; i++)
    this->projectivizedChambeR.AddOnTop(tempList[i]);
}

std::string GeneralizedVermaModuleCharacters::CheckMultiplicitiesVsOrbits(GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  int totalDimAffine=this->WeylLarger.GetDim()+this->WeylSmaller.GetDim();
  int smallDim=this->WeylSmaller.GetDim();
  Vector<Rational> normal;
  normal.MakeZero(totalDimAffine+1);
  Vectors<Rational> newWalls;
  ConeComplex tempComplex;
  tempComplex=this->projectivizedChambeR;
  for (int i=0; i<this->WeylChamberSmallerAlgebra.Normals.size; i++)
  { for (int j=0; j<smallDim; j++)
      normal[j]=this->WeylChamberSmallerAlgebra.Normals[i][j];
    newWalls.AddOnTop(normal);
    tempComplex.splittingNormals.AddOnTop(normal);
  }
  tempComplex.indexLowestNonRefinedChamber=0;
  tempComplex.Refine(theGlobalVariables);
  out << "Number chambers with new walls: " << tempComplex.size;
  out << "\n" << tempComplex.ToString();
  return out.str();
}

void GeneralizedVermaModuleCharacters::IncrementComputation
  (Vector<Rational>& parabolicSel, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
//  this->UpperLimitChambersForDebugPurposes=5;
  this->thePauseControlleR.InitComputation();
  this->ParabolicLeviPartRootSpacesZeroStandsForSelected=parabolicSel;
  if (false)
  if (this->UpperLimitChambersForDebugPurposes==0 || this->theLinearOperators.size==0)
    this->ReadFromDefaultFile(&theGlobalVariables);
  switch (this->computationPhase)
  { case 0:
//      this->theParser.theHmm.MakeG2InB3(this->theParser, theGlobalVariables);
      this->initFromHomomorphism(parabolicSel, this->theHmm, theGlobalVariables);
      this->TransformToWeylProjectiveStep1(theGlobalVariables);
//      out << theGlobalVariables.theIndicatorVariables.StatusString1;
      this->TransformToWeylProjectiveStep2(theGlobalVariables);
//      out << theGlobalVariables.theIndicatorVariables.StatusString1;
      break;
    case 1:
      this->projectivizedChambeR.Refine(theGlobalVariables);
      this->SortMultiplicities(theGlobalVariables);
      out << this->projectivizedChambeR.ToString(false, false);
//      out << theGlobalVariables.theIndicatorVariables.StatusString1;
      break;
    case 2:
      this->ComputeQPsFromChamberComplex(theGlobalVariables);
      out << this->ElementToStringMultiplicitiesReport(theGlobalVariables);
      break;
    case 3:
//      out << this->CheckMultiplicitiesVsOrbits(theGlobalVariables);
      break;
    case 4:
      this->InitTheMaxComputation(theGlobalVariables);
//      out << theGlobalVariables.theIndicatorVariables.StatusString1;
      break;
    case 5:
      this->theMaxComputation.FindExtremaParametricStep1(this->thePauseControlleR, true, theGlobalVariables);
//      out << theGlobalVariables.theIndicatorVariables.StatusString1;
      break;
    case 6:
      this->theMaxComputation.FindExtremaParametricStep3(this->thePauseControlleR, theGlobalVariables);
//      out << theGlobalVariables.theIndicatorVariables.StatusString1;
      break;
    case 7:
      this->theMaxComputation.FindExtremaParametricStep4(this->thePauseControlleR, theGlobalVariables);
//      out << theGlobalVariables.theIndicatorVariables.StatusString1;
      break;
    case 8:
      this->theMaxComputation.FindExtremaParametricStep5(this->thePauseControlleR, theGlobalVariables);
//      out << theGlobalVariables.theIndicatorVariables.StatusString1;
      break;
    default:
      break;
  }
  this->computationPhase++;
//  theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
  if (this->computationPhase>8)
  { //theGlobalVariables.theIndicatorVariables.StatusString1=this->PrepareReport(theGlobalVariables);
  }
//  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
//  theGlobalVariables.MakeReport();
  if (this->UpperLimitChambersForDebugPurposes<=0)
    if (this->computationPhase < 30)
      this->WriteToDefaultFile(&theGlobalVariables);
  this->thePauseControlleR.ExitComputation();
}

void GeneralizedVermaModuleCharacters::WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables)
{ output << XML::GetOpenTagNoInputCheckAppendSpacE(this->GetXMLClassName());
  output << "ComputationPhase: " << this->computationPhase << "\n";
  output << "NumProcessedConesParam: " << this->NumProcessedConesParam << "\n";
  output << "NumProcessedExtremaEqualOne: " << this->NumProcessedExtremaEqualOne << "\n";
  output << "NormalConstAdjustment: ";
  this->NonIntegralOriginModificationBasisChanged.WriteToFile(output);
  output << "\n";
  output << "ChamberIndicatorHighestWeightLargerAlgebra: ";
  this->ParabolicLeviPartRootSpacesZeroStandsForSelected.WriteToFile(output);
  this->ParabolicSelectionSmallerAlgebra.WriteToFile(output);
  output << "\n";
  this->WeylLarger.WriteToFile(output);
  this->WeylSmaller.WriteToFile(output);
  this->preferredBasiS.WriteToFile(output);
  this->preferredBasisChangE.WriteToFile(output);
  this->preferredBasisChangeInversE.WriteToFile(output);
  this->theExtendedIntegralLatticeMatForM.WriteToFile(output, theGlobalVariables);
  ProgressReport theReport(theGlobalVariables);
  if (theGlobalVariables!=0)
    theReport.Report("Writing small data... ");
  this->theMaxComputation.WriteToFile(output, theGlobalVariables);
  this->GmodKnegativeWeightS.WriteToFile(output, theGlobalVariables);
  this->GmodKNegWeightsBasisChanged.WriteToFile(output, theGlobalVariables);
  this->theLinearOperators.WriteToFile(output);
  this->theLinearOperatorsExtended.WriteToFile(output);
  this->PreimageWeylChamberLargerAlgebra.WriteToFile(output, theGlobalVariables);
  this->PreimageWeylChamberSmallerAlgebra.WriteToFile(output, theGlobalVariables);
  this->WeylChamberSmallerAlgebra.WriteToFile(output, theGlobalVariables);
  if (theGlobalVariables!=0)
    theReport.Report("Writing QP's non-subbed... ");
  this->theQPsNonSubstituted.WriteToFile(output, theGlobalVariables);
  if (theGlobalVariables!=0)
    theReport.Report("Writing QP's subbed... ");
  output << XML::GetOpenTagNoInputCheckAppendSpacE("QPsSubbed");
  this->theQPsSubstituted.WriteToFile(output, theGlobalVariables);
  output << XML::GetCloseTagNoInputCheckAppendSpacE("QPsSubbed");
  if (theGlobalVariables!=0)
    theReport.Report("Writing small data... ");

  output << XML::GetOpenTagNoInputCheckAppendSpacE("theMultiplicities");
  this->theMultiplicities.WriteToFile(output, theGlobalVariables, this->UpperLimitChambersForDebugPurposes);
  output << XML::GetCloseTagNoInputCheckAppendSpacE("theMultiplicities");
//  this->theMultiplicitiesExtremaCandidates.WriteToFile(output, theGlobalVariables);
  this->theCoeffs.WriteToFile(output);
  this->theTranslationS.WriteToFile(output, theGlobalVariables);
  this->theTranslationsProjectedBasisChanged.WriteToFile(output, theGlobalVariables);
  this->thePfs.WriteToFile(output, theGlobalVariables);
//  this->paramSubChambers.WriteToFile(output, theGlobalVariables);
//  this->nonParamVertices.WriteToFile(output, theGlobalVariables);
  if (theGlobalVariables!=0)
    theReport.Report("Writing param chamber complex... ");
  this->projectivizedParamComplex.WriteToFile(output, theGlobalVariables);
  if (theGlobalVariables!=0)
    theReport.Report("Writing projectivized chamber complex... ");
  this->smallerAlgebraChamber.WriteToFile(output, theGlobalVariables, this->UpperLimitChambersForDebugPurposes);
  this->projectivizedChambeR.WriteToFile(output, theGlobalVariables, this->UpperLimitChambersForDebugPurposes);
  if (theGlobalVariables!=0)
    theReport.Report("Writing to file done...");
  output << XML::GetCloseTagNoInputCheckAppendSpacE(this->GetXMLClassName());
}

void GeneralizedVermaModuleCharacters::initFromHomomorphism(Vector<Rational>& theParabolicSel, HomomorphismSemisimpleLieAlgebra& input, GlobalVariables& theGlobalVariables)
{ Vectors<Rational> tempRoots;
  this->WeylLarger=input.theRange().theWeyl;
  this->WeylSmaller=input.theDomain().theWeyl;
  WeylGroup& theWeYl=input.theRange().theWeyl;
//  input.ProjectOntoSmallCartan(theWeyl.RootsOfBorel, tempRoots, theGlobalVariables);
  this->log << "projections: " << tempRoots.ToString();
  theWeYl.ComputeAllElements();
  this->NonIntegralOriginModificationBasisChanged="(1/2,1/2)";
  Matrix<Rational>  theProjectionBasisChanged;
  Vector<Rational> startingWeight, projectedWeight;
  FormatExpressions theFormat;
  crash << crash;
//  SSalgebraModuleOld tempM;
  std::stringstream tempStream;
  input.ComputeHomomorphismFromImagesSimpleChevalleyGenerators(theGlobalVariables);
  crash << crash;
//  tempM.InduceFromEmbedding(tempStream, input, theGlobalVariables);
  input.GetWeightsGmodKInSimpleCoordsK(this->GmodKnegativeWeightS, theGlobalVariables);
//  this->log << "weights of g mod k: " << this->GmodKnegativeWeights.ToString();
  Matrix<Rational>  tempMat;
  tempMat=input.theDomain().theWeyl.CartanSymmetric;
  tempMat.Invert(&theGlobalVariables);
//  tempMat.ActOnVectorsColumn(this->GmodKnegativeWeightS);
  this->log << this->GmodKnegativeWeightS.ToString();
  this->preferredBasiS.SetSize(2);
  this->preferredBasiS[0]=-this->GmodKnegativeWeightS[1];
  this->preferredBasiS[1]=-this->GmodKnegativeWeightS[2];
  /////////////////////////////////////////
  //this->preferredBasiS[0]="(1,0)";
  //this->preferredBasiS[1]="(0,1)";
  ////////////////////////////////////////
  this->preferredBasisChangE.AssignVectorsToRows(this->preferredBasiS);
  this->preferredBasisChangE.Transpose();
  this->preferredBasisChangeInversE=this->preferredBasisChangE;
  this->preferredBasisChangeInversE.Invert();
  this->preferredBasisChangeInversE.ActOnVectorsColumn
  (this->GmodKnegativeWeightS, this->GmodKNegWeightsBasisChanged);
  this->log << "\nWeights after basis change: " << this->GmodKNegWeightsBasisChanged.ToString();
  for (int i=0; i<this->GmodKnegativeWeightS.size; i++)
    if (this->GmodKnegativeWeightS[i].IsPositiveOrZero())
    { this->GmodKnegativeWeightS.RemoveIndexSwapWithLast(i);
      i--;
    }
  for (int i=0; i<this->GmodKNegWeightsBasisChanged.size; i++)
    if (this->GmodKNegWeightsBasisChanged[i].IsPositiveOrZero())
    { this->GmodKNegWeightsBasisChanged.RemoveIndexSwapWithLast(i);
      i--;
    }
  this->log << "\nNegative weights after basis change: " << this->GmodKNegWeightsBasisChanged.ToString();
  theProjectionBasisChanged.init(input.theDomain().GetRank(), input.theRange().GetRank());
  for (int i=0; i<input.theRange().GetRank(); i++)
  { startingWeight.MakeEi(input.theRange().GetRank(), i);
    input.ProjectOntoSmallCartan(startingWeight, projectedWeight, theGlobalVariables);
    this->preferredBasisChangeInversE.ActOnVectorColumn(projectedWeight);
    for (int j=0; j<projectedWeight.size; j++)
      theProjectionBasisChanged.elements[j][i]=projectedWeight[j];
  }
  SubgroupWeylGroupOLD theSubgroup;
  this->ParabolicLeviPartRootSpacesZeroStandsForSelected=theParabolicSel;
  Matrix<Rational>  DualCartanEmbedding;
  input.GetMapSmallCartanDualToLargeCartanDual(DualCartanEmbedding);
  Vector<Rational> ParabolicEvaluationRootImage, tempRoot;
  ParabolicEvaluationRootImage=this->ParabolicLeviPartRootSpacesZeroStandsForSelected;
  this->ParabolicSelectionSmallerAlgebra.init(input.theDomain().GetRank());
  for (int i=0; i<input.theDomain().GetRank(); i++)
  { DualCartanEmbedding.GetVectorFromColumn(i, tempRoot);
    if (ParabolicEvaluationRootImage.ScalarEuclidean(tempRoot).IsPositive())
      this->ParabolicSelectionSmallerAlgebra.AddSelectionAppendNewIndex(i);
  }
  this->log << "\nDual cartan embedding smaller into larger:\n" <<
  DualCartanEmbedding.ToString(&theGlobalVariables.theDefaultFormat);
  this->log << "\nParabolic subalgebra large algebra: " << this->ParabolicLeviPartRootSpacesZeroStandsForSelected.ToString();
  tempRoot= this->ParabolicSelectionSmallerAlgebra;
  this->log << "\nParabolic subalgebra smaller algebra: " << tempRoot.ToString();
  theSubgroup.MakeParabolicFromSelectionSimpleRoots(theWeYl, this->ParabolicLeviPartRootSpacesZeroStandsForSelected, theGlobalVariables, -1);

  this->theLinearOperators.SetSize(theSubgroup.size);
  this->theLinearOperatorsExtended.SetSize(theSubgroup.size);
  this->theTranslationS.SetSize(theSubgroup.size);
  this->theTranslationsProjectedBasisChanged.SetSize(theSubgroup.size);
  this->theCoeffs.SetSize(theSubgroup.size);
  this->log << " \n******************\nthe subgroup: \n" << theSubgroup.ToString() << "\n\n\n\n\n\n";
  this->log << theSubgroup.ElementToStringBruhatGraph();
  this->log << "\nMatrix form of the elements of Weyl group of the Levi part of the parabolic (" << theSubgroup.size << " elements):\n";
  for (int i=0; i<theSubgroup.size; i++)
  { Matrix<Rational>& currentLinearOperator=this->theLinearOperators[i];
    theSubgroup.GetMatrixOfElement(theSubgroup[i], currentLinearOperator);
//    currentLinearOperator.MultiplyOnTheLeft(preferredBasisChangeInverse);
    this->log << "\n" << currentLinearOperator.ToString(&theGlobalVariables.theDefaultFormat);
    currentLinearOperator.ActOnVectorColumn(theSubgroup.GetRho(), this->theTranslationS[i]);
    this->theTranslationS[i]-=theSubgroup.GetRho();
    this->theTranslationS[i].Minus();
    theProjectionBasisChanged.ActOnVectorColumn(this->theTranslationS[i], this->theTranslationsProjectedBasisChanged[i]);
    if (theSubgroup[i].generatorsLastAppliedFirst.size%2==0)
      this->theCoeffs[i]=1;
    else
      this->theCoeffs[i]=-1;
  }
  this->log << "\n\n\nMatrix of the projection operator (basis-changed):\n"
  << theProjectionBasisChanged.ToString(&theGlobalVariables.theDefaultFormat);
  this->log << "\n\n\nMatrix form of the operators $u_w$, the translations $\tau_w$ and their projections (" << this->theLinearOperatorsExtended.size << "):";
  //List<Matrix<Rational> > tempList;
  for (int k=0; k<this->theLinearOperators.size; k++)
  { Matrix<Rational> & currentLO=this->theLinearOperators.TheObjects[k];
    Matrix<Rational> & currentLOExtended=this->theLinearOperatorsExtended.TheObjects[k];
    currentLO.MultiplyOnTheLeft(theProjectionBasisChanged);
    currentLO*=-1;
    //tempList.AddOnTopNoRepetition(this->theLinearOperators.TheObjects[i]);
    currentLOExtended.MakeIdMatrix(currentLO.NumRows);
    currentLOExtended.Resize(currentLO.NumRows, currentLO.NumRows+currentLO.NumCols, true);
    for (int i=0; i<currentLO.NumRows; i++)
      for (int j=0; j<currentLO.NumCols; j++)
        currentLOExtended.elements[i][j+currentLO.NumRows]=currentLO.elements[i][j];
    this->log << "\n\n" << currentLOExtended.ToString(&theGlobalVariables.theDefaultFormat);
    this->log << this->theTranslationS[k].ToString() << ";   " << this->theTranslationsProjectedBasisChanged[k].ToString();
  }

  List<int> displayIndicesReflections;
  for (int i=0; i<this->ParabolicLeviPartRootSpacesZeroStandsForSelected.MaxSize; i++)
    if (!this->ParabolicLeviPartRootSpacesZeroStandsForSelected.selected[i])
      displayIndicesReflections.AddOnTop(i+1);
  Matrix<Polynomial<Rational> > tempMatPoly;
  Vector<Polynomial<Rational> > tempVect, tempVect2;
  tempVect.SetSize(input.theDomain().theWeyl.GetDim()+input.theRange().theWeyl.GetDim());
  for (int i=0; i<tempVect.size; i++)
    tempVect[i].MakeMonomiaL(i, 1, (Rational) 1, tempVect.size);
  tempMatPoly.init(input.theDomain().theWeyl.GetDim(), tempVect.size);
  Polynomial<Rational> polyZero;
  polyZero.MakeZero();
  theFormat.polyAlphabeT.SetSize(5);
  theFormat.polyAlphabeT[0]="x_1";
  theFormat.polyAlphabeT[1]="x_2";
  theFormat.polyAlphabeT[2]="y_1";
  theFormat.polyAlphabeT[3]="y_2";
  theFormat.polyAlphabeT[4]="y_3";
  tempRoot=theSubgroup.GetRho();
  this->theLinearOperators[0].ActOnVectorColumn(tempRoot);
  this->preferredBasisChangE.ActOnVectorColumn(tempRoot);
  tempRoot.Minus();
  this->log << "\n\nIn $so(7)$-simple basis coordinates, $\\rho_{\\mathfrak l}=" << theSubgroup.GetRho().ToStringLetterFormat("\\eta") << "$; $\\pr(\\rho)="
  << tempRoot.ToStringLetterFormat("\\alpha") << "$.";
  this->log << "\n\n\\begin{longtable}{r|l}$w$ & \\begin{tabular}{c}Argument of the vector partition function in (\\ref{eqMultG2inB3General}) =\\\\ $u_w\\circ"
  << tempVect.ToString(&theFormat) << "-\\tau_w$ \\end{tabular}  \\\\ \\hline \\endhead";
  for (int i=0; i<this->theLinearOperatorsExtended.size; i++)
  { Matrix<Rational> & currentLoExt=this->theLinearOperatorsExtended[i];
    for (int j=0; j<currentLoExt.NumRows; j++)
      for (int k=0; k<currentLoExt.NumCols; k++)
        tempMatPoly.elements[j][k].MakeConst(currentLoExt.elements[j][k], tempVect.size);
    tempMatPoly.ActOnVectorColumn(tempVect, tempVect2, polyZero);
    for (int j=0; j<tempVect2.size; j++)
      tempVect2[j]+=this->theTranslationsProjectedBasisChanged[i][j];
    this->log << "\n$" <<  theSubgroup[i].ToString(0, & displayIndicesReflections) << "$&$"
    << tempVect2.ToString(&theFormat) << "$\\\\";
  }
  this->log <<"\\end{longtable}\n\n";
//  this->log << "\n\n\nThere are " << tempList.size << " different operators.";
  Lattice tempLattice;
  theWeYl.GetIntegralLatticeInSimpleCoordinates(tempLattice);
  this->theExtendedIntegralLatticeMatForM.basisRationalForm.MakeIdMatrix(input.theDomain().GetRank());
  this->theExtendedIntegralLatticeMatForM.basisRationalForm.DirectSumWith(tempLattice.basisRationalForm, (Rational) 0);
  this->theExtendedIntegralLatticeMatForM.MakeFromMat(this->theExtendedIntegralLatticeMatForM.basisRationalForm);
  tempMat=theWeYl.CartanSymmetric;
  tempMat.Invert(&theGlobalVariables);
  Vectors<Rational> WallsWeylChamberLargerAlgebra;
  for (int i=0; i<tempMat.NumRows; i++)
  { tempMat.GetVectorFromRow(i, tempRoot);
    if(ParabolicEvaluationRootImage[i].IsEqualToZero())
    { WallsWeylChamberLargerAlgebra.SetSize(WallsWeylChamberLargerAlgebra.size+1);
      *WallsWeylChamberLargerAlgebra.LastObject()=tempRoot;
    }
  }
  this->log << "\n\n\n**************\nParabolic selection larger algebra:" << ParabolicEvaluationRootImage.ToString() << "\nWalls Weyl chamber larger algebra: " << WallsWeylChamberLargerAlgebra.ToString();
  this->log << "\n**************\n\n";
  Vectors<Rational> rootsGeneratingExtendedLattice;
  int totalDim=input.theRange().GetRank()+input.theDomain().GetRank();
  rootsGeneratingExtendedLattice.SetSize(totalDim);
  this->log << "\n" << tempMat.ToString(&theGlobalVariables.theDefaultFormat) << "\n";
  this->log << this->theExtendedIntegralLatticeMatForM.ToString(false, false);
  this->WeylChamberSmallerAlgebra.CreateFromNormals(WallsWeylChamberLargerAlgebra, &theGlobalVariables);
  this->log << "\nWeyl chamber larger algebra before projectivizing: " << this->WeylChamberSmallerAlgebra.ToString(&theFormat) << "\n";
  this->PreimageWeylChamberSmallerAlgebra.Normals=this->WeylChamberSmallerAlgebra.Normals;
  for (int i=0; i<this->PreimageWeylChamberLargerAlgebra.Normals.size; i++)
  { tempRoot.MakeZero(input.theRange().GetRank()+input.theDomain().GetRank()+1);
    for (int j=0; j<input.theRange().GetRank(); j++)
      tempRoot.TheObjects[j+input.theDomain().GetRank()]=this->PreimageWeylChamberLargerAlgebra.Normals[i][j];
    this->PreimageWeylChamberLargerAlgebra.Normals[i]=tempRoot;
  }
  tempMat=input.theDomain().theWeyl.CartanSymmetric;
  tempMat.Invert(&theGlobalVariables);
  tempRoots.size=0;
  Vector<Rational> ParabolicEvaluationRootSmallerAlgebra;
  ParabolicEvaluationRootSmallerAlgebra=this->ParabolicSelectionSmallerAlgebra;
  for (int i=0; i<tempMat.NumRows; i++)
  { input.theDomain().theWeyl.CartanSymmetric.GetVectorFromRow(i, tempRoot);
    if (tempRoot.ScalarEuclidean(ParabolicEvaluationRootSmallerAlgebra).IsEqualToZero())
    { tempRoots.SetSize(tempRoots.size+1);
      tempMat.GetVectorFromRow(i, *tempRoots.LastObject());
    }
  }
  this->preferredBasisChangeInversE.ActOnVectorsColumn(tempRoots);
  this->log << "**********************\n\n\n";
  this->log << "\nthe smaller parabolic selection: " << this->ParabolicSelectionSmallerAlgebra.ToString();
  this->log << "the Vectors<Rational> generating the chamber walls: " << tempRoots.ToString();
  this->PreimageWeylChamberSmallerAlgebra.CreateFromVertices(tempRoots, &theGlobalVariables);
  this->log << "\nWeyl chamber smaller algebra: " << this->PreimageWeylChamberSmallerAlgebra.ToString(&theFormat) << "\n";
  this->log << "**********************\n\n\n";
  this->log << "\nThe first operator extended:\n" << this->theLinearOperatorsExtended[0].ToString(&theGlobalVariables.theDefaultFormat) << "\n";
  this->log << "\nThe second operator extended:\n" << this->theLinearOperatorsExtended[1].ToString(&theGlobalVariables.theDefaultFormat) << "\n";
  /*tempMat=this->theLinearOperatorsExtended.TheObjects[0];
  tempMat.Transpose();
  tempMat.ActOnVectorsColumn(this->PreimageWeylChamberSmallerAlgebra);
  for (int i=0; i<this->PreimageWeylChamberSmallerAlgebra.size; i++)
  { this->PreimageWeylChamberSmallerAlgebra.TheObjects[i].SetSize(input.theRange.GetRank()+input.theDomain.GetRank()+1);
    *this->PreimageWeylChamberSmallerAlgebra.TheObjects[i].LastObject()=0;
  }*/
  for (int i=0; i<this->PreimageWeylChamberSmallerAlgebra.Normals.size; i++)
  { tempRoot.MakeZero(input.theRange().GetRank()+input.theDomain().GetRank()+1);
    for (int j=0; j<input.theDomain().GetRank(); j++)
      tempRoot.TheObjects[j]=this->PreimageWeylChamberSmallerAlgebra.Normals[i][j];
  //  for (int j=0; j<input.theRange.GetRank(); j++)
   //   tempRoot.TheObjects[j+input.theDomain.GetRank()]=tempRoot2.TheObjects[j];
    this->PreimageWeylChamberSmallerAlgebra.Normals[i]=tempRoot;
  }

  tempRoot.MakeEi(input.theRange().GetRank()+input.theDomain().GetRank()+1, input.theRange().GetRank()+input.theDomain().GetRank());
  this->PreimageWeylChamberLargerAlgebra.Normals.AddOnTop(tempRoot);
  this->log << "\nPreimage Weyl chamber smaller algebra: " << this->PreimageWeylChamberSmallerAlgebra.ToString(&theFormat) << "\n";
  this->log << "\nPreimage Weyl chamber larger algebra: " << this->PreimageWeylChamberLargerAlgebra.ToString(&theFormat) << "\n";

  //theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  //theGlobalVariables.theIndicatorVariables.StatusString1=this->log.str();
  //theGlobalVariables.MakeReport();
}

std::string GeneralizedVermaModuleCharacters::PrepareReport(GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  FormatExpressions theFormat;
  int tempI=0;
  Vector<Rational> tempRoot;
  theFormat.polyAlphabeT.SetSize(5);
  theFormat.polyAlphabeT[tempI]="x_1"; tempI++;
  theFormat.polyAlphabeT[tempI]="x_2"; tempI++;
  theFormat.polyAlphabeT[tempI]="y_1"; tempI++;
  theFormat.polyAlphabeT[tempI]="y_2"; tempI++;
  theFormat.polyAlphabeT[tempI]="y_3"; tempI++;
  out << "\\documentclass{article}\\usepackage{amsmath, longtable, amsfonts, amssymb, verbatim, hyperref}"
  << "\n\\begin{document}\\tiny\n";
  out << "\n The chamber complex + multiplicities follow.\n\n\n"
  << "\\begin{longtable}{cc}\\caption{Multiplicities of generalized Verma modules $m(x_1,x_2, y_1, y_2, y_3)$"
  << " for $\\gop$ with Dynkin diagram";
  std::stringstream tempStream;
  tempStream << "(";
  for (int i=0; i<this->ParabolicLeviPartRootSpacesZeroStandsForSelected.MaxSize; i++)
  { if (this->ParabolicLeviPartRootSpacesZeroStandsForSelected.selected[i])
      tempStream << "+";
    else
      tempStream << "0";
    if (i!=this->ParabolicLeviPartRootSpacesZeroStandsForSelected.MaxSize-1)
      tempStream << ",";
  }
  tempStream << ")";
  out << "$" << tempStream.str() <<  "$ \\label{table" << tempStream.str() << "}}\\\\\n";
  out << "Inequlities& $m(x_1,x_2, y_1, y_2, y_3)$\\endhead\n";
  int numFoundChambers=0;
  List<int> DisplayIndicesprojectivizedChambers;
  for (int i=0; i<this->projectivizedChambeR.size; i++)
  { QuasiPolynomial& theMult=this->theMultiplicities[i];
    if (!theMult.IsEqualToZero())
    { numFoundChambers++;
      out << "\\hline\\multicolumn{2}{c}{Chamber " << numFoundChambers << "}\\\\\n";
      DisplayIndicesprojectivizedChambers.AddOnTop(numFoundChambers);
      out << this->PrepareReportOneCone(theFormat, this->projectivizedChambeR[i], theGlobalVariables) << "&";
      out << "\\begin{tabular}{c}";
      out << theMult.ToString(false, true, &theFormat) << "\\end{tabular}\\\\\n";
    } else
      DisplayIndicesprojectivizedChambers.AddOnTop(-1);
  }
  out << "\\end{longtable}\n\n\n Multiplicity free chambers \n";
  numFoundChambers=0;
  out << "\n\\begin{longtable}{cc} ";
  out << "Normals& Multiplicity of module with highest weight $(x_1,x_2)$\\endhead\n";
 /* for (int i=0; i<this->projectivezedChambersSplitByMultFreeWalls.size; i++)
  { tempRoot=this->projectivezedChambersSplitByMultFreeWalls.TheObjects[i].GetInternalPoint();
    bool found=false;
    for (int j=0; j<this->projectivizedChamber.size; j++)
      if (this->projectivizedChamber.TheObjects[j].IsInCone(tempRoot))
      { if(found)
          crash << crash;
        found=true;
      }
  }
  for (int i=0; i<this->projectivizedChamber.size; i++)
  { QuasiPolynomial& theMult=this->theMultiplicities.TheObjects[i];
    if (!theMult.IsEqualToZero())
    { int indexMultFreeChamber=-1;
      for (int j=0; j<this->projectivezedChambersSplitByMultFreeWalls.size; j++)
      { tempRoot=this->projectivezedChambersSplitByMultFreeWalls.TheObjects[j].GetInternalPoint();
        if (this->projectivizedChamber.TheObjects[i].IsInCone(tempRoot))
        { Rational tempRat;
          tempRat=*tempRoot.LastObject();
          if (tempRat!=0)
            tempRoot/=tempRat;
          theMult.valueOnEachLatticeShift.TheObjects[0].Evaluate(tempRoot, tempRat);
          if(tempRat<1)
          { indexMultFreeChamber=j;
            break;
          }
        }
      }
      if (indexMultFreeChamber!=-1)
      { numFoundChambers++;
        out << "\\hline\\multicolumn{2}{c}{Chamber " << DisplayIndicesprojectivizedChambers.TheObjects[i] << "}\\\\\n";
        out << this->PrepareReportOneCone(theFormat, this->projectivezedChambersSplitByMultFreeWalls.TheObjects[indexMultFreeChamber], theGlobalVariables) << "&";
        out << theMult.ToString(false, true, theFormat) << "\\\\\n";
      }
    }
  }*/
  out << "Total number of chambers with multiplicity 1 or less: " << numFoundChambers;
  out << "\\end{longtable}\n\n\n\n";
  out << "\\end{document}";
  return out.str();
}

void GeneralizedVermaModuleCharacters::InitTheMaxComputation(GlobalVariables& theGlobalVariables)
{ this->theMaxComputation.numNonParaM=2;
  this->theMaxComputation.theConesLargerDim.ReservE(this->projectivizedChambeR.size);
  this->theMaxComputation.LPtoMaximizeLargerDim.ReservE(this->theMultiplicities.size);
  this->theMaxComputation.theConesLargerDim.SetSize(0);
  this->theMaxComputation.LPtoMaximizeLargerDim.SetSize(0);
  Lattice ZnLattice;
  int theAffineDim=5;
//  int theProjectivizedDim=theAffineDim+1;
  ZnLattice.MakeZn(theAffineDim);
  this->numNonZeroMults=0;
  ProgressReport theReport(&theGlobalVariables);
  ConeLatticeAndShift currentCLS;
  Vector<Rational> theLPtoMax;
  for (int i=0; i<this->theMultiplicities.size; i++)
    if (! this->theMultiplicities[i].IsEqualToZero())
    { currentCLS.theProjectivizedCone=this->projectivizedChambeR.TheObjects[i];
      currentCLS.theShift.MakeZero(theAffineDim);
      currentCLS.theLattice=ZnLattice;
      bool tempBool= this->theMultiplicities[i].valueOnEachLatticeShift[0].GetRootFromLinPolyConstTermLastVariable(theLPtoMax, (Rational) 0);
      if(!tempBool)
        crash << crash;
      this->theMaxComputation.theConesLargerDim.AddOnTop(currentCLS);
      this->theMaxComputation.LPtoMaximizeLargerDim.AddOnTop(theLPtoMax);
      this->numNonZeroMults++;
      std::stringstream out;
      out << "Initialized " << i+1 << " out of " << this->theMaxComputation.theConesLargerDim.size
      << "; so far " << this->numNonZeroMults << " non-zero multiplicities";
      theReport.Report(out.str());
    }
}

std::string GeneralizedVermaModuleCharacters::PrepareReportOneCone
(FormatExpressions& theFormat, const Cone& theCone, GlobalVariables& theGlobalVariables)
{ std::stringstream out1;
  std::string tempS1, tempS2;
  Vector<Rational> normalNoConstant;
  int dimSmallerAlgebra=this->theLinearOperators[0].NumRows;
  int dimLargerAlgebra=this->theLinearOperators[0].NumCols;
  Rational theConst;
  out1 << "\\begin{tabular}{rcl}";
  for (int i=0; i<theCone.Normals.size; i++)
  { Vector<Rational>& currentNormal=theCone.Normals[i];
    normalNoConstant=currentNormal;
    normalNoConstant.SetSize(dimSmallerAlgebra+dimLargerAlgebra);
    theConst=-(*currentNormal.LastObject());
    if(!normalNoConstant.IsEqualToZero())
      out1 << "$" << normalNoConstant.ToStringLetterFormat("x", &theFormat) << "$ & $\\geq$ & $"
      << theConst.ToString() << "$ \\\\";
  }
  out1 << "\\end{tabular}";
  return out1.str();
}

bool GeneralizedVermaModuleCharacters::ReadFromFileNoComputationPhase(std::fstream& input, GlobalVariables* theGlobalVariables)
{ std::string tempS;
  ProgressReport theReport(theGlobalVariables);
  input >> tempS >> this->NumProcessedConesParam;
  input >> tempS >> this->NumProcessedExtremaEqualOne;
  input >> tempS;
  int numReadWords;
  if (tempS!="NormalConstAdjustment:")
  { crash << crash;
    return false;
  }
  this->NonIntegralOriginModificationBasisChanged.ReadFromFile(input);
  input >> tempS;
  this->ParabolicLeviPartRootSpacesZeroStandsForSelected.ReadFromFile(input);
  this->ParabolicSelectionSmallerAlgebra.ReadFromFile(input);
  this->WeylLarger.ReadFromFile(input);
  this->WeylSmaller.ReadFromFile(input);
  this->preferredBasiS.ReadFromFile(input, theGlobalVariables);
  this->preferredBasisChangE.ReadFromFile(input, theGlobalVariables);
  this->preferredBasisChangeInversE.ReadFromFile(input);
  this->theExtendedIntegralLatticeMatForM.ReadFromFile(input, theGlobalVariables);
  if (theGlobalVariables!=0)
{} //   this->theParser.theHmm.MakeG2InB3(this->theParser, *theGlobalVariables);
  else
  { GlobalVariables tempGlobalVars;
{}//    this->theParser.theHmm.MakeG2InB3(this->theParser, tempGlobalVars);
  }
  theReport.Report("Loading param subchambers cone form... ");
  this->theMaxComputation.ReadFromFile(input, theGlobalVariables, this->UpperLimitChambersForDebugPurposes);
  theReport.Report("Loading more pieces of data... ");
  this->GmodKnegativeWeightS.ReadFromFile(input, theGlobalVariables);
  this->GmodKNegWeightsBasisChanged.ReadFromFile(input, theGlobalVariables);
  this->theLinearOperators.ReadFromFile(input, theGlobalVariables);
  this->theLinearOperatorsExtended.ReadFromFile(input, theGlobalVariables);
  this->PreimageWeylChamberLargerAlgebra.ReadFromFile(input, theGlobalVariables);
  this->PreimageWeylChamberSmallerAlgebra.ReadFromFile(input, theGlobalVariables);
  this->WeylChamberSmallerAlgebra.ReadFromFile(input, theGlobalVariables);
  this->theQPsNonSubstituted.ReadFromFile(input, theGlobalVariables);
  XML::ReadThroughFirstOpenTag(input, numReadWords, "QPsSubbed");
  this->theQPsSubstituted.ReadFromFile(input, theGlobalVariables);
  XML::ReadEverythingPassedTagOpenUntilTagClose(input, numReadWords, "QPsSubbed");
  theReport.Report("Loading multiplicities... ");
  XML::ReadThroughFirstOpenTag(input, numReadWords, "theMultiplicities");
  this->theMultiplicities.ReadFromFile(input, theGlobalVariables, this->UpperLimitChambersForDebugPurposes);
  XML::ReadEverythingPassedTagOpenUntilTagClose(input, numReadWords, "theMultiplicities");
  this->theCoeffs.ReadFromFile(input);
  this->theTranslationS.ReadFromFile(input, theGlobalVariables);
  this->theTranslationsProjectedBasisChanged.ReadFromFile(input, theGlobalVariables);
  theReport.Report("Loading partial fractions... ");
//  this->thePfs.ReadFromFile(input, theGlobalVariables);
  theReport.Report("Loading projectivized param complex... ");
  this->projectivizedParamComplex.ReadFromFile(input, theGlobalVariables, -1);
  theReport.Report("Loading the complex... ");
  this->smallerAlgebraChamber.ReadFromFile(input, theGlobalVariables, this->UpperLimitChambersForDebugPurposes);
  this->projectivizedChambeR.ReadFromFile(input, theGlobalVariables, this->UpperLimitChambersForDebugPurposes);
  theReport.Report("Loading complete... ");
  return true;
}

void GeneralizedVermaModuleCharacters::ReadFromDefaultFile(GlobalVariables* theGlobalVariables)
{ std::fstream input;
  if (!FileOperations::FileExists( "/home/todor/math/vectorpartition/trunk/GenVermaComputation.txt"))
  { this->computationPhase=0;
    return;
  }
  FileOperations::OpenFileCreateIfNotPresent(input, "/home/todor/math/vectorpartition/trunk/GenVermaComputation.txt", false, false, false);
  this->ReadFromFile(input, theGlobalVariables);
  input.close();
}

void GeneralizedVermaModuleCharacters::WriteToDefaultFile(GlobalVariables* theGlobalVariables)
{ std::fstream output;
  FileOperations::OpenFileCreateIfNotPresent(output, "/home/todor/math/vectorpartition/trunk/GenVermaComputation.txt", false, true, false);
  this->WriteToFile(output, theGlobalVariables);
}

std::string GeneralizedVermaModuleCharacters::ElementToStringMultiplicitiesReport(GlobalVariables& theGlobalVariables)
{ if (this->theMultiplicities.size!= this->projectivizedChambeR.size)
    crash << crash;
  std::stringstream out;
  FormatExpressions theFormat;
  theFormat.polyAlphabeT.SetSize(5);
  theFormat.polyAlphabeT[0]="x_1";
  theFormat.polyAlphabeT[1]="x_2";
  theFormat.polyAlphabeT[2]="y_1";
  theFormat.polyAlphabeT[3]="y_2";
  theFormat.polyAlphabeT[4]="y_3";
  out << "Number chambers: " << projectivizedChambeR.size << " of them " << this->numNonZeroMults << " non-zero.";
  int numInequalities=0;
  for (int i=0; i<this->projectivizedChambeR.size; i++)
  { numInequalities+=this->projectivizedChambeR[i].Normals.size;
  }
  out << "\nNumber of inequalities: " << numInequalities;
  if (!this->ParabolicLeviPartRootSpacesZeroStandsForSelected.CardinalitySelection==0)
    out << this->PrepareReport(theGlobalVariables);
  return out.str();
}

void GeneralizedVermaModuleCharacters::GetSubFromNonParamArray
(Matrix<Rational>& output, Vector<Rational>& outputTranslation, Vectors<Rational>& NonParams, int numParams)
{ //reminder: the very last variable comes from the projectivization and contributes to the translation only!
  int numNonParams=NonParams.size;
  output.init(numParams+numNonParams-1, numParams-1);
  outputTranslation.MakeZero(numParams+numNonParams-1);
  output.MakeZero();
  for (int l=0; l<numNonParams; l++)
  { for (int k=0; k<numParams-1; k++)
      output.elements[l][k]= NonParams[l][k];
    outputTranslation.TheObjects[l]=*NonParams.TheObjects[l].LastObject();
  }
  for (int l=0; l<numParams-1; l++)
    output.elements[l+numNonParams][l]= 1;
}

void GeneralizedVermaModuleCharacters::GetProjection(int indexOperator, const Vector<Rational>& input, Vector<Rational>& output)
{ Matrix<Rational>& currentExtendedOperator=this->theLinearOperatorsExtended[indexOperator];
  Vector<Rational>& currentTranslation=this->theTranslationsProjectedBasisChanged[indexOperator];
  if (input.LastObject()->IsEqualToZero())
    crash << crash;
  output=input;
  Rational tempRat=*output.LastObject();
  output/=tempRat;
  output.size--;
  currentExtendedOperator.ActOnVectorColumn(output);
  output+=currentTranslation;
}

void GeneralizedVermaModuleCharacters::GetSubFromIndex
(PolynomialSubstitution<Rational>& outputSub, Matrix<LargeInt>& outputMat, LargeIntUnsigned& outputDen, int theIndex)
{ Matrix<Rational> & theOperator=this->theLinearOperators.TheObjects[theIndex];
  int dimLargerAlgebra=theOperator.NumCols;
  int dimSmallerAlgebra=theOperator.NumRows;
  Vector<Rational>& theTranslation=this->theTranslationS[theIndex];
  Matrix<Rational> tempMat;
  tempMat.init(dimLargerAlgebra+dimSmallerAlgebra+1, dimSmallerAlgebra);
  tempMat.MakeZero();
  for (int j=0; j<dimSmallerAlgebra; j++)
  { tempMat.elements[j][j]=1;
    for (int i=0; i<dimLargerAlgebra; i++)
      tempMat.elements[i][j]-=theOperator.elements[j][i];
    tempMat.elements[dimLargerAlgebra+dimSmallerAlgebra][j]=-theTranslation.TheObjects[j];
  }
  tempMat.GetMatrixIntWithDen(outputMat, outputDen);
  outputSub.MakeSubFromMatrixIntAndDen(outputMat, outputDen);
}

void GeneralizedVermaModuleCharacters::TransformToWeylProjective(int indexOperator, Vector<Rational>& startingNormal, Vector<Rational>& outputNormal)
{ Matrix<Rational> theOperatorExtended=this->theLinearOperatorsExtended[indexOperator];
  Vector<Rational>& theTranslation=this->theTranslationsProjectedBasisChanged[indexOperator];
  //the goddamned sign in front of theTranslation is now checked: it should be + and not -
  Rational theConst;
  startingNormal.ScalarEuclidean(this->NonIntegralOriginModificationBasisChanged+theTranslation, theConst);
  theOperatorExtended.Transpose();
  outputNormal=startingNormal;
  theOperatorExtended.ActOnVectorColumn(outputNormal);
  outputNormal.SetSize(outputNormal.size+1);
  *outputNormal.LastObject()=-theConst;
}

void GeneralizedVermaModuleCharacters::TransformToWeylProjectiveStep1(GlobalVariables& theGlobalVariables)
{ this->smallerAlgebraChamber.InitFromDirectionsAndRefine(this->GmodKNegWeightsBasisChanged, theGlobalVariables);
  ProgressReport theReport1(&theGlobalVariables);
  ProgressReport theReport2(&theGlobalVariables);
  theReport1.Report(this->smallerAlgebraChamber.ToString(false, false));
  this->log << "Directions for making the chamber basis changed: " << this->GmodKNegWeightsBasisChanged.ToString() << "\n Resulting chamber before projectivization:\n "
  << this->smallerAlgebraChamber.ToString(false, false);
  theReport2.Report(this->log.str());
}

void GeneralizedVermaModuleCharacters::TransformToWeylProjectiveStep2(GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  ConeComplex projectivizedChamberFinal;
  Cone currentProjectiveCone;
  Vectors<Rational> tempRoots, extraWeylChamberWalls;
  Vector<Rational> tempRoot, wallAtInfinity, wallToSliceWith;
  ProgressReport theReport(&theGlobalVariables);
//  int dimSmallerAlgebra=this->theLinearOperators.TheObjects[0].NumRows;
//  int dimLargerAlgebra=this->theLinearOperators.TheObjects[0].NumCols;
//  int dimFinal=dimSmallerAlgebra+dimLargerAlgebra+1;
  projectivizedChamberFinal.init();
  for (int i=0; i<this->smallerAlgebraChamber.size; i++)
  { Cone& currentAffineCone=this->smallerAlgebraChamber.TheObjects[i];
    tempRoots.SetSize(currentAffineCone.Normals.size);
    for (int j=0; j<currentAffineCone.Normals.size; j++)
      this->TransformToWeylProjective(0, currentAffineCone.Normals[j], tempRoots[j]);
    tempRoots.AddListOnTop(this->PreimageWeylChamberLargerAlgebra.Normals);
    theReport.Report(tempRoots.ToString());
    currentProjectiveCone.CreateFromNormals(tempRoots, &theGlobalVariables);
    projectivizedChamberFinal.AddNonRefinedChamberOnTopNoRepetition(currentProjectiveCone, theGlobalVariables);
  }
  for (int i=0; i<this->PreimageWeylChamberSmallerAlgebra.Normals.size; i++)
    projectivizedChamberFinal.splittingNormals.AddOnTop(this->PreimageWeylChamberSmallerAlgebra.Normals[i]);
  out << "projectivized chamber before chopping non-dominant part:\n" << projectivizedChamberFinal.ToString(false, false);
  projectivizedChamberFinal.Refine(theGlobalVariables);
  out << "Refined projectivized chamber before chopping non-dominant part:\n" << projectivizedChamberFinal.ToString(false, false);
  for (int i=0; i<projectivizedChamberFinal.size; i++)
  { const Cone& currentCone=projectivizedChamberFinal[i];
    bool isNonDominant=false;
    for (int j=0; j<this->PreimageWeylChamberSmallerAlgebra.Normals.size; j++)
      if (currentCone.GetInternalPoint().ScalarEuclidean(this->PreimageWeylChamberSmallerAlgebra.Normals[j]).IsNegative())
      { isNonDominant=true;
        break;
      }
    if (isNonDominant)
    { projectivizedChamberFinal.PopChamberSwapWithLast(i);
      i--;
    }
  }
  theReport.Report(out.str());

  projectivizedChamberFinal.indexLowestNonRefinedChamber=0;
  this->projectivizedChambeR=projectivizedChamberFinal;
  for (int k=1; k<this->theLinearOperators.size; k++)
    for (int i=0; i<this->smallerAlgebraChamber.size; i++)
      for (int j=0; j<this->smallerAlgebraChamber[i].Normals.size; j++)
      { this->TransformToWeylProjective(k, this->smallerAlgebraChamber[i].Normals[j], wallToSliceWith);
        wallToSliceWith.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
        this->projectivizedChambeR.splittingNormals.AddOnTopNoRepetition(wallToSliceWith);
      }
  out << "projectivized chamber chopped non-dominant part:\n"  << this->projectivizedChambeR.ToString(false, false);
  theReport.Report(out.str());
}
