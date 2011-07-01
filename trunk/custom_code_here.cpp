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
//  bool flagUsingNewSplit;
  List<MatrixLargeRational> theLinearOperators;
  std::fstream theMultiplicitiesMaxOutput;
  std::fstream theMultiplicitiesMaxOutputReport2;
  roots GmodKnegativeWeights;
  ConeGlobal PreimageWeylChamberLargerAlgebra;
  ConeGlobal WeylChamberSmallerAlgebra;
  List<QuasiPolynomial> theQPsNonSubstituted;
  List<List<QuasiPolynomial> > theQPsSubstituted;
  List<QuasiPolynomial> theMultiplicities;
  List<QuasiPolynomial> theMultiplicitiesExtremaCandidates;
  int tempDebugCounter;
  List<Rational> theCoeffs;
  roots theTranslations;
  roots theTranslationsProjected;
  partFractions thePfs;
//  List<List<roots> > paramSubChambers, nonParamVertices;
  List<List<QuasiPolynomial> > ExtremeQPsParamSubchambers;
//  List<roots> allParamSubChambersRepetitionsAllowed;
  List<Cone> allParamSubChambersRepetitionsAllowedConeForm;
  CombinatorialChamberContainer projectivizedChamberOld;
  ConeComplex projectivizedParamComplex;
  ConeComplex projectivizedChamber;
  std::stringstream log;
  Parser theParser;
  int computationPhase;

  void IncrementComputation(GlobalVariables& theGlobalVariables);
  GeneralizedVermaModuleCharacters(){ this->tempDebugCounter=1; this->computationPhase=0;}
  void WriteToFile
  (std::fstream& output, GlobalVariables& theGlobalVariables)
  ;
  bool ReadFromFile
  (std::fstream& input, GlobalVariables& theGlobalVariables)
  { std::string tempS;
    input >> tempS >> this->computationPhase;
    if (tempS!="ComputationPhase:")
      return false;
    return this->ReadFromFileNoComputationPhase(input, theGlobalVariables);
  }
  bool ReadFromFileNoComputationPhase
  (std::fstream& input, GlobalVariables& theGlobalVariables)
  ;
  void GetProjection(int indexOperator, const root& input, root& output);
  void FindMultiplicitiesExtremaStep1(GlobalVariables& theGlobalVariables);
  void FindMultiplicitiesExtremaStep2(GlobalVariables& theGlobalVariables);
  void FindMultiplicitiesExtremaStep3(GlobalVariables& theGlobalVariables);
  void FindMultiplicitiesExtremaStep4(GlobalVariables& theGlobalVariables);
  void ProcessExtremaOneChamber
  (Cone& input, GlobalVariables& theGlobalVariables)
  ;
  void ProcessOneParametricChamber
  (int numNonParams, int numParams, roots& inputNormals, List<roots>& theParamChambers, List<roots>& theNonParamVertices, GlobalVariables& theGlobalVariables)
  ;
  void ComputeQPsFromChamberComplex
  (GlobalVariables& theGlobalVariables)
  ;
  void GetSubFromIndex(QPSub& output, int theIndex);
  void GetSubFromNonParamArray
  (QPSub& output, roots& NonParams, int numParams)
  ;
  void initQPs
  (GlobalVariables& theGlobalVariables)
  ;
  void initFromHomomorphism
  (HomomorphismSemisimpleLieAlgebra& input, GlobalVariables& theGlobalVariables)
  ;
  void TransformToWeylProjective
  (WallData& output, GlobalVariables& theGlobalVariables)
  ;
  void TransformToWeylProjective
  (GlobalVariables& theGlobalVariables)
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
(GlobalVariables& theGlobalVariables)
{ this->projectivizedChamberOld.flagSliceWithAWallInitDone=true;
  this->projectivizedChamberOld.theDirections=this->GmodKnegativeWeights;
  this->projectivizedChamberOld.init();
  this->projectivizedChamberOld.SliceTheEuclideanSpace(theGlobalVariables, false);
  this->projectivizedChamberOld.drawOutput(theGlobalVariables.theDrawingVariables, this->projectivizedChamberOld.IndicatorRoot, 0);
  std::string tempS;
  this->projectivizedChamberOld.ElementToString(tempS);
  this->log << tempS;
  this->projectivizedChamberOld.NewHyperplanesToSliceWith.size=0;
  this->projectivizedChamberOld.theHyperplanes.size=0;
  this->projectivizedChamberOld.AmbientDimension=this->projectivizedChamberOld.AmbientDimension+this->theLinearOperators.TheObjects[0].NumCols+1;
  root wallToSliceWith;
//  roots oldDirections;
  for (int k=0; k<this->theLinearOperators.size; k++)
    for (int i=0; i<this->projectivizedChamberOld.size; i++)
      if (this->projectivizedChamberOld.TheObjects[i]!=0)
        for (int j=0; j<this->projectivizedChamberOld.TheObjects[i]->Externalwalls.size; j++)
        { this->TransformToWeylProjective(k, this->projectivizedChamberOld.TheObjects[i]->Externalwalls.TheObjects[j].normal, wallToSliceWith);
//          if (k==0)
//            oldDirections.AddOnBottomNoRepetition(wallToSliceWith);
          wallToSliceWith.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
          if (k>0)
            this->projectivizedChamberOld.NewHyperplanesToSliceWith.AddOnTopNoRepetition(wallToSliceWith);
          this->projectivizedChamberOld.theHyperplanes.AddObjectOnTopNoRepetitionOfObjectHash(wallToSliceWith);
        }
  this->log << "\n Projectivized walls to slice with(" << this->projectivizedChamberOld.NewHyperplanesToSliceWith.size << "):" ;
  for (int i=0; i<this->projectivizedChamberOld.NewHyperplanesToSliceWith.size; i++)
    this->log << "\n" << this->projectivizedChamberOld.NewHyperplanesToSliceWith.TheObjects[i].ElementToString();
  this->log << "\n";
  for (int i=0; i<this->projectivizedChamberOld.size; i++)
    if (this->projectivizedChamberOld.TheObjects[i]!=0)
      this->TransformToWeylProjective(this->projectivizedChamberOld, *this->projectivizedChamberOld.TheObjects[i], theGlobalVariables);
  root tempRoot;
  for (int i=0; i<this->projectivizedChamberOld.TheGlobalConeNormals.size; i++)
  { tempRoot.MakeZero(this->projectivizedChamberOld.AmbientDimension);
    int startingDim=this->projectivizedChamberOld.TheGlobalConeNormals.TheObjects[i].size;
    for (int j=0; j<startingDim; j++)
    { tempRoot.TheObjects[j]=this->projectivizedChamberOld.TheGlobalConeNormals.TheObjects[i].TheObjects[j];
      tempRoot.TheObjects[j+startingDim]=-tempRoot.TheObjects[j];
    }
    this->projectivizedChamberOld.TheGlobalConeNormals.TheObjects[i]=tempRoot;
  }
  roots tempRoots;
  this->projectivizedChamberOld.TheGlobalConeNormals.AddListOnTop(this->PreimageWeylChamberLargerAlgebra);
  this->log << "the global cone normals: " << this->projectivizedChamberOld.TheGlobalConeNormals.ElementToString();
  for (int i=0; i<this->WeylChamberSmallerAlgebra.size; i++)
  { root& currentWeylWall=this->WeylChamberSmallerAlgebra.TheObjects[i];
    this->projectivizedChamberOld.SliceWithAWall(currentWeylWall, theGlobalVariables);
    for (int j=0; j<this->projectivizedChamberOld.size; j++)
      if (this->projectivizedChamberOld.TheObjects[j]!=0)
      { CombinatorialChamber& current=*this->projectivizedChamberOld.TheObjects[j];
        if (root::RootScalarEuclideanRoot(current.InternalPoint, currentWeylWall).IsNegative())
          current.flagPermanentlyZero=true;
      }
  }
//  if(this->flagUsingNewSplit)
  { List<roots> tempChambers;
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
  }
  this->projectivizedChamberOld.ElementToString(tempS);
  this->log << tempS;
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.theIndicatorVariables.StatusString1=this->log.str();
  theGlobalVariables.MakeReport();
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
    this->theTranslations.TheObjects[i]-=theWeyl.rho;
    theProjection.ActOnAroot(this->theTranslations.TheObjects[i], this->theTranslationsProjected.TheObjects[i]);
    if (theWeyl.TheObjects[i].size%2==0)
      this->theCoeffs.TheObjects[i]=1;
    else
      this->theCoeffs.TheObjects[i]=-1;
  }
  this->log << "\n\n\nMatrix of the projection operator:\n" << theProjection.ElementToString(false, false);
  this->log << "\n\n\nMatrix form of the operators $u_w$, the translations and their projections (" << this->theLinearOperators.size << "):";
  List<MatrixLargeRational> tempList;
  for (int i=0; i<this->theLinearOperators.size; i++)
  { this->theLinearOperators.TheObjects[i].MultiplyOnTheLeft(theProjection);
    this->log << "\n\n" << this->theLinearOperators.TheObjects[i].ElementToString(false, false);
    this->log << this->theTranslations.TheObjects[i].ElementToString() << ";   " << this->theTranslationsProjected.TheObjects[i].ElementToString();
    tempList.AddOnTopNoRepetition(this->theLinearOperators.TheObjects[i]);
  }
  this->log << "\n\n\nThere are " << tempList.size << " different operators.";
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
  int totalDim=this->theTranslations.TheObjects[0].size-1;
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
  Lattice integralLattice;
  MatrixLargeRational theExtendedIntegralLatticeMatForm;
  this->theParser.theHmm.theRange.theWeyl.GetIntegralLatticeInSimpleCoordinates(integralLattice);
  out << "\nThe integral lattice:\n" << integralLattice.ElementToString(false, false);
  this->theMultiplicitiesMaxOutputReport2.flush();
  for (int i=0; i<this->projectivizedChamber.size; i++)
  { this->theQPsSubstituted.TheObjects[i].SetSize(this->theLinearOperators.size);
    for (int k=0; k<this->theLinearOperators.size; k++)
    { QuasiPolynomial& currentQPSub=this->theQPsSubstituted.TheObjects[i].TheObjects[k];
      this->GetProjection(k, this->projectivizedChamber.TheObjects[i].GetInternalPoint(), tempRoot);
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
      this->GetSubFromIndex(theSub, k);
      Lattice::GetHomogeneousSubMatFromSubIgnoreConstantTerms(theSub.RationalPolyForm, theExtendedIntegralLatticeMatForm, theGlobalVariables);
      this->theMultiplicitiesMaxOutputReport2 << "\nExtended lattice:\n " << theExtendedIntegralLatticeMatForm.ElementToString(false, false);
      this->theMultiplicitiesMaxOutputReport2.flush();
      //theExtendedIntegralLatticeMatForm.Resize(
//      theQPNoSub.Substitution(theSub.RationalPolyForm, theExtendedIntegralLatticeMatForm, currentQPSub, theGlobalVariables);
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
  this->projectivizedChamber.ComputeDebugString();
  out << "\n\n" << this->projectivizedChamber.DebugString;
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
  theGlobalVariables.MakeReport();
  this->theMultiplicitiesMaxOutputReport2 << out.str();
}

void ComputationSetup::ComputeGenVermaCharaterG2inB3(ComputationSetup& inputData, GlobalVariables& theGlobalVariables)
{ tempCharsEraseWillBeErasedShouldntHaveLocalObjectsLikeThis.IncrementComputation(theGlobalVariables);
}

void GeneralizedVermaModuleCharacters::IncrementComputation(GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  std::fstream input; std::string tempS;
  std::string theFileName="/home/todor/math/vectorpartition/trunk/GenVermaComputation.txt";
  CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(input, theFileName, false, false, false);
  int candidatePhase;
  input >> tempS >> candidatePhase;
  if (tempS=="ComputationPhase:")
  { this->computationPhase=candidatePhase;
    this->ReadFromFileNoComputationPhase(input, theGlobalVariables);
  } else
    this->computationPhase=0;
  input.close();
  switch (this->computationPhase)
  { case 0:
      this->theParser.theHmm.MakeG2InB3(this->theParser, theGlobalVariables);
      this->initFromHomomorphism(this->theParser.theHmm, theGlobalVariables);
      this->TransformToWeylProjective(theGlobalVariables);
      out << this->projectivizedChamber.ElementToString();
      break;
    case 1:
      this->projectivizedChamber.Refine(theGlobalVariables);
      out << this->projectivizedChamber.ElementToString();
      break;
    case 2:
      this->ComputeQPsFromChamberComplex(theGlobalVariables);
      break;
    case 3:
      this->FindMultiplicitiesExtremaStep1(theGlobalVariables);
      break;
    case 4:
      this->FindMultiplicitiesExtremaStep2(theGlobalVariables);
      break;
    case 5:
      this->FindMultiplicitiesExtremaStep3(theGlobalVariables);
      break;
    case 6:
      this->FindMultiplicitiesExtremaStep4(theGlobalVariables);
      break;
    default:
      break;
  }
  this->computationPhase++;
  theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.MakeReport();
  if (this->computationPhase < 10)
  { CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent(input, theFileName, false, true, false);
    this->WriteToFile(input, theGlobalVariables);
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

std::string roots::ElementsToInequalitiesString(bool useLatex, bool useHtml)
{ std::stringstream out;
  std::string theLetter="x";
  if (useLatex)
    out << "\\begin{array}{l}";
  for (int i=0; i<this->size; i++)
  { root& current=this->TheObjects[i];
    out << current.ElementToStringLetterFormat(theLetter, useLatex);
    if (useLatex)
      out << "\\geq 0\\\\";
    else
      out << "=>0\n";
    if (useHtml)
      out << "<br>";
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
(QPSub& output, roots& NonParams, int numParams)
{ int numNonParams=NonParams.size;
  MatrixLargeRational subRationalForm;
  subRationalForm.init(numParams, numParams+numNonParams-1);
  subRationalForm.NullifyAll();
  for (int k=0; k<numParams; k++)
    for (int l=0; l<numNonParams; l++)
      subRationalForm.elements[k][l]= NonParams.TheObjects[l].TheObjects[k];
  for (int l=0; l<numParams-1; l++)
    subRationalForm.elements[l][l+numNonParams]= 1;
  output.MakeSubFromMatrixRational(subRationalForm);
}

void GeneralizedVermaModuleCharacters::FindMultiplicitiesExtremaStep1(GlobalVariables& theGlobalVariables)
{ //this->paramSubChambers.SetSize(this->projectivizedChamber.size);
  //this->nonParamVertices.SetSize(this->projectivizedChamber.size);
  int theDimension=6;
  if (this->projectivizedChamber.size>0)
    theDimension=this->projectivizedChamber.TheObjects[0].Normals.TheObjects[0].size;
  this->ExtremeQPsParamSubchambers.SetSize(this->projectivizedChamber.size);
//  this->allParamSubChambersRepetitionsAllowed.MakeActualSizeAtLeastExpandOnTop(this->projectivizedChamber.size*theDimension);
//  this->allParamSubChambersRepetitionsAllowed.size=0;
  std::stringstream out;
  QPSub subForFindingExtrema;
  QuasiPolynomial currentExtremaCandidate;
  int numParams=0, numNonParams=0;
  if (this->theLinearOperators.size>0)
  { numParams=this->theLinearOperators.TheObjects[0].NumCols+1;
    numNonParams=this->theLinearOperators.TheObjects[0].NumRows;
  }
  List<roots> currentParamChamberList, currentNonParamVerticesList;
  this->theMultiplicitiesExtremaCandidates.MakeActualSizeAtLeastExpandOnTop(this->projectivizedChamber.size*theDimension);
  this->allParamSubChambersRepetitionsAllowedConeForm.MakeActualSizeAtLeastExpandOnTop(this->projectivizedChamber.size*theDimension);
  Cone currentCone;
  theGlobalVariables.theIndicatorVariables.String1NeedsRefresh=true;
  theGlobalVariables.theIndicatorVariables.String2NeedsRefresh=true;
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
      { this->GetSubFromNonParamArray(subForFindingExtrema, currentNonParamVerticesList.TheObjects[j], numParams);
        this->theMultiplicities.TheObjects[i].Substitution(subForFindingExtrema.RationalPolyForm, currentExtremaCandidate, theGlobalVariables);
        this->theMultiplicitiesExtremaCandidates.AddObjectOnTop(currentExtremaCandidate);
        this->allParamSubChambersRepetitionsAllowedConeForm.AddObjectOnTop(currentCone);
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
}

void GeneralizedVermaModuleCharacters::FindMultiplicitiesExtremaStep4(GlobalVariables& theGlobalVariables)
{ this->theMultiplicitiesExtremaCandidates.SetSize(this->projectivizedParamComplex.size);
  theGlobalVariables.theIndicatorVariables.String1NeedsRefresh=true;
  CGIspecificRoutines::OpenDataFileOrCreateIfNotPresent
  (this->theMultiplicitiesMaxOutput, "/home/todor/math/vectorpartition/trunk/ExtremaOutput.txt", false, true, false);
  for (int i=0; i<this->projectivizedParamComplex.size; i++)
  //if (i<this->tempDebugCounter)
  { Cone& currentCone= this->projectivizedParamComplex.TheObjects[i];
    std::stringstream out3;
    out3 << "Processing extrema in chamber " << i+1 << " out of " << this->projectivizedParamComplex.size;
    theGlobalVariables.theIndicatorVariables.ProgressReportString1=out3.str();
    theGlobalVariables.MakeReport();
    this->theMultiplicitiesMaxOutput << "\n\n\n\n\nChamber " << i+ 1;
    this->ProcessExtremaOneChamber(currentCone, theGlobalVariables);
  }
  this->theMultiplicitiesMaxOutput.close();
}

void GeneralizedVermaModuleCharacters::ProcessExtremaOneChamber
  (Cone& input, GlobalVariables& theGlobalVariables)
{ std::stringstream out3, out1, out2;
//  int projectiveDimension=input.GetDim();
  List<QuasiPolynomial> theExtremaOutput, extremaCandidates;
  for (int j=0; j<this->allParamSubChambersRepetitionsAllowedConeForm.size; j++)
  // if (j<this->tempDebugCounter)
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

}

bool ParserNode::ExtractArgumentList
(List<int>& outputArgumentIndices)
{ if (this->children.size!=1)
    return false;
  ParserNode& theArgument=this->owner->TheObjects[this->children.TheObjects[0]];
  if (theArgument.ExpressionType==this->typeArray)
    outputArgumentIndices=theArgument.array.GetElement();
  else
    outputArgumentIndices.initFillInObject(1, this->children.TheObjects[0]);
  return true;
}

int ParserNode::EvaluateChamberParam(GlobalVariables& theGlobalVariables)
{ List<int> theArguments;
  this->ExtractArgumentList(theArguments);
  GeneralizedVermaModuleCharacters tempChars;
  if (theArguments.size<=2)
    return this->SetError(this->errorBadOrNoArgument);
  if (!this->owner->TheObjects[theArguments.TheObjects[0]].ConvertToType(this->typeIntegerOrIndex, theGlobalVariables) || !this->owner->TheObjects[theArguments.TheObjects[1]].ConvertToType(this->typeIntegerOrIndex, theGlobalVariables))
    return this->SetError(this->errorBadOrNoArgument);
  int dimNonParam=this->owner->TheObjects[theArguments.TheObjects[0]].intValue;
  int dimParam=this->owner->TheObjects[theArguments.TheObjects[1]].intValue;
  roots theWalls;
  root tempRoot;
  List<int> nodesCurrentRoot;
  for (int i=2; i<theArguments.size; i++)
  { ParserNode& currentNode=this->owner->TheObjects[theArguments.TheObjects[i]];
    if (!currentNode.ConvertToType(this->typeArray, theGlobalVariables))
      return this->SetError(this->errorBadOrNoArgument);
    nodesCurrentRoot= currentNode.array.GetElement();
    if (nodesCurrentRoot.size!=dimNonParam+dimParam)
      return this->SetError(this->errorBadOrNoArgument);
    tempRoot.SetSize(nodesCurrentRoot.size);
    for (int j=0; j<nodesCurrentRoot.size; j++)
    { ParserNode& currentCoordinateNode=this->owner->TheObjects[nodesCurrentRoot.TheObjects[j]];
      if (!currentCoordinateNode.ConvertToType(this->typeRational, theGlobalVariables))
        return this->SetError(this->errorBadOrNoArgument);
      tempRoot.TheObjects[j]=currentCoordinateNode.rationalValue;
    }
    theWalls.AddObjectOnTop(tempRoot);
  }
  List<roots> outputParamChambers, outputNonParamVertices;
  tempChars.ProcessOneParametricChamber(dimNonParam, dimParam, theWalls, outputParamChambers, outputNonParamVertices, theGlobalVariables);
  std::stringstream out;
  out << "<div class=\"math\">" << theWalls.ElementsToInequalitiesString(true, false) << "</div>";
  this->outputString=out.str();
  this->ExpressionType=(this->typeString);
  return this->errorNoError;
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
  newPlusCone.LowestIndexIHaventBeenCheckedForSplitting=myDyingCone.LowestIndexIHaventBeenCheckedForSplitting+1;
  newMinusCone.LowestIndexIHaventBeenCheckedForSplitting=myDyingCone.LowestIndexIHaventBeenCheckedForSplitting+1;
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
  for (; currentCone.LowestIndexIHaventBeenCheckedForSplitting<this->splittingNormals.size; currentCone.LowestIndexIHaventBeenCheckedForSplitting++)
    if (this->SplitChamber(this->indexLowestNonRefinedChamber, this->splittingNormals.TheObjects[currentCone.LowestIndexIHaventBeenCheckedForSplitting], theGlobalVariables))
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

std::string Cone::ElementToString(bool useLatex, bool useHtml)
{ std::stringstream out;
  out << "Index next wall to refine by: " << this->LowestIndexIHaventBeenCheckedForSplitting << "\n";
  if (useHtml)
    out << "<br>";
  out << "Normals:\n";
  if (useHtml)
    out << "<br>";
  out << this->Normals.ElementsToInequalitiesString(useLatex, useHtml);
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

int ParserNode::EvaluateCone(GlobalVariables& theGlobalVariables)
{ List<int> argumentsList;
  this->ExtractArgumentList(argumentsList);
  int theDim=-1;
  roots coneWalls; root currentWall;
  for (int i=0; i<argumentsList.size; i++)
  { ParserNode& currentNode=this->owner->TheObjects[argumentsList.TheObjects[i]];
    if (!currentNode.ConvertToType(this->typeArray, theGlobalVariables))
      return this->SetError(this->errorBadOrNoArgument);
    if (theDim==-1)
      theDim=currentNode.array.GetElement().size;
    if (currentNode.array.GetElement().size!=theDim)
      return this->SetError(this->errorDimensionProblem);
    currentWall.SetSize(theDim);
    for (int j=0; j<currentNode.array.GetElement().size; j++)
    { ParserNode& currentCoord=this->owner->TheObjects[currentNode.array.GetElement().TheObjects[j]];
      if (!currentCoord.ConvertToType(this->typeRational, theGlobalVariables))
        return this->SetError(this->errorBadOrNoArgument);
      currentWall.TheObjects[j]=currentCoord.rationalValue;
    }
    coneWalls.AddObjectOnTop(currentWall);
  }
  std::stringstream out;
  if (!this->theCone.GetElement().CreateFromNormals(coneWalls, theGlobalVariables))
    out << "the cone has too few or no vertices.";
  out << "<br>Cone walls input: " << coneWalls.ElementToString(false, false, false);
  out << "<br><br>Cone:" << this->theCone.GetElement().ElementToString(false, true);
  this->outputString=out.str();

  this->ExpressionType=this->typeCone;
  return this->errorNoError;
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

void Lattice::Reduce
()
{ LargeInt tempInt, tempInt2;
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

int ParserNode::EvaluateLattice(GlobalVariables& theGlobalVariables)
{ List<int> theArgumentList;
  this->ExtractArgumentList(theArgumentList);
  if (theArgumentList.size<1)
    return this->SetError(this->errorBadOrNoArgument);
  int theDim=this->owner->TheObjects[theArgumentList.TheObjects[0]].array.GetElement().size;
  root currentRoot;
  roots LatticeBase;
  currentRoot.SetSize(theDim);
  for (int i=0; i<theArgumentList.size; i++)
  { ParserNode& currentNode=this->owner->TheObjects[theArgumentList.TheObjects[i]];
    if (!currentNode.ConvertToType(this->typeArray, theGlobalVariables))
      return this->SetError(this->errorBadOrNoArgument);
    if (currentNode.array.GetElement().size!=theDim)
      return this->SetError(this->errorDimensionProblem);
    for (int j=0; j<currentNode.array.GetElement().size; j++)
    { ParserNode& currentCoord=this->owner->TheObjects[currentNode.array.GetElement().TheObjects[j]];
      if (!currentCoord.ConvertToType(this->typeRational, theGlobalVariables))
        return this->SetError(this->errorConversionError);
      currentRoot.TheObjects[j]=currentCoord.rationalValue;
    }
    LatticeBase.AddObjectOnTop(currentRoot);
  }
  this->theLattice.GetElement().MakeFromRoots(LatticeBase);
  this->outputString=this->theLattice.GetElement().ElementToString();
  this->ExpressionType=this->typeLattice;
  return this->errorNoError;
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
  (const Lattice& rougherLattice, roots& output)
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

void Lattice::IntersectWithPreimageOfLattice
  (const MatrixLargeRational& theLinearMap, const Lattice& other, GlobalVariables& theGlobalVariables)
{ roots startingBasis, imageStartingBasis, basisImageIntersection, basisImageIntersectionInCoordsWRTimageStartingBasis, ImageBasisInImageStartingBasisCoords;
  roots resultNonKernelPart, resultKernelPart, result, tempRoots;
  startingBasis.AssignMatrixRows(this->basisRationalForm);
  std::cout << "<br>Starting basis: " << startingBasis.ElementToString();
  theLinearMap.ActOnRoots(startingBasis, imageStartingBasis);
  std::cout << "<br>Image of starting basis: " << imageStartingBasis.ElementToString();
  Lattice ImageLattice;
  ImageLattice.MakeFromRoots(imageStartingBasis);
  ImageLattice.IntersectWith(other);
  std::cout << "<br>Image lattice: " << ImageLattice.ElementToString(true, false);
  basisImageIntersection.AssignMatrixRows(ImageLattice.basisRationalForm);
  basisImageIntersection.GetCoordsInBasis(imageStartingBasis, ImageBasisInImageStartingBasisCoords, theGlobalVariables);
  std::cout << "<br> Basis (of image lattice) in image of starting basis coordinates: " << ImageBasisInImageStartingBasisCoords.ElementToString();
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
  std::cout << "<br>this basis: " << startBasis.ElementToString();
  std::cout << "<br>other basis: " << otherBasis.ElementToString();
  startBasis.IntersectTwoLinSpaces(startBasis, otherBasis, commonBasis, theGlobalVariables);
  std::cout << "<br> basis of linear space intersection: " << commonBasis.ElementToString() << "<br><br> ";
  Lattice thisLatticeIntersected, otherLatticeIntersected;
  thisLatticeIntersected=*this;
  otherLatticeIntersected=other;
  thisLatticeIntersected.IntersectWithLinearSubspaceSpannedBy(commonBasis);
  std::cout << "<br> linear space intersected with this lattice: " << thisLatticeIntersected.ElementToString(true, false);
  otherLatticeIntersected.IntersectWithLinearSubspaceSpannedBy(commonBasis);
  std::cout << "<br> linear space intersected with other lattice: " << otherLatticeIntersected.ElementToString(true, false);
  roots thisCommonBasis, otherCommonBasis, thisCommonCoords, otherCommonCoords;
  thisCommonBasis.AssignMatrixRows(thisLatticeIntersected.basisRationalForm);
  otherCommonBasis.AssignMatrixRows(otherLatticeIntersected.basisRationalForm);
  thisCommonBasis.GetCoordsInBasis(commonBasis, thisCommonCoords, theGlobalVariables);
  otherCommonBasis.GetCoordsInBasis(commonBasis, otherCommonCoords, theGlobalVariables);
  std::cout << "<br>this lattice intersection new coords: " << thisCommonBasis.ElementToString();
  std::cout << "<br>other lattice intersection new coords: " << otherBasis.ElementToString();
  Lattice thisCommonCoordsLattice, otherCommonCoordsLattice;
  thisCommonCoordsLattice.MakeFromRoots(thisCommonCoords);
  otherCommonCoordsLattice.MakeFromRoots(otherCommonCoords);
  std::cout << "<br> linear space intersected with this lattice new coords: " << thisCommonCoordsLattice.ElementToString(true, false);
  std::cout << "<br> linear space intersected with other lattice new coords: " << otherCommonCoordsLattice.ElementToString(true, false);
  thisCommonCoordsLattice.IntersectWithBothOfMaxRank(otherCommonCoordsLattice);
  std::cout << "<br> intersection lattice new coords: " << thisCommonCoordsLattice.ElementToString(true, false);
  roots resultBasis;
  resultBasis.SetSize(thisCommonCoordsLattice.basisRationalForm.NumRows);
  for (int i=0; i<resultBasis.size; i++)
  { root& currentRoot=resultBasis.TheObjects[i];
    currentRoot.MakeZero(this->GetDim());
    for (int j=0; j< thisCommonCoordsLattice.basisRationalForm.NumCols; j++)
      currentRoot+=thisCommonCoordsLattice.basisRationalForm.elements[i][j]*commonBasis.TheObjects[j];
  }
  this->MakeFromRoots(resultBasis);
  std::cout << "<br>final answer: intersection is: " << this->ElementToString(true, false);
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
  for (int i=0; i<this->LatticeShifts.size; i++)
  { //if(useHtml)
      //out << "<br>Shift: " << this->LatticeShifts.TheObjects[i].ElementToString() << "; polynomial: ";
    out << this->valueOnEachLatticeShift.TheObjects[i].ElementToString(thePolyFormat);
    out << " over ";
    if (!this->LatticeShifts.TheObjects[i].IsEqualToZero())
      out << this->LatticeShifts.TheObjects[i].ElementToString() << " + ";
    out << "L; ";
  }
  out << " where L= <";
  roots tempRoots;
  tempRoots.AssignMatrixRows(this->AmbientLatticeReduced.basisRationalForm);
  for (int i=0; i<tempRoots.size; i++)
  { out << tempRoots.TheObjects[i].ElementToString();
    if (i!=tempRoots.size-1)
      out << ", ";
  }
  out << ">";
  return out.str();
}

bool ParserNode::GetRootInt(Vector<int>& output, GlobalVariables& theGlobalVariables)
{ if (this->ExpressionType!=this->typeArray)
  { if (!this->ConvertToType(this->typeIntegerOrIndex, theGlobalVariables))
      return false;
    output.SetSize(1);
    output.TheObjects[0]=this->intValue;
    return true;
  }
  output.SetSize(this->array.GetElement().size);
  for (int i=0; i<this->array.GetElement().size; i++)
    if (!this->owner->TheObjects[this->array.GetElement().TheObjects[i]].ConvertToType(this->typeIntegerOrIndex, theGlobalVariables))
      return false;
    else
      output.TheObjects[i]=this->owner->TheObjects[this->array.GetElement().TheObjects[i]].intValue;
  return true;
}

bool ParserNode::GetRootRational(root& output, GlobalVariables& theGlobalVariables)
{ if (this->ExpressionType!=this->typeArray)
  { if (!this->ConvertToType(this->typeRational, theGlobalVariables))
      return false;
    output.SetSize(1);
    output.TheObjects[0]=this->rationalValue;
    return true;
  }
  output.SetSize(this->array.GetElement().size);
  for (int i=0; i<this->array.GetElement().size; i++)
    if (!this->owner->TheObjects[this->array.GetElement().TheObjects[i]].ConvertToType(this->typeRational, theGlobalVariables))
      return false;
    else
      output.TheObjects[i]=this->owner->TheObjects[this->array.GetElement().TheObjects[i]].rationalValue;
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
  this->basis.MakeIdMatrix(theDim);
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

int ParserNode::EvaluateSplit(GlobalVariables& theGlobalVariables)
{ List<int> argumentList;
  this->ExtractArgumentList(argumentList);
  if (argumentList.size!=1)
    return this->SetError(this->errorBadOrNoArgument);
  ParserNode& thePFNode=this->owner->TheObjects[argumentList.TheObjects[0]];
  this->thePFs.GetElement()=thePFNode.thePFs.GetElement();
  this->thePFs.GetElement().split(theGlobalVariables, 0);
  std::string tempS;
  std::stringstream out;
  this->thePFs.GetElement().ElementToString(tempS, theGlobalVariables);
  out << "<div class=\"math\">" << tempS << "</div>";
  this->outputString=out.str();
  this->ExpressionType=this->typePartialFractions;
  return this->errorNoError;
}

int ParserNode::EvaluatePartialFractions(GlobalVariables& theGlobalVariables)
{ List<int> argumentList;
  this->ExtractArgumentList(argumentList);
  if (argumentList.size==0)
    return this->SetError(this->errorBadOrNoArgument);
  roots theVectors;
  theVectors.SetSize(argumentList.size);
  for (int i=0; i<argumentList.size; i++)
  { ParserNode& currentNode=this->owner->TheObjects[argumentList.TheObjects[i]];
    if (!currentNode.GetRootRational(theVectors.TheObjects[i], theGlobalVariables))
      return this->SetError(this->errorBadOrNoArgument);
    if (theVectors.TheObjects[i].size!=theVectors.TheObjects[0].size)
      return this->SetError(this->errorDimensionProblem);
  }
  if (!this->thePFs.GetElement().initFromRoots(theVectors, theGlobalVariables))
    return this->SetError(this->errorImplicitRequirementNotSatisfied);
  std::string tempS;
  std::stringstream out;
  this->thePFs.GetElement().ElementToString(tempS, theGlobalVariables);
  out << "<div class=\"math\">" << tempS << "</div>";
  this->outputString=out.str();
  this->ExpressionType=this->typePartialFractions;
  return this->errorNoError;
}

int ParserNode::EvaluateVectorPFIndicator(GlobalVariables& theGlobalVariables)
{ List<int> argumentList;
  this->ExtractArgumentList(argumentList);
  if (argumentList.size!=2)
    return this->SetError(this->errorBadOrNoArgument);
  ParserNode& PFNode=this->owner->TheObjects[argumentList.TheObjects[0]];
  ParserNode& IndicatorNode=this->owner->TheObjects[argumentList.TheObjects[1]];
  if (!PFNode.ConvertToType(this->typePartialFractions, theGlobalVariables))
    return this->SetError(this->errorBadOrNoArgument);
  root theIndicator;
  if (!IndicatorNode.GetRootRational(theIndicator, theGlobalVariables))
    return this->SetError(this->errorBadOrNoArgument);
  partFractions& currentPF=PFNode.thePFs.GetElement();
  QuasiPolynomial& currentQP=this->theQP.GetElement();
  std::stringstream out;
  std::string startingPFString, splitPFString ;
  currentPF.ElementToString(startingPFString, theGlobalVariables);
  currentPF.split(theGlobalVariables, 0);
  currentPF.ElementToString(splitPFString, theGlobalVariables);
  currentPF.GetVectorPartitionFunction(currentQP, theIndicator, theGlobalVariables);
  this->ExpressionType=this->typeQuasiPolynomial;
  out << currentQP.ElementToString(true, false) << "<br><div class=\"math\">" << startingPFString << "</div>=<div class=\"math\">" << splitPFString << " </div>";
  this->outputString=out.str();
  return this->errorNoError;
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

bool QuasiPolynomial::Substitution
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
{ std::cout << "<br>Starting lattice: " << this->ElementToString(true, false) << "<br>";
  roots startingBasis, resultBasis;
  startingBasis.AssignMatrixRows(this->basisRationalForm);
  std::cout << "Starting normal: " << theNormal.ElementToString();
  root theScalarProducts;
  theScalarProducts.SetSize(startingBasis.size);
  for (int i=0; i<this->basisRationalForm.NumRows; i++)
    theScalarProducts.TheObjects[i]=root::RootScalarEuclideanRoot(startingBasis.TheObjects[i], theNormal);
  if (theScalarProducts.IsEqualToZero())
    return;
  std::cout << "<br>the scalar products: " << theScalarProducts.ElementToString();
  int pivotColumnIndex=theScalarProducts.getIndexLastNonZeroCoordinate();
  Rational pivotCoeff=theScalarProducts.TheObjects[pivotColumnIndex];
  theScalarProducts/=-pivotCoeff;
  std::cout << "<br>the scalar products after scaling: " << theScalarProducts.ElementToString();
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
  std::cout << "<br>the scalar products after scaling to integral: " << theScalarProducts.ElementToString();
  eigenSpacePlusOrthogonalComponent.TheObjects[pivotColumnIndex]=theScalarProducts;
  std::cout << "<br>The eigenspace before intersection: " << eigenSpacePlusOrthogonalComponent.ElementToString();
  Lattice eigenLattice, theZnLattice;
  eigenLattice.MakeFromRoots(eigenSpacePlusOrthogonalComponent);
  std::cout << "<br>The eigen-Lattice: " << eigenLattice.ElementToString(true, false);
  theZnLattice.MakeZn(theScalarProducts.size);
  std::cout << "<br>The Zn-Lattice: " << theZnLattice.ElementToString(true, false);
  theZnLattice.IntersectWithBothOfMaxRank(eigenLattice);
  std::cout << "<br>Zn intersected with eigen-Lattice: " << theZnLattice.ElementToString(true, false);
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
  std::cout << "<br>Resulting basis: " << resultBasis.ElementToString();
  this->MakeFromRoots(resultBasis);
  std::cout << "<br>Final answer lattice form: " << this->ElementToString(true, false);
}

void Lattice::IntersectWithLinearSubspaceSpannedBy(const roots& theSubspaceBasis)
{ roots theNormals;
  MatrixLargeRational theMat;
  theSubspaceBasis.GetMatRootsToRows(theMat);
  theMat.FindZeroEigenSpace(theNormals);
  this->IntersectWithLinearSubspaceGivenByNormals(theNormals);
}

void Lattice::IntersectWithLinearSubspaceGivenByNormals(const roots& theSubspaceNormals)
{ for (int i=0; i<theSubspaceNormals.size; i++)
    this->IntersectWithLinearSubspaceGivenByNormal(theSubspaceNormals.TheObjects[i]);
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
  theGlobalVariables.theIndicatorVariables.String1NeedsRefresh=true;
  theGlobalVariables.theIndicatorVariables.ProgressReportString1="Writing small data... ";
  theGlobalVariables.MakeReport();
//  this->allParamSubChambersRepetitionsAllowed.WriteToFile(output, theGlobalVariables);
  this->allParamSubChambersRepetitionsAllowedConeForm.WriteToFile(output, theGlobalVariables);
  this->ExtremeQPsParamSubchambers.WriteToFile(output, theGlobalVariables);
  this->GmodKnegativeWeights.WriteToFile(output, theGlobalVariables);
  this->theLinearOperators.WriteToFile(output);
  this->PreimageWeylChamberLargerAlgebra.WriteToFile(output, theGlobalVariables);
  this->WeylChamberSmallerAlgebra.WriteToFile(output, theGlobalVariables);
  theGlobalVariables.theIndicatorVariables.ProgressReportString1="Writing QP's non-subbed... ";
  theGlobalVariables.MakeReport();
  this->theQPsNonSubstituted.WriteToFile(output, theGlobalVariables);
  theGlobalVariables.theIndicatorVariables.ProgressReportString1="Writing QP's subbed... ";
  theGlobalVariables.MakeReport();
  this->theQPsSubstituted.WriteToFile(output, theGlobalVariables);
  theGlobalVariables.theIndicatorVariables.ProgressReportString1="Writing small data... ";
  theGlobalVariables.MakeReport();
  this->theMultiplicities.WriteToFile(output, theGlobalVariables);
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
  this->projectivizedChamber.WriteToFile(output, theGlobalVariables);
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
{ this->theParser.theHmm.MakeG2InB3(this->theParser, theGlobalVariables);
  theGlobalVariables.theIndicatorVariables.String1NeedsRefresh=true;
//  this->allParamSubChambersRepetitionsAllowed.ReadFromFile(input, theGlobalVariables);
  theGlobalVariables.theIndicatorVariables.ProgressReportString1="Loading param subchambers cone form... ";
  theGlobalVariables.MakeReport();
  this->allParamSubChambersRepetitionsAllowedConeForm.ReadFromFile(input, theGlobalVariables);
  theGlobalVariables.theIndicatorVariables.ProgressReportString1="Loading extreme QPs subchambers...";
  theGlobalVariables.MakeReport();
  this->ExtremeQPsParamSubchambers.ReadFromFile(input, theGlobalVariables);
  theGlobalVariables.theIndicatorVariables.ProgressReportString1="Loading more pieces of data... ";
  theGlobalVariables.MakeReport();
  this->GmodKnegativeWeights.ReadFromFile(input, theGlobalVariables);
  this->theLinearOperators.ReadFromFile(input, theGlobalVariables);
  this->PreimageWeylChamberLargerAlgebra.ReadFromFile(input, theGlobalVariables);
  this->WeylChamberSmallerAlgebra.ReadFromFile(input, theGlobalVariables);
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
  theGlobalVariables.theIndicatorVariables.ProgressReportString1="Loading complete... ";
  theGlobalVariables.MakeReport();
  return true;
}

void ConeComplex::WriteToFile
  (std::fstream& output, GlobalVariables& theGlobalVariables)
{ this->List<Cone>::WriteToFile(output, theGlobalVariables);
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

int ParserNode::EvaluatePrintRootSystem(GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  WeylGroup& theWeyl=this->owner->theHmm.theRange.theWeyl;
  out << "<br>Symmetric Cartan matrix in Bourbaki order:<br><div class=\"math\">" << this->owner->theHmm.theRange.theWeyl.CartanSymmetric.ElementToString(false, true) << "</div>";
  Rational tempRat;
  MatrixLargeRational tempMat;
  tempMat=  theWeyl.CartanSymmetric;
  tempMat.ComputeDeterminantOverwriteMatrix(tempRat);
  out  << "The determinant of the symmetric Cartan matrix is: " << tempRat.ElementToString();
  out << "<br>Root system:";
  for (int i=0; i<theWeyl.RootSystem.size; i++)
  { root& current=this->owner->theHmm.theRange.theWeyl.RootSystem.TheObjects[i];
    out << "<br>" << current.ElementToString();
  }
  this->outputString=out.str();
  this->ExpressionType=this->typeString;
  return this->errorNoError;
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

int ParserNode::EvaluateIntersectLatticeWithPreimageLattice(GlobalVariables& theGlobalVariables)
{ List<int> argumentList;
  this->ExtractArgumentList(argumentList);
  if (argumentList.size<3)
    return this->SetError(this->errorBadOrNoArgument);
  roots theLinearMap;
  root tempRoot;
  ParserNode& firstLatticeNode=this->owner->TheObjects[0];
  ParserNode& secondLatticeNode=this->owner->TheObjects[1];
  if (!firstLatticeNode.ConvertToType(this->typeLattice, theGlobalVariables))
    return this->SetError(this->errorBadOrNoArgument);
  if (!secondLatticeNode.ConvertToType(this->typeLattice, theGlobalVariables))
    return this->SetError(this->errorBadOrNoArgument);
  int theDim=-1;
  for (int i=2; i<argumentList.size; i++)
  { ParserNode& currentNode=this->owner->TheObjects[i];
    if (!currentNode.GetRootRational(tempRoot, theGlobalVariables))
      return this->SetError(this->errorBadOrNoArgument);
    if (theDim==-1)
      theDim=tempRoot.size;
    if (theDim!=tempRoot.size)
      return this->SetError(this->errorDimensionProblem);
    theLinearMap.AddObjectOnTop(tempRoot);
  }
  MatrixLargeRational theLinearMapMat;
  theLinearMapMat.AssignRootsToRowsOfMatrix(theLinearMap);
  if (firstLatticeNode.theLattice.GetElement().GetDim()!=theLinearMapMat.NumCols || secondLatticeNode.theLattice.GetElement().GetDim()!=theLinearMapMat.NumRows)
    return this->SetError(this->errorDimensionProblem);
  this->theLattice.GetElement()=firstLatticeNode.theLattice.GetElement();
  this->theLattice.GetElement().IntersectWithPreimageOfLattice(theLinearMapMat, secondLatticeNode.theLattice.GetElement(), theGlobalVariables);
  this->ExpressionType=this->typeLattice;
  return this->errorNoError;
}
