//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfCharacterHeader_alreadyIncluded
#define vpfCharacterHeader_alreadyIncluded
#include "vpfHeader1General0_General.h"
#include "vpfHeader2Math0_General.h"
#include "vpfHeader2Math8_VectorSpace.h"
#include "vpfHeader2Math4_Graph.h"
#include "vpfJson.h"

//To do: move Weyl groups to this file. Eliminate all redundant code and organize nicely.

static ProjectInformationInstance ProjectInfoVpfFiniteGroups(__FILE__, "Header file, finite groups. Work in progress.");

template <typename ElementEuclideanDomain>
struct DivisionResult {
  ElementEuclideanDomain quotient;
  ElementEuclideanDomain remainder;
};

template<class someFiniteGroup, typename coefficient>
class ClassFunction {
public:
  someFiniteGroup *G;
  Vector<coefficient> data;
  ClassFunction(): G(0) {
  } //the syntax :G(0) initializes the pointer G with 0.
  //Although there may be a minor speed penalty
  //(such a speed penalty is system dependent and possibly
  //even hardware dependent),
  //it is worth it to initialize all pointers with 0.
  void MakeZero(someFiniteGroup& inputWeyl);
  bool IsEqualToZero() const {
    return this->data.IsEqualToZero();
  }
  bool IsMonEqualToZero() const {
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
  std::string ToString(FormatExpressions* theFormat = 0) const;
  static unsigned int HashFunction(const ClassFunction& input);
  inline unsigned int HashFunction() const {
    return this->HashFunction(*this);
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
    out << data.ToString();
    return out;
  }
};

template <typename someGroup, typename coefficient>
class GroupRepresentation;

template <typename someGroup, typename coefficient>
class GroupRepresentationCarriesAllMatrices;

template <typename someGroup, typename elementSomeGroup>
class SubgroupData;

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
// unsigned int HashFunction() const;
//
// To make certain operations go faster, it might be nice to put smarter algorithms
// in the function pointers in the FiniteGroup class
//
//assumptions on the FiniteGroup:
//1. The finite group has a small number of conjugacy classes. Z/10000000Z is not OK
//
// An uninitialized element is expected to be the identity element to the full
// extent allowed by law.  Two uninitialized elements can be expected to multiply,
// an an uninitialized element can multiply by an initialized element and where
// needed thereby discover what group it belongs to.  Equality and sorting operators
// and the HashFunction need to know what the identity is representation
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
  LargeInt sizePrivate;

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
      output << theClass.ToString();
      return output;
    }
  public:
    ConjugacyClass() {
      this->flagRepresentativeComputed = false;
      this->flagElementsComputed = false;
    }
    LargeInt size;
    List<int> indicesEltsInOwner;
    int representativeIndex;
    List<elementSomeGroup> theElements;
    elementSomeGroup representative;
    bool flagRepresentativeComputed;
    bool flagElementsComputed;
    bool flagRepresentativeWordComputed;
    List<int> representativeWord;
    std::string ToString(FormatExpressions* theFormat = 0) const {
      (void) theFormat;//avoid unused parameter warning, portable
      std::stringstream out;
      out << "Conj. class size: " << this->size.ToString();
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

  //<-The character polynomials in the ``standard representation''.
  //The ``standard representation'' is specified by the elementSomeGroup class.
  //It is up to the user of the FiniteGroup template to define which representation is
  //``standard'' - this is not a serious restriction as one can always choose the trivial representation.
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
  virtual ~FiniteGroup() {
    this->flagDeallocated = true;
  }
  virtual void InitGenerators() {
  }
  bool CheckConsistency() const {
    if (this->flagDeallocated) {
      crash << "This is a programming error: use after free of Finite group. " << crash;
    }
    //if (this == 0)
    //  crash << "Finite group this pointer is zero. " << crash;
    return true;
  }
  bool CheckInitialization() const;
  // WARNING:  This recalculates conjugacy classes, screwing up their order for other methods
  bool CheckConjugacyClassRepsMatchCCsizes();
  bool CheckOrthogonalityCharTable();
  void SetSizE(const LargeInt& inputSize) {
    this->sizePrivate = inputSize;
  }
  void init();
  std::string ToString(FormatExpressions* theFormat = 0);
  std::string ToStringElements(FormatExpressions* theFormat = 0) const;
  std::string ToStringConjugacyClasses(FormatExpressions* theFormat = 0);
  int ConjugacyClassCount() const;
  LargeInt GetSize();
  LargeInt SizeByFormulaOrNeg1() {
    this->CheckConsistency();
    if (this->GetSizeByFormula != 0) {
      return GetSizeByFormula(*this);
    }
    return - 1;
  } //non-positive result means no formula is known.

  bool (*AreConjugateByFormula)(const elementSomeGroup& x, const elementSomeGroup& y);
  void (*ComputeCCSizesAndRepresentativesByFormula)(FiniteGroup<elementSomeGroup>& G);
  LargeInt (*GetSizeByFormula)(FiniteGroup<elementSomeGroup>& G);
  bool AreConjugate(const elementSomeGroup& left, const elementSomeGroup& right);
  bool AreConjugate_OLD_Deprecated_Version_By_Todor(const elementSomeGroup& left, const elementSomeGroup& right);
  bool ComputeAllElements(bool andWords, int MaxElements = 1000000);
  //MaxElements gives an upper bound to the number of elements this function will try to find.
  //Set MaxElements = - 1 for "unlimited" upper bound.
  bool ComputeAllElementsLargeGroup(bool andWords, int MaxElements = 1000000);
  //MaxElements gives an upper bound to the number of elements this function will try to find.
  //Set MaxElements = - 1 for "unlimited" upper bound.

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
  void MakeID(elementSomeGroup& x);
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

  // A parabolic kinda subgroup H = <hᵢ> is a subgroup of G = <gᵢ> in which for h∈H,
  // there is a canonical word in gᵢ..gₖ for h in G, with the property that the gᵢ's
  // in h are actually hᵢ's.  Where this happens, it makes it much easier to come up
  // with words in H, to be used for representation theory
  // The intended use for this API call is
  // auto H = G.ParabolicKindaSubgroupGeneratorSubset(subgenids);
  SubgroupData<FiniteGroup<elementSomeGroup>, elementSomeGroup> ParabolicKindaSubgroupGeneratorSubset(const List<int>& subgens);

  // auto M = G.GetEmptyRationalRepresentation()
  GroupRepresentation<FiniteGroup<elementSomeGroup>, Rational> GetEmptyRationalRepresentation();

  // This is a problem, not everything can have a ToString, but things need ToString(globalVariables) called
  // Personally, I think the output class should have scalars special cased and all MilevObject types have
  // their own ToString, which is exclusively called from the output class's operator<<, and the formatting
  // parts of globalVariables are carried by the object of the output class that is surreptitiously passed
  // around.  I mean, not that left shifting some random object by a string is a good design anyway
  friend std::ostream& operator<<(std::ostream& out, FiniteGroup<elementSomeGroup>& data) {
    out << data.ToString(0);
    return out;
  }
};

struct simpleReflectionOrOuterAuto {
  bool flagIsOuter;
  int index;
  simpleReflectionOrOuterAuto(): flagIsOuter(false), index(- 1) {
   
  }
  std::string ToString() const;
  void MakeOuterAuto(int inputIndex) {
    this->flagIsOuter = true;
    this->index = inputIndex;
  }
  void MakeSimpleReflection(int inputIndex) {
    this->flagIsOuter = false;
    this->index = inputIndex;
  }
  unsigned int HashFunction() const {
    return index + 100 * flagIsOuter;
  }
  bool operator== (const simpleReflectionOrOuterAuto& other) const {
    return this->flagIsOuter == other.flagIsOuter && this->index == other.index;
  }
  bool operator>(const simpleReflectionOrOuterAuto& right) const {
    if (right.flagIsOuter != this->flagIsOuter) {
      return this->flagIsOuter > right.flagIsOuter;
    }
    return this->index>right.index;
  }
};

// templateWeylGroup needs to be templateWeylData now
template <class templateWeylGroup>
class ElementWeylGroup {
  friend std::ostream& operator << (std::ostream& output, const ElementWeylGroup<templateWeylGroup>& theElement) {
    output << theElement.ToString();
    return output;
  }
public:
  templateWeylGroup* owner;
  List<simpleReflectionOrOuterAuto> generatorsLastAppliedFirst;
  bool flagDeallocated;
  ElementWeylGroup(const ElementWeylGroup& other) {
    this->flagDeallocated = false;
    this->operator=(other);
  }
  ElementWeylGroup(): owner(0), flagDeallocated(false) {
   
  }
  ~ElementWeylGroup() {
    this->flagDeallocated = true;
  }
  void operator= (const ElementWeylGroup& other) {
    this->owner = other.owner;
    this->generatorsLastAppliedFirst = other.generatorsLastAppliedFirst;
  }
  template<class coefficient>
  static void ActOn(const ElementWeylGroup& inputElt, const Vector<coefficient>& inputV, Vector<coefficient>& output) {
    inputElt.CheckInitialization();
    inputElt.owner->ActOn(inputElt, inputV, output);
  }
  template<class coefficient>
  void ActOn(Vector<coefficient>& inputOutput) const {
    this->CheckInitialization();
    this->owner->ActOn(*this, inputOutput, inputOutput);
  }
  bool CheckConsistency() const {
    if (this->flagDeallocated) {
      crash << "Programming error: use after free of class ElementWeylGroup" << crash;
    }
    return true;
  }
  bool CheckInitialization() const {
    this->CheckConsistency();
    if (this->owner == 0) {
      crash << "This is a programming error: non-initialized element Weyl group. " << crash;
      return false;
    }
    this->owner->CheckConsistency();
    return true;
  }
  int Sign() const {
    return this->generatorsLastAppliedFirst.size % 2 == 0 ? 1 : - 1;
  }
  void MultiplyOnTheRightBySimpleReflection(int reflectionIndex) {
    simpleReflectionOrOuterAuto theGen;
    theGen.MakeSimpleReflection(reflectionIndex);
    this->generatorsLastAppliedFirst.AddOnTop(theGen);
  }
  void MultiplyOnTheRightByOuterAuto(int outerAutoIndex) {
    simpleReflectionOrOuterAuto theGen;
    theGen.index = outerAutoIndex;
    theGen.flagIsOuter = true;
    this->generatorsLastAppliedFirst.AddOnTop(theGen);
  }
  static void ConjugationAction(
    const ElementWeylGroup& conjugateWith, const ElementWeylGroup& conjugateOn, ElementWeylGroup& output
  );
  ElementWeylGroup operator^(const ElementWeylGroup& right) const;
  void MakeFromRhoImage(const Vector<Rational>& inputRhoImage, WeylGroupData& inputWeyl);
  void MakeOuterAuto(int outerAutoIndex, WeylGroupData& inputWeyl);
  void MakeSimpleReflection(int simpleRootIndex, WeylGroupData& inputWeyl);
  void MakeRootReflection(const Vector<Rational>& mustBeRoot, WeylGroupData& inputWeyl);
  void MakeCanonical();
  // This is actually a very reasonable API call to have
  void MakeID(templateWeylGroup& inputWeyl);
  // but for consistency with the other group element type, we must also have
  void MakeID(const FiniteGroup<ElementWeylGroup<templateWeylGroup> >& inputGroup);
  void MakeID(const ElementWeylGroup& initializeFrom);
  bool IsID();
  std::string ToStringInvariants(FormatExpressions* theFormat) const;
  std::string ToString(FormatExpressions* theFormat = 0) const;
  unsigned int HashFunction() const;
  static inline unsigned int HashFunction(const ElementWeylGroup<templateWeylGroup>& input) {
    return input.HashFunction();
  }
  void operator*= (const ElementWeylGroup<WeylGroupData>& other);
  ElementWeylGroup<WeylGroupData> operator*(const ElementWeylGroup<WeylGroupData>& other) const {
    ElementWeylGroup<WeylGroupData> result = *this;
    result *= other;
    return result;
  }
  static inline bool IsMonEqualToZero() {
    return false;
  }
  void GetCycleStructure(VectorSparse<Rational>& outputIndexIsCycleSizeCoordinateIsCycleMult) const;
  Vector<Rational> operator*(const Vector<Rational>& v) const;
  ElementWeylGroup<WeylGroupData> Inverse() const;
  bool HasDifferentConjugacyInvariantsFrom(const ElementWeylGroup<templateWeylGroup>& right) const;
  void GetCharacteristicPolyStandardRepresentation(Polynomial<Rational>& output) const;
  bool operator==(const ElementWeylGroup<WeylGroupData>& other) const {
    if (this->owner != other.owner) {
      return false;
    }
    return this->generatorsLastAppliedFirst == other.generatorsLastAppliedFirst;
  }
  void MakeFromReadableReflections(
    WeylGroupData& input, bool dontMakeCanonical_SET_TRUE_ON_YOUR_OWN_RISK, const std::string& inputReflections
  );
  bool operator>(const ElementWeylGroup<templateWeylGroup>& other) const;
};

template <class coefficient>
class FinitelyGeneratedMatrixMonoid {
  public:
  List<MatrixTensor<coefficient> > theGenerators;
  HashedList<MatrixTensor<coefficient> > theElements;
  bool GenerateElements(int upperBoundNonPositiveMeansNoLimit);
  std::string ToString(FormatExpressions* theFormat = 0) const;
};

template <class coefficient>
bool FinitelyGeneratedMatrixMonoid<coefficient>::GenerateElements(int upperBoundNonPositiveMeansNoLimit) {
  MacroRegisterFunctionWithName("FinitelyGeneratedMatrixMonoid::GenerateElements");
  this->theElements.Clear();
  this->theElements.AddOnTopNoRepetition(theGenerators);
  MatrixTensor<coefficient> currentElement;
  for (int i = 0; i < this->theElements.size; i ++) {
    for (int j = 0; j < this->theGenerators.size; j ++) {
      currentElement = this->theGenerators[j];
      currentElement *= this->theElements[i];
      this->theElements.AddOnTopNoRepetition(currentElement);
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
  bool flagOuterAutosGeneratorsComputed;
  bool flagAllOuterAutosComputed;
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
  void operator= (const WeylGroupData& other);
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
  FiniteGroup<ElementWeylGroup<WeylGroupData> > theGroup;

  MemorySaving<FinitelyGeneratedMatrixMonoid<Rational> > theOuterAutos;
  List<std::string> ccCarterLabels;
  List<std::string> irrepsCarterLabels;
  //  void MakeFromParSel(Vector<Rational> & parSel, WeylGroup& input);
  bool flagDeallocated;

  typedef FiniteGroup<ElementWeylGroup<WeylGroupData> > WeylGroupBase;
  void init();
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
  void ComputeOrLoadCharacterTable(std::stringstream* reportStream = 0);
  void ComputeOrLoadConjugacyClasses(std::stringstream* reportStream = 0);
  static void ComputeIrreducibleRepresentationsWithFormulasImplementation(FiniteGroup<ElementWeylGroup<WeylGroupData> >& G);
  void ComputeExtremeRootInTheSameKMod(
    const Vectors<Rational>& inputSimpleBasisK,
    const Vector<Rational>& inputRoot,
    Vector<Rational>& output,
    bool lookingForHighest
  );
  void AddCharacter(const ClassFunction<WeylGroupData::WeylGroupBase, Rational>& X);
  void ComputeRho(bool Recompute);
  std::string ToStringRootsAndRootReflections(FormatExpressions* theFormat = 0);
  std::string ToString(FormatExpressions* theFormat = 0);
  std::string ToStringCppConjugacyClasses(FormatExpressions* theFormat = 0);
  std::string ToStringCppCharTable(FormatExpressions* theFormat = 0);
  std::string ToStringIrrepLabel(int irrepIndex);
  std::string ToStringSignSignatureRootSubsystem(const List<SubgroupDataRootReflections>& inputSubgroups);
  void MakeArbitrarySimple(char WeylGroupLetter, int n, const Rational* firstCoRootLengthSquared = 0);
  void MakeFromDynkinType(const DynkinType& inputType);
  void MakeFinalSteps();
  void InitGenerators();
  template <class coefficient>
  LargeInt GetOrbitSize(Vector<coefficient>& theWeight);
  void MakeMeFromMyCartanSymmetric();
  void MakeFromDynkinTypeDefaultLengthKeepComponentOrder(const DynkinType& inputType);
  void ComputeCoCartanSymmetricFromCartanSymmetric();
  void ComputeOuterAutoGenerators();
  void ComputeOuterAutos();
  bool CheckConsistency() const;
  static bool GetWordByFormulaImplementation(
    FiniteGroup<ElementWeylGroup<WeylGroupData> >& G, const ElementWeylGroup<WeylGroupData>& g, List<int>& out
  );
  void GetSignCharacter(Vector<Rational>& out);
  void GetStandardRepresentation(
    GroupRepresentationCarriesAllMatrices<FiniteGroup<ElementWeylGroup<WeylGroupData> >, Rational>& output
  );
  void GetSignRepresentation(
    GroupRepresentationCarriesAllMatrices<FiniteGroup<ElementWeylGroup<WeylGroupData> >, Rational>& output
  );
  void GetTrivialRepresentation(
    GroupRepresentationCarriesAllMatrices<FiniteGroup<ElementWeylGroup<WeylGroupData> >, Rational>& output
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
  Rational GetLongestRootLengthSquared();
  static unsigned int HashFunction(const WeylGroupData& input) {
    return input.CartanSymmetric.HashFunction();
  }
  unsigned int HashFunction() const {
    return this->HashFunction(*this);
  }
  WeylGroupData(): flagDeallocated(false) {
    this->init();
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
      crash << "This is a programming error. The input fundamental weight has "
      << result.size << " coordinates, while the rank of the Weyl group is "
      << this->GetDim() << ". " << crash;
    }
    for (int i = 0; i < result.size; i ++) {
      result[i] *= this->CartanSymmetric.elements[i][i] / 2;
    }
    return result;
  }
  template <class coefficient>
  coefficient GetScalarProdSimpleRoot(const Vector<coefficient>& input, int indexSimpleRoot) const {
    if (indexSimpleRoot < 0 || indexSimpleRoot >= this->GetDim()) {
      crash << "This is a programming error. Attempting to take scalar product with simple root of index "
      << indexSimpleRoot << " which is impossible, as the rank of the Weyl group is "
      << this->GetDim() << ". " << crash;
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
    int* sign = 0,
    bool* stabilizerFound = 0,
    ElementWeylGroup<WeylGroupData>* raisingElt = 0
  );
  bool AreMaximallyDominant(List<Vector<Rational> >& theWeights, bool useOuterAutos);
  template <class coefficient>
  void RaiseToMaximallyDominant(List<Vector<coefficient> >& theWeights, bool useOuterAutos);
  void GetCoxeterPlane(Vector<double>& outputBasis1, Vector<double>& outputBasis2);
  void GetSimpleReflectionMatrix(int indexSimpleRoot, Matrix<Rational>& output) const;
  ElementWeylGroup<WeylGroupData> SimpleConjugation(int i, const ElementWeylGroup<WeylGroupData>& vv);
  Matrix<Rational> SimpleReflectionMatrix(int indexSimpleRoot) const;
  void GetStandardRepresentationMatrix(int g, Matrix<Rational>& output) const;
  void DrawRootSystem(
    DrawingVariables& outputDV,
    bool wipeCanvas,
    bool drawWeylChamber,
    Vector<Rational>* bluePoint = 0,
    bool LabelDynkinDiagramVertices = false,
    Vectors<Rational>* predefinedProjectionPlane = 0
  );
  bool HasStronglyPerpendicularDecompositionWRT(
    Vector<Rational>& input,
    int UpperBoundNumBetas,
    Vectors<Rational>& theSet,
    Vectors<Rational>& output,
    List<Rational>& outputCoeffs,
    bool IntegralCoefficientsOnly
  );
  //  void MakeFromDynkinType(List<char>& theLetters, List<int>& theRanks, List<int>* theMultiplicities);
  //  void MakeFromDynkinType(List<char>& theLetters, List<int>& theRanks){ this->MakeFromDynkinType(theLetters, theRanks, 0); }
  //void GetLongRootLength(Rational& output);
  static bool IsAddmisibleDynkinType(char candidateLetter, int n);
  //the below will not do anything if the inputLetter is not a valid Dynkin letter
  static void TransformToAdmissibleDynkinType(char inputLetter, int& outputRank);
  void ComputeEpsilonMatrix() {
    if (this->MatrixSendsSimpleVectorsToEpsilonVectors.IsZeroPointer()) {
      this->theDynkinType.GetEpsilonMatrix(this->MatrixSendsSimpleVectorsToEpsilonVectors.GetElement());
    }
  }
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
  bool LeftIsHigherInBruhatOrderThanRight(ElementWeylGroup<WeylGroupData>& left, ElementWeylGroup<WeylGroupData>& right);
  bool IsElementWeylGroup(const MatrixTensor<Rational>& theMat);
  bool IsElementWeylGroupOrOuterAuto(const MatrixTensor<Rational>& theMat);
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
  std::string GenerateWeightSupportMethoD1
  (Vector<Rational>& highestWeightSimpleCoords, Vectors<Rational>& outputWeightsSimpleCoords, int upperBoundWeights);
  void GetIntegralLatticeInSimpleCoordinates(Lattice& output);
  void GetFundamentalWeightsInSimpleCoordinates(Vectors<Rational>& output);
  inline int GetDim() const {
    return this->CartanSymmetric.NumRows;
  }
  void ComputeWeylGroupAndRootsOfBorel(Vectors<Rational>& output);
  void ComputeRootsOfBorel(Vectors<Rational>& output);
  static LargeInt GetSizeByFormulaImplementation(FiniteGroup<ElementWeylGroup<WeylGroupData> >& G);
  static LargeInt SizeByFormulaOrNeg1(char weylLetter, int theDim);
  bool IsARoot(const Vector<Rational>& input) const {
    return this->RootSystem.Contains(input);
  }
  void GenerateRootSubsystem(Vectors<Rational>& theRoots);
  template <class coefficient>
  bool GenerateOuterOrbit(
    Vectors<coefficient>& theWeights,
    HashedList<Vector<coefficient> >& output,
    HashedList<ElementWeylGroup<WeylGroupData> >* outputSubset = 0,
    int UpperLimitNumElements = - 1,
    GlobalVariables* theGlobalVariables = 0
  );
  template <class coefficient>
  bool GenerateOrbit(
    Vectors<coefficient>& theWeights,
    bool RhoAction,
    HashedList<Vector<coefficient> >& output,
    bool UseMinusRho,
    int expectedOrbitSize = - 1,
    HashedList<ElementWeylGroup<WeylGroupData> >* outputSubset = 0,
    int UpperLimitNumElements = - 1
  );
  template <class coefficient>
  bool GenerateOrbit(
    Vector<coefficient>& theWeight,
    bool RhoAction,
    HashedList<Vector<coefficient> >& output,
    bool UseMinusRho,
    int expectedOrbitSize = - 1,
    HashedList<ElementWeylGroup<WeylGroupData> >* outputSubset = 0,
    int UpperLimitNumElements = - 1
  ) {
    Vectors<coefficient> theWeights;
    theWeights.AddOnTop(theWeight);
    return this->GenerateOrbit(
      theWeights, RhoAction, output, UseMinusRho, expectedOrbitSize, outputSubset, UpperLimitNumElements
    );
  }
//  int GetNumRootsFromFormula();
  void GenerateRootSystem();
  void WriteToFile(std::fstream& output);
  void ReadFromFile(std::fstream& input);
  void ActOnAffineHyperplaneByGroupElement(int index, affineHyperplane<Rational>& output, bool RhoAction, bool UseMinusRho);
  void ProjectOnTwoPlane(Vector<Rational>& orthonormalBasisVector1, Vector<Rational>& orthonormalBasisVector2);
  void GetLowestElementInOrbit(
    Vector<Rational>& inputOutput,
    ElementWeylGroup<WeylGroupData>* outputWeylElt,
    Vectors<Rational>& bufferEiBAsis,
    bool RhoAction,
    bool UseMinusRho,
    int* sign = 0,
    bool* stabilizerFound = 0
  ) {
    this->GetExtremeElementInOrbit(
      inputOutput, outputWeylElt, bufferEiBAsis, true, RhoAction, UseMinusRho, sign, stabilizerFound
    );
  }
  void GetHighestElementInOrbit(
    Vector<Rational>& inputOutput,
    ElementWeylGroup<WeylGroupData>* outputWeylElt,
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
    ElementWeylGroup<WeylGroupData>* outputWeylElt,
    Vectors<Rational>& bufferEiBAsis,
    bool findLowest,
    bool RhoAction,
    bool UseMinusRho,
    int* sign,
    bool* stabilizerFound
  );
  void GetLongestWeylElt(ElementWeylGroup<WeylGroupData>& outputWeylElt);
  bool IsEigenSpaceGeneratorCoxeterElement(Vector<Rational>& input);
  void GetCoxeterElement(ElementWeylGroup<WeylGroupData>& outputWeylElt) {
    outputWeylElt.generatorsLastAppliedFirst.SetSize(this->GetDim());
    for (int i = 0; i < outputWeylElt.generatorsLastAppliedFirst.size; i ++) {
      outputWeylElt.generatorsLastAppliedFirst[i].MakeSimpleReflection(i);
    }
  }
  Vector<Rational> ApplyReflectionList(const List<int>& rightReflectionsActFirst, const Vector<Rational>& vv) const;
  template <class coefficient>
  void ActOn(const ElementWeylGroup<WeylGroupData>& theGroupElement, Vector<coefficient>& inputOutput) const {
    this->ActOn(theGroupElement, inputOutput, inputOutput);
  }
  template <class coefficient>
  void ActOn(
    const ElementWeylGroup<WeylGroupData>& theGroupElement,
    const Vector<coefficient>& inputVector,
    Vector<coefficient>& outputVector
  ) const {
    //if (&inputVector == &outputVector)
    //{ Vector<coefficient> inputCopy = inputVector;
    //  this->ActOn(theGroupElement, inputCopy, outputVector);
    //  return;
    //}
    outputVector = inputVector;
    for (int i = theGroupElement.generatorsLastAppliedFirst.size - 1; i >= 0; i --) {
      if (!theGroupElement.generatorsLastAppliedFirst[i].flagIsOuter) {
        this->SimpleReflection(theGroupElement.generatorsLastAppliedFirst[i].index, outputVector);
      } else {
        if (!this->theOuterAutos.IsZeroPointer()) {
          this->theOuterAutos.GetElementConst().theGenerators[theGroupElement.generatorsLastAppliedFirst[i].index]
          .ActOnVectorColumn(outputVector, outputVector);
        } else {
          crash << "Weyl group of type " << this->theDynkinType.ToString()
          << " does not have its outer autos computed at a place where it should. " << crash;
        }
      }
    }
  }
  template <class coefficient>
  void ActOnRhoModified(const ElementWeylGroup<WeylGroupData>& theGroupElement, Vector<coefficient>& theVector) const {
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
  void ActOn(int indexOfWeylElement, Vector<coefficient>& theVector) const {
    this->ActOn(this->theGroup.theElements[indexOfWeylElement], theVector, theVector);
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
  ElementWeylGroup<WeylGroupData> GetRootReflection(int rootIndex);
  void GetWord(int g, List<int>& out) const;
  void GetWord(ElementWeylGroup<WeylGroupData> g, List<int>& out) const {
    out.SetSize(g.generatorsLastAppliedFirst.size);
    for (int i = 0; i <g.generatorsLastAppliedFirst.size; i ++) {
      if (g.generatorsLastAppliedFirst[i].flagIsOuter) {
        crash << "group::GetWord(element, List<int>) called on an element "
        << "that isn't described by a simple word in generators. "
        << "You probably meant to do something else, "
        << "and at any rate need to do some refactoring to do this. " << crash;
      }
      out[i] = g.generatorsLastAppliedFirst[i].index;
    }
  }
  Matrix<Rational> GetMatrixStandardRep(int elementIndex) const;
  void GetMatrixStandardRep(const ElementWeylGroup<WeylGroupData>& input, Matrix<Rational>& outputMatrix) const;
  Matrix<Rational> GetMatrixStandardRep(const ElementWeylGroup<WeylGroupData>& input) const {
    Matrix<Rational> result;
    this->GetMatrixStandardRep(input, result);
    return result;
  }
  void GetElementOfMatrix(Matrix<Rational>& input, ElementWeylGroup<WeylGroupData> &output);
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
      crash.theCrashReport << "This is a programming error: attempting to take the root system scalar product of "
      << "vectors of different dimension or of dimension different "
      << "from that of the ambient Lie algebra. The two input vectors were "
      << r1 << " and " << r2 << " and the rank of the Weyl group is " << this->GetDim() << ". ";
      crash << crash;
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
      out.MakeID(generatorImages[0]);
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
    out.generatorS.SetSize(this->preimageGroup->generators.size);
    for (int i = 0; i < this->preimageGroup->generators.size; i ++) {
      elementSecondGroup ki = (*this)(this->preimageGroup->generators[i]);
      M2.GetMatrixOfElement(ki,out.generatorS[i]);
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
    repms.SetSize(this->ownerGroup->theElements.size);
    for (int i = 0; i < this->ownerGroup->theElements.size; i ++) {
      this->GetMatrixOfElement(this->ownerGroup->theElements[i], repms[i]);
    }
    for (int i = 0; i < this->ownerGroup->theElements.size; i ++) {
      for (int j = 0; j < this->ownerGroup->theElements.size; j ++) {
        if (repms[i] * repms[j] != repms[this->ownerGroup->theElements.GetIndex(
          this->ownerGroup->theElements[i] * this->ownerGroup->theElements[j])]
        ) {
          crash << "Bad representation. " << crash;
        }
      }
    }
    stOutput << "Expensive verification complete, this is indeed a representation\n";
    return true;
  }
  std::string DescribeAsDirectSum();
  void operator*= (const GroupRepresentation<someGroup, coefficient>& other);

  JSData JSOut();

  template <typename somestream>
  somestream& IntoStream(somestream& out) const;
  std::string ToString(FormatExpressions* fmt = 0) const;
  friend std::ostream& operator<< (std::ostream& out, GroupRepresentation<someGroup, coefficient>& data) {
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
      out.names.AddOnTop(this->identifyingString);
    }
    out.theCharacteR = this->theCharacteR;
    return out;
  }
  //Note:  The group representation types compute the hash value from the character,
  // in which the order of the conjugacy classes determines the value
  // If you compare hash values from two groups which calculated their conjugacy classes in a different way,
  // you must ensure they are in the same order, or hashing won't work as expected.
  unsigned int HashFunction() const;
  static unsigned int HashFunction(const GroupRepresentation<someGroup, coefficient>& in) {
    return in.HashFunction();
  }
};

template <typename someGroup, typename coefficient>
template <typename elementSomeGroup>
bool GroupRepresentation<someGroup, coefficient>::GetMatrixOfElement(const elementSomeGroup& g, Matrix<coefficient>& out) {
  out.MakeID(this->generatorS[0]);
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
    crash << "This is a programming error: group not initialized at a place where it should be." << crash;
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
  this->theCharacteR.data.SetSize(this->ownerGroup->conjugacyClasseS.size);
  for (int cci = 0; cci < this->ownerGroup->conjugacyClasseS.size; cci ++) {
    Matrix<coefficient> M;
    M.MakeID(this->generatorS[0]);
    List<int> ccirWord;
    //    this->GetMatrixOfElement(this->ownerGroup->conjugacyClasseS[cci].representative);
    this->ownerGroup->GetWord(this->ownerGroup->conjugacyClasseS[cci].representative, ccirWord);
    for (int i = 0; i < ccirWord.size; i ++) {
      M *= this->generatorS[ccirWord[i]];
    }
    this->theCharacteR.data[cci] = M.GetTrace();
  }
  this->theCharacteR.G = ownerGroup;
  this->flagCharacterIsComputed = true;
}

// this is not mathematical but collating order
// the intention is to sort in reverse order within dimensions, according to
// the usual way character tables are displayed
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
  generatorS.SetSize(right.generatorS.size);
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
  out << "Representation of group " << ownerGroup->ToString();
  if (!this->identifyingString.empty()) {
    out << "identified as " << identifyingString;
  }
  out << " with character " << this->theCharacteR;
  return out;
}

template <typename someGroup, typename coefficient>
std::string GroupRepresentation<someGroup, coefficient>::ToString(FormatExpressions* fmt) const {
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
    out["character"].type = JSData::JSnull;
  }
  for (int i = 0; i < generatorS.size; i ++) {
    out["generators"][i] = generatorS[i];
  }
  return out;
}

template <typename somegroup, typename coefficient>
class GroupRepresentationCarriesAllMatrices {
  friend std::ostream& operator << (std::ostream& output, const GroupRepresentationCarriesAllMatrices& theIrrep) {
    output << theIrrep.ToString();
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
  //Note:  The group representation types compute the hash value from the character,
  // in which the order of the conjugacy classes determines the value
  // If you compare hash values from two groups which calculated their conjugacy classes in a different way,
  // you must ensure they are in the same order, or hashing won't work as expected.
  unsigned int HashFunction() const;
  bool CheckInitialization() const;
  bool CheckAllSimpleGensAreOK() const;
  static unsigned int HashFunction(const GroupRepresentationCarriesAllMatrices<somegroup, coefficient>& input) {
    return input.HashFunction();
  }
  void ComputeAllElementImages();
  const ClassFunction<somegroup, coefficient>& GetCharacter();
  VectorSpace<coefficient> FindDecentBasis() const;
  void MultiplyBy(
    const GroupRepresentationCarriesAllMatrices<somegroup, coefficient>& other,
    GroupRepresentationCarriesAllMatrices<somegroup, coefficient>& output
  ) const;
  void GetLargestDenominatorSimpleGens(LargeIntUnsigned& outputLCM, LargeIntUnsigned& outputDen) const;

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
    result*= other;
    return result;
  }
  bool operator== (const GroupRepresentationCarriesAllMatrices<somegroup, coefficient>& other) const {
    return this->ownerGroup == other.ownerGroup && this->theCharacteR == other.theCharacteR;
  }
  void SpreadVector(const Vector<coefficient>& input, Vectors<coefficient>& outputBasisGeneratedSpace);
  std::string GetName() const;
  std::string ToString(FormatExpressions* theFormat = 0) const;
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
class VirtualRepresentation : public MonomialCollection<ClassFunction<somegroup, coefficient>, Rational> {
public:
  void operator*= (const VirtualRepresentation<somegroup, coefficient>& other);
  void AssignRep(const GroupRepresentationCarriesAllMatrices<somegroup, Rational>& other);
  void AssignRep(const GroupRepresentation<somegroup, Rational>& other);
  inline static unsigned int HashFunction(const VirtualRepresentation<somegroup, coefficient>& input) {
    return input.HashFunction();
  }
  inline unsigned int HashFunction() const {
    return this->::MonomialCollection<ClassFunction<somegroup, coefficient>, Rational>::HashFunction();
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
  typedef void (*GroupAction)(
    const elementGroup& actingElement, const elementRepresentation& inputElementActedUpon, elementRepresentation& output
  );
private:
  HashedList<elementRepresentation> privateLayer1, privateLayer2, privateLayer3;
  elementRepresentation bufferRepresentationElement;
public:
  HashedList<elementRepresentation>* previousLayer;
  HashedList<elementRepresentation>* currentLayer;
  HashedList<elementRepresentation>* nextLayer;
  OrbitIterator::GroupAction theGroupAction;
  List<elementGroup> theGroupGeneratingElements;
  int indexCurrentElement;
  OrbitIterator() {
    this->reset();
  }
  std::string ToStringLayerSize() const;
  void reset() {
    this->previousLayer = &this->privateLayer1;
    this->currentLayer = &this->privateLayer2;
    this->nextLayer = &this->privateLayer3;
    this->theGroupAction = 0;
    this->indexCurrentElement = - 1;
  }
  const elementRepresentation& GetCurrentElement();
  bool IncrementReturnFalseIfPastLastFALSE();
  bool IncrementReturnFalseIfPastLast();
  void init(
    const List<elementGroup>& inputGenerators,
    const elementRepresentation& inputElement,
    OrbitIterator::GroupAction inputGroupAction
  );
};

class OrbitIteratorWeylGroup: public OrbitIterator<ElementWeylGroup<WeylGroupData>, ElementWeylGroup<WeylGroupData> > {
};

class OrbitFDRepIteratorWeylGroup {
  OrbitIterator<ElementWeylGroup<WeylGroupData>, Vector<Rational> > theIterator;
  List<Vector<Rational> > orbitBuffer;
  Vector<Rational> orbitDefiningElement;
  int maxOrbitBufferSize;
public:
  int orbitSize;
  LargeInt computedSize;
  int currentIndexInBuffer;
  bool flagOrbitIsBuffered;
  bool flagOrbitEnumeratedOnce;
  OrbitFDRepIteratorWeylGroup() {
    this->reset();
  }
  void reset();
  bool IncrementReturnFalseIfPastLast();
  const Vector<Rational>& GetCurrentElement();
  std::string ToString() const;
  std::string ToStringSize() const;
  bool CheckConsistency();
  void init();
  void init(
    const List<ElementWeylGroup<WeylGroupData> >& inputGenerators,
    const Vector<Rational>& inputElement,
    OrbitIterator<ElementWeylGroup<WeylGroupData>, Vector<Rational> >::GroupAction inputGroupAction
  );
};

template <typename coefficient>
std::ostream& operator<<(std::ostream& out, const ClassFunction<WeylGroupData, coefficient> X);

template <typename elementSomeGroup>
class Coset {
public:
  elementSomeGroup representative;
  List<int> representativeWord;
  List<int> supergroupIndices;
  std::string ToString() const;
};

template <typename elementSomeGroup>
std::string Coset<elementSomeGroup>::ToString() const {
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
  LargeInt SizeByFormulaOrNeg1() const {
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
  std::string ToString(FormatExpressions* theFormat);
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
  void (*CosetRepresentativeEnumerator)(void* H) = NULL;
  bool (*SameCosetAsByFormula)(void* H, elementSomeGroup& g1, elementSomeGroup& g2) = NULL;
  //GroupRepresentation<Subgroup<somegroup,elementSomeGroup>, Rational> GetEmptyRationalRepresentationSubgroup();
};

template <typename elementSomeGroup>
bool TranslatableWordsSubgroupElementGetWord(FiniteGroup<elementSomeGroup>& H, const elementSomeGroup& g, List<int>& out) {
  List<int> superword;
  H.parentRelationship->theGroup->GetWord(g, superword);
  out.SetSize(0);
  for (int i = 0; i < superword.size; i ++) {
    if (!H.parentRelationship->superGeneratorSubWordExists[superword[i]]) {
      if (!H.HasElement(g)) {
        stOutput << "element " << g << " isn't even a member of " << H << '\n';
      }
      crash << "element " << g << " is assigned parent word " << superword.ToStringCommaDelimited()
      << " containing generator not found in subgroup " << superword[i]
      << " so if this does belong to the subgroup, we need a better algorithm. "
      << crash;
    }
    out.AddListOnTop(H.parentRelationship->superGeneratorSubWords[superword[i]]);
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
  stOutput << "Coset multiplication table\n" << csmt.ToStringPlainText() << '\n';
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
  crash << "Error: element " << g << " (possible index " << gi << ") not found in cosets. "
  << "Also some groups are unprintable. " << crash;
  return - 1;
}

template <typename someGroup, typename elementSomeGroup>
template <typename coefficient>
void SubgroupData<someGroup, elementSomeGroup>::QuotientGroupPermutationRepresentation(GroupRepresentation<someGroup, coefficient>& out) {
  this->ComputeCosets();
  out.ownerGroup = this->theGroup;
  out.identifyingString = "Quotient permutation representation";
  stOutput << "Subgroup::QuotientGroupPermutationRepresentation: Permuting " << cosets.size << " cosets.\n";
  out.generatorS.SetSize(this->theGroup->generators.size);
  for (int i = 0; i < this->theGroup->generators.size; i ++) {
    out.generatorS[i].init(this->cosets.size, this->cosets.size);
    out.generatorS[i].MakeZero();
    for (int ci = 0; ci <out.generatorS[i].NumCols; ci ++) {
      elementSomeGroup g =  this->theGroup->generators[i] * this->cosets[ci].representative;
      int j = this->GetCosetId(g);
      out.generatorS[i](j, ci) = 1;
    }
    stOutput << "Element " << this->theGroup->generators[i] << " of coset " << this->GetCosetId(this->theGroup->generators[i]);
    stOutput << " permutes the other cosets as\n" << out.generatorS[i].ToStringPlainText() << '\n';
  }
}

// G⊗_H V
// g(w) = h[g](Ⓧv) where [g] rearranges the v's
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
  sr.generatorS.SetSize(this->theGroup->generators.size);
  // in TODO: make random FiniteGroups capable of finding their conjugacy classes in less than at least 5 minutes
  stOutput << "inducing from subgroup representation:\n";
  for (int i = 0; i < this->theSubgroup->generators.size; i ++) {
    stOutput << this->theSubgroup->generators[i] << '\n' << in.generatorS[i].ToStringPlainText() << '\n';
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
    stOutput << "element " << this->theGroup->generators[i] << " belongs to coset " << csi;
    stOutput << " represented by " << cosets[csi].representative << " and corresponds to subgroup element " << g;
    in.GetMatrixOfElement(g,sr.generatorS[i]);
    stOutput << " which is assigned matrix\n" << sr.generatorS[i].ToStringPlainText() << '\n';
  }
  GroupRepresentation<someGroup, coefficient> out;
  out.MakeTensorRepresentation(qr,sr);
  stOutput << "Subgroup representation: " << sr.ToString() << "\n";
  for (int i = 0; i < this->theGroup->generators.size; i ++) {
    stOutput << this->theGroup->generators[i] << ' ' << sr.generatorS[i].GetTrace() << '\n'
    << sr.generatorS[i].ToStringPlainText() << '\n';
  }
  stOutput << "Quotient representation: " << qr.ToString() << "\n";
  for (int i = 0; i < this->theGroup->generators.size; i ++) {
    stOutput << this->theGroup->generators[i] << ' ' << qr.generatorS[i].GetTrace() << '\n'
    << qr.generatorS[i].ToStringPlainText() << '\n';
  }
  stOutput << "Induced representation: " << out.ToString() << '\n';
  for (int i = 0; i <out.generatorS.size; i ++) {
    stOutput << this->theGroup->generators[i] << ' ' << out.generatorS[i].GetTrace() << '\n'
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
  out.generatorS.SetSize(this->theGroup->generators.size);
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
  //stOutput << "Induced representation: " << out.ToString() << '\n';
  for (int i = 0; i <out.generatorS.size; i ++) {
    stOutput << this->theGroup->generators[i] << ' ' << out.generatorS[i].GetTrace() << '\n'
    << out.generatorS[i].ToStringPlainText() << '\n';
  }
  if (!out.VerifyRepresentation()) {
    if (!in.VerifyRepresentationExpensive()) {
      stOutput << "Well, we weren't given a proper representation either.";
    }
    FiniteGroup<Matrix<coefficient> > ingroup;
    ingroup.generators = in.generatorS;
    stOutput << "Generator commutation relations for input representation:\n"
    << ingroup.PrettyPrintGeneratorCommutationRelations();
    stOutput << "a quotient group of\n" << this->theSubgroup->PrettyPrintGeneratorCommutationRelations();
    FiniteGroup<Matrix<coefficient> > outgroup;
    outgroup.generators = out.generatorS;
    stOutput << "Generator commutation relations for 'representation':\n"
    << outgroup.PrettyPrintGeneratorCommutationRelations();
    stOutput << "It was supposed to be a quotient group of\n" << this->theGroup->PrettyPrintGeneratorCommutationRelations();
    crash << crash;
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
  SubgroupData<FiniteGroup<ElementWeylGroup<WeylGroupData> >, ElementWeylGroup<WeylGroupData> > theSubgroupData;
  List<Rational> tauSignature;
  bool CheckInitialization();
  void ComputeTauSignature();
  void GetSignCharacter(Vector<Rational>& out);
  SubgroupDataWeylGroup();
  std::string ToString(FormatExpressions* theFormat = 0);
};

class SubgroupDataRootReflections : public SubgroupDataWeylGroup {
public:
  SubgroupData<FiniteGroup<ElementWeylGroup<WeylGroupData> >, ElementWeylGroup<WeylGroupData> > subGroupDataContainer;
  Matrix<Rational> SubCartanSymmetric;
  DynkinType theDynkinType;
  Vectors<Rational> generatingSimpleRoots;
  bool flagIsParabolic;
  bool flagIsExtendedParabolic;
  Selection simpleRootsInLeviParabolic;
  void InitGenerators();
  void MakeParabolicSubgroup(WeylGroupData& G, const Selection& inputGeneratingSimpleRoots);
  void MakeFromRoots(WeylGroupData& G, const Vectors<Rational>& inputRootReflections);
  LargeInt SizeByFormulaOrNeg1() const {
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
  SubgroupDataRootReflections() {
    this->flagIsParabolic = false;
    this->flagIsExtendedParabolic = false;
  }
  std::string ToString(FormatExpressions* theFormat = 0);
};

class SubgroupWeylGroupOLD: public HashedList<ElementWeylGroup<WeylGroupData> > {
public:
  bool truncated;
  WeylGroupData* AmbientWeyl;
  WeylGroupData Elements;
  List<ElementWeylGroup<WeylGroupData> > RepresentativesQuotientAmbientOrder;
  Vectors<Rational> simpleGenerators;
  //format: each element of of the group is a list of generators,
  //reflections with respect to the simple generators, and outer
  //automorphisms.
  //format of the externalAutomorphisms:
  // For a fixed external automorphism (of type Vectors<Rational>) the i^th entry gives the image
  //of the simple root  with 1 on the i^th coordinate
  List<Vectors<Rational> > ExternalAutomorphisms;
  HashedList<Vector<Rational> > RootSubsystem;
  Vectors<Rational> RootsOfBorel;
  SubgroupWeylGroupOLD() {
    this->AmbientWeyl = 0;
  }
  bool CheckInitialization();
  void ToString(std::string& output, bool displayElements);
  void GetGroupElementsIndexedAsAmbientGroup(List<ElementWeylGroup<WeylGroupData> >& output);
  std::string ElementToStringBruhatGraph();
  std::string ElementToStringCosetGraph();
  std::string ElementToStringFromLayersAndArrows(
    List<List<List<int> > >& arrows, List<List<int> >& Layers, int GraphWidth, bool useAmbientIndices
  );
  std::string ToString(bool displayElements = true) {
    std::string tempS;
    this->ToString(tempS, displayElements);
    return tempS;
  }
  Vector<Rational> GetRho();
  template <class coefficient>
  void RaiseToDominantWeight(Vector<coefficient>& theWeight, int* sign = 0, bool* stabilizerFound = 0);
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
  coefficient WeylDimFormulaSimpleCoords(
    const Vector<coefficient>& theWeightInSimpleCoords, const coefficient& theRingUnit = 1
  );
  void FindQuotientRepresentatives(int UpperLimit);
  void GetMatrixOfElement(const ElementWeylGroup<WeylGroupData>& input, Matrix<Rational>& outputMatrix) const;
  template <class coefficient>
  bool GenerateOrbitReturnFalseIfTruncated(
    const Vector<coefficient>& input, Vectors<coefficient>& outputOrbit, int UpperLimitNumElements
  );
  bool ComputeSubGroupFromGeneratingReflections(
    Vectors<Rational>* inputGenerators,
    List<Vectors<Rational> >* inputExternalAutos,
    int UpperLimitNumElements,
    bool recomputeAmbientRho
  );
  void ComputeRootSubsystem();
  template <class coefficient>
  void ActByElement(int index, Vector<coefficient>& theRoot) const;
  template <class coefficient>
  void ActByElement(int index, Vector<coefficient>& input, Vector<coefficient>& output) const;
  template <class coefficient>
  void ActByElement(
    const ElementWeylGroup<WeylGroupData>& theElement, const Vector<coefficient>& input, Vector<coefficient>& output
  ) const;
  template <class coefficient>
  void ActByElement(int index, const Vectors<coefficient>& input, Vectors<coefficient>& output) const {
    this->ActByElement(this->TheObjects[index], input, output);
  }
  template <class coefficient>
  void ActByElement(
    const ElementWeylGroup<WeylGroupData>& theElement, const Vectors<coefficient>& input, Vectors<coefficient>& output
  ) const;
  void WriteToFile(std::fstream& output);
  void ReadFromFile(std::fstream& input);
};

template <typename coefficient>
class ElementWeylGroupRing : public ElementMonomialAlgebra<ElementWeylGroup<WeylGroupData>, coefficient> {
public:
  void MakeEi(WeylGroupData* G, int i);
  void MakeFromClassFunction(WeylGroupData* G, const List<coefficient>& l);
  void MakeFromClassFunction(const ClassFunction<WeylGroupData, coefficient>& l);
};

template <typename coefficient>
std::ostream& operator<<(std::ostream& out, const ElementWeylGroupRing<coefficient> g);

template <typename coefficient>
void ElementWeylGroupRing<coefficient>::MakeEi(WeylGroupData* GG, int i) {
  ElementWeylGroup<WeylGroupData> theMon;
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
  if (GG == 0) {
    crash << "Weyl group pointer not allowed to be zero. " << crash;
  }
  this->MakeZero();
  ElementWeylGroup<WeylGroupData> theMon;
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
    crash << "Attempting to multiply class functions belonging to different groups." << crash;
  }
  for (int i = 0; i < this->data.size; i ++) {
    this->data[i] *= right[i];
  }
}

template<class someFiniteGroup, typename coefficient>
ClassFunction<someFiniteGroup, coefficient> ClassFunction<someFiniteGroup, coefficient>::Sym2() const {
  ClassFunction<someFiniteGroup, coefficient> l;
  l.G = G;
  l.data.SetExpectedSize(G->ConjugacyClassCount());
  for (int i = 0; i < G->ConjugacyClassCount(); i ++) {
    l.data.AddOnTop((this->data[i] * this->data[i] + this->data[G->squaresFirstConjugacyClassRep[i]]) / 2);
  }
  return l;
}

template<class someFiniteGroup, typename coefficient>
ClassFunction<someFiniteGroup, coefficient> ClassFunction<someFiniteGroup, coefficient>::Alt2() const {
  ClassFunction<someFiniteGroup, coefficient> l;
  l.G = G;
  l.data.SetExpectedSize(G->ConjugacyClassCount());
  for (int i = 0; i < G->ConjugacyClassCount(); i ++) {
    l.data.AddOnTop((this->data[i] * this->data[i] - this->data[G->squaresFirstConjugacyClassRep[i]]) / 2);
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
  //UDPolynomial<coefficient> operator+(const UDPolynomial<coefficient>& right) const;
  void operator+=(const UDPolynomial<coefficient>& right);
  //UDPolynomial<coefficient> operator-(const UDPolynomial<coefficient>& right) const;
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
  std::string ToString(FormatExpressions* theFormat = 0) const;
  void AssignMinPoly(const Matrix<coefficient>& input);
  void AssignCharPoly(const Matrix<coefficient>& input); // method due to Urbain Le Verrier
};

template <typename coefficient>
void UDPolynomial<coefficient>::AssignMinPoly(const Matrix<coefficient>& input) {
  int n = input.NumCols;
  this->data.SetSize(1);
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
    out.data.SetSize(p.size + 1);
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
  this->data.SetSize(n + 1);
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
void UDPolynomial<coefficient>::operator+= (const UDPolynomial<coefficient>& right) {
  int t = min(right.data.size, data.size);
  for (int i = 0; i < t; i ++) {
    data[i] += right.data[i];
  }
  if (right.data.size > data.size) {
    int n = data.size;
    data.SetSize(right.data.size);
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
    data.SetSize(right.data.size);
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
  out.data.SetSize(data.size + right.data.size - 1);
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
  out.data.SetSize(data.size + n);
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
void UDPolynomial<coefficient>::operator*= (const coefficient& right) {
  for (int i = 0; i < data.size; i ++) {
    data[i] *= right;
  }
}

template <class coefficient>
std::string UDPolynomial<coefficient>::ToString(FormatExpressions* theFormat) const {
  Polynomial<coefficient> tempP;
  tempP.MakeZero();
  MonomialP tempM;
  for (int i = 0; i < this->data.size; i ++) {
    tempM.MakeEi(0, i, 1);
    tempP.AddMonomial(tempM, this->data[i]);
  }
  return tempP.ToString(theFormat);
}

template <typename coefficient>
std::ostream& operator<<(std::ostream& out, const UDPolynomial<coefficient>& p) {
  FormatExpressions tempFormat;
  tempFormat.polyAlphabeT.SetSize(1);
  tempFormat.polyAlphabeT[0] = "q";
  return out << p.ToString(&tempFormat);
}

template <class elementSomeGroup>
std::string FiniteGroup<elementSomeGroup>::ToString(FormatExpressions* theFormat) {
  std::stringstream out;
  out << this->ToStringElements(theFormat);
  if (this->flagCCRepresentativesComputed) {
    out << this->ToStringConjugacyClasses(theFormat);
  }
  return out.str();
}

template <typename elementSomeGroup>
int FiniteGroup<elementSomeGroup>::MultiplyElements(int indexLeft, int indexRight) const {
  return this->theElements.GetIndex(this->theElements[indexLeft] * this->theElements[indexRight]);
}

template <typename elementSomeGroup>
int FiniteGroup<elementSomeGroup>::Invert(int g) const {
  return this->theElements.GetIndex(this->theElements[g].Inverse());
}
#endif
