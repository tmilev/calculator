//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeaderSemisimpleLieAlgebrasRootSubalgebras
#define vpfHeaderSemisimpleLieAlgebrasRootSubalgebras

#include "vpfHeader2Math1_SemisimpleLieAlgebras.h"

static ProjectInformationInstance ProjectInfoVpfSSlieAlgRootSAs(__FILE__, "Header, root and sl2 subalgebras of semisimple Lie algebras. ");

class coneRelation
{
public:
  Vectors<Rational> Alphas;
  Vectors<Rational> Betas;
  List<Rational> AlphaCoeffs;
  List<Rational> BetaCoeffs;
  List<List<int> > AlphaKComponents;
  List<List<int> > BetaKComponents;
  int IndexOwnerRootSubalgebra;
  bool GenerateAutomorphisms(coneRelation& right);
  DynkinDiagramRootSubalgebra theDiagram;
  DynkinDiagramRootSubalgebra theDiagramRelAndK;
  std::string DebugString;
  std::string stringConnectedComponents;
  void ReadFromFile(std::fstream& input, rootSubalgebras& owner);
  void WriteToFile(std::fstream& output);
  void ComputeTheDiagramAndDiagramRelAndK(rootSubalgebra& owner);
  void ComputeDiagramRelAndK(rootSubalgebra& owner);
  void FixRepeatingRoots(Vectors<Rational>& theRoots, List<Rational>& coeffs);
  void RelationOneSideToString(std::string& output, const std::string& letterType, List<Rational>& coeffs, List<List<int> >& kComponents, Vectors<Rational>& theRoots, bool useLatex, rootSubalgebra& owner);
  int ToString(std::string& output, rootSubalgebras& owners, bool useLatex, bool includeScalarsProductsEachSide, bool includeMixedScalarProducts);
  int RootsToScalarProductString(Vectors<Rational>& inputLeft, Vectors<Rational>& inputRight, const std::string& letterTypeLeft, const std::string& letterTypeRight, std::string& output, rootSubalgebra& owner);
  void ComputeConnectedComponents(Vectors<Rational>& input, rootSubalgebra& owner, List<List<int> >& output);
  void ComputeDebugString(rootSubalgebras& owner, bool includeScalarsProducts, bool includeMixedScalarProducts)
  { this->ToString(this->DebugString, owner, true, includeScalarsProducts, includeMixedScalarProducts);
  }
  void MakeLookCivilized(rootSubalgebra& owner);
  bool IsStrictlyWeaklyProhibiting(rootSubalgebra& owner, Vectors<Rational>& NilradicalRoots, rootSubalgebras& owners, int indexInOwner);
  void FixRightHandSide(rootSubalgebra& owner, Vectors<Rational>& NilradicalRoots);
  bool leftSortedBiggerThanOrEqualToRight(List<int>& left, List<int>& right);
  void ComputeKComponents(Vectors<Rational>& input, List<List<int> >& output, rootSubalgebra& owner);
  void RelationOneSideToStringCoordForm(std::string& output, List<Rational>& coeffs, Vectors<Rational>& theRoots, bool EpsilonForm);
  void GetSumAlphas(Vector<Rational>& output, int theDimension);
  bool CheckForBugs(rootSubalgebra& owner, Vectors<Rational>& NilradicalRoots);
  void SortRelation(rootSubalgebra& owner);
  bool operator==(const coneRelation& right)
  { return this->DebugString==right.DebugString;
  }
  unsigned int HashFunction() const
  { int tempI= ::MathRoutines::Minimum((int) this->DebugString.length(), ::SomeRandomPrimesSize);
    unsigned int result=0;
    for (int i=0; i<tempI; i++)
      result+= this->DebugString[i]*::SomeRandomPrimes[i];
    return result;
  }
  static inline unsigned int HashFunction(const coneRelation& input)
  { return input.HashFunction();
  }
  coneRelation()
  {this->IndexOwnerRootSubalgebra=-1;
  }
};

class coneRelations: public HashedList<coneRelation>
{
public:
  int NumAllowedLatexLines;
  bool flagIncludeSmallerRelations;
  bool flagIncludeCoordinateRepresentation;
  bool flagIncludeSubalgebraDataInDebugString;
  std::string DebugString;
  List<std::string> CoordinateReps;
  void GetLatexHeaderAndFooter(std::string& outputHeader, std::string& outputFooter);
  void ToString
  (std::string& output, rootSubalgebras& owners, bool useLatex);
  void ComputeDebugString(rootSubalgebras& owners);
  void WriteToFile(std::fstream& output);
  void ReadFromFile(std::fstream& input, rootSubalgebras& owner);
  void AddRelationNoRepetition(coneRelation& input, rootSubalgebras& owners);
  coneRelations()
  { this->NumAllowedLatexLines=40;
    this->flagIncludeSmallerRelations=true;
    this->flagIncludeCoordinateRepresentation=false;
    this->flagIncludeSubalgebraDataInDebugString=false;
  }
};

class rootSubalgebra
{
  friend std::ostream& operator << (std::ostream& output, rootSubalgebra& theSA)
  { output << theSA.ToString();
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

  charSSAlgMod<Rational> ModuleDecompoHighestWeights;
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
  rootSubalgebras* ownEr;
  HashedList<Vector<Rational> > bufferForModuleGeneration;
  Matrix<Rational> scalarProdMatrixPermuted, scalarProdMatrixOrdered;
  Matrix<Rational> scalarProdInvertedMatrixOrdered;

  List<Matrix<Rational> > potentialExtensionCartanSymmetrics;
  List<List<int> > potentialExtensionRootPermutations;
  List<DynkinType> potentialExtensionDynkinTypes;
  VectorSparse<Rational> moduleDecompoAmbientAlgebraDimensionsOnly;
//  int indexInOwners;
  rootSubalgebra();
  ~rootSubalgebra()
  { this->flagDeallocated=true;
  }
  WeylGroupData& GetAmbientWeyl()const;
  SemisimpleLieAlgebra& GetOwnerSSalg()const;
  //returns -1 if the weight/root  is not in g/k
  bool CheckInitialization()const;
  bool CheckConsistency()const;
  bool CheckScalarProdMatrixOrdered()const;
  Vector<Rational> GetFundamentalCoordsOverKss(const Vector<Rational>& inputGweightSimpleCoords)const;
  Vector<Rational> GetSimpleCoordsOverKss(const Vector<Rational>& inputGweightSimpleCoords)const;
  inline int GetNumModules()const
  { return this->HighestVectors.size;
  }
  int GetIndexKmoduleContainingRoot(const Vector<Rational>& input);
  void GetCoxeterPlane(Vector<double>& outputBasis1, Vector<double>& outputBasis2);
  void GetCoxeterElement(Matrix<Rational>& output);
  void ComputePotentialExtensions();
  void GetSsl2SubalgebrasAppendListNoRepetition(SltwoSubalgebras& output, int indexRootSAinContainer);
  bool IsEquivalentToByDiagramsAndDimensions(const rootSubalgebra& other)const;
  void ComputeOuterSAautosExtendingToAmbientAutosGenerators();
  bool IsGeneratingSingularVectors(int indexKmod, Vectors<Rational>& NilradicalRoots);
  bool rootIsInCentralizer(const Vector<Rational>& input);
  bool IsBKhighest(const Vector<Rational>& input);
  bool IsBKlowest(const Vector<Rational>& input);
  bool CompareLeftGreaterThanRight(const Vector<Rational>& weightLeft, const Vector<Rational>& weightRight);
  bool rootIsInNilradicalParabolicCentralizer(Selection& positiveSimpleRootsSel, Vector<Rational>& input);
  void ComputeEpsCoordsWRTk();
  bool AttemptTheTripleTrick(coneRelation& theRel, Vectors<Rational>& NilradicalRoots);
  bool AttemptTheTripleTrickWRTSubalgebra(coneRelation& theRel, Vectors<Rational>& highestWeightsAllowed, Vectors<Rational>& NilradicalRoots);
  void ExtractRelations(Matrix<Rational>& matA, Matrix<Rational>& matX, Vectors<Rational>& NilradicalRoots, rootSubalgebras& owner, int indexInOwner, Vectors<Rational>& Ksingular);
  bool GenerateIsomorphismsPreservingBorel(rootSubalgebra& right, SubgroupWeylGroupOLD* outputAutomorphisms);
  void GenerateAutomorphismsPreservingBorel(SubgroupWeylGroupOLD& outputAutomorphisms);
  void MakeGeneratingSingularVectors(coneRelation& theRelation, Vectors<Rational>& nilradicalRoots);
  bool attemptExtensionToIsomorphismNoCentralizer(Vectors<Rational>& Domain, Vectors<Rational>& Range, int RecursionDepth, SubgroupWeylGroupOLD* outputAutomorphisms, bool GenerateAllpossibleExtensions, bool* abortKmodule, Vectors<Rational>* additionalDomain, Vectors<Rational>* additionalRange);
  static bool attemptExtensionToIsomorphism
  (Vectors<Rational>& Domain, Vectors<Rational>& Range, SubgroupWeylGroupOLD* outputAutomorphisms,
   rootSubalgebras& inputOwner, bool* DomainAndRangeGenerateNonIsoSAs);
  bool CheckForSmallRelations(coneRelation& theRel, Vectors<Rational>& nilradicalRoots);
  int NumRootsInNilradical();
  void MakeSureAlphasDontSumToRoot(coneRelation& theRel, Vectors<Rational>& NilradicalRoots);
  bool IsARoot(const Vector<Rational>& input);
  bool IsARootOrZero(const Vector<Rational>& input);
  void KEnumerationsToLinComb();
  void DoKRootsEnumeration();
  void ComputeModuleDecompoAmbientAlgebraDimensionsOnly();
  void ComputeCentralizerFromKModulesAndSortKModules();
  void MatrixToRelation(coneRelation& output, Matrix<Rational>& matA, Matrix<Rational>& matX, int theDimension, Vectors<Rational>& NilradicalRoots);
  void DoKRootsEnumerationRecursively(int indexEnumeration);
  void MakeProgressReportPossibleNilradicalComputation(rootSubalgebras& owner);
  void MakeProgressReportGenAutos(int progress, int outOf, int found);
  bool IndexIsCompatibleWithPrevious(int startIndex, int RecursionDepth, List<List<List<int> > >& multTable, List<Selection>& impliedSelections, List<int>& oppositeKmods, rootSubalgebras& owner);
  bool IsAnIsomorphism(Vectors<Rational>& domain, Vectors<Rational>& range, SubgroupWeylGroupOLD* outputAutomorphisms, Vectors<Rational>* additionalDomain, Vectors<Rational>* additionalRange);
  bool ListHasNonSelectedIndexLowerThanGiven(int index, List<int>& tempList, Selection& tempSel);
  void GeneratePossibleNilradicalsRecursive(Controller& PauseMutex, List<List<List<int> > >& multTable, List<Selection>& impliedSelections, List<int>& oppositeKmods, rootSubalgebras& owner, int indexInOwner);
  void GeneratePossibleNilradicals(Controller& PauseMutex, List<Selection>& impliedSelections, int& parabolicsCounter, bool useParabolicsInNilradical, rootSubalgebras& owner, int indexInOwner);
  void GeneratePossibleNilradicalsInit(List<Selection>& impliedSelections, int& parabolicsCounter);
  void WriteToFileNilradicalGeneration(std::fstream& output);
  void ReadFromFileNilradicalGeneration(std::fstream& input, rootSubalgebras& owner);
  bool ConeConditionHolds(rootSubalgebras& owner, int indexInOwner, bool doExtractRelations);
  bool ConeConditionHolds(rootSubalgebras& owner, int indexInOwner, Vectors<Rational>& NilradicalRoots, Vectors<Rational>& Ksingular, bool doExtractRelations);
  void PossibleNilradicalComputation(Selection& selKmods, rootSubalgebras& owner, int indexInOwner);
  std::string ToString(FormatExpressions* theFormat=0);
  void ToHTML(int index, FormatExpressions* theFormat);
  std::string ToStringMultTable(bool useLaTeX, bool useHtml, rootSubalgebra& owner);
  bool RootsDefineASubalgebra(Vectors<Rational>& theRoots);
  void GenerateKmodMultTable(List<List<List<int> > >& output, List<int>& oppositeKmods);
  void KmodTimesKmod(int index1, int index2, List<int>& oppositeKmods, List<int>& output);
  void ComputeDynkinDiagramKandCentralizer();
  bool CheckRankInequality()const;
  bool ComputeEssentialsIfNew();
  void ComputeEssentialS();
  bool CheckForMaximalDominanceCartanSA();
  void ComputeRootsOfK();
  void ComputeKModules();
  void ComputeModuleDecompo();
  void ComputeHighestVectorsHighestWeights();
  void ComputeModulesFromHighestVectors();
  void ComputeModuleFromHighestVector(int moduleIndex);

  void ComputeHighestWeightInTheSameKMod(const Vector<Rational>& input, Vector<Rational>& outputHW);
  void ComputeExtremeWeightInTheSameKMod(const Vector<Rational>& input, Vector<Rational>& outputW, bool lookingForHighest);
  void ComputeLowestWeightInTheSameKMod(const Vector<Rational>& input, Vector<Rational>& outputLW);
  void GetLinearCombinationFromMaxRankRootsAndExtraRoot(bool DoEnumeration);
//  void commonCodeForGetLinearCombinationFromMaxRankRootsAndExtraRoot();
  void initForNilradicalGeneration();
  void initNoOwnerReset();
  void GetLinearCombinationFromMaxRankRootsAndExtraRootMethod2();
  bool LinCombToString(const Vector<Rational>& alphaRoot, int coeff, Vector<Rational>& linComb, std::string& output);
  bool LinCombToStringDistinguishedIndex(int distinguished, Vector<Rational>& alphaRoot, int coeff, Vector<Rational> & linComb, std::string& output);
  void WriteMultTableAndOppositeKmodsToFile(std::fstream& output, List<List<List<int> > >& inMultTable, List<int>& inOpposites);
  void ReadMultTableAndOppositeKmodsFromFile(std::fstream& input, List<List<List<int> > >& outMultTable, List<int>& outOpposites);
  bool operator>(const rootSubalgebra& other)const;
};

class rootSubalgebras
{
public:
  List<rootSubalgebra> theSubalgebras;
  coneRelations theBadRelations;
  coneRelations theGoodRelations;
  coneRelations theMinRels;
  List<List<int> > ActionsNormalizerCentralizerNilradical;
  ListReferences<SubgroupWeylGroupOLD> CentralizerOuterIsomorphisms;
  ListReferences<SubgroupWeylGroupOLD> CentralizerIsomorphisms;
  //Code used in nilradical generation:
  List<Selection> ImpiedSelectionsNilradical;
  List<List<List<int> > > storedNilradicals;
  HashedList<Rational> validScales;
  SemisimpleLieAlgebra* owner;
  int parabolicsCounterNilradicalGeneration;
  List<int> numNilradicalsBySA;
  int IndexCurrentSANilradicalsGeneration;
  int NumReductiveRootSAsToBeProcessedNilradicalsGeneration;
  List<int> CountersNilradicalsGeneration;
  List<int> NumConeConditionHoldsBySSpart;
  List<rootSubalgebra> theSubalgebrasOrder_Parabolic_PseudoParabolic_Neither;
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
  SemisimpleLieAlgebra& GetOwnerSSalgebra()const;
  WeylGroupData& GetOwnerWeyl()const;
  bool CheckInitialization()const;
  bool GrowDynkinType(const DynkinType& input, List<DynkinType>& output, List<List<int> >* outputPermutationSimpleRoots)const;
  void ComputeKmodMultTables();
  bool ApproveKmoduleSelectionWRTActionsNormalizerCentralizerNilradical(Selection& targetSel);
  bool ApproveSelAgainstOneGenerator(List<int>& generator, Selection& targetSel);
  void RaiseSelectionUntilApproval(Selection& targetSel);
  void ApplyOneGenerator(List<int>& generator, Selection& targetSel);
  void GenerateActionKintersectBIsos(rootSubalgebra& theRootSA);
  void GenerateKintersectBOuterIsos(rootSubalgebra& theRootSA);
  void ComputeActionNormalizerOfCentralizerIntersectNilradical(Selection& SelectedBasisRoots, rootSubalgebra& theRootSA);
  void ComputeNormalizerOfCentralizerIntersectNilradical(Selection& SelectedBasisRoots, rootSubalgebra& theRootSA);

  void ComputeAllReductiveRootSAsInit();
  void ComputeAllReductiveRootSubalgebrasUpToIsomorphism();
  void ComputeAllReductiveRootSubalgebrasUpToIsomorphismOLD(bool sort, bool computeEpsCoords);
  void ComputeParabolicPseudoParabolicNeitherOrder();
  bool IsANewSubalgebra(rootSubalgebra& input);
  int GetIndexSubalgebraIsomorphicTo(rootSubalgebra& input);
  int GetIndexUpToEquivalenceByDiagramsAndDimensions(const rootSubalgebra& theSA);
  int IndexSubalgebra(rootSubalgebra& input);
  void ComputeAllReductiveRootSubalgebrasContainingInputUpToIsomorphismOLD(List<rootSubalgebra>& bufferSAs, int RecursionDepth);
  void SortDescendingOrderBySSRank();
  void WriteToDefaultFileNilradicalGeneration();
  bool ReadFromDefaultFileNilradicalGeneration();
  void WriteToFileNilradicalGeneration(std::fstream& output);
  void ReadFromFileNilradicalGeneration(std::fstream& input);
  void ElementToStringRootSpaces(std::string& output, bool includeMatrixForm, Vectors<Rational>& input);
  void ElementToStringConeConditionNotSatisfying(std::string& output, bool includeMatrixForm);
  void ElementToStringCentralizerIsomorphisms(std::string& output, bool useLatex, bool useHtml, int fromIndex, int NumToProcess);
  void ToHTML(FormatExpressions* theFormat);
  std::string ToString(FormatExpressions* theFormat);
  std::string ToStringAlgebraLink(int index);
  std::string ToStringDynkinTableHTML(FormatExpressions* theFormat);
  std::string ToStringDynkinTableFormat2LaTeX(FormatExpressions* theFormat);
  void ComputeLProhibitingRelations();
  void ComputeAllRootSubalgebrasUpToIso(int StartingIndex, int NumToBeProcessed);
  void MakeProgressReportAutomorphisms(SubgroupWeylGroupOLD& theSubgroup, rootSubalgebra& theRootSA);
  void initOwnerMustBeNonZero();
  void initForNilradicalGeneration();
  rootSubalgebras();
};

class slTwoSubalgebra
{
public:
/////////////////////////////////////////////
  friend std::ostream& operator << (std::ostream& output, const slTwoSubalgebra& theSl2)
  { output << theSl2.ToString();
    return output;
  }
/////////////////////////////////////////////
  charSSAlgMod<Rational> moduleDecompositionAmbientSA;
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
  List<charSSAlgMod<Rational> > moduleDecompositionMinimalContainingRootSAs;
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
  void init();
  slTwoSubalgebra()
  { this->flagDeallocated=false;
    this->init();
  }
  ~slTwoSubalgebra()
  { this->flagDeallocated=true;
  }
  bool CheckConsistency()const;

  SltwoSubalgebras& GetContainerSl2s()
  { if (this->container==0)
      crash << "This is a programming error: attempting to access the container list of a non-initialized sl(2)-subalgebra. " << crash;
    return *this->container;
  }
  WeylGroupData& GetOwnerWeyl();
  SemisimpleLieAlgebra& GetOwnerSSAlgebra()
  { if (this->owner==0)
      crash << "This is a programming error: attempting to access the ambient Lie algebra of a non-initialized sl(2)-subalgebra. " << crash;
    return *this->owner;
  }
  std::string ToString(FormatExpressions* theFormat=0)const;
  void GetInvolvedPosGenerators(List<ChevalleyGenerator>& output);
  void GetInvolvedNegGenerators(List<ChevalleyGenerator>& output);
  void ElementToStringModuleDecompositionMinimalContainingRegularSAs
  (bool useLatex, bool useHtml, SltwoSubalgebras& owner, std::string& output)const;
  void ComputeModuleDecomposition
  (Vectors<Rational>& positiveRootsContainingRegularSA, int dimensionContainingRegularSA,
   charSSAlgMod<Rational>& outputHWs, List<int>& outputModuleDimensions)
   ;
  void ComputeModuleDecompositionAmbientLieAlgebra();
  bool AttemptToComputeCentralizer();
  bool AttemptExtendingHFtoHEFWRTSubalgebra
  (Vectors<Rational>& RootsWithCharacteristic2, Selection& theZeroCharacteristics, Vectors<Rational>& simpleBasisSA, Vector<Rational>& h,
   ElementSemisimpleLieAlgebra<Rational>& outputE, ElementSemisimpleLieAlgebra<Rational>& outputF, Matrix<Rational>& outputMatrixSystemToBeSolved,
   PolynomialSubstitution<Rational>& outputSystemToBeSolved, Matrix<Rational>& outputSystemColumnVector);
  void initHEFSystemFromECoeffs
  (Vectors<Rational>& rootsInPlay,
   int numberVariables, int halfNumberVariables, Vector<Rational>& targetH, Matrix<Rational>& inputFCoeffs,
   Matrix<Rational>& outputMatrixSystemToBeSolved, Matrix<Rational>& outputSystemColumnVector, PolynomialSubstitution<Rational>& outputSystemToBeSolved);

  void ComputeModuleDecompositionOfMinimalContainingRegularSAs
  (SltwoSubalgebras& owne);
  bool ModuleDecompositionFitsInto(const slTwoSubalgebra& other)const;
  static bool ModuleDecompositionLeftFitsIntoRight
  (const charSSAlgMod<Rational>& moduleDecompoLeft,
   const charSSAlgMod<Rational>& moduleDecompoRight);
  void MakeReportPrecomputations(int indexMinimalContainingRegularSA, rootSubalgebra& MinimalContainingRegularSubalgebra);
  //the below is outdated, must be deleted as soon as equivalent code is written.
  void ComputeDynkinsEpsilon(WeylGroupData& theWeyl);
  void ToHTML(std::string& filePath);
  bool operator==(const slTwoSubalgebra& right)const;
  bool operator>(const slTwoSubalgebra& right)const;
  unsigned int HashFunction() const
  { int tempI=MathRoutines::Minimum(SomeRandomPrimesSize, this->hCharacteristic.size);
    int result=0;
    for (int i=0; i<tempI; i++)
      result+= this->hCharacteristic[i].NumShort*SomeRandomPrimes[i];
    return result;
  }
  static inline unsigned int HashFunction(const slTwoSubalgebra& input)
  { return input.HashFunction();
  }
};

class SltwoSubalgebras : public HashedList<slTwoSubalgebra>
{
  friend class SemisimpleSubalgebras;
  SemisimpleLieAlgebra* owner;
public:
  List<List<int> > IndicesSl2sContainedInRootSA;
  List<int> IndicesSl2decompositionFlas;
  Vectors<Rational> BadHCharacteristics;
  int IndexZeroWeight;
  rootSubalgebras theRootSAs;
  bool flagDeallocated;
  ~SltwoSubalgebras()
  { this->flagDeallocated=true;
  }
  SltwoSubalgebras(): owner(0), flagDeallocated(false){}
  SltwoSubalgebras(SemisimpleLieAlgebra& inputOwner): owner(&inputOwner), flagDeallocated(false)
  {}
  bool operator==(const SltwoSubalgebras& other)const
  { if (this->owner==0)
      return other.owner==0;
    if (other.owner==0)
      return false;
    return this->GetOwner()==other.GetOwner();
  }
  bool CheckConsistency()const;
  void CheckForCorrectInitializationCrashIfNot()const
  { if (this->owner==0)
      crash << "<br>This is a programming error. Object SltwoSubalgebras is not initialized, although it is supposed to be. " << crash;
  }
  WeylGroupData& GetOwnerWeyl()const
  { return this->GetOwner().theWeyl;
  }
  SemisimpleLieAlgebra& GetOwner()const
  { this->CheckForCorrectInitializationCrashIfNot();
    return *this->owner;
  }
  void ComputeModuleDecompositionsOfAmbientLieAlgebra();
  void reset(SemisimpleLieAlgebra& inputOwners);
  void ComputeDebugStringCurrent();
  bool ContainsSl2WithGivenH(Vector<Rational>& theH, int* outputIndex);
  bool ContainsSl2WithGivenHCharacteristic(Vector<Rational>& theHCharacteristic, int* outputIndex);
  void ToHTML(FormatExpressions* theFormat=0);
  std::string ToStringSummary(FormatExpressions* theFormat=0);
  void ElementToStringModuleDecompositionMinimalContainingRegularSAs(std::string& output, bool useLatex, bool useHtml);
  std::string ToString(FormatExpressions* theFormat=0);
};

#endif
