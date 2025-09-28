#ifndef header_math_extra_semisimple_lie_algebras_root_subalgebras_ALREADY_INCLUDED
#define header_math_extra_semisimple_lie_algebras_root_subalgebras_ALREADY_INCLUDED

#include "math_extra_algebraic_numbers.h"
#include "math_extra_semisimple_lie_algebras.h"

class ConeRelation {
public:
  Vectors<Rational> alphas;
  Vectors<Rational> betas;
  List<Rational> alphaCoefficients;
  List<Rational> betaCoefficients;
  List<List<int> > alphaKComponents;
  List<List<int> > betaKComponents;
  int indexOwnerRootSubalgebra;
  bool generateAutomorphisms(ConeRelation& right);
  DynkinDiagramRootSubalgebra diagram;
  DynkinDiagramRootSubalgebra diagramRelationAndK;
  std::string relationString;
  std::string stringConnectedComponents;
  void computeDiagramAndDiagramRelationsAndK(RootSubalgebra& owner);
  void computeDiagramRelationsAndK(RootSubalgebra& owner);
  void fixRepeatingRoots(
    Vectors<Rational>& roots, List<Rational>& coefficients
  );
  void relationOneSideToString(
    std::string& output,
    const std::string& letterType,
    List<Rational>& coefficients,
    List<List<int> >& kComponents,
    Vectors<Rational>& roots,
    bool useLatex,
    RootSubalgebra& owner
  );
  int toString(
    std::string& output,
    RootSubalgebras& owners,
    bool useLatex,
    bool includeScalarsProductsEachSide,
    bool includeMixedScalarProducts
  );
  int rootsToScalarProductString(
    Vectors<Rational>& inputLeft,
    Vectors<Rational>& inputRight,
    const std::string& letterTypeLeft,
    const std::string& letterTypeRight,
    std::string& output,
    RootSubalgebra& owner
  );
  void computeConnectedComponents(
    Vectors<Rational>& input, RootSubalgebra& owner, List<List<int> >& output
  );
  void computeRelationString(
    RootSubalgebras& owner,
    bool includeScalarsProducts,
    bool includeMixedScalarProducts
  ) {
    this->toString(
      this->relationString,
      owner,
      true,
      includeScalarsProducts,
      includeMixedScalarProducts
    );
  }
  void makeLookCivilized(RootSubalgebra& owner);
  bool isStrictlyWeaklyProhibiting(
    RootSubalgebra& owner,
    Vectors<Rational>& nilradicalRoots,
    RootSubalgebras& owners,
    int indexInOwner
  );
  void fixRightHandSide(
    RootSubalgebra& owner, Vectors<Rational>& nilradicalRoots
  );
  bool leftSortedBiggerThanOrEqualToRight(List<int>& left, List<int>& right);
  void computeKComponents(
    Vectors<Rational>& input, List<List<int> >& output, RootSubalgebra& owner
  );
  void relationOneSideToStringCoordinateForm(
    std::string& output,
    List<Rational>& coefficients,
    Vectors<Rational>& roots,
    bool epsilonForm
  );
  void getSumAlphas(Vector<Rational>& output, int dimension);
  bool checkForBugs(RootSubalgebra& owner, Vectors<Rational>& nilradicalRoots);
  void sortRelation(RootSubalgebra& owner);
  bool operator==(const ConeRelation& right) {
    return this->relationString == right.relationString;
  }
  unsigned int hashFunction() const {
    return HashFunctions::hashFunction(this->relationString);
  }
  static unsigned int hashFunction(const ConeRelation& input) {
    return input.hashFunction();
  }
  ConeRelation() {
    this->indexOwnerRootSubalgebra = - 1;
  }
};

class ConeRelations: public HashedList<ConeRelation> {
public:
  int numberOfAllowedLatexLines;
  bool flagIncludeSmallerRelations;
  bool flagIncludeCoordinateRepresentation;
  bool flagIncludeSubalgebraDataInDebugString;
  std::string DebugString;
  List<std::string> CoordinateReps;
  void getLatexHeaderAndFooter(
    std::string& outputHeader, std::string& outputFooter
  );
  void toString(std::string& output, RootSubalgebras& owners, bool useLatex);
  void writeToFile(std::fstream& output);
  void readFromFile(std::fstream& input, RootSubalgebras& owner);
  void addRelationNoRepetition(ConeRelation& input, RootSubalgebras& owners);
  ConeRelations() {
    this->numberOfAllowedLatexLines = 40;
    this->flagIncludeSmallerRelations = true;
    this->flagIncludeCoordinateRepresentation = false;
    this->flagIncludeSubalgebraDataInDebugString = false;
  }
};

class SlTwoSubalgebraCandidate;

class RootSubalgebra {
  friend std::ostream& operator<<(
    std::ostream& output, RootSubalgebra& subalgebra
  ) {
    output << subalgebra.toString();
    return output;
  }
public:
  int totalHeirsRejectedNotMaximallyDominant;
  int totalHeirsRejectedSameModuleDecomposition;
  int totalHeirsRejectedBadAngles;
  int totalHeirsRejectedNotMaximalWithRespectToOuterAutomorphisms;
  List<DynkinType> parabolicallyAllowedHeirs;
  int numberOfNilradicalsAllowed;
  int numberOfConeConditionFailures;
  int numberOfRelationsWithStronglyPerpendicularDecomposition;
  int numberOfGModKTableRowsAllowedLatex;
  int totalSubalgebras;
  int indexInducingSubalgebra;
  bool flagFirstRoundCounting;
  bool flagComputeConeCondition;
  bool flagMakingProgressReport;
  bool flagDeallocated;
  bool flagAllInclusionsAreComputed;
  List<List<ElementSemisimpleLieAlgebra<Rational> > > modules;
  List<ElementSemisimpleLieAlgebra<Rational> > highestVectors;
  List<Vectors<Rational> > weightsModulesNONPrimalSimple;
  List<Vectors<Rational> > weightsModulesNONPrimalFundamental;
  List<Vectors<Rational> > weightsModulesPrimalSimple;
  List<Vector<Rational> > highestWeightsNonPrimalFundamental;
  List<Vector<Rational> > highestWeightsPrimalSimple;
  List<Vector<Rational> > lowestWeightsPrimalSimple;
  CharacterSemisimpleLieAlgebraModule<Rational>
  moduleDecompositionHighestWeights;
  // A larger subalgebra A is said to immediately contain
  // a smaller subalgebra B if
  // the addition of a single root to the root system of the smaller
  // subalgebra B generates a subalgebra isomorphic
  // to the larger subalgebra A.
  List<int> immediateContainerIndices;
  // All the subalgebras containing a
  // conjugate copy of the present subalgebra.
  HashedList<int> containerIndices;
  // All the subalgebras containing a conjugate copy of the present
  // subalgebra without any intermediate root subalgebra containers.
  List<int> minimalContainerIndices;
  List<List<List<int> > > pairingTable;
  List<int> oppositeKModules;
  DynkinDiagramRootSubalgebra dynkinDiagram;
  DynkinDiagramRootSubalgebra centralizerDiagram;
  DynkinType dynkinType;
  DynkinType centralizerDynkinType;
  List<List<int> > coneRelationsBuffer;
  List<int> coneRelationsNumberOfSameTypeComponentsTaken;
  List<DynkinDiagramRootSubalgebra> relationsDiagrams;
  FinitelyGeneratedMatrixMonoid<Rational>
  outerSAautosExtendingToAmbientAutosGenerators;
  FinitelyGeneratedMatrixMonoid<Rational> outerSAautos;
  Vectors<Rational> generatorsK;
  Vectors<Rational> simpleRootsReductiveSubalgebra;
  Vectors<Rational> simpleBasisKScaledToActByTwo;
  Vectors<Rational> simpleBasisKInOrderOfGeneration;
  Vectors<Rational> positiveRootsReductiveSubalgebra;
  HashedList<Vector<Rational> > allRootsSubalgebra;
  Selection nilradicalKModules;
  Selection centralizerKmodules;
  Vectors<Rational> testedRootsAlpha;
  Vectors<Rational> centralizerRoots;
  Vectors<Rational> simpleBasisCentralizerRoots;
  Vectors<Rational> simpleBasisKEpsilonCoordinates;
  Vectors<Rational> simpleBasisgEpsilonCoordinates;
  List<Vectors<Rational> > kModulesKEpsilonCoordinates;
  List<Vectors<Rational> > kModulesGEpsilonCoordinates;
  List<Vectors<Rational> > positiveRootsKConnectedComponents;
  List<Selection> kEnumerations;
  List<int> kComponentRanks;
  RootSubalgebras* owner;
  HashedList<Vector<Rational> > bufferForModuleGeneration;
  Matrix<Rational> scalarProductMatrixPermuted;
  Matrix<Rational> scalarProductMatrixOrdered;
  Matrix<Rational> scalarProductInvertedMatrixOrdered;
  List<Matrix<Rational> > potentialExtensionCartanSymmetrics;
  List<List<int> > potentialExtensionRootPermutations;
  List<DynkinType> potentialExtensionDynkinTypes;
  VectorSparse<Rational> moduleDecompositionAmbientAlgebraDimensionsOnly;
  RootSubalgebra();
  ~RootSubalgebra() {
    this->flagDeallocated = true;
  }
  WeylGroupData& getAmbientWeyl() const;
  WeylGroupAutomorphisms& getAmbientWeylAutomorphisms() const;
  SemisimpleLieAlgebra& getOwnerLieAlgebra() const;
  // returns - 1 if the weight/root  is not in g/k
  bool checkInitialization() const;
  bool checkConsistency() const;
  bool checkScalarProductMatrixOrdered() const;
  Vector<Rational> getFundamentalCoordinatessOverSubalgebraSemisimplePart(
    const Vector<Rational>& inputGWeightSimpleCoordinates
  ) const;
  Vector<Rational> getSimpleCoordinatesOverSubalgebraSemisimplePart(
    const Vector<Rational>& inputGWeightSimpleCoordinates
  ) const;
  inline int getNumberOfModules() const {
    return this->highestVectors.size;
  }
  int getIndexKModuleContainingRoot(const Vector<Rational>& input);
  void getCoxeterPlane(
    Vector<double>& outputBasis1, Vector<double>& outputBasis2
  );
  void getCoxeterElement(Matrix<Rational>& output);
  void computePotentialExtensions();
  void addSlTwoSubalgebraIfNew(
    SlTwoSubalgebraCandidate& candidate,
    SlTwoSubalgebras& output,
    int indexRootSubalgebraInContainer
  );
  void registerSubalgebraContainsSl2(
    SlTwoSubalgebras& output, int indexSl2, int indexRootSubalgebraInContainer
  );
  bool isEquivalentToByDiagramsAndDimensions(const RootSubalgebra& other)
  const;
  void
  computeOuterSubalgebraAutomorphismsExtendingToAmbientAutomorphismsGenerators(
  );
  bool isGeneratingSingularVectors(
    int indexKModules, Vectors<Rational>& nilradicalRoots
  );
  bool rootIsInCentralizer(const Vector<Rational>& input);
  bool isSubalgebraBorelHighest(const Vector<Rational>& input);
  bool isSubalgebraBorelLowest(const Vector<Rational>& input);
  bool compareLeftGreaterThanRight(
    const Vector<Rational>& weightLeft, const Vector<Rational>& weightRight
  );
  bool rootIsInNilradicalParabolicCentralizer(
    Selection& positiveSimpleRootsSelection, Vector<Rational>& input
  );
  void computeEpsilonCoordinatesWithRespectToSubalgebra();
  bool attemptTheTripleTrick(
    ConeRelation& relation, Vectors<Rational>& nilradicalRoots
  );
  bool attemptTheTripleTrickWRTSubalgebra(
    ConeRelation& relation,
    Vectors<Rational>& highestWeightsAllowed,
    Vectors<Rational>& nilradicalRoots
  );
  void extractRelations(
    Matrix<Rational>& matrixA,
    Matrix<Rational>& matrixX,
    Vectors<Rational>& nilradicalRoots,
    RootSubalgebras& owner,
    int indexInOwner,
    Vectors<Rational>& kSingular
  );
  bool generateIsomorphismsPreservingBorel(
    RootSubalgebra& right,
    SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms*
    outputAutomorphisms
  );
  void generateAutomorphismsPreservingBorel(
    SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms&
    outputAutomorphisms
  );
  void makeGeneratingSingularVectors(
    ConeRelation& relation, Vectors<Rational>& nilradicalRoots
  );
  bool attemptExtensionToIsomorphismNoCentralizer(
    Vectors<Rational>& domain,
    Vectors<Rational>& range,
    int recursionDepth,
    SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms*
    outputAutomorphisms,
    bool generateAllpossibleExtensions,
    bool* abortKmodule,
    Vectors<Rational>* additionalDomain,
    Vectors<Rational>* additionalRange
  );
  static bool attemptExtensionToIsomorphism(
    Vectors<Rational>& domain,
    Vectors<Rational>& range,
    SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms*
    outputAutomorphisms,
    RootSubalgebras& inputOwner,
    bool* domainAndRangeGenerateNonIsoSAs
  );
  bool checkForSmallRelations(
    ConeRelation& relation, Vectors<Rational>& nilradicalRoots
  );
  int numberOfRootsInNilradical();
  void ensureAlphasDontSumToRoot(
    ConeRelation& relation, Vectors<Rational>& nilradicalRoots
  );
  bool isARoot(const Vector<Rational>& input);
  bool isARootOrZero(const Vector<Rational>& input);
  void subalgebraEnumerationsToLinearCombinations();
  void doKRootsEnumeration();
  void computeModuleDecompositionAmbientAlgebraDimensionsOnly();
  void computeCentralizerFromKModulesAndSortKModules();
  void matrixToRelation(
    ConeRelation& output,
    Matrix<Rational>& matrixA,
    Matrix<Rational>& matrixX,
    int dimension,
    Vectors<Rational>& nilradicalRoots
  );
  void doKRootsEnumerationRecursively(int indexEnumeration);
  void makeProgressReportpossibleNilradicalComputation(RootSubalgebras& owner);
  void makeProgressReportGeneratorAutomorphisms(
    int progress, int outOf, int found
  );
  bool indexIsCompatibleWithPrevious(
    int startIndex,
    int recursionDepth,
    List<List<List<int> > >& multiplicityTable,
    List<Selection>& impliedSelections,
    List<int>& oppositeKModules,
    RootSubalgebras& owner
  );
  bool isAnIsomorphism(
    Vectors<Rational>& domain,
    Vectors<Rational>& range,
    SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms*
    outputAutomorphisms,
    Vectors<Rational>* additionalDomain,
    Vectors<Rational>* additionalRange
  );
  bool listHasNonSelectedIndexLowerThanGiven(
    int index, List<int>& input, Selection& selection
  );
  void generatePossibleNilradicalsRecursive(
    List<List<List<int> > >& multiplicityTable,
    List<Selection>& impliedSelections,
    List<int>& oppositeKModules,
    RootSubalgebras& owner,
    int indexInOwner
  );
  void generatePossibleNilradicals(
    List<Selection>& impliedSelections,
    int& parabolicsCounter,
    bool useParabolicsInNilradical,
    RootSubalgebras& owner,
    int indexInOwner
  );
  void generatePossibleNilradicalsInit(
    List<Selection>& impliedSelections, int& parabolicsCounter
  );
  bool coneConditionHolds(
    RootSubalgebras& owner, int indexInOwner, bool doExtractRelations
  );
  bool coneConditionHolds(
    RootSubalgebras& owner,
    int indexInOwner,
    Vectors<Rational>& nilradicalRoots,
    Vectors<Rational>& kSingular,
    bool doextractRelations
  );
  void possibleNilradicalComputation(
    Selection& selectionKModules, RootSubalgebras& owner, int indexInOwner
  );
  std::string toString(FormatExpressions* format = nullptr);
  std::string toStringPotentialExtensions(FormatExpressions* format = nullptr)
  const;
  std::string toStringImmediatelyContainingRegularSubalgebras();
  std::string toStringMinimallyContainingRegularSubalgebras();
  std::string toStringAllContainingRegularSubalgebras();
  void toHTML(int index, FormatExpressions* format);
  std::string toStringLieBracketTable(
    bool useLaTeX, bool useHtml, RootSubalgebra& owner
  );
  bool rootsDefineSubalgebra(Vectors<Rational>& roots);
  void generateKModuleLieBracketTable(
    List<List<List<int> > >& output, List<int>& oppositeKModules
  );
  void kModuleLieBracketKModule(
    int index1, int index2, List<int>& oppositeKModules, List<int>& output
  );
  void computeDynkinDiagramKAndCentralizer();
  bool checkRankInequality() const;
  void rejectNonMaximallyDominantAccountContainers();
  bool computeEssentialsIfNew(RootSubalgebra* baseSubalgebraWeExtend);
  void computeEssentials();
  bool checkForMaximalDominanceCartanSubalgebra();
  void computeRootsOfK();
  void computeKModules();
  void computeModuleDecomposition();
  void computeHighestVectorsHighestWeights();
  void computeModulesFromHighestVectors();
  void computeModuleFromHighestVector(int moduleIndex);
  void computeHighestWeightInTheSameKModule(
    const Vector<Rational>& input, Vector<Rational>& outputHW
  );
  void computeExtremeWeightInTheSameKModule(
    const Vector<Rational>& input,
    Vector<Rational>& outputW,
    bool lookingForHighest
  );
  void computeLowestWeightInTheSameKModule(
    const Vector<Rational>& input, Vector<Rational>& outputLW
  );
  void getLinearCombinationFromMaxRankRootsAndExtraRoot(bool doEnumeration);
  //  void commonCodeForgetLinearCombinationFromMaxRankRootsAndExtraRoot();
  void initForNilradicalGeneration();
  void initializeNoOwnerReset();
  void getLinearCombinationFromMaxRankRootsAndExtraRootMethod2();
  bool linearCombinationToString(
    const Vector<Rational>& alphaRoot,
    int coefficient,
    Vector<Rational>& linearCombination,
    std::string& output
  );
  bool linearCombinationToStringDistinguishedIndex(
    int distinguished,
    Vector<Rational>& alphaRoot,
    int coefficient,
    Vector<Rational>& linearCombination,
    std::string& output
  );
  void writeLieBracketTableAndOppositeKModulesToFile(
    std::fstream& output,
    List<List<List<int> > >& inMultTable,
    List<int>& inOpposites
  );
  void readLieBracketTableAndOppositeKModulesFromFile(
    std::fstream& input,
    List<List<List<int> > >& outLieBracketTable,
    List<int>& outOpposites
  );
  bool operator>(const RootSubalgebra& other) const;
};

class RootSubalgebras {
public:
  List<RootSubalgebra> subalgebras;
  ConeRelations badRelations;
  ConeRelations goodRelations;
  ConeRelations minimalRelations;
  List<List<int> > actionsNormalizerCentralizerNilradical;
  ListReferences<
    SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms
  > centralizerOuterIsomorphisms;
  ListReferences<
    SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms
  > centralizerIsomorphisms;
  // Code used in nilradical generation:
  List<Selection> impiedSelectionsNilradical;
  List<List<List<int> > > storedNilradicals;
  HashedList<Rational> validScales;
  SemisimpleLieAlgebra* owner;
  WeylGroupAutomorphisms weylGroupAutomorphisms;
  int parabolicsCounterNilradicalGeneration;
  List<int> totalNilradicalsBySA;
  int indexCurrentSubalgebraNilradicalsGeneration;
  int numberReductiveRootSubalgebrasToBeProcessedNilradicalsGeneration;
  List<int> countersNilradicalsGeneration;
  List<int> numberOfConeConditionHoldsBySemisimplePart;
  List<RootSubalgebra> subalgebrasOrderParabolicPseudoParabolicNeither;
  int recursionDepthNilradicalsGeneration;
  int numberOfSubalgebrasProcessed;
  int numberOfConeConditionFailures;
  int subalgebrasCounted;
  int linesPerTableLatex;
  int columnsPerTableLatex;
  int totalPseudoParabolicNonParabolic;
  int totalParabolics;
  int totalNonPseudoParabolic;
  int upperLimitElementsWeylGroup;
  int64_t extensionComputationTimeInMilliseconds;
  int64_t sortingTimeInMilliseconds;
  std::string ReportStringNonNilradicalParabolic;
  bool flagComputingLprohibitingWeights;
  bool flagUsingParabolicsInCentralizers;
  bool flagUsingActionsNormalizerCentralizerNilradical;
  bool flagNilradicalComputationInitialized;
  bool flagCountingNilradicalsOnlyNoComputation;
  bool flagComputeConeCondition;
  bool flagLookingForMinimalRels;
  bool flagStoringNilradicals;
  bool flagPrintGAPInput;
  bool flagPrintParabolicPseudoParabolicInfo;
  bool flagComputeInclusions;
  SemisimpleLieAlgebra& getOwnerSemisimpleLieAlgebra() const;
  WeylGroupData& getOwnerWeyl() const;
  bool checkInitialization() const;
  bool growDynkinType(
    const DynkinType& input,
    List<DynkinType>& output,
    List<List<int> >* outputPermutationSimpleRoots
  ) const;
  void findAllExtensions(
    RootSubalgebra& toBeExtended,
    int indexOfToBeExtended,
    ProgressReport& progressReport
  );
  void maybeExtendByHighestWeightOfModuleIndex(
    RootSubalgebra& toBeExtended, int indexOfToBeExtended, int j
  );
  void computeKmodMultTables();
  bool approveKModuleSelectionWRTActionsNormalizerCentralizerNilradical(
    Selection& targetSelection
  );
  bool approveSelAgainstOneGenerator(
    List<int>& generator, Selection& targetSelection
  );
  void raiseSelectionUntilApproval(Selection& targetSelection);
  void applyOneGenerator(List<int>& generator, Selection& targetSelection);
  void generateActionKintersectBIsos(RootSubalgebra& rootSubalgebra);
  void generateKintersectBOuterIsos(RootSubalgebra& rootSubalgebra);
  void computeActionNormalizerOfCentralizerIntersectNilradical(
    Selection& selectedBasisRoots, RootSubalgebra& rootSubalgebra
  );
  void computeNormalizerOfCentralizerIntersectNilradical(
    Selection& selectedBasisRoots, RootSubalgebra& rootSubalgebra
  );
  void computeAllReductiveRootSubalgebrasInitialize();
  void computeAllReductiveRootSubalgebrasUpToIsomorphism();
  void computeAllReductiveRootSubalgebrasUpToIsomorphismOLD(
    bool sort, bool computeEpsilonCoordinates
  );
  void computeParabolicPseudoParabolicNeitherOrder();
  bool isNewSubalgebra(RootSubalgebra& input);
  int getindexSubalgebraIsomorphicTo(RootSubalgebra& input);
  int getIndexUpToEquivalenceByDiagramsAndDimensions(
    const RootSubalgebra& subalgebra
  );
  int indexSubalgebra(RootSubalgebra& input);
  // Computes the immediate inclusions between the subalgebras.
  // Here, by immediate inclusion we mean that the smaller root system gets
  // only one single root added to it to generate the larger subalgebra.
  void computeImmediateInclusions();
  void computeImmediateInclusionsOnce(
    RootSubalgebra& toBeModified, int indexOfToBeModified
  );
  void computeAllInclusions();
  void computeAllInclusionsOfOneSubalgebra(
    RootSubalgebra& toBeModified, int recursionDepth
  );
  int findIndexOfRootSubalgebraWithPermutedSimpleComponents(
    RootSubalgebra& toBeFound
  );
  void sortDescendingOrderBySemisimpleRank();
  void toStringRootSpaces(
    std::string& output, bool includeMatrixForm, Vectors<Rational>& input
  );
  void toStringConeConditionNotSatisfying(
    std::string& output, bool includeMatrixForm
  );
  void toStringCentralizerIsomorphisms(
    std::string& output,
    bool useLatex,
    bool useHtml,
    int fromIndex,
    int amountToProcess
  );
  void toHTML(FormatExpressions* format);
  std::string toString(FormatExpressions* format);
  std::string toStringAlgebraLink(int index) const;
  std::string toStringAlgebraLinks(const List<int>& subalgebras) const;
  std::string toStringDynkinTableHTML(FormatExpressions* format);
  std::string toStringDynkinTableFormatToLaTeX(FormatExpressions* format);
  void computeLProhibitingRelations();
  void computeAllRootSubalgebrasUpToIsomorphism(
    int startingIndex, int numberToBeProcessed
  );
  void makeProgressReportAutomorphisms(
    SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms&
    subgroup,
    RootSubalgebra& rootSubalgebra
  );
  void initOwnerMustBeNonZero();
  void initForNilradicalGeneration();
  RootSubalgebras();
};

#endif // header_math_extra_semisimple_lie_algebras_root_subalgebras_ALREADY_INCLUDED
