// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeaderSemisimpleLieAlgebrasRootSubalgebras
#define vpfHeaderSemisimpleLieAlgebrasRootSubalgebras

#include "math_extra_semisimple_lie_algebras.h"

class ConeRelation {
public:
  Vectors<Rational> Alphas;
  Vectors<Rational> Betas;
  List<Rational> AlphaCoeffs;
  List<Rational> BetaCoeffs;
  List<List<int> > AlphaKComponents;
  List<List<int> > BetaKComponents;
  int IndexOwnerRootSubalgebra;
  bool generateAutomorphisms(ConeRelation& right);
  DynkinDiagramRootSubalgebra diagram;
  DynkinDiagramRootSubalgebra theDiagramRelAndK;
  std::string relationString;
  std::string stringConnectedComponents;
  void computeDiagramAndDiagramRelationsAndK(RootSubalgebra& owner);
  void computeDiagramRelationsAndK(RootSubalgebra& owner);
  void fixRepeatingRoots(Vectors<Rational>& theRoots, List<Rational>& coeffs);
  void relationOneSideToString(
    std::string& output,
    const std::string& letterType,
    List<Rational>& coeffs,
    List<List<int> >& kComponents,
    Vectors<Rational>& theRoots,
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
  void computeConnectedComponents(Vectors<Rational>& input, RootSubalgebra& owner, List<List<int> >& output);
  void computeRelationString(RootSubalgebras& owner, bool includeScalarsProducts, bool includeMixedScalarProducts) {
    this->toString(this->relationString, owner, true, includeScalarsProducts, includeMixedScalarProducts);
  }
  void makeLookCivilized(RootSubalgebra& owner);
  bool IsStrictlyWeaklyProhibiting(
    RootSubalgebra& owner, Vectors<Rational>& NilradicalRoots, RootSubalgebras& owners, int indexInOwner
  );
  void FixRightHandSide(RootSubalgebra& owner, Vectors<Rational>& NilradicalRoots);
  bool leftSortedBiggerThanOrEqualToRight(List<int>& left, List<int>& right);
  void ComputeKComponents(Vectors<Rational>& input, List<List<int> >& output, RootSubalgebra& owner);
  void RelationOneSideToStringCoordForm(
    std::string& output, List<Rational>& coeffs, Vectors<Rational>& theRoots, bool EpsilonForm
  );
  void getSumAlphas(Vector<Rational>& output, int theDimension);
  bool checkForBugs(RootSubalgebra& owner, Vectors<Rational>& NilradicalRoots);
  void sortRelation(RootSubalgebra& owner);
  bool operator==(const ConeRelation& right) {
    return this->relationString == right.relationString;
  }
  unsigned int hashFunction() const {
    return MathRoutines::hashString(this->relationString);
  }
  static unsigned int hashFunction(const ConeRelation& input) {
    return input.hashFunction();
  }
  ConeRelation() {
   this->IndexOwnerRootSubalgebra = - 1;
  }
};

class ConeRelations: public HashedList<ConeRelation> {
public:
  int NumAllowedLatexLines;
  bool flagIncludeSmallerRelations;
  bool flagIncludeCoordinateRepresentation;
  bool flagIncludeSubalgebraDataInDebugString;
  std::string DebugString;
  List<std::string> CoordinateReps;
  void getLatexHeaderAndFooter(std::string& outputHeader, std::string& outputFooter);
  void toString(std::string& output, RootSubalgebras& owners, bool useLatex);
  void ComputeDebugString(RootSubalgebras& owners);
  void writeToFile(std::fstream& output);
  void readFromFile(std::fstream& input, RootSubalgebras& owner);
  void addRelationNoRepetition(ConeRelation& input, RootSubalgebras& owners);
  ConeRelations() {
    this->NumAllowedLatexLines = 40;
    this->flagIncludeSmallerRelations = true;
    this->flagIncludeCoordinateRepresentation = false;
    this->flagIncludeSubalgebraDataInDebugString = false;
  }
};

class RootSubalgebra {
  friend std::ostream& operator << (std::ostream& output, RootSubalgebra& subalgebra) {
    output << subalgebra.toString();
    return output;
  }
public:
  int numHeirsRejectedNotMaximallyDominant;
  int numHeirsRejectedSameModuleDecompo;
  int numHeirsRejectedBadAngleS;
  int numHeirsRejectedNotMaxWRTouterAuto;

  List<DynkinType> parabolicallyAllowedHeirs;

  int numberOfNilradicalsAllowed;
  int numberOfConeConditionFailures;
  int NumRelationsWithStronglyPerpendicularDecomposition;
  //int NumRelationsgreaterLengthThan2;
  int NumGmodKtableRowsAllowedLatex;
  int NumTotalSubalgebras;
  int indexInducingSubalgebra;
  bool flagFirstRoundCounting;
  bool flagComputeConeCondition;
  bool flagMakingProgressReport;
  bool flagDeallocated;
  List<List<ElementSemisimpleLieAlgebra<Rational> > > modules;
  List<ElementSemisimpleLieAlgebra<Rational> > highestVectors;
  List<Vectors<Rational> > WeightsModulesNONPrimalSimple;
  List<Vectors<Rational> > WeightsModulesNONPrimalFundamental;
  List<Vectors<Rational> > WeightsModulesPrimalSimple;
  List<Vector<Rational> > highestWeightsNonPrimalFundamental;

  List<Vector<Rational> > highestWeightsPrimalSimple;
  List<Vector<Rational> > LowestWeightsPrimalSimple;

//  HashedList<Vector<Rational> > highestWeightsPrimal;

  CharacterSemisimpleLieAlgebraModule<Rational> ModuleDecompoHighestWeights;
  List<int> indicesSubalgebrasContainingK;
  List<List<List<int> > > theMultTable;
  List<int> theOppositeKmods;
  DynkinDiagramRootSubalgebra dynkinDiagram;
  DynkinDiagramRootSubalgebra theCentralizerDiagram;
  DynkinType theDynkinType;
  DynkinType theCentralizerDynkinType;
  List<List<int> > coneRelationsBuffer;
  List<int> coneRelationsNumSameTypeComponentsTaken;
  List<DynkinDiagramRootSubalgebra> relationsDiagrams;
  FinitelyGeneratedMatrixMonoid<Rational> outerSAautosExtendingToAmbientAutosGenerators;
  FinitelyGeneratedMatrixMonoid<Rational> outerSAautos;
  Vectors<Rational> genK;
  Vectors<Rational> simpleRootsReductiveSubalgebra;
  Vectors<Rational> simpleBasisKScaledToActByTwo;
  Vectors<Rational> simpleBasisKinOrderOfGeneration;
  Vectors<Rational> positiveRootsReductiveSubalgebra;
  HashedList<Vector<Rational> > allRootsSubalgebra;
  Selection nilradicalKmodules;
  Selection centralizerKmodules;
  Vectors<Rational> testedRootsAlpha;
  Vectors<Rational> CentralizerRoots;
  Vectors<Rational> SimpleBasisCentralizerRoots;
  Vectors<Rational> SimpleBasisKEpsCoords;
  Vectors<Rational> SimpleBasisgEpsCoords;
  List<Vectors<Rational> > kModulesKepsCoords;
  List<Vectors<Rational> > kModulesgEpsCoords;
  List<Vectors<Rational> > positiveRootsKConnectedComponents;
  List<Selection> theKEnumerations;
  List<int> theKComponentRanks;
  RootSubalgebras* owner;
  HashedList<Vector<Rational> > bufferForModuleGeneration;
  Matrix<Rational> scalarProdMatrixPermuted, scalarProdMatrixOrdered;
  Matrix<Rational> scalarProdInvertedMatrixOrdered;

  List<Matrix<Rational> > potentialExtensionCartanSymmetrics;
  List<List<int> > potentialExtensionRootPermutations;
  List<DynkinType> potentialExtensionDynkinTypes;
  VectorSparse<Rational> moduleDecompoAmbientAlgebraDimensionsOnly;
  RootSubalgebra();
  ~RootSubalgebra() {
    this->flagDeallocated = true;
  }
  WeylGroupData& getAmbientWeyl() const;
  WeylGroupAutomorphisms& getAmbientWeylAutomorphisms() const;
  SemisimpleLieAlgebra& getOwnerLieAlgebra() const;
  //returns - 1 if the weight/root  is not in g/k
  bool checkInitialization() const;
  bool checkConsistency() const;
  bool checkScalarProductMatrixOrdered() const;
  Vector<Rational> getFundamentalCoordinatessOverSubalgebraSemisimplePart(const Vector<Rational>& inputGweightSimpleCoords) const;
  Vector<Rational> getSimpleCoordinatesOverSubalgebraSemisimplePart(const Vector<Rational>& inputGweightSimpleCoords) const;
  inline int getNumberOfModules() const {
    return this->highestVectors.size;
  }
  int getIndexKModuleContainingRoot(const Vector<Rational>& input);
  void getCoxeterPlane(Vector<double>& outputBasis1, Vector<double>& outputBasis2);
  void getCoxeterElement(Matrix<Rational>& output);
  void computePotentialExtensions();
  void getSsl2SubalgebrasAppendListNoRepetition(
    SlTwoSubalgebras& output, int indexRootSAinContainer, AlgebraicClosureRationals* algebraicClosure
  );
  bool isEquivalentToByDiagramsAndDimensions(const RootSubalgebra& other) const;
  void computeOuterSubalgebraAutomorphismsExtendingToAmbientAutomorphismsGenerators();
  bool isGeneratingSingularVectors(int indexKmod, Vectors<Rational>& NilradicalRoots);
  bool rootIsInCentralizer(const Vector<Rational>& input);
  bool isSubalgebraBorelHighest(const Vector<Rational>& input);
  bool isSubalgebraBorelLowest(const Vector<Rational>& input);
  bool compareLeftGreaterThanRight(const Vector<Rational>& weightLeft, const Vector<Rational>& weightRight);
  bool rootIsInNilradicalParabolicCentralizer(Selection& positiveSimpleRootsSel, Vector<Rational>& input);
  void computeEpsilonCoordinatesWithRespectToSubalgebra();
  bool attemptTheTripleTrick(ConeRelation& theRel, Vectors<Rational>& NilradicalRoots);
  bool attemptTheTripleTrickWRTSubalgebra(
    ConeRelation& theRel, Vectors<Rational>& highestWeightsAllowed, Vectors<Rational>& NilradicalRoots
  );
  void extractRelations(
    Matrix<Rational>& matA,
    Matrix<Rational>& matX,
    Vectors<Rational>& NilradicalRoots,
    RootSubalgebras& owner,
    int indexInOwner,
    Vectors<Rational>& Ksingular
  );
  bool generateIsomorphismsPreservingBorel(
    RootSubalgebra& right, SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms* outputAutomorphisms
  );
  void generateAutomorphismsPreservingBorel(
    SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms& outputAutomorphisms
  );
  void makeGeneratingSingularVectors(ConeRelation& theRelation, Vectors<Rational>& nilradicalRoots);
  bool attemptExtensionToIsomorphismNoCentralizer(
    Vectors<Rational>& Domain,
    Vectors<Rational>& Range,
    int RecursionDepth,
    SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms* outputAutomorphisms,
    bool GenerateAllpossibleExtensions,
    bool* abortKmodule,
    Vectors<Rational>* additionalDomain,
    Vectors<Rational>* additionalRange
  );
  static bool attemptExtensionToIsomorphism(
    Vectors<Rational>& domain,
    Vectors<Rational>& range,
    SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms* outputAutomorphisms,
    RootSubalgebras& inputOwner,
    bool* domainAndRangeGenerateNonIsoSAs
  );
  bool checkForSmallRelations(ConeRelation& theRel, Vectors<Rational>& nilradicalRoots);
  int numberOfRootsInNilradical();
  void ensureAlphasDontSumToRoot(ConeRelation& theRel, Vectors<Rational>& NilradicalRoots);
  bool isARoot(const Vector<Rational>& input);
  bool isARootOrZero(const Vector<Rational>& input);
  void subalgebraEnumerationsToLinearCombinations();
  void doKRootsEnumeration();
  void computeModuleDecompositionAmbientAlgebraDimensionsOnly();
  void computeCentralizerFromKModulesAndSortKModules();
  void matrixToRelation(
    ConeRelation& output,
    Matrix<Rational>& matA,
    Matrix<Rational>& matX,
    int dimension,
    Vectors<Rational>& nilradicalRoots
  );
  void doKRootsEnumerationRecursively(int indexEnumeration);
  void makeProgressReportpossibleNilradicalComputation(RootSubalgebras& owner);
  void makeProgressReportGeneratorAutomorphisms(int progress, int outOf, int found);
  bool indexIsCompatibleWithPrevious(
    int startIndex,
    int RecursionDepth,
    List<List<List<int> > >& multTable,
    List<Selection>& impliedSelections,
    List<int>& oppositeKmods,
    RootSubalgebras& owner
  );
  bool isAnIsomorphism(
    Vectors<Rational>& domain,
    Vectors<Rational>& range,
    SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms* outputAutomorphisms,
    Vectors<Rational>* additionalDomain,
    Vectors<Rational>* additionalRange
  );
  bool listHasNonSelectedIndexLowerThanGiven(int index, List<int>& tempList, Selection& tempSel);
  void generatePossibleNilradicalsRecursive(
    PauseThread& PauseMutex,
    List<List<List<int> > >& multTable,
    List<Selection>& impliedSelections,
    List<int>& oppositeKmods,
    RootSubalgebras& owner,
    int indexInOwner
  );
  void generatePossibleNilradicals(
    PauseThread& PauseMutex,
    List<Selection>& impliedSelections,
    int& parabolicsCounter,
    bool useParabolicsInNilradical,
    RootSubalgebras& owner,
    int indexInOwner
  );
  void generatePossibleNilradicalsInit(List<Selection>& impliedSelections, int& parabolicsCounter);
  bool coneConditionHolds(RootSubalgebras& owner, int indexInOwner, bool doextractRelations);
  bool coneConditionHolds(
    RootSubalgebras& owner,
    int indexInOwner,
    Vectors<Rational>& NilradicalRoots,
    Vectors<Rational>& Ksingular,
    bool doextractRelations
  );
  void possibleNilradicalComputation(Selection& selKmods, RootSubalgebras& owner, int indexInOwner);
  std::string toString(FormatExpressions* format = nullptr);
  void toHTML(int index, FormatExpressions* format);
  std::string toStringLieBracketTable(bool useLaTeX, bool useHtml, RootSubalgebra& owner);
  bool rootsDefineSubalgebra(Vectors<Rational>& theRoots);
  void generateKModuleLieBracketTable(List<List<List<int> > >& output, List<int>& oppositeKmods);
  void KModuleLieBracketKmodule(int index1, int index2, List<int>& oppositeKmods, List<int>& output);
  void computeDynkinDiagramKAndCentralizer();
  bool checkRankInequality() const;
  bool computeEssentialsIfNew();
  void computeEssentials();
  bool checkForMaximalDominanceCartanSubalgebra();
  void computeRootsOfK();
  void computeKModules();
  void computeModuleDecomposition();
  void computeHighestVectorsHighestWeights();
  void computeModulesFromHighestVectors();
  void computeModuleFromHighestVector(int moduleIndex);

  void computeHighestWeightInTheSameKModule(const Vector<Rational>& input, Vector<Rational>& outputHW);
  void computeExtremeWeightInTheSameKModule(const Vector<Rational>& input, Vector<Rational>& outputW, bool lookingForHighest);
  void computeLowestWeightInTheSameKModule(const Vector<Rational>& input, Vector<Rational>& outputLW);
  void getLinearCombinationFromMaxRankRootsAndExtraRoot(bool DoEnumeration);
//  void commonCodeForgetLinearCombinationFromMaxRankRootsAndExtraRoot();
  void initForNilradicalGeneration();
  void initNoOwnerReset();
  void getLinearCombinationFromMaxRankRootsAndExtraRootMethod2();
  bool linearCombinationToString(const Vector<Rational>& alphaRoot, int coeff, Vector<Rational>& linComb, std::string& output);
  bool linearCombinationToStringDistinguishedIndex(
    int distinguished, Vector<Rational>& alphaRoot, int coeff, Vector<Rational>& linComb, std::string& output
  );
  void writeLieBracketTableAndOppositeKModulesToFile(
    std::fstream& output, List<List<List<int> > >& inMultTable, List<int>& inOpposites
  );
  void readLieBracketTableAndOppositeKModulesFromFile(
    std::fstream& input, List<List<List<int> > >& outMultTable, List<int>& outOpposites
  );
  bool operator>(const RootSubalgebra& other) const;
};

class RootSubalgebras {
public:
  List<RootSubalgebra> subalgebras;
  ConeRelations badRelations;
  ConeRelations goodRelations;
  ConeRelations theMinRels;
  List<List<int> > ActionsNormalizerCentralizerNilradical;
  ListReferences<SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms> centralizerOuterIsomorphisms;
  ListReferences<SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms> CentralizerIsomorphisms;
  //Code used in nilradical generation:
  List<Selection> ImpiedSelectionsNilradical;
  List<List<List<int> > > storedNilradicals;
  HashedList<Rational> validScales;
  SemisimpleLieAlgebra* owner;
  WeylGroupAutomorphisms theWeylGroupAutomorphisms;
  int parabolicsCounterNilradicalGeneration;
  List<int> numNilradicalsBySA;
  int IndexCurrentSANilradicalsGeneration;
  int NumReductiveRootSAsToBeProcessedNilradicalsGeneration;
  List<int> CountersNilradicalsGeneration;
  List<int> NumconeConditionHoldsBySSpart;
  List<RootSubalgebra> theSubalgebrasOrder_Parabolic_PseudoParabolic_Neither;
  int RecursionDepthNilradicalsGeneration;
  int NumSubalgebrasProcessed;
  int numberOfConeConditionFailures;
  int NumSubalgebrasCounted;
  int NumLinesPerTableLatex;
  int NumColsPerTableLatex;
  int NumPseudoParabolicNonParabolic;
  int NumParabolic;
  int NumNonPseudoParabolic;
  int UpperLimitNumElementsWeyl;
  std::string ReportStringNonNilradicalParabolic;
  bool flagComputingLprohibitingWeights;
  bool flagUsingParabolicsInCentralizers;
  bool flagUsingActionsNormalizerCentralizerNilradical;
  bool flagNilradicalComputationInitialized;
  bool flagCountingNilradicalsOnlyNoComputation;
  bool flagComputeConeCondition;
  bool flagLookingForMinimalRels;
  bool flagStoringNilradicals;
  bool flagPrintGAPinput;
  bool flagPrintParabolicPseudoParabolicInfo;
  SemisimpleLieAlgebra& GetOwnerSSalgebra() const;
  WeylGroupData& getOwnerWeyl() const;
  bool checkInitialization() const;
  bool growDynkinType(const DynkinType& input, List<DynkinType>& output, List<List<int> >* outputPermutationSimpleRoots) const;
  void computeKmodMultTables();
  bool approveKModuleSelectionWRTActionsNormalizerCentralizerNilradical(Selection& targetSel);
  bool approveSelAgainstOneGenerator(List<int>& generator, Selection& targetSel);
  void raiseSelectionUntilApproval(Selection& targetSel);
  void applyOneGenerator(List<int>& generator, Selection& targetSel);
  void generateActionKintersectBIsos(RootSubalgebra& theRootSA);
  void generateKintersectBOuterIsos(RootSubalgebra& theRootSA);
  void computeActionNormalizerOfCentralizerIntersectNilradical(Selection& SelectedBasisRoots, RootSubalgebra& theRootSA);
  void computeNormalizerOfCentralizerIntersectNilradical(Selection& SelectedBasisRoots, RootSubalgebra& theRootSA);

  void computeAllReductiveRootSAsInit();
  void computeAllReductiveRootSubalgebrasUpToIsomorphism();
  void computeAllReductiveRootSubalgebrasUpToIsomorphismOLD(bool sort, bool computeEpsilonCoordinates);
  void computeParabolicPseudoParabolicNeitherOrder();
  bool isNewSubalgebra(RootSubalgebra& input);
  int getindexSubalgebraIsomorphicTo(RootSubalgebra& input);
  int getIndexUpToEquivalenceByDiagramsAndDimensions(const RootSubalgebra& theSA);
  int indexSubalgebra(RootSubalgebra& input);
  void computeAllReductiveRootSubalgebrasContainingInputUpToIsomorphismOLD(
    List<RootSubalgebra>& bufferSAs, int RecursionDepth
  );
  void sortDescendingOrderBySSRank();
  void toStringRootSpaces(std::string& output, bool includeMatrixForm, Vectors<Rational>& input);
  void toStringConeConditionNotSatisfying(std::string& output, bool includeMatrixForm);
  void toStringCentralizerIsomorphisms(std::string& output, bool useLatex, bool useHtml, int fromIndex, int NumToProcess);
  void toHTML(FormatExpressions* format);
  std::string toString(FormatExpressions* format);
  std::string toStringAlgebraLink(int index);
  std::string toStringDynkinTableHTML(FormatExpressions* format);
  std::string toStringDynkinTableFormatToLaTeX(FormatExpressions* format);
  void computeLProhibitingRelations();
  void computeAllRootSubalgebrasUpToIsomorphism(int StartingIndex, int NumToBeProcessed);
  void makeProgressReportAutomorphisms(
    SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms& theSubgroup, RootSubalgebra& theRootSA
  );
  void initOwnerMustBeNonZero();
  void initForNilradicalGeneration();
  RootSubalgebras();
};

// template <>
// class ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >;

class SlTwoSubalgebra {
public:
  friend std::ostream& operator << (std::ostream& output, const SlTwoSubalgebra& sl2) {
    output << sl2.toString();
    return output;
  }
  SemisimpleLieAlgebra* owner;
  SlTwoSubalgebras* container;
  AlgebraicClosureRationals* algebraicClosure;
  CharacterSemisimpleLieAlgebraModule<Rational> moduleDecompositionAmbientSubalgebra;
  List<int> moduleDimensions;
  Rational lengthHSquared;
  int indexInContainer;
  int dimensionCentralizer;
  DynkinType centralizerTypeIfKnown;
  Rational dimCentralizerToralPart;
  bool flagCentralizerTypeComputed;
  bool flagCentralizerIsRegular;
  List<int> indicesContainingRootSubalgebras;
  List<int> indicesMinimalContainingRootSubalgebras;
  Vectors<Rational> preferredAmbientSimpleBasis;

  Vectors<Rational> participatingPositiveRoots;

  ElementSemisimpleLieAlgebra<Rational> hElement;
  // Mathematically equal to the previous.
  ElementSemisimpleLieAlgebra<AlgebraicNumber> hAlgebraic;
  // Mathematically equal to the previous.
  ElementSemisimpleLieAlgebra<Polynomial<Rational> > hPolynomialRational;
  // Mathematically equal to the previous.
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > hPolynomialAlgebraic;

  // One possible element e for which [e, f] = h
  // [h, e] = 2e.
  ElementSemisimpleLieAlgebra<Rational> eElement;
  // One possible element f for which [e, f] = h
  // [h, f] = -2f.
  ElementSemisimpleLieAlgebra<Rational> fElement;

  ElementSemisimpleLieAlgebra<Polynomial<Rational> > eUnknown;
  ElementSemisimpleLieAlgebra<Polynomial<Rational> > fUnknown;
  ElementSemisimpleLieAlgebra<Polynomial<Rational> > eBracketFMinusHUnknown;
  // The polynomial system required to solve to find e, f.
  PolynomialSubstitution<Rational> systemToSolve;

  // Used to check our work over the rationals
  ElementSemisimpleLieAlgebra<Rational> hBracketE, hBracketF, eBracketF;

  // The following describes an optional Cartan involution.
  // A map \theta is a Cartan involution if the following hold.
  // \theta is linear
  // \theta^2 = id.
  // \theta is a lie bracket automorphism, i.e., [\theta(a), \theta(b)] = \theta([a,b]).
  // Here, if theta is not null, we request that it
  // correspond to the maximally compact real form and have the property
  // \theta(h) = -h.
  // TODO(tmilev): does the property above need to be tweaked when
  // we compute outside of the maximally compact real form?
  LinearMapSemisimpleLieAlgebra<Rational>* cartanInvolutionPreservedByEMinusF;

  // Fix one Cartan involution theta for which theta(h) = -h.
  //
  // The following element has all properties that the rational element e has,
  // as well as the following additional property:
  //
  // 1) e - f is invariant with respect the cartan involution, i.e., theta(e-f)=e-f.
  //
  // An h, e, f-triple as above is called a Kostant-Sekiguchi sl(2)-triple.
  ElementSemisimpleLieAlgebra<AlgebraicNumber> eKostantSekiguchi;
  // F element of the Kostant-Sekiguchi sl(2)-triple.
  ElementSemisimpleLieAlgebra<AlgebraicNumber> fKostantSekiguchi;

  // Same as hKostantSekiguchi but with polynomial coefficients.
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > hKnownKostantSekiguchi;
  // An element of the form x_1 g_{k_1} + ... + x_s g_{k_s} where
  // x_1, ..., x_s are unknowns and g_{?} are the Chevalley generators
  // of the root spaces given by participatingPositiveRoots.
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > eKostantSekiguchiUnknown;
  // An element of the form x_1 g_{-k_1} + ... + x_s g_{-k_s} where
  // x_1, ..., x_s are unknowns and g_{-?} are the Chevalley generators
  // of the root spaces given by the negatives of the participatingPositiveRoots.
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > fKostantSekiguchiUnknown;
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > eBracketFMinusHUnknownKostantSekiguchi;

  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > eMinusFUnknown;
  ElementSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> > involutionAppliedToEMinusF;

  // The polynomial system required to solve to find a Kostant-sekiguchi triple e, f, h.
  // Here, we assume that theta(h) = - h is already satisfied.
  PolynomialSubstitution<AlgebraicNumber> systemToSolveKostantSekiguchi;

  // Arbitrarily chosen coefficients of the F element over the rationals.
  // This arbitrary choice of F allows to reduce
  // the polynomial system needed to realize the E and F
  // to a linear one.
  // The rationale for this arbitrary choice:
  // 1) Speed (linear systems are easer to solve than polynomial systems).
  // 2) Algorithmic simplicity. When this function was first designed,
  // we did not have machinery for solving polynomial systems
  // so this used to be an algorithmic necessity.
  //
  // Here, we recall that the element H is known and computed by algorithms
  // following Dynkin.
  ElementSemisimpleLieAlgebra<Polynomial<Rational> > fArbitrary;
  ElementSemisimpleLieAlgebra<Polynomial<Rational> > eArbitraryUnknown;
  ElementSemisimpleLieAlgebra<Polynomial<Rational> > eBracketFMinusHArbitraryUnknown;
  // The polynomial (actually, linear) system required to solve for eArbitraryUnknown.
  PolynomialSubstitution<Rational> systemToSolveArbitrary;
  // The matrix of the linear system given by systemToSolveArbitrary.
  Matrix<Rational> systemArbitraryMatrix;
  // The column-vector of the linear system given by systemToSolveArbitrary.
  Matrix<Rational> systemArbitraryColumnVector;

  List<CharacterSemisimpleLieAlgebraModule<Rational> > moduleDecompositionMinimalContainingRootSubalgebras;
  Vector<Rational> hCharacteristic;
  Vector<Rational> hElementCorrespondingToCharacteristic;
  Vectors<Rational> hCommutingRootSpaces;
  Vectors<Rational> rootsWithScalar2WithH;
  DynkinDiagramRootSubalgebra diagramM;
  int dynkinsEpsilon;
  bool flagDeallocated;
  void initialize();
  SlTwoSubalgebra() {
    this->flagDeallocated = false;
    this->initialize();
  }
  ~SlTwoSubalgebra() {
    this->flagDeallocated = true;
  }
  bool checkConsistency() const;

  SlTwoSubalgebras& getContainerSl2s() {
    if (this->container == nullptr) {
      global.fatal << "Attempt to "
      << "access the container list of "
      << "a non-initialized sl(2)-subalgebra. " << global.fatal;
    }
    return *this->container;
  }
  const WeylGroupData& getOwnerWeyl() const;
  SemisimpleLieAlgebra& getOwnerSemisimpleAlgebra() {
    if (this->owner == nullptr) {
      global.fatal << "Attempt to access "
      << "the ambient Lie algebra of a "
      << "non-initialized sl(2)-subalgebra. " << global.fatal;
    }
    return *this->owner;
  }
  std::string toStringTriple(FormatExpressions* format) const;
  std::string toStringTripleStandardRealization() const;
  std::string toStringTripleVerification(FormatExpressions* format) const;
  std::string toStringTripleUnknowns(FormatExpressions* format) const;
  std::string toStringTripleUnknownsPolynomialSystem(FormatExpressions* format = nullptr) const;
  template<typename Coefficient>
  std::string toStringPolynomialSystem(
    const PolynomialSubstitution<Coefficient>& system,
    FormatExpressions* format = nullptr
  ) const;
  std::string toStringTripleArbitrary(FormatExpressions* format) const;
  std::string toStringTripleArbitraryMatrix() const;
  std::string toString(FormatExpressions* format = nullptr) const;
  std::string toStringKostantSekiguchiTripleInternals(FormatExpressions* format) const;
  std::string toStringKostantSekiguchiTriple(FormatExpressions* format) const;
  std::string toStringKostantSekiguchiTripleStandardRealization() const;
  void getInvolvedPositiveGenerators(List<ChevalleyGenerator>& output);
  void getInvolvedNegativeGenerators(List<ChevalleyGenerator>& output);
  void toStringModuleDecompositionMinimalContainingRegularSAs(
    bool useLatex, bool useHtml, SlTwoSubalgebras& owner, std::string& output
  ) const;
  void computeModuleDecompositionsition(
    const Vectors<Rational>& positiveRootsContainingRegularSubalgebra,
    int dimensionContainingRegularSubalgebra,
    CharacterSemisimpleLieAlgebraModule<Rational>& outputHighestWeights,
    List<int>& outputModuleDimensions
  );
  Rational getDynkinIndex() const;
  void computeModuleDecompositionsitionAmbientLieAlgebra();
  bool attemptToComputeCentralizer();
  bool attemptExtendingHFtoHEFWithRespectToSubalgebra(
    Vectors<Rational>& rootsWithCharacteristic2,
    Selection& zeroCharacteristics,
    Vectors<Rational>& simpleBasisSubalgebras,
    Vector<Rational>& h,
    AlgebraicClosureRationals* inputAlgebraicClosure
  );
  bool attemptRealizingKostantSekiguchi();
  bool checkConsistencyParticipatingRoots(const Vector<Rational>& targetH);
  // Initializes the h,e,f computation with f arbitrarily chosen.
  // See the preceding comments on why f is chosen arbitrarily.
  void initializeUnknownTriples(const Vector<Rational>& targetH);
  void computePolynomialSystems();
  void adjoinKostantSekiguchiRelationsToPolynomialSystem(
    LinearMapSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >* cartanInvolutionPreservedByEMinusF
  );
  void computeLieBracketsUnknowns();
  void initializeHEFSystemFromFCoefficients(
    const Vector<Rational>& targetH,
    LinearMapSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >* cartanInvolutionPreservedByEMinusF
  );
  void initializeHEFSystemFromFCoefficientsPartTwo();
  // Whether the ambient Lie algebra has a Cartan involution that has been implemented.
  bool hasImplementedStandardCartanInvolution(
    LinearMapSemisimpleLieAlgebra<Polynomial<AlgebraicNumber> >* whichInvolution
  );
  void computeModuleDecompositionsitionOfMinimalContainingRegularSAs(SlTwoSubalgebras& owner);
  bool moduleDecompositionFitsInto(const SlTwoSubalgebra& other) const;
  static bool moduleDecompositionLeftFitsIntoRight(
    const CharacterSemisimpleLieAlgebraModule<Rational>& moduleDecompoLeft,
    const CharacterSemisimpleLieAlgebraModule<Rational>& moduleDecompoRight
  );
  void makeReportPrecomputations(
    int indexMinimalContainingRegularSA,
    RootSubalgebra& minimalContainingRegularSubalgebra
  );
  //the below is outdated, must be deleted as soon as equivalent code is written.
  void computeDynkinsEpsilon(WeylGroupData& weyl);
  void toHTML(std::string& filePath);
  bool operator==(const SlTwoSubalgebra& right) const;
  bool operator>(const SlTwoSubalgebra& right) const;
  unsigned int hashFunction() const;
  static inline unsigned int hashFunction(const SlTwoSubalgebra& input) {
    return input.hashFunction();
  }
};

class SlTwoSubalgebras : public HashedList<SlTwoSubalgebra> {
  friend class SemisimpleSubalgebras;
  SemisimpleLieAlgebra* owner;
public:
  static std::string descriptionHCharacteristic;
  static std::string descriptionModuleDecompositionOverSl2;
  static std::string descriptionHRealization;
  static std::string descriptionMinimalContainingRegularSubalgebras;

  List<List<int> > indicesSl2sContainedInRootSubalgebras;
  List<int> indicesSl2DecompositionFormulas;
  Vectors<Rational> badHCharacteristics;
  int indexZeroWeight;
  RootSubalgebras rootSubalgebras;
  ~SlTwoSubalgebras() {
  }
  SlTwoSubalgebras(): owner(nullptr) {
  }
  SlTwoSubalgebras(SemisimpleLieAlgebra& inputOwner): owner(&inputOwner) {
  }
  bool operator==(const SlTwoSubalgebras& other) const {
    if (this->owner == nullptr) {
      return other.owner == nullptr;
    }
    if (other.owner == nullptr) {
      return false;
    }
    return this->getOwner() == other.getOwner();
  }
  bool checkConsistency() const;
  void checkInitialization() const {
    if (this->owner == nullptr) {
      global.fatal << "<br>Object SlTwoSubalgebras "
      << "is not initialized, although it is supposed to be. " << global.fatal;
    }
  }
  WeylGroupData& getOwnerWeyl() const {
    return this->getOwner().weylGroup;
  }
  SemisimpleLieAlgebra& getOwner() const {
    this->checkInitialization();
    return *this->owner;
  }
  void computeModuleDecompositionsitionsOfAmbientLieAlgebra();
  void reset(SemisimpleLieAlgebra& inputOwners);
  bool containsSl2WithGivenH(Vector<Rational>& elementH, int* outputIndex);
  bool containsSl2WithGivenHCharacteristic(Vector<Rational>& hCharacteristic, int* outputIndex);
  void writeHTML(FormatExpressions* format = nullptr);
  std::string toStringSummary(FormatExpressions* format = nullptr);
  void toStringModuleDecompositionMinimalContainingRegularSAs(std::string& output, bool useLatex, bool useHtml);
  std::string toString(FormatExpressions* format = nullptr);
};

#endif