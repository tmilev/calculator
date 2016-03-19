//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpf.h"
#include "vpfHeader3Calculator2_InnerFunctions.h"

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
  MapList<double, Vector<double>, MathRoutines::HashVectorDoubles> theEvaluatedPoints;
  HashedList<Expression> knownEs;
  List<double> knownValues;
  Vector<double> lowerLeftCorner, upperRightCorner;
  PlotObject theGrid;
  PlotObject theCurve;
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
};

void MeshTriangles::PlotGrid(int theColor)
{ MacroRegisterFunctionWithName("MeshTriangles::PlotGrid");
  this->theGrid.theLines.SetSize(0);
  this->theGrid.theLines.SetExpectedSize(this->theGrid.theLines.size+this->theTriangles.size*3);
  List<Vector<double> > currentLine;
  currentLine.SetSize(2);
  for (int i=0; i<this->theTriangles.size; i++)
  { currentLine[0]=this->theTriangles[i][0];
    currentLine[1]=this->theTriangles[i][1];
    this->theGrid.theLines.AddOnTop(currentLine);
    currentLine[0]=this->theTriangles[i][1];
    currentLine[1]=this->theTriangles[i][2];
    this->theGrid.theLines.AddOnTop(currentLine);
    currentLine[0]=this->theTriangles[i][2];
    currentLine[1]=this->theTriangles[i][0];
    this->theGrid.theLines.AddOnTop(currentLine);
  }
  this->theGrid.colorRGB=theColor;
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
  this->theEvaluatedPoints.SetValue(functionValue, thePoint);
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
  Vectors<double> theSegment;
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
    this->theCurve.theLines.AddOnTop(theSegment);
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
    this->thePlot.thePlots.AddOnTop(this->theGrid);
  }
  this->ComputeImplicitPlotPart2();
  if (this->flagShowGrid)
  { this->PlotGrid(CGI::RedGreenBlue(100, 100, 100));
    this->thePlot.thePlots.AddOnTop(this->theGrid);
  }
  this->theCurve.colorRGB=CGI::RedGreenBlue(255,0,0);
  this->thePlot.thePlots.AddOnTop(this->theCurve);
//  stOutput << "ze lines: " << this->theCurve.theLines;
}

bool Calculator::GetVectorDoubles(const Expression& input, Vector<double>& output)
{ return this->GetVectoR(input, output, 0, 0, CalculatorFunctionsGeneral::innerEvaluateToDouble);
}

bool CalculatorFunctionsGeneral::innerPlotImplicitFunction(Calculator& theCommands, const Expression& input, Expression& output)
{ return CalculatorFunctionsGeneral::innerPlotImplicitFunctionFull(theCommands, input, output, false);
}

bool CalculatorFunctionsGeneral::innerPlotImplicitShowGridFunction(Calculator& theCommands, const Expression& input, Expression& output)
{ return CalculatorFunctionsGeneral::innerPlotImplicitFunctionFull(theCommands, input, output, true);
}

bool CalculatorFunctionsGeneral::innerPlotImplicitFunctionFull(Calculator& theCommands, const Expression& input, Expression& output, bool showGrid)
{ MacroRegisterFunctionWithName("CalculatorFunctionsGeneral::innerPlotImplicitFunction");
  if (input.children.size<6)
    return false;
  MeshTriangles theMesh;
  theMesh.flagShowGrid=showGrid;
  theMesh.owner=&theCommands;
  theMesh.theFun=input[1];
  theMesh.knownEs=theCommands.knownDoubleConstants;
  theMesh.knownValues=theCommands.knownDoubleConstantValues;
  theMesh.knownValues.AddOnTop(0);
  theMesh.knownValues.AddOnTop(0);
  Expression xE, yE;
  xE.MakeAtom("x", theCommands);
  yE.MakeAtom("y", theCommands);
  theMesh.knownEs.AddOnTopNoRepetitionMustBeNewCrashIfNot(xE);
  theMesh.knownEs.AddOnTopNoRepetitionMustBeNewCrashIfNot(yE);
  if (!theCommands.GetVectorDoubles(input[2], theMesh.lowerLeftCorner))
    return theCommands << "Failed to extract lower left corner from: " << input[2].ToString();
  if (!theCommands.GetVectorDoubles(input[3], theMesh.upperRightCorner))
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
  theMesh.thePlot.DesiredHtmlWidthInPixels=widthHeightInPixels[0];
  theMesh.thePlot.DesiredHtmlWidthInPixels=widthHeightInPixels[1];
  theMesh.XstartingGridCount=theGridCount[0];
  theMesh.YstartingGridCount=theGridCount[1];
  if (input.children.size>=7)
  { if (!input[6].IsSmallInteger(&theMesh.maxNumTriangles))
      return theCommands << "Failed to extract small integer from: " << input[6].ToString();
    if (theMesh.maxNumTriangles>20000)
    { theMesh.maxNumTriangles=20000;
      theCommands << "Max number of triangles decreased from your input: " << input[6].ToString()
      << " to 20000. If you'd like to lift the restriction, modify code around: file: " << __FILE__
      << " line: " << __LINE__ << ". ";
    }
  }
  theMesh.ComputeImplicitPlot();
  theCommands << "Evaluated function in: " << theMesh.numGoodEvaluations << " points and failed to evaluate it at: "
  << theMesh.numBadEvaluations << " points. ";
  return output.AssignValue(theMesh.thePlot, theCommands);
}
