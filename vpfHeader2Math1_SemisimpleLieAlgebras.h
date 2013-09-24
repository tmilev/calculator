//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeader1_4_h_already_included
#define vpfHeader1_4_h_already_included

#include "vpfHeader2Math0_General.h"
static ProjectInformationInstance ProjectInfoVpfHeader1_4(__FILE__, "Header, math routines concerning semisimple Lie algebras. ");

class slTwoSubalgebra
{
public:
  List<int> highestWeights;
  List<int> multiplicitiesHighestWeights;
  List<int> weightSpaceDimensions;
  ElementSemisimpleLieAlgebra<Rational> theH, theE, theF;
  ElementSemisimpleLieAlgebra<Rational> bufferHbracketE, bufferHbracketF, bufferEbracketF;
  SemisimpleLieAlgebra* owneR;
  SltwoSubalgebras* container;
  Rational LengthHsquared;
  int indexInContainer;
  List<int> IndicesContainingRootSAs;
  List<int> IndicesMinimalContainingRootSA;
  List<List<int> > HighestWeightsDecompositionMinimalContainingRootSA;
  List<List<int> > MultiplicitiesDecompositionMinimalContainingRootSA;
  Vectors<Rational> preferredAmbientSimpleBasis;
  Vector<Rational> hCharacteristic;
  Vector<Rational> hElementCorrespondingToCharacteristic;
  Vectors<Rational> hCommutingRootSpaces;
  Vectors<Rational> RootsWithScalar2WithH;
  DynkinDiagramRootSubalgebra DiagramM;
  DynkinDiagramRootSubalgebra CentralizerDiagram;
  PolynomialSubstitution<Rational> theSystemToBeSolved;
  Matrix<Rational> theSystemMatrixForm;
  Matrix<Rational> theSystemColumnVector;
  bool DifferenceTwoHsimpleRootsIsARoot;
  int DynkinsEpsilon;
  bool flagDeallocated;
  slTwoSubalgebra(): owneR(0), container(0), indexInContainer(-1), flagDeallocated(false){}
  ~slTwoSubalgebra()
  { this->flagDeallocated=true;
  }
  bool CheckConsistency()const;

  SltwoSubalgebras& GetContainerSl2s()
  { if (this->container==0)
    { std::cout << "This is a programming error: attempting to access the container list of a non-initialized sl(2)-subalgebra. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    return *this->container;
  }
  WeylGroup& GetOwnerWeyl();
  SemisimpleLieAlgebra& GetOwnerSSAlgebra()
  { if (this->owneR==0)
    { std::cout << "This is a programming error: attempting to access the ambient Lie algebra of a non-initialized sl(2)-subalgebra. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    return *this->owneR;
  }
  std::string ToString(FormatExpressions* theFormat=0)const;
  void GetInvolvedPosGenerators(List<ChevalleyGenerator>& output);
  void GetInvolvedNegGenerators(List<ChevalleyGenerator>& output);
  void ElementToStringModuleDecomposition(bool useLatex, bool useHtml, std::string& output)const;
  void ElementToStringModuleDecompositionMinimalContainingRegularSAs
  (bool useLatex, bool useHtml, SltwoSubalgebras& owner, std::string& output)const;
  void ComputePrimalModuleDecomposition(Vectors<Rational>& positiveRootsContainingRegularSA, int dimensionContainingRegularSA, List<int>& outputHighestWeights, List<int>& outputMultiplicitiesHighestWeights, List<int>& outputWeightSpaceDimensions, GlobalVariables& theGlobalVariables);
  void ComputeModuleDecompositionAmbientLieAlgebra(GlobalVariables& theGlobalVariables);
  void ComputeModuleDecompositionOfMinimalContainingRegularSAs(SltwoSubalgebras& owner, int IndexInOwner, GlobalVariables& theGlobalVariables);
  bool ModuleDecompositionFitsInto(const slTwoSubalgebra& other)const;
  static bool ModuleDecompositionFitsInto
  (const List<int>& highestWeightsLeft, const List<int>& multiplicitiesHighestWeightsLeft,
   const List<int>& highestWeightsRight, const List<int>& multiplicitiesHighestWeightsRight);
  void MakeReportPrecomputations(GlobalVariables& theGlobalVariables, SltwoSubalgebras& container, int indexInContainer, int indexMinimalContainingRegularSA, rootSubalgebra& MinimalContainingRegularSubalgebra);
  //the below is outdated, must be deleted as soon as equivalent code is written.
  void ComputeDynkinsEpsilon(WeylGroup& theWeyl);
  void ElementToHtml(std::string& filePath);
  void ElementToHtmlCreateFormulaOutputReference
  (const std::string& formulaTex, std::stringstream& output, bool usePNG, bool useHtml, SltwoSubalgebras& container, std::string* physicalPath, std::string* htmlPathServer)const;
  bool operator==(const slTwoSubalgebra& right)const;
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
  List<int> MultiplicitiesFixedHweight;
  List<List<int> > IndicesSl2sContainedInRootSA;
  List<int> IndicesSl2decompositionFlas;
  Vectors<Rational> BadHCharacteristics;
  int IndexZeroWeight;
  rootSubalgebras theRootSAs;
  List<std::string> texFileNamesForPNG;
  List<std::string> texStringsEachFile;
  List<std::string> listSystemCommandsLatex;
  List<std::string> listSystemCommandsDVIPNG;
  bool operator==(const SltwoSubalgebras& other)const
  { if (this->owner==0)
      return other.owner==0;
    if (other.owner==0)
      return false;
    return this->GetOwner()==other.GetOwner();
  }
  void CheckForCorrectInitializationCrashIfNot()const
  { if (this->owner==0)
    { std::cout << "<br>This is a programming error. Object SltwoSubalgebras is not initialized, although it is supposed to be. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
  }
  WeylGroup& GetOwnerWeyl()const
  { return this->GetOwner().theWeyl;
  }
  SemisimpleLieAlgebra& GetOwner()const
  { this->CheckForCorrectInitializationCrashIfNot();
    return *this->owner;
  }
  void ComputeModuleDecompositionsOfAmbientLieAlgebra(GlobalVariables& theGlobalVariables);
  void reset(SemisimpleLieAlgebra& inputOwners);
  SltwoSubalgebras(): owner(0){}
  SltwoSubalgebras(SemisimpleLieAlgebra& inputOwner)
  : owner(&inputOwner)
  {}
  void ComputeModuleDecompositionsOfMinimalContainingRegularSAs(GlobalVariables& theGlobalVariables)
  { std::cout << "This is a programming error. This function used to work in an older version of the program, but, as the requirements have changed, now needs a rewrite. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
    /*for(int i=0; i<this->size; i++)
      (*this)[i].ComputeModuleDecompositionOfMinimalContainingRegularSAs(*this, i, theGlobalVariables);
      */
  }
  void ComputeDebugStringCurrent();
  bool ContainsSl2WithGivenH(Vector<Rational>& theH, int* outputIndex);
  bool ContainsSl2WithGivenHCharacteristic(Vector<Rational>& theHCharacteristic, int* outputIndex);
  void ElementToHtml
  (FormatExpressions* theFormat=0, GlobalVariables* theGlobalVariables=0)
   ;
  std::string ElementToStringNoGenerators(FormatExpressions* theFormat=0);
  void ElementToStringModuleDecompositionMinimalContainingRegularSAs(std::string& output, bool useLatex, bool useHtml);
  std::string ToString(FormatExpressions* theFormat=0);
};

class SemisimpleSubalgebras;
class CandidateSSSubalgebra;

class NilradicalCandidate
{
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

  List<ElementSemisimpleLieAlgebra<Rational> > theNonFKhwVectors;
  List<ElementSemisimpleLieAlgebra<Rational> > theNonFKHVectorsStronglyTwoSided;
  List<ElementSemisimpleLieAlgebra<Rational> > theNilradical;
  List<ElementSemisimpleLieAlgebra<Rational> > theNilradicalElementOpposites;
  Selection theNilradSubsel;
  List<ElementSemisimpleLieAlgebra<Rational> > theNilradicalSubset;
  List<ElementSemisimpleLieAlgebra<Rational> > theNonFKhwVectorsStrongRelativeToSubset;
  Vectors<Rational> theNilradicalSubsetWeights;
  Vectors<Rational> theNonFKhwVectorsStrongRelativeToSubsetWeights;

  NilradicalCandidate():owner(0),flagRestrictedCentralizerConditionHoldS(false), flagParabolicACextendsToParabolicAC(false), flagLinfiniteRelFound(false){}
  void reset();
  void CheckInitialization()const;
  bool IsStronglySingular(int moduleIndex);
  bool IsStronglySingularRelativeToSubset(int nonFKweightIndex);
  Vector<Rational> GetConeStrongIntersectionWeight()const;
  Vector<Rational> GetNilradicalLinearCombi()const;
  void GetModGeneratedByNonHWVandNilradElt
  (int indexInNilradSubset, List<ElementSemisimpleLieAlgebra<Rational> >& outputLeft,
   List<ElementSemisimpleLieAlgebra<Rational> >& outputRight, List<ElementSemisimpleLieAlgebra<Rational> >& outputBrackets)const
  ;
  bool TryFindingLInfiniteRels(GlobalVariables* theGlobalVariables);
  void ComputeParabolicACextendsToParabolicAC(GlobalVariables* theGlobalVariables);
//  bool IsLInfiniteRel(GlobalVariables* theGlobalVariables);
  bool IsCommutingSelectionNilradicalElements(Selection& inputNilradSel);
  void ProcessMe(GlobalVariables* theGlobalVariables);
  std::string ToString(FormatExpressions* theFormat=0)const;
  std::string ToStringTableElementWithWeights(const List<ElementSemisimpleLieAlgebra<Rational> >& theElts, const Vectors<Rational>& theWeights)const;
  void ComputeTheTwoCones(GlobalVariables* theGlobalVariables)
  ;
  void ComputeTheTwoConesRelativeToNilradicalSubset();
};

class CandidateSSSubalgebra
{
public:
  WeylGroup theWeylNonEmbeddeD;
  WeylGroup theWeylNonEmbeddeDdefaultScale;
  DynkinDiagramRootSubalgebra theCentralizerSubDiagram;
  DynkinType theCentralizerType;

  List<Vectors<Rational> > CartanSAsByComponent;
  Vectors<Rational> theHsScaledToActByTwo;
  Vectors<Rational> theHs;
  Matrix<Rational> BilinearFormSimplePrimal;
  Matrix<Rational> BilinearFormFundPrimal;
  List<ElementSemisimpleLieAlgebra<Rational> > thePosGens;
  List<ElementSemisimpleLieAlgebra<Rational> > theNegGens;
  List<ElementSemisimpleLieAlgebra<Rational> > theBasis;
  List<ElementSemisimpleLieAlgebra<Rational> > fullBasisByModules;
  List<int> fullBasisOwnerModules;
  List<ElementSemisimpleLieAlgebra<Polynomial<Rational> > > theUnknownPosGens;
  List<ElementSemisimpleLieAlgebra<Polynomial<Rational> > > theUnknownNegGens;
  List<ElementSemisimpleLieAlgebra<Polynomial<Rational> > > theUnknownCartanCentralizerBasis;

  List<List<int> > theHorbitIndices;
//  List<List<ElementWeylGroup> > theHWeylGroupElts;
//  Vector<Rational> aSolution;
  List<List<ChevalleyGenerator> > theInvolvedPosGenerators;
  List<List<ChevalleyGenerator> > theInvolvedNegGenerators;
  charSSAlgMod<Rational> theCharFundamentalCoordsRelativeToCartan;
  charSSAlgMod<Rational> theCharNonPrimalFundCoords;
  charSSAlgMod<Rational> thePrimalChaR;
  Vectors<Rational> PosRootsPerpendicularPrecedingWeights;
  Vectors<Rational> CartanOfCentralizer;
  List<Polynomial<Rational> > theSystemToSolve;
  List<Polynomial<Rational> > transformedSystem;
  SemisimpleSubalgebras* owner;
  int indexInOwner;
  int indexInOwnersOfNonEmbeddedMe;
  int indexMaxSSContainer;
  List<int> indicesDirectSummandSuperAlgebra;
  MemorySaving<FormatExpressions> charFormaT;
  bool flagSystemSolved;
  bool flagSystemProvedToHaveNoSolution;
  bool flagSystemGroebnerBasisFound;
  bool flagCentralizerIsWellChosen;
  int RecursionDepthCounterForNilradicalGeneration;
  int totalNumUnknownsNoCentralizer;
  int totalNumUnknownsWithCentralizer;

  List<NilradicalCandidate> FKNilradicalCandidates;
  int NumConeIntersections;
  int NumCasesNoLinfiniteRelationFound;
  int NumBadParabolics;
  int NumCentralizerConditionFailsConeConditionHolds;
  //The highest weight vectors are by definition cartan-centralizer-split
  List<ElementSemisimpleLieAlgebra<Rational> > HighestVectorsNonSorted;
  List<Vector<Rational> > HighestWeightsPrimalNonSorted;
  List<Vector<Rational> > HighestWeightsNONprimalNonSorted;

  List<List<ElementSemisimpleLieAlgebra<Rational> > > HighestVectors;
  HashedList<Vector<Rational> > HighestWeightsPrimal;

  List<List<List<ElementSemisimpleLieAlgebra<Rational> > > > Modules;
  List<List<List<ElementSemisimpleLieAlgebra<Rational> > > > ModulesSl2opposite;
  List<List<ElementSemisimpleLieAlgebra<Rational> > > ModulesIsotypicallyMerged;
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
  std::string nilradicalGenerationLog;
  Rational centralizerRank;

  bool flagDeallocated;
  CandidateSSSubalgebra();
  ~CandidateSSSubalgebra()
  { this->flagDeallocated=true;
  }
  int GetPrimalRank()const;

  void GetHsByType
  (List<List<Vectors<Rational> > >& outputHsByType, List<DynkinSimpleType>& outputTypeList)const
  ;
  bool HasConjugateHsTo(List<Vector<Rational> >& other)const ;
  bool IsDirectSummandOf(const CandidateSSSubalgebra& other, bool computeImmediateDirectSummandOnly);

  void GetGenericCartanCentralizerLinearCombination
  (int indexCartanCentralizerGen, ElementSemisimpleLieAlgebra<Polynomial<Rational> >& output)
  ;
  void GetGenericPosGenLinearCombination
  (int indexPosGens, ElementSemisimpleLieAlgebra<Polynomial<Rational> >& output)
;
  bool IsExtremeWeight(int moduleIndex, int indexInIsoComponent)const;
  void GetGenericNegGenLinearCombination
  (int indexNegGens, ElementSemisimpleLieAlgebra<Polynomial<Rational> >& output)
  ;
  bool AmRegularSA()const;
  bool CompareLeftGreaterThanRight(const Vector<Rational>& left, const Vector<Rational>& right);
  void GetGenericLinearCombination
  (int numVars, int varOffset, List<ChevalleyGenerator>& involvedGens,
   ElementSemisimpleLieAlgebra<Polynomial<Rational> >& output)
  ;
  void GetGenericLinearCombinationInvolvedPosGens
  (int theIndex, ElementSemisimpleLieAlgebra<Polynomial<Rational> >& output)
;
  void ComputeCentralizerIsWellChosen();
  void AdjustCentralizerAndRecompute(GlobalVariables* theGlobalVariables);
  void AddToSystem
  (const ElementSemisimpleLieAlgebra<Polynomial<Rational> >& elementThatMustVanish)
  ;
  void EnumerateAllNilradicals(GlobalVariables* theGlobalVariables)
;
  std::string ToStringNilradicalSelection(const List<int>& theSelection);
  void EnumerateNilradicalsRecursively
  (List<int>& theSelection, GlobalVariables* theGlobalVariables, std::stringstream* logStream=0)
  ;
  void ExtendNilradicalSelectionToMultFreeOverSSpartSubalgebra
  (HashedList<int, MathRoutines::IntUnsignIdentity>& inputOutput, GlobalVariables* theGlobalVariables,
   std::stringstream* logStream)
  ;
  bool IsPossibleNilradicalCarryOutSelectionImplications
  (List<int>& theSelection, GlobalVariables* theGlobalVariables, std::stringstream* logStream=0)
  ;
  void ExtendToModule
(List<ElementSemisimpleLieAlgebra<Rational> >& inputOutput, GlobalVariables* theGlobalVariables)
  ;
  Vector<Rational> GetPrimalWeightFirstGen(const ElementSemisimpleLieAlgebra<Rational>& input)const;
  void ComputeKsl2triples(GlobalVariables* theGlobalVariables);
  void ComputeKsl2triplesGetOppositeEltsInOppositeModule
  (const Vector<Rational>& theElementWeight, const List<ElementSemisimpleLieAlgebra<Rational> >& inputOppositeModule,
   List<ElementSemisimpleLieAlgebra<Rational> >& outputElts)
   ;
  void ComputeKsl2triplesGetOppositeEltsAll
  (const Vector<Rational>& theElementWeight, List<ElementSemisimpleLieAlgebra<Rational> >& outputElts)
   ;
  bool ComputeKsl2tripleSetUpAndSolveSystem
  (const ElementSemisimpleLieAlgebra<Rational>& theE, const List<ElementSemisimpleLieAlgebra<Rational> >& FisLinearCombiOf,
   ElementSemisimpleLieAlgebra<Rational>& outputF, GlobalVariables* theGlobalVariables)
   ;
  void ComputeCharsPrimalModules();
  void ComputePairingTable(GlobalVariables* theGlobalVariables);
  void ComputeSinglePair
(int leftIndex, int rightIndex, List<int>& output, GlobalVariables* theGlobalVariables)
;
  int GetNumModules()const;
  void ComputePairKweightElementAndModule
(const ElementSemisimpleLieAlgebra<Rational>& leftKweightElt, int rightIndex,
 List<int>& output, GlobalVariables* theGlobalVariables)
;
  bool IsWeightSystemSpaceIndex
(int theIndex, const Vector<Rational>& AmbientRootTestedForWeightSpace);
  void AddTypeIncomplete(const DynkinSimpleType& theNewType);
  void AddHincomplete
  (const Vector<Rational>& theH, const ElementWeylGroup& theWE, int indexOfOrbit)
  ;
  bool CheckInitialization()const;
  bool CheckModuleDimensions()const;

  SemisimpleLieAlgebra& GetAmbientSS()const;
  WeylGroup& GetAmbientWeyl()const;
  void ComputeCartanOfCentralizer(GlobalVariables* theGlobalVariables);
  void ComputePrimalModuleDecomposition(GlobalVariables* theGlobalVariables);
  void ComputePrimalModuleDecompositionHWsHWVsOnly(GlobalVariables* theGlobalVariables);
  void ComputePrimalModuleDecompositionHWVsOnly
  (GlobalVariables* theGlobalVariables,
   HashedList<Vector<Rational> >& inputHws)
   ;
  void ComputePrimalModuleDecompositionHighestWeightsOnly
  (GlobalVariables* theGlobalVariables, HashedList<Vector<Rational> >& outputHWsCoords)
  ;
  void ComputePrimalModuleDecompositionHWsHWVsOnlyLastPart(GlobalVariables* theGlobalVariables);
  void GetPrimalWeightProjectionFundCoords
  (const Vector<Rational>& inputAmbientWeight, Vector<Rational>& output)const
  ;
  bool CheckGensBracketToHs();
  void GetWeightProjectionFundCoords
  (const Vector<Rational>& inputAmbientweight, Vector<Rational>& output)
  ;
  bool ComputeSystem
(GlobalVariables* theGlobalVariables, bool AttemptToChooseCentalizer)
 ;
  bool ComputeSystemPart2
(GlobalVariables* theGlobalVariables, bool AttemptToChooseCentalizer)
 ;
  bool ComputeChar
(bool allowBadCharacter, GlobalVariables* theGlobalVariables)
  ;
  bool AttemptToSolveSytem
  (GlobalVariables* theGlobalVariables)
  ;
  bool isGoodForTheTop
  (WeylGroup& ownerWeyl, const Vector<Rational>& HneW)const
  ;
  Rational GetScalarSA(const Vector<Rational>& primalWeightLeft, const Vector<Rational>& primalWeightRight)const;
  std::string ToStringTypeAndHs(FormatExpressions* theFormat=0)const;
  std::string ToStringGenerators(FormatExpressions* theFormat=0)const;
  std::string ToString(FormatExpressions* theFormat=0)const;
  std::string ToStringSystem(FormatExpressions* theFormat=0)const;
  std::string ToStringCentralizer(FormatExpressions* theFormat=0)const;
  std::string ToStringCartanSA(FormatExpressions* theFormat=0)const;
  std::string ToStringPairingTable(FormatExpressions* theFormat=0)const;
  std::string ToStringPairingTableLaTeX(FormatExpressions* theFormat=0)const;
  std::string ToStringNilradicals(FormatExpressions* theFormat=0)const;
  std::string ToStringNilradicalsSummary(FormatExpressions* theFormat=0)const;
  std::string ToStringModuleDecompo(FormatExpressions* theFormat=0)const;
  std::string ToStringModuleDecompoLaTeX(FormatExpressions* theFormat=0)const;
  std::string ToStringDrawWeights(FormatExpressions* theFormat=0)const;
//  std::string ToStringDrawWeightsVersion2(FormatExpressions* theFormat=0)const;

  bool operator>(const CandidateSSSubalgebra& other)const ;

};

class SemisimpleSubalgebras
{
public:
  SemisimpleLieAlgebra* owneR;
  SltwoSubalgebras theSl2s;
  List<SemisimpleLieAlgebra> SimpleComponentsSubalgebras;
  HashedListReferences<SemisimpleLieAlgebra> theSubalgebrasNonEmbedded;
  List<SltwoSubalgebras> theSl2sOfSubalgebras;

  List<CandidateSSSubalgebra> theSubalgebraCandidates;
  int theRecursionCounter;
  bool flagAttemptToSolveSystems;
  bool flagComputePairingTable;
  bool flagComputeModuleDecomposition;
  bool flagComputeNilradicals;
  bool flagProduceLaTeXtables;
  double timeComputationStartInSeconds;
  double timeComputationEndInSeconds;
  int numAdditions;
  int numMultiplications;
  bool flagDeallocated;
  int GetNumPossibleSAs()const;
  void ScaleDynkinType(DynkinType& theType)const;
  std::string ToStringAlgebraLink(int ActualIndexSubalgebra, FormatExpressions* theFormat)const;
  int GetDisplayIndexFromActual(int ActualIndexSubalgebra)const;
  std::string GetPhysicalFileNameSubalgebra(int ActualIndexSubalgebra, FormatExpressions* theFormat)const;
  std::string GetDisplayFileNameSubalgebra(int ActualIndexSubalgebra, FormatExpressions* theFormat)const;
  std::string GetPhysicalFileNameFKFTNilradicals(int ActualIndexSubalgebra, FormatExpressions* theFormat)const;
  std::string GetDisplayFileNameFKFTNilradicals(int ActualIndexSubalgebra, FormatExpressions* theFormat)const;

  bool operator==(const SemisimpleSubalgebras& other)
  { if (this->owneR==other.owneR)
      return true;
    if (this->owneR==0 || other.owneR==0)
    { std::cout << "This is a programming error: comparing non-initialized Semisimple Lie Subalgebras. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    return *this->owneR==*other.owneR;
  }

  SemisimpleLieAlgebra& GetSSowner()const
  { if (this->owneR==0)
    { std::cout << "This is a programming error: attempted to access non-initialized "
      << " semisimple Lie subalgerbas. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    return *this->owneR;
  }
  void initHookUpPointers(SemisimpleLieAlgebra& inputOwner)
  { this->owneR=&inputOwner;
    this->theSl2s.owner=&inputOwner;
  }
  void reset();
  ~SemisimpleSubalgebras()
  { this->flagDeallocated=true;
  }
  SemisimpleSubalgebras(): flagDeallocated(false)
  { this->reset();
  }
  SemisimpleSubalgebras(SemisimpleLieAlgebra& inputOwner): flagDeallocated(false)
  { this->reset();
    this->initHookUpPointers(inputOwner);
  }
  bool CheckConsistency()const;
  std::string ToString(FormatExpressions* theFormat=0);
  std::string ToStringSSsumaryLaTeX(FormatExpressions* theFormat=0)const;
  std::string ToStringSSsumaryHTML(FormatExpressions* theFormat=0)const;

  void RegisterPossibleCandidate
  (CandidateSSSubalgebra& theCandidate, GlobalVariables* theGlobalVariables)
  ;
  void HookUpCentralizers
  ( GlobalVariables* theGlobalVariables=0)
  ;
  void FindAllEmbeddings
  (DynkinSimpleType& theType, SemisimpleLieAlgebra& theOwner, GlobalVariables* theGlobalVariables)
  ;
  void FindTheSSSubalgebras
  (SemisimpleLieAlgebra& newOwner, GlobalVariables* theGlobalVariables)
  ;

  void FindTheSSSubalgebrasPart2
  (GlobalVariables* theGlobalVariables)
  ;
  void ExtendCandidatesRecursive
  (const CandidateSSSubalgebra& baseCandidate, bool propagateRecursion,
   GlobalVariables* theGlobalVariables)
  ;
  void ExtendOneComponentOneTypeAllLengthsRecursive
  (const CandidateSSSubalgebra& baseCandidate, DynkinSimpleType& theType,
   bool propagateRecursion, GlobalVariables* theGlobalVariables)
;
  void ExtendOneComponentRecursive
  (const CandidateSSSubalgebra& baseCandidate, bool propagateRecursion,
   GlobalVariables* theGlobalVariables)
  ;
};


#endif

