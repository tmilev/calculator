#ifdef polyhedra_h_already_included
//to be merged in polyhedra.h
class minimalRelationsProverState
{
public:
  std::string DebugString;
  void ElementToString(std::string& output);
  void ComputeDebugString(){this->ElementToString(this->DebugString);};
  roots NilradicalRoots;
  roots PositiveKroots;
  roots SimplePosRootsK;
  roots GmodLroots;
  roots BKSingularRoots;
  roots UndecidedRoots;
  coneRelation PartialRelation;
  bool StateIsPossible;
  void ComputeState
    ( coneRelation& input,roots& givenPositiveRootsOfK, GlobalVariables& TheGlobalVariables,
      WeylGroup& theWeyl);
  void AttemptToShorten
    ( coneRelation& theRelation, Selection& selAlphas, Selection& selBetas,
      WeylGroup& theWeyl);

};
#endif
