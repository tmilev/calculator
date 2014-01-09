//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfImplementationHeaderFiniteGroupsIncluded
#define vpfImplementationHeaderFiniteGroupsIncluded

#include "vpfHeader2Math3_FiniteGroups.h"
static ProjectInformationInstance ProjectInfovpfImplementationHeaderFiniteGroups(__FILE__, "Implementation header, finite groups. ");

template <class coefficient>
std::string FinitelyGeneratedMatrixMonoid<coefficient>::ToString(FormatExpressions* theFormat)const
{ std::stringstream out;
  out << "Number of generators: " << this->theGenerators.size;
  out << "<br>Number of elements: " << this->theElements.size;
  out << "<br>The elements follow.";
  int numEltstoDisplay=this->theElements.size;
  if (numEltstoDisplay>100)
  { out << "<b>Displaying only the first " << 100 << " elements.</b>";
    numEltstoDisplay=100;
  }
  for (int i=0; i<numEltstoDisplay; i++)
    out << "<br>" << this->theElements[i].ToStringMatForm(theFormat);
  return out.str();
}

template <typename elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::ComputeAllElements(int MaxElements, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("Subgroup::ComputeAllElements");
  this->InitGenerators();
  if (this->generators.size==0)
    crash << "Groups with zero generators are not allowed: if you wanted to create a trivial group, "
    << " trivial groups are assumed to have a generator (the identity). " << crash;
  this->theElements.Clear();
  elementSomeGroup currentElement;
  currentElement.MakeID(this->generators[0]);
  this->theElements.AddOnTop(currentElement);
  ProgressReport theReport(theGlobalVariables);
  //Check the generators have no repetitions:
  for (int j=0; j<this->theElements.size; j++)
    for(int i=0; i<this->generators.size; i++)
    { currentElement=this->generators[i]*this->theElements[j];
      this->theElements.AddOnTopNoRepetition(currentElement);
      if (theGlobalVariables!=0)
        if (this->theElements.size%100==0)
        { std::stringstream reportStream;
          LargeInt sizeByFla=this->GetGroupSizeByFormula();
          reportStream << "So far, generated " << this->theElements.size << " elements";
          if (sizeByFla>0)
            reportStream << " out of " << sizeByFla.ToString();
          reportStream << ".";
          theReport.Report(reportStream.str());
        }
      if (MaxElements>0)
        if (this->theElements.size>MaxElements)
          return false;
    }
  if (theGlobalVariables!=0)
  { std::stringstream reportStream;
    reportStream << "Generated group with a total of " << this->theElements.size << " elements. ";
    theReport.Report(reportStream.str());
  }
  this->sizePrivate=this->theElements.size;
  this->flagAllElementsAreComputed=true;
  return true;
}

template <class templateWeylGroup>
void ElementWeylGroup<templateWeylGroup>::Conjugate
(const ElementWeylGroup& elementWeConjugateBy, const ElementWeylGroup& inputToBeConjugated, ElementWeylGroup& output)
{ if (elementWeConjugateBy.owner!=inputToBeConjugated.owner)
    crash << "Attempting to multiply elements belonging to different Weyl groups." << crash;
  if (&elementWeConjugateBy==&output || &inputToBeConjugated==&output)
  { ElementWeylGroup copyActingElt=elementWeConjugateBy;
    ElementWeylGroup copyToBeConjugated=inputToBeConjugated;
    ElementWeylGroup<templateWeylGroup>::Conjugate(copyActingElt, copyToBeConjugated, output);
    return;
  }
  output.owner=elementWeConjugateBy.owner;
  output.generatorsLastAppliedFirst.ReservE(elementWeConjugateBy.generatorsLastAppliedFirst.size*2+inputToBeConjugated.generatorsLastAppliedFirst.size);
  output.generatorsLastAppliedFirst=elementWeConjugateBy.generatorsLastAppliedFirst;
  output.generatorsLastAppliedFirst.AddListOnTop(inputToBeConjugated.generatorsLastAppliedFirst);
  for (int i=elementWeConjugateBy.generatorsLastAppliedFirst.size-1; i>=0; i--)
    output.generatorsLastAppliedFirst.AddOnTop(elementWeConjugateBy.generatorsLastAppliedFirst[i]);
  output.MakeCanonical();
}

template <class templateWeylGroup>
void ElementWeylGroup<templateWeylGroup>::operator*=(const ElementWeylGroup<WeylGroup>& other)
{ if (this->owner!=other.owner)
    crash << "This is a programming error: attempting to multiply elements belonging to different Weyl groups. " << crash;
  if (&other==this)
  { ElementWeylGroup<WeylGroup> otherCopy=other;
    (*this)*=otherCopy;
    return;
  }
  this->generatorsLastAppliedFirst.AddListOnTop(other.generatorsLastAppliedFirst);
  this->MakeCanonical();
}

template <class templateWeylGroup>
Vector<Rational> ElementWeylGroup<templateWeylGroup>::operator*(const Vector<Rational>& v) const
{ Vector<Rational> out = v;
  this->owner->ActOn(*this,out);
  return out;
}

template <class templateWeylGroup>
std::string ElementWeylGroup<templateWeylGroup>::ToString(int NumSimpleGens, FormatExpressions* theFormat, List<int>* DisplayIndicesOfSimpleRoots)const
{ MacroRegisterFunctionWithName("ElementWeylGroup::ToString");
  if (this->generatorsLastAppliedFirst.size==0)
    return "id";
  std::string outerAutoLetter= "a";
  std::stringstream out;
  for (int i=0; i<this->generatorsLastAppliedFirst.size; i++)
    if (NumSimpleGens<0 || this->generatorsLastAppliedFirst[i]<NumSimpleGens)
    { out << "s_{";
      if (DisplayIndicesOfSimpleRoots==0)
        out << this->generatorsLastAppliedFirst[i]+1;
      else
        out << (*DisplayIndicesOfSimpleRoots)[this->generatorsLastAppliedFirst[i]];
      out << "}";
    } else
      out << outerAutoLetter << "_{" << this->generatorsLastAppliedFirst[i]-NumSimpleGens+1 << "}";
  return out.str();
}

template <class templateWeylGroup>
unsigned int ElementWeylGroup<templateWeylGroup>::HashFunction() const
{ int top = MathRoutines::Minimum(this->generatorsLastAppliedFirst.size, ::SomeRandomPrimesSize);
  unsigned int result =0;
  for (int i=0; i<top; i++)
    result+=this->generatorsLastAppliedFirst[i]*::SomeRandomPrimes[i];
  return result;
}

template <class templateWeylGroup>
bool ElementWeylGroup<templateWeylGroup>::operator>(const ElementWeylGroup<templateWeylGroup>& other)const
{ if (this->owner!=other.owner)
    crash << "Comparing elements of different weyl groups. " << crash;
  return this->generatorsLastAppliedFirst>other.generatorsLastAppliedFirst;
}

template <class templateWeylGroup>
void ElementWeylGroup<templateWeylGroup>::MakeID(templateWeylGroup& inputWeyl)
{ this->owner=&inputWeyl;
  this->generatorsLastAppliedFirst.SetSize(0);
}

template <class templateWeylGroup>
void ElementWeylGroup<templateWeylGroup>::MakeID(const ElementWeylGroup& initializeFrom)
{ this->MakeID(*initializeFrom.owner);
}

template <class templateWeylGroup>
void ElementWeylGroup<templateWeylGroup>::MakeRootReflection
(const Vector<Rational>& mustBeRoot, WeylGroup& inputWeyl)
{ *this=inputWeyl.GetRootReflection(inputWeyl.RootSystem.GetIndexIMustContainTheObject(mustBeRoot));
}

template <class templateWeylGroup>
void ElementWeylGroup<templateWeylGroup>::MakeSimpleReflection(int simpleRootIndex, WeylGroup& inputWeyl)
{ this->owner=&inputWeyl;
  this->generatorsLastAppliedFirst.SetSize(1);
  this->generatorsLastAppliedFirst[0]=simpleRootIndex;
}

template <class templateWeylGroup>
void ElementWeylGroup<templateWeylGroup>::MakeFromRhoImage(const Vector<Rational>& inputRhoImage, WeylGroup& inputWeyl)
{ this->owner=&inputWeyl;
  int theRank=this->owner->GetDim();
  this->generatorsLastAppliedFirst.SetSize(0);
  //std::cout << theVector.ToString();
  Vector<Rational> theVector=inputRhoImage;
  while (theVector!=this->owner->rho)
    for (int i=0; i<theRank; i++)
      if (this->owner->GetScalarProdSimpleRoot(theVector, i)<0)
      { this->owner->SimpleReflection(i, theVector);
        //std::cout << "--"  << i+1 << "-- > " << theVector.ToString() ;
        this->generatorsLastAppliedFirst.AddOnTop(i);
        break;
      }
//  std::cout << ", finally: " << this->ToString();
}

template <class templateWeylGroup>
void ElementWeylGroup<templateWeylGroup>::MakeCanonical()
{ //std::cout << "making " << this->ToString() << " canonical: ";
  this->CheckInitialization();
  if (this->owner->rho.size==0)
    this->owner->ComputeRho(false);
  Vector<Rational> theVector=this->owner->rho;
  this->owner->ActOn(*this, theVector);
  this->MakeFromRhoImage(theVector, *this->owner);
}

template <class templateWeylGroup>
bool ElementWeylGroup<templateWeylGroup>::HasDifferentConjugacyInvariantsFrom
(const ElementWeylGroup<templateWeylGroup>& right)const
{ MacroRegisterFunctionWithName("ElementWeylGroup::HasDifferentConjugacyInvariantsFrom");
  //std::cout << "<br>Comparing invariants of " << this->ToString() << " and " << right.ToString();
  if ((this->generatorsLastAppliedFirst.size%2 )!=(right.generatorsLastAppliedFirst.size%2))
    return true;
  if (*this==right) //just in case
    return false;
  this->CheckInitialization();
  //std::cout << " the sign is the same... ";
  Polynomial<Rational> leftCharPoly, rightCharPoly;
  this->GetCharacteristicPolyStandardRepresentation(leftCharPoly);
  right.GetCharacteristicPolyStandardRepresentation(rightCharPoly);
  if(leftCharPoly!=rightCharPoly)
    return true;
  //std::cout << " the char polys are the same... ";
  VectorSparse<Rational> leftCycleStructure, rightCycleStructure;
  this->GetCycleStructure(leftCycleStructure);
  right.GetCycleStructure(rightCycleStructure);
  if (leftCycleStructure!=rightCycleStructure)
    return true;
  //std::cout << " the cycle structures are the same... the elements are POSSIBLY conjugate.";
  return false;
}

template <class templateWeylGroup>
std::string ElementWeylGroup<templateWeylGroup>::ToStringInvariants(FormatExpressions* theFormat)const
{ MacroRegisterFunctionWithName("ElementWeylGroup::GetCycleStructure");
  VectorSparse<Rational> theCycleStructure;
  this->GetCycleStructure(theCycleStructure);
  FormatExpressions cycleLetterFormat;
  cycleLetterFormat.polyDefaultLetter="c";
  std::stringstream out;
  out << "Cycle structure: " << theCycleStructure.ToString(&cycleLetterFormat) << ". ";
  return out.str();
}

template <class templateWeylGroup>
void ElementWeylGroup<templateWeylGroup>::GetCycleStructure
(VectorSparse<Rational>& outputIndexIsCycleSizeCoordinateIsCycleMult)const
{ MacroRegisterFunctionWithName("ElementWeylGroup::GetCycleStructure");
  this->CheckInitialization();
  outputIndexIsCycleSizeCoordinateIsCycleMult.MakeZero();
  List<bool> Explored;
  HashedList<Vector<Rational> >& theRootSystem=this->owner->RootSystem;
  Explored.initFillInObject(theRootSystem.size, false);
  Vector<Rational> currentRoot;
  for (int i=0; i<Explored.size; i++)
    if (!Explored[i])
    { int currentCycleSize=1;
      currentRoot=theRootSystem[i];
      for (this->owner->ActOn(*this, currentRoot); currentRoot!=theRootSystem[i]; this->owner->ActOn(*this, currentRoot))
      { currentCycleSize++;
        Explored[theRootSystem.GetIndex(currentRoot)]=true;
      }
      outputIndexIsCycleSizeCoordinateIsCycleMult.AddMonomial(MonomialVector(currentCycleSize), 1);
    }
}

template <class templateWeylGroup>
void ElementWeylGroup<templateWeylGroup>::GetCharacteristicPolyStandardRepresentation(Polynomial<Rational>& output)const
{ MacroRegisterFunctionWithName("ElementWeylGroup::GetCharacteristicPolyStandardRepresentation");
  this->CheckInitialization();
  Matrix<Rational> standardRepMat;
  this->owner->GetMatrixStandardRep(*this, standardRepMat);
  output.AssignCharPoly(standardRepMat);
}

template <class templateWeylGroup>
ElementWeylGroup<WeylGroup> ElementWeylGroup<templateWeylGroup>::Inverse() const
{ ElementWeylGroup<WeylGroup> out = *this;
  out.generatorsLastAppliedFirst.ReverseOrderElements();
  out.MakeCanonical();
  return out;
}

template <class elementGroup, class elementRepresentation>
void OrbitIterator<elementGroup, elementRepresentation>::init
  (const List<elementGroup>& inputGenerators, const elementRepresentation& inputElement, OrbitIterator::GroupAction inputGroupAction)
{ this->reset();
  this->theGroupGeneratingElements=inputGenerators;
  this->theGroupAction=inputGroupAction;
  this->currentLayer->Clear();
  this->currentLayer->AddOnTop(inputElement);
  this->indexCurrentElement=0;
  this->previousLayer->Clear();
  this->nextLayer->Clear();
}

template <class elementGroup, class elementRepresentation>
const elementRepresentation& OrbitIterator<elementGroup, elementRepresentation>::GetCurrentElement()
{ return (*this->currentLayer)[this->indexCurrentElement];
}

template <class elementGroup, class elementRepresentation>
bool OrbitIterator<elementGroup, elementRepresentation>::IncrementReturnFalseIfPastLast()
{ MacroRegisterFunctionWithName("OrbitIterator::IncrementReturnFalseIfPastLast");
  if (this->theGroupGeneratingElements.size==0)
    return false;
  for (int i=0; i<this->theGroupGeneratingElements.size; i++)
  { this->theGroupAction
    (this->theGroupGeneratingElements[i], (*this->currentLayer)[this->indexCurrentElement],
     this->bufferRepresentationElement);
    if (!this->previousLayer->Contains(this->bufferRepresentationElement) &&
        !this->currentLayer->Contains(this->bufferRepresentationElement))
      this->nextLayer->AddOnTopNoRepetition(this->bufferRepresentationElement);
  }
  this->indexCurrentElement++;
  if (this->indexCurrentElement<this->currentLayer->size)
    return true;
  if (this->nextLayer->size==0)
    return false;
  HashedList<elementRepresentation>* layerPtr=this->previousLayer;
  this->previousLayer=this->currentLayer;
  this->currentLayer=this->nextLayer;
  this->nextLayer=layerPtr;
  this->nextLayer->Clear();
  this->indexCurrentElement=0;
  return true;
}

template <class elementSomeGroup>
void FiniteGroup<elementSomeGroup>::init()
{ this->generators.SetSize(0);
  this->conjugacyClasseS.SetSize(0);
  this->theElements.Clear();
  this->flagAllElementsAreComputed=false;
  this->flagCCsComputed=false;
  this->flagCCRepresentativesComputed=false;
  this->flagCharPolysAreComputed=false;
  this->flagGeneratorsConjugacyClassesComputed=false;
  this->sizePrivate=0;
}

template <typename somegroup, class elementSomeGroup>
void Subgroup<somegroup, elementSomeGroup>::initFromGroupAndGenerators(somegroup& inputGroup, const List<elementSomeGroup>& inputGenerators)
{ MacroRegisterFunctionWithName("Subgroup::initFromGroupAndGenerators");
  if (&inputGenerators==&this->generators)//<-handle naughty programmers
  { List<ElementWeylGroup<somegroup> > inputGeneratorsCopy=inputGenerators;
    this->initFromGroupAndGenerators(inputGroup, inputGeneratorsCopy);
    return;
  }
  this->parent = &inputGroup;
  this->generators.SetSize(0);
  this->generators.AddOnTopNoRepetition(inputGenerators);//<- we have eliminated all repeating generators
  //(there shouldn't be any, but knowing I am one of the programmers...)
}

template <typename somegroup, class elementSomeGroup>
bool Subgroup<somegroup, elementSomeGroup>::CheckInitialization()
{ if (this->parent==0)
    crash << "This is a programming error: subgroup not initialized when it should be";
  return true;
}

template <class elementSomeGroup>
void FiniteGroup<elementSomeGroup>::GetSignCharacter(Vector<Rational>& outputCharacter)
{ if(!this->flagCCsComputed)
    this->ComputeCCSizesAndRepresentatives(0);
  outputCharacter.SetSize(this->ConjugacyClassCount());
  for(int i=0; i<this->ConjugacyClassCount(); i++)
    outputCharacter[i]=this->conjugacyClasseS[i].representative.Sign();
}

template <typename somegroup, class elementSomeGroup>
void Subgroup<somegroup, elementSomeGroup>::init()
{ this->parent=0;
  this->ccRepresentativesPreimages.SetSize(0);
  this->generatorPreimages.SetSize(0);
  this->::FiniteGroup<elementSomeGroup>::init();
}

template <typename somegroup, class elementSomeGroup>
Subgroup<somegroup, elementSomeGroup>::Subgroup()
{ this->init();
}

template <class elementSomeGroup>
template <typename coefficient>
coefficient FiniteGroup<elementSomeGroup>::GetHermitianProduct
(const Vector<coefficient>& X1, const Vector<coefficient>& X2) const
{ coefficient acc = 0;
  for(int i=0; i<X1.size; i++)
  { acc += X1[i].GetComplexConjugate() * X2[i] * this->conjugacyClasseS[i].size;
    if (this->conjugacyClasseS[i].size==0)
      crash << "Error: conjugacy class size is zero." << crash;
  }
  return acc / this->size();
}

template <class elementSomeGroup>
LargeInt FiniteGroup<elementSomeGroup>::size()const
{ if (this->sizePrivate<=0)
    crash << "Requesting size of group whose size is not computed. " << crash;
  return this->sizePrivate;
}

template <class elementSomeGroup>
int FiniteGroup<elementSomeGroup>::ConjugacyClassCount()const
{ if (!this->flagCCRepresentativesComputed)
    crash << "Requesting conjugacy class count but conjugacy class representatives are not computed." << crash;
  return this->conjugacyClasseS.size;
}

template <typename somegroup, class elementSomeGroup>
void Subgroup<somegroup, elementSomeGroup>::ComputeCCRepresentativesPreimages(GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("Subgroup::ComputeCCRepresentativesPreimages");
  this->ccRepresentativesPreimages.SetSize(this->ConjugacyClassCount());
  if (this->generators.size==0)
    crash << "At this computation the group must have initialized generators. " << crash;
  if (this->parent->generators.size==0)
    crash << "Parent group must have initialized generators. " << crash;
  for(int i=0; i<this->ConjugacyClassCount(); i++)
  { bool notFound=true;
    elementSomeGroup& g=this->conjugacyClasseS[i].representative;
    for(int ci=0; notFound && ci<this->parent->ConjugacyClassCount(); ci++)
      if(this->parent->AreConjugate(g, this->parent->conjugacyClasseS[ci].representative))
      { this->ccRepresentativesPreimages[i] = ci;
        notFound=false;
      } //else
        //std::cout << "<hr>" << g.ToString() << " and " << this->parent->conjugacyClasseS[ci].representative.ToString() << " are not conjugate. ";
    if (notFound)
      crash << "Programming error: couldn't find preimage of the subgroup conjugacy class representative "
      << g.ToString() << crash;
  }
}

template <class elementSomeGroup>
std::string FiniteGroup<elementSomeGroup>::ToStringElements(FormatExpressions* theFormat)const
{ MacroRegisterFunctionWithName("FiniteGroup::ToStringElements");
  if (!this->flagAllElementsAreComputed)
    return "";
  std::stringstream out;
  out << "<br>Elements of the group(" << this->theElements.size << " total):\n ";
  if (this->theElements.size<=100)
    for (int i=0; i<this->theElements.size; i++)
      out << i << ". " << this->theElements[i].ToString() << "\n";
  else
    out << "... too many, not displaying. ";
  return out.str();
}

template <class elementSomeGroup>
std::string FiniteGroup<elementSomeGroup>::ToStringConjugacyClasses(FormatExpressions* theFormat)const
{ MacroRegisterFunctionWithName("Subgroup::ToStringConjugacyClasses");
  std::stringstream out;
  out << "<br>Size: " << this->size().ToString() << "\n";
  FormatExpressions charPolyFormat;
  charPolyFormat.polyAlphabeT.SetSize(1);
  charPolyFormat.polyAlphabeT[0]="q";
  //  out <<"Number of Vectors<Rational>: "<<this->RootSystem.size<<"\n
  if (this->ConjugacyClassCount()>0)
  { out << "<br>" << this->ConjugacyClassCount() << " conjugacy classes total.\n";
    for (int i=0; i<this->conjugacyClasseS.size; i++)
    { out << "<hr>Conjugacy class " << i+1 << ": ";
      if (this->conjugacyClasseS[i].flagRepresentativeComputed)
      { out << " represented by " << this->conjugacyClasseS[i].representative.ToString(theFormat) << ". ";
        out << this->conjugacyClasseS[i].representative.ToStringInvariants(theFormat);
      } else
        out << " representative not computed. ";
      out << "Class size: " << this->conjugacyClasseS[i].size.ToString() << ".\n<br>\n";
      if (this->flagCharPolysAreComputed)
        if (i<this->CCsStandardRepCharPolys.size)
        { out << "Characteristic poly standard representation: "
          << this->CCsStandardRepCharPolys[i].ToString(&charPolyFormat);
          const List<int>& currentHashList=
          this->CCsStandardRepCharPolys.GetHashArray
          (this->CCsStandardRepCharPolys.GetHash(this->CCsStandardRepCharPolys[i]));
          int numClassesSameCharPoly=0;
          for (int j=0; j<currentHashList.size; j++)
            if (this->CCsStandardRepCharPolys[currentHashList[j]]==this->CCsStandardRepCharPolys[i])
              numClassesSameCharPoly++;
          if (numClassesSameCharPoly>1)
          { out << " The characteristic polynomial is the same as that of " << numClassesSameCharPoly
            << " conjugacy classes, numbers: ";
            for (int j=0; j<currentHashList.size; j++)
              if (this->CCsStandardRepCharPolys[currentHashList[j]]==this->CCsStandardRepCharPolys[i])
                out << currentHashList[j]+1 << (j==currentHashList.size-1 ? "" : ", ");
          }
        }
      if (!this->conjugacyClasseS[i].flagElementsComputed)
        continue;
      out << " The elements of the class are: ";
      int numEltsToDisplay=this->conjugacyClasseS[i].theElements.size;
      if (this->conjugacyClasseS[i].theElements.size>10)
      { out << " too many, displaying the first 10 elements only: ";
        numEltsToDisplay=10;
      }
      for (int j=0; j<numEltsToDisplay; j++)
      { out << this->conjugacyClasseS[i].theElements[j].ToString(theFormat);
        if (j!=numEltsToDisplay-1)
          out << ", ";
      }
      out << ". ";
    }
  }
  return out.str();
}

template <class elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::CheckInitialization()const
{ this->CheckConsistency();
  if (this->generators.size==0)
    crash << "Error: group has 0 generators, which is not allowed. If you want to use the trivial "
    << "group, you are still supposed to put the identity element in the group generators. " << crash;
  return true;
}

template <class elementSomeGroup>
void FiniteGroup<elementSomeGroup>::ComputeGeneratorsConjugacyClasses
  (GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("FiniteGroup::ComputeGeneratorsConjugacyClasses");
  if (this->flagGeneratorsConjugacyClassesComputed)
    return;
  this->unionGeneratorsCC.Clear();
  this->unionGeneratorsCC.AddOnTopNoRepetition(this->generators);
  elementSomeGroup currentElt;
  for (int i=0; i<this->unionGeneratorsCC.size; i++)
    for (int j=0; j<this->generators.size; j++)
    { elementSomeGroup::Conjugate(this->generators[j], this->unionGeneratorsCC[i], currentElt);
      this->unionGeneratorsCC.AddOnTopNoRepetition(currentElt);
    }
  this->flagGeneratorsConjugacyClassesComputed=true;
}

template <class elementSomeGroup>
void FiniteGroup<elementSomeGroup>::ComputeCCSizeOrCCFromRepresentative
(ConjugacyClass& inputOutputClass, bool storeCC, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("FiniteGroup::ComputeCCSizesFromCCRepresentatives");
  OrbitIterator<elementSomeGroup, elementSomeGroup> theOrbitIterator;
  theOrbitIterator.init(this->generators, inputOutputClass.representative, elementSomeGroup::Conjugate);
  inputOutputClass.size=1;
  if (storeCC)
  { inputOutputClass.theElements.SetSize(0);
    inputOutputClass.theElements.AddOnTop(inputOutputClass.representative);
  }
//  std::cout << "<br> Generating orbit: <br>Element 1: " << theOrbitIterator.GetCurrentElement().ToString();
//  Vector<Rational> randomVector, tempV;
//  randomVector.AssignString("(23,29,31,37,41,43)");
//  HashedList<Vector<Rational> > orbitRV;
//  tempV=randomVector;
//  theOrbitIterator.GetCurrentElement().ActOn(tempV);
//  orbitRV.AddOnTop(tempV);
  while (theOrbitIterator.IncrementReturnFalseIfPastLast())
  { inputOutputClass.size++;
//    std::cout << "<br>Element " << inputOutputClass.size.ToString() << ": "
//    << theOrbitIterator.GetCurrentElement().ToString();
//    tempV=randomVector;
//    theOrbitIterator.GetCurrentElement().ActOn(tempV);
//    if (orbitRV.Contains(tempV))
//    {
//      crash << "Element " << theOrbitIterator.GetCurrentElement().ToString()
//      << " is bad, has same action on " << randomVector.ToString() << " as " <<
//      inputOutputClass.theElements[orbitRV.GetIndex(tempV)].ToString() << crash
//      ;
//    }
//    orbitRV.AddOnTop(tempV);
    if (storeCC)
    { if (inputOutputClass.theElements.Contains(theOrbitIterator.GetCurrentElement()))
        crash << " !element " << theOrbitIterator.GetCurrentElement().ToString()
        << " already contained !" << crash;
      inputOutputClass.theElements.AddOnTop(theOrbitIterator.GetCurrentElement());
    }
  }
}

template <class elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::CheckConjugacyClassRepsMatchCCsizes(GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("FiniteGroup::CheckConjugacyClassRepsMatchCCsizes");
  LargeInt computedSize=0;
  for (int i=0; i<this->conjugacyClasseS.size; i++)
  { LargeInt oldCCsize=this->conjugacyClasseS[i].size;
    this->ComputeCCSizeOrCCFromRepresentative
    (this->conjugacyClasseS[i], true, theGlobalVariables);
    if (oldCCsize!=this->conjugacyClasseS[i].size)
      crash << "The precomputed size " << oldCCsize.ToString() << " of the class represented by " << this->conjugacyClasseS[i].representative.ToString()
      << " doesn't match actual class size which is: " << this->conjugacyClasseS[i].size.ToString()
      << crash;
    computedSize+=oldCCsize;
  }
  if (computedSize!=this->sizePrivate)
    crash << "Computed size " << computedSize.ToString() << " is different from recorded size "
    << sizePrivate.ToString() << crash;
  if (this->GetGroupSizeByFormula()>0)
    if (computedSize!=this->GetGroupSizeByFormula())
      crash << "Computed size is different from size dicated by formula which is: "
      << this->GetGroupSizeByFormula().ToString() << crash;
  return true;
}

template <class elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::RegisterCCclass
(const elementSomeGroup& theRepresentative, bool dontAddIfSameInvariants, GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("FiniteGroup::RegisterCCclass");
  ConjugacyClass theClass;
  theClass.representative=theRepresentative;
  Polynomial<Rational> theCharPoly;
  theClass.representative.GetCharacteristicPolyStandardRepresentation(theCharPoly);
  if (this->CCsStandardRepCharPolys.Contains(theCharPoly))
  { const List<int>& indicesPossibleConjugates=
    this->CCsStandardRepCharPolys.GetHashArray(this->CCsStandardRepCharPolys.GetHash(theCharPoly));
    for (int i=0; i<indicesPossibleConjugates.size; i++)
    { elementSomeGroup& otherRepresentative=this->conjugacyClasseS[indicesPossibleConjugates[i]].representative;
      if (!dontAddIfSameInvariants)
      { if (this->AreConjugate(theClass.representative, otherRepresentative))
          return false;
      } else
        if (!theClass.representative.HasDifferentConjugacyInvariantsFrom(otherRepresentative))
          return false;
    }
  }
  theClass.flagRepresentativeComputed=true;
  this->ComputeCCSizeOrCCFromRepresentative(theClass, false, theGlobalVariables);
  this->conjugacyClasseS.AddOnTop(theClass);
  this->CCsStandardRepCharPolys.AddOnTop(theCharPoly);
  this->sizePrivate+=theClass.size;
  return true;
}

template <class elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::ComputeCCRepresentatives
(GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("FiniteGroup::ComputeCCRepresentatives");
  //This algorithm is effective if the sum of the sizes of the conjugacy classes
  //of the generators is small.
  //The algorithm "hops" from conjugacy class representative to conjugacy representative
  //by multiplying the representatives by the each element of each conjugacy class of the generators.
  //We prove this algorithm is guaranteed to generate all conjugacy classes.
  //Let g be a generator, H a conjugacy class, and h \in H an arbitrary element of H.
  //Suppose gH intersects a conjugacy class F. We claim there exists a generator g and an
  //element x such that x g x^{-1} h\in F. Indeed, as gH intersects F, g x^{-1} h x\in F for some
  //element x, and therefore x g x^{-1} h =x(g x^{-1} h x) x^{-1}\in F, which proves our claim.
  //The preceding considerations show that
  //a representative of each conjugacy class containing an element from g H can be obtained
  //by applying the entire conjugacy class of g onto an arbitrary representative of H.
  //As we are using the conjugacy classes of a set of generators, this shows that the following
  //algorithm will generate a representative of each conjugacy class.

  //First we compute the generator's conjugacy classes:
  this->ComputeGeneratorsConjugacyClasses(theGlobalVariables);

  ProgressReport theReport(theGlobalVariables);
  elementSomeGroup currentElement;
  LargeInt groupSizeByFla=this->GetGroupSizeByFormula();
  this->flagCharPolysAreComputed=true;
  for (int phase=0; phase<2; phase++)//In phase 0 we try to add a new conjugacy class only if
    //the class has a new character polynomial. In phase 1 we try every single conjugacy class,
    //in case there are two non-conjugate elements with the same char poly.
    for (int i=0; i<this->conjugacyClasseS.size; i++)
      for (int j=0; j<this->unionGeneratorsCC.size; j++)
      { if (theGlobalVariables!=0)
        { std::stringstream reportStream;
          reportStream << "Exploring conjugacy class " << i+1
          << " out of " << this->conjugacyClasseS.size
          << " generator " << j+1 << " out of " << this->unionGeneratorsCC.size;
          theReport.Report(reportStream.str());
        }
        currentElement=this->conjugacyClasseS[i].representative*this->unionGeneratorsCC[j];
        this->RegisterCCclass(currentElement,(phase==0), theGlobalVariables);
        if (this->sizePrivate==groupSizeByFla)
          return true;
      }
  if (groupSizeByFla>0)
    if (this->sizePrivate!=groupSizeByFla)
      crash << "Something went very wrong: number of elements generated is not equal to group size by formula. " << crash;
  return true;
}

template <class elementSomeGroup>
void FiniteGroup<elementSomeGroup>::ComputeCCSizesAndRepresentatives(GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("FiniteGroup::ComputeCCSizesAndRepresentatives");
  if (this->flagCCRepresentativesComputed)
    return;
  this->InitGenerators();
  this->CheckInitialization();
  this->CCsStandardRepCharPolys.Clear();
  this->conjugacyClasseS.SetSize(0);
  this->sizePrivate=0;

  elementSomeGroup currentElt;
  currentElt.MakeID(this->generators[0]);
  this->RegisterCCclass(currentElt, false, theGlobalVariables);
  this->ComputeCCRepresentatives(theGlobalVariables);
  this->flagCCRepresentativesComputed=true;
}

template <class elementSomeGroup>
void FiniteGroup<elementSomeGroup>::ComputeCCfromCCindicesInAllElements(const List<List<int> >& ccIndices)
{ MacroRegisterFunctionWithName("FiniteGroup::ComputeCCfromCCindicesInAllElements");
  this->conjugacyClasseS.SetSize(ccIndices.size);
  for (int i=0; i<ccIndices.size; i++)
  { this->conjugacyClasseS[i].size= ccIndices[i].size;
    this->conjugacyClasseS[i].indicesEltsInOwner=ccIndices[i];
    this->conjugacyClasseS[i].theElements.SetSize(ccIndices[i].size);
    for (int j=0; j<ccIndices[i].size; j++)
      this->conjugacyClasseS[i].theElements[j]=this->theElements[ccIndices[i][j]];
    this->conjugacyClasseS[i].representative=this->theElements[ccIndices[i][0]];
    this->conjugacyClasseS[i].flagRepresentativeComputed=true;
    this->conjugacyClasseS[i].flagElementsComputed=true;
  }
  this->flagCCsComputed=true;
  this->flagCCRepresentativesComputed=true;
}

template <class elementSomeGroup>
void FiniteGroup<elementSomeGroup>::ComputeCCfromAllElements(GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("Subgroup::ComputeCCfromAllElements");
  this->ComputeAllElements(-1, theGlobalVariables);
  List<bool> Accounted;
  Accounted.initFillInObject(this->theElements.size, false);
  HashedList<int, MathRoutines::IntUnsignIdentity> theStack;
  theStack.SetExpectedSize(this->theElements.size);
  List<elementSomeGroup> inversesOfGenerators;
  inversesOfGenerators.SetSize(this->generators.size);
  for(int i=0; i<this->generators.size; i++)
    inversesOfGenerators[i] = this->generators[i].Inverse();
  elementSomeGroup currentElement;
  List<List<int> > ccIndices;
  ccIndices.ReservE(120);
  for(int i=0; i<this->theElements.size; i++)
    if (!Accounted[i])
    { theStack.Clear();
      theStack.AddOnTop(i);
      for (int j=0; j<theStack.size; j++)
        for (int k=0; k<this->generators.size; k++)
        { currentElement = inversesOfGenerators[k]*this->theElements[theStack[j]]*this->generators[k];
          int accountedIndex=this->theElements.GetIndexIMustContainTheObject(currentElement);
          theStack.AddOnTopNoRepetition(accountedIndex);
          Accounted[accountedIndex]=true;
        }
      ccIndices.AddOnTop(theStack);
      ccIndices.LastObject()->QuickSortAscending();
    }
  ccIndices.QuickSortAscending();
  this->ComputeCCfromCCindicesInAllElements(ccIndices);
}

template <class coefficient>
bool WeylGroup::GenerateOuterOrbit
(Vectors<coefficient>& theWeights, HashedList<Vector<coefficient> >& output, HashedList<ElementWeylGroup<WeylGroup> >* outputSubset,  int UpperLimitNumElements)
{ MacroRegisterFunctionWithName("WeylGroup::GenerateOuterOrbit");
  this->ComputeOuterAutoGenerators();
  List<MatrixTensor<Rational> > theOuterGens=this->theOuterAutos.GetElement().theGenerators;
  output.Clear();
  for (int i=0; i<theWeights.size; i++)
    output.AddOnTop(theWeights[i]);
  Vector<coefficient> currentRoot;
  ElementWeylGroup<WeylGroup> currentElt;
  output.SetExpectedSize(UpperLimitNumElements);
  if (outputSubset!=0)
  { currentElt.MakeID(*this);
    outputSubset->SetExpectedSize(UpperLimitNumElements);
    outputSubset->Clear();
    outputSubset->AddOnTop(currentElt);
  }
  int numGens=this->GetDim()+theOuterGens.size;
  for (int i=0; i<output.size; i++)
    for (int j=0; j<numGens; j++)
    { if (j<this->GetDim())
      { currentRoot=output[i];
        this->SimpleReflection(j, currentRoot);
      } else
        theOuterGens[j-this->GetDim()].ActOnVectorColumn(output[i], currentRoot);
      if (output.AddOnTopNoRepetition(currentRoot))
        if (outputSubset!=0)
        { currentElt.MakeID(*this);
          currentElt.generatorsLastAppliedFirst.AddOnTop(j);
          currentElt.generatorsLastAppliedFirst.AddListOnTop((*outputSubset)[i].generatorsLastAppliedFirst);
          outputSubset->AddOnTop(currentElt);
        }
      if (UpperLimitNumElements>0)
        if (output.size>=UpperLimitNumElements)
          return false;
    }
  return true;
}

template <class coefficient>
void WeylGroup::RaiseToDominantWeight(Vector<coefficient>& theWeight, int* sign, bool* stabilizerFound, ElementWeylGroup<WeylGroup>* raisingElt)
{ if (sign!=0)
    *sign=1;
  if (stabilizerFound!=0)
    *stabilizerFound=false;
  coefficient theScalarProd;
  int theDim=this->GetDim();
  if (raisingElt!=0)
    raisingElt->MakeID(*this);
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
        if (raisingElt!=0)
          raisingElt->generatorsLastAppliedFirst.AddOnTop(i);//warning order of raising element is reversed, must reverse back
      }
      if (stabilizerFound!=0)
        if (theScalarProd.IsEqualToZero())
          *stabilizerFound=true;
    }
  }
  if (raisingElt!=0)
  { raisingElt->generatorsLastAppliedFirst.ReverseOrderElements();
    raisingElt->MakeCanonical();
  }
//  std::cout << "<hr># simple reflections applied total: " << numTimesReflectionWasApplied;
}

template <class coefficient>
void WeylGroup::SimpleReflectionRhoModified(int index, Vector<coefficient>& theVector)const
{ coefficient alphaShift, tempRat;
  alphaShift=0;
  for (int i=0; i<this->CartanSymmetric.NumCols; i++)
  { tempRat=(theVector[i]);
    tempRat*=this->CartanSymmetric.elements[index][i]*(-2);
    alphaShift+=(tempRat);
  }
  alphaShift/=(this->CartanSymmetric.elements[index][index]);
  alphaShift-=1;
  theVector[index]+=(alphaShift);
}

template <class coefficient>
bool WeylGroup::GenerateOrbit
(Vectors<coefficient>& theWeights, bool RhoAction, HashedList<Vector<coefficient> >& output, bool UseMinusRho, int expectedOrbitSize,
 HashedList<ElementWeylGroup<WeylGroup> >* outputSubset, int UpperLimitNumElements, GlobalVariables* theGlobalVariables)
{ output.Clear();
  for (int i=0; i<theWeights.size; i++)
    output.AddOnTopNoRepetition(theWeights[i]);
  Vector<coefficient> currentRoot;
  ElementWeylGroup<WeylGroup> currentElt;
  if (expectedOrbitSize<=0)
    if (!this->GetGroupSizeByFormula().IsSmallEnoughToFitInInt(&expectedOrbitSize))
      expectedOrbitSize=-1;
  if (UpperLimitNumElements>0 && expectedOrbitSize>UpperLimitNumElements)
    expectedOrbitSize=UpperLimitNumElements;
  //std::cout << "<hr>Setting expected orbit size: " << expectedOrbitSize;
  output.SetExpectedSize(expectedOrbitSize);
  if (outputSubset!=0)
  { if (UpperLimitNumElements>0)
      expectedOrbitSize=MathRoutines::Minimum(UpperLimitNumElements, expectedOrbitSize);
    currentElt.MakeID(*this);
    outputSubset->SetExpectedSize(expectedOrbitSize);
    outputSubset->Clear();
    outputSubset->AddOnTop(currentElt);
  }
  ProgressReport theReport(theGlobalVariables);
  for (int i=0; i<output.size; i++)
    for (int j=0; j<this->CartanSymmetric.NumRows; j++)
    { currentRoot=output[i];
      if (theGlobalVariables!=0 && i%100==0)
      { std::stringstream reportStream;
        reportStream << "So far found " << i+1 << " elements in the orbit(s) of the starting weight(s) " <<
        theWeights.ToString() << ". ";
        theReport.Report(reportStream.str());
      }
      if (!RhoAction)
        this->SimpleReflection(j, currentRoot);
      else if (!UseMinusRho)
        this->SimpleReflectionRhoModified(j, currentRoot);
      else
        this->SimpleReflectionMinusRhoModified(j, currentRoot);
      if (output.AddOnTopNoRepetition(currentRoot))
        if (outputSubset!=0)
        { currentElt.generatorsLastAppliedFirst.SetSize(1);
          currentElt.generatorsLastAppliedFirst[0]=j;
          currentElt.generatorsLastAppliedFirst.AddListOnTop((*outputSubset)[i].generatorsLastAppliedFirst);
          currentElt.MakeCanonical();
          outputSubset->AddOnTop(currentElt);
        }
      if (UpperLimitNumElements>0)
        if (output.size>=UpperLimitNumElements)
          return false;
    }
  return true;
}

template <class coefficient>
void WeylGroup::SimpleReflectionMinusRhoModified(int index, Vector<coefficient>& theVector)const
{ coefficient alphaShift, tempRat;
  alphaShift=0;
  for (int i=0; i<this->CartanSymmetric.NumCols; i++)
  { tempRat=theVector[i];
    tempRat*=this->CartanSymmetric.elements[index][i];
    alphaShift+=tempRat;
  }
  alphaShift*=-2;
  alphaShift/=(this->CartanSymmetric.elements[index][index]);
  alphaShift+=(1);
  theVector[index]+=(alphaShift);
}

template <class coefficient>
void WeylGroup::SimpleReflection(int index, Vector<coefficient>& theVector)const
{ if (index<0 || index>=this->CartanSymmetric.NumCols)
    crash << "This is a programming error: simple reflection with respect to index " << index+1 << " in a Weyl group of rank "
    << this->GetDim() << crash;
  coefficient alphaShift, tempRat;
  alphaShift=0;
  for (int i=0; i<this->CartanSymmetric.NumCols; i++)
  { tempRat=theVector[i];
    tempRat*=this->CartanSymmetric.elements[index][i];
    alphaShift+=tempRat;
  }
  alphaShift*=-2;
  alphaShift/=this->CartanSymmetric.elements[index][index];
  theVector[index]+=alphaShift;
}

template <class coefficient>
bool WeylGroup::IsDominantWeight(const Vector<coefficient>& theWeight)
{ int theDimension= this->GetDim();
  for (int i=0; i<theDimension; i++)
    if (!this->IsDominantWRTgenerator(theWeight, i))
      return false;
  return true;
}

template<class coefficient>
coefficient WeylGroup::WeylDimFormulaSimpleCoords(Vector<coefficient>& theWeightInSimpleCoords, const coefficient& theRingUnit)
{ coefficient Result, buffer;
  Vector<coefficient> rhoOverNewRing, rootOfBorelNewRing, sumWithRho;//<-to facilitate type conversion!
  rhoOverNewRing=this->rho;//<-type conversion here!
  Result=theRingUnit;
//  std::cout << "<br>doing the weyl dim formula with: " << theWeightInSimpleCoords.ToString();
//  std::cout << "<br>rho is:" << rhoOverNewRing.ToString();
//  std::cout << "<br>rho before conversion: " << this->rho.ToString();
//  std::cout << "<br>we get: ";
  for (int i=0; i<this->RootsOfBorel.size; i++)
  { rootOfBorelNewRing=this->RootsOfBorel[i]; //<-type conversion here!
    sumWithRho=rhoOverNewRing+theWeightInSimpleCoords;
    buffer=(this->RootScalarCartanRoot(sumWithRho, rootOfBorelNewRing));
    buffer/=this->RootScalarCartanRoot(rhoOverNewRing, rootOfBorelNewRing);
//    std::cout << "(" << buffer.ToString() << ")";
    Result*=buffer;
  }
  return Result;
}

template<class coefficient>
Vector<coefficient> WeylGroup::GetFundamentalCoordinatesFromSimple(const Vector<coefficient>& inputInFundamentalCoords)
{ Matrix<Rational>& tempMat=*this->GetMatrixSimpleToFundamentalCoords();
  Vector<coefficient> result=inputInFundamentalCoords;
  tempMat.ActOnVectorColumn(result);
  return result;
}

template<class coefficient>
Vector<coefficient> WeylGroup::GetSimpleCoordinatesFromFundamental(const Vector<coefficient>& inputInFundamentalCoords)
{ Matrix<Rational>& tempMat=*this->GetMatrixFundamentalToSimpleCoords();
  Vector<coefficient> result;
  result=inputInFundamentalCoords;
//  std::cout << "<br>transition matrix from fundamental to simple: " << tempMat.ToString();
  tempMat.ActOnVectorColumn(result);
  return result;
}

template<class coefficient>
Vectors<coefficient> WeylGroup::GetSimpleCoordinatesFromFundamental(const Vectors<coefficient>& inputInFundamentalCoords)
{ Matrix<Rational>& tempMat=*this->GetMatrixFundamentalToSimpleCoords();
  Vectors<coefficient> result;
  result=inputInFundamentalCoords;
//  std::cout << "<br>transition matrix from fundamental to simple: " << tempMat.ToString();
  tempMat.ActOnVectorsColumn(result);
  return result;
}

template<class coefficient>
coefficient WeylGroup::WeylDimFormulaFundamentalCoords(Vector<coefficient>& weightFundCoords, const coefficient& theRingUnit)
{ Vector<coefficient> theWeightInSimpleCoords;
  theWeightInSimpleCoords = this->GetSimpleCoordinatesFromFundamental(weightFundCoords);
  return this->WeylDimFormulaSimpleCoords(theWeightInSimpleCoords);
}

template<class leftType, class rightType>
void WeylGroup::RootScalarCartanRoot(const Vector<leftType>& r1, const Vector<rightType>& r2, leftType& output)const
{ if (r1.size!=r2.size || r1.size!=this->GetDim())
  { crash.theCrashReport << "This is a programming error: attempting to get the scalar product of the weight " << r1 << " (dimension " << r1.size
    << ") with the weight " << r2 << " (dimension " << r2.size << "), while the dimension of the ambient Weyl group is " << this->GetDim()
    << ". ";
    crash << crash;
  }
  output=r1[0].GetZero();
  leftType buffer;
  for (int i=0; i<this->CartanSymmetric.NumRows; i++)
    for (int j=0; j<this->CartanSymmetric.NumCols; j++)
    { buffer=r1[i];
      buffer*=r2[j];
      buffer*=this->CartanSymmetric.elements[i][j];
      output+=(buffer);
    }
}

template <class coefficient>
bool WeylGroup::FreudenthalEval
(Vector<coefficient>& inputHWfundamentalCoords, HashedList<Vector<coefficient> >& outputDominantWeightsSimpleCoords,
 List<coefficient>& outputMultsSimpleCoords, std::string* outputDetails, GlobalVariables* theGlobalVariables, int UpperBoundFreudenthal)
{ //double startTimer=theGlobalVariables.GetElapsedSeconds();
  MacroRegisterFunctionWithName("WeylGroup::FreudenthalEval");
  for (int i=0; i<inputHWfundamentalCoords.size; i++)
    if (inputHWfundamentalCoords[i]<0)
    { if (outputDetails!=0)
        *outputDetails= "The highest weight is not dominant and I cannot apply the Freudenthal formula.";
      return false;
    }
  this->ComputeRho(true);
  Vectors<Rational> EiBasis;
  EiBasis.MakeEiBasis(this->GetDim());
  List<bool> Explored;
  Vector<coefficient> hwSimpleCoords=this->GetSimpleCoordinatesFromFundamental(inputHWfundamentalCoords);
  if (!this->GetAlLDominantWeightsHWFDIM(hwSimpleCoords, outputDominantWeightsSimpleCoords, UpperBoundFreudenthal, outputDetails, theGlobalVariables))
    return false;
  Explored.initFillInObject(outputDominantWeightsSimpleCoords.size, false);
  outputMultsSimpleCoords.SetSize(outputDominantWeightsSimpleCoords.size);
  Vector<coefficient> currentWeight, currentDominantRepresentative, convertor;
  coefficient hwPlusRhoSquared;
  convertor=hwSimpleCoords;
  convertor+=this->rho;//<-implicit type conversion here!!!!
  hwPlusRhoSquared=this->RootScalarCartanRoot(convertor, convertor);
  outputMultsSimpleCoords[0]=1;
  Explored[0]=true;
//  std::cout << "<br>time for generating weights and initializations: " << theGlobalVariables.GetElapsedSeconds()-startTimer;
  //static double totalTimeSpentOnHashIndexing=0;
//  static double timeSpentRaisingWeights=0;
  coefficient BufferCoeff;
  ProgressReport theReport(theGlobalVariables);
  for (int k=1; k<outputDominantWeightsSimpleCoords.size; k++)
  { Explored[k]=true;
    coefficient& currentAccum=outputMultsSimpleCoords[k];
    currentAccum=0;
    for (int j=0; j<this->RootsOfBorel.size; j++)
      for (int i=1; ; i++)
      { currentWeight=outputDominantWeightsSimpleCoords[k];
        currentWeight+=this->RootsOfBorel[j]*i;
        currentDominantRepresentative=currentWeight;
//        double startLocal=theGlobalVariables.GetElapsedSeconds();
        this->RaiseToDominantWeight(currentDominantRepresentative);
//        timeSpentRaisingWeights+=theGlobalVariables.GetElapsedSeconds()-startLocal;
        //double beforeHash=theGlobalVariables.GetElapsedSeconds();
        int theIndex=outputDominantWeightsSimpleCoords.GetIndex(currentDominantRepresentative);
        //totalTimeSpentOnHashIndexing+=theGlobalVariables.GetElapsedSeconds()-beforeHash;
        if (theIndex==-1)
          break;
//        std::cout << "<br> summing over weight: " << currentWeight.ToString();
        if (!Explored[theIndex])
        { if (outputDetails!=0)
          { std::stringstream errorLog;
            errorLog << "This is an internal error check. If you see it, that means that the Freudenthal algorithm implementation is "
            << " wrong (because the author of the implementation was dumb enough to try to write less code than what is suggested by LiE).";
            *outputDetails=errorLog.str();
          }
          return false;
        }
        BufferCoeff=this->RootScalarCartanRoot(currentWeight, this->RootsOfBorel[j]);
        BufferCoeff*=outputMultsSimpleCoords[theIndex];
        currentAccum+=BufferCoeff;
      }
    currentAccum*=2;
    convertor=outputDominantWeightsSimpleCoords[k];
    convertor+=this->rho;
    BufferCoeff=hwPlusRhoSquared;
    BufferCoeff-=this->RootScalarCartanRoot(convertor, convertor);
    if (BufferCoeff==0)
      crash << "This is a programming or a mathematical error. I get that the denominator in the Freundenthal formula is zero. "
      << " The highest weight is " << inputHWfundamentalCoords.ToString() << ". The Weyl group details follow. " << this->ToString() << crash;
    currentAccum/=BufferCoeff;

//    std::cout << "<br>Coeff we divide by: " << (hwPlusRhoSquared-this->RootScalarCartanRoot
 //   (outputDominantWeightsSimpleCoords[k]+this->rho, outputDominantWeightsSimpleCoords[k]+this->rho))
  //  .ToString()
   // ;
    std::stringstream out;
    out << " Computed the multiplicities of " << k+1 << " out of " << outputDominantWeightsSimpleCoords.size << " dominant weights in the support.";
    theReport.Report(out.str());
//    std::cout
//    << "<hr> Computed the multiplicities of " << k+1 << " out of " << outputDominantWeightsSimpleCoords.size << " dominant weights in the support.";
//    std::cout << "<br>time so far: " << theGlobalVariables.GetElapsedSeconds()-startTimer;
//    std::cout << " of which " << totalTimeSpentOnHashIndexing << " used for hash routines";
//    std::cout << " of which " << timeSpentRaisingWeights << " used to raise weights";
  }
//  std::cout << "<br>Total freudenthal running time: " << theGlobalVariables.GetElapsedSeconds()-startTimer;
  return true;
}

template<class coefficient>
bool WeylGroup::GetAlLDominantWeightsHWFDIM
(Vector<coefficient>& highestWeightSimpleCoords, HashedList<Vector<coefficient> >& outputWeightsSimpleCoords,
 int upperBoundDominantWeights, std::string* outputDetails, GlobalVariables* theGlobalVariables)
{ std::stringstream out;
//  double startTime=theGlobalVariables.GetElapsedSeconds();
//  std::cout << "<br>time elapsed: " << theGlobalVariables.GetElapsedSeconds()-startTime;
  Vector<coefficient> highestWeightTrue;
  highestWeightTrue=highestWeightSimpleCoords;
  this->RaiseToDominantWeight(highestWeightTrue);
  Vector<coefficient> highestWeightFundCoords=this->GetFundamentalCoordinatesFromSimple(highestWeightTrue);
  if (!highestWeightFundCoords.SumCoords().IsSmallInteger())
  { if (outputDetails!=0)
    { out << "<hr> The highest weight you gave in simple coordinates: " << highestWeightSimpleCoords.ToString()
      << " which equals " << highestWeightFundCoords.ToString() << "  in fundamental coordinates is not integral dominant.<br>";
      *outputDetails=out.str();
    }
    return false;
  }
  int theTopHeightSimpleCoords=(int) highestWeightSimpleCoords.GetVectorRational().SumCoords().DoubleValue()+1;
//  int theTopHeightFundCoords=(int) highestWeightFundCoords.SumCoords().DoubleValue();
  if (theTopHeightSimpleCoords<0)
    theTopHeightSimpleCoords=0;
  List<HashedList<Vector<coefficient> > > outputWeightsByHeight;
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
  Vector<coefficient> currentWeight;
//  std::cout << "<br>time spend before working cycle: " << theGlobalVariables.GetElapsedSeconds()-startTime;
  for(int lowestUnexploredHeightDiff=0; lowestUnexploredHeightDiff<=theTopHeightSimpleCoords; lowestUnexploredHeightDiff++)
  { //double startCycleTime=theGlobalVariables.GetElapsedSeconds();
    if (upperBoundDominantWeights>0 && numTotalWeightsFound>upperBoundDominantWeights)
      break;
    int bufferIndexShift=lowestUnexploredHeightDiff%topHeightRootSystemPlusOne;
    HashedList<Vector<coefficient> >& currentHashes=outputWeightsByHeight[bufferIndexShift];
    for (int lowest=0; lowest<currentHashes.size; lowest++)
      for (int i=0; i<numPosRoots; i++)
      { currentWeight=currentHashes[lowest];
        currentWeight-=this->RootsOfBorel[i];
        if (this->IsDominantWeight(currentWeight))
        { int currentIndexShift=this->RootsOfBorel[i].SumCoords().NumShort;
          currentIndexShift=(currentIndexShift+bufferIndexShift)%topHeightRootSystemPlusOne;
          if (outputWeightsByHeight[currentIndexShift].AddOnTopNoRepetition(currentWeight))
          { numTotalWeightsFound++;
            outputWeightsByHeight[currentIndexShift].AdjustHashes();
          }
        }
      }
//    std::cout << "<br>time spent before accounting at height level " << lowestUnexploredHeightDiff
//    << ": " << theGlobalVariables.GetElapsedSeconds()-startCycleTime;
//    std::cout << " Size of current level: " << currentHashes.size;
    outputWeightsSimpleCoords.AddOnTop(currentHashes);
//    std::cout << ". Time spent after accounting at height level "
//    << lowestUnexploredHeightDiff
//    << ": " << theGlobalVariables.GetElapsedSeconds()-startCycleTime;
//    startCycleTime=theGlobalVariables.GetElapsedSeconds();
    outputWeightsSimpleCoords.AdjustHashes();
    currentHashes.Clear();
//    std::cout << ". Time spent clearing up buffer at height level " << lowestUnexploredHeightDiff
//    << ": " << theGlobalVariables.GetElapsedSeconds()-startCycleTime;
    if (numTotalWeightsFound>upperBoundDominantWeights && upperBoundDominantWeights>0)
    { out << "<hr>The number of weights has exceeded the RAM memory limits, aborting the weight generation. ";
      return false;
    }
  }
  if (outputDetails!=0)
  { out << " Total number of dominant weights: " << outputWeightsSimpleCoords.size;
    *outputDetails=out.str();
  }
  return true;
}

template <class coefficient>
void WeylGroup::ReflectBetaWRTAlpha(const Vector<Rational>& alpha, const Vector<coefficient>& Beta, bool RhoAction, Vector<coefficient>& Output)const
{ coefficient bufferCoeff, alphaShift, lengthA;
  Vector<coefficient> result;
  result=(Beta);
  alphaShift=Beta[0].GetZero();//<-the zero of coefficient is not known at compile time (think multivariate polynomials)
  lengthA=alphaShift;
  if (RhoAction)
    result+=(this->rho);//<-implicit type conversion here if coefficient is not Rational
  for (int i=0; i<this->CartanSymmetric.NumRows; i++)
    for (int j=0; j<this->CartanSymmetric.NumCols; j++)
    { bufferCoeff=(result[j]);
      bufferCoeff*=(alpha[i]);
      bufferCoeff*=(this->CartanSymmetric.elements[i][j]*(-2));
      alphaShift+=(bufferCoeff);
      bufferCoeff=(alpha[i]);
      bufferCoeff*=(alpha[j]);
      bufferCoeff*=(this->CartanSymmetric.elements[i][j]);
      lengthA+=(bufferCoeff);
    }
  alphaShift/=(lengthA);
  Output.SetSize(this->CartanSymmetric.NumRows);
  for (int i=0; i<this->CartanSymmetric.NumCols; i++)
  { bufferCoeff=(alphaShift);
    bufferCoeff*=(alpha[i]);
    bufferCoeff+=(result[i]);
    Output[i]=(bufferCoeff);
  }
  if (RhoAction)
    Output-=this->rho;
}

template <typename coefficient>
bool WeylGroupRepresentation<coefficient>::CheckInitialization()const
{ if (this->ownerGroup==0)
  { crash << "This is a programming error: working with a representation with non-initialized owner Weyl group. " << crash;
    return false;
  }
  return true;
}

template <typename coefficient>
void WeylGroupRepresentation<coefficient>::reset()
{ this->parent=0;
  this->flagCharacterIsComputed=false;
  this->ownerGroup=0;
}

template <typename coefficient>
void WeylGroupRepresentation<coefficient>::init(WeylGroup& inputOwner)
{ this->reset();
  this->ownerGroup=&inputOwner;
  this->ownerGroup->CheckInitializationConjugacyClasses();
  this->generatorS.SetSize(this->ownerGroup->GetDim());
  this->theElementImageS.SetSize(this->ownerGroup->theElements.size);
  this->theElementIsComputed.initFillInObject(this->ownerGroup->theElements.size, false);
  this->classFunctionMatrices.SetSize(this->ownerGroup->ConjugacyClassCount());
  this->classFunctionMatricesComputed.initFillInObject(this->ownerGroup->ConjugacyClassCount(), false);
  this->CheckInitialization();
}

template <typename coefficient>
unsigned int WeylGroupRepresentation<coefficient>::HashFunction()const
{ unsigned int result= this->ownerGroup==0? 0 : this->ownerGroup->HashFunction();
  result+=this->theCharacteR.HashFunction();
  return result;
}

template <typename coefficient>
Matrix<coefficient>& WeylGroupRepresentation<coefficient>::GetMatrixElement(int groupElementIndex)
{ Matrix<coefficient>& theMat=this->theElementImageS[groupElementIndex];
  if (this->theElementIsComputed[groupElementIndex])
    return theMat;
  const ElementWeylGroup<WeylGroup>& theElt=this->ownerGroup->theElements[groupElementIndex];
  this->theElementIsComputed[groupElementIndex]=true;
  this->GetMatrixElement(theElt, theMat);
  return theMat;
}

template <typename coefficient>
Matrix<coefficient> WeylGroupRepresentation<coefficient>::GetMatrixElement(const ElementWeylGroup<WeylGroup>& input)
{ Matrix<coefficient> result;
  this->GetMatrixElement(input, result);
  return result;
}

template <typename coefficient>
int WeylGroupRepresentation<coefficient>::GetDim()const
{ if (this->ownerGroup->GetDim()==0)
    return 1;
  return this->generatorS[0].NumRows;
}

template <typename coefficient>
void WeylGroupRepresentation<coefficient>::GetMatrixElement(const ElementWeylGroup<WeylGroup>& input, Matrix<coefficient>& output)
{ this->CheckInitialization();
  if (this->ownerGroup->GetDim()==0) //here be trivial weyl group
  { output.MakeIdMatrix(1);
    return;
  }
  this->ownerGroup->CheckInitializationConjugacyClasses();
  output.MakeIdMatrix(this->GetDim());
  for (int i=0; i<input.generatorsLastAppliedFirst.size; i++)
    output.MultiplyOnTheRight(this->generatorS[input.generatorsLastAppliedFirst[i]]);
}

template <typename coefficient>
std::string WeylGroupRepresentation<coefficient>::GetName() const
{ std::string name;
  for(int i=0; i<this->names.size; i++)
  { name.append(this->names[i]);
    if(i!=this->names.size-1)
      name.append(" aka ");
  }
  return name;
}

template <typename coefficient>
bool WeylGroupRepresentation<coefficient>::operator>(const WeylGroupRepresentation<coefficient>& right)const
{ return this->theCharacteR > right.theCharacteR;
}

template <typename coefficient>
bool WeylGroupRepresentation<coefficient>::operator<(const WeylGroupRepresentation<coefficient>& right)const
{ return this->theCharacteR < right.theCharacteR;
}

template <typename coefficient>
void WeylGroupRepresentation<coefficient>::GetClassFunctionMatrix
(ClassFunction<WeylGroup::WeylGroupBase, coefficient>& inputChar, Matrix<coefficient>& outputMat, GlobalVariables* theGlobalVariables)
{ this->CheckInitialization();
  this->ownerGroup->CheckInitializationFDrepComputation();
  outputMat.MakeZeroMatrix(this->GetDim());
  int numClasses=this->ownerGroup->ConjugacyClassCount();
  ProgressReport theReport(theGlobalVariables);
  for(int cci=0; cci<numClasses; cci++)
  { if(inputChar[cci] == 0)
      continue;
    if (!this->classFunctionMatricesComputed[cci])
    { this->classFunctionMatricesComputed[cci]=true;
      // classFunctionMatrices does not have to be initialized.
      bool useParent=false;
      if (this->parent!=0)
        useParent=(this->parent->classFunctionMatrices.size == this->ownerGroup->ConjugacyClassCount()) && (this->parent->classFunctionMatricesComputed[cci]);
      if(useParent)
        Matrix<coefficient>::MatrixInBasis(this->parent->classFunctionMatrices[cci], this->classFunctionMatrices[cci], this->basis, this->gramMatrixInverted);
      else
      { WeylGroup::ConjugacyClass& currentCC=this->ownerGroup->conjugacyClasseS[cci];
        this->classFunctionMatrices[cci].MakeZeroMatrix(this->GetDim());
        for (int i=0; i<currentCC.theElements.size; i++)
        { if (!this->theElementIsComputed[currentCC.indicesEltsInOwner[i]])
            this->ComputeAllGeneratorImagesFromSimple(theGlobalVariables);
          this->classFunctionMatrices[cci]+=this->theElementImageS[currentCC.indicesEltsInOwner[i]];
          if (theGlobalVariables!=0)
          { std::stringstream reportstream;
            reportstream << " Computing conjugacy class " << currentCC.indicesEltsInOwner[i]+1
            << " (total num classes is " << numClasses << ").";
            theReport.Report(reportstream.str());
          }
        }
      }
      if (theGlobalVariables!=0)
      { std::stringstream reportstream;
        reportstream << "<br>Class function matrix of conjugacy class " << cci+1 << " (total num classes is " << numClasses << ") computed to be: "
        << this->classFunctionMatrices[cci].ToString();
        theReport.Report(reportstream.str());
      }

    }
    for(int j=0; j<outputMat.NumRows; j++)
      for(int k=0; k<outputMat.NumCols; k++)
        outputMat(j,k)+= this->classFunctionMatrices[cci](j,k) * inputChar[cci].GetComplexConjugate();
  }
}

template <typename coefficient>
void WeylGroupRepresentation<coefficient>::ClassFunctionMatrix
(ClassFunction<WeylGroup::WeylGroupBase, coefficient>& inputCF, Matrix<coefficient>& outputMat, GlobalVariables* theGlobalVariables)
{ int theDim=this->generatorS[0].NumRows;
  outputMat.MakeZeroMatrix(theDim);
  if(classFunctionMatrices.size == 0)
    classFunctionMatrices.SetSize(this->ownerGroup->ConjugacyClassCount());
  for(int cci=0; cci<this->ownerGroup->ConjugacyClassCount(); cci++)
  { if(inputCF[cci] == 0)
      continue;
    if(classFunctionMatrices[cci].NumCols == 0)
    { //std::cout << "Generating class function matrix " << cci << " with dimension " << this->generatorS[0].NumCols
      //<< "(cc has " << this->ownerGroup->conjugacyClasses[cci].size << ")" << std::endl;
      classFunctionMatrices[cci].MakeZeroMatrix(this->generatorS[0].NumCols);
      for(int icci=0; icci<this->ownerGroup->conjugacyClasseS[cci].size; icci++)
      { //Matrix<coefficient> Mi;
        //Mi.MakeIdMatrix(this->generators[0].NumCols);
        this->classFunctionMatrices[cci] += this->GetMatrixElement(this->ownerGroup->conjugacyClasseS[cci].theElements[icci]);
        //for(int gi=g.reflections.size-1; ; gi--)
        //{  if(gi < 0)
        //      break;
        //    Mi.MultiplyOnTheRight(gens[g.reflections[gi]]);
        //}
        //classFunctionMatrices[cci] += Mi;
      }
    }
    for(int i=0; i<outputMat.NumRows; i++)
      for(int j=0; j<outputMat.NumCols; j++)
        outputMat.elements[i][j]+= classFunctionMatrices[cci].elements[i][j] * inputCF[cci];
  }
//  std::cout << outputMat.ToString(&consoleFormat) << std::endl;

}

template <typename coefficient>
std::string WeylGroupRepresentation<coefficient>::ToString(FormatExpressions* theFormat)const
{ if (this->ownerGroup==0)
    return "non-initialized representation";
  std::stringstream out;
  if (this->flagCharacterIsComputed)
    out << "Character: " << this->theCharacteR.ToString(theFormat) << " of norm " << this->theCharacteR.Norm();
  else
    out << "Character needs to be computed.";
  int theRank=this->ownerGroup->GetDim();
  LargeIntUnsigned theLCM, theDen;
  this->GetLargestDenominatorSimpleGens(theLCM, theDen);
  out << "\n<br>\n LCM denominators simple generators: " << theLCM.ToString() << ", largest denominator: " << theDen.ToString();
  out << "\n<br>\nThe simple generators (" << theRank << " total):<br> ";
  std::stringstream forYourCopyConvenience;
  for (int i=0; i<theRank; i++)
    if (i<this->generatorS.size)
    { std::stringstream tempStream;
      tempStream << "s_" << i+1 << ":=MatrixRationals{}" << this->generatorS[i].ToString(theFormat) << "; \\\\\n";
      forYourCopyConvenience << tempStream.str();
      out << CGI::GetMathSpanPure("\\begin{array}{l}"+ tempStream.str()+"\\end{array}", 3000);
    } else
      out << "Simple generator " << i+1 << "} not computed ";
  out << "<br>For your copy convenience: <br>" << forYourCopyConvenience.str();
  return out.str();
}

template <class coefficient>
void SubgroupWeylGroupOLD::ActByElement(int index, Vector<coefficient>& theRoot)const
{ Vector<coefficient> tempRoot;
  this->ActByElement(index, theRoot, tempRoot);
  theRoot=(tempRoot);
}

template <class coefficient>
void SubgroupWeylGroupOLD::ActByElement(int index, Vector<coefficient>& input, Vector<coefficient>& output)const
{ this->ActByElement(this->TheObjects[index], input, output);
}

template <class coefficient>
void SubgroupWeylGroupOLD::ActByElement(const ElementWeylGroup<WeylGroup>& theElement, const Vector<coefficient>& input, Vector<coefficient>& output)const
{ if(&input==&output)
    crash << crash;
  Vector<coefficient> tempRoot, tempRoot2;
  output=(input);
  for (int i=theElement.generatorsLastAppliedFirst.size-1; i>=0; i--)
  { int tempI=theElement.generatorsLastAppliedFirst[i];
    if(tempI<this->simpleGenerators.size)
      this->AmbientWeyl.ReflectBetaWRTAlpha(this->simpleGenerators[tempI], output, false, output);
    else
    { tempI-=this->simpleGenerators.size;
      tempRoot.MakeZero(input.size);
      for (int j=0; j<output.size; j++)
      { tempRoot2=this->ExternalAutomorphisms[tempI][j];
        tempRoot2*=output[j];
        tempRoot+=tempRoot2;
      }
    }
  }
}

template<class coefficient>
void SubgroupWeylGroupOLD::ActByElement(const ElementWeylGroup<WeylGroup>& theElement, const Vectors<coefficient>& input, Vectors<coefficient>& output)const
{ if(&input==&output)
    crash << crash;
  output.SetSize(input.size);
  for (int i=0; i<input.size; i++)
    this->ActByElement(theElement, input[i], output[i]);
}

template <class coefficient>
bool SubgroupWeylGroupOLD::IsDominantWeight(const Vector<coefficient>& theWeight)
{ for (int i=0; i<this->simpleGenerators.size; i++)
    if (! this->IsDominantWRTgenerator(theWeight, i))
      return false;
  return true;
}

template<class coefficient>
coefficient SubgroupWeylGroupOLD::WeylDimFormulaSimpleCoords(const Vector<coefficient>& theWeightInSimpleCoords, const coefficient& theRingUnit)
{ coefficient Result, buffer;
  Vector<coefficient> rhoOverNewRing, rootOfBorelNewRing, sumWithRho;//<-to facilitate type conversion!
  Vector<Rational> rho;
  this->RootsOfBorel.sum(rho, this->AmbientWeyl.GetDim());
  rho/=2;
  rhoOverNewRing=rho;//<-type conversion here!
  Result=theRingUnit;
//  std::cout << "<br>doing the weyl dim formula with: " << theWeightInSimpleCoords.ToString();
//  std::cout << "<br>rho is:" << rhoOverNewRing.ToString();
//  std::cout << "<br>we get: ";
  for (int i=0; i<this->RootsOfBorel.size; i++)
  { rootOfBorelNewRing=this->RootsOfBorel[i]; //<-type conversion here!
    sumWithRho=rhoOverNewRing+theWeightInSimpleCoords;
    buffer=(this->AmbientWeyl.RootScalarCartanRoot(sumWithRho, rootOfBorelNewRing));
    buffer/=this->AmbientWeyl.RootScalarCartanRoot(rhoOverNewRing, rootOfBorelNewRing);
//    std::cout << "(" << buffer.ToString() << ")";
    Result*=buffer;
  }
  return Result;
}

template <class coefficient>
bool SubgroupWeylGroupOLD::GetAlLDominantWeightsHWFDIM
(Vector<coefficient>& highestWeightSimpleCoords, HashedList<Vector<coefficient> >& outputWeightsSimpleCoords,
 int upperBoundDominantWeights, std::string& outputDetails, GlobalVariables& theGlobalVariables)
{ MacroRegisterFunctionWithName("SubgroupWeylGroupOLD::GetAlLDominantWeightsHWFDIM");
  std::stringstream out;
  this->ComputeRootSubsystem();
//  double startTime=theGlobalVariables.GetElapsedSeconds();
//  std::cout << "<br>time elapsed: " << theGlobalVariables.GetElapsedSeconds()-startTime;
  Vector<coefficient> highestWeightTrue=highestWeightSimpleCoords;
  Vectors<Rational> basisEi;
  int theDim=this->AmbientWeyl.GetDim();
  basisEi.MakeEiBasis(theDim);
  this->RaiseToDominantWeight(highestWeightTrue);
  Vector<coefficient> highestWeightFundCoords=this->AmbientWeyl.GetFundamentalCoordinatesFromSimple(highestWeightTrue);
  int theTopHeightSimpleCoords=(int) highestWeightSimpleCoords.GetVectorRational().SumCoords().DoubleValue()+1;
//  int theTopHeightFundCoords=(int) highestWeightFundCoords.SumCoords().DoubleValue();
  if (theTopHeightSimpleCoords<0)
    theTopHeightSimpleCoords=0;
  List<HashedList<Vector<coefficient> > > outputWeightsByHeight;
  int topHeightRootSystem=this->AmbientWeyl.RootsOfBorel.LastObject()->SumCoords().NumShort;
  int topHeightRootSystemPlusOne=topHeightRootSystem+1;
  outputWeightsByHeight.SetSize(topHeightRootSystemPlusOne);
  int finalHashSize=100;
  for (int i=0; i<topHeightRootSystemPlusOne; i++)
    outputWeightsByHeight[i].SetHashSizE(finalHashSize);
  outputWeightsSimpleCoords.Clear();
  outputWeightsByHeight[0].AddOnTop(highestWeightTrue);
  int numTotalWeightsFound=0;
  Vector<coefficient> currentWeight, currentWeightRaisedToDominant;
//  std::cout << "<br>time spend before working cycle: " << theGlobalVariables.GetElapsedSeconds()-startTime;
  for (int lowestUnexploredHeightDiff=0; lowestUnexploredHeightDiff<=theTopHeightSimpleCoords;
  lowestUnexploredHeightDiff++)
  { //double startCycleTime=theGlobalVariables.GetElapsedSeconds();
    if (upperBoundDominantWeights>0 && numTotalWeightsFound>upperBoundDominantWeights)
      break;
    int bufferIndexShift=lowestUnexploredHeightDiff%topHeightRootSystemPlusOne;
    HashedList<Vector<coefficient> >& currentHashes=outputWeightsByHeight[bufferIndexShift];
    for (int lowest=0; lowest<currentHashes.size; lowest++)
      for (int i=0; i<this->RootsOfBorel.size; i++)
      { currentWeight=currentHashes[lowest];
        currentWeight-=this->RootsOfBorel[i];
        if (this->IsDominantWeight(currentWeight))
        { int currentIndexShift=this->RootsOfBorel[i].SumCoords().NumShort;
          currentIndexShift=(currentIndexShift+bufferIndexShift)%topHeightRootSystemPlusOne;
          if (outputWeightsByHeight[currentIndexShift].AddOnTopNoRepetition(currentWeight))
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
    out << "<hr>This message is generated either because the number of weights has exceeded the hard-coded RAM memory limits, or because "
    << " a priori bound for the number of weights is WRONG. If the latter is the case, make sure to send an angry email to the author(s).";
  outputDetails=out.str();
  //std::cout << "<hr><hr>Total time spent generating weights: " << -startTime+theGlobalVariables.GetElapsedSeconds();
  return (numTotalWeightsFound<=upperBoundDominantWeights);
}

template <class coefficient>
void SubgroupWeylGroupOLD::RaiseToDominantWeight(Vector<coefficient>& theWeight, int* sign, bool* stabilizerFound)
{ MacroRegisterFunctionWithName("SubgroupWeylGroupOLD::RaiseToDominantWeight");
  if (sign!=0)
    *sign=1;
  if (stabilizerFound!=0)
    *stabilizerFound=false;
  Rational theScalarProd;
//  int theDim=this->AmbientWeyl.GetDim();
  for (bool found = true; found; )
  { found=false;
    for (int i=0; i<this->simpleGenerators.size; i++)
    { if (! this->IsDominantWRTgenerator(theWeight, i))
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

template <class coefficient>
bool SubgroupWeylGroupOLD::GenerateOrbitReturnFalseIfTruncated(const Vector<coefficient>& input, Vectors<coefficient>& outputOrbit, int UpperLimitNumElements)
{ HashedList<Vector<coefficient> > theOrbit;
  bool result = true;
  theOrbit.Clear();
  Vector<coefficient> tempRoot;
  theOrbit.AddOnTop(input);
  MemorySaving<Vectors<coefficient> >ExternalAutosOverAmbientField;
  for (int i=0; i<theOrbit.size; i++)
  { for (int j=0; j<this->simpleGenerators.size; j++)
    { this->AmbientWeyl.ReflectBetaWRTAlpha(this->simpleGenerators[j], theOrbit[i], false, tempRoot);
//      int oldsize=theOrbit.size;
//      std::string debugString=tempRoot.ToString() ;
      theOrbit.AddOnTopNoRepetition(tempRoot);
//      if (oldsize<theOrbit.size)
//        std::cout << "<br>" << debugString << " with hash " << tempRoot.HashFunction() << " added, ";
//      else
//        std::cout << "<br>" << debugString << " with hash " << tempRoot.HashFunction() << " NOT added, ";
    }
    for (int j=1; j<this->ExternalAutomorphisms.size; j++)
    { ExternalAutosOverAmbientField.GetElement()=this->ExternalAutomorphisms[j];
      theOrbit[i].GetCoordsInBasiS(ExternalAutosOverAmbientField.GetElement(), tempRoot);
      theOrbit.AddOnTopNoRepetition(tempRoot);
    }
    if (UpperLimitNumElements>0)
      if (theOrbit.size>=UpperLimitNumElements)
      { result=false;
        break;
      }
  }
  outputOrbit=(theOrbit);
  return result;
}

template <class coefficient>
bool SubgroupWeylGroupOLD::FreudenthalEvalIrrepIsWRTLeviPart
(const Vector<coefficient>& inputHWfundamentalCoords, HashedList<Vector<coefficient> >& outputDominantWeightsSimpleCoordS,
 List<coefficient>& outputMultsSimpleCoords, std::string& outputDetails, GlobalVariables& theGlobalVariables, int UpperBoundFreudenthal)
{ MacroRegisterFunctionWithName("SubgroupWeylGroupOLD::FreudenthalEvalIrrepIsWRTLeviPart");
  //double startTimer=theGlobalVariables.GetElapsedSeconds();
  this->ComputeRootSubsystem();
  Vector<Rational> EiVect;
  List<bool> Explored;
  coefficient theRingZero;
  theRingZero=inputHWfundamentalCoords[0].GetZero();
  /////////////////////////
  Vector<coefficient> hwSimpleCoordsLeviPart, hwSimpleCoordsNilPart;
  hwSimpleCoordsLeviPart=inputHWfundamentalCoords;
  hwSimpleCoordsNilPart=inputHWfundamentalCoords;
  for (int i=0; i<hwSimpleCoordsLeviPart.size; i++)
  { EiVect.MakeEi(hwSimpleCoordsLeviPart.size, i);
    if (!this->RootsOfBorel.Contains(EiVect))
      hwSimpleCoordsLeviPart[i]=theRingZero;
    else
      hwSimpleCoordsNilPart[i]=theRingZero;
  }
  hwSimpleCoordsLeviPart=this->AmbientWeyl.GetSimpleCoordinatesFromFundamental(hwSimpleCoordsLeviPart);
  hwSimpleCoordsNilPart=this->AmbientWeyl.GetSimpleCoordinatesFromFundamental(hwSimpleCoordsNilPart);
//  std::cout << "highest weight levi part simple coords: " << hwSimpleCoordsLeviPart.ToString();
//  std::cout << "highest weight nil part siple coords: " << hwSimpleCoordsNilPart.ToString();
  ///////////////////////////
  HashedList<Vector<coefficient> > outputDomWeightsSimpleCoordsLeviPart;

  if (!this->GetAlLDominantWeightsHWFDIM(hwSimpleCoordsLeviPart, outputDomWeightsSimpleCoordsLeviPart, UpperBoundFreudenthal, outputDetails, theGlobalVariables))
  { std::stringstream errorLog;
    errorLog << "Error: the number of dominant weights exceeded hard-coded limit of " << UpperBoundFreudenthal
    << ". Please check out whether LiE's implementation of the Freudenthal formula can do your computation.";
    outputDetails=errorLog.str();
    return false;
  }
//  std::cout << "Highest weight: " << hwSimpleCoords.ToString() << "<br>Dominant weights wrt. levi part("
//  << outputDomWeightsSimpleCoordsLeviPart.size << "):<br> ";
//  for (int i=0; i<outputDomWeightsSimpleCoordsLeviPart.size; i++)
//    std::cout << "<br>" << outputDomWeightsSimpleCoordsLeviPart[i].ToString();
  Explored.initFillInObject(outputDomWeightsSimpleCoordsLeviPart.size, false);
  outputMultsSimpleCoords.SetSize(outputDomWeightsSimpleCoordsLeviPart.size);
  Vector<coefficient> currentWeight, currentDominantRepresentative;
  Vector<coefficient> Rho;
  Rho=this->GetRho();//<-implicit type conversion here!
//  std::cout << "<br> Rho equals: " << Rho.ToString();
  //out << "<br> Rho equals: " << Rho.ToString();
  coefficient hwPlusRhoSquared;
  hwPlusRhoSquared=this->AmbientWeyl.RootScalarCartanRoot(hwSimpleCoordsLeviPart+Rho, hwSimpleCoordsLeviPart+Rho);
  Explored[0]=true;
  outputMultsSimpleCoords[0]=1;
//  std::cout << "<br>time for generating weights and initializations: " << theGlobalVariables.GetElapsedSeconds()-startTimer;
  //static double totalTimeSpentOnHashIndexing=0;
//  static double timeSpentRaisingWeights=0;
//std::cout << "Freudenthal eval w.r.t subalgebra: positive root subsystem: " <<  this->RootsOfBorel.ToString();
  Vector<coefficient> convertor;
  coefficient bufferCoeff;
  ProgressReport theReport(&theGlobalVariables);
  for (int k=1; k< outputDomWeightsSimpleCoordsLeviPart.size; k++)
  { Explored[k]=true;
    coefficient& currentAccum=outputMultsSimpleCoords[k];
    currentAccum=0;
    for (int j=0; j<this->RootsOfBorel.size; j++)
      for (int i=1; ; i++)
      { convertor=this->RootsOfBorel[j];
        convertor*=i;
        currentWeight= outputDomWeightsSimpleCoordsLeviPart[k]+convertor;
        currentDominantRepresentative=currentWeight;
//        double startLocal=theGlobalVariables.GetElapsedSeconds();
        this->RaiseToDominantWeight(currentDominantRepresentative);
//        std::cout << "<br>currentDominant representative: " << currentDominantRepresentative.ToString();
        int theIndex=outputDomWeightsSimpleCoordsLeviPart.GetIndex(currentDominantRepresentative);
//        std::cout << "<br>index of currentDomain rep: " << theIndex;
        //totalTimeSpentOnHashIndexing+=theGlobalVariables.GetElapsedSeconds()-beforeHash;
        if (theIndex==-1)
          break;
//        std::cout << "<br> summing over weight: " << currentWeight.ToString();
        if (!Explored[theIndex])
        { std::stringstream errorLog;
          errorLog << "This is an internal error check. If you see it, that means "
          << " that the Freudenthal algorithm implementation is "
          << " wrong (because the author of the implementation was dumb enough to"
          << " try to write less code than what is "
          << " suggested by LiE).";
          outputDetails=errorLog.str();
          return false;
        }
        convertor=this->RootsOfBorel[j];//<-implicit type conversion here!
        bufferCoeff=this->AmbientWeyl.RootScalarCartanRoot(currentWeight, convertor);
        bufferCoeff*=outputMultsSimpleCoords[theIndex];
        currentAccum+=bufferCoeff;
//        std::cout << "<hr>current weight: " << currentWeight.ToString();
//        std::cout << "<br>current dominant representative " << currentDominantRepresentative.ToString();
      }
    currentAccum*=2;
   // std::cout << "<br>hwPlusRhoSquared: " << hwPlusRhoSquared.ToString();
    bufferCoeff=hwPlusRhoSquared;
    bufferCoeff-=this->AmbientWeyl.RootScalarCartanRoot(outputDomWeightsSimpleCoordsLeviPart[k]+Rho, outputDomWeightsSimpleCoordsLeviPart[k]+Rho);
    //bufferCoeff now holds the denominator participating in the Freudenthal formula.
    if(bufferCoeff.IsEqualToZero())
      crash << crash;
    currentAccum/=bufferCoeff;
//    std::cout << "<br>Coeff we divide by: " << bufferCoeff.ToString()
 //   ;
    std::stringstream out;
    out << " Computed the multiplicities of " << k+1 << " out of " << outputDomWeightsSimpleCoordsLeviPart.size << " dominant weights in the support.";
    theReport.Report(out.str());
//    std::cout
  //  << "<hr> Computed the multiplicities of " << k+1 << " out of " << outputDomWeightsSimpleCoordsLeviPart.size << " dominant weights in the support.";
 //   theGlobalVariables.MakeStatusReport(out.str());
//    std::cout << "<br>time so far: " << theGlobalVariables.GetElapsedSeconds()-startTimer;
//    std::cout << " of which " << totalTimeSpentOnHashIndexing << " used for hash routines";
//    std::cout << " of which " << timeSpentRaisingWeights << " used to raise weights";
  }
  outputDominantWeightsSimpleCoordS.Clear();
  outputDominantWeightsSimpleCoordS.SetExpectedSize(outputDomWeightsSimpleCoordsLeviPart.size);
  for (int i=0; i<outputDomWeightsSimpleCoordsLeviPart.size; i++)
    outputDominantWeightsSimpleCoordS.AddOnTop(outputDomWeightsSimpleCoordsLeviPart[i]+hwSimpleCoordsNilPart);
//  std::cout << "<br>Total freudenthal running time: " << theGlobalVariables.GetElapsedSeconds()-startTimer;
  return true;
}

template<class someFiniteGroup, typename coefficient>
void ClassFunction<someFiniteGroup, coefficient>::MakeZero(someFiniteGroup& inputWeyl)
{ this->G=&inputWeyl;
  this->data.MakeZero(this->G->ConjugacyClassCount());
}

template<class someFiniteGroup, typename coefficient>
coefficient& ClassFunction<someFiniteGroup, coefficient>::operator[](int i) const
{ return this->data[i];
}

template<class someFiniteGroup, typename coefficient>
std::string ClassFunction<someFiniteGroup, coefficient>::ToString(FormatExpressions* theFormat) const
{ if (this->G==0)
    return "(not initialized)";
  std::stringstream out;
  out << "(";
  for(int i=0;i<this->data.size;i++){
    out << this->data[i];
    if(i<this->data.size-1)
      out << ", ";
  }
  out << ")";
  if (this->Norm().ToString()!="1")
  { out << "]";
    out << this->Norm();
    out << "]";
  }
  return out.str();
}

template<class someFiniteGroup, typename coefficient>
std::ostream& operator<<(std::ostream& out, const ClassFunction<someFiniteGroup, coefficient> X)
{ out << X.ToString();
  return out;
}

template<class someFiniteGroup, typename coefficient>
unsigned int ClassFunction<someFiniteGroup, coefficient>::HashFunction(const ClassFunction<someFiniteGroup, coefficient>& input)
{
  unsigned int acc;
  int N = (input.data.size < SomeRandomPrimesSize) ? input.data.size : SomeRandomPrimesSize;
  for(int i=0; i<N; i++)
    acc = input.data[i].HashFunction()*SomeRandomPrimes[i];
  return acc;
}

template<class someFiniteGroup, typename coefficient>
bool ClassFunction<someFiniteGroup, coefficient>::operator==(const ClassFunction<someFiniteGroup, coefficient>& other)const
{ if (this->G!=other.G)
    return false; //we allow comparison of class functions belonging to different groups
  //(this should be handy for the calculator interface, an user may have more than 1 group in play).
  if(this->data == other.data)
    return true;
  return false;
}

template<class someFiniteGroup, typename coefficient>
bool ClassFunction<someFiniteGroup, coefficient>::operator>(const ClassFunction<someFiniteGroup, coefficient>& right) const
{ for(int i=0; i<this->data.size; i++)
    if(!(this->data[i] == right.data[i]))
      return this->data[i] > right.data[i];
  return false;
}
#endif
