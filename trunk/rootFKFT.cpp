#include "polyhedra.h"
#include "rootFKFT.h"
//to be merged in main file polyhedra.cpp later

int minimalRelationsProverState::ProblemCounter=0;
int ::minimalRelationsProverStates::ProblemCounter=0;
template < > int ListBasicObjects<minimalRelationsProverState>::ListBasicObjectsActualSizeIncrement=800;
extern ::IndicatorWindowVariables IndicatorWindowGlobalVariables;


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
		{ this->StateIsPossible=false;
			return false;
		}
		if (NumFoundIndices==1)
			this->PositiveKroots.AddObjectOnTopNoRepetitionOfObject
        (theWeyl.RootSystem.TheObjects[LastFoundIndex]);
	}
	this->nonLNonSingularRootsInNeedOfPosKroots.size=0;
	for (int i=0;i<this->nonLNonSingularRoots.size;i++)
		if (this->SumWithNoPosRootIsARoot(this->nonLNonSingularRoots.TheObjects[i],theWeyl))
		{ this->flagNeedsAdditionOfPositiveKroots=true;
      this->nonLNonSingularRootsInNeedOfPosKroots.AddObjectOnTopNoRepetitionOfObject
				(this->nonLNonSingularRoots.TheObjects[i]);
		}
	return true;
}

bool minimalRelationsProverState::ComputeStateReturnFalseIfDubious
  ( GlobalVariables& TheGlobalVariables,  WeylGroup& theWeyl, bool AssumeGlobalMinimalityRHS)
{ this->StateIsPossible=true;
//  this->StateIsDubious=false;
  this->NilradicalRoots.AddRootSnoRepetition(this->PartialRelation.Betas);
  this->BKSingularGmodLRoots.AddRootSnoRepetition(this->PartialRelation.Alphas);
  this->PositiveKroots.AddRootSnoRepetition(this->ChosenPositiveKroots);
  SelectionWithMaxMultiplicity selBetas, selAlphas;
  selBetas.initMe2(this->PartialRelation.Betas.size,2);
  selAlphas.initMe2(this->PartialRelation.Alphas.size,2);
  int NumAlphas= MathRoutines::KToTheNth
		(selAlphas.MaxMultiplicity+1,this->PartialRelation.Alphas.size);
  int NumBetas=MathRoutines::KToTheNth
		(selBetas.MaxMultiplicity+1,this->PartialRelation.Betas.size);
//  this->ComputeDebugString(theWeyl, TheGlobalVariables);
  for (int i=0;i<NumAlphas;i++)
  {	for (int j=0;j<NumBetas;j++)
		{ if (this->CanBeShortened
            (this->PartialRelation, selAlphas,selBetas,theWeyl,AssumeGlobalMinimalityRHS))
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
	//::IndicatorWindowGlobalVariables.String1NeedsRefresh=false;
	//::IndicatorWindowGlobalVariables.String2NeedsRefresh=false;
	//::IndicatorWindowGlobalVariables.String3NeedsRefresh=false;
	//::IndicatorWindowGlobalVariables.String4NeedsRefresh=false;
	::IndicatorWindowGlobalVariables.String5NeedsRefresh=true;
	::IndicatorWindowGlobalVariables.StatusString1NeedsRefresh=false;
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
  this->StateIsComplete=right.StateIsComplete;
  this->theScalarProducts.Assign(right.theScalarProducts);
  this->ChosenPositiveKroots=right.ChosenPositiveKroots;
  this->theChoicesWeMake.CopyFromBase(right.theChoicesWeMake);
  this->currentSeparatingNormalEpsilonForm.Assign(right.currentSeparatingNormalEpsilonForm);
  this->nonLRoots.CopyFromBase(right.nonLRoots);
  this->flagNeedsAdditionOfPositiveKroots=right.flagNeedsAdditionOfPositiveKroots;
  this->nonLNonSingularRoots=right.nonLNonSingularRoots;
  this->nonLNonSingularRootsInNeedOfPosKroots=right.nonLNonSingularRootsInNeedOfPosKroots;
  this->ImpossibleChildStates= right.ImpossibleChildStates;
  this->PossibleChildStates= right.PossibleChildStates;
  this->CompleteChildStates= right.CompleteChildStates;
  this->activeChild=right.activeChild;
  this->nonLNonSingularsAleviatedByChosenPosKRoots=right.nonLNonSingularsAleviatedByChosenPosKRoots;
}

void minimalRelationsProverStates::MakeProgressReportCurrentState
	(	int index, GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl)
{	std::stringstream out; std::string tempS;
  out << this->TheObjects[index].DebugString;
  out<<"\r\nPreferred dual basis: "<<this->PreferredDualBasisEpsilonCoords.DebugString;
  IndicatorWindowGlobalVariables.StatusString1=out.str();
	::IndicatorWindowGlobalVariables.StatusString1NeedsRefresh=true;
	if (TheGlobalVariables.FeedDataToIndicatorWindowDefault!=0)
		TheGlobalVariables.FeedDataToIndicatorWindowDefault(::IndicatorWindowGlobalVariables);
	//::IndicatorWindowGlobalVariables.StatusString1NeedsRefresh=false;
}

void minimalRelationsProverStates::MakeProgressReportIsos
	(int progress, int numSearchedWithinState, int outOf, GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl)
{	std::stringstream out3;
	out3 <<"Searching for automorphisms: " << progress+1 << " from "<< this->size << " states; "
          << numSearchedWithinState+1<<" out of "<< outOf <<" possibilities within current state";
	::IndicatorWindowGlobalVariables.String3NeedsRefresh=true;
	//::IndicatorWindowGlobalVariables.String1NeedsRefresh=false;
	//::IndicatorWindowGlobalVariables.String2NeedsRefresh=false;
	//::IndicatorWindowGlobalVariables.String4NeedsRefresh=false;
	//::IndicatorWindowGlobalVariables.String5NeedsRefresh=false;
	//::IndicatorWindowGlobalVariables.ProgressReportString3=out3.str();
	if (TheGlobalVariables.FeedDataToIndicatorWindowDefault!=0)
		TheGlobalVariables.FeedDataToIndicatorWindowDefault(::IndicatorWindowGlobalVariables);
}

void minimalRelationsProverStates::MakeProgressReportChildStates
	(int numSearched, int outOf, int NewFound, GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl)
{	this->MakeProgressReportStack(TheGlobalVariables, theWeyl);
	std::stringstream out4;
	out4<<"Computing children states: "<< numSearched+1<<" out of "<< outOf << "; "
        << this->TheObjects[*this->theIndexStack.LastObject()].PossibleChildStates.size<<" possible, "
				<< this->TheObjects[*this->theIndexStack.LastObject()].ImpossibleChildStates.size<<" impossible ";
	::IndicatorWindowGlobalVariables.String4NeedsRefresh=true;
//	::IndicatorWindowGlobalVariables.String1NeedsRefresh=false;
//	::IndicatorWindowGlobalVariables.String2NeedsRefresh=false;
//	::IndicatorWindowGlobalVariables.String3NeedsRefresh=false;
//	::IndicatorWindowGlobalVariables.String5NeedsRefresh=false;
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
                << " of "<< this->TheObjects[currentIndex].PossibleChildStates.size	<<"; ";
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
//	::IndicatorWindowGlobalVariables.String5NeedsRefresh=false;
	::IndicatorWindowGlobalVariables.StatusString1NeedsRefresh=false;
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
  this->flagNeedsAdditionOfPositiveKroots=false;
	this->activeChild=-1;
}

bool minimalRelationsProverState::IsAGoodPosRootsKChoice
  (WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{ theWeyl.GenerateAdditivelyClosedSubset(this->PositiveKroots,this->PositiveKroots);
  for (int i=0;i<this->BKSingularGmodLRoots.size;i++)
    if (!this->SumWithNoPosRootIsARoot(this->BKSingularGmodLRoots.TheObjects[i], theWeyl))
      return false;
  return true;
}

bool minimalRelationsProverState::FindBetaWithoutTwoAlphas
  (root& outputBeta, roots& inputBetas, roots& inputAlphas, WeylGroup& theWeyl)
{ for (int i=0;i<inputBetas.size;i++)
    if (inputAlphas.NumRootsConnectedTo(inputBetas.TheObjects[i],theWeyl)<2)
    { outputBeta.Assign(inputBetas.TheObjects[i]);
      return true;
    }
  return false;
}

bool minimalRelationsProverStates::StateIsEqualTo
  ( minimalRelationsProverState& theState, int IndexOther, WeylGroup& theWeyl,  GlobalVariables& TheGlobalVariables)
{	return this->ExtendToIsomorphismRootSystem(theState,IndexOther,TheGlobalVariables,theWeyl);
}

bool minimalRelationsProverStates::AddObjectOnTopNoRepetitionOfObject
  ( int ParentIndex, minimalRelationsProverState& theState, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{ theState.SortAlphasAndBetas(TheGlobalVariables, theWeyl);
	theState.ComputeScalarProductsMatrix(TheGlobalVariables,theWeyl);
	//theState.theScalarProducts.ComputeDebugString();
 // if (!this->TheObjects[ParentIndex].flagNeedsAdditionOfPositiveKroots || ParentIndex==0)
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
	(	GlobalVariables& TheGlobalVariables, WeylGroup& theWeyl, roots& theAlphas,		roots& theBetas, MatrixLargeRational& output)
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
	( std::string& output, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables, bool displayEpsilons)
{ std::string tempS; std::stringstream out;
	for(int i=0;i<this->PartialRelation.Alphas.size;i++)
	{	this->PartialRelation.Alphas.TheObjects[i].ElementToString(tempS);
		out << "a_"<<i+1<<tempS <<" + ";
	}
	out <<"... = ";
	for(int i=0;i<this->PartialRelation.Betas.size;i++)
	{	this->PartialRelation.Betas.TheObjects[i].ElementToString(tempS);
		out << "b_"<<i+1<<tempS <<" + ";
	}
	out <<"...     (a_i,b_i>0)\r\n";
	root tempRoot; roots tempRoots;
	for(int i=0;i<this->PartialRelation.Alphas.size;i++)
	{	theWeyl.GetEpsilonCoords
      (this->PartialRelation.Alphas.TheObjects[i],tempRoot,TheGlobalVariables);
		tempRoot.ElementToStringEpsilonForm(tempS,false,false);
		out << "a_"<<i+1<<"("<<tempS <<") + ";
	}
	out <<"... = ";
	for(int i=0;i<this->PartialRelation.Betas.size;i++)
	{	theWeyl.GetEpsilonCoords(this->PartialRelation.Betas.TheObjects[i],tempRoot,TheGlobalVariables);
		tempRoot.ElementToStringEpsilonForm(tempS,false,false);
		out << "b_"<<i+1<<"("<<tempS <<") + ";
	}
	out <<"...";
  this->currentSeparatingNormalEpsilonForm.ElementToStringEpsilonForm(tempS,false,false);
  out <<"\nCurrent separating normal: " << tempS;
  out<<"\r\nChildren states: "<<  this->PossibleChildStates.size <<" possible, "<< this->ImpossibleChildStates.size
      <<" impossible, " << this->CompleteChildStates.size<<" complete. Index next possible child to explore: " << this->activeChild+2;
  roots AlphaChildren, BetaChildren, Kchildren;
  for (int i=0;i<this->PossibleChildStates.size;i++)
  {	::minimalRelationsProverState& child = owner->TheObjects[this->PossibleChildStates.TheObjects[i]];
		if (this->PartialRelation.Alphas.size<child.PartialRelation.Alphas.size)
		{	theWeyl.GetEpsilonCoords(*child.theChoicesWeMake.LastObject(),tempRoot,TheGlobalVariables);
			AlphaChildren.AddObjectOnTop(tempRoot);
		}
		if (this->PartialRelation.Betas.size<child.PartialRelation.Betas.size)
		{	theWeyl.GetEpsilonCoords(*child.theChoicesWeMake.LastObject(),tempRoot,TheGlobalVariables);
			BetaChildren.AddObjectOnTop(tempRoot);
		}
		if (this->ChosenPositiveKroots.size<child.ChosenPositiveKroots.size)
		{	theWeyl.GetEpsilonCoords(*child.ChosenPositiveKroots.LastObject(),tempRoot,TheGlobalVariables);
			Kchildren.AddObjectOnTop(tempRoot);
		}
  }
  //assert(!(AlphaChildren.size+BetaChildren.size+Kchildren.size==0));
	if (AlphaChildren.size>0)
	{	AlphaChildren.ElementToStringEpsilonForm(tempS,false,false,false);
		out <<"\r\nNew alphas added next: " <<tempS;
  }
	if (BetaChildren.size>0)
	{	BetaChildren.ElementToStringEpsilonForm(tempS,false,false,false);
		out <<"\r\nNew betas added next: " <<tempS;
  }
	if (Kchildren.size>0)
	{	Kchildren.ElementToStringEpsilonForm(tempS,false,false,false);
		out <<"\r\nNew k-roots added next: " <<tempS;
  }
  if(!displayEpsilons)
    this->ChosenPositiveKroots.ElementToString(tempS);
  else
  { theWeyl.GetEpsilonCoords(this->ChosenPositiveKroots,tempRoots,TheGlobalVariables);
    tempRoots.ElementToStringEpsilonForm(tempS,false,false,false);
  }
  out <<"\r\nPositive choices for K-roots made:" << tempS;
  if(!displayEpsilons)
    this->nonLNonSingularRootsInNeedOfPosKroots.ElementToString(tempS);
  else
  { theWeyl.GetEpsilonCoords(this->nonLNonSingularRootsInNeedOfPosKroots, tempRoots, TheGlobalVariables);
    tempRoots.ElementToStringEpsilonForm(tempS,false,false,false);
  }
  out <<"\r\nNon-L non-singular roots in need of a K- positive root: " << tempS;
  if (!displayEpsilons)
    this->theChoicesWeMake.ElementToString(tempS);
  else
  { theWeyl.GetEpsilonCoords(this->theChoicesWeMake,tempRoots,TheGlobalVariables);
    tempRoots.ElementToStringEpsilonForm(tempS,false,false,false);
  }
  out <<"\r\nThe choices we make: "<< tempS;
	if (!displayEpsilons)
    this->BKSingularGmodLRoots.ElementToString(tempS);
  else
  { theWeyl.GetEpsilonCoords(this->BKSingularGmodLRoots,tempRoots,TheGlobalVariables);
    tempRoots.ElementToStringEpsilonForm(tempS,false,false,false);
  }
  out <<"\r\nBK singular g mod l roots: "<< tempS;
	if (!displayEpsilons)
    this->nonAlphas.ElementToString(tempS);
  else
  { theWeyl.GetEpsilonCoords(this->nonAlphas,tempRoots,TheGlobalVariables);
    tempRoots.ElementToStringEpsilonForm(tempS,false,false,false);
  }
  out <<"\r\nNon-alphas: "<< tempS;
  if (!displayEpsilons)
    this->nonBetas.ElementToString(tempS);
  else
  { theWeyl.GetEpsilonCoords(this->nonBetas,tempRoots,TheGlobalVariables);
    tempRoots.ElementToStringEpsilonForm(tempS,false,false,false);
  }
  out <<"\r\nNon-betas: "<< tempS;
  if (!displayEpsilons)
    this->nonBKSingularGmodLRoots.ElementToString(tempS);
  else
  { theWeyl.GetEpsilonCoords(this->nonBKSingularGmodLRoots,tempRoots,TheGlobalVariables);
    tempRoots.ElementToStringEpsilonForm(tempS,false,false,false);
  }
  out <<"\r\nnon BK-singular g mod l roots: "<< tempS;
  if (!displayEpsilons)
    this->NilradicalRoots.ElementToString(tempS);
  else
  { theWeyl.GetEpsilonCoords(this->NilradicalRoots,tempRoots,TheGlobalVariables);
    tempRoots.ElementToStringEpsilonForm(tempS,false,false,false);
  }
  out << "\r\nNilradical roots: "<< tempS;
  if (!displayEpsilons)
    this->nonNilradicalRoots.ElementToString(tempS);
  else
  { theWeyl.GetEpsilonCoords(this->nonNilradicalRoots,tempRoots,TheGlobalVariables);
    tempRoots.ElementToStringEpsilonForm(tempS,false,false,false);
  }
  out << "\r\nnon-nilradical roots: "<< tempS;
  if (!displayEpsilons)
    this->PositiveKroots.ElementToString(tempS);
  else
  { theWeyl.GetEpsilonCoords(this->PositiveKroots,tempRoots,TheGlobalVariables);
    tempRoots.ElementToStringEpsilonForm(tempS,false,false,false);
  }
  out << "\r\nPositive roots of K: "<< tempS;
  if (!displayEpsilons)
    this->nonKRoots.ElementToString(tempS);
  else
  { theWeyl.GetEpsilonCoords(this->nonKRoots,tempRoots,TheGlobalVariables);
    tempRoots.ElementToStringEpsilonForm(tempS,false,false,false);
  }
  out <<"\r\nNon-roots of K: "<< tempS;
  if (!displayEpsilons)
    this->nonPositiveKRoots.ElementToString(tempS);
  else
  { theWeyl.GetEpsilonCoords(this->nonPositiveKRoots,tempRoots,TheGlobalVariables);
    tempRoots.ElementToStringEpsilonForm(tempS,false,false,false);
  }
  out <<"\r\nNon-positive roots of K: "<< tempS;
  if (!displayEpsilons)
    this->nonLRoots.ElementToString(tempS);
  else
  { theWeyl.GetEpsilonCoords(this->nonLRoots,tempRoots,TheGlobalVariables);
    tempRoots.ElementToStringEpsilonForm(tempS,false,false,false);
  }
  out <<"\r\nNon-L roots: "<< tempS;
  if (!displayEpsilons)
    this->nonLNonSingularRoots.ElementToString(tempS);
  else
  { theWeyl.GetEpsilonCoords(this->nonLNonSingularRoots,tempRoots,TheGlobalVariables);
    tempRoots.ElementToStringEpsilonForm(tempS,false,false,false);
  }
  out <<"\r\nNon-L non-singular roots: "<< tempS;
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
	{	this->TheObjects[i].ElementToString(tempS,theWeyl, TheGlobalVariables,true);
		out <<tempS<<"\n\n";
	}
	output=out.str();
}

void minimalRelationsProverStates::ComputePreferredDualBasis
  (char WeylLetter, int theDimension, GlobalVariables& TheGlobalVariables)
{ this->PreferredDualBasis.SetSizeExpandOnTopNoObjectInit(theDimension);
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
	if (WeylLetter=='E' && theDimension==8)
  {  this->PreferredDualBasis.SetSizeExpandOnTopNoObjectInit(0);
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
  this->theWeylGroup.GetEpsilonCoords
    ( this->PreferredDualBasis, this->PreferredDualBasisEpsilonCoords, TheGlobalVariables);
	this->PreferredDualBasisEpsilonCoords.ElementToStringEpsilonForm
    ( this->PreferredDualBasisEpsilonCoords.DebugString, false, false, false);
}


void minimalRelationsProverStates::GenerateStartingState
  ( ComputationSetup& theSetup, GlobalVariables& TheGlobalVariables, char WeylLetter, int theDimension)
{  minimalRelationsProverState tempState;
    this->theWeylGroup.MakeArbitrary(WeylLetter, theDimension);
    this->theWeylGroup.ComputeRho(false);
    this->isomorphismComputer.AmbientWeyl.Assign(this->theWeylGroup);
    this->isomorphismComputer.ComputeAll();
    this->PreferredDualBasis.size=0;
    this->flagAssumeGlobalMinimalityRHS=false;
    this->invertedCartan.AssignMatrixIntWithDen(this->theWeylGroup.KillingFormMatrix,1);
    this->invertedCartan.Invert(TheGlobalVariables);
    this->ComputePreferredDualBasis(WeylLetter, theDimension, TheGlobalVariables);
    this->size=0;
    root tempRoot;
    tempRoot.Assign(this->theWeylGroup.RootSystem.TheObjects[7]);
    tempState.PartialRelation.Alphas.AddObjectOnTop(tempRoot);
    for (int i=0;i<this->theWeylGroup.RootSystem.size;i++)
      if (this->theWeylGroup.RootSystem.TheObjects[i]!=tempRoot && this->theWeylGroup.RootScalarKillingFormMatrixRoot
              (tempRoot, this->theWeylGroup.RootSystem.TheObjects[i]).IsPositive() )
      { tempState.PartialRelation.Betas.AddObjectOnTop(this->theWeylGroup.RootSystem.TheObjects[i]);
          break;
      }
    tempState.theChoicesWeMake.AddObjectOnTop(tempState.PartialRelation.Betas.TheObjects[0]);
    tempState.theChoicesWeMake.AddObjectOnTop(tempState.PartialRelation.Alphas.TheObjects[0]);
    tempState.owner=this;
  /*  tempState.PartialRelation.Betas.AddObjectOnTop(tempRoot);
    tempRoot.Assign(this->theWeylGroup.RootSystem.TheObjects[5]);
    tempState.theChoicesWeMake.AddObjectOnTop(tempRoot);
    tempState.PartialRelation.Betas.AddObjectOnTop(tempRoot);
    tempState.theChoicesWeMake.AddObjectOnTop(tempRoot);
    this->MinNumDifferentBetas=2;
    */
    this->AddObjectOnTop(tempState);
    this->LastObject()->ComputeStateReturnFalseIfDubious(TheGlobalVariables, this->theWeylGroup, this->flagAssumeGlobalMinimalityRHS);
    this->LastObject()->ComputeScalarProductsMatrix(TheGlobalVariables, this->theWeylGroup);
    this->LastObject()->ComputeDebugString(this->theWeylGroup,TheGlobalVariables);
    this->theIndexStack.AddObjectOnTop(0);
    this->MakeProgressReportCurrentState(0,TheGlobalVariables,this->theWeylGroup);
    this->ComputeLastStackIndex(this->theWeylGroup, TheGlobalVariables);
    this->flagComputationIsInitialized=true;
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
	{ this->theOutput.DebugString.append("The bad relations: \n\n");
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
{ rootSubalgebra tempSA;
	this->theRootSubalgebras.flagUseDynkinClassificationForIsomorphismComputation=false;
	this->theRootSubalgebras.flagUsingActionsNormalizerCentralizerNilradical=true;
	this->theRootSubalgebras.flagLookingForMinimalRels=true;
	this->theRootSubalgebras.theGoodRelations.flagIncludeCoordinateRepresentation=true;
	this->theRootSubalgebras.theBadRelations.flagIncludeCoordinateRepresentation=true;
	this->theRootSubalgebras.theMinRels.flagIncludeCoordinateRepresentation=true;
	this->theRootSubalgebras.theGoodRelations.flagIncludeSubalgebraDataInDebugString=false;
	this->theRootSubalgebras.theBadRelations.flagIncludeSubalgebraDataInDebugString=false;
	this->theRootSubalgebras.GenerateAllRootSubalgebrasUpToIsomorphism
    ( *this->theGlobalVariablesContainer->Default(), 'E', 6, true, true);
  this->theRootSubalgebras.ComputeDebugString(true, false,true,0,0,*this->theGlobalVariablesContainer->Default() );
  if (this->theRootSubalgebras.flagComputeConeCondition)
    this->theRootSubalgebras.ComputeLProhibitingRelations
      (*this->theGlobalVariablesContainer->Default(), 0, this->theRootSubalgebras.size-1);
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

void WeylGroup::GetEpsilonCoords(roots &input, roots &output, GlobalVariables &theGlobalVariables)
{ roots tempRoots;
	tempRoots.SetSizeExpandOnTopNoObjectInit(this->KillingFormMatrix.NumRows);
	for (int i=0;i<this->KillingFormMatrix.NumRows;i++)
	{ tempRoots.TheObjects[i].MakeZero(this->KillingFormMatrix.NumRows);
		tempRoots.TheObjects[i].TheObjects[i].MakeOne();
	}
	this->GetEpsilonCoordsWRTsubalgebra(tempRoots,input,output,theGlobalVariables);
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
	(	minimalRelationsProverState& theState, int indexOther, GlobalVariables& theGlobalVariables, WeylGroup& theWeyl)
{	//return true;
  theState.SortAlphasAndBetas(theGlobalVariables, theWeyl);
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
  if (!thePermAlphas.HasSameMaxMultiplicities(tempPermAlphas) || !thePermBetas.HasSameMaxMultiplicities(tempPermBetas) ||
      !thePermK.HasSameMaxMultiplicities(tempPermK))
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
      {	root& tempRoot=theAlphas.TheObjects[tempList.TheObjects[k]];
        thePermutedAlphas.AddObjectOnTop(tempRoot);
        theDomain.AddObjectOnTop(tempRoot);
      }
      //theDomain.ComputeDebugString();
      for (int j=0;j<NumCyclesBetas;j++)
      { theDomain.size=theAlphas.size+theKroots.size;
        thePermutedBetas.size=0;
        thePermBetas.GetPermutation(tempList);
        for (int k=0;k<tempList.size;k++)
        {	root& tempRoot=theBetas.TheObjects[tempList.TheObjects[k]];
          thePermutedBetas.AddObjectOnTop(tempRoot);
          theDomain.AddObjectOnTop(tempRoot);
        }
  //			theDomain.ComputeDebugString();
        this->MakeProgressReportIsos
          ( indexOther, (l*NumCyclesAlphas+ i)*NumCyclesBetas+j, NumCyclesK*NumCyclesBetas*NumCyclesAlphas,
            theGlobalVariables,theWeyl);
        theState.ComputeScalarProductsMatrix(theGlobalVariables, theWeyl,thePermutedAlphas,thePermutedBetas,thisMatrix);
        if (thisMatrix.IsEqualTo(theOtherMatrix))
        {	if (this->isomorphismComputer.attemptExtensionToIsomorphism
                (theDomain, theRange,	theGlobalVariables, 0, false, theWeyl, &DomainAndRangeGenerateNonIsoSAs))
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

void minimalRelationsProverStates::GetIsoTypicComponents
  ( roots& theRoots, roots& theOtherTypeRoots, permutation& outputComponents, minimalRelationsProverState& theState,
    WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
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
		{	tempList.AddObjectOnTop(isotypicPieces.LastObject()->size);
			isotypicPieces.SetSizeExpandOnTopNoObjectInit(isotypicPieces.size+1);
			isotypicPieces.LastObject()->size=0;
		}
		isotypicPieces.LastObject()->AddObjectOnTop(theRoots.TheObjects[i]);
	}
	tempList.AddObjectOnTop(isotypicPieces.LastObject()->size);
	outputComponents.initPermutation(tempList,theRoots.size);
}

bool rootSubalgebra::attemptExtensionToIsomorphism
	( roots& Domain, roots& Range, GlobalVariables& theGlobalVariables,	ReflectionSubgroupWeylGroup* outputAutomorphisms,
    bool actOnCentralizerOnly, WeylGroup& theWeyl, bool *DomainAndRangeGenerateNonIsoSAs)
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
	if (theDomainRootSA.theDynkinDiagram.DebugString!= theRangeRootSA.theDynkinDiagram.DebugString ||
			theDomainRootSA.theCentralizerDiagram.DebugString!=theRangeRootSA.theCentralizerDiagram.DebugString)
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
	for (int i=0; i<CentralizerDiagramAutomorphisms.size; i++)
		tempAutosCentralizer.MaxMultiplicities.TheObjects[i] = CentralizerDiagramAutomorphisms.TheObjects[i].size-1;
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
					( l+NumAutosCentralizer*j, tempI2*NumAutosCentralizer, outputAutomorphisms->ExternalAutomorphisms.size, theGlobalVariables);
			tempAutosCentralizer.IncrementSubset();
		}
		permComponentsCentralizer.incrementAndGetPermutation(tempPermutation2);
  }
  return false;
}

