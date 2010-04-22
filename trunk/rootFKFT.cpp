#include "polyhedra.h"
#include "rootFKFT.h"
//to be merged in main file polyhedra.cpp later

int minimalRelationsProverState::ProblemCounter=0;
int ::minimalRelationsProverStates::ProblemCounter=0;
template < > int ListBasicObjects<minimalRelationsProverState>::ListBasicObjectsActualSizeIncrement=800;
extern ::IndicatorWindowVariables IndicatorWindowGlobalVariables;


bool minimalRelationsProverState::IsBKSingularNonImplied(root& input, WeylGroup& theWeyl)
{ root tempRoot;
  for (int j=0;j<this->PositiveKroots.size;j++)
  { tempRoot=input+this->PositiveKroots.TheObjects[j];
    if(theWeyl.IsARoot(tempRoot))
      return false;
  }
  return true;
}

bool minimalRelationsProverState::IsBKSingularImplied(root& input, WeylGroup& theWeyl)
{ if (!this->IsBKSingularNonImplied(input, theWeyl))
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

void minimalRelationsProverState::SortAlphasAndBetas
	(GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl)
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

bool minimalRelationsProverState::Root1IsGreaterThanRoot2
	(	int index1, int index2,roots& setWeBelongTo, roots& setOtherSet,
		GlobalVariables &TheGlobalVariables, WeylGroup &theWeyl)
{	root& root1=setWeBelongTo.TheObjects[index1];
	root& root2=setWeBelongTo.TheObjects[index2];
	bool IsLong1, IsLong2;
	int NumLongValue1, NumMixedValue1, NumShortValue1,
			NumMinusLongValue1, NumMinusMixedValue1, NumMinusShortValue1,
			NumLongValue2, NumMixedValue2, NumShortValue2,
			NumMinusLongValue2, NumMinusMixedValue2, NumMinusShortValue2;
	this->GetNumberScalarProductsData
		(	root1,setWeBelongTo, IsLong1,
			NumLongValue1, NumMixedValue1, NumShortValue1,
			NumMinusLongValue1, NumMinusMixedValue1, NumMinusShortValue1,
			TheGlobalVariables, theWeyl);
	this->GetNumberScalarProductsData
		(	root2,setWeBelongTo, IsLong2,
			NumLongValue2, NumMixedValue2, NumShortValue2,
			NumMinusLongValue2, NumMinusMixedValue2, NumMinusShortValue2,
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

bool minimalRelationsProverState::ComputeCommonSenseImplicationsReturnFalseIfContradiction
	(WeylGroup& theWeyl,GlobalVariables& TheGlobalVariables)
{	root tempRoot;
	this->ComputeDebugString(theWeyl, TheGlobalVariables);
	for (int j=0;j<theWeyl.RootSystem.size;j++)
	{	for (int i=0;i<this->PartialRelation.Alphas.size;i++)
		{	tempRoot= this->PartialRelation.Alphas.TheObjects[i]+theWeyl.RootSystem.TheObjects[j];
			if (theWeyl.IsARoot(tempRoot))
				this->nonPositiveKRoots.AddObjectOnTopNoRepetitionOfObject
          (theWeyl.RootSystem.TheObjects[j]);
		}
		for (int i=0; i<this->PartialRelation.Betas.size;i++)
			if (theWeyl.IsARoot
            (this->PartialRelation.Betas.TheObjects[i]+theWeyl.RootSystem.TheObjects[j]))
				this->nonBetas.AddObjectOnTopNoRepetitionOfObject(theWeyl.RootSystem.TheObjects[j]);
	}
	this->ComputeDebugString(theWeyl, TheGlobalVariables);
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
  //tempRoots.ComputeDebugString();
  //roots& tempRoots2           = TheGlobalVariables.rootsProverStateComputation3;
  roots& nonLroots            = TheGlobalVariables.rootsProverStateComputation4;
  roots& nonLnonSingularRoots = TheGlobalVariables.rootsProverStateComputation5;
  for (int i=0;i<this->nonPositiveKRoots.size;i++)
  {	this->nonKRoots.AddObjectOnTopNoRepetitionOfObject
			(this->nonPositiveKRoots.TheObjects[i]);
		this->nonKRoots.AddObjectOnTopNoRepetitionOfObject
			(-this->nonPositiveKRoots.TheObjects[i]);
  }
  this->nonKRoots.intersectWith(this->nonNilradicalRoots,nonLroots);
  for (int i=0;i<nonLroots.size;i++)
  { if (this->IsBKSingularImplied(nonLroots.TheObjects[i], theWeyl))
      this->BKSingularGmodLRoots.AddObjectOnTopNoRepetitionOfObject
        (nonLroots.TheObjects[i]);
  }
  nonLroots.ComputeDebugString();
  nonLroots.intersectWith(this->nonBKSingularGmodLRoots, nonLnonSingularRoots);
  if (nonLnonSingularRoots.size>0)
  { nonLnonSingularRoots.ComputeDebugString();
  }
  for(int i=oldsize;i<tempRoots.size;i++)
		this->NilradicalRoots.AddObjectOnTop(tempRoots.TheObjects[i]);
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
  if (!this->SatisfyNonBKSingularRoots(theWeyl, TheGlobalVariables))
		return false;
  for (int i=0;i<this->PositiveKroots.size;i++)
	{	this->nonNilradicalRoots.AddObjectOnTopNoRepetitionOfObject
      ( this->PositiveKroots.TheObjects[i]);
		this->nonNilradicalRoots.AddObjectOnTopNoRepetitionOfObject
      (-this->PositiveKroots.TheObjects[i]);
		this->nonBKSingularGmodLRoots.AddObjectOnTopNoRepetitionOfObject
      ( this->PositiveKroots.TheObjects[i]);
		this->nonBKSingularGmodLRoots.AddObjectOnTopNoRepetitionOfObject
      (-this->PositiveKroots.TheObjects[i]);
	}
	this->nonAlphas.AddRootSnoRepetition(this->nonBKSingularGmodLRoots);
	this->nonBetas.AddRootSnoRepetition(this->nonNilradicalRoots);
//	this->ComputeDebugString(theWeyl, TheGlobalVariables);
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

bool minimalRelationsProverState::SatisfyNonBKSingularRoots
	(WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{	root theRoot,tempRoot;
	this->nonBKSingularGmodLRoots.ComputeDebugString();
//	this->ComputeDebugString(theWeyl, TheGlobalVariables);
	for (int i=0;i<this->nonBKSingularGmodLRoots.size;i++)
	{ theRoot.Assign(this->nonBKSingularGmodLRoots.TheObjects[i]);
		if (	this->nonNilradicalRoots.ContainsObject(theRoot) &&
					this->nonKRoots.ContainsObject(theRoot))
		{	int LastFoundIndex=-1;
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
			{	this->StateIsPossible=false;
				return false;
			}
			if (NumFoundIndices==1)
				this->PositiveKroots.AddObjectOnTopNoRepetitionOfObject
					(theWeyl.RootSystem.TheObjects[LastFoundIndex]);
		}
	}
	return true;
}

bool minimalRelationsProverState::ComputeStateReturnFalseIfDubious
  ( GlobalVariables& TheGlobalVariables,  WeylGroup& theWeyl)
{ this->StateIsPossible=true;
//  this->StateIsDubious=false;
  this->NilradicalRoots.AddRootSnoRepetition(this->PartialRelation.Betas);
  this->BKSingularGmodLRoots.AddRootSnoRepetition(this->PartialRelation.Alphas);
  SelectionWithMaxMultiplicity selBetas, selAlphas;
  selBetas.initMe2(this->PartialRelation.Betas.size,1);
  selAlphas.initMe2(this->PartialRelation.Alphas.size,1);
  int NumAlphas= MathRoutines::KToTheNth
		(selAlphas.MaxMultiplicity+1,this->PartialRelation.Alphas.size);
  int NumBetas=MathRoutines::KToTheNth
		(selBetas.MaxMultiplicity+1,this->PartialRelation.Betas.size);
//  this->ComputeDebugString(theWeyl, TheGlobalVariables);
  for (int i=0;i<NumAlphas;i++)
  {	for (int j=0;j<NumBetas;j++)
		{ if (this->CanBeShortened(this->PartialRelation, selAlphas,selBetas,theWeyl))
      {	this->StateIsPossible=false;
        return false;
      }
      selBetas.IncrementSubset();
      this->MakeProgressReportCanBeShortened(i*NumBetas+j,NumAlphas*NumBetas,TheGlobalVariables);
    }
    selAlphas.IncrementSubset();
  }
//  this->ComputeDebugString(theWeyl, TheGlobalVariables);
  if (!this->ComputeCommonSenseImplicationsReturnFalseIfContradiction(theWeyl, TheGlobalVariables))
	{	this->StateIsPossible=false;
		return false;
  }
//  this->ComputeDebugString(theWeyl, TheGlobalVariables);
 	roots possibleAlphas, possibleBetas;
	this->GetPossibleAlphasAndBetas(possibleAlphas, possibleBetas, theWeyl);
	if (!roots::ConesIntersect
				(	TheGlobalVariables,possibleAlphas,possibleBetas,theWeyl.KillingFormMatrix.NumRows))
	{	this->StateIsPossible=false;
		return false;
	}
  this->ComputeDebugString(theWeyl, TheGlobalVariables);
  return true;
}

void minimalRelationsProverState::MakeProgressReportCanBeShortened
	(int checked, int outOf, GlobalVariables &theGlobalVariables)
{ std::stringstream out5;
	out5<<checked+1 << " checked out of " << outOf;
	::IndicatorWindowGlobalVariables.ProgressReportString5=out5.str();
	::IndicatorWindowGlobalVariables.String1NeedsRefresh=false;
	::IndicatorWindowGlobalVariables.String2NeedsRefresh=false;
	::IndicatorWindowGlobalVariables.String3NeedsRefresh=false;
	::IndicatorWindowGlobalVariables.String4NeedsRefresh=false;
	::IndicatorWindowGlobalVariables.String5NeedsRefresh=true;
	if (theGlobalVariables.FeedDataToIndicatorWindowDefault!=0)
		theGlobalVariables.FeedDataToIndicatorWindowDefault(IndicatorWindowGlobalVariables);
}

void minimalRelationsProverState::GetPossibleAlphasAndBetas
	(roots& outputAlphas, roots& outputBetas, WeylGroup& theWeyl)
{	outputBetas.size=0; outputAlphas.size=0;
	for (int i=0;i<theWeyl.RootSystem.size;i++)
	{ if (!this->nonBKSingularGmodLRoots.ContainsObject(theWeyl.RootSystem.TheObjects[i]))
			outputAlphas.AddObjectOnTop(theWeyl.RootSystem.TheObjects[i]);
		if (!this->nonBetas.ContainsObject(theWeyl.RootSystem.TheObjects[i]))
			outputBetas.AddObjectOnTop(theWeyl.RootSystem.TheObjects[i]);
	}
}

bool minimalRelationsProverState::CanBeShortened
  (	coneRelation& theRelation, SelectionWithMaxMultiplicity& selAlphas,
		SelectionWithMaxMultiplicity& selBetas, WeylGroup& theWeyl)
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
    if (selAlphas.elements.size>1)
    { this->nonBKSingularGmodLRoots.AddObjectOnTopNoRepetitionOfObject(MinusCandidate);
//      this->StateIsDubious=true;
      if (this->PartialRelation.Alphas.ContainsObject(MinusCandidate))
        return true;
    }
    if (selAlphas.elements.size==1)
    { this->nonPositiveKRoots.AddObjectOnTopNoRepetitionOfObject(MinusCandidate);
			this->nonPositiveKRoots.AddObjectOnTopNoRepetitionOfObject(Candidate);
    }
  }
  return false;
}

void minimalRelationsProverState::Assign(const minimalRelationsProverState& right)
{ this->BKSingularGmodLRoots.CopyFromBase(right.BKSingularGmodLRoots);
  this->nonBKSingularGmodLRoots.CopyFromBase(right.nonBKSingularGmodLRoots);
  this->PositiveKroots.CopyFromBase(right.PositiveKroots);
	this->nonPositiveKRoots.CopyFromBase(right.nonPositiveKRoots);
	this->NilradicalRoots.CopyFromBase(right.NilradicalRoots);
	this->nonNilradicalRoots.CopyFromBase(right.nonNilradicalRoots);
	this->nonKRoots.CopyFromBase(right.nonKRoots);
  this->nonAlphas.CopyFromBase(right.nonAlphas);
  this->nonBetas.CopyFromBase(right.nonBetas);
  this->owner=right.owner;
  this->PartialRelation=right.PartialRelation;
  this->StateIsPossible=right.StateIsPossible;
  this->theScalarProducts.Assign(right.theScalarProducts);
  this->StateIsComplete=right.StateIsComplete;
}

void minimalRelationsProverStates::MakeProgressReportCurrentState
	(	int index, GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl)
{	::IndicatorWindowGlobalVariables.StatusString1=this->TheObjects[index].DebugString;
	::IndicatorWindowGlobalVariables.StatusString1NeedsRefresh=true;
	if (TheGlobalVariables.FeedDataToIndicatorWindowDefault!=0)
		TheGlobalVariables.FeedDataToIndicatorWindowDefault(::IndicatorWindowGlobalVariables);
	::IndicatorWindowGlobalVariables.StatusString1NeedsRefresh=false;
}

void minimalRelationsProverStates::MakeProgressReportIsos
	(int numSearched, int outOf, GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl)
{	std::stringstream out3;
	out3 <<"Searching for automorphisms: "<< numSearched+1<<" out of "<< outOf;
	::IndicatorWindowGlobalVariables.String3NeedsRefresh=true;
	::IndicatorWindowGlobalVariables.String1NeedsRefresh=false;
	::IndicatorWindowGlobalVariables.String2NeedsRefresh=false;
	::IndicatorWindowGlobalVariables.String4NeedsRefresh=false;
	::IndicatorWindowGlobalVariables.String5NeedsRefresh=false;
	::IndicatorWindowGlobalVariables.ProgressReportString3=out3.str();
	if (TheGlobalVariables.FeedDataToIndicatorWindowDefault!=0)
		TheGlobalVariables.FeedDataToIndicatorWindowDefault(::IndicatorWindowGlobalVariables);
}

void minimalRelationsProverStates::MakeProgressReportChildStates
	(int numSearched, int outOf, int NewFound, GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl)
{	this->MakeProgressReportStack(TheGlobalVariables, theWeyl);
	std::stringstream out4;
	out4	<<"Computing children states: "<< numSearched+1<<" out of "<< outOf << "; "
				<< this->TheObjects[*this->theIndexStack.LastObject()].childStates.size
				<<" possible out of " <<NewFound<<" new states";
	::IndicatorWindowGlobalVariables.String4NeedsRefresh=true;
	::IndicatorWindowGlobalVariables.String1NeedsRefresh=false;
	::IndicatorWindowGlobalVariables.String2NeedsRefresh=false;
	::IndicatorWindowGlobalVariables.String3NeedsRefresh=false;
	::IndicatorWindowGlobalVariables.String5NeedsRefresh=false;
	::IndicatorWindowGlobalVariables.ProgressReportString4=out4.str();
	if (TheGlobalVariables.FeedDataToIndicatorWindowDefault!=0)
		TheGlobalVariables.FeedDataToIndicatorWindowDefault(::IndicatorWindowGlobalVariables);
}

void minimalRelationsProverStates::MakeProgressReportStack
	( GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl)
{	std::stringstream out1, out2, out3, out4;
	std::stringstream* tempOut1=&out1;
	for (int i=0;i<this->theIndexStack.size;i++)
	{ int currentIndex=this->theIndexStack.TheObjects[i];
	  (*tempOut1)	<< currentIndex << ": " << this->TheObjects[currentIndex].activeChild+1
                << " of "<< this->TheObjects[currentIndex].childStates.size	<<"; ";
		if (i==4)	tempOut1=&out2;
		if (i==8)	tempOut1=&out3;
	//	if (i==12)	tempOut1=&out4;
	}
	if (theIndexStack.size!=0)
    (*tempOut1)
				<< this->TheObjects[*this->theIndexStack.LastObject()].PartialRelation.Alphas.size
        <<" alphas + ... = "
        <<this->TheObjects[*this->theIndexStack.LastObject()].PartialRelation.Betas.size
        <<" betas + ...";
	::IndicatorWindowGlobalVariables.ProgressReportString1=out1.str();
	::IndicatorWindowGlobalVariables.ProgressReportString2=out2.str();
	::IndicatorWindowGlobalVariables.ProgressReportString3=out3.str();
	::IndicatorWindowGlobalVariables.ProgressReportString4=out4.str();
	::IndicatorWindowGlobalVariables.String1NeedsRefresh=true;
	::IndicatorWindowGlobalVariables.String2NeedsRefresh=true;
	::IndicatorWindowGlobalVariables.String3NeedsRefresh=true;
	::IndicatorWindowGlobalVariables.String4NeedsRefresh=true;
	::IndicatorWindowGlobalVariables.String5NeedsRefresh=false;
	if (TheGlobalVariables.FeedDataToIndicatorWindowDefault!=0)
		TheGlobalVariables.FeedDataToIndicatorWindowDefault(::IndicatorWindowGlobalVariables);
}

bool minimalRelationsProverState::RootIsGoodForProblematicIndex
	(	root& input,int problemIndex, bool AddingAlphas, bool NeedPositiveContribution,
		roots& theDualBasis, WeylGroup& theWeyl)
{	if (AddingAlphas)
	{	if (this->nonAlphas.ContainsObject(input))
			return false;
	}else
	{ if (this->nonBetas.ContainsObject(input))
			return false;
	}
	Rational tempRat=
    theWeyl.RootScalarKillingFormMatrixRoot(input,theDualBasis.TheObjects[problemIndex]);
	if (NeedPositiveContribution &&  tempRat.IsPositive())
		return true;
	if (!NeedPositiveContribution && tempRat.IsNegative())
		return true;
	return false;
}

bool minimalRelationsProverState::RootIsGoodForPreferredSimpleRoot
	(	root &input, int preferredIndex, bool &GoodForAlpha, WeylGroup &theWeyl,
    GlobalVariables &TheGlobalVariables, root& AlphasMinusBetas)
{	Rational tempRat1, tempRat2;
	tempRat2=AlphasMinusBetas.TheObjects[preferredIndex]; tempRat2.AssignAbsoluteValue();
	tempRat1= AlphasMinusBetas.TheObjects[preferredIndex]-input.TheObjects[preferredIndex];
	tempRat1.AssignAbsoluteValue();
	if (tempRat2.IsGreaterThan(tempRat1))
		if (!this->nonBetas.ContainsObject(input))
		{	GoodForAlpha=false;
			return true;
		}
	tempRat1=AlphasMinusBetas.TheObjects[preferredIndex]+input.TheObjects[preferredIndex];
	tempRat1.AssignAbsoluteValue();
	if (tempRat2.IsGreaterThan(tempRat1))
		if (!this->nonBKSingularGmodLRoots.ContainsObject(input))
		{	GoodForAlpha=true;
			return true;
		}
	return false;
}

minimalRelationsProverState::minimalRelationsProverState()
{ this->StateIsComplete=false;
	this->activeChild=-1;
}

bool minimalRelationsProverState::IsAGoodPosRootsKChoice
  (WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{ theWeyl.GenerateAdditivelyClosedSubset(this->PositiveKroots,this->PositiveKroots);
  for (int i=0;i<this->BKSingularGmodLRoots.size;i++)
    if (!this->IsBKSingularNonImplied(this->BKSingularGmodLRoots.TheObjects[i],theWeyl))
      return false;
  return true;
}


/*void minimalRelationsProverStates::RemoveDoubt
  (int index, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{ //roots& tempPosRootsK=TheGlobalVariables.rootsRemoveDoubt;
  //tempPosRootsK.size=0;
  //tempPosRootsK.AddListOnTop(this->PositiveKroots);
  for (int i=0;i<this->TheObjects[index].nonBKSingularRoots.size;i++)
    if (this->TheObjects[index].IsBKSingularNonImplied
					(this->TheObjects[index].nonBKSingularRoots.TheObjects[i], theWeyl))
      for (int j=0;j<this->TheObjects[index].UndecidedRoots.size;j++)
      { int oldsize=this->TheObjects[index].PositiveKroots.size;
        this->TheObjects[index].PositiveKroots.AddObjectOnTop
					(this->TheObjects[index].UndecidedRoots.TheObjects[i]);
        if (this->TheObjects[index].IsAGoodPosRootsKChoice(theWeyl, TheGlobalVariables))
          this->ExtensionStep(index, theWeyl,TheGlobalVariables,this->TheObjects[index]);
        this->TheObjects[index].PositiveKroots.size=oldsize;
      }
}*/

bool minimalRelationsProverState::FindBetaWithoutTwoAlphas
  (root& outputBeta, roots& inputBetas, roots& inputAlphas, WeylGroup& theWeyl)
{ for (int i=0;i<inputBetas.size;i++)
    if (inputAlphas.NumRootsConnectedTo(inputBetas.TheObjects[i],theWeyl)<2){
      outputBeta.Assign(inputBetas.TheObjects[i]);
      return true;    }
  return false;
}

bool minimalRelationsProverStates::StateIsEqualTo
  ( minimalRelationsProverState& theState, int IndexOther, WeylGroup& theWeyl,
    GlobalVariables& TheGlobalVariables)
{	return this->ExtendToIsomorphismRootSystem
		(theState,IndexOther,TheGlobalVariables,theWeyl);
}

bool minimalRelationsProverStates::AddObjectOnTopNoRepetitionOfObject
  ( minimalRelationsProverState& theState, WeylGroup& theWeyl,
    GlobalVariables& TheGlobalVariables)
{ theState.SortAlphasAndBetas(TheGlobalVariables, theWeyl);
	theState.ComputeScalarProductsMatrix(TheGlobalVariables,theWeyl);
	theState.theScalarProducts.ComputeDebugString();
  for (int i=0;i<this->size;i++)
    if (this->StateIsEqualTo(theState,i,theWeyl,TheGlobalVariables))
      return false;
  this->AddObjectOnTop(theState);
  return true;
}

void minimalRelationsProverState::ComputeScalarProductsMatrix
  (GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl)
{ return this->ComputeScalarProductsMatrix
		(	TheGlobalVariables, theWeyl, this->PartialRelation.Alphas,
			this->PartialRelation.Betas, this->theScalarProducts);
}

void minimalRelationsProverState::ComputeScalarProductsMatrix
	(	GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl, roots& theAlphas,
		roots& theBetas, MatrixLargeRational& output)
{ roots& tempRoots=TheGlobalVariables.rootsProverStateComputation1;
  tempRoots.size=0;
  tempRoots.AddListOnTop(theAlphas);
  tempRoots.AddListOnTop(theBetas);
  output.init(tempRoots.size, tempRoots.size);
  for (int i=0;i<tempRoots.size;i++)
    for (int j=0;j<tempRoots.size;j++)
      theWeyl.RootScalarKillingFormMatrixRoot
				(tempRoots.TheObjects[i], tempRoots.TheObjects[j],output.elements[i][j]);
}


void minimalRelationsProverState::ElementToString
	( std::string& output, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{ std::string tempS; std::stringstream out;
  this->PartialRelation.Alphas.ElementToLinearCombinationString(tempS);
  out << tempS <<"+ ... = ";
  this->PartialRelation.Betas.ElementToLinearCombinationString(tempS);
  out << tempS <<" + ...\r\n";
	root tempRoot;
	for(int i=0;i<this->PartialRelation.Alphas.size;i++)
	{	theWeyl.GetEpsilonCoords(this->PartialRelation.Alphas.TheObjects[i],tempRoot,TheGlobalVariables);
		tempRoot.ElementToStringEpsilonForm(tempS,false,false);
		out << tempS <<" + ";
	}
	out <<"... = ";
	for(int i=0;i<this->PartialRelation.Betas.size;i++)
	{	theWeyl.GetEpsilonCoords(this->PartialRelation.Betas.TheObjects[i],tempRoot,TheGlobalVariables);
		tempRoot.ElementToStringEpsilonForm(tempS,false,false);
		out << tempS <<" + ";
	}
	out <<"...";
  this->BKSingularGmodLRoots.ElementToString(tempS);
  out <<"\r\nBK singular g mod l roots: "<< tempS;
  this->nonAlphas.ElementToString(tempS);
  out <<"\r\nNon-alphas: "<< tempS;
  this->nonBetas.ElementToString(tempS);
  out <<"\r\nNon-betas: "<< tempS;
  this->nonBKSingularGmodLRoots.ElementToString(tempS);
  out <<"\r\nnon BK-singular g mod l roots: "<< tempS;
  this->NilradicalRoots.ElementToString(tempS);
  out << "\r\nNilradical roots: "<< tempS;
  this->nonNilradicalRoots.ElementToString(tempS);
  out << "\r\nnon-nilradical roots: "<< tempS;
  this->PositiveKroots.ElementToString(tempS);
  out << "\r\nPositive roots of K: "<< tempS;
  this->nonPositiveKRoots.ElementToString(tempS);
  out <<"\r\nNon-positive roots of K: "<< tempS;
  if (!this->StateIsPossible)
		out << "\r\n I am Impossible!";
  if (this->StateIsComplete)
		out <<"\r\nI am complete!";
  out<<"\r\n\n";
  output=out.str();
}

void minimalRelationsProverStates::ElementToString
	(std::string& output,WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{ std::string tempS; std::stringstream out;
	for (int i=0;i<this->size;i++)
	{	this->TheObjects[i].ElementToString(tempS,theWeyl, TheGlobalVariables);
		out <<tempS<<"\n\n";
	}
	output=out.str();
}

void minimalRelationsProverStates::GenerateStates
  ( ComputationSetup& theSetup, GlobalVariables& TheGlobalVariables, char WeylLetter,
    int theDimension)
{ minimalRelationsProverState tempState;
	WeylGroup theWeyl;
	theWeyl.MakeArbitrary(WeylLetter, theDimension);
  theWeyl.ComputeRho(false);
  this->isomorphismComputer.AmbientWeyl.Assign(theWeyl);
  this->isomorphismComputer.ComputeAll();
  this->PreferredDualBasis.size=0;
  /*this->PreferredDualBasis.SetSizeExpandOnTopNoObjectInit(theDimension);
  for (int i=0;i<theDimension;i++)
		this->PreferredDualBasis.TheObjects[i].MakeZero(theDimension);
	if (WeylLetter=='D')
		for (int i=0;i<theDimension;i++)
		{	for (int j=i;j<theDimension-2;j++)
				this->PreferredDualBasis.TheObjects[i].TheObjects[j]=2;
			this->PreferredDualBasis.TheObjects[i].TheObjects[theDimension-1]=1;
			this->PreferredDualBasis.TheObjects[i].TheObjects[theDimension-2]=1;
			if (i==theDimension-1)
				this->PreferredDualBasis.TheObjects[i].TheObjects[theDimension-2]=-1;
		}
	this->PreferredDualBasis.ComputeDebugString();*/

  root tempRoot;
  tempRoot.Assign(theWeyl.RootSystem.TheObjects[0]);
  tempState.PartialRelation.Alphas.AddObjectOnTop(tempRoot);
  for (int i=0;i<theWeyl.RootSystem.size;i++)
		if (i!=0 && theWeyl.RootScalarKillingFormMatrixRoot
          (tempRoot, theWeyl.RootSystem.TheObjects[i]).IsPositive() )
		{	tempState.PartialRelation.Betas.AddObjectOnTop(theWeyl.RootSystem.TheObjects[i]);
			break;
		}
  this->size=0;
  this->AddObjectOnTop(tempState);
	this->LastObject()->ComputeStateReturnFalseIfDubious(TheGlobalVariables,theWeyl);
//	this->LastObject()->UndecidedRoots.CopyFromBase(theWeyl.RootSystem);
//	this->LastObject()->UndecidedRoots.PopFirstOccurenceObjectSwapWithLast(tempRoot);
//	tempRoot.MinusRoot();
//	this->LastObject()->UndecidedRoots.PopFirstOccurenceObjectSwapWithLast(tempRoot);
	this->LastObject()->ComputeScalarProductsMatrix(TheGlobalVariables, theWeyl);
	this->theIndexStack.AddObjectOnTop(0);
	this->LastObject()->ComputeDebugString(theWeyl,TheGlobalVariables);
//	this->LastObject()->theScalarProducts.ComputeDebugString();
  this->Extend(0,0,theWeyl, TheGlobalVariables);
  this->ComputeDebugString(theWeyl,TheGlobalVariables);
  theSetup.theRootSubalgebras.DebugString=this->DebugString;
}

void ComputationSetup::RunCustom()
{	this->DoTheRootSAComputationCustom();
//    this->theRootSubalgebras.ComputeDebugString(true);
  this->theOutput.DebugString.append(	"\\documentclass{article}\n\\usepackage{amssymb}\n");
	this->theOutput.DebugString.append("\\addtolength{\\hoffset}{-3.5cm}\\addtolength{\\textwidth}{7cm}");
	this->theOutput.DebugString.append("\\addtolength{\\voffset}{-3.5cm}\\addtolength{\\textheight}{7cm}");
	this->theOutput.DebugString.append("\\begin{document}~");
	this->theRootSubalgebras.theBadRelations.ComputeDebugString
		(this->theRootSubalgebras,*this->theGlobalVariablesContainer->Default());
	this->theRootSubalgebras.theGoodRelations.ComputeDebugString
		(this->theRootSubalgebras,*this->theGlobalVariablesContainer->Default());
	//this->theRootSubalgebras.theMinRels.ComputeDebugString
	//	(this->theRootSubalgebras,*this->theGlobalVariablesContainer->Default());
	this->theOutput.DebugString.append(this->theRootSubalgebras.DebugString);
	this->theOutput.DebugString.append("\n\n\n");
	if (this->theRootSubalgebras.theGoodRelations.size!=0)
    this->theOutput.DebugString.append(this->theRootSubalgebras.theGoodRelations.DebugString);
	this->theOutput.DebugString.append("\n\n\n");
	if (this->theRootSubalgebras.theBadRelations.size>0)
	{	this->theOutput.DebugString.append("The bad relations: \n\n");
		this->theOutput.DebugString.append
			(this->theRootSubalgebras.theBadRelations.DebugString);
	}
	if (this->theRootSubalgebras.flagLookingForMinimalRels)
	{ this->theOutput.DebugString.append("\n\nMinimal relations: \n\n");
		this->theOutput.DebugString.append(this->theRootSubalgebras.theMinRels.DebugString);
	}
  this->theOutput.DebugString.append("\\end{document}");
}


void ComputationSetup::DoTheRootSAComputationCustom()
{	//rootSubalgebra theRootSA, theRootSA2;
  //MatrixLargeRational tempMat,tempB, tempX;
  //tempMat.init(3,4); tempB.init(3,1);
  //tempMat.elements[0][0]=0;tempMat.elements[0][1]=80;tempMat.elements[0][2]=1;tempMat.elements[0][3]=0;
  //tempMat.elements[1][0]=1;tempMat.elements[1][1]=96;tempMat.elements[1][2]=0;tempMat.elements[1][3]=0;
  //tempMat.elements[2][0]=0;tempMat.elements[2][1]=95;tempMat.elements[2][2]=1;tempMat.elements[2][3]=0;
  //tempB.elements[0][0]=5;
  //tempB.elements[1][0]=6;
  //tempB.elements[2][0]=6;
  std::stringstream out;
  //if (MatrixLargeRational::Solve_Ax_Equals_b_ModifyInputReturnFirstSolutionIfExists(tempMat,tempB,tempX))
   // out <<"has solution\n\n";
  //else
   // out<<"no solution\n\n";
  //tempX.ComputeDebugString();
  //out<<tempX.DebugString;
  //this->theRootSubalgebras.DebugString=out.str();
  //return;
 /* this->theChevalleyConstantComputer.ComputeChevalleyConstants
    ('A',2 , *this->theGlobalVariablesContainer->Default());
  this->theChevalleyConstantComputer.ComputeDebugString
		(false,true,*this->theGlobalVariablesContainer->Default());*/
  /*out <<"\n\n"<< this->theChevalleyConstantComputer.DebugString<<"\n\n";

  ElementSimpleLieAlgebra e,f;
  e.Nullify(this->theChevalleyConstantComputer);
  e.ComputeDebugString(this->theChevalleyConstantComputer,false, true);
  root root1;
  root1.SetSizeExpandOnTopLight
    (this->theChevalleyConstantComputer.theWeyl.KillingFormMatrix.NumRows);
  root1.TheObjects[0]=1;root1.TheObjects[1]=0;
  e.SetCoefficient(root1,1,this->theChevalleyConstantComputer);
  root1.TheObjects[0]=0;root1.TheObjects[1]=1;

  return;*/
  //this->theSltwoSubalgebras.Compute(*this->theGlobalVariablesContainer->Default(),false);
	//this->theRootSubalgebras.DebugString=this->theSltwoSubalgebras.DebugString;
	this->theChevalleyConstantComputer.FindSl2Subalgebras
    ('E',8,*this->theGlobalVariablesContainer->Default(),this->theSltwoSubalgebras);
//	this->theRootSubalgebras.DebugString=this->theChevalleyConstantComputer.DebugString;
	//return;

	/*this->theChevalleyConstantComputer.ComputeDebugString
		(false,true,*this->theGlobalVariablesContainer->Default());
  out <<this->theChevalleyConstantComputer.DebugString;
  */
  this->theRootSubalgebras.DebugString=this->theChevalleyConstantComputer.DebugString;
	return;
  minimalRelationsProverStates tempProver;
  WeylGroup tempWeyl;
  tempWeyl.MakeDn(5);
  tempProver.GenerateStates(*this,*this->theGlobalVariablesContainer->Default(),'D',6);
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

bool minimalRelationsProverStates::ExtendToIsomorphismRootSystem
	(	minimalRelationsProverState& theState, int indexOther,
		GlobalVariables& theGlobalVariables, WeylGroup& theWeyl)
{	//return true;
  theState.SortAlphasAndBetas(theGlobalVariables, theWeyl);
	minimalRelationsProverState& theOtherState=this->TheObjects[indexOther];
	//rootSubalgebra& theRootSA=theGlobalVariables.rootSAProverIsos;
	roots& theAlphas = theState.PartialRelation.Alphas;
	roots& theBetas  = theState.PartialRelation.Betas;
	roots& theOtherAlphas = theOtherState.PartialRelation.Alphas;
	roots& theOtherBetas  = theOtherState.PartialRelation.Betas;
	if (theAlphas.size!=theOtherAlphas.size)
		return false;
	if (theBetas.size!=theOtherBetas.size)
		return false;
	rootsCollection& isotypicAlphaPieces=theGlobalVariables.rootsExtendToIsomorphismRootSystem1;
	rootsCollection& isotypicBetaPieces=theGlobalVariables.rootsExtendToIsomorphismRootSystem2;
	isotypicAlphaPieces.MakeActualSizeAtLeastExpandOnTop(theAlphas.size);
	isotypicBetaPieces.MakeActualSizeAtLeastExpandOnTop(theBetas.size);
	isotypicAlphaPieces.SetSizeExpandOnTopNoObjectInit(1);
	isotypicAlphaPieces.TheObjects[0].size=0;
	isotypicAlphaPieces.TheObjects[0].AddObjectOnTop(theAlphas.TheObjects[0]);
	ListBasicObjects<int> tempList;
	tempList.size=0;
	permutation thePermAlphas, thePermBetas;
	for (int i=1;i<theAlphas.size;i++)
	{ if (theState.Root1IsGreaterThanRoot2
					(i-1,i,theAlphas,theBetas,theGlobalVariables,theWeyl))
		{	tempList.AddObjectOnTop(isotypicAlphaPieces.LastObject()->size);
			isotypicAlphaPieces.SetSizeExpandOnTopNoObjectInit(isotypicAlphaPieces.size+1);
			isotypicAlphaPieces.LastObject()->size=0;
		}
		isotypicAlphaPieces.LastObject()->AddObjectOnTop(theAlphas.TheObjects[i]);
	}
	tempList.AddObjectOnTop(isotypicAlphaPieces.LastObject()->size);
	thePermAlphas.initPermutation(tempList,theAlphas.size);
	isotypicBetaPieces.SetSizeExpandOnTopNoObjectInit(1);
	isotypicBetaPieces.TheObjects[0].size=0;
	isotypicBetaPieces.TheObjects[0].AddObjectOnTop(theBetas.TheObjects[0]);
	tempList.size=0;
	for (int i=1;i<theBetas.size;i++)
	{ if (theState.Root1IsGreaterThanRoot2
					(i-1,i,theBetas,theAlphas,theGlobalVariables,theWeyl))
		{	tempList.AddObjectOnTop(isotypicBetaPieces.LastObject()->size);
			isotypicBetaPieces.SetSizeExpandOnTopNoObjectInit(isotypicBetaPieces.size+1);
			isotypicBetaPieces.LastObject()->size=0;
		}
		isotypicBetaPieces.LastObject()->AddObjectOnTop(theBetas.TheObjects[i]);
	}
	tempList.AddObjectOnTop(isotypicBetaPieces.LastObject()->size);
	thePermBetas.initPermutation(tempList,theBetas.size);
	int NumCyclesAlphas = thePermAlphas.getTotalNumSubsets();
	int NumCyclesBetas  = thePermBetas.getTotalNumSubsets();
	roots theDomain, thePermutedAlphas, thePermutedBetas, theRange;
	theRange.size=0;
	theRange.AddListOnTop(theOtherAlphas); theRange.AddListOnTop(theOtherBetas);
	MatrixLargeRational theOtherMatrix, thisMatrix;
	this->TheObjects[indexOther].ComputeScalarProductsMatrix
		(theGlobalVariables, theWeyl, theOtherAlphas, theOtherBetas, theOtherMatrix);
	this->TheObjects[indexOther].theScalarProducts.ComputeDebugString();
	bool DomainAndRangeGenerateNonIsoSAs;
	for(int i=0;i<NumCyclesAlphas;i++)
	{	theDomain.size=0;
		thePermutedAlphas.size=0;
		thePermAlphas.GetPermutation(tempList);
		for (int k=0;k<tempList.size;k++)
		{	root& tempRoot=theAlphas.TheObjects[tempList.TheObjects[k]];
			thePermutedAlphas.AddObjectOnTop(tempRoot);
			theDomain.AddObjectOnTop(tempRoot);
		}
		theDomain.ComputeDebugString();
		for (int j=0;j<NumCyclesBetas;j++)
		{	theDomain.size=theAlphas.size;
			thePermutedBetas.size=0;
			thePermBetas.GetPermutation(tempList);
			for (int k=0;k<tempList.size;k++)
			{	root& tempRoot=theBetas.TheObjects[tempList.TheObjects[k]];
				thePermutedBetas.AddObjectOnTop(tempRoot);
				theDomain.AddObjectOnTop(tempRoot);
			}
			theDomain.ComputeDebugString();
			this->MakeProgressReportIsos
				(i*NumCyclesBetas+j,NumCyclesBetas*NumCyclesAlphas,theGlobalVariables,theWeyl);
			theState.ComputeScalarProductsMatrix
				(theGlobalVariables, theWeyl,thePermutedAlphas,thePermutedBetas,thisMatrix);
			thisMatrix.ComputeDebugString();

			if (thisMatrix.IsEqualTo(theOtherMatrix))
			{	if (this->isomorphismComputer.attemptExtensionToIsomorphism
							(theDomain, theRange,	theGlobalVariables, 0, false, theWeyl, &DomainAndRangeGenerateNonIsoSAs))
				{ theDomain.ComputeDebugString();
					theRange.ComputeDebugString();
					return true;
				} else
				{ if (DomainAndRangeGenerateNonIsoSAs)
						return false;
				}
			}
			thePermBetas.IncrementSubset();
		}
		thePermAlphas.IncrementSubset();
	}
	//theSA.attemptExtensionToIsomorphismNoCentralizer
	return false;
}

bool 	minimalRelationsProverStates::GetSeparatingRootIfExists
	(	roots& ConeStrictlyPositive, roots& ConeNonNegative, root& output, WeylGroup& theWeyl,
		GlobalVariables& TheGlobalVariables)
{ for (int i=0;i<theWeyl.RootSystem.size;i++)
	{ bool isGood=true;
		root& Candidate=theWeyl.RootSystem.TheObjects[i];
		for (int j=0; j<ConeStrictlyPositive.size;j++)
			if ( theWeyl.RootScalarKillingFormMatrixRoot
						(ConeStrictlyPositive.TheObjects[j],Candidate).IsNonPositive())
			{ isGood=false;
				break;
			}
		if (isGood)
			for (int j=0; j<ConeNonNegative.size;j++)
				if ( theWeyl.RootScalarKillingFormMatrixRoot
							(ConeNonNegative.TheObjects[j],Candidate).IsPositive())
				{ isGood=false;
					break;
				}
		if (isGood)
		{	output.Assign(Candidate);
			output.ComputeDebugString();
			return true;
		}
	}
	return false;
}


bool rootSubalgebra::attemptExtensionToIsomorphism
	( roots& Domain, roots& Range, GlobalVariables& theGlobalVariables,
		ReflectionSubgroupWeylGroup* outputAutomorphisms, bool actOnCentralizerOnly,
		WeylGroup& theWeyl, bool *DomainAndRangeGenerateNonIsoSAs)
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
	if (theDomainRootSA.theDynkinDiagram.DebugString!=
				theRangeRootSA.theDynkinDiagram.DebugString ||
			theDomainRootSA.theCentralizerDiagram.DebugString!=
				theRangeRootSA.theCentralizerDiagram.DebugString)
  {	if (DomainAndRangeGenerateNonIsoSAs!=0)
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
	for (int i=0;i<CentralizerDiagramAutomorphisms.size;i++)
		tempAutosCentralizer.MaxMultiplicities.TheObjects[i]=
			CentralizerDiagramAutomorphisms.TheObjects[i].size-1;
  tempList.SetSizeExpandOnTopNoObjectInit
    (theDomainRootSA.theCentralizerDiagram.sameTypeComponents.size);
  int tempSize=0;
  for (int i=0;i<theDomainRootSA.theCentralizerDiagram.sameTypeComponents.size;i++)
  { tempList.TheObjects[i]=
			theDomainRootSA.theCentralizerDiagram.sameTypeComponents.TheObjects[i].size;
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
  {	for(int l=0;l<NumAutosCentralizer;l++)
		{	isoDomain.size=givenSize; isoRange.size=givenSize;
			theDomainRootSA.theCentralizerDiagram.GetMapFromPermutation
				(	isoDomain,isoRange,tempPermutation2,CentralizerDiagramAutomorphisms,
					tempAutosCentralizer, theRangeRootSA.theCentralizerDiagram);
			if (theDomainRootSA.attemptExtensionToIsomorphismNoCentralizer
						(	isoDomain, isoRange, theGlobalVariables, 0, outputAutomorphisms,
							false, 0, &Domain, &Range))//GenerateAllAutos))
				if (outputAutomorphisms==0)
					return true;
			if (outputAutomorphisms!=0)
				theDomainRootSA.MakeProgressReportGenAutos
					( l+NumAutosCentralizer*j, tempI2*NumAutosCentralizer,
						outputAutomorphisms->ExternalAutomorphisms.size, theGlobalVariables);
			tempAutosCentralizer.IncrementSubset();
		}
		permComponentsCentralizer.incrementAndGetPermutation(tempPermutation2);
  }
  return false;
}
