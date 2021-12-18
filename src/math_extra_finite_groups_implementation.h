// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfImplementationHeaderFiniteGroupsIncluded
#define vpfImplementationHeaderFiniteGroupsIncluded

#include "math_extra_finite_groups.h"
#include "math_general_polynomial_computations_advanced_implementation.h"

template <class Coefficient>
std::string FinitelyGeneratedMatrixMonoid<Coefficient>::toString(FormatExpressions* format) const {
  std::stringstream out;
  out << "Number of generators: " << this->generators.size;
  out << "<br>Number of elements: " << this->elements.size;
  out << "<br>The elements follow.";
  int numEltstoDisplay = this->elements.size;
  if (numEltstoDisplay > 100) {
    out << "<b>Displaying only the first " << 100 << " elements.</b>";
    numEltstoDisplay = 100;
  }
  for (int i = 0; i < numEltstoDisplay; i ++) {
    out << "<br>" << this->elements[i].toStringMatrixForm(format);
  }
  return out.str();
}

template <typename elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::computeAllElements(
  bool andWords, int maximumElements
) {
  MacroRegisterFunctionWithName("FiniteGroup::computeAllElements");
  this->checkConsistency();
  //double startTimeDebug= global.getElapsedSeconds();
  this->sizePrivate = this->sizeByFormulaOrNegative1();
  if (this->sizePrivate > 0 && maximumElements > 0 && this->sizePrivate > maximumElements) {
    return false;
  }
  if (!this->computeAllElementsLargeGroup(andWords, maximumElements)) {
    return false;
  }
  return true;
}

template <typename elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::computeAllElementsLargeGroup(bool andWords, int maximumElements) {
  MacroRegisterFunctionWithName("Subgroup::computeAllElementsLargeGroup");
  this->initializeGenerators();
  if (this->generators.size == 0) {
    global.fatal << "Groups with zero generators are not allowed: if you wanted to create a trivial group, "
    << " trivial groups are assumed to have a generator (the identity). " << global.fatal;
  }
  this->elements.clear();
  elementSomeGroup currentElement;
  currentElement.makeIdentity(this->generators[0]);
  this->elements.addOnTop(currentElement);
  this->words.setSize(0);
  if (andWords) {
    this->words.setSize(1);
    this->words.lastObject()->setSize(0);
  }
  ProgressReport report(1000, GlobalVariables::Response::ReportType::general);
  // Warning: not checking whether the generators have repetitions.
  for (int j = 0; j < this->elements.size; j ++) {
    for (int i = 0; i < this->generators.size; i ++) {
      currentElement = this->generators[i] * this->elements[j];
      if (this->elements.addOnTopNoRepetition(currentElement) && andWords) {
        if (this->getWordByFormula == 0) {
          this->words.addOnTop(this->words[j]);
          this->words.lastObject()->addOnTop(i);
        } else {
          this->words.setSize(this->words.size + 1);
          this->getWordByFormula(*this, currentElement, *this->words.lastObject());
        }
      }
      if (report.tickAndWantReport()) {
        std::stringstream reportStream;
        LargeInteger sizeByFla = this->sizeByFormulaOrNegative1();
        reportStream << "So far, generated " << this->elements.size << " elements";
        if (sizeByFla > 0) {
          reportStream << " out of " << sizeByFla.toString();
        }
        reportStream << ".";
        report.report(reportStream.str());
      }
      if (maximumElements > 0) {
        if (this->elements.size > maximumElements) {
          return false;
        }
      }
    }
  }
  report.ticksPerReport = 1;
  if (report.tickAndWantReport()) {
    std::stringstream reportStream;
    reportStream << "Generated group with a total of " << this->elements.size << " elements. ";
    report.report(reportStream.str());
  }
  this->sizePrivate = this->elements.size;
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
  this->groupAction.name = inputGroupAction.name;
  this->groupAction.actOn = inputGroupAction.actOn;
  this->currentLayer->clear();
  this->currentLayer->addOnTop(inputElement);
  this->indexCurrentElement = 0;
  this->previousLayer->clear();
  this->nextLayer->clear();
}

template <class elementGroup, class elementRepresentation>
const elementRepresentation& OrbitIterator<elementGroup, elementRepresentation>::getCurrentElement() {
  return (*this->currentLayer)[this->indexCurrentElement];
}

template <class elementGroup, class elementRepresentation>
std::string OrbitIterator<elementGroup, elementRepresentation>::toStringLayerSize() const {
  std::stringstream out;
  out << " Total number of elements stored in layers: "
  << this->privateLayer1.size + this->privateLayer2.size + this->privateLayer3.size;
  out << " Previous layer: " << this->previousLayer->size
  << "; current layer: " << this->currentLayer->size
  << "; next layer: " << this->nextLayer->size;
  return out.str();
}

template <class elementGroup, class elementRepresentation>
bool OrbitIterator<elementGroup, elementRepresentation>::incrementReturnFalseIfPastLast() {
  MacroRegisterFunctionWithName("OrbitIterator::incrementReturnFalseIfPastLast");
  if (this->theGroupGeneratingElements.size == 0) {
    return false;
  }
  for (int i = 0; i < this->theGroupGeneratingElements.size; i ++) {
    this->groupAction.actOn(
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
  this->groupAction.checkInitialization();
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
    this->groupAction.actOn(
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
  if (&inputGenerators == &this->subgroupContent->generators) {
    List<elementSomeGroup> inputGeneratorsCopy = inputGenerators;
    this->initFromGroupAndGenerators(inputGroup, inputGeneratorsCopy);
    return;
  }
  this->groupContent = &inputGroup;
  this->subgroupContent = &this->subgroupMayBeHere;
  this->subgroupContent->generators.setSize(0);
  this->subgroupContent->generators.addOnTopNoRepetition(inputGenerators);//<- we have eliminated all repeating generators
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
  this->groupContent = &inputGroup;
  this->subgroupContent = &this->subgroupMayBeHere;
  this->subgroupContent->generators = subGenerators;
  this->superGeneratorSubWords.setSize(this->groupContent->generators.size);
  this->superGeneratorSubWordExists.setSize(this->groupContent->generators.size);
  for (int i = 0; i < superGeneratorSubWordExists.size; i ++) {
    superGeneratorSubWordExists[i] = false;
  }
  // done initializing things.  Now for actual code.
  // not the most comprehensive algorithm, is it?
  // I mean, not that I care to prove it incomplete at this point.
  // and so much work for this silly little speed optimization, too
  for (int i = 0; i < this->groupContent->generators.size; i ++) {
    int si = this->subgroupContent->generators.getIndex(this->groupContent->generators[i]);
    if (si != - 1) {
      this->superGeneratorSubWords[i].addOnTop(si);
      this->superGeneratorSubWordExists[i] = true;
    }
  }
  this->subgroupContent->getWordByFormula = SubgroupData<someGroup, elementSomeGroup>::translatableWordsSubgroupElementGetWord;
  this->subgroupContent->parentRelationship = this;
}

template <class someGroup, class elementSomeGroup>
bool SubgroupData<someGroup, elementSomeGroup>::checkInitialization() {
  if (this == 0) {
    global.fatal << "A subgroup has the ``this'' pointer equal to zero. " << global.fatal;
  }
  if (this->groupContent == 0) {
    global.fatal << "Subgroup not initialized when it should be" << global.fatal;
  }
  return true;
}

template <typename elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::hasElement(const elementSomeGroup& g) {
  if (!this->flagAllElementsAreComputed) {
    this->computeAllElements(false);
  }
  return this->elements.contains(g);
}

template <class elementSomeGroup>
void FiniteGroup<elementSomeGroup>::getSignCharacter(Vector<Rational>& outputCharacter) {
  MacroRegisterFunctionWithName("FiniteGroup::getSignCharacter");
  if (!this->flagCCsComputed) {
    this->computeConjugacyClassSizesAndRepresentatives();
  }
  outputCharacter.setSize(this->conjugacyClassCount());
  for (int i = 0; i < this->conjugacyClassCount(); i ++) {
    outputCharacter[i] = this->conjugacyClasses[i].representative.sign();
  }
}

template <class someGroup, class elementSomeGroup>
void SubgroupData<someGroup, elementSomeGroup>::initialize() {
  this->groupContent = 0;
  this->subgroupContent = 0;
  this->subgroupMayBeHere.initialize();
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
SubgroupData<FiniteGroup<elementSomeGroup>, elementSomeGroup> FiniteGroup<elementSomeGroup>::parabolicKindaSubgroupGeneratorSubset(
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
GroupRepresentation<FiniteGroup<elementSomeGroup>, Rational> FiniteGroup<elementSomeGroup>::getEmptyRationalRepresentation() {
  GroupRepresentation<FiniteGroup<elementSomeGroup>, Rational> out;
  out.ownerGroup = this;
  return out;
}

template <class elementSomeGroup>
template <typename Coefficient>
Coefficient FiniteGroup<elementSomeGroup>::getHermitianProduct(
  const Vector<Coefficient>& X1, const Vector<Coefficient>& X2
) {
  Coefficient acc = 0;
  for (int i = 0; i < X1.size; i ++) {
    acc += MathRoutines::complexConjugate(X1[i]) * X2[i] * this->conjugacyClasses[i].size;
    if (this->conjugacyClasses[i].size == 0) {
      global.fatal << "Error: conjugacy class size is zero." << global.fatal;
    }
  }
  return acc / this->getSize();
}

template <class elementSomeGroup>
int FiniteGroup<elementSomeGroup>::conjugacyClassCount() const {
  if (!this->flagCCRepresentativesComputed) {
    global.fatal << "Requesting conjugacy class count but conjugacy class representatives are not computed." << global.fatal;
  }
  return this->conjugacyClasses.size;
}

template <class someGroup, class elementSomeGroup>
void SubgroupData<someGroup, elementSomeGroup>::computeCCRepresentativesPreimages() {
  MacroRegisterFunctionWithName("Subgroup::computeCCRepresentativesPreimages");
  this->ccRepresentativesPreimages.setSize(this->subgroupContent->conjugacyClassCount());
  if (this->subgroupContent->generators.size == 0) {
    global.fatal << "At this computation the group must have initialized generators. " << global.fatal;
  }
  if (this->groupContent->generators.size == 0) {
    global.fatal << "Parent group must have initialized generators. " << global.fatal;
  }
  for (int i = 0; i < this->subgroupContent->conjugacyClasses.size; i ++) {
    elementSomeGroup& scr = this->subgroupContent->conjugacyClasses[i].representative;
    bool notFound = true;
    for (int j = 0; notFound && j < this->groupContent->conjugacyClasses.size; j ++) {
      elementSomeGroup& gcr = this->groupContent->conjugacyClasses[j].representative;
      if (this->groupContent->areConjugate(scr, gcr)) {
        this->ccRepresentativesPreimages[i] = j;
        notFound = false;
        break;
      }
    }
    if (notFound) {
      global.fatal << "Couldn't find preimage "
      << "of the subgroup conjugacy class representative " << scr << global.fatal;
    }
  }
}

template <class elementSomeGroup>
std::string FiniteGroup<elementSomeGroup>::toStringElements(FormatExpressions* format) const {
  MacroRegisterFunctionWithName("FiniteGroup::toStringElements");
  (void) format;
  if (!this->flagAllElementsAreComputed) {
    return "";
  }
  std::stringstream out;
  out << "<br>Elements of the group(" << this->elements.size << " total):\n ";
  if (this->elements.size <= 100) {
    for (int i = 0; i < this->elements.size; i ++) {
      out << i << ". " << this->elements[i].toString() << "\n";
    }
  } else {
    out << "... too many, not displaying. ";
  }
  return out.str();
}

template <class elementSomeGroup>
std::string FiniteGroup<elementSomeGroup>::toStringConjugacyClasses(FormatExpressions* format) {
  MacroRegisterFunctionWithName("Subgroup::toStringConjugacyClasses");
  std::stringstream out;
  out << "<br>Size: " << this->getSize().toString() << "\n";
  FormatExpressions charPolyFormat;
  charPolyFormat.polynomialAlphabet.setSize(1);
  charPolyFormat.polynomialAlphabet[0] = "q";
  if (this->conjugacyClassCount() > 0) {
    out << "<br>" << this->conjugacyClassCount() << " conjugacy classes total.\n";
    for (int i = 0; i < this->conjugacyClasses.size; i ++) {
      out << "<hr>Conjugacy class " << i + 1 << ": ";
      if (this->conjugacyClasses[i].flagRepresentativeComputed) {
        out << " represented by " << this->conjugacyClasses[i].representative.toString(format) << ". ";
      } else {
        out << " representative not computed. ";
      }
      out << "Class size: " << this->conjugacyClasses[i].size.toString() << ".\n<br>\n";
      if (this->flagCharPolysAreComputed) {
        if (i < this->characterPolynomialsConjugacyClassesStandardRepresentation.size) {
          Polynomial<Rational> characteristicPolynomial = this->characterPolynomialsConjugacyClassesStandardRepresentation[i];
          out << "Characteristic poly standard representation: "
          << characteristicPolynomial.toString(&charPolyFormat);
          int numClassesSameCharPoly = 0;
          for (int j = 0; j < this->characterPolynomialsConjugacyClassesStandardRepresentation.size; j ++) {
            if (
              this->characterPolynomialsConjugacyClassesStandardRepresentation[j] ==
              characteristicPolynomial
            ) {
              numClassesSameCharPoly ++;
            }
          }
          if (numClassesSameCharPoly > 1) {
            out << " The characteristic polynomial is the same as that of " << numClassesSameCharPoly
            << " conjugacy classes, numbers: ";
            for (int j = 0; j < this->characterPolynomialsConjugacyClassesStandardRepresentation.size; j ++) {
              if (
                this->characterPolynomialsConjugacyClassesStandardRepresentation[j] ==
                characteristicPolynomial
              ) {
                out << j << ", ";
              }
            }
          }
        }
      }
      if (!this->conjugacyClasses[i].flagElementsComputed) {
        continue;
      }
      out << " The elements of the class are: ";
      int numEltsToDisplay = this->conjugacyClasses[i].elements.size;
      if (this->conjugacyClasses[i].elements.size > 10) {
        out << " too many, displaying the first 10 elements only: ";
        numEltsToDisplay = 10;
      }
      for (int j = 0; j < numEltsToDisplay; j ++) {
        out << this->conjugacyClasses[i].elements[j].toString(format);
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
bool FiniteGroup<elementSomeGroup>::checkOrthogonalityCharacterTable() {
  MacroRegisterFunctionWithName("FiniteGroup::checkOrthogonalityCharacterTable");
  for (int i = 0; i < this->characterTable.size; i ++) {
    for (int j = i; j < this->characterTable.size; j ++) {
      ClassFunction<FiniteGroup, Rational>& leftChar = this->characterTable[i];
      ClassFunction<FiniteGroup, Rational>& rightChar = this->characterTable[j];
      Rational theScalarProd = this->getHermitianProduct(leftChar.data, rightChar.data);
      if (j != i) {
        if (theScalarProd != 0) {
          global.fatal << "Error: the character table is not orthonormal: char number " << i + 1 << " = "
          << leftChar.toString() << " is not orthogonal to char number "
          << j + 1 << " = " << rightChar.toString() << ". <br>The entire char table is: "
          << this->prettyPrintCharacterTable() << global.fatal;
        }
      }
      if (j == i) {
        if (theScalarProd != 1) {
          global.fatal << "Error: the character table is not orthonormal: char number " << i + 1 << " = "
          << leftChar.toString() << " is not of norm 1. "
          << "<br>The entire char table is: " << this->prettyPrintCharacterTable() << global.fatal;
        }
      }
    }
  }
  return true;
}

template <class elementSomeGroup>
void FiniteGroup<elementSomeGroup>::computeGeneratorsConjugacyClasses() {
  MacroRegisterFunctionWithName("FiniteGroup::computeGeneratorsConjugacyClasses");
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
void FiniteGroup<elementSomeGroup>::computeCCSizeOrCCFromRepresentative(
  ConjugacyClass& inputOutputClass, bool storeCC
) {
  MacroRegisterFunctionWithName("FiniteGroup::ComputeCCSizesFromCCRepresentatives");
  OrbitIterator<elementSomeGroup, elementSomeGroup> theOrbitIterator;
  theOrbitIterator.groupAction.actOn = elementSomeGroup::conjugationAction;
  theOrbitIterator.groupAction.name = "conjugation action";
  theOrbitIterator.initialize(this->generators, inputOutputClass.representative, theOrbitIterator.groupAction);
  inputOutputClass.size = 1;
  if (storeCC) {
    inputOutputClass.elements.setSize(0);
    inputOutputClass.elements.addOnTop(inputOutputClass.representative);
  }
  while (theOrbitIterator.incrementReturnFalseIfPastLast()) {
    inputOutputClass.size ++;
    if (storeCC) {
      if (inputOutputClass.elements.contains(theOrbitIterator.getCurrentElement())) {
        global.fatal << " !element " << theOrbitIterator.getCurrentElement().toString()
        << " already contained !" << global.fatal;
      }
      inputOutputClass.elements.addOnTop(theOrbitIterator.getCurrentElement());
    }
  }
}

template <class elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::checkConjugacyClassRepresentationsMatchCCSizes() {
  MacroRegisterFunctionWithName("FiniteGroup::checkConjugacyClassRepresentationsMatchCCSizes");
  LargeInteger computedSize = 0;
  for (int i = 0; i < this->conjugacyClasses.size; i ++) {
    LargeInteger oldCCsize = this->conjugacyClasses[i].size;
    this->computeCCSizeOrCCFromRepresentative(this->conjugacyClasses[i], true);
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
  if (this->sizeByFormulaOrNegative1() > 0) {
    if (computedSize != this->sizeByFormulaOrNegative1()) {
      global.fatal << "Computed size is different from size dicated by formula which is: "
      << this->sizeByFormulaOrNegative1().toString() << global.fatal;
    }
  }
  return true;
}

template <class elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::computeConjugacyClassesRepresentatives() {
  MacroRegisterFunctionWithName("FiniteGroup::computeConjugacyClassesRepresentatives");
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
  this->computeGeneratorsConjugacyClasses();

  ProgressReport report;
  elementSomeGroup currentElement;
  LargeInteger groupSizeByFla = this->sizeByFormulaOrNegative1();
  this->flagCharPolysAreComputed = true;
  for (int phase = 0; phase < 2; phase ++) {
    // In phase 0 we try to add a new conjugacy class only if
    // the class has a new character polynomial. In phase 1 we try every single conjugacy class,
    // in case there are two non-conjugate elements with the same char poly.
    for (int i = 0; i < this->conjugacyClasses.size; i ++) {
      for (int j = 0; j < this->unionGeneratorsCC.size; j ++) {
        if (report.tickAndWantReport()) {
          std::stringstream reportStream;
          reportStream << "Exploring conjugacy class " << i + 1
          << " out of " << this->conjugacyClasses.size
          << " generator " << j + 1 << " out of " << this->unionGeneratorsCC.size;
          report.report(reportStream.str());
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
  if (this->getSizeByFormula != 0) {
    LargeInteger theSize = this->getSizeByFormula(*this);
    //extended digit separators only appear in cxx14
    if (theSize > 100000000) {
      if (this->flagCanComputeCCsWithOrbitIterator) {
        this->computeConjugacyClassSizesAndRepresentativesWithOrbitIterator();
        return;
      }
    }
  }
  if (useComputeCCSizesRepresentativesWords) {
    this->computeConjugacyClassesSizesRepresentativesWords();
  }
}

template <class elementSomeGroup>
void FiniteGroup<elementSomeGroup>::computeConjugacyClassSizesAndRepresentativesWithOrbitIterator() {
  MacroRegisterFunctionWithName("FiniteGroup::computeConjugacyClassSizesAndRepresentativesWithOrbitIterator");
  if (this->flagCCRepresentativesComputed) {
    return;
  }

  this->initializeGenerators();
  this->checkInitialization();
  this->characterPolynomialsConjugacyClassesStandardRepresentation.clear();
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
  this->computeConjugacyClassesRepresentatives();
  recursionCount --;
  this->flagCCRepresentativesComputed = true;
}

template <class elementSomeGroup>
void FiniteGroup<elementSomeGroup>::computeConjugacyClassesFromConjugacyClassIndicesInAllElements(const List<List<int> >& ccIndices) {
  MacroRegisterFunctionWithName("FiniteGroup::computeConjugacyClassesFromConjugacyClassIndicesInAllElements");
  this->conjugacyClasses.setSize(ccIndices.size);
  for (int i = 0; i < ccIndices.size; i ++) {
    this->conjugacyClasses[i].size = ccIndices[i].size;
    this->conjugacyClasses[i].indicesEltsInOwner = ccIndices[i];
    this->conjugacyClasses[i].elements.setSize(ccIndices[i].size);
    for (int j = 0; j < ccIndices[i].size; j ++) {
      this->conjugacyClasses[i].elements[j] = this->elements[ccIndices[i][j]];
    }
    this->conjugacyClasses[i].representative = this->elements[ccIndices[i][0]];
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
  this->computeAllElements(false, - 1);
  List<bool> Accounted;
  Accounted.initializeFillInObject(this->elements.size, false);
  HashedList<int, HashFunctions::hashFunction> theStack;
  theStack.setExpectedSize(this->elements.size);
  List<elementSomeGroup> inversesOfGenerators;
  inversesOfGenerators.setSize(this->generators.size);
  for (int i = 0; i < this->generators.size; i ++) {
    inversesOfGenerators[i] = this->generators[i].inverse();
  }
  elementSomeGroup currentElement;
  List<List<int> > ccIndices;
  ccIndices.reserve(120);
  for (int i = 0; i < this->elements.size; i ++) {
    if (!Accounted[i]) {
      theStack.clear();
      theStack.addOnTop(i);
      for (int j = 0; j < theStack.size; j ++) {
        for (int k = 0; k < this->generators.size; k ++) {
          currentElement = inversesOfGenerators[k] * this->elements[theStack[j]] * this->generators[k];
          int accountedIndex = this->elements.getIndexNoFail(currentElement);
          theStack.addOnTopNoRepetition(accountedIndex);
          Accounted[accountedIndex] = true;
        }
      }
      ccIndices.addOnTop(theStack);
      ccIndices.lastObject()->quickSortAscending();
    }
  }
  ccIndices.quickSortAscending();
  this->computeConjugacyClassesFromConjugacyClassIndicesInAllElements(ccIndices);
}

template<class Coefficient>
LargeInteger WeylGroupData::getOrbitSize(Vector<Coefficient>& weight) {
  MacroRegisterFunctionWithName("WeylGroup::getOrbitSize");
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
  for (int i = 0; i < this->rootsOfBorel.size; i ++) {
    this->reflectBetaWithRespectToAlpha(this->rootsOfBorel[i], weight, false, currentWeight);
    if (currentWeight == weight) {
      theStabilizingRoots.addOnTop(this->rootsOfBorel[i]);
    }
  }
  DynkinDiagramRootSubalgebra theStabilizerSubsystem;
  theStabilizerSubsystem.computeDiagramTypeModifyInput(theStabilizingRoots, *this);
  DynkinType theStabilizerDynkinType;
  theStabilizerSubsystem.getDynkinType(theStabilizerDynkinType);
  Rational resultRat = this->group.getSize();
  resultRat /= theStabilizerDynkinType.getWeylGroupSizeByFormula();
  LargeInteger result;
  if (!resultRat.isInteger(&result)) {
    global.fatal << "Something has gone very wrong: orbit size reported to be " << resultRat.toString()
    << " which is non-integer!" << global.fatal;
  }
  bool doDebug = true;
  if (doDebug) {
    if (result < 100000) {
      HashedList<Vector<Coefficient> > comparisonOrbit;
      this->generateOrbit(weight, false, comparisonOrbit, false, - 1, 0, - 1);
      if (result != comparisonOrbit.size) {
        global.fatal << "Actual orbit of "
        << weight.toString()
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
bool WeylGroupAutomorphisms::generateOuterOrbit(
  Vectors<Coefficient>& weights,
  HashedList<Vector<Coefficient> >& output,
  HashedList<ElementWeylGroupAutomorphisms>* outputSubset,
  int upperLimitNumberOfElements
) {
  MacroRegisterFunctionWithName("WeylGroup::generateOuterOrbit");
  this->checkInitialization();
  this->computeOuterAutoGenerators();
  List<MatrixTensor<Rational> > theOuterGens = this->outerAutomorphisms.generators;
  output.clear();
  for (int i = 0; i < weights.size; i ++) {
    output.addOnTop(weights[i]);
  }
  Vector<Coefficient> currentRoot;
  ElementWeylGroupAutomorphisms currentElt;
  int numElementsToReserve = MathRoutines::minimum(upperLimitNumberOfElements, 1000000);
  output.setExpectedSize(numElementsToReserve);
  ProgressReport report(3000, GlobalVariables::Response::ReportType::general);
  SimpleReflectionOrOuterAutomorphism theGen;
  if (outputSubset != nullptr) {
    currentElt.makeIdentity(*this);
    outputSubset->setExpectedSize(numElementsToReserve);
    outputSubset->clear();
    outputSubset->addOnTop(currentElt);
  }
  int numGens = this->weylGroup->getDimension() + theOuterGens.size;
  for (int i = 0; i < output.size; i ++) {
    for (int j = 0; j < numGens; j ++) {
      if (j < this->weylGroup->getDimension()) {
        currentRoot = output[i];
        this->weylGroup->reflectSimple(j, currentRoot);
      } else {
        theOuterGens[j - this->weylGroup->getDimension()].actOnVectorColumn(output[i], currentRoot);
      }
      if (output.addOnTopNoRepetition(currentRoot)) {
        if (outputSubset != nullptr) {
          currentElt.makeIdentity(*this);
          theGen.makeSimpleReflection(j);
          currentElt.generatorsLastAppliedFirst.addOnTop(theGen);
          currentElt.generatorsLastAppliedFirst.addListOnTop((*outputSubset)[i].generatorsLastAppliedFirst);
          outputSubset->addOnTop(currentElt);
        }
      }
      if (upperLimitNumberOfElements > 0) {
        if (output.size >= upperLimitNumberOfElements) {
          return false;
        }
      }
      if (report.tickAndWantReport()) {
        std::stringstream reportStream;
        reportStream << "Generating outer orbit, " << output.size
        << " elements found so far, Weyl group type: "
        << this->weylGroup->dynkinType.toString() << ". ";
        report.report(reportStream.str());
      }
    }
  }
  return true;
}

template <class Coefficient>
void WeylGroupData::raiseToDominantWeight(
  Vector<Coefficient>& weight,
  int* sign,
  bool* stabilizerFound,
  ElementWeylGroup* raisingElt
) {
  MacroRegisterFunctionWithName("WeylGroup::raiseToDominantWeight");
  if (sign != nullptr) {
    *sign = 1;
  }
  if (stabilizerFound != nullptr) {
    *stabilizerFound = false;
  }
  Coefficient theScalarProd;
  int dimension = this->getDimension();
  SimpleReflection theGen;
  if (raisingElt != nullptr) {
    raisingElt->makeIdentity(*this);
  }
  for (bool found = true; found; ) {
    found = false;
    for (int i = 0; i < dimension; i ++) {
      theScalarProd = this->getScalarProductSimpleRoot(weight, i);
      if (theScalarProd.isNegative()) {
        found = true;
        theScalarProd *= 2;
        theScalarProd /= this->cartanSymmetric.elements[i][i];
        weight[i] -= theScalarProd;
        if (sign != nullptr) {
          *sign *= - 1;
        }
        theGen.makeSimpleReflection(i);
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
    raisingElt->makeCanonical();
  }
}

template <class Coefficient>
void WeylGroupData::reflectRhoSimple(int index, Vector<Coefficient>& vector) const {
  Coefficient alphaShift, tempRat;
  alphaShift = 0;
  for (int i = 0; i < this->cartanSymmetric.numberOfColumns; i ++) {
    tempRat = vector[i];
    tempRat *= this->cartanSymmetric.elements[index][i] * (- 2);
    alphaShift += tempRat;
  }
  alphaShift /= this->cartanSymmetric.elements[index][index];
  alphaShift -= 1;
  vector[index] += alphaShift;
}

template <class Coefficient>
bool WeylGroupData::generateOrbit(
  Vectors<Coefficient>& weights,
  bool rhoAction,
  HashedList<Vector<Coefficient> >& output,
  bool useMinusRho,
  int expectedOrbitSize,
  HashedList<ElementWeylGroup>* outputSubset,
  int upperLimitNumberOfElements
) {
  MacroRegisterFunctionWithName("WeylGroup::generateOrbit");
  output.clear();
  for (int i = 0; i < weights.size; i ++) {
    output.addOnTopNoRepetition(weights[i]);
  }
  Vector<Coefficient> currentRoot;
  ElementWeylGroup currentElt;
  if (expectedOrbitSize <= 0) {
    if (!this->group.getSize().isIntegerFittingInInt(&expectedOrbitSize)) {
      expectedOrbitSize = - 1;
    }
  }
  if (upperLimitNumberOfElements > 0 && expectedOrbitSize > upperLimitNumberOfElements) {
    expectedOrbitSize = upperLimitNumberOfElements;
  }
  if (expectedOrbitSize > 10000000) {
    expectedOrbitSize = 10000000;
  }
  output.setExpectedSize(expectedOrbitSize);
  if (outputSubset != nullptr) {
    if (upperLimitNumberOfElements > 0) {
      expectedOrbitSize = MathRoutines::minimum(upperLimitNumberOfElements, expectedOrbitSize);
    }
    currentElt.makeIdentity(*this);
    outputSubset->setExpectedSize(expectedOrbitSize);
    outputSubset->clear();
    outputSubset->addOnTop(currentElt);
  }
  ProgressReport report(1000, GlobalVariables::Response::ReportType::general);
  SimpleReflection theGen;
  for (int i = 0; i < output.size; i ++) {
    for (int j = 0; j < this->cartanSymmetric.numberOfRows; j ++) {
      currentRoot = output[i];
      if (report.tickAndWantReport()) {
        std::stringstream reportStream;
        reportStream << "So far found " << i + 1 << " elements in the orbit(s) of the starting weight(s) "
        << weights.toString() << ". ";
        report.report(reportStream.str());
      }
      if (!rhoAction) {
        this->reflectSimple(j, currentRoot);
      } else if (!useMinusRho) {
        this->reflectRhoSimple(j, currentRoot);
      } else {
        this->reflectMinusRhoSimple(j, currentRoot);
      }
      if (output.addOnTopNoRepetition(currentRoot)) {
        if (outputSubset != nullptr) {
          currentElt.generatorsLastAppliedFirst.setSize(1);
          theGen.makeSimpleReflection(j);
          currentElt.generatorsLastAppliedFirst[0] = theGen;
          currentElt.generatorsLastAppliedFirst.addListOnTop((*outputSubset)[i].generatorsLastAppliedFirst);
          currentElt.makeCanonical();
          outputSubset->addOnTop(currentElt);
        }
      }
      if (upperLimitNumberOfElements > 0) {
        if (output.size >= upperLimitNumberOfElements) {
          return false;
        }
      }
    }
  }
  return true;
}

template <class Coefficient>
void WeylGroupData::reflectMinusRhoSimple(int index, Vector<Coefficient>& vector) const {
  Coefficient alphaShift, tempRat;
  alphaShift = 0;
  for (int i = 0; i < this->cartanSymmetric.numberOfColumns; i ++) {
    tempRat = vector[i];
    tempRat *= this->cartanSymmetric.elements[index][i];
    alphaShift += tempRat;
  }
  alphaShift *= - 2;
  alphaShift /= this->cartanSymmetric.elements[index][index];
  Coefficient one;
  one = 1;
  alphaShift += one;
  vector[index] += alphaShift;
}

template <class Coefficient>
void WeylGroupData::reflectSimple(int index, Vector<Coefficient>& vector) const {
  if (index < 0 || index >= this->cartanSymmetric.numberOfColumns) {
    global.fatal << "Simple reflection with respect to index "
    << index + 1 << " in a Weyl group of rank "
    << this->getDimension() << global.fatal;
  }
  Coefficient alphaShift, tempRat;
  alphaShift = 0;
  for (int i = 0; i < this->cartanSymmetric.numberOfColumns; i ++) {
    tempRat = vector[i];
    tempRat *= this->cartanSymmetric.elements[index][i];
    alphaShift += tempRat;
  }
  alphaShift *= - 2;
  alphaShift /= this->cartanSymmetric.elements[index][index];
  vector[index] += alphaShift;
}

template <class Coefficient>
bool WeylGroupData::isDominantWeight(const Vector<Coefficient>& weight) {
  int dimension = this->getDimension();
  for (int i = 0; i < dimension; i ++) {
    if (!this->isDominantWithRespectToGenerator(weight, i)) {
      return false;
    }
  }
  return true;
}

template<class Coefficient>
Coefficient WeylGroupData::weylDimensionFormulaSimpleCoordinates(
  Vector<Coefficient>& weightInSimpleCoords, const Coefficient& ringUnit
) {
  Coefficient result, buffer;
  Vector<Coefficient> rhoOverNewRing, rootOfBorelNewRing, sumWithRho;//<-to facilitate type conversion!
  rhoOverNewRing = this->rho;//<-type conversion here!
  result = ringUnit;
  for (int i = 0; i < this->rootsOfBorel.size; i ++) {
    rootOfBorelNewRing = this->rootsOfBorel[i]; //<-type conversion here!
    sumWithRho = rhoOverNewRing + weightInSimpleCoords;
    buffer = this->rootScalarCartanRoot(sumWithRho, rootOfBorelNewRing);
    buffer /= this->rootScalarCartanRoot(rhoOverNewRing, rootOfBorelNewRing);
    result *= buffer;
  }
  return result;
}

template<class Coefficient>
Vector<Coefficient> WeylGroupData::getFundamentalCoordinatesFromSimple(const Vector<Coefficient>& inputInFundamentalCoords) {
  Matrix<Rational>& fundamentalCoordinates = *this->getMatrixSimpleToFundamentalCoords();
  Vector<Coefficient> result = inputInFundamentalCoords;
  fundamentalCoordinates.actOnVectorColumn(result, Coefficient::zero());
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
    result[i] = simpleBasisEpsCoords[i].scalarEuclidean(inputInEpsCoords);
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
Vector<Coefficient> WeylGroupData::getSimpleCoordinatesFromFundamental(
  const Vector<Coefficient>& inputInFundamentalCoords,
  const Coefficient& zero
) {
  Matrix<Rational>& transitionMatrix = *this->getMatrixFundamentalToSimpleCoords();
  Vector<Coefficient> result;
  result = inputInFundamentalCoords;
  transitionMatrix.actOnVectorColumn(result, zero);
  return result;
}

template<class Coefficient>
Vectors<Coefficient> WeylGroupData::getSimpleCoordinatesFromFundamental(
  const Vectors<Coefficient>& inputInFundamentalCoords,
  const Coefficient& zero
) {
  Matrix<Rational>& transitionMatrix = *this->getMatrixFundamentalToSimpleCoords();
  Vectors<Coefficient> result;
  result = inputInFundamentalCoords;
  transitionMatrix.actOnVectorsColumn(result, zero);
  return result;
}

template<class Coefficient>
Coefficient WeylGroupData::weylDimFormulaFundamentalCoords(Vector<Coefficient>& weightFundamentalCoords) {
  Vector<Coefficient> weightInSimpleCoords;
  weightInSimpleCoords = this->getSimpleCoordinatesFromFundamental(weightFundamentalCoords, Coefficient::zero());
  return this->weylDimensionFormulaSimpleCoordinates(weightInSimpleCoords);
}

template <class Coefficient>
bool WeylGroupData::freudenthalFormula(
  Vector<Coefficient>& inputHWfundamentalCoords,
  HashedList<Vector<Coefficient> >& outputDominantWeightsSimpleCoords,
  List<Coefficient>& outputMultsSimpleCoords,
  std::string* outputDetails,
  int UpperBoundFreudenthal
) {
  MacroRegisterFunctionWithName("WeylGroup::freudenthalFormula");
  for (int i = 0; i < inputHWfundamentalCoords.size; i ++) {
    if (inputHWfundamentalCoords[i] < 0) {
      if (outputDetails != nullptr) {
        *outputDetails = "The highest weight is not dominant and I cannot apply the Freudenthal formula.";
      }
      return false;
    }
  }
  this->computeRho(true);
  Vectors<Rational> eiBasis;
  eiBasis.makeEiBasis(this->getDimension());
  List<bool> Explored;
  Vector<Coefficient> hwSimpleCoords = this->getSimpleCoordinatesFromFundamental(inputHWfundamentalCoords);
  if (!this->getAllDominantWeightsHWFDIM(
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
  hwPlusRhoSquared = this->rootScalarCartanRoot(convertor, convertor);
  outputMultsSimpleCoords[0] = 1;
  Explored[0] = true;
  Coefficient bufferCoefficient;
  ProgressReport report;
  for (int k = 1; k < outputDominantWeightsSimpleCoords.size; k ++) {
    Explored[k] = true;
    Coefficient& currentAccum = outputMultsSimpleCoords[k];
    currentAccum = 0;
    for (int j = 0; j < this->rootsOfBorel.size; j ++) {
      for (int i = 1; ; i ++) {
        currentWeight = outputDominantWeightsSimpleCoords[k];
        currentWeight += this->rootsOfBorel[j] * i;
        currentDominantRepresentative = currentWeight;
        this->raiseToDominantWeight(currentDominantRepresentative);
        int index = outputDominantWeightsSimpleCoords.getIndex(currentDominantRepresentative);
        if (index == - 1) {
          break;
        }
        if (!Explored[index]) {
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
        bufferCoefficient = this->rootScalarCartanRoot(currentWeight, this->rootsOfBorel[j]);
        bufferCoefficient *= outputMultsSimpleCoords[index];
        currentAccum += bufferCoefficient;
      }
    }
    currentAccum *= 2;
    convertor = outputDominantWeightsSimpleCoords[k];
    convertor += this->rho;
    bufferCoefficient = hwPlusRhoSquared;
    bufferCoefficient -= this->rootScalarCartanRoot(convertor, convertor);
    if (bufferCoefficient == 0) {
      global.fatal << "This is a programming or a mathematical error. "
      << "I get that the denominator in the Freundenthal formula is zero. "
      << " The highest weight is " << inputHWfundamentalCoords.toString()
      << ". The Weyl group details follow. " << this->toString() << global.fatal;
    }
    currentAccum /= bufferCoefficient;
    std::stringstream out;
    out << " Computed the multiplicities of " << k + 1 << " out of "
    << outputDominantWeightsSimpleCoords.size << " dominant weights in the support.";
    report.report(out.str());
  }
  return true;
}

template<class Coefficient>
bool WeylGroupData::getAllDominantWeightsHWFDIM(
  Vector<Coefficient>& highestWeightSimpleCoords,
  HashedList<Vector<Coefficient> >& outputWeightsSimpleCoords,
  int upperBoundDominantWeights,
  std::string* outputDetails
) {
  std::stringstream out;
  Vector<Coefficient> highestWeightTrue;
  highestWeightTrue = highestWeightSimpleCoords;
  this->raiseToDominantWeight(highestWeightTrue);
  Vector<Coefficient> highestWeightFundCoords = this->getFundamentalCoordinatesFromSimple(highestWeightTrue);
  if (!highestWeightFundCoords.sumCoordinates().isSmallInteger()) {
    if (outputDetails != nullptr) {
      out << "<hr> The highest weight you gave in simple coordinates: " << highestWeightSimpleCoords.toString()
      << " which equals " << highestWeightFundCoords.toString()
      << "  in fundamental coordinates is not integral dominant.<br>";
      *outputDetails = out.str();
    }
    return false;
  }
  int theTopHeightSimpleCoords = static_cast<int>(highestWeightSimpleCoords.getVectorRational().sumCoordinates().getDoubleValue()) + 1;
  if (theTopHeightSimpleCoords < 0) {
    theTopHeightSimpleCoords = 0;
  }
  List<HashedList<Vector<Coefficient> > > outputWeightsByHeight;
  int topHeightRootSystem = this->rootsOfBorel.lastObject()->sumCoordinates().numeratorShort;
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
  int numPosRoots = this->rootsOfBorel.size;
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
        currentWeight -= this->rootsOfBorel[i];
        if (this->isDominantWeight(currentWeight)) {
          int currentIndexShift = this->rootsOfBorel[i].sumCoordinates().numeratorShort;
          currentIndexShift = (currentIndexShift + bufferIndexShift) % topHeightRootSystemPlusOne;
          if (outputWeightsByHeight[currentIndexShift].addOnTopNoRepetition(currentWeight)) {
            numTotalWeightsFound ++;
            outputWeightsByHeight[currentIndexShift].adjustHashes();
          }
        }
      }
    }
    outputWeightsSimpleCoords.addListOnTop(currentHashes);
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
void WeylGroupData::reflectBetaWithRespectToAlpha(
  const Vector<Rational>& alpha, const Vector<Coefficient>& beta, bool rhoAction, Vector<Coefficient>& output
) const {
  Coefficient bufferCoeff, alphaShift, lengthA;
  Vector<Coefficient> result;
  result = beta;
  alphaShift = beta[0].zero();//<-the zero of coefficient is not known at compile time (think multivariate polynomials)
  lengthA = alphaShift;
  if (rhoAction) {
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
  output.setSize(this->cartanSymmetric.numberOfRows);
  for (int i = 0; i < this->cartanSymmetric.numberOfColumns; i ++) {
    bufferCoeff = alphaShift;
    bufferCoeff *= alpha[i];
    bufferCoeff += result[i];
    output[i] = bufferCoeff;
  }
  if (rhoAction) {
    output -= this->rho;
  }
}

template <typename somegroup, typename Coefficient>
bool GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::checkInitialization() const {
  if (this->ownerGroup == 0) {
    global.fatal << "Working with a representation "
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
  this->ownerGroup->checkInitializationConjugacyClasses();
  this->generators.setSize(this->ownerGroup->generators.size);
  this->elementImages.setSize(this->ownerGroup->elements.size);
  this->elementIsComputed.initializeFillInObject(this->ownerGroup->elements.size, false);
  this->classFunctionMatrices.setSize(this->ownerGroup->conjugacyClassCount());
  this->classFunctionMatricesComputed.initializeFillInObject(this->ownerGroup->conjugacyClassCount(), false);
  this->checkInitialization();
}

//Note:  The group representation types compute the hash value from the character,
// in which the order of the conjugacy classes determines the value
// If you compare hash values from two groups which calculated their conjugacy classes in a different way,
// you must ensure they are in the same order, or hashing won't work as expected.
template <typename somegroup, typename Coefficient>
unsigned int GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::hashFunction() const {
  unsigned int result = 0;
  result += this->character.hashFunction();
  return result;
}

template <typename somegroup, typename Coefficient>
unsigned int GroupRepresentation<somegroup, Coefficient>::hashFunction() const {
  unsigned int result = 0;
  result += this->character.hashFunction();
  return result;
}

template <typename somegroup, typename Coefficient>
Matrix<Coefficient>& GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::getMatrixElement(int groupElementIndex) {
  Matrix<Coefficient>& matrix = this->elementImages[groupElementIndex];
  if (this->elementIsComputed[groupElementIndex]) {
    return matrix;
  }
  const ElementWeylGroup& element = this->ownerGroup->elements[groupElementIndex];
  this->elementIsComputed[groupElementIndex] = true;
  this->getMatrixElement(element, matrix);
  return matrix;
}

template <typename somegroup, typename Coefficient>
template <typename elementSomeGroup>
Matrix<Coefficient> GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::getMatrixElement(const elementSomeGroup& input) {
  Matrix<Coefficient> result;
  this->getMatrixElement(input, result);
  return result;
}

template <typename somegroup, typename Coefficient>
int GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::getDimension() const {
  if (this->ownerGroup->generators.size == 0) {
    return 1;
  }
  return this->generators[0].numberOfRows;
}

template <typename somegroup, typename Coefficient>
template <typename elementSomeGroup>
void GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::getMatrixElement(
  const elementSomeGroup& input, Matrix<Coefficient>& output
) {
  this->checkInitialization();
  if (this->ownerGroup->generators.size == 0) {//here be trivial weyl group
    output.makeIdentityMatrix(1);
    return;
  }
  this->ownerGroup->checkInitializationConjugacyClasses();
  output.makeIdentityMatrix(this->getDimension());
  for (int i = 0; i < input.generatorsLastAppliedFirst.size; i ++) {
    output.multiplyOnTheRight(this->generators[input.generatorsLastAppliedFirst[i].index]);
  }
}

template <typename somegroup, typename Coefficient>
std::string GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::getName() const {
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
  return this->character > right.character;
}

template <typename somegroup, typename Coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::getClassFunctionMatrix(
  ClassFunction<somegroup, Coefficient>& inputChar, Matrix<Coefficient>& outputMat
) {
  this->checkInitialization();
  this->ownerGroup->checkInitializationFiniteDimensionalRepresentationComputation();
  outputMat.makeZeroMatrix(this->getDimension());
  int numClasses = this->ownerGroup->conjugacyClassCount();
  ProgressReport report;
  for (int cci = 0; cci < numClasses; cci ++) {
    if (inputChar[cci] == 0) {
      continue;
    }
    if (!this->classFunctionMatricesComputed[cci]) {
      this->classFunctionMatricesComputed[cci] = true;
      // classFunctionMatrices does not have to be initialized.
      bool useParent = false;
      if (this->parent != 0) {
        useParent = (this->parent->classFunctionMatrices.size == this->ownerGroup->conjugacyClassCount()) &&
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
        this->classFunctionMatrices[cci].makeZeroMatrix(this->getDimension());
        for (int i = 0; i < currentCC.elements.size; i ++) {
          if (!this->elementIsComputed[currentCC.indicesEltsInOwner[i]]) {
            this->computeAllElementImages();
          }
          this->classFunctionMatrices[cci] += this->elementImages[currentCC.indicesEltsInOwner[i]];
          if (report.tickAndWantReport()) {
            std::stringstream reportstream;
            reportstream << " Computing conjugacy class " << currentCC.indicesEltsInOwner[i] + 1
            << " (total num classes is " << numClasses << ").";
            report.report(reportstream.str());
          }
        }
      }
      if (report.tickAndWantReport()) {
        std::stringstream reportstream;
        reportstream << "<br>Class function matrix of conjugacy class " << cci + 1
        << " (total num classes is " << numClasses << ") computed to be: "
        << this->classFunctionMatrices[cci].toString();
        report.report(reportstream.str());
      }
    }
    for (int j = 0; j < outputMat.numberOfRows; j ++) {
      for (int k = 0; k < outputMat.numberOfColumns; k ++) {
        outputMat(j, k) += this->classFunctionMatrices[cci](j, k) * MathRoutines::complexConjugate(inputChar[cci]);
      }
    }
  }
}

template <typename somegroup, typename Coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::classFunctionMatrix(
  ClassFunction<somegroup, Coefficient>& inputCF, Matrix<Coefficient>& outputMat
) {
  int dimension = this->generators[0].numberOfRows;
  outputMat.makeZeroMatrix(dimension);
  if (classFunctionMatrices.size == 0) {
    classFunctionMatrices.setSize(this->ownerGroup->conjugacyClassCount());
  }
  for (int cci = 0; cci < this->ownerGroup->conjugacyClassCount(); cci ++) {
    if (inputCF[cci] == 0) {
      continue;
    }
    if (classFunctionMatrices[cci].numberOfColumns == 0) {
      classFunctionMatrices[cci].makeZeroMatrix(this->generators[0].numberOfColumns);
      for (int icci = 0; icci < this->ownerGroup->conjugacyClasses[cci].size; icci ++) {
        this->classFunctionMatrices[cci] += this->getMatrixElement(this->ownerGroup->conjugacyClasses[cci].elements[icci]);
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
std::string GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::toString(FormatExpressions* format) const {
  if (this->ownerGroup == 0) {
    return "non-initialized representation";
  }
  std::stringstream out;
  if (this->flagCharacterIsComputed) {
    out << "Character: " << this->character.toString(format) << " of norm " << this->character.norm();
  } else {
    out << "Character needs to be computed.";
  }
  int theRank = this->ownerGroup->generators.size;
  LargeIntegerUnsigned theLCM, theDen;
  this->getLargestDenominatorSimpleGenerators(theLCM, theDen);
  out << "\n<br>\n LCM denominators simple generators: " << theLCM.toString() << ", largest denominator: " << theDen.toString();
  out << "\n<br>\nThe simple generators (" << theRank << " total):<br> ";
  std::stringstream forYourCopyConvenience;
  for (int i = 0; i < theRank; i ++) {
    if (i < this->generators.size) {
      std::stringstream tempStream;
      tempStream << "s_" << i + 1 << "=MatrixRationals{}" << this->generators[i].toString(format) << "; \\\\\n";
      forYourCopyConvenience << tempStream.str();
      out << HtmlRoutines::getMathNoDisplay("\\begin{array}{l}" + tempStream.str() + "\\end{array}", 3000);
    } else {
      out << "Simple generator " << i + 1 << "} not computed ";
    }
  }
  out << "<br>For your copy convenience: <br>" << forYourCopyConvenience.str();
  return out.str();
}

template <class Coefficient>
void SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::actByNonSimpleElement(int index, Vector<Coefficient>& inputOutput) const {
  Vector<Coefficient> tempRoot;
  this->actByNonSimpleElement(index, inputOutput, tempRoot);
  inputOutput = tempRoot;
}

template <class Coefficient>
void SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::actByNonSimpleElement(
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
  const ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms& element,
  const Vector<Coefficient>& input,
  Vector<Coefficient>& output
) const {
  if (&input == &output) {
    global.fatal << "Input not allowed to coincide with output. " << global.fatal;
  }
  Vector<Coefficient> tempRoot, tempRoot2;
  output = input;
  for (int i = element.generatorsLastAppliedFirst.size - 1; i >= 0; i --) {
    int tempI = element.generatorsLastAppliedFirst[i].index;
    if (tempI < this->simpleRootsInner.size) {
      this->ambientWeyl->reflectBetaWithRespectToAlpha(this->simpleRootsInner[tempI], output, false, output);
    } else {
      tempI -= this->simpleRootsInner.size;
      tempRoot.makeZero(input.size);
      for (int j = 0; j < output.size; j ++) {
        tempRoot2 = this->externalAutomorphisms[tempI][j];
        tempRoot2 *= output[j];
        tempRoot += tempRoot2;
      }
    }
  }
}

template <class Coefficient>
bool SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::isDominantWeight(const Vector<Coefficient>& weight) {
  for (int i = 0; i < this->simpleRootsInner.size; i ++) {
    if (!this->isDominantWithRespectToGenerator(weight, i)) {
      return false;
    }
  }
  return true;
}

template<class Coefficient>
Coefficient SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::weylDimensionFormulaInnerSimpleCoords(
  const Vector<Coefficient>& weightInnerSimpleCoords, const Coefficient& ringUnit
) {
  MacroRegisterFunctionWithName("SubgroupWeylGroupOLD::weylDimensionFormulaSimpleCoordinates");
  this->checkInitialization();
  Coefficient result, buffer;
  Vector<Coefficient> rhoOverNewRing, rootOfBorelNewRing, sumWithRho; //<-to facilitate type conversion!
  Vector<Rational> rho;
  this->rootsOfBorel.sum(rho, this->ambientWeyl->getDimension());
  rho /= 2;
  rhoOverNewRing = rho;//<-type conversion here!
  result = ringUnit;
  for (int i = 0; i < this->rootsOfBorel.size; i ++) {
    rootOfBorelNewRing = this->rootsOfBorel[i]; //<-type conversion here!
    sumWithRho = rhoOverNewRing + weightInnerSimpleCoords;
    buffer = this->ambientWeyl->rootScalarCartanRoot(sumWithRho, rootOfBorelNewRing);
    buffer /= this->ambientWeyl->rootScalarCartanRoot(rhoOverNewRing, rootOfBorelNewRing);
    result *= buffer;
  }
  return result;
}

template <class Coefficient>
bool SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::getAllDominantWeightsHWFDIM(
  Vector<Coefficient>& highestWeightSimpleCoords,
  HashedList<Vector<Coefficient> >& outputWeightsSimpleCoords,
  int upperBoundDominantWeights,
  std::string& outputDetails
) {
  MacroRegisterFunctionWithName("SubgroupWeylGroupOLD::getAllDominantWeightsHWFDIM");
  std::stringstream out;
  this->checkInitialization();
  this->computeRootSubsystem();
  Vector<Coefficient> highestWeightTrue = highestWeightSimpleCoords;
  Vectors<Rational> basisEi;
  int dimension = this->ambientWeyl->getDimension();
  basisEi.makeEiBasis(dimension);
  this->raiseToDominantWeightInner(highestWeightTrue);
  Vector<Coefficient> highestWeightFundCoords = this->ambientWeyl->getFundamentalCoordinatesFromSimple(highestWeightTrue);
  int theTopHeightSimpleCoords = static_cast<int>(highestWeightSimpleCoords.getVectorRational().sumCoordinates().getDoubleValue()) + 1;
  if (theTopHeightSimpleCoords < 0) {
    theTopHeightSimpleCoords = 0;
  }
  List<HashedList<Vector<Coefficient> > > outputWeightsByHeight;
  int topHeightRootSystem = this->ambientWeyl->rootsOfBorel.lastObject()->sumCoordinates().numeratorShort;
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
      for (int i = 0; i < this->rootsOfBorel.size; i ++) {
        currentWeight = currentHashes[lowest];
        currentWeight -= this->rootsOfBorel[i];
        if (this->isDominantWeight(currentWeight)) {
          int currentIndexShift = this->rootsOfBorel[i].sumCoordinates().numeratorShort;
          currentIndexShift = (currentIndexShift + bufferIndexShift) % topHeightRootSystemPlusOne;
          if (outputWeightsByHeight[currentIndexShift].addOnTopNoRepetition(currentWeight)) {
            numTotalWeightsFound ++;
            outputWeightsByHeight[currentIndexShift].adjustHashes();
          }
        }
      }
    }
    outputWeightsSimpleCoords.addListOnTop(currentHashes);
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
void SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::raiseToDominantWeightInner(
  Vector<Coefficient>& weight, int* sign, bool* stabilizerFound
) {
  MacroRegisterFunctionWithName("SubgroupWeylGroupOLD::raiseToDominantWeightInner");
  if (sign != nullptr) {
    *sign = 1;
  }
  if (stabilizerFound != nullptr) {
    *stabilizerFound = false;
  }
  Rational theScalarProd;
//  int dimension= this->ambientWeyl->getDimension();
  for (bool found = true; found; ) {
    found = false;
    for (int i = 0; i < this->simpleRootsInner.size; i ++) {
      if (!this->isDominantWithRespectToGenerator(weight, i)) {
        found = true;
        this->ambientWeyl->reflectBetaWithRespectToAlpha(this->simpleRootsInner[i], weight, false, weight);
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
bool SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::generateOrbitReturnFalseIfTruncated(
  const Vector<Coefficient>& input, Vectors<Coefficient>& outputOrbit, bool restrictToInner, int upperLimitNumberOfElements
) {
  MacroRegisterFunctionWithName("SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::generateOrbitReturnFalseIfTruncated");
  HashedList<Vector<Coefficient> > orbit;
  bool result = true;
  orbit.clear();
  Vector<Coefficient> tempRoot;
  orbit.addOnTop(input);
  Vectors<Coefficient> externalAutosOverAmbientField;
  for (int i = 0; i < orbit.size; i ++) {
    if (upperLimitNumberOfElements > 0) {
      if (orbit.size >= upperLimitNumberOfElements) {
        result = false;
        break;
      }
    }
    for (int j = 0; j < this->simpleRootsInner.size; j ++) {
      this->ambientWeyl->reflectBetaWithRespectToAlpha(this->simpleRootsInner[j], orbit[i], false, tempRoot);
      orbit.addOnTopNoRepetition(tempRoot);
    }
    if (restrictToInner) {
      continue;
    }
    for (int j = 1; j < this->externalAutomorphisms.size; j ++) {
      externalAutosOverAmbientField = this->externalAutomorphisms[j];
      orbit[i].getCoordinatesInBasis(externalAutosOverAmbientField, tempRoot);
      orbit.addOnTopNoRepetition(tempRoot);
    }
  }
  outputOrbit = orbit;
  return result;
}

template <class Coefficient>
bool SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::freudenthalFormulaIrrepIsWRTLeviPart(
  const Vector<Coefficient>& inputHWfundamentalCoords,
  HashedList<Vector<Coefficient> >& outputDominantWeightsSimpleCoordS,
  List<Coefficient>& outputMultsSimpleCoords,
  std::string& outputDetails,
  int UpperBoundFreudenthal
) {
  MacroRegisterFunctionWithName("SubgroupWeylGroupOLD::freudenthalFormulaIrrepIsWRTLeviPart");
  //double startTimer = global.getElapsedSeconds();
  this->computeRootSubsystem();
  Vector<Rational> eiVect;
  List<bool> Explored;
  Coefficient ringZero;
  ringZero = inputHWfundamentalCoords[0].zero();
  /////////////////////////
  Vector<Coefficient> hwSimpleCoordsLeviPart, hwSimpleCoordsNilPart;
  hwSimpleCoordsLeviPart = inputHWfundamentalCoords;
  hwSimpleCoordsNilPart = inputHWfundamentalCoords;
  for (int i = 0; i < hwSimpleCoordsLeviPart.size; i ++) {
    eiVect.makeEi(hwSimpleCoordsLeviPart.size, i);
    if (!this->rootsOfBorel.contains(eiVect)) {
      hwSimpleCoordsLeviPart[i] = ringZero;
    } else {
      hwSimpleCoordsNilPart[i] = ringZero;
    }
  }
  hwSimpleCoordsLeviPart = this->ambientWeyl->getSimpleCoordinatesFromFundamental(hwSimpleCoordsLeviPart);
  hwSimpleCoordsNilPart = this->ambientWeyl->getSimpleCoordinatesFromFundamental(hwSimpleCoordsNilPart);
  ///////////////////////////
  HashedList<Vector<Coefficient> > outputDomWeightsSimpleCoordsLeviPart;

  if (!this->getAllDominantWeightsHWFDIM(
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
  Vector<Coefficient> rho;
  rho = this->getRho();//<-implicit type conversion here!
  Coefficient hwPlusRhoSquared;
  hwPlusRhoSquared = this->ambientWeyl->rootScalarCartanRoot(hwSimpleCoordsLeviPart + rho, hwSimpleCoordsLeviPart + rho);
  Explored[0] = true;
  outputMultsSimpleCoords[0] = 1;
  Vector<Coefficient> convertor;
  Coefficient bufferCoeff;
  ProgressReport report;
  for (int k = 1; k < outputDomWeightsSimpleCoordsLeviPart.size; k ++) {
    Explored[k] = true;
    Coefficient& currentAccum = outputMultsSimpleCoords[k];
    currentAccum = 0;
    for (int j = 0; j < this->rootsOfBorel.size; j ++) {
      for (int i = 1; ; i ++) {
        convertor = this->rootsOfBorel[j];
        convertor *= i;
        currentWeight = outputDomWeightsSimpleCoordsLeviPart[k] + convertor;
        currentDominantRepresentative = currentWeight;
        this->raiseToDominantWeightInner(currentDominantRepresentative);
        int index = outputDomWeightsSimpleCoordsLeviPart.getIndex(currentDominantRepresentative);
        if (index == - 1) {
          break;
        }
        if (!Explored[index]) {
          std::stringstream errorLog;
          errorLog << "This is an internal error check. If you see it, that means "
          << " that the Freudenthal algorithm implementation is "
          << " wrong (because the author of the implementation was dumb enough to"
          << " try to write less code than what is "
          << " suggested by LiE).";
          outputDetails = errorLog.str();
          return false;
        }
        convertor = this->rootsOfBorel[j];//<-implicit type conversion here!
        bufferCoeff = this->ambientWeyl->rootScalarCartanRoot(currentWeight, convertor);
        bufferCoeff *= outputMultsSimpleCoords[index];
        currentAccum += bufferCoeff;
      }
    }
    currentAccum *= 2;
    bufferCoeff = hwPlusRhoSquared;
    bufferCoeff -= this->ambientWeyl->rootScalarCartanRoot(
      outputDomWeightsSimpleCoordsLeviPart[k] + rho, outputDomWeightsSimpleCoordsLeviPart[k] + rho
    );
    //bufferCoeff now holds the denominator participating in the Freudenthal formula.
    if (bufferCoeff.isEqualToZero()) {
      global.fatal << "Coefficient must not be zero at this point. " << global.fatal;
    }
    currentAccum /= bufferCoeff;
    std::stringstream out;
    out << " Computed the multiplicities of " << k + 1 << " out of "
    << outputDomWeightsSimpleCoordsLeviPart.size << " dominant weights in the support.";
    report.report(out.str());
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
  this->data.makeZero(this->G->conjugacyClassCount());
}

template<class someFiniteGroup, typename Coefficient>
Coefficient& ClassFunction<someFiniteGroup, Coefficient>::operator[](int i) const {
  return this->data[i];
}

template<class someFiniteGroup, typename Coefficient>
std::string ClassFunction<someFiniteGroup, Coefficient>::toString(FormatExpressions* format) const {
  (void) format;
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
  if (this->norm().toString() != "1") {
    out << "[";
    out << this->norm();
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
unsigned int ClassFunction<someFiniteGroup, Coefficient>::hashFunction() const {
  return this->hashFunction(*this);
}

template<class someFiniteGroup, typename Coefficient>
unsigned int ClassFunction<someFiniteGroup, Coefficient>::hashFunction(
  const ClassFunction<someFiniteGroup, Coefficient>& input
) {
  unsigned int acc = 0;
  int j = 0;
  for (int i = 0; i < input.data.size; i ++) {
    acc += input.data[i].hashFunction() * HashConstants::getConstantIncrementCounter(j);
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
void SubgroupData<someGroup, elementSomeGroup>::computeCosets() {
  if (flagCosetSetsComputed) {
    return;
  }
  if (this->cosetRepresentativeEnumerator && this->sameCosetAsByFormula) {
    return this->cosetRepresentativeEnumerator(this);
  }
  this->subgroupContent->computeAllElements(true);
  this->groupContent->computeAllElements(true);
  GraphOLD orbitg = GraphOLD(this->groupContent->elements.size, this->subgroupContent->generators.size);
  for (int i = 0; i < this->groupContent->elements.size; i ++) {
    for (int j = 0; j < this->subgroupContent->generators.size; j ++) {
      orbitg.addEdge(
        this->groupContent->elements.getIndex(
          this->groupContent->elements[i] * this->subgroupContent->generators[j]
        ), i
      );
    }
  }
  List<List<int> > orbits;
  orbits = orbitg.destructivelyGetConnectedComponents();
  this->cosets.setSize(orbits.size);
  for (int i = 0; i < orbits.size; i ++) {
    cosets[i].supergroupIndices = orbits[i];
    cosets[i].representative = this->groupContent->elements[orbits[i][0]];
    this->groupContent->getWord(this->groupContent->elements[orbits[i][0]], cosets[i].representativeWord);
  }
  this->flagCosetSetsComputed = true;
  this->flagCosetRepresentativesComputed = true;
  this->verifyCosets();
}

template <typename someGroup, typename elementSomeGroup>
bool SubgroupData<someGroup, elementSomeGroup>::verifyCosets() {
  // this returns true or crashes because if the cosets aren't cosets something is seriously wrong
  computeCosets();
  for (int i = 0; i < this->subgroupContent->generators.size; i ++) {
    for (int cs = 0; cs < this->cosets.size; cs ++) {
      auto g = cosets[cs].representative * this->subgroupContent->generators[i];
      if (this->getCosetId(g) != cs) {
        global.fatal << "Error: element " << g << " not found in coset " << cs
        << " despite being product of subgroup generator "
        << this->subgroupContent->generators[i] << " by coset representative "
        << cosets[cs].representative << global.fatal;
      }
    }
  }
  if (this->flagCosetSetsComputed) {
    for (int cs = 0; cs < this->cosets.size; cs ++) {
      auto g = this->cosets[cs].representative;
      g.invert();
      for (int i = 0; i < this->cosets[cs].supergroupIndices.size; i ++) {
        auto g2 = this->groupContent->elements[this->cosets[cs].supergroupIndices[i]];
        auto g3 = g * g2;
        if (!this->subgroupContent->hasElement(g3)) {
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
void FiniteGroup<elementSomeGroup>::addIrreducibleRepresentation(
  GroupRepresentationCarriesAllMatrices<FiniteGroup<elementSomeGroup>, Rational>& p
) {
  this->irreps_grcam.BSInsertDontDup(p);
  auto p_gr = p.makeOtherGroupRepresentationClass();
  this->addIrreducibleRepresentation(p_gr);
}

template <typename elementSomeGroup>
void FiniteGroup<elementSomeGroup>::addIrreducibleRepresentation(
  GroupRepresentation<FiniteGroup<elementSomeGroup>, Rational>& p
) {
  p.computeCharacter();
  this->irreducibleRepresentations.BSInsertDontDup(p);
  this->characterTable.BSInsertDontDup(p.character);
}

template <typename elementSomeGroup>
void FiniteGroup<elementSomeGroup>::addCharacter(const ClassFunction<FiniteGroup<elementSomeGroup>, Rational>& X) {
  this->characterTable.BSInsertDontDup(X);
  // the following is the reason the characterTable is not a list of pointers
  // this method is kept because (1) it will be inlined anyway
  //                             (2) to remind the user not to use a list of pointers
  /*  ClassFunction<FiniteGroup<elementSomeGroup>, Rational>* place = 0;
  for (int i = 0; i < this->irreps.size; i ++)
    if (this->irreps[i].theCharacter == X)
      place = &(this->irreps[i].theCharacter);
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
void FiniteGroup<elementSomeGroup>::computeIrreducibleRepresentationsTodorsVersion() {
  MacroRegisterFunctionWithName("FiniteGroup::computeIrreducibleRepresentationsTodorsVersion");
  if (this->irreps_grcam.size == 0) {
    if (this->irreducibleRepresentations.size == 0) {
      global.fatal << "Need an initial irrep.  check up the call chain and find out where it should be provided" << global.fatal;
    }
    for (int i = 0; i < this->irreducibleRepresentations.size; i ++) {
      this->irreps_grcam.addOnTop(irreducibleRepresentations[i].makeGRCAM());
    }
  }
  List<GroupRepresentationCarriesAllMatrices<FiniteGroup<elementSomeGroup>, Rational> > appendOnlyIrrepsList;
  appendOnlyIrrepsList = this->irreps_grcam;
  {
    bool nontrivial = false;
    for (int i = 0; i < appendOnlyIrrepsList[0].character.data.size; i ++) {
      if (appendOnlyIrrepsList[0].character.data[i] != 1) {
        nontrivial = true;
        break;
      }
    }
    if (!nontrivial) {
      appendOnlyIrrepsList.removeIndexShiftDown(0);
    }
  }
  int initialcount = appendOnlyIrrepsList.size;

  if (this->elements.size == 0) {
    this->computeConjugacyClassesFromAllElements();
  }
  GroupRepresentationCarriesAllMatrices<FiniteGroup<ElementWeylGroup>, Rational> newRep;
  int NumClasses = this->conjugacyClassCount();
  VirtualRepresentation<FiniteGroup<ElementWeylGroup>, Rational> decompositionNewRep;
  ProgressReport report1;
  //  int indexFirstPredefinedRep = 1; //<-this should be the index of the sign rep.
  //  int indexLastPredefinedrep = 2; //<-this should be the index of the standard rep.
  for (int i = 0; i < appendOnlyIrrepsList.size && this->irreducibleRepresentations.size != NumClasses; i ++) {
    for (int j = 0; j < initialcount; j ++) {
      if (report1.tickAndWantReport()) {
        std::stringstream reportStream;
        reportStream << this->irreducibleRepresentations.size << " irreducible representations found so far. ";
        reportStream << "<br>Decomposing " << appendOnlyIrrepsList[j].character
        << " * " << appendOnlyIrrepsList[i].character << "\n";
        report1.report(reportStream.str());
      }
      newRep = appendOnlyIrrepsList[j];//we are initializing by the sign or natural rep.
      newRep *= appendOnlyIrrepsList[i];
      bool tempB = newRep.decomposeTodorsVersion(decompositionNewRep, &appendOnlyIrrepsList);
      if (!tempB) {
        global.fatal << "This is a mathematical error: failed to decompose " << newRep.character.toString() << ". " << global.fatal;
      }
    }
  }
  if (report1.tickAndWantReport()) {
    std::stringstream reportStream;
    reportStream << "Irrep table:";
    for (int i = 0; i < this->irreducibleRepresentations.size; i ++) {
      reportStream << "\n<br>\n" << this->irreducibleRepresentations[i].character.toString();
    }
    FormatExpressions tempFormat;
    tempFormat.flagUseLatex = true;
    for (int i = 0; i < this->irreducibleRepresentations.size; i ++) {
      reportStream << "<hr>irrep " << i + 1 << "<br>" << this->irreps_grcam[i].toString(&tempFormat);
    }
    report1.report(reportStream.str());
  }
  this->flagCharTableIsComputed = true;
  this->flagIrrepsAreComputed = true;
  if (this->characterTable.size < this->conjugacyClasses.size) {
    global.fatal << "Character table size does not equal the number of conjugacy classes. " << global.fatal;
  }
  if (this->irreducibleRepresentations.size < this->conjugacyClasses.size) {
    global.fatal << "Bad number of irreducible representations. " << global.fatal;
  }
}

template <typename somegroup, typename Coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::spreadVector(
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
      this->elementImages[j + 1].actOnVectorColumn(tempV);
      if (!outputBasisGeneratedSpace.linearSpanContainsVector(tempV)) {
        outputBasisGeneratedSpace.addOnTop(tempV);
      }
    }
  }
}

template <typename somegroup, typename Coefficient>
void GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::getLargestDenominatorSimpleGenerators(
  LargeIntegerUnsigned& outputLCM, LargeIntegerUnsigned& outputDen
) const {
  MacroRegisterFunctionWithName("GroupRepresentationCarriesAllMatrices::getLargestDenominatorSimpleGenerators");
  outputLCM = 1;
  outputDen = 1;
  for (int gi = 0; gi < this->generators.size; gi ++) {
    for (int mi = 0; mi < this->generators[gi].numberOfRows; mi ++) {
      for (int mj = 0; mj < this->generators[gi].numberOfColumns; mj ++) {
        if (this->generators[gi](mi, mj).getDenominator() > outputDen) {
          outputDen = this->generators[gi](mi, mj).getDenominator();
        }
        outputLCM = LargeIntegerUnsigned::leastCommonMultiple(outputLCM, this->generators[gi](mi, mj).getDenominator());
      }
    }
  }
}


template <typename somegroup, typename Coefficient>
bool GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::decomposeTodorsVersionRecursive(
  VirtualRepresentation<somegroup, Coefficient>& outputIrrepMults,
  List<GroupRepresentation<somegroup, Coefficient> >& appendOnlyIrrepsList,
  List<GroupRepresentationCarriesAllMatrices<somegroup, Coefficient> >* appendOnlyGRCAMSList
) {
  MacroRegisterFunctionWithName("WeylGroupRepresentation::decomposeTodorsVersionRecursive");
  this->checkInitialization();

  this->ownerGroup->checkInitializationFiniteDimensionalRepresentationComputation();
  this->getCharacter();
  Coefficient sumOfNumComponentsSquared = this->getNumberOfComponents();
  if (sumOfNumComponentsSquared == 0) {
    global.fatal << "A module has character " << this->character.toString()
    << " of zero length, which is impossible. " << "Here is a printout of the module. "
    << this->toString() << global.fatal;
  }
  if (sumOfNumComponentsSquared == 1) {
    int i = this->ownerGroup->characterTable.BSGetIndex(this->character);
    if (i == - 1) {
      this->ownerGroup->addIrreducibleRepresentation(*this);
      appendOnlyIrrepsList.addOnTop(this->makeOtherGroupRepresentationClass());
      if (appendOnlyGRCAMSList) {
        appendOnlyGRCAMSList->addOnTop(*this);
      }
      i = this->ownerGroup->characterTable.BSGetIndex(this->character);
    }
    outputIrrepMults.addMonomial(this->ownerGroup->characterTable[i], 1);
    return true;
  }
  Matrix<Coefficient> splittingOperatorMatrix;
  Vectors<Coefficient> splittingMatrixKernel, remainingVectorSpace, tempSpace;
  ClassFunction<somegroup, Coefficient> remainingCharacter = this->character;
  remainingVectorSpace.makeEiBasis(this->getDimension());
  ProgressReport Report1, Report2, Report3, Report4;
  {
    std::stringstream reportStream;
    reportStream << "<br>\nDecomposing module with character " << this->character.toString();
    LargeIntegerUnsigned largestDen, lcmDen;
    this->getLargestDenominatorSimpleGenerators(lcmDen, largestDen);
    reportStream << "\n<br>\n Largest denominator is " << largestDen.toString()
    << ", denominator lcm is: " << lcmDen.toString();
    Report1.report(reportStream.str());
  }
  //chop off already known pieces:
  for (int i = 0; i < appendOnlyIrrepsList.size; i ++) {
    Coefficient NumIrrepsOfType = this->character.innerProduct(appendOnlyIrrepsList[i].getCharacter());
    if (NumIrrepsOfType != 0) {
      this->ownerGroup->checkInitializationFiniteDimensionalRepresentationComputation();
      {
        std::stringstream reportStream;
        reportStream << "<hr>\ncontains irrep " << appendOnlyIrrepsList[i].character.toString() << " with multiplicity "
        << NumIrrepsOfType << "\n";
        reportStream << "<hr>\nGetting class f-n matrix from character: " << appendOnlyIrrepsList[i].character;
        Report2.report(reportStream.str());
      }
      this->getClassFunctionMatrix(appendOnlyIrrepsList[i].character, splittingOperatorMatrix);
      {
        std::stringstream reportStream;
        reportStream << "<br>class f-n matrix: " << splittingOperatorMatrix.toString() << "\n <br>\n"
        << " computing its zero eigenspace... ";
        Report3.report(reportStream.str());
      }
      splittingOperatorMatrix.getZeroEigenSpaceModifyMe(splittingMatrixKernel);

      remainingVectorSpace.intersectTwoLinearSpaces(splittingMatrixKernel, remainingVectorSpace, tempSpace);

      // I'm not sure how much of a good idea it is to ensure that outputIrrepMults only takes monomials
      // from ownerGroup->characterTable, it might be better to add the character from irreps pointed to
      // by the appendOnlyIrrepsList[i]
      int ci = this->ownerGroup->characterTable.BSGetIndex(appendOnlyIrrepsList[i].character);
      outputIrrepMults.addMonomial(this->ownerGroup->characterTable[ci], NumIrrepsOfType);
      remainingCharacter -= appendOnlyIrrepsList[i].character*NumIrrepsOfType;
      {
        std::stringstream reportStream;
        reportStream << "<br>Intersecting kernel of class f-n matrix" << splittingMatrixKernel.toString() << " with "
        << remainingVectorSpace.toString() << " to get: " << tempSpace.toString()
        << " with remaining character: " << remainingCharacter.toString();
        Report4.report(reportStream.str());
      }
      remainingVectorSpace = tempSpace;
      if (remainingCharacter[0] != remainingVectorSpace.size) {
        global.fatal << "<br>Remaining char " << remainingCharacter.toString() << " indicates dimension "
        << remainingCharacter[0].toString()
        << " but remaining vector space has dim " << remainingVectorSpace.size << global.fatal;
      }
      if (remainingCharacter.isEqualToZero()) {
        if (remainingVectorSpace.size != 0) {
          global.fatal << "Remaining character is zero but "
          << "remaining space is " << remainingVectorSpace.toString()
          << ". Starting char: " << this->character.toString() << global.fatal;
        }
      }
    }
  }
  if ((remainingVectorSpace.size < this->getDimension()) && (remainingVectorSpace.size > 0)) {
    GroupRepresentationCarriesAllMatrices<somegroup, Coefficient> reducedRep;
    this->restrictRepresentation(remainingVectorSpace, remainingCharacter, reducedRep);
    return reducedRep.decomposeTodorsVersionRecursive(outputIrrepMults, appendOnlyIrrepsList, appendOnlyGRCAMSList);
  }
  if (remainingVectorSpace.size == 0) {
    return true;
  }
  int NumClasses = this->ownerGroup->conjugacyClassCount();
  ClassFunction<WeylGroupData::WeylGroupBase, Coefficient> virtualChar;
  List<Vectors<Coefficient> > subRepresentationBasis;
  for (int cfi = NumClasses - 1; cfi >= 0; cfi --) {
    virtualChar.makeZero(*this->ownerGroup);
    virtualChar[cfi] = 1;
    this->getClassFunctionMatrix(virtualChar, splittingOperatorMatrix);
    bool tempB = splittingOperatorMatrix.getEigenspacesProvidedAllAreIntegralWithEigenValueSmallerThanDimension(subRepresentationBasis);
    if (!tempB) {
      global.fatal << "<br>This is a mathematical or programming mistake: "
      << "splittingOperatorMatrix should have "
      << "small integral values, which it doesn't!" << global.fatal;
    }
    GroupRepresentationCarriesAllMatrices<somegroup, Coefficient> newRep;
    if (subRepresentationBasis.size > 1) {
      //we found splitting, so let us recursively decompose:
      for (int i = 0; i < subRepresentationBasis.size; i ++) {
        remainingCharacter.makeZero(*this->ownerGroup);
        this->restrictRepresentation(subRepresentationBasis[i], remainingCharacter, newRep);
        if (!newRep.decomposeTodorsVersionRecursive(outputIrrepMults, appendOnlyIrrepsList, appendOnlyGRCAMSList)) {
          return false;
        }
      }
      return true;
    }
  }
  Vector<Coefficient> startingVector, tempV, average;
  startingVector.makeEi(this->getDimension(), 0);
  average.makeZero(this->getDimension());
  for (int i = 0; i < this->elementImages.size; i ++) {
    if (!this->elementIsComputed[i]) {
      global.fatal << "<hr>An internal check failed. " << global.fatal;
    }
    this->elementImages[i].actOnVectorColumn(startingVector, tempV);
    average += tempV;
  }
  return false;
}

template <typename somegroup, typename Coefficient>
GroupRepresentation<somegroup, Coefficient> GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::makeOtherGroupRepresentationClass() const {
  GroupRepresentation<somegroup, Coefficient> out;
  out.flagCharacterIsComputed = this->flagCharacterIsComputed;
  out.generators = this->generators;
  out.ownerGroup = this->ownerGroup;
  std::stringstream s;
  for (int i = 0; i < this->names.size; i ++) {
    s << this->names[i];
    if (i < this->names.size - 1) {
      s << " ";
    }
  }
  out.identifyingString = s.str();
  out.character = this->character;
  return out;
}

template <typename elementSomeGroup>
void FiniteGroup<elementSomeGroup>::computeIrreducibleRepresentationsThomasVersion(
  GroupRepresentationCarriesAllMatrices<FiniteGroup<elementSomeGroup>, Rational>* startingIrrep
) {
  MacroRegisterFunctionWithName("WeylGroup::computeIrreducibleRepresentationsThomasVersion");
  if (!startingIrrep) {
    if (this->irreps_grcam.size != 0) {
      startingIrrep = &(this->irreps_grcam[0]);
    } else {
      if (this->irreducibleRepresentations.size != 0) {
        startingIrrep = &(this->irreducibleRepresentations[0].makeGRCAM());
      }
    }
    if (!startingIrrep || (startingIrrep->generators.size > 0 && startingIrrep->generators[0].numberOfRows == 1)) {
      global.fatal << "Can't find a good starting irrep.  If you think you provided one, change the present assertion. "
      << global.fatal;
    }
  }
  GroupRepresentationCarriesAllMatrices<FiniteGroup<elementSomeGroup>, Rational>& sr = *startingIrrep;
  if (this->theGroup.squaresCCReps.size == 0) {
    this->theGroup.computeSquaresCCReps(global);
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
    List<GroupRepresentationCarriesAllMatrices<FiniteGroup<elementSomeGroup>, Rational> > spaces = tspace.decomposeThomasVersion();
    for (int spi = 0; spi < spaces.size; spi ++) {
      if (spaces[spi].getNumberOfComponents() == 1) {
        if (!this->theGroup.characterTable.contains(spaces[spi].getCharacter())) {
          this->theGroup.characterTable.addOnTop(spaces[spi].getCharacter());
          this->irreps_grcam.addOnTop(spaces[spi]);
          newspaces.addOnTop(spaces[spi]);
        }
      } else {
        incompletely_digested.addOnTop(spaces[spi]);
      }
    }
    if (irreps_grcam.size == this->theGroup.conjugacyClassCount()) {
      break;
    }
    for (int spi = 0; spi < incompletely_digested.size; spi ++) {
      for (int ci = 0; ci < this->theGroup.characterTable.size; ci ++) {
        if (incompletely_digested[spi].getCharacter().innerProduct(this->theGroup.characterTable[ci]) != 0) {
          List<GroupRepresentationCarriesAllMatrices<FiniteGroup<elementSomeGroup>, Rational> > shards = incompletely_digested[spi].decomposeThomasVersion();
          incompletely_digested.removeIndexShiftDown(spi);
          for (int shi = 0; shi < shards.size; shi ++) {
            if (shards[shi].getNumberOfComponents() == 1) {
              if (!this->theGroup.characterTable.contains(shards[shi].getCharacter())) {
                this->irreps_grcam.addOnTop(spaces[spi]);
                this->characterTable.addOnTop(this->irreps_grcam.lastObject()->character);
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
  this->irreducibleRepresentations.setSize(0);
  this->characterTable.setSize(0);
  for (int i = 0; i < irreps_grcam.size; i ++) {
    this->irreducibleRepresentations.addOnTop(this->irreps_grcam[i].makeOtherGroupRepresentationClass());
    this->characterTable.addOnTop(&(this->irreducibleRepresentations[i].character));
  }
  this->theGroup.flagCharTableIsComputed = true;
  this->theGroup.flagIrrepsAreComputed = true;
}

template <typename elementSomeGroup>
void FiniteGroup<elementSomeGroup>::computeIrreducibleRepresentations() {
  if (this->ComputeIrreducibleRepresentationsWithFormulas) {
    this->ComputeIrreducibleRepresentationsWithFormulas(*this);
  } else if (this->irreps_grcam.size != 0 || this->irreducibleRepresentations.size != 0) {
    this->computeIrreducibleRepresentationsTodorsVersion();
  } else {
    global.fatal << "FiniteGroup<elementSomeGroup>::computeIrreducibleRepresentations: "
    << "We must have either a formula to generate the irreps, "
    << "or a list of irreps with everything in their tensor products. " << global.fatal;
  }
}

template <typename somegroup, typename Coefficient>
const ClassFunction<somegroup, Coefficient>& GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::getCharacter() {
  if (this->flagCharacterIsComputed) {
    return this->character;
  }
  this->character.G = this->ownerGroup;
  this->character.data.setSize(this->ownerGroup->conjugacyClassCount());
  for (int cci = 0; cci < this->ownerGroup->conjugacyClassCount(); cci ++) {
    this->character.data[cci] = this->getMatrixElement(this->ownerGroup->conjugacyClasses[cci].representative).getTrace();
  }
  this->flagCharacterIsComputed = true;
  return this->character;
}

template <typename somegroup, typename Coefficient>
Coefficient GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>::getNumberOfComponents() {
  ClassFunction<somegroup, Coefficient> X;
  X = getCharacter();
  return X.norm();
}

template <class Coefficient>
void WeylGroupAutomorphisms::raiseToMaximallyDominant(List<Vector<Coefficient> >& weights) {
  MacroRegisterFunctionWithName("WeylGroupAutomorphisms::raiseToMaximallyDominant");
  this->checkInitialization();
  bool found;
  Vectors<Coefficient> weightsCopy;
  this->computeOuterAutomorphisms();
  for (int i = 0; i < weights.size; i ++) {
    do {
      found = false;
      for (int j = 0; j < this->weylGroup->rootsOfBorel.size; j ++) {
        if (this->weylGroup->rootScalarCartanRoot(this->weylGroup->rootsOfBorel[j], weights[i]) < 0) {
          bool isGood = true;
          for (int k = 0; k < i; k ++) {
            if (this->weylGroup->rootScalarCartanRoot(this->weylGroup->rootsOfBorel[j], weights[k]) > 0) {
              isGood = false;
              break;
            }
          }
          if (!isGood) {
            continue;
          }
          for (int k = 0; k < weights.size; k ++) {
            this->weylGroup->reflectBetaWithRespectToAlpha(this->weylGroup->rootsOfBorel[j], weights[k], false, weights[k]);
          }
          found = true;
        }
      }
      Vector<Rational> zeroWeight;
      zeroWeight.makeZero(this->weylGroup->getDimension());
      HashedList<MatrixTensor<Rational> >& outerAutos = this->outerAutomorphisms.elements;
      for (int j = 0; j < outerAutos.size; j ++) {
        weightsCopy = weights;
        outerAutos[j].actOnVectorsColumn(weightsCopy);
        bool isGood = true;
        for (int k = 0; k < i; k ++) {
          if (!(weightsCopy[k] - weights[k]).isPositiveOrZero()) {
            isGood = false;
            break;
          }
        }
        if (!isGood) {
          continue;
        }
        if (!(weightsCopy[i] - weights[i]).isGreaterThanLexicographic(zeroWeight)) {
          continue;
        }
        found = true;
        weights = weightsCopy;
      }
    } while (found);
  }
}
#endif
