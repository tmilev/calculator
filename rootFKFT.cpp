#include "polyhedra.h"
#include "rootFKFT.h"
//to be merged in main file polyhedra.cpp later

int minimalRelationsProverState::ProblemCounter=0;
int ::minimalRelationsProverStates::ProblemCounter=0;
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
{ if (this->IsBKSingularNonImplied(input, theWeyl))
    return true;
  root tempRoot;
  for (int j=0;j<theWeyl.RootSystem.size;j++)
  { tempRoot=input+theWeyl.RootSystem.TheObjects[j];
    if(theWeyl.IsARoot(tempRoot))
    { bool found=false;
      for (int i=0;i<this->BKSingularGmodLRoots.size;i++)
        if (theWeyl.IsARoot
              (this->BKSingularGmodLRoots.TheObjects[i]+theWeyl.RootSystem.TheObjects[j]))
        { found=true;
          break;
        }
      if (!found)
        return false;
    }
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
	for (int j=0;j<theWeyl.RootSystem.size;j++)
	{	for (int i=0;i<this->PartialRelation.Alphas.size;i++)
		{	tempRoot.Assign( this->PartialRelation.Alphas.TheObjects[i]);
			tempRoot.Add(theWeyl.RootSystem.TheObjects[j]);
			if (theWeyl.IsARoot(tempRoot))
				this->nonBKSingularGmodLRoots.AddObjectOnTopNoRepetitionOfObject
          (theWeyl.RootSystem.TheObjects[j]);
		}
		for (int i=0; i<this->PartialRelation.Betas.size;i++)
			if (theWeyl.IsARoot
            (this->PartialRelation.Betas.TheObjects[i]+theWeyl.RootSystem.TheObjects[j]))
				this->nonBetas.AddObjectOnTopNoRepetitionOfObject(theWeyl.RootSystem.TheObjects[j]);
	}
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
  roots& tempRoots2 = TheGlobalVariables.rootsProverStateComputation3;
  this->nonPositiveKRoots.intersectWith(this->nonNilradicalRoots,tempRoots2);
  for (int i=0;i<tempRoots2.size;i++)
  { if (this->IsBKSingularImplied(tempRoots2.TheObjects[i], theWeyl))
      this->BKSingularGmodLRoots.AddObjectOnTopNoRepetitionOfObject
        (tempRoots2.TheObjects[i]);
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
//	this->nonAlphas.AddRootSnoRepetition(this->nonBKSingularGmodLRoots);
	this->nonBetas.AddRootSnoRepetition(this->nonNilradicalRoots);
//	this->ComputeDebugString(theWeyl, TheGlobalVariables);
	if (this->nonNilradicalRoots.HasACommonElementWith(this->NilradicalRoots))
		return false;
	if (this->PositiveKroots.HasACommonElementWith(this->nonPositiveKRoots))
		return false;
	if (this->BKSingularGmodLRoots.HasACommonElementWith(this->nonBKSingularGmodLRoots))
		return false;
	return true;
}

bool minimalRelationsProverState::SatisfyNonBKSingularRoots
	(WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{	root theRoot, MinusTheRoot, tempRoot;
	this->nonBKSingularGmodLRoots.ComputeDebugString();
//	this->ComputeDebugString(theWeyl, TheGlobalVariables);
	for (int i=0;i<this->nonBKSingularGmodLRoots.size;i++)
	{ theRoot.Assign(this->nonBKSingularGmodLRoots.TheObjects[i]);
		MinusTheRoot.Assign(theRoot);
		MinusTheRoot.MinusRoot();
		if (	this->nonNilradicalRoots.ContainsObject(theRoot) &&
					this->nonPositiveKRoots.ContainsObject(theRoot)	&&
					this->nonPositiveKRoots.ContainsObject(MinusTheRoot))
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
  int NumAlphas=MathRoutines::KToTheNth(2,this->PartialRelation.Alphas.size);
  int NumBetas=MathRoutines::KToTheNth(2,this->PartialRelation.Betas.size);
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
	out5<<checked << " checked out of " << outOf;
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
//  this->nonAlphas.CopyFromBase(right.nonAlphas);
  this->nonBetas.CopyFromBase(right.nonBetas);
  this->childStates.CopyFromBase(right.childStates);
  this->owner=right.owner;
  this->PartialRelation=right.PartialRelation;
  this->StateIsPossible=right.StateIsPossible;
  this->theScalarProducts.Assign(right.theScalarProducts);
  this->StateIsComplete=right.StateIsComplete;
}

void minimalRelationsProverStates::MakeProgressReportStack
	( GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl)
{	std::stringstream out1, out2, out3, out4;
	std::stringstream* tempOut1=&out1;
	for (int i=0;i<this->theIndexStack.size;i++)
	{ (*tempOut1)	<< this->theIndexStack.TheObjects[i] << ": "
								<< this->theCounter1Stack.TheObjects[i] << " of " << theWeyl.RootSystem.size
								<<" " << this->theCounter2Stack.TheObjects[i]<< " of "<< theWeyl.RootSystem.size
								<<"; ";
		if (i==4)	tempOut1=&out2;
		if (i==8)	tempOut1=&out3;
		if (i==12)	tempOut1=&out4;
	}
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

void minimalRelationsProverStates::ExtensionStep
  ( int index, int preferredSimpleRootIndex, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables,
    minimalRelationsProverState& newState)
{ this->MakeProgressReportStack(TheGlobalVariables, theWeyl);
	int currentNewIndex=this->size;
	if (this->AddObjectOnTopNoRepetitionOfObject(newState, theWeyl,TheGlobalVariables))
  { this->TheObjects[currentNewIndex].ComputeStateReturnFalseIfDubious
			(TheGlobalVariables, theWeyl);
//		this->ComputeDebugString(theWeyl, TheGlobalVariables);
    if (this->TheObjects[currentNewIndex].StateIsPossible)
    { //if (this->TheObjects[currentNewIndex].StateIsDubious)
      //  this->RemoveDoubt(currentNewIndex,theWeyl,TheGlobalVariables);
      //else
      this->Extend(currentNewIndex,preferredSimpleRootIndex, theWeyl,TheGlobalVariables);
    }
  }
}

void minimalRelationsProverStates::Extend
  (int index, int preferredSimpleRoot, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{ root theBeta, theAlpha, theMinusAlpha, theMinusBeta;
//  this->ComputeDebugString(theWeyl, TheGlobalVariables);
  minimalRelationsProverState newState;
  int theDimension=theWeyl.KillingFormMatrix.NumRows;
  roots& theAlphas = this->TheObjects[index].PartialRelation.Alphas;
  roots& theBetas  = this->TheObjects[index].PartialRelation.Betas;
  int firstProblematicIndex=-1;
  bool AddAlphas=false;
  bool PositiveContribution=false;
	Rational tempRat;
  for (int i=0;i<theDimension;i++)
	{	bool foundPlusAlpha=false;
		bool foundMinusAlpha=false;
		for (int j=0;j<theAlphas.size;j++)
		{	tempRat = theWeyl.RootScalarKillingFormMatrixRoot
				(theAlphas.TheObjects[j],this->PreferredDualBasis.TheObjects[i]);
			if(tempRat.IsPositive())
				foundPlusAlpha=true;
			if(tempRat.IsNegative())
				foundMinusAlpha=true;
		}
		bool foundPlusBeta=false;
		bool foundMinusBeta=false;
		for (int j=0;j<theBetas.size;j++)
		{	tempRat = theWeyl.RootScalarKillingFormMatrixRoot
				(theBetas.TheObjects[j],this->PreferredDualBasis.TheObjects[i]);
			if(tempRat.IsPositive())
				foundPlusBeta=true;
			if(tempRat.IsNegative())
				foundMinusBeta=true;
		}
		bool noProblem = (foundPlusAlpha && foundMinusAlpha)|| (foundPlusBeta && foundMinusBeta);
		bool AlphasHaveNoContribution=(!foundPlusAlpha && !foundMinusAlpha);
		bool BetasHaveNoContribution= (!foundPlusBeta && !foundMinusBeta);
		noProblem=noProblem || (AlphasHaveNoContribution && BetasHaveNoContribution);
		if (!noProblem)
		{	if ( (foundPlusAlpha==foundPlusBeta) && (foundMinusBeta==foundMinusAlpha))
				noProblem=true;
			else
			{ firstProblematicIndex=i;
				AddAlphas=AlphasHaveNoContribution;
				if (AddAlphas)
					PositiveContribution=foundPlusBeta;
				else
					PositiveContribution=foundPlusAlpha;
				break;
			}
		}
	}
	if (firstProblematicIndex!=-1)
		for (int i=0;i<theWeyl.RootSystem.size;i++)
			if (this->TheObjects[index].RootIsGoodForProblematicIndex
						(	theWeyl.RootSystem.TheObjects[i], firstProblematicIndex, AddAlphas,
							PositiveContribution, this->PreferredDualBasis, theWeyl))
			{	newState.Assign(this->TheObjects[index]);
				if (AddAlphas)
					newState.PartialRelation.Alphas.AddObjectOnTop(theWeyl.RootSystem.TheObjects[i]);
				else
					newState.PartialRelation.Betas.AddObjectOnTop(theWeyl.RootSystem.TheObjects[i]);
				this->theIndexStack.AddObjectOnTop(index);
				this->theCounter1Stack.AddObjectOnTop(i);
				this->theCounter2Stack.AddObjectOnTop(0);
				this->ExtensionStep(index,0,theWeyl,TheGlobalVariables,newState);
				this->theIndexStack.PopLastObject();
				this->theCounter1Stack.PopLastObject();
				this->theCounter2Stack.PopLastObject();
			}

/*	root theDifference, tempRoot;
	this->TheObjects[index].PartialRelation.Alphas.Sum(theDifference);
	this->TheObjects[index].PartialRelation.Betas.Sum(tempRoot);
	theDifference.Subtract(tempRoot);
	bool AddAlpha;
	bool found=false;
	while (!found && preferredSimpleRoot<theDimension)
	{	if (!theDifference.TheObjects[preferredSimpleRoot].IsEqualToZero())
			for (int i=0;i<theWeyl.RootSystem.size;i++)
				if (this->TheObjects[index].RootIsGoodForPreferredSimpleRoot
							(	theWeyl.RootSystem.TheObjects[i], preferredSimpleRoot, AddAlpha,
								theWeyl, TheGlobalVariables,theDifference))
				{ newState.Assign(this->TheObjects[index]);
					if (AddAlpha)
						newState.PartialRelation.Alphas.AddObjectOnTop(theWeyl.RootSystem.TheObjects[i]);
					else
						newState.PartialRelation.Betas.AddObjectOnTop(theWeyl.RootSystem.TheObjects[i]);
						this->ExtensionStep(index,preferredSimpleRoot, theWeyl, TheGlobalVariables, newState);
					found=true;
				}
		if (!found)
			preferredSimpleRoot++;
	}
*/
	/*if (this->TheObjects[index].FindBetaWithoutTwoAlphas
        (	theBeta, this->TheObjects[index].PartialRelation.Betas,
					this->TheObjects[index].PartialRelation.Alphas, theWeyl))
  { for (int i=0;i<theWeyl.RootSystem.size;i++)
      if (theWeyl.RootScalarKillingFormMatrixRoot
						(theBeta, theWeyl.RootSystem.TheObjects[i]).IsPositive())
				if (!this->TheObjects[index].nonAlphas.ContainsObject
							(theWeyl.RootSystem.TheObjects[i]))
				{	newState.Assign(this->TheObjects[index]);
					newState.PartialRelation.Alphas.AddObjectOnTop(theWeyl.RootSystem.TheObjects[i]);
					this->ExtensionStep(this->size-1, theWeyl, TheGlobalVariables, newState);
				}
  } else if (this->TheObjects[index].FindBetaWithoutTwoAlphas
              ( theAlpha,this->TheObjects[index].PartialRelation.Alphas,
								this->TheObjects[index].PartialRelation.Betas,theWeyl))
  {	for (int i=0;i<theWeyl.RootSystem.size;i++)
			if (theWeyl.RootScalarKillingFormMatrixRoot
						(theAlpha, theWeyl.RootSystem.TheObjects[i]).IsPositive())
				if (!this->TheObjects[index].nonBetas.ContainsObject(theWeyl.RootSystem.TheObjects[i]))
				{ newState.Assign(this->TheObjects[index]);
					newState.PartialRelation.Betas.AddObjectOnTop
						(theWeyl.RootSystem.TheObjects[i]);
					this->ExtensionStep(index, theWeyl, TheGlobalVariables, newState);
				}
  }*/

  if (!roots::ConesIntersect
				(	TheGlobalVariables,this->TheObjects[index].PartialRelation.Alphas,
					this->TheObjects[index].NilradicalRoots,theDimension))
  { bool addFirstAlpha=false;
		if (this->TheObjects[index].FindBetaWithoutTwoAlphas
					(	theBeta, this->TheObjects[index].PartialRelation.Betas,
						this->TheObjects[index].PartialRelation.Alphas, theWeyl))
			addFirstAlpha=true;
		if (this->TheObjects[index].FindBetaWithoutTwoAlphas
					( theAlpha,this->TheObjects[index].PartialRelation.Alphas,
						this->TheObjects[index].PartialRelation.Betas,theWeyl))
			addFirstAlpha=false;
		for (int i=0;i<theWeyl.RootSystem.size;i++)
			this->TestAddingExtraRoot
				(	index,theWeyl,TheGlobalVariables,theWeyl.RootSystem.TheObjects[i], addFirstAlpha,i);
		for (int i=0;i<theWeyl.RootSystem.size;i++)
			this->TestAddingExtraRoot
				(	index,theWeyl,TheGlobalVariables,theWeyl.RootSystem.TheObjects[i], !addFirstAlpha,i);
	}
	else
	{ this->TheObjects[index].StateIsComplete=true;
	}
}

void minimalRelationsProverStates::TestAddingExtraRoot
	(	int Index, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables, root& theRoot,
		bool AddAlpha, int indexAddedRoot)
{ minimalRelationsProverState newState;
  int theDimension=theWeyl.KillingFormMatrix.NumRows;
  bool tempBool;
  if (AddAlpha)
  { tempBool =
			(!this->TheObjects[Index].PartialRelation.Alphas.ContainsObject(theRoot))&&
			(!this->TheObjects[Index].nonBKSingularGmodLRoots.ContainsObject(theRoot));
  } else
  { tempBool =
 			(!this->TheObjects[Index].PartialRelation.Betas.ContainsObject(theRoot))&&
			(!this->TheObjects[Index].nonBetas.ContainsObject(theRoot));
  }
	if (tempBool)
	{	if (AddAlpha)
			tempBool =
        this->TheObjects[Index].PartialRelation.Betas.ContainsARootConnectedTo(theRoot,theWeyl);
		else
			tempBool =
        this->TheObjects[Index].PartialRelation.Alphas.ContainsARootConnectedTo(theRoot,theWeyl);
		if (tempBool)
		{	roots tempRoots;
			tempRoots.size=0;
			tempRoots.AddObjectOnTop(theRoot);
			if (AddAlpha)
				tempBool =
					!roots::ConesIntersect
						( TheGlobalVariables, this->TheObjects[Index].PartialRelation.Alphas,
              tempRoots, theDimension);
			else
				tempBool =
					!roots::ConesIntersect
						( TheGlobalVariables, this->TheObjects[Index].PartialRelation.Betas,
              tempRoots, theDimension);
			if (tempBool)
			{	newState.Assign(this->TheObjects[Index]);
				if (AddAlpha)
					newState.PartialRelation.Alphas.AddObjectOnTop(theRoot);
				else
					newState.PartialRelation.Betas.AddObjectOnTop(theRoot);
				this->theIndexStack.AddObjectOnTop(Index);
				this->theCounter1Stack.AddObjectOnTop(theWeyl.RootSystem.size);
				this->theCounter2Stack.AddObjectOnTop(indexAddedRoot);
				this->ExtensionStep(Index,0, theWeyl, TheGlobalVariables, newState);
				this->theIndexStack.PopLastObject();
				this->theCounter1Stack.PopLastObject();
				this->theCounter2Stack.PopLastObject();
			}
		}
	}
}

bool minimalRelationsProverState::RootIsGoodForProblematicIndex
	(	root& input,int problemIndex, bool AddingAlphas, bool NeedPositiveContribution,
		roots& theDualBasis, WeylGroup& theWeyl)
{	if (AddingAlphas)
	{	if (this->nonBKSingularGmodLRoots.ContainsObject(input))
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

bool minimalRelationsProverState::IsEqualTo
  ( minimalRelationsProverState& right, WeylGroup& theWeyl,
    GlobalVariables& TheGlobalVariables)
{ if (	this->PartialRelation.Alphas.size!=right.PartialRelation.Alphas.size ||
				this->PartialRelation.Betas.size!=right.PartialRelation.Betas.size)
    return false;
  return this->theScalarProducts.IsEqualTo(right.theScalarProducts);
}

bool minimalRelationsProverStates::AddObjectOnTopNoRepetitionOfObject
  ( minimalRelationsProverState& theState, WeylGroup& theWeyl,
    GlobalVariables& TheGlobalVariables)
{ theState.SortAlphasAndBetas(TheGlobalVariables, theWeyl);
	theState.ComputeScalarProductsMatrix(TheGlobalVariables,theWeyl);
	theState.theScalarProducts.ComputeDebugString();
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

void minimalRelationsProverState::ElementToString
	( std::string& output, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{ std::string tempS; std::stringstream out;
  this->PartialRelation.Alphas.ElementToLinearCombinationString(tempS);
  out << tempS <<"+ ... = ";
  this->PartialRelation.Betas.ElementToLinearCombinationString(tempS);
  out << tempS <<" + ...\n";
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
  out <<"\nBK singular g mod l roots: "<< tempS;
//  this->nonAlphas.ElementToString(tempS);
 // out <<"\nNon-alphas: "<< tempS;
  this->nonBetas.ElementToString(tempS);
  out <<"\nNon-betas: "<< tempS;
  this->nonBKSingularGmodLRoots.ElementToString(tempS);
  out <<"\nnon BK-singular g mod l roots: "<< tempS;
  this->NilradicalRoots.ElementToString(tempS);
  out << "\nNilradical roots: "<< tempS;
  this->nonNilradicalRoots.ElementToString(tempS);
  out << "\nnon-nilradical roots: "<< tempS;
  this->PositiveKroots.ElementToString(tempS);
  out << "\nPositive roots of K: "<< tempS;
  this->nonPositiveKRoots.ElementToString(tempS);
  out <<"\nNon-positive roots of K: "<< tempS;
  if (!this->StateIsPossible)
		out << "\n I am Impossible!";
  if (this->StateIsComplete)
		out <<"\nI am complete!";
  out<<"\n\n";
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
  (ComputationSetup& theSetup, GlobalVariables& TheGlobalVariables, char WeylLetter, int theDimension)
{ minimalRelationsProverState tempState;
	WeylGroup theWeyl;
	theWeyl.MakeArbitrary(WeylLetter, theDimension);
  theWeyl.ComputeRho();
  this->PreferredDualBasis.SetSizeExpandOnTopNoObjectInit(theDimension);
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
	this->PreferredDualBasis.ComputeDebugString();

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
  minimalRelationsProverStates tempProver;
  WeylGroup tempWeyl;
  tempWeyl.MakeDn(5);
  tempProver.GenerateStates(*this,*this->theGlobalVariablesContainer->Default(),'D',5);
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

