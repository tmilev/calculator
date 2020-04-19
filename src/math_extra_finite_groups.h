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
template <typename someGroup, typename coefficient>
class GroupRepresentation;

template <typename someGroup, typename coefficient>
class GroupRepresentationCarriesAllMatrices;

template <typename someGroup, typename elementSomeGroup>
class SubgroupData;

template <typename ElementEuclideanDomain>
class DivisionResult {
public:
  ElementEuclideanDomain quotient;
  ElementEuclideanDomain remainder;
};

template<class someFiniteGroup, typename coefficient>
class ClassFunction {
public:
  someFiniteGroup* G;
  Vector<coefficient> data;
  ClassFunction(): G(nullptr) {
  }
  void makeZero(someFiniteGroup& inputWeyl);
  bool IsEqualToZero() const {
    return this->data.IsEqualToZero();
  }
  coefficient InnerProduct(const ClassFunction& other) const;
  coefficient Norm() const;
  ClassFunction operator*(const ClassFunction& other) const;
  ClassFunction Sym2() const;
  ClassFunction Alt2() const;
  ClassFunction operator+(const ClassFunction& other) const;
  ClassFunction operator-(const ClassFunction& other) const;
  ClassFunction ReducedWithChars(const List<ClassFunction>& chars);
  coefficient& operator[](int i) const;
  std::string toString(FormatExpressions* theFormat = nullptr) const;
  static unsigned int hashFunction(const ClassFunction& input);
  unsigned int hashFunction() const {
    return this->hashFunction(*this);
  }
  void operator*=(const coefficient& inputCF) {
    this->data *= inputCF;
  }
  ClassFunction operator*(const coefficient& inputCF) const {
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
  friend std::ostream& operator<<(std::ostream& out, const ClassFunction<someFiniteGroup, coefficient>& data) {
    out << data.toString();
    return out;
  }
};

// To make a FiniteGroup, define an element class with the following methods
// elementSomeGroup operator*(const elementSomeGroup& right) const;
// void Invert();
// elementSomeGroup operator^(const elementSomeGroup& conjugateBy) const;
// static void ConjugationAction(const elementSomeGroup& conjugator,
//                               const elementSomeGroup& conjugated,
//                                     elementSomeGroup& out);
// bool HasDifferentConjugacyInvariantsFrom(const elementSomeGroup& other) const;
// //the purpose of this is to test if it is possible for two elements to be conjugate
// //return true if there is an easy way to tell if they are not conjugate
// //return false otherwise
// void GetCharacteristicPolyStandardRepresentation(Polynomial<Rational>& out) const;
// bool operator== (const elementSomeGroup& right) const;
// bool operator>(const elementSomeGroup& right) const;
// //group elements are not in general comparable, but do something consistent
// //and reasonable for collating and printing purposes
// unsigned int hashFunction() const;
//
// To make certain operations go faster, it might be nice to put smarter algorithms
// in the function pointers in the FiniteGroup class
//
// assumptions on the FiniteGroup:
// 1. The finite group has a small number of conjugacy classes. Z/10000000Z is not OK
//
// An uninitialized element is expected to be the identity element to the full
// extent allowed by law.  Two uninitialized elements can be expected to multiply,
// an an uninitialized element can multiply by an initialized element and where
// needed thereby discover what group it belongs to.  Equality and sorting operators
// and the hashFunction need to know what the identity is representation
// It should hash to 0 and sort first.
//
// Elements generally know of a faithful representation, which may by reducible,
// especially over a group that isn't their entire group they want to belong to.
// That is the representation they give the characteristic polynomial for.
// At present, however, ElementZ2N gives the charpoly for one of its one-dimensional
// representations
// also that particual API is kinda dumb and likely shouldn't exist.

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
  // Currently, 'occasionally useful' means the GetWordByFormula method of subgroups
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
      (void) theFormat;//avoid unused parameter warning, portable
      std::stringstream out;
      out << "Conj. class size: " << this->size.toString();
      return out.str();
    }
    bool operator>(const ConjugacyClass& other) const {
      return this->representative>other.representative;
    }
  };
  List<ConjugacyClass> conjugacyClasseS;
  HashedList<Polynomial<Rational> > CCsStandardRepCharPolys;

  // All of these lists will be expected to be sorted at all times, so please
  // insert using .BSInsertDontDup() if you for some reason can't use the friendly
  // insertion methods provided
  // The reason to use the provided methods is that they ensure that the characterTable and irreps
  // are up to date if you insert a GRCAM.  That's pretty much it.
  List<GroupRepresentation<FiniteGroup<elementSomeGroup>, Rational> > irreps;
  List<GroupRepresentationCarriesAllMatrices<FiniteGroup<elementSomeGroup>, Rational> > irreps_grcam;
  List<ClassFunction<FiniteGroup<elementSomeGroup>, Rational> > characterTable;

  void AddIrreducibleRepresentation(GroupRepresentation<FiniteGroup<elementSomeGroup>, Rational>& r);
  void AddIrreducibleRepresentation(GroupRepresentationCarriesAllMatrices<FiniteGroup<elementSomeGroup>, Rational>& r);
  void AddCharacter(const ClassFunction<FiniteGroup<elementSomeGroup>, Rational>& X);


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
    this->init();
  }
  ~FiniteGroup() {
    this->flagDeallocated = true;
  }
  void InitGenerators() {
  }
  bool CheckConsistency() const {
    if (this->flagDeallocated) {
      global.fatal << "This is a programming error: use after free of Finite group. " << global.fatal;
    }
    return true;
  }
  bool CheckInitialization() const;
  // WARNING:  This recalculates conjugacy classes, screwing up their order for other methods
  bool CheckConjugacyClassRepsMatchCCsizes();
  bool CheckOrthogonalityCharTable();
  void SetSizE(const LargeInteger& inputSize) {
    this->sizePrivate = inputSize;
  }
  void init();
  std::string toString(FormatExpressions* theFormat = nullptr){
    std::stringstream out;
    out << this->ToStringElements(theFormat);
    if (this->flagCCRepresentativesComputed) {
      out << this->ToStringConjugacyClasses(theFormat);
    }
    return out.str();
  }
  std::string ToStringElements(FormatExpressions* theFormat = nullptr) const;
  std::string ToStringConjugacyClasses(FormatExpressions* theFormat = nullptr);
  int ConjugacyClassCount() const;
  LargeInteger GetSize();
  LargeInteger SizeByFormulaOrNeg1() {
    this->CheckConsistency();
    if (this->GetSizeByFormula != 0) {
      return GetSizeByFormula(*this);
    }
    return - 1;
  } //non-positive result means no formula is known.

  bool (*AreConjugateByFormula)(const elementSomeGroup& x, const elementSomeGroup& y);
  void (*ComputeCCSizesAndRepresentativesByFormula)(FiniteGroup<elementSomeGroup>& G);
  LargeInteger (*GetSizeByFormula)(FiniteGroup<elementSomeGroup>& G);
  bool AreConjugate(const elementSomeGroup& left, const elementSomeGroup& right);
  bool AreConjugate_OLD_Deprecated_Version_By_Todor(const elementSomeGroup& left, const elementSomeGroup& right);
  bool ComputeAllElements(bool andWords, int MaxElements = 1000000);
  // MaxElements gives an upper bound to the number of elements this function will try to find.
  // Set MaxElements = - 1 for "unlimited" upper bound.
  bool ComputeAllElementsLargeGroup(bool andWords, int MaxElements = 1000000);
  // MaxElements gives an upper bound to the number of elements this function will try to find.
  // Set MaxElements = - 1 for "unlimited" upper bound.

  // Historical note: this was from Thomas' second finite group class, and is
  // as of 2015- 11 the only way to generate the words and conjugacy information
  void ComputeCCfromAllElements();
  void ComputeCCfromCCindicesInAllElements(const List<List<int> >& ccIndices);

  void ComputeCCSizeOrCCFromRepresentative(ConjugacyClass& inputOutputClass, bool storeCC);
  bool RegisterCCclass(const elementSomeGroup& theRepresentative, bool dontAddIfSameInvariants);
  bool ComputeCCRepresentatives();
  void ComputeGeneratorsConjugacyClasses();
  void ComputeCCSizesAndRepresentatives(bool useComputeCCSizesRepresentativesWords = true);
  void ComputeCCSizesAndRepresentativesWithOrbitIterator();
  void ComputeCCSizesRepresentativesWords();
  bool CheckInitializationFDrepComputation() const;
  void ComputeSquaresCCReps();
  bool HasElement(const elementSomeGroup& g);
  bool GetWord(const elementSomeGroup& g, List<int>& out);
  bool (*GetWordByFormula)(FiniteGroup<elementSomeGroup>& G, const elementSomeGroup& g, List<int>& out);
  void GetSignCharacter(Vector<Rational>& outputCharacter);
  template <typename coefficient>
  coefficient GetHermitianProduct(const Vector<coefficient>& leftCharacter, const Vector<coefficient>& rightCharacter);

  bool PossiblyConjugate(const elementSomeGroup& x, const elementSomeGroup& y);
  void makeIdentity(elementSomeGroup& x);
  bool IsID(elementSomeGroup& x);
  void ComputeGeneratorCommutationRelations();
  void VerifyCCSizesAndRepresentativesFormula();
  void VerifyWords();
  void VerifyArithmetic();
  std::string PrettyPrintGeneratorCommutationRelations(bool andPrint = false);
  std::string PrettyPrintCharacterTable(bool andPrint = false);
  std::string PrettyPrintCCRepsSizes(bool andPrint = false);
  JSData RepresentationDataIntoJS();

  // these methods are refugees from the WeylGroup class
  int MultiplyElements(int left, int right) const;
  int Invert(int g) const;
  bool CheckInitializationConjugacyClasses() const;
  // This method needs some initial irreps to be seeded in either the irreps or the irreps_grcam locations
  void ComputeIrreducibleRepresentationsTodorsVersion();
  // In order for this to work, the starting irrep must be one whose n-fold tensor products contain every other irrep
  void ComputeIrreducibleRepresentationsThomasVersion(
    GroupRepresentationCarriesAllMatrices<FiniteGroup<elementSomeGroup>, Rational>* startingIrrep
  );
  void (*ComputeIrreducibleRepresentationsWithFormulas)(FiniteGroup<elementSomeGroup>& G);
  void ComputeIrreducibleRepresentations();

  // A parabolic subgroup H = <hᵢ> is a subgroup of G = <gᵢ> in which for h∈H,
  // there is a canonical word in gᵢ..gₖ for h in G, with the property that the gᵢ's
  // in h are actually hᵢ's.  Where this happens, it makes it much easier to come up
  // with words in H, to be used for representation theory.
  // The intended use for this API call is
  // auto H = G.ParabolicKindaSubgroupGeneratorSubset(subgenids);
  SubgroupData<FiniteGroup<elementSomeGroup>, elementSomeGroup> ParabolicKindaSubgroupGeneratorSubset(const List<int>& subgens);

  // auto M = G.GetEmptyRationalRepresentation()
  GroupRepresentation<FiniteGroup<elementSomeGroup>, Rational> GetEmptyRationalRepresentation();

  friend std::ostream& operator<<(std::ostream& out, FiniteGroup<elementSomeGroup>& data) {
    out << data.toString(0);
    return out;
  }
};

template <class elementSomeGroup>
void FiniteGroup<elementSomeGroup>::init() {
  this->generators.setSize(0);
  this->conjugacyClasseS.setSize(0);
  this->squaresCCReps.setSize(0);
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
  this->specificDataPointer = nullptr;
  this->AreConjugateByFormula = nullptr;
  this->ComputeCCSizesAndRepresentativesByFormula = nullptr;
  this->GetSizeByFormula = nullptr;
  this->GetWordByFormula = nullptr;
}

struct simpleReflectionOrOuterAutomorphism {
  bool flagIsOuter;
  int index;
  simpleReflectionOrOuterAutomorphism(): flagIsOuter(false), index(- 1) {
  }
  std::string toString() const;
  void MakeOuterAuto(int inputIndex) {
    this->flagIsOuter = true;
    this->index = inputIndex;
  }
  void MakeSimpleReflection(int inputIndex) {
    this->flagIsOuter = false;
    this->index = inputIndex;
  }
  unsigned int hashFunction() const {
    return static_cast<unsigned>(1 + this->index + 100 * flagIsOuter);
  }
  bool operator==(const simpleReflectionOrOuterAutomorphism& other) const {
    return this->flagIsOuter == other.flagIsOuter && this->index == other.index;
  }
  bool operator>(const simpleReflectionOrOuterAutomorphism& right) const {
    if (right.flagIsOuter != this->flagIsOuter) {
      return this->flagIsOuter > right.flagIsOuter;
    }
    return this->index > right.index;
  }
  unsigned int hashFunction(const simpleReflectionOrOuterAutomorphism& input) const {
    return input.hashFunction();
  }
};

struct simpleReflection {
  int index;
  simpleReflection() {
    this->index = - 1;
  }
  std::string toString() const;
  void MakeSimpleReflection(int inputIndex) {
    this->index = inputIndex;
  }
  unsigned int hashFunction() const {
    return static_cast<unsigned>(index);
  }
  bool operator==(const simpleReflection& other) const {
    return this->index == other.index;
  }
  bool operator>(const simpleReflection& right) const {
    return this->index > right.index;
  }
};

class ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms {
public:
  SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms* owner;
  List<simpleReflectionOrOuterAutomorphism> generatorsLastAppliedFirst;
  bool flagDeallocated;
  void MultiplyOnTheRightBySimpleRootInner(int simpleRootIndex);
  void MultiplyOnTheRightByOuterAutomorphism(int indexOuterAutomorphism);
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
  List<simpleReflection> generatorsLastAppliedFirst;
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
  template<class coefficient>
  static void ActOn(const ElementWeylGroup& inputElt, const Vector<coefficient>& inputV, Vector<coefficient>& output);
  template<class coefficient>
  void ActOn(Vector<coefficient>& inputOutput) const;
  bool CheckConsistency() const;
  bool CheckInitialization() const;
  int Sign() const;
  void MultiplyOnTheRightBySimpleReflection(int reflectionIndex);
  static void ConjugationAction(
    const ElementWeylGroup& conjugateWith, const ElementWeylGroup& conjugateOn, ElementWeylGroup& output
  );
  ElementWeylGroup operator^(const ElementWeylGroup& right) const;
  void MakeFromRhoImage(const Vector<Rational>& inputRhoImage, WeylGroupData& inputWeyl);
  void MakeSimpleReflection(int simpleRootIndex, WeylGroupData& inputWeyl);
  void MakeRootReflection(const Vector<Rational>& mustBeRoot, WeylGroupData& inputWeyl);
  void MakeCanonical();
  // This is actually a very reasonable API call to have
  void makeIdentity(WeylGroupData& inputWeyl);
  // but for consistency with the other group element type, we must also have
  void makeIdentity(const FiniteGroup<ElementWeylGroup>& inputGroup);
  void makeIdentity(const ElementWeylGroup& initializeFrom);
  bool IsID();
  std::string ToStringInvariants(FormatExpressions* theFormat) const;
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
  void GetCycleStructure(VectorSparse<Rational>& outputIndexIsCycleSizeCoordinateIsCycleMult) const;
  Vector<Rational> operator*(const Vector<Rational>& v) const;
  ElementWeylGroup Inverse() const;
  bool HasDifferentConjugacyInvariantsFrom(const ElementWeylGroup& right) const;
  void GetCharacteristicPolyStandardRepresentation(Polynomial<Rational>& output) const;
  bool operator==(const ElementWeylGroup& other) const {
    if (this->owner != other.owner) {
      return false;
    }
    return this->generatorsLastAppliedFirst == other.generatorsLastAppliedFirst;
  }
  void MakeFromReadableReflections(
    WeylGroupData& input, bool dontMakeCanonical_SET_TRUE_ON_YOUR_OWN_RISK, const std::string& inputReflections
  );
  bool operator>(const ElementWeylGroup& other) const;
};

class WeylGroupAutomorphisms;
class ElementWeylGroupAutomorphisms {
public:
  WeylGroupAutomorphisms* owner;
  List<simpleReflectionOrOuterAutomorphism> generatorsLastAppliedFirst;
  bool flagDeallocated;
  void makeIdentity(WeylGroupAutomorphisms& inputAutomorphisms);
  ElementWeylGroupAutomorphisms();
  ~ElementWeylGroupAutomorphisms();
  bool CheckInitialization() const;
  template<class coefficient>
  static void ActOn(
    const ElementWeylGroupAutomorphisms& inputElt, const Vector<coefficient>& inputV, Vector<coefficient>& output
  );
  template<class coefficient>
  void ActOn(Vector<coefficient>& inputOutput) const;
  void MakeOuterAuto(int outerAutoIndex, WeylGroupAutomorphisms& inputWeyl);
  void MakeSimpleReflection(int simpleRootIndex, WeylGroupAutomorphisms& inputWeyl);
  void MultiplyOnTheRightByOuterAuto(int outerAutoIndex);
  unsigned int hashFunction() const;
  static unsigned int hashFunction(const ElementWeylGroupAutomorphisms& input);
  std::string toString(FormatExpressions* theFormat = nullptr) const;
};

template <class coefficient>
class FinitelyGeneratedMatrixMonoid {
  public:
  List<MatrixTensor<coefficient> > theGenerators;
  HashedList<MatrixTensor<coefficient> > theElements;
  bool GenerateElements(int upperBoundNonPositiveMeansNoLimit);
  std::string toString(FormatExpressions* theFormat = nullptr) const;
};

template <class coefficient>
bool FinitelyGeneratedMatrixMonoid<coefficient>::GenerateElements(int upperBoundNonPositiveMeansNoLimit) {
  MacroRegisterFunctionWithName("FinitelyGeneratedMatrixMonoid::GenerateElements");
  this->theElements.Clear();
  this->theElements.addOnTopNoRepetition(theGenerators);
  MatrixTensor<coefficient> currentElement;
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

template <typename somegroup, typename coefficient>
class VirtualRepresentation;

template <typename somegroup, typename coefficient>
class GroupRepresentationCarriesAllMatrices;

class SubgroupDataRootReflections;

class WeylGroupData {
  Matrix<Rational> FundamentalToSimpleCoords;
  Matrix<Rational> SimpleToFundamentalCoords;
  MemorySaving<Matrix<Rational> > MatrixSendsSimpleVectorsToEpsilonVectors;
  bool flagFundamentalToSimpleMatricesAreComputed;
  void ComputeFundamentalToSimpleMatrices() {
    if (this->flagFundamentalToSimpleMatricesAreComputed) {
      return;
    }
    Vectors<Rational> fundamentalBasis;
    this->GetFundamentalWeightsInSimpleCoordinates(fundamentalBasis);
    this->FundamentalToSimpleCoords.AssignVectorsToRows(fundamentalBasis);
    this->FundamentalToSimpleCoords.Transpose();
    this->SimpleToFundamentalCoords = this->FundamentalToSimpleCoords;
    this->SimpleToFundamentalCoords.Invert();
    this->flagFundamentalToSimpleMatricesAreComputed = true;
  }
  bool LoadConjugacyClasses();
  bool LoadCharTable();
  WeylGroupData(const WeylGroupData& other);
  void operator=(const WeylGroupData& other);
  //<- once created, WeylGroupData can't be moved: a pointer to it is stored in FiniteGroup
public:
  bool flagIrrepsAreComputed;
  bool flagCharTableIsComputed;

  DynkinType theDynkinType;
  Matrix<Rational> CartanSymmetric;
  Matrix<Rational> CoCartanSymmetric;
  Vector<Rational> rho;
  HashedList<Vector<Rational> > rhoOrbit;
  HashedList<Vector<Rational> > RootSystem;
  Vectors<Rational> RootsOfBorel;
  FiniteGroup<ElementWeylGroup> theGroup;

  List<std::string> ccCarterLabels;
  List<std::string> irrepsCarterLabels;
  // void MakeFromParSel(Vector<Rational> & parSel, WeylGroup& input);
  bool flagDeallocated;

  typedef FiniteGroup<ElementWeylGroup> WeylGroupBase;
  void reset();
  static void GetCoCartanSymmetric(const Matrix<Rational>& input, Matrix<Rational>& output);
  void ComputeSquares();
  void ComputeInitialIrreps();
  void GetSignSignatureParabolics(List<SubgroupDataRootReflections>& outputSubgroups);
  void GetSignSignatureExtendedParabolics(List<SubgroupDataRootReflections>& outputSubgroups);
  void GetSignSignatureAllRootSubsystems(List<SubgroupDataRootReflections>& outputSubgroups);
  void GetSignSignatureRootSubgroups(
    List<SubgroupDataRootReflections>& outputSubgroups, const List<Vectors<Rational> >& rootsGeneratingReflections
  );
  bool LoadConjugacyClassesHelper();
  bool LoadSignSignatures(List<SubgroupDataRootReflections>& outputSubgroups);
  void ComputeOrLoadCharacterTable(std::stringstream* reportStream = nullptr);
  void ComputeOrLoadConjugacyClasses(std::stringstream* reportStream = nullptr);
  static void ComputeIrreducibleRepresentationsWithFormulasImplementation(FiniteGroup<ElementWeylGroup>& G);
  void ComputeExtremeRootInTheSameKMod(
    const Vectors<Rational>& inputSimpleBasisK,
    const Vector<Rational>& inputRoot,
    Vector<Rational>& output,
    bool lookingForHighest
  );
  void AddCharacter(const ClassFunction<WeylGroupData::WeylGroupBase, Rational>& X);
  void ComputeRho(bool Recompute);
  std::string ToStringRootsAndRootReflections(FormatExpressions* theFormat = nullptr);
  std::string toString(FormatExpressions* theFormat = nullptr);
  std::string ToStringCppConjugacyClasses(FormatExpressions* theFormat = nullptr);
  std::string ToStringCppCharTable(FormatExpressions* theFormat = nullptr);
  std::string ToStringIrrepLabel(int irrepIndex);
  std::string ToStringSignSignatureRootSubsystem(const List<SubgroupDataRootReflections>& inputSubgroups);
  void MakeArbitrarySimple(char WeylGroupLetter, int n, const Rational* firstCoRootLengthSquared = nullptr);
  void MakeFromDynkinType(const DynkinType& inputType);
  void MakeFinalSteps();
  void InitGenerators();
  template <class coefficient>
  LargeInteger GetOrbitSize(Vector<coefficient>& theWeight);
  void MakeMeFromMyCartanSymmetric();
  void MakeFromDynkinTypeDefaultLengthKeepComponentOrder(const DynkinType& inputType);
  void ComputeCoCartanSymmetricFromCartanSymmetric();
  bool CheckConsistency() const;
  static bool GetWordByFormulaImplementation(
    FiniteGroup<ElementWeylGroup>& G, const ElementWeylGroup& g, List<int>& out
  );
  void GetSignCharacter(Vector<Rational>& out);
  void GetStandardRepresentation(
    GroupRepresentationCarriesAllMatrices<FiniteGroup<ElementWeylGroup>, Rational>& output
  );
  void GetSignRepresentation(
    GroupRepresentationCarriesAllMatrices<FiniteGroup<ElementWeylGroup>, Rational>& output
  );
  void GetTrivialRepresentation(
    GroupRepresentationCarriesAllMatrices<FiniteGroup<ElementWeylGroup>, Rational>& output
  );
  void GenerateAdditivelyClosedSubset(Vectors<Rational>& input, Vectors<Rational>& output);
  bool LoadGAPRootSystem(HashedList<Vector<Rational> >& outputPositiveRootSystem) const;
  Rational GetKillingDivTraceRatio();
  Rational EstimateNumDominantWeightsBelow(Vector<Rational>& inputHWsimpleCoords);
  bool ContainsARootNonStronglyPerpendicularTo(Vectors<Rational>& theVectors, Vector<Rational>& input);
  int NumRootsConnectedTo(Vectors<Rational>& theVectors, Vector<Rational>& input);
  void GetHighestWeightsAllRepsDimLessThanOrEqualTo(
    List<Vector<Rational> >& outputHighestWeightsFundCoords, int inputDimBound
  );
  Rational GetLongestRootLengthSquared() const;
  static unsigned int hashFunction(const WeylGroupData& input) {
    return input.CartanSymmetric.hashFunction();
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
  bool IsOfSimpleType(char desiredType, int desiredRank) const {
    return this->theDynkinType.IsOfSimpleType(desiredType, desiredRank);
  }
  Matrix<Rational>* GetMatrixFundamentalToSimpleCoords() {
    this->ComputeFundamentalToSimpleMatrices();
    return &this->FundamentalToSimpleCoords;
  }
  Matrix<Rational>* GetMatrixSimpleToFundamentalCoords() {
    this->ComputeFundamentalToSimpleMatrices();
    return &this->SimpleToFundamentalCoords;
  }
  template<class coefficient>
  Vector<coefficient> GetSimpleCoordinatesFromFundamental(const Vector<coefficient>& inputInFundamentalCoords);
  template<class coefficient>
  Vectors<coefficient> GetSimpleCoordinatesFromFundamental(const Vectors<coefficient>& inputInFundamentalCoords);
  template<class coefficient>
  Vector<coefficient> GetFundamentalCoordinatesFromSimple(const Vector<coefficient>& inputInSimpleCoords);
  template<class coefficient>
  Vector<coefficient> GetFundamentalCoordinatesFromEpsilon(const Vector<coefficient>& inputInEpsCoords);
  template<class coefficient>
  Vector<coefficient> GetSimpleCoordinatesFromEpsilon(const Vector<coefficient>& inputInEpsCoords);
  template<class coefficient>
  Vector<coefficient> GetDualCoordinatesFromSimple(const Vector<coefficient>& inputInSimpleCoords) {
    return this->GetDualCoordinatesFromFundamental(this->GetFundamentalCoordinatesFromSimple(inputInSimpleCoords));
  }
  template <class coefficient>
  Vector<coefficient> GetDualCoordinatesFromFundamental(const Vector<coefficient>& inputInFundamentalCoords) {
    Vector<coefficient> result = inputInFundamentalCoords;
    if (result.size != this->GetDim()) {
      global.fatal << "This is a programming error. The input fundamental weight has "
      << result.size << " coordinates, while the rank of the Weyl group is "
      << this->GetDim() << ". " << global.fatal;
    }
    for (int i = 0; i < result.size; i ++) {
      result[i] *= this->CartanSymmetric.elements[i][i] / 2;
    }
    return result;
  }
  template <class coefficient>
  coefficient GetScalarProdSimpleRoot(const Vector<coefficient>& input, int indexSimpleRoot) const {
    if (indexSimpleRoot < 0 || indexSimpleRoot >= this->GetDim()) {
      global.fatal << "This is a programming error. "
      << "Attempting to take scalar product with simple root of index "
      << indexSimpleRoot
      << " which is impossible, as the rank of the Weyl group is "
      << this->GetDim() << ". " << global.fatal;
    }
    coefficient result, buffer;
    result = 0;
    Rational* currentRow = this->CartanSymmetric.elements[indexSimpleRoot];
    for (int i = 0; i < input.size; i ++) {
      buffer = input[i];
      buffer *= currentRow[i];
      result += buffer;
    }
    return result;
  }
  template <class coefficient>
  coefficient WeylDimFormulaSimpleCoords(Vector<coefficient>& theWeightInSimpleCoords, const coefficient& theRingUnit = 1);
  template <class coefficient>
  coefficient WeylDimFormulaFundamentalCoords(Vector<coefficient>& weightFundCoords);
  template <class coefficient>
  void RaiseToDominantWeight(
    Vector<coefficient>& theWeight,
    int* sign = nullptr,
    bool* stabilizerFound = nullptr,
    ElementWeylGroup* raisingElt = nullptr
  );
  bool AreMaximallyDominantGroupInner(List<Vector<Rational> >& theWeights);
  template <class coefficient>
  void RaiseToMaximallyDominanT(List<Vector<coefficient> >& theWeights);
  void GetCoxeterPlane(Vector<double>& outputBasis1, Vector<double>& outputBasis2);
  void GetSimpleReflectionMatrix(int indexSimpleRoot, Matrix<Rational>& output) const;
  ElementWeylGroup SimpleConjugation(int i, const ElementWeylGroup& vv);
  Matrix<Rational> SimpleReflectionMatrix(int indexSimpleRoot) const;
  void GetStandardRepresentationMatrix(int g, Matrix<Rational>& output) const;
  void DrawRootSystem(
    DrawingVariables& outputDV,
    bool wipeCanvas,
    bool drawWeylChamber,
    Vector<Rational>* bluePoint = nullptr,
    bool LabelDynkinDiagramVertices = false,
    Vectors<Rational>* predefinedProjectionPlane = nullptr
  );
  bool HasStronglyPerpendicularDecompositionWRT(
    Vector<Rational>& input,
    int UpperBoundNumBetas,
    Vectors<Rational>& theSet,
    Vectors<Rational>& output,
    List<Rational>& outputCoeffs,
    bool IntegralCoefficientsOnly
  );
  // void MakeFromDynkinType(List<char>& theLetters, List<int>& theRanks, List<int>* theMultiplicities);
  // void MakeFromDynkinType(List<char>& theLetters, List<int>& theRanks){ this->MakeFromDynkinType(theLetters, theRanks, 0); }
  // void GetLongRootLength(Rational& output);
  static bool IsAddmisibleDynkinType(char candidateLetter, int n);
  // the below will not do anything if the inputLetter is not a valid Dynkin letter
  static void TransformToAdmissibleDynkinType(char inputLetter, int& outputRank);
  void ComputeEpsilonMatrix();
  template <class coefficient>
  void GetEpsilonCoords(const Vector<coefficient>& input, Vector<coefficient>& output) {
    this->ComputeEpsilonMatrix();
    output = input;
    this->MatrixSendsSimpleVectorsToEpsilonVectors.GetElement().ActOnVectorColumn(output);
  }
  void GetEpsilonCoords(const List<Vector<Rational> >& input, Vectors<Rational>& output);
  template <class coefficient>
  Vector<coefficient> GetEpsilonCoords(const Vector<coefficient>& input) {
    Vector<coefficient> output;
    this->GetEpsilonCoords(input, output);
    return output;
  }
  bool IsStronglyPerpendicularTo(const Vector<Rational>& input, const Vector<Rational>& other);
  bool IsStronglyPerpendicularTo(const Vector<Rational>& input, const Vectors<Rational>& others);
  void GetEpsilonCoordsWRTsubalgebra(Vectors<Rational>& generators, List<Vector<Rational> >& input, Vectors<Rational>& output);
  bool LeftIsHigherInBruhatOrderThanRight(ElementWeylGroup& left, ElementWeylGroup& right);
  bool IsElementWeylGroup(const MatrixTensor<Rational>& theMat);
  void GetMatrixReflection(Vector<Rational>& reflectionRoot, Matrix<Rational>& output);
  template <class coefficient>
  bool GetAlLDominantWeightsHWFDIM(
    Vector<coefficient>& highestWeightSimpleCoords,
    HashedList<Vector<coefficient> >& outputWeightsSimpleCoords,
    int upperBoundDominantWeights,
    std::string* outputDetails
  );
  template <class coefficient>
  bool FreudenthalEval(
    Vector<coefficient>& inputHWfundamentalCoords,
    HashedList<Vector<coefficient> >& outputDominantWeightsSimpleCoords,
    List<coefficient>& outputMultsSimpleCoords,
    std::string* outputDetails,
    int UpperBoundFreudenthal
  );
  void GetWeylChamber(Cone& output);
  std::string GenerateWeightSupportMethoD1(
    Vector<Rational>& highestWeightSimpleCoords, Vectors<Rational>& outputWeightsSimpleCoords, int upperBoundWeights
  );
  void GetIntegralLatticeInSimpleCoordinates(Lattice& output);
  void GetFundamentalWeightsInSimpleCoordinates(Vectors<Rational>& output);
  int GetDim() const {
    return this->CartanSymmetric.NumRows;
  }
  void ComputeWeylGroupAndRootsOfBorel(Vectors<Rational>& output);
  void ComputeRootsOfBorel(Vectors<Rational>& output);
  static LargeInteger GetSizeByFormulaImplementation(FiniteGroup<ElementWeylGroup>& G);
  static LargeInteger SizeByFormulaOrNeg1(char weylLetter, int theDim);
  bool IsARoot(const Vector<Rational>& input) const {
    return this->RootSystem.Contains(input);
  }
  void GenerateRootSubsystem(Vectors<Rational>& theRoots);
  template <class coefficient>
  bool GenerateOrbit(
    Vectors<coefficient>& theWeights,
    bool RhoAction,
    HashedList<Vector<coefficient> >& output,
    bool UseMinusRho,
    int expectedOrbitSize = - 1,
    HashedList<ElementWeylGroup>* outputSubset = nullptr,
    int UpperLimitNumElements = - 1
  );
  template <class coefficient>
  bool GenerateOrbit(
    Vector<coefficient>& theWeight,
    bool RhoAction,
    HashedList<Vector<coefficient> >& output,
    bool UseMinusRho,
    int expectedOrbitSize = - 1,
    HashedList<ElementWeylGroup>* outputSubset = nullptr,
    int UpperLimitNumElements = - 1
  ) {
    Vectors<coefficient> theWeights;
    theWeights.addOnTop(theWeight);
    return this->GenerateOrbit(
      theWeights, RhoAction, output, UseMinusRho, expectedOrbitSize, outputSubset, UpperLimitNumElements
    );
  }
//  int GetNumRootsFromFormula();
  void GenerateRootSystem();
  void ActOnAffineHyperplaneByGroupElement(int index, affineHyperplane<Rational>& output, bool RhoAction, bool UseMinusRho);
  void ProjectOnTwoPlane(Vector<Rational>& orthonormalBasisVector1, Vector<Rational>& orthonormalBasisVector2);
  void GetLowestElementInOrbit(
    Vector<Rational>& inputOutput,
    ElementWeylGroup* outputWeylElt,
    Vectors<Rational>& bufferEiBAsis,
    bool RhoAction,
    bool UseMinusRho,
    int* sign = nullptr,
    bool* stabilizerFound = nullptr
  ) {
    this->GetExtremeElementInOrbit(
      inputOutput, outputWeylElt, bufferEiBAsis, true, RhoAction, UseMinusRho, sign, stabilizerFound
    );
  }
  void GetHighestElementInOrbit(
    Vector<Rational>& inputOutput,
    ElementWeylGroup* outputWeylElt,
    Vectors<Rational>& bufferEiBAsis,
    bool RhoAction,
    bool UseMinusRho,
    int* sign,
    bool* stabilizerFound
  ) {
    this->GetExtremeElementInOrbit(
      inputOutput, outputWeylElt, bufferEiBAsis, false, RhoAction, UseMinusRho, sign, stabilizerFound
    );
  }
  void GetExtremeElementInOrbit(
    Vector<Rational>& inputOutput,
    ElementWeylGroup* outputWeylElt,
    Vectors<Rational>& bufferEiBAsis,
    bool findLowest,
    bool RhoAction,
    bool UseMinusRho,
    int* sign,
    bool* stabilizerFound
  );
  void GetLongestWeylElt(ElementWeylGroup& outputWeylElt);
  bool IsEigenSpaceGeneratorCoxeterElement(Vector<Rational>& input);
  void GetCoxeterElement(ElementWeylGroup& outputWeylElt) {
    outputWeylElt.generatorsLastAppliedFirst.setSize(this->GetDim());
    for (int i = 0; i < outputWeylElt.generatorsLastAppliedFirst.size; i ++) {
      outputWeylElt.generatorsLastAppliedFirst[i].MakeSimpleReflection(i);
    }
  }
  Vector<Rational> ApplyReflectionList(const List<int>& rightReflectionsActFirst, const Vector<Rational>& vv) const;
  template <class coefficient>
  void ActOn(const ElementWeylGroup& theGroupElement, Vector<coefficient>& inputOutput) const {
    this->ActOn(theGroupElement, inputOutput, inputOutput);
  }
  template <class coefficient>
  void ActOn(int indexOfWeylElement, Vector<coefficient>& theVector) const {
    this->ActOn(this->theGroup.theElements[indexOfWeylElement], theVector, theVector);
  }
  template <class coefficient>
  void ActOn(
    const ElementWeylGroup& theGroupElement,
    const Vector<coefficient>& inputVector,
    Vector<coefficient>& outputVector
  ) const {
    outputVector = inputVector;
    for (int i = theGroupElement.generatorsLastAppliedFirst.size - 1; i >= 0; i --) {
      this->SimpleReflection(theGroupElement.generatorsLastAppliedFirst[i].index, outputVector);
    }
  }
  template <class coefficient>
  void ActOnRhoModified(const ElementWeylGroup& theGroupElement, Vector<coefficient>& theVector) const {
    Vector<coefficient> tempV;
    theVector += this->rho;
    this->ActOn(theGroupElement, theVector, tempV);
    theVector = tempV;
    theVector -= this->rho;
  }
  template <class coefficient>
  void ActOnRhoModified(int indexOfWeylElement, Vector<coefficient>& theVector) const {
    this->ActOnRhoModified(this->theGroup.theElements[indexOfWeylElement], theVector);
  }
  template <class coefficient>
  void ActOnDual(int index,Vector<coefficient>& theVector, bool RhoAction, const coefficient& theRingZero);
  //theRoot is a list of the simple coordinates of the Vector<Rational>
  //theRoot serves as both input and output
  void ActOnRootAlgByGroupElement(int index, PolynomialSubstitution<Rational>& theRoot, bool RhoAction);
  void ActOnRootByGroupElement(int index, Vector<Rational>& theRoot, bool RhoAction, bool UseMinusRho);
  void ActOnDualSpaceElementByGroupElement(int index, Vector<Rational>& theDualSpaceElement, bool RhoAction);
  void SimpleReflectionRoot(int index, Vector<Rational>& theRoot, bool RhoAction, bool UseMinusRho);
  template <class coefficient>
  void SimpleReflection(int index, Vector<coefficient>& theVector) const;
  template <class coefficient>
  void SimpleReflectionRhoModified(int index, Vector<coefficient>& theVector) const;
  template <class coefficient>
  void SimpleReflectionMinusRhoModified(int index, Vector<coefficient>& theVector) const;
  ElementWeylGroup GetRootReflection(int rootIndex);
  void GetWord(int g, List<int>& out) const;
  void GetWord(const ElementWeylGroup& g, List<int>& out) const {
    out.setSize(g.generatorsLastAppliedFirst.size);
    for (int i = 0; i < g.generatorsLastAppliedFirst.size; i ++) {
      out[i] = g.generatorsLastAppliedFirst[i].index;
    }
  }
  Matrix<Rational> GetMatrixStandardRep(int elementIndex) const;
  void GetMatrixStandardRep(const ElementWeylGroup& input, Matrix<Rational>& outputMatrix) const;
  Matrix<Rational> GetMatrixStandardRep(const ElementWeylGroup& input) const {
    Matrix<Rational> result;
    this->GetMatrixStandardRep(input, result);
    return result;
  }
  void GetElementOfMatrix(Matrix<Rational>& input, ElementWeylGroup& output);
  void SimpleReflectionDualSpace(int index, Vector<Rational>& DualSpaceElement);
  void SimpleReflectionRootAlg(int index, PolynomialSubstitution<Rational>& theRoot, bool RhoAction);
  bool IsPositiveOrPerpWRTh(const Vector<Rational>& input, const Vector<Rational>& theH) {
    return this->RootScalarCartanRoot(input, theH).IsPositiveOrZero();
  }
  template<class coefficient>
  void ReflectBetaWRTAlpha(
    const Vector<Rational>& alpha, const Vector<coefficient>& Beta, bool RhoAction, Vector<coefficient>& Output
  ) const;
  bool IsRegular(Vector<Rational>& input, int* indexFirstPerpendicularRoot);
  template <class leftType, class rightType>
  void RootScalarCartanRoot(const Vector<leftType>& r1, const Vector<rightType>& r2, leftType& output) const;
  double RootScalarCartanRoot(const Vector<double>& r1, const Vector<double>& r2) const {
    if (r1.size != r2.size || r1.size != this->GetDim()) {
      global.fatal << "This is a programming error: attempting to take the root system scalar product of "
      << "vectors of different dimension or of dimension different "
      << "from that of the ambient Lie algebra. The two input vectors were "
      << r1 << " and " << r2 << " and the rank of the Weyl group is " << this->GetDim() << ". ";
      global.fatal << global.fatal;
    }
    double result = 0;
    for (int i = 0; i < this->GetDim(); i ++) {
      for (int j = 0; j < this->GetDim(); j ++) {
        result += this->CartanSymmetric.elements[i][j].GetDoubleValue() * r1[i] * r2[j];
      }
    }
    return result;
  }
  template <class leftType, class rightType>
  leftType RootScalarCartanRoot(const Vector<leftType>& r1, const Vector<rightType>& r2) const {
    leftType tempRat;
    this->RootScalarCartanRoot(r1, r2, tempRat);
    return tempRat;
  }
  //the below functions perturbs input so that inputH has
  //non-zero scalar product with Vectors<Rational> of the Vector<Rational>  system,
  //without changing the inputH-sign of any Vector<Rational>  that had a non-zero scalar product to begin with
  void PerturbWeightToRegularWRTrootSystem(const Vector<Rational>& inputH, Vector<Rational>& output);
  template <class coefficient>
  bool IsDominantWRTgenerator(const Vector<coefficient>& theWeight, int generatorIndex);
  template <class coefficient>
  bool IsDominantWeight(const Vector<coefficient>& theWeight);
  static void TransformToSimpleBasisGenerators(
    Vectors<Rational>& theGens, const HashedList<Vector<Rational> >& inputRootSystem
  );
  static void TransformToSimpleBasisGeneratorsArbitraryCoords(
    Vectors<Rational>& theGens, const HashedList<Vector<Rational> >& inputRootSystem
  );
  void TransformToSimpleBasisGeneratorsWRTh(Vectors<Rational>& theGens, const Vector<Rational>& theH);
  int operator()(int i, int j) const;
  bool operator== (const WeylGroupData& other) const;
  void operator+= (const WeylGroupData& other);
};

template<class leftType, class rightType>
void WeylGroupData::RootScalarCartanRoot(const Vector<leftType>& r1, const Vector<rightType>& r2, leftType& output) const {
  if (r1.size != r2.size || r1.size != this->GetDim()) {
    global.fatal << "This is a programming error: attempting to get the scalar product of the weight "
    << r1 << " (dimension " << r1.size
    << ") with the weight " << r2 << " (dimension " << r2.size
    << "), while the dimension of the ambient Weyl group is " << this->GetDim()
    << ". ";
    global.fatal << global.fatal;
  }
  output = r1[0].zero();
  leftType buffer;
  for (int i = 0; i < this->CartanSymmetric.NumRows; i ++) {
    for (int j = 0; j < this->CartanSymmetric.NumCols; j ++) {
      buffer = r1[i];
      buffer *= r2[j];
      buffer *= this->CartanSymmetric.elements[i][j];
      output += buffer;
    }
  }
}

template<class coefficient>
void ElementWeylGroup::ActOn(const ElementWeylGroup& inputElt, const Vector<coefficient>& inputV, Vector<coefficient>& output) {
  inputElt.CheckInitialization();
  inputElt.owner->ActOn(inputElt, inputV, output);
}

template<class coefficient>
void ElementWeylGroup::ActOn(Vector<coefficient>& inputOutput) const {
  this->CheckInitialization();
  this->owner->ActOn(*this, inputOutput, inputOutput);
}

class WeylGroupAutomorphisms {
public:
  bool flagOuterAutosGeneratorsComputed;
  bool flagAllOuterAutosComputed;
  bool flagDeallocated;
  WeylGroupData* theWeyl;
  FinitelyGeneratedMatrixMonoid<Rational> theOuterAutos;
  HashedList<ElementWeylGroupAutomorphisms> allElements;
  void ComputeOuterAutoGenerators();
  void ComputeOuterAutos();
  WeylGroupAutomorphisms();
  ~WeylGroupAutomorphisms();
  template <class coefficient>
  bool GenerateOuterOrbit(
    Vectors<coefficient>& theWeights,
    HashedList<Vector<coefficient> >& output,
    HashedList<ElementWeylGroupAutomorphisms>* outputSubset = nullptr,
    int UpperLimitNumElements = - 1
  );
  LargeInteger GetOrbitSize(Vector<Rational>& theWeight);
  bool IsElementWeylGroupOrOuterAuto(const MatrixTensor<Rational>& theMat);
  bool AreMaximallyDominantGroupOuter(List<Vector<Rational> >& theWeights);
  bool checkInitialization() const;

  template <class coefficient>
  void ActOn(
    const ElementWeylGroupAutomorphisms& theGroupElement,
    const Vector<coefficient>& inputVector,
    Vector<coefficient>& outputVector
  ) const;
  template <class coefficient>
  void RaiseToMaximallyDominant(List<Vector<coefficient> >& theWeights);
};

template<class coefficient>
void ElementWeylGroupAutomorphisms::ActOn(
  const ElementWeylGroupAutomorphisms& inputElt, const Vector<coefficient>& inputV, Vector<coefficient>& output
) {
  inputElt.CheckInitialization();
  inputElt.owner->ActOn(inputElt, inputV, output);
}

template<class coefficient>
void ElementWeylGroupAutomorphisms::ActOn(Vector<coefficient>& inputOutput) const {
  this->CheckInitialization();
  this->owner->ActOn(*this, inputOutput, inputOutput);
}

template <class coefficient>
void WeylGroupAutomorphisms::ActOn(
  const ElementWeylGroupAutomorphisms& theGroupElement,
  const Vector<coefficient>& inputVector,
  Vector<coefficient>& outputVector
) const {
  this->checkInitialization();
  outputVector = inputVector;
  for (int i = theGroupElement.generatorsLastAppliedFirst.size - 1; i >= 0; i --) {
    simpleReflectionOrOuterAutomorphism& currentGenerator = theGroupElement.generatorsLastAppliedFirst[i];
    if (!currentGenerator.flagIsOuter) {
      this->theWeyl->SimpleReflection(currentGenerator.index, outputVector);
    } else {
      if (!this->flagAllOuterAutosComputed) {
        global.fatal << "Weyl group of type " << this->theWeyl->theDynkinType.toString()
        << " does not have its outer autos computed at a place where it should. " << global.fatal;
      }
      this->theOuterAutos.theGenerators[currentGenerator.index].ActOnVectorColumn(outputVector, outputVector);
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
    this->preimageGroup->GetWord(g, word);
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

  template <typename coefficient>
  GroupRepresentation<FiniteGroup<elementFirstGroup>, coefficient> PullbackRepresentation(
    GroupRepresentation<FiniteGroup<elementSecondGroup>, coefficient> M2
  ) {
    GroupRepresentation<FiniteGroup<elementFirstGroup>, coefficient> out;
    out.ownerGroup = this->preimageGroup;
    out.identifyingString = M2.identifyingString;
    out.generatorS.setSize(this->preimageGroup->generators.size);
    for (int i = 0; i < this->preimageGroup->generators.size; i ++) {
      elementSecondGroup ki = (*this)(this->preimageGroup->generators[i]);
      M2.GetMatrixOfElement(ki, out.generatorS[i]);
    }
    return out;
  }
};

template <typename someGroup, typename coefficient>
class GroupRepresentation {
public:
  someGroup* ownerGroup;
  List<Matrix<coefficient> > generatorS;
  mutable ClassFunction<someGroup, coefficient> theCharacteR;
  mutable bool flagCharacterIsComputed;
  std::string identifyingString; // in Python, this would be an anonymous object

  GroupRepresentation() {
    this->ownerGroup = NULL;
    this->flagCharacterIsComputed = false;
  }

  bool CheckInitialization() const;

  template <typename elementSomeGroup>
  bool GetMatrixOfElement(const elementSomeGroup& g, Matrix<coefficient>& out);

  void ComputeCharacter() const;
  ClassFunction<someGroup, coefficient> GetCharacter() {
    this->ComputeCharacter();
    return this->theCharacteR;
  }

  bool operator>(const GroupRepresentation& right) const;

  void MakeTensorRepresentation(
    GroupRepresentation<someGroup, coefficient>& right, GroupRepresentation<someGroup, coefficient>& left
  );

  bool VerifyRepresentation();

  bool VerifyRepresentationExpensive() {
    List<Matrix<coefficient> > repms;
    repms.setSize(this->ownerGroup->theElements.size);
    for (int i = 0; i < this->ownerGroup->theElements.size; i ++) {
      this->GetMatrixOfElement(this->ownerGroup->theElements[i], repms[i]);
    }
    for (int i = 0; i < this->ownerGroup->theElements.size; i ++) {
      for (int j = 0; j < this->ownerGroup->theElements.size; j ++) {
        if (repms[i] * repms[j] != repms[this->ownerGroup->theElements.GetIndex(
          this->ownerGroup->theElements[i] * this->ownerGroup->theElements[j])]
        ) {
          global.fatal << "Bad representation. " << global.fatal;
        }
      }
    }
    global.Comments << "Expensive verification complete, this is indeed a representation\n";
    return true;
  }
  std::string DescribeAsDirectSum();
  void operator*= (const GroupRepresentation<someGroup, coefficient>& other);

  JSData JSOut();

  template <typename somestream>
  somestream& IntoStream(somestream& out) const;
  std::string toString(FormatExpressions* fmt = nullptr) const;
  friend std::ostream& operator<<(std::ostream& out, GroupRepresentation<someGroup, coefficient>& data) {
    return data.IntoStream(out);
  }
  bool operator== (const GroupRepresentation<someGroup, coefficient>& right) const {
    if (this->ownerGroup != right.ownerGroup) {
      return false;
    }
    if (this->generatorS != right.generatorS) {
      return false;
    }
    return true;
    // if it ever becomes useful to compare element matrices...
    /*for (int i = 0; i < this->ownerGroup.)
    right.GetMatrixOfElement(this->ownerGroup.generators[i], M)
    if (M != this->generatorS[i])
      return false;*/
  }
  GroupRepresentation(const GroupRepresentationCarriesAllMatrices<someGroup, coefficient>& in) {
    this->generatorS = in.generatorS;
    this->theCharacteR = in.theCharacteR;
    this->flagCharacterIsComputed = in.flagCharacterIsComputed;
  }
  GroupRepresentationCarriesAllMatrices<someGroup, coefficient> MakeGRCAM() const {
    GroupRepresentationCarriesAllMatrices<someGroup, coefficient> out;
    out.generatorS = this->generatorS;
    out.flagCharacterIsComputed = this->flagCharacterIsComputed;
    if (this->identifyingString.size() > 0) {
      out.names.addOnTop(this->identifyingString);
    }
    out.theCharacteR = this->theCharacteR;
    return out;
  }
  // Note:  The group representation types compute the hash value from the character,
  // in which the order of the conjugacy classes determines the value
  // If you compare hash values from two groups which calculated their conjugacy classes in a different way,
  // you must ensure they are in the same order, or hashing won't work as expected.
  unsigned int hashFunction() const;
  static unsigned int hashFunction(const GroupRepresentation<someGroup, coefficient>& in) {
    return in.hashFunction();
  }
};

template <typename someGroup, typename coefficient>
template <typename elementSomeGroup>
bool GroupRepresentation<someGroup, coefficient>::GetMatrixOfElement(const elementSomeGroup& g, Matrix<coefficient>& out) {
  out.makeIdentity(this->generatorS[0]);
  List<int> word;
  if (!this->ownerGroup->GetWord(g, word)) {
    return false;
  }
  for (int i = 0; i < word.size; i ++) {
    out *= this->generatorS[word[i]];
  }
  return true;
}

template <typename someGroup, typename coefficient>
bool GroupRepresentation<someGroup, coefficient>::CheckInitialization() const {
  if (this->ownerGroup == 0) {
    global.fatal << "This is a programming error: group not initialized at a place where it should be." << global.fatal;
  }
  return true;
}

template <typename someGroup, typename coefficient>
void GroupRepresentation<someGroup, coefficient>::ComputeCharacter() const {
  if (this->flagCharacterIsComputed) {
    return;
  }
  this->CheckInitialization();
  if (!this->ownerGroup->flagCCsComputed) {
    this->ownerGroup->ComputeCCSizesAndRepresentatives();
  }
  this->theCharacteR.G = ownerGroup;
  this->theCharacteR.data.setSize(this->ownerGroup->conjugacyClasseS.size);
  for (int cci = 0; cci < this->ownerGroup->conjugacyClasseS.size; cci ++) {
    Matrix<coefficient> M;
    M.makeIdentity(this->generatorS[0]);
    List<int> ccirWord;
    // this->GetMatrixOfElement(this->ownerGroup->conjugacyClasseS[cci].representative);
    this->ownerGroup->GetWord(this->ownerGroup->conjugacyClasseS[cci].representative, ccirWord);
    for (int i = 0; i < ccirWord.size; i ++) {
      M *= this->generatorS[ccirWord[i]];
    }
    this->theCharacteR.data[cci] = M.GetTrace();
  }
  this->theCharacteR.G = ownerGroup;
  this->flagCharacterIsComputed = true;
}

// We aim to sort in reverse order within dimensions, according to
// the usual way character tables are displayed.
template <typename someGroup, typename coefficient>
bool GroupRepresentation<someGroup, coefficient>::operator>(const GroupRepresentation<someGroup, coefficient>& right) const {
  if (!this->flagCharacterIsComputed) {
    this->ComputeCharacter();
  }
  if (!right.flagCharacterIsComputed) {
    right.ComputeCharacter();
  }
  return this->theCharacteR > right.theCharacteR;
}

template <typename someGroup, typename coefficient>
void GroupRepresentation<someGroup, coefficient>::MakeTensorRepresentation(
  GroupRepresentation<someGroup, coefficient>& right, GroupRepresentation<someGroup, coefficient>& left
) {
  ownerGroup = right.ownerGroup;
  generatorS.setSize(right.generatorS.size);
  for (int i = 0; i < right.generatorS.size; i ++) {
    generatorS[i].AssignTensorProduct(right.generatorS[i], left.generatorS[i]);
  }
}

template <typename someGroup, typename coefficient>
template <typename somestream>
somestream& GroupRepresentation<someGroup, coefficient>::IntoStream(somestream& out) const {
  if (!this->flagCharacterIsComputed) {
    //this->ComputeCharacter();
    out << "Representation of the character not computed yet.";
    return out;
  }
  // WeylGroup needs to be printable
  // WeylGroup really needs to be printable lol
  out << "Representation of group " << ownerGroup->toString();
  if (!this->identifyingString.empty()) {
    out << "identified as " << identifyingString;
  }
  out << " with character " << this->theCharacteR;
  return out;
}

template <typename someGroup, typename coefficient>
std::string GroupRepresentation<someGroup, coefficient>::toString(FormatExpressions* fmt) const {
  (void) fmt;//portable way of avoiding unused parameter warning
  std::stringstream out;
  this->IntoStream(out);
  return out.str();
}

template <typename someGroup, typename coefficient>
JSData GroupRepresentation<someGroup, coefficient>::JSOut() {
  JSData out;
  out["identifyingString"] = identifyingString;
  if (this->flagCharacterIsComputed) {
    out["character"] = this->theCharacteR.data;
  } else {
    out["character"].theType = JSData::token::tokenNull;
  }
  for (int i = 0; i < generatorS.size; i ++) {
    out["generators"][i] = generatorS[i];
  }
  return out;
}

template <typename somegroup, typename coefficient>
class GroupRepresentationCarriesAllMatrices {
  friend std::ostream& operator << (std::ostream& output, const GroupRepresentationCarriesAllMatrices& theIrrep) {
    output << theIrrep.toString();
    return output;
  }
  // at one time, this class used the 'private' and 'friend' features of c++
  // specifically, friend WeylGroup;
  // which can't be immediately translated to templates
  // isn't that so sad?
  // QQ
public:
  List<Matrix<coefficient> > theElementImageS;
  List<bool> theElementIsComputed;
  ClassFunction<somegroup, coefficient> theCharacteR;
  List<Matrix<coefficient> > classFunctionMatrices;
  List<bool> classFunctionMatricesComputed;
  List<Matrix<coefficient> > generatorS;
  GroupRepresentationCarriesAllMatrices* parent;
  Vectors<coefficient> basis;
  Matrix<coefficient> gramMatrixInverted;
  public:
  bool flagCharacterIsComputed;
  somegroup* ownerGroup;
  List<std::string> names;

  GroupRepresentationCarriesAllMatrices() {
    this->reset();
  }
  GroupRepresentation<somegroup, coefficient> MakeOtherGroupRepresentationClass() const;
  // Note:  The group representation types compute the hash value from the character,
  // in which the order of the conjugacy classes determines the value
  // If you compare hash values from two groups which calculated their conjugacy classes in a different way,
  // you must ensure they are in the same order, or hashing won't work as expected.
  unsigned int hashFunction() const;
  bool CheckInitialization() const;
  bool CheckAllSimpleGensAreOK() const;
  static unsigned int hashFunction(const GroupRepresentationCarriesAllMatrices<somegroup, coefficient>& input) {
    return input.hashFunction();
  }
  void ComputeAllElementImages();
  const ClassFunction<somegroup, coefficient>& GetCharacter();
  VectorSpace<coefficient> FindDecentBasis() const;
  void MultiplyBy(
    const GroupRepresentationCarriesAllMatrices<somegroup, coefficient>& other,
    GroupRepresentationCarriesAllMatrices<somegroup, coefficient>& output
  ) const;
  void GetLargestDenominatorSimpleGens(LargeIntegerUnsigned& outputLCM, LargeIntegerUnsigned& outputDen) const;

  void reset();
  void init(somegroup& inputOwner);
  void CheckRepIsMultiplicativelyClosed();
  void GetClassFunctionMatrix(ClassFunction<somegroup, coefficient>& inputChar, Matrix<coefficient>& outputMat);
  void ClassFunctionMatrix(ClassFunction<somegroup, coefficient>& inputCF, Matrix<coefficient>& outputMat);
  int GetDim() const;
  void Restrict(
    const Vectors<coefficient>& VectorSpaceBasisSubrep,
    const ClassFunction<somegroup, Rational>& remainingCharacter,
    GroupRepresentationCarriesAllMatrices<somegroup, coefficient>& output
  );
  bool DecomposeTodorsVersionRecursive(
    VirtualRepresentation<somegroup, coefficient>& outputIrrepMults,
    List<GroupRepresentation<somegroup, coefficient> >& appendOnlyIrrepList,
    List<GroupRepresentationCarriesAllMatrices<somegroup, coefficient> >* appendOnlyGRCAMSList = 0
  );
  bool DecomposeTodorsVersion(
    VirtualRepresentation<somegroup, coefficient>& outputIrrepMults,
    List<GroupRepresentationCarriesAllMatrices<somegroup, coefficient> >* appendOnlyIrrepsList = 0
  );
  List<GroupRepresentationCarriesAllMatrices<somegroup, coefficient> > DecomposeThomasVersion();
  GroupRepresentationCarriesAllMatrices<somegroup, coefficient> Reduced() const;
  coefficient GetNumberOfComponents();
  void operator*= (const GroupRepresentationCarriesAllMatrices<somegroup, coefficient>& other);
  GroupRepresentationCarriesAllMatrices<somegroup, coefficient> operator*(
    const GroupRepresentationCarriesAllMatrices<somegroup, coefficient>& other
  ) const {
    GroupRepresentationCarriesAllMatrices<somegroup, coefficient> result = *this;
    result *= other;
    return result;
  }
  bool operator==(const GroupRepresentationCarriesAllMatrices<somegroup, coefficient>& other) const {
    return this->ownerGroup == other.ownerGroup && this->theCharacteR == other.theCharacteR;
  }
  void SpreadVector(const Vector<coefficient>& input, Vectors<coefficient>& outputBasisGeneratedSpace);
  std::string GetName() const;
  std::string toString(FormatExpressions* theFormat = nullptr) const;
  Matrix<coefficient>& GetMatrixElement(int groupElementIndex);
  template <typename elementSomeGroup>
  void GetMatrixElement(const elementSomeGroup& input, Matrix<coefficient>& output);
  template <typename elementSomeGroup>
  Matrix<coefficient> GetMatrixElement(const elementSomeGroup& input);
  void SetElementImage(int elementIndex, const Matrix<coefficient>& input) {
    this->theElementImageS[elementIndex] = input;
    this->theElementIsComputed[elementIndex] = true;
  }
  void SetGenerator(int generatorIndex, const Matrix<coefficient>& input) {
    this->generatorS[generatorIndex] = input;
  }
  bool operator>(const GroupRepresentationCarriesAllMatrices<somegroup, coefficient>& other) const;
  //  bool operator<(const GroupRepresentationCarriesAllMatrices<somegroup, coefficient>& other) const;
};

template <class somegroup, class coefficient>
class VirtualRepresentation : public LinearCombination<ClassFunction<somegroup, coefficient>, Rational> {
public:
  void operator*= (const VirtualRepresentation<somegroup, coefficient>& other);
  void AssignRep(const GroupRepresentationCarriesAllMatrices<somegroup, Rational>& other);
  void AssignRep(const GroupRepresentation<somegroup, Rational>& other);
  static unsigned int hashFunction(const VirtualRepresentation<somegroup, coefficient>& input) {
    return input.hashFunction();
  }
  unsigned int hashFunction() const {
    return this->::LinearCombination<ClassFunction<somegroup, coefficient>, Rational>::hashFunction();
  }
};

//This class iterates over all elements of the orbit of a single element
//using the generators of the group.
//The element can be an element of the group, representation, etc.
//(note that the group elements can be interpreted as elements of
//the group ring representation).
template <class elementGroup, class elementRepresentation>
class OrbitIterator {
public:
  struct GroupActionWithName {
    typedef void (*GroupAction)(
      const elementGroup& actingElement, const elementRepresentation& inputElementActedUpon, elementRepresentation& output
    );
    OrbitIterator::GroupActionWithName::GroupAction actOn;
    std::string name;
    bool CheckInitialization() const {
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
  bool CheckInitialization() const;
  std::string ToStringLayerSize() const;
  void resetNoActionChange() {
    this->previousLayer = &this->privateLayer1;
    this->currentLayer = &this->privateLayer2;
    this->nextLayer = &this->privateLayer3;
    this->indexCurrentElement = - 1;
  }
  const elementRepresentation& GetCurrentElement();
  bool IncrementReturnFalseIfPastLastFALSE();
  bool IncrementReturnFalseIfPastLast();
  void init(
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
  bool IncrementReturnFalseIfPastLast();
  const Vector<Rational>& GetCurrentElement();
  std::string toString() const;
  std::string ToStringSize() const;
  bool CheckConsistency();
  void initialize();
  void initialize(const List<ElementWeylGroupAutomorphisms>& inputGenerators,
    const Vector<Rational>& inputElement);
};

template <typename coefficient>
std::ostream& operator<<(std::ostream& out, const ClassFunction<WeylGroupData, coefficient>& X);

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
  bool CheckInitialization();
  void init();
  void MakeSubgroupOf(someGroup& G) {
    this->init();
    this->theGroup = &G;
    this->theSubgroup = &this->theSubgroupMayBeHere;
  }
  LargeInteger SizeByFormulaOrNeg1() const {
    return - 1;
  }
  void initFromGroupAndGenerators(someGroup& inputGroup, const List<elementSomeGroup>& inputGenerators);
  void MakeTranslatableWordsSubgroup(someGroup& inputGroup, const List<elementSomeGroup>& subGenerators);
  void ComputeCCRepresentativesPreimages();
  void ComputeCCSizesRepresentativesPreimages() {
    this->ComputeCCSizesAndRepresentatives();
    this->ComputeCCRepresentativesPreimages();
  }
  // Note: Cosets are... whichever of left coset or right coset they are
  bool flagCosetSetsComputed;
  bool flagCosetRepresentativesComputed;
  void ComputeCosets();
  bool VerifyCosets();
  bool VerifyNormal();
  std::string toString(FormatExpressions* theFormat);
  bool SameCosetAs(elementSomeGroup& g1, elementSomeGroup& g2);
  int GetCosetId(elementSomeGroup& g);
  int QIDMul(int i, int j);
  template <typename coefficient>
  void QuotientGroupPermutationRepresentation(GroupRepresentation<someGroup, coefficient>& out);
  template <typename coefficient>
  GroupRepresentation<someGroup, coefficient> InduceRepresentation(GroupRepresentation<someGroup, coefficient>& in);
  template <typename coefficient>
  GroupRepresentation<someGroup, coefficient> InduceRepresentationNormalSubgroup(
    GroupRepresentation<someGroup, coefficient>& in
  );
  void (*CosetRepresentativeEnumerator)(void* H) = nullptr;
  bool (*SameCosetAsByFormula)(void* H, elementSomeGroup& g1, elementSomeGroup& g2) = nullptr;
  //GroupRepresentation<Subgroup<somegroup,elementSomeGroup>, Rational> GetEmptyRationalRepresentationSubgroup();
};

template <typename elementSomeGroup>
bool TranslatableWordsSubgroupElementGetWord(FiniteGroup<elementSomeGroup>& H, const elementSomeGroup& g, List<int>& out) {
  List<int> superword;
  H.parentRelationship->theGroup->GetWord(g, superword);
  out.setSize(0);
  for (int i = 0; i < superword.size; i ++) {
    if (!H.parentRelationship->superGeneratorSubWordExists[superword[i]]) {
      if (!H.HasElement(g)) {
        global.Comments << "element g isn't a member of H\n";
      }
      global.fatal << "element " << g << " is assigned parent word " << superword.ToStringCommaDelimited()
      << " containing generator not found in subgroup " << superword[i]
      << " so if this does belong to the subgroup, we need a better algorithm. "
      << global.fatal;
    }
    out.addListOnTop(H.parentRelationship->superGeneratorSubWords[superword[i]]);
  }
  return true;
}

template <typename someGroup, typename elementSomeGroup>
int SubgroupData<someGroup, elementSomeGroup>::QIDMul(int i, int j) {
  this->ComputeCosets();
  auto g = this->cosets[i].representative * this->cosets[j].representative;
  return this->GetCosetId(g);
}

template <typename someGroup, typename elementSomeGroup>
bool SubgroupData<someGroup, elementSomeGroup>::VerifyNormal() {
  Matrix<int> csmt;
  csmt.init(cosets.size, cosets.size);
  for (int i = 0; i < cosets.size; i ++) {
    for (int j = 0; j < cosets.size; j ++) {
      csmt(i, j) = QIDMul(i, j);
    }
  }
  global.Comments << "Coset multiplication table\n" << csmt.ToStringPlainText() << '\n';
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
bool SubgroupData<someGroup, elementSomeGroup>::SameCosetAs(elementSomeGroup& g1, elementSomeGroup& g2) {
  if (this->SameCosetAsByFormula) {
    return SameCosetAsByFormula(this, g1, g2);
  }
  this->ComputeCosets();
  int g1i = this->theGroup->theElements.GetIndex(g1);
  int g2i = this->theGroup->theElements.GetIndex(g2);
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
int SubgroupData<someGroup, elementSomeGroup>::GetCosetId(elementSomeGroup& g) {
  int gi = - 1;
  if (this->CosetRepresentativeEnumerator && this->SameCosetAsByFormula) {
    if (!this->flagCosetRepresentativesComputed) {
      this->CosetRepresentativeEnumerator(this);
    }
  } else {
    if (!flagCosetSetsComputed) {
      this->ComputeCosets();
    }
    gi = this->theGroup->theElements.GetIndex(g);
  }
  for (int i = 0; i < this->cosets.size; i ++) {
    if (this->SameCosetAsByFormula) {
      if (this->SameCosetAsByFormula(this, this->cosets[i].representative, g)) {
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
template <typename coefficient>
void SubgroupData<someGroup, elementSomeGroup>::QuotientGroupPermutationRepresentation(GroupRepresentation<someGroup, coefficient>& out) {
  this->ComputeCosets();
  out.ownerGroup = this->theGroup;
  out.identifyingString = "Quotient permutation representation";
  global.Comments << "Subgroup::QuotientGroupPermutationRepresentation: Permuting " << cosets.size << " cosets.\n";
  out.generatorS.setSize(this->theGroup->generators.size);
  for (int i = 0; i < this->theGroup->generators.size; i ++) {
    out.generatorS[i].init(this->cosets.size, this->cosets.size);
    out.generatorS[i].makeZero();
    for (int ci = 0; ci <out.generatorS[i].NumCols; ci ++) {
      elementSomeGroup g =  this->theGroup->generators[i] * this->cosets[ci].representative;
      int j = this->GetCosetId(g);
      out.generatorS[i](j, ci) = 1;
    }
    global.Comments << "Element " << this->theGroup->generators[i] << " of coset " << this->GetCosetId(this->theGroup->generators[i]);
    global.Comments << " permutes the other cosets as\n" << out.generatorS[i].ToStringPlainText() << '\n';
  }
}

// G\otimes_H V
// g(w) = h[g](\otimes v) where [g] rearranges the v's
//
template <typename someGroup, typename elementSomeGroup>
template <typename coefficient>
GroupRepresentation<someGroup, coefficient> SubgroupData<someGroup, elementSomeGroup>::InduceRepresentationNormalSubgroup(
  GroupRepresentation<someGroup, coefficient>& in
) {
  GroupRepresentation<someGroup, coefficient> qr;
  this->QuotientGroupPermutationRepresentation(qr);
  GroupRepresentation<someGroup, coefficient> sr;
  sr.ownerGroup = this->theGroup;
  sr.generatorS.setSize(this->theGroup->generators.size);
  // in TODO: make random FiniteGroups capable of finding their conjugacy classes in less than at least 5 minutes
  global.Comments << "inducing from subgroup representation:\n";
  for (int i = 0; i < this->theSubgroup->generators.size; i ++) {
    global.Comments << this->theSubgroup->generators[i] << '\n' << in.generatorS[i].ToStringPlainText() << '\n';
  }
  for (int i = 0; i < this->theGroup->generators.size; i ++) {
    elementSomeGroup g;
    int csi = this->GetCosetId(this->theGroup->generators[i]);
    if (csi == 0) {
      g = this->theGroup->generators[i];
    } else {
      elementSomeGroup cg = cosets[csi].representative;
      cg.Invert();
      g = cg * this->theGroup->generators[i];
    }
    global.Comments << "element " << this->theGroup->generators[i] << " belongs to coset " << csi;
    global.Comments << " represented by " << cosets[csi].representative << " and corresponds to subgroup element " << g;
    in.GetMatrixOfElement(g, sr.generatorS[i]);
    global.Comments << " which is assigned matrix\n" << sr.generatorS[i].ToStringPlainText() << '\n';
  }
  GroupRepresentation<someGroup, coefficient> out;
  out.MakeTensorRepresentation(qr,sr);
  global.Comments << "Subgroup representation: " << sr.toString() << "\n";
  for (int i = 0; i < this->theGroup->generators.size; i ++) {
    global.Comments << this->theGroup->generators[i] << ' ' << sr.generatorS[i].GetTrace() << '\n'
    << sr.generatorS[i].ToStringPlainText() << '\n';
  }
  global.Comments << "Quotient representation: " << qr.toString() << "\n";
  for (int i = 0; i < this->theGroup->generators.size; i ++) {
    global.Comments << this->theGroup->generators[i] << ' ' << qr.generatorS[i].GetTrace() << '\n'
    << qr.generatorS[i].ToStringPlainText() << '\n';
  }
  global.Comments << "Induced representation: " << out.toString() << '\n';
  for (int i = 0; i <out.generatorS.size; i ++) {
    global.Comments << this->theGroup->generators[i] << ' ' << out.generatorS[i].GetTrace() << '\n'
    << out.generatorS[i].ToStringPlainText() << '\n';
  }
  return out;
}

template <typename someGroup, typename elementSomeGroup>
template <typename coefficient>
GroupRepresentation<someGroup, coefficient> SubgroupData<someGroup, elementSomeGroup>::InduceRepresentation(
  GroupRepresentation<someGroup, coefficient>& in
) {
  GroupRepresentation<someGroup, coefficient> out;
  out.generatorS.setSize(this->theGroup->generators.size);
  for (int i = 0; i < this->theGroup->generators.size; i ++) {
    this->ComputeCosets();
    // parent->generators[i] = cg * h
    // g1 * g2 = cg1 * h1 * cg2 * h2
    elementSomeGroup g = this->theGroup->generators[i];

    if (in.generatorS.size == 0) {
      out.generatorS[i].MakeZeroMatrix(1 * this->cosets.size);
    } else {
      out.generatorS[i].MakeZeroMatrix(in.generatorS[0].NumRows * this->cosets.size);
    }
    for (int ci = 0; ci < cosets.size; ci ++) {
      elementSomeGroup k = g * cosets[ci].representative;
      int kcsi = this->GetCosetId(k);
      elementSomeGroup ck = cosets[kcsi].representative;
      elementSomeGroup cki = ck;
      cki.Invert();
      elementSomeGroup hk = cki * k;
      Matrix<coefficient> ikblock;
      in.GetMatrixOfElement(hk, ikblock);
      out.generatorS[i].AssignBlock(ikblock, kcsi * ikblock.NumRows, ci * ikblock.NumCols);
    }
  }
  out.ownerGroup = this->theGroup;
  for (int i = 0; i <out.generatorS.size; i ++) {
    global.Comments << this->theGroup->generators[i] << ' ' << out.generatorS[i].GetTrace() << '\n'
    << out.generatorS[i].ToStringPlainText() << '\n';
  }
  if (!out.VerifyRepresentation()) {
    if (!in.VerifyRepresentationExpensive()) {
      global.Comments << "Well, we weren't given a proper representation either.";
    }
    FiniteGroup<Matrix<coefficient> > ingroup;
    ingroup.generators = in.generatorS;
    global.Comments << "Generator commutation relations for input representation:\n"
    << ingroup.PrettyPrintGeneratorCommutationRelations();
    global.Comments << "a quotient group of\n" << this->theSubgroup->PrettyPrintGeneratorCommutationRelations();
    FiniteGroup<Matrix<coefficient> > outgroup;
    outgroup.generators = out.generatorS;
    global.Comments << "Generator commutation relations for 'representation':\n"
    << outgroup.PrettyPrintGeneratorCommutationRelations();
    global.Comments << "It was supposed to be a quotient group of\n" << this->theGroup->PrettyPrintGeneratorCommutationRelations();
    global.fatal << "Error in InduceRepresentation. " << global.fatal;
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
  bool CheckInitialization();
  void ComputeTauSignature();
  void GetSignCharacter(Vector<Rational>& out);
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
  void InitGenerators();
  void MakeParabolicSubgroup(WeylGroupData& G, const Selection& inputGeneratingSimpleRoots);
  void MakeFromRoots(WeylGroupData& G, const Vectors<Rational>& inputRootReflections);
  LargeInteger SizeByFormulaOrNeg1() const {
    return this->theDynkinType.GetWeylGroupSizeByFormula();
  }
  void MakeFromHardCodedStrings(
    WeylGroupData& G,
    const std::string& inputParabolicity,
    const std::string& inputType,
    const std::string& inputIndicesGeneratingRoots,
    const std::string& inputTauSignature
  );
  void ComputeDynkinType();
  void ComputeCCSizesRepresentativesPreimages();
  SubgroupDataRootReflections();
  std::string toString(FormatExpressions* theFormat = nullptr);
};

class SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms {
public:
  bool truncated;
  bool flagDeallocated;
  WeylGroupData* AmbientWeyl;
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
  bool CheckInitialization();
  void toString(std::string& output, bool displayElements);
  void GetGroupElementsIndexedAsAmbientGroup(List<ElementWeylGroup>& output);
  std::string ElementToStringBruhatGraph();
  std::string ElementToStringCosetGraph();
  std::string ElementToStringFromLayersAndArrows(
    List<List<List<int> > >& arrows, List<List<int> >& Layers, int GraphWidth, bool useAmbientIndices
  );
  std::string toString(bool displayElements = true) {
    std::string tempS;
    this->toString(tempS, displayElements);
    return tempS;
  }
  Vector<Rational> GetRho();
  template <class coefficient>
  void RaiseToDominantWeightInner(
    Vector<coefficient>& theWeight, int* sign = nullptr, bool* stabilizerFound = nullptr
  );
  template <class coefficient>
  bool FreudenthalEvalIrrepIsWRTLeviPart(
    const Vector<coefficient>& inputHWfundamentalCoords,
    HashedList<Vector<coefficient> >& outputDominantWeightsSimpleCoords,
    List<coefficient>& outputMultsSimpleCoordS,
    std::string& outputDetails,
    int UpperBoundFreudenthal
  );
  bool MakeParabolicFromSelectionSimpleRoots(
    WeylGroupData& inputWeyl, const Selection& ZeroesMeanSimpleRootSpaceIsInParabolic, int UpperLimitNumElements
  );
  void MakeParabolicFromSelectionSimpleRoots(
    WeylGroupData& inputWeyl, const Vector<Rational>& ZeroesMeanSimpleRootSpaceIsInParabolic, int UpperLimitNumElements
  );
  bool GetAlLDominantWeightsHWFDIMwithRespectToAmbientAlgebra(
    Vector<Rational>& highestWeightSimpleCoords,
    HashedList<Vector<Rational> >& outputWeightsSimpleCoords,
    int upperBoundDominantWeights,
    std::string& outputDetails
  );
  template <class coefficient>
  bool GetAlLDominantWeightsHWFDIM(
    Vector<coefficient>& highestWeightSimpleCoords,
    HashedList<Vector<coefficient> >& outputWeightsSimpleCoords,
    int upperBoundDominantWeights,
    std::string& outputDetails
  );
  bool DrawContour(
    const Vector<Rational>& highestWeightSimpleCoord,
    DrawingVariables& theDV,
    const std::string& color,
    int UpperBoundVertices
  );
  //The body of this function must
  //appear after the definitions of IsDominantWRTgenerator.
  //This appears to be a quirk in the compiler / c ++ specification.
  template <class coefficient>
  bool IsDominantWeight(const Vector<coefficient>& theWeight);
  template <class coefficient>
  bool IsDominantWRTgenerator(const Vector<coefficient>& theWeight, int generatorIndex);
  template <class coefficient>
  coefficient WeylDimFormulaInnerSimpleCoords(
    const Vector<coefficient>& theWeightInnerSimpleCoords, const coefficient& theRingUnit = 1
  );
  void FindQuotientRepresentatives(int UpperLimit);
  void GetMatrixOfElement(
    const ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms& input, Matrix<Rational>& outputMatrix
  ) const;
  template <class coefficient>
  bool GenerateOrbitReturnFalseIfTruncated(
    const Vector<coefficient>& input, Vectors<coefficient>& outputOrbit, bool restrictToInner, int UpperLimitNumElements
  );
  bool ComputeSubGroupFromGeneratingReflections(
    Vectors<Rational>* inputRoots,
    List<Vectors<Rational> >* inputExternalAutos,
    int UpperLimitNumElements,
    bool recomputeAmbientRho
  );
  void ComputeRootSubsystem();
  template <class coefficient>
  void ActByNonSimpleElement(int index, Vector<coefficient>& inputOutput) const;
  template <class coefficient>
  void ActByNonSimpleElement(int index, Vector<coefficient>& input, Vector<coefficient>& output) const;
  template <class coefficient>
  void ActByElement(
    const ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms& theElement,
    const Vector<coefficient>& input,
    Vector<coefficient>& output
  ) const;
  template <class coefficient>
  void ActByElement(
    const ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms& theElement,
    const Vectors<coefficient>& input,
    Vectors<coefficient>& output
  ) const;
  template <class coefficient>
  void ActByNonSimpleElement(int index, const Vectors<coefficient>& input, Vectors<coefficient>& output) const {
    this->ActByElement(this->allElements[index], input, output);
  }
};

template <typename coefficient>
class ElementWeylGroupRing : public ElementMonomialAlgebra<ElementWeylGroup, coefficient> {
public:
  void MakeEi(WeylGroupData* G, int i);
  void MakeFromClassFunction(WeylGroupData* G, const List<coefficient>& l);
  void MakeFromClassFunction(const ClassFunction<WeylGroupData, coefficient>& l);
};

template <typename coefficient>
std::ostream& operator<<(std::ostream& out, const ElementWeylGroupRing<coefficient> g);

template <typename coefficient>
void ElementWeylGroupRing<coefficient>::MakeEi(WeylGroupData* GG, int i) {
  ElementWeylGroup theMon;
  theMon = GG->theGroup.theElements[i];
  *this = theMon;
}

template <typename coefficient>
void ElementWeylGroupRing<coefficient>::MakeFromClassFunction(const ClassFunction<WeylGroupData, coefficient>& l) {
  MakeFromClassFunction(l.G, l.data);
}

template <typename coefficient>
void ElementWeylGroupRing<coefficient>::MakeFromClassFunction(WeylGroupData* GG, const List<coefficient>& l) {
  MacroRegisterFunctionWithName("ElementWeylGroupRing::MakeFromClassFunction");
  if (GG == nullptr) {
    global.fatal << "Weyl group pointer not allowed to be zero. " << global.fatal;
  }
  this->makeZero();
  ElementWeylGroup theMon;
  for (int i = 0; i < GG->theGroup.ConjugacyClassCount(); i ++) {
    if (l[i] != 0) {
      for (int j = 0; j < GG->theGroup.conjugacyClasseS.size; j ++) {
        this->AddMonomial(GG->theGroup.conjugacyClasseS[i].theElements[j], l[i]);
      }
    }
  }
}

//Matrix<Element>
template <typename coefficient>
std::ostream& operator<<(std::ostream& out, const ElementWeylGroupRing<coefficient> g) {
  out << g.data;
  return out;
}

//--------------------------------Finite group characters----------------------------

template<class someFiniteGroup, typename coefficient>
coefficient ClassFunction<someFiniteGroup, coefficient>::InnerProduct(const ClassFunction<someFiniteGroup, coefficient>& other) const {
  return this->G->GetHermitianProduct(this->data, other.data);
}

template<class someFiniteGroup, typename coefficient>
coefficient ClassFunction<someFiniteGroup, coefficient>::Norm() const {
  return this->InnerProduct(*this);
}

template<class someFiniteGroup, typename coefficient>
ClassFunction<someFiniteGroup, coefficient> ClassFunction<someFiniteGroup, coefficient>::operator*(const ClassFunction<someFiniteGroup, coefficient>& other) const {
  ClassFunction<someFiniteGroup, coefficient> result = *this;
  result *= other;
  return result;
}

template<class someFiniteGroup, typename coefficient>
void ClassFunction<someFiniteGroup, coefficient>::operator*= (const ClassFunction<someFiniteGroup, coefficient>& right) {
  if (this->G != right.G) {
    global.fatal << "Attempting to multiply class functions belonging to different groups." << global.fatal;
  }
  for (int i = 0; i < this->data.size; i ++) {
    this->data[i] *= right[i];
  }
}

template<class someFiniteGroup, typename coefficient>
ClassFunction<someFiniteGroup, coefficient> ClassFunction<someFiniteGroup, coefficient>::Sym2() const {
  ClassFunction<someFiniteGroup, coefficient> l;
  l.G = G;
  l.data.setExpectedSize(G->ConjugacyClassCount());
  for (int i = 0; i < G->ConjugacyClassCount(); i ++) {
    l.data.addOnTop((this->data[i] * this->data[i] + this->data[G->squaresFirstConjugacyClassRep[i]]) / 2);
  }
  return l;
}

template<class someFiniteGroup, typename coefficient>
ClassFunction<someFiniteGroup, coefficient> ClassFunction<someFiniteGroup, coefficient>::Alt2() const {
  ClassFunction<someFiniteGroup, coefficient> l;
  l.G = G;
  l.data.setExpectedSize(G->ConjugacyClassCount());
  for (int i = 0; i < G->ConjugacyClassCount(); i ++) {
    l.data.addOnTop((this->data[i] * this->data[i] - this->data[G->squaresFirstConjugacyClassRep[i]]) / 2);
  }
  return l;
}

template<class someFiniteGroup, typename coefficient>
ClassFunction<someFiniteGroup, coefficient> ClassFunction<someFiniteGroup, coefficient>::operator+(
  const ClassFunction<someFiniteGroup, coefficient>& other
) const {
  ClassFunction<someFiniteGroup, coefficient> l = *this;
  l.data += other.data;
  return l;
}

template<class someFiniteGroup, typename coefficient>
ClassFunction<someFiniteGroup, coefficient> ClassFunction<someFiniteGroup, coefficient>::operator-(
  const ClassFunction& other
) const {
  ClassFunction<someFiniteGroup, coefficient> l = *this;
  l.data += other.data;
  return l;
}

template<class someFiniteGroup, typename coefficient>
ClassFunction<someFiniteGroup, coefficient> ClassFunction<someFiniteGroup, coefficient>::ReducedWithChars(
  const List<ClassFunction<someFiniteGroup, coefficient> >& cchars
) {
  ClassFunction<someFiniteGroup, coefficient> X = *this;
  if (X.Norm() == 0) {
    return X;
  }
  List<ClassFunction<someFiniteGroup, coefficient> > chars;
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
        coefficient n = X2[0];
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
        if (X2.Norm() < X.Norm()) {
          X = X2;
          if (X.Norm() == 0) {
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
template <typename coefficient>
class UDPolynomial {
public:
   // "So the last shall be first, and the first last" -- Matthew 20:12
  List<coefficient> data;
  UDPolynomial() {
  }
  UDPolynomial(const UDPolynomial<coefficient>& other) {
    this->data = other.data;
  }
  UDPolynomial& operator=(const UDPolynomial<coefficient>& other) {
    this->data = other.data;
    return *this;
  }
  // UDPolynomial<coefficient> operator+(const UDPolynomial<coefficient>& right) const;
  void operator+=(const UDPolynomial<coefficient>& right);
  // UDPolynomial<coefficient> operator-(const UDPolynomial<coefficient>& right) const;
  void operator-=(const UDPolynomial<coefficient>& right);
  UDPolynomial<coefficient> operator*(const UDPolynomial<coefficient>& right) const;
  //  UDPolynomial<coefficient> operator*(const coefficient& right) const;
  void operator*=(const coefficient& right);
  void operator*=(const UDPolynomial<coefficient>& other) {
    *this = (*this) * other;
  }

  UDPolynomial<coefficient> TimesXn(int n) const;
  // Quick divisibility test
  // bool DivisibleBy(const UDPolynomial<coefficient>& divisor) const;
  struct DivisionResult<UDPolynomial<coefficient> > DivideBy(const UDPolynomial<coefficient>& right) const;
  UDPolynomial<coefficient> operator/(const UDPolynomial<coefficient>& divisor) const;
  UDPolynomial<coefficient> operator%(const UDPolynomial<coefficient>& divisor) const;
  void operator/=(const coefficient& right);
  void operator/=(const UDPolynomial<coefficient>& right) {
    *this = *this / right;
  }
  coefficient operator()(const coefficient& x) const;
  void ClearDenominators();
  void FormalDerivative();
  void SquareFree();
  List<coefficient> GetRoots() const;
  void DoKronecker() const;
  coefficient& operator[](int i) const;
  bool operator<(const UDPolynomial<coefficient>& right) const;
  bool operator==(int other) const;
  std::string toString(FormatExpressions* theFormat = nullptr) const;
  void AssignMinPoly(const Matrix<coefficient>& input);
  void AssignCharPoly(const Matrix<coefficient>& input); // method due to Urbain Le Verrier
};

template <typename coefficient>
void UDPolynomial<coefficient>::AssignMinPoly(const Matrix<coefficient>& input) {
  int n = input.NumCols;
  this->data.setSize(1);
  this->data[0] = 1;
  for (int col = 0; col < n; col ++) {
    VectorSpace<coefficient> vs;
    Vector<coefficient> v, w;
    v.MakeEi(n, col);
    vs.AddVectorToBasis(v);
    for (int i = 0; i < n; i ++) {
      w = input * v;
      if (!vs.AddVectorToBasis(w)) {
        break;
      }
      v = w;
    }
    Vector<coefficient> p = vs.basis.PutInBasis(w);
    UDPolynomial<coefficient> out;
    out.data.setSize(p.size + 1);
    for (int i = 0; i <p.size; i ++) {
      out.data[i] = - p[i];
    }
    out.data[p.size] = 1;
    *this = MathRoutines::lcm(*this, out);
  }
}

template <typename coefficient>
void UDPolynomial<coefficient>::AssignCharPoly(const Matrix<coefficient>& input) {
  int n = input.NumCols;
  this->data.setSize(n + 1);
  this->data[0] = 1;
  Matrix<coefficient> acc;
  acc = input;
  for (int i = 1; i < n; i ++) {
    this->data[i] = - acc.GetTrace() / i;
    for (int j = 0; j < n; j ++) {
      acc.elements[j][j] += this->data[i];
    }
    acc.MultiplyOnTheLeft(input);
  }
  this->data[n] = - acc.GetTrace() / n;
}

template <typename coefficient>
coefficient& UDPolynomial<coefficient>::operator[](int i) const {
  return data[i];
}

template <typename coefficient>
coefficient UDPolynomial<coefficient>::operator()(const coefficient& x) const {
  coefficient acc = 0;
  coefficient y = 1;
  for (int i = 0; i < data.size; i ++) {
    acc += y * data[i];
    y *= x;
  }
  return acc;
}

template <typename coefficient>
void UDPolynomial<coefficient>::operator+=(const UDPolynomial<coefficient>& right) {
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

template <typename coefficient>
void UDPolynomial<coefficient>::operator-=(const UDPolynomial<coefficient>& right) {
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

template <typename coefficient>
UDPolynomial<coefficient> UDPolynomial<coefficient>::operator*(const UDPolynomial<coefficient>& right) const {
  UDPolynomial<coefficient> out;
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

template <typename coefficient>
UDPolynomial<coefficient> UDPolynomial<coefficient>::TimesXn(int n) const {
  UDPolynomial<coefficient> out;
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

template <typename coefficient>
void UDPolynomial<coefficient>::operator*=(const coefficient& right) {
  for (int i = 0; i < data.size; i ++) {
    data[i] *= right;
  }
}

template <class coefficient>
std::string UDPolynomial<coefficient>::toString(FormatExpressions* theFormat) const {
  Polynomial<coefficient> tempP;
  tempP.makeZero();
  MonomialP tempM;
  for (int i = 0; i < this->data.size; i ++) {
    tempM.MakeEi(0, i, 1);
    tempP.AddMonomial(tempM, this->data[i]);
  }
  return tempP.toString(theFormat);
}

template <typename coefficient>
std::ostream& operator<<(std::ostream& out, const UDPolynomial<coefficient>& p) {
  FormatExpressions tempFormat;
  tempFormat.polyAlphabeT.setSize(1);
  tempFormat.polyAlphabeT[0] = "q";
  return out << p.toString(&tempFormat);
}

template <typename elementSomeGroup>
int FiniteGroup<elementSomeGroup>::MultiplyElements(int indexLeft, int indexRight) const {
  return this->theElements.GetIndex(this->theElements[indexLeft] * this->theElements[indexRight]);
}

template <typename elementSomeGroup>
int FiniteGroup<elementSomeGroup>::Invert(int g) const {
  return this->theElements.GetIndex(this->theElements[g].Inverse());
}

template <class elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::RegisterCCclass(
  const elementSomeGroup& theRepresentative, bool dontAddIfSameInvariants
) {
  MacroRegisterFunctionWithName("FiniteGroup::RegisterCCclass");
  ConjugacyClass theClass;
  theClass.representative = theRepresentative;
  Polynomial<Rational> theCharPoly;
  theClass.representative.GetCharacteristicPolyStandardRepresentation(theCharPoly);
  if (this->CCsStandardRepCharPolys.Contains(theCharPoly)) {
    const List<int>& indicesPossibleConjugates =
    this->CCsStandardRepCharPolys.GetHashArray(this->CCsStandardRepCharPolys.GetHash(theCharPoly));
    for (int i = 0; i < indicesPossibleConjugates.size; i ++) {
      elementSomeGroup& otherRepresentative = this->conjugacyClasseS[indicesPossibleConjugates[i]].representative;
      if (!dontAddIfSameInvariants) {
        if (this->AreConjugate(theClass.representative, otherRepresentative)) {
          return false;
        }
      } else {
        if (!theClass.representative.HasDifferentConjugacyInvariantsFrom(otherRepresentative)) {
          return false;
        }
      }
    }
  }
  theClass.flagRepresentativeComputed = true;
  this->ComputeCCSizeOrCCFromRepresentative(theClass, false);
  this->conjugacyClasseS.addOnTop(theClass);
  this->CCsStandardRepCharPolys.addOnTop(theCharPoly);
  this->sizePrivate += theClass.size;
  this->conjugacyClasseS.QuickSortAscending();
  return true;
}

template <typename elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::GetWord(const elementSomeGroup& g, List<int>& word) {
  if (this->GetWordByFormula != nullptr) {
    return this->GetWordByFormula(*this, g, word);
  }
  if (!this->flagWordsComputed) {
    this->ComputeAllElementsLargeGroup(true);
  }
  int index = this->theElements.GetIndex(g);
  if (index == - 1) {
    return false;
  }
  word = this->theWords[index];
  return true;
}

template <class elementSomeGroup>
LargeInteger FiniteGroup<elementSomeGroup>::GetSize() {
  this->CheckConsistency();
  if (this->sizePrivate > 0) {
    return sizePrivate;
  }
  if (this->GetSizeByFormula != 0) {
    this->sizePrivate = this->GetSizeByFormula(*this);
    return sizePrivate;
  }
  this->ComputeAllElements(false);
  return this->sizePrivate;
}

template <typename elementSomeGroup>
bool FiniteGroup<elementSomeGroup>::CheckInitializationConjugacyClasses() const {
  if (this->ConjugacyClassCount() == 0) {
    global.fatal << "This is a programming error: requesting to "
    << "compute character hermitian product in a group whose "
    << "conjugacy classes and/or elements have not been computed. The group reports to have "
    << this->ConjugacyClassCount() << " conjugacy classes and " << this->theElements.size << " elements. "
    << global.fatal;
    return false;
  }
  for (int i = 0; i < this->irreps.size; i ++) {
    if (this->irreps[i].theCharacteR.data.IsEqualToZero()) {
      global.fatal << "This is a programming error: irrep number " << i + 1 << " has zero character!" << global.fatal;
      return false;
    }
  }
/*  Rational sumSquares = 0;
  Rational tempRat;
  for (int i = 0; i < this->ConjugacyClassCount(); i ++) {
    tempRat = this->ConjugacyClassCount()/this->GetGroupSizeByFormula();
    sumSquares+= tempRat*tempRat;
  }
  if (sumSquares !=1)
    global.fatal << "This is a programming error: sumSquares equals " << sumSquares.toString() << " when it should equal 1. " << global.fatal;*/
  return true;
}

#endif
