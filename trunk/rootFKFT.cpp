#include "polyhedra.h"
#include "rootFKFT.h"
//to be merged in main file polyhedra.cpp later

int minimalRelationsProverState::ProblemCounter=0;
int ::minimalRelationsProverStates::ProblemCounter=0;
template < > int ListBasicObjects<minimalRelationsProverState>::ListBasicObjectsActualSizeIncrement=800;
extern ::IndicatorWindowVariables IndicatorWindowGlobalVariables;

bool StopDebug()
{ return true;
}


bool minimalRelationsProverState::SumWithNoPosRootIsARoot(root& input, WeylGroup& theWeyl)
{ root tempRoot;
  for (int j=0;j<this->PositiveKroots.size;j++)
  { tempRoot=input+this->PositiveKroots.TheObjects[j];
    if(theWeyl.IsARoot(tempRoot))
      return false;
  }
  return true;
}

bool minimalRelationsProverState::IsBKSingularImplied(root& input, WeylGroup& theWeyl)
{ if (!this->SumWithNoPosRootIsARoot(input, theWeyl))
    return false;
  root tempRoot;
  for (int j=0;j<theWeyl.RootSystem.size;j++)
		if (!this->nonPositiveKRoots.ContainsObject( theWeyl.RootSystem.TheObjects[j]))
		{	tempRoot=input+theWeyl.RootSystem.TheObjects[j];
			if(theWeyl.IsARoot(tempRoot))
				return false;
		}
  return true;
}

void minimalRelationsProverState::SortAlphasAndBetas(GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl)
{	roots& theAlphas = this->PartialRelation.Alphas;
	roots& theBetas  = this->PartialRelation.Betas;
	for(int i=0;i<theAlphas.size;i++)
		for (int j=i+1;j<theAlphas.size;j++)
			if (this->Root1IsGreaterThanRoot2(j,i,theAlphas,theBetas,TheGlobalVariables,theWeyl))
				theAlphas.SwapTwoIndices(i,j);
	for(int i=0;i<theBetas.size;i++)
		for (int j=i+1;j<theBetas.size;j++)
			if (this->Root1IsGreaterThanRoot2(j,i,theBetas,theAlphas,TheGlobalVariables,theWeyl))
				theBetas.SwapTwoIndices(i,j);
}

void minimalRelationsProverStateFixedK::SortAlphasAndBetas(GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl)
{	roots& theAlphas = this->PartialRelation.Alphas;
	roots& theBetas  = this->PartialRelation.Betas;
	for(int i=0;i<theAlphas.size;i++)
		for (int j=i+1;j<theAlphas.size;j++)
			if (minimalRelationsProverState::Root1IsGreaterThanRoot2(j,i,theAlphas,theBetas,TheGlobalVariables,theWeyl))
				theAlphas.SwapTwoIndices(i,j);
	for(int i=0;i<theBetas.size;i++)
		for (int j=i+1;j<theBetas.size;j++)
			if (minimalRelationsProverState::Root1IsGreaterThanRoot2(j,i,theBetas,theAlphas,TheGlobalVariables,theWeyl))
				theBetas.SwapTwoIndices(i,j);
}

bool minimalRelationsProverState::Root1IsGreaterThanRoot2
	(	int index1, int index2,roots& setWeBelongTo, roots& setOtherSet,
		GlobalVariables &TheGlobalVariables, WeylGroup &theWeyl)
{	root& root1=setWeBelongTo.TheObjects[index1];
	root& root2=setWeBelongTo.TheObjects[index2];
	bool IsLong1, IsLong2;
	int NumLongValue1, NumMixedValue1, NumShortValue1,  NumMinusLongValue1, NumMinusMixedValue1, NumMinusShortValue1,
			NumLongValue2, NumMixedValue2, NumShortValue2,	NumMinusLongValue2, NumMinusMixedValue2, NumMinusShortValue2;
	minimalRelationsProverState::GetNumberScalarProductsData
		(	root1,setWeBelongTo, IsLong1, NumLongValue1, NumMixedValue1, NumShortValue1, NumMinusLongValue1, NumMinusMixedValue1, NumMinusShortValue1,
			TheGlobalVariables, theWeyl);
	minimalRelationsProverState::GetNumberScalarProductsData
		(	root2,setWeBelongTo, IsLong2, NumLongValue2, NumMixedValue2, NumShortValue2,		NumMinusLongValue2, NumMinusMixedValue2, NumMinusShortValue2,
			TheGlobalVariables, theWeyl);
	if (IsLong1> IsLong2) return true;
	if (IsLong1< IsLong2) return false;
	if (NumLongValue1>NumLongValue2) return true;
	if (NumLongValue1<NumLongValue2) return false;
	if (NumMixedValue1>NumMixedValue2) return true;
	if (NumMixedValue1<NumMixedValue2) return false;
	if (NumShortValue1>NumShortValue2) return true;
	if (NumShortValue1<NumShortValue2) return false;
	if (NumMinusLongValue1>NumMinusLongValue2) return true;
	if (NumMinusLongValue1<NumMinusLongValue2) return false;
	if (NumMinusMixedValue1>NumMinusMixedValue2) return true;
	if (NumMinusMixedValue1<NumMinusMixedValue2) return false;
	if (NumMinusShortValue1>NumMinusShortValue2) return true;
	if (NumMinusShortValue1<NumMinusShortValue2) return false;
	return false;
}

void minimalRelationsProverState::GetNumberScalarProductsData
	(	root& input, roots& theRoots, bool& isLong,
		int& NumLongValue, int& NumMixedValue, int& NumShortValue,
		int& NumMinusLongValue, int& NumMinusMixedValue, int& NumMinusShortValue,
		GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl)
{	Rational tempRat = theWeyl.RootScalarKillingFormMatrixRoot(input,input);
	isLong=false; NumMinusShortValue=0;
	NumLongValue=0; NumMixedValue=0; NumShortValue=0; NumMinusLongValue=0;
	NumMinusMixedValue=0;
	if (tempRat.IsEqualTo(theWeyl.LongRootLength))
		isLong=true;
	Rational tempRat2;
	for (int i=0;i<theRoots.size;i++)
		if (!input.IsEqualTo(theRoots.TheObjects[i]))
		{ theWeyl.RootScalarKillingFormMatrixRoot(input,theRoots.TheObjects[i],tempRat);
			if (!tempRat.IsEqualToZero())
			{	theWeyl.RootScalarKillingFormMatrixRoot
					(theRoots.TheObjects[i],theRoots.TheObjects[i],tempRat2);
				bool otherIsLong=false;
				bool ScalarProdIsPositive=tempRat.IsPositive();
				if (theWeyl.LongRootLength.IsEqualTo(tempRat2))
					otherIsLong=true;
				if (!(otherIsLong==isLong))
				{ if (ScalarProdIsPositive)
						NumMixedValue++;
					else
						NumMinusMixedValue++;
				} else
				{ if (isLong)
					{	if (ScalarProdIsPositive)
							NumLongValue++;
						else
							NumMinusLongValue++;
					}	else
					{	if (ScalarProdIsPositive)
							NumShortValue++;
						else
							NumMinusShortValue++;
					}
				}
			}
		}
}

bool minimalRelationsProverState::SatisfyNonLnonBKSingularRoots
	(WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{	root tempRoot;
	//this->nonBKSingularGmodLRoots.ComputeDebugString();
	//this->ComputeDebugString(theWeyl, TheGlobalVariables);
	for (int i=0;i<this->nonLNonSingularRoots.size;i++)
	{ root& theRoot= this->nonLNonSingularRoots.TheObjects[i];
    int LastFoundIndex=-1;
		int NumFoundIndices=0;
		for(int j=0;j<theWeyl.RootSystem.size;j++)
		{	tempRoot=theRoot+theWeyl.RootSystem.TheObjects[j];
			if (theWeyl.IsARoot(tempRoot))
			{	if (this->PositiveKroots.ContainsObject(theWeyl.RootSystem.TheObjects[j]))
				{	NumFoundIndices++;
					LastFoundIndex=j;
					break;
				}
				if (!this->nonPositiveKRoots.ContainsObject(theWeyl.RootSystem.TheObjects[j]))
				{	NumFoundIndices++;
					LastFoundIndex=j;
				}
			}
		}
		if (NumFoundIndices==0)
		{ this->StateIsInternallyPossible=false;
			return false;
		}
		if (NumFoundIndices==1)
			this->PositiveKroots.AddObjectOnTopNoRepetitionOfObject(theWeyl.RootSystem.TheObjects[LastFoundIndex]);
	}
	this->nonLNonSingularRootsInNeedOfPosKroots.size=0;
	for (int i=0;i<this->nonLNonSingularRoots.size;i++)
		if (this->SumWithNoPosRootIsARoot(this->nonLNonSingularRoots.TheObjects[i],theWeyl))
		{ this->flagNeedsAdditionOfPositiveKroots=true;
      this->nonLNonSingularRootsInNeedOfPosKroots.AddObjectOnTopNoRepetitionOfObject(this->nonLNonSingularRoots.TheObjects[i]);
		}
	return true;
}

bool minimalRelationsProverState::ComputeStateReturnFalseIfDubious(minimalRelationsProverStates& owner, GlobalVariables& TheGlobalVariables,  WeylGroup& theWeyl, bool AssumeGlobalMinimalityRHS)
{ this->StateIsInternallyPossible=true;
  this->InternalStateIsComputed=true;
//  this->StateIsDubious=false;
  this->NilradicalRoots.AddRootSnoRepetition(this->PartialRelation.Betas);
  this->BKSingularGmodLRoots.AddRootSnoRepetition(this->PartialRelation.Alphas);
  this->PositiveKroots.AddRootSnoRepetition(this->ChosenPositiveKroots);
  SelectionWithMaxMultiplicity selBetas, selAlphas;
  selBetas.initMe2(this->PartialRelation.Betas.size,1);
  selAlphas.initMe2(this->PartialRelation.Alphas.size,1);
  int NumAlphas= MathRoutines::KToTheNth(selAlphas.MaxMultiplicity+1,this->PartialRelation.Alphas.size);
  int NumBetas=MathRoutines::KToTheNth(selBetas.MaxMultiplicity+1,this->PartialRelation.Betas.size);
//  this->ComputeDebugString(theWeyl, TheGlobalVariables);
  for (int i=0;i<NumAlphas;i++)
  {	for (int j=0;j<NumBetas;j++)
		{ if (this->CanBeShortened(this->PartialRelation, selAlphas, selBetas, theWeyl, AssumeGlobalMinimalityRHS))
      {	this->StateIsInternallyPossible=false;
        return false;
      }
      selBetas.IncrementSubset();
      this->MakeProgressReportCanBeShortened(i*NumBetas+j,NumAlphas*NumBetas,TheGlobalVariables);
    }
    selAlphas.IncrementSubset();
  }
//  this->ComputeDebugString(theWeyl, TheGlobalVariables);
	if (! this->ComputeCommonSenseImplicationsReturnFalseIfContradiction(theWeyl, TheGlobalVariables))
	{	this->StateIsInternallyPossible=false;
		return false;
	}
//  this->ComputeDebugString(theWeyl, TheGlobalVariables);
	roots tempRoots;
	tempRoots.AddListOnTop(this->PartialRelation.Alphas);
	tempRoots.AddListOnTop(this->PartialRelation.Betas);
	if (tempRoots.GetRankOfSpanOfElements(TheGlobalVariables)< this->PartialRelation.Betas.size+this->PartialRelation.Alphas.size)
	{	if (!roots::ConesIntersect(TheGlobalVariables, this->PartialRelation.Betas, this->PartialRelation.Alphas, theWeyl.KillingFormMatrix.NumRows))
		{	this->StateIsInternallyPossible=false;
			return false;
		}
		this->StateIsComplete=true;
		return true;
	}
 	roots possibleAlphas, possibleBetas;
	this->GetPossibleAlphasAndBetas(possibleAlphas, possibleBetas, theWeyl);
	if (!roots::ConesIntersect(TheGlobalVariables, possibleBetas, possibleAlphas, theWeyl.KillingFormMatrix.NumRows))
	{	this->StateIsInternallyPossible=false;
		return false;
	}
  this->ComputeDebugString(theWeyl, TheGlobalVariables);
  return true;
}

bool minimalRelationsProverStateFixedK::ComputeStateReturnFalseIfDubious( GlobalVariables& TheGlobalVariables,  WeylGroup& theWeyl, bool AssumeGlobalMinimalityRHS)
{ this->StateIsPossible=true;
//  this->StateIsDubious=false;
	for (int i=0;i<this->PartialRelation.Betas.size;i++)
		this->theNilradicalModules.AddSelectionAppendNewIndex(this->owner->GetModuleIndex(this->PartialRelation.Betas.TheObjects[i]));
  for (int i=0;i<this->PartialRelation.Alphas.size;i++)
		this->theGmodLmodules.AddSelectionAppendNewIndex(this->owner->GetModuleIndex(this->PartialRelation.Alphas.TheObjects[i]));
  SelectionWithMaxMultiplicity selBetas, selAlphas;
  selBetas.initMe2(this->PartialRelation.Betas.size,1);
  selAlphas.initMe2(this->PartialRelation.Alphas.size,1);
  int NumAlphas= MathRoutines::KToTheNth(selAlphas.MaxMultiplicity+1,this->PartialRelation.Alphas.size);
  int NumBetas=MathRoutines::KToTheNth(selBetas.MaxMultiplicity+1,this->PartialRelation.Betas.size);
//  this->ComputeDebugString(theWeyl, TheGlobalVariables);
  for (int i=0;i<NumAlphas;i++)
  {	for (int j=0;j<NumBetas;j++)
		{ if (this->CanBeShortened(this->PartialRelation, selAlphas,selBetas,theWeyl,AssumeGlobalMinimalityRHS))
      {	this->StateIsPossible=false;
        return false;
      }
      selBetas.IncrementSubset();
      this->MakeProgressReportCanBeShortened(i*NumBetas+j,NumAlphas*NumBetas,TheGlobalVariables);
    }
    selAlphas.IncrementSubset();
  }
//  this->ComputeDebugString(theWeyl, TheGlobalVariables);
	if (! this->ComputeCommonSenseImplicationsReturnFalseIfContradictionFixedK(theWeyl, TheGlobalVariables))
	{ this->StateIsPossible=false;
		return false;
	}
//  this->ComputeDebugString(theWeyl, TheGlobalVariables);
	//A relation consists always of minimally linearly dependent vectors, hence the below check.
	roots tempRoots;
	tempRoots.AddListOnTop(this->PartialRelation.Alphas);
	tempRoots.AddListOnTop(this->PartialRelation.Betas);
	if (tempRoots.GetRankOfSpanOfElements(TheGlobalVariables)< this->PartialRelation.Betas.size+this->PartialRelation.Alphas.size)
	{	if (!roots::ConesIntersect(TheGlobalVariables, this->PartialRelation.Betas, this->PartialRelation.Alphas, theWeyl.KillingFormMatrix.NumRows))
		{	this->StateIsPossible=false;
			return false;
		}
		this->StateIsComplete=true;
		return true;
	}

 	roots possibleAlphas, possibleBetas;
	this->GetPossibleAlphasAndBetas(possibleAlphas, possibleBetas, theWeyl);
	if (!roots::ConesIntersect(TheGlobalVariables, possibleBetas, possibleAlphas, theWeyl.KillingFormMatrix.NumRows))
	{	this->StateIsPossible=false;
		return false;
	}
  this->ComputeDebugString(theWeyl, TheGlobalVariables);
  return true;
}

void minimalRelationsProverState::MakeProgressReportCanBeShortened(int checked, int outOf, GlobalVariables &theGlobalVariables)
{ std::stringstream out5;
	out5<<checked+1 << " checked out of " << outOf;
	IndicatorWindowGlobalVariables.ProgressReportString5=out5.str();
	//::IndicatorWindowGlobalVariables.String1NeedsRefresh=false;
	//::IndicatorWindowGlobalVariables.String2NeedsRefresh=false;
	//::IndicatorWindowGlobalVariables.String3NeedsRefresh=false;
	//::IndicatorWindowGlobalVariables.String4NeedsRefresh=false;
	IndicatorWindowGlobalVariables.String5NeedsRefresh=true;
	IndicatorWindowGlobalVariables.StatusString1NeedsRefresh=false;
	if (theGlobalVariables.FeedDataToIndicatorWindowDefault!=0)
		theGlobalVariables.FeedDataToIndicatorWindowDefault(IndicatorWindowGlobalVariables);
}

void minimalRelationsProverStateFixedK::MakeProgressReportCanBeShortened(int checked, int outOf, GlobalVariables &theGlobalVariables)
{ std::stringstream out5;
	out5<<checked+1 << " checked out of " << outOf;
	IndicatorWindowGlobalVariables.ProgressReportString5=out5.str();
	//::IndicatorWindowGlobalVariables.String1NeedsRefresh=false;
	//::IndicatorWindowGlobalVariables.String2NeedsRefresh=false;
	//::IndicatorWindowGlobalVariables.String3NeedsRefresh=false;
	//::IndicatorWindowGlobalVariables.String4NeedsRefresh=false;
	IndicatorWindowGlobalVariables.String5NeedsRefresh=true;
	IndicatorWindowGlobalVariables.StatusString1NeedsRefresh=false;
	if (theGlobalVariables.FeedDataToIndicatorWindowDefault!=0)
		theGlobalVariables.FeedDataToIndicatorWindowDefault(IndicatorWindowGlobalVariables);
}

void minimalRelationsProverState::GetPossibleAlphasAndBetas(roots& outputAlphas, roots& outputBetas, WeylGroup& theWeyl)
{	outputBetas.size=0; outputAlphas.size=0;
	for (int i=0;i<theWeyl.RootSystem.size;i++)
	{ if (!this->nonBKSingularGmodLRoots.ContainsObject(theWeyl.RootSystem.TheObjects[i]))
			outputAlphas.AddObjectOnTop(theWeyl.RootSystem.TheObjects[i]);
		if (!this->nonBetas.ContainsObject(theWeyl.RootSystem.TheObjects[i]))
			outputBetas.AddObjectOnTop(theWeyl.RootSystem.TheObjects[i]);
	}
}

void minimalRelationsProverStateFixedK::GetPossibleAlphasAndBetas(roots& outputAlphas, roots& outputBetas, WeylGroup& theWeyl)
{	outputBetas.size=0; outputAlphas.size=0;
	for (int i=0; i<this->owner->theK.kModules.size; i++)
	{	if (!this->theNilradicalModules.selected[i])
			outputAlphas.AddObjectOnTop(this->owner->theK.HighestWeightsGmodK.TheObjects[i]);
		if (!this->theGmodLmodules.selected[i])
			outputBetas.AddListOnTop(this->owner->theK.kModules.TheObjects[i]);
	}
}

bool minimalRelationsProverState::IsSeparatingCones(root& input, bool& oneBetaIsPositive, WeylGroup& theWeyl)
{	return minimalRelationsProverStateFixedK::IsSeparatingCones(input, this->PartialRelation.Alphas, this->PartialRelation.Betas, oneBetaIsPositive, theWeyl);
}

bool minimalRelationsProverStateFixedK::IsSeparatingCones(root& input, bool& oneBetaIsPositive, WeylGroup& theWeyl)
{	return this->IsSeparatingCones(input, this->PartialRelation.Alphas, this->PartialRelation.Betas, oneBetaIsPositive, theWeyl);
}

bool minimalRelationsProverStateFixedK::IsSeparatingCones( root& input, roots& theAlphas, roots& theBetas, bool& oneBetaIsPositive, WeylGroup& theWeyl)
{	bool foundPosBeta=false;
	bool foundNegBeta=false;
	bool foundPosAlpha=false;
	bool foundNegAlpha=false;
	Rational tempRat;
	for (int i=0;i<theBetas.size;i++)
	{	tempRat=theWeyl.RootScalarKillingFormMatrixRoot(theBetas.TheObjects[i],input);
		if (tempRat.IsPositive())
		{	if (foundNegBeta)
				return false;
			foundPosBeta=true;
		}
		if (tempRat.IsNegative())
		{	if (foundPosBeta)
				return false;
			foundNegBeta=true;
		}
	}
	for (int i=0;i<theAlphas.size;i++)
	{	tempRat=theWeyl.RootScalarKillingFormMatrixRoot(theAlphas.TheObjects[i],input);
		if (tempRat.IsPositive())
		{	if (foundNegAlpha || foundPosBeta)
				return false;
			foundPosAlpha=true;
		}
		if (tempRat.IsNegative())
		{	if (foundPosAlpha || foundNegBeta)
				return false;
			foundNegAlpha=true;
		}
	}
	if (!(foundPosBeta|| foundNegBeta || foundPosAlpha || foundNegAlpha))
		return false;
	oneBetaIsPositive=foundPosBeta;
	return true;
}

void minimalRelationsProverStateFixedK::WriteToFile(std::fstream &output, GlobalVariables &theGlobalVariables)
{ output <<"\n\n\n\nNum_isos: " << this->indicesIsosRespectingInitialNilradicalChoice.size<<"\n";
	for (int i=0;i<this->indicesIsosRespectingInitialNilradicalChoice.size;i++)
		output <<this->indicesIsosRespectingInitialNilradicalChoice.TheObjects[i]<<" ";
	output << "\nAlphas: ";
	this->PartialRelation.Alphas.WriteToFile(output, theGlobalVariables);
	output << "Betas: ";
	this->PartialRelation.Betas.WriteToFile(output, theGlobalVariables);
	output<<"\nSeparating_normal: ";
	this->SeparatingNormalUsed.WriteToFile(output);
	output <<"Num_possible_children: " << this->PossibleChildStates.size<<"\n";
	output <<"Active_child: " << this->activeChild<<"\n";
	output <<"Children: ";
	for (int i=0;i<this->PossibleChildStates.size;i++)
		output <<this->PossibleChildStates.TheObjects[i]<<" ";
	output<<"\nthe_choices_we_make: ";
	this->theChoicesWeMake.WriteToFile(output, theGlobalVariables);
	output<<"\nNon_L_modules: ";
	this->theGmodLmodules.WriteToFile(output);
	output<<"\nN_modules: ";
	this->theNilradicalModules.WriteToFile(output);
	output<<"\nNon-alphas: ";
	this->nonAlphas.WriteToFile(output, theGlobalVariables);
	output<<"\nNon-betas: ";
	this->nonBetas.WriteToFile(output, theGlobalVariables);
}

void ::minimalRelationsProverStateFixedK::ReadFromFile(	std::fstream &input, GlobalVariables&  theGlobalVariables)
{	std::string tempS;
	int tempI;
	input>>tempS >> tempI;	assert(tempS=="Num_isos:");
	this->indicesIsosRespectingInitialNilradicalChoice.SetSizeExpandOnTopNoObjectInit(tempI);
	for (int i=0;i<this->indicesIsosRespectingInitialNilradicalChoice.size;i++)
		input>>this->indicesIsosRespectingInitialNilradicalChoice.TheObjects[i];
	input >>tempS;
	this->PartialRelation.Alphas.ReadFromFile(input, theGlobalVariables);
	input >> tempS;
	this->PartialRelation.Betas.ReadFromFile(input, theGlobalVariables);
	input>>tempS;
	this->SeparatingNormalUsed.ReadFromFile(input);
	input >>tempS >> tempI;
	this->PossibleChildStates.SetSizeExpandOnTopNoObjectInit(tempI);
	input >>tempS >> this->activeChild;
	input >>tempS;
	for (int i=0;i<this->PossibleChildStates.size;i++)
		input >>this->PossibleChildStates.TheObjects[i];
	input>>tempS;
	this->theChoicesWeMake.ReadFromFile(input, theGlobalVariables);
	input>>tempS;
	this->theGmodLmodules.ReadFromFile(input);
	input>>tempS;
	this->theNilradicalModules.ReadFromFile(input);
	input>>tempS;
	this->nonAlphas.ReadFromFile(input, theGlobalVariables);
	input>>tempS;
	this->nonBetas.ReadFromFile(input, theGlobalVariables);
	assert(tempS=="Non-betas:");
}

void minimalRelationsProverState::WriteToFile(std::fstream& output, GlobalVariables&  theGlobalVariables)
{ output<< "\nAlphas: ";
	this->PartialRelation.Alphas.WriteToFile(output, theGlobalVariables);
	output<<"\nBetas: ";
	this->PartialRelation.Betas.WriteToFile(output, theGlobalVariables);
	output<<"\nActive_child(non-updated_see_header_for_true_data): " << this->activeChild <<" "<< this->PossibleChildStates.size<<" ";
	for (int i=0; i<this->PossibleChildStates.size; i++)
		output<< this->PossibleChildStates.TheObjects[i]<<" ";
	output<<"\nThe_choices_we_make: ";
	this->theChoicesWeMake.WriteToFile(output, theGlobalVariables);
	output<<"\nChosen_positive_Kroots: ";
	this->ChosenPositiveKroots.WriteToFile(output, theGlobalVariables);
	output<<"\nState_possible: "<<this->StateIsInternallyPossible;
	/*output<<"\nBk_singular: ";
	this->BKSingularGmodLRoots.WriteToFile(output, theGlobalVariables);
	output<<"\nChosen_normal_eps_form: ";
	this->currentSeparatingNormalEpsilonForm.WriteToFile(output);
	output<<"\nNeedsPosKroots: "<<this->flagNeedsAdditionOfPositiveKroots;
	output<<"\nNilradical_roots: ";
	this->NilradicalRoots.WriteToFile(output, theGlobalVariables);
	output<<"\nNon-alphas: ";
	this->nonAlphas.WriteToFile(output, theGlobalVariables);
	output<<"\nNon-betas: ";
	this->nonBetas.WriteToFile(output, theGlobalVariables);
	output<<"\nNonBK_singularGmodLroots: ";
	this->nonBKSingularGmodLRoots.WriteToFile(output, theGlobalVariables);
	output<<"\nNon_Kroots: ";
	this->nonKRoots.WriteToFile(output, theGlobalVariables);
	output<<"\nNon_L_non_singular: ";
	this->nonLNonSingularRoots.WriteToFile(output, theGlobalVariables);
	output<<"\nNon_L_non_singular_need_posK: ";
	this->nonLNonSingularRootsInNeedOfPosKroots.WriteToFile(output, theGlobalVariables);
	output<<"\nNon_L_aleviated: ";
	this->nonLNonSingularsAleviatedByChosenPosKRoots.WriteToFile(output, theGlobalVariables);
	output<<"\nNon_L_roots: ";
	this->nonLRoots.WriteToFile(output, theGlobalVariables);
	output<<"\nNon_nilradical_roots: ";
	this->nonNilradicalRoots.WriteToFile(output, theGlobalVariables);
	output<<"\nNon_PosKroots: ";
	this->nonPositiveKRoots.WriteToFile(output, theGlobalVariables);
	output<<"\nPosKroots: ";
	this->PositiveKroots.WriteToFile(output, theGlobalVariables);*/
}

void minimalRelationsProverState::ReadFromFile(std::fstream& input, GlobalVariables&  theGlobalVariables)
{	std::string tempS; int tempI;
	input>> tempS;
	this->PartialRelation.Alphas.ReadFromFile(input, theGlobalVariables);
	input>> tempS;
	this->PartialRelation.Betas.ReadFromFile(input, theGlobalVariables);
	input>> tempS >> this->activeChild>>tempI;
	this->PossibleChildStates.SetSizeExpandOnTopNoObjectInit(tempI);
	for (int i=0; i<this->PossibleChildStates.size; i++)
		input>> this->PossibleChildStates.TheObjects[i];
	input>> tempS;
	this->theChoicesWeMake.ReadFromFile(input, theGlobalVariables);
	input>> tempS;
	this->ChosenPositiveKroots.ReadFromFile(input, theGlobalVariables);
	input>> tempS>>this->StateIsInternallyPossible;
	assert(tempS=="State_possible:");
/*
	input>> tempS;
	this->BKSingularGmodLRoots.ReadFromFile(input, theGlobalVariables);
	input>> tempS;
	this->currentSeparatingNormalEpsilonForm.ReadFromFile(input);
	input>> tempS>>this->flagNeedsAdditionOfPositiveKroots;
	input>> tempS;
	this->NilradicalRoots.ReadFromFile(input, theGlobalVariables);
	input>> tempS;
	this->nonAlphas.ReadFromFile(input, theGlobalVariables);
	input>> tempS;
	this->nonBetas.ReadFromFile(input, theGlobalVariables);
	input>> tempS;
	this->nonBKSingularGmodLRoots.ReadFromFile(input, theGlobalVariables);
	input>> tempS;
	this->nonKRoots.ReadFromFile(input, theGlobalVariables);
	input>> tempS;
	this->nonLNonSingularRoots.ReadFromFile(input, theGlobalVariables);
	input>> tempS;
	this->nonLNonSingularRootsInNeedOfPosKroots.ReadFromFile(input, theGlobalVariables);
	input>> tempS;
	this->nonLNonSingularsAleviatedByChosenPosKRoots.ReadFromFile(input, theGlobalVariables);
	input>> tempS;
	this->nonLRoots.ReadFromFile(input, theGlobalVariables);
	input>> tempS;
	this->nonNilradicalRoots.ReadFromFile(input, theGlobalVariables);
	input>> tempS;
	this->nonPositiveKRoots.ReadFromFile(input, theGlobalVariables);
	input>> tempS;
	this->PositiveKroots.ReadFromFile(input, theGlobalVariables);*/
}

void minimalRelationsProverStateFixedK::GetCertainGmodLhighestAndNilradicalRoots(roots& outputAGmodLhighest, roots& outputNilradicalRoots, WeylGroup& theWeyl)
{	outputAGmodLhighest.size=0; outputNilradicalRoots.size=0;
	for (int i=0; i<this->owner->theK.kModules.size; i++)
	{	if (this->theNilradicalModules.selected[i])
			outputNilradicalRoots.AddListOnTop(this->owner->theK.kModules.TheObjects[i]);
		if (this->theGmodLmodules.selected[i])
			outputAGmodLhighest.AddObjectOnTop(this->owner->theK.HighestWeightsGmodK.TheObjects[i]);
	}
}

void ::minimalRelationsProverStatesFixedK::WriteToFile(std::fstream& output, GlobalVariables& theGlobalVariables)
{ output <<"Weyl_letter: " << this->theWeylGroup.WeylLetter << " dim: "<< this->theWeylGroup.KillingFormMatrix.NumRows<<"\n";
	output<<"Simple_basis_K: ";
	//this->theK.SimpleBasisK.ComputeDebugString();
	this->theK.SimpleBasisK.WriteToFile(output, theGlobalVariables);
	this->theIsos.WriteToFile(output, theGlobalVariables);
	output<<"\nState_stack_size: "<< this->theIndexStack.size<<" ";
	for (int i=0;i<this->theIndexStack.size;i++)
		output << this->theIndexStack.TheObjects[i]<<" ";
	output<<"\nNum_states: "<< this->size<<"\n";
	for (int i=0;i<this->size;i++)
		this->TheObjects[i].WriteToFile(output, theGlobalVariables);
	this->theK.WriteMultTableAndOppositeKmodsToFile(output, this->theK.theMultTable, this->theK.theOppositeKmods);
}

void ::minimalRelationsProverStatesFixedK::ReadFromFile(std::fstream& input, GlobalVariables& theGlobalVariables)
{	std::string tempS;
	int tempI;
	input >> tempS>> this->theWeylGroup.WeylLetter >> tempS>> tempI;
	if (tempI<=0)
		return;
	this->theWeylGroup.MakeArbitrary(this->theWeylGroup.WeylLetter, tempI);
	input>> tempS;
	this->theK.genK.ReadFromFile(input, theGlobalVariables);
	this->theK.genK.ComputeDebugString();
	this->theIsos.ReadFromFile(input, theGlobalVariables);
	input>>tempS>>tempI;
	this->theIndexStack.SetSizeExpandOnTopNoObjectInit(tempI);
	for (int i=0;i<this->theIndexStack.size;i++)
		input>>this->theIndexStack.TheObjects[i];
	input>> tempS>> tempI;
	this->SetSizeExpandOnTopNoObjectInit(tempI);
	for(int i=0; i<this->size; i++)
	{	this->TheObjects[i].ReadFromFile(input, theGlobalVariables);
		this->TheObjects[i].owner=this;
	}
	this->theK.ReadMultTableAndOppositeKmodsToFile(input, this->theK.theMultTable, this->theK.theOppositeKmods);
	this->theWeylGroup.ComputeRho(true);
	this->initShared(this->theWeylGroup, theGlobalVariables);
	this->theIsos.ComputeSubGroupFromGeneratingReflections(this->theIsos.simpleGenerators,this->theIsos.ExternalAutomorphisms	,theGlobalVariables, -1, false);
	this->theK.ComputeAll();
	this->flagComputationIsInitialized=true;
	if (theGlobalVariables.FeedDataToIndicatorWindowDefault!=0)
		if (this->theIndexStack.size>0)
		{ int tempI= *this->theIndexStack.LastObject();
			this->TheObjects[tempI].ComputeDebugString(this->theWeylGroup, theGlobalVariables);
			IndicatorWindowGlobalVariables.StatusString1=this->TheObjects[tempI].DebugString;
			IndicatorWindowGlobalVariables.StatusString1NeedsRefresh=true;
			theGlobalVariables.FeedDataToIndicatorWindowDefault(::IndicatorWindowGlobalVariables);
		}
	//this->theK.GenerateKmodMultTable(this->theK.theMultTable, this->theK.theOppositeKmods, theGlobalVariables);
}

void minimalRelationsProverStatesFixedK::WriteToFile(std::string& fileName, GlobalVariables&  theGlobalVariables)
{ CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(this->theFile, fileName, false, true, false);
	this->WriteToFile(this->theFile,theGlobalVariables);
	this->theFile.close();
}

void minimalRelationsProverStatesFixedK::ReadFromFile(std::string& fileName, GlobalVariables&  theGlobalVariables)
{	if(!rootFKFTcomputation::OpenDataFile(this->theFile, fileName))
		return;
	this->ReadFromFile(this->theFile, theGlobalVariables);
	this->theFile.close();
}

void minimalRelationsProverStates::WriteToFileAppend( GlobalVariables&  theGlobalVariables)
{ if ( this->size-this->sizeByLastPurge >200)
  { this->sizeByLastSave=0;
    this->PurgeImpossibleStates();
  }
  if (this->sizeByLastSave!=0)
		CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(this->theFileBody, this->FileBodyString, true, false, false);
	else
		CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(this->theFileBody, this->FileBodyString, false, true, false);
	CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(this->theFileHeader, this->FileHeaderString, false, true, false);
	assert(this->theFileHeader.is_open());
	this->WriteToFileAppend(this->theFileHeader, this->theFileBody, theGlobalVariables);
	this->theFileHeader.close();
	this->theFileBody.close();
}

void minimalRelationsProverStates::ReadFromFile(GlobalVariables&  theGlobalVariables)
{ if(!rootFKFTcomputation::OpenDataFile(this->theFileBody, this->FileBodyString))
		return;
	if(!rootFKFTcomputation::OpenDataFile(this->theFileHeader, this->FileHeaderString))
		return;
	this->ReadFromFile(this->theFileHeader, this->theFileBody, theGlobalVariables);
	this->theFileHeader.close();
	this->theFileBody.close();
}

void minimalRelationsProverStates::WriteToFileAppend(std::fstream& outputHeader, std::fstream& outputBody, GlobalVariables &theGlobalVariables)
{ outputHeader <<"Weyl_letter: " << this->theWeylGroup.WeylLetter << " dim: "<< this->theWeylGroup.KillingFormMatrix.NumRows<<"\n" <<"\nNum_states: "<<this->size;
	outputHeader<<"\nState_stack_size: "<< this->theIndexStack.size<<" ";
	for (int i=0; i<this->theIndexStack.size; i++)
		outputHeader << this->theIndexStack.TheObjects[i]<<" ";
	outputHeader<<"\nActive_child_list: ";
	for (int i=0;i<this->size; i++)
		outputHeader<<this->TheObjects[i].activeChild<<" ";
	outputHeader<< "\nTree_structure_excluding_complete_states: ";
	for(int i=0; i<this->size; i++)
	{ outputHeader<<"\nChild_index "<<i<<" size: "<< this->TheObjects[i].PossibleChildStates.size<<" ";
		for (int j=0; j<this->TheObjects[i].PossibleChildStates.size; j++)
			outputHeader<< this->TheObjects[i].PossibleChildStates.TheObjects[j]<<" ";
	}
	outputHeader<< "\nTree_structure_complete_states: ";
	for(int i=0; i<this->size; i++)
	{ outputHeader<<"\nChild_index "<<i<<" size: "<< this->TheObjects[i].CompleteChildStates.size<<" ";
		for (int j=0; j<this->TheObjects[i].CompleteChildStates.size; j++)
			outputHeader<< this->TheObjects[i].CompleteChildStates.TheObjects[j]<<" ";
    outputHeader<<"Num_impossible: "<< this->TheObjects[i].NumImpossibleChildren<<" ";
	}
	for (int i=this->sizeByLastSave; i<this->size; i++)
	{ outputBody<<"\n\n\nState_index: "<< i<<" \n";
		this->TheObjects[i].WriteToFile(outputBody, theGlobalVariables);
		if (theGlobalVariables.FeedDataToIndicatorWindowDefault!=0)
		{ std::stringstream out3;
			out3<< i+1<<" out of "<< this->size<< " states stored to disk";
			IndicatorWindowGlobalVariables.ProgressReportString3= out3.str();
			IndicatorWindowGlobalVariables.String3NeedsRefresh=true;
			theGlobalVariables.FeedDataToIndicatorWindowDefault(IndicatorWindowGlobalVariables);
		}
	}
	this->sizeByLastSave=this->size;
}

void minimalRelationsProverStates::ReadFromFile(std::fstream &inputHeader, std::fstream &inputBody, GlobalVariables &theGlobalVariables)
{ std::string tempS; int tempI;
	inputHeader >>tempS >> this->theWeylGroup.WeylLetter >>tempS>> tempI;
	this->theWeylGroup.MakeArbitrary(this->theWeylGroup.WeylLetter, tempI);
	inputHeader>>tempS>>tempI;
	this->SetSizeExpandOnTopNoObjectInit(tempI);
	inputHeader >>tempS>> tempI;
	this->theIndexStack.SetSizeExpandOnTopNoObjectInit(tempI);
	for (int i=0; i<this->theIndexStack.size; i++)
		inputHeader>>this->theIndexStack.TheObjects[i];
	ListBasicObjects<int> theActiveChildren;
	ListBasicObjects< ListBasicObjects<int> > thePossibleChildStates, theCompleteChildStates;
	theActiveChildren.SetSizeExpandOnTopNoObjectInit(this->size);
	thePossibleChildStates.SetSizeExpandOnTopNoObjectInit(this->size);
	theCompleteChildStates.SetSizeExpandOnTopNoObjectInit(this->size);
	inputHeader>>tempS;
	assert(tempS=="Active_child_list:");
	for (int i=0;i<this->size;i++)
		inputHeader>>theActiveChildren.TheObjects[i];
	inputHeader>> tempS;
	for(int i=0; i<this->size; i++)
	{ inputHeader>>tempS>>tempI>> tempS>>tempI;
		assert(tempS=="size:");
		thePossibleChildStates.TheObjects[i].SetSizeExpandOnTopNoObjectInit(tempI);
		for (int j=0; j<tempI; j++)
			inputHeader>> thePossibleChildStates.TheObjects[i].TheObjects[j];
	}
	inputHeader>> tempS;
	assert(tempS=="Tree_structure_complete_states:");
	for(int i=0; i<this->size; i++)
	{ inputHeader>>tempS>>tempI>> tempS>>tempI;
		assert(tempS=="size:");
		theCompleteChildStates.TheObjects[i].SetSizeExpandOnTopNoObjectInit(tempI);
		for (int j=0; j<tempI; j++)
			inputHeader>> theCompleteChildStates.TheObjects[i].TheObjects[j];
    inputHeader>>tempS>>this->TheObjects[i].NumImpossibleChildren;
    assert(tempS=="Num_impossible:");
	}
	for (int i=0; i<this->size; i++)
	{ inputBody>> tempS>>tempI;
		assert(tempI==i);
		this->TheObjects[i].ReadFromFile(inputBody, theGlobalVariables);
		this->TheObjects[i].owner=this;
		this->TheObjects[i].activeChild=theActiveChildren.TheObjects[i];
		this->TheObjects[i].PossibleChildStates.CopyFromBase(thePossibleChildStates.TheObjects[i]);
		this->TheObjects[i].CompleteChildStates.CopyFromBase(theCompleteChildStates.TheObjects[i]);
		if (theGlobalVariables.FeedDataToIndicatorWindowDefault!=0)
		{ std::stringstream out3;
      if (i+1==12361)
      { StopDebug();
      }
			out3<< i+1<<" out of "<< this->size<< " states read from disk";
			IndicatorWindowGlobalVariables.ProgressReportString3= out3.str();
			IndicatorWindowGlobalVariables.StatusString1NeedsRefresh=false;
			IndicatorWindowGlobalVariables.String3NeedsRefresh=true;
			theGlobalVariables.FeedDataToIndicatorWindowDefault(IndicatorWindowGlobalVariables);
		}
	}
	IndicatorWindowGlobalVariables.StatusString1NeedsRefresh=true;
	this->initShared(this->theWeylGroup, theGlobalVariables);
	if (this->theIndexStack.size>0)
	{ int currentI=*this->theIndexStack.LastObject();
		this->TheObjects[currentI].ComputeDebugString(this->theWeylGroup, theGlobalVariables);
		this->MakeProgressReportCurrentState(currentI, theGlobalVariables, this->theWeylGroup);
		this->MakeProgressReportStack(theGlobalVariables, this->theWeylGroup);
	}
	this->sizeByLastSave=this->size;
}

void minimalRelationsProverState::ReduceMemoryUse()
{ if (!this->InternalStateIsComputed)
    return;
  this->InternalStateIsComputed=false;
  this->theScalarProducts.ReleaseMemory();
	this->BKSingularGmodLRoots.ReleaseMemory();
  this->nonBKSingularGmodLRoots.ReleaseMemory();
  this->PositiveKroots.ReleaseMemory();
	this->nonPositiveKRoots.ReleaseMemory();
	this->NilradicalRoots.ReleaseMemory();
	this->nonNilradicalRoots.ReleaseMemory();
	this->nonKRoots.ReleaseMemory();
  this->nonAlphas.ReleaseMemory();
  this->nonBetas.ReleaseMemory();
  this->nonLRoots.ReleaseMemory();
  this->nonLNonSingularsAleviatedByChosenPosKRoots.ReleaseMemory();
  this->nonLNonSingularRoots.ReleaseMemory();
  this->nonLNonSingularRootsInNeedOfPosKroots.ReleaseMemory();
}

void minimalRelationsProverState::ComputeIsPossible(minimalRelationsProverStates& theOwner)
{ if (this->CompleteChildStates.size>0)
  { this->StateIsPossible=true;
    return;
  }
  this->StateIsPossible=false;
  for (int i=0;i<this->PossibleChildStates.size;i++)
    if (theOwner.TheObjects[this->PossibleChildStates.TheObjects[i]].StateIsPossible)
    { this->StateIsPossible=true;
      return;
    }
}

void minimalRelationsProverState::Assign(const minimalRelationsProverState& right)
{ this->initFromParent(right);
  this->StateIsPossible=right.StateIsPossible;
  this->theScalarProducts.Assign(right.theScalarProducts);
  this->currentSeparatingNormalEpsilonForm.Assign(right.currentSeparatingNormalEpsilonForm);
  this->PossibleChildStates= right.PossibleChildStates;
  this->CompleteChildStates= right.CompleteChildStates;
  this->NumImpossibleChildren=right.NumImpossibleChildren;
  this->activeChild=right.activeChild;
}

void minimalRelationsProverState::initFromParent(const minimalRelationsProverState& right)
{ this->PartialRelation=right.PartialRelation;
	this->BKSingularGmodLRoots.CopyFromBase(right.BKSingularGmodLRoots);
  this->nonBKSingularGmodLRoots.CopyFromBase(right.nonBKSingularGmodLRoots);
  this->PositiveKroots.CopyFromBase(right.PositiveKroots);
	this->nonPositiveKRoots.CopyFromBase(right.nonPositiveKRoots);
	this->NilradicalRoots.CopyFromBase(right.NilradicalRoots);
	this->nonNilradicalRoots.CopyFromBase(right.nonNilradicalRoots);
	this->nonKRoots.CopyFromBase(right.nonKRoots);
  this->nonAlphas.CopyFromBase(right.nonAlphas);
  this->nonBetas.CopyFromBase(right.nonBetas);
  this->owner=right.owner;
  this->StateIsInternallyPossible=right.StateIsInternallyPossible;
  this->StateIsComplete=right.StateIsComplete;
  this->ChosenPositiveKroots=right.ChosenPositiveKroots;
  this->theChoicesWeMake.CopyFromBase(right.theChoicesWeMake);
  this->nonLRoots.CopyFromBase(right.nonLRoots);
  this->nonLNonSingularsAleviatedByChosenPosKRoots=right.nonLNonSingularsAleviatedByChosenPosKRoots;
  this->flagNeedsAdditionOfPositiveKroots=right.flagNeedsAdditionOfPositiveKroots;
  this->nonLNonSingularRoots=right.nonLNonSingularRoots;
  this->nonLNonSingularRootsInNeedOfPosKroots=right.nonLNonSingularRootsInNeedOfPosKroots;
}

void minimalRelationsProverStateFixedK::Assign(const minimalRelationsProverStateFixedK& right)
{ this->theNilradicalModules.Assign(right.theNilradicalModules);
  this->theGmodLmodules.Assign(right.theGmodLmodules);
  this->owner=right.owner;
  this->nonAlphas.CopyFromBase(right.nonAlphas);
  this->nonBetas.CopyFromBase(right.nonBetas);
  this->PartialRelation.Alphas.CopyFromBase(right.PartialRelation.Alphas);
  this->PartialRelation.Betas.CopyFromBase(right.PartialRelation.Betas);
  this->StateIsPossible=right.StateIsPossible;
  this->StateIsComplete=right.StateIsComplete;
  this->theScalarProducts.Assign(right.theScalarProducts);
  this->theChoicesWeMake.CopyFromBase(right.theChoicesWeMake);
  this->currentSeparatingNormalEpsilonForm.Assign(right.currentSeparatingNormalEpsilonForm);
  this->flagNeedsAdditionOfPositiveKroots=right.flagNeedsAdditionOfPositiveKroots;
  this->ImpossibleChildStates= right.ImpossibleChildStates;
  this->PossibleChildStates.CopyFromBase( right.PossibleChildStates);
  this->CompleteChildStates= right.CompleteChildStates;
  this->activeChild=right.activeChild;
  this->indicesIsosRespectingInitialNilradicalChoice.CopyFromBase(right.indicesIsosRespectingInitialNilradicalChoice);
  this->SeparatingNormalUsed.Assign(right.SeparatingNormalUsed);
}

void minimalRelationsProverStateFixedK::initFromParentState(minimalRelationsProverStateFixedK& parent)
{ this->theNilradicalModules.Assign(parent.theNilradicalModules);
  this->theGmodLmodules.Assign(parent.theGmodLmodules);
  this->owner=parent.owner;
  this->nonAlphas.CopyFromBase(parent.nonAlphas);
  this->nonBetas.CopyFromBase(parent.nonBetas);
  this->PartialRelation=parent.PartialRelation;
  this->StateIsPossible=parent.StateIsPossible;
  this->StateIsComplete=parent.StateIsComplete;
  this->theScalarProducts.Assign(parent.theScalarProducts);
  this->theChoicesWeMake.CopyFromBase(parent.theChoicesWeMake);
  this->currentSeparatingNormalEpsilonForm.MakeZero(0);
  this->flagNeedsAdditionOfPositiveKroots=false;
  this->ImpossibleChildStates.size=0;
  this->PossibleChildStates.size=0;
  this->CompleteChildStates.size=0;
  this->activeChild=-1;
  this->indicesIsosRespectingInitialNilradicalChoice.CopyFromBase(parent.indicesIsosRespectingInitialNilradicalChoice);
  this->SeparatingNormalUsed.MakeZero(0);
}

void minimalRelationsProverStates::MakeProgressReportCurrentState(	int index, GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl)
{ std::stringstream out; std::string tempS;
  out << this->TheObjects[index].DebugString;
  out<<"\r\nPreferred dual basis: "<<this->PreferredDualBasisEpsilonCoords.DebugString;
  IndicatorWindowGlobalVariables.StatusString1=out.str();
	IndicatorWindowGlobalVariables.StatusString1NeedsRefresh=true;
	if (TheGlobalVariables.FeedDataToIndicatorWindowDefault!=0)
		TheGlobalVariables.FeedDataToIndicatorWindowDefault(IndicatorWindowGlobalVariables);
	//::IndicatorWindowGlobalVariables.StatusString1NeedsRefresh=false;
}

void minimalRelationsProverStatesFixedK::MakeProgressReportCurrentState(	int index, GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl)
{ std::stringstream out; std::string tempS;
  out << this->TheObjects[index].DebugString;
  out<<"\r\nPreferred dual basis: "<<this->PreferredDualBasisEpsilonCoords.DebugString;
  IndicatorWindowGlobalVariables.StatusString1=out.str();
	IndicatorWindowGlobalVariables.StatusString1NeedsRefresh=true;
	if (TheGlobalVariables.FeedDataToIndicatorWindowDefault!=0)
		TheGlobalVariables.FeedDataToIndicatorWindowDefault(IndicatorWindowGlobalVariables);
	//::IndicatorWindowGlobalVariables.StatusString1NeedsRefresh=false;
}

void minimalRelationsProverStates::MakeProgressReportIsos(int progress, int numSearchedWithinState, int outOf, GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl)
{ std::stringstream out3;
	out3 <<"Searching for automorphisms: " << progress+1 << " from "<< this->size << " states; "<< numSearchedWithinState+1<<" out of "<< outOf <<" possibilities within current state";
	IndicatorWindowGlobalVariables.String3NeedsRefresh=true;
	//::IndicatorWindowGlobalVariables.String1NeedsRefresh=false;
	//::IndicatorWindowGlobalVariables.String2NeedsRefresh=false;
	//::IndicatorWindowGlobalVariables.String4NeedsRefresh=false;
	//::IndicatorWindowGlobalVariables.String5NeedsRefresh=false;
	IndicatorWindowGlobalVariables.ProgressReportString3=out3.str();
	if (TheGlobalVariables.FeedDataToIndicatorWindowDefault!=0)
		TheGlobalVariables.FeedDataToIndicatorWindowDefault(IndicatorWindowGlobalVariables);
}

void minimalRelationsProverStatesFixedK::MakeProgressReportIsos(int progress, int numSearchedWithinState, int outOf, GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl)
{ std::stringstream out3;
	out3 <<"Searching for automorphisms: " << progress+1 << " from "<< this->size << " states; "<< numSearchedWithinState+1<<" out of "<< outOf <<" possibilities within current state";
	IndicatorWindowGlobalVariables.String3NeedsRefresh=true;
	//::IndicatorWindowGlobalVariables.String1NeedsRefresh=false;
	//::IndicatorWindowGlobalVariables.String2NeedsRefresh=false;
	//::IndicatorWindowGlobalVariables.String4NeedsRefresh=false;
	//::IndicatorWindowGlobalVariables.String5NeedsRefresh=false;
	//::IndicatorWindowGlobalVariables.ProgressReportString3=out3.str();
	if (TheGlobalVariables.FeedDataToIndicatorWindowDefault!=0)
		TheGlobalVariables.FeedDataToIndicatorWindowDefault(IndicatorWindowGlobalVariables);
}

void minimalRelationsProverStates::MakeProgressReportChildStates(int numSearched, int outOf, int NewFound, GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl)
{ this->MakeProgressReportStack(TheGlobalVariables, theWeyl);
	std::stringstream out4;
	out4<<"Computing children states: "<< numSearched+1<<" out of "<< outOf << "; "
          << this->TheObjects[*this->theIndexStack.LastObject()].PossibleChildStates.size<<" possible ";
	::IndicatorWindowGlobalVariables.String4NeedsRefresh=true;
//	::IndicatorWindowGlobalVariables.String1NeedsRefresh=false;
//	::IndicatorWindowGlobalVariables.String2NeedsRefresh=false;
//	::IndicatorWindowGlobalVariables.String3NeedsRefresh=false;
//	::IndicatorWindowGlobalVariables.String5NeedsRefresh=false;
	IndicatorWindowGlobalVariables.ProgressReportString4=out4.str();
	if (TheGlobalVariables.FeedDataToIndicatorWindowDefault!=0)
		TheGlobalVariables.FeedDataToIndicatorWindowDefault(IndicatorWindowGlobalVariables);
}

void minimalRelationsProverStatesFixedK::MakeProgressReportChildStates(int numSearched, int outOf, int NewFound, GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl)
{ this->MakeProgressReportStack(TheGlobalVariables, theWeyl);
	std::stringstream out4;
	out4<<"Computing children states: "<< numSearched+1<<" out of "<< outOf << "; "
        << this->TheObjects[*this->theIndexStack.LastObject()].PossibleChildStates.size<<" possible, "
				<< this->TheObjects[*this->theIndexStack.LastObject()].ImpossibleChildStates.size<<" impossible ";
	IndicatorWindowGlobalVariables.String4NeedsRefresh=true;
//	::IndicatorWindowGlobalVariables.String1NeedsRefresh=false;
//	::IndicatorWindowGlobalVariables.String2NeedsRefresh=false;
//	::IndicatorWindowGlobalVariables.String3NeedsRefresh=false;
//	::IndicatorWindowGlobalVariables.String5NeedsRefresh=false;
	IndicatorWindowGlobalVariables.ProgressReportString4=out4.str();
	if (TheGlobalVariables.FeedDataToIndicatorWindowDefault!=0)
		TheGlobalVariables.FeedDataToIndicatorWindowDefault(IndicatorWindowGlobalVariables);
}
void minimalRelationsProverStates::MakeProgressReportStack( GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl)
{ std::stringstream out1, out2, out3, out4;
	std::stringstream* tempOut1=&out1;
	for (int i=0;i<this->theIndexStack.size;i++)
	{ int currentIndex=this->theIndexStack.TheObjects[i];
	  (*tempOut1)	<< currentIndex << ": " << this->TheObjects[currentIndex].activeChild+1<< " of "<< this->TheObjects[currentIndex].PossibleChildStates.size	<<"; ";
		if (i==4)	tempOut1=&out2;
		if (i==8)	tempOut1=&out3;
	//	if (i==12)	tempOut1=&out4;
	}
	if (theIndexStack.size!=0)
    (*tempOut1)<< this->TheObjects[*this->theIndexStack.LastObject()].PartialRelation.Alphas.size<<" alphas + ... = "<<this->TheObjects[*this->theIndexStack.LastObject()].PartialRelation.Betas.size<<" betas + ...";
	IndicatorWindowGlobalVariables.ProgressReportString1=out1.str();
	IndicatorWindowGlobalVariables.ProgressReportString2=out2.str();
	if (this->theIndexStack.size>8)
		IndicatorWindowGlobalVariables.ProgressReportString3=out3.str();
	IndicatorWindowGlobalVariables.ProgressReportString4=out4.str();
	IndicatorWindowGlobalVariables.String1NeedsRefresh=true;
	IndicatorWindowGlobalVariables.String2NeedsRefresh=true;
	IndicatorWindowGlobalVariables.String3NeedsRefresh=true;
	IndicatorWindowGlobalVariables.String4NeedsRefresh=true;
//	::IndicatorWindowGlobalVariables.String5NeedsRefresh=false;
	IndicatorWindowGlobalVariables.StatusString1NeedsRefresh=false;
	if (TheGlobalVariables.FeedDataToIndicatorWindowDefault!=0)
		TheGlobalVariables.FeedDataToIndicatorWindowDefault(::IndicatorWindowGlobalVariables);
}

void minimalRelationsProverStatesFixedK::MakeProgressReportStack	( GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl)
{ std::stringstream out1, out2, out3, out4;
	std::stringstream* tempOut1=&out1;
	for (int i=0;i<this->theIndexStack.size;i++)
	{ int currentIndex=this->theIndexStack.TheObjects[i];
	  (*tempOut1)	<< currentIndex << ": " << this->TheObjects[currentIndex].activeChild+1<< " of "<< this->TheObjects[currentIndex].PossibleChildStates.size	<<"; ";
		if (i==4)	tempOut1=&out2;
		if (i==8)	tempOut1=&out3;
	//	if (i==12)	tempOut1=&out4;
	}
	if (theIndexStack.size!=0)
    (*tempOut1)<< this->TheObjects[*this->theIndexStack.LastObject()].PartialRelation.Alphas.size <<" alphas + ... = " <<this->TheObjects[*this->theIndexStack.LastObject()].PartialRelation.Betas.size <<" betas + ...";
	IndicatorWindowGlobalVariables.ProgressReportString1=out1.str();
	IndicatorWindowGlobalVariables.ProgressReportString2=out2.str();
	if (this->theIndexStack.size>8)
		::IndicatorWindowGlobalVariables.ProgressReportString3=out3.str();
	IndicatorWindowGlobalVariables.ProgressReportString4=out4.str();
	IndicatorWindowGlobalVariables.String1NeedsRefresh=true;
	IndicatorWindowGlobalVariables.String2NeedsRefresh=true;
	IndicatorWindowGlobalVariables.String3NeedsRefresh=true;
	IndicatorWindowGlobalVariables.String4NeedsRefresh=true;
//	::IndicatorWindowGlobalVariables.String5NeedsRefresh=false;
	IndicatorWindowGlobalVariables.StatusString1NeedsRefresh=false;
	if (TheGlobalVariables.FeedDataToIndicatorWindowDefault!=0)
		TheGlobalVariables.FeedDataToIndicatorWindowDefault(::IndicatorWindowGlobalVariables);
}

bool minimalRelationsProverState::RootIsGoodForProblematicIndex(root& input,int problemIndex, bool AddingAlphas, bool NeedPositiveContribution, roots& theDualBasis, WeylGroup& theWeyl)
{ if (AddingAlphas)
	{ if (this->nonAlphas.ContainsObject(input))
			return false;
	}else
	{ if (this->nonBetas.ContainsObject(input))
			return false;
	}
	Rational tempRat=theWeyl.RootScalarKillingFormMatrixRoot(input,theDualBasis.TheObjects[problemIndex]);
	if (NeedPositiveContribution &&  tempRat.IsPositive())
		return true;
	if (!NeedPositiveContribution && tempRat.IsNegative())
		return true;
	return false;
}

bool minimalRelationsProverState::RootIsGoodForPreferredSimpleRoot(root &input, int preferredIndex, bool &GoodForAlpha, WeylGroup &theWeyl, GlobalVariables &TheGlobalVariables, root& AlphasMinusBetas)
{ Rational tempRat1, tempRat2;
	tempRat2=AlphasMinusBetas.TheObjects[preferredIndex]; tempRat2.AssignAbsoluteValue();
	tempRat1= AlphasMinusBetas.TheObjects[preferredIndex]-input.TheObjects[preferredIndex];
	tempRat1.AssignAbsoluteValue();
	if (tempRat2.IsGreaterThan(tempRat1))
		if (!this->nonBetas.ContainsObject(input))
		{ GoodForAlpha=false;
			return true;
		}
	tempRat1=AlphasMinusBetas.TheObjects[preferredIndex]+input.TheObjects[preferredIndex];
	tempRat1.AssignAbsoluteValue();
	if (tempRat2.IsGreaterThan(tempRat1))
		if (!this->nonBKSingularGmodLRoots.ContainsObject(input))
		{ GoodForAlpha=true;
			return true;
		}
	return false;
}

minimalRelationsProverState::minimalRelationsProverState()
{ this->StateIsComplete=false;
  this->StateIsPossible=true;
	this->InternalStateIsComputed=false;
	this->flagNeedsAdditionOfPositiveKroots=false;
  this->NumImpossibleChildren=0;
	this->activeChild=-1;
}

minimalRelationsProverStateFixedK::minimalRelationsProverStateFixedK()
{ this->StateIsComplete=false;
  this->flagNeedsAdditionOfPositiveKroots=false;
	this->activeChild=-1;
}

bool minimalRelationsProverState::IsAGoodPosRootsKChoice(WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{ theWeyl.GenerateAdditivelyClosedSubset(this->PositiveKroots,this->PositiveKroots);
  for (int i=0;i<this->BKSingularGmodLRoots.size;i++)
    if (!this->SumWithNoPosRootIsARoot(this->BKSingularGmodLRoots.TheObjects[i], theWeyl))
      return false;
  return true;
}

bool minimalRelationsProverState::FindBetaWithoutTwoAlphas(root& outputBeta, roots& inputBetas, roots& inputAlphas, WeylGroup& theWeyl)
{ for (int i=0;i<inputBetas.size;i++)
    if (inputAlphas.NumRootsConnectedTo(inputBetas.TheObjects[i],theWeyl)<2)
    { outputBeta.Assign(inputBetas.TheObjects[i]);
      return true;
    }
  return false;
}

bool minimalRelationsProverStates::StateIsEqualTo(minimalRelationsProverState& theState, int IndexOther, WeylGroup& theWeyl,  GlobalVariables& TheGlobalVariables)
{ return this->ExtendToIsomorphismRootSystem(theState,IndexOther,TheGlobalVariables,theWeyl);
}

bool minimalRelationsProverStates::AddObjectOnTopNoRepetitionOfObject( int ParentIndex, minimalRelationsProverState& theState, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{ if (theState.PartialRelation.Alphas.size+theState.PartialRelation.Betas.size<7)
		for (int i=0;i<this->size;i++)
			if (this->StateIsEqualTo(theState,i,theWeyl,TheGlobalVariables))
				return false;
  this->AddObjectOnTop(theState);
  return true;
}

void minimalRelationsProverState::ComputeScalarProductsMatrix(GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl)
{ return this->ComputeScalarProductsMatrix(	TheGlobalVariables, theWeyl, this->PartialRelation.Alphas, this->PartialRelation.Betas, this->theScalarProducts);
}

void minimalRelationsProverState::ComputeScalarProductsMatrix(GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl, roots& theAlphas,		roots& theBetas, MatrixLargeRational& output)
{ roots& tempRoots=TheGlobalVariables.rootsProverStateComputation1;
  tempRoots.size=0;
  tempRoots.AddListOnTop(theAlphas);
  tempRoots.AddListOnTop(theBetas);
  output.init(tempRoots.size, tempRoots.size);
  for (int i=0;i<tempRoots.size;i++)
    for (int j=0;j<tempRoots.size;j++)
      theWeyl.RootScalarKillingFormMatrixRoot(tempRoots.TheObjects[i], tempRoots.TheObjects[j],output.elements[i][j]);
}


void minimalRelationsProverState::ElementToString( std::string& output, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables, bool displayEpsilons)
{ std::string tempS; std::stringstream out;
	for(int i=0;i<this->PartialRelation.Alphas.size;i++)
	{ this->PartialRelation.Alphas.TheObjects[i].ElementToString(tempS);
		out << "a_"<<i+1<<tempS <<" + ";
	}
	out <<"... = ";
	for(int i=0;i<this->PartialRelation.Betas.size;i++)
	{ this->PartialRelation.Betas.TheObjects[i].ElementToString(tempS);
		out << "b_"<<i+1<<tempS <<" + ";
	}
	out <<"...     (a_i,b_i>0)\r\n";
	root tempRoot; roots tempRoots;
	for(int i=0; i<this->PartialRelation.Alphas.size; i++)
	{ theWeyl.GetEpsilonCoords(this->PartialRelation.Alphas.TheObjects[i],tempRoot,TheGlobalVariables);
		tempRoot.ElementToStringEpsilonForm(tempS,false,false);
		out << "a_"<<i+1<<"("<<tempS <<") + ";
	}
	out <<"... = ";
	for(int i=0;i<this->PartialRelation.Betas.size;i++)
	{ theWeyl.GetEpsilonCoords(this->PartialRelation.Betas.TheObjects[i],tempRoot,TheGlobalVariables);
		tempRoot.ElementToStringEpsilonForm(tempS,false,false);
		out << "b_"<<i+1<<"("<<tempS <<") + ";
	}
	out <<"...";
  this->currentSeparatingNormalEpsilonForm.ElementToStringEpsilonForm(tempS,false,false);
  out <<"\nCurrent separating normal: " << tempS;
  out <<"\r\nChildren states: "<<  this->PossibleChildStates.size <<" possible, "<< this->NumImpossibleChildren <<" impossible, "<< this->CompleteChildStates.size
        <<" complete. Index next possible child to explore: " << this->activeChild+2;
  roots AlphaChildren, BetaChildren, Kchildren;
  for (int i=0;i<this->PossibleChildStates.size;i++)
  { minimalRelationsProverState& child = owner->TheObjects[this->PossibleChildStates.TheObjects[i]];
		if (this->PartialRelation.Alphas.size<child.PartialRelation.Alphas.size)
		{ theWeyl.GetEpsilonCoords(*child.theChoicesWeMake.LastObject(),tempRoot,TheGlobalVariables);
			AlphaChildren.AddObjectOnTop(tempRoot);
		}
		if (this->PartialRelation.Betas.size<child.PartialRelation.Betas.size)
		{ theWeyl.GetEpsilonCoords(*child.theChoicesWeMake.LastObject(),tempRoot,TheGlobalVariables);
			BetaChildren.AddObjectOnTop(tempRoot);
		}
		if (this->ChosenPositiveKroots.size<child.ChosenPositiveKroots.size)
		{ theWeyl.GetEpsilonCoords(*child.ChosenPositiveKroots.LastObject(),tempRoot,TheGlobalVariables);
			Kchildren.AddObjectOnTop(tempRoot);
		}
  }
  //assert(!(AlphaChildren.size+BetaChildren.size+Kchildren.size==0));
	if (AlphaChildren.size>0)
	{ AlphaChildren.ElementToStringEpsilonForm(tempS,false,false,false);
		out <<"\r\nNew alphas added next: " <<tempS;
  }
	if (BetaChildren.size>0)
	{ BetaChildren.ElementToStringEpsilonForm(tempS,false,false,false);
		out <<"\r\nNew betas added next: " <<tempS;
  }
	if (Kchildren.size>0)
	{ Kchildren.ElementToStringEpsilonForm(tempS,false,false,false);
		out <<"\r\nNew k-roots added next: " <<tempS;
  }
  if(!displayEpsilons)
    this->ChosenPositiveKroots.ElementToString(tempS);
  else
  { theWeyl.GetEpsilonCoords(this->ChosenPositiveKroots,tempRoots,TheGlobalVariables);
    tempRoots.ElementToStringEpsilonForm(tempS,false,false,false);
  }
  out <<"\r\nPositive choices for K-roots made("<<this->ChosenPositiveKroots.size<<"): " << tempS;
  if(!displayEpsilons)
    this->nonLNonSingularRootsInNeedOfPosKroots.ElementToString(tempS);
  else
  { theWeyl.GetEpsilonCoords(this->nonLNonSingularRootsInNeedOfPosKroots, tempRoots, TheGlobalVariables);
    tempRoots.ElementToStringEpsilonForm(tempS,false,false,false);
  }
  out <<"\r\nNon-L non-singular roots in need of a K- positive root("<<this->nonLNonSingularRootsInNeedOfPosKroots.size <<"): " << tempS;
  if (!displayEpsilons)
    this->theChoicesWeMake.ElementToString(tempS);
  else
  { theWeyl.GetEpsilonCoords(this->theChoicesWeMake,tempRoots,TheGlobalVariables);
    tempRoots.ElementToStringEpsilonForm(tempS,false,false,false);
  }
  out <<"\r\nThe choices we make("<<this->theChoicesWeMake.size <<"): "<< tempS;
	if (!displayEpsilons)
    this->BKSingularGmodLRoots.ElementToString(tempS);
  else
  { theWeyl.GetEpsilonCoords(this->BKSingularGmodLRoots,tempRoots,TheGlobalVariables);
    tempRoots.ElementToStringEpsilonForm(tempS,false,false,false);
  }
  out <<"\r\nBK singular g mod l roots("<< this->BKSingularGmodLRoots.size<<"): "<< tempS;
	if (!displayEpsilons)
    this->nonAlphas.ElementToString(tempS);
  else
  { theWeyl.GetEpsilonCoords(this->nonAlphas,tempRoots,TheGlobalVariables);
    tempRoots.ElementToStringEpsilonForm(tempS,false,false,false);
  }
  out <<"\r\nNon-alphas("<< this->nonAlphas.size<<"): "<< tempS;
  if (!displayEpsilons)
    this->nonBetas.ElementToString(tempS);
  else
  { theWeyl.GetEpsilonCoords(this->nonBetas,tempRoots,TheGlobalVariables);
    tempRoots.ElementToStringEpsilonForm(tempS,false,false,false);
  }
  out <<"\r\nNon-betas("<<this->nonBetas.size <<"): "<< tempS;
  if (!displayEpsilons)
    this->nonBKSingularGmodLRoots.ElementToString(tempS);
  else
  { theWeyl.GetEpsilonCoords(this->nonBKSingularGmodLRoots,tempRoots,TheGlobalVariables);
    tempRoots.ElementToStringEpsilonForm(tempS,false,false,false);
  }
  out <<"\r\nnon BK-singular g mod l roots("<< this->nonBKSingularGmodLRoots.size<<"): "<< tempS;
  if (!displayEpsilons)
    this->NilradicalRoots.ElementToString(tempS);
  else
  { theWeyl.GetEpsilonCoords(this->NilradicalRoots,tempRoots,TheGlobalVariables);
    tempRoots.ElementToStringEpsilonForm(tempS,false,false,false);
  }
  out << "\r\nNilradical roots("<< this->NilradicalRoots.size<<"): "<< tempS;
  if (!displayEpsilons)
    this->nonNilradicalRoots.ElementToString(tempS);
  else
  { theWeyl.GetEpsilonCoords(this->nonNilradicalRoots,tempRoots,TheGlobalVariables);
    tempRoots.ElementToStringEpsilonForm(tempS,false,false,false);
  }
  out << "\r\nnon-nilradical roots("<< this->nonNilradicalRoots.size<<"): "<< tempS;
  if (!displayEpsilons)
    this->PositiveKroots.ElementToString(tempS);
  else
  { theWeyl.GetEpsilonCoords(this->PositiveKroots,tempRoots,TheGlobalVariables);
    tempRoots.ElementToStringEpsilonForm(tempS,false,false,false);
  }
  out << "\r\nPositive roots of K("<<this->PositiveKroots.size <<"): "<< tempS;
  if (!displayEpsilons)
    this->nonKRoots.ElementToString(tempS);
  else
  { theWeyl.GetEpsilonCoords(this->nonKRoots,tempRoots,TheGlobalVariables);
    tempRoots.ElementToStringEpsilonForm(tempS,false,false,false);
  }
  out <<"\r\nNon-roots of K("<<this->nonKRoots.size <<"): "<< tempS;
  if (!displayEpsilons)
    this->nonPositiveKRoots.ElementToString(tempS);
  else
  { theWeyl.GetEpsilonCoords(this->nonPositiveKRoots,tempRoots,TheGlobalVariables);
    tempRoots.ElementToStringEpsilonForm(tempS,false,false,false);
  }
  out <<"\r\nNon-positive roots of K("<< this->nonPositiveKRoots.size<<"): "<< tempS;
  if (!displayEpsilons)
    this->nonLRoots.ElementToString(tempS);
  else
  { theWeyl.GetEpsilonCoords(this->nonLRoots,tempRoots,TheGlobalVariables);
    tempRoots.ElementToStringEpsilonForm(tempS,false,false,false);
  }
  out <<"\r\nNon-L roots("<<this->nonLRoots.size <<"): "<< tempS;
  if (!displayEpsilons)
    this->nonLNonSingularRoots.ElementToString(tempS);
  else
  { theWeyl.GetEpsilonCoords(this->nonLNonSingularRoots,tempRoots,TheGlobalVariables);
    tempRoots.ElementToStringEpsilonForm(tempS,false,false,false);
  }
  out <<"\r\nNon-L non-singular roots("<< this->nonLNonSingularRoots.size<<"): "<< tempS;
  if (!this->StateIsInternallyPossible)
		out << "\r\n I am Impossible!";
  if (this->StateIsComplete)
		out <<"\r\nI am complete!";
  out<<"\r\n\n";
  output=out.str();
}

void minimalRelationsProverStateFixedK::ElementToString( std::string& output, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables, bool displayEpsilons)
{ std::string tempS; std::stringstream out;
	for(int i=0;i<this->PartialRelation.Alphas.size;i++)
	{ this->PartialRelation.Alphas.TheObjects[i].ElementToString(tempS);
		out << "a_"<<i+1<<tempS <<" + ";
	}
	out <<"... = ";
	for(int i=0;i<this->PartialRelation.Betas.size;i++)
	{ this->PartialRelation.Betas.TheObjects[i].ElementToString(tempS);
		out << "b_"<<i+1<<tempS <<" + ";
	}
	out <<"...     (a_i,b_i>0)\r\n";
	root tempRoot; roots tempRoots;
	for(int i=0; i<this->PartialRelation.Alphas.size; i++)
	{ theWeyl.GetEpsilonCoords(this->PartialRelation.Alphas.TheObjects[i],tempRoot,TheGlobalVariables);
		tempRoot.ElementToStringEpsilonForm(tempS,false,false);
		out << "a_"<<i+1<<"("<<tempS <<") + ";
	}
	out <<"... = ";
	for(int i=0; i<this->PartialRelation.Betas.size; i++)
	{ theWeyl.GetEpsilonCoords(this->PartialRelation.Betas.TheObjects[i],tempRoot,TheGlobalVariables);
		tempRoot.ElementToStringEpsilonForm(tempS,false,false);
		out << "b_"<<i+1<<"("<<tempS <<") + ";
	}
	out <<"...";
  this->currentSeparatingNormalEpsilonForm.ElementToStringEpsilonForm(tempS,false,false);
  out <<"\nCurrent separating normal: " << tempS;
  out <<"\nInternal separating normal: " << this->SeparatingNormalUsed.ElementToString();
  out<<"\r\nChildren states: "<<  this->PossibleChildStates.size <<" possible, "<< this->ImpossibleChildStates.size
				<<" impossible, " << this->CompleteChildStates.size<<" complete. Index next possible child to explore: " << this->activeChild+2;
  roots AlphaChildren, BetaChildren, Kchildren;
  for (int i=0; i<this->PossibleChildStates.size; i++)
  { minimalRelationsProverStateFixedK& child = owner->TheObjects[this->PossibleChildStates.TheObjects[i]];
		if (this->PartialRelation.Alphas.size<child.PartialRelation.Alphas.size)
		{ theWeyl.GetEpsilonCoords(*child.theChoicesWeMake.LastObject(),tempRoot,TheGlobalVariables);
			AlphaChildren.AddObjectOnTop(tempRoot);
		}
		if (this->PartialRelation.Betas.size<child.PartialRelation.Betas.size)
		{ theWeyl.GetEpsilonCoords(*child.theChoicesWeMake.LastObject(),tempRoot,TheGlobalVariables);
			BetaChildren.AddObjectOnTop(tempRoot);
		}
  }
  //assert(!(AlphaChildren.size+BetaChildren.size+Kchildren.size==0));
	if (AlphaChildren.size>0)
	{ AlphaChildren.ElementToStringEpsilonForm(tempS,false,false,false);
		out <<"\r\nNew alphas added next: " <<tempS;
  }
	if (BetaChildren.size>0)
	{ BetaChildren.ElementToStringEpsilonForm(tempS,false,false,false);
		out <<"\r\nNew betas added next: " <<tempS;
  }
	if (Kchildren.size>0)
	{ Kchildren.ElementToStringEpsilonForm(tempS,false,false,false);
		out <<"\r\nNew k-roots added next: " <<tempS;
  }
  if (!displayEpsilons)
    this->theChoicesWeMake.ElementToString(tempS);
  else
  { theWeyl.GetEpsilonCoords(this->theChoicesWeMake,tempRoots,TheGlobalVariables);
    tempRoots.ElementToStringEpsilonForm(tempS,false,false,false);
  }
  out <<"\r\nThe choices we make: "<< tempS;
  if (!displayEpsilons)
    this->nonAlphas.ElementToString(tempS);
  else
  { theWeyl.GetEpsilonCoords(this->nonAlphas,tempRoots,TheGlobalVariables);
    tempRoots.ElementToStringEpsilonForm(tempS, false, false, false);
  }
  out <<"\r\nNon alphas("<< this->nonAlphas.size<<"): "<< tempS;
  if (!displayEpsilons)
    this->nonBetas.ElementToString(tempS);
  else
  { theWeyl.GetEpsilonCoords(this->nonBetas,tempRoots,TheGlobalVariables);
    tempRoots.ElementToStringEpsilonForm(tempS,false,false,false);
  }
  out <<"\r\nNon betas("<<this->nonBetas.size<<"): "<< tempS;
  out<<"\r\nG mod L modules(" <<this->theGmodLmodules.CardinalitySelection<<"): ";
  for (int i=0;i<this->theGmodLmodules.CardinalitySelection;i++)
  	out<< this->theGmodLmodules.elements[i]<<" ";
  out<<"\r\nNilradical modules(" <<this->theNilradicalModules.CardinalitySelection<<"): ";
  for (int i=0;i<this->theNilradicalModules.CardinalitySelection;i++)
  	out<< this->theNilradicalModules.elements[i]<<" ";
  if (!this->StateIsPossible)
		out << "\r\n I am Impossible!";
  if (this->StateIsComplete)
		out <<"\r\nI am complete!";
  out<<"\r\n\n";
  out <<"Call stack: ";
  for (int i=0; i<this->owner->theIndexStack.size; i++)
  { out << "state "<<this->owner->theIndexStack.TheObjects[i];
		if (i!=this->owner->theIndexStack.size-1)
			out<<"-> ";
  }
  output=out.str();
}

void minimalRelationsProverStates::ElementToString(std::string& output,WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{ std::string tempS; std::stringstream out;
	for (int i=0;i<this->size;i++)
	{ this->TheObjects[i].ElementToString(tempS,theWeyl, TheGlobalVariables,true);
		out <<tempS<<"\n\n";
	}
	output=out.str();
}

void minimalRelationsProverStates::ComputePreferredDualBasis(char WeylLetter, int theDimension, GlobalVariables& TheGlobalVariables)
{ this->PreferredDualBasis.SetSizeExpandOnTopNoObjectInit(theDimension);
  for (int i=0;i<theDimension;i++)
		this->PreferredDualBasis.TheObjects[i].MakeZero(theDimension);
	if (WeylLetter=='D')
		for (int i=0;i<theDimension;i++)
		{ for (int j=i;j<theDimension-2;j++)
				this->PreferredDualBasis.TheObjects[i].TheObjects[j]=2;
			this->PreferredDualBasis.TheObjects[i].TheObjects[theDimension-1]=1;
			this->PreferredDualBasis.TheObjects[i].TheObjects[theDimension-2]=1;
			if (i==theDimension-1)
				this->PreferredDualBasis.TheObjects[i].TheObjects[theDimension-2]=-1;
		}
	if (WeylLetter=='E' && theDimension==8)
  { this->PreferredDualBasis.SetSizeExpandOnTopNoObjectInit(0);
    //for (int i=0; i<theDimension; i++)
     // this->PreferredDualBasis.TheObjects[i].TheObjects[i].MakeOne();
/*    for (int i=0; i<7; i++)
    { this->PreferredDualBasis.TheObjects[i].TheObjects[1]=1;
			this->PreferredDualBasis.TheObjects[i].TheObjects[2]=-1;
      for (int j=0; j<i; j++)
        this->PreferredDualBasis.TheObjects[i].TheObjects[j+2]+=2;
    }
    this->PreferredDualBasis.TheObjects[7].TheObjects[0]=4;
    this->PreferredDualBasis.TheObjects[7]-=this->PreferredDualBasis.TheObjects[0];
    for (int i=1; i< 7;i++)
      this->PreferredDualBasis.TheObjects[7]+=this->PreferredDualBasis.TheObjects[i];*/
  }
  int oldsize=PreferredDualBasis.size;
  for (int i=0;i<oldsize;i++)
    PreferredDualBasis.AddObjectOnTop(-PreferredDualBasis.TheObjects[i]);
  this->theWeylGroup.GetEpsilonCoords( this->PreferredDualBasis, this->PreferredDualBasisEpsilonCoords, TheGlobalVariables);
	this->PreferredDualBasisEpsilonCoords.ElementToStringEpsilonForm( this->PreferredDualBasisEpsilonCoords.DebugString, false, false, false);
}

void minimalRelationsProverStates::initShared(WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{ this->theWeylGroup.ComputeRho(true);
  this->isomorphismComputer.AmbientWeyl.Assign(this->theWeylGroup);
  this->isomorphismComputer.ComputeAll();
  this->flagAssumeGlobalMinimalityRHS=false;
  this->invertedCartan.AssignMatrixIntWithDen(this->theWeylGroup.KillingFormMatrix,1);
  this->invertedCartan.Invert(TheGlobalVariables);

	this->flagComputationIsInitialized=true;
}

void minimalRelationsProverStates::GenerateStartingState( ComputationSetup& theSetup, GlobalVariables& TheGlobalVariables, char WeylLetter, int theDimension)
{ if (this->flagComputationIsInitialized)
		return;
  this->WriteToFileAppend(TheGlobalVariables);
	minimalRelationsProverState tempState;
  this->theWeylGroup.MakeArbitrary(WeylLetter, theDimension);
  this->PreferredDualBasis.size=0;
	this->initShared(this->theWeylGroup, TheGlobalVariables);
  this->ComputePreferredDualBasis(WeylLetter, theDimension, TheGlobalVariables);
  this->size=0;
  root tempRoot;
  tempRoot.Assign(this->theWeylGroup.RootSystem.TheObjects[7]);
  tempState.PartialRelation.Alphas.AddObjectOnTop(tempRoot);
  for (int i=0;i<this->theWeylGroup.RootSystem.size;i++)
    if (this->theWeylGroup.RootSystem.TheObjects[i]!=tempRoot && this->theWeylGroup.RootScalarKillingFormMatrixRoot(tempRoot, this->theWeylGroup.RootSystem.TheObjects[i]).IsPositive() )
    { tempState.PartialRelation.Betas.AddObjectOnTop(this->theWeylGroup.RootSystem.TheObjects[i]);
      break;
    }
  tempState.theChoicesWeMake.AddObjectOnTop(tempState.PartialRelation.Betas.TheObjects[0]);
  tempState.theChoicesWeMake.AddObjectOnTop(tempState.PartialRelation.Alphas.TheObjects[0]);
  tempState.owner=this;
  this->AddObjectOnTop(tempState);
  this->LastObject()->ComputeStateReturnFalseIfDubious(*this, TheGlobalVariables, this->theWeylGroup, this->flagAssumeGlobalMinimalityRHS);
  this->LastObject()->ComputeScalarProductsMatrix(TheGlobalVariables, this->theWeylGroup);
  this->LastObject()->ComputeDebugString(this->theWeylGroup,TheGlobalVariables);
  this->theIndexStack.AddObjectOnTop(0);
  this->MakeProgressReportCurrentState(0,TheGlobalVariables, this->theWeylGroup);
  this->ComputeLastStackIndex(this->theWeylGroup, TheGlobalVariables);
}

void ComputationSetup::RunCustom()
{ this->DoTheRootSAComputationCustom();
//    this->theRootSubalgebras.ComputeDebugString(true);
  this->theOutput.DebugString.append(	"\\documentclass{article}\n\\usepackage{amssymb}\n");
	this->theOutput.DebugString.append("\\addtolength{\\hoffset}{-3.5cm}\\addtolength{\\textwidth}{7cm}");
	this->theOutput.DebugString.append("\\addtolength{\\voffset}{-3.5cm}\\addtolength{\\textheight}{7cm}");
	this->theOutput.DebugString.append("\\begin{document}~");
	this->theRootSubalgebras.theBadRelations.ComputeDebugString(this->theRootSubalgebras,*this->theGlobalVariablesContainer->Default());
	this->theRootSubalgebras.theGoodRelations.ComputeDebugString(this->theRootSubalgebras,*this->theGlobalVariablesContainer->Default());
	//this->theRootSubalgebras.theMinRels.ComputeDebugString
	//	(this->theRootSubalgebras,*this->theGlobalVariablesContainer->Default());
	this->theOutput.DebugString.append(this->theRootSubalgebras.DebugString);
	this->theOutput.DebugString.append("\n\n\n");
	if (this->theRootSubalgebras.theGoodRelations.size!=0)
    this->theOutput.DebugString.append(this->theRootSubalgebras.theGoodRelations.DebugString);
	this->theOutput.DebugString.append("\n\n\n");
	if (this->theRootSubalgebras.theBadRelations.size>0)
	{ this->theOutput.DebugString.append("The bad relations: \n\n");
		this->theOutput.DebugString.append(this->theRootSubalgebras.theBadRelations.DebugString);
	}
	if (this->theRootSubalgebras.flagLookingForMinimalRels)
	{ this->theOutput.DebugString.append("\n\nMinimal relations: \n\n");
		this->theOutput.DebugString.append(this->theRootSubalgebras.theMinRels.DebugString);
	}
  this->theOutput.DebugString.append("\\end{document}");
}


void ComputationSetup::DoTheRootSAComputationCustom()
{ rootSubalgebra tempSA;
	this->theRootSubalgebras.flagUseDynkinClassificationForIsomorphismComputation=false;
	this->theRootSubalgebras.flagUsingActionsNormalizerCentralizerNilradical=true;
	this->theRootSubalgebras.flagLookingForMinimalRels=true;
	this->theRootSubalgebras.theGoodRelations.flagIncludeCoordinateRepresentation=true;
	this->theRootSubalgebras.theBadRelations.flagIncludeCoordinateRepresentation=true;
	this->theRootSubalgebras.theMinRels.flagIncludeCoordinateRepresentation=true;
	this->theRootSubalgebras.theGoodRelations.flagIncludeSubalgebraDataInDebugString=false;
	this->theRootSubalgebras.theBadRelations.flagIncludeSubalgebraDataInDebugString=false;
	this->theRootSubalgebras.GenerateAllRootSubalgebrasUpToIsomorphism(*this->theGlobalVariablesContainer->Default(), this->WeylGroupLetter, this->WeylGroupIndex, true, true);
  this->theRootSubalgebras.ComputeDebugString(true, false, true, 0, 0, *this->theGlobalVariablesContainer->Default());
  if (this->theRootSubalgebras.flagComputeConeCondition)
    this->theRootSubalgebras.ComputeLProhibitingRelations(*this->theGlobalVariablesContainer->Default(), 0, this->theRootSubalgebras.size-1);
  std::string tempS;
  this->theRootSubalgebras.ElementToStringCentralizerIsomorphisms(tempS, *this->theGlobalVariablesContainer->Default());
  this->theRootSubalgebras.DebugString.append(tempS);
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

void WeylGroup::GetEpsilonCoords(ListBasicObjects<root>& input, roots& output, GlobalVariables& theGlobalVariables)
{ roots tempRoots;
	tempRoots.SetSizeExpandOnTopNoObjectInit(this->KillingFormMatrix.NumRows);
	for (int i=0;i<this->KillingFormMatrix.NumRows;i++)
	{ tempRoots.TheObjects[i].MakeZero(this->KillingFormMatrix.NumRows);
		tempRoots.TheObjects[i].TheObjects[i].MakeOne();
	}
	this->GetEpsilonCoordsWRTsubalgebra(tempRoots, input, output, theGlobalVariables);
}

void WeylGroup::GetEpsilonCoords(root &input, root &output, GlobalVariables &theGlobalVariables)
{ roots tempRoots;
	roots tempInput, tempOutput;
	tempInput.AddObjectOnTop(input);
	tempRoots.SetSizeExpandOnTopNoObjectInit(this->KillingFormMatrix.NumRows);
	for (int i=0;i<this->KillingFormMatrix.NumRows;i++)
	{ tempRoots.TheObjects[i].MakeZero(this->KillingFormMatrix.NumRows);
		tempRoots.TheObjects[i].TheObjects[i].MakeOne();
	}
	this->GetEpsilonCoordsWRTsubalgebra(tempRoots,tempInput,tempOutput,theGlobalVariables);
	output.Assign(tempOutput.TheObjects[0]);
}

bool minimalRelationsProverStates::CheckConsistencyOfTree()
{ bool result=true;
  for (int i=0; i<this->size; i++)
    if (this->TheObjects[i].activeChild>-1 && this->TheObjects[i].activeChild<this->TheObjects[i].PossibleChildStates.size)
      if (!this->theIndexStack.ContainsObject(i))
      { result=false;
        assert(false);
      }
  return  result;
}

bool minimalRelationsProverStates::ExtendToIsomorphismRootSystem(	minimalRelationsProverState& theState, int indexOther, GlobalVariables& theGlobalVariables, WeylGroup& theWeyl)
{	//return true;
  theState.SortAlphasAndBetas(theGlobalVariables, theWeyl);
	theState.ComputeScalarProductsMatrix(theGlobalVariables,theWeyl);
	minimalRelationsProverState& theOtherState=this->TheObjects[indexOther];
	//rootSubalgebra& theRootSA=theGlobalVariables.rootSAProverIsos;
	roots& theAlphas = theState.PartialRelation.Alphas;
	roots& theBetas  = theState.PartialRelation.Betas;
	roots& theKroots = theState.ChosenPositiveKroots;
	roots& theOtherAlphas = theOtherState.PartialRelation.Alphas;
	roots& theOtherBetas  = theOtherState.PartialRelation.Betas;
  roots& theOtherKroots = theOtherState.ChosenPositiveKroots;
	if (theAlphas.size!=theOtherAlphas.size)
		return false;
	if (theBetas.size!=theOtherBetas.size)
		return false;
  if(theKroots.size!= theOtherKroots.size)
    return false;
  DynkinDiagramRootSubalgebra diagram1, diagram2;
  roots tempRoots; tempRoots.CopyFromBase(theState.PositiveKroots);
  diagram1.ComputeDiagramType(tempRoots, theWeyl);
  tempRoots.CopyFromBase(theState.PositiveKroots);
  diagram2.ComputeDiagramType(tempRoots, theWeyl);
  if (!(diagram1==diagram2))
    return false;
	ListBasicObjects<int> tempList;
	permutation thePermAlphas, thePermBetas, thePermK, tempPermAlphas, tempPermBetas, tempPermK;
  this->GetIsoTypicComponents(theAlphas, theBetas, thePermAlphas, theState, theWeyl,  theGlobalVariables);
  this->GetIsoTypicComponents(theBetas, theAlphas, thePermBetas, theState, theWeyl, theGlobalVariables);
  this->GetIsoTypicComponents(theKroots, theAlphas, thePermK, theState, theWeyl, theGlobalVariables);
  this->GetIsoTypicComponents(theOtherAlphas, theOtherBetas, tempPermAlphas, theOtherState, theWeyl,  theGlobalVariables);
  this->GetIsoTypicComponents(theOtherBetas, theOtherAlphas, tempPermBetas, theOtherState, theWeyl, theGlobalVariables);
  this->GetIsoTypicComponents(theOtherKroots, theOtherAlphas, tempPermK, theOtherState, theWeyl, theGlobalVariables);
  if (!thePermAlphas.HasSameMaxMultiplicities(tempPermAlphas) || !thePermBetas.HasSameMaxMultiplicities(tempPermBetas) || !thePermK.HasSameMaxMultiplicities(tempPermK))
    return false;
	int NumCyclesAlphas = thePermAlphas.getTotalNumSubsets();
	int NumCyclesBetas  = thePermBetas.getTotalNumSubsets();
	int NumCyclesK= thePermK.getTotalNumSubsets();
	roots theDomain, thePermutedAlphas, thePermutedBetas, thePermutedKs, theRange;
	theRange.size=0;
	theRange.AddListOnTop(theOtherKroots);
	theRange.AddListOnTop(theOtherAlphas);
	theRange.AddListOnTop(theOtherBetas);
	MatrixLargeRational theOtherMatrix, thisMatrix;
	this->TheObjects[indexOther].ComputeScalarProductsMatrix(theGlobalVariables, theWeyl, theOtherAlphas, theOtherBetas, theOtherMatrix);
	this->TheObjects[indexOther].theScalarProducts.ComputeDebugString();
	bool DomainAndRangeGenerateNonIsoSAs;
	//theDomain.AddListOnTop(theState.ChosenPositiveKroots);
	for (int l=0;l<NumCyclesK;l++)
	{ thePermutedKs.size=0;
    thePermK.GetPermutation(tempList);
    for (int k=0;k<tempList.size;k++)
    { root& tempRoot=theKroots.TheObjects[tempList.TheObjects[k]];
      thePermutedKs.AddObjectOnTop(tempRoot);
      theDomain.AddObjectOnTop(tempRoot);
    }
	  for(int i=0;i<NumCyclesAlphas;i++)
    { theDomain.size=theKroots.size;
      thePermutedAlphas.size=0;
      thePermAlphas.GetPermutation(tempList);
      for (int k=0;k<tempList.size;k++)
      { root& tempRoot=theAlphas.TheObjects[tempList.TheObjects[k]];
        thePermutedAlphas.AddObjectOnTop(tempRoot);
        theDomain.AddObjectOnTop(tempRoot);
      }
      //theDomain.ComputeDebugString();
      for (int j=0;j<NumCyclesBetas;j++)
      { theDomain.size=theAlphas.size+theKroots.size;
        thePermutedBetas.size=0;
        thePermBetas.GetPermutation(tempList);
        for (int k=0;k<tempList.size;k++)
        { root& tempRoot=theBetas.TheObjects[tempList.TheObjects[k]];
          thePermutedBetas.AddObjectOnTop(tempRoot);
          theDomain.AddObjectOnTop(tempRoot);
        }
  //			theDomain.ComputeDebugString();
        this->MakeProgressReportIsos( indexOther, (l*NumCyclesAlphas+ i)*NumCyclesBetas+j, NumCyclesK*NumCyclesBetas*NumCyclesAlphas, theGlobalVariables, theWeyl);
        theState.ComputeScalarProductsMatrix(theGlobalVariables, theWeyl, thePermutedAlphas, thePermutedBetas, thisMatrix);
        if (thisMatrix.IsEqualTo(theOtherMatrix))
        { if (this->isomorphismComputer.attemptExtensionToIsomorphism(theDomain, theRange,	theGlobalVariables, 0, false, theWeyl, &DomainAndRangeGenerateNonIsoSAs))
            return true;
          else
            if (DomainAndRangeGenerateNonIsoSAs)
              return false;
        }
        thePermBetas.IncrementSubset();
      }
      thePermAlphas.IncrementSubset();
    }
    thePermK.IncrementSubset();
	}
	//theSA.attemptExtensionToIsomorphismNoCentralizer
	return false;
}

void minimalRelationsProverStates::GetIsoTypicComponents( roots& theRoots, roots& theOtherTypeRoots, permutation& outputComponents, minimalRelationsProverState& theState, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{ ListBasicObjects<int> tempList;
  if(theRoots.size==0)
  { tempList.size=0;
    outputComponents.initPermutation(tempList,0);
    return;
  }
  tempList.MakeActualSizeAtLeastExpandOnTop(theRoots.size);
	rootsCollection& isotypicPieces=TheGlobalVariables.rootsExtendToIsomorphismRootSystem1;
	isotypicPieces.MakeActualSizeAtLeastExpandOnTop(theRoots.size);
	isotypicPieces.SetSizeExpandOnTopNoObjectInit(1);
	isotypicPieces.TheObjects[0].size=0;
	isotypicPieces.TheObjects[0].AddObjectOnTop(theRoots.TheObjects[0]);
	tempList.size=0;
	for (int i=1; i<theRoots.size; i++)
	{ if (theState.Root1IsGreaterThanRoot2	(i-1,i,theRoots,theOtherTypeRoots,TheGlobalVariables,theWeyl))
		{ tempList.AddObjectOnTop(isotypicPieces.LastObject()->size);
			isotypicPieces.SetSizeExpandOnTopNoObjectInit(isotypicPieces.size+1);
			isotypicPieces.LastObject()->size=0;
		}
		isotypicPieces.LastObject()->AddObjectOnTop(theRoots.TheObjects[i]);
	}
	tempList.AddObjectOnTop(isotypicPieces.LastObject()->size);
	outputComponents.initPermutation(tempList,theRoots.size);
}

void minimalRelationsProverStatesFixedK::GetIsoTypicComponents
  ( roots& theRoots, roots& theOtherTypeRoots, permutation& outputComponents, minimalRelationsProverStateFixedK& theState, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{ ListBasicObjects<int> tempList;
  if(theRoots.size==0)
  { tempList.size=0;
    outputComponents.initPermutation(tempList,0);
    return;
  }
  tempList.MakeActualSizeAtLeastExpandOnTop(theRoots.size);
	rootsCollection& isotypicPieces=TheGlobalVariables.rootsExtendToIsomorphismRootSystem1;
	isotypicPieces.MakeActualSizeAtLeastExpandOnTop(theRoots.size);
	isotypicPieces.SetSizeExpandOnTopNoObjectInit(1);
	isotypicPieces.TheObjects[0].size=0;
	isotypicPieces.TheObjects[0].AddObjectOnTop(theRoots.TheObjects[0]);
	tempList.size=0;
	for (int i=1; i<theRoots.size; i++)
	{ if (::minimalRelationsProverState::Root1IsGreaterThanRoot2	(i-1,i,theRoots,theOtherTypeRoots,TheGlobalVariables,theWeyl))
		{ tempList.AddObjectOnTop(isotypicPieces.LastObject()->size);
			isotypicPieces.SetSizeExpandOnTopNoObjectInit(isotypicPieces.size+1);
			isotypicPieces.LastObject()->size=0;
		}
		isotypicPieces.LastObject()->AddObjectOnTop(theRoots.TheObjects[i]);
	}
	tempList.AddObjectOnTop(isotypicPieces.LastObject()->size);
	outputComponents.initPermutation(tempList,theRoots.size);
}

bool rootSubalgebra::attemptExtensionToIsomorphism( roots& Domain, roots& Range, GlobalVariables& theGlobalVariables,	ReflectionSubgroupWeylGroup* outputAutomorphisms, bool actOnCentralizerOnly, WeylGroup& theWeyl, bool *DomainAndRangeGenerateNonIsoSAs)
{ if (outputAutomorphisms!=0)
		outputAutomorphisms->ExternalAutomorphisms.size=0;
	if (DomainAndRangeGenerateNonIsoSAs!=0)
		*DomainAndRangeGenerateNonIsoSAs=false;
  //rootSubalgebra::ProblemCounter++;
  rootSubalgebra& theDomainRootSA = theGlobalVariables.rootSAAttemptExtensionToIso1;
  rootSubalgebra& theRangeRootSA  = theGlobalVariables.rootSAAttemptExtensionToIso2;
  theDomainRootSA.AmbientWeyl.Assign(theWeyl);
  theRangeRootSA.AmbientWeyl.Assign(theWeyl);
  theDomainRootSA.genK.CopyFromBase(Domain);
  theRangeRootSA.genK.CopyFromBase(Range);
	theDomainRootSA.ComputeAllButAmbientWeyl();
	theRangeRootSA.ComputeAllButAmbientWeyl();
	if (theDomainRootSA.theDynkinDiagram.DebugString!= theRangeRootSA.theDynkinDiagram.DebugString || theDomainRootSA.theCentralizerDiagram.DebugString!=theRangeRootSA.theCentralizerDiagram.DebugString)
  { if (DomainAndRangeGenerateNonIsoSAs!=0)
			*DomainAndRangeGenerateNonIsoSAs=true;
		return false;
  }
  roots isoDomain, isoRange;
  permutation permComponentsCentralizer;
  ListBasicObjects<int> tempList, tempPermutation1, tempPermutation2;
	SelectionWithDifferentMaxMultiplicities tempAutosCentralizer;
  ListBasicObjects<ListBasicObjects<ListBasicObjects<int> > > CentralizerDiagramAutomorphisms;
  theDomainRootSA.theCentralizerDiagram.GetAutomorphisms(CentralizerDiagramAutomorphisms);
  theDomainRootSA.theCentralizerDiagram.ComputeDebugString();
  tempAutosCentralizer.initIncomplete(CentralizerDiagramAutomorphisms.size);
	for (int i=0; i<CentralizerDiagramAutomorphisms.size; i++)
		tempAutosCentralizer.MaxMultiplicities.TheObjects[i] = CentralizerDiagramAutomorphisms.TheObjects[i].size-1;
  tempList.SetSizeExpandOnTopNoObjectInit(theDomainRootSA.theCentralizerDiagram.sameTypeComponents.size);
  int tempSize=0;
  for (int i=0;i<theDomainRootSA.theCentralizerDiagram.sameTypeComponents.size;i++)
  { tempList.TheObjects[i]=theDomainRootSA.theCentralizerDiagram.sameTypeComponents.TheObjects[i].size;
    tempSize+=tempList.TheObjects[i];
  }
  permComponentsCentralizer.initPermutation(tempList,tempSize);
	int tempI2= permComponentsCentralizer.getTotalNumSubsets();
	int NumAutosCentralizer= tempAutosCentralizer.getTotalNumSubsets();
  permComponentsCentralizer.GetPermutation(tempPermutation2);
  for (int i=0;i<Domain.size;i++)
  { isoDomain.AddObjectOnTop(Domain.TheObjects[i]);
		if (isoDomain.GetRankOfSpanOfElements(theGlobalVariables)<isoDomain.size)
			isoDomain.PopLastObject();
		else
			isoRange.AddObjectOnTop(Range.TheObjects[i]);
  }
  if (isoRange.GetRankOfSpanOfElements(theGlobalVariables)<isoRange.size)
		return false;
  int givenSize=isoDomain.size;
	for(int j=0;j<tempI2;j++)
  { for(int l=0;l<NumAutosCentralizer;l++)
		{ isoDomain.size=givenSize; isoRange.size=givenSize;
			theDomainRootSA.theCentralizerDiagram.GetMapFromPermutation(isoDomain,isoRange,tempPermutation2,CentralizerDiagramAutomorphisms, tempAutosCentralizer, theRangeRootSA.theCentralizerDiagram);
			if (theDomainRootSA.attemptExtensionToIsomorphismNoCentralizer(	isoDomain, isoRange, theGlobalVariables, 0, outputAutomorphisms, false, 0, &Domain, &Range))//GenerateAllAutos))
				if (outputAutomorphisms==0)
					return true;
			if (outputAutomorphisms!=0)
				theDomainRootSA.MakeProgressReportGenAutos( l+NumAutosCentralizer*j, tempI2*NumAutosCentralizer, outputAutomorphisms->ExternalAutomorphisms.size, theGlobalVariables);
			tempAutosCentralizer.IncrementSubset();
		}
		permComponentsCentralizer.incrementAndGetPermutation(tempPermutation2);
  }
  return false;
}
