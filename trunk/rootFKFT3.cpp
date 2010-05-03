#include "polyhedra.h"
extern ::IndicatorWindowVariables IndicatorWindowGlobalVariables;

int rootSubalgebra::ProblemCounter=0;
bool rootSubalgebra::GenerateAutomorphisms ( rootSubalgebra& right, GlobalVariables& theGlobalVariables, ReflectionSubgroupWeylGroup* outputAutomorphisms, bool actOnCentralizerOnly)
{ if (this->theDynkinDiagram.DebugString!= right.theDynkinDiagram.DebugString)
    return false;
  if (this->theCentralizerDiagram.DebugString!= right.theCentralizerDiagram.DebugString)
    return false;
	if (outputAutomorphisms!=0)
	{ outputAutomorphisms->ExternalAutomorphisms.size=0;
    outputAutomorphisms->simpleGenerators.size=0;
    outputAutomorphisms->simpleGenerators.CopyFromBase(this->SimpleBasisCentralizerRoots);
	}
  roots isoDomain, isoRange;
  permutation permComponents, permComponentsCentralizer;
  ListBasicObjects<int> tempList, tempPermutation1, tempPermutation2;
	SelectionWithDifferentMaxMultiplicities tempAutos, tempAutosCentralizer;
  ListBasicObjects<ListBasicObjects<ListBasicObjects<int> > > DiagramAutomorphisms, CentralizerDiagramAutomorphisms;
  this->theDynkinDiagram.GetAutomorphisms(DiagramAutomorphisms);
  this->theCentralizerDiagram.GetAutomorphisms(CentralizerDiagramAutomorphisms);
  tempAutos.initIncomplete(DiagramAutomorphisms.size);
  tempAutosCentralizer.initIncomplete(CentralizerDiagramAutomorphisms.size);
  for (int i=0; i<DiagramAutomorphisms.size; i++)
		tempAutos.MaxMultiplicities.TheObjects[i]= DiagramAutomorphisms.TheObjects[i].size-1;
	for (int i=0; i<CentralizerDiagramAutomorphisms.size; i++)
		tempAutosCentralizer.MaxMultiplicities.TheObjects[i]= CentralizerDiagramAutomorphisms.TheObjects[i].size-1;
  tempList.SetSizeExpandOnTopNoObjectInit(this->theDynkinDiagram.sameTypeComponents.size);
  int tempSize=0;
  for (int i=0; i<this->theDynkinDiagram.sameTypeComponents.size; i++)
  { tempList.TheObjects[i]= this->theDynkinDiagram.sameTypeComponents.TheObjects[i].size;
    tempSize+=tempList.TheObjects[i];
  }
  permComponents.initPermutation(tempList,tempSize);
  tempList.SetSizeExpandOnTopNoObjectInit(this->theCentralizerDiagram.sameTypeComponents.size);
  tempSize=0;
  for (int i=0; i<this->theCentralizerDiagram.sameTypeComponents.size; i++)
  { tempList.TheObjects[i]= this->theCentralizerDiagram.sameTypeComponents.TheObjects[i].size;
    tempSize+=tempList.TheObjects[i];
  }
  permComponentsCentralizer.initPermutation(tempList,tempSize);
	int tempI1;
	int NumAutos;
  tempI1= permComponents.getTotalNumSubsets();
	NumAutos=tempAutos.getTotalNumSubsets();
	int tempI2= permComponentsCentralizer.getTotalNumSubsets();
	int NumAutosCentralizer= tempAutosCentralizer.getTotalNumSubsets();
  permComponents.GetPermutation(tempPermutation1);
  permComponentsCentralizer.GetPermutation(tempPermutation2);
  for (int i=0; i<tempI1; i++)
  { for(int j=0; j<tempI2; j++)
    { for (int k=0; k<NumAutos; k++)
			{	for(int l=0; l<NumAutosCentralizer; l++)
				{	isoDomain.size=0; isoRange.size=0;
					this->theDynkinDiagram.GetMapFromPermutation
						(	isoDomain,isoRange,tempPermutation1,DiagramAutomorphisms, tempAutos, right.theDynkinDiagram);
					this->theCentralizerDiagram.GetMapFromPermutation
						( isoDomain, isoRange, tempPermutation2, CentralizerDiagramAutomorphisms, tempAutosCentralizer, right.theCentralizerDiagram);
					if (this->attemptExtensionToIsomorphismNoCentralizer(isoDomain,isoRange,theGlobalVariables,0, outputAutomorphisms,false,0,0,0))//GenerateAllAutos))
						if (outputAutomorphisms==0)
							return true;
					if (outputAutomorphisms!=0)
						this->MakeProgressReportGenAutos
							( l+NumAutosCentralizer*(k+NumAutos*(j+i*tempI2)), tempI1*tempI2*NumAutos*NumAutosCentralizer,outputAutomorphisms->ExternalAutomorphisms.size,
								theGlobalVariables);
					tempAutosCentralizer.IncrementSubset();
				}
				tempAutos.IncrementSubset();
			}
			permComponentsCentralizer.incrementAndGetPermutation(tempPermutation2);
    }
    permComponents.incrementAndGetPermutation(tempPermutation1);
  }
  return false;
}


void minimalRelationsProverStates::GenerateStartingStatesFixedK( ComputationSetup& theSetup, GlobalVariables& TheGlobalVariables, char WeylLetter, int theDimension)
{ minimalRelationsProverState tempState; tempState.owner=this;
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
  this->theK.AmbientWeyl.Assign(this->theWeylGroup);
  this->theK.genK.SetSizeExpandOnTopNoObjectInit(4);
  this->theK.genK.TheObjects[0].InitFromIntegers(8, 1,0,0,0,0,0,0,0);
  this->theK.genK.TheObjects[1].InitFromIntegers(8, 0,1,0,0,0,0,0,0);
  this->theK.genK.TheObjects[2].InitFromIntegers(8, 0,0,0,0,1,0,0,0);
  this->theK.genK.TheObjects[3].InitFromIntegers(8, 0,0,0,0,0,0,1,0);
  this->theK.ComputeAll();
  this->theK.GenerateAutomorphisms(this->theK, TheGlobalVariables, &this->theIsos, true);
  this->theIsos.AmbientWeyl.Assign(this->theWeylGroup);
  this->theIsos.simpleGenerators.size=0;
  this->theIsos.ComputeSubGroupFromGeneratingReflections(this->theK.SimpleBasisCentralizerRoots, this->theIsos.ExternalAutomorphisms, TheGlobalVariables, 20000, true);
  ListBasicObjects<int> kModOrbitInfo;
  ListBasicObjects<int> kModOrbitRepresentatives;
  kModOrbitInfo.initFillInObject(this->theK.kModules.size,-1);
  kModOrbitRepresentatives.size=0;
  rootsCollection IsoOrbits;
  roots tempRoots;
  for (int i=0;i<this->theK.kModules.size;i++)
		if (kModOrbitInfo.TheObjects[i]==-1)
		{	this->theIsos.GenerateOrbitReturnFalseIfTruncated(this->theK.kModules.TheObjects[i].TheObjects[0], tempRoots,0);
			kModOrbitInfo.TheObjects[i]=i;
			kModOrbitRepresentatives.AddObjectOnTop(i);
			for (int j=1; j<tempRoots.size;j++)
				kModOrbitInfo.TheObjects[this->theK.GetIndexKmoduleContainingRoot(tempRoots.TheObjects[j])]=i;
    }
  for (int i=0;i<this->theWeylGroup.RootSystem.size;i++)
  { root& theRoot= this->theWeylGroup.RootSystem.TheObjects[i];
		if (this->theK.AllRootsK.ContainsObject(theRoot))
		{	tempState.nonNilradicalRoots.AddObjectOnTop(theRoot);
			if (theRoot.IsPositiveOrZero())
				tempState.PositiveKroots.AddObjectOnTop(theRoot);
			else
				tempState.nonPositiveKRoots.AddObjectOnTop(theRoot);
			tempState.nonAlphas.AddObjectOnTop(theRoot);
			tempState.nonBetas.AddObjectOnTop(theRoot);
			tempState.nonBKSingularGmodLRoots.AddObjectOnTop(theRoot);
		} else
			if (!this->theK.HighestWeightsGmodK.ContainsObject(theRoot))
			{ tempState.nonBKSingularGmodLRoots.AddObjectOnTop(theRoot);
				tempState.nonKRoots.AddObjectOnTop(theRoot);
				tempState.nonPositiveKRoots.AddObjectOnTop(theRoot);
				tempState.nonAlphas.AddObjectOnTop(theRoot);
			}
  }
  this->AddObjectOnTop(tempState);
  for(int i=0;i<kModOrbitRepresentatives.size;i++)
  {	root& theHW= this->theK.HighestWeightsGmodK.TheObjects[kModOrbitRepresentatives.TheObjects[i]];
		tempState.theChoicesWeMake.AddObjectOnTop(theHW);
		tempState.PartialRelation.Alphas.AddObjectOnTop(theHW);
		this->ExtensionStepFixedK(0,this->theWeylGroup,TheGlobalVariables,tempState);
		tempState.theChoicesWeMake.size=0;
		tempState.PartialRelation.Alphas.size=0;
  }
  if (this->TheObjects[0].PossibleChildStates.size>0)
  {	this->TheObjects[0].activeChild=-1;
		this->theIndexStack.AddObjectOnTop(0);
  }
  this->MakeProgressReportCurrentState(0,TheGlobalVariables, this->theWeylGroup);
  this->flagComputationIsInitialized=true;
}

void ReflectionSubgroupWeylGroup::ComputeSubGroupFromGeneratingReflections
	( roots& generators, rootsCollection& ExternalAutos, GlobalVariables& theGlobalVariables, int UpperLimitNumElements,	bool recomputeAmbientRho)
{ hashedRoots& orbitRho = theGlobalVariables.hashedRootsComputeSubGroupFromGeneratingReflections;
	this->truncated=false;
	this->ClearTheObjects();
	orbitRho.ClearTheObjects();
	if (recomputeAmbientRho)
		this->AmbientWeyl.ComputeRho(false);
	this->simpleGenerators.CopyFromBase(generators);
	this->AmbientWeyl.TransformToSimpleBasisGenerators(this->simpleGenerators);
	this->ComputeRootSubsystem();
	ElementWeylGroup tempEW;
	tempEW.size=0;
	root tempRoot;
	tempRoot.Assign(this->AmbientWeyl.rho);
	// rho is invariant under external graph automorphisms (!)
	//Hence the below lines. Needs to be fixed (mathematically).
  //for the time being the below lines remain, until I think of how to do it properly.
	tempRoot.MultiplyByInteger(3);
	tempRoot.TheObjects[0].AddInteger(1);

	orbitRho.AddObjectOnTopHash(tempRoot);
	this->AddObjectOnTopHash(tempEW);
	root currentRoot;
	for (int i=0;i<this->size;i++)
	{ tempEW=this->TheObjects[i];
		for (int j=0; j<this->simpleGenerators.size;j++)
		{ this->AmbientWeyl.ReflectBetaWRTAlpha
				(this->simpleGenerators.TheObjects[j],orbitRho.TheObjects[i],false,currentRoot);
			if (!orbitRho.ContainsObjectHash(currentRoot))
			{ orbitRho.AddObjectOnTopHash(currentRoot);
				tempEW.AddObjectOnTop(j);
				this->AddObjectOnTopHash(tempEW);
				tempEW.PopLastObject();
			}
		}
		for (int j=1;j<ExternalAutos.size;j++)
		{ ExternalAutos.TheObjects[j].MakeBasisChange(orbitRho.TheObjects[i],currentRoot);
			if (!orbitRho.ContainsObjectHash(currentRoot))
			{ orbitRho.AddObjectOnTopHash(currentRoot);
				tempEW.AddObjectOnTop(j+this->simpleGenerators.size);
				this->AddObjectOnTopHash(tempEW);
				tempEW.PopLastObject();
			}
		}
		if (UpperLimitNumElements>0)
			if (this->size>=UpperLimitNumElements)
			{ this->truncated=true;
				return;
			}
	}
}

bool ReflectionSubgroupWeylGroup::GenerateOrbitReturnFalseIfTruncated(root& input, roots& outputOrbit, int UpperLimitNumElements)
{ hashedRoots theOrbit;
	bool result = true;
	this->ClearTheObjects();
	theOrbit.ClearTheObjects();
	root tempRoot=input;
	theOrbit.AddObjectOnTopHash(tempRoot);
	for (int i=0;i<theOrbit.size;i++)
	{ for (int j=0; j<this->simpleGenerators.size;j++)
		{ this->AmbientWeyl.ReflectBetaWRTAlpha(this->simpleGenerators.TheObjects[j], theOrbit.TheObjects[i], false, tempRoot);
			theOrbit.AddObjectOnTopNoRepetitionOfObjectHash(tempRoot);
		}
		for (int j=1;j<this->ExternalAutomorphisms.size;j++)
		{ this->ExternalAutomorphisms.TheObjects[j].MakeBasisChange(theOrbit.TheObjects[i], tempRoot);
      theOrbit.AddObjectOnTopNoRepetitionOfObjectHash(tempRoot);
		}
		if (UpperLimitNumElements>0)
			if (theOrbit.size>=UpperLimitNumElements)
			{ result=false;
				break;
			}
	}
	outputOrbit.CopyFromBase(theOrbit);
	return result;
}

void minimalRelationsProverStates::ExtensionStepFixedK( int index, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables, minimalRelationsProverState& newState)
{ newState.ComputeStateReturnFalseIfDubious(TheGlobalVariables, theWeyl,this->flagAssumeGlobalMinimalityRHS);
  if (newState.StateIsPossible)
  { int currentNewIndex=this->size;
    if (this->AddObjectOnTopNoRepetitionOfObjectFixedK(index, newState, theWeyl, TheGlobalVariables))
    { this->TheObjects[currentNewIndex].ComputeDebugString(theWeyl,TheGlobalVariables);
      if (this->TheObjects[currentNewIndex].StateIsPossible && !this->TheObjects[currentNewIndex].StateIsComplete)
      { this->TheObjects[index].PossibleChildStates.AddObjectOnTop(currentNewIndex);
        this->TheObjects[index].activeChild=-1;
      } else
        if(!this->TheObjects[currentNewIndex].StateIsPossible)
          this->TheObjects[index].ImpossibleChildStates.AddObjectOnTop(currentNewIndex);
        else
          this->TheObjects[index].CompleteChildStates.AddObjectOnTop(currentNewIndex);
    }
  }
}

bool minimalRelationsProverStates::AddObjectOnTopNoRepetitionOfObjectFixedK
  ( int ParentIndex, minimalRelationsProverState& theState, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{ for (int i=0;i<this->size;i++)
    if (this->ExtendToIsomorphismRootSystemFixedK(theState,i,TheGlobalVariables,theWeyl))
      return false;
  this->AddObjectOnTop(theState);
  return true;
}

bool minimalRelationsProverStates::ExtendToIsomorphismRootSystemFixedK( minimalRelationsProverState& theState, int indexOther, GlobalVariables& theGlobalVariables, WeylGroup& theWeyl)
{ theState.SortAlphasAndBetas(theGlobalVariables, theWeyl);
	minimalRelationsProverState& theOtherState=this->TheObjects[indexOther];
	roots& theAlphas = theState.PartialRelation.Alphas;
	roots& theBetas  = theState.PartialRelation.Betas;
	roots& theOtherAlphas = theOtherState.PartialRelation.Alphas;
	roots& theOtherBetas  = theOtherState.PartialRelation.Betas;
	if (theAlphas.size!=theOtherAlphas.size)
		return false;
	if (theBetas.size!=theOtherBetas.size)
		return false;
  DynkinDiagramRootSubalgebra diagram1, diagram2;
  roots tempRoots1, tempRoots2;
  tempRoots1.CopyFromBase(this->theK.SimpleBasisK);  tempRoots1.AddListOnTop(theAlphas);
  diagram1.ComputeDiagramType(tempRoots1, this->theWeylGroup);
  tempRoots2.CopyFromBase(this->theK.SimpleBasisK); tempRoots2.AddListOnTop(theOtherAlphas);
  diagram2.ComputeDiagramType(tempRoots2, this->theWeylGroup);
  if (!(diagram1==diagram2))
    return false;
  tempRoots1.AddListOnTop(theBetas);
  diagram1.ComputeDiagramType(tempRoots1, this->theWeylGroup);
  tempRoots2.AddListOnTop(theOtherBetas);
  diagram2.ComputeDiagramType(tempRoots2, this->theWeylGroup);
  if (!(diagram1==diagram2))
    return false;
  tempRoots1.CopyFromBase(this->theK.SimpleBasisK);  tempRoots1.AddListOnTop(theBetas);
  diagram1.ComputeDiagramType(tempRoots1, this->theWeylGroup);
  tempRoots2.CopyFromBase(this->theK.SimpleBasisK); tempRoots2.AddListOnTop(theOtherBetas);
  diagram2.ComputeDiagramType(tempRoots2, this->theWeylGroup);
  if (!(diagram1==diagram2))
    return false;
  ListBasicObjects<int> tempList;
	permutation thePermAlphas, thePermBetas, tempPermAlphas, tempPermBetas;
  this->GetIsoTypicComponents(theAlphas, theBetas, thePermAlphas, theState, theWeyl,  theGlobalVariables);
  this->GetIsoTypicComponents(theBetas, theAlphas, thePermBetas, theState, theWeyl, theGlobalVariables);
  this->GetIsoTypicComponents(theOtherAlphas, theOtherBetas, tempPermAlphas, theOtherState, theWeyl,  theGlobalVariables);
  this->GetIsoTypicComponents(theOtherBetas, theOtherAlphas, tempPermBetas, theOtherState, theWeyl, theGlobalVariables);
  if (!thePermAlphas.HasSameMaxMultiplicities(tempPermAlphas) || !thePermBetas.HasSameMaxMultiplicities(tempPermBetas))
    return false;
  for(int i=0; i<this->theIsos.size;i++)
  { this->theIsos.ActByElement(i, theAlphas, tempRoots1);
    this->theIsos.ActByElement(i, theBetas, tempRoots2);
    if (tempRoots1==theOtherAlphas && tempRoots2==theOtherBetas)
      return true;
  }
	return false;
}

void ReflectionSubgroupWeylGroup::ActByElement(int index, root& theRoot)
{ root tempRoot;
  this->ActByElement(index, theRoot, tempRoot);
  theRoot.Assign(tempRoot);
}

void ReflectionSubgroupWeylGroup::ActByElement(int index, root& input, root& output)
{	assert(&input!=&output);
		ElementWeylGroup& tempEW= this->TheObjects[index];
		int NumElts=tempEW.size;
		root tempRoot;
		output.Assign(input);
		for (int i=0;i<NumElts;i++)
		{	int tempI=tempEW.TheObjects[i];
				if(tempI<this->simpleGenerators.size)
					this->AmbientWeyl.ReflectBetaWRTAlpha(this->simpleGenerators.TheObjects[tempI], output, false, output);
				else
				{	tempI-=this->simpleGenerators.size;
						this->ExternalAutomorphisms.TheObjects[tempI].MakeBasisChange(output,tempRoot);
						output.Assign(tempRoot);
				}
		}
}

void ReflectionSubgroupWeylGroup::ActByElement(int index, roots& input, roots& output)
{ assert(&input!=&output);
    output.SetSizeExpandOnTopNoObjectInit(input.size);
    for (int i=0; i<input.size; i++)
      this->ActByElement(index, input.TheObjects[i], output.TheObjects[i]);
}

void minimalRelationsProverStates::ComputeLastStackIndexFixedK(WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{ int index= *this->theIndexStack.LastObject();
    this->MakeProgressReportCurrentState(index, TheGlobalVariables, theWeyl);
    if (!this->TheObjects[index].StateIsPossible || this->TheObjects[index].StateIsComplete)
      return;
  /*if (this->TheObjects[index].theChoicesWeMake.size>=8)
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
        if (theState.StateAllowsPositiveKChoice(theWeyl.RootSystem.TheObjects[j] ,firstRoot, TheGlobalVariables, theWeyl))
          TotalCounter++;
      for (int j=0; j<theWeyl.RootSystem.size;j++)
        if (theState.StateAllowsPositiveKChoice(theWeyl.RootSystem.TheObjects[j] ,firstRoot, TheGlobalVariables, theWeyl))
        { this->MakeProgressReportChildStates
            (Counter2,TotalCounter,this->TheObjects[index].PossibleChildStates.size, TheGlobalVariables, theWeyl);
          Counter2++;
          newState.Assign(theState);
          newState.ChosenPositiveKroots.AddObjectOnTop(theWeyl.RootSystem.TheObjects[j]);
          newState.nonLNonSingularsAleviatedByChosenPosKRoots.AddObjectOnTop(theWeyl.RootSystem.TheObjects[j]);
          this->ExtensionStep(index, theWeyl,TheGlobalVariables,newState);
        }
      return;
    }*/
    root theBeta, theAlpha, theMinusAlpha, theMinusBeta;
    this->TheObjects[index].PossibleChildStates.size=0;
    minimalRelationsProverState newState;
    int theDimension=theWeyl.KillingFormMatrix.NumRows;
    Rational tempRat;
    if (!roots::ConesIntersect( TheGlobalVariables, this->TheObjects[index].PartialRelation.Alphas, this->TheObjects[index].NilradicalRoots, theDimension))
    {	root NormalSeparatingCones;
        bool oneBetaIsPositive = this->GetNormalSeparatingConesReturnTrueIfOneBetaIsPositive(index, NormalSeparatingCones, theWeyl, TheGlobalVariables);
        this->TheObjects[index].ComputeDebugString(theWeyl, TheGlobalVariables);
        this->MakeProgressReportCurrentState(index, TheGlobalVariables, theWeyl);
        bool addFirstAlpha=true;
        for (int i=0; i<theWeyl.RootSystem.size; i++)
        {	this->TestAddingExtraRoot( index, theWeyl, TheGlobalVariables, theWeyl.RootSystem.TheObjects[i], addFirstAlpha, i, NormalSeparatingCones, oneBetaIsPositive);
            this->MakeProgressReportChildStates( i, theWeyl.RootSystem.size*2, this->TheObjects[index].PossibleChildStates.size, TheGlobalVariables, theWeyl);
        }
        for (int i=0;i<theWeyl.RootSystem.size;i++)
        {	this->TestAddingExtraRoot( index, theWeyl, TheGlobalVariables, theWeyl.RootSystem.TheObjects[i], !addFirstAlpha, i, NormalSeparatingCones, oneBetaIsPositive);
            this->MakeProgressReportChildStates
              ( i+theWeyl.RootSystem.size, theWeyl.RootSystem.size*2, this->TheObjects[index].PossibleChildStates.size, TheGlobalVariables, theWeyl);
        }
    }
    else
    {	this->MakeProgressReportCurrentState(index, TheGlobalVariables, theWeyl);
        this->TheObjects[index].StateIsComplete=true;
    }
//	this->TheObjects[index].ComputeDebugString(theWeyl, TheGlobalVariables);*/
}
