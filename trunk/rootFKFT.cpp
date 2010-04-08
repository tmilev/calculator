#include "polyhedra.h"
#include "rootFKFT.h"
//to be merged in main file polyhedra.cpp later


bool minimalRelationsProverState::IsBKSingular(root& input, WeylGroup& theWeyl)
{ root tempRoot;
  for (int j=0;j<this->PositiveKroots.size;j++){
    tempRoot=input+this->PositiveKroots.TheObjects[j];
    if(theWeyl.IsARoot(input))
      return false; }
  return true;
}

bool minimalRelationsProverState::ComputeStateReturnFalseIfDubious
  ( GlobalVariables& TheGlobalVariables,  WeylGroup& theWeyl)
{ this->StateIsPossible=true;
  this->StateIsDubious=false;
//  theWeyl.TransformToSimpleBasisGenerators(this->SimplePosRootsK);
  root tempRoot;
  for (int i=0;i<this->BKSingularRoots.size;i++)
    if (!this->IsBKSingular(this->BKSingularRoots.TheObjects[i],theWeyl))    {
      this->StateIsPossible=false;
      this->StateIsDubious=true;
      return false; }
  roots& tempRoots=TheGlobalVariables.rootsProverStateComputation2;
  tempRoots.CopyFromBase(this->PositiveKroots);
  theWeyl.GenerateRootSubsystem(tempRoots);
  tempRoots.AddListOnTop(this->NilradicalRoots);
  int oldsize= tempRoots.size;
  theWeyl.GenerateAdditivelyClosedSubset(tempRoots, tempRoots);
  for(int i=oldsize;i<tempRoots.size;i++)
    this->NilradicalRoots.AddObjectOnTop(tempRoots.TheObjects[i]);
  if (this->NilradicalRoots.ContainsOppositeRoots())
  { this->StateIsPossible=false;
    return false;
  }
  if (this->NilradicalRoots.HasACommonElementWith(this->PositiveKroots) ||
      this->NilradicalRoots.HasACommonElementWith(this->BKSingularRoots)||
      this->BKSingularRoots.HasACommonElementWith(this->PositiveKroots))
  { this->StateIsPossible=false;
    return false;
  }

  Selection selBetas; Selection selAlphas;
  int NumAlphas=MathRoutines::TwoToTheNth(this->PartialRelation.Alphas.size);
  int NumBetas=MathRoutines::TwoToTheNth(this->PartialRelation.Betas.size);
  for (int i=0;i<NumAlphas;i++)  {
    for (int j=0;j<NumBetas;j++) {
      if (this->CanBeShortened(this->PartialRelation,selAlphas,selBetas,theWeyl)){
        return false; }
      selBetas.incrementSelection(); }
    selAlphas.incrementSelection();}
  this->ComputeDebugString();
  return true;
}

bool minimalRelationsProverState::CanBeShortened
  (coneRelation& theRelation, Selection& selAlphas, Selection& selBetas, WeylGroup& theWeyl)
{ if (selBetas.CardinalitySelection==0 && selAlphas.CardinalitySelection==0)
    return false;
  root Candidate; Candidate.MakeZero(theWeyl.KillingFormMatrix.NumRows);
  for(int i=0;i<selBetas.CardinalitySelection;i++)
    Candidate.Add(theRelation.Betas.TheObjects[selBetas.elements[i]]);
  for(int i=0;i<selAlphas.CardinalitySelection;i++)
    Candidate.Subtract(theRelation.Alphas.TheObjects[selAlphas.elements[i]]);
  if (Candidate.IsEqualToZero())
    return true;
  root MinusCandidate;
  MinusCandidate.Assign(Candidate); MinusCandidate.MinusRoot();
  if (theWeyl.IsARoot(Candidate))
  { if( this->NilradicalRoots.ContainsObject(Candidate))
      return true;
    if ( this->PositiveKroots.ContainsObject(Candidate) ||
         this->PositiveKroots.ContainsObject(MinusCandidate))
    { for (int i=0;i<this->NilradicalRoots.size;i++)
        if (theWeyl.IsARoot(this->NilradicalRoots.TheObjects[i]-Candidate))
          return true;
      this->nonNilradicalRoots.AddObjectOnTopNoRepetitionOfObject(Candidate);
      this->UndecidedRoots.PopFirstOccurenceObjectSwapWithLast(Candidate);
    }
    if (selAlphas.CardinalitySelection>1)
    { this->BKnonSingularRoots.AddObjectOnTopNoRepetitionOfObject(MinusCandidate);
      this->UndecidedRoots.PopFirstOccurenceObjectSwapWithLast(MinusCandidate);
      this->StateIsDubious=true;
      if (this->PartialRelation.Alphas.ContainsObject(MinusCandidate))
        return true;
    }
  }
  return false;
}

void minimalRelationsProverState::Assign(const minimalRelationsProverState& right)
{ this->BKSingularRoots.CopyFromBase(right.BKSingularRoots);
  this->GmodLroots.CopyFromBase(right.BKSingularRoots);
  this->childStates.CopyFromBase(right.childStates);
  this->NilradicalRoots.CopyFromBase(right.NilradicalRoots);
  this->owner=right.owner;
  this->PartialRelation=right.PartialRelation;
  this->StateIsPossible=right.StateIsPossible;
  this->PositiveKroots.CopyFromBase(right.PositiveKroots);
  this->UndecidedRoots.CopyFromBase(right.UndecidedRoots);
}

void minimalRelationsProverState::ExtensionStep
  ( minimalRelationsProverStates& owner, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables,
    minimalRelationsProverState& newState)
{ if (owner.AddObjectOnTopNoRepetitionOfObject(newState, theWeyl,TheGlobalVariables))
  { owner.LastObject()->ComputeStateReturnFalseIfDubious(TheGlobalVariables, theWeyl);
    if (owner.LastObject()->StateIsPossible)
    { if (owner.LastObject()->StateIsDubious)
        owner.LastObject()->RemoveDoubt(owner,theWeyl,TheGlobalVariables);
      else
        owner.LastObject()->Extend(owner,theWeyl,TheGlobalVariables);
    }
  }
}

void minimalRelationsProverState::Extend
  (minimalRelationsProverStates& owner, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{ root theBeta, theAlpha;
  owner.ComputeDebugString();
  if (this->FindBetaWithoutTwoAlphas
        (theBeta, this->PartialRelation.Betas, this->PartialRelation.Alphas, theWeyl))
  { owner.MakeActualSizeAtLeastExpandOnTop(owner.size+this->UndecidedRoots.size);
    minimalRelationsProverState newState;
    for (int i=0;i<this->UndecidedRoots.size;i++)
      if (theWeyl.RootScalarKillingFormMatrixRoot(theBeta, this->UndecidedRoots.TheObjects[i])>0)
      { newState.Assign(*this);
        newState.PartialRelation.Alphas.AddObjectOnTop(this->UndecidedRoots.TheObjects[i]);
        this->ExtensionStep(owner, theWeyl, TheGlobalVariables, newState);
      }
    for (int i=0;i<this->BKnonSingularRoots.size;i++)
      if (theWeyl.RootScalarKillingFormMatrixRoot
            (theBeta, this->nonNilradicalRoots.TheObjects[i])>0)
        if (!this->BKnonSingularRoots.ContainsObject(this->nonNilradicalRoots.TheObjects[i]))
        { newState.Assign(*this);
          newState.PartialRelation.Alphas.AddObjectOnTop(this->nonNilradicalRoots.TheObjects[i]);
          this->ExtensionStep(owner, theWeyl, TheGlobalVariables, newState);
        }
  } else if (this->FindBetaWithoutTwoAlphas
              ( theAlpha,this->PartialRelation.Alphas, this->PartialRelation.Betas,theWeyl))
  { owner.MakeActualSizeAtLeastExpandOnTop(owner.size+this->UndecidedRoots.size);
    minimalRelationsProverState newState;
    for (int i=0;i<this->UndecidedRoots.size;i++)
      if (theWeyl.RootScalarKillingFormMatrixRoot(theAlpha, this->UndecidedRoots.TheObjects[i])>0)
      { newState.Assign(*this);
        newState.PartialRelation.Betas.AddObjectOnTop(this->UndecidedRoots.TheObjects[i]);
        this->ExtensionStep(owner, theWeyl, TheGlobalVariables, newState);
      }
    for (int i=0;i<this->BKnonSingularRoots.size;i++)
      if (theWeyl.RootScalarKillingFormMatrixRoot
            (theAlpha, this->BKnonSingularRoots.TheObjects[i])>0)
      { newState.Assign(*this);
        newState.PartialRelation.Betas.AddObjectOnTop(this->BKnonSingularRoots.TheObjects[i]);
        this->ExtensionStep(owner, theWeyl, TheGlobalVariables, newState);
      }
   }
}

bool minimalRelationsProverState::IsAGoodPosRootsKChoice
  (WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{ theWeyl.GenerateAdditivelyClosedSubset(this->PositiveKroots,this->PositiveKroots);
  for (int i=0;i<this->BKSingularRoots.size;i++)
    if (!this->IsBKSingular(this->BKSingularRoots.TheObjects[i],theWeyl))
      return false;
  return true;
}


void minimalRelationsProverState::RemoveDoubt
  (minimalRelationsProverStates& owner, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{ //roots& tempPosRootsK=TheGlobalVariables.rootsRemoveDoubt;
  //tempPosRootsK.size=0;
  //tempPosRootsK.AddListOnTop(this->PositiveKroots);
  for (int i=0;i<this->BKnonSingularRoots.size;i++)
    if (this->IsBKSingular(this->BKnonSingularRoots.TheObjects[i], theWeyl))
      for (int j=0;j<this->UndecidedRoots.size;j++)
      { int oldsize=this->PositiveKroots.size;
        this->PositiveKroots.AddObjectOnTop(this->UndecidedRoots.TheObjects[i]);
        if (this->IsAGoodPosRootsKChoice(theWeyl, TheGlobalVariables))
          this->ExtensionStep(owner, theWeyl,TheGlobalVariables,*this);
        this->PositiveKroots.size=oldsize;
      }
}

bool minimalRelationsProverState::FindBetaWithoutTwoAlphas
  (root& outputBeta, roots& inputBetas, roots& inputAlphas, WeylGroup& theWeyl)
{ for (int i=0;i<inputBetas.size;i++)
    if (inputAlphas.NumRootsConnectedTo(inputBetas.TheObjects[i],theWeyl)<2){
      outputBeta.Assign(inputBetas.TheObjects[i]);
      return true;    }
  return false;
}

bool minimalRelationsProverState::IsEqualTo
  ( minimalRelationsProverState& right, WeylGroup& theWeyl,
    GlobalVariables& TheGlobalVariables)
{ if (this->PartialRelation.Alphas.size!=right.PartialRelation.Alphas.size)
    return false;
  return this->theScalarProducts.IsEqualTo(right.theScalarProducts);
}

bool minimalRelationsProverStates::AddObjectOnTopNoRepetitionOfObject
  ( minimalRelationsProverState& theState, WeylGroup& theWeyl,
    GlobalVariables& TheGlobalVariables)
{ theState.ComputeScalarProductsMatrix(TheGlobalVariables,theWeyl);
  for (int i=0;i<this->size;i++)
    if (this->TheObjects[i].IsEqualTo(theState,theWeyl,TheGlobalVariables))
      return false;
  this->AddObjectOnTop(theState);
  return true;
}

void minimalRelationsProverState::ComputeScalarProductsMatrix
  (GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl)
{ roots& tempRoots=TheGlobalVariables.rootsProverStateComputation1;
  tempRoots.size=0;
  tempRoots.AddListOnTop(this->PartialRelation.Alphas);
  tempRoots.AddListOnTop(this->PartialRelation.Betas);
  this->theScalarProducts.init(tempRoots.size, tempRoots.size);
  for (int i=0;i<tempRoots.size;i++)
    for (int j=0;j<tempRoots.size;j++)
      this->theScalarProducts.elements[i][j].Assign
        (theWeyl.RootScalarKillingFormMatrixRoot
          (tempRoots.TheObjects[i], tempRoots.TheObjects[j]));
}

void minimalRelationsProverState::ElementToString( std::string& output)
{ std::string tempS; std::stringstream out;
  this->PartialRelation.Alphas.ElementToLinearCombinationString(tempS);
  out << tempS <<" = ";
  this->PartialRelation.Betas.ElementToLinearCombinationString(tempS);
  out << tempS;
  output=out.str();
}

void minimalRelationsProverStates::ElementToString(std::string& output)
{ this->ElementToStringGeneric(output);
}

void minimalRelationsProverStates::GenerateStates
  (GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl)
{ minimalRelationsProverState tempState;
  theWeyl.ComputeRho();
  tempState.PartialRelation.Alphas.AddObjectOnTop(theWeyl.RootSystem.TheObjects[0]);
  tempState.ComputeStateReturnFalseIfDubious(TheGlobalVariables,theWeyl);
  tempState.Extend(*this,theWeyl, TheGlobalVariables);
}

void ComputationSetup::DoTheRootSAComputationCustom()
{	//rootSubalgebra theRootSA, theRootSA2;
  minimalRelationsProverStates tempProver;
  WeylGroup tempWeyl;
  tempWeyl.MakeDn(5);
  tempProver.GenerateStates(*this->theGlobalVariablesContainer->Default(),tempWeyl);
  return;
  rootFKFTcomputation tempFKFT;
  std::string tempS;
  tempFKFT.SearchForMinimalRelations(tempS);
  this->theRootSubalgebras.DebugString=tempS;
  if(true)
    return;
	rootSubalgebra tempSA;
	this->theRootSubalgebras.flagUseDynkinClassificationForIsomorphismComputation=false;
	this->theRootSubalgebras.flagUsingActionsNormalizerCentralizerNilradical=true;
	this->theRootSubalgebras.flagComputeConeCondition=true;
	this->theRootSubalgebras.flagLookingForMinimalRels=true;
	this->theRootSubalgebras.theGoodRelations.flagIncludeCoordinateRepresentation=true;
	this->theRootSubalgebras.theBadRelations.flagIncludeCoordinateRepresentation=true;
	this->theRootSubalgebras.theMinRels.flagIncludeCoordinateRepresentation=true;
	this->theRootSubalgebras.theGoodRelations.flagIncludeSubalgebraDataInDebugString=false;
	this->theRootSubalgebras.theBadRelations.flagIncludeSubalgebraDataInDebugString=false;
	this->theRootSubalgebras.GenerateAllRootSubalgebrasUpToIsomorphism
		( *this->theGlobalVariablesContainer->Default(),'E',6,true, true);
  this->theRootSubalgebras.ComputeDebugString
    (true, false,true,0,0,*this->theGlobalVariablesContainer->Default() );
	//this->theRootSubalgebras.GenerateAllRootSubalgebrasUpToIsomorphism
	//	(*this->theGlobalVariablesContainer->Default(),'E',7);
	//this->theRootSubalgebras.SortDescendingOrderBySSRank();
	this->theRootSubalgebras.ComputeLProhibitingRelations
		(*this->theGlobalVariablesContainer->Default(),0,this->theRootSubalgebras.size-1);
//		(*this->theGlobalVariablesContainer->Default(),0,this->theRootSubalgebras.size-1);
}

void WeylGroup::GenerateRootSubsystem(roots& theRoots)
{ root tempRoot;
  int oldsize=theRoots.size;
  for (int i=0;i<oldsize;i++)
    theRoots.AddObjectOnTopNoRepetitionOfObject(-theRoots.TheObjects[i]);
  for (int i =0;i<theRoots.size;i++)
    for (int j=0;j<theRoots.size;j++)
    { tempRoot= theRoots.TheObjects[i]+theRoots.TheObjects[j];
      if (this->IsARoot(tempRoot))
        theRoots.AddObjectOnTopNoRepetitionOfObject(tempRoot);
    }
}
