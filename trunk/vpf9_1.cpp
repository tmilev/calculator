//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader1_2.h"

ProjectInformationInstance ProjectInfoVpf9_1cpp(__FILE__, "Main implementation file, part 2. ");

bool rootSubalgebra::ConeConditionHolds(GlobalVariables& theGlobalVariables, rootSubalgebras& owner, int indexInOwner, bool doExtractRelations)
{ Vectors<Rational>& NilradicalRoots= theGlobalVariables.rootsNilradicalRoots.GetElement();
  Vectors<Rational>& Ksingular=theGlobalVariables.rootsConeConditionHolds2.GetElement();
  if (this->kModules.size==0)
    return true;
  NilradicalRoots.size=0;
  int counter=0;
  for (int i=0; i<this->NilradicalKmods.CardinalitySelection; i++)
  { Vectors<Rational>& tempKmod= this->kModules.TheObjects[this->NilradicalKmods.elements[i]];
    for (int j=0; j<tempKmod.size; j++)
    { NilradicalRoots.AddOnTop(tempKmod.TheObjects[j]);
      counter++;
    }
  }
  Ksingular.size=0;
  for (int i=0; i<this->kModules.size; i++)
    if (!this->NilradicalKmods.selected[i])
      Ksingular.AddOnTop(this->HighestWeightsGmodK.TheObjects[i]);
  if ( !this->ConeConditionHolds(theGlobalVariables, owner, indexInOwner, NilradicalRoots, Ksingular, doExtractRelations))
    return false;
  else
    return true;
}

bool rootSubalgebra::CheckForSmallRelations(coneRelation& theRel, Vectors<Rational>& nilradicalRoots)
{ //return false;
  Vector<Rational> tempRoot;
  bool tempBool; int tempI;
  for (int i=0; i<this->kModules.size; i++)
    if (!this->NilradicalKmods.selected[i])
      for (int j=i+1; j<this->kModules.size; j++)
      { if (!this->NilradicalKmods.selected[j])
        { tempRoot=(this->HighestWeightsGmodK.TheObjects[i]);
          tempRoot+=(this->HighestWeightsGmodK.TheObjects[j]);
          if (!tempRoot.IsEqualToZero())
          { theRel.BetaCoeffs.SetSize(0);
            theRel.Betas.SetSize(0);
            tempI= nilradicalRoots.GetIndex(tempRoot);
            if (tempI!=-1)
            { tempBool=true;
              theRel.BetaCoeffs.SetSize(1);
              theRel.Betas.SetSize(1);
              theRel.BetaCoeffs.TheObjects[0].MakeOne();
              theRel.Betas.TheObjects[0]=(nilradicalRoots.TheObjects[tempI]);
            } else
              tempBool=
              this->GetAmbientWeyl().HasStronglyPerpendicularDecompositionWRT
              (tempRoot, -1, nilradicalRoots, theRel.Betas, theRel.BetaCoeffs, true)
              ;
            if (tempBool)
            { theRel.Alphas.size=0;
              theRel.AlphaCoeffs.size=0;
              theRel.Alphas.AddOnTop(this->HighestWeightsGmodK.TheObjects[i]);
              theRel.Alphas.AddOnTop(this->HighestWeightsGmodK.TheObjects[j]);
              theRel.AlphaCoeffs.AddOnTop(1);
              theRel.AlphaCoeffs.AddOnTop(1);
//              theRel.ComputeDebugString(*this, true);
              return true;
            }
          }
        }
      }
  return false;
}

void rootSubalgebra::MatrixToRelation( coneRelation& output, Matrix<Rational> & matA, Matrix<Rational> & matX, int theDimension, Vectors<Rational>& NilradicalRoots)
{ output.AlphaCoeffs.size=0;  output.Alphas.size=0;
  output.BetaCoeffs.size=0;  output.Betas.size=0;
  Vector<Rational> tempRoot; tempRoot.SetSize(theDimension);
  matX.ScaleToIntegralForMinRationalHeightNoSignChange();
  assert(matA.NumCols==matX.NumRows);
  for (int i=0; i<matA.NumCols; i++)
    if (!matX.elements[i][0].IsEqualToZero())
    { for (int j=0; j<theDimension; j++)
        tempRoot.TheObjects[j].Assign(matA.elements[j][i]);
      assert(matX.elements[i][0].DenShort==1);
      if (i<NilradicalRoots.size)
      { output.Betas.AddOnTop(tempRoot);
        output.BetaCoeffs.AddOnTop(matX.elements[i][0]);
      } else
      { tempRoot.Minus();
        output.Alphas.AddOnTop(tempRoot);
        output.AlphaCoeffs.AddOnTop(matX.elements[i][0]);
      }
    }
}

void rootSubalgebra::ExtractRelations
(Matrix<Rational>& matA, Matrix<Rational>& matX, Vectors<Rational>& NilradicalRoots, rootSubalgebras& owner,
 int indexInOwner, GlobalVariables& theGlobalVariables, Vectors<Rational>& Ksingular)
{ int theDimension= this->GetOwnerSSalg().GetRank();
  if (this->flagAnErrorHasOccuredTimeToPanic)
  { //this->NilradicalKmods.ComputeDebugString();
    //NilradicalRoots.ComputeDebugString();
    //matX.ComputeDebugString();
  }
  Vector<Rational> tempRoot; tempRoot.MakeZero(theDimension);
  coneRelation theRel; theRel.IndexOwnerRootSubalgebra=indexInOwner;
  if (owner.flagLookingForMinimalRels)
  { theRel.FixRightHandSide(*this, NilradicalRoots);
    theRel.MakeLookCivilized(*this, NilradicalRoots);
    owner.theMinRels.AddRelationNoRepetition(theRel, owner, indexInOwner);
  }
  bool tempBool=this->AttemptTheTripleTrick(theRel, NilradicalRoots, theGlobalVariables);
  if (tempBool)
  { this->NumRelationsWithStronglyPerpendicularDecomposition++;
    theRel.MakeLookCivilized(*this, NilradicalRoots);
    owner.theGoodRelations.AddRelationNoRepetition(theRel, owner, indexInOwner);
  }
  else
  { //if(!this->CheckForSmallRelations(theRel, NilradicalRoots))
    this->MatrixToRelation(theRel, matA, matX, theDimension, NilradicalRoots);
    this->MakeGeneratingSingularVectors(theRel, NilradicalRoots);
    theRel.FixRightHandSide(*this, NilradicalRoots);
    theRel.MakeLookCivilized(*this, NilradicalRoots);
    theRel.ComputeDebugString(owner, true, true);
    if (false)
      if (theRel.theDiagram.ToString()=="C^{2}_3")
      { Selection tempSel;
        tempSel.init(Ksingular.size);
        int tempNum=MathRoutines::NChooseK(Ksingular.size, 2);
        for (int i=0; i<tempNum; i++)
        { tempSel.incrementSelectionFixedCardinality(2);
          theRel.Alphas.SetSize(2);
          theRel.AlphaCoeffs.SetSize(2);
          theRel.Betas.size=0;
          theRel.BetaCoeffs.size=0;
          for (int j=0; j<tempSel.CardinalitySelection; j++)
            theRel.Alphas.TheObjects[j]=(Ksingular.TheObjects[tempSel.elements[j]]);
          if (theRel.IsStrictlyWeaklyProhibiting(*this, NilradicalRoots, theGlobalVariables, owner, indexInOwner))
            break;
        }
      assert(theRel.CheckForBugs(*this, NilradicalRoots));
    }
    owner.theBadRelations.AddOnTop(theRel);
  }
}

bool rootSubalgebra::AttemptTheTripleTrick(coneRelation& theRel, Vectors<Rational>& NilradicalRoots, GlobalVariables& theGlobalVariables)
{ Vectors<Rational>& tempRoots= theGlobalVariables.rootsAttemptTheTripleTrick.GetElement();
  tempRoots.size=0;
  for (int i=0; i<this->kModules.size; i++)
    if (!this->NilradicalKmods.selected[i])
      if (this->IsGeneratingSingularVectors(i, NilradicalRoots))
        tempRoots.AddOnTop(this->HighestWeightsGmodK.TheObjects[i]);
  //tempRoots.ComputeDebugString();
  return this->AttemptTheTripleTrickWRTSubalgebra(theRel, tempRoots, NilradicalRoots, theGlobalVariables);
}

bool rootSubalgebra::AttemptTheTripleTrickWRTSubalgebra(coneRelation& theRel, Vectors<Rational>& highestWeightsAllowed, Vectors<Rational>& NilradicalRoots, GlobalVariables& theGlobalVariables)
{ Vector<Rational> tempRoot, Accum;
  SelectionWithMaxMultiplicity tempSel;
  Vectors<Rational>& chosenAlphas= theGlobalVariables.rootsAttepmtTheTripleTrickWRTSA.GetElement();
  int theRank=this->GetOwnerSSalg().GetRank();
  for (int i=2; i<=MathRoutines::Maximum(highestWeightsAllowed.size, theRank); i++)
  { tempSel.initMaxMultiplicity(highestWeightsAllowed.size, i);
    int NumElts=tempSel.NumCombinationsOfCardinality(i);
    for (int j=0; j<NumElts; j++)
    { tempSel.IncrementSubsetFixedCardinality(i);
      Accum.MakeZero(this->GetAmbientWeyl().CartanSymmetric.NumRows);
      chosenAlphas.size=0;
      for (int k=0; k<tempSel.elements.size; k++)
      { tempRoot=(highestWeightsAllowed.TheObjects[tempSel.elements.TheObjects[k]]);
        chosenAlphas.AddOnTop(tempRoot);
        tempRoot*=(tempSel.Multiplicities.TheObjects[tempSel.elements.TheObjects[k]]);
        Accum+=(tempRoot);
      }
      theRel.Betas.size=0; theRel.BetaCoeffs.size=0;
      if (!Accum.IsEqualToZero())
        if (this->GetAmbientWeyl().HasStronglyPerpendicularDecompositionWRT(Accum, -1, NilradicalRoots, theRel.Betas, theRel.BetaCoeffs, true))
        { int startNumBetas=theRel.Betas.size;
          //int numAlphas=tempSel.CardinalitySelectionWithoutMultiplicities();
          //int numParticipatingRoots=numAlphas+startNumBetas;
          DynkinDiagramRootSubalgebra& theDiagram= theGlobalVariables.dynAttemptTheTripleTrick.GetElement();
          chosenAlphas.AddListOnTop(theRel.Betas);
          //chosenAlphas.ComputeDebugString();
          //theRel.Betas.ComputeDebugString();
          theDiagram.ComputeDiagramTypeModifyInput(chosenAlphas, this->GetAmbientWeyl());
          int theRank=theDiagram.RankTotal();
          if (theRank>4 || theDiagram.ToString()=="B^{2}_4" || theDiagram.ToString()=="C^{2}_4")
          { int goalNumBetas= 2;
            theRel.Betas.size=0; theRel.BetaCoeffs.size=0;
            for (int l=goalNumBetas-1; l<startNumBetas; l++)
              if (this->GetAmbientWeyl().HasStronglyPerpendicularDecompositionWRT
                  (Accum, l+1, NilradicalRoots, theRel.Betas, theRel.BetaCoeffs, true))
                break;
          }
          theRel.Alphas.SetSize(tempSel.CardinalitySelectionWithoutMultiplicities());
          theRel.AlphaCoeffs.SetSize(tempSel.elements.size);
          for (int k=0; k<tempSel.elements.size; k++)
          { theRel.Alphas.TheObjects[k]=(highestWeightsAllowed.TheObjects[tempSel.elements.TheObjects[k]]);
            theRel.AlphaCoeffs.TheObjects[k]=(tempSel.Multiplicities.TheObjects[tempSel.elements.TheObjects[k]]);
          }
          //theRel.Alphas.ComputeDebugString();
          //theRel.Betas.ComputeDebugString();
          return true;
        }
    }
  }
  return false;
}

void rootSubalgebra::MakeSureAlphasDontSumToRoot(coneRelation& theRel, Vectors<Rational>& NilradicalRoots)
{ Vector<Rational> alpha1, alpha2, beta1, tempRoot;
  bool madeChange=true;
  while(madeChange)
  { madeChange=false;
    for (int i=0; i<theRel.Alphas.size; i++)
      for(int j=i+1; j<theRel.Alphas.size; j++)
      { //theRel.ComputeDebugString(*this);
        beta1=(theRel.Alphas.TheObjects[i]);
        beta1+=(theRel.Alphas.TheObjects[j]);
        if (this->IsARootOrZero(beta1))
        { this->ComputeHighestWeightInTheSameKMod(beta1, tempRoot);
          if (NilradicalRoots.Contains(beta1))
          { alpha1=(theRel.Alphas.TheObjects[i]);
            alpha2=(theRel.Alphas.TheObjects[j]);
            theRel.Alphas.SetSize(2);
            theRel.AlphaCoeffs.SetSize(2);
            theRel.Alphas.TheObjects[0]=(alpha1);
            theRel.Alphas.TheObjects[1]=(alpha2);
            theRel.AlphaCoeffs.TheObjects[0]=(1);
            theRel.AlphaCoeffs.TheObjects[1]=(1);
            theRel.Betas.SetSize(1);
            theRel.BetaCoeffs.SetSize(1);
            theRel.BetaCoeffs.TheObjects[0]=1;
            theRel.Betas.TheObjects[0]=beta1;
            madeChange=false;
            break;
          }
          else
          { int changedIndex=i, otherIndex=j;
            Rational alpha1coeff, alpha2coeff;
            if (alpha1coeff.IsGreaterThanOrEqualTo(alpha2coeff))
            { changedIndex=j;
              otherIndex=i;
            }
            alpha1coeff=(theRel.AlphaCoeffs.TheObjects[changedIndex]);
            alpha2coeff=(theRel.AlphaCoeffs.TheObjects[otherIndex]);
            alpha2=(theRel.Alphas.TheObjects[otherIndex]);
            alpha2coeff.Subtract(alpha1coeff);
            madeChange=true;
            theRel.Alphas.TheObjects[changedIndex]=(beta1);
            theRel.AlphaCoeffs.TheObjects[changedIndex]=(alpha1coeff);
            if (alpha2coeff.IsEqualToZero())
            { theRel.Alphas.RemoveIndexSwapWithLast(otherIndex);
              theRel.AlphaCoeffs.RemoveIndexSwapWithLast(otherIndex);
            } else
            { theRel.Alphas.TheObjects[otherIndex]=(alpha2);
              theRel.AlphaCoeffs.TheObjects[otherIndex]=(alpha2coeff);
            }
          }
        }
      }
  }
}

void rootSubalgebra::ComputeEpsCoordsWRTk(GlobalVariables& theGlobalVariables)
{ this->kModulesKepsCoords.SetSize(this->kModules.size);
  this->kModulesgEpsCoords.SetSize(this->kModules.size);
  Matrix<Rational> & InvertedGramMatrix=theGlobalVariables.matComputeEpsCoordsWRTk.GetElement();
  Vectors<Rational>& tempRoots=theGlobalVariables.rootsComputeEpsCoordsWRTk.GetElement();
  Vectors<Rational>& simpleBasisG=theGlobalVariables.rootsComputeEpsCoordsWRTk2.GetElement();
  int theDimension=this->GetAmbientWeyl().CartanSymmetric.NumRows;
  simpleBasisG.SetSize(theDimension);
  for (int i=0; i<theDimension; i++)
  { simpleBasisG[i].MakeZero(theDimension);
    simpleBasisG[i][i]=1;
  }
  this->SimpleBasisK.GetGramMatrix(InvertedGramMatrix, this->GetAmbientWeyl().CartanSymmetric);
  InvertedGramMatrix.Invert(theGlobalVariables);
  Vector<Rational> tempRoot, tempRoot2, tempRoot3;
  for(int i=0; i<this->kModules.size; i++)
  { tempRoots.size=0;
    for (int j=0; j<this->kModules[i].size; j++)
    { tempRoot.SetSize(this->SimpleBasisK.size);
      for (int k=0; k<this->SimpleBasisK.size; k++)
        this->GetAmbientWeyl().RootScalarCartanRoot(this->kModules[i][j], this->SimpleBasisK[k], tempRoot[k]);
      InvertedGramMatrix.ActOnVectorColumn(tempRoot, tempRoot3);
      tempRoot2.MakeZero(this->GetAmbientWeyl().CartanSymmetric.NumRows);
      for (int j=0; j<this->SimpleBasisK.size; j++)
        tempRoot2+=this->SimpleBasisK[j]*tempRoot3[j];
      tempRoots.AddOnTop(tempRoot2);
    }
//    tempRoots.ComputeDebugString();
    this->GetAmbientWeyl().GetEpsilonCoordsWRTsubalgebra(this->SimpleBasisK, tempRoots, this->kModulesKepsCoords[i]);
    this->GetAmbientWeyl().GetEpsilonCoordsWRTsubalgebra(simpleBasisG, this->kModules[i], this->kModulesgEpsCoords[i]);
    Vector<Rational> tempRoot;
    if (this->kModulesKepsCoords[i].size>0)
    { this->kModulesKepsCoords[i].average(tempRoot, this->kModulesKepsCoords[i][0].size);
      assert(tempRoot.IsEqualToZero());
    }
   // this->kModulesgEpsCoords.TheObjects[i].Average
     // (tempRoot, this->kModulesgEpsCoords.TheObjects[i].TheObjects[0].size);
    //assert(tempRoot.IsEqualToZero());
  //  this->kModulesEpsCoords.TheObjects[i].ComputeDebugString();
  }
  this->GetAmbientWeyl().GetEpsilonCoordsWRTsubalgebra(this->SimpleBasisK, this->SimpleBasisK, this->SimpleBasisKEpsCoords);
  this->GetAmbientWeyl().GetEpsilonCoordsWRTsubalgebra(simpleBasisG, this->SimpleBasisK, this->SimpleBasisgEpsCoords);
}

void rootSubalgebra::Assign(const rootSubalgebra& right)
{ this->genK=(right.genK);
  this->owneR=right.owneR;
//  this->indexInOwners=right.indexInOwners;
  this->AllRootsK=(right.AllRootsK);
  this->CentralizerKmods.Assign(right.CentralizerKmods);
  this->CentralizerRoots=(right.CentralizerRoots);
  this->HighestRootsK=(right.HighestRootsK);
  this->HighestWeightsGmodK=(right.HighestWeightsGmodK);
  this->kModules=(right.kModules);
  this->LowestWeightsGmodK=(right.LowestWeightsGmodK);
  this->flagAnErrorHasOccuredTimeToPanic=right.flagAnErrorHasOccuredTimeToPanic;
  this->theDynkinDiagram.Assign(right.theDynkinDiagram);
  this->theCentralizerDiagram.Assign(right.theCentralizerDiagram);
  this->PositiveRootsK=(right.PositiveRootsK);
  this->PosRootsKConnectedComponents=(right.PosRootsKConnectedComponents);
  this->NilradicalKmods.Assign(right.NilradicalKmods);
  this->SimpleBasisCentralizerRoots=(right.SimpleBasisCentralizerRoots);
  this->SimpleBasisK=(right.SimpleBasisK);
  this->indicesSubalgebrasContainingK=(right.indicesSubalgebrasContainingK);
}

void rootSubalgebras::GenerateAllReductiveRootSubalgebrasUpToIsomorphism
(GlobalVariables& theGlobalVariables, char WeylLetter, int WeylRank, bool sort, bool computeEpsCoords)
{ this->GenerateAllReductiveRootSubalgebrasUpToIsomorphism(theGlobalVariables, sort, computeEpsCoords);
}

void rootSubalgebras::GenerateAllReductiveRootSubalgebrasContainingInputUpToIsomorphism
(rootSubalgebras& bufferSAs, int RecursionDepth, GlobalVariables& theGlobalVariables)
{ this->AddOnTop(bufferSAs[RecursionDepth-1]);
  int currentAlgebraIndex=this->size-1;
  rootSubalgebra::ProblemCounter++;
  if (RecursionDepth>=bufferSAs.size)
    bufferSAs.SetSize(bufferSAs.size+this->GetOwnerWeyl().CartanSymmetric.NumRows);
  bufferSAs[RecursionDepth].genK = bufferSAs[RecursionDepth-1].genK;
  bufferSAs[RecursionDepth].owneR=this->owneR;
  ProgressReport theReport(&theGlobalVariables);
  for (int k=0; k<bufferSAs[RecursionDepth-1].kModules.size; k++)
    if (bufferSAs[RecursionDepth-1].HighestWeightsGmodK[k].IsPositiveOrZero())
    { bufferSAs[RecursionDepth].genK.AddOnTop(bufferSAs[RecursionDepth-1].HighestWeightsGmodK[k]);
      bufferSAs[RecursionDepth].ComputeDynkinDiagramKandCentralizer();
      std::stringstream out;
      out << "Included root " << k+1 << " out of " << bufferSAs[RecursionDepth-1].kModules.size
      << " Total found SAs: " << this->size;
      theReport.Report(out.str());
      int indexSA= this->IndexSubalgebra(bufferSAs[RecursionDepth], theGlobalVariables);
      if (indexSA==-1)
      { bufferSAs[RecursionDepth].ComputeAllButAmbientWeyl();
        (*this)[currentAlgebraIndex].indicesSubalgebrasContainingK.AddOnTopNoRepetition(this->size);
        this->GenerateAllReductiveRootSubalgebrasContainingInputUpToIsomorphism
        (bufferSAs, RecursionDepth+1, theGlobalVariables);
      } else
        (*this)[currentAlgebraIndex].indicesSubalgebrasContainingK.AddOnTopNoRepetition(indexSA);
      bufferSAs[RecursionDepth].genK.RemoveIndexSwapWithLast(bufferSAs[RecursionDepth].genK.size-1);
    }
}

void rootSubalgebras::MakeProgressReportAutomorphisms(ReflectionSubgroupWeylGroup& theSubgroup, rootSubalgebra& theRootSA, GlobalVariables& theGlobalVariables)
{ std::stringstream out4, out1;
  out1 << "k_ss: " << theRootSA.theDynkinDiagram.ToString() << " C(k_ss): "
  << theRootSA.theCentralizerDiagram.ToString();
  out4 << "Num elements ";
  if (theSubgroup.truncated)
    out4 << "truncated ";
  out4 << "group preserving k: " << theSubgroup.size;
  ProgressReport theReport(&theGlobalVariables);
  theReport.Report(out1.str()+out4.str());
}

void rootSubalgebras::GenerateActionKintersectBIsos(rootSubalgebra& theRootSA, GlobalVariables& theGlobalVariables)
{ Selection emptySel;
  emptySel.init(theRootSA.SimpleBasisCentralizerRoots.size);
  this->ComputeNormalizerOfCentralizerIntersectNilradical(theGlobalVariables.subGroupActionNormalizerCentralizer.GetElement(), emptySel, theRootSA, theGlobalVariables);
}

void rootSubalgebras::GenerateKintersectBOuterIsos(rootSubalgebra& theRootSA, GlobalVariables& theGlobalVariables)
{ Selection fullSel;
  fullSel.init(theRootSA.SimpleBasisCentralizerRoots.size);
  fullSel.incrementSelectionFixedCardinality(theRootSA.SimpleBasisCentralizerRoots.size);
  this->ComputeNormalizerOfCentralizerIntersectNilradical(theGlobalVariables.subGroupActionNormalizerCentralizer.GetElement(), fullSel, theRootSA, theGlobalVariables);
}

void rootSubalgebras::ComputeActionNormalizerOfCentralizerIntersectNilradical(Selection& SelectedBasisRoots, rootSubalgebra& theRootSA, GlobalVariables& theGlobalVariables)
{ ReflectionSubgroupWeylGroup& theSubgroup=theGlobalVariables.subGroupActionNormalizerCentralizer.GetElement();
  this->ComputeNormalizerOfCentralizerIntersectNilradical(theSubgroup, SelectedBasisRoots, theRootSA, theGlobalVariables);
  this->ActionsNormalizerCentralizerNilradical.SetSize(theSubgroup.size-1);
  Vector<Rational> tempRoot;
  ProgressReport theReport(&theGlobalVariables);
  for(int i=0; i<theSubgroup.size-1; i++)
  { this->ActionsNormalizerCentralizerNilradical[i].SetSize(theRootSA.kModules.size);
    for (int j=0; j<theRootSA.kModules.size; j++)
    { tempRoot=(theRootSA.HighestWeightsGmodK[j]);
      theSubgroup.ActByElement(i+1, tempRoot);
      int tempI=theRootSA.GetIndexKmoduleContainingRoot(tempRoot);
      this->ActionsNormalizerCentralizerNilradical[i][j]= tempI;
    //  for (int k=0; k<theRootSA.kModules.TheObjects[j].size; k++)
    //  { tempRoot.Assign(theRootSA.kModules.TheObjects[j].TheObjects[k]);
    //    theSubgroup.ActByElement(i+1, tempRoot);
    //    assert(tempI==theRootSA.GetIndexKmoduleContainingRoot(tempRoot));
    //  }
    }
    if (theGlobalVariables.GetFeedDataToIndicatorWindowDefault()!=0)
    { std::stringstream out;
      out << "Computing action of element " << i+1 << " out of " << theSubgroup.size;
      theReport.Report(out.str());
    }
  }
}

void rootSubalgebras::ComputeNormalizerOfCentralizerIntersectNilradical
(ReflectionSubgroupWeylGroup& outputSubgroup, Selection& SelectedBasisRoots,
 rootSubalgebra& theRootSA, GlobalVariables& theGlobalVariables)
{ Vectors<Rational> selectedRootsBasisCentralizer;
  selectedRootsBasisCentralizer.size=0;
  for (int i=0; i<SelectedBasisRoots.MaxSize; i++)
    if (!SelectedBasisRoots.selected[i])
      selectedRootsBasisCentralizer.AddOnTop(theRootSA.SimpleBasisCentralizerRoots.TheObjects[i]);
  outputSubgroup.AmbientWeyl=(theRootSA.GetAmbientWeyl());
  this->MakeProgressReportAutomorphisms(outputSubgroup, theRootSA, theGlobalVariables);
  theRootSA.GenerateIsomorphismsPreservingBorel
  (theRootSA, theGlobalVariables, &outputSubgroup, true);
  //std::string tempS;
  //theSubgroup.ToString(tempS);
  //theGlobalVariables.theIndicatorVariables.StatusString1=tempS;
  //theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  //theGlobalVariables.FeedIndicatorWindow(theGlobalVariables.theIndicatorVariables);
  outputSubgroup.ComputeSubGroupFromGeneratingReflections
  (&selectedRootsBasisCentralizer, &outputSubgroup.ExternalAutomorphisms, &theGlobalVariables,
   this->UpperLimitNumElementsWeyl, false);
  outputSubgroup.simpleGenerators=(selectedRootsBasisCentralizer);
  this->CentralizerIsomorphisms.ReservE(this->size);
  this->CentralizerOuterIsomorphisms.ReservE(this->size);
  this->CentralizerIsomorphisms.AddOnTop(outputSubgroup);
  this->CentralizerOuterIsomorphisms.SetSize(this->CentralizerIsomorphisms.size);
  this->CentralizerOuterIsomorphisms.LastObject()->ExternalAutomorphisms=(outputSubgroup.ExternalAutomorphisms);
  this->CentralizerOuterIsomorphisms.LastObject()->AmbientWeyl=(this->GetOwnerWeyl());
  this->MakeProgressReportAutomorphisms(outputSubgroup, theRootSA, theGlobalVariables);
  //theSubgroup.ComputeDebugString();
}

bool rootSubalgebras::ApproveKmoduleSelectionWRTActionsNormalizerCentralizerNilradical(Selection& targetSel, GlobalVariables& theGlobalVariables)
{ if (!this->flagUsingActionsNormalizerCentralizerNilradical)
    return true;
  for (int i=0; i<this->ActionsNormalizerCentralizerNilradical.size; i++)
    if (!this->ApproveSelAgainstOneGenerator(this->ActionsNormalizerCentralizerNilradical.TheObjects[i], targetSel, theGlobalVariables))
      return false;
  return true;
}

void rootSubalgebras::RaiseSelectionUntilApproval(Selection& targetSel, GlobalVariables& theGlobalVariables)
{ bool raised=true;
  while (raised)
  { raised=false;
    for (int i=0; i<this->ActionsNormalizerCentralizerNilradical.size; i++)
      if(!this->ApproveSelAgainstOneGenerator(this->ActionsNormalizerCentralizerNilradical.TheObjects[i], targetSel, theGlobalVariables))
      { this->ApplyOneGenerator(this->ActionsNormalizerCentralizerNilradical.TheObjects[i], targetSel, theGlobalVariables);
        raised=true;
      }
  }
}

void rootSubalgebras::ApplyOneGenerator(List<int>& generator, Selection& targetSel, GlobalVariables& theGlobalVariables)
{ Selection& tempSel= theGlobalVariables.selApproveSelAgainstOneGenerator.GetElement();
  tempSel.initNoMemoryAllocation();
  for (int i=0; i<targetSel.CardinalitySelection; i++)
    tempSel.AddSelectionAppendNewIndex(generator.TheObjects[targetSel.elements[i]]);
  targetSel.Assign(tempSel);
}

bool rootSubalgebras::ApproveSelAgainstOneGenerator(List<int>& generator, Selection& targetSel, GlobalVariables& theGlobalVariables)
{ Selection& tempSel= theGlobalVariables.selApproveSelAgainstOneGenerator.GetElement();
  tempSel.initNoMemoryAllocation();
  for (int i=0; i<targetSel.CardinalitySelection; i++)
    tempSel.AddSelectionAppendNewIndex(generator.TheObjects[targetSel.elements[i]]);
  for (int i=0; i<tempSel.MaxSize; i++)
  { if (targetSel.selected[i] && !tempSel.selected[i])
      return true;
    if (!targetSel.selected[i] && tempSel.selected[i])
      return false;
  }
  return true;
}

int rootSubalgebras::IndexSubalgebra(rootSubalgebra& input, GlobalVariables& theGlobalVariables)
{ this->ProblemCounter++;
  int result=-1;
  for (int j=0; j<this->size; j++)
  { rootSubalgebra& right=this->TheObjects[j];
    if (input.theDynkinDiagram.ToString() == right.theDynkinDiagram.ToString() &&
        input.theCentralizerDiagram.ToString()== right.theCentralizerDiagram.ToString())
    { result=j;
      if (!this->flagUseDynkinClassificationForIsomorphismComputation ||
          this->GetOwnerWeyl().IsOfSimpleType('E', 7))
      { input.ComputeAllButAmbientWeyl();
        if(!input.GenerateIsomorphismsPreservingBorel(right, theGlobalVariables, 0, false))
          result=-1;
      }
      if (result!=-1)
        return result;
    }
  }
  return result;
}

bool rootSubalgebras::IsANewSubalgebra(rootSubalgebra& input, GlobalVariables& theGlobalVariables)
{ return this->IndexSubalgebra(input, theGlobalVariables)==-1;
}

void rootSubalgebra::ComputeRootsOfK()
{ this->AllRootsK.size=0;
  this->PositiveRootsK.size=0;
  HashedList<Vector<Rational> >& AllRoots=this->GetAmbientWeyl().RootSystem;
  this->AllRootsK.ReservE(AllRoots.size);
  this->PositiveRootsK.ReservE(AllRoots.size);
  for (int i=0; i<AllRoots.size; i++)
  { Vector<Rational> tempHW, tempLW;
    this->ComputeHighestWeightInTheSameKMod(AllRoots.TheObjects[i], tempHW);
    this->ComputeLowestWeightInTheSameKMod(AllRoots.TheObjects[i], tempLW);
    if (tempHW.IsEqualToZero() || tempLW.IsEqualToZero())
    { this->AllRootsK.AddOnTop(AllRoots.TheObjects[i]);
      if (AllRoots.TheObjects[i].IsPositiveOrZero())
        this->PositiveRootsK.AddOnTop(AllRoots.TheObjects[i]);
    }
  }
}

int rootSubalgebra::ProblemCounter2=0;

bool rootSubalgebra::attemptExtensionToIsomorphismNoCentralizer
(Vectors<Rational>& Domain, Vectors<Rational>& Range, GlobalVariables& theGlobalVariables,
 int RecursionDepth, ReflectionSubgroupWeylGroup* outputAutomorphisms, bool GenerateAllpossibleExtensions,
 bool* abortKmodule, Vectors<Rational>* additionalDomain, Vectors<Rational>* additionalRange)
{ Matrix<Rational> tempMat;
  Selection tempSel;
  int CurrentRank=Domain.GetRankOfSpanOfElements(&tempMat, &tempSel);
  assert(CurrentRank==Range.GetRankOfSpanOfElements(&tempMat, &tempSel));
  if (abortKmodule!=0)
    *abortKmodule=false;
  if (CurrentRank==this->GetAmbientWeyl().CartanSymmetric.NumRows)
    return this->IsAnIsomorphism(Domain, Range, theGlobalVariables, outputAutomorphisms, additionalDomain, additionalRange);
  if (RecursionDepth>=theGlobalVariables.rootsAttemptExtensionIso1.GetElement().size)
  { int theDimension= this->GetAmbientWeyl().CartanSymmetric.NumRows;
    theGlobalVariables.rootsAttemptExtensionIso1.GetElement().SetSize(theGlobalVariables.rootsAttemptExtensionIso1.GetElement().size+theDimension);
    theGlobalVariables.rootsAttemptExtensionIso2.GetElement().SetSize(theGlobalVariables.rootsAttemptExtensionIso2.GetElement().size+theDimension);
    theGlobalVariables.rootSAAttemptExtensionIso1.GetElement().SetSize(theGlobalVariables.rootSAAttemptExtensionIso1.GetElement().size+theDimension);
    theGlobalVariables.rootSAAttemptExtensionIso2.GetElement().SetSize(theGlobalVariables.rootSAAttemptExtensionIso2.GetElement().size+theDimension);
    theGlobalVariables.rootsAttemptExtensionIso3.GetElement().SetSize(theGlobalVariables.rootsAttemptExtensionIso3.GetElement().size+theDimension);
    theGlobalVariables.rootsAttemptExtensionIso4.GetElement().SetSize(theGlobalVariables.rootsAttemptExtensionIso4.GetElement().size+theDimension);
  }
//  if (ProblemCounter==49)
//    Stop();
  Vectors<Rational>& domainRec =theGlobalVariables.rootsAttemptExtensionIso1.GetElement().TheObjects[RecursionDepth];
  Vectors<Rational>& rangeRec =theGlobalVariables.rootsAttemptExtensionIso2.GetElement().TheObjects[RecursionDepth];
  domainRec=(Domain);
  rangeRec=(Range);
  rootSubalgebra& leftSA=theGlobalVariables.rootSAAttemptExtensionIso1.GetElement()[RecursionDepth];
  rootSubalgebra& rightSA= theGlobalVariables.rootSAAttemptExtensionIso2.GetElement()[RecursionDepth];
  Rational tempRatD, tempRatR;
  Vector<Rational>& LastRootD=*domainRec.LastObject();
  Vector<Rational>& LastRootR=*rangeRec.LastObject();
  if (RecursionDepth!=0)
    for (int i=0; i<domainRec.size; i++)
    { this->GetAmbientWeyl().RootScalarCartanRoot(domainRec[i], LastRootD, tempRatD);
      this->GetAmbientWeyl().RootScalarCartanRoot(rangeRec[i], LastRootR, tempRatR);
      if (!tempRatR.IsEqualTo(tempRatD))
        return false;
    }
  leftSA.init(*this->owneR);
  rightSA.init(*this->owneR);
  leftSA.genK=domainRec;
  rightSA.genK=rangeRec;
  leftSA.ComputeAllButAmbientWeyl(); rightSA.ComputeAllButAmbientWeyl();
  if (RecursionDepth!=0)
    if (leftSA.theDynkinDiagram.ToString()!=rightSA.theDynkinDiagram.ToString() ||
        leftSA.theCentralizerDiagram.ToString()!=rightSA.theCentralizerDiagram.ToString() ||
        rightSA.kModules.size!=leftSA.kModules.size)
    { if (abortKmodule!=0)
        *abortKmodule=true;
      return false;
    }
  int counter =0;
  domainRec.AddOnTop(leftSA.HighestWeightsGmodK[counter]);
  while(domainRec.GetRankOfSpanOfElements(&tempMat, &tempSel)==CurrentRank)
  { counter++;
    assert(leftSA.kModules.size>counter);
    domainRec.RemoveIndexSwapWithLast(domainRec.size-1);
    domainRec.AddOnTop(leftSA.HighestWeightsGmodK[counter]);
  }
  //find a minimal possible new kmodule to throw in
  for (int i=0; i<leftSA.kModules.size; i++)
    if (leftSA.kModules[i].size> leftSA.kModules[counter].size)
    { domainRec.LastObject()->operator=(leftSA.HighestWeightsGmodK[i]);
      if (domainRec.GetRankOfSpanOfElements(&tempMat, &tempSel)==CurrentRank)
        domainRec.LastObject()->operator=(leftSA.HighestWeightsGmodK[counter]);
      else
        counter=i;
    }
  assert(domainRec.GetRankOfSpanOfElements(&tempMat, &tempSel)==CurrentRank+1);
  Vectors<Rational>& firstKmodLeft= leftSA.kModules[counter];
  bool result=false;
  bool tempBool;
  for (int i=0; i<rightSA.kModules.size; i++)
    if (firstKmodLeft.size==rightSA.kModules[i].size)
      for (int j=0; j<firstKmodLeft.size; j++)
      { rangeRec.AddOnTop(rightSA.kModules[i][j]);
        if (rangeRec.GetRankOfSpanOfElements(&tempMat, &tempSel)==(CurrentRank+1))
        { if (this->attemptExtensionToIsomorphismNoCentralizer(domainRec, rangeRec, theGlobalVariables, RecursionDepth+1, outputAutomorphisms, GenerateAllpossibleExtensions, &tempBool, additionalDomain, additionalRange))
          { if (!GenerateAllpossibleExtensions)
              return true;
            else
              result=true;
          }
          else
            if (tempBool)
              break;
        }
        rangeRec.RemoveIndexSwapWithLast(rangeRec.size-1);
      }
  return result;
}

bool rootSubalgebra::IsAnIsomorphism(Vectors<Rational> &domain, Vectors<Rational> &range, GlobalVariables& theGlobalVariables, ReflectionSubgroupWeylGroup* outputAutomorphisms, Vectors<Rational>* additionalDomain, Vectors<Rational>* additionalRange)
{ Matrix<Rational> matB;
  Vectors<Rational> tempRoots;
  int theDimension= this->GetAmbientWeyl().CartanSymmetric.NumRows;
  tempRoots.SetSize(theDimension);
  matB.init(theDimension, theDimension);
  this->flagAnErrorHasOccuredTimeToPanic=true;
//  if (this->ProblemCounter==49)
  //  Stop();
  for (int i=0; i<theDimension; i++)
  { for (int j=0; j<theDimension; j++)
      matB.elements[i][j].Assign(domain[i][j]);
    tempRoots[i].MakeZero(theDimension);
  }
  //if (this->flagAnErrorHasOccuredTimeToPanic)
  //{  domain.ComputeDebugString();
  //  range.ComputeDebugString();
  //  matB.ComputeDebugString();
  //}
  matB.Invert(theGlobalVariables);
  Rational tempRat2;
  for (int k=0; k<theDimension; k++)
    for (int i=0; i<theDimension; i++)
      for (int j=0; j<theDimension; j++)
      { tempRat2.Assign(range[j][k]);
        tempRat2.MultiplyBy(matB.elements[i][j]);
        tempRoots[i][k]+=(tempRat2);
      }
  //if (this->flagAnErrorHasOccuredTimeToPanic)
    //tempRoots.ComputeDebugString();
  Vector<Rational> tempRoot;
  if (additionalDomain!=0)
    for (int i=0; i<additionalDomain->size; i++)
    { additionalDomain->TheObjects[i].GetCoordsInBasiS(tempRoots, tempRoot);
      if (!(tempRoot==additionalRange->TheObjects[i]))
        return false;
    }
  for (int i=0; i<this->GetAmbientWeyl().RootsOfBorel.size; i++)
  { this->GetAmbientWeyl().RootsOfBorel[i].GetCoordsInBasiS(tempRoots, tempRoot);
    if (!this->IsARoot(tempRoot))
      return false;
  }
  if (outputAutomorphisms!=0)
    outputAutomorphisms->ExternalAutomorphisms.AddOnTop(tempRoots);
  return true;
}

void rootSubalgebra::ElementToHtml
(int index, std::string& path, SltwoSubalgebras* sl2s, GlobalVariables* theGlobalVariables)
{ std::fstream output; std::string tempS;
  std::string MyPath, childrenPath;
  MyPath=path; childrenPath=path;
  std::stringstream out;
  out << path << "rootSA" << index;
  childrenPath=out.str();
  out << ".html";
  MyPath=out.str();
  CGI::OpenFileCreateIfNotPresent(output, MyPath, false, true, false);
  this->ToString(tempS, sl2s, index,  false, true, true, theGlobalVariables);
  output << "<html><title>"
  << this->GetAmbientWeyl().theDynkinType.GetLieAlgebraName()
  << " root subalgebra of type "
  << this->theDynkinDiagram.ToString() << "</title>";
  output << "<meta name=\"keywords\" content=\""
  << this->GetAmbientWeyl().theDynkinType.GetLieAlgebraName()
  << " root subsystems, root subsystems, root systems";
  if (this->GetAmbientWeyl().theDynkinType.HasExceptionalComponent())
    output << ", exceptional Lie algebra";
  output << " \">";
//  output << CGI::GetHtmlSwitchMenuDoNotEncloseInTags();
  output << "<body>" << tempS << "</body></html>";
  output.close();
}

void rootSubalgebra::ElementToStringHeaderFooter(std::string& outputHeader, std::string& outputFooter, bool useLatex, bool useHtml, bool includeKEpsCoords)
{ outputHeader.clear();
  outputFooter.clear();
  if (useHtml)
  { outputHeader.append("\ng/k k-submodules<table border=\"1\">\n<tr><th>id</th><th>size</th>");
    outputHeader.append("<th>b\\cap k-lowest weight</th><th>b\\cap k-highest weight</th><th>Vectors<Rational></th>");
    outputHeader.append("<th>epsilon coordinates</th>");
    if (includeKEpsCoords)
      outputHeader.append("<th>epsilon coords wrt k</th>");
    outputHeader.append("</tr>");
    outputFooter.append("</td></tr></table>");
  }
  if(useLatex)
  { if (!includeKEpsCoords)
      outputHeader.append("\n\n\\noindent\\begin{tabular}{|cccccc|} \n \\multicolumn{5}{c}{");
    else
      outputHeader.append("\n\n\\noindent\\begin{tabular}{|ccccccc|} \n \\multicolumn{6}{c}{");
    outputHeader.append("$\\mathfrak{g}/\\mathfrak{k}$ $\\mathfrak{k}$-submodules} \\\\");
    outputHeader.append ("id & size & $\\mathfrak{\\mathfrak{b}\\cap\\mathfrak{k}}$-lowest weight&");
    outputHeader.append(" $\\mathfrak{\\mathfrak{b}\\cap\\mathfrak{k}}$-highest weight& elements");
    outputHeader.append(" & $\\varepsilon$-coordinates ");
    if (includeKEpsCoords)
      outputHeader.append(" & $\\varepsilon$-coordinates wrt $\\mathfrak{k}$");
    outputHeader.append("\\\\");
    outputFooter.append("\\hline \\end{tabular}");
  }
}

std::string rootSubalgebra::ToString
(FormatExpressions* theFormat, GlobalVariables* theGlobalVariables)
{ std::stringstream out;
  std::string tempS;
  std::string latexFooter, latexHeader;
  bool useLatex=false;
  bool useHtml=true;
  bool includeKEpsCoords=false;
  if (this->SimpleBasisgEpsCoords.size!=this->SimpleBasisK.size || this->SimpleBasisKEpsCoords.size!= this->SimpleBasisK.size || this->kModulesgEpsCoords.size!= this->kModules.size || this->kModulesKepsCoords.size!= this->kModules.size)
    includeKEpsCoords=false;
  int LatexLineCounter=0;
  this->ElementToStringHeaderFooter (latexHeader, latexFooter, useLatex, useHtml, includeKEpsCoords);
  tempS=this->theDynkinDiagram.ToString();
  if (useLatex)
    out << "\\noindent$\\mathfrak{k}_{ss}:$ ";
  else
    out << "k_{ss}: ";
  out << tempS;
  tempS=this->SimpleBasisK.ToString();
  if (useHtml)
    out << "\n<br>\n";
  if (useLatex)
    out << "\n\\noindent";
  out << " Simple basis: "<< tempS;
  tempS=this->SimpleBasisgEpsCoords.ElementToStringEpsilonForm(useLatex, useHtml, false);
  if (useHtml)
    out << "\n<br>\nSimple basis epsilon form: " << tempS;
  tempS=this->SimpleBasisKEpsCoords.ElementToStringEpsilonForm(useLatex, useHtml, false);
  if (useHtml)
    out << "\n<br>\nSimple basis epsilon form with respect to k: " << tempS;
  tempS=this->theCentralizerDiagram.ToString();
  if(!useLatex)
    CGI::clearDollarSigns(tempS, tempS);
  if (useLatex)
    out << "\n\n\\noindent ";
  if (useHtml)
    out << "<br>\n";
  if (useLatex)
    out << "$C(\\mathfrak{k_{ss}})_{ss}$: ";
  else
    out << "C(k_{ss})_{ss}: ";
  out << tempS;
  //int CartanPieceSize=
    //this->AmbientWeyl.CartanSymmetric.NumRows- this->SimpleBasisCentralizerRoots.size-
    //  this->SimpleBasisK.size;
  //if (CartanPieceSize!=0)
  //{  if (useLatex)
  //    out << "$\\oplus\\mathfrak{h}_" << CartanPieceSize<<"$";
  //  if (useHtml)
  //    out <<"+h_"<<CartanPieceSize;
  //}
  if (useHtml)
    out << "<br>\n simple basis centralizer: ";
  if (useLatex)
    out << "; simple basis centralizer: ";
  tempS=this->SimpleBasisCentralizerRoots.ToString();
  out << tempS;
  if (useHtml)
  { out << "<hr>\n Number of k-submodules of g/k: " << this->HighestWeightsGmodK.size;
    out << "<br>Module decomposition over k follows. The decomposition is given in 1) epsilon coordinates w.r.t. g 2) simple coordinates w.r.t. g <br> ";
    std::stringstream //tempStream1,
    tempStream2, tempStream3;
    for(int i=0; i<this->HighestWeightsGmodK.size; i++)
    { //tempStream1 << "\\underbrace{V_{";
      tempStream2 << "\\underbrace{V_{";
      tempStream3 << "\\underbrace{V_{";
      //tempStream1
      //<< this->AmbientWeyl.GetFundamentalCoordinatesFromSimple
      //(this->HighestWeightsGmodK[i]).ElementToStringLetterFormat("\\omega", true, false);
    //  tempStream2
     // << this->kModulesgEpsCoords[i][0].ToStringLetterFormat("\\epsilon");
      tempStream3
      << this->HighestWeightsGmodK[i].ToStringLetterFormat("\\alpha");
      //tempStream1 << "}}_{dim= " << this->kModules[i].size << "} ";
      tempStream2 << "}}_{dim= " << this->kModules[i].size << "} ";
      tempStream3 << "}}_{dim= " << this->kModules[i].size << "} ";
      if (i!=this->HighestWeightsGmodK.size-1)
      { //tempStream1 << "\\oplus";
        tempStream2 << "\\oplus";
        tempStream3 << "\\oplus";
      }
    }
//    out << "\n<hr>\n" << CGI::GetHtmlMathSpanFromLatexFormula(tempStream1.str()) << "\n";
    out << "\n<hr>\n" << CGI::GetHtmlMathSpanFromLatexFormula(tempStream2.str()) << "\n";
    out << "\n<hr>\n" << CGI::GetHtmlMathSpanFromLatexFormula(tempStream3.str()) << "\n<hr>\n";
  }
  if (useLatex)
    out << "\n\n\\noindent Number $\\mathfrak{g}/\\mathfrak{k}$ $\\mathfrak{k}$-submodules: ";
  if (!useHtml)
    out << this->LowestWeightsGmodK.size ;
  if (useHtml)
    out << "<br>\n";
  if (useLatex)
    out << "\n\n";
  out << latexHeader;
  this->kModulesgEpsCoords.SetSize(this->kModules.size);
  for (int i=0; i<this->kModules.size; i++)
  { tempS=this->LowestWeightsGmodK[i].ToString();
    if (useHtml)
      out << "\n<tr><td>";
    if (useLatex)
      out << "\\hline ";
    out << i;
    if (useHtml)
      out << "</td><td>";
    if (useLatex)
      out << " & ";
    out << this->kModules[i].size;
    if (useHtml)
      out << "</td><td>";
    if (useLatex)
      out << " & ";
    out << tempS;
    tempS=this->HighestWeightsGmodK[i].ToString();
    if (useHtml)
      out << "</td><td>";
    if (useLatex)
      out << " & ";
    out  << tempS;
    if (useHtml)
      out << "</td><td>";
    if (useLatex)
      out << " & \n";
    out << this->kModules[i].ToString();
    if (useHtml)
      out << "</td><td>";
    if (i>=this->kModulesgEpsCoords.size)
      this->GetAmbientWeyl().GetEpsilonCoords(this->kModules[i], this->kModulesgEpsCoords[i]);
    out << this->kModulesgEpsCoords[i].ElementToStringEpsilonForm(useLatex, useHtml, true);
    if (useLatex)
      out << " & \n";
    if (useHtml)
      out << "</td>";
    if (includeKEpsCoords)
    { if (useHtml)
        out << "<td>";
      if (useLatex)
        out << " & ";
      out << tempS << this->kModulesKepsCoords[i].ElementToStringEpsilonForm(useLatex, useHtml, true);
      if (useHtml)
        out << "</td>";
      if (useLatex)
        out << "\\\\\n";
    }
    if (useHtml)
      out << "</tr>";
    if (LatexLineCounter>this->NumGmodKtableRowsAllowedLatex)
    { LatexLineCounter=0;
      out << latexFooter << latexHeader;
    }
    if (i!=this->kModules.size-1)
    { LatexLineCounter+=this->kModules.TheObjects[i].size;
      if (useLatex)
       if ((LatexLineCounter>this->NumGmodKtableRowsAllowedLatex) && (LatexLineCounter!=this->kModules.TheObjects[i].size))
        { out << latexFooter << latexHeader;
          LatexLineCounter = this->kModules.TheObjects[i].size;
        }
    }
  }
  if (useHtml)
    out << "</table>";
  if (useLatex)
    out << latexFooter;
  if ((useLatex|| useHtml)&& this->theMultTable.size==0 && this->kModules.size!=0)
    this->GenerateKmodMultTable(this->theMultTable, this->theOppositeKmods, theGlobalVariables);
  if (this->theMultTable.size!=0)
  { if (useHtml)
      out << "\n\n Pairing table:\n\n";
    if (useLatex)
      out << "\n\n\\noindent Pairing table:\n\n\\noindent";
    this->theMultTable.ToString(tempS, useLatex, useHtml, *this);
    out << tempS << "\n";
  }
  return out.str();
}

bool rootSubalgebra::IsGeneratingSingularVectors(int indexKmod, Vectors<Rational>& NilradicalRoots)
{ Vector<Rational>& currentRoot=this->HighestWeightsGmodK.TheObjects[indexKmod];
  Vector<Rational> tempRoot;
  for (int i=0; i<NilradicalRoots.size; i++)
  { tempRoot=(currentRoot);
    tempRoot+=(NilradicalRoots.TheObjects[i]);
    if (this->IsARootOrZero(tempRoot))
      if (!NilradicalRoots.Contains(tempRoot))
        return false;
  }
  return true;
}

void rootSubalgebra::MakeGeneratingSingularVectors(coneRelation& theRelation, Vectors<Rational>& nilradicalRoots)
{ bool isMaximal=false;
  Vector<Rational> beta, tempRoot;
  //theRelation.ComputeDebugString(*this);
  while (!isMaximal)
  { isMaximal=true;
    for (int i=0; i<theRelation.AlphaCoeffs.size; i++)
    { for (int j=0; j<nilradicalRoots.size; j++)
      { tempRoot=(theRelation.Alphas.TheObjects[i]);
        //theRelation.Alphas.TheObjects[i].ComputeDebugString();
        //nilradicalRoots.TheObjects[j].ComputeDebugString();
        tempRoot+=(nilradicalRoots.TheObjects[j]);
        //tempRoot.ComputeDebugString();
        //theRelation.ComputeDebugString(*this);
        if ((this->IsARoot(tempRoot) || tempRoot.IsEqualToZero()) &&(!nilradicalRoots.Contains(tempRoot)))
        { this->ComputeHighestWeightInTheSameKMod(tempRoot, tempRoot);
          //tempRoot.ComputeDebugString();
          tempRoot-=(theRelation.Alphas.TheObjects[i]);
          //tempRoot.ComputeDebugString();
          theRelation.Alphas.TheObjects[i]+=(tempRoot);
          int tempI=theRelation.Betas.GetIndex(tempRoot);
          if (tempI==-1)
          { theRelation.Betas.AddOnTop(tempRoot);
            theRelation.BetaCoeffs.AddOnTop(theRelation.AlphaCoeffs.TheObjects[i]);
          }
          else
            theRelation.BetaCoeffs.TheObjects[tempI]+=(theRelation.AlphaCoeffs.TheObjects[i]);
          isMaximal=false;
          break;
        }
        if (theRelation.Alphas.TheObjects[i].IsEqualToZero())
        { theRelation.Alphas.RemoveIndexSwapWithLast(i);
          theRelation.AlphaCoeffs.RemoveIndexSwapWithLast(i);
          i--;
          isMaximal=false;
          break;
        }
      }
    }
  }
}

void DynkinDiagramRootSubalgebra::SwapDynkinStrings(int i, int j)
{ this->SimpleComponentTypes.SwapTwoIndices(i, j);
  this->SimpleBasesConnectedComponents.SwapTwoIndices(i, j);
  this->indicesThreeNodes.SwapTwoIndices(i, j);
  this->indicesEnds.SwapTwoIndices(i, j);
}

void DynkinDiagramRootSubalgebra::Sort()
{ //doing bubble sort
  for (int i=0; i<this->SimpleBasesConnectedComponents.size; i++)
    for (int j=i+1; j<this->SimpleBasesConnectedComponents.size; j++)
    { bool tempBool=false;
      if (this->SimpleBasesConnectedComponents[i].size<this->SimpleBasesConnectedComponents[j].size)
        tempBool=true;
      if (this->SimpleBasesConnectedComponents[i].size==this->SimpleBasesConnectedComponents[j].size)
        tempBool=((this->SimpleComponentTypes[i])<(this->SimpleComponentTypes[j]));
      if (tempBool)
        this->SwapDynkinStrings(i, j);
    }
  this->sameTypeComponents.size=0;
  this->indexInUniComponent.SetSize(this->SimpleBasesConnectedComponents.size);
  this->indexUniComponent.SetSize(this->SimpleBasesConnectedComponents.size);
  this->sameTypeComponents.ReservE(this->SimpleBasesConnectedComponents.size);
  DynkinSimpleType tempType;
  for (int i=0; i<this->SimpleBasesConnectedComponents.size; i++)
  { if (!(this->SimpleComponentTypes[i]==tempType))
    { this->sameTypeComponents.SetSize(this->sameTypeComponents.size+1);
      this->sameTypeComponents.LastObject()->size=0;
      tempType=this->SimpleComponentTypes[i];
    }
    this->sameTypeComponents.LastObject()->AddOnTop(i);
    this->indexUniComponent[i]=this->sameTypeComponents.size-1;
    this->indexInUniComponent[i]=this->sameTypeComponents.LastObject()->size-1;
  }
}

void DynkinDiagramRootSubalgebra::ComputeDiagramTypeKeepInput
(const Vectors<Rational>& simpleBasisInput, WeylGroup& theWeyl)
{ this->SimpleBasesConnectedComponents.size=0;
  this->SimpleBasesConnectedComponents.ReservE(simpleBasisInput.size);
//  std::string tempS= simpleBasisInput.ToString();
  //if (tempS=="Num vectors: 2\n(1, 0, 0), (0, 1, 1)")
  //  tempS="!!";
  for (int i=0; i<simpleBasisInput.size; i++)
  { int indexFirstComponentConnectedToRoot=-1;
    for (int j=0; j<this->SimpleBasesConnectedComponents.size; j++)
      if (this->SimpleBasesConnectedComponents[j].ContainsARootNonPerpendicularTo
          (simpleBasisInput[i], theWeyl.CartanSymmetric))
      { if (indexFirstComponentConnectedToRoot==-1)
        { indexFirstComponentConnectedToRoot=j;
          this->SimpleBasesConnectedComponents[j].AddOnTop(simpleBasisInput[i]);
        }
        else
        { this->SimpleBasesConnectedComponents[indexFirstComponentConnectedToRoot].AddListOnTop(this->SimpleBasesConnectedComponents.TheObjects[j]);
          this->SimpleBasesConnectedComponents.RemoveIndexSwapWithLast(j);
          j--;
        }
      }
    if (indexFirstComponentConnectedToRoot==-1)
    { this->SimpleBasesConnectedComponents.SetSize(this->SimpleBasesConnectedComponents.size+1);
      this->SimpleBasesConnectedComponents.LastObject()->size=0;
      this->SimpleBasesConnectedComponents.LastObject()->AddOnTop(simpleBasisInput[i]);
    }
  }
  this->ComputeDynkinStrings(theWeyl);
  this->Sort();
  this->ComputeDynkinStrings(theWeyl);
}

bool DynkinDiagramRootSubalgebra::LetterIsDynkinGreaterThanLetter(char letter1, char letter2)
{ if( (letter1=='B' || letter1=='D') && (letter2=='B' || letter2=='D') )
  { if (letter1==letter2)
      return false;
    if (letter1=='B')
      return true;
    else
      return false;
  }
  return letter1>letter2;
}

bool DynkinDiagramRootSubalgebra::IsGreaterThan(DynkinDiagramRootSubalgebra& right)
{ if (this->RankTotal()>right.RankTotal())
    return true;
  if (this->RankTotal()<right.RankTotal())
    return false;
  assert(this->SimpleComponentTypes.size==this->SimpleBasesConnectedComponents.size);
  for (int i=0; i<this->SimpleComponentTypes.size; i++)
  { if (this->SimpleBasesConnectedComponents[i].size>right.SimpleBasesConnectedComponents[i].size)
      return true;
     if (right.SimpleBasesConnectedComponents[i].size>this->SimpleBasesConnectedComponents[i].size)
      return false;
    if (this->SimpleComponentTypes[i]>right.SimpleComponentTypes[i])
      return true;
    if (right.SimpleComponentTypes[i]>this->SimpleComponentTypes[i])
      return false;
  }
  return this->ToString()>right.ToString();
}

void DynkinDiagramRootSubalgebra::GetMapFromPermutation(Vectors<Rational>& domain, Vectors<Rational>& range, List<int>& thePerm, List<List<List<int > > >& theAutos, SelectionWithDifferentMaxMultiplicities& theAutosPerm, DynkinDiagramRootSubalgebra& right)
{ for (int i=0; i<this->SimpleBasesConnectedComponents.size; i++)
    for (int j=0; j<this->SimpleBasesConnectedComponents.TheObjects[i].size; j++)
    { assert(this->SimpleBasesConnectedComponents.TheObjects[i].size==right.SimpleBasesConnectedComponents.TheObjects[thePerm.TheObjects[i]].size);
      domain.AddOnTop( this->SimpleBasesConnectedComponents.TheObjects[i].TheObjects[j]);
      int indexTargetComponent=thePerm.TheObjects[i];
      int indexAutomorphismInComponent=theAutosPerm.Multiplicities.TheObjects[i];
      int indexRoot=theAutos.TheObjects[i].TheObjects[indexAutomorphismInComponent].TheObjects[j];
      range.AddOnTop(right.SimpleBasesConnectedComponents.TheObjects[indexTargetComponent].TheObjects[indexRoot]);
    }
}

void DynkinDiagramRootSubalgebra::ComputeDynkinStrings
  (WeylGroup& theWeyl)
{ this->indicesThreeNodes.SetSize(this->SimpleBasesConnectedComponents.size);
  this->SimpleComponentTypes.SetSize(this->SimpleBasesConnectedComponents.size);
  this->indicesEnds.SetSize(this->SimpleBasesConnectedComponents.size);
  for (int i=0; i<this->SimpleBasesConnectedComponents.size; i++)
    this->ComputeDynkinString(i, theWeyl);
}

bool DynkinDiagramRootSubalgebra::operator==(const DynkinDiagramRootSubalgebra& right) const
{ if (right.SimpleBasesConnectedComponents.size!= this->SimpleBasesConnectedComponents.size)
    return false;
  for (int i=0; i<this->SimpleBasesConnectedComponents.size; i++)
  { bool tempBool=((this->SimpleBasesConnectedComponents[i].size ==
                    right.SimpleBasesConnectedComponents[i].size) &&
                   (this->SimpleComponentTypes[i]==right.SimpleComponentTypes[i]));
    if (!tempBool)
      return false;
  }
  return true;
}

void DynkinDiagramRootSubalgebra::Assign(const DynkinDiagramRootSubalgebra& right)
{ this->SimpleComponentTypes=(right.SimpleComponentTypes);
  this->indicesThreeNodes=(right.indicesThreeNodes);
  this->SimpleBasesConnectedComponents=(right.SimpleBasesConnectedComponents);
  this->indexInUniComponent=(right.indexInUniComponent);
  this->indexUniComponent=(right.indexUniComponent);
  this->indicesEnds=(right.indicesEnds);
  this->indicesThreeNodes=(right.indicesThreeNodes);
  this->sameTypeComponents=(right.sameTypeComponents);
}

void DynkinDiagramRootSubalgebra::GetAutomorphism(List<List<int> >& output, int index)
{ Vectors<Rational>& currentComponent= this->SimpleBasesConnectedComponents[index];
  DynkinSimpleType& currentStrinG=this->SimpleComponentTypes[index];
  List<int> thePermutation;
  thePermutation.SetSize(currentComponent.size);
  output.size=0;
  for (int i=0; i<currentComponent.size; i++)
    thePermutation[i]=i;
  output.AddOnTop(thePermutation);
  if (currentStrinG.theLetter=='A' && currentComponent.size!=1)
  { thePermutation.ReverseOrderElements();
    output.AddOnTop(thePermutation);
  }
  if (currentStrinG.theLetter=='D')
  { if (currentComponent.size==4)
    {//the automorphism group of the Dynkin Diagram is S3
      thePermutation[1]=2; thePermutation[2]=3; thePermutation[3]=1;
      output.AddOnTop(thePermutation);
      thePermutation[1]=1; thePermutation[2]=3; thePermutation[3]=2;
      output.AddOnTop(thePermutation);
      thePermutation[1]=2; thePermutation[2]=1; thePermutation[3]=3;
      output.AddOnTop(thePermutation);
      thePermutation[1]=3; thePermutation[2]=1; thePermutation[3]=2;
      output.AddOnTop(thePermutation);
      thePermutation[1]=3; thePermutation[2]=2; thePermutation[3]=1;
      output.AddOnTop(thePermutation);
    } else
    { thePermutation[currentComponent.size-2]=currentComponent.size-1;
      thePermutation[currentComponent.size-1]=currentComponent.size-2;
      output.AddOnTop(thePermutation);
    }
  }
  if (currentStrinG.theLetter=='E' && currentStrinG.theRank==6)
  { thePermutation[1]=3; thePermutation[2]=4;
    thePermutation[3]=1; thePermutation[4]=2;
    output.AddOnTop(thePermutation);
  }
}

void DynkinDiagramRootSubalgebra::GetAutomorphisms(List<List<List<int> > >& output)
{ output.SetSize(this->SimpleBasesConnectedComponents.size);
  for (int i=0; i<this->SimpleBasesConnectedComponents.size; i++)
    this->GetAutomorphism(output[i], i);
}

int DynkinDiagramRootSubalgebra::RankTotal()
{ int result=0;
  for (int i=0; i<this->SimpleBasesConnectedComponents.size; i++)
    result+=this->SimpleBasesConnectedComponents[i].size;
  return result;
}

int DynkinDiagramRootSubalgebra::NumRootsGeneratedByDiagram()
{ int result=0;
  assert(this->SimpleBasesConnectedComponents.size==this->SimpleComponentTypes.size);
  for (int i=0; i<this->SimpleComponentTypes.size; i++)
  { int Rank=this->SimpleBasesConnectedComponents[i].size;
    if (this->SimpleComponentTypes[i].theLetter=='A')
      result+=Rank*(Rank+1);
    if (this->SimpleComponentTypes[i].theLetter=='B'|| this->SimpleComponentTypes[i].theLetter=='C')
      result+=Rank*Rank*2;
    if (this->SimpleComponentTypes[i].theLetter=='D')
      result+=Rank*(Rank-1)*2;
    if (this->SimpleComponentTypes[i].theLetter=='E')
    { if (Rank==6)
        result+=72;
      if (Rank==7)
        result+=126;
      if (Rank==8)
        result+=240;
    }
    if (this->SimpleComponentTypes[i].theLetter=='F')
      result+=48;
    if (this->SimpleComponentTypes[i].theLetter=='G')
      result+=12;
  }
  return result;
}

int DynkinDiagramRootSubalgebra::numberOfThreeValencyNodes(int indexComponent, WeylGroup& theWeyl)
{ Vectors<Rational>& currentComponent = this->SimpleBasesConnectedComponents.TheObjects[indexComponent];
  int numEnds=0; int result=0;
  this->indicesThreeNodes.TheObjects[indexComponent] =-1;
  this->indicesEnds.TheObjects[indexComponent].size=0;
  for (int i=0; i<currentComponent.size; i++)
  { int counter=0;
    for (int j=0; j<currentComponent.size; j++)
    { Rational tempRat;
      theWeyl.RootScalarCartanRoot(currentComponent.TheObjects[i], currentComponent.TheObjects[j], tempRat);
      if (tempRat.IsNegative())
        counter++;
    }
    assert(counter<=3);
    if (counter==3)
    { result++;
      this->indicesThreeNodes.TheObjects[indexComponent]=i;
    }
    if (counter<=1)
    { numEnds++;
      this->indicesEnds.TheObjects[indexComponent].AddOnTop(i);
    }
  }
  assert(result<=1);
  if (result==1)
    assert(numEnds==3);
  else
    assert(numEnds<=2);
  return result;
}

void rootSubalgebra::GetLinearCombinationFromMaxRankRootsAndExtraRoot(bool DoEnumeration, GlobalVariables& theGlobalVariables)
{ int theDimension = this->GetAmbientWeyl().CartanSymmetric.NumRows;
  std::stringstream out2;
  std::stringstream out;
  //this->ComputeDebugString(theGlobalVariables);
  out2 << this->ToString() << "\n";
  Matrix<Rational>  tempMat;
  this->SimpleBasisK.GetMatrixRootsToRows(tempMat);
  tempMat.Invert(theGlobalVariables);
  int counter=0;
  HashedList<Vector<Rational> >& AllRoots= this->GetAmbientWeyl().RootSystem;
  for(int i=0; i<AllRoots.size; i++)
  { Vector<Rational> linComb;
    if (this->AllRootsK.GetIndex(AllRoots.TheObjects[i])==-1)
    { for (int j=0; j<theDimension; j++)
      { linComb.TheObjects[j].MakeZero();
        for(int k=0; k<theDimension; k++)
        { Rational tempRat;
          tempRat.Assign(tempMat.elements[k][j]);
          tempRat.MultiplyBy(AllRoots.TheObjects[i].TheObjects[k]);
          linComb.TheObjects[j]+=(tempRat);
        }
      }
      int x= linComb.FindLCMDenominatorsTruncateToInt();
      linComb*=(-x);
      std::string tempS;
      if (this->LinCombToString(AllRoots.TheObjects[i], x, linComb, tempS))
      { out << tempS << "\n";
        counter++;
        if (this->LowestWeightsGmodK.GetIndex(AllRoots.TheObjects[i]) !=-1)
          out2 << tempS << "\n";
      }
    }
  }
  out << "\\multicolumn{2}{|c|}{Number of relations: " << counter << " }\\\\\\hline";
  std::string tempS=out.str();
  out2 << "\n\n" << tempS;
  if (DoEnumeration)
  { this->TestedRootsAlpha=(this->LowestWeightsGmodK);
    this->DoKRootsEnumeration(theGlobalVariables);
  }
//  this->GetLinearCombinationFromMaxRankRootsAndExtraRootMethod2();
}

void rootSubalgebra::GetLinearCombinationFromMaxRankRootsAndExtraRootMethod2(GlobalVariables& theGlobalVariables)
{ int theDimension = this->GetAmbientWeyl().CartanSymmetric.NumRows;
  std::stringstream out;
  out << this->ToString() << "\n\n";
  Vector<Rational> tempRoot;
  tempRoot=(this->SimpleBasisK[0]);
  this->ComputeHighestWeightInTheSameKMod(tempRoot, tempRoot);
  HashedList<Vector<Rational> >& AllRoots= this->GetAmbientWeyl().RootSystem;
  for (int l=0; l<this->SimpleBasisK.size; l++)
  { Rational tempRat;
    this->GetAmbientWeyl().RootScalarCartanRoot(tempRoot, this->SimpleBasisK[l], tempRat);
    if (!tempRat.IsEqualToZero())
    { int counter=0;
      Vectors<Rational> tempRoots;
      tempRoots=(this->SimpleBasisK);
      tempRoots[l]=(tempRoot);
      Matrix<Rational> tempMat;
      tempRoots.GetMatrixRootsToRows(tempMat);
      tempMat.Invert(theGlobalVariables);
      for(int i=0; i<AllRoots.size; i++)
      { Vector<Rational> linComb;
        if (this->AllRootsK.GetIndex(AllRoots.TheObjects[i])==-1)
        { for (int j=0; j<theDimension; j++)
          { linComb.TheObjects[j].MakeZero();
            for(int k=0; k<theDimension; k++)
            { Rational tempRat;
              tempRat.Assign(tempMat.elements[k][j]);
              tempRat.MultiplyBy(AllRoots.TheObjects[i].TheObjects[k]);
              linComb.TheObjects[j]+=(tempRat);
            }
          }
          int x= linComb.FindLCMDenominatorsTruncateToInt();
          linComb*=(-x);
          std::string tempS;
          if (this->LinCombToStringDistinguishedIndex(l, AllRoots.TheObjects[i], x, linComb, tempS))
          { out << tempS << "\n";
            counter++;
          }
        }
      }
      out << "\\multicolumn{2}{|c|}{Number of relations: " << counter << " }\\\\\\hline";
    }
  }
}

bool rootSubalgebra::LinCombToString(Vector<Rational>& alphaRoot, int coeff, Vector<Rational>& linComb, std::string& output)
{ int theDimension = this->GetAmbientWeyl().CartanSymmetric.NumRows;
  if (coeff==1)
    return false;
  std::stringstream out;
  std::string tempS=alphaRoot.ToString();
  out << "(" << tempS << ")&$";
  out << coeff << "\\alpha_" << theDimension+1;
  for (int i=0; i<theDimension; i++)
  {  //if (linComb.coordinates[i].IsEqualToZero())
    //  return false;
    tempS=linComb[i].ToString();
    if (tempS!="0")
    { if (tempS=="-1")
        tempS="-";
      if (tempS=="1")
        tempS="+";
      if (!(tempS[0]=='+')&& !(tempS[0]=='-'))
        tempS.insert(0, "+");
      out << tempS << "\\beta_" << i+1;
    }
  }
  out << "=0$\\\\";
  output=out.str();
  return true;
}

bool rootSubalgebra::LinCombToStringDistinguishedIndex(int distinguished, Vector<Rational>& alphaRoot, int coeff, Vector<Rational>& linComb, std::string& output)
{ int theDimension = this->GetAmbientWeyl().CartanSymmetric.NumRows;
  if (coeff==1)
    return false;
  std::stringstream out;
  std::string tempS=alphaRoot.ToString();
  out << "(" << tempS << ")&$";
  out << coeff << "\\alpha_" << theDimension+1;
  for (int i=0; i<theDimension; i++)
  { //if (linComb.coordinates[i].IsEqualToZero())
    //  return false;
    tempS=linComb.TheObjects[i].ToString();
    if (tempS!="0")
    { if (tempS=="-1")
        tempS="-";
      if (tempS=="1")
        tempS="+";
      if (!(tempS[0]=='+') && !(tempS[0]=='-'))
        tempS.insert(0, "+");
      out << tempS;
      if (i!=distinguished)
        out << "\\beta_" << i+1;
      else
        out << "\\gamma";
    }
  }
  out << "=0$\\\\";
  output=out.str();
  return true;
}

void rootSubalgebra::DoKRootsEnumeration(GlobalVariables& theGlobalVariables)
{ this->theKEnumerations.SetSize(this->PosRootsKConnectedComponents.size);
  this->theKComponentRanks.SetSize(this->PosRootsKConnectedComponents.size);
  Matrix<Rational> tempMat;
  Selection tempSel;
  for (int i=0; i<this->PosRootsKConnectedComponents.size; i++)
  { this->theKEnumerations[i].init(this->PosRootsKConnectedComponents[i].size);
    this->theKComponentRanks[i]=
    this->PosRootsKConnectedComponents[i].GetRankOfSpanOfElements(&tempMat, &tempSel);
  }
  this->DoKRootsEnumerationRecursively(0, theGlobalVariables);
}

void rootSubalgebra::DoKRootsEnumerationRecursively(int indexEnumeration, GlobalVariables& theGlobalVariables)
{ int theRank=this->theKComponentRanks[indexEnumeration];
  int numRuns=MathRoutines::NChooseK
  (this->PosRootsKConnectedComponents[indexEnumeration].size, theRank);
  this->theKEnumerations[indexEnumeration].initNoMemoryAllocation();
  for (int i=0; i<numRuns; i++)
  { this->theKEnumerations[indexEnumeration].incrementSelectionFixedCardinality(theRank);
    if (indexEnumeration<this->PosRootsKConnectedComponents.size-1)
      this->DoKRootsEnumerationRecursively(indexEnumeration+1, theGlobalVariables);
    else
      this->KEnumerationsToLinComb(theGlobalVariables);
  }
}

void rootSubalgebra::KEnumerationsToLinComb(GlobalVariables& theGlobalVariables)
{ int theDimension = this->GetAmbientWeyl().CartanSymmetric.NumRows;
  Matrix<Rational> tempMat;
  Selection tempSelection;
  tempMat.init((int)theDimension, (int)theDimension);
  int counter=0;
  for (int i=0; i<this->PosRootsKConnectedComponents.size; i++)
  { this->PosRootsKConnectedComponents.TheObjects[i].SelectionToMatrixAppend(this->theKEnumerations.TheObjects[i], theDimension, tempMat, counter);
    counter+=this->theKComponentRanks.TheObjects[i];
  }
  //tempMat.ComputeDebugString();
  if (tempMat.Invert(theGlobalVariables))
  { //tempMat.ComputeDebugString();
    for (int l=0; l<this->TestedRootsAlpha.size; l++)
    { Vector<Rational> linComb;
      Vector<Rational>& TestedRootAlpha= this->TestedRootsAlpha.TheObjects[l];
      for (int j=0; j<theDimension; j++)
      { linComb.TheObjects[j].MakeZero();
        for(int k=0; k<theDimension; k++)
        { Rational tempRat;
          tempRat.Assign(tempMat.elements[k][j]);
          tempRat.MultiplyBy(TestedRootAlpha.TheObjects[k]);
          linComb.TheObjects[j]+=(tempRat);
        }
      }
      int x= linComb.FindLCMDenominatorsTruncateToInt();
      linComb*=(-x);
      bool foundBadCombination=true;
      for (int i=0; i<theDimension; i++)
        if (linComb.TheObjects[i].NumShort==-1 || linComb.TheObjects[i].NumShort== 1)
        { foundBadCombination=false;
          break;
        }
      if (foundBadCombination)
      { std::string tempS;
        this->LinCombToString(TestedRootAlpha, x, linComb, tempS);
      }
    }
  }
}

void IndicatorWindowVariables::Assign(IndicatorWindowVariables& right)
{ this->ProgressReportStringS=right.ProgressReportStringS;
  this->flagRootIsModified= right.flagRootIsModified;
  this->modifiedRoot=right.modifiedRoot;
}

bool affineCone::SplitByAffineHyperplane(affineHyperplane& theKillerPlane, affineCones& output)
{ return true;
}

bool affineCone::WallIsInternalInCone(affineHyperplane& theKillerCandidate)
{ return true;
}

inline int affineCone::GetDimension()
{ if (this->theWalls.size==0)
    return 0;
  return this->theWalls.TheObjects[0].affinePoint.size;
}

inline unsigned int affineCone::HashFunction() const
{ int tempMin=MathRoutines::Minimum(this->theWalls.size, ::SomeRandomPrimesSize);
  unsigned int result=0;
  for (int i=0; i<tempMin; i++)
    result+= this->theWalls[i].HashFunction()*::SomeRandomPrimes[i];
  return result;
}

bool affineHyperplane::operator ==(const affineHyperplane& right)
{ Vector<Rational> tempRoot1, tempRoot2;
  tempRoot1=(this->normal);
  tempRoot2=(right.normal);
  tempRoot1.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
  tempRoot2.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
  if (!(tempRoot1==tempRoot2))
    return false;
  Rational tempRat1, tempRat2;
  tempRat1=Vector<Rational>::ScalarEuclidean(tempRoot1, this->affinePoint);
  tempRat2=Vector<Rational>::ScalarEuclidean(tempRoot1, right.affinePoint);
  return tempRat1.IsEqualTo(tempRat2);
}

bool affineHyperplane::ProjectFromFacetNormal(Vector<Rational>& input)
{ Rational tempRat;
  int tempI=input.GetIndexFirstNonZeroCoordinate();
  assert(tempI!=-1);
  if (tempI==input.size-1)
    return false;
  this->affinePoint.MakeZero(input.size);
  this->affinePoint.SetSize(input.size-1);
  this->affinePoint.TheObjects[tempI].Assign(input.TheObjects[input.size-1]);
  this->affinePoint.TheObjects[tempI].Minus();
  this->affinePoint.TheObjects[tempI].DivideBy(input.TheObjects[tempI]);
  this->normal=(input);
  this->normal.SetSize(input.size-1);
  return true;
}

bool affineHyperplane::ContainsPoint(Vector<Rational>& thePoint)
{ Rational tempRat1, tempRat2;
  tempRat1=Vector<Rational>::ScalarEuclidean(this->normal, thePoint);
  tempRat2=Vector<Rational>::ScalarEuclidean(this->normal, this->affinePoint);
  return tempRat2.IsEqualTo(tempRat1);
}

bool affineHyperplane::HasACommonPointWithPositiveTwoToTheNth_ant()
{ Rational tempRat;
  tempRat= Vector<Rational>::ScalarEuclidean(this->normal, this->affinePoint);
  if (tempRat.IsEqualToZero())
    return true;
  for(int i=0; i<this->normal.size; i++)
  { Rational& tempRat2= this->normal.TheObjects[i];
    if (tempRat.IsNegative() && tempRat2.IsNegative())
      return true;
    if (tempRat.IsPositive() && tempRat2.IsPositive())
      return true;
  }
  return false;
}

void affineHyperplane::MakeFromNormalAndPoint(Vector<Rational>& inputPoint, Vector<Rational>& inputNormal)
{ this->affinePoint=(inputPoint);
  this->normal=(inputNormal);
}

void affineHyperplane::ToString(std::string& output)
{ std::stringstream out;
  std::string tempS=this->affinePoint.ToString();
  out << "point: " << tempS;
  tempS=this->normal.ToString();
  out << " normal: " << tempS;
  output= out.str();
}

unsigned int affineHyperplane::HashFunction() const
{ //warning: if normal gets streched, the hashfunction should not change!
  Vector<Rational> tempNormal;
  tempNormal=(this->normal);
  tempNormal.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
  Rational tempRat=Vector<Rational>::ScalarEuclidean(this->normal, this->affinePoint);
  return this->normal.HashFunction()+ tempRat.HashFunction();
}

void affineHyperplanes::ToString(std::string& output)
{ std::stringstream out;
  for (int i=0; i<this->size; i++)
  { std::string tempS;
    this->TheObjects[i].ToString(tempS);
    out << "index: " << i << " " << tempS << "\n";
  }
  output= out.str();
}

void multTableKmods::ToString(std::string& output, rootSubalgebra& owner)
{ this->ToString(output, false, false, owner);
}

void multTableKmods::ToString(std::string& output, bool useLaTeX, bool useHtml, rootSubalgebra& owner)
{  std::stringstream out;
  out << "\t";
  if (!(useLaTeX||useHtml))
    for (int i=0; i<this->size; i++)
      out << i << "\t";
  else
  { if (useHtml)
      out << "<table><tr><th></th>";
    if (useLaTeX)
    { out << "\\begin{tabular}{c|";
      for (int i=0; i<this->size; i++)
        out << "c";
      out << "|} & ";
    }
    for (int i=0; i<this->size; i++)
    { if (useHtml)
        out << "<th>";
      out << i;
      if (useHtml)
        out << "</th>";
      else
      { if (i!=this->size-1)
          out << "&";
      }
    }
    if (useHtml)
      out << "</tr>";
    if (useLaTeX)
      out << "\\\\\\hline";
  }
  for (int i=0; i<this->size; i++)
  { if (useHtml)
      out << "\n<tr><td>";
    out << "\n" << i;
    if (useLaTeX)
      out << " & ";
    if (useHtml)
      out << "</td>";
    for (int j=0; j<this->TheObjects[i].size; j++)
    { if ((j==owner.CentralizerRoots.size-1) && (i<=owner.CentralizerRoots.size-1))
      { if(useLaTeX)
          out << "\\multicolumn{1}{c|}{";
        if(useHtml)
          out << "<td rhs=\"1\">";
      } else
      { if (useHtml)
          out << "<td>";
      }
      for (int k=0; k<this->TheObjects[i].TheObjects[j].size; k++)
        out << this->TheObjects[i].TheObjects[j].TheObjects[k] << ", ";
      if (useLaTeX )
      { if ((j==owner.CentralizerRoots.size-1) && (i<=owner.CentralizerRoots.size-1))
          out << "}";
        if (j!=this->TheObjects[i].size-1)
          out << " & ";
      }
      if (useHtml)
        out << "</td>";
    }
    if (useHtml)
      out << "</tr>";
    if (useLaTeX)
    { out << "\\\\";
      if (i==owner.CentralizerRoots.size-1)
        out << "\\cline{2-" << owner.CentralizerRoots.size+1 << "}";
    }
  }
  if (useLaTeX)
    out << "\n\\hline opposite & ";
  if(useHtml)
    out << "<tr><td>opposite</td>";
  for (int i=0; i<owner.theOppositeKmods.size; i++)
  { if (useHtml)
      out << "<td>";
    out << i << "/" << owner.theOppositeKmods.TheObjects[i];
    if (useHtml)
      out << "</td>";
    if (useLaTeX)
    { if (i!=owner.theOppositeKmods.size-1)
      { if (useLaTeX)
          out << " & ";
        else
          out << "\t";
      }
    }
  }
  if (useLaTeX)
    out << "\n\\end{tabular}";
  output=out.str();
}

rootSubalgebra::rootSubalgebra()
{ this->owneR=0;
//  this->indexInOwners=-1;
  this->flagAnErrorHasOccuredTimeToPanic=false;
  this->NumGmodKtableRowsAllowedLatex=35;
  this->flagMakingProgressReport=true;
  this->flagComputeConeCondition=true;
  this->initForNilradicalGeneration();
}

void rootSubalgebra::initForNilradicalGeneration()
{ this->NumNilradicalsAllowed=0;
  this->NumConeConditionFailures=0;
  this->NumRelationsWithStronglyPerpendicularDecomposition=0;
  this->flagFirstRoundCounting=true;
}

void coneRelation::RelationOneSideToStringCoordForm(std::string& output, List<Rational>& coeffs, Vectors<Rational>& theRoots, bool EpsilonForm)
{ std::stringstream out;
  std::string tempS;
  for (int i=0; i<theRoots.size; i++)
  { tempS=coeffs[i].ToString();
    if (tempS=="1")
      tempS="";
    if (tempS=="-1")
      tempS="-";
    assert(!(tempS=="0"));
    out<< tempS;
    if (!EpsilonForm)
      tempS=theRoots[i].ToString();
    else
      tempS=theRoots[i].ToStringEpsilonFormat();
    out << "(" << tempS << ")";
    if (i!=theRoots.size-1)
      out << " + ";
  }
   output=out.str();
}

void coneRelation::RelationOneSideToString(std::string& output, const std::string& letterType, List<Rational>& coeffs, List<List<int> >& kComponents, Vectors<Rational>& theRoots, bool useLatex, rootSubalgebra& owner)
{ assert(theRoots.size==coeffs.size);
  std::stringstream out;
  std::string tempS;
  if (useLatex)
  { out << "\\begin{tabular}{";
    for (int i=0; i< theRoots.size; i++)
      out << "c";
    out << "}";
  }
  for (int i=0; i<theRoots.size; i++)
  { tempS=coeffs[i].ToString();
    if (tempS=="1")
      tempS="";
    if (tempS=="-1")
      tempS="-";
    assert(!(tempS=="0"));
    out << tempS;
    if (!useLatex)
    { tempS=theRoots[i].ToString();
      out << "(" << tempS << ")";
      if (i!=theRoots.size-1)
        out << " + ";
    } else
    { out << "$" << letterType << "_" << i+1 << "$";
      if (i!=theRoots.size-1)
        out << "+ & ";
    }
  }
  if (useLatex)
    out << "\\\\";
  List<int> TakenIndices;
  List<int> NumPrimesUniTypicComponent;
  TakenIndices.initFillInObject(owner.theDynkinDiagram.SimpleBasesConnectedComponents.size, -1);
  NumPrimesUniTypicComponent.initFillInObject(owner.theDynkinDiagram.sameTypeComponents.size, -1);
  for (int i=0; i<kComponents.size; i++)
  { if (useLatex)
      out << "\\tiny{ ";
    for (int j=0; j<kComponents[i].size; j++)
    { int index= kComponents[i][j];
      int indexUniComponent=owner.theDynkinDiagram.indexUniComponent[index];
      out << owner.theDynkinDiagram.SimpleComponentTypes[index].ToString();
      if (TakenIndices[index]==-1)
      { NumPrimesUniTypicComponent[indexUniComponent]++;
        TakenIndices[index]=NumPrimesUniTypicComponent[indexUniComponent];
      }
      for (int k=0; k<TakenIndices[index]; k++)
        out << "'";
      if (j!=kComponents[i].size-1)
        out << "+";
    }
    if (useLatex)
    { out << " }";
      if(i!=kComponents.size-1)
        out << " & ";
    }
  }
  if (useLatex)
    out << "\\end{tabular}";
  output=out.str();
}

int coneRelation::ToString(std::string& output, rootSubalgebras& owners, bool useLatex, bool includeScalarsProductsEachSide, bool includeMixedScalarProducts)
{ std::string tempS;
  std::stringstream out;
  assert(this->AlphaCoeffs.size==this->Alphas.size);
  assert(this->BetaCoeffs.size==this->Betas.size);
  int LatexLineCounter=0;
  this->ComputeConnectedComponents(this->Alphas, owners.TheObjects[this->IndexOwnerRootSubalgebra],  this->AlphaKComponents);
  this->ComputeConnectedComponents(this->Betas, owners.TheObjects[this->IndexOwnerRootSubalgebra], this->BetaKComponents);
  this->RelationOneSideToString(tempS, "\\alpha", this->AlphaCoeffs, this->AlphaKComponents, this->Alphas, useLatex, owners.TheObjects[this->IndexOwnerRootSubalgebra]);
  out << tempS;
  if (useLatex)
    out << " &\\begin{tabular}{c} ";
  out << "=";
  if (useLatex)
    out << " \\\\~ \\end{tabular} & ";
  this->RelationOneSideToString(tempS, "\\beta", this->BetaCoeffs, this->BetaKComponents, this->Betas, useLatex, owners.TheObjects[this->IndexOwnerRootSubalgebra]);
  out << tempS;
  if (useLatex)
    out << " & ";
  this->theDiagram.ToString();
  out << tempS;
  this->theDiagramRelAndK.ToString();
  if (useLatex)
    out << " & ";
  out << tempS;
  if (includeScalarsProductsEachSide)
  { out << " & ";
    LatexLineCounter+=this->RootsToScalarProductString(this->Alphas, this->Alphas, "\\alpha", "\\alpha", tempS, useLatex, owners.TheObjects[this->IndexOwnerRootSubalgebra]);
    out << tempS;
    LatexLineCounter+=this->RootsToScalarProductString(this->Betas, this->Betas, "\\beta", "\\beta", tempS, useLatex, owners.TheObjects[this->IndexOwnerRootSubalgebra]);
    out << " " << tempS;
  }
  if (includeMixedScalarProducts)
  { LatexLineCounter+= this->RootsToScalarProductString(this->Alphas, this->Betas, "\\alpha", "\\beta", tempS, useLatex, owners.TheObjects[this->IndexOwnerRootSubalgebra]);
    out << tempS;
  }
  out << "\n";
  output=out.str();
  return MathRoutines::Maximum(2, LatexLineCounter);
}

int coneRelation::RootsToScalarProductString(Vectors<Rational>& inputLeft, Vectors<Rational>& inputRight, const std::string& letterTypeLeft, const std::string& letterTypeRight, std::string& output, bool useLatex, rootSubalgebra& owner)
{ std::string tempS; std::stringstream out;
  int numLinesLatex=0;
  Rational tempRat;
  for (int i=0; i<inputLeft.size; i++)
    for(int j=0; j<inputRight.size; j++)
      if (i<j || letterTypeLeft!=letterTypeRight)
      { owner.GetAmbientWeyl().RootScalarCartanRoot(inputLeft[i], inputRight[j], tempRat);
        if (!tempRat.IsEqualToZero())
        { tempS=tempRat.ToString();
          out << "$\\langle" << letterTypeLeft << "_" << i+1 << ", "
          << letterTypeRight << "_" << j+1 << "\\rangle=" << tempS << "$, ";
          numLinesLatex++;
        }
      }
  output= out.str();
  return numLinesLatex;
}

void coneRelation::ComputeConnectedComponents(Vectors<Rational>& input, rootSubalgebra& owner, List<List<int> >& output)
{ output.SetSize(input.size);
  for (int i=0; i<input.size; i++)
  { output[i].size=0;
    for (int j=0; j<owner.theDynkinDiagram.SimpleBasesConnectedComponents.size; j++)
      if (owner.theDynkinDiagram.SimpleBasesConnectedComponents[j].
          ContainsARootNonPerpendicularTo(input[i], owner.GetAmbientWeyl().CartanSymmetric))
        output[i].AddOnTop(j);
  }
}

bool coneRelation::IsStrictlyWeaklyProhibiting(rootSubalgebra& owner, Vectors<Rational>& NilradicalRoots, GlobalVariables& theGlobalVariables, rootSubalgebras& owners, int indexInOwner)
{ Vectors<Rational> tempRoots;
  tempRoots=(this->Alphas);
  tempRoots.AddListOnTop(this->Betas);
  tempRoots.AddListOnTop(owner.genK);
  //owner.AmbientWeyl.TransformToSimpleBasisGenerators(tempRoots);
  this->theDiagram.ComputeDiagramTypeModifyInput(tempRoots, owner.GetAmbientWeyl());
  if (this->theDiagram.ToString()=="F^{1}_4")
    return false;
  if (this->theDiagram.SimpleComponentTypes[0].theLetter=='A' &&
      this->theDiagram.SimpleComponentTypes[0].theRank==1)
  {//  assert(false);
  }
  ReflectionSubgroupWeylGroup tempSubgroup;
  tempSubgroup.AmbientWeyl=(owner.GetAmbientWeyl());
  tempSubgroup.ComputeSubGroupFromGeneratingReflections
  (&tempRoots, &tempSubgroup.ExternalAutomorphisms, &theGlobalVariables, 0, true);
  Vectors<Rational> NilradicalIntersection, genSingHW;
  tempRoots=(tempSubgroup.RootSubsystem);
  NilradicalRoots.IntersectWith(tempRoots, NilradicalIntersection);
  for (int i=0; i<owner.HighestWeightsGmodK.size; i++)
    if (!owner.NilradicalKmods.selected[i] && tempRoots.Contains(owner.HighestWeightsGmodK[i]) && owner.IsGeneratingSingularVectors(i, NilradicalIntersection))
      genSingHW.AddOnTop(owner.HighestWeightsGmodK[i]);
  if (owner.ConeConditionHolds(theGlobalVariables, owners, indexInOwner, NilradicalIntersection, genSingHW, false))
    return false;
  if (!owner.AttemptTheTripleTrickWRTSubalgebra(*this, genSingHW, NilradicalIntersection, theGlobalVariables))
    return false;
  this->SortRelation(owner);
  this->FixRepeatingRoots(this->Alphas, this->AlphaCoeffs);
  //this->FixRightHandSide(owner, NilradicalRoots);
  this->ComputeDiagramRelAndK(owner);
  return true;
}

void coneRelation::ComputeTheDiagramAndDiagramRelAndK(rootSubalgebra& owner)
{ Vectors<Rational> tempRoots;
  tempRoots=(this->Alphas);
  tempRoots.AddListOnTop(this->Betas);
  this->theDiagram.ComputeDiagramTypeModifyInput(tempRoots, owner.GetAmbientWeyl());
  this->ComputeDiagramRelAndK(owner);
}

void coneRelation::MakeLookCivilized(rootSubalgebra& owner, Vectors<Rational>& NilradicalRoots)
{ if (this->Alphas.size==0 || this->Betas.size==0)
    return;
  Vectors<Rational> tempRoots;
//  this->Alphas.ComputeDebugString();
//  this->Betas.ComputeDebugString();
  tempRoots=(this->Alphas);
  tempRoots.AddListOnTop(this->Betas);
  //owner.AmbientWeyl.TransformToSimpleBasisGenerators(tempRoots);
  this->theDiagram.ComputeDiagramTypeModifyInput(tempRoots, owner.GetAmbientWeyl());
  if (this->theDiagram.SimpleComponentTypes[0].theLetter=='A' &&
      this->theDiagram.SimpleComponentTypes[0].theRank==1)
  { this->ComputeDiagramRelAndK(owner);
    assert(false);
  }
  this->SortRelation(owner);
  this->FixRepeatingRoots(this->Alphas, this->AlphaCoeffs);
  //this->FixRightHandSide(owner, NilradicalRoots);
  this->ComputeDiagramRelAndK(owner);
}

void coneRelation::FixRightHandSide(rootSubalgebra& owner, Vectors<Rational>& NilradicalRoots)
{ bool hasChanged=true;
  Vector<Rational> tempRoot;
  while(hasChanged)
  { hasChanged=false;
    for (int i=0; i<this->Betas.size; i++)
      for (int j=i+1; j<this->Betas.size; j++)
      { tempRoot=(this->Betas[i]);
        tempRoot+=(this->Betas[j]);
        if (owner.GetAmbientWeyl().RootSystem.Contains(tempRoot))
        { int leavingIndex=j; int remainingIndex=i;
          if (this->BetaCoeffs[j].IsGreaterThan(this->BetaCoeffs[i]))
          { leavingIndex=i;
            remainingIndex=j;
          }
          this->Betas[leavingIndex]=(tempRoot);
          this->BetaCoeffs[remainingIndex].Subtract(this->BetaCoeffs[leavingIndex]);
          if (this->BetaCoeffs[remainingIndex].IsEqualToZero())
          { this->BetaCoeffs.RemoveIndexSwapWithLast(remainingIndex);
            this->Betas.RemoveIndexSwapWithLast(remainingIndex);
          }
          assert(NilradicalRoots.Contains(tempRoot));
          hasChanged=true;
        }
      }
  }
}

bool coneRelation::CheckForBugs(rootSubalgebra& owner, Vectors<Rational>& NilradicalRoots)
{ for (int i=0; i<this->Alphas.size; i++)
  { int tempI= owner.HighestWeightsGmodK.GetIndex(this->Alphas.TheObjects[i]);
    if (tempI==-1)
      return false;
    if (NilradicalRoots.Contains(this->Alphas.TheObjects[i]))
      return false;
  }
  for (int i=0; i<this->Betas.size; i++)
    if (!NilradicalRoots.Contains(this->Betas.TheObjects[i]))
      return false;
  return true;
}

void coneRelation::GetSumAlphas(Vector<Rational>& output, int theDimension)
{ assert(this->AlphaCoeffs.size==this->Alphas.size);
  output.MakeZero(theDimension);
  Vector<Rational> tempRoot;
  for(int i=0; i<this->Alphas.size; i++)
  { tempRoot=(this->Alphas[i]);
    tempRoot*=(this->AlphaCoeffs.TheObjects[i]);
    output+=(tempRoot);
  }
}

void coneRelation::SortRelation(rootSubalgebra& owner)
{ this->ComputeKComponents(this->Alphas, this->AlphaKComponents, owner);
  this->ComputeKComponents(this->Betas, this->BetaKComponents, owner);
  //bubble sort
  for (int i=0; i<this->Alphas.size; i++)
    for (int j=i+1; j<this->Alphas.size; j++)
      if (this->leftSortedBiggerThanOrEqualToRight(this->AlphaKComponents.TheObjects[j], this->AlphaKComponents.TheObjects[i]))
      { this->AlphaCoeffs.SwapTwoIndices(i, j);
        this->Alphas.SwapTwoIndices(i, j);
        this->AlphaKComponents.SwapTwoIndices(i, j);
      }
  for (int i=0; i<this->Betas.size; i++)
    for (int j=i+1; j<this->Betas.size; j++)
      if (this->leftSortedBiggerThanOrEqualToRight(this->BetaKComponents.TheObjects[j], this->BetaKComponents.TheObjects[i]))
      { this->BetaCoeffs.SwapTwoIndices(i, j);
        this->Betas.SwapTwoIndices(i, j);
        this->BetaKComponents.SwapTwoIndices(i, j);
      }
}

void coneRelation::ComputeKComponents(Vectors<Rational>& input, List<List<int> >& output, rootSubalgebra& owner)
{ output.SetSize(input.size);
  for(int i=0; i<input.size; i++)
  { output[i].size=0;
    for(int j=0; j<owner.theDynkinDiagram.SimpleBasesConnectedComponents.size; j++)
      if (owner.theDynkinDiagram.SimpleBasesConnectedComponents[j].ContainsARootNonPerpendicularTo
          (input[i], owner.GetAmbientWeyl().CartanSymmetric))
        output[i].AddOnTop(j);
  }
}

void coneRelation::ComputeDiagramRelAndK(rootSubalgebra& owner)
{ Vectors<Rational> tempRoots;
  tempRoots.size=0;
  tempRoots.ReservE(owner.GetAmbientWeyl().CartanSymmetric.NumRows*2);
  tempRoots.AddListOnTop(owner.SimpleBasisK);
  for (int i=0; i<this->theDiagram.SimpleBasesConnectedComponents.size; i++)
    tempRoots.AddListOnTop(this->theDiagram.SimpleBasesConnectedComponents[i]);
  //owner.AmbientWeyl.TransformToSimpleBasisGenerators(tempRoots);
  this->theDiagramRelAndK.ComputeDiagramTypeModifyInput(tempRoots, owner.GetAmbientWeyl());
}

void coneRelation::FixRepeatingRoots(Vectors<Rational>& theRoots, List<Rational>& coeffs)
{ for (int i=0; i<theRoots.size; i++)
  { if (coeffs[i].IsEqualToZero())
    { coeffs.RemoveIndexSwapWithLast(i);
      theRoots.RemoveIndexSwapWithLast(i);
      i--;
    }
    for (int j=i+1; j<theRoots.size; j++)
      if (theRoots[i]==(theRoots[j]))
      { coeffs[i]+=(coeffs[j]);
        theRoots.RemoveIndexSwapWithLast(j);
        coeffs.RemoveIndexSwapWithLast(j);
        j--;
      }
  }
}

bool coneRelation::leftSortedBiggerThanOrEqualToRight(List<int>& left, List<int>& right)
{ if (left.size>right.size)
    return true;
  if (right.size>left.size)
    return false;
  for(int i=0; i<right.size; i++)
  { if (right.TheObjects[i]>left.TheObjects[i])
      return false;
    if (left.TheObjects[i]>right.TheObjects[i])
      return true;
  }
  return true;
}

bool coneRelation::GenerateAutomorphisms(coneRelation& right, rootSubalgebras& owners)
{ /*if (this->DebugString.length()!=right.DebugString.length())
    return false;
  for (unsigned int i=0; i<this->DebugString.length(); i++)
  {  if (this->DebugString[i]!=right.DebugString[i])
      return false;
  }*/
  if (this->DebugString!=right.DebugString)
    return false;
  return true;
}

void coneRelations::AddRelationNoRepetition(coneRelation& input, rootSubalgebras& owners, int indexInRootSubalgebras)
{ input.ComputeDebugString(owners, true, true);
  int i=this->GetHash(input);
  List<int>& theIndices= this->TheHashedArrays[i];
  for (int j=0; j<theIndices.size; j++)
  { if(this->TheObjects[theIndices.TheObjects[j]].GenerateAutomorphisms(input, owners))
      return;
  }
  if (!this->flagIncludeSmallerRelations)
    if (input.theDiagramRelAndK.ToString()!=owners[0].theDynkinDiagram.ToString())
      return;
  this->AddOnTop(input);
  if (this->flagIncludeCoordinateRepresentation)
  {
  }
}

void permutation::initPermutation(int n)
{ this->initIncomplete(n);
  for(int i=0; i<n; i++)
  { this->MaxMultiplicities.TheObjects[i]=n-i-1;
    this->Multiplicities.TheObjects[i]=0;
  }
}

void permutation::initPermutation(List<int>& disjointSubsets, int TotalNumElements)
{ this->initIncomplete(TotalNumElements);
  int counter=0;
  for(int i=0; i<disjointSubsets.size; i++)
  { for (int j=0; j<disjointSubsets.TheObjects[i]; j++)
    { this->MaxMultiplicities.TheObjects[counter]=disjointSubsets.TheObjects[i]-j-1;
      this->Multiplicities.TheObjects[counter]=0;
      counter++;
    }
    TotalNumElements-=disjointSubsets.TheObjects[i];
  }
  assert(TotalNumElements==0);
}

void permutation::incrementAndGetPermutation(List<int>& output)
{ this->IncrementSubset();
  this->GetPermutationLthElementIsTheImageofLthIndex(output);
}

void permutation::GetPermutationLthElementIsTheImageofLthIndex(List<int>& output)
{ int numElements=this->Multiplicities.size;
  output.SetSize(numElements);
  for (int i=0; i<numElements; i++)
    output[i]=i;
  for (int i=0; i<numElements; i++)
    MathRoutines::swap(output[i], output[i+this->Multiplicities[i]]);
}

void rootSubalgebras::ComputeKmodMultTables(GlobalVariables* theGlobalVariables)
{ for (int i=0; i<this->size; i++)
    (*this)[i].GenerateKmodMultTable
    ((*this)[i].theMultTable, (*this)[i].theOppositeKmods, theGlobalVariables);
}

void rootSubalgebras::ComputeAllRootSubalgebrasUpToIso(GlobalVariables& theGlobalVariables, int StartingIndex, int NumToBeProcessed)
{ this->NumSubalgebrasProcessed=0;
  this->NumConeConditionFailures=0;
  this->NumSubalgebrasCounted=0;
  for (int i=StartingIndex; i<NumToBeProcessed+StartingIndex; i++)
  { this->TheObjects[i].flagComputeConeCondition=this->flagComputeConeCondition;
    this->TheObjects[i].GeneratePossibleNilradicals(this->controllerLProhibitingRelations, this->ImpiedSelectionsNilradical, this->parabolicsCounterNilradicalGeneration, theGlobalVariables, false, *this, i);
    if (i!=NumToBeProcessed+StartingIndex-1)
      this->TheObjects[i+1].GeneratePossibleNilradicalsInit(this->ImpiedSelectionsNilradical, this->parabolicsCounterNilradicalGeneration);
  }
}

void rootSubalgebras::ComputeLProhibitingRelations(GlobalVariables& theGlobalVariables)
{ if (this->flagStoringNilradicals)
  { this->storedNilradicals.SetSize(this->size);
  }
  for (; this->IndexCurrentSANilradicalsGeneration<this->NumReductiveRootSAsToBeProcessedNilradicalsGeneration; this->IndexCurrentSANilradicalsGeneration++)
  { this->TheObjects[this->IndexCurrentSANilradicalsGeneration].flagComputeConeCondition=this->flagComputeConeCondition;
    this->TheObjects[this->IndexCurrentSANilradicalsGeneration].GeneratePossibleNilradicals(this->controllerLProhibitingRelations, this->ImpiedSelectionsNilradical, this->parabolicsCounterNilradicalGeneration, theGlobalVariables, this->flagUsingParabolicsInCentralizers, *this, this->IndexCurrentSANilradicalsGeneration);
    if (this->IndexCurrentSANilradicalsGeneration!= this->NumReductiveRootSAsToBeProcessedNilradicalsGeneration-1)
      this->TheObjects[this->IndexCurrentSANilradicalsGeneration+1].GeneratePossibleNilradicalsInit(this->ImpiedSelectionsNilradical, this->parabolicsCounterNilradicalGeneration);
  }
}

void rootSubalgebras::SortDescendingOrderBySSRank()
{//Bubble sort
  rootSubalgebras output;
  List<int> SortingArray;
  List<int> inverseOfSortingArray;
  SortingArray.SetSize(this->size);
  for (int i=0; i<this->size; i++)
    SortingArray.TheObjects[i]=i;
  for (int i=0; i<this->size; i++)
    for (int j=i+1; j<this->size; j++)
      if(this->TheObjects[SortingArray.TheObjects[j]].theDynkinDiagram.IsGreaterThan(this->TheObjects[SortingArray.TheObjects[i]].theDynkinDiagram))
        SortingArray.SwapTwoIndices(i, j);
  inverseOfSortingArray.SetSize(SortingArray.size);
  for(int i=0; i<SortingArray.size; i++)
    inverseOfSortingArray.TheObjects[SortingArray.TheObjects[i]]=i;
  output.ReservE(this->size);
  for (int i=0; i<this->size; i++)
  { output.AddOnTop(this->TheObjects[SortingArray.TheObjects[i]]);
    List<int>& otherArray=this->TheObjects[SortingArray.TheObjects[i]].indicesSubalgebrasContainingK;
    List<int>& currentArray=output.LastObject()->indicesSubalgebrasContainingK;
    currentArray.ReservE(otherArray.size);
    currentArray.size=0;
    for(int j=0; j<otherArray.size; j++)
      currentArray.AddOnTop(inverseOfSortingArray[otherArray[j]]);
  }
  for(int i=0; i<this->size; i++)
    this->TheObjects[i].Assign(output.TheObjects[i]);
}

void rootSubalgebras::GetTableHeaderAndFooter(std::string& outputHeader, std::string& outputFooter, bool useLatex, bool useHtml)
{ std::stringstream out1, out2; std::string tempS;
  if (useHtml)
  { out1 << "<table border=\"1\">\n <colgroup>";
    for (int i=0; i<this->NumColsPerTableLatex; i++)
      out1<< "<col width=\"80\">";
    out1<< "</colgroup><tr>";
    out2 << "</td></tr></table>";
  }
  if (useLatex)
  { out1 << "\n\n \\begin{tabular}{";
    for (int i=0; i<this->NumColsPerTableLatex; i++)
      out1 << "c";
    out1 << "}\n";
    out2 << "\\end{tabular}";
  }
  outputFooter=out2.str();
  outputHeader=out1.str();
}

void rootSubalgebras::ElementToHtml
(std::string& header, std::string& pathPhysical, std::string& htmlPathServer,
 SltwoSubalgebras* Sl2s, const std::string& calculatorDisplayName, GlobalVariables* theGlobalVariables)
{ std::fstream output; std::string tempS;
  std::string MyPathPhysical, childrenPathPhysical;
  std::string MyPathServer, childrenPathServer;
  MyPathPhysical=pathPhysical; childrenPathPhysical=pathPhysical;
  MyPathServer=htmlPathServer; childrenPathServer= htmlPathServer;
  MyPathPhysical.append("rootHtml.html");
  MyPathServer.append("rootHtml.html");
  childrenPathPhysical.append("rootHtml_");
  childrenPathServer.append("rootHtml_");
  CGI::OpenFileCreateIfNotPresent(output, MyPathPhysical, false, true, false);
  output << "<html><title> Root subsystems of "
  << this->TheObjects[0].theDynkinDiagram.ToString()
  << "</title>";
  output << "<meta name=\"keywords\" content=\""
  << this->TheObjects[0].theDynkinDiagram.ToString()
  << " root subsystems, root subsystems, root systems";
  if (this->GetOwnerWeyl().theDynkinType.HasExceptionalComponent())
    output << ", exceptional Lie algebra";
  output << " \">";
  this->ToString
  (tempS, Sl2s, false, true, false, &childrenPathPhysical, &childrenPathServer,
   theGlobalVariables, calculatorDisplayName);
  output << "<body>" << header << tempS
   << "</body></html>";
  output.close();
  for (int i=0; i<this->size; i++)
    this->TheObjects[i].ElementToHtml(i, childrenPathPhysical, Sl2s, theGlobalVariables);
}

void rootSubalgebras::ToString
(std::string& output, SltwoSubalgebras* sl2s, bool useLatex, bool useHtml, bool includeKEpsCoords,
 std::string* htmlPathPhysical, std::string* htmlPathServer,
 GlobalVariables* theGlobalVariables, const std::string& DisplayNameCalculator)
{ std::stringstream out; std::string tempS;
  if (useHtml)
    out << " <a href=\"" << DisplayNameCalculator
    << "\">Calculator main page</a><br><a href=\"/tmp/manual_vector_partition.pdf\">"
    << "Notation and conventions (incomplete). Will evolve to a manual of the program.</a><br>";
  this->ElementToStringDynkinTable(useLatex, useHtml, htmlPathPhysical, htmlPathServer, tempS);
  out << tempS;
  //this->AmbientWeyl.ComputeRho(true);
  //this->ElementToStringCentralizerIsomorphisms(tempS, useLatex, useHtml, theGlobalVariables);
  //out << tempS;
/*  for (int i=0; i<this->size; i++)
  {  this->TheObjects[i].ToString(tempS, sl2s, i, useLatex, useHtml, includeKEpsCoords, theGlobalVariables);
    out << tempS <<"\n\n";
  }*/
  output= out.str();
}

void rootSubalgebras::pathToHtmlFileNameElements(int index, std::string* htmlPathServer, std::string& output, bool includeDotHtml)
{ if (htmlPathServer==0)
  { output.clear();
    return;
  }
  std::stringstream out;
  out << *htmlPathServer << "rootSA" << index;
  if (includeDotHtml)
    out << ".html";
  output=out.str();
}

void rootSubalgebras::pathToHtmlReference(int index, std::string& DisplayString, std::string* htmlPathServer, std::string& output)
{ if(htmlPathServer==0)
  { output.clear();
    return;
  }
  std::stringstream out; std::string tempS;
  this->pathToHtmlFileNameElements(index, htmlPathServer, tempS, true);
  out << "<a href=\"" << tempS << "\">" << DisplayString << "</a>";
  output=out.str();
}

void rootSubalgebras::ElementToStringDynkinTable(bool useLatex, bool useHtml, std::string* htmlPathPhysical, std::string* htmlPathServer, std::string& output)
{ std::stringstream out; std::string header, footer, tempS, tempS2, tempS3;
  std::string tooltipSAs="h - fixed Cartan subalgebra. k - subalgebra containing h. k_{ss}=[k, k] - regular semisimple subalgebra in the sense of Dynkin, Semisimple Lie subalgebras of semisimple Lie algebras. k_{ss} is parametrized by a Vector<Rational> subsytem of \\Delta(g). C(k_{ss}) consists Vector<Rational> spaces with Vectors<Rational> strongly orthogonal to \\Delta(k) and a part of the Cartan h";
  this->GetTableHeaderAndFooter(header, footer, useLatex, useHtml);
  int col=0; int row=0;
  tempS=this->TheObjects[0].theDynkinDiagram.ToString();
  if (useLatex)
    out << "$\\mathfrak{g}$: ";
  else
    out << "g: ";
  out << tempS << "  (Number of table entries: " << this->size-2 << " larger than the Cartan subalgebra + the Cartan subalgebra)\n\n";
  for (int i=1; i<this->size; i++)
  { if (col==0 && row==0)
    { out << header;
      if (useHtml)
        out << "<td title=\"" << tooltipSAs << "\">";
    }
    tempS=this->TheObjects[i].theDynkinDiagram.ToString();
    tempS2=this->TheObjects[i].theCentralizerDiagram.ToString();
    if (tempS=="") tempS="-";
    if (useLatex)
    { CGI::subEqualitiesWithSimeq(tempS, tempS);
      CGI::subEqualitiesWithSimeq(tempS2, tempS2);
      out << "\\begin{tabular}{p{2cm}}\n $\\mathfrak{k}_{ss}$: " << tempS;
      out << "\\\\\n";
    }
    else
    { CGI::clearDollarSigns(tempS, tempS);
      CGI::clearDollarSigns(tempS2, tempS2);
      out << "k_{ss}: ";
      if (!useHtml)
        out << tempS;
      else
      { this->pathToHtmlReference(i, tempS, htmlPathServer, tempS3);
        out << tempS3;
      }
    }
    if (useLatex)
      out << "$C(\\mathfrak{k_{ss}})_{ss}$: ";
    else
    { if (useHtml)
        out << "\n<br>\n";
      out << "C(k_{ss})_{ss}: ";
    }
    if (tempS2=="") tempS2="-";
    out << tempS2;
    if (useLatex)
      out << "\\\\\n";
    if (useHtml)
      out << "\n<br>\n";
    if (useLatex)
      out << "\n$\\mathfrak{k}_{ss}$ lies in: ";
    else
      out << "\n k_ss lies in: ";
    if (useLatex)
      out <<  "\\\\\n";
    if (useHtml)
      out << "\n<br>\n";
    int counter=0;
    for(int j=0; j<this->TheObjects[i].indicesSubalgebrasContainingK.size; j++)
    { int tempI=this->TheObjects[i].indicesSubalgebrasContainingK[j];
      tempS=this->TheObjects[tempI].theDynkinDiagram.ToString();
      if (useLatex)
        CGI::subEqualitiesWithSimeq(tempS, tempS);
      counter+=(signed)tempS.length();
      if (!useHtml)
        out << tempS << ", ";
      else
      { if (!useLatex)
          CGI::clearDollarSigns(tempS, tempS);
        this->pathToHtmlReference(tempI, tempS, htmlPathServer, tempS3);
        out << tempS3 << " , ";
      }
      if (useLatex && counter>20)
      { counter=0;
        out << "\\\\\n";
      }
    }
    if (useLatex)
      out << "\\end{tabular}";
    row = (i)/this->NumColsPerTableLatex;
    col = (i)% this->NumColsPerTableLatex;
    if (row == this->NumLinesPerTableLatex)
      row=0;
    if (col==0 && row!=0)
    { if (useLatex)
        out << "\\\\\n\\hline";
      if (useHtml)
      { out << "</td></tr>";
        if(i!=this->size-1)
          out << "<tr><td title=\"" << tooltipSAs << "\">";
      }
    }
    if (col!=0)
    { if (useLatex)
        out << " & ";
      if(useHtml)
        out << "</td>";
      if (i!=this->size-1 && useHtml)
        out << "<td title=\"" << tooltipSAs << "\">";
    }
    if (row==0 && col==0)
      out << footer;
  }
  if (!(col==0 && row==0))
    out << footer;
  out << "\n\n";
  output=out.str();
}

void coneRelations::GetLatexHeaderAndFooter(std::string& outputHeader, std::string& outputFooter)
{ outputHeader.clear();
  outputFooter.clear();
  outputHeader.append("\\begin{tabular}{rcl p{1cm}p{1cm}p{3cm} } \\multicolumn{3}{c}");
  outputHeader.append("{ Relation / linked $\\mathfrak{k}$-components}");
  outputHeader.append(" &$\\alpha_i$'s, $\\beta_i$'s generate & adding $\\mathfrak{k}$ generates&");
  outputHeader.append("Non-zero scalar products\\\\");
  outputFooter.append("\\end{tabular}");
}

void coneRelations::ToString
(std::string& output, rootSubalgebras& owners, bool useLatex, bool useHtml, std::string* htmlPathPhysical,
 std::string* htmlPathServer, GlobalVariables& theGlobalVariables, const std::string& DisplayNameCalculator)
{ std::stringstream out;
  std::string tempS, header, footer;
  Vectors<Rational> tempAlphas, tempBetas;
  this->GetLatexHeaderAndFooter(header, footer);
  if (useLatex)
    out << header;
  int oldIndex=-1;
  int lineCounter=0;
  for(int i=0; i<this->size; i++)
  { if (oldIndex!=this->TheObjects[i].IndexOwnerRootSubalgebra)
    { oldIndex=this->TheObjects[i].IndexOwnerRootSubalgebra;
      if (useLatex)
        out << "\\hline\\multicolumn{5}{c}{$\\mathfrak{k}$-semisimple type: "
        << owners[oldIndex].theDynkinDiagram.ToString() << "}\\\\\n\\hline\\hline";
      //if (useHtml)
      //{ out << "<table>" << "<tr>"<< owners.TheObjects[oldIndex].theDynkinDiagram.DebugString
       //     <<"</tr>";
      //}
    }
    lineCounter+=this->TheObjects[i].ToString(tempS, owners, useLatex, true, true);
    out << tempS;
    if (useLatex)
      out << "\\\\";
    out << "\n";
    if (this->flagIncludeCoordinateRepresentation)
    { lineCounter+=2;
      out << "\\multicolumn{5}{c}{$\\varepsilon$-form~relative~to~the~subalgebra~generated~by~$\\mathfrak{k}$~and~the~relation}\\\\\n";
      //(*this)[i].GetEpsilonCoords(this->TheObjects[i].Alphas, tempAlphas, owners.GetOwnerWeyl(), theGlobalVariables);
      //this->TheObjects[i].GetEpsilonCoords(this->TheObjects[i].Betas, tempBetas, owners.GetOwnerWeyl(), theGlobalVariables);
      this->TheObjects[i].RelationOneSideToStringCoordForm(tempS, this->TheObjects[i].AlphaCoeffs, tempAlphas, true);
      out << "\\multicolumn{5}{c}{" << tempS;
      this->TheObjects[i].RelationOneSideToStringCoordForm(tempS, this->TheObjects[i].BetaCoeffs, tempBetas, true);
      out << "=" << tempS; //<<"~~~~";
    //  this->TheObjects[i].RelationOneSideToStringCoordForm
    //    (tempS, this->TheObjects[i].AlphaCoeffs, this->TheObjects[i].Alphas, false);
    //  out <<tempS;
    //  this->TheObjects[i].RelationOneSideToStringCoordForm
    //    (tempS, this->TheObjects[i].BetaCoeffs, this->TheObjects[i].Betas, false);
    //  out <<"="<<tempS<<"}\\\\\n";
      out << "}\\\\\\hline\n";
    }
    if (lineCounter>this->NumAllowedLatexLines)
    { out << footer << "\n\n\n" << header;
      lineCounter=0;
    }
  }
  if (useLatex)
    out << footer;
  if (this->flagIncludeSubalgebraDataInDebugString)
  { owners.ToString
    (tempS, 0, useLatex, useHtml, false, htmlPathPhysical, htmlPathServer,
     &theGlobalVariables, DisplayNameCalculator);
    out << "\n\n\\newpage" << tempS;
  }
  output=out.str();
}

void SemisimpleLieAlgebra::ComputeChevalleyConstantS
(GlobalVariables* theGlobalVariables)
{ MacroRegisterFunctionWithName("SemisimpleLieAlgebra::ComputeChevalleyConstantS");
  this->theWeyl.ComputeRho(true);
  this->ChevalleyConstants.init(this->theWeyl.RootSystem.size, this->theWeyl.RootSystem.size);
  this->Computed.init(this->theWeyl.RootSystem.size, this->theWeyl.RootSystem.size);
  this->Computed.NullifyAll(false);
  Selection nonExploredRoots;
  this->flagAnErrorHasOccurredTimeToPanic=false;
  Vectors<Rational>& posRoots=this->theWeyl.RootsOfBorel;
  nonExploredRoots.MakeFullSelection(posRoots.size);
  Vector<Rational> tempRoot;
  std::stringstream out;
  ProgressReport theReport(theGlobalVariables);
  double startTimer=-1;
  if (theGlobalVariables!=0)
  { out << "Initializing matrix for structure constant computation of "
    << this->GetLieAlgebraName() << "... ";
    startTimer= theGlobalVariables->GetElapsedSeconds();
    theReport.Report(out.str());
  }
  for (int i=0; i<this->theWeyl.RootSystem.size; i++)
    for(int j=i; j<this->theWeyl.RootSystem.size; j++)
    { tempRoot=this->theWeyl.RootSystem.TheObjects[i]+this->theWeyl.RootSystem.TheObjects[j];
      if(!tempRoot.IsEqualToZero())
        if(!this->theWeyl.IsARoot(tempRoot))
        { this->Computed.elements[i][j]=true;
          this->ChevalleyConstants.elements[i][j].MakeZero();
          this->Computed.elements[j][i]=true;
          this->ChevalleyConstants.elements[j][i].MakeZero();
        }
    }
  double startStructureConstantComputation=-1;
  if (theGlobalVariables!=0)
  { out << "done in " << theGlobalVariables->GetElapsedSeconds()- startTimer
    << " seconds.<br> " << "Computing structure constants...";
    theReport.Report(out.str());
    startStructureConstantComputation= theGlobalVariables->GetElapsedSeconds();
  }
  Rational tempRat;
  while (nonExploredRoots.CardinalitySelection>0)
  { //this->ComputeDebugString();
    //nonExploredRoots.ComputeDebugString();
    int theBorelIndex=nonExploredRoots.elements[0];
    Rational theHeight= posRoots.TheObjects[theBorelIndex].SumCoords();
    for (int i=1; i<nonExploredRoots.CardinalitySelection; i++)
    { tempRat= posRoots.TheObjects[nonExploredRoots.elements[i]].SumCoords();
      if (theHeight.IsGreaterThan(tempRat))
      { theHeight.Assign(tempRat);
        theBorelIndex=nonExploredRoots.elements[i];
      }
    }
    Vector<Rational>& theRoot= posRoots.TheObjects[theBorelIndex];
    int theIndex=this->theWeyl.RootSystem.GetIndex(theRoot);
    Vector<Rational> smallRoot2;
    int FirstIndexFirstPosChoice=-1;
//    int FirstIndexFirstNegChoice;
    int SecondIndexFirstPosChoice=-1;
//    int SecondIndexFirstNegChoice;
    Rational CurrentHeight;
    for (int i=0; i<this->theWeyl.RootsOfBorel.size; i++)
    { Vector<Rational>& smallRoot1=this->theWeyl.RootsOfBorel.TheObjects[i];
      CurrentHeight=smallRoot1.SumCoords();
      int FirstPosIndex= this->theWeyl.RootSystem.GetIndex(smallRoot1);
      int FirstNegIndex= this->theWeyl.RootSystem.GetIndex(-smallRoot1);
      if (theHeight.IsGreaterThan(CurrentHeight))
      { smallRoot2=theRoot-smallRoot1;
        int SecondPosIndex=this->theWeyl.RootSystem.GetIndex(smallRoot2);
        if (FirstPosIndex<SecondPosIndex)
          if (SecondPosIndex!=-1)
          { int SecondNegIndex=this->theWeyl.RootSystem.GetIndex(-smallRoot2);
            if (FirstIndexFirstPosChoice==-1)
            { FirstIndexFirstPosChoice=FirstPosIndex;
              SecondIndexFirstPosChoice=SecondPosIndex;
//              FirstIndexFirstNegChoice=FirstNegIndex;
//              SecondIndexFirstNegChoice= SecondNegIndex;
              int thePower;
              this->GetMaxQForWhichBetaMinusQAlphaIsARoot(smallRoot1, smallRoot2, thePower);
              this->ChevalleyConstants.elements[FirstNegIndex][SecondNegIndex]=-1-thePower;
              this->Computed.elements[FirstNegIndex][SecondNegIndex]=true;
            }
            else
              this->ComputeOneChevalleyConstant(FirstIndexFirstPosChoice, SecondIndexFirstPosChoice, FirstNegIndex, SecondNegIndex, theIndex);
            //this->ComputeDebugString();
            this->ExploitSymmetryAndCyclicityChevalleyConstants(FirstNegIndex, SecondNegIndex);
            //this->ComputeDebugString();
          }
      }
//      if (this->flagAnErrorHasOccurredTimeToPanic)
//        this->ComputeDebugString(false, false, theGlobalVariables);
    }
    nonExploredRoots.selected[theBorelIndex]=false;
    nonExploredRoots.ComputeIndicesFromSelection();
  }
  double startMultTable=-1;
  if (theGlobalVariables!=0)
  { out << "done in " << theGlobalVariables->GetElapsedSeconds() - startStructureConstantComputation
    << " seconds.<br> Computing Lie bracket pairing (``multplication'') table...";
    theReport.Report(out.str());
    startMultTable=theGlobalVariables->GetElapsedSeconds();
  }
  this->ComputeMultTable(*theGlobalVariables);
  if (theGlobalVariables!=0)
  { out << " done in " << theGlobalVariables->GetElapsedSeconds()-startMultTable
    << " seconds. Total structure constant computation time: "
    << theGlobalVariables->GetElapsedSeconds()- startTimer
    << " seconds. ";
    theReport.Report(out.str());
  }
  if (this->GetNumPosRoots()<=0)
  { std::cout
    << "This is a programming error: number of positive roots of a semisimple "
    << "Lie algebra is reported to be zero. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__ );
    assert(false);
  }
//  this->TestForConsistency(theGlobalVariables);
}

void SemisimpleLieAlgebra::ComputeMultTable(GlobalVariables& theGlobalVariables)
{ int numPosRoots=this->theWeyl.RootsOfBorel.size;
  int theRank= this->theWeyl.CartanSymmetric.NumRows;
  int numRoots = numPosRoots*2;
  int numGenerators=numRoots+theRank;
  Vector<Rational> tempRoot;
  this->theLiebrackets.init(numGenerators, numGenerators);
//  this->theLiebracketPairingIndices.init(numGenerators, numGenerators);
  this->UEGeneratorOrderIncludingCartanElts.initFillInObject(numGenerators, -1);
//  this->theLiebracketPairingIndices.NullifyAll(-1);
//  this->OppositeRootSpaces.initFillInObject(numRoots+theDimension, -1);
  Vector<Rational> leftWeight, rightWeight, hRoot;
  for (int i=0; i<numGenerators; i++)
  { leftWeight=this->GetWeightOfGenerator(i);
    for (int j=i; j<numGenerators; j++)
    { rightWeight=this->GetWeightOfGenerator(j);
      if (leftWeight.IsEqualToZero() && rightWeight.IsEqualToZero())
      { this->theLiebrackets.elements[i][j].MakeZero(*this);
        continue;
      }
      if (leftWeight.IsEqualToZero() && !rightWeight.IsEqualToZero())
      { this->theLiebrackets.elements[i][j].MakeGenerator(j, *this);
        hRoot.MakeEi(theRank, i-numPosRoots);
        this->theLiebrackets.elements[i][j]*=Vector<Rational>::ScalarProduct(hRoot, rightWeight, this->theWeyl.CartanSymmetric);
        continue;
      }
      if (!leftWeight.IsEqualToZero() && rightWeight.IsEqualToZero())
      { this->theLiebrackets.elements[i][j].MakeGenerator(i, *this);
        hRoot.MakeEi(theRank, j-numPosRoots);
        this->theLiebrackets.elements[i][j]*=-Vector<Rational>::ScalarProduct(hRoot, leftWeight, this->theWeyl.CartanSymmetric);
        continue;
      }
      if (!leftWeight.IsEqualToZero() && !rightWeight.IsEqualToZero())
      { int newIndex=this->GetGeneratorFromRoot(leftWeight+rightWeight);
        if (newIndex!=-1)
        { this->theLiebrackets.elements[i][j].MakeGenerator(newIndex, *this);
          int leftIndex=this->GetRootIndexFromGenerator(i);
          int rightIndex=this->GetRootIndexFromGenerator(j);
          this->theLiebrackets.elements[i][j]*=this->ChevalleyConstants.elements[leftIndex][rightIndex];
        } else
        { if (!(leftWeight+rightWeight).IsEqualToZero())
            this->theLiebrackets.elements[i][j].MakeZero(*this);
          else
          { ElementSemisimpleLieAlgebra<Rational>& current=this->theLiebrackets.elements[i][j];
            current.MakeHgenerator(leftWeight*2/(this->theWeyl.RootScalarCartanRoot(leftWeight, leftWeight)), *this);
          }
        }
        continue;
      }
    }
  }
  for (int i=0; i<numGenerators; i++)
    for (int j=i+1; j<numGenerators; j++)
    { this->theLiebrackets.elements[j][i]=this->theLiebrackets.elements[i][j];
      this->theLiebrackets.elements[j][i]*=-1;
    }
  for (int i=0; i<numGenerators; i++)
    this->UEGeneratorOrderIncludingCartanElts[i]=i;
}

void SemisimpleLieAlgebra::ExploitSymmetryAndCyclicityChevalleyConstants(int indexI, int indexJ)
{ Vector<Rational>& rootI= this->theWeyl.RootSystem.TheObjects[indexI];
  Vector<Rational>& rootJ= this->theWeyl.RootSystem.TheObjects[indexJ];
  assert(!(rootI+rootJ).IsEqualToZero());
  //int indexMinusI= this->theWeyl.RootSystem.GetIndex(-rootI);
  //int indexMinusJ= this->theWeyl.RootSystem.GetIndex(-rootJ);
  //this->ComputeDebugString();
  this->ExploitSymmetryChevalleyConstants(indexI, indexJ);
  //this->ComputeDebugString();
  //int indexRootIPlusRootJ=this->theWeyl.RootSystem.GetIndex(rootI+rootJ);
  int indexMinusIMinusJ=this->theWeyl.RootSystem.GetIndex(-rootI-rootJ);
  this->ExploitTheCyclicTrick(indexI, indexJ, indexMinusIMinusJ);
  //this->ComputeDebugString();
  //this->ExploitTheCyclicTrick(indexMinusI, indexMinusJ, indexRootIPlusRootJ);
  //this->ComputeDebugString();
}

void SemisimpleLieAlgebra::ExploitSymmetryChevalleyConstants(int indexI, int indexJ)
{ Vector<Rational>& rootI= this->theWeyl.RootSystem.TheObjects[indexI];
  Vector<Rational>& rootJ= this->theWeyl.RootSystem.TheObjects[indexJ];
  assert(this->Computed.elements[indexI][indexJ]);
  int indexMinusI = this->theWeyl.RootSystem.GetIndex(-rootI);
  int indexMinusJ = this->theWeyl.RootSystem.GetIndex(-rootJ);
  this->ChevalleyConstants.elements[indexJ][indexI].Assign(this->ChevalleyConstants.elements[indexI][indexJ]*(-1));
  //this->ComputeDebugString();
  this->Computed.elements[indexJ][indexI]=true;
  assert(!(rootI+rootJ).IsEqualToZero());
  int thePower;
  this->GetMaxQForWhichBetaMinusQAlphaIsARoot
  (this->theWeyl.RootSystem.TheObjects[indexMinusI],
   this->theWeyl.RootSystem.TheObjects[indexMinusJ], thePower);
  int i=1+thePower;
  this->ChevalleyConstants.elements[indexMinusI][indexMinusJ].AssignInteger(-i*i);
  this->ChevalleyConstants.elements[indexMinusI][indexMinusJ].DivideBy(this->ChevalleyConstants.elements[indexI][indexJ]);
  this->Computed.elements[indexMinusI][indexMinusJ]=true;
  //this->ComputeDebugString();
  this->ChevalleyConstants.elements[indexMinusJ][indexMinusI].Assign(this->ChevalleyConstants.elements[indexMinusI][indexMinusJ]*(-1));
  this->Computed.elements[indexMinusJ][indexMinusI]=true;
  //this->ComputeDebugString();
}

void SemisimpleLieAlgebra::ExploitTheCyclicTrick(int i, int j, int k)
{ Vector<Rational>& rootI= this->theWeyl.RootSystem.TheObjects[i];
  Vector<Rational>& rootK= this->theWeyl.RootSystem.TheObjects[k];
  Vector<Rational>& rootJ= this->theWeyl.RootSystem.TheObjects[j];
  //the following three checks can be commented out to increase speed. They have never failed so far.
  assert((rootI+rootK+rootJ).IsEqualToZero());
  assert(!(rootI+rootJ).IsEqualToZero() && !(rootK+rootK).IsEqualToZero() && !(rootJ+rootI).IsEqualToZero());
  assert(this->Computed.elements[i][j]);
  /////////////////////////////////////////////////////////////////
  Rational& tempRat= this->ChevalleyConstants.elements[i][j];
  Rational tempRat2= this->theWeyl.RootScalarCartanRoot(rootK, rootK);
  this->ChevalleyConstants.elements[j][k]=(tempRat*this->theWeyl.RootScalarCartanRoot(rootI, rootI))/tempRat2;
  this->Computed.elements[j][k]=true;
  this->ChevalleyConstants.elements[k][i] = (tempRat*this->theWeyl.RootScalarCartanRoot(rootJ, rootJ))/tempRat2;
  this->Computed.elements[k][i]=true;
  this->ExploitSymmetryChevalleyConstants(j, k);
  this->ExploitSymmetryChevalleyConstants(k, i);
}

bool SemisimpleLieAlgebra::GetMaxQForWhichBetaMinusQAlphaIsARoot
(const Vector<Rational>& alpha, const Vector<Rational>& beta, int& output)
{ output=-1;
  Vector<Rational> tempRoot=beta;
  if (alpha.IsEqualToZero())
  { std::cout << "This is a programming error: calling function GetMaxQForWhichBetaMinusQAlphaIsARoot"
    << " with zero value for alpha is not allowed. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  bool foundRoot=false;
  while (this->theWeyl.IsARoot(tempRoot))
  { output++;
    tempRoot-=(alpha);
    foundRoot=true;
  }
  return foundRoot;
}

void SemisimpleLieAlgebra::ComputeOneChevalleyConstant
(int indexGamma, int indexDelta, int indexMinusEpsilon, int indexMinusZeta, int indexEta)
{//using formula (**), 2.9, page 49, Samelson, Notes on Lie algebras, 1989
  Vector<Rational>& gamma= this->theWeyl.RootSystem.TheObjects[indexGamma];
  Vector<Rational>& delta= this->theWeyl.RootSystem.TheObjects[indexDelta];
  Vector<Rational>& minusEpsilon= this->theWeyl.RootSystem.TheObjects[indexMinusEpsilon];
  Vector<Rational>& eta= this->theWeyl.RootSystem.TheObjects[indexEta];
  Vector<Rational>& minusZeta=this->theWeyl.RootSystem.TheObjects[indexMinusZeta];
  assert(eta==gamma+delta);
  assert(this->theWeyl.IsARoot(eta+minusEpsilon));
  assert(this->Computed.elements[indexDelta][indexMinusEpsilon] && this->Computed.elements[indexMinusEpsilon][indexGamma] && this->Computed.elements[indexGamma][indexDelta] );
  assert(!this->ChevalleyConstants.elements[indexGamma][indexDelta].IsEqualToZero());
  int indexDeltaMinusEpsilon= this->theWeyl.RootSystem.GetIndex(delta+minusEpsilon);
  int indexGammaMinusEpsilon= this->theWeyl.RootSystem.GetIndex(gamma+minusEpsilon);
  Rational FirstSummand, SecondSummand;
  if (indexDeltaMinusEpsilon!=-1)
  { assert(this->Computed.elements[indexGamma][indexDeltaMinusEpsilon] && this->Computed.elements[indexDelta][indexMinusEpsilon]);
    FirstSummand=this->ChevalleyConstants.elements[indexGamma][indexDeltaMinusEpsilon]*this->ChevalleyConstants.elements[indexDelta][indexMinusEpsilon];
  } else
    FirstSummand.MakeZero();
  if (indexGammaMinusEpsilon!=-1)
  { assert(this->Computed.elements[indexDelta][indexGammaMinusEpsilon] && this->Computed.elements[indexMinusEpsilon][indexGamma]);
    SecondSummand =this->ChevalleyConstants.elements[indexDelta][indexGammaMinusEpsilon]*this->ChevalleyConstants.elements[indexMinusEpsilon][indexGamma];
  } else
    SecondSummand.MakeZero();
  this->ChevalleyConstants.elements[indexMinusEpsilon][indexMinusZeta]=(this->theWeyl.RootScalarCartanRoot(eta, eta)/ this->theWeyl.RootScalarCartanRoot(minusZeta, minusZeta))*( FirstSummand+SecondSummand)/ this->ChevalleyConstants.elements[indexGamma][indexDelta];
  this->Computed.elements[indexMinusEpsilon][indexMinusZeta]=true;
}

bool SemisimpleLieAlgebra::TestForConsistency(GlobalVariables& theGlobalVariables)
{ //HashedList<Vector<Rational> >& theRoots=this->theWeyl.RootSystem;
  FormatExpressions& theFormat=theGlobalVariables.theDefaultFormat;
  ElementSemisimpleLieAlgebra<Rational> g1, g2, g3, g23, g31, g12, g123, g231, g312, temp;
  //this->ComputeDebugString(false, false, theGlobalVariables);
  for (int i=0; i<this->GetNumGenerators(); i++)
  { g1.MakeGenerator(i, *this);
    for (int j=0; j<this->GetNumGenerators(); j++)
    { g2.MakeGenerator(j, *this);
      for (int k=0; k<this->GetNumGenerators(); k++)
      { g3.MakeGenerator(k, *this);
        this->LieBracket(g2, g3, g23); this->LieBracket(g1, g23, g123);
        this->LieBracket(g3, g1, g31); this->LieBracket(g2, g31, g231);
        this->LieBracket(g1, g2, g12); this->LieBracket(g3, g12, g312);
        temp=g123;
        temp+=g231;
        temp+=g312;
        if (!temp.IsEqualToZero())
        { std::cout << "This is a programming error. The computed structure constants are wrong: "
          << " the Jacobi identity fails. More precisely, I get that "
          << "<br>[" << g1.ToString(&theFormat) << ", " << g2.ToString(&theFormat) << "]=" << g12.ToString(&theFormat)
          << "<br>[" << g2.ToString(&theFormat) << ", " << g3.ToString(&theFormat) << "]=" << g23.ToString(&theFormat)
          << "<br>[" << g3.ToString(&theFormat) << ", " << g1.ToString(&theFormat) << "]=" << g31.ToString(&theFormat)
          << "<br>g123= " << g123.ToString(&theFormat) << "<br>g231="
          << g231.ToString(&theFormat)
          << "<br>g312=" << g312.ToString(&theFormat) << "<br>"
          << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
          assert(false);
          return false;
        }
        this->MakeChevalleyTestReport(i, j, k, this->GetNumGenerators(), theGlobalVariables);
      }
    }
  }
  return true;
}

Rational SemisimpleLieAlgebra::GetConstant(const Vector<Rational> &root1, const  Vector<Rational> &root2)
{ int index1=this->theWeyl.RootSystem.GetIndex(root1);
  int index2= this->theWeyl.RootSystem.GetIndex(root2);
  Rational tempRat;
  if (index1==-1 || index2==-1)
  { tempRat.MakeZero();
    return tempRat;
  }
  tempRat.Assign(this->ChevalleyConstants.elements[index1][index2]);
  return tempRat;
}

bool SemisimpleLieAlgebra::GetConstantOrHElement
(const Vector<Rational>& root1, const Vector<Rational>& root2, Rational& outputRat, Vector<Rational>& outputH)
{ if (!(root1+root2).IsEqualToZero())
  { outputRat=this->GetConstant(root1, root2);
    return true;
  }
  if (this->theWeyl.RootSystem.GetIndex(root1)==-1 || this->theWeyl.RootSystem.GetIndex(root2)==-1)
  { outputRat.MakeZero();
    return true;
  }
  outputH=(root1*2)/Vector<Rational>::ScalarProduct(root1, root1, this->theWeyl.CartanSymmetric);
  return false;
}

void SemisimpleLieAlgebra::MakeChevalleyTestReport(int i, int j, int k, int Total, GlobalVariables& theGlobalVariables)
{ if (theGlobalVariables.GetFeedDataToIndicatorWindowDefault()==0)
    return;
  std::stringstream out2, out3;
  int x=(i*Total*Total+j*Total+k+1);
  out2 << "i: " << i+1 << " of " << Total << " j: " << j+1 << " of "
  << Total << " k: " << k+1 << " of " << Total;
  out3 << "Total progress: " << x << " out of " << (Total*Total*Total);
  ProgressReport theReport(&theGlobalVariables);
  theReport.Report(out2.str()+out3.str());
}

void WeylGroup::GenerateRootSubsystem(Vectors<Rational>& theRoots)
{ Vector<Rational> tempRoot;
  int oldsize=theRoots.size;
  for (int i=0; i<oldsize; i++)
    theRoots.AddOnTopNoRepetition(-theRoots[i]);
  for (int i =0; i<theRoots.size; i++)
    for (int j=0; j<theRoots.size; j++)
    { tempRoot= theRoots[i]+theRoots[j];
      if (this->IsARoot(tempRoot))
        theRoots.AddOnTopNoRepetition(tempRoot);
    }
}

bool rootSubalgebra::attemptExtensionToIsomorphism
(Vectors<Rational>& Domain, Vectors<Rational>& Range,
 GlobalVariables& theGlobalVariables, ReflectionSubgroupWeylGroup* outputAutomorphisms,
 bool actOnCentralizerOnly, SemisimpleLieAlgebra& inputOwner,
 bool* DomainAndRangeGenerateNonIsoSAs)
{ if (outputAutomorphisms!=0)
    outputAutomorphisms->ExternalAutomorphisms.size=0;
  if (DomainAndRangeGenerateNonIsoSAs!=0)
    *DomainAndRangeGenerateNonIsoSAs=false;
  //rootSubalgebra::ProblemCounter++;
  rootSubalgebra& theDomainRootSA = theGlobalVariables.rootSAAttemptExtensionToIso1.GetElement();
  rootSubalgebra& theRangeRootSA = theGlobalVariables.rootSAAttemptExtensionToIso2.GetElement();
  theDomainRootSA.init(inputOwner);
  theRangeRootSA.init(inputOwner);
  theDomainRootSA.genK=(Domain);
  theRangeRootSA.genK=(Range);
  theDomainRootSA.ComputeAllButAmbientWeyl();
  theRangeRootSA.ComputeAllButAmbientWeyl();
  if (theDomainRootSA.theDynkinDiagram.ToString()!= theRangeRootSA.theDynkinDiagram.ToString() ||
      theDomainRootSA.theCentralizerDiagram.ToString()!=theRangeRootSA.theCentralizerDiagram.ToString())
  { if (DomainAndRangeGenerateNonIsoSAs!=0)
      *DomainAndRangeGenerateNonIsoSAs=true;
    return false;
  }
  Vectors<Rational> isoDomain, isoRange;
  permutation permComponentsCentralizer;
  List<int> tempList, tempPermutation1, tempPermutation2;
  SelectionWithDifferentMaxMultiplicities tempAutosCentralizer;
  List<List<List<int> > > CentralizerDiagramAutomorphisms;
  theDomainRootSA.theCentralizerDiagram.GetAutomorphisms(CentralizerDiagramAutomorphisms);
  theDomainRootSA.theCentralizerDiagram.ToString();
  tempAutosCentralizer.initIncomplete(CentralizerDiagramAutomorphisms.size);
  for (int i=0; i<CentralizerDiagramAutomorphisms.size; i++)
    tempAutosCentralizer.MaxMultiplicities[i] = CentralizerDiagramAutomorphisms[i].size-1;
  tempList.SetSize(theDomainRootSA.theCentralizerDiagram.sameTypeComponents.size);
  int tempSize=0;
  for (int i=0; i<theDomainRootSA.theCentralizerDiagram.sameTypeComponents.size; i++)
  { tempList[i]=theDomainRootSA.theCentralizerDiagram.sameTypeComponents[i].size;
    tempSize+=tempList[i];
  }
  permComponentsCentralizer.initPermutation(tempList, tempSize);
  int tempI2= permComponentsCentralizer.getTotalNumSubsets();
  int NumAutosCentralizer= tempAutosCentralizer.getTotalNumSubsets();
  permComponentsCentralizer.GetPermutationLthElementIsTheImageofLthIndex(tempPermutation2);
  Matrix<Rational> tempMat;
  Selection tempSel;
  for (int i=0; i<Domain.size; i++)
  { isoDomain.AddOnTop(Domain[i]);
    if (isoDomain.GetRankOfSpanOfElements(&tempMat, &tempSel)<isoDomain.size)
      isoDomain.RemoveLastObject();
    else
      isoRange.AddOnTop(Range[i]);
  }
  if (isoRange.GetRankOfSpanOfElements(&tempMat, &tempSel)<isoRange.size)
    return false;
  int givenSize=isoDomain.size;
  for(int j=0; j<tempI2; j++)
  { for(int l=0; l<NumAutosCentralizer; l++)
    { isoDomain.size=givenSize; isoRange.size=givenSize;
      theDomainRootSA.theCentralizerDiagram.GetMapFromPermutation(isoDomain, isoRange, tempPermutation2, CentralizerDiagramAutomorphisms, tempAutosCentralizer, theRangeRootSA.theCentralizerDiagram);
      if (theDomainRootSA.attemptExtensionToIsomorphismNoCentralizer(isoDomain, isoRange, theGlobalVariables, 0, outputAutomorphisms, false, 0, &Domain, &Range))//GenerateAllAutos))
        if (outputAutomorphisms==0)
          return true;
      if (outputAutomorphisms!=0)
        theDomainRootSA.MakeProgressReportGenAutos(l+NumAutosCentralizer*j, tempI2*NumAutosCentralizer, outputAutomorphisms->ExternalAutomorphisms.size, theGlobalVariables);
      tempAutosCentralizer.IncrementSubset();
    }
    permComponentsCentralizer.incrementAndGetPermutation(tempPermutation2);
  }
  return false;
}

int rootSubalgebra::ProblemCounter=0;
bool rootSubalgebra::GenerateIsomorphismsPreservingBorel(rootSubalgebra& right, GlobalVariables& theGlobalVariables, ReflectionSubgroupWeylGroup* outputAutomorphisms, bool actOnCentralizerOnly)
{ if (this->theDynkinDiagram.ToString()!= right.theDynkinDiagram.ToString())
    return false;
  if (this->theCentralizerDiagram.ToString()!= right.theCentralizerDiagram.ToString())
    return false;
  if (outputAutomorphisms!=0)
  { outputAutomorphisms->ExternalAutomorphisms.size=0;
    outputAutomorphisms->simpleGenerators.size=0;
    outputAutomorphisms->simpleGenerators=(this->SimpleBasisCentralizerRoots);
  }
  Vectors<Rational> isoDomain, isoRange;
  permutation permComponents, permComponentsCentralizer;
  List<int> tempList, tempPermutation1, tempPermutation2;
  SelectionWithDifferentMaxMultiplicities tempAutos, tempAutosCentralizer;
  List<List<List<int> > > DiagramAutomorphisms, CentralizerDiagramAutomorphisms;
  this->theDynkinDiagram.GetAutomorphisms(DiagramAutomorphisms);
  this->theCentralizerDiagram.GetAutomorphisms(CentralizerDiagramAutomorphisms);
  tempAutos.initIncomplete(DiagramAutomorphisms.size);
  tempAutosCentralizer.initIncomplete(CentralizerDiagramAutomorphisms.size);
  for (int i=0; i<DiagramAutomorphisms.size; i++)
    tempAutos.MaxMultiplicities[i]= DiagramAutomorphisms[i].size-1;
  for (int i=0; i<CentralizerDiagramAutomorphisms.size; i++)
    tempAutosCentralizer.MaxMultiplicities[i]= CentralizerDiagramAutomorphisms[i].size-1;
  tempList.SetSize(this->theDynkinDiagram.sameTypeComponents.size);
  int tempSize=0;
  for (int i=0; i<this->theDynkinDiagram.sameTypeComponents.size; i++)
  { tempList[i]= this->theDynkinDiagram.sameTypeComponents[i].size;
    tempSize+=tempList[i];
  }
  permComponents.initPermutation(tempList, tempSize);
  tempList.SetSize(this->theCentralizerDiagram.sameTypeComponents.size);
  tempSize=0;
  for (int i=0; i<this->theCentralizerDiagram.sameTypeComponents.size; i++)
  { tempList[i]= this->theCentralizerDiagram.sameTypeComponents[i].size;
    tempSize+=tempList[i];
  }
  permComponentsCentralizer.initPermutation(tempList, tempSize);
  int tempI1;
  int NumAutos;
  tempI1= permComponents.getTotalNumSubsets();
  NumAutos=tempAutos.getTotalNumSubsets();
  int tempI2= permComponentsCentralizer.getTotalNumSubsets();
  int NumAutosCentralizer= tempAutosCentralizer.getTotalNumSubsets();
  permComponents.GetPermutationLthElementIsTheImageofLthIndex(tempPermutation1);
  permComponentsCentralizer.GetPermutationLthElementIsTheImageofLthIndex(tempPermutation2);
  for (int i=0; i<tempI1; i++)
  { for(int j=0; j<tempI2; j++)
    { for (int k=0; k<NumAutos; k++)
      { for(int l=0; l<NumAutosCentralizer; l++)
        { isoDomain.size=0; isoRange.size=0;
          this->theDynkinDiagram.GetMapFromPermutation(isoDomain, isoRange, tempPermutation1, DiagramAutomorphisms, tempAutos, right.theDynkinDiagram);
          this->theCentralizerDiagram.GetMapFromPermutation(isoDomain, isoRange, tempPermutation2, CentralizerDiagramAutomorphisms, tempAutosCentralizer, right.theCentralizerDiagram);
          if (this->attemptExtensionToIsomorphismNoCentralizer(isoDomain, isoRange, theGlobalVariables, 0, outputAutomorphisms, false, 0, 0, 0))//GenerateAllAutos))
            if (outputAutomorphisms==0)
              return true;
          if (outputAutomorphisms!=0)
            this->MakeProgressReportGenAutos( l+NumAutosCentralizer*(k+NumAutos*(j+i*tempI2)), tempI1*tempI2*NumAutos*NumAutosCentralizer, outputAutomorphisms->ExternalAutomorphisms.size, theGlobalVariables);
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


