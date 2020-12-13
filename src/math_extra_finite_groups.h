// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfCharacterHeader_alreadyIncluded
#define vpfCharacterHeader_alreadyIncluded
#include "general_lists.h"
#include "math_general.h"
#include "math_extra_vector_space.h"
#include "math_extra_graph.h"
#include "json.h"

//To do: move Weyl groups to this file. Eliminate all redundant code and organize nicely.

class SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms;
template <typename someGroup, typename Coefficient>
class GroupRepresentation;

template <typename someGroup, typename Coefficient>
class GroupRepresentationCarriesAllMatrices;

template <typename someGroup, typename elementSomeGroup>
class SubgroupData;

template <typename ElementEuclideanDomain>
class DivisionResult {
public:
  ElementEuclideanDomain quotient;
  ElementEuclideanDomain remainder;
};

template<class someFiniteGroup, typename Coefficient>
class ClassFunction {
public:
  someFiniteGroup* G;
  Vector<Coefficient> data;
  ClassFunction(): G(nullptr) {
  }
  void makeZero(someFiniteGroup& inputWeyl);
  bool isEqualToZero() const {
    return this->data.isEqualToZero();
  }
  Coefficient innerProduct(const ClassFunction& other) const;
  Coefficient norm() const;
  ClassFunction operator*(const ClassFunction& other) const;
  ClassFunction symmetricSecondPower() const;
  ClassFunction alternateSecondPower() const;
  ClassFunction operator+(const ClassFunction& other) const;
  ClassFunction operator-(const ClassFunction& other) const;
  ClassFunction reducedWithCharacters(const List<ClassFunction>& chars);
  Coefficient& operator[](int i) const;
  std::string toString(FormatExpressions* theFormat = nullptr) const;
  static unsigned int hashFunction(const ClassFunction& input);
  unsigned int hashFunction() const {
    return this->hashFunction(*this);
  }
  void operator*=(const Coefficient& inputCF) {
    this->data *= inputCF;
  }
  ClassFunction operator*(const Coefficient& inputCF) const {
    ClassFunction result = *this;
    result.data *= inputCF;
    return result;
  }
  void operator+= (const ClassFunction& right) {
    this->data += right.data;
  }
  void operator-= (const ClassFunction& right) {
    this->data -= right.data;
  }
  void operator*= (const ClassFunction& right);
  bool operator== (const ClassFunction& other) const;
  bool operator!= (const ClassFunction& other) const {
    return !(*this == other);
  }
  bool operator>(const ClassFunction& right) const;
  bool operator<(const ClassFunction& right) const {
    if (*this == right || *this > right) {
      return false;
    }
    return true;
  }
  bool operator<= (const ClassFunction& right) const {
    if (*this > right) {
      return false;
    }
    return true;
  }
  friend std::ostream& operator<<(std::ostream& out, const ClassFunction<someFiniteGroup, Coefficient>& data) {
    out << data.toString();
    return out;
  }
};

// To make a FiniteGroup, define an element class with the following methods
// elementSomeGroup operator*(const elementSomeGroup& right) const;
// void invert();
// elementSomeGroup operator^(const elementSomeGroup& conjugateBy) const;
// static void conjugationAction(const elementSomeGroup& conjugator,
//                               const elementSomeGroup& conjugated,
//                                     elementSomeGroup& out);
// bool hasDifferentConjugacyInvariantsFrom(const elementSomeGroup& other) const;
// //the purpose of this is to test if it is possible for two elements to be conjugate
// //return true if there is an easy way to tell if they are not conjugate
// //return false otherwise
// void getCharacteristicPolynomialStandardRepresentation(Polynomial<Rational>& out) const;
// bool operator== (const elementSomeGroup& right) const;
// bool operator>(const elementSomeGroup& right) const;
// //group elements are not in general comparable, but do something consistent
// //and reasonable for collating and printing purposes
// unsigned int hashFunction() const;
//
// To make certain operations go faster, it might be nice to put smarter algorithms
// in the function pointers in the FiniteGroup class.
//
// Assumptions on the FiniteGroup:
// 1. The finite group has a small number of conjugacy classes. Z/10000000Z is not OK
//
// An uninitialized element is expected to be the identity element to the extend possible.
// Two uninitialized elements can be expected to multiply,
// an an uninitialized element can multiply by an initialized element and where
// needed thereby discover what group it belongs to. Equality and sorting operators
// and the hashFunction need to know what the identity is representation
// It should hash to 0 and sort first.
//
// Elements generally know of a faithful representation, which may by reducible,
// especially over a group that isn't their entire group they want to belong to.
// That is the representation they give the characteristic polynomial for.
// At present, however, ElementZ2N gives the charpoly for one of its one-dimensional
// representations.

template <typename elementSomeGroup>
class FiniteGroup {
public:
  // sizePrivate was protected, but, WeylGroupData needs access
  LargeInteger sizePrivate;

  // certain methods, such as the Get*ByFormula methods, need access to special
  // data not found in FiniteGroup, but in WeylData, PermutationGroupData,
  // and other such classes.  If a Get*ByFormula method is included in
  // a particular FiniteGroup, it may require something pointed from specificDataPointer
  // which should usually be a PermutationGroupData for FiniteGroup<PermutationR2>
  // and so forth
  void* specificDataPointer;

  // It is occasionally useful to a group that is formed as a subgroup of another
  // group to know about its relationship to its parent
  // Currently, 'occasionally useful' means the getWordByFormula method of subgroups
  // that can use the TranslatableWordsSubgroupGetWordByFormula algorithm
  SubgroupData<FiniteGroup<elementSomeGroup>, elementSomeGroup>* parentRelationship;

  List<elementSomeGroup> generators;
  HashedList<elementSomeGroup> theElements;
  HashedList<elementSomeGroup> unionGeneratorsCC;
  struct ConjugacyClass {
    friend std::ostream& operator << (std::ostream& output, const ConjugacyClass& theClass) {
      output << theClass.toString();
      return output;
    }
  public:
    ConjugacyClass() {
      this->flagRepresentativeComputed = false;
      this->flagElementsComputed = false;
    }
    LargeInteger size;
    List<int> indicesEltsInOwner;
    int representativeIndex;
    List<elementSomeGroup> theElements;
    elementSomeGroup representative;
    bool flagRepresentativeComputed;
    bool flagElementsComputed;
    bool flagRepresentativeWordComputed;
    List<int> representativeWord;
    std::string toString(FormatExpressions* theFormat = nullptr) const {
      (void) theFormat;
      std::stringstream out;
      out << "Conj. class size: " << this->size.toString();
      return out.str();
    }
    bool operator>(const ConjugacyClass& other) const {
      return this->representative>other.representative;
    }
  };
  List<ConjugacyClass> conjugacyClasses;
  HashedList<Polynomial<Rational> > characterPolynomialsConjugacyClassesStandardRepresentation;

  // All of these lists will be expected to be sorted at all times, so please
  // insert using .BSInsertDontDup() if you for some reason can't use the friendly
  // insertion methods provided.
  List<GroupRepresentation<FiniteGroup<elementSomeGroup>, Rational> > irreps;
  List<GroupRepresentationCarriesAllMatrices<FiniteGroup<elementSomeGroup>, Rational> > irreps_grcam;
  List<ClassFunction<FiniteGroup<elementSomeGroup>, Rational> > characterTable;

  void addIrreducibleRepresentation(GroupRepresentation<FiniteGroup<elementSomeGroup>, Rational>& r);
  void addIrreducibleRepresentation(GroupRepresentationCarriesAllMatrices<FiniteGroup<elementSomeGroup>, Rational>& r);
  void addCharacter(const ClassFunction<FiniteGroup<elementSomeGroup>, Rational>& X);


  List<elementSomeGroup> squaresCCReps;

  Matrix<int> generatorCommutationRelations;

  List<List<int> > theWords;

  // <-The character polynomials in the ``standard representation''.
  // The ``standard representation'' is specified by the elementSomeGroup class.
  // It is up to the user of the FiniteGroup template to define which representation is
  // ``standard'' - this is not a serious restriction as one can always choose the trivial representation.
  bool flagCCsComputed;
  bool flagCCRepresentativesComputed;
  bool flagAllElementsAreComputed;
  bool flagCharPolysAreComputed;
  bool flagGeneratorsConjugacyClassesComputed;
  bool flagWordsComputed;

  bool flagCanComputeCCsWithOrbitIterator;

  bool flagCharTableIsComputed;
  bool flagIrrepsAreComputed;

  bool flagDeallocated;
  FiniteGroup(): flagDeallocated(false) {
    this->initialize();
  }
  ~FiniteGroup() {
    this->flagDeallocated = true;
  }
  void initializeGenerators() {
  }
  bool checkConsistency() const {
    if (this->flagDeallocated) {
      global.fatal << "Use after free of Finite group. " << global.fatal;
    }
    return true;
  }
  bool checkInitialization() const;
  // WARNING:  This recalculates conjugacy classes, screwing up their order for other methods
  bool checkConjugacyClassRepresentationsMatchCCSizes();
  bool checkOrthogonalityCharacterTable();
  void initialize();
  std::string toString(FormatExpressions* theFormat = nullptr) {
    std::stringstream out;
    out << this->toStringElements(theFormat);
    if (this->flagCCRepresentativesComputed) {
      out << this->toStringConjugacyClasses(theFormat);
    }
    return out.str();
  }
  std::string toStringElements(FormatExpressions* theFormat = nullptr) const;
  std::string toStringConjugacyClasses(FormatExpressions* theFormat = nullptr);
  int conjugacyClassCount() const;
  LargeInteger getSize();
  LargeInteger sizeByFormulaOrNegative1() {
    this->checkConsistency();
    if (this->getSizeByFormula != 0) {
      return getSizeByFormula(*this);
    }
    return - 1;
  } //non-positive result means no formula is known.

  bool (*areConjugateByFormula)(const elementSomeGroup& x, const elementSomeGroup& y);
  void (*computeConjugacyClassSizesAndRepresentativesByFormula)(FiniteGroup<elementSomeGroup>& G);
  LargeInteger (*getSizeByFormula)(FiniteGroup<elementSomeGroup>& G);
  bool areConjugate(const elementSomeGroup& left, const elementSomeGroup& right);
  bool areConjugate_OLD_Deprecated_Version_By_Todor(const elementSomeGroup& left, const elementSomeGroup& right);
  bool computeAllElements(bool andWords, int maximumElements = 1000000);
  // maximumElements gives an upper bound to the number of elements this function will try to find.
  // Set maximumElements = - 1 for "unlimited" upper bound.
  bool computeAllElementsLargeGroup(bool andWords, int maximumElements = 1000000);
  // maximumElements gives an upper bound to the number of elements this function will try to find.
  // Set maximumElements = - 1 for "unlimited" upper bound.

  // Historical note: this was from Thomas' second finite group class, and is
  // as of 2015- 11 the only way to generate the words and conjugacy information
  void computeConjugacyClassesFromAllElements();
  void computeConjugacyClassesFromConjugacyClassIndicesInAllElements(const List<List<int> >& ccIndices);

  void computeCCSizeOrCCFromRepresentative(ConjugacyClass& inputOutputClass, bool storeCC);
  bool registerConjugacyClass(const elementSomeGroup& theRepresentative, bool dontAddIfSameInvariants);
  bool computeConjugacyClassesRepresentatives();
  void computeGeneratorsConjugacyClasses();
  void computeConjugacyClassSizesAndRepresentatives(bool useComputeCCSizesRepresentativesWords = true);
  void computeConjugacyClassSizesAndRepresentativesWithOrbitIterator();
  void computeConjugacyClassesSizesRepresentativesWords();
  bool checkInitializationFiniteDimensionalRepresentationComputation() const;
  void computeSquaresCCReps();
  bool hasElement(const elementSomeGroup& g);
  bool getWord(const elementSomeGroup& g, List<int>& out);
  bool (*getWordByFormula)(FiniteGroup<elementSomeGroup>& G, const elementSomeGroup& g, List<int>& out);
  void getSignCharacter(Vector<Rational>& outputCharacter);
  template <typename Coefficient>
  Coefficient getHermitianProduct(const Vector<Coefficient>& leftCharacter, const Vector<Coefficient>& rightCharacter);

  bool possiblyConjugate(const elementSomeGroup& x, const elementSomeGroup& y);
  void makeIdentity(elementSomeGroup& x);
  bool isIdentity(elementSomeGroup& x);
  void computeGeneratorCommutationRelations();
  void verifyCCSizesAndRepresentativesFormula();
  void verifyWords();
  std::string prettyPrintGeneratorCommutationRelations(bool andPrint = false);
  std::string prettyPrintCharacterTable(bool andPrint = false);
  std::string prettyPrintCCRepsSizes(bool andPrint = false);
  JSData representationDataIntoJS();

  // these methods are refugees from the WeylGroup class
  int multiplyElements(int left, int right) const;
  int invert(int g) const;
  bool checkInitializationConjugacyClasses() const;
  // This method needs some initial irreps to be seeded in either the irreps or the irreps_grcam locations
  void computeIrreducibleRepresentationsTodorsVersion();
  // In order for this to work, the starting irrep must be one whose n-fold tensor products contain every other irrep
  void computeIrreducibleRepresentationsThomasVersion(
    GroupRepresentationCarriesAllMatrices<FiniteGroup<elementSomeGroup>, Rational>* startingIrrep
  );
  void (*ComputeIrreducibleRepresentationsWithFormulas)(FiniteGroup<elementSomeGroup>& G);
  void computeIrreducibleRepresentations();

  // A parabolic subgroup H = <hᵢ> is a subgroup of G = <gᵢ> in which for h∈H,
  // there is a canonical word in gᵢ..gₖ for h in G, with the property that the gᵢ's
  // in h are actually hᵢ's.  Where this happens, it makes it much easier to come up
  // with words in H, to be used for representation theory.
  // The intended use for this API call is
  // auto H = G.parabolicKindaSubgroupGeneratorSubset(subgenids);
  SubgroupData<FiniteGroup<elementSomeGroup>, elementSomeGroup> parabolicKindaSubgroupGeneratorSubset(const List<int>& subgens);

  // auto M = G.getEmptyRationalRepresentation()
  GroupRepresentation<FiniteGroup<elementSomeGroup>, Rational> getEmptyRationalRepresentation();

  friend std::ostream& operator<<(std::ostream& out, FiniteGroup<elementSomeGroup>& data) {
    out << data.toString(0);
    return out;
  }
};

template <class elementSomeGroup>
void FiniteGroup<elementSomeGroup>::initialize() {
  this->generators.setSize(0);
  this->conjugacyClasses.setSize(0);
  this->squaresCCReps.setSize(0);
  this->theElements.clear();
  this->flagAllElementsAreComputed = false;
  this->flagCCsComputed = false;
  this->flagCCRepresentativesComputed = false;
  this->flagCharPolysAreComputed = false;
  this->flagGeneratorsConjugacyClassesComputed = false;
  this->flagWordsComputed = false;
  this->flagCharTableIsComputed = false;
  this->flagIrrepsAreComputed = false;
  this->sizePrivate = 0;
  this->specificDataPointer = nullptr;
  this->areConjugateByFormula = nullptr;
  this->computeConjugacyClassSizesAndRepresentativesByFormula = nullptr;
  this->getSizeByFormula = nullptr;
  this->getWordByFormula = nullptr;
}

struct SimpleReflectionOrOuterAutomorphism {
  bool flagIsOuter;
  int index;
  SimpleReflectionOrOuterAutomorphism(): flagIsOuter(false), index(- 1) {
  }
  std::string toString() const;
  void makeOuterAuto(int inputIndex) {
    this->flagIsOuter = true;
    this->index = inputIndex;
  }
  void makeSimpleReflection(int inputIndex) {
    this->flagIsOuter = false;
    this->index = inputIndex;
  }
  unsigned int hashFunction() const {
    return static_cast<unsigned>(1 + this->index + 100 * flagIsOuter);
  }
  bool operator==(const SimpleReflectionOrOuterAutomorphism& other) const {
    return this->flagIsOuter == other.flagIsOuter && this->index == other.index;
  }
  bool operator>(const SimpleReflectionOrOuterAutomorphism& right) const {
    if (right.flagIsOuter != this->flagIsOuter) {
      return this->flagIsOuter > right.flagIsOuter;
    }
    return this->index > right.index;
  }
  unsigned int hashFunction(const SimpleReflectionOrOuterAutomorphism& input) const {
    return input.hashFunction();
  }
};

struct SimpleReflection {
  int index;
  SimpleReflection() {
    this->index = - 1;
  }
  std::string toString() const;
  void makeSimpleReflection(int inputIndex) {
    this->index = inputIndex;
  }
  unsigned int hashFunction() const {
    return static_cast<unsigned>(index);
  }
  bool operator==(const SimpleReflection& other) const {
    return this->index == other.index;
  }
  bool operator>(const SimpleReflection& right) const {
    return this->index > right.index;
  }
};

class ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms {
public:
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms* owner;
  List<SimpleReflectionOrOuterAutomorphism> generatorsLastAppliedFirst;
  bool flagDeallocated;
  void multiplyOnTheRightBySimpleRootInner(int simpleRootIndex);
  void multiplyOnTheRightByOuterAutomorphism(int indexOuterAutomorphism);
  static unsigned int hashFunction(const ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms& input);
  unsigned int hashFunction() const;
  std::string toString(FormatExpressions* theFormat = nullptr) const;
};

class ElementWeylGroup {
  friend std::ostream& operator << (std::ostream& output, const ElementWeylGroup& theElement) {
    output << theElement.toString();
    return output;
  }
public:
  WeylGroupData* owner;
  List<SimpleReflection> generatorsLastAppliedFirst;
  bool flagDeallocated;
  ElementWeylGroup(const ElementWeylGroup& other) {
    this->flagDeallocated = false;
    this->operator=(other);
  }
  ElementWeylGroup(): owner(nullptr), flagDeallocated(false) {

  }
  ~ElementWeylGroup() {
    this->flagDeallocated = true;
  }
  void operator=(const ElementWeylGroup& other) {
    this->owner = other.owner;
    this->generatorsLastAppliedFirst = other.generatorsLastAppliedFirst;
  }
  template<class Coefficient>
  static void actOn(const ElementWeylGroup& inputElt, const Vector<Coefficient>& inputV, Vector<Coefficient>& output);
  template<class Coefficient>
  void actOn(Vector<Coefficient>& inputOutput) const;
  bool checkConsistency() const;
  bool checkInitialization() const;
  int sign() const;
  void multiplyOnTheRightBySimpleReflection(int reflectionIndex);
  static void conjugationAction(
    const ElementWeylGroup& conjugateWith, const ElementWeylGroup& conjugateOn, ElementWeylGroup& output
  );
  ElementWeylGroup operator^(const ElementWeylGroup& right) const;
  void makeFromRhoImage(const Vector<Rational>& inputRhoImage, WeylGroupData& inputWeyl);
  void makeSimpleReflection(int simpleRootIndex, WeylGroupData& inputWeyl);
  void MakeRootReflection(const Vector<Rational>& mustBeRoot, WeylGroupData& inputWeyl);
  void makeCanonical();
  // This is actually a very reasonable API call to have
  void makeIdentity(WeylGroupData& inputWeyl);
  // but for consistency with the other group element type, we must also have
  void makeIdentity(const FiniteGroup<ElementWeylGroup>& inputGroup);
  void makeIdentity(const ElementWeylGroup& initializeFrom);
  bool isIdentity();
  std::string toStringInvariants(FormatExpressions* theFormat) const;
  std::string toString(FormatExpressions* theFormat = nullptr) const;
  unsigned int hashFunction() const;
  static unsigned int hashFunction(const ElementWeylGroup& input) {
    return input.hashFunction();
  }
  void operator*=(const ElementWeylGroup& other);
  ElementWeylGroup operator*(const ElementWeylGroup& other) const {
    ElementWeylGroup result = *this;
    result *= other;
    return result;
  }
  void getCycleStructure(VectorSparse<Rational>& outputIndexIsCycleSizeCoordinateIsCycleMult) const;
  Vector<Rational> operator*(const Vector<Rational>& v) const;
  ElementWeylGroup inverse() const;
  bool hasDifferentConjugacyInvariantsFrom(const ElementWeylGroup& right) const;
  void getCharacteristicPolynomialStandardRepresentation(Polynomial<Rational>& output) const;
  bool operator==(const ElementWeylGroup& other) const {
    if (this->owner != other.owner) {
      return false;
    }
    return this->generatorsLastAppliedFirst == other.generatorsLastAppliedFirst;
  }
  void makeFromReadableReflections(
    WeylGroupData& input, bool dontmakeCanonical_SET_TRUE_ON_YOUR_OWN_RISK, const std::string& inputReflections
  );
  bool operator>(const ElementWeylGroup& other) const;
};

class WeylGroupAutomorphisms;
class ElementWeylGroupAutomorphisms {
public:
  WeylGroupAutomorphisms* owner;
  List<SimpleReflectionOrOuterAutomorphism> generatorsLastAppliedFirst;
  bool flagDeallocated;
  void makeIdentity(WeylGroupAutomorphisms& inputAutomorphisms);
  ElementWeylGroupAutomorphisms();
  ~ElementWeylGroupAutomorphisms();
  bool checkInitialization() const;
  template<class Coefficient>
  static void actOn(
    const ElementWeylGroupAutomorphisms& inputElt, const Vector<Coefficient>& inputV, Vector<Coefficient>& output
  );
  template<class Coefficient>
  void actOn(Vector<Coefficient>& inputOutput) const;
  void makeOuterAuto(int outerAutoIndex, WeylGroupAutomorphisms& inputWeyl);
  void makeSimpleReflection(int simpleRootIndex, WeylGroupAutomorphisms& inputWeyl);
  void multiplyOnTheRightByOuterAuto(int outerAutoIndex);
  unsigned int hashFunction() const;
  static unsigned int hashFunction(const ElementWeylGroupAutomorphisms& input);
  std::string toString(FormatExpressions* theFormat = nullptr) const;
};

template <class Coefficient>
class FinitelyGeneratedMatrixMonoid {
  public:
  List<MatrixTensor<Coefficient> > theGenerators;
  HashedList<MatrixTensor<Coefficient> > theElements;
  bool generateElements(int upperBoundNonPositiveMeansNoLimit);
  std::string toString(FormatExpressions* theFormat = nullptr) const;
};

template <class Coefficient>
bool FinitelyGeneratedMatrixMonoid<Coefficient>::generateElements(int upperBoundNonPositiveMeansNoLimit) {
  MacroRegisterFunctionWithName("FinitelyGeneratedMatrixMonoid::generateElements");
  this->theElements.clear();
  this->theElements.addOnTopNoRepetition(theGenerators);
  MatrixTensor<Coefficient> currentElement;
  for (int i = 0; i < this->theElements.size; i ++) {
    for (int j = 0; j < this->theGenerators.size; j ++) {
      currentElement = this->theGenerators[j];
      currentElement *= this->theElements[i];
      this->theElements.addOnTopNoRepetition(currentElement);
      if (upperBoundNonPositiveMeansNoLimit > 0) {
        if (this->theElements.size > upperBoundNonPositiveMeansNoLimit) {
          return false;
        }
      }
    }
  }
  return true;
}

template <typename somegroup, typename Coefficient>
class VirtualRepresentation;

template <typename somegroup, typename Coefficient>
class GroupRepresentationCarriesAllMatrices;

class SubgroupDataRootReflections;

class WeylGroupData {
  Matrix<Rational> fundamentalToSimpleCoords;
  Matrix<Rational> simpleToFundamentalCoords;
  MemorySaving<Matrix<Rational> > matrixSendsSimpleVectorsToEpsilonVectors;
  bool flagFundamentalToSimpleMatricesAreComputed;
  void computeFundamentalToSimpleMatrices() {
    if (this->flagFundamentalToSimpleMatricesAreComputed) {
      return;
    }
    Vectors<Rational> fundamentalBasis;
    this->getFundamentalWeightsInSimpleCoordinates(fundamentalBasis);
    this->fundamentalToSimpleCoords.assignVectorsToRows(fundamentalBasis);
    this->fundamentalToSimpleCoords.transpose();
    this->simpleToFundamentalCoords = this->fundamentalToSimpleCoords;
    this->simpleToFundamentalCoords.invert();
    this->flagFundamentalToSimpleMatricesAreComputed = true;
  }
  bool loadConjugacyClasses();
  bool loadCharacterTable();
  WeylGroupData(const WeylGroupData& other);
  void operator=(const WeylGroupData& other);
  //<- once created, WeylGroupData can't be moved: a pointer to it is stored in FiniteGroup
public:
  bool flagIrrepsAreComputed;
  bool flagCharTableIsComputed;

  DynkinType theDynkinType;
  Matrix<Rational> cartanSymmetric;
  Matrix<Rational> coCartanSymmetric;
  Vector<Rational> rho;
  HashedList<Vector<Rational> > rhoOrbit;
  HashedList<Vector<Rational> > rootSystem;
  Vectors<Rational> rootsOfBorel;
  FiniteGroup<ElementWeylGroup> theGroup;

  List<std::string> ccCarterLabels;
  List<std::string> irrepsCarterLabels;
  // void MakeFromParSel(Vector<Rational> & parSel, WeylGroup& input);
  bool flagDeallocated;

  typedef FiniteGroup<ElementWeylGroup> WeylGroupBase;
  void reset();
  static void getCoCartanSymmetric(const Matrix<Rational>& input, Matrix<Rational>& output);
  void computeSquares();
  void computeInitialIrreducibleRepresentations();
  void getSignSignatureParabolics(List<SubgroupDataRootReflections>& outputSubgroups);
  void getSignSignatureExtendedParabolics(List<SubgroupDataRootReflections>& outputSubgroups);
  void getSignSignatureAllRootSubsystems(List<SubgroupDataRootReflections>& outputSubgroups);
  void getSignSignatureRootSubgroups(
    List<SubgroupDataRootReflections>& outputSubgroups,
    const List<Vectors<Rational> >& rootsGeneratingReflections
  );
  bool loadConjugacyClassesHelper();
  bool loadSignSignatures(List<SubgroupDataRootReflections>& outputSubgroups);
  void computeOrLoadCharacterTable(std::stringstream* reportStream = nullptr);
  void computeOrLoadConjugacyClasses(std::stringstream* reportStream = nullptr);
  static void computeIrreducibleRepresentationsWithFormulasImplementation(FiniteGroup<ElementWeylGroup>& G);
  void computeExtremeRootInTheSameKMod(
    const Vectors<Rational>& inputSimpleBasisK,
    const Vector<Rational>& inputRoot,
    Vector<Rational>& output,
    bool lookingForHighest
  );
  void addCharacter(const ClassFunction<WeylGroupData::WeylGroupBase, Rational>& X);
  void computeRho(bool recompute);
  std::string toStringRootsAndRootReflections(FormatExpressions* theFormat = nullptr);
  std::string toString(FormatExpressions* theFormat = nullptr);
  std::string toStringCppConjugacyClasses(FormatExpressions* theFormat = nullptr);
  std::string toStringCppCharTable(FormatExpressions* theFormat = nullptr);
  std::string toStringIrreducibleRepresentationLabel(int irrepIndex);
  std::string toStringSignSignatureRootSubsystem(const List<SubgroupDataRootReflections>& inputSubgroups);
  void makeArbitrarySimple(char WeylGroupLetter, int n, const Rational* firstCoRootLengthSquared = nullptr);
  void makeFromDynkinType(const DynkinType& inputType);
  void makeFinalSteps();
  void initializeGenerators();
  template <class Coefficient>
  LargeInteger getOrbitSize(Vector<Coefficient>& theWeight);
  void makeMeFromMyCartanSymmetric();
  void makeFromDynkinTypeDefaultLengthKeepComponentOrder(const DynkinType& inputType);
  void computeCoCartanSymmetricFromCartanSymmetric();
  bool checkConsistency() const;
  static bool getWordByFormulaImplementation(
    FiniteGroup<ElementWeylGroup>& G, const ElementWeylGroup& g, List<int>& out
  );
  void getSignCharacter(Vector<Rational>& out);
  void getStandardRepresentation(
    GroupRepresentationCarriesAllMatrices<FiniteGroup<ElementWeylGroup>, Rational>& output
  );
  void getSignRepresentation(
    GroupRepresentationCarriesAllMatrices<FiniteGroup<ElementWeylGroup>, Rational>& output
  );
  void getTrivialRepresentation(
    GroupRepresentationCarriesAllMatrices<FiniteGroup<ElementWeylGroup>, Rational>& output
  );
  void generateAdditivelyClosedSubset(Vectors<Rational>& input, Vectors<Rational>& output);
  bool loadGAPRootSystem(HashedList<Vector<Rational> >& outputPositiveRootSystem) const;
  Rational getKillingDividedByTraceRatio();
  bool containsRootNonStronglyPerpendicularTo(Vectors<Rational>& theVectors, Vector<Rational>& input);
  int numberOfRootsConnectedTo(Vectors<Rational>& theVectors, Vector<Rational>& input);
  void getHighestWeightsAllRepresentationsDimensionLessThanOrEqualTo(
    List<Vector<Rational> >& outputHighestWeightsFundCoords, int inputDimBound
  );
  Rational getLongestRootLengthSquared() const;
  static unsigned int hashFunction(const WeylGroupData& input) {
    return input.cartanSymmetric.hashFunction();
  }
  unsigned int hashFunction() const {
    return this->hashFunction(*this);
  }
  WeylGroupData() {
    this->flagDeallocated = false;
    this->reset();
  }
  ~WeylGroupData() {
    this->flagDeallocated = true;
  }
  bool isOfSimpleType(char desiredType, int desiredRank) const {
    return this->theDynkinType.isOfSimpleType(desiredType, desiredRank);
  }
  Matrix<Rational>* getMatrixFundamentalToSimpleCoords() {
    this->computeFundamentalToSimpleMatrices();
    return &this->fundamentalToSimpleCoords;
  }
  Matrix<Rational>* getMatrixSimpleToFundamentalCoords() {
    this->computeFundamentalToSimpleMatrices();
    return &this->simpleToFundamentalCoords;
  }
  template<class Coefficient>
  Vector<Coefficient> getSimpleCoordinatesFromFundamental(
    const Vector<Coefficient>& inputInFundamentalCoords,
    const Coefficient& zero = Coefficient::zero()
  );
  template<class Coefficient>
  Vectors<Coefficient> getSimpleCoordinatesFromFundamental(
    const Vectors<Coefficient>& inputInFundamentalCoords,
    const Coefficient& zero = Coefficient::zero()
  );
  template<class Coefficient>
  Vector<Coefficient> getFundamentalCoordinatesFromSimple(const Vector<Coefficient>& inputInSimpleCoords);
  template<class Coefficient>
  Vector<Coefficient> getFundamentalCoordinatesFromEpsilon(const Vector<Coefficient>& inputInEpsCoords);
  template<class Coefficient>
  Vector<Coefficient> getSimpleCoordinatesFromEpsilon(const Vector<Coefficient>& inputInEpsCoords);
  template<class Coefficient>
  Vector<Coefficient> getDualCoordinatesFromSimple(const Vector<Coefficient>& inputInSimpleCoords) {
    return this->getDualCoordinatesFromFundamental(this->getFundamentalCoordinatesFromSimple(inputInSimpleCoords));
  }
  template <class Coefficient>
  Vector<Coefficient> getDualCoordinatesFromFundamental(const Vector<Coefficient>& inputInFundamentalCoords) {
    Vector<Coefficient> result = inputInFundamentalCoords;
    if (result.size != this->getDimension()) {
      global.fatal << "The input fundamental weight has "
      << result.size << " coordinates, while the rank of the Weyl group is "
      << this->getDimension() << ". " << global.fatal;
    }
    for (int i = 0; i < result.size; i ++) {
      result[i] *= this->cartanSymmetric.elements[i][i] / 2;
    }
    return result;
  }
  template <class Coefficient>
  Coefficient getScalarProductSimpleRoot(const Vector<Coefficient>& input, int indexSimpleRoot) const {
    if (indexSimpleRoot < 0 || indexSimpleRoot >= this->getDimension()) {
      global.fatal
      << "Attempting to take scalar product with simple root of index "
      << indexSimpleRoot
      << " which is impossible, as the rank of the Weyl group is "
      << this->getDimension() << ". " << global.fatal;
    }
    Coefficient result, buffer;
    result = 0;
    Rational* currentRow = this->cartanSymmetric.elements[indexSimpleRoot];
    for (int i = 0; i < input.size; i ++) {
      buffer = input[i];
      buffer *= currentRow[i];
      result += buffer;
    }
    return result;
  }
  template <class Coefficient>
  Coefficient weylDimensionFormulaSimpleCoordinates(
    Vector<Coefficient>& theWeightInSimpleCoords,
    const Coefficient& ringUnit = 1
  );
  template <class Coefficient>
  Coefficient weylDimFormulaFundamentalCoords(Vector<Coefficient>& weightFundCoords);
  template <class Coefficient>
  void raiseToDominantWeight(
    Vector<Coefficient>& theWeight,
    int* sign = nullptr,
    bool* stabilizerFound = nullptr,
    ElementWeylGroup* raisingElt = nullptr
  );
  bool areMaximallyDominantGroupInner(List<Vector<Rational> >& theWeights);
  template <class Coefficient>
  void raiseToMaximallyDominant(List<Vector<Coefficient> >& theWeights);
  void getCoxeterPlane(Vector<double>& outputBasis1, Vector<double>& outputBasis2);
  void getSimpleReflectionMatrix(int indexSimpleRoot, Matrix<Rational>& output) const;
  ElementWeylGroup simpleConjugation(int i, const ElementWeylGroup& vv);
  Matrix<Rational> simpleReflectionMatrix(int indexSimpleRoot) const;
  void getStandardRepresentationMatrix(int g, Matrix<Rational>& output) const;
  void drawRootSystem(
    DrawingVariables& outputDV,
    bool wipeCanvas,
    bool drawWeylChamber,
    Vector<Rational>* bluePoint = nullptr,
    bool LabelDynkinDiagramVertices = false,
    Vectors<Rational>* predefinedProjectionPlane = nullptr
  );
  bool hasStronglyPerpendicularDecompositionWRT(
    Vector<Rational>& input,
    int UpperBoundNumBetas,
    Vectors<Rational>& theSet,
    Vectors<Rational>& output,
    List<Rational>& outputCoeffs,
    bool IntegralCoefficientsOnly
  );
  // void makeFromDynkinType(List<char>& theLetters, List<int>& theRanks, List<int>* theMultiplicities);
  // void makeFromDynkinType(List<char>& theLetters, List<int>& theRanks){ this->makeFromDynkinType(theLetters, theRanks, 0); }
  // void GetLongRootLength(Rational& output);
  static bool isAddmisibleDynkinType(char candidateLetter, int n);
  // the below will not do anything if the inputLetter is not a valid Dynkin letter
  static void transformToAdmissibleDynkinType(char inputLetter, int& outputRank);
  void computeEpsilonMatrix();
  template <class Coefficient>
  void getEpsilonCoordinates(const Vector<Coefficient>& input, Vector<Coefficient>& output) {
    this->computeEpsilonMatrix();
    output = input;
    this->matrixSendsSimpleVectorsToEpsilonVectors.getElement().actOnVectorColumn(output, Coefficient::zero());
  }
  void getEpsilonCoordinates(const List<Vector<Rational> >& input, Vectors<Rational>& output);
  template <class Coefficient>
  Vector<Coefficient> getEpsilonCoordinates(const Vector<Coefficient>& input) {
    Vector<Coefficient> output;
    this->getEpsilonCoordinates(input, output);
    return output;
  }
  bool isStronglyPerpendicularTo(const Vector<Rational>& input, const Vector<Rational>& other);
  bool isStronglyPerpendicularTo(const Vector<Rational>& input, const Vectors<Rational>& others);
  void getEpsilonCoordinatesWRTsubalgebra(Vectors<Rational>& generators, List<Vector<Rational> >& input, Vectors<Rational>& output);
  bool leftIsHigherInBruhatOrderThanRight(ElementWeylGroup& left, ElementWeylGroup& right);
  bool isElementWeylGroup(const MatrixTensor<Rational>& theMat);
  void getMatrixReflection(Vector<Rational>& reflectionRoot, Matrix<Rational>& output);
  template <class Coefficient>
  bool getAllDominantWeightsHWFDIM(
    Vector<Coefficient>& highestWeightSimpleCoords,
    HashedList<Vector<Coefficient> >& outputWeightsSimpleCoords,
    int upperBoundDominantWeights,
    std::string* outputDetails
  );
  template <class Coefficient>
  bool freudenthalFormula(
    Vector<Coefficient>& inputHWfundamentalCoords,
    HashedList<Vector<Coefficient> >& outputDominantWeightsSimpleCoords,
    List<Coefficient>& outputMultsSimpleCoords,
    std::string* outputDetails,
    int UpperBoundFreudenthal
  );
  void getWeylChamber(Cone& output);
  std::string generateWeightSupportMethod1(
    Vector<Rational>& highestWeightSimpleCoords, Vectors<Rational>& outputWeightsSimpleCoords, int upperBoundWeights
  );
  void getIntegralLatticeInSimpleCoordinates(Lattice& output);
  void getFundamentalWeightsInSimpleCoordinates(Vectors<Rational>& output);
  int getDimension() const {
    return this->cartanSymmetric.numberOfRows;
  }
  void computeWeylGroupAndRootsOfBorel(Vectors<Rational>& output);
  void computeRootsOfBorel(Vectors<Rational>& output);
  static LargeInteger getSizeByFormulaImplementation(FiniteGroup<ElementWeylGroup>& G);
  static LargeInteger sizeByFormulaOrNegative1(char weylLetter, int theDim);
  bool isARoot(const Vector<Rational>& input) const {
    return this->rootSystem.contains(input);
  }
  void generateRootSubsystem(Vectors<Rational>& theRoots);
  template <class Coefficient>
  bool generateOrbit(
    Vectors<Coefficient>& theWeights,
    bool RhoAction,
    HashedList<Vector<Coefficient> >& output,
    bool UseMinusRho,
    int expectedOrbitSize = - 1,
    HashedList<ElementWeylGroup>* outputSubset = nullptr,
    int upperLimitNumberOfElements = - 1
  );
  template <class Coefficient>
  bool generateOrbit(
    Vector<Coefficient>& theWeight,
    bool RhoAction,
    HashedList<Vector<Coefficient> >& output,
    bool UseMinusRho,
    int expectedOrbitSize = - 1,
    HashedList<ElementWeylGroup>* outputSubset = nullptr,
    int upperLimitNumberOfElements = - 1
  ) {
    Vectors<Coefficient> theWeights;
    theWeights.addOnTop(theWeight);
    return this->generateOrbit(
      theWeights, RhoAction, output, UseMinusRho, expectedOrbitSize, outputSubset, upperLimitNumberOfElements
    );
  }
//  int GetNumRootsFromFormula();
  void generateRootSystem();
  void actOnAffineHyperplaneByGroupElement(int index, AffineHyperplane<Rational>& output, bool RhoAction, bool UseMinusRho);
  void getLowestElementInOrbit(
    Vector<Rational>& inputOutput,
    ElementWeylGroup* outputWeylElt,
    Vectors<Rational>& bufferEiBAsis,
    bool RhoAction,
    bool UseMinusRho,
    int* sign = nullptr,
    bool* stabilizerFound = nullptr
  ) {
    this->getExtremeElementInOrbit(
      inputOutput, outputWeylElt, bufferEiBAsis, true, RhoAction, UseMinusRho, sign, stabilizerFound
    );
  }
  void getHighestElementInOrbit(
    Vector<Rational>& inputOutput,
    ElementWeylGroup* outputWeylElt,
    Vectors<Rational>& bufferEiBAsis,
    bool RhoAction,
    bool UseMinusRho,
    int* sign,
    bool* stabilizerFound
  ) {
    this->getExtremeElementInOrbit(
      inputOutput, outputWeylElt, bufferEiBAsis, false, RhoAction, UseMinusRho, sign, stabilizerFound
    );
  }
  void getExtremeElementInOrbit(
    Vector<Rational>& inputOutput,
    ElementWeylGroup* outputWeylElt,
    Vectors<Rational>& bufferEiBAsis,
    bool findLowest,
    bool RhoAction,
    bool UseMinusRho,
    int* sign,
    bool* stabilizerFound
  );
  void getLongestWeylElt(ElementWeylGroup& outputWeylElt);
  bool isEigenSpaceGeneratorCoxeterElement(Vector<Rational>& input);
  void getCoxeterElement(ElementWeylGroup& outputWeylElt) {
    outputWeylElt.generatorsLastAppliedFirst.setSize(this->getDimension());
    for (int i = 0; i < outputWeylElt.generatorsLastAppliedFirst.size; i ++) {
      outputWeylElt.generatorsLastAppliedFirst[i].makeSimpleReflection(i);
    }
  }
  Vector<Rational> applyReflectionList(const List<int>& rightReflectionsActFirst, const Vector<Rational>& vv) const;
  template <class Coefficient>
  void actOn(const ElementWeylGroup& theGroupElement, Vector<Coefficient>& inputOutput) const {
    this->actOn(theGroupElement, inputOutput, inputOutput);
  }
  template <class Coefficient>
  void actOn(int indexOfWeylElement, Vector<Coefficient>& theVector) const {
    this->actOn(this->theGroup.theElements[indexOfWeylElement], theVector, theVector);
  }
  template <class Coefficient>
  void actOn(
    const ElementWeylGroup& theGroupElement,
    const Vector<Coefficient>& inputVector,
    Vector<Coefficient>& outputVector
  ) const {
    outputVector = inputVector;
    for (int i = theGroupElement.generatorsLastAppliedFirst.size - 1; i >= 0; i --) {
      this->reflectSimple(theGroupElement.generatorsLastAppliedFirst[i].index, outputVector);
    }
  }
  template <class Coefficient>
  void actOnRhoModified(const ElementWeylGroup& theGroupElement, Vector<Coefficient>& theVector) const {
    Vector<Coefficient> tempV;
    theVector += this->rho;
    this->actOn(theGroupElement, theVector, tempV);
    theVector = tempV;
    theVector -= this->rho;
  }
  template <class Coefficient>
  void actOnRhoModified(int indexOfWeylElement, Vector<Coefficient>& theVector) const {
    this->actOnRhoModified(this->theGroup.theElements[indexOfWeylElement], theVector);
  }
  template <class Coefficient>
  void actOnDual(int index,Vector<Coefficient>& theVector, bool RhoAction, const Coefficient& ringZero);
  //theRoot is a list of the simple coordinates of the Vector<Rational>
  //theRoot serves as both input and output
  void actOnRootAlgByGroupElement(int index, PolynomialSubstitution<Rational>& theRoot, bool RhoAction);
  void actOnRootByGroupElement(int index, Vector<Rational>& theRoot, bool RhoAction, bool UseMinusRho);
  // Simple reflection with respect to vector alpha.
  // If neither rhoAction nor minusRho is set, this is equivalent to
  // the transformation v -> v - 2 * <alpha, v> / <alpha, alpha> alpha,
  // where <*,*> stands for the root scalar product.
  //
  // Let s be the transformation above. If rhoAction is set, then
  // this function will instead carry out the transformation v -> s(v - rho) + rho,
  // where rho is the half-sum of the positive roots.
  //
  // If minusRho and rhoAction are both set, then instead we do v -> s(v + rho) - rho.
  void reflectSimple(int index, Vector<Rational>& theRoot, bool rhoAction, bool useMinusRho);
  // Simple reflection with respect to the simple root with given index.
  // Acts on the second argument in place.
  template <class Coefficient>
  void reflectSimple(int index, Vector<Coefficient>& theVector) const;
  template <class Coefficient>
  void reflectRhoSimple(int index, Vector<Coefficient>& theVector) const;
  template <class Coefficient>
  void reflectMinusRhoSimple(int index, Vector<Coefficient>& theVector) const;
  ElementWeylGroup getRootReflection(int rootIndex);
  void getWord(int g, List<int>& out) const;
  void getWord(const ElementWeylGroup& g, List<int>& out) const {
    out.setSize(g.generatorsLastAppliedFirst.size);
    for (int i = 0; i < g.generatorsLastAppliedFirst.size; i ++) {
      out[i] = g.generatorsLastAppliedFirst[i].index;
    }
  }
  Matrix<Rational> getMatrixStandardRepresentation(int elementIndex) const;
  void getMatrixStandardRepresentation(const ElementWeylGroup& input, Matrix<Rational>& outputMatrix) const;
  Matrix<Rational> getMatrixStandardRepresentation(const ElementWeylGroup& input) const {
    Matrix<Rational> result;
    this->getMatrixStandardRepresentation(input, result);
    return result;
  }
  void simpleReflectionDualSpace(int index, Vector<Rational>& DualSpaceElement);
  void simpleReflectionRootPolynomial(int index, PolynomialSubstitution<Rational>& theRoot, bool RhoAction);
  bool isPositiveOrPerpWithRespectToH(const Vector<Rational>& input, const Vector<Rational>& theH) {
    return this->rootScalarCartanRoot(input, theH).isPositiveOrZero();
  }
  template<class Coefficient>
  void reflectBetaWithRespectToAlpha(
    const Vector<Rational>& alpha, const Vector<Coefficient>& Beta, bool RhoAction, Vector<Coefficient>& Output
  ) const;
  bool isRegular(Vector<Rational>& input, int* indexFirstPerpendicularRoot);
  template <class leftType, class rightType>
  void rootScalarCartanRoot(const Vector<leftType>& r1, const Vector<rightType>& r2, leftType& output) const;
  double rootScalarCartanRoot(const Vector<double>& r1, const Vector<double>& r2) const {
    if (r1.size != r2.size || r1.size != this->getDimension()) {
      global.fatal << "Attempt to take the root system scalar product of "
      << "vectors of different dimension or of dimension different "
      << "from that of the ambient Lie algebra. The two input vectors were "
      << r1 << " and " << r2 << " and the rank of the Weyl group is " << this->getDimension() << ". ";
      global.fatal << global.fatal;
    }
    double result = 0;
    for (int i = 0; i < this->getDimension(); i ++) {
      for (int j = 0; j < this->getDimension(); j ++) {
        result += this->cartanSymmetric.elements[i][j].getDoubleValue() * r1[i] * r2[j];
      }
    }
    return result;
  }
  template <class leftType, class rightType>
  leftType rootScalarCartanRoot(const Vector<leftType>& r1, const Vector<rightType>& r2) const {
    leftType tempRat;
    this->rootScalarCartanRoot(r1, r2, tempRat);
    return tempRat;
  }
  // the below functions perturbs input so that inputH has
  // non-zero scalar product with Vectors<Rational> of the Vector<Rational>  system,
  // without changing the inputH-sign of any Vector<Rational>  that had a non-zero scalar product to begin with
  void perturbWeightToRegularWithRespectToRootSystem(const Vector<Rational>& inputH, Vector<Rational>& output);
  template <class Coefficient>
  bool isDominantWithRespectToGenerator(const Vector<Coefficient>& theWeight, int generatorIndex);
  template <class Coefficient>
  bool isDominantWeight(const Vector<Coefficient>& theWeight);
  static void transformToSimpleBasisGenerators(
    Vectors<Rational>& theGens, const HashedList<Vector<Rational> >& inputRootSystem
  );
  static void transformToSimpleBasisGeneratorsArbitraryCoordinates(
    Vectors<Rational>& theGens, const HashedList<Vector<Rational> >& inputRootSystem
  );
  void transformToSimpleBasisGeneratorsWithRespectToH(Vectors<Rational>& theGens, const Vector<Rational>& theH);
  int operator()(int i, int j) const;
  bool operator==(const WeylGroupData& other) const;
  void operator+=(const WeylGroupData& other);
};

template<class leftType, class rightType>
void WeylGroupData::rootScalarCartanRoot(const Vector<leftType>& r1, const Vector<rightType>& r2, leftType& output) const {
  if (r1.size != r2.size || r1.size != this->getDimension()) {
    global.fatal << "Attempt to get the scalar product of the weight "
    << r1 << " (dimension " << r1.size
    << ") with the weight " << r2 << " (dimension " << r2.size
    << "), while the dimension of the ambient Weyl group is " << this->getDimension()
    << ". ";
    global.fatal << global.fatal;
  }
  output = r1[0].zero();
  leftType buffer;
  for (int i = 0; i < this->cartanSymmetric.numberOfRows; i ++) {
    for (int j = 0; j < this->cartanSymmetric.numberOfColumns; j ++) {
      buffer = r1[i];
      buffer *= r2[j];
      buffer *= this->cartanSymmetric.elements[i][j];
      output += buffer;
    }
  }
}

template<class Coefficient>
void ElementWeylGroup::actOn(const ElementWeylGroup& inputElt, const Vector<Coefficient>& inputV, Vector<Coefficient>& output) {
  inputElt.checkInitialization();
  inputElt.owner->actOn(inputElt, inputV, output);
}

template<class Coefficient>
void ElementWeylGroup::actOn(Vector<Coefficient>& inputOutput) const {
  this->checkInitialization();
  this->owner->actOn(*this, inputOutput, inputOutput);
}

class WeylGroupAutomorphisms {
public:
  bool flagOuterAutosGeneratorsComputed;
  bool flagAllOuterAutosComputed;
  bool flagDeallocated;
  WeylGroupData* theWeyl;
  FinitelyGeneratedMatrixMonoid<Rational> theOuterAutos;
  HashedList<ElementWeylGroupAutomorphisms> allElements;
  void computeOuterAutoGenerators();
  void computeOuterAutomorphisms();
  WeylGroupAutomorphisms();
  ~WeylGroupAutomorphisms();
  template <class Coefficient>
  bool generateOuterOrbit(
    Vectors<Coefficient>& theWeights,
    HashedList<Vector<Coefficient> >& output,
    HashedList<ElementWeylGroupAutomorphisms>* outputSubset = nullptr,
    int upperLimitNumberOfElements = - 1
  );
  LargeInteger getOrbitSize(Vector<Rational>& theWeight);
  bool isElementWeylGroupOrOuterAutomorphisms(const MatrixTensor<Rational>& theMat);
  bool areMaximallyDominantGroupOuter(List<Vector<Rational> >& theWeights);
  bool checkInitialization() const;

  template <class Coefficient>
  void actOn(
    const ElementWeylGroupAutomorphisms& theGroupElement,
    const Vector<Coefficient>& inputVector,
    Vector<Coefficient>& outputVector
  ) const;
  template <class Coefficient>
  void raiseToMaximallyDominant(List<Vector<Coefficient> >& theWeights);
};

template<class Coefficient>
void ElementWeylGroupAutomorphisms::actOn(
  const ElementWeylGroupAutomorphisms& inputElt,
  const Vector<Coefficient>& inputV,
  Vector<Coefficient>& output
) {
  inputElt.checkInitialization();
  inputElt.owner->actOn(inputElt, inputV, output);
}

template<class Coefficient>
void ElementWeylGroupAutomorphisms::actOn(Vector<Coefficient>& inputOutput) const {
  this->checkInitialization();
  this->owner->actOn(*this, inputOutput, inputOutput);
}

template <class Coefficient>
void WeylGroupAutomorphisms::actOn(
  const ElementWeylGroupAutomorphisms& theGroupElement,
  const Vector<Coefficient>& inputVector,
  Vector<Coefficient>& outputVector
) const {
  this->checkInitialization();
  outputVector = inputVector;
  for (int i = theGroupElement.generatorsLastAppliedFirst.size - 1; i >= 0; i --) {
    SimpleReflectionOrOuterAutomorphism& currentGenerator = theGroupElement.generatorsLastAppliedFirst[i];
    if (!currentGenerator.flagIsOuter) {
      this->theWeyl->reflectSimple(currentGenerator.index, outputVector);
    } else {
      if (!this->flagAllOuterAutosComputed) {
        global.fatal << "Weyl group of type " << this->theWeyl->theDynkinType.toString()
        << " does not have its outer autos computed at a place where it should. " << global.fatal;
      }
      this->theOuterAutos.theGenerators[currentGenerator.index].actOnVectorColumn(outputVector, outputVector);
    }
  }
}

template <typename elementFirstGroup, typename elementSecondGroup>
class GroupHomomorphism {
public:
  FiniteGroup<elementFirstGroup>* preimageGroup;
  List<elementSecondGroup> generatorImages;

  elementSecondGroup operator()(const elementFirstGroup& g) const {
    List<int> word;
    this->preimageGroup->getWord(g, word);
    if (word.size == 0) {
      elementSecondGroup out;
      out.makeIdentity(generatorImages[0]);
      return out;
    }
    if (word.size == 1) {
      return generatorImages[word[0]];
    }
    elementSecondGroup h = generatorImages[word[0]];
    for (int i = 1; i < word.size; i ++) {
      h = h * generatorImages[word[i]];
    }
    return h;
  }

  template <typename Coefficient>
  GroupRepresentation<FiniteGroup<elementFirstGroup>, Coefficient> pullbackRepresentation(
    GroupRepresentation<FiniteGroup<elementSecondGroup>, Coefficient> M2
  ) {
    GroupRepresentation<FiniteGroup<elementFirstGroup>, Coefficient> out;
    out.ownerGroup = this->preimageGroup;
    out.identifyingString = M2.identifyingString;
    out.generators.setSize(this->preimageGroup->generators.size);
    for (int i = 0; i < this->preimageGroup->generators.size; i ++) {
      elementSecondGroup ki = (*this)(this->preimageGroup->generators[i]);
      M2.getMatrixOfElement(ki, out.generators[i]);
    }
    return out;
  }
};

template <typename someGroup, typename Coefficient>
class GroupRepresentation {
public:
  someGroup* ownerGroup;
  List<Matrix<Coefficient> > generators;
  mutable ClassFunction<someGroup, Coefficient> theCharacter;
  mutable bool flagCharacterIsComputed;
  std::string identifyingString; // in Python, this would be an anonymous object

  GroupRepresentation() {
    this->ownerGroup = NULL;
    this->flagCharacterIsComputed = false;
  }

  bool checkInitialization() const;

  template <typename elementSomeGroup>
  bool getMatrixOfElement(const elementSomeGroup& g, Matrix<Coefficient>& out);

  void computeCharacter() const;
  ClassFunction<someGroup, Coefficient> getCharacter() {
    this->computeCharacter();
    return this->theCharacter;
  }

  bool operator>(const GroupRepresentation& right) const;

  void makeTensorRepresentation(
    GroupRepresentation<someGroup, Coefficient>& right, GroupRepresentation<someGroup, Coefficient>& left
  );

  bool verifyRepresentation();

  bool verifyRepresentationExpensive() {
    List<Matrix<Coefficient> > repms;
    repms.setSize(this->ownerGroup->theElements.size);
    for (int i = 0; i < this->ownerGroup->theElements.size; i ++) {
      this->getMatrixOfElement(this->ownerGroup->theElements[i], repms[i]);
    }
    for (int i = 0; i < this->ownerGroup->theElements.size; i ++) {
      for (int j = 0; j < this->ownerGroup->theElements.size; j ++) {
        if (repms[i] * repms[j] != repms[this->ownerGroup->theElements.getIndex(
          this->ownerGroup->theElements[i] * this->ownerGroup->theElements[j])]
        ) {
          global.fatal << "Bad representation. " << global.fatal;
        }
      }
    }
    global.comments << "Expensive verification complete, this is indeed a representation\n";
    return true;
  }
  std::string describeAsDirectSum();
  void operator*= (const GroupRepresentation<someGroup, Coefficient>& other);

  JSData toJSON();

  template <typename somestream>
  somestream& intoStream(somestream& out) const;
  std::string toString(FormatExpressions* fmt = nullptr) const;
  friend std::ostream& operator<<(std::ostream& out, GroupRepresentation<someGroup, Coefficient>& data) {
    return data.intoStream(out);
  }
  bool operator==(const GroupRepresentation<someGroup, Coefficient>& right) const {
    if (this->ownerGroup != right.ownerGroup) {
      return false;
    }
    if (this->generators != right.generators) {
      return false;
    }
    return true;
    // if it ever becomes useful to compare element matrices...
    /*for (int i = 0; i < this->ownerGroup.)
    right.getMatrixOfElement(this->ownerGroup.generators[i], M)
    if (M != this->generators[i])
      return false;*/
  }
  GroupRepresentation(const GroupRepresentationCarriesAllMatrices<someGroup, Coefficient>& in) {
    this->generators = in.generators;
    this->theCharacter = in.theCharacter;
    this->flagCharacterIsComputed = in.flagCharacterIsComputed;
  }
  GroupRepresentationCarriesAllMatrices<someGroup, Coefficient> makeGRCAM() const {
    GroupRepresentationCarriesAllMatrices<someGroup, Coefficient> out;
    out.generators = this->generators;
    out.flagCharacterIsComputed = this->flagCharacterIsComputed;
    if (this->identifyingString.size() > 0) {
      out.names.addOnTop(this->identifyingString);
    }
    out.theCharacter = this->theCharacter;
    return out;
  }
  // Note: the group representation types compute the hash value from the character,
  // in which the order of the conjugacy classes determines the value
  // If you compare hash values from two groups which calculated their conjugacy classes in a different way,
  // you must ensure they are in the same order, or hashing won't work as expected.
  unsigned int hashFunction() const;
  static unsigned int hashFunction(const GroupRepresentation<someGroup, Coefficient>& in) {
    return in.hashFunction();
  }
};

template <typename someGroup, typename Coefficient>
template <typename elementSomeGroup>
bool GroupRepresentation<someGroup, Coefficient>::getMatrixOfElement(const elementSomeGroup& g, Matrix<Coefficient>& out) {
  out.makeIdentity(this->generators[0]);
  List<int> word;
  if (!this->ownerGroup->getWord(g, word)) {
    return false;
  }
  for (int i = 0; i < word.size; i ++) {
    out *= this->generators[word[i]];
  }
  return true;
}

template <typename someGroup, typename Coefficient>
bool GroupRepresentation<someGroup, Coefficient>::checkInitialization() const {
  if (this->ownerGroup == 0) {
    global.fatal << "Group not initialized at a place where it should be." << global.fatal;
  }
  return true;
}

template <typename someGroup, typename Coefficient>
void GroupRepresentation<someGroup, Coefficient>::computeCharacter() const {
  if (this->flagCharacterIsComputed) {
    return;
  }
  this->checkInitialization();
  if (!this->ownerGroup->flagCCsComputed) {
    this->ownerGroup->computeConjugacyClassSizesAndRepresentatives();
  }
  this->theCharacter.G = ownerGroup;
  this->theCharacter.data.setSize(this->ownerGroup->conjugacyClasses.size);
  for (int cci = 0; cci < this->ownerGroup->conjugacyClasses.size; cci ++) {
    Matrix<Coefficient> M;
    M.makeIdentity(this->generators[0]);
    List<int> ccirWord;
    // this->getMatrixOfElement(this->ownerGroup->conjugacyClasses[cci].representative);
    this->ownerGroup->getWord(this->ownerGroup->conjugacyClasses[cci].representative, ccirWord);
    for (int i = 0; i < ccirWord.size; i ++) {
      M *= this->generators[ccirWord[i]];
    }
    this->theCharacter.data[cci] = M.getTrace();
  }
  this->theCharacter.G = ownerGroup;
  this->flagCharacterIsComputed = true;
}

// We aim to sort in reverse order within dimensions, according to
// the usual way character tables are displayed.
template <typename someGroup, typename Coefficient>
bool GroupRepresentation<someGroup, Coefficient>::operator>(const GroupRepresentation<someGroup, Coefficient>& right) const {
  if (!this->flagCharacterIsComputed) {
    this->computeCharacter();
  }
  if (!right.flagCharacterIsComputed) {
    right.computeCharacter();
  }
  return this->theCharacter > right.theCharacter;
}

template <typename someGroup, typename Coefficient>
void GroupRepresentation<someGroup, Coefficient>::makeTensorRepresentation(
  GroupRepresentation<someGroup, Coefficient>& right, GroupRepresentation<someGroup, Coefficient>& left
) {
  ownerGroup = right.ownerGroup;
  generators.setSize(right.generators.size);
  for (int i = 0; i < right.generators.size; i ++) {
    generators[i].assignTensorProduct(right.generators[i], left.generators[i]);
  }
}

template <typename someGroup, typename Coefficient>
template <typename somestream>
somestream& GroupRepresentation<someGroup, Coefficient>::intoStream(somestream& out) const {
  if (!this->flagCharacterIsComputed) {
    //this->computeCharacter();
    out << "Representation of the character not computed yet.";
    return out;
  }
  // WeylGroup needs to be printable
  // WeylGroup really needs to be printable lol
  out << "Representation of group " << ownerGroup->toString();
  if (!this->identifyingString.empty()) {
    out << "identified as " << identifyingString;
  }
  out << " with character " << this->theCharacter;
  return out;
}

template <typename someGroup, typename Coefficient>
std::string GroupRepresentation<someGroup, Coefficient>::toString(FormatExpressions* fmt) const {
  (void) fmt;//portable way of avoiding unused parameter warning
  std::stringstream out;
  this->intoStream(out);
  return out.str();
}

template <typename someGroup, typename Coefficient>
JSData GroupRepresentation<someGroup, Coefficient>::toJSON() {
  JSData out;
  out["identifyingString"] = identifyingString;
  if (this->flagCharacterIsComputed) {
    out["character"] = this->theCharacter.data;
  } else {
    out["character"].theType = JSData::token::tokenNull;
  }
  for (int i = 0; i < generators.size; i ++) {
    out["generators"][i] = generators[i];
  }
  return out;
}

template <typename somegroup, typename Coefficient>
class GroupRepresentationCarriesAllMatrices {
  friend std::ostream& operator << (std::ostream& output, const GroupRepresentationCarriesAllMatrices& theIrrep) {
    output << theIrrep.toString();
    return output;
  }
public:
  List<Matrix<Coefficient> > theElementImages;
  List<bool> theElementIsComputed;
  ClassFunction<somegroup, Coefficient> theCharacter;
  List<Matrix<Coefficient> > classFunctionMatrices;
  List<bool> classFunctionMatricesComputed;
  List<Matrix<Coefficient> > generators;
  GroupRepresentationCarriesAllMatrices* parent;
  Vectors<Coefficient> basis;
  Matrix<Coefficient> gramMatrixInverted;
  public:
  bool flagCharacterIsComputed;
  somegroup* ownerGroup;
  List<std::string> names;

  GroupRepresentationCarriesAllMatrices() {
    this->reset();
  }
  GroupRepresentation<somegroup, Coefficient> makeOtherGroupRepresentationClass() const;
  // Note: The group representation types compute the hash value from the character,
  // in which the order of the conjugacy classes determines the value.
  // If you compare hash values from two groups which calculated their conjugacy classes in a different way,
  // you must ensure they are in the same order, or hashing won't work as expected.
  unsigned int hashFunction() const;
  bool checkInitialization() const;
  bool checkAllSimpleGeneratorsAreOK() const;
  static unsigned int hashFunction(const GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>& input) {
    return input.hashFunction();
  }
  void computeAllElementImages();
  const ClassFunction<somegroup, Coefficient>& getCharacter();
  VectorSpace<Coefficient> findBasis() const;
  void multiplyBy(
    const GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>& other,
    GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>& output
  ) const;
  void getLargestDenominatorSimpleGenerators(LargeIntegerUnsigned& outputLCM, LargeIntegerUnsigned& outputDen) const;

  void reset();
  void initialize(somegroup& inputOwner);
  void checkRepresentationIsMultiplicativelyClosed();
  void getClassFunctionMatrix(ClassFunction<somegroup, Coefficient>& inputChar, Matrix<Coefficient>& outputMat);
  void classFunctionMatrix(ClassFunction<somegroup, Coefficient>& inputCF, Matrix<Coefficient>& outputMat);
  int getDimension() const;
  void restrictRepresentation(
    const Vectors<Coefficient>& vectorSpaceBasisSubrep,
    const ClassFunction<somegroup, Rational>& remainingCharacter,
    GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>& output
  );
  bool decomposeTodorsVersionRecursive(
    VirtualRepresentation<somegroup, Coefficient>& outputIrrepMults,
    List<GroupRepresentation<somegroup, Coefficient> >& appendOnlyIrrepList,
    List<GroupRepresentationCarriesAllMatrices<somegroup, Coefficient> >* appendOnlyGRCAMSList = 0
  );
  bool decomposeTodorsVersion(
    VirtualRepresentation<somegroup, Coefficient>& outputIrrepMults,
    List<GroupRepresentationCarriesAllMatrices<somegroup, Coefficient> >* appendOnlyIrrepsList = 0
  );
  List<GroupRepresentationCarriesAllMatrices<somegroup, Coefficient> > decomposeThomasVersion();
  GroupRepresentationCarriesAllMatrices<somegroup, Coefficient> reduced() const;
  Coefficient getNumberOfComponents();
  void operator*= (const GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>& other);
  GroupRepresentationCarriesAllMatrices<somegroup, Coefficient> operator*(
    const GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>& other
  ) const {
    GroupRepresentationCarriesAllMatrices<somegroup, Coefficient> result = *this;
    result *= other;
    return result;
  }
  bool operator==(const GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>& other) const {
    return this->ownerGroup == other.ownerGroup && this->theCharacter == other.theCharacter;
  }
  void spreadVector(const Vector<Coefficient>& input, Vectors<Coefficient>& outputBasisGeneratedSpace);
  std::string getName() const;
  std::string toString(FormatExpressions* theFormat = nullptr) const;
  Matrix<Coefficient>& getMatrixElement(int groupElementIndex);
  template <typename elementSomeGroup>
  void getMatrixElement(const elementSomeGroup& input, Matrix<Coefficient>& output);
  template <typename elementSomeGroup>
  Matrix<Coefficient> getMatrixElement(const elementSomeGroup& input);
  void setElementImage(int elementIndex, const Matrix<Coefficient>& input) {
    this->theElementImages[elementIndex] = input;
    this->theElementIsComputed[elementIndex] = true;
  }
  void setGenerator(int generatorIndex, const Matrix<Coefficient>& input) {
    this->generators[generatorIndex] = input;
  }
  bool operator>(const GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>& other) const;
  //  bool operator<(const GroupRepresentationCarriesAllMatrices<somegroup, Coefficient>& other) const;
};

template <class somegroup, class Coefficient>
class VirtualRepresentation : public LinearCombination<ClassFunction<somegroup, Coefficient>, Rational> {
public:
  void operator*= (const VirtualRepresentation<somegroup, Coefficient>& other);
  void assignRepresentation(const GroupRepresentationCarriesAllMatrices<somegroup, Rational>& other);
  void assignRepresentation(const GroupRepresentation<somegroup, Rational>& other);
  static unsigned int hashFunction(const VirtualRepresentation<somegroup, Coefficient>& input) {
    return input.hashFunction();
  }
  unsigned int hashFunction() const {
    return this->::LinearCombination<ClassFunction<somegroup, Coefficient>, Rational>::hashFunction();
  }
};

// This class iterates over all elements of the orbit of a single element
// using the generators of the group.
// The element can be an element of the group, representation, etc.
// (note that the group elements can be interpreted as elements of
// the group ring representation).
template <class elementGroup, class elementRepresentation>
class OrbitIterator {
public:
  struct GroupActionWithName {
    typedef void (*GroupAction)(
      const elementGroup& actingElement, const elementRepresentation& inputElementActedUpon, elementRepresentation& output
    );
    OrbitIterator::GroupActionWithName::GroupAction actOn;
    std::string name;
    bool checkInitialization() const {
      if (this->actOn == 0) {
        global.fatal << "Non-initialized group action with name: " << this->name << ". " << global.fatal;
      }
      return true;
    }
  };
private:
  HashedList<elementRepresentation> privateLayer1, privateLayer2, privateLayer3;
  elementRepresentation bufferRepresentationElement;
public:
  HashedList<elementRepresentation>* previousLayer;
  HashedList<elementRepresentation>* currentLayer;
  HashedList<elementRepresentation>* nextLayer;
  OrbitIterator::GroupActionWithName theGroupAction;
  List<elementGroup> theGroupGeneratingElements;
  int indexCurrentElement;
  OrbitIterator() {
    this->theGroupAction.actOn = 0;
    this->resetNoActionChange();
  }
  bool checkInitialization() const;
  std::string toStringLayerSize() const;
  void resetNoActionChange() {
    this->previousLayer = &this->privateLayer1;
    this->currentLayer = &this->privateLayer2;
    this->nextLayer = &this->privateLayer3;
    this->indexCurrentElement = - 1;
  }
  const elementRepresentation& getCurrentElement();
  bool incrementReturnFalseIfPastLastFALSE();
  bool incrementReturnFalseIfPastLast();
  void initialize(
    const List<elementGroup>& inputGenerators,
    const elementRepresentation& inputElement,
    const OrbitIterator::GroupActionWithName& inputGroupAction
  );
};

class OrbitIteratorWeylGroup: public OrbitIterator<ElementWeylGroup, ElementWeylGroup> {
};

class OrbitIteratorRootActionWeylGroupAutomorphisms {
  OrbitIterator<ElementWeylGroupAutomorphisms, Vector<Rational> > theIterator;
  List<Vector<Rational> > orbitBuffer;
  Vector<Rational> orbitDefiningElement;
  int maxOrbitBufferSize;
public:
  int orbitSize;
  LargeInteger computedSize;
  int currentIndexInBuffer;
  bool flagOrbitIsBuffered;
  bool flagOrbitEnumeratedOnce;
  OrbitIteratorRootActionWeylGroupAutomorphisms();
  void reset();
  bool incrementReturnFalseIfPastLast();
  const Vector<Rational>& getCurrentElement();
  std::string toString() const;
  std::string toStringSize() const;
  bool checkConsistency();
  void initialize();
  void initialize(const List<ElementWeylGroupAutomorphisms>& inputGenerators,
    const Vector<Rational>& inputElement);
};

template <typename Coefficient>
std::ostream& operator<<(std::ostream& out, const ClassFunction<WeylGroupData, Coefficient>& X);

template <typename elementSomeGroup>
class Coset {
public:
  elementSomeGroup representative;
  List<int> representativeWord;
  List<int> supergroupIndices;
  std::string toString() const;
};

template <typename elementSomeGroup>
std::string Coset<elementSomeGroup>::toString() const {
  std::stringstream out;
  out << "Coset with representative " << representative;
  if (supergroupIndices.size != 0) {
    out << " and " << supergroupIndices.size << " elements";
  }
  return out.str();
}

// The long term plan for this class is to install FiniteGroup<elementSomeGroup>
// as the only someGroup
// also move most of the stuff here to GroupHomomorphism and then include a
// GroupHomomorphism inclusionMap;
template <class someGroup, class elementSomeGroup>
class SubgroupData {
public:
  someGroup *theGroup;
  someGroup *theSubgroup;
  someGroup theSubgroupMayBeHere;
  // one for each supergroup generator.  for word translation.
  List<List<int> > superGeneratorSubWords;
  List<bool> superGeneratorSubWordExists;
  List<int> ccRepresentativesPreimages;
  List<int> generatorPreimages;
  List<Coset<elementSomeGroup> > cosets;
  SubgroupData();
  bool checkInitialization();
  void initialize();
  void makeSubgroupOf(someGroup& G) {
    this->initialize();
    this->theGroup = &G;
    this->theSubgroup = &this->theSubgroupMayBeHere;
  }
  LargeInteger sizeByFormulaOrNegative1() const {
    return - 1;
  }
  void initFromGroupAndGenerators(someGroup& inputGroup, const List<elementSomeGroup>& inputGenerators);
  void makeTranslatableWordsSubgroup(someGroup& inputGroup, const List<elementSomeGroup>& subGenerators);
  void computeCCRepresentativesPreimages();
  void computeCCSizesRepresentativesPreimages() {
    this->computeConjugacyClassSizesAndRepresentatives();
    this->computeCCRepresentativesPreimages();
  }
  // TODO(tmilev): left or right coset?
  // Document, also document what left and right mean.
  bool flagCosetSetsComputed;
  bool flagCosetRepresentativesComputed;
  void computeCosets();
  bool verifyCosets();
  bool verifyNormal();
  std::string toString(FormatExpressions* theFormat);
  bool sameCosetAs(elementSomeGroup& g1, elementSomeGroup& g2);
  int getCosetId(elementSomeGroup& g);
  int qIDMul(int i, int j);
  template <typename Coefficient>
  void quotientGroupPermutationRepresentation(GroupRepresentation<someGroup, Coefficient>& out);
  template <typename Coefficient>
  GroupRepresentation<someGroup, Coefficient> induceRepresentation(GroupRepresentation<someGroup, Coefficient>& in);
  template <typename Coefficient>
  GroupRepresentation<someGroup, Coefficient> induceRepresentationNormalSubgroup(
    GroupRepresentation<someGroup, Coefficient>& in
  );
  void (*cosetRepresentativeEnumerator)(void* H) = nullptr;
  bool (*sameCosetAsByFormula)(void* H, elementSomeGroup& g1, elementSomeGroup& g2) = nullptr;
  //GroupRepresentation<Subgroup<somegroup,elementSomeGroup>, Rational> GetEmptyRationalRepresentationSubgroup();
};

template <typename elementSomeGroup>
bool translatableWordsSubgroupElementGetWord(
  FiniteGroup<elementSomeGroup>& H, const elementSomeGroup& g, List<int>& out
) {
  List<int> superword;
  H.parentRelationship->theGroup->getWord(g, superword);
  out.setSize(0);
  for (int i = 0; i < superword.size; i ++) {
    if (!H.parentRelationship->superGeneratorSubWordExists[superword[i]]) {
      if (!H.hasElement(g)) {
        global.comments << "element g isn't a member of H\n";
      }
      global.fatal << "element " << g << " is assigned parent word "
      << superword.toStringCommaDelimited()
      << " containing generator not found in subgroup " << superword[i]
      << " so if this does belong to the subgroup, we need a better algorithm. "
      << global.fatal;
    }
    out.addListOnTop(H.parentRelationship->superGeneratorSubWords[superword[i]]);
  }
  return true;
}

template <typename someGroup, typename elementSomeGroup>
int SubgroupData<someGroup, elementSomeGroup>::qIDMul(int i, int j) {
  this->computeCosets();
  auto g = this->cosets[i].representative * this->cosets[j].representative;
  return this->getCosetId(g);
}

template <typename someGroup, typename elementSomeGroup>
bool SubgroupData<someGroup, elementSomeGroup>::verifyNormal() {
  Matrix<int> csmt;
  csmt.initialize(cosets.size, cosets.size);
  for (int i = 0; i < cosets.size; i ++) {
    for (int j = 0; j < cosets.size; j ++) {
      csmt(i, j) = qIDMul(i, j);
    }
  }
  global.comments << "Coset multiplication table\n" << csmt.toStringPlainText() << '\n';
  for (int i = 0; i < cosets.size; i ++) {
    for (int j = i; j < cosets.size; j ++) {
      for (int k = j; k < cosets.size; k ++) {
        if (csmt(i, csmt(j, k)) != csmt(csmt(i, j), k)) {
          return false;
        }
      }
    }
  }
  return true;
}

template <typename someGroup, typename elementSomeGroup>
bool SubgroupData<someGroup, elementSomeGroup>::sameCosetAs(elementSomeGroup& g1, elementSomeGroup& g2) {
  if (this->sameCosetAsByFormula) {
    return sameCosetAsByFormula(this, g1, g2);
  }
  this->computeCosets();
  int g1i = this->theGroup->theElements.getIndex(g1);
  int g2i = this->theGroup->theElements.getIndex(g2);
  for (int i = 0; i < this->cosets.size; i ++) {
    if (this->cosets[i].BSContains(g1i)) {
      if (this->cosets[i].BSContains(g2i)) {
        return true;
      }
    }
  }
  return false;
}

template <typename someGroup, typename elementSomeGroup>
int SubgroupData<someGroup, elementSomeGroup>::getCosetId(elementSomeGroup& g) {
  int gi = - 1;
  if (this->cosetRepresentativeEnumerator && this->sameCosetAsByFormula) {
    if (!this->flagCosetRepresentativesComputed) {
      this->cosetRepresentativeEnumerator(this);
    }
  } else {
    if (!flagCosetSetsComputed) {
      this->computeCosets();
    }
    gi = this->theGroup->theElements.getIndex(g);
  }
  for (int i = 0; i < this->cosets.size; i ++) {
    if (this->sameCosetAsByFormula) {
      if (this->sameCosetAsByFormula(this, this->cosets[i].representative, g)) {
        return i;
      }
    } else {
      if (this->cosets[i].supergroupIndices.BSContains(gi)) {
        return i;
      }
    }
  }
  global.fatal << "Error: element " << g << " (possible index " << gi << ") not found in cosets. "
  << "Also some groups are unprintable. " << global.fatal;
  return - 1;
}

template <typename someGroup, typename elementSomeGroup>
template <typename Coefficient>
void SubgroupData<someGroup, elementSomeGroup>::quotientGroupPermutationRepresentation(GroupRepresentation<someGroup, Coefficient>& out) {
  this->computeCosets();
  out.ownerGroup = this->theGroup;
  out.identifyingString = "Quotient Permutation representation";
  global.comments << "Subgroup::quotientGroupPermutationRepresentation: Permuting " << cosets.size << " cosets.\n";
  out.generators.setSize(this->theGroup->generators.size);
  for (int i = 0; i < this->theGroup->generators.size; i ++) {
    out.generators[i].initialize(this->cosets.size, this->cosets.size);
    out.generators[i].makeZero();
    for (int ci = 0; ci <out.generators[i].numberOfColumns; ci ++) {
      elementSomeGroup g =  this->theGroup->generators[i] * this->cosets[ci].representative;
      int j = this->getCosetId(g);
      out.generators[i](j, ci) = 1;
    }
    global.comments << "Element " << this->theGroup->generators[i] << " of coset " << this->getCosetId(this->theGroup->generators[i]);
    global.comments << " permutes the other cosets as\n" << out.generators[i].toStringPlainText() << '\n';
  }
}

// G\otimes_H V
// g(w) = h[g](\otimes v) where [g] rearranges the v's
//
template <typename someGroup, typename elementSomeGroup>
template <typename Coefficient>
GroupRepresentation<someGroup, Coefficient> SubgroupData<someGroup, elementSomeGroup>::induceRepresentationNormalSubgroup(
  GroupRepresentation<someGroup, Coefficient>& in
) {
  GroupRepresentation<someGroup, Coefficient> qr;
  this->quotientGroupPermutationRepresentation(qr);
  GroupRepresentation<someGroup, Coefficient> sr;
  sr.ownerGroup = this->theGroup;
  sr.generators.setSize(this->theGroup->generators.size);
  // in TODO: make random FiniteGroups capable of finding their conjugacy classes in less than at least 5 minutes
  global.comments << "inducing from subgroup representation:\n";
  for (int i = 0; i < this->theSubgroup->generators.size; i ++) {
    global.comments << this->theSubgroup->generators[i] << '\n' << in.generators[i].toStringPlainText() << '\n';
  }
  for (int i = 0; i < this->theGroup->generators.size; i ++) {
    elementSomeGroup g;
    int csi = this->getCosetId(this->theGroup->generators[i]);
    if (csi == 0) {
      g = this->theGroup->generators[i];
    } else {
      elementSomeGroup cg = cosets[csi].representative;
      cg.invert();
      g = cg * this->theGroup->generators[i];
    }
    global.comments << "element " << this->theGroup->generators[i] << " belongs to coset " << csi;
    global.comments << " represented by " << cosets[csi].representative << " and corresponds to subgroup element " << g;
    in.getMatrixOfElement(g, sr.generators[i]);
    global.comments << " which is assigned matrix\n" << sr.generators[i].toStringPlainText() << '\n';
  }
  GroupRepresentation<someGroup, Coefficient> out;
  out.makeTensorRepresentation(qr,sr);
  global.comments << "Subgroup representation: " << sr.toString() << "\n";
  for (int i = 0; i < this->theGroup->generators.size; i ++) {
    global.comments << this->theGroup->generators[i] << ' ' << sr.generators[i].getTrace() << '\n'
    << sr.generators[i].toStringPlainText() << '\n';
  }
  global.comments << "Quotient representation: " << qr.toString() << "\n";
  for (int i = 0; i < this->theGroup->generators.size; i ++) {
    global.comments << this->theGroup->generators[i] << ' ' << qr.generators[i].getTrace() << '\n'
    << qr.generators[i].toStringPlainText() << '\n';
  }
  global.comments << "Induced representation: " << out.toString() << '\n';
  for (int i = 0; i <out.generators.size; i ++) {
    global.comments << this->theGroup->generators[i] << ' ' << out.generators[i].getTrace() << '\n'
    << out.generators[i].toStringPlainText() << '\n';
  }
  return out;
}

template <typename someGroup, typename elementSomeGroup>
template <typename Coefficient>
GroupRepresentation<someGroup, Coefficient> SubgroupData<someGroup, elementSomeGroup>::induceRepresentation(
  GroupRepresentation<someGroup, Coefficient>& in
) {
  GroupRepresentation<someGroup, Coefficient> out;
  out.generators.setSize(this->theGroup->generators.size);
  for (int i = 0; i < this->theGroup->generators.size; i ++) {
    this->computeCosets();
    // parent->generators[i] = cg * h
    // g1 * g2 = cg1 * h1 * cg2 * h2
    elementSomeGroup g = this->theGroup->generators[i];

    if (in.generators.size == 0) {
      out.generators[i].makeZeroMatrix(1 * this->cosets.size);
    } else {
      out.generators[i].makeZeroMatrix(in.generators[0].numberOfRows * this->cosets.size);
    }
    for (int ci = 0; ci < cosets.size; ci ++) {
      elementSomeGroup k = g * cosets[ci].representative;
      int kcsi = this->getCosetId(k);
      elementSomeGroup ck = cosets[kcsi].representative;
      elementSomeGroup cki = ck;
      cki.invert();
      elementSomeGroup hk = cki * k;
      Matrix<Coefficient> ikblock;
      in.getMatrixOfElement(hk, ikblock);
      out.generators[i].assignBlock(ikblock, kcsi * ikblock.numberOfRows, ci * ikblock.numberOfColumns);
    }
  }
  out.ownerGroup = this->theGroup;
  for (int i = 0; i < out.generators.size; i ++) {
    global.comments << this->theGroup->generators[i] << ' ' << out.generators[i].getTrace() << '\n'
    << out.generators[i].toStringPlainText() << '\n';
  }
  if (!out.verifyRepresentation()) {
    if (!in.verifyRepresentationExpensive()) {
      global.comments << "Well, we weren't given a proper representation either.";
    }
    FiniteGroup<Matrix<Coefficient> > ingroup;
    ingroup.generators = in.generators;
    global.comments << "Generator commutation relations for input representation:\n"
    << ingroup.prettyPrintGeneratorCommutationRelations();
    global.comments << "a quotient group of\n" << this->theSubgroup->prettyPrintGeneratorCommutationRelations();
    FiniteGroup<Matrix<Coefficient> > outgroup;
    outgroup.generators = out.generators;
    global.comments << "Generator commutation relations for 'representation':\n"
    << outgroup.prettyPrintGeneratorCommutationRelations();
    global.comments << "It was supposed to be a quotient group of\n" << this->theGroup->prettyPrintGeneratorCommutationRelations();
    global.fatal << "Error in induceRepresentation. " << global.fatal;
  }
  return out;
}

// This class represents any subgroup of a Weyl group.
// The WeylGroupData is of course shared between the group and the subgroup,
// while the SubgroupData is the standard object to represent
// a FiniteGroup being a subgroup of another FiniteGroup.
// The only other thing in this is a tau signature, because this class
// was created to facilitate computations of tau signatures
class SubgroupDataWeylGroup {
public:
  WeylGroupData* theWeylData;
  SubgroupData<FiniteGroup<ElementWeylGroup>, ElementWeylGroup> theSubgroupData;
  List<Rational> tauSignature;
  bool checkInitialization();
  void ComputeTauSignature();
  void getSignCharacter(Vector<Rational>& out);
  SubgroupDataWeylGroup();
  std::string toString(FormatExpressions* theFormat = nullptr);
};

class SubgroupDataRootReflections : public SubgroupDataWeylGroup {
public:
  SubgroupData<FiniteGroup<ElementWeylGroup>, ElementWeylGroup> subGroupDataContainer;
  Matrix<Rational> SubCartanSymmetric;
  DynkinType theDynkinType;
  Vectors<Rational> generatingSimpleRoots;
  bool flagIsParabolic;
  bool flagIsExtendedParabolic;
  Selection simpleRootsInLeviParabolic;
  void initializeGenerators();
  void makeParabolicSubgroup(WeylGroupData& G, const Selection& inputGeneratingSimpleRoots);
  void makeFromRoots(WeylGroupData& G, const Vectors<Rational>& inputRootReflections);
  LargeInteger sizeByFormulaOrNegative1() const {
    return this->theDynkinType.getWeylGroupSizeByFormula();
  }
  void computeDynkinType();
  void computeCCSizesRepresentativesPreimages();
  SubgroupDataRootReflections();
  std::string toString(FormatExpressions* theFormat = nullptr);
};

class SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms {
public:
  bool truncated;
  bool flagDeallocated;
  WeylGroupData* ambientWeyl;
  HashedList<ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms> allElements;
  // WeylGroupData ElementS;
  List<ElementWeylGroup> RepresentativesQuotientAmbientOrder;
  Vectors<Rational> simpleRootsInner;
  // Each element of the group is a list of inner and outer generators.
  // The inner generators are reflections with
  // respect to the fixed set of roots contained in this->simpleRootsInner.
  // The outer generators are linear operators mapping
  // the vector space of the root system onto itself.
  // Here, outer automorphism are recorded using type Vectors<Rational>.
  // Here, the i^th automorphism entry gives the image
  // of the i^th simple inner root.
  List<Vectors<Rational> > ExternalAutomorphisms;
  HashedList<Vector<Rational> > RootSubsystem;
  Vectors<Rational> RootsOfBorel;
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms();
  ~SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms();
  bool checkInitialization();
  void toString(std::string& output, bool displayElements);
  void getGroupElementsIndexedAsAmbientGroup(List<ElementWeylGroup>& output);
  std::string toStringBruhatGraph();
  std::string toStringCosetGraph();
  std::string toStringFromLayersAndArrows(
    List<List<List<int> > >& arrows, List<List<int> >& Layers, int GraphWidth, bool useAmbientIndices
  );
  std::string toString(bool displayElements = true) {
    std::string tempS;
    this->toString(tempS, displayElements);
    return tempS;
  }
  Vector<Rational> getRho();
  template <class Coefficient>
  void raiseToDominantWeightInner(
    Vector<Coefficient>& theWeight, int* sign = nullptr, bool* stabilizerFound = nullptr
  );
  template <class Coefficient>
  bool freudenthalFormulaIrrepIsWRTLeviPart(
    const Vector<Coefficient>& inputHWfundamentalCoords,
    HashedList<Vector<Coefficient> >& outputDominantWeightsSimpleCoords,
    List<Coefficient>& outputMultsSimpleCoordS,
    std::string& outputDetails,
    int UpperBoundFreudenthal
  );
  bool makeParabolicFromSelectionSimpleRoots(
    WeylGroupData& inputWeyl,
    const Selection& ZeroesMeanSimpleRootSpaceIsInParabolic,
    int upperLimitNumberOfElements
  );
  void makeParabolicFromSelectionSimpleRoots(
    WeylGroupData& inputWeyl,
    const Vector<Rational>& ZeroesMeanSimpleRootSpaceIsInParabolic,
    int upperLimitNumberOfElements
  );
  bool getAllDominantWeightsHWFDIMwithRespectToAmbientAlgebra(
    Vector<Rational>& highestWeightSimpleCoords,
    HashedList<Vector<Rational> >& outputWeightsSimpleCoords,
    int upperBoundDominantWeights,
    std::string& outputDetails
  );
  template <class Coefficient>
  bool getAllDominantWeightsHWFDIM(
    Vector<Coefficient>& highestWeightSimpleCoords,
    HashedList<Vector<Coefficient> >& outputWeightsSimpleCoords,
    int upperBoundDominantWeights,
    std::string& outputDetails
  );
  bool drawContour(
    const Vector<Rational>& highestWeightSimpleCoord,
    DrawingVariables& theDV,
    const std::string& color,
    int UpperBoundVertices
  );
  // The body of this function must
  // appear after the definitions of isDominantWithRespectToGenerator.
  template <class Coefficient>
  bool isDominantWeight(const Vector<Coefficient>& theWeight);
  template <class Coefficient>
  bool isDominantWithRespectToGenerator(const Vector<Coefficient>& theWeight, int generatorIndex);
  template <class Coefficient>
  Coefficient weylDimensionFormulaInnerSimpleCoords(
    const Vector<Coefficient>& theWeightInnerSimpleCoords,
    const Coefficient& ringUnit = Coefficient::one()
  );
  void findQuotientRepresentatives(int UpperLimit);
  void getMatrixOfElement(
    const ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms& input,
    Matrix<Rational>& outputMatrix
  ) const;
  template <class Coefficient>
  bool generateOrbitReturnFalseIfTruncated(
    const Vector<Coefficient>& input, Vectors<Coefficient>& outputOrbit, bool restrictToInner, int upperLimitNumberOfElements
  );
  bool computeSubGroupFromGeneratingReflections(
    Vectors<Rational>* inputRoots,
    List<Vectors<Rational> >* inputExternalAutos,
    int upperLimitNumberOfElements,
    bool recomputeAmbientRho
  );
  void computeRootSubsystem();
  template <class Coefficient>
  void actByNonSimpleElement(int index, Vector<Coefficient>& inputOutput) const;
  template <class Coefficient>
  void actByNonSimpleElement(int index, Vector<Coefficient>& input, Vector<Coefficient>& output) const;
  template <class Coefficient>
  void actByElement(
    const ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms& theElement,
    const Vector<Coefficient>& input,
    Vector<Coefficient>& output
  ) const;
  template <class Coefficient>
  void actByElement(
    const ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms& theElement,
    const Vectors<Coefficient>& input,
    Vectors<Coefficient>& output
  ) const;
  template <class Coefficient>
  void actByNonSimpleElement(int index, const Vectors<Coefficient>& input, Vectors<Coefficient>& output) const {
    this->actByElement(this->allElements[index], input, output);
  }
};

template <typename Coefficient>
class ElementWeylGroupRing : public ElementMonomialAlgebra<ElementWeylGroup, Coefficient> {
public:
  void makeEi(WeylGroupData* G, int i);
  void makeFromClassFunction(WeylGroupData* G, const List<Coefficient>& l);
  void makeFromClassFunction(const ClassFunction<WeylGroupData, Coefficient>& l);
};

template <typename Coefficient>
std::ostream& operator<<(std::ostream& out, const ElementWeylGroupRing<Coefficient> g);

template <typename Coefficient>
void ElementWeylGroupRing<Coefficient>::makeEi(WeylGroupData* GG, int i) {
  ElementWeylGroup theMon;
  theMon = GG->theGroup.theElements[i];
  *this = theMon;
}

template <typename Coefficient>
void ElementWeylGroupRing<Coefficient>::makeFromClassFunction(const ClassFunction<WeylGroupData, Coefficient>& l) {
  makeFromClassFunction(l.G, l.data);
}

template <typename Coefficient>
void ElementWeylGroupRing<Coefficient>::makeFromClassFunction(WeylGroupData* GG, const List<Coefficient>& l) {
  MacroRegisterFunctionWithName("ElementWeylGroupRing::makeFromClassFunction");
  if (GG == nullptr) {
    global.fatal << "Weyl group pointer not allowed to be zero. " << global.fatal;
  }
  this->makeZero();
  ElementWeylGroup theMon;
  for (int i = 0; i < GG->theGroup.conjugacyClassCount(); i ++) {
    if (l[i] != 0) {
      for (int j = 0; j < GG->theGroup.conjugacyClasses.size; j ++) {
        this->addMonomial(GG->theGroup.conjugacyClasses[i].theElements[j], l[i]);
      }
    }
  }
}

//Matrix<Element>
template <typename Coefficient>
std::ostream& operator<<(std::ostream& out, const ElementWeylGroupRing<Coefficient> g) {
  out << g.data;
  return out;
}

//--------------------------------Finite group characters----------------------------

template<class someFiniteGroup, typename Coefficient>
Coefficient ClassFunction<someFiniteGroup, Coefficient>::innerProduct(const ClassFunction<someFiniteGroup, Coefficient>& other) const {
  return this->G->getHermitianProduct(this->data, other.data);
}

template<class someFiniteGroup, typename Coefficient>
Coefficient ClassFunction<someFiniteGroup, Coefficient>::norm() const {
  return this->innerProduct(*this);
}

template<class someFiniteGroup, typename Coefficient>
ClassFunction<someFiniteGroup, Coefficient> ClassFunction<someFiniteGroup, Coefficient>::operator*(const ClassFunction<someFiniteGroup, Coefficient>& other) const {
  ClassFunction<someFiniteGroup, Coefficient> result = *this;
  result *= other;
  return result;
}

template<class someFiniteGroup, typename Coefficient>
void ClassFunction<someFiniteGroup, Coefficient>::operator*=(const ClassFunction<someFiniteGroup, Coefficient>& right) {
  if (this->G != right.G) {
    global.fatal << "Attempt to multiply class functions belonging to different groups." << global.fatal;
  }
  for (int i = 0; i < this->data.size; i ++) {
    this->data[i] *= right[i];
  }
}

template<class someFiniteGroup, typename Coefficient>
ClassFunction<someFiniteGroup, Coefficient> ClassFunction<someFiniteGroup, Coefficient>::symmetricSecondPower() const {
  ClassFunction<someFiniteGroup, Coefficient> l;
  l.G = G;
  l.data.setExpectedSize(G->conjugacyClassCount());
  for (int i = 0; i < G->conjugacyClassCount(); i ++) {
    l.data.addOnTop((this->data[i] * this->data[i] + this->data[G->squaresFirstConjugacyClassRep[i]]) / 2);
  }
  return l;
}

template<class someFiniteGroup, typename Coefficient>
ClassFunction<someFiniteGroup, Coefficient> ClassFunction<someFiniteGroup, Coefficient>::alternateSecondPower() const {
  ClassFunction<someFiniteGroup, Coefficient> l;
  l.G = G;
  l.data.setExpectedSize(G->conjugacyClassCount());
  for (int i = 0; i < G->conjugacyClassCount(); i ++) {
    l.data.addOnTop((this->data[i] * this->data[i] - this->data[G->squaresFirstConjugacyClassRep[i]]) / 2);
  }
  return l;
}

template<class someFiniteGroup, typename Coefficient>
ClassFunction<someFiniteGroup, Coefficient> ClassFunction<someFiniteGroup, Coefficient>::operator+(
  const ClassFunction<someFiniteGroup, Coefficient>& other
) const {
  ClassFunction<someFiniteGroup, Coefficient> l = *this;
  l.data += other.data;
  return l;
}

template<class someFiniteGroup, typename Coefficient>
ClassFunction<someFiniteGroup, Coefficient> ClassFunction<someFiniteGroup, Coefficient>::operator-(
  const ClassFunction& other
) const {
  ClassFunction<someFiniteGroup, Coefficient> l = *this;
  l.data += other.data;
  return l;
}

template<class someFiniteGroup, typename Coefficient>
ClassFunction<someFiniteGroup, Coefficient> ClassFunction<someFiniteGroup, Coefficient>::reducedWithCharacters(
  const List<ClassFunction<someFiniteGroup, Coefficient> >& cchars
) {
  ClassFunction<someFiniteGroup, Coefficient> X = *this;
  if (X.norm() == 0) {
    return X;
  }
  List<ClassFunction<someFiniteGroup, Coefficient> > chars;
  if (cchars == 0) {
    chars = this->G->characterTable;
  } else {
    chars = cchars;
  }
  bool outerChanged = false;
  bool innerChanged = false;
  ClassFunction X2;
  do {
    for (int i = 0; i < chars.size; i ++) {
      do {
        innerChanged = false;
        X2 = X - chars[i];
        // No virtual characters allowed
        Coefficient n = X2[0];
        if (n < 0) {
          continue;
        }
        bool bad = false;
        for (int iii = 0; iii < X2.data.size; iii ++) {
          if (n < X2[iii] || n < - X2[iii]) {
            bad = true;
          }
        }
        if (bad) {
          continue;
        }
        if (X2.norm() < X.norm()) {
          X = X2;
          if (X.norm() == 0) {
            return X;
          }
          innerChanged = true;
        }
      } while (innerChanged);
    }
  } while (outerChanged);
  return X;
}

// Univariate dense polynomials.
template <typename Coefficient>
class UDPolynomial {
public:
   // "So the last shall be first, and the first last" -- Matthew 20:12
  List<Coefficient> data;
  UDPolynomial() {
  }
  UDPolynomial(const UDPolynomial<Coefficient>& other) {
    this->data = other.data;
  }
  UDPolynomial& operator=(const UDPolynomial<Coefficient>& other) {
    this->data = other.data;
    return *this;
  }
  // UDPolynomial<Coefficient> operator+(const UDPolynomial<Coefficient>& right) const;
  void operator+=(const UDPolynomial<Coefficient>& right);
  // UDPolynomial<Coefficient> operator-(const UDPolynomial<Coefficient>& right) const;
  void operator-=(const UDPolynomial<Coefficient>& right);
  UDPolynomial<Coefficient> operator*(const UDPolynomial<Coefficient>& right) const;
  //  UDPolynomial<Coefficient> operator*(const Coefficient& right) const;
  void operator*=(const Coefficient& right);
  void operator*=(const UDPolynomial<Coefficient>& other) {
    *this = (*this) * other;
  }

  UDPolynomial<Coefficient> timesXn(int n) const;
  // Quick divisibility test
  // bool DivisibleBy(const UDPolynomial<Coefficient>& divisor) const;
  struct DivisionResult<UDPolynomial<Coefficient> > divideBy(const UDPolynomial<Coefficient>& right) const;
  UDPolynomial<Coefficient> operator/(const UDPolynomial<Coefficient>& divisor) const;
  UDPolynomial<Coefficient> operator%(const UDPolynomial<Coefficient>& divisor) const;
  void operator/=(const Coefficient& right);
  void operator/=(const UDPolynomial<Coefficient>& right) {
    *this = *this / right;
  }
  Coefficient operator()(const Coefficient& x) const;
  void clearDenominators();
  void formalDerivative();
  void squareFree();
  Coefficient& operator[](int i) const;
  bool operator<(const UDPolynomial<Coefficient>& right) const;
  bool operator==(int other) const;
  std::string toString(FormatExpressions* theFormat = nullptr) const;
  void assignMinimalPolynomial(const Matrix<Coefficient>& input);
  void assignCharacteristicPolynomial(const Matrix<Coefficient>& input); // method due to Urbain Le Verrier
};

template <typename Coefficient>
void UDPolynomial<Coefficient>::assignMinimalPolynomial(const Matrix<Coefficient>& input) {
  int n = input.numberOfColumns;
  this->data.setSize(1);
  this->data[0] = 1;
  for (int col = 0; col < n; col ++) {
    VectorSpace<Coefficient> vs;
    Vector<Coefficient> v, w;
    v.makeEi(n, col);
    vs.addVectorToBasis(v);
    for (int i = 0; i < n; i ++) {
      w = input * v;
      if (!vs.addVectorToBasis(w)) {
        break;
      }
      v = w;
    }
    Vector<Coefficient> p = vs.basis.putInBasis(w);
    UDPolynomial<Coefficient> out;
    out.data.setSize(p.size + 1);
    for (int i = 0; i <p.size; i ++) {
      out.data[i] = - p[i];
    }
    out.data[p.size] = 1;
    *this = MathRoutines::leastCommonMultiple(*this, out);
  }
}

template <typename Coefficient>
void UDPolynomial<Coefficient>::assignCharacteristicPolynomial(const Matrix<Coefficient>& input) {
  int n = input.numberOfColumns;
  this->data.setSize(n + 1);
  this->data[0] = 1;
  Matrix<Coefficient> acc;
  acc = input;
  for (int i = 1; i < n; i ++) {
    this->data[i] = - acc.getTrace() / i;
    for (int j = 0; j < n; j ++) {
      acc.elements[j][j] += this->data[i];
    }
    acc.multiplyOnTheLeft(input);
  }
  this->data[n] = - acc.getTrace() / n;
}

template <typename Coefficient>
Coefficient& UDPolynomial<Coefficient>::operator[](int i) const {
  return data[i];
}

template <typename Coefficient>
Coefficient UDPolynomial<Coefficient>::operator()(const Coefficient& x) const {
  Coefficient acc = 0;
  Coefficient y = 1;
  for (int i = 0; i < data.size; i ++) {
    acc += y * data[i];
    y *= x;
  }
  return acc;
}

template <typename Coefficient>
void UDPolynomial<Coefficient>::operator+=(const UDPolynomial<Coefficient>& right) {
  int t = min(right.data.size, data.size);
  for (int i = 0; i < t; i ++) {
    data[i] += right.data[i];
  }
  if (right.data.size > data.size) {
    int n = data.size;
    data.setSize(right.data.size);
    for (int i = n; i < right.data.size; i ++) {
      data[i] = right.data[i];
    }
  } else {
    while ((data.size != 0) and (data[data.size - 1] != 0)) {
      data.size --;
    }
  }
}

template <typename Coefficient>
void UDPolynomial<Coefficient>::operator-=(const UDPolynomial<Coefficient>& right) {
  int t = right.data.size;
  if (data.size < t) {
    t = data.size;
  }

  for (int i = 0; i < t; i ++) {
    data[i] -= right.data[i];
  }
  if (right.data.size > data.size) {
    int n = data.size;
    data.setSize(right.data.size);
    for (int i = n; i < right.data.size; i ++) {
      data[i] = - right.data[i];
    }
  } else {
    while ((data.size != 0) and (data[data.size - 1] == 0)) {
      data.size --;
    }
  }
}

template <typename Coefficient>
UDPolynomial<Coefficient> UDPolynomial<Coefficient>::operator*(const UDPolynomial<Coefficient>& right) const {
  UDPolynomial<Coefficient> out;
  out.data.setSize(data.size + right.data.size - 1);
  for (int i = 0; i <out.data.size; i ++) {
    out.data[i] = 0;
  }
  for (int i = 0; i < data.size; i ++) {
    for (int j = 0; j < right.data.size; j ++) {
      out.data[i + j] += data[i] * right.data[j];
    }
  }
  return out;
}

template <typename Coefficient>
UDPolynomial<Coefficient> UDPolynomial<Coefficient>::timesXn(int n) const {
  UDPolynomial<Coefficient> out;
  out.data.setSize(data.size + n);
  for (int i = 0; i < n; i ++) {
    out.data[i] = 0;
  }
   // not memcpy()
  for (int i = 0; i < data.size; i ++) {
    out.data[i + n] = data[i];
  }
  return out;
}

template <typename Coefficient>
void UDPolynomial<Coefficient>::operator*=(const Coefficient& right) {
  for (int i = 0; i < data.size; i ++) {
    data[i] *= right;
  }
}

template <class Coefficient>
std::string UDPolynomial<Coefficient>::toString(FormatExpressions* theFormat) const {
  Polynomial<Coefficient> tempP;
  tempP.makeZero();
  MonomialP tempM;
  for (int i = 0; i < this->data.size; i ++) {
    tempM.makeEi(0, i, 1);
    tempP.addMonomial(tempM, this->data[i]);
  }
  return tempP.toString(theFormat);
}

template <typename Coefficient>
std::ostream& operator<<(std::ostream& out, const UDPolynomial<Coefficient>& p) {
  FormatExpressions tempFormat;
  tempFormat.polynomialAlphabet.setSize(1);
  tempFormat.polynomialAlphabet[0] = "q";
  return out << p.toString(&tempFormat);
}

template <typename elementSomeGroup>
int FiniteGroup<elementSomeGroup>::multiplyElements(int indexLeft, int indexRight) const {
  return this->theElements.getIndex(this->theElements[indexLeft] * this->theElements[indexRight]);
}

template <typename elementSomeGroup>
int FiniteGroup<elementSomeGroup>::invert(int g) const {
  return this->theElements.getIndex(this->theElements[g].inverse());
}

template <class elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::registerConjugacyClass(
  const elementSomeGroup& theRepresentative, bool dontAddIfSameInvariants
) {
  MacroRegisterFunctionWithName("FiniteGroup::registerConjugacyClass");
  ConjugacyClass theClass;
  theClass.representative = theRepresentative;
  Polynomial<Rational> theCharPoly;
  theClass.representative.getCharacteristicPolynomialStandardRepresentation(theCharPoly);
  if (this->characterPolynomialsConjugacyClassesStandardRepresentation.contains(theCharPoly)) {
    const List<int>& indicesPossibleConjugates =
    this->characterPolynomialsConjugacyClassesStandardRepresentation.getHashArray(
      this->characterPolynomialsConjugacyClassesStandardRepresentation.getHash(theCharPoly)
    );
    for (int i = 0; i < indicesPossibleConjugates.size; i ++) {
      elementSomeGroup& otherRepresentative = this->conjugacyClasses[indicesPossibleConjugates[i]].representative;
      if (!dontAddIfSameInvariants) {
        if (this->areConjugate(theClass.representative, otherRepresentative)) {
          return false;
        }
      } else {
        if (!theClass.representative.hasDifferentConjugacyInvariantsFrom(otherRepresentative)) {
          return false;
        }
      }
    }
  }
  theClass.flagRepresentativeComputed = true;
  this->computeCCSizeOrCCFromRepresentative(theClass, false);
  this->conjugacyClasses.addOnTop(theClass);
  this->characterPolynomialsConjugacyClassesStandardRepresentation.addOnTop(theCharPoly);
  this->sizePrivate += theClass.size;
  this->conjugacyClasses.quickSortAscending();
  return true;
}

template <typename elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::getWord(const elementSomeGroup& g, List<int>& word) {
  if (this->getWordByFormula != nullptr) {
    return this->getWordByFormula(*this, g, word);
  }
  if (!this->flagWordsComputed) {
    this->computeAllElementsLargeGroup(true);
  }
  int index = this->theElements.getIndex(g);
  if (index == - 1) {
    return false;
  }
  word = this->theWords[index];
  return true;
}

template <class elementSomeGroup>
LargeInteger FiniteGroup<elementSomeGroup>::getSize() {
  this->checkConsistency();
  if (this->sizePrivate > 0) {
    return sizePrivate;
  }
  if (this->getSizeByFormula != 0) {
    this->sizePrivate = this->getSizeByFormula(*this);
    return sizePrivate;
  }
  this->computeAllElements(false);
  return this->sizePrivate;
}

template <typename elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::checkInitializationConjugacyClasses() const {
  if (this->conjugacyClassCount() == 0) {
    global.fatal << "Request to "
    << "compute character hermitian product in a group whose "
    << "conjugacy classes and/or elements have not been computed. "
    << "The group reports to have "
    << this->conjugacyClassCount() << " conjugacy classes and "
    << this->theElements.size << " elements. "
    << global.fatal;
    return false;
  }
  for (int i = 0; i < this->irreps.size; i ++) {
    if (this->irreps[i].theCharacter.data.isEqualToZero()) {
      global.fatal << "Irrep number "
      << i + 1 << " has zero character!" << global.fatal;
      return false;
    }
  }
  return true;
}

#endif
