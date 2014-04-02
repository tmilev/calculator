//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader1General0_General.h"
#include "vpfHeader2Math0_General.h"
#include "vpfImplementationHeader2Math051_PolynomialComputations_Basic.h"
#include "vpfImplementationHeader2Math052_PolynomialComputations_Advanced.h"

ProjectInformationInstance ProjectInfoVpf8cpp(__FILE__, "Math routines implementation. ");


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
      crash << "This is a programming error: LargeIntUnsigned::AssignString" << " called on the string " << input
      << " which does not consist entirely of digits. Please note that LargeIntUnsigned::AssignString is a no-fail function, intended for "
      << " internal use only. If you want to parse arbitrary unsafe expressions coming from the user, please use the big gun (a.k.a. Calculator). "
      << crash;
    this->operator+=((unsigned) whichDigit);
  }
}

void Rational::AssignString(const std::string& input)
{ this->MakeZero();
  if (input=="0")
    return;
  int sign=1;
  int ReaderDigit=-1;
  Rational readerDen=1;
  int positionInString=0;
  for (; positionInString<(signed)input.size(); positionInString++)
  { if (input[positionInString]=='-')
    { sign=-1;
      positionInString++;
      break;
    }
    if (MathRoutines::isADigit(input[positionInString]))
      break;
  }
  for (; positionInString<(signed)input.size(); positionInString++)
  { if (MathRoutines::isADigit(input[positionInString], &ReaderDigit))
    { *this*=10;
      *this+=ReaderDigit;
    }
    if (input[positionInString]=='/')
    { positionInString++;
      readerDen=0;
      break;
    }
  }
  for (; positionInString<(signed)input.size(); positionInString++)
    if (MathRoutines::isADigit(input[positionInString], &ReaderDigit))
    { readerDen*=10;
      readerDen+=ReaderDigit;
    }
  if (readerDen==0)
    crash << "Programming error: Rational::AssignString encountered a zero denominator. " << crash;
  *this/=readerDen;
  *this*=sign;
}

void Selection::operator=(const Vector<Rational>& other)
{ this->init(other.size);
  for (int i=0; i<other.size; i++)
    if (!other[i].IsEqualToZero())
      this->selected[i]=true;
  this->ComputeIndicesFromSelection();
}

void ConeComplex::InitFromAffineDirectionsAndRefine(Vectors<Rational>& inputDirections, Vectors<Rational>& inputAffinePoints, GlobalVariables& theGlobalVariables)
{ if(inputDirections.size!=inputAffinePoints.size || inputDirections.size<=0)
    crash << crash;
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

void ConeComplex::MakeAffineAndTransformToProjectiveDimPlusOne(Vector<Rational>& affinePoint, ConeComplex& output, GlobalVariables& theGlobalVariables)
{ if(&output==this)
    crash << crash;
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

void Lattice::GetRootOnLatticeSmallestPositiveProportionalTo(Vector<Rational>& input, Vector<Rational>& output, GlobalVariables& theGlobalVariables)
{ if(&input==&output)
    crash << crash;
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
  if(!tempLattice.basisRationalForm.NumRows==1)crash << crash;
  tempLattice.basisRationalForm.RowToRoot(0, output);
  Rational tempRat;
  bool tempBool=  output.IsProportionalTo(input, tempRat);
  if(!tempBool)crash << crash;
  if (tempRat.IsNegative())
    output.Minus();*/
}

bool Cone::GetLatticePointsInCone
(Lattice& theLattice, Vector<Rational>& theShift, int upperBoundPointsInEachDim, bool lastCoordinateIsOne, Vectors<Rational>& outputPoints, Vector<Rational>* shiftAllPointsBy)const
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

bool PiecewiseQuasipolynomial::MakeVPF(Vectors<Rational>& theRoots, std::string& outputstring, GlobalVariables& theGlobalVariables)
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
  out << CGI::GetMathMouseHover(theFracs.ToString(theGlobalVariables, theFormat));
  theFracs.split(theGlobalVariables, 0);
  out << CGI::GetMathMouseHover(theFracs.ToString(theGlobalVariables, theFormat));
  //theFracs.theChambers.InitFromDirectionsAndRefine(theRoots, theGlobalVariables);
  crash << crash ;
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
//  stOutput << "shiftRoot: " << shiftRoot.ToString();
  theFracs.theChambers.MakeAffineAndTransformToProjectiveDimPlusOne
  (shiftRoot, this->theProjectivizedComplex, theGlobalVariables);
  outputstring=out.str();
  return true;
}

bool Lattice::GetInternalPointInConeForSomeFundamentalDomain(Vector<Rational>& output, Cone& coneContainingOutputPoint, GlobalVariables& theGlobalVariables)
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
{ if(theTranslationVector.size!=this->GetDim()-1)
    crash << crash;
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
{ if(!this->theBuffers!=0)
    crash << crash;
  Vector<Rational> chamberShift=-translateToBeAddedToArgument;
//  stOutput << "the translation: " << translateToBeAddedToArgument.ToString();
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
      out << CGI::GetMathSpanBeginArrayL(currentQP.ToString(useHtml, useLatex));
    else
      out << currentQP.ToString(useHtml, useLatex);
    if (useHtml)
      out << "<hr>";
  }
  return out.str();
}

void PiecewiseQuasipolynomial::DrawMe(DrawingVariables& theDrawingVars, int numLatticePointsPerDim, Cone* RestrictingChamber, Vector<Rational>* distinguishedPoint)
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
{ if(input.size!=this->theProjectivizedComplex.GetDim()-1)
    crash << crash;
  Vector<Rational> ProjectivizedInput=input;
  ProjectivizedInput.SetSize(input.size+1);
  *ProjectivizedInput.LastObject()=1;
  return this->EvaluateInputProjectivized(ProjectivizedInput);
}

Rational PiecewiseQuasipolynomial::EvaluateInputProjectivized(const Vector<Rational>& input)
{ Rational result;
  if(input.size!=this->theProjectivizedComplex.GetDim())
    crash << crash;
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
        stOutput << "<hr>Error!!! Failed on chamber " << theIndex+1 << " and " << i+1;
        stOutput << "<br>Evaluating at point " << AffineInput.ToString() << "<br>";
        stOutput << "<br>Chamber " << theIndex+1 << ": " << this->theProjectivizedComplex[theIndex].ToString(&tempFormat);
        stOutput << "<br>QP: " << this->theQPs[theIndex].ToString(true, false);
        stOutput << "<br>value: " << result.ToString();
        stOutput << "<br><br>Chamber " << i+1 << ": " << this->theProjectivizedComplex[i].ToString(&tempFormat);
        stOutput << "<br>QP: " << this->theQPs[i].ToString(true, false);
        stOutput << "<br>value: " << altResult.ToString();
        if (firstFail)
        { DrawingVariables tempDV;
          stOutput << "<br><b>Point of failure: " << AffineInput.ToString() << "</b>";
          //this->DrawMe(tempDV);
          this->theProjectivizedComplex.DrawMeLastCoordAffine(true, tempDV, tempFormat);
          tempDV.NumHtmlGraphics=5;
          tempDV.theBuffer.drawCircleAtVectorBuffer(AffineInput, 5, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(0,0,0));
          tempDV.theBuffer.drawCircleAtVectorBuffer(AffineInput, 10, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(0,0,0));
          tempDV.theBuffer.drawCircleAtVectorBuffer(AffineInput, 4, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(255,0,0));
          stOutput << "<br> <script src=\"http://ajax.googleapis.com/ajax/libs/dojo/1.6.1/dojo/dojo.xd.js\" type=\"text/javascript\"></script>\n";
          stOutput << tempDV.GetHtmlFromDrawOperationsCreateDivWithUniqueName(this->theProjectivizedComplex.GetDim()-1);

        }
        firstFail=false;
      }
//      if(result!=altResult) crash << crash;
    }
  return result;
}

void PiecewiseQuasipolynomial::MakeCommonRefinement(const ConeComplex& other)
{ if(this->theBuffers==0)
    crash << crash;
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

void Lattice::GetDefaultFundamentalDomainInternalPoint(Vector<Rational>& output)
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
//  stOutput << "<br>checksum start: " << this->StartCheckSum.ToString();
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
//  stOutput << "<br>checksum finish: " << this->EndCheckSum.ToString();

  return false;
}

void Cone::ChangeBasis(Matrix<Rational>& theLinearMap, GlobalVariables& theGlobalVariables)
{ //Vectors<Rational> newNormals;
//  Matrix<Rational> tempMat=theLinearMap;
  theLinearMap.ActOnVectorsColumn(this->Normals);
  this->CreateFromNormals(this->Normals, &theGlobalVariables);
}

void Cone::TransformToWeylProjective(ConeComplex& owner, GlobalVariables& theGlobalVariables)
{ crash << crash;
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

void ConeComplex::TransformToWeylProjective(GlobalVariables& theGlobalVariables)
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
  //stOutput <<output.ToString(false, true);
  output.Refine(theGlobalVariables);
  //stOutput <<output.ToString(false, true);
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
{ //stOutput << "<hr>starting complex: " << this->ToString();
  FormatExpressions tempFormat;
  ProgressReport theReport1(&theGlobalVariables);
  ProgressReport theReport2(&theGlobalVariables);
  ProgressReport theReport3(&theGlobalVariables);
  for (; this->numProcessedNonParam<this->numNonParaM; this->numProcessedNonParam++)
  { while(this->theConesLargerDim.size>0)
    { ConeLatticeAndShift& currentCLS=*this->theConesLargerDim.LastObject();
      if(this->LPtoMaximizeLargerDim.LastObject()->size!=currentCLS.GetDimAffine()+1)
        crash << crash;
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
    //stOutput << "<hr><hr>" << this->ToString();
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
  theProjectionLatticeLevel.MakeZero();
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
//  stOutput << "<hr>complex before refining: <br>\n" << complexBeforeProjection.ToString(false, true);
  complexBeforeProjection.Refine(theGlobalVariables);
//  stOutput << "<hr>complex before projection: <br>\n" << complexBeforeProjection.ToString(false, true);
  Vector<Rational> tempRoot, extraEquation, exitNormalAffine, enteringNormalAffine, exitNormalLatticeLevel, enteringNormalLatticeLevel, exitNormalShiftedAffineProjected;
  Vector<Rational> directionSmallerDim, directionSmallerDimOnLattice;
  Vector<Rational> theShiftReduced=this->theShift;
  this->theLattice.ReduceVector(theShiftReduced);
  Vectors<Rational> exitRepresentatives, exitWallsShifted;
  Vectors<Rational> currentShifts;
  Lattice exitRougherLattice;
  ConeLatticeAndShift tempCLS;
  directionSmallerDim.MakeEi(theDimProjectivized-1, 0);
//  stOutput << "<hr>";
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
      { //stOutput << "<hr>";
        //stOutput << "<br>currentWall: " << currentNormal.ToString();
        numNonPerpWalls++;
        if(numNonPerpWalls>=3)
          crash << crash;
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
    stOutput << "<hr><hr><hr>" << currentCone.ToString(&theFormat);
    stOutput << "<br>Entering normal: " << ((foundEnteringNormal) ? enteringNormalAffine.ToString() : "not found");
    stOutput << "<br>Exit normal: " << ((foundExitNormal) ? exitNormalAffine.ToString() : "not found");
    stOutput << "<br>The shifted lattice representatives: " << exitRepresentatives.ToString() << "<br>exitNormalsShiftedAffineProjected";
    if(theNewNormals.size<=0)
      crash << crash;
    for (int j=0; j<exitRepresentatives.size; j++)
    { tempCLS.theProjectivizedCone.Normals=theNewNormals;
      exitNormalShiftedAffineProjected=exitNormalAffine.GetShiftToTheLeftOnePosition();
      *exitNormalShiftedAffineProjected.LastObject()=-exitNormalLatticeLevel.ScalarEuclidean(exitRepresentatives[j]);
      stOutput << exitNormalShiftedAffineProjected.ToString() << ", ";
      if (foundEnteringNormal)
      { extraEquation= enteringNormalAffine.GetShiftToTheLeftOnePosition();
        extraEquation-=(exitNormalShiftedAffineProjected*enteringNormalAffine[0])/exitNormalAffine[0];
        stOutput << "extra equation: " << extraEquation.ToString() << ", ";
        tempCLS.theProjectivizedCone.Normals.AddOnTop(extraEquation);
      }
      Rational tempRat=theLPToMaximizeAffine[0];
      tempRoot=theLPToMaximizeAffine.GetShiftToTheLeftOnePosition();
      tempRoot-=exitNormalShiftedAffineProjected*theLPToMaximizeAffine[0]/exitNormalAffine[0];
      outputAppendLPToMaximizeAffine.AddOnTop(tempRoot);
      if(tempCLS.theProjectivizedCone.Normals.size<=0)
        crash << crash;
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
      //if(!tempBool)crash << crash;
      //stOutput << tempCLS.theProjectivizedCone.ToString(false, true, true, true, theFormat);
      if (!tempCLS.theProjectivizedCone.flagIsTheZeroCone)
      { theProjectionLatticeLevel.ActOnVectorColumn(exitRepresentatives[j], tempCLS.theShift);
        outputAppend.AddOnTop(tempCLS);
        if (tempCLS.GetDimProjectivized()==0)
        { theReport.Report(tempTempRoots.ToString());
          while(true) {}
        }
        if(tempCLS.GetDimProjectivized()!=theDimProjectivized-1)
          crash << crash;
      }
    }
  }
//  stOutput << "<hr><hr><hr>";
//  for (int i=0; i<outputAppend.size; i++)
//  { stOutput << outputAppend[i].theProjectivizedCone.ToString(false, true, true, true);
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
  Cone newPlusCone, newMinusCone;
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
  if(tempCone.Vertices.size!=newPlusCone.Vertices.size)crash << crash;
  tempCone.CreateFromNormals(newMinusCone.Normals, theGlobalVariables);
  if(tempCone.Vertices.size!=newMinusCone.Vertices.size)crash << crash;
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
//      stOutput << "<br>Normals to the subspace spanned by the vertices: " << NormalsToSubspace.ToString();
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
      crash << crash;
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
 // stOutput << inputVertices.ToString();
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
//  stOutput << "<br>Candidate normals: " << this->Normals.ToString();
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
//  stOutput << "<br>Normals (" << inputNormals.size << " input " << numAddedFakeWalls << " fake): " << this->Normals.ToString();
  this->ComputeVerticesFromNormalsNoFakeVertices(theGlobalVariables);
//  stOutput << "<br>Vertices before adding minus vertices: " << this->Vertices.ToString();
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
//  stOutput << "<br>Vertices: " << this->Vertices.ToString();
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
    default: //this should never happen! maybe crash << crash here...
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
//  stOutput << currentPoly.ToString(false, tempFormat) << "<br>";
  }
  GroebnerBasisComputation<Rational> theComputation;
  theComputation.thePolynomialOrder.theMonOrder=MonomialP::LeftIsGEQLexicographicLastVariableWeakest;
  theComputation.TransformToReducedGroebnerBasis(theGroebnerBasis, &theGlobalVariables);
//  stOutput << "<br>the ending generators:<br> ";
//  for (int i=0; i<theGroebnerBasis.size; i++)
//  { stOutput << theGroebnerBasis.TheObjects[i].ToString(false, tempFormat) << "<br>";
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
    { stOutput << "You messed up the total degree which must be one, instead it is "
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
  stOutput << this->ToString(false, true);
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
  { //stOutput << "<br>Chamber " << i+1 << " maximum linear function is the function of index " << outputMaximumOverEeachSubChamber.TheObjects[i] << ": " << inputLinPolys.TheObjects[outputMaximumOverEeachSubChamber.TheObjects[i]].ToString();
    //stOutput << "<br>The chamber is given by: " << this->TheObjects[i].ToString(false, true);
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
//    stOutput << "<br>basis rational form: " << this->basisRationalForm.ToString(true, false);
//    out << "<br>basis rational form: " << this->basisRationalForm.ToString(true, false);
//    std::string tempS=out.str();
//    if(testMatrix!=this->basisRationalForm)crash << crash;
//  }
}

void Lattice::TestGaussianEliminationEuclideanDomainRationals(Matrix<Rational> & output)
{ output.AssignMatrixIntWithDen(this->basis, this->Den);
  std::stringstream out;
  stOutput << "Test output: " << output.ToString();
  out << "Test output: " << output.ToString();
  output.GaussianEliminationEuclideanDomain();
  stOutput << "<br>After gaussian elimination:" << output.ToString();
  out << "<br>After gaussian elimination:" << output.ToString();
//  tempDebugString=out.str();
}

void Lattice::RefineByOtherLattice(const Lattice& other)
{ if (other.basis.NumCols==0)
    return;
  if (other.basis==this->basis && this->Den==other.Den)
    return;
  if(other.GetDim()!=this->GetDim())
    crash << crash;
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
{ //stOutput << "<br>remainder by 2 is " << ((*this)%2).ToString()
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
//    stOutput << "Dividing " << this->ToString() << " by " << x.ToString() << " yields quotient " << quotientOutput.ToString()
//    << " and remainder " << remainderOutput.ToString();
    return;
  }
  LargeIntUnsigned remainder, quotient, divisor;
  remainder=(*this);
  divisor=(x);
  if(divisor.IsEqualToZero())
    crash << crash;
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
//  stOutput << "Dividing " << this->ToString() << " by " << x.ToString() << " yields quotient " << quotientOutput.ToString()
//  << " and remainder " << remainderOutput.ToString();
//  if(!remainderOut.CheckForConsistensy())crash << crash;
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
    currentPeriodInt=currentPeriod.GetNumerator();
    if (currentPeriodInt.value.theDigits.size>1)
//    { stOutput << "exited at point 1 counter i is " << i;
      return false;
//    }
    else
      thePeriods[i]=currentPeriodInt.value.theDigits[0];
    this->basisRationalForm.GetVectorFromRow(i, thePeriodVectors[i]);
    rougherLattice.basisRationalForm.GetVectorFromRow(i, tempRoot2);
    tempRoot=thePeriodVectors[i];
    tempRoot*=thePeriods[i];

//    if (!(tempRoot-tempRoot2).IsEqualToZero())
//    { stOutput << "exited at point 2 counter i is " << i << " the period vector=" << thePeriodVectors.TheObjects[i].ToString() << " rougher lattice basis vector: " << tempRoot2.ToString();
//      return false;
//    }
  }
//  stOutput << thePeriodVectors.ToString() << "<br>The periods: ";
//  for (int i=0; i<thePeriods.size; i++)
//    stOutput << thePeriods.TheObjects[i] << ", ";
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
//  stOutput << "The representatives: " << output.ToString(false, true, false);
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
  stOutput << "<br>the movement: " << theMovement.ToString() << ", (" << theShift.ToString() << " - " << startingPoint.ScalarEuclidean(theNormal).ToString() << ")/ " << theDirection.ScalarEuclidean(theNormal).ToString() << ", ";
  if (!theMovement.IsInteger())
  { stOutput << "the movement aint integral; ";
    theMovement.AssignFloor();
    if (theDirection.ScalarEuclidean(theNormal).IsPositive())
      theMovement+=1;
  }
  stOutput << "the normal: " << theNormal.ToString() << ", the direction: " << theDirection.ToString() << ", the shift: " << theShift.ToString()
  << ", the movement: " << theMovement.ToString() << ", startingPoint: " << startingPoint.ToString();
  outputPoint=startingPoint;
  outputPoint+=(theDirection*theMovement);
  stOutput << ", finalPoint: " << outputPoint.ToString();
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
  //stOutput << "roughening " << this->AmbientLatticeReduced.ToString() << "by" << latticeToRoughenBy.ToString();
  Lattice OldLattice;
  OldLattice=this->AmbientLatticeReduced;
  this->AmbientLatticeReduced.IntersectWith(latticeToRoughenBy);
  Vectors<Rational> representativesQuotientLattice;
  //stOutput << "getting all representatives of " << OldLattice.ToString() << "inside" << this->AmbientLatticeReduced.ToString();
  //stOutput << "<br><br><br><br>*********<br><br><br><br>";
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
  //stOutput << "<hr>acting by " << this->basisRationalForm.ToString(true, false) << " on " << output.ToString();
  Matrix<Rational> tempMat;
  tempMat=this->basisRationalForm;
  tempMat.Transpose();
  tempMat.ActOnVectorsColumn(output);
  //stOutput << "<br> to get " << output.ToString();
  return result;
}

void Lattice::IntersectWithPreimageOfLattice(const Matrix<Rational> & theLinearMap, const Lattice& other, GlobalVariables& theGlobalVariables)
{ Vectors<Rational> startingBasis, imageStartingBasis, basisImageIntersection, basisImageIntersectionInCoordsWRTimageStartingBasis, ImageBasisInImageStartingBasisCoords;
  Vectors<Rational> resultNonKernelPart, resultKernelPart, result, tempRoots;
  startingBasis.AssignMatrixRows(this->basisRationalForm);
  //stOutput << "<br>Starting basis: " << startingBasis.ToString();
  //stOutput << "<br>The linear map: " << theLinearMap.ToString(true, false);
  theLinearMap.ActOnVectorsColumn(startingBasis, imageStartingBasis);
  //stOutput << "<br>Image of starting basis: " << imageStartingBasis.ToString();
  Lattice ImageLattice;
  ImageLattice.MakeFromRoots(imageStartingBasis);
  ImageLattice.IntersectWith(other);
  //stOutput << "<br>Image lattice: " << ImageLattice.ToString(true, false);
  basisImageIntersection.AssignMatrixRows(ImageLattice.basisRationalForm);
  Vectors<Rational> tempBasisImageIntersection, tempImageStartingBasis, tempImageBasisInImageStartingBasisCoords;
  basisImageIntersection=(tempBasisImageIntersection);
  imageStartingBasis=(tempImageStartingBasis);
  bool tempBool= tempBasisImageIntersection.GetIntegralCoordsInBasisIfTheyExist(tempImageStartingBasis, tempImageBasisInImageStartingBasisCoords, (Rational) 1, (Rational) -1, (Rational) 0);
  ImageBasisInImageStartingBasisCoords=(tempImageBasisInImageStartingBasisCoords);
  if(!tempBool)
    crash << crash;
  //stOutput << "<br> Basis of image lattice expressed in coordinates with respect to image of starting basis: " << ImageBasisInImageStartingBasisCoords.ToString();
  resultNonKernelPart.SetSize(ImageBasisInImageStartingBasisCoords.size);
  for (int i=0; i<resultNonKernelPart.size; i++)
  { Vector<Rational>& currentRoot=resultNonKernelPart[i];
    currentRoot.MakeZero(this->GetDim());
    for (int j=0; j<startingBasis.size; j++)
      currentRoot+=startingBasis[j]*ImageBasisInImageStartingBasisCoords[i][j];
  }
  //stOutput << "<br> Result non-kernel part (representatives): " << resultNonKernelPart.ToString();
  Lattice KernelPart;
  KernelPart=*this;
  tempRoots.AssignMatrixRows(theLinearMap);
  KernelPart.IntersectWithLinearSubspaceGivenByNormals(tempRoots);
  //stOutput << "<br>Kernel part of the result: " << KernelPart.ToString(true, false);
  result.AssignMatrixRows(KernelPart.basisRationalForm);
  result.AddListOnTop(resultNonKernelPart);
  this->MakeFromRoots(result);
  //stOutput << "<br> And the result is.... " << this->ToString(true, false);
}

void Lattice::IntersectWith(const Lattice& other)
{ Vectors<Rational> commonBasis, otherBasis, startBasis;
  Vectors<Rational> bufferVect;
  Matrix<Rational> bufferMat;
  startBasis.AssignMatrixRows(this->basisRationalForm);
  otherBasis.AssignMatrixRows(other.basisRationalForm);
//  stOutput << "<br>this basis: " << startBasis.ToString();
//  stOutput << "<br>other basis: " << otherBasis.ToString();
  startBasis.IntersectTwoLinSpaces(startBasis, otherBasis, commonBasis);
//  stOutput << "<br> basis of linear space intersection: " << commonBasis.ToString() << "<br><br> ";
  Lattice thisLatticeIntersected, otherLatticeIntersected;
  thisLatticeIntersected=*this;
  otherLatticeIntersected=other;
  thisLatticeIntersected.IntersectWithLinearSubspaceSpannedBy(commonBasis);
//  stOutput << "<br> linear space intersected with this lattice: " << thisLatticeIntersected.ToString(true, false);
  otherLatticeIntersected.IntersectWithLinearSubspaceSpannedBy(commonBasis);
//  stOutput << "<br> linear space intersected with other lattice: " << otherLatticeIntersected.ToString(true, false);
  Vectors<Rational> thisCommonBasis, otherCommonBasis, thisCommonCoords, otherCommonCoords;
  thisCommonBasis.AssignMatrixRows(thisLatticeIntersected.basisRationalForm);
  otherCommonBasis.AssignMatrixRows(otherLatticeIntersected.basisRationalForm);
  thisCommonBasis.GetCoordsInBasis(commonBasis, thisCommonCoords, bufferVect, bufferMat);
  otherCommonBasis.GetCoordsInBasis(commonBasis, otherCommonCoords, bufferVect, bufferMat);
//  stOutput << "<br>this lattice intersection new coords: " << thisCommonBasis.ToString();
//  stOutput << "<br>other lattice intersection new coords: " << otherBasis.ToString();
  Lattice thisCommonCoordsLattice, otherCommonCoordsLattice;
  thisCommonCoordsLattice.MakeFromRoots(thisCommonCoords);
  otherCommonCoordsLattice.MakeFromRoots(otherCommonCoords);
//  stOutput << "<br> linear space intersected with this lattice new coords: " << thisCommonCoordsLattice.ToString(true, false);
//  stOutput << "<br> linear space intersected with other lattice new coords: " << otherCommonCoordsLattice.ToString(true, false);
  thisCommonCoordsLattice.IntersectWithBothOfMaxRank(otherCommonCoordsLattice);
//  stOutput << "<br> intersection lattice new coords: " << thisCommonCoordsLattice.ToString(true, false);
  Vectors<Rational> resultBasis;
  resultBasis.SetSize(thisCommonCoordsLattice.basisRationalForm.NumRows);
  for (int i=0; i<resultBasis.size; i++)
  { Vector<Rational>& currentRoot=resultBasis.TheObjects[i];
    currentRoot.MakeZero(this->GetDim());
    for (int j=0; j<thisCommonCoordsLattice.basisRationalForm.NumCols; j++)
      currentRoot+=commonBasis[j]*thisCommonCoordsLattice.basisRationalForm.elements[i][j];
  }
  this->MakeFromRoots(resultBasis);
//  stOutput << "<br>final answer: intersection is: " << this->ToString(true, false);
}

void Lattice::IntersectWithBothOfMaxRank(const Lattice& other)
{ Lattice dualLatticeThis, dualLatticeOther;
  //stOutput << "intersecting " << this->ToString() << " and " << other.ToString();
  if(this->basis.NumRows!=this->GetDim() || this->GetDim()!=other.GetDim() || other.basis.NumRows!= this->GetDim())
    crash << crash;
  this->GetDualLattice(dualLatticeThis);
  //stOutput << "<br>dual lattice of left: " << dualLatticeThis.ToString();
  other.GetDualLattice(dualLatticeOther);
  //stOutput << "<br>dual lattice of right: " << dualLatticeOther.ToString();
  dualLatticeThis.RefineByOtherLattice(dualLatticeOther);
  //stOutput << "<br> common refinement of dual lattice: " << dualLatticeThis.ToString();
  dualLatticeThis.GetDualLattice(*this);
  //stOutput << "<br>final result: " << this->ToString();
}

void QuasiPolynomial::operator+=(const QuasiPolynomial& other)
{ //stOutput << "current ambient lattice: " << this->AmbientLatticeReduced.ToString();
  this->MakeRougherLattice(other.AmbientLatticeReduced);
  //stOutput << "roughened: " << this->AmbientLatticeReduced.ToString();
  QuasiPolynomial tempQP=other;
  //stOutput << "<br><br>other ambient lattice: " << this->AmbientLatticeReduced.ToString();
  tempQP.MakeRougherLattice(this->AmbientLatticeReduced);
  //stOutput << "roughened: " << this->AmbientLatticeReduced.ToString() << "<br><br><br><br>*******<br><br><br>";
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
  if(theVector.size!=this->GetDim())
    crash << crash;
  Vectors<Rational> basisRoots;
  basisRoots.AssignMatrixRows(this->basisRationalForm);
  //stOutput <<  "the basis: " << basisRoots.ToString();
  if (!theVector.GetCoordsInBasiS(basisRoots, output))
  { crash << "oops bad!" << crash;
    return false;
  }
  for (int i=0; i<output.size; i++)
    output[i].AssignFracValue();
  theVector.MakeZero(theVector.size);
  for (int i=0; i<basisRoots.size; i++)
    theVector+=basisRoots[i]*output[i];
  //stOutput << "the vector " << theVector.ToString() << " in the basis " << basisRoots.ToString() << " has coordinates: " << output.ToString();
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
{ if(theDim<=0)
    crash << crash;
  this->AmbientLatticeReduced.MakeZn(theDim);
  this->LatticeShifts.size=0;
  this->valueOnEachLatticeShift.size=0;
}

void PartFraction::GetRootsFromDenominator(PartFractions& owner, Vectors<Rational>& output)const
{ output.SetSize(this->IndicesNonZeroMults.size);
  for (int i=0; i<this->IndicesNonZeroMults.size; i++)
  { output[i]=owner.startingVectors[this->IndicesNonZeroMults[i]];
    oneFracWithMultiplicitiesAndElongations& current=this->TheObjects[this->IndicesNonZeroMults[i]];
    if(current.Elongations.size!=1)
      crash << crash;
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
//  stOutput << "<hr><hr> the lattice generators: " << theLatticeGenerators.ToString();
//  stOutput << "<br>Corresponding lattice: " << theLattice.ToString();
  Matrix<Rational>  theNormalsMatForm;
  theNormalsMatForm.AssignVectorsToRows(theLatticeGenerators);
  theNormalsMatForm.Invert();
  theNormals.AssignMatrixColumns(theNormalsMatForm);
  output.MakeZeroLatTiceZn(owner.AmbientDimension);

  for (int i=0; i<theCoeff.size(); i++)
  { this->ComputePolyCorrespondingToOneMonomial(shiftedPoly, theCoeff[i], theNormals, theLattice, theGlobalVariables);
//    output.ComputeDebugString();
//    shiftedPoly.ComputeDebugString();
//    stOutput << "<hr>Current fraction monomial " << i+1 << " out of " << this->Coefficient.size;
//    stOutput << "<br>Contribution: " << shiftedPoly.ToString(true, false);
    shiftedPoly*=theCoeff.theCoeffs[i];
    output+=shiftedPoly;
//    stOutput << "<br>Accumulator: " << output.ToString(true, false);
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
//    if(tempLRat.DenShort!=1)crash << crash;
//    if(!tempLRat.IsEqualTo(PartFraction::CheckSum))crash << crash;
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
    theReport.Report("Indicator modified to regular");
  this->ResetRelevanceIsComputed();
  if (!this->CheckForMinimalityDecompositionWithRespectToRoot(&newIndicator, theGlobalVariables))
    return false;
  this->NumProcessedForVPFfractions=0;
  Rational oldCheckSum;
//  QuasiPolynomialOld oldOutput;
  if (PartFraction::MakingConsistencyCheck)
    PartFractions::CheckSum.MakeZero();
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
        if(!tempLRat2.den.IsEqualTo(LIOne))crash << crash;
        if(!tempLRat3.den.IsEqualTo(LIOne))crash << crash;
        if(!tempLRat2.IsEqualTo(PartFraction::CheckSum))crash << crash;
        if(!tempLRat4.IsEqualTo(PartFractions::CheckSum))crash << crash;
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
  //        crash << crash;
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
      crash << "LargeInt::AssignString is not allowed to fail." << crash;
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
  crash << crash;
//  this->theChambersOld.theDirections=toBePartitioned;
  this->AmbientDimension=toBePartitioned.GetDim();
//  this->theChambersOld.AmbientDimension=toBePartitioned.GetDim();
//  this->theChambersOld.thePauseController.InitComputation();
  //this->theChambers.ReadFromDefaultFile(theGlobalVariables);
  std::stringstream out;
  std::stringstream outHtml;
  crash << crash ;
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
  crash << crash;
//  this->initFromRoots(theChambersOld.theDirections, theGlobalVariables);
  out << "\\documentclass{article}\\usepackage{amsmath, amsfonts, amssymb} \n\\begin{document}\n";
  out << "The vector partition funciton is the number of ways you can represent a vector $(x_1,\\dots, x_n)$ as a non-negative integral linear combination of "
  << " a given set of vectors.  You requested the vector partition function with respect to the set of vectors: ";
  crash << crash;
//  out << this->theChambersOld.theDirections.ElementToStringGeneric();
  out << "\n\n The corresponding generating function is: " << this->ToString(theGlobalVariables, theFormat) << "= (after splitting acording to algorithm)";
  this->split(theGlobalVariables, 0);
  out << this->ToString(theGlobalVariables, theFormat);
  crash << crash;
//  out << "Therefore the vector partition function is given by " << this->theChambersOld.GetNumNonZeroPointers()
//        << " quasipolynomials depending on which set of linear inequalities is satisfied (each such set we call ``Chamber'').";
//  outHtml << "There are " << this->theChambersOld.size << " systems of linear inequalities "
//  << " such that on each such system of inequalities the vector partition function is quasi-polynomial. "
//  << " A full list of the systems of inequalities (\"chambers\") and the corresponding quasi-polynomials follows.<hr> ";
  QuasiPolynomial tempQP;
  std::string tempS;
  Vector<Rational> tempIndicator;
  crash << crash;
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
  if(this==&output || mapFromNewSpaceToOldSpace.NumRows!=this->GetNumVars() || ambientLatticeNewSpace.GetDim()!=mapFromNewSpaceToOldSpace.NumCols)
    crash << crash;
  output.AmbientLatticeReduced=ambientLatticeNewSpace;
  output.AmbientLatticeReduced.IntersectWithPreimageOfLattice(mapFromNewSpaceToOldSpace, this->AmbientLatticeReduced, theGlobalVariables);
  Vectors<Rational> allRepresentatives, imagesAllRepresentatives;
  bool tempBool=ambientLatticeNewSpace.GetAllRepresentatives(output.AmbientLatticeReduced, allRepresentatives);
  if(!tempBool)
    crash << crash;
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
      crash << "This is a programming error: substitution   " << theSub.ToString() << " into polynomial " << tempP.ToString()
      << " failed but the current function does not handle this properly. " << crash;
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
      crash << "This is a programming error: substitution   " << theSub.ToString() << " into polynomial " << tempP.ToString()
      << " failed but the current function does not handle this properly. " << crash;
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
        crash << "This is a programming error: substitution   " << theSub.ToString() << " into polynomial " << tempP.ToString()
        << " failed but the current function does not handle this properly. " << crash;
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
      currentPoly.GetCoeffInFrontOfLinearTermVariableIndex(j, output.elements[i][j]);
  }
  return true;
}

void Lattice::IntersectWithLinearSubspaceGivenByNormal(const Vector<Rational>& theNormal)
{ //stOutput << "<br>Starting lattice: " << this->ToString(true, false) << "<br>";
  Vectors<Rational> startingBasis, resultBasis;
  startingBasis.AssignMatrixRows(this->basisRationalForm);
  //stOutput << "Starting normal: " << theNormal.ToString();
  Vector<Rational> theScalarProducts;
  theScalarProducts.SetSize(startingBasis.size);
  for (int i=0; i<this->basisRationalForm.NumRows; i++)
    theScalarProducts[i]=theNormal.ScalarEuclidean(startingBasis[i]);
  if (theScalarProducts.IsEqualToZero())
    return;
  //stOutput << "<br>the scalar products: " << theScalarProducts.ToString();
  int pivotColumnIndex=theScalarProducts.getIndexLastNonZeroCoordinate();
  Rational pivotCoeff=theScalarProducts[pivotColumnIndex];
  theScalarProducts/=-pivotCoeff;
  //stOutput << "<br>the scalar products after scaling: " << theScalarProducts.ToString();
  Vectors<Rational> eigenSpacePlusOrthogonalComponent;
  eigenSpacePlusOrthogonalComponent.SetSize(theScalarProducts.size);
  for (int i=0; i<theScalarProducts.size; i++)
    if (i!=pivotColumnIndex)
    { Vector<Rational>& currentRoot=eigenSpacePlusOrthogonalComponent[i];
      currentRoot.MakeZero(theScalarProducts.size);
      currentRoot[i]=1;
      currentRoot[pivotColumnIndex]=theScalarProducts[i];
    }
  theScalarProducts.ScaleByPositiveRationalToIntegralMinHeight();
  //stOutput << "<br>the scalar products after scaling to integral: " << theScalarProducts.ToString();
  eigenSpacePlusOrthogonalComponent[pivotColumnIndex]=theScalarProducts;
  //stOutput << "<br>The eigenspace before intersection: " << eigenSpacePlusOrthogonalComponent.ToString();
  Lattice eigenLattice, theZnLattice;
  eigenLattice.MakeFromRoots(eigenSpacePlusOrthogonalComponent);
  //stOutput << "<br>The eigen-Lattice: " << eigenLattice.ToString(true, false);
  theZnLattice.MakeZn(theScalarProducts.size);
  //stOutput << "<br>The Zn-Lattice: " << theZnLattice.ToString(true, false);
  theZnLattice.IntersectWithBothOfMaxRank(eigenLattice);
  //stOutput << "<br>Zn intersected with eigen-Lattice: " << theZnLattice.ToString(true, false);
  resultBasis.ReservE(theScalarProducts.size-1);
  Vector<Rational> tempRoot, resultRoot; Rational orthogonalComponent;
  for (int i=0; i<theZnLattice.basisRationalForm.NumRows; i++)
  { theZnLattice.basisRationalForm.GetVectorFromRow(i, tempRoot);
    orthogonalComponent=tempRoot.ScalarEuclidean(theScalarProducts)/theScalarProducts.ScalarEuclidean(theScalarProducts);
    tempRoot-=theScalarProducts*orthogonalComponent;
    if(!orthogonalComponent.IsInteger())
      crash << crash;
    if (!tempRoot.IsEqualToZero())
    { resultRoot.MakeZero(this->GetDim());
      for (int j=0; j<startingBasis.size; j++)
        resultRoot+=startingBasis[j]*tempRoot[j];
      resultBasis.AddOnTop(resultRoot);
    }
  }
  //stOutput << "<br>Resulting basis: " << resultBasis.ToString();
  this->MakeFromRoots(resultBasis);
  //stOutput << "<br>Final answer lattice form: " << this->ToString(true, false);
}

void Lattice::IntersectWithLinearSubspaceSpannedBy(const Vectors<Rational>& theSubspaceBasis)
{ Vectors<Rational> theNormals;
  Matrix<Rational> theMat;
  theSubspaceBasis.GetMatrixRootsToRows(theMat);
  theMat.GetZeroEigenSpace(theNormals);
  this->IntersectWithLinearSubspaceGivenByNormals(theNormals);
}

void Lattice::IntersectWithLinearSubspaceGivenByNormals(const Vectors<Rational>& theSubspaceNormals)
{ //stOutput << "<br>********************Debug info for IntersectWithLinearSubspaceGivenByNormals*******************";
  for (int i=0; i<theSubspaceNormals.size; i++)
    this->IntersectWithLinearSubspaceGivenByNormal(theSubspaceNormals[i]);
  //stOutput << "<br>********************End of debug info for IntersectWithLinearSubspaceGivenByNormals*******************";
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
  //stOutput <<"<br> the sub: " << theSub.ToString(true, false) << "<br>";
  int startingDim=this->GetDim();
  Matrix<Rational> theMat, oldBasisTransformed, matRelationBetweenStartingVariables;
  theMat=theSub;
  oldBasisTransformed=this->basisRationalForm;
  oldBasisTransformed.Transpose();
  Selection nonPivotPoints;
  //stOutput << "<br>the matrices to be transformed: " << theMat.ToString(true, false) << "<br>" << oldBasisTransformed.ToString(true, false);
  theMat.GaussianEliminationByRows(&oldBasisTransformed, &nonPivotPoints);
  //stOutput << "<br>afer transformation: " << theMat.ToString(true, false) << "<br>" << oldBasisTransformed.ToString(true, false);
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
  //stOutput << "<br>matRelationBetweenStartingVariables" <<  matRelationBetweenStartingVariables.ToString(true, false);
  for (int i=0; i<theEigenSpace.size; i++)
    theEigenSpace[i].ScaleByPositiveRationalToIntegralMinHeight();
  //stOutput << "the basis: " << theEigenSpace.ToString();
  oldBasisTransformed.ActOnVectorsColumn(theEigenSpace);
  //stOutput << "<br>the basis transformed: " << theEigenSpace.ToString();
  this->basisRationalForm.init(targetDim, targetDim);
  for (int i=0; i<targetDim; i++)
    for (int j=0; j<targetDim; j++)
      this->basisRationalForm.elements[i][j]=theEigenSpace.TheObjects[i].TheObjects[j];
  this->basisRationalForm.GetMatrixIntWithDen(this->basis, this->Den);
  this->Reduce();
  //stOutput << "<br><br>and the sub result is: <br>" << this->ToString(true, false);
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
{ output << XML::GetOpenTagNoInputCheckAppendSpacE(this->GetXMLClassName());
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
  output << XML::GetCloseTagNoInputCheckAppendSpacE(this->GetXMLClassName());
}

bool Cone::ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables)
{ std::string tempS, rootString;
  Vectors<Rational> buffer;
  int NumWordsRead;
  XML::ReadThroughFirstOpenTag(input, NumWordsRead, this->GetXMLClassName());
  if(NumWordsRead!=0)
    crash << crash;
  input >> tempS;
  buffer.size=0;
  Vector<Rational> tempRoot;
  if (tempS!="Cone(")
  { stOutput << "tempS was instead " << tempS;
    XML::ReadEverythingPassedTagOpenUntilTagClose(input, NumWordsRead, this->GetXMLClassName());
    return false;
  }
  for (input >> tempS; tempS!=")" && tempS!=""; input >> tempS)
  { tempRoot.AssignString(tempS);
    buffer.AddOnTop(tempRoot);
    //stOutput << "vector input " << tempS << " read as " << tempRoot.ToString();
  }
  if (buffer.size<1)
  { XML::ReadEverythingPassedTagOpenUntilTagClose(input, NumWordsRead, this->GetXMLClassName());
    return false;
  }
  int theDim=buffer.TheObjects[0].size;
  for (int i=1; i<buffer.size; i++)
    if (buffer.TheObjects[i].size!=theDim)
    { XML::ReadEverythingPassedTagOpenUntilTagClose(input, NumWordsRead, this->GetXMLClassName());
      return false;
    }
  bool result;
  result=this->CreateFromNormals(buffer, theGlobalVariables);
  result= XML::ReadEverythingPassedTagOpenUntilTagClose(input, NumWordsRead, this->GetXMLClassName()) && result;
//  if(!tempBool) crash << crash;
  return result;
}

void ConeLatticeAndShift::WriteToFile
  (std::fstream& output, GlobalVariables* theGlobalVariables)
{ output << XML::GetOpenTagNoInputCheckAppendSpacE(this->GetXMLClassName());
  this->theLattice.WriteToFile(output, theGlobalVariables);
  this->theProjectivizedCone.WriteToFile(output, theGlobalVariables);
  this->theShift.WriteToFile(output);
  output << XML::GetCloseTagNoInputCheckAppendSpacE(this->GetXMLClassName());
}

bool ConeLatticeAndShift::ReadFromFile
  (std::fstream& output, GlobalVariables* theGlobalVariables)
{ XML::ReadThroughFirstOpenTag(output, this->GetXMLClassName());
  this->theLattice.ReadFromFile(output, theGlobalVariables);
  this->theProjectivizedCone.ReadFromFile(output, theGlobalVariables);
  this->theShift.ReadFromFile(output);
  XML::ReadEverythingPassedTagOpenUntilTagClose(output, this->GetXMLClassName());
  return true;
}

void ConeLatticeAndShiftMaxComputation::WriteToFile
  (std::fstream& output, GlobalVariables* theGlobalVariables)
{ output << XML::GetOpenTagNoInputCheckAppendSpacE(this->GetXMLClassName());
  output << " NumParam: " << this->numNonParaM << " numProcessedNonParam: " << this->numProcessedNonParam << "\n";

  output << XML::GetOpenTagNoInputCheckAppendSpacE("complexStartingPerRepresentative");
  this->complexStartingPerRepresentative.WriteToFile(output, theGlobalVariables);
  output << XML::GetCloseTagNoInputCheckAppendSpacE("complexStartingPerRepresentative");

  output << XML::GetOpenTagNoInputCheckAppendSpacE("complexRefinedPerRepresentative");
  this->complexRefinedPerRepresentative.WriteToFile(output, theGlobalVariables);
  output << XML::GetCloseTagNoInputCheckAppendSpacE("complexRefinedPerRepresentative");

  output << XML::GetOpenTagNoInputCheckAppendSpacE("theMaximaCandidates");
  this->theMaximaCandidates.WriteToFile(output);
  output << XML::GetCloseTagNoInputCheckAppendSpacE("theMaximaCandidates");

  output << XML::GetOpenTagNoInputCheckAppendSpacE("startingLPtoMaximize");
  this->startingLPtoMaximize.WriteToFile(output);
  output << XML::GetCloseTagNoInputCheckAppendSpacE("startingLPtoMaximize");

  output << XML::GetOpenTagNoInputCheckAppendSpacE("finalMaxima");
  this->finalMaxima.WriteToFile(output);
  output << XML::GetCloseTagNoInputCheckAppendSpacE("finalMaxima");

  output << XML::GetOpenTagNoInputCheckAppendSpacE("theStartingLattice");
  this->theStartingLattice.WriteToFile(output, theGlobalVariables);
  output << XML::GetCloseTagNoInputCheckAppendSpacE("theStartingLattice");

  output << XML::GetOpenTagNoInputCheckAppendSpacE("theFinalRougherLattice");
  this->theFinalRougherLattice.WriteToFile(output, theGlobalVariables);
  output << XML::GetCloseTagNoInputCheckAppendSpacE("theFinalRougherLattice");

  output << XML::GetOpenTagNoInputCheckAppendSpacE("theStartingRepresentative");
  this->theStartingRepresentative.WriteToFile(output);
  output << XML::GetCloseTagNoInputCheckAppendSpacE("theStartingRepresentative");

  output << XML::GetOpenTagNoInputCheckAppendSpacE("theFinalRepresentatives");
  this->theFinalRepresentatives.WriteToFile(output, theGlobalVariables);
  output << XML::GetCloseTagNoInputCheckAppendSpacE("theFinalRepresentatives");

  output << XML::GetOpenTagNoInputCheckAppendSpacE("theConesLargerDim");
  this->theConesLargerDim.WriteToFile(output, theGlobalVariables);
  output << XML::GetCloseTagNoInputCheckAppendSpacE("theConesLargerDim");

  output << XML::GetOpenTagNoInputCheckAppendSpacE("theConesSmallerDim");
  this->theConesSmallerDim.WriteToFile(output, theGlobalVariables);
  output << XML::GetCloseTagNoInputCheckAppendSpacE("theConesSmallerDim");

  output << XML::GetOpenTagNoInputCheckAppendSpacE("IsInfinity");
  output << this->IsInfinity;
  output << XML::GetCloseTagNoInputCheckAppendSpacE("IsInfinity");

  output << XML::GetOpenTagNoInputCheckAppendSpacE("LPtoMaximizeLargerDim");
  this->LPtoMaximizeLargerDim.WriteToFile(output, theGlobalVariables);
  output << XML::GetCloseTagNoInputCheckAppendSpacE("LPtoMaximizeLargerDim");

  output << XML::GetOpenTagNoInputCheckAppendSpacE("LPtoMaximizeSmallerDim");
  this->LPtoMaximizeSmallerDim.WriteToFile(output, theGlobalVariables);
  output << XML::GetCloseTagNoInputCheckAppendSpacE("LPtoMaximizeSmallerDim");

  output << XML::GetCloseTagNoInputCheckAppendSpacE(this->GetXMLClassName()) << "\n";
}

bool ConeLatticeAndShiftMaxComputation::ReadFromFile
(std::fstream& input, GlobalVariables* theGlobalVariables, int UpperLimitDebugPurposes)
{ ProgressReport theReport(theGlobalVariables);
  int numReadWords;
  XML::ReadThroughFirstOpenTag(input, numReadWords, this->GetXMLClassName());
  std::string tempS;
  input >> tempS >> this->numNonParaM >> tempS >> this->numProcessedNonParam;
  theReport.Report("Loading complex starting per representative...");
  XML::ReadThroughFirstOpenTag(input, "complexStartingPerRepresentative");
  this->complexStartingPerRepresentative.ReadFromFile(input, theGlobalVariables);
  XML::ReadEverythingPassedTagOpenUntilTagClose(input, "complexStartingPerRepresentative");
  theReport.Report("Loading complex refined per representative...");
  XML::ReadThroughFirstOpenTag(input, "complexRefinedPerRepresentative");
  this->complexRefinedPerRepresentative.ReadFromFile(input, theGlobalVariables);
  XML::ReadEverythingPassedTagOpenUntilTagClose(input, "complexRefinedPerRepresentative");
  theReport.Report("Loading the max candidates...");
  XML::ReadThroughFirstOpenTag(input, "theMaximaCandidates");
  this->theMaximaCandidates.ReadFromFile(input, theGlobalVariables);
  XML::ReadEverythingPassedTagOpenUntilTagClose(input, "theMaximaCandidates");
  theReport.Report("Loading starting linear polys...");
  XML::ReadThroughFirstOpenTag(input, "startingLPtoMaximize");
  this->startingLPtoMaximize.ReadFromFile(input, theGlobalVariables);
  XML::ReadEverythingPassedTagOpenUntilTagClose(input, "startingLPtoMaximize");
  theReport.Report("Loading final maxima...");
  XML::ReadThroughFirstOpenTag(input, "finalMaxima");
  this->finalMaxima.ReadFromFile(input, theGlobalVariables);
  XML::ReadEverythingPassedTagOpenUntilTagClose(input, "finalMaxima");
  theReport.Report("Loading starting lattice...");
  XML::ReadThroughFirstOpenTag(input, "theStartingLattice");
  this->theStartingLattice.ReadFromFile(input, theGlobalVariables);
  XML::ReadEverythingPassedTagOpenUntilTagClose(input, "theStartingLattice");
  theReport.Report("Loading final rougher lattice...");
  XML::ReadThroughFirstOpenTag(input, "theFinalRougherLattice");
  this->theFinalRougherLattice.ReadFromFile(input, theGlobalVariables);
  XML::ReadEverythingPassedTagOpenUntilTagClose(input, "theFinalRougherLattice");
  theReport.Report("Loading starting representative...");
  XML::ReadThroughFirstOpenTag(input, "theStartingRepresentative");
  this->theStartingRepresentative.ReadFromFile(input);
  XML::ReadEverythingPassedTagOpenUntilTagClose(input, "theStartingRepresentative");
  theReport.Report("Loading final representatives...");
  XML::ReadThroughFirstOpenTag(input, "theFinalRepresentatives");
  this->theFinalRepresentatives.ReadFromFile(input, theGlobalVariables);
  XML::ReadEverythingPassedTagOpenUntilTagClose(input, "theFinalRepresentatives");
  theReport.Report("Loading cones larger dim...");
  XML::ReadThroughFirstOpenTag(input, "theConesLargerDim");
  this->theConesLargerDim.ReadFromFile(input, theGlobalVariables, UpperLimitDebugPurposes);
  XML::ReadEverythingPassedTagOpenUntilTagClose(input, "theConesLargerDim");
  theReport.Report("Loading cones smaller dim...");
  XML::ReadThroughFirstOpenTag(input, "theConesSmallerDim");
  this->theConesSmallerDim.ReadFromFile(input, theGlobalVariables);
  XML::ReadEverythingPassedTagOpenUntilTagClose(input, "theConesSmallerDim");
  theReport.Report("Loading IsInfinite array...");
  XML::ReadThroughFirstOpenTag(input, "IsInfinity");
  input >> this->IsInfinity;
  XML::ReadEverythingPassedTagOpenUntilTagClose(input, "IsInfinity");
  theReport.Report("Loading LPtoMaximizeLargerDim...");
  XML::ReadThroughFirstOpenTag(input, "LPtoMaximizeLargerDim");
  this->LPtoMaximizeLargerDim.ReadFromFile(input, theGlobalVariables, UpperLimitDebugPurposes);
  XML::ReadEverythingPassedTagOpenUntilTagClose(input, "LPtoMaximizeLargerDim");
  theReport.Report("Loading LPtoMaximizeSmallerDim...");
  XML::ReadThroughFirstOpenTag(input, "LPtoMaximizeSmallerDim");
  this->LPtoMaximizeSmallerDim.ReadFromFile(input, theGlobalVariables);
  XML::ReadEverythingPassedTagOpenUntilTagClose(input, "LPtoMaximizeSmallerDim");
  XML::ReadEverythingPassedTagOpenUntilTagClose(input, numReadWords, this->GetXMLClassName());
  if(numReadWords!=0)
    crash << crash;
  return true;
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
{ output << XML::GetOpenTagNoInputCheckAppendSpacE(this->GetXMLClassName());
  this->basisRationalForm.WriteToFile(output);
  output << XML::GetCloseTagNoInputCheckAppendSpacE(this->GetXMLClassName());
}

bool Lattice::ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables)
{ int numReadWords;
  XML::ReadThroughFirstOpenTag(input, numReadWords, this->GetXMLClassName());
  bool result=this->basisRationalForm.ReadFromFile(input);
  this->basisRationalForm.GetMatrixIntWithDen(this->basis, this->Den);
  XML::ReadEverythingPassedTagOpenUntilTagClose(input, numReadWords, this->GetXMLClassName());
  if(numReadWords!=0)
    crash << crash;
  return result;
}

void ConeComplex::WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables, int UpperLimit)
{ this->List<Cone>::WriteToFile(output, theGlobalVariables, UpperLimit);
  output << "IndexLowestNonRefined: " << this->indexLowestNonRefinedChamber << "\n";
  this->splittingNormals.WriteToFile(output);
  this->slicingDirections.WriteToFile(output);
  this->ConvexHull.WriteToFile(output, theGlobalVariables);
}

bool ConeComplex::ReadFromFile(std::fstream& input, GlobalVariables* theGlobalVariables, int UpperLimitDebugPurposes)
{ if(!this->List<Cone>::ReadFromFile(input, theGlobalVariables, UpperLimitDebugPurposes))
  { crash << crash ;
    return false;
  }
  std::string tempS;
  input >> tempS >> this->indexLowestNonRefinedChamber;
  if (tempS!="IndexLowestNonRefined:")
  { crash << crash;
    return false;
  }
  if (!this->splittingNormals.ReadFromFile(input))
  { crash << crash;
    return false;
  }
  this->slicingDirections.ReadFromFile(input, theGlobalVariables);
  this->ConvexHull.ReadFromFile(input, theGlobalVariables);
  return true;
}

void Cone::IntersectAHyperplane(Vector<Rational>& theNormal, Cone& outputConeLowerDim, GlobalVariables& theGlobalVariables)
{ if(theNormal.IsEqualToZero())
    crash << crash;
  int theDimension=theNormal.size;
  Matrix<Rational> tempMat, theEmbedding, theProjection;
  tempMat.AssignVectorRow(theNormal);
  Vectors<Rational> theBasis;
  tempMat.GetZeroEigenSpace(theBasis);
  if(theBasis.size!=theNormal.size-1)
    crash << crash;
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
  if(tempBool)
    crash << crash;
}

bool Cone::GetRootFromLPolyConstantTermGoesToLastVariable(Polynomial<Rational>& inputLPoly, Vector<Rational>& output)
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
      case '\"': out << "%22"; break;
//      case '': out << ""; break;
      default: out << input[i]; break;
    }
  return out.str();
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
          thePF.GetNElongationPoly(this->startingVectors, currentIndex, ElongationValue, numSummands, tempIP, this->AmbientDimension);
          //tempIP.ComputeDebugString();
          tempIP.RaiseToPower(currentFrac.Multiplicities[i], (LargeInt) 1);
          //tempIP.ComputeDebugString();
          currentCoeff*=tempIP;
//          ComputationalBufferCoefficientNonExpanded.ComputeDebugString();
          currentFrac.AddMultiplicity(currentFrac.Multiplicities[i], LCMElongations);
          currentFrac.AddMultiplicity(-currentFrac.Multiplicities[i], ElongationValue);
//          thePF.ComputeOneCheckSum(*this, localEndCheckSum, this->AmbientDimension, theGlobalVariables);
          if(!localEndCheckSum.IsEqualTo(localStartCheckSum))
            crash << crash;
        }
      }
    }
    if (PartFraction::MakingConsistencyCheck || this->flagAnErrorHasOccurredTimeToPanic)
    { //thePF.ComputeOneCheckSum(*this, localEndCheckSum, this->AmbientDimension, theGlobalVariables);
      //localEndCheckSum.ToString(tempS1);
      //if(!localStartCheckSum.IsEqualTo(localEndCheckSum))crash << crash;
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
    crash << "This is a programming error: calling lcm on zero elements is not allowed. " << crash;
  LargeIntUnsigned::gcd(a, b, tempUI);
  a.MultiplyBy(b, tempUI2);
  output=(tempUI2);
  output.DivPositive(tempUI, output, tempUI2);
  if(output.IsEqualToZero())
    crash << crash;
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
  //stOutput << "<br>the normal: " << theNormal.ToString();
  //stOutput << "<br> the direction lattice: " << theDirectionLattice.ToString();
  theHyperplaneLatticeNoShift=*this;
  theHyperplaneLatticeNoShift.IntersectWithLinearSubspaceGivenByNormal(theNormal);
  //stOutput << "<br>the non-affine hyperplane intersected with the lattice: " << theHyperplaneLatticeNoShift.ToString();
  tempRoots.AssignMatrixRows(theHyperplaneLatticeNoShift.basisRationalForm);
  tempRoots.AddOnTop(outputDirectionMultipleOnLattice);
  outputRougherLattice.MakeFromRoots(tempRoots);
  this->GetAllRepresentatives(outputRougherLattice, outputRepresentatives);
  //stOutput << "<br>the rougher lattice: " << outputRougherLattice.ToString();
  //stOutput << "<br>representatives of the quotient of the two lattices: " << outputRepresentatives.ToString();
  for (int i=0; i<outputRepresentatives.size; i++)
  { outputRepresentatives.TheObjects[i]+=theShift;
    outputRougherLattice.ReduceVector(outputRepresentatives.TheObjects[i]);
  }
  Rational theShiftedConst, unitMovement, tempRat;
  unitMovement=theNormal.ScalarEuclidean(outputDirectionMultipleOnLattice);
  movementInDirectionPerRepresentative.SetSize(outputRepresentatives.size);
  //stOutput << "<br>Affine hyperplane per representative: ";
  for (int i=0; i<outputRepresentatives.size; i++)
  { tempRat=(theNormal.ScalarEuclidean(outputRepresentatives.TheObjects[i]) - theConstOnTheOtherSide)/unitMovement;
    tempRat.AssignFracValue();
    theShiftedConst=theConstOnTheOtherSide + tempRat;
    Vector<Rational>& currentMovement=movementInDirectionPerRepresentative.TheObjects[i];
    currentMovement=theAffineHyperplane;
    *currentMovement.LastObject()=theShiftedConst;
    //stOutput << "<br>Representative: " << outputRepresentatives.TheObjects[i].ToString() << " and the hyperplane: " << currentMovement.ToString();
  }
//  stOutput << "<hr>"
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
//  stOutput << "<br>Num cycles:" << numCycles << "<br>The basis mons (there are " << basisMonsZeroWeight.size << " of them): "  << basisMonsZeroWeight.ToString(PolyFormatLocal);
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
//    stOutput << "<div class=\"math\" scale=\"50\">" << tempMat.ToString(false, true) << "</div>";
  Vectors<Rational> tempRoots;
  tempMat.GetZeroEigenSpaceModifyMe(tempRoots);
  output.SetSize(tempRoots.size);
//  stOutput << "<br>invariants Vector<Rational> form: " << tempRoots.ToString();
//  stOutput << "<br> .... and the invariants are: ";
  for (int i=0; i<output.size; i++)
  { Polynomial<Rational>& current=output[i];
    current.MakeZero();
    for (int j=0; j<basisMonsZeroWeight.size(); j++)
      if (!tempRoots[i][j].IsEqualToZero())
      { theMon=(basisMonsZeroWeight[j]);
        current.AddMonomial(theMon, tempRoots[i][j]);
      }
 //   stOutput << "<br>Invariant " << i << ":<br>" << current.ToString(PolyFormatLocal);
  }
  return true;
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

void DrawOperations::MakeMeAStandardBasis(int theDim)
{ if (theDim<1)
    return;
  if (theDim>3)
  { this->ProjectionsEiVectors.SetSizeMakeMatrix(theDim, 2);
    for (int i=0; i<theDim; i++)
    { this->ProjectionsEiVectors[i][0]=FloatingPoint::sin((double)i/(double)theDim* MathRoutines::Pi());
      this->ProjectionsEiVectors[i][1]=FloatingPoint::cos((double)i/(double)theDim* MathRoutines::Pi());
    }
  } else
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

std::string ConeComplex::DrawMeToHtmlLastCoordAffine(DrawingVariables& theDrawingVariables, FormatExpressions& theFormat)
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
    //stOutput <<"<hr> drawing number " << i+1 << ": " << theDrawingVariables.GetHtmlFromDrawOperationsCreateDivWithUniqueName(this->GetDim()-1);
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
          theDrawingVariables.theBuffer.BasisProjectionPlane[0][i][j]=tempRoots[i][j].GetDoubleValue();
    }
  }
  for (int i=0; i<this->size; i++)
  { //theDrawingVariables.theBuffer.init();
    result=(this->TheObjects[i].DrawMeProjective(coordCenterTranslation, false, theDrawingVariables, theFormat) && result);
    //stOutput <<"<hr> drawing number " << i+1 << ": " << theDrawingVariables.GetHtmlFromDrawOperationsCreateDivWithUniqueName(this->GetDim()-1);
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
    if(sumAbsValuesCoords.IsEqualToZero())
      crash << crash;
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

std::string CGI::GetHtmlButton(const std::string& buttonID, const std::string& theScript, const std::string& buttonText)
{ std::stringstream out;
  out << "\n<button id=\"" << buttonID << "\" " << CGI::GetStyleButtonLikeHtml() << " onclick=\"" << theScript << "\">" << buttonText << "</button>";
  return out.str();
}

std::string CGI::GetHtmlSpanHidableStartsHiddeN(const std::string& input)
{ std::stringstream out;
  CGI::GlobalGeneralPurposeID ++;
  std::stringstream buttonLabel;
  std::stringstream spanLabel;
  spanLabel << "hidableSpan" << CGI::GlobalGeneralPurposeID;
  buttonLabel << "buttonHS" << CGI::GlobalGeneralPurposeID;
  out << CGI::GetHtmlButton(buttonLabel.str(), "switchMenu('"+spanLabel.str() +"');", "info expand/collapse");
  out << "<span";
  out << " id=\"" << spanLabel.str() << "\" style=\"display: none\">";
  out << input << "</span>";
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

void DrawOperations::init()
{ this->IndexNthDrawOperation.ReservE(10000);
  this->TypeNthDrawOperation.ReservE(10000);
  this->theDrawLineBetweenTwoRootsOperations.ReservE(10000);
  this->theDrawTextAtVectorOperations.ReservE(15);
  this->theDrawCircleAtVectorOperations.ReservE(280);
  this->IndexNthDrawOperation.size=0;
  this->TypeNthDrawOperation.size=0;
  this->theDrawTextOperations.size=0;
  this->theDrawLineOperations.size=0;
  this->theDrawLineBetweenTwoRootsOperations.size=0;
  this->theDrawTextAtVectorOperations.size=0;
  this->theDrawCircleAtVectorOperations.size=0;
  this->centerX.initFillInObject(this->BasisProjectionPlane.size, 300);
  this->centerY.initFillInObject(this->BasisProjectionPlane.size, 300);
  this->GraphicsUnit.initFillInObject(this->BasisProjectionPlane.size, DrawOperations::GraphicsUnitDefault);
  this->labeledVectors.SetSize(0);
  this->labelsOfLabeledVectors.SetSize(0);
  this->ClickToleranceX=5;
  this->ClickToleranceY=5;
  this->SelectedCircleMinus2noneMinus1Center=-2;
  this->SelectedPlane=0;
  this->flagRotatingPreservingAngles=true;
  this->flagAnimatingMovingCoordSystem=false;
  this->flagIsPausedWhileAnimating=false;
}

void AnimationBuffer::operator=(const AnimationBuffer& other)
{ this->stillFrame=other.stillFrame;
  this->thePhysicalDrawOps=other.thePhysicalDrawOps;
  this->theVirtualOpS=other.theVirtualOpS;
  this->flagAnimating=other.flagAnimating;
  this->flagIsPausedWhileAnimating=other.flagIsPausedWhileAnimating;
  this->indexVirtualOp=other.indexVirtualOp;
  if(this->indexVirtualOp>=this->theVirtualOpS.size)
    crash << crash;
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
  if(this->indexVirtualOp>=this->theVirtualOpS.size)
    crash << crash;
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
  if(this->indexVirtualOp>=this->theVirtualOpS.size)
    crash << crash;
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
    result= FloatingPoint::arctan(y/x);
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
(std::stringstream& logger, Vector<double>& input, Vector<double>& output, Vector<double>& orthoBasis1, Vector<double>& orthoBasis2, double oldTanSquared, double newTanSquared)
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
  double oldAngle=FloatingPoint::arctan(FloatingPoint::sqrt(oldTanSquared));
  double newAngle=FloatingPoint::arctan(FloatingPoint::sqrt(newTanSquared));
  double angleChange=-oldAngle+newAngle;
  projection=orthoBasis1;
  projection*=FloatingPoint::cos(angleChange)*scal1-FloatingPoint::sin(angleChange)*scal2;
  projection+=orthoBasis2*(FloatingPoint::sin(angleChange)*scal1+FloatingPoint::sin(angleChange)*scal2);
  output = vComponent;
  output+=projection;
}

void DrawOperations::ModifyToOrthonormalNoShiftSecond(Vector<double>& root1, Vector<double>& root2)
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
  { tempRoot.MakeEi(theDimension, i);
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
  NewVectorE1= currentBasisPlane[0]*FloatingPoint::cos(AngleChange);
  NewVectorE1+=currentBasisPlane[1]*FloatingPoint::sin(AngleChange);
  NewVectorE2= currentBasisPlane[1]*FloatingPoint::cos(AngleChange);
  NewVectorE2+=currentBasisPlane[0]*(-FloatingPoint::sin(AngleChange));
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
  { this->theValue=other.GetDoubleValue();
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
  { this->theValue=FloatingPoint::floor(this->theValue);
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
        crash << crash;
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

void RationalFunctionOld::Substitution(const PolynomialSubstitution<Rational>& theSub)
{ if (theSub.size<1)
    return;
//  FormatExpressions tempFormat;
//  int commentMEWhenDone;
  switch(this->expressionType)
  { case RationalFunctionOld::typeRational:
//      if(!this->checkConsistency())crash << crash;
      return;
    case RationalFunctionOld::typePoly:
//      stOutput <<"<hr>subbing in<br>" << this->ToString(tempFormat) << " using " << theSub.ToString()
//      << " to get ";
      if (!this->Numerator.GetElement().Substitution(theSub, 1))
        crash << "This is a programming error: substitution   " << theSub.ToString() << " into polynomial " << this->Numerator.GetElement().ToString()
        << " failed but the current function does not handle this properly. " << crash;
//      stOutput << "<br>finally:<br>" << this->Numerator.GetElement().ToString();
      this->Simplify();
//      stOutput << ", which, simplified, yields<br> " << this->ToString(tempFormat);
//      if(!this->checkConsistency())crash << crash;
      return;
    case RationalFunctionOld::typeRationalFunction:
      if (!this->Numerator.GetElement().Substitution(theSub, 1))
        crash << "This is a programming error: substitution " << theSub.ToString() << " into polynomial " << this->Numerator.GetElement().ToString()
        << " failed but the current function does not handle this properly. " << crash;
      if (!this->Denominator.GetElement().Substitution(theSub, 1))
        crash << "This is a programming error: substitution   " << theSub.ToString() << " into polynomial " << this->Denominator.GetElement().ToString()
        << " failed but the current function does not handle this properly. " << crash;
      this->Simplify();
//      if(!this->checkConsistency())crash << crash;
      return;
    default:
      crash << crash;
      break;
  }
}
