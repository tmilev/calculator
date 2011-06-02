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

class GeneralizedVermaModuleCharacters
{
public:
  List<MatrixLargeRational> theLinearOperators;
  roots GmodKnegativeWeights;
  Cone PreimageWeylChamberLargerAlgebra;
  Cone WeylChamberSmallerAlgebra;
  List< List<QuasiPolynomial> > theQPs;
  roots theTranslations;
  roots theTranslationsProjected;
  std::stringstream log;
  void GetProjection(int indexOperator, root& input, root& output);
  void ComputeQPsFromChamberComplex
  (partFractions& thePFs, GlobalVariables& theGlobalVariables)
  ;
  void initFromHomomorphism
  (HomomorphismSemisimpleLieAlgebra& input, GlobalVariables& theGlobalVariables)
  ;
  void TransformToWeylProjective
  (WallData& output, GlobalVariables& theGlobalVariables)
  ;
  void TransformToWeylProjective
  (CombinatorialChamberContainer& output, GlobalVariables& theGlobalVariables)
  ;
  void TransformToWeylProjective
  (CombinatorialChamberContainer& owner, CombinatorialChamber& output, GlobalVariables& theGlobalVariables)
  ;
  void TransformToWeylProjective
  (int indexOperator, root& startingNormal, root& outputNormal)
  ;
};

void GeneralizedVermaModuleCharacters::TransformToWeylProjective
  (int indexOperator, root& startingNormal, root& outputNormal)
{ MatrixLargeRational& theOperator=this->theLinearOperators.TheObjects[indexOperator];
  root& theTranslation=this->theTranslationsProjected.TheObjects[indexOperator];
  outputNormal.MakeZero(startingNormal.size+theOperator.NumCols+1);
  for (int i=0; i<startingNormal.size; i++)
  { outputNormal.TheObjects[i]=startingNormal.TheObjects[i];
    *outputNormal.LastObject()-=theTranslation.TheObjects[i]*startingNormal.TheObjects[i];
  }
  for (int i=0; i<theOperator.NumCols; i++)
    for (int j=0; j<theOperator.NumRows; j++)
      outputNormal.TheObjects[startingNormal.size+i]-=theOperator.elements[j][i]*startingNormal.TheObjects[j];
}

void GeneralizedVermaModuleCharacters::TransformToWeylProjective
  (WallData& output, GlobalVariables& theGlobalVariables)
{ root tempRoot=output.normal;
  this->TransformToWeylProjective(0, tempRoot, output.normal);
}

void GeneralizedVermaModuleCharacters::TransformToWeylProjective
  (CombinatorialChamberContainer& owner, CombinatorialChamber& output, GlobalVariables& theGlobalVariables)
{ for (int i=0; i<output.Externalwalls.size; i++)
    this->TransformToWeylProjective(output.Externalwalls.TheObjects[i], theGlobalVariables);
  WallData newWall;
  output.Externalwalls.MakeActualSizeAtLeastExpandOnTop(this->PreimageWeylChamberLargerAlgebra.size+output.Externalwalls.size);
  for (int i=0; i<this->PreimageWeylChamberLargerAlgebra.size; i++)
  { newWall.normal=PreimageWeylChamberLargerAlgebra.TheObjects[i];
    output.Externalwalls.AddObjectOnTop(newWall);
  }
//  output.ComputeDebugString(owner);
  output.AllVertices.size=0;
  output.ComputeVerticesFromNormals(owner, theGlobalVariables);
}

void GeneralizedVermaModuleCharacters::TransformToWeylProjective
  (CombinatorialChamberContainer& output, GlobalVariables& theGlobalVariables)
{ output.flagSliceWithAWallInitDone=true;
  output.theDirections=this->GmodKnegativeWeights;
  output.init();
  output.SliceTheEuclideanSpace(theGlobalVariables, false);
  output.drawOutput(theGlobalVariables.theDrawingVariables, output.IndicatorRoot, 0);
  std::string tempS;
  output.ElementToString(tempS);
  this->log << tempS;
  output.NewHyperplanesToSliceWith.size=0;
  output.theHyperplanes.size=0;
  output.AmbientDimension=output.AmbientDimension+this->theLinearOperators.TheObjects[0].NumCols+1;
  root wallToSliceWith;
//  roots oldDirections;
  for (int k=0; k<this->theLinearOperators.size; k++)
    for (int i=0; i<output.size; i++)
      if (output.TheObjects[i]!=0)
        for (int j=0; j<output.TheObjects[i]->Externalwalls.size; j++)
        { this->TransformToWeylProjective(k, output.TheObjects[i]->Externalwalls.TheObjects[j].normal, wallToSliceWith);
//          if (k==0)
//            oldDirections.AddOnBottomNoRepetition(wallToSliceWith);
          wallToSliceWith.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
          if (k>0)
            output.NewHyperplanesToSliceWith.AddOnTopNoRepetition(wallToSliceWith);
          output.theHyperplanes.AddObjectOnTopNoRepetitionOfObjectHash(wallToSliceWith);
        }
  this->log << "\n Projectivized walls to slice with(" << output.NewHyperplanesToSliceWith.size << "):" ;
  for (int i=0; i<output.NewHyperplanesToSliceWith.size; i++)
    this->log << "\n" << output.NewHyperplanesToSliceWith.TheObjects[i].ElementToString();
  this->log << "\n";
  for (int i=0; i<output.size; i++)
    if (output.TheObjects[i]!=0)
      this->TransformToWeylProjective(output, *output.TheObjects[i], theGlobalVariables);
  root tempRoot;
  for (int i=0; i<output.TheGlobalConeNormals.size; i++)
  { tempRoot.MakeZero(output.AmbientDimension);
    int startingDim=output.TheGlobalConeNormals.TheObjects[i].size;
    for (int j=0; j<startingDim; j++)
    { tempRoot.TheObjects[j]=output.TheGlobalConeNormals.TheObjects[i].TheObjects[j];
      tempRoot.TheObjects[j+startingDim]=-tempRoot.TheObjects[j];
    }
    output.TheGlobalConeNormals.TheObjects[i]=tempRoot;
  }
  roots tempRoots;
  output.TheGlobalConeNormals.AddListOnTop(this->PreimageWeylChamberLargerAlgebra);
  this->log << "the global cone normals: " << output.TheGlobalConeNormals.ElementToString();
  for (int i=0; i<this->WeylChamberSmallerAlgebra.size; i++)
  { root& currentWeylWall=this->WeylChamberSmallerAlgebra.TheObjects[i];
    output.SliceWithAWall(currentWeylWall, theGlobalVariables);
    for (int j=0; j<output.size; j++)
      if (output.TheObjects[j]!=0)
      { CombinatorialChamber& current=*output.TheObjects[j];
        if (root::RootScalarEuclideanRoot(current.InternalPoint, currentWeylWall).IsNegative())
          current.flagPermanentlyZero=true;
      }
  }
  output.ElementToString(tempS);
  this->log << tempS;
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.theIndicatorVariables.StatusString1=this->log.str();
  theGlobalVariables.MakeReport();
  output.NumAffineHyperplanesProcessed=-1;
  output.NewHyperplanesToSliceWith.size=1;
}

void GeneralizedVermaModuleCharacters::initFromHomomorphism
  (HomomorphismSemisimpleLieAlgebra& input, GlobalVariables& theGlobalVariables)
{ roots tempRoots;
  WeylGroup& theWeyl=input.theRange.theWeyl;
//  input.ProjectOntoSmallCartan(theWeyl.RootsOfBorel, tempRoots, theGlobalVariables);
  this->log << "projections: " << tempRoots.ElementToString();
  theWeyl.ComputeWeylGroup();
  this->theLinearOperators.SetSize(theWeyl.size);
  this->theTranslations.SetSize(theWeyl.size);
  this->theTranslationsProjected.SetSize(theWeyl.size);
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
  for (int i=0; i< input.theRange.GetRank(); i++)
  { startingWeight.MakeEi(input.theRange.GetRank(), i);
    input.ProjectOntoSmallCartan(startingWeight, projectedWeight, theGlobalVariables);
    for (int j=0; j<projectedWeight.size; j++)
      theProjection.elements[j][i]=projectedWeight.TheObjects[j];
  }
  this->log << "\nMatrix form of the elements of W(B_3) (" << theWeyl.size << " elements):\n";
  for (int i=0; i<theWeyl.size; i++)
  { theWeyl.GetMatrixOfElement(i, this->theLinearOperators.TheObjects[i]);
    this->log << "\n" << this->theLinearOperators.TheObjects[i].ElementToString(false, false);
    this->theTranslations.TheObjects[i]=theWeyl.rho;
    theWeyl.ActOn(i, this->theTranslations.TheObjects[i], true, false, (Rational) 0);
    theProjection.ActOnAroot(this->theTranslations.TheObjects[i], this->theTranslationsProjected.TheObjects[i]);
  }
  this->log << "\n\n\nMatrix of the projection operator:\n" << theProjection.ElementToString(false, false);
  this->log << "\n\n\nMatrix form of the operators $u_w$, the translations and their projections:";
  for (int i=0; i<this->theLinearOperators.size; i++)
  { this->theLinearOperators.TheObjects[i].MultiplyOnTheLeft(theProjection);
    this->log << "\n\n" << this->theLinearOperators.TheObjects[i].ElementToString(false, false);
    this->log << this->theTranslations.TheObjects[i].ElementToString() << ";   " << this->theTranslationsProjected.TheObjects[i].ElementToString();
  }
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
  this->WeylChamberSmallerAlgebra.ComputeFromDirections(tempRoots, theGlobalVariables, input.theDomain.GetRank());
  this->log << "Weyl chamber smaller algebra before projectivizing: " << this->WeylChamberSmallerAlgebra.ElementToString() << "\n";
  for (int i=0; i<this->WeylChamberSmallerAlgebra.size; i++)
  { tempRoot.MakeZero(input.theRange.GetRank()+input.theDomain.GetRank()+1);
    for (int j=0; j<input.theDomain.GetRank(); j++)
      tempRoot.TheObjects[j]=this->WeylChamberSmallerAlgebra.TheObjects[i].TheObjects[j];
    this->WeylChamberSmallerAlgebra.TheObjects[i]=tempRoot;
  }

  tempRoot.MakeEi(input.theRange.GetRank()+input.theDomain.GetRank()+1, input.theRange.GetRank()+input.theDomain.GetRank());
  this->PreimageWeylChamberLargerAlgebra.AddObjectOnTop(tempRoot);
  this->log << "\nWeyl chamber larger algebra after projectivizing: " << this->PreimageWeylChamberLargerAlgebra.ElementToString() << "\n";
  this->log << "\nWeyl chamber smaller algebra after projectivizing: " << this->WeylChamberSmallerAlgebra.ElementToString() << "\n";

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

void GeneralizedVermaModuleCharacters::GetProjection(int indexOperator, root& input, root& output)
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

void GeneralizedVermaModuleCharacters::ComputeQPsFromChamberComplex
(partFractions& thePFs, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  PolynomialOutputFormat theFormat;
  root tempRoot;
  this->theQPs.SetSize(thePFs.theChambers.size);
  for (int i=0; i<thePFs.theChambers.size; i++)
    if (thePFs.theChambers.TheObjects[i]!=0)
      if (!thePFs.theChambers.TheObjects[i]->flagPermanentlyZero)
      { this->theQPs.TheObjects[i].SetSize(this->theLinearOperators.size);
        for (int k=0; k<this->theLinearOperators.size; k++)
        { QuasiPolynomial& currentQP= this->theQPs.TheObjects[i].TheObjects[k];
          this->GetProjection(k, thePFs.theChambers.TheObjects[i]->InternalPoint, tempRoot);
          thePFs.partFractionsToPartitionFunctionAdaptedToRoot(currentQP, tempRoot, false, false, theGlobalVariables, true);
          out << "\nChamber " << i << " translation " << k << ": " << currentQP.ElementToString(false, theFormat);
        }
      }
  out << "\n\n" << thePFs.theChambers.DebugString;
  theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
  theGlobalVariables.MakeReport();
}

void ComputationSetup::ComputeGenVermaCharaterG2inB3(ComputationSetup& inputData, GlobalVariables& theGlobalVariables)
{ if (inputData.thePartialFraction.theChambers.flagSliceWithAWallInitDone)
  { while (inputData.thePartialFraction.theChambers.oneStepChamberSlice(theGlobalVariables)){}
    tempCharsEraseWillBeErasedShouldntHaveLocalObjectsLikeThis.ComputeQPsFromChamberComplex(inputData.thePartialFraction, theGlobalVariables);
  }
  else
  { inputData.theParser.theHmm.MakeG2InB3(inputData.theParser, theGlobalVariables);
    tempCharsEraseWillBeErasedShouldntHaveLocalObjectsLikeThis.initFromHomomorphism(inputData.theParser.theHmm, theGlobalVariables);
    tempCharsEraseWillBeErasedShouldntHaveLocalObjectsLikeThis.TransformToWeylProjective(inputData.thePartialFraction.theChambers, theGlobalVariables);
    std::stringstream out;
    inputData.thePartialFraction.initFromRoots(tempCharsEraseWillBeErasedShouldntHaveLocalObjectsLikeThis.GmodKnegativeWeights, theGlobalVariables);
    inputData.thePartialFraction.ComputeDebugString(theGlobalVariables);
    out << inputData.thePartialFraction.DebugString;
    inputData.thePartialFraction.split(theGlobalVariables, 0);
    inputData.thePartialFraction.ComputeDebugString(theGlobalVariables);
    out << "=" << inputData.thePartialFraction.DebugString;
    theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
    theGlobalVariables.MakeReport();
    QuasiPolynomial tempQP;
    PolynomialOutputFormat polyFormat;
    root tempRoot;
    tempRoot="(-1,-3)";
    inputData.thePartialFraction.partFractionsToPartitionFunctionAdaptedToRoot(tempQP, tempRoot, false, false, theGlobalVariables, true);
    out << "\nIndicator: " << tempRoot.ElementToString() << "\n" << tempQP.ElementToString(false, polyFormat);
    theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
    theGlobalVariables.MakeReport();

  }
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

void ParserNode::EvaluateWeylAction
  (GlobalVariables& theGlobalVariables, bool DualAction, bool useRho, bool useMinusRho)
{ if (this->children.size!=1)
  { this->SetError(this->errorProgramming);
    return;
  }
  ParserNode& theArgument=this->owner->TheObjects[this->children.TheObjects[0]];
  HomomorphismSemisimpleLieAlgebra& theHmm= this->owner->theHmm;
  Vector<RationalFunction> theWeight;
  if (theHmm.theRange.GetRank()>1)
  { if (!theArgument.ConvertChildrenToType(this->typeRationalFunction, theGlobalVariables)|| theHmm.theRange.GetRank()!=theArgument.children.size)
    { this->SetError(this->errorBadOrNoArgument);
      return;
    }
    int theDimension=theArgument.children.size;
    theWeight.SetSize(theDimension);
    for (int i=0; i<theDimension; i++)
    { ParserNode& current= this->owner->TheObjects[theArgument.children.TheObjects[i]];
      theWeight.TheObjects[i]=current.ratFunction.GetElement();
    }
  } else
    if (!theArgument.ConvertToType(this->typeRationalFunction, theGlobalVariables))
    { this->SetError(this->errorBadOrNoArgument);
      return;
    } else
    { theWeight.SetSize(1);
      theWeight.TheObjects[0]=theArgument.ratFunction.GetElement();
    }
  std::stringstream out;
  theHmm.theRange.theWeyl.ComputeWeylGroup(51840);
  if (theHmm.theRange.theWeyl.size>=51840)
    out << "Only the first 51840 elements have been computed. <br> If you want a larger computation <br> please use the C++ code directly.";
  out << "Number of elements: " << theHmm.theRange.theWeyl.size << "<br>";
  Vector<RationalFunction> theOrbitElement;
  RationalFunction RFZero;
  RFZero.Nullify(this->owner->NumVariables, &theGlobalVariables);
  for (int i=0; i<theHmm.theRange.theWeyl.size; i++)
  { theOrbitElement=theWeight;
    if (!DualAction)
      theHmm.theRange.theWeyl.ActOn<RationalFunction>(i, theOrbitElement, useRho, useMinusRho, RFZero);
    else
    {
    }
    out << theOrbitElement.ElementToString() << "<br>";
  }
  this->outputString=out.str();
  this->ExpressionType=this->typeString;
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
  if (ParallelComputing::GlobalPointerCounter>::ParallelComputing::cgiLimitRAMuseNumPointersInList){ std::cout <<"<b>Error:</b> Number of pointers allocated exceeded allowed limit of " <<::ParallelComputing::cgiLimitRAMuseNumPointersInList; std::exit(0); }
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
    if (ParallelComputing::GlobalPointerCounter>::ParallelComputing::cgiLimitRAMuseNumPointersInList){ std::cout <<"<b>Error:</b> Number of pointers allocated exceeded allowed limit of " <<::ParallelComputing::cgiLimitRAMuseNumPointersInList; std::exit(0); }
  #endif
            this->TheObjects[this->PreferredNextChambers.TheObjects[0]]=0;
          }
      this->PreferredNextChambers.PopIndexShiftUp(0);
      if (this->flagAnErrorHasOcurredTimeToPanic)
        this->ComputeDebugString();
    }
}
