// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeaderSemisimpleLieSubalgebrasIncluded
#define vpfHeaderSemisimpleLieSubalgebrasIncluded

#include "math_general.h"
#include "math_extra_semisimple_Lie_algebras_root_subalgebras.h"

class SemisimpleSubalgebras;
class CandidateSemisimpleSubalgebra;
class SubalgebraSemisimpleLieAlgebra;

class NilradicalCandidate {
  public:
  CandidateSemisimpleSubalgebra* owner;
  std::string FKnilradicalLog;
  bool flagNilradicalConesIntersect;
  bool flagNilradicalConesStronglyIntersect;
  bool flagComputedRelativelyStrongIntersections;
  bool flagRestrictedCentralizerConditionHoldS;
  bool flagParabolicACextendsToParabolicAC;

  bool flagLinfiniteRelFound;
  DynkinDiagramRootSubalgebra theLeviDiagramAmbienT, theLeviDiagramSmalL;
  //0->not selected; 1->selected; 2->undecided.
  List<int> theNilradicalSelection;
  Vector<Rational> ConeIntersection;
  Vector<Rational> ConeStrongIntersection;
  Vector<Rational> ConeRelativelyStrongIntersection;
  //  Vector<Rational> LInfiniteRelation;
  Vector<Rational> ConeSeparatingNormal;
  Vectors<Rational> theNilradicalWeights;
  Vectors<Rational> theNonFKhws;
  Vectors<Rational> theNonFKhwsStronglyTwoSided;

  List<int> ownerModulesNilradicalElements;
  List<int> ownerModulestheNonFKhwVectors;

  Vectors<Rational> leviRootsAmbienT;
  Vectors<Rational> leviRootsSmallPrimalFundCoords;

  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > theNonFKhwVectors;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > theNonFKHVectorsStronglyTwoSided;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > theNilradical;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > theNilradicalElementOpposites;
  Selection theNilradSubsel;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > theNilradicalSubset;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > theNonFKhwVectorsStrongRelativeToSubset;
  Vectors<Rational> theNilradicalSubsetWeights;
  Vectors<Rational> theNonFKhwVectorsStrongRelativeToSubsetWeights;

  NilradicalCandidate();
  void reset();
  void checkInitialization() const;
  bool isStronglySingular(int moduleIndex);
  bool isStronglySingularRelativeToSubset(int nonFKweightIndex);
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
  bool isCommutingSelectionNilradicalElements(Selection& inputNilradSel);
  void processMe();
  std::string toString(FormatExpressions* theFormat = nullptr) const;
  std::string toStringTableElementWithWeights(
    const List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& theElts,
    const Vectors<Rational>& theWeights
  ) const;
  void computeTheTwoCones();
  void computeTheTwoConesRelativeToNilradicalSubset();
};

class SubalgebraSemisimpleLieAlgebra {
public:
  std::string toString(FormatExpressions* theFormat = nullptr);
  SemisimpleLieAlgebra* owner;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > theGenerators;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > basis;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > cartanSubalgebra;
  void computeBasis();
  void computeCartanSubalgebra();
  SubalgebraSemisimpleLieAlgebra();
  bool findCartanSubalgebraCandidate(
    const List<MatrixTensor<AlgebraicNumber> >& adjointOperators,
    ElementSemisimpleLieAlgebra<AlgebraicNumber>& outputCandidate,
    const List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& currentCentralizer
  );
  bool checkInitialization();
};

class CandidateSemisimpleSubalgebra {
  friend std::ostream& operator << (std::ostream& output, const CandidateSemisimpleSubalgebra& theCandidate) {
    output << theCandidate.toString();
    return output;
  }

public:
  WeylGroupData* weylNonEmbedded;
  SemisimpleLieAlgebra* theSubalgebraNonEmbeddedDefaultScale;
  DynkinDiagramRootSubalgebra theCentralizerSubDiagram;
  DynkinType theCentralizerType;

  List<Vectors<Rational> > cartanSubalgebrasByComponentScaledToActByTwo;
  List<AlgebraicNumber> RatioKillingsByComponent;
  Vectors<Rational> theHsScaledToActByTwo;
  Vectors<Rational> theHs;
  Vectors<Rational> theHsScaledToActByTwoInOrderOfCreation;
  Matrix<Rational> bilinearFormSimplePrimal;
  Matrix<Rational> bilinearFormFundPrimal;
  Matrix<Rational> inducedEmbeddingPrimalFundCoordsIntoSimpleAmbientCoords;
  Matrix<Rational> inducedEmbeddingPrimalFundCoordsIntoFundAmbientCoords;
  Matrix<Rational> matMultiplyFundCoordsToGetSimple;

  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > thePosGens;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > theNegGens;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > theBasis;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > fullBasisByModules;
  List<int> fullBasisOwnerModules;
  List<ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > > theUnknownPosGens;
  List<ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > > theUnknownNegGens;
  List<ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > > theUnknownCartanCentralizerBasis;

//  Vector<Rational> aSolution;
  List<List<ChevalleyGenerator> > theInvolvedPosGenerators;
  List<List<ChevalleyGenerator> > theInvolvedNegGenerators;
  CharacterSemisimpleLieAlgebraModule<Rational> theCharFundamentalCoordsRelativeToCartan;
  CharacterSemisimpleLieAlgebraModule<Rational> theCharNonPrimalFundCoords;
  CharacterSemisimpleLieAlgebraModule<Rational> primalCharacter;
  Vectors<Rational> PosRootsPerpendicularPrecedingWeights;
  Vectors<Rational> CartanOfCentralizer;
  List<Polynomial<AlgebraicNumber> > theSystemToSolve;
  List<Polynomial<AlgebraicNumber> > transformedSystem;
  SemisimpleSubalgebras* owner;
  int indexInOwner;
  int indexIamInducedFrom;
  List<int> rootInjectionsFromInducer;
  int indexNonEmbeddedMeStandard;
  int indexNonEmbeddedMeNonStandardCartan;
  int indexHcandidateBeingGrown;
  int indexMaxSSContainer;
  int indexSSPartCentralizer;
  List<int> indicesDirectSummandSuperAlgebra;
  MemorySaving<FormatExpressions> charFormaT;
  bool flagSubalgebraPreloadedButNotVerified;
  bool flagSystemSolved;
  bool flagSystemProvedToHaveNoSolution;
  bool flagSystemGroebnerBasisFound;
  bool flagCentralizerIsWellChosen;
  bool flagCentralizerTypeIsComputed;
  bool flagUsedInducingSubalgebraRealization;
  int RecursionDepthCounterForNilradicalGeneration;
  int totalNumUnknownsNoCentralizer;
  int totalNumUnknownsWithCentralizer;
  long long int totalArithmeticOpsToSolveSystem;

  List<NilradicalCandidate> FKNilradicalCandidates;
  int NumConeIntersections;
  int NumCasesNoLinfiniteRelationFound;
  int NumBadParabolics;
  int NumCentralizerConditionFailsconeConditionHolds;
  //The highest weight vectors are by definition cartan-centralizer-split
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > highestVectorsNonSorted;
  List<Vector<Rational> > highestWeightsPrimalNonSorted;
  List<Vector<Rational> > highestWeightsNonPrimalNonSorted;

  List<List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > > highestVectors;
  HashedList<Vector<Rational> > highestWeightsPrimal;
  List<Vector<Rational> > highestWeightsNonPrimal;

  List<List<List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > > > modules;
  List<List<List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > > > modulesSl2opposite;
  List<List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > > ModulesIsotypicallyMerged;
//  List<List<ElementSemisimpleLieAlgebra<Rational> > > ModulesSemisimpleSubalgebra;

  List<Vectors<Rational> > WeightsModulesNONprimal;
  List<Vectors<Rational> > WeightsModulesPrimal;
  List<CharacterSemisimpleLieAlgebraModule<Rational> > CharsPrimalModules;
  List<CharacterSemisimpleLieAlgebraModule<Rational> > CharsPrimalModulesMerged;
  HashedList<Vector<Rational> > RootSystemCentralizerPrimalCoords;
  HashedList<Vector<Rational> > RootSystemSubalgebraPrimalCoords;

  List<List<List<int> > > NilradicalPairingTable;
  List<int> subalgebraModules;
  List<int> centralizerSubalgebraModules;
  List<int> primalSubalgebraModules;
  List<List<int> > OppositeModulesByStructure;
  List<List<int> > OppositeModulesByChar;

  HashedList<int, HashFunctions::hashFunction> modulesWithZeroWeights;
  std::string comments;
  std::string nilradicalGenerationLog;
  Rational centralizerRank;
  Rational centralizerDimension;

  bool flagDeallocated;
  CandidateSemisimpleSubalgebra();
  ~CandidateSemisimpleSubalgebra();
  void reset(SemisimpleSubalgebras* inputOwner = nullptr);
  bool checkConsistency() const;
  bool checkMaximalDominance() const;
  bool computeAndVerifyFromGeneratorsAndHs();
  int getPrimalRank() const;
  int getRank() const;
  int getSemisimplePartCentralizerOfSemisimplePartCentralizer() const;
  void GetHsScaledToActByTwoByType(
    List<List<Vectors<Rational> > >& outputHsByType,
    List<DynkinSimpleType>& outputTypeList
  ) const;
  bool hasHsScaledByTwoConjugateTo(List<Vector<Rational> >& other) const;
  bool computeCentralizerTypeFailureAllowed();
  bool isDirectSummandOf(const CandidateSemisimpleSubalgebra& other);
  void getGenericCartanCentralizerLinearCombination(
    int indexCartanCentralizerGen,
    ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& output
  );
  void getGenericPositiveGeneratorLinearCombination(
    int indexPosGens,
    ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& output
  );
  bool isExtremeWeight(int moduleIndex, int indexInIsoComponent) const;
  void getGenericNegativeGeneratorLinearCombination(
    int indexNegGens,
    ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& output
  );
  bool amRegularSA() const;
  bool compareLeftGreaterThanRight(const Vector<Rational>& left, const Vector<Rational>& right);
  void getGenericLinearCombination(
    int numVars,
    int varOffset,
    List<ChevalleyGenerator>& involvedGens,
    ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& output
  );
  void getGenericLinearCombinationInvolvedPosGens(
    int theIndex, ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& output
  );
  void computeCentralizerIsWellChosen();
  void adjustCentralizerAndRecompute(bool allowNonPolynomialSystemFailure);
  void computeRatioKillingsByComponent();
  void addToSystem(
    const ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& elementThatMustVanish
  );
  bool createAndAddExtendBaseSubalgebra(
    const CandidateSemisimpleSubalgebra& baseSubalgebra,
    Vector<Rational>& newHrescaledToActByTwo,
    const DynkinType& theNewType,
    const List<int>& theRootInjection
  );
  bool getCentralizerTypeIfComputableAndKnown(const DynkinType& input, DynkinType& output);
  void computeHsAndHsScaledToActByTwoFromComponents();
  void setUpInjectionHs(
    const CandidateSemisimpleSubalgebra& baseSubalgebra,
    const DynkinType& theNewType,
    const List<int>& theRootInjection,
    Vector<Rational>* newHScaledToActByTwo = nullptr
  );
  void enumerateAllNilradicals();
  std::string toStringNilradicalSelection(const List<int>& theSelection);
  void enumerateNilradicalsRecursively(
    List<int>& theSelection,
    std::stringstream* logStream = nullptr
  );
  void extendNilradicalSelectionToMultFreeOverSemisimplePartSubalgebra(
    HashedList<int, HashFunctions::hashFunction>& inputOutput
  );
  bool isPossibleNilradicalCarryOutSelectionImplications(
    List<int>& theSelection,
    std::stringstream* logStream = nullptr
  );
  void extendToModule(List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& inputOutput);
  Vector<Rational> getPrimalWeightFirstGen(const ElementSemisimpleLieAlgebra<AlgebraicNumber>& input) const;
  Vector<Rational> getNonPrimalWeightFirstGenerator(const ElementSemisimpleLieAlgebra<AlgebraicNumber>& input) const;
  void computeKsl2Triples();
  void computeKsl2TriplesGetOppositeElementsInOppositeModule(
    const Vector<Rational>& theElementWeight,
    const List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& inputOppositeModule,
    List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& outputElts
  );
  void computeKsl2TriplesGetOppositeElementsAll(
    const Vector<Rational>& theElementWeight,
    List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& outputElts
  );
  bool computeKsl2TriplesetUpAndSolveSystem(
    const ElementSemisimpleLieAlgebra<AlgebraicNumber>& theE,
    const List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& FisLinearCombiOf,
    ElementSemisimpleLieAlgebra<AlgebraicNumber>& outputF
  );
  void computeCharactersPrimalModules();
  void computePairingTable();
  void computeSinglePair(int leftIndex, int rightIndex, List<int>& output);
  int getNumberOfModules() const;
  void computePairKWeightElementAndModule(
    const ElementSemisimpleLieAlgebra<AlgebraicNumber>& leftKweightElt,
    int rightIndex,
    List<int>& output
  );
  bool isWeightSystemSpaceIndex(int theIndex, const Vector<Rational>& AmbientRootTestedForWeightSpace);
  void addHIncomplete(const Vector<Rational>& theH);
  bool checkBasicInitialization() const;
  bool checkCandidateInitialization() const;
  bool checkFullInitialization() const;
  bool checkModuleDimensions() const;
  SemisimpleLieAlgebra& getAmbientSemisimpleLieAlgebra() const;
  WeylGroupData& getAmbientWeyl() const;
  WeylGroupAutomorphisms& getAmbientWeylAutomorphisms() const;
  void computeCartanOfCentralizer();
  void computePrimalModuleDecomposition();
  void computePrimalModuleDecompositionHWsHWVsOnly();
  void computePrimalModuleDecompositionHWVsOnly(HashedList<Vector<Rational> >& inputHighestWeights);
  void computePrimalModuleDecompositionHighestWeightsOnly(HashedList<Vector<Rational> >& outputHWsCoords);
  void computePrimalModuleDecompositionHWsHWVsOnlyLastPart();
  void getPrimalWeightProjectionFundamentalCoordinates(
    const Vector<Rational>& inputAmbientWeight,
    Vector<Rational>& output
  ) const;
  bool checkGeneratorsBracketToHs();
  void getWeightProjectionFundamentalCoordinates(
    const Vector<Rational>& inputAmbientweight,
    Vector<Rational>& output
  ) const;
  bool computeSystem(bool AttemptToChooseCentalizer, bool allowNonPolynomialSystemFailure);
  bool computeSystemPart2(bool AttemptToChooseCentalizer, bool allowNonPolynomialSystemFailure);
  bool computeCharacter(bool allowBadCharacter);
  bool attemptToSolveSystem();
  bool isGoodHNewActingByTwo(
    const Vector<Rational>& HNewActingByTwo,
    const List<int>& theRootInjections
  ) const;
  Rational getScalarSubalgebra(
    const Vector<Rational>& primalWeightLeft,
    const Vector<Rational>& primalWeightRight
  ) const;
  std::string toStringType(FormatExpressions* theFormat = nullptr) const;
  std::string toStringTypeAndHs(FormatExpressions* theFormat = nullptr) const;
  std::string toStringGenerators(FormatExpressions* theFormat = nullptr) const;
  std::string toString(FormatExpressions* theFormat = nullptr) const;
  std::string toStringSystem(FormatExpressions* theFormat = nullptr) const;
  std::string toStringSystemPart2(FormatExpressions* theFormat = nullptr) const;
  std::string toStringLoadUnknown(FormatExpressions* theFormat = nullptr) const;
  std::string toStringCentralizer(FormatExpressions* theFormat = nullptr) const;
  std::string toStringCentralizerDebugData(FormatExpressions* theFormat = nullptr) const;
  std::string toStringCartanSubalgebra(FormatExpressions* theFormat = nullptr) const;
  std::string toStringPairingTable(FormatExpressions* theFormat = nullptr) const;
  std::string toStringPairingTableLaTeX(FormatExpressions* theFormat = nullptr) const;
  std::string toStringNilradicals(FormatExpressions* theFormat = nullptr) const;
  std::string toStringNilradicalsSummary(FormatExpressions* theFormat = nullptr) const;
  std::string toStringModuleDecomposition(FormatExpressions* theFormat = nullptr) const;
  std::string toStringModuleDecompositionLaTeX(FormatExpressions* theFormat = nullptr) const;
  std::string toStringDrawWeights(FormatExpressions* theFormat = nullptr) const;
  std::string toStringDrawWeightsHelper(int indexModule, const Vector<Rational>& theWeight) const;
  //std::string ToStringDrawWeightsVersion2(FormatExpressions* theFormat = nullptr) const;
  bool operator>(const CandidateSemisimpleSubalgebra& other) const;
};

class SemisimpleSubalgebras {
public:
  FormatExpressions currentFormat;
  SemisimpleLieAlgebra* owner;
  AlgebraicClosureRationals* ownerField;
  DynkinType targetDynkinType;
  SlTwoSubalgebras theSl2s;
  MapReferences<DynkinType, SemisimpleLieAlgebra>* theSubalgebrasNonEmbedded;
  MapReferences<Matrix<Rational>, SemisimpleLieAlgebra> theSubalgebrasNonDefaultCartanAndScale;
  List<List<Rational> > cachedDynkinIndicesSl2subalgebrasSimpleTypes;
  HashedList<DynkinSimpleType> cachedDynkinSimpleTypesWithComputedSl2Subalgebras;
  List<OrbitIteratorRootActionWeylGroupAutomorphisms> orbits;
  HashedList<Rational> theOrbitHelementLengths;
  HashedList<DynkinSimpleType> theOrbitDynkinIndices;
  // List<HashedList<ElementWeylGroup<WeylGroup> > > theOrbitGeneratingElts;
  // if an entry in orbit sizes is - 1 this means the corresponding orbit size has not been computed yet.
  int maxStoredOrbitSize;
  std::string comments;
  std::string fileNameToLogComments;

  //current computation state:
  // state that gets stored to hd:
  List<CandidateSemisimpleSubalgebra> currentSubalgebraChain;
  List<int> currentNumLargerTypesExplored;
  List<int> currentNumHcandidatesExplored;
  // state that is recomputed each load:
  List<List<DynkinType> > currentPossibleLargerDynkinTypes;
  List<List<List<int> > > currentRootInjections;
  List<Vectors<Rational> > currentHCandidatesScaledToActByTwo;

  //end current computation state variables.

  MapReferences<Vectors<Rational>, CandidateSemisimpleSubalgebra> theSubalgebras; //used to search for subalgebras quickly
  bool flagRealizedAllCandidates;
  bool flagAttemptToSolveSystems;
  bool flagcomputePairingTable;
  bool flagcomputeModuleDecompositionsition;
  bool flagComputeNilradicals;
  bool flagProduceLaTeXtables;
  bool flagAttemptToAdjustCentralizers;
  int64_t millisecondsComputationStart;
  int64_t millisecondsComputationEnd;
  unsigned long long int numAdditions;
  unsigned long long int numMultiplications;
  bool flagDeallocated;
  std::string displayNameMainFile1NoPath;
  std::string displayNameMainFile1WithPath;

  std::string virtualNameMainFile1;

  // Possible values:
  // 1. nulltpr.
  // 2. CalculatorConversions::innerStringFromSemisimpleSubalgebras.
  std::string (*ToStringExpressionString)(SemisimpleSubalgebras& input);
  bool loadState(
    List<int>& currentChainInt,
    List<int>& numExploredTypes,
    List<int>& numExploredHs,
    std::stringstream& reportStream
  );
  int getDisplayIndexFromActual(int actualindexSubalgebra) const;
  std::string toStringSubalgebraNumberWithAmbientLink(
    int actualindexSubalgebra, FormatExpressions* theFormat
  ) const;
  void computeFolderNames(FormatExpressions& outputFormat);
  void checkFileWritePermissions();
  void writeReportToFiles();
  std::string toStringAlgebraLink(
    int actualindexSubalgebra, FormatExpressions* theFormat
  ) const;
  std::string getRelativePhysicalFileNameSubalgebra(int actualindexSubalgebra) const;
  std::string getDisplayFileNameSubalgebraRelative(
    int actualindexSubalgebra, FormatExpressions* theFormat
  ) const;
  std::string getDisplayFileNameSubalgebraAbsolute(
    int actualindexSubalgebra, FormatExpressions* theFormat
  ) const;
  std::string getRelativePhysicalFileNameFKFTNilradicals(
    int actualindexSubalgebra
  ) const;
  std::string getDisplayFileNameFKFTNilradicals(
    int actualindexSubalgebra, FormatExpressions* theFormat
  ) const;
  const CandidateSemisimpleSubalgebra& baseSubalgebra();
  bool operator==(const SemisimpleSubalgebras& other) {
    MacroRegisterFunctionWithName("SemisimpleSubalgebras::operator==");
    if (this->owner == other.owner) {
      return true;
    }
    if (this->owner == nullptr || other.owner == nullptr) {
      global.fatal << "Comparison of non-initialized Semisimple Lie Subalgebras. " << global.fatal;
    }
    return *this->owner == *other.owner;
  }
  SemisimpleLieAlgebra& getSemisimpleOwner() const {
    if (this->owner == nullptr) {
      global.fatal << "Attempted to "
      << "access non-initialized semisimple Lie subalgerbas. " << global.fatal;
    }
    return *this->owner;
  }
  void makeEmptyCandidateSubalgebra(CandidateSemisimpleSubalgebra& output);
  void makeCandidateSubalgebra(const DynkinType& input, CandidateSemisimpleSubalgebra& output);
  bool incrementReturnFalseIfPastLast();
  bool removeLastSubalgebra();
  bool getCentralizerTypeIfComputableAndKnown(const DynkinType& input, DynkinType& output);
  bool combinatorialCriteriaAllowRealization();
  bool centralizersComputedToHaveUnsuitableNilpotentOrbits();
  bool centralizerOfBaseComputedToHaveUnsuitableNilpotentOrbits();

  bool computeCurrentHCandidates();
  void initHookUpPointers(
    SemisimpleLieAlgebra& inputOwner,
    AlgebraicClosureRationals* theField,
    MapReferences<DynkinType, SemisimpleLieAlgebra>* inputSubalgebrasNonEmbedded,
    ListReferences<SlTwoSubalgebras>* inputSl2sOfSubalgebras
  );
  void reset();
  ~SemisimpleSubalgebras() {
    this->flagDeallocated = true;
  }
  SemisimpleSubalgebras(): flagDeallocated(false) {
    this->reset();
  }
  void addSubalgebraIfNewSetToStackTop(CandidateSemisimpleSubalgebra& input);
  void addSubalgebraToStack(
    CandidateSemisimpleSubalgebra& input,
    int inputNumLargerTypesExplored,
    int inputNumHcandidatesExplored
  );
  bool setUpParabolicInductionDataPrecomputedSubalgebra(CandidateSemisimpleSubalgebra& theCandidate);
  bool checkConsistencyHs() const;
  bool checkConsistency() const;
  bool checkInitialization() const;
  std::string toStringState(FormatExpressions* theFormat = nullptr);
  std::string ToStringCurrentChain(FormatExpressions* theFormat = nullptr);
  std::string ToStringProgressReport(FormatExpressions* theFormat = nullptr);
  std::string toString(FormatExpressions* theFormat = nullptr);
  std::string toStringPart2(FormatExpressions* theFormat = nullptr);
  std::string toStringTableSubalgebraLinksTable(FormatExpressions* theFormat);
  std::string toStringSubalgebrasNoHDWrite(FormatExpressions* theFormat = nullptr);
  std::string toStringSubalgebrasWithHDWrite(FormatExpressions* theFormat = nullptr);
  void writeSubalgebraToFile(FormatExpressions* theFormat, int subalgebraIndex);
  std::string toStringPart3(FormatExpressions* theFormat = nullptr);
  std::string toStringSl2s(FormatExpressions* theFormat = nullptr);
  std::string toStringSemisimpleSubalgebrasSummaryLaTeX(FormatExpressions* theFormat = nullptr) const;
  std::string toStringSemisimpleSubalgebraSummaryHTML(FormatExpressions* theFormat = nullptr) const;
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
  getHighestWeightFundNewComponentFromImagesOldSimpleRootsAndNewRoot(
    const DynkinType& input,
    const List<int>& imagesOldSimpleRootsAndNewRoot,
    CandidateSemisimpleSubalgebra& theSSSubalgebraToBeModified
  );
  //void RegisterPossibleCandidate(CandidateSemisimpleSubalgebra& theCandidate);
  void hookUpCentralizers(bool allowNonPolynomialSystemFailure);
  void computeSl2sInitOrbitsForComputationOnDemand();
  bool findTheSemisimpleSubalgebrasFromScratch(
    SemisimpleLieAlgebra& newOwner,
    AlgebraicClosureRationals& ownerField,
    MapReferences<DynkinType, SemisimpleLieAlgebra>& containerSubalgebras,
    ListReferences<SlTwoSubalgebras>& containerSl2Subalgebras,
    const DynkinType* targetType
  );
  bool computeStructureWriteFiles(
    SemisimpleLieAlgebra& newOwner,
    AlgebraicClosureRationals& ownerField,
    MapReferences<DynkinType, SemisimpleLieAlgebra>& containerSubalgebras,
    ListReferences<SlTwoSubalgebras>& containerSl2Subalgebras,
    std::stringstream* outputStream,
    bool forceRecompute,
    bool doFullInit,
    bool computeNilradicals,
    bool computeModuleDecomposition,
    bool attemptToSolveSystems,
    bool computePairingTable,
    bool adjustCentralizers
  );
  void findTheSemisimpleSubalgebrasInitialize();
  bool findTheSemisimpleSubalgebrasContinue();

  void getHCandidates(
    Vectors<Rational>& outputHCandidatesScaledToActByTwo,
    CandidateSemisimpleSubalgebra& newCandidate,
    DynkinType& currentType,
    List<int>& currentRootInjection
  );
};

#endif
