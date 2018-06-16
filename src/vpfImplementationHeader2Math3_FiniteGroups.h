//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfImplementationHeaderFiniteGroupsIncluded
#define vpfImplementationHeaderFiniteGroupsIncluded

#include "vpfHeader2Math3_FiniteGroups.h"
static ProjectInformationInstance ProjectInfovpfImplementationHeaderFiniteGroups(__FILE__, "Implementation header, finite groups. ");

template <class coefficient>
std::string FinitelyGeneratedMatrixMonoid<coefficient>::ToString(FormatExpressions* theFormat) const
{ std::stringstream out;
  out << "Number of generators: " << this->theGenerators.size;
  out << "<br>Number of elements: " << this->theElements.size;
  out << "<br>The elements follow.";
  int numEltstoDisplay = this->theElements.size;
  if (numEltstoDisplay > 100)
  { out << "<b>Displaying only the first " << 100 << " elements.</b>";
    numEltstoDisplay = 100;
  }
  for (int i = 0; i < numEltstoDisplay; i ++)
    out << "<br>" << this->theElements[i].ToStringMatForm(theFormat);
  return out.str();
}

template <typename elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::ComputeAllElements
(bool andWords, int MaxElements)
{ MacroRegisterFunctionWithName("FiniteGroup::ComputeAllElements");
  this->CheckConsistency();
  //double startTimeDebug= theGlobalVariables.GetElapsedSeconds();
  this->sizePrivate = this->SizeByFormulaOrNeg1();
  if (this->sizePrivate > 0 && MaxElements > 0 && this->sizePrivate > MaxElements)
    return false;
  if (!this->ComputeAllElementsLargeGroup(andWords, MaxElements))
    return false;
  return true;
}

template <typename elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::ComputeAllElementsLargeGroup(bool andWords, int MaxElements)
{ MacroRegisterFunctionWithName("Subgroup::ComputeAllElements");
  this->InitGenerators();
  if (this->generators.size == 0)
    crash << "Groups with zero generators are not allowed: if you wanted to create a trivial group, "
    << " trivial groups are assumed to have a generator (the identity). " << crash;
  this->theElements.Clear();
  elementSomeGroup currentElement;
  currentElement.MakeID(this->generators[0]);
  this->theElements.AddOnTop(currentElement);
  this->theWords.SetSize(0);
  if (andWords)
  { this->theWords.SetSize(1);
    this->theWords.LastObject()->SetSize(0);
  }
  ProgressReport theReport;
  //Warning: not checking whether the generators have repetitions.
  for (int j = 0; j < this->theElements.size; j ++)
    for (int i = 0; i < this->generators.size; i ++)
    { currentElement = this->generators[i] * this->theElements[j];
      if (this->theElements.AddOnTopNoRepetition(currentElement) && andWords)
      { if (this->GetWordByFormula == 0)
        { this->theWords.AddOnTop(this->theWords[j]);
          this->theWords.LastObject()->AddOnTop(i);
        } else
        { this->theWords.SetSize(this->theWords.size + 1);
          this->GetWordByFormula(*this, currentElement, *this->theWords.LastObject());
        }
      }
      if (theGlobalVariables.flagReportEverything)
        if (this->theElements.size % 100 == 0)
        { std::stringstream reportStream;
          LargeInt sizeByFla = this->SizeByFormulaOrNeg1();
          reportStream << "So far, generated " << this->theElements.size << " elements";
          if (sizeByFla > 0)
            reportStream << " out of " << sizeByFla.ToString();
          reportStream << ".";
          theReport.Report(reportStream.str());
        }
      if (MaxElements > 0)
        if (this->theElements.size > MaxElements)
          return false;
    }
  if (theGlobalVariables.flagReportEverything)
  { std::stringstream reportStream;
    reportStream << "Generated group with a total of " << this->theElements.size << " elements. ";
    theReport.Report(reportStream.str());
  }
  this->sizePrivate = this->theElements.size;
  this->flagAllElementsAreComputed = true;
  if (andWords)
    this->flagWordsComputed = true;
  return true;
}

template <class templateWeylGroup>
ElementWeylGroup<templateWeylGroup> ElementWeylGroup<templateWeylGroup>::operator^(const ElementWeylGroup<templateWeylGroup>& right) const
{ if (this->owner != right.owner)
    crash << "Not allowed to conjugate elements of different Weyl groups.  If you did this intentionally, try changing "
    << __FILE__ << ":" << __LINE__ << crash;
  ElementWeylGroup<templateWeylGroup> out;
  out.owner = this->owner;
  out.generatorsLastAppliedFirst = right.generatorsLastAppliedFirst;
  out.generatorsLastAppliedFirst.AddListOnTop(this->generatorsLastAppliedFirst);
  for (int i = right.generatorsLastAppliedFirst.size - 1; i >= 0; i --)
    out.generatorsLastAppliedFirst.AddOnTop(right.generatorsLastAppliedFirst[i]);
  out.MakeCanonical();
  return out;
}

template <class templateWeylGroup>
void ElementWeylGroup<templateWeylGroup>::ConjugationAction
(const ElementWeylGroup<templateWeylGroup>& ConjugateWith,
 const ElementWeylGroup<templateWeylGroup>& ConjugateOn,
 ElementWeylGroup<templateWeylGroup>& out)
{ out = ConjugateOn^ConjugateWith;
}

template <class templateWeylGroup>
void ElementWeylGroup<templateWeylGroup>::operator*=(const ElementWeylGroup<WeylGroupData>& other)
{ if (this->owner != other.owner)
    crash << "This is a programming error: attempting to multiply elements belonging to different Weyl groups. " << crash;
  if (&other == this)
  { ElementWeylGroup<WeylGroupData> otherCopy = other;
    (*this) *= otherCopy;
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
std::string ElementWeylGroup<templateWeylGroup>::ToString
(FormatExpressions* theFormat) const
{ MacroRegisterFunctionWithName("ElementWeylGroup::ToString");
  (void) theFormat;//avoid unused parameter warning, portable
  if (this->generatorsLastAppliedFirst.size == 0)
    return "id";
  std::stringstream out;
  for (int i = 0; i < this->generatorsLastAppliedFirst.size; i ++)
    out << this->generatorsLastAppliedFirst[i].ToString();
  return out.str();
}

template <class templateWeylGroup>
unsigned int ElementWeylGroup<templateWeylGroup>::HashFunction() const
{ int top = MathRoutines::Minimum(this->generatorsLastAppliedFirst.size, ::SomeRandomPrimesSize);
  unsigned int result = 0;
  for (int i = 0; i < top; i ++)
    result += this->generatorsLastAppliedFirst[i].HashFunction() * ::SomeRandomPrimes[i];
  return result;
}

template <class templateWeylGroup>
bool ElementWeylGroup<templateWeylGroup>::operator>(const ElementWeylGroup<templateWeylGroup>& other) const
{ if (this->owner != other.owner)
    crash << "Comparing elements of different Weyl groups. " << crash;
  return this->generatorsLastAppliedFirst>other.generatorsLastAppliedFirst;
}

template <class templateWeylGroup>
void ElementWeylGroup<templateWeylGroup>::MakeID(templateWeylGroup& inputWeyl)
{ this->owner = &inputWeyl;
  this->generatorsLastAppliedFirst.SetSize(0);
}

template <class templateWeylGroup>
void ElementWeylGroup<templateWeylGroup>::MakeID(const FiniteGroup<ElementWeylGroup<templateWeylGroup> >& inputGroup)
{ this->owner = inputGroup.generators[0].owner;
  this->generatorsLastAppliedFirst.SetSize(0);
}

template <class templateWeylGroup>
void ElementWeylGroup<templateWeylGroup>::MakeID(const ElementWeylGroup& initializeFrom)
{ this->MakeID(*initializeFrom.owner);
}

template <class templateWeylGroup>
bool ElementWeylGroup<templateWeylGroup>::IsID()
{ this->MakeCanonical();
  return this->generatorsLastAppliedFirst.size == 0;
}

template <class templateWeylGroup>
void ElementWeylGroup<templateWeylGroup>::MakeRootReflection
(const Vector<Rational>& mustBeRoot, WeylGroupData& inputWeyl)
{ *this = inputWeyl.GetRootReflection(inputWeyl.RootSystem.GetIndexIMustContainTheObject(mustBeRoot));
}

template <class templateWeylGroup>
void ElementWeylGroup<templateWeylGroup>::MakeSimpleReflection(int simpleRootIndex, WeylGroupData& inputWeyl)
{ this->owner = &inputWeyl;
  this->generatorsLastAppliedFirst.SetSize(1);
  this->generatorsLastAppliedFirst[0].MakeSimpleReflection(simpleRootIndex);
}

template <class templateWeylGroup>
void ElementWeylGroup<templateWeylGroup>::MakeOuterAuto(int outerAutoIndex, WeylGroupData& inputWeyl)
{ this->owner = &inputWeyl;
  this->generatorsLastAppliedFirst.SetSize(1);
  this->generatorsLastAppliedFirst[0].MakeOuterAuto(outerAutoIndex);
}

template <class templateWeylGroup>
void ElementWeylGroup<templateWeylGroup>::MakeFromRhoImage(const Vector<Rational>& inputRhoImage, WeylGroupData& inputWeyl)
{ this->owner = &inputWeyl;
  int theRank = this->owner->GetDim();
  this->generatorsLastAppliedFirst.SetSize(0);
  //stOutput << theVector.ToString();
  Vector<Rational> theVector = inputRhoImage;
  simpleReflectionOrOuterAuto theGen;
  while (theVector != this->owner->rho)
    for (int i = 0; i < theRank; i ++)
      if (this->owner->GetScalarProdSimpleRoot(theVector, i) < 0)
      { this->owner->SimpleReflection(i, theVector);
        theGen.MakeSimpleReflection(i);
        //stOutput << "--"  << i+ 1 << "-- > " << theVector.ToString() ;
        this->generatorsLastAppliedFirst.AddOnTop(theGen);
        break;
      }
//  stOutput << ", finally: " << this->ToString();
}

template <class templateWeylGroup>
void ElementWeylGroup<templateWeylGroup>::MakeCanonical()
{ //stOutput << "<hr>making " << this->ToString() << " canonical: ";
  MacroRegisterFunctionWithName("ElementWeylGroup::MakeCanonical");
  this->CheckInitialization();
  if (this->owner->rho.size == 0)
    this->owner->ComputeRho(false);
  Vector<Rational> theVector;
  this->owner->ActOn(*this, this->owner->rho, theVector);
  this->MakeFromRhoImage(theVector, *this->owner);
  //stOutput << " end result: " << this->ToString() << ". ";
}

template <class templateWeylGroup>
bool ElementWeylGroup<templateWeylGroup>::HasDifferentConjugacyInvariantsFrom
(const ElementWeylGroup<templateWeylGroup>& right) const
{ MacroRegisterFunctionWithName("ElementWeylGroup::HasDifferentConjugacyInvariantsFrom");
  //stOutput << "<br>Comparing invariants of " << this->ToString() << " and " << right.ToString();
  if ((this->generatorsLastAppliedFirst.size % 2) != (right.generatorsLastAppliedFirst.size % 2))
    return true;
  if (*this == right) //just in case
    return false;
  this->CheckInitialization();
  //stOutput << " the sign is the same... ";
  Polynomial<Rational> leftCharPoly, rightCharPoly;
  this->GetCharacteristicPolyStandardRepresentation(leftCharPoly);
  right.GetCharacteristicPolyStandardRepresentation(rightCharPoly);
  if (leftCharPoly != rightCharPoly)
    return true;
  //stOutput << " the char polys are the same... ";
  VectorSparse<Rational> leftCycleStructure, rightCycleStructure;
  this->GetCycleStructure(leftCycleStructure);
  right.GetCycleStructure(rightCycleStructure);
  if (leftCycleStructure != rightCycleStructure)
    return true;
  //stOutput << " the cycle structures are the same... the elements are POSSIBLY conjugate.";
  return false;
}

template <class templateWeylGroup>
std::string ElementWeylGroup<templateWeylGroup>::ToStringInvariants(FormatExpressions* theFormat) const
{ MacroRegisterFunctionWithName("ElementWeylGroup::GetCycleStructure");
  VectorSparse<Rational> theCycleStructure;
  this->GetCycleStructure(theCycleStructure);
  FormatExpressions cycleLetterFormat;
  cycleLetterFormat.polyDefaultLetter = "c";
  std::stringstream out;
  out << "Cycle structure: " << theCycleStructure.ToString(&cycleLetterFormat) << ". ";
  return out.str();
}

template <class templateWeylGroup>
void ElementWeylGroup<templateWeylGroup>::GetCycleStructure
(VectorSparse<Rational>& outputIndexIsCycleSizeCoordinateIsCycleMult) const
{ MacroRegisterFunctionWithName("ElementWeylGroup::GetCycleStructure");
  this->CheckInitialization();
  outputIndexIsCycleSizeCoordinateIsCycleMult.MakeZero();
  List<bool> Explored;
  HashedList<Vector<Rational> >& theRootSystem = this->owner->RootSystem;
  Explored.initFillInObject(theRootSystem.size, false);
  Vector<Rational> currentRoot;
  for (int i = 0; i < Explored.size; i ++)
    if (!Explored[i])
    { int currentCycleSize = 1;
      currentRoot = theRootSystem[i];
      for (this->owner->ActOn(*this, currentRoot, currentRoot); currentRoot != theRootSystem[i]; this->owner->ActOn(*this, currentRoot, currentRoot))
      { currentCycleSize ++;
        Explored[theRootSystem.GetIndex(currentRoot)] = true;
      }
      outputIndexIsCycleSizeCoordinateIsCycleMult.AddMonomial(MonomialVector(currentCycleSize - 1), 1);
    }
}

template <class templateWeylGroup>
void ElementWeylGroup<templateWeylGroup>::GetCharacteristicPolyStandardRepresentation(Polynomial<Rational>& output) const
{ MacroRegisterFunctionWithName("ElementWeylGroup::GetCharacteristicPolyStandardRepresentation");
  this->CheckInitialization();
  Matrix<Rational> standardRepMat;
  this->owner->GetMatrixStandardRep(*this, standardRepMat);
  output.AssignCharPoly(standardRepMat);
}

template <class templateWeylGroup>
ElementWeylGroup<WeylGroupData> ElementWeylGroup<templateWeylGroup>::Inverse() const
{ ElementWeylGroup<WeylGroupData> out = *this;
  out.generatorsLastAppliedFirst.ReverseOrderElements();
  out.MakeCanonical();
  return out;
}

template <class elementGroup, class elementRepresentation>
void OrbitIterator<elementGroup, elementRepresentation>::init
(const List<elementGroup>& inputGenerators, const elementRepresentation& inputElement, OrbitIterator::GroupAction inputGroupAction)
{ this->reset();
  this->theGroupGeneratingElements = inputGenerators;
  this->theGroupAction = inputGroupAction;
  this->currentLayer->Clear();
  this->currentLayer->AddOnTop(inputElement);
  this->indexCurrentElement = 0;
  this->previousLayer->Clear();
  this->nextLayer->Clear();
}

template <class elementGroup, class elementRepresentation>
const elementRepresentation& OrbitIterator<elementGroup, elementRepresentation>::GetCurrentElement()
{ return (*this->currentLayer)[this->indexCurrentElement];
}

template <class elementGroup, class elementRepresentation>
std::string OrbitIterator<elementGroup, elementRepresentation>::ToStringLayerSize() const
{ std::stringstream out;
  out << " Total number of elements stored in layers: " << this->privateLayer1.size + this->privateLayer2.size + this->privateLayer3.size;
  out << " Previous layer: " << this->previousLayer->size << "; current layer: " << this->currentLayer->size
  << "; next layer: " << this->nextLayer->size;
  return out.str();
}

template <class elementGroup, class elementRepresentation>
bool OrbitIterator<elementGroup, elementRepresentation>::IncrementReturnFalseIfPastLast()
{ MacroRegisterFunctionWithName("OrbitIterator::IncrementReturnFalseIfPastLast");
  if (this->theGroupGeneratingElements.size == 0)
    return false;
  for (int i = 0; i < this->theGroupGeneratingElements.size; i ++)
  { this->theGroupAction
    (this->theGroupGeneratingElements[i], (*this->currentLayer)[this->indexCurrentElement],
     this->bufferRepresentationElement);
    if (!this->currentLayer->Contains(this->bufferRepresentationElement))
      this->currentLayer->AddOnTopNoRepetition(this->bufferRepresentationElement);
  }
  this->indexCurrentElement ++;
  if (this->indexCurrentElement < this->currentLayer->size)
    return true;
  this->indexCurrentElement = 0;
  return false;
}

template <class elementGroup, class elementRepresentation>
bool OrbitIterator<elementGroup, elementRepresentation>::IncrementReturnFalseIfPastLastFALSE()
{ MacroRegisterFunctionWithName("OrbitIterator::IncrementReturnFalseIfPastLast");
  if (this->theGroupGeneratingElements.size == 0)
    return false;
  for (int i = 0; i < this->theGroupGeneratingElements.size; i ++)
  { this->theGroupAction
    (this->theGroupGeneratingElements[i], (*this->currentLayer)[this->indexCurrentElement],
     this->bufferRepresentationElement);
    if (!this->previousLayer->Contains(this->bufferRepresentationElement) &&
        !this->currentLayer->Contains(this->bufferRepresentationElement))
      this->nextLayer->AddOnTopNoRepetition(this->bufferRepresentationElement);
  }
  this->indexCurrentElement ++;
  if (this->indexCurrentElement < this->currentLayer->size)
    return true;
  if (this->nextLayer->size == 0)
    return false;
  HashedList<elementRepresentation>* layerPtr = this->previousLayer;
  this->previousLayer = this->currentLayer;
  this->currentLayer = this->nextLayer;
  this->nextLayer = layerPtr;
  this->nextLayer->Clear();
  this->indexCurrentElement = 0;
  return true;
}

template <class elementSomeGroup>
void FiniteGroup<elementSomeGroup>::init()
{ this->generators.SetSize(0);
  this->conjugacyClasseS.SetSize(0);
  this->squaresCCReps.SetSize(0);
  this->theElements.Clear();
  this->flagAllElementsAreComputed = false;
  this->flagCCsComputed = false;
  this->flagCCRepresentativesComputed = false;
  this->flagCharPolysAreComputed = false;
  this->flagGeneratorsConjugacyClassesComputed = false;
  this->flagWordsComputed = false;
  this->flagCharTableIsComputed = false;
  this->flagIrrepsAreComputed = false;
  this->sizePrivate = 0;
  this->specificDataPointer = 0;
  this->AreConjugateByFormula = 0;
  this->ComputeCCSizesAndRepresentativesByFormula = 0;
  this->GetSizeByFormula = 0;
  this->GetWordByFormula = 0;
}

template <class someGroup, class elementSomeGroup>
void SubgroupData<someGroup, elementSomeGroup>::initFromGroupAndGenerators
(someGroup& inputGroup, const List<elementSomeGroup>& inputGenerators)
{ MacroRegisterFunctionWithName("Subgroup::initFromGroupAndGenerators");
  if (&inputGenerators == &this->theSubgroup->generators)//<-handle naughty programmers
  { List<elementSomeGroup> inputGeneratorsCopy = inputGenerators;
    this->initFromGroupAndGenerators(inputGroup, inputGeneratorsCopy);
    return;
  }
  this->theGroup = &inputGroup;
  this->theSubgroup = &this->theSubgroupMayBeHere;
  this->theSubgroup->generators.SetSize(0);
  this->theSubgroup->generators.AddOnTopNoRepetition(inputGenerators);//<- we have eliminated all repeating generators
  //(there shouldn't be any, but knowing I am one of the programmers...)
}

// A well chosen generating set for G can have the property that for interesting subgroups H,
// the usual choice of word in G for h∈H will be translatable into letters of H.
// This is the case for, in particular, the parabolic subgroups of the complete hyperoctahedral
// groups in the generating set that tacks on to the symmetric groups a complete set of ℤ₂ generators
template <class someGroup, class elementSomeGroup>
void SubgroupData<someGroup, elementSomeGroup>::MakeTranslatableWordsSubgroup
(someGroup &inputGroup, const List<elementSomeGroup>& subGenerators)
{ MacroRegisterFunctionWithName("Subgroup::MakeTranslatableWordsSubgroup");
  this->theGroup = &inputGroup;
  this->theSubgroup = &this->theSubgroupMayBeHere;
  this->theSubgroup->generators = subGenerators;
  this->superGeneratorSubWords.SetSize(this->theGroup->generators.size);
  this->superGeneratorSubWordExists.SetSize(this->theGroup->generators.size);
  for (int i = 0; i < superGeneratorSubWordExists.size; i ++)
    superGeneratorSubWordExists[i] = false;
  // done initializing things.  Now for actual code.
  // not the most comprehensive algorithm, is it?
  // I mean, not that I care to prove it incomplete at this point.
  // and so much work for this silly little speed optimization, too
  for (int i = 0; i < this->theGroup->generators.size; i ++)
  { int si = this->theSubgroup->generators.GetIndex(this->theGroup->generators[i]);
    if (si != - 1)
    { this->superGeneratorSubWords[i].AddOnTop(si);
      this->superGeneratorSubWordExists[i] = true;
    }
  }
  this->theSubgroup->GetWordByFormula = TranslatableWordsSubgroupElementGetWord<elementSomeGroup>;
  this->theSubgroup->parentRelationship = this;
  //stOutput << "SubgroupData::MakeTranslatableWordsSubgroup:\n";
  //for (int i = 0; i < this->theSubgroup->generators.size; i ++)
  //  stOutput << i << " " << this->theSubgroup->generators[i] << " "
  //           << this->superGeneratorSubWords[i].ToStringCommaDelimited() << '\n';
  //for (int i = 0; i < this->theSubgroup->generators.size; i ++)
  //  stOutput << i << " " << this->theSubgroup->generators[i] << '\n';
}

template <class someGroup, class elementSomeGroup>
bool SubgroupData<someGroup, elementSomeGroup>::CheckInitialization()
{ if (this == 0)
    crash << "A subgroup has the ``this'' pointer equal to zero. " << crash;
  if (this->theGroup == 0)
    crash << "This is a programming error: subgroup not initialized when it should be" << crash;
  return true;
}

template <typename elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::HasElement(const elementSomeGroup& g)
{ if (!this->flagAllElementsAreComputed)
    this->ComputeAllElements(false);
  return this->theElements.Contains(g);
}

template <class elementSomeGroup>
void FiniteGroup<elementSomeGroup>::GetSignCharacter(Vector<Rational>& outputCharacter)
{ MacroRegisterFunctionWithName("FiniteGroup::GetSignCharacter");
  if (!this->flagCCsComputed)
    this->ComputeCCSizesAndRepresentatives();
  outputCharacter.SetSize(this->ConjugacyClassCount());
  for (int i = 0; i < this->ConjugacyClassCount(); i ++)
    outputCharacter[i] = this->conjugacyClasseS[i].representative.Sign();
}

template <class someGroup, class elementSomeGroup>
void SubgroupData<someGroup, elementSomeGroup>::init()
{ this->theGroup = 0;
  this->theSubgroup = 0;
  this->theSubgroupMayBeHere.init();
  this->ccRepresentativesPreimages.SetSize(0);
  this->generatorPreimages.SetSize(0);
  this->flagCosetSetsComputed = false;
  this->flagCosetRepresentativesComputed = false;
}

template <class someGroup, class elementSomeGroup>
SubgroupData<someGroup, elementSomeGroup>::SubgroupData()
{ this->init();
}

template <typename elementSomeGroup>
SubgroupData<FiniteGroup<elementSomeGroup>, elementSomeGroup> FiniteGroup<elementSomeGroup>::ParabolicKindaSubgroupGeneratorSubset(const List<int>& subgenids)
{ SubgroupData<FiniteGroup<elementSomeGroup>, elementSomeGroup> out;
  List<elementSomeGroup> subgens;
  subgens.SetSize(subgenids.size);
  for (int i = 0; i < subgenids.size; i ++)
    subgens[i] = this->generators[subgenids[i]];
  out.MakeTranslatableWordsSubgroup(*this, subgens);
  return out;
}

template <typename elementSomeGroup>
GroupRepresentation<FiniteGroup<elementSomeGroup>, Rational> FiniteGroup<elementSomeGroup>::GetEmptyRationalRepresentation()
{ GroupRepresentation<FiniteGroup<elementSomeGroup>, Rational> out;
  out.ownerGroup = this;
  return out;
}



template <class elementSomeGroup>
template <typename coefficient>
coefficient FiniteGroup<elementSomeGroup>::GetHermitianProduct
(const Vector<coefficient>& X1, const Vector<coefficient>& X2)
{ coefficient acc = 0;
  for (int i = 0; i < X1.size; i ++)
  { acc += MathRoutines::ComplexConjugate(X1[i]) * X2[i] * this->conjugacyClasseS[i].size;
    if (this->conjugacyClasseS[i].size == 0)
      crash << "Error: conjugacy class size is zero." << crash;
  }
  return acc / this->GetSize();
}

template <class elementSomeGroup>
LargeInt FiniteGroup<elementSomeGroup>::GetSize()
{ this->CheckConsistency();
  if (this->sizePrivate > 0)
    return sizePrivate;
  if (this->GetSizeByFormula != 0)
  { this->sizePrivate = this->GetSizeByFormula(*this);
    return sizePrivate;
  }
  //crash << "Requesting size of group whose size is not computed.  If you want"
  //      << "to compute it as a side effect, change the stuff near "
  //      << __FILE__ << ":" << __LINE__ << crash;
  this->ComputeAllElements(false);
  return this->sizePrivate;
}

template <class elementSomeGroup>
int FiniteGroup<elementSomeGroup>::ConjugacyClassCount() const
{ if (!this->flagCCRepresentativesComputed)
    crash << "Requesting conjugacy class count but conjugacy class representatives are not computed." << crash;
  return this->conjugacyClasseS.size;
}

template <class someGroup, class elementSomeGroup>
void SubgroupData<someGroup, elementSomeGroup>::ComputeCCRepresentativesPreimages()
{ MacroRegisterFunctionWithName("Subgroup::ComputeCCRepresentativesPreimages");
  this->ccRepresentativesPreimages.SetSize(this->theSubgroup->ConjugacyClassCount());
  if (this->theSubgroup->generators.size == 0)
    crash << "At this computation the group must have initialized generators. " << crash;
  if (this->theGroup->generators.size == 0)
    crash << "Parent group must have initialized generators. " << crash;
  for (int i = 0; i < this->theSubgroup->conjugacyClasseS.size; i ++)
  { elementSomeGroup& scr = this->theSubgroup->conjugacyClasseS[i].representative;
    bool notFound = true;
    for (int j = 0; notFound && j < this->theGroup->conjugacyClasseS.size; j ++)
    { elementSomeGroup& gcr = this->theGroup->conjugacyClasseS[j].representative;
      if (this->theGroup->AreConjugate(scr,gcr))
      { this->ccRepresentativesPreimages[i] = j;
        //stOutput << "I think that " << scr << " is conjugate to " << gcr << '\n';
        notFound = false;
        break;
      }
    }
    if (notFound)
      crash << "Programming error: couldn't find preimage of the subgroup conjugacy class representative " << scr << crash;
  }
}

template <class elementSomeGroup>
std::string FiniteGroup<elementSomeGroup>::ToStringElements(FormatExpressions* theFormat) const
{ MacroRegisterFunctionWithName("FiniteGroup::ToStringElements");
  (void) theFormat;//avoid unused parameter warning, portable
  if (!this->flagAllElementsAreComputed)
    return "";
  std::stringstream out;
  out << "<br>Elements of the group(" << this->theElements.size << " total):\n ";
  if (this->theElements.size <= 100)
    for (int i = 0; i < this->theElements.size; i ++)
      out << i << ". " << this->theElements[i].ToString() << "\n";
  else
    out << "... too many, not displaying. ";
  return out.str();
}

template <class elementSomeGroup>
std::string FiniteGroup<elementSomeGroup>::ToStringConjugacyClasses(FormatExpressions* theFormat)
{ MacroRegisterFunctionWithName("Subgroup::ToStringConjugacyClasses");
  std::stringstream out;
  out << "<br>Size: " << this->GetSize().ToString() << "\n";
  FormatExpressions charPolyFormat;
  charPolyFormat.polyAlphabeT.SetSize(1);
  charPolyFormat.polyAlphabeT[0] = "q";
  //  out <<"Number of Vectors<Rational>: "<< this->RootSystem.size<<"\n
  if (this->ConjugacyClassCount() > 0)
  { out << "<br>" << this->ConjugacyClassCount() << " conjugacy classes total.\n";
    for (int i = 0; i < this->conjugacyClasseS.size; i ++)
    { out << "<hr>Conjugacy class " << i + 1 << ": ";
      if (this->conjugacyClasseS[i].flagRepresentativeComputed)
      { out << " represented by " << this->conjugacyClasseS[i].representative.ToString(theFormat) << ". ";
        //out << this->conjugacyClasseS[i].ToStringInvariants(theFormat); FIXME: do this sanely
      } else
        out << " representative not computed. ";
      out << "Class size: " << this->conjugacyClasseS[i].size.ToString() << ".\n<br>\n";
      if (this->flagCharPolysAreComputed)
        if (i < this->CCsStandardRepCharPolys.size)
        { out << "Characteristic poly standard representation: "
          << this->CCsStandardRepCharPolys[i].ToString(&charPolyFormat);
          const List<int>& currentHashList =
          this->CCsStandardRepCharPolys.GetHashArray
          (this->CCsStandardRepCharPolys.GetHash(this->CCsStandardRepCharPolys[i]));
          int numClassesSameCharPoly = 0;
          for (int j = 0; j < currentHashList.size; j ++)
            if (this->CCsStandardRepCharPolys[currentHashList[j]] == this->CCsStandardRepCharPolys[i])
              numClassesSameCharPoly ++;
          if (numClassesSameCharPoly > 1)
          { out << " The characteristic polynomial is the same as that of " << numClassesSameCharPoly
            << " conjugacy classes, numbers: ";
            for (int j = 0; j < currentHashList.size; j ++)
              if (this->CCsStandardRepCharPolys[currentHashList[j]] == this->CCsStandardRepCharPolys[i])
                out << currentHashList[j] + 1 << (j == currentHashList.size - 1 ? "" : ", ");
          }
        }
      if (!this->conjugacyClasseS[i].flagElementsComputed)
        continue;
      out << " The elements of the class are: ";
      int numEltsToDisplay = this->conjugacyClasseS[i].theElements.size;
      if (this->conjugacyClasseS[i].theElements.size > 10)
      { out << " too many, displaying the first 10 elements only: ";
        numEltsToDisplay = 10;
      }
      for (int j = 0; j < numEltsToDisplay; j ++)
      { out << this->conjugacyClasseS[i].theElements[j].ToString(theFormat);
        if (j != numEltsToDisplay - 1)
          out << ", ";
      }
      out << ". ";
    }
  }
  return out.str();
}

template <class elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::CheckInitialization() const
{ this->CheckConsistency();
  if (this->generators.size == 0)
    crash << "Error: group has 0 generators, which is not allowed. If you want to use the trivial "
    << "group, you are still supposed to put the identity element in the group generators. " << crash;
  return true;
}

template <class elementSomeGroup>
void FiniteGroup<elementSomeGroup>::ComputeGeneratorsConjugacyClasses()
{ MacroRegisterFunctionWithName("FiniteGroup::ComputeGeneratorsConjugacyClasses");
  if (this->flagGeneratorsConjugacyClassesComputed)
    return;
  this->unionGeneratorsCC.Clear();
  this->unionGeneratorsCC.AddOnTopNoRepetition(this->generators);
  elementSomeGroup currentElt;
  for (int i = 0; i < this->unionGeneratorsCC.size; i ++)
    for (int j = 0; j < this->generators.size; j ++)
    { elementSomeGroup::ConjugationAction(this->generators[j], this->unionGeneratorsCC[i], currentElt);
      this->unionGeneratorsCC.AddOnTopNoRepetition(currentElt);
    }
  this->flagGeneratorsConjugacyClassesComputed = true;
}

template <class elementSomeGroup>
void FiniteGroup<elementSomeGroup>::ComputeCCSizeOrCCFromRepresentative
(ConjugacyClass& inputOutputClass, bool storeCC)
{ MacroRegisterFunctionWithName("FiniteGroup::ComputeCCSizesFromCCRepresentatives");
  OrbitIterator<elementSomeGroup, elementSomeGroup> theOrbitIterator;
  theOrbitIterator.init(this->generators, inputOutputClass.representative, elementSomeGroup::ConjugationAction);
  inputOutputClass.size = 1;
  if (storeCC)
  { inputOutputClass.theElements.SetSize(0);
    inputOutputClass.theElements.AddOnTop(inputOutputClass.representative);
  }
/*  stOutput << "<br> Generating orbit: <br>Element 1: " << theOrbitIterator.GetCurrentElement().ToString();
  Vector<Rational> randomVector, tempV;
  randomVector.AssignString("(23,29,31,37,41,43, 47)");
  HashedList<Vector<Rational> > orbitRV;
  tempV=randomVector;
  theOrbitIterator.GetCurrentElement().ActOn(tempV);
  orbitRV.AddOnTop(tempV);*/
  while (theOrbitIterator.IncrementReturnFalseIfPastLast())
  { inputOutputClass.size ++;
/*    stOutput << "<br>Element " << inputOutputClass.size.ToString() << ": "
    << theOrbitIterator.GetCurrentElement().ToString();
    tempV=randomVector;
    theOrbitIterator.GetCurrentElement().ActOn(tempV);
    if (orbitRV.Contains(tempV))
    {
      crash << "Element " << theOrbitIterator.GetCurrentElement().ToString()
      << " is bad, has same action on " << randomVector.ToString() << " as " <<
      inputOutputClass.theElements[orbitRV.GetIndex(tempV)].ToString() << crash
      ;
    }
    orbitRV.AddOnTop(tempV);*/
    if (storeCC)
    { if (inputOutputClass.theElements.Contains(theOrbitIterator.GetCurrentElement()))
        crash << " !element " << theOrbitIterator.GetCurrentElement().ToString()
        << " already contained !" << crash;
      inputOutputClass.theElements.AddOnTop(theOrbitIterator.GetCurrentElement());
    }
  }
}

template <class elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::CheckConjugacyClassRepsMatchCCsizes()
{ MacroRegisterFunctionWithName("FiniteGroup::CheckConjugacyClassRepsMatchCCsizes");
  LargeInt computedSize = 0;
  for (int i = 0; i < this->conjugacyClasseS.size; i ++)
  { LargeInt oldCCsize = this->conjugacyClasseS[i].size;
    this->ComputeCCSizeOrCCFromRepresentative(this->conjugacyClasseS[i], true);
    if (oldCCsize != this->conjugacyClasseS[i].size)
      crash << "The precomputed size " << oldCCsize.ToString() << " of the class represented by " << this->conjugacyClasseS[i].representative.ToString()
      << " doesn't match actual class size which is: " << this->conjugacyClasseS[i].size.ToString()
      << crash;
    computedSize += oldCCsize;
  }
  if (computedSize != this->sizePrivate)
    crash << "Computed size " << computedSize.ToString() << " is different from recorded size "
    << sizePrivate.ToString() << crash;
  if (this->SizeByFormulaOrNeg1() > 0)
    if (computedSize != this->SizeByFormulaOrNeg1())
      crash << "Computed size is different from size dicated by formula which is: "
      << this->SizeByFormulaOrNeg1().ToString() << crash;
  return true;
}

template <class elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::RegisterCCclass
(const elementSomeGroup& theRepresentative, bool dontAddIfSameInvariants)
{ MacroRegisterFunctionWithName("FiniteGroup::RegisterCCclass");
  ConjugacyClass theClass;
  theClass.representative = theRepresentative;
  Polynomial<Rational> theCharPoly;
  theClass.representative.GetCharacteristicPolyStandardRepresentation(theCharPoly);
  if (this->CCsStandardRepCharPolys.Contains(theCharPoly))
  { const List<int>& indicesPossibleConjugates =
    this->CCsStandardRepCharPolys.GetHashArray(this->CCsStandardRepCharPolys.GetHash(theCharPoly));
    for (int i = 0; i < indicesPossibleConjugates.size; i ++)
    { elementSomeGroup& otherRepresentative = this->conjugacyClasseS[indicesPossibleConjugates[i]].representative;
      if (!dontAddIfSameInvariants)
      { if (this->AreConjugate(theClass.representative, otherRepresentative))
          return false;
      } else
        if (!theClass.representative.HasDifferentConjugacyInvariantsFrom(otherRepresentative))
          return false;
    }
  }
  theClass.flagRepresentativeComputed = true;
  this->ComputeCCSizeOrCCFromRepresentative(theClass, false);
  this->conjugacyClasseS.AddOnTop(theClass);
  this->CCsStandardRepCharPolys.AddOnTop(theCharPoly);
  this->sizePrivate += theClass.size;
  this->conjugacyClasseS.QuickSortAscending();
  return true;
}

template <class elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::ComputeCCRepresentatives()
{ MacroRegisterFunctionWithName("FiniteGroup::ComputeCCRepresentatives");
  //This algorithm is effective if the sum of the sizes of the conjugacy classes
  //of the generators is small.
  //The algorithm "hops" from conjugacy class representative to conjugacy representative
  //by multiplying the representatives by the each element of each conjugacy class of the generators.
  //We prove this algorithm is guaranteed to generate all conjugacy classes.
  //Let g be a generator, H a conjugacy class, and h \in H an arbitrary element of H.
  //Suppose gH intersects a conjugacy class F. We claim there exists a generator g and an
  //element x such that x g x^{- 1} h\in F. Indeed, as gH intersects F, g x^{- 1} h x\in F for some
  //element x, and therefore x g x^{- 1} h =x(g x^{- 1} h x) x^{- 1}\in F, which proves our claim.
  //The preceding considerations show that
  //a representative of each conjugacy class containing an element from g H can be obtained
  //by applying the entire conjugacy class of g onto an arbitrary representative of H.
  //As we are using the conjugacy classes of a set of generators, this shows that the following
  //algorithm will generate a representative of each conjugacy class.

  //First we compute the generator's conjugacy classes:
  this->ComputeGeneratorsConjugacyClasses();

  ProgressReport theReport;
  elementSomeGroup currentElement;
  LargeInt groupSizeByFla = this->SizeByFormulaOrNeg1();
  this->flagCharPolysAreComputed = true;
  for (int phase = 0; phase < 2; phase ++)//In phase 0 we try to add a new conjugacy class only if
    //the class has a new character polynomial. In phase 1 we try every single conjugacy class,
    //in case there are two non-conjugate elements with the same char poly.
    for (int i = 0; i < this->conjugacyClasseS.size; i ++)
      for (int j = 0; j < this->unionGeneratorsCC.size; j ++)
      { if (theGlobalVariables.flagReportEverything)
        { std::stringstream reportStream;
          reportStream << "Exploring conjugacy class " << i + 1
          << " out of " << this->conjugacyClasseS.size
          << " generator " << j + 1 << " out of " << this->unionGeneratorsCC.size;
          theReport.Report(reportStream.str());
        }
        currentElement = this->conjugacyClasseS[i].representative * this->unionGeneratorsCC[j];
        this->RegisterCCclass(currentElement, phase == 0);
        if (this->sizePrivate == groupSizeByFla)
          return true;
      }
  if (groupSizeByFla > 0)
    if (this->sizePrivate != groupSizeByFla)
      crash << "Something went very wrong: number of elements generated is not equal to group size by formula. " << crash;
  return true;
}

template <class elementSomeGroup>
void FiniteGroup<elementSomeGroup>::ComputeCCSizesAndRepresentatives
(bool useComputeCCSizesRepresentativesWords)
{ MacroRegisterFunctionWithName("FiniteGroup::ComputeCCSizesAndRepresentatives");
  this->CheckConsistency();
  if (this->GetSizeByFormula != 0)
  { LargeInt theSize = this->GetSizeByFormula(*this);
    // extended digit separators only appear in cxx14
    if (theSize > 100000000)
      if (this->flagCanComputeCCsWithOrbitIterator)
      { this->ComputeCCSizesAndRepresentativesWithOrbitIterator();
        return;
      }
  }
  if (useComputeCCSizesRepresentativesWords)
    this->ComputeCCSizesRepresentativesWords();
}

template <class elementSomeGroup>
void FiniteGroup<elementSomeGroup>::ComputeCCSizesAndRepresentativesWithOrbitIterator()
{ MacroRegisterFunctionWithName("FiniteGroup::ComputeCCSizesAndRepresentativesWithOrbitIterator");
  if (this->flagCCRepresentativesComputed)
    return;

  this->InitGenerators();
  this->CheckInitialization();
  this->CCsStandardRepCharPolys.Clear();
  this->conjugacyClasseS.SetSize(0);
  this->sizePrivate = 0;

  elementSomeGroup currentElt;
  currentElt.MakeID(this->generators[0]);
  static int recursionCount = 0;
  recursionCount ++;
  if (recursionCount > 100)
    crash << "Oh no something is very wrong " << crash;
  std::cout << "got to here!\n";
  this->RegisterCCclass(currentElt, false);
  std::cout << "got to here2!\n";
  this->ComputeCCRepresentatives();
  recursionCount --;
  std::cout << "got to here3!\n";
  this->flagCCRepresentativesComputed = true;
}

template <class elementSomeGroup>
void FiniteGroup<elementSomeGroup>::ComputeCCfromCCindicesInAllElements(const List<List<int> >& ccIndices)
{ MacroRegisterFunctionWithName("FiniteGroup::ComputeCCfromCCindicesInAllElements");
  this->conjugacyClasseS.SetSize(ccIndices.size);
  for (int i = 0; i < ccIndices.size; i ++)
  { this->conjugacyClasseS[i].size = ccIndices[i].size;
    this->conjugacyClasseS[i].indicesEltsInOwner = ccIndices[i];
    this->conjugacyClasseS[i].theElements.SetSize(ccIndices[i].size);
    for (int j = 0; j < ccIndices[i].size; j ++)
      this->conjugacyClasseS[i].theElements[j] = this->theElements[ccIndices[i][j]];
    this->conjugacyClasseS[i].representative = this->theElements[ccIndices[i][0]];
    this->conjugacyClasseS[i].flagRepresentativeComputed = true;
    this->conjugacyClasseS[i].flagElementsComputed = true;
  }
  this->flagCCsComputed = true;
  this->flagCCRepresentativesComputed = true;
  this->conjugacyClasseS.QuickSortAscending();
}

template <class elementSomeGroup>
void FiniteGroup<elementSomeGroup>::ComputeCCfromAllElements()
{ MacroRegisterFunctionWithName("FiniteGroup::ComputeCCfromAllElements");
  this->ComputeAllElements(false, - 1);
  List<bool> Accounted;
  Accounted.initFillInObject(this->theElements.size, false);
  HashedList<int, MathRoutines::IntUnsignIdentity> theStack;
  theStack.SetExpectedSize(this->theElements.size);
  List<elementSomeGroup> inversesOfGenerators;
  inversesOfGenerators.SetSize(this->generators.size);
  for (int i = 0; i < this->generators.size; i ++)
    inversesOfGenerators[i] = this->generators[i].Inverse();
  elementSomeGroup currentElement;
  List<List<int> > ccIndices;
  ccIndices.Reserve(120);
  for (int i = 0; i < this->theElements.size; i ++)
    if (!Accounted[i])
    { theStack.Clear();
      theStack.AddOnTop(i);
      for (int j = 0; j < theStack.size; j ++)
        for (int k = 0; k < this->generators.size; k ++)
        { currentElement = inversesOfGenerators[k] * this->theElements[theStack[j]] * this->generators[k];
          int accountedIndex = this->theElements.GetIndexIMustContainTheObject(currentElement);
          theStack.AddOnTopNoRepetition(accountedIndex);
          Accounted[accountedIndex] = true;
        }
      ccIndices.AddOnTop(theStack);
      ccIndices.LastObject()->QuickSortAscending();
    }
  ccIndices.QuickSortAscending();
  this->ComputeCCfromCCindicesInAllElements(ccIndices);
}

template<class coefficient>
LargeInt WeylGroupData::GetOrbitSize(Vector<coefficient>& theWeight)
{ MacroRegisterFunctionWithName("WeylGroup::GetOrbitSize");
  //I read somewhere, I think it was a paper by W. de Graaf, that the stabilizer
  //of a weight is generated by
  //the root reflections that stabilize the weight.
  //Whether my memory has served me well shall be seen through implementing this function.
  //In particular, we will compute all root reflections that stabilize the weight,
  //then get a Dynkin diagram from these roots, then compute the size of the stabilizer,
  // and finally compute the size of the orbit. I will check numerically if everything is ok
  //all the way up to E6.
//  stOutput << "<hr>DEBUG: Calling WeylGroup::GetOrbitSize with input: " << theWeight.ToString()
//  << ". The Weyl type is: " << this->theDynkinType.ToString();
  Vector<coefficient> currentWeight;
  Vectors<Rational> theStabilizingRoots;
  for (int i = 0; i < this->RootsOfBorel.size; i ++)
  { this->ReflectBetaWRTAlpha(this->RootsOfBorel[i], theWeight, false, currentWeight);
    if (currentWeight == theWeight)
      theStabilizingRoots.AddOnTop(this->RootsOfBorel[i]);
  }
//  stOutput << "<br>DEBUG: I found the stabilizing roots to be: " << theStabilizingRoots.ToString();
  DynkinDiagramRootSubalgebra theStabilizerSubsystem;
  theStabilizerSubsystem.ComputeDiagramTypeModifyInput(theStabilizingRoots, *this);
//  stOutput << "<br>DEBUG: The stabilizer subsystem is: " << theStabilizerSubsystem.ToString();
  DynkinType theStabilizerDynkinType;
  theStabilizerSubsystem.GetDynkinType(theStabilizerDynkinType);
  Rational resultRat = this->theGroup.GetSize();
  resultRat /= theStabilizerDynkinType.GetWeylGroupSizeByFormula();
  LargeInt result;
  if (!resultRat.IsInteger(&result))
    crash << "Something has gone very wrong: orbit size reported to be " << resultRat.ToString()
    << " which is non-integer!" << crash;
  bool doDebug = true;
  if (doDebug)
    if (result < 100000)
    { HashedList<Vector<coefficient> > comparisonOrbit;
//      double DEbugStartTime= theGlobalVariables.GetElapsedSeconds();
//      stOutput << "<br>DEBUG: calling generateOrbit for input: " << theWeight.ToString();
      this->GenerateOrbit(theWeight, false, comparisonOrbit, false, - 1, 0, - 1);
//      stOutput << "<br>Orbit generated in " << theGlobalVariables.GetElapsedSeconds()-DEbugStartTime << " seconds. ";
      if (result != comparisonOrbit.size)
        crash << "Actual orbit of " << theWeight.ToString() << " has size " << comparisonOrbit.size << " but I computed "
        << " the orbit size to be " << result.ToString() << ". This may be a mathematical error. " << crash;
    }
  return result;
}

template <class coefficient>
bool WeylGroupData::GenerateOuterOrbit
(Vectors<coefficient>& theWeights, HashedList<Vector<coefficient> >& output,
 HashedList<ElementWeylGroup<WeylGroupData> >* outputSubset,  int UpperLimitNumElements,
 GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("WeylGroup::GenerateOuterOrbit");
  this->ComputeOuterAutoGenerators();
  List<MatrixTensor<Rational> > theOuterGens = this->theOuterAutos.GetElement().theGenerators;
  output.Clear();
  for (int i = 0; i < theWeights.size; i ++)
    output.AddOnTop(theWeights[i]);
  Vector<coefficient> currentRoot;
  ElementWeylGroup<WeylGroupData> currentElt;
  int numElementsToReserve = MathRoutines::Minimum(UpperLimitNumElements, 1000000);
  output.SetExpectedSize(numElementsToReserve);
  ProgressReport theReport;
  simpleReflectionOrOuterAuto theGen;
  if (outputSubset != 0)
  { currentElt.MakeID(*this);
    outputSubset->SetExpectedSize(numElementsToReserve);
    outputSubset->Clear();
    outputSubset->AddOnTop(currentElt);
  }
  int numGens = this->GetDim() + theOuterGens.size;
  int orbitSizeDiv10000 = 0;
  for (int i = 0; i < output.size; i ++)
    for (int j = 0; j < numGens; j ++)
    { if (j < this->GetDim())
      { currentRoot = output[i];
        this->SimpleReflection(j, currentRoot);
      } else
        theOuterGens[j - this->GetDim()].ActOnVectorColumn(output[i], currentRoot);
      if (output.AddOnTopNoRepetition(currentRoot))
        if (outputSubset != 0)
        { currentElt.MakeID(*this);
          theGen.MakeSimpleReflection(j);
          currentElt.generatorsLastAppliedFirst.AddOnTop(theGen);
          currentElt.generatorsLastAppliedFirst.AddListOnTop((*outputSubset)[i].generatorsLastAppliedFirst);
          outputSubset->AddOnTop(currentElt);
        }
      if (UpperLimitNumElements > 0)
        if (output.size >= UpperLimitNumElements)
          return false;
      if (theGlobalVariables != 0)
      { if (output.size / 10000 > orbitSizeDiv10000)
        { std::stringstream reportStream;
          reportStream << "Generating outer orbit, " << output.size
          << " elements found so far, Weyl group type: "
          << this->theDynkinType.ToString() << ". ";
          theReport.Report(reportStream.str());
        }
        orbitSizeDiv10000 = output.size / 10000;
      }
    }
  return true;
}

template <class coefficient>
void WeylGroupData::RaiseToDominantWeight
(Vector<coefficient>& theWeight, int* sign, bool* stabilizerFound, ElementWeylGroup<WeylGroupData>* raisingElt)
{ MacroRegisterFunctionWithName("WeylGroup::RaiseToDominantWeight");
  if (sign != 0)
    *sign = 1;
  if (stabilizerFound != 0)
    *stabilizerFound = false;
  coefficient theScalarProd;
  int theDim = this->GetDim();
  simpleReflectionOrOuterAuto theGen;
  if (raisingElt != 0)
    raisingElt->MakeID(*this);
  for (bool found = true; found; )
  { found = false;
    for (int i = 0; i < theDim; i ++)
    { theScalarProd = this->GetScalarProdSimpleRoot(theWeight, i);
      if (theScalarProd.IsNegative())
      { found = true;
        theScalarProd *= 2;
        theScalarProd /= this->CartanSymmetric.elements[i][i];
        theWeight[i] -= theScalarProd;
        if (sign != 0)
          *sign *= - 1;
        theGen.MakeSimpleReflection(i);
        if (raisingElt != 0)
          raisingElt->generatorsLastAppliedFirst.AddOnTop(theGen);//warning order of raising element is reversed, must reverse back
      }
      if (stabilizerFound != 0)
        if (theScalarProd.IsEqualToZero())
          *stabilizerFound = true;
    }
  }
  if (raisingElt != 0)
  { raisingElt->generatorsLastAppliedFirst.ReverseOrderElements();
    raisingElt->MakeCanonical();
  }
//  stOutput << "<hr># simple reflections applied total: " << numTimesReflectionWasApplied;
}

template <class coefficient>
void WeylGroupData::SimpleReflectionRhoModified(int index, Vector<coefficient>& theVector) const
{ coefficient alphaShift, tempRat;
  alphaShift = 0;
  for (int i = 0; i < this->CartanSymmetric.NumCols; i ++)
  { tempRat = theVector[i];
    tempRat *= this->CartanSymmetric.elements[index][i] * (- 2);
    alphaShift += tempRat;
  }
  alphaShift /= this->CartanSymmetric.elements[index][index];
  alphaShift -= 1;
  theVector[index] += alphaShift;
}

template <class coefficient>
bool WeylGroupData::GenerateOrbit
(Vectors<coefficient>& theWeights, bool RhoAction, HashedList<Vector<coefficient> >& output,
 bool UseMinusRho, int expectedOrbitSize, HashedList<ElementWeylGroup<WeylGroupData> >* outputSubset,
 int UpperLimitNumElements)
{ MacroRegisterFunctionWithName("WeylGroup::GenerateOrbit");
  output.Clear();
//  stOutput << "<br>Generating orbit in type " << this->theDynkinType.ToString() << " of weights: "
//  << theWeights.ToString();
  for (int i = 0; i < theWeights.size; i ++)
    output.AddOnTopNoRepetition(theWeights[i]);
  Vector<coefficient> currentRoot;
  ElementWeylGroup<WeylGroupData> currentElt;
  if (expectedOrbitSize <= 0)
    if (!this->theGroup.GetSize().IsIntegerFittingInInt(&expectedOrbitSize))
      expectedOrbitSize = - 1;
  if (UpperLimitNumElements > 0 && expectedOrbitSize > UpperLimitNumElements)
    expectedOrbitSize = UpperLimitNumElements;
  if (expectedOrbitSize > 10000000 )
    expectedOrbitSize = 10000000;
  //stOutput << "<br>Setting expected orbit size: " << expectedOrbitSize;
  output.SetExpectedSize(expectedOrbitSize);
  //stOutput << "<br>Orbit size set successfully: " << expectedOrbitSize;
  if (outputSubset != 0)
  { if (UpperLimitNumElements > 0)
      expectedOrbitSize = MathRoutines::Minimum(UpperLimitNumElements, expectedOrbitSize);
    currentElt.MakeID(*this);
    outputSubset->SetExpectedSize(expectedOrbitSize);
    outputSubset->Clear();
    outputSubset->AddOnTop(currentElt);
  }
  ProgressReport theReport;
  simpleReflectionOrOuterAuto theGen;
  // stOutput << "<br>Got to main cycle of: WeylGroup::GenerateOrbit. ";
  for (int i = 0; i < output.size; i ++)
    for (int j = 0; j < this->CartanSymmetric.NumRows; j ++)
    { currentRoot = output[i];
      if (theGlobalVariables.flagReportEverything && i % 100 == 0)
      { std::stringstream reportStream;
        reportStream << "So far found " << i + 1 << " elements in the orbit(s) of the starting weight(s) "
        << theWeights.ToString() << ". ";
        theReport.Report(reportStream.str());
        //stOutput << reportStream.str();
      }
      if (!RhoAction)
        this->SimpleReflection(j, currentRoot);
      else if (!UseMinusRho)
        this->SimpleReflectionRhoModified(j, currentRoot);
      else
        this->SimpleReflectionMinusRhoModified(j, currentRoot);
      if (output.AddOnTopNoRepetition(currentRoot))
        if (outputSubset != 0)
        { currentElt.generatorsLastAppliedFirst.SetSize(1);
          theGen.MakeSimpleReflection(j);
          currentElt.generatorsLastAppliedFirst[0] = theGen;
          currentElt.generatorsLastAppliedFirst.AddListOnTop((*outputSubset)[i].generatorsLastAppliedFirst);
          currentElt.MakeCanonical();
          outputSubset->AddOnTop(currentElt);
        }
      if (UpperLimitNumElements > 0)
        if (output.size >= UpperLimitNumElements)
          return false;
    }
  return true;
}

template <class coefficient>
void WeylGroupData::SimpleReflectionMinusRhoModified(int index, Vector<coefficient>& theVector) const
{ coefficient alphaShift, tempRat;
  alphaShift = 0;
  for (int i = 0; i < this->CartanSymmetric.NumCols; i ++)
  { tempRat = theVector[i];
    tempRat *= this->CartanSymmetric.elements[index][i];
    alphaShift += tempRat;
  }
  alphaShift *= - 2;
  alphaShift /= this->CartanSymmetric.elements[index][index];
  alphaShift += 1;
  theVector[index] += alphaShift;
}

template <class coefficient>
void WeylGroupData::SimpleReflection(int index, Vector<coefficient>& theVector) const
{ if (index < 0 || index >= this->CartanSymmetric.NumCols)
    crash << "This is a programming error: simple reflection with respect to index " << index + 1 << " in a Weyl group of rank "
    << this->GetDim() << crash;
  coefficient alphaShift, tempRat;
  alphaShift = 0;
  for (int i = 0; i < this->CartanSymmetric.NumCols; i ++)
  { tempRat = theVector[i];
    tempRat *= this->CartanSymmetric.elements[index][i];
    alphaShift += tempRat;
  }
  alphaShift *= - 2;
  alphaShift /= this->CartanSymmetric.elements[index][index];
  theVector[index] += alphaShift;
}

template <class coefficient>
bool WeylGroupData::IsDominantWeight(const Vector<coefficient>& theWeight)
{ int theDimension = this->GetDim();
  for (int i = 0; i < theDimension; i ++)
    if (!this->IsDominantWRTgenerator(theWeight, i))
      return false;
  return true;
}

template<class coefficient>
coefficient WeylGroupData::WeylDimFormulaSimpleCoords(Vector<coefficient>& theWeightInSimpleCoords, const coefficient& theRingUnit)
{ coefficient Result, buffer;
  Vector<coefficient> rhoOverNewRing, rootOfBorelNewRing, sumWithRho;//<-to facilitate type conversion!
  rhoOverNewRing = this->rho;//<-type conversion here!
  Result = theRingUnit;
//  stOutput << "<br>doing the weyl dim formula with: " << theWeightInSimpleCoords.ToString();
//  stOutput << "<br>rho is:" << rhoOverNewRing.ToString();
//  stOutput << "<br>rho before conversion: " << this->rho.ToString();
//  stOutput << "<br>we get: ";
  for (int i = 0; i < this->RootsOfBorel.size; i ++)
  { rootOfBorelNewRing = this->RootsOfBorel[i]; //<-type conversion here!
    sumWithRho = rhoOverNewRing + theWeightInSimpleCoords;
    buffer = this->RootScalarCartanRoot(sumWithRho, rootOfBorelNewRing);
    buffer /= this->RootScalarCartanRoot(rhoOverNewRing, rootOfBorelNewRing);
//    stOutput << "(" << buffer.ToString() << ")";
    Result *= buffer;
  }
  return Result;
}

template<class coefficient>
Vector<coefficient> WeylGroupData::GetFundamentalCoordinatesFromSimple(const Vector<coefficient>& inputInFundamentalCoords)
{ Matrix<Rational>& tempMat = *this->GetMatrixSimpleToFundamentalCoords();
  Vector<coefficient> result = inputInFundamentalCoords;
  tempMat.ActOnVectorColumn(result);
  return result;
}

template<class coefficient>
Vector<coefficient> WeylGroupData::GetSimpleCoordinatesFromEpsilon(const Vector<coefficient>& inputInEpsCoords)
{ Vectors<Rational> simpleBasis, simpleBasisEpsCoords;
  simpleBasis.MakeEiBasis(this->GetDim());
  this->GetEpsilonCoords(simpleBasis, simpleBasisEpsCoords);
//  stOutput << "simple basis eps coords: " << simpleBasisEpsCoords.ToString();
  Vector<coefficient> result;
  result.SetSize(this->GetDim());
  for (int i = 0; i < simpleBasisEpsCoords.size; i ++)
    result[i] = simpleBasisEpsCoords[i].ScalarEuclidean(inputInEpsCoords);
  Matrix<Rational> invertedCartanSymmetric = this->CartanSymmetric;
  invertedCartanSymmetric.Invert();
  invertedCartanSymmetric.ActOnVectorColumn(result);
  return result;
}

template<class coefficient>
Vector<coefficient> WeylGroupData::GetFundamentalCoordinatesFromEpsilon(const Vector<coefficient>& inputInEpsCoords)
{ return this->GetFundamentalCoordinatesFromSimple(this->GetSimpleCoordinatesFromEpsilon(inputInEpsCoords));
}

template<class coefficient>
Vector<coefficient> WeylGroupData::GetSimpleCoordinatesFromFundamental(const Vector<coefficient>& inputInFundamentalCoords)
{ Matrix<Rational>& tempMat = *this->GetMatrixFundamentalToSimpleCoords();
  Vector<coefficient> result;
  result = inputInFundamentalCoords;
//  stOutput << "<br>transition matrix from fundamental to simple: " << tempMat.ToString();
  tempMat.ActOnVectorColumn(result);
  return result;
}

template<class coefficient>
Vectors<coefficient> WeylGroupData::GetSimpleCoordinatesFromFundamental(const Vectors<coefficient>& inputInFundamentalCoords)
{ Matrix<Rational>& tempMat = *this->GetMatrixFundamentalToSimpleCoords();
  Vectors<coefficient> result;
  result = inputInFundamentalCoords;
//  stOutput << "<br>transition matrix from fundamental to simple: " << tempMat.ToString();
  tempMat.ActOnVectorsColumn(result);
  return result;
}

template<class coefficient>
coefficient WeylGroupData::WeylDimFormulaFundamentalCoords(Vector<coefficient>& weightFundCoords)
{ Vector<coefficient> theWeightInSimpleCoords;
  theWeightInSimpleCoords = this->GetSimpleCoordinatesFromFundamental(weightFundCoords);
  return this->WeylDimFormulaSimpleCoords(theWeightInSimpleCoords);
}

template<class leftType, class rightType>
void WeylGroupData::RootScalarCartanRoot(const Vector<leftType>& r1, const Vector<rightType>& r2, leftType& output) const
{ if (r1.size != r2.size || r1.size != this->GetDim())
  { crash.theCrashReport << "This is a programming error: attempting to get the scalar product of the weight "
    << r1 << " (dimension " << r1.size
    << ") with the weight " << r2 << " (dimension " << r2.size << "), while the dimension of the ambient Weyl group is " << this->GetDim()
    << ". ";
    crash << crash;
  }
  output = r1[0].GetZero();
  leftType buffer;
  for (int i = 0; i < this->CartanSymmetric.NumRows; i ++)
    for (int j = 0; j < this->CartanSymmetric.NumCols; j ++)
    { buffer = r1[i];
      buffer *= r2[j];
      buffer *= this->CartanSymmetric.elements[i][j];
      output += buffer;
    }
}

template <class coefficient>
bool WeylGroupData::FreudenthalEval
(Vector<coefficient>& inputHWfundamentalCoords, HashedList<Vector<coefficient> >& outputDominantWeightsSimpleCoords,
 List<coefficient>& outputMultsSimpleCoords, std::string* outputDetails, int UpperBoundFreudenthal)
{ //double startTimer = theGlobalVariables.GetElapsedSeconds();
  MacroRegisterFunctionWithName("WeylGroup::FreudenthalEval");
  for (int i = 0; i < inputHWfundamentalCoords.size; i ++)
    if (inputHWfundamentalCoords[i] < 0)
    { if (outputDetails != 0)
        *outputDetails = "The highest weight is not dominant and I cannot apply the Freudenthal formula.";
      return false;
    }
  this->ComputeRho(true);
  Vectors<Rational> EiBasis;
  EiBasis.MakeEiBasis(this->GetDim());
  List<bool> Explored;
  Vector<coefficient> hwSimpleCoords = this->GetSimpleCoordinatesFromFundamental(inputHWfundamentalCoords);
  if (!this->GetAlLDominantWeightsHWFDIM(hwSimpleCoords, outputDominantWeightsSimpleCoords, UpperBoundFreudenthal, outputDetails))
    return false;
  Explored.initFillInObject(outputDominantWeightsSimpleCoords.size, false);
  outputMultsSimpleCoords.SetSize(outputDominantWeightsSimpleCoords.size);
  Vector<coefficient> currentWeight, currentDominantRepresentative, convertor;
  coefficient hwPlusRhoSquared;
  convertor = hwSimpleCoords;
  convertor += this->rho;//<-implicit type conversion here!!!!
  hwPlusRhoSquared = this->RootScalarCartanRoot(convertor, convertor);
  outputMultsSimpleCoords[0] = 1;
  Explored[0] = true;
//  stOutput << "<br>time for generating weights and initializations: " << theGlobalVariables.GetElapsedSeconds()-startTimer;
  //static double totalTimeSpentOnHashIndexing= 0;
//  static double timeSpentRaisingWeights = 0;
  coefficient BufferCoeff;
  ProgressReport theReport;
  for (int k = 1; k < outputDominantWeightsSimpleCoords.size; k ++)
  { Explored[k] = true;
    coefficient& currentAccum = outputMultsSimpleCoords[k];
    currentAccum = 0;
    for (int j = 0; j < this->RootsOfBorel.size; j ++)
      for (int i = 1; ; i ++)
      { currentWeight = outputDominantWeightsSimpleCoords[k];
        currentWeight += this->RootsOfBorel[j] * i;
        currentDominantRepresentative = currentWeight;
//        double startLocal = theGlobalVariables.GetElapsedSeconds();
        this->RaiseToDominantWeight(currentDominantRepresentative);
//        timeSpentRaisingWeights+= theGlobalVariables.GetElapsedSeconds()-startLocal;
        //double beforeHash= theGlobalVariables.GetElapsedSeconds();
        int theIndex = outputDominantWeightsSimpleCoords.GetIndex(currentDominantRepresentative);
        //totalTimeSpentOnHashIndexing+= theGlobalVariables.GetElapsedSeconds()-beforeHash;
        if (theIndex == - 1)
          break;
//        stOutput << "<br> summing over weight: " << currentWeight.ToString();
        if (!Explored[theIndex])
        { if (outputDetails != 0)
          { std::stringstream errorLog;
            errorLog << "This is an internal error check. If you see it, that means that the Freudenthal algorithm implementation is "
            << " wrong (because the author of the implementation was dumb enough to try to write less code than what is suggested by LiE).";
            *outputDetails = errorLog.str();
          }
          return false;
        }
        BufferCoeff = this->RootScalarCartanRoot(currentWeight, this->RootsOfBorel[j]);
        BufferCoeff *= outputMultsSimpleCoords[theIndex];
        currentAccum += BufferCoeff;
      }
    currentAccum *= 2;
    convertor = outputDominantWeightsSimpleCoords[k];
    convertor += this->rho;
    BufferCoeff = hwPlusRhoSquared;
    BufferCoeff -= this->RootScalarCartanRoot(convertor, convertor);
    if (BufferCoeff == 0)
      crash << "This is a programming or a mathematical error. I get that the denominator in the Freundenthal formula is zero. "
      << " The highest weight is " << inputHWfundamentalCoords.ToString() << ". The Weyl group details follow. " << this->ToString() << crash;
    currentAccum /= BufferCoeff;

//    stOutput << "<br>Coeff we divide by: " << (hwPlusRhoSquared-this->RootScalarCartanRoot
 //   (outputDominantWeightsSimpleCoords[k]+this->rho, outputDominantWeightsSimpleCoords[k]+this->rho))
  //  .ToString()
   // ;
    std::stringstream out;
    out << " Computed the multiplicities of " << k + 1 << " out of "
    << outputDominantWeightsSimpleCoords.size << " dominant weights in the support.";
    theReport.Report(out.str());
//    stOutput
//    << "<hr> Computed the multiplicities of " << k+ 1 << " out of " << outputDominantWeightsSimpleCoords.size << " dominant weights in the support.";
//    stOutput << "<br>time so far: " << theGlobalVariables.GetElapsedSeconds()-startTimer;
//    stOutput << " of which " << totalTimeSpentOnHashIndexing << " used for hash routines";
//    stOutput << " of which " << timeSpentRaisingWeights << " used to raise weights";
  }
//  stOutput << "<br>Total freudenthal running time: " << theGlobalVariables.GetElapsedSeconds()-startTimer;
  return true;
}

template<class coefficient>
bool WeylGroupData::GetAlLDominantWeightsHWFDIM
(Vector<coefficient>& highestWeightSimpleCoords, HashedList<Vector<coefficient> >& outputWeightsSimpleCoords,
 int upperBoundDominantWeights, std::string* outputDetails)
{ std::stringstream out;
//  double startTime= theGlobalVariables.GetElapsedSeconds();
//  stOutput << "<br>time elapsed: " << theGlobalVariables.GetElapsedSeconds()-startTime;
  Vector<coefficient> highestWeightTrue;
  highestWeightTrue = highestWeightSimpleCoords;
  this->RaiseToDominantWeight(highestWeightTrue);
  Vector<coefficient> highestWeightFundCoords = this->GetFundamentalCoordinatesFromSimple(highestWeightTrue);
  if (!highestWeightFundCoords.SumCoords().IsSmallInteger())
  { if (outputDetails != 0)
    { out << "<hr> The highest weight you gave in simple coordinates: " << highestWeightSimpleCoords.ToString()
      << " which equals " << highestWeightFundCoords.ToString() << "  in fundamental coordinates is not integral dominant.<br>";
      *outputDetails = out.str();
    }
    return false;
  }
  int theTopHeightSimpleCoords = (int) highestWeightSimpleCoords.GetVectorRational().SumCoords().GetDoubleValue() + 1;
//  int theTopHeightFundCoords =(int) highestWeightFundCoords.SumCoords().GetDoubleValue();
  if (theTopHeightSimpleCoords < 0)
    theTopHeightSimpleCoords = 0;
  List<HashedList<Vector<coefficient> > > outputWeightsByHeight;
  int topHeightRootSystem = this->RootsOfBorel.LastObject()->SumCoords().NumShort;
  int topHeightRootSystemPlusOne = topHeightRootSystem + 1;
  outputWeightsByHeight.SetSize(topHeightRootSystemPlusOne);
  int finalHashSize = 100;
  for (int i = 0; i < topHeightRootSystemPlusOne; i ++)
    outputWeightsByHeight[i].SetHashSizE(finalHashSize);
  outputWeightsSimpleCoords.Clear();
  outputWeightsSimpleCoords.SetHashSizE(10000);
  outputWeightsByHeight[0].AddOnTop(highestWeightTrue);
  int numTotalWeightsFound = 0;
  int numPosRoots = this->RootsOfBorel.size;
  Vector<coefficient> currentWeight;
//  stOutput << "<br>time spend before working cycle: " << theGlobalVariables.GetElapsedSeconds()-startTime;
  for (int lowestUnexploredHeightDiff = 0; lowestUnexploredHeightDiff <= theTopHeightSimpleCoords; lowestUnexploredHeightDiff ++)
  { //double startCycleTime= theGlobalVariables.GetElapsedSeconds();
    if (upperBoundDominantWeights > 0 && numTotalWeightsFound > upperBoundDominantWeights)
      break;
    int bufferIndexShift = lowestUnexploredHeightDiff % topHeightRootSystemPlusOne;
    HashedList<Vector<coefficient> >& currentHashes = outputWeightsByHeight[bufferIndexShift];
    for (int lowest = 0; lowest < currentHashes.size; lowest ++)
      for (int i = 0; i < numPosRoots; i ++)
      { currentWeight = currentHashes[lowest];
        currentWeight -= this->RootsOfBorel[i];
        if (this->IsDominantWeight(currentWeight))
        { int currentIndexShift = this->RootsOfBorel[i].SumCoords().NumShort;
          currentIndexShift = (currentIndexShift + bufferIndexShift) % topHeightRootSystemPlusOne;
          if (outputWeightsByHeight[currentIndexShift].AddOnTopNoRepetition(currentWeight))
          { numTotalWeightsFound ++;
            outputWeightsByHeight[currentIndexShift].AdjustHashes();
          }
        }
      }
//    stOutput << "<br>time spent before accounting at height level " << lowestUnexploredHeightDiff
//    << ": " << theGlobalVariables.GetElapsedSeconds()-startCycleTime;
//    stOutput << " Size of current level: " << currentHashes.size;
    outputWeightsSimpleCoords.AddOnTop(currentHashes);
//    stOutput << ". Time spent after accounting at height level "
//    << lowestUnexploredHeightDiff
//    << ": " << theGlobalVariables.GetElapsedSeconds()-startCycleTime;
//    startCycleTime= theGlobalVariables.GetElapsedSeconds();
    outputWeightsSimpleCoords.AdjustHashes();
    currentHashes.Clear();
//    stOutput << ". Time spent clearing up buffer at height level " << lowestUnexploredHeightDiff
//    << ": " << theGlobalVariables.GetElapsedSeconds()-startCycleTime;
    if (numTotalWeightsFound > upperBoundDominantWeights && upperBoundDominantWeights > 0)
    { out << "<hr>The number of weights has exceeded the RAM memory limits, aborting the weight generation. ";
      return false;
    }
  }
  if (outputDetails != 0)
  { out << " Total number of dominant weights: " << outputWeightsSimpleCoords.size;
    *outputDetails = out.str();
  }
  return true;
}

template <class coefficient>
void WeylGroupData::ReflectBetaWRTAlpha(const Vector<Rational>& alpha, const Vector<coefficient>& Beta, bool RhoAction, Vector<coefficient>& Output) const
{ coefficient bufferCoeff, alphaShift, lengthA;
  Vector<coefficient> result;
  result = (Beta);
  alphaShift = Beta[0].GetZero();//<-the zero of coefficient is not known at compile time (think multivariate polynomials)
  lengthA = alphaShift;
  if (RhoAction)
    result += this->rho;//<-implicit type conversion here if coefficient is not Rational
  for (int i = 0; i < this->CartanSymmetric.NumRows; i ++)
    for (int j = 0; j < this->CartanSymmetric.NumCols; j ++)
    { bufferCoeff = result[j];
      bufferCoeff *= alpha[i];
      bufferCoeff *= this->CartanSymmetric.elements[i][j] * (- 2);
      alphaShift += bufferCoeff;
      bufferCoeff = alpha[i];
      bufferCoeff *= alpha[j];
      bufferCoeff *= this->CartanSymmetric.elements[i][j];
      lengthA += bufferCoeff;
    }
  alphaShift /= lengthA;
  Output.SetSize(this->CartanSymmetric.NumRows);
  for (int i = 0; i < this->CartanSymmetric.NumCols; i ++)
  { bufferCoeff = alphaShift;
    bufferCoeff *= alpha[i];
    bufferCoeff += result[i];
    Output[i] = bufferCoeff;
  }
  if (RhoAction)
    Output -= this->rho;
}

template <typename somegroup, typename coefficient>
bool GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::CheckInitialization() const
{ if (this->ownerGroup == 0)
  { crash << "This is a programming error: working with a representation with non-initialized owner Weyl group. " << crash;
    return false;
  }
  return true;
}

template <typename somegroup, typename coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::reset()
{ this->parent = 0;
  this->flagCharacterIsComputed = false;
  this->ownerGroup = 0;
}

template <typename somegroup, typename coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::init(somegroup& inputOwner)
{ this->reset();
  this->ownerGroup = &inputOwner;
  this->ownerGroup->CheckInitializationConjugacyClasses();
  this->generatorS.SetSize(this->ownerGroup->generators.size);
  this->theElementImageS.SetSize(this->ownerGroup->theElements.size);
  this->theElementIsComputed.initFillInObject(this->ownerGroup->theElements.size, false);
  this->classFunctionMatrices.SetSize(this->ownerGroup->ConjugacyClassCount());
  this->classFunctionMatricesComputed.initFillInObject(this->ownerGroup->ConjugacyClassCount(), false);
  this->CheckInitialization();
}

//Note:  The group representation types compute the hash value from the character,
// in which the order of the conjugacy classes determines the value
// If you compare hash values from two groups which calculated their conjugacy classes in a different way,
// you must ensure they are in the same order, or hashing won't work as expected.
template <typename somegroup, typename coefficient>
unsigned int GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::HashFunction() const
{ unsigned int result = 0;
  result += this->theCharacteR.HashFunction();
  return result;
}

template <typename somegroup, typename coefficient>
unsigned int GroupRepresentation<somegroup, coefficient>::HashFunction() const
{ unsigned int result = 0;
  result += this->theCharacteR.HashFunction();
  return result;
}

template <typename somegroup, typename coefficient>
Matrix<coefficient>& GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::GetMatrixElement(int groupElementIndex)
{ Matrix<coefficient>& theMat = this->theElementImageS[groupElementIndex];
  if (this->theElementIsComputed[groupElementIndex])
    return theMat;
  const ElementWeylGroup<WeylGroupData>& theElt = this->ownerGroup->theElements[groupElementIndex];
  this->theElementIsComputed[groupElementIndex] = true;
  this->GetMatrixElement(theElt, theMat);
  return theMat;
}

template <typename somegroup, typename coefficient>
template <typename elementSomeGroup>
Matrix<coefficient> GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::GetMatrixElement(const elementSomeGroup& input)
{ Matrix<coefficient> result;
  this->GetMatrixElement(input, result);
  return result;
}

template <typename somegroup, typename coefficient>
int GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::GetDim() const
{ if (this->ownerGroup->generators.size == 0)
    return 1;
  return this->generatorS[0].NumRows;
}

template <typename somegroup, typename coefficient>
template <typename elementSomeGroup>
void GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::GetMatrixElement(const elementSomeGroup& input, Matrix<coefficient>& output)
{ this->CheckInitialization();
  if (this->ownerGroup->generators.size == 0) //here be trivial weyl group
  { output.MakeIdMatrix(1);
    return;
  }
  this->ownerGroup->CheckInitializationConjugacyClasses();
  output.MakeIdMatrix(this->GetDim());
  for (int i = 0; i < input.generatorsLastAppliedFirst.size; i ++)
  { if (input.generatorsLastAppliedFirst[i].flagIsOuter)
      crash << "WeylGroupRepresentation::GetMatrixElement called on an element that has outer automorphisms. "
      << "This is not allowed. "
      << crash;
    output.MultiplyOnTheRight(this->generatorS[input.generatorsLastAppliedFirst[i].index]);
  }
}

template <typename somegroup, typename coefficient>
std::string GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::GetName() const
{ std::string name;
  for (int i = 0; i < this->names.size; i ++)
  { name.append(this->names[i]);
    if (i != this->names.size - 1)
      name.append(" aka ");
  }
  return name;
}

template <typename somegroup, typename coefficient>
bool GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::operator>(const GroupRepresentationCarriesAllMatrices<somegroup, coefficient>& right) const
{ return this->theCharacteR > right.theCharacteR;
}

template <typename somegroup, typename coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::GetClassFunctionMatrix
(ClassFunction<somegroup, coefficient>& inputChar, Matrix<coefficient>& outputMat)
{ this->CheckInitialization();
  this->ownerGroup->CheckInitializationFDrepComputation();
  outputMat.MakeZeroMatrix(this->GetDim());
  int numClasses = this->ownerGroup->ConjugacyClassCount();
  ProgressReport theReport;
  for (int cci = 0; cci < numClasses; cci ++)
  { if (inputChar[cci] == 0)
      continue;
    if (!this->classFunctionMatricesComputed[cci])
    { this->classFunctionMatricesComputed[cci] = true;
      // classFunctionMatrices does not have to be initialized.
      bool useParent = false;
      if (this->parent != 0)
        useParent = (this->parent->classFunctionMatrices.size == this->ownerGroup->ConjugacyClassCount()) && (this->parent->classFunctionMatricesComputed[cci]);
      if (useParent)
        Matrix<coefficient>::MatrixInBasis(this->parent->classFunctionMatrices[cci], this->classFunctionMatrices[cci], this->basis, this->gramMatrixInverted);
      else
      { auto& currentCC = this->ownerGroup->conjugacyClasseS[cci];
        this->classFunctionMatrices[cci].MakeZeroMatrix(this->GetDim());
        for (int i = 0; i < currentCC.theElements.size; i ++)
        { if (!this->theElementIsComputed[currentCC.indicesEltsInOwner[i]])
            this->ComputeAllElementImages();
          this->classFunctionMatrices[cci] += this->theElementImageS[currentCC.indicesEltsInOwner[i]];
          if (theGlobalVariables.flagReportEverything)
          { std::stringstream reportstream;
            reportstream << " Computing conjugacy class " << currentCC.indicesEltsInOwner[i]+ 1
            << " (total num classes is " << numClasses << ").";
            theReport.Report(reportstream.str());
          }
        }
      }
      if (theGlobalVariables.flagReportEverything)
      { std::stringstream reportstream;
        reportstream << "<br>Class function matrix of conjugacy class " << cci + 1
        << " (total num classes is " << numClasses << ") computed to be: "
        << this->classFunctionMatrices[cci].ToString();
        theReport.Report(reportstream.str());
      }

    }
    for (int j = 0; j < outputMat.NumRows; j ++)
      for (int k = 0; k < outputMat.NumCols; k ++)
        outputMat(j, k) += this->classFunctionMatrices[cci](j, k) * MathRoutines::ComplexConjugate(inputChar[cci]);
  }
}

template <typename somegroup, typename coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::ClassFunctionMatrix
(ClassFunction<somegroup, coefficient>& inputCF, Matrix<coefficient>& outputMat)
{ int theDim = this->generatorS[0].NumRows;
  outputMat.MakeZeroMatrix(theDim);
  if (classFunctionMatrices.size == 0)
    classFunctionMatrices.SetSize(this->ownerGroup->ConjugacyClassCount());
  for (int cci = 0; cci < this->ownerGroup->ConjugacyClassCount(); cci ++)
  { if (inputCF[cci] == 0)
      continue;
    if (classFunctionMatrices[cci].NumCols == 0)
    { //stOutput << "Generating class function matrix " << cci << " with dimension " << this->generatorS[0].NumCols
      //<< "(cc has " << this->ownerGroup->conjugacyClasses[cci].size << ")" << "\n";
      classFunctionMatrices[cci].MakeZeroMatrix(this->generatorS[0].NumCols);
      for (int icci = 0; icci < this->ownerGroup->conjugacyClasseS[cci].size; icci ++)
      { //Matrix<coefficient> Mi;
        //Mi.MakeIdMatrix(this->generators[0].NumCols);
        this->classFunctionMatrices[cci] += this->GetMatrixElement(this->ownerGroup->conjugacyClasseS[cci].theElements[icci]);
        //for (int gi =g.reflections.size- 1; ; gi--)
        //{  if (gi < 0)
        //      break;
        //    Mi.MultiplyOnTheRight(gens[g.reflections[gi]]);
        //}
        //classFunctionMatrices[cci] += Mi;
      }
    }
    for (int i = 0; i < outputMat.NumRows; i ++)
      for (int j = 0; j < outputMat.NumCols; j ++)
        outputMat.elements[i][j] += classFunctionMatrices[cci].elements[i][j] * inputCF[cci];
  }
//  stOutput << outputMat.ToString(&consoleFormat) << "\n";

}

template <typename somegroup, typename coefficient>
std::string GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::ToString(FormatExpressions* theFormat) const
{ if (this->ownerGroup == 0)
    return "non-initialized representation";
  std::stringstream out;
  if (this->flagCharacterIsComputed)
    out << "Character: " << this->theCharacteR.ToString(theFormat) << " of norm " << this->theCharacteR.Norm();
  else
    out << "Character needs to be computed.";
  int theRank = this->ownerGroup->generators.size;
  LargeIntUnsigned theLCM, theDen;
  this->GetLargestDenominatorSimpleGens(theLCM, theDen);
  out << "\n<br>\n LCM denominators simple generators: " << theLCM.ToString() << ", largest denominator: " << theDen.ToString();
  out << "\n<br>\nThe simple generators (" << theRank << " total):<br> ";
  std::stringstream forYourCopyConvenience;
  for (int i = 0; i < theRank; i ++)
    if (i < this->generatorS.size)
    { std::stringstream tempStream;
      tempStream << "s_" << i + 1 << "=MatrixRationals{}" << this->generatorS[i].ToString(theFormat) << "; \\\\\n";
      forYourCopyConvenience << tempStream.str();
      out << HtmlRoutines::GetMathSpanPure("\\begin{array}{l}"+ tempStream.str()+"\\end{array}", 3000);
    } else
      out << "Simple generator " << i + 1 << "} not computed ";
  out << "<br>For your copy convenience: <br>" << forYourCopyConvenience.str();
  return out.str();
}

template <class coefficient>
void SubgroupWeylGroupOLD::ActByElement(int index, Vector<coefficient>& theRoot) const
{ Vector<coefficient> tempRoot;
  this->ActByElement(index, theRoot, tempRoot);
  theRoot = tempRoot;
}

template <class coefficient>
void SubgroupWeylGroupOLD::ActByElement(int index, Vector<coefficient>& input, Vector<coefficient>& output) const
{ this->ActByElement(this->TheObjects[index], input, output);
}

template <class coefficient>
void SubgroupWeylGroupOLD::ActByElement(const ElementWeylGroup<WeylGroupData>& theElement, const Vector<coefficient>& input, Vector<coefficient>& output) const
{ if (&input == &output)
    crash << crash;
  Vector<coefficient> tempRoot, tempRoot2;
  output = input;
  for (int i = theElement.generatorsLastAppliedFirst.size - 1; i >= 0; i --)
  { int tempI = theElement.generatorsLastAppliedFirst[i].index;
    if (tempI< this->simpleGenerators.size)
      this->AmbientWeyl->ReflectBetaWRTAlpha(this->simpleGenerators[tempI], output, false, output);
    else
    { tempI -= this->simpleGenerators.size;
      tempRoot.MakeZero(input.size);
      for (int j = 0; j < output.size; j ++)
      { tempRoot2 = this->ExternalAutomorphisms[tempI][j];
        tempRoot2 *= output[j];
        tempRoot += tempRoot2;
      }
    }
  }
}

template<class coefficient>
void SubgroupWeylGroupOLD::ActByElement(const ElementWeylGroup<WeylGroupData>& theElement, const Vectors<coefficient>& input, Vectors<coefficient>& output) const
{ if (&input == &output)
    crash << crash;
  output.SetSize(input.size);
  for (int i = 0; i < input.size; i ++)
    this->ActByElement(theElement, input[i], output[i]);
}

template <class coefficient>
bool SubgroupWeylGroupOLD::IsDominantWeight(const Vector<coefficient>& theWeight)
{ for (int i = 0; i < this->simpleGenerators.size; i ++)
    if (!this->IsDominantWRTgenerator(theWeight, i))
      return false;
  return true;
}

template<class coefficient>
coefficient SubgroupWeylGroupOLD::WeylDimFormulaSimpleCoords(const Vector<coefficient>& theWeightInSimpleCoords, const coefficient& theRingUnit)
{ MacroRegisterFunctionWithName("SubgroupWeylGroupOLD::WeylDimFormulaSimpleCoords");
  this->CheckInitialization();
  coefficient Result, buffer;
  Vector<coefficient> rhoOverNewRing, rootOfBorelNewRing, sumWithRho;//<-to facilitate type conversion!
  Vector<Rational> rho;
  this->RootsOfBorel.sum(rho, this->AmbientWeyl->GetDim());
  rho /= 2;
  rhoOverNewRing = rho;//<-type conversion here!
  Result = theRingUnit;
//  stOutput << "<br>doing the weyl dim formula with: " << theWeightInSimpleCoords.ToString();
//  stOutput << "<br>rho is:" << rhoOverNewRing.ToString();
//  stOutput << "<br>we get: ";
  for (int i = 0; i < this->RootsOfBorel.size; i ++)
  { rootOfBorelNewRing = this->RootsOfBorel[i]; //<-type conversion here!
    sumWithRho = rhoOverNewRing + theWeightInSimpleCoords;
    buffer = this->AmbientWeyl->RootScalarCartanRoot(sumWithRho, rootOfBorelNewRing);
    buffer /= this->AmbientWeyl->RootScalarCartanRoot(rhoOverNewRing, rootOfBorelNewRing);
//    stOutput << "(" << buffer.ToString() << ")";
    Result *= buffer;
  }
  return Result;
}

template <class coefficient>
bool SubgroupWeylGroupOLD::GetAlLDominantWeightsHWFDIM
(Vector<coefficient>& highestWeightSimpleCoords, HashedList<Vector<coefficient> >& outputWeightsSimpleCoords,
 int upperBoundDominantWeights, std::string& outputDetails)
{ MacroRegisterFunctionWithName("SubgroupWeylGroupOLD::GetAlLDominantWeightsHWFDIM");
  std::stringstream out;
  this->CheckInitialization();
  this->ComputeRootSubsystem();
//  double startTime= theGlobalVariables.GetElapsedSeconds();
//  stOutput << "<br>time elapsed: " << theGlobalVariables.GetElapsedSeconds()-startTime;
  Vector<coefficient> highestWeightTrue = highestWeightSimpleCoords;
  Vectors<Rational> basisEi;
  int theDim = this->AmbientWeyl->GetDim();
  basisEi.MakeEiBasis(theDim);
  this->RaiseToDominantWeight(highestWeightTrue);
  Vector<coefficient> highestWeightFundCoords = this->AmbientWeyl->GetFundamentalCoordinatesFromSimple(highestWeightTrue);
  int theTopHeightSimpleCoords = (int) highestWeightSimpleCoords.GetVectorRational().SumCoords().GetDoubleValue() + 1;
//  int theTopHeightFundCoords =(int) highestWeightFundCoords.SumCoords().GetDoubleValue();
  if (theTopHeightSimpleCoords < 0)
    theTopHeightSimpleCoords = 0;
  List<HashedList<Vector<coefficient> > > outputWeightsByHeight;
  int topHeightRootSystem = this->AmbientWeyl->RootsOfBorel.LastObject()->SumCoords().NumShort;
  int topHeightRootSystemPlusOne = topHeightRootSystem + 1;
  outputWeightsByHeight.SetSize(topHeightRootSystemPlusOne);
  int finalHashSize = 100;
  for (int i = 0; i < topHeightRootSystemPlusOne; i ++)
    outputWeightsByHeight[i].SetHashSizE(finalHashSize);
  outputWeightsSimpleCoords.Clear();
  outputWeightsByHeight[0].AddOnTop(highestWeightTrue);
  int numTotalWeightsFound = 0;
  Vector<coefficient> currentWeight;
//  stOutput << "<br>time spend before working cycle: " << theGlobalVariables.GetElapsedSeconds()-startTime;
  for (int lowestUnexploredHeightDiff = 0; lowestUnexploredHeightDiff <= theTopHeightSimpleCoords;
       lowestUnexploredHeightDiff ++)
  { //double startCycleTime= theGlobalVariables.GetElapsedSeconds();
    if (upperBoundDominantWeights > 0 && numTotalWeightsFound > upperBoundDominantWeights)
      break;
    int bufferIndexShift = lowestUnexploredHeightDiff % topHeightRootSystemPlusOne;
    HashedList<Vector<coefficient> >& currentHashes = outputWeightsByHeight[bufferIndexShift];
    for (int lowest = 0; lowest < currentHashes.size; lowest ++)
      for (int i = 0; i < this->RootsOfBorel.size; i ++)
      { currentWeight = currentHashes[lowest];
        currentWeight -= this->RootsOfBorel[i];
        if (this->IsDominantWeight(currentWeight))
        { int currentIndexShift = this->RootsOfBorel[i].SumCoords().NumShort;
          currentIndexShift = (currentIndexShift + bufferIndexShift) % topHeightRootSystemPlusOne;
          if (outputWeightsByHeight[currentIndexShift].AddOnTopNoRepetition(currentWeight))
          { numTotalWeightsFound ++;
            outputWeightsByHeight[currentIndexShift].AdjustHashes();
          }
        }
      }
//    stOutput << "<br>time spent before accounting at height level " << lowestUnexploredHeightDiff
//    << ": " << theGlobalVariables.GetElapsedSeconds()-startCycleTime;
//    stOutput << " Size of current level: " << currentHashes.size;
    outputWeightsSimpleCoords.AddOnTop(currentHashes);
//    stOutput << ". Time spent after accounting at height level " << lowestUnexploredHeightDiff
//    << ": " << theGlobalVariables.GetElapsedSeconds()-startCycleTime;
//    startCycleTime= theGlobalVariables.GetElapsedSeconds();
    outputWeightsSimpleCoords.AdjustHashes();
    currentHashes.Clear();
//    stOutput << ". Time spent clearing up buffer at height level " << lowestUnexploredHeightDiff
//    << ": " << theGlobalVariables.GetElapsedSeconds()-startCycleTime;
  }
  out << " Total number of dominant weights: " << outputWeightsSimpleCoords.size;
  if (numTotalWeightsFound >= upperBoundDominantWeights)
    out << "<hr>This message is generated either because the number of weights has exceeded the hard-coded RAM memory limits, or because "
    << " a priori bound for the number of weights is WRONG. If the latter is the case, make sure to send an angry email to the author(s).";
  outputDetails = out.str();
  //stOutput << "<hr><hr>Total time spent generating weights: " << -startTime +theGlobalVariables.GetElapsedSeconds();
  return numTotalWeightsFound <= upperBoundDominantWeights;
}

template <class coefficient>
void SubgroupWeylGroupOLD::RaiseToDominantWeight(Vector<coefficient>& theWeight, int* sign, bool* stabilizerFound)
{ MacroRegisterFunctionWithName("SubgroupWeylGroupOLD::RaiseToDominantWeight");
  if (sign != 0)
    *sign = 1;
  if (stabilizerFound != 0)
    *stabilizerFound = false;
  Rational theScalarProd;
//  int theDim= this->AmbientWeyl->GetDim();
  for (bool found = true; found; )
  { found = false;
    for (int i = 0; i < this->simpleGenerators.size; i ++)
    { if (!this->IsDominantWRTgenerator(theWeight, i))
      { found = true;
        this->AmbientWeyl->ReflectBetaWRTAlpha(this->simpleGenerators[i], theWeight, false, theWeight);
        if (sign != 0)
          *sign *= - 1;
      }
      if (stabilizerFound != 0)
        if (theScalarProd.IsEqualToZero())
          *stabilizerFound = true;
    }
  }
//  stOutput << "<hr># simple reflections applied total: " << numTimesReflectionWasApplied;
}

template <class coefficient>
bool SubgroupWeylGroupOLD::GenerateOrbitReturnFalseIfTruncated(const Vector<coefficient>& input, Vectors<coefficient>& outputOrbit, int UpperLimitNumElements)
{ HashedList<Vector<coefficient> > theOrbit;
  bool result = true;
  theOrbit.Clear();
  Vector<coefficient> tempRoot;
  theOrbit.AddOnTop(input);
  MemorySaving<Vectors<coefficient> >ExternalAutosOverAmbientField;
  for (int i = 0; i < theOrbit.size; i ++)
  { for (int j = 0; j < this->simpleGenerators.size; j ++)
    { this->AmbientWeyl->ReflectBetaWRTAlpha(this->simpleGenerators[j], theOrbit[i], false, tempRoot);
//      int oldsize= theOrbit.size;
//      std::string debugString= tempRoot.ToString() ;
      theOrbit.AddOnTopNoRepetition(tempRoot);
//      if (oldsize<theOrbit.size)
//        stOutput << "<br>" << debugString << " with hash " << tempRoot.HashFunction() << " added, ";
//      else
//        stOutput << "<br>" << debugString << " with hash " << tempRoot.HashFunction() << " NOT added, ";
    }
    for (int j = 1; j < this->ExternalAutomorphisms.size; j ++)
    { ExternalAutosOverAmbientField.GetElement() = this->ExternalAutomorphisms[j];
      theOrbit[i].GetCoordsInBasiS(ExternalAutosOverAmbientField.GetElement(), tempRoot);
      theOrbit.AddOnTopNoRepetition(tempRoot);
    }
    if (UpperLimitNumElements > 0)
      if (theOrbit.size >= UpperLimitNumElements)
      { result = false;
        break;
      }
  }
  outputOrbit = theOrbit;
  return result;
}

template <class coefficient>
bool SubgroupWeylGroupOLD::FreudenthalEvalIrrepIsWRTLeviPart
(const Vector<coefficient>& inputHWfundamentalCoords, HashedList<Vector<coefficient> >& outputDominantWeightsSimpleCoordS,
 List<coefficient>& outputMultsSimpleCoords, std::string& outputDetails, int UpperBoundFreudenthal)
{ MacroRegisterFunctionWithName("SubgroupWeylGroupOLD::FreudenthalEvalIrrepIsWRTLeviPart");
  //double startTimer = theGlobalVariables.GetElapsedSeconds();
  this->ComputeRootSubsystem();
  Vector<Rational> EiVect;
  List<bool> Explored;
  coefficient theRingZero;
  theRingZero = inputHWfundamentalCoords[0].GetZero();
  /////////////////////////
  Vector<coefficient> hwSimpleCoordsLeviPart, hwSimpleCoordsNilPart;
  hwSimpleCoordsLeviPart = inputHWfundamentalCoords;
  hwSimpleCoordsNilPart = inputHWfundamentalCoords;
  for (int i = 0; i < hwSimpleCoordsLeviPart.size; i ++)
  { EiVect.MakeEi(hwSimpleCoordsLeviPart.size, i);
    if (!this->RootsOfBorel.Contains(EiVect))
      hwSimpleCoordsLeviPart[i] = theRingZero;
    else
      hwSimpleCoordsNilPart[i] = theRingZero;
  }
  hwSimpleCoordsLeviPart = this->AmbientWeyl->GetSimpleCoordinatesFromFundamental(hwSimpleCoordsLeviPart);
  hwSimpleCoordsNilPart = this->AmbientWeyl->GetSimpleCoordinatesFromFundamental(hwSimpleCoordsNilPart);
//  stOutput << "highest weight levi part simple coords: " << hwSimpleCoordsLeviPart.ToString();
//  stOutput << "highest weight nil part siple coords: " << hwSimpleCoordsNilPart.ToString();
  ///////////////////////////
  HashedList<Vector<coefficient> > outputDomWeightsSimpleCoordsLeviPart;

  if (!this->GetAlLDominantWeightsHWFDIM(hwSimpleCoordsLeviPart, outputDomWeightsSimpleCoordsLeviPart, UpperBoundFreudenthal, outputDetails))
  { std::stringstream errorLog;
    errorLog << "Error: the number of dominant weights exceeded hard-coded limit of " << UpperBoundFreudenthal
    << ". Please check out whether LiE's implementation of the Freudenthal formula can do your computation.";
    outputDetails = errorLog.str();
    return false;
  }
//  stOutput << "Highest weight: " << hwSimpleCoords.ToString() << "<br>Dominant weights wrt. levi part("
//  << outputDomWeightsSimpleCoordsLeviPart.size << "):<br> ";
//  for (int i = 0; i <outputDomWeightsSimpleCoordsLeviPart.size; i ++)
//    stOutput << "<br>" << outputDomWeightsSimpleCoordsLeviPart[i].ToString();
  Explored.initFillInObject(outputDomWeightsSimpleCoordsLeviPart.size, false);
  outputMultsSimpleCoords.SetSize(outputDomWeightsSimpleCoordsLeviPart.size);
  Vector<coefficient> currentWeight, currentDominantRepresentative;
  Vector<coefficient> Rho;
  Rho = this->GetRho();//<-implicit type conversion here!
//  stOutput << "<br> Rho equals: " << Rho.ToString();
  //out << "<br> Rho equals: " << Rho.ToString();
  coefficient hwPlusRhoSquared;
  hwPlusRhoSquared = this->AmbientWeyl->RootScalarCartanRoot(hwSimpleCoordsLeviPart + Rho, hwSimpleCoordsLeviPart + Rho);
  Explored[0] = true;
  outputMultsSimpleCoords[0] = 1;
//  stOutput << "<br>time for generating weights and initializations: " << theGlobalVariables.GetElapsedSeconds()-startTimer;
  //static double totalTimeSpentOnHashIndexing= 0;
//  static double timeSpentRaisingWeights = 0;
//stOutput << "Freudenthal eval w.r.t subalgebra: positive root subsystem: " <<  this->RootsOfBorel.ToString();
  Vector<coefficient> convertor;
  coefficient bufferCoeff;
  ProgressReport theReport;
  for (int k = 1; k < outputDomWeightsSimpleCoordsLeviPart.size; k ++)
  { Explored[k] = true;
    coefficient& currentAccum = outputMultsSimpleCoords[k];
    currentAccum = 0;
    for (int j = 0; j < this->RootsOfBorel.size; j ++)
      for (int i = 1; ; i ++)
      { convertor = this->RootsOfBorel[j];
        convertor *= i;
        currentWeight = outputDomWeightsSimpleCoordsLeviPart[k] + convertor;
        currentDominantRepresentative = currentWeight;
//        double startLocal = theGlobalVariables.GetElapsedSeconds();
        this->RaiseToDominantWeight(currentDominantRepresentative);
//        stOutput << "<br>currentDominant representative: " << currentDominantRepresentative.ToString();
        int theIndex = outputDomWeightsSimpleCoordsLeviPart.GetIndex(currentDominantRepresentative);
//        stOutput << "<br>index of currentDomain rep: " << theIndex;
        //totalTimeSpentOnHashIndexing+= theGlobalVariables.GetElapsedSeconds()-beforeHash;
        if (theIndex == - 1)
          break;
//        stOutput << "<br> summing over weight: " << currentWeight.ToString();
        if (!Explored[theIndex])
        { std::stringstream errorLog;
          errorLog << "This is an internal error check. If you see it, that means "
          << " that the Freudenthal algorithm implementation is "
          << " wrong (because the author of the implementation was dumb enough to"
          << " try to write less code than what is "
          << " suggested by LiE).";
          outputDetails = errorLog.str();
          return false;
        }
        convertor = this->RootsOfBorel[j];//<-implicit type conversion here!
        bufferCoeff = this->AmbientWeyl->RootScalarCartanRoot(currentWeight, convertor);
        bufferCoeff *= outputMultsSimpleCoords[theIndex];
        currentAccum += bufferCoeff;
//        stOutput << "<hr>current weight: " << currentWeight.ToString();
//        stOutput << "<br>current dominant representative " << currentDominantRepresentative.ToString();
      }
    currentAccum *= 2;
   // stOutput << "<br>hwPlusRhoSquared: " << hwPlusRhoSquared.ToString();
    bufferCoeff = hwPlusRhoSquared;
    bufferCoeff -= this->AmbientWeyl->RootScalarCartanRoot(outputDomWeightsSimpleCoordsLeviPart[k]+Rho, outputDomWeightsSimpleCoordsLeviPart[k]+Rho);
    //bufferCoeff now holds the denominator participating in the Freudenthal formula.
    if (bufferCoeff.IsEqualToZero())
      crash << crash;
    currentAccum /= bufferCoeff;
//    stOutput << "<br>Coeff we divide by: " << bufferCoeff.ToString()
 //   ;
    std::stringstream out;
    out << " Computed the multiplicities of " << k + 1 << " out of "
    << outputDomWeightsSimpleCoordsLeviPart.size << " dominant weights in the support.";
    theReport.Report(out.str());
//    stOutput
  //  << "<hr> Computed the multiplicities of " << k+ 1 << " out of " << outputDomWeightsSimpleCoordsLeviPart.size << " dominant weights in the support.";
 //   theGlobalVariables.MakeStatusReport(out.str());
//    stOutput << "<br>time so far: " << theGlobalVariables.GetElapsedSeconds()-startTimer;
//    stOutput << " of which " << totalTimeSpentOnHashIndexing << " used for hash routines";
//    stOutput << " of which " << timeSpentRaisingWeights << " used to raise weights";
  }
  outputDominantWeightsSimpleCoordS.Clear();
  outputDominantWeightsSimpleCoordS.SetExpectedSize(outputDomWeightsSimpleCoordsLeviPart.size);
  for (int i = 0; i < outputDomWeightsSimpleCoordsLeviPart.size; i ++)
    outputDominantWeightsSimpleCoordS.AddOnTop(outputDomWeightsSimpleCoordsLeviPart[i] + hwSimpleCoordsNilPart);
//  stOutput << "<br>Total freudenthal running time: " << theGlobalVariables.GetElapsedSeconds()-startTimer;
  return true;
}

template<class someFiniteGroup, typename coefficient>
void ClassFunction<someFiniteGroup, coefficient>::MakeZero(someFiniteGroup& inputWeyl)
{ this->G = &inputWeyl;
  this->data.MakeZero(this->G->ConjugacyClassCount());
}

template<class someFiniteGroup, typename coefficient>
coefficient& ClassFunction<someFiniteGroup, coefficient>::operator[](int i) const
{ return this->data[i];
}

template<class someFiniteGroup, typename coefficient>
std::string ClassFunction<someFiniteGroup, coefficient>::ToString(FormatExpressions* theFormat) const
{ (void) theFormat;//avoid unused parameter warning, portable
  if (this->G == 0)
    return "(not initialized)";
  std::stringstream out;
  out << "(";
  for (int i = 0; i < this->data.size; i ++)
  { out << this->data[i];
    if (i < this->data.size - 1)
      out << ", ";
  }
  out << ")";
  if (this->Norm().ToString() != "1")
  { out << "[";
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
{ unsigned int acc = 0;
  int N = (input.data.size < SomeRandomPrimesSize) ? input.data.size : SomeRandomPrimesSize;
  for (int i = 0; i < N; i ++)
    acc += input.data[i].HashFunction() * SomeRandomPrimes[i];
  return acc;
}

template<class someFiniteGroup, typename coefficient>
bool ClassFunction<someFiniteGroup, coefficient>::operator==(const ClassFunction<someFiniteGroup, coefficient>& other) const
{ if (this->G != other.G)
    return false; //we allow comparison of class functions belonging to different groups
  //(this should be handy for the calculator interface, an user may have more than 1 group in play).
  if (this->data == other.data)
    return true;
  return false;
}

// collation order
template<class someFiniteGroup, typename coefficient>
bool ClassFunction<someFiniteGroup, coefficient>::operator>(const ClassFunction<someFiniteGroup, coefficient>& right) const
{ if (this->data[0] > right.data[0])
    return true;
  if (right.data[0] > this->data[0])
    return false;
  if (this->data > right.data)
    return false;
  if (right.data > this->data)
    return true;
  return false;
}

// Interal API of Subgroup class can go here
template <typename someGroup, typename elementSomeGroup>
void SubgroupData<someGroup, elementSomeGroup>::ComputeCosets()
{ if (flagCosetSetsComputed)
    return;
  if (this->CosetRepresentativeEnumerator && this->SameCosetAsByFormula)
    return this->CosetRepresentativeEnumerator(this);
  this->theSubgroup->ComputeAllElements(true);
  this->theGroup->ComputeAllElements(true);
  GraphOLD orbitg = GraphOLD(this->theGroup->theElements.size, this->theSubgroup->generators.size);
  for (int i = 0; i < this->theGroup->theElements.size; i ++)
    for (int j = 0; j < this->theSubgroup->generators.size; j ++)
      orbitg.AddEdge(this->theGroup->theElements.GetIndex(this->theGroup->theElements[i] * this->theSubgroup->generators[j]), i);
  List<List<int> > orbits;
  orbits = orbitg.DestructivelyGetConnectedComponents();
  this->cosets.SetSize(orbits.size);
  for (int i = 0; i < orbits.size; i ++)
  { cosets[i].supergroupIndices = orbits[i];
    //stOutput << cosets[i].supergroupIndices;
    cosets[i].representative = this->theGroup->theElements[orbits[i][0]];
    this->theGroup->GetWord(this->theGroup->theElements[orbits[i][0]], cosets[i].representativeWord);
  }
  this->flagCosetSetsComputed = true;
  this->flagCosetRepresentativesComputed = true;
  //stOutput << "Computed cosets of group " /*<< *parent*/ << " have " << cosets.size << '\n';
  //for (int i = 0; i <cosets.size; i ++)
  //  stOutput << cosets[i].ToString() << ", ";
  //stOutput << '\n';
  this->VerifyCosets();
}

template <typename someGroup, typename elementSomeGroup>
bool SubgroupData<someGroup, elementSomeGroup>::VerifyCosets()
{ // this returns true or crashes because if the cosets aren't cosets something is seriously wrong
  ComputeCosets();
  for (int i = 0; i < this->theSubgroup->generators.size; i ++)
    for (int cs = 0; cs < this->cosets.size; cs ++)
    { auto g = cosets[cs].representative * this->theSubgroup->generators[i];
      if (this->GetCosetId(g) != cs)
        crash << "Error: element " << g << " not found in coset " << cs
        << " despite being product of subgroup generator " << this->theSubgroup->generators[i] << " by coset representative "
        << cosets[cs].representative << crash;
    }
  if (this->flagCosetSetsComputed)
    for (int cs = 0; cs < this->cosets.size; cs ++)
    { auto g = this->cosets[cs].representative;
      g.Invert();
      for (int i = 0; i < this->cosets[cs].supergroupIndices.size; i ++)
      { auto g2 = this->theGroup->theElements[this->cosets[cs].supergroupIndices[i]];
        auto g3 = g * g2;
        if (!this->theSubgroup->HasElement(g3))
          crash << "Error: coset " << cs << " has representative " << this->cosets[cs].representative
          << "; a putative coset element has " << g << " * " << g2 << " = " << g3
          << " which is not in the subgroup" << crash;
      }
    }
  return true;
}

// Refugees from vpfCharacters_CalculatorInterface.cpp

template <typename elementSomeGroup>
void FiniteGroup<elementSomeGroup>::AddIrreducibleRepresentation(GroupRepresentationCarriesAllMatrices<FiniteGroup<elementSomeGroup>, Rational>& p)
{ this->irreps_grcam.BSInsertDontDup(p);
  auto p_gr = p.MakeOtherGroupRepresentationClass();
  this->AddIrreducibleRepresentation(p_gr);
}

template <typename elementSomeGroup>
void FiniteGroup<elementSomeGroup>::AddIrreducibleRepresentation(GroupRepresentation<FiniteGroup<elementSomeGroup>, Rational>& p)
{ p.ComputeCharacter();
  this->irreps.BSInsertDontDup(p);
  this->characterTable.BSInsertDontDup(p.theCharacteR);
}

template <typename elementSomeGroup>
void FiniteGroup<elementSomeGroup>::AddCharacter(const ClassFunction<FiniteGroup<elementSomeGroup>, Rational>& X)
{ this->characterTable.BSInsertDontDup(X);
  // the following is the reason the characterTable is not a list of pointers
  // this method is kept because (1) it will be inlined anyway
  //                             (2) to remind the user not to use a list of pointers
  /*  ClassFunction<FiniteGroup<elementSomeGroup>, Rational>* place = 0;
  for (int i = 0; i < this->irreps.size; i ++)
    if (this->irreps[i].theCharacteR == X)
      place = &(this->irreps[i].theCharacteR);
  if (place == 0)
    for (int i = 0; i < this->orphanCharacters.size; i ++)
      if (this->orphanCharacters[i] == X)
        place = orphanCharacters[i];
  if (place == 0)
  { orphanCharacters.AddOnTop(X);
    place = &(orphanCharacters.LastObject());
  }
  for (int i = 0; i <characterTable.size; i ++)
    if (characterTable[i] == place)
      return;
  characterTable.AddOnTop(place);*/
}

template <typename elementSomeGroup>
void FiniteGroup<elementSomeGroup>::ComputeIrreducibleRepresentationsTodorsVersion()
{ MacroRegisterFunctionWithName("FiniteGroup::ComputeIrreducibleRepresentationsTodorsVersion");
  if (this->irreps_grcam.size == 0)
  { if (this->irreps.size == 0)
      crash << "Need an initial irrep.  Check up the call chain and find out where it should be provided" << crash;
    for (int i = 0; i < this->irreps.size; i ++)
      this->irreps_grcam.AddOnTop(irreps[i].MakeGRCAM());
  }
  List<GroupRepresentationCarriesAllMatrices<FiniteGroup<elementSomeGroup>, Rational> > appendOnlyIrrepsList;
  appendOnlyIrrepsList = this->irreps_grcam;

  { bool nontrivial = false;
    for (int i = 0; i < appendOnlyIrrepsList[0].theCharacteR.data.size; i ++)
      if (appendOnlyIrrepsList[0].theCharacteR.data[i] != 1)
      { nontrivial = true;
        break;
      }
    if (!nontrivial)
      appendOnlyIrrepsList.RemoveIndexShiftDown(0);
  }
  int initialcount = appendOnlyIrrepsList.size;

  if (this->theElements.size == 0)
    this->ComputeCCfromAllElements();
  GroupRepresentationCarriesAllMatrices<FiniteGroup<ElementWeylGroup<WeylGroupData> >, Rational> newRep;
  int NumClasses = this->ConjugacyClassCount();
  VirtualRepresentation<FiniteGroup<ElementWeylGroup<WeylGroupData> >, Rational> decompositionNewRep;
  ProgressReport theReport1;
//  int indexFirstPredefinedRep =1; //<-this should be the index of the sign rep.
//  int indexLastPredefinedrep =2; //<-this should be the index of the standard rep.
  for (int i = 0; i < appendOnlyIrrepsList.size && this->irreps != NumClasses; i ++)
    for (int j = 0; j < initialcount; j ++)
    { if (theGlobalVariables.flagReportEverything)
      { std::stringstream reportStream;
        reportStream << this->irreps.size << " irreducible representations found so far. ";
        reportStream << "<br>Decomposing " << appendOnlyIrrepsList[j].theCharacteR << " * " << appendOnlyIrrepsList[i].theCharacteR << "\n";
        theReport1.Report(reportStream.str());
      }
      newRep = appendOnlyIrrepsList[j];//we are initializing by the sign or natural rep.
      newRep *= appendOnlyIrrepsList[i];
      bool tempB = newRep.DecomposeTodorsVersion(decompositionNewRep, &appendOnlyIrrepsList);
      if (!tempB)
        crash << "This is a mathematical error: failed to decompose " << newRep.theCharacteR.ToString() << ". " << crash;
    }
  if (theGlobalVariables.flagReportEverything)
  { std::stringstream reportStream;
    reportStream << "Irrep table:";
    for (int i = 0; i < this->irreps.size; i ++)
      reportStream << "\n<br>\n" << this->irreps[i].theCharacteR.ToString();
    FormatExpressions tempFormat;
    tempFormat.flagUseLatex = true;
    for (int i = 0; i < this->irreps.size; i ++)
      reportStream << "<hr>irrep " << i + 1 << "<br>" << this->irreps_grcam[i].ToString(&tempFormat);
    theReport1.Report(reportStream.str());
  }
  this->flagCharTableIsComputed = true;
  this->flagIrrepsAreComputed = true;
//  stOutput << this->PrettyPrintCharacterTable();
  if (this->characterTable.size < this->conjugacyClasseS.size)
    crash << crash;
  if (this->irreps.size < this->conjugacyClasseS.size)
    crash << crash;
}


template <typename somegroup, typename coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::SpreadVector(const Vector<coefficient>& input, Vectors<coefficient>& outputBasisGeneratedSpace)
{ this->CheckInitialization();
  outputBasisGeneratedSpace.SetSize(1);
  outputBasisGeneratedSpace[0] = input;
  Vector<coefficient> tempV;
  int theRank = this->ownerGroup->GetDim();
  for (int i = 0; i < outputBasisGeneratedSpace.size; i ++)
    for (int j = 0; j < theRank; j ++)
    { tempV = outputBasisGeneratedSpace[i];
      this->theElementImageS[j + 1].ActOnVectorColumn(tempV);
      if (!outputBasisGeneratedSpace.LinSpanContainsVector(tempV))
        outputBasisGeneratedSpace.AddOnTop(tempV);
    }
}

template <typename somegroup, typename coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::GetLargestDenominatorSimpleGens(LargeIntUnsigned& outputLCM, LargeIntUnsigned& outputDen) const
{ MacroRegisterFunctionWithName("GroupRepresentationCarriesAllMatrices::GetLargestDenominatorSimpleGens");
  outputLCM = 1;
  outputDen = 1;
  for (int gi = 0; gi < this->generatorS.size; gi ++)
    for (int mi = 0; mi < this->generatorS[gi].NumRows; mi ++)
      for (int mj = 0; mj < this->generatorS[gi].NumCols; mj ++)
      { if (this->generatorS[gi](mi, mj).GetDenominator() > outputDen)
          outputDen = this->generatorS[gi](mi, mj).GetDenominator();
        outputLCM = LargeIntUnsigned::lcm(outputLCM, this->generatorS[gi](mi, mj).GetDenominator());
      }
}


template <typename somegroup, typename coefficient>
bool GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::DecomposeTodorsVersionRecursive
(VirtualRepresentation<somegroup, coefficient>& outputIrrepMults,
 List<GroupRepresentation<somegroup, coefficient> >& appendOnlyIrrepsList,
 List<GroupRepresentationCarriesAllMatrices<somegroup, coefficient> >* appendOnlyGRCAMSList)
{ MacroRegisterFunctionWithName("WeylGroupRepresentation::DecomposeTodorsVersionRecursive");
  this->CheckInitialization();

  this->ownerGroup->CheckInitializationFDrepComputation();
  this->GetCharacter();
  coefficient SumOfNumComponentsSquared = this->GetNumberOfComponents();
  if (SumOfNumComponentsSquared == 0)
    crash << "This is a programming error: a module has character " << this->theCharacteR.ToString()
    << " of zero length, which is impossible. "<< "Here is a printout of the module. "
    << this->ToString() << crash;
  if (SumOfNumComponentsSquared == 1)
  { int i = this->ownerGroup->characterTable.BSGetIndex(this->theCharacteR);
    if (i == - 1)
    { this->ownerGroup->AddIrreducibleRepresentation(*this);
      appendOnlyIrrepsList.AddOnTop(this->MakeOtherGroupRepresentationClass());
      if (appendOnlyGRCAMSList)
        appendOnlyGRCAMSList->AddOnTop(*this);
      i = this->ownerGroup->characterTable.BSGetIndex(this->theCharacteR);
    }
    outputIrrepMults.AddMonomial(this->ownerGroup->characterTable[i], 1);
    return true;
  }
  Matrix<coefficient> splittingOperatorMatrix;
  Vectors<coefficient> splittingMatrixKernel, remainingVectorSpace, tempSpace;
  ClassFunction<somegroup, coefficient> remainingCharacter = this->theCharacteR;
  remainingVectorSpace.MakeEiBasis(this->GetDim());
  ProgressReport Report1, Report2,
  Report3, Report4;
  { std::stringstream reportStream;
    reportStream << "<br>\nDecomposing module with character " << this->theCharacteR.ToString();
    LargeIntUnsigned largestDen, lcmDen;
    this->GetLargestDenominatorSimpleGens(lcmDen, largestDen);
    reportStream << "\n<br>\n Largest denominator is " << largestDen.ToString() << ", denominator lcm is: " << lcmDen.ToString();
    Report1.Report(reportStream.str());
  }
  //chop off already known pieces:
  for (int i = 0; i < appendOnlyIrrepsList.size; i ++)
  { coefficient NumIrrepsOfType = this->theCharacteR.InnerProduct(appendOnlyIrrepsList[i].GetCharacter());
    if (NumIrrepsOfType != 0)
    { this->ownerGroup->CheckInitializationFDrepComputation();
      { std::stringstream reportStream;
        reportStream << "<hr>\ncontains irrep " << appendOnlyIrrepsList[i].theCharacteR.ToString() << " with multiplicity "
        << NumIrrepsOfType << "\n";
        reportStream << "<hr>\nGetting class f-n matrix from character: " << appendOnlyIrrepsList[i].theCharacteR;
        Report2.Report(reportStream.str());
      }
      this->GetClassFunctionMatrix(appendOnlyIrrepsList[i].theCharacteR, splittingOperatorMatrix);
      { std::stringstream reportStream;
        reportStream << "<br>class f-n matrix: " << splittingOperatorMatrix.ToString() << "\n <br>\n" << " computing its zero eigenspace... ";
        Report3.Report(reportStream.str());
      }
      splittingOperatorMatrix.GetZeroEigenSpaceModifyMe(splittingMatrixKernel);

      remainingVectorSpace.IntersectTwoLinSpaces(splittingMatrixKernel, remainingVectorSpace, tempSpace);

      // I'm not sure how much of a good idea it is to ensure that outputIrrepMults only takes monomials
      // from ownerGroup->characterTable, it might be better to add the character from irreps pointed to
      // by the appendOnlyIrrepsList[i]
      int ci = this->ownerGroup->characterTable.BSGetIndex(appendOnlyIrrepsList[i].theCharacteR);
      outputIrrepMults.AddMonomial(this->ownerGroup->characterTable[ci], NumIrrepsOfType);
      remainingCharacter -= appendOnlyIrrepsList[i].theCharacteR*NumIrrepsOfType;
      { std::stringstream reportStream;
        reportStream << "<br>Intersecting kernel of class f-n matrix" << splittingMatrixKernel.ToString() << " with "
        << remainingVectorSpace.ToString() << " to get: " << tempSpace.ToString() << " with remaining character: " << remainingCharacter.ToString();
        Report4.Report(reportStream.str());
      }
      remainingVectorSpace = tempSpace;
      if (remainingCharacter[0] != remainingVectorSpace.size)
      { crash << "<br>This is a programming error: remaining char " << remainingCharacter.ToString() << " indicates dimension "
        << remainingCharacter[0].ToString()
        << " but remaining vector space has dim " << remainingVectorSpace.size << crash;
      }
      if (remainingCharacter.IsEqualToZero())
        if (!remainingVectorSpace.size == 0)
        { crash << "This is a programming error: remaining char is zero but remaining space is " << remainingVectorSpace.ToString()
          << ". Starting char: " << this->theCharacteR.ToString() << crash;
        }
    }
  }
  if ((remainingVectorSpace.size < this->GetDim()) && (remainingVectorSpace.size > 0))
  { //stOutput << "<br>restricting to subrep(s)... ";
    GroupRepresentationCarriesAllMatrices<somegroup, coefficient> reducedRep;
    this->Restrict(remainingVectorSpace, remainingCharacter, reducedRep);
    //stOutput << "done" << "\n";
    //stOutput << "Decomposing remaining subrep(s) " << reducedRep.GetCharacter() << "\n";
    return reducedRep.DecomposeTodorsVersionRecursive(outputIrrepMults, appendOnlyIrrepsList, appendOnlyGRCAMSList);
  }
  if (remainingVectorSpace.size == 0)
    return true;
  int NumClasses = this->ownerGroup->ConjugacyClassCount();
  ClassFunction<WeylGroupData::WeylGroupBase, coefficient> virtualChar;
  List<Vectors<coefficient> > theSubRepsBasis;
  for (int cfi = NumClasses - 1; cfi >= 0; cfi --)
  { virtualChar.MakeZero(*this->ownerGroup);
    virtualChar[cfi] = 1;
//    stOutput << "<br>getting matrix of virtual char " << virtualChar << "\n";
    this->GetClassFunctionMatrix(virtualChar, splittingOperatorMatrix);
//    FormatExpressions tempFormat;
//    tempFormat.flagUseLatex= true;
//    stOutput << "... and the result is:<br>" << splittingOperatorMatrix.ToString(&tempFormat);
    bool tempB = splittingOperatorMatrix.GetEigenspacesProvidedAllAreIntegralWithEigenValueSmallerThanDim(theSubRepsBasis);
    if (!tempB)
      crash << "<br>This is a mathematical or programming mistake: splittingOperatorMatrix should have small integral values, which it doesn't!" << crash;
    //stOutput << "<br>the eigenspaces were of dimensions: ";
//    for (int i = 0; i <theSubRepsBasis.size; i ++)
//      stOutput << theSubRepsBasis[i].size << " ";
    GroupRepresentationCarriesAllMatrices<somegroup, coefficient> newRep;
    if (theSubRepsBasis.size > 1)//we found splitting, so let us recursively decompose:
    { for (int i = 0; i < theSubRepsBasis.size; i ++)
      { //stOutput << "<br>restricting current rep to basis " << theSubRepsBasis[i].ToString();
        remainingCharacter.MakeZero(*this->ownerGroup);
        this->Restrict(theSubRepsBasis[i], remainingCharacter, newRep);
        if (!newRep.DecomposeTodorsVersionRecursive(outputIrrepMults, appendOnlyIrrepsList, appendOnlyGRCAMSList))
          return false;
      }
      return true;
    }
//    stOutput << "\n";
  }
  Vector<coefficient> startingVector, tempV, average;
  startingVector.MakeEi(this->GetDim(), 0);
  average.MakeZero(this->GetDim());
  for (int i = 0; i < this->theElementImageS.size; i ++)
  { if (!this->theElementIsComputed[i])
      crash << "<hr>This is a programming error: an internal check failed. " << crash;
    this->theElementImageS[i].ActOnVectorColumn(startingVector, tempV);
    average += tempV;
  }
//  stOutput << "<br>Current char: " << this->theCharacteR.ToString();
//  stOutput << "<br>Spreading: " << average.ToString();
/*  Vectors<coefficient> theSpread;
  this->SpreadVector(average, theSpread);
  if (theSpread.size== this->GetDim())
    crash << "<hr><b>I was horribly wrong!</b>" << crash;
  else
  { stOutput << "<hr>The image of the spread is " << theSpread.size << " dimensional. ";
  }
*/
  return false;
}

template <typename somegroup, typename coefficient>
GroupRepresentation<somegroup, coefficient> GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::MakeOtherGroupRepresentationClass() const
{ GroupRepresentation<somegroup, coefficient> out;
  out.flagCharacterIsComputed = this->flagCharacterIsComputed;
  out.generatorS = this->generatorS;
  out.ownerGroup = this->ownerGroup;
  std::stringstream s;
  for (int i = 0; i < this->names.size; i ++)
  { s << this->names[i];
    if (i < this->names.size - 1)
      s << " ";
  }
  out.identifyingString = s.str();
  out.theCharacteR = this->theCharacteR;
  return out;
}

template <typename elementSomeGroup>
void FiniteGroup<elementSomeGroup>::ComputeIrreducibleRepresentationsThomasVersion
(GroupRepresentationCarriesAllMatrices<FiniteGroup<elementSomeGroup>, Rational>* startingIrrep)
{ MacroRegisterFunctionWithName("WeylGroup::ComputeIrreducibleRepresentationsThomasVersion");
  if (!startingIrrep)
  { if (this->irreps_grcam.size != 0)
      startingIrrep = &(this->irreps_grcam[0]);
    else
    { if (this->irreps.size != 0)
        startingIrrep = &(this->irreps[0].MakeGRCAM());
    }
    if (!startingIrrep || (startingIrrep->generatorS.size > 0 && startingIrrep->generatorS[0].NumRows == 1))
      crash << "Can't find a good starting irrep.  If you think you provided one, change the code near "
      << __FILE__ << ":" << __LINE__ << crash;
  }
  GroupRepresentationCarriesAllMatrices<FiniteGroup<elementSomeGroup>, Rational>& sr = *startingIrrep;
  if (this->theGroup.squaresCCReps.size == 0)
    this->theGroup.ComputeSquaresCCReps(theGlobalVariables);
  this->theGroup.characterTable.SetSize(0);
  this->irreps_grcam.SetSize(0);
//  stOutput << "<hr>Character of initial module: " << sr.GetCharacter() << "\n";
  List<GroupRepresentationCarriesAllMatrices<FiniteGroup<elementSomeGroup>, Rational> > newspaces;
  newspaces.AddOnTop(sr);
  List<GroupRepresentationCarriesAllMatrices<FiniteGroup<elementSomeGroup>, Rational> > incompletely_digested;
  while ((newspaces.size > 0) || (incompletely_digested.size > 0))
  { GroupRepresentationCarriesAllMatrices<FiniteGroup<elementSomeGroup>, Rational> nspace;
    if (newspaces.size > 0)
      nspace = newspaces.PopLastObject();
    else
    { nspace = incompletely_digested[0];
      incompletely_digested.RemoveIndexShiftDown(0);
    }
    GroupRepresentationCarriesAllMatrices<FiniteGroup<elementSomeGroup>, Rational> tspace = sr * nspace;
    tspace.flagCharacterIsComputed = false;
    //stOutput << "<hr>Decomposing " << tspace.GetCharacter();
    //stOutput << " = " << sr.GetCharacter().ToString() << "*" << nspace.ToString() << "\n";
    List<GroupRepresentationCarriesAllMatrices<FiniteGroup<elementSomeGroup>, Rational> > spaces = tspace.DecomposeThomasVersion();
    //stOutput << "<hr><hr> to get: " << spaces.ToString() << "<hr><hr>";
//      tspace = nspace * sr;
//      tspace.character.data.size = 0;
//      stOutput << "Decomposing (right tensor)" << tspace.GetCharacter() << "\n";
//      spaces.AddListOnTop(tspace.Decomposition());
    for (int spi = 0; spi < spaces.size; spi ++)
      if (spaces[spi].GetNumberOfComponents() == 1)
      { if (!this->theGroup.characterTable.Contains(spaces[spi].GetCharacter()))
        { this->theGroup.characterTable.AddOnTop(spaces[spi].GetCharacter());
          this->irreps_grcam.AddOnTop(spaces[spi]);
          newspaces.AddOnTop(spaces[spi]);
          //stOutput << "we have " << irreps_grcam.size << " irreps" << "\n";
        }
      } else
        incompletely_digested.AddOnTop(spaces[spi]);
    if (irreps_grcam.size == this->theGroup.ConjugacyClassCount())
      break;
    for (int spi = 0; spi < incompletely_digested.size; spi ++)
      for (int ci = 0; ci < this->theGroup.characterTable.size; ci ++)
        if (incompletely_digested[spi].GetCharacter().InnerProduct(this->theGroup.characterTable[ci]) != 0)
        { //stOutput << "incompletely digested " << incompletely_digested[spi].GetCharacter() << " will now be further decomposed " << "\n";
          List<GroupRepresentationCarriesAllMatrices<FiniteGroup<elementSomeGroup>, Rational> > shards = incompletely_digested[spi].DecomposeThomasVersion();
          incompletely_digested.RemoveIndexShiftDown(spi);
          for (int shi = 0; shi < shards.size; shi ++)
            if (shards[shi].GetNumberOfComponents() == 1)
            { if (!this->theGroup.characterTable.Contains(shards[shi].GetCharacter()))
              { this->irreps_grcam.AddOnTop(spaces[spi]);
                this->characterTable.AddOnTop(this->irreps_grcam.LastObject()->theCharacteR);
                newspaces.AddOnTop(spaces[spi]);
                //stOutput << "we have " << irreps_grcam.size << " irreps (got from shards)" << "\n";
              }
            } else
              incompletely_digested.AddOnTop(shards[shi]);
          break;
        }
  }
  this->irreps_grcam.QuickSortAscending();
  this->irreps.SetSize(0);
  this->characterTable.SetSize(0);
  for (int i = 0; i < irreps_grcam.size; i ++)
  { this->irreps.AddOnTop(this->irreps_grcam[i].MakeOtherGroupRepresentationClass());
    this->characterTable.AddOnTop(&(this->irreps[i].theCharacteR));
  }
  this->theGroup.flagCharTableIsComputed = true;
  this->theGroup.flagIrrepsAreComputed = true;
  //stOutput << this->PrettyPrintCharacterTable();
}

template <typename elementSomeGroup>
void FiniteGroup<elementSomeGroup>::ComputeIrreducibleRepresentations()
{ if (this->ComputeIrreducibleRepresentationsWithFormulas)
    this->ComputeIrreducibleRepresentationsWithFormulas(*this);
  else if (this->irreps_grcam.size != 0 || this->irreps.size != 0)
      this->ComputeIrreducibleRepresentationsTodorsVersion();
  else
      crash << "FiniteGroup<elementSomeGroup>::ComputeIrreducibleRepresentations: We must have either a formula to generate the irreps, or a list of irreps with everything in their tensor products" << __FILE__ << ":" << __LINE__;
}

template <typename somegroup, typename coefficient>
const ClassFunction<somegroup, coefficient>& GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::GetCharacter()
{ if (this->flagCharacterIsComputed)
    return this->theCharacteR;
  this->theCharacteR.G = this->ownerGroup;
  this->theCharacteR.data.SetSize(this->ownerGroup->ConjugacyClassCount());
  for (int cci = 0; cci < this->ownerGroup->ConjugacyClassCount(); cci ++)
    this->theCharacteR.data[cci] = this->GetMatrixElement(this->ownerGroup->conjugacyClasseS[cci].representative).GetTrace();
  this->flagCharacterIsComputed = true;
  return this->theCharacteR;
}

template <typename somegroup, typename coefficient>
coefficient GroupRepresentationCarriesAllMatrices<somegroup, coefficient>::GetNumberOfComponents()
{ ClassFunction<somegroup, coefficient> X;
  X = GetCharacter();
  return X.Norm();
}

#endif
