#ifndef header_math_extra_semisimple_lie_subalgebras_ALREADY_INCLUDED
#define header_math_extra_semisimple_lie_subalgebras_ALREADY_INCLUDED

#include "math_extra_algebraic_numbers.h"
#include "math_extra_semisimple_lie_subalgebras_sltwos.h"
#include "math_general.h"

class SemisimpleSubalgebras;
class CandidateSemisimpleSubalgebra;
class SubalgebraSemisimpleLieAlgebra;
class PossibleExtensionsOfSemisimpleLieSubalgebra;

class NilradicalCandidate {
public:
  CandidateSemisimpleSubalgebra* owner;
  std::string FKnilradicalLog;
  bool flagNilradicalConesIntersect;
  bool flagNilradicalConesStronglyIntersect;
  bool flagComputedRelativelyStrongIntersections;
  bool flagRestrictedCentralizerConditionHoldS;
  bool flagParabolicACExtendsToParabolicAC;
  bool flagLinfiniteRelFound;
  DynkinDiagramRootSubalgebra leviDiagramAmbient;
  DynkinDiagramRootSubalgebra leviDiagramSmall;
  // 0->not selected; 1->selected; 2->undecided.
  List<int> nilradicalSelection;
  Vector<Rational> coneIntersection;
  Vector<Rational> coneStrongIntersection;
  Vector<Rational> coneRelativelyStrongIntersection;
  Vector<Rational> coneSeparatingNormal;
  Vectors<Rational> nilradicalWeights;
  Vectors<Rational> nonFernandoKacHighestWeights;
  Vectors<Rational> nonFernandoKacHighestWeightsStronglyTwoSided;
  List<int> ownerModulesNilradicalElements;
  List<int> ownerModulesNonFernandoKacHighestWeightVectors;
  Vectors<Rational> leviRootsAmbient;
  Vectors<Rational> leviRootsSmallPrimalFundamentalCoordinates;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >
  nonFernandoKacHighestWeightVectors;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >
  nonFernandoKacHighestVectorsStronglyTwoSided;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > nilradical;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >
  nilradicalElementOpposites;
  Selection nilradicalSubselection;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > nilradicalSubset;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >
  nonFernandoKacHighestWeightVectorsStrongRelativeToSubset;
  Vectors<Rational> nilradicalSubsetWeights;
  Vectors<Rational>
  nonFernandoKacHighestWeightVectorsStrongRelativeToSubsetWeights;
  NilradicalCandidate();
  void reset();
  void checkInitialization() const;
  bool isStronglySingular(int moduleIndex);
  bool isStronglySingularRelativeToSubset(int nonFernandoKacWeightIndex);
  Vector<Rational> getConeStrongIntersectionWeight() const;
  Vector<Rational> getNilradicalLinearCombination() const;
  void getModGeneratedByNonHighestWeightVectorAndNilradElement(
    int indexInNilradSubset,
    List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& outputLeft,
    List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& outputRight,
    List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& outputBrackets
  ) const;
  bool tryFindingLInfiniteRelations();
  void computeParabolicACExtendsToParabolicAC();
  bool isCommutingSelectionNilradicalElements(
    Selection& inputNilradicalSelection
  );
  void processMe();
  std::string toString(FormatExpressions* format = nullptr) const;
  std::string toStringTableElementWithWeights(
    const List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& elements,
    const Vectors<Rational>& weights
  ) const;
  void computeTheTwoCones();
  void computeTheTwoConesRelativeToNilradicalSubset();
};

class SubalgebraSemisimpleLieAlgebra {
public:
  std::string toString(FormatExpressions* format = nullptr);
  SemisimpleLieAlgebra* owner;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > generators;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > basis;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > cartanSubalgebra;
  void computeBasis();
  void computeCartanSubalgebra();
  SubalgebraSemisimpleLieAlgebra();
  bool findCartanSubalgebraCandidate(
    const List<MatrixTensor<AlgebraicNumber> >& adjointOperators,
    ElementSemisimpleLieAlgebra<AlgebraicNumber>& outputCandidate,
    const List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >&
    currentCentralizer
  );
  bool checkInitialization();
};

enum CandidateSubalgebraStatus {
  unknown,
  realized,
  unrealizableNoSerreSystemSolutions,
  unrealizableNonFittingCharacter,
  neitherRealizedNorProvedImpossible,
  previouslyRealizedAsADirectSummand,
  // Bad user input (user error)
  // or bad precomputed data (programming error).
  corrupt,
};

class CandidateSemisimpleSubalgebra {
  friend std::ostream& operator<<(
    std::ostream& output, const CandidateSemisimpleSubalgebra& candidate
  ) {
    output << candidate.toString(nullptr, false);
    return output;
  }
public:
  WeylGroupData* weylNonEmbedded;
  SemisimpleLieAlgebra* subalgebraNonEmbeddedDefaultScale;
  List<Vectors<Rational> > cartanSubalgebrasByComponentScaledToActByTwo;
  List<AlgebraicNumber> ratiosKillingsByComponent;
  Vectors<Rational> cartanElementsScaledToActByTwo;
  Vectors<Rational> cartanElementsSubalgebra;
  Vectors<Rational> hsScaledToActByTwoInOrderOfCreation;
  Matrix<Rational> bilinearFormSimplePrimal;
  Matrix<Rational> bilinearFormFundamentalPrimal;
  Matrix<Rational>
  inducedEmbeddingPrimalFundamentalCoordinatesIntoSimpleAmbientCoordinates;
  Matrix<Rational> matrixMultiplyFundamentalCoordinatesToGetSimple;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > positiveGenerators;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > negativeGenerators;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > basis;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > fullBasisByModules;
  List<int> fullBasisOwnerModules;
  List<ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > >
  unknownPositiveGenerators;
  List<ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > >
  unknownNegativeGenerators;
  List<ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > >
  unknownCartanCentralizerBasis;
  List<List<ChevalleyGenerator> > involvedPositiveGenerators;
  List<List<ChevalleyGenerator> > involvedNegativeGenerators;
  List<List<Vector<Rational> > > involvedPositiveRoots;
  // Character of the ambient Lie algebra with
  // respect to (Cartan subalgebra of) sl(2).
  CharacterSemisimpleLieAlgebraModule<Rational>
  characterFundamentalCoordinatesRelativeToCartan;
  // Character of the ambient Lie algebra with respect to sl(2),
  // i.e., formal sum of the highest weights of the k-modules.
  CharacterSemisimpleLieAlgebraModule<Rational>
  characterNonPrimalFundamentalCoordinates;
  // Formal sum of the highest weights of the k-modules,
  // with modules additionally subdivided by their weights
  // with respect to the Cartan subalgebra of the centralizer of k.
  CharacterSemisimpleLieAlgebraModule<Rational> primalCharacter;
  Vectors<Rational> posistiveRootsPerpendicularToPrecedingWeights;
  Vectors<Rational> cartanOfCentralizer;
  List<Polynomial<AlgebraicNumber> > systemToSolve;
  List<Polynomial<AlgebraicNumber> > transformedSystem;
  PolynomialSystem<AlgebraicNumber> configuredSystemToSolve;
  SemisimpleSubalgebras* owner;
  int indexInOwner;
  int indexInducedFrom;
  // If the subalgebra is an sl(2), contains its location in the sl2Subalgebras
  // array, else equals -1.
  int indexInSlTwos;
  List<int> rootInjectionsFromInducer;
  int indexNonEmbeddedMeStandard;
  int indexNonEmbeddedMeNonStandardCartan;
  int indexHcandidateBeingGrown;
  int indexMaximalSemisimpleContainer;
  int indexSemisimplePartCentralizer;
  List<int> indicesDirectSummandSuperAlgebra;
  MemorySaving<FormatExpressions> characterFormat;
  CandidateSubalgebraStatus status;
  bool flagCentralizerIsWellChosen;
  bool flagCentralizerTypeIsComputed;
  bool flagUsedInducingSubalgebraRealization;
  bool flagComputedPrimalDecomposition;
  bool flagComputedBasics;
  // Whether we have computed all internals from the given
  // subalgebra generators.
  bool flagInternalInitializationAndVerificationComplete;
  int recursionDepthCounterForNilradicalGeneration;
  int totalUnknownsNoCentralizer;
  int totalUnknownsWithCentralizer;
  long long int totalArithmeticOperationsToSolveSystem;
  List<NilradicalCandidate> fernandoKacNilradicalCandidates;
  int numberOfConeIntersections;
  int numberOfCasesNoLInfiniteRelationFound;
  int numberOfBadParabolics;
  int numberOfCentralizerConditionFailsConeConditionHolds;
  // The highest weight vectors are by definition cartan-centralizer-split.
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > highestVectorsNonSorted;
  List<Vector<Rational> > highestWeightsPrimalNonSorted;
  List<Vector<Rational> > highestWeightsNonPrimalNonSorted;
  List<List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > > highestVectors;
  HashedList<Vector<Rational> > highestWeightsPrimal;
  List<Vector<Rational> > highestWeightsNonPrimal;
  // A list of lists of all k-modules, grouped by primal highest weight.
  // A sub-list contains all k-modules of a given primal highest weight.
  // Each sub-sub-list contains a basis elements of a k-module.
  // The first element in the list is a highest weight vector.
  // The last element is a lowest weight vector.
  // Vectors with higher indices have lower weight
  // (in the partial order of weights) that vectors
  // with lower indices.
  List<List<List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > > > modules;
  List<List<List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > > >
  modulesSl2Opposite;
  List<List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > >
  modulesIsotypicallyMerged;
  List<Vectors<Rational> > weightsModulesNONprimal;
  List<Vectors<Rational> > weightsModulesPrimal;
  List<CharacterSemisimpleLieAlgebraModule<Rational> > charactersPrimalModules;
  List<CharacterSemisimpleLieAlgebraModule<Rational> >
  charactersPrimalModulesMerged;
  HashedList<Vector<Rational> > rootSystemCentralizerPrimalCoordinates;
  HashedList<Vector<Rational> > rootSystemSubalgebraPrimalCoordinates;
  List<List<List<int> > > nilradicalPairingTable;
  List<int> subalgebraModules;
  List<int> centralizerSubalgebraModules;
  List<int> primalSubalgebraModules;
  List<List<int> > oppositeModulesByStructure;
  List<List<int> > oppositeModulesByChar;
  HashedList<int, HashFunctions::hashFunction> modulesWithZeroWeights;
  std::string comments;
  std::string nilradicalGenerationLog;
  Rational centralizerRank;
  Rational centralizerDimension;
  DynkinDiagramRootSubalgebra centralizerSubDiagram;
  DynkinType centralizerType;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > centralizerManualBasis;
  Matrix<AlgebraicNumber> killingFormCentralizer;
  class WConjecture {
  public:
    bool flagWConjectureHolds;
    bool flagSlTwoIsSmall;
    List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > basisCentralizerOfHInP;
    List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > basisCentralizerOfH;
    List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > basisCentralizerOfSl2;
    List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >
    basisCentralizerOfSl2InP;
    List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >
    basisCentralizerOfSl2InK;
    List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >
    basisOrthogonalComplementOfCentralizerOfSl2InP;
    // The vector space whose basis is below is called W, from where the
    // WConjecture derives its name.
    List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >
    basisOrthogonalComplementCentralizerSl2InPIntersectedWithCentralizerOfHInP;
    // A human-readable math formula that describes w_1, w_2 \in W
    // such that [w_1, w_2] \in the centralizer of sl(2) in k.
    List<std::string> triplesNotViolatingWConjecture;
    // Same as above but with [w_1, w_2] \notin the centralizer of sl(2) in k.
    List<std::string> triplesViolatingWConjecture;
    ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > elementE;
    ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >
    genericNegativeOneWeightElement;
    ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >
    adEOfGenericNegativeOneWeightElement;
    WConjecture();
    void compute(
      CandidateSemisimpleSubalgebra& owner, SlTwoSubalgebra& ownerSl2
    );
    void processOnePair(int indexLeft, int indexRight);
    std::string toStringWConjectureHolds() const;
    std::string toString(const CandidateSemisimpleSubalgebra& owner) const;
    std::string toStringAdjointActionNegativeOne() const;
    std::string toStringLieBracketTriples(const List<std::string>& triple)
    const;
    void computeSmallOrbits(const CandidateSemisimpleSubalgebra& owner);
    void computeAdEAsPolynomialMap(const CandidateSemisimpleSubalgebra& owner);
  };

  WConjecture wConjecture;
  bool flagDeallocated;
  CandidateSemisimpleSubalgebra();
  ~CandidateSemisimpleSubalgebra();
  void reset(SemisimpleSubalgebras* inputOwner = nullptr);
  bool checkConsistency() const;
  bool checkMaximalDominance() const;
  bool computeAndVerifyFromKnownGeneratorsAndHsPrepare(
    std::stringstream* commentsOnError
  );
  bool computeAndVerifyFromKnownGeneratorsAndHs();
  int getPrimalRank() const;
  int getRank() const;
  int getSemisimplePartCentralizerOfSemisimplePartCentralizer() const;
  void getHsScaledToActByTwoByType(
    List<List<Vectors<Rational> > >& outputHsByType,
    List<DynkinSimpleType>& outputTypeList
  ) const;
  bool isUnrealizable() const;
  bool isRealized() const;
  bool isZero() const;
  bool hasHsScaledByTwoConjugateTo(List<Vector<Rational> >& other) const;
  bool computeCentralizerTypeFailureAllowed();
  bool isDirectSummandOf(const CandidateSemisimpleSubalgebra& other);
  void getGenericCartanCentralizerLinearCombination(
    int indexCartanCentralizerGenerator,
    ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& output
  );
  void getGenericNegativeGeneratorLinearCombination(
    int indexNegativeGenerators,
    ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& output
  );
  void getGenericPositiveGeneratorLinearCombination(
    int indexPositiveGenerators,
    ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& output
  );
  bool isExtremeWeight(int moduleIndex, int indexInIsomorphismComponent) const;
  bool isRegularSubalgebra() const;
  bool compareLeftGreaterThanRight(
    const Vector<Rational>& left, const Vector<Rational>& right
  );
  // Given chevalley generators a_1, ..., a_k, returns the
  // generic linear combination along the lines of:
  // x_1 a_1+...+x_k a_k.
  // Here, the variables x_1, ..., x_k may run in a different variable range;
  // the exact variable range is determined by the variableOffset and the
  // numberOfVariables.
  // Additionally, if useArbitraryValues is set, then
  // instead of arbitrary values x_1, ..., x_k, we will use
  // arbitrarily chosen constants.
  void getGenericLinearCombination(
    int numberOfVariables,
    int variableOffset,
    List<ChevalleyGenerator>& involvedGenerators,
    ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& output
  );
  void computeCentralizerIsWellChosen();
  void computeCentralizerManually();
  void adjustCentralizerAndRecompute(bool allowNonPolynomialSystemFailure);
  void computeRatioKillingsByComponent();
  void addToSystem(
    const ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >&
    elementThatMustVanish,
    List<Polynomial<AlgebraicNumber> >& outputSystem
  ) const;
  CandidateSubalgebraStatus attemptToRealize(
    PossibleExtensionsOfSemisimpleLieSubalgebra* currentExtension
  );
  bool getCentralizerTypeIfComputableAndKnown(
    const DynkinType& input, DynkinType& output
  );
  void computeHsAndHsScaledToActByTwoFromComponents();
  void setUpInjectionHs(
    const CandidateSemisimpleSubalgebra& baseSubalgebra,
    const DynkinType& newType,
    const List<int>& rootInjection,
    Vector<Rational>* newHScaledToActByTwo = nullptr
  );
  void enumerateAllNilradicals();
  std::string toStringNilradicalSelection(const List<int>& selection);
  void enumerateNilradicalsRecursively(
    List<int>& selection, std::stringstream* logStream = nullptr
  );
  void extendNilradicalSelectionToMultFreeOverSemisimplePartSubalgebra(
    HashedList<int, HashFunctions::hashFunction>& inputOutput
  );
  bool isPossibleNilradicalCarryOutSelectionImplications(
    List<int>& selection, std::stringstream* logStream = nullptr
  );
  void extendToModule(
    List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& inputOutput
  );
  Vector<Rational> getPrimalWeightFirstGenerator(
    const ElementSemisimpleLieAlgebra<AlgebraicNumber>& input
  ) const;
  Vector<Rational> getNonPrimalWeightFirstGenerator(
    const ElementSemisimpleLieAlgebra<AlgebraicNumber>& input
  ) const;
  void computeKsl2Triples();
  void computeKsl2TriplesGetOppositeElementsInOppositeModule(
    const Vector<Rational>& weight,
    const List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >&
    inputOppositeModule,
    List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& outputElements
  );
  void computeKsl2TriplesGetOppositeElementsAll(
    const Vector<Rational>& weight,
    List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& outputElements
  );
  bool computeKsl2TripleSetUpAndSolveSystem(
    const ElementSemisimpleLieAlgebra<AlgebraicNumber>& element,
    const List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >&
    fIsLinearCombinationOf,
    ElementSemisimpleLieAlgebra<AlgebraicNumber>& outputF
  );
  void computeCharactersPrimalModules();
  void computePairingTable();
  CandidateSubalgebraStatus computeFromGenerators(
    bool allowNonPolynomialSystemFailure
  );
  bool checkDimensionOfSubalgebra(bool allowNonPolynomialSystemFailure);
  void computeSinglePair(int leftIndex, int rightIndex, List<int>& output);
  int getNumberOfModules() const;
  void computePairKWeightElementAndModule(
    const ElementSemisimpleLieAlgebra<AlgebraicNumber>& leftKWeightElement,
    int rightIndex,
    List<int>& output
  );
  bool isWeightSystemSpaceIndex(
    int index, const Vector<Rational>& ambientRootTestedForWeightSpace
  );
  void addHIncomplete(const Vector<Rational>& hElement);
  static bool checkElementSemisimpleLieAlgebra(
    const ElementSemisimpleLieAlgebra<AlgebraicNumber>& toBeChecked
  );
  bool checkAll() const;
  bool checkBasicInitialization() const;
  bool checkCandidateInitialization() const;
  bool checkFullInitialization() const;
  bool checkModuleDimensions() const;
  SemisimpleLieAlgebra& getAmbientSemisimpleLieAlgebra() const;
  WeylGroupData& getAmbientWeyl() const;
  WeylGroupAutomorphisms& getAmbientWeylAutomorphisms() const;
  void computeCartanOfCentralizer();
  void computePrimalModuleDecomposition();
  void computePrimalModuleDecompositionHighestWeightsAndHighestWeightVectors();
  void computePrimalModuleDecompositionHighestWeightVectors(
    HashedList<Vector<Rational> >& inputHighestWeights
  );
  void computePrimalModuleDecompositionHighestWeights(
    HashedList<Vector<Rational> >& outputHighestWeightsCoordinates
  );
  void computePrimalModuleDecompositionHighestWeightsLastPart();
  void getPrimalWeightProjectionFundamentalCoordinates(
    const Vector<Rational>& inputAmbientWeight, Vector<Rational>& output
  ) const;
  bool checkGeneratorsBracketToHs();
  void getWeightProjectionFundamentalCoordinates(
    const Vector<Rational>& inputAmbientWeight, Vector<Rational>& output
  ) const;
  void attemptToSolveSystem(
    bool attemptToChooseCentalizer,
    bool allowNonPolynomialSystemFailure,
    PossibleExtensionsOfSemisimpleLieSubalgebra* currentExtension
  );
  void computeInvolvedGeneratorsOfIndex(int index);
  bool canReusePreviouslyFoundRealization(
    int index, PossibleExtensionsOfSemisimpleLieSubalgebra* currentExtension
  );
  void attemptToSolveSystemPart2(
    bool attemptToChooseCentalizer, bool allowNonPolynomialSystemFailure
  );
  CandidateSubalgebraStatus attemptToSolveSystemFinal();
  bool prepareSystem(
    bool attemptToChooseCentalizer, bool allowNonPolynomialSystemFailure
  );
  bool induceUnknownsFromBaseSubalgebra(
    int indexCurrentRoot, int indexNewRoot
  );
  bool candidateInvolvesAllowedGeneratorsOnly(
    ElementSemisimpleLieAlgebra<AlgebraicNumber>& candidate,
    List<ChevalleyGenerator>& allowedSummands
  );
  void prepareSystemCentralizerCommutingRelations();
  void prepareSystemSerreRelationsForIndexPair(
    int negativeGeneratorIndex,
    int positiveGeneratorIndex,
    List<Polynomial<AlgebraicNumber> >& outputSystem
  ) const;
  void prepareSystemSerreRelations();
  bool computeCharacter(bool allowBadCharacter);
  void configurePolynomialSystem();
  bool verifySolution(PolynomialSystem<AlgebraicNumber>& system);
  void computeCentralizerOfOldHs(List<Vector<Rational> >& output);
  bool centralizesOldHs(const Vector<Rational>& candidate);
  bool isGoodHNewActingByTwo(
    const Vector<Rational>& hNewActingByTwo,
    const List<int>& rootInjections,
    const List<Vector<Rational> >& centralizerOfOldHs
  ) const;
  Rational getScalarSubalgebra(
    const Vector<Rational>& primalWeightLeft,
    const Vector<Rational>& primalWeightRight
  ) const;
  static std::string toStringStatusStatic(CandidateSubalgebraStatus status);
  std::string toStringStatus() const {
    return CandidateSemisimpleSubalgebra::toStringStatusStatic(this->status);
  }
  std::string toStringType(FormatExpressions* format = nullptr) const;
  std::string toStringTypeAndHs(FormatExpressions* format = nullptr) const;
  std::string toStringGenerators(FormatExpressions* format) const;
  std::string toString(FormatExpressions* format, bool generateLinks) const;
  std::string toStringSystem(FormatExpressions* format = nullptr) const;
  std::string toStringSystemPart2(FormatExpressions* format = nullptr) const;
  std::string toStringInvolvedPositiveRoots() const;
  std::string toStringSubSystems() const;
  // Prints the serre relations that only involve some of the generators.
  // Ignores the generators that match the cartan elements whose
  // weights match cartan generators whose index is out of the given range.
  std::string toStringSubSystemOfRank(
    int startCartanGeneratorIndex, int numberOfCartanGenerators
  ) const;
  std::string toStringLoadUnknown(FormatExpressions* format = nullptr) const;
  std::string toStringCentralizer(
    FormatExpressions* format, bool writeToHardDisk
  ) const;
  std::string toStringCentralizerDebugData(FormatExpressions* format = nullptr)
  const;
  std::string toStringCartanSubalgebra(FormatExpressions* format = nullptr)
  const;
  std::string toStringPairingTable(FormatExpressions* format = nullptr) const;
  std::string toStringPairingTableLaTeX(FormatExpressions* format = nullptr)
  const;
  std::string toStringNilradicals(FormatExpressions* format = nullptr) const;
  std::string toStringNilradicalsSummary(FormatExpressions* format = nullptr)
  const;
  std::string toStringModuleDecomposition(FormatExpressions* format = nullptr)
  const;
  std::string toStringModuleDecompositionLaTeX(
    FormatExpressions* format = nullptr
  ) const;
  std::string toStringDrawWeights(FormatExpressions* format = nullptr) const;
  std::string toStringDrawWeightsHelper(
    int indexModule, const Vector<Rational>& weight
  ) const;
  bool operator>(const CandidateSemisimpleSubalgebra& other) const;
};

class RealizedSemisimpleSubalgebra {
public:
  friend std::ostream& operator<<(
    std::ostream& output, const RealizedSemisimpleSubalgebra& input
  ) {
    output << input.toString(nullptr, false);
    return output;
  }
  bool operator>(const RealizedSemisimpleSubalgebra& other) const;
  CandidateSemisimpleSubalgebra content;
  bool flagDeallocated;
  int indexInOwner;
  RealizedSemisimpleSubalgebra();
  ~RealizedSemisimpleSubalgebra();
  bool checkAll() const;
  std::string toString(FormatExpressions* format, bool generateLinks) const;
};

class NonRealizedSemisimpleSubalgebra {
public:
  CandidateSemisimpleSubalgebra candidate;
  bool flagDeallocated;
  int indexInOwner;
  NonRealizedSemisimpleSubalgebra();
  ~NonRealizedSemisimpleSubalgebra();
  bool checkAll() const;
};

// Encodes a possible extension of a base subalgebra
// to a larger Semisimple Lie algebra.
// The base is an already realized semisimple subalagebra or zero.
class PossibleExtensionsOfSemisimpleLieSubalgebra {
public:
  RealizedSemisimpleSubalgebra* realizedBase;
  SemisimpleSubalgebras* owner;
  int numberOfLargerTypesExplored;
  int indexOfCurrentHCandidate;
  bool flagInitialized;
  // state that is recomputed each load:
  List<DynkinType> possibleLargerDynkinTypes;
  List<List<int> > possibleRootInjections;
  List<Vector<Rational> > candidateHsScaledToActByTwo;
  PossibleExtensionsOfSemisimpleLieSubalgebra();
  // Returns true if all possible extensions have been explored.
  bool isExhausted() const;
  // Increments to the next possible extension, or
  // returns last if all extensions have been explored.
  bool incrementReturnFalseIfPastLast(ProgressReport* report);
  void initializeIfNeeded();
  // Attempts to realize the extension currently being explored.
  void attemptExtension(
    CandidateSemisimpleSubalgebra& newCandidate,
    PossibleExtensionsOfSemisimpleLieSubalgebra* currentExtension
  );
  DynkinType& nextUnexploredDynkinType();
  List<int>& nextPossibleRootInjection();
  void computeCurrentHCandidates();
  bool moduleDecompostionOverBaseAllowsExtension(
    DynkinType& candidateExtensionType,
    List<int>& candidateRootInjection,
    CandidateSemisimpleSubalgebra& newCandidate
  );
  Vector<Rational>& nextCandidateHScaledToActByTwo();
  std::string toString();
  bool checkConsistency();
};

struct SemisimpleSubalgebrasComputationOptions {
public:
  bool forceRecompute;
  bool doFullInitialization;
  bool computeNilradicals;
  bool computeModuleDecomposition;
  bool attemptToSolveSystems;
  bool computePairingTable;
  bool adjustCentralizers;
  SemisimpleSubalgebrasComputationOptions();
};

class SemisimpleSubalgebras {
public:
  friend std::ostream& operator<<(
    std::ostream& output, const SemisimpleSubalgebras& subalgebras
  ) {
    output << subalgebras.toStringShort();
    return output;
  }
  FormatExpressions currentFormat;
  SemisimpleLieAlgebra* owner;
  // The shared base field over which we compute.
  AlgebraicClosureRationals* ownerField;
  // Additional fields used for computations of the centralizers of
  // sl(2)-triples
  MapReferences<std::string, AlgebraicClosureRationals>*
  ownerFieldsForExtraComputation;
  DynkinType targetDynkinType;
  SlTwoSubalgebras slTwoSubalgebras;
  MapReferences<DynkinType, SemisimpleLieAlgebra>* subalgebrasNonEmbedded;
  MapReferences<Matrix<Rational>, SemisimpleLieAlgebra>
  subalgebrasNonDefaultCartanAndScale;
  List<List<Rational> > cachedDynkinIndicesSl2subalgebrasSimpleTypes;
  HashedList<DynkinSimpleType>
  cachedDynkinSimpleTypesWithComputedSl2Subalgebras;
  List<IteratorRootActionWeylGroupAutomorphisms> orbits;
  HashedList<Rational> orbitHElementLengths;
  HashedList<DynkinSimpleType> orbitDynkinIndices;
  // If an entry in orbit sizes is - 1 it means the corresponding orbit size
  // has not been computed yet.
  int maxStoredOrbitSize;
  std::string comments;
  std::string fileNameToLogComments;
  std::string fileNamePrefix;
  // current computation state:
  // state that gets stored to hd:
  List<PossibleExtensionsOfSemisimpleLieSubalgebra> currentSubalgebraChain;
  // End current computation state variables.
  // The map keys are used to search for subalgebras quickly.
  MapReferences<Vectors<Rational>, CandidateSemisimpleSubalgebra>
  slTwoSubalgebrasRealForm;
  RealizedSemisimpleSubalgebra zeroSubalgebra;
  // All found subalgebras are here.
  // The keys are their embedded symmetric Cartan matrices.
  MapReferences<Vectors<Rational>, RealizedSemisimpleSubalgebra> subalgebras;
  // Subalgebras not realized.
  // Non-realized subalgebras include:
  // 1) Subalgebras that may or may not exist
  // but we could neither find a solution nor prove impossibility.
  // 2) Subalgebras that are impossible.
  MapReferences<Vectors<Rational>, NonRealizedSemisimpleSubalgebra>
  nonRealizedSubalgebras;
  bool flagHasPossibleSubalgebrasWeCouldntSolveFor;
  bool flagAttemptToSolveSystems;
  // This flag determines whether our computation is over
  // the real forms of the Lie algebra.
  bool flagRealForms;
  bool flagComputePairingTable;
  bool flagComputeModuleDecomposition;
  bool flagComputeNilradicals;
  bool flagProduceLaTeXTables;
  bool flagAttemptToAdjustCentralizers;
  int64_t millisecondsComputationStart;
  int64_t millisecondsComputationEnd;
  unsigned long long int numberOfAdditions;
  unsigned long long int numberOfMultiplications;
  bool flagDeallocated;
  std::string displayNameMainFile1NoPath;
  std::string displayNameMainFile1WithPath;
  std::string virtualNameMainFile1;
  struct Statistics {
    Statistics();
    int totalExtensionsTried;
    int subalgebrasNeitherRealizedNorProvedImpossible;
    int subalgebrasProvedImpossible;
  };

  SemisimpleSubalgebras::Statistics statistics;
  class WConjecture {
  public:
    // The W-conjecture is an ongoing research project b-w D. King, T. Milev,
    // A.Noel.
    // Details may or may not be written up in a scientific paper pending
    // outcomes.
    LinearMapSemisimpleLieAlgebra<AlgebraicNumber> cartanInvolutionAmbient;
    // \mathfrak{p} is the negative one eigenspace of the Cartan involution;
    // \mathfrak{k} is the plus one eigenspace of the Cartan involution.
    // The ambient Lie algebra decomposes as \mathfrak{k}\oplus \mathfrak{p}.
    List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > basisPAmbient;
    List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > basisKAmbient;
    Matrix<Rational> killingForm;
    Matrix<AlgebraicNumber> killingFormRescaled;
    int numberOfSmallSlTwos;
    std::string toStringRealForm(const SemisimpleSubalgebras& owner) const;
    std::string toStringElementSemisimpleLieAlgebraOrMatrix(
      const SemisimpleSubalgebras& owner,
      const List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& input,
      const std::string& notationLetter
    ) const;
    void computeBeforeSubalgebras(SemisimpleSubalgebras& owner);
    void computeAfterSubalgebras(SemisimpleSubalgebras& owner);
    bool wConjectureHolds(SemisimpleSubalgebras& owner);
  };

  WConjecture wConjecture;
  // Possible values:
  // 1. nulltpr.
  // 2. CalculatorConversions::stringFromSemisimpleSubalgebras.
  std::string(*toStringExpressionString)(SemisimpleSubalgebras& input);
  bool loadState(
    List<int>& currentChainIntegers,
    List<int>& numberOfExploredTypes,
    List<int>& numberOfExploredHs,
    std::stringstream& reportStream
  );
  int getDisplayIndexFromActual(int actualindexSubalgebra) const;
  std::string toStringSubalgebraNumberWithAmbientLink(
    int actualIndexSubalgebra, FormatExpressions* format
  ) const;
  void computeFolderNames(FormatExpressions& outputFormat);
  void checkFileWritePermissions();
  void writeReportToFiles();
  std::string toStringAlgebraLink(
    int actualindexSubalgebra, FormatExpressions* format, bool writeToHardDisk
  ) const;
  std::string getRelativePhysicalFileNameSubalgebra(int actualIndexSubalgebra)
  const;
  std::string getDisplayFileNameSubalgebraRelative(
    int actualindexSubalgebra, FormatExpressions* format
  ) const;
  std::string getDisplayFileNameSubalgebraAbsolute(
    int actualindexSubalgebra, FormatExpressions* format
  ) const;
  std::string getRelativePhysicalFileNameFKFTNilradicals(
    int actualindexSubalgebra
  ) const;
  std::string getDisplayFileNameFKFTNilradicals(
    int actualindexSubalgebra, FormatExpressions* format
  ) const;
  const CandidateSemisimpleSubalgebra& baseSubalgebra();
  bool operator==(const SemisimpleSubalgebras& other) {
    STACK_TRACE("SemisimpleSubalgebras::operator==");
    if (this->owner == other.owner) {
      return true;
    }
    if (this->owner == nullptr || other.owner == nullptr) {
      global.fatal
      << "Comparison of non-initialized Semisimple Lie Subalgebras. "
      << global.fatal;
    }
    return *this->owner == *other.owner;
  }
  SemisimpleLieAlgebra& getSemisimpleOwner() const {
    if (this->owner == nullptr) {
      global.fatal
      << "Attempted to "
      << "access non-initialized semisimple Lie subalgerbas. "
      << global.fatal;
    }
    return *this->owner;
  }
  RealizedSemisimpleSubalgebra& emptyCandidateSubalgebra();
  void makeCandidateSubalgebra(
    const DynkinType& input, CandidateSemisimpleSubalgebra& output
  );
  void makeCandidateFromSlTwo(
    SlTwoSubalgebra& candidate, CandidateSemisimpleSubalgebra& output
  );
  bool incrementReturnFalseIfPastLast();
  bool getCentralizerTypeIfComputableAndKnown(
    const DynkinType& input, DynkinType& output
  );
  void logComments(const std::string& extraComments);
  bool combinatorialCriteriaAllowRealization();
  bool centralizersComputedToHaveUnsuitableNilpotentOrbits();
  bool centralizerOfBaseComputedToHaveUnsuitableNilpotentOrbits();
  void writeProgressFile();
  void initHookUpPointers(
    SemisimpleLieAlgebra& inputOwner,
    AlgebraicClosureRationals* field,
    MapReferences<std::string, AlgebraicClosureRationals>*
    inputOwnerFieldsForExrtaComputations,
    MapReferences<DynkinType, SemisimpleLieAlgebra>*
    inputSubalgebrasNonEmbedded,
    ListReferences<SlTwoSubalgebras>* inputSl2sOfSubalgebras
  );
  void resetPointers();
  void resetComputations();
  ~SemisimpleSubalgebras() {
    this->flagDeallocated = true;
  }
  SemisimpleSubalgebras(): flagDeallocated(false) {
    this->resetPointers();
  }
  RealizedSemisimpleSubalgebra& addRealizedSubalgebraIfNew(
    CandidateSemisimpleSubalgebra& input
  );
  NonRealizedSemisimpleSubalgebra& addNonRealizedSubalgebraIfNew(
    CandidateSemisimpleSubalgebra& input
  );
  RealizedSemisimpleSubalgebra& addSubalgebraIfNewSetToStackTop(
    CandidateSemisimpleSubalgebra& input
  );
  void addSubalgebraToStack(RealizedSemisimpleSubalgebra& realized);
  bool checkConsistencyHs() const;
  bool checkConsistency() const;
  bool checkInitialization() const;
  bool checkIsEmpty() const;
  bool checkAll() const;
  void computeStatistics();
  std::string toStringCandidateStatistics(bool showInternals);
  std::string toStringShort() const;
  std::string toStringState(FormatExpressions* format = nullptr);
  std::string toStringCurrentChain(FormatExpressions* format = nullptr);
  std::string toStringProgressReport(FormatExpressions* format = nullptr);
  std::string toStringHTML();
  std::string toStringHeader(FormatExpressions* format);
  std::string toString(FormatExpressions* format, bool writeToHardDisk);
  std::string toStringPart2(FormatExpressions* format, bool writeToHardDisk);
  std::string toStringTableSubalgebraLinksTable(FormatExpressions* format);
  std::string toStringSubalgebrasNoHDWrite(
    FormatExpressions* format, bool generateLinks
  );
  std::string toStringRealizedSubalgebraCollection(
    FormatExpressions* format,
    MapReferences<Vectors<Rational>, RealizedSemisimpleSubalgebra>& collection,
    const std::string& collectionDescription
  );
  std::string toStringNonRealizedSubalgebraCollection(
    FormatExpressions* format
  );
  std::string toStringSubalgebrasWithHDWrite(
    FormatExpressions* format = nullptr
  );
  void writeSubalgebraToFile(FormatExpressions* format, int subalgebraIndex);
  std::string toStringPart3(FormatExpressions* format, bool writeToHardDisk);
  std::string toStringSl2s(FormatExpressions* format = nullptr);
  std::string toStringSemisimpleSubalgebrasSummaryLaTeX(
    FormatExpressions* format = nullptr
  ) const;
  std::string toStringSemisimpleSubalgebraSummaryHTML(
    FormatExpressions* format = nullptr
  ) const;
  void computePairingTablesAndFKFTtypes();
  void getCentralizerChains(List<List<int> >& outputChains);
  int getIndexFullSubalgebra() const;
  bool ranksAndIndicesFit(const DynkinType& input) const;
  bool growDynkinType(
    const DynkinType& input,
    List<DynkinType>& output,
    List<List<int> >* outputImagesSimpleRoots
  ) const;
  Vector<Rational>
  getHighestWeightFundamentalNewComponentFromImagesOldSimpleRootsAndNewRoot(
    const DynkinType& input,
    const List<int>& imagesOldSimpleRootsAndNewRoot,
    CandidateSemisimpleSubalgebra& subalgebraToBeModified
  );
  void hookUpCentralizers(bool allowNonPolynomialSystemFailure);
  void computeSl2sInitOrbitsForComputationOnDemand(bool computeRealSlTwos);
  void findSemisimpleSubalgebrasInitialize();
  bool findSemisimpleSubalgebrasContinue();
  bool findSemisimpleSubalgebrasFromScratch(
    SemisimpleLieAlgebra& newOwner,
    AlgebraicClosureRationals& inputOwnerField,
    MapReferences<std::string, AlgebraicClosureRationals>&
    inputOwnerFieldsForExrtaComputations,
    MapReferences<DynkinType, SemisimpleLieAlgebra>& containerSubalgebras,
    ListReferences<SlTwoSubalgebras>& containerSl2Subalgebras,
    const DynkinType* targetType
  );
  bool computeStructureWriteFiles(
    SemisimpleLieAlgebra& newOwner,
    AlgebraicClosureRationals& inputOwnerField,
    MapReferences<std::string, AlgebraicClosureRationals>&
    inputOwnerFieldsForExrtaComputations,
    MapReferences<DynkinType, SemisimpleLieAlgebra>& containerSubalgebras,
    ListReferences<SlTwoSubalgebras>& containerSl2Subalgebras,
    std::string(*toStringExpression)(SemisimpleSubalgebras&),
    std::stringstream* outputStream,
    SemisimpleSubalgebrasComputationOptions& options,
    const std::string& extraDynkinDiagramPlot
  );
  bool computeStructureRealFormsWriteFiles(
    SemisimpleLieAlgebra& newOwner,
    AlgebraicClosureRationals& inputOwnerField,
    MapReferences<std::string, AlgebraicClosureRationals>&
    inputOwnerFieldsForExrtaComputations,
    MapReferences<DynkinType, SemisimpleLieAlgebra>& containerSubalgebras,
    ListReferences<SlTwoSubalgebras>& containerSl2Subalgebras,
    std::stringstream* outputStream
  );
  bool computeStructureRealForms(
    SemisimpleLieAlgebra& newOwner,
    AlgebraicClosureRationals& inputOwnerField,
    MapReferences<std::string, AlgebraicClosureRationals>&
    inputOwnerFieldsForExrtaComputations,
    MapReferences<DynkinType, SemisimpleLieAlgebra>& containerSubalgebras,
    ListReferences<SlTwoSubalgebras>& containerSl2Subalgebras
  );
  bool computeStructureRealFormsSlTwos();
  bool computeStructureRealFormOneSlTwo(
    SlTwoSubalgebra& input, int indexInSlTwos
  );
  bool computeStructureRealFormOneSlTwoPartTwo(
    CandidateSemisimpleSubalgebra& added, SlTwoSubalgebra& input
  );
  bool computeStructureRealFormsInitialize(
    SemisimpleLieAlgebra& newOwner,
    AlgebraicClosureRationals& inputOwnerField,
    MapReferences<std::string, AlgebraicClosureRationals>&
    inputOwnerFieldsForExrtaComputations,
    MapReferences<DynkinType, SemisimpleLieAlgebra>& containerSubalgebras,
    ListReferences<SlTwoSubalgebras>& containerSl2Subalgebras
  );
  bool writeFilesRealForms(std::stringstream* outputStream);
  void writeHCandidates(
    List<Vector<Rational> >& outputHCandidatesScaledToActByTwo,
    CandidateSemisimpleSubalgebra& newCandidate,
    DynkinType& currentType,
    List<int>& currentRootInjection
  );
  void writeHCandidatesForOneOrbit(
    const SlTwoSubalgebra& currentSubalgebra,
    const Rational& desiredHScaledToActByTwoLengthSquared,
    int baseRank,
    List<Vector<Rational> >& outputHCandidatesScaledToActByTwo,
    CandidateSemisimpleSubalgebra& newCandidate,
    List<int>& currentRootInjection
  );
  void writeOneHCandidate(
    const Vector<Rational>& currentCandidate,
    List<Vector<Rational> >& outputHCandidatesScaledToActByTwo,
    CandidateSemisimpleSubalgebra& newCandidate,
    const List<int>& currentRootInjection,
    const List<Vector<Rational> >& centralizerOfOldHs
  );
  class Test {
  public:
    static bool all();
    static bool constructAllB3Subalgebras();
  };
};

#endif // header_math_extra_semisimple_lie_subalgebras_ALREADY_INCLUDED
