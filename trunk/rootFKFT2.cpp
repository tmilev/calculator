#include "polyhedra.h"
#include "rootFKFT.h"
//to be merged in main file polyhedra.cpp later

extern ::IndicatorWindowVariables IndicatorWindowGlobalVariables;
bool minimalRelationsProverState::ComputeCommonSenseImplicationsReturnFalseIfContradiction(WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{ root tempRoot;
	this->flagNeedsAdditionOfPositiveKroots=false;
	for (int i=0;i<this->PositiveKroots.size;i++)
		this->nonPositiveKRoots.AddObjectOnTopNoRepetitionOfObject(-this->PositiveKroots.TheObjects[i]);
	for (int j=0;j<theWeyl.RootSystem.size;j++)
	{ root& tested=theWeyl.RootSystem.TheObjects[j];
	  for (int i=0; i<this->BKSingularGmodLRoots.size;i++)
		{ tempRoot = this->BKSingularGmodLRoots.TheObjects[i]+tested;
			if (theWeyl.IsARoot(tempRoot))
			{ this->nonPositiveKRoots.AddObjectOnTopNoRepetitionOfObject(tested);
        this->nonAlphas.AddObjectOnTopNoRepetitionOfObject(tested);
			}
		}
		for (int i=0; i<this->PartialRelation.Betas.size;i++)
			if (theWeyl.IsARoot(this->PartialRelation.Betas.TheObjects[i]+tested))
				this->nonBetas.AddObjectOnTopNoRepetitionOfObject(theWeyl.RootSystem.TheObjects[j]);
	}
	theWeyl.GenerateAdditivelyClosedSubset(this->PositiveKroots,this->PositiveKroots);
  roots& tempRoots=TheGlobalVariables.rootsProverStateComputation2;
  tempRoots.CopyFromBase(this->PositiveKroots);
  theWeyl.GenerateRootSubsystem(tempRoots);
  tempRoots.AddListOnTop(this->NilradicalRoots);
  int oldsize= tempRoots.size;
  theWeyl.GenerateAdditivelyClosedSubset(tempRoots, tempRoots);
  for(int i=oldsize;i<tempRoots.size;i++)
		this->NilradicalRoots.AddObjectOnTop(tempRoots.TheObjects[i]);
	for (int i=0;i<theWeyl.RootSystem.size;i++)
	{ root& theRoot=theWeyl.RootSystem.TheObjects[i];
		if (this->nonPositiveKRoots.ContainsObject(theRoot) && this->nonPositiveKRoots.ContainsObject(-theRoot))
		{ this->nonKRoots.AddObjectOnTopNoRepetitionOfObject(theRoot);
			this->nonKRoots.AddObjectOnTopNoRepetitionOfObject(theRoot);
		}
  }
  this->nonKRoots.intersectWith(this->nonNilradicalRoots,this->nonLRoots);
  for (int i=0;i<this->nonLRoots.size;i++)
		if (this->IsBKSingularImplied(this->nonLRoots.TheObjects[i], theWeyl))
      this->BKSingularGmodLRoots.AddObjectOnTopNoRepetitionOfObject(this->nonLRoots.TheObjects[i]);
  this->nonLRoots.intersectWith(this->nonBKSingularGmodLRoots, this->nonLNonSingularRoots);
	for (int i=0;i<this->NilradicalRoots.size;i++)
	{ this->nonNilradicalRoots.AddObjectOnTopNoRepetitionOfObject(-this->NilradicalRoots.TheObjects[i]);
		this->nonBKSingularGmodLRoots.AddObjectOnTopNoRepetitionOfObject(this->NilradicalRoots.TheObjects[i]);
		this->nonPositiveKRoots.AddObjectOnTopNoRepetitionOfObject(-this->NilradicalRoots.TheObjects[i]);
		this->nonPositiveKRoots.AddObjectOnTopNoRepetitionOfObject(this->NilradicalRoots.TheObjects[i]);
  }
  for (int i=0; i<this->BKSingularGmodLRoots.size; i++)
	{ this->nonNilradicalRoots.AddObjectOnTopNoRepetitionOfObject(this->BKSingularGmodLRoots.TheObjects[i]);
		this->nonPositiveKRoots.AddObjectOnTopNoRepetitionOfObject(this->BKSingularGmodLRoots.TheObjects[i]);
		this->nonPositiveKRoots.AddObjectOnTopNoRepetitionOfObject(-this->BKSingularGmodLRoots.TheObjects[i]);
	}
  if (!this->SatisfyNonLnonBKSingularRoots(theWeyl, TheGlobalVariables))
		return false;
  for (int i=0;i<this->PositiveKroots.size;i++)
	{ this->nonNilradicalRoots.AddObjectOnTopNoRepetitionOfObject( this->PositiveKroots.TheObjects[i]);
		this->nonNilradicalRoots.AddObjectOnTopNoRepetitionOfObject(-this->PositiveKroots.TheObjects[i]);
		this->nonBKSingularGmodLRoots.AddObjectOnTopNoRepetitionOfObject( this->PositiveKroots.TheObjects[i]);
		this->nonBKSingularGmodLRoots.AddObjectOnTopNoRepetitionOfObject(-this->PositiveKroots.TheObjects[i]);
	}
	this->nonAlphas.AddRootSnoRepetition(this->nonBKSingularGmodLRoots);
	this->nonBetas.AddRootSnoRepetition(this->nonNilradicalRoots);
	if(this->nonLNonSingularRoots.HasACommonElementWith(this->BKSingularGmodLRoots))
		return false;
  if ( this->nonLRoots.HasACommonElementWith(this->NilradicalRoots) || this->nonLRoots.HasACommonElementWith(this->PositiveKroots))
    return false;
	if (this->nonNilradicalRoots.HasACommonElementWith(this->NilradicalRoots))
		return false;
	if (this->PositiveKroots.HasACommonElementWith(this->nonPositiveKRoots))
		return false;
	if (this->BKSingularGmodLRoots.HasACommonElementWith(this->nonBKSingularGmodLRoots))
		return false;
  return true;
}

bool minimalRelationsProverState::CanBeShortened
  (	coneRelation& theRelation, SelectionWithMaxMultiplicity& selAlphas, SelectionWithMaxMultiplicity& selBetas, WeylGroup& theWeyl, bool AssumeGlobalMinimalityRHS)
{	//selAlphas.ComputeDebugString();
	//selBetas.ComputeDebugString();
	if (selBetas.CardinalitySelectionWithMultiplicities()==0 && selAlphas.CardinalitySelectionWithMultiplicities()==0)
    return false;
  root Candidate; Candidate.MakeZero(theWeyl.KillingFormMatrix.NumRows);
  root tempRoot;
  for(int i=0;i<selBetas.elements.size;i++)
	{ tempRoot.Assign(theRelation.Betas.TheObjects[selBetas.elements.TheObjects[i]]);
		tempRoot.MultiplyByInteger(selBetas.Multiplicities.TheObjects[selBetas.elements.TheObjects[i]]);
		Candidate.Subtract(tempRoot);
  }
  for(int i=0;i<selAlphas.elements.size;i++)
	{ tempRoot.Assign(theRelation.Alphas.TheObjects[selAlphas.elements.TheObjects[i]]);
		tempRoot.MultiplyByInteger(selAlphas.Multiplicities.TheObjects[selAlphas.elements.TheObjects[i]]);
		Candidate.Add(tempRoot);
  }
  bool bothSelsAreMaximal =( selBetas.CardinalitySelectionWithoutMultiplicities() == theRelation.Betas.size ) && ( selAlphas.CardinalitySelectionWithoutMultiplicities()== theRelation.Alphas.size) ;
  bool bothSelsHaveZeroesAndOnes= selBetas.HasMultiplicitiesZeroAndOneOnly() && selAlphas.HasMultiplicitiesZeroAndOneOnly();
  if (Candidate.IsEqualToZero())
  { if (	bothSelsAreMaximal && selBetas.CardinalitySelectionWithMultiplicities()!=0)
			return false;
    return true;
  }
  root MinusCandidate = -Candidate;
  if (theWeyl.IsARoot(Candidate))
  {	this->nonPositiveKRoots.AddObjectOnTopNoRepetitionOfObject(Candidate);
		if( !bothSelsAreMaximal)
    { this->nonNilradicalRoots.AddObjectOnTopNoRepetitionOfObject(Candidate);
      if (this->NilradicalRoots.ContainsObject(Candidate))
				return true;
    }
    if(bothSelsHaveZeroesAndOnes && ( selAlphas.CardinalitySelectionWithMultiplicities()>0 || selBetas.CardinalitySelectionWithMultiplicities()>1))
    { this->nonNilradicalRoots.AddObjectOnTopNoRepetitionOfObject(MinusCandidate);
      if (this->NilradicalRoots.ContainsObject(MinusCandidate))
				return true;
    }
    if(bothSelsHaveZeroesAndOnes && (selAlphas.elements.size>1 || ( AssumeGlobalMinimalityRHS && selBetas.elements.size>0 && selAlphas.elements.size==1 )))
    { this->nonBKSingularGmodLRoots.AddObjectOnTopNoRepetitionOfObject(Candidate);
      if (this->BKSingularGmodLRoots.ContainsObject(Candidate))
        return true;
    }
    if (!bothSelsAreMaximal && selBetas.CardinalitySelectionWithMultiplicities()>0)
    { this->nonBKSingularGmodLRoots.AddObjectOnTopNoRepetitionOfObject(MinusCandidate);
      if (this->BKSingularGmodLRoots.ContainsObject(MinusCandidate))
        return true;
    }
    if (selAlphas.elements.size==1)
    { this->nonPositiveKRoots.AddObjectOnTopNoRepetitionOfObject(Candidate);
      this->nonPositiveKRoots.AddObjectOnTopNoRepetitionOfObject(MinusCandidate);
      this->nonKRoots.AddObjectOnTopNoRepetitionOfObject(Candidate);
      this->nonKRoots.AddObjectOnTopNoRepetitionOfObject(MinusCandidate);
    }
  }
  return false;
}

bool minimalRelationsProverStateFixedK::CanBeShortened
  (	coneRelation& theRelation, SelectionWithMaxMultiplicity& selAlphas, SelectionWithMaxMultiplicity& selBetas, WeylGroup& theWeyl, bool AssumeGlobalMinimalityRHS)
{	//selAlphas.ComputeDebugString();
	//selBetas.ComputeDebugString();
	if (selBetas.CardinalitySelectionWithMultiplicities()==0 && selAlphas.CardinalitySelectionWithMultiplicities()==0)
    return false;
  root Candidate; Candidate.MakeZero(theWeyl.KillingFormMatrix.NumRows);
  root tempRoot;
  for(int i=0;i<selBetas.elements.size;i++)
	{ tempRoot.Assign(theRelation.Betas.TheObjects[selBetas.elements.TheObjects[i]]);
		tempRoot.MultiplyByInteger(selBetas.Multiplicities.TheObjects[selBetas.elements.TheObjects[i]]);
		Candidate.Subtract(tempRoot);
  }
  for(int i=0;i<selAlphas.elements.size;i++)
	{ tempRoot.Assign(theRelation.Alphas.TheObjects[selAlphas.elements.TheObjects[i]]);
		tempRoot.MultiplyByInteger(selAlphas.Multiplicities.TheObjects[selAlphas.elements.TheObjects[i]]);
		Candidate.Add(tempRoot);
  }
  bool bothSelsAreMaximal =
    ( selBetas.CardinalitySelectionWithoutMultiplicities() == theRelation.Betas.size ) && ( selAlphas.CardinalitySelectionWithoutMultiplicities()== theRelation.Alphas.size) ;
  bool bothSelsHaveZeroesAndOnes= selBetas.HasMultiplicitiesZeroAndOneOnly() && selAlphas.HasMultiplicitiesZeroAndOneOnly();
  if (Candidate.IsEqualToZero())
  { if (	bothSelsAreMaximal && selBetas.CardinalitySelectionWithMultiplicities()!=0)
			return false;
    return true;
  }
  root MinusCandidate = -Candidate;
  if (theWeyl.IsARoot(Candidate))
  { int indexModuleCandidate= this->owner->GetModuleIndex(Candidate);
		int indexModuleMinusCandidate=this->owner->GetModuleIndex(MinusCandidate);
		if (indexModuleCandidate==-1)
		{ assert(indexModuleMinusCandidate==-1);
			if (Candidate.IsNegativeOrZero())
				return false;
			if (selAlphas.CardinalitySelectionWithMultiplicities()==0&& selBetas.CardinalitySelectionWithMultiplicities()!=0)
				return false;
		}
		else
		{	assert(indexModuleMinusCandidate!=-1);
			if( !bothSelsAreMaximal)
			{ this->theGmodLmodules.AddSelectionAppendNewIndex(indexModuleCandidate);
				if (this->theNilradicalModules.selected[indexModuleCandidate])
					return true;
			}
			if(bothSelsHaveZeroesAndOnes && ( selAlphas.CardinalitySelectionWithMultiplicities()>0 || selBetas.CardinalitySelectionWithMultiplicities()>1))
			{ this->theGmodLmodules.AddSelectionAppendNewIndex(indexModuleMinusCandidate);
				if (this->theNilradicalModules.selected[indexModuleMinusCandidate])
					return true;
			}
			if(bothSelsHaveZeroesAndOnes && (selAlphas.elements.size>1 || ( AssumeGlobalMinimalityRHS && selBetas.elements.size>0 && selAlphas.elements.size==1 )))
				if (this->owner->theK.IsBKhighest(Candidate))
				{	this->theNilradicalModules.AddSelectionAppendNewIndex(indexModuleCandidate);
					this->theGmodLmodules.AddSelectionAppendNewIndex(indexModuleMinusCandidate);
					if (this->theGmodLmodules.selected[indexModuleCandidate] || this->theNilradicalModules.selected[indexModuleMinusCandidate])
						return true;
				}
			if (!bothSelsAreMaximal && selBetas.CardinalitySelectionWithMultiplicities()>0)
				if (this->owner->theK.IsBKhighest(MinusCandidate))
				{ this->theNilradicalModules.AddSelectionAppendNewIndex(indexModuleMinusCandidate);
					this->theGmodLmodules.AddSelectionAppendNewIndex(indexModuleCandidate);
					if (this->theNilradicalModules.selected[indexModuleCandidate]|| this->theGmodLmodules.selected[indexModuleMinusCandidate])
						return true;
				}
		}
  }
  return false;
}

bool minimalRelationsProverStates::GetNormalSeparatingConesReturnTrueIfOneBetaIsPositive(int index, root& outputNormal, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{ bool result;
  int theDimension= theWeyl.KillingFormMatrix.NumRows;
  if (!this->GetNormalSeparatingConesFromPreferredBasis
          (index, this->PreferredDualBasis, outputNormal, theWeyl, TheGlobalVariables, result))
    if(!this->GetNormalSeparatingConesFromPreferredBasis
          (index, this->TheObjects[index].theChoicesWeMake, outputNormal, theWeyl, TheGlobalVariables, result))
      if(!this->GetNormalSeparatingConesFromPreferredBasis
            (index, this->TheObjects[index].BKSingularGmodLRoots, outputNormal, theWeyl, TheGlobalVariables, result))
        if(!this->GetNormalSeparatingConesFromPreferredBasis
              (index, this->TheObjects[index].NilradicalRoots, outputNormal, theWeyl, TheGlobalVariables, result))
          if(!this->GetNormalSeparatingConesFromPreferredBasis
               (index, this->TheObjects[index].nonLNonSingularRootsInNeedOfPosKroots, outputNormal, theWeyl, TheGlobalVariables, result))
            if(!this->GetNormalSeparatingConesFromPreferredBasis
                 (index, this->TheObjects[index].nonLNonSingularRoots, outputNormal, theWeyl, TheGlobalVariables, result))
              if (!this->GetNormalSeparatingConesFromPreferredBasis
                  (index, this->TheObjects[index].nonBKSingularGmodLRoots, outputNormal, theWeyl, TheGlobalVariables, result))
                if (!this->GetNormalSeparatingConesFromPreferredBasis
                    (index, this->TheObjects[index].nonNilradicalRoots, outputNormal, theWeyl, TheGlobalVariables, result))
                  if(!this->GetNormalSeparatingConesFromPreferredBasis
                        (index, this->TheObjects[index].ChosenPositiveKroots, outputNormal, theWeyl, TheGlobalVariables, result))
                    if(!this->GetNormalSeparatingConesFromPreferredBasis
                          (index, this->theWeylGroup.RootSystem, outputNormal, theWeyl, TheGlobalVariables, result))
                    { root tempRoot;
                      bool tempBool= roots::GetNormalSeparatingCones( TheGlobalVariables, theDimension, this->TheObjects[index].NilradicalRoots, this->TheObjects[index].PartialRelation.Alphas, tempRoot);
                      assert(tempBool);
                      this->invertedCartan.ActOnAroot(tempRoot, outputNormal);
                    }
	theWeyl.GetEpsilonCoords(outputNormal, this->TheObjects[index].currentSeparatingNormalEpsilonForm, TheGlobalVariables);
	this->TheObjects[index].ComputeDebugString(theWeyl, TheGlobalVariables);
	this->MakeProgressReportCurrentState(index, TheGlobalVariables, theWeyl);
	return result;
}

bool minimalRelationsProverStatesFixedK::GetNormalSeparatingConesReturnTrueIfOneBetaIsPositive(int index, root& outputNormal, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{ bool result;
  int theDimension= theWeyl.KillingFormMatrix.NumRows;
  minimalRelationsProverStateFixedK& theState= this->TheObjects[index];
  roots posAlphas, posBetas, theCertainNilradicalRoots, theCertainGmodLhighestRoots;
  theState.GetPossibleAlphasAndBetas(posAlphas, posBetas, theWeyl);
  theState.GetCertainGmodLhighestAndNilradicalRoots(theCertainGmodLhighestRoots, theCertainNilradicalRoots, theWeyl);
  if(!this->GetNormalSeparatingConesFromPreferredBasis
        (index, theState.theChoicesWeMake, outputNormal, theWeyl, TheGlobalVariables, result))
	//	if(!this->GetNormalSeparatingConesFromPreferredBasis
	//			(index, this->theK.SimpleBasisCentralizerRoots, outputNormal, theWeyl, TheGlobalVariables, result))
//		if(!this->GetNormalSeparatingConesFromPreferredBasis
//				(index, this->theK.SimpleBasisK, outputNormal, theWeyl, TheGlobalVariables, result))
		if(!this->GetNormalSeparatingConesFromPreferredBasis
				(index, theCertainGmodLhighestRoots, outputNormal, theWeyl, TheGlobalVariables, result))
			if(!this->GetNormalSeparatingConesFromPreferredBasis
					(index, theCertainNilradicalRoots, outputNormal, theWeyl, TheGlobalVariables, result))
				if(!this->GetNormalSeparatingConesFromPreferredBasis
             (index, posAlphas, outputNormal, theWeyl, TheGlobalVariables, result))
					if(!this->GetNormalSeparatingConesFromPreferredBasis
               (index, posBetas, outputNormal, theWeyl, TheGlobalVariables, result))
						if(!this->GetNormalSeparatingConesFromPreferredBasis
									(index, this->theWeylGroup.RootSystem, outputNormal, theWeyl, TheGlobalVariables, result))
						{ root tempRoot;
              bool tempBool= roots::GetNormalSeparatingCones( TheGlobalVariables, theDimension, theCertainNilradicalRoots, this->TheObjects[index].PartialRelation.Alphas, tempRoot);
              assert(tempBool);
              this->invertedCartan.ActOnAroot(tempRoot, outputNormal);
            }
	this->TheObjects[index].ComputeDebugString(theWeyl, TheGlobalVariables);
	this->MakeProgressReportCurrentState(index, TheGlobalVariables, theWeyl);
	return result;
}

bool minimalRelationsProverState::StateAllowsPositiveKChoice
  (root& theCandidate, root& theNonSingularRoot, GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl)
{ if ( this->nonPositiveKRoots.ContainsObject(theCandidate) ||  !theWeyl.IsARoot(theCandidate+theNonSingularRoot))
    return  false;
  assert(! this->PositiveKroots.ContainsObject(theCandidate) );
  assert(this->ChosenPositiveKroots.size==this->nonLNonSingularsAleviatedByChosenPosKRoots.size);
  for(int i=0;i<this->nonLNonSingularsAleviatedByChosenPosKRoots.size;i++)
    if ( theWeyl.IsARoot(theCandidate+this->ChosenPositiveKroots.TheObjects[i]+this->nonLNonSingularsAleviatedByChosenPosKRoots.TheObjects[i]) &&
         theWeyl.IsARoot(this->nonLNonSingularsAleviatedByChosenPosKRoots.TheObjects[i]+theCandidate))
      return false;
  return true;
}

void minimalRelationsProverStates::BranchByAddingKRoots(int index, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{	if (this->TheObjects[index].flagNeedsAdditionOfPositiveKroots)
	{ minimalRelationsProverState& theState=this->TheObjects[index];
    minimalRelationsProverState newState;
		assert(theState.nonLNonSingularRootsInNeedOfPosKroots.size>0);
		root& firstRoot=theState.nonLNonSingularRootsInNeedOfPosKroots.TheObjects[0];
		firstRoot.ComputeDebugString();
		theState.nonKRoots.ComputeDebugString();
		theState.ComputeDebugString(theWeyl, TheGlobalVariables);
		int TotalCounter=0; int Counter2=0;
		for (int j=0; j<theWeyl.RootSystem.size;j++)
		 if (theState.StateAllowsPositiveKChoice(theWeyl.RootSystem.TheObjects[j] ,firstRoot, TheGlobalVariables, theWeyl))
			 TotalCounter++;
		for (int j=0; j<theWeyl.RootSystem.size;j++)
		 if (theState.StateAllowsPositiveKChoice(theWeyl.RootSystem.TheObjects[j] ,firstRoot, TheGlobalVariables, theWeyl))
		 { this->MakeProgressReportChildStates(Counter2,TotalCounter,this->TheObjects[index].PossibleChildStates.size, TheGlobalVariables, theWeyl);
			 Counter2++;
			 newState.initFromParent(theState);
			 newState.ChosenPositiveKroots.AddObjectOnTop(theWeyl.RootSystem.TheObjects[j]);
			 newState.nonLNonSingularsAleviatedByChosenPosKRoots.AddObjectOnTop(theWeyl.RootSystem.TheObjects[j]);
			 this->ExtensionStep(index, theWeyl,TheGlobalVariables,newState);
		 }
	}
}

int minimalRelationsProverStates::CountNumSeparatingNormals(roots& theAlphas, roots& theBetas, WeylGroup& theWeyl)
{	int counter=0;
	bool tempBool;
	for (int i=0;i<theWeyl.RootsOfBorel.size;i++)
		if (minimalRelationsProverStateFixedK::IsSeparatingCones(theWeyl.RootsOfBorel.TheObjects[i],theAlphas, theBetas, tempBool, theWeyl))
			counter++;
	return counter;
}

void minimalRelationsProverStates::ComputeLastStackIndex(WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{ int index= *this->theIndexStack.LastObject();
  this->MakeProgressReportCurrentState(index, TheGlobalVariables, theWeyl);
	if (!this->TheObjects[index].StateIsPossible || this->TheObjects[index].StateIsComplete)
    return;
//	this->BranchByAddingKRoots(index, theWeyl, TheGlobalVariables);
	root theBeta, theAlpha, theMinusAlpha, theMinusBeta;
	this->TheObjects[index].PossibleChildStates.size=0;
	minimalRelationsProverState newState;
	int theDimension=theWeyl.KillingFormMatrix.NumRows;
	Rational tempRat;
 	if (!roots::ConesIntersect( TheGlobalVariables, this->TheObjects[index].PartialRelation.Alphas, this->TheObjects[index].NilradicalRoots, theDimension))
	{ root NormalSeparatingCones;
    bool oneBetaIsPositive = this->GetNormalSeparatingConesReturnTrueIfOneBetaIsPositive(index, NormalSeparatingCones, theWeyl, TheGlobalVariables);
    this->TheObjects[index].ComputeDebugString(theWeyl, TheGlobalVariables);
    this->MakeProgressReportCurrentState(index, TheGlobalVariables, theWeyl);
		bool addFirstAlpha=true;
		flagSearchForOptimalSeparatingRoot=false;//(this->TheObjects[index].PartialRelation.Alphas.size+this->TheObjects[index].PartialRelation.Betas.size<7);
    if (this->flagSearchForOptimalSeparatingRoot)
		{	int minNumChildren=theWeyl.RootSystem.size;
			int oldSize=this->size;
			root tempNormal;
			bool tempBetaPos;
			int NumNormalsToCheck=1+this->CountNumSeparatingNormals(this->TheObjects[index].PartialRelation.Alphas, this->TheObjects[index].PartialRelation.Betas,theWeyl);
			int counter=-1;
			for (int i=-1;	i<theWeyl.RootsOfBorel.size; i++)
			{ if (i!=-1) 
					tempNormal= theWeyl.RootsOfBorel.TheObjects[i];
				else
					tempNormal= NormalSeparatingCones;
				if (this->TheObjects[index].IsSeparatingCones(tempNormal, tempBetaPos, theWeyl))
				{	counter++;
					this->InvokeExtensionOfState(index, minNumChildren, tempBetaPos, tempNormal, true, theWeyl, TheGlobalVariables);
					if (this->size-oldSize<minNumChildren)
					{	NormalSeparatingCones.Assign(tempNormal);
						NormalSeparatingCones.ComputeDebugString();
						theWeyl.GetEpsilonCoords(NormalSeparatingCones, this->TheObjects[index].currentSeparatingNormalEpsilonForm, TheGlobalVariables);
						oneBetaIsPositive=tempBetaPos;
						minNumChildren=this->size-oldSize;
					}
					this->size=oldSize;
					this->TheObjects[index].PossibleChildStates.size=0;
					if (TheGlobalVariables.FeedDataToIndicatorWindowDefault!=0)
					{	IndicatorWindowGlobalVariables.String3NeedsRefresh=true;
						std::stringstream out3;
						out3<< counter+1 <<" out of  "<< NumNormalsToCheck<<" normals tested, best hit "<<minNumChildren<<" children";
						IndicatorWindowGlobalVariables.ProgressReportString3= out3.str();
						TheGlobalVariables.FeedDataToIndicatorWindowDefault(IndicatorWindowGlobalVariables);
					}
				}
			}
		}
		this->InvokeExtensionOfState(index, -1, oneBetaIsPositive, NormalSeparatingCones, addFirstAlpha, theWeyl, TheGlobalVariables);
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
			this->MakeProgressReportCurrentState(*this->theIndexStack.LastObject(), TheGlobalVariables, theWeyl);
		this->WriteToFileAppend(TheGlobalVariables);
	}
}

void minimalRelationsProverStatesFixedK::TheFullRecursionFixedK (WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{ while (this->theIndexStack.size>0)
	{ this->RecursionStepFixedK(theWeyl, TheGlobalVariables);
		if (this->theIndexStack.size>0)
			this->MakeProgressReportCurrentState(*this->theIndexStack.LastObject(), TheGlobalVariables, theWeyl);
		this->WriteToFile(this->ProverFileName, TheGlobalVariables);
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
		this->theIndexStack.AddObjectOnTop(this->TheObjects[currentIndex].PossibleChildStates.TheObjects[this->TheObjects[currentIndex].activeChild]);
		this->ComputeLastStackIndex(theWeyl, TheGlobalVariables);
		this->MakeProgressReportStack(TheGlobalVariables, theWeyl);
	}

}

void minimalRelationsProverStatesFixedK::RecursionStepFixedK (WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{ if (this->theIndexStack.size<1)
		return;
	int currentIndex= *this->theIndexStack.LastObject();
	if(this->TheObjects[currentIndex].activeChild>=this->TheObjects[currentIndex].PossibleChildStates.size-1)
		this->theIndexStack.PopLastObject();
	else
	{ this->TheObjects[currentIndex].activeChild++;
		this->theIndexStack.AddObjectOnTop(this->TheObjects[currentIndex].PossibleChildStates.TheObjects[this->TheObjects[currentIndex].activeChild]);
		this->ComputeLastStackIndexFixedK(theWeyl, TheGlobalVariables);
		this->MakeProgressReportStack(TheGlobalVariables, theWeyl);
	}
}


void minimalRelationsProverStates::TestAddingExtraRoot
	( int Index, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables, root& theRoot, bool AddAlpha, int indexAddedRoot, root& normalSeparatingCones, bool oneBetaIsPositive)
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
		tempBool =
      !this->TheObjects[Index].PartialRelation.Alphas.ContainsObject(theRoot)&& !this->TheObjects[Index].nonAlphas.ContainsObject(theRoot) &&
      !this->TheObjects[Index].nonBKSingularGmodLRoots.ContainsObject(theRoot);
  else
		tempBool = !this->TheObjects[Index].PartialRelation.Betas.ContainsObject(theRoot) && !this->TheObjects[Index].nonBetas.ContainsObject(theRoot);
	if (tempBool)
  { newState.initFromParent(this->TheObjects[Index]);
		if (AddAlpha)
			newState.PartialRelation.Alphas.AddObjectOnTop(theRoot);
		else
			newState.PartialRelation.Betas.AddObjectOnTop(theRoot);
    newState.theChoicesWeMake.AddObjectOnTop(theRoot);
		this->ExtensionStep(Index, theWeyl, TheGlobalVariables, newState);
  }
}

void minimalRelationsProverStatesFixedK::TestAddingExtraRootFixedK
	( int Index, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables, root& theRoot, bool AddAlpha, int indexAddedRoot, root& normalSeparatingCones, bool oneBetaIsPositive)
{ minimalRelationsProverStateFixedK newState;
  bool tempBool;
  int indexModule= this->theK.GetIndexKmoduleContainingRoot(theRoot);
  if (indexModule==-1)
		return;
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
		tempBool =
      this->theK.HighestWeightsGmodK.ContainsObject(theRoot) && !this->TheObjects[Index].theNilradicalModules.selected[indexModule] &&
      !this->TheObjects[Index].nonAlphas.ContainsObject(theRoot);
  else
		tempBool = !this->TheObjects[Index].theGmodLmodules.selected[indexModule]&& !this->TheObjects[Index].nonAlphas.ContainsObject(theRoot);
	if (tempBool)
  { newState.initFromParentState(this->TheObjects[Index]);
		if (AddAlpha)
			newState.PartialRelation.Alphas.AddObjectOnTop(theRoot);
		else
			newState.PartialRelation.Betas.AddObjectOnTop(theRoot);
    newState.theChoicesWeMake.AddObjectOnTop(theRoot);
		this->ExtensionStepFixedK(Index, theWeyl, TheGlobalVariables, newState);
  }
}

void minimalRelationsProverStates::ExtensionStep( int index, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables, minimalRelationsProverState& newState)
{ newState.ComputeStateReturnFalseIfDubious(TheGlobalVariables, theWeyl, this->flagAssumeGlobalMinimalityRHS);
  if (newState.StateIsPossible)
  { int currentNewIndex=this->size;
    if (this->AddObjectOnTopNoRepetitionOfObject(index, newState, theWeyl, TheGlobalVariables))
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
		{ tempRat=TheWeyl.RootScalarKillingFormMatrixRoot(ConeOneStrictlyPositive.TheObjects[j],Candidate);
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
  ( int theIndex, ListBasicObjects<root>&inputPreferredBasis, root& output, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables, bool& oneBetaIsPositive )
{ minimalRelationsProverState& theRel=this->TheObjects[theIndex];
  roots& theAlphas = theRel.PartialRelation.Alphas;
  roots& theBetas = theRel.PartialRelation.Betas;
  int firstChoiceSeparated1, firstChoiceSeparated2;
  root rootCandidate;
  bool result=false;
  if( this->GetSeparatingRootIfExistsFromSet( &theRel.theChoicesWeMake, &firstChoiceSeparated1, theAlphas, theBetas, rootCandidate, theWeyl, TheGlobalVariables, inputPreferredBasis))
  { oneBetaIsPositive=false;
    output.Assign(rootCandidate);
    result=true;
  }
  if( this->GetSeparatingRootIfExistsFromSet( &theRel.theChoicesWeMake, &firstChoiceSeparated2, theBetas,theAlphas, rootCandidate, theWeyl, TheGlobalVariables, inputPreferredBasis))
		if (!(result && firstChoiceSeparated1<firstChoiceSeparated2))
    { result=true;
      oneBetaIsPositive=true;
      output.Assign(rootCandidate);
    }
  return result;
}

bool minimalRelationsProverStatesFixedK::GetNormalSeparatingConesFromPreferredBasis
  ( int theIndex, ListBasicObjects<root>&inputPreferredBasis, root& output, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables, bool& oneBetaIsPositive )
{ minimalRelationsProverStateFixedK& theRel=this->TheObjects[theIndex];
  roots& theAlphas = theRel.PartialRelation.Alphas;
  roots& theBetas = theRel.PartialRelation.Betas;
  int firstChoiceSeparated1, firstChoiceSeparated2;
  root rootCandidate;
  bool result=false;
  if(  minimalRelationsProverStates::GetSeparatingRootIfExistsFromSet( &theRel.theChoicesWeMake, &firstChoiceSeparated1, theAlphas, theBetas, rootCandidate, theWeyl, TheGlobalVariables, inputPreferredBasis))
  { oneBetaIsPositive=false;
    output.Assign(rootCandidate);
    result=true;
  }
  if(  minimalRelationsProverStates::GetSeparatingRootIfExistsFromSet
      ( &theRel.theChoicesWeMake, &firstChoiceSeparated2, theBetas,theAlphas, rootCandidate, theWeyl, TheGlobalVariables, inputPreferredBasis))
		if (!(result && firstChoiceSeparated1<firstChoiceSeparated2))
    { result=true;
      oneBetaIsPositive=true;
      output.Assign(rootCandidate);
    }
  return result;
}

