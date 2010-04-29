#include "polyhedra.h"

int rootSubalgebra::ProblemCounter=0;
bool rootSubalgebra::GenerateAutomorphisms
	( rootSubalgebra& right, GlobalVariables& theGlobalVariables, ReflectionSubgroupWeylGroup* outputAutomorphisms,
    bool actOnCentralizerOnly)
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
  for (int i=0;i<DiagramAutomorphisms.size;i++)
		tempAutos.MaxMultiplicities.TheObjects[i]=
			DiagramAutomorphisms.TheObjects[i].size-1;
	for (int i=0;i<CentralizerDiagramAutomorphisms.size;i++)
		tempAutosCentralizer.MaxMultiplicities.TheObjects[i]=
			CentralizerDiagramAutomorphisms.TheObjects[i].size-1;
  tempList.SetSizeExpandOnTopNoObjectInit
    (this->theDynkinDiagram.sameTypeComponents.size);
  int tempSize=0;
  for (int i=0;i<this->theDynkinDiagram.sameTypeComponents.size;i++)
  { tempList.TheObjects[i]= this->theDynkinDiagram.sameTypeComponents.TheObjects[i].size;
    tempSize+=tempList.TheObjects[i];
  }
  permComponents.initPermutation(tempList,tempSize);
  tempList.SetSizeExpandOnTopNoObjectInit
    (this->theCentralizerDiagram.sameTypeComponents.size);
  tempSize=0;
  for (int i=0;i<this->theCentralizerDiagram.sameTypeComponents.size;i++)
  { tempList.TheObjects[i]= this->theCentralizerDiagram.sameTypeComponents.TheObjects[i].size;
    tempSize+=tempList.TheObjects[i];
  }
  permComponentsCentralizer.initPermutation(tempList,tempSize);
	int tempI1;
	int NumAutos;
  //if (!actOnCentralizerOnly)
  { tempI1= permComponents.getTotalNumSubsets();
		NumAutos=tempAutos.getTotalNumSubsets();
	} //else
	//{ tempI1=1;
	//	NumAutos=1;
	//}
	int tempI2= permComponentsCentralizer.getTotalNumSubsets();
	int NumAutosCentralizer= tempAutosCentralizer.getTotalNumSubsets();
  permComponents.GetPermutation(tempPermutation1);
  permComponentsCentralizer.GetPermutation(tempPermutation2);
  for (int i=0;i<tempI1;i++)
  { for(int j=0;j<tempI2;j++)
    { for (int k=0;k<NumAutos;k++)
			{	for(int l=0;l<NumAutosCentralizer;l++)
				{	isoDomain.size=0; isoRange.size=0;
					this->theDynkinDiagram.GetMapFromPermutation
						(	isoDomain,isoRange,tempPermutation1,DiagramAutomorphisms,
							tempAutos, right.theDynkinDiagram);
					this->theCentralizerDiagram.GetMapFromPermutation
						( isoDomain, isoRange, tempPermutation2, CentralizerDiagramAutomorphisms,
							tempAutosCentralizer, right.theCentralizerDiagram);
//					isoDomain.ComputeDebugString();
//					isoRange.ComputeDebugString();
					//bool GenerateAllAutos=false;
					//if (outputAutomorphisms!=0 && k==0 && l==0)
					//	GenerateAllAutos=true;
					if (this->attemptExtensionToIsomorphismNoCentralizer
								(isoDomain,isoRange,theGlobalVariables,0, outputAutomorphisms,false,0,0,0))//GenerateAllAutos))
						if (outputAutomorphisms==0)
							return true;
					if (outputAutomorphisms!=0)
						this->MakeProgressReportGenAutos
							( l+NumAutosCentralizer*(k+NumAutos*(j+i*tempI2)),
								tempI1*tempI2*NumAutos*NumAutosCentralizer,outputAutomorphisms->ExternalAutomorphisms.size,
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


void minimalRelationsProverStates::GenerateStartingStatesFixedK
  ( ComputationSetup& theSetup, GlobalVariables& TheGlobalVariables, char WeylLetter, int theDimension)
{ minimalRelationsProverState tempState;
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
  this->theIsos.ComputeSubGroupFromGeneratingReflections
    (this->theK.SimpleBasisCentralizerRoots, this->theIsos.ExternalAutomorphisms, TheGlobalVariables, -1, true);
  this->AddObjectOnTop(tempState);
  ListBasicObjects<int> kModOrbitInfo;
  ListBasicObjects<int> kModOrbitRepresentatives;
  kModOrbitInfo.initFillInObject(this->theK.kModules.size,-1);
  kModOrbitRepresentatives.size=0;
  rootsCollection IsoOrbits;
  roots tempRoots;
  for (int i=0;i<this->theK.kModules.size;i++)
    if (kModOrbitInfo.TheObjects[i]==-1)
    { this->theIsos.GenerateOrbitReturnFalseIfTruncated(this->theK.kModules.TheObjects[i].TheObjects[0], tempRoots,0);
      kModOrbitInfo.TheObjects[i]=i;
      kModOrbitRepresentatives.AddObjectOnTop(i);
      for (int j=1; j<tempRoots.size;j++)
        kModOrbitInfo.TheObjects[this->theK.GetIndexKmoduleContainingRoot(tempRoots.TheObjects[j])]=i;
    }
  for(int i=0;i<kModOrbitRepresentatives.size;i++)
  { root& theHW= this->theK.HighestWeightsGmodK.TheObjects[kModOrbitRepresentatives.TheObjects[i]];
    tempState.theChoicesWeMake.AddObjectOnTop(theHW);
    tempState.PartialRelation.Alphas.AddObjectOnTop(theHW);
    this->ExtensionStepFixedK(0,this->theWeylGroup,TheGlobalVariables,tempState);
    tempState.theChoicesWeMake.size=0;
    tempState.PartialRelation.Alphas.size=0;
  }
	this->theIndexStack.AddObjectOnTop(0);
  if (this->TheObjects[0].PossibleChildStates.size>0)
  { this->TheObjects[0].activeChild=-1;
    this->theIndexStack.AddObjectOnTop(0);
  }
  this->MakeProgressReportCurrentState(0,TheGlobalVariables, this->theWeylGroup);
  this->flagComputationIsInitialized=true;
}

void ReflectionSubgroupWeylGroup::ComputeSubGroupFromGeneratingReflections
	( roots& generators, rootsCollection& ExternalAutos, GlobalVariables& theGlobalVariables, int UpperLimitNumElements,
		bool recomputeAmbientRho)
{ hashedRoots& orbitRho = theGlobalVariables.hashedRootsComputeSubGroupFromGeneratingReflections;
	this->truncated=false;
//	ExternalAutos.ComputeDebugString();
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
	tempRoot.MultiplyByInteger(3);
	tempRoot.TheObjects[0].AddInteger(1);
	//orbitRho.AddObjectOnTopHash(this->AmbientWeyl.rho);
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

bool ReflectionSubgroupWeylGroup::GenerateOrbitReturnFalseIfTruncated
  (root& input, roots& outputOrbit, int UpperLimitNumElements)
{ hashedRoots theOrbit;
	bool result = true;
	this->ClearTheObjects();
	theOrbit.ClearTheObjects();
	root tempRoot=input;
	theOrbit.AddObjectOnTopHash(tempRoot);
	for (int i=0;i<theOrbit.size;i++)
	{ for (int j=0; j<this->simpleGenerators.size;j++)
		{ this->AmbientWeyl.ReflectBetaWRTAlpha
				(this->simpleGenerators.TheObjects[j], theOrbit.TheObjects[i], false, tempRoot);
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

void minimalRelationsProverStates::ExtensionStepFixedK
  ( int index, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables, minimalRelationsProverState& newState)
{ /*newState.ComputeStateReturnFalseIfDubious(TheGlobalVariables, theWeyl,this->flagAssumeGlobalMinimalityRHS);
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
  }*/
}
