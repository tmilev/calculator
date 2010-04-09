#ifdef polyhedra_h_already_included
//to be merged in polyhedra.h

class minimalRelationsProverStates;


class minimalRelationsProverState
{
public:
  std::string DebugString;
  void ElementToString(std::string& output);
  void ComputeDebugString(){this->ElementToString(this->DebugString);};
  roots NilradicalRoots;
  roots nonNilradicalRoots;
  roots PositiveKroots;
//  roots SimplePosRootsK;
  roots GmodLroots;
  roots BKSingularRoots;
  roots BKnonSingularRoots;
  roots UndecidedRoots;
  coneRelation PartialRelation;
  MatrixLargeRational theScalarProducts;
  bool StateIsPossible;
  bool StateIsDubious;
  ListBasicObjects<int> childStates;
  minimalRelationsProverStates* owner;
  void ComputeScalarProductsMatrix(GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl);
  bool ComputeStateReturnFalseIfDubious
    ( GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl);
  bool CanBeShortened
    ( coneRelation& theRelation, Selection& selAlphas, Selection& selBetas,
      WeylGroup& theWeyl);
  bool IsBKSingular(root& input, WeylGroup& theWeyl);
  void Assign(const minimalRelationsProverState& right);
  bool FindBetaWithoutTwoAlphas
    (root& outputBeta, roots& inputBetas, roots& inputAlphas, WeylGroup& theWeyl);
  bool IsEqualTo
    ( minimalRelationsProverState& right, WeylGroup& theWeyl,
      GlobalVariables& TheGlobalVariables);
  bool IsAGoodPosRootsKChoice(WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
  void MakeAlphaBetaMatrix(MatrixLargeRational& output);
  void operator=(const minimalRelationsProverState& right){this->Assign(right);};
 // bool operator==(const minimalRelationsProverState& right){return this->IsEqualTo(right);};
};

class minimalRelationsProverStates: public ListBasicObjects<minimalRelationsProverState>
{
public:
  std::string DebugString;
  void ElementToString(std::string& output);
  void ComputeDebugString(){this->ElementToString(this->DebugString);};
  bool AddObjectOnTopNoRepetitionOfObject
    ( minimalRelationsProverState& theState, WeylGroup& theWeyl,
      GlobalVariables& TheGlobalVariables);
  void GenerateStates(GlobalVariables& TheGlobalVariables,WeylGroup& theWeyl);
  void Extend
    (	int index, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
  void ExtensionStep
		( int index, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables,
			minimalRelationsProverState& newState);
  void RemoveDoubt
		(int index, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables);
};
template < > int ListBasicObjects<minimalRelationsProverState>::ListBasicObjectsActualSizeIncrement=10;
#endif
