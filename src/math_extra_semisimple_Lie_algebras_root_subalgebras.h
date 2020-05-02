// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeaderSemisimpleLieAlgebrasRootSubalgebras
#define vpfHeaderSemisimpleLieAlgebrasRootSubalgebras

#include "math_extra_semisimple_Lie_algebras.h"

class ConeRelation {
public:
  Vectors<Rational> Alphas;
  Vectors<Rational> Betas;
  List<Rational> AlphaCoeffs;
  List<Rational> BetaCoeffs;
  List<List<int> > AlphaKComponents;
  List<List<int> > BetaKComponents;
  int IndexOwnerRootSubalgebra;
  bool GenerateAutomorphisms(ConeRelation& right);
  DynkinDiagramRootSubalgebra theDiagram;
  DynkinDiagramRootSubalgebra theDiagramRelAndK;
  std::string DebugString;
  std::string stringConnectedComponents;
  void ComputeTheDiagramAndDiagramRelAndK(RootSubalgebra& owner);
  void ComputeDiagramRelAndK(RootSubalgebra& owner);
  void FixRepeatingRoots(Vectors<Rational>& theRoots, List<Rational>& coeffs);
  void RelationOneSideToString(
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
  int RootsToScalarProductString(
    Vectors<Rational>& inputLeft,
    Vectors<Rational>& inputRight,
    const std::string& letterTypeLeft,
    const std::string& letterTypeRight,
    std::string& output,
    RootSubalgebra& owner
  );
  void ComputeConnectedComponents(Vectors<Rational>& input, RootSubalgebra& owner, List<List<int> >& output);
  void ComputeDebugString(RootSubalgebras& owner, bool includeScalarsProducts, bool includeMixedScalarProducts) {
    this->toString(this->DebugString, owner, true, includeScalarsProducts, includeMixedScalarProducts);
  }
  void MakeLookCivilized(RootSubalgebra& owner);
  bool IsStrictlyWeaklyProhibiting(
    RootSubalgebra& owner, Vectors<Rational>& NilradicalRoots, RootSubalgebras& owners, int indexInOwner
  );
  void FixRightHandSide(RootSubalgebra& owner, Vectors<Rational>& NilradicalRoots);
  bool leftSortedBiggerThanOrEqualToRight(List<int>& left, List<int>& right);
  void ComputeKComponents(Vectors<Rational>& input, List<List<int> >& output, RootSubalgebra& owner);
  void RelationOneSideToStringCoordForm(
    std::string& output, List<Rational>& coeffs, Vectors<Rational>& theRoots, bool EpsilonForm
  );
  void GetSumAlphas(Vector<Rational>& output, int theDimension);
  bool CheckForBugs(RootSubalgebra& owner, Vectors<Rational>& NilradicalRoots);
  void SortRelation(RootSubalgebra& owner);
  bool operator==(const ConeRelation& right) {
    return this->DebugString == right.DebugString;
  }
  unsigned int hashFunction() const {
    int tempI = ::MathRoutines::Minimum(static_cast<int>(this->DebugString.length()), ::someRandomPrimesSize);
    unsigned int result = 0;
    for (int i = 0; i < tempI; i ++) {
      result += static_cast<unsigned>(
        this->DebugString[static_cast<unsigned>(i)]
      ) * ::someRandomPrimes[i];
    }
    return result;
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
  void GetLatexHeaderAndFooter(std::string& outputHeader, std::string& outputFooter);
  void toString(std::string& output, RootSubalgebras& owners, bool useLatex);
  void ComputeDebugString(RootSubalgebras& owners);
  void WriteToFile(std::fstream& output);
  void ReadFromFile(std::fstream& input, RootSubalgebras& owner);
  void AddRelationNoRepetition(ConeRelation& input, RootSubalgebras& owners);
  ConeRelations() {
    this->NumAllowedLatexLines = 40;
    this->flagIncludeSmallerRelations = true;
    this->flagIncludeCoordinateRepresentation = false;
    this->flagIncludeSubalgebraDataInDebugString = false;
  }
};

class RootSubalgebra {
  friend std::ostream& operator << (std::ostream& output, RootSubalgebra& theSA) {
    output << theSA.toString();
    return output;
  }
public:
  int numHeirsRejectedNotMaximallyDominant;
  int numHeirsRejectedSameModuleDecompo;
  int numHeirsRejectedBadAngleS;
  int numHeirsRejectedNotMaxWRTouterAuto;

  List<DynkinType> parabolicallyAllowedHeirs;

  int NumNilradicalsAllowed;
  int NumConeConditionFailures;
  int NumRelationsWithStronglyPerpendicularDecomposition;
  //int NumRelationsgreaterLengthThan2;
  int NumGmodKtableRowsAllowedLatex;
  int NumTotalSubalgebras;
  int indexInducingSubalgebra;
  bool flagFirstRoundCounting;
  bool flagComputeConeCondition;
  bool flagMakingProgressReport;
  bool flagDeallocated;
  List<List<ElementSemisimpleLieAlgebra<Rational> > > Modules;
  List<ElementSemisimpleLieAlgebra<Rational> > HighestVectors;
  List<Vectors<Rational> > WeightsModulesNONPrimalSimple;
  List<Vectors<Rational> > WeightsModulesNONPrimalFundamental;
  List<Vectors<Rational> > WeightsModulesPrimalSimple;
  List<Vector<Rational> > HighestWeightsNONPrimalFundamental;

  List<Vector<Rational> > HighestWeightsPrimalSimple;
  List<Vector<Rational> > LowestWeightsPrimalSimple;

//  HashedList<Vector<Rational> > HighestWeightsPrimal;

  CharacterSemisimpleLieAlgebraModule<Rational> ModuleDecompoHighestWeights;
  List<int> indicesSubalgebrasContainingK;
  List<List<List<int> > > theMultTable;
  List<int> theOppositeKmods;
  DynkinDiagramRootSubalgebra theDynkinDiagram;
  DynkinDiagramRootSubalgebra theCentralizerDiagram;
  DynkinType theDynkinType;
  DynkinType theCentralizerDynkinType;
  List<List<int> > coneRelationsBuffer;
  List<int> coneRelationsNumSameTypeComponentsTaken;
  List<DynkinDiagramRootSubalgebra> relationsDiagrams;
  FinitelyGeneratedMatrixMonoid<Rational> outerSAautosExtendingToAmbientAutosGenerators;
  FinitelyGeneratedMatrixMonoid<Rational> outerSAautos;
  Vectors<Rational> genK;
  Vectors<Rational> SimpleBasisK;
  Vectors<Rational> SimpleBasisKScaledToActByTwo;
  Vectors<Rational> SimpleBasisKinOrderOfGeneration;
  Vectors<Rational> PositiveRootsK;
  HashedList<Vector<Rational> > AllRootsK;
  Selection NilradicalKmods;
  Selection CentralizerKmods;
  Vectors<Rational> HighestRootsK;
  Vectors<Rational> TestedRootsAlpha;
  Vectors<Rational> CentralizerRoots;
  Vectors<Rational> SimpleBasisCentralizerRoots;
  Vectors<Rational> SimpleBasisKEpsCoords;
  Vectors<Rational> SimpleBasisgEpsCoords;
  List<Vectors<Rational> > kModulesKepsCoords;
  List<Vectors<Rational> > kModulesgEpsCoords;
  List<Vectors<Rational> > PosRootsKConnectedComponents;
  List<Selection> theKEnumerations;
  List<int> theKComponentRanks;
  RootSubalgebras* ownEr;
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
  WeylGroupData& GetAmbientWeyl() const;
  WeylGroupAutomorphisms& GetAmbientWeylAutomorphisms() const;
  SemisimpleLieAlgebra& getOwnerLieAlgebra() const;
  //returns - 1 if the weight/root  is not in g/k
  bool checkInitialization() const;
  bool checkConsistency() const;
  bool CheckScalarProdMatrixOrdered() const;
  Vector<Rational> GetFundamentalCoordsOverKss(const Vector<Rational>& inputGweightSimpleCoords) const;
  Vector<Rational> GetSimpleCoordsOverKss(const Vector<Rational>& inputGweightSimpleCoords) const;
  inline int GetNumModules() const {
    return this->HighestVectors.size;
  }
  int GetIndexKmoduleContainingRoot(const Vector<Rational>& input);
  void GetCoxeterPlane(Vector<double>& outputBasis1, Vector<double>& outputBasis2);
  void GetCoxeterElement(Matrix<Rational>& output);
  void ComputePotentialExtensions();
  void GetSsl2SubalgebrasAppendListNoRepetition(SltwoSubalgebras& output, int indexRootSAinContainer);
  bool IsEquivalentToByDiagramsAndDimensions(const RootSubalgebra& other) const;
  void ComputeOuterSAAutosExtendingToAmbientAutosGenerators();
  bool IsGeneratingSingularVectors(int indexKmod, Vectors<Rational>& NilradicalRoots);
  bool rootIsInCentralizer(const Vector<Rational>& input);
  bool IsBKhighest(const Vector<Rational>& input);
  bool IsBKlowest(const Vector<Rational>& input);
  bool compareLeftGreaterThanRight(const Vector<Rational>& weightLeft, const Vector<Rational>& weightRight);
  bool rootIsInNilradicalParabolicCentralizer(Selection& positiveSimpleRootsSel, Vector<Rational>& input);
  void ComputeEpsCoordsWRTk();
  bool AttemptTheTripleTrick(ConeRelation& theRel, Vectors<Rational>& NilradicalRoots);
  bool AttemptTheTripleTrickWRTSubalgebra(
    ConeRelation& theRel, Vectors<Rational>& highestWeightsAllowed, Vectors<Rational>& NilradicalRoots
  );
  void ExtractRelations(
    Matrix<Rational>& matA,
    Matrix<Rational>& matX,
    Vectors<Rational>& NilradicalRoots,
    RootSubalgebras& owner,
    int indexInOwner,
    Vectors<Rational>& Ksingular
  );
  bool GenerateIsomorphismsPreservingBorel(
    RootSubalgebra& right, SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms* outputAutomorphisms
  );
  void GenerateAutomorphismsPreservingBorel(
    SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms& outputAutomorphisms
  );
  void MakeGeneratingSingularVectors(ConeRelation& theRelation, Vectors<Rational>& nilradicalRoots);
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
    Vectors<Rational>& Domain,
    Vectors<Rational>& Range,
    SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms* outputAutomorphisms,
    RootSubalgebras& inputOwner,
    bool* DomainAndRangeGenerateNonIsoSAs
  );
  bool CheckForSmallRelations(ConeRelation& theRel, Vectors<Rational>& nilradicalRoots);
  int NumRootsInNilradical();
  void MakeSureAlphasDontSumToRoot(ConeRelation& theRel, Vectors<Rational>& NilradicalRoots);
  bool IsARoot(const Vector<Rational>& input);
  bool IsARootOrZero(const Vector<Rational>& input);
  void KEnumerationsToLinComb();
  void DoKRootsEnumeration();
  void ComputeModuleDecompoAmbientAlgebraDimensionsOnly();
  void ComputeCentralizerFromKModulesAndSortKModules();
  void MatrixToRelation(
    ConeRelation& output, Matrix<Rational>& matA, Matrix<Rational>& matX, int theDimension, Vectors<Rational>& NilradicalRoots
  );
  void DoKRootsEnumerationRecursively(int indexEnumeration);
  void MakeProgressReportPossibleNilradicalComputation(RootSubalgebras& owner);
  void MakeProgressReportGenAutos(int progress, int outOf, int found);
  bool IndexIsCompatibleWithPrevious(
    int startIndex,
    int RecursionDepth,
    List<List<List<int> > >& multTable,
    List<Selection>& impliedSelections,
    List<int>& oppositeKmods,
    RootSubalgebras& owner
  );
  bool IsAnIsomorphism(
    Vectors<Rational>& domain,
    Vectors<Rational>& range,
    SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms* outputAutomorphisms,
    Vectors<Rational>* additionalDomain,
    Vectors<Rational>* additionalRange
  );
  bool ListHasNonSelectedIndexLowerThanGiven(int index, List<int>& tempList, Selection& tempSel);
  void GeneratePossibleNilradicalsRecursive(
    PauseThread& PauseMutex,
    List<List<List<int> > >& multTable,
    List<Selection>& impliedSelections,
    List<int>& oppositeKmods,
    RootSubalgebras& owner,
    int indexInOwner
  );
  void GeneratePossibleNilradicals(
    PauseThread& PauseMutex,
    List<Selection>& impliedSelections,
    int& parabolicsCounter,
    bool useParabolicsInNilradical,
    RootSubalgebras& owner,
    int indexInOwner
  );
  void GeneratePossibleNilradicalsInit(List<Selection>& impliedSelections, int& parabolicsCounter);
  bool ConeConditionHolds(RootSubalgebras& owner, int indexInOwner, bool doExtractRelations);
  bool ConeConditionHolds(
    RootSubalgebras& owner,
    int indexInOwner,
    Vectors<Rational>& NilradicalRoots,
    Vectors<Rational>& Ksingular,
    bool doExtractRelations
  );
  void PossibleNilradicalComputation(Selection& selKmods, RootSubalgebras& owner, int indexInOwner);
  std::string toString(FormatExpressions* theFormat = nullptr);
  void toHTML(int index, FormatExpressions* theFormat);
  std::string ToStringMultTable(bool useLaTeX, bool useHtml, RootSubalgebra& owner);
  bool RootsDefineASubalgebra(Vectors<Rational>& theRoots);
  void GenerateKmodMultTable(List<List<List<int> > >& output, List<int>& oppositeKmods);
  void KmodTimesKmod(int index1, int index2, List<int>& oppositeKmods, List<int>& output);
  void ComputeDynkinDiagramKandCentralizer();
  bool CheckRankInequality() const;
  bool ComputeEssentialsIfNew();
  void ComputeEssentialS();
  bool CheckForMaximalDominancecartanSubalgebra();
  void ComputeRootsOfK();
  void ComputeKModules();
  void ComputeModuleDecompo();
  void ComputeHighestVectorsHighestWeights();
  void ComputeModulesFromHighestVectors();
  void ComputeModuleFromHighestVector(int moduleIndex);

  void ComputeHighestWeightInTheSameKMod(const Vector<Rational>& input, Vector<Rational>& outputHW);
  void ComputeExtremeWeightInTheSameKMod(const Vector<Rational>& input, Vector<Rational>& outputW, bool lookingForHighest);
  void ComputeLowestWeightInTheSameKMod(const Vector<Rational>& input, Vector<Rational>& outputLW);
  void getLinearCombinationFromMaxRankRootsAndExtraRoot(bool DoEnumeration);
//  void commonCodeForgetLinearCombinationFromMaxRankRootsAndExtraRoot();
  void initForNilradicalGeneration();
  void initNoOwnerReset();
  void getLinearCombinationFromMaxRankRootsAndExtraRootMethod2();
  bool LinCombToString(const Vector<Rational>& alphaRoot, int coeff, Vector<Rational>& linComb, std::string& output);
  bool LinCombToStringDistinguishedIndex(
    int distinguished, Vector<Rational>& alphaRoot, int coeff, Vector<Rational>& linComb, std::string& output
  );
  void WriteMultTableAndOppositeKmodsToFile(
    std::fstream& output, List<List<List<int> > >& inMultTable, List<int>& inOpposites
  );
  void ReadMultTableAndOppositeKmodsFromFile(
    std::fstream& input, List<List<List<int> > >& outMultTable, List<int>& outOpposites
  );
  bool operator>(const RootSubalgebra& other) const;
};

class RootSubalgebras {
public:
  List<RootSubalgebra> theSubalgebras;
  ConeRelations theBadRelations;
  ConeRelations theGoodRelations;
  ConeRelations theMinRels;
  List<List<int> > ActionsNormalizerCentralizerNilradical;
  ListReferences<SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms> CentralizerOuterIsomorphisms;
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
  List<int> NumConeConditionHoldsBySSpart;
  List<RootSubalgebra> theSubalgebrasOrder_Parabolic_PseudoParabolic_Neither;
  int RecursionDepthNilradicalsGeneration;
  int NumSubalgebrasProcessed;
  int NumConeConditionFailures;
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
  WeylGroupData& GetOwnerWeyl() const;
  bool checkInitialization() const;
  bool GrowDynkinType(const DynkinType& input, List<DynkinType>& output, List<List<int> >* outputPermutationSimpleRoots) const;
  void ComputeKmodMultTables();
  bool ApproveKmoduleSelectionWRTActionsNormalizerCentralizerNilradical(Selection& targetSel);
  bool ApproveSelAgainstOneGenerator(List<int>& generator, Selection& targetSel);
  void RaiseSelectionUntilApproval(Selection& targetSel);
  void ApplyOneGenerator(List<int>& generator, Selection& targetSel);
  void GenerateActionKintersectBIsos(RootSubalgebra& theRootSA);
  void GenerateKintersectBOuterIsos(RootSubalgebra& theRootSA);
  void ComputeActionNormalizerOfCentralizerIntersectNilradical(Selection& SelectedBasisRoots, RootSubalgebra& theRootSA);
  void ComputeNormalizerOfCentralizerIntersectNilradical(Selection& SelectedBasisRoots, RootSubalgebra& theRootSA);

  void ComputeAllReductiveRootSAsInit();
  void ComputeAllReductiveRootSubalgebrasUpToIsomorphism();
  void ComputeAllReductiveRootSubalgebrasUpToIsomorphismOLD(bool sort, bool computeEpsCoords);
  void ComputeParabolicPseudoParabolicNeitherOrder();
  bool IsANewSubalgebra(RootSubalgebra& input);
  int GetIndexSubalgebraIsomorphicTo(RootSubalgebra& input);
  int GetIndexUpToEquivalenceByDiagramsAndDimensions(const RootSubalgebra& theSA);
  int IndexSubalgebra(RootSubalgebra& input);
  void ComputeAllReductiveRootSubalgebrasContainingInputUpToIsomorphismOLD(
    List<RootSubalgebra>& bufferSAs, int RecursionDepth
  );
  void SortDescendingOrderBySSRank();
  void ElementToStringRootSpaces(std::string& output, bool includeMatrixForm, Vectors<Rational>& input);
  void ElementToStringConeConditionNotSatisfying(std::string& output, bool includeMatrixForm);
  void ElementToStringCentralizerIsomorphisms(std::string& output, bool useLatex, bool useHtml, int fromIndex, int NumToProcess);
  void toHTML(FormatExpressions* theFormat);
  std::string toString(FormatExpressions* theFormat);
  std::string ToStringAlgebraLink(int index);
  std::string ToStringDynkinTableHTML(FormatExpressions* theFormat);
  std::string ToStringDynkinTableFormat2LaTeX(FormatExpressions* theFormat);
  void ComputeLProhibitingRelations();
  void ComputeAllRootSubalgebrasUpToIso(int StartingIndex, int NumToBeProcessed);
  void MakeProgressReportAutomorphisms(
    SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms& theSubgroup, RootSubalgebra& theRootSA
  );
  void initOwnerMustBeNonZero();
  void initForNilradicalGeneration();
  RootSubalgebras();
};

class SlTwoSubalgebra {
public:
/////////////////////////////////////////////
  friend std::ostream& operator << (std::ostream& output, const SlTwoSubalgebra& theSl2) {
    output << theSl2.toString();
    return output;
  }
/////////////////////////////////////////////
  CharacterSemisimpleLieAlgebraModule<Rational> moduleDecompositionAmbientSA;
  List<int> moduleDimensions;
  ElementSemisimpleLieAlgebra<Rational> theH, theE, theF;
  ElementSemisimpleLieAlgebra<Rational> bufferHbracketE, bufferHbracketF, bufferEbracketF;
  SemisimpleLieAlgebra* owner;
  SltwoSubalgebras* container;
  Rational LengthHsquared;
  int indexInContainer;
  int dimensionCentralizer;
  DynkinType CentralizerTypeIfKnown;
  Rational dimCentralizerToralPart;
  bool flagCentralizerTypeComputed;
  bool flagCentralizerIsRegular;
  List<int> IndicesContainingRootSAs;
  List<int> IndicesMinimalContainingRootSAs;
  List<CharacterSemisimpleLieAlgebraModule<Rational> > moduleDecompositionMinimalContainingRootSAs;
  Vectors<Rational> preferredAmbientSimpleBasis;
  Vector<Rational> hCharacteristic;
  Vector<Rational> hElementCorrespondingToCharacteristic;
  Vectors<Rational> hCommutingRootSpaces;
  Vectors<Rational> RootsWithScalar2WithH;
  DynkinDiagramRootSubalgebra DiagramM;
  //  DynkinDiagramRootSubalgebra CentralizerDiagram;
  PolynomialSubstitution<Rational> theSystemToBeSolved;
  Matrix<Rational> theSystemMatrixForm;
  Matrix<Rational> theSystemColumnVector;
  bool DifferenceTwoHsimpleRootsIsARoot;
  int DynkinsEpsilon;
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

  SltwoSubalgebras& GetContainerSl2s() {
    if (this->container == nullptr) {
      global.fatal << "Attempt to "
      << "access the container list of "
      << "a non-initialized sl(2)-subalgebra. " << global.fatal;
    }
    return *this->container;
  }
  const WeylGroupData& GetOwnerWeyl() const;
  SemisimpleLieAlgebra& GetOwnerSSAlgebra() {
    if (this->owner == nullptr) {
      global.fatal << "Attempt to access "
      << "the ambient Lie algebra of a "
      << "non-initialized sl(2)-subalgebra. " << global.fatal;
    }
    return *this->owner;
  }
  std::string toString(FormatExpressions* theFormat = nullptr) const;
  void GetInvolvedPosGenerators(List<ChevalleyGenerator>& output);
  void GetInvolvedNegGenerators(List<ChevalleyGenerator>& output);
  void ElementToStringModuleDecompositionMinimalContainingRegularSAs(
    bool useLatex, bool useHtml, SltwoSubalgebras& owner, std::string& output
  ) const;
  void ComputeModuleDecomposition(
    const Vectors<Rational>& positiveRootsContainingRegularSA,
    int dimensionContainingRegularSA,
    CharacterSemisimpleLieAlgebraModule<Rational>& outputHWs,
    List<int>& outputModuleDimensions
  );
  Rational GetDynkinIndex() const;
  void ComputeModuleDecompositionAmbientLieAlgebra();
  bool AttemptToComputeCentralizer();
  bool AttemptExtendingHFtoHEFWRTSubalgebra(
    Vectors<Rational>& RootsWithCharacteristic2,
    Selection& theZeroCharacteristics,
    Vectors<Rational>& simpleBasisSA,
    Vector<Rational>& h,
    ElementSemisimpleLieAlgebra<Rational>& outputE,
    ElementSemisimpleLieAlgebra<Rational>& outputF,
    Matrix<Rational>& outputMatrixSystemToBeSolved,
    PolynomialSubstitution<Rational>& outputSystemToBeSolved,
    Matrix<Rational>& outputSystemColumnVector
  );
  void initHEFSystemFromECoeffs(
    Vectors<Rational>& rootsInPlay,
    int numberVariables,
    int halfNumberVariables,
    Vector<Rational>& targetH,
    Matrix<Rational>& inputFCoeffs,
    Matrix<Rational>& outputMatrixSystemToBeSolved,
    Matrix<Rational>& outputSystemColumnVector,
    PolynomialSubstitution<Rational>& outputSystemToBeSolved
  );
  void ComputeModuleDecompositionOfMinimalContainingRegularSAs(SltwoSubalgebras& owner);
  bool ModuleDecompositionFitsInto(const SlTwoSubalgebra& other) const;
  static bool ModuleDecompositionLeftFitsIntoRight(
    const CharacterSemisimpleLieAlgebraModule<Rational>& moduleDecompoLeft, const CharacterSemisimpleLieAlgebraModule<Rational>& moduleDecompoRight
  );
  void MakeReportPrecomputations(int indexMinimalContainingRegularSA, RootSubalgebra& MinimalContainingRegularSubalgebra);
  //the below is outdated, must be deleted as soon as equivalent code is written.
  void ComputeDynkinsEpsilon(WeylGroupData& theWeyl);
  void toHTML(std::string& filePath);
  bool operator==(const SlTwoSubalgebra& right) const;
  bool operator>(const SlTwoSubalgebra& right) const;
  unsigned int hashFunction() const {
    int tempI = MathRoutines::Minimum(someRandomPrimesSize, this->hCharacteristic.size);
    unsigned int result = 0;
    for (int i = 0; i < tempI; i ++) {
      result += static_cast<unsigned>(this->hCharacteristic[i].numeratorShort) * someRandomPrimes[i];
    }
    return result;
  }
  static inline unsigned int hashFunction(const SlTwoSubalgebra& input) {
    return input.hashFunction();
  }
};

class SltwoSubalgebras : public HashedList<SlTwoSubalgebra> {
  friend class SemisimpleSubalgebras;
  SemisimpleLieAlgebra* owner;
public:
  static std::string descriptionHCharacteristic;
  static std::string descriptionModuleDecompositionOverSl2;
  static std::string descriptionHRealization;
  static std::string descriptionMinimalContainingRegularSubalgebras;

  List<List<int> > IndicesSl2sContainedInRootSA;
  List<int> IndicesSl2decompositionFlas;
  Vectors<Rational> BadHCharacteristics;
  int IndexZeroWeight;
  RootSubalgebras theRootSAs;
  // bool flagDeallocated;
  ~SltwoSubalgebras() {
    // this->flagDeallocated = true;
  }
  SltwoSubalgebras(): owner(nullptr) /*, flagDeallocated(false)*/ {
  }
  SltwoSubalgebras(SemisimpleLieAlgebra& inputOwner): owner(&inputOwner)/*, flagDeallocated(false)*/ {
   
  }
  bool operator==(const SltwoSubalgebras& other) const {
    if (this->owner == nullptr) {
      return other.owner == nullptr;
    }
    if (other.owner == nullptr) {
      return false;
    }
    return this->GetOwner() == other.GetOwner();
  }
  bool checkConsistency() const;
  void CheckForCorrectInitializationCrashIfNot() const {
    if (this->owner == nullptr) {
      global.fatal << "<br>This is a programming error. Object SltwoSubalgebras "
      << "is not initialized, although it is supposed to be. " << global.fatal;
    }
  }
  WeylGroupData& GetOwnerWeyl() const {
    return this->GetOwner().theWeyl;
  }
  SemisimpleLieAlgebra& GetOwner() const {
    this->CheckForCorrectInitializationCrashIfNot();
    return *this->owner;
  }
  void ComputeModuleDecompositionsOfAmbientLieAlgebra();
  void reset(SemisimpleLieAlgebra& inputOwners);
  void ComputeDebugStringCurrent();
  bool ContainsSl2WithGivenH(Vector<Rational>& theH, int* outputIndex);
  bool ContainsSl2WithGivenHCharacteristic(Vector<Rational>& theHCharacteristic, int* outputIndex);
  void toHTML(FormatExpressions* theFormat = nullptr);
  std::string toStringSummary(FormatExpressions* theFormat = nullptr);
  void ElementToStringModuleDecompositionMinimalContainingRegularSAs(std::string& output, bool useLatex, bool useHtml);
  std::string toString(FormatExpressions* theFormat = nullptr);
};

#endif
