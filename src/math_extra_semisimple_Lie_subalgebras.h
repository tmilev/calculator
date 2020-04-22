// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeaderSemisimpleLieSubalgebrasIncluded
#define vpfHeaderSemisimpleLieSubalgebrasIncluded

#include "math_general.h"
#include "math_extra_semisimple_Lie_algebras_root_subalgebras.h"

class SemisimpleSubalgebras;
class CandidateSSSubalgebra;
class SubalgebraSemisimpleLieAlgebra;

class NilradicalCandidate {
  public:
  CandidateSSSubalgebra* owner;
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
  void CheckInitialization() const;
  bool IsStronglySingular(int moduleIndex);
  bool IsStronglySingularRelativeToSubset(int nonFKweightIndex);
  Vector<Rational> GetConeStrongIntersectionWeight() const;
  Vector<Rational> GetNilradicalLinearCombi() const;
  void GetModGeneratedByNonHWVandNilradElt(
    int indexInNilradSubset,
    List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& outputLeft,
    List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& outputRight,
    List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& outputBrackets
  ) const;
  bool TryFindingLInfiniteRels();
  void ComputeParabolicACextendsToParabolicAC();
  bool IsCommutingSelectionNilradicalElements(Selection& inputNilradSel);
  void ProcessMe();
  std::string toString(FormatExpressions* theFormat = nullptr) const;
  std::string ToStringTableElementWithWeights(
    const List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& theElts,
    const Vectors<Rational>& theWeights
  ) const;
  void ComputeTheTwoCones();
  void ComputeTheTwoConesRelativeToNilradicalSubset();
};

class SubalgebraSemisimpleLieAlgebra {
public:
  std::string toString(FormatExpressions* theFormat = nullptr);
  SemisimpleLieAlgebra* owner;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > theGenerators;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > theBasis;
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > CartanSA;
  void ComputeBasis();
  void ComputeCartanSA();
  SubalgebraSemisimpleLieAlgebra();
  bool CheckInitialization();

};

class CandidateSSSubalgebra {
  friend std::ostream& operator << (std::ostream& output, const CandidateSSSubalgebra& theCandidate) {
    output << theCandidate.toString();
    return output;
  }

public:
  WeylGroupData* theWeylNonEmbedded;
  SemisimpleLieAlgebra* theSubalgebraNonEmbeddedDefaultScale;
  DynkinDiagramRootSubalgebra theCentralizerSubDiagram;
  DynkinType theCentralizerType;

  List<Vectors<Rational> > CartanSAsByComponentScaledToActByTwo;
  List<AlgebraicNumber> RatioKillingsByComponent;
  Vectors<Rational> theHsScaledToActByTwo;
  Vectors<Rational> theHs;
  Vectors<Rational> theHsScaledToActByTwoInOrderOfCreation;
  Matrix<Rational> BilinearFormSimplePrimal;
  Matrix<Rational> BilinearFormFundPrimal;
  Matrix<Rational> InducedEmbeddingPrimalFundCoordsIntoSimpleAmbientCoords;
  Matrix<Rational> InducedEmbeddingPrimalFundCoordsIntoFundAmbientCoords;
  Matrix<Rational> MatMultiplyFundCoordsToGetSimple;

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
  charSSAlgMod<Rational> theCharFundamentalCoordsRelativeToCartan;
  charSSAlgMod<Rational> theCharNonPrimalFundCoords;
  charSSAlgMod<Rational> thePrimalChaR;
  Vectors<Rational> PosRootsPerpendicularPrecedingWeights;
  Vectors<Rational> CartanOfCentralizer;
  List<Polynomial<AlgebraicNumber> > theSystemToSolve;
  List<Polynomial<AlgebraicNumber> > transformedSystem;
  SemisimpleSubalgebras* owner;
  int indexInOwner;
  int indexIamInducedFrom;
  List<int> RootInjectionsFromInducer;
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
  int NumCentralizerConditionFailsConeConditionHolds;
  //The highest weight vectors are by definition cartan-centralizer-split
  List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > HighestVectorsNonSorted;
  List<Vector<Rational> > HighestWeightsPrimalNonSorted;
  List<Vector<Rational> > HighestWeightsNONprimalNonSorted;

  List<List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > > HighestVectors;
  HashedList<Vector<Rational> > HighestWeightsPrimal;
  List<Vector<Rational> > HighestWeightsNONPrimal;

  List<List<List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > > > Modules;
  List<List<List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > > > ModulesSl2opposite;
  List<List<ElementSemisimpleLieAlgebra<AlgebraicNumber> > > ModulesIsotypicallyMerged;
//  List<List<ElementSemisimpleLieAlgebra<Rational> > > ModulesSemisimpleSubalgebra;

  List<Vectors<Rational> > WeightsModulesNONprimal;
  List<Vectors<Rational> > WeightsModulesPrimal;
  List<charSSAlgMod<Rational> > CharsPrimalModules;
  List<charSSAlgMod<Rational> > CharsPrimalModulesMerged;
  HashedList<Vector<Rational> > RootSystemCentralizerPrimalCoords;
  HashedList<Vector<Rational> > RootSystemSubalgebraPrimalCoords;

  List<List<List<int> > > NilradicalPairingTable;
  List<int> subalgebraModules;
  List<int> centralizerSubalgebraModules;
  List<int> primalSubalgebraModules;
  List<List<int> > OppositeModulesByStructure;
  List<List<int> > OppositeModulesByChar;

  HashedList<int, MathRoutines::IntUnsignIdentity> modulesWithZeroWeights;
  std::string comments;
  std::string nilradicalGenerationLog;
  Rational centralizerRank;
  Rational centralizerDimension;

  bool flagDeallocated;
  CandidateSSSubalgebra();
  ~CandidateSSSubalgebra();
  WeylGroupData& GetWeylNonEmbeddedDefaultScale();
  void reset(SemisimpleSubalgebras* inputOwner = nullptr);
  bool checkConsistency() const;
  bool CheckMaximalDominance() const;
  bool ComputeAndVerifyFromGeneratorsAndHs();
  int GetPrimalRank() const;
  int GetRank() const;
  int GetSSpartCentralizerOfSSPartCentralizer() const;
  void GetHsScaledToActByTwoByType(
    List<List<Vectors<Rational> > >& outputHsByType,
    List<DynkinSimpleType>& outputTypeList
  ) const;
  bool HasHsScaledByTwoConjugateTo(List<Vector<Rational> >& other) const;
  bool ComputeCentralizerTypeFailureAllowed();
  bool IsDirectSummandOf(const CandidateSSSubalgebra& other);
  void GetGenericCartanCentralizerLinearCombination(
    int indexCartanCentralizerGen,
    ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& output
  );
  void GetGenericPosGenLinearCombination(
    int indexPosGens,
    ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& output
  );
  bool IsExtremeWeight(int moduleIndex, int indexInIsoComponent) const;
  void GetGenericNegGenLinearCombination(
    int indexNegGens,
    ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& output
  );
  bool AmRegularSA() const;
  bool CompareLeftGreaterThanRight(const Vector<Rational>& left, const Vector<Rational>& right);
  void GetGenericLinearCombination(
    int numVars,
    int varOffset,
    List<ChevalleyGenerator>& involvedGens,
    ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& output
  );
  void GetGenericLinearCombinationInvolvedPosGens(
    int theIndex, ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& output
  );
  void ComputeCentralizerIsWellChosen();
  void AdjustCentralizerAndRecompute(bool allowNonPolynomialSystemFailure);
  void ComputeRatioKillingsByComponent();
  void AddToSystem(
    const ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >& elementThatMustVanish
  );
  bool CreateAndAddExtendBaseSubalgebra(
    const CandidateSSSubalgebra& baseSubalgebra,
    Vector<Rational>& newHrescaledToActByTwo,
    const DynkinType& theNewType,
    const List<int>& theRootInjection
  );
  bool ComputeSltwoDynkinIndices(const DynkinType& input);
  bool GetCentralizerTypeIfComputableAndKnown(const DynkinType& input, DynkinType& output);
  void ComputeHsAndHsScaledToActByTwoFromComponents();
  void SetUpInjectionHs(
    const CandidateSSSubalgebra& baseSubalgebra,
    const DynkinType& theNewType,
    const List<int>& theRootInjection,
    Vector<Rational>* newHScaledToActByTwo = nullptr
  );
  void EnumerateAllNilradicals();
  std::string ToStringNilradicalSelection(const List<int>& theSelection);
  void EnumerateNilradicalsRecursively(
    List<int>& theSelection,
    std::stringstream* logStream = nullptr
  );
  void ExtendNilradicalSelectionToMultFreeOverSSpartSubalgebra(
    HashedList<int, MathRoutines::IntUnsignIdentity>& inputOutput
  );
  bool IsPossibleNilradicalCarryOutSelectionImplications(
    List<int>& theSelection,
    std::stringstream* logStream = nullptr
  );
  void ExtendToModule(List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& inputOutput);
  Vector<Rational> GetPrimalWeightFirstGen(const ElementSemisimpleLieAlgebra<AlgebraicNumber>& input) const;
  Vector<Rational> GetNonPrimalWeightFirstGen(const ElementSemisimpleLieAlgebra<AlgebraicNumber>& input) const;
  void ComputeKsl2triples();
  void ComputeKsl2triplesGetOppositeEltsInOppositeModule(
    const Vector<Rational>& theElementWeight,
    const List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& inputOppositeModule,
    List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& outputElts
  );
  void ComputeKsl2triplesGetOppositeEltsAll(
    const Vector<Rational>& theElementWeight,
    List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& outputElts
  );
  bool ComputeKsl2tripleSetUpAndSolveSystem(
    const ElementSemisimpleLieAlgebra<AlgebraicNumber>& theE,
    const List<ElementSemisimpleLieAlgebra<AlgebraicNumber> >& FisLinearCombiOf,
    ElementSemisimpleLieAlgebra<AlgebraicNumber>& outputF
  );
  void ComputeCharsPrimalModules();
  void ComputePairingTable();
  void ComputeSinglePair(int leftIndex, int rightIndex, List<int>& output);
  int GetNumModules() const;
  void ComputePairKweightElementAndModule(
    const ElementSemisimpleLieAlgebra<AlgebraicNumber>& leftKweightElt,
    int rightIndex,
    List<int>& output
  );
  bool IsWeightSystemSpaceIndex(int theIndex, const Vector<Rational>& AmbientRootTestedForWeightSpace);
  void AddHincomplete(const Vector<Rational>& theH);
  bool CheckBasicInitialization() const;
  bool CheckCandidateInitialization() const;
  bool CheckFullInitializatioN() const;
  bool CheckModuleDimensions() const;
  SemisimpleLieAlgebra& GetAmbientSS() const;
  WeylGroupData& GetAmbientWeyl() const;
  WeylGroupAutomorphisms& GetAmbientWeylAutomorphisms() const;
  void ComputeCartanOfCentralizer();
  void ComputePrimalModuleDecomposition();
  void ComputePrimalModuleDecompositionHWsHWVsOnly();
  void ComputePrimalModuleDecompositionHWVsOnly(HashedList<Vector<Rational> >& inputHws);
  void ComputePrimalModuleDecompositionHighestWeightsOnly(HashedList<Vector<Rational> >& outputHWsCoords);
  void ComputePrimalModuleDecompositionHWsHWVsOnlyLastPart();
  void GetPrimalWeightProjectionFundCoords(
    const Vector<Rational>& inputAmbientWeight,
    Vector<Rational>& output
  ) const;
  bool CheckGensBracketToHs();
  void GetWeightProjectionFundCoords(
    const Vector<Rational>& inputAmbientweight,
    Vector<Rational>& output
  ) const;
  bool ComputeSystem(bool AttemptToChooseCentalizer, bool allowNonPolynomialSystemFailure);
  bool ComputeSystemPart2(bool AttemptToChooseCentalizer, bool allowNonPolynomialSystemFailure);
  bool ComputeChar(bool allowBadCharacter);
  bool AttemptToSolveSystem();
  bool IsGoodHnewActingByTwo(
    const Vector<Rational>& HNewActingByTwo,
    const List<int>& theRootInjections
  ) const;
  Rational GetScalarSA(
    const Vector<Rational>& primalWeightLeft,
    const Vector<Rational>& primalWeightRight
  ) const;
  std::string ToStringType(FormatExpressions* theFormat = nullptr) const;
  std::string ToStringTypeAndHs(FormatExpressions* theFormat = nullptr) const;
  std::string ToStringGenerators(FormatExpressions* theFormat = nullptr) const;
  std::string toString(FormatExpressions* theFormat = nullptr) const;
  std::string ToStringSystem(FormatExpressions* theFormat = nullptr) const;
  std::string ToStringSystemPart2(FormatExpressions* theFormat = nullptr) const;
  std::string ToStringLoadUnknown(FormatExpressions* theFormat = nullptr) const;
  std::string ToStringCentralizer(FormatExpressions* theFormat = nullptr) const;
  std::string ToStringCentralizerDebugData(FormatExpressions* theFormat = nullptr) const;
  std::string ToStringCartanSA(FormatExpressions* theFormat = nullptr) const;
  std::string ToStringPairingTable(FormatExpressions* theFormat = nullptr) const;
  std::string ToStringPairingTableLaTeX(FormatExpressions* theFormat = nullptr) const;
  std::string ToStringNilradicals(FormatExpressions* theFormat = nullptr) const;
  std::string ToStringNilradicalsSummary(FormatExpressions* theFormat = nullptr) const;
  std::string ToStringModuleDecomposition(FormatExpressions* theFormat = nullptr) const;
  std::string ToStringModuleDecompositionLaTeX(FormatExpressions* theFormat = nullptr) const;
  std::string ToStringDrawWeights(FormatExpressions* theFormat = nullptr) const;
  std::string ToStringDrawWeightsHelper(int indexModule, const Vector<Rational>& theWeight) const;
  //std::string ToStringDrawWeightsVersion2(FormatExpressions* theFormat = nullptr) const;
  bool operator>(const CandidateSSSubalgebra& other) const;
};

class SemisimpleSubalgebras {
public:
  FormatExpressions currentFormat;
  SemisimpleLieAlgebra* owner;
  AlgebraicClosureRationals* ownerField;
  DynkinType targetDynkinType;
  SltwoSubalgebras theSl2s;
  MapReferences<DynkinType, SemisimpleLieAlgebra>* theSubalgebrasNonEmbedded;
  MapReferences<Matrix<Rational>, SemisimpleLieAlgebra> theSubalgebrasNonDefaultCartanAndScale;
  List<List<Rational> > CachedDynkinIndicesSl2subalgebrasSimpleTypes;
  HashedList<DynkinSimpleType> CachedDynkinSimpleTypesWithComputedSl2Subalgebras;
  List<OrbitIteratorRootActionWeylGroupAutomorphisms> theOrbiTs;
  HashedList<Rational> theOrbitHelementLengths;
  HashedList<DynkinSimpleType> theOrbitDynkinIndices;
  // List<HashedList<ElementWeylGroup<WeylGroup> > > theOrbitGeneratingElts;
  // if an entry in orbit sizes is - 1 this means the corresponding orbit size has not been computed yet.
  int maxStoredOrbitSize;
  std::string comments;
  std::string fileNameToLogComments;

  //current computation state:
  // state that gets stored to hd:
  List<CandidateSSSubalgebra> currentSubalgebraChain;
  List<int> currentNumLargerTypesExplored;
  List<int> currentNumHcandidatesExplored;
  // state that is recomputed each load:
  List<List<DynkinType> > currentPossibleLargerDynkinTypes;
  List<List<List<int> > > currentRootInjections;
  List<Vectors<Rational> > currentHCandidatesScaledToActByTwo;

  //end current computation state variables.

  MapReferences<Vectors<Rational>, CandidateSSSubalgebra> theSubalgebras; //used to search for subalgebras quickly
  bool flagRealizedAllCandidates;
  bool flagAttemptToSolveSystems;
  bool flagComputePairingTable;
  bool flagComputeModuleDecomposition;
  bool flagComputeNilradicals;
  bool flagProduceLaTeXtables;
  bool flagAttemptToAdjustCentralizers;
  int64_t millisecondsComputationStart;
  int64_t millisecondsComputationEnd;
  unsigned long long int numAdditions;
  unsigned long long int numMultiplications;
  bool flagDeallocated;
  std::string DisplayNameMainFile1NoPath;
  std::string DisplayNameMainFile1WithPath;

  std::string VirtualNameMainFile1;

  // Possible values:
  // 1. nulltpr.
  // 2. CalculatorConversions::innerStringFromSemisimpleSubalgebras.
  std::string (*ToStringExpressionString)(SemisimpleSubalgebras& input);
  bool LoadState(
    List<int>& currentChainInt,
    List<int>& numExploredTypes,
    List<int>& numExploredHs,
    std::stringstream& reportStream
  );
  int GetDisplayIndexFromActual(int ActualIndexSubalgebra) const;
  std::string ToStringSubalgebraNumberWithAmbientLink(
    int actualIndexSubalgebra, FormatExpressions* theFormat
  ) const;
  void ComputeFolderNames(FormatExpressions& outputFormat);
  void CheckFileWritePermissions();
  void WriteReportToFiles();
  std::string ToStringAlgebraLink(
    int ActualIndexSubalgebra, FormatExpressions* theFormat
  ) const;
  std::string GetRelativePhysicalFileNameSubalgebra(int ActualIndexSubalgebra) const;
  std::string GetDisplayFileNameSubalgebraRelative(
    int ActualIndexSubalgebra, FormatExpressions* theFormat
  ) const;
  std::string GetDisplayFileNameSubalgebraAbsolute(
    int ActualIndexSubalgebra, FormatExpressions* theFormat
  ) const;
  std::string GetRelativePhysicalFileNameFKFTNilradicals(
    int ActualIndexSubalgebra
  ) const;
  std::string GetDisplayFileNameFKFTNilradicals(
    int ActualIndexSubalgebra, FormatExpressions* theFormat
  ) const;
  const CandidateSSSubalgebra& baseSubalgebra();
  bool operator==(const SemisimpleSubalgebras& other) {
    MacroRegisterFunctionWithName("SemisimpleSubalgebras::operator==");
    if (this->owner == other.owner) {
      return true;
    }
    if (this->owner == nullptr || other.owner == nullptr) {
      global.fatal << "This is a programming error: comparing non-initialized Semisimple Lie Subalgebras. " << global.fatal;
    }
    return *this->owner == *other.owner;
  }
  SemisimpleLieAlgebra& GetSSowner() const {
    if (this->owner == nullptr) {
      global.fatal << "This is a programming error: attempted to "
      << "access non-initialized semisimple Lie subalgerbas. " << global.fatal;
    }
    return *this->owner;
  }
  void MakeEmptyCandidateSA(CandidateSSSubalgebra& output);
  void MakeCandidateSA(const DynkinType& input, CandidateSSSubalgebra& output);
  bool IncrementReturnFalseIfPastLast();
  bool RemoveLastSubalgebra();
  bool GetCentralizerTypeIfComputableAndKnown(const DynkinType& input, DynkinType& output);
  bool CombinatorialCriteriaAllowRealization();
  bool CentralizersComputedToHaveUnsuitableNilpotentOrbits();
  bool CentralizerOfBaseComputedToHaveUnsuitableNilpotentOrbits();

  bool ComputeCurrentHCandidates();
  void initHookUpPointers(
    SemisimpleLieAlgebra& inputOwner,
    AlgebraicClosureRationals* theField,
    MapReferences<DynkinType, SemisimpleLieAlgebra>* inputSubalgebrasNonEmbedded,
    ListReferences<SltwoSubalgebras>* inputSl2sOfSubalgebras
  );
  void reset();
  ~SemisimpleSubalgebras() {
    this->flagDeallocated = true;
  }
  SemisimpleSubalgebras(): flagDeallocated(false) {
    this->reset();
  }
  void AddSubalgebraIfNewSetToStackTop(CandidateSSSubalgebra& input);
  void AddSubalgebraToStack(
    CandidateSSSubalgebra& input,
    int inputNumLargerTypesExplored,
    int inputNumHcandidatesExplored
  );
  bool SetUpParabolicInductionDataPrecomputedSA(CandidateSSSubalgebra& theCandidate);
  bool CheckConsistencyHs() const;
  bool checkConsistency() const;
  bool CheckInitialization() const;
  std::string ToStringState(FormatExpressions* theFormat = nullptr);
  std::string ToStringCurrentChain(FormatExpressions* theFormat = nullptr);
  std::string ToStringProgressReport(FormatExpressions* theFormat = nullptr);
  std::string toString(FormatExpressions* theFormat = nullptr);
  std::string ToStringPart2(FormatExpressions* theFormat = nullptr);
  std::string ToStringTableSubalgebraLinksTable(FormatExpressions* theFormat);
  std::string ToStringSubalgebrasNoHDWrite(FormatExpressions* theFormat = nullptr);
  std::string ToStringSubalgebrasWithHDWrite(FormatExpressions* theFormat = nullptr);
  void WriteSubalgebraToFile(FormatExpressions* theFormat, int subalgebraIndex);
  std::string ToStringPart3(FormatExpressions* theFormat = nullptr);
  std::string ToStringSl2s(FormatExpressions* theFormat = nullptr);
  std::string ToStringSSsumaryLaTeX(FormatExpressions* theFormat = nullptr) const;
  std::string ToStringSSsumaryHTML(FormatExpressions* theFormat = nullptr) const;
  void ComputePairingTablesAndFKFTtypes();
  void GetCentralizerChains(List<List<int> >& outputChains);
  int GetIndexFullSubalgebra() const;
  bool RanksAndIndicesFit(const DynkinType& input) const;
  bool GrowDynkinType(
    const DynkinType& input,
    List<DynkinType>& output,
    List<List<int> >* outputImagesSimpleRoots
  ) const;
  Vector<Rational>
  GetHighestWeightFundNewComponentFromImagesOldSimpleRootsAndNewRoot(
    const DynkinType& input,
    const List<int>& imagesOldSimpleRootsAndNewRoot,
    CandidateSSSubalgebra& theSSSubalgebraToBeModified
  );
  //void RegisterPossibleCandidate(CandidateSSSubalgebra& theCandidate);
  void HookUpCentralizers(bool allowNonPolynomialSystemFailure);
  void ComputeSl2sInitOrbitsForComputationOnDemand();
  void FindAllEmbeddings(DynkinSimpleType& theType, SemisimpleLieAlgebra& theOwner);
  bool FindTheSSSubalgebrasFromScratch(
    SemisimpleLieAlgebra& newOwner,
    AlgebraicClosureRationals& ownerField,
    MapReferences<DynkinType, SemisimpleLieAlgebra>& containerSubalgebras,
    ListReferences<SltwoSubalgebras>& containerSl2Subalgebras,
    const DynkinType* targetType
  );
  bool ComputeStructureWriteFiles(
    SemisimpleLieAlgebra& newOwner,
    AlgebraicClosureRationals& ownerField,
    MapReferences<DynkinType, SemisimpleLieAlgebra>& containerSubalgebras,
    ListReferences<SltwoSubalgebras>& containerSl2Subalgebras,
    std::stringstream* outputStream,
    bool forceRecompute,
    bool doFullInit,
    bool computeNilradicals,
    bool computeModuleDecomposition,
    bool attemptToSolveSystems,
    bool computePairingTable,
    bool adjustCentralizers
  );
  void FindTheSSSubalgebrasInit();
  bool FindTheSSSubalgebrasContinue();

  void GetHCandidates(
    Vectors<Rational>& outputHCandidatesScaledToActByTwo,
    CandidateSSSubalgebra& newCandidate,
    DynkinType& currentType,
    List<int>& currentRootInjection
  );
};

#endif
