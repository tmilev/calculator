#include "polyhedra.h"
#include "rootFKFT.h"

void minimalRelationsProverStates::Extend
  (	int index, int preferredSimpleRoot, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{ root theBeta, theAlpha, theMinusAlpha, theMinusBeta;
//  this->ComputeDebugString(theWeyl, TheGlobalVariables);
	this->MakeProgressReportCurrentState(index,TheGlobalVariables,theWeyl);
  this->TheObjects[index].childStates.size=0;
  minimalRelationsProverState newState;
  int theDimension=theWeyl.KillingFormMatrix.NumRows;
  //roots& theAlphas = this->TheObjects[index].PartialRelation.Alphas;
  //roots& theBetas  = this->TheObjects[index].PartialRelation.Betas;
  int firstProblematicIndex=-1;
	Rational tempRat;
  /*
  bool AddAlphas=false;
  bool PositiveContribution=false;
  for (int i=0;i<this->PreferredDualBasis.size;i++)
	{	bool foundPlusAlpha=false;
		bool foundMinusAlpha=false;
		for (int j=0;j<this->TheObjects[index].PartialRelation.Alphas.size;j++)
		{	tempRat = theWeyl.RootScalarKillingFormMatrixRoot
				(	this->TheObjects[index].PartialRelation.Alphas.TheObjects[j],
					this->PreferredDualBasis.TheObjects[i]);
			if(tempRat.IsPositive())
				foundPlusAlpha=true;
			if(tempRat.IsNegative())
				foundMinusAlpha=true;
		}
		bool foundPlusBeta=false;
		bool foundMinusBeta=false;
		for (int j=0;j<this->TheObjects[index].PartialRelation.Betas.size;j++)
		{	tempRat = theWeyl.RootScalarKillingFormMatrixRoot
				(	this->TheObjects[index].PartialRelation.Betas.TheObjects[j],
					this->PreferredDualBasis.TheObjects[i]);
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
			}*/

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
	if (firstProblematicIndex==-1)
	{	if (!roots::ConesIntersect
					(	TheGlobalVariables,this->TheObjects[index].PartialRelation.Alphas,
						this->TheObjects[index].NilradicalRoots,theDimension))
		{ root NormalSeparatingCones;
      bool tempBool=
        roots::GetNormalSeparatingCones
          ( TheGlobalVariables, theDimension, this->TheObjects[index].NilradicalRoots,
            this->TheObjects[index].PartialRelation.Alphas, NormalSeparatingCones);
      assert(tempBool);
      root tempRoot;
      bool usingWeyl=
        this->GetSeparatingRootIfExists
          (	this->TheObjects[index].NilradicalRoots,
            this->TheObjects[index].PartialRelation.Alphas,
            tempRoot,theWeyl,TheGlobalVariables);
      if (usingWeyl)
        NormalSeparatingCones.Assign(tempRoot);
      bool addFirstAlpha=false;
      if (this->TheObjects[index].FindBetaWithoutTwoAlphas
            (	theBeta, this->TheObjects[index].PartialRelation.Betas,
              this->TheObjects[index].PartialRelation.Alphas, theWeyl))
        addFirstAlpha=true;
      if (this->TheObjects[index].FindBetaWithoutTwoAlphas
            ( theAlpha,this->TheObjects[index].PartialRelation.Alphas,
              this->TheObjects[index].PartialRelation.Betas,theWeyl))
        addFirstAlpha=false;
      //experimental lines!!!!:
      /*addFirstAlpha=true;
      if (this->TheObjects[index].PartialRelation.Betas.size<
            this->TheObjects[index].PartialRelation.Alphas.size)
        addFirstAlpha=true;
      *///end of experimental lines
      int oldSize=this->size;
      for (int i=0;i<theWeyl.RootSystem.size;i++)
      { this->TestAddingExtraRoot
          (	index, theWeyl, TheGlobalVariables, theWeyl.RootSystem.TheObjects[i],
            addFirstAlpha, i, NormalSeparatingCones, usingWeyl);
				this->MakeProgressReportChildStates
					(	i, theWeyl.RootSystem.size*2, this->size-oldSize,
						TheGlobalVariables, theWeyl);
      }
      for (int i=0;i<theWeyl.RootSystem.size;i++)
      {	this->TestAddingExtraRoot
          (	index, theWeyl, TheGlobalVariables, theWeyl.RootSystem.TheObjects[i],
            !addFirstAlpha, i, NormalSeparatingCones, usingWeyl);
				this->MakeProgressReportChildStates
					(	i+theWeyl.RootSystem.size, theWeyl.RootSystem.size*2, 
						this->size-oldSize, TheGlobalVariables, theWeyl);
      }
      for (int i=0;i<this->TheObjects[index].childStates.size;i++)
      { int newIndex= this->TheObjects[index].childStates.TheObjects[i];
        this->theIndexStack.AddObjectOnTop(newIndex);
        this->TheObjects[index].activeChild=i;
				this->MakeProgressReportStack(TheGlobalVariables, theWeyl);
				this->Extend(newIndex,0,theWeyl,TheGlobalVariables);
        this->theIndexStack.PopLastObject();
    	}
		}
		else
		{ this->TheObjects[index].StateIsComplete=true;
		}
	}
}


void minimalRelationsProverStates::TestAddingExtraRoot
	(	int Index, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables, root& theRoot,
		bool AddAlpha, int indexAddedRoot, root& normalSeparatingCones, bool usingWeyl)
{ minimalRelationsProverState newState;
  int theDimension=theWeyl.KillingFormMatrix.NumRows;
  bool tempBool;
  Rational tempRat;
  if (!usingWeyl)
		root::RootScalarEuclideanRoot(theRoot,normalSeparatingCones,tempRat);
	else
		theWeyl.RootScalarKillingFormMatrixRoot(theRoot,normalSeparatingCones,tempRat);
	if (AddAlpha)
	{	if (tempRat.IsNonPositive())
			return;
  } else
  { if (tempRat.IsNonNegative())
			return;
  }
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
	{	if (AddAlpha)
			tempBool =
        this->TheObjects[Index].PartialRelation.Betas.ContainsARootConnectedTo
					(theRoot,theWeyl);
		else
			tempBool =
        this->TheObjects[Index].PartialRelation.Alphas.ContainsARootConnectedTo
					(theRoot,theWeyl);
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
				this->ExtensionStep(Index,0, theWeyl, TheGlobalVariables, newState);
			}
		}
	}
}


void minimalRelationsProverStates::ExtensionStep
  ( int index, int preferredSimpleRootIndex, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables,
    minimalRelationsProverState& newState)
{ int currentNewIndex=this->size;
	if (this->AddObjectOnTopNoRepetitionOfObject(newState, theWeyl,TheGlobalVariables))
  { this->TheObjects[currentNewIndex].ComputeStateReturnFalseIfDubious
			(TheGlobalVariables, theWeyl);
//		this->ComputeDebugString(theWeyl, TheGlobalVariables);
    if (this->TheObjects[currentNewIndex].StateIsPossible &&
				!this->TheObjects[currentNewIndex].StateIsComplete)
    { //if (this->TheObjects[currentNewIndex].StateIsDubious)
      //  this->RemoveDoubt(currentNewIndex,theWeyl,TheGlobalVariables);
      //else
      this->TheObjects[index].childStates.AddObjectOnTop(currentNewIndex);
    }
  }
}
