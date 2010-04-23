#ifdef polyhedra_h_already_included
//to be merged in polyhedra.h
#ifndef rootFKFT_already_included
#define rootFKFT_already_included
class minimalRelationsProverStates;


class minimalRelationsProverState
{
public:
  std::string DebugString;
  static int ProblemCounter;
  bool flagAnErrorHasOccurredTimeToPanic;
  void ElementToString
    ( std::string& output, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables,
      bool displayEpsilons);
  void ComputeDebugString(WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
  {	this->ElementToString(this->DebugString,theWeyl, TheGlobalVariables,true);
  };
  bool flagNeedsAdditionOfPositiveKroots;
  root currentSeparatingNormalEpsilonForm;
  roots NilradicalRoots;
  roots nonNilradicalRoots;
  roots PositiveKroots;
  roots nonPositiveKRoots;
  roots nonKRoots;
  roots BKSingularGmodLRoots;
  roots nonBKSingularGmodLRoots;
  roots nonAlphas;
  roots nonBetas;
  roots nonLRoots;
  roots nonLNonSingularRoots;
  roots nonLNonSingularRootsInNeedOfPosKroots;
  roots chosenKroots;
//  roots nonBKsingularRoots;
	//roots UndecidedRoots;
  coneRelation PartialRelation;
  MatrixLargeRational theScalarProducts;
  bool StateIsPossible;
  bool StateIsComplete;
  //bool StateIsDubious;
  ListBasicObjects<int> childStates;
  int activeChild;
  minimalRelationsProverStates* owner;
  void SortAlphasAndBetas(GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl);
  void GetNumberScalarProductsData
		(	root& input, roots& theRoots, bool& isLong, int& NumLongValue, int& NumMixedValue,
			int& NumShortValue, int& NumMinusLongValue, int& NumMinusMixedValue,
			int& NumMinusShortValue, GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl);
	bool Root1IsGreaterThanRoot2
		(	int index1, int index2,roots& setWeBelongTo, roots& setOtherSet,
			GlobalVariables &TheGlobalVariables, WeylGroup &theWeyl);
  void ComputeScalarProductsMatrix
		( GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl);
  void ComputeScalarProductsMatrix
		(	GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl, roots& theAlphas,
			roots& theBetas, MatrixLargeRational& output);
  bool ComputeStateReturnFalseIfDubious
    ( GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl, bool AssumeGlobalMinimalityRHS);
  bool CanBeShortened
    ( coneRelation& theRelation, SelectionWithMaxMultiplicity& selAlphas,
			SelectionWithMaxMultiplicity& selBetas, WeylGroup& theWeyl,
			bool AssumeGlobalMinimalityRHS);
  bool IsBKSingularNonImplied(root& input, WeylGroup& theWeyl);
  bool IsBKSingularImplied(root& input, WeylGroup& theWeyl);
  void Assign(const minimalRelationsProverState& right);
	bool RootIsGoodForPreferredSimpleRoot
		(	root& input,int preferredIndex, bool& GoodForAlpha, WeylGroup& theWeyl,
			GlobalVariables& TheGlobalVariables, root& AlphasMinusBetas);
	void GetPossibleAlphasAndBetas(roots& outputAlphas, roots& outputBetas, WeylGroup& theWeyl);
	bool RootIsGoodForProblematicIndex
		(	root& input,int problemIndex, bool AddingAlphas, bool NeedPositiveContribution,
			roots& theDualBasis, WeylGroup& theWeyl);
	bool FindBetaWithoutTwoAlphas
    ( root& outputBeta, roots& inputBetas, roots& inputAlphas, WeylGroup& theWeyl);
  bool ComputeCommonSenseImplicationsReturnFalseIfContradiction
		( WeylGroup& theWeyl,GlobalVariables& TheGlobalVariables);
	bool SatisfyNonLnonBKSingularRoots(WeylGroup& theWeyl,GlobalVariables& TheGlobalVariables);
  bool IsAGoodPosRootsKChoice(WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
  void MakeAlphaBetaMatrix(MatrixLargeRational& output);
  void operator=(const minimalRelationsProverState& right){this->Assign(right);};
  minimalRelationsProverState();
  void MakeProgressReportCanBeShortened
		( int checked, int outOf, GlobalVariables& theGlobalVariables);
};

class minimalRelationsProverStates: public ListBasicObjects<minimalRelationsProverState>
{
public:
  std::string DebugString;
  static int ProblemCounter;
  roots PreferredDualBasis;
  roots PreferredDualBasisEpsilonCoords;
  ListBasicObjects<int> theIndexStack;
  rootSubalgebra isomorphismComputer;
  MatrixLargeRational invertedCartan;
  bool flagAssumeGlobalMinimalityRHS;
 	bool ExtendToIsomorphismRootSystem
		(	minimalRelationsProverState& theState, int indexOther,
			GlobalVariables& theGlobalVariables, WeylGroup& theWeyl);
  bool flagAnErrorHasOccurredTimeToPanic;
  void ElementToString(std::string& output, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
  void GetIsoTypicComponents
    ( roots& theRoots, roots& theOtherTypeRoots, permutation& outputComponents, minimalRelationsProverState& theState,
      WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
  void ComputeDebugString( WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
  {	this->ElementToString(this->DebugString,theWeyl, TheGlobalVariables);
  };
  bool AddObjectOnTopNoRepetitionOfObject
    ( minimalRelationsProverState& theState, WeylGroup& theWeyl,
      GlobalVariables& TheGlobalVariables);
  void GenerateStates
    ( ComputationSetup& theSetup, GlobalVariables& TheGlobalVariables, char WeylLetter, int theDimension);
  void Extend
    (	int index, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
  void ExploreAllChildrenIndex
    ( int index, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
  void ExtensionStep
		( int index, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables,
			minimalRelationsProverState& newState);
	void TestAddingExtraRoot
		(	int Index, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables, root& theRoot,
			bool AddAlpha, int indexAddedRoot, root& normalSeparatingConesOneBetaPositive, bool oneBetaIsPositive);
  bool getNormalSeparatingConesFromPreferredBasis
    ( int theIndex, root& output, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables,
      bool& oneBetaIsPositive );
	bool GetSeparatingRootIfExists
		(	roots& ConeOneStrictlyPositive, roots& ConeNonPositive, root& output, WeylGroup& theWeyl,
			GlobalVariables& TheGlobalVariables);
	bool GetSeparatingRootIfExistsFromSet
		(	roots& ConeOneStrictlyPositive, roots& ConeNonNegative, root& output, WeylGroup& TheWeyl,
			GlobalVariables& TheGlobalVariables, ListBasicObjects<root>& theNormalCandidates);
  void RemoveDoubt
		(	int index, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
	bool StateIsEqualTo
    ( minimalRelationsProverState& theState, int IndexOther, WeylGroup& theWeyl,
      GlobalVariables& TheGlobalVariables);
	void MakeProgressReportStack(GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl);
	void MakeProgressReportIsos
		(	int numSearched, int outOf, GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl);
	void MakeProgressReportCurrentState
		(	int index, GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl);
	void MakeProgressReportChildStates
		(	int numSearched, int outOf, int NewFound,
			GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl);
};

#endif
#endif
