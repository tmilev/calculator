//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader1General0_General.h"
#include "vpfHeader2Math0_General.h"
#include "vpfImplementationHeader2Math051_PolynomialComputations_Basic.h"
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
    int whichDigit=input[i]-'0';
    if (whichDigit>9 || whichDigit<0)
    { std::cout << "This is a programming error: LargeIntUnsigned::AssignString" << " called on the string " << input
      << " which does not consist entirely of digits. Please note that LargeIntUnsigned::AssignString is a no-fail function, intended for "
      << " internal use only. If you want to parse arbitrary unsafe expressions coming from the user, please use the big gun (a.k.a. CommandList). "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    this->operator+=((unsigned) whichDigit);
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

void GeneralizedVermaModuleCharacters::TransformToWeylProjective(int indexOperator, Vector<Rational>& startingNormal, Vector<Rational>& outputNormal)
{ Matrix<Rational>  theOperatorExtended=this->theLinearOperatorsExtended[indexOperator];
  Vector<Rational>& theTranslation=this->theTranslationsProjectedBasisChanged[indexOperator];
  //the goddamned sign in front of theTranslation is now checked: it should be + and not -
  Rational theConst;
  startingNormal.ScalarEuclidean(this->NonIntegralOriginModificationBasisChanged+theTranslation, theConst);
  theOperatorExtended.Transpose();
  outputNormal=startingNormal;
  theOperatorExtended.ActOnVectorColumn(outputNormal);
  outputNormal.SetSize(outputNormal.size+1);
  *outputNormal.LastObject()=-theConst;
}

void GeneralizedVermaModuleCharacters::TransformToWeylProjectiveStep1(GlobalVariables& theGlobalVariables)
{ this->smallerAlgebraChamber.InitFromDirectionsAndRefine(this->GmodKNegWeightsBasisChanged, theGlobalVariables);
  ProgressReport theReport1(&theGlobalVariables);
  ProgressReport theReport2(&theGlobalVariables);
  theReport1.Report(this->smallerAlgebraChamber.ToString(false, false));
  this->log << "Directions for making the chamber basis changed: " << this->GmodKNegWeightsBasisChanged.ToString() << "\n Resulting chamber before projectivization:\n "
  << this->smallerAlgebraChamber.ToString(false, false);
  theReport2.Report(this->log.str());
}

void GeneralizedVermaModuleCharacters::TransformToWeylProjectiveStep2(GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  ConeComplex projectivizedChamberFinal;
  Cone currentProjectiveCone;
  Vectors<Rational> tempRoots, extraWeylChamberWalls;
  Vector<Rational> tempRoot, wallAtInfinity, wallToSliceWith;
  ProgressReport theReport(&theGlobalVariables);
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
    theReport.Report(tempRoots.ToString());
    currentProjectiveCone.CreateFromNormals(tempRoots, &theGlobalVariables);
    projectivizedChamberFinal.AddNonRefinedChamberOnTopNoRepetition(currentProjectiveCone, theGlobalVariables);
  }
  for (int i=0; i<this->PreimageWeylChamberSmallerAlgebra.Normals.size; i++)
    projectivizedChamberFinal.splittingNormals.AddOnTop(this->PreimageWeylChamberSmallerAlgebra.Normals[i]);
  out << "projectivized chamber before chopping non-dominant part:\n" << projectivizedChamberFinal.ToString(false, false);
  projectivizedChamberFinal.Refine(theGlobalVariables);
  out << "Refined projectivized chamber before chopping non-dominant part:\n" << projectivizedChamberFinal.ToString(false, false);
  for (int i=0; i<projectivizedChamberFinal.size; i++)
  { const Cone& currentCone=projectivizedChamberFinal[i];
    bool isNonDominant=false;
    for (int j=0; j<this->PreimageWeylChamberSmallerAlgebra.Normals.size; j++)
      if (currentCone.GetInternalPoint().ScalarEuclidean(this->PreimageWeylChamberSmallerAlgebra.Normals[j]).IsNegative())
      { isNonDominant=true;
        break;
      }
    if (isNonDominant)
    { projectivizedChamberFinal.PopChamberSwapWithLast(i);
      i--;
    }
  }
  theReport.Report(out.str());

  projectivizedChamberFinal.indexLowestNonRefinedChamber=0;
  this->projectivizedChambeR=projectivizedChamberFinal;
  for (int k=1; k<this->theLinearOperators.size; k++)
    for (int i=0; i<this->smallerAlgebraChamber.size; i++)
      for (int j=0; j<this->smallerAlgebraChamber[i].Normals.size; j++)
      { this->TransformToWeylProjective(k, this->smallerAlgebraChamber[i].Normals[j], wallToSliceWith);
        wallToSliceWith.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
        this->projectivizedChambeR.splittingNormals.AddOnTopNoRepetition(wallToSliceWith);
      }
  out << "projectivized chamber chopped non-dominant part:\n"  << this->projectivizedChambeR.ToString(false, false);
  theReport.Report(out.str());
}

void HomomorphismSemisimpleLieAlgebra::ApplyHomomorphism(const ElementSemisimpleLieAlgebra<Rational>& input, ElementSemisimpleLieAlgebra<Rational>& output)
{ assert(&output!=&input);
  output.MakeZero();
  for (int i=0; i<input.size(); i++)
  { int currentIndex=input[i].theGeneratorIndex;
    output+=this->imagesAllChevalleyGenerators[currentIndex]*input.theCoeffs[i];
  }
}

void HomomorphismSemisimpleLieAlgebra::GetMapSmallCartanDualToLargeCartanDual(Matrix<Rational> & output)
{ output.init(this->theRange().GetRank(), this->theDomain().GetRank());
  ElementSemisimpleLieAlgebra<Rational> domainElt, imageElt;
  for (int i=0; i<this->theDomain().GetRank(); i++)
  { domainElt.MakeHgenerator
    (Vector<Rational>::GetEi(this->theDomain().GetRank(), i), this->theDomain());
    this->ApplyHomomorphism(domainElt, imageElt);
    output.AssignVectorToColumnKeepOtherColsIntactNoInit(i, imageElt.GetCartanPart());
  }
}

Vector<Rational> ReflectionSubgroupWeylGroup::GetRho()
{ Vector<Rational> result;
  this->RootsOfBorel.sum(result, this->AmbientWeyl.GetDim());
  result/=2;
  return result;
}

void ReflectionSubgroupWeylGroup::GetMatrixOfElement(const ElementWeylGroup& input, Matrix<Rational>& outputMatrix)const
{ Vectors<Rational> startBasis, imageBasis ;
  startBasis.MakeEiBasis(this->AmbientWeyl.GetDim());
  this->ActByElement(input, startBasis, imageBasis);
  outputMatrix.AssignVectorsToRows(imageBasis);
  outputMatrix.Transpose();
}

void GeneralizedVermaModuleCharacters::initFromHomomorphism(Vector<Rational>& theParabolicSel, HomomorphismSemisimpleLieAlgebra& input, GlobalVariables& theGlobalVariables)
{ Vectors<Rational> tempRoots;
  this->WeylLarger=input.theRange().theWeyl;
  this->WeylSmaller=input.theDomain().theWeyl;
  WeylGroup& theWeYl=input.theRange().theWeyl;
//  input.ProjectOntoSmallCartan(theWeyl.RootsOfBorel, tempRoots, theGlobalVariables);
  this->log << "projections: " << tempRoots.ToString();
  theWeYl.ComputeAllElements();
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
//  this->log << "weights of g mod k: " << this->GmodKnegativeWeights.ToString();
  Matrix<Rational>  tempMat;
  tempMat=input.theDomain().theWeyl.CartanSymmetric;
  tempMat.Invert(&theGlobalVariables);
//  tempMat.ActOnVectorsColumn(this->GmodKnegativeWeightS);
  this->log << this->GmodKnegativeWeightS.ToString();
  this->preferredBasiS.SetSize(2);
  this->preferredBasiS[0]=-this->GmodKnegativeWeightS[1];
  this->preferredBasiS[1]=-this->GmodKnegativeWeightS[2];
  /////////////////////////////////////////
  //this->preferredBasiS[0]="(1,0)";
  //this->preferredBasiS[1]="(0,1)";
  ////////////////////////////////////////
  this->preferredBasisChangE.AssignVectorsToRows(this->preferredBasiS);
  this->preferredBasisChangE.Transpose();
  this->preferredBasisChangeInversE=this->preferredBasisChangE;
  this->preferredBasisChangeInversE.Invert();
  this->preferredBasisChangeInversE.ActOnVectorsColumn
  (this->GmodKnegativeWeightS, this->GmodKNegWeightsBasisChanged);
  this->log << "\nWeights after basis change: " << this->GmodKNegWeightsBasisChanged.ToString();
  for (int i=0; i<this->GmodKnegativeWeightS.size; i++)
    if (this->GmodKnegativeWeightS[i].IsPositiveOrZero())
    { this->GmodKnegativeWeightS.RemoveIndexSwapWithLast(i);
      i--;
    }
  for (int i=0; i<this->GmodKNegWeightsBasisChanged.size; i++)
    if (this->GmodKNegWeightsBasisChanged[i].IsPositiveOrZero())
    { this->GmodKNegWeightsBasisChanged.RemoveIndexSwapWithLast(i);
      i--;
    }
  this->log << "\nNegative weights after basis change: " << this->GmodKNegWeightsBasisChanged.ToString();
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
  { DualCartanEmbedding.GetVectorFromColumn(i, tempRoot);
    if (ParabolicEvaluationRootImage.ScalarEuclidean(tempRoot).IsPositive())
      this->ParabolicSelectionSmallerAlgebra.AddSelectionAppendNewIndex(i);
  }
  this->log << "\nDual cartan embedding smaller into larger:\n" <<
  DualCartanEmbedding.ToString(&theGlobalVariables.theDefaultFormat);
  this->log << "\nParabolic subalgebra large algebra: " << this->ParabolicLeviPartRootSpacesZeroStandsForSelected.ToString();
  tempRoot= this->ParabolicSelectionSmallerAlgebra;
  this->log << "\nParabolic subalgebra smaller algebra: " << tempRoot.ToString();
  theSubgroup.MakeParabolicFromSelectionSimpleRoots(theWeYl, this->ParabolicLeviPartRootSpacesZeroStandsForSelected, theGlobalVariables, -1);

  this->theLinearOperators.SetSize(theSubgroup.size);
  this->theLinearOperatorsExtended.SetSize(theSubgroup.size);
  this->theTranslationS.SetSize(theSubgroup.size);
  this->theTranslationsProjectedBasisChanged.SetSize(theSubgroup.size);
  this->theCoeffs.SetSize(theSubgroup.size);
  this->log << " \n******************\nthe subgroup: \n" << theSubgroup.ToString() << "\n\n\n\n\n\n";
  this->log << theSubgroup.ElementToStringBruhatGraph();
  this->log << "\nMatrix form of the elements of Weyl group of the Levi part of the parabolic (" << theSubgroup.size << " elements):\n";
  for (int i=0; i<theSubgroup.size; i++)
  { Matrix<Rational>& currentLinearOperator=this->theLinearOperators[i];
    theSubgroup.GetMatrixOfElement(theSubgroup[i], currentLinearOperator);
//    currentLinearOperator.MultiplyOnTheLeft(preferredBasisChangeInverse);
    this->log << "\n" << currentLinearOperator.ToString(&theGlobalVariables.theDefaultFormat);
    currentLinearOperator.ActOnVectorColumn(theSubgroup.GetRho(), this->theTranslationS[i]);
    this->theTranslationS[i]-=theSubgroup.GetRho();
    this->theTranslationS[i].Minus();
    theProjectionBasisChanged.ActOnVectorColumn(this->theTranslationS[i], this->theTranslationsProjectedBasisChanged[i]);
    if (theSubgroup[i].size%2==0)
      this->theCoeffs[i]=1;
    else
      this->theCoeffs[i]=-1;
  }
  this->log << "\n\n\nMatrix of the projection operator (basis-changed):\n"
  << theProjectionBasisChanged.ToString(&theGlobalVariables.theDefaultFormat);
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
    this->log << "\n\n" << currentLOExtended.ToString(&theGlobalVariables.theDefaultFormat);
    this->log << this->theTranslationS[k].ToString() << ";   " << this->theTranslationsProjectedBasisChanged[k].ToString();
  }

  List<int> displayIndicesReflections;
  for (int i=0; i<this->ParabolicLeviPartRootSpacesZeroStandsForSelected.MaxSize; i++)
    if (!this->ParabolicLeviPartRootSpacesZeroStandsForSelected.selected[i])
      displayIndicesReflections.AddOnTop(i+1);
  Matrix<Polynomial<Rational> > tempMatPoly;
  Vector<Polynomial<Rational> > tempVect, tempVect2;
  tempVect.SetSize(input.theDomain().theWeyl.GetDim()+input.theRange().theWeyl.GetDim());
  for (int i=0; i<tempVect.size; i++)
    tempVect[i].MakeMonomiaL(i, 1, (Rational) 1, tempVect.size);
  tempMatPoly.init(input.theDomain().theWeyl.GetDim(), tempVect.size);
  Polynomial<Rational> polyZero;
  polyZero.MakeZero();
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
  theSubgroup.GetRho().ToStringLetterFormat("\\eta")
  << "$; $\\pr(\\rho)=" << tempRoot.ToStringLetterFormat("\\alpha") << "$."
  ;
  this->log << "\n\n\\begin{longtable}{r|l}$w$ & \\begin{tabular}{c}Argument of the vector partition function in (\\ref{eqMultG2inB3General}) =\\\\ $u_w\\circ"
  << tempVect.ToString(&theFormat) << "-\\tau_w$ \\end{tabular}  \\\\ \\hline \\endhead";
  for (int i=0; i<this->theLinearOperatorsExtended.size; i++)
  { Matrix<Rational> & currentLoExt=this->theLinearOperatorsExtended[i];
    for (int j=0; j<currentLoExt.NumRows; j++)
      for (int k=0; k<currentLoExt.NumCols; k++)
        tempMatPoly.elements[j][k].MakeConst(currentLoExt.elements[j][k], tempVect.size);
    tempMatPoly.ActOnVectorColumn(tempVect, tempVect2, polyZero);
    for (int j=0; j<tempVect2.size; j++)
      tempVect2[j]+=this->theTranslationsProjectedBasisChanged[i][j];
    this->log << "\n$" <<  theSubgroup[i].ToString(0, & displayIndicesReflections) << "$&$"
    << tempVect2.ToString(&theFormat) << "$\\\\";
  }
  this->log <<"\\end{longtable}\n\n";
//  this->log << "\n\n\nThere are " << tempList.size << " different operators.";
  Lattice tempLattice;
  theWeYl.GetIntegralLatticeInSimpleCoordinates(tempLattice);
  this->theExtendedIntegralLatticeMatForM.basisRationalForm.MakeIdMatrix(input.theDomain().GetRank());
  this->theExtendedIntegralLatticeMatForM.basisRationalForm.DirectSumWith(tempLattice.basisRationalForm, (Rational) 0);
  this->theExtendedIntegralLatticeMatForM.MakeFromMat(this->theExtendedIntegralLatticeMatForM.basisRationalForm);
  tempMat=theWeYl.CartanSymmetric;
  tempMat.Invert(&theGlobalVariables);
  Vectors<Rational> WallsWeylChamberLargerAlgebra;
  for (int i=0; i<tempMat.NumRows; i++)
  { tempMat.GetVectorFromRow(i, tempRoot);
    if(ParabolicEvaluationRootImage[i].IsEqualToZero())
    { WallsWeylChamberLargerAlgebra.SetSize(WallsWeylChamberLargerAlgebra.size+1);
      *WallsWeylChamberLargerAlgebra.LastObject()=tempRoot;
    }
  }
  this->log << "\n\n\n**************\nParabolic selection larger algebra:" << ParabolicEvaluationRootImage.ToString() << "\nWalls Weyl chamber larger algebra: " << WallsWeylChamberLargerAlgebra.ToString();
  this->log << "\n**************\n\n";
  Vectors<Rational> rootsGeneratingExtendedLattice;
  int totalDim=input.theRange().GetRank()+input.theDomain().GetRank();
  rootsGeneratingExtendedLattice.SetSize(totalDim);
  this->log << "\n" << tempMat.ToString(&theGlobalVariables.theDefaultFormat) << "\n";
  this->log << this->theExtendedIntegralLatticeMatForM.ToString(false, false);
  this->WeylChamberSmallerAlgebra.CreateFromNormals(WallsWeylChamberLargerAlgebra, &theGlobalVariables);
  this->log << "\nWeyl chamber larger algebra before projectivizing: " << this->WeylChamberSmallerAlgebra.ToString(&theFormat) << "\n";
  this->PreimageWeylChamberSmallerAlgebra.Normals=this->WeylChamberSmallerAlgebra.Normals;
  for (int i=0; i<this->PreimageWeylChamberLargerAlgebra.Normals.size; i++)
  { tempRoot.MakeZero(input.theRange().GetRank()+input.theDomain().GetRank()+1);
    for (int j=0; j<input.theRange().GetRank(); j++)
      tempRoot.TheObjects[j+input.theDomain().GetRank()]=this->PreimageWeylChamberLargerAlgebra.Normals[i][j];
    this->PreimageWeylChamberLargerAlgebra.Normals[i]=tempRoot;
  }
  tempMat=input.theDomain().theWeyl.CartanSymmetric;
  tempMat.Invert(&theGlobalVariables);
  tempRoots.size=0;
  Vector<Rational> ParabolicEvaluationRootSmallerAlgebra;
  ParabolicEvaluationRootSmallerAlgebra=this->ParabolicSelectionSmallerAlgebra;
  for (int i=0; i<tempMat.NumRows; i++)
  { input.theDomain().theWeyl.CartanSymmetric.GetVectorFromRow(i, tempRoot);
    if (tempRoot.ScalarEuclidean(ParabolicEvaluationRootSmallerAlgebra).IsEqualToZero())
    { tempRoots.SetSize(tempRoots.size+1);
      tempMat.GetVectorFromRow(i, *tempRoots.LastObject());
    }
  }
  this->preferredBasisChangeInversE.ActOnVectorsColumn(tempRoots);
  this->log << "**********************\n\n\n";
  this->log << "\nthe smaller parabolic selection: " << this->ParabolicSelectionSmallerAlgebra.ToString();
  this->log << "the Vectors<Rational> generating the chamber walls: " << tempRoots.ToString();
  this->PreimageWeylChamberSmallerAlgebra.CreateFromVertices(tempRoots, &theGlobalVariables);
  this->log << "\nWeyl chamber smaller algebra: " << this->PreimageWeylChamberSmallerAlgebra.ToString(&theFormat) << "\n";
  this->log << "**********************\n\n\n";
  this->log << "\nThe first operator extended:\n"
  << this->theLinearOperatorsExtended.TheObjects[0].ToString(&theGlobalVariables.theDefaultFormat) << "\n";
  this->log << "\nThe second operator extended:\n"
  << this->theLinearOperatorsExtended.TheObjects[1].ToString(&theGlobalVariables.theDefaultFormat) << "\n";
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
  this->log << "\nPreimage Weyl chamber smaller algebra: " << this->PreimageWeylChamberSmallerAlgebra.ToString(&theFormat) << "\n";
  this->log << "\nPreimage Weyl chamber larger algebra: " << this->PreimageWeylChamberLargerAlgebra.ToString(&theFormat) << "\n";

  //theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  //theGlobalVariables.theIndicatorVariables.StatusString1=this->log.str();
  //theGlobalVariables.MakeReport();
}

void WeylGroup::GetMatrixOfElement(int theIndex, Matrix<Rational>& outputMatrix)const
{ this->GetMatrixOfElement(this->theElements[theIndex], outputMatrix);
}

bool ReflectionSubgroupWeylGroup::MakeParabolicFromSelectionSimpleRoots
(WeylGroup& inputWeyl, const Selection& ZeroesMeanSimpleRootSpaceIsInParabolic, GlobalVariables& theGlobalVariables, int UpperLimitNumElements)
{ Vectors<Rational> selectedRoots;
  selectedRoots.ReservE
  (ZeroesMeanSimpleRootSpaceIsInParabolic.MaxSize-
   ZeroesMeanSimpleRootSpaceIsInParabolic.CardinalitySelection);
  this->AmbientWeyl=inputWeyl;
  for (int i=0; i<ZeroesMeanSimpleRootSpaceIsInParabolic.MaxSize; i++)
    if (!ZeroesMeanSimpleRootSpaceIsInParabolic.selected[i])
    { selectedRoots.SetSize(selectedRoots.size+1);
      selectedRoots.LastObject()->MakeEi(inputWeyl.GetDim(), i);
    }
  List<Vectors<Rational> > tempRootsCol;
  return
  this->ComputeSubGroupFromGeneratingReflections
  (&selectedRoots, &tempRootsCol, &theGlobalVariables, UpperLimitNumElements, true);
}

void WeylGroup::GetMatrixOfElement(const ElementWeylGroup& input, Matrix<Rational>& outputMatrix)const
{ Vector<Rational> tempRoot;
  int theDim=this->CartanSymmetric.NumRows;
  outputMatrix.init(theDim, theDim);
  for (int i=0; i<theDim; i++)
  { tempRoot.MakeEi(theDim, i);
    this->ActOn(input, tempRoot);
    for (int j=0; j<theDim; j++)
      outputMatrix.elements[j][i]=tempRoot.TheObjects[j];
  }
}

/*
void WeylGroup::GetElementOfMatrix(Matrix<Rational> &input, ElementWeylGroup &output)
{ Vector<Rational> v = input * this->rho;
  Vector<Rational> v2;
  int i;
  while(v != rho){
      for(i=0;i<this->generatorMatrices.size;i++){
          v2 = generatorMatrices[i]*v;
          if(v2 > v){
             v = v2;
             output.AddOnTop(i);
             break;
          }
      }
      if(i==this.generatorMatrices.size){
        std::cout << "matrix" << input << "did not belong to Weyl group" << CGI::GetStackTraceEtcErrorMessage(__FILE__,__LINE__) << std::endl;
        }
  }
}
*/

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
  assert(false);
/*
  for (int i=0; i<this->thePfs.theChambersOld.size; i++)
    if (this->thePfs.theChambersOld.TheObjects[i]!=0)
    { QuasiPolynomial& currentQPNoSub= this->theQPsNonSubstituted.TheObjects[i];
      this->theQPsSubstituted.TheObjects[i].SetSize(this->theLinearOperators.size);
      this->thePfs.GetVectorPartitionFunction(currentQPNoSub, this->thePfs.theChambersOld.TheObjects[i]->InternalPoint, theGlobalVariables);
      out << "\nChamber " << i+1 << " with internal point " << this->thePfs.theChambersOld.TheObjects[i]->InternalPoint.ToString() << " the quasipoly is: " << currentQPNoSub.ToString(false, false);
      for (int k=0; k<this->theLinearOperators.size; k++)
      { QuasiPolynomial& currentQPSub=this->theQPsSubstituted.TheObjects[i].TheObjects[k];
        std::stringstream tempStream;
        tempStream << "Processing chamber " << i+1 << " linear operator " << k+1;
        theGlobalVariables.theIndicatorVariables.ProgressReportStrings[0]= tempStream.str();
        theGlobalVariables.MakeReport();
        currentQPNoSub.Substitution(this->theLinearOperatorsExtended.TheObjects[k], this->theTranslationsProjectedBasisChanged[k], this->theExtendedIntegralLatticeMatForM, currentQPSub, theGlobalVariables);
        out << "; after substitution we get: " << currentQPSub.ToString(false, false);
      }
    }
    */
//  this->theParser.theHmm.theRange.theWeyl.GetIntegralLatticeInSimpleCoordinates(integralLattice);
  //out << "\nThe integral lattice:\n" << integralLattice.ToString(false, false);
  //this->theMultiplicitiesMaxOutputReport2.flush();
  QuasiPolynomial tempQP;
  this->theMultiplicities.SetSize(this->projectivizedChambeR.size);
  this->numNonZeroMults=0;
  ProgressReport theReport(&theGlobalVariables);
  ProgressReport theReport2(&theGlobalVariables);
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
      theReport.Report(tempStream.str());
    }
    if (!currentSum.IsEqualToZero())
      this->numNonZeroMults++;
    std::stringstream tempStream;
    tempStream << " So far " << i+1 << " out of " << this->projectivizedChambeR.size << " processed " << this->numNonZeroMults
    << " non-zero total.";
    theReport2.Report(tempStream.str());
    out << "\nChamber " << i+1 << ": the quasipolynomial is: " << currentSum.ToString(false, false);
    out << "\nThe chamber is: " << this->projectivizedChambeR[i].ToString(&theFormat);
  }
//  this->projectivizedChamber.ComputeDebugString();
//  out << "\n\n" << this->projectivizedChamber.DebugString;
  theReport.Report(out.str());
  this->theMultiplicitiesMaxOutputReport2 << out.str();
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
    tempCone.CreateFromNormals(newNormals, &theGlobalVariables);
    output.AddNonRefinedChamberOnTopNoRepetition(tempCone, theGlobalVariables);
  }
}

template<class coefficient>
Vector<coefficient> Vector<coefficient>::GetProjectivizedNormal(Vector<coefficient>& affinePoint)
{ Vector<coefficient> result=*this;
  result.SetSize(this->size+1);
  *result.LastObject()=-affinePoint.ScalarEuclidean(*this);
  return result;
}

void Lattice::GetRootOnLatticeSmallestPositiveProportionalTo
(Vector<Rational>& input, Vector<Rational>& output, GlobalVariables& theGlobalVariables)
{ assert(&input!=&output);
  Vectors<Rational> theBasis;
  Vector<Rational> tempRoot;
  theBasis.AssignMatrixRows(this->basisRationalForm);
  input.GetCoordsInBasiS(theBasis, tempRoot);
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
 Vectors<Rational>& outputPoints, Vector<Rational>* shiftAllPointsBy)const
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
  outputPoints.ReservE(numCycles);
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
  PartFractions theFracs;
  FormatExpressions theFormat;
  std::stringstream out;
  std::string whatWentWrong;

  theFracs.initFromRoots(theRoots, theGlobalVariables);
  out << CGI::GetHtmlMathDivFromLatexFormulA(theFracs.ToString(theGlobalVariables, theFormat));
  theFracs.split(theGlobalVariables, 0);
  out << CGI::GetHtmlMathDivFromLatexFormulA(theFracs.ToString(theGlobalVariables, theFormat));
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
  baseCone.CreateFromVertices(theRoots, &theGlobalVariables);
  Vector<Rational> shiftRoot;
  baseLattice.GetInternalPointInConeForSomeFundamentalDomain(shiftRoot, baseCone, theGlobalVariables);
  shiftRoot.Minus();
//  std::cout << "shiftRoot: " << shiftRoot.ToString();
  theFracs.theChambers.MakeAffineAndTransformToProjectiveDimPlusOne
  (shiftRoot, this->theProjectivizedComplex, theGlobalVariables);
  outputstring=out.str();
  return true;
}

bool Lattice::GetInternalPointInConeForSomeFundamentalDomain
(Vector<Rational>& output, Cone& coneContainingOutputPoint, GlobalVariables& theGlobalVariables)
{ Vector<Rational> coordsInBasis;
  coneContainingOutputPoint.GetInternalPoint(output);
  Vectors<Rational> basisRoots;
  basisRoots.AssignMatrixRows(this->basisRationalForm);
  if (!output.GetCoordsInBasiS(basisRoots, coordsInBasis))
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
      outputHashed.AddOnTopNoRepetition(tempRoot);
    }
  output=(outputHashed);
}

void ConeComplex::RefineMakeCommonRefinement(const ConeComplex& other, GlobalVariables& theGlobalVariables)
{ Vectors<Rational> newWalls;
  Cone tempCone=this->ConvexHull;
  if (tempCone.MakeConvexHullOfMeAnd(other.ConvexHull, theGlobalVariables))
  { this->GetAllWallsConesNoOrientationNoRepetitionNoSplittingNormals(newWalls);
    this->init();
    this->ConvexHull=tempCone;
    this->AddNonRefinedChamberOnTopNoRepetition(tempCone, theGlobalVariables);
    this->splittingNormals.AddOnTopNoRepetition(newWalls);
  }
  other.GetAllWallsConesNoOrientationNoRepetitionNoSplittingNormals(newWalls);
  this->splittingNormals.AddOnTopNoRepetition(newWalls);
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
//  std::cout << "the translation: " << translateToBeAddedToArgument.ToString();
  this->theProjectivizedComplex.TranslateMeMyLastCoordinateAffinization(chamberShift, theGlobalVariables);
  QuasiPolynomial tempQP;
  for (int i=0; i<this->theQPs.size; i++)
  { this->theQPs[i].Substitution(-translateToBeAddedToArgument, tempQP, theGlobalVariables);
    this->theQPs[i]=tempQP;
  }
}

std::string PiecewiseQuasipolynomial::ToString(bool useLatex, bool useHtml)
{ std::stringstream out;
  FormatExpressions theFormat;
  for (int i=0; i<this->theProjectivizedComplex.size; i++)
  { const Cone& currentCone= this->theProjectivizedComplex[i];
    QuasiPolynomial& currentQP=this->theQPs[i];
    out << "Chamber number " << i+1;
    if (useHtml)
      out << "<br>";
    out << currentCone.ToString(&theFormat);
    if (useHtml)
      out << "<br>";
    out << "quasipolynomial: ";
    if (useLatex& useHtml)
      out << CGI::GetHtmlMathSpanFromLatexFormulaAddBeginArrayL(currentQP.ToString(useHtml, useLatex));
    else
    { out << currentQP.ToString(useHtml, useLatex);
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
    (theLatticePointsFinal[i], this->EvaluateInputProjectivized(theLatticePointsFinal[i]).ToString(),
     theLatticePointColors[i],
    DrawingVariables::PenStyleNormal, 0);
  }
}

Rational QuasiPolynomial::Evaluate(const Vector<Rational>& input)
{ Vector<Rational> testLatticeBelonging;
  for (int i=0; i<this->LatticeShifts.size; i++)
  { testLatticeBelonging=this->LatticeShifts[i]-input;
    if (this->AmbientLatticeReduced.IsInLattice(testLatticeBelonging))
      return this->valueOnEachLatticeShift[i].Evaluate(input);
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
        std::cout << "<br>Evaluating at point " << AffineInput.ToString() << "<br>";
        std::cout << "<br>Chamber " << theIndex+1 << ": "
        << this->theProjectivizedComplex[theIndex].ToString(&tempFormat);
        std::cout << "<br>QP: " << this->theQPs[theIndex].ToString(true, false);
        std::cout << "<br>value: " << result.ToString();
        std::cout << "<br><br>Chamber " << i+1 << ": "
        << this->theProjectivizedComplex[i].ToString(&tempFormat);
        std::cout << "<br>QP: " << this->theQPs[i].ToString(true, false);
        std::cout << "<br>value: " << altResult.ToString();
        if (firstFail)
        { DrawingVariables tempDV;
          std::cout << "<br><b>Point of failure: " << AffineInput.ToString() << "</b>";
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
  { this->basisRationalForm.GetVectorFromRow(i, tempRoot);
    output+=tempRoot;
  }
  output/=2;
}

bool PartFractions::split(GlobalVariables& theGlobalVariables, Vector<Rational>* Indicator)
{ //PartFraction::flagAnErrorHasOccurredTimeToPanic=true;
  //this->flagAnErrorHasOccurredTimeToPanic=true;
  if (!this->flagInitialized)
  { this->IndexLowestNonProcessed=0;
    this->PrepareIndicatorVariables();
    this->PrepareCheckSums(theGlobalVariables);
    this->flagInitialized=true;
  }
//  std::cout << "<br>checksum start: " << this->StartCheckSum.ToString();
  if (this->splitPartial(theGlobalVariables, Indicator))
  { //this->ComputeDebugString();
//    this->CompareCheckSums(theGlobalVariables);
    this->RemoveRedundantShortRoots(theGlobalVariables, Indicator);
    //this->ComputeDebugString();
    //this->ComputeDebugString();
    this->CompareCheckSums(theGlobalVariables);
    this->IndexLowestNonProcessed= this->size();
    this->MakeProgressReportSplittingMainPart(theGlobalVariables);
  }
//  std::cout << "<br>checksum finish: " << this->EndCheckSum.ToString();

  return false;
}

void Cone::ChangeBasis
  (Matrix<Rational>& theLinearMap, GlobalVariables& theGlobalVariables)
{ //Vectors<Rational> newNormals;
//  Matrix<Rational> tempMat=theLinearMap;
  theLinearMap.ActOnVectorsColumn(this->Normals);
  this->CreateFromNormals(this->Normals, &theGlobalVariables);
}

std::string GeneralizedVermaModuleCharacters::ComputeMultsLargerAlgebraHighestWeight
  (Vector<Rational>& highestWeightLargerAlgebraFundamentalCoords, Vector<Rational>& parabolicSel,
   GlobalVariables& theGlobalVariables)
{ std::stringstream out;
  WeylGroup& LargerWeyl=this->theHmm.theRange().theWeyl;
  WeylGroup& SmallerWeyl=this->theHmm.theDomain().theWeyl;
  if (!LargerWeyl.IsOfSimpleType('B', 3))
    return "Error: algebra is not so(7).";
  this->initFromHomomorphism(parabolicSel, this->theHmm, theGlobalVariables);
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
  tmpWeyl.MakeArbitrarySimple('A',2);
  drawOps.theBuffer.initDimensions(tmpWeyl.CartanSymmetric, theDraggableBasis, theDraggableBasis, 1);
  FormatExpressions theFormat;
  drawOps.theBuffer.BasisProjectionPlane[0][0][0]=1; drawOps.theBuffer.BasisProjectionPlane[0][0][1]=0;
  drawOps.theBuffer.BasisProjectionPlane[0][1][0]=1;  drawOps.theBuffer.BasisProjectionPlane[0][1][1]=1;
  drawOps.theBuffer.ModifyToOrthonormalNoShiftSecond
  (drawOps.theBuffer.BasisProjectionPlane[0][1], drawOps.theBuffer.BasisProjectionPlane[0][0]);
  drawOps.theBuffer.GraphicsUnit[0]=50;
  PiecewiseQuasipolynomial theStartingPoly(theGlobalVariables),
  theSubbedPoly(theGlobalVariables), Accum(theGlobalVariables);
  //std::cout << "<hr>" << this->GmodKNegWeightsBasisChanged.ToString() << "<hr>";
  std::string tempS;
  theStartingPoly.MakeVPF(this->GmodKNegWeightsBasisChanged, tempS, theGlobalVariables);
  Vectors<Rational> translationsProjectedFinal;
  translationsProjectedFinal.SetSize(this->theLinearOperators.size);
  this->theLinearOperators[0].ActOnVectorColumn(highestWeightLargerAlgSimpleCoords, translationsProjectedFinal[0]);
//  translationsProjectedFinal[0].Minus();
  out << "<br>Input so(7)-highest weight: " << highestWeightLargerAlgSimpleCoords.ToString();
  out << "<br>Input parabolics selections: " << parabolicSel.ToString();
  out << "<br>the argument translations: " << this->theTranslationsProjectedBasisChanged.ToString();
  out << "<br>Element u_w: projection, multiplication by -1, and basis change of so(7)-highest weight to G_2: "
  << translationsProjectedFinal[0].ToString();
  theStartingPoly.MakeVPF(this->GmodKNegWeightsBasisChanged, tempS, theGlobalVariables);
  //std::cout << theStartingPoly.ToString(false, true);
  drawOps.drawCoordSystemBuffer(drawOps, 2, 0);
  //out << this->log.str();
  Cone smallWeylChamber;
  tempMat=SmallerWeyl.CartanSymmetric;
  tempMat.Invert();
  Vectors<Rational> tempVertices;
  Vector<Rational> tMpRt;
  tMpRt=this->ParabolicSelectionSmallerAlgebra;
//  std::cout << "<br>sel smaller: " << tMpRt.ToString();
  for (int i=0; i<this->ParabolicSelectionSmallerAlgebra.MaxSize; i++)
  { tempMat.GetVectorFromRow(i, tempRoot);
    tempVertices.AddOnTop(tempRoot);
    if(this->ParabolicSelectionSmallerAlgebra.selected[i])
      tempVertices.AddOnTop(-tempRoot);
  }
  smallWeylChamber.CreateFromVertices(tempVertices, &theGlobalVariables);
  tempMat.init(2,2);
  tempMat.elements[0][0]=1; tempMat.elements[0][1]=0;
  tempMat.elements[1][0]=1; tempMat.elements[1][1]=1;

//  std::cout << smallWeylChamber.ToString(false, true, theFormat);
  tempMat.Transpose();
  smallWeylChamber.ChangeBasis
  (tempMat, theGlobalVariables)
  ;
//  std::cout << "<br> after the basis change: " << smallWeylChamber.ToString(false, true, theFormat);
  out << "<br> The small Weyl chamber: " << smallWeylChamber.ToString(&theFormat);
  Vector<Rational> highestWeightSmallAlgBasisChanged= -translationsProjectedFinal[0];
//  std::cout << highestWeightSmallAlgBasisChanged.ToString();
  theGlobalVariables.MaxComputationTimeSecondsNonPositiveMeansNoLimit=100;
  for (int i=0; i<this->theLinearOperators.size; i++)
  { this->theLinearOperators[i].ActOnVectorColumn(highestWeightLargerAlgSimpleCoords, translationsProjectedFinal[i]);
    translationsProjectedFinal[i]+=this->theTranslationsProjectedBasisChanged[i];
    drawOps.drawCircleAtVectorBuffer(-translationsProjectedFinal[i], 3, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(250,0,0));
  }
  out << "<br>the translations projected final: " << translationsProjectedFinal.ToString();
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
  drawOps.theBuffer.theDrawCircleAtVectorOperations.ReservE(2500);
  Accum.DrawMe(drawOps, 10, &smallWeylChamber, &highestWeightSmallAlgBasisChanged);
//  smallWeylChamber.DrawMeProjective(0, false, drawOps, theFormat);
//  out << tempVars.GetHtmlFromDrawOperationsCreateDivWithUniqueName(2);
  out << drawOps.GetHtmlFromDrawOperationsCreateDivWithUniqueName(2);
  out << Accum.ToString(false, true);
/*  out << refinedProjectivized.ToString(false, true);*/
/*
  for (int i=0; i<this->theLinearOperators.size; i++)
  { this->theLinearOperators[i].ActOnVectorColumn(highestWeightLargerAlg, tempRoot);
    tempRoot.Minus();
    tempRoot-=this->theTranslationsProjecteD[i];
    drawOps.
    drawCircleAtVectorBuffer(tempRoot, 2, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(0,0,100));
    drawOps.drawLineBetweenTwoVectorsBuffer(tempRoot, ZeroRoot, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(240, 240, 240));
    tempComplex.DrawMeProjective(&tempRoot, false, drawOps, tempFormat);
    out << tempRoot.ToString();
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
(int indexComponent, const Matrix<Rational>& theBilinearForm)
{ MacroRegisterFunctionWithName("DynkinDiagramRootSubalgebra::ComputeDynkinString");
  assert(indexComponent<this->SimpleBasesConnectedComponents.size);
  DynkinSimpleType& outputType=this->SimpleComponentTypes[indexComponent];
  Vectors<Rational>& currentComponent= this->SimpleBasesConnectedComponents[indexComponent];
  List<int>& currentEnds=this->indicesEnds[indexComponent];
  if (currentComponent.size<1)
  { std::cout << "This is a programming error: currentComponent is empty "
    << " which is impossible. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
//  std::cout << "<hr> Extracting component from " << currentComponent.ToString() << " with bilinear form "
//  << theBilinearForm.ToString();
  if (this->numberOfThreeValencyNodes(indexComponent, theBilinearForm)==1)
  {//type D or E
    //in type D first comes the triple node, then the long string, then the one-root strings
    // the long string is oriented with the end that is connected to the triple node having
    //smaller index
    //in type E similarly the longest string comes first oriented with the root that is
    //linked to the triple node having smaller index
    // then comes the second longest string (oriented in the same fashion)
    // and last the one-root string
    Vector<Rational> tripleNode;
    int tripleNodeindex=this->indicesThreeNodes[indexComponent];
    tripleNode=(currentComponent[tripleNodeindex]);
    Vectors<Rational> tempRoots;
    tempRoots=(currentComponent);
    tempRoots.RemoveIndexSwapWithLast(tripleNodeindex);
    DynkinDiagramRootSubalgebra tempDiagram;
    tempDiagram.ComputeDiagramTypeKeepInput(tempRoots, theBilinearForm);
    assert(tempDiagram.SimpleBasesConnectedComponents.size==3);
    List<int> indicesLongComponents;
    indicesLongComponents.size=0;
    Rational tempRat;
    for (int i=0; i<3; i++)
    { if(tempDiagram.SimpleBasesConnectedComponents[i].size>1)
        indicesLongComponents.AddOnTop(i);
      if (tempDiagram.SimpleBasesConnectedComponents[i][0].ScalarProduct(currentComponent[tripleNodeindex], theBilinearForm).IsEqualToZero())
        tempDiagram.SimpleBasesConnectedComponents[i].ReverseOrderElements();
    }
    for(int i=0; i<3; i++)
      for(int j=i+1; j<3; j++)
        if (tempDiagram.SimpleBasesConnectedComponents[i].size<tempDiagram.SimpleBasesConnectedComponents.TheObjects[j].size)
        { tempRoots=(tempDiagram.SimpleBasesConnectedComponents[i]);
          tempDiagram.SimpleBasesConnectedComponents[i]=(tempDiagram.SimpleBasesConnectedComponents.TheObjects[j]);
          tempDiagram.SimpleBasesConnectedComponents[j]=(tempRoots);
        }
    currentComponent.size=0;
    currentComponent.AddOnTop(tripleNode);
    for (int i=0; i<3; i++)
      currentComponent.AddListOnTop(tempDiagram.SimpleBasesConnectedComponents[i]);
    Rational theCoRootLength=4;
    theCoRootLength/=currentComponent[0].ScalarProduct(currentComponent[0], theBilinearForm);
    if ( indicesLongComponents.size==1 || indicesLongComponents.size==0)
      outputType.MakeArbitrary('D', currentComponent.size, theCoRootLength);
    else
      outputType.MakeArbitrary('E', currentComponent.size, theCoRootLength);
  } else
  { Rational length1, length2, tempRat;
    Rational colength1, colength2;
    length1=currentComponent[0].ScalarProduct(currentComponent[0], theBilinearForm);
    int numLength1=1;
    int numLength2=0;
    for(int i=1; i<currentComponent.size; i++)
      if (currentComponent[i].ScalarProduct(currentComponent[i], theBilinearForm)==length1)
        numLength1++;
      else
      { numLength2++;
        length2=currentComponent[i].ScalarProduct(currentComponent[i], theBilinearForm);
      }
    if (numLength2==0 )
    { //type A
//      if (length1.IsEqualTo(theWeyl.GetLongestRootLengthSquared()))
        outputType.MakeArbitrary('A', numLength1, ((Rational) 4)/length1);
//      else
//        out << this->SetComponent("A'", numLength1, indexComponent);
    } else
    {//the longer root should have smaller index
      Rational greaterlength, tempRat;
      int numGreaterLength=numLength2;
      int numSmallerLength=numLength1;
      greaterlength=(length2);
      if (length1>length2)
      { greaterlength.Assign(length1);
        numGreaterLength=numLength1;
        numSmallerLength=numLength2;
      }
      colength1=(Rational) 4/length1;
      colength2=(Rational) 4/length2;
      if (greaterlength>currentComponent[currentEnds[0]].ScalarProduct(currentComponent[currentEnds[0]], theBilinearForm))
        currentEnds.SwapTwoIndices(0, 1);
      if (numLength1==numLength2)
      {//B2, C2, F4 or G2
        if (numLength1!=1)
          outputType.MakeArbitrary('F', 4, colength1);
        else
        { if (length1>length2)
          { MathRoutines::swap(length1, length2);
            MathRoutines::swap(colength1, colength2);
            currentEnds.SwapTwoIndices(0, 1);//<- in G_2, the longer root comes second.
          }
          if (length2/length1==3)
            outputType.MakeArbitrary('G', 2, colength1);
          else
            outputType.MakeArbitrary('B', 2, colength1);
        }
      } else
      { if (numGreaterLength>numSmallerLength)
          outputType.MakeArbitrary('B', currentComponent.size, colength1);
        else
          outputType.MakeArbitrary('C', currentComponent.size, colength2);
      }
    }
    currentComponent.SwapTwoIndices(0, currentEnds[0]);
    for (int i=0; i<currentComponent.size; i++)
      for (int j=i+1; j<currentComponent.size; j++)
        if (!currentComponent[i].ScalarProduct(currentComponent[j], theBilinearForm).IsEqualToZero())
        { currentComponent.SwapTwoIndices(i+1, j);
          break;
        }
  }
}

std::string DynkinDiagramRootSubalgebra::ToStringRelativeToAmbientType(const DynkinSimpleType& ambientType)const
{ std::stringstream out;
  DynkinType theType;
  theType.MakeZero();
  for (int j=0; j<this->SimpleComponentTypes.size; j++)
    theType.AddMonomial(this->SimpleComponentTypes[j], 1);

  return theType.ToStringRelativeToAmbientType(ambientType);
}

void WeylGroup::GetWeylChamber
(Cone& output, GlobalVariables& theGlobalVariables)
{ Matrix<Rational> tempMat;
  tempMat=this->CartanSymmetric;
  tempMat.Invert();
  Vectors<Rational> tempRoots;
  tempRoots.AssignMatrixRows(tempMat);
  output.CreateFromVertices(tempRoots, &theGlobalVariables);
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
  out << "\n" << tempComplex.ToString();
  return out.str();
}

void GeneralizedVermaModuleCharacters::IncrementComputation
  (Vector<Rational>& parabolicSel, GlobalVariables& theGlobalVariables)
{ std::stringstream out;
//  this->UpperLimitChambersForDebugPurposes=5;
  this->thePauseControlleR.InitComputation();
  this->ParabolicLeviPartRootSpacesZeroStandsForSelected=parabolicSel;
  if (false)
  if (this->UpperLimitChambersForDebugPurposes==0 || this->theLinearOperators.size==0)
    this->ReadFromDefaultFile(&theGlobalVariables);
  switch (this->computationPhase)
  { case 0:
//      this->theParser.theHmm.MakeG2InB3(this->theParser, theGlobalVariables);
      this->initFromHomomorphism(parabolicSel, this->theHmm, theGlobalVariables);
      this->TransformToWeylProjectiveStep1(theGlobalVariables);
//      out << theGlobalVariables.theIndicatorVariables.StatusString1;
      this->TransformToWeylProjectiveStep2(theGlobalVariables);
//      out << theGlobalVariables.theIndicatorVariables.StatusString1;
      break;
    case 1:
      this->projectivizedChambeR.Refine(theGlobalVariables);
      this->SortMultiplicities(theGlobalVariables);
      out << this->projectivizedChambeR.ToString(false, false);
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
//      out << theGlobalVariables.theIndicatorVariables.StatusString1;
      break;
    case 5:
      this->theMaxComputation.FindExtremaParametricStep1(this->thePauseControlleR, true, theGlobalVariables);
//      out << theGlobalVariables.theIndicatorVariables.StatusString1;
      break;
    case 6:
      this->theMaxComputation.FindExtremaParametricStep3(this->thePauseControlleR, theGlobalVariables);
//      out << theGlobalVariables.theIndicatorVariables.StatusString1;
      break;
    case 7:
      this->theMaxComputation.FindExtremaParametricStep4(this->thePauseControlleR, theGlobalVariables);
//      out << theGlobalVariables.theIndicatorVariables.StatusString1;
      break;
    case 8:
      this->theMaxComputation.FindExtremaParametricStep5(this->thePauseControlleR, theGlobalVariables);
//      out << theGlobalVariables.theIndicatorVariables.StatusString1;
      break;
    default:
      break;
  }
  this->computationPhase++;
//  theGlobalVariables.theIndicatorVariables.StatusString1=out.str();
  if (this->computationPhase>8)
  { //theGlobalVariables.theIndicatorVariables.StatusString1=this->PrepareReport(theGlobalVariables);
  }
//  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
//  theGlobalVariables.MakeReport();
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
{/* this->AmbientWeyl.GetElement().ComputeAllElements();
  this->log << this->AmbientWeyl.GetElement().ToString();
  std::string tempS;
  this->ToString(tempS);
  this->log << "\nWeyl chamber: " << this->WeylChamber.ToString() << "\n";
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
          this->theHyperplanes.AddOnTopNoRepetition(wallToSliceWith);
        }
  this->log << "\n Affine walls to slice with:";
  for (int i=0; i<this->NewHyperplanesToSliceWith.size; i++)
    this->log << "\n" << this->NewHyperplanesToSliceWith.TheObjects[i].ToString();
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
  this->log << "the global cone normals: " << this->TheGlobalConeNormals.ToString();
  this->ToString(tempS);
  this->log << tempS;
  theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
  theGlobalVariables.theIndicatorVariables.StatusString1=this->log.str();
  theGlobalVariables.MakeReport();*/
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

void Cone::SliceInDirection
  (Vector<Rational>& theDirection, ConeComplex& output, GlobalVariables& theGlobalVariables)
{ output.init();
  output.AddNonRefinedChamberOnTopNoRepetition(*this, theGlobalVariables);
  output.slicingDirections.AddOnTop(theDirection);
  //std::cout <<output.ToString(false, true);
  output.Refine(theGlobalVariables);
  //std::cout <<output.ToString(false, true);
}

void Lattice::ApplyLinearMap(Matrix<Rational> & theMap, Lattice& output)
{ Vectors<Rational> tempRoots;
  tempRoots.AssignMatrixRows(this->basisRationalForm);
  theMap.ActOnVectorsColumn(tempRoots);
  output.MakeFromRoots(tempRoots);
}

std::string ConeLatticeAndShiftMaxComputation::ToString
  (FormatExpressions* theFormat)
{ std::stringstream out;
  out << "<hr>Resulting lattice: " << this->theFinalRougherLattice.ToString(true, false) << "<hr><hr>";
/*  if (this->complexStartingPerRepresentative.size>0)
  { out << "<hr> Non-refined complex per representative:<br>\n ";
    for (int i=0; i<this->complexStartingPerRepresentative.size; i++)
    { out << "Lattice+shift="  << this->theFinalRepresentatives[i].ToString() << " + " << this->theFinalRougherLattice.ToString();
      out << "<br>\n" << this->complexStartingPerRepresentative[i].ToString(false, true);
      out << "the function we need to max: " << this->LPtoMaximizeSmallerDim[i].ToString();
    }
  }*/
  out << "<hr><hr>Cones not processed(number of cones " << this->theConesLargerDim.size << "):\n<hr>\n";
  DrawingVariables theDrawingVariables;
  Polynomial<Rational>  tempP;
  for (int i=0; i<this->theConesLargerDim.size; i++)
  { out << "";// << this->theConesLargerDim[i].ToString(theFormat);
    //out << "<br>" << this->LPtoMaximizeLargerDim[i].ToString();
    theDrawingVariables.theBuffer.init();
    out << "<br>" << this->theConesLargerDim[i].theProjectivizedCone.DrawMeToHtmlLastCoordAffine(theDrawingVariables, *theFormat);
    out << "<br>over " << this->theConesLargerDim[i].theShift.ToString() << " + " << this->theConesLargerDim[i].theLattice.ToString();
    tempP.MakeLinPolyFromRootLastCoordConst(this->LPtoMaximizeLargerDim[i]);
    out << "<br>the function we have maxed, as a function of the remaining variables, is: " << tempP.ToString(theFormat) << "<hr><hr>";
  }
  if (this->theConesSmallerDim.size>0)
  { out << "<br>Cones processed: <br>";
    for (int i=0; i<this->theConesSmallerDim.size; i++)
    { out << this->theConesSmallerDim[i].ToString(*theFormat);
      //out << "<br>" << this->LPtoMaximizeSmallerDim[i].ToString();
      theDrawingVariables.theBuffer.init();
      out << this->theConesSmallerDim[i].theProjectivizedCone.DrawMeToHtmlLastCoordAffine(theDrawingVariables, *theFormat);
    }
  }
  return out.str();
}

void ConeLatticeAndShiftMaxComputation::FindExtremaParametricStep3
    (Controller& thePauseController, GlobalVariables& theGlobalVariables)
{ this->theFinalRougherLattice=this->theConesLargerDim[0].theLattice;
  ProgressReport theReport(&theGlobalVariables);
  ProgressReport theReport2(&theGlobalVariables);
  for (int i=1; i<this->theConesLargerDim.size; i++)
  { this->theFinalRougherLattice.IntersectWith(this->theConesLargerDim[i].theLattice);
    std::stringstream tempStream;
    tempStream << "intersecing lattice " << i+1 << " out of " << this->theConesLargerDim.size;
    theReport.Report(tempStream.str());
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
    theReport2.Report(tempStream.str());
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
    { this->theConesLargerDim.RemoveIndexSwapWithLast(i);
      this->LPtoMaximizeLargerDim.RemoveIndexSwapWithLast(i);
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
  ProgressReport theReport(&theGlobalVariables);
  for (int i=0; i<this->theFinalRepresentatives.size; i++)
  { ConeComplex& currentComplex= this->complexRefinedPerRepresentative[i];
    currentComplex.initFromCones(this->complexStartingPerRepresentative[i], true, theGlobalVariables);
    std::stringstream tempStream;
    tempStream << "Processing representative " << i+1 << " out of " << this->theFinalRepresentatives.size;
    theReport.Report(tempStream.str());
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
    { const Cone& currentCone=this->complexRefinedPerRepresentative[i][j];
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
{ //std::cout << "<hr>starting complex: " << this->ToString();
  FormatExpressions tempFormat;
  ProgressReport theReport1(&theGlobalVariables);
  ProgressReport theReport2(&theGlobalVariables);
  ProgressReport theReport3(&theGlobalVariables);
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
      theReport1.Report(tempStream1.str());
      theReport2.Report(tempStream2.str());
      theReport3.Report(tempStream3.str());
    }
    //std::cout << "<hr><hr>" << this->ToString();
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
{ Matrix<Rational> matVertices;
  matVertices.AssignVectorsToRows(this->theProjectivizedCone.Vertices);
  Vectors<Rational> theNormals;
  matVertices.GetZeroEigenSpaceModifyMe(theNormals);
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
  tempCLS.theProjectivizedCone.CreateFromNormals(newNormals, &theGlobalVariables);
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
  if (!this->theProjectivizedCone.Vertices.LinSpanContainsVector(direction))
  { this->FindExtremaInDirectionOverLatticeOneNonParamDegenerateCase
      (theLPToMaximizeAffine, outputAppendLPToMaximizeAffine, outputAppend, theProjectionLatticeLevel,
       theGlobalVariables);
    return;
  }
  ProgressReport theReport(&theGlobalVariables);
  if (outputAppend.size>=10)
  { std::stringstream tempStream;
    tempStream << "<hr><hr><hr><hr>The bad cone:" << this->theProjectivizedCone.ToString(&theFormat);
    theReport.Report(tempStream.str());
  }
  ConeComplex complexBeforeProjection;
  complexBeforeProjection.init();
  complexBeforeProjection.AddNonRefinedChamberOnTopNoRepetition(this->theProjectivizedCone, theGlobalVariables);
  if (direction.ScalarEuclidean(theLPToMaximizeAffine).IsNegative())
    direction.Minus();
  complexBeforeProjection.slicingDirections.AddOnTop(direction);
  complexBeforeProjection.slicingDirections.AddOnTop(-direction);
//  std::cout << "<hr>complex before refining: <br>\n" << complexBeforeProjection.ToString(false, true);
  complexBeforeProjection.Refine(theGlobalVariables);
//  std::cout << "<hr>complex before projection: <br>\n" << complexBeforeProjection.ToString(false, true);
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
        //std::cout << "<br>currentWall: " << currentNormal.ToString();
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
    std::cout << "<hr><hr><hr>" << currentCone.ToString(&theFormat);
    std::cout << "<br>Entering normal: " << ((foundEnteringNormal) ? enteringNormalAffine.ToString() : "not found");
    std::cout << "<br>Exit normal: " << ((foundExitNormal) ? exitNormalAffine.ToString() : "not found");
    std::cout << "<br>The shifted lattice representatives: " << exitRepresentatives.ToString() << "<br>exitNormalsShiftedAffineProjected";
    assert(theNewNormals.size>0);
    for (int j=0; j<exitRepresentatives.size; j++)
    { tempCLS.theProjectivizedCone.Normals=theNewNormals;
      exitNormalShiftedAffineProjected=exitNormalAffine.GetShiftToTheLeftOnePosition();
      *exitNormalShiftedAffineProjected.LastObject()=-exitNormalLatticeLevel.ScalarEuclidean(exitRepresentatives[j]);
      std::cout << exitNormalShiftedAffineProjected.ToString() << ", ";
      if (foundEnteringNormal)
      { extraEquation= enteringNormalAffine.GetShiftToTheLeftOnePosition();
        extraEquation-=(exitNormalShiftedAffineProjected*enteringNormalAffine[0])/exitNormalAffine[0];
        std::cout << "extra equation: " << extraEquation.ToString() << ", ";
        tempCLS.theProjectivizedCone.Normals.AddOnTop(extraEquation);
      }
      Rational tempRat=theLPToMaximizeAffine[0];
      tempRoot=theLPToMaximizeAffine.GetShiftToTheLeftOnePosition();
      tempRoot-=exitNormalShiftedAffineProjected*theLPToMaximizeAffine[0]/exitNormalAffine[0];
      outputAppendLPToMaximizeAffine.AddOnTop(tempRoot);
      assert(tempCLS.theProjectivizedCone.Normals.size>0);
      Vectors<Rational> tempTempRoots=tempCLS.theProjectivizedCone.Normals;
      //bool tempBool=
      tempCLS.theProjectivizedCone.CreateFromNormals(tempTempRoots, &theGlobalVariables);
      /*if (!tempBool)
      { std::stringstream tempStream;
        tempStream << "The bad starting cone (cone number " << i+1 << "):" << this->ToString(theFormat) << "<hr><hr><hr><hr>The bad cone:" << tempCLS.ToString(theFormat);
        tempStream << "<br>\n\n" << this->theProjectivizedCone.Normals.ToString(false, false, false);
        tempStream << "\n\n<br>\n\n" << complexBeforeProjection.ToString(false, true);
        if (!foundEnteringNormal)
          tempStream << "<hr>not found entering normal!!!!!!<hr>";
        if (!foundExitNormal)
          tempStream << "<hr>not found exit normal!!!!!!<hr>";
        Cone tempCone;
        tempCone.CreateFromNormals(tempTempRoots, theGlobalVariables);
        tempStream << "\n\n\n\n<br><br><hr>The bad normals: " << tempTempRoots.ToString();
        tempStream << "\n\n\n\n<br><br><hr>The bad normals after creation: " << tempCLS.theProjectivizedCone.Normals.ToString();
        theGlobalVariables.theIndicatorVariables.StatusString1=tempStream.str();
        theGlobalVariables.theIndicatorVariables.StatusString1NeedsRefresh=true;
        theGlobalVariables.MakeReport();
        for (int i=0; i<10000000; i++)
          if (i%3==0)
            i=i+2;
        while(true){}
      }*/
      //assert(tempBool);
      //std::cout << tempCLS.theProjectivizedCone.ToString(false, true, true, true, theFormat);
      if (!tempCLS.theProjectivizedCone.flagIsTheZeroCone)
      { theProjectionLatticeLevel.ActOnVectorColumn(exitRepresentatives[j], tempCLS.theShift);
        outputAppend.AddOnTop(tempCLS);
        if (tempCLS.GetDimProjectivized()==0)
        { theReport.Report(tempTempRoots.ToString());
          while(true) {}
        }
        assert(tempCLS.GetDimProjectivized()==theDimProjectivized-1);
      }
    }
  }
//  std::cout << "<hr><hr><hr>";
//  for (int i=0; i<outputAppend.size; i++)
//  { std::cout << outputAppend[i].theProjectivizedCone.ToString(false, true, true, true);
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
  Matrix<Rational>& theLinearAlgebra=theGlobalVariables.matGetNewVerticesAppend.GetElement();
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
    theLinearAlgebra.GaussianEliminationByRows(0, &nonPivotPoints);
    if (nonPivotPoints.CardinalitySelection==1)
    { theLinearAlgebra.NonPivotPointsToEigenVector(nonPivotPoints, tempRoot, (Rational) 1, (Rational) 0);
      tempRoot.ScaleByPositiveRationalToIntegralMinHeight();
      if (myDyingCone.IsInCone(tempRoot))
        outputVertices.AddOnTopNoRepetition(tempRoot);
      else
      { tempRoot.Minus();
        if (myDyingCone.IsInCone(tempRoot))
          outputVertices.AddOnTopNoRepetition(tempRoot);
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
  (myDyingCone.Normals.GetRankOfSpanOfElements(&bufferMat, &bufferSel)<this->GetDim());
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
  { killerNormal.ScalarEuclidean(myDyingCone.Vertices[i], tempRat);
    if (tempRat.IsPositive())
      newPlusCone.Vertices.AddOnTop(myDyingCone.Vertices[i]);
    if (tempRat.IsEqualToZero())
      ZeroVertices.AddOnTopNoRepetition(myDyingCone.Vertices.TheObjects[i]);
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
  { newPlusCone.CreateFromNormals(newPlusCone.Normals, &theGlobalVariables);
    newMinusCone.CreateFromNormals(newMinusCone.Normals, &theGlobalVariables);
  }
  this->AddNonRefinedChamberOnTopNoRepetition(newPlusCone, theGlobalVariables);
  this->AddNonRefinedChamberOnTopNoRepetition(newMinusCone, theGlobalVariables);
  return true;
}

void ConeComplex::PopChamberSwapWithLast(int index)
{ this->RemoveIndexSwapWithLast(index);
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
  this->CreateFromVertices(newVertices, &theGlobalVariables);
  return true;
}

bool ConeComplex::AddNonRefinedChamberOnTopNoRepetition(const Cone& newCone, GlobalVariables& theGlobalVariables)
{ Cone theConeSorted;
  theConeSorted=newCone;
  theConeSorted.Normals.QuickSortAscending();
  this->ConvexHull.MakeConvexHullOfMeAnd(theConeSorted, theGlobalVariables);
  return this->AddOnTopNoRepetition(theConeSorted);
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
  startingCone.CreateFromVertices(inputVectors, &theGlobalVariables);
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
{ ProgressReport theReport(&theGlobalVariables);
  while (this->indexLowestNonRefinedChamber<this->size)
  { this->RefineOneStep(theGlobalVariables);
    std::stringstream out;
    out << "Refined " << this->indexLowestNonRefinedChamber << " out of " << this->size;
    theReport.Report(out.str());
  }
}

void Cone::ComputeVerticesFromNormalsNoFakeVertices(GlobalVariables* theGlobalVariables)
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
  Matrix<Rational> theMat;
  Vector<Rational> tempRoot;
  theMat.init(theDim-1, theDim);
  for (int i=0; i<numCycles; i++)
  { theSel.incrementSelectionFixedCardinality(theDim-1);
    for (int j=0; j<theSel.CardinalitySelection; j++)
      for (int k=0; k<theDim; k++)
        theMat.elements[j][k]=this->Normals.TheObjects[theSel.elements[j]].TheObjects[k];
    theMat.GaussianEliminationByRows(0, &nonPivotPoints);
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

bool Cone::EliminateFakeNormalsUsingVertices(int theDiM, int numAddedFakeWalls, GlobalVariables* theGlobalVariables)
{ if(this->Vertices.size==0)
  { this->flagIsTheZeroCone=true;
    this->Normals.SetSize(0);
    return false;
  }
  Vectors<Rational> verticesOnWall;
  if (numAddedFakeWalls!=0)
  { //we modify the normals so that they lie in the subspace spanned by the vertices
    Matrix<Rational> tempMat, matNormals, gramMatrixInverted;
    tempMat.AssignVectorsToRows(this->Vertices);
    Vectors<Rational> NormalsToSubspace;
    tempMat.GetZeroEigenSpaceModifyMe(NormalsToSubspace);
    if (NormalsToSubspace.size>0)
    { matNormals.AssignVectorsToRows(NormalsToSubspace);
//      std::cout << "<br>Normals to the subspace spanned by the vertices: " << NormalsToSubspace.ToString();
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
        { this->Normals.RemoveIndexSwapWithLast(i);
          i--;
        }
      }
      //all normals should now lie in the subspace spanned by the vertices
      //add the walls needed to go down to the subspace
      this->Normals.ReservE(this->Normals.size+2*NormalsToSubspace.size);
      for (int i=0; i<NormalsToSubspace.size; i++)
      { NormalsToSubspace[i].ScaleByPositiveRationalToIntegralMinHeight();
        this->Normals.AddOnTop(NormalsToSubspace[i]);
        this->Normals.AddOnTop(-NormalsToSubspace[i]);
      }
    }
  }
  Matrix<Rational> tempMatX;
  Selection tempSelX;
  int DesiredRank=this->Vertices.GetRankOfSpanOfElements(&tempMatX, &tempSelX);
  if (DesiredRank>1)
    for (int i=0; i<this->Normals.size; i++)
    { Vector<Rational>& currentNormal=this->Normals[i];
      verticesOnWall.size=0;
      bool wallIsGood=false;
      for (int j=0; j<this->Vertices.size; j++)
        if (currentNormal.ScalarEuclidean(this->Vertices[j]).IsEqualToZero())
        { verticesOnWall.AddOnTop(this->Vertices[j]);
          int theRank=verticesOnWall.GetRankOfSpanOfElements(&tempMatX, &tempSelX);
          if (theRank< verticesOnWall.size)
            verticesOnWall.RemoveLastObject();
          else
            if (theRank==DesiredRank-1)
            { wallIsGood=true;
              break;
            }
        }
      if (!wallIsGood)
      { this->Normals.RemoveIndexSwapWithLast(i);
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

bool Cone::ProduceNormalFromTwoNormalsAndSlicingDirection(Vector<Rational>& SlicingDirection, Vector<Rational>& normal1, Vector<Rational>& normal2, Vector<Rational>& output)
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

bool Cone::CreateFromVertices(const Vectors<Rational>& inputVertices, GlobalVariables* theGlobalVariables)
{ this->LowestIndexNotCheckedForChopping=0;
  this->LowestIndexNotCheckedForSlicingInDirection=0;
 // std::cout << inputVertices.ToString();
  this->flagIsTheZeroCone=false;
  if (inputVertices.size<=0)
  { this->Normals.size=0;
    this->Vertices.size=0;
    this->flagIsTheZeroCone=true;
    return false;
  }
  this->Normals.size=0;
  Matrix<Rational> tempMat;
  Selection tempSel;
  int rankVerticesSpan=inputVertices.GetRankOfSpanOfElements(&tempMat, &tempSel);
  int theDim=inputVertices.GetDim();
  Vectors<Rational> extraVertices;
  extraVertices.SetSize(0);
  if (rankVerticesSpan<theDim)
  { Matrix<Rational>  tempMat;
    tempMat.AssignVectorsToRows(inputVertices);
    tempMat.GetZeroEigenSpace(extraVertices);
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
//  std::cout << "<br>Candidate normals: " << this->Normals.ToString();
  return this->CreateFromNormals(this->Normals, theGlobalVariables);
}

bool Cone::CreateFromNormalS(Vectors<Rational>& inputNormals, bool UseWithExtremeMathCautionAssumeConeHasSufficientlyManyProjectiveVertices, GlobalVariables* theGlobalVariables)
{ this->flagIsTheZeroCone=false;
  this->LowestIndexNotCheckedForChopping=0;
  this->LowestIndexNotCheckedForSlicingInDirection=0;
  int theDim=1;
  if (inputNormals.size>0)
    theDim=inputNormals[0].size;
  this->Normals=inputNormals;
  for (int i=0; i<this->Normals.size; i++)
    if (this->Normals[i].IsEqualToZero())
    { this->Normals.RemoveIndexSwapWithLast(i);
      i--;
    }
  int numAddedFakeWalls=0;
  Matrix<Rational> tempMat;
  Selection tempSel;
  if (!UseWithExtremeMathCautionAssumeConeHasSufficientlyManyProjectiveVertices)
    for (int i=0; i<theDim && this->Normals.GetRankOfSpanOfElements(&tempMat, &tempSel)<theDim; i++)
    { Vector<Rational> tempRoot;
      tempRoot.MakeEi(theDim, i);
      if(!this->Normals.LinSpanContainsVector(tempRoot, tempMat, tempSel))
      { numAddedFakeWalls++;
        this->Normals.AddOnTop(tempRoot);
      }
    }
//  std::cout << "<br>Normals (" << inputNormals.size << " input " << numAddedFakeWalls << " fake): " << this->Normals.ToString();
  this->ComputeVerticesFromNormalsNoFakeVertices(theGlobalVariables);
//  std::cout << "<br>Vertices before adding minus vertices: " << this->Vertices.ToString();
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
//  std::cout << "<br>Vertices: " << this->Vertices.ToString();
  return this->EliminateFakeNormalsUsingVertices(theDim, numAddedFakeWalls, theGlobalVariables);
}

void ConeComplex::initFromCones(List<Cone>& NormalsOfCones, bool AssumeConesHaveSufficientlyManyProjectiveVertices, GlobalVariables& theGlobalVariables)
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
  ProgressReport theReport(&theGlobalVariables);
  theReport.Report(NormalsOfCones.ToString());
//  for (int i=0; i<10000000; i++){int j=i*i*i;}
  for (int i=0; i<NormalsOfCones.size; i++)
  { if (tempCone.CreateFromNormalS(NormalsOfCones[i], UseWithExtremeMathCautionAssumeConeHasSufficientlyManyProjectiveVertices, &theGlobalVariables))
      this->AddNonRefinedChamberOnTopNoRepetition(tempCone, theGlobalVariables);
    std::stringstream out;
    out << "Initializing cone " << i+1 << " out of " << NormalsOfCones.size;
    theReport.Report(out.str());
  }
  Vector<Rational> tempRoot;
  this->splittingNormals.Clear();
  for (int i=0; i<this->size; i++)
    for (int j=0; j<this->TheObjects[i].Normals.size; j++)
    { tempRoot=this->TheObjects[i].Normals[j];
      tempRoot.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
      this->splittingNormals.AddOnTopNoRepetition(tempRoot);
      std::stringstream out;
      out << "Extracting walls from cone " << i+1 << " out of " << this->size << " total distinct chambers.";
      out << "\nProcessed " << j+1 << " out of " << this->TheObjects[i].Normals.size << " walls of the current chamber.";
      out << "\nTotal # of distinct walls found: " << this->splittingNormals.size;
      theReport.Report(out.str());
    }
}

std::string Cone::ToString(FormatExpressions* theFormat)const
{ std::stringstream out;
  bool PrepareMathReport= theFormat==0 ? false: theFormat->flagUseLatex;
  bool useHtml= theFormat==0 ? false: theFormat->flagUseHTML;
  bool useLatex= theFormat==0 ? false: theFormat->flagUseLatex;
  bool lastVarIsConstant=false;
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
  FormatExpressions tempF;
  if (theFormat==0)
    theFormat=&tempF;
  out << this->Normals.ElementsToInequalitiesString(useLatex, useHtml, lastVarIsConstant, *theFormat);
  if (useLatex)
    out << "\\]";
  out << "\nProjectivized Vertices: " << this->Vertices.ToString();
  if (useHtml)
    out << "<br>";
  if (this->Vertices.size>0)
    out << "\nInternal point: " << this->GetInternalPoint().ToString();
  return out.str();
}

std::string ConeComplex::ToString(bool useLatex, bool useHtml)
{ std::stringstream out;
  FormatExpressions theFormat;
  out << "Number of chambers: " << this->size;
  if (useHtml)
    out << "<br>";
  out << " Next non-refined chamber: " << this->indexLowestNonRefinedChamber+1;
  if (useHtml)
    out << "<br>";
  out << "Normals of walls to refine by: ";
  Vectors<Rational> tempRoots;
  tempRoots=(this->splittingNormals);
  out << tempRoots.ToString(&theFormat);
  if (this->slicingDirections.size>0)
  { if (useHtml)
      out << "<br>\n";
    out << " Directions to slice along: " << this->slicingDirections.ToString();
  }
  for (int i=0; i<this->size; i++)
  { if (useHtml)
      out << "<hr>";
    out << "\n\n\nChamber " << i+1 << ":\n";
    if (useHtml)
      out << "<br>";
    out << this->TheObjects[i].ToString(&theFormat) << "\n\n\n";
  }
  return out.str();
}

int RationalFunctionOld::GetMinNumVars()const
{ switch(this->expressionType)
  { case RationalFunctionOld::typeRational:
      return 0;
    case RationalFunctionOld::typePoly:
      return this->Numerator.GetElementConst().GetMinNumVars();
    case RationalFunctionOld::typeRationalFunction:
      return MathRoutines::Maximum(this->Numerator.GetElementConst().GetMinNumVars(), this->Denominator.GetElementConst().GetMinNumVars());
    default: //this should never happen! maybe assert(false) here...
      return -1;
  }
}

void RationalFunctionOld::GetRelations(List<Polynomial<Rational> >& theGenerators, GlobalVariables& theGlobalVariables)
{ if (theGenerators.size==0)
    return;
  List<Polynomial<Rational> > theGroebnerBasis;
  theGroebnerBasis=theGenerators;
  int numStartingGenerators=theGenerators.size;

  int numStartingVariables=0;
  for (int i=0; i<theGenerators.size; i++)
    numStartingVariables=MathRoutines::Maximum(numStartingVariables, theGenerators[0].GetMinNumVars());
  Polynomial<Rational> tempP;
  FormatExpressions tempFormat;
  for (int i=0; i<numStartingGenerators; i++)
  { Polynomial<Rational>& currentPoly=theGroebnerBasis[i];
    currentPoly.SetNumVariablesSubDeletedVarsByOne(numStartingVariables+numStartingGenerators);
    tempP.MakeDegreeOne(numStartingVariables+numStartingGenerators, i+numStartingVariables, (Rational) -1);
    currentPoly+=tempP;
//  std::cout << currentPoly.ToString(false, tempFormat) << "<br>";
  }
  GroebnerBasisComputation<Rational> theComputation;
  theComputation.theMonOrdeR=MonomialP::LeftIsGEQLexicographicLastVariableWeakest;
  theComputation.TransformToReducedGroebnerBasis(theGroebnerBasis, &theGlobalVariables);
//  std::cout << "<br>the ending generators:<br> ";
//  for (int i=0; i<theGroebnerBasis.size; i++)
//  { std::cout << theGroebnerBasis.TheObjects[i].ToString(false, tempFormat) << "<br>";
//  }
  theGenerators.ReservE(theGroebnerBasis.size);
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

bool ConeComplex::findMaxLFOverConeProjective
  (const Cone& input, List<Polynomial<Rational> >& inputLinPolys, List<int>& outputMaximumOverEeachSubChamber, GlobalVariables& theGlobalVariables)
{ Vectors<Rational> HyperPlanesCorrespondingToLF;
  if (input.Normals.size<1 || inputLinPolys.size<1)
    return false;
  int theDim=input.Normals[0].size;
  HyperPlanesCorrespondingToLF.SetSize(inputLinPolys.size);
  for (int i=0; i<inputLinPolys.size; i++)
  { Polynomial<Rational>& currentPoly=inputLinPolys[i];
    if (currentPoly.TotalDegree()!=1 )
    { std::cout << "You messed up the total degree which must be one, instead it is "
      << currentPoly.TotalDegree() << ". The dimension of the cone is " << theDim;
      return false;
    }
    Vector<Rational>& newWall=HyperPlanesCorrespondingToLF.TheObjects[i];
    newWall.MakeZero(theDim);
    for (int j=0; j<currentPoly.size(); j++)
      for (int k=0; k<theDim; k++)
        if (currentPoly[j](k)==1)
        { newWall[k]=currentPoly.theCoeffs[j];
          break;
        }
  }
  return this->findMaxLFOverConeProjective
  (input, HyperPlanesCorrespondingToLF, outputMaximumOverEeachSubChamber, theGlobalVariables);
}

bool ConeComplex::findMaxLFOverConeProjective
  (const Cone& input, Vectors<Rational>& inputLFsLastCoordConst,
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
        this->splittingNormals.AddOnTopNoRepetition(tempRoot);
    }
  std::cout << this->ToString(false, true);
  this->Refine(theGlobalVariables);
  outputMaximumOverEeachSubChamber.SetSize(this->size);
  Rational theMax=0;
  for (int i=0; i<this->size; i++)
  { this->TheObjects[i].GetInternalPoint(tempRoot);
    bool isInitialized=false;
    for (int j=0; j<inputLFsLastCoordConst.size; j++)
      if (!isInitialized || tempRoot.ScalarEuclidean(inputLFsLastCoordConst[j])>theMax)
      { theMax=tempRoot.ScalarEuclidean(inputLFsLastCoordConst[j]);
        outputMaximumOverEeachSubChamber[i]=j;
        isInitialized=true;
      }
  }
  for (int i=0; i<this->size; i++)
  { //std::cout << "<br>Chamber " << i+1 << " maximum linear function is the function of index " << outputMaximumOverEeachSubChamber.TheObjects[i] << ": " << inputLinPolys.TheObjects[outputMaximumOverEeachSubChamber.TheObjects[i]].ToString();
    //std::cout << "<br>The chamber is given by: " << this->TheObjects[i].ToString(false, true);
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
  this->basis.GaussianEliminationEuclideanDomain();
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
//    std::cout << "<br>basis rational form: " << this->basisRationalForm.ToString(true, false);
//    out << "<br>basis rational form: " << this->basisRationalForm.ToString(true, false);
//    std::string tempS=out.str();
//    assert(testMatrix==this->basisRationalForm);
//  }
}

void Lattice::TestGaussianEliminationEuclideanDomainRationals(Matrix<Rational> & output)
{ output.AssignMatrixIntWithDen(this->basis, this->Den);
  std::stringstream out;
  std::cout << "Test output: " << output.ToString();
  out << "Test output: " << output.ToString();
  output.GaussianEliminationEuclideanDomain();
  std::cout << "<br>After gaussian elimination:" << output.ToString();
  out << "<br>After gaussian elimination:" << output.ToString();
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

void Lattice::MakeFromMat(const Matrix<Rational> & input)
{ this->basisRationalForm=input;
  this->basisRationalForm.GetMatrixIntWithDen(this->basis, this->Den);
  this->Reduce();
}

void Lattice::MakeFromRoots(const Vectors<Rational>& input)
{ Matrix<Rational> tempMat;
  tempMat.AssignVectorsToRows(input);
  tempMat.GetMatrixIntWithDen(this->basis, this->Den);
  this->Reduce();
}

std::string Lattice::ToString(bool useHtml, bool useLatex)const
{ std::stringstream out;
  out << "L=<";
  Vectors<Rational> tempRoots;
  tempRoots.AssignMatrixRows(this->basisRationalForm);
  for (int i=0; i<this->basisRationalForm.NumRows; i++)
  { out << tempRoots.TheObjects[i].ToString();
    if (i!=this->basisRationalForm.NumRows-1)
      out << ",";
  }
  out << ">";
  return out.str();
}

bool LargeIntUnsigned::IsEven()const
{ //std::cout << "<br>remainder by 2 is " << ((*this)%2).ToString()
  //<< " and ((*this)%2)==0 is " << (((*this)%2)==0);
  return ((*this)%2)==0;
}

bool LargeIntUnsigned::operator==(const LargeIntUnsigned& other)const
{ return this->theDigits==other.theDigits;
}

bool LargeIntUnsigned::operator!=(const LargeIntUnsigned& other)const
{ return ! ((*this)==other);
}

bool LargeInt::IsEven()const
{ return this->value.IsEven();
}

#ifdef WIN32
#pragma warning(disable:4244)//warning 4244: data loss from conversion
#endif
void LargeIntUnsigned::DivPositive(const LargeIntUnsigned& x, LargeIntUnsigned& quotientOutput, LargeIntUnsigned& remainderOutput) const
{ if (x.theDigits.size==1 && this->theDigits.size==1)
  { quotientOutput.AssignShiftedUInt(this->theDigits[0]/x.theDigits[0], 0);
    remainderOutput.AssignShiftedUInt(this->theDigits[0]%x.theDigits[0], 0);
//    std::cout << "Dividing " << this->ToString() << " by " << x.ToString() << " yields quotient " << quotientOutput.ToString()
//    << " and remainder " << remainderOutput.ToString();
    return;
  }
  LargeIntUnsigned remainder, quotient, divisor;
  remainder=(*this);
  divisor=(x);
  assert(!divisor.IsEqualToZero());
  quotient.MakeZero();
  //std::string tempS1, tempS2, tempS3;
  while (remainder.IsGEQ(divisor))
  { unsigned int q;
    LargeIntUnsigned current, Total;
    if (*remainder.theDigits.LastObject()>*divisor.theDigits.LastObject())
    { q=*remainder.theDigits.LastObject()/(*divisor.theDigits.LastObject()+1);
      current.AssignShiftedUInt(q, remainder.theDigits.size-divisor.theDigits.size);
    }
    else
    { if (remainder.theDigits.size==divisor.theDigits.size)
        current.AssignShiftedUInt(1, 0);
      else
      { q=this->CarryOverBound/(divisor.theDigits[divisor.theDigits.size-1]+1);
        current.AssignShiftedUInt(q, remainder.theDigits.size- divisor.theDigits.size-1);
        current.MultiplyByUInt(remainder.theDigits[remainder.theDigits.size-1]);
      }
    }
    Total=(divisor);
    Total.MultiplyBy(current);
    //if (!remainder.IsGEQ(Total))
    //{ tempS1= remainder.ToString();
    //  tempS2=Total.ToString();
    //  remainder.IsGEQ(Total);
    //}
    remainder.SubtractSmallerPositive(Total);
    quotient+=(current);
  }
  remainderOutput=(remainder);
  quotientOutput=(quotient);
//  std::cout << "Dividing " << this->ToString() << " by " << x.ToString() << " yields quotient " << quotientOutput.ToString()
//  << " and remainder " << remainderOutput.ToString();
//  assert(remainderOut.CheckForConsistensy());
}
#ifdef WIN32
#pragma warning(default:4244)//warning 4244: data loss from conversion
#endif

//returning false means that the lattice given as rougher is not actually rougher than the current lattice
//or that there are too many representatives
bool Lattice::GetAllRepresentatives(const Lattice& rougherLattice, Vectors<Rational>& output)const
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
    currentPeriod.GetNumerator(currentPeriodInt);
    if (currentPeriodInt.value.theDigits.size>1)
//    { std::cout << "exited at point 1 counter i is " << i;
      return false;
//    }
    else
      thePeriods[i]=currentPeriodInt.value.theDigits[0];
    this->basisRationalForm.GetVectorFromRow(i, thePeriodVectors[i]);
    rougherLattice.basisRationalForm.GetVectorFromRow(i, tempRoot2);
    tempRoot=thePeriodVectors[i];
    tempRoot*=thePeriods[i];

//    if (!(tempRoot-tempRoot2).IsEqualToZero())
//    { std::cout << "exited at point 2 counter i is " << i << " the period vector=" << thePeriodVectors.TheObjects[i].ToString() << " rougher lattice basis vector: " << tempRoot2.ToString();
//      return false;
//    }
  }
//  std::cout << thePeriodVectors.ToString() << "<br>The periods: ";
//  for (int i=0; i<thePeriods.size; i++)
//    std::cout << thePeriods.TheObjects[i] << ", ";
  for (int i=0; i<thePeriods.size; i++)
    thePeriods[i]--;
  SelectionWithDifferentMaxMultiplicities theCoeffSelection;
  theCoeffSelection.initFromInts(thePeriods);
  int NumCycles=theCoeffSelection.getTotalNumSubsets();
  output.SetSize(NumCycles);
  for (int i=0; i<NumCycles; i++, theCoeffSelection.IncrementSubset())
  { output[i].MakeZero(theDim);
    for (int j=0; j<theCoeffSelection.Multiplicities.size; j++)
      output[i]+=thePeriodVectors[j]*theCoeffSelection.Multiplicities[j];
  }
//  std::cout << "The representatives: " << output.ToString(false, true, false);
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
  std::cout << "<br>the movement: " << theMovement.ToString() << ", (" << theShift.ToString() << " - " << startingPoint.ScalarEuclidean(theNormal).ToString() << ")/ " << theDirection.ScalarEuclidean(theNormal).ToString() << ", ";
  if (!theMovement.IsInteger())
  { std::cout << "the movement aint integral; ";
    theMovement.AssignFloor();
    if (theDirection.ScalarEuclidean(theNormal).IsPositive())
      theMovement+=1;
  }
  std::cout << "the normal: " << theNormal.ToString() << ", the direction: " << theDirection.ToString() << ", the shift: " << theShift.ToString()
  << ", the movement: " << theMovement.ToString() << ", startingPoint: " << startingPoint.ToString();
  outputPoint=startingPoint;
  outputPoint+=(theDirection*theMovement);
  std::cout << ", finalPoint: " << outputPoint.ToString();
  return true;
}

bool Lattice::GetAllRepresentativesProjectingDownTo(const Lattice& rougherLattice, Vectors<Rational>& startingShifts, Vectors<Rational>& output)const
{ Vectors<Rational> tempRepresentatives;
  if (!this->GetAllRepresentatives(rougherLattice, tempRepresentatives))
    return false;
  output.SetExpectedSize(startingShifts.size*tempRepresentatives.size);
  output.size=0;
  for (int i=0; i<startingShifts.size; i++)
    for (int j=0; j<tempRepresentatives.size; j++)
      output.AddOnTop(startingShifts.TheObjects[i]+tempRepresentatives[j]);
  return true;
}

void QuasiPolynomial::MakeRougherLattice(const Lattice& latticeToRoughenBy)
{ if (this->AmbientLatticeReduced==latticeToRoughenBy)
    return;
  //std::cout << "roughening " << this->AmbientLatticeReduced.ToString() << "by" << latticeToRoughenBy.ToString();
  Lattice OldLattice;
  OldLattice=this->AmbientLatticeReduced;
  this->AmbientLatticeReduced.IntersectWith(latticeToRoughenBy);
  Vectors<Rational> representativesQuotientLattice;
  //std::cout << "getting all representatives of " << OldLattice.ToString() << "inside" << this->AmbientLatticeReduced.ToString();
  //std::cout << "<br><br><br><br>*********<br><br><br><br>";
  OldLattice.GetAllRepresentatives(this->AmbientLatticeReduced, representativesQuotientLattice);
  Vectors<Rational> OldLatticeShifts=this->LatticeShifts;
  List<Polynomial<Rational> > oldValues;
  oldValues=this->valueOnEachLatticeShift;
  this->LatticeShifts.SetSize(OldLatticeShifts.size*representativesQuotientLattice.size);
  this->valueOnEachLatticeShift.SetSize(this->LatticeShifts.size);
  for (int i=0; i<OldLatticeShifts.size; i++)
    for (int j=0; j<representativesQuotientLattice.size; j++)
    { this->LatticeShifts[i*representativesQuotientLattice.size+j]=OldLatticeShifts[i]+representativesQuotientLattice[j];
      this->valueOnEachLatticeShift[i*representativesQuotientLattice.size+j]=oldValues[i];
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
  //std::cout << "<hr>acting by " << this->basisRationalForm.ToString(true, false) << " on " << output.ToString();
  Matrix<Rational> tempMat;
  tempMat=this->basisRationalForm;
  tempMat.Transpose();
  tempMat.ActOnVectorsColumn(output);
  //std::cout << "<br> to get " << output.ToString();
  return result;
}

void Lattice::IntersectWithPreimageOfLattice(const Matrix<Rational> & theLinearMap, const Lattice& other, GlobalVariables& theGlobalVariables)
{ Vectors<Rational> startingBasis, imageStartingBasis, basisImageIntersection, basisImageIntersectionInCoordsWRTimageStartingBasis, ImageBasisInImageStartingBasisCoords;
  Vectors<Rational> resultNonKernelPart, resultKernelPart, result, tempRoots;
  startingBasis.AssignMatrixRows(this->basisRationalForm);
  //std::cout << "<br>Starting basis: " << startingBasis.ToString();
  //std::cout << "<br>The linear map: " << theLinearMap.ToString(true, false);
  theLinearMap.ActOnVectorsColumn(startingBasis, imageStartingBasis);
  //std::cout << "<br>Image of starting basis: " << imageStartingBasis.ToString();
  Lattice ImageLattice;
  ImageLattice.MakeFromRoots(imageStartingBasis);
  ImageLattice.IntersectWith(other);
  //std::cout << "<br>Image lattice: " << ImageLattice.ToString(true, false);
  basisImageIntersection.AssignMatrixRows(ImageLattice.basisRationalForm);
  Vectors<Rational> tempBasisImageIntersection, tempImageStartingBasis, tempImageBasisInImageStartingBasisCoords;
  basisImageIntersection=(tempBasisImageIntersection);
  imageStartingBasis=(tempImageStartingBasis);
  bool tempBool= tempBasisImageIntersection.GetIntegralCoordsInBasisIfTheyExist(tempImageStartingBasis, tempImageBasisInImageStartingBasisCoords, (Rational) 1, (Rational) -1, (Rational) 0);
  ImageBasisInImageStartingBasisCoords=(tempImageBasisInImageStartingBasisCoords);
  assert(tempBool);
  //std::cout << "<br> Basis of image lattice expressed in coordinates with respect to image of starting basis: " << ImageBasisInImageStartingBasisCoords.ToString();
  resultNonKernelPart.SetSize(ImageBasisInImageStartingBasisCoords.size);
  for (int i=0; i<resultNonKernelPart.size; i++)
  { Vector<Rational>& currentRoot=resultNonKernelPart[i];
    currentRoot.MakeZero(this->GetDim());
    for (int j=0; j<startingBasis.size; j++)
      currentRoot+=startingBasis[j]*ImageBasisInImageStartingBasisCoords[i][j];
  }
  //std::cout << "<br> Result non-kernel part (representatives): " << resultNonKernelPart.ToString();
  Lattice KernelPart;
  KernelPart=*this;
  tempRoots.AssignMatrixRows(theLinearMap);
  KernelPart.IntersectWithLinearSubspaceGivenByNormals(tempRoots);
  //std::cout << "<br>Kernel part of the result: " << KernelPart.ToString(true, false);
  result.AssignMatrixRows(KernelPart.basisRationalForm);
  result.AddListOnTop(resultNonKernelPart);
  this->MakeFromRoots(result);
  //std::cout << "<br> And the result is.... " << this->ToString(true, false);
}

void Lattice::IntersectWith(const Lattice& other)
{ Vectors<Rational> commonBasis, otherBasis, startBasis;
  Vectors<Rational> bufferVect;
  Matrix<Rational> bufferMat;
  startBasis.AssignMatrixRows(this->basisRationalForm);
  otherBasis.AssignMatrixRows(other.basisRationalForm);
//  std::cout << "<br>this basis: " << startBasis.ToString();
//  std::cout << "<br>other basis: " << otherBasis.ToString();
  startBasis.IntersectTwoLinSpaces(startBasis, otherBasis, commonBasis);
//  std::cout << "<br> basis of linear space intersection: " << commonBasis.ToString() << "<br><br> ";
  Lattice thisLatticeIntersected, otherLatticeIntersected;
  thisLatticeIntersected=*this;
  otherLatticeIntersected=other;
  thisLatticeIntersected.IntersectWithLinearSubspaceSpannedBy(commonBasis);
//  std::cout << "<br> linear space intersected with this lattice: " << thisLatticeIntersected.ToString(true, false);
  otherLatticeIntersected.IntersectWithLinearSubspaceSpannedBy(commonBasis);
//  std::cout << "<br> linear space intersected with other lattice: " << otherLatticeIntersected.ToString(true, false);
  Vectors<Rational> thisCommonBasis, otherCommonBasis, thisCommonCoords, otherCommonCoords;
  thisCommonBasis.AssignMatrixRows(thisLatticeIntersected.basisRationalForm);
  otherCommonBasis.AssignMatrixRows(otherLatticeIntersected.basisRationalForm);
  thisCommonBasis.GetCoordsInBasis(commonBasis, thisCommonCoords, bufferVect, bufferMat);
  otherCommonBasis.GetCoordsInBasis(commonBasis, otherCommonCoords, bufferVect, bufferMat);
//  std::cout << "<br>this lattice intersection new coords: " << thisCommonBasis.ToString();
//  std::cout << "<br>other lattice intersection new coords: " << otherBasis.ToString();
  Lattice thisCommonCoordsLattice, otherCommonCoordsLattice;
  thisCommonCoordsLattice.MakeFromRoots(thisCommonCoords);
  otherCommonCoordsLattice.MakeFromRoots(otherCommonCoords);
//  std::cout << "<br> linear space intersected with this lattice new coords: " << thisCommonCoordsLattice.ToString(true, false);
//  std::cout << "<br> linear space intersected with other lattice new coords: " << otherCommonCoordsLattice.ToString(true, false);
  thisCommonCoordsLattice.IntersectWithBothOfMaxRank(otherCommonCoordsLattice);
//  std::cout << "<br> intersection lattice new coords: " << thisCommonCoordsLattice.ToString(true, false);
  Vectors<Rational> resultBasis;
  resultBasis.SetSize(thisCommonCoordsLattice.basisRationalForm.NumRows);
  for (int i=0; i<resultBasis.size; i++)
  { Vector<Rational>& currentRoot=resultBasis.TheObjects[i];
    currentRoot.MakeZero(this->GetDim());
    for (int j=0; j<thisCommonCoordsLattice.basisRationalForm.NumCols; j++)
      currentRoot+=commonBasis[j]*thisCommonCoordsLattice.basisRationalForm.elements[i][j];
  }
  this->MakeFromRoots(resultBasis);
//  std::cout << "<br>final answer: intersection is: " << this->ToString(true, false);
}

void Lattice::IntersectWithBothOfMaxRank(const Lattice& other)
{ Lattice dualLatticeThis, dualLatticeOther;
  //std::cout << "intersecting " << this->ToString() << " and " << other.ToString();
  assert(this->basis.NumRows==this->GetDim() && this->GetDim()==other.GetDim() && other.basis.NumRows== this->GetDim());
  this->GetDualLattice(dualLatticeThis);
  //std::cout << "<br>dual lattice of left: " << dualLatticeThis.ToString();
  other.GetDualLattice(dualLatticeOther);
  //std::cout << "<br>dual lattice of right: " << dualLatticeOther.ToString();
  dualLatticeThis.RefineByOtherLattice(dualLatticeOther);
  //std::cout << "<br> common refinement of dual lattice: " << dualLatticeThis.ToString();
  dualLatticeThis.GetDualLattice(*this);
  //std::cout << "<br>final result: " << this->ToString();
}

void QuasiPolynomial::operator+=(const QuasiPolynomial& other)
{ //std::cout << "current ambient lattice: " << this->AmbientLatticeReduced.ToString();
  this->MakeRougherLattice(other.AmbientLatticeReduced);
  //std::cout << "roughened: " << this->AmbientLatticeReduced.ToString();
  QuasiPolynomial tempQP=other;
  //std::cout << "<br><br>other ambient lattice: " << this->AmbientLatticeReduced.ToString();
  tempQP.MakeRougherLattice(this->AmbientLatticeReduced);
  //std::cout << "roughened: " << this->AmbientLatticeReduced.ToString() << "<br><br><br><br>*******<br><br><br>";
  for(int i=0; i<tempQP.LatticeShifts.size; i++)
    this->AddLatticeShift(tempQP.valueOnEachLatticeShift[i], tempQP.LatticeShifts[i]);
}

std::string QuasiPolynomial::ToString(bool useHtml, bool useLatex, FormatExpressions* thePolyFormat)
{ std::stringstream out;
  //if (useHtml)
//  out << "the lattice: " << this->AmbientLatticeReduced.ToString(useHtml, useLatex);
//  out << "<br>We have " << this->LatticeShifts.size << " lattice shifts. The polynomial on each lattice shift follows.";
  if (this->LatticeShifts.size==0)
    return "0";
  if (useLatex&& !useHtml)
    out << "\\begin{tabular}{c}";
  if (useLatex&& useHtml)
    out << "\\begin{array}{rcl}&&";
  for (int i=0; i<this->LatticeShifts.size; i++)
  { //if(useHtml)
      //out << "<br>Shift: " << this->LatticeShifts.TheObjects[i].ToString() << "; polynomial: ";
    if (useLatex)
    { if(!useHtml)
        out << "$\\begin{array}{rcl}&&";
      else
        out << "<span class=\"math\">";
    }
    out << this->valueOnEachLatticeShift[i].ToString(thePolyFormat);
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
    if (!this->LatticeShifts[i].IsEqualToZero())
      out << this->LatticeShifts[i].ToString() << " + ";
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
    { out << tempRoots[i].ToString();
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
        out << ", where $\\Lambda=\\mathbb{Z}^{" << this->GetNumVars() << "}$";
      else
        out << ", \\mathrm{~where~} \\Lambda=\\mathbb{Z}^{" << this->GetNumVars() << "}";
    }
    else
      out << "Z^" <<  this->GetNumVars();
  if (useLatex&& !useHtml)
    out << "\\end{tabular}";
  if (useLatex&& useHtml)
    out << "\\end{array}";
  return out.str();
}

void QuasiPolynomial::MakeFromPolyShiftAndLattice
(const Polynomial<Rational>& inputPoly, const MonomialP& theShift, const Lattice& theLattice, GlobalVariables& theGlobalVariables)
{ this->AmbientLatticeReduced=theLattice;
  this->LatticeShifts.SetSize(1);
  Vector<Rational>& firstShift=this->LatticeShifts[0];
  firstShift.SetSize(theLattice.GetDim());
  for (int i=0; i<theLattice.GetDim(); i++)
    firstShift[i]=theShift(i);
  this->AmbientLatticeReduced.ReduceVector(this->LatticeShifts[0]);
  this->valueOnEachLatticeShift.SetSize(1);
  this->valueOnEachLatticeShift[0]=inputPoly;
}

bool Lattice::ReduceVector(Vector<Rational>& theVector)const
{ Vector<Rational> output;
  assert(theVector.size==this->GetDim());
  Vectors<Rational> basisRoots;
  basisRoots.AssignMatrixRows(this->basisRationalForm);
  //std::cout <<  "the basis: " << basisRoots.ToString();
  if (!theVector.GetCoordsInBasiS(basisRoots, output))
  { std::cout << "oops bad!";
    assert(false);
    return false;
  }
  for (int i=0; i<output.size; i++)
    output[i].AssignFracValue();
  theVector.MakeZero(theVector.size, (Rational) 0);
  for (int i=0; i<basisRoots.size; i++)
    theVector+=basisRoots[i]*output[i];
  //std::cout << "the vector " << theVector.ToString() << " in the basis " << basisRoots.ToString() << " has coordinates: " << output.ToString();
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

void PartFraction::GetRootsFromDenominator(PartFractions& owner, Vectors<Rational>& output)const
{ output.SetSize(this->IndicesNonZeroMults.size);
  for (int i=0; i<this->IndicesNonZeroMults.size; i++)
  { output[i]=owner.startingVectors[this->IndicesNonZeroMults[i]];
    oneFracWithMultiplicitiesAndElongations& current=this->TheObjects[this->IndicesNonZeroMults[i]];
    assert(current.Elongations.size==1);
    output[i]*=current.Elongations[0];
  }
}

void PartFraction::ComputePolyCorrespondingToOneMonomial
  (QuasiPolynomial& outputQP, const MonomialP& theMon, Vectors<Rational>& normals, Lattice& theLattice, GlobalVariables& theGlobalVariables)const
{ Polynomial<Rational> tempP, outputPolyPart;
  Rational tempRat2;
  outputPolyPart.MakeOne(theMon.GetMinNumVars());
  for (int i=0; i<theMon.GetMinNumVars(); i++)
  { this->MakePolynomialFromOneNormal(normals[i], theMon, this->TheObjects[this->IndicesNonZeroMults[i]].Multiplicities[0], tempP);
    outputPolyPart*=(tempP);
  }
  outputQP.MakeFromPolyShiftAndLattice(outputPolyPart, theMon, theLattice, theGlobalVariables);
}

void PartFraction::GetVectorPartitionFunction
  (PartFractions& owner, Polynomial<LargeInt>& theCoeff, QuasiPolynomial& output, GlobalVariables& theGlobalVariables)const
{ QuasiPolynomial shiftedPoly;
  Vectors<Rational> theNormals, theLatticeGenerators;
  if (PartFraction::MakingConsistencyCheck)
    PartFraction::CheckSum.MakeZero();
  this->GetRootsFromDenominator(owner, theLatticeGenerators);
  Lattice theLattice;
  theLattice.MakeFromRoots(theLatticeGenerators);
//  std::cout << "<hr><hr> the lattice generators: " << theLatticeGenerators.ToString();
//  std::cout << "<br>Corresponding lattice: " << theLattice.ToString();
  Matrix<Rational>  theNormalsMatForm;
  theNormalsMatForm.AssignVectorsToRows(theLatticeGenerators);
  theNormalsMatForm.Invert();
  theNormals.AssignMatrixColumns(theNormalsMatForm);
  output.MakeZeroLatTiceZn(owner.AmbientDimension);

  for (int i=0; i<theCoeff.size(); i++)
  { this->ComputePolyCorrespondingToOneMonomial(shiftedPoly, theCoeff[i], theNormals, theLattice, theGlobalVariables);
//    output.ComputeDebugString();
//    shiftedPoly.ComputeDebugString();
//    std::cout << "<hr>Current fraction monomial " << i+1 << " out of " << this->Coefficient.size;
//    std::cout << "<br>Contribution: " << shiftedPoly.ToString(true, false);
    shiftedPoly*=theCoeff.theCoeffs[i];
    output+=shiftedPoly;
//    std::cout << "<br>Accumulator: " << output.ToString(true, false);
//    if (RecordNumMonomials)
//    { std::stringstream out4, out3;
//      out4 << "Current fraction: " << i+1<< " out of " << this->Coefficient.size << " processed";
//      PartFractions::NumProcessedForVPFMonomialsTotal++;
//      out3  << " Processed " << PartFractions::NumProcessedForVPFMonomialsTotal << " out of " << PartFractions::NumMonomialsInNumeratorsRelevantFractions << " relevant monomials";
//      theGlobalVariables.theIndicatorVariables.ProgressReportStrings[3]= out4.str();
//      theGlobalVariables.theIndicatorVariables.ProgressReportStrings[2]= out3.str();
//      theGlobalVariables.FeedIndicatorWindow(theGlobalVariables.theIndicatorVariables);
//    }
  }
//  if (PartFraction::MakingConsistencyCheck)
//  { Rational tempLRat;
//    output.Evaluate(PartFraction::theVectorToBePartitioned, tempLRat);
//    assert(tempLRat.DenShort==1);
//    assert(tempLRat.IsEqualTo(PartFraction::CheckSum));
//  }
//  if (StoreToFile)
//  { this->FileStoragePosition= PartFraction::TheBigDump.tellp();
//    output.WriteToFile(PartFraction::TheBigDump);
//    PartFractions::ComputedContributionsList.flush();
//  }

//  Accum.ComputeDebugString();
}

bool PartFractions::GetVectorPartitionFunction(QuasiPolynomial& output, Vector<Rational>& newIndicator, GlobalVariables& theGlobalVariables)
{ ProgressReport theReport(&theGlobalVariables);
  if(this->AssureIndicatorRegularity(theGlobalVariables, newIndicator))
  { theGlobalVariables.theIndicatorVariables.modifiedRoot=(newIndicator);
    theReport.Report("Indicator modified to regular");
  } else
    theGlobalVariables.theIndicatorVariables.flagRootIsModified=false;
  this->ResetRelevanceIsComputed();
  if (!this->CheckForMinimalityDecompositionWithRespectToRoot(&newIndicator, theGlobalVariables))
    return false;
  this->NumProcessedForVPFfractions=0;
  Rational oldCheckSum;
//  QuasiPolynomialOld oldOutput;
  if (PartFraction::MakingConsistencyCheck)
    PartFractions::CheckSum.MakeZero();
  theGlobalVariables.theIndicatorVariables.NumProcessedMonomialsCurrentFraction=0;
  output.MakeZeroLatTiceZn(this->AmbientDimension);
  ///////////////////////////////////////////////
  //this->flagAnErrorHasOccurredTimeToPanic=true;
  //PartFraction::flagAnErrorHasOccurredTimeToPanic=true;
  //this->ComputeDebugString();
  ///////////////////////////////////////////////
  QuasiPolynomial tempQP;
  for (int i=0; i<this->size(); i++)
  { //if (this->flagAnErrorHasOccurredTimeToPanic)
    //{ this->TheObjects[i].ComputeDebugString();
    //}
    if ((*this)[i].rootIsInFractionCone(*this, &newIndicator, theGlobalVariables))
    { const PartFraction& currentPF=(*this)[i];
      currentPF.GetVectorPartitionFunction(*this, this->theCoeffs[i], tempQP, theGlobalVariables);
/*      if(PartFraction::MakingConsistencyCheck)
      {  Rational tempLRat2, tempLRat3, tempLRat4;
        std::string tempS1, tempS2, tempS3, tempS4;
        tempQP.Evaluate(PartFraction::theVectorToBePartitioned, tempLRat2);
        output.Evaluate(PartFraction::theVectorToBePartitioned, tempLRat3);
        tempLRat2.ToString(tempS1);
        tempLRat3.ToString(tempS2);
        tempLRat4.Assign(tempLRat2);
        tempLRat4.Add(tempLRat3);
        PartFraction::CheckSum.ToString(tempS3);
        PartFractions::CheckSum.ToString(tempS4);
        assert(tempLRat2.den.IsEqualTo(LIOne));
        assert(tempLRat3.den.IsEqualTo(LIOne));
        assert(tempLRat2.IsEqualTo(PartFraction::CheckSum));
        assert(tempLRat4.IsEqualTo(PartFractions::CheckSum));
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
  /*    if (PartFraction::MakingConsistencyCheck)
      {  Rational tempLRat;
        output.Evaluate(PartFraction::theVectorToBePartitioned, tempLRat);
        if ((! tempLRat.IsEqualTo(PartFractions::CheckSum)) || (! tempLRat.den.IsEqualTo(LIOne)))
        { std::string tempS, tempS2, tempS3, tempS4;
          Rational tempLRat2;
          tempQP.Evaluate(PartFraction::theVectorToBePartitioned, tempLRat2);
          PartFractions::CheckSum.ToString(tempS);
          tempLRat.ToString(tempS2);
          oldCheckSum.ToString(tempS3);
          tempLRat2.ToString(tempS4);
          PartFraction::flagAnErrorHasOccurredTimeToPanic=true;
  //        assert(false);
        }
        oldCheckSum.Assign(PartFractions::CheckSum);
        oldOutput.Assign(output);
      }*/
      this->MakeProgressVPFcomputation(theGlobalVariables);
    }
  }
/*  if (PartFraction::MakingConsistencyCheck)
  { std::string tempS;
    PartFractions::CheckSum.ToString(tempS);
    Stop();
  }*/
  return true;
}

void LargeInt::WriteToFile(std::fstream& output)
{ std::string tempS;
  this->ToString(tempS);
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
  { int x=input[i]-'0';
    if (x<0 || x>9)
      return;
    if (i>startingIndex)
      this->value*=10;
    this->value+=(unsigned) x;
  }
  if (!this->IsEqualToZero())
    if (input[0]=='-')
      this->sign=-1;
}

std::string PartFractions::DoTheFullComputationReturnLatexFileString
(GlobalVariables& theGlobalVariables, Vectors<Rational>& toBePartitioned, FormatExpressions& theFormat, std::string* outputHtml)
{ std::string whatWentWrong;
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
  outHtml << theGlobalVariables.theDrawingVariables.GetHtmlFromDrawOperationsCreateDivWithUniqueName(this->AmbientDimension);
  Vector<Rational> tempRoot; tempRoot.MakeZero(this->AmbientDimension);
  tempRoot.MakeZero(this->AmbientDimension);
  assert(false);
//  this->initFromRoots(theChambersOld.theDirections, theGlobalVariables);
  out << "\\documentclass{article}\\usepackage{amsmath, amsfonts, amssymb} \n\\begin{document}\n";
  out << "The vector partition funciton is the number of ways you can represent a vector $(x_1,\\dots, x_n)$ as a non-negative integral linear combination of "
  << " a given set of vectors.  You requested the vector partition function with respect to the set of vectors: ";
  assert(false);
//  out << this->theChambersOld.theDirections.ElementToStringGeneric();
  out << "\n\n The corresponding generating function is: " << this->ToString(theGlobalVariables, theFormat) << "= (after splitting acording to algorithm)";
  this->split(theGlobalVariables, 0);
  out << this->ToString(theGlobalVariables, theFormat);
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
      out << "\n\n" << currentChamber.ToString(true, false, true, false, theFormat);
      out << "\n\nQuasipolynomial: " << tempQP.ToString(false, true, theFormat);
      outHtml << "<hr>Chamber: " << currentChamber.ToString(false, true, true, false, theFormat);
      bool useHtml=false;
      if (tempQP.valueOnEachLatticeShift.size>0)
        if (tempQP.valueOnEachLatticeShift[0].size<30)
          useHtml=true;
      outHtml << "<br>Quasi-polynomial: " <<
      CGI::GetHtmlMathDivFromLatexFormulA(tempQP.ToString(useHtml, true, theFormat));
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

void QuasiPolynomial::AddLatticeShift
(const Polynomial<Rational>& input, const Vector<Rational>& inputShift)
{ Vector<Rational> theShift=inputShift;
  this->AmbientLatticeReduced.ReduceVector(theShift);
  int index=this->LatticeShifts.GetIndex(theShift);
  if (index==-1)
  { index=this->LatticeShifts.size;
    this->LatticeShifts.AddOnTop(theShift);
    this->valueOnEachLatticeShift.ExpandOnTop(1);
    this->valueOnEachLatticeShift.LastObject()->MakeZero();
  }
  this->valueOnEachLatticeShift.TheObjects[index]+=input;
  if (this->valueOnEachLatticeShift.TheObjects[index].IsEqualToZero())
  { this->LatticeShifts.RemoveIndexSwapWithLast(index);
    this->valueOnEachLatticeShift.RemoveIndexSwapWithLast(index);
  }
}

void QuasiPolynomial::Substitution
(const Matrix<Rational>& mapFromNewSpaceToOldSpace, const Lattice& ambientLatticeNewSpace, QuasiPolynomial& output, GlobalVariables& theGlobalVariables)
{ MacroRegisterFunctionWithName("QuasiPolynomial::Substitution");
  //Format of the substitution.
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
    mapFromNewSpaceToOldSpace.GetVectorFromRow(i, tempRoot);
    currentPoly.MakeLinPolyFromRootNoConstantTerm(tempRoot);
  }
  Polynomial<Rational> tempP;
  for (int i=0; i<this->valueOnEachLatticeShift.size; i++)
  { tempP=this->valueOnEachLatticeShift[i];
    bool tempB=tempP.Substitution(theSub);
    if (!tempB)
    { std::cout << "This is a programming error: substitution   " << theSub.ToString() << " into polynomial "
      << tempP.ToString() << " failed but the current function does not handle this properly. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    for (int j=0; j<allRepresentatives.size; j++)
      if (imagesAllRepresentatives[j]==this->LatticeShifts[i])
        output.AddLatticeShift(tempP, allRepresentatives[j]);
  }
}

void QuasiPolynomial::Substitution
(const Vector<Rational>& inputTranslationSubtractedFromArgument, QuasiPolynomial& output, GlobalVariables& theGlobalVariables)
{ MacroRegisterFunctionWithName("QuasiPolynomial::Substitution");
  //format of the translation. If the starting quasipolynomial was P(y_1, ..., y_n),
  //and the translation has coordinates (t_1, ..., t_n),
  //then the resulting quasipolynomial will be P(x_1-t_1, ..., x_n-t_n)
  PolynomialSubstitution<Rational> theSub;
  theSub.MakeIdSubstitution(this->GetNumVars(), (Rational) 1);
  for (int i=0; i<theSub.size; i++)
    theSub[i].AddConstant(-inputTranslationSubtractedFromArgument[i]);
  Polynomial<Rational> tempP;
  output.MakeZeroLatTiceZn(this->GetNumVars());
  output.AmbientLatticeReduced=this->AmbientLatticeReduced;
  for (int i=0; i<this->valueOnEachLatticeShift.size; i++)
  { tempP=this->valueOnEachLatticeShift[i];
    bool tempB=tempP.Substitution(theSub);
    if (!tempB)
    { std::cout << "This is a programming error: substitution   " << theSub.ToString() << " into polynomial "
      << tempP.ToString() << " failed but the current function does not handle this properly. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
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

bool QuasiPolynomial::SubstitutionLessVariables(const PolynomialSubstitution<Rational>& theSub, QuasiPolynomial& output, GlobalVariables& theGlobalVariables)const
{ Matrix<Rational> theLatticeSub;
  if (!this->AmbientLatticeReduced.GetHomogeneousSubMatFromSubIgnoreConstantTerms(theSub, theLatticeSub, theGlobalVariables))
    return false;
  Matrix<Rational> theSubLatticeShift;
  output.AmbientLatticeReduced=this->AmbientLatticeReduced;
  if(!output.AmbientLatticeReduced.SubstitutionHomogeneous(theLatticeSub, theGlobalVariables))
    return false;
  theSubLatticeShift.init(theLatticeSub.NumRows,1);
  for (int i=0; i<theSubLatticeShift.NumRows; i++)
    theSub.TheObjects[i].GetConstantTerm(theSubLatticeShift.elements[i][0], (Rational) 0);
  Matrix<Rational> theShiftImage, shiftMatForm;
  output.LatticeShifts.size=0;
  output.valueOnEachLatticeShift.size=0;
  output.valueOnEachLatticeShift.ReservE(this->LatticeShifts.size);
  output.LatticeShifts.ReservE(this->LatticeShifts.size);
  Vector<Rational> tempRoot;
  Polynomial<Rational> tempP;
  for (int i=0; i<this->LatticeShifts.size; i++)
  { shiftMatForm.AssignVectorColumn(this->LatticeShifts.TheObjects[i]);
    shiftMatForm-=theSubLatticeShift;
    if (theLatticeSub.Solve_Ax_Equals_b_ModifyInputReturnFirstSolutionIfExists(theLatticeSub, shiftMatForm, theShiftImage))
    { tempRoot.AssignMatDetectRowOrColumn(theShiftImage);
      tempP=this->valueOnEachLatticeShift[i];
      bool tempB=tempP.Substitution(theSub);
      if (!tempB)
      { std::cout << "This is a programming error: substitution   " << theSub.ToString() << " into polynomial "
        << tempP.ToString() << " failed but the current function does not handle this properly. "
        << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
        assert(false);
      }
      output.AddLatticeShift(tempP, tempRoot);
    }
  }
  return true;
}

bool Lattice::SubstitutionHomogeneous(const PolynomialSubstitution<Rational>& theSub, GlobalVariables& theGlobalVariables)
{ Matrix<Rational>  matSub;
  if (!this->GetHomogeneousSubMatFromSubIgnoreConstantTerms(theSub, matSub, theGlobalVariables))
    return false;
  return this->SubstitutionHomogeneous(matSub, theGlobalVariables);
}

bool Lattice::GetHomogeneousSubMatFromSubIgnoreConstantTerms
(const PolynomialSubstitution<Rational>& theSub, Matrix<Rational>& output, GlobalVariables& theGlobalVariables)
{ if (theSub.size<1)
    return false;
  int theTargetDim=0;
  for (int i=0; i<theSub.size; i++)
   theTargetDim=MathRoutines::Maximum(theTargetDim,  theSub[i].GetMinNumVars());
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
{ //std::cout << "<br>Starting lattice: " << this->ToString(true, false) << "<br>";
  Vectors<Rational> startingBasis, resultBasis;
  startingBasis.AssignMatrixRows(this->basisRationalForm);
  //std::cout << "Starting normal: " << theNormal.ToString();
  Vector<Rational> theScalarProducts;
  theScalarProducts.SetSize(startingBasis.size);
  for (int i=0; i<this->basisRationalForm.NumRows; i++)
    theScalarProducts[i]=theNormal.ScalarEuclidean(startingBasis[i]);
  if (theScalarProducts.IsEqualToZero())
    return;
  //std::cout << "<br>the scalar products: " << theScalarProducts.ToString();
  int pivotColumnIndex=theScalarProducts.getIndexLastNonZeroCoordinate();
  Rational pivotCoeff=theScalarProducts[pivotColumnIndex];
  theScalarProducts/=-pivotCoeff;
  //std::cout << "<br>the scalar products after scaling: " << theScalarProducts.ToString();
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
  //std::cout << "<br>the scalar products after scaling to integral: " << theScalarProducts.ToString();
  eigenSpacePlusOrthogonalComponent.TheObjects[pivotColumnIndex]=theScalarProducts;
  //std::cout << "<br>The eigenspace before intersection: " << eigenSpacePlusOrthogonalComponent.ToString();
  Lattice eigenLattice, theZnLattice;
  eigenLattice.MakeFromRoots(eigenSpacePlusOrthogonalComponent);
  //std::cout << "<br>The eigen-Lattice: " << eigenLattice.ToString(true, false);
  theZnLattice.MakeZn(theScalarProducts.size);
  //std::cout << "<br>The Zn-Lattice: " << theZnLattice.ToString(true, false);
  theZnLattice.IntersectWithBothOfMaxRank(eigenLattice);
  //std::cout << "<br>Zn intersected with eigen-Lattice: " << theZnLattice.ToString(true, false);
  resultBasis.ReservE(theScalarProducts.size-1);
  Vector<Rational> tempRoot, resultRoot; Rational orthogonalComponent;
  for (int i=0; i<theZnLattice.basisRationalForm.NumRows; i++)
  { theZnLattice.basisRationalForm.GetVectorFromRow(i, tempRoot);
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
  //std::cout << "<br>Resulting basis: " << resultBasis.ToString();
  this->MakeFromRoots(resultBasis);
  //std::cout << "<br>Final answer lattice form: " << this->ToString(true, false);
}

void Lattice::IntersectWithLinearSubspaceSpannedBy(const Vectors<Rational>& theSubspaceBasis)
{ Vectors<Rational> theNormals;
  Matrix<Rational> theMat;
  theSubspaceBasis.GetMatrixRootsToRows(theMat);
  theMat.GetZeroEigenSpace(theNormals);
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
  (const Matrix<Rational>& theSub, GlobalVariables& theGlobalVariables)
{ int targetDim=theSub.NumCols;
  if (theSub.NumRows!=this->GetDim())
    return false;
  //std::cout <<"<br> the sub: " << theSub.ToString(true, false) << "<br>";
  int startingDim=this->GetDim();
  Matrix<Rational> theMat, oldBasisTransformed, matRelationBetweenStartingVariables;
  theMat=theSub;
  oldBasisTransformed=this->basisRationalForm;
  oldBasisTransformed.Transpose();
  Selection nonPivotPoints;
  //std::cout << "<br>the matrices to be transformed: " << theMat.ToString(true, false) << "<br>" << oldBasisTransformed.ToString(true, false);
  theMat.GaussianEliminationByRows(&oldBasisTransformed, &nonPivotPoints);
  //std::cout << "<br>afer transformation: " << theMat.ToString(true, false) << "<br>" << oldBasisTransformed.ToString(true, false);
  if (nonPivotPoints.CardinalitySelection!=0)
    return false;
  int numNonZeroRows=nonPivotPoints.MaxSize;
  int numZeroRows=theMat.NumRows-numNonZeroRows;
  matRelationBetweenStartingVariables.init(numZeroRows, startingDim);
  for (int i=0; i<numZeroRows; i++)
    for (int j=0; j<startingDim; j++)
      matRelationBetweenStartingVariables.elements[i][j]=oldBasisTransformed.elements[i+numNonZeroRows][j];
  Vectors<Rational> theEigenSpace;
  matRelationBetweenStartingVariables.GetZeroEigenSpaceModifyMe(theEigenSpace);
  //std::cout << "<br>matRelationBetweenStartingVariables" <<  matRelationBetweenStartingVariables.ToString(true, false);
  for (int i=0; i<theEigenSpace.size; i++)
    theEigenSpace[i].ScaleByPositiveRationalToIntegralMinHeight();
  //std::cout << "the basis: " << theEigenSpace.ToString();
  oldBasisTransformed.ActOnVectorsColumn(theEigenSpace);
  //std::cout << "<br>the basis transformed: " << theEigenSpace.ToString();
  this->basisRationalForm.init(targetDim, targetDim);
  for (int i=0; i<targetDim; i++)
    for (int j=0; j<targetDim; j++)
      this->basisRationalForm.elements[i][j]=theEigenSpace.TheObjects[i].TheObjects[j];
  this->basisRationalForm.GetMatrixIntWithDen(this->basis, this->Den);
  this->Reduce();
  //std::cout << "<br><br>and the sub result is: <br>" << this->ToString(true, false);
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
    { output << this->Normals.TheObjects[i].TheObjects[j].ToString();
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
    //std::cout << "vector input " << tempS << " read as " << tempRoot.ToString();
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
  result=this->CreateFromNormals(buffer, theGlobalVariables);
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
{ ProgressReport theReport(theGlobalVariables);
  int numReadWords;
  XMLRoutines::ReadThroughFirstOpenTag(input, numReadWords, this->GetXMLClassName());
  std::string tempS;
  input >> tempS >> this->numNonParaM >> tempS >> this->numProcessedNonParam;
  theReport.Report("Loading complex starting per representative...");
  XMLRoutines::ReadThroughFirstOpenTag(input, "complexStartingPerRepresentative");
  this->complexStartingPerRepresentative.ReadFromFile(input, theGlobalVariables);
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, "complexStartingPerRepresentative");
  theReport.Report("Loading complex refined per representative...");
  XMLRoutines::ReadThroughFirstOpenTag(input, "complexRefinedPerRepresentative");
  this->complexRefinedPerRepresentative.ReadFromFile(input, theGlobalVariables);
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, "complexRefinedPerRepresentative");
  theReport.Report("Loading the max candidates...");
  XMLRoutines::ReadThroughFirstOpenTag(input, "theMaximaCandidates");
  this->theMaximaCandidates.ReadFromFile(input, theGlobalVariables);
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, "theMaximaCandidates");
  theReport.Report("Loading starting linear polys...");
  XMLRoutines::ReadThroughFirstOpenTag(input, "startingLPtoMaximize");
  this->startingLPtoMaximize.ReadFromFile(input, theGlobalVariables);
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, "startingLPtoMaximize");
  theReport.Report("Loading final maxima...");
  XMLRoutines::ReadThroughFirstOpenTag(input, "finalMaxima");
  this->finalMaxima.ReadFromFile(input, theGlobalVariables);
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, "finalMaxima");
  theReport.Report("Loading starting lattice...");
  XMLRoutines::ReadThroughFirstOpenTag(input, "theStartingLattice");
  this->theStartingLattice.ReadFromFile(input, theGlobalVariables);
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, "theStartingLattice");
  theReport.Report("Loading final rougher lattice...");
  XMLRoutines::ReadThroughFirstOpenTag(input, "theFinalRougherLattice");
  this->theFinalRougherLattice.ReadFromFile(input, theGlobalVariables);
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, "theFinalRougherLattice");
  theReport.Report("Loading starting representative...");
  XMLRoutines::ReadThroughFirstOpenTag(input, "theStartingRepresentative");
  this->theStartingRepresentative.ReadFromFile(input);
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, "theStartingRepresentative");
  theReport.Report("Loading final representatives...");
  XMLRoutines::ReadThroughFirstOpenTag(input, "theFinalRepresentatives");
  this->theFinalRepresentatives.ReadFromFile(input, theGlobalVariables);
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, "theFinalRepresentatives");
  theReport.Report("Loading cones larger dim...");
  XMLRoutines::ReadThroughFirstOpenTag(input, "theConesLargerDim");
  this->theConesLargerDim.ReadFromFile(input, theGlobalVariables, UpperLimitDebugPurposes);
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, "theConesLargerDim");
  theReport.Report("Loading cones smaller dim...");
  XMLRoutines::ReadThroughFirstOpenTag(input, "theConesSmallerDim");
  this->theConesSmallerDim.ReadFromFile(input, theGlobalVariables);
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, "theConesSmallerDim");
  theReport.Report("Loading IsInfinite array...");
  XMLRoutines::ReadThroughFirstOpenTag(input, "IsInfinity");
  input >> this->IsInfinity;
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, "IsInfinity");
  theReport.Report("Loading LPtoMaximizeLargerDim...");
  XMLRoutines::ReadThroughFirstOpenTag(input, "LPtoMaximizeLargerDim");
  this->LPtoMaximizeLargerDim.ReadFromFile(input, theGlobalVariables, UpperLimitDebugPurposes);
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, "LPtoMaximizeLargerDim");
  theReport.Report("Loading LPtoMaximizeSmallerDim...");
  XMLRoutines::ReadThroughFirstOpenTag(input, "LPtoMaximizeSmallerDim");
  this->LPtoMaximizeSmallerDim.ReadFromFile(input, theGlobalVariables);
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, "LPtoMaximizeSmallerDim");
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
  ProgressReport theReport(theGlobalVariables);
  if (theGlobalVariables!=0)
    theReport.Report("Writing small data... ");
  this->theMaxComputation.WriteToFile(output, theGlobalVariables);
  this->GmodKnegativeWeightS.WriteToFile(output, theGlobalVariables);
  this->GmodKNegWeightsBasisChanged.WriteToFile(output, theGlobalVariables);
  this->theLinearOperators.WriteToFile(output);
  this->theLinearOperatorsExtended.WriteToFile(output);
  this->PreimageWeylChamberLargerAlgebra.WriteToFile(output, theGlobalVariables);
  this->PreimageWeylChamberSmallerAlgebra.WriteToFile(output, theGlobalVariables);
  this->WeylChamberSmallerAlgebra.WriteToFile(output, theGlobalVariables);
  if (theGlobalVariables!=0)
    theReport.Report("Writing QP's non-subbed... ");
  this->theQPsNonSubstituted.WriteToFile(output, theGlobalVariables);
  if (theGlobalVariables!=0)
    theReport.Report("Writing QP's subbed... ");
  output << XMLRoutines::GetOpenTagNoInputCheckAppendSpacE("QPsSubbed");
  this->theQPsSubstituted.WriteToFile(output, theGlobalVariables);
  output << XMLRoutines::GetCloseTagNoInputCheckAppendSpacE("QPsSubbed");
  if (theGlobalVariables!=0)
    theReport.Report("Writing small data... ");

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
    theReport.Report("Writing param chamber complex... ");
  this->projectivizedParamComplex.WriteToFile(output, theGlobalVariables);
  if (theGlobalVariables!=0)
    theReport.Report("Writing projectivized chamber complex... ");
  this->smallerAlgebraChamber.WriteToFile(output, theGlobalVariables, this->UpperLimitChambersForDebugPurposes);
  this->projectivizedChambeR.WriteToFile(output, theGlobalVariables, this->UpperLimitChambersForDebugPurposes);
  if (theGlobalVariables!=0)
    theReport.Report("Writing to file done...");
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
  ProgressReport theReport(theGlobalVariables);
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
{} //   this->theParser.theHmm.MakeG2InB3(this->theParser, *theGlobalVariables);
  else
  { GlobalVariables tempGlobalVars;
{}//    this->theParser.theHmm.MakeG2InB3(this->theParser, tempGlobalVars);
  }
  theReport.Report("Loading param subchambers cone form... ");
  this->theMaxComputation.ReadFromFile(input, theGlobalVariables, this->UpperLimitChambersForDebugPurposes);
  theReport.Report("Loading more pieces of data... ");
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
  theReport.Report("Loading multiplicities... ");
  XMLRoutines::ReadThroughFirstOpenTag(input, numReadWords, "theMultiplicities");
  this->theMultiplicities.ReadFromFile(input, theGlobalVariables, this->UpperLimitChambersForDebugPurposes);
  XMLRoutines::ReadEverythingPassedTagOpenUntilTagClose(input, numReadWords, "theMultiplicities");
  this->theCoeffs.ReadFromFile(input);
  this->theTranslationS.ReadFromFile(input, theGlobalVariables);
  this->theTranslationsProjectedBasisChanged.ReadFromFile(input, theGlobalVariables);
  theReport.Report("Loading partial fractions... ");
//  this->thePfs.ReadFromFile(input, theGlobalVariables);
  theReport.Report("Loading projectivized param complex... ");
  this->projectivizedParamComplex.ReadFromFile(input, theGlobalVariables, -1);
  theReport.Report("Loading the complex... ");
  this->smallerAlgebraChamber.ReadFromFile(input, theGlobalVariables, this->UpperLimitChambersForDebugPurposes);
  this->projectivizedChambeR.ReadFromFile(input, theGlobalVariables, this->UpperLimitChambersForDebugPurposes);
  theReport.Report("Loading complete... ");
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
{ Matrix<Rational> tempMat;
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
//  std::cout << output.basisRationalForm.ToString(true, false);
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

void Cone::IntersectAHyperplane
  (Vector<Rational>& theNormal, Cone& outputConeLowerDim, GlobalVariables& theGlobalVariables)
{ assert(!theNormal.IsEqualToZero());
  int theDimension=theNormal.size;
  Matrix<Rational> tempMat, theEmbedding, theProjection;
  tempMat.AssignVectorRow(theNormal);
  Vectors<Rational> theBasis;
  tempMat.GetZeroEigenSpace(theBasis);
  assert(theBasis.size==theNormal.size-1);
  theEmbedding.AssignVectorsToRows(theBasis);
  theEmbedding.Transpose();
  theBasis.AddOnTop(theNormal);
  Vectors<Rational> tempRoots, tempRoots2, tempRoots3;
  Matrix<Rational> tempMat2;
  tempRoots.MakeEiBasis(theDimension);
  tempRoots.GetCoordsInBasis(theBasis, tempRoots2, tempRoots3, tempMat2);
  theProjection.AssignVectorsToRows(tempRoots2);
  theProjection.Transpose();
  theProjection.Resize(theDimension-1, theDimension, false);
  Vectors<Rational> newNormals=this->Normals;
  theProjection.ActOnVectorsColumn(newNormals);
  bool tempBool=outputConeLowerDim.CreateFromNormals(newNormals, &theGlobalVariables);
  assert(!tempBool);

}

bool Cone::GetRootFromLPolyConstantTermGoesToLastVariable
(Polynomial<Rational>& inputLPoly, Vector<Rational>& output)
{ if (!inputLPoly.IsLinear())
    return false;
  output.MakeZero(inputLPoly.GetMinNumVars()+1);
  for (int i=0; i<inputLPoly.size(); i++)
  { int theIndex;
    if (inputLPoly[i].::MonomialP::IsOneLetterFirstDegree(&theIndex))
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
      case '%': out << "%25"; break;
      case '&': out << "%26"; break;
      case '\n': out << "%0D%0A"; break;
      case '\t': out << "%09"; break;
      case '~': out << "%7E"; break;
//      case '': out << ""; break;
      default: out << input[i]; break;
    }
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
      out << theMult.ToString(false, true, &theFormat) << "\\end{tabular}\\\\\n";
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
        out << theMult.ToString(false, true, theFormat) << "\\\\\n";
      }
    }
  }*/
  out << "Total number of chambers with multiplicity 1 or less: " << numFoundChambers;
  out << "\\end{longtable}\n\n\n\n";
  out << "\\end{document}";
  return out.str();
}

std::string GeneralizedVermaModuleCharacters::PrepareReportOneCone
  (FormatExpressions& theFormat, const Cone& theCone, GlobalVariables& theGlobalVariables)
{ std::stringstream out1;
  std::string tempS1, tempS2;
  Vector<Rational> normalNoConstant;
  int dimSmallerAlgebra=this->theLinearOperators[0].NumRows;
  int dimLargerAlgebra=this->theLinearOperators[0].NumCols;
  Rational theConst;
  out1 << "\\begin{tabular}{rcl}";
  for (int i=0; i<theCone.Normals.size; i++)
  { Vector<Rational>& currentNormal=theCone.Normals[i];
    normalNoConstant=currentNormal;
    normalNoConstant.SetSize(dimSmallerAlgebra+dimLargerAlgebra);
    theConst=-(*currentNormal.LastObject());
    if(!normalNoConstant.IsEqualToZero())
      out1 << "$" << normalNoConstant.ToStringLetterFormat("x", &theFormat) << "$ & $\\geq$ & $"
      << theConst.ToString() << "$ \\\\";
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

bool PartFractions::RemoveRedundantShortRootsIndex
(MonomialCollection<PartFraction, Polynomial<LargeInt> >& output, int theIndex, GlobalVariables& theGlobalVariables,
 Vector<Rational>* Indicator)
 { if (!(*this)[theIndex].rootIsInFractionCone(*this, Indicator, theGlobalVariables))
    return false;
  bool found=false;
  for (int k=0; k < (*this)[theIndex].IndicesNonZeroMults.size; k++)
  { int currentIndex=(*this)[theIndex].IndicesNonZeroMults.TheObjects[k];
    const oneFracWithMultiplicitiesAndElongations& currentFrac = (*this)[theIndex].TheObjects[currentIndex];
    if (currentFrac.Elongations.size>1)
    { found=true;
      break;
    }
  }
  if (!found)
    return false;
  PartFraction thePF;
  Rational localStartCheckSum, localEndCheckSum;
  std::string tempS, tempS1, tempS2;
  Polynomial<LargeInt> tempIP, currentCoeff;
  this->PopMonomial(theIndex, thePF, currentCoeff);
  for (int k=0; k<thePF.IndicesNonZeroMults.size; k++)
  { int currentIndex=thePF.IndicesNonZeroMults.TheObjects[k];
    oneFracWithMultiplicitiesAndElongations& currentFrac = thePF.TheObjects[currentIndex];
    int LCMElongations = currentFrac.GetLCMElongations();
    tempS= this->startingVectors[currentIndex].ToString();
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
    if (PartFraction::MakingConsistencyCheck || this->flagAnErrorHasOccurredTimeToPanic)
    { //thePF.ComputeOneCheckSum(*this, localEndCheckSum, this->AmbientDimension, theGlobalVariables);
      //localEndCheckSum.ToString(tempS1);
      //assert(localStartCheckSum.IsEqualTo(localEndCheckSum));
    }
  }
  this->AddMonomial(thePF, currentCoeff);
  return true;
}

void LargeIntUnsigned::ElementToStringLargeElementDecimal(std::string& output)const
{ std::stringstream out;
  if (this->CarryOverBound==1000000000UL)
  { std::string tempS;
    out << (*this->theDigits.LastObject());
    for (int i=this->theDigits.size-2; i>=0; i--)
    { std::stringstream tempStream;
      tempStream << this->theDigits[i];
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
  int initialNumDigitsEstimate=MaxNumIntegersPerCarryOverBound*this->theDigits.size;
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
        currentPower+=bufferPowersOfBase[highestBufferIndex];
      }
      out << theDigit;
      numRemainingDigits--;
      if (theDigit!=1)
        bufferPowersOfBase[highestBufferIndex]*=theDigit;
      Remainder.SubtractSmallerPositive(bufferPowersOfBase[highestBufferIndex]);
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

LargeIntUnsigned::LargeIntUnsigned()
{ this->theDigits.SetSize(1);
  this->theDigits[0]=0;
}

LargeIntUnsigned::LargeIntUnsigned(unsigned int x)
{ this->AssignShiftedUInt(x,0);
}

LargeIntUnsigned::  LargeIntUnsigned(const LargeIntUnsigned& x)
{ (*this)=x;
}

void LargeIntUnsigned::lcm(const LargeIntUnsigned& a, const LargeIntUnsigned& b, LargeIntUnsigned& output)
{ LargeIntUnsigned tempUI, tempUI2;
  if (a.IsEqualToZero() || b.IsEqualToZero())
  { std::cout << "This is a programming error: calling lcm on zero elements is not allowed. "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  LargeIntUnsigned::gcd(a, b, tempUI);
  a.MultiplyBy(b, tempUI2);
  output=(tempUI2);
  output.DivPositive(tempUI, output, tempUI2);
  assert(!output.IsEqualToZero());
}

void LargeIntUnsigned::operator=(const LargeIntUnsigned& x)
{ this->theDigits=x.theDigits;
}

void LargeIntUnsigned::operator=(unsigned int x)
{ this->AssignShiftedUInt(x,0);
}

unsigned int LargeIntUnsigned::HashFunction()const
{ int numCycles=MathRoutines::Minimum(this->theDigits.size, SomeRandomPrimesSize);
  unsigned int result=0;
  for (int i=0; i<numCycles; i++)
    result+=this->theDigits[i]*SomeRandomPrimes[i];
  return result;
}

bool LargeIntUnsigned::IsPositive()const
{ return this->theDigits.size>1 || this->theDigits[0]>0;
}

bool LargeIntUnsigned::IsEqualToOne()const
{ return this->theDigits.size==1 && this->theDigits[0]==1;
}

bool LargeIntUnsigned::IsEqualToZero()const
{ return this->theDigits.size==1 && this->theDigits[0]==0;
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
 Vectors<Rational>& outputRepresentatives, Vectors<Rational>& movementInDirectionPerRepresentative, Lattice& outputRougherLattice, GlobalVariables& theGlobalVariables)
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
  theDirectionLattice.basisRationalForm.GetVectorFromRow(0, outputDirectionMultipleOnLattice);
  //std::cout << "<br>the normal: " << theNormal.ToString();
  //std::cout << "<br> the direction lattice: " << theDirectionLattice.ToString();
  theHyperplaneLatticeNoShift=*this;
  theHyperplaneLatticeNoShift.IntersectWithLinearSubspaceGivenByNormal(theNormal);
  //std::cout << "<br>the non-affine hyperplane intersected with the lattice: " << theHyperplaneLatticeNoShift.ToString();
  tempRoots.AssignMatrixRows(theHyperplaneLatticeNoShift.basisRationalForm);
  tempRoots.AddOnTop(outputDirectionMultipleOnLattice);
  outputRougherLattice.MakeFromRoots(tempRoots);
  this->GetAllRepresentatives(outputRougherLattice, outputRepresentatives);
  //std::cout << "<br>the rougher lattice: " << outputRougherLattice.ToString();
  //std::cout << "<br>representatives of the quotient of the two lattices: " << outputRepresentatives.ToString();
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
    //std::cout << "<br>Representative: " << outputRepresentatives.TheObjects[i].ToString() << " and the hyperplane: " << currentMovement.ToString();
  }
//  std::cout << "<hr>"
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
  Polynomial<Rational> basisMonsZeroWeight, basisMonsAll;
  basisMonsZeroWeight.MakeZero();
  basisMonsZeroWeight.SetExpectedSize(numCycles);
  basisMonsAll.MakeZero();
  basisMonsAll.SetExpectedSize(numCycles);
  MonomialP theMon;
  theMon.MakeOne(this->theDimension);
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
    if (theWeight.ScalarEuclidean(theCartanAction).IsEqualToZero())
      basisMonsZeroWeight.AddMonomial(theMon, theMonCoeff);
  }
//  std::cout << "<br>Num cycles:" << numCycles << "<br>The basis mons (there are " << basisMonsZeroWeight.size << " of them): "  << basisMonsZeroWeight.ToString(PolyFormatLocal);
  Matrix<Rational> tempMat;
  tempMat.init(basisMonsAll.size()*2, basisMonsZeroWeight.size());
//  tempMat.init(basisMonsAll.size*numGenerators, basisMonsZeroWeight.size);
  Polynomial<Rational>  tempP;
  for (int l=0; l<2; l++)
    for (int k=0; k<basisMonsZeroWeight.size(); k++)
    { if (l==0)
        this->theE.ActOnMonomialAsDifferentialOperator(basisMonsZeroWeight[k], tempP);
      else
        this->theF.ActOnMonomialAsDifferentialOperator(basisMonsZeroWeight[k], tempP);
      for (int j=0; j<basisMonsAll.size(); j++)
      { int indexInResult=tempP.theMonomials.GetIndex(basisMonsAll[j]);
        int currentRow=l*basisMonsAll.size()+j;
        if (indexInResult==-1)
          tempMat.elements[currentRow][k]=0;
        else
          tempMat.elements[currentRow][k]=tempP.theCoeffs[indexInResult];
      }
    }
//  if (tempMat.NumRows<120)
//    std::cout << "<div class=\"math\" scale=\"50\">" << tempMat.ToString(false, true) << "</div>";
  Vectors<Rational> tempRoots;
  tempMat.GetZeroEigenSpaceModifyMe(tempRoots);
  output.SetSize(tempRoots.size);
//  std::cout << "<br>invariants Vector<Rational> form: " << tempRoots.ToString();
//  std::cout << "<br> .... and the invariants are: ";
  for (int i=0; i<output.size; i++)
  { Polynomial<Rational>& current=output[i];
    current.MakeZero();
    for (int j=0; j<basisMonsZeroWeight.size(); j++)
      if (!tempRoots[i][j].IsEqualToZero())
      { theMon=(basisMonsZeroWeight[j]);
        current.AddMonomial(theMon, tempRoots[i][j]);
      }
 //   std::cout << "<br>Invariant " << i << ":<br>" << current.ToString(PolyFormatLocal);
  }
  return true;
}

void GeneralizedVermaModuleCharacters::InitTheMaxComputation
(GlobalVariables& theGlobalVariables)
{ this->theMaxComputation.numNonParaM=2;
  this->theMaxComputation.theConesLargerDim.ReservE(this->projectivizedChambeR.size);
  this->theMaxComputation.LPtoMaximizeLargerDim.ReservE(this->theMultiplicities.size);
  this->theMaxComputation.theConesLargerDim.SetSize(0);
  this->theMaxComputation.LPtoMaximizeLargerDim.SetSize(0);
  Lattice ZnLattice;
  int theAffineDim=5;
//  int theProjectivizedDim=theAffineDim+1;
  ZnLattice.MakeZn(theAffineDim);
  this->numNonZeroMults=0;
  ProgressReport theReport(&theGlobalVariables);
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
      theReport.Report(out.str());
    }
}

std::string DrawingVariables::GetColorPsTricksFromColorIndex(int colorIndex)
{ std::stringstream out;
  int r=(colorIndex/65536)%256;
  int g=(colorIndex/256)%256;
  int b=colorIndex%256;
  out << "\\\\newrgbcolor{currentColor}{" <<((double) r)/255 << ", " << ((double) g)/255 << ", " << ((double) b)/255 << "}";
  return out.str();
}

std::string DrawingVariables::GetColorHtmlFromColorIndex(int colorIndex)
{ std::stringstream out;
  int r=(colorIndex/65536)%256;
  int g=(colorIndex/256)%256;
  int b=colorIndex%256;
  out << "#";
  if (r<16)
    out << 0;
  out << std::hex << r;
  if (g<16)
    out << 0;
  out << std::hex << g;
  if (b<16)
    out << 0;
  out << std::hex << b;
  return out.str();
}

std::string ConeLatticeAndShift::ToString(FormatExpressions& theFormat)
{ std::stringstream out;
  out << this->theProjectivizedCone.ToString(&theFormat);
  out << "<br>Shift+lattice: " << this->theShift.ToString() << " + " << this->theLattice.ToString();
  return out.str();
}

std::string SemisimpleLieAlgebra::ToString(FormatExpressions* theFormat)
{ MacroRegisterFunctionWithName("SemisimpleLieAlgebra::ToString");
  std::stringstream out;
  std::string tempS;
  Vector<Rational> tempRoot, tempRoot2;
  int numRoots=this->theWeyl.RootSystem.size;
  int theDimension = this->theWeyl.CartanSymmetric.NumRows;
  ElementSemisimpleLieAlgebra<Rational> tempElt1, tempElt2, tempElt3;
//  out << beginMath << "\\begin{array}{ccc}a& a&a\\\\a&a&a\\end{array}";
  std::string hLetter="h";
  std::string gLetter="g";
  if (theFormat!=0)
  { hLetter=theFormat->chevalleyHgeneratorLetter;
    gLetter=theFormat->chevalleyGgeneratorLetter;
  }
  out << "Type " << this->theWeyl.theDynkinType.ToString() << ".";
  out << "The letter " << CGI::GetHtmlMathSpanPure(hLetter) << " stands for elements of the Cartan subalgebra, <br>"
  << " the letter " << CGI::GetHtmlMathSpanPure(gLetter) << " stands for the Chevalley (root space) generators of non-zero weight. <br>"
  << " The generator " << CGI::GetHtmlMathSpanPure(hLetter+"_i") << " is the element of the Cartan subalgebra dual to the <br>"
  << "i^th simple root, that is, " << CGI::GetHtmlMathSpanPure("[" + hLetter + "_i, g]=\\langle \\alpha_i , \\gamma\\rangle g")
  << ", <br> where g is a Chevalley generator, " << CGI::GetHtmlMathSpanPure("\\gamma") << " is its weight, and <br>"
  << CGI::GetHtmlMathSpanPure("\\alpha_i") << " is the i^th simple root. ";
  std::stringstream theTableLateXStream, theHtmlStream;
  theHtmlStream << "<table><tr><td> roots simple coords </td><td>epsilon coordinates</td>"
  << "<td>[,]</td>";
  theTableLateXStream << "\\begin{array}{cc|";
  for (int i=0; i<this->GetNumGenerators()+1; i++)
    theTableLateXStream << "c";
  theTableLateXStream << "}\n";
  theTableLateXStream << "\\mathrm{roots~simple~coords}&\\varepsilon-\\mathrm{root~notation}&" << "[\\bullet, \\bullet]\n";
  for (int i=0; i<numRoots+theDimension; i++)
  { tempElt1.MakeGenerator(i, *this);
    tempS=tempElt1.ToString(theFormat);
    theHtmlStream << "<td>" << tempS << "</td>";
    theTableLateXStream << " & ";
    theTableLateXStream << tempS;
  }
  theTableLateXStream << "\\\\\n";
  theHtmlStream << "</tr>";
  Rational tempRat;
  //int lineCounter=0;
  for (int i=0; i<theDimension+numRoots; i++)
  { tempRoot=this->GetWeightOfGenerator(i);
    theTableLateXStream << tempRoot.ToString() << "&";
    theHtmlStream << "<tr><td>" << tempRoot.ToString() << "</td>";
    this->theWeyl.GetEpsilonCoords(tempRoot, tempRoot2);
    theTableLateXStream << tempRoot2.ToStringLetterFormat("\\varepsilon") << "&";
    theHtmlStream << "<td>" << tempRoot2.ToStringLetterFormat("e") << "</td>";
    tempElt1.MakeGenerator(i, *this);
    tempS=tempElt1.ToString(theFormat);
    theTableLateXStream << tempS;
    theHtmlStream << "<td>" << tempS << "</td>";
    for (int j=0; j<numRoots+theDimension; j++)
    { tempElt2.MakeGenerator(j, *this);
      this->LieBracket(tempElt1, tempElt2, tempElt3);
      tempS=tempElt3.ToString(theFormat);
      theTableLateXStream << "& ";
      theTableLateXStream << tempS;
      theHtmlStream << "<td>" << tempS << "</td>";
    }
    theHtmlStream << "</tr>";
    theTableLateXStream << "\\\\\n";
  }
  theHtmlStream << "</table>";
  theTableLateXStream << "\\end{array}";
  if (this->GetNumGenerators()<22)
  { out << "<div class=\"math\">" << theTableLateXStream.str() << "</div>";
    return out.str();
  }
  out << "<br><b> The Lie bracket table is too large to be rendered in LaTeX, displaying in"
  << " html format instead.</b> "
  << " Below you can find the same table in pure LaTeX, "
  << " which you can render in a separate LaTeX session, should "
  << " wish to do so on your own. " <<  theHtmlStream.str()
  << "The above table in LaTex format follows. <hr>"
  << theTableLateXStream.str() << "<hr>";
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

std::string ConeComplex::DrawMeToHtmlLastCoordAffine
(DrawingVariables& theDrawingVariables, FormatExpressions& theFormat)
{ bool isBad=false;
  isBad=this->DrawMeLastCoordAffine(true, theDrawingVariables, theFormat);
  std::stringstream out;
  out << theDrawingVariables.GetHtmlFromDrawOperationsCreateDivWithUniqueName(this->GetDim()-1);
  if (isBad)
    out << "<hr>" << "found cones which I can't draw<hr>";
  out << this->ToString(false, true);
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
  out << this->ToString(false, true);
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
      tempMat.GetZeroEigenSpace(tempRoots);
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
 int ChamberWallColor)const
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
  out << "<br>" << this->ToString(&theFormat);
  return out.str();
}

bool Cone::DrawMeProjective
(Vector<Rational>* coordCenterTranslation, bool initTheDrawVars,
 DrawingVariables& theDrawingVariables, FormatExpressions& theFormat)const
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
  { out << "There has been a programming error. The cone is empty.<br>"
    << this->ToString(&theFormat);
    return out.str();
  }
  theDrawingVariables.theBuffer.MakeMeAStandardBasis(this->GetDim());
  this->DrawMeProjective(0, true, theDrawingVariables, theFormat);
  theDrawingVariables.drawCoordSystemBuffer(theDrawingVariables, this->GetDim() ,0);
  out << theDrawingVariables.GetHtmlFromDrawOperationsCreateDivWithUniqueName(this->GetDim());
  out << "<br>" << this->ToString(&theFormat);
  return out.str();
}

std::string CGI::GetHtmlButton
(const std::string& buttonID, const std::string& theScript, const std::string& buttonText)
{ std::stringstream out;
  out << "\n<button id=\"" << buttonID << "\" " << CGI::GetStyleButtonLikeHtml()
    << " onclick=\"" << theScript << "\">" << buttonText << "</button>";
  return out.str();
}

std::string CGI::GetHtmlSpanHidableStartsHiddeN
  (const std::string& input)
{ std::stringstream out;
  CGI::GlobalFormulaIdentifier ++;
  std::stringstream buttonLabel;
  std::stringstream spanLabel;
  spanLabel << "hidableSpan" << CGI::GlobalFormulaIdentifier;
  buttonLabel << "button" << CGI::GlobalFormulaIdentifier;
  out << CGI::GetHtmlButton(buttonLabel.str(), "switchMenu('"+spanLabel.str() +"');", "info expand/collapse");
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
    out << "\\begin{array}{l}\n";
  out << input;
  if (useBeginArrayRCL)
    out << "\n\\end{array}";
  if (useDiv)
    out << "</div><br>";
  else
    out << "</span>";
  out << "<textarea id=\"theResultLatex" << CGI::GlobalFormulaIdentifier << "\" style=\"display: none\">";
  if (useBeginArrayRCL)
    out << "\\begin{array}{l}\n";
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
//  std::cout << "rho: " << this->rho.ToString() << "<hr>";
  Vectors<Rational> tempRoots;
  tempRoots.MakeEiBasis(this->GetDim());
  this->GetLowestElementInOrbit(lowest, &outputWeylElt, tempRoots, false, false);
  //std::stringstream out;
  //out << outputWeylElt;
//  out << "\n<br>";
  //Matrix<Rational>  tempMat;
  //this->GetMatrixOfElement(outputWeylElt, tempMat);
  //out << tempMat.ToString(true, false);
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
        if (!RhoAction)
          this->SimpleReflection<Rational>(i, inputOutput);
        else if (!UseMinusRho)
          this->SimpleReflectionRhoModified(i, inputOutput);
        else
          this->SimpleReflectionMinusRhoModified(i, inputOutput);
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

WeylGroup::WeylGroup()
{ this->flagFundamentalToSimpleMatricesAreComputed=false;
  this->flagOuterAutosComputed=false;
}

Rational WeylGroup::GetLongestRootLengthSquared()
{ Rational result;
  result=this->CartanSymmetric(0,0);
  for (int i=1; i<this->CartanSymmetric.NumRows; i++)
    result=MathRoutines::Maximum(result, this->CartanSymmetric(i,i));
  return result;
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

void WeylGroup::GetMatrixReflection(Vector<Rational>& reflectionRoot, Matrix<Rational>& output)
{ Vectors<Rational> basis;
  int theDim=this->GetDim();
  basis.MakeEiBasis(theDim);
//  output.init(theDim, theDim);
  for (int i=0; i<theDim; i++)
    this->ReflectBetaWRTAlpha(reflectionRoot, basis[i], false, basis[i]);
  output.AssignVectorsToRows(basis);
  output.Transpose();
}

void rootSubalgebra::GetCoxeterElement(Matrix<Rational>& output)
{ int theDim=this->GetAmbientWeyl().GetDim();
  output.MakeIdMatrix(theDim);
  Matrix<Rational>  tempMat;
  for (int i=0; i<this->SimpleBasisK.size; i++)
  { this->GetAmbientWeyl().GetMatrixReflection(this->SimpleBasisK[i], tempMat);
    output.MultiplyOnTheLeft(tempMat);
  }
}

void rootSubalgebra::GetCoxeterPlane
  (Vector<double>& outputBasis1, Vector<double>& outputBasis2, GlobalVariables& theGlobalVariables)
{ //this->ComputeRho(true);
  int theDimension=this->GetAmbientWeyl().GetDim();
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
  std::cout << "the SA Coxeter matrix: " << matCoxeterElt.ToString();
//  tempMat=matCoxeterElt;
  this->ComputeDynkinDiagramKandCentralizer();
  ReflectionSubgroupWeylGroup tempGroup;
  int coxeterNumber=1;
  for (int i=0; i<this->theDynkinDiagram.SimpleBasesConnectedComponents.size; i++)
  { tempGroup.AmbientWeyl=this->GetAmbientWeyl();
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
//  std::cout << "<br>coxeter transformation to the power of " << coxeterNumber << " equals: " << tempMat.ToString(true, false);
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
  List<Vector<CompleX<double> > > theEigenSpaceList;
  eigenMat.GetZeroEigenSpace(theEigenSpaceList);
  Vectors<CompleX<double> > theEigenSpace;
  theEigenSpace.operator=(theEigenSpaceList);
  DrawOperations tempDO;
  tempDO.initDimensions(theDimension, 1);
  for (int i=0; i<theDimension; i++)
    for (int j=0; j<theDimension; j++)
      tempDO.theBilinearForm.elements[i][j]=
      this->GetAmbientWeyl().CartanSymmetric.elements[i][j].DoubleValue();
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
//  std::cout << matCoxeterElt.ToString(true, false);
  tempMat=matCoxeterElt;
  int coxeterNumber=this->RootSystem.LastObject()->SumCoords().NumShort+1;
  for (int i=0; i<coxeterNumber-1; i++)
    tempMat.MultiplyOnTheLeft(matCoxeterElt);
//  std::cout << "<br>coxeter transformation to the power of " << coxeterNumber << " equals: " << tempMat.ToString(true, false);
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
  List<Vector<CompleX<double> > > theEigenSpaceList;
  eigenMat.GetZeroEigenSpace(theEigenSpaceList);
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
(DrawingVariables& outputDV, bool wipeCanvas, GlobalVariables& theGlobalVariables, bool drawWeylChamber, Vector<Rational>* bluePoint,
 bool LabelDynkinDiagramVertices, Vectors<Rational>* predefinedProjectionPlane)
{ DrawOperations& output=outputDV.theBuffer;
  if (wipeCanvas)
    output.init();
  int theDimension=this->GetDim();
  if(theDimension==1)
  { int color=CGI::RedGreenBlue(0, 255, 0);
    Vector<Rational> tempRoot, tempZero;
    tempZero.MakeZero(2);
    tempRoot.MakeEi(2, 0);
    for (int i=0; i<2; i++)
    { output.drawLineBetweenTwoVectorsBuffer(tempZero, tempRoot, DrawingVariables::PenStyleNormal, color);
      output.drawCircleAtVectorBuffer(tempRoot, 2, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(255,0,255));
      tempRoot.Minus();
    }
    return;
  }
  this->ComputeRho(true);
  Vector<Rational> ZeroRoot;
  ZeroRoot.MakeZero(theDimension);
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
//  std::cout << "<hr><hr>the eigenspace: " << theEigenSpace.ToString(false, true, false);
//  std::stringstream tempStream;
//  tempStream << "<hr>the eigen mat:";
//  tempStream << eigenMat;
//  std::cout << tempStream.str();
  Vectors<Rational> RootSystemSorted;
  RootSystemSorted=(this->RootSystem);
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
//  std::cout << "<hr>the min length is: " << minLength.ToString();
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
  this->GetEpsilonCoords(epsNotationSimpleBasis, epsNotationSimpleBasis);
  for (int i=0; i<theDimension; i++)
  { tempRootRat.MakeEi(theDimension, i);
    output.drawCircleAtVectorBuffer(tempRootRat, 1, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(255,0,0));
    output.drawCircleAtVectorBuffer(tempRootRat, 3, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(255,0,0));
    output.drawCircleAtVectorBuffer(tempRootRat, 4, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(255,0,0));
    if (LabelDynkinDiagramVertices)
    { Vector<Rational>& current=epsNotationSimpleBasis[i];
      output.drawTextAtVectorBuffer
      (tempRootRat, current.ToStringLetterFormat("e"),0, 10, DrawingVariables::TextStyleNormal);
    }
  }
  std::stringstream tempStream;
  tempStream << this->theDynkinType.GetWeylGroupName();
  if (this->GetDim()==2 && predefinedProjectionPlane!=0)
  { theTwoPlane[1][0]=1;
    theTwoPlane[1][1]=0;
    theTwoPlane[0][0]=0;
    theTwoPlane[0][1]=1;
    outputDV.theBuffer.ModifyToOrthonormalNoShiftSecond(theTwoPlane[0], theTwoPlane[1]);
  }
  output.drawTextBuffer(0, 0, tempStream.str(), 10, CGI::RedGreenBlue(0,0,0), DrawingVariables::TextStyleNormal);
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
  this->theVirtualOpS.ReservE(this->theVirtualOpS.size+other.BasisProjectionPlane.size);
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
  this->theVirtualOpS.ReservE(this->theVirtualOpS.size+numFrames);
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

std::string VirtualDrawOp::ToString()
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

std::string AnimationBuffer::ToString()
{ std::stringstream out;
  for (int i=0; i<this->theVirtualOpS.size; i++)
    out << "Frame " << i << ": " << this->theVirtualOpS[i].ToString() << "\n";
  return out.str();
}

void AnimationBuffer::operator+=(const AnimationBuffer& other)
{ if (other.theVirtualOpS.size<=0)
    return;
  int physicalOpShift=this->thePhysicalDrawOps.size;
  int physicalFrameShift=0;
  if (this->theVirtualOpS.size>0)
    physicalFrameShift=this->theVirtualOpS.LastObject()->indexPhysicalFrame+1;
  this->thePhysicalDrawOps.ReservE(this->thePhysicalDrawOps.size+other.thePhysicalDrawOps.size);
  for (int i=0; i<other.thePhysicalDrawOps.size; i++)
    this->thePhysicalDrawOps.AddOnTop(other.thePhysicalDrawOps[i]);
  this->theVirtualOpS.ReservE(this->theVirtualOpS.size+other.theVirtualOpS.size);
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
{ int indexCurrentFrame=-2;
  int numTotalPhysicalFrames=this->GetNumPhysicalFramesNoStillFrame();
  ProgressReport theReport(&theGlobalVariables);
  for (this->indexVirtualOp=0;  this->indexVirtualOp<this->theVirtualOpS.size; this->indexVirtualOp++)
    if (this->GetIndexCurrentPhysicalFrame()!=indexCurrentFrame)
    { indexCurrentFrame=this->GetIndexCurrentPhysicalFrame();
      std::stringstream tempStream;
      tempStream << "Computing frame " << indexCurrentFrame+1 << " out of "
      << numTotalPhysicalFrames << " physical frames.";
      theReport.Report(tempStream.str());
      theDrawingVariables.drawBufferNoIniT(this->GetCurrentDrawOps());
    }
  this->indexVirtualOp=0;
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
    out << "\nscaled vOrthogonal=" << vOrthogonal << "->"
    << this->theBilinearForm.ScalarProduct(vOrthogonal, vOrthogonal);
    out << "\nscaled vProjection=" << vProjection << "->"
    << this->theBilinearForm.ScalarProduct(vProjection, vProjection);
    out << "\ntheScalarProd: " << this->theBilinearForm.ScalarProduct(vOrthogonal, vProjection);
    this->RotateOutOfPlane(out, currentBasisPlane[0], currentBasisPlane[0], vProjection, vOrthogonal, oldRatioProjectionOverHeightSquared, newRatioProjectionOverHeightSquared);
    this->RotateOutOfPlane(out, currentBasisPlane[1], currentBasisPlane[1], vProjection, vOrthogonal, oldRatioProjectionOverHeightSquared, newRatioProjectionOverHeightSquared);
  }
//  this->e1.ComputeDebugString();
//  this->e2.ComputeDebugString();
  this->ModifyToOrthonormalNoShiftSecond(currentBasisPlane[0], currentBasisPlane[1]);
//  this->e1.ComputeDebugString();
//  this->e2.ComputeDebugString();
  out << "\ne1=" << currentBasisPlane[0];
  out << "\ne2=" << currentBasisPlane[1];
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
  std::string ToString(FormatExpressions* theFormat=0)const
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
  void operator/=(const ImpreciseDouble& other)
  { ImpreciseDouble copyMe;
    copyMe=*this;
    *this=copyMe/other;
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

void DrawOperations::projectionMultiplicityMergeOnBasisChange(DrawOperations& theOps, GlobalVariables& theGlobalVariables)
{ Matrix<ImpreciseDouble> theMat;
  int theDim=theOps.theBilinearForm.NumRows;
  theMat.init(theDim, 2);
//we assume that the ComputeProjectionsEiVectors has been called
//  theOps.ComputeProjectionsEiVectors();
  for(int i=0; i<theOps.ProjectionsEiVectors.size; i++)
    for (int j=0; j<2; j++)
      theMat.elements[i][j]=theOps.ProjectionsEiVectors[i][j];
  ProgressReport theReport(&theGlobalVariables);
  std::stringstream out;
  out << "before elimination:\n" << theMat.ToString();
  theMat.GaussianEliminationEuclideanDomain(0, ImpreciseDouble::GetMinusOne(), ImpreciseDouble::GetOne());
  out << "after elimination:\n" << theMat.ToString();
  theReport.Report(out.str());
}

std::string WeylGroup::GenerateWeightSupportMethoD1
(Vector<Rational>& highestWeightSimpleCoords, Vectors<Rational>& outputWeightsSimpleCoords,
 int upperBoundWeights, GlobalVariables& theGlobalVariables)
{ HashedList<Vector<Rational> > theDominantWeights;
  double upperBoundDouble=100000/this->theDynkinType.GetSizeWeylGroupByFormula().DoubleValue();
  int upperBoundInt = MathRoutines::Maximum((int) upperBoundDouble, 10000);
  //int upperBoundInt = 10000;
  Vector<Rational> highestWeightTrue=highestWeightSimpleCoords;
  this->RaiseToDominantWeight(highestWeightTrue);
  std::stringstream out;
  if (highestWeightTrue!=highestWeightSimpleCoords)
    out << "<br>Cheater! The input weight is not highest... using the highest weight in the same orbit instead. "
    << "Your input in simple coordinates was: "
    << highestWeightSimpleCoords.ToString() << ".<br> ";
  out << "The highest weight in simple coordinates is: " << highestWeightTrue.ToString() << ".<br>";
  std::string tempS;
  bool isTrimmed = !this->GetAlLDominantWeightsHWFDIM
  (highestWeightSimpleCoords, theDominantWeights, upperBoundInt, &tempS, &theGlobalVariables);

  out << tempS << "<br>";
  if (isTrimmed)
    out << "Trimmed the # of dominant weights - upper bound is " << upperBoundInt << ". <br>";
  else
    out << "Number of (non-strictly) dominant weights: " << theDominantWeights.size << "<br>";
  Vectors<Rational> tempRoots;
  HashedList<Vector<Rational> > finalWeights;
  int estimatedNumWeights=(int)
  (this->theDynkinType.GetSizeWeylGroupByFormula().DoubleValue()*theDominantWeights.size);
  estimatedNumWeights= MathRoutines::Minimum(10000, estimatedNumWeights);
  finalWeights.ReservE(estimatedNumWeights);
  finalWeights.SetHashSizE(estimatedNumWeights);
  Vectors<Rational> dominantWeightsNonHashed;
  dominantWeightsNonHashed=(theDominantWeights);
  this->GenerateOrbit(dominantWeightsNonHashed, false, finalWeights, false, 0, 0, 10000);
  if (finalWeights.size>=10000)
  { out << "Did not generate all weights of the module due to RAM limits. ";
    if (!isTrimmed)
      out << "However, all dominant weights were computed and are drawn.";
    out << "<br>";
  }
  if (!isTrimmed && finalWeights.size<10000)
    out << "All weights were computed and are drawn. <br>";
  outputWeightsSimpleCoords=(finalWeights);
  return out.str();
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
  this->IndexNthDrawOperation.ReservE
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

std::string ElementWeylGroup::ToString
  (int NumSimpleGens, FormatExpressions* theFormat, List<int>* DisplayIndicesOfSimpleRoots)const
{ if (this->size==0)
    return "id";
  std::string outerAutoLetter= "a";
  std::stringstream out;
  for (int i=this->size-1; i>=0; i--)
    if (NumSimpleGens<0 || (*this)[i]<NumSimpleGens)
    { out << "s_{";
      if (DisplayIndicesOfSimpleRoots==0)
        out << (*this)[i]+1;
      else
        out << (*DisplayIndicesOfSimpleRoots)[(*this)[i]];
      out << "}";
    } else
      out << outerAutoLetter << "_{" << (*this)[i]-NumSimpleGens+1 << "}";
  return out.str();
}

std::string ReflectionSubgroupWeylGroup::ElementToStringFromLayersAndArrows
(List<List<List<int> > >& arrows, List<List<int> >& Layers, int GraphWidth, bool useAmbientIndices)
{ std::stringstream out;
//  std::cout << this->simpleGenerators.ToString();
  List<int> DisplayIndicesSimpleGenerators;
  if (!useAmbientIndices)
  { DisplayIndicesSimpleGenerators.SetSize(this->simpleGenerators.size);
    for (int i=0; i<this->simpleGenerators.size; i++)
      DisplayIndicesSimpleGenerators[i]=this->AmbientWeyl.RootsOfBorel.GetIndex(this->simpleGenerators[i])+1;
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
        out << this->TheObjects[Layers[i][j]].ToString(0, &DisplayIndicesSimpleGenerators);
      else
        out << this->RepresentativesQuotientAmbientOrder[Layers[i][j]].ToString();
      int currentOffset=j+currentRowOffset;
      if (Layers[i].size%2==0)
        if (currentOffset>=GraphWidth/2)
          currentOffset++;
      for (int k=0; k<arrows[i][j].size; k++)
      { out << " \\ar[d";
        int indexInLayer=Layers[i+1].GetIndex(arrows[i][j][k]);
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
  Layers.ReservE(this->size);
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
  orbit.ReservE(this->size);
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

void ReflectionSubgroupWeylGroup::ToString(std::string& output, bool displayElements)
{ std::stringstream out, head, head2;
  List<int> DisplayIndicesSimpleGenerators;
  DisplayIndicesSimpleGenerators.SetSize(this->simpleGenerators.size);
  FormatExpressions latexFormat;
  latexFormat.flagUseHTML=false;
  latexFormat.flagUseLatex=true;
  bool isGood=true;
  for (int i=0; i<this->simpleGenerators.size; i++)
  { DisplayIndicesSimpleGenerators[i]=this->AmbientWeyl.RootsOfBorel.GetIndex(this->simpleGenerators[i])+1;
    if (DisplayIndicesSimpleGenerators[i]==0)
    { isGood=false;
      break;
    }
  }
  if (!isGood)
    for (int i=0; i<this->simpleGenerators.size; i++)
      DisplayIndicesSimpleGenerators[i]=i+1;
  DynkinDiagramRootSubalgebra tempDyn;
  tempDyn.ComputeDiagramTypeKeepInput(this->simpleGenerators, this->AmbientWeyl.CartanSymmetric);
  out << "Dynkin diagram & subalgebra of root subsystem generated by the given root: "
  << tempDyn.ToStringRelativeToAmbientType(this->AmbientWeyl.theDynkinType[0]);
  out << "<br>Simple roots:\n<br>\n ";
  head << "\\begin{array}{rcl}";
  for (int i=0; i<this->simpleGenerators.size; i++)
    head << "\n\\eta_{" << DisplayIndicesSimpleGenerators[i]
    << "}&:=&" << this->simpleGenerators[i].ToString()
    << "\\\\";
  head << "\\end{array}";
  out << CGI::GetHtmlMathSpanFromLatexFormula(head.str());
  if (this->ExternalAutomorphisms.size>0)
  { out << "<br>Outer automorphisms: \n";
    Matrix<Rational>  tempMat;
    head2 << "\\begin{array}{rcl}";
    for (int i=0; i<this->ExternalAutomorphisms.size; i++)
    { tempMat.AssignVectorsToRows(this->ExternalAutomorphisms[i]);
      tempMat.Transpose();
      head2 << "a_{" << i+1 << "}&:=&" << tempMat.ToString(&latexFormat) << "\\\\";
    }
    head2 << "\\end{array}";
    out << CGI::GetHtmlMathDivFromLatexFormulA(head2.str());
  }
  out << "<br>Half sum of the positive roots: " << this->GetRho().ToString();
  out << "<br>Roots of Borel (" << this->RootsOfBorel.size << " total): ";
  for (int i=0; i<this->RootsOfBorel.size; i++)
  { out << this->RootsOfBorel[i].ToString();
  }
  if (displayElements)
  { std::stringstream body;
    out << "<br>The elements of the weyl group of the subgroup written with minimal # of generators:<br>";
    body << "\\begin{array}{l}";
    for (int i=0; i<this->size; i++)
    { const ElementWeylGroup& currentElt=(*this)[i];
      body << currentElt.ToString(this->simpleGenerators.size, 0, &DisplayIndicesSimpleGenerators)
      << "\\\\";
    }
    body << "\\end{array}";
    out << CGI::GetHtmlMathSpanFromLatexFormula(body.str());
  }
  output=out.str();
}

void RationalFunctionOld::Substitution(const PolynomialSubstitution<Rational>& theSub)
{ if (theSub.size<1)
    return;
//  FormatExpressions tempFormat;
//  int commentMEWhenDone;
  switch(this->expressionType)
  { case RationalFunctionOld::typeRational:
//      assert(this->checkConsistency());
      return;
    case RationalFunctionOld::typePoly:
//      std::cout <<"<hr>subbing in<br>" << this->ToString(tempFormat) << " using " << theSub.ToString()
//      << " to get ";
      if (!this->Numerator.GetElement().Substitution(theSub, 1))
      { std::cout << "This is a programming error: "
        << " substitution   " << theSub.ToString() << " into polynomial "
        << this->Numerator.GetElement().ToString()
        << " failed but the current function does not handle this properly. "
        << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
        assert(false);
      }
//      std::cout << "<br>finally:<br>" << this->Numerator.GetElement().ToString();
      this->Simplify();
//      std::cout << ", which, simplified, yields<br> " << this->ToString(tempFormat);
//      assert(this->checkConsistency());
      return;
    case RationalFunctionOld::typeRationalFunction:
      if (!this->Numerator.GetElement().Substitution(theSub, 1))
      { std::cout << "This is a programming error: "
        << " substitution   " << theSub.ToString() << " into polynomial "
        << this->Numerator.GetElement().ToString()
        << " failed but the current function does not handle this properly. "
        << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
        assert(false);
      }
      if (!this->Denominator.GetElement().Substitution(theSub, 1))
      { std::cout << "This is a programming error: "
        << " substitution   " << theSub.ToString() << " into polynomial "
        << this->Denominator.GetElement().ToString()
        << " failed but the current function does not handle this properly. "
        << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
        assert(false);
      }
      this->Simplify();
//      assert(this->checkConsistency());
      return;
    default:
      assert(false);
      break;
  }
}
