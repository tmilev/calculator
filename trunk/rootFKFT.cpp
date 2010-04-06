#include "polyhedra.h"
//to be merged in main file polyhedra.cpp later

void rootFKFTcomputation::SearchForMinimalRelations(std::string& output)
{
}

bool rootFKFTcomputation::PartialRelationCouldBeMinimal
  (coneRelation& thePartialRelation, GlobalVariables& TheGlobalVariables)
{ return false;
}

void rootFKFTcomputation::GetImpliedMembers
  ( coneRelation& thePartialRelation, GlobalVariables& TheGlobalVariables,
    roots& impliedNilradical, roots& impliedK, roots& impliedGmodL, roots impliedBKSingular)
{
}


void minimalRelationsProverState::ComputeState
  ( coneRelation& input, roots& givenPositiveRootsOfK, GlobalVariables& TheGlobalVariables,
    WeylGroup& theWeyl)
{ this->StateIsPossible=true;
  this->NilradicalRoots.CopyFromBase(input.Betas);
  this->BKSingularRoots.CopyFromBase(input.Alphas);
  this->PositiveKroots.CopyFromBase(givenPositiveRootsOfK);
  this->SimplePosRootsK.CopyFromBase(this->PositiveKroots);
  theWeyl.TransformToSimpleBasisGenerators(this->SimplePosRootsK);
  root tempRoot;
  for (int i=0;i<this->BKSingularRoots.size;i++)
    for (int j=0;j<this->SimplePosRootsK.size;j++)
    { tempRoot=this->BKSingularRoots.TheObjects[i]+this->SimplePosRootsK.TheObjects[j];
      if(theWeyl.IsARoot(tempRoot))
      { this->StateIsPossible=false;
        return;
      }
    }
  theWeyl.GenerateAdditivelyClosedSubset(this->NilradicalRoots, this->NilradicalRoots);
  Selection selBetas; Selection selAlphas;
  selAlphas.init(input.Alphas.size);
  selBetas.init(input.Betas.size);
  int NumAlphas=MathRoutines::TwoToTheNth(input.Alphas.size);
  int NumBetas=MathRoutines::TwoToTheNth(input.Betas.size);
  for (int i=0;i<NumAlphas;i++)
  { for (int j=0;j<NumBetas;j++)
    { this->AttemptToShorten(input,selAlphas,selBetas,theWeyl);
      selBetas.incrementSelection();
    }
    selAlphas.incrementSelection();
  }
}

void minimalRelationsProverState::AttemptToShorten
  (coneRelation& theRelation, Selection& selAlphas, Selection& selBetas, WeylGroup& theWeyl)
{ root Candidate; Candidate.MakeZero(theWeyl.KillingFormMatrix.NumRows);
  for(int i=0;i<selBetas.CardinalitySelection;i++)
    Candidate.Add(theRelation.Betas.TheObjects[i]);
  for(int i=0;i<selAlphas.CardinalitySelection;i++)
    Candidate.Subtract(theRelation.Alphas.TheObjects[i]);
  if (theWeyl.IsARoot(Candidate))
  {
  }
}

