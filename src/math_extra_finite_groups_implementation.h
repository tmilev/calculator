// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfImplementationHeaderFiniteGroupsIncluded
#define vpfImplementationHeaderFiniteGroupsIncluded

#include "math_extra_finite_groups.h"
#include "math_general_polynomial_computations_advanced_implementation.h"

template <class Coefficient>
std::string FinitelyGeneratedMatrixMonoid<Coefficient>::toString(FormatExpressions* theFormat) const {
  std::stringstream out;
  out << "Number of generators: " << this->theGenerators.size;
  out << "<br>Number of elements: " << this->theElements.size;
  out << "<br>The elements follow.";
  int numEltstoDisplay = this->theElements.size;
  if (numEltstoDisplay > 100) {
    out << "<b>Displaying only the first " << 100 << " elements.</b>";
    numEltstoDisplay = 100;
  }
  for (int i = 0; i < numEltstoDisplay; i ++) {
    out << "<br>" << this->theElements[i].toStringMatrixForm(theFormat);
  }
  return out.str();
}

template <typename elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::ComputeAllElements(
  bool andWords, int MaxElements
) {
  MacroRegisterFunctionWithName("FiniteGroup::ComputeAllElements");
  this->checkConsistency();
  //double startTimeDebug= global.getElapsedSeconds();
  this->sizePrivate = this->SizeByFormulaOrNeg1();
  if (this->sizePrivate > 0 && MaxElements > 0 && this->sizePrivate > MaxElements) {
    return false;
  }
  if (!this->ComputeAllElementsLargeGroup(andWords, MaxElements)) {
    return false;
  }
  return true;
}

template <typename elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::ComputeAllElementsLargeGroup(bool andWords, int MaxElements) {
  MacroRegisterFunctionWithName("Subgroup::ComputeAllElementsLargeGroup");
  this->initializeGenerators();
  if (this->generators.size == 0) {
    global.fatal << "Groups with zero generators are not allowed: if you wanted to create a trivial group, "
    << " trivial groups are assumed to have a generator (the identity). " << global.fatal;
  }
  this->theElements.clear();
  elementSomeGroup currentElement;
  currentElement.makeIdentity(this->generators[0]);
  this->theElements.addOnTop(currentElement);
  this->theWords.setSize(0);
  if (andWords) {
    this->theWords.setSize(1);
    this->theWords.lastObject()->setSize(0);
  }
  ProgressReport theReport(1000, GlobalVariables::Response::ReportType::general);
  // Warning: not checking whether the generators have repetitions.
  for (int j = 0; j < this->theElements.size; j ++) {
    for (int i = 0; i < this->generators.size; i ++) {
      currentElement = this->generators[i] * this->theElements[j];
      if (this->theElements.addOnTopNoRepetition(currentElement) && andWords) {
        if (this->GetWordByFormula == 0) {
          this->theWords.addOnTop(this->theWords[j]);
          this->theWords.lastObject()->addOnTop(i);
        } else {
          this->theWords.setSize(this->theWords.size + 1);
          this->GetWordByFormula(*this, currentElement, *this->theWords.lastObject());
        }
      }
      if (theReport.tickAndWantReport()) {
        std::stringstream reportStream;
        LargeInteger sizeByFla = this->SizeByFormulaOrNeg1();
        reportStream << "So far, generated " << this->theElements.size << " elements";
        if (sizeByFla > 0) {
          reportStream << " out of " << sizeByFla.toString();
        }
        reportStream << ".";
        theReport.report(reportStream.str());
      }
      if (MaxElements > 0) {
        if (this->theElements.size > MaxElements) {
          return false;
        }
      }
    }
  }
  theReport.ticksPerReport = 1;
  if (theReport.tickAndWantReport()) {
    std::stringstream reportStream;
    reportStream << "Generated group with a total of " << this->theElements.size << " elements. ";
    theReport.report(reportStream.str());
  }
  this->sizePrivate = this->theElements.size;
  this->flagAllElementsAreComputed = true;
  if (andWords) {
    this->flagWordsComputed = true;
  }
  return true;
}

template <class elementGroup, class elementRepresentation>
void OrbitIterator<elementGroup, elementRepresentation>::initialize(
  const List<elementGroup>& inputGenerators,
  const elementRepresentation& inputElement,
  const GroupActionWithName &inputGroupAction
) {
  this->resetNoActionChange();
  this->theGroupGeneratingElements = inputGenerators;
  this->theGroupAction.name = inputGroupAction.name;
  this->theGroupAction.actOn = inputGroupAction.actOn;
  this->currentLayer->clear();
  this->currentLayer->addOnTop(inputElement);
  this->indexCurrentElement = 0;
  this->previousLayer->clear();
  this->nextLayer->clear();
}

template <class elementGroup, class elementRepresentation>
const elementRepresentation& OrbitIterator<elementGroup, elementRepresentation>::GetCurrentElement() {
  return (*this->currentLayer)[this->indexCurrentElement];
}

template <class elementGroup, class elementRepresentation>
std::string OrbitIterator<elementGroup, elementRepresentation>::ToStringLayerSize() const {
  std::stringstream out;
  out << " Total number of elements stored in layers: "
  << this->privateLayer1.size + this->privateLayer2.size + this->privateLayer3.size;
  out << " Previous layer: " << this->previousLayer->size
  << "; current layer: " << this->currentLayer->size
  << "; next layer: " << this->nextLayer->size;
  return out.str();
}

template <class elementGroup, class elementRepresentation>
bool OrbitIterator<elementGroup, elementRepresentation>::IncrementReturnFalseIfPastLast() {
  MacroRegisterFunctionWithName("OrbitIterator::IncrementReturnFalseIfPastLast");
  if (this->theGroupGeneratingElements.size == 0) {
    return false;
  }
  for (int i = 0; i < this->theGroupGeneratingElements.size; i ++) {
    this->theGroupAction.actOn(
      this->theGroupGeneratingElements[i],
      (*this->currentLayer)[this->indexCurrentElement],
      this->bufferRepresentationElement
    );
    if (!this->currentLayer->contains(this->bufferRepresentationElement)) {
      this->currentLayer->addOnTopNoRepetition(this->bufferRepresentationElement);
    }
  }
  this->indexCurrentElement ++;
  if (this->indexCurrentElement < this->currentLayer->size) {
    return true;
  }
  this->indexCurrentElement = 0;
  return false;
}

template <class elementGroup, class elementRepresentation>
bool OrbitIterator<elementGroup, elementRepresentation>::checkInitialization() const {
  this->theGroupAction.checkInitialization();
  return true;
}

template <class elementGroup, class elementRepresentation>
bool OrbitIterator<elementGroup, elementRepresentation>::incrementReturnFalseIfPastLastFALSE() {
  MacroRegisterFunctionWithName("OrbitIterator::incrementReturnFalseIfPastLastFALSE");
  if (this->theGroupGeneratingElements.size == 0) {
    return false;
  }
  this->checkInitialization();
  for (int i = 0; i < this->theGroupGeneratingElements.size; i ++) {
    this->theGroupAction.actOn(
      this->theGroupGeneratingElements[i],
      (*this->currentLayer)[this->indexCurrentElement],
      this->bufferRepresentationElement
    );
    if (
      !this->previousLayer->contains(this->bufferRepresentationElement) &&
      !this->currentLayer->contains(this->bufferRepresentationElement)
    ) {
      this->nextLayer->addOnTopNoRepetition(this->bufferRepresentationElement);
    }
  }
  this->indexCurrentElement ++;
  if (this->indexCurrentElement < this->currentLayer->size) {
    return true;
  }
  if (this->nextLayer->size == 0) {
    return false;
  }
  HashedList<elementRepresentation>* layerPtr = this->previousLayer;
  this->previousLayer = this->currentLayer;
  this->currentLayer = this->nextLayer;
  this->nextLayer = layerPtr;
  this->nextLayer->clear();
  this->indexCurrentElement = 0;
  return true;
}

template <class someGroup, class elementSomeGroup>
void SubgroupData<someGroup, elementSomeGroup>::initFromGroupAndGenerators(
  someGroup& inputGroup, const List<elementSomeGroup>& inputGenerators
) {
  MacroRegisterFunctionWithName("Subgroup::initFromGroupAndGenerators");
  if (&inputGenerators == &this->theSubgroup->generators) {
    List<elementSomeGroup> inputGeneratorsCopy = inputGenerators;
    this->initFromGroupAndGenerators(inputGroup, inputGeneratorsCopy);
    return;
  }
  this->theGroup = &inputGroup;
  this->theSubgroup = &this->theSubgroupMayBeHere;
  this->theSubgroup->generators.setSize(0);
  this->theSubgroup->generators.addOnTopNoRepetition(inputGenerators);//<- we have eliminated all repeating generators
  //(there shouldn't be any, but knowing I am one of the programmers...)
}

// A well chosen generating set for G can have the property that for interesting subgroups H,
// the usual choice of word in G for h∈H will be translatable into letters of H.
// This is the case for, in particular, the parabolic subgroups of the complete hyperoctahedral
// groups in the generating set that tacks on to the symmetric groups a complete set of ℤ₂ generators
template <class someGroup, class elementSomeGroup>
void SubgroupData<someGroup, elementSomeGroup>::makeTranslatableWordsSubgroup(
  someGroup &inputGroup, const List<elementSomeGroup>& subGenerators
) {
  MacroRegisterFunctionWithName("Subgroup::makeTranslatableWordsSubgroup");
  this->theGroup = &inputGroup;
  this->theSubgroup = &this->theSubgroupMayBeHere;
  this->theSubgroup->generators = subGenerators;
  this->superGeneratorSubWords.setSize(this->theGroup->generators.size);
  this->superGeneratorSubWordExists.setSize(this->theGroup->generators.size);
  for (int i = 0; i < superGeneratorSubWordExists.size; i ++) {
    superGeneratorSubWordExists[i] = false;
  }
  // done initializing things.  Now for actual code.
  // not the most comprehensive algorithm, is it?
  // I mean, not that I care to prove it incomplete at this point.
  // and so much work for this silly little speed optimization, too
  for (int i = 0; i < this->theGroup->generators.size; i ++) {
    int si = this->theSubgroup->generators.getIndex(this->theGroup->generators[i]);
    if (si != - 1) {
      this->superGeneratorSubWords[i].addOnTop(si);
      this->superGeneratorSubWordExists[i] = true;
    }
  }
  this->theSubgroup->GetWordByFormula = TranslatableWordsSubgroupElementGetWord<elementSomeGroup>;
  this->theSubgroup->parentRelationship = this;
}

template <class someGroup, class elementSomeGroup>
bool SubgroupData<someGroup, elementSomeGroup>::checkInitialization() {
  if (this == 0) {
    global.fatal << "A subgroup has the ``this'' pointer equal to zero. " << global.fatal;
  }
  if (this->theGroup == 0) {
    global.fatal << "This is a programming error: subgroup not initialized when it should be" << global.fatal;
  }
  return true;
}

template <typename elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::HasElement(const elementSomeGroup& g) {
  if (!this->flagAllElementsAreComputed) {
    this->ComputeAllElements(false);
  }
  return this->theElements.contains(g);
}

template <class elementSomeGroup>
void FiniteGroup<elementSomeGroup>::GetSignCharacter(Vector<Rational>& outputCharacter) {
  MacroRegisterFunctionWithName("FiniteGroup::GetSignCharacter");
  if (!this->flagCCsComputed) {
    this->computeConjugacyClassSizesAndRepresentatives();
  }
  outputCharacter.setSize(this->ConjugacyClassCount());
  for (int i = 0; i < this->ConjugacyClassCount(); i ++) {
    outputCharacter[i] = this->conjugacyClasses[i].representative.Sign();
  }
}

template <class someGroup, class elementSomeGroup>
void SubgroupData<someGroup, elementSomeGroup>::initialize() {
  this->theGroup = 0;
  this->theSubgroup = 0;
  this->theSubgroupMayBeHere.initialize();
  this->ccRepresentativesPreimages.setSize(0);
  this->generatorPreimages.setSize(0);
  this->flagCosetSetsComputed = false;
  this->flagCosetRepresentativesComputed = false;
}

template <class someGroup, class elementSomeGroup>
SubgroupData<someGroup, elementSomeGroup>::SubgroupData() {
  this->initialize();
}

template <typename elementSomeGroup>
SubgroupData<FiniteGroup<elementSomeGroup>, elementSomeGroup> FiniteGroup<elementSomeGroup>::ParabolicKindaSubgroupGeneratorSubset(
  const List<int>& subgenids
) {
  SubgroupData<FiniteGroup<elementSomeGroup>, elementSomeGroup> out;
  List<elementSomeGroup> subgens;
  subgens.setSize(subgenids.size);
  for (int i = 0; i < subgenids.size; i ++) {
    subgens[i] = this->generators[subgenids[i]];
  }
  out.makeTranslatableWordsSubgroup(*this, subgens);
  return out;
}

template <typename elementSomeGroup>
GroupRepresentation<FiniteGroup<elementSomeGroup>, Rational> FiniteGroup<elementSomeGroup>::GetEmptyRationalRepresentation() {
  GroupRepresentation<FiniteGroup<elementSomeGroup>, Rational> out;
  out.ownerGroup = this;
  return out;
}

template <class elementSomeGroup>
template <typename Coefficient>
Coefficient FiniteGroup<elementSomeGroup>::GetHermitianProduct(
  const Vector<Coefficient>& X1, const Vector<Coefficient>& X2
) {
  Coefficient acc = 0;
  for (int i = 0; i < X1.size; i ++) {
    acc += MathRoutines::ComplexConjugate(X1[i]) * X2[i] * this->conjugacyClasses[i].size;
    if (this->conjugacyClasses[i].size == 0) {
      global.fatal << "Error: conjugacy class size is zero." << global.fatal;
    }
  }
  return acc / this->getSize();
}

template <class elementSomeGroup>
int FiniteGroup<elementSomeGroup>::ConjugacyClassCount() const {
  if (!this->flagCCRepresentativesComputed) {
    global.fatal << "Requesting conjugacy class count but conjugacy class representatives are not computed." << global.fatal;
  }
  return this->conjugacyClasses.size;
}

template <class someGroup, class elementSomeGroup>
void SubgroupData<someGroup, elementSomeGroup>::ComputeCCRepresentativesPreimages() {
  MacroRegisterFunctionWithName("Subgroup::ComputeCCRepresentativesPreimages");
  this->ccRepresentativesPreimages.setSize(this->theSubgroup->ConjugacyClassCount());
  if (this->theSubgroup->generators.size == 0) {
    global.fatal << "At this computation the group must have initialized generators. " << global.fatal;
  }
  if (this->theGroup->generators.size == 0) {
    global.fatal << "Parent group must have initialized generators. " << global.fatal;
  }
  for (int i = 0; i < this->theSubgroup->conjugacyClasses.size; i ++) {
    elementSomeGroup& scr = this->theSubgroup->conjugacyClasses[i].representative;
    bool notFound = true;
    for (int j = 0; notFound && j < this->theGroup->conjugacyClasses.size; j ++) {
      elementSomeGroup& gcr = this->theGroup->conjugacyClasses[j].representative;
      if (this->theGroup->areConjugate(scr, gcr)) {
        this->ccRepresentativesPreimages[i] = j;
        notFound = false;
        break;
      }
    }
    if (notFound) {
      global.fatal << "Programming error: couldn't find preimage "
      << "of the subgroup conjugacy class representative " << scr << global.fatal;
    }
  }
}

template <class elementSomeGroup>
std::string FiniteGroup<elementSomeGroup>::ToStringElements(FormatExpressions* theFormat) const {
  MacroRegisterFunctionWithName("FiniteGroup::ToStringElements");
  (void) theFormat;
  if (!this->flagAllElementsAreComputed) {
    return "";
  }
  std::stringstream out;
  out << "<br>Elements of the group(" << this->theElements.size << " total):\n ";
  if (this->theElements.size <= 100) {
    for (int i = 0; i < this->theElements.size; i ++) {
      out << i << ". " << this->theElements[i].toString() << "\n";
    }
  } else {
    out << "... too many, not displaying. ";
  }
  return out.str();
}

template <class elementSomeGroup>
std::string FiniteGroup<elementSomeGroup>::ToStringConjugacyClasses(FormatExpressions* theFormat) {
  MacroRegisterFunctionWithName("Subgroup::ToStringConjugacyClasses");
  std::stringstream out;
  out << "<br>Size: " << this->getSize().toString() << "\n";
  FormatExpressions charPolyFormat;
  charPolyFormat.polynomialAlphabet.setSize(1);
  charPolyFormat.polynomialAlphabet[0] = "q";
  //  out << "Number of Vectors<Rational>: " << this->RootSystem.size<< "\n
  if (this->ConjugacyClassCount() > 0) {
    out << "<br>" << this->ConjugacyClassCount() << " conjugacy classes total.\n";
    for (int i = 0; i < this->conjugacyClasses.size; i ++) {
      out << "<hr>Conjugacy class " << i + 1 << ": ";
      if (this->conjugacyClasses[i].flagRepresentativeComputed) {
        out << " represented by " << this->conjugacyClasses[i].representative.toString(theFormat) << ". ";
        //out << this->conjugacyClasses[i].ToStringInvariants(theFormat); FIXME: do this sanely
      } else {
        out << " representative not computed. ";
      }
      out << "Class size: " << this->conjugacyClasses[i].size.toString() << ".\n<br>\n";
      if (this->flagCharPolysAreComputed) {
        if (i < this->CCsStandardRepCharPolys.size) {
          out << "Characteristic poly standard representation: "
          << this->CCsStandardRepCharPolys[i].toString(&charPolyFormat);
          const List<int>& currentHashList = this->CCsStandardRepCharPolys.GetHashArray(
            this->CCsStandardRepCharPolys.getHash(this->CCsStandardRepCharPolys[i])
          );
          int numClassesSameCharPoly = 0;
          for (int j = 0; j < currentHashList.size; j ++) {
            if (this->CCsStandardRepCharPolys[currentHashList[j]] == this->CCsStandardRepCharPolys[i]) {
              numClassesSameCharPoly ++;
            }
          }
          if (numClassesSameCharPoly > 1) {
            out << " The characteristic polynomial is the same as that of " << numClassesSameCharPoly
            << " conjugacy classes, numbers: ";
            for (int j = 0; j < currentHashList.size; j ++) {
              if (this->CCsStandardRepCharPolys[currentHashList[j]] == this->CCsStandardRepCharPolys[i]) {
                out << currentHashList[j] + 1 << (j == currentHashList.size - 1 ? "" : ", ");
              }
            }
          }
        }
      }
      if (!this->conjugacyClasses[i].flagElementsComputed) {
        continue;
      }
      out << " The elements of the class are: ";
      int numEltsToDisplay = this->conjugacyClasses[i].theElements.size;
      if (this->conjugacyClasses[i].theElements.size > 10) {
        out << " too many, displaying the first 10 elements only: ";
        numEltsToDisplay = 10;
      }
      for (int j = 0; j < numEltsToDisplay; j ++) {
        out << this->conjugacyClasses[i].theElements[j].toString(theFormat);
        if (j != numEltsToDisplay - 1) {
          out << ", ";
        }
      }
      out << ". ";
    }
  }
  return out.str();
}

template <class elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::checkInitialization() const {
  this->checkConsistency();
  if (this->generators.size == 0) {
    global.fatal << "Error: group has 0 generators, "
    << "which is not allowed. If you want to use the trivial "
    << "group, you are still supposed to put the "
    << "identity element in the group generators. " << global.fatal;
  }
  return true;
}

template <class elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::CheckOrthogonalityCharTable() {
  MacroRegisterFunctionWithName("FiniteGroup::CheckOrthogonalityCharTable");
  for (int i = 0; i < this->characterTable.size; i ++) {
    for (int j = i; j < this->characterTable.size; j ++) {
      ClassFunction<FiniteGroup, Rational>& leftChar = this->characterTable[i];
      ClassFunction<FiniteGroup, Rational>& rightChar = this->characterTable[j];
      Rational theScalarProd = this->GetHermitianProduct(leftChar.data, rightChar.data);
      if (j != i) {
        if (theScalarProd != 0) {
          global.fatal << "Error: the character table is not orthonormal: char number " << i + 1 << " = "
          << leftChar.toString() << " is not orthogonal to char number "
          << j + 1 << " = " << rightChar.toString() << ". <br>The entire char table is: "
          << this->PrettyPrintCharacterTable() << global.fatal;
        }
      }
      if (j == i) {
        if (theScalarProd != 1) {
          global.fatal << "Error: the character table is not orthonormal: char number " << i + 1 << " = "
          << leftChar.toString() << " is not of norm 1. "
          << "<br>The entire char table is: " << this->PrettyPrintCharacterTable() << global.fatal;
        }
      }
    }
  }
  return true;
}

template <class elementSomeGroup>
void FiniteGroup<elementSomeGroup>::ComputeGeneratorsConjugacyClasses() {
  MacroRegisterFunctionWithName("FiniteGroup::ComputeGeneratorsConjugacyClasses");
  if (this->flagGeneratorsConjugacyClassesComputed) {
    return;
  }
  this->unionGeneratorsCC.clear();
  this->unionGeneratorsCC.addOnTopNoRepetition(this->generators);
  elementSomeGroup currentElt;
  for (int i = 0; i < this->unionGeneratorsCC.size; i ++) {
    for (int j = 0; j < this->generators.size; j ++) {
      elementSomeGroup::conjugationAction(this->generators[j], this->unionGeneratorsCC[i], currentElt);
      this->unionGeneratorsCC.addOnTopNoRepetition(currentElt);
    }
  }
  this->flagGeneratorsConjugacyClassesComputed = true;
}

template <class elementSomeGroup>
void FiniteGroup<elementSomeGroup>::ComputeCCSizeOrCCFromRepresentative(
  ConjugacyClass& inputOutputClass, bool storeCC
) {
  MacroRegisterFunctionWithName("FiniteGroup::ComputeCCSizesFromCCRepresentatives");
  OrbitIterator<elementSomeGroup, elementSomeGroup> theOrbitIterator;
  theOrbitIterator.theGroupAction.actOn = elementSomeGroup::conjugationAction;
  theOrbitIterator.theGroupAction.name = "conjugation action";
  theOrbitIterator.initialize(this->generators, inputOutputClass.representative, theOrbitIterator.theGroupAction);
  inputOutputClass.size = 1;
  if (storeCC) {
    inputOutputClass.theElements.setSize(0);
    inputOutputClass.theElements.addOnTop(inputOutputClass.representative);
  }
  while (theOrbitIterator.IncrementReturnFalseIfPastLast()) {
    inputOutputClass.size ++;
    if (storeCC) {
      if (inputOutputClass.theElements.contains(theOrbitIterator.GetCurrentElement())) {
        global.fatal << " !element " << theOrbitIterator.GetCurrentElement().toString()
        << " already contained !" << global.fatal;
      }
      inputOutputClass.theElements.addOnTop(theOrbitIterator.GetCurrentElement());
    }
  }
}

template <class elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::CheckConjugacyClassRepsMatchCCsizes() {
  MacroRegisterFunctionWithName("FiniteGroup::CheckConjugacyClassRepsMatchCCsizes");
  LargeInteger computedSize = 0;
  for (int i = 0; i < this->conjugacyClasses.size; i ++) {
    LargeInteger oldCCsize = this->conjugacyClasses[i].size;
    this->ComputeCCSizeOrCCFromRepresentative(this->conjugacyClasses[i], true);
    if (oldCCsize != this->conjugacyClasses[i].size) {
      global.fatal << "The precomputed size " << oldCCsize.toString()
      << " of the class represented by "
      << this->conjugacyClasses[i].representative.toString()
      << " doesn't match actual class size which is: "
      << this->conjugacyClasses[i].size.toString()
      << global.fatal;
    }
    computedSize += oldCCsize;
  }
  if (computedSize != this->sizePrivate) {
    global.fatal << "Computed size " << computedSize.toString() << " is different from recorded size "
    << sizePrivate.toString() << global.fatal;
  }
  if (this->SizeByFormulaOrNeg1() > 0) {
    if (computedSize != this->SizeByFormulaOrNeg1()) {
      global.fatal << "Computed size is different from size dicated by formula which is: "
      << this->SizeByFormulaOrNeg1().toString() << global.fatal;
    }
  }
  return true;
}

template <class elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::ComputeCCRepresentatives() {
  MacroRegisterFunctionWithName("FiniteGroup::ComputeCCRepresentatives");
  // This algorithm is effective if the sum of the sizes of the conjugacy classes
  // of the generators is small.
  // The algorithm "hops" from conjugacy class representative to conjugacy representative
  // by multiplying the representatives by the each element of each conjugacy class of the generators.
  // We prove this algorithm is guaranteed to generate all conjugacy classes.
  // Let g be a generator, H a conjugacy class, and h \in H an arbitrary element of H.
  // Suppose gH intersects a conjugacy class F. We claim there exists a generator g and an
  // element x such that x g x^{- 1} h\in F. Indeed, as gH intersects F, g x^{- 1} h x\in F for some
  // element x, and therefore x g x^{- 1} h =x(g x^{- 1} h x) x^{- 1}\in F, which proves our claim.
  // The preceding considerations show that
  // a representative of each conjugacy class containing an element from g H can be obtained
  // by applying the entire conjugacy class of g onto an arbitrary representative of H.
  // As we are using the conjugacy classes of a set of generators, this shows that the following
  // algorithm will generate a representative of each conjugacy class.

  // First we compute the generator's conjugacy classes:
  this->ComputeGeneratorsConjugacyClasses();

  ProgressReport theReport;
  elementSomeGroup currentElement;
  LargeInteger groupSizeByFla = this->SizeByFormulaOrNeg1();
  this->flagCharPolysAreComputed = true;
  for (int phase = 0; phase < 2; phase ++) {
    // In phase 0 we try to add a new conjugacy class only if
    // the class has a new character polynomial. In phase 1 we try every single conjugacy class,
    // in case there are two non-conjugate elements with the same char poly.
    for (int i = 0; i < this->conjugacyClasses.size; i ++) {
      for (int j = 0; j < this->unionGeneratorsCC.size; j ++) {
        if (theReport.tickAndWantReport()) {
          std::stringstream reportStream;
          reportStream << "Exploring conjugacy class " << i + 1
          << " out of " << this->conjugacyClasses.size
          << " generator " << j + 1 << " out of " << this->unionGeneratorsCC.size;
          theReport.report(reportStream.str());
        }
        currentElement = this->conjugacyClasses[i].representative * this->unionGeneratorsCC[j];
        this->registerConjugacyClass(currentElement, phase == 0);
        if (this->sizePrivate == groupSizeByFla) {
          return true;
        }
      }
    }
  }
  if (groupSizeByFla > 0) {
    if (this->sizePrivate != groupSizeByFla) {
      global.fatal << "Something went very wrong: number of elements "
      << "generated is not equal to group size by formula. " << global.fatal;
    }
  }
  return true;
}

template <class elementSomeGroup>
void FiniteGroup<elementSomeGroup>::computeConjugacyClassSizesAndRepresentatives(bool useComputeCCSizesRepresentativesWords) {
  MacroRegisterFunctionWithName("FiniteGroup::computeConjugacyClassSizesAndRepresentatives");
  this->checkConsistency();
  if (this->GetSizeByFormula != 0) {
    LargeInteger theSize = this->GetSizeByFormula(*this);
    //extended digit separators only appear in cxx14
    if (theSize > 100000000) {
      if (this->flagCanComputeCCsWithOrbitIterator) {
        this->ComputeCCSizesAndRepresentativesWithOrbitIterator();
        return;
      }
    }
  }
  if (useComputeCCSizesRepresentativesWords) {
    this->computeConjugacyClassesSizesRepresentativesWords();
  }
}

template <class elementSomeGroup>
void FiniteGroup<elementSomeGroup>::ComputeCCSizesAndRepresentativesWithOrbitIterator() {
  MacroRegisterFunctionWithName("FiniteGroup::ComputeCCSizesAndRepresentativesWithOrbitIterator");
  if (this->flagCCRepresentativesComputed) {
    return;
  }

  this->initializeGenerators();
  this->checkInitialization();
  this->CCsStandardRepCharPolys.clear();
  this->conjugacyClasses.setSize(0);
  this->sizePrivate = 0;

  elementSomeGroup currentElt;
  currentElt.makeIdentity(this->generators[0]);
  static int recursionCount = 0;
  recursionCount ++;
  if (recursionCount > 100) {
    global.fatal << "Recursion too deep: something is very wrong. " << global.fatal;
  }
  this->registerConjugacyClass(currentElt, false);
  this->ComputeCCRepresentatives();
  recursionCount --;
  this->flagCCRepresentativesComputed = true;
}

template <class elementSomeGroup>
void FiniteGroup<elementSomeGroup>::ComputeCCfromCCindicesInAllElements(const List<List<int> >& ccIndices) {
  MacroRegisterFunctionWithName("FiniteGroup::ComputeCCfromCCindicesInAllElements");
  this->conjugacyClasses.setSize(ccIndices.size);
  for (int i = 0; i < ccIndices.size; i ++) {
    this->conjugacyClasses[i].size = ccIndices[i].size;
    this->conjugacyClasses[i].indicesEltsInOwner = ccIndices[i];
    this->conjugacyClasses[i].theElements.setSize(ccIndices[i].size);
    for (int j = 0; j < ccIndices[i].size; j ++) {
      this->conjugacyClasses[i].theElements[j] = this->theElements[ccIndices[i][j]];
    }
    this->conjugacyClasses[i].representative = this->theElements[ccIndices[i][0]];
    this->conjugacyClasses[i].flagRepresentativeComputed = true;
    this->conjugacyClasses[i].flagElementsComputed = true;
  }
  this->flagCCsComputed = true;
  this->flagCCRepresentativesComputed = true;
  this->conjugacyClasses.quickSortAscending();
}

template <class elementSomeGroup>
void FiniteGroup<elementSomeGroup>::computeConjugacyClassesFromAllElements() {
  MacroRegisterFunctionWithName("FiniteGroup::computeConjugacyClassesFromAllElements");
  this->ComputeAllElements(false, - 1);
  List<bool> Accounted;
  Accounted.initializeFillInObject(this->theElements.size, false);
  HashedList<int, MathRoutines::IntUnsignIdentity> theStack;
  theStack.setExpectedSize(this->theElements.size);
  List<elementSomeGroup> inversesOfGenerators;
  inversesOfGenerators.setSize(this->generators.size);
  for (int i = 0; i < this->generators.size; i ++) {
    inversesOfGenerators[i] = this->generators[i].inverse();
  }
  elementSomeGroup currentElement;
  List<List<int> > ccIndices;
  ccIndices.reserve(120);
  for (int i = 0; i < this->theElements.size; i ++) {
    if (!Accounted[i]) {
      theStack.clear();
      theStack.addOnTop(i);
      for (int j = 0; j < theStack.size; j ++) {
        for (int k = 0; k < this->generators.size; k ++) {
          currentElement = inversesOfGenerators[k] * this->theElements[theStack[j]] * this->generators[k];
          int accountedIndex = this->theElements.getIndexNoFail(currentElement);
          theStack.addOnTopNoRepetition(accountedIndex);
          Accounted[accountedIndex] = true;
        }
      }
      ccIndices.addOnTop(theStack);
      ccIndices.lastObject()->quickSortAscending();
    }
  }
  ccIndices.quickSortAscending();
  this->ComputeCCfromCCindicesInAllElements(ccIndices);
}

template<class Coefficient>
LargeInteger WeylGroupData::GetOrbitSize(Vector<Coefficient>& theWeight) {
  MacroRegisterFunctionWithName("WeylGroup::GetOrbitSize");
  // I read somewhere, I think it was a paper by W. de Graaf, that the stabilizer
  // of a weight is generated by
  // the root reflections that stabilize the weight.
  // Whether my memory has served me well shall
  // be seen through implementing this function.
  // In particular, we will compute all root reflections that stabilize the weight,
  // then get a Dynkin diagram from these roots, then compute the size of the stabilizer,
  // and finally compute the size of the orbit. I will check numerically if everything is ok
  // all the way up to E6.
  Vector<Coefficient> currentWeight;
  Vectors<Rational> theStabilizingRoots;
  for (int i = 0; i < this->RootsOfBorel.size; i ++) {
    this->ReflectBetaWRTAlpha(this->RootsOfBorel[i], theWeight, false, currentWeight);
    if (currentWeight == theWeight) {
      theStabilizingRoots.addOnTop(this->RootsOfBorel[i]);
    }
  }
  DynkinDiagramRootSubalgebra theStabilizerSubsystem;
  theStabilizerSubsystem.ComputeDiagramTypeModifyInput(theStabilizingRoots, *this);
  DynkinType theStabilizerDynkinType;
  theStabilizerSubsystem.GetDynkinType(theStabilizerDynkinType);
  Rational resultRat = this->theGroup.getSize();
  resultRat /= theStabilizerDynkinType.GetWeylGroupSizeByFormula();
  LargeInteger result;
  if (!resultRat.isInteger(&result)) {
    global.fatal << "Something has gone very wrong: orbit size reported to be " << resultRat.toString()
    << " which is non-integer!" << global.fatal;
  }
  bool doDebug = true;
  if (doDebug) {
    if (result < 100000) {
      HashedList<Vector<Coefficient> > comparisonOrbit;
      this->generateOrbit(theWeight, false, comparisonOrbit, false, - 1, 0, - 1);
      if (result != comparisonOrbit.size) {
        global.fatal << "Actual orbit of "
        << theWeight.toString()
        << " has size "
        << comparisonOrbit.size << " but I computed "
        << " the orbit size to be " << result.toString()
        << ". This may be a mathematical error. " << global.fatal;
      }
    }
  }
  return result;
}

template <class Coefficient>
bool WeylGroupAutomorphisms::GenerateOuterOrbit(
  Vectors<Coefficient>& theWeights,
  HashedList<Vector<Coefficient> >& output,
  HashedList<ElementWeylGroupAutomorphisms>* outputSubset,
  int UpperLimitNumElements
) {
  MacroRegisterFunctionWithName("WeylGroup::GenerateOuterOrbit");
  this->checkInitialization();
  this->ComputeOuterAutoGenerators();
  List<MatrixTensor<Rational> > theOuterGens = this->theOuterAutos.theGenerators;
  output.clear();
  for (int i = 0; i < theWeights.size; i ++) {
    output.addOnTop(theWeights[i]);
  }
  Vector<Coefficient> currentRoot;
  ElementWeylGroupAutomorphisms currentElt;
  int numElementsToReserve = MathRoutines::Minimum(UpperLimitNumElements, 1000000);
  output.setExpectedSize(numElementsToReserve);
  ProgressReport theReport(3000, GlobalVariables::Response::ReportType::general);
  SimpleReflectionOrOuterAutomorphism theGen;
  if (outputSubset != nullptr) {
    currentElt.makeIdentity(*this);
    outputSubset->setExpectedSize(numElementsToReserve);
    outputSubset->clear();
    outputSubset->addOnTop(currentElt);
  }
  int numGens = this->theWeyl->getDimension() + theOuterGens.size;
  for (int i = 0; i < output.size; i ++) {
    for (int j = 0; j < numGens; j ++) {
      if (j < this->theWeyl->getDimension()) {
        currentRoot = output[i];
        this->theWeyl->reflectSimple(j, currentRoot);
      } else {
        theOuterGens[j - this->theWeyl->getDimension()].actOnVectorColumn(output[i], currentRoot);
      }
      if (output.addOnTopNoRepetition(currentRoot)) {
        if (outputSubset != nullptr) {
          currentElt.makeIdentity(*this);
          theGen.MakeSimpleReflection(j);
          currentElt.generatorsLastAppliedFirst.addOnTop(theGen);
          currentElt.generatorsLastAppliedFirst.addListOnTop((*outputSubset)[i].generatorsLastAppliedFirst);
          outputSubset->addOnTop(currentElt);
        }
      }
      if (UpperLimitNumElements > 0) {
        if (output.size >= UpperLimitNumElements) {
          return false;
        }
      }
      if (theReport.tickAndWantReport()) {
        std::stringstream reportStream;
        reportStream << "Generating outer orbit, " << output.size
        << " elements found so far, Weyl group type: "
        << this->theWeyl->theDynkinType.toString() << ". ";
        theReport.report(reportStream.str());
      }
    }
  }
  return true;
}

template <class Coefficient>
void WeylGroupData::RaiseToDominantWeight(
  Vector<Coefficient>& theWeight,
  int* sign,
  bool* stabilizerFound,
  ElementWeylGroup* raisingElt
) {
  MacroRegisterFunctionWithName("WeylGroup::RaiseToDominantWeight");
  if (sign != nullptr) {
    *sign = 1;
  }
  if (stabilizerFound != nullptr) {
    *stabilizerFound = false;
  }
  Coefficient theScalarProd;
  int theDim = this->getDimension();
  SimpleReflection theGen;
  if (raisingElt != nullptr) {
    raisingElt->makeIdentity(*this);
  }
  for (bool found = true; found; ) {
    found = false;
    for (int i = 0; i < theDim; i ++) {
      theScalarProd = this->GetScalarProdSimpleRoot(theWeight, i);
      if (theScalarProd.isNegative()) {
        found = true;
        theScalarProd *= 2;
        theScalarProd /= this->cartanSymmetric.elements[i][i];
        theWeight[i] -= theScalarProd;
        if (sign != nullptr) {
          *sign *= - 1;
        }
        theGen.MakeSimpleReflection(i);
        if (raisingElt != nullptr) {
          raisingElt->generatorsLastAppliedFirst.addOnTop(theGen);//warning order of raising element is reversed, must reverse back
        }
      }
      if (stabilizerFound != nullptr) {
        if (theScalarProd.isEqualToZero()) {
          *stabilizerFound = true;
        }
      }
    }
  }
  if (raisingElt != nullptr) {
    raisingElt->generatorsLastAppliedFirst.reverseElements();
    raisingElt->MakeCanonical();
  }
}

template <class Coefficient>
void WeylGroupData::reflectRhoSimple(int index, Vector<Coefficient>& theVector) const {
  Coefficient alphaShift, tempRat;
  alphaShift = 0;
  for (int i = 0; i < this->cartanSymmetric.numberOfColumns; i ++) {
    tempRat = theVector[i];
    tempRat *= this->cartanSymmetric.elements[index][i] * (- 2);
    alphaShift += tempRat;
  }
  alphaShift /= this->cartanSymmetric.elements[index][index];
  alphaShift -= 1;
  theVector[index] += alphaShift;
}

template <class Coefficient>
bool WeylGroupData::generateOrbit(
  Vectors<Coefficient>& theWeights,
  bool RhoAction,
  HashedList<Vector<Coefficient> >& output,
  bool UseMinusRho,
  int expectedOrbitSize,
  HashedList<ElementWeylGroup>* outputSubset,
  int UpperLimitNumElements
) {
  MacroRegisterFunctionWithName("WeylGroup::generateOrbit");
  output.clear();
  for (int i = 0; i < theWeights.size; i ++) {
    output.addOnTopNoRepetition(theWeights[i]);
  }
  Vector<Coefficient> currentRoot;
  ElementWeylGroup currentElt;
  if (expectedOrbitSize <= 0) {
    if (!this->theGroup.getSize().IsIntegerFittingInInt(&expectedOrbitSize)) {
      expectedOrbitSize = - 1;
    }
  }
  if (UpperLimitNumElements > 0 && expectedOrbitSize > UpperLimitNumElements) {
    expectedOrbitSize = UpperLimitNumElements;
  }
  if (expectedOrbitSize > 10000000) {
    expectedOrbitSize = 10000000;
  }
  output.setExpectedSize(expectedOrbitSize);
  if (outputSubset != nullptr) {
    if (UpperLimitNumElements > 0) {
      expectedOrbitSize = MathRoutines::Minimum(UpperLimitNumElements, expectedOrbitSize);
    }
    currentElt.makeIdentity(*this);
    outputSubset->setExpectedSize(expectedOrbitSize);
    outputSubset->clear();
    outputSubset->addOnTop(currentElt);
  }
  ProgressReport theReport(1000, GlobalVariables::Response::ReportType::general);
  SimpleReflection theGen;
  for (int i = 0; i < output.size; i ++) {
    for (int j = 0; j < this->cartanSymmetric.numberOfRows; j ++) {
      currentRoot = output[i];
      if (theReport.tickAndWantReport()) {
        std::stringstream reportStream;
        reportStream << "So far found " << i + 1 << " elements in the orbit(s) of the starting weight(s) "
        << theWeights.toString() << ". ";
        theReport.report(reportStream.str());
      }
      if (!RhoAction) {
        this->reflectSimple(j, currentRoot);
      } else if (!UseMinusRho) {
        this->reflectRhoSimple(j, currentRoot);
      } else {
        this->reflectMinusRhoSimple(j, currentRoot);
      }
      if (output.addOnTopNoRepetition(currentRoot)) {
        if (outputSubset != nullptr) {
          currentElt.generatorsLastAppliedFirst.setSize(1);
          theGen.MakeSimpleReflection(j);
          currentElt.generatorsLastAppliedFirst[0] = theGen;
          currentElt.generatorsLastAppliedFirst.addListOnTop((*outputSubset)[i].generatorsLastAppliedFirst);
          currentElt.MakeCanonical();
          outputSubset->addOnTop(currentElt);
        }
      }
      if (UpperLimitNumElements > 0) {
        if (output.size >= UpperLimitNumElements) {
          return false;
        }
      }
    }
  }
  return true;
}

template <class Coefficient>
void WeylGroupData::reflectMinusRhoSimple(int index, Vector<Coefficient>& theVector) const {
  Coefficient alphaShift, tempRat;
  alphaShift = 0;
  for (int i = 0; i < this->cartanSymmetric.numberOfColumns; i ++) {
    tempRat = theVector[i];
    tempRat *= this->cartanSymmetric.elements[index][i];
    alphaShift += tempRat;
  }
  alphaShift *= - 2;
  alphaShift /= this->cartanSymmetric.elements[index][index];
  Coefficient one;
  one = 1;
  alphaShift += one;
  theVector[index] += alphaShift;
}

template <class Coefficient>
void WeylGroupData::reflectSimple(int index, Vector<Coefficient>& theVector) const {
  if (index < 0 || index >= this->cartanSymmetric.numberOfColumns) {
    global.fatal << "This is a programming error: simple reflection with respect to index "
    << index + 1 << " in a Weyl group of rank "
    << this->getDimension() << global.fatal;
  }
  Coefficient alphaShift, tempRat;
  alphaShift = 0;
  for (int i = 0; i < this->cartanSymmetric.numberOfColumns; i ++) {
    tempRat = theVector[i];
    tempRat *= this->cartanSymmetric.elements[index][i];
    alphaShift += tempRat;
  }
  alphaShift *= - 2;
  alphaShift /= this->cartanSymmetric.elements[index][index];
  theVector[index] += alphaShift;
}

template <class Coefficient>
bool WeylGroupData::isDominantWeight(const Vector<Coefficient>& theWeight) {
  int theDimension = this->getDimension();
  for (int i = 0; i < theDimension; i ++) {
    if (!this->IsDominantWRTgenerator(theWeight, i)) {
      return false;
    }
  }
  return true;
}

template<class Coefficient>
Coefficient WeylGroupData::weylDimFormulaSimpleCoords(
  Vector<Coefficient>& theWeightInSimpleCoords, const Coefficient& theRingUnit
) {
  Coefficient Result, buffer;
  Vector<Coefficient> rhoOverNewRing, rootOfBorelNewRing, sumWithRho;//<-to facilitate type conversion!
  rhoOverNewRing = this->rho;//<-type conversion here!
  Result = theRingUnit;
  for (int i = 0; i < this->RootsOfBorel.size; i ++) {
    rootOfBorelNewRing = this->RootsOfBorel[i]; //<-type conversion here!
    sumWithRho = rhoOverNewRing + theWeightInSimpleCoords;
    buffer = this->RootScalarCartanRoot(sumWithRho, rootOfBorelNewRing);
    buffer /= this->RootScalarCartanRoot(rhoOverNewRing, rootOfBorelNewRing);
    Result *= buffer;
  }
  return Result;
}

template<class Coefficient>
Vector<Coefficient> WeylGroupData::getFundamentalCoordinatesFromSimple(const Vector<Coefficient>& inputInFundamentalCoords) {
  Matrix<Rational>& tempMat = *this->GetMatrixSimpleToFundamentalCoords();
  Vector<Coefficient> result = inputInFundamentalCoords;
  tempMat.actOnVectorColumn(result);
  return result;
}

template<class Coefficient>
Vector<Coefficient> WeylGroupData::getSimpleCoordinatesFromEpsilon(const Vector<Coefficient>& inputInEpsCoords) {
  Vectors<Rational> simpleBasis, simpleBasisEpsCoords;
  simpleBasis.makeEiBasis(this->getDimension());
  this->getEpsilonCoordinates(simpleBasis, simpleBasisEpsCoords);
  Vector<Coefficient> result;
  result.setSize(this->getDimension());
  for (int i = 0; i < simpleBasisEpsCoords.size; i ++) {
    result[i] = simpleBasisEpsCoords[i].ScalarEuclidean(inputInEpsCoords);
  }
  Matrix<Rational> invertedCartanSymmetric = this->cartanSymmetric;
  invertedCartanSymmetric.invert();
  invertedCartanSymmetric.actOnVectorColumn(result);
  return result;
}

template<class Coefficient>
Vector<Coefficient> WeylGroupData::getFundamentalCoordinatesFromEpsilon(const Vector<Coefficient>& inputInEpsCoords) {
  return this->getFundamentalCoordinatesFromSimple(this->getSimpleCoordinatesFromEpsilon(inputInEpsCoords));
}

template<class Coefficient>
Vector<Coefficient> WeylGroupData::getSimpleCoordinatesFromFundamental(const Vector<Coefficient>& inputInFundamentalCoords) {
  Matrix<Rational>& tempMat = *this->GetMatrixFundamentalToSimpleCoords();
  Vector<Coefficient> result;
  result = inputInFundamentalCoords;
  tempMat.actOnVectorColumn(result);
  return result;
}

template<class Coefficient>
Vectors<Coefficient> WeylGroupData::getSimpleCoordinatesFromFundamental(const Vectors<Coefficient>& inputInFundamentalCoords) {
  Matrix<Rational>& tempMat = *this->GetMatrixFundamentalToSimpleCoords();
  Vectors<Coefficient> result;
  result = inputInFundamentalCoords;
  tempMat.actOnVectorsColumn(result);
  return result;
}

template<class Coefficient>
Coefficient WeylGroupData::weylDimFormulaFundamentalCoords(Vector<Coefficient>& weightFundCoords) {
  Vector<Coefficient> theWeightInSimpleCoords;
  theWeightInSimpleCoords = this->getSimpleCoordinatesFromFundamental(weightFundCoords);
  return this->weylDimFormulaSimpleCoords(theWeightInSimpleCoords);
}

template <class Coefficient>
bool WeylGroupData::FreudenthalEval(
  Vector<Coefficient>& inputHWfundamentalCoords,
  HashedList<Vector<Coefficient> >& outputDominantWeightsSimpleCoords,
  List<Coefficient>& outputMultsSimpleCoords,
  std::string* outputDetails,
  int UpperBoundFreudenthal
) {
  MacroRegisterFunctionWithName("WeylGroup::FreudenthalEval");
  for (int i = 0; i < inputHWfundamentalCoords.size; i ++) {
    if (inputHWfundamentalCoords[i] < 0) {
      if (outputDetails != nullptr) {
        *outputDetails = "The highest weight is not dominant and I cannot apply the Freudenthal formula.";
      }
      return false;
    }
  }
  this->ComputeRho(true);
  Vectors<Rational> EiBasis;
  EiBasis.makeEiBasis(this->getDimension());
  List<bool> Explored;
  Vector<Coefficient> hwSimpleCoords = this->getSimpleCoordinatesFromFundamental(inputHWfundamentalCoords);
  if (!this->GetAlLDominantWeightsHWFDIM(
    hwSimpleCoords, outputDominantWeightsSimpleCoords, UpperBoundFreudenthal, outputDetails
  )) {
    return false;
  }
  Explored.initializeFillInObject(outputDominantWeightsSimpleCoords.size, false);
  outputMultsSimpleCoords.setSize(outputDominantWeightsSimpleCoords.size);
  Vector<Coefficient> currentWeight, currentDominantRepresentative, convertor;
  Coefficient hwPlusRhoSquared;
  convertor = hwSimpleCoords;
  convertor += this->rho;//<-implicit type conversion here!!!!
  hwPlusRhoSquared = this->RootScalarCartanRoot(convertor, convertor);
  outputMultsSimpleCoords[0] = 1;
  Explored[0] = true;
  Coefficient BufferCoeff;
  ProgressReport theReport;
  for (int k = 1; k < outputDominantWeightsSimpleCoords.size; k ++) {
    Explored[k] = true;
    Coefficient& currentAccum = outputMultsSimpleCoords[k];
    currentAccum = 0;
    for (int j = 0; j < this->RootsOfBorel.size; j ++) {
      for (int i = 1; ; i ++) {
        currentWeight = outputDominantWeightsSimpleCoords[k];
        currentWeight += this->RootsOfBorel[j] * i;
        currentDominantRepresentative = currentWeight;
        this->RaiseToDominantWeight(currentDominantRepresentative);
        int theIndex = outputDominantWeightsSimpleCoords.getIndex(currentDominantRepresentative);
        if (theIndex == - 1) {
          break;
        }
        if (!Explored[theIndex]) {
          if (outputDetails != nullptr) {
            std::stringstream errorLog;
            errorLog << "This is an internal error check. "
            << "If you see it, that means that the Freudenthal algorithm implementation is "
            << "wrong (because the author of the implementation "
            << "tried to write less code than what is suggested by LiE).";
            *outputDetails = errorLog.str();
          }
          return false;
        }
        BufferCoeff = this->RootScalarCartanRoot(currentWeight, this->RootsOfBorel[j]);
        BufferCoeff *= outputMultsSimpleCoords[theIndex];
        currentAccum += BufferCoeff;
      }
    }
    currentAccum *= 2;
    convertor = outputDominantWeightsSimpleCoords[k];
    convertor += this->rho;
    BufferCoeff = hwPlusRhoSquared;
    BufferCoeff -= this->RootScalarCartanRoot(convertor, convertor);
    if (BufferCoeff == 0) {
      global.fatal << "This is a programming or a mathematical error. "
      << "I get that the denominator in the Freundenthal formula is zero. "
      << " The highest weight is " << inputHWfundamentalCoords.toString()
      << ". The Weyl group details follow. " << this->toString() << global.fatal;
    }
    currentAccum /= BufferCoeff;
    std::stringstream out;
    out << " Computed the multiplicities of " << k + 1 << " out of "
    << outputDominantWeightsSimpleCoords.size << " dominant weights in the support.";
    theReport.report(out.str());
  }
  return true;
}

template<class Coefficient>
bool WeylGroupData::GetAlLDominantWeightsHWFDIM(
  Vector<Coefficient>& highestWeightSimpleCoords,
  HashedList<Vector<Coefficient> >& outputWeightsSimpleCoords,
  int upperBoundDominantWeights,
  std::string* outputDetails
) {
  std::stringstream out;
  Vector<Coefficient> highestWeightTrue;
  highestWeightTrue = highestWeightSimpleCoords;
  this->RaiseToDominantWeight(highestWeightTrue);
  Vector<Coefficient> highestWeightFundCoords = this->getFundamentalCoordinatesFromSimple(highestWeightTrue);
  if (!highestWeightFundCoords.SumCoords().isSmallInteger()) {
    if (outputDetails != nullptr) {
      out << "<hr> The highest weight you gave in simple coordinates: " << highestWeightSimpleCoords.toString()
      << " which equals " << highestWeightFundCoords.toString()
      << "  in fundamental coordinates is not integral dominant.<br>";
      *outputDetails = out.str();
    }
    return false;
  }
  int theTopHeightSimpleCoords = static_cast<int>(highestWeightSimpleCoords.GetVectorRational().SumCoords().GetDoubleValue()) + 1;
  if (theTopHeightSimpleCoords < 0) {
    theTopHeightSimpleCoords = 0;
  }
  List<HashedList<Vector<Coefficient> > > outputWeightsByHeight;
  int topHeightRootSystem = this->RootsOfBorel.lastObject()->SumCoords().numeratorShort;
  int topHeightRootSystemPlusOne = topHeightRootSystem + 1;
  outputWeightsByHeight.setSize(topHeightRootSystemPlusOne);
  int finalHashSize = 100;
  for (int i = 0; i < topHeightRootSystemPlusOne; i ++) {
    outputWeightsByHeight[i].setHashSize(finalHashSize);
  }
  outputWeightsSimpleCoords.clear();
  outputWeightsSimpleCoords.setHashSize(10000);
  outputWeightsByHeight[0].addOnTop(highestWeightTrue);
  int numTotalWeightsFound = 0;
  int numPosRoots = this->RootsOfBorel.size;
  Vector<Coefficient> currentWeight;
  for (
    int lowestUnexploredHeightDiff = 0;
    lowestUnexploredHeightDiff <= theTopHeightSimpleCoords;
    lowestUnexploredHeightDiff ++
  ) {
    if (upperBoundDominantWeights > 0 && numTotalWeightsFound > upperBoundDominantWeights) {
      break;
    }
    int bufferIndexShift = lowestUnexploredHeightDiff % topHeightRootSystemPlusOne;
    HashedList<Vector<Coefficient> >& currentHashes = outputWeightsByHeight[bufferIndexShift];
    for (int lowest = 0; lowest < currentHashes.size; lowest ++) {
      for (int i = 0; i < numPosRoots; i ++) {
        currentWeight = currentHashes[lowest];
        currentWeight -= this->RootsOfBorel[i];
        if (this->isDominantWeight(currentWeight)) {
          int currentIndexShift = this->RootsOfBorel[i].SumCoords().numeratorShort;
          currentIndexShift = (currentIndexShift + bufferIndexShift) % topHeightRootSystemPlusOne;
          if (outputWeightsByHeight[currentIndexShift].addOnTopNoRepetition(currentWeight)) {
            numTotalWeightsFound ++;
            outputWeightsByHeight[currentIndexShift].adjustHashes();
          }
        }
      }
    }
    outputWeightsSimpleCoords.addOnTop(currentHashes);
    outputWeightsSimpleCoords.adjustHashes();
    currentHashes.clear();
    if (numTotalWeightsFound > upperBoundDominantWeights && upperBoundDominantWeights > 0) {
      out << "<hr>The number of weights has exceeded the RAM memory limits, aborting the weight generation. ";
      return false;
    }
  }
  if (outputDetails != nullptr) {
    out << " Total number of dominant weights: " << outputWeightsSimpleCoords.size;
    *outputDetails = out.str();
  }
  return true;
}

template <class Coefficient>
void WeylGroupData::ReflectBetaWRTAlpha(
  const Vector<Rational>& alpha, const Vector<Coefficient>& Beta, bool RhoAction, Vector<Coefficient>& Output
) const {
  Coefficient bufferCoeff, alphaShift, lengthA;
  Vector<Coefficient> result;
  result = (Beta);
  alphaShift = Beta[0].zero();//<-the zero of coefficient is not known at compile time (think multivariate polynomials)
  lengthA = alphaShift;
  if (RhoAction) {
    result += this->rho;//<-implicit type conversion here if coefficient is not Rational
  }
  for (int i = 0; i < this->cartanSymmetric.numberOfRows; i ++) {
    for (int j = 0; j < this->cartanSymmetric.numberOfColumns; j ++) {
      bufferCoeff = result[j];
      bufferCoeff *= alpha[i];
      bufferCoeff *= this->cartanSymmetric.elements[i][j] * (- 2);
      alphaShift += bufferCoeff;
      bufferCoeff = alpha[i];
      bufferCoeff *= alpha[j];
      bufferCoeff *= this->cartanSymmetric.elements[i][j];
      lengthA += bufferCoeff;
    }
  }
  alphaShift /= lengthA;
  Output.setSize(this->cartanSymmetric.numberOfRows);
  for (int i = 0; i < this->cartanSymmetric.numberOfColumns; i ++) {
    bufferCoeff = alphaShift;
    bufferCoeff *= alpha[i];
    bufferCoeff += result[i];
    Output[i] = bufferCoeff;
  }
  if (RhoAction) {
    Output -= this->rho;
  }
}

template <typename somegroup, typename Coefficient>
bool GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::checkInitialization() const {
  if (this->ownerGroup == 0) {
    global.fatal << "This is a programming error: working with a representation "
    << "with non-initialized owner Weyl group. " << global.fatal;
    return false;
  }
  return true;
}

template <typename somegroup, typename Coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::reset() {
  this->parent = 0;
  this->flagCharacterIsComputed = false;
  this->ownerGroup = 0;
}

template <typename somegroup, typename Coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::initialize(somegroup& inputOwner) {
  this->reset();
  this->ownerGroup = &inputOwner;
  this->ownerGroup->CheckInitializationConjugacyClasses();
  this->generatorS.setSize(this->ownerGroup->generators.size);
  this->theElementImageS.setSize(this->ownerGroup->theElements.size);
  this->theElementIsComputed.initializeFillInObject(this->ownerGroup->theElements.size, false);
  this->classFunctionMatrices.setSize(this->ownerGroup->ConjugacyClassCount());
  this->classFunctionMatricesComputed.initializeFillInObject(this->ownerGroup->ConjugacyClassCount(), false);
  this->checkInitialization();
}

//Note:  The group representation types compute the hash value from the character,
// in which the order of the conjugacy classes determines the value
// If you compare hash values from two groups which calculated their conjugacy classes in a different way,
// you must ensure they are in the same order, or hashing won't work as expected.
template <typename somegroup, typename Coefficient>
unsigned int GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::hashFunction() const {
  unsigned int result = 0;
  result += this->theCharacteR.hashFunction();
  return result;
}

template <typename somegroup, typename Coefficient>
unsigned int GroupRepresentation<somegroup, Coefficient>::hashFunction() const {
  unsigned int result = 0;
  result += this->theCharacteR.hashFunction();
  return result;
}

template <typename somegroup, typename Coefficient>
Matrix<Coefficient>& GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::GetMatrixElement(int groupElementIndex) {
  Matrix<Coefficient>& theMat = this->theElementImageS[groupElementIndex];
  if (this->theElementIsComputed[groupElementIndex]) {
    return theMat;
  }
  const ElementWeylGroup& theElt = this->ownerGroup->theElements[groupElementIndex];
  this->theElementIsComputed[groupElementIndex] = true;
  this->GetMatrixElement(theElt, theMat);
  return theMat;
}

template <typename somegroup, typename Coefficient>
template <typename elementSomeGroup>
Matrix<Coefficient> GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::GetMatrixElement(const elementSomeGroup& input) {
  Matrix<Coefficient> result;
  this->GetMatrixElement(input, result);
  return result;
}

template <typename somegroup, typename Coefficient>
int GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::getDimension() const {
  if (this->ownerGroup->generators.size == 0) {
    return 1;
  }
  return this->generatorS[0].numberOfRows;
}

template <typename somegroup, typename Coefficient>
template <typename elementSomeGroup>
void GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::GetMatrixElement(
  const elementSomeGroup& input, Matrix<Coefficient>& output
) {
  this->checkInitialization();
  if (this->ownerGroup->generators.size == 0) {//here be trivial weyl group
    output.MakeIdMatrix(1);
    return;
  }
  this->ownerGroup->CheckInitializationConjugacyClasses();
  output.MakeIdMatrix(this->getDimension());
  for (int i = 0; i < input.generatorsLastAppliedFirst.size; i ++) {
    output.MultiplyOnTheRight(this->generatorS[input.generatorsLastAppliedFirst[i].index]);
  }
}

template <typename somegroup, typename Coefficient>
std::string GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::GetName() const {
  std::string name;
  for (int i = 0; i < this->names.size; i ++) {
    name.append(this->names[i]);
    if (i != this->names.size - 1) {
      name.append(" aka ");
    }
  }
  return name;
}

template <typename somegroup, typename Coefficient>
bool GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::operator>(
  const GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>& right
) const {
  return this->theCharacteR > right.theCharacteR;
}

template <typename somegroup, typename Coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::GetClassFunctionMatrix(
  ClassFunction<somegroup, Coefficient>& inputChar, Matrix<Coefficient>& outputMat
) {
  this->checkInitialization();
  this->ownerGroup->CheckInitializationFDrepComputation();
  outputMat.MakeZeroMatrix(this->getDimension());
  int numClasses = this->ownerGroup->ConjugacyClassCount();
  ProgressReport theReport;
  for (int cci = 0; cci < numClasses; cci ++) {
    if (inputChar[cci] == 0) {
      continue;
    }
    if (!this->classFunctionMatricesComputed[cci]) {
      this->classFunctionMatricesComputed[cci] = true;
      // classFunctionMatrices does not have to be initialized.
      bool useParent = false;
      if (this->parent != 0) {
        useParent = (this->parent->classFunctionMatrices.size == this->ownerGroup->ConjugacyClassCount()) &&
        (this->parent->classFunctionMatricesComputed[cci]);
      }
      if (useParent) {
        Matrix<Coefficient>::matrixInBasis(
          this->parent->classFunctionMatrices[cci],
          this->classFunctionMatrices[cci],
          this->basis,
          this->gramMatrixInverted
        );
      } else {
        auto& currentCC = this->ownerGroup->conjugacyClasses[cci];
        this->classFunctionMatrices[cci].MakeZeroMatrix(this->getDimension());
        for (int i = 0; i < currentCC.theElements.size; i ++) {
          if (!this->theElementIsComputed[currentCC.indicesEltsInOwner[i]]) {
            this->ComputeAllElementImages();
          }
          this->classFunctionMatrices[cci] += this->theElementImageS[currentCC.indicesEltsInOwner[i]];
          if (theReport.tickAndWantReport()) {
            std::stringstream reportstream;
            reportstream << " Computing conjugacy class " << currentCC.indicesEltsInOwner[i] + 1
            << " (total num classes is " << numClasses << ").";
            theReport.report(reportstream.str());
          }
        }
      }
      if (theReport.tickAndWantReport()) {
        std::stringstream reportstream;
        reportstream << "<br>Class function matrix of conjugacy class " << cci + 1
        << " (total num classes is " << numClasses << ") computed to be: "
        << this->classFunctionMatrices[cci].toString();
        theReport.report(reportstream.str());
      }
    }
    for (int j = 0; j < outputMat.numberOfRows; j ++) {
      for (int k = 0; k < outputMat.numberOfColumns; k ++) {
        outputMat(j, k) += this->classFunctionMatrices[cci](j, k) * MathRoutines::ComplexConjugate(inputChar[cci]);
      }
    }
  }
}

template <typename somegroup, typename Coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::ClassFunctionMatrix(
  ClassFunction<somegroup, Coefficient>& inputCF, Matrix<Coefficient>& outputMat
) {
  int theDim = this->generatorS[0].numberOfRows;
  outputMat.MakeZeroMatrix(theDim);
  if (classFunctionMatrices.size == 0) {
    classFunctionMatrices.setSize(this->ownerGroup->ConjugacyClassCount());
  }
  for (int cci = 0; cci < this->ownerGroup->ConjugacyClassCount(); cci ++) {
    if (inputCF[cci] == 0) {
      continue;
    }
    if (classFunctionMatrices[cci].numberOfColumns == 0) {
      classFunctionMatrices[cci].MakeZeroMatrix(this->generatorS[0].numberOfColumns);
      for (int icci = 0; icci < this->ownerGroup->conjugacyClasses[cci].size; icci ++) {
        this->classFunctionMatrices[cci] += this->GetMatrixElement(this->ownerGroup->conjugacyClasses[cci].theElements[icci]);
      }
    }
    for (int i = 0; i < outputMat.numberOfRows; i ++) {
      for (int j = 0; j < outputMat.numberOfColumns; j ++) {
        outputMat.elements[i][j] += classFunctionMatrices[cci].elements[i][j] * inputCF[cci];
      }
    }
  }
}

template <typename somegroup, typename Coefficient>
std::string GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::toString(FormatExpressions* theFormat) const {
  if (this->ownerGroup == 0) {
    return "non-initialized representation";
  }
  std::stringstream out;
  if (this->flagCharacterIsComputed) {
    out << "Character: " << this->theCharacteR.toString(theFormat) << " of norm " << this->theCharacteR.Norm();
  } else {
    out << "Character needs to be computed.";
  }
  int theRank = this->ownerGroup->generators.size;
  LargeIntegerUnsigned theLCM, theDen;
  this->GetLargestDenominatorSimpleGens(theLCM, theDen);
  out << "\n<br>\n LCM denominators simple generators: " << theLCM.toString() << ", largest denominator: " << theDen.toString();
  out << "\n<br>\nThe simple generators (" << theRank << " total):<br> ";
  std::stringstream forYourCopyConvenience;
  for (int i = 0; i < theRank; i ++) {
    if (i < this->generatorS.size) {
      std::stringstream tempStream;
      tempStream << "s_" << i + 1 << "=MatrixRationals{}" << this->generatorS[i].toString(theFormat) << "; \\\\\n";
      forYourCopyConvenience << tempStream.str();
      out << HtmlRoutines::GetMathSpanPure("\\begin{array}{l}" + tempStream.str() + "\\end{array}", 3000);
    } else {
      out << "Simple generator " << i + 1 << "} not computed ";
    }
  }
  out << "<br>For your copy convenience: <br>" << forYourCopyConvenience.str();
  return out.str();
}

template <class Coefficient>
void SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::ActByNonSimpleElement(int index, Vector<Coefficient>& inputOutput) const {
  Vector<Coefficient> tempRoot;
  this->ActByNonSimpleElement(index, inputOutput, tempRoot);
  inputOutput = tempRoot;
}

template <class Coefficient>
void SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::ActByNonSimpleElement(
  int index, Vector<Coefficient>& input, Vector<Coefficient>& output
) const {
  this->actByElement(this->allElements[index], input, output);
}

template <class Coefficient>
void SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::actByElement(
  const ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms& theElement,
  const Vectors<Coefficient>& input,
  Vectors<Coefficient>& output
) const {
  output.setSize(input.size);
  for (int i = 0; i < input.size; i ++) {
    this->actByElement(theElement, input[i], output[i]);
  }
}

template <class Coefficient>
void SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::actByElement(
  const ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms& theElement,
  const Vector<Coefficient>& input,
  Vector<Coefficient>& output
) const {
  if (&input == &output) {
    global.fatal << "Input not allowed to coincide with output. " << global.fatal;
  }
  Vector<Coefficient> tempRoot, tempRoot2;
  output = input;
  for (int i = theElement.generatorsLastAppliedFirst.size - 1; i >= 0; i --) {
    int tempI = theElement.generatorsLastAppliedFirst[i].index;
    if (tempI < this->simpleRootsInner.size) {
      this->AmbientWeyl->ReflectBetaWRTAlpha(this->simpleRootsInner[tempI], output, false, output);
    } else {
      tempI -= this->simpleRootsInner.size;
      tempRoot.makeZero(input.size);
      for (int j = 0; j < output.size; j ++) {
        tempRoot2 = this->ExternalAutomorphisms[tempI][j];
        tempRoot2 *= output[j];
        tempRoot += tempRoot2;
      }
    }
  }
}

template <class Coefficient>
bool SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::isDominantWeight(const Vector<Coefficient>& theWeight) {
  for (int i = 0; i < this->simpleRootsInner.size; i ++) {
    if (!this->IsDominantWRTgenerator(theWeight, i)) {
      return false;
    }
  }
  return true;
}

template<class Coefficient>
Coefficient SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::WeylDimFormulaInnerSimpleCoords(
  const Vector<Coefficient>& theWeightInnerSimpleCoords, const Coefficient& theRingUnit
) {
  MacroRegisterFunctionWithName("SubgroupWeylGroupOLD::weylDimFormulaSimpleCoords");
  this->checkInitialization();
  Coefficient Result, buffer;
  Vector<Coefficient> rhoOverNewRing, rootOfBorelNewRing, sumWithRho;//<-to facilitate type conversion!
  Vector<Rational> rho;
  this->RootsOfBorel.sum(rho, this->AmbientWeyl->getDimension());
  rho /= 2;
  rhoOverNewRing = rho;//<-type conversion here!
  Result = theRingUnit;
  for (int i = 0; i < this->RootsOfBorel.size; i ++) {
    rootOfBorelNewRing = this->RootsOfBorel[i]; //<-type conversion here!
    sumWithRho = rhoOverNewRing + theWeightInnerSimpleCoords;
    buffer = this->AmbientWeyl->RootScalarCartanRoot(sumWithRho, rootOfBorelNewRing);
    buffer /= this->AmbientWeyl->RootScalarCartanRoot(rhoOverNewRing, rootOfBorelNewRing);
    Result *= buffer;
  }
  return Result;
}

template <class Coefficient>
bool SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::GetAlLDominantWeightsHWFDIM(
  Vector<Coefficient>& highestWeightSimpleCoords,
  HashedList<Vector<Coefficient> >& outputWeightsSimpleCoords,
  int upperBoundDominantWeights,
  std::string& outputDetails
) {
  MacroRegisterFunctionWithName("SubgroupWeylGroupOLD::GetAlLDominantWeightsHWFDIM");
  std::stringstream out;
  this->checkInitialization();
  this->ComputeRootSubsystem();
  Vector<Coefficient> highestWeightTrue = highestWeightSimpleCoords;
  Vectors<Rational> basisEi;
  int theDim = this->AmbientWeyl->getDimension();
  basisEi.makeEiBasis(theDim);
  this->RaiseToDominantWeightInner(highestWeightTrue);
  Vector<Coefficient> highestWeightFundCoords = this->AmbientWeyl->getFundamentalCoordinatesFromSimple(highestWeightTrue);
  int theTopHeightSimpleCoords = static_cast<int>(highestWeightSimpleCoords.GetVectorRational().SumCoords().GetDoubleValue()) + 1;
  if (theTopHeightSimpleCoords < 0) {
    theTopHeightSimpleCoords = 0;
  }
  List<HashedList<Vector<Coefficient> > > outputWeightsByHeight;
  int topHeightRootSystem = this->AmbientWeyl->RootsOfBorel.lastObject()->SumCoords().numeratorShort;
  int topHeightRootSystemPlusOne = topHeightRootSystem + 1;
  outputWeightsByHeight.setSize(topHeightRootSystemPlusOne);
  int finalHashSize = 100;
  for (int i = 0; i < topHeightRootSystemPlusOne; i ++) {
    outputWeightsByHeight[i].setHashSize(finalHashSize);
  }
  outputWeightsSimpleCoords.clear();
  outputWeightsByHeight[0].addOnTop(highestWeightTrue);
  int numTotalWeightsFound = 0;
  Vector<Coefficient> currentWeight;
  for (
    int lowestUnexploredHeightDiff = 0;
    lowestUnexploredHeightDiff <= theTopHeightSimpleCoords;
    lowestUnexploredHeightDiff ++
  ) {
    if (upperBoundDominantWeights > 0 && numTotalWeightsFound > upperBoundDominantWeights) {
      break;
    }
    int bufferIndexShift = lowestUnexploredHeightDiff % topHeightRootSystemPlusOne;
    HashedList<Vector<Coefficient> >& currentHashes = outputWeightsByHeight[bufferIndexShift];
    for (int lowest = 0; lowest < currentHashes.size; lowest ++) {
      for (int i = 0; i < this->RootsOfBorel.size; i ++) {
        currentWeight = currentHashes[lowest];
        currentWeight -= this->RootsOfBorel[i];
        if (this->isDominantWeight(currentWeight)) {
          int currentIndexShift = this->RootsOfBorel[i].SumCoords().numeratorShort;
          currentIndexShift = (currentIndexShift + bufferIndexShift) % topHeightRootSystemPlusOne;
          if (outputWeightsByHeight[currentIndexShift].addOnTopNoRepetition(currentWeight)) {
            numTotalWeightsFound ++;
            outputWeightsByHeight[currentIndexShift].adjustHashes();
          }
        }
      }
    }
    outputWeightsSimpleCoords.addOnTop(currentHashes);
    outputWeightsSimpleCoords.adjustHashes();
    currentHashes.clear();
  }
  out << " Total number of dominant weights: " << outputWeightsSimpleCoords.size;
  if (numTotalWeightsFound >= upperBoundDominantWeights) {
    out << "<hr>This message is generated either because the number of "
    << "weights has exceeded the hard-coded RAM memory limits, or because "
    << " a priori bound for the number of weights is WRONG. "
    << "If the latter is the case, make sure to send an angry email to the author(s).";
  }
  outputDetails = out.str();
  return numTotalWeightsFound <= upperBoundDominantWeights;
}

template <class Coefficient>
void SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::RaiseToDominantWeightInner(
  Vector<Coefficient>& theWeight, int* sign, bool* stabilizerFound
) {
  MacroRegisterFunctionWithName("SubgroupWeylGroupOLD::RaiseToDominantWeightInner");
  if (sign != nullptr) {
    *sign = 1;
  }
  if (stabilizerFound != nullptr) {
    *stabilizerFound = false;
  }
  Rational theScalarProd;
//  int theDim= this->AmbientWeyl->getDimension();
  for (bool found = true; found; ) {
    found = false;
    for (int i = 0; i < this->simpleRootsInner.size; i ++) {
      if (!this->IsDominantWRTgenerator(theWeight, i)) {
        found = true;
        this->AmbientWeyl->ReflectBetaWRTAlpha(this->simpleRootsInner[i], theWeight, false, theWeight);
        if (sign != nullptr) {
          *sign *= - 1;
        }
      }
      if (stabilizerFound != nullptr) {
        if (theScalarProd.isEqualToZero()) {
          *stabilizerFound = true;
        }
      }
    }
  }
}

template <class Coefficient>
bool SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::GenerateOrbitReturnFalseIfTruncated(
  const Vector<Coefficient>& input, Vectors<Coefficient>& outputOrbit, bool restrictToInner, int UpperLimitNumElements
) {
  MacroRegisterFunctionWithName("SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::GenerateOrbitReturnFalseIfTruncated");
  HashedList<Vector<Coefficient> > theOrbit;
  bool result = true;
  theOrbit.clear();
  Vector<Coefficient> tempRoot;
  theOrbit.addOnTop(input);
  Vectors<Coefficient> ExternalAutosOverAmbientField;
  for (int i = 0; i < theOrbit.size; i ++) {
    if (UpperLimitNumElements > 0) {
      if (theOrbit.size >= UpperLimitNumElements) {
        result = false;
        break;
      }
    }
    for (int j = 0; j < this->simpleRootsInner.size; j ++) {
      this->AmbientWeyl->ReflectBetaWRTAlpha(this->simpleRootsInner[j], theOrbit[i], false, tempRoot);
      theOrbit.addOnTopNoRepetition(tempRoot);
    }
    if (restrictToInner) {
      continue;
    }
    for (int j = 1; j < this->ExternalAutomorphisms.size; j ++) {
      ExternalAutosOverAmbientField = this->ExternalAutomorphisms[j];
      theOrbit[i].getCoordinatesInBasis(ExternalAutosOverAmbientField, tempRoot);
      theOrbit.addOnTopNoRepetition(tempRoot);
    }
  }
  outputOrbit = theOrbit;
  return result;
}

template <class Coefficient>
bool SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::FreudenthalEvalIrrepIsWRTLeviPart(
  const Vector<Coefficient>& inputHWfundamentalCoords,
  HashedList<Vector<Coefficient> >& outputDominantWeightsSimpleCoordS,
  List<Coefficient>& outputMultsSimpleCoords,
  std::string& outputDetails,
  int UpperBoundFreudenthal
) {
  MacroRegisterFunctionWithName("SubgroupWeylGroupOLD::FreudenthalEvalIrrepIsWRTLeviPart");
  //double startTimer = global.getElapsedSeconds();
  this->ComputeRootSubsystem();
  Vector<Rational> EiVect;
  List<bool> Explored;
  Coefficient theRingZero;
  theRingZero = inputHWfundamentalCoords[0].zero();
  /////////////////////////
  Vector<Coefficient> hwSimpleCoordsLeviPart, hwSimpleCoordsNilPart;
  hwSimpleCoordsLeviPart = inputHWfundamentalCoords;
  hwSimpleCoordsNilPart = inputHWfundamentalCoords;
  for (int i = 0; i < hwSimpleCoordsLeviPart.size; i ++) {
    EiVect.makeEi(hwSimpleCoordsLeviPart.size, i);
    if (!this->RootsOfBorel.contains(EiVect)) {
      hwSimpleCoordsLeviPart[i] = theRingZero;
    } else {
      hwSimpleCoordsNilPart[i] = theRingZero;
    }
  }
  hwSimpleCoordsLeviPart = this->AmbientWeyl->getSimpleCoordinatesFromFundamental(hwSimpleCoordsLeviPart);
  hwSimpleCoordsNilPart = this->AmbientWeyl->getSimpleCoordinatesFromFundamental(hwSimpleCoordsNilPart);
  ///////////////////////////
  HashedList<Vector<Coefficient> > outputDomWeightsSimpleCoordsLeviPart;

  if (!this->GetAlLDominantWeightsHWFDIM(
    hwSimpleCoordsLeviPart, outputDomWeightsSimpleCoordsLeviPart, UpperBoundFreudenthal, outputDetails
  )) {
    std::stringstream errorLog;
    errorLog << "Error: the number of dominant weights exceeded hard-coded limit of " << UpperBoundFreudenthal
    << ". Please check out whether LiE's implementation of the Freudenthal formula can do your computation.";
    outputDetails = errorLog.str();
    return false;
  }
  Explored.initializeFillInObject(outputDomWeightsSimpleCoordsLeviPart.size, false);
  outputMultsSimpleCoords.setSize(outputDomWeightsSimpleCoordsLeviPart.size);
  Vector<Coefficient> currentWeight, currentDominantRepresentative;
  Vector<Coefficient> Rho;
  Rho = this->GetRho();//<-implicit type conversion here!
  Coefficient hwPlusRhoSquared;
  hwPlusRhoSquared = this->AmbientWeyl->RootScalarCartanRoot(hwSimpleCoordsLeviPart + Rho, hwSimpleCoordsLeviPart + Rho);
  Explored[0] = true;
  outputMultsSimpleCoords[0] = 1;
  Vector<Coefficient> convertor;
  Coefficient bufferCoeff;
  ProgressReport theReport;
  for (int k = 1; k < outputDomWeightsSimpleCoordsLeviPart.size; k ++) {
    Explored[k] = true;
    Coefficient& currentAccum = outputMultsSimpleCoords[k];
    currentAccum = 0;
    for (int j = 0; j < this->RootsOfBorel.size; j ++) {
      for (int i = 1; ; i ++) {
        convertor = this->RootsOfBorel[j];
        convertor *= i;
        currentWeight = outputDomWeightsSimpleCoordsLeviPart[k] + convertor;
        currentDominantRepresentative = currentWeight;
        this->RaiseToDominantWeightInner(currentDominantRepresentative);
        int theIndex = outputDomWeightsSimpleCoordsLeviPart.getIndex(currentDominantRepresentative);
        if (theIndex == - 1) {
          break;
        }
        if (!Explored[theIndex]) {
          std::stringstream errorLog;
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
      }
    }
    currentAccum *= 2;
    bufferCoeff = hwPlusRhoSquared;
    bufferCoeff -= this->AmbientWeyl->RootScalarCartanRoot(
      outputDomWeightsSimpleCoordsLeviPart[k] + Rho, outputDomWeightsSimpleCoordsLeviPart[k] + Rho
    );
    //bufferCoeff now holds the denominator participating in the Freudenthal formula.
    if (bufferCoeff.isEqualToZero()) {
      global.fatal << "Coefficient must not be zero at this point. " << global.fatal;
    }
    currentAccum /= bufferCoeff;
    std::stringstream out;
    out << " Computed the multiplicities of " << k + 1 << " out of "
    << outputDomWeightsSimpleCoordsLeviPart.size << " dominant weights in the support.";
    theReport.report(out.str());
  }
  outputDominantWeightsSimpleCoordS.clear();
  outputDominantWeightsSimpleCoordS.setExpectedSize(outputDomWeightsSimpleCoordsLeviPart.size);
  for (int i = 0; i < outputDomWeightsSimpleCoordsLeviPart.size; i ++) {
    outputDominantWeightsSimpleCoordS.addOnTop(outputDomWeightsSimpleCoordsLeviPart[i] + hwSimpleCoordsNilPart);
  }
  return true;
}

template<class someFiniteGroup, typename Coefficient>
void ClassFunction<someFiniteGroup, Coefficient>::makeZero(someFiniteGroup& inputWeyl) {
  this->G = &inputWeyl;
  this->data.makeZero(this->G->ConjugacyClassCount());
}

template<class someFiniteGroup, typename Coefficient>
Coefficient& ClassFunction<someFiniteGroup, Coefficient>::operator[](int i) const {
  return this->data[i];
}

template<class someFiniteGroup, typename Coefficient>
std::string ClassFunction<someFiniteGroup, Coefficient>::toString(FormatExpressions* theFormat) const {
  (void) theFormat;
  if (this->G == 0) {
    return "(not initialized)";
  }
  std::stringstream out;
  out << "(";
  for (int i = 0; i < this->data.size; i ++) {
    out << this->data[i];
    if (i < this->data.size - 1) {
      out << ", ";
    }
  }
  out << ")";
  if (this->Norm().toString() != "1") {
    out << "[";
    out << this->Norm();
    out << "]";
  }
  return out.str();
}

template<class someFiniteGroup, typename Coefficient>
std::ostream& operator<<(std::ostream& out, const ClassFunction<someFiniteGroup, Coefficient> X) {
  out << X.toString();
  return out;
}

template<class someFiniteGroup, typename Coefficient>
unsigned int ClassFunction<someFiniteGroup, Coefficient>::hashFunction(const ClassFunction<someFiniteGroup, Coefficient>& input) {
  unsigned int acc = 0;
  int N = (input.data.size < someRandomPrimesSize) ? input.data.size : someRandomPrimesSize;
  for (int i = 0; i < N; i ++) {
    acc += input.data[i].hashFunction() * someRandomPrimes[i];
  }
  return acc;
}

template<class someFiniteGroup, typename Coefficient>
bool ClassFunction<someFiniteGroup, Coefficient>::operator==(const ClassFunction<someFiniteGroup, Coefficient>& other) const {
  if (this->G != other.G) {
    return false; // we allow comparison of class functions belonging to different groups
  }
  // (this should be handy for the calculator interface, an user may have more than 1 group in play).
  if (this->data == other.data) {
    return true;
  }
  return false;
}

// collation order
template<class someFiniteGroup, typename Coefficient>
bool ClassFunction<someFiniteGroup, Coefficient>::operator>(const ClassFunction<someFiniteGroup, Coefficient>& right) const {
  if (this->data[0] > right.data[0]) {
    return true;
  }
  if (right.data[0] > this->data[0]) {
    return false;
  }
  if (this->data > right.data) {
    return false;
  }
  if (right.data > this->data) {
    return true;
  }
  return false;
}

// Interal API of Subgroup class can go here
template <typename someGroup, typename elementSomeGroup>
void SubgroupData<someGroup, elementSomeGroup>::ComputeCosets() {
  if (flagCosetSetsComputed) {
    return;
  }
  if (this->CosetRepresentativeEnumerator && this->SameCosetAsByFormula) {
    return this->CosetRepresentativeEnumerator(this);
  }
  this->theSubgroup->ComputeAllElements(true);
  this->theGroup->ComputeAllElements(true);
  GraphOLD orbitg = GraphOLD(this->theGroup->theElements.size, this->theSubgroup->generators.size);
  for (int i = 0; i < this->theGroup->theElements.size; i ++) {
    for (int j = 0; j < this->theSubgroup->generators.size; j ++) {
      orbitg.AddEdge(
        this->theGroup->theElements.getIndex(
          this->theGroup->theElements[i] * this->theSubgroup->generators[j]
        ), i
      );
    }
  }
  List<List<int> > orbits;
  orbits = orbitg.DestructivelyGetConnectedComponents();
  this->cosets.setSize(orbits.size);
  for (int i = 0; i < orbits.size; i ++) {
    cosets[i].supergroupIndices = orbits[i];
    cosets[i].representative = this->theGroup->theElements[orbits[i][0]];
    this->theGroup->getWord(this->theGroup->theElements[orbits[i][0]], cosets[i].representativeWord);
  }
  this->flagCosetSetsComputed = true;
  this->flagCosetRepresentativesComputed = true;
  this->VerifyCosets();
}

template <typename someGroup, typename elementSomeGroup>
bool SubgroupData<someGroup, elementSomeGroup>::VerifyCosets() {
  // this returns true or crashes because if the cosets aren't cosets something is seriously wrong
  ComputeCosets();
  for (int i = 0; i < this->theSubgroup->generators.size; i ++) {
    for (int cs = 0; cs < this->cosets.size; cs ++) {
      auto g = cosets[cs].representative * this->theSubgroup->generators[i];
      if (this->GetCosetId(g) != cs) {
        global.fatal << "Error: element " << g << " not found in coset " << cs
        << " despite being product of subgroup generator "
        << this->theSubgroup->generators[i] << " by coset representative "
        << cosets[cs].representative << global.fatal;
      }
    }
  }
  if (this->flagCosetSetsComputed) {
    for (int cs = 0; cs < this->cosets.size; cs ++) {
      auto g = this->cosets[cs].representative;
      g.invert();
      for (int i = 0; i < this->cosets[cs].supergroupIndices.size; i ++) {
        auto g2 = this->theGroup->theElements[this->cosets[cs].supergroupIndices[i]];
        auto g3 = g * g2;
        if (!this->theSubgroup->HasElement(g3)) {
          global.fatal << "Error: coset " << cs << " has representative " << this->cosets[cs].representative
          << "; a putative coset element has " << g << " * " << g2 << " = " << g3
          << " which is not in the subgroup" << global.fatal;
        }
      }
    }
  }
  return true;
}

// Refugees from vpfCharacters_CalculatorInterface.cpp

template <typename elementSomeGroup>
void FiniteGroup<elementSomeGroup>::AddIrreducibleRepresentation(
  GroupRepresentationCarriesAllMatrices<FiniteGroup<elementSomeGroup>, Rational>& p
) {
  this->irreps_grcam.BSInsertDontDup(p);
  auto p_gr = p.MakeOtherGroupRepresentationClass();
  this->AddIrreducibleRepresentation(p_gr);
}

template <typename elementSomeGroup>
void FiniteGroup<elementSomeGroup>::AddIrreducibleRepresentation(
  GroupRepresentation<FiniteGroup<elementSomeGroup>, Rational>& p
) {
  p.ComputeCharacter();
  this->irreps.BSInsertDontDup(p);
  this->characterTable.BSInsertDontDup(p.theCharacteR);
}

template <typename elementSomeGroup>
void FiniteGroup<elementSomeGroup>::AddCharacter(const ClassFunction<FiniteGroup<elementSomeGroup>, Rational>& X) {
  this->characterTable.BSInsertDontDup(X);
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
  if (place == 0) {
    orphanCharacters.addOnTop(X);
    place = &(orphanCharacters.lastObject());
  }
  for (int i = 0; i <characterTable.size; i ++)
    if (characterTable[i] == place)
      return;
  characterTable.addOnTop(place);*/
}

template <typename elementSomeGroup>
void FiniteGroup<elementSomeGroup>::ComputeIrreducibleRepresentationsTodorsVersion() {
  MacroRegisterFunctionWithName("FiniteGroup::ComputeIrreducibleRepresentationsTodorsVersion");
  if (this->irreps_grcam.size == 0) {
    if (this->irreps.size == 0) {
      global.fatal << "Need an initial irrep.  Check up the call chain and find out where it should be provided" << global.fatal;
    }
    for (int i = 0; i < this->irreps.size; i ++) {
      this->irreps_grcam.addOnTop(irreps[i].MakeGRCAM());
    }
  }
  List<GroupRepresentationCarriesAllMatrices<FiniteGroup<elementSomeGroup>, Rational> > appendOnlyIrrepsList;
  appendOnlyIrrepsList = this->irreps_grcam;
  {
    bool nontrivial = false;
    for (int i = 0; i < appendOnlyIrrepsList[0].theCharacteR.data.size; i ++) {
      if (appendOnlyIrrepsList[0].theCharacteR.data[i] != 1) {
        nontrivial = true;
        break;
      }
    }
    if (!nontrivial) {
      appendOnlyIrrepsList.removeIndexShiftDown(0);
    }
  }
  int initialcount = appendOnlyIrrepsList.size;

  if (this->theElements.size == 0) {
    this->computeConjugacyClassesFromAllElements();
  }
  GroupRepresentationCarriesAllMatrices<FiniteGroup<ElementWeylGroup>, Rational> newRep;
  int NumClasses = this->ConjugacyClassCount();
  VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational> decompositionNewRep;
  ProgressReport theReport1;
  //  int indexFirstPredefinedRep = 1; //<-this should be the index of the sign rep.
  //  int indexLastPredefinedrep = 2; //<-this should be the index of the standard rep.
  for (int i = 0; i < appendOnlyIrrepsList.size && this->irreps.size != NumClasses; i ++) {
    for (int j = 0; j < initialcount; j ++) {
      if (theReport1.tickAndWantReport()) {
        std::stringstream reportStream;
        reportStream << this->irreps.size << " irreducible representations found so far. ";
        reportStream << "<br>Decomposing " << appendOnlyIrrepsList[j].theCharacteR
        << " * " << appendOnlyIrrepsList[i].theCharacteR << "\n";
        theReport1.report(reportStream.str());
      }
      newRep = appendOnlyIrrepsList[j];//we are initializing by the sign or natural rep.
      newRep *= appendOnlyIrrepsList[i];
      bool tempB = newRep.DecomposeTodorsVersion(decompositionNewRep, &appendOnlyIrrepsList);
      if (!tempB) {
        global.fatal << "This is a mathematical error: failed to decompose " << newRep.theCharacteR.toString() << ". " << global.fatal;
      }
    }
  }
  if (theReport1.tickAndWantReport()) {
    std::stringstream reportStream;
    reportStream << "Irrep table:";
    for (int i = 0; i < this->irreps.size; i ++) {
      reportStream << "\n<br>\n" << this->irreps[i].theCharacteR.toString();
    }
    FormatExpressions tempFormat;
    tempFormat.flagUseLatex = true;
    for (int i = 0; i < this->irreps.size; i ++) {
      reportStream << "<hr>irrep " << i + 1 << "<br>" << this->irreps_grcam[i].toString(&tempFormat);
    }
    theReport1.report(reportStream.str());
  }
  this->flagCharTableIsComputed = true;
  this->flagIrrepsAreComputed = true;
  if (this->characterTable.size < this->conjugacyClasses.size) {
    global.fatal << "Character table size does not equal the number of conjugacy classes. " << global.fatal;
  }
  if (this->irreps.size < this->conjugacyClasses.size) {
    global.fatal << "Bad number of irreducible representations. " << global.fatal;
  }
}

template <typename somegroup, typename Coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::SpreadVector(
  const Vector<Coefficient>& input, Vectors<Coefficient>& outputBasisGeneratedSpace
) {
  this->checkInitialization();
  outputBasisGeneratedSpace.setSize(1);
  outputBasisGeneratedSpace[0] = input;
  Vector<Coefficient> tempV;
  int theRank = this->ownerGroup->getDimension();
  for (int i = 0; i < outputBasisGeneratedSpace.size; i ++) {
    for (int j = 0; j < theRank; j ++) {
      tempV = outputBasisGeneratedSpace[i];
      this->theElementImageS[j + 1].actOnVectorColumn(tempV);
      if (!outputBasisGeneratedSpace.linearSpanContainsVector(tempV)) {
        outputBasisGeneratedSpace.addOnTop(tempV);
      }
    }
  }
}

template <typename somegroup, typename Coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::GetLargestDenominatorSimpleGens(
  LargeIntegerUnsigned& outputLCM, LargeIntegerUnsigned& outputDen
) const {
  MacroRegisterFunctionWithName("GroupRepresentationCarriesAllMatrices::GetLargestDenominatorSimpleGens");
  outputLCM = 1;
  outputDen = 1;
  for (int gi = 0; gi < this->generatorS.size; gi ++) {
    for (int mi = 0; mi < this->generatorS[gi].numberOfRows; mi ++) {
      for (int mj = 0; mj < this->generatorS[gi].numberOfColumns; mj ++) {
        if (this->generatorS[gi](mi, mj).getDenominator() > outputDen) {
          outputDen = this->generatorS[gi](mi, mj).getDenominator();
        }
        outputLCM = LargeIntegerUnsigned::lcm(outputLCM, this->generatorS[gi](mi, mj).getDenominator());
      }
    }
  }
}


template <typename somegroup, typename Coefficient>
bool GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::DecomposeTodorsVersionRecursive(
  VirtualRepresentation<somegroup, Coefficient>& outputIrrepMults,
  List<GroupRepresentation<somegroup, Coefficient> >& appendOnlyIrrepsList,
  List<GroupRepresentationCarriesAllMatrices<somegroup, Coefficient> >* appendOnlyGRCAMSList
) {
  MacroRegisterFunctionWithName("WeylGroupRepresentation::DecomposeTodorsVersionRecursive");
  this->checkInitialization();

  this->ownerGroup->CheckInitializationFDrepComputation();
  this->GetCharacter();
  Coefficient SumOfNumComponentsSquared = this->GetNumberOfComponents();
  if (SumOfNumComponentsSquared == 0) {
    global.fatal << "This is a programming error: a module has character " << this->theCharacteR.toString()
    << " of zero length, which is impossible. " << "Here is a printout of the module. "
    << this->toString() << global.fatal;
  }
  if (SumOfNumComponentsSquared == 1) {
    int i = this->ownerGroup->characterTable.BSGetIndex(this->theCharacteR);
    if (i == - 1) {
      this->ownerGroup->AddIrreducibleRepresentation(*this);
      appendOnlyIrrepsList.addOnTop(this->MakeOtherGroupRepresentationClass());
      if (appendOnlyGRCAMSList) {
        appendOnlyGRCAMSList->addOnTop(*this);
      }
      i = this->ownerGroup->characterTable.BSGetIndex(this->theCharacteR);
    }
    outputIrrepMults.addMonomial(this->ownerGroup->characterTable[i], 1);
    return true;
  }
  Matrix<Coefficient> splittingOperatorMatrix;
  Vectors<Coefficient> splittingMatrixKernel, remainingVectorSpace, tempSpace;
  ClassFunction<somegroup, Coefficient> remainingCharacter = this->theCharacteR;
  remainingVectorSpace.makeEiBasis(this->getDimension());
  ProgressReport Report1, Report2, Report3, Report4;
  {
    std::stringstream reportStream;
    reportStream << "<br>\nDecomposing module with character " << this->theCharacteR.toString();
    LargeIntegerUnsigned largestDen, lcmDen;
    this->GetLargestDenominatorSimpleGens(lcmDen, largestDen);
    reportStream << "\n<br>\n Largest denominator is " << largestDen.toString()
    << ", denominator lcm is: " << lcmDen.toString();
    Report1.report(reportStream.str());
  }
  //chop off already known pieces:
  for (int i = 0; i < appendOnlyIrrepsList.size; i ++) {
    Coefficient NumIrrepsOfType = this->theCharacteR.InnerProduct(appendOnlyIrrepsList[i].GetCharacter());
    if (NumIrrepsOfType != 0) {
      this->ownerGroup->CheckInitializationFDrepComputation();
      {
        std::stringstream reportStream;
        reportStream << "<hr>\ncontains irrep " << appendOnlyIrrepsList[i].theCharacteR.toString() << " with multiplicity "
        << NumIrrepsOfType << "\n";
        reportStream << "<hr>\nGetting class f-n matrix from character: " << appendOnlyIrrepsList[i].theCharacteR;
        Report2.report(reportStream.str());
      }
      this->GetClassFunctionMatrix(appendOnlyIrrepsList[i].theCharacteR, splittingOperatorMatrix);
      {
        std::stringstream reportStream;
        reportStream << "<br>class f-n matrix: " << splittingOperatorMatrix.toString() << "\n <br>\n"
        << " computing its zero eigenspace... ";
        Report3.report(reportStream.str());
      }
      splittingOperatorMatrix.GetZeroEigenSpaceModifyMe(splittingMatrixKernel);

      remainingVectorSpace.IntersectTwoLinSpaces(splittingMatrixKernel, remainingVectorSpace, tempSpace);

      // I'm not sure how much of a good idea it is to ensure that outputIrrepMults only takes monomials
      // from ownerGroup->characterTable, it might be better to add the character from irreps pointed to
      // by the appendOnlyIrrepsList[i]
      int ci = this->ownerGroup->characterTable.BSGetIndex(appendOnlyIrrepsList[i].theCharacteR);
      outputIrrepMults.addMonomial(this->ownerGroup->characterTable[ci], NumIrrepsOfType);
      remainingCharacter -= appendOnlyIrrepsList[i].theCharacteR*NumIrrepsOfType;
      {
        std::stringstream reportStream;
        reportStream << "<br>Intersecting kernel of class f-n matrix" << splittingMatrixKernel.toString() << " with "
        << remainingVectorSpace.toString() << " to get: " << tempSpace.toString()
        << " with remaining character: " << remainingCharacter.toString();
        Report4.report(reportStream.str());
      }
      remainingVectorSpace = tempSpace;
      if (remainingCharacter[0] != remainingVectorSpace.size) {
        global.fatal << "<br>This is a programming error: remaining char " << remainingCharacter.toString() << " indicates dimension "
        << remainingCharacter[0].toString()
        << " but remaining vector space has dim " << remainingVectorSpace.size << global.fatal;
      }
      if (remainingCharacter.isEqualToZero()) {
        if (remainingVectorSpace.size != 0) {
          global.fatal << "This is a programming error: remaining char is zero but remaining space is " << remainingVectorSpace.toString()
          << ". Starting char: " << this->theCharacteR.toString() << global.fatal;
        }
      }
    }
  }
  if ((remainingVectorSpace.size < this->getDimension()) && (remainingVectorSpace.size > 0)) {
    GroupRepresentationCarriesAllMatrices<somegroup, Coefficient> reducedRep;
    this->Restrict(remainingVectorSpace, remainingCharacter, reducedRep);
    return reducedRep.DecomposeTodorsVersionRecursive(outputIrrepMults, appendOnlyIrrepsList, appendOnlyGRCAMSList);
  }
  if (remainingVectorSpace.size == 0) {
    return true;
  }
  int NumClasses = this->ownerGroup->ConjugacyClassCount();
  ClassFunction<WeylGroupData::WeylGroupBase, Coefficient> virtualChar;
  List<Vectors<Coefficient> > theSubRepsBasis;
  for (int cfi = NumClasses - 1; cfi >= 0; cfi --) {
    virtualChar.makeZero(*this->ownerGroup);
    virtualChar[cfi] = 1;
    this->GetClassFunctionMatrix(virtualChar, splittingOperatorMatrix);
    bool tempB = splittingOperatorMatrix.GetEigenspacesProvidedAllAreIntegralWithEigenValueSmallerThanDim(theSubRepsBasis);
    if (!tempB) {
      global.fatal << "<br>This is a mathematical or programming mistake: "
      << "splittingOperatorMatrix should have small integral values, which it doesn't!" << global.fatal;
    }
    GroupRepresentationCarriesAllMatrices<somegroup, Coefficient> newRep;
    if (theSubRepsBasis.size > 1) {
      //we found splitting, so let us recursively decompose:
      for (int i = 0; i < theSubRepsBasis.size; i ++) {
        remainingCharacter.makeZero(*this->ownerGroup);
        this->Restrict(theSubRepsBasis[i], remainingCharacter, newRep);
        if (!newRep.DecomposeTodorsVersionRecursive(outputIrrepMults, appendOnlyIrrepsList, appendOnlyGRCAMSList)) {
          return false;
        }
      }
      return true;
    }
  }
  Vector<Coefficient> startingVector, tempV, average;
  startingVector.makeEi(this->getDimension(), 0);
  average.makeZero(this->getDimension());
  for (int i = 0; i < this->theElementImageS.size; i ++) {
    if (!this->theElementIsComputed[i]) {
      global.fatal << "<hr>This is a programming error: an internal check failed. " << global.fatal;
    }
    this->theElementImageS[i].actOnVectorColumn(startingVector, tempV);
    average += tempV;
  }
  return false;
}

template <typename somegroup, typename Coefficient>
GroupRepresentation<somegroup, Coefficient> GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::MakeOtherGroupRepresentationClass() const {
  GroupRepresentation<somegroup, Coefficient> out;
  out.flagCharacterIsComputed = this->flagCharacterIsComputed;
  out.generatorS = this->generatorS;
  out.ownerGroup = this->ownerGroup;
  std::stringstream s;
  for (int i = 0; i < this->names.size; i ++) {
    s << this->names[i];
    if (i < this->names.size - 1) {
      s << " ";
    }
  }
  out.identifyingString = s.str();
  out.theCharacteR = this->theCharacteR;
  return out;
}

template <typename elementSomeGroup>
void FiniteGroup<elementSomeGroup>::ComputeIrreducibleRepresentationsThomasVersion(
  GroupRepresentationCarriesAllMatrices<FiniteGroup<elementSomeGroup>, Rational>* startingIrrep
) {
  MacroRegisterFunctionWithName("WeylGroup::ComputeIrreducibleRepresentationsThomasVersion");
  if (!startingIrrep) {
    if (this->irreps_grcam.size != 0) {
      startingIrrep = &(this->irreps_grcam[0]);
    } else {
      if (this->irreps.size != 0) {
        startingIrrep = &(this->irreps[0].MakeGRCAM());
      }
    }
    if (!startingIrrep || (startingIrrep->generatorS.size > 0 && startingIrrep->generatorS[0].numberOfRows == 1)) {
      global.fatal << "Can't find a good starting irrep.  If you think you provided one, change the present assertion. "
      << global.fatal;
    }
  }
  GroupRepresentationCarriesAllMatrices<FiniteGroup<elementSomeGroup>, Rational>& sr = *startingIrrep;
  if (this->theGroup.squaresCCReps.size == 0) {
    this->theGroup.ComputeSquaresCCReps(global);
  }
  this->theGroup.characterTable.setSize(0);
  this->irreps_grcam.setSize(0);
  List<GroupRepresentationCarriesAllMatrices<FiniteGroup<elementSomeGroup>, Rational> > newspaces;
  newspaces.addOnTop(sr);
  List<GroupRepresentationCarriesAllMatrices<FiniteGroup<elementSomeGroup>, Rational> > incompletely_digested;
  while ((newspaces.size > 0) || (incompletely_digested.size > 0)) {
    GroupRepresentationCarriesAllMatrices<FiniteGroup<elementSomeGroup>, Rational> nspace;
    if (newspaces.size > 0) {
      nspace = newspaces.popLastObject();
    } else {
      nspace = incompletely_digested[0];
      incompletely_digested.removeIndexShiftDown(0);
    }
    GroupRepresentationCarriesAllMatrices<FiniteGroup<elementSomeGroup>, Rational> tspace = sr * nspace;
    tspace.flagCharacterIsComputed = false;
    List<GroupRepresentationCarriesAllMatrices<FiniteGroup<elementSomeGroup>, Rational> > spaces = tspace.DecomposeThomasVersion();
    for (int spi = 0; spi < spaces.size; spi ++) {
      if (spaces[spi].GetNumberOfComponents() == 1) {
        if (!this->theGroup.characterTable.contains(spaces[spi].GetCharacter())) {
          this->theGroup.characterTable.addOnTop(spaces[spi].GetCharacter());
          this->irreps_grcam.addOnTop(spaces[spi]);
          newspaces.addOnTop(spaces[spi]);
        }
      } else {
        incompletely_digested.addOnTop(spaces[spi]);
      }
    }
    if (irreps_grcam.size == this->theGroup.ConjugacyClassCount()) {
      break;
    }
    for (int spi = 0; spi < incompletely_digested.size; spi ++) {
      for (int ci = 0; ci < this->theGroup.characterTable.size; ci ++) {
        if (incompletely_digested[spi].GetCharacter().InnerProduct(this->theGroup.characterTable[ci]) != 0) {
          List<GroupRepresentationCarriesAllMatrices<FiniteGroup<elementSomeGroup>, Rational> > shards = incompletely_digested[spi].DecomposeThomasVersion();
          incompletely_digested.removeIndexShiftDown(spi);
          for (int shi = 0; shi < shards.size; shi ++) {
            if (shards[shi].GetNumberOfComponents() == 1) {
              if (!this->theGroup.characterTable.contains(shards[shi].GetCharacter())) {
                this->irreps_grcam.addOnTop(spaces[spi]);
                this->characterTable.addOnTop(this->irreps_grcam.lastObject()->theCharacteR);
                newspaces.addOnTop(spaces[spi]);
              }
            } else {
              incompletely_digested.addOnTop(shards[shi]);
            }
          }
          break;
        }
      }
    }
  }
  this->irreps_grcam.quickSortAscending();
  this->irreps.setSize(0);
  this->characterTable.setSize(0);
  for (int i = 0; i < irreps_grcam.size; i ++) {
    this->irreps.addOnTop(this->irreps_grcam[i].MakeOtherGroupRepresentationClass());
    this->characterTable.addOnTop(&(this->irreps[i].theCharacteR));
  }
  this->theGroup.flagCharTableIsComputed = true;
  this->theGroup.flagIrrepsAreComputed = true;
}

template <typename elementSomeGroup>
void FiniteGroup<elementSomeGroup>::ComputeIrreducibleRepresentations() {
  if (this->ComputeIrreducibleRepresentationsWithFormulas) {
    this->ComputeIrreducibleRepresentationsWithFormulas(*this);
  } else if (this->irreps_grcam.size != 0 || this->irreps.size != 0) {
    this->ComputeIrreducibleRepresentationsTodorsVersion();
  } else {
    global.fatal << "FiniteGroup<elementSomeGroup>::ComputeIrreducibleRepresentations: "
    << "We must have either a formula to generate the irreps, "
    << "or a list of irreps with everything in their tensor products. " << global.fatal;
  }
}

template <typename somegroup, typename Coefficient>
const ClassFunction<somegroup, Coefficient>& GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::GetCharacter() {
  if (this->flagCharacterIsComputed) {
    return this->theCharacteR;
  }
  this->theCharacteR.G = this->ownerGroup;
  this->theCharacteR.data.setSize(this->ownerGroup->ConjugacyClassCount());
  for (int cci = 0; cci < this->ownerGroup->ConjugacyClassCount(); cci ++) {
    this->theCharacteR.data[cci] = this->GetMatrixElement(this->ownerGroup->conjugacyClasses[cci].representative).getTrace();
  }
  this->flagCharacterIsComputed = true;
  return this->theCharacteR;
}

template <typename somegroup, typename Coefficient>
Coefficient GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::GetNumberOfComponents() {
  ClassFunction<somegroup, Coefficient> X;
  X = GetCharacter();
  return X.Norm();
}

template <class Coefficient>
void WeylGroupAutomorphisms::raiseToMaximallyDominant(List<Vector<Coefficient> >& theWeights) {
  MacroRegisterFunctionWithName("WeylGroupAutomorphisms::raiseToMaximallyDominant");
  this->checkInitialization();
  bool found;
  Vectors<Coefficient> theWeightsCopy;
  this->ComputeOuterAutos();
  for (int i = 0; i < theWeights.size; i ++) {
    do {
      found = false;
      for (int j = 0; j < this->theWeyl->RootsOfBorel.size; j ++) {
        if (this->theWeyl->RootScalarCartanRoot(this->theWeyl->RootsOfBorel[j], theWeights[i]) < 0) {
          bool isGood = true;
          for (int k = 0; k < i; k ++) {
            if (this->theWeyl->RootScalarCartanRoot(this->theWeyl->RootsOfBorel[j], theWeights[k]) > 0) {
              isGood = false;
              break;
            }
          }
          if (!isGood) {
            continue;
          }
          for (int k = 0; k < theWeights.size; k ++) {
            this->theWeyl->ReflectBetaWRTAlpha(this->theWeyl->RootsOfBorel[j], theWeights[k], false, theWeights[k]);
          }
          found = true;
        }
      }
      Vector<Rational> zeroWeight;
      zeroWeight.makeZero(this->theWeyl->getDimension());
      HashedList<MatrixTensor<Rational> >& outerAutos = this->theOuterAutos.theElements;
      for (int j = 0; j < outerAutos.size; j ++) {
        theWeightsCopy = theWeights;
        outerAutos[j].actOnVectorsColumn(theWeightsCopy);
        bool isGood = true;
        for (int k = 0; k < i; k ++) {
          if (!(theWeightsCopy[k] - theWeights[k]).isPositiveOrZero()) {
            isGood = false;
            break;
          }
        }
        if (!isGood) {
          continue;
        }
        if (!(theWeightsCopy[i] - theWeights[i]).IsGreaterThanLexicographic(zeroWeight)) {
          continue;
        }
        found = true;
        theWeights = theWeightsCopy;
      }
    } while (found);
  }
}
#endif
