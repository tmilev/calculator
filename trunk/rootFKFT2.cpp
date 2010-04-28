#include "polyhedra.h"
#include "rootFKFT.h"

bool minimalRelationsProverState::ComputeCommonSenseImplicationsReturnFalseIfContradiction
	(WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{	root tempRoot;
	this->flagNeedsAdditionOfPositiveKroots=false;
	for (int i=0;i<this->PositiveKroots.size;i++)
		this->nonPositiveKRoots.AddObjectOnTopNoRepetitionOfObject(-this->PositiveKroots.TheObjects[i]);
	//this->ComputeDebugString(theWeyl, TheGlobalVariables);
 // this->nonBKSingularGmodLRoots.ComputeDebugString();
	for (int j=0;j<theWeyl.RootSystem.size;j++)
	{	root& tested=theWeyl.RootSystem.TheObjects[j];
	  for (int i=0;i<this->BKSingularGmodLRoots.size;i++)
		{	tempRoot= this->BKSingularGmodLRoots.TheObjects[i]+tested;
			if (theWeyl.IsARoot(tempRoot))
			{	this->nonPositiveKRoots.AddObjectOnTopNoRepetitionOfObject(tested);
        this->nonAlphas.AddObjectOnTopNoRepetitionOfObject(tested);
			}
		}
		for (int i=0; i<this->PartialRelation.Betas.size;i++)
			if (theWeyl.IsARoot(this->PartialRelation.Betas.TheObjects[i]+tested))
				this->nonBetas.AddObjectOnTopNoRepetitionOfObject(theWeyl.RootSystem.TheObjects[j]);
	}
	//this->ComputeDebugString(theWeyl, TheGlobalVariables);
	theWeyl.GenerateAdditivelyClosedSubset(this->PositiveKroots,this->PositiveKroots);
  roots& tempRoots=TheGlobalVariables.rootsProverStateComputation2;
  tempRoots.CopyFromBase(this->PositiveKroots);
  //tempRoots.ComputeDebugString();
  theWeyl.GenerateRootSubsystem(tempRoots);
  //tempRoots.ComputeDebugString();
  tempRoots.AddListOnTop(this->NilradicalRoots);
  //tempRoots.ComputeDebugString();
  int oldsize= tempRoots.size;
  theWeyl.GenerateAdditivelyClosedSubset(tempRoots, tempRoots);
  for(int i=oldsize;i<tempRoots.size;i++)
		this->NilradicalRoots.AddObjectOnTop(tempRoots.TheObjects[i]);
	//tempRoots.ComputeDebugString();
  //roots& tempRoots2           = TheGlobalVariables.rootsProverStateComputation3;
	for (int i=0;i<theWeyl.RootSystem.size;i++)
	{	root& theRoot=theWeyl.RootSystem.TheObjects[i];
		if (this->nonPositiveKRoots.ContainsObject(theRoot) &&
				this->nonPositiveKRoots.ContainsObject(-theRoot))
		{	this->nonKRoots.AddObjectOnTopNoRepetitionOfObject(theRoot);
			this->nonKRoots.AddObjectOnTopNoRepetitionOfObject(theRoot);
		}
  } 
  this->nonKRoots.intersectWith(this->nonNilradicalRoots,this->nonLRoots);
  for (int i=0;i<this->nonLRoots.size;i++)
		if (this->IsBKSingularImplied(this->nonLRoots.TheObjects[i], theWeyl))
      this->BKSingularGmodLRoots.AddObjectOnTopNoRepetitionOfObject
        (this->nonLRoots.TheObjects[i]);
  this->nonBKSingularGmodLRoots.ComputeDebugString();
  this->nonLRoots.intersectWith(this->nonBKSingularGmodLRoots, this->nonLNonSingularRoots);
	for (int i=0;i<this->NilradicalRoots.size;i++)
	{	this->nonNilradicalRoots.AddObjectOnTopNoRepetitionOfObject
      (-this->NilradicalRoots.TheObjects[i]);
		this->nonBKSingularGmodLRoots.AddObjectOnTopNoRepetitionOfObject
      (this->NilradicalRoots.TheObjects[i]);
		this->nonPositiveKRoots.AddObjectOnTopNoRepetitionOfObject
      (-this->NilradicalRoots.TheObjects[i]);
		this->nonPositiveKRoots.AddObjectOnTopNoRepetitionOfObject
      (this->NilradicalRoots.TheObjects[i]);
  }
  for (int i=0;i<this->BKSingularGmodLRoots.size;i++)
	{	this->nonNilradicalRoots.AddObjectOnTopNoRepetitionOfObject
      (this->BKSingularGmodLRoots.TheObjects[i]);
		this->nonPositiveKRoots.AddObjectOnTopNoRepetitionOfObject
      (this->BKSingularGmodLRoots.TheObjects[i]);
		this->nonPositiveKRoots.AddObjectOnTopNoRepetitionOfObject
      (-this->BKSingularGmodLRoots.TheObjects[i]);
	}
  if (!this->SatisfyNonLnonBKSingularRoots(theWeyl, TheGlobalVariables))
		return false;
  for (int i=0;i<this->PositiveKroots.size;i++)
	{	this->nonNilradicalRoots.AddObjectOnTopNoRepetitionOfObject( this->PositiveKroots.TheObjects[i]);
		this->nonNilradicalRoots.AddObjectOnTopNoRepetitionOfObject(-this->PositiveKroots.TheObjects[i]);
		this->nonBKSingularGmodLRoots.AddObjectOnTopNoRepetitionOfObject( this->PositiveKroots.TheObjects[i]);
		this->nonBKSingularGmodLRoots.AddObjectOnTopNoRepetitionOfObject(-this->PositiveKroots.TheObjects[i]);
	}
	this->nonAlphas.AddRootSnoRepetition(this->nonBKSingularGmodLRoots);
	this->nonBetas.AddRootSnoRepetition(this->nonNilradicalRoots);
//	this->ComputeDebugString(theWeyl, TheGlobalVariables);
	if(this->nonLNonSingularRoots.HasACommonElementWith(this->BKSingularGmodLRoots))
		return false;
	this->nonLRoots.ComputeDebugString();
	this->NilradicalRoots.ComputeDebugString();
	this->PositiveKroots.ComputeDebugString();
  if (this->nonLRoots.HasACommonElementWith(this->NilradicalRoots) || this->nonLRoots.HasACommonElementWith(this->PositiveKroots))
    return false;
	if (this->nonNilradicalRoots.HasACommonElementWith(this->NilradicalRoots))
		return false;
	if (this->PositiveKroots.HasACommonElementWith(this->nonPositiveKRoots))
		return false;
	if (this->BKSingularGmodLRoots.HasACommonElementWith(this->nonBKSingularGmodLRoots))
		return false;
	//if (this->BKSingularGmodLRoots.HasACommonElementWith(this->nonBKSingularGmodLRoots))
	//	return false;
  return true;
}

bool minimalRelationsProverState::CanBeShortened
  (	coneRelation& theRelation, SelectionWithMaxMultiplicity& selAlphas, SelectionWithMaxMultiplicity& selBetas, WeylGroup& theWeyl,
		bool AssumeGlobalMinimalityRHS)
{	selAlphas.ComputeDebugString();
	selBetas.ComputeDebugString();
	if (selBetas.CardinalitySelectionWithMultiplicities()==0 &&
			selAlphas.CardinalitySelectionWithMultiplicities()==0)
    return false;
  root Candidate; Candidate.MakeZero(theWeyl.KillingFormMatrix.NumRows);
  root tempRoot;
  for(int i=0;i<selBetas.elements.size;i++)
	{	tempRoot.Assign(theRelation.Betas.TheObjects[selBetas.elements.TheObjects[i]]);
		tempRoot.MultiplyByInteger(selBetas.Multiplicities.TheObjects[selBetas.elements.TheObjects[i]]);
		Candidate.Add(tempRoot);
  }
  for(int i=0;i<selAlphas.elements.size;i++)
	{	tempRoot.Assign(theRelation.Alphas.TheObjects[selAlphas.elements.TheObjects[i]]);
		tempRoot.MultiplyByInteger(selAlphas.Multiplicities.TheObjects[selAlphas.elements.TheObjects[i]]);
		Candidate.Subtract(tempRoot);
  }
  if (Candidate.IsEqualToZero())
  {	if (	selBetas.CardinalitySelectionWithMultiplicities() == selBetas.MaxCardinalityWithMultiplicities() &&
					selAlphas.CardinalitySelectionWithMultiplicities()== selAlphas.MaxCardinalityWithMultiplicities())
			return false;
    return true;
  }
  root MinusCandidate;
  MinusCandidate.Assign(Candidate); MinusCandidate.MinusRoot();
  Candidate.ComputeDebugString(); MinusCandidate.ComputeDebugString();
  if (theWeyl.IsARoot(Candidate))
  { if( selAlphas.CardinalitySelectionWithMultiplicities()>0 ||
        selBetas.CardinalitySelectionWithMultiplicities()>1)
    { if (this->NilradicalRoots.ContainsObject(Candidate))
				return true;
			else
				this->nonNilradicalRoots.AddObjectOnTopNoRepetitionOfObject(Candidate);
    }
    if ( this->PositiveKroots.ContainsObject(Candidate) ||
         this->PositiveKroots.ContainsObject(MinusCandidate))
    { for (int i=0;i<this->NilradicalRoots.size;i++)
        if (theWeyl.IsARoot(this->NilradicalRoots.TheObjects[i]-Candidate))
          return true;
      this->nonNilradicalRoots.AddObjectOnTopNoRepetitionOfObject(Candidate);
    }
//    if (selAlphas.CardinalitySelectionWithMultiplicities()>1)
    if (selAlphas.elements.size!=0)
    { if (selAlphas.elements.size>1 ||
            ( AssumeGlobalMinimalityRHS && selBetas.elements.size!=0 &&
              selAlphas.elements.size==1 ))
      { this->nonBKSingularGmodLRoots.AddObjectOnTopNoRepetitionOfObject(MinusCandidate);
  //      this->StateIsDubious=true;
        if (this->PartialRelation.Alphas.ContainsObject(MinusCandidate))
          return true;
      }
      if (selAlphas.elements.size==1)
      { this->nonPositiveKRoots.AddObjectOnTopNoRepetitionOfObject(MinusCandidate);
        this->nonPositiveKRoots.AddObjectOnTopNoRepetitionOfObject(Candidate);
      }
      if ((!((selBetas.CardinalitySelectionWithMultiplicities() == selBetas.MaxCardinalityWithMultiplicities()) &&
            selAlphas.CardinalitySelectionWithMultiplicities()== selAlphas.MaxCardinalityWithMultiplicities()) &&
            selBetas.CardinalitySelectionWithMultiplicities()>0 )|| selBetas.CardinalitySelectionWithMultiplicities()==1)
        this->nonBKSingularGmodLRoots.AddObjectOnTopNoRepetitionOfObject(Candidate);
    }
  }
  return false;
}

bool minimalRelationsProverStates::GetNormalSeparatingConesReturnTrueIfOneBetaIsPositive
  (int index, root& outputNormal, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{ bool result;
  int theDimension= theWeyl.KillingFormMatrix.NumRows;
  if (!this->GetNormalSeparatingConesFromPreferredBasis
          (index, this->PreferredDualBasis, outputNormal, theWeyl, TheGlobalVariables, result))
    if(!this->GetNormalSeparatingConesFromPreferredBasis
          (index, this->TheObjects[index].theChoicesWeMake, outputNormal, theWeyl, TheGlobalVariables, result))
			if(!this->GetNormalSeparatingConesFromPreferredBasis
						(index, this->TheObjects[index].ChosenPositiveKroots, outputNormal, theWeyl, TheGlobalVariables, result))
				if(!this->GetNormalSeparatingConesFromPreferredBasis
							(index, this->theWeylGroup.RootSystem, outputNormal, theWeyl, TheGlobalVariables, result))
				{ root tempRoot;
					bool tempBool= roots::GetNormalSeparatingCones
						( TheGlobalVariables, theDimension, this->TheObjects[index].NilradicalRoots, 
							this->TheObjects[index].PartialRelation.Alphas, tempRoot);
					assert(tempBool);
					this->invertedCartan.ActOnAroot(tempRoot, outputNormal);
				}
	theWeyl.GetEpsilonCoords
		(outputNormal, this->TheObjects[index].currentSeparatingNormalEpsilonForm, TheGlobalVariables);
	this->TheObjects[index].ComputeDebugString(theWeyl, TheGlobalVariables);
	this->MakeProgressReportCurrentState(index, TheGlobalVariables, theWeyl);
	return result;
}

void minimalRelationsProverStates::ComputeLastStackIndex(WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{ int index= *this->theIndexStack.LastObject();
  this->MakeProgressReportCurrentState(index, TheGlobalVariables, theWeyl);
	if (!this->TheObjects[index].StateIsPossible || this->TheObjects[index].StateIsComplete)
    return;
	if (this->TheObjects[index].flagNeedsAdditionOfPositiveKroots)
	{ minimalRelationsProverState& theState=this->TheObjects[index];
		minimalRelationsProverState newState;
		assert(theState.nonLNonSingularRootsInNeedOfPosKroots.size>0);
		root& firstRoot=theState.nonLNonSingularRootsInNeedOfPosKroots.TheObjects[0];
		firstRoot.ComputeDebugString();
		theState.nonKRoots.ComputeDebugString();
		theState.ComputeDebugString(theWeyl, TheGlobalVariables);
		int TotalCounter=0; int Counter2=0;
		for (int j=0; j<theWeyl.RootSystem.size;j++)
			if (!theState.nonPositiveKRoots.ContainsObject(theWeyl.RootSystem.TheObjects[j])&&
					!theState.PositiveKroots.ContainsObject(theWeyl.RootSystem.TheObjects[j]) &&
					theWeyl.IsARoot(firstRoot+theWeyl.RootSystem.TheObjects[j]))
				TotalCounter++;
		for (int j=0; j<theWeyl.RootSystem.size;j++)
			if (!theState.nonPositiveKRoots.ContainsObject(theWeyl.RootSystem.TheObjects[j])&&
					!theState.PositiveKroots.ContainsObject(theWeyl.RootSystem.TheObjects[j]) &&
					theWeyl.IsARoot(firstRoot+theWeyl.RootSystem.TheObjects[j]))
			{ this->MakeProgressReportChildStates
					(Counter2,TotalCounter,this->TheObjects[index].PossibleChildStates.size, TheGlobalVariables, theWeyl);
				Counter2++;
				newState.Assign(theState);
				newState.ChosenPositiveKroots.AddObjectOnTop(theWeyl.RootSystem.TheObjects[j]);
				this->ExtensionStep(index, theWeyl,TheGlobalVariables,newState);
			}
		return;
	}
	root theBeta, theAlpha, theMinusAlpha, theMinusBeta;
	//this->ComputeDebugString(theWeyl, TheGlobalVariables);
	this->TheObjects[index].PossibleChildStates.size=0;
	minimalRelationsProverState newState;
	int theDimension=theWeyl.KillingFormMatrix.NumRows;
	//roots& theAlphas = this->TheObjects[index].PartialRelation.Alphas;
	//roots& theBetas  = this->TheObjects[index].PartialRelation.Betas;

	Rational tempRat;
 	if (!roots::ConesIntersect
				( TheGlobalVariables, this->TheObjects[index].PartialRelation.Alphas,
					this->TheObjects[index].NilradicalRoots, theDimension))
	{ root NormalSeparatingCones;
    bool oneBetaIsPositive = this->GetNormalSeparatingConesReturnTrueIfOneBetaIsPositive
      (index, NormalSeparatingCones, theWeyl, TheGlobalVariables);

		bool addFirstAlpha=true;
		if (this->MinNumDifferentBetas==-1 || this->MinNumDifferentBetas<=this->TheObjects[index].PartialRelation.Betas.size)
    { for (int i=0;i<theWeyl.RootSystem.size;i++)
      { this->TestAddingExtraRoot
          ( index, theWeyl, TheGlobalVariables, theWeyl.RootSystem.TheObjects[i],
            addFirstAlpha, i, NormalSeparatingCones, oneBetaIsPositive);
        this->MakeProgressReportChildStates
          ( i, theWeyl.RootSystem.size*2, this->TheObjects[index].PossibleChildStates.size, TheGlobalVariables, theWeyl);
      }
      for (int i=0;i<theWeyl.RootSystem.size;i++)
      {	this->TestAddingExtraRoot
          ( index, theWeyl, TheGlobalVariables, theWeyl.RootSystem.TheObjects[i],
            !addFirstAlpha, i, NormalSeparatingCones, oneBetaIsPositive);
        this->MakeProgressReportChildStates
          ( i+theWeyl.RootSystem.size, theWeyl.RootSystem.size*2, this->TheObjects[index].PossibleChildStates.size, TheGlobalVariables, theWeyl);
      }
    } else
    { minimalRelationsProverState newState;
      this->TheObjects[index].currentSeparatingNormalEpsilonForm.MakeZero(theDimension);
      for(int i=0;i<theWeyl.RootSystem.size;i++)
      { root& newRoot= theWeyl.RootSystem.TheObjects[i];
        if(!this->TheObjects[index].nonNilradicalRoots.ContainsObject(newRoot) &&
            !this->TheObjects[index].PartialRelation.Betas.ContainsObject(newRoot))
        { newState.Assign(this->TheObjects[index]);
          newState.PartialRelation.Betas.AddObjectOnTop(newRoot);
          newState.theChoicesWeMake.AddObjectOnTop(newRoot);
          this->ExtensionStep(index, theWeyl,TheGlobalVariables,newState);
          this->MakeProgressReportChildStates
            ( i, theWeyl.RootSystem.size, this->TheObjects[index].PossibleChildStates.size, TheGlobalVariables, theWeyl);
        }
      }
    }
	}
	else
	{ this->MakeProgressReportCurrentState(index, TheGlobalVariables, theWeyl);
		this->TheObjects[index].StateIsComplete=true;
	}
//	this->TheObjects[index].ComputeDebugString(theWeyl, TheGlobalVariables);
}

void minimalRelationsProverStates::TheFullRecursion(	WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{ while (this->theIndexStack.size>0)
	{ this->RecursionStep(theWeyl, TheGlobalVariables);
		if (this->theIndexStack.size>0)
			this->MakeProgressReportCurrentState(*this->theIndexStack.LastObject(),TheGlobalVariables,theWeyl);
	}
}

void minimalRelationsProverStates::RecursionStep(	WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{ if (this->theIndexStack.size<1)
		return;
	int currentIndex= *this->theIndexStack.LastObject();
	if(this->TheObjects[currentIndex].activeChild>=this->TheObjects[currentIndex].PossibleChildStates.size-1)
		this->theIndexStack.PopLastObject();
	else
	{ this->TheObjects[currentIndex].activeChild++;
		this->theIndexStack.AddObjectOnTop
			(this->TheObjects[currentIndex].PossibleChildStates.TheObjects[this->TheObjects[currentIndex].activeChild]);
		this->ComputeLastStackIndex(theWeyl, TheGlobalVariables);
		this->MakeProgressReportStack(TheGlobalVariables, theWeyl);
	}
}

void minimalRelationsProverStates::TestAddingExtraRoot
	( int Index, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables, root& theRoot,
    bool AddAlpha, int indexAddedRoot, root& normalSeparatingCones, bool oneBetaIsPositive)
{ minimalRelationsProverState newState;
  bool tempBool;
  Rational tempRat=theWeyl.RootScalarKillingFormMatrixRoot(theRoot, normalSeparatingCones);
	if (tempRat.IsEqualToZero())
    return;
	if (AddAlpha)
  { if (oneBetaIsPositive==tempRat.IsNegative())
      return;
  } else
    if (oneBetaIsPositive==tempRat.IsPositive())
      return;
  if (AddAlpha)
  { tempBool =
			(!this->TheObjects[Index].PartialRelation.Alphas.ContainsObject(theRoot))&&
			(!this->TheObjects[Index].nonAlphas.ContainsObject(theRoot));
  } else
  { tempBool =
 			(!this->TheObjects[Index].PartialRelation.Betas.ContainsObject(theRoot))&&
			(!this->TheObjects[Index].nonBetas.ContainsObject(theRoot));
  }
	if (tempBool)
  { newState.Assign(this->TheObjects[Index]);
		if (AddAlpha)
			newState.PartialRelation.Alphas.AddObjectOnTop(theRoot);
		else
			newState.PartialRelation.Betas.AddObjectOnTop(theRoot);
    newState.theChoicesWeMake.AddObjectOnTop(theRoot);
		this->ExtensionStep(Index, theWeyl, TheGlobalVariables, newState);
  }
}


void minimalRelationsProverStates::ExtensionStep
  ( int index, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables, minimalRelationsProverState& newState)
{ newState.ComputeStateReturnFalseIfDubious(TheGlobalVariables, theWeyl,this->flagAssumeGlobalMinimalityRHS);
  if (newState.StateIsPossible)
  { int currentNewIndex=this->size;
    if (this->AddObjectOnTopNoRepetitionOfObject(newState, theWeyl, TheGlobalVariables))
    { //this->TheObjects[currentNewIndex].ComputeStateReturnFalseIfDubious
      //  (TheGlobalVariables, theWeyl,  this->flagAssumeGlobalMinimalityRHS);
  //		this->ComputeDebugString(theWeyl, TheGlobalVariables);
      this->TheObjects[currentNewIndex].ComputeDebugString(theWeyl,TheGlobalVariables);
      if (this->TheObjects[currentNewIndex].StateIsPossible && !this->TheObjects[currentNewIndex].StateIsComplete)
      { //if (this->TheObjects[currentNewIndex].StateIsDubious)
        //  this->RemoveDoubt(currentNewIndex,theWeyl,TheGlobalVariables);
        //else
        this->TheObjects[index].PossibleChildStates.AddObjectOnTop(currentNewIndex);
        this->TheObjects[index].activeChild=-1;
      } else
        if(!this->TheObjects[currentNewIndex].StateIsPossible)
          this->TheObjects[index].ImpossibleChildStates.AddObjectOnTop(currentNewIndex);
        else
          this->TheObjects[index].CompleteChildStates.AddObjectOnTop(currentNewIndex);
    }
  }
}

bool minimalRelationsProverStates::GetSeparatingRootIfExistsFromSet
  (	roots* choicePreferrence, int* choiceIndex, roots& ConeOneStrictlyPositive, roots& ConeNonPositive, root& output, WeylGroup& TheWeyl,
		GlobalVariables& TheGlobalVariables, ListBasicObjects<root>& theNormalCandidates)
{ Rational tempRat;
  bool foundSeparatingRoot=false;
  if (choicePreferrence!=0)
    *choiceIndex= choicePreferrence->size;
  for (int i=0;i<theNormalCandidates.size;i++)
	{ bool isGood=true;
    bool oneFound=false;
		root& Candidate=theNormalCandidates.TheObjects[i];
		for (int j=0; j<ConeOneStrictlyPositive.size;j++)
		{ tempRat=TheWeyl.RootScalarKillingFormMatrixRoot
				(ConeOneStrictlyPositive.TheObjects[j],Candidate);
		  if ( tempRat.IsNegative())
			{ isGood=false;
				break;
			} else
        if (tempRat.IsPositive())
          oneFound=true;
		}
		if (!oneFound)
      isGood=false;
		if (isGood)
			for (int j=0; j<ConeNonPositive.size;j++)
				if ( TheWeyl.RootScalarKillingFormMatrixRoot
							(ConeNonPositive.TheObjects[j],Candidate).IsPositive())
				{ isGood=false;
					break;
				}
		if (isGood)
		{ foundSeparatingRoot=true;
//			output.ComputeDebugString();
      if (choicePreferrence==0)
      { output.Assign(Candidate);
        return true;
      } else
      { for (int i=0;i<choicePreferrence->size;i++)
          if (TheWeyl.RootScalarKillingFormMatrixRoot(Candidate,choicePreferrence->TheObjects[i]).IsPositive())
            if (i<*choiceIndex)
            { *choiceIndex=i;
              output.Assign(Candidate);
              break;
            }
      }
		}
	}
	if (foundSeparatingRoot)
    return true;
	return false;
}

bool minimalRelationsProverStates::GetNormalSeparatingConesFromPreferredBasis
  ( int theIndex, ListBasicObjects<root>&inputPreferredBasis, root& output, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables,
    bool& oneBetaIsPositive )
{ minimalRelationsProverState& theRel=this->TheObjects[theIndex];
  roots& theAlphas = theRel.PartialRelation.Alphas;
  roots& theBetas = theRel.PartialRelation.Betas;
  int firstChoiceSeparated1, firstChoiceSeparated2;
  root rootCandidate;
  bool result=false;
  if( this->GetSeparatingRootIfExistsFromSet
      ( &theRel.theChoicesWeMake, &firstChoiceSeparated1, theAlphas, theBetas, rootCandidate, theWeyl, TheGlobalVariables,
        inputPreferredBasis))
  { oneBetaIsPositive=false;
    output.Assign(rootCandidate);
    result=true;
  }
  if( this->GetSeparatingRootIfExistsFromSet
      ( &theRel.theChoicesWeMake, &firstChoiceSeparated2, theBetas,theAlphas, rootCandidate, theWeyl, TheGlobalVariables,
        inputPreferredBasis))
  { if (!(result && firstChoiceSeparated1<firstChoiceSeparated2))
    { result=true;
      oneBetaIsPositive=true;
      output.Assign(rootCandidate);
    }
  }
  return result;
}
