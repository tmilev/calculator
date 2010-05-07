#include "polyhedra.h"
extern ::IndicatorWindowVariables IndicatorWindowGlobalVariables;
template < > int ListBasicObjects< minimalRelationsProverStateFixedK>::ListBasicObjectsActualSizeIncrement=10;

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

int minimalRelationsProverStatesFixedK::GetModuleIndex(root& input)
{	int tempI= this->theWeylGroup.RootSystem.IndexOfObjectHash(input);
	if (tempI==-1)
		return -1;
	return this->IndexKmoduleByRoots.TheObjects[tempI];
}

void minimalRelationsProverStatesFixedK::initShared(WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{ this->theK.AmbientWeyl.Assign(this->theWeylGroup);
	this->theK.ComputeAll();
  this->IndexKmoduleByRoots.SetSizeExpandOnTopNoObjectInit(this->theWeylGroup.RootSystem.size);
  for (int i=0;i<this->theWeylGroup.RootSystem.size;i++)
		this->IndexKmoduleByRoots.TheObjects[i]=this->theK.GetIndexKmoduleContainingRoot(this->theWeylGroup.RootSystem.TheObjects[i]);
  this->theIsos.AmbientWeyl.Assign(this->theWeylGroup);

}

void minimalRelationsProverStatesFixedK::GenerateStartingStatesFixedK( ComputationSetup& theSetup, GlobalVariables& TheGlobalVariables, char WeylLetter, int theDimension)
{ if (this->flagComputationIsInitialized)
		return;
	minimalRelationsProverStateFixedK tempState; tempState.owner=this;
	this->theWeylGroup.MakeArbitrary(WeylLetter, theDimension);
  this->theWeylGroup.ComputeRho(false);
  this->isomorphismComputer.AmbientWeyl.Assign(this->theWeylGroup);
  this->isomorphismComputer.ComputeAll();
  this->PreferredDualBasis.size=0;
  this->flagAssumeGlobalMinimalityRHS=false;
  this->invertedCartan.AssignMatrixIntWithDen(this->theWeylGroup.KillingFormMatrix,1);
  this->invertedCartan.Invert(TheGlobalVariables);
  //this->ComputePreferredDualBasis(WeylLetter, theDimension, TheGlobalVariables);
  this->size=0;
  this->theK.genK.SetSizeExpandOnTopNoObjectInit(4);
  this->theK.genK.TheObjects[0].InitFromIntegers(8, 1,0,0,0,0,0,0,0);
  this->theK.genK.TheObjects[1].InitFromIntegers(8, 0,1,0,0,0,0,0,0);
  this->theK.genK.TheObjects[2].InitFromIntegers(8, 0,0,0,0,1,0,0,0);
  this->theK.genK.TheObjects[3].InitFromIntegers(8, 0,0,0,0,0,0,1,0);
	this->initShared(this->theWeylGroup,TheGlobalVariables);
	this->theK.GenerateAutomorphisms(this->theK, TheGlobalVariables, &this->theIsos, true);
  this->theK.GenerateKmodMultTable(this->theK.theMultTable, this->theK.theOppositeKmods,TheGlobalVariables);
  this->theIsos.simpleGenerators.size=0;
  this->theIsos.ComputeSubGroupFromGeneratingReflections(this->theK.SimpleBasisCentralizerRoots, this->theIsos.ExternalAutomorphisms, TheGlobalVariables, 20000, true);
  tempState.theNilradicalModules.init(this->theK.kModules.size);
  tempState.theGmodLmodules.init(this->theK.kModules.size);
  this->AddObjectOnTop(tempState);
  int numParabolics=MathRoutines::TwoToTheNth(this->theK.SimpleBasisCentralizerRoots.size);
  //this->theIsos.simpleGenerators.ComputeDebugString();
  //this->theK.SimpleBasisCentralizerRoots.ComputeDebugString();
  Selection selCentralizerNilradical;
  selCentralizerNilradical.init(this->theK.SimpleBasisCentralizerRoots.size);
  roots InitialCentralizerNilradicalChoicePositiveSimpleRoots, tempRoots, tempRoots2;
  for (int i=0; i<numParabolics; i++)
  { tempState.initFromParentState(this->TheObjects[0]);
    for (int j=0; j<this->theWeylGroup.RootSystem.size; j++)
    { root& tempRoot=this->theWeylGroup.RootSystem.TheObjects[j];
      if (this->theK.rootIsInCentralizer(tempRoot))
      { if (this->theK.rootIsInNilradicalParabolicCentralizer(selCentralizerNilradical, tempRoot))
          tempState.theNilradicalModules.AddSelectionAppendNewIndex(this->theK.GetIndexKmoduleContainingRoot(tempRoot));
        else
          tempState.theGmodLmodules.AddSelectionAppendNewIndex(this->theK.GetIndexKmoduleContainingRoot(tempRoot));
      }
    }
    InitialCentralizerNilradicalChoicePositiveSimpleRoots.size=0;
    tempState.indicesIsosRespectingInitialNilradicalChoice.size=0;
    for (int i=0;i<selCentralizerNilradical.CardinalitySelection;i++)
      InitialCentralizerNilradicalChoicePositiveSimpleRoots.AddObjectOnTop(this->theK.SimpleBasisCentralizerRoots.TheObjects[selCentralizerNilradical.elements[i]]);
    for (int i=0; i<this->theIsos.size; i++)
    { this->theIsos.ActByElement(i, InitialCentralizerNilradicalChoicePositiveSimpleRoots, tempRoots);
      tempRoots.intersectWith(InitialCentralizerNilradicalChoicePositiveSimpleRoots, tempRoots2);
      assert(tempRoots.size==InitialCentralizerNilradicalChoicePositiveSimpleRoots.size);
      if (tempRoots2.size==tempRoots.size)
        tempState.indicesIsosRespectingInitialNilradicalChoice.AddObjectOnTop(i);
    }
    this->ExtensionStepFixedK(0, theWeylGroup, TheGlobalVariables, tempState);
    selCentralizerNilradical.incrementSelection();
  }
	this->theIndexStack.AddObjectOnTop(0);
	this->TheObjects[0].activeChild=-1;
  this->MakeProgressReportCurrentState(0,TheGlobalVariables, this->theWeylGroup);
  //this->WriteToFile(TheGlobalVariables);
  this->theFile.flush();
  this->flagComputationIsInitialized=true;
}

void ReflectionSubgroupWeylGroup::WriteToFile(std::fstream& output, GlobalVariables& theGlobalVariables)
{	output << "generator_reflections: ";
	this->simpleGenerators.WriteToFile(output, theGlobalVariables);
	output <<"\nouter_generators: ";
	this->ExternalAutomorphisms.WriteToFile(output, theGlobalVariables);
}

void ReflectionSubgroupWeylGroup::ReadFromFile(std::fstream& input, GlobalVariables& theGlobalVariables)
{	std::string tempS;
	input>> tempS;
	this->simpleGenerators.ReadFromFile(input, theGlobalVariables);
	input>> tempS;
	this->ExternalAutomorphisms.ReadFromFile(input, theGlobalVariables);
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
		{ this->AmbientWeyl.ReflectBetaWRTAlpha(this->simpleGenerators.TheObjects[j],orbitRho.TheObjects[i],false,currentRoot);
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

void minimalRelationsProverStatesFixedK::ExtensionStepFixedK( int index, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables, minimalRelationsProverStateFixedK& newState)
{ newState.ComputeStateReturnFalseIfDubious(TheGlobalVariables, theWeyl, this->flagAssumeGlobalMinimalityRHS);
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

bool minimalRelationsProverStatesFixedK::AddObjectOnTopNoRepetitionOfObjectFixedK
  ( int ParentIndex, minimalRelationsProverStateFixedK& theState, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{ for (int i=0;i<this->size;i++)
    if (this->ExtendToIsomorphismRootSystemFixedK(theState,i,TheGlobalVariables,theWeyl))
      return false;
  this->AddObjectOnTop(theState);
  return true;
}

bool minimalRelationsProverStatesFixedK::ExtendToIsomorphismRootSystemFixedK( minimalRelationsProverStateFixedK& theState, int indexOther, GlobalVariables& theGlobalVariables, WeylGroup& theWeyl)
{ theState.SortAlphasAndBetas(theGlobalVariables, theWeyl);
	minimalRelationsProverStateFixedK& theOtherState=this->TheObjects[indexOther];
	roots& theAlphas = theState.PartialRelation.Alphas;
	roots& theBetas  = theState.PartialRelation.Betas;
	roots& theOtherAlphas = theOtherState.PartialRelation.Alphas;
	roots& theOtherBetas  = theOtherState.PartialRelation.Betas;
	if (theAlphas.size!=theOtherAlphas.size)
		return false;
	if (theBetas.size!=theOtherBetas.size)
		return false;
	if (theOtherState.indicesIsosRespectingInitialNilradicalChoice!= theState.indicesIsosRespectingInitialNilradicalChoice)
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
  roots tempRoots3, tempRoots4;
  for(int i=0; i<theState.indicesIsosRespectingInitialNilradicalChoice.size;i++)
  { this->theIsos.ActByElement(theState.indicesIsosRespectingInitialNilradicalChoice.TheObjects[i], theAlphas, tempRoots1);
    this->theIsos.ActByElement(theState.indicesIsosRespectingInitialNilradicalChoice.TheObjects[i], theBetas, tempRoots2);
    theOtherAlphas.intersectWith(tempRoots1, tempRoots3);
    theOtherBetas.intersectWith(tempRoots2,tempRoots4);
    if (tempRoots1.size==tempRoots3.size&& tempRoots2.size==tempRoots4.size)
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
{ assert(&input!=&output);
	ElementWeylGroup& tempEW= this->TheObjects[index];
	int NumElts=tempEW.size;
	root tempRoot;
	output.Assign(input);
	for (int i=0; i<NumElts; i++)
	{ int tempI=tempEW.TheObjects[i];
		if(tempI<this->simpleGenerators.size)
			this->AmbientWeyl.ReflectBetaWRTAlpha(this->simpleGenerators.TheObjects[tempI], output, false, output);
		else
		{ tempI-=this->simpleGenerators.size;
			this->ExternalAutomorphisms.TheObjects[tempI].MakeBasisChange(output, tempRoot);
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

void minimalRelationsProverStatesFixedK::ComputeLastStackIndexFixedK(WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{ int index= *this->theIndexStack.LastObject();
	this->MakeProgressReportCurrentState(index, TheGlobalVariables, theWeyl);
	if (this->TheObjects[index].PartialRelation.Alphas.size==0 && this->TheObjects[index].PartialRelation.Betas.size==0)
	{	::minimalRelationsProverStateFixedK theNewState;
		for (int i=0;i<this->theK.HighestWeightsGmodK.size;i++)
		{ theNewState.initFromParentState(this->TheObjects[index]);
			theNewState.PartialRelation.Alphas.AddObjectOnTop(this->theK.HighestWeightsGmodK.TheObjects[i]);
			theNewState.theChoicesWeMake.AddObjectOnTop(this->theK.HighestWeightsGmodK.TheObjects[i]);
			this->ExtensionStepFixedK(index, theWeyl, TheGlobalVariables, theNewState);
			this->MakeProgressReportChildStates( i, this->theK.HighestWeightsGmodK.size, this->TheObjects[index].PossibleChildStates.size, TheGlobalVariables, theWeyl);
		}
		return;
	}
	if (!this->TheObjects[index].StateIsPossible || this->TheObjects[index].StateIsComplete)
		return;
	root theBeta, theAlpha, theMinusAlpha, theMinusBeta;
  this->TheObjects[index].PossibleChildStates.size=0;
  minimalRelationsProverState newState;
  int theDimension=theWeyl.KillingFormMatrix.NumRows;
  Rational tempRat;
  roots theNilradicalRoots, tempRoots;
  this->TheObjects[index].GetCertainGmodLhighestAndNilradicalRoots(tempRoots, theNilradicalRoots, theWeyl);  
  if (!roots::ConesIntersect( TheGlobalVariables, theNilradicalRoots, this->TheObjects[index].PartialRelation.Alphas, theDimension))
  { root NormalSeparatingCones;
    bool oneBetaIsPositive = this->GetNormalSeparatingConesReturnTrueIfOneBetaIsPositive(index, NormalSeparatingCones, theWeyl, TheGlobalVariables);
    this->TheObjects[index].ComputeDebugString(theWeyl, TheGlobalVariables);
    this->MakeProgressReportCurrentState(index, TheGlobalVariables, theWeyl);
    bool addFirstAlpha=true;
    if (this->flagSearchForOptimalSeparatingRoot)
		{	int minNumChildren=theWeyl.RootSystem.size;
			int oldSize=this->size;
			root tempNormal;
			bool tempBetaPos;
			for (int i=-1;	i<theWeyl.RootsOfBorel.size; i++)
			{ if (i!=-1) 
					tempNormal= theWeyl.RootsOfBorel.TheObjects[i];
				else
					tempNormal= NormalSeparatingCones;
				if (this->TheObjects[index].IsSeparatingCones(tempNormal, tempBetaPos, theWeyl))
				{	this->InvokeExtensionOfState(index, minNumChildren, tempBetaPos, tempNormal, true, theWeyl, TheGlobalVariables);
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
					{	::IndicatorWindowGlobalVariables.String3NeedsRefresh=true;
						std::stringstream out3;
						out3<< i+1 <<" out of  "<< theWeyl.RootsOfBorel.size<<" normals tested, best hit "<<minNumChildren<<" children";
						::IndicatorWindowGlobalVariables.ProgressReportString3= out3.str();
						TheGlobalVariables.FeedDataToIndicatorWindowDefault(IndicatorWindowGlobalVariables);
					}
				}
			}
		}
		NormalSeparatingCones.ComputeDebugString();
		this->TheObjects[index].SeparatingNormalUsed.Assign(NormalSeparatingCones);
		this->InvokeExtensionOfState(index,-1, oneBetaIsPositive, NormalSeparatingCones, addFirstAlpha, theWeyl, TheGlobalVariables);
  }
  else
  { this->MakeProgressReportCurrentState(index, TheGlobalVariables, theWeyl);
    this->TheObjects[index].StateIsComplete=true;
  }
//	this->TheObjects[index].ComputeDebugString(theWeyl, TheGlobalVariables);*/
}

void ::minimalRelationsProverStatesFixedK::InvokeExtensionOfState
	(int index, int UpperLimitChildren, bool oneBetaIsPositive, root& NormalSeparatingCones, bool addFirstAlpha, WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{	for (int i=0; i<theWeyl.RootSystem.size; i++)
	{	if (UpperLimitChildren>=0)
			if (this->TheObjects[index].PossibleChildStates.size>=UpperLimitChildren)
				return;
		this->TestAddingExtraRootFixedK
			( index, theWeyl, TheGlobalVariables, theWeyl.RootSystem.TheObjects[i], addFirstAlpha, i, NormalSeparatingCones, oneBetaIsPositive);
		this->MakeProgressReportChildStates( i, theWeyl.RootSystem.size*2, this->TheObjects[index].PossibleChildStates.size, TheGlobalVariables, theWeyl);
	}
	for (int i=0; i<theWeyl.RootSystem.size; i++)
	{ if (UpperLimitChildren>=0)
			if (this->TheObjects[index].PossibleChildStates.size>=UpperLimitChildren)
				return;
		this->TestAddingExtraRootFixedK
			( index, theWeyl, TheGlobalVariables, theWeyl.RootSystem.TheObjects[i], !addFirstAlpha, i, NormalSeparatingCones, oneBetaIsPositive);
		this->MakeProgressReportChildStates
			( i+theWeyl.RootSystem.size, theWeyl.RootSystem.size*2, this->TheObjects[index].PossibleChildStates.size, TheGlobalVariables, theWeyl);
	}
}

bool minimalRelationsProverStateFixedK::ComputeCommonSenseImplicationsReturnFalseIfContradictionFixedK(WeylGroup& theWeyl, GlobalVariables& TheGlobalVariables)
{ root tempRoot;
	rootSubalgebra& kAlg= this->owner->theK;
	for (int i=0; i<this->theNilradicalModules.CardinalitySelection; i++)
    for(int j=0; j<this->theNilradicalModules.CardinalitySelection; j++)
		{ int i1= this->theNilradicalModules.elements[i];
			int i2= this->theNilradicalModules.elements[j];
			for (int k=0; k<kAlg.theMultTable.TheObjects[i1].TheObjects[i2].size;k++)
				this->theNilradicalModules.AddSelectionAppendNewIndex(kAlg.theMultTable.TheObjects[i1].TheObjects[i2].TheObjects[k]);
		}
	for (int i=0; i<this->theNilradicalModules.CardinalitySelection; i++)
	{ int theIndex = this->theNilradicalModules.elements[i];
		if (this->theGmodLmodules.selected[theIndex])
			return false;
		if (this->theNilradicalModules.selected[this->owner->theK.theOppositeKmods.TheObjects[theIndex]])
			return false;
  }
  for (int i=0; i<this->theGmodLmodules.CardinalitySelection; i++)
	{ int theIndex = this->theGmodLmodules.elements[i];
		if (this->theNilradicalModules.selected[theIndex])
			return false;
	}
	for(int i=0;i<this->owner->theK.kModules.size;i++)
    if (!this->theNilradicalModules.selected[i]&& !this->theGmodLmodules.selected[i])
    { for (int k=0; k<this->PartialRelation.Alphas.size; k++)
      { if (theWeyl.IsARoot(this->PartialRelation.Alphas.TheObjects[k]+this->owner->theK.HighestWeightsGmodK.TheObjects[i]))
        { this->nonAlphas.AddObjectOnTopNoRepetitionOfObject(this->owner->theK.HighestWeightsGmodK.TheObjects[i]);
          break;
        }
       }
      for (int j=0;j<this->owner->theK.kModules.TheObjects[i].size;j++)
      { root& theRoot= this->owner->theK.kModules.TheObjects[i].TheObjects[j];
        for (int k=0; k<this->PartialRelation.Betas.size; k++)
        {	theRoot.ComputeDebugString();  this->PartialRelation.Betas.TheObjects[k].ComputeDebugString();
					if (theWeyl.IsARoot(this->PartialRelation.Betas.TheObjects[k]+theRoot))
          { this->nonBetas.AddObjectOnTopNoRepetitionOfObject(theRoot);
            break;
          }
         }
      }
    }
  return true;
}
