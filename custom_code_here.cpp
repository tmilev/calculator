#include "polyhedra.h"
// This file is meant to be for people to modify if they do not want to modify the main files polyhedra.cpp or polyhedra.h
// The reason I would recommend that is because the file polyhedra.cpp compiles very slowly (around 30 seconds), so small modifications
// take a long time to check. Using a separate file and linking it to the GUI is the solution I chose (may not be the best, see below for discussion).
// If the code you write here reaches the mature phase where you have realized all functions you think it should have
// and you are generally satisfied with it, simply cut & paste it in the main files (the class declarations in polyhedra.h and the implementation in polyhedra.cpp).

//Discussion: other options for speeding up the compilation that I have considered.
//1. Cut up the main files polyhedra.h and polyhedra.cpp into small quick-to-compile pieces. This might be the best solution in the long run. However, I do not want to
//   do it just yet, because 1) I am not sure what should those parts be - it is not yet clear in my head how to partition the code in conceptually distinct pieces
//   2) this would certainly create additional maintainance work 3) this will increase the learning curve for a person wanting to just use the program and wanting to eventually
//   modify some tiny bit and 4) I got an advice on the c++ forum www.cplusplus.com that partitioning the .h file will eventually lead to slower compile times,
//   especially with the massive use of templates that I do. Therefore, such a partitioning should not be done before the code reaches
//   greater maturity (see also point (1) ).
//2. Use precompiled headers or some other voodoo. I am tocally against that. Those are compiler specific, will require me to learn extra unnecessary info which might
//    be out of date in a few years, and will make even higher entry learning curve for another to join the project. This is bad.
//    I should mention in favor of Microsoft that their IDE does recompile very quickly small modifications of the file polyhedra.cpp. I believe it does so by
//    keeping a database of your recently changed code, and recompiling only the recently changed pieces. Hats off to Microsoft for doing that completely programmatically,
//    and not bothering the programmer with stupid things such as how to set up precompiled headers.
//
// To whomever might be reading this (if anyone): happy hacking and I hope you find my code useful, that it didn't cause you many headaches, and that you
// did something useful with it! Cheers!

void ComputationSetup::ComputeCharaterFormulas(ComputationSetup& inputData, GlobalVariables& theGlobalVariables)
{ if (inputData.thePartialFraction.theChambers.flagSliceWithAWallInitDone)
    while (inputData.thePartialFraction.theChambers.oneStepChamberSlice(theGlobalVariables)){}
  else
  { WeylGroup A2test;
    A2test.MakeArbitrary('G', 2);
    A2test.ComputeRho(true);
    inputData.thePartialFraction.theChambers.initCharacterComputation(A2test, A2test.RootsOfBorel, theGlobalVariables);
  }
}

void GeneralizedVermaModuleCharacters::TransformToWeylProjective
  (int indexOperator, root& startingNormal, root& outputNormal)
{ MatrixLargeRational theOperatorExtended=this->theLinearOperatorsExtended.TheObjects[indexOperator];
  root& theTranslation=this->theTranslationsProjected.TheObjects[indexOperator];
  Rational theConst=root::RootScalarEuclideanRoot(theTranslation+this->NonIntegralOriginModification, startingNormal);
  theOperatorExtended.Transpose();
  outputNormal=startingNormal;
  theOperatorExtended.ActOnAroot(outputNormal);
  outputNormal.SetSize(outputNormal.size+1);
  *outputNormal.LastObject()=-theConst;
}

void GeneralizedVermaModuleCharacters::TransformToWeylProjectiveStep1
(GlobalVariables& theGlobalVariables)
{ this->projectivizedChamberOld.flagSliceWithAWallInitDone=true;
  this->projectivizedChamberOld.theDirections=this->GmodKnegativeWeights;
  this->projectivizedChamberOld.init();
  this->projectivizedChamberOld.SliceTheEuclideanSpace(theGlobalVariables, false);
  this->projectivizedChamberOld.drawOutput(theGlobalVariables.theDrawingVariables, this->projectivizedChamberOld.IndicatorRoot, 0);
  List<roots> tempChambers;
  tempChambers.MakeActualSizeAtLeastExpandOnTop(this->projectivizedChamberOld.size);
  for (int j=0; j<this->projectivizedChamberOld.size; j++)
    if (this->projectivizedChamberOld.TheObjects[j]!=0)
      if (!this->projectivizedChamberOld.TheObjects[j]->flagPermanentlyZero)
      { CombinatorialChamber& current=*this->projectivizedChamberOld.TheObjects[j];
        tempChambers.SetSize(tempChambers.size+1);
        current.GetWallNormals(*tempChambers.LastObject());
      }
  this->projectivizedChamber.initFromCones(tempChambers, theGlobalVariables);
  this->projectivizedChamber.splittingNormals.ClearTheObjects();
  this->projectivizedChamber.splittingNormals.AddListOnTopNoRepetitionOfObjectHash(this->projectivizedChamberOld.theHyperplanes);
  this->log << this->projectivizedChamber.ElementToString(false, false);
  theGlobalVariables.theIndicatorVariables.StatusString1=this->log.str();
}

void GeneralizedVermaModuleCharacters::TransformToWeylProjectiveStep2
(GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  ConeComplex projectivizedChamberFinal;
  Cone currentProjectiveCone;
  roots tempRoots, extraWeylChamberWalls;
  root tempRoot, wallAtInfinity, wallToSliceWith;
//  int dimSmallerAlgebra=this->theLinearOperators.TheObjects[0].NumRows;
//  int dimLargerAlgebra=this->theLinearOperators.TheObjects[0].NumCols;
//  int dimFinal=dimSmallerAlgebra+dimLargerAlgebra+1;
  projectivizedChamberFinal.init();
  for (int i=0; i<this->projectivizedChamber.size; i++)
  { Cone& currentAffineCone=this->projectivizedChamber.TheObjects[i];
    tempRoots.SetSize(currentAffineCone.Normals.size);
    for (int j=0; j<currentAffineCone.Normals.size; j++)
      this->TransformToWeylProjective(0, currentAffineCone.Normals.TheObjects[j], tempRoots.TheObjects[j]);
    tempRoots.AddListOnTop(this->PreimageWeylChamberLargerAlgebra);
    currentProjectiveCone.CreateFromNormals(tempRoots, theGlobalVariables);
    projectivizedChamberFinal.AddNonRefinedChamberOnTopNoRepetition(currentProjectiveCone);
  }
  for (int i=0; i<this->PreimageWeylChamberSmallerAlgebra.size; i++)
    projectivizedChamberFinal.splittingNormals.AddObjectOnTopHash(this->PreimageWeylChamberSmallerAlgebra.TheObjects[i]);
  out << "projectivized chamber before chopping non-dominant part:\n"  << projectivizedChamberFinal.ElementToString(false, false);
  projectivizedChamberFinal.Refine(theGlobalVariables);
  out << "Refined projectivized chamber before chopping non-dominant part:\n"  << projectivizedChamberFinal.ElementToString(false, false);
  for (int i=0; i<projectivizedChamberFinal.size; i++)
  { Cone& currentCone=projectivizedChamberFinal.TheObjects[i];
    bool isNonDominant=false;
    for (int j=0; j< this->PreimageWeylChamberSmallerAlgebra.size; j++)
      if (root::RootScalarEuclideanRoot(currentCone.GetInternalPoint(), this->PreimageWeylChamberSmallerAlgebra.TheObjects[j]).IsNegative())
      { isNonDominant=true;
        break;
      }
    if (isNonDominant)
    { projectivizedChamberFinal.PopChamberSwapWithLast(i);
      i--;
    }
  }
  projectivizedChamberFinal.indexLowestNonRefinedChamber=0;
  this->projectivizedChamber=projectivizedChamberFinal;
  for (int k=1; k<this->theLinearOperators.size; k++)
    for (int i=0; i<this->projectivizedChamberOld.size; i++)
      if (this->projectivizedChamberOld.TheObjects[i]!=0)
        for (int j=0; j<this->projectivizedChamberOld.TheObjects[i]->Externalwalls.size; j++)
        { this->TransformToWeylProjective(k, this->projectivizedChamberOld.TheObjects[i]->Externalwalls.TheObjects[j].normal, wallToSliceWith);
          wallToSliceWith.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
          this->projectivizedChamber.splittingNormals.AddObjectOnTopNoRepetitionOfObjectHash(wallToSliceWith);
        }
  out << "projectivized chamber chopped non-dominant part:\n"  << this->projectivizedChamber.ElementToString(false, false);
  theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
  this->projectivizedChamberOld.NumAffineHyperplanesProcessed=-1;

//  this->projectivizedChamberOld.NewHyperplanesToSliceWith.size=1;
}

void GeneralizedVermaModuleCharacters::initFromHomomorphism
  (HomomorphismSemisimpleLieAlgebra& input, GlobalVariables& theGlobalVariables)
{ roots tempRoots;
  WeylGroup& theWeyl=input.theRange.theWeyl;
//  input.ProjectOntoSmallCartan(theWeyl.RootsOfBorel, tempRoots, theGlobalVariables);
  this->log << "projections: " << tempRoots.ElementToString();
  theWeyl.ComputeWeylGroup();
  this->theLinearOperators.SetSize(theWeyl.size);
  this->theLinearOperatorsExtended.SetSize(theWeyl.size);
  this->theTranslations.SetSize(theWeyl.size);
  this->theTranslationsProjected.SetSize(theWeyl.size);
  this->theCoeffs.SetSize(theWeyl.size);
  MatrixLargeRational theProjection;
  theProjection.init(input.theDomain.GetRank(), input.theRange.GetRank());
  root startingWeight, projectedWeight;
  SSalgebraModule tempM;
  std::stringstream tempStream;
  input.ComputeHomomorphismFromImagesSimpleChevalleyGenerators(theGlobalVariables);
  tempM.InduceFromEmbedding(tempStream, input, theGlobalVariables);
  input.GetWeightsGmodK(this->GmodKnegativeWeights, theGlobalVariables);
  MatrixLargeRational tempMat;
  tempMat=input.theDomain.theWeyl.CartanSymmetric;
  tempMat.Invert(theGlobalVariables);
  tempMat.ActOnRoots(this->GmodKnegativeWeights);
  this->log << this->GmodKnegativeWeights.ElementToString();
  for (int i=0; i<this->GmodKnegativeWeights.size; i++)
    if (this->GmodKnegativeWeights.TheObjects[i].IsPositiveOrZero())
    { this->GmodKnegativeWeights.PopIndexSwapWithLast(i);
      i--;
    }
  for (int i=0; i<input.theRange.GetRank(); i++)
  { startingWeight.MakeEi(input.theRange.GetRank(), i);
    input.ProjectOntoSmallCartan(startingWeight, projectedWeight, theGlobalVariables);
    for (int j=0; j<projectedWeight.size; j++)
      theProjection.elements[j][i]=projectedWeight.TheObjects[j];
  }
  this->log << "\nMatrix form of the elements of W(B_3) (" << theWeyl.size << " elements):\n";
  for (int i=0; i<theWeyl.size; i++)
  { MatrixLargeRational& currentLinearOperator=this->theLinearOperators.TheObjects[i];
    theWeyl.GetMatrixOfElement(i, currentLinearOperator);
    this->log << "\n" << currentLinearOperator.ElementToString(false, false);
    this->theTranslations.TheObjects[i]=theWeyl.rho;
    theWeyl.ActOn(i, this->theTranslations.TheObjects[i], true, false, (Rational) 0);
    this->theTranslations.TheObjects[i]-=theWeyl.rho;
    this->theTranslations.TheObjects[i].MinusRoot();
    theProjection.ActOnAroot(this->theTranslations.TheObjects[i], this->theTranslationsProjected.TheObjects[i]);
    if (theWeyl.TheObjects[i].size%2==0)
      this->theCoeffs.TheObjects[i]=1;
    else
      this->theCoeffs.TheObjects[i]=-1;
  }
  this->log << "\n\n\nMatrix of the projection operator:\n" << theProjection.ElementToString(false, false);
  this->log << "\n\n\nMatrix form of the operators $u_w$, the translations $\tau_w$ and their projections (" << this->theLinearOperatorsExtended.size << "):";
  //List<MatrixLargeRational> tempList;
  for (int k=0; k<this->theLinearOperators.size; k++)
  { MatrixLargeRational& currentLO=this->theLinearOperators.TheObjects[k];
    MatrixLargeRational& currentLOExtended=this->theLinearOperatorsExtended.TheObjects[k];
    currentLO.MultiplyOnTheLeft(theProjection);
    //tempList.AddOnTopNoRepetition(this->theLinearOperators.TheObjects[i]);
    currentLOExtended.MakeIdMatrix(currentLO.NumRows);
    currentLOExtended.Resize(currentLO.NumRows, currentLO.NumRows+currentLO.NumCols, true);
    for (int i=0; i<currentLO.NumRows; i++)
      for (int j=0; j<currentLO.NumCols; j++)
        currentLOExtended.elements[i][j+currentLO.NumRows]=-currentLO.elements[i][j];
    this->log << "\n\n" << currentLOExtended.ElementToString(false, false);
    this->log << this->theTranslations.TheObjects[k].ElementToString() << ";   " << this->theTranslationsProjected.TheObjects[k].ElementToString();
  }
//  this->log << "\n\n\nThere are " << tempList.size << " different operators.";
  tempMat=theWeyl.CartanSymmetric;
  tempMat.Invert(theGlobalVariables);
  tempRoots.AssignMatrixRows(tempMat);
  this->PreimageWeylChamberLargerAlgebra.ComputeFromDirections(tempRoots, theGlobalVariables, theWeyl.GetDim());
  this->log << "\nWeyl chamber larger algebra before projectivizing: " << this->PreimageWeylChamberLargerAlgebra.ElementToString() << "\n";
  root tempRoot;
  for (int i=0; i<this->PreimageWeylChamberLargerAlgebra.size; i++)
  { tempRoot.MakeZero(input.theRange.GetRank()+input.theDomain.GetRank()+1);
    for (int j=0; j<input.theRange.GetRank(); j++)
      tempRoot.TheObjects[j+input.theDomain.GetRank()]=this->PreimageWeylChamberLargerAlgebra.TheObjects[i].TheObjects[j];
    this->PreimageWeylChamberLargerAlgebra.TheObjects[i]=tempRoot;
  }
  tempMat=input.theDomain.theWeyl.CartanSymmetric;
  tempMat.Invert(theGlobalVariables);
  tempRoots.AssignMatrixRows(tempMat);
  this->PreimageWeylChamberSmallerAlgebra.ComputeFromDirections(tempRoots, theGlobalVariables, input.theDomain.GetRank());
  this->log << "\nWeyl chamber smaller algebra: " << this->PreimageWeylChamberSmallerAlgebra.ElementToString() << "\n";
  this->log << "\nThe first operator extended:\n" << this->theLinearOperatorsExtended.TheObjects[0].ElementToString(false, false) << "\n";
  this->log << "\nThe second operator extended:\n" << this->theLinearOperatorsExtended.TheObjects[1].ElementToString(false, false) << "\n";
  /*tempMat=this->theLinearOperatorsExtended.TheObjects[0];
  tempMat.Transpose();
  tempMat.ActOnRoots(this->PreimageWeylChamberSmallerAlgebra);
  for (int i=0; i<this->PreimageWeylChamberSmallerAlgebra.size; i++)
  { this->PreimageWeylChamberSmallerAlgebra.TheObjects[i].SetSize(input.theRange.GetRank()+input.theDomain.GetRank()+1);
    *this->PreimageWeylChamberSmallerAlgebra.TheObjects[i].LastObject()=0;
  }*/
  for (int i=0; i<this->PreimageWeylChamberSmallerAlgebra.size; i++)
  { tempRoot.MakeZero(input.theRange.GetRank()+input.theDomain.GetRank()+1);
    for (int j=0; j<input.theDomain.GetRank(); j++)
      tempRoot.TheObjects[j]=this->PreimageWeylChamberSmallerAlgebra.TheObjects[i].TheObjects[j];
  //  for (int j=0; j<input.theRange.GetRank(); j++)
   //   tempRoot.TheObjects[j+input.theDomain.GetRank()]=tempRoot2.TheObjects[j];
    this->PreimageWeylChamberSmallerAlgebra.TheObjects[i]=tempRoot;
  }
  this->NonIntegralOriginModification.SetSize(input.theDomain.GetRank());
  for (int i=0; i<this->NonIntegralOriginModification.size; i++)
    this->NonIntegralOriginModification.TheObjects[i].AssignNumeratorAndDenominator(1,2);
  tempRoot.MakeEi(input.theRange.GetRank()+input.theDomain.GetRank()+1, input.theRange.GetRank()+input.theDomain.GetRank());
  this->PreimageWeylChamberLargerAlgebra.AddObjectOnTop(tempRoot);
  this->log << "\nPreimage Weyl chamber smaller algebra: " << this->PreimageWeylChamberSmallerAlgebra.ElementToString() << "\n";
  this->log << "\nPreimage Weyl chamber larger algebra: " << this->PreimageWeylChamberLargerAlgebra.ElementToString() << "\n";

  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.theIndicatorVariables.StatusString1=this->log.str();
  theGlobalVariables.MakeReport();
}

void WeylGroup::GetMatrixOfElement(int theIndex, MatrixLargeRational& outputMatrix)
{ root tempRoot;
  int theDim=this->CartanSymmetric.NumRows;
  outputMatrix.init(theDim, theDim);
  for (int i=0; i<theDim; i++)
  { tempRoot.MakeEi(theDim, i);
    this->ActOn(theIndex, tempRoot, false, false, (Rational) 0);
    for (int j=0; j<theDim; j++)
      outputMatrix.elements[j][i]=tempRoot.TheObjects[j];
  }
}

GeneralizedVermaModuleCharacters tempCharsEraseWillBeErasedShouldntHaveLocalObjectsLikeThis;

void GeneralizedVermaModuleCharacters::GetProjection(int indexOperator, const root& input, root& output)
{ MatrixLargeRational& currentOperator=this->theLinearOperators.TheObjects[indexOperator];
  root& currentTranslation=this->theTranslations.TheObjects[indexOperator];
  root tempRoot2;
  tempRoot2.SetSize(currentOperator.NumCols);
  output.MakeZero(currentOperator.NumRows);
  for (int i=0; i<currentOperator.NumCols; i++)
    tempRoot2.TheObjects[i]=input.TheObjects[currentOperator.NumRows+i];
  for (int j=0; j<currentOperator.NumRows; j++)
    output.TheObjects[j]=input.TheObjects[j];
  currentOperator.ActOnAroot(tempRoot2);
  output-=tempRoot2;
  currentOperator.ActOnAroot(currentTranslation, tempRoot2);
  output-=tempRoot2;
}

void MatrixLargeRational::GetMatrixIntWithDen(Matrix<LargeInt>& outputMat, LargeIntUnsigned& outputDen)
{ outputDen.MakeOne();
  MatrixLargeRational tempMat;
  tempMat=*this;
  outputMat.init(this->NumRows, this->NumCols);
  LargeIntUnsigned tempUI;
  Rational tempRat;
  for (int i=0; i<this->NumRows; i++)
    for (int j=0; j<this->NumCols; j++)
    { tempMat.elements[i][j].GetDen(tempUI);
      if (!tempUI.IsEqualToOne())
      { outputDen*=tempUI;
        tempRat=tempUI;
        tempMat*=tempRat;
      }
    }
  for (int i=0; i<this->NumRows; i++)
    for (int j=0; j<this->NumCols; j++)
      tempMat.elements[i][j].GetNum(outputMat.elements[i][j]);
}

void MatrixLargeRational::GetMatrixIntWithDen(MatrixIntTightMemoryFit& outputMat, int& outputDen)
{ outputDen=this->FindPositiveLCMCoefficientDenominatorsTruncated();
  outputMat.init(this->NumRows, this->NumCols);
  for (int i=0; i<this->NumRows; i++)
    for (int j=0; j<this->NumCols; j++)
      outputMat.elements[i][j]=(this->elements[i][j]*outputDen).NumShort;
}

void QPSub::MakeSubFromMatrixRational(MatrixLargeRational& input)
{ input.GetMatrixIntWithDen(this->TheQNSub, this->QNSubDen);
  this->MakeSubFromMatrixIntAndDen(this->TheQNSub, this->QNSubDen);
}

void GeneralizedVermaModuleCharacters::GetSubFromIndex(QPSub& output, int theIndex)
{ MatrixLargeRational& theOperator=this->theLinearOperators.TheObjects[theIndex];
  int dimLargerAlgebra=theOperator.NumCols;
  int dimSmallerAlgebra=theOperator.NumRows;
  root& theTranslation=this->theTranslations.TheObjects[theIndex];
  MatrixLargeRational tempMat;
  tempMat.init(dimLargerAlgebra+dimSmallerAlgebra+1, dimSmallerAlgebra);
  tempMat.NullifyAll();
  for (int j=0; j<dimSmallerAlgebra; j++)
  { tempMat.elements[j][j]=1;
    for (int i=0; i<dimLargerAlgebra; i++)
      tempMat.elements[i][j]-=theOperator.elements[j][i];
    tempMat.elements[dimLargerAlgebra+dimSmallerAlgebra][j]=-theTranslation.TheObjects[j];
  }
  output.MakeSubFromMatrixRational(tempMat);
}

std::string QPSub::ElementToString()
{ std::stringstream out;
  out << "Matrix den:" << this->QNSubDen << " and the matrix:\n";
  for (int i=0; i<this->TheQNSub.NumRows; i++)
  { for (int j=0; j<this->TheQNSub.NumCols; j++)
      out << this->TheQNSub.elements[i][j] << "\t";
    out << "\n";
  }
  for (int i=0; i<this->RationalPolyForm.size; i++)
    out << "x_" << i+1 << "->" << this->RationalPolyForm.TheObjects[i].ElementToString() << ", ";
  return out.str();
}

void GeneralizedVermaModuleCharacters::ComputeQPsFromChamberComplex
(GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  PolynomialOutputFormat theFormat;
  root tempRoot;
  QPSub theSub;
  CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent
  (this->theMultiplicitiesMaxOutputReport2, "/home/todor/math/vectorpartition/trunk/ExtremaPolys.txt", false, true, false);
  this->thePfs.initFromRoots(this->GmodKnegativeWeights, theGlobalVariables);
  this->thePfs.ComputeDebugString(theGlobalVariables);
  out << this->thePfs.DebugString;
  this->thePfs.split(theGlobalVariables, 0);
  this->thePfs.ComputeDebugString(theGlobalVariables);
  out << "=" << this->thePfs.DebugString;
  int totalDim=this->theTranslations.TheObjects[0].size+this->theTranslationsProjected.TheObjects[0].size;
  this->theQPsSubstituted.SetSize(this->projectivizedChamber.size);
  this->theMultiplicities.SetSize(this->projectivizedChamber.size);
  this->thePfs.theChambers.init();
  this->thePfs.theChambers.theDirections=this->GmodKnegativeWeights;
  this->thePfs.theChambers.SliceTheEuclideanSpace(theGlobalVariables, false);
  this->theQPsNonSubstituted.SetSize(this->thePfs.theChambers.size);
  out << "\n\nThe vector partition functions in each chamber follow.";
  for (int i=0; i<this->thePfs.theChambers.size; i++)
    if (this->thePfs.theChambers.TheObjects[i]!=0)
    { QuasiPolynomial& currentQPNoSub= this->theQPsNonSubstituted.TheObjects[i];
      this->thePfs.GetVectorPartitionFunction(currentQPNoSub, this->thePfs.theChambers.TheObjects[i]->InternalPoint, theGlobalVariables);
      out << "\nChamber " << i+1 << " with internal point " << this->thePfs.theChambers.TheObjects[i]->InternalPoint.ElementToString() << " the quasipoly is: " << currentQPNoSub.ElementToString(false, false);
    }
  QuasiPolynomial theQPNoSub;
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=false;
  Lattice theExtendedIntegralLatticeMatForm;
//  this->theParser.theHmm.theRange.theWeyl.GetIntegralLatticeInSimpleCoordinates(integralLattice);
  //out << "\nThe integral lattice:\n" << integralLattice.ElementToString(false, false);
  //this->theMultiplicitiesMaxOutputReport2.flush();
  theExtendedIntegralLatticeMatForm.MakeZn(totalDim);
  for (int i=0; i<this->projectivizedChamber.size; i++)
  { this->theQPsSubstituted.TheObjects[i].SetSize(this->theLinearOperators.size);
    for (int k=0; k<this->theLinearOperators.size; k++)
    { QuasiPolynomial& currentQPSub=this->theQPsSubstituted.TheObjects[i].TheObjects[k];
      this->GetProjection(k, this->projectivizedChamber.TheObjects[i].GetInternalPoint(), tempRoot);
      tempRoot-=this->NonIntegralOriginModification;
      int theIndex= this->thePfs.theChambers.GetFirstChamberIndexContainingPoint(tempRoot);
      if (theIndex==-1)
        theQPNoSub.MakeZeroLatticeZn(tempRoot.size);
      else
        theQPNoSub=this->theQPsNonSubstituted.TheObjects[theIndex];
      std::stringstream tempStream;
      tempStream << "chamber " << i+1 << " linear opeator " << k+1;
      theGlobalVariables.theIndicatorVariables.ProgressReportString1= tempStream.str();
      theGlobalVariables.MakeReport();
      out << "\nChamber " << i << " translation " << k << ": " << theQPNoSub.ElementToString(false, false);
      this->theMultiplicitiesMaxOutputReport2.flush();
      theQPNoSub.Substitution(this->theLinearOperatorsExtended.TheObjects[k], this->theTranslationsProjected.TheObjects[k], theExtendedIntegralLatticeMatForm, currentQPSub, theGlobalVariables);
      out << "; after substitution we get: " << currentQPSub.ElementToString(false, false);
      out << "\nthe sub is: " << theSub.ElementToString();
    }
  }
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  QuasiPolynomial tempQP;
  for (int i=0; i<this->projectivizedChamber.size; i++)
  { QuasiPolynomial& currentSum=this->theMultiplicities.TheObjects[i];
    currentSum.MakeZeroLatticeZn(totalDim);
    for (int k=0; k<this->theLinearOperators.size; k++)
    { tempQP=this->theQPsSubstituted.TheObjects[i].TheObjects[k];
      tempQP*=this->theCoeffs.TheObjects[k];
      currentSum+=tempQP;
    }
    out << "\nChamber " << i+1 << ": the quasipolynomial is: " << currentSum.ElementToString(false, false);
    out << "\nThe chamber is: " << this->projectivizedChamber.TheObjects[i].ElementToString();
  }
//  this->projectivizedChamber.ComputeDebugString();
//  out << "\n\n" << this->projectivizedChamber.DebugString;
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
  theGlobalVariables.MakeReport();
  this->theMultiplicitiesMaxOutputReport2 << out.str();
}

void ComputationSetup::ComputeGenVermaCharaterG2inB3(ComputationSetup& inputData, GlobalVariables& theGlobalVariables)
{ tempCharsEraseWillBeErasedShouldntHaveLocalObjectsLikeThis.thePauseController.InitComputation();
  tempCharsEraseWillBeErasedShouldntHaveLocalObjectsLikeThis.IncrementComputation(theGlobalVariables);
  tempCharsEraseWillBeErasedShouldntHaveLocalObjectsLikeThis.thePauseController.ExitComputation();
}

void GeneralizedVermaModuleCharacters::ReadFromDefaultFile(GlobalVariables& theGlobalVariables)
{ std::fstream input;
  std::string tempS;
  CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(input, "/home/todor/math/vectorpartition/trunk/GenVermaComputation.txt", false, false, false);
  int candidatePhase;
  input >> tempS >> candidatePhase;
  if (tempS=="ComputationPhase:")
  { this->computationPhase=candidatePhase;
    this->ReadFromFileNoComputationPhase(input, theGlobalVariables);
  } else
    this->computationPhase=0;
  input.close();
}

void GeneralizedVermaModuleCharacters::WriteToDefaultFile(GlobalVariables& theGlobalVariables)
{ std::fstream output;
  CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(output, "/home/todor/math/vectorpartition/trunk/GenVermaComputation.txt", false, true, false);
  this->WriteToFile(output, theGlobalVariables);
}

void GeneralizedVermaModuleCharacters::IncrementComputation(GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  this->ReadFromDefaultFile(theGlobalVariables);
  switch (this->computationPhase)
  { case 0:
      this->theParser.theHmm.MakeG2InB3(this->theParser, theGlobalVariables);
      this->initFromHomomorphism(this->theParser.theHmm, theGlobalVariables);
      this->TransformToWeylProjectiveStep1(theGlobalVariables);
      out << theGlobalVariables.theIndicatorVariables.StatusString1;
      break;
    case 1:
      this->TransformToWeylProjectiveStep2(theGlobalVariables);
      out << theGlobalVariables.theIndicatorVariables.StatusString1;
      break;
    case 2:
      this->projectivizedChamber.Refine(theGlobalVariables);
      out << this->projectivizedChamber.ElementToString(false, false);
//      out << theGlobalVariables.theIndicatorVariables.StatusString1;
      break;
    case 3:
      this->ComputeQPsFromChamberComplex(theGlobalVariables);
      break;
    case 4:
      this->FindMultiplicitiesFree(theGlobalVariables);
      out << this->projectivezedChambersSplitByMultFreeWalls.ElementToString(false, false);
      break;
    case 5:
//      this->FindMultiplicitiesExtremaStep1(theGlobalVariables);
      break;
    case 6:
//      this->FindMultiplicitiesExtremaStep2(theGlobalVariables);
      break;
    case 7:
 //     this->FindMultiplicitiesExtremaStep3(theGlobalVariables);
      break;
    case 8:
//      this->FindMultiplicitiesExtremaStep4(theGlobalVariables);
      break;
    case 9:
//      this->FindMultiplicitiesExtremaStep5(theGlobalVariables);
      break;
    default:
      break;
  }
  this->computationPhase++;
  theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
  if (this->computationPhase>8)
  { theGlobalVariables.theIndicatorVariables.StatusString1=this->PrepareReport(theGlobalVariables);
  }
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.MakeReport();
  if (this->computationPhase < 30)
    this->WriteToDefaultFile(theGlobalVariables);
}

void CombinatorialChamberContainer::initCharacterComputation
  (WeylGroup& inputWeyl, roots& inputWeights, GlobalVariables& theGlobalVariables)
{ this->flagSliceWithAWallInitDone=true;
  this->AmbientWeyl.GetElement()=inputWeyl;
  this->theDirections=inputWeights;
  this->init();
  this->SliceTheEuclideanSpace(theGlobalVariables, false);
  this->drawOutput(theGlobalVariables.theDrawingVariables, this->IndicatorRoot, 0);
  WeylGroup& theWeyl= this->AmbientWeyl.GetElement();
  MatrixLargeRational tempMat;
  tempMat=theWeyl.CartanSymmetric;
  tempMat.Invert(theGlobalVariables);
  roots tempRoots;
  tempRoots.AssignMatrixRows(tempMat);
  this->WeylChamber.ComputeFromDirections(tempRoots, theGlobalVariables, theWeyl.GetDim());
  this->TransformToWeylProjective(theGlobalVariables);
  this->NumAffineHyperplanesProcessed=-1;
}

void WallData::TransformToWeylProjective
  (GlobalVariables& theGlobalVariables)
{ root tempRoot;
  int startingDimension=this->normal.size;
  tempRoot.MakeZero(startingDimension*2+1);
  for (int i=0; i<startingDimension; i++)
  { tempRoot.TheObjects[i]=this->normal.TheObjects[i];
    tempRoot.TheObjects[i+startingDimension]= -this->normal.TheObjects[i];
  }
  this->normal=tempRoot;
}

void CombinatorialChamberContainer::GetWeylChamberWallsForCharacterComputation(roots& output)
{ output.SetSize(this->WeylChamber.size+1);
  int startingDimension=(this->AmbientDimension-1)/2;
  for (int i=0; i<this->WeylChamber.size; i++)
  { output.TheObjects[i].MakeZero(this->AmbientDimension);
    for(int j=0; j<startingDimension; j++)
      output.TheObjects[i].TheObjects[startingDimension+j]=this->WeylChamber.TheObjects[i].TheObjects[j];
    output.TheObjects[i].ComputeDebugString();
  }
  output.LastObject()->MakeZero(this->AmbientDimension);
  *output.LastObject()->LastObject()=1;
}

void CombinatorialChamber::TransformToWeylProjective
  (CombinatorialChamberContainer& owner, GlobalVariables& theGlobalVariables)
{ for (int i=0; i<this->Externalwalls.size; i++)
    this->Externalwalls.TheObjects[i].TransformToWeylProjective(theGlobalVariables);
  WallData newWall;
  this->Externalwalls.MakeActualSizeAtLeastExpandOnTop(owner.WeylChamber.size+this->Externalwalls.size);
  roots newExternalWalls;
  owner.GetWeylChamberWallsForCharacterComputation(newExternalWalls);
  for (int i=0; i<newExternalWalls.size; i++)
  { newWall.normal=newExternalWalls.TheObjects[i];
    this->Externalwalls.AddObjectOnTop(newWall);
  }
  this->AllVertices.size=0;
  this->ComputeVerticesFromNormals(owner, theGlobalVariables);
}

void CombinatorialChamberContainer::TransformToWeylProjective
  (GlobalVariables& theGlobalVariables)
{ this->AmbientWeyl.GetElement().ComputeWeylGroup();
  this->log << this->AmbientWeyl.GetElement().ElementToString();
  std::string tempS;
  this->ElementToString(tempS);
  this->log << "\nWeyl chamber: " << this->WeylChamber.ElementToString() << "\n";
  this->log << tempS;
  this->NewHyperplanesToSliceWith.size=0;
  this->theHyperplanes.size=0;
  root wallToSliceWith;
//  roots oldDirections;
  for (int k=0; k<this->AmbientWeyl.GetElement().size; k++)
    for (int i=0; i<this->size; i++)
      if (this->TheObjects[i]!=0)
        for (int j=0; j<this->TheObjects[i]->Externalwalls.size; j++)
        { this->GetAffineWallImage(k, this->TheObjects[i]->Externalwalls.TheObjects[j], wallToSliceWith, theGlobalVariables);
//          if (k==0)
//            oldDirections.AddOnBottomNoRepetition(wallToSliceWith);
          wallToSliceWith.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
          if (k>0)
            this->NewHyperplanesToSliceWith.AddOnTopNoRepetition(wallToSliceWith);
          this->theHyperplanes.AddObjectOnTopNoRepetitionOfObjectHash(wallToSliceWith);
        }
  this->log << "\n Affine walls to slice with:";
  for (int i=0; i<this->NewHyperplanesToSliceWith.size; i++)
    this->log << "\n" << this->NewHyperplanesToSliceWith.TheObjects[i].ElementToString();
  this->log << "\n";
  this->AmbientDimension=this->AmbientDimension*2+1;
  for (int i=0; i<this->size; i++)
    if (this->TheObjects[i]!=0)
    { this->TheObjects[i]->TransformToWeylProjective(*this, theGlobalVariables);
//      theVertices.AddRootSnoRepetition(this->TheObjects[i]->AllVertices);
    }
  //this->startingCones.initFromDirections()
  root tempRoot;
  for (int i=0; i<this->TheGlobalConeNormals.size; i++)
  { tempRoot.MakeZero(this->AmbientDimension);
    int startingDim=this->TheGlobalConeNormals.TheObjects[i].size;
    for (int j=0; j<startingDim; j++)
    { tempRoot.TheObjects[j]=this->TheGlobalConeNormals.TheObjects[i].TheObjects[j];
      tempRoot.TheObjects[j+startingDim]=-tempRoot.TheObjects[j];
    }
    this->TheGlobalConeNormals.TheObjects[i]=tempRoot;
  }
  roots tempRoots;
  this->GetWeylChamberWallsForCharacterComputation(tempRoots);
  this->TheGlobalConeNormals.AddListOnTop(tempRoots);
  this->log << "the global cone normals: " << this->TheGlobalConeNormals.ElementToString();
  this->ElementToString(tempS);
  this->log << tempS;
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.theIndicatorVariables.StatusString1=this->log.str();
  theGlobalVariables.MakeReport();
}

std::string roots::ElementsToInequalitiesString(bool useLatex, bool useHtml, bool LastVarIsConstant)
{ std::stringstream out;
  std::string tempS;
  std::string theLetter="x";
  if (useLatex)
    out << "\\begin{array}{l}";
  for (int i=0; i<this->size; i++)
  { root& current=this->TheObjects[i];
    tempS=  current.ElementToStringLetterFormat(theLetter, useLatex, LastVarIsConstant);
    if (!LastVarIsConstant || tempS!="")
    { out << tempS;
      if (!LastVarIsConstant)
      { if (useLatex)
          out << "\\geq 0\\\\";
        else
          out << "=>0\n";
      } else
      { if (useLatex)
          out << "\\geq " << (-(*current.LastObject())).ElementToString() << "\\\\";
        else
          out << "=>" <<  (-(*current.LastObject())).ElementToString();
      }
      if (useHtml)
        out << "<br>";
      else
        out << "\n";
    }
  }
  if (useLatex)
    out << "\\end{array}";
  return out.str();
}

void CombinatorialChamber::ElementToInequalitiesString(std::string& output, CombinatorialChamberContainer& owner, bool useLatex, bool useHtml, PolynomialOutputFormat& PolyFormatLocal)
{ int theDimension=owner.AmbientDimension;
  this->SortNormals();
  this->AllVertices.QuickSortAscending();
  std::string tempS; std::stringstream out;
  if (useLatex)
    out << "\n\\begin{eqnarray*}\n";
  if (useHtml)
    out << "\n<br>\n";
  for (int i=0; i<this->Externalwalls.size; i++)
  { root& currentNormal= this->ExternalwallsNormalsSorted.TheObjects[i];
    for (int j=0; j<theDimension; j++)
    { currentNormal.TheObjects[j].ElementToString(tempS);
      if (tempS!="0")
      { if (tempS=="1")
        { tempS="";
          out << '+';
        }
        if (tempS=="-1")
          tempS="-";
        if (j!=0)
          if (tempS.size()!=0)
            if (tempS[0]!='-')
              out << '+';
        out << tempS << PolyFormatLocal.GetLetterIndex(j);
      }
    }
    if (this->Externalwalls.TheObjects[i].flagIsClosed)
    { if (useLatex)
        out << "& \\geq & 0\\\\";
      if (useHtml)
        out << " >= 0\n<br>\n";
      if (!(useHtml || useLatex))
        out << ">=0\n";
    } else
    { if (useLatex)
        out << "& > & 0\\\\";
      if (useHtml)
        out << " > 0\n<br>\n";
      if (!(useHtml || useLatex))
        out << ">0\n";
    }
  }
  if (useLatex)
    out << "\\end{eqnarray*}";
  output=out.str();
}

bool CombinatorialChamberContainer::oneStepChamberSlice(GlobalVariables& theGlobalVariables)
{ if (this->PreferredNextChambers.size==0 && this->NumAffineHyperplanesProcessed<this->NewHyperplanesToSliceWith.size)
  this->NumAffineHyperplanesProcessed++;
  if (!(this->NumAffineHyperplanesProcessed < this->NewHyperplanesToSliceWith.size))
  { this->flagDrawingProjective=false;
//    this->ProjectToDefaultAffineSpace(theGlobalVariables);
    std::stringstream out;
    this->ComputeDebugString();
    out << this->DebugString;
    theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
    return false;
  }
  if (this->PreferredNextChambers.size==0)
    this->SliceWithAWallInitSimple(this->NewHyperplanesToSliceWith.TheObjects[this->NumAffineHyperplanesProcessed], theGlobalVariables);
  else
    this->SliceWithAWallOneIncrement(this->NewHyperplanesToSliceWith.TheObjects[this->NumAffineHyperplanesProcessed], theGlobalVariables);
  std::stringstream out;
  //this->ComputeDebugString();
  out << "Next chamber to slice: " << this->indexNextChamberToSlice;
  out << "Preferred next chambers:  ";
  for (int i=0; i<this->PreferredNextChambers.size; i++)
    out << this->PreferredNextChambers.TheObjects[i] << ",";
  out << "\nProcessed hyperplanes: " << this->NumAffineHyperplanesProcessed  << " out of " << this->NewHyperplanesToSliceWith.size << "\n";
  out << "Number of chamber pointers (including zero pointers): " << this->size;
  //out << this->DebugString;
  theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
  theGlobalVariables.MakeReport();
  return true;
}

void CombinatorialChamberContainer::GetAffineWallImage
  (int indexWeylElement, WallData& input, root& output, GlobalVariables& theGlobalVariables)
{ WeylGroup& currentWeyl=this->AmbientWeyl.GetElement();
  Vector<PolynomialRationalCoeff> tempRoot;
  PolynomialRationalCoeff tempP, PZero;
  PZero.Nullify(this->AmbientDimension);
  tempRoot.SetSize(this->AmbientDimension);
  for (int i=0; i<this->AmbientDimension; i++)
  { tempP.MakeMonomialOneLetter(this->AmbientDimension, i, 1, (Rational) 1);
    tempRoot.TheObjects[i]=tempP;
  }
  this->log << "\nConverting: " << input.normal.ElementToString() << ". Action of element " << indexWeylElement << " on " << tempRoot.ElementToString();
  currentWeyl.ActOn(indexWeylElement, tempRoot, true, true, PZero);
  this->log << ": " << tempRoot.ElementToString();
  output.MakeZero(this->AmbientDimension*2+1);
  for (int i=0; i<this->AmbientDimension; i++)
  { output.TheObjects[i]=input.normal.TheObjects[i];
    PolynomialRationalCoeff& tempP=tempRoot.TheObjects[i];
    for (int j=0; j< tempP.size; j++)
    { Monomial<Rational>& tempMon=tempP.TheObjects[j];
      int theVarIndex;
      if (tempMon.IsAConstant())
        (*output.LastObject())+=-tempMon.Coefficient*output.TheObjects[i];
      else
      { if (!tempMon.IsOneLetterFirstDegree(theVarIndex))
          assert(false);
        output.TheObjects[theVarIndex+this->AmbientDimension]+=-tempMon.Coefficient*output.TheObjects[i];
      }
    }
  }
}

int ParserNode::EvaluateWeylAction
  (ParserNode& theNode,
   List<int>& theArgumentList, GlobalVariables& theGlobalVariables,
   bool DualAction, bool useRho, bool useMinusRho)
{ Vector<RationalFunction> theWeight;
  WeylGroup& theWeyl=theNode.owner->theHmm.theRange.theWeyl;
  if (theArgumentList.size!=theWeyl.GetDim())
    return theNode.SetError(theNode.errorDimensionProblem);
  int theDim=theArgumentList.size;
  int numVars=MathRoutines::Maximum(theDim, theNode.owner->NumVariables);
  theWeight.SetSize(theArgumentList.size);
  for (int i=0; i<theArgumentList.size; i++)
  { theWeight.TheObjects[i]=theNode.owner->TheObjects[theArgumentList.TheObjects[i]].ratFunction.GetElement();
    theWeight.TheObjects[i].SetNumVariablesSubDeletedVarsByOne(numVars);
  }
  std::stringstream out;
  theWeyl.ComputeWeylGroup(51840);
//  std::cout << theWeight.ElementToString();
  if (theWeyl.size>=51840)
    out << "Only the first 51840 elements have been computed. <br> If you want a larger computation <br> please use the C++ code directly.";
  out << "Number of elements: " << theWeyl.size << "<br>";
  Vector<RationalFunction> theOrbitElement;
  RationalFunction RFZero;
  RFZero.Nullify(numVars, &theGlobalVariables);
  for (int i=0; i<theWeyl.size; i++)
  { theOrbitElement=theWeight;
    if (!DualAction)
      theWeyl.ActOn<RationalFunction>(i, theOrbitElement, useRho, useMinusRho, RFZero);
    else
    {
    }
    out << theOrbitElement.ElementToString() << "<br>";
  }
  theNode.outputString=out.str();
  theNode.ExpressionType=theNode.typeString;
  return theNode.errorNoError;
}

void CombinatorialChamberContainer::SliceWithAWallInitSimple(root& TheKillerFacetNormal, GlobalVariables& theGlobalVariables)
{ this->flagMakingASingleHyperplaneSlice=true;
  this->flagSliceWithAWallInitDone=false;
  this->PurgeZeroPointers();
  this->PreferredNextChambers.size=0;
  this->LabelAllUnexplored();
  this->PreferredNextChambers.size=0;
  for (int i=0; i<this->size; i++)
    if (this->TheObjects[i]!=0)
      this->PreferredNextChambers.AddObjectOnTop(i);
  this->indexNextChamberToSlice=*this->PreferredNextChambers.LastObject();
  this->PreferredNextChambers.PopLastObject();
  this->flagSliceWithAWallInitDone=true;
}

void CombinatorialChamberContainer::SliceWithAWallInit(root& TheKillerFacetNormal, GlobalVariables& theGlobalVariables)
{ this->flagMakingASingleHyperplaneSlice=true;
  this->flagSliceWithAWallInitDone=false;
  this->PurgeZeroPointers();
  this->PreferredNextChambers.size=0;
  this->LabelAllUnexplored();
  root tempRoot; tempRoot.MakeZero(this->AmbientDimension);
  if (this->flagAnErrorHasOcurredTimeToPanic)
    TheKillerFacetNormal.ComputeDebugString();
  for (int i=0; i<this->size; i++)
  { if (this->flagAnErrorHasOcurredTimeToPanic)
      this->ComputeDebugString();
    if (this->TheObjects[i]!=0)
    { this->indexNextChamberToSlice=i;
      if (this->TheObjects[i]->SplitChamber(TheKillerFacetNormal, *this, tempRoot, theGlobalVariables))
      { delete this->TheObjects[i];
#ifdef CGIversionLimitRAMuse
  ParallelComputing::GlobalPointerCounter--;
  ParallelComputing::CheckPointerCounters();
#endif
        this->TheObjects[i]=0;
        break;
      }
    }
  }
  this->flagSliceWithAWallInitDone=true;
}

void CombinatorialChamberContainer::SliceWithAWall(root& TheKillerFacetNormal, GlobalVariables& theGlobalVariables)
{ this->SliceWithAWallInitSimple(TheKillerFacetNormal, theGlobalVariables);
  while (this->PreferredNextChambers.size>0)
    this->SliceWithAWallOneIncrement(TheKillerFacetNormal, theGlobalVariables);
  this->PurgeZeroPointers();
  this->PurgeInternalWalls();
}

void CombinatorialChamberContainer::SliceWithAWallOneIncrement(root& TheKillerFacetNormal, GlobalVariables& theGlobalVariables)
{ root tempRoot; tempRoot.MakeZero(this->AmbientDimension);
  if (!this->flagSliceWithAWallInitDone)
    this->SliceWithAWallInitSimple(TheKillerFacetNormal, theGlobalVariables);
  else
    if(this->PreferredNextChambers.size>0)
    { this->indexNextChamberToSlice=this->PreferredNextChambers.TheObjects[0];
      if (this->TheObjects[this->PreferredNextChambers.TheObjects[0]]!=0)
        if (!this->TheObjects[this->PreferredNextChambers.TheObjects[0]]->flagExplored)
          if (this->TheObjects[this->PreferredNextChambers.TheObjects[0]]->SplitChamber(TheKillerFacetNormal, *this, tempRoot, theGlobalVariables))
          { delete this->TheObjects[this->PreferredNextChambers.TheObjects[0]];
  #ifdef CGIversionLimitRAMuse
    ParallelComputing::GlobalPointerCounter--;
    ParallelComputing::CheckPointerCounters();
  #endif
            this->TheObjects[this->PreferredNextChambers.TheObjects[0]]=0;
          }
      this->PreferredNextChambers.PopIndexShiftUp(0);
      if (this->flagAnErrorHasOcurredTimeToPanic)
        this->ComputeDebugString();
    }
}

void GeneralizedVermaModuleCharacters::GetSubFromNonParamArray
(MatrixLargeRational& output, root& outputTranslation, roots& NonParams, int numParams)
{ //reminder: the very last variable comes from the projectivization and contributes to the translation only!
  int numNonParams=NonParams.size;
  output.init(numParams+numNonParams-1, numParams-1);
  outputTranslation.MakeZero(numParams+numNonParams-1);
  output.NullifyAll();
  for (int l=0; l<numNonParams; l++)
  { for (int k=0; k<numParams-1; k++)
      output.elements[l][k]= NonParams.TheObjects[l].TheObjects[k];
    outputTranslation.TheObjects[l]=*NonParams.TheObjects[l].LastObject();
  }
  for (int l=0; l<numParams-1; l++)
    output.elements[l+numNonParams][l]= 1;
}

void GeneralizedVermaModuleCharacters::FindMultiplicitiesExtremaStep1(GlobalVariables& theGlobalVariables)
{ int theDimension=6;
  if (this->projectivizedChamber.size>0)
    theDimension=this->projectivizedChamber.TheObjects[0].Normals.TheObjects[0].size;
//  this->ExtremeQPsParamSubchambers.SetSize(this->projectivizedChamber.size);
  std::stringstream out;
  std::fstream MultReport;
  CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(MultReport, "/home/todor/math/vectorpartition/trunk/ReportFindMultiplicitiesExtremaStep1.txt", false, true, false);
  MatrixLargeRational subForFindingExtrema;
  root translationForFindingExtrema;
  QuasiPolynomial currentExtremaCandidate;
//  Lattice AmbientLattice;
  int numParams=0, numNonParams=0;
  if (this->theLinearOperators.size>0)
  { numParams=this->theLinearOperators.TheObjects[0].NumCols+1;
    numNonParams=this->theLinearOperators.TheObjects[0].NumRows;
  }
//  AmbientLattice.MakeZn(numParams);
  List<roots> currentParamChamberList, currentNonParamVerticesList;
  this->theMultiplicitiesExtremaCandidates.MakeActualSizeAtLeastExpandOnTop(this->projectivizedChamber.size*theDimension);
  this->allParamSubChambersRepetitionsAllowedConeForm.MakeActualSizeAtLeastExpandOnTop(this->projectivizedChamber.size*theDimension);
  Cone currentCone;
  theGlobalVariables.theIndicatorVariables.String1NeedsRefresh=true;
  theGlobalVariables.theIndicatorVariables.String2NeedsRefresh=true;
  PolynomialsRationalCoeff tempSub;
  for (int i=0; i<this->projectivizedChamber.size; i++)
  { std::stringstream progressReport1;
    progressReport1 << "processing chamber " << i+1;
    theGlobalVariables.theIndicatorVariables.ProgressReportString1=progressReport1.str();
    theGlobalVariables.MakeReport();
    this->ProcessOneParametricChamber(this->theLinearOperators.TheObjects[0].NumRows, this->theLinearOperators.TheObjects[0].NumCols+1, this->projectivizedChamber.TheObjects[i].Normals, currentParamChamberList, currentNonParamVerticesList, theGlobalVariables);
    for (int j=0; j<currentParamChamberList.size; j++)
    { std::stringstream progressReport2;
      progressReport2 << "Parametric chamber candidate " << j+1 << " out of " << currentParamChamberList.size;
      if (currentCone.CreateFromNormals(currentParamChamberList.TheObjects[j], theGlobalVariables))
      { this->GetSubFromNonParamArray(subForFindingExtrema, translationForFindingExtrema, currentNonParamVerticesList.TheObjects[j], numParams);
        QuasiPolynomial& currentQP=this->theMultiplicities.TheObjects[i];
        tempSub.MakeLinearSubbedVarsCorrespondToMatRows(subForFindingExtrema, translationForFindingExtrema);
        bool tempBool = currentQP.SubstitutionLessVariables(tempSub, currentExtremaCandidate, theGlobalVariables);
        assert(tempBool);
//        currentQP.Substitution(subForFindingExtrema, translationForFindingExtrema, AmbientLattice, currentExtremaCandidate, theGlobalVariables);
        this->theMultiplicitiesExtremaCandidates.AddObjectOnTop(currentExtremaCandidate);
        this->allParamSubChambersRepetitionsAllowedConeForm.AddObjectOnTop(currentCone);
        MultReport << "Chamber " << i+1 << " parametric chamber candidate " << j+1 << " out of " << currentParamChamberList.size << "; candidate found: " << currentExtremaCandidate.ElementToString(false, false) << "\n";
        progressReport2 << " is non-trivial";
      } else
        progressReport2 << " is trivial";
      theGlobalVariables.theIndicatorVariables.ProgressReportString2=progressReport2.str();
      theGlobalVariables.MakeReport();
    }
  }
}

void GeneralizedVermaModuleCharacters::FindMultiplicitiesExtremaStep2(GlobalVariables& theGlobalVariables)
{ List<roots> tempRoots;
  tempRoots.SetSize(this->allParamSubChambersRepetitionsAllowedConeForm.size);
  for (int i=0; i<tempRoots.size; i++)
    tempRoots.TheObjects[i]=this->allParamSubChambersRepetitionsAllowedConeForm.TheObjects[i].Normals;
  this->projectivizedParamComplex.initFromCones(tempRoots, theGlobalVariables);
}

void GeneralizedVermaModuleCharacters::FindMultiplicitiesExtremaStep3(GlobalVariables& theGlobalVariables)
{ this->projectivizedParamComplex.Refine(theGlobalVariables);
  this->projectivizedExtremaCones.SetSize(this->projectivizedParamComplex.size);
  this->theExtrema.SetSize(this->projectivizedParamComplex.size);
}

void GeneralizedVermaModuleCharacters::FindMultiplicitiesExtremaStep4(GlobalVariables& theGlobalVariables)
{ this->theMultiplicitiesExtremaCandidates.SetSize(this->projectivizedParamComplex.size);
  theGlobalVariables.theIndicatorVariables.String1NeedsRefresh=true;
  CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent
  (this->theMultiplicitiesMaxOutput, "/home/todor/math/vectorpartition/trunk/ExtremaOutput.txt", true, false, false);
  for (; this->NumProcessedConesParam<this->projectivizedParamComplex.size; this->NumProcessedConesParam++)
  { Cone& currentCone= this->projectivizedParamComplex.TheObjects[this->NumProcessedConesParam];
    std::stringstream out3;
    out3 << "Processing extrema in chamber " << this->NumProcessedConesParam+1 << " out of " << this->projectivizedParamComplex.size;
    theGlobalVariables.theIndicatorVariables.ProgressReportString1=out3.str();
    theGlobalVariables.MakeReport();
    this->theMultiplicitiesMaxOutput << "\n\n\n\n\nChamber " << this->NumProcessedConesParam+ 1;
    this->ProcessExtremaOneChamber(currentCone, this->projectivizedExtremaCones.TheObjects[this->NumProcessedConesParam], this->theExtrema.TheObjects[this->NumProcessedConesParam], theGlobalVariables);
    this->thePauseController.SafePoint();
  }
  this->theMultiplicitiesMaxOutput.close();
}

void GeneralizedVermaModuleCharacters::ProcessExtremaOneChamber
  (Cone& input, List<Cone>& outputSubdivision, List<QuasiPolynomial>& theExtremaOutput, GlobalVariables& theGlobalVariables)
{ std::stringstream out3, out1, out2;
//  int projectiveDimension=input.GetDim();
  List<QuasiPolynomial> extremaCandidates;
  extremaCandidates.MakeActualSizeAtLeastExpandOnTop(allParamSubChambersRepetitionsAllowedConeForm.size);
  for (int j=0; j<this->allParamSubChambersRepetitionsAllowedConeForm.size; j++)
    if (this->allParamSubChambersRepetitionsAllowedConeForm.TheObjects[j].IsInCone(input.GetInternalPoint()))
      extremaCandidates.AddObjectOnTop(this->theMultiplicitiesExtremaCandidates.TheObjects[j]);
  out3 << "# of maximum candidates: " << extremaCandidates.size;
  theGlobalVariables.theIndicatorVariables.String2NeedsRefresh=true;
  theGlobalVariables.theIndicatorVariables.ProgressReportString2=out3.str();
  theGlobalVariables.MakeReport();
  ConeComplex extremaComplex;
  out1 << "size of required subdivision: ?";
  theGlobalVariables.theIndicatorVariables.String3NeedsRefresh=true;
  theGlobalVariables.theIndicatorVariables.ProgressReportString3=out1.str();
  theGlobalVariables.MakeReport();
  this->theMultiplicitiesMaxOutput << "\nNumber of candidates for maximum: " << extremaCandidates.size;
  for (int i=0; i<extremaCandidates.size; i++)
  { this->theMultiplicitiesMaxOutput << "\nCandidate "<< i+1 << ": " << extremaCandidates.TheObjects[i].ElementToString(false, false);
  }
  bool tempBool=  extremaComplex.findMaxQPOverConeProjective(input, extremaCandidates, theExtremaOutput, theGlobalVariables);
  if (tempBool)
  { this->theMultiplicitiesMaxOutput << "\n\nFound extrema: ";
    if (extremaComplex.size>1)
      this->theMultiplicitiesMaxOutput << "\n" << extremaComplex.size << " subchambers total";
    for (int i=0; i<extremaComplex.size; i++)
    { if (extremaComplex.size>1)
        this->theMultiplicitiesMaxOutput << "Subchamber " << i+1<< ": ";
      this->theMultiplicitiesMaxOutput << theExtremaOutput.TheObjects[i].ElementToString(false, false);
    }
  } else
  { this->theMultiplicitiesMaxOutput << "\nFailed to find extrema! Something is wrong...";
  }
  out2 << "size of required subdivision:   " << extremaComplex.size;
  theGlobalVariables.theIndicatorVariables.ProgressReportString3=out2.str();
  theGlobalVariables.MakeReport();
  outputSubdivision.CopyFromBase(extremaComplex);
}

bool ParserNode::ExtractArgumentList
(List<int>& outputArgumentIndices)
{ assert(false);
  return true;
}

int ParserNode::EvaluateSolveLNEqParamOverConeAndLattice
    (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ PolynomialRationalCoeff& theLinPoly=theNode.owner->TheObjects[theArgumentList.TheObjects[0]].polyValue.GetElement();
  int numNonParams =  theNode.owner->TheObjects[theArgumentList.TheObjects[1]].intValue;
  int numParams =  theNode.owner->TheObjects[theArgumentList.TheObjects[2]].intValue;
  Cone& currentCone=theNode.owner->TheObjects[theArgumentList.TheObjects[3]].theCone.GetElement();
  Lattice& currentLattice=theNode.owner->TheObjects[theArgumentList.TheObjects[4]].theLattice.GetElement();
  root theNEq;
  if(!theLinPoly.IsLinearGetRootConstantTermLastCoordinate(theNEq, (Rational) 0))
    return theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);
  if(theNEq.size!=currentCone.GetDim())
    return theNode.SetError(theNode.errorDimensionProblem);
  if (theNEq.size!=numNonParams+numParams+1)
    return theNode.SetError(theNode.errorBadOrNoArgument);
  GeneralizedVermaModuleCharacters tempChars;
  root tempRoot;
  List<int> nodesCurrentRoot;
  List<roots> outputParamChambers, outputNonParamVertices;
  PolynomialOutputFormat tempFormat;
  std::cout << "Input data: normal: " << theNEq.ElementToString()
  << "; numNonParams: " << numNonParams << "; numParams: " << numParams
  << "; cone: " << currentCone.ElementToString(false, true, false, true);
  ConeComplex theConeComplex;
  currentCone.SolveLNeqParamOverLattice
  (numNonParams, numParams, currentLattice, theConeComplex, theGlobalVariables);
  std::stringstream out;
  out << currentCone.ElementToString(false, true);
  theNode.outputString=out.str();
  theNode.ExpressionType=theNode.typeString;
  return theNode.errorNoError;
}


void Cone::SolveLNeqParamOverLatticeOneNonParam
    (Lattice& theLattice, root& theShift,
     List<Cone>& outputCones, List<Lattice>& outputLattices, roots outputShifts, GlobalVariables& theGlobalVariables)
{ //the first variable is non-parameter, the remaining ones are parameters. The last variable represents constants.
  /*MatrixLargeRational column, theMat;
  column.init(this->Normals.size, 1);
  root currentColumn;
  Selection NonPivotPoints;
  roots theNewNormals;
  Lattice CurrentLattice;

  for (int i=0; i<this->Normals.size; i++)
  { theMat.AssignRootsToRowsOfMatrix(this->Normals);
    theMat.SwitchTwoRows(i, 0);
    if (!theMat.elements[0][0].IsEqualToZero())
    { Rational

      for (int j=0; j<column.NumRows; j++)
        column.elements[j][0]=theMat.elements[j][0];
      column.GaussianEliminationByRows(theMat, NonPivotPoints);
      theNewNormals.SetSize(column.NumRows-1);
      for (int j=0; j<theMat.NumRows-1; j++)
      { root& currentRoot=theNewNormals.TheObjects[j+1];
        currentRoot.SetSize(theMat.NumCols-1);
        for (int k=0; k<theMat.NumCols-1; k++)
          currentRoot.TheObjects[k]=theMat.elements[j+1][k+1];
      }
    }

  }
*/
}

void Cone::SolveLNeqParamOverLattice
    (int numNonParams, int numParamsNoConstTerm, Lattice& theLattice,
     ConeComplex& output, GlobalVariables& theGlobalVariables)
{ //this->SolveLNeqParamOverLatticeOneNonParam(0, theLattice, output, theGlobalVariables);

  /*Selection selectedNormals;
  List<roots> theParamChambers, theNonParamVertices;
  selectedNormals.init(this->Normals.size);
  int numCycles=MathRoutines::NChooseK(this->Normals.size, numNonParams);
  MatrixLargeRational matrixNonParams, matrixParams;
  matrixNonParams.init(this->Normals.size, numNonParams);
  int numParamsWithConstTerm=numParamsNoConstTerm+1;
  matrixParams.init(this->Normals.size, numParamsWithConstTerm);
  Selection NonPivotPoints;
  roots basisRoots;
  basisRoots.SetSize(numNonParams);
  roots inducedParamChamber;
  roots nonParamVertices;
  inducedParamChamber.SetSize(this->Normals.size-numNonParams);
  nonParamVertices.SetSize(numNonParams);
  theParamChambers.MakeActualSizeAtLeastExpandOnTop(numCycles);
  theNonParamVertices.MakeActualSizeAtLeastExpandOnTop(numCycles);
  theParamChambers.size=0;
  theNonParamVertices.size=0;
  std::cout << "<hr>Num cycles: " << numCycles << "<br>";
  for (int i=0; i<numCycles; i++)
  { selectedNormals.incrementSelectionFixedCardinality(numNonParams);
    for (int j=0; j<numNonParams; j++)
    { root& currentNormal=this->Normals.TheObjects[selectedNormals.elements[j]];
      root& currentBasisElt=basisRoots.TheObjects[j];
      currentBasisElt.SetSize(numNonParams);
      for (int k=0; k<numNonParams; k++)
      { matrixNonParams.elements[j][k]=currentNormal.TheObjects[k];
        currentBasisElt.TheObjects[k]=currentNormal.TheObjects[k];
      }
      for (int k=0; k<numParamsWithConstTerm; k++)
        matrixParams.elements[j][k]=currentNormal.TheObjects[k+numNonParams];
    }
    if (basisRoots.GetRankOfSpanOfElements(theGlobalVariables)==numNonParams)
    { for (int j=0, counter=numNonParams; j<this->Normals.size; j++)
        if (!selectedNormals.selected[j])
        { root& currentNormal=this->Normals.TheObjects[j];
          for (int k=0; k<numNonParams; k++)
            matrixNonParams.elements[counter][k]=currentNormal.TheObjects[k];
          for (int k=0; k<numParamsWithConstTerm; k++)
            matrixParams.elements[counter][k]=currentNormal.TheObjects[k+numNonParams];
          counter++;
        }
      std::cout << "<br>Solving: " << matrixNonParams.ElementToString(true, false) << " and " << matrixParams.ElementToString(true, false);
      MatrixLargeRational::GaussianEliminationByRows(matrixNonParams, matrixParams, NonPivotPoints);
      for (int j=0; j<numNonParams; j++)
        matrixParams.RowToRoot(j, nonParamVertices.TheObjects[j]);
      for (int j=0; j<this->Normals.size-numNonParams; j++)
        matrixParams.RowToRoot(j+numNonParams, inducedParamChamber.TheObjects[j]);
      theNonParamVertices.AddObjectOnTop(nonParamVertices);
      theParamChambers.AddObjectOnTop(inducedParamChamber);
    }
  }
  for (int i=0; i< theNonParamVertices.size; i++)
  { std::cout << "<br>Number " << i+1 << ", vertices: " << theNonParamVertices.TheObjects[i].ElementToString();
    std::cout << "<br>the parametric chamber: ";
    std::cout << "<div class=\"math\">" << theParamChambers.TheObjects[i].ElementsToInequalitiesString(true, false, true) << "</div>";
  }
  output.initFromCones(theParamChambers, theGlobalVariables);
  std::cout << "<hr>Starting cone: "  << output.ElementToString(false, true);
  output.Refine(theGlobalVariables);
  std::cout << "<hr>Cone refined: " << output.ElementToString(false, true);*/
}


void GeneralizedVermaModuleCharacters::ProcessOneParametricChamber
  (int numNonParams, int numParams, roots& inputNormals, List<roots>& theParamChambers, List<roots>& theNonParamVertices, GlobalVariables& theGlobalVariables)
{ /*roots nonParametricPart, parametricPart;
  nonParametricPart.SetSize(input.Externalwalls.size);
  parametricPart.SetSize(input.Externalwalls.size);
  for (int i=0; i<input.Externalwalls.size; i++)
  { root& currentNP=nonParametricPart.TheObjects[i];
    root& currentP= parametricPart.TheObjects[i];
    currentNP.SetSize(numNonParams);
    currentP.SetSize(numParams);
    for (int j=0; j<numNonParams; j++)
      currentNP.TheObjects[j]=input.Externalwalls.TheObjects[i].normal.TheObjects[j];
    for (int j=0; j<numParams; j++)
      currentNP.TheObjects[j]=input.Externalwalls.TheObjects[i].normal.TheObjects[numNonParams+j];
  }*/
  Selection selectedNormals;
  selectedNormals.init(inputNormals.size);
  int numCycles=MathRoutines::NChooseK(inputNormals.size, numNonParams);
  MatrixLargeRational matrixNonParams, matrixParams;
  matrixNonParams.init(inputNormals.size, numNonParams);
  matrixParams.init(inputNormals.size, numParams);
  Selection NonPivotPoints;
  roots basisRoots;
  basisRoots.SetSize(numNonParams);
  roots inducedParamChamber;
  roots nonParamVertices;
  inducedParamChamber.SetSize(inputNormals.size-numNonParams);
  nonParamVertices.SetSize(numNonParams);
  theParamChambers.MakeActualSizeAtLeastExpandOnTop(numCycles);
  theNonParamVertices.MakeActualSizeAtLeastExpandOnTop(numCycles);
  theParamChambers.size=0;
  theNonParamVertices.size=0;
  for (int i=0; i<numCycles; i++)
  { selectedNormals.incrementSelectionFixedCardinality(numNonParams);
    for (int j=0; j<numNonParams; j++)
    { root& currentNormal=inputNormals.TheObjects[selectedNormals.elements[j]];
      root& currentBasisElt=basisRoots.TheObjects[j];
      currentBasisElt.SetSize(numNonParams);
      for (int k=0; k<numNonParams; k++)
      { matrixNonParams.elements[j][k]=currentNormal.TheObjects[k];
        currentBasisElt.TheObjects[k]=currentNormal.TheObjects[k];
      }
      for (int k=0; k<numParams; k++)
        matrixParams.elements[j][k]=currentNormal.TheObjects[k+numNonParams];
    }
    if (basisRoots.GetRankOfSpanOfElements(theGlobalVariables)==numNonParams)
    { for (int j=0, counter=numNonParams; j<inputNormals.size; j++)
        if (!selectedNormals.selected[j])
        { root& currentNormal=inputNormals.TheObjects[j];
          for (int k=0; k<numNonParams; k++)
            matrixNonParams.elements[counter][k]=currentNormal.TheObjects[k];
          for (int k=0; k<numParams; k++)
            matrixParams.elements[counter][k]=currentNormal.TheObjects[k+numNonParams];
          counter++;
        }
      MatrixLargeRational::GaussianEliminationByRows(matrixNonParams, matrixParams, NonPivotPoints);
      for (int j=0; j<numNonParams; j++)
        matrixParams.RowToRoot(j, nonParamVertices.TheObjects[j]);
      for (int j=0; j<inputNormals.size-numNonParams; j++)
        matrixParams.RowToRoot(j+numNonParams, inducedParamChamber.TheObjects[j]);
      theNonParamVertices.AddObjectOnTop(nonParamVertices);
      theParamChambers.AddObjectOnTop(inducedParamChamber);
    }
  }
  for (int i=0; i< theNonParamVertices.size; i++)
  { std::cout << "<br>Number " << i+1 << ", vertices: " << theNonParamVertices.TheObjects[i].ElementToString();
    std::cout << "<br>the parametric chamber: ";
    std::cout << "<div class=\"math\">" << theParamChambers.TheObjects[i].ElementsToInequalitiesString(true, false) << "</div>";
  }
}

void ConeComplex::GetNewVerticesAppend
  (Cone& myDyingCone, root& killerNormal, hashedRoots& outputVertices, GlobalVariables& theGlobalVariables)
{ int theDimMinusTwo=killerNormal.size-2;
  int theDim=killerNormal.size;
  int numCycles=MathRoutines::NChooseK(myDyingCone.Normals.size, theDimMinusTwo);
  Selection& theSel=theGlobalVariables.selComputeNormalExcludingIndex;
  Selection& nonPivotPoints=theGlobalVariables.selNonPivotPointsNewSplit;
  theSel.init(myDyingCone.Normals.size);
//  int IndexLastZeroWithOneBefore, NumOnesAfterLastZeroWithOneBefore;
  MatrixLargeRational& theLinearAlgebra=theGlobalVariables.matComputeNormalExcludingIndex;
  MatrixLargeRational matEmpty;
  theLinearAlgebra.init(theDimMinusTwo+1, theDim);
  root tempRoot;
  for (int i=0; i<numCycles; i++)
  { theSel.incrementSelectionFixedCardinality(theDimMinusTwo);//, IndexLastZeroWithOneBefore, NumOnesAfterLastZeroWithOneBefore);
    //int LowestRowUnchanged=theDimMinusTwo-2-NumOnesAfterLastZeroWithOneBefore;
    //for (int j=theDimMinusTwo-1; j>LowestRowUnchanged; j--)
    for (int j=0; j<theDimMinusTwo; j++)
    { root& currentNormal=myDyingCone.Normals.TheObjects[theSel.elements[j]];
      for (int k=0; k<theDim; k++)
        theLinearAlgebra.elements[j][k]=currentNormal.TheObjects[k];
    }
    for (int k=0; k<theDim; k++)
      theLinearAlgebra.elements[theDimMinusTwo][k]=killerNormal.TheObjects[k];
    theLinearAlgebra.GaussianEliminationByRows(theLinearAlgebra, matEmpty, nonPivotPoints);
    if (nonPivotPoints.CardinalitySelection==1)
    { theLinearAlgebra.NonPivotPointsToEigenVector(nonPivotPoints, tempRoot, (Rational) 1, (Rational) 0);
      tempRoot.ScaleToIntegralMinHeight();
      if (myDyingCone.IsInCone(tempRoot))
        outputVertices.AddObjectOnTopNoRepetitionOfObjectHash(tempRoot);
      else
      { tempRoot.MinusRoot();
        if (myDyingCone.IsInCone(tempRoot))
          outputVertices.AddObjectOnTopNoRepetitionOfObjectHash(tempRoot);
      }
    }
  }
}

bool ConeComplex::SplitChamber
(int indexChamberBeingRefined, root& killerNormal, GlobalVariables& theGlobalVariables)
{ Cone& myDyingCone=this->TheObjects[indexChamberBeingRefined];
  if (!myDyingCone.flagHasSufficientlyManyVertices)
  { this->flagChambersHaveTooFewVertices=true;
    return false;
  }
  Cone& newPlusCone= theGlobalVariables.coneBuffer1NewSplit;
  Cone& newMinusCone=theGlobalVariables.coneBuffer2NewSplit;
  newPlusCone.flagHasSufficientlyManyVertices=true;
  newMinusCone.flagHasSufficientlyManyVertices=true;
  newPlusCone.LowestIndexNotCheckedForSplitting=myDyingCone.LowestIndexNotCheckedForSplitting+1;
  newMinusCone.LowestIndexNotCheckedForSplitting=myDyingCone.LowestIndexNotCheckedForSplitting+1;
  newPlusCone.Vertices.size=0; newPlusCone.Normals.size=0;
  newMinusCone.Vertices.size=0; newMinusCone.Normals.size=0;
  hashedRoots& ZeroVertices=theGlobalVariables.hashedRootsNewChamberSplit;
  ZeroVertices.ClearTheObjects();
  Rational tempRat;
  for (int i=0; i<myDyingCone.Vertices.size; i++)
  { root::RootScalarEuclideanRoot(killerNormal, myDyingCone.Vertices.TheObjects[i], tempRat);
    if (tempRat.IsPositive())
      newPlusCone.Vertices.AddObjectOnTop(myDyingCone.Vertices.TheObjects[i]);
    if (tempRat.IsEqualToZero())
      ZeroVertices.AddObjectOnTopNoRepetitionOfObjectHash(myDyingCone.Vertices.TheObjects[i]);
    if (tempRat.IsNegative())
      newMinusCone.Vertices.AddObjectOnTop(myDyingCone.Vertices.TheObjects[i]);
  }
  if (newPlusCone.Vertices.size==0 || newMinusCone.Vertices.size==0)
    return false;
  this->GetNewVerticesAppend(myDyingCone, killerNormal, ZeroVertices, theGlobalVariables);
  for (int i=0; i<myDyingCone.Normals.size; i++)
  { if (newPlusCone.Vertices.HasAnElementPerpendicularTo(myDyingCone.Normals.TheObjects[i]))
      newPlusCone.Normals.AddObjectOnTop(myDyingCone.Normals.TheObjects[i]);
    if (newMinusCone.Vertices.HasAnElementPerpendicularTo(myDyingCone.Normals.TheObjects[i]))
      newMinusCone.Normals.AddObjectOnTop(myDyingCone.Normals.TheObjects[i]);
  }
  newPlusCone.Normals.AddObjectOnTop(killerNormal);
  newMinusCone.Normals.AddObjectOnTop(-killerNormal);
  newPlusCone.Vertices.AddListOnTop(ZeroVertices);
  newMinusCone.Vertices.AddListOnTop(ZeroVertices);
/*  Cone tempCone;
  tempCone.CreateFromNormals(newPlusCone.Normals, theGlobalVariables);
  assert(tempCone.Vertices.size==newPlusCone.Vertices.size);
  tempCone.CreateFromNormals(newMinusCone.Normals, theGlobalVariables);
  assert(tempCone.Vertices.size==newMinusCone.Vertices.size);
*/
  this->PopChamberSwapWithLast(indexChamberBeingRefined);
  this->AddNonRefinedChamberOnTopNoRepetition(newPlusCone);
  this->AddNonRefinedChamberOnTopNoRepetition(newMinusCone);
  return true;
}

void ConeComplex::PopChamberSwapWithLast(int index)
{ this->PopIndexSwapWithLastHash(index);
}

void ConeComplex::AddNonRefinedChamberOnTopNoRepetition(Cone& newCone)
{ newCone.Normals.QuickSortAscending();
  this->AddObjectOnTopNoRepetitionOfObjectHash(newCone);
}

void ConeComplex::RefineOneStep(GlobalVariables& theGlobalVariables)
{ if (this->indexLowestNonRefinedChamber>=this->size)
    return;
  Cone& currentCone=this->TheObjects[this->indexLowestNonRefinedChamber];
  for (; currentCone.LowestIndexNotCheckedForSplitting<this->splittingNormals.size; currentCone.LowestIndexNotCheckedForSplitting++)
    if (this->SplitChamber(this->indexLowestNonRefinedChamber, this->splittingNormals.TheObjects[currentCone.LowestIndexNotCheckedForSplitting], theGlobalVariables))
      return;
    this->indexLowestNonRefinedChamber++;
}

void ConeComplex::Refine(GlobalVariables& theGlobalVariables)
{ while (this->indexLowestNonRefinedChamber<this->size)
  { this->RefineOneStep(theGlobalVariables);
    std::stringstream out;
    out << "Refined " << this->indexLowestNonRefinedChamber << " out of " << this->size;
    theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
    theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
    theGlobalVariables.MakeReport();
  }
}

bool Cone::CreateFromNormals
  (roots& inputNormals, GlobalVariables& theGlobalVariables)
{ this->Normals.CopyFromBase(inputNormals);
//o  this->Data=inputData;
//  roots& candidateVertices=theGlobalVariables.rootsGeneralPurposeBuffer1;
  Selection theSel, nonPivotPoints;
  this->Vertices.size=0;
  if (this->Normals.size==0)
    return false;
  for (int i=0; i<this->Normals.size; i++)
    this->Normals.TheObjects[i].ScaleToIntegralMinHeight();
  int theDim=this->Normals.TheObjects[0].size;
  theSel.init(this->Normals.size);
  int numCycles=theSel.GetNumCombinationsFixedCardinality(theDim-1);
  MatrixLargeRational& theMat=theGlobalVariables.matComputeNormalFromSelection;
  MatrixLargeRational emptyMat;
  root tempRoot;
  theMat.init(theDim-1, theDim);
  for (int i=0; i<numCycles; i++)
  { theSel.incrementSelectionFixedCardinality(theDim-1);
    for (int j=0; j<theSel.CardinalitySelection; j++)
      for (int k=0; k<theDim; k++)
        theMat.elements[j][k]=this->Normals.TheObjects[theSel.elements[j]].TheObjects[k];
    theMat.GaussianEliminationByRows(theMat, emptyMat, nonPivotPoints);
    if (nonPivotPoints.CardinalitySelection==1)
    { theMat.NonPivotPointsToRoot(nonPivotPoints, tempRoot);
      bool tempBool=this->IsInCone(tempRoot);
      if (!tempBool)
      { tempRoot.MinusRoot();
        tempBool=this->IsInCone(tempRoot);
      }
      if (tempBool)
      { tempRoot.ScaleToIntegralMinHeight();
        this->Vertices.AddOnTopNoRepetition(tempRoot);
      }
    }
  }
  //time to eliminate possible fake walls
  roots& verticesOnWall=theGlobalVariables.rootsGeneralPurposeBuffer2;
  for (int i=0; i<this->Normals.size; i++)
  { root& currentNormal=this->Normals.TheObjects[i];
    verticesOnWall.size=0;
    bool wallIsGood=false;
    for (int j=0; j<this->Vertices.size; j++)
      if (root::RootScalarEuclideanRoot(this->Vertices.TheObjects[j], currentNormal).IsEqualToZero())
      { verticesOnWall.AddObjectOnTop(this->Vertices.TheObjects[j]);
        int theRank=verticesOnWall.GetRankOfSpanOfElements(theGlobalVariables);
        if (theRank< verticesOnWall.size)
          verticesOnWall.PopLastObject();
        else
          if (theRank==theDim-1)
          { wallIsGood=true;
            break;
          }
      }
    if (!wallIsGood)
    { this->Normals.PopIndexSwapWithLast(i);
      i--;
    }
  }
  if (this->Normals.size==0 || this->Vertices.size==0)
    return false;
  this->Normals.QuickSortAscending();
  //this->ComputeDebugString();
  for (int i=0; i<this->Normals.size; i++)
    if (!(this->Vertices.HasAnElementWithNegativeScalarProduct(this->Normals.TheObjects[i])||this->Vertices.HasAnElementWithPositiveScalarProduct(this->Normals.TheObjects[i])))
      return false;
  return true;
}

void ConeComplex::initFromCones
(List<roots>& NormalsOfCones, GlobalVariables& theGlobalVariables)
{ Cone tempCone;
  this->ClearTheObjects();
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.theIndicatorVariables.StatusString1=NormalsOfCones.ElementToStringGeneric();
  theGlobalVariables.MakeReport();
  theGlobalVariables.MakeReport();
//  for (int i=0; i<10000000; i++){int j=i*i*i;}
  for (int i=0; i<NormalsOfCones.size; i++)
  { if (tempCone.CreateFromNormals(NormalsOfCones.TheObjects[i], theGlobalVariables))
      this->AddNonRefinedChamberOnTopNoRepetition(tempCone);
    std::stringstream out;
    out << "Initializing cone " << i+1 << " out of " << NormalsOfCones.size;
    theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
    theGlobalVariables.MakeReport();
  }
  root tempRoot;
  this->splittingNormals.ClearTheObjects();
  for (int i=0; i<this->size; i++)
    for (int j=0; j<this->TheObjects[i].Normals.size; j++)
    { tempRoot=this->TheObjects[i].Normals.TheObjects[j];
      tempRoot.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
      this->splittingNormals.AddObjectOnTopNoRepetitionOfObjectHash(tempRoot);
      std::stringstream out;
      out << "Extracting walls from cone " << i+1 << " out of " << this->size << " total distinct chambers.";
      out << "\nProcessed " << j+1 << " out of " << this->TheObjects[i].Normals.size << " walls of the current chamber.";
      out << "\nTotal # of distinct walls found: " << this->splittingNormals.size;
      theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
      theGlobalVariables.MakeReport();
    }
}

std::string Cone::ElementToString(bool useLatex, bool useHtml, bool PrepareMathReport, bool lastVarIsConstant)
{ std::stringstream out;
  if (!PrepareMathReport)
    out << "Index next wall to refine by: " << this->LowestIndexNotCheckedForSplitting << "\n";
  if (useHtml)
    out << "<br>";
  out << "Normals:\n";
  if (useHtml)
    out << "<br>";
  if (useLatex)
    out << "\\[";
  out << this->Normals.ElementsToInequalitiesString(useLatex, useHtml, lastVarIsConstant);
  if (useLatex)
    out << "\\]";
  out << "\nVertices: " << this->Vertices.ElementToString();
  if (useHtml)
    out << "<br>";

  out << "\nInternal point: " << this->GetInternalPoint().ElementToString();
  return out.str();
}

std::string ConeComplex::ElementToString(bool useLatex, bool useHtml)
{ std::stringstream out;
  out << "Number of chambers: " << this->size;
  if (useHtml)
    out << "<br>";
  out  << " Next non-refined chamber: " << this->indexLowestNonRefinedChamber+1;
  if (useHtml)
    out << "<br>";
  out << "Normals of walls to refine by: ";
  roots tempRoots;
  tempRoots.CopyFromBase(this->splittingNormals);
  out << tempRoots.ElementToString(useLatex, useHtml, false);
  for (int i=0; i<this->size; i++)
  { if (useHtml)
      out << "<br>";
    out << "\nChamber " << i+1 << ":\n";
    if (useHtml)
      out << "<br>";
    out << this->TheObjects[i].ElementToString(useLatex, useHtml) << "\n\n\n";
  }
  return out.str();
}

int ParserNode::EvaluateGroebner
(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ List<PolynomialRationalCoeff> inputBasis, outputGroebner;
  for (int i=0; i<theArgumentList.size; i++)
  { ParserNode& currentNode=theNode.owner->TheObjects[theArgumentList.TheObjects[i]];
    outputGroebner.AddObjectOnTop(currentNode.polyValue.GetElement());
  }
  inputBasis=outputGroebner;
  PolynomialRationalCoeff buffer1, buffer2, buffer3, buffer4;
  Monomial<Rational> bufferMon1, bufferMon2;
  RationalFunction::TransformToReducedGroebnerBasis(outputGroebner, buffer1, buffer2, buffer3, buffer4, bufferMon1, bufferMon2, & theGlobalVariables);
  std::stringstream out;
  out << "<br>Starting basis: ";
  std::stringstream out1, out2;
  PolynomialOutputFormat theFormat;
  for(int i=0; i<inputBasis.size; i++)
    out1 << inputBasis.TheObjects[i].ElementToString(theFormat) << ", ";
  out << CGIspecificRoutines::GetHtmlMathDivFromLatexFormula(out1.str());
  out << "<br>Reduced Groebner basis:";
  for(int i=0; i<outputGroebner.size; i++)
    out2 << outputGroebner.TheObjects[i].ElementToString(theFormat) << ", ";
  out << CGIspecificRoutines::GetHtmlMathDivFromLatexFormula(out2.str());
  theNode.ExpressionType=theNode.typeString;
  theNode.outputString= out.str();
  return theNode.errorNoError;
}

void RationalFunction::GetRelations
  ( List<PolynomialRationalCoeff>& theGenerators, GlobalVariables& theGlobalVariables
   )
{ if (theGenerators.size==0)
    return;
  List<PolynomialRationalCoeff> theGroebnerBasis;
  theGroebnerBasis=theGenerators;
  int numStartingGenerators=theGenerators.size;
  int numStartingVariables= theGenerators.TheObjects[0].NumVars;
  PolynomialRationalCoeff tempP;
  PolynomialOutputFormat tempFormat;
  for (int i=0; i<numStartingGenerators; i++)
  { PolynomialRationalCoeff& currentPoly=theGroebnerBasis.TheObjects[i];
    currentPoly.SetNumVariablesSubDeletedVarsByOne(numStartingVariables+numStartingGenerators);
    tempP.MakeNVarDegOnePoly(numStartingVariables+numStartingGenerators, i+numStartingVariables, (Rational) -1);
    currentPoly+=tempP;
//  std::cout << currentPoly.ElementToString(false, tempFormat) << "<br>";
  }
  PolynomialRationalCoeff buffer1, buffer2, buffer3, buffer4;
  Monomial<Rational> bufferMon1, bufferMon2;
  RationalFunction::TransformToReducedGroebnerBasis
  (
   theGroebnerBasis, buffer1, buffer2, buffer3, buffer4, bufferMon1, bufferMon2,
   & Monomial<Rational>::LeftIsGEQLexicographicLastVariableWeakest, & theGlobalVariables
  );
//  std::cout << "<br>the ending generators:<br> ";
//  for (int i=0; i<theGroebnerBasis.size; i++)
//  { std::cout << theGroebnerBasis.TheObjects[i].ElementToString(false, tempFormat) << "<br>";
//  }
  theGenerators.MakeActualSizeAtLeastExpandOnTop(theGroebnerBasis.size);
  theGenerators.size=0;
  for (int i=0; i<theGroebnerBasis.size; i++)
  { PolynomialRationalCoeff& currentPoly= theGroebnerBasis.TheObjects[i];
    bool bad=false;
    for (int j=0; j<numStartingVariables; j++)
      if(currentPoly.GetMaxPowerOfVariableIndex(j)>0)
      { bad=true;
        break;
      }
    if (!bad)
     theGenerators.AddObjectOnTop(currentPoly);
  }
}

int ParserNode::EvaluateRelations
(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ List<PolynomialRationalCoeff> inputBasis, outputRelations;
  for (int i=0; i<theArgumentList.size; i++)
  { ParserNode& currentNode=theNode.owner->TheObjects[theArgumentList.TheObjects[i]];
    outputRelations.AddObjectOnTop(currentNode.polyValue.GetElement());
  }
  inputBasis=outputRelations;
  RationalFunction::GetRelations(outputRelations, theGlobalVariables);
  std::stringstream out;
  out << "<br>Starting elements:";
  int numVariables= inputBasis.TheObjects[0].NumVars;
  PolynomialOutputFormat theFormat;
  std::stringstream out3;
  for(int i=0; i<inputBasis.size; i++)
    out3 << "u_{" << i+numVariables+1 << "}:=" << inputBasis.TheObjects[i].ElementToString(theFormat) << ", ";
  out << CGIspecificRoutines::GetHtmlMathSpanFromLatexFormula(out3.str());
  out << "<br>Resulting relations:";
  std::stringstream out2;
  theFormat.MakeAlphabetArbitraryWithIndex("u");
  for(int i=0; i<outputRelations.size; i++)
    out2 << outputRelations.TheObjects[i].ElementToString(theFormat) << ", ";
  out << CGIspecificRoutines::GetHtmlMathSpanFromLatexFormula(out2.str());
  theNode.ExpressionType=theNode.typeString;
  theNode.outputString= out.str();
  return theNode.errorNoError;
}

int ParserNode::EvaluateCone
(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ roots theNormals;
  int theDim;
  if (!theNode.GetRootsEqualDimNoConversionNoEmptyArgument(theArgumentList, theNormals, theDim))
    return theNode.SetError(theNode.errorDimensionProblem);
  Cone& currentCone=theNode.theCone.GetElement();
  currentCone.CreateFromNormals(theNormals, theGlobalVariables);
  theNode.outputString=currentCone.ElementToString(false, true);
  theNode.ExpressionType=theNode.typeCone;
  return theNode.errorNoError;
}

int ParserNode::EvaluateMaxQPOverCone(GlobalVariables& theGlobalVariables)
{ List<int> argumentList;
  this->ExtractArgumentList(argumentList);
  if (argumentList.size<2)
    return this->SetError(this->errorBadOrNoArgument);
  List<QuasiPolynomial> theQPolys;
  theQPolys.SetSize(argumentList.size-1);
  for (int i=0; i<argumentList.size-1; i++)
  { ParserNode& currentNode=this->owner->TheObjects[argumentList.TheObjects[i]];
    if (!currentNode.ConvertToType(this->typeQuasiPolynomial, theGlobalVariables))
      return this->SetError(this->errorBadOrNoArgument);
    theQPolys.TheObjects[i]=currentNode.theQP.GetElement();
  }
  ParserNode& coneNode=this->owner->TheObjects[*argumentList.LastObject()];
  if (!coneNode.ConvertToType(this->typeCone, theGlobalVariables))
    return this->SetError(this->errorBadOrNoArgument);
  Cone currentCone;
  currentCone=coneNode.theCone.GetElement();
  std::stringstream out;
//  out << "input polys: ";
//  for (int i=0; i<theQPolys.size; i++)
//    out <<  theQPolys.TheObjects[i].ElementToString(true, false) << "<br>";
//  out << "<br>The cone: " << currentCone.ElementToString();
  ConeComplex theComplex;
  List<QuasiPolynomial> output;
  theComplex.findMaxQPOverConeProjective(currentCone, theQPolys, output, theGlobalVariables);
  out << "<br>The complex size is: " << theComplex.size;

  for (int i=0; i<theComplex.size; i++)
  { out << "<br>over cone number " << i+1 << " with internal point " << theComplex.TheObjects[i].GetInternalPoint().ElementToString() <<  " the maximum is: " << output.TheObjects[i].ElementToString(true, false);
  }
  this->outputString=out.str();
  this->ExpressionType=this->typeString;
  return this->errorNoError;
}

int ParserNode::EvaluateMaxLFOverCone(GlobalVariables& theGlobalVariables)
{ List<int> argumentList;
  this->ExtractArgumentList(argumentList);
  if (argumentList.size<2)
    return this->SetError(this->errorBadOrNoArgument);
  List<PolynomialRationalCoeff> thePolys;
  thePolys.SetSize(argumentList.size-1);
  for (int i=0; i<argumentList.size-1; i++)
  { ParserNode& currentNode=this->owner->TheObjects[argumentList.TheObjects[i]];
    if (!currentNode.ConvertToType(this->typePoly, theGlobalVariables))
      return this->SetError(this->errorBadOrNoArgument);
    thePolys.TheObjects[i]=currentNode.polyValue.GetElement();
  }
  ParserNode& coneNode=this->owner->TheObjects[*argumentList.LastObject()];
  if (!coneNode.ConvertToType(this->typeCone, theGlobalVariables))
    return this->SetError(this->errorBadOrNoArgument);
  Cone currentCone;
  currentCone=coneNode.theCone.GetElement();
  std::stringstream out;
  out << "input polys: ";
  for (int i=0; i<thePolys.size; i++)
    out <<  thePolys.TheObjects[i].ElementToString() << "<br>";
  out << "<br>The cone: " << currentCone.ElementToString();
  ConeComplex theComplex;
  List<int> output;
  theComplex.findMaxLFOverConeProjective(currentCone, thePolys, output, theGlobalVariables);
  out << "<br>" << theComplex.ElementToString(false, true);
  this->outputString=out.str();
  this->ExpressionType=this->typeString;
  return this->errorNoError;
}

bool ConeComplex::findMaxQPOverConeProjective
  (Cone& input, List<QuasiPolynomial>& inputQPs, List<QuasiPolynomial>& outputMaximumOverEeachSubChamber, GlobalVariables& theGlobalVariables)
{ if (inputQPs.size==0)
    return false;
  List<PolynomialRationalCoeff> allPolys;
  PolynomialRationalCoeff tempP;
  Rational tempRat;
  Monomial<Rational> tempM;
  Lattice theLatticeIntersection;
  bool latticeInited=false;
  for (int i=0; i<inputQPs.size; i++)
  { QuasiPolynomial& currentQP=inputQPs.TheObjects[i];
    for (int j=0; j<currentQP.valueOnEachLatticeShift.size; j++)
    { PolynomialRationalCoeff& currentPoly=currentQP.valueOnEachLatticeShift.TheObjects[j];
      currentPoly.GetConstantTerm(tempRat, (Rational) 0);
      tempP=currentPoly;
      tempM.MakeMonomialOneLetter(tempP.NumVars+1, tempP.NumVars, 1, tempRat);
      tempP.IncreaseNumVariables(1);
      tempP.AddMonomial(tempM);
      tempP.AddConstant(-tempRat);
      allPolys.AddObjectOnTop(tempP);
    }
    if (!latticeInited)
    { theLatticeIntersection=currentQP.AmbientLatticeReduced;
      latticeInited=true;
    } else
      theLatticeIntersection.IntersectWith(currentQP.AmbientLatticeReduced);
  }
  //std::cout << "allPolys: ";
  //for (int i=0; i<allPolys.size; i++)
  //{ std::cout << allPolys.TheObjects[i].ElementToString() << " with numvars: " << allPolys.TheObjects[i].NumVars << ", ";
  //}
  List<int> tempList;
  if (!this->findMaxLFOverConeProjective(input, allPolys, tempList, theGlobalVariables))
    return false;
//  std::cout << "MaxLF passed";
  List<QuasiPolynomial> commonlyRefinedQPs;
  commonlyRefinedQPs=inputQPs;
  roots allShifts;
  for (int i=0; i<commonlyRefinedQPs.size; i++)
  { QuasiPolynomial& currentQP=commonlyRefinedQPs.TheObjects[i];
    currentQP.MakeRougherLattice(theLatticeIntersection);
    for (int j=0; j<currentQP.LatticeShifts.size; j++)
      allShifts.AddOnTopNoRepetition(currentQP.LatticeShifts.TheObjects[j]);
  }
  root currentInternalPoint;
  outputMaximumOverEeachSubChamber.SetSize(this->size);
  for (int i=0; i<this->size; i++)
  { QuasiPolynomial& outputMax=outputMaximumOverEeachSubChamber.TheObjects[i];
    outputMax.AmbientLatticeReduced=theLatticeIntersection;
    for (int j=0; j<allShifts.size; j++)
    { root& currentShift=allShifts.TheObjects[j];
      Cone& currentCone=this->TheObjects[i];
      currentCone.GetInternalPoint(currentInternalPoint);
      if (*currentInternalPoint.LastObject()!=0)
        currentInternalPoint/=*currentInternalPoint.LastObject();
      bool initializedMax=false;
      Rational theMax, currentValue;
      currentValue=0; theMax=0;
      tempP.Nullify(currentCone.GetDim());
      /*for (int k=0; k<commonlyRefinedQPs.size; k++)
      { QuasiPolynomial& currentQP=commonlyRefinedQPs.TheObjects[k];
        for (int l=0; l< currentQP.LatticeShifts.size; l++)
        { currentQP.valueOnEachLatticeShift.TheObjects[l].Evaluate(currentInternalPoint, currentValue);
          std::cout << "<br>cone " << i+1 << " shift " << j+1 << " poly " << k+1 << " shift "<< currentQP.LatticeShifts.TheObjects[l].ElementToString() <<  " value: " << currentValue.ElementToString();
        }
      }*/
      for (int k=0; k<commonlyRefinedQPs.size; k++)
      { QuasiPolynomial& currentQP=commonlyRefinedQPs.TheObjects[k];
        int index=currentQP.LatticeShifts.IndexOfObject(currentShift);
        if (index==-1)
          currentValue=0;
        else
          currentQP.valueOnEachLatticeShift.TheObjects[index].Evaluate(currentInternalPoint, currentValue);
        if (currentValue>theMax || !initializedMax)
        { initializedMax=true;
          theMax=currentValue;
          if (index==-1)
            tempP.Nullify(currentCone.GetDim());
          else
            tempP=currentQP.valueOnEachLatticeShift.TheObjects[index];
        }
      }
      outputMax.AddLatticeShift(tempP, currentShift);
    }
  }
  return true;
}

bool ConeComplex::findMaxLFOverConeProjective
  (Cone& input, List<PolynomialRationalCoeff>& inputLinPolys, List<int>& outputMaximumOverEeachSubChamber, GlobalVariables& theGlobalVariables)
{ this->init();
  //std::cout << "Starting cone: " << input.ElementToString(false, true);
  this->AddNonRefinedChamberOnTopNoRepetition(input);
  roots HyperPlanesCorrespondingToLF;
  if (input.Normals.size<1 || inputLinPolys.size<1)
    return false;
  int theDim=input.Normals.TheObjects[0].size;
  HyperPlanesCorrespondingToLF.SetSize(inputLinPolys.size);
  for (int i=0; i<inputLinPolys.size; i++)
  { PolynomialRationalCoeff& currentPoly=inputLinPolys.TheObjects[i];
    if (currentPoly.TotalDegree()!=1 || theDim!=currentPoly.NumVars)
    { std::cout << "u messed up the dim! The num vars is " << currentPoly.NumVars << " and the dim of the cone is " << theDim;
      return false;
    }
    root& newWall=HyperPlanesCorrespondingToLF.TheObjects[i];
    newWall.MakeZero(theDim);
    for (int j=0; j<currentPoly.size; j++)
      for (int k=0; k<theDim; k++)
        if (currentPoly.TheObjects[j].degrees[k]==1)
        { newWall.TheObjects[k]=currentPoly.TheObjects[j].Coefficient;
          break;
        }
  }
  root tempRoot;
  for (int i=0; i<HyperPlanesCorrespondingToLF.size; i++)
    for (int j=i+1; j<HyperPlanesCorrespondingToLF.size; j++)
    { tempRoot=HyperPlanesCorrespondingToLF.TheObjects[i]-HyperPlanesCorrespondingToLF.TheObjects[j];
      tempRoot.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
      if (!tempRoot.IsEqualToZero())
        this->splittingNormals.AddObjectOnTopNoRepetitionOfObjectHash(tempRoot);
    }
//  std::cout << this->ElementToString(false, true);
  this->Refine(theGlobalVariables);
  outputMaximumOverEeachSubChamber.SetSize(this->size);
  Rational theMax=0;
  for (int i=0; i<this->size; i++)
  { this->TheObjects[i].GetInternalPoint(tempRoot);
    bool isInitialized=false;
    tempRoot.ComputeDebugString();
    for (int j=0; j<HyperPlanesCorrespondingToLF.size; j++)
      if (!isInitialized || tempRoot.ScalarEuclidean(HyperPlanesCorrespondingToLF.TheObjects[j], (Rational) 0)>theMax)
      { HyperPlanesCorrespondingToLF.TheObjects[j].ComputeDebugString();
        theMax=tempRoot.ScalarEuclidean(HyperPlanesCorrespondingToLF.TheObjects[j], (Rational) 0);
        outputMaximumOverEeachSubChamber.TheObjects[i]=j;
        isInitialized=true;
      }
  }
  for (int i=0; i<this->size; i++)
  { //std::cout << "<br>Chamber " << i+1 << " maximum linear function is the function of index " << outputMaximumOverEeachSubChamber.TheObjects[i] << ": " << inputLinPolys.TheObjects[outputMaximumOverEeachSubChamber.TheObjects[i]].ElementToString();
    //std::cout << "<br>The chamber is given by: " << this->TheObjects[i].ElementToString(false, true);
  }
  return true;
}

//std::string tempDebugString;
void Lattice::Reduce
()
{ //////////////////////////////////the below function is for debugging purposes only!
  //Don't wanna mess around with #ifdefs, but it should be surrounded by #ifdef DEBUG .... #endif
  //bool flagTesting=true;
  //static MatrixLargeRational testMatrix;
  //if (flagTesting)
  //  this->TestGaussianEliminationEuclideanDomainRationals(testMatrix);
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  LargeInt tempInt, tempInt2;
  tempInt.MakeMOne(); tempInt2.MakeOne();
  this->basis.GaussianEliminationEuclideanDomain(tempInt, tempInt2);
  int numRowsToTrim=0;
  for (int i=this->basis.NumRows-1; i>=0; i--)
  { bool foundNonZeroRow=false;
    for (int j=0; j<this->basis.NumCols; j++)
      if (!this->basis.elements[i][j].IsEqualToZero())
      { foundNonZeroRow=true;
        break;
      }
    if (foundNonZeroRow)
      break;
    numRowsToTrim++;
  }
  this->basis.Resize(this->basis.NumRows-numRowsToTrim, this->basis.NumCols, true);
  this->basisRationalForm.AssignMatrixIntWithDen(this->basis, this->Den);
//  if (flagTesting)
 // { testMatrix.Resize(testMatrix.NumRows-numRowsToTrim, testMatrix.NumCols, true);
//    std::stringstream out;
//    std::cout << "<br>basis rational form: " << this->basisRationalForm.ElementToString(true, false);
//    out << "<br>basis rational form: " << this->basisRationalForm.ElementToString(true, false);
//    std::string tempS=out.str();
//    assert(testMatrix==this->basisRationalForm);
//  }
}

void Lattice::TestGaussianEliminationEuclideanDomainRationals(MatrixLargeRational& output)
{ output.AssignMatrixIntWithDen(this->basis, this->Den);
  std::stringstream out;
  std::cout << "Test output: " << output.ElementToString(true, false);
  out << "Test output: " << output.ElementToString(true, false);
  output.GaussianEliminationEuclideanDomain((Rational) -1, (Rational) 1);
  std::cout << "<br>After gaussian elimination:" << output.ElementToString(true, false);
  out << "<br>After gaussian elimination:" << output.ElementToString(true, false);
//  tempDebugString=out.str();
}

void Lattice::RefineByOtherLattice(const Lattice& other)
{ if (other.basis.NumCols==0)
    return;
  if (other.basis==this->basis && this->Den==other.Den)
    return;

  assert(other.GetDim()==this->GetDim());
  int theDim=this->GetDim();
  LargeIntUnsigned oldDen=this->Den;
  LargeIntUnsigned::lcm(other.Den, oldDen, this->Den);
  LargeIntUnsigned scaleThis, scaleOther;
  scaleThis=this->Den/oldDen;
  scaleOther=this->Den/other.Den;
  int oldNumRows=this->basis.NumRows;
  LargeInt tempI; tempI=scaleThis;
  this->basis*=tempI;
  this->basis.Resize(this->basis.NumRows+other.basis.NumRows, theDim, true);
  for (int i=oldNumRows; i<this->basis.NumRows; i++)
    for (int j=0; j<this->basis.NumCols; j++)
      this->basis.elements[i][j]=other.basis.elements[i-oldNumRows][j]*scaleOther;
  this->Reduce();
}

bool ParserNode::GetRootsEqualDimNoConversionNoEmptyArgument
(List<int>& theArgumentList, roots& output, int& outputDim)
{ ParserNode& firstNode= this->owner->TheObjects[theArgumentList.TheObjects[0]];
  output.SetSize(theArgumentList.size);
  if (firstNode.ExpressionType!=this->typeArray)
  { outputDim=1;
    for (int i=0; i<theArgumentList.size; i++)
    { ParserNode& currentNode=this->owner->TheObjects[theArgumentList.TheObjects[i]];
      if (currentNode.ExpressionType!=this->typeRational)
        return false;
      output.TheObjects[i].SetSize(1);
      output.TheObjects[i].TheObjects[0]=currentNode.rationalValue;
    }
    return true;
  }
  outputDim=firstNode.children.size;
  for (int i=0; i<theArgumentList.size; i++)
  { ParserNode& currentNode=this->owner->TheObjects[theArgumentList.TheObjects[i]];
    if (outputDim!=currentNode.children.size)
      return false;
    output.TheObjects[i].SetSize(outputDim);
    for (int j=0; j<currentNode.children.size; j++)
      output.TheObjects[i].TheObjects[j]=this->owner->TheObjects[currentNode.children.TheObjects[j]].rationalValue;
  }
  return true;
}

int ParserNode::EvaluateLattice
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ roots LatticeBase;
  int theDim=-1;
  if(!theNode.GetRootsEqualDimNoConversionNoEmptyArgument(theArgumentList, LatticeBase, theDim))
    return theNode.SetError(theNode.errorDimensionProblem);
  theNode.theLattice.GetElement().MakeFromRoots(LatticeBase);
  theNode.outputString=theNode.theLattice.GetElement().ElementToString();
  theNode.ExpressionType=theNode.typeLattice;
  return theNode.errorNoError;
}

void Lattice::MakeFromMat(const MatrixLargeRational& input)
{ this->basisRationalForm=input;
  this->basisRationalForm.GetMatrixIntWithDen(this->basis, this->Den);
  this->Reduce();
}

void Lattice::MakeFromRoots(const roots& input)
{ MatrixLargeRational tempMat;
  tempMat.AssignRootsToRowsOfMatrix(input);
  tempMat.GetMatrixIntWithDen(this->basis, this->Den);
  this->Reduce();
}

std::string Lattice::ElementToString(bool useHtml, bool useLatex)const
{ std::stringstream out;
  out << "L=<";
  roots tempRoots;
  tempRoots.AssignMatrixRows(this->basisRationalForm);
  for (int i=0; i<this->basisRationalForm.NumRows; i++)
  { out << tempRoots.TheObjects[i].ElementToString();
    if (i!=this->basisRationalForm.NumRows-1)
      out << ",";
  }
  out << ">";
  return out.str();
}

#ifdef WIN32
#pragma warning(disable:4244)//warning 4244: data loss from conversion
#endif
void LargeIntUnsigned::DivPositive(const LargeIntUnsigned& x, LargeIntUnsigned& quotientOutput, LargeIntUnsigned& remainderOutput) const
{ LargeIntUnsigned remainder, quotient, divisor;
  remainder.Assign(*this);
  divisor.Assign(x);
  assert(!divisor.IsEqualToZero());
  quotient.MakeZero();
  //std::string tempS1, tempS2, tempS3;
  while (remainder.IsGEQ(divisor))
  { unsigned int q;
    LargeIntUnsigned current, Total;
    if (*remainder.LastObject()>*divisor.LastObject())
    { q=*remainder.LastObject()/(*divisor.LastObject()+1);
      current.AssignShiftedUInt(q, remainder.size-divisor.size);
    }
    else
    { if (remainder.size==divisor.size)
        current.AssignShiftedUInt(1, 0);
      else
      { q=this->CarryOverBound/(divisor.TheObjects[divisor.size-1]+1);
        current.AssignShiftedUInt(q, remainder.size- divisor.size-1);
        current.MultiplyByUInt(remainder.TheObjects[remainder.size-1]);
      }
    }
    Total.Assign(divisor);
    Total.MultiplyBy(current);
    //if (!remainder.IsGEQ(Total))
    //{ tempS1= remainder.ElementToString();
    //  tempS2=Total.ElementToString();
    //  remainder.IsGEQ(Total);
    //}
    remainder.SubtractSmallerPositive(Total);
    quotient.Add(current);
  }
  remainderOutput.Assign(remainder);
  quotientOutput.Assign(quotient);
//  assert(remainderOut.CheckForConsistensy());
}
#ifdef WIN32
#pragma warning(default:4244)//warning 4244: data loss from conversion
#endif

//returning false means that the lattice given as rougher is not actually rougher than the current lattice
//or that there are too many representatives
bool Lattice::GetAllRepresentatitves
  (const Lattice& rougherLattice, roots& output)const
{ output.size=0;
  if (this->basis.NumRows!=rougherLattice.basis.NumRows)
    return false;
  List<int> thePeriods;
  roots thePeriodVectors;
  thePeriods.SetSize(this->basis.NumRows);
  thePeriodVectors.SetSize(this->basis.NumRows);
  root tempRoot, tempRoot2;
  int col=0;
  int theDim=this->GetDim();
  Rational currentPeriod;
  LargeInt currentPeriodInt;
  for (int i=0; i<this->basis.NumRows; i++)
  { while (this->basisRationalForm.elements[i][col].IsEqualToZero())
      col++;
    currentPeriod=rougherLattice.basisRationalForm.elements[i][col]/this->basisRationalForm.elements[i][col];
    currentPeriod.GetNum(currentPeriodInt);
    if (currentPeriodInt.value.size>1)
//    { std::cout << "exited at point 1 counter i is " << i;
      return false;
//    }
    else
      thePeriods.TheObjects[i]=currentPeriodInt.value.TheObjects[0];
    this->basisRationalForm.RowToRoot(i, thePeriodVectors.TheObjects[i]);
    rougherLattice.basisRationalForm.RowToRoot(i, tempRoot2);
    tempRoot=thePeriodVectors.TheObjects[i];
    tempRoot*=thePeriods.TheObjects[i];

//    if (!(tempRoot-tempRoot2).IsEqualToZero())
//    { std::cout << "exited at point 2 counter i is " << i << " the period vector=" << thePeriodVectors.TheObjects[i].ElementToString() << " rougher lattice basis vector: " << tempRoot2.ElementToString();
//      return false;
//    }
  }
//  std::cout << thePeriodVectors.ElementToString() << "<br>The periods: ";
//  for (int i=0; i<thePeriods.size; i++)
//    std::cout << thePeriods.TheObjects[i] << ", ";
  for (int i=0; i<thePeriods.size; i++)
    thePeriods.TheObjects[i]--;
  SelectionWithDifferentMaxMultiplicities theCoeffSelection;
  theCoeffSelection.initFromInts(thePeriods);
  int NumCycles=theCoeffSelection.getTotalNumSubsets();
  output.SetSize(NumCycles);
  for (int i=0; i<NumCycles; i++, theCoeffSelection.IncrementSubset())
  { output.TheObjects[i].MakeZero(theDim);
    for (int j=0; j<theCoeffSelection.Multiplicities.size; j++)
      output.TheObjects[i]+=thePeriodVectors.TheObjects[j]*theCoeffSelection.Multiplicities.TheObjects[j];
  }
//  std::cout << "The representatives: " << output.ElementToString(false, true, false);
  return true;
}

int ParserNode::EvaluateGetAllRepresentatives(GlobalVariables& theGlobalVariables)
{ List<int> argumentList;
  this->ExtractArgumentList(argumentList);
  if (argumentList.size!=2)
    return this->SetError(this->errorBadOrNoArgument);
  for (int i=0; i<argumentList.size; i++)
  { ParserNode& currentNode=this->owner->TheObjects[argumentList.TheObjects[i]];
    if (!currentNode.ConvertToType(this->typeLattice, theGlobalVariables))
      return this->SetError(this->errorBadOrNoArgument);
  }
  Lattice& finerLattice=this->owner->TheObjects[argumentList.TheObjects[0]].theLattice.GetElement();
  Lattice& rougherLattice=this->owner->TheObjects[argumentList.TheObjects[1]].theLattice.GetElement();
  roots tempRoots;
  if (!finerLattice.GetAllRepresentatitves(rougherLattice, tempRoots))
    return this->SetError(this->errorImplicitRequirementNotSatisfied);
  this->outputString=tempRoots.ElementToString();
  this->ExpressionType=this->typeString;
  return this->errorNoError;
}

void QuasiPolynomial::MakeRougherLattice(const Lattice& latticeToRoughenBy)
{ if (this->AmbientLatticeReduced==latticeToRoughenBy)
    return;
  //std::cout << "roughening " << this->AmbientLatticeReduced.ElementToString() << "by" << latticeToRoughenBy.ElementToString();
  Lattice OldLattice;
  OldLattice=this->AmbientLatticeReduced;
  this->AmbientLatticeReduced.IntersectWith(latticeToRoughenBy);
  roots representativesQuotientLattice;
  //std::cout << "getting all representatives of " << OldLattice.ElementToString() << "inside" << this->AmbientLatticeReduced.ElementToString();
  //std::cout << "<br><br><br><br>*********<br><br><br><br>";
  OldLattice.GetAllRepresentatitves(this->AmbientLatticeReduced, representativesQuotientLattice);
  roots OldLatticeShifts=this->LatticeShifts;
  List<PolynomialRationalCoeff> oldValues;
  oldValues=this->valueOnEachLatticeShift;
  this->LatticeShifts.SetSize(OldLatticeShifts.size*representativesQuotientLattice.size);
  this->valueOnEachLatticeShift.SetSize(this->LatticeShifts.size);
  for (int i=0; i<OldLatticeShifts.size; i++)
    for (int j=0; j<representativesQuotientLattice.size; j++)
    { this->LatticeShifts.TheObjects[i*representativesQuotientLattice.size+j]=OldLatticeShifts.TheObjects[i]+representativesQuotientLattice.TheObjects[j];
      this->valueOnEachLatticeShift.TheObjects[i*representativesQuotientLattice.size+j]=oldValues.TheObjects[i];
    }
}

void Lattice::GetDualLattice(Lattice& output)const
{ if (this->GetRank()!=this->GetDim())
    return;
  MatrixLargeRational tempMat;
  tempMat=this->basisRationalForm;
  tempMat.Invert();
  tempMat.Transpose();
  tempMat.GetMatrixIntWithDen(output.basis, output.Den);
  output.Reduce();
}

bool Lattice::FindOnePreimageInLatticeOf
    (const MatrixLargeRational& theLinearMap, const roots& input, roots& output, GlobalVariables& theGlobalVariables)
{ Vectors<Rational> thisBasis, tempInput, tempOutput;
  thisBasis.AssignMatrixRows(this->basisRationalForm);
  theLinearMap.ActOnVectorsColumn(thisBasis, (Rational) 0);
  input.GetVectorsRational(tempInput);
  bool result=tempInput.GetIntegralCoordsInBasisIfTheyExist(thisBasis, tempOutput, (Rational) 1, (Rational) -1, (Rational) 0);
  output.AssignVectorsRational(tempOutput);
  //std::cout << "<hr>acting by " << this->basisRationalForm.ElementToString(true, false) << " on " << output.ElementToString();
  MatrixLargeRational tempMat;
  tempMat=this->basisRationalForm;
  tempMat.Transpose();
  tempMat.ActOnRoots(output);
  //std::cout << "<br> to get " << output.ElementToString();
  return result;
}

void Lattice::IntersectWithPreimageOfLattice
  (const MatrixLargeRational& theLinearMap, const Lattice& other, GlobalVariables& theGlobalVariables)
{ roots startingBasis, imageStartingBasis, basisImageIntersection, basisImageIntersectionInCoordsWRTimageStartingBasis, ImageBasisInImageStartingBasisCoords;
  roots resultNonKernelPart, resultKernelPart, result, tempRoots;
  startingBasis.AssignMatrixRows(this->basisRationalForm);
  std::cout << "<br>Starting basis: " << startingBasis.ElementToString();
  std::cout << "<br>The linear map: " << theLinearMap.ElementToString(true, false);
  theLinearMap.ActOnRoots(startingBasis, imageStartingBasis);
  std::cout << "<br>Image of starting basis: " << imageStartingBasis.ElementToString();
  Lattice ImageLattice;
  ImageLattice.MakeFromRoots(imageStartingBasis);
  ImageLattice.IntersectWith(other);
  std::cout << "<br>Image lattice: " << ImageLattice.ElementToString(true, false);
  basisImageIntersection.AssignMatrixRows(ImageLattice.basisRationalForm);
  Vectors<Rational> tempBasisImageIntersection, tempImageStartingBasis, tempImageBasisInImageStartingBasisCoords;
  basisImageIntersection.GetVectorsRational(tempBasisImageIntersection);
  imageStartingBasis.GetVectorsRational(tempImageStartingBasis);
  bool tempBool= tempBasisImageIntersection.GetIntegralCoordsInBasisIfTheyExist(tempImageStartingBasis, tempImageBasisInImageStartingBasisCoords, (Rational) 1, (Rational) -1, (Rational) 0);
  ImageBasisInImageStartingBasisCoords.AssignVectorsRational(tempImageBasisInImageStartingBasisCoords);
  assert(tempBool);
  std::cout << "<br> Basis of image lattice expressed in coordinates with respect to image of starting basis: " << ImageBasisInImageStartingBasisCoords.ElementToString();
  resultNonKernelPart.SetSize(ImageBasisInImageStartingBasisCoords.size);
  for (int i=0; i<resultNonKernelPart.size; i++)
  { root& currentRoot=resultNonKernelPart.TheObjects[i];
    currentRoot.MakeZero(this->GetDim());
    for (int j=0; j<startingBasis.size; j++)
      currentRoot+=startingBasis.TheObjects[j]*ImageBasisInImageStartingBasisCoords.TheObjects[i].TheObjects[j];
  }
  std::cout << "<br> Result non-kernel part (representatives): " << resultNonKernelPart.ElementToString();
  Lattice KernelPart;
  KernelPart=*this;
  tempRoots.AssignMatrixRows(theLinearMap);
  KernelPart.IntersectWithLinearSubspaceGivenByNormals(tempRoots);
  std::cout << "<br>Kernel part of the result: " << KernelPart.ElementToString(true, false);
  result.AssignMatrixRows(KernelPart.basisRationalForm);
  result.AddListOnTop(resultNonKernelPart);
  this->MakeFromRoots(result);
  std::cout << "<br> And the result is.... " << this->ElementToString(true, false);
}

void Lattice::IntersectWith(const Lattice& other)
{ roots commonBasis, otherBasis, startBasis;
  startBasis.AssignMatrixRows(this->basisRationalForm);
  otherBasis.AssignMatrixRows(other.basisRationalForm);
  GlobalVariables theGlobalVariables;
//  std::cout << "<br>this basis: " << startBasis.ElementToString();
//  std::cout << "<br>other basis: " << otherBasis.ElementToString();
  startBasis.IntersectTwoLinSpaces(startBasis, otherBasis, commonBasis, theGlobalVariables);
//  std::cout << "<br> basis of linear space intersection: " << commonBasis.ElementToString() << "<br><br> ";
  Lattice thisLatticeIntersected, otherLatticeIntersected;
  thisLatticeIntersected=*this;
  otherLatticeIntersected=other;
  thisLatticeIntersected.IntersectWithLinearSubspaceSpannedBy(commonBasis);
//  std::cout << "<br> linear space intersected with this lattice: " << thisLatticeIntersected.ElementToString(true, false);
  otherLatticeIntersected.IntersectWithLinearSubspaceSpannedBy(commonBasis);
//  std::cout << "<br> linear space intersected with other lattice: " << otherLatticeIntersected.ElementToString(true, false);
  roots thisCommonBasis, otherCommonBasis, thisCommonCoords, otherCommonCoords;
  thisCommonBasis.AssignMatrixRows(thisLatticeIntersected.basisRationalForm);
  otherCommonBasis.AssignMatrixRows(otherLatticeIntersected.basisRationalForm);
  thisCommonBasis.GetCoordsInBasis(commonBasis, thisCommonCoords, theGlobalVariables);
  otherCommonBasis.GetCoordsInBasis(commonBasis, otherCommonCoords, theGlobalVariables);
//  std::cout << "<br>this lattice intersection new coords: " << thisCommonBasis.ElementToString();
//  std::cout << "<br>other lattice intersection new coords: " << otherBasis.ElementToString();
  Lattice thisCommonCoordsLattice, otherCommonCoordsLattice;
  thisCommonCoordsLattice.MakeFromRoots(thisCommonCoords);
  otherCommonCoordsLattice.MakeFromRoots(otherCommonCoords);
//  std::cout << "<br> linear space intersected with this lattice new coords: " << thisCommonCoordsLattice.ElementToString(true, false);
//  std::cout << "<br> linear space intersected with other lattice new coords: " << otherCommonCoordsLattice.ElementToString(true, false);
  thisCommonCoordsLattice.IntersectWithBothOfMaxRank(otherCommonCoordsLattice);
//  std::cout << "<br> intersection lattice new coords: " << thisCommonCoordsLattice.ElementToString(true, false);
  roots resultBasis;
  resultBasis.SetSize(thisCommonCoordsLattice.basisRationalForm.NumRows);
  for (int i=0; i<resultBasis.size; i++)
  { root& currentRoot=resultBasis.TheObjects[i];
    currentRoot.MakeZero(this->GetDim());
    for (int j=0; j<thisCommonCoordsLattice.basisRationalForm.NumCols; j++)
      currentRoot+=thisCommonCoordsLattice.basisRationalForm.elements[i][j]*commonBasis.TheObjects[j];
  }
  this->MakeFromRoots(resultBasis);
//  std::cout << "<br>final answer: intersection is: " << this->ElementToString(true, false);
}

void Lattice::IntersectWithBothOfMaxRank(const Lattice& other)
{ Lattice dualLatticeThis, dualLatticeOther;
  //std::cout << "intersecting " << this->ElementToString() << " and " << other.ElementToString();
  assert(this->basis.NumRows==this->GetDim() && this->GetDim()==other.GetDim() && other.basis.NumRows== this->GetDim());
  this->GetDualLattice(dualLatticeThis);
  //std::cout << "<br>dual lattice of left: " << dualLatticeThis.ElementToString();
  other.GetDualLattice(dualLatticeOther);
  //std::cout << "<br>dual lattice of right: " << dualLatticeOther.ElementToString();
  dualLatticeThis.RefineByOtherLattice(dualLatticeOther);
  //std::cout << "<br> common refinement of dual lattice: " << dualLatticeThis.ElementToString();
  dualLatticeThis.GetDualLattice(*this);
  //std::cout << "<br>final result: " << this->ElementToString();
}

void QuasiPolynomial::operator+=(const QuasiPolynomial& other)
{ //std::cout << "current ambient lattice: " << this->AmbientLatticeReduced.ElementToString();
  this->MakeRougherLattice(other.AmbientLatticeReduced);
  //std::cout << "roughened: " << this->AmbientLatticeReduced.ElementToString();
  QuasiPolynomial tempQP=other;
  //std::cout << "<br><br>other ambient lattice: " << this->AmbientLatticeReduced.ElementToString();
  tempQP.MakeRougherLattice(this->AmbientLatticeReduced);
  //std::cout << "roughened: " << this->AmbientLatticeReduced.ElementToString() << "<br><br><br><br>*******<br><br><br>";
  for(int i=0; i<tempQP.LatticeShifts.size; i++)
    this->AddLatticeShift(tempQP.valueOnEachLatticeShift.TheObjects[i], tempQP.LatticeShifts.TheObjects[i]);
}

int ParserNode::EvaluateInvertLattice(GlobalVariables& theGlobalVariables)
{ List<int> argumentList;
  this->ExtractArgumentList(argumentList);
  if (argumentList.size!=1)
    return this->SetError(this->errorBadOrNoArgument);
  ParserNode& currentNode=this->owner->TheObjects[argumentList.TheObjects[0]];
  if(!currentNode.ConvertToType(this->typeLattice, theGlobalVariables))
    return this->SetError(this->errorBadOrNoArgument);
  currentNode.theLattice.GetElement().GetDualLattice(this->theLattice.GetElement());
  this->ExpressionType=this->typeLattice;
  this->outputString=this->theLattice.GetElement().ElementToString();
  return this->errorNoError;
}

std::string QuasiPolynomial::ElementToString(bool useHtml, bool useLatex, const PolynomialOutputFormat& thePolyFormat)
{ std::stringstream out;
  //if (useHtml)
//  out << "the lattice: " << this->AmbientLatticeReduced.ElementToString(useHtml, useLatex);
//  out << "<br>We have " << this->LatticeShifts.size << " lattice shifts. The polynomial on each lattice shift follows.";
  if (this->LatticeShifts.size==0)
    return "0";
  for (int i=0; i<this->LatticeShifts.size; i++)
  { //if(useHtml)
      //out << "<br>Shift: " << this->LatticeShifts.TheObjects[i].ElementToString() << "; polynomial: ";
    if (useLatex)
    { if(!useHtml)
        out << "$\\begin{array}{rcl}&&";
      else
        out << "<span class=\"math\">";
    }
    out << this->valueOnEachLatticeShift.TheObjects[i].ElementToString(thePolyFormat);
    if (useLatex)
    { if(!useHtml)
        out << "\\end{array}$";
      else
        out << "</span>";
    }
    out << " over ";
    if (!this->LatticeShifts.TheObjects[i].IsEqualToZero())
      out << this->LatticeShifts.TheObjects[i].ElementToString() << " + ";
    if (useLatex)
    { if (!useHtml)
        out << "$\\Lambda$ ";
      else
        out << "<span class=\"math\"> \\Lambda</span>";
    } else
      out << "L ";
    if (this->LatticeShifts.size>1)
    { if (useHtml)
        out << "<br>";
      out << "\n\n";
    }
  }
  if (!this->AmbientLatticeReduced.basisRationalForm.IsIdMatrix())
  { if (useLatex)
    { if (!useHtml)
        out << ", where $\\Lambda$=$<$ ";
      else
        out << ", where <span class=\"math\">\\Lambda</span>=<";
    }
    else
      out << ", where L=< ";
    roots tempRoots;
    tempRoots.AssignMatrixRows(this->AmbientLatticeReduced.basisRationalForm);
    for (int i=0; i<tempRoots.size; i++)
    { out << tempRoots.TheObjects[i].ElementToString();
      if (i!=tempRoots.size-1)
        out << ", ";
    }
    if (!useLatex)
      out << ">";
    else
      out << " $>$";
  } else
    if (useLatex)
      out << ", where $\\Lambda=\\mathbb{Z}^{" <<  this->GetNumVars() << "}$";
    else
      out << "Z^" <<  this->GetNumVars();
  return out.str();
}

bool ParserNode::GetRootInt(Vector<int>& output, GlobalVariables& theGlobalVariables)
{ assert(false);
  return true;
}

bool ParserNode::GetRootRational(root& output, GlobalVariables& theGlobalVariables)
{ if (this->ExpressionType!=this->typeArray)
  { output.SetSize(1);
    if (!this->ConvertToType(this->typeRational, theGlobalVariables))
      return false;
    output.TheObjects[0]=this->rationalValue;
    return true;
  }
  output.SetSize(this->children.size);
  for (int i=0; i<output.size; i++)
  { ParserNode& currentNode=this->owner->TheObjects[this->children.TheObjects[i]];
    if (!currentNode.ConvertToType(this->typeRational, theGlobalVariables))
      return false;
    output.TheObjects[i]=currentNode.rationalValue;
  }
  return true;
}

void QuasiPolynomial::MakeFromPolyShiftAndLattice
(const PolynomialRationalCoeff& inputPoly, const root& theShift, const Lattice& theLattice, GlobalVariables& theGlobalVariables)
{ this->AmbientLatticeReduced=theLattice;
  this->LatticeShifts.SetSize(1);
  this->LatticeShifts.TheObjects[0]=theShift;
  this->AmbientLatticeReduced.ReduceVector(this->LatticeShifts.TheObjects[0]);
  this->valueOnEachLatticeShift.SetSize(1);
  this->valueOnEachLatticeShift.TheObjects[0]=inputPoly;
}

bool Lattice::ReduceVector(Vector<Rational>& theVector)
{ root output;
  Vectors<Rational> basisRoots;
  basisRoots.AssignMatrixRows(this->basisRationalForm);
  //std::cout <<  "the basis: " << basisRoots.ElementToString();
  if (!theVector.GetCoordsInBasiS(basisRoots, output, (Rational) 1, (Rational) 0))
  { std::cout << "oops bad!";
    return false;
  }
  for (int i=0; i<output.size; i++)
    output.TheObjects[i].AssignFracValue();
  theVector.MakeZero(theVector.size, (Rational) 0);
  for (int i=0; i<basisRoots.size; i++)
    theVector+=basisRoots.TheObjects[i]*output.TheObjects[i];
  //std::cout << "the vector " << theVector.ElementToString() << " in the basis " << basisRoots.ElementToString() << " has coordinates: " << output.ElementToString();
  return true;
}

int ParserNode::EvaluateQuasiPolynomial(GlobalVariables& theGlobalVariables)
{ List<int> argumentList;
  this->ExtractArgumentList(argumentList);
//  std::cout << "number of arguments: " << argumentList.size;
  //std::cout << "<br> before evaluating your quasipoly the dim is " << this->owner->NumVariables;
  if (argumentList.size!=3)
    return this->SetError(this->errorBadOrNoArgument);
  root theShift;
  ParserNode& thePolyNode=this->owner->TheObjects[argumentList.TheObjects[0]];
  ParserNode& theShiftNode=this->owner->TheObjects[argumentList.TheObjects[1]];
  ParserNode& theLatticeNode=this->owner->TheObjects[argumentList.TheObjects[2]];
  if (!theShiftNode.GetRootRational(theShift, theGlobalVariables) )
    return this->SetError(this->errorConversionError);
  int theDim=theShift.size;
  if (theDim<this->owner->NumVariables)
    return this->SetError(this->errorDimensionProblem);
  this->owner->NumVariables=theDim;
  if (!thePolyNode.ConvertToType(this->typePoly, theGlobalVariables) || !theLatticeNode.ConvertToType(this->typeLattice, theGlobalVariables))
    return this->SetError(this->errorConversionError);
  QuasiPolynomial& output=this->theQP.GetElement();
//  std::cout << "Dimension: " << theDim;
 // std::cout << thePolyNode.ElementToStringValueAndType(true);
//  if (thePolyNode.polyValue.GetElement().NumVars!=theDim)
//    std::cout << "poly conversion failed";
  if (thePolyNode.polyValue.GetElement().NumVars!=theDim || theLatticeNode.theLattice.GetElement().GetDim()!=theDim || theLatticeNode.theLattice.GetElement().GetRank()!=theDim)
    return this->SetError(this->errorDimensionProblem);
  output.MakeFromPolyShiftAndLattice(thePolyNode.polyValue.GetElement(), theShift, theLatticeNode.theLattice.GetElement(), theGlobalVariables);
  this->outputString=output.ElementToString(true, false);
  this->ExpressionType=this->typeQuasiPolynomial;
  return this->errorNoError;
}

void Lattice::MakeZn(int theDim)
{ this->basisRationalForm.MakeIdMatrix(theDim);
  this->basis.MakeIdMatrix(theDim, (LargeInt) 1, (LargeInt) 0);
  this->Den.MakeOne();
}

void QuasiPolynomial::MakeZeroLatticeZn(int theDim)
{ this->AmbientLatticeReduced.MakeZn(theDim);
  this->LatticeShifts.size=0;
  this->valueOnEachLatticeShift.size=0;
}

void partFraction::GetRootsFromDenominator
(partFractions& owner, roots& output)
{ output.SetSize(this->IndicesNonZeroMults.size);
  for (int i=0; i<this->IndicesNonZeroMults.size; i++)
    output.TheObjects[i].AssignIntRoot(owner.RootsToIndices.TheObjects[this->IndicesNonZeroMults.TheObjects[i]]);
}

void partFraction::ComputePolyCorrespondingToOneMonomial
  (partFractions& owner, QuasiPolynomial& outputQP, int monomialIndex, roots& normals, Lattice& theLattice, GlobalVariables& theGlobalVariables)
{ int theDimension=owner.AmbientDimension;
  root shiftRational; shiftRational.SetSize(theDimension);
  Monomial<LargeInt>& currentMon=this->Coefficient.TheObjects[monomialIndex];
  PolynomialRationalCoeff tempP, outputPolyPart;
  for (int j=0; j<theDimension; j++)
    shiftRational.TheObjects[j].AssignInteger(currentMon.degrees[j]);
  Rational tempRat, tempRat2;
  tempRat=currentMon.Coefficient;
  outputPolyPart.MakeNVarConst((int)theDimension, tempRat);
  for (int i=0; i<theDimension; i++)
  { this->MakePolynomialFromOneNormal(normals.TheObjects[i], shiftRational, this->TheObjects[this->IndicesNonZeroMults.TheObjects[i]].Multiplicities.TheObjects[0], tempP);
    outputPolyPart.MultiplyBy(tempP);
  }
  outputQP.MakeFromPolyShiftAndLattice(outputPolyPart, shiftRational, theLattice, theGlobalVariables);
  this->AlreadyAccountedForInGUIDisplay=true;
}

void partFraction::GetVectorPartitionFunction
  (partFractions& owner, QuasiPolynomial& output, GlobalVariables& theGlobalVariables)
{ QuasiPolynomial shiftedPoly;
  roots theNormals, theLatticeGenerators;
  if (partFraction::MakingConsistencyCheck)
    partFraction::CheckSum.MakeZero();
  if (partFraction::flagAnErrorHasOccurredTimeToPanic)
    this->ComputeDebugString(owner, theGlobalVariables);
  this->GetRootsFromDenominator(owner, theLatticeGenerators);
  Lattice theLattice;
  theLattice.MakeFromRoots(theLatticeGenerators);
  MatrixLargeRational theNormalsMatForm;
  theNormalsMatForm.AssignRootsToRowsOfMatrix(theLatticeGenerators);
  theNormalsMatForm.Invert();
  theNormals.AssignMatrixColumns(theNormalsMatForm);
  output.MakeZeroLatticeZn(owner.AmbientDimension);

  for (int i=0; i<this->Coefficient.size; i++)
  { this->ComputePolyCorrespondingToOneMonomial(owner, shiftedPoly, i, theNormals, theLattice, theGlobalVariables);
    output.ComputeDebugString();
    shiftedPoly.ComputeDebugString();
    output+=shiftedPoly;
//    if (RecordNumMonomials)
//    { std::stringstream out4, out3;
//      out4 << "Current fraction: " << i+1<< " out of " << this->Coefficient.size << " processed";
//      partFractions::NumProcessedForVPFMonomialsTotal++;
//      out3  << " Processed " << partFractions::NumProcessedForVPFMonomialsTotal << " out of " << partFractions::NumMonomialsInNumeratorsRelevantFractions << " relevant monomials";
//      theGlobalVariables.theIndicatorVariables.ProgressReportString4= out4.str();
//      theGlobalVariables.theIndicatorVariables.ProgressReportString3= out3.str();
//      theGlobalVariables.FeedIndicatorWindow(theGlobalVariables.theIndicatorVariables);
//    }
  }
//  if (partFraction::MakingConsistencyCheck)
//  { Rational tempLRat;
//    output.Evaluate(partFraction::theVectorToBePartitioned, tempLRat);
//    assert(tempLRat.DenShort==1);
//    assert(tempLRat.IsEqualTo(partFraction::CheckSum));
//  }
//  if (StoreToFile)
//  { this->FileStoragePosition= partFraction::TheBigDump.tellp();
//    output.WriteToFile(partFraction::TheBigDump);
//    partFractions::ComputedContributionsList.flush();
//  }

//  Accum.ComputeDebugString();
}

bool partFractions::GetVectorPartitionFunction
  (QuasiPolynomial& output, root& newIndicator, GlobalVariables& theGlobalVariables)
{ if(this->AssureIndicatorRegularity(theGlobalVariables, newIndicator))
  { theGlobalVariables.theIndicatorVariables.flagRootIsModified=true;
    theGlobalVariables.theIndicatorVariables.modifiedRoot.AssignRoot(newIndicator);
    theGlobalVariables.theIndicatorVariables.ProgressReportString5="Indicator modified to regular";
    theGlobalVariables.FeedIndicatorWindow(theGlobalVariables.theIndicatorVariables);
  } else
    theGlobalVariables.theIndicatorVariables.flagRootIsModified=false;
  this->ResetRelevanceIsComputed();
  if (!this->CheckForMinimalityDecompositionWithRespectToRoot(&newIndicator, theGlobalVariables))
    return false;
  this->NumProcessedForVPFfractions=0;
  Rational oldCheckSum;
  QuasiPolynomialOld oldOutput;
  if (partFraction::MakingConsistencyCheck)
    partFractions::CheckSum.MakeZero();
  theGlobalVariables.theIndicatorVariables.NumProcessedMonomialsCurrentFraction=0;
  output.MakeZeroLatticeZn(this->AmbientDimension);
  ///////////////////////////////////////////////
  //this->flagAnErrorHasOccurredTimeToPanic=true;
  //partFraction::flagAnErrorHasOccurredTimeToPanic=true;
  //this->ComputeDebugString();
  ///////////////////////////////////////////////
  QuasiPolynomial tempQP;
  for (int i=0; i<this->size; i++)
  { //if (this->flagAnErrorHasOccurredTimeToPanic)
    //{ this->TheObjects[i].ComputeDebugString();
    //}
    if (this->TheObjects[i].rootIsInFractionCone(*this, &newIndicator, theGlobalVariables))
    { partFraction& currentPF=this->TheObjects[i];
      currentPF.GetVectorPartitionFunction(*this, tempQP, theGlobalVariables);
/*      if(partFraction::MakingConsistencyCheck)
      {  Rational tempLRat2, tempLRat3, tempLRat4;
        std::string tempS1, tempS2, tempS3, tempS4;
        tempQP.Evaluate(partFraction::theVectorToBePartitioned, tempLRat2);
        output.Evaluate(partFraction::theVectorToBePartitioned, tempLRat3);
        tempLRat2.ElementToString(tempS1);
        tempLRat3.ElementToString(tempS2);
        tempLRat4.Assign(tempLRat2);
        tempLRat4.Add(tempLRat3);
        partFraction::CheckSum.ElementToString(tempS3);
        partFractions::CheckSum.ElementToString(tempS4);
        assert(tempLRat2.den.IsEqualTo(LIOne));
        assert(tempLRat3.den.IsEqualTo(LIOne));
        assert(tempLRat2.IsEqualTo(partFraction::CheckSum));
        assert(tempLRat4.IsEqualTo(partFractions::CheckSum));
        if (i==4)
        { Stop();
          QuasiPolynomialOld::flagAnErrorHasOccurredTimeToPanic=true;
          ::RandomCodeIDontWantToDelete theEvilBug;
          theEvilBug.EvilPoly1.Assign(output);
          theEvilBug.EvilPoly2.Assign(tempQP);
          theEvilBug.RevealTheEvilConspiracy();
          RandomCodeIDontWantToDelete::UsingEvilList1=false;
        }
      }*/
      output+=tempQP;
  /*    if (partFraction::MakingConsistencyCheck)
      {  Rational tempLRat;
        output.Evaluate(partFraction::theVectorToBePartitioned, tempLRat);
        if ((! tempLRat.IsEqualTo(partFractions::CheckSum)) || (! tempLRat.den.IsEqualTo(LIOne)))
        { std::string tempS, tempS2, tempS3, tempS4;
          Rational tempLRat2;
          tempQP.Evaluate(partFraction::theVectorToBePartitioned, tempLRat2);
          partFractions::CheckSum.ElementToString(tempS);
          tempLRat.ElementToString(tempS2);
          oldCheckSum.ElementToString(tempS3);
          tempLRat2.ElementToString(tempS4);
          partFraction::flagAnErrorHasOccurredTimeToPanic=true;
  //        assert(false);
        }
        oldCheckSum.Assign(partFractions::CheckSum);
        oldOutput.Assign(output);
      }*/
      this->MakeProgressVPFcomputation(theGlobalVariables);
    }
  }
/*  if (partFraction::MakingConsistencyCheck)
  { std::string tempS;
    partFractions::CheckSum.ElementToString(tempS);
    Stop();
  }*/
  return true;
}

bool PolynomialRationalCoeff::GetIntegerPoly(Polynomial<LargeInt>& output)const
{ output.Nullify(this->NumVars);
  Monomial<LargeInt> tempM;
  tempM.init(this->NumVars);
  output.MakeActualSizeAtLeastExpandOnTop(this->size);
  for (int i=0; i<this->size; i++)
  { Monomial<Rational>& currentMon=this->TheObjects[i];
    if (!currentMon.Coefficient.IsInteger())
      return false;
    for (int j=0; j<this->NumVars; j++)
      tempM.degrees[i]=currentMon.degrees[i];
    currentMon.Coefficient.GetNum(tempM.Coefficient);
    output.AddMonomial(tempM);
  }
  return true;
}

void LargeInt::WriteToFile(std::fstream& output)
{ std::string tempS;
  this->ElementToString(tempS);
  output << tempS;
}

void LargeInt::ReadFromFile(std::fstream& input)
{ std::string tempS;
  input >> tempS;
  this->AssignString(tempS);
}

void LargeInt::AssignString(const std::string& input)
{ if (input.size()==0)
    return;
  this->MakeZero();
  unsigned int startingIndex=0;
  if (input[0]=='-')
    startingIndex=1;
  for (unsigned int i=startingIndex; i<input.size(); i++)
  { int x=MathRoutines::GetIntFromDigit(input[i]);
    if (x==-1)
      return;
    if (i>startingIndex)
      this->value*=10;
    this->value+=(unsigned) x;
  }
  if (!this->IsEqualToZero())
    if (input[0]=='-')
      this->sign=-1;
}

int ParserNode::EvaluateSplit
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ ParserNode& thePFNode=theNode.owner->TheObjects[theArgumentList.TheObjects[0]];
  theNode.thePFs.GetElement()=thePFNode.thePFs.GetElement();
  theNode.thePFs.GetElement().split(theGlobalVariables, 0);
//  std::string tempS;
//  std::stringstream out;
//  theNode.thePFs.GetElement().ElementToString(tempS, theGlobalVariables);
//  out << "<div class=\"math\">" << tempS << "</div>";
//  theNode.outputString=out.str();
  theNode.ExpressionType=theNode.typePartialFractions;
  return theNode.errorNoError;
}

int ParserNode::EvaluatePartialFractions
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ roots theVectors;
  int theDim=-1;
  if (!theNode.GetRootsEqualDimNoConversionNoEmptyArgument(theArgumentList, theVectors, theDim))
    return theNode.SetError(theNode.errorDimensionProblem);
  //std::cout << theVectors.ElementToString();
  if (!theNode.thePFs.GetElement().initFromRoots(theVectors, theGlobalVariables))
    return theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);
  std::string tempS;
  std::stringstream out;
  PolynomialOutputFormat theFormat;
  theNode.thePFs.GetElement().ElementToString(tempS, theGlobalVariables, theFormat);
  out << "<div class=\"math\">" << tempS << "</div>";
  theNode.outputString=out.str();
  theNode.ExpressionType=theNode.typePartialFractions;
  return theNode.errorNoError;
}

void ParserNode::CreateDefaultLatexAndPDFfromString
(std::string& theLatexFileString)
{ std::fstream outputFile;
  std::string fileName;
  std::stringstream out;
  fileName.append(this->owner->outputFolderPath);
  fileName.append("output.tex");
  CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(outputFile, fileName, false, true, false);
  outputFile << theLatexFileString;
  out << "A latex/pdf file: <a href=\"" << this->owner->outputFolderDisplayPath << "output.tex\"> output.tex</a>";
  out << ", <a href=\"" << this->owner->outputFolderDisplayPath << "output.pdf\"> output.pdf</a>";
  this->outputString=out.str();
  std::stringstream theCommand;
  theCommand << "pdflatex -output-directory=" << this->owner->outputFolderPath << "   " << fileName ;
  //std::cout << theCommand.str();
  this->owner->SystemCommands.AddObjectOnTop(theCommand.str());
  this->ExpressionType=this->typeString;
}

std::string partFractions::DoTheFullComputationReturnLatexFileString(GlobalVariables& theGlobalVariables, roots& toBePartitioned, PolynomialOutputFormat& theFormat)
{ if (toBePartitioned.size<1)
    return "";
  this->AmbientDimension= toBePartitioned.TheObjects[0].size;
  this->theChambers.AmbientDimension= this->AmbientDimension;
  this->theChambers.theDirections.CopyFromBase(toBePartitioned);
  return this->DoTheFullComputationReturnLatexFileString(theGlobalVariables, theFormat);
}

std::string partFractions::DoTheFullComputationReturnLatexFileString(GlobalVariables& theGlobalVariables, PolynomialOutputFormat& theFormat)
{ this->theChambers.thePauseController.InitComputation();
  //this->theChambers.ReadFromDefaultFile(theGlobalVariables);
  std::stringstream out;
  this->theChambers.SliceTheEuclideanSpace(theGlobalVariables, false);
  this->theChambers.QuickSortAscending();
  this->theChambers.LabelChamberIndicesProperly();
  root tempRoot; tempRoot.MakeZero(this->AmbientDimension);
  tempRoot.MakeZero(this->AmbientDimension);
  this->theChambers.drawOutput(theGlobalVariables.theDrawingVariables, tempRoot, 0);
  this->theChambers.thePauseController.ExitComputation();
  this->initFromRoots(theChambers.theDirections, theGlobalVariables);
  out << "\\documentclass{article}\\usepackage{amsmath, amsfonts, amssymb} \n\\begin{document}\n";
  out << "The vector partition funciton is the number of ways you can represent a vector $(x_1,\\dots, x_n)$ as a non-negative integral linear combination of "
        << " a given set of vectors.  You requested the vector partition function with respect to the set of vectors: " << theChambers.theDirections.ElementToStringGeneric();
  out << "\n\n The corresponding generating function is: " << this->ElementToString(theGlobalVariables, theFormat) << "= (after splitting acording to algorithm)";
  this->split(theGlobalVariables, 0);
  out << this->ElementToString(theGlobalVariables, theFormat);
  out << "Therefore the vector partition function is given by " << this->theChambers.GetNumNonZeroPointers()
        << " quasipolynomials depending on which set of linear inequalities is satisfied (each such set we call ``Chamber'').";
  QuasiPolynomial tempQP;
  std::string tempS;
  for (int i=0; i<this->theChambers.size; i++)
    if (this->theChambers.TheObjects[i]!=0)
    { CombinatorialChamber& currentChamber=*this->theChambers.TheObjects[i];
      this->GetVectorPartitionFunction(tempQP, currentChamber.InternalPoint, theGlobalVariables);
      currentChamber.ElementToInequalitiesString(tempS, this->theChambers, true, false, theFormat);
      out << "\n\nChamber: " << tempS;
      out << "Quasipolynomial: " << tempQP.ElementToString(false, true, theFormat);
    }
  out << "\\end{document}";
  return out.str();
}

int ParserNode::EvaluateVectorPFIndicator
(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ partFractions& currentPF=theNode.thePFs.GetElement();
  roots toBePartitioned; int tempDim;
  theNode.GetRootsEqualDimNoConversionNoEmptyArgument(theArgumentList, toBePartitioned, tempDim);
  std::stringstream out;
  PolynomialOutputFormat theFormat;
//  theFormat.alphabet.TheObjects[0]="t_1";
//  theFormat.alphabet.TheObjects[1]="t_2";
  std::string tempS= currentPF.DoTheFullComputationReturnLatexFileString(theGlobalVariables, toBePartitioned, theFormat);
  theNode.CreateDefaultLatexAndPDFfromString(tempS);
  return theNode.errorNoError;
}

void QuasiPolynomial::AddLatticeShift(const PolynomialRationalCoeff& input, const root& inputShift)
{ root theShift=inputShift;
  this->AmbientLatticeReduced.ReduceVector(theShift);
  int index=this->LatticeShifts.IndexOfObject(theShift);
  if (index==-1)
  { index=this->LatticeShifts.size;
    this->LatticeShifts.AddObjectOnTop(theShift);
    this->valueOnEachLatticeShift.ExpandOnTop(1);
    this->valueOnEachLatticeShift.LastObject()->Nullify(input.NumVars);
  }
  this->valueOnEachLatticeShift.TheObjects[index]+=input;
  if (this->valueOnEachLatticeShift.TheObjects[index].IsEqualToZero())
  { this->LatticeShifts.PopIndexSwapWithLast(index);
    this->valueOnEachLatticeShift.PopIndexSwapWithLast(index);
  }
}


void QuasiPolynomial::Substitution
  (const MatrixLargeRational& mapFromNewSpaceToOldSpace,
   const Lattice& ambientLatticeNewSpace, QuasiPolynomial& output, GlobalVariables& theGlobalVariables)
{ //Format of the substitution.
  //If we want to carry out a substitution in P(y_1, ..., y_m),
  // of the form
  //y_1=a_11 x_1+...+a_1nx_n
  //...
  //y_m=a_m1 x_1+...+a_mnx_n
  // then the mapFromNewSpaceToOldSpace shouldbe the matrix
  //a_11 ... a_1n
  //...
  //a_m1 ... a_mn
  assert(this!=&output);
  assert(mapFromNewSpaceToOldSpace.NumRows==this->GetNumVars());
  assert(ambientLatticeNewSpace.GetDim()==mapFromNewSpaceToOldSpace.NumCols);
  output.AmbientLatticeReduced=ambientLatticeNewSpace;
  output.AmbientLatticeReduced.IntersectWithPreimageOfLattice(mapFromNewSpaceToOldSpace, this->AmbientLatticeReduced, theGlobalVariables);
  roots allRepresentatives, imagesAllRepresentatives;
  bool tempBool=ambientLatticeNewSpace.GetAllRepresentatitves(output.AmbientLatticeReduced, allRepresentatives);
  assert(tempBool);
  mapFromNewSpaceToOldSpace.ActOnRoots(allRepresentatives, imagesAllRepresentatives);
  PolynomialsRationalCoeff theSub;
  theSub.SetSize(this->GetNumVars());
  root tempRoot;
  for (int i=0; i<theSub.size; i++)
  { Polynomial<Rational>& currentPoly=theSub.TheObjects[i];
    mapFromNewSpaceToOldSpace.RowToRoot(i, tempRoot);
    currentPoly.MakeLinPolyFromRootNoConstantTerm(tempRoot);
  }
  PolynomialRationalCoeff tempP;
  for (int i=0; i<this->valueOnEachLatticeShift.size; i++)
  { this->valueOnEachLatticeShift.TheObjects[i].Substitution(theSub, tempP, mapFromNewSpaceToOldSpace.NumCols, (Rational) 1);
    for (int j=0; j<allRepresentatives.size; j++)
      if (imagesAllRepresentatives.TheObjects[j]==this->LatticeShifts.TheObjects[i])
        output.AddLatticeShift(tempP, allRepresentatives.TheObjects[j]);
  }
}

void QuasiPolynomial::Substitution
(const root& inputTranslation, QuasiPolynomial& output, GlobalVariables& theGlobalVariables)
{ //format of the translation. If the starting quasipolynomial was P(y_1, ..., y_n),
  //and the translation has coordinates (t_1, ..., t_n),
  //then the resulting quasipolynomial will be P(x_1-t_1, ..., x_n-t_n)
  PolynomialsRationalCoeff theSub;
  theSub.MakeIdSubstitution(this->GetNumVars(), (Rational) 1);
  for (int i=0; i<theSub.size; i++)
    theSub.TheObjects[i].AddConstant(-inputTranslation.TheObjects[i]);
  PolynomialRationalCoeff tempP;
  output.MakeZeroLatticeZn(this->GetNumVars());
  output.AmbientLatticeReduced=this->AmbientLatticeReduced;
  for (int i=0; i<this->valueOnEachLatticeShift.size; i++)
  { this->valueOnEachLatticeShift.TheObjects[i].Substitution(theSub, tempP, this->GetNumVars(), (Rational) 1);
    output.AddLatticeShift(tempP, this->LatticeShifts.TheObjects[i]+inputTranslation);
  }
}

void QuasiPolynomial::Substitution
  (const MatrixLargeRational& mapFromNewSpaceToOldSpace, const root& inputTranslation,
   const Lattice& ambientLatticeNewSpace, QuasiPolynomial& output, GlobalVariables& theGlobalVariables)
{ QuasiPolynomial tempQP;
  this->Substitution(inputTranslation, tempQP, theGlobalVariables);
  tempQP.Substitution(mapFromNewSpaceToOldSpace, ambientLatticeNewSpace, output, theGlobalVariables);
}

bool QuasiPolynomial::SubstitutionLessVariables
  (const PolynomialsRationalCoeff& theSub, QuasiPolynomial& output, GlobalVariables& theGlobalVariables)const
{ MatrixLargeRational theLatticeSub;
  if (!this->AmbientLatticeReduced.GetHomogeneousSubMatFromSubIgnoreConstantTerms(theSub, theLatticeSub, theGlobalVariables))
    return false;
  MatrixLargeRational theSubLatticeShift;
  output.AmbientLatticeReduced=this->AmbientLatticeReduced;
  if(!output.AmbientLatticeReduced.SubstitutionHomogeneous(theLatticeSub, theGlobalVariables))
    return false;
  theSubLatticeShift.init(theLatticeSub.NumRows,1);
  for (int i=0; i<theSubLatticeShift.NumRows; i++)
    theSub.TheObjects[i].GetConstantTerm(theSubLatticeShift.elements[i][0], (Rational) 0);
  MatrixLargeRational theShiftImage, shiftMatForm;
  output.LatticeShifts.size=0;
  output.valueOnEachLatticeShift.size=0;
  output.valueOnEachLatticeShift.MakeActualSizeAtLeastExpandOnTop(this->LatticeShifts.size);
  output.LatticeShifts.MakeActualSizeAtLeastExpandOnTop(this->LatticeShifts.size);
  root tempRoot;
  PolynomialRationalCoeff tempP;
  for (int i=0; i<this->LatticeShifts.size; i++)
  { shiftMatForm.AssignVectorColumn(this->LatticeShifts.TheObjects[i]);
    shiftMatForm-=theSubLatticeShift;
    if (theLatticeSub.Solve_Ax_Equals_b_ModifyInputReturnFirstSolutionIfExists(theLatticeSub, shiftMatForm, theShiftImage))
    { tempRoot.AssignMatDetectRowOrColumn(theShiftImage);
      this->valueOnEachLatticeShift.TheObjects[i].Substitution(theSub, tempP, tempRoot.size, (Rational) 1);
      output.AddLatticeShift(tempP, tempRoot);
    }
  }
  return true;
}

bool Lattice::SubstitutionHomogeneous
    (const PolynomialsRationalCoeff& theSub, GlobalVariables& theGlobalVariables)
{ MatrixLargeRational matSub;
  if (!this->GetHomogeneousSubMatFromSubIgnoreConstantTerms(theSub, matSub, theGlobalVariables))
    return false;
  return this->SubstitutionHomogeneous(matSub, theGlobalVariables);
}

bool Lattice::GetHomogeneousSubMatFromSubIgnoreConstantTerms
(const PolynomialsRationalCoeff& theSub, MatrixLargeRational& output, GlobalVariables& theGlobalVariables)
{ if (theSub.size<1)
    return false;
  int theTargetDim=theSub.TheObjects[0].NumVars;
  output.init(theSub.size, theTargetDim);
  for (int i=0; i<theSub.size; i++)
  { Polynomial<Rational>& currentPoly=theSub.TheObjects[i];
    if (!currentPoly.IsLinear())
      return false;
    for (int j=0; j<theTargetDim; j++)
      currentPoly.GetCoeffInFrontOfLinearTermVariableIndex(j, output.elements[i][j], (Rational) 0);
  }
  return true;
}

void Lattice::IntersectWithLinearSubspaceGivenByNormal(const root& theNormal)
{ //std::cout << "<br>Starting lattice: " << this->ElementToString(true, false) << "<br>";
  roots startingBasis, resultBasis;
  startingBasis.AssignMatrixRows(this->basisRationalForm);
  //std::cout << "Starting normal: " << theNormal.ElementToString();
  root theScalarProducts;
  theScalarProducts.SetSize(startingBasis.size);
  for (int i=0; i<this->basisRationalForm.NumRows; i++)
    theScalarProducts.TheObjects[i]=root::RootScalarEuclideanRoot(startingBasis.TheObjects[i], theNormal);
  if (theScalarProducts.IsEqualToZero())
    return;
  //std::cout << "<br>the scalar products: " << theScalarProducts.ElementToString();
  int pivotColumnIndex=theScalarProducts.getIndexLastNonZeroCoordinate();
  Rational pivotCoeff=theScalarProducts.TheObjects[pivotColumnIndex];
  theScalarProducts/=-pivotCoeff;
  //std::cout << "<br>the scalar products after scaling: " << theScalarProducts.ElementToString();
  roots eigenSpacePlusOrthogonalComponent;
  eigenSpacePlusOrthogonalComponent.SetSize(theScalarProducts.size);
  for (int i=0; i<theScalarProducts.size; i++)
    if (i!=pivotColumnIndex)
    { root& currentRoot=eigenSpacePlusOrthogonalComponent.TheObjects[i];
      currentRoot.MakeZero(theScalarProducts.size);
      currentRoot.TheObjects[i]=1;
      currentRoot.TheObjects[pivotColumnIndex]=theScalarProducts.TheObjects[i];
    }
  theScalarProducts.ScaleToIntegralMinHeight();
  //std::cout << "<br>the scalar products after scaling to integral: " << theScalarProducts.ElementToString();
  eigenSpacePlusOrthogonalComponent.TheObjects[pivotColumnIndex]=theScalarProducts;
  //std::cout << "<br>The eigenspace before intersection: " << eigenSpacePlusOrthogonalComponent.ElementToString();
  Lattice eigenLattice, theZnLattice;
  eigenLattice.MakeFromRoots(eigenSpacePlusOrthogonalComponent);
  //std::cout << "<br>The eigen-Lattice: " << eigenLattice.ElementToString(true, false);
  theZnLattice.MakeZn(theScalarProducts.size);
  //std::cout << "<br>The Zn-Lattice: " << theZnLattice.ElementToString(true, false);
  theZnLattice.IntersectWithBothOfMaxRank(eigenLattice);
  //std::cout << "<br>Zn intersected with eigen-Lattice: " << theZnLattice.ElementToString(true, false);
  resultBasis.MakeActualSizeAtLeastExpandOnTop(theScalarProducts.size-1);
  root tempRoot, resultRoot; Rational orthogonalComponent;
  for (int i=0; i<theZnLattice.basisRationalForm.NumRows; i++)
  { theZnLattice.basisRationalForm.RowToRoot(i, tempRoot);
    orthogonalComponent=tempRoot.ScalarEuclidean(theScalarProducts)/theScalarProducts.ScalarEuclidean(theScalarProducts);
    tempRoot-=theScalarProducts*orthogonalComponent;
    assert(orthogonalComponent.IsInteger());
    if (!tempRoot.IsEqualToZero())
    { resultRoot.MakeZero(this->GetDim());
      for (int j=0; j<startingBasis.size; j++)
        resultRoot+=startingBasis.TheObjects[j]*tempRoot.TheObjects[j];
      resultBasis.AddObjectOnTop(resultRoot);
    }
  }
  //std::cout << "<br>Resulting basis: " << resultBasis.ElementToString();
  this->MakeFromRoots(resultBasis);
  //std::cout << "<br>Final answer lattice form: " << this->ElementToString(true, false);
}

void Lattice::IntersectWithLinearSubspaceSpannedBy(const roots& theSubspaceBasis)
{ roots theNormals;
  MatrixLargeRational theMat;
  theSubspaceBasis.GetMatRootsToRows(theMat);
  theMat.FindZeroEigenSpace(theNormals);
  this->IntersectWithLinearSubspaceGivenByNormals(theNormals);
}

void Lattice::IntersectWithLinearSubspaceGivenByNormals(const roots& theSubspaceNormals)
{ //std::cout << "<br>********************Debug info for IntersectWithLinearSubspaceGivenByNormals*******************";
  for (int i=0; i<theSubspaceNormals.size; i++)
    this->IntersectWithLinearSubspaceGivenByNormal(theSubspaceNormals.TheObjects[i]);
  //std::cout << "<br>********************End of debug info for IntersectWithLinearSubspaceGivenByNormals*******************";
}

bool Lattice::SubstitutionHomogeneous
 (const MatrixLargeRational& theSub, Lattice& resultIsSubsetOf, GlobalVariables& theGlobalVariables)
{ /*roots preimageBasis;
  preimageBasis.AssignMatrixRows(this->basisRationalForm);
  MatrixLargeRational theSubModifiable, currentBasisVector, oneSolution;
  for (int i=0; i<preimageBasis.size; i++)
  { theSubModifiable=theSub;
    currentBasisVector.AssignVectorColumn(preimageBasis.TheObjects[i]);
    if (theSubModifiable.Solve_Ax_Equals_b_ModifyInputReturnFirstSolutionIfExists(theSubModifiable, currentBasisVector, oneSolution))
    { theSubModifiable=theSub;
      theSubModifiable.fin
    }
  }*/
return false;
}

bool Lattice::SubstitutionHomogeneous
  (const MatrixLargeRational& theSub, GlobalVariables& theGlobalVariables)
{ int targetDim=theSub.NumCols;
  if (theSub.NumRows!=this->GetDim())
    return false;
  //std::cout <<"<br> the sub: " << theSub.ElementToString(true, false) << "<br>";
  int startingDim=this->GetDim();
  MatrixLargeRational theMat, oldBasisTransformed, matRelationBetweenStartingVariables;
  theMat=theSub;
  oldBasisTransformed=this->basisRationalForm;
  oldBasisTransformed.Transpose();
  Selection nonPivotPoints;
  theMat.ComputeDebugString(false, false);
  //std::cout << "<br>the matrices to be transformed: " << theMat.ElementToString(true, false) << "<br>" << oldBasisTransformed.ElementToString(true, false);
  theMat.GaussianEliminationByRows(oldBasisTransformed, nonPivotPoints);
  //std::cout << "<br>afer transformation: " << theMat.ElementToString(true, false) << "<br>" << oldBasisTransformed.ElementToString(true, false);
  if (nonPivotPoints.CardinalitySelection!=0)
    return false;
  int numNonZeroRows=nonPivotPoints.MaxSize;
  int numZeroRows=theMat.NumRows-numNonZeroRows;
  matRelationBetweenStartingVariables.init(numZeroRows, startingDim);
  for (int i=0; i<numZeroRows; i++)
    for (int j=0; j<startingDim; j++)
      matRelationBetweenStartingVariables.elements[i][j]=oldBasisTransformed.elements[i+numNonZeroRows][j];
  roots theEigenSpace;
  matRelationBetweenStartingVariables.FindZeroEigenSpace(theEigenSpace, theGlobalVariables);
  //std::cout << "<br>matRelationBetweenStartingVariables" <<  matRelationBetweenStartingVariables.ElementToString(true, false);
  for (int i=0; i<theEigenSpace.size; i++)
    theEigenSpace.TheObjects[i].ScaleToIntegralMinHeight();
  //std::cout << "the basis: " << theEigenSpace.ElementToString();
  oldBasisTransformed.ActOnRoots(theEigenSpace);
  //std::cout << "<br>the basis transformed: " << theEigenSpace.ElementToString();
  this->basisRationalForm.init(targetDim, targetDim);
  for (int i=0; i<targetDim; i++)
    for (int j=0; j<targetDim; j++)
      this->basisRationalForm.elements[i][j]=theEigenSpace.TheObjects[i].TheObjects[j];
  this->basisRationalForm.GetMatrixIntWithDen(this->basis, this->Den);
  this->Reduce();
  //std::cout << "<br><br>and the sub result is: <br>" << this->ElementToString(true, false);
  return true;
}

void QuasiPolynomial::operator*=(const Rational& theConst)
{ if (theConst.IsEqualToZero())
  { this->valueOnEachLatticeShift.size=0;
    this->LatticeShifts.size=0;
    return;
  }
  for (int i=0; i<this->valueOnEachLatticeShift.size; i++)
    this->valueOnEachLatticeShift.TheObjects[i]*=theConst;
}

int ParserNode::EvaluateReadFromFile(GlobalVariables& theGlobalVariables)
{ std::fstream input;
  std::string theFileName, tempS;
  theFileName=this->owner->outputFolderPath;
  theFileName.append("output.txt");
  CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(input, theFileName, false, false, false);
  input.seekg(0);
  std::stringstream outString;
  int candidateExpressionType;
  input >> tempS >> candidateExpressionType;
  //std::cout << "header: " << tempS << " expression type: " << candidateExpressionType;
  if (tempS!="ExpressionType:")
  { outString << "missing header of file " << theFileName << " header read is: " << tempS;
    this->outputString=outString.str();;
    return this->SetError(this->errorBadFileFormat);
  }
  switch (candidateExpressionType)
  { case ParserNode::typeCone:
      if(!this->theCone.GetElement().ReadFromFile(input, theGlobalVariables))
        return this->SetError(this->errorBadFileFormat);
      this->outputString=this->theCone.GetElement().ElementToString(false, true);
      break;
    case ParserNode::typeQuasiPolynomial:
      if(!this->theQP.GetElement().ReadFromFile(input, theGlobalVariables))
        return this->SetError(this->errorBadFileFormat);
      this->outputString=this->theQP.GetElement().ElementToString(true, false);
      break;
    case ParserNode::typeLattice:
      if(!this->theLattice.GetElement().ReadFromFile(input, theGlobalVariables))
        return this->SetError(this->errorBadFileFormat);
      this->outputString=this->theLattice.GetElement().ElementToString(true, false);
      break;
    default: return this->SetError(this->errorBadFileFormat);
  }
  this->ExpressionType=candidateExpressionType;
  return this->errorNoError;

}

int ParserNode::EvaluateWriteToFile(GlobalVariables& theGlobalVariables)
{ List<int> argumentList;
  this->ExtractArgumentList(argumentList);
  if (argumentList.size!=1)
    return this->SetError(this->errorBadOrNoArgument);
  ParserNode& argument=this->owner->TheObjects[argumentList.TheObjects[0]];
  std::fstream output;
  std::string theFileName;
  theFileName=this->owner->outputFolderPath;
  theFileName.append("output.txt");
  //std::cout  <<theFileName;
  CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(output, theFileName, false, true, false);
  std::stringstream outString;
  output << "ExpressionType: " << argument.ExpressionType << "\n";
  switch (argument.ExpressionType)
  { case ParserNode::typeCone:
      argument.theCone.GetElement().WriteToFile(output, theGlobalVariables);
      break;
    case ParserNode::typeQuasiPolynomial:
      argument.theQP.GetElement().WriteToFile(output, theGlobalVariables);
      break;
    case ParserNode::typeLattice:
      argument.theLattice.GetElement().WriteToFile(output, theGlobalVariables);
      break;
    default: return this->SetError(this->errorDunnoHowToDoOperation);
  }
  outString << "A latex/pdf file: <a href=\"" <<  this->owner->outputFolderDisplayPath << "output.txt\"> output.txt</a>";
  this->outputString=outString.str();
  this->ExpressionType=this->typeFile;
  return this->errorNoError;
}

void Cone::WriteToFile
  (std::fstream& output, GlobalVariables& theGlobalVariables)
{ output << "Cone( ";
  for (int i=0; i<this->Normals.size; i++)
  { output << "(";
    for (int j=0; j<this->Normals.TheObjects[i].size; j++)
    { output << this->Normals.TheObjects[i].TheObjects[j].ElementToString();
      if (j!=this->Normals.TheObjects[i].size-1)
        output << ",";
    }
    output << ")";
    if (i!=this->Normals.size-1)
      output << ", ";
  }
  output << " ) ";
}

bool Cone::ReadFromFile
  (std::fstream& input, GlobalVariables& theGlobalVariables)
{ std::string tempS, rootString;
  input >> tempS;
  roots& buffer=theGlobalVariables.rootsConeWriteToFileBuffer;
  buffer.size=0;
  root tempRoot;
  if (tempS!="Cone(")
  { std::cout << "tempS was instead " << tempS;
    return false;
  }
  for (input >> tempS; tempS!=")" && tempS!=""; input >> tempS)
  { tempRoot.AssignString(tempS);
    buffer.AddObjectOnTop(tempRoot);
    //std::cout << "vector input " << tempS << " read as " << tempRoot.ElementToString();
  }
  if (buffer.size<1)
    return false;
  int theDim=buffer.TheObjects[0].size;
  for (int i=1; i<buffer.size; i++)
    if (buffer.TheObjects[i].size!=theDim)
      return false;
  bool tempBool=this->CreateFromNormals(buffer, theGlobalVariables);
  assert(tempBool);
  return tempBool;
}

void GeneralizedVermaModuleCharacters::WriteToFile
  (std::fstream& output, GlobalVariables& theGlobalVariables)
{ output << "ComputationPhase: " << this->computationPhase << "\n";
  output << "NumProcessedConesParam: " << this->NumProcessedConesParam << "\n";
  output << "NumProcessedExtremaEqualOne: " << this->NumProcessedExtremaEqualOne << "\n";
  output << "NormalConstAdjustment: ";
  this->NonIntegralOriginModification.WriteToFile(output);
  output << "\n";
  theGlobalVariables.theIndicatorVariables.String1NeedsRefresh=true;
  theGlobalVariables.theIndicatorVariables.ProgressReportString1="Writing small data... ";
  theGlobalVariables.MakeReport();
//  this->allParamSubChambersRepetitionsAllowed.WriteToFile(output, theGlobalVariables);
  this->allParamSubChambersRepetitionsAllowedConeForm.WriteToFile(output, theGlobalVariables);
//  this->ExtremeQPsParamSubchambers.WriteToFile(output, theGlobalVariables);
  this->GmodKnegativeWeights.WriteToFile(output, theGlobalVariables);
  this->theLinearOperators.WriteToFile(output);
  this->theLinearOperatorsExtended.WriteToFile(output);
  this->PreimageWeylChamberLargerAlgebra.WriteToFile(output, theGlobalVariables);
  this->PreimageWeylChamberSmallerAlgebra.WriteToFile(output, theGlobalVariables);
  theGlobalVariables.theIndicatorVariables.ProgressReportString1="Writing QP's non-subbed... ";
  theGlobalVariables.MakeReport();
  this->theQPsNonSubstituted.WriteToFile(output, theGlobalVariables);
  theGlobalVariables.theIndicatorVariables.ProgressReportString1="Writing QP's subbed... ";
  theGlobalVariables.MakeReport();
  this->theQPsSubstituted.WriteToFile(output, theGlobalVariables);
  theGlobalVariables.theIndicatorVariables.ProgressReportString1="Writing small data... ";
  theGlobalVariables.MakeReport();
  this->theMultiplicities.WriteToFile(output, theGlobalVariables, this->UpperLimitChambersForDebugPurposes);
  this->theMultiplicitiesExtremaCandidates.WriteToFile(output, theGlobalVariables);
  this->theCoeffs.WriteToFile(output);
  this->theTranslations.WriteToFile(output, theGlobalVariables);
  this->theTranslationsProjected.WriteToFile(output, theGlobalVariables);
  this->thePfs.WriteToFile(output, theGlobalVariables);
//  this->paramSubChambers.WriteToFile(output, theGlobalVariables);
//  this->nonParamVertices.WriteToFile(output, theGlobalVariables);
  theGlobalVariables.theIndicatorVariables.ProgressReportString1="Writing param chamber complex... ";
  theGlobalVariables.MakeReport();
  this->projectivizedParamComplex.WriteToFile(output, theGlobalVariables);
  this->projectivizedChamber.WriteToFile(output, theGlobalVariables, this->UpperLimitChambersForDebugPurposes);
  this->projectivezedChambersSplitByMultFreeWalls.WriteToFile(output, theGlobalVariables);
  theGlobalVariables.theIndicatorVariables.ProgressReportString1="Writing param chamber extrema subcones... ";
  theGlobalVariables.MakeReport();
  this->projectivizedExtremaCones.WriteToFile(output, theGlobalVariables);
  theGlobalVariables.theIndicatorVariables.ProgressReportString1="Writing the extrema... ";
  theGlobalVariables.MakeReport();
  this->theExtrema.WriteToFile(output, theGlobalVariables);

  theGlobalVariables.theIndicatorVariables.ProgressReportString1="Writing extrema equals one cones... ";
  theGlobalVariables.MakeReport();
  this->projectivizedExtremaEqualsOneCones.WriteToFile(output, theGlobalVariables);

  theGlobalVariables.theIndicatorVariables.ProgressReportString1="Writing to file done... ";
  theGlobalVariables.MakeReport();
}

void QuasiPolynomial::WriteToFile(std::fstream& output, GlobalVariables& theGlobalVariables)
{ this->valueOnEachLatticeShift.WriteToFile(output, theGlobalVariables);
  this->AmbientLatticeReduced.WriteToFile(output, theGlobalVariables);
  this->LatticeShifts.WriteToFile(output, theGlobalVariables);
}

bool QuasiPolynomial::ReadFromFile(std::fstream& input, GlobalVariables& theGlobalVariables)
{ this->valueOnEachLatticeShift.ReadFromFile(input, theGlobalVariables);
  this->AmbientLatticeReduced.ReadFromFile(input, theGlobalVariables);
  this->LatticeShifts.ReadFromFile(input, theGlobalVariables);
  this->buffers=&theGlobalVariables;
  return true;
}

void Lattice::WriteToFile(std::fstream& output, GlobalVariables& theGlobalVariables)
{ output << "Lattice: ";
  this->basisRationalForm.WriteToFile(output);
}

bool Lattice::ReadFromFile(std::fstream& input, GlobalVariables& theGlobalVariables)
{ std::string tempS;
  input >> tempS;
  if (tempS!="Lattice:")
  { assert(false);
    return false;
  }
  if(!this->basisRationalForm.ReadFromFile(input))
    return false;
  this->basisRationalForm.GetMatrixIntWithDen(this->basis, this->Den);
  return true;
}

bool GeneralizedVermaModuleCharacters::ReadFromFileNoComputationPhase
  (std::fstream& input, GlobalVariables& theGlobalVariables)
{ std::string tempS;
  input >> tempS >> this->NumProcessedConesParam;
  input >> tempS >> this->NumProcessedExtremaEqualOne;
  input >> tempS;
  if (tempS!="NormalConstAdjustment:")
  { assert(false);
    return false;
  }
  this->NonIntegralOriginModification.ReadFromFile(input);
  this->theParser.theHmm.MakeG2InB3(this->theParser, theGlobalVariables);
  theGlobalVariables.theIndicatorVariables.String1NeedsRefresh=true;
//  this->allParamSubChambersRepetitionsAllowed.ReadFromFile(input, theGlobalVariables);
  theGlobalVariables.theIndicatorVariables.ProgressReportString1="Loading param subchambers cone form... ";
  theGlobalVariables.MakeReport();
  this->allParamSubChambersRepetitionsAllowedConeForm.ReadFromFile(input, theGlobalVariables);
  theGlobalVariables.theIndicatorVariables.ProgressReportString1="Loading more pieces of data... ";
  theGlobalVariables.MakeReport();
  this->GmodKnegativeWeights.ReadFromFile(input, theGlobalVariables);
  this->theLinearOperators.ReadFromFile(input, theGlobalVariables);
  this->theLinearOperatorsExtended.ReadFromFile(input, theGlobalVariables);
  this->PreimageWeylChamberLargerAlgebra.ReadFromFile(input, theGlobalVariables);
  this->PreimageWeylChamberSmallerAlgebra.ReadFromFile(input, theGlobalVariables);
  this->theQPsNonSubstituted.ReadFromFile(input, theGlobalVariables);
  this->theQPsSubstituted.ReadFromFile(input, theGlobalVariables);
  this->theMultiplicities.ReadFromFile(input, theGlobalVariables);
  this->theMultiplicitiesExtremaCandidates.ReadFromFile(input, theGlobalVariables);
  this->theCoeffs.ReadFromFile(input);
  this->theTranslations.ReadFromFile(input, theGlobalVariables);
  this->theTranslationsProjected.ReadFromFile(input, theGlobalVariables);
  theGlobalVariables.theIndicatorVariables.ProgressReportString1="Loading partial fractions... ";
  theGlobalVariables.MakeReport();
  this->thePfs.ReadFromFile(input, theGlobalVariables);
  theGlobalVariables.theIndicatorVariables.ProgressReportString1="Loading projectivized param complex... ";
  theGlobalVariables.MakeReport();
  this->projectivizedParamComplex.ReadFromFile(input, theGlobalVariables);
  this->projectivizedChamber.ReadFromFile(input, theGlobalVariables);
  this->projectivezedChambersSplitByMultFreeWalls.ReadFromFile(input, theGlobalVariables);
  theGlobalVariables.theIndicatorVariables.ProgressReportString1="Loading param chamber extrema subcones...";
  theGlobalVariables.MakeReport();
  this->projectivizedExtremaCones.ReadFromFile(input, theGlobalVariables);
  theGlobalVariables.theIndicatorVariables.ProgressReportString1="Loading the extrema...";
  theGlobalVariables.MakeReport();
  this->theExtrema.ReadFromFile(input, theGlobalVariables);
  theGlobalVariables.theIndicatorVariables.ProgressReportString1="Loading the extrema equals one cones...";
  theGlobalVariables.MakeReport();
  this->projectivizedExtremaEqualsOneCones.ReadFromFile(input, theGlobalVariables);
  theGlobalVariables.theIndicatorVariables.ProgressReportString1="Loading complete... ";
  theGlobalVariables.MakeReport();
  return true;
}

void ConeComplex::WriteToFile
  (std::fstream& output, GlobalVariables& theGlobalVariables, int UpperLimit)
{ this->List<Cone>::WriteToFile(output, theGlobalVariables, UpperLimit);
  output << "IndexLowestNonRefined: " << this->indexLowestNonRefinedChamber << "\n";
  this->splittingNormals.WriteToFile(output);
}

bool ConeComplex::ReadFromFile
  (std::fstream& input, GlobalVariables& theGlobalVariables)
{ if(!this->List<Cone>::ReadFromFile(input, theGlobalVariables))
  { assert(false);
    return false;
  }
  std::string tempS;
  input >> tempS >> this->indexLowestNonRefinedChamber;
  if (tempS!="IndexLowestNonRefined:")
  { assert(false);
    return false;
  }
  return this->splittingNormals.ReadFromFile(input);
}

void WeylGroup::GetIntegralLatticeInSimpleCoordinates(Lattice& output)
{ output.basisRationalForm=this->CartanSymmetric;
  output.basisRationalForm.Invert();
  output.basisRationalForm.GetMatrixIntWithDen(output.basis, output.Den);
  output.Reduce();
}

int ParserNode::EvaluatePrintRootSystem
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  WeylGroup& theWeyl=theNode.owner->theHmm.theRange.theWeyl;
  out << "<br>Symmetric Cartan matrix in Bourbaki order:<br><div class=\"math\">" << theNode.owner->theHmm.theRange.theWeyl.CartanSymmetric.ElementToString(false, true) << "</div>";
  Rational tempRat;
  MatrixLargeRational tempMat;
  tempMat = theWeyl.CartanSymmetric;
  tempMat.ComputeDeterminantOverwriteMatrix(tempRat);
  out  << "The determinant of the symmetric Cartan matrix is: " << tempRat.ElementToString();
  out << "<br>Root system:";
  for (int i=0; i<theWeyl.RootSystem.size; i++)
  { root& current=theNode.owner->theHmm.theRange.theWeyl.RootSystem.TheObjects[i];
    out << "<br>" << current.ElementToString();
  }
  theNode.outputString=out.str();
  theNode.ExpressionType=theNode.typeString;
  return theNode.errorNoError;
}

int ParserNode::EvaluateIntersectLatticeWithSubspaces(GlobalVariables& theGlobalVariables)
{ List<int> argumentList;
  this->ExtractArgumentList(argumentList);
  //std::stringstream out;
  if (argumentList.size<2)
    return this->SetError(this->errorBadOrNoArgument);
  ParserNode& latticeNode=this->owner->TheObjects[argumentList.TheObjects[0]];
  if (!latticeNode.ConvertToType(this->typeLattice, theGlobalVariables))
    return this->SetError(this->errorBadOrNoArgument);
  roots tempRoots;
  tempRoots.SetSize(argumentList.size-1);
  this->theLattice.GetElement()=latticeNode.theLattice.GetElement();
  int theDim=this->theLattice.GetElement().GetDim();
  for (int i=1; i<argumentList.size; i++)
  { if (!this->owner->TheObjects[argumentList.TheObjects[i]].GetRootRational(tempRoots.TheObjects[i-1], theGlobalVariables))
      return this->SetError(this->errorBadOrNoArgument);
    if (tempRoots.TheObjects[i-1].size!=theDim)
      return this->SetError(this->errorDimensionProblem);
  }
  this->theLattice.GetElement().IntersectWithLinearSubspaceSpannedBy(tempRoots);
  //this->outputString=out.str();
  this->ExpressionType=this->typeLattice;
  return this->errorNoError;
}

template<class CoefficientType>
void Vectors<CoefficientType>::IntersectTwoLinSpaces
  (const Vectors<CoefficientType>& firstSpace, const Vectors<CoefficientType>& secondSpace,
   Vectors<CoefficientType>& output, const CoefficientType& theRingZero,
   GlobalVariables& theGlobalVariables)
{ //std::cout << "<br>*****Debugging Intersection linear spaces: ";
  //std::cout << "<br>input first space: " << firstSpace.ElementToString();
  //std::cout << "<br>input second space: " << secondSpace.ElementToString();
  Vectors<CoefficientType> firstReduced, secondReduced;
  Selection tempSel;
  firstSpace.SelectABasis(firstReduced, theRingZero, tempSel, theGlobalVariables);
  secondSpace.SelectABasis(secondReduced, theRingZero, tempSel, theGlobalVariables);
  //std::cout << "<br>first selected basis: " << firstSpace.ElementToString();
  //std::cout << "<br>second selected basis: " << secondSpace.ElementToString();
  if (firstReduced.size==0 || secondReduced.size==0)
  { output.size=0;
    return;
  }
  int theDim=firstReduced.TheObjects[0].size;
  Matrix<CoefficientType> theMat;
  theMat.init(theDim, firstReduced.size+secondReduced.size);
  for (int i=0; i<theDim; i++)
  { for (int j=0; j<firstReduced.size; j++)
      theMat.elements[i][j]=firstReduced.TheObjects[j].TheObjects[i];
    for (int j=0; j<secondReduced.size; j++)
    { theMat.elements[i][firstReduced.size+j]=theRingZero;
      theMat.elements[i][firstReduced.size+j]-=secondReduced.TheObjects[j].TheObjects[i];
    }
  }
  Matrix<CoefficientType> matEmpty;
  //std::cout << "<br>The matrix before the gaussian elimination:" << theMat.ElementToString(true, false);
  theMat.GaussianEliminationByRows(matEmpty, tempSel);
  //std::cout << "<br>The matrix after the gaussian elimination:" << theMat.ElementToString(true, false);
  output.MakeActualSizeAtLeastExpandOnTop(tempSel.CardinalitySelection);
  output.size=0;
  Vector<CoefficientType> nextIntersection;
  for(int i=0; i<tempSel.CardinalitySelection; i++)
  { int currentIndex=tempSel.elements[i];
    //std::cout << "<br>current pivot index : " << currentIndex;
    assert(currentIndex>=firstReduced.size);
    nextIntersection.MakeZero(theDim, theRingZero);
    for (int j=0; j<firstReduced.size; j++)
      if (!tempSel.selected[j])
        nextIntersection+=firstReduced.TheObjects[j]*theMat.elements[j][currentIndex];
    output.AddObjectOnTop(nextIntersection);
  }
  //std::cout << "<br> final output: " << output.ElementToString();
  //std::cout << "<br>******************End of debugging linear space intersections";
}

int ParserNode::EvaluateIntersectLatticeWithPreimageLattice
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ roots theLinearMap;
  Lattice& firstLattice=theNode.owner->TheObjects[theArgumentList.TheObjects[0]].theLattice.GetElement();
  Lattice& secondLattice=theNode.owner->TheObjects[theArgumentList.TheObjects[1]].theLattice.GetElement();
  List<int> theMapTempList;
  theMapTempList.AddObjectOnTop(theArgumentList.TheObjects[2]);
  int theDim;
  if (!theNode.owner->TheObjects[theArgumentList.TheObjects[2]].GetRootsEqualDimNoConversionNoEmptyArgument(theMapTempList, theLinearMap, theDim))
    return theNode.SetError(theNode.errorDimensionProblem);
  MatrixLargeRational theLinearMapMat;
  theLinearMapMat.AssignRootsToRowsOfMatrix(theLinearMap);
  if(theLinearMapMat.NumCols!=firstLattice.GetDim() || theLinearMapMat.NumRows!=secondLattice.GetDim())
    return theNode.SetError(theNode.errorDimensionProblem);
  //theLinearMapMat.Transpose();

  theNode.theLattice.GetElement()=firstLattice;
  theNode.theLattice.GetElement().IntersectWithPreimageOfLattice(theLinearMapMat, secondLattice, theGlobalVariables);
  theNode.ExpressionType=theNode.typeLattice;
  return theNode.errorNoError;
}

int ParserNode::EvaluateSubstitutionInQuasipolynomial(GlobalVariables& theGlobalVariables)
{ List<int> argumentList;
  this->ExtractArgumentList(argumentList);
  if (argumentList.size!=3)
    return this->SetError(this->errorBadOrNoArgument);
  roots theLinearMap;
  root tempRoot, theTranslation;
  ParserNode& QPNode=this->owner->TheObjects[argumentList.TheObjects[0]];
  ParserNode& mapNode=this->owner->TheObjects[argumentList.TheObjects[1]];
  ParserNode& translationNode=this->owner->TheObjects[argumentList.TheObjects[2]];

  if (!QPNode.ConvertToType(this->typeQuasiPolynomial, theGlobalVariables))
    return this->SetError(this->errorBadOrNoArgument);
  if (!translationNode.GetRootRational(theTranslation, theGlobalVariables))
    return this->SetError(this->errorBadOrNoArgument);
  int theDim=theTranslation.size;
  if (theDim!=QPNode.theQP.GetElement().GetNumVars())
    return this->SetError(this->errorDimensionProblem);
  if (mapNode.GetRootRational(tempRoot, theGlobalVariables))
    theLinearMap.AddObjectOnTop(tempRoot);
  else
    if (mapNode.ConvertToType(this->typeArray, theGlobalVariables))
      for (int i=0; i<mapNode.children.size; i++)
      { ParserNode& currentNode=this->owner->TheObjects[mapNode.children.TheObjects[i]];
        if (!currentNode.GetRootRational(tempRoot, theGlobalVariables))
          return this->SetError(this->errorBadOrNoArgument);
        if (theDim!=tempRoot.size)
          return this->SetError(this->errorDimensionProblem);
        theLinearMap.AddObjectOnTop(tempRoot);
      }
    else
      return this->SetError(this->errorBadOrNoArgument);
  MatrixLargeRational theLinearMapMat;
  theLinearMapMat.AssignRootsToRowsOfMatrix(theLinearMap);
  theLinearMapMat.Transpose();
  QuasiPolynomial& currentQP=QPNode.theQP.GetElement();
  if (currentQP.GetNumVars()!=theLinearMapMat.NumRows)
    return this->SetError(this->errorDimensionProblem);
  Lattice AmbientLattice;
  AmbientLattice.MakeZn(theLinearMapMat.NumCols);
  currentQP.Substitution(theLinearMapMat, theTranslation, AmbientLattice, this->theQP.GetElement(), theGlobalVariables);
  this->ExpressionType=this->typeQuasiPolynomial;
  this->outputString=this->theQP.GetElement().ElementToString(true, false);
  return this->errorNoError;
}

void PolynomialsRationalCoeff::MakeLinearSubbedVarsCorrespondToMatRows(MatrixLargeRational& theMat, root& theConstants)
{ MatrixLargeRational tempMat;
  tempMat=theMat;
  tempMat.Transpose();
  tempMat.Resize(tempMat.NumRows+1, tempMat.NumCols, true);
  for (int i=0; i<theConstants.size; i++)
    tempMat.elements[tempMat.NumRows-1][i]=theConstants.TheObjects[i];
  this->MakeLinearSubConstTermsLastRow(tempMat);
}

void PolynomialsRationalCoeff::MakeLinearSubConstTermsLastRow(MatrixLargeRational& theMat)
{ this->SetSize(theMat.NumCols);
  Monomial<Rational> tempM;
  for (int i=0; i<this->size; i++)
  { this->TheObjects[i].Nullify((int)theMat.NumRows-1);
    for (int j=0; j<theMat.NumRows-1; j++)
    { tempM.init((int)theMat.NumRows-1);
      tempM.degrees[j]=1;
      tempM.Coefficient.Assign(theMat.elements[j][i]);
      this->TheObjects[i].AddMonomial(tempM);
    }
    this->TheObjects[i].AddConstant(theMat.elements[theMat.NumRows-1][i]);
  }
}

void Cone::IntersectAHyperplane
  (root& theNormal, Cone& outputConeLowerDim, GlobalVariables& theGlobalVariables)
{ assert(!theNormal.IsEqualToZero());
  int theDimension=theNormal.size;
  MatrixLargeRational tempMat, theEmbedding, theProjection;
  tempMat.AssignVectorRow(theNormal);
  roots theBasis;
  tempMat.FindZeroEigenSpace(theBasis);
  assert(theBasis.size==theNormal.size-1);
  theEmbedding.AssignRootsToRowsOfMatrix(theBasis);
  theEmbedding.Transpose();
  theBasis.AddObjectOnTop(theNormal);
  roots tempRoots, tempRoots2;
  tempRoots.MakeEiBasis(theDimension);
  tempRoots.GetCoordsInBasis(theBasis, tempRoots2, theGlobalVariables);
  theProjection.AssignRootsToRowsOfMatrix(tempRoots2);
  theProjection.Transpose();
  theProjection.Resize(theDimension-1, theDimension, false);
  roots newNormals=this->Normals;
  theProjection.ActOnRoots(newNormals);
  bool tempBool=outputConeLowerDim.CreateFromNormals(newNormals, theGlobalVariables);
  assert(!tempBool);

}

int ParserNode::EvaluateIntersectHyperplaneByACone(GlobalVariables& theGlobalVariables)
{ List<int> argumentList;
  this->ExtractArgumentList(argumentList);
  if (argumentList.size!=2)
    return this->SetError(this->errorBadOrNoArgument);
  ParserNode& coneNode=this->owner->TheObjects[argumentList.TheObjects[0]];
  ParserNode& hyperplaneNode=this->owner->TheObjects[argumentList.TheObjects[1]];
  if (!coneNode.ConvertToType(this->typeCone, theGlobalVariables))
    return this->SetError(this->errorBadOrNoArgument);
  root tempRoot;
  if (!hyperplaneNode.GetRootRational(tempRoot, theGlobalVariables))
    return this->SetError(this->errorBadOrNoArgument);
  if (tempRoot.size!=coneNode.theCone.GetElement().GetDim())
    return this->SetError(this->errorDimensionProblem);
  Cone& currentCone= coneNode.theCone.GetElement();
  currentCone.IntersectAHyperplane(tempRoot, this->theCone.GetElement(), theGlobalVariables);
  this->outputString=this->theCone.GetElement().ElementToString(false, true);
  return this->errorNoError;
}

bool Cone::GetRootFromLPolyConstantTermGoesToLastVariable
(PolynomialRationalCoeff& inputLPoly, root& output)
{ if (!inputLPoly.IsLinear())
    return false;
  output.MakeZero(inputLPoly.NumVars+1);
  for (int i=0; i<inputLPoly.size; i++)
  { int theIndex;
    if (inputLPoly.TheObjects[i].::Monomial<Rational>::IsOneLetterFirstDegree(theIndex))
      output.TheObjects[theIndex]=inputLPoly.TheObjects[i].Coefficient;
    else
      *output.LastObject()=inputLPoly.TheObjects[i].Coefficient;
  }
  return true;
}

bool Cone::SolveLPolyEqualsZeroIAmProjective
  ( PolynomialRationalCoeff& inputLPoly,
   Cone& outputCone, GlobalVariables& theGlobalVariables
   )
{ root theNormal;
  if (!this->GetRootFromLPolyConstantTermGoesToLastVariable(inputLPoly, theNormal))
    return false;
  this->IntersectAHyperplane(theNormal, outputCone, theGlobalVariables);
  return true;
}

bool Cone::SolveLQuasiPolyEqualsZeroIAmProjective
  ( QuasiPolynomial& inputLQP,
   List<Cone>& outputConesOverEachLatticeShift, GlobalVariables& theGlobalVariables
   )
{ outputConesOverEachLatticeShift.SetSize(inputLQP.LatticeShifts.size);
  bool result=true;
  for (int i=0; i<inputLQP.LatticeShifts.size; i++)
    result=result && this->SolveLPolyEqualsZeroIAmProjective(inputLQP.valueOnEachLatticeShift.TheObjects[i], outputConesOverEachLatticeShift.TheObjects[i], theGlobalVariables);
  return result;
}

bool ParserFunctionArgumentTree::ConvertOneArgumentIndex
  (ParserNode& theNode, int theIndex, int& lowestNestedIndexNonExplored, GlobalVariables& theGlobalVariables)
{ int theType=this->functionArguments.TheObjects[theIndex];
  if (theType==theNode.typeDots)
  { int oldlowestNestedIndexNonExplored=lowestNestedIndexNonExplored-1;
    return this->ConvertOneArgumentIndex(theNode, theIndex-1, oldlowestNestedIndexNonExplored, theGlobalVariables);
  }
  if (theType==theNode.typeArray)
  { List<int> tempList;
    lowestNestedIndexNonExplored++;
    return this->nestedArgumentsOfArguments.TheObjects[lowestNestedIndexNonExplored-1].ConvertArguments(theNode, tempList, theGlobalVariables);
  }
  return theNode.ConvertToType(theType, theGlobalVariables);
}

bool ParserFunctionArgumentTree::ConvertArguments
  (ParserNode& theNode, List<int>& outputArgumentIndices, GlobalVariables& theGlobalVariables)
{ if (theNode.ExpressionType==theNode.typeArray)
    outputArgumentIndices=theNode.children;
  else
  { outputArgumentIndices.size=0;
    outputArgumentIndices.AddObjectOnTop(theNode.indexInOwner);
  }
  int argumentCounter=0;
  int lowestNestedIndexNonExplored=0;
  List<int> tempList;
  for (int i=0; i<this->functionArguments.size; i++)
    if (this->functionArguments.TheObjects[i]==theNode.typeDots)
      while(argumentCounter<outputArgumentIndices.size)
      { ParserNode& currentNode=theNode.owner->TheObjects[outputArgumentIndices.TheObjects[argumentCounter]];
        if (!this->ConvertOneArgumentIndex(currentNode, i, lowestNestedIndexNonExplored, theGlobalVariables))
          break;
        else
          argumentCounter++;
      }
    else
    { if (argumentCounter<outputArgumentIndices.size)
      { ParserNode& currentNode=theNode.owner->TheObjects[outputArgumentIndices.TheObjects[argumentCounter]];
        if (!this->ConvertOneArgumentIndex(currentNode, i, lowestNestedIndexNonExplored, theGlobalVariables))
          return false;
        argumentCounter++;
      }
      else
        return false;
    }
  if (argumentCounter<outputArgumentIndices.size)
    return false;
  return true;
}

int ParserFunction::CallMe
  (ParserNode& theNode, GlobalVariables& theGlobalVariables)
{ assert(this->theActualFunction!=0);
  List<int> argumentsIndices;
  if (theNode.Operation==Parser::tokenFunction)
  { assert(theNode.children.size>0);
    ParserNode& argumentNode=theNode.owner->TheObjects[theNode.children.TheObjects[0]];
    if(!this->theArguments.ConvertArguments(argumentNode, argumentsIndices, theGlobalVariables))
      return theNode.SetError(theNode.errorBadOrNoArgument);
  } else
  { assert(theNode.Operation==Parser::tokenFunctionNoArgument);
  }
  return this->theActualFunction(theNode, argumentsIndices, theGlobalVariables);
}

bool ParserFunction::MakeMe
  (const std::string& theFunctionName, const std::string& theFunctionArguments, const std::string& theFunctionDescription, const std::string& theExample, int (*inputFunctionAddress)(ParserNode& theNode, List<int>& theArguments, GlobalVariables& theGlobalVariables))
{ if(!this->theArguments.MakeFunctionArgumentFromString(theFunctionArguments))
    return false;
  this->functionDescription=theFunctionDescription;
  this->functionName=theFunctionName;
  this->theActualFunction=inputFunctionAddress;
  this->example=theExample;
  return true;
}

std::string ParserFunctionArgumentTree::GetArgumentStringFromToken(int theArgument)
{ switch(theArgument)
  { case ParserNode::typeRational: return "Rational";
    case ParserNode::typeQuasiPolynomial: return "QP";
    case ParserNode::typeIntegerOrIndex: return "Integer";
    case ParserNode::typePoly: return "Polynomial";
    case ParserNode::typeCone: return "Cone";
    case ParserNode::typeRationalFunction: return "RF";
    case ParserNode::typePartialFractions: return "PF";
    case ParserNode::typeLattice: return "Lattice";
    case ParserNode::typeDots: return "...";
    case ParserNode::typeArray: return "";
    default: return "Error";
  }
}

int ParserFunctionArgumentTree::GetTokenFromArgumentStringNoSpaces
(const std::string& theArgument)
{ if (theArgument=="Rational")
    return ParserNode::typeRational;
  if (theArgument=="QP")
    return ParserNode::typeQuasiPolynomial;
  if (theArgument=="Polynomial")
    return ParserNode::typePoly;
  if (theArgument=="Cone")
    return ParserNode::typeCone;
  if (theArgument=="Integer")
    return ParserNode::typeIntegerOrIndex;
  if (theArgument=="PF")
    return ParserNode::typePartialFractions;
  if (theArgument=="Lattice")
    return ParserNode::typeLattice;
  if (theArgument=="RF")
    return ParserNode::typeRationalFunction;
  if (theArgument=="...")
    return ParserNode::typeDots;
  return -1;
}

bool ParserFunctionArgumentTree::MakeFunctionArgumentFromString
(const std::string& theArgumentList)
{ unsigned int currentChar=0;
  return this->MakeFromString(currentChar, theArgumentList);
}

bool ParserFunctionArgumentTree::MakeFromString
(unsigned int& currentChar, const std::string& theArgumentList)
{ std::string currentArgument;
  this->functionArguments.size=0;
  this->nestedArgumentsOfArguments.size=0;
  for (; currentChar<theArgumentList.size(); currentChar++)
    if (theArgumentList[currentChar]=='(')
      break;
  currentChar++;
  for (; currentChar<theArgumentList.size(); currentChar++)
  { if (theArgumentList[currentChar]==',' || theArgumentList[currentChar]==')')
    { if (currentArgument!="")
      { this->functionArguments.AddObjectOnTop(this->GetTokenFromArgumentStringChangeInput(currentArgument));
        if (*this->functionArguments.LastObject()==-1)
          return false;
        currentArgument="";
      }
    }
    else
      currentArgument.push_back(theArgumentList[currentChar]);
    if (theArgumentList[currentChar]==')')
      return true;
    if (theArgumentList[currentChar]=='(')
    { this->functionArguments.AddObjectOnTop(ParserNode::typeArray);
      this->nestedArgumentsOfArguments.ExpandOnTop(1);
      if(!this->nestedArgumentsOfArguments.LastObject()->MakeFromString(currentChar, theArgumentList))
        return false;
      currentArgument="";
    }
  }
  return false;
}

int ParserNode::EvaluateSolveLPolyEqualsZeroOverCone
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ ParserNode& polyNode=theNode.owner->TheObjects[theArgumentList.TheObjects[0]];
  ParserNode& coneNode=theNode.owner->TheObjects[theArgumentList.TheObjects[1]];
  if (polyNode.polyValue.GetElement().NumVars!=coneNode.theCone.GetElement().GetDim()-1)
    return theNode.SetError(theNode.errorDimensionProblem);
  Cone& currentCone=coneNode.theCone.GetElement();
  currentCone.SolveLPolyEqualsZeroIAmProjective(polyNode.polyValue.GetElement(), theNode.theCone.GetElement(), theGlobalVariables);
  theNode.outputString=theNode.theCone.GetElement().ElementToString(false, true);
  return theNode.errorNoError;
}

int ParserNode::EvaluatePrintRootSAsAndSlTwos
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables, bool redirectToSlTwos, bool forceRecompute)
{ std::stringstream out1, out2, outSltwoPath, outSltwoDisplayPath, outSltwoMainFile, out;
  CGIspecificRoutines::SetCGIServerIgnoreUserAbort();
  //double oldMaxAllowedComputationTimeInSeconds= theGlobalVariables.MaxAllowedComputationTimeInSeconds;
  theGlobalVariables.MaxAllowedComputationTimeInSeconds=10000;
  char weylLetter=theNode.owner->DefaultWeylLetter;
  int theRank=theNode.owner->DefaultWeylRank;
  out1 << theNode.owner->outputFolderPath << weylLetter << theRank << "/rootHtml.html";
  out2 << theNode.owner->outputFolderDisplayPath << weylLetter << theRank << "/rootHtml.html";
  outSltwoPath << theNode.owner->outputFolderPath << weylLetter << theRank << "/sl2s/";
  outSltwoDisplayPath << theNode.owner->outputFolderDisplayPath << weylLetter << theRank << "/sl2s/";
  outSltwoMainFile << outSltwoPath.str() << "sl2s_nopng.html";
  //std::string tempS=outSltwoMainFile.str();
  bool NeedASecondRun=false;
  if (!CGIspecificRoutines::FileExists(out1.str()) || !CGIspecificRoutines::FileExists(outSltwoMainFile.str()) || forceRecompute)
  { std::stringstream outMkDirCommand1, outMkDirCommand2;
    outMkDirCommand1 << "mkdir " << theNode.owner->outputFolderPath << weylLetter << theRank;
    outMkDirCommand2 << "mkdir " << theNode.owner->outputFolderPath << weylLetter << theRank << "/sl2s";
    theNode.owner->SystemCommands.AddObjectOnTop(outMkDirCommand1.str());
    theNode.owner->SystemCommands.AddObjectOnTop(outMkDirCommand2.str());
    if (!CGIspecificRoutines::FileExists(outSltwoPath.str()))
      NeedASecondRun=true;
    else
    { SltwoSubalgebras theSl2s;
      theSl2s.owner.FindSl2Subalgebras(theSl2s, weylLetter, theRank, theGlobalVariables);
      std::string PathSl2= outSltwoPath.str(); std::string DisplayPathSl2=outSltwoDisplayPath.str();
      theSl2s.ElementToHtml(theGlobalVariables, theSl2s.owner.theWeyl, true, PathSl2, DisplayPathSl2);
      theNode.owner->SystemCommands.AddListOnTop(theSl2s.listSystemCommandsLatex);
      theNode.owner->SystemCommands.AddListOnTop(theSl2s.listSystemCommandsDVIPNG);
    }
  }
  if (!NeedASecondRun)
  { out << "<META http-equiv=\"refresh\" content=\"0; url=";
    if (!redirectToSlTwos)
      out << out2.str();
    else
      out << outSltwoDisplayPath.str() << "sl2s.html";
    out << "\">  Redirecting to the requested page: <br><br> <a href=\"" <<  out2.str() << "\">"  << out1.str() << "</a>.<br><br>";
  }
  else
  { out << "<br><br>... Created the missing folders for the database. <b> Running  a second time... (you wil be redirected automatically)</b><br><br><META http-equiv=\"refresh\" content=\"0; url="
          << "/cgi-bin/calculator?"
          << " textType=" << weylLetter << "&textDim=" << theRank << "&textInput=";
    if (redirectToSlTwos)
      out << CGIspecificRoutines::UnCivilizeStringCGI("printRootSubalgebras");
    else
      out << CGIspecificRoutines::UnCivilizeStringCGI("printSlTwos");
    out << "\">";
  }
  theNode.outputString=out.str();
  theNode.ExpressionType=theNode.typeString;
//  theGlobalVariables.MaxAllowedComputationTimeInSeconds=oldMaxAllowedComputationTimeInSeconds;
  return theNode.errorNoError;
}

std::string Parser::GetFunctionDescription()
{ std::stringstream out;
  for (int i=0; i<this->theFunctionList.size; i++)
    out << this->theFunctionList.TheObjects[i].ElementToString(true, false) << " <br>";
  return out.str();
}

std::string ParserFunctionArgumentTree::ElementToString(bool useHtml, bool useLatex)const
{ std::stringstream out;
  out << "(";
  int lowestIndexNonExplored=0;
  if (this->functionArguments.size==0)
    return "";
  for (int i=0; i<this->functionArguments.size; i++)
  { out << this->GetArgumentStringFromToken(this->functionArguments.TheObjects[i]);
    if (this->functionArguments.TheObjects[i]==ParserNode::typeArray)
    { out << this->nestedArgumentsOfArguments.TheObjects[lowestIndexNonExplored].ElementToString(useHtml, useLatex);
      lowestIndexNonExplored++;
    }
    if (i!=this->functionArguments.size-1)
      out << ", ";
  }
  out  << ")";
  return out.str();
}

bool CGIspecificRoutines::GetHtmlStringSafeishReturnFalseIfIdentical(const std::string& input, std::string& output)
{ std::stringstream out;
  bool result=false;
  for (unsigned int i=0; i<input.size(); i++)
  { if (input[i]=='<')
      out << "&lt;";
    if (input[i]=='>')
      out << "&gt;";
    if (input[i]!='<' && input[i]!='>')
      out << input[i];
    else
      result=true;
  }
  output=out.str();
  return result;
}

std::string CGIspecificRoutines::UnCivilizeStringCGI(const std::string& input)
{ std::stringstream out;
  for (unsigned int i=0; i<input.size(); i++)
    switch (input[i])
    { case ' ': out << "+"; break;
      case '+': out << "%2B"; break;
      case '(': out << "%28"; break;
      case ')': out << "%29"; break;
      case '[': out << "%5B"; break;
      case ']': out << "%5D"; break;
      case ',': out << "%2C"; break;
      case '{': out << "%7B"; break;
      case ';': out << "%3B"; break;
      case '/': out << "%2F"; break;
      case ':': out << "%3A"; break;
      case '^': out << "%5E"; break;
      case '\\': out << "%5C"; break;
      case '}': out << "%7D"; break;
      case '\n': out << "%0D%0A"; break;
//      case '': out << ""; break;
      default: out << input[i]; break;
    }
  return out.str();
}

std::string ParserFunction::ElementToString(bool useHtml, bool useLatex)const
{ std::stringstream out;
  out << "<div style=\"display: inline\" id=\"functionBox" << this->functionName << "\" >";
  out << this->functionName;
  out << this->theArguments.ElementToString(useHtml, useLatex);
  out <<  "<button" << CGIspecificRoutines::GetStyleButtonLikeHtml() << " onclick=\"switchMenu('fun" << this->functionName << "');\">More/less info</button><div id=\"fun" << this->functionName
        << "\" style=\"display: none\"><br>" << this->functionDescription << "<br>Example. <a href=\"/cgi-bin/calculator?"
        << " textType=" << this->exampleAmbientWeylLetter << "&textDim=" << this->exampleAmbientWeylRank << "&textInput="
        << CGIspecificRoutines::UnCivilizeStringCGI(this->example)
        << "\"> " << this->example << "</a></div></div>";
  return out.str();
}

void GeneralizedVermaModuleCharacters::FindMultiplicitiesExtremaStep5(GlobalVariables& theGlobalVariables)
{ if (this->NumProcessedExtremaEqualOne==0)
    this->projectivizedExtremaEqualsOneCones.SetSize(this->theExtrema.size);
  theGlobalVariables.theIndicatorVariables.String1NeedsRefresh=true;
  theGlobalVariables.theIndicatorVariables.String2NeedsRefresh=true;

  for (; this->NumProcessedExtremaEqualOne<this->projectivizedExtremaEqualsOneCones.size; this->NumProcessedExtremaEqualOne++)
  { this->projectivizedExtremaEqualsOneCones.TheObjects[this->NumProcessedExtremaEqualOne].SetSize(this->theExtrema.TheObjects[this->NumProcessedExtremaEqualOne].size);
    for (int i=0; i<this->projectivizedExtremaEqualsOneCones.TheObjects[this->NumProcessedExtremaEqualOne].size; i++)
    { std::stringstream out3, out4;
      out3 << "Processing extrema in chamber " << this->NumProcessedExtremaEqualOne+1 << " out of " << this->theExtrema.size;
      theGlobalVariables.theIndicatorVariables.ProgressReportString1=out3.str();
      out4 << "Subchamber " << i+1 << " out of " << this->projectivizedExtremaEqualsOneCones.TheObjects[this->NumProcessedExtremaEqualOne].size;
      theGlobalVariables.theIndicatorVariables.ProgressReportString2=out4.str();
      theGlobalVariables.MakeReport();
      this->theMultiplicitiesMaxOutput << "\n\n\n\n\nChamber " << this->NumProcessedConesParam+ 1;
      QuasiPolynomial& currentQP= this->theExtrema.TheObjects[this->NumProcessedExtremaEqualOne].TheObjects[i];
      Cone& currentCone=this->projectivizedExtremaCones.TheObjects[this->NumProcessedExtremaEqualOne].TheObjects[i];
      List<Cone>& currentOutput=this->projectivizedExtremaEqualsOneCones.TheObjects[this->NumProcessedExtremaEqualOne].TheObjects[i];
      currentCone.SolveLQuasiPolyEqualsZeroIAmProjective(currentQP, currentOutput, theGlobalVariables);
    }
    this->thePauseController.SafePoint();
  }
}

std::string GeneralizedVermaModuleCharacters::PrepareReport(GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  PolynomialOutputFormat theFormat;
  int tempI=0;
  root tempRoot;
  theFormat.alphabet.TheObjects[tempI]="x_1"; tempI++;
  theFormat.alphabet.TheObjects[tempI]="x_2"; tempI++;
  theFormat.alphabet.TheObjects[tempI]="y_1"; tempI++;
  theFormat.alphabet.TheObjects[tempI]="y_2"; tempI++;
  theFormat.alphabet.TheObjects[tempI]="y_3"; tempI++;
  out << "\\documentclass{article}\\usepackage{amsmath, longtable, amsfonts, amssymb, verbatim, hyperref}\n\\begin{document}\\tiny\n";
  out << "\n The chamber complex + multiplicities follow. \\begin{longtable}{cc} ";
  out << "Normals& Multiplicity of module with highest weight $(x_1,x_2)$\\endhead\n";
  int numFoundChambers=0;
  List<int> DisplayIndicesprojectivizedChambers;
  for (int i=0; i<this->projectivizedChamber.size; i++)
  { QuasiPolynomial& theMult=this->theMultiplicities.TheObjects[i];
    if (!theMult.IsEqualToZero())
    { numFoundChambers++;
      out << "\\hline\\multicolumn{2}{c}{Chamber " << numFoundChambers << "}\\\\\n";
      DisplayIndicesprojectivizedChambers.AddObjectOnTop(numFoundChambers);
      out << this->PrepareReportOneCone(theFormat, this->projectivizedChamber.TheObjects[i], theGlobalVariables) << "&";
      out << theMult.ElementToString(false, true, theFormat) << "\\\\\n";
    } else
      DisplayIndicesprojectivizedChambers.AddObjectOnTop(-1);
  }
  out << "\\end{longtable}\n\n\n Multiplicity free chambers \n";
  numFoundChambers=0;
  out << "\n\\begin{longtable}{cc} ";
  out << "Normals& Multiplicity of module with highest weight $(x_1,x_2)$\\endhead\n";
  for (int i=0; i<this->projectivezedChambersSplitByMultFreeWalls.size; i++)
  { tempRoot=this->projectivezedChambersSplitByMultFreeWalls.TheObjects[i].GetInternalPoint();
    bool found=false;
    for (int j=0; j<this->projectivizedChamber.size; j++)
      if (this->projectivizedChamber.TheObjects[j].IsInCone(tempRoot))
      { assert(!found);
        found=true;
      }
  }
  for (int i=0; i<this->projectivizedChamber.size; i++)
  { QuasiPolynomial& theMult=this->theMultiplicities.TheObjects[i];
    if (!theMult.IsEqualToZero())
    { int indexMultFreeChamber=-1;
      for (int j=0; j<this->projectivezedChambersSplitByMultFreeWalls.size; j++)
      { tempRoot=this->projectivezedChambersSplitByMultFreeWalls.TheObjects[j].GetInternalPoint();
        if (this->projectivizedChamber.TheObjects[i].IsInCone(tempRoot))
        { Rational tempRat;
          tempRat=*tempRoot.LastObject();
          if (tempRat!=0)
            tempRoot/=tempRat;
          theMult.valueOnEachLatticeShift.TheObjects[0].Evaluate(tempRoot, tempRat);
          if(tempRat<1)
          { indexMultFreeChamber=j;
            break;
          }
        }
      }
      if (indexMultFreeChamber!=-1)
      { numFoundChambers++;
        out << "\\hline\\multicolumn{2}{c}{Chamber " << DisplayIndicesprojectivizedChambers.TheObjects[i] << "}\\\\\n";
        out << this->PrepareReportOneCone(theFormat, this->projectivezedChambersSplitByMultFreeWalls.TheObjects[indexMultFreeChamber], theGlobalVariables) << "&";
        out << theMult.ElementToString(false, true, theFormat) << "\\\\\n";
      }
    }
  }
  out << "Total number of chambers with multiplicity 1 or less: " << numFoundChambers;
  out << "\\end{longtable}\n\n\n\n";
  out << "\\end{document}";
  return out.str();
}

std::string GeneralizedVermaModuleCharacters::PrepareReportOneCone
  (PolynomialOutputFormat& theFormat, Cone& theCone, GlobalVariables& theGlobalVariables)
{ std::stringstream out1;
  std::string tempS1, tempS2;
  root normalNoConstant;
  int dimSmallerAlgebra=this->theLinearOperators.TheObjects[0].NumRows;
  int dimLargerAlgebra=this->theLinearOperators.TheObjects[0].NumCols;
  Rational theConst;
  out1 << "\\begin{tabular}{rcl}";
  for (int i=0; i<theCone.Normals.size; i++)
  { root& currentNormal=theCone.Normals.TheObjects[i];
    normalNoConstant=currentNormal;
    normalNoConstant.SetSize(dimSmallerAlgebra+dimLargerAlgebra);
    theConst=-(*currentNormal.LastObject());
    if(!normalNoConstant.IsEqualToZero())
      out1 << "$" << normalNoConstant.ElementToStringLetterFormat(theFormat, true, false) << "$ & $\\geq$ & $"
               << theConst.ElementToString() << "$ \\\\";

  }
  out1 << "\\end{tabular}";
  return out1.str();
}

std::string root::ElementToStringLetterFormat(const std::string& inputLetter, bool useLatex, bool DontIncludeLastVar)
{ PolynomialOutputFormat tempFormat;
  tempFormat.MakeAlphabetArbitraryWithIndex(inputLetter);
  return this->ElementToStringLetterFormat(tempFormat, useLatex, DontIncludeLastVar);
}

std::string root::ElementToStringLetterFormat(const PolynomialOutputFormat& theFormat, bool useLatex, bool DontIncludeLastVar)
{ if (this->IsEqualToZero())
    return "0";
  std::stringstream out;
  std::string tempS;
  bool found=false;
  int NumVars= DontIncludeLastVar ? this->size-1 : this->size;
  for(int i=0; i<NumVars; i++)
    if (!this->TheObjects[i].IsEqualToZero())
    { this->TheObjects[i].ElementToString(tempS);
      if (tempS=="1")
        tempS="";
      if (tempS=="-1")
        tempS="-";
      if (found)
      { if (tempS.size()>0)
        { if (tempS[0]!='-')
            out << "+";
        } else
          out << "+";
      }
      found=true;
      out << tempS;
      out << theFormat.alphabet.TheObjects[i];
    }
  return out.str();
}

bool MatrixLargeRational::IsIdMatrix()const
{ if (this->NumRows!=this->NumCols || this->NumRows<=0)
     return false;
   for (int i=0; i<this->NumRows; i++)
     for (int j=0; j<this->NumCols; j++)
       if (i==j)
       { if (!this->elements[i][j].IsEqualToOne())
           return false;
       } else
         if (!this->elements[i][j].IsEqualToZero())
           return false;
   return true;
}

root oneFracWithMultiplicitiesAndElongations::GetCheckSumRoot(int NumVars)
{ root output;
  output.SetSize(NumVars);
  for (int i=0; i<NumVars; i++)
    output.TheObjects[i]=i+2;
  return output;
}

bool partFractions::RemoveRedundantShortRootsIndex(GlobalVariables& theGlobalVariables, root* Indicator, int theIndex)
{ if (!this->TheObjects[theIndex].rootIsInFractionCone(*this, Indicator, theGlobalVariables))
    return false;
  bool found=false;
  for (int k=0; k < this->TheObjects[theIndex].IndicesNonZeroMults.size; k++)
  { int currentIndex=this->TheObjects[theIndex].IndicesNonZeroMults.TheObjects[k];
    oneFracWithMultiplicitiesAndElongations& currentFrac = this->TheObjects[theIndex].TheObjects[currentIndex];
    if (currentFrac.Elongations.size>1)
    { found=true;
      break;
    }
  }
  if (!found)
    return false;
  partFraction& thePF= theGlobalVariables.fracRemoveRedundantRootsBuffer1;
  thePF.Assign(this->TheObjects[theIndex]);
  this->PopIndexSwapLastHashAndAccount(theIndex, theGlobalVariables, Indicator);
  Rational localStartCheckSum, localEndCheckSum;
  std::string tempS, tempS1, tempS2;
  Polynomial<LargeInt> ComputationalBufferCoefficient, tempIP;
  PolyPartFractionNumerator ComputationalBufferCoefficientNonExpanded, tempPP;
  ComputationalBufferCoefficient.AssignPolynomialLight(thePF.Coefficient);
  thePF.CoefficientNonExpanded.ComputePolyPartFractionNumerator(ComputationalBufferCoefficientNonExpanded, this->AmbientDimension);
  if (this->flagMakingProgressReport || this->flagAnErrorHasOccurredTimeToPanic)
  { //thePF.ComputeDebugString(*this, theGlobalVariables);
    //std::cout << thePF.DebugString;
    thePF.ComputeOneCheckSum(*this, localStartCheckSum, this->AmbientDimension, theGlobalVariables);
    localStartCheckSum.ElementToString(tempS2);
  }
  for (int k=0; k<thePF.IndicesNonZeroMults.size; k++)
  { int currentIndex=thePF.IndicesNonZeroMults.TheObjects[k];
    oneFracWithMultiplicitiesAndElongations& currentFrac = thePF.TheObjects[currentIndex];
    int LCMElongations = currentFrac.GetLCMElongations();
    this->RootsToIndices.TheObjects[currentIndex].ElementToString(tempS);
    while (currentFrac.Elongations.size>1)
    { for (int i=0; i<currentFrac.Elongations.size; i++)
      { int ElongationValue=currentFrac.Elongations.TheObjects[i];
        if (ElongationValue!=LCMElongations)
        { int numSummands=LCMElongations/ElongationValue;
          if (thePF.UncoveringBrackets)
          { thePF.GetNElongationPoly(*this, currentIndex, ElongationValue, numSummands, tempIP, this->AmbientDimension);
            tempIP.ComputeDebugString();
            tempIP.RaiseToPower(currentFrac.Multiplicities.TheObjects[i], (LargeInt) 1);
            tempIP.ComputeDebugString();
            ComputationalBufferCoefficient.MultiplyBy(tempIP);
          }
          else
          { PolyPartFractionNumerator tempP;
            thePF.GetNElongationPoly(*this, currentIndex, ElongationValue, numSummands, tempP, this->AmbientDimension);
            tempP.RaiseToPower(currentFrac.Multiplicities.TheObjects[i], (LargeInt) 1);
//            this->CoefficientNonExpanded.ComputeDebugString();
            ComputationalBufferCoefficientNonExpanded.MultiplyBy(tempP);
//            this->CoefficientNonExpanded.ComputeDebugString();
          }
          ComputationalBufferCoefficientNonExpanded.ComputeDebugString();
          currentFrac.AddMultiplicity(currentFrac.Multiplicities.TheObjects[i], LCMElongations);
          currentFrac.AddMultiplicity(-currentFrac.Multiplicities.TheObjects[i], ElongationValue);
          thePF.Coefficient.AssignPolynomial(ComputationalBufferCoefficient);
          thePF.ComputeOneCheckSum(*this, localEndCheckSum, this->AmbientDimension, theGlobalVariables);
          assert(localEndCheckSum.IsEqualTo(localStartCheckSum));
        }
      }
    }
    if (partFraction::MakingConsistencyCheck || this->flagAnErrorHasOccurredTimeToPanic)
    { thePF.Coefficient.AssignPolynomial(ComputationalBufferCoefficient);
      thePF.ComputeOneCheckSum(*this, localEndCheckSum, this->AmbientDimension, theGlobalVariables);
      localEndCheckSum.ElementToString(tempS1);
      assert(localStartCheckSum.IsEqualTo(localEndCheckSum));
    }
  }
  thePF.CoefficientNonExpanded.AssignPolyPartFractionNumerator(ComputationalBufferCoefficientNonExpanded);
  thePF.Coefficient.AssignPolynomial(ComputationalBufferCoefficient);
  int tempI = this->IndexOfObjectHash(thePF);
  if (tempI==-1)
    this->AddAlreadyReduced(thePF, theGlobalVariables, Indicator);
  else
  { this->AccountPartFractionInternals(-1, tempI, Indicator, theGlobalVariables);
    this->TheObjects[tempI].AddReturnShouldAttemptReduction(thePF, *this, theGlobalVariables);
    this->AccountPartFractionInternals(1, tempI, Indicator, theGlobalVariables);
  }
  return true;
}

int ParserNode::EvaluateInvariantsSl2DegreeM
(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ List<int> thePartition;
  if (theArgumentList.size!=2)
    return theNode.SetError(theNode.errorWrongNumberOfArguments);
  int theDegree=theNode.owner->TheObjects[theArgumentList.TheObjects[0]].intValue;
  if (theDegree<0)
    return theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);
  ParserNode& partitionNode=theNode.owner->TheObjects[theArgumentList.TheObjects[1]];
  if (partitionNode.ExpressionType==typeArray)
  { thePartition.SetSize(partitionNode.children.size);
    for (int i=0; i<partitionNode.children.size; i++)
    { thePartition.TheObjects[i]=theNode.owner->TheObjects[partitionNode.children.TheObjects[i]].intValue;
      if (thePartition.TheObjects[i]<1)
        return theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);
  //    std::cout<< thePartition.TheObjects[i] << ",";
    }
  } else
  { thePartition.SetSize(1);
    thePartition.TheObjects[0]=partitionNode.intValue;
  }
  slTwoInSlN theSl2;
  theNode.ExpressionType=theNode.typeString;
  List<PolynomialRationalCoeff> outputList;
  std::string tempS;
  if (! theSl2.ComputeInvariantsOfDegree(thePartition, theDegree, outputList, tempS, theGlobalVariables))
  { theNode.outputString=tempS;
    return theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);
  }
  theNode.ExpressionType=theNode.typeString;
  std::stringstream out;
  out << "A basis for the invariants of degree " << theDegree << " is given by (number of elements: " << outputList.size << ")";
  for (int i=0; i<outputList.size; i++)
  { out << "<br>" << outputList.TheObjects[i].ElementToString() << ", ";
  }
  theNode.outputString=out.str();
  return theNode.errorNoError;
}

void MatrixLargeRational::ActOnMonomialAsDifferentialOperator(Monomial<Rational>& input, PolynomialRationalCoeff& output)
{ assert(this->NumRows==this->NumCols && this->NumRows==input.NumVariables);
  Monomial<Rational> tempMon;
  tempMon.init(tempMon.NumVariables);
  output.Nullify(tempMon.NumVariables);
  for (int i=0; i<this->NumRows; i++)
    for (int j=0; j<this->NumCols; j++)
    { tempMon=input;
      tempMon.Coefficient*=tempMon.degrees[j];
      tempMon.Coefficient*=this->elements[i][j];
      tempMon.degrees[j]--;
      tempMon.degrees[i]++;
      output.AddMonomial(tempMon);
    }
}

void LargeIntUnsigned::ElementToStringLargeElementDecimal(std::string& output)const
{ std::stringstream out;
  if (this->CarryOverBound==1000000000UL)
  { std::string tempS;
    out << (this->TheObjects[this->size-1]);
    for (int i=this->size-2; i>=0; i--)
    { std::stringstream tempStream;
      tempStream << this->TheObjects[i];
      tempS=tempStream.str();
      int numZeroesToPad=9-tempS.length();
      for (int j=0; j<numZeroesToPad; j++)
          out << "0";
      out << tempS;
    }
    output= out.str();
    return;
  }
  unsigned int base=10;
  int MaxNumIntegersPerCarryOverBound=11;
  List<LargeIntUnsigned> bufferPowersOfBase;
  int initialNumDigitsEstimate=MaxNumIntegersPerCarryOverBound*this->size;
  int sizeBufferPowersOfBase=MathRoutines::Minimum(initialNumDigitsEstimate, 10000);
  bufferPowersOfBase.SetSize(sizeBufferPowersOfBase);
  LargeIntUnsigned currentPower;
  LargeIntUnsigned Remainder=*this;
  int numRemainingDigits;
  while (!Remainder.IsEqualToZero())
  { currentPower.MakeOne();
    numRemainingDigits=0;
    int highestBufferIndex=-1;
    bufferPowersOfBase.TheObjects[0].MakeOne();
    bool bufferFilled=false;
    while (Remainder.IsGEQ(currentPower))
    { numRemainingDigits++;
      highestBufferIndex++;
      highestBufferIndex%=sizeBufferPowersOfBase;
      bufferPowersOfBase.TheObjects[highestBufferIndex]=currentPower;
      if (highestBufferIndex==sizeBufferPowersOfBase-1)
        bufferFilled=true;
      currentPower*=base;
    }
    int startIndex=highestBufferIndex;
    do
    { currentPower=bufferPowersOfBase.TheObjects[highestBufferIndex];
      unsigned int theDigit=0;
      while (Remainder.IsGEQ(currentPower))
      { theDigit++;
        currentPower+=bufferPowersOfBase.TheObjects[highestBufferIndex];
      }
      out << theDigit;
      numRemainingDigits--;
      if (theDigit!=1)
        bufferPowersOfBase.TheObjects[highestBufferIndex]*=theDigit;
      Remainder.SubtractSmallerPositive(bufferPowersOfBase.TheObjects[highestBufferIndex]);
      highestBufferIndex--;
      if (highestBufferIndex==-1)
      { highestBufferIndex=sizeBufferPowersOfBase-1;
        if (!bufferFilled)
          break;
      }
    } while (highestBufferIndex!=startIndex);
  }
  for (int i=0; i<numRemainingDigits; i++)
    out << "0";
  output=out.str();
}

int ParserNode::EvaluateFactorial
    (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ Rational tempRat;
  int theArgument=theNode.owner->TheObjects[theArgumentList.TheObjects[0]].intValue;
  if (theArgument<0)
    return theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);
  if (theArgument>5000)
  { theNode.outputString="Computation too large.";
    return theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);
  }
  theNode.rationalValue=tempRat.Factorial(theArgument, & theGlobalVariables);
//  theNode.ExpressionType=theNode.typeRational;
  theNode.ExpressionType=theNode.typeRational;
  //theNode.outputString=theNode.rationalValue.ElementToString();
  return theNode.errorNoError;
}

int ParserNode::EvaluatePrintAllPrimesEratosthenes
    (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ Rational tempRat;
  int theArgument=theNode.owner->TheObjects[theArgumentList.TheObjects[0]].intValue;
  if (theArgument<0)
    return theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);
  if (theArgument>20000)
  { theNode.outputString="Computation too large.";
    return theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);
  }
  List<unsigned int> thePrimes;
  LargeIntUnsigned::GetAllPrimesSmallerThanOrEqualToUseEratosthenesSieve(theArgument, thePrimes);
  std::stringstream out;
  for (int i=0; i<thePrimes.size; i++)
    out << thePrimes.TheObjects[i] << ", ";
  theNode.outputString=out.str();
  theNode.ExpressionType=theNode.typeString;
  return theNode.errorNoError;
}

void LargeIntUnsigned::AssignFactorial(unsigned int x, GlobalVariables* theGlobalVariables)
{ this->MakeOne();
  List<unsigned int> primesBelowX;
  LargeIntUnsigned::GetAllPrimesSmallerThanOrEqualToUseEratosthenesSieve(x, primesBelowX);
  LargeIntUnsigned tempInt, tempOne;
  tempOne.MakeOne();
  for (int i=0; i<primesBelowX.size; i++)
  { unsigned int thePrime=primesBelowX.TheObjects[i];
    unsigned int thePowerOfThePrime=0;
    unsigned int currentPower=thePrime;
    do
    { thePowerOfThePrime+=x/currentPower;
      currentPower*=thePrime;
    }
    while (currentPower<=x);
    tempInt.AssignShiftedUInt(thePrime, 0);
    MathRoutines::RaiseToPower(tempInt, thePowerOfThePrime, tempOne);
    *this*=tempInt;
/*    if (theGlobalVariables!=0)
    { std::stringstream out;
      out << "processing prime " << thePrime << " (" << i+1 << " out of " << primesBelowX.size << ").";
      theGlobalVariables->theIndicatorVariables.ProgressReportString1=out.str();
      theGlobalVariables->theIndicatorVariables.String1NeedsRefresh=true;
      theGlobalVariables->MakeReport();
    }*/
  }
}

void Lattice::GetClosestPointToHyperplaneWRTFirstCoordinate
  (root& theDirection, root& theAffineHyperplane, roots& outputRepresentatives, roots& movementInDirectionPerRepresentative, GlobalVariables& theGlobalVariables)
{ if (theAffineHyperplane.TheObjects[0].IsEqualToZero())
    return;
  root theNormal=theAffineHyperplane;
  theNormal.SetSize(theNormal.size-1);
  if (theDirection.ScalarEuclidean(theNormal).IsEqualToZero())
    return;
  Rational theConstOnTheOtherSide=-*theAffineHyperplane.LastObject();
  roots theBasis;
  theBasis.AssignMatrixRows(this->basisRationalForm);
  Lattice theHyperplaneLatticeNoShift, theDirectionLattice, theRougherLattice;//, normalProjectionLattice, theTrueProjectionLattice;
  roots tempRoots; //root tempRoot;
  tempRoots.AddObjectOnTop(theDirection);
  theDirectionLattice=*this;
  theDirectionLattice.IntersectWithLinearSubspaceSpannedBy(tempRoots);
  root theTrueDirection;
  theDirectionLattice.basisRationalForm.RowToRoot(0, theTrueDirection);
  std::cout << "<br>the normal: " << theNormal.ElementToString();
  std::cout << "<br> the direction lattice: " << theDirectionLattice.ElementToString();
  theHyperplaneLatticeNoShift=*this;
  theHyperplaneLatticeNoShift.IntersectWithLinearSubspaceGivenByNormal(theNormal);
  std::cout << "<br>the non-affine hyperplane intersected with the lattice: " << theHyperplaneLatticeNoShift.ElementToString();
  tempRoots.AssignMatrixRows(theHyperplaneLatticeNoShift.basisRationalForm);
  tempRoots.AddObjectOnTop(theTrueDirection);
  theRougherLattice.MakeFromRoots(tempRoots);
  this->GetAllRepresentatitves(theRougherLattice, outputRepresentatives);
  std::cout << "<br>the rougher lattice: " << theRougherLattice.ElementToString();
  std::cout << "<br>representatives of the quotient of the two lattices: " << outputRepresentatives.ElementToString();
  Rational theShiftedConst, unitMovement, tempRat;
  unitMovement=theNormal.ScalarEuclidean(theTrueDirection);
  movementInDirectionPerRepresentative.SetSize(outputRepresentatives.size);
  std::cout << "<br>Affine hyperplane per representative: ";
  for (int i=0; i<outputRepresentatives.size; i++)
  { tempRat=(theNormal.ScalarEuclidean(outputRepresentatives.TheObjects[i]) -theConstOnTheOtherSide)/unitMovement;
    tempRat.AssignFracValue();
    theShiftedConst=theConstOnTheOtherSide+tempRat;
    root& currentMovement=movementInDirectionPerRepresentative.TheObjects[i];
    currentMovement=theAffineHyperplane;
    *currentMovement.LastObject()=theShiftedConst;
    std::cout << "<br>Representative: " << outputRepresentatives.TheObjects[i].ElementToString() << " and the hyperplane: " << currentMovement.ElementToString();
    //currentMovement
  }
/*  roots theNormalProjections;
  theNormalProjections.SetSize(theBasis.size);
  MatrixLargeRational projectionMap;
  projectionMap.AssignVectorRow(theNormal);
  for (int i=0; i<theBasis.size; i++)
  { theNormalProjections.TheObjects[i].SetSize(1);
    theNormalProjections.TheObjects[i].TheObjects[0]= theNormal.ScalarEuclidean(theBasis.TheObjects[i]);
  }
  normalProjectionLattice.MakeFromRoots(theNormalProjections);

  std::cout << "<br>normal projection lattice: " << normalProjectionLattice.ElementToString();

  root theTrueProjection; theTrueProjection.SetSize(1);
  theTrueProjection.TheObjects[0]= theTrueDirection.ScalarEuclidean(theNormal);
  tempRoots.size=0;
  tempRoots.AddObjectOnTop(theTrueProjection);
  theTrueProjectionLattice.MakeFromRoots(tempRoots);
  std::cout << "<br> the true projection lattice: " << theTrueProjectionLattice.ElementToString(true, false);
  Lattice theRougherLattice;

  theRougherLattice=*this;
  std::cout << "<br> starting lattice: " << this->ElementToString();
  theRougherLattice.IntersectWithPreimageOfLattice(projectionMap, theTrueProjectionLattice, theGlobalVariables);
  std::cout << "<br> the rougher lattice: " << theRougherLattice.ElementToString(true, false);
  roots allRepresentativesStartingLattice;
  this->GetAllRepresentatitves(theRougherLattice, allRepresentativesStartingLattice);
  root theShift; theShift.SetSize(1);
  theShift.TheObjects[0]=-*theAffineHyperplane.LastObject();
  roots allRepresentatives, allRepresentativesPreimages; root tempRoot;
  normalProjectionLattice.GetAllRepresentatitves(theTrueProjectionLattice, allRepresentatives);
  std::cout << "<br>all the representatives: " << allRepresentatives.ElementToString();
  std::cout << "<br> the shift: " << theShift.ElementToString();
  for (int i=0; i< allRepresentatives.size; i++)
  { allRepresentatives.TheObjects[i]+=theShift;
    theTrueProjectionLattice.ReduceVector(allRepresentatives.TheObjects[i]);
  }
  this->FindOnePreimageInLatticeOf(projectionMap, allRepresentatives, allRepresentativesPreimages, theGlobalVariables);
  std::cout << "<br>all the representatives shifted and modded: " << allRepresentatives.ElementToString();
  std::cout << "<br> one preimage for each representative: " << allRepresentativesPreimages.ElementToString();
  roots outputShifts;
  PolynomialRationalCoeff tempP, tempP2;
  PolynomialOutputFormat tempFormat;
  for (int i=0; i<allRepresentatives.size; i++)
  { tempP.Nullify(this->GetDim());
    Rational theDen=theTrueDirection.ScalarEuclidean(theNormal);
    for (int j=0; j<this->GetDim(); j++)
    { tempP2.MakeNVarDegOnePoly(this->GetDim(), j, -theNormal.TheObjects[j]/theDen+1);
      tempP+=tempP2;
    }
    tempP+=allRepresentatives.TheObjects[i].TheObjects[0]/theDen;
    std::cout << "<br>representative" << i+1 << ": " << tempP.ElementToString(tempFormat);
  }*/
}

int ParserNode::EvaluateClosestPointToHyperplaneAlongTheNormal
(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ Lattice& currentLattice=theNode.owner->TheObjects[theArgumentList.TheObjects[2]].theLattice.GetElement();
  ParserNode& theHyperplaneNode=theNode.owner->TheObjects[theArgumentList.TheObjects[1]];
  ParserNode& theDirectionNode=theNode.owner->TheObjects[theArgumentList.TheObjects[0]];

  //ParserNode& theRayNode=theNode.owner->TheObjects[theArgumentList.TheObjects[2]];
  root theAffineNormal, theRay;//, theRay;
  if (!theHyperplaneNode.GetRootRational(theAffineNormal, theGlobalVariables))
    return theNode.SetError(theNode.errorProgramming);
  if (!theDirectionNode.GetRootRational(theRay, theGlobalVariables))
    return theNode.SetError(theNode.errorProgramming);
//  if (!theRayNode.GetRootRational(theRay, theGlobalVariables))
//    return theNode.SetError(theNode.errorProgramming);
  //int theDim=t;
  if (theAffineNormal.size-1!=currentLattice.GetDim() || currentLattice.GetRank()!=currentLattice.GetDim() || theRay.size!=currentLattice.GetDim())
    return theNode.SetError(theNode.errorDimensionProblem);
  roots representatives, theMovements;
  currentLattice.GetClosestPointToHyperplaneWRTFirstCoordinate(theRay, theAffineNormal, representatives, theMovements, theGlobalVariables);
  return theNode.errorNoError;
}

bool slTwoInSlN::ComputeInvariantsOfDegree
  (List<int>& decompositionDimensions, int theDegree, List<PolynomialRationalCoeff>& output, std::string& outputError, GlobalVariables& theGlobalVariables)
{ this->initFromModuleDecomposition(decompositionDimensions, false, false);
  SelectionWithMaxMultiplicity theSel;
  theSel.initMaxMultiplicity(this->theDimension, theDegree);
  PolynomialOutputFormat PolyFormatLocal;
  outputError="";
  int numCycles=theSel.NumCombinationsOfCardinality(theDegree);
  if (numCycles<=0)
  { outputError= " Computation too large. ";
    return false;
  }
  PolynomialRationalCoeff basisMonsZeroWeight, basisMonsAll;
  basisMonsZeroWeight.Nullify(this->theDimension);
  basisMonsZeroWeight.MakeActualSizeAtLeastExpandOnTop(numCycles);
  basisMonsAll.Nullify(this->theDimension);
  basisMonsAll.MakeActualSizeAtLeastExpandOnTop(numCycles);
  Monomial<Rational> theMon;
  theMon.init(this->theDimension);
  root theWeight;
  root theCartanAction;
  theCartanAction.SetSize(this->theDimension);
  theWeight.SetSize(this->theDimension);
  for (int j=0; j<this->theDimension; j++)
    theCartanAction.TheObjects[j]=this->theH.elements[j][j];
  theSel.IncrementSubsetFixedCardinality(theDegree);
  theMon.Coefficient.MakeOne();
  for (int i=0; i<numCycles; i++, theSel.IncrementSubsetFixedCardinality(theDegree))
  { for (int j=0; j<this->theDimension; j++)
    { theMon.degrees[j]=theSel.Multiplicities.TheObjects[j];
      theWeight.TheObjects[j]=theMon.degrees[j];
    }
    basisMonsAll.AddMonomial(theMon);
    if (root::RootScalarEuclideanRoot(theWeight, theCartanAction).IsEqualToZero())
      basisMonsZeroWeight.AddMonomial(theMon);
  }
//  std::cout << "<br>Num cycles:" << numCycles << "<br>The basis mons (there are " << basisMonsZeroWeight.size << " of them): "  << basisMonsZeroWeight.ElementToString(PolyFormatLocal);
  MatrixLargeRational tempMat;
  tempMat.init(basisMonsAll.size*2, basisMonsZeroWeight.size);
//  tempMat.init(basisMonsAll.size*numGenerators, basisMonsZeroWeight.size);
  PolynomialRationalCoeff tempP;
  for (int l=0; l<2; l++)
    for (int k=0; k<basisMonsZeroWeight.size; k++)
    { if (l==0)
      { this->theE.ActOnMonomialAsDifferentialOperator(basisMonsZeroWeight.TheObjects[k], tempP);

      }
      else
        this->theF.ActOnMonomialAsDifferentialOperator(basisMonsZeroWeight.TheObjects[k], tempP);
      for (int j=0; j<basisMonsAll.size; j++)
      { int indexInResult=tempP.IndexOfObjectHash(basisMonsAll.TheObjects[j]);
        int currentRow=l*basisMonsAll.size+j;
        if (indexInResult==-1)
          tempMat.elements[currentRow][k]=0;
        else
          tempMat.elements[currentRow][k]=tempP.TheObjects[indexInResult].Coefficient;
      }
    }
//  if (tempMat.NumRows<120)
//    std::cout << "<div class=\"math\" scale=\"50\">" << tempMat.ElementToString(false, true) << "</div>";
  roots tempRoots;
  tempMat.FindZeroEigenSpace(tempRoots, theGlobalVariables);
  output.SetSize(tempRoots.size);
//  std::cout << "<br>invariants root form: " << tempRoots.ElementToString();
//  std::cout << "<br> .... and the invariants are: ";
  for (int i=0; i<output.size; i++)
  { Polynomial<Rational>& current=output.TheObjects[i];
    current.Nullify(this->theDimension);
    for (int j=0; j<basisMonsZeroWeight.size; j++)
      if (!tempRoots.TheObjects[i].TheObjects[j].IsEqualToZero())
      { theMon.Assign(basisMonsZeroWeight.TheObjects[j]);
        theMon.Coefficient=tempRoots.TheObjects[i].TheObjects[j];
        current.AddMonomial(theMon);
      }
 //   std::cout << "<br>Invariant " << i << ":<br>" << current.ElementToString(PolyFormatLocal);
  }
  return true;
}

void GeneralizedVermaModuleCharacters::FindMultiplicitiesFree
(GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  this->projectivezedChambersSplitByMultFreeWalls.init();
  ConeComplex tempComplex;
  root theMultFreeWall;
  for (int i=0; i<this->projectivizedChamber.size; i++)
  { Cone& currentCone=this->projectivizedChamber.TheObjects[i];
    tempComplex.init();
    tempComplex.AddNonRefinedChamberOnTopNoRepetition(currentCone);
    assert(tempComplex.size==1);
    tempComplex.LastObject()->LowestIndexNotCheckedForSplitting=0;
    QuasiPolynomial& theMult=this->theMultiplicities.TheObjects[i];
    for (int j=0; j<theMult.valueOnEachLatticeShift.size; j++)
    { PolynomialRationalCoeff& currentPoly= theMult.valueOnEachLatticeShift.TheObjects[j];
      theMultFreeWall.MakeZero(theMult.GetNumVars()+1);
      for (int k=0; k< currentPoly.size; k++)
      { Monomial<Rational>& theMon=currentPoly.TheObjects[k];
        if (theMon.IsAConstant())
          *theMultFreeWall.LastObject()=theMon.Coefficient-1;
        else
        { int theIndex=0;
          if(!theMon.IsOneLetterFirstDegree(theIndex))
            assert(false);
          theMultFreeWall.TheObjects[theIndex]=theMon.Coefficient;
        }
      }
      tempComplex.splittingNormals.AddObjectOnTopHash(theMultFreeWall);
      *theMultFreeWall.LastObject()+=1025;
      tempComplex.splittingNormals.AddObjectOnTopHash(theMultFreeWall);
    }
    tempComplex.Refine(theGlobalVariables);
    assert(tempComplex.size<=2);
    this->projectivezedChambersSplitByMultFreeWalls.AddListOnTopNoRepetitionOfObjectHash(tempComplex);
  }
  out << this->projectivezedChambersSplitByMultFreeWalls.ElementToString(false, false);
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
  theGlobalVariables.MakeReport();
}

std::string CGIspecificRoutines::GetHtmlMathFromLatexFormula (const std::string& input, bool useDiv)
{ std::stringstream out;
  CGIspecificRoutines::GlobalFormulaIdentifier++;
  if (useDiv)
    out << "<div";
  else
    out << "<span";
  out << " id=\"theResult" << CGIspecificRoutines::GlobalFormulaIdentifier << "\" class=\"math\" scale=\"50\">\\begin{array}{rcl}&&\n";
  out << input;
  out << "\n\\end{array}";
  if (useDiv)
    out << "</div><br>";
  else
    out << "</span>";
  out << "<textarea id=\"theResultLatex" << CGIspecificRoutines::GlobalFormulaIdentifier << "\" style=\"display: none\">";
  out << "\\begin{array}{rcl}&&\n" << input << "\n\\end{array}";
  out << "</textarea>";
  if (useDiv)
    out << "\n<br>";
  out << "\n<button id=\"ButtonToggleLatex"  << CGIspecificRoutines::GlobalFormulaIdentifier
        << " \" " << CGIspecificRoutines::GetStyleButtonLikeHtml() << " onclick=\"switchMenu('theResult" << GlobalFormulaIdentifier
        << "'); switchMenu('theResultLatex" << CGIspecificRoutines::GlobalFormulaIdentifier << "');\"\">LaTeX show/hide</button>";
  return out.str();
}

void Parser::initFunctionList(char defaultExampleWeylLetter, int defaultExampleWeylRank)
{ if (this->flagFunctionListInitialized)
    return;
  this->flagFunctionListInitialized=true;
  ParserFunction theFunction;
  this->AddOneFunctionToDictionaryNoFail
  ("cone",
   "((Rational,...),...)",
   "A projective cone. The argument vectors describe the normals of the cone walls.",
   "cone((1,0),(0,1)) ",
    & ParserNode::EvaluateCone
   );
  this->AddOneFunctionToDictionaryNoFail
  ("slTwoInSlN",
   "(Integer,...)",
   "Prints out sl(2) embedden in sl(N) according to the partition described by the arguments.",
   "slTwoInSlN(2,3)",
    & ParserNode::EvaluateSlTwoInSlN
   );
  this->AddOneFunctionToDictionaryNoFail
  ("getWeylDim",
   "(Rational,...)",
   "Evaluates the Weyl dimension formula. Input: a vector in dual root coordinates.",
   "getWeylDim(0,0,1)",
    & ParserNode::EvaluateWeylDimFormula
   );
  this->AddOneFunctionToDictionaryNoFail
  ("printRootSystem",
   "()",
   "Print the root system of the ambient Lie algebra.",
   "printRootSystem",
   DefaultWeylLetter, DefaultWeylRank,
    & ParserNode::EvaluatePrintRootSystem
   );
  this->AddOneFunctionToDictionaryNoFail
  ("printBthreeDecomposedOverGtwo",
   "()",
   "Prints the decomposition of so(7) over G2. The G2-generators embedding is hardcoded according to an article of MacGovern.",
   "printBthreeDecomposedOverGtwo",
    & ParserNode::EvaluatePrintDecomposition
   );
  this->AddOneFunctionToDictionaryNoFail
  ("actByWeyl",
   "(RF,...)",
   "Act by the weyl group on a rational function (the i^th variable corresponds to the i^th coordinate in the simple basis). <br> Variables of index higher than the ambient dimension are treated as constants.",
   "actByWeyl(x_1, 1, 1/2+x_2)",
    & ParserNode::EvaluateWeylAction
   );
  this->AddOneFunctionToDictionaryNoFail
  ("partialFraction",
   "((Rational,...),...)",
   "Gives the partial fraction corresponding to the vector partition function of the arguments.<br> The arguments must be non-zero vectors with non-negative integral coordinates.",
   "partialFraction((1,0), (0,1), (1,1))",
    & ParserNode::EvaluatePartialFractions
   );
  this->AddOneFunctionToDictionaryNoFail
  ("split",
   "(PF)",
   "Splits a partial fraction according to this <a href=\"http://arxiv.org/abs/0910.4675\"> text </a>, or a modification/improvement of it.",
   "split(partialFraction((1,0), (0,1), (1,1)))",
    & ParserNode::EvaluateSplit
   );
  this->AddOneFunctionToDictionaryNoFail
  ("vpf",
   "((Rational,...),...)",
   "Computes the vector partition function with respect to the input vectors, according to this <a href=\"http://arxiv.org/abs/0910.4675\"> text </a>.",
   "vpf((1,0), (0,1), (1,1))",
    & ParserNode::EvaluateVectorPFIndicator
   );
   this->AddOneFunctionToDictionaryNoFail
  ("transformToReducedGroebnerBasis",
   "(Polynomial,...)",
   "<b> This function is largely untested. If you use it make sure to double-check the output. </b> Transforms to reduced Groebner basis using Buchberger's algorithm with respect to the lexicographic monomial ordering x_1^l&lt; x_2^m&lt;x_3^n&lt;....",
   "transformToReducedGroebnerBasis(x_1^3+x_1x_2+1, x_1x_2, x_2^3)",
    & ParserNode::EvaluateGroebner
   );
   this->AddOneFunctionToDictionaryNoFail
  ("getRelations",
   "(Polynomial,...)",
   "<b> This function is largely untested. If you use it make sure to double-check the output. </b>Get the algebraic relations between the input polynomials.",
   "getRelations(x_1^2, x_1x_2, x_2^2)",
    & ParserNode::EvaluateRelations
   );
  this->AddOneFunctionToDictionaryNoFail
  ("printRootSubalgebras",
   "()",
   "Computes all root subalgebras. The computation is served from the hard disk if it is already computed. The function will redirect you to a new page, to return to the calculator use the back button.",
   "printRootSubalgebras",
   DefaultWeylLetter, DefaultWeylRank,
    & ParserNode::EvaluatePrintRootSAs
   );
  this->AddOneFunctionToDictionaryNoFail
  ("printSlTwos",
   "()",
   "Computes all sl(2) subalgebras (equivalently, all nilpotent orbits) over the complex numbers. The computation is served from the hard disk if it is already computed. The function will redirect you to a new page, to return to the calculator use the back button.",
   "printSlTwos",
   DefaultWeylLetter, DefaultWeylRank,
    & ParserNode::EvaluatePrintSlTwos
   );
  this->AddOneFunctionToDictionaryNoFail
  ("printSlTwosAndRootSAsFORCERecompute",
   "()",
   "<b>Use only if there are broken links in root subalgebra/sl(2)-database. When executing this command, please be patient and do not click any links until automatically redirected. </b> I have a problem with setting up the Apache server and the way it handles SIGABORT signals (this is what happens when you don't wait for the computation to finish and click on a link prematurely). This function is a temporary solution to this (minor) technical problem.",
   "printSlTwosAndRootSAsFORCERecompute",
    DefaultWeylLetter, DefaultWeylRank,
    & ParserNode::EvaluatePrintRootSAsForceRecompute
   );
  this->AddOneFunctionToDictionaryNoFail
  ("factorial",
   "(Integer)",
   "Returns the factorial of a non-negative integer.<=5000",
   "factorial(2011)",
    & ParserNode::EvaluateFactorial
   );
  this->AddOneFunctionToDictionaryNoFail
  ("printAllPrimesUpTo",
   "(Integer)",
   "Prints all primes smaller than argument and smaller than<=20000 using Eratosthenes sieve.",
   "printAllPrimesUpTo(2011)",
    & ParserNode::EvaluatePrintAllPrimesEratosthenes
   );
  this->AddOneFunctionToDictionaryNoFail
  ("invariantsSlTwoOfDegree",
   "(Integer, (Integer,...))",
   "Given an sl(2)-representation  V_1+...+V_n , computes the symmetric tensor algebra invariants of sl(2). Input: the first variable is the degree, followed by a vector describing the dimensions of  each V_i.",
   "invariantsSlTwoOfDegree(2,(2,2))",
    & ParserNode::EvaluateInvariantsSl2DegreeM
   );
   /*
  this->AddOneFunctionToDictionaryNoFail
  ("solveLinNEQParamOverCone",
   "(Polynomial, Integer, Integer, Cone, Lattice)",
   "<b> This function is under development. It does not do what it claims it does; please don't use until this message disappers (you are seeing this function only if I forgot to hide it).</b>Solve linear inequality given by the linear polynomial over the cone and lattice, where the first integer gives number of non-parameters x_i and the socond integer is the number of parameters y_i. The solution is the set of y_i's for which the linear poly is satisfied for all x_j's, allowed by the starting cone.",
   "solveLinNEQParamOverCone(-2x_1+x_2,1, 1, cone((-1,1,0),(0,1,0), (-1,-2,4)), lattice((1,0),(0,1)))",
    & ParserNode::EvaluateSolveLNEqParamOverConeAndLattice
   );*/
  this->AddOneFunctionToDictionaryNoFail
  ("lattice",
   "((Rational,...),...)",
   "Rational lattice generated by the argument vectors.",
   "lattice((1,3), (2,2))",
    & ParserNode::EvaluateLattice
   );
  this->AddOneFunctionToDictionaryNoFail
  ("getPointOnLatticeClosestToWallInDirection",
   "((Rational,...), (Rational,...), Lattice)",
   "<b>Experimental, please don't use</b>.",
   "getPointOnLatticeClosestToWallInDirection( (2,3), (1,1/5,1/3), lattice((1,1), (0,2)))",
    & ParserNode::EvaluateClosestPointToHyperplaneAlongTheNormal
   );
  this->AddOneFunctionToDictionaryNoFail
  ("intersectLatticeWithPreimageOfLattice",
   "(Lattice, Lattice, ((Rational,...),...))",
   "Intersects the first lattice with the preimage of the second lattice under the linear map described by the third argument. Suppose the dimension of vector space of the first lattice is m, and the dimension of the vector space of the second lattice is n. Then the linear map should be given by a list of m vectors, with n coordinates each. The i^th of these vectors should give the image of the i^th basis vector of the vector space of the first lattice. ",
   "intersectLatticeWithPreimageOfLattice(  lattice((1,1), (0,2)), lattice(26/5), ( (1), (1/5)) )",
    & ParserNode::EvaluateIntersectLatticeWithPreimageLattice
   );
/*   this->AddOneFunctionToDictionaryNoFail
  ("solveLPolyEqualsZeroOverCone",
   "(Polynomial, Cone)",
   "<b> This function is largely untested. If you use it make sure to double-check the output. </b> Solves a linear equation over a set of linear inequalities. The first argument must be the linear polynomial (of, say, n variables). Then second argument must be the set of projectivized linear inequalities (in n+1 variables).",
   "solveLPolyEqualsZeroOverCone(x_1+x_2-1/2,cone((1,0,0), (0,1,0), (1, 1, 1) ))",
    & ParserNode::EvaluateSolveLPolyEqualsZeroOverCone
   );*/
   //printAllSlTwos
   this->theFunctionList.QuickSortAscending();
}
