//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
#include "vpfHeader3Calculator2_InnerFunctions.h"
#include "vpfHeader3Calculator1_InnerTypedFunctions.h"
#include "vpfHeader2Math9DrawingVariables.h"
#include "vpfHeader3Calculator4HtmlFunctions.h"
#include "vpfHeader5Crypto.h"
#include "vpfHeader8HtmlSnippets.h"
ProjectInformationInstance ProjectInfoVpf6_35cpp(__FILE__, "More calculator built-in functions. ");

struct MeshTriangles{
  Expression theFun;
  Calculator* owner;
  int XstartingGridCount;
  int YstartingGridCount;
  int numBadEvaluations;
  int numGoodEvaluations;
  int maxNumTriangles;
  double Height;
  double Width;
  double minTriangleSideAsPercentOfWidthPlusHeight;
  MeshTriangles()
  { this->XstartingGridCount=0;
    this->YstartingGridCount=0;
    this->numBadEvaluations=0;
    this->numGoodEvaluations=0;
    this->flagFunctionEvaluationFailed=false;
    this->minTriangleSideAsPercentOfWidthPlusHeight=0.001;
    this->maxNumTriangles=2000;
    this->flagTriangleLimitReached=false;
    this->flagShowGrid=false;
  }
  List<bool> trianglesUsed;
  List<List<Vector<double> > > theTriangles;
  MapLisT<Vector<double>, double, MathRoutines::HashVectorDoubles> theEvaluatedPoints;
  HashedList<Expression> knownEs;
  List<double> knownValues;
  Vector<double> lowerLeftCorner, upperRightCorner;
  Plot theGrid;
  Plot theCurve;
  Plot thePlot;
  bool flagShowGrid;
  bool flagFunctionEvaluationFailed;
  bool flagTriangleLimitReached;
  int CleanUpTrianglesReturnUpdatedCurrentIndex(int currentIndex);
  void ComputeImplicitPlot();
  void ComputeImplicitPlotPart2();
  void PlotGrid(int theColor);
  bool EvaluatesToDouble(double& whichDouble);
  double GetValueAtPoint(const Vector<double>& thePoint);
  void EvaluateFunAtTriangleVertices(int triangleIndex);
  double GetTriangleMaxSideLength(int triangleIndex);
  void Subdivide(int triangleIndex);
  void AddPointFromVerticesValues
  (Vectors<double>& outputAppend, const Vector<double>& left,
   const Vector<double>& right, double leftVal, double rightVal)
   ;
  bool ComputePoints
  (Calculator& theCommands, const Expression& input, bool showGrid)
;
};

void MeshTriangles::PlotGrid(int theColor)
{ MacroRegisterFunctionWithName("MeshTriangles::PlotGrid");
  this->theGrid.thePlots.SetSize(0);
  this->theGrid.thePlots.SetExpectedSize(this->theGrid.thePlots.size+this->theTriangles.size*3);
//  List<Vector<double> > currentLine;
//  currentLine.SetSize(2);
  PlotObject currentLinePlot;
  List<Vector<double> >& pointsVector=currentLinePlot.thePoints;
  currentLinePlot.thePoints.SetSize(4);
  currentLinePlot.colorRGB=theColor;
  for (int i=0; i<this->theTriangles.size; i++)
  { pointsVector[0]=this->theTriangles[i][0];
    pointsVector[1]=this->theTriangles[i][1];
    pointsVector[2]=this->theTriangles[i][2];
    pointsVector[3]=this->theTriangles[i][0];
    this->theGrid.thePlots.AddOnTop(currentLinePlot);
  }
//  this->theGrid.the=theColor;
}

double MeshTriangles::GetValueAtPoint(const Vector<double>& thePoint)
{ int theIndex= this->theEvaluatedPoints.GetIndex(thePoint);
  if (theIndex!=-1)
    return this->theEvaluatedPoints[theIndex];
  this->knownValues[this->knownValues.size-2]=thePoint[0];
  this->knownValues[this->knownValues.size-1]=thePoint[1];
  double functionValue=0;
  if (this->theFun.EvaluatesToDoubleUnderSubstitutions(this->knownEs, this->knownValues, &functionValue))
    this->numGoodEvaluations++;
  else
  { this->numBadEvaluations++;
    functionValue=NAN;
  }
  this->theEvaluatedPoints.SetKeyValue(thePoint, functionValue);
  return functionValue;
}

void MeshTriangles::EvaluateFunAtTriangleVertices(int triangleIndex)
{ MacroRegisterFunctionWithName("MeshTriangles::EvaluateFunAtTriangleVertices");
  for (int j=0; j<this->theTriangles[triangleIndex].size; j++)
    this->GetValueAtPoint(this->theTriangles[triangleIndex][j]);
}

double MeshTriangles::GetTriangleMaxSideLength(int triangleIndex)
{ MacroRegisterFunctionWithName("MeshTriangles::GetTriangleMaxSideLength");
  if (this->theTriangles[triangleIndex].size!=3)
    crash << "Error: triangle needs three vertices, instead it has vertices: " << this->theTriangles[triangleIndex] << crash;
  Vector<double>& firstV = this->theTriangles[triangleIndex][0];
  Vector<double>& secondV= this->theTriangles[triangleIndex][1];
  Vector<double>& thirdV = this->theTriangles[triangleIndex][2];
  List<Vector<double> > theSides;
  theSides.AddOnTop(firstV-secondV);
  theSides.AddOnTop(secondV-thirdV);
  theSides.AddOnTop(thirdV-firstV);
  double result=0;
  for (int i=0; i<theSides.size; i++)
  { //stOutput << "<br>current side: " << theSides[i];
    double normSquared= theSides[i][0]*theSides[i][0]+theSides[i][1]*theSides[i][1];
    result=MathRoutines::Maximum(result, FloatingPoint::sqrt(normSquared));
  }
  return result;
}

void MeshTriangles::AddPointFromVerticesValues
  (Vectors<double>& outputAppend, const Vector<double>& left,
   const Vector<double>& right, double leftVal, double rightVal)
{ //MacroRegisterFunctionWithName("MeshTriangles::AddPointFromVerticesValues");
  double Delta=leftVal-rightVal;
  if (Delta==0)
  { outputAppend.AddOnTop(left);
    return;
  }
  double contributionLeft=rightVal/Delta;
  if (contributionLeft<0)
    contributionLeft*=-1;
  double contributionRight=leftVal/Delta;
  if (contributionRight<0)
    contributionRight*=-1;
  outputAppend.AddOnTop(left*contributionLeft+right*contributionRight);
}

int MeshTriangles::CleanUpTrianglesReturnUpdatedCurrentIndex(int currentIndex)
{ MacroRegisterFunctionWithName("MeshTriangles::CleanUpTrianglesReturnUpdatedCurrentIndex");
  if (this->flagTriangleLimitReached || this->theTriangles.size<this->maxNumTriangles*2)
    return currentIndex;
  //clean up carried out only when we have twice exceeded the triangle limit
  //this way clean up will be carried only after we have generated at least maxNumTriangles since the last cleanup
  //(which leaves no more than maxNumTriangles alive).
  if (this->theTriangles.size!=this->trianglesUsed.size)
    crash << "this->theTriangles.size is not equal to this->trianglesUsed.size: the values are: "
    << this->theTriangles.size << " and " << this->trianglesUsed.size << ". " << crash;
  int lowestFree=0;
  for (int i=0; i<this->theTriangles.size; i++)
    if (this->trianglesUsed[i])
    { this->theTriangles[lowestFree]=this->theTriangles[i];
      this->trianglesUsed[lowestFree]=true;
      if (i==currentIndex)
        currentIndex=lowestFree;
      lowestFree++;
    }
  this->theTriangles.SetSize(lowestFree);
  this->trianglesUsed.SetSize(lowestFree);
  if (lowestFree>this->maxNumTriangles)
    this->flagTriangleLimitReached=true;
  return currentIndex;
}

void MeshTriangles::Subdivide(int triangleIndex)
{ MacroRegisterFunctionWithName("MeshTriangles::Subdivide");
  List<Vector<double> > currentTriangle=this->theTriangles[triangleIndex];
  if (currentTriangle.size!=3)
    crash << "Triangle in mesh with less than 3 sides! " << crash;
  List<Vector<double> > insideTriange;
  insideTriange.SetSize(3);
  insideTriange[0]=(currentTriangle[1]+currentTriangle[2])*0.5;
  insideTriange[1]=(currentTriangle[2]+currentTriangle[0])*0.5;
  insideTriange[2]=(currentTriangle[0]+currentTriangle[1])*0.5;
  this->theTriangles.AddOnTop(insideTriange);
  this->trianglesUsed.AddOnTop(true);
  List<Vector<double> > newTriangle;
  newTriangle=insideTriange;
  newTriangle[0]=currentTriangle[0];
  this->theTriangles.AddOnTop(newTriangle);
  this->trianglesUsed.AddOnTop(true);
  newTriangle=insideTriange;
  newTriangle[1]=currentTriangle[1];
  this->theTriangles.AddOnTop(newTriangle);
  this->trianglesUsed.AddOnTop(true);
  newTriangle=insideTriange;
  newTriangle[2]=currentTriangle[2];
  this->theTriangles.AddOnTop(newTriangle);
  this->trianglesUsed.AddOnTop(true);
}

void MeshTriangles::ComputeImplicitPlotPart2()
{ MacroRegisterFunctionWithName("MeshTriangles::ComputeImplicitPlotPart2");
  this->theTriangles.SetExpectedSize(this->maxNumTriangles*2);
  this->trianglesUsed.SetExpectedSize(this->maxNumTriangles*2);
  this->trianglesUsed.initFillInObject(this->theTriangles.size, true);
  this->theEvaluatedPoints.SetExpectedSize(this->maxNumTriangles*4);
  this->flagTriangleLimitReached=false;
  for (int i=0; i<this->theTriangles.size; i++)
    this->EvaluateFunAtTriangleVertices(i);
  if (this->numGoodEvaluations<5)
  { this->flagFunctionEvaluationFailed=true;
    return;
  }
  double minSide=MathRoutines::Minimum(this->Height, this->Width)*this->minTriangleSideAsPercentOfWidthPlusHeight;
  PlotObject currentPlot;
  currentPlot.colorRGB=CGI::RedGreenBlue(255, 0, 0);
  Vectors<double>& theSegment=currentPlot.thePoints;
  List<Vector<double> > currentTriangle;
  for (int i=0; i<this->theTriangles.size; i++)
  { currentTriangle=this->theTriangles[i]; //making a copy in case this->theTriangles changes underneath.
    if (currentTriangle.size!=3)
      crash << "Error: triangle needs three vertices, instead it has vertices: " << currentTriangle << crash;
    bool isGood=true;
    for (int j=0; j<currentTriangle.size; j++)
      if (this->GetValueAtPoint(currentTriangle[j])==NAN )
      { isGood=false;
        break;
      }
    if (!isGood)
    { this->trianglesUsed=false;
      continue;
    }
    double val0=this->GetValueAtPoint(currentTriangle[0]);
    double val1=this->GetValueAtPoint(currentTriangle[1]);
    double val2=this->GetValueAtPoint(currentTriangle[2]);
    double prod01=val0*val1;
    double prod12=val1*val2;
    double prod20=val2*val0;
    if (prod01>0 && prod12>0 && prod20>0)
    { this->trianglesUsed[i]=false;
      continue;
    }
    i=this->CleanUpTrianglesReturnUpdatedCurrentIndex(i);
    if (this->GetTriangleMaxSideLength(i)>minSide && !this->flagTriangleLimitReached)
    { this->Subdivide(i);
      continue;
    }
    theSegment.SetSize(0);
    if (prod01<=0)
      this->AddPointFromVerticesValues(theSegment, currentTriangle[0], currentTriangle[1], val0, val1);
    if (prod12<=0)
      this->AddPointFromVerticesValues(theSegment, currentTriangle[1], currentTriangle[2], val1, val2);
    if (prod20<=0 && theSegment.size<2)
      this->AddPointFromVerticesValues(theSegment, currentTriangle[2], currentTriangle[0], val2, val0);
    if (theSegment.size!=2)
      continue;
    this->theCurve.thePlots.AddOnTop(currentPlot);
  }
}

void MeshTriangles::ComputeImplicitPlot()
{ MacroRegisterFunctionWithName("MeshTriangles::ComputeImplicitPlot");
  if (this->XstartingGridCount==0 || this->YstartingGridCount==0)
    return;
  this->Width=this->upperRightCorner[0]-this->lowerLeftCorner[0];
  this->Height=this->upperRightCorner[1]-this->lowerLeftCorner[1];
  double DeltaX =(this->Width)/this->XstartingGridCount;
  double DeltaY =(this->Height)/this->YstartingGridCount;
  Vector<double> currentPoint;
  currentPoint.initFillInObject(2, 0);
  List<Vector<double> > currentTriangle;
  currentTriangle.initFillInObject(3, currentPoint);
  for (int i=0; i<this->XstartingGridCount; i++)
    for (int j=0; j<this->YstartingGridCount; j++)
    { currentTriangle[0][0]=this->lowerLeftCorner[0]+DeltaX*i;
      currentTriangle[0][1]=this->lowerLeftCorner[1]+DeltaY*j;
      currentTriangle[1]=currentTriangle[0];
      currentTriangle[1][0]+=DeltaX;
      currentTriangle[2]=currentTriangle[1];
      currentTriangle[2][1]+=DeltaY;
      this->theTriangles.AddOnTop(currentTriangle);
      currentTriangle[1]=currentTriangle[0];
      currentTriangle[1][1]+=DeltaY;
      this->theTriangles.AddOnTop(currentTriangle);
    }
  if (this->flagShowGrid)
  { this->PlotGrid(CGI::RedGreenBlue(240, 240, 0));
    this->thePlot.thePlots.AddListOnTop(this->theGrid.thePlots);
  }
  this->ComputeImplicitPlotPart2();
  if (this->flagShowGrid)
  { this->PlotGrid(CGI::RedGreenBlue(100, 100, 100));
    this->thePlot.thePlots.AddListOnTop(this->theGrid.thePlots);
  }
//  this->theCurve.colorRGB=CGI::RedGreenBlue(255,0,0);
  this->thePlot.thePlots.AddListOnTop(this->theCurve.thePlots);
//  stOutput << "ze lines: " << this->theCurve.theLines;
}

bool Calculator::GetVectorDoubles(const Expression& input, Vector<double>& output, int DesiredDimensionNonMandatory)
{ return this->GetVectoR(input, output, 0, DesiredDimensionNonMandatory, CalculatorFunctionsGeneral::innerEvaluateToDouble);
}

bool Calculator::GetVectorDoublesFromFunctionArguments(const Expression& input, Vector<double>& output, int DesiredDimensionNonMandatory)
{ return this->GetVectorFromFunctionArguments(input, output, 0, DesiredDimensionNonMandatory, CalculatorFunctionsGeneral::innerEvaluateToDouble);
}

bool CalculatorFunctionsGeneral::innerGetPointsImplicitly
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerGetPointsImplicitly");
  MeshTriangles theMesh;
  if (!theMesh.ComputePoints(theCommands, input, false))
    return false;
  HashedList<Vector<double>, MathRoutines::HashVectorDoubles> thePoints;
  for (int i=0; i<theMesh.theCurve.thePlots.size; i++)
  { thePoints.AddOnTopNoRepetition(theMesh.theCurve.thePlots[i].thePoints[0]);
    thePoints.AddOnTopNoRepetition(theMesh.theCurve.thePlots[i].thePoints[1]);
  }
  Matrix<double> theMatrix;
  theMatrix.AssignVectorsToRows(thePoints);
  return output.AssignValue(theMatrix, theCommands);
}

bool CalculatorFunctionsGeneral::innerPlotImplicitFunction
(Calculator& theCommands, const Expression& input, Expression& output)
{ return CalculatorFunctionsGeneral::innerPlotImplicitFunctionFull(theCommands, input, output, false);
}

bool CalculatorFunctionsGeneral::innerPlotImplicitShowGridFunction(Calculator& theCommands, const Expression& input, Expression& output)
{ return CalculatorFunctionsGeneral::innerPlotImplicitFunctionFull(theCommands, input, output, true);
}

bool MeshTriangles::ComputePoints
(Calculator& theCommands, const Expression& input, bool showGrid)
{ if (input.children.size<6)
    return false;
  this->thePlot.dimension=2;
  this->flagShowGrid=showGrid;
  this->owner=&theCommands;
  this->theFun=input[1];
  this->knownEs=theCommands.knownDoubleConstants;
  this->knownValues=theCommands.knownDoubleConstantValues;
  this->knownValues.AddOnTop(0);
  this->knownValues.AddOnTop(0);
  HashedList<Expression> theFreeVars;
  if (!this->theFun.GetFreeVariables(theFreeVars, true))
    return theCommands << "Failed to extract free variables from: " << this->theFun.ToString();
  theFreeVars.QuickSortAscending();
  if (theFreeVars.size>2)
    return theCommands << "I got that your curve depends on " << theFreeVars.size << " expressions, namely: "
    << theFreeVars.ToStringCommaDelimited()
    << " and that is too many (2 max). ";
  Expression tempE;
  if (theFreeVars.size==0)
  { tempE.MakeAtom("x", theCommands);
    theFreeVars.AddOnTop(tempE);
  }
  if (theFreeVars.size==1)
  { tempE.MakeAtom("y",theCommands);
    if (theFreeVars[0]==tempE)
      tempE.MakeAtom("x", theCommands);
    theFreeVars.AddOnTop(tempE);
  }
  this->knownEs.AddOnTopNoRepetitionMustBeNewCrashIfNot(theFreeVars[0]);
  this->knownEs.AddOnTopNoRepetitionMustBeNewCrashIfNot(theFreeVars[1]);
  if (!theCommands.GetVectorDoubles(input[2], this->lowerLeftCorner))
    return theCommands << "Failed to extract lower left corner from: " << input[2].ToString();
  if (!theCommands.GetVectorDoubles(input[3], this->upperRightCorner))
    return theCommands << "Failed to extract upper right corner from: " << input[3].ToString();
  List<int> theGridCount, widthHeightInPixels;
  if (!theCommands.GetVectoRInt(input[4], theGridCount))
    return theCommands << "Failed to extract pair of small integers from: " << input[4].ToString();
  if (theGridCount.size!=2)
    return theCommands << "Failed to extract pair of small integers from: " << input[4].ToString();
  if (!theCommands.GetVectoRInt(input[5], widthHeightInPixels))
    return theCommands << "Failed to extract pair of small integers from: " << input[5].ToString();
  if (widthHeightInPixels.size!=2)
    return theCommands << "Failed to extract pair of small integers from: " << input[5].ToString();
  this->thePlot.DesiredHtmlWidthInPixels=widthHeightInPixels[0];
  this->thePlot.DesiredHtmlWidthInPixels=widthHeightInPixels[1];
  this->XstartingGridCount=theGridCount[0];
  this->YstartingGridCount=theGridCount[1];
  if (input.children.size>=7)
  { if (!input[6].IsSmallInteger(&this->maxNumTriangles))
      return theCommands << "Failed to extract small integer from: " << input[6].ToString();
    if (this->maxNumTriangles>20000)
    { this->maxNumTriangles=20000;
      theCommands << "Max number of triangles decreased from your input: " << input[6].ToString()
      << " to 20000. If you'd like to lift the restriction, modify code around: file: " << __FILE__
      << " line: " << __LINE__ << ". ";
    }
  }
  this->ComputeImplicitPlot();
  theCommands << "Evaluated function in: " << this->numGoodEvaluations << " points and failed to evaluate it at: "
  << this->numBadEvaluations << " points. ";
  return true;
}

bool CalculatorFunctionsGeneral::innerPlotImplicitFunctionFull
(Calculator& theCommands, const Expression& input, Expression& output, bool showGrid)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerPlotImplicitFunctionFull");
  MeshTriangles theMesh;
  if (!theMesh.ComputePoints(theCommands, input, showGrid))
    return false;
  return output.AssignValue(theMesh.thePlot, theCommands);
}

bool CalculatorConversions::innerMatrixDouble(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorConversions::innerMatrixDouble");
//  stOutput << "ere i am, jh";
  Matrix<double> theMat;
  if (!theCommands.GetMatrix(input, theMat, 0, 0, CalculatorFunctionsGeneral::innerEvaluateToDouble))
    return theCommands << "<br>Failed to get matrix of algebraic numbers. ";
  return output.AssignValue(theMat, theCommands);
}

bool CalculatorFunctionsGeneral::innerIntegratePullConstant(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIntegratePullConstant");
  Expression theFunctionE, theVariableE, theSetE;
  if (!input.IsIndefiniteIntegralfdx(&theVariableE, &theFunctionE, &theSetE))
    return false;
  Expression theFunCoeff, theFunNoCoeff;
  theFunctionE.GetCoefficientMultiplicandForm
  (theFunCoeff, theFunNoCoeff);
  if (theFunCoeff.IsEqualToOne())
    return false;
  Expression theNewIntegralE;
  theNewIntegralE.MakeIntegral(theCommands, theSetE, theFunNoCoeff, theVariableE);
  //stOutput << "theNewIntegralE:" << theNewIntegralE.ToString();
  output=theFunCoeff*theNewIntegralE;
  return true;
}

bool CalculatorFunctionsGeneral::innerIntegrateSqrtOneMinusXsquared(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIntegrateSqrtOneMinusXsquared");
  Expression theFunctionE, theVariableE, theSetE;
  if (!input.IsIndefiniteIntegralfdx(&theVariableE, &theFunctionE, &theSetE))
    return false;
  Expression theFunCoeff, theFunNoCoeff;
  theFunctionE.GetCoefficientMultiplicandForm
  (theFunCoeff, theFunNoCoeff);
  if (!theFunNoCoeff.StartsWith(theCommands.opThePower(),3))
    return false;
  if (!theFunNoCoeff[2].IsEqualToHalf())
    return false;
  Expression a, b, c;
  if (!CalculatorFunctionsGeneral::
       extractQuadraticCoeffsWRTvariable
       (theFunNoCoeff[1], theVariableE, a, b, c))
    return false;
  if (!b.IsEqualToZero())
    return false;
  if (!a.IsNegativeConstant())
    return false;
  if (!c.IsPositiveNumber())
    return false;
  Expression theSQRTedCoeff, theRadSquared, theRad;
  theSQRTedCoeff.MakeSqrt(theCommands, a*(-1));
  theFunCoeff*=theSQRTedCoeff;
  theRadSquared=c/a*(-1);
  theRad.MakeSqrt(theCommands, theRadSquared);
  Expression rescaledArgument, arcSinPart, algSQRTPart, algPart;
  rescaledArgument=theVariableE/theRad;
  arcSinPart.MakeOX(theCommands, theCommands.opArcSin(), rescaledArgument);
  arcSinPart*=theRadSquared/2;
  algSQRTPart=theRadSquared-theVariableE*theVariableE;
  algPart.MakeSqrt(theCommands, algSQRTPart);
  algPart*=theVariableE/2;
  output=theFunCoeff*(arcSinPart+algPart);
  return true;
}

bool CalculatorFunctionsGeneral::innerIntegrateXpowerNePowerAx(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIntegrateXpowerNePowerAx");
  Expression theFunctionE, theVariableE, theSetE;
  if (!input.IsIndefiniteIntegralfdx(&theVariableE, &theFunctionE, &theSetE))
    return false;
  if (!theFunctionE.StartsWith(theCommands.opTimes(), 3))
    return false;
  Expression exponentPartE=theFunctionE[1];//<- note: the seemingly odd order is intentional!
  Expression polyPartE=theFunctionE[2]; //<- note: the seemingly odd order is intentional!
  Expression powerOfXE, powerOfEE;
  Expression aE, bE;//exponent is of form aX+b
  powerOfXE.AssignValue(1, theCommands);
  bool isGood=false;
  for (int i=0; i<2; i++)
  { MathRoutines::swap(exponentPartE, polyPartE);
    if (!exponentPartE.StartsWith(theCommands.opThePower(),3))
      continue;
    if (!exponentPartE[1].IsAtomGivenData(theCommands.opE()))
      continue;
    powerOfEE=exponentPartE[2];
    if (!CalculatorFunctionsGeneral::extractLinearCoeffsWRTvariable(powerOfEE, theVariableE, aE, bE))
      continue;
    if (!aE.IsConstantNumber() || !bE.IsConstantNumber())
      continue;
    if (polyPartE!=theVariableE)
    { if (!polyPartE.StartsWith(theCommands.opThePower(),3))
        continue;
      if (polyPartE[1]!=theVariableE)
        continue;
      int theInt=-1;
      if (!polyPartE[2].IsSmallInteger(& theInt))
        continue;
      if (theInt<=0)
        continue;
      powerOfXE=polyPartE[2];
    }
    isGood=true;
    break;
  }
  if (!isGood)
    return false;
  Expression remainingIntegrand, integralPart;
  Expression oneE;
  oneE.AssignValue(1, theCommands);
  remainingIntegrand.MakeXOX(theCommands, theCommands.opThePower(), theVariableE, powerOfXE-oneE);
  remainingIntegrand*=exponentPartE;
  integralPart.MakeIntegral(theCommands, theSetE,remainingIntegrand, theVariableE);
  output= (polyPartE*exponentPartE-powerOfXE*integralPart)/aE;
  return true;
}

bool CalculatorFunctionsGeneral::innerIntegrateSqrtXsquaredMinusOne(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerIntegrateSqrtXsquaredMinusOne");
  Expression theFunctionE, theVariableE, theSetE;
  if (!input.IsIndefiniteIntegralfdx(&theVariableE, &theFunctionE, &theSetE))
    return false;
  Expression theFunCoeff, theFunNoCoeff;
  theFunctionE.GetCoefficientMultiplicandForm
  (theFunCoeff, theFunNoCoeff);
  if (!theFunNoCoeff.StartsWith(theCommands.opThePower(),3))
    return false;
  if (!theFunNoCoeff[2].IsEqualToHalf())
    return false;
  Expression a, b, c;
  if (!CalculatorFunctionsGeneral::
       extractQuadraticCoeffsWRTvariable
       (theFunNoCoeff[1], theVariableE, a, b, c))
    return false;
  if (!b.IsEqualToZero())
    return false;
  if (!a.IsPositiveNumber())
    return false;
  if (!c.IsNegativeConstant())
    return false;

  Expression extraCF, theVarChangeCF, theNewVarE, oneE;
  oneE.AssignValue(1,theCommands);
  extraCF.MakeSqrt(theCommands,c*(-1));
  theFunCoeff*=extraCF;
  theVarChangeCF.MakeSqrt(theCommands, (a/c)*(-1));
  theNewVarE=theVariableE* theVarChangeCF;
  theFunCoeff/=theVarChangeCF;
  Expression algSQRTPart, algPart, lnPart;
  algSQRTPart=theNewVarE*theNewVarE-oneE;
  algPart.MakeSqrt(theCommands, algSQRTPart);
  lnPart.MakeOX
  (theCommands,theCommands.opLog(), theNewVarE-algPart);
  output=theFunCoeff*(algPart*theNewVarE+lnPart)/2;
  return true;
}

bool CalculatorFunctionsGeneral::innerApplyToSubexpressionsRecurseThroughCalculusFunctions
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("innerApplyToSubexpressionsRecurseThroughCalculusFunctions");
  if (input.size()!=3)
    return false;
  if (!input.StartsWithGivenAtom("ApplyToSubexpressionsRecurseThroughCalculusFunctions"))
    return false;
  const Expression& theArg=input[2];
  if (theArg.StartsWith(theCommands.opPlus()) ||
      theArg.StartsWith(theCommands.opMinus()) ||
      theArg.StartsWith(theCommands.opTimes()) ||
      theArg.StartsWith(theCommands.opThePower()) ||
      theArg.StartsWith(theCommands.opDivide()) ||
      theArg.StartsWith(theCommands.opSequence()) ||
      theArg.StartsWith(theCommands.opSqrt()) ||
      theArg.StartsWith(theCommands.opCos()) ||
      theArg.StartsWith(theCommands.opSin())
        )
  { output.reset(theCommands);
    output.AddChildOnTop(input[1]);
    Expression theRecursivelyModifiedE(theCommands), nextE(theCommands);
    theRecursivelyModifiedE.AddChildOnTop(theArg[0]);
    nextE.AddChildAtomOnTop("ApplyToSubexpressionsRecurseThroughCalculusFunctions");
    nextE.AddChildOnTop(input[1]);
    for (int i=1; i<theArg.size(); i++)
    { nextE.children.SetSize(2);
      nextE.AddChildOnTop(theArg[i]);
      theRecursivelyModifiedE.AddChildOnTop(nextE);
    }
    return output.AddChildOnTop(theRecursivelyModifiedE);
  }
  output.reset(theCommands);
  output.AddChildOnTop(input[1]);
  output.AddChildOnTop(theArg);
  return true;
}

bool CalculatorFunctionsGeneral::innerNumerator(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerNumerator");
  if (input.StartsWith(theCommands.opDivide()))
    if (input.children.size>1)
    { output=input[1];
      return true;
    }
  output=input;
  return true;
}

bool CalculatorFunctionsGeneral::innerDenominator(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerDenominator");
  Rational theRat, theDen;
  if (input.IsRational(&theRat))
  { theDen=theRat.GetDenominator();
    return output.AssignValue(theDen, theCommands);
  }
  if (input.StartsWith(theCommands.opDivide()))
    if (input.children.size>2)
    { output=input[2];
      return true;
    }
  return output.AssignValue(1, theCommands);
}

bool CalculatorFunctionsGeneral::innerSumSequence
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerSumSequence");
  if (input.size()<1)
    return false;
  if (input.StartsWith(theCommands.opLimitBoundary()))
    return false;
  if (input.size()==1)
    return output.AssignValue(1, theCommands);
  //stOutput << "DEBUG: making sum from: " << input.ToStringSemiFull();
  if (input[1].StartsWith(theCommands.opLimitBoundary()))
    return false;
  List<Expression> theTerms;
  for (int i=1; i<input.size(); i++)
    theTerms.AddOnTop(input[i]);
  return output.MakeSum(theCommands, theTerms);
}

bool CalculatorFunctionsGeneral::innerMultiplySequence
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerMultiplySequence");
  if (input.size()<1)
    return false;
  if (input.size()==1)
    return output.AssignValue(1, theCommands);
  List<Expression> theTerms;
  for (int i=1; i<input.size(); i++)
    theTerms.AddOnTop(input[i]);
  return output.MakeProducT(theCommands, theTerms);
}

bool CalculatorFunctionsGeneral::innerEnsureExpressionDependsOnlyOnStandard
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerEnsureExpressionDependsOnlyOnStandard");
  if (input.size()<3)
    return false;
  const Expression& theExpression=input[1];
  HashedList<Expression> allowedFreeVars, presentFreeVars;
  allowedFreeVars.SetExpectedSize(input.size()-2);
  presentFreeVars.SetExpectedSize(input.size()-2);
  for (int i=2; i<input.size(); i++)
    allowedFreeVars.AddOnTopNoRepetition(input[i]);
  std::stringstream out;
  theExpression.GetFreeVariables(presentFreeVars, true);
  if (!allowedFreeVars.Contains(presentFreeVars))
  { out << "<hr>";
    out << "Your expression:<br>\\(" << input[1].ToString() << "\\)"
    << "<br><span style='color:red'><b>contains the unexpected variable(s):</b></span><br><b>";
    bool found=false;
    for (int i=0; i< presentFreeVars.size; i++)
      if (!allowedFreeVars.Contains(presentFreeVars[i]))
      { if (found)
          out << ", ";
        found=true;
        out << presentFreeVars[i].ToString();
      }
    out << "</b>.";
    out << "<br>The expected variables are: " << allowedFreeVars.ToStringCommaDelimited() << ". ";
    out << "<br>Beware of typos such as:<br>[wrong:] <span style='color:red'>lnx, sqrt2</span>  "
    << "<br>[correct:] <span style='color:green'>ln(x)</span> or <span style='color:green'>ln x</span>, "
    << "<span style='color:green'>sqrt(2)</span> or <span style='color:green'>sqrt 2</span>.<hr>";
    return output.AssignValue(out.str(), theCommands);
  }
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerSort(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerSort");
  if (!input.IsListStartingWithAtom(theCommands.theAtoms.GetIndexIMustContainTheObject("Sort"))
      &&
      !input.IsSequenceNElementS())
    return false;
  List<Expression> sortedExpressions;
  sortedExpressions.Reserve(input.children.size-1);
  for (int i=1; i<input.children.size; i++)
    sortedExpressions.AddOnTop(input[i]);
/*  stOutput << "<hr>DEBUG";
  if (sortedExpressions.size==3)
  { if (sortedExpressions[0]>sortedExpressions[1])
      stOutput << "<br>" << sortedExpressions[0].ToString() << "&gt;" << sortedExpressions[1].ToString();
    else
      stOutput << "<br>" << sortedExpressions[1].ToString() << "&gt;" << sortedExpressions[0].ToString();
    if (sortedExpressions[0]>sortedExpressions[2])
      stOutput << "<br>" << sortedExpressions[0].ToString() << "&gt;" << sortedExpressions[2].ToString();
    else
      stOutput << "<br>" << sortedExpressions[2].ToString() << "&gt;" << sortedExpressions[0].ToString();
    if (sortedExpressions[1]>sortedExpressions[2])
      stOutput << "<br>" << sortedExpressions[1].ToString() << "&gt;" << sortedExpressions[2].ToString();
    else
      stOutput << "<br>" << sortedExpressions[2].ToString() << "&gt;" << sortedExpressions[1].ToString();
  }
    stOutput << "<hr>";*/
  sortedExpressions.QuickSortAscending();
  return output.MakeSequence(theCommands, &sortedExpressions);
}

bool CalculatorFunctionsGeneral::innerSortDescending
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerSortDescending");
  if (!input.IsListStartingWithAtom(theCommands.theAtoms.GetIndexIMustContainTheObject("SortDescending"))
      &&
      !input.IsSequenceNElementS())
    return false;
  List<Expression> sortedExpressions;
  sortedExpressions.Reserve(input.children.size-1);
  for (int i=1; i<input.children.size; i++)
    sortedExpressions.AddOnTop(input[i]);
  sortedExpressions.QuickSortDescending();
  return output.MakeSequence(theCommands, &sortedExpressions);
}

bool CalculatorFunctionsGeneral::innerLength(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerLength");
  if (input.IsListStartingWithAtom(theCommands.theAtoms.GetIndexIMustContainTheObject("Length")) ||
      input.IsSequenceNElementS())
    return output.AssignValue(input.children.size-1, theCommands);
  return false;
}

bool CalculatorFunctionsGeneral::innerEnsureExpressionDependsOnlyOnMandatoryVariables
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerEnsureExpressionDependsOnlyOnMandatoryVariables");
  if (input.size()<3)
    return false;
  const Expression& theExpression=input[1];
  HashedList<Expression> mandatoryFreeVars, allowedFreeVars, presentFreeVars;
  if (input[2].IsSequenceNElementS())
  { mandatoryFreeVars.SetExpectedSize(input[2].size()-1);
    for (int i=1; i<input[2].size(); i++)
      mandatoryFreeVars.AddOnTop(input[2][i]);
  } else
    mandatoryFreeVars.AddOnTop(input[2]);
  allowedFreeVars.AddOnTop(mandatoryFreeVars);
  if (input.size()>3)
  { if (input[3].IsSequenceNElementS())
      for (int i=1; i<input[3].size(); i++)
        allowedFreeVars.AddOnTop(input[3][i]);
    else
      allowedFreeVars.AddOnTop(input[3]);
  }
  presentFreeVars.SetExpectedSize(input.size()-2);
  theExpression.GetFreeVariables(presentFreeVars, true);
  std::stringstream out;
  if (!presentFreeVars.Contains(mandatoryFreeVars))
  { out << "<hr>";
    out << "Your expression:<br>\\(" << input[1].ToString() << "\\)"
    << "<br><span style='color:red'><b>is required to contain the variables:</b></span><br><b>";
    bool found=false;
    for (int i=0; i< mandatoryFreeVars.size; i++)
      if (!presentFreeVars.Contains(mandatoryFreeVars[i]))
      { if (found)
          out << ", ";
        found=true;
        out << mandatoryFreeVars[i].ToString();
      }
    out << "</b>.";
    out << "<br>The mandatory variable(s) are: " << mandatoryFreeVars.ToStringCommaDelimited() << ". ";
  }
  if (!allowedFreeVars.Contains(presentFreeVars))
  { out << "<hr>";
    out << "Your expression:<br>\\(" << input[1].ToString() << "\\)"
    << "<br><span style='color:red'><b>contains the unexpected variable(s):</b></span><br><b>";
    bool found=false;
    for (int i=0; i< presentFreeVars.size; i++)
      if (!allowedFreeVars.Contains(presentFreeVars[i]))
      { if (found)
          out << ", ";
        found=true;
        out << presentFreeVars[i].ToString();
      }
    out << "</b>.";
    out << "<br>The expected variables are: " << allowedFreeVars.ToStringCommaDelimited() << ". ";
  }
  if (out.str()!="")
    out << "<br>Beware of typos such as:<br>[wrong:] <span style='color:red'>lnx, sqrt2</span>  "
    << "<br>[correct:] <span style='color:green'>ln(x)</span> or <span style='color:green'>ln x</span>, "
    << "<span style='color:green'>sqrt(2)</span> or <span style='color:green'>sqrt 2</span>.<hr>";

  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerPlotLabel
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerPlotLabel");
  if (input.size()!=3)
    return false;
  Vector<double> labelPosition;
  if (!theCommands.GetVectorDoubles(input[1], labelPosition, -1))
    return false;
  std::string theLabel;
  if (!input[2].IsOfType<std::string>(&theLabel))
    theLabel=input[2].ToString();
  PlotObject thePlot;
  thePlot.dimension=labelPosition.size;
  thePlot.thePlotString=theLabel;
  thePlot.thePoints.AddOnTop(labelPosition);
  thePlot.thePlotType="label";
  thePlot.colorJS="black";
  return output.AssignValue(thePlot, theCommands);
}

bool CalculatorFunctionsGeneral::innerPlotRectangle
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerPlotRectangle");
  if (input.size()!=3)
    return false;
  Vectors<double> theRectangle;
  theRectangle.SetSize(2);
  if (!theCommands.GetVectorDoubles(input[1], theRectangle[0],2) || !theCommands.GetVectorDoubles(input[2], theRectangle[1], 2))
    return false;
  PlotObject thePlot;
  thePlot.dimension=2;
  thePlot.thePlotType="pathFilled";
  Vector<double> currentCorner=theRectangle[0];
  Vector<double>& dimensions=theRectangle[1];

  thePlot.thePoints.AddOnTop(currentCorner);
  currentCorner[0]+=dimensions[0];
  thePlot.thePoints.AddOnTop(currentCorner);
  currentCorner[1]+=dimensions[1];
  thePlot.thePoints.AddOnTop(currentCorner);
  currentCorner[0]-=dimensions[0];
  thePlot.thePoints.AddOnTop(currentCorner);
  currentCorner[1]-=dimensions[1];
  thePlot.thePoints.AddOnTop(currentCorner);
  thePlot.colorFillJS="cyan";
  thePlot.colorJS="blue";
  thePlot.thePoints.AddOnTop(currentCorner);
  thePlot.theRectangles.AddOnTop(theRectangle);
  thePlot.colorRGB=CGI::RedGreenBlue(0,0,255);
  thePlot.colorFillRGB=CGI::RedGreenBlue(0,255,255);
  return output.AssignValue(thePlot, theCommands);
}

bool CalculatorFunctionsGeneral::innerSolveUnivariatePolynomialWithRadicalsWRT
(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerSolveUnivariatePolynomialWithRadicalsWRT");
  if (input.children.size!=3)
    return theCommands << "SolveFor takes as input three arguments. ";
  Expression thePowers;
  Expression modifiedInput=input;
  if (!modifiedInput[2].StartsWith(theCommands.opDefine()))
  { if (!CalculatorFunctionsGeneral::innerCoefficientsPowersOf(theCommands, modifiedInput, thePowers))
      return theCommands << "Failed to extract the coefficients of " << modifiedInput[1].ToString()
      << " in " << modifiedInput[2].ToString();
  } else
  { Expression convertedEqualityE, convertedSimplifiedEqualityE;
    if (!CalculatorFunctionsGeneral::innerEqualityToArithmeticExpression(theCommands, modifiedInput[2], convertedEqualityE))
      return theCommands << "Failed to interpret the equality " << modifiedInput[2].ToString();
    if (!Calculator::EvaluateExpression(theCommands, convertedEqualityE, convertedSimplifiedEqualityE))
      return theCommands << "Failed to simplify: " << convertedEqualityE.ToString();
    //stOutput << "DEBUG: here be i; convertedSimplifiedEqualityE: " << convertedSimplifiedEqualityE.ToString() << ", lispified: "
    //<< convertedSimplifiedEqualityE.ToStringSemiFull();
    modifiedInput.SetChilD(2, convertedSimplifiedEqualityE);
    //stOutput << "DEBUG: here be i; modifiedInput: " << modifiedInput.ToString() << ", lispified: "
    //<< modifiedInput.ToStringSemiFull();
    if (!CalculatorFunctionsGeneral::innerCoefficientsPowersOf(theCommands, modifiedInput, thePowers))
      return theCommands << "Failed to extract the coefficients of " << modifiedInput[1].ToString()
      << " in " << modifiedInput[2].ToString() << " which was obtained from the equality "
      << input[2].ToString();
    //stOutput << "DEBUG: here be i; thePowers: " << thePowers.ToString() << ", lispified: "
    //<< thePowers.ToStringSemiFull();
  }
  if (!thePowers.IsSequenceNElementS())
    return theCommands << "This is not supposed to happen: expression "
    << thePowers.ToString() << " should be a list. This may be a programming bug. ";
  if (thePowers.size()==2)
    return theCommands << "Cannot solve: " << modifiedInput[2].ToString()
    << ". The expression does not depend on " << modifiedInput[1].ToString() << ". The coefficients of "
    << modifiedInput[1].ToString() << " are: " << thePowers.ToString();
  if (thePowers.size()==3)
  { output=thePowers[1];
    output*=-1;
    output/=thePowers[2];
    return true;
  }
  if (thePowers.children.size==4)
  { const Expression& a=thePowers[3];
    const Expression& b=thePowers[2];
    const Expression& c=thePowers[1];
    output.MakeSequence(theCommands);
//    stOutput << "DEBIG: a,b,c" << thePowers[3] << thePowers[2] << thePowers[1] << "<br>";
    Expression currentRoot;
    Expression theDiscriminant;
    theDiscriminant=b*b-a*c*4;
    Expression sqrtDiscriminant;
    sqrtDiscriminant.MakeSqrt(theCommands, theDiscriminant, 2);
    currentRoot=(b*(-1)-sqrtDiscriminant)/(a*2);
    output.AddChildOnTop(currentRoot);
    currentRoot=(b*(-1)+sqrtDiscriminant)/(a*2);
    output.AddChildOnTop(currentRoot);
    return true;
  }
  return false;
}

bool CalculatorFunctionsGeneral::innerIntegralUpperBound(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerPowerIntToAny");
  if (!input.StartsWith(theCommands.opThePower(),3))
    return false;
  const Expression& baseE=input[1];
  Expression integrationSet;
  if (baseE.IsAtomGivenData(theCommands.opIntegral()))
  { output.reset(theCommands);
    output.AddChildAtomOnTop(theCommands.opIntegral());
    integrationSet.MakeSequence(theCommands);
    integrationSet.AddChildAtomOnTop(theCommands.opIndefiniteIntegralIndicator());
    integrationSet.AddChildOnTop(input[2]);
    output.AddChildOnTop(integrationSet);
    return true;
  }
  if (!baseE.StartsWith(theCommands.opIntegral(),2))
    return false;
  if (baseE.size()<2)
    return false;
  if (baseE[1].IsAtomGivenData(theCommands.opIndefiniteIntegralIndicator()))
  { integrationSet.MakeSequence(theCommands);
    integrationSet.AddChildAtomOnTop(theCommands.opIndefiniteIntegralIndicator());
    integrationSet.AddChildOnTop(input[2]);
    output=baseE[1];
    return output.SetChilD(1,integrationSet);
  }
  if (baseE[1].IsSequenceNElementS(2))
  { Expression newSet=baseE[1];
    newSet.SetChilD(2,input[2]);
    output=baseE;
    return output.SetChilD(1,newSet);
  }
  return false;
}

bool CalculatorFunctionsGeneral::innerUnderscoreIntWithAny(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerPowerIntToAny");
  if (!input.StartsWith(theCommands.opUnderscore(),3))
    return false;
  const Expression& baseE=input[1];
  Expression integrationSet;
  if (baseE.IsAtomGivenData(theCommands.opIntegral()))
  { output.reset(theCommands);
    output.AddChildAtomOnTop(theCommands.opIntegral());
    integrationSet.MakeSequence(theCommands);
    integrationSet.AddChildOnTop(input[2]);
    integrationSet.AddChildAtomOnTop(theCommands.opIndefiniteIntegralIndicator());
    output.AddChildOnTop(integrationSet);
    return true;
  }
  if (!baseE.StartsWith(theCommands.opIntegral(),2))
    return false;
  if (baseE.size()<2)
    return false;
  if (baseE[1].IsAtomGivenData(theCommands.opIndefiniteIntegralIndicator()))
  { integrationSet.MakeSequence(theCommands);
    integrationSet.AddChildOnTop(input[2]);
    integrationSet.AddChildAtomOnTop(theCommands.opIndefiniteIntegralIndicator());
    output=baseE[1];
    return output.SetChilD(1,integrationSet);
  }
  if (baseE[1].IsSequenceNElementS(2))
  { Expression newSet=baseE[1];
    newSet.SetChilD(1,input[2]);
    output=baseE;
    return output.SetChilD(1,newSet);
  }
  return false;
}

bool CalculatorFunctionsGeneral::innerPowerExponentToLog(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerPowerExponentToLog");
  if (!input.StartsWith(theCommands.opThePower(), 3))
    return false;
  const Expression& baseE=input[1];
  const Expression& powerE=input[2];
  if (baseE.IsAtomGivenData(theCommands.opE()))
    if (powerE.StartsWith(theCommands.opLog(), 2))
    { output=powerE[1];
      return true;
    }
  return false;
}

bool CalculatorFunctionsGeneral::innerSqrt(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("Calculator::innerSqrt");
  if (input.size()!=3)
    return false;
//  stOutput << "<br>Calling sqrt with input: " << input.ToString();
  Rational ratPower;
  if (input[1].IsRational(&ratPower))
    if (ratPower!=0)
    { Expression powerE, powerEreduced, theExponentE;
      ratPower.Invert();
      theExponentE.AssignValue(ratPower, theCommands);
      powerE.MakeXOX(theCommands,theCommands.opThePower(), input[2], theExponentE);
      if (CalculatorFunctionsBinaryOps::innerPowerRatByRatReducePrimeFactors
          (theCommands, powerE, powerEreduced))
        if (powerEreduced!=powerE && powerEreduced!=input)
        { output=powerEreduced;
          return true;
        }
    }
  if (input[2].IsEqualToOne())
    return output.AssignValue(1, theCommands);
  int thePower=0;
  if (!input[1].IsSmallInteger(&thePower))
    return false;
//  stOutput << "<br>the power that be: " << thePower ;
  if (!input[2].IsConstantNumber())
  {// stOutput << "<br>input is: " << input[2].ToString();
    theCommands.CheckInputNotSameAsOutput(input, output);
    Expression theExponent;
    Rational thePowerRat(1, thePower);
    theExponent.AssignValue(thePowerRat, theCommands);
    return output.MakeXOX(theCommands, theCommands.opThePower(), input[2], theExponent);
  }
  if (thePower>0 && input[2].IsEqualToZero())
    return output.AssignValue(0, theCommands);
  if (thePower==0 && input[2].IsEqualToZero())
    return output.AssignValue(1, theCommands);
  Rational rationalValue;
  if (!input[2].IsRational(&rationalValue))
    return false;
  if (thePower<0)
  { if (rationalValue.IsEqualToZero())
      return output.MakeError("Division by zero in expression: " + input.ToString(), theCommands, true);
    thePower*=-1;
    rationalValue.Invert();
  }
  if (thePower!=2)
    return false;
//  stOutput << "<br>Got here: rat value is: " << rationalValue.ToString();
  AlgebraicNumber theNumber;
  if (!theNumber.AssignRationalQuadraticRadical(rationalValue, theCommands.theObjectContainer.theAlgebraicClosure))
    return false;
  return output.AssignValue(theNumber, theCommands);
}

bool CalculatorFunctionsGeneral::innerFloor(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerFloor");
  Rational theRat;
  if(input.IsOfType<Rational>(&theRat))
  { theRat.AssignFloor();
    return output.AssignValue(theRat, theCommands);
  }
  double theDouble=0;
  if (input.EvaluatesToDouble(&theDouble))
    return output.AssignValue((int) std::floor(theDouble), theCommands);
  return false;
}

bool CalculatorFunctionsGeneral::innerPlotSegment(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerPlotSegment");
  if (input.size()<3)
    return false;
//  stOutput << "DEBUG: Here i am";
  const Expression& leftE=input[1];
  const Expression& rightE=input[2];
  Vector<double> leftV, rightV;
  if (!theCommands.GetVectorDoubles(leftE,leftV ) || !theCommands.GetVectorDoubles(rightE, rightV))
    return false;
  if (leftV.size!=rightV.size)
    return false;
  if (leftV.size!=2 && leftV.size!=3)
    return false;
  PlotObject theSegment;
  if (input.size()>=4)
  { theSegment.colorJS="black";
    theSegment.colorRGB=CGI::RedGreenBlue(0,0,0);
    const Expression& colorE=input[3];
    if (!colorE.IsOfType<std::string>(&theSegment.colorJS))
      theSegment.colorJS= colorE.ToString();
    if(!DrawingVariables::GetColorIntFromColorString
        (theSegment.colorJS, theSegment.colorRGB))
      theCommands << "Unrecognized color: " << theSegment.colorJS;
  }
  if (input.size()>=5)
  { const Expression& lineWidthE=input[4];
    if (!lineWidthE.EvaluatesToDouble(&theSegment.lineWidth))
      theCommands << "Failed to extract line width from: "
      << lineWidthE.ToString();
    std::stringstream lineWidthStream;
    lineWidthStream.precision(4);
    lineWidthStream << theSegment.lineWidth;
    theSegment.lineWidthJS=lineWidthStream.str();
  }
  theSegment.thePlotType="segment";
  if (leftV.size==3)
    theSegment.dimension=3;
  else
    theSegment.dimension=2;
  theSegment.thePoints.AddOnTop(leftV);
  theSegment.thePoints.AddOnTop(rightV);
  if (input.size()>=5)
    if (!input[4].EvaluatesToDouble(&theSegment.lineWidth))
      theSegment.lineWidth=1;
  Plot thePlot;
  thePlot+=theSegment;
  return output.AssignValue(thePlot, theCommands);
}

bool CalculatorFunctionsGeneral::innerThaw(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerThaw");
  if (!input.StartsWith(theCommands.opFreeze()))
  { output=input;
    return true;
  }
  if (input.size()>2)
  { output=input;
    return output.SetChildAtomValue(0, theCommands.opSequence());
  }
  if (input.size()==2)
  { output=input[1];
    return true;
  }
  return false;
}

bool CalculatorFunctionsGeneral::innerLCM(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerLCM");
  if (input.size()<3)
    return false;
  Vector<LargeInt> theInts;
  if (!theCommands.GetVectorLargeIntFromFunctionArguments(input, theInts))
    return false;
  if (theInts.size<1)//<-this shouldn't happen if GetVectorLargeIntFromFunctionArguments works as intended.
    return false;
  LargeIntUnsigned theResult=theInts[0].value;
  if (theResult==0)
    return false;
  for (int i=1; i<theInts.size; i++)
  { if (theInts[i].value==0)
      return false;
    theResult=LargeIntUnsigned::lcm(theResult, theInts[i].value);
  }
  return output.AssignValue(theResult, theCommands);
}

bool CalculatorFunctionsGeneral::innerGCD(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerGCD");
  if (input.size()<3)
    return false;
  Vector<LargeInt> theInts;
  if (!theCommands.GetVectorLargeIntFromFunctionArguments(input, theInts))
    return false;
  if (theInts.size<1)//<-this shouldn't happen if GetVectorLargeIntFromFunctionArguments works as intended.
    return false;
  LargeIntUnsigned theResult=theInts[0].value;
  if (theResult==0)
    return false;
  for (int i=1; i<theInts.size; i++)
  { if (theInts[i].value==0)
      return false;
    theResult=LargeIntUnsigned::gcd(theResult, theInts[i].value);
  }
  return output.AssignValue(theResult, theCommands);
}

bool CalculatorFunctionsGeneral::innerLogBaseSimpleCases(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerLogBaseSimpleCases");
  //stOutput << "reducing ...." << input.ToStringSemiFull();
  if (!input.StartsWith(theCommands.opLogBase(), 3))
    return false;
  Rational theBase, theArg;
  if (!input[1].IsOfType<Rational>(&theBase) || !input[2].IsOfType<Rational>(&theArg))
    return false;
  if (theBase<0 || theArg<0)
    return false;
  if (theBase==1)
    return false;
  bool changed=false;
  int theSign=1;
  if (theBase<1)
  { theBase.Invert();
    theSign*=-1;
    changed=true;
  }
  if (theArg==1)
    return output.AssignValue(0, theCommands);
  if (theArg<1)
  { theArg.Invert();
    theSign*=-1;
    changed=true;
  }
  LargeInt baseInt, argNum;
  if (!theBase.IsInteger(&baseInt))
    return false;
  argNum=theArg.GetNumerator();
  LargeInt argDen=theArg.GetDenominator();
  double doubleBase= baseInt.GetDoubleValue();
  double doubleArgNum=argNum.GetDoubleValue();
  if (FloatingPoint::log(doubleArgNum)/FloatingPoint::log(doubleBase)>1000)
    return false;
  int intPart=0;
  while (argNum% baseInt ==0)
  { intPart++;
    argNum/=baseInt;
    changed=true;
  }
  while (argDen% baseInt ==0)
  { intPart--;
    argDen/=baseInt;
    changed=true;
  }
  if (!changed)
    return false;
  intPart*=theSign;
  theArg=argNum;
  theArg/=argDen;
  Expression logPartE, newBaseE, newArgE;
  newBaseE.AssignValue(theBase, theCommands);
  newArgE.AssignValue(theArg, theCommands);
  logPartE.MakeXOX(theCommands, theCommands.opLogBase(), newBaseE, newArgE);
  if (theSign<0)
  { Expression mOneE;
    mOneE.AssignValue(-1, theCommands);
    logPartE*=mOneE;
  }
  if (intPart==0)
  { output=logPartE;
    return true;
  }
  Expression intPartE;
  intPartE.AssignValue(intPart, theCommands);
  output=intPartE+logPartE;
  return true;
}

std::string InputBox::GetSliderName()const
{ return this->name+ Crypto::computeSha1outputBase64(this->name);
}

std::string InputBox::GetUserInputBox()const
{ MacroRegisterFunctionWithName("InputBox::GetUserInputBox");
  std::stringstream out;
  double theReader=0;
  out.precision(4);
  out << std::fixed;
  if (this->value.EvaluatesToDouble(&theReader))
    out << "\\FormInput" << "[" << theReader
    << "]" << "{" << this->name << "}" ;
  else
    out << "\\FormInput" << "[" << this->value.ToString()
    << "]" << "{" << this->name << "}" ;
  return out.str();
}

bool CalculatorFunctionsGeneral::innerMakeJavascriptExpression(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerMakeJavascriptExpression");
  RecursionDepthCounter theCounter(&theCommands.RecursionDeptH);
  if (theCommands.RecursionDepthExceededHandleRoughly())
    return false;
  std::string atomString;
  if (input.IsAtom(&atomString))
  { if (input.IsAtomGivenData(theCommands.opE()))
      return output.AssignValue<std::string>(" 2.718281828 ", theCommands);
    if (input.IsAtomGivenData(theCommands.opPi()))
      return output.AssignValue<std::string>(" 3.141592654 ", theCommands);
    if (input.theData>=theCommands.NumPredefinedAtoms)
      return output.AssignValue(HtmlSnippets::GetJavascriptVariable(atomString), theCommands);
    if (atomString=="+" || atomString=="*" || atomString=="/" || atomString=="-")
      return output.AssignValue(atomString, theCommands);
    return output.AssignValue(atomString, theCommands);
  }
  std::stringstream out;
  InputBox theBox;
  if (input.IsOfType(&theBox))
  { out << "parseFloat(document.getElementById('"
    << theBox.GetSliderName() << "').value)";
    return output.AssignValue(out.str(),theCommands);
  }
  out.precision(7);
  bool hasDoubleValue=false;;
  double theDoubleValue=-1;
  if (input.IsOfType<Rational>())
  { hasDoubleValue=true;
    theDoubleValue=input.GetValue<Rational>().GetDoubleValue();
  }
  if (input.IsOfType<AlgebraicNumber>())
    hasDoubleValue=input.GetValue<AlgebraicNumber>().EvaluatesToDouble(&theDoubleValue);
  if (input.IsOfType<double>())
  { hasDoubleValue=true;
    theDoubleValue=input.GetValue<double>();
  }
  if (hasDoubleValue)
  { std::string theDoubleString=FloatingPoint::DoubleToString(theDoubleValue);
    if (theDoubleString.size()>0)
    { if (theDoubleString[0]=='-')
        out << "(" << theDoubleString << ")";
      else
        out << " " << theDoubleString;
    }
    return output.AssignValue(out.str(), theCommands);
  }
  Expression opE, leftE, rightE;
  std::string opString, leftString, rightString;
  std::stringstream logStream;
  if (input.size()==3 || input.size()==2)
  { Expression* currentE=&opE;
    std::string* currentString=&opString;
    for (int i=0; i<input.size(); i++)
    { if (!CalculatorFunctionsGeneral::innerMakeJavascriptExpression(theCommands, input[i], *currentE))
        return output.AssignValue("(Failed to convert "+input[i].ToString()+")", theCommands);
      if (!currentE->IsOfType(currentString))
        return output.AssignValue("(Failed to convert "+input[i].ToString()+")", theCommands);
      logStream << "Converted: " << input[i].ToString() << " to: "
      << *currentString << ". ";
      if (i==0)
      { currentE=&leftE;
        currentString=&leftString;
      }
      if (i==1)
      { currentE=&rightE;
        currentString=&rightString;
      }
    }
    if (input.size()==3)
    { if (opString=="+" || opString=="-" || opString=="/" || opString=="*")
      { out << "(" << leftString <<  " " << opString << " " << rightString << ")";
        return output.AssignValue(out.str(), theCommands);
      }
      if (opString=="\\sqrt")
      { out << "Math.pow(" << rightString << ", 1/" << leftString << ")";
        return output.AssignValue(out.str(), theCommands);
      }
      if (opString=="^")
      { out << "Math.pow(" << leftString << ", " << rightString << ")";
        return output.AssignValue(out.str(), theCommands);
      }
    }
    if (input.size()==2)
      if (opString=="\\sin" || opString == "\\cos" || opString=="\\log")
      { std::string chopped=opString.substr(1);
        out << "(Math." << chopped << "( " << leftString << "))";
        return output.AssignValue(out.str(), theCommands);
      }
  }
  out << "(Failed to make expression from " << input.ToString() << ". "
  << logStream.str() << ")";
  return output.AssignValue(out.str(), theCommands);
}

bool CalculatorFunctionsGeneral::innerPlotSetProjectionScreenBasis(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerPlotSetProjectionScreenBasis");
  if (input.size()!=3)
    return false;
  Vector<double> v1, v2;
  if (!theCommands.GetVectorDoubles(input[1], v1, 3) ||
      !theCommands.GetVectorDoubles(input[2], v2, 3) )
    return theCommands << "Failed to extract 3d-vectors from "
    << input[1].ToString() << ", "
    << input[2].ToString() << ".";
  Plot resultPlot;
  resultPlot.dimension=3;
  PlotObject thePlot;
  thePlot.thePlotType="setProjectionScreen";
  thePlot.thePoints.AddOnTop(v1);
  thePlot.thePoints.AddOnTop(v2);
  resultPlot+=thePlot;
  return output.AssignValue(resultPlot, theCommands);
}

bool CalculatorFunctionsGeneral::innerPlotCoordinateSystem(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerPlotCoordinateSystem");
  if (input.size()!=3)
    return false;
  Vector<double> corner1, corner2;
  if (!theCommands.GetVectorDoubles(input[1], corner1, 3) ||
      !theCommands.GetVectorDoubles(input[2], corner2, 3) )
    return theCommands << "Failed to extract 3d-vectors from "
    << input[1].ToString() << ", "
    << input[2].ToString() << ".";
  Plot resultPlot;
  resultPlot.dimension=3;
  PlotObject thePlot;
  thePlot.colorJS="black";
  thePlot.thePlotType="segment";
  thePlot.thePoints.SetSize(2);
  for (int i=0; i<3; i++)
  { thePlot.thePoints[0].MakeZero(3);
    thePlot.thePoints[1].MakeZero(3);
    thePlot.thePoints[0][i]=corner1[i];
    thePlot.thePoints[1][i]=corner2[i];
    resultPlot+=(thePlot);
  }
  PlotObject plotLabels;
  plotLabels.thePlotType="label";
  plotLabels.thePoints.SetSize(1);
  plotLabels.colorJS="blue";
  for (unsigned char i=0; i<3; i++)
  { plotLabels.thePoints[0].MakeZero(3);
    plotLabels.thePoints[0][i]=corner2[i];
    std::stringstream out;
    out << (unsigned char) ('x'+i);
    plotLabels.thePlotString=out.str();
    resultPlot+=plotLabels;
  }
  return output.AssignValue(resultPlot, theCommands);
}

bool CalculatorFunctionsGeneral::innerPlotSurface(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerPlotSurface");
  PlotObject thePlot;
  bool found=false;
  for (int i=0; i<input.size(); i++)
    if (input[i].IsSequenceNElementS(3))
    { thePlot.theSurface=input[i];
      found=true;
      break;
    }
  if (!found)
    return theCommands << "Could not find a triple of functions expressions to use for "
    << " the surface. ";
  thePlot.theSurface.GetFreeVariables(thePlot.variablesInPlay, true);
  if (thePlot.variablesInPlay.size>2)
    return theCommands << "Got a surface with "
    << thePlot.variablesInPlay.size
    << " variables, namely: "
    << thePlot.variablesInPlay.ToStringCommaDelimited()
    << ". I've been taught to plot 2d surfaces only. "
    << " Please reduce the number of variables to 2. ";
  Expression uE,vE;
  uE.MakeAtom("u", theCommands);
  vE.MakeAtom("v", theCommands);
  if (thePlot.variablesInPlay.size==1)
  { if (thePlot.variablesInPlay.Contains(vE))
      thePlot.variablesInPlay.AddOnTop(uE);
    else
      thePlot.variablesInPlay.AddOnTop(vE);
  }
  if (thePlot.variablesInPlay.size==0)
  { thePlot.variablesInPlay.AddOnTop(uE);
    thePlot.variablesInPlay.AddOnTop(vE);
  }
  thePlot.variablesInPlay.QuickSortAscending();
  thePlot.coordinateFunctionsE.SetSize(thePlot.theSurface.size()-1);
  thePlot.coordinateFunctionsJS.SetSize(thePlot.coordinateFunctionsE.size);
  thePlot.theVarRangesJS.SetSize(2);
  thePlot.variablesInPlayJS.SetSize(2);
  for (int i=0; i<2; i++)
  { thePlot.theVarRangesJS[i].SetSize(2);
    thePlot.variablesInPlayJS[i]=thePlot.variablesInPlay[i].ToString();
  }
  Expression jsConverter;
  for (int i=1; i<thePlot.theSurface.size(); i++)
  { thePlot.coordinateFunctionsE[i-1]=thePlot.theSurface[i];
    bool isGood=CalculatorFunctionsGeneral::innerMakeJavascriptExpression
    (theCommands, thePlot.coordinateFunctionsE[i-1], jsConverter);
    if (isGood)
      isGood= jsConverter.IsOfType<std::string>
      (&thePlot.coordinateFunctionsJS[i-1]);
    if (!isGood)
      return theCommands << "Failed to convert "
      << thePlot.coordinateFunctionsE[i-1].ToString()
      << " to a javascript expression. ";
  }
  for (int i =1; i<input.size(); i++)
    if (input[i].StartsWith(theCommands.opIn(), 3))
    { int theIndex=thePlot.variablesInPlay.GetIndex(input[i][1]);
      if (theIndex<0 || theIndex>2) //theIndex>2 should never happen
        continue;
      if (input[i][2].size()!=3)
        continue;
      for (int j=0; j<2; j++)
      { bool isGood=CalculatorFunctionsGeneral::innerMakeJavascriptExpression
        (theCommands, input[i][2][j+1], jsConverter);
        if (isGood)
          isGood=jsConverter.IsOfType<std::string>(&thePlot.theVarRangesJS[theIndex][j]);
        if (!isGood)
          return theCommands << "Failed to convert "
          << input[i][2][j+1].ToString() << " to a javascript expression. ";
      }
    }
  MapLisT<std::string, Expression, MathRoutines::hashString> theKeys;
  if (CalculatorConversions::innerLoadKeysFromStatementList(theCommands, input, theKeys, &theCommands.Comments, true))
  { if (theKeys.Contains("color1"))
      thePlot.colorUV=theKeys.GetValueCreateIfNotPresent("color1").ToString();
    if (theKeys.Contains("color2"))
      thePlot.colorVU=theKeys.GetValueCreateIfNotPresent("color2").ToString();
    MapLisT<std::string, std::string, MathRoutines::hashString>
    keysToConvert;
    keysToConvert.GetValueCreateIfNotPresent("numSegments1");
    keysToConvert.GetValueCreateIfNotPresent("numSegments2");
    keysToConvert.GetValueCreateIfNotPresent("lineWidth");
    for (int i=0; i<keysToConvert.size(); i++)
    { if (!theKeys.Contains(keysToConvert.theKeys[i]))
        continue;
      Expression expressionToConvert=theKeys.GetValueCreateIfNotPresent(keysToConvert.theKeys[i]);
      bool isGood=CalculatorFunctionsGeneral::innerMakeJavascriptExpression
      (theCommands, expressionToConvert, jsConverter);
      if (isGood )
        isGood= jsConverter.IsOfType<std::string>(&keysToConvert.theValues[i]);
      if (!isGood)
        return theCommands << "Failed to convert "
        << expressionToConvert.ToString()
        << " to a javascript expression. ";
    }
    if(keysToConvert.GetValueCreateIfNotPresent("numSegments1")!="")
      thePlot.numSegmentsU=keysToConvert.GetValueCreateIfNotPresent("numSegments1");
    if(keysToConvert.GetValueCreateIfNotPresent("numSegments2")!="")
      thePlot.numSegmentsV=keysToConvert.GetValueCreateIfNotPresent("numSegments2");
    if(keysToConvert.GetValueCreateIfNotPresent("lineWidth")!="")
    { thePlot.lineWidthJS=keysToConvert.GetValueCreateIfNotPresent("lineWidth");
      //stOutput << "DEBUG: line width set to: " << thePlot.lineWidthJS;
    }
  }
  if (thePlot.theVarRangesJS[0][0]=="" || thePlot.theVarRangesJS[0][1]=="" ||
      thePlot.theVarRangesJS[1][0]=="" || thePlot.theVarRangesJS[1][1]=="")
  { return theCommands << "Could not extract variable ranges, got the var ranges: "
    << thePlot.theVarRangesJS;
  }
  thePlot.thePlotType="surface";
  thePlot.dimension=thePlot.coordinateFunctionsE.size;
  Plot result;
  result+=thePlot;
  input.HasInputBoxVariables(&result.boxesThatUpdateMe);
  return output.AssignValue(result, theCommands);
}

class QRFactorizationComputation{


};

bool CalculatorFunctionsGeneral::innerGramSchmidtVerbose(Calculator& theCommands, const Expression& input, Expression& output)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerGramSchmidtVerbose");
  Matrix<AlgebraicNumber> theMatAlg;
  bool matAlgWorks=false;
  if (input.IsOfType<Matrix<AlgebraicNumber> >(&theMatAlg))
    matAlgWorks=true;
  if (!matAlgWorks)
    if (theCommands.GetMatriXFromArguments(input, theMatAlg, 0, -1, 0))
      matAlgWorks=true;
  if (matAlgWorks)
  { if (theMatAlg.NumRows!=theMatAlg.NumCols || theMatAlg.NumCols<1)
      return output.MakeError("The matrix is not square", theCommands, true);
    if (theMatAlg.GetDeterminant()==0)
      return output.MakeError("Matrix determinant is zero.", theCommands, true);
    QRFactorizationComputation theComputation;

    theMatAlg.Invert();
    return output.AssignValue(theMatAlg, theCommands);
  }
  return theCommands << "<hr>Failed to extract algebraic number matrix from: "
  << input.ToString();
}
