//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader1.h"
#include "vpfHeader1_2.h"
ProjectInformationInstance ProjectInfoVpf8cpp(__FILE__, "Second main implementation file. ");


void LargeIntUnsigned::AssignString(const std::string& input)
{ if (input.size()<10)
  { unsigned int x=std::atoi(input.c_str());
    this->AssignShiftedUInt(x, 0);
    return;
  }
  this->MakeZero();
  for (unsigned int i=0; i<input.size(); i++)
  { this->operator*=(10);
    this->operator+=((unsigned) MathRoutines::GetIntFromDigit(input[i]));
  }
}

void Rational::AssignString(const std::string& input)
{ this->MakeZero();
  if (input=="0")
    return;
  bool inputIsNegative=false;
  int positionInTempS=0;
  if (input[0]=='-')
  { positionInTempS++;
    inputIsNegative=true;
  }
  std::string buffer;
  buffer.reserve(input.size());
  LargeIntUnsigned reader;
  bool readingNumerator=true;
  for (unsigned i=positionInTempS; i<input.length(); i++)
  { char a= input[i];
    if (a=='/')
    { readingNumerator=false;
      reader.AssignString(buffer);
      this->operator=(reader);
      buffer="";
    } else
      buffer.push_back(input[i]);
  }
  reader.AssignString(buffer);
  if (readingNumerator)
    this->operator=(reader);
  else
    this->operator/=(reader);
  if (inputIsNegative)
    this->Minus();
}

void GeneralizedVermaModuleCharacters::TransformToWeylProjective
  (int indexOperator, Vector<Rational>& startingNormal, Vector<Rational>& outputNormal)
{ Matrix<Rational>  theOperatorExtended=this->theLinearOperatorsExtended.TheObjects[indexOperator];
  Vector<Rational>& theTranslation=this->theTranslationsProjectedBasisChanged.TheObjects[indexOperator];
  //the goddamned sign in front of theTranslation is now checked: it should be + and not -
  Rational theConst=Vector<Rational>::ScalarEuclidean
  (this->NonIntegralOriginModificationBasisChanged+theTranslation, startingNormal);
  theOperatorExtended.Transpose();
  outputNormal=startingNormal;
  theOperatorExtended.ActOnVectorColumn(outputNormal);
  outputNormal.SetSize(outputNormal.size+1);
  *outputNormal.LastObject()=-theConst;
}

void GeneralizedVermaModuleCharacters::TransformToWeylProjectiveStep1
(GlobalVariables& theGlobalVariables)
{ this->smallerAlgebraChamber.InitFromDirectionsAndRefine(this->GmodKNegWeightsBasisChanged, theGlobalVariables);
  theGlobalVariables.theIndicatorVariables.StatusString1=this->smallerAlgebraChamber.ElementToString(false, false);
  theGlobalVariables.MakeReport();
  this->log << "Directions for making the chamber basis changed: " << this->GmodKNegWeightsBasisChanged.ElementToString()
  << "\n Resulting chamber before projectivization:\n " << this->smallerAlgebraChamber.ElementToString(false, false);
  theGlobalVariables.theIndicatorVariables.StatusString1=this->log.str();
}

void GeneralizedVermaModuleCharacters::TransformToWeylProjectiveStep2
(GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  ConeComplex projectivizedChamberFinal;
  Cone currentProjectiveCone;
  Vectors<Rational> tempRoots, extraWeylChamberWalls;
  Vector<Rational> tempRoot, wallAtInfinity, wallToSliceWith;
//  int dimSmallerAlgebra=this->theLinearOperators.TheObjects[0].NumRows;
//  int dimLargerAlgebra=this->theLinearOperators.TheObjects[0].NumCols;
//  int dimFinal=dimSmallerAlgebra+dimLargerAlgebra+1;
  projectivizedChamberFinal.init();
  for (int i=0; i<this->smallerAlgebraChamber.size; i++)
  { Cone& currentAffineCone=this->smallerAlgebraChamber.TheObjects[i];
    tempRoots.SetSize(currentAffineCone.Normals.size);
    for (int j=0; j<currentAffineCone.Normals.size; j++)
      this->TransformToWeylProjective(0, currentAffineCone.Normals[j], tempRoots[j]);
    tempRoots.AddListOnTop(this->PreimageWeylChamberLargerAlgebra.Normals);
    theGlobalVariables.theIndicatorVariables.StatusString1=tempRoots.ElementToString();
    theGlobalVariables.MakeReport();
    currentProjectiveCone.CreateFromNormals(tempRoots, theGlobalVariables);
    projectivizedChamberFinal.AddNonRefinedChamberOnTopNoRepetition(currentProjectiveCone, theGlobalVariables);
  }
  for (int i=0; i<this->PreimageWeylChamberSmallerAlgebra.Normals.size; i++)
    projectivizedChamberFinal.splittingNormals.AddOnTop(this->PreimageWeylChamberSmallerAlgebra.Normals[i]);
  out << "projectivized chamber before chopping non-dominant part:\n" << projectivizedChamberFinal.ElementToString(false, false);
  projectivizedChamberFinal.Refine(theGlobalVariables);
  out << "Refined projectivized chamber before chopping non-dominant part:\n" << projectivizedChamberFinal.ElementToString(false, false);
  for (int i=0; i<projectivizedChamberFinal.size; i++)
  { Cone& currentCone=projectivizedChamberFinal[i];
    bool isNonDominant=false;
    for (int j=0; j< this->PreimageWeylChamberSmallerAlgebra.Normals.size; j++)
      if (currentCone.GetInternalPoint().ScalarEuclidean(this->PreimageWeylChamberSmallerAlgebra.Normals[j]).IsNegative())
      { isNonDominant=true;
        break;
      }
    if (isNonDominant)
    { projectivizedChamberFinal.PopChamberSwapWithLast(i);
      i--;
    }
  }
  theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
  theGlobalVariables.MakeReport();

  projectivizedChamberFinal.indexLowestNonRefinedChamber=0;
  this->projectivizedChambeR=projectivizedChamberFinal;
  for (int k=1; k<this->theLinearOperators.size; k++)
    for (int i=0; i<this->smallerAlgebraChamber.size; i++)
      for (int j=0; j<this->smallerAlgebraChamber[i].Normals.size; j++)
      { this->TransformToWeylProjective(k, this->smallerAlgebraChamber[i].Normals[j], wallToSliceWith);
        wallToSliceWith.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
        this->projectivizedChambeR.splittingNormals.AddNoRepetition(wallToSliceWith);
      }
  out << "projectivized chamber chopped non-dominant part:\n"  << this->projectivizedChambeR.ElementToString(false, false);
  theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
}

void HomomorphismSemisimpleLieAlgebra::ApplyHomomorphism
  (ElementSemisimpleLieAlgebra& input, ElementSemisimpleLieAlgebra& output)
{ assert(&output!=&input);
  output.MakeZero(*this->owners, this->indexRange);
  for (int i=0; i<input.size; i++)
  { int currentIndex=input[i].theGeneratorIndex;
    output+=this->imagesAllChevalleyGenerators[currentIndex]*input.theCoeffs[i];
  }
}

void HomomorphismSemisimpleLieAlgebra::GetMapSmallCartanDualToLargeCartanDual(Matrix<Rational> & output)
{ output.init(this->theRange().GetRank(), this->theDomain().GetRank());
  ElementSemisimpleLieAlgebra domainElt, imageElt;
  for (int i=0; i<this->theDomain().GetRank(); i++)
  { domainElt.AssignElementCartan
    (Vector<Rational>::GetEi(this->theDomain().GetRank(), i), *this->owners, this->indexDomain);
    this->ApplyHomomorphism(domainElt, imageElt);
    output.AssignVectorToColumnKeepOtherColsIntactNoInit(i, imageElt.GetCartanPart());
  }
}

void ElementSemisimpleLieAlgebra::AssignElementCartan
(const Vector<Rational>& theH, List<SemisimpleLieAlgebra>& inputOwners, int inputIndexInOwners)
{ ChevalleyGenerator tempGen;
  this->MakeZero(inputOwners, inputIndexInOwners);
  for (int i=0; i<theH.size; i++)
  { tempGen.MakeGenerator(inputOwners, inputIndexInOwners, inputOwners[inputIndexInOwners].CartanIndexToChevalleyGeneratorIndex(i));
    this->AddMonomial(tempGen, theH[i]);
  }
}

Vector<Rational> ReflectionSubgroupWeylGroup::GetRho()
{ Vector<Rational> result;
  this->RootsOfBorel.sum(result, this->AmbientWeyl.GetDim());
  result/=2;
  return result;
}

void ReflectionSubgroupWeylGroup::GetMatrixOfElement
(ElementWeylGroup& input, Matrix<Rational> & outputMatrix)
{ Vectors<Rational> startBasis, imageBasis ;
  startBasis.MakeEiBasis(this->AmbientWeyl.GetDim());
  this->ActByElement(input, startBasis, imageBasis);
  outputMatrix.AssignRootsToRowsOfMatrix(imageBasis);
  outputMatrix.Transpose();
}

void GeneralizedVermaModuleCharacters::initFromHomomorphism
  (Vector<Rational>& theParabolicSel, HomomorphismSemisimpleLieAlgebra& input, GlobalVariables& theGlobalVariables)
{ Vectors<Rational> tempRoots;
  this->WeylLarger=input.theRange().theWeyl;
  this->WeylSmaller=input.theDomain().theWeyl;
  WeylGroup& theWeYl=input.theRange().theWeyl;
//  input.ProjectOntoSmallCartan(theWeyl.RootsOfBorel, tempRoots, theGlobalVariables);
  this->log << "projections: " << tempRoots.ElementToString();
  theWeYl.ComputeWeylGroup();
  this->NonIntegralOriginModificationBasisChanged="(1/2,1/2)";
  Matrix<Rational>  theProjectionBasisChanged;
  Vector<Rational> startingWeight, projectedWeight;
  FormatExpressions theFormat;
  assert(false);
//  SSalgebraModuleOld tempM;
  std::stringstream tempStream;
  input.ComputeHomomorphismFromImagesSimpleChevalleyGenerators(theGlobalVariables);
  assert(false);
//  tempM.InduceFromEmbedding(tempStream, input, theGlobalVariables);
  input.GetWeightsGmodKInSimpleCoordsK(this->GmodKnegativeWeightS, theGlobalVariables);
//  this->log << "weights of g mod k: " << this->GmodKnegativeWeights.ElementToString();
  Matrix<Rational>  tempMat;
  tempMat=input.theDomain().theWeyl.CartanSymmetric;
  tempMat.Invert(theGlobalVariables);
//  tempMat.ActOnVectorsColumn(this->GmodKnegativeWeightS);
  this->log << this->GmodKnegativeWeightS.ElementToString();
  this->preferredBasiS.SetSize(2);
  this->preferredBasiS[0]=-this->GmodKnegativeWeightS[1];
  this->preferredBasiS[1]=-this->GmodKnegativeWeightS[2];
  /////////////////////////////////////////
  //this->preferredBasiS[0]="(1,0)";
  //this->preferredBasiS[1]="(0,1)";
  ////////////////////////////////////////
  this->preferredBasisChangE.AssignRootsToRowsOfMatrix(this->preferredBasiS);
  this->preferredBasisChangE.Transpose();
  this->preferredBasisChangeInversE=this->preferredBasisChangE;
  this->preferredBasisChangeInversE.Invert();
  this->preferredBasisChangeInversE.ActOnVectorsColumn
  (this->GmodKnegativeWeightS, this->GmodKNegWeightsBasisChanged);
  this->log << "\nWeights after basis change: " << this->GmodKNegWeightsBasisChanged.ElementToString();
  for (int i=0; i<this->GmodKnegativeWeightS.size; i++)
    if (this->GmodKnegativeWeightS[i].IsPositiveOrZero())
    { this->GmodKnegativeWeightS.PopIndexSwapWithLast(i);
      i--;
    }
  for (int i=0; i<this->GmodKNegWeightsBasisChanged.size; i++)
    if (this->GmodKNegWeightsBasisChanged[i].IsPositiveOrZero())
    { this->GmodKNegWeightsBasisChanged.PopIndexSwapWithLast(i);
      i--;
    }
  this->log << "\nNegative weights after basis change: " << this->GmodKNegWeightsBasisChanged.ElementToString();
  theProjectionBasisChanged.init(input.theDomain().GetRank(), input.theRange().GetRank());
  for (int i=0; i<input.theRange().GetRank(); i++)
  { startingWeight.MakeEi(input.theRange().GetRank(), i);
    input.ProjectOntoSmallCartan(startingWeight, projectedWeight, theGlobalVariables);
    this->preferredBasisChangeInversE.ActOnVectorColumn(projectedWeight);
    for (int j=0; j<projectedWeight.size; j++)
      theProjectionBasisChanged.elements[j][i]=projectedWeight[j];
  }
  ReflectionSubgroupWeylGroup theSubgroup;
  this->ParabolicLeviPartRootSpacesZeroStandsForSelected=theParabolicSel;
  Matrix<Rational>  DualCartanEmbedding;
  input.GetMapSmallCartanDualToLargeCartanDual(DualCartanEmbedding);
  Vector<Rational> ParabolicEvaluationRootImage, tempRoot;
  ParabolicEvaluationRootImage=this->ParabolicLeviPartRootSpacesZeroStandsForSelected;
  this->ParabolicSelectionSmallerAlgebra.init(input.theDomain().GetRank());
  for (int i=0; i<input.theDomain().GetRank(); i++)
  { DualCartanEmbedding.ColToRoot(i, tempRoot);
    if (ParabolicEvaluationRootImage.ScalarEuclidean(tempRoot).IsPositive())
      this->ParabolicSelectionSmallerAlgebra.AddSelectionAppendNewIndex(i);
  }
  this->log << "\nDual cartan embedding smaller into larger:\n" << DualCartanEmbedding.ElementToString(false, false);
  this->log << "\nParabolic subalgebra large algebra: " << this->ParabolicLeviPartRootSpacesZeroStandsForSelected.ElementToString();
  tempRoot= this->ParabolicSelectionSmallerAlgebra;
  this->log << "\nParabolic subalgebra smaller algebra: " << tempRoot.ElementToString();
  theSubgroup.MakeParabolicFromSelectionSimpleRoots
  (theWeYl, this->ParabolicLeviPartRootSpacesZeroStandsForSelected, theGlobalVariables, -1);

  this->theLinearOperators.SetSize(theSubgroup.size);
  this->theLinearOperatorsExtended.SetSize(theSubgroup.size);
  this->theTranslationS.SetSize(theSubgroup.size);
  this->theTranslationsProjectedBasisChanged.SetSize(theSubgroup.size);
  this->theCoeffs.SetSize(theSubgroup.size);
  this->log << " \n******************\nthe subgroup: \n" << theSubgroup.ElementToString() << "\n\n\n\n\n\n";
  this->log << theSubgroup.ElementToStringBruhatGraph();
  this->log << "\nMatrix form of the elements of Weyl group of the Levi part of the parabolic (" << theSubgroup.size << " elements):\n";

  for (int i=0; i<theSubgroup.size; i++)
  { Matrix<Rational> & currentLinearOperator=this->theLinearOperators[i];
    theSubgroup.GetMatrixOfElement(theSubgroup[i], currentLinearOperator);
//    currentLinearOperator.MultiplyOnTheLeft(preferredBasisChangeInverse);
    this->log << "\n" << currentLinearOperator.ElementToString(false, false);
    currentLinearOperator.ActOnVectorColumn(theSubgroup.GetRho(), this->theTranslationS[i]);
    this->theTranslationS[i]-=theSubgroup.GetRho();
    this->theTranslationS[i].Minus();
    theProjectionBasisChanged.ActOnVectorColumn(this->theTranslationS[i], this->theTranslationsProjectedBasisChanged[i]);
    if (theSubgroup[i].size%2==0)
      this->theCoeffs[i]=1;
    else
      this->theCoeffs[i]=-1;
  }
  this->log << "\n\n\nMatrix of the projection operator (basis-changed):\n" << theProjectionBasisChanged.ElementToString(false, false);
  this->log << "\n\n\nMatrix form of the operators $u_w$, the translations $\tau_w$ and their projections (" << this->theLinearOperatorsExtended.size << "):";
  //List<Matrix<Rational> > tempList;
  for (int k=0; k<this->theLinearOperators.size; k++)
  { Matrix<Rational> & currentLO=this->theLinearOperators.TheObjects[k];
    Matrix<Rational> & currentLOExtended=this->theLinearOperatorsExtended.TheObjects[k];
    currentLO.MultiplyOnTheLeft(theProjectionBasisChanged);
    currentLO*=-1;
    //tempList.AddOnTopNoRepetition(this->theLinearOperators.TheObjects[i]);
    currentLOExtended.MakeIdMatrix(currentLO.NumRows);
    currentLOExtended.Resize(currentLO.NumRows, currentLO.NumRows+currentLO.NumCols, true);
    for (int i=0; i<currentLO.NumRows; i++)
      for (int j=0; j<currentLO.NumCols; j++)
        currentLOExtended.elements[i][j+currentLO.NumRows]=currentLO.elements[i][j];
    this->log << "\n\n" << currentLOExtended.ElementToString(false, false);
    this->log << this->theTranslationS[k].ElementToString() << ";   " << this->theTranslationsProjectedBasisChanged[k].ElementToString();
  }

  List<int> displayIndicesReflections;
  for (int i=0; i<this->ParabolicLeviPartRootSpacesZeroStandsForSelected.MaxSize; i++)
    if (!this->ParabolicLeviPartRootSpacesZeroStandsForSelected.selected[i])
      displayIndicesReflections.AddOnTop(i+1);
  Matrix<Polynomial<Rational> > tempMatPoly;
  Vector<Polynomial<Rational> > tempVect, tempVect2;
  tempVect.SetSize(input.theDomain().theWeyl.GetDim()+input.theRange().theWeyl.GetDim());
  for (int i=0; i<tempVect.size; i++)
    tempVect[i].MakeMonomial(tempVect.size, i, 1, (Rational) 1);
  tempMatPoly.init(input.theDomain().theWeyl.GetDim(), tempVect.size);
  Polynomial<Rational>  polyZero;
  polyZero.MakeZero(tempVect.size);
  theFormat.polyAlphabeT.SetSize(5);
  theFormat.polyAlphabeT[0]="x_1";
  theFormat.polyAlphabeT[1]="x_2";
  theFormat.polyAlphabeT[2]="y_1";
  theFormat.polyAlphabeT[3]="y_2";
  theFormat.polyAlphabeT[4]="y_3";
  tempRoot=theSubgroup.GetRho();
  this->theLinearOperators[0].ActOnVectorColumn(tempRoot);
  this->preferredBasisChangE.ActOnVectorColumn(tempRoot);
  tempRoot.Minus();
  this->log << "\n\nIn $so(7)$-simple basis coordinates, $\\rho_{\\mathfrak l}=" <<
  theSubgroup.GetRho().ElementToStringLetterFormat("\\eta", true)
  << "$; $\\pr(\\rho)=" << tempRoot.ElementToStringLetterFormat("\\alpha", true) << "$."
  ;
  this->log << "\n\n\\begin{longtable}{r|l}$w$ & \\begin{tabular}{c}Argument of the vector partition function in (\\ref{eqMultG2inB3General}) =\\\\ $u_w\\circ"
  << tempVect.ElementToString(&theFormat) << "-\\tau_w$ \\end{tabular}  \\\\ \\hline \\endhead";
  for (int i=0; i<this->theLinearOperatorsExtended.size; i++)
  { Matrix<Rational> & currentLoExt=this->theLinearOperatorsExtended[i];
    for (int j=0; j<currentLoExt.NumRows; j++)
      for (int k=0; k<currentLoExt.NumCols; k++)
        tempMatPoly.elements[j][k].MakeConst(tempVect.size, currentLoExt.elements[j][k]);
    tempMatPoly.ActOnVectorColumn(tempVect, tempVect2, polyZero);
    for (int j=0; j<tempVect2.size; j++)
      tempVect2[j]+=this->theTranslationsProjectedBasisChanged[i][j];
    this->log << "\n$" <<  theSubgroup[i].ElementToString(true, false, "\\eta", & displayIndicesReflections) << "$&$"
    << tempVect2.ElementToString(&theFormat) << "$\\\\";
  }
  this->log <<"\\end{longtable}\n\n";
//  this->log << "\n\n\nThere are " << tempList.size << " different operators.";
  Lattice tempLattice;
  theWeYl.GetIntegralLatticeInSimpleCoordinates(tempLattice);
  this->theExtendedIntegralLatticeMatForM.basisRationalForm.MakeIdMatrix(input.theDomain().GetRank());
  this->theExtendedIntegralLatticeMatForM.basisRationalForm.DirectSumWith(tempLattice.basisRationalForm, (Rational) 0);
  this->theExtendedIntegralLatticeMatForM.MakeFromMat(this->theExtendedIntegralLatticeMatForM.basisRationalForm);
  tempMat=theWeYl.CartanSymmetric;
  tempMat.Invert(theGlobalVariables);
  Vectors<Rational> WallsWeylChamberLargerAlgebra;
  for (int i=0; i<tempMat.NumRows; i++)
  { tempMat.RowToRoot(i, tempRoot);
    if(ParabolicEvaluationRootImage[i].IsEqualToZero())
    { WallsWeylChamberLargerAlgebra.SetSize(WallsWeylChamberLargerAlgebra.size+1);
      *WallsWeylChamberLargerAlgebra.LastObject()=tempRoot;
    }
  }
  this->log << "\n\n\n**************\nParabolic selection larger algebra:" << ParabolicEvaluationRootImage.ElementToString() << "\nWalls Weyl chamber larger algebra: " << WallsWeylChamberLargerAlgebra.ElementToString();
  this->log << "\n**************\n\n";
  Vectors<Rational> rootsGeneratingExtendedLattice;
  int totalDim=input.theRange().GetRank()+input.theDomain().GetRank();
  rootsGeneratingExtendedLattice.SetSize(totalDim);
  this->log << "\n" << tempMat.ElementToString(false, false) << "\n";
  this->log << this->theExtendedIntegralLatticeMatForM.ElementToString(false, false);
  this->WeylChamberSmallerAlgebra.CreateFromNormals(WallsWeylChamberLargerAlgebra, theGlobalVariables);
  this->log << "\nWeyl chamber larger algebra before projectivizing: " << this->WeylChamberSmallerAlgebra.ElementToString(&theFormat) << "\n";
  this->PreimageWeylChamberSmallerAlgebra.Normals=this->WeylChamberSmallerAlgebra.Normals;
  for (int i=0; i<this->PreimageWeylChamberLargerAlgebra.Normals.size; i++)
  { tempRoot.MakeZero(input.theRange().GetRank()+input.theDomain().GetRank()+1);
    for (int j=0; j<input.theRange().GetRank(); j++)
      tempRoot.TheObjects[j+input.theDomain().GetRank()]=this->PreimageWeylChamberLargerAlgebra.Normals[i][j];
    this->PreimageWeylChamberLargerAlgebra.Normals[i]=tempRoot;
  }
  tempMat=input.theDomain().theWeyl.CartanSymmetric;
  tempMat.Invert(theGlobalVariables);
  tempRoots.size=0;
  Vector<Rational> ParabolicEvaluationRootSmallerAlgebra;
  ParabolicEvaluationRootSmallerAlgebra=this->ParabolicSelectionSmallerAlgebra;
  for (int i=0; i<tempMat.NumRows; i++)
  { input.theDomain().theWeyl.CartanSymmetric.RowToRoot(i, tempRoot);
    if (tempRoot.ScalarEuclidean(ParabolicEvaluationRootSmallerAlgebra).IsEqualToZero())
    { tempRoots.SetSize(tempRoots.size+1);
      tempMat.RowToRoot(i, *tempRoots.LastObject());
    }
  }
  this->preferredBasisChangeInversE.ActOnVectorsColumn(tempRoots);
  this->log << "**********************\n\n\n";
  this->log << "\nthe smaller parabolic selection: " << this->ParabolicSelectionSmallerAlgebra.ElementToString();
  this->log << "the Vectors<Rational> generating the chamber walls: " << tempRoots.ElementToString();
  this->PreimageWeylChamberSmallerAlgebra.CreateFromVertices(tempRoots, theGlobalVariables);
  this->log << "\nWeyl chamber smaller algebra: " << this->PreimageWeylChamberSmallerAlgebra.ElementToString(&theFormat) << "\n";
  this->log << "**********************\n\n\n";
  this->log << "\nThe first operator extended:\n" << this->theLinearOperatorsExtended.TheObjects[0].ElementToString(false, false) << "\n";
  this->log << "\nThe second operator extended:\n" << this->theLinearOperatorsExtended.TheObjects[1].ElementToString(false, false) << "\n";
  /*tempMat=this->theLinearOperatorsExtended.TheObjects[0];
  tempMat.Transpose();
  tempMat.ActOnVectorsColumn(this->PreimageWeylChamberSmallerAlgebra);
  for (int i=0; i<this->PreimageWeylChamberSmallerAlgebra.size; i++)
  { this->PreimageWeylChamberSmallerAlgebra.TheObjects[i].SetSize(input.theRange.GetRank()+input.theDomain.GetRank()+1);
    *this->PreimageWeylChamberSmallerAlgebra.TheObjects[i].LastObject()=0;
  }*/
  for (int i=0; i<this->PreimageWeylChamberSmallerAlgebra.Normals.size; i++)
  { tempRoot.MakeZero(input.theRange().GetRank()+input.theDomain().GetRank()+1);
    for (int j=0; j<input.theDomain().GetRank(); j++)
      tempRoot.TheObjects[j]=this->PreimageWeylChamberSmallerAlgebra.Normals[i][j];
  //  for (int j=0; j<input.theRange.GetRank(); j++)
   //   tempRoot.TheObjects[j+input.theDomain.GetRank()]=tempRoot2.TheObjects[j];
    this->PreimageWeylChamberSmallerAlgebra.Normals[i]=tempRoot;
  }

  tempRoot.MakeEi(input.theRange().GetRank()+input.theDomain().GetRank()+1, input.theRange().GetRank()+input.theDomain().GetRank());
  this->PreimageWeylChamberLargerAlgebra.Normals.AddOnTop(tempRoot);
  this->log << "\nPreimage Weyl chamber smaller algebra: " << this->PreimageWeylChamberSmallerAlgebra.ElementToString(&theFormat) << "\n";
  this->log << "\nPreimage Weyl chamber larger algebra: " << this->PreimageWeylChamberLargerAlgebra.ElementToString(&theFormat) << "\n";

  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.theIndicatorVariables.StatusString1=this->log.str();
  theGlobalVariables.MakeReport();
}

void WeylGroup::GetMatrixOfElement(int theIndex, Matrix<Rational> & outputMatrix)
{ assert(theIndex<this->size);
  this->GetMatrixOfElement(this->TheObjects[theIndex], outputMatrix);
}

void ReflectionSubgroupWeylGroup::MakeParabolicFromSelectionSimpleRoots
(WeylGroup& inputWeyl, const Selection& ZeroesMeanSimpleRootSpaceIsInParabolic, GlobalVariables& theGlobalVariables, int UpperLimitNumElements)
{ Vectors<Rational> selectedRoots;
  selectedRoots.Reserve(ZeroesMeanSimpleRootSpaceIsInParabolic.MaxSize- ZeroesMeanSimpleRootSpaceIsInParabolic.CardinalitySelection);
  this->AmbientWeyl=inputWeyl;
  for (int i=0; i<ZeroesMeanSimpleRootSpaceIsInParabolic.MaxSize; i++)
    if (!ZeroesMeanSimpleRootSpaceIsInParabolic.selected[i])
    { selectedRoots.SetSize(selectedRoots.size+1);
      selectedRoots.LastObject()->MakeEi(inputWeyl.GetDim(), i);
    }
  List<Vectors<Rational> >  tempRootsCol;
  this->ComputeSubGroupFromGeneratingReflections(selectedRoots, tempRootsCol, theGlobalVariables, UpperLimitNumElements, true);
}

void WeylGroup::GetMatrixOfElement(ElementWeylGroup& input, Matrix<Rational> & outputMatrix)
{ Vector<Rational> tempRoot;
  int theDim=this->CartanSymmetric.NumRows;
  outputMatrix.init(theDim, theDim);
  for (int i=0; i<theDim; i++)
  { tempRoot.MakeEi(theDim, i);
    this->ActOn(input, tempRoot, false, false, (Rational) 0);
    for (int j=0; j<theDim; j++)
      outputMatrix.elements[j][i]=tempRoot.TheObjects[j];
  }
}

GeneralizedVermaModuleCharacters tempCharsEraseWillBeErasedShouldntHaveLocalObjectsLikeThis;

void GeneralizedVermaModuleCharacters::GetProjection(int indexOperator, const Vector<Rational>& input, Vector<Rational>& output)
{ Matrix<Rational> & currentExtendedOperator=this->theLinearOperatorsExtended[indexOperator];
  Vector<Rational>& currentTranslation=this->theTranslationsProjectedBasisChanged[indexOperator];
  assert (!input.LastObject()->IsEqualToZero());
  output=input;
  Rational tempRat=*output.LastObject();
  output/=tempRat;
  output.size--;
  currentExtendedOperator.ActOnVectorColumn(output);
  output+=currentTranslation;
}

void GeneralizedVermaModuleCharacters::GetSubFromIndex
(PolynomialSubstitution<Rational>& outputSub, Matrix<LargeInt>& outputMat, LargeIntUnsigned& outputDen, int theIndex)
{ Matrix<Rational> & theOperator=this->theLinearOperators.TheObjects[theIndex];
  int dimLargerAlgebra=theOperator.NumCols;
  int dimSmallerAlgebra=theOperator.NumRows;
  Vector<Rational>& theTranslation=this->theTranslationS[theIndex];
  Matrix<Rational> tempMat;
  tempMat.init(dimLargerAlgebra+dimSmallerAlgebra+1, dimSmallerAlgebra);
  tempMat.NullifyAll();
  for (int j=0; j<dimSmallerAlgebra; j++)
  { tempMat.elements[j][j]=1;
    for (int i=0; i<dimLargerAlgebra; i++)
      tempMat.elements[i][j]-=theOperator.elements[j][i];
    tempMat.elements[dimLargerAlgebra+dimSmallerAlgebra][j]=-theTranslation.TheObjects[j];
  }
  tempMat.GetMatrixIntWithDen(outputMat, outputDen);
  outputSub.MakeSubFromMatrixIntAndDen(outputMat, outputDen);
}

void Selection::operator=(const Vector<Rational>& other)
{ this->init(other.size);
  for (int i=0; i<other.size; i++)
    if (!other[i].IsEqualToZero())
      this->selected[i]=true;
  this->ComputeIndicesFromSelection();
}

void GeneralizedVermaModuleCharacters::ComputeQPsFromChamberComplex
(GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  FormatExpressions theFormat;
  Vector<Rational> tempRoot;
  CGI::OpenFileCreateIfNotPresent(this->theMultiplicitiesMaxOutputReport2, "/home/todor/math/vectorpartition/trunk/ExtremaPolys.txt", false, true, false);
  this->thePfs.initFromRoots(this->GmodKNegWeightsBasisChanged, theGlobalVariables);
  this->thePfs.ComputeDebugString(theGlobalVariables);
  out << this->thePfs.DebugString;
  this->thePfs.split(theGlobalVariables, 0);
  this->thePfs.ComputeDebugString(theGlobalVariables);
  out << "=" << this->thePfs.DebugString;
//  int totalDim=this->theTranslationS[0].size+this->theTranslationsProjecteD[0].size;
  this->theQPsSubstituted.SetSize(this->projectivizedChambeR.size);
  assert(false);
//  this->thePfs.theChambersOld.init();
//  this->thePfs.theChambersOld.theDirections=this->GmodKNegWeightsBasisChanged;
//  this->thePfs.theChambersOld.SliceTheEuclideanSpace(theGlobalVariables, false);
//  this->theQPsNonSubstituted.SetSize(this->thePfs.theChambersOld.size);
//  this->theQPsSubstituted.SetSize(this->thePfs.theChambersOld.size);
  out << "\n\nThe vector partition functions in each chamber follow.";
  theGlobalVariables.theIndicatorVariables.ProgressReportStringsNeedRefresh=true;
  assert(false);
/*
  for (int i=0; i<this->thePfs.theChambersOld.size; i++)
    if (this->thePfs.theChambersOld.TheObjects[i]!=0)
    { QuasiPolynomial& currentQPNoSub= this->theQPsNonSubstituted.TheObjects[i];
      this->theQPsSubstituted.TheObjects[i].SetSize(this->theLinearOperators.size);
      this->thePfs.GetVectorPartitionFunction(currentQPNoSub, this->thePfs.theChambersOld.TheObjects[i]->InternalPoint, theGlobalVariables);
      out << "\nChamber " << i+1 << " with internal point " << this->thePfs.theChambersOld.TheObjects[i]->InternalPoint.ElementToString() << " the quasipoly is: " << currentQPNoSub.ElementToString(false, false);
      for (int k=0; k<this->theLinearOperators.size; k++)
      { QuasiPolynomial& currentQPSub=this->theQPsSubstituted.TheObjects[i].TheObjects[k];
        std::stringstream tempStream;
        tempStream << "Processing chamber " << i+1 << " linear operator " << k+1;
        theGlobalVariables.theIndicatorVariables.ProgressReportStrings[0]= tempStream.str();
        theGlobalVariables.MakeReport();
        currentQPNoSub.Substitution(this->theLinearOperatorsExtended.TheObjects[k], this->theTranslationsProjectedBasisChanged[k], this->theExtendedIntegralLatticeMatForM, currentQPSub, theGlobalVariables);
        out << "; after substitution we get: " << currentQPSub.ElementToString(false, false);
      }
    }
    */
//  this->theParser.theHmm.theRange.theWeyl.GetIntegralLatticeInSimpleCoordinates(integralLattice);
  //out << "\nThe integral lattice:\n" << integralLattice.ElementToString(false, false);
  //this->theMultiplicitiesMaxOutputReport2.flush();
  QuasiPolynomial tempQP;
  this->theMultiplicities.SetSize(this->projectivizedChambeR.size);
  this->numNonZeroMults=0;
  for (int i=0; i<this->projectivizedChambeR.size; i++)
  { QuasiPolynomial& currentSum=this->theMultiplicities.TheObjects[i];
    currentSum.MakeZeroOverLattice(this->theExtendedIntegralLatticeMatForM);
    for (int k=0; k<this->theLinearOperators.size; k++)
    { this->GetProjection(k, this->projectivizedChambeR.TheObjects[i].GetInternalPoint(), tempRoot);
      tempRoot-=this->NonIntegralOriginModificationBasisChanged;
      assert(false);
      int theIndex;//= this->thePfs.theChambersOld.GetFirstChamberIndexContainingPoint(tempRoot);
      if (theIndex!=-1)
      { tempQP=this->theQPsSubstituted[theIndex][k];
        tempQP*=this->theCoeffs[k];
        currentSum+=tempQP;
      }
      std::stringstream tempStream;
      tempStream << " Chamber " << i+1 << " translation " << k+1;
      theGlobalVariables.theIndicatorVariables.ProgressReportStrings[0]=tempStream.str();
      theGlobalVariables.MakeReport();
    }
    if (!currentSum.IsEqualToZero())
      this->numNonZeroMults++;
    std::stringstream tempStream;
    tempStream << " So far " << i+1 << " out of " << this->projectivizedChambeR.size << " processed " << this->numNonZeroMults
    << " non-zero total.";
    theGlobalVariables.theIndicatorVariables.ProgressReportStrings[1]=tempStream.str();
    theGlobalVariables.MakeReport();
    out << "\nChamber " << i+1 << ": the quasipolynomial is: " << currentSum.ElementToString(false, false);
    out << "\nThe chamber is: " << this->projectivizedChambeR.TheObjects[i].ElementToString(&theFormat);
  }
//  this->projectivizedChamber.ComputeDebugString();
//  out << "\n\n" << this->projectivizedChamber.DebugString;
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
  theGlobalVariables.MakeReport();
  this->theMultiplicitiesMaxOutputReport2 << out.str();
}

int ParserNode::EvaluateCreateFromDirectionsAndSalamiSlice
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ Vectors<Rational> tempRoots;
  int theDim;
  if(!theNode.GetRootsEqualDimNoConversionNoEmptyArgument(theArgumentList, tempRoots, theDim))
    return theNode.SetError(theNode.errorDimensionProblem);
  ConeComplex tempComplex;
  tempComplex.InitFromDirectionsAndRefine(tempRoots, theGlobalVariables);
  FormatExpressions tempFormat;
  theNode.outputString=tempComplex.DrawMeToHtmlProjective(theGlobalVariables.theDrawingVariables, tempFormat);
  theNode.ExpressionType=theNode.typeString;
  return theNode.errorNoError;
}

int ParserNode::EvaluateG2InB3Computation
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ Vector<Rational> parSel;
  theNode.GetRootRationalFromFunctionArguments(parSel, theGlobalVariables);
  if (parSel.size!=3)
    return theNode.SetError(theNode.errorDimensionProblem);
  tempCharsEraseWillBeErasedShouldntHaveLocalObjectsLikeThis.IncrementComputation
    (parSel, theGlobalVariables)
    ;
  return theNode.errorNoError;
}

void GeneralizedVermaModuleCharacters::ReadFromDefaultFile(GlobalVariables* theGlobalVariables)
{ std::fstream input;
  if (!CGI::FileExists( "/home/todor/math/vectorpartition/trunk/GenVermaComputation.txt"))
  { this->computationPhase=0;
    return;
  }
  CGI::OpenFileCreateIfNotPresent(input, "/home/todor/math/vectorpartition/trunk/GenVermaComputation.txt", false, false, false);
  this->ReadFromFile(input, theGlobalVariables);
  input.close();
}

void GeneralizedVermaModuleCharacters::WriteToDefaultFile(GlobalVariables* theGlobalVariables)
{ std::fstream output;
  CGI::OpenFileCreateIfNotPresent(output, "/home/todor/math/vectorpartition/trunk/GenVermaComputation.txt", false, true, false);
  this->WriteToFile(output, theGlobalVariables);
}

std::string GeneralizedVermaModuleCharacters::ElementToStringMultiplicitiesReport
(GlobalVariables& theGlobalVariables)
{ assert(this->theMultiplicities.size== this->projectivizedChambeR.size);
  std::stringstream out;
  FormatExpressions theFormat;
  theFormat.polyAlphabeT.SetSize(5);
  theFormat.polyAlphabeT[0]="x_1";
  theFormat.polyAlphabeT[1]="x_2";
  theFormat.polyAlphabeT[2]="y_1";
  theFormat.polyAlphabeT[3]="y_2";
  theFormat.polyAlphabeT[4]="y_3";
  out << "Number chambers: " << projectivizedChambeR.size << " of them " << this->numNonZeroMults << " non-zero.";
  int numInequalities=0;
  for (int i=0; i<this->projectivizedChambeR.size; i++)
  { numInequalities+=this->projectivizedChambeR[i].Normals.size;
  }
  out << "\nNumber of inequalities: " << numInequalities;
  if (!this->ParabolicLeviPartRootSpacesZeroStandsForSelected.CardinalitySelection==0)
    out << this->PrepareReport(theGlobalVariables);
  return out.str();
}

int ParserNode::EvaluateG2InB3MultsParabolic
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ Vector<Rational> highestWeight, parSel;
  ParserNode& nodeHW=theNode.owner->TheObjects[theArgumentList[0]];
  ParserNode& nodeSel=theNode.owner->TheObjects[theArgumentList[1]];
  if(!nodeHW.GetRootRationalDontUseForFunctionArguments(highestWeight, theGlobalVariables))
    return theNode.SetError(theNode.errorProgramming);
  if (!nodeSel.GetRootRationalDontUseForFunctionArguments(parSel, theGlobalVariables))
    return theNode.SetError(theNode.errorProgramming);
  if (highestWeight.size!=3 || parSel.size!=3)
    return theNode.SetError(theNode.errorDimensionProblem);
  theNode.owner->initTestAlgebraNeedsToBeRewrittenG2InB3(theGlobalVariables);
  theNode.outputString=tempCharsEraseWillBeErasedShouldntHaveLocalObjectsLikeThis
  .ComputeMultsLargerAlgebraHighestWeight(highestWeight, parSel, *theNode.owner, theGlobalVariables);
  theNode.ExpressionType=theNode.typeString;
  return theNode.errorNoError;
}

void ConeComplex::InitFromAffineDirectionsAndRefine
  (Vectors<Rational>& inputDirections, Vectors<Rational>& inputAffinePoints, GlobalVariables& theGlobalVariables)
{ assert(inputDirections.size==inputAffinePoints.size);
  assert(inputDirections.size>0);
  Vectors<Rational> projectivizedDirections;
  projectivizedDirections.SetSize(inputDirections.size*2);
  int theAffineDim= inputDirections[0].size;
  for (int i=0; i<inputDirections.size; i++)
  { projectivizedDirections[i]=inputDirections[i];
    projectivizedDirections[i].SetSize(theAffineDim+1);
    *projectivizedDirections[i].LastObject()=0;
  }
  for (int i=0; i<inputAffinePoints.size; i++)
  { projectivizedDirections[i+inputAffinePoints.size]=inputAffinePoints[i];
    projectivizedDirections[i+inputAffinePoints.size].SetSize(theAffineDim+1);
    *projectivizedDirections[i+inputAffinePoints.size].LastObject()=1;
  }
  this->InitFromDirectionsAndRefine(projectivizedDirections, theGlobalVariables);
}

void ConeComplex::MakeAffineAndTransformToProjectiveDimPlusOne
  (Vector<Rational>& affinePoint, ConeComplex& output, GlobalVariables& theGlobalVariables)
{ assert(&output!=this);
  output.init();
  Cone tempCone;
  Vectors<Rational> newNormals;
  Vector<Rational> tempRoot;
  int theAffineDim=affinePoint.size;
  for (int i=0; i<this->size; i++)
  { newNormals.SetSize(this->TheObjects[i].Normals.size+1);
    for (int j=0; j<this->TheObjects[i].Normals.size; j++)
      newNormals[j]= this->TheObjects[i].Normals[j].GetProjectivizedNormal(affinePoint);
    newNormals.LastObject()->MakeEi(theAffineDim+1, theAffineDim);
    tempCone.CreateFromNormals(newNormals, theGlobalVariables);
    output.AddNonRefinedChamberOnTopNoRepetition(tempCone, theGlobalVariables);
  }
}

template<class CoefficientType>
Vector<CoefficientType> Vector<CoefficientType>::GetProjectivizedNormal(Vector<CoefficientType>& affinePoint)
{ Vector<CoefficientType> result=*this;
  result.SetSize(this->size+1);
  *result.LastObject()=-affinePoint.ScalarEuclidean(*this);
  return result;
}

void Lattice::GetRootOnLatticeSmallestPositiveProportionalTo
(Vector<Rational>& input, Vector<Rational>& output, GlobalVariables& theGlobalVariables)
{ assert(&input!=&output);
  Vectors<Rational> theBasis;
  Vector<Rational> tempRoot;
  Vectors<Rational> bufferVect;
  Matrix<Rational> bufferMat;
  theBasis.AssignMatrixRows(this->basisRationalForm);
  input.GetCoordsInBasiS(theBasis, tempRoot, bufferVect, bufferMat);
  tempRoot.ScaleByPositiveRationalToIntegralMinHeight();
  Matrix<Rational>  tempMat;
  tempMat=this->basisRationalForm;
  tempMat.Transpose();
  tempMat.ActOnVectorColumn(tempRoot, output);
/*
  Vectors<Rational> tempRoots;
  tempRoots.AddOnTop(input);
  Lattice tempLattice=*this;
  tempLattice.IntersectWithLinearSubspaceSpannedBy(tempRoots);
  assert(tempLattice.basisRationalForm.NumRows==1);
  tempLattice.basisRationalForm.RowToRoot(0, output);
  Rational tempRat;
  bool tempBool=  output.IsProportionalTo(input, tempRat);
  assert(tempBool);
  if (tempRat.IsNegative())
    output.Minus();*/
}

bool Cone::GetLatticePointsInCone
  (Lattice& theLattice, Vector<Rational>& theShift, int upperBoundPointsInEachDim, bool lastCoordinateIsOne,
   Vectors<Rational>& outputPoints, Vector<Rational>* shiftAllPointsBy)
{ if (upperBoundPointsInEachDim<=0)
    upperBoundPointsInEachDim=5;
  Vector<Rational> theActualShift=theShift;
  theLattice.ReduceVector(theActualShift);
  int theDimAffine=this->GetDim();
  if (lastCoordinateIsOne)
    theDimAffine--;
  SelectionWithMaxMultiplicity boundingBox;
  boundingBox.initMaxMultiplicity(theDimAffine, upperBoundPointsInEachDim*2);
  //format of the boundingBox:
  // if bounding box shows a vector (x_1, ...) then
  // it corresponds to a vector with coodinates (x_1-upperBoundPointsInEachDim, x_2-upperBoundPointsInEachDim, ...)
  int numCycles= boundingBox.NumSelectionsTotal();
  if (numCycles<=0 || numCycles>1000000)//we test up to 1 million lattice points.
  //This is very restrictive: in 8 dimensions, selecting upperBoundPointsInEachDim=2,
  //we get a total of (2*2+1)^8=390625 points to test, which is a pretty darn small box
    return false;
  outputPoints.Reserve(numCycles);
  outputPoints.size=0;
  Vector<Rational> candidatePoint;
  Vectors<Rational> LatticeBasis;
  LatticeBasis.AssignMatrixRows(theLattice.basisRationalForm);
  for (int i=0; i<numCycles; i++, boundingBox.IncrementSubset())
  { candidatePoint=theActualShift;
    if (shiftAllPointsBy!=0)
      candidatePoint+=*shiftAllPointsBy;
    for (int j=0; j<boundingBox.Multiplicities.size; j++)
      candidatePoint+=LatticeBasis[j]*
      (boundingBox.Multiplicities[j]-upperBoundPointsInEachDim);
    if (lastCoordinateIsOne)
    { candidatePoint.SetSize(candidatePoint.size+1);
      *candidatePoint.LastObject()=1;
    }
    if (this->IsInCone(candidatePoint))
    {
      outputPoints.AddOnTop(candidatePoint);
    }
  }
  return true;
}

void PiecewiseQuasipolynomial::operator*=(const Rational& other)
{ if (other.IsEqualToZero())
  { this->MakeZero(this->NumVariables, *this->theBuffers);
    return;
  }
  for (int i=0; i<this->theQPs.size; i++)
    this->theQPs[i]*=other;
}

void PiecewiseQuasipolynomial::operator+=(const PiecewiseQuasipolynomial& other)
{ this->MakeCommonRefinement(other);
  for (int i=0; i<this->theProjectivizedComplex.size; i++)
  { int theIndex=other.theProjectivizedComplex.GetLowestIndexchamberContaining
    (this->theProjectivizedComplex[i].GetInternalPoint());
    if (theIndex!=-1)
      this->theQPs[i]+=other.theQPs[theIndex];
  }
}

bool PiecewiseQuasipolynomial::MakeVPF
  (Vectors<Rational>& theRoots, std::string& outputstring, GlobalVariables& theGlobalVariables)
{ if (theRoots.size<=0)
  { outputstring = "Error.";
    return false;
  }
  this->theBuffers=& theGlobalVariables;
  this->NumVariables=theRoots.GetDim();
  partFractions theFracs;
  FormatExpressions theFormat;
  std::stringstream out;
  std::string whatWentWrong;
  if (!theFracs.ArgumentsAllowed(theRoots, whatWentWrong, theGlobalVariables))
  { out << whatWentWrong;
    outputstring= out.str();
    return false;
  }
  theFracs.initFromRoots(theRoots, theGlobalVariables);
  out << CGI::GetHtmlMathDivFromLatexFormulA(theFracs.ElementToString(theGlobalVariables, theFormat));
  theFracs.split(theGlobalVariables, 0);
  out << CGI::GetHtmlMathDivFromLatexFormulA(theFracs.ElementToString(theGlobalVariables, theFormat));
  //theFracs.theChambers.InitFromDirectionsAndRefine(theRoots, theGlobalVariables);
  assert(false);
//  theFracs.theChambersOld.AmbientDimension=theRoots[0].size;
//  theFracs.theChambersOld.theDirections=theRoots;
//  theFracs.theChambersOld.SliceTheEuclideanSpace(theGlobalVariables, false);
//  theFracs.theChambers.AssignCombinatorialChamberComplex(theFracs.theChambersOld, theGlobalVariables);
  this->theQPs.SetSize(theFracs.theChambers.size);
  Vector<Rational> indicator;
  for (int i=0; i< theFracs.theChambers.size; i++)
  { indicator= theFracs.theChambers[i].GetInternalPoint();
    theFracs.GetVectorPartitionFunction(this->theQPs[i], indicator, theGlobalVariables);
    //QuasiPolynomial& currentQP=this->theQPs[i];
  }
  Lattice baseLattice;
  baseLattice.MakeFromRoots(theRoots);
  Cone baseCone;
  baseCone.CreateFromVertices(theRoots, theGlobalVariables);
  Vector<Rational> shiftRoot;
  baseLattice.GetInternalPointInConeForSomeFundamentalDomain(shiftRoot, baseCone, theGlobalVariables);
  shiftRoot.Minus();
//  std::cout << "shiftRoot: " << shiftRoot.ElementToString();
  theFracs.theChambers.MakeAffineAndTransformToProjectiveDimPlusOne
  (shiftRoot, this->theProjectivizedComplex, theGlobalVariables);
  outputstring=out.str();
  return true;
}

bool Lattice::GetInternalPointInConeForSomeFundamentalDomain
(Vector<Rational>& output, Cone& coneContainingOutputPoint, GlobalVariables& theGlobalVariables)
{ Vector<Rational> coordsInBasis;
  coneContainingOutputPoint.GetInternalPoint(output);
  Vectors<Rational> basisRoots, bufferVect;
  Matrix<Rational> bufferMat;
  basisRoots.AssignMatrixRows(this->basisRationalForm);
  if (!output.GetCoordsInBasiS(basisRoots, coordsInBasis, bufferVect, bufferMat))
    return false;
  Rational maxCoord=coordsInBasis[0];
  if (maxCoord<0)
    maxCoord=-maxCoord;
  for (int i=0; i<coordsInBasis.size; i++)
  { Rational tempRat=(coordsInBasis[i]<0)? -coordsInBasis[i] : coordsInBasis[i];
    if (tempRat>maxCoord)
      maxCoord=tempRat;
  }
  maxCoord+=1;
  output/=maxCoord;
  return true;
}

void Cone::TranslateMeMyLastCoordinateAffinization(Vector<Rational>& theTranslationVector)
{ assert(theTranslationVector.size==this->GetDim()-1);
  Vector<Rational> tempRoot;
  for (int i=0; i<this->Normals.size; i++)
  { tempRoot=this->Normals[i];
    tempRoot.size--;
    (*this->Normals[i].LastObject())-= tempRoot.ScalarEuclidean(theTranslationVector);
  }
  tempRoot=theTranslationVector;
  tempRoot.SetSize(theTranslationVector.size+1);
  *tempRoot.LastObject()=0;
  for (int i=0; i<this->Vertices.size; i++)
    if (!this->Vertices[i].LastObject()->IsEqualToZero())
    { Rational tempRat=*this->Vertices[i].LastObject();
      this->Vertices[i]/=tempRat;
      this->Vertices[i]+=tempRoot;
      this->Vertices[i].ScaleByPositiveRationalToIntegralMinHeight();
    }
}

void ConeComplex::GetAllWallsConesNoOrientationNoRepetitionNoSplittingNormals(Vectors<Rational>& output)const
{ HashedList<Vector<Rational> > outputHashed;
  Vector<Rational> tempRoot;
  for (int i=0; i< this->size; i++)
    for (int j=0; j<this->TheObjects[i].Normals.size; j++)
    { tempRoot=this->TheObjects[i].Normals[j];
      tempRoot.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
      outputHashed.AddNoRepetition(tempRoot);
    }
  output.CopyFromBase(outputHashed);
}

void ConeComplex::RefineMakeCommonRefinement(const ConeComplex& other, GlobalVariables& theGlobalVariables)
{ Vectors<Rational> newWalls;
  Cone tempCone=this->ConvexHull;
  if (tempCone.MakeConvexHullOfMeAnd(other.ConvexHull, theGlobalVariables))
  { this->GetAllWallsConesNoOrientationNoRepetitionNoSplittingNormals(newWalls);
    this->init();
    this->ConvexHull=tempCone;
    this->AddNonRefinedChamberOnTopNoRepetition(tempCone, theGlobalVariables);
    this->splittingNormals.AddNoRepetition(newWalls);
  }
  other.GetAllWallsConesNoOrientationNoRepetitionNoSplittingNormals(newWalls);
  this->splittingNormals.AddNoRepetition(newWalls);
  this->indexLowestNonRefinedChamber=0;
  this->Refine(theGlobalVariables);
}

void ConeComplex::TranslateMeMyLastCoordinateAffinization(Vector<Rational>& theTranslationVector, GlobalVariables& theGlobalVariables)
{ ConeComplex myCopy;
  myCopy=*this;
  this->init();
  Cone tempCone;
  for (int i=0; i<myCopy.size; i++)
  { tempCone=myCopy[i];
    tempCone.TranslateMeMyLastCoordinateAffinization(theTranslationVector);
    this->AddNonRefinedChamberOnTopNoRepetition(tempCone, theGlobalVariables);
  }
  Vector<Rational> normalNoAffinePart, newNormal;
  for (int j=0; j<myCopy.splittingNormals.size; j++)
  { normalNoAffinePart= myCopy.splittingNormals[j];
    newNormal=normalNoAffinePart;
    normalNoAffinePart.size--;
    (*newNormal.LastObject())-=normalNoAffinePart.ScalarEuclidean(theTranslationVector);
    this->splittingNormals.AddOnTop(newNormal);
  }
}

void PiecewiseQuasipolynomial::TranslateArgument(Vector<Rational>& translateToBeAddedToArgument, GlobalVariables& theGlobalVariables)
{ assert(this->theBuffers!=0);
  Vector<Rational> chamberShift=-translateToBeAddedToArgument;
//  std::cout << "the translation: " << translateToBeAddedToArgument.ElementToString();
  this->theProjectivizedComplex.TranslateMeMyLastCoordinateAffinization(chamberShift, theGlobalVariables);
  QuasiPolynomial tempQP;
  for (int i=0; i<this->theQPs.size; i++)
  { this->theQPs[i].Substitution(-translateToBeAddedToArgument, tempQP, theGlobalVariables);
    this->theQPs[i]=tempQP;
  }
}

std::string PiecewiseQuasipolynomial::ElementToString(bool useLatex, bool useHtml)
{ std::stringstream out;
  FormatExpressions theFormat;
  for (int i=0; i<this->theProjectivizedComplex.size; i++)
  { Cone& currentCone= this->theProjectivizedComplex[i];
    QuasiPolynomial& currentQP=this->theQPs[i];
    out << "Chamber number " << i+1;
    if (useHtml)
      out << "<br>";
    out << currentCone.ElementToString(false, true, true, true, theFormat);
    if (useHtml)
      out << "<br>";
    out << "quasipolynomial: ";
    if (useLatex& useHtml)
      out << CGI::GetHtmlMathSpanFromLatexFormulaAddBeginArrayRCL(currentQP.ElementToString(useHtml, useLatex));
    else
    { out << currentQP.ElementToString(useHtml, useLatex);
    }
    if (useHtml)
      out << "<hr>";
  }
  return out.str();
}

void PiecewiseQuasipolynomial::DrawMe
(DrawingVariables& theDrawingVars, int numLatticePointsPerDim, Cone* RestrictingChamber,
 Vector<Rational>* distinguishedPoint)
{ FormatExpressions theFormat;
  Vectors<Rational> latticePoints;
  HashedList<Vector<Rational> > theLatticePointsFinal;
  List<int> theLatticePointColors;
  List<int> tempList;
  if (numLatticePointsPerDim<0)
    numLatticePointsPerDim=0;
  int ZeroColor=CGI::RedGreenBlue(200, 200, 200);
  for (int i=0; i<this->theProjectivizedComplex.size; i++)
  { int chamberWallColor=0;
    bool isZeroChamber=this->theQPs[i].IsEqualToZero();
    if (isZeroChamber)
      chamberWallColor= ZeroColor;
    this->theProjectivizedComplex[i].DrawMeLastCoordAffine
    (false, theDrawingVars, theFormat, chamberWallColor);
    std::stringstream tempStream;
    tempStream << i+1;
    Vector<Rational> tempRoot=this->theProjectivizedComplex[i].GetInternalPoint();
    tempRoot.MakeAffineUsingLastCoordinate();
//    theDrawingVars.drawTextAtVectorBuffer
//     (tempRoot, tempStream.str(), chamberWallColor, theDrawingVars.PenStyleNormal, 0);
    for (int j=0; j<this->theQPs[i].LatticeShifts.size; j++)
    { this->theProjectivizedComplex[i].GetLatticePointsInCone
      (this->theQPs[i].AmbientLatticeReduced, this->theQPs[i].LatticeShifts[j], numLatticePointsPerDim, true, latticePoints,
      distinguishedPoint);
      tempList.initFillInObject(latticePoints.size, chamberWallColor);
      if (RestrictingChamber!=0)
        for (int k=0; k<latticePoints.size; k++)
        { tempRoot=latticePoints[k];
          tempRoot.MakeAffineUsingLastCoordinate();
          if (!RestrictingChamber->IsInCone(tempRoot))
            tempList[k]=ZeroColor;
        }
      theLatticePointsFinal.AddOnTop(latticePoints);
      theLatticePointColors.AddListOnTop(tempList);
    }
  }
  for (int i=0; i<theLatticePointsFinal.size; i++)
  { theDrawingVars.drawCircleAtVectorBuffer
    (theLatticePointsFinal[i], 2, theDrawingVars.PenStyleNormal, theLatticePointColors[i]);
    theDrawingVars.drawTextAtVectorBuffer
    (theLatticePointsFinal[i], this->EvaluateInputProjectivized(theLatticePointsFinal[i]).ElementToString(),
     theLatticePointColors[i],
    DrawingVariables::PenStyleNormal, 0);
  }
}

Rational QuasiPolynomial::Evaluate(const Vector<Rational>& input)
{ Vector<Rational> testLatticeBelonging;
  for (int i=0; i<this->LatticeShifts.size; i++)
  { testLatticeBelonging=this->LatticeShifts[i]-input;
    if (this->AmbientLatticeReduced.IsInLattice(testLatticeBelonging))
    { Rational result;
      this->valueOnEachLatticeShift[i].Evaluate(input, result);
      return result;
    }
  }
  return 0;
}

Rational PiecewiseQuasipolynomial::Evaluate(const Vector<Rational>& input)
{ assert(input.size==this->theProjectivizedComplex.GetDim()-1);
  Vector<Rational> ProjectivizedInput=input;
  ProjectivizedInput.SetSize(input.size+1);
  *ProjectivizedInput.LastObject()=1;
  return this->EvaluateInputProjectivized(ProjectivizedInput);
}

Rational PiecewiseQuasipolynomial::EvaluateInputProjectivized(const Vector<Rational>& input)
{ Rational result;
  assert(input.size==this->theProjectivizedComplex.GetDim());
  Vector<Rational> AffineInput=input;
  AffineInput.SetSize(input.size-1);
  int theIndex=this->theProjectivizedComplex.GetLowestIndexchamberContaining(input);
  if (theIndex==-1)
    return 0;
  result=this->theQPs[theIndex].Evaluate(AffineInput);
  //the following for cycle is for self-check purposes only. Comment it out as soon as
  // the code has been tested sufficiently
  for (int i=0; i<this->theProjectivizedComplex.size; i++)
    if (this->theProjectivizedComplex[i].IsInCone(input))
    { Rational altResult=this->theQPs[i].Evaluate(AffineInput);
      if (result!=altResult)
      if (false)
      { static bool firstFail=true;
        if (!firstFail)
          break;
        FormatExpressions tempFormat;
        std::cout << "<hr>Error!!! Failed on chamber " << theIndex+1 << " and " << i+1;
        std::cout << "<br>Evaluating at point " << AffineInput.ElementToString() << "<br>";
        std::cout << "<br>Chamber " << theIndex+1 << ": "
        << this->theProjectivizedComplex[theIndex].ElementToString(false, true, true, true, tempFormat);
        std::cout << "<br>QP: " << this->theQPs[theIndex].ElementToString(true, false);
        std::cout << "<br>value: " << result.ElementToString();
        std::cout << "<br><br>Chamber " << i+1 << ": "
        << this->theProjectivizedComplex[i].ElementToString(false, true, true, true, tempFormat);
        std::cout << "<br>QP: " << this->theQPs[i].ElementToString(true, false);
        std::cout << "<br>value: " << altResult.ElementToString();
        if (firstFail)
        { DrawingVariables tempDV;
          std::cout << "<br><b>Point of failure: " << AffineInput.ElementToString() << "</b>";
          //this->DrawMe(tempDV);
          this->theProjectivizedComplex.DrawMeLastCoordAffine(true, tempDV, tempFormat);
          tempDV.NumHtmlGraphics=5;
          tempDV.theBuffer.drawCircleAtVectorBuffer(AffineInput, 5, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(0,0,0));
          tempDV.theBuffer.drawCircleAtVectorBuffer(AffineInput, 10, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(0,0,0));
          tempDV.theBuffer.drawCircleAtVectorBuffer(AffineInput, 4, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(255,0,0));
          std::cout << "<br>"
          << " <script src=\"http://ajax.googleapis.com/ajax/libs/dojo/1.6.1/dojo/dojo.xd.js\" type=\"text/javascript\"></script>\n";
          std::cout
          << tempDV.GetHtmlFromDrawOperationsCreateDivWithUniqueName(this->theProjectivizedComplex.GetDim()-1);

        }
        firstFail=false;
      }
//      assert(result==altResult);
    }
  return result;
}

void PiecewiseQuasipolynomial::MakeCommonRefinement(const ConeComplex& other)
{ assert(this->theBuffers!=0);
  GlobalVariables& theGlobalVariables=*this->theBuffers;
  List<QuasiPolynomial> oldQPList=this->theQPs;
  ConeComplex oldComplex=this->theProjectivizedComplex;
  this->theProjectivizedComplex.RefineMakeCommonRefinement(other, theGlobalVariables);
  this->theQPs.SetSize(this->theProjectivizedComplex.size);
  for (int i=0; i<this->theProjectivizedComplex.size; i++)
  { int theOldIndex=oldComplex.GetLowestIndexchamberContaining(this->theProjectivizedComplex[i].GetInternalPoint());
    if (theOldIndex!=-1)
      this->theQPs[i]=oldQPList[theOldIndex];
    else
    //the below needs to be fixed!!!!!
      this->theQPs[i].MakeZeroLatTiceZn(this->GetNumVars());
  }
}

void Lattice::GetDefaultFundamentalDomainInternalPoint
(Vector<Rational>& output)
{ output.MakeZero(this->GetDim());
  Vector<Rational> tempRoot;
  for (int i=0; i<this->basisRationalForm.NumRows; i++)
  { this->basisRationalForm.RowToRoot(i, tempRoot);
    output+=tempRoot;
  }
  output/=2;
}

bool partFractions::split(GlobalVariables& theGlobalVariables, Vector<Rational>* Indicator)
{ //partFraction::flagAnErrorHasOccurredTimeToPanic=true;
  //this->flagAnErrorHasOccurredTimeToPanic=true;
  if (!this->flagInitialized)
  { this->IndexLowestNonProcessed=0;
    this->PrepareIndicatorVariables();
    this->PrepareCheckSums(theGlobalVariables);
    this->flagInitialized=true;
  }
//  std::cout << "<br>checksum start: " << this->StartCheckSum.ElementToString();
  if (this->splitPartial(theGlobalVariables, Indicator))
  { //this->ComputeDebugString();
//    this->CompareCheckSums(theGlobalVariables);
    this->RemoveRedundantShortRoots(theGlobalVariables, Indicator);
    //this->ComputeDebugString();
    //this->ComputeDebugString();
    this->CompareCheckSums(theGlobalVariables);
    this->IndexLowestNonProcessed= this->size;
    this->MakeProgressReportSplittingMainPart(theGlobalVariables);
  }
//  std::cout << "<br>checksum finish: " << this->EndCheckSum.ElementToString();

  return false;
}

void Cone::ChangeBasis
  (Matrix<Rational> & theLinearMap, GlobalVariables& theGlobalVariables)
{ //Vectors<Rational> newNormals;
//  Matrix<Rational>  tempMat=theLinearMap;
  theLinearMap.ActOnVectorsColumn(this->Normals);
  this->CreateFromNormals(this->Normals, theGlobalVariables);
}

Vector<Rational> WeylGroup::GetSimpleCoordinatesFromFundamental
(const Vector<Rational>& inputInFundamentalCoords)
{ Matrix<Rational> & tempMat=*this->GetMatrixFundamentalToSimpleCoords();
  Vector<Rational> result;
  result=inputInFundamentalCoords;
  tempMat.ActOnVectorColumn(result);
  return result;
}

Vector<Rational> WeylGroup::GetFundamentalCoordinatesFromSimple
(const Vector<Rational>& inputInFundamentalCoords)
{ Matrix<Rational> & tempMat=*this->GetMatrixSimpleToFundamentalCoords();
  Vector<Rational> result=inputInFundamentalCoords;
  tempMat.ActOnVectorColumn(result);
  return result;
}

std::string GeneralizedVermaModuleCharacters::ComputeMultsLargerAlgebraHighestWeight
  (Vector<Rational>& highestWeightLargerAlgebraFundamentalCoords, Vector<Rational>& parabolicSel, Parser& theParser,
   GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  WeylGroup& LargerWeyl=theParser.theHmm.theRange().theWeyl;
  WeylGroup& SmallerWeyl=theParser.theHmm.theDomain().theWeyl;
  if (LargerWeyl.GetDim()!=3 || LargerWeyl.WeylLetter!='B')
    return "Error: algebra is not so(7).";
  this->initFromHomomorphism(parabolicSel, theParser.theHmm, theGlobalVariables);
  this->TransformToWeylProjectiveStep1(theGlobalVariables);
  this->TransformToWeylProjectiveStep2(theGlobalVariables);
  Vector<Rational> highestWeightLargerAlgSimpleCoords;
  highestWeightLargerAlgSimpleCoords=LargerWeyl.GetSimpleCoordinatesFromFundamental
  (highestWeightLargerAlgebraFundamentalCoords);

  Matrix<Rational>  tempMat;


  Vector<Rational> tempRoot, ZeroRoot;
  DrawingVariables& drawOps=theGlobalVariables.theDrawingVariables;
  int theSmallDim=SmallerWeyl.CartanSymmetric.NumRows;
//  drawOps.theBuffer.initDimensions(theSmallDim, 1);
  Vectors<double> theDraggableBasis;
  theDraggableBasis.MakeEiBasis(theSmallDim, 1, 0);
  WeylGroup tmpWeyl;
  tmpWeyl.MakeAn(2);
  drawOps.theBuffer.initDimensions(tmpWeyl.CartanSymmetric, theDraggableBasis, theDraggableBasis, 1);
  FormatExpressions theFormat;
  drawOps.theBuffer.BasisProjectionPlane[0][0][0]=1; drawOps.theBuffer.BasisProjectionPlane[0][0][1]=0;
  drawOps.theBuffer.BasisProjectionPlane[0][1][0]=1;  drawOps.theBuffer.BasisProjectionPlane[0][1][1]=1;
  drawOps.theBuffer.ModifyToOrthonormalNoShiftSecond
  (drawOps.theBuffer.BasisProjectionPlane[0][1], drawOps.theBuffer.BasisProjectionPlane[0][0]);
  drawOps.theBuffer.GraphicsUnit[0]=50;
  PiecewiseQuasipolynomial theStartingPoly(theGlobalVariables),
  theSubbedPoly(theGlobalVariables), Accum(theGlobalVariables);
  //std::cout << "<hr>" << this->GmodKNegWeightsBasisChanged.ElementToString() << "<hr>";
  std::string tempS;
  theStartingPoly.MakeVPF(this->GmodKNegWeightsBasisChanged, tempS, theGlobalVariables);
  Vectors<Rational> translationsProjectedFinal;
  translationsProjectedFinal.SetSize(this->theLinearOperators.size);
  this->theLinearOperators[0].ActOnVectorColumn(highestWeightLargerAlgSimpleCoords, translationsProjectedFinal[0]);
//  translationsProjectedFinal[0].Minus();
  out << "<br>Input so(7)-highest weight: " << highestWeightLargerAlgSimpleCoords.ElementToString();
  out << "<br>Input parabolics selections: " << parabolicSel.ElementToString();
  out << "<br>the argument translations: " << this->theTranslationsProjectedBasisChanged.ElementToString();
  out << "<br>Element u_w: projection, multiplication by -1, and basis change of so(7)-highest weight to G_2: "
  << translationsProjectedFinal[0].ElementToString();
  theStartingPoly.MakeVPF(this->GmodKNegWeightsBasisChanged, tempS, theGlobalVariables);
  //std::cout << theStartingPoly.ElementToString(false, true);
  drawOps.drawCoordSystemBuffer(drawOps, 2, 0);
  //out << this->log.str();
  Cone smallWeylChamber;
  tempMat=SmallerWeyl.CartanSymmetric;
  tempMat.Invert();
  Vectors<Rational> tempVertices;
  Vector<Rational> tMpRt;
  tMpRt=this->ParabolicSelectionSmallerAlgebra;
//  std::cout << "<br>sel smaller: " << tMpRt.ElementToString();
  for (int i=0; i<this->ParabolicSelectionSmallerAlgebra.MaxSize; i++)
  { tempMat.RowToRoot(i, tempRoot);
    tempVertices.AddOnTop(tempRoot);
    if(this->ParabolicSelectionSmallerAlgebra.selected[i])
      tempVertices.AddOnTop(-tempRoot);
  }
  smallWeylChamber.CreateFromVertices(tempVertices, theGlobalVariables);
  tempMat.init(2,2);
  tempMat.elements[0][0]=1; tempMat.elements[0][1]=0;
  tempMat.elements[1][0]=1; tempMat.elements[1][1]=1;

//  std::cout << smallWeylChamber.ElementToString(false, true, theFormat);
  tempMat.Transpose();
  smallWeylChamber.ChangeBasis
  (tempMat, theGlobalVariables)
  ;
//  std::cout << "<br> after the basis change: " << smallWeylChamber.ElementToString(false, true, theFormat);
  out << "<br> The small Weyl chamber: " << smallWeylChamber.ElementToString(false, true, theFormat);
  Vector<Rational> highestWeightSmallAlgBasisChanged= -translationsProjectedFinal[0];
//  std::cout << highestWeightSmallAlgBasisChanged.ElementToString();
  theGlobalVariables.MaxAllowedComputationTimeInSeconds=100;
  for (int i=0; i<this->theLinearOperators.size; i++)
  { this->theLinearOperators[i].ActOnVectorColumn(highestWeightLargerAlgSimpleCoords, translationsProjectedFinal[i]);
    translationsProjectedFinal[i]+=this->theTranslationsProjectedBasisChanged[i];
    drawOps.drawCircleAtVectorBuffer(-translationsProjectedFinal[i], 3, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(250,0,0));
  }
  out << "<br>the translations projected final: " << translationsProjectedFinal.ElementToString();
  Accum.MakeZero(theStartingPoly.NumVariables, theGlobalVariables);
  for (int i=0; i<this->theLinearOperators.size; i++)
  { theSubbedPoly=theStartingPoly;
    theSubbedPoly*=this->theCoeffs[i];
    theSubbedPoly.TranslateArgument(translationsProjectedFinal[i], theGlobalVariables);
    //theSubbedPoly.DrawMe(tempVars);
   /* if (i==2)
    { DrawingVariables tempDV, tempDV2;
      tempDV.NumHtmlGraphics=100;
      tempDV2.NumHtmlGraphics=109;
      std::cout << "<hr><hr>first point of failure: <hr>accum: ";
      Accum.DrawMe(tempDV);
      tempDV.drawCoordSystemBuffer(tempDV, 2, 0);
      std::cout << tempDV.GetHtmlFromDrawOperationsCreateDivWithUniqueName(2);
      std::cout << "<hr>subbed poly: ";
      theSubbedPoly.DrawMe(tempDV2);
      tempDV.drawCoordSystemBuffer(tempDV2, 2, 0);
      std::cout << tempDV2.GetHtmlFromDrawOperationsCreateDivWithUniqueName(2);
    }*/
    Accum+=theSubbedPoly;
/*    if (i==2)
    { DrawingVariables tempDrawOps;
      tempDrawOps.NumHtmlGraphics=500;
      std::cout << "<hr><hr> <b>Index: " << i+1 << " out of " << this->theLinearOperators.size << "</b> <hr>";
      Accum.DrawMe(tempDrawOps);
      std::cout << tempDrawOps.GetHtmlFromDrawOperationsCreateDivWithUniqueName(2);
    }
    */
  }
//  std::cout << "<hr>so far so good!";
  drawOps.theBuffer.theDrawCircleAtVectorOperations.Reserve(2500);
  Accum.DrawMe(drawOps, 10, &smallWeylChamber, &highestWeightSmallAlgBasisChanged);
//  smallWeylChamber.DrawMeProjective(0, false, drawOps, theFormat);
//  out << tempVars.GetHtmlFromDrawOperationsCreateDivWithUniqueName(2);
  out << drawOps.GetHtmlFromDrawOperationsCreateDivWithUniqueName(2);
  out << Accum.ElementToString(false, true);
/*  out << refinedProjectivized.ElementToString(false, true);*/
/*
  for (int i=0; i<this->theLinearOperators.size; i++)
  { this->theLinearOperators[i].ActOnVectorColumn(highestWeightLargerAlg, tempRoot);
    tempRoot.Minus();
    tempRoot-=this->theTranslationsProjecteD[i];
    drawOps.
    drawCircleAtVectorBuffer(tempRoot, 2, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(0,0,100));
    drawOps.drawLineBetweenTwoVectorsBuffer(tempRoot, ZeroRoot, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(240, 240, 240));
    tempComplex.DrawMeProjective(&tempRoot, false, drawOps, tempFormat);
    out << tempRoot.ElementToString();
  }
  drawOps.drawCoordSystemBuffer(drawOps, 3, 0);
  out << drawOps.GetHtmlFromDrawOperationsCreateDivWithUniqueName(2);*/

  return out.str();
}

void GeneralizedVermaModuleCharacters::SortMultiplicities(GlobalVariables& theGlobalVariables)
{ List<Cone> tempList;
  tempList=this->projectivizedChambeR;
  tempList.QuickSortAscending();
  List<QuasiPolynomial> tempQPlist;
  tempQPlist.SetSize(this->theMultiplicities.size);
  for (int i=0; i<this->theMultiplicities.size; i++)
    tempQPlist[i]=this->theMultiplicities[this->projectivizedChambeR.GetIndex(tempList[i])];
  this->theMultiplicities=tempQPlist;
  this->projectivizedChambeR.Clear();
  for (int i=0; i<tempList.size; i++)
    this->projectivizedChambeR.AddOnTop(tempList[i]);
}

void DynkinDiagramRootSubalgebra::ComputeDynkinString
(int indexComponent, WeylGroup& theWeyl)
{ assert(indexComponent<this->SimpleBasesConnectedComponents.size);
  std::stringstream out;
  Vectors<Rational>& currentComponent= this->SimpleBasesConnectedComponents.TheObjects[indexComponent];
  List<int>& currentEnds=this->indicesEnds.TheObjects[indexComponent];
  if (this->numberOfThreeValencyNodes(indexComponent, theWeyl)==1)
  {//type D or E
    //in type D first comes the triple node, then the long string, then the one-Vector<Rational> strings
    // the long string is oriented with the end that is connected to the triple node having
    //smaller index
    //in type E similarly the longest string comes first oriented with the Vector<Rational> that is linked to the triple node having smaller index
    // then comes the second longest string (oriented in the same fashion)
    // and last the one-Vector<Rational> string
    Vector<Rational> tripleNode;
    int tripleNodeindex=this->indicesThreeNodes.TheObjects[indexComponent];
    tripleNode=( currentComponent.TheObjects[tripleNodeindex]);
    Vectors<Rational> tempRoots;
    tempRoots.CopyFromBase(currentComponent);
    tempRoots.PopIndexSwapWithLast(tripleNodeindex);
    DynkinDiagramRootSubalgebra tempDiagram;
    tempDiagram.ComputeDiagramTypeKeepInput(tempRoots, theWeyl);
    assert(tempDiagram.SimpleBasesConnectedComponents.size==3);
    List<int> indicesLongComponents;
    indicesLongComponents.size=0;
    Rational tempRat;
    for (int i=0; i<3; i++)
    { if(tempDiagram.SimpleBasesConnectedComponents.TheObjects[i].size>1)
        indicesLongComponents.AddOnTop(i);
      theWeyl.RootScalarCartanRoot(tempDiagram.SimpleBasesConnectedComponents.TheObjects[i].TheObjects[0], currentComponent.TheObjects[tripleNodeindex], tempRat);
      if (tempRat.IsEqualToZero())
        tempDiagram.SimpleBasesConnectedComponents.TheObjects[i].ReverseOrderElements();
    }
    for(int i=0; i<3; i++)
      for(int j=i+1; j<3; j++)
        if (tempDiagram.SimpleBasesConnectedComponents.TheObjects[i].size<tempDiagram.SimpleBasesConnectedComponents.TheObjects[j].size)
        { tempRoots.CopyFromBase(tempDiagram.SimpleBasesConnectedComponents.TheObjects[i]);
          tempDiagram.SimpleBasesConnectedComponents.TheObjects[i].CopyFromBase(tempDiagram.SimpleBasesConnectedComponents.TheObjects[j]);
          tempDiagram.SimpleBasesConnectedComponents.TheObjects[j].CopyFromBase(tempRoots);
        }
    currentComponent.size=0;
    currentComponent.AddOnTop(tripleNode);
    for (int i=0; i<3; i++)
      currentComponent.AddListOnTop(tempDiagram.SimpleBasesConnectedComponents.TheObjects[i]);
    if ( indicesLongComponents.size==1 || indicesLongComponents.size==0)
      out << this->SetComponent("D", currentComponent.size, indexComponent);
    else
    {//type E
      assert(indicesLongComponents.size==2);
      out << this->SetComponent("E", currentComponent.size, indexComponent);
    }
  }else
  { Rational length1, length2, tempRat;
    theWeyl.RootScalarCartanRoot(currentComponent.TheObjects[0], currentComponent.TheObjects[0], length1);
    int numLength1=1; int numLength2=0;
    for(int i=1; i<currentComponent.size; i++)
    { theWeyl.RootScalarCartanRoot(currentComponent.TheObjects[i], currentComponent.TheObjects[i],  tempRat);
      if (tempRat.IsEqualTo(length1))
        numLength1++;
      else
      { numLength2++;
        length2.Assign(tempRat);
      }
    }
    if (numLength2==0 )
    { //type A
      if (length1.IsEqualTo(theWeyl.LongRootLength))
        out << this->SetComponent("A", numLength1, indexComponent);
      else
        out << this->SetComponent("A'", numLength1, indexComponent);
    }
    else
    {//the longer Vector<Rational> should have smaller index
      Rational greaterlength, tempRat;
      int numGreaterLength=numLength2;
      int numSmallerLength=numLength1;
      greaterlength.Assign(length2);
      if (length1.IsGreaterThan(length2))
      { greaterlength.Assign(length1);
        numGreaterLength=numLength1;
        numSmallerLength=numLength2;
      }
      theWeyl.RootScalarCartanRoot(currentComponent.TheObjects[currentEnds.TheObjects[0]], currentComponent.TheObjects[currentEnds.TheObjects[0]], tempRat);
      if (greaterlength.IsGreaterThan(tempRat))
        currentEnds.SwapTwoIndices(0, 1);
      if (numLength1==numLength2)
      {//B2, C2, F4 or G2
        if (numLength1!=1)
        { out << this->SetComponent("F", 4, indexComponent);
          assert(numLength1==2);
        } else
        { if (length1.NumShort==6 || length2.NumShort==6)
            out << this->SetComponent("G", 2, indexComponent);
          else
            out << this->SetComponent("B", 2, indexComponent);
        }
      } else
      { if (numGreaterLength>numSmallerLength)
          out << this->SetComponent("B", currentComponent.size, indexComponent);
        else
          out << this->SetComponent("C", currentComponent.size, indexComponent);
      }
    }
    currentComponent.SwapTwoIndices(0, currentEnds.TheObjects[0]);
    for (int i=0; i<currentComponent.size; i++)
      for (int j=i+1; j<currentComponent.size; j++)
      { theWeyl.RootScalarCartanRoot(currentComponent.TheObjects[i], currentComponent.TheObjects[j], tempRat);
        if (!tempRat.IsEqualToZero())
        { currentComponent.SwapTwoIndices(i+1, j);
          break;
        }
      }
  }
  this->DynkinTypeStrings.TheObjects[indexComponent]=out.str();
}

std::string DynkinDiagramRootSubalgebra::SetComponent(const std::string& WeylLetterWithLength, int WeylRank, int componentIndex)
{ this->ComponentLetters[componentIndex]=WeylLetterWithLength;
  this->ComponentRanks[componentIndex]=WeylRank;
  std::string result=this->GetNameFrom(WeylLetterWithLength, WeylRank, false);
  this->DynkinTypeStrings[componentIndex]=result;
  return result;
}

std::string DynkinDiagramRootSubalgebra::GetNameFrom
  (const std::string& WeylLetterWithLength , int WeylRank, bool IncludeAlgebraNames)
{ std::stringstream out;
  out << WeylLetterWithLength << "_" << WeylRank;
  if (IncludeAlgebraNames && (WeylLetterWithLength[0]=='A' || WeylLetterWithLength[0]=='B' || WeylLetterWithLength[0]=='C' || WeylLetterWithLength[0]=='D'))
    out << "(" << SemisimpleLieAlgebra::GetLieAlgebraName(WeylLetterWithLength[0], WeylRank, true) << ")";
  return out.str();
}

void DynkinDiagramRootSubalgebra::ElementToStrinG
  (std::string& output, bool useDollarSigns, bool IncludeAlgebraNames)
{ std::stringstream out;
  for (int j=0; j<this->sameTypeComponents.size; j++)
  { int numSameTypeComponents= this->sameTypeComponents.TheObjects[j].size;
    if (useDollarSigns)
      out << "$";
    if (numSameTypeComponents!=1)
      out << numSameTypeComponents;
    out << this->GetNameFrom(this->ComponentLetters[this->sameTypeComponents[j][0]], this->ComponentRanks[this->sameTypeComponents[j][0]], IncludeAlgebraNames);
    if (useDollarSigns)
      out << "$";
    if (j!=this->sameTypeComponents.size-1)
      out << "+";
  }
  output=out.str();
}

void WeylGroup::GetWeylChamber
(Cone& output, GlobalVariables& theGlobalVariables)
{ Matrix<Rational>  tempMat;
  tempMat=this->CartanSymmetric;
  tempMat.Invert();
  Vectors<Rational> tempRoots;
  tempRoots.AssignMatrixRows(tempMat);
  output.CreateFromVertices(tempRoots, theGlobalVariables);
//  output.CreateFromNormals(tempRoots, theGlobalVariables);
}

std::string GeneralizedVermaModuleCharacters::CheckMultiplicitiesVsOrbits
  (GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  int totalDimAffine=this->WeylLarger.GetDim()+this->WeylSmaller.GetDim();
  int smallDim=this->WeylSmaller.GetDim();
  Vector<Rational> normal;
  normal.MakeZero(totalDimAffine+1);
  Vectors<Rational> newWalls;
  ConeComplex tempComplex;
  tempComplex=this->projectivizedChambeR;
  for (int i=0; i<this->WeylChamberSmallerAlgebra.Normals.size; i++)
  { for (int j=0; j<smallDim; j++)
      normal[j]=this->WeylChamberSmallerAlgebra.Normals[i][j];
    newWalls.AddOnTop(normal);
    tempComplex.splittingNormals.AddOnTop(normal);
  }
  tempComplex.indexLowestNonRefinedChamber=0;
  tempComplex.Refine(theGlobalVariables);
  out << "Number chambers with new walls: " << tempComplex.size;
  out << "\n" << tempComplex.ElementToString();
  return out.str();
}

void GeneralizedVermaModuleCharacters::IncrementComputation
  (Vector<Rational>& parabolicSel, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
//  this->UpperLimitChambersForDebugPurposes=5;
  Polynomial<Rational> ::PreferredHashSize=10;
  this->thePauseControlleR.InitComputation();
  this->ParabolicLeviPartRootSpacesZeroStandsForSelected=parabolicSel;
  if (false)
  if (this->UpperLimitChambersForDebugPurposes==0 || this->theLinearOperators.size==0)
    this->ReadFromDefaultFile(&theGlobalVariables);
  switch (this->computationPhase)
  { case 0:
      this->theParser.theHmm.MakeG2InB3(this->theParser, theGlobalVariables);
      this->initFromHomomorphism(parabolicSel, this->theParser.theHmm, theGlobalVariables);
      this->TransformToWeylProjectiveStep1(theGlobalVariables);
      out << theGlobalVariables.theIndicatorVariables.StatusString1;
      this->TransformToWeylProjectiveStep2(theGlobalVariables);
      out << theGlobalVariables.theIndicatorVariables.StatusString1;
      break;
    case 1:
      this->projectivizedChambeR.Refine(theGlobalVariables);
      this->SortMultiplicities(theGlobalVariables);
      out << this->projectivizedChambeR.ElementToString(false, false);
//      out << theGlobalVariables.theIndicatorVariables.StatusString1;
      break;
    case 2:
      this->ComputeQPsFromChamberComplex(theGlobalVariables);
      out << this->ElementToStringMultiplicitiesReport(theGlobalVariables);
      break;
    case 3:
//      out << this->CheckMultiplicitiesVsOrbits(theGlobalVariables);
      break;
    case 4:
      this->InitTheMaxComputation(theGlobalVariables);
      out << theGlobalVariables.theIndicatorVariables.StatusString1;
      break;
    case 5:
      this->theMaxComputation.FindExtremaParametricStep1(this->thePauseControlleR, true, theGlobalVariables);
      out << theGlobalVariables.theIndicatorVariables.StatusString1;
      break;
    case 6:
      this->theMaxComputation.FindExtremaParametricStep3(this->thePauseControlleR, theGlobalVariables);
      out << theGlobalVariables.theIndicatorVariables.StatusString1;
      break;
    case 7:
      this->theMaxComputation.FindExtremaParametricStep4(this->thePauseControlleR, theGlobalVariables);
      out << theGlobalVariables.theIndicatorVariables.StatusString1;
      break;
    case 8:
      this->theMaxComputation.FindExtremaParametricStep5(this->thePauseControlleR, theGlobalVariables);
      out << theGlobalVariables.theIndicatorVariables.StatusString1;
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
  if (this->UpperLimitChambersForDebugPurposes<=0)
    if (this->computationPhase < 30)
      this->WriteToDefaultFile(&theGlobalVariables);
  this->thePauseControlleR.ExitComputation();
}


void Cone::TransformToWeylProjective
  (ConeComplex& owner, GlobalVariables& theGlobalVariables)
{ assert(false);
/*
  for (int i=0; i<this->Externalwalls.size; i++)
    this->Externalwalls.TheObjects[i].TransformToWeylProjective(theGlobalVariables);
  WallData newWall;
  this->Externalwalls.Reserve(owner.WeylChamber.size+this->Externalwalls.size);
  Vectors<Rational> newExternalWalls;
  owner.GetWeylChamberWallsForCharacterComputation(newExternalWalls);
  for (int i=0; i<newExternalWalls.size; i++)
  { newWall.normal=newExternalWalls.TheObjects[i];
    this->Externalwalls.AddOnTop(newWall);
  }
  this->AllVertices.size=0;
  this->ComputeVerticesFromNormals(owner, theGlobalVariables);*/
}

void ConeComplex::TransformToWeylProjective
  (GlobalVariables& theGlobalVariables)
{/* this->AmbientWeyl.GetElement().ComputeWeylGroup();
  this->log << this->AmbientWeyl.GetElement().ElementToString();
  std::string tempS;
  this->ElementToString(tempS);
  this->log << "\nWeyl chamber: " << this->WeylChamber.ElementToString() << "\n";
  this->log << tempS;
  this->NewHyperplanesToSliceWith.size=0;
  this->theHyperplanes.size=0;
  Vector<Rational> wallToSliceWith;
//  Vectors<Rational> oldDirections;
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
          this->theHyperplanes.AddNoRepetition(wallToSliceWith);
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
  Vector<Rational> tempRoot;
  for (int i=0; i<this->TheGlobalConeNormals.size; i++)
  { tempRoot.MakeZero(this->AmbientDimension);
    int startingDim=this->TheGlobalConeNormals.TheObjects[i].size;
    for (int j=0; j<startingDim; j++)
    { tempRoot.TheObjects[j]=this->TheGlobalConeNormals.TheObjects[i].TheObjects[j];
      tempRoot.TheObjects[j+startingDim]=-tempRoot.TheObjects[j];
    }
    this->TheGlobalConeNormals.TheObjects[i]=tempRoot;
  }
  Vectors<Rational> tempRoots;
  this->GetWeylChamberWallsForCharacterComputation(tempRoots);
  this->TheGlobalConeNormals.AddListOnTop(tempRoots);
  this->log << "the global cone normals: " << this->TheGlobalConeNormals.ElementToString();
  this->ElementToString(tempS);
  this->log << tempS;
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.theIndicatorVariables.StatusString1=this->log.str();
  theGlobalVariables.MakeReport();*/
}

int ParserNode::EvaluateWeylAction
  (ParserNode& theNode,
   List<int>& theArgumentList, GlobalVariables& theGlobalVariables,
   bool DualAction, bool useRho, bool useMinusRho)
{ Vector<RationalFunction> theWeight;
  WeylGroup& theWeyl=theNode.owner->theHmm.theRange().theWeyl;
  if (theArgumentList.size!=theWeyl.GetDim())
    return theNode.SetError(theNode.errorDimensionProblem);
  int theDim=theArgumentList.size;
  theNode.impliedNumVars=theNode.GetMaxImpliedNumVarsChildren();
  theWeight.SetSize(theDim);
  for (int i=0; i<theArgumentList.size; i++)
  { theWeight.TheObjects[i]=theNode.owner->TheObjects[theArgumentList.TheObjects[i]].ratFunction.GetElement();
    theWeight.TheObjects[i].SetNumVariablesSubDeletedVarsByOne(theNode.impliedNumVars);
  }
  std::stringstream out;
  theWeyl.ComputeWeylGroup(51840);
//  std::cout << theWeight.ElementToString();
  out << "We denote the Weyl group elements by using their minimal-length expressions using simple reflections.<br>"
  << " In what follows, s_{\\eta_ i} denotes the reflection with respect to the i^th simple Vector<Rational>. <br>The elements of the Weyl group are given in Bruhat order, "
  << "i.e. are sorted according to the length of their expression using simple reflections.<br>";
  if (theWeyl.size>=51840)
    out << "Only the first 51840 elements have been computed. <br> If you want a larger computation <br> please use the C++ code directly.";
  out << "Number of elements: " << theWeyl.size << "<br>";
  Vector<RationalFunction> theOrbitElement;
  RationalFunction RFZero;
  RFZero.MakeZero(theNode.impliedNumVars, &theGlobalVariables);
  std::stringstream tempStream;
  tempStream << "\\begin{array}{rcl}";
  for (int i=0; i<theWeyl.size; i++)
  { theOrbitElement=theWeight;
    if (!DualAction)
      theWeyl.ActOn<RationalFunction>(i, theOrbitElement, useRho, useMinusRho, RFZero);
    else
    {
    }
    tempStream << theWeyl[i].ElementToString() << "\\circ " << theWeight.ElementToString() << " &=& " << theOrbitElement.ElementToString() << " \\\\ <br>";
  }
  tempStream << "\\end{array}";
  out << CGI::GetHtmlMathDivFromLatexFormulA(tempStream.str());
  theNode.outputString=out.str();
  theNode.ExpressionType=theNode.typeString;
  return theNode.errorNoError;
}

void GeneralizedVermaModuleCharacters::GetSubFromNonParamArray
(Matrix<Rational>& output, Vector<Rational>& outputTranslation, Vectors<Rational>& NonParams, int numParams)
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

void ConeLatticeAndShiftMaxComputation::init
(Vector<Rational>& theNEq, Cone& startingCone, Lattice& startingLattice, Vector<Rational>& startingShift)
{ ConeLatticeAndShift theCLS;
  theCLS.theProjectivizedCone=startingCone;
  theCLS.theLattice=startingLattice;
  theCLS.theShift=startingShift;
  this->numNonParaM=0;
  this->numProcessedNonParam=0;
  this->LPtoMaximizeLargerDim.size=0;
  this->LPtoMaximizeSmallerDim.size=0;
  this->theStartingRepresentative=startingShift;
  this->theFinalRepresentatives.size=0;
  this->complexStartingPerRepresentative.size=0;
  this->complexRefinedPerRepresentative.size=0;
  this->theConesLargerDim.AddOnTop(theCLS);
  this->LPtoMaximizeLargerDim.AddOnTop(theNEq);
  this->IsInfinity.initFillInObject(1, false);
}

int ParserNode::EvaluateFindExtremaInDirectionOverLattice
    (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ Polynomial<Rational> & theLinPoly=theNode.owner->TheObjects[theArgumentList.TheObjects[0]].polyValue.GetElement();
 // int numNonParams =  theNode.owner->TheObjects[theArgumentList.TheObjects[1]].intValue;
  //int numParams =  theNode.owner->TheObjects[theArgumentList.TheObjects[2]].intValue;
  ParserNode& theShiftNode= theNode.owner->TheObjects[theArgumentList[1]];
  Lattice& currentLattice=theNode.owner->TheObjects[theArgumentList[2]].theLattice.GetElement();
  Cone& currentCone=theNode.owner->TheObjects[theArgumentList[3]].theCone.GetElement();
  int numNonParam=theNode.owner->TheObjects[theArgumentList[4]].intValue;
  Vector<Rational> theShift;
  Vector<Rational> theNEqBeforeCheck, theNEq;
  if(!theLinPoly.IsLinearGetRootConstantTermLastCoordinate(theNEqBeforeCheck, (Rational) 0))
    return theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);
  if(theNEqBeforeCheck.size>currentCone.GetDim())
    return theNode.SetError(theNode.errorDimensionProblem);
  else
  { theNEq.MakeZero(currentCone.GetDim());
    for (int i=0; i<theNEqBeforeCheck.size; i++)
      theNEq[i]=theNEqBeforeCheck[i];
  }
  if (!theShiftNode.GetRootRationalDontUseForFunctionArguments(theShift, theGlobalVariables))
    return theNode.SetError(errorDimensionProblem);
  if (theShift.size!=theNEq.size-1)
    return theNode.SetError(errorDimensionProblem);
/*  if (theNEq.size!=numNonParams+numParams+1)
    return theNode.SetError(theNode.errorBadOrNoArgument);*/
  if (numNonParam>theShift.size-1)
    return theNode.SetError(errorImplicitRequirementNotSatisfied);
  GeneralizedVermaModuleCharacters tempChars;
  List<int> nodesCurrentRoot;
  List<Vectors<Rational> > outputParamChambers, outputNonParamVertices;
  std::stringstream out;
  out << "Input data: normal: " << theNEq.ElementToString()
//  << "; numNonParams: " << numNonParams << "; numParams: " << numParams
  << "; cone: " << currentCone.ElementToString(false, true, false, true, theGlobalVariables.theDefaultPolyFormat);
  std::cout << "Input data: normal: " << theNEq.ElementToString()
//  << "; numNonParams: " << numNonParams << "; numParams: " << numParams
  << "; cone: " << currentCone.ElementToString(false, true, false, true, theGlobalVariables.theDefaultPolyFormat);
  ConeLatticeAndShiftMaxComputation theComputation;
  theComputation.init(theNEq, currentCone, currentLattice, theShift);
  Controller pauseController;
  theComputation.numNonParaM=numNonParam;
  theComputation.FindExtremaParametricStep1(pauseController, false, theGlobalVariables);
  out << "<hr><hr><hr>" << theComputation.ElementToString(&theGlobalVariables.theDefaultPolyFormat);
  theNode.outputString=out.str();
  theNode.ExpressionType=theNode.typeString;
  return theNode.errorNoError;
}

int ParserNode::EvaluateSliceCone
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ Cone& theCone=theNode.owner->TheObjects[theArgumentList.TheObjects[0]].theCone.GetElement();
  Vector<Rational> theDirection;
  if (! theNode.owner->TheObjects[theArgumentList.TheObjects[1]].GetRootRationalDontUseForFunctionArguments(theDirection, theGlobalVariables))
    return theNode.errorProgramming;
  if (theCone.GetDim()!=theDirection.size)
    return theNode.errorDimensionProblem;
  if (theCone.flagIsTheZeroCone || theCone.Normals.size==0)
  { theNode.outputString="Error: the cone to be sliced is either empty or is the entire space.";
    return theNode.errorImplicitRequirementNotSatisfied;
  }
  ConeComplex theComplex;
  FormatExpressions tempFormat;
  theCone.SliceInDirection(theDirection, theComplex, theGlobalVariables);
  theNode.outputString=theComplex.DrawMeToHtmlProjective(theGlobalVariables.theDrawingVariables, tempFormat);
//  theNode.outputString=theComplex.ElementToString(false, true);
  theNode.ExpressionType=theNode.typeString;
  return theNode.errorNoError;
}

bool Cone::ProduceNormalFromTwoNormalsAndSlicingDirection
  (Vector<Rational>& SlicingDirection, Vector<Rational>& normal1, Vector<Rational>& normal2, Vector<Rational>& output)
{ // we are looking for a normal n of the form n=t1*normal1+t2*normal2
  // such that  <t1*normal1+t2*normal2, slicingDirection>=0
  Rational normal1ScalarDirection=normal1.ScalarEuclidean(SlicingDirection);
  if (normal1ScalarDirection.IsEqualToZero())
  { output=normal1;
    return false;
  }
  //from now on we assume t2=1;
  Rational t1=-normal2.ScalarEuclidean(SlicingDirection)/normal1ScalarDirection;
  output=normal2;
  output+=normal1*t1;
  output.ScaleByPositiveRationalToIntegralMinHeight();
  return true;
}

void Cone::SliceInDirection
  (Vector<Rational>& theDirection, ConeComplex& output, GlobalVariables& theGlobalVariables)
{ output.init();
  output.AddNonRefinedChamberOnTopNoRepetition(*this, theGlobalVariables);
  output.slicingDirections.AddOnTop(theDirection);
  //std::cout <<output.ElementToString(false, true);
  output.Refine(theGlobalVariables);
  //std::cout <<output.ElementToString(false, true);
}

void Lattice::ApplyLinearMap(Matrix<Rational> & theMap, Lattice& output)
{ Vectors<Rational> tempRoots;
  tempRoots.AssignMatrixRows(this->basisRationalForm);
  theMap.ActOnVectorsColumn(tempRoots);
  output.MakeFromRoots(tempRoots);
}

std::string ConeLatticeAndShiftMaxComputation::ElementToString
  (FormatExpressions* theFormat)
{ std::stringstream out;
  out << "<hr>Resulting lattice: " << this->theFinalRougherLattice.ElementToString(true, false) << "<hr><hr>";
/*  if (this->complexStartingPerRepresentative.size>0)
  { out << "<hr> Non-refined complex per representative:<br>\n ";
    for (int i=0; i<this->complexStartingPerRepresentative.size; i++)
    { out << "Lattice+shift="  << this->theFinalRepresentatives[i].ElementToString() << " + " << this->theFinalRougherLattice.ElementToString();
      out << "<br>\n" << this->complexStartingPerRepresentative[i].ElementToString(false, true);
      out << "the function we need to max: " << this->LPtoMaximizeSmallerDim[i].ElementToString();
    }
  }*/
  out << "<hr><hr>Cones not processed(number of cones " << this->theConesLargerDim.size << "):\n<hr>\n";
  DrawingVariables theDrawingVariables;
  Polynomial<Rational>  tempP;
  for (int i=0; i<this->theConesLargerDim.size; i++)
  { out << "";// << this->theConesLargerDim[i].ElementToString(theFormat);
    //out << "<br>" << this->LPtoMaximizeLargerDim[i].ElementToString();
    theDrawingVariables.theBuffer.init();
    out << "<br>" << this->theConesLargerDim[i].theProjectivizedCone.DrawMeToHtmlLastCoordAffine(theDrawingVariables, *theFormat);
    out << "<br>over " << this->theConesLargerDim[i].theShift.ElementToString() << " + " << this->theConesLargerDim[i].theLattice.ElementToString();
    tempP.MakeLinPolyFromRootLastCoordConst(this->LPtoMaximizeLargerDim[i]);
    out << "<br>the function we have maxed, as a function of the remaining variables, is: " << tempP.ElementToString(theFormat) << "<hr><hr>";
  }
  if (this->theConesSmallerDim.size>0)
  { out << "<br>Cones processed: <br>";
    for (int i=0; i<this->theConesSmallerDim.size; i++)
    { out << this->theConesSmallerDim[i].ElementToString(*theFormat);
      //out << "<br>" << this->LPtoMaximizeSmallerDim[i].ElementToString();
      theDrawingVariables.theBuffer.init();
      out << this->theConesSmallerDim[i].theProjectivizedCone.DrawMeToHtmlLastCoordAffine(theDrawingVariables, *theFormat);
    }
  }
  return out.str();
}

void ConeLatticeAndShiftMaxComputation::FindExtremaParametricStep3
    (Controller& thePauseController, GlobalVariables& theGlobalVariables)
{ this->theFinalRougherLattice=this->theConesLargerDim[0].theLattice;
  theGlobalVariables.theIndicatorVariables.ProgressReportStringsNeedRefresh=true;
  for (int i=1; i<this->theConesLargerDim.size; i++)
  { this->theFinalRougherLattice.IntersectWith(this->theConesLargerDim[i].theLattice);
    std::stringstream tempStream;
    tempStream << "intersecing lattice " << i+1 << " out of " << this->theConesLargerDim.size;
    theGlobalVariables.theIndicatorVariables.ProgressReportStrings[0]=tempStream.str();
    theGlobalVariables.MakeReport();
  }
  this->theFinalRepresentatives.size=0;
  Vectors<Rational> tempRoots, tempRoots2;
  tempRoots2.SetSize(1);
  for (int i=0; i<this->theConesLargerDim.size; i++)
  { tempRoots2[0]=this->theConesLargerDim[i].theShift;
    this->theConesLargerDim[i].theLattice.GetAllRepresentativesProjectingDownTo(this->theFinalRougherLattice, tempRoots2, tempRoots);
    this->theFinalRepresentatives.AddOnTopNoRepetition(tempRoots);
    std::stringstream tempStream;
    tempStream << "Computing representative " << i+1 << " out of " << this->theConesLargerDim.size;
    tempStream << "\nSo far " << this->theFinalRepresentatives.size << " found.";
    theGlobalVariables.theIndicatorVariables.ProgressReportStrings[1]=tempStream.str();
    theGlobalVariables.MakeReport();
  }
  this->complexStartingPerRepresentative.SetSize(this->theFinalRepresentatives.size);
  this->startingLPtoMaximize.SetSize(this->theFinalRepresentatives.size);
  this->finalMaxima.SetSize(this->theFinalRepresentatives.size);
  Vector<Rational> tempRoot;
  for (int i=0; i<this->theFinalRepresentatives.size; i++)
    for (int j=0; j<this->theConesLargerDim.size; j++)
    { tempRoot=this->theFinalRepresentatives[i];
      this->theConesLargerDim[j].theLattice.ReduceVector(tempRoot);
      if (tempRoot==this->theConesLargerDim[j].theShift)
      { this->complexStartingPerRepresentative[i].AddOnTop(this->theConesLargerDim[j].theProjectivizedCone);
        this->startingLPtoMaximize[i].AddOnTop(this->LPtoMaximizeLargerDim[j]);
      }
    }
}

/*void ConeLatticeAndShiftMaxComputation::FindExtremaParametricStep2TrimChamberForMultOne
    (Controller& thePauseController, GlobalVariables& theGlobalVariables)
{ Cone trimmedCone;
  Vectors<Rational> tempRoots;
  Vector<Rational> multFreeWall;
  int startingNumCones=this->theConesLargerDim.size;
  int numKilledCones=0;
  for (int i=0; i<this->theConesLargerDim.size; i++)
  { trimmedCone.Normals=this->theConesLargerDim[i].theProjectivizedCone.Normals;
    multFreeWall=this->LPtoMaximizeLargerDim[i];
    multFreeWall.Minus();
    *multFreeWall.LastObject()+=1;
    trimmedCone.Normals.AddOnTop(multFreeWall);
    trimmedCone.CreateFromNormals(trimmedCone.Normals, theGlobalVariables);
    if (!trimmedCone.flagIsTheZeroCone)
      this->theConesLargerDim[i].theProjectivizedCone=trimmedCone;
    else
    { this->theConesLargerDim.PopIndexSwapWithLast(i);
      this->LPtoMaximizeLargerDim.PopIndexSwapWithLast(i);
      i--;
      numKilledCones++;
    }
    std::stringstream tempStream;
    tempStream << "Processed " << i+numKilledCones << " out of " << startingNumCones;
    tempStream << "\nKilled " << numKilledCones << " cones so far";
    theGlobalVariables.theIndicatorVariables.ProgressReportStrings[2]=tempStream.str();
    theGlobalVariables.MakeReport();
  }
}
*/

void ConeLatticeAndShiftMaxComputation::FindExtremaParametricStep4
    (Controller& thePauseController, GlobalVariables& theGlobalVariables)
{ this->complexRefinedPerRepresentative.SetSize(this->theFinalRepresentatives.size);
  this->theMaximaCandidates.SetSize(this->theFinalRepresentatives.size);
  for (int i=0; i<this->theFinalRepresentatives.size; i++)
  { ConeComplex& currentComplex= this->complexRefinedPerRepresentative[i];
    currentComplex.initFromCones(this->complexStartingPerRepresentative[i], true, theGlobalVariables);
    std::stringstream tempStream;
    tempStream << "Processing representative " << i+1 << " out of " << this->theFinalRepresentatives.size;
    theGlobalVariables.theIndicatorVariables.ProgressReportStrings[0]=tempStream.str();
    theGlobalVariables.MakeReport();
    currentComplex.Refine(theGlobalVariables);
    this->theMaximaCandidates[i].SetSize(currentComplex.size);
    for (int j=0; j<currentComplex.size; j++)
      for (int k=0; k<this->complexStartingPerRepresentative[k].size; k++)
        if (this->complexStartingPerRepresentative[i][k].IsInCone(currentComplex[j].GetInternalPoint()))
          this->theMaximaCandidates[i][j].AddOnTopNoRepetition(this->startingLPtoMaximize[i][k]);
  }
}

void ConeLatticeAndShiftMaxComputation::FindExtremaParametricStep5
    (Controller& thePauseController, GlobalVariables& theGlobalVariables)
{ this->finalMaximaChambers.SetSize(this->theFinalRepresentatives.size);
  this->finalMaximaChambersIndicesMaxFunctions.SetSize(this->theFinalRepresentatives.size);
  for (int i=0; i<1; i++ )//this->theFinalRepresentatives.size; i++)
  { this->finalMaximaChambers[i].SetSize(this->complexRefinedPerRepresentative[i].size);
    this->theFinalRepresentatives[i].SetSize(this->complexRefinedPerRepresentative[i].size);
    for(int j=0; j<1; j++)//this->complexRefinedPerRepresentative[i].size; j++)
    { Cone& currentCone=this->complexRefinedPerRepresentative[i][j];
      this->finalMaximaChambers[i][j].init();
      this->finalMaximaChambers[i][j]
      .findMaxLFOverConeProjective
      (currentCone, this->theMaximaCandidates[i][j],
      this->finalMaximaChambersIndicesMaxFunctions[i][j], theGlobalVariables);
     // tempComplex.a
    }
  }
}

void ConeLatticeAndShiftMaxComputation::FindExtremaParametricStep1
    (Controller& thePauseController, bool assumeNewConesHaveSufficientlyManyProjectiveVertices, GlobalVariables& theGlobalVariables)
{ //std::cout << "<hr>starting complex: " << this->ElementToString();
  FormatExpressions tempFormat;
  for (; this->numProcessedNonParam<this->numNonParaM; this->numProcessedNonParam++)
  { while(this->theConesLargerDim.size>0)
    { ConeLatticeAndShift& currentCLS=*this->theConesLargerDim.LastObject();
      assert(this->LPtoMaximizeLargerDim.LastObject()->size==currentCLS.GetDimAffine()+1);
      if (!this->LPtoMaximizeLargerDim.LastObject()->IsEqualToZero())
        currentCLS.FindExtremaInDirectionOverLatticeOneNonParam
          (*this->LPtoMaximizeLargerDim.LastObject(), this->LPtoMaximizeSmallerDim, this->theConesSmallerDim,
           theGlobalVariables);
      this->theConesLargerDim.size--;
      this->LPtoMaximizeLargerDim.size--;
      thePauseController.SafePointDontCallMeFromDestructors();
      std::stringstream tempStream1, tempStream2, tempStream3;
      tempStream1 << "Processing " << this->numProcessedNonParam+1 << " out of " << this->numNonParaM;
      tempStream2 << "Remaining cones: " << this->theConesLargerDim.size;
      tempStream3 << "Cones smaller dim total: " << this->theConesSmallerDim.size;
      theGlobalVariables.theIndicatorVariables.ProgressReportStrings[0]=tempStream1.str();
      theGlobalVariables.theIndicatorVariables.ProgressReportStrings[1]=tempStream2.str();
      theGlobalVariables.theIndicatorVariables.ProgressReportStrings[2]=tempStream3.str();
      theGlobalVariables.MakeReport();
    }
    //std::cout << "<hr><hr>" << this->ElementToString();
    std::stringstream out;
    theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
    theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
    theGlobalVariables.MakeReport();
    this->LPtoMaximizeLargerDim=this->LPtoMaximizeSmallerDim;
    this->theConesLargerDim=this->theConesSmallerDim;
    this->theConesSmallerDim.size=0;
    this->LPtoMaximizeSmallerDim.size=0;
  }
}

void ConeLatticeAndShift::FindExtremaInDirectionOverLatticeOneNonParamDegenerateCase
(Vector<Rational>& theLPToMaximizeAffine, Vectors<Rational>& outputAppendLPToMaximizeAffine,
 List<ConeLatticeAndShift>& outputAppend, Matrix<Rational> & theProjectionLatticeLevel,
 GlobalVariables& theGlobalVariables)
{ Matrix<Rational>  matVertices;
  matVertices.AssignRootsToRowsOfMatrix(this->theProjectivizedCone.Vertices);
  Vectors<Rational> theNormals;
  matVertices.FindZeroEigenSpace(theNormals, theGlobalVariables);
  Vector<Rational> preferredNormal;
  for (int i=0; i<theNormals.size; i++)
    if (!theNormals[i][0].IsEqualToZero())
    { preferredNormal=theNormals[i];
      break;
    }
  Rational firstCoord=preferredNormal[0];
  preferredNormal.ShiftToTheLeftOnePos();
  preferredNormal/=-firstCoord;
  ConeLatticeAndShift tempCLS;
  Vectors<Rational> newNormals=this->theProjectivizedCone.Normals;
  Rational firstCoordNewNormal, tempRat;
  for (int i=0; i<newNormals.size; i++)
  { firstCoordNewNormal=newNormals[i][0];
    newNormals[i].ShiftToTheLeftOnePos();
    newNormals[i]+=preferredNormal*firstCoordNewNormal;
  }
  //bool tempBool=
  tempCLS.theProjectivizedCone.CreateFromNormals(newNormals, theGlobalVariables);
  tempCLS.theShift=this->theShift;
  tempCLS.theShift.ShiftToTheLeftOnePos();
  this->theLattice.ApplyLinearMap(theProjectionLatticeLevel, tempCLS.theLattice);
  Vector<Rational> tempRoot;
  tempRoot=theLPToMaximizeAffine.GetShiftToTheLeftOnePosition();
  //Vector<Rational> tempRoot2;
  //tempRoot2=preferredNormal;
  tempRoot+=(preferredNormal*theLPToMaximizeAffine[0]);
  if (!tempCLS.theProjectivizedCone.flagIsTheZeroCone)
  { outputAppend.AddOnTop(tempCLS);
    outputAppendLPToMaximizeAffine.AddOnTop(tempRoot);
  }
}

void ConeLatticeAndShift::FindExtremaInDirectionOverLatticeOneNonParam
(Vector<Rational>& theLPToMaximizeAffine, Vectors<Rational>& outputAppendLPToMaximizeAffine,
 List<ConeLatticeAndShift>& outputAppend,
 GlobalVariables& theGlobalVariables)
{ Vector<Rational> direction;
  FormatExpressions theFormat;
  int theDimProjectivized=this->GetDimProjectivized();
  Matrix<Rational>  theProjectionLatticeLevel;
  Matrix<Rational>  theProjectionAffine;
  theProjectionLatticeLevel.init(theDimProjectivized-2, theDimProjectivized-1);
  theProjectionLatticeLevel.NullifyAll();
  for (int i=0; i<theProjectionLatticeLevel.NumRows; i++)
    theProjectionLatticeLevel.elements[i][i+1]=1;
  direction.MakeEi(theDimProjectivized, 0);
  if (!this->theProjectivizedCone.Vertices.LinSpanContainsRoot(direction))
  { this->FindExtremaInDirectionOverLatticeOneNonParamDegenerateCase
      (theLPToMaximizeAffine, outputAppendLPToMaximizeAffine, outputAppend, theProjectionLatticeLevel,
       theGlobalVariables);
    return;
  }
  if (outputAppend.size>=10)
  { std::stringstream tempStream;
    tempStream << "<hr><hr><hr><hr>The bad cone:" << this->theProjectivizedCone.ElementToString(&theFormat);
    theGlobalVariables.theIndicatorVariables.StatusString1=tempStream.str();
    theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
    theGlobalVariables.MakeReport();
  }
  ConeComplex complexBeforeProjection;
  complexBeforeProjection.init();
  complexBeforeProjection.AddNonRefinedChamberOnTopNoRepetition(this->theProjectivizedCone, theGlobalVariables);
  if (direction.ScalarEuclidean(theLPToMaximizeAffine).IsNegative())
    direction.Minus();
  complexBeforeProjection.slicingDirections.AddOnTop(direction);
  complexBeforeProjection.slicingDirections.AddOnTop(-direction);
//  std::cout << "<hr>complex before refining: <br>\n" << complexBeforeProjection.ElementToString(false, true);
  complexBeforeProjection.Refine(theGlobalVariables);
//  std::cout << "<hr>complex before projection: <br>\n" << complexBeforeProjection.ElementToString(false, true);
  Vector<Rational> tempRoot, extraEquation, exitNormalAffine, enteringNormalAffine, exitNormalLatticeLevel, enteringNormalLatticeLevel, exitNormalShiftedAffineProjected;
  Vector<Rational> directionSmallerDim, directionSmallerDimOnLattice;
  Vector<Rational> theShiftReduced=this->theShift;
  this->theLattice.ReduceVector(theShiftReduced);
  Vectors<Rational> exitRepresentatives, exitWallsShifted;
  Vectors<Rational> currentShifts;
  Lattice exitRougherLattice;
  ConeLatticeAndShift tempCLS;
  directionSmallerDim.MakeEi(theDimProjectivized-1, 0);
//  std::cout << "<hr>";
  Vectors<Rational> theNewNormals;
  for (int i=0; i<complexBeforeProjection.size; i++)
  { Cone& currentCone=complexBeforeProjection.TheObjects[i];
    int numNonPerpWalls=0;
    theNewNormals.size=0;
    bool foundEnteringNormal=false;
    bool foundExitNormal=false;
    for (int j=0; j<currentCone.Normals.size; j++)
    { Vector<Rational>& currentNormal=currentCone.Normals.TheObjects[j];
      if (currentNormal[0].IsEqualToZero())
      { tempRoot=currentNormal.GetShiftToTheLeftOnePosition();
        theNewNormals.AddOnTop(tempRoot);
      } else
      { //std::cout << "<hr>";
        //std::cout << "<br>currentWall: " << currentNormal.ElementToString();
        numNonPerpWalls++;
        assert(numNonPerpWalls<3);
        if (!currentNormal.ScalarEuclidean(direction).IsPositive() && !foundExitNormal)
        { theLattice.GetRougherLatticeFromAffineHyperplaneDirectionAndLattice
            (directionSmallerDim, directionSmallerDimOnLattice, theShift, currentNormal, exitRepresentatives,
             exitWallsShifted, exitRougherLattice, theGlobalVariables);
          exitNormalAffine=currentNormal;
          exitNormalLatticeLevel=exitNormalAffine;
          exitNormalLatticeLevel.SetSize(theDimProjectivized-1);
          foundExitNormal=true;
        } else
        { enteringNormalAffine=currentNormal;
          foundEnteringNormal=true;
        }
      }
    }
    exitRougherLattice.ApplyLinearMap(theProjectionLatticeLevel, tempCLS.theLattice);
    for (int j=0; j<exitRepresentatives.size; j++)
    { exitRepresentatives[j]+=theShiftReduced;
      Lattice::GetClosestPointInDirectionOfTheNormalToAffineWallMovingIntegralStepsInDirection
        (exitRepresentatives[j], exitNormalAffine, directionSmallerDimOnLattice, exitRepresentatives[j]);
    }
    std::cout << "<hr><hr><hr>" << currentCone.ElementToString(false, true, theFormat);
    std::cout << "<br>Entering normal: " << ((foundEnteringNormal) ? enteringNormalAffine.ElementToString() : "not found");
    std::cout << "<br>Exit normal: " << ((foundExitNormal) ? exitNormalAffine.ElementToString() : "not found");
    std::cout << "<br>The shifted lattice representatives: " << exitRepresentatives.ElementToString() << "<br>exitNormalsShiftedAffineProjected";
    assert(theNewNormals.size>0);
    for (int j=0; j<exitRepresentatives.size; j++)
    { tempCLS.theProjectivizedCone.Normals=theNewNormals;
      exitNormalShiftedAffineProjected=exitNormalAffine.GetShiftToTheLeftOnePosition();
      *exitNormalShiftedAffineProjected.LastObject()=-exitNormalLatticeLevel.ScalarEuclidean(exitRepresentatives[j]);
      std::cout << exitNormalShiftedAffineProjected.ElementToString() << ", ";
      if (foundEnteringNormal)
      { extraEquation= enteringNormalAffine.GetShiftToTheLeftOnePosition();
        extraEquation-=(exitNormalShiftedAffineProjected*enteringNormalAffine[0])/exitNormalAffine[0];
        std::cout << "extra equation: " << extraEquation.ElementToString() << ", ";
        tempCLS.theProjectivizedCone.Normals.AddOnTop(extraEquation);
      }
      Rational tempRat=theLPToMaximizeAffine[0];
      tempRoot=theLPToMaximizeAffine.GetShiftToTheLeftOnePosition();
      tempRoot-=exitNormalShiftedAffineProjected*theLPToMaximizeAffine[0]/exitNormalAffine[0];
      outputAppendLPToMaximizeAffine.AddOnTop(tempRoot);
      assert(tempCLS.theProjectivizedCone.Normals.size>0);
      Vectors<Rational> tempTempRoots=tempCLS.theProjectivizedCone.Normals;
      //bool tempBool=
      tempCLS.theProjectivizedCone.CreateFromNormals(tempTempRoots, theGlobalVariables);
      /*if (!tempBool)
      { std::stringstream tempStream;
        tempStream << "The bad starting cone (cone number " << i+1 << "):" << this->ElementToString(theFormat) << "<hr><hr><hr><hr>The bad cone:" << tempCLS.ElementToString(theFormat);
        tempStream << "<br>\n\n" << this->theProjectivizedCone.Normals.ElementToString(false, false, false);
        tempStream << "\n\n<br>\n\n" << complexBeforeProjection.ElementToString(false, true);
        if (!foundEnteringNormal)
          tempStream << "<hr>not found entering normal!!!!!!<hr>";
        if (!foundExitNormal)
          tempStream << "<hr>not found exit normal!!!!!!<hr>";
        Cone tempCone;
        tempCone.CreateFromNormals(tempTempRoots, theGlobalVariables);
        tempStream << "\n\n\n\n<br><br><hr>The bad normals: " << tempTempRoots.ElementToString();
        tempStream << "\n\n\n\n<br><br><hr>The bad normals after creation: " << tempCLS.theProjectivizedCone.Normals.ElementToString();
        theGlobalVariables.theIndicatorVariables.StatusString1=tempStream.str();
        theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
        theGlobalVariables.MakeReport();
        for (int i=0; i<10000000; i++)
          if (i%3==0)
            i=i+2;
        while(true){}
      }*/
      //assert(tempBool);
      //std::cout << tempCLS.theProjectivizedCone.ElementToString(false, true, true, true, theFormat);
      if (!tempCLS.theProjectivizedCone.flagIsTheZeroCone)
      { theProjectionLatticeLevel.ActOnVectorColumn(exitRepresentatives[j], tempCLS.theShift);
        outputAppend.AddOnTop(tempCLS);
        if (tempCLS.GetDimProjectivized()==0)
        { theGlobalVariables.theIndicatorVariables.StatusString1=tempTempRoots.ElementToString();
          theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
          theGlobalVariables.MakeReport();
          while(true) {}
        }
        assert(tempCLS.GetDimProjectivized()==theDimProjectivized-1);
      }
    }
  }
//  std::cout << "<hr><hr><hr>";
//  for (int i=0; i<outputAppend.size; i++)
//  { std::cout << outputAppend[i].theProjectivizedCone.ElementToString(false, true, true, true);
//  }
}

void ConeComplex::GetNewVerticesAppend
  (Cone& myDyingCone, Vector<Rational>& killerNormal, HashedList<Vector<Rational> >& outputVertices, GlobalVariables& theGlobalVariables)
{ int theDimMinusTwo=killerNormal.size-2;
  int theDim=killerNormal.size;
  int numCycles=MathRoutines::NChooseK(myDyingCone.Normals.size, theDimMinusTwo);
  Selection theSel=theGlobalVariables.selGetNewVerticesAppend.GetElement();
  Selection nonPivotPoints=theGlobalVariables.selGetNewVerticesAppend2.GetElement();
  theSel.init(myDyingCone.Normals.size);
//  int IndexLastZeroWithOneBefore, NumOnesAfterLastZeroWithOneBefore;
  Matrix<Rational> & theLinearAlgebra=theGlobalVariables.matGetNewVerticesAppend.GetElement();
  Matrix<Rational>  matEmpty;
  theLinearAlgebra.init(theDimMinusTwo+1, theDim);
  Vector<Rational> tempRoot;
  for (int i=0; i<numCycles; i++)
  { theSel.incrementSelectionFixedCardinality(theDimMinusTwo);//, IndexLastZeroWithOneBefore, NumOnesAfterLastZeroWithOneBefore);
    //int LowestRowUnchanged=theDimMinusTwo-2-NumOnesAfterLastZeroWithOneBefore;
    //for (int j=theDimMinusTwo-1; j>LowestRowUnchanged; j--)
    for (int j=0; j<theDimMinusTwo; j++)
    { Vector<Rational>& currentNormal=myDyingCone.Normals.TheObjects[theSel.elements[j]];
      for (int k=0; k<theDim; k++)
        theLinearAlgebra.elements[j][k]=currentNormal.TheObjects[k];
    }
    for (int k=0; k<theDim; k++)
      theLinearAlgebra.elements[theDimMinusTwo][k]=killerNormal.TheObjects[k];
    theLinearAlgebra.GaussianEliminationByRows(theLinearAlgebra, matEmpty, nonPivotPoints);
    if (nonPivotPoints.CardinalitySelection==1)
    { theLinearAlgebra.NonPivotPointsToEigenVector(nonPivotPoints, tempRoot, (Rational) 1, (Rational) 0);
      tempRoot.ScaleByPositiveRationalToIntegralMinHeight();
      if (myDyingCone.IsInCone(tempRoot))
        outputVertices.AddNoRepetition(tempRoot);
      else
      { tempRoot.Minus();
        if (myDyingCone.IsInCone(tempRoot))
          outputVertices.AddNoRepetition(tempRoot);
      }
    }
  }
}

bool ConeComplex::SplitChamber
(int indexChamberBeingRefined, bool weAreSlicingInDirection, bool weAreChopping, Vector<Rational>& killerNormal, GlobalVariables& theGlobalVariables)
{ Cone& myDyingCone=this->TheObjects[indexChamberBeingRefined];
/*  if (!myDyingCone.flagHasSufficientlyManyVertices)
  { this->flagChambersHaveTooFewVertices=true;
    return false;
  }*/
  Cone& newPlusCone= theGlobalVariables.coneBuffer1NewSplit.GetElement();
  Cone& newMinusCone=theGlobalVariables.coneBuffer2NewSplit.GetElement();
  Matrix<Rational>& bufferMat=theGlobalVariables.matSplitChamberBuff.GetElement();
  Selection& bufferSel=theGlobalVariables.selSplitChamber.GetElement();
  bool needToRecomputeVertices=
  (myDyingCone.Normals.GetRankOfSpanOfElements(bufferMat, bufferSel)<this->GetDim());
//  newPlusCone.flagHasSufficientlyManyVertices=true;
//  newMinusCone.flagHasSufficientlyManyVertices=true;
  newPlusCone.LowestIndexNotCheckedForSlicingInDirection=myDyingCone.LowestIndexNotCheckedForSlicingInDirection;
  newMinusCone.LowestIndexNotCheckedForSlicingInDirection=myDyingCone.LowestIndexNotCheckedForSlicingInDirection;
  newPlusCone.LowestIndexNotCheckedForChopping=myDyingCone.LowestIndexNotCheckedForChopping;
  newMinusCone.LowestIndexNotCheckedForChopping=myDyingCone.LowestIndexNotCheckedForChopping;
  newPlusCone.flagIsTheZeroCone=false;
  newMinusCone.flagIsTheZeroCone=false;
  if (weAreChopping)
  { newPlusCone.LowestIndexNotCheckedForChopping++;
    newMinusCone.LowestIndexNotCheckedForChopping++;
  }
  newPlusCone.Vertices.size=0; newPlusCone.Normals.size=0;
  newMinusCone.Vertices.size=0; newMinusCone.Normals.size=0;
  HashedList<Vector<Rational> >& ZeroVertices=theGlobalVariables.hashedRootsNewChamberSplit.GetElement();
  ZeroVertices.Clear();
  Rational tempRat;
  for (int i=0; i<myDyingCone.Vertices.size; i++)
  { tempRat=Vector<Rational>::ScalarEuclidean(killerNormal, myDyingCone.Vertices.TheObjects[i]);
    if (tempRat.IsPositive())
      newPlusCone.Vertices.AddOnTop(myDyingCone.Vertices.TheObjects[i]);
    if (tempRat.IsEqualToZero())
      ZeroVertices.AddNoRepetition(myDyingCone.Vertices.TheObjects[i]);
    if (tempRat.IsNegative())
      newMinusCone.Vertices.AddOnTop(myDyingCone.Vertices.TheObjects[i]);
  }
  if (newPlusCone.Vertices.size==0 || newMinusCone.Vertices.size==0)
    return false;
  this->GetNewVerticesAppend(myDyingCone, killerNormal, ZeroVertices, theGlobalVariables);
  for (int i=0; i<myDyingCone.Normals.size; i++)
  { if (newPlusCone.Vertices.HasAnElementPerpendicularTo(myDyingCone.Normals[i]))
      newPlusCone.Normals.AddOnTop(myDyingCone.Normals[i]);
    if (newMinusCone.Vertices.HasAnElementPerpendicularTo(myDyingCone.Normals[i]))
      newMinusCone.Normals.AddOnTop(myDyingCone.Normals[i]);
  }
  newPlusCone.Normals.AddOnTop(killerNormal);
  newMinusCone.Normals.AddOnTop(-killerNormal);
  newPlusCone.Vertices.AddListOnTop(ZeroVertices);
  newMinusCone.Vertices.AddListOnTop(ZeroVertices);
/*  Cone tempCone;
  tempCone.CreateFromNormals(newPlusCone.Normals, theGlobalVariables);
  assert(tempCone.Vertices.size==newPlusCone.Vertices.size);
  tempCone.CreateFromNormals(newMinusCone.Normals, theGlobalVariables);
  assert(tempCone.Vertices.size==newMinusCone.Vertices.size);
*/
  this->PopChamberSwapWithLast(indexChamberBeingRefined);
  if (needToRecomputeVertices)
  { newPlusCone.CreateFromNormals(newPlusCone.Normals, theGlobalVariables);
    newMinusCone.CreateFromNormals(newMinusCone.Normals, theGlobalVariables);
  }
  this->AddNonRefinedChamberOnTopNoRepetition(newPlusCone, theGlobalVariables);
  this->AddNonRefinedChamberOnTopNoRepetition(newMinusCone, theGlobalVariables);
  return true;
}

void ConeComplex::PopChamberSwapWithLast(int index)
{ this->PopIndexSwapWithLast(index);
}

bool Cone::MakeConvexHullOfMeAnd(const Cone& other, GlobalVariables& theGlobalVariables)
{ if (this->IsInCone(other.Vertices))
    return false;
  if (other.flagIsTheZeroCone)
    return false;
  if (this->flagIsTheZeroCone)
  { this->operator=(other);
    return true;
  }
  Vectors<Rational> newVertices;
  newVertices.AddListOnTop(other.Vertices);
  newVertices.AddListOnTop(this->Vertices);
  this->CreateFromVertices(newVertices, theGlobalVariables);
  return true;
}

bool ConeComplex::AddNonRefinedChamberOnTopNoRepetition
(Cone& newCone, GlobalVariables& theGlobalVariables)
{ newCone.Normals.QuickSortAscending();
  this->ConvexHull.MakeConvexHullOfMeAnd(newCone, theGlobalVariables);
  return this->AddNoRepetition(newCone);
}

void ConeComplex::RefineOneStep(GlobalVariables& theGlobalVariables)
{ if (this->indexLowestNonRefinedChamber>=this->size)
    return;
  Cone& currentCone=this->TheObjects[this->indexLowestNonRefinedChamber];
  for (; currentCone.LowestIndexNotCheckedForChopping<this->splittingNormals.size; currentCone.LowestIndexNotCheckedForChopping++)
   if (this->SplitChamber(this->indexLowestNonRefinedChamber, false, true, this->splittingNormals.TheObjects[currentCone.LowestIndexNotCheckedForChopping], theGlobalVariables))
      return;
  Vector<Rational> currentNewWall;
  for (; currentCone.LowestIndexNotCheckedForSlicingInDirection< this->slicingDirections.size; currentCone.LowestIndexNotCheckedForSlicingInDirection++)
    for (int i=0; i<currentCone.Normals.size; i++)
      if (this->slicingDirections.TheObjects[currentCone.LowestIndexNotCheckedForSlicingInDirection].ScalarEuclidean(currentCone.Normals.TheObjects[i]).IsPositive())
        for (int j=i+1; j<currentCone.Normals.size; j++)
          if (this->slicingDirections.TheObjects[currentCone.LowestIndexNotCheckedForSlicingInDirection].ScalarEuclidean(currentCone.Normals.TheObjects[j]).IsPositive())
            if (currentCone.ProduceNormalFromTwoNormalsAndSlicingDirection(this->slicingDirections.TheObjects[currentCone.LowestIndexNotCheckedForSlicingInDirection], currentCone.Normals.TheObjects[i], currentCone.Normals.TheObjects[j], currentNewWall))
              if (this->SplitChamber(this->indexLowestNonRefinedChamber, true, false, currentNewWall, theGlobalVariables))
                return;
  this->indexLowestNonRefinedChamber++;
}

void ConeComplex::InitFromDirectionsAndRefine(Vectors<Rational>& inputVectors, GlobalVariables& theGlobalVariables)
{ this->init();
  Cone startingCone;
  startingCone.CreateFromVertices(inputVectors, theGlobalVariables);
  this->AddNonRefinedChamberOnTopNoRepetition(startingCone, theGlobalVariables);
  this->slicingDirections.AddListOnTop(inputVectors);
  this->Refine(theGlobalVariables);
}

void ConeComplex::Sort(GlobalVariables& theGlobalVariables)
{ List<Cone> tempList;
  tempList=*this;
  tempList.QuickSortAscending();
  this->Clear();
  for (int i=0; i<tempList.size; i++)
    this->AddOnTop(tempList[i]);
}

void ConeComplex::RefineAndSort(GlobalVariables& theGlobalVariables)
{ this->Refine(theGlobalVariables);
  this->Sort(theGlobalVariables);
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

void Cone::ComputeVerticesFromNormalsNoFakeVertices
(GlobalVariables& theGlobalVariables)
{ this->Vertices.size=0;
  Selection theSel, nonPivotPoints;
  for (int i=0; i<this->Normals.size; i++)
    this->Normals.TheObjects[i].ScaleByPositiveRationalToIntegralMinHeight();
  int theDim=this->Normals.TheObjects[0].size;
  theSel.init(this->Normals.size);
  int numCycles=theSel.GetNumCombinationsFixedCardinality(theDim-1);
  if (theDim==1)
  { numCycles=0;
    bool foundNegative=false;
    bool foundPositive=false;
    for (int i=0; i<this->Normals.size; i++)
    { if(this->Normals[i].IsPositiveOrZero())
        foundPositive=true;
      if (this->Normals[i].IsNegativeOrZero())
        foundNegative=true;
    }
    if (foundNegative xor foundPositive)
    { this->Vertices.SetSizeMakeMatrix(1,1);
      if (foundNegative)
        this->Vertices[0][0]=-1;
      else
        this->Vertices[0][0]=1;
    }
    return;
  }
  Matrix<Rational>& theMat=theGlobalVariables.matComputeNormalFromSelection.GetElement();
  Matrix<Rational> emptyMat;
  Vector<Rational> tempRoot;
  theMat.init(theDim-1, theDim);
  for (int i=0; i<numCycles; i++)
  { theSel.incrementSelectionFixedCardinality(theDim-1);
    for (int j=0; j<theSel.CardinalitySelection; j++)
      for (int k=0; k<theDim; k++)
        theMat.elements[j][k]=this->Normals.TheObjects[theSel.elements[j]].TheObjects[k];
    theMat.GaussianEliminationByRows(theMat, emptyMat, nonPivotPoints);
    if (nonPivotPoints.CardinalitySelection==1)
    { theMat.NonPivotPointsToEigenVector(nonPivotPoints, tempRoot);
      bool tempBool=this->IsInCone(tempRoot);
      if (!tempBool)
      { tempRoot.Minus();
        tempBool=this->IsInCone(tempRoot);
      }
      if (tempBool)
      { tempRoot.ScaleByPositiveRationalToIntegralMinHeight();
        this->Vertices.AddOnTopNoRepetition(tempRoot);
      }
    }
  }
}

bool Cone::EliminateFakeNormalsUsingVertices
(int theDiM, int numAddedFakeWalls, GlobalVariables& theGlobalVariables)
{ if(this->Vertices.size==0)
  { this->flagIsTheZeroCone=true;
    this->Normals.SetSize(0);
    return false;
  }
  Vectors<Rational>& verticesOnWall=theGlobalVariables.rootsEliminateFakeNormalsUsingVertices.GetElement();
  if (numAddedFakeWalls!=0)
  { //we modify the normals so that they lie in the subspace spanned by the vertices
    Matrix<Rational> tempMat, matNormals, gramMatrixInverted;
    tempMat.AssignRootsToRowsOfMatrix(this->Vertices);
    Vectors<Rational> NormalsToSubspace;
    tempMat.FindZeroEigenSpace(NormalsToSubspace, theGlobalVariables);
    if (NormalsToSubspace.size>0)
    { matNormals.AssignRootsToRowsOfMatrix(NormalsToSubspace);
//      std::cout << "<br>Normals to the subspace spanned by the vertices: " << NormalsToSubspace.ElementToString();
      gramMatrixInverted=matNormals;
      gramMatrixInverted.Transpose();
      gramMatrixInverted.MultiplyOnTheLeft(matNormals);
      gramMatrixInverted.Invert(theGlobalVariables);
      Vector<Rational> tempRoot;
      for (int i=0; i<this->Normals.size; i++)
      { matNormals.ActOnVectorColumn(this->Normals[i], tempRoot);
        gramMatrixInverted.ActOnVectorColumn(tempRoot);
        for (int j=0; j<tempRoot.size; j++)
          this->Normals[i]-=NormalsToSubspace[j]*tempRoot[j];
        this->Normals[i].ScaleByPositiveRationalToIntegralMinHeight();
        if (this->Normals[i].IsEqualToZero())
        { this->Normals.PopIndexSwapWithLast(i);
          i--;
        }
      }
      //all normals should now lie in the subspace spanned by the vertices
      //add the walls needed to go down to the subspace
      this->Normals.Reserve(this->Normals.size+2*NormalsToSubspace.size);
      for (int i=0; i<NormalsToSubspace.size; i++)
      { NormalsToSubspace[i].ScaleByPositiveRationalToIntegralMinHeight();
        this->Normals.AddOnTop(NormalsToSubspace[i]);
        this->Normals.AddOnTop(-NormalsToSubspace[i]);
      }
    }
  }
  Matrix<Rational>& tempMatX=theGlobalVariables.matEliminateFakeNormalsUsingVertices.GetElement();
  Selection& tempSelX=theGlobalVariables.selEliminateFakeNormalsUsingVertices.GetElement();
  int DesiredRank=this->Vertices.GetRankOfSpanOfElements(tempMatX, tempSelX);
  if (DesiredRank>1)
    for (int i=0; i<this->Normals.size; i++)
    { Vector<Rational>& currentNormal=this->Normals.TheObjects[i];
      verticesOnWall.size=0;
      bool wallIsGood=false;
      for (int j=0; j<this->Vertices.size; j++)
        if (Vector<Rational>::ScalarEuclidean(this->Vertices.TheObjects[j], currentNormal).IsEqualToZero())
        { verticesOnWall.AddOnTop(this->Vertices.TheObjects[j]);
          int theRank=verticesOnWall.GetRankOfSpanOfElements(tempMatX, tempSelX);
          if (theRank< verticesOnWall.size)
            verticesOnWall.PopLastObject();
          else
            if (theRank==DesiredRank-1)
            { wallIsGood=true;
              break;
            }
        }
      if (!wallIsGood)
      { this->Normals.PopIndexSwapWithLast(i);
        i--;
      }
    }
  //eliminate identical normals
  this->Normals.QuickSortAscending();
  int currentUniqueElementIndex=0;
  for (int i=1; i<this->Normals.size; i++)
    if (this->Normals[currentUniqueElementIndex]!=this->Normals[i])
    { currentUniqueElementIndex++;
      this->Normals.SwapTwoIndices(currentUniqueElementIndex, i);
    }
  if (this->Normals.size>0)
    this->Normals.SetSize(currentUniqueElementIndex+1);
  for (int i=0; i<this->Vertices.size; i++)
    if (this->Normals.HasAnElementWithNegativeScalarProduct(this->Vertices[i]))
      assert(false);
  for (int i=0; i<this->Normals.size; i++)
    if (!this->Vertices.HasAnElementWithPositiveScalarProduct(this->Normals.TheObjects[i]))
      return false;
  return numAddedFakeWalls==0;
}

bool Cone::CreateFromVertices(Vectors<Rational>& inputVertices, GlobalVariables& theGlobalVariables)
{ this->LowestIndexNotCheckedForChopping=0;
  this->LowestIndexNotCheckedForSlicingInDirection=0;
 // std::cout << inputVertices.ElementToString();
  this->flagIsTheZeroCone=false;
  if (inputVertices.size<=0)
  { this->Normals.size=0;
    this->Vertices.size=0;
    this->flagIsTheZeroCone=true;
    return false;
  }
  this->Normals.size=0;
  Matrix<Rational>& tempMat=theGlobalVariables.matCreateFromVertices.GetElement();
  Selection& tempSel=theGlobalVariables.selCreateFromVertices.GetElement();
  int rankVerticesSpan=inputVertices.GetRankOfSpanOfElements(tempMat, tempSel);
  int theDim=inputVertices.GetDim();
  Vectors<Rational>& extraVertices=theGlobalVariables.rootsCreateFromVertices.GetElement();
  extraVertices.SetSize(0);
  if (rankVerticesSpan<theDim)
  { Matrix<Rational>  tempMat;
    tempMat.AssignRootsToRowsOfMatrix(inputVertices);
    tempMat.FindZeroEigenSpace(extraVertices);
    for (int i=0; i<extraVertices.size; i++)
    { this->Normals.AddOnTop(extraVertices[i]);
      this->Normals.AddOnTop(-extraVertices[i]);
    }
  }
  int NumCandidates = MathRoutines::NChooseK(inputVertices.size, rankVerticesSpan-1);
  Selection theSelection;
  theSelection.init(inputVertices.size);
  Vector<Rational> normalCandidate;
  for (int i=0; i<NumCandidates; i++)
  { theSelection.incrementSelectionFixedCardinality(rankVerticesSpan-1);
    for (int j=0; j<theSelection.CardinalitySelection; j++)
      extraVertices.AddOnTop(inputVertices[theSelection.elements[j]]);
    if (extraVertices.ComputeNormal(normalCandidate, theDim))
    { bool hasPositive; bool hasNegative;
      hasPositive=false; hasNegative=false;
      for (int j=0; j<inputVertices.size; j++)
      { Rational tempRat= normalCandidate.ScalarEuclidean(inputVertices[j]);
        if (tempRat.IsNegative())
          hasNegative=true;
        if (tempRat.IsPositive())
          hasPositive=true;
        if (hasNegative && hasPositive)
          break;
      }
      normalCandidate.ScaleByPositiveRationalToIntegralMinHeight();
      if ((hasNegative && !hasPositive))
        normalCandidate.Minus();
      if (!(hasNegative && hasPositive))
        this->Normals.AddOnTopNoRepetition(normalCandidate);
    }
    extraVertices.size=theDim-rankVerticesSpan;
  }
//  std::cout << "<br>Candidate normals: " << this->Normals.ElementToString();
  return this->CreateFromNormals(this->Normals, theGlobalVariables);
}

bool Cone::CreateFromNormalS
  (Vectors<Rational>& inputNormals, bool UseWithExtremeMathCautionAssumeConeHasSufficientlyManyProjectiveVertices,
   GlobalVariables& theGlobalVariables)
{ this->flagIsTheZeroCone=false;
  this->LowestIndexNotCheckedForChopping=0;
  this->LowestIndexNotCheckedForSlicingInDirection=0;
  int theDim=1;
  if (inputNormals.size>0)
    theDim=inputNormals[0].size;
  this->Normals=inputNormals;
  for (int i=0; i<this->Normals.size; i++)
    if (this->Normals[i].IsEqualToZero())
    { this->Normals.PopIndexSwapWithLast(i);
      i--;
    }
  int numAddedFakeWalls=0;
  Matrix<Rational>& tempMat=theGlobalVariables.matCreateFromVertices.GetElement();
  Selection& tempSel=theGlobalVariables.selCreateFromVertices.GetElement();
  if (!UseWithExtremeMathCautionAssumeConeHasSufficientlyManyProjectiveVertices)
    for (int i=0; i<theDim && this->Normals.GetRankOfSpanOfElements(tempMat, tempSel)<theDim; i++)
    { Vector<Rational> tempRoot;
      tempRoot.MakeEi(theDim, i);
      if(!this->Normals.LinSpanContainsRoot(tempRoot, tempMat, tempSel))
      { numAddedFakeWalls++;
        this->Normals.AddOnTop(tempRoot);
      }
    }
//  std::cout << "<br>Normals (" << inputNormals.size << " input " << numAddedFakeWalls << " fake): " << this->Normals.ElementToString();
  this->ComputeVerticesFromNormalsNoFakeVertices(theGlobalVariables);
//  std::cout << "<br>Vertices before adding minus vertices: " << this->Vertices.ElementToString();
  if (numAddedFakeWalls>0)
  { this->Normals.SetSize(this->Normals.size-numAddedFakeWalls);
    Vector<Rational> tempRoot;
    int originalSize=this->Vertices.size;
    for (int i=0; i<originalSize; i++)
    { tempRoot=-this->Vertices[i];
      if (this->IsInCone(tempRoot))
        this->Vertices.AddOnTopNoRepetition(tempRoot);
    }
  }
//  std::cout << "<br>Vertices: " << this->Vertices.ElementToString();
  return this->EliminateFakeNormalsUsingVertices(theDim, numAddedFakeWalls, theGlobalVariables);
}

void ConeComplex::initFromCones
(List<Cone>& NormalsOfCones, bool AssumeConesHaveSufficientlyManyProjectiveVertices, GlobalVariables& theGlobalVariables)
{ List<Vectors<Rational> > tempRoots;
  tempRoots.SetSize(NormalsOfCones.size);
  for (int i=0; i<NormalsOfCones.size; i++)
    tempRoots[i]=NormalsOfCones[i].Normals;
  this->initFromCones(tempRoots, AssumeConesHaveSufficientlyManyProjectiveVertices, theGlobalVariables);
}

void ConeComplex::initFromCones
(List<Vectors<Rational> >& NormalsOfCones, bool UseWithExtremeMathCautionAssumeConeHasSufficientlyManyProjectiveVertices, GlobalVariables& theGlobalVariables)
{ Cone tempCone;
  this->Clear();
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.theIndicatorVariables.StatusString1=NormalsOfCones.ElementToStringGeneric();
  theGlobalVariables.MakeReport();
  theGlobalVariables.MakeReport();
//  for (int i=0; i<10000000; i++){int j=i*i*i;}
  for (int i=0; i<NormalsOfCones.size; i++)
  { if (tempCone.CreateFromNormalS(NormalsOfCones.TheObjects[i], UseWithExtremeMathCautionAssumeConeHasSufficientlyManyProjectiveVertices, theGlobalVariables))
      this->AddNonRefinedChamberOnTopNoRepetition(tempCone, theGlobalVariables);
    std::stringstream out;
    out << "Initializing cone " << i+1 << " out of " << NormalsOfCones.size;
    theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
    theGlobalVariables.MakeReport();
  }
  Vector<Rational> tempRoot;
  this->splittingNormals.Clear();
  for (int i=0; i<this->size; i++)
    for (int j=0; j<this->TheObjects[i].Normals.size; j++)
    { tempRoot=this->TheObjects[i].Normals.TheObjects[j];
      tempRoot.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
      this->splittingNormals.AddNoRepetition(tempRoot);
      std::stringstream out;
      out << "Extracting walls from cone " << i+1 << " out of " << this->size << " total distinct chambers.";
      out << "\nProcessed " << j+1 << " out of " << this->TheObjects[i].Normals.size << " walls of the current chamber.";
      out << "\nTotal # of distinct walls found: " << this->splittingNormals.size;
      theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
      theGlobalVariables.MakeReport();
    }
}

std::string Cone::ElementToString(bool useLatex, bool useHtml, bool PrepareMathReport, bool lastVarIsConstant, FormatExpressions& theFormat)
{ std::stringstream out;
  if (this->flagIsTheZeroCone)
    out << "The cone is the zero cone.";
  else if(this->Normals.size==0)
    out << "The cone is the entire space";
  if (!PrepareMathReport)
  { out << "Index next wall to refine by: " << this->LowestIndexNotCheckedForChopping << "\n";
    if (useHtml)
      out << "<br>";
    out << "\nIndex next direction to slice in: " << this->LowestIndexNotCheckedForSlicingInDirection << "\n";
  }
  if (useHtml)
    out << "<br>";
  out << "Normals:\n";
  if (useHtml)
    out << "<br>";
  if (useLatex)
    out << "\\[";
  out << this->Normals.ElementsToInequalitiesString(useLatex, useHtml, lastVarIsConstant, theFormat);
  if (useLatex)
    out << "\\]";
  out << "\nProjectivized Vertices: " << this->Vertices.ElementToString();
  if (useHtml)
    out << "<br>";
  if (this->Vertices.size>0)
    out << "\nInternal point: " << this->GetInternalPoint().ElementToString();
  return out.str();
}

std::string ConeComplex::ElementToString(bool useLatex, bool useHtml)
{ std::stringstream out;
  FormatExpressions theFormat;
  out << "Number of chambers: " << this->size;
  if (useHtml)
    out << "<br>";
  out  << " Next non-refined chamber: " << this->indexLowestNonRefinedChamber+1;
  if (useHtml)
    out << "<br>";
  out << "Normals of walls to refine by: ";
  Vectors<Rational> tempRoots;
  tempRoots.CopyFromBase(this->splittingNormals);
  out << tempRoots.ElementToString(useLatex, useHtml, false);
  if (this->slicingDirections.size>0)
  { if (useHtml)
      out << "<br>\n";
    out << " Directions to slice along: " << this->slicingDirections.ElementToString();
  }
  for (int i=0; i<this->size; i++)
  { if (useHtml)
      out << "<hr>";
    out << "\n\n\nChamber " << i+1 << ":\n";
    if (useHtml)
      out << "<br>";
    out << this->TheObjects[i].ElementToString(useLatex, useHtml, theFormat) << "\n\n\n";
  }
  return out.str();
}

int ParserNode::EvaluateGroebner
(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ List<Polynomial<Rational> > inputBasis, outputGroebner;
  theNode.impliedNumVars= theNode.GetMaxImpliedNumVarsChildren();
  for (int i=0; i<theArgumentList.size; i++)
  { Polynomial<Rational> & currentPoly=theNode.owner->TheObjects[theArgumentList[i]].polyValue.GetElement();
    currentPoly.SetNumVariablesSubDeletedVarsByOne(theNode.impliedNumVars);
    outputGroebner.AddOnTop(currentPoly);
  }
  inputBasis=outputGroebner;
  Polynomial<Rational>  buffer1, buffer2, buffer3, buffer4;
  MonomialP bufferMon1, bufferMon2;
  RationalFunction::TransformToReducedGroebnerBasis(outputGroebner, buffer1, buffer2, buffer3, buffer4, bufferMon1, bufferMon2, & theGlobalVariables);
  std::stringstream out;
  out << "<br>Starting basis: ";
  std::stringstream out1, out2;
  FormatExpressions theFormat;
  for(int i=0; i<inputBasis.size; i++)
    out1 << inputBasis.TheObjects[i].ElementToString(&theFormat) << ", ";
  out << CGI::GetHtmlMathDivFromLatexAddBeginARCL(out1.str());
  out << "<br>Reduced Groebner basis:";
  for(int i=0; i<outputGroebner.size; i++)
    out2 << outputGroebner.TheObjects[i].ElementToString(&theFormat) << ", ";
  out << CGI::GetHtmlMathDivFromLatexAddBeginARCL(out2.str());
  theNode.ExpressionType=theNode.typeString;
  theNode.outputString= out.str();
  return theNode.errorNoError;
}

void RationalFunction::GetRelations
  ( List<Polynomial<Rational> >& theGenerators, GlobalVariables& theGlobalVariables
   )
{ if (theGenerators.size==0)
    return;
  List<Polynomial<Rational> > theGroebnerBasis;
  theGroebnerBasis=theGenerators;
  int numStartingGenerators=theGenerators.size;
  int numStartingVariables= theGenerators.TheObjects[0].NumVars;
  Polynomial<Rational>  tempP;
  FormatExpressions tempFormat;
  for (int i=0; i<numStartingGenerators; i++)
  { Polynomial<Rational> & currentPoly=theGroebnerBasis.TheObjects[i];
    currentPoly.SetNumVariablesSubDeletedVarsByOne(numStartingVariables+numStartingGenerators);
    tempP.MakeNVarDegOnePoly(numStartingVariables+numStartingGenerators, i+numStartingVariables, (Rational) -1);
    currentPoly+=tempP;
//  std::cout << currentPoly.ElementToString(false, tempFormat) << "<br>";
  }
  Polynomial<Rational>  buffer1, buffer2, buffer3, buffer4;
  MonomialP bufferMon1, bufferMon2;
  RationalFunction::TransformToReducedGroebnerBasis
  (
   theGroebnerBasis, buffer1, buffer2, buffer3, buffer4, bufferMon1, bufferMon2,
   & MonomialP::LeftIsGEQLexicographicLastVariableWeakest, & theGlobalVariables
  );
//  std::cout << "<br>the ending generators:<br> ";
//  for (int i=0; i<theGroebnerBasis.size; i++)
//  { std::cout << theGroebnerBasis.TheObjects[i].ElementToString(false, tempFormat) << "<br>";
//  }
  theGenerators.Reserve(theGroebnerBasis.size);
  theGenerators.size=0;
  for (int i=0; i<theGroebnerBasis.size; i++)
  { Polynomial<Rational> & currentPoly= theGroebnerBasis.TheObjects[i];
    bool bad=false;
    for (int j=0; j<numStartingVariables; j++)
      if(currentPoly.GetMaxPowerOfVariableIndex(j)>0)
      { bad=true;
        break;
      }
    if (!bad)
     theGenerators.AddOnTop(currentPoly);
  }
}

int ParserNode::EvaluateRelations
(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ List<Polynomial<Rational> > inputBasis, outputRelations;
  theNode.impliedNumVars= theNode.GetMaxImpliedNumVarsChildren();
  for (int i=0; i<theArgumentList.size; i++)
  { Polynomial<Rational> & currentPoly=theNode.owner->TheObjects[theArgumentList[i]].polyValue.GetElement();
    currentPoly.SetNumVariablesSubDeletedVarsByOne(theNode.impliedNumVars);
    outputRelations.AddOnTop(currentPoly);
  }
  inputBasis=outputRelations;
  RationalFunction::GetRelations(outputRelations, theGlobalVariables);
  std::stringstream out;
  out << "<br>Starting elements:";
  FormatExpressions theFormat;
  std::stringstream out3;
  for(int i=0; i<inputBasis.size; i++)
    out3 << " u_{" << i+1+theNode.impliedNumVars << "}:=" << inputBasis[i].ElementToString(&theFormat) << ", ";
  out << CGI::GetHtmlMathSpanFromLatexFormula(out3.str());
  out << "<br>Resulting relations:";
  std::stringstream out2;
  theFormat.polyDefaultLetter="u";
  for(int i=0; i<outputRelations.size; i++)
    out2 << outputRelations.TheObjects[i].ElementToString(&theFormat) << ", ";
  out << CGI::GetHtmlMathSpanFromLatexFormula(out2.str());
  theNode.ExpressionType=theNode.typeString;
  theNode.outputString= out.str();
  return theNode.errorNoError;
}

int ParserNode::EvaluateCone
(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ Vectors<Rational> theNormals;
  int theDim;
  if (!theNode.GetRootsEqualDimNoConversionNoEmptyArgument(theArgumentList, theNormals, theDim))
    return theNode.SetError(theNode.errorDimensionProblem);
  Cone& currentCone=theNode.theCone.GetElement();
  currentCone.CreateFromNormals(theNormals, theGlobalVariables);
  FormatExpressions theFormat;
  theNode.outputString=currentCone.DrawMeToHtmlProjective(theGlobalVariables.theDrawingVariables, theFormat);
  theNode.ExpressionType=theNode.typeCone;
  return theNode.errorNoError;
}

int ParserNode::EvaluateConeFromVertices
(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ Vectors<Rational> theVertices;
  int theDim;
  if (!theNode.GetRootsEqualDimNoConversionNoEmptyArgument(theArgumentList, theVertices, theDim))
    return theNode.SetError(theNode.errorDimensionProblem);
  Cone& currentCone=theNode.theCone.GetElement();
  currentCone.CreateFromVertices(theVertices, theGlobalVariables);
  FormatExpressions theFormat;
  theNode.outputString=currentCone.DrawMeToHtmlProjective(theGlobalVariables.theDrawingVariables, theFormat);
  theNode.ExpressionType=theNode.typeCone;
  return theNode.errorNoError;
}

bool ConeComplex::findMaxLFOverConeProjective
  (Cone& input, List<Polynomial<Rational> >& inputLinPolys, List<int>& outputMaximumOverEeachSubChamber, GlobalVariables& theGlobalVariables)
{ Vectors<Rational> HyperPlanesCorrespondingToLF;
  if (input.Normals.size<1 || inputLinPolys.size<1)
    return false;
  int theDim=input.Normals.TheObjects[0].size;
  HyperPlanesCorrespondingToLF.SetSize(inputLinPolys.size);
  for (int i=0; i<inputLinPolys.size; i++)
  { Polynomial<Rational> & currentPoly=inputLinPolys.TheObjects[i];
    if (currentPoly.TotalDegree()!=1 || theDim!=currentPoly.NumVars)
    { std::cout << "u messed up the dim! The num vars is " << currentPoly.NumVars << " and the dim of the cone is " << theDim;
      return false;
    }
    Vector<Rational>& newWall=HyperPlanesCorrespondingToLF.TheObjects[i];
    newWall.MakeZero(theDim);
    for (int j=0; j<currentPoly.size; j++)
      for (int k=0; k<theDim; k++)
        if (currentPoly[j][k]==1)
        { newWall[k]=currentPoly.theCoeffs[j];
          break;
        }
  }
  return this->findMaxLFOverConeProjective
  (input, HyperPlanesCorrespondingToLF, outputMaximumOverEeachSubChamber, theGlobalVariables);
}

bool ConeComplex::findMaxLFOverConeProjective
  (Cone& input, Vectors<Rational>& inputLFsLastCoordConst,
   List<int>& outputMaximumOverEeachSubChamber,
   GlobalVariables& theGlobalVariables)
{ this->init();
  this->AddNonRefinedChamberOnTopNoRepetition(input, theGlobalVariables);
  Vector<Rational> tempRoot;
  for (int i=0; i<inputLFsLastCoordConst.size; i++)
    for (int j=i+1; j<inputLFsLastCoordConst.size; j++)
    { tempRoot=inputLFsLastCoordConst[i]-inputLFsLastCoordConst[j];
      tempRoot.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
      if (!tempRoot.IsEqualToZero())
        this->splittingNormals.AddNoRepetition(tempRoot);
    }
  std::cout << this->ElementToString(false, true);
  this->Refine(theGlobalVariables);
  outputMaximumOverEeachSubChamber.SetSize(this->size);
  Rational theMax=0;
  for (int i=0; i<this->size; i++)
  { this->TheObjects[i].GetInternalPoint(tempRoot);
    bool isInitialized=false;
    for (int j=0; j<inputLFsLastCoordConst.size; j++)
      if (!isInitialized || tempRoot.ScalarEuclidean(inputLFsLastCoordConst.TheObjects[j], (Rational) 0)>theMax)
      { theMax=tempRoot.ScalarEuclidean(inputLFsLastCoordConst.TheObjects[j], (Rational) 0);
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
  //static Matrix<Rational>  testMatrix;
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

void Lattice::TestGaussianEliminationEuclideanDomainRationals(Matrix<Rational> & output)
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

bool ParserNode::GetRootSRationalDontUseForFunctionArguments
(Vectors<Rational>& output, int& outputDim, GlobalVariables& theGlobalVariables)
{ if (this->ExpressionType!=this->typeArray)
  { if (this->ExpressionType!=this->typeRational)
      return false;
    output.SetSize(1);
    output[0].SetSize(1);
    output[0][0]=this->rationalValue;
    outputDim=1;
    return true;
  }
  output.SetSize(this->children.size);
  for (int i=0; i<this->children.size; i++)
  { ParserNode& currentNode=this->owner->TheObjects[this->children[i]];
    currentNode.GetRootRationalDontUseForFunctionArguments(output[i], theGlobalVariables);
    if (output[i].size!=output[0].size)
      return false;
    outputDim=output[0].size;
  }
  return true;
}

bool ParserNode::GetListRootsEqualSizeEqualDimNoConversionNoEmptyArgument
(List<int>& theArgumentList, List<Vectors<Rational> >& output, int& outputRootsSize, int& outputDim,
 GlobalVariables& theGlobalVariables)
{ ParserNode& firstNode= this->owner->TheObjects[theArgumentList[0]];
  if (firstNode.ExpressionType!=this->typeArray)
  { outputRootsSize=1;
    if (firstNode.ExpressionType!=this->typeRational)
      return false;
    outputDim=1;
    output.SetSize(1);
    output[0].SetSize(1);
    output[0][0].SetSize(1);
    output[0][0][0]=firstNode.rationalValue;
    return true;
  }
  output.SetSize(theArgumentList.size);
  outputRootsSize=firstNode.children.size;
  Vectors<Rational> tempRoots;
  int currentDim;
  for (int i=0; i<theArgumentList.size; i++)
  { ParserNode& currentNode=this->owner->TheObjects[theArgumentList[i]];
    if(!currentNode.GetRootSRationalDontUseForFunctionArguments(tempRoots, currentDim, theGlobalVariables))
      return false;
    if (i==0)
      outputDim=currentDim;
    if (currentDim!=outputDim)
      return false;
  }
  return true;
}

bool ParserNode::GetRootsEqualDimNoConversionNoEmptyArgument
(List<int>& theArgumentList, Vectors<Rational>& output, int& outputDim)
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
{ Vectors<Rational> LatticeBase;
  int theDim=-1;
  if(!theNode.GetRootsEqualDimNoConversionNoEmptyArgument(theArgumentList, LatticeBase, theDim))
    return theNode.SetError(theNode.errorDimensionProblem);
  theNode.theLattice.GetElement().MakeFromRoots(LatticeBase);
  theNode.outputString=theNode.theLattice.GetElement().ElementToString();
  theNode.ExpressionType=theNode.typeLattice;
  return theNode.errorNoError;
}

void Lattice::MakeFromMat(const Matrix<Rational> & input)
{ this->basisRationalForm=input;
  this->basisRationalForm.GetMatrixIntWithDen(this->basis, this->Den);
  this->Reduce();
}

void Lattice::MakeFromRoots(const Vectors<Rational>& input)
{ Matrix<Rational>  tempMat;
  tempMat.AssignRootsToRowsOfMatrix(input);
  tempMat.GetMatrixIntWithDen(this->basis, this->Den);
  this->Reduce();
}

std::string Lattice::ElementToString(bool useHtml, bool useLatex)const
{ std::stringstream out;
  out << "L=<";
  Vectors<Rational> tempRoots;
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
bool Lattice::GetAllRepresentatives
  (const Lattice& rougherLattice, Vectors<Rational>& output)const
{ output.size=0;
  if (this->basis.NumRows!=rougherLattice.basis.NumRows)
    return false;
  List<int> thePeriods;
  Vectors<Rational> thePeriodVectors;
  thePeriods.SetSize(this->basis.NumRows);
  thePeriodVectors.SetSize(this->basis.NumRows);
  Vector<Rational> tempRoot, tempRoot2;
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

bool Lattice::GetClosestPointInDirectionOfTheNormalToAffineWallMovingIntegralStepsInDirection
  (Vector<Rational>& startingPoint, Vector<Rational>& theAffineHyperplane, Vector<Rational>& theDirection, Vector<Rational>& outputPoint)
{ Vector<Rational> theNormal=theAffineHyperplane;
  theNormal.SetSize(theAffineHyperplane.size-1);
  Rational theShift=-(*theAffineHyperplane.LastObject());
  if (theNormal.ScalarEuclidean(startingPoint)==theShift)
  { outputPoint=startingPoint;
    return true;
  }
  if (theDirection.ScalarEuclidean(theNormal).IsEqualToZero())
    return false;
  Rational theMovement= (theShift-startingPoint.ScalarEuclidean(theNormal))/theDirection.ScalarEuclidean(theNormal);
  std::cout << "<br>the movement: " << theMovement.ElementToString() << ", (" << theShift.ElementToString() << " - " << startingPoint.ScalarEuclidean(theNormal).ElementToString() << ")/ " << theDirection.ScalarEuclidean(theNormal).ElementToString() << ", ";
  if (!theMovement.IsInteger())
  { std::cout << "the movement aint integral; ";
    theMovement.AssignFloor();
    if (theDirection.ScalarEuclidean(theNormal).IsPositive())
      theMovement+=1;
  }
  std::cout << "the normal: " << theNormal.ElementToString() <<", the direction: " << theDirection.ElementToString() << ", the shift: " << theShift.ElementToString()
  << ", the movement: " << theMovement.ElementToString() << ", startingPoint: " << startingPoint.ElementToString();
  outputPoint=startingPoint;
  outputPoint+=(theDirection*theMovement);
  std::cout << ", finalPoint: " << outputPoint.ElementToString();
  return true;
}

bool Lattice::GetAllRepresentativesProjectingDownTo
  (const Lattice& rougherLattice, Vectors<Rational>& startingShifts, Vectors<Rational>& output)const
{ Vectors<Rational> tempRepresentatives;
  if (!this->GetAllRepresentatives(rougherLattice, tempRepresentatives))
    return false;
  output.Reserve(startingShifts.size*tempRepresentatives.size);
  output.size=0;
  for (int i=0; i<startingShifts.size; i++)
    for (int j=0; j<tempRepresentatives.size; j++)
      output.AddOnTop(startingShifts.TheObjects[i]+tempRepresentatives[j]);
  return true;
}

void QuasiPolynomial::MakeRougherLattice(const Lattice& latticeToRoughenBy)
{ if (this->AmbientLatticeReduced==latticeToRoughenBy)
    return;
  //std::cout << "roughening " << this->AmbientLatticeReduced.ElementToString() << "by" << latticeToRoughenBy.ElementToString();
  Lattice OldLattice;
  OldLattice=this->AmbientLatticeReduced;
  this->AmbientLatticeReduced.IntersectWith(latticeToRoughenBy);
  Vectors<Rational> representativesQuotientLattice;
  //std::cout << "getting all representatives of " << OldLattice.ElementToString() << "inside" << this->AmbientLatticeReduced.ElementToString();
  //std::cout << "<br><br><br><br>*********<br><br><br><br>";
  OldLattice.GetAllRepresentatives(this->AmbientLatticeReduced, representativesQuotientLattice);
  Vectors<Rational> OldLatticeShifts=this->LatticeShifts;
  List<Polynomial<Rational> > oldValues;
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
  Matrix<Rational>  tempMat;
  tempMat=this->basisRationalForm;
  tempMat.Invert();
  tempMat.Transpose();
  tempMat.GetMatrixIntWithDen(output.basis, output.Den);
  output.Reduce();
}

bool Lattice::FindOnePreimageInLatticeOf
    (const Matrix<Rational> & theLinearMap, const Vectors<Rational>& input, Vectors<Rational>& output, GlobalVariables& theGlobalVariables)
{ Vectors<Rational> thisBasis;
  thisBasis.AssignMatrixRows(this->basisRationalForm);
  theLinearMap.ActOnVectorsColumn(thisBasis, (Rational) 0);
  bool result=input.GetIntegralCoordsInBasisIfTheyExist(thisBasis, output, 1, -1, 0);
  //std::cout << "<hr>acting by " << this->basisRationalForm.ElementToString(true, false) << " on " << output.ElementToString();
  Matrix<Rational> tempMat;
  tempMat=this->basisRationalForm;
  tempMat.Transpose();
  tempMat.ActOnVectorsColumn(output);
  //std::cout << "<br> to get " << output.ElementToString();
  return result;
}

void Lattice::IntersectWithPreimageOfLattice
  (const Matrix<Rational> & theLinearMap, const Lattice& other, GlobalVariables& theGlobalVariables)
{ Vectors<Rational> startingBasis, imageStartingBasis, basisImageIntersection, basisImageIntersectionInCoordsWRTimageStartingBasis, ImageBasisInImageStartingBasisCoords;
  Vectors<Rational> resultNonKernelPart, resultKernelPart, result, tempRoots;
  startingBasis.AssignMatrixRows(this->basisRationalForm);
  //std::cout << "<br>Starting basis: " << startingBasis.ElementToString();
  //std::cout << "<br>The linear map: " << theLinearMap.ElementToString(true, false);
  theLinearMap.ActOnVectorsColumn(startingBasis, imageStartingBasis);
  //std::cout << "<br>Image of starting basis: " << imageStartingBasis.ElementToString();
  Lattice ImageLattice;
  ImageLattice.MakeFromRoots(imageStartingBasis);
  ImageLattice.IntersectWith(other);
  //std::cout << "<br>Image lattice: " << ImageLattice.ElementToString(true, false);
  basisImageIntersection.AssignMatrixRows(ImageLattice.basisRationalForm);
  Vectors<Rational> tempBasisImageIntersection, tempImageStartingBasis, tempImageBasisInImageStartingBasisCoords;
  basisImageIntersection=(tempBasisImageIntersection);
  imageStartingBasis=(tempImageStartingBasis);
  bool tempBool= tempBasisImageIntersection.GetIntegralCoordsInBasisIfTheyExist(tempImageStartingBasis, tempImageBasisInImageStartingBasisCoords, (Rational) 1, (Rational) -1, (Rational) 0);
  ImageBasisInImageStartingBasisCoords=(tempImageBasisInImageStartingBasisCoords);
  assert(tempBool);
  //std::cout << "<br> Basis of image lattice expressed in coordinates with respect to image of starting basis: " << ImageBasisInImageStartingBasisCoords.ElementToString();
  resultNonKernelPart.SetSize(ImageBasisInImageStartingBasisCoords.size);
  for (int i=0; i<resultNonKernelPart.size; i++)
  { Vector<Rational>& currentRoot=resultNonKernelPart.TheObjects[i];
    currentRoot.MakeZero(this->GetDim());
    for (int j=0; j<startingBasis.size; j++)
      currentRoot+=startingBasis.TheObjects[j]*ImageBasisInImageStartingBasisCoords.TheObjects[i].TheObjects[j];
  }
  //std::cout << "<br> Result non-kernel part (representatives): " << resultNonKernelPart.ElementToString();
  Lattice KernelPart;
  KernelPart=*this;
  tempRoots.AssignMatrixRows(theLinearMap);
  KernelPart.IntersectWithLinearSubspaceGivenByNormals(tempRoots);
  //std::cout << "<br>Kernel part of the result: " << KernelPart.ElementToString(true, false);
  result.AssignMatrixRows(KernelPart.basisRationalForm);
  result.AddListOnTop(resultNonKernelPart);
  this->MakeFromRoots(result);
  //std::cout << "<br> And the result is.... " << this->ElementToString(true, false);
}

void Lattice::IntersectWith(const Lattice& other)
{ Vectors<Rational> commonBasis, otherBasis, startBasis;
  Vectors<Rational> bufferVect;
  Matrix<Rational> bufferMat;
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
  Vectors<Rational> thisCommonBasis, otherCommonBasis, thisCommonCoords, otherCommonCoords;
  thisCommonBasis.AssignMatrixRows(thisLatticeIntersected.basisRationalForm);
  otherCommonBasis.AssignMatrixRows(otherLatticeIntersected.basisRationalForm);
  thisCommonBasis.GetCoordsInBasis(commonBasis, thisCommonCoords, bufferVect, bufferMat);
  otherCommonBasis.GetCoordsInBasis(commonBasis, otherCommonCoords, bufferVect, bufferMat);
//  std::cout << "<br>this lattice intersection new coords: " << thisCommonBasis.ElementToString();
//  std::cout << "<br>other lattice intersection new coords: " << otherBasis.ElementToString();
  Lattice thisCommonCoordsLattice, otherCommonCoordsLattice;
  thisCommonCoordsLattice.MakeFromRoots(thisCommonCoords);
  otherCommonCoordsLattice.MakeFromRoots(otherCommonCoords);
//  std::cout << "<br> linear space intersected with this lattice new coords: " << thisCommonCoordsLattice.ElementToString(true, false);
//  std::cout << "<br> linear space intersected with other lattice new coords: " << otherCommonCoordsLattice.ElementToString(true, false);
  thisCommonCoordsLattice.IntersectWithBothOfMaxRank(otherCommonCoordsLattice);
//  std::cout << "<br> intersection lattice new coords: " << thisCommonCoordsLattice.ElementToString(true, false);
  Vectors<Rational> resultBasis;
  resultBasis.SetSize(thisCommonCoordsLattice.basisRationalForm.NumRows);
  for (int i=0; i<resultBasis.size; i++)
  { Vector<Rational>& currentRoot=resultBasis.TheObjects[i];
    currentRoot.MakeZero(this->GetDim());
    for (int j=0; j<thisCommonCoordsLattice.basisRationalForm.NumCols; j++)
      currentRoot+=commonBasis[j]*thisCommonCoordsLattice.basisRationalForm.elements[i][j];
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

std::string QuasiPolynomial::ElementToString(bool useHtml, bool useLatex, FormatExpressions* thePolyFormat)
{ std::stringstream out;
  //if (useHtml)
//  out << "the lattice: " << this->AmbientLatticeReduced.ElementToString(useHtml, useLatex);
//  out << "<br>We have " << this->LatticeShifts.size << " lattice shifts. The polynomial on each lattice shift follows.";
  if (this->LatticeShifts.size==0)
    return "0";
  if (useLatex&& !useHtml)
    out << "\\begin{tabular}{c}";
  if (useLatex&& useHtml)
    out << "\\begin{array}{rcl}&&";
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
    if (!useLatex)
      out << " over ";
    else
      if (useHtml)
        out << " \\mathrm{~over~}";
      else
        out << " over ";
    if (!this->LatticeShifts.TheObjects[i].IsEqualToZero())
      out << this->LatticeShifts.TheObjects[i].ElementToString() << " + ";
    if (useLatex)
    { if (!useHtml)
        out << "$\\Lambda$, \\\\\\hline\n ";
      else
        out << " \\Lambda";
    } else
      out << "L ";
    if (this->LatticeShifts.size>1)
    { if (useHtml & ! useLatex)
        out << "<br>";
      if (useLatex && !useHtml)
        out << "\\\\";
      if (useLatex && useHtml)
        out << "\\\\&&";
      out << "\n\n";
    }
  }
  if (!this->AmbientLatticeReduced.basisRationalForm.IsIdMatrix())
  { if(!useLatex)
      out << ", where L=< ";
    if (useLatex && useHtml)
      out << ", \\mathrm{~where~} \\Lambda=<";
    if (useLatex && !useHtml)
      out << " where $\\Lambda=\\left\\langle\\begin{array}{c}";
    Vectors<Rational> tempRoots;
    tempRoots.AssignMatrixRows(this->AmbientLatticeReduced.basisRationalForm);
    for (int i=0; i<tempRoots.size; i++)
    { out << tempRoots.TheObjects[i].ElementToString();
      if (i!=tempRoots.size-1)
      { out << ", ";
        if (useLatex)
          out << "\\\\";
      }
    }
    if(!useLatex)
      out << " >";
    if (useLatex && useHtml)
      out << " >";
    if (useLatex && !useHtml)
      out << "\\end{array}\\right\\rangle$";
  } else
    if (useLatex)
    { if (!useHtml)
        out << ", where $\\Lambda=\\mathbb{Z}^{" <<  this->GetNumVars() << "}$";
      else
        out << ", \\mathrm{~where~} \\Lambda=\\mathbb{Z}^{" <<  this->GetNumVars() << "}";
    }
    else
      out << "Z^" <<  this->GetNumVars();
  if (useLatex&& !useHtml)
    out << "\\end{tabular}";
  if (useLatex&& useHtml)
    out << "\\end{array}";
  return out.str();
}

bool ParserNode::GetRootInt(Vector<int>& output, GlobalVariables& theGlobalVariables)
{ assert(false);
  return true;
}

bool ParserNode::GetRootRationalFromFunctionArguments
(//List<int>& argumentList,
 Vector<Rational>& output, GlobalVariables& theGlobalVariables)
{ Vectors<Rational> tempRoots;
  int theDim;
  this->GetRootsEqualDimNoConversionNoEmptyArgument(this->children, tempRoots, theDim);
  if (tempRoots.size!=1)
    return false;
  output = tempRoots[0];
  return true;
}

void QuasiPolynomial::MakeFromPolyShiftAndLattice
(const Polynomial<Rational> & inputPoly, const Vector<Rational>& theShift, const Lattice& theLattice, GlobalVariables& theGlobalVariables)
{ this->AmbientLatticeReduced=theLattice;
  this->LatticeShifts.SetSize(1);
  this->LatticeShifts.TheObjects[0]=theShift;
  this->AmbientLatticeReduced.ReduceVector(this->LatticeShifts.TheObjects[0]);
  this->valueOnEachLatticeShift.SetSize(1);
  this->valueOnEachLatticeShift.TheObjects[0]=inputPoly;
}

bool Lattice::ReduceVector(Vector<Rational>& theVector)const
{ Vector<Rational> output;
  assert(theVector.size==this->GetDim());
  Vectors<Rational> basisRoots;
  basisRoots.AssignMatrixRows(this->basisRationalForm);
  //std::cout <<  "the basis: " << basisRoots.ElementToString();
  if (!theVector.GetCoordsInBasiS(basisRoots, output, (Rational) 1, (Rational) 0))
  { std::cout << "oops bad!";
    assert(false);
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

void Lattice::MakeZn(int theDim)
{ this->basisRationalForm.MakeIdMatrix(theDim);
  this->basis.MakeIdMatrix(theDim, (LargeInt) 1, (LargeInt) 0);
  this->Den.MakeOne();
}

void QuasiPolynomial::MakeZeroOverLattice(Lattice& theLattice)
{ this->AmbientLatticeReduced=theLattice;
  this->LatticeShifts.size=0;
  this->valueOnEachLatticeShift.size=0;
}

void QuasiPolynomial::MakeZeroLatTiceZn(int theDim)
{ assert(theDim>0);
  this->AmbientLatticeReduced.MakeZn(theDim);
  this->LatticeShifts.size=0;
  this->valueOnEachLatticeShift.size=0;
}

void partFraction::GetRootsFromDenominator
(partFractions& owner, Vectors<Rational>& output)
{ output.SetSize(this->IndicesNonZeroMults.size);
  for (int i=0; i<this->IndicesNonZeroMults.size; i++)
  { output[i]=owner.startingVectors[this->IndicesNonZeroMults[i]];
    oneFracWithMultiplicitiesAndElongations& current=this->TheObjects[this->IndicesNonZeroMults[i]];
    assert(current.Elongations.size==1);
    output[i]*=current.Elongations[0];
  }
}

void partFraction::ComputePolyCorrespondingToOneMonomial
  (QuasiPolynomial& outputQP, MonomialP& theMon, Vectors<Rational>& normals,
   Lattice& theLattice, GlobalVariables& theGlobalVariables)
{ Polynomial<Rational> tempP, outputPolyPart;
  Rational tempRat2;
  outputPolyPart.MakeConst(theMon.size, 1);
  for (int i=0; i<theMon.size; i++)
  { this->MakePolynomialFromOneNormal(normals[i], theMon, this->TheObjects[this->IndicesNonZeroMults[i]].Multiplicities[0], tempP);
    outputPolyPart*=(tempP);
  }
  outputQP.MakeFromPolyShiftAndLattice(outputPolyPart, theMon, theLattice, theGlobalVariables);
  this->AlreadyAccountedForInGUIDisplay=true;
}

void partFraction::GetVectorPartitionFunction
  (partFractions& owner, Polynomial<LargeInt>& theCoeff, QuasiPolynomial& output, GlobalVariables& theGlobalVariables)
{ QuasiPolynomial shiftedPoly;
  Vectors<Rational> theNormals, theLatticeGenerators;
  if (partFraction::MakingConsistencyCheck)
    partFraction::CheckSum.MakeZero();
  this->GetRootsFromDenominator(owner, theLatticeGenerators);
  Lattice theLattice;
  theLattice.MakeFromRoots(theLatticeGenerators);
//  std::cout << "<hr><hr> the lattice generators: " << theLatticeGenerators.ElementToString();
//  std::cout << "<br>Corresponding lattice: " << theLattice.ElementToString();
  Matrix<Rational>  theNormalsMatForm;
  theNormalsMatForm.AssignRootsToRowsOfMatrix(theLatticeGenerators);
  theNormalsMatForm.Invert();
  theNormals.AssignMatrixColumns(theNormalsMatForm);
  output.MakeZeroLatTiceZn(owner.AmbientDimension);

  for (int i=0; i<theCoeff.size; i++)
  { this->ComputePolyCorrespondingToOneMonomial(shiftedPoly, theCoeff[i], theNormals, theLattice, theGlobalVariables);
//    output.ComputeDebugString();
//    shiftedPoly.ComputeDebugString();
//    std::cout << "<hr>Current fraction monomial " << i+1 << " out of " << this->Coefficient.size;
//    std::cout << "<br>Contribution: " << shiftedPoly.ElementToString(true, false);
    shiftedPoly*=theCoeff.theCoeffs[i];
    output+=shiftedPoly;
//    std::cout << "<br>Accumulator: " << output.ElementToString(true, false);
//    if (RecordNumMonomials)
//    { std::stringstream out4, out3;
//      out4 << "Current fraction: " << i+1<< " out of " << this->Coefficient.size << " processed";
//      partFractions::NumProcessedForVPFMonomialsTotal++;
//      out3  << " Processed " << partFractions::NumProcessedForVPFMonomialsTotal << " out of " << partFractions::NumMonomialsInNumeratorsRelevantFractions << " relevant monomials";
//      theGlobalVariables.theIndicatorVariables.ProgressReportStrings[3]= out4.str();
//      theGlobalVariables.theIndicatorVariables.ProgressReportStrings[2]= out3.str();
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
  (QuasiPolynomial& output, Vector<Rational>& newIndicator, GlobalVariables& theGlobalVariables)
{ if(this->AssureIndicatorRegularity(theGlobalVariables, newIndicator))
  { theGlobalVariables.theIndicatorVariables.flagRootIsModified=true;
    theGlobalVariables.theIndicatorVariables.modifiedRoot=(newIndicator);
    theGlobalVariables.theIndicatorVariables.ProgressReportStrings[4]="Indicator modified to regular";
    theGlobalVariables.FeedIndicatorWindow(theGlobalVariables.theIndicatorVariables);
  } else
    theGlobalVariables.theIndicatorVariables.flagRootIsModified=false;
  this->ResetRelevanceIsComputed();
  if (!this->CheckForMinimalityDecompositionWithRespectToRoot(&newIndicator, theGlobalVariables))
    return false;
  this->NumProcessedForVPFfractions=0;
  Rational oldCheckSum;
//  QuasiPolynomialOld oldOutput;
  if (partFraction::MakingConsistencyCheck)
    partFractions::CheckSum.MakeZero();
  theGlobalVariables.theIndicatorVariables.NumProcessedMonomialsCurrentFraction=0;
  output.MakeZeroLatTiceZn(this->AmbientDimension);
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
      currentPF.GetVectorPartitionFunction(*this, this->theCoeffs[i], tempQP, theGlobalVariables);
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
{ Vectors<Rational> theVectors;
  int theDim=-1;
  if (!theNode.GetRootsEqualDimNoConversionNoEmptyArgument(theArgumentList, theVectors, theDim))
    return theNode.SetError(theNode.errorDimensionProblem);
  //std::cout << theVectors.ElementToString();
  if (!theNode.thePFs.GetElement().initFromRoots(theVectors, theGlobalVariables))
    return theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);
  std::string tempS;
  std::stringstream out;
  FormatExpressions theFormat;
  theNode.thePFs.GetElement().ElementToString(tempS, theGlobalVariables, theFormat);
  out << "<div class=\"math\">" << tempS << "</div>";
  theNode.outputString=out.str();
  theNode.ExpressionType=theNode.typePartialFractions;
  return theNode.errorNoError;
}

void ParserNode::CreateDefaultLatexAndPDFfromString
  (const std::string& theLatexFileString)
{ std::fstream outputFile;
  std::stringstream out;
  CGI::OpenFileCreateIfNotPresent(outputFile, this->owner->PhysicalNameDefaultOutput+".tex", false, true, false);
  outputFile << theLatexFileString;
  out << "A latex/pdf file: <a href=\"" << this->owner->DisplayNameDefaultOutput << ".tex\">"
  << this->owner->DisplayNameDefaultOutputNoPath << ".tex</a>";
  out << ", <a href=\"" << this->owner->DisplayNameDefaultOutput << ".pdf\">"
  << this->owner->DisplayNameDefaultOutputNoPath << ".pdf</a>";
  this->outputString=out.str();
  std::stringstream theCommand;
  theCommand << "pdflatex -output-directory=" << this->owner->PhysicalPathOutputFolder << "   "
  << this->owner->PhysicalNameDefaultOutput;
  //std::cout << theCommand.str();
  this->owner->SystemCommands.AddOnTop(theCommand.str());
  this->ExpressionType=this->typeString;
}

std::string partFractions::DoTheFullComputationReturnLatexFileString
(GlobalVariables& theGlobalVariables, Vectors<Rational>& toBePartitioned, FormatExpressions& theFormat, std::string* outputHtml)
{ std::string whatWentWrong;
  if (!this->ArgumentsAllowed(toBePartitioned, whatWentWrong, theGlobalVariables))
    return whatWentWrong;
  assert(false);
//  this->theChambersOld.theDirections=toBePartitioned;
  this->AmbientDimension=toBePartitioned.GetDim();
//  this->theChambersOld.AmbientDimension=toBePartitioned.GetDim();
//  this->theChambersOld.thePauseController.InitComputation();
  //this->theChambers.ReadFromDefaultFile(theGlobalVariables);
  std::stringstream out;
  std::stringstream outHtml;
  assert(false);
//  this->theChambersOld.SliceTheEuclideanSpace(theGlobalVariables, false);
//  this->theChambersOld.QuickSortAscending();
//  this->theChambersOld.LabelChamberIndicesProperly();
//  this->theChambers.AssignCombinatorialChamberComplex(this->theChambersOld, theGlobalVariables);
//  this->theChambersOld.drawOutput(theGlobalVariables.theDrawingVariables, tempRoot, 0);
//  this->theChambersOld.thePauseController.ExitComputation();
  this->theChambers.DrawMeProjective(0, true, theGlobalVariables.theDrawingVariables, theFormat);
  outHtml <<
    theGlobalVariables.theDrawingVariables.GetHtmlFromDrawOperationsCreateDivWithUniqueName
    (this->AmbientDimension);
  Vector<Rational> tempRoot; tempRoot.MakeZero(this->AmbientDimension);
  tempRoot.MakeZero(this->AmbientDimension);
  assert(false);
//  this->initFromRoots(theChambersOld.theDirections, theGlobalVariables);
  out << "\\documentclass{article}\\usepackage{amsmath, amsfonts, amssymb} \n\\begin{document}\n";
  out << "The vector partition funciton is the number of ways you can represent a vector $(x_1,\\dots, x_n)$ as a non-negative integral linear combination of "
        << " a given set of vectors.  You requested the vector partition function with respect to the set of vectors: ";
  assert(false);
//  out << this->theChambersOld.theDirections.ElementToStringGeneric();
  out << "\n\n The corresponding generating function is: " << this->ElementToString(theGlobalVariables, theFormat) << "= (after splitting acording to algorithm)";
  this->split(theGlobalVariables, 0);
  out << this->ElementToString(theGlobalVariables, theFormat);
  assert(false);
//  out << "Therefore the vector partition function is given by " << this->theChambersOld.GetNumNonZeroPointers()
//        << " quasipolynomials depending on which set of linear inequalities is satisfied (each such set we call ``Chamber'').";
//  outHtml << "There are " << this->theChambersOld.size << " systems of linear inequalities "
//  << " such that on each such system of inequalities the vector partition function is quasi-polynomial. "
//  << " A full list of the systems of inequalities (\"chambers\") and the corresponding quasi-polynomials follows.<hr> ";
  QuasiPolynomial tempQP;
  std::string tempS;
  Vector<Rational> tempIndicator;
  assert(false);
  /*for (int i=0; i<this->theChambersOld.size; i++)
    if (this->theChambersOld.TheObjects[i]!=0)
    { Cone& currentChamber=this->theChambers[i];
      tempIndicator=currentChamber.GetInternalPoint();
      this->GetVectorPartitionFunction(tempQP, tempIndicator, theGlobalVariables);
      out << "\n\n" << currentChamber.ElementToString(true, false, true, false, theFormat);
      out << "\n\nQuasipolynomial: " << tempQP.ElementToString(false, true, theFormat);
      outHtml << "<hr>Chamber: " << currentChamber.ElementToString(false, true, true, false, theFormat);
      bool useHtml=false;
      if (tempQP.valueOnEachLatticeShift.size>0)
        if (tempQP.valueOnEachLatticeShift[0].size<30)
          useHtml=true;
      outHtml << "<br>Quasi-polynomial: " <<
      CGI::GetHtmlMathDivFromLatexFormulA(tempQP.ElementToString(useHtml, true, theFormat));
    }
    */
  out << "\\end{document}";
  if (outputHtml!=0)
    *outputHtml=outHtml.str();
  return out.str();
}

void PiecewiseQuasipolynomial::operator=(const PiecewiseQuasipolynomial& other)
{ this->theBuffers=other.theBuffers;
  this->theProjectivizedComplex=other.theProjectivizedComplex;
  this->theQPs=other.theQPs;
  this->NumVariables=other.NumVariables;
}

int ParserNode::EvaluateVPF
(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ //partFractions& currentPF=theNode.thePFs.GetElement();
  theGlobalVariables.MaxAllowedComputationTimeInSeconds=50;
  Vectors<Rational> toBePartitioned; int tempDim;
  theNode.GetRootsEqualDimNoConversionNoEmptyArgument(theArgumentList, toBePartitioned, tempDim);
  std::stringstream out;
  FormatExpressions theFormat;
//  theFormat.alphabet.TheObjects[0]="t_1";
//  theFormat.alphabet.TheObjects[1]="t_2";
  PiecewiseQuasipolynomial& thePQP=theNode.thePiecewiseQP.GetElement();
  std::string tempS;
  if(!thePQP.MakeVPF(toBePartitioned, tempS, theGlobalVariables))
  { out << tempS;
    theNode.outputString=out.str();
    return theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);
  }
  out << tempS;
  theGlobalVariables.theDrawingVariables.theBuffer.MakeMeAStandardBasis(tempDim);
  theGlobalVariables.theDrawingVariables.drawCoordSystemBuffer(theGlobalVariables.theDrawingVariables, tempDim, 0);
  if (tempDim==2)
  { thePQP.DrawMe(theGlobalVariables.theDrawingVariables, 10);
    out << theGlobalVariables.theDrawingVariables.GetHtmlFromDrawOperationsCreateDivWithUniqueName(tempDim);
  }
  out << thePQP.ElementToString(true, true);
  theNode.outputString=out.str();
  theNode.ExpressionType=theNode.typePiecewiseQP;
  return theNode.errorNoError;
}

int ParserNode::EvaluateVectorPFIndicator
(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ partFractions& currentPF=theNode.thePFs.GetElement();
  Vectors<Rational> toBePartitioned; int tempDim;
  if (!theNode.GetRootsEqualDimNoConversionNoEmptyArgument(theArgumentList, toBePartitioned, tempDim))
    return theNode.SetError(theNode.errorBadOrNoArgument);
  std::stringstream out;
  FormatExpressions theFormat;
//  theFormat.alphabet.TheObjects[0]="t_1";
//  theFormat.alphabet.TheObjects[1]="t_2";
  theGlobalVariables.MaxAllowedComputationTimeInSeconds=50;
  std::string htmlString;
  std::string tempS= currentPF.DoTheFullComputationReturnLatexFileString
  (theGlobalVariables, toBePartitioned, theFormat, &htmlString)
  ;
  theNode.CreateDefaultLatexAndPDFfromString(tempS);
  out << theNode.outputString << "<br>" << htmlString;
  theNode.outputString=out.str();
  return theNode.errorNoError;
}

void QuasiPolynomial::AddLatticeShift(const Polynomial<Rational> & input, const Vector<Rational>& inputShift)
{ Vector<Rational> theShift=inputShift;
  this->AmbientLatticeReduced.ReduceVector(theShift);
  int index=this->LatticeShifts.IndexOfObject(theShift);
  if (index==-1)
  { index=this->LatticeShifts.size;
    this->LatticeShifts.AddOnTop(theShift);
    this->valueOnEachLatticeShift.ExpandOnTop(1);
    this->valueOnEachLatticeShift.LastObject()->MakeZero(input.NumVars);
  }
  this->valueOnEachLatticeShift.TheObjects[index]+=input;
  if (this->valueOnEachLatticeShift.TheObjects[index].IsEqualToZero())
  { this->LatticeShifts.PopIndexSwapWithLast(index);
    this->valueOnEachLatticeShift.PopIndexSwapWithLast(index);
  }
}

void QuasiPolynomial::Substitution
  (const Matrix<Rational> & mapFromNewSpaceToOldSpace,
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
  Vectors<Rational> allRepresentatives, imagesAllRepresentatives;
  bool tempBool=ambientLatticeNewSpace.GetAllRepresentatives(output.AmbientLatticeReduced, allRepresentatives);
  assert(tempBool);
  mapFromNewSpaceToOldSpace.ActOnVectorsColumn(allRepresentatives, imagesAllRepresentatives);
  PolynomialSubstitution<Rational> theSub;
  theSub.SetSize(this->GetNumVars());
  Vector<Rational> tempRoot;
  for (int i=0; i<theSub.size; i++)
  { Polynomial<Rational>& currentPoly=theSub.TheObjects[i];
    mapFromNewSpaceToOldSpace.RowToRoot(i, tempRoot);
    currentPoly.MakeLinPolyFromRootNoConstantTerm(tempRoot);
  }
  Polynomial<Rational>  tempP;
  for (int i=0; i<this->valueOnEachLatticeShift.size; i++)
  { this->valueOnEachLatticeShift.TheObjects[i].Substitution(theSub, tempP, mapFromNewSpaceToOldSpace.NumCols, (Rational) 1);
    for (int j=0; j<allRepresentatives.size; j++)
      if (imagesAllRepresentatives.TheObjects[j]==this->LatticeShifts.TheObjects[i])
        output.AddLatticeShift(tempP, allRepresentatives.TheObjects[j]);
  }
}

void QuasiPolynomial::Substitution
(const Vector<Rational>& inputTranslationSubtractedFromArgument, QuasiPolynomial& output, GlobalVariables& theGlobalVariables)
{ //format of the translation. If the starting quasipolynomial was P(y_1, ..., y_n),
  //and the translation has coordinates (t_1, ..., t_n),
  //then the resulting quasipolynomial will be P(x_1-t_1, ..., x_n-t_n)
  PolynomialSubstitution<Rational> theSub;
  theSub.MakeIdSubstitution(this->GetNumVars(), (Rational) 1);
  for (int i=0; i<theSub.size; i++)
    theSub.TheObjects[i].AddConstant(-inputTranslationSubtractedFromArgument[i]);
  Polynomial<Rational>  tempP;
  output.MakeZeroLatTiceZn(this->GetNumVars());
  output.AmbientLatticeReduced=this->AmbientLatticeReduced;
  for (int i=0; i<this->valueOnEachLatticeShift.size; i++)
  { this->valueOnEachLatticeShift.TheObjects[i].Substitution(theSub, tempP, this->GetNumVars(), (Rational) 1);
    output.AddLatticeShift(tempP, this->LatticeShifts.TheObjects[i]+inputTranslationSubtractedFromArgument);
  }
}

void QuasiPolynomial::Substitution
  (const Matrix<Rational> & mapFromNewSpaceToOldSpace, const Vector<Rational>& inputTranslationSubtractedFromArgument,
   const Lattice& ambientLatticeNewSpace, QuasiPolynomial& output, GlobalVariables& theGlobalVariables)
{ QuasiPolynomial tempQP;
  this->Substitution(inputTranslationSubtractedFromArgument, tempQP, theGlobalVariables);
  tempQP.Substitution(mapFromNewSpaceToOldSpace, ambientLatticeNewSpace, output, theGlobalVariables);
}

bool QuasiPolynomial::SubstitutionLessVariables
  (const PolynomialSubstitution<Rational>& theSub, QuasiPolynomial& output, GlobalVariables& theGlobalVariables)const
{ Matrix<Rational>  theLatticeSub;
  if (!this->AmbientLatticeReduced.GetHomogeneousSubMatFromSubIgnoreConstantTerms(theSub, theLatticeSub, theGlobalVariables))
    return false;
  Matrix<Rational>  theSubLatticeShift;
  output.AmbientLatticeReduced=this->AmbientLatticeReduced;
  if(!output.AmbientLatticeReduced.SubstitutionHomogeneous(theLatticeSub, theGlobalVariables))
    return false;
  theSubLatticeShift.init(theLatticeSub.NumRows,1);
  for (int i=0; i<theSubLatticeShift.NumRows; i++)
    theSub.TheObjects[i].GetConstantTerm(theSubLatticeShift.elements[i][0], (Rational) 0);
  Matrix<Rational>  theShiftImage, shiftMatForm;
  output.LatticeShifts.size=0;
  output.valueOnEachLatticeShift.size=0;
  output.valueOnEachLatticeShift.Reserve(this->LatticeShifts.size);
  output.LatticeShifts.Reserve(this->LatticeShifts.size);
  Vector<Rational> tempRoot;
  Polynomial<Rational>  tempP;
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
    (const PolynomialSubstitution<Rational>& theSub, GlobalVariables& theGlobalVariables)
{ Matrix<Rational>  matSub;
  if (!this->GetHomogeneousSubMatFromSubIgnoreConstantTerms(theSub, matSub, theGlobalVariables))
    return false;
  return this->SubstitutionHomogeneous(matSub, theGlobalVariables);
}

bool Lattice::GetHomogeneousSubMatFromSubIgnoreConstantTerms
(const PolynomialSubstitution<Rational>& theSub, Matrix<Rational> & output, GlobalVariables& theGlobalVariables)
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

void Lattice::IntersectWithLinearSubspaceGivenByNormal(const Vector<Rational>& theNormal)
{ //std::cout << "<br>Starting lattice: " << this->ElementToString(true, false) << "<br>";
  Vectors<Rational> startingBasis, resultBasis;
  startingBasis.AssignMatrixRows(this->basisRationalForm);
  //std::cout << "Starting normal: " << theNormal.ElementToString();
  Vector<Rational> theScalarProducts;
  theScalarProducts.SetSize(startingBasis.size);
  for (int i=0; i<this->basisRationalForm.NumRows; i++)
    theScalarProducts[i]=Vector<Rational>::ScalarEuclidean(startingBasis[i], theNormal);
  if (theScalarProducts.IsEqualToZero())
    return;
  //std::cout << "<br>the scalar products: " << theScalarProducts.ElementToString();
  int pivotColumnIndex=theScalarProducts.getIndexLastNonZeroCoordinate();
  Rational pivotCoeff=theScalarProducts[pivotColumnIndex];
  theScalarProducts/=-pivotCoeff;
  //std::cout << "<br>the scalar products after scaling: " << theScalarProducts.ElementToString();
  Vectors<Rational> eigenSpacePlusOrthogonalComponent;
  eigenSpacePlusOrthogonalComponent.SetSize(theScalarProducts.size);
  for (int i=0; i<theScalarProducts.size; i++)
    if (i!=pivotColumnIndex)
    { Vector<Rational>& currentRoot=eigenSpacePlusOrthogonalComponent.TheObjects[i];
      currentRoot.MakeZero(theScalarProducts.size);
      currentRoot[i]=1;
      currentRoot.TheObjects[pivotColumnIndex]=theScalarProducts.TheObjects[i];
    }
  theScalarProducts.ScaleByPositiveRationalToIntegralMinHeight();
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
  resultBasis.Reserve(theScalarProducts.size-1);
  Vector<Rational> tempRoot, resultRoot; Rational orthogonalComponent;
  for (int i=0; i<theZnLattice.basisRationalForm.NumRows; i++)
  { theZnLattice.basisRationalForm.RowToRoot(i, tempRoot);
    orthogonalComponent=tempRoot.ScalarEuclidean(theScalarProducts)/theScalarProducts.ScalarEuclidean(theScalarProducts);
    tempRoot-=theScalarProducts*orthogonalComponent;
    assert(orthogonalComponent.IsInteger());
    if (!tempRoot.IsEqualToZero())
    { resultRoot.MakeZero(this->GetDim());
      for (int j=0; j<startingBasis.size; j++)
        resultRoot+=startingBasis.TheObjects[j]*tempRoot.TheObjects[j];
      resultBasis.AddOnTop(resultRoot);
    }
  }
  //std::cout << "<br>Resulting basis: " << resultBasis.ElementToString();
  this->MakeFromRoots(resultBasis);
  //std::cout << "<br>Final answer lattice form: " << this->ElementToString(true, false);
}

void Lattice::IntersectWithLinearSubspaceSpannedBy(const Vectors<Rational>& theSubspaceBasis)
{ Vectors<Rational> theNormals;
  Matrix<Rational> theMat;
  theSubspaceBasis.GetMatrixRootsToRows(theMat);
  theMat.FindZeroEigenSpace(theNormals);
  this->IntersectWithLinearSubspaceGivenByNormals(theNormals);
}

void Lattice::IntersectWithLinearSubspaceGivenByNormals(const Vectors<Rational>& theSubspaceNormals)
{ //std::cout << "<br>********************Debug info for IntersectWithLinearSubspaceGivenByNormals*******************";
  for (int i=0; i<theSubspaceNormals.size; i++)
    this->IntersectWithLinearSubspaceGivenByNormal(theSubspaceNormals.TheObjects[i]);
  //std::cout << "<br>********************End of debug info for IntersectWithLinearSubspaceGivenByNormals*******************";
}

bool Lattice::SubstitutionHomogeneous
 (const Matrix<Rational> & theSub, Lattice& resultIsSubsetOf, GlobalVariables& theGlobalVariables)
{ /*Vectors<Rational> preimageBasis;
  preimageBasis.AssignMatrixRows(this->basisRationalForm);
  Matrix<Rational>  theSubModifiable, currentBasisVector, oneSolution;
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
  (const Matrix<Rational> & theSub, GlobalVariables& theGlobalVariables)
{ int targetDim=theSub.NumCols;
  if (theSub.NumRows!=this->GetDim())
    return false;
  //std::cout <<"<br> the sub: " << theSub.ElementToString(true, false) << "<br>";
  int startingDim=this->GetDim();
  Matrix<Rational>  theMat, oldBasisTransformed, matRelationBetweenStartingVariables;
  theMat=theSub;
  oldBasisTransformed=this->basisRationalForm;
  oldBasisTransformed.Transpose();
  Selection nonPivotPoints;
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
  Vectors<Rational> theEigenSpace;
  matRelationBetweenStartingVariables.FindZeroEigenSpace(theEigenSpace, theGlobalVariables);
  //std::cout << "<br>matRelationBetweenStartingVariables" <<  matRelationBetweenStartingVariables.ElementToString(true, false);
  for (int i=0; i<theEigenSpace.size; i++)
    theEigenSpace.TheObjects[i].ScaleByPositiveRationalToIntegralMinHeight();
  //std::cout << "the basis: " << theEigenSpace.ElementToString();
  oldBasisTransformed.ActOnVectorsColumn(theEigenSpace);
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

void Cone::WriteToFile
  (std::fstream& output, GlobalVariables* theGlobalVariables)
{ output << XMLRoutines::GetOpenTagNoInputCheckAppendSpacE(this->GetXMLClassName());
  output << "Cone( ";
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
  output << XMLRoutines::GetCloseTagNoInputCheckAppendSpacE(this->GetXMLClassName());
}

bool Cone::ReadFromFile(std::fstream& output, GlobalVariables* theGlobalVariables)
{ if (theGlobalVariables!=0)
    return this->ReadFromFile(output, theGlobalVariables->rootsConeWriteToFileBuffer.GetElement(), theGlobalVariables);
  else
  { Vectors<Rational> tempRoots;
    return this->ReadFromFile(output, tempRoots, 0);
  }
}

bool Cone::ReadFromFile
  (std::fstream& input, Vectors<Rational>& buffer, GlobalVariables* theGlobalVariables)
{ std::string tempS, rootString;
  int NumWordsRead;
  XMLRoutines::ReadThroughFirstOpenTag(input, NumWordsRead, this->GetXMLClassName());
  assert(NumWordsRead==0);
  input >> tempS;
  buffer.size=0;
  Vector<Rational> tempRoot;
  if (tempS!="Cone(")
  { std::cout << "tempS was instead " << tempS;
    XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, NumWordsRead, this->GetXMLClassName());
    return false;
  }
  for (input >> tempS; tempS!=")" && tempS!=""; input >> tempS)
  { tempRoot.AssignString(tempS);
    buffer.AddOnTop(tempRoot);
    //std::cout << "vector input " << tempS << " read as " << tempRoot.ElementToString();
  }
  if (buffer.size<1)
  { XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, NumWordsRead, this->GetXMLClassName());
    return false;
  }
  int theDim=buffer.TheObjects[0].size;
  for (int i=1; i<buffer.size; i++)
    if (buffer.TheObjects[i].size!=theDim)
    { XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, NumWordsRead, this->GetXMLClassName());
      return false;
    }
  bool result;
  if (theGlobalVariables!=0)
    result=this->CreateFromNormals(buffer, *theGlobalVariables);
  else
  { GlobalVariables tempGlobalVariables;
    result=this->CreateFromNormals(buffer, tempGlobalVariables);
  }
  result= XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, NumWordsRead, this->GetXMLClassName()) && result;
//  assert(tempBool);
  return result;
}

void ConeLatticeAndShift::WriteToFile
  (std::fstream& output, GlobalVariables* theGlobalVariables)
{ output << XMLRoutines::GetOpenTagNoInputCheckAppendSpacE(this->GetXMLClassName());
  this->theLattice.WriteToFile(output, theGlobalVariables);
  this->theProjectivizedCone.WriteToFile(output, theGlobalVariables);
  this->theShift.WriteToFile(output);
  output << XMLRoutines::GetCloseTagNoInputCheckAppendSpacE(this->GetXMLClassName());
}

bool ConeLatticeAndShift::ReadFromFile
  (std::fstream& output, GlobalVariables* theGlobalVariables)
{ XMLRoutines::ReadThroughFirstOpenTag(output, this->GetXMLClassName());
  this->theLattice.ReadFromFile(output, theGlobalVariables);
  this->theProjectivizedCone.ReadFromFile(output, theGlobalVariables);
  this->theShift.ReadFromFile(output);
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(output, this->GetXMLClassName());
  return true;
}

void ConeLatticeAndShiftMaxComputation::WriteToFile
  (std::fstream& output, GlobalVariables* theGlobalVariables)
{ output << XMLRoutines::GetOpenTagNoInputCheckAppendSpacE(this->GetXMLClassName());
  output << " NumParam: " << this->numNonParaM << " numProcessedNonParam: " << this->numProcessedNonParam << "\n";

  output << XMLRoutines::GetOpenTagNoInputCheckAppendSpacE("complexStartingPerRepresentative");
  this->complexStartingPerRepresentative.WriteToFile(output, theGlobalVariables);
  output << XMLRoutines::GetCloseTagNoInputCheckAppendSpacE("complexStartingPerRepresentative");

  output << XMLRoutines::GetOpenTagNoInputCheckAppendSpacE("complexRefinedPerRepresentative");
  this->complexRefinedPerRepresentative.WriteToFile(output, theGlobalVariables);
  output << XMLRoutines::GetCloseTagNoInputCheckAppendSpacE("complexRefinedPerRepresentative");

  output << XMLRoutines::GetOpenTagNoInputCheckAppendSpacE("theMaximaCandidates");
  this->theMaximaCandidates.WriteToFile(output);
  output << XMLRoutines::GetCloseTagNoInputCheckAppendSpacE("theMaximaCandidates");

  output << XMLRoutines::GetOpenTagNoInputCheckAppendSpacE("startingLPtoMaximize");
  this->startingLPtoMaximize.WriteToFile(output);
  output << XMLRoutines::GetCloseTagNoInputCheckAppendSpacE("startingLPtoMaximize");

  output << XMLRoutines::GetOpenTagNoInputCheckAppendSpacE("finalMaxima");
  this->finalMaxima.WriteToFile(output);
  output << XMLRoutines::GetCloseTagNoInputCheckAppendSpacE("finalMaxima");

  output << XMLRoutines::GetOpenTagNoInputCheckAppendSpacE("theStartingLattice");
  this->theStartingLattice.WriteToFile(output, theGlobalVariables);
  output << XMLRoutines::GetCloseTagNoInputCheckAppendSpacE("theStartingLattice");

  output << XMLRoutines::GetOpenTagNoInputCheckAppendSpacE("theFinalRougherLattice");
  this->theFinalRougherLattice.WriteToFile(output, theGlobalVariables);
  output << XMLRoutines::GetCloseTagNoInputCheckAppendSpacE("theFinalRougherLattice");

  output << XMLRoutines::GetOpenTagNoInputCheckAppendSpacE("theStartingRepresentative");
  this->theStartingRepresentative.WriteToFile(output);
  output << XMLRoutines::GetCloseTagNoInputCheckAppendSpacE("theStartingRepresentative");

  output << XMLRoutines::GetOpenTagNoInputCheckAppendSpacE("theFinalRepresentatives");
  this->theFinalRepresentatives.WriteToFile(output, theGlobalVariables);
  output << XMLRoutines::GetCloseTagNoInputCheckAppendSpacE("theFinalRepresentatives");

  output << XMLRoutines::GetOpenTagNoInputCheckAppendSpacE("theConesLargerDim");
  this->theConesLargerDim.WriteToFile(output, theGlobalVariables);
  output << XMLRoutines::GetCloseTagNoInputCheckAppendSpacE("theConesLargerDim");

  output << XMLRoutines::GetOpenTagNoInputCheckAppendSpacE("theConesSmallerDim");
  this->theConesSmallerDim.WriteToFile(output, theGlobalVariables);
  output << XMLRoutines::GetCloseTagNoInputCheckAppendSpacE("theConesSmallerDim");

  output << XMLRoutines::GetOpenTagNoInputCheckAppendSpacE("IsInfinity");
  output << this->IsInfinity;
  output << XMLRoutines::GetCloseTagNoInputCheckAppendSpacE("IsInfinity");

  output << XMLRoutines::GetOpenTagNoInputCheckAppendSpacE("LPtoMaximizeLargerDim");
  this->LPtoMaximizeLargerDim.WriteToFile(output, theGlobalVariables);
  output << XMLRoutines::GetCloseTagNoInputCheckAppendSpacE("LPtoMaximizeLargerDim");

  output << XMLRoutines::GetOpenTagNoInputCheckAppendSpacE("LPtoMaximizeSmallerDim");
  this->LPtoMaximizeSmallerDim.WriteToFile(output, theGlobalVariables);
  output << XMLRoutines::GetCloseTagNoInputCheckAppendSpacE("LPtoMaximizeSmallerDim");

  output << XMLRoutines::GetCloseTagNoInputCheckAppendSpacE(this->GetXMLClassName()) << "\n";
}

bool ConeLatticeAndShiftMaxComputation::ReadFromFile
(std::fstream& input, GlobalVariables* theGlobalVariables, int UpperLimitDebugPurposes)
{ int numReadWords;
  XMLRoutines::ReadThroughFirstOpenTag(input, numReadWords, this->GetXMLClassName());
  std::string tempS;
  input >> tempS >> this->numNonParaM >> tempS >> this->numProcessedNonParam;
  if (theGlobalVariables!=0)
  { theGlobalVariables->IncrementReadWriteDepth();
    theGlobalVariables->theIndicatorVariables.ProgressReportStrings[theGlobalVariables->ProgressReportDepth]="Loading complex starting per representative...";
    theGlobalVariables->MakeReport();
  }
  XMLRoutines::ReadThroughFirstOpenTag(input, "complexStartingPerRepresentative");
  this->complexStartingPerRepresentative.ReadFromFile(input, theGlobalVariables);
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, "complexStartingPerRepresentative");

  if (theGlobalVariables!=0)
  { theGlobalVariables->theIndicatorVariables.ProgressReportStrings[theGlobalVariables->ProgressReportDepth]="Loading complex refined per representative...";
    theGlobalVariables->MakeReport();
  }
  XMLRoutines::ReadThroughFirstOpenTag(input, "complexRefinedPerRepresentative");
  this->complexRefinedPerRepresentative.ReadFromFile(input, theGlobalVariables);
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, "complexRefinedPerRepresentative");

  if (theGlobalVariables!=0)
  { theGlobalVariables->theIndicatorVariables.ProgressReportStrings[theGlobalVariables->ProgressReportDepth]="Loading the max candidates...";
    theGlobalVariables->MakeReport();
  }
  XMLRoutines::ReadThroughFirstOpenTag(input, "theMaximaCandidates");
  this->theMaximaCandidates.ReadFromFile(input, theGlobalVariables);
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, "theMaximaCandidates");

  if (theGlobalVariables!=0)
  { theGlobalVariables->theIndicatorVariables.ProgressReportStrings[theGlobalVariables->ProgressReportDepth]="Loading starting linear polys...";
    theGlobalVariables->MakeReport();
  }
  XMLRoutines::ReadThroughFirstOpenTag(input, "startingLPtoMaximize");
  this->startingLPtoMaximize.ReadFromFile(input, theGlobalVariables);
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, "startingLPtoMaximize");

  if (theGlobalVariables!=0)
  { theGlobalVariables->theIndicatorVariables.ProgressReportStrings[theGlobalVariables->ProgressReportDepth]="Loading final maxima...";
    theGlobalVariables->MakeReport();
  }
  XMLRoutines::ReadThroughFirstOpenTag(input, "finalMaxima");
  this->finalMaxima.ReadFromFile(input, theGlobalVariables);
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, "finalMaxima");

  if (theGlobalVariables!=0)
  { theGlobalVariables->theIndicatorVariables.ProgressReportStrings[theGlobalVariables->ProgressReportDepth]="Loading starting lattice...";
    theGlobalVariables->MakeReport();
  }
  XMLRoutines::ReadThroughFirstOpenTag(input, "theStartingLattice");
  this->theStartingLattice.ReadFromFile(input, theGlobalVariables);
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, "theStartingLattice");

  if (theGlobalVariables!=0)
  { theGlobalVariables->theIndicatorVariables.ProgressReportStrings[theGlobalVariables->ProgressReportDepth]="Loading final rougher lattice...";
    theGlobalVariables->MakeReport();
  }
  XMLRoutines::ReadThroughFirstOpenTag(input, "theFinalRougherLattice");
  this->theFinalRougherLattice.ReadFromFile(input, theGlobalVariables);
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, "theFinalRougherLattice");

  if (theGlobalVariables!=0)
  { theGlobalVariables->theIndicatorVariables.ProgressReportStrings[theGlobalVariables->ProgressReportDepth]="Loading starting representative...";
    theGlobalVariables->MakeReport();
  }
  XMLRoutines::ReadThroughFirstOpenTag(input, "theStartingRepresentative");
  this->theStartingRepresentative.ReadFromFile(input);
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, "theStartingRepresentative");

  if (theGlobalVariables!=0)
  { theGlobalVariables->theIndicatorVariables.ProgressReportStrings[theGlobalVariables->ProgressReportDepth]="Loading final representatives...";
    theGlobalVariables->MakeReport();
  }
  XMLRoutines::ReadThroughFirstOpenTag(input, "theFinalRepresentatives");
  this->theFinalRepresentatives.ReadFromFile(input, theGlobalVariables);
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, "theFinalRepresentatives");

  if (theGlobalVariables!=0)
  { theGlobalVariables->theIndicatorVariables.ProgressReportStrings[theGlobalVariables->ProgressReportDepth]="Loading cones larger dim...";
    theGlobalVariables->MakeReport();
  }
  XMLRoutines::ReadThroughFirstOpenTag(input, "theConesLargerDim");
  this->theConesLargerDim.ReadFromFile(input, theGlobalVariables, UpperLimitDebugPurposes);
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, "theConesLargerDim");

  if (theGlobalVariables!=0)
  { theGlobalVariables->theIndicatorVariables.ProgressReportStrings[theGlobalVariables->ProgressReportDepth]="Loading cones smaller dim...";
    theGlobalVariables->MakeReport();
  }
  XMLRoutines::ReadThroughFirstOpenTag(input, "theConesSmallerDim");
  this->theConesSmallerDim.ReadFromFile(input, theGlobalVariables);
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, "theConesSmallerDim");

  if (theGlobalVariables!=0)
  { theGlobalVariables->theIndicatorVariables.ProgressReportStrings[theGlobalVariables->ProgressReportDepth]="Loading IsInfinite array...";
    theGlobalVariables->MakeReport();
  }
  XMLRoutines::ReadThroughFirstOpenTag(input, "IsInfinity");
  input >> this->IsInfinity;
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, "IsInfinity");

  if (theGlobalVariables!=0)
  { theGlobalVariables->theIndicatorVariables.ProgressReportStrings[theGlobalVariables->ProgressReportDepth]="Loading LPtoMaximizeLargerDim...";
    theGlobalVariables->MakeReport();
  }
  XMLRoutines::ReadThroughFirstOpenTag(input, "LPtoMaximizeLargerDim");
  this->LPtoMaximizeLargerDim.ReadFromFile(input, theGlobalVariables, UpperLimitDebugPurposes);
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, "LPtoMaximizeLargerDim");

  if (theGlobalVariables!=0)
  { theGlobalVariables->theIndicatorVariables.ProgressReportStrings[theGlobalVariables->ProgressReportDepth]="Loading LPtoMaximizeSmallerDim...";
    theGlobalVariables->MakeReport();
  }
  XMLRoutines::ReadThroughFirstOpenTag(input, "LPtoMaximizeSmallerDim");
  this->LPtoMaximizeSmallerDim.ReadFromFile(input, theGlobalVariables);
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, "LPtoMaximizeSmallerDim");

  if (theGlobalVariables!=0)
    theGlobalVariables->ProgressReportDepth--;
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, numReadWords, this->GetXMLClassName());
  assert(numReadWords==0);
  return true;
}

void GeneralizedVermaModuleCharacters::WriteToFile
  (std::fstream& output, GlobalVariables* theGlobalVariables)
{ output << XMLRoutines::GetOpenTagNoInputCheckAppendSpacE(this->GetXMLClassName());
  output << "ComputationPhase: " << this->computationPhase << "\n";
  output << "NumProcessedConesParam: " << this->NumProcessedConesParam << "\n";
  output << "NumProcessedExtremaEqualOne: " << this->NumProcessedExtremaEqualOne << "\n";
  output << "NormalConstAdjustment: ";
  this->NonIntegralOriginModificationBasisChanged.WriteToFile(output);
  output << "\n";
  output << "ChamberIndicatorHighestWeightLargerAlgebra: ";
  this->ParabolicLeviPartRootSpacesZeroStandsForSelected.WriteToFile(output);
  this->ParabolicSelectionSmallerAlgebra.WriteToFile(output);
  output << "\n";
  this->WeylLarger.WriteToFile(output);
  this->WeylSmaller.WriteToFile(output);
  this->preferredBasiS.WriteToFile(output);
  this->preferredBasisChangE.WriteToFile(output);
  this->preferredBasisChangeInversE.WriteToFile(output);
  this->theExtendedIntegralLatticeMatForM.WriteToFile(output, theGlobalVariables);
  if (theGlobalVariables!=0)
  { theGlobalVariables->theIndicatorVariables.ProgressReportStringsNeedRefresh=true;
    theGlobalVariables->theIndicatorVariables.ProgressReportStrings[0]="Writing small data... ";
    theGlobalVariables->MakeReport();
  }
  this->theMaxComputation.WriteToFile(output, theGlobalVariables);
  this->GmodKnegativeWeightS.WriteToFile(output, theGlobalVariables);
  this->GmodKNegWeightsBasisChanged.WriteToFile(output, theGlobalVariables);
  this->theLinearOperators.WriteToFile(output);
  this->theLinearOperatorsExtended.WriteToFile(output);
  this->PreimageWeylChamberLargerAlgebra.WriteToFile(output, theGlobalVariables);
  this->PreimageWeylChamberSmallerAlgebra.WriteToFile(output, theGlobalVariables);
  this->WeylChamberSmallerAlgebra.WriteToFile(output, theGlobalVariables);
  if (theGlobalVariables!=0)
  { theGlobalVariables->theIndicatorVariables.ProgressReportStrings[0]="Writing QP's non-subbed... ";
    theGlobalVariables->MakeReport();
  }
  this->theQPsNonSubstituted.WriteToFile(output, theGlobalVariables);
  if (theGlobalVariables!=0)
  { theGlobalVariables->theIndicatorVariables.ProgressReportStrings[0]="Writing QP's subbed... ";
    theGlobalVariables->MakeReport();
  }

  output << XMLRoutines::GetOpenTagNoInputCheckAppendSpacE("QPsSubbed");
  this->theQPsSubstituted.WriteToFile(output, theGlobalVariables);
  output << XMLRoutines::GetCloseTagNoInputCheckAppendSpacE("QPsSubbed");
  if (theGlobalVariables!=0)
  { theGlobalVariables->theIndicatorVariables.ProgressReportStrings[0]="Writing small data... ";
    theGlobalVariables->MakeReport();
  }
  output << XMLRoutines::GetOpenTagNoInputCheckAppendSpacE("theMultiplicities");
  this->theMultiplicities.WriteToFile(output, theGlobalVariables, this->UpperLimitChambersForDebugPurposes);
  output << XMLRoutines::GetCloseTagNoInputCheckAppendSpacE("theMultiplicities");
//  this->theMultiplicitiesExtremaCandidates.WriteToFile(output, theGlobalVariables);
  this->theCoeffs.WriteToFile(output);
  this->theTranslationS.WriteToFile(output, theGlobalVariables);
  this->theTranslationsProjectedBasisChanged.WriteToFile(output, theGlobalVariables);
  this->thePfs.WriteToFile(output, theGlobalVariables);
//  this->paramSubChambers.WriteToFile(output, theGlobalVariables);
//  this->nonParamVertices.WriteToFile(output, theGlobalVariables);
  if (theGlobalVariables!=0)
  { theGlobalVariables->theIndicatorVariables.ProgressReportStrings[0]="Writing param chamber complex... ";
    theGlobalVariables->MakeReport();
  }
  this->projectivizedParamComplex.WriteToFile(output, theGlobalVariables);
  if (theGlobalVariables!=0)
  { theGlobalVariables->theIndicatorVariables.ProgressReportStrings[0]="Writing projectivized chamber complex... ";
    theGlobalVariables->MakeReport();
  }
  this->smallerAlgebraChamber.WriteToFile(output, theGlobalVariables, this->UpperLimitChambersForDebugPurposes);
  this->projectivizedChambeR.WriteToFile(output, theGlobalVariables, this->UpperLimitChambersForDebugPurposes);
  if (theGlobalVariables!=0)
  { theGlobalVariables->theIndicatorVariables.ProgressReportStrings[0]="Writing to file done...";
    theGlobalVariables->MakeReport();
  }
  output << XMLRoutines::GetCloseTagNoInputCheckAppendSpacE(this->GetXMLClassName());
}

void QuasiPolynomial::WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables)
{ this->valueOnEachLatticeShift.WriteToFile(output, theGlobalVariables);
  this->AmbientLatticeReduced.WriteToFile(output, theGlobalVariables);
  this->LatticeShifts.WriteToFile(output, theGlobalVariables);
}

bool QuasiPolynomial::ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables)
{ this->valueOnEachLatticeShift.ReadFromFile(input, theGlobalVariables);
  this->AmbientLatticeReduced.ReadFromFile(input, theGlobalVariables);
  this->LatticeShifts.ReadFromFile(input, theGlobalVariables);
  this->buffers=theGlobalVariables;
  return true;
}

void Lattice::WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables)
{ output << XMLRoutines::GetOpenTagNoInputCheckAppendSpacE(this->GetXMLClassName());
  this->basisRationalForm.WriteToFile(output);
  output << XMLRoutines::GetCloseTagNoInputCheckAppendSpacE(this->GetXMLClassName());
}

bool Lattice::ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables)
{ int numReadWords;
  XMLRoutines::ReadThroughFirstOpenTag(input, numReadWords, this->GetXMLClassName());
  bool result=this->basisRationalForm.ReadFromFile(input);
  this->basisRationalForm.GetMatrixIntWithDen(this->basis, this->Den);
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, numReadWords, this->GetXMLClassName());
  assert(numReadWords==0);
  return result;
}

bool GeneralizedVermaModuleCharacters::ReadFromFileNoComputationPhase
  (std::fstream& input, GlobalVariables* theGlobalVariables)
{ std::string tempS;
  input >> tempS >> this->NumProcessedConesParam;
  input >> tempS >> this->NumProcessedExtremaEqualOne;
  input >> tempS;
  int numReadWords;
  if (tempS!="NormalConstAdjustment:")
  { assert(false);
    return false;
  }
  this->NonIntegralOriginModificationBasisChanged.ReadFromFile(input);
  input >> tempS;
  this->ParabolicLeviPartRootSpacesZeroStandsForSelected.ReadFromFile(input);
  this->ParabolicSelectionSmallerAlgebra.ReadFromFile(input);
  this->WeylLarger.ReadFromFile(input);
  this->WeylSmaller.ReadFromFile(input);
  this->preferredBasiS.ReadFromFile(input, theGlobalVariables);
  this->preferredBasisChangE.ReadFromFile(input, theGlobalVariables);
  this->preferredBasisChangeInversE.ReadFromFile(input);
  this->theExtendedIntegralLatticeMatForM.ReadFromFile(input, theGlobalVariables);
  if (theGlobalVariables!=0)
    theGlobalVariables->IncrementReadWriteDepth();
  if (theGlobalVariables!=0)
    this->theParser.theHmm.MakeG2InB3(this->theParser, *theGlobalVariables);
  else
  { GlobalVariables tempGlobalVars;
    this->theParser.theHmm.MakeG2InB3(this->theParser, tempGlobalVars);
  }
  if (theGlobalVariables!=0)
  { theGlobalVariables->theIndicatorVariables.StatusString1="";
    theGlobalVariables->theIndicatorVariables.ProgressReportStringsNeedRefresh=true;
    theGlobalVariables->theIndicatorVariables.ProgressReportStrings[theGlobalVariables->ProgressReportDepth]="Loading param subchambers cone form... ";
    theGlobalVariables->MakeReport();
  }
  this->theMaxComputation.ReadFromFile(input, theGlobalVariables, this->UpperLimitChambersForDebugPurposes);
  if (theGlobalVariables!=0)
  { theGlobalVariables->theIndicatorVariables.ProgressReportStrings[theGlobalVariables->ProgressReportDepth]="Loading more pieces of data... ";
    theGlobalVariables->MakeReport();
  }
  this->GmodKnegativeWeightS.ReadFromFile(input, theGlobalVariables);
  this->GmodKNegWeightsBasisChanged.ReadFromFile(input, theGlobalVariables);
  this->theLinearOperators.ReadFromFile(input, theGlobalVariables);
  this->theLinearOperatorsExtended.ReadFromFile(input, theGlobalVariables);
  this->PreimageWeylChamberLargerAlgebra.ReadFromFile(input, theGlobalVariables);
  this->PreimageWeylChamberSmallerAlgebra.ReadFromFile(input, theGlobalVariables);
  this->WeylChamberSmallerAlgebra.ReadFromFile(input, theGlobalVariables);
  this->theQPsNonSubstituted.ReadFromFile(input, theGlobalVariables);
  XMLRoutines::ReadThroughFirstOpenTag(input, numReadWords, "QPsSubbed");
  this->theQPsSubstituted.ReadFromFile(input, theGlobalVariables);
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, numReadWords, "QPsSubbed");
  if (theGlobalVariables!=0)
  { theGlobalVariables->theIndicatorVariables.ProgressReportStrings[theGlobalVariables->ProgressReportDepth]="Loading multiplicities... ";
    theGlobalVariables->MakeReport();
  }
  XMLRoutines::ReadThroughFirstOpenTag(input, numReadWords, "theMultiplicities");
  this->theMultiplicities.ReadFromFile(input, theGlobalVariables, this->UpperLimitChambersForDebugPurposes);
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, numReadWords, "theMultiplicities");
  this->theCoeffs.ReadFromFile(input);
  this->theTranslationS.ReadFromFile(input, theGlobalVariables);
  this->theTranslationsProjectedBasisChanged.ReadFromFile(input, theGlobalVariables);
  if (theGlobalVariables!=0)
  { theGlobalVariables->theIndicatorVariables.ProgressReportStrings[theGlobalVariables->ProgressReportDepth]="Loading partial fractions... ";
    theGlobalVariables->MakeReport();
  }
//  this->thePfs.ReadFromFile(input, theGlobalVariables);
  if (theGlobalVariables!=0)
  { theGlobalVariables->theIndicatorVariables.ProgressReportStrings[theGlobalVariables->ProgressReportDepth]="Loading projectivized param complex... ";
    theGlobalVariables->MakeReport();
  }
  this->projectivizedParamComplex.ReadFromFile(input, theGlobalVariables, -1);
  if (theGlobalVariables!=0)
  { theGlobalVariables->theIndicatorVariables.ProgressReportStrings[theGlobalVariables->ProgressReportDepth]="Loading the complex... ";
    theGlobalVariables->MakeReport();
  }
  this->smallerAlgebraChamber.ReadFromFile(input, theGlobalVariables, this->UpperLimitChambersForDebugPurposes);
  this->projectivizedChambeR.ReadFromFile(input, theGlobalVariables, this->UpperLimitChambersForDebugPurposes);
  if (theGlobalVariables!=0)
  { theGlobalVariables->theIndicatorVariables.ProgressReportStrings[theGlobalVariables->ProgressReportDepth]="Loading complete... ";
    theGlobalVariables->MakeReport();
    theGlobalVariables->ProgressReportDepth--;
  }
  return true;
}

void ConeComplex::WriteToFile
  (std::fstream& output, GlobalVariables* theGlobalVariables, int UpperLimit)
{ this->List<Cone>::WriteToFile(output, theGlobalVariables, UpperLimit);
  output << "IndexLowestNonRefined: " << this->indexLowestNonRefinedChamber << "\n";
  this->splittingNormals.WriteToFile(output);
  this->slicingDirections.WriteToFile(output);
  this->ConvexHull.WriteToFile(output, theGlobalVariables);
}

bool ConeComplex::ReadFromFile
  (std::fstream& input, GlobalVariables* theGlobalVariables, int UpperLimitDebugPurposes)
{ if(!this->List<Cone>::ReadFromFile(input, theGlobalVariables, UpperLimitDebugPurposes))
  { assert(false);
    return false;
  }
  std::string tempS;
  input >> tempS >> this->indexLowestNonRefinedChamber;
  if (tempS!="IndexLowestNonRefined:")
  { assert(false);
    return false;
  }
  if (!this->splittingNormals.ReadFromFile(input))
  { assert(false);
    return false;
  }
  this->slicingDirections.ReadFromFile(input, theGlobalVariables);
  this->ConvexHull.ReadFromFile(input, theGlobalVariables);
  return true;
}

void WeylGroup::GetFundamentalWeightsInSimpleCoordinates(Vectors<Rational>& output)
{ Matrix<Rational>  tempMat;
  tempMat=this->CartanSymmetric;
  Rational tempRat;
  for (int i=0; i<this->GetDim(); i++)
  { tempRat=2/this->CartanSymmetric.elements[i][i];
    tempMat.RowTimesScalar(i, tempRat);
  }
  tempMat.Transpose();
  tempMat.Invert();
  output.AssignMatrixRows(tempMat);
}

void WeylGroup::GetIntegralLatticeInSimpleCoordinates(Lattice& output)
{ output.basisRationalForm=this->CartanSymmetric;
  Vector<Rational> tempRoot;
  for (int i=0; i<this->GetDim(); i++)
  { tempRoot.MakeEi(this->GetDim(), i);
    output.basisRationalForm.RowTimesScalar(i, 2/this->RootScalarCartanRoot(tempRoot, tempRoot));
  }
  output.basisRationalForm.Transpose();
  output.basisRationalForm.Invert();
//  std::cout << output.basisRationalForm.ElementToString(true, false);
  output.MakeFromMat(output.basisRationalForm);
  output.Reduce();
}

Rational WeylGroup::GetKillingDivTraceRatio()
{ Rational result=0;
  Rational tempRat;
  for (int i=0; i<this->RootSystem.size; i++)
  { tempRat=this->RootScalarCartanRoot(this->RootSystem[i], this->RootSystem[0]);
    result+=tempRat*tempRat;
  }
  result/=this->RootScalarCartanRoot(this->RootSystem[0], this->RootSystem[0]);
  return result;
}

int ParserNode::EvaluatePrintRootSystem
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  WeylGroup& theWeyl=theNode.owner->theHmm.theRange().theWeyl;
  out << "<br>Symmetric Cartan matrix follows."
  <<" The entry in the i-th row and j-th column defines the scalar product of the i^th and j^th Vectors<Rational>.<br>" <<
  CGI::GetHtmlMathDivFromLatexAddBeginARCL
  (theNode.owner->theHmm.theRange().theWeyl.CartanSymmetric.ElementToString(false, true) );
  Rational tempRat;
  Matrix<Rational>  tempMat;
  tempMat = theWeyl.CartanSymmetric;
  tempMat.ComputeDeterminantOverwriteMatrix(tempRat);
  out  << "<br>The determinant of the symmetric Cartan matrix is: " << tempRat.ElementToString();
/*  Rational theRatio;
  for (int j=0; j<theWeyl.GetDim(); j++)
  { theRatio=0;
    for (int i=0; i<theWeyl.RootSystem.size; i++)
    { Rational tempRat=theWeyl.RootScalarCartanRoot(theWeyl.RootSystem[i], theWeyl.RootSystem[j]);
      theRatio+=tempRat*tempRat;
    }
    theRatio.Invert();
    theRatio*=theWeyl.RootScalarCartanRoot(theWeyl.RootSystem[j], theWeyl.RootSystem[j]);
    Rational tempRat=theWeyl.GetKillingDivTraceRatio();
    tempRat.Invert();
//    std::cout << "<br>" << j+1 << ": " << theRatio.ElementToString() << "=? " << tempRat.ElementToString();
  }*/
  //Lattice tempLattice;
  //theWeyl.GetIntegralLatticeInSimpleCoordinates(tempLattice);
  //out << "<br>The integral lattice in simple coordinates is (generated by): " << tempLattice.ElementToString(true, false);
  //Vectors<Rational> integralRoots, integralRootsEpsForm;
  Vectors<Rational> fundamentalWeights, fundamentalWeightsEpsForm;
  //integralRoots.AssignMatrixRows(tempLattice.basisRationalForm);
  //theWeyl.GetEpsilonCoords(integralRoots, integralRootsEpsForm, theGlobalVariables);
  //out << "<br>The integral lattice generators in epsilon format: " << integralRootsEpsForm.ElementToStringEpsilonForm();
  theWeyl.GetFundamentalWeightsInSimpleCoordinates(fundamentalWeights);
  Vectors<Rational> simpleBasis, simplebasisEpsCoords;
  out << "<hr> Half sum of positive Vectors<Rational>: " << theWeyl.rho.ElementToString();
  Vector<Rational> tempRoot;
  theWeyl.GetEpsilonCoords(theWeyl.rho, tempRoot, theGlobalVariables);
  out << "= " << CGI::GetHtmlMathSpanFromLatexFormula(tempRoot.ElementToStringEpsilonForm(true, false));
  out << "<hr>Size of Weyl group according to formula: " <<
  theWeyl.GetSizeWeylByFormula(theWeyl.WeylLetter, theWeyl.GetDim()).ElementToString();
  out << "<hr>Simple basis (regular and epsilon coordinates): <table>";
  simpleBasis.MakeEiBasis(theWeyl.GetDim());
  theWeyl.GetEpsilonCoords(simpleBasis, simplebasisEpsCoords, theGlobalVariables);
  for (int i=0; i< simplebasisEpsCoords.size; i++)
  { std::stringstream tempStream;
    tempStream << "\\omega_{" << i+1 << "}:=";
    out << "<tr><td>" << CGI::GetHtmlMathSpanFromLatexFormula(tempStream.str())
    << simpleBasis[i].ElementToString() << " </td><td>=</td> <td>"
    << CGI::
    GetHtmlMathFromLatexFormulA
    (simplebasisEpsCoords[i].ElementToStringEpsilonForm(true, false), "", "</td><td>", false, false)
    << "</td></tr>";
  }
  out << "</table>";
  out << "<hr>The fundamental weights (the j^th fundamental weight is the weight that has scalar product 1 with the j^th simple Vector<Rational> times 2 divided by the Vector<Rational> length squared, and 0 with the remaining simple Vectors<Rational>): ";
  theWeyl.GetEpsilonCoords(fundamentalWeights, fundamentalWeightsEpsForm, theGlobalVariables);
  out << "<table>";
  for (int i=0; i< fundamentalWeights.size; i++)
  { out << "<tr><td>" << fundamentalWeights[i].ElementToString() << "</td><td> =</td><td> "
    << CGI::
    GetHtmlMathFromLatexFormulA
    (fundamentalWeightsEpsForm[i].ElementToStringEpsilonForm(true, false), "", "</td><td>", false, false)
    << "</td></tr>";
  }
  out << "</table>";
  out << "<hr>Root system:<table><tr><td>Simple basis coordinates</td><td></td><td>Epsilon coordinates (convention: Humphreys)</td></tr> ";
  Vectors<Rational> rootSystemEpsCoords;
  theWeyl.GetEpsilonCoords(theWeyl.RootSystem, rootSystemEpsCoords, theGlobalVariables);
  for (int i=0; i<theWeyl.RootSystem.size; i++)
  { Vector<Rational>& current=theNode.owner->theHmm.theRange().theWeyl.RootSystem.TheObjects[i];
    out << "<tr><td>" << current.ElementToString() << "</td><td>=</td><td>"
    << rootSystemEpsCoords[i].ElementToStringLetterFormat("e", false)
    << "</td></tr>";
  }
  out << "</table>";
  DrawingVariables theDV;
  theWeyl.DrawRootSystem(theDV, true, theGlobalVariables, true, 0, true, 0);
  out << "<hr>Below a drawing of the Vector<Rational> system in its corresponding Coxeter plane "
  << "(computed as explained on John Stembridge's website). "
  << "<br>The darker red dots can be dragged with the mouse to rotate the picture."
  << "<br>The grey lines are the edges of the Weyl chamber."
  << theDV.GetHtmlFromDrawOperationsCreateDivWithUniqueName(theWeyl.GetDim());
  theNode.outputString=out.str();
  theNode.ExpressionType=theNode.typeString;
  return theNode.errorNoError;
}

template<class CoefficientType>
void Vectors<CoefficientType>::IntersectTwoLinSpaces
  (const Vectors<CoefficientType>& firstSpace, const Vectors<CoefficientType>& secondSpace,
   Vectors<CoefficientType>& output, GlobalVariables& theGlobalVariables, const CoefficientType& theRingZero
   )
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
  output.Reserve(tempSel.CardinalitySelection);
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
    output.AddOnTop(nextIntersection);
  }
  //std::cout << "<br> final output: " << output.ElementToString();
  //std::cout << "<br>******************End of debugging linear space intersections";
}

int ParserNode::EvaluateIntersectLatticeWithPreimageLattice
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ Vectors<Rational> theLinearMap;
  Lattice& firstLattice=theNode.owner->TheObjects[theArgumentList.TheObjects[0]].theLattice.GetElement();
  Lattice& secondLattice=theNode.owner->TheObjects[theArgumentList.TheObjects[1]].theLattice.GetElement();
  List<int> theMapTempList;
  theMapTempList.AddOnTop(theArgumentList.TheObjects[2]);
  int theDim;
  if (!theNode.owner->TheObjects[theArgumentList.TheObjects[2]].GetRootsEqualDimNoConversionNoEmptyArgument(theMapTempList, theLinearMap, theDim))
    return theNode.SetError(theNode.errorDimensionProblem);
  Matrix<Rational>  theLinearMapMat;
  theLinearMapMat.AssignRootsToRowsOfMatrix(theLinearMap);
  if(theLinearMapMat.NumCols!=firstLattice.GetDim() || theLinearMapMat.NumRows!=secondLattice.GetDim())
    return theNode.SetError(theNode.errorDimensionProblem);
  //theLinearMapMat.Transpose();

  theNode.theLattice.GetElement()=firstLattice;
  theNode.theLattice.GetElement().IntersectWithPreimageOfLattice(theLinearMapMat, secondLattice, theGlobalVariables);
  theNode.ExpressionType=theNode.typeLattice;
  return theNode.errorNoError;
}

void Cone::IntersectAHyperplane
  (Vector<Rational>& theNormal, Cone& outputConeLowerDim, GlobalVariables& theGlobalVariables)
{ assert(!theNormal.IsEqualToZero());
  int theDimension=theNormal.size;
  Matrix<Rational>  tempMat, theEmbedding, theProjection;
  tempMat.AssignVectorRow(theNormal);
  Vectors<Rational> theBasis;
  tempMat.FindZeroEigenSpace(theBasis);
  assert(theBasis.size==theNormal.size-1);
  theEmbedding.AssignRootsToRowsOfMatrix(theBasis);
  theEmbedding.Transpose();
  theBasis.AddOnTop(theNormal);
  Vectors<Rational> tempRoots, tempRoots2, tempRoots3;
  Matrix<Rational> tempMat2;
  tempRoots.MakeEiBasis(theDimension);
  tempRoots.GetCoordsInBasis(theBasis, tempRoots2, tempRoots3, tempMat2);
  theProjection.AssignRootsToRowsOfMatrix(tempRoots2);
  theProjection.Transpose();
  theProjection.Resize(theDimension-1, theDimension, false);
  Vectors<Rational> newNormals=this->Normals;
  theProjection.ActOnVectorsColumn(newNormals);
  bool tempBool=outputConeLowerDim.CreateFromNormals(newNormals, theGlobalVariables);
  assert(!tempBool);

}

bool Cone::GetRootFromLPolyConstantTermGoesToLastVariable
(Polynomial<Rational>& inputLPoly, Vector<Rational>& output)
{ if (!inputLPoly.IsLinear())
    return false;
  output.MakeZero(inputLPoly.NumVars+1);
  for (int i=0; i<inputLPoly.size; i++)
  { int theIndex;
    if (inputLPoly[i].::MonomialP::IsOneLetterFirstDegree(theIndex))
      output[theIndex]=inputLPoly.theCoeffs[i];
    else
      *output.LastObject()=inputLPoly.theCoeffs[i];
  }
  return true;
}

bool Cone::SolveLPolyEqualsZeroIAmProjective
  ( Polynomial<Rational> & inputLPoly,
   Cone& outputCone, GlobalVariables& theGlobalVariables
   )
{ Vector<Rational> theNormal;
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
  return theNode.ConvertToType(theType, theNode.impliedNumVars, theGlobalVariables);
}

bool ParserFunctionArgumentTree::ConvertArguments
  (ParserNode& theNode, List<int>& outputArgumentIndices, GlobalVariables& theGlobalVariables)
{ if (theNode.ExpressionType==theNode.typeArray)
    outputArgumentIndices=theNode.children;
  else
  { outputArgumentIndices.size=0;
    outputArgumentIndices.AddOnTop(theNode.indexInOwner);
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
    case ParserNode::typeWeylAlgebraElement: return "WeylAlgebraElement";
    case ParserNode::typeRationalFunction: return "RF";
    case ParserNode::typePartialFractions: return "PF";
    case ParserNode::typeLattice: return "Lattice";
    case ParserNode::typeUEElementOrdered: return "UEOrdered";
    case ParserNode::typeUEelement: return "UE";
    case ParserNode::typeDots: return "...";
    case ParserNode::typeArray: return "";
    case ParserNode::typeLittelman: return "LittelmannPath";
    case ParserNode::typeCharSSFDMod: return "Char";
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
  if (theArgument=="WeylAlgebraElement")
    return ParserNode::typeWeylAlgebraElement;
  if (theArgument=="PF")
    return ParserNode::typePartialFractions;
  if (theArgument=="Lattice")
    return ParserNode::typeLattice;
  if (theArgument=="RF")
    return ParserNode::typeRationalFunction;
  if (theArgument=="UEOrdered")
    return ParserNode::typeUEElementOrdered;
  if (theArgument=="LittelmannPath")
    return ParserNode::typeLittelman;
  if (theArgument=="UE")
    return ParserNode::typeUEelement;
  if (theArgument=="...")
    return ParserNode::typeDots;
  if (theArgument=="Char")
    return ParserNode::typeCharSSFDMod;
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
      { this->functionArguments.AddOnTop(this->GetTokenFromArgumentStringChangeInput(currentArgument));
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
    { this->functionArguments.AddOnTop(ParserNode::typeArray);
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
  FormatExpressions theFormat;
  theNode.outputString=theNode.theCone.GetElement().ElementToString(false, true, theFormat);
  return theNode.errorNoError;
}

int ParserNode::EvaluateDrawConeAffine
(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ theGlobalVariables.theDrawingVariables.theBuffer.init();
  FormatExpressions theFormat;
  theNode.outputString= theNode.owner->TheObjects[theArgumentList[0]].theCone.GetElement().DrawMeToHtmlLastCoordAffine(theGlobalVariables.theDrawingVariables, theFormat);
  theNode.ExpressionType=theNode.typeString;
  return theNode.errorNoError;
}

int ParserNode::EvaluateDrawConeProjective
(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ theGlobalVariables.theDrawingVariables.theBuffer.init();
  FormatExpressions theFormat;
  theNode.outputString= theNode.owner->TheObjects[theArgumentList[0]].theCone.GetElement().DrawMeToHtmlProjective(theGlobalVariables.theDrawingVariables, theFormat);
  theNode.ExpressionType=theNode.typeString;
  return theNode.errorNoError;
}

void rootSubalgebras::MakeProgressReportGenerationSubalgebras(rootSubalgebras& bufferSAs, int RecursionDepth, GlobalVariables& theGlobalVariables, int currentIndex, int TotalIndex)
{ if (theGlobalVariables.GetFeedDataToIndicatorWindowDefault()==0)
    return;
  for (int i=0; i<=RecursionDepth; i++)
  { int lineIndex=i/3;
    theGlobalVariables.theIndicatorVariables.ProgressReportStrings.SetSize(MathRoutines::Maximum(lineIndex+1, 5));
    std::string& currentLine=theGlobalVariables.theIndicatorVariables.ProgressReportStrings[lineIndex];
    if (i%3==0)
      currentLine="";
    currentLine+= bufferSAs.TheObjects[i].theDynkinDiagram.DynkinStrinG;
    if (i!=RecursionDepth)
      currentLine+=": ";
  }
  theGlobalVariables.theIndicatorVariables.ProgressReportStrings.SetSize(MathRoutines::Maximum(theGlobalVariables.theIndicatorVariables.ProgressReportStrings.size+1, 5));
  std::stringstream out;
  out << "Included Vector<Rational> " << currentIndex+1 << " out of " << TotalIndex << " Total found SAs: " << this->size;
  *theGlobalVariables.theIndicatorVariables.ProgressReportStrings.LastObject()=out.str();
  theGlobalVariables.FeedIndicatorWindow(theGlobalVariables.theIndicatorVariables);
}

int ParserNode::EvaluatePrintRootSAsAndSlTwos
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables, bool redirectToSlTwos,
   bool forceRecompute)
{ std::stringstream outSltwoPath, outMainPath, out, outSltwoDisplayPath, outMainDisplayPath;
  CGI::SetCGIServerIgnoreUserAbort();
  theGlobalVariables.MaxAllowedComputationTimeInSeconds=10000;
  char weylLetter=theNode.owner->DefaultWeylLetter;
  int theRank=theNode.owner->DefaultWeylRank;
  outMainPath << theNode.owner->PhysicalPathOutputFolder <<  weylLetter << theRank << "/";
  outMainDisplayPath << theNode.owner->DisplayPathOutputFolder << weylLetter << theRank << "/";
  outSltwoPath << outMainPath.str() << "sl2s/";
  outSltwoDisplayPath << outMainDisplayPath.str() << "sl2s/";
  bool NeedToCreateFolders=(!CGI::FileExists(outMainPath.str()) || !CGI::FileExists(outSltwoPath.str()));
  if (NeedToCreateFolders)
  { std::stringstream outMkDirCommand1, outMkDirCommand2;
    outMkDirCommand1 << "mkdir " << outMainPath.str();
    outMkDirCommand2 << "mkdir " << outSltwoPath.str();
    theNode.owner->SystemCommands.AddOnTop(outMkDirCommand1.str());
    theNode.owner->SystemCommands.AddOnTop(outMkDirCommand2.str());
    out << "<br><br>... Created the missing folders for the database. <b> Running  a second time... Please wait for automatic redirection."
    <<  " Clicking back/refresh button in the browser will cause broken links in the calculator due to a technical "
    << "(Apache server configuration) problem.</b><br><br>"
    << "<META http-equiv=\"refresh\" content=\"3; url="
    << theNode.owner->DisplayNameCalculator << "?"
    << " textType=" << weylLetter << "&textDim=" << theRank << "&textInput=";
    out << CGI::UnCivilizeStringCGI(theNode.owner->StringBeingParsed);
    out << "\">  Redirecting in 3 seconds";
    theNode.outputString=out.str();
    theNode.ExpressionType=theNode.typeString;
    return theNode.errorNoError;
  }
  std::stringstream outRootHtmlFileName, outRootHtmlDisplayName, outSltwoMainFile, outSltwoFileDisplayName;
  outSltwoMainFile << outSltwoPath.str() << "sl2s.html";
  outSltwoFileDisplayName << outSltwoDisplayPath.str() << "sl2s.html";
  outRootHtmlFileName << outMainPath.str() << "rootHtml.html";
  outRootHtmlDisplayName << outMainDisplayPath.str() << "rootHtml.html";
  bool mustRecompute=forceRecompute;
  if (!CGI::FileExists(outSltwoMainFile.str()) || !CGI::FileExists(outRootHtmlFileName.str()))
    mustRecompute=true;
  bool showIndicator=theNode.owner->flagDisplayIndicator;
  if (mustRecompute)
  { if (showIndicator)
    { out << "<br>The computation is in progress. <b><br>Please do not click back/refresh button: it will cause broken links in the calculator. <br>Appologies for this technical (Apache server configuration) problem. <br>Hope to alleviate it soon.</b>"
      << "<br>Below is an indicator for the progress of the computation."
      << "<br> The computation is slow, up to around 10 minutes for E_8."
      << "<br>Once it is done, you should be redirected automatically to the result page."
      << "<br>To go back to the calculator main page use the back button on your browser.";
      std::stringstream afterSystemCommands;
      std::stringstream noindicatorLink;
      afterSystemCommands << "<META http-equiv=\"refresh\" content=\"0; url=";
      noindicatorLink << theNode.owner->DisplayNameCalculator << "?"
      << " textType=" << weylLetter << "&textDim=" << theRank << "&textInput="
      << CGI::UnCivilizeStringCGI(theNode.owner->StringBeingParsed+ " NoIndicator");
      afterSystemCommands << noindicatorLink.str() << "\">";
      theNode.owner->afterSystemCommands=afterSystemCommands.str();
      out << "\n<br>\nComputing ...<br>" << theNode.owner->javaScriptDisplayingIndicator;
      theGlobalVariables.ClearIndicatorVars();
      theGlobalVariables.MakeReport();
      theNode.outputString=out.str();
      theNode.ExpressionType=theNode.typeString;
      return theNode.errorNoError;
    } else
    { SltwoSubalgebras theSl2s;
      theSl2s.owner[0].FindSl2Subalgebras(theSl2s, weylLetter, theRank, theGlobalVariables);
      std::string PathSl2= outSltwoPath.str(); std::string DisplayPathSl2=outSltwoDisplayPath.str();
      theSl2s.ElementToHtml
      (theGlobalVariables, theSl2s.owner[0].theWeyl, true, PathSl2, DisplayPathSl2, theNode.owner->DisplayNameCalculator);
      theNode.owner->SystemCommands.AddListOnTop(theSl2s.listSystemCommandsLatex);
      theNode.owner->SystemCommands.AddListOnTop(theSl2s.listSystemCommandsDVIPNG);
    }
  }
//  std::stringstream afterSystemCommands;
  out << "<META http-equiv=\"refresh\" content=\"0; url=";
  if (!redirectToSlTwos)
    out << outRootHtmlDisplayName.str();
  else
    out << outSltwoFileDisplayName.str();
  out << "\">";
  //theNode.owner->afterSystemCommands=afterSystemCommands.str();
  theNode.outputString=out.str();
  theNode.ExpressionType=theNode.typeString;
//  theGlobalVariables.MaxAllowedComputationTimeInSeconds=oldMaxAllowedComputationTimeInSeconds;
  return theNode.errorNoError;
}

std::string Parser::GetFunctionDescription()
{ std::stringstream out;
  for (int i=0; i<this->theFunctionList.size; i++)
    if (this->theFunctionList.TheObjects[i].flagVisible)
      out << this->theFunctionList.TheObjects[i].ElementToString(true, false, *this) << " <br>";
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
  out << ")";
  return out.str();
}

bool CGI::GetHtmlStringSafeishReturnFalseIfIdentical(const std::string& input, std::string& output)
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

std::string CGI::UnCivilizeStringCGI(const std::string& input)
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
      case '=': out << "%3D"; break;
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

std::string ParserFunction::ElementToString(bool useHtml, bool useLatex, Parser& owner)const
{ std::stringstream out;
  out << "<div style=\"display: inline\" id=\"functionBox" << this->functionName << "\" >";
  out << this->functionName;
  out << this->theArguments.ElementToString(useHtml, useLatex);
  out <<  "<button" << CGI::GetStyleButtonLikeHtml() << " onclick=\"switchMenu('fun" << this->functionName
  << "');\">More/less info</button><div id=\"fun" << this->functionName
  << "\" style=\"display: none\"><br>" << this->functionDescription << "<br>Example. <a href=\"" << owner.DisplayNameCalculator  << "?"
  << " textType=" << this->exampleAmbientWeylLetter << "&textDim=" << this->exampleAmbientWeylRank << "&textInput="
  << CGI::UnCivilizeStringCGI(this->example)
  << "\"> " << this->example << "</a></div></div>";
  return out.str();
}

std::string GeneralizedVermaModuleCharacters::PrepareReport(GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  FormatExpressions theFormat;
  int tempI=0;
  Vector<Rational> tempRoot;
  theFormat.polyAlphabeT.SetSize(5);
  theFormat.polyAlphabeT[tempI]="x_1"; tempI++;
  theFormat.polyAlphabeT[tempI]="x_2"; tempI++;
  theFormat.polyAlphabeT[tempI]="y_1"; tempI++;
  theFormat.polyAlphabeT[tempI]="y_2"; tempI++;
  theFormat.polyAlphabeT[tempI]="y_3"; tempI++;
  out << "\\documentclass{article}\\usepackage{amsmath, longtable, amsfonts, amssymb, verbatim, hyperref}"
  << "\n\\begin{document}\\tiny\n";
  out << "\n The chamber complex + multiplicities follow.\n\n\n"
  << "\\begin{longtable}{cc}\\caption{Multiplicities of generalized Verma modules $m(x_1,x_2, y_1, y_2, y_3)$"
  << " for $\\gop$ with Dynkin diagram";
  std::stringstream tempStream;
  tempStream << "(";
  for (int i=0; i<this->ParabolicLeviPartRootSpacesZeroStandsForSelected.MaxSize; i++)
  { if (this->ParabolicLeviPartRootSpacesZeroStandsForSelected.selected[i])
      tempStream << "+";
    else
      tempStream << "0";
    if (i!=this->ParabolicLeviPartRootSpacesZeroStandsForSelected.MaxSize-1)
      tempStream << ",";
  }
  tempStream << ")";
  out << "$" << tempStream.str() <<  "$ \\label{table" << tempStream.str() << "}}\\\\\n";
  out << "Inequlities& $m(x_1,x_2, y_1, y_2, y_3)$\\endhead\n";
  int numFoundChambers=0;
  List<int> DisplayIndicesprojectivizedChambers;
  for (int i=0; i<this->projectivizedChambeR.size; i++)
  { QuasiPolynomial& theMult=this->theMultiplicities[i];
    if (!theMult.IsEqualToZero())
    { numFoundChambers++;
      out << "\\hline\\multicolumn{2}{c}{Chamber " << numFoundChambers << "}\\\\\n";
      DisplayIndicesprojectivizedChambers.AddOnTop(numFoundChambers);
      out << this->PrepareReportOneCone(theFormat, this->projectivizedChambeR[i], theGlobalVariables) << "&";
      out << "\\begin{tabular}{c}";
      out << theMult.ElementToString(false, true, &theFormat) << "\\end{tabular}\\\\\n";
    } else
      DisplayIndicesprojectivizedChambers.AddOnTop(-1);
  }
  out << "\\end{longtable}\n\n\n Multiplicity free chambers \n";
  numFoundChambers=0;
  out << "\n\\begin{longtable}{cc} ";
  out << "Normals& Multiplicity of module with highest weight $(x_1,x_2)$\\endhead\n";
 /* for (int i=0; i<this->projectivezedChambersSplitByMultFreeWalls.size; i++)
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
  }*/
  out << "Total number of chambers with multiplicity 1 or less: " << numFoundChambers;
  out << "\\end{longtable}\n\n\n\n";
  out << "\\end{document}";
  return out.str();
}

std::string GeneralizedVermaModuleCharacters::PrepareReportOneCone
  (FormatExpressions& theFormat, Cone& theCone, GlobalVariables& theGlobalVariables)
{ std::stringstream out1;
  std::string tempS1, tempS2;
  Vector<Rational> normalNoConstant;
  int dimSmallerAlgebra=this->theLinearOperators.TheObjects[0].NumRows;
  int dimLargerAlgebra=this->theLinearOperators.TheObjects[0].NumCols;
  Rational theConst;
  out1 << "\\begin{tabular}{rcl}";
  for (int i=0; i<theCone.Normals.size; i++)
  { Vector<Rational>& currentNormal=theCone.Normals.TheObjects[i];
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

Vector<Rational> oneFracWithMultiplicitiesAndElongations::GetCheckSumRoot(int NumVars)
{ Vector<Rational> output;
  output.SetSize(NumVars);
  for (int i=0; i<NumVars; i++)
    output.TheObjects[i]=i+2;
  return output;
}

bool partFractions::RemoveRedundantShortRootsIndex
(MonomialCollection<partFraction, Polynomial<LargeInt> >& output, int theIndex, GlobalVariables& theGlobalVariables,
 Vector<Rational>* Indicator)
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
  partFraction thePF;
  Rational localStartCheckSum, localEndCheckSum;
  std::string tempS, tempS1, tempS2;
  Polynomial<LargeInt> tempIP, currentCoeff;
  this->PopMonomial(theIndex, thePF, currentCoeff);
  for (int k=0; k<thePF.IndicesNonZeroMults.size; k++)
  { int currentIndex=thePF.IndicesNonZeroMults.TheObjects[k];
    oneFracWithMultiplicitiesAndElongations& currentFrac = thePF.TheObjects[currentIndex];
    int LCMElongations = currentFrac.GetLCMElongations();
    tempS= this->startingVectors[currentIndex].ElementToString();
    while (currentFrac.Elongations.size>1)
    { for (int i=0; i<currentFrac.Elongations.size; i++)
      { int ElongationValue=currentFrac.Elongations[i];
        if (ElongationValue!=LCMElongations)
        { int numSummands=LCMElongations/ElongationValue;
          thePF.GetNElongationPoly
          (this->startingVectors, currentIndex, ElongationValue, numSummands, tempIP, this->AmbientDimension);
          //tempIP.ComputeDebugString();
          tempIP.RaiseToPower(currentFrac.Multiplicities.TheObjects[i], (LargeInt) 1);
          //tempIP.ComputeDebugString();
          currentCoeff*=tempIP;
//          ComputationalBufferCoefficientNonExpanded.ComputeDebugString();
          currentFrac.AddMultiplicity(currentFrac.Multiplicities.TheObjects[i], LCMElongations);
          currentFrac.AddMultiplicity(-currentFrac.Multiplicities.TheObjects[i], ElongationValue);
//          thePF.ComputeOneCheckSum(*this, localEndCheckSum, this->AmbientDimension, theGlobalVariables);
          assert(localEndCheckSum.IsEqualTo(localStartCheckSum));
        }
      }
    }
    if (partFraction::MakingConsistencyCheck || this->flagAnErrorHasOccurredTimeToPanic)
    { //thePF.ComputeOneCheckSum(*this, localEndCheckSum, this->AmbientDimension, theGlobalVariables);
      //localEndCheckSum.ElementToString(tempS1);
      //assert(localStartCheckSum.IsEqualTo(localEndCheckSum));
    }
  }
  this->AddMonomial(thePF, currentCoeff);
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
  List<Polynomial<Rational> > outputList;
  std::string tempS;
  if (! theSl2.ComputeInvariantsOfDegree(thePartition, theDegree, outputList, tempS, theGlobalVariables))
  { theNode.outputString=tempS;
    return theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);
  }
  theNode.ExpressionType=theNode.typeString;
  std::stringstream out;
  out << "A basis for the invariants of degree " << theDegree << " is given by (number of elements: " << outputList.size << ")";
  for (int i=0; i<outputList.size; i++)
  { out << "<br>" << outputList.TheObjects[i].ElementToString(&theGlobalVariables.theDefaultPolyFormat) << ", ";
  }
  theNode.outputString=out.str();
  return theNode.errorNoError;
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
      theGlobalVariables->theIndicatorVariables.ProgressReportStrings[0]=out.str();
      theGlobalVariables->theIndicatorVariables.ProgressReportStringsNeedRefresh=true;
      theGlobalVariables->MakeReport();
    }*/
  }
}

void Lattice::GetRougherLatticeFromAffineHyperplaneDirectionAndLattice
  (const Vector<Rational>& theDirection, Vector<Rational>& outputDirectionMultipleOnLattice, Vector<Rational>& theShift, Vector<Rational>& theAffineHyperplane,
   Vectors<Rational>& outputRepresentatives,
   Vectors<Rational>& movementInDirectionPerRepresentative,
   Lattice& outputRougherLattice,
   GlobalVariables& theGlobalVariables)
{ Vector<Rational> theNormal=theAffineHyperplane;
  theNormal.SetSize(theNormal.size-1);
  if (theDirection.ScalarEuclidean(theNormal).IsEqualToZero())
    return;
  Rational theConstOnTheOtherSide=-*theAffineHyperplane.LastObject();
  Vectors<Rational> theBasis;
  theBasis.AssignMatrixRows(this->basisRationalForm);
  Lattice theHyperplaneLatticeNoShift, theDirectionLattice;//, normalProjectionLattice, theTrueProjectionLattice;
  Vectors<Rational> tempRoots; //Vector<Rational> tempRoot;
  tempRoots.AddOnTop(theDirection);
  theDirectionLattice=*this;
  theDirectionLattice.IntersectWithLinearSubspaceSpannedBy(tempRoots);
  theDirectionLattice.basisRationalForm.RowToRoot(0, outputDirectionMultipleOnLattice);
  //std::cout << "<br>the normal: " << theNormal.ElementToString();
  //std::cout << "<br> the direction lattice: " << theDirectionLattice.ElementToString();
  theHyperplaneLatticeNoShift=*this;
  theHyperplaneLatticeNoShift.IntersectWithLinearSubspaceGivenByNormal(theNormal);
  //std::cout << "<br>the non-affine hyperplane intersected with the lattice: " << theHyperplaneLatticeNoShift.ElementToString();
  tempRoots.AssignMatrixRows(theHyperplaneLatticeNoShift.basisRationalForm);
  tempRoots.AddOnTop(outputDirectionMultipleOnLattice);
  outputRougherLattice.MakeFromRoots(tempRoots);
  this->GetAllRepresentatives(outputRougherLattice, outputRepresentatives);
  //std::cout << "<br>the rougher lattice: " << outputRougherLattice.ElementToString();
  //std::cout << "<br>representatives of the quotient of the two lattices: " << outputRepresentatives.ElementToString();
  for (int i=0; i<outputRepresentatives.size; i++)
  { outputRepresentatives.TheObjects[i]+=theShift;
    outputRougherLattice.ReduceVector(outputRepresentatives.TheObjects[i]);
  }
  Rational theShiftedConst, unitMovement, tempRat;
  unitMovement=theNormal.ScalarEuclidean(outputDirectionMultipleOnLattice);
  movementInDirectionPerRepresentative.SetSize(outputRepresentatives.size);
  //std::cout << "<br>Affine hyperplane per representative: ";
  for (int i=0; i<outputRepresentatives.size; i++)
  { tempRat=(theNormal.ScalarEuclidean(outputRepresentatives.TheObjects[i]) - theConstOnTheOtherSide)/unitMovement;
    tempRat.AssignFracValue();
    theShiftedConst=theConstOnTheOtherSide + tempRat;
    Vector<Rational>& currentMovement=movementInDirectionPerRepresentative.TheObjects[i];
    currentMovement=theAffineHyperplane;
    *currentMovement.LastObject()=theShiftedConst;
    //std::cout << "<br>Representative: " << outputRepresentatives.TheObjects[i].ElementToString() << " and the hyperplane: " << currentMovement.ElementToString();
  }
//  std::cout << "<hr>"
}

int ParserNode::EvaluateClosestPointToHyperplaneAlongTheNormal
(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ Lattice& currentLattice=theNode.owner->TheObjects[theArgumentList.TheObjects[3]].theLattice.GetElement();
  ParserNode& theHyperplaneNode=theNode.owner->TheObjects[theArgumentList.TheObjects[2]];
  ParserNode& theDirectionNode=theNode.owner->TheObjects[theArgumentList.TheObjects[0]];
  ParserNode& theShiftNode=theNode.owner->TheObjects[theArgumentList.TheObjects[1]];
  //ParserNode& theRayNode=theNode.owner->TheObjects[theArgumentList.TheObjects[2]];
  Vector<Rational> theAffineNormal, theRay, theRayMultiple, theShift;//, theRay;
  if (!theHyperplaneNode.GetRootRationalDontUseForFunctionArguments(theAffineNormal, theGlobalVariables))
    return theNode.SetError(theNode.errorProgramming);
  if (!theDirectionNode.GetRootRationalDontUseForFunctionArguments(theRay, theGlobalVariables))
    return theNode.SetError(theNode.errorProgramming);
  if (!theShiftNode.GetRootRationalDontUseForFunctionArguments(theShift, theGlobalVariables))
    return theNode.SetError(theNode.errorProgramming);
//  if (!theRayNode.GetRootRational(theRay, theGlobalVariables))
//    return theNode.SetError(theNode.errorProgramming);
  //int theDim=t;
  if (theAffineNormal.size-1!=currentLattice.GetDim() || currentLattice.GetRank()!=currentLattice.GetDim() || theRay.size!=currentLattice.GetDim() || theRay.size!=theShift.size)
    return theNode.SetError(theNode.errorDimensionProblem);
  Vectors<Rational> representatives, theMovements;
  Lattice theRougherLattice;
  currentLattice.GetRougherLatticeFromAffineHyperplaneDirectionAndLattice
  (theRay, theRayMultiple, theShift, theAffineNormal, representatives, theMovements, theRougherLattice, theGlobalVariables);
  return theNode.errorNoError;
}

bool slTwoInSlN::ComputeInvariantsOfDegree
  (List<int>& decompositionDimensions, int theDegree, List<Polynomial<Rational> >& output, std::string& outputError, GlobalVariables& theGlobalVariables)
{ this->initFromModuleDecomposition(decompositionDimensions, false, false);
  SelectionWithMaxMultiplicity theSel;
  theSel.initMaxMultiplicity(this->theDimension, theDegree);
  FormatExpressions PolyFormatLocal;
  outputError="";
  int numCycles=theSel.NumCombinationsOfCardinality(theDegree);
  if (numCycles<=0)
  { outputError= " Computation too large. ";
    return false;
  }
  Polynomial<Rational>  basisMonsZeroWeight, basisMonsAll;
  basisMonsZeroWeight.MakeZero(this->theDimension);
  basisMonsZeroWeight.SetExpectedSize(numCycles);
  basisMonsAll.MakeZero(this->theDimension);
  basisMonsAll.SetExpectedSize(numCycles);
  MonomialP theMon;
  theMon.MakeZero(this->theDimension);
  Vector<Rational> theWeight;
  Vector<Rational> theCartanAction;
  theCartanAction.SetSize(this->theDimension);
  theWeight.SetSize(this->theDimension);
  for (int j=0; j<this->theDimension; j++)
    theCartanAction.TheObjects[j]=this->theH.elements[j][j];
  theSel.IncrementSubsetFixedCardinality(theDegree);
  Rational theMonCoeff=1;
  for (int i=0; i<numCycles; i++, theSel.IncrementSubsetFixedCardinality(theDegree))
  { for (int j=0; j<this->theDimension; j++)
    { theMon[j]=theSel.Multiplicities[j];
      theWeight[j]=theMon[j];
    }
    basisMonsAll.AddMonomial(theMon, theMonCoeff);
    if (Vector<Rational>::ScalarEuclidean(theWeight, theCartanAction).IsEqualToZero())
      basisMonsZeroWeight.AddMonomial(theMon, theMonCoeff);
  }
//  std::cout << "<br>Num cycles:" << numCycles << "<br>The basis mons (there are " << basisMonsZeroWeight.size << " of them): "  << basisMonsZeroWeight.ElementToString(PolyFormatLocal);
  Matrix<Rational>  tempMat;
  tempMat.init(basisMonsAll.size*2, basisMonsZeroWeight.size);
//  tempMat.init(basisMonsAll.size*numGenerators, basisMonsZeroWeight.size);
  Polynomial<Rational>  tempP;
  for (int l=0; l<2; l++)
    for (int k=0; k<basisMonsZeroWeight.size; k++)
    { if (l==0)
        this->theE.ActOnMonomialAsDifferentialOperator(basisMonsZeroWeight[k], tempP);
      else
        this->theF.ActOnMonomialAsDifferentialOperator(basisMonsZeroWeight[k], tempP);
      for (int j=0; j<basisMonsAll.size; j++)
      { int indexInResult=tempP.GetIndex(basisMonsAll[j]);
        int currentRow=l*basisMonsAll.size+j;
        if (indexInResult==-1)
          tempMat.elements[currentRow][k]=0;
        else
          tempMat.elements[currentRow][k]=tempP.theCoeffs[indexInResult];
      }
    }
//  if (tempMat.NumRows<120)
//    std::cout << "<div class=\"math\" scale=\"50\">" << tempMat.ElementToString(false, true) << "</div>";
  Vectors<Rational> tempRoots;
  tempMat.FindZeroEigenSpace(tempRoots, theGlobalVariables);
  output.SetSize(tempRoots.size);
//  std::cout << "<br>invariants Vector<Rational> form: " << tempRoots.ElementToString();
//  std::cout << "<br> .... and the invariants are: ";
  for (int i=0; i<output.size; i++)
  { Polynomial<Rational>& current=output.TheObjects[i];
    current.MakeZero(this->theDimension);
    for (int j=0; j<basisMonsZeroWeight.size; j++)
      if (!tempRoots.TheObjects[i].TheObjects[j].IsEqualToZero())
      { theMon=(basisMonsZeroWeight[j]);
        current.AddMonomial(theMon, tempRoots[i][j]);
      }
 //   std::cout << "<br>Invariant " << i << ":<br>" << current.ElementToString(PolyFormatLocal);
  }
  return true;
}

void GeneralizedVermaModuleCharacters::InitTheMaxComputation
(GlobalVariables& theGlobalVariables)
{ this->theMaxComputation.numNonParaM=2;
  this->theMaxComputation.theConesLargerDim.Reserve(this->projectivizedChambeR.size);
  this->theMaxComputation.LPtoMaximizeLargerDim.Reserve(this->theMultiplicities.size);
  this->theMaxComputation.theConesLargerDim.SetSize(0);
  this->theMaxComputation.LPtoMaximizeLargerDim.SetSize(0);
  Lattice ZnLattice;
  int theAffineDim=5;
//  int theProjectivizedDim=theAffineDim+1;
  ZnLattice.MakeZn(theAffineDim);
  this->numNonZeroMults=0;
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  ConeLatticeAndShift currentCLS;
  Vector<Rational> theLPtoMax;
  for (int i=0; i<this->theMultiplicities.size; i++)
    if (! this->theMultiplicities[i].IsEqualToZero())
    { currentCLS.theProjectivizedCone=this->projectivizedChambeR.TheObjects[i];
      currentCLS.theShift.MakeZero(theAffineDim);
      currentCLS.theLattice=ZnLattice;
      bool tempBool= this->theMultiplicities[i].valueOnEachLatticeShift[0].GetRootFromLinPolyConstTermLastVariable(theLPtoMax, (Rational) 0);
      assert(tempBool);
      this->theMaxComputation.theConesLargerDim.AddOnTop(currentCLS);
      this->theMaxComputation.LPtoMaximizeLargerDim.AddOnTop(theLPtoMax);
      this->numNonZeroMults++;
      std::stringstream out;
      out << "Initialized " << i+1 << " out of " << this->theMaxComputation.theConesLargerDim.size
      << "; so far " << this->numNonZeroMults << " non-zero multiplicities";
      theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
      theGlobalVariables.MakeReport();
    }
}

std::string DrawingVariables::GetColorHtmlFromColorIndex(int colorIndex)
{ std::stringstream out;
  int r=(colorIndex/65536)%256;
  int g=(colorIndex/256)%256;
  int b=colorIndex%256;
  out << "#";
  if (r<16)
    out << 0;
  out << std::hex  << r;
  if (g<16)
    out << 0;
  out << std::hex  << g;
  if (b<16)
    out << 0;
  out << std::hex  << b;
  return out.str();
}

std::string ConeLatticeAndShift::ElementToString(FormatExpressions& theFormat)
{ std::stringstream out;
  out << this->theProjectivizedCone.ElementToString(false, true, true, true, theFormat);
  out << "<br>Shift+lattice: " << this->theShift.ElementToString() << " + " << this->theLattice.ElementToString();
  return out.str();
}

std::string SemisimpleLieAlgebra::ElementToString(FormatExpressions* theFormat)
{ std::stringstream out;
  std::string tempS;
  Vector<Rational> tempRoot;
  std::string beginMath;
  std::string endMath;
  bool usePNG=false;
  bool useRootNotation=false;
  bool useHtml=true;

  if (!usePNG)
  { beginMath="<DIV class=\"math\" scale=\"50\">\\begin{array}{";
    endMath="\\end{array}</DIV>";
  } else
  { beginMath="\\begin{tabular}{";
    endMath="\\end{tabular}";
  }
  int numRoots=this->theWeyl.RootSystem.size;
  int numPosRoots=this->theWeyl.RootsOfBorel.size;
  int theDimension = this->theWeyl.CartanSymmetric.NumRows;
  ElementSemisimpleLieAlgebra tempElt1, tempElt2, tempElt3;
//  out << beginMath << "\\begin{array}{ccc}a& a&a\\\\a&a&a\\end{array}";
  if (usePNG)
  { out << "\\begin{tabular}{cc}";
    out << "\\hline generator &corresponding Vector<Rational> space\\\\\\hline";
    for (int i=0; i<numRoots+theDimension; i++)
    { if (i==numPosRoots)
      { if (useRootNotation)
          out << "\\hline\\begin{tabular}{c}$"
          << theFormat->chevalleyHgeneratorLetter << "_\\alpha$:=$\\frac{\\langle\\alpha,\\alpha\\rangle}{2}["
          << theFormat->chevalleyGgeneratorLetter << "_{\\alpha},"
          << theFormat->chevalleyGgeneratorLetter << "_{-\\alpha}]$\\\\$"
          << theFormat->chevalleyHgeneratorLetter << "_\\alpha$ is dual to the Vector<Rational> $\\alpha$\\end{tabular} & 0 \\\\\\hline";
        else
          out << "\\hline\\begin{tabular}{c}$"
          << theFormat->chevalleyHgeneratorLetter << "_i$:=$\\frac{\\langle\\alpha_i,\\alpha_i\\rangle}{2}["
          << theFormat->chevalleyGgeneratorLetter << "_{i},"
          << theFormat->chevalleyGgeneratorLetter << "_{-i}]$\\\\$"
          << theFormat->chevalleyHgeneratorLetter << "_i$ is dual to the Vector<Rational> $\\alpha_i$\\end{tabular} & 0 \\\\\\hline";
//        out << "  \\\\\\hline";
        //out << "\\hline generator & corresponding Vector<Rational> space\\\\\\hline";
        i+=theDimension;
      }
      out << "$" << this->GetStringFromChevalleyGenerator(i, theFormat);
      int rootIndex=this->ChevalleyGeneratorIndexToRootIndex(i);
      out << "$&";
      out << this->theWeyl.RootSystem[rootIndex].ElementToString() << "\\\\";
    }
    out << "\\end{tabular}";
  }
  if (numRoots+theDimension<100)
  { for (int i=0; i<numRoots+theDimension+1; i++)
      beginMath+="c";
    beginMath+="}\n";
    out << beginMath;
    std::string tempHeader=out.str();
    if(usePNG)
      out << "$";
    out << "[\\bullet, \\bullet]\n";
    if(usePNG)
      out << "$";
    for (int i=0; i<numRoots+theDimension; i++)
    { tempElt1.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE
      (i, *this->owner, this->indexInOwner);
      tempS=tempElt1.ElementToString(theFormat);
      out << " & ";
      if(usePNG)
        out << "$";
      out << tempS;
      if(usePNG)
        out << "$";
    }
    out << "\\\\\n";
    if (useHtml)
      out << "<hr>";
    Rational tempRat;
    //int lineCounter=0;
    for (int i=0; i<theDimension+numRoots; i++)
    { tempElt1.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE
      (i,*this->owner, this->indexInOwner);
      tempS=tempElt1.ElementToString(theFormat);
      if(usePNG)
        out << "$";
      out << tempS;
      if(usePNG)
        out << "$";
      for (int j=0; j<numRoots+theDimension; j++)
      { tempElt2.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE
        (j, *this->owner, this->indexInOwner);
        this->LieBracket(tempElt1, tempElt2, tempElt3);
        tempS=tempElt3.ElementToString(theFormat);
        out << "& ";
        if(usePNG)
          out << "$";
        out << tempS;
        if(usePNG)
          out << "$";
      }
      out << "\\\\\n";
    }
    out << endMath;
  } else
    out << "\\begin{tabular}{c} table trimmed to avoid LaTeX/browser memory issues.\\end{tabular}";
  return out.str();
}

std::string ElementSemisimpleLieAlgebra::ElementToString(FormatExpressions* theFormat)const
{ std::stringstream out; std::string tempS;
  if (this->IsEqualToZero())
    out << "0";
  bool found=false;
  for (int i=0; i<this->size; i++)
    if (!this->GetOwner().IsGeneratorFromCartan(this->TheObjects[i].theGeneratorIndex))
    { tempS=this->theCoeffs[i].ElementToString();
      if (tempS=="1")
        tempS="";
      if (tempS=="-1")
        tempS="-";
      if (i!=0)
      { if (tempS!="")
        { if (tempS[0]!='-')
            out << "+";
        } else
          out << "+";
      }
      out << tempS << this->GetOwner().GetStringFromChevalleyGenerator(this->TheObjects[i].theGeneratorIndex, theFormat);
      found=true;
    }
  Vector<Rational> hComponent=this->GetCartanPart();
  if (!hComponent.IsEqualToZero())
  { std::string tempS=hComponent.ElementToStringLetterFormat("h", false);
    if (tempS[0]!='-' && found)
      out << "+";
    out << tempS;
  }
  return out.str();
}

void DrawOperations::MakeMeAStandardBasis(int theDim)
{ if (theDim<1)
    return;
  if (theDim>3)
  { this->ProjectionsEiVectors.SetSizeMakeMatrix(theDim, 2);
    for (int i=0; i<theDim; i++)
    { this->ProjectionsEiVectors[i][0]=sin((double)i/(double)theDim* MathRoutines::Pi());
      this->ProjectionsEiVectors[i][1]=cos((double)i/(double)theDim* MathRoutines::Pi());
    }
  }
  else
  { this->ProjectionsEiVectors.SetSizeMakeMatrix(3, 2);
    this->ProjectionsEiVectors[0][0]=1;
    this->ProjectionsEiVectors[0][1]=0;
    this->ProjectionsEiVectors[1][0]=0;
    this->ProjectionsEiVectors[1][1]=-1;
    this->ProjectionsEiVectors[2][0]=.7;
    this->ProjectionsEiVectors[2][1]=-0.4;
    this->ProjectionsEiVectors.SetSize(theDim);
  }
  if (this->BasisProjectionPlane.size<1)
    this->BasisProjectionPlane.SetSize(1);
  this->BasisProjectionPlane[0].MakeEiBasis(theDim, 1, 0);
  this->BasisProjectionPlane[0].size=2;
  for (int i=0; i<this->BasisProjectionPlane[0][1].size; i++)
    this->BasisProjectionPlane[0][1][i]=2*i+1;
  for (int i=0; i<this->BasisProjectionPlane[0][0].size; i++)
    this->BasisProjectionPlane[0][0][i]=3*i+2;
  if (this->theBilinearForm.NumRows!=theDim)
    this->theBilinearForm.MakeIdMatrix(theDim, 1, 0);
}

std::string DrawingVariables::GetHtmlFromDrawOperationsCreateDivWithUniqueName(int theDimension)
{ if (theDimension<2)
    return "<br><b>Pictures of dimension less than two are not drawn.</b><br>";
  std::stringstream out, tempStream1, tempStream2, tempStream3, tempStream4, tempStream5, tempStream6;
  std::stringstream tempStream7, tempStream8, tempStream9, tempStream10, tempStream11,
  tempStream12, tempStream13, tempStream14, tempStream15;
  this->NumHtmlGraphics++;
  int timesCalled=this->NumHtmlGraphics;
  tempStream1 << "drawConeInit" << timesCalled;
  std::string theInitFunctionName= tempStream1.str();
  tempStream5 << "drawAll" << timesCalled;
  std::string theDrawFunctionName= tempStream5.str();
  tempStream2 << "idCanvasCone" << timesCalled;
  std::string theCanvasId= tempStream2.str();
  tempStream3 << "surf" << timesCalled;
  std::string theSurfaceName=tempStream3.str();
  tempStream4 << "pts1" << timesCalled;
  std::string Points1ArrayName=tempStream4.str();
  tempStream10 << "pts2" << timesCalled;
  std::string Points2ArrayName=tempStream10.str();
  tempStream11<< "circ" << timesCalled;
  std::string circArrayName=tempStream11.str();
  tempStream12 << "txt" << timesCalled;
  std::string txtArrayName=tempStream12.str();

  tempStream6 << "basisCircles" << timesCalled;
  std::string basisCircles = tempStream6.str();
  tempStream13 << "proj" << timesCalled;
  std::string projName = tempStream13.str();
  tempStream14 << "projCirc" << timesCalled;
  std::string projBasisCircles= tempStream14.str();
  tempStream15 << "eiBasis" << timesCalled;
  std::string eiBasis= tempStream15.str();

  tempStream7 << "shiftXCone" << timesCalled;
  std::string shiftX=tempStream7.str();
  tempStream8 << "shiftYCone" << timesCalled;
  std::string shiftY=tempStream8.str();
  tempStream9 << "convXY" << timesCalled;
  std::string functionConvertToXYName=tempStream9.str();
  out << "<script type=\"text/javascript\" src=\"http://ajax.googleapis.com/ajax/libs/dojo/1.6/dojo/dojo.xd.js\""
  << " djConfig = \"parseOnLoad: true\"></script>";
  out << "<div style=\"width:" << this->DefaultHtmlWidth << ";height:" << this->DefaultHtmlHeight << ";border:solid 1px\" id=\"" << theCanvasId
  << "\" onmousedown=\"clickCanvasCone" << timesCalled << "(event.clientX, event.clientY);\" onmouseup=\"selectedBasisIndexCone" << timesCalled
  << "=-1;\" onmousemove=\"mouseMoveRedrawCone" <<  timesCalled << "(event.clientX, event.clientY);\" "
  << "onmousewheel=\"mouseHandleWheelCone" << timesCalled << "(event);\""
  << "></div>";
  out
  << "<br>The projection plane (drawn on the screen) is spanned by the following two vectors<br> \n";
  List<List<std::string> > textEbasisNames;
  textEbasisNames.SetSize(2);
  for (int i=0; i<2; i++)
  { textEbasisNames[i].SetSize(theDimension);
    for (int j=0; j<theDimension; j++)
    { std::stringstream tmpStream;
      tmpStream << "textEbasis" << timesCalled << "_" << i << "_" << j;
      textEbasisNames[i][j]=tmpStream.str();
      out << "<textarea rows=\"1\" cols=\"2\" id=\"" << textEbasisNames[i][j]
      << "\">" << "</textarea>\n";
    }
    out << "<br>";
  }
  out << "<button onclick=\"startProjectionPlaneUser" << timesCalled << "();\">Change to basis</button><br>";
  out
  << "The bilinear form of the vector space follows. The ij^th element "
  << " gives the scalar product of e_i and e_j. If you enter a degenerate or non-positive definite "
  << " symmetric bilinear form"
  << " the javascript might crash. You are expected to enter a symmetric strictly positive definite matrix. <br> \n";
  for (int i=0; i<this->theBuffer.theBilinearForm.NumRows; i++)
  { for (int j=0; j<this->theBuffer.theBilinearForm.NumCols; j++)
    { std::stringstream tmpStream;
      tmpStream << "textBilinearForm" << timesCalled << "_" << i << "_" << j;
      out << "<textarea rows=\"1\" cols=\"2\" id=\"" << tmpStream.str()
      << "\" \n onChange=\"BilinearForm"
      << timesCalled << "[" << i << "]["
      << j << "]=document.getElementById('" << tmpStream.str() << "').value;\">"
      << this->theBuffer.theBilinearForm.elements[i][j] << "</textarea>\n";
    }
    out << "<br>";
  }
  out << CGI::GetHtmlButton("button"+theCanvasId, theDrawFunctionName+"();", "redraw");
  out << "<br>The picture is drawn using javascript."
  << "<br> Left click + hold+ move the mouse on a special vector = rotates the special vector. "
  << "Special vectors, if any, should be labeled according to the author's ``artistic'' inspiration (usually dark red dots).  "
  << "<br>Moving a vector rotates ``infinitesimally'' the projection plane of your screen "
  << "<br>1) inside the projection plane "
  << "<br>2) in the plane spanned by the selected vector and its orthogonal complement relative to the projection plane. "
  << "<br>The angle change matches the motion of your mouse pointer.  "
  << " Special care must be taken if the selected vector lies "
  << "inside the projection plane or the selected vector is orthogonal to the projection plane. "
  << "If one of these cases happens, the picture might jump around a bit."
  << "<br>The mouse wheel zooms in/out. "
  << " Zooming is tested to work on Firefox and google Chrome browsers on Ubuntu."
  ;
  out << "<br>Currently, " << this->theBuffer.IndexNthDrawOperation.size << " elements are drawn. ";
  if (this->theBuffer.IndexNthDrawOperation.size>500)
    out  << " The visualization is javascript/pc processor <b>intensive</b> so it will <b>not work well</b> "
    <<  " for graphics with lots of elements. This message is displayed only when the number of drawn elements is more than 500.";
  out << "<hr>";

  out << "<script type=\"text/javascript\">\n";
  if (this->theBuffer.ProjectionsEiVectors.size!= theDimension || this->theBuffer.theBilinearForm.NumRows!=theDimension)
  { this->theBuffer.MakeMeAStandardBasis(theDimension);
    //std::cout << "made a standard basis!";
  }
  out << "var BilinearForm" << timesCalled << "= new Array(" << theDimension << ");\n";
  for (int i=0; i<theDimension; i++)
  { out << "BilinearForm" << timesCalled << "[" << i << "]=new Array(" << theDimension << ");\n";
    for (int j=0; j<theDimension; j++)
      out << "BilinearForm" << timesCalled << "[" << i << "][" << j << "]=" << this->theBuffer.theBilinearForm.elements[i][j] << ";\t";
    out << "\n";
  }
  this->theBuffer.ModifyToOrthonormalNoShiftSecond
  (this->theBuffer.BasisProjectionPlane[0][1], this->theBuffer.BasisProjectionPlane[0][0]);
//  (this->theBuffer.BasisProjectionPlane[0][0], this->theBuffer.BasisProjectionPlane[0][1]);
  out
  << "var VectorE1Cone" << timesCalled << "= new Array(" << theDimension << ");\n"
  << "var VectorE2Cone" << timesCalled << "= new Array(" << theDimension << ");\n"
  << "var VectorE1ConeGoal" << timesCalled << "= new Array(" << theDimension << ");\n"
  << "var VectorE2ConeGoal" << timesCalled << "= new Array(" << theDimension << ");\n"
  << "var VectorE1ConeStart" << timesCalled << "= new Array(" << theDimension << ");\n"
  << "var VectorE2ConeStart" << timesCalled << "= new Array(" << theDimension << ");\n";

  for (int i=0; i<theDimension; i++)
  { out << "VectorE2Cone" << timesCalled << "[" << i << "]=" << this->theBuffer.BasisProjectionPlane[0][1][i] << ";\t";
    out << "VectorE1Cone" << timesCalled << "[" << i << "]=" << this->theBuffer.BasisProjectionPlane[0][0][i] << ";\n";
  }
  out << "var frameCount" << timesCalled << "=0;\n";
  out << "var frameCountGoesUp" << timesCalled << "=true;\n";
  int numFramesUserPlane=50;
  out << "function startProjectionPlaneUser" << timesCalled << "(){\n"
  << " frameCount" << timesCalled << "=0;\n";
  for (int i=0; i<theDimension; i++)
    out
    << "  VectorE1ConeStart" << timesCalled << "[" << i << "]=VectorE1Cone" << timesCalled << "[" << i << "];\n"
    << "  VectorE2ConeStart" << timesCalled << "[" << i << "]=VectorE2Cone" << timesCalled << "[" << i << "];\n"
    << "  VectorE1ConeGoal" << timesCalled << "[" << i << "]=document.getElementById(\""
    << textEbasisNames[0][i] << "\").value;\n"
    << "  VectorE2ConeGoal" << timesCalled << "[" << i << "]=document.getElementById(\""
    << textEbasisNames[1][i] << "\").value;\n";
  out
  << "  changeProjectionPlaneUser" << timesCalled << "();\n"
  << "\n}\n";
  out << "function changeProjectionPlaneUser" << timesCalled << "(){\n"
  << "  frameCount" << timesCalled << "++;\n"
  << "  if (frameCount" << timesCalled << "> " << numFramesUserPlane << ")\n"
  << "    return;\n"
  << "  for (i=0; i<" << theDimension << "; i++)\n"
  << "  { "
  << "VectorE1Cone" << timesCalled << "[i]=VectorE1ConeGoal" << timesCalled << "[i]*"
  << "(frameCount" << timesCalled << "/" << numFramesUserPlane << ")"
  << "+ VectorE1ConeStart" << timesCalled << "[i]*"
  << "(1-frameCount" << timesCalled << "/" << numFramesUserPlane << ");\n"
  << "    VectorE2Cone" << timesCalled << "[i]=VectorE2ConeGoal" << timesCalled << "[i]*"
  << "(frameCount" << timesCalled << "/" << numFramesUserPlane << ")"
  << "+ VectorE2ConeStart" << timesCalled << "[i]*"
  << "(1-frameCount" << timesCalled << "/" << numFramesUserPlane << ");\n"
  << "  }\n"
  << "  MakeVectorE1AndE2orthonormal" << timesCalled << "();\n"
  << "  ComputeProjections" << timesCalled << "();\n"
  << "  " << theDrawFunctionName << "();\n"
  << "  window.setTimeout(\"changeProjectionPlaneUser" << timesCalled << "()\",100);\n"
  << "}";
  if (this->theBuffer.BasisProjectionPlane.size>2)
  { out << "BasisProjectionPlane" << timesCalled << "=new Array(" << this->theBuffer.BasisProjectionPlane.size << ");\n";
    for (int j=0; j<this->theBuffer.BasisProjectionPlane.size; j++)
    { out << "BasisProjectionPlane" << timesCalled << "[" << j << "]=new Array(2);\n";
      for (int k=0; k<2; k++)
      { out << "BasisProjectionPlane" << timesCalled << "[" << j << "][" << k << "]=new Array(" << theDimension << ");\n";
        for (int l=0; l<theDimension; l++)
          out << "BasisProjectionPlane" << timesCalled << "[" << j << "][" << k << "][" << l << "]="
          << this->theBuffer.BasisProjectionPlane[j][k][l] << ";\t";
        out << "\n";
      }
      out << "\n";
    }
    out << "function changeProjectionPlaneOnTimer" << timesCalled << "(){\n"
    << "if(frameCountGoesUp" << timesCalled << ")\n"
    << "  frameCount" << timesCalled << "++;\n"
    << "else\n"
    << "  frameCount" << timesCalled << "--;\n"
    << "if (frameCount" << timesCalled << "==" << this->theBuffer.BasisProjectionPlane.size-1 << " || "
    << "frameCount" << timesCalled << "==0)\n"
    << "{ \n"
    << "  frameCountGoesUp" << timesCalled << "=! frameCountGoesUp" << timesCalled << ";\n"
    << "}\n"
    << "if (frameCount" << timesCalled << ">= " << this->theBuffer.BasisProjectionPlane.size << ")\n"
    << "  return;"
    << "for (i=0; i<" << theDimension << "; i++)\n"
    << "{ VectorE1Cone" << timesCalled << "[i]=BasisProjectionPlane" << timesCalled
    << "[frameCount" << timesCalled << "][0][i];\n"
    << "  VectorE2Cone" << timesCalled << "[i]=BasisProjectionPlane" << timesCalled
    << "[frameCount" << timesCalled << "][1][i];\n"
    << "\n}\n";
    out << theDrawFunctionName << "();\n";
    out << "window.setTimeout(\"changeProjectionPlaneOnTimer" << timesCalled << "()\",100);\n"
    << "}\n";
  }

  out << "var " << projName << "= new Array(" << theDimension << ");\n";
  out << "var " << eiBasis << "= new Array(" << theDimension << ");\n";
  for (int i=0; i<theDimension; i++)
    out << projName << "[" << i << "]= new Array(2);\n";
  out  << "var " << basisCircles << "=new Array(" << this->theBuffer.BasisToDrawCirclesAt.size << ");\n";
  out  << "var " << projBasisCircles << "=new Array(" << this->theBuffer.BasisToDrawCirclesAt.size << ");\n";
  for (int i=0; i<this->theBuffer.BasisToDrawCirclesAt.size; i++)
  { out << basisCircles << "[" << i << "]=[";
    for (int j=0; j<theDimension; j++)
    { out << this->theBuffer.BasisToDrawCirclesAt[i][j];
      if(j!=theDimension-1)
        out << ",";
     }
    out <<  "];\n";
  }
  for (int i=0; i<theDimension; i++)
  { ////////////////////
    out << eiBasis << "[" << i << "]=[";
    for (int j=0; j<theDimension; j++)
    { out << (i==j)? 1 :0;
      if(j!=theDimension-1)
        out << ",";
     }
    out <<  "];\n";
    //////////////////
    out << projBasisCircles << "[" << i << "]= new Array(2);\n";
  }
  out << "var " << Points1ArrayName << "=new Array(" << this->theBuffer.theDrawLineBetweenTwoRootsOperations.size << ");\n"
  << "var " << Points2ArrayName << "=new Array(" << this->theBuffer.theDrawLineBetweenTwoRootsOperations.size << ");\n"
  << "var " << circArrayName << "=new Array(" << this->theBuffer.theDrawCircleAtVectorOperations.size << ");\n"
  << "var " << txtArrayName << "=new Array(" << this->theBuffer.theDrawTextAtVectorOperations.size << ");\n"
  ;
  for (int i=0; i<this->theBuffer.theDrawLineBetweenTwoRootsOperations.size; i++)
  { Vector<double>& current1=theBuffer.theDrawLineBetweenTwoRootsOperations[i].v1;
    Vector<double>& current2=theBuffer.theDrawLineBetweenTwoRootsOperations[i].v2;
    out << Points1ArrayName << "[" << i << "]=[";
    for (int j=0; j<theDimension; j++)
    { out << current1[j];
      if (j!=theDimension-1)
        out << ",";
    }
    out << "];\n";
    out << Points2ArrayName << "[" << i << "]=[";
    for (int j=0; j<theDimension; j++)
    { out << current2[j];
      if (j!=theDimension-1)
        out << ",";
    }
    out << "];\n";
  }
  for (int i=0; i<this->theBuffer.theDrawCircleAtVectorOperations.size; i++)
  { Vector<double>& current1=theBuffer.theDrawCircleAtVectorOperations[i].theVector;
    out << circArrayName << "[" << i << "]=[";
    for (int j=0; j<theDimension; j++)
    { out << current1[j];
      if (j!=theDimension-1)
        out << ",";
    }
    out << "];\n";
  }
  for (int i=0; i<this->theBuffer.theDrawTextAtVectorOperations.size; i++)
  { Vector<double>& current1=theBuffer.theDrawTextAtVectorOperations [i].theVector;
    out << txtArrayName << "[" << i << "]=[";
    for (int j=0; j<theDimension; j++)
    { out << current1[j];
      if (j!=theDimension-1)
        out << ",";
    }
    out << "];\n";
  }
  out << "var " << shiftX << "=" <<
  this->theBuffer.centerX[0]
  //100
  << ";\n";
  out << "var " <<  shiftY << "=" <<
  this->theBuffer.centerY[0]
  //100
  << ";\n";
  out << "var GraphicsUnitCone" << timesCalled << "=" << this->theBuffer.GraphicsUnit[0] << ";\n";
  out << "function " << functionConvertToXYName << "(vector){\n";
  out << "resultX=" << shiftX << "; resultY=" << shiftY << ";\nfor (i=0; i<" << theDimension << "; i++){\n";
  out << "resultX+=vector[i]*" << projName << "[i][0];\n";
  out << "resultY+=vector[i]*" << projName << "[i][1];\n}\n";
  out << "result=[resultX, resultY];\n";
  out << "return result;\n";
  out << "}\n";
  out << "var " << theSurfaceName << "=0;\n"
  << "function " << theDrawFunctionName << "(){\n"
  << "  if (" << theSurfaceName << "==0)\n"
  << "    " << theInitFunctionName << "();\n"
  << "ComputeProjections" << timesCalled << "();\n"
  << theSurfaceName << ".clear();\n";
  for (int i=0; i<this->theBuffer.IndexNthDrawOperation.size; i++)
  { int currentIndex=this->theBuffer.IndexNthDrawOperation[i];
    switch(theBuffer.TypeNthDrawOperation[i])
    { case DrawOperations::typeDrawLineBetweenTwoVectors:
        out << theSurfaceName << ".createLine({"
              << " x1 :" << functionConvertToXYName << "( " << Points1ArrayName << "["
              << currentIndex<< "])[0],"
              << " y1 :" << functionConvertToXYName << "( " << Points1ArrayName << "["
              << currentIndex << "])[1],"
              << " x2 :"  << functionConvertToXYName << "( " << Points2ArrayName << "["
              << currentIndex << "])[0],"
              << " y2 :" << functionConvertToXYName << "( " << Points2ArrayName << "["
              << currentIndex << "])[1] }).setStroke({color : \""
              << this->GetColorHtmlFromColorIndex(this->theBuffer.theDrawLineBetweenTwoRootsOperations[currentIndex].ColorIndex)
              << "\"});\n";
        break;
      case DrawOperations::typeDrawCircleAtVector:
        out << theSurfaceName << ".createEllipse({"
              << " cx :" << functionConvertToXYName << "( " << circArrayName << "["
              << currentIndex << "])[0],"
              << " cy :" << functionConvertToXYName << "( " << circArrayName << "["
              << currentIndex << "])[1],"
              << " rx : "  <<  this->theBuffer.theDrawCircleAtVectorOperations[currentIndex].radius << ","
              << " ry :" << this->theBuffer.theDrawCircleAtVectorOperations[currentIndex].radius
              << " }).setStroke({color : \""
              << this->GetColorHtmlFromColorIndex(this->theBuffer.theDrawCircleAtVectorOperations[currentIndex].ColorIndex)
              << "\"});\n";
        break;
      case DrawOperations::typeDrawTextAtVector:
        out << theSurfaceName << ".createText({"
              << " x :" << functionConvertToXYName << "( " << txtArrayName << "["
              << currentIndex<< "])[0],"
              << " y :" << functionConvertToXYName << "( " << txtArrayName << "["
              << currentIndex << "])[1],"
              << " text : \""  <<  this->theBuffer.theDrawTextAtVectorOperations[currentIndex].theText
              << "\" }).setStroke({color : \""
              << this->GetColorHtmlFromColorIndex(this->theBuffer.theDrawTextAtVectorOperations[currentIndex].ColorIndex)
              << "\"});\n";
      default: break;
    }
  }
  out << "}\n"
  << "function " << theInitFunctionName << "(){\n"
  << "node = dojo.byId(\"" << theCanvasId << "\");\n"
  << theSurfaceName << "  = dojox.gfx.createSurface(node," << this->DefaultHtmlWidth << "," << this->DefaultHtmlHeight << ");\n"
  << theDrawFunctionName << "();\n";
  if (this->theBuffer.BasisProjectionPlane.size>2)
    out << "window.setTimeout(\"changeProjectionPlaneOnTimer" << timesCalled << "()\",100);\n";
  out << " }\n";
  out << "var selectedBasisIndexCone" << timesCalled << "=-1;\n"
  << "var clickTolerance=5;\n"
  << "function ptsWithinClickToleranceCone" << timesCalled << "(x1, y1, x2, y2)\n"
  << "{ if (x1-x2>clickTolerance || x2-x1>clickTolerance || y1-y2>clickTolerance || y2-y1>clickTolerance )\n    return false;\n  return true;\n}";

  out << "function MultiplyVector" << timesCalled << "(output, coeff)"
  << "{ for (var i=0; i<output.length; i++)\n"
  << "  output[i]*=coeff;\n"
  << "}\n"
  << "function AddVectorTimes" << timesCalled << "(output, inputVector, coeff)\n"
  << "{ for (var i=0; i<output.length; i++)\n"
  << "  output[i]+=inputVector[i]*coeff;\n"
  << "}\n"
  << "function ScaleToUnitLength" << timesCalled << "(vector)\n"
  << "{ MultiplyVector" << timesCalled << "(vector, 1/Math.sqrt(getScalarProduct" << timesCalled << "(vector,vector)));\n"
  << "}\n";
  out
  << "function RotateOutOfPlane" << timesCalled << "(input, orthoBasis1, orthoBasis2, oldTanSquared, newTanSquared, newX, newY, oldX, oldY)"
  << "{ var projection= dojo.clone(orthoBasis1);\n"
  << "  var vComponent= dojo.clone(input);\n"
  << "  var scal1= getScalarProduct" << timesCalled << "(orthoBasis1, input);\n"
  << "  var scal2= getScalarProduct" << timesCalled << "(orthoBasis2, input);\n"
  << "  MultiplyVector" << timesCalled << "(projection, scal1);\n"
  << "  AddVectorTimes" << timesCalled << "(projection, orthoBasis2, scal2);\n"
  << "  AddVectorTimes" << timesCalled << "(vComponent, projection, -1);\n"
  << "  var oldAngle=Math.atan(Math.sqrt(oldTanSquared));\n"
  << "  var newAngle=Math.atan(Math.sqrt(newTanSquared));\n"
  << "  if (isNaN(oldAngle) || isNaN(newAngle))\n"
  << "    return input;\n"
  << "  var angleChange=-oldAngle+newAngle;\n"
  << "  if (newX*oldX<0 && newY*oldY<0)\n"
//  <<  " {
//  << "    angleChange=oldAngle+newAngle;\n"
  << "    angleChange*=-1;\n"
//  << "  };"
//  << "  topBound=Math.PI/2;\n"
//  << "  bottomBound=-Math.PI/2;\n"
//  << "  while (angleChange>=topBound || angleChange<bottomBound)\n"
//  << "    if(angleChange>=topBound)\n"
//  << "      angleChange-=Math.PI;\n"
//  << "    else"
//  << "      angleChange+=Math.PI;\n"
  << "  projection=dojo.clone(orthoBasis1);\n"
  << "  MultiplyVector" << timesCalled << "(projection, Math.cos(angleChange)*scal1-Math.sin(angleChange)*scal2);\n"
  << "  AddVectorTimes" << timesCalled << "(projection, orthoBasis2, Math.sin(angleChange)*scal1+Math.sin(angleChange)*scal2);\n"
  << "  var result= vComponent;\n"
  << "  AddVectorTimes" << timesCalled << "(result, projection, 1);\n"
  << "  return result;\n"
  << "}\n";
  out << "function MakeVectorE1AndE2orthonormal" << timesCalled << "(){\n"
  << "  AddVectorTimes" << timesCalled << "(VectorE2Cone" << timesCalled
  << ", VectorE1Cone" << timesCalled << ", -getScalarProduct"
  << timesCalled << "(VectorE1Cone" << timesCalled << ", VectorE2Cone" << timesCalled
  << ")/getScalarProduct" << timesCalled
  << "(VectorE1Cone" << timesCalled << ", VectorE1Cone" << timesCalled << "));\n"
  << "  ScaleToUnitLength" << timesCalled << "(VectorE1Cone" << timesCalled << ");\n"
  << "  ScaleToUnitLength" << timesCalled << "(VectorE2Cone" << timesCalled << ");\n"
  << "}\n";
  out << "function ComputeProjections" << timesCalled << "()\n"
  << "{ for (var i=0; i<" << theDimension << "; i++)\n"
  << "  { " << projName << "[i][0]=GraphicsUnitCone" << timesCalled << "*getScalarProduct" << timesCalled
  << "(VectorE1Cone" << timesCalled << ","  << eiBasis << "[i]);\n"
  << "    " << projName << "[i][1]=-GraphicsUnitCone" << timesCalled << "*getScalarProduct" << timesCalled
  << "(VectorE2Cone" << timesCalled << ", " << eiBasis << "[i]);\n"
  << "  }\n";
  for (int j=0; j<theDimension; j++)
    out << "  document.getElementById(\"" << textEbasisNames[0][j] << "\").value="
    << "VectorE1Cone" << timesCalled << "[" << j << "];\n"
    << "  document.getElementById(\"" << textEbasisNames[1][j] << "\").value="
    << "VectorE2Cone" << timesCalled << "[" << j << "];\n";
  out
  << "  for (var i=0; i<" << this->theBuffer.BasisToDrawCirclesAt.size << "; i++)\n"
  << "  { " << projBasisCircles << "[i][0]=GraphicsUnitCone" << timesCalled << "*getScalarProduct" << timesCalled
  << "(VectorE1Cone" << timesCalled << ", " << basisCircles << "[i]);\n"
  << "    " << projBasisCircles << "[i][1]=-GraphicsUnitCone" << timesCalled << "*getScalarProduct" << timesCalled
  << "(VectorE2Cone" << timesCalled << ", " << basisCircles << "[i]);\n"
  << "  }\n"
  << "}\n";
  out << "\nfunction getScalarProduct" << timesCalled << "(root1, root2)\n"
  << "{ var result=0;\n"
  << "  for (var i=0; i<" << theDimension << "; i++)\n"
  << "    for (var j=0; j<" << theDimension << "; j++)\n"
  << "      result+= root1[i]*root2[j]*BilinearForm" << timesCalled << "[i][j];\n"
  << "  return result;\n"
  << "}\n";
  out
  << "function getAngleFromXandY" << timesCalled << "(x, y, neighborX, neighborY)\n"
  << "{ var result;\n"
  << "  if (x!=0)\n"
  << "   result= Math.atan(y/x);\n"
  << "  else\n"
  << "    if (y>0)\n"
  << "      result= Math.PI/2;\n"
  << "    else\n"
  << "      result= -Math.PI/2;\n"
  << "  return result;\n"
  << "}\n";
  out
  << "function getAngleChange" << timesCalled << "(newX, newY, oldX, oldY)\n"
  << "{ var result=getAngleFromXandY" << timesCalled
  << "(newX, newY, oldX, oldY)-getAngleFromXandY" << timesCalled << "(oldX, oldY, newX, newY);\n"
  << "  topBound=Math.PI/2;\n"
  << "  bottomBound=-Math.PI/2;\n"
  << "  while (result>topBound || result< bottomBound)\n"
  << "    if (result>topBound)\n"
  << "      result-=Math.PI;\n"
  << "    else\n"
  << "      result+=Math.PI;\n"
  << "  return result;\n"
  << "}\n";
  out << "\nfunction changeBasis" << timesCalled <<  "(selectedIndex, newX, newY)\n"
  << "{ if (newX==0 && newY==0)\n"
  << "    return;\n"
  << "  var selectedRoot=" << basisCircles << "[selectedIndex];\n"
  << "  var selectedRootLength=getScalarProduct" << timesCalled << "(selectedRoot,selectedRoot);\n"
  << "  var projectionSelected=" << projBasisCircles << "[selectedIndex];\n"
  << "  var oldX=projectionSelected[0]/GraphicsUnitCone" << timesCalled << ";\n"
  << "  var oldY=-projectionSelected[1]/GraphicsUnitCone" << timesCalled << ";\n"
  << "  newX/=GraphicsUnitCone" << timesCalled << ";\n"
  << "  newY/=GraphicsUnitCone" << timesCalled << ";\n"
  << "  var epsilon=0.000000015;\n"
  << "  if (newX*newX+newY*newY>0.003)\n"
  << "  { var AngleChange= -getAngleChange" << timesCalled << "(newX, newY, oldX, oldY);\n"
  << "    var NewVectorE1=dojo.clone(VectorE1Cone" << timesCalled << ");\n"
  << "    var NewVectorE2= dojo.clone(VectorE2Cone" << timesCalled << ");\n"
  << "    MultiplyVector" << timesCalled << "(NewVectorE1, Math.cos(AngleChange));\n"
  << "    AddVectorTimes" << timesCalled << "(NewVectorE1, VectorE2Cone" << timesCalled << ", Math.sin(AngleChange));\n"
  << "    MultiplyVector" << timesCalled << "(NewVectorE2, Math.cos(AngleChange));\n"
  << "    AddVectorTimes" << timesCalled << "(NewVectorE2, VectorE1Cone" << timesCalled << ", -Math.sin(AngleChange));\n"
  << "    VectorE1Cone" << timesCalled << "=NewVectorE1;\n"
  << "    VectorE2Cone" << timesCalled << "=NewVectorE2;\n"
  << "  }\n"
  << "  if (newX*newX+newY*newY>0.0001)\n"
  << "  { var RootTimesE1=getScalarProduct" << timesCalled << "(selectedRoot, VectorE1Cone" << timesCalled << ");\n"
  << "    var RootTimesE2=getScalarProduct" << timesCalled << "(selectedRoot, VectorE2Cone" << timesCalled << ");\n"
  << "    var vOrthogonal=dojo.clone(selectedRoot);\n"
  << "    var vProjection=dojo.clone(VectorE1Cone" << timesCalled << ");\n"
  << "    MultiplyVector" << timesCalled << "(vProjection, RootTimesE1);\n"
  << "    AddVectorTimes" << timesCalled << "(vProjection, VectorE2Cone" << timesCalled << ", RootTimesE2" << ");\n"
  << "    AddVectorTimes" << timesCalled << "(vOrthogonal, vProjection, -1);\n"
  << "    var oldRatioProjectionOverHeightSquared = (oldX*oldX+oldY*oldY)/ (selectedRootLength-oldX*oldX-oldY*oldY);\n"
  << "    var newRatioProjectionOverHeightSquared = (newX*newX+newY*newY)/ (selectedRootLength-newX*newX-newY*newY);\n";
  if (theDimension>2)
  { out
    << "    if (getScalarProduct" << timesCalled << "(vOrthogonal, vOrthogonal)<epsilon && getScalarProduct" << timesCalled
    << "(vOrthogonal, vOrthogonal)>-epsilon)\n"
    << "    { vOrthogonal=dojo.clone(" << eiBasis <<  "[2]);\n"
    << "    }\n";
  }
out
  << "    if (getScalarProduct" << timesCalled << "(vOrthogonal, vOrthogonal)>epsilon || getScalarProduct" << timesCalled
  << "(vOrthogonal, vOrthogonal)<-epsilon)\n"
  << "    { if (oldRatioProjectionOverHeightSquared==0)\n"
  << "      { vProjection=dojo.clone(VectorE1Cone" << timesCalled << ");\n"
  << "        MultiplyVector" << timesCalled << "(vProjection, -newX);\n"
  << "        AddVectorTimes" << timesCalled << "(vProjection, VectorE2Cone" << timesCalled << ", newY" << ");\n"
  << "      }\n"
  << "      ScaleToUnitLength" << timesCalled << "(vProjection);\n"
  << "      ScaleToUnitLength" << timesCalled << "(vOrthogonal);\n"
  << "      VectorE1Cone" << timesCalled << "=RotateOutOfPlane" << timesCalled << "(VectorE1Cone" << timesCalled
  << ", vProjection, vOrthogonal, oldRatioProjectionOverHeightSquared, newRatioProjectionOverHeightSquared, newX, newY, oldX, oldY);\n"
  << "      VectorE2Cone" << timesCalled << "=RotateOutOfPlane" << timesCalled << "(VectorE2Cone" << timesCalled
  << ", vProjection, vOrthogonal, oldRatioProjectionOverHeightSquared, newRatioProjectionOverHeightSquared, newX, newY, oldX, oldY);\n"
  << "    }\n"
  << "    MakeVectorE1AndE2orthonormal" << timesCalled << "();\n"
  << "    ComputeProjections" << timesCalled << "();\n"
  << "  }\n"
  << "}\n";

  out << "\nfunction clickCanvasCone" << timesCalled << "(cx,cy)\n"
  << "{ divPosX=0;\n  divPosY=0;\n  thePointer= document.getElementById(\"idCanvasCone" << timesCalled << "\");\n"
  << "  while(thePointer)  {\n  divPosX += thePointer.offsetLeft;\n  divPosY += thePointer.offsetTop;\n  thePointer = thePointer.offsetParent;\n  }"
  << "\n  posx=(cx-divPosX+document.body.scrollLeft-" << shiftX << ");"
  << "\n  posy=(cy-divPosY+document.body.scrollTop-" << shiftY << ");\n  selectedBasisIndexCone" << timesCalled <<"=-1;\n"
  << "if (ptsWithinClickToleranceCone" << timesCalled << "(posx,posy,0,0))" << "\nselectedBasisIndexCone" << timesCalled << "=-2;\n"
  <<  "for (i=0; i<" << theDimension << ";i++)  {\n if (ptsWithinClickToleranceCone" << timesCalled
  << "(posx, posy, " << projBasisCircles << "[i][0]" << ", " << projBasisCircles
  << "[i][1]" << "))\n"
  << "  selectedBasisIndexCone" << timesCalled << "=i;  \n}\n}\nfunction mouseMoveRedrawCone" << timesCalled << "(cx, cy)\n"
  << "{ if (selectedBasisIndexCone" << timesCalled << "==-1)\n    return;\n  divPosX=0;\n  divPosY=0;\n"
  << "  thePointer= document.getElementById(\"idCanvasCone"<< timesCalled << "\");\n  while(thePointer)\n  { divPosX += thePointer.offsetLeft;\n"
  << "    divPosY += thePointer.offsetTop;\n    thePointer = thePointer.offsetParent;\n  }\n"
  << "  posx=(cx-divPosX+document.body.scrollLeft-" << shiftX << ");\n"
  << "  posy=-(cy-divPosY+document.body.scrollTop-" << shiftY << ");\n"
  << "if (selectedBasisIndexCone" << timesCalled << "==-2)\n{ shiftXCone" << timesCalled << "=(cx-divPosX+document.body.scrollLeft);\n"
  << shiftY << "=(cy-divPosY+document.body.scrollTop);\n  }  else\n"
  << "{ changeBasis" << timesCalled << "(selectedBasisIndexCone" << timesCalled << ", posx, posy);\n  }\n  "
  << theDrawFunctionName << " ();\n}\n";
  out << "\n tempDiv=document.getElementById(\"" << theCanvasId << "\");";
  out << "\ntempDiv.addEventListener (\"DOMMouseScroll\", mouseHandleWheelCone"
  << timesCalled << ", true);\n";
  out << "function mouseHandleWheelCone" << timesCalled << "(theEvent){\n"
  << "theEvent = theEvent ? theEvent : window.event;\n theEvent.preventDefault();\ntheEvent.stopPropagation();\ntheWheelDelta = theEvent.detail ? theEvent.detail * -1 : theEvent.wheelDelta / 40;\n"
  << "GraphicsUnitCone" << timesCalled << "+=theWheelDelta;\n"
  << "if (GraphicsUnitCone" << timesCalled << "==0)\n GraphicsUnitCone" << timesCalled << "=3;\n"
//  << "for (i=0; i<" << theDimension << "; i++){\n "
//  << basisName << "[i][0]*=GraphicsUnitCone" << timesCalled << "/(GraphicsUnitCone" << timesCalled << "-theWheelDelta);\n"
//  << basisName << "[i][1]*=GraphicsUnitCone" << timesCalled << "/(GraphicsUnitCone" << timesCalled << "-theWheelDelta);\n"
//  << "}\n"
  << theDrawFunctionName << "();\n}\n";

  out
  << "dojo.require(\"dojox.gfx\");\n"
  << "dojo.addOnLoad(" << theInitFunctionName << ");\n"
//  << "dojo.addOnLoad(" << theDrawFunctionName << ");\n"
  << "</script>\n"
  ;
  return out.str();
}

std::string ConeComplex::DrawMeToHtmlLastCoordAffine
(DrawingVariables& theDrawingVariables, FormatExpressions& theFormat)
{ bool isBad=false;
  isBad=this->DrawMeLastCoordAffine(true, theDrawingVariables, theFormat);
  std::stringstream out;
  out << theDrawingVariables.GetHtmlFromDrawOperationsCreateDivWithUniqueName(this->GetDim()-1);
  if (isBad)
    out << "<hr>" << "found cones which I can't draw<hr>";
  out << this->ElementToString(false, true);
/*  for (int i=0; i<this->size; i++)
  { theDrawingVariables.theBuffer.init();
    out <<"<hr>" << this->TheObjects[i].DrawMeToHtmlLastCoordAffine(theDrawingVariables, theFormat);
  }*/
  return out.str();
}

std::string ConeComplex::DrawMeToHtmlProjective
(DrawingVariables& theDrawingVariables, FormatExpressions& theFormat)
{ bool isGood=true;
  isGood=this->DrawMeProjective(0, true, theDrawingVariables, theFormat);
  std::stringstream out;
  out << theDrawingVariables.GetHtmlFromDrawOperationsCreateDivWithUniqueName(this->GetDim());
  if (!isGood)
    out << "<hr>" << "found cones which I can't draw<hr>";
  out << this->ElementToString(false, true);
/*  for (int i=0; i<this->size; i++)
  { theDrawingVariables.theBuffer.init();
    out <<"<hr>" << this->TheObjects[i].DrawMeToHtmlLastCoordAffine(theDrawingVariables, theFormat);
  }*/
  return out.str();
}

bool ConeComplex::DrawMeLastCoordAffine
(bool InitDrawVars, DrawingVariables& theDrawingVariables, FormatExpressions& theFormat)
{ bool result=true;
  if (InitDrawVars)
    theDrawingVariables.theBuffer.initDimensions(this->GetDim()-1, 1);

  theDrawingVariables.drawCoordSystemBuffer(theDrawingVariables, this->GetDim()-1, 0);
  for (int i=0; i<this->size; i++)
  { //theDrawingVariables.theBuffer.init();
    result=this->TheObjects[i].DrawMeLastCoordAffine(InitDrawVars, theDrawingVariables, theFormat) && result;
    std::stringstream tempStream;
    tempStream << i+1;
    Vector<Rational> tempRoot=this->TheObjects[i].GetInternalPoint();
    tempRoot.MakeAffineUsingLastCoordinate();
    theDrawingVariables.drawTextAtVectorBuffer
    (tempRoot, tempStream.str(), CGI::RedGreenBlue(0,0,0), theDrawingVariables.PenStyleNormal, 0);
    //std::cout <<"<hr> drawing number " << i+1 << ": " << theDrawingVariables.GetHtmlFromDrawOperationsCreateDivWithUniqueName(this->GetDim()-1);
  }
  return result;
}

bool ConeComplex::DrawMeProjective
(Vector<Rational>* coordCenterTranslation, bool InitDrawVars, DrawingVariables& theDrawingVariables, FormatExpressions& theFormat)
{ bool result=true;
  Vector<Rational> tempRoot;
  Vectors<Rational> tempRoots;
  Matrix<Rational>  tempMat;
  if (this->GetDim()<=1)
    return false;
  if (InitDrawVars)
  { theDrawingVariables.theBuffer.init();
    theDrawingVariables.theBuffer.initDimensions(this->GetDim(), 1);
    theDrawingVariables.theBuffer.MakeMeAStandardBasis(this->GetDim());
    theDrawingVariables.drawCoordSystemBuffer(theDrawingVariables, this->GetDim(), 0);
    if (this->GetDim()>2)
    { this->ConvexHull.GetInternalPoint(tempRoot);
      tempMat.AssignVectorRow(tempRoot);
      tempMat.FindZeroEigenSpace(tempRoots);
      for (int i=0; i<2; i++)
        for (int j=0; j<this->GetDim(); j++)
          theDrawingVariables.theBuffer.BasisProjectionPlane[0][i][j]=tempRoots[i][j].DoubleValue();
    }
  }
  for (int i=0; i<this->size; i++)
  { //theDrawingVariables.theBuffer.init();
    result=(this->TheObjects[i].DrawMeProjective(coordCenterTranslation, false, theDrawingVariables, theFormat) && result);
    //std::cout <<"<hr> drawing number " << i+1 << ": " << theDrawingVariables.GetHtmlFromDrawOperationsCreateDivWithUniqueName(this->GetDim()-1);
  }
  return result;
}

bool Cone::DrawMeLastCoordAffine
(bool InitDrawVars, DrawingVariables& theDrawingVariables, FormatExpressions& theFormat,
 int ChamberWallColor)
{ Vector<Rational> ZeroRoot;
  ZeroRoot.MakeZero(this->GetDim()-1);
  Vectors<Rational> VerticesScaled;
//  VerticesScaled.Reserve(this->Vertices.size*2);
  VerticesScaled=this->Vertices;
  Rational tempRat;
  List<bool> DrawVertex;
  DrawVertex.initFillInObject(this->Vertices.size, true);
  bool foundBadVertex=false;

  for (int i=0; i<this->Vertices.size; i++)
  { tempRat=*VerticesScaled[i].LastObject();
    VerticesScaled[i].SetSize(this->GetDim()-1);
    if (tempRat.IsPositive())
      VerticesScaled[i]/=tempRat;
    if (tempRat.IsEqualToZero())
    { VerticesScaled[i]*=10000;
    }
    if (tempRat.IsNegative())
    { DrawVertex[i]=false;
      foundBadVertex=true;
    }
  }
  Vector<Rational> iScaledVertex, jScaledVertex;
  if (InitDrawVars)
    theDrawingVariables.drawCoordSystemBuffer(theDrawingVariables, this->GetDim()-1,0);
  for (int k=0; k<this->Normals.size; k++)
    for (int i=0; i<VerticesScaled.size; i++)
      if (DrawVertex[i] && this->Normals[k].ScalarEuclidean(this->Vertices[i]).IsEqualToZero())
        for (int j=i+1; j<VerticesScaled.size; j++)
          if(DrawVertex[j] && this->Normals[k].ScalarEuclidean(this->Vertices[j]).IsEqualToZero())
            if (this->IsAnHonest1DEdgeAffine(i,j))
            { /*bool iVertexLiesAtInfinity=this->Vertices[i].LastObject()->IsEqualToZero();
              bool jVertexLiesAtInfinity=this->Vertices[j].LastObject()->IsEqualToZero();
              if (iVertexLiesAtInfinity || jVertexLiesAtInfinity)
              { iScaledVertex=VerticesScaled[i];
                jScaledVertex=VerticesScaled[j];
                if (iVertexLiesAtInfinity)
                  iScaledVertex*=10000;
                if (jVertexLiesAtInfinity)
                  jScaledVertex*=10000;
                theDrawingVariables.drawLineBetweenTwoVectorsBuffer
                (iScaledVertex, jScaledVertex, theDrawingVariables.PenStyleNormal, CGI::RedGreenBlue(200,200,200));
              }*/
              theDrawingVariables.drawLineBetweenTwoVectorsBuffer
              (VerticesScaled[i], VerticesScaled[j], theDrawingVariables.PenStyleNormal, ChamberWallColor);
            }
  return foundBadVertex;
}

std::string Cone::DrawMeToHtmlLastCoordAffine
(DrawingVariables& theDrawingVariables, FormatExpressions& theFormat)
{ if (this->flagIsTheZeroCone)
    return "The cone is empty.";
  if (this->Normals.size<1)
    return "The cone is the entire space";
  if (this->Vertices.size<1)
    return "The cone is empty";
  std::stringstream out;
  theDrawingVariables.theBuffer.MakeMeAStandardBasis(this->GetDim()-1);
  bool foundBadVertex=this->DrawMeLastCoordAffine(false, theDrawingVariables, theFormat);
  theDrawingVariables.drawCoordSystemBuffer(theDrawingVariables, this->GetDim()-1, 0);
  if (foundBadVertex)
    out << "<br>The cone does not lie in the upper half-space. ";
  else
    out << theDrawingVariables.GetHtmlFromDrawOperationsCreateDivWithUniqueName(this->GetDim()-1);
  out << "<br>" << this->ElementToString(false, true, true, true, theFormat);
  return out.str();
}

bool Cone::DrawMeProjective
(Vector<Rational>* coordCenterTranslation, bool initTheDrawVars, DrawingVariables& theDrawingVariables, FormatExpressions& theFormat)
{ Vector<Rational> ZeroRoot, coordCenter;
  ZeroRoot.MakeZero(this->GetDim());
  if (coordCenterTranslation==0)
    coordCenter=ZeroRoot;
  else
    coordCenter=*coordCenterTranslation;
//  theDrawingVariables.theBuffer.init();
  Vectors<Rational> VerticesScaled=this->Vertices;
  for (int i=0; i<VerticesScaled.size; i++)
  { Rational sumAbsValuesCoords=0;
    for (int j=0; j<this->GetDim(); j++)
      sumAbsValuesCoords+=(VerticesScaled[i][j].IsPositive()) ? VerticesScaled[i][j] : -VerticesScaled[i][j];
    assert(!sumAbsValuesCoords.IsEqualToZero());
    VerticesScaled[i]/=sumAbsValuesCoords;
  }
  Vector<Rational> tempRoot;
  if (initTheDrawVars)
  { theDrawingVariables.theBuffer.MakeMeAStandardBasis(this->GetDim());
    for (int i=0; i<this->GetDim(); i++)
    { tempRoot.MakeEi(this->GetDim(), i);
      theDrawingVariables.drawLineBetweenTwoVectorsBuffer
      (ZeroRoot+coordCenter, tempRoot+coordCenter, theDrawingVariables.PenStyleNormal, CGI::RedGreenBlue(205,205,205));
    }
  }
  for (int i=0; i<this->Vertices.size; i++)
    theDrawingVariables.drawLineBetweenTwoVectorsBuffer
    (ZeroRoot+coordCenter, VerticesScaled[i]*10000+coordCenter, theDrawingVariables.PenStyleNormal, CGI::RedGreenBlue(180,180,180));
  for (int k=0; k<this->Normals.size; k++)
    for (int i=0; i<this->Vertices.size; i++)
      if (this->Normals[k].ScalarEuclidean(this->Vertices[i]).IsEqualToZero())
        for (int j=i+1; j<this->Vertices.size; j++)
          if(this->Normals[k].ScalarEuclidean(this->Vertices[j]).IsEqualToZero())
            if (this->IsAnHonest1DEdgeAffine(i,j))
              theDrawingVariables.drawLineBetweenTwoVectorsBuffer
              (VerticesScaled[i]+coordCenter, VerticesScaled[j]+coordCenter, theDrawingVariables.PenStyleNormal, CGI::RedGreenBlue(0,0,0));
  return true;
}

std::string Cone::DrawMeToHtmlProjective(DrawingVariables& theDrawingVariables, FormatExpressions& theFormat)
{ if (this->flagIsTheZeroCone)
    return "The cone is the zero cone (i.e. contains only the origin).";
  if (this->Normals.size<=0)
    return "The cone is the entire space.";
  std::stringstream out;
  if (this->Vertices.size<1)
  { out << "There has been a programming error. The cone is empty.<br>" << this->ElementToString(false, true, true, false, theFormat);
    return out.str();
  }
  theDrawingVariables.theBuffer.MakeMeAStandardBasis(this->GetDim());
  this->DrawMeProjective(0, true, theDrawingVariables, theFormat);
  theDrawingVariables.drawCoordSystemBuffer(theDrawingVariables, this->GetDim() ,0);
  out << theDrawingVariables.GetHtmlFromDrawOperationsCreateDivWithUniqueName(this->GetDim());
  out << "<br>" << this->ElementToString(false, true, true, false, theFormat);
  return out.str();
}

std::string CGI::GetHtmlButton
(const std::string& buttonID, const std::string& theScript, const std::string& buttonText)
{ std::stringstream out;
  out << "\n<button id=\"" << buttonID << "\" " << CGI::GetStyleButtonLikeHtml()
    << " onclick=\"" << theScript << "\">" << buttonText << "</button>";
  return out.str();
}

std::string CGI::GetHtmlSpanHidableStartsHidden
  (const std::string& input)
{ std::stringstream out;
  CGI::GlobalFormulaIdentifier ++;
  std::stringstream buttonLabel;
  std::stringstream spanLabel;
  spanLabel << "hidableSpan" << CGI::GlobalFormulaIdentifier;
  buttonLabel << "button" << CGI::GlobalFormulaIdentifier;
  out << CGI::GetHtmlButton(buttonLabel.str(), "switchMenu('"+spanLabel.str() +"');", "show/hide");
  out << "<span";
  out << " id=\"" << spanLabel.str() << "\" style=\"display: none\">";
  out << input << "</span>";
  return out.str();
}

std::string CGI::GetHtmlMathFromLatexFormulA
  (const std::string& input, const std::string& prependString, const std::string& appendStringBeforeButton,
   bool useDiv, bool useBeginArrayRCL)
{ std::stringstream out;
  CGI::GlobalFormulaIdentifier++;
  out << prependString;
  if (useDiv)
    out << "<div";
  else
    out << "<span";
  out << " id=\"theResult" << CGI::GlobalFormulaIdentifier << "\" class=\"math\" scale=\"50\">";
  if (useBeginArrayRCL)
    out << "\\begin{array}{rcl}&&\n";
  out << input;
  if (useBeginArrayRCL)
    out << "\n\\end{array}";
  if (useDiv)
    out << "</div><br>";
  else
    out << "</span>";
  out << "<textarea id=\"theResultLatex" << CGI::GlobalFormulaIdentifier << "\" style=\"display: none\">";
  if (useBeginArrayRCL)
    out << "\\begin{array}{rcl}&&\n";
  out << input ;
  if (useBeginArrayRCL)
    out << "\n\\end{array}";
  out << "</textarea>";
  if (useDiv)
    out << "\n<br>";
  out << appendStringBeforeButton;
  out << "\n<button id=\"ButtonToggleLatex"  << CGI::GlobalFormulaIdentifier
        << " \" " << CGI::GetStyleButtonLikeHtml() << " onclick=\"switchMenu('theResult" << GlobalFormulaIdentifier
        << "'); switchMenu('theResultLatex" << CGI::GlobalFormulaIdentifier << "');\"\">LaTeX show/hide</button>";
  return out.str();
}

int DrawOperations::GetDimFirstDimensionDependentOperation()
{ if (this->theDrawLineBetweenTwoRootsOperations.size>0)
    return this->theDrawLineBetweenTwoRootsOperations[0].v1.size;
  if (this->theDrawTextAtVectorOperations.size>0)
    return this->theDrawTextAtVectorOperations[0].theVector.size;
  return 2;
}

int DrawOperations::GetDimFromBilinearForm()
{ return this->theBilinearForm.NumRows;
}

void DrawOperations::initDimensions(int theDim, int numAnimationFrames)
{ if (theDim<2)
    theDim=2;
  this->theBilinearForm.MakeIdMatrix(theDim, 1, 0);
  this->ProjectionsEiVectors.SetSizeMakeMatrix(theDim, 2);
  Vectors<double> tempBasis;
  tempBasis.MakeEiBasis(theDim, 1, 0);
  tempBasis.size=2;
/*  for (int i=0; i<tempBasis[1].size; i++)
    tempBasis[1][i]=2*i+1;
  for (int i=0; i<tempBasis[0].size; i++)
    tempBasis[0][i]=3*i+2;*/
  this->ModifyToOrthonormalNoShiftSecond(tempBasis[1], tempBasis[0]);
//  this->ModifyToOrthonormalNoShiftSecond(tempBasis[0], tempBasis[1]);
  this->BasisProjectionPlane.initFillInObject(numAnimationFrames, tempBasis);
  this->BasisToDrawCirclesAt.MakeEiBasis(theDim, 1, 0);
  this->SelectedPlane=0;
  this->SelectedCircleMinus2noneMinus1Center=-2;
  this->centerX.initFillInObject(numAnimationFrames, 300);
  this->centerY.initFillInObject(numAnimationFrames, 300);
  this->GraphicsUnit.initFillInObject(numAnimationFrames, DrawOperations::GraphicsUnitDefault);
}

void DrawOperations::EnsureProperInitialization()
{ int theDim=this->GetDimFirstDimensionDependentOperation();
  bool isGood=(this->ProjectionsEiVectors.size==theDim && this->theBilinearForm.NumRows==theDim);
  int numFrames=this->BasisProjectionPlane.size;
  if (this->SelectedPlane>= this->BasisProjectionPlane.size)
  { numFrames=this->SelectedPlane+1;
    isGood=false;
  }
  if (isGood)
    isGood=(this->BasisProjectionPlane[this->SelectedPlane].size==2);
  if (isGood)
    isGood=(this->BasisProjectionPlane[this->SelectedPlane][0].size==theDim);
  if (!isGood)
    this->initDimensions(theDim, numFrames);
}

void WeylGroup::GetLongestWeylElt(ElementWeylGroup& outputWeylElt)
{ this->ComputeRho(false);
  Vector<Rational> lowest=this->rho;
//  std::cout << "rho: " << this->rho.ElementToString() << "<hr>";
  Vectors<Rational> tempRoots;
  tempRoots.MakeEiBasis(this->GetDim());
  this->GetLowestElementInOrbit(lowest, &outputWeylElt, tempRoots, false, false);
  //std::stringstream out;
  //out << outputWeylElt;
//  out << "\n<br>";
  //Matrix<Rational>  tempMat;
  //this->GetMatrixOfElement(outputWeylElt, tempMat);
  //out << tempMat.ElementToString(true, false);
  //std::cout << out.str();
  //std::cout << outputWeylElt;
 //std::cout << this->GetMatrixOfElement(
}

void WeylGroup::GetExtremeElementInOrbit
  (Vector<Rational>& inputOutput, ElementWeylGroup* outputWeylElt,
   Vectors<Rational>& bufferEiBAsis,
   bool findLowest, bool RhoAction, bool UseMinusRho, int* sign,
   bool* stabilizerFound)
{ if (outputWeylElt!=0)
    outputWeylElt->size=0;
  if (sign!=0)
    *sign=1;
  if (stabilizerFound!=0)
    *stabilizerFound=false;
  Rational theScalarProd;
//  static int numTimesReflectionWasApplied=0;

  for (bool found = true; found; )
  { found=false;
    for (int i=0; i<this->GetDim(); i++)
    { bool shouldApplyReflection=false;
      theScalarProd=this->RootScalarCartanRoot(inputOutput, bufferEiBAsis[i]);
      if (findLowest)
        shouldApplyReflection=theScalarProd.IsPositive();
      else
        shouldApplyReflection= theScalarProd.IsNegative();
      if (stabilizerFound!=0)
        if (theScalarProd.IsEqualToZero())
          *stabilizerFound=true;
      if (shouldApplyReflection)
      { found=true;
        this->SimpleReflection<Rational>(i, inputOutput, RhoAction, UseMinusRho, (Rational) 0);
        if (outputWeylElt!=0)
          outputWeylElt->AddOnTop(i);
        if (sign!=0)
          *sign*=-1;
//        numTimesReflectionWasApplied++;
      }
    }
  }
//  std::cout << "<hr># simple reflections applied total: " << numTimesReflectionWasApplied;
}

bool WeylGroup::IsEigenSpaceGeneratorCoxeterElement(Vector<Rational>& input)
{ ElementWeylGroup tempElt;
  this->GetCoxeterElement(tempElt);
  Matrix<Rational>  matCoxeterElt;
  this->GetMatrixOfElement(tempElt, matCoxeterElt);
  Vector<Rational> tempRoot=input;
  for (int i=0; i<this->GetDim(); i++)
    matCoxeterElt.ActOnVectorColumn(tempRoot);
  return tempRoot==input;
}

template<class Base>
std::iostream& operator<< (std::iostream& output, const CompleX<Base>& input)
{ if (input.IsEqualToZero())
  { output << "0";
    return output;
  }
  if (input.Re!=0)
  { output << input.Re;
    if (input.Im!=0)
      output << " + ";
  }
  if (input.Im!=0)
    output << "i";
  if (input.Im<0)
    output << "(";
  if (input.Im!=1)
    output << input.Im;
  if (input.Im<0)
    output << ")";
  return output;
}

void WeylGroup::GetMatrixReflection(Vector<Rational>& reflectionRoot, Matrix<Rational> & output)
{ Vectors<Rational> basis;
  int theDim=this->GetDim();
  basis.MakeEiBasis(theDim);
//  output.init(theDim, theDim);
  for (int i=0; i<theDim; i++)
    this->ReflectBetaWRTAlpha(reflectionRoot, basis[i], false, basis[i]);
  output.AssignRootsToRowsOfMatrix(basis);
  output.Transpose();
}

void rootSubalgebra::GetCoxeterElement(Matrix<Rational> & output)
{ int theDim=this->AmbientWeyl.GetDim();
  output.MakeIdMatrix(theDim);
  Matrix<Rational>  tempMat;
  for (int i=0; i<this->SimpleBasisK.size; i++)
  { this->AmbientWeyl.GetMatrixReflection(this->SimpleBasisK[i], tempMat);
    output.MultiplyOnTheLeft(tempMat);
  }
}

void rootSubalgebra::GetCoxeterPlane
  (Vector<double>& outputBasis1, Vector<double>& outputBasis2, GlobalVariables& theGlobalVariables)
{ //this->ComputeRho(true);
  int theDimension=this->AmbientWeyl.GetDim();
  if (theDimension<2)
    return;
  if (this->SimpleBasisK.size<2)
  { if (this->SimpleBasisK.size==1)
      outputBasis1=this->SimpleBasisK[0].GetVectorDouble();
    else
      outputBasis1.MakeEi(theDimension, 0);
    if(outputBasis1[0]==0)
      outputBasis2.MakeEi(theDimension, 0);
    else
      outputBasis2.MakeEi(theDimension, 1);
    return;
  }
  Vector<Rational> ZeroRoot;
  ZeroRoot.MakeZero(theDimension);
  Matrix<Rational>  matCoxeterElt;
  this->GetCoxeterElement(matCoxeterElt);
  std::cout << "the SA Coxeter matrix: " << matCoxeterElt.ElementToString(true, false);
//  tempMat=matCoxeterElt;
  this->ComputeDynkinDiagramKandCentralizer();
  ReflectionSubgroupWeylGroup tempGroup;
  int coxeterNumber=1;
  for (int i=0; i<this->theDynkinDiagram.SimpleBasesConnectedComponents.size; i++)
  { tempGroup.AmbientWeyl=this->AmbientWeyl;
    tempGroup.simpleGenerators=this->theDynkinDiagram.SimpleBasesConnectedComponents[i];
    tempGroup.ComputeRootSubsystem();
    Vector<Rational>& lastRoot= *tempGroup.RootSubsystem.LastObject();
    Vector<Rational> lastRootInSimpleCoords;
    lastRoot.GetCoordsInBasiS(tempGroup.simpleGenerators, lastRootInSimpleCoords);
    coxeterNumber=MathRoutines::Maximum
    (lastRootInSimpleCoords.SumCoords().NumShort, coxeterNumber)
    ;
  }
  std::cout << "<hr>the corresponding Coxeter number: " << coxeterNumber;
//  for (int i=0; i<coxeterNumber-1; i++)
//    tempMat.MultiplyOnTheLeft(matCoxeterElt);
//  std::cout << "<br>coxeter transformation to the power of " << coxeterNumber << " equals: " << tempMat.ElementToString(true, false);
  CompleX<double> theEigenValue;
  theEigenValue.Re= cos(2*MathRoutines::Pi()/coxeterNumber);
  theEigenValue.Im= sin(2*MathRoutines::Pi()/coxeterNumber);
  Matrix<CompleX<double> > eigenMat, idMat;
  eigenMat.init(matCoxeterElt.NumRows, matCoxeterElt.NumCols);
  for (int i =0; i<eigenMat.NumRows; i++)
    for (int j=0; j<eigenMat.NumCols; j++)
    { eigenMat.elements[i][j]=matCoxeterElt.elements[i][j].DoubleValue();
      if (i==j)
        eigenMat.elements[i][i]-=theEigenValue;
    }
  List<List<CompleX<double> > > theEigenSpaceList;
  eigenMat.FindZeroEigenSpacE
  (theEigenSpaceList, (CompleX<double>) 1, (CompleX<double>) -1, (CompleX<double>) 0,
   theGlobalVariables);
  Vectors<CompleX<double> > theEigenSpace;
  theEigenSpace.operator=(theEigenSpaceList);
  DrawOperations tempDO;
  tempDO.initDimensions(theDimension, 1);
  for (int i=0; i<theDimension; i++)
    for (int j=0; j<theDimension; j++)
      tempDO.theBilinearForm.elements[i][j]=
      this->AmbientWeyl.CartanSymmetric.elements[i][j].DoubleValue();
  Vector<double> tempRoot;
  outputBasis1.SetSize(theDimension);
  outputBasis2.SetSize(theDimension);
  if (theEigenSpace.size>0)
  { if (coxeterNumber>2)
    { for (int j=0; j<theDimension; j++)
      { outputBasis1[j]=theEigenSpace[0][j].Re;
        outputBasis2[j]=theEigenSpace[0][j].Im;
      }
      tempDO.ModifyToOrthonormalNoShiftSecond
      (outputBasis2, outputBasis1);
    } else if (coxeterNumber<=2 && theEigenSpace.size>1)
    { for (int j=0; j<theDimension; j++)
      { outputBasis1[j]=theEigenSpace[0][j].Re;
        outputBasis2[j]=theEigenSpace[1][j].Re;
      }
      tempDO.ModifyToOrthonormalNoShiftSecond
      (outputBasis2, outputBasis1);
    }
  }
}

void WeylGroup::GetCoxeterPlane
  (Vector<double>& outputBasis1, Vector<double>& outputBasis2, GlobalVariables& theGlobalVariables)
{ this->ComputeRho(true);
  Vector<Rational> ZeroRoot;
  int theDimension=this->GetDim();
  if (theDimension<2)
    return;
  ZeroRoot.MakeZero(theDimension);
  ElementWeylGroup tempElt;
  this->GetCoxeterElement(tempElt);
  Matrix<Rational>  matCoxeterElt, tempMat;
  this->GetMatrixOfElement(tempElt, matCoxeterElt);
//  std::cout << matCoxeterElt.ElementToString(true, false);
  tempMat=matCoxeterElt;
  int coxeterNumber=this->RootSystem.LastObject()->SumCoords().NumShort+1;
  for (int i=0; i<coxeterNumber-1; i++)
    tempMat.MultiplyOnTheLeft(matCoxeterElt);
//  std::cout << "<br>coxeter transformation to the power of " << coxeterNumber << " equals: " << tempMat.ElementToString(true, false);
  CompleX<double> theEigenValue;
  theEigenValue.Re= cos(2*MathRoutines::Pi()/coxeterNumber);
  theEigenValue.Im= sin(2*MathRoutines::Pi()/coxeterNumber);
  Matrix<CompleX<double> > eigenMat, idMat;
  eigenMat.init(matCoxeterElt.NumRows, matCoxeterElt.NumCols);
  for (int i =0; i<eigenMat.NumRows; i++)
    for (int j=0; j<eigenMat.NumCols; j++)
    { eigenMat.elements[i][j]=matCoxeterElt.elements[i][j].DoubleValue();
      if (i==j)
        eigenMat.elements[i][i]-=theEigenValue;
    }
  List<List<CompleX<double> > > theEigenSpaceList;
  eigenMat.FindZeroEigenSpacE
  (theEigenSpaceList, (CompleX<double>) 1, (CompleX<double>) -1, (CompleX<double>) 0,
   theGlobalVariables);
  Vectors<CompleX<double> > theEigenSpace;
  outputBasis1.SetSize(theDimension);
  outputBasis2.SetSize(theDimension);
  theEigenSpace.operator=(theEigenSpaceList);
  DrawOperations tempDO;
  tempDO.initDimensions(theDimension, 1);
  tempDO.GraphicsUnit[0]=DrawOperations::GraphicsUnitDefault;
  theEigenSpace.operator=(theEigenSpaceList);
  for (int i=0; i<theDimension; i++)
    for (int j=0; j<theDimension; j++)
      tempDO.theBilinearForm.elements[i][j]=this->CartanSymmetric.elements[i][j].DoubleValue();

  if (theEigenSpace.size>0)
  { if (coxeterNumber>2)
      for (int j=0; j<theDimension; j++)
      { outputBasis1[j]=theEigenSpace[0][j].Re;
        outputBasis2[j]=theEigenSpace[0][j].Im;
      }
    else if (coxeterNumber==1 && theEigenSpace.size>1 )
      for (int j=0; j<theDimension; j++)
      { outputBasis1[j]=theEigenSpace[0][j].Re;
        outputBasis2[j]=theEigenSpace[1][j].Re;
      }
    tempDO.ModifyToOrthonormalNoShiftSecond(outputBasis2, outputBasis1);
  }
}

void WeylGroup::DrawRootSystem
(DrawingVariables& outputDV, bool wipeCanvas, GlobalVariables& theGlobalVariables,
 bool drawWeylChamber, Vector<Rational>* bluePoint, bool LabelDynkinDiagramVertices, Vectors<Rational>* predefinedProjectionPlane)
{ DrawOperations& output=outputDV.theBuffer;
  this->ComputeRho(true);
  Vector<Rational> ZeroRoot;
  int theDimension=this->GetDim();
  ZeroRoot.MakeZero(theDimension);
  if (wipeCanvas)
    output.init();
  output.initDimensions(theDimension, 1);
  output.GraphicsUnit[0]=DrawOperations::GraphicsUnitDefault;
  for (int i=0; i<theDimension; i++)
    for (int j=0; j<theDimension; j++)
      output.theBilinearForm.elements[i][j]=this->CartanSymmetric.elements[i][j].DoubleValue();
  Vector<double> tempRoot;
  output.SelectedPlane=0;
  Vectors<double>& theTwoPlane= output.BasisProjectionPlane[0];
  if (predefinedProjectionPlane==0)
    this->GetCoxeterPlane(theTwoPlane[0], theTwoPlane[1], theGlobalVariables);
  else
    predefinedProjectionPlane->GetVectorsDouble(theTwoPlane);
  assert(theTwoPlane.size==2);
//  std::cout << "<hr><hr>the eigenspace: " << theEigenSpace.ElementToString(false, true, false);
//  std::stringstream tempStream;
//  tempStream << "<hr>the eigen mat:";
//  tempStream << eigenMat;
//  std::cout << tempStream.str();
  Vectors<Rational> RootSystemSorted;
  RootSystemSorted.CopyFromBase(this->RootSystem);
  List<double> lengths;
  lengths.SetSize(RootSystemSorted.size);
  for (int i=0; i<this->RootSystem.size; i++)
  { tempRoot.SetSize(theDimension);
    for (int j=0; j<theDimension; j++)
      tempRoot[j]=this->RootSystem[i][j].DoubleValue();
    double Length1 = this->RootScalarCartanRoot(tempRoot, output.BasisProjectionPlane[0][0]);
    double Length2 = this->RootScalarCartanRoot(tempRoot, output.BasisProjectionPlane[0][1]);
    lengths[i]=sqrt(Length1*Length1+Length2*Length2);
  }
  for (int i=0; i<RootSystemSorted.size; i++)
    for (int j=i; j<RootSystemSorted.size; j++)
      if (lengths[i]<lengths[j])
      { MathRoutines::swap(lengths[i], lengths[j]);
        MathRoutines::swap(RootSystemSorted[i], RootSystemSorted[j]);
      }
  Vector<Rational> differenceRoot;
  differenceRoot=RootSystemSorted[0]-RootSystemSorted[1];
  Rational minLength= this->RootScalarCartanRoot(differenceRoot, differenceRoot);
  for (int i=2; i<RootSystemSorted.size; i++)
  { differenceRoot=RootSystemSorted[0]-RootSystemSorted[i];
    if (minLength> this->RootScalarCartanRoot(differenceRoot, differenceRoot))
      minLength=this->RootScalarCartanRoot(differenceRoot, differenceRoot);
  }
//  std::cout << "<hr>the min length is: " << minLength.ElementToString();
  Rational tempRat;
  if (bluePoint!=0)
  { output.drawCircleAtVectorBuffer(*bluePoint, 5, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(0,0,255));
    output.drawCircleAtVectorBuffer(*bluePoint, 4, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(0,0,255));
    output.drawCircleAtVectorBuffer(*bluePoint, 3, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(0,0,255));
  }
  if (drawWeylChamber)
  { Cone theWeylChamber;
    this->GetWeylChamber(theWeylChamber, theGlobalVariables);
    FormatExpressions tempFormat;
    theWeylChamber.DrawMeProjective(0, false, outputDV, tempFormat);
  }
  theGlobalVariables.theDrawingVariables.DefaultHtmlHeight=600;
  theGlobalVariables.theDrawingVariables.DefaultHtmlWidth=600;
  output.centerX[0]=300;
  output.centerY[0]=300;
  for (int i=0; i<RootSystemSorted.size; i++)
  { int color=CGI::RedGreenBlue(0, 255, 0);
    output.drawLineBetweenTwoVectorsBuffer(ZeroRoot, RootSystemSorted[i], DrawingVariables::PenStyleNormal, color);
    output.drawCircleAtVectorBuffer(RootSystemSorted[i], 2, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(255,0,255));
    for (int j=i+1; j<RootSystemSorted.size; j++)
    { differenceRoot=RootSystemSorted[i]-RootSystemSorted[j];
      tempRat=this->RootScalarCartanRoot(differenceRoot, differenceRoot);
      if (minLength== tempRat)
        output.drawLineBetweenTwoVectorsBuffer(RootSystemSorted[i], RootSystemSorted[j], DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(0, 0, 255));
    }
  }
  Vector<Rational> tempRootRat;
  Vectors<Rational> epsNotationSimpleBasis;
  epsNotationSimpleBasis.MakeEiBasis(theDimension);
  this->GetEpsilonCoords(epsNotationSimpleBasis, epsNotationSimpleBasis, theGlobalVariables);
  for (int i=0; i<theDimension; i++)
  { tempRootRat.MakeEi(theDimension, i);
    output.drawCircleAtVectorBuffer(tempRootRat, 1, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(255,0,0));
    output.drawCircleAtVectorBuffer(tempRootRat, 3, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(255,0,0));
    output.drawCircleAtVectorBuffer(tempRootRat, 4, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(255,0,0));
    if (LabelDynkinDiagramVertices)
    { Vector<Rational>& current=epsNotationSimpleBasis[i];
      output.drawTextAtVectorBuffer
      (tempRootRat, current.ElementToStringLetterFormat("e", false, false),0, 10, DrawingVariables::TextStyleNormal);
    }
  }
  std::stringstream tempStream;
  tempStream << this->WeylLetter << this->GetDim() << " (" << SemisimpleLieAlgebra::GetLieAlgebraName
  (this->WeylLetter, this->GetDim(), true) << ")";
  if (this->GetDim()==2 && predefinedProjectionPlane!=0)
  { theTwoPlane[1][0]=1;
    theTwoPlane[1][1]=0;
    theTwoPlane[0][0]=0;
    theTwoPlane[0][1]=1;
    outputDV.theBuffer.ModifyToOrthonormalNoShiftSecond(theTwoPlane[0], theTwoPlane[1]);
  }
  output.drawTextBuffer(0, 0, tempStream.str(), 10, CGI::RedGreenBlue(0,0,0), DrawingVariables::TextStyleNormal);
}

int ParserNode::EvaluateG2ParabolicSupport
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ Vector<Rational> highestWeight, parSel;
  ParserNode& nodeHW=theNode.owner->TheObjects[theArgumentList[0]];
  ParserNode& nodeSel=theNode.owner->TheObjects[theArgumentList[1]];
  if(!nodeHW.GetRootRationalDontUseForFunctionArguments(highestWeight, theGlobalVariables))
    return theNode.SetError(theNode.errorProgramming);
  if (!nodeSel.GetRootRationalDontUseForFunctionArguments(parSel, theGlobalVariables))
    return theNode.SetError(theNode.errorProgramming);
  if (highestWeight.size!=2 || parSel.size!=2)
    return theNode.SetError(theNode.errorDimensionProblem);
  std::stringstream out;
  PiecewiseQuasipolynomial theChar;
  WeylGroup theWeyl;
  theWeyl.MakeG2();
  theWeyl.ComputeRho(true);
  Vectors<Rational> theNegativeRoots;
  theNegativeRoots.CopyFromBase(theWeyl.RootSystem);
  theNegativeRoots.size=6;
  out << "the negative Vectors<Rational> of G_2: " << theNegativeRoots.ElementToString();
  std::string tempS;
  theChar.MakeVPF(theNegativeRoots, tempS, theGlobalVariables);
  theGlobalVariables.theDrawingVariables.theBuffer.MakeMeAStandardBasis(2);
  for (int i=0; i<2; i++)
    for (int j=0; j<2; j++)
      theGlobalVariables.theDrawingVariables.theBuffer.theBilinearForm.elements[i][j]=
      theWeyl.CartanSymmetric.elements[i][j].DoubleValue();
  theChar.DrawMe(theGlobalVariables.theDrawingVariables,10);
//  out << theGlobalVariables.theDrawingVariables.GetHtmlFromDrawOperationsCreateDivWithUniqueName(2);
  theNode.outputString=out.str();
  theNode.ExpressionType=theNode.typeString;
  return theNode.errorNoError;
}

int ParserNode::EvaluateDrawRootSystem
  (ParserNode& theNode, char theWeylLetter, int theDimension, GlobalVariables& theGlobalVariables,
   Vector<Rational>* bluePoint, bool wipeCanvas, bool LabelDynkin, bool DrawWeylChamber, Vectors<Rational>* projectionPlane)
{ WeylGroup theWeyl;
  theWeyl.MakeArbitrary(theWeylLetter, theDimension);
  theNode.impliedNumVars=theDimension;
  DrawingVariables theDV;
  DrawOperations& theDrawOperators=theDV.theBuffer;
  theWeyl.DrawRootSystem(theDV, true, theGlobalVariables, DrawWeylChamber, bluePoint, LabelDynkin, projectionPlane);
  theGlobalVariables.theDrawingVariables.theBuffer=theDrawOperators;
  theGlobalVariables.theDrawingVariables.theBuffer.ComputeProjectionsEiVectors();
  std::stringstream out;
  out << "<hr>Below is a javascript visualization of a Vector<Rational> system drawn as described on John Stembridge's website"
  <<" (<a href=\"http://www.math.lsa.umich.edu/~jrs/coxplane.html\">"
  << "http://www.math.lsa.umich.edu/~jrs/coxplane.html</a>).<br>"
  << "The darker red points can be rotated around by dragging them with the mouse pointer; they denote the simple positive Vectors<Rational>.<br> "
  << "Mouse wheel zooms-in/out. <br>";
  out << "<hr>Root system " << SemisimpleLieAlgebra::GetLieAlgebraName(theWeylLetter, theDimension);
  out << theGlobalVariables.theDrawingVariables.GetHtmlFromDrawOperationsCreateDivWithUniqueName(theDimension);
//  out << "<br>Note that for a Vector<Rational> system of rank 4 or more there might be javascript-compilation lag.";
  theNode.outputString=out.str();
  theNode.theAnimation.GetElement().MakeZero();
  theNode.theAnimation.GetElement()+=theDrawOperators;
  theNode.ExpressionType=theNode.typeAnimation;
  theNode.theAnimation.GetElement().flagAnimating=true;
  theNode.theAnimation.GetElement().flagIsPausedWhileAnimating=true;
  return theNode.errorNoError;

}

int ParserNode::EvaluateAnimateRootSystem
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables, Vector<Rational>* bluePoint)
{ char theWeylLetter= theNode.owner->DefaultWeylLetter;
  int theDim= theNode.owner->DefaultWeylRank;
  int NumFrameS=theNode.owner->TheObjects[theArgumentList[0]].intValue;
  if(NumFrameS<1)
    NumFrameS=1;
//  std::cout << theArgumentList.size;
  if (theArgumentList.size<5 || theArgumentList.size%2 ==0)
    return theNode.SetError(theNode.errorBadOrNoArgument);
  Vectors<Rational> tempRootsX;
  tempRootsX.SetSize(theArgumentList.size-1);
  for (int i=1; i<theArgumentList.size; i++)
  { ParserNode& currentNode=theNode.owner->TheObjects[theArgumentList[i]];
    if (!currentNode.GetRootRationalDontUseForFunctionArguments(tempRootsX[i-1], theGlobalVariables))
      return theNode.SetError(theNode.errorBadOrNoArgument);
    else if (tempRootsX[i-1].size!=theDim)
      return theNode.SetError(theNode.errorDimensionProblem);

  }
  int result=ParserNode::EvaluateDrawRootSystem
  (theNode, theWeylLetter, theDim, theGlobalVariables, bluePoint, true);
  if (result==theNode.typeError)
    return result;
  theNode.ExpressionType=theNode.typeAnimation;
  DrawOperations theOps;
  theOps=theNode.theAnimation.GetElement().thePhysicalDrawOps[0];
  theOps.flagAnimatingMovingCoordSystem=true;
  Vectors<double> theFirstBasis=theOps.BasisProjectionPlane[0];
  Matrix<double> theForm=theOps.theBilinearForm;
  Vectors<double> theDraggableBasis=theOps.BasisToDrawCirclesAt;
  Vector<double> start1, start2, target1, target2;
  int numTransitions=(theArgumentList.size-3)/2;
  if (numTransitions<1)
    numTransitions=1;
  int NumFrameSPerTransition=NumFrameS/numTransitions;
  if (NumFrameSPerTransition<2)
    NumFrameSPerTransition=2;
  NumFrameS=NumFrameSPerTransition*numTransitions;
  theOps.initDimensions(theForm, theDraggableBasis, theFirstBasis, NumFrameS);
  int& indexBitMap=theOps.SelectedPlane;
  indexBitMap=-1;
  for (int j=0; j<numTransitions; j++)
  { start1=tempRootsX[j*2+0].GetVectorDouble();
    start2=tempRootsX[j*2+1].GetVectorDouble();
    target1=tempRootsX[j*2+2].GetVectorDouble();
    target2=tempRootsX[j*2+3].GetVectorDouble();
    theOps.ModifyToOrthonormalNoShiftSecond(target2, target1);
    theOps.ModifyToOrthonormalNoShiftSecond(start2, start1);
    for (int i=0; i<NumFrameSPerTransition; i++)
    { indexBitMap++;
      double fraction=0;
      if (NumFrameSPerTransition>1)
        fraction=((double) i)/((double)(NumFrameSPerTransition-1));
      theOps.BasisProjectionPlane[indexBitMap][0]=start1*(1-fraction);
      theOps.BasisProjectionPlane[indexBitMap][1]=start2*(1-fraction);
      theOps.BasisProjectionPlane[indexBitMap][0]+=target1*fraction;
      theOps.BasisProjectionPlane[indexBitMap][1]+=target2*fraction;
      theOps.ModifyToOrthonormalNoShiftSecond
      (theOps.BasisProjectionPlane[indexBitMap][1], theOps.BasisProjectionPlane[indexBitMap][0]);
    }
  }
  indexBitMap=0;
  theOps.drawTextBuffer(0, 15, "center can be moved", 0, 10, DrawingVariables::TextStyleNormal);
  theOps.drawTextBuffer(0, 30, "basis(=darker red dots) can be rotated", 0, 10, DrawingVariables::TextStyleNormal);
  theNode.theAnimation.GetElement().MakeZero();
  theNode.theAnimation.GetElement()+=theOps;
  theNode.theAnimation.GetElement().flagAnimating=true;
  theNode.theAnimation.GetElement().indexVirtualOp=0;
  theGlobalVariables.theDrawingVariables.theBuffer=theOps;
  theNode.outputString= theGlobalVariables.theDrawingVariables.
  GetHtmlFromDrawOperationsCreateDivWithUniqueName
  (theDim);
  return result;
}

void AnimationBuffer::operator=(const AnimationBuffer& other)
{ this->stillFrame=other.stillFrame;
  this->thePhysicalDrawOps=other.thePhysicalDrawOps;
  this->theVirtualOpS=other.theVirtualOpS;
  this->flagAnimating=other.flagAnimating;
  this->flagIsPausedWhileAnimating=other.flagIsPausedWhileAnimating;
  this->indexVirtualOp=other.indexVirtualOp;
  assert(this->indexVirtualOp<this->theVirtualOpS.size);

}

AnimationBuffer::AnimationBuffer()
{ this->stillFrame.init();
  this->MakeZero();
}

void AnimationBuffer::operator+=(const DrawOperations& other)
{ VirtualDrawOp theOp;
  theOp.theVirtualOp=this->typeDrawOps;
  theOp.indexPhysicalDrawOp=this->thePhysicalDrawOps.size;
  this->thePhysicalDrawOps.AddOnTop(other);
  theOp.indexPhysicalFrame=0;
  if (this->theVirtualOpS.size>0)
    theOp.indexPhysicalFrame=this->theVirtualOpS.LastObject()->indexPhysicalFrame+1;
  theOp.selectedPlaneInPhysicalDrawOp=0;
  this->theVirtualOpS.Reserve(this->theVirtualOpS.size+other.BasisProjectionPlane.size);
  for (int i=0; i<other.BasisProjectionPlane.size; i++)
  { this->theVirtualOpS.AddOnTop(theOp);
    theOp.indexPhysicalFrame++;
    theOp.selectedPlaneInPhysicalDrawOp++;
  }
}

void AnimationBuffer::AddPause(int numFrames)
{ VirtualDrawOp theVOp;
  theVOp.indexPhysicalFrame=this->GetNumPhysicalFramesNoStillFrame()-1;
  theVOp.selectedPlaneInPhysicalDrawOp=-1;
  theVOp.theVirtualOp=this->typePause;
  theVOp.indexPhysicalDrawOp=this->thePhysicalDrawOps.size-1;
  this->theVirtualOpS.Reserve(this->theVirtualOpS.size+numFrames);
  for (int i=0; i<numFrames; i++)
    this->theVirtualOpS.AddOnTop(theVOp);
}

void AnimationBuffer::MakeZero()
{ this->theVirtualOpS.size=0;
  this->thePhysicalDrawOps.size=0;
  this->indexVirtualOp=-1;
  this->flagAnimating=false;
  this->flagIsPausedWhileAnimating=false;
}

std::string VirtualDrawOp::ElementToString()
{ std::stringstream out;
  switch(this->theVirtualOp)
  { case AnimationBuffer::typeDrawOps:
      out << "draw operations; ";
      break;
    case AnimationBuffer::typeClearScreen:
      out << "clear screen;";
      break;
    case AnimationBuffer::typePause:
      out << "pause;";
      break;
    default:
      out << "type of draw function not documented";
      break;
  }
  out << " draw op: " << this->indexPhysicalDrawOp;
  out << " sel. plane: " << this->selectedPlaneInPhysicalDrawOp;
  out << " phys. frame index: " << this->indexPhysicalFrame;
  return out.str();
}

std::string AnimationBuffer::ElementToString()
{ std::stringstream out;
  for (int i=0; i<this->theVirtualOpS.size; i++)
    out << "Frame " << i << ": " << this->theVirtualOpS[i].ElementToString() << "\n";
  return out.str();
}

void AnimationBuffer::operator+=(const AnimationBuffer& other)
{ if (other.theVirtualOpS.size<=0)
    return;
  int physicalOpShift=this->thePhysicalDrawOps.size;
  int physicalFrameShift=0;
  if (this->theVirtualOpS.size>0)
    physicalFrameShift=this->theVirtualOpS.LastObject()->indexPhysicalFrame+1;
  this->thePhysicalDrawOps.Reserve(this->thePhysicalDrawOps.size+other.thePhysicalDrawOps.size);
  for (int i=0; i<other.thePhysicalDrawOps.size; i++)
    this->thePhysicalDrawOps.AddOnTop(other.thePhysicalDrawOps[i]);
  this->theVirtualOpS.Reserve(this->theVirtualOpS.size+other.theVirtualOpS.size);
  VirtualDrawOp currentOp;
  for (int i=0; i<other.theVirtualOpS.size; i++)
  { currentOp=other.theVirtualOpS[i];
    currentOp.indexPhysicalDrawOp+=physicalOpShift;
    currentOp.indexPhysicalFrame+=physicalFrameShift;
    this->theVirtualOpS.AddOnTop(currentOp);
  }
  if (this->thePhysicalDrawOps.size>0)
    this->indexVirtualOp=0;
  if (other.flagAnimating)
    this->flagAnimating=true;
}

int AnimationBuffer::GetIndexCurrentDrawOps()
{ if (!this->flagAnimating || this->thePhysicalDrawOps.size==0 || this->indexVirtualOp<0)
    return -1;
  assert(this->indexVirtualOp<this->theVirtualOpS.size);
  return this->theVirtualOpS[this->indexVirtualOp].indexPhysicalDrawOp;
}

DrawOperations& AnimationBuffer::GetCurrentDrawOps()
{ int theIndex=this->GetIndexCurrentDrawOps();
  if (theIndex==-1)
  { return this->stillFrame;
  }
  DrawOperations& result=this->thePhysicalDrawOps[theIndex];
  result.SelectedPlane=this->theVirtualOpS[this->indexVirtualOp].selectedPlaneInPhysicalDrawOp;
  if (result.SelectedPlane<0 || result.BasisProjectionPlane.size<=result.SelectedPlane)
    result.SelectedPlane=result.BasisProjectionPlane.size-1;
  return result;
}

int AnimationBuffer::GetNumPhysicalFramesNoStillFrame()
{ if (this->theVirtualOpS.size<=0)
    return 0;
  int result = this->theVirtualOpS.LastObject()->indexPhysicalFrame+1;
  if (result<=0)
    result=1;
  return result;
}

bool AnimationBuffer::IncrementOpReturnNeedsRedraw()
{ if (!this->flagAnimating)
    return false;
  if (this->theVirtualOpS.size<=0)
    return false;
  int oldPhysicalFrame=this->GetIndexCurrentPhysicalFrame();
  this->indexVirtualOp=(this->indexVirtualOp+1)% this->theVirtualOpS.size;
  return (oldPhysicalFrame!=this->GetIndexCurrentPhysicalFrame());
}

int AnimationBuffer::GetIndexCurrentPhysicalFrame()
{ if (this->theVirtualOpS.size<=0 || this->indexVirtualOp<0)
    return 0;
  assert(this->indexVirtualOp<this->theVirtualOpS.size);
  int result=this->theVirtualOpS[this->indexVirtualOp].indexPhysicalFrame;
  if (result<0)
    result=0;
  return result;
}

void AnimationBuffer::DrawNoInit(DrawingVariables& theDrawingVariables, GlobalVariables& theGlobalVariables)
{ theGlobalVariables.theIndicatorVariables.ProgressReportStringsNeedRefresh=true;
  int indexCurrentFrame=-2;
  int numTotalPhysicalFrames=this->GetNumPhysicalFramesNoStillFrame();
  for (this->indexVirtualOp=0;  this->indexVirtualOp<this->theVirtualOpS.size; this->indexVirtualOp++)
    if (this->GetIndexCurrentPhysicalFrame()!=indexCurrentFrame)
    { indexCurrentFrame=this->GetIndexCurrentPhysicalFrame();
      std::stringstream tempStream;
      tempStream << "Computing frame " << indexCurrentFrame+1 << " out of " << numTotalPhysicalFrames << " physical frames.";
      theGlobalVariables.theIndicatorVariables.ProgressReportStrings[0]=tempStream.str();
      theDrawingVariables.drawBufferNoIniT(this->GetCurrentDrawOps());
    }
  this->indexVirtualOp=0;
}

int ParserNode::EvaluateAnimateRootSystemBluePoint
(ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ Vector<Rational> tempRoot;
  ParserNode& aNode=theNode.owner->TheObjects[theArgumentList[3]];
  aNode.GetRootRationalDontUseForFunctionArguments(tempRoot, theGlobalVariables);
  return theNode.EvaluateAnimateRootSystem(theNode, theArgumentList, theGlobalVariables, & tempRoot);
}

int ParserNode::EvaluateAnimationPause
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ int NumFrames=theNode.owner->TheObjects[theArgumentList[0]].intValue;
  if(NumFrames<0)
    NumFrames=0;
  theNode.ExpressionType=theNode.typeAnimation;
  theNode.theAnimation.GetElement().MakeZero();
  theNode.theAnimation.GetElement().AddPause(NumFrames);
  return theNode.errorNoError;
}

int ParserNode::EvaluateDrawRootSystemCoxeterPlaneRootSA
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ DrawingVariables theDrawVars;
  DrawOperations& theDrawOps=theDrawVars.theBuffer;
  rootSubalgebras theRootSAs;
  char theWeylLetter=theNode.owner->DefaultWeylLetter;
  int theDim=theNode.owner->DefaultWeylRank;
  theRootSAs.GenerateAllReductiveRootSubalgebrasUpToIsomorphism
  (theGlobalVariables, theWeylLetter, theDim, true, false)
  ;
  int theIndex=theNode.owner->TheObjects[theArgumentList[0]].intValue;
  if (theIndex<0 || theIndex>=theRootSAs.size)
    return theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);

  theRootSAs.AmbientWeyl.DrawRootSystem(theDrawVars, true, theGlobalVariables, true);
  Vector<double> start1, start2;
  rootSubalgebra& currentSA=theRootSAs[theIndex];
  std::stringstream out;
  out << "The Dynkin diagram of the Vector<Rational> Subalgebra is " << currentSA.theDynkinDiagram.DynkinStrinG << ". ";
  if (currentSA.SimpleBasisK.size<2)
  { out << "<br>The subalgebra is of rank less than two, using default Coxeter plane instead.";
  } else
  { currentSA.GetCoxeterPlane(start1, start2, theGlobalVariables);
    theDrawOps.ModifyToOrthonormalNoShiftSecond(start2, start1);
    theDrawOps.BasisProjectionPlane[0][0]=start1;
    theDrawOps.BasisProjectionPlane[0][1]=start2;
  }
  theNode.ExpressionType=theNode.typeAnimation;
  theNode.theAnimation.GetElement().MakeZero();
  theNode.theAnimation.GetElement()+=(theDrawOps);
  theNode.theAnimation.GetElement().flagAnimating=false;
  theNode.ExpressionType=theNode.typeAnimation;
  out << theDrawVars.GetHtmlFromDrawOperationsCreateDivWithUniqueName(theDim);
  theNode.outputString=out.str();
  return theNode.errorNoError;
}

int ParserNode::EvaluateAnimateRootSAs
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ DrawingVariables theDrawingVars;
  DrawOperations& theDrawOps=theDrawingVars.theBuffer;
  rootSubalgebras theRootSAs;
  char theWeylLetter=theNode.owner->DefaultWeylLetter;
  int theDim=theNode.owner->DefaultWeylRank;
  theRootSAs.GenerateAllReductiveRootSubalgebrasUpToIsomorphism
  (theGlobalVariables, theWeylLetter, theDim, true, false)
  ;
  int NumTransitions=0;
  int numFramesPerTransition=20;
  if (numFramesPerTransition<3)
    numFramesPerTransition=3;
  for (int i=0; i<theRootSAs.size-1; i++)
  { rootSubalgebra& currentSA=theRootSAs[i];
    if (currentSA.SimpleBasisK.size<2)
      break;
    NumTransitions++;
  }
  NumTransitions--;
  theDrawOps.initDimensions(theDim, NumTransitions*numFramesPerTransition);
  theRootSAs.AmbientWeyl.DrawRootSystem(theDrawingVars, true, theGlobalVariables, true);
  Vector<double> start1, start2, target1, target2;
  //theGlobalVariables.MakeStatusReport("The SA: " + currentSA.theDynkinDiagram.DynkinStrinG);
  theDrawOps.BasisProjectionPlane.size=0;

  int FrameCounter=-1;
  for (int i=0; i<theRootSAs.size-1; i++)
  { rootSubalgebra& currentSA=theRootSAs[i];
    rootSubalgebra& nextSA=theRootSAs[i+1];
    if (currentSA.SimpleBasisK.size<2 || nextSA.SimpleBasisK.size<2)
      break;
    currentSA.GetCoxeterPlane(start1, start2, theGlobalVariables);
    nextSA.GetCoxeterPlane(target1, target2, theGlobalVariables);
    theDrawOps.ModifyToOrthonormalNoShiftSecond(start2, start1);
    theDrawOps.BasisProjectionPlane.SetSize(theDrawOps.BasisProjectionPlane.size+numFramesPerTransition);
    theDrawOps.centerX.SetSize(theDrawOps.BasisProjectionPlane.size);
    theDrawOps.centerY.SetSize(theDrawOps.BasisProjectionPlane.size);
    theDrawOps.GraphicsUnit.SetSize(theDrawOps.BasisProjectionPlane.size);
    for (int j=0; j<numFramesPerTransition; j++)
    { FrameCounter++;
      double fraction=((double) j)/((double) numFramesPerTransition-1);
      theDrawOps.BasisProjectionPlane[FrameCounter].SetSize(2);
      theDrawOps.BasisProjectionPlane[FrameCounter][0]=start1*(1-fraction)+ target1*fraction;
      theDrawOps.BasisProjectionPlane[FrameCounter][1]=start2*(1-fraction)+ target2*fraction;
      theDrawOps.ModifyToOrthonormalNoShiftSecond
      (theDrawOps.BasisProjectionPlane[FrameCounter][1], theDrawOps.BasisProjectionPlane[FrameCounter][0]);
      theDrawOps.centerX[FrameCounter]=theDrawOps.centerX[0];
      theDrawOps.centerY[FrameCounter]=theDrawOps.centerY[0];
      theDrawOps.GraphicsUnit[FrameCounter]=theDrawOps.GraphicsUnit[0];
    }
  }
  theNode.ExpressionType=theNode.typeAnimation;
  theNode.theAnimation.GetElement().MakeZero();
  theNode.theAnimation.GetElement()+=(theDrawOps);
  theNode.theAnimation.GetElement().flagAnimating=true;
  theNode.ExpressionType=theNode.typeAnimation;
  theNode.outputString=theDrawingVars.GetHtmlFromDrawOperationsCreateDivWithUniqueName(theDim);
  return theNode.errorNoError;
}

int ParserNode::EvaluateAnimationDots
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ char theWeylLetter= (char)theNode.owner->TheObjects[theArgumentList[0]].intValue;
  int theDim= theNode.owner->TheObjects[theArgumentList[1]].intValue;
  theWeylLetter+='A';
  CGI::MakeSureWeylGroupIsSane(theWeylLetter, theDim);
  WeylGroup theWeyl;
  theWeyl.MakeArbitrary('A', 2);
  DrawOperations theDrawOps;
  Vectors<double> theFirstBasis;
  theDim=2;
  theFirstBasis.MakeEiBasis(theDim, 1, 0);
  Vectors<double> theDraggableBasis;
  theDraggableBasis.MakeEiBasis(theDim, 1, 0);
  theDrawOps.init();
  theDrawOps.initDimensions(theWeyl.CartanSymmetric, theDraggableBasis, theFirstBasis, 1);
  theDrawOps.flagAnimatingMovingCoordSystem=true;
  ParserNode& weightNode=theNode.owner->TheObjects[theArgumentList[3]];
  Vectors<Rational> dots;
  int thenewDim;
  if (!weightNode.GetRootsEqualDimNoConversionNoEmptyArgument(weightNode.children, dots, thenewDim))
    return theNode.SetError(errorDimensionProblem);
  if (thenewDim!=theDim)
    return theNode.SetError(errorDimensionProblem);
  LargeIntUnsigned tempInt;
  Rational tempRat=theNode.owner->TheObjects[theArgumentList[2]].rationalValue;
  tempRat.GetNumUnsigned(tempInt);
  int theColor=tempInt.TheObjects[0];
//  for(int i=0; i<theArgumentList.size-3; i++)
//    extraDot[i]= theNode.owner->TheObjects[theArgumentList[i+3]].rationalValue;
  dots.MakeEiBasis(2);
  for (int i=0; i<dots.size;i++)
    theDrawOps.drawCircleAtVectorBuffer(dots[i], 2, DrawingVariables::PenStyleNormal, theColor);
  theDrawOps.SelectedPlane=0;
  theDrawOps.GraphicsUnit=10;
  theNode.ExpressionType=theNode.typeAnimation;
  theNode.theAnimation.GetElement().MakeZero();
  theNode.theAnimation.GetElement()+=(theDrawOps);
  theNode.theAnimation.GetElement().flagAnimating=true;
  theNode.ExpressionType=theNode.typeAnimation;

  return theNode.errorNoError;
}

int ParserNode::EvaluateAnimationClearScreen
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ char theWeylLetter= (char)theNode.owner->TheObjects[theArgumentList[0]].intValue;
  int theDim= theNode.owner->TheObjects[theArgumentList[1]].intValue;
  theWeylLetter+='A';
  CGI::MakeSureWeylGroupIsSane(theWeylLetter, theDim);
  WeylGroup theWeyl;
  theWeyl.MakeArbitrary(theWeylLetter, theDim);
  theWeyl.ComputeRho(true);
  DrawOperations theOps;
  Vectors<double> theFirstBasis;
  theFirstBasis.MakeEiBasis(theDim, 1, 0);
  Vectors<double> theDraggableBasis;
  theDraggableBasis.MakeEiBasis(theDim, 1, 0);
  theOps.init();
  theOps.initDimensions(theWeyl.CartanSymmetric, theDraggableBasis, theFirstBasis, 1);
  theOps.flagAnimatingMovingCoordSystem=true;

  theNode.ExpressionType=theNode.typeAnimation;
  theNode.theAnimation.GetElement().MakeZero();
  theNode.theAnimation.GetElement()+=theOps;
  theNode.theAnimation.GetElement().flagAnimating=true;
  return theNode.errorNoError;
}

int ParserNode::EvaluateDrawRootSystemOld
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables, Vector<Rational>* bluePoint)
{ char theWeylLetter= (char)theNode.owner->TheObjects[theArgumentList[0]].intValue;
  int theDimension= theNode.owner->TheObjects[theArgumentList[1]].intValue;
  theWeylLetter+='A';
  CGI::MakeSureWeylGroupIsSane(theWeylLetter, theDimension);
  return theNode.EvaluateDrawRootSystem(theNode, theWeylLetter, theDimension, theGlobalVariables, bluePoint);
}

int ParserNode::EvaluateDrawRootSystemLabelDynkin
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ char theWeylLetter= (char)theNode.owner->DefaultWeylLetter;
  int theDimension= theNode.owner->DefaultWeylRank;
  return theNode.EvaluateDrawRootSystem
  (theNode, theWeylLetter, theDimension, theGlobalVariables, 0, true, true, true);
}

int ParserNode::EvaluateDrawRootSystemFixedProjectionPlane
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ int theDim;
  Vectors<Rational> thePlane;
  if (!theNode.GetRootsEqualDimNoConversionNoEmptyArgument(theArgumentList, thePlane, theDim))
    return theNode.SetError(theNode.errorBadOrNoArgument);
  if (theDim!=theNode.owner->DefaultWeylRank)
    return theNode.SetError(theNode.errorDimensionProblem);
  if (thePlane.size!=2)
    return theNode.SetError(theNode.errorBadOrNoArgument);
  return theNode.EvaluateDrawRootSystem
    (theNode, theNode.owner->DefaultWeylLetter, theDim, theGlobalVariables, 0, true, false, false, & thePlane);
}

int ParserNode::EvaluateDrawRootSystem
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables, Vector<Rational>* bluePoint)
{ char theWeylLetter= (char)theNode.owner->DefaultWeylLetter;
  int theDimension= theNode.owner->DefaultWeylRank;
  return theNode.EvaluateDrawRootSystem(theNode, theWeylLetter, theDimension, theGlobalVariables, bluePoint);
}

double DrawOperations::getAngleFromXandY(double x, double y, double neighborX, double neighborY)
{ double result;
  if (x!=0)
   result= atan(y/x);
  else
    if (y>0)
      result= MathRoutines::Pi()/2;
    else
      result= MathRoutines::Pi()/(-2);
  return result;
}

void DrawOperations::click(double x , double y)
{ this->EnsureProperInitialization();
  this->SelectedCircleMinus2noneMinus1Center=-2;
  if (this->AreWithinClickTolerance(x, y, this->centerX[this->SelectedPlane], this->centerY[this->SelectedPlane]))
    this->SelectedCircleMinus2noneMinus1Center=-1;
  int theDim=this->theBilinearForm.NumRows;
  for (int i=0; i<theDim; i++)
  { double Xbasis, Ybasis;
    this->GetCoordsDrawingComputeAll(this->BasisToDrawCirclesAt[i], Xbasis, Ybasis);
    if (this->AreWithinClickTolerance(x, y, Xbasis, Ybasis))
    { this->SelectedCircleMinus2noneMinus1Center=i;
      return;
    }
  }
}

void DrawOperations::RotateOutOfPlane
  (std::stringstream& logger, Vector<double>& input, Vector<double>& output,
   Vector<double>& orthoBasis1, Vector<double>& orthoBasis2, double oldTanSquared, double newTanSquared)
{ Vector<double> projection= orthoBasis1;
  Vector<double> vComponent= input;
  double scal1= this->theBilinearForm.ScalarProduct(orthoBasis1, input);
  double scal2= this->theBilinearForm.ScalarProduct(orthoBasis2, input);
  projection*=scal1;
  projection+= orthoBasis2*scal2;
  vComponent-=projection;
  logger << "\ngetScalarProd=" << this->theBilinearForm.ScalarProduct(projection, vComponent);
  if (oldTanSquared<0 || newTanSquared<0)
    return;
  double oldAngle=atan(sqrt(oldTanSquared));
  double newAngle=atan(sqrt(newTanSquared));
  double angleChange=-oldAngle+newAngle;
  projection=orthoBasis1;
  projection*=cos(angleChange)*scal1-sin(angleChange)*scal2;
  projection+=orthoBasis2*(sin(angleChange)*scal1+sin(angleChange)*scal2);
  output = vComponent;
  output+=projection;
}

void DrawOperations::ModifyToOrthonormalNoShiftSecond
(Vector<double>& root1, Vector<double>& root2)
{ //if  (this->getScalarProduct(root2, root2)==0)
  //  root2.MakeEi(this->theWeyl.CartanSymmetric.NumRows,1);
  double theScalar= this->theBilinearForm.ScalarProduct(root1, root2)/this->theBilinearForm.ScalarProduct(root2, root2);
  root1-=root2*theScalar;
  this->ScaleToUnitLength(root1);
  this->ScaleToUnitLength(root2);
}

void DrawOperations::ComputeProjectionsEiVectors()
{ int theDimension=this->theBilinearForm.NumRows;
  this->ProjectionsEiVectors.SetSizeMakeMatrix(theDimension, 2);
  Vector<double> tempRoot;
  for (int i=0; i<theDimension; i++)
  { tempRoot.MakeEi(theDimension, i, 1, 0);
    this->ProjectionsEiVectors[i][0]=this->theBilinearForm.ScalarProduct(tempRoot, this->BasisProjectionPlane[this->SelectedPlane][0]);
    this->ProjectionsEiVectors[i][1]=this->theBilinearForm.ScalarProduct(tempRoot, this->BasisProjectionPlane[this->SelectedPlane][1]);
  }
}

void DrawOperations::ComputeXYsFromProjectionsEisAndGraphicsUnit()
{ for (int i=0; i<this->theDrawLineBetweenTwoRootsOperations.size; i++)
  { DrawLineBetweenTwoRootsOperation& theOperation= this->theDrawLineBetweenTwoRootsOperations[i];
    this->GetCoordsForDrawingProjectionsComputed(theOperation.v1, theOperation.v2, theOperation.precomputedX1, theOperation.precomputedY1, theOperation.precomputedX2, theOperation.precomputedY2);
  }
  for (int i=0; i<this->theDrawTextAtVectorOperations.size; i++)
  { DrawTextAtVectorOperation& theTextOperation=this->theDrawTextAtVectorOperations[i];
    this->GetCoordsForDrawingProjectionsComputed(theTextOperation.theVector, theTextOperation.precomputedX, theTextOperation.precomputedY);
  }
  for (int i=0; i<this->theDrawCircleAtVectorOperations.size; i++)
  { DrawCircleAtVectorOperation& theCircleOperation=this->theDrawCircleAtVectorOperations[i];
    this->GetCoordsForDrawingProjectionsComputed(theCircleOperation.theVector, theCircleOperation.precomputedX, theCircleOperation.precomputedY);
  }
}

void DrawOperations::changeBasisPreserveAngles(double newX, double newY, GlobalVariables& theGlobalVariables)
{ double bufferCenterX=this->centerX[this->SelectedPlane];
  double bufferCenterY=this->centerY[this->SelectedPlane];
  double bufferGraphicsUnit=this->GraphicsUnit[this->SelectedPlane];
  newX=(newX-bufferCenterX)/bufferGraphicsUnit;
  newY=(newY-bufferCenterY)/bufferGraphicsUnit;
  if (newX==0 && newY==0)
    return;
  std::stringstream out;
  Vector<double>& selectedRoot=this->BasisToDrawCirclesAt[this->SelectedCircleMinus2noneMinus1Center];
  double selectedRootLength=this->theBilinearForm.ScalarProduct(selectedRoot, selectedRoot);
  double oldX, oldY;
  this->GetCoordsDrawingComputeAll(selectedRoot, oldX, oldY);
  oldX=(oldX- bufferCenterX)/bufferGraphicsUnit;
  oldY=(oldY- bufferCenterY)/bufferGraphicsUnit;

  double oldAngle= getAngleFromXandY(oldX, oldY, newX, newY);
  double newAngle= getAngleFromXandY(newX, newY, oldX, oldY);
  double AngleChange= -newAngle+oldAngle;
  double epsilon=0.000000000001;
  while (AngleChange>MathRoutines::Pi()/2+epsilon)
  { AngleChange-=MathRoutines::Pi();
  }
  while (AngleChange<=MathRoutines::Pi()/(-2)-epsilon)
  { AngleChange+=MathRoutines::Pi();
  }
  out << "\nold angle: " << oldAngle;
  out << "\nnew angle:  " << newAngle;
  Vector<double> NewVectorE1, NewVectorE2;
  Vectors<double>& currentBasisPlane=this->BasisProjectionPlane[this->SelectedPlane];
  NewVectorE1= currentBasisPlane[0]*cos(AngleChange);
  NewVectorE1+=currentBasisPlane[1]*sin(AngleChange);
  NewVectorE2= currentBasisPlane[1]*cos(AngleChange);
  NewVectorE2+=currentBasisPlane[0]*(-sin(AngleChange));
  currentBasisPlane[0]=NewVectorE1;
  currentBasisPlane[1]=NewVectorE2;
  double RootTimesE1=this->theBilinearForm.ScalarProduct(selectedRoot, currentBasisPlane[0]);
  double RootTimesE2=this->theBilinearForm.ScalarProduct(selectedRoot, currentBasisPlane[1]);
  Vector<double> vOrthogonal=selectedRoot;
  Vector<double> vProjection=currentBasisPlane[0]*RootTimesE1;
  vProjection+=currentBasisPlane[1]*RootTimesE2;
  vOrthogonal-= vProjection;
  double oldRatioProjectionOverHeightSquared = (oldX*oldX+oldY*oldY)/ (selectedRootLength-oldX*oldX-oldY*oldY);
  double newRatioProjectionOverHeightSquared = (newX*newX+newY*newY)/ (selectedRootLength-newX*newX-newY*newY);
  out << "\noldRatio: " << oldRatioProjectionOverHeightSquared;
  out << "\nnewRatio: " << newRatioProjectionOverHeightSquared;
  if (this->theBilinearForm.ScalarProduct(vOrthogonal, vOrthogonal)>epsilon || this->theBilinearForm.ScalarProduct(vOrthogonal, vOrthogonal)<-epsilon)
  { this->ScaleToUnitLength(vProjection);
    this->ScaleToUnitLength(vOrthogonal);
    out << "\nscaled vOrthogonal=" << vOrthogonal.ElementToString() << "->" << this->theBilinearForm.ScalarProduct(vOrthogonal, vOrthogonal);
    out << "\nscaled vProjection=" << vProjection.ElementToString() << "->" <<this->theBilinearForm.ScalarProduct(vProjection, vProjection);
    out << "\ntheScalarProd: " << this->theBilinearForm.ScalarProduct(vOrthogonal, vProjection);
    this->RotateOutOfPlane(out, currentBasisPlane[0], currentBasisPlane[0], vProjection, vOrthogonal, oldRatioProjectionOverHeightSquared, newRatioProjectionOverHeightSquared);
    this->RotateOutOfPlane(out, currentBasisPlane[1], currentBasisPlane[1], vProjection, vOrthogonal, oldRatioProjectionOverHeightSquared, newRatioProjectionOverHeightSquared);
  }
//  this->e1.ComputeDebugString();
//  this->e2.ComputeDebugString();
  this->ModifyToOrthonormalNoShiftSecond(currentBasisPlane[0], currentBasisPlane[1]);
//  this->e1.ComputeDebugString();
//  this->e2.ComputeDebugString();
  out << "\ne1=" << currentBasisPlane[0].ElementToString();
  out << "\ne2=" << currentBasisPlane[1].ElementToString();
  out << "\ne1*e2=" << this->theBilinearForm.ScalarProduct(currentBasisPlane[0], currentBasisPlane[1]);
  if (this->specialOperationsOnBasisChange!=0)
    this->specialOperationsOnBasisChange(*this, theGlobalVariables);
  this->ComputeProjectionsEiVectors();
  this->DebugString= out.str();
}

class ImpreciseDouble
{
  private:
  double precision;
  double theValue;
  public:
  void ElementToString(std::string& output)
  { output=this->ElementToString();
  }
  std::string ElementToString()
  { std::stringstream out;
    out << this->theValue;
    return out.str();
  }
  void operator=(const ImpreciseDouble& other)
  { this->theValue=other.theValue;
    this->precision=other.precision;
  }
  void operator=(double other)
  { this->theValue=other;
  }
  ImpreciseDouble(const ImpreciseDouble& other)
  { this->operator=(other);
  }
  ImpreciseDouble()
  { this->theValue=0;
    this->precision=0.1;
  }
  ImpreciseDouble(double other)
  { this->operator=(other);
  }
  void operator+=(const ImpreciseDouble& other)
  { if (!other.IsEqualToZero())
      this->theValue+=other.theValue;
  }
  void operator-=(const ImpreciseDouble& other)
  { if (!other.IsEqualToZero())
      this->theValue-=other.theValue;
  }
  void operator=(const Rational& other)
  { this->theValue=other.DoubleValue();
  }
  bool IsEqualToZero()const
  { if (this->theValue<0)
      return (-theValue)< this->precision;
    return this->theValue<this->precision;
  }
  inline bool operator<=(const ImpreciseDouble& other)
  { return ! (other<*this);
  }
  bool IsPositive()const
  { return this->theValue>this->precision;
  }
  inline bool IsNegative()const
  { return *this<this->GetZero();
  }
  bool operator<(const ImpreciseDouble& other)const
  { ImpreciseDouble temp=other;
    temp-=*this;
    return temp.IsPositive();
  }
  void AssignFloor()
  { this->theValue=floor(this->theValue);
  }
  inline ImpreciseDouble operator/(const ImpreciseDouble& other)const
  { ImpreciseDouble result;
    result=*this;
    if (other.IsEqualToZero())
    { //the below is written like this to avoid this->theValue/0;
      //If the user attempts to divide by zero, I want a regular division by zero exception to be generated
      result.theValue=this->theValue/(other.theValue-other.theValue);
      return result;
    }
    result.theValue/=other.theValue;
    return result;
  }
  void operator*=(const ImpreciseDouble& other)
  { if (!other.IsEqualToZero())
      this->theValue*=other.theValue;
    else
      this->theValue=0;
  }
  bool operator==(const ImpreciseDouble& other)const
  { int diff=this->theValue-other.theValue;
    if (diff<0)
      diff=-diff;
    return diff<this->precision;
  }
  static inline ImpreciseDouble GetMinusOne()
  { return -1;
  }
  static inline ImpreciseDouble GetOne()
  { return 1;
  }
  static inline ImpreciseDouble GetZero()
  { return 0;
  }
};

int ParserNode::EvaluateLatticeImprecise
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ Vectors<Rational> tempRoots;
  int theDim=-1;
  if (!theNode.GetRootsEqualDimNoConversionNoEmptyArgument(theArgumentList, tempRoots, theDim))
    return theNode.SetError(theNode.errorBadOrNoArgument);
  Matrix<ImpreciseDouble> theMat;
  theMat.init(tempRoots.size, theDim);
  for (int i=0; i<theMat.NumRows; i++)
    for (int j=0; j<theMat.NumCols; j++)
      theMat.elements[i][j]=tempRoots[i][j];
  std::cout << "starting matrix: " << theMat.ElementToString(true, false);
  theMat.GaussianEliminationEuclideanDomain(ImpreciseDouble::GetMinusOne(), ImpreciseDouble::GetOne());
  std::cout << "<br>final matrix: " << theMat.ElementToString(true, false);
  return theNode.errorNoError;
}

void DrawOperations::projectionMultiplicityMergeOnBasisChange(DrawOperations& theOps, GlobalVariables& theGlobalVariables)
{ Matrix<ImpreciseDouble> theMat;
  int theDim=theOps.theBilinearForm.NumRows;
  theMat.init(theDim, 2);
//we assume that the ComputeProjectionsEiVectors has been called
//  theOps.ComputeProjectionsEiVectors();
  for(int i=0; i<theOps.ProjectionsEiVectors.size; i++)
    for (int j=0; j<2; j++)
      theMat.elements[i][j]=theOps.ProjectionsEiVectors[i][j];
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  std::stringstream out;
  out << "before elimination:\n" << theMat.ElementToString(false, false);
  theMat.GaussianEliminationEuclideanDomain(ImpreciseDouble::GetMinusOne(), ImpreciseDouble::GetOne());
  out << "after elimination:\n" << theMat.ElementToString(false, false);
  theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
  theGlobalVariables.MakeReport();

}

std::string WeylGroup::GenerateWeightSupportMethoD1
(Vector<Rational>& highestWeightSimpleCoords, Vectors<Rational>& outputWeightsSimpleCoords, int upperBoundWeights,
 GlobalVariables& theGlobalVariables)
{ HashedList<Vector<Rational> > theDominantWeights;
  double upperBoundDouble=100000/this->GetSizeWeylByFormula(this->WeylLetter, this->GetDim()).DoubleValue();
  int upperBoundInt = MathRoutines::Maximum((int) upperBoundDouble, 10000);
  //int upperBoundInt = 10000;
  Vector<Rational> highestWeightTrue=highestWeightSimpleCoords;
  this->RaiseToDominantWeight(highestWeightTrue);
  std::stringstream out;
  if (highestWeightTrue!=highestWeightSimpleCoords)
    out << "<br>Cheater! The input weight is not highest... using the highest weight in the same orbit instead. "
    << "Your input in simple coordinates was: "
    << highestWeightSimpleCoords.ElementToString() << ".<br> ";
  out << "The highest weight in simple coordinates is: " << highestWeightTrue.ElementToString() << ".<br>";
  std::string tempS;
  bool isTrimmed = !this->GetAlLDominantWeightsHWFDIM
  (highestWeightSimpleCoords, theDominantWeights, upperBoundInt, tempS, theGlobalVariables);

  out <<  tempS << "<br>";
  if (isTrimmed)
    out << "Trimmed the # of dominant weights - upper bound is " << upperBoundInt << ". <br>";
  else
    out << "Number of (non-strictly) dominant weights: " << theDominantWeights.size << "<br>";
  Vectors<Rational> tempRoots;
  HashedList<Vector<Rational> > finalWeights;
  int estimatedNumWeights=(int )
  (this->GetSizeWeylByFormula(this->WeylLetter, this->GetDim()).DoubleValue()*theDominantWeights.size);
  estimatedNumWeights= MathRoutines::Minimum(10000, estimatedNumWeights);
  finalWeights.Reserve(estimatedNumWeights);
  finalWeights.SetHashSizE(estimatedNumWeights);
  Vectors<Rational> dominantWeightsNonHashed;
  dominantWeightsNonHashed.CopyFromBase(theDominantWeights);
  this->GenerateOrbit(dominantWeightsNonHashed, false, finalWeights, false, 0, 0, 10000);
  if (finalWeights.size>=10000)
  { out << "Did not generate all weights of the module due to RAM limits. ";
    if (!isTrimmed)
      out << "However, all dominant weights were computed and are drawn.";
    out << "<br>";
  }
  if (!isTrimmed && finalWeights.size<10000)
    out << "All weights were computed and are drawn. <br>";
  outputWeightsSimpleCoords.CopyFromBase(finalWeights);
  return out.str();
}

int ParserNode::EvaluateDrawWeightSupport
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ //theNode.owner->theHmm.MakeG2InB3(theParser, theGlobalVariables);
  Vector<Rational> highestWeightFundCoords;
  if (!theNode.GetRootRationalFromFunctionArguments(highestWeightFundCoords, theGlobalVariables))
    return theNode.SetError(theNode.errorDimensionProblem);
  if (highestWeightFundCoords.size!=theNode.owner->DefaultWeylRank)
    return theNode.SetError(theNode.errorDimensionProblem);
  if (!highestWeightFundCoords.IsIntegral())
    return theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);
  //Vectors<Rational> fundamentalWeights;
  Vector<Rational> highestWeightSimpleCoords;
  WeylGroup& theWeyl=theNode.owner->theHmm.theRange().theWeyl;
  highestWeightSimpleCoords= theWeyl.GetSimpleCoordinatesFromFundamental(highestWeightFundCoords);
  //Vectors<Rational> theWeightsToBeDrawn;
  std::stringstream out;
  charSSAlgMod theChar;
  theChar.MakeFromWeight(highestWeightSimpleCoords, theNode.owner->theAlgebras, 0);
  DrawingVariables theDV;
  std::string report;
  theChar.DrawMeNoMults(report, theGlobalVariables, theDV, 10000);
  out << report << theDV.GetHtmlFromDrawOperationsCreateDivWithUniqueName(theWeyl.GetDim());

  theNode.theAnimation.GetElement().MakeZero();
  theNode.theAnimation.GetElement()+=theDV.theBuffer;
  theNode.ExpressionType=theNode.typeAnimation;
  theNode.outputString=out.str();
//  theGlobalVariables.theDrawingVariables.theBuffer=theOps;
//std::cout << "I am alive!";
  return theNode.errorNoError;
}

int ParserNode::EvaluateDrawWeightSupportWithMults
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ //theNode.owner->theHmm.MakeG2InB3(theParser, theGlobalVariables);
  Vector<Rational> highestWeightFundCoords;
  if (!theNode.GetRootRationalFromFunctionArguments(highestWeightFundCoords, theGlobalVariables))
    return theNode.SetError(theNode.errorDimensionProblem);
  if (highestWeightFundCoords.size!=theNode.owner->DefaultWeylRank)
    return theNode.SetError(theNode.errorDimensionProblem);
  if (!highestWeightFundCoords.IsIntegral())
    return  theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);
  //Vectors<Rational> fundamentalWeights;
  Vector<Rational> highestWeightSimpleCoords;
  WeylGroup& theWeyl=theNode.owner->theHmm.theRange().theWeyl;
  highestWeightSimpleCoords= theWeyl.GetSimpleCoordinatesFromFundamental(highestWeightFundCoords);
  //Vectors<Rational> theWeightsToBeDrawn;
  std::stringstream out;
  charSSAlgMod theChar;
  theChar.MakeFromWeight(highestWeightSimpleCoords, theNode.owner->theAlgebras, 0);
  DrawingVariables theDV;
  std::string report;
  theChar.DrawMeWithMults(report, theGlobalVariables, theDV, 10000);
  out << report << theDV.GetHtmlFromDrawOperationsCreateDivWithUniqueName(theWeyl.GetDim());
  theNode.theAnimation.GetElement().MakeZero();
  theNode.theAnimation.GetElement()+=theDV.theBuffer;
  theNode.ExpressionType=theNode.typeAnimation;
  theNode.outputString=out.str();
  return theNode.errorNoError;
}

void DrawOperations::operator+=(const DrawOperations& other)
{ if (this->theBilinearForm.NumRows!=other.theBilinearForm.NumRows)
    return;
  this->TypeNthDrawOperation.AddListOnTop(other.TypeNthDrawOperation);
  int shiftDrawText=this->theDrawTextOperations.size;
  int shiftDrawTextAtVector=this->theDrawTextAtVectorOperations.size;
  int shiftDrawLine=this->theDrawLineOperations.size;
  int shiftDrawLineBnVectors=this->theDrawLineBetweenTwoRootsOperations.size;
  int shiftDrawCircleAtVector=this->theDrawCircleAtVectorOperations.size;
  this->IndexNthDrawOperation.Reserve
  (this->IndexNthDrawOperation.size+other.IndexNthDrawOperation.size);
  for (int i=0; i<other.TypeNthDrawOperation.size; i++)
    switch(other.TypeNthDrawOperation[i])
    { case DrawOperations::typeDrawCircleAtVector:
        this->IndexNthDrawOperation.AddOnTop(other.IndexNthDrawOperation[i]+shiftDrawCircleAtVector);
        break;
      case DrawOperations::typeDrawLine:
        this->IndexNthDrawOperation.AddOnTop(other.IndexNthDrawOperation[i]+shiftDrawLine);
        break;
      case DrawOperations::typeDrawLineBetweenTwoVectors:
        this->IndexNthDrawOperation.AddOnTop(other.IndexNthDrawOperation[i]+shiftDrawLineBnVectors);
        break;
      case DrawOperations::typeDrawText:
        this->IndexNthDrawOperation.AddOnTop(other.IndexNthDrawOperation[i]+shiftDrawText);
        break;
      case DrawOperations::typeDrawTextAtVector:
        this->IndexNthDrawOperation.AddOnTop(other.IndexNthDrawOperation[i]+shiftDrawTextAtVector);
        break;
      default:
        assert(false);
        break;
    }
  this->theDrawTextOperations.AddListOnTop(other.theDrawTextOperations);
  this->theDrawLineOperations.AddListOnTop(other.theDrawLineOperations);
  this->theDrawLineBetweenTwoRootsOperations.AddListOnTop(other.theDrawLineBetweenTwoRootsOperations);
  this->theDrawTextAtVectorOperations.AddListOnTop(other.theDrawTextAtVectorOperations);
  this->theDrawCircleAtVectorOperations.AddListOnTop(other.theDrawCircleAtVectorOperations);
  //this->BasisProjectionPlane.AddListOnTop(other.BasisProjectionPlane);
  //this->centerX.AddListOnTop(other.centerX);
  //this->centerY.AddListOnTop(other.centerY);
  //this->GraphicsUnit.AddListOnTop(other.GraphicsUnit);
  this->SelectedPlane=0;
}

DrawOperations& AnimationBuffer::GetLastDrawOps()
{ if (this->thePhysicalDrawOps.size<=0)
    return this->stillFrame;
  return *this->thePhysicalDrawOps.LastObject();
}

std::string ElementWeylGroup::ElementToString
  (int NumSimpleGens, bool useLatex, bool useHtml, const std::string& simpleRootLetter,
   const std::string& outerAutoLetter, List<int>* DisplayIndicesOfSimpleRoots)
{ if (this->size==0)
    return "id";
  std::stringstream out;
  for (int i=this->size-1; i>=0; i--)
    if (NumSimpleGens<0 || this->TheObjects[i]<NumSimpleGens)
    { out << "s_{" << simpleRootLetter << "_{";
      if (DisplayIndicesOfSimpleRoots==0)
        out << this->TheObjects[i]+1;
      else
        out << (*DisplayIndicesOfSimpleRoots)[this->TheObjects[i]];
      out << "} }";
    }
    else
      out << outerAutoLetter << "_{" << this->TheObjects[i]-NumSimpleGens+1 << "}";
  return out.str();
}

int ParserNode::EvaluateParabolicWeylGroups
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ WeylGroup& theAmbientWeyl=theNode.owner->theHmm.theRange().theWeyl;
  Selection parabolicSel;
  parabolicSel.init(theAmbientWeyl.GetDim());
  int numCycles=MathRoutines::TwoToTheNth(parabolicSel.MaxSize);
  ReflectionSubgroupWeylGroup theSubgroup;
  std::stringstream out;
  for (int i=0; i<numCycles; i++, parabolicSel.incrementSelection())
  { theSubgroup.MakeParabolicFromSelectionSimpleRoots(theAmbientWeyl, parabolicSel, theGlobalVariables, 2000);
    out << "<hr>" << CGI::GetHtmlMathDivFromLatexFormulA(theSubgroup.ElementToString());
  }
  theNode.outputString=out.str();
  theNode.ExpressionType=theNode.typeString;
  return theNode.errorNoError;
}

int ParserNode::EvaluateActByWeylAlgebraElement
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ Polynomial<Rational> & result= theNode.polyValue.GetElement();
  ElementWeylAlgebra& left = theNode.owner->TheObjects[theArgumentList[0]].WeylAlgebraElement.GetElement();
  Polynomial<Rational> & right = theNode.owner->TheObjects[theArgumentList[1]].polyValue.GetElement();
  theNode.impliedNumVars=MathRoutines::Maximum(left.NumVariables, right.NumVars);
  result=right;
  result.SetNumVariablesSubDeletedVarsByOne(theNode.impliedNumVars);
  left.SetNumVariables(theNode.impliedNumVars);
  left.ActOnPolynomial(result);
  theNode.ExpressionType=theNode.typePoly;
  return theNode.errorNoError;
}

bool ElementWeylAlgebra::ActOnPolynomial(Polynomial<Rational>& thePoly)
{ assert(thePoly.NumVars==this->NumVariables);
  Polynomial<Rational> result;
  result.MakeZero(this->NumVariables);
  MonomialP tempMon;
  Rational coeffMon;
  for (int i=0; i<this->StandardOrder.size; i++)
  { MonomialP& currentActingMon=this->StandardOrder[i];
    if (!currentActingMon.IsIntegral())
    { std::cout << "Element weyl algebra called inappropriately. Not raising alarm, although I should. ";
      return false;
    }
    for (int j=0; j<thePoly.size; j++)
    { MonomialP& currentMon=thePoly[j];
      tempMon=currentMon;
      coeffMon=thePoly.theCoeffs[j];
      coeffMon*=this->StandardOrder.theCoeffs[i];
      for (int k=0; k<this->NumVariables; k++)
      { for (int numDiff=currentActingMon[k+this->NumVariables].NumShort; numDiff>0; numDiff--)
        { coeffMon*=tempMon[k];
          tempMon[k]-=1;
        }
        tempMon[k]+=currentActingMon[k];
      }
      result.AddMonomial(tempMon, coeffMon);
    }
  }
  thePoly=result;
  return true;
}

std::string ReflectionSubgroupWeylGroup::ElementToStringFromLayersAndArrows
(List<List<List<int> > >& arrows, List<List<int> >& Layers, int GraphWidth, bool useAmbientIndices)
{ std::stringstream out;
//  std::cout << this->simpleGenerators.ElementToString();
  List<int> DisplayIndicesSimpleGenerators;
  if (!useAmbientIndices)
  { DisplayIndicesSimpleGenerators.SetSize(this->simpleGenerators.size);
    for (int i=0; i<this->simpleGenerators.size; i++)
      DisplayIndicesSimpleGenerators[i]=this->AmbientWeyl.RootsOfBorel.IndexOfObject(this->simpleGenerators[i])+1;
  }
  out << "\\xymatrix{";
  bool GraphWidthIsOdd=((GraphWidth%2)!=0);
  if (!GraphWidthIsOdd)
    GraphWidth++;
  for (int i=0; i<Layers.size; i++)
  { int currentRowOffset=(GraphWidth-Layers[i].size)/2;
    int nextRowOffset=-1;
    if (i<Layers.size-1)
      nextRowOffset=(GraphWidth-Layers[i+1].size)/2;
    for (int j=0; j<currentRowOffset; j++)
      out << "&";
    for (int j=0; j<Layers[i].size; j++)
    { if (!useAmbientIndices)
        out << this->TheObjects[Layers[i][j]].ElementToString(true, false, "\\eta", &DisplayIndicesSimpleGenerators);
      else
        out << this->RepresentativesQuotientAmbientOrder[Layers[i][j]].ElementToString(true, false, "\\eta", 0);
      int currentOffset=j+currentRowOffset;
      if (Layers[i].size%2==0)
        if (currentOffset>=GraphWidth/2)
          currentOffset++;
      for (int k=0; k<arrows[i][j].size; k++)
      { out << " \\ar[d";
        int indexInLayer=Layers[i+1].IndexOfObject(arrows[i][j][k]);
        assert(indexInLayer!=-1);
        int nextOffset=indexInLayer+nextRowOffset;
        if (Layers[i+1].size%2==0)
          if (nextOffset>=GraphWidth/2)
            nextOffset++;
        int actualOffset=-currentOffset+nextOffset;
        for (int l=0; l<actualOffset; l++)
          out << "r";
        for (int l=0; l>actualOffset; l--)
          out << "l";
        out << "]";
      }
      out << " & ";
      if (Layers[i].size%2==0 && j==Layers[i].size/2-1)
        out << " & ";
    }
    out << " \\\\\n";
  }
  out << "}";
  return out.str();

}

std::string ReflectionSubgroupWeylGroup::ElementToStringBruhatGraph()
{ if (this->size<1)
    return "Error, non-initialized group";
  if (this->size==1)
    return "id";
  List<List<List<int> > > arrows;
  List<List<int> > Layers;
  Vector<Rational> tempRoot;
  Layers.Reserve(this->size);
  int GraphWidth=1;
  int oldLayerElementLength=-1;
  for (int i=0; i< this->size; i++)
  { if (this->TheObjects[i].size!=oldLayerElementLength)
    { Layers.SetSize(Layers.size+1);
      oldLayerElementLength=this->TheObjects[i].size;
    }
    Layers.LastObject()->AddOnTop(i);
    GraphWidth=MathRoutines::Maximum(GraphWidth, Layers.LastObject()->size);
  }
  HashedList<Vector<Rational> > orbit;
  orbit.Reserve(this->size);
  for (int i=0; i<this->size; i++)
  { this->ActByElement(i, this->AmbientWeyl.rho, tempRoot);
    orbit.AddOnTop(tempRoot);
  }
  arrows.SetSize(Layers.size);
  for (int i=0; i< Layers.size; i++)
  { arrows[i].SetSize(Layers[i].size);
    for (int j=0; j<Layers[i].size; j++)
      for (int k=0; k<this->simpleGenerators.size; k++)
      { this->AmbientWeyl.ReflectBetaWRTAlpha(this->simpleGenerators[k], orbit[Layers[i][j]], false, tempRoot);
        int index=orbit.GetIndex(tempRoot);
        assert(index!=-1);
        if (this->TheObjects[index].size>this->TheObjects[Layers[i][j]].size)
          arrows[i][j].AddOnTop(index);
      }
  }
  return this->ElementToStringFromLayersAndArrows(arrows, Layers, GraphWidth, false);
}

void ReflectionSubgroupWeylGroup::ElementToString(std::string& output, bool displayElements)
{ std::stringstream out, head, head2;
  List<int> DisplayIndicesSimpleGenerators;
  DisplayIndicesSimpleGenerators.SetSize(this->simpleGenerators.size);
  for (int i=0; i<this->simpleGenerators.size; i++)
    DisplayIndicesSimpleGenerators[i]=this->AmbientWeyl.RootsOfBorel.IndexOfObject(this->simpleGenerators[i])+1;
  DynkinDiagramRootSubalgebra tempDyn;
  tempDyn.ComputeDiagramTypeKeepInput(this->simpleGenerators, this->AmbientWeyl);
  out << "Dynkin diagram & subalgebra of Vector<Rational> subsystem generated by the given Vectors<Rational>: "
  << tempDyn.DynkinStrinG;
  out << "<br>Simple Vectors<Rational>:\n<br>\n ";
  head << "\\begin{array}{rcl}";
  for (int i=0; i<this->simpleGenerators.size; i++)
    head << "\n\\eta_{" << DisplayIndicesSimpleGenerators[i] << "}&:=&" << this->simpleGenerators[i].ElementToString()
    << "\\\\";
  head << "\\end{array}";
  out << CGI::GetHtmlMathSpanFromLatexFormula(head.str());
  if (this->ExternalAutomorphisms.size>0)
  { out << "<br>Outer automorphisms: \n";
    Matrix<Rational>  tempMat;
    head2 << "\\begin{array}{rcl}";
    for (int i=0; i<this->ExternalAutomorphisms.size; i++)
    { tempMat.AssignRootsToRowsOfMatrix(this->ExternalAutomorphisms[i]);
      tempMat.Transpose();
      head2 << "a_{" << i+1 << "}&:=&" << tempMat.ElementToString(false, true) << "\\\\";
    }
    head2 << "\\end{array}";
    out << CGI::GetHtmlMathDivFromLatexFormulA(head2.str());
  }
  out << "<br>Half sum of the positive Vectors<Rational>: " << this->GetRho().ElementToString();
  if (displayElements)
  { std::stringstream body;
    out << "<br>The elements of the weyl group of the subgroup written with minimal # of generators:<br>";
    body << "\\begin{array}{l}";
    for (int i=0; i<this->size; i++)
    { ElementWeylGroup& currentElt=this->TheObjects[i];
      body << currentElt.ElementToString(this->simpleGenerators.size, true, false, "\\eta", "a", &DisplayIndicesSimpleGenerators)
      << "\\\\";
    }
    body << "\\end{array}";
    out << CGI::GetHtmlMathSpanFromLatexFormula(body.str());
  }
  output=out.str();
}

int ParserNode::EvaluateParabolicWeylGroupsBruhatGraph
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ WeylGroup& theAmbientWeyl=theNode.owner->theHmm.theRange().theWeyl;
  Vector<Rational> tempRoot;
  if (!theNode.GetRootRationalFromFunctionArguments(tempRoot, theGlobalVariables))
    return theNode.SetError(theNode.errorBadOrNoArgument);
  if (tempRoot.size!=theAmbientWeyl.GetDim())
    return theNode.SetError(theNode.errorDimensionProblem);
  Selection parabolicSel;
  parabolicSel=tempRoot;
  ReflectionSubgroupWeylGroup theSubgroup;
  std::stringstream out;
  std::fstream outputFile, outputFile2;
  std::string fileName, filename2;
  fileName=theNode.owner->PhysicalNameDefaultOutput+"1";
  filename2=theNode.owner->PhysicalNameDefaultOutput+"2";
  CGI::OpenFileCreateIfNotPresent(outputFile, fileName+".tex", false, true, false);
  CGI::OpenFileCreateIfNotPresent(outputFile2, filename2+".tex", false, true, false);
  theSubgroup.MakeParabolicFromSelectionSimpleRoots(theAmbientWeyl,  parabolicSel, theGlobalVariables, 500);
  theSubgroup.FindQuotientRepresentatives(2000);
  out << "<br>Number elements of the coset: " << theSubgroup.RepresentativesQuotientAmbientOrder.size;
  out << "<br>Number of elements of the Weyl group of the Levi part: " << theSubgroup.size;
  out << "<br>Number of elements of the ambient Weyl: " << theSubgroup.AmbientWeyl.size;
  outputFile << "\\documentclass{article}\\usepackage[all,cmtip]{xy}\\begin{document}\n";
  outputFile2 << "\\documentclass{article}\\usepackage[all,cmtip]{xy}\\begin{document}\n";
  if (theSubgroup.size>498)
  { if (theSubgroup.AmbientWeyl.GetSizeWeylByFormula('E', 6) <=
        theSubgroup.AmbientWeyl.GetSizeWeylByFormula(theAmbientWeyl.WeylLetter, theAmbientWeyl.GetDim()))
      out << "Even I can't handle the truth, when it is so large<br>";
    else
      out << "LaTeX can't handle handle the truth, when it is so large. <br>";
  }
  else
  { outputFile << "\\[" << theSubgroup.ElementToStringBruhatGraph() << "\\]";
    outputFile << "\n\\end{document}";
    outputFile2 << "\\[" << theSubgroup.ElementToStringCosetGraph() << "\\]";
    outputFile2 << "\n\\end{document}";
    out << "<hr><b>The .png file might be bad if LaTeX crashed while trying to process it; \
    please check whether the .tex corresponds to the .png!</b><br>"
    << " The requested Bruhat graph is located in a png file here: <a href=\""
    << theNode.owner->DisplayNameDefaultOutput << "1.tex\"> " <<  theNode.owner->DisplayNameDefaultOutput << "1.tex</a>";
    out << ", <a href=\"" << theNode.owner->DisplayNameDefaultOutput << "1.png\">" << theNode.owner->DisplayNameDefaultOutput << "1.png</a>";
    out << "<hr><hr><b>The .png file might be bad if LaTeX crashed while trying to process it; \
    please check whether the .tex corresponds to the .png! </b><br>"
    << " The coset graph is located in a png file here: <a href=\"" << theNode.owner->DisplayNameDefaultOutput
    << "2.tex\"> " <<  theNode.owner->DisplayNameDefaultOutput << "2.tex</a>";
    out << ", <a href=\"" << theNode.owner->DisplayNameDefaultOutput << "2.png\"> "
    << theNode.owner->DisplayNameDefaultOutput << "2.png</a>";

    out << "<hr>Additional printout follows.<br> ";
    out << "<br>Representatives of the coset follow. Below them you can find the elements of the subgroup. <br>";
    for (int i=0; i<theSubgroup.RepresentativesQuotientAmbientOrder.size; i++)
    { ElementWeylGroup& current=theSubgroup.RepresentativesQuotientAmbientOrder[i];
      out << "<br>" << current.ElementToString();
    }
    out << "<hr>";
    out << theSubgroup.ElementToString();
  }

  theNode.outputString=out.str();
//  theCommand << "pdflatex -output-directory=" << theNode.owner->outputFolderPath << "   " << fileName ;

  theNode.owner->SystemCommands.AddOnTop
  ("latex  -output-directory=" + theNode.owner->PhysicalPathOutputFolder + " " + fileName + ".tex");
  theNode.owner->SystemCommands.AddOnTop("dvipng " + fileName + ".dvi -o " + fileName + ".png -T tight");
  theNode.owner->SystemCommands.AddOnTop
  ("latex  -output-directory=" + theNode.owner->PhysicalPathOutputFolder + " " + filename2 + ".tex");
  theNode.owner->SystemCommands.AddOnTop("dvipng " + filename2 + ".dvi -o " + filename2 + ".png -T tight");
  theNode.outputString=out.str();
  theNode.ExpressionType=theNode.typeString;
  return theNode.errorNoError;
}

void Parser::initTestAlgebraNeedsToBeRewrittenG2InB3(GlobalVariables& theGlobalVariables)
{ if(this->DefaultWeylLetter!='B' || this->DefaultWeylRank!=3)
    return;
  this->theHmm.MakeG2InB3(*this, theGlobalVariables);
//  SSalgebraModuleOld theModule;
  std::stringstream out;
//  theModule.InduceFromEmbedding(out, this->theHmm, theGlobalVariables);
  List<ElementSemisimpleLieAlgebra> theBasis;
  theBasis.SetSize(this->theHmm.theRange().GetNumGenerators());
  for (int i=0; i<this->theHmm.imagesAllChevalleyGenerators.size; i++)
  { int theIndex=i;
    if (i>=6 && i<8)
      theIndex=3+i;
    if (i>=8)
      theIndex=i+7;
    ElementSemisimpleLieAlgebra& currentElt=theBasis.TheObjects[theIndex];
    currentElt=this->theHmm.imagesAllChevalleyGenerators.TheObjects[i];
  }
  for (int i=0; i<theBasis.size; i++)
  { int displayIndex=i-9;
    if (displayIndex>=0)
    { if (displayIndex<3)
        displayIndex+=10;
      else
        displayIndex-=2;
    }
  }
  this->testAlgebra.init(theBasis, this->theHmm.theRange(), theGlobalVariables);
}

void Parser::initTestAlgebraNeedsToBeRewritteN(GlobalVariables& theGlobalVariables)
{ this->testAlgebra.initDefaultOrder(this->theHmm.theRange(), theGlobalVariables);
}

void RationalFunction::Substitution(PolynomialSubstitution<Rational>& theSub)
{ if (theSub.size<1)
    return;
//  FormatExpressions tempFormat;
  Rational rationalOne=1;
  switch(this->expressionType)
  { case RationalFunction::typeRational: return;
    case RationalFunction::typePoly:
//      std::cout <<"<hr>subbing in<br>" << this->ElementToString(tempFormat) << " using " << theSub.ElementToString()
//      << " to get ";
      this->Numerator.GetElement().Substitution(theSub, theSub[0].NumVars, rationalOne);
//      std::cout << "<br>finally:<br>" << this->Numerator.GetElement().ElementToString();
      this->Simplify();
//      std::cout << ", which, simplified, yields<br> " << this->ElementToString(tempFormat);
      return;
    case RationalFunction::typeRationalFunction:
      this->Numerator.GetElement().Substitution(theSub, theSub[0].NumVars, rationalOne);
      this->Denominator.GetElement().Substitution(theSub, theSub[0].NumVars, rationalOne);
      this->Simplify();
      return;
    default: assert(false); break;
  }
}

int ParserNode::EvaluateModVermaRelations
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ ParserNode& theArgument=theNode.owner->TheObjects[theArgumentList[0]];
  theNode.impliedNumVars=theArgument.impliedNumVars;
  theNode.UEElement.GetElement()=theArgument.UEElement.GetElement();
  std::stringstream out;
  out << "The element you wanted to be modded out, before simplification: "
  << CGI::GetHtmlMathDivFromLatexAddBeginARCL
  ( theNode.UEElement.GetElement().ElementToString(&theGlobalVariables.theDefaultLieFormat))
;
  Polynomial<Rational>  polyOne, polyZero;
  polyOne.MakeConst(theNode.impliedNumVars, 1);
  polyZero.MakeZero(theNode.impliedNumVars);
  theNode.UEElement.GetElement().Simplify(theGlobalVariables, polyOne, polyZero);
  out << "<br>And after simplification: "
  << CGI::GetHtmlMathDivFromLatexAddBeginARCL
  (theNode.UEElement.GetElement().ElementToString(&theGlobalVariables.theDefaultLieFormat))
  ;
  assert(false);

//  theNode.UEElement.GetElement().ModOutVermaRelations(theGlobalVariables);
  theNode.impliedNumVars=theNode.UEElement.GetElement().GetNumVars();
  theNode.ExpressionType=theNode.typeUEelement;
  theNode.outputString=out.str();
  return theNode.errorNoError;
}

template<class CoefficientType>
void ElementUniversalEnveloping<CoefficientType>::LieBracketOnTheLeft(const ElementSemisimpleLieAlgebra& left)
{ if (this->IsEqualToZero())
  { this->MakeZero(*this->owners, this->indexInOwners);
    return;
  }
  ElementUniversalEnveloping<CoefficientType> tempElt1, tempElt2;
  tempElt1.AssignElementLieAlgebra
  (left, *this->owners, this->indexInOwners, this->theCoeffs[0].GetOne(), this->theCoeffs[0].GetZero());
  tempElt2=*this;
  tempElt2.LieBracketOnTheRight(tempElt1, *this);
}

template<class CoefficientType>
bool MonomialUniversalEnveloping<CoefficientType>::AdjointRepresentationAction
  (const ElementUniversalEnveloping<CoefficientType>& input, ElementUniversalEnveloping<CoefficientType>& output,
   GlobalVariables& theGlobalVariables)
{ output.MakeZero(*this->owners, this->indexInOwners);
  ElementSemisimpleLieAlgebra tempElt;
  output=input;
  for (int i=this->generatorsIndices.size-1; i>=0; i--)
  { int nextCycleSize;
    if (!this->Powers[i].IsSmallInteger(nextCycleSize))
      return false;
    for (int j=0; j<nextCycleSize; j++)
    { tempElt.AssignChevalleyGeneratorCoeffOneIndexNegativeRootspacesFirstThenCartanThenPositivE
        (this->generatorsIndices[i], *this->owners, this->indexInOwners) ;
      output.LieBracketOnTheLeft(tempElt);
    }
  }
  return true;
}

template<class CoefficientType>
bool ElementUniversalEnveloping<CoefficientType>::AdjointRepresentationAction
  (const ElementUniversalEnveloping<CoefficientType>& input, ElementUniversalEnveloping<CoefficientType>& output,
   GlobalVariables& theGlobalVariables)
{ assert(&input!=&output);
  output.MakeZero(*this->owners, this->indexInOwners);
  ElementUniversalEnveloping<CoefficientType> summand;
  for (int i=0; i<this->size; i++)
  { if(!this->TheObjects[i].AdjointRepresentationAction(input, summand, theGlobalVariables))
      return false;
    summand*=this->theCoeffs[i];
    output+=summand;
  }
  return true;
}

int ParserNode::EvaluateAdjointAction
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ ParserNode& actingNode= theNode.owner->TheObjects[theArgumentList[0]];
  ParserNode& NodeBeingActedUpon=theNode.owner->TheObjects[theArgumentList[1]];
  theNode.impliedNumVars= MathRoutines::Maximum(actingNode.impliedNumVars, NodeBeingActedUpon.impliedNumVars);
  actingNode.UEElement.GetElement().SetNumVariables(theNode.impliedNumVars);
  NodeBeingActedUpon.UEElement.GetElement().SetNumVariables(theNode.impliedNumVars);
  ElementUniversalEnveloping<Polynomial<Rational> >& output=theNode.UEElement.GetElement();
  if ( !actingNode.UEElement.GetElement().AdjointRepresentationAction
  (NodeBeingActedUpon.UEElement.GetElement(), output, theGlobalVariables))
    return theNode.SetError(theNode.errorImplicitRequirementNotSatisfied);
  theNode.ExpressionType=theNode.typeUEelement;
  return theNode.errorNoError;
}

int ParserNode::EvaluateMakeCasimir
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ theNode.ExpressionType=theNode.typeUEelement;
  if (theGlobalVariables.MaxAllowedComputationTimeInSeconds<20)
    theGlobalVariables.MaxAllowedComputationTimeInSeconds=20;
  Polynomial<Rational>  polyOne, polyZero;
  polyOne.MakeConst(0,1);
  polyZero.MakeZero(0);
  theNode.UEElement.GetElement().MakeCasimir
  (theNode.GetContextLieAlgebra(), theGlobalVariables, polyOne, polyZero);
  std::stringstream out;
  out << "Context Lie algebra: " << theNode.GetContextLieAlgebra().GetLieAlgebraName
  (theNode.GetContextLieAlgebra().theWeyl.WeylLetter, theNode.GetContextLieAlgebra().GetRank());
  out << ". The coefficient: " << theNode.GetContextLieAlgebra().theWeyl.GetKillingDivTraceRatio().ElementToString()
  <<  ". The Casimir element of the ambient Lie algebra. Denoted also by c.";
  theNode.outputString=out.str();

  return theNode.errorNoError;
}

int ParserNode::EvaluateUnderscoreLeftArgumentIsArray(GlobalVariables& theGlobalVariables)
{ ParserNode& leftNode=this->owner->TheObjects[this->children[0]];
  ParserNode& rightNode=this->owner->TheObjects[this->children[1]];
  if (leftNode.Operation!=Parser::tokenG && leftNode.Operation!=Parser::tokenH)
    return this->SetError(this->errorBadIndex);
  Vector<Rational> theWeight;
  if (!rightNode.GetRootRationalDontUseForFunctionArguments(theWeight, theGlobalVariables))
    return this->SetError(this->errorBadIndex);
  ElementUniversalEnveloping<Polynomial<Rational> >& theUEElement=this->UEElement.GetElement();
  Polynomial<Rational>  polyOne, PolyZero;
  polyOne.MakeConst(0, (Rational) 1);
  PolyZero.MakeZero(0);
  if (leftNode.Operation==Parser::tokenG)
  { if (!this->GetContextLieAlgebra().theWeyl.IsARoot(theWeight))
      return this->SetError(this->errorDunnoHowToDoOperation);
    theUEElement.MakeOneGeneratorCoeffOne
    (theWeight, this->owner->theAlgebras, this->IndexContextLieAlgebra, polyOne, PolyZero);
    this->ExpressionType=this->typeUEelement;
    return this->errorNoError;
  }
  if (leftNode.Operation==Parser::tokenH)
  { if (theWeight.size!=this->GetContextLieAlgebra().GetRank())
      return this->SetError(this->errorDimensionProblem);
    Polynomial<Rational>  PolyZero;
    theUEElement.AssignElementCartan
    (theWeight, this->owner->theAlgebras, this->IndexContextLieAlgebra, polyOne, PolyZero);
    this->ExpressionType=this->typeUEelement;
    return this->errorNoError;
  }
  return this->SetError(this->errorProgramming);
}

int ParserNode::EvaluateGetCoxeterBasis
  (ParserNode& theNode, List<int>& theArgumentList, GlobalVariables& theGlobalVariables)
{ WeylGroup theWeyl;
  int theDimension=theNode.owner->DefaultWeylRank;
  std::stringstream out;
  theNode.ExpressionType=theNode.typeString;
  if (theDimension<2)
  { out << "Rank of Lie algebra too small -> no coxeter plane. ";
    theNode.outputString=out.str();
    return theNode.errorNoError;
  }
  theWeyl.MakeArbitrary(theNode.owner->DefaultWeylLetter, theDimension);
  Vector<double> basis1, basis2;
  theWeyl.GetCoxeterPlane(basis1, basis2, theGlobalVariables);
  out.precision(5);
  out << "Lie algebra of type " << SemisimpleLieAlgebra::GetLieAlgebraName(theWeyl.WeylLetter, theDimension, true)
  << "<br> One orthonormal basis of coxeter plane:<br>" << basis1.ElementToString() << ", "
  << basis2.ElementToString();
  theNode.outputString=out.str();
  return theNode.errorNoError;
}

void Parser::initFunctionList(char defaultExampleWeylLetter, int defaultExampleWeylRank)
{ if (this->flagFunctionListInitialized)
    return;
  this->flagFunctionListInitialized=true;
  ParserFunction theFunction;
  this->theFunctionList.Reserve(1000);
  this->AddOneFunctionToDictionaryNoFail
  ("coneFromNormals",
   "((Rational,...),...)",
   "A polyhedron in n dimensions such that all of its walls pass through zero. \
   The argument vectors describe the normals of the cone walls. \
   To each such polyhedron C we assign the affine polyhedron obtained by intersecting C with the hyperplane passing through \
   (0,...,0,1) and parallel to the hyperplane spanned by the vectors with last coordinate zero.",
   "coneFromNormals((1,0),(0,1)) ",
    & ParserNode::EvaluateCone
   );
  this->AddOneFunctionToDictionaryNoFail
  ("coneFromVertices",
   "((Rational,...),...)",
   "Create the cone spanned over the non-negative rationals by the input vectors. You can use the alias name cone, \
   for example cone((1,0,0),(0,1,0), (0,0,1),(0,1,1), (1,1,0),(1,1,1)).",
   "coneFromVertices((1,0,0),(0,1,0), (0,0,1),(0,1,1), (1,1,0),(1,1,1)) ",
    & ParserNode::EvaluateConeFromVertices
   );
  this->AddOneFunctionToDictionaryNoFail
  ("cone",
   "((Rational,...),...)",
   "Alias for coneFromVertices.",
   "cone((1,0,0),(0,1,0), (0,0,1),(0,1,1), (1,1,0),(1,1,1)) ",
    DefaultWeylLetter, DefaultWeylRank, false,
    & ParserNode::EvaluateConeFromVertices
   );
  this->AddOneFunctionToDictionaryNoFail
  ("slTwoInSlN",
   "(Integer,...)",
   "Prints out sl(2) embedded in sl(N) according to the partition described by the arguments.",
   "slTwoInSlN(2,3)",
    & ParserNode::EvaluateSlTwoInSlN
   );
  this->AddOneFunctionToDictionaryNoFail
  ("getWeylDim",
   "(Rational,...)",
   "Evaluates the Weyl dimension formula. Input: a vector in fundamental weight coordinates. \
   The j^th fundamental weight is the weight that has scalar product 1 with the j^th simple Vector<Rational> times 2 \
   divided by the Vector<Rational> length squared, and 0 with the remaining simple Vectors<Rational>.",
   "getWeylDim(0,0,1)",
    & ParserNode::EvaluateWeylDimFormula
   );
  this->AddOneFunctionToDictionaryNoFail
  ("printRootSystem",
   "()",
   "Print the Vector<Rational> system of the ambient Lie algebra.",
   "printRootSystem",
   DefaultWeylLetter, DefaultWeylRank,
    & ParserNode::EvaluatePrintRootSystem
   );
  this->AddOneFunctionToDictionaryNoFail
  ("actByWeyl",
   "(RF,...)",
   "Act by the Weyl group on a weight vector whose coordinates are given in simple basis coordinates. \
   For example, for the Weyl group of A_2 (sl(3)),  the vector (1, -x_1) corresponds to a-x_1b, where a and b are the\
    first and second simple Vectors<Rational>, and x_1 is a variable. The coordinates can be arbitrary polynomials \
    (rational functions work too but *very* slow). ",
   "actByWeyl(x_1, x_2, x_3)",
    & ParserNode::EvaluateWeylAction
   );
  this->AddOneFunctionToDictionaryNoFail
  ("actByWeylRho",
   "(RF,...)",
   "Act using the \\rho-modified action on a weight vector whose coordinates are given in simple basis coordinates. \
   For an element of the Weyl group w, the \\rho-modified action of w on \\alpha is given by w(\\alpha+\\rho)-\\rho, \
   where \\rho is the half-sum of the positive Vectors<Rational> of the ambient Vector<Rational> system. \
   This function is the affine \\rho-modification of the function ActByWeyl. The following example calls the action of the Weyl group\
    of so(7); the half-sum of the positive Vectors<Rational> of so(7) in simple coordinates equals (5/2,8/2,9/2).",
   "actByWeylRho(x_1-5/2, x_2-8/2,x_3-9/2)",
    & ParserNode::EvaluateWeylRhoAction
   );
  this->AddOneFunctionToDictionaryNoFail
  ("partialFraction",
   "((Rational,...),...)",
   "Gives the partial fraction corresponding to the vector partition function of the arguments.<br> \
   The arguments must be non-zero vectors with non-negative integral coordinates.",
   "partialFraction((1,0), (0,1), (1,1))",
      DefaultWeylLetter, DefaultWeylRank, false ,
    & ParserNode::EvaluatePartialFractions
   );
  this->AddOneFunctionToDictionaryNoFail
  ("split",
   "(PF)",
   "Splits a partial fraction according to this <a href=\"http://arxiv.org/abs/0910.4675\"> text </a>, or a modification/improvement of it.",
   "split(partialFraction((1,0), (0,1), (1,1)))",
      DefaultWeylLetter, DefaultWeylRank, false ,
    & ParserNode::EvaluateSplit
   );
  this->AddOneFunctionToDictionaryNoFail
  ("vpfReport",
   "((Rational,...),...)",
   "Computes the vector partition function with respect to the input vectors, according to this \
   <a href=\"http://arxiv.org/abs/0910.4675\"> text </a>. The example below is the Kostant partition function \
   of Vector<Rational> system of type A3 (sl(4)).",
   "vpfReport((1,0,0), (0,1,0), (0,0,1), (1,1,0), (0,1,1), (1,1,1))",
      DefaultWeylLetter, DefaultWeylRank, false ,
    & ParserNode::EvaluateVectorPFIndicator
   );
  this->AddOneFunctionToDictionaryNoFail
  ("vpf",
   "((Rational,...),...)",
   "Computes the vector partition function with respect to the input vectors, according to this \
   <a href=\"http://arxiv.org/abs/0910.4675\"> text </a>. The example below is the Kostant partition function \
   of Vector<Rational> system of type B2 (so(5)).",
   "vpf((-1,0), (0,-1), (-1,-1), (-1,-2))",
      DefaultWeylLetter, DefaultWeylRank, false ,
    & ParserNode::EvaluateVPF
   );
   this->AddOneFunctionToDictionaryNoFail
  ("transformToReducedGroebnerBasis",
   "(Polynomial,...)",
   "<b> This function is largely untested. If you use it make sure to double-check the output. \
   </b> Transforms to reduced Groebner basis using Buchberger's algorithm with respect to the lexicographic monomial \
   ordering x_1^l&lt; x_2^m&lt;x_3^n&lt;....",
   "transformToReducedGroebnerBasis(x_1^3+x_1x_2+1, x_1x_2, x_2^3)",
    & ParserNode::EvaluateGroebner
   );
   this->AddOneFunctionToDictionaryNoFail
  ("getRelations",
   "(Polynomial,...)",
   "<b> This function is largely untested. If you use it make sure to double-check the output. </b>\
   Get the algebraic relations between the input polynomials.",
   "getRelations(x_1^2, x_1x_2, x_2^2)",
    & ParserNode::EvaluateRelations
   );
  this->AddOneFunctionToDictionaryNoFail
  ("printRootSubalgebras",
   "()",
   "Computes all Vector<Rational> subalgebras of the ambient Lie algebra. The computation is served from \
   the hard disk if it is already computed. \
   The function will redirect you to a new page, to return to the calculator use the back button.",
   "printRootSubalgebras",
   DefaultWeylLetter, DefaultWeylRank,
    & ParserNode::EvaluatePrintRootSAs
   );
  this->AddOneFunctionToDictionaryNoFail
  ("printSlTwos",
   "()",
   "Computes all sl(2) subalgebras (equivalently, all nilpotent orbits) of the ambient Lie algebra over the complex numbers. \
   The computation is served from the hard disk if it is already computed. \
   The function will redirect you to a new page, to return to the calculator use the back button.",
   "printSlTwos",
   DefaultWeylLetter, DefaultWeylRank,
    & ParserNode::EvaluatePrintSlTwos
   );
  this->AddOneFunctionToDictionaryNoFail
  ("zprintSlTwosAndRootSAsFORCERecompute",
   "()",
   "<b>Use only if there are broken links in Vector<Rational> subalgebra/sl(2)-database. \
   When executing this command, please be patient and do not click any links until automatically redirected. </b> \
   I have a problem with setting up the Apache server and the way it handles SIGABORT signals \
   (this is what happens when you don't wait for the computation to finish and click on a link prematurely). \
   This function is a temporary solution to this (minor) technical problem. \
   The function name starts with z because it is meant to be used rarely.",
   "zprintSlTwosAndRootSAsFORCERecompute",
    DefaultWeylLetter, DefaultWeylRank, false,
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
   "Given an sl(2)-representation  V_1+...+V_n , computes the symmetric tensor algebra invariants of sl(2). \
   Input: the first variable is the degree, followed by a vector describing the dimensions of  each V_i.",
   "invariantsSlTwoOfDegree(2,(2,2))",
    & ParserNode::EvaluateInvariantsSl2DegreeM
   );
/*  this->AddOneFunctionToDictionaryNoFail
  ("findExtremaInDirectionOverLatticeShiftedOneNonParam",
   "(Polynomial, (Rational,...), Lattice, Cone)",
   "<b> Experimental please don't use.</b>",
   "findExtremaInDirectionOverLatticeShiftedOneNonParam(-2x_1+x_2, (0,0), lattice((1,0),(0,1)), cone((-1,1,0),(1,0,0), (-2,-1,4)))",
    & ParserNode::EvaluateFindExtremaInDirectionOverLatticeOneNonParam
   );*/
  this->AddOneFunctionToDictionaryNoFail
  ("findMaximumInDirectionOverLatticeShifted",
   "(Polynomial, (Rational,...), Lattice, Cone, Integer)",
   "<b> Experimental please don't use.</b>",
   "findMaximumInDirectionOverLatticeShifted(-2x_1+x_2+x_3, (0,0,0), lattice((1,0,0),(0,1,0),(0,0,1)), cone((1,0,0,0),(0,1,0,0), (1,1,1,-4), (0,1,-1,0), (0,0,0,1)),2)",
    DefaultWeylLetter, DefaultWeylRank, false,
    & ParserNode::EvaluateFindExtremaInDirectionOverLattice
   );
  this->AddOneFunctionToDictionaryNoFail
  ("lattice",
   "((Rational,...),...)",
   "Rational lattice generated by the argument vectors.",
   "lattice((1,3), (2,2))",
      DefaultWeylLetter, DefaultWeylRank, false,

    & ParserNode::EvaluateLattice
   );
  this->AddOneFunctionToDictionaryNoFail
  ("getPointOnShiftedLatticeClosestToWallInDirection",
   "((Rational,...), (Rational,...), (Rational,...), Lattice)",
   "<b>Experimental, please don't use</b>.",
   "getPointOnShiftedLatticeClosestToWallInDirection( (2,3), (1/2,1/2), (1,1/5,1/3), lattice((1,1), (0,2)))",
   DefaultWeylLetter, DefaultWeylRank, false,
    & ParserNode::EvaluateClosestPointToHyperplaneAlongTheNormal
   );
  this->AddOneFunctionToDictionaryNoFail
  ("intersectLatticeWithPreimageOfLattice",
   "(Lattice, Lattice, ((Rational,...),...))",
   "Intersects the first lattice with the preimage of the second lattice under the linear map described by the third argument. \
   Suppose the dimension of vector space of the first lattice is m, and the dimension of the vector space of the second lattice is n. \
   Then the linear map should be given by a list of m vectors, with n coordinates each. \
   The i^th of these vectors should give the image of the i^th basis vector of the vector space of the first lattice. ",
   "intersectLatticeWithPreimageOfLattice(  lattice((1,1), (0,2)), lattice(26/5), ( (1), (1/5)) )",
   DefaultWeylLetter, DefaultWeylRank, false,
    & ParserNode::EvaluateIntersectLatticeWithPreimageLattice
   );
  this->AddOneFunctionToDictionaryNoFail
  ("sliceConeUniqueExitWall",
   "(Cone, (Rational, ...))",
   "Slices the projective cone into smaller projective cones such that in each piece, for all point inside that piece, \
   tracing a ray in the direction opposite to the one given by the second argument will \
   exit the cone from a unique wall (i.e. the exit wall does not depend on the starting point). \
   This slice can be named the \"salami\"-slice, as a piece of salami can be cut at an arbitrary angle, \
   however the ends of the salami should always consist of a single plane.",
   "sliceConeUniqueExitWall( coneFromNormals((1,0,0),(0,1,0),(0,0,1)), (1,1,1) )",
    & ParserNode::EvaluateSliceCone
   );
  this->AddOneFunctionToDictionaryNoFail
  ("sliceDirections",
   "((Rational, ...),...)",
   "Exit wall slice. Slices the cone spanned by the input vectors so as to satisfy the salami slice condition inductively with respect to each new added vector. ",
   "sliceDirections( (1,0,0),(0,1,0),(0,0,1),(1,1,0), (0,1,1),(1,1,1) )",
    DefaultWeylLetter, DefaultWeylRank, true,
    & ParserNode::EvaluateCreateFromDirectionsAndSalamiSlice
   );
   this->AddOneFunctionToDictionaryNoFail
  ("drawConeProjective",
   "(Cone)",
   "On condition that the cone has a vertex at 0, draws a picture corresponding to the polyhedron with walls passing through 0 represented by the argument. ",
   "drawConeProjective( coneFromNormals((1,0,0),(0,1,0),(0,0,1)))",
    & ParserNode::EvaluateDrawConeProjective
   );
   this->AddOneFunctionToDictionaryNoFail
  ("drawConeAffine",
   "(Cone)",
   "On condition that the cone lies in the non-strict upper half-plane and has a vertex at 0, \
   draws the intersection of the cone with the affine hyperplane passing through (0,...,0,1) and parallel \
    to the hyperplane spanned by the vectors with last coordinate 0. ",
   "drawConeAffine( coneFromNormals((1,0,0,0),(0,1,0,0),(0,0,1,0),(-1,0,0,1), (0,-1,0,1), (0,0,-1,1)))",
    & ParserNode::EvaluateDrawConeAffine
   );
  this->AddOneFunctionToDictionaryNoFail
  ("runGtwoInBthree",
   "(Integer, Integer, Integer)",
   "Run the G_2 in B_3 computation. Experimental, don't use.",
   "runGtwoInBthree(1,0,0)",
   DefaultWeylLetter, DefaultWeylRank, false,
    & ParserNode::EvaluateG2InB3Computation
   );
  this->AddOneFunctionToDictionaryNoFail
  ("drawRootSystemOld",
   "(Integer, Integer)",
   "Draw the Vector<Rational> system in its Coxeter plane. First argument = weyl letter (A=0, B=1,..., G=7) . Second argument=weyl rank, which must be at most 8. The example draws E6.",
   "drawRootSystemOld(4,6)",
   DefaultWeylLetter, DefaultWeylRank, false,
    & ParserNode::EvaluateDrawRootSystemOld
   );
  this->AddOneFunctionToDictionaryNoFail
  ("drawRootSystem",
   "()",
   "Draw the Vector<Rational> system of the ambient Lie algebra in its Coxeter plane. ",
   "drawRootSystem",
   DefaultWeylLetter, DefaultWeylRank, true,
    & ParserNode::EvaluateDrawRootSystem
   );
  this->AddOneFunctionToDictionaryNoFail
  ("drawRootSystemInCoxeterPlaneOfRootSA",
   "(Integer)",
   "<b>Experimental</b>. ",
   "drawRootSystemInCoxeterPlaneOfRootSA(0)",
   DefaultWeylLetter, DefaultWeylRank, false,
    & ParserNode::EvaluateDrawRootSystemCoxeterPlaneRootSA
   );
  this->AddOneFunctionToDictionaryNoFail
  ("animateRootSystem",
   "(Integer, (Rational,...),...)",
   "Animate the Vector<Rational> system. First argument N>=3 = number of frames. The remaining arguments \
   must describe an even number>=4 of elements of h* with rational coordinates, written in simple basis coordinates. \
   The consecutive pairs of such vectors parametrize the projection plane waypoints. More precisely, \
   each pair of input vectors must give an arbitrary basis of a ``waypoint'' projection plane.  \
   The animation will start at the first projection plane, fly through to the\
   second projection plane, then away to the third, and so on. When at the last frame, the animation will \
   start backwards. The example rotates the Vector<Rational> system of B3(so(7)) from a Vector<Rational> subsystem of type B2(so(5)) to a \
   plane the projection to which induces the embedding of exceptional Lie algebra G2 in B3. This \
   plane also coincides with one of the (finitely many) choices of a Coxeter plane of B3(so(7)). ",
   "animateRootSystem(100, (0,1,0),(0,0,1), (0,1,0),(1,0,2) )",
   'B', 3, false,
    & ParserNode::EvaluateAnimateRootSystem
   );
  this->AddOneFunctionToDictionaryNoFail
  ("animatePause",
   "(Integer)",
   "<b>Experimental.</b> Animate the Vector<Rational> system. First argument = weyl letter 0=A. Second argument=weyl rank. This argument= number of frames.",
   "animatePause(100)",
   DefaultWeylLetter, DefaultWeylRank, false,
    & ParserNode::EvaluateAnimationPause
   );
  this->AddOneFunctionToDictionaryNoFail
  ("animateClearScreen",
   "(Integer, Integer)",
   "<b>Experimental.</b> Animate the Vector<Rational> system. First argument = weyl letter 0=A. Second argument=weyl rank. This argument= number of frames.",
   "animateClearScreen(0,2)",
   DefaultWeylLetter, DefaultWeylRank, false,
    & ParserNode::EvaluateAnimationClearScreen
   );
  this->AddOneFunctionToDictionaryNoFail
  ("animateRootSystemBlueDot",
   "(Integer, Integer, Integer, (Rational,...))",
   "<b>Experimental.</b> Animate the Vector<Rational> system. First argument = weyl letter 0=A. Second argument=weyl rank. This argument= number of frames.",
   "animateRootSystemBlueDot(0,2, 16777216, ((1,0),(0,1)))",
   DefaultWeylLetter, DefaultWeylRank, false,
    & ParserNode::EvaluateAnimateRootSystemBluePoint
   );
  this->AddOneFunctionToDictionaryNoFail
  ("latticeImprecise",
   "((Rational,...),...)",
   "<b>Experimental.</b> Draws G2 in B3.",
   "latticeImprecise((1, 0.000000001), (1.00000000001, 1.00000000001), (1/2, 1/2))",
   DefaultWeylLetter, DefaultWeylRank, false,
    & ParserNode::EvaluateLatticeImprecise
   );
  this->AddOneFunctionToDictionaryNoFail
  ("drawWeightSupport",
   "(Rational, ... )",
   "Draws the weight support of a highest weight module of highest weight given in \
   fundamendal weight basis. All arguments must be non-negative integers. \
   The maximum number of drawn weights allowed is (hard-code) limited to 10 000. If \
   the number of weights in the weight support exceeds this number, only the first 10 000 weights will be drawn. ",
   "drawWeightSupport(1,1,1)",
   'B', 3, false,
    & ParserNode::EvaluateDrawWeightSupport
   );
  this->AddOneFunctionToDictionaryNoFail
  ("drawWeightSupportWithMults",
   "(Rational, ... )",
   "Same as drawWeightSupport but also displays weight multiplicities. <b> Warning. Drawing text (i.e. multiplicities) \
   is very javascrtipt-processor-intensive. Use only for *small* examples, else you might hang your browser. </b> ",
   "drawWeightSupportWithMults(1,1)",
   'G', 2, true,
    & ParserNode::EvaluateDrawWeightSupportWithMults
   );
  this->AddOneFunctionToDictionaryNoFail
  ("parabolicsInfo",
   "()",
   "Makes a table with information about the parabolic subalgebras of the ambient Lie algebra.",
   "parabolicsInfo",
   DefaultWeylLetter, DefaultWeylRank, false,
    & ParserNode::EvaluateParabolicWeylGroups
   );
  this->AddOneFunctionToDictionaryNoFail
  ("parabolicsInfoBruhatGraph",
   "(Integer,...)",
   "<b>Please do not use for subalgebras larger than B_4 (so(9)). The vpf program has no problem handling this \
   function up to E_6 but LaTeX crashes trying to process the output. </b> \
   Makes a table with information about the Weyl group of a parabolic subalgebra of the ambient Lie algebra, \
   as well as the cosets \
   (given by minimal coset representatives) of the Weyl subgroup in question. \
   The input must have as many integers as there are simple Vectors<Rational> in the ambient \
   Lie algebra. If the Vector<Rational> is crossed out (i.e. not a Vector<Rational> space of the Levi part), one should put a 1 in the corresponding \
   coordinate. \
   Otherwise, one should put 0. For example, for Lie algebra B3(so(7)), \
   calling parabolicsInfoBruhatGraph(0,0,0) gives you the Weyl group info for the entire algebra; \
   calling parabolicsInfoBruhatGraph(1,0,0) gives you info for the Weyl subgroup generated by the last two simple Vectors<Rational>. \
   In the produced graph, the element s_{\\eta_i} corresponds to a reflection with respect to the i^th simple Vector<Rational>. \
   You will get your output as a .png file link, you must click onto the link to see the end result. ",
   "parabolicsInfoBruhatGraph(1,0,0)",
//   DefaultWeylLetter, DefaultWeylRank, true,
    & ParserNode::EvaluateParabolicWeylGroupsBruhatGraph
   );
  this->AddOneFunctionToDictionaryNoFail
  ("gTwoInBthreeMultsParabolic",
   "((Rational,...), (Rational,...))",
   "<b>Experimental, please don't use.</b> Computes the multiplicities of all G2 generalized Verma modules in the generalized Verma module of so(7) with so(7)-highest weight given by the first argument. \
   The second argument describes the parabolic subalgebra of so(7) (its intersection with G2 determines the parabolic subalgebra in G2). ",
   "gTwoInBthreeMultsParabolic((2,0,0), (1,0,0) )",
   DefaultWeylLetter, DefaultWeylRank, false,
    & ParserNode::EvaluateG2InB3MultsParabolic
   );
  this->AddOneFunctionToDictionaryNoFail
  ("modOutVermaRelations",
   "(UE)",
   "<b>Experimental, not tested completely.</b> Mods out the Verma relations. Let the the simple basis be alpha_1,...,alpha_n,\
    and its dual basis be beta_1,..., beta_n, i.e. the scalar product of alpha_i  and beta_j is 1 if i=j and 0 otherwise. Then the highest \
    weight is assumed to be x_1 beta_1 +...+x_n beta_n, where x_1,... x_n are variables. Assume a monomial is written so that all\
    positive Vector<Rational> generators are on the right side, all elements in the Cartan in the middle, and the negative Vector<Rational> generators on the left (i.e\
    the default calculator format). Then the Verma relations send the positive generators to zero; they send the element h_\\alpha_i to the \
    variable x_i, and finally they leave the negative generators untouched. The example mods out the Verma relations from the Casimir element, c. \
    The result is the general form of a character of Verma module.",
   "modOutVermaRelations(c)",
   DefaultWeylLetter, DefaultWeylRank, true,
    & ParserNode::EvaluateModVermaRelations
   );
  this->AddOneFunctionToDictionaryNoFail
  ("casimir",
   "()",
   "Gives the Casimir element of the ambient Lie algebra. You can also use the one-letter alias c.",
   "c",
   DefaultWeylLetter, DefaultWeylRank, true,
    & ParserNode::EvaluateMakeCasimir
   );

  this->AddOneFunctionToDictionaryNoFail
  ("actByWeylAlgebraElement",
   "(WeylAlgebraElement, Polynomial)",
   "<b>Not fully tested.</b> Act by a weyl algebra element (the first argument)\
    on the a polynomial (the second argument).",
   "actByWeylAlgebraElement(x_1d_1+x_2d_2, x_1x_2)",
//   DefaultWeylLetter, DefaultWeylRank, true,
    & ParserNode::EvaluateActByWeylAlgebraElement
   );
  this->AddOneFunctionToDictionaryNoFail
  ("weightSupportGtwoGenVermaModule",
   "((Rational,...), (Rational,...))",
   "<b>Experimental please don't use.</b> Draw the weight support of a generalized G_2-module.\
   The first argument gives the parabolic of G_2; the second argument gives the highest weight of G_2 in fundamental \
   basis coordinates.",
   "weightSupportGtwoGenVermaModule((0,1),(0,2))",
   DefaultWeylLetter, DefaultWeylRank, false,
    & ParserNode::EvaluateG2ParabolicSupport
   );
  this->AddOneFunctionToDictionaryNoFail
  ("getCoxeterPlaneBasis",
   "()",
   " Returns a basis of the Coxeter plane of the Weyl group of the ambient Lie algebra (if it exists). The output is in string format, for the time being. ",
   "getCoxeterPlaneBasis",
   DefaultWeylLetter, DefaultWeylRank, true,
    & ParserNode::EvaluateGetCoxeterBasis
   );
   this->AddOneFunctionToDictionaryNoFail
  ("animateFlyThroughRootSAs",
   "()",
   "<b> Experimental. This function shouldn't be visible, if it is, then somebody (*who can it be*) messed up.</b>Experiment: trying to make sense of coxeter planes for reducible Vector<Rational> systems.",
   "animateFlyThroughRootSAs",
   DefaultWeylLetter, DefaultWeylRank, false,
    & ParserNode::EvaluateAnimateRootSAs
   );
   this->AddOneFunctionToDictionaryNoFail
  ("drawRootSystemIncludeWeylChamberAndDynkinLabels",
   "()",
   "Same as drawRootSystem but draws in addition the weyl chamber and labels the Dynkin diagram vertices.",
   "drawRootSystemIncludeWeylChamberAndDynkinLabels",
   DefaultWeylLetter, DefaultWeylRank, true,
    & ParserNode::EvaluateDrawRootSystemLabelDynkin
   );
   this->AddOneFunctionToDictionaryNoFail
  ("drawRootSystemFixProjection",
   "((Rational,...),(Rational,...))",
   "Same as drawRootSystem but draws the Vector<Rational> system in a user-defined projection plane.The first and the secondargument\
   must be two vectors \
   forming a basis of the projection plane. The vectors must be given in simple basis coordinates. \
   The example projects the Vector<Rational> system of sl(8) onto the\
   plane spanned by the Vector<Rational> of the sl(2)-subalgebra given by 4+4 partition \
   and the weight corresponding to the element of the Cartan of sl(8) that commutes with the sl(2).",
   "drawRootSystemFixProjection( (3,4,3,0,3,4,3), (1,2,3,4,3,2,1))",
   'A', 7, true,
    & ParserNode::EvaluateDrawRootSystemFixedProjectionPlane
   );
   this->AddOneFunctionToDictionaryNoFail
  ("char",
   "(Rational,...)",
   "<b>Not fully tested, however several tests against LiE work just fine. </b> Creates a character corresponding to an \
    irreducible highest weight module with respect to the ambient simple Lie algebra. \
    The argument of the function is the highest weight \
    given in fundamental coordinates. Some arithmetic operations between characters are allowed. \
    Multiplication of characters (i.e. tensor decomposition) is implemented via the Brauer-Klimyk formula. \
    The basic algorithms used are the same as in LiE, however \
    the current implementation is more than 10 times slower (for technical reasons), \
    so for heavy computations you might want to use LiE instead. ",
    "(char(0,0,1)*char(1,0,0)-char(0,1,0))*char(0,0,1)",
   'B', 3, false,
    & ParserNode::EvaluateChar
   );
   this->AddOneFunctionToDictionaryNoFail
  ("freudenthal",
   "(Char)",
   "<b>Experimental. </b> Freudenthal formula that gives the multiplicities of the dominant weights of the \
    representation. The implementation works considerably slower than LiE(>10 times - the reason is technical; \
     the algorithm used is approximately the same), so consider using LiE if you need to do a larger example. ",
    "freudenthal(char(2,2,2))",
   'B', 3, false,
    & ParserNode::EvaluateFreudenthal
   );
   this->AddOneFunctionToDictionaryNoFail
  ("v",
   "((Polynomial,...),(Rational,...))",
   "<b>Experimental. </b> Highest weight vector of a generalized Verma module.\
   The first argument gives the highest weight of the module; the second argument gives the parabolic subalgebra. \
   In the second argument, a zero stands for a Vector<Rational> space that is a Vector<Rational> of the Levi part of the parabolic subalgebra. \
   A non-zero element indicates that the corresponding Vector<Rational> space that is not in the Levi part of \
   the parabolic subalgebra.",
    "v((1,0,1), (1,0,0))",
   'B', 3, true,
    & ParserNode::EvaluateHWV
   );
   this->AddOneFunctionToDictionaryNoFail
  ("littelmann",
   "(Rational,...)",
   "<b>For testing purposes. Will remain in future versions only as a hidden function.</b> Returns the Littelmann path obtained by connecting the \
   zero weight to the argument weight given in fundamental coordinates.",
    "littelmann(1,0,0)",
   'B', 3, false,
    & ParserNode::EvaluateLittelmannPaths
   );
  this->AddOneFunctionToDictionaryNoFail
  ("eAlpha",
   "(Integer, LittelmannPath)",
   "<b>For testing purposes. Will remain in future versions only as a hidden function. </b> \
   The Littelmann e_\\alpha and f_\\alpha operators. \
   The integer index gives the index of the simple Vector<Rational>. Positive indices correspond to the Littelmann e_\\alpha operators, \
   Negative indices correspond to the f_\\alpha operators.",
    "eAlpha(-1, eAlpha(-2, littelmann(1,0,0))",
   'B', 3, false,
    & ParserNode::EvaluateLittelmannEAlpha
   );
  this->AddOneFunctionToDictionaryNoFail
  ("allLittelmannPaths",
   "(Rational,...)",
   "<b>Experimental. Has passed a few tests. </b> Gives all Littelmann paths starting from the path that \
   connects with a straight line the zero weight with the weight given by the argument in fundamental \
   coordinates. The number of paths is capped at 1000; if you request a computation whose answer \
   has more than 1000 paths, you will be informed with an appropriate error message. \
   Reference: P. Littelmann, Paths and Vector<Rational> operators in representation theory. Let e_{\\alpha_i}, f_{\\alpha_i} \
   be the Vector<Rational> operators as defined in the cited article, where \\alpha_i denotes the i^th simple \
   Vector<Rational>. In the printouts, for i>=1, the expression g_{-i} denotes the Vector<Rational> operator f_{\\alpha_i}, \
   and the expression g_{i} denotes the Vector<Rational> operator e_{\\alpha_i}. The notation g_{i} is a valid calculator input \
   and is also used to denote the i^th Chevalley-Weyl generator (i<0 allowed). \
    ",
    "allLittelmannPaths(0,1)",
   'G', 2, true,
    & ParserNode::EvaluateAllLittelmannPaths
   );
  this->AddOneFunctionToDictionaryNoFail
  ("littelmannPathsFromWaypoints",
   "((Rational,...),...)",
   "<b>For testing purposes. Will remain in future versions only as a hidden function. \
   </b> Gives a Littelmann path from waypoints given in simple \
   coordinates.",
    "littelmannPathsFromWaypoints((0,0,0),(1,2,2))",
   'B', 3, true,
    & ParserNode::EvaluateLittelmannPathFromWayPoints
   );
  this->AddOneFunctionToDictionaryNoFail
  ("irreducibleRep",
   "((Rational,...),...)",
   "<b>Experimental, not tested. \
   </b> Creates an irreducible finite dimensional representation from highest weight given in fundamental \
   coordinates.",
    "irreducibleRep(1,1)",
   'A', 2, true,
    & ParserNode::EvaluateRepresentationFromHWFundCoords
   );
  this->AddOneFunctionToDictionaryNoFail
  ("mta",
   "(UE)",
   "<b>Experimental, might be hidden in future versions. \
   </b> MTA= minus transpose automorphism. Stands for the automorphism of g \
   defined by g_{\\alpha} -> -g_{-\\alpha}. In the case of gl(n), the matrix minus transpose automorphism of gl(n)\
   coincides with the so defined mta() map; for the other classical types, the mta() map coincides with the minus transpose\
   automorphism for the matrix realizations by the Vinberg-Onishchik book (Seminar...).",
    "mta(g_1)",
   'A', 2, true,
    & ParserNode::EvaluateMinusTransposeAuto
   );
  this->AddOneFunctionToDictionaryNoFail
  ("hwMTAbf",
   "(UE, UE, (Rational,...))",
   "<b>Experimental, might be hidden or changed in future versions. \
   </b> Highest weight bilinear form. Let M be a Verma module with highest weight vector v, and let P:M->M\
   be a projection map onto Cv that maps every weight vector of M of weight different from the \
   highest to 0. Let u_1, u_2 be two words in the universal enveloping algebra. Then define hwMTAbf(u_1,u_2):=\
   Tr_M (P ( mta(u_2) u_1 +mta(u_1)u_2 ) ), where mta() is the minus transpose automorphism of g.",
    "hwMTAbf(g_{-1} g_{-2}, g_{-1}g_{-2}, (2,2))",
   'G', 2, false,
    & ParserNode::EvaluateHWMTABilinearForm
   );
  this->AddOneFunctionToDictionaryNoFail
  ("hwTAAbf",
   "(UE, UE, (Polynomial,...))",
   "<b>Experimental, might be hidden or changed in future versions. \
   </b> Highest weight bilinear form. Let M be a Verma module with highest weight vector v given in \
   fundamental coordinates. Let P:M->M\
   be a projection map onto Cv that maps every weight vector of M of weight different from the \
   highest to 0. Let u_1, u_2 be two words in the universal enveloping algebra. Then define hwTAAbf(u_1,u_2):=\
   Tr_M (P ( taa(u_1) u_2 ), where taa() is the transpose anti-automorphism of g.",
    "hwTAAbf(g_{-1} g_{-2}, g_{-1}g_{-2}, (2,2))",
   'G', 2, false,
    & ParserNode::EvaluateHWTAABilinearForm
   );
  this->AddOneFunctionToDictionaryNoFail
  ("taa",
   "(UE)",
   "<b>Experimental, might be hidden or changed in future versions. \
   </b>Transpose anti-automorphism of the universal enveloping. This map sends g_{\\alpha} to g_{-\\alpha} \
   and reverses the order of multiplication. ",
    "taa(g_{-1} g_{-2})",
   'G', 2, true,
    & ParserNode::EvaluateTransposeAntiAuto
   );
  this->AddOneFunctionToDictionaryNoFail
  ("isInproperSubmodule",
   "(UE, (Rational, ...))",
   "<b>Experimental, might be hidden or changed in future versions. \
   </b>Generates and prints out the ad(g_{\\alpha_i})-\"orbit\" of the first argument in \
   the Verma module of highest weight given in fundamental coordinates by the second argument, where \\alpha_i\
   run over all simple positive Vectors<Rational> of the ambient Lie algebra.",
    "isInproperSubmodule\
    (11/5 g_{-1}^{2}g_{-2}g_{-1}g_{-2}g_{-1}^{2}g_{-2}^{2}g_{-1}^{2} \
      - g_{-1}^{3}g_{-2}^{2}g_{-1}^{2}g_{-2}g_{-1}g_{-2}g_{-1} , (2,2))",
   'C', 2, true,
    & ParserNode::EvaluateIsInProperSubmoduleVermaModule
   );
  this->AddOneFunctionToDictionaryNoFail
  ("splitIrrepOverLeviParabolic",
   "( (Rational, ...), (Rational, ...))",
   "<b>Might be hidden or changed in future versions. \
   </b>Splits an irreducible finite dimensional representation with highest weight \
   given by the first argument over the Levi part of the parabolic subalgebra given by the \
   second argument. The splitting both splits the characters and finds the singular vectors \
   that realize this splitting",
    "splitIrrepOverLeviParabolic((1,1),(0,1))",
   'B', 2, true,
    & ParserNode::EvaluateSplitIrrepOverLeviParabolic
   );
  this->AddOneFunctionToDictionaryNoFail
  ("splitFDPartGenVermaOverLeviParabolic",
   "( (Rational, ...), (Rational, ...), (Rational, ...))",
   "<b>Might be hidden or changed in future versions. \
   </b>Splits the inducing module of a generalized verma module with highest weight vector given \
   by the first two arguments over \
   the Levi part of the parabolic subalgebra given by the \
   third argument.",
    "splitFDPartGenVermaOverLeviParabolic((0, 1,1),(1,0,0), (1,0,1))",
   'B', 3, true,
    & ParserNode::EvaluateSplitFDPartGenVermaOverLeviParabolic
   );
  this->AddOneFunctionToDictionaryNoFail
  ("splitCharOverLeviParabolic",
   "( Char, (Rational, ...))",
   "<b>Please don't use. At the time this function is for testing purposes only. Might be hidden or changed in future versions. \
   </b>First argument is a representation character. Second argument gives the parabolic selection.",
    "splitCharOverLeviParabolic(char(1,1),(0,1))",
   'B', 2, true,
    & ParserNode::EvaluateSplitCharOverLeviParabolic
   );
  this->AddOneFunctionToDictionaryNoFail
  ("makeWeylFromParabolicSelection",
   "(Rational, ...)",
   "<b>Please don't use. For testing purposes. \
   </b>Prints out information about the Weyl subgroup of the Levi part of the parabolic subalgebra given by \
   the argument.",
    "makeWeylFromParabolicSelection(0,0,1,0)",
   'F', 4, true,
    & ParserNode::EvaluateMakeWeylFromParSel
   );
/*  this->AddOneFunctionToDictionaryNoFail
  ("splitGenVermaBthreeOverGtwo",
   "((Rational, Rational), (Rational, Rational, Rational), (Rational, Rational, Rational))",
   "<b>Please don't use. For testing purposes. \
   </b>Prints out information about the Weyl subgroup of the Levi part of the parabolic subalgebra given by \
   the argument.",
    "splitGenVermaBthreeOverGtwo((-1,-1), (1,1,0), (1,0,0) )",
   'B', 3, true,
    & ParserNode::EvaluateSplitGenVermaBthreeOverGtwo
   );*/

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
