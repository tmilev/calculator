// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"
#include "calculator_inner_functions.h"
#include "calculator_inner_typed_functions.h"
#include "math_extra_drawing_variables.h"
#include "calculator_html_functions.h"
#include "crypto.h"
#include "web_api.h"
#include "math_general_polynomial_computations_advanced_implementation.h"
#include <cmath>

class MeshTriangles {
public:
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
  List<bool> trianglesUsed;
  List<List<Vector<double> > > theTriangles;
  MapList<Vector<double>, double, MathRoutines::hashVectorDoubles> theEvaluatedPoints;
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
  void plotGrid(int theColor);
  bool evaluatesToDouble(double& whichDouble);
  double GetValueAtPoint(const Vector<double>& thePoint);
  void EvaluateFunAtTriangleVertices(int triangleIndex);
  double GetTriangleMaxSideLength(int triangleIndex);
  void subdivide(int triangleIndex);
  void AddPointFromVerticesValues(
    Vectors<double>& outputAppend,
    const Vector<double>& left,
    const Vector<double>& right,
    double leftVal,
    double rightVal
  );
  bool ComputePoints(Calculator& theCommands, const Expression& input, bool showGrid);
  MeshTriangles();
};

MeshTriangles::MeshTriangles() {
  this->XstartingGridCount = 0;
  this->YstartingGridCount = 0;
  this->numBadEvaluations = 0;
  this->numGoodEvaluations = 0;
  this->minTriangleSideAsPercentOfWidthPlusHeight = 0.001;
  this->maxNumTriangles = 2000;
  this->flagTriangleLimitReached = false;
  this->flagShowGrid = false;
  this->flagFunctionEvaluationFailed = false;
}

void MeshTriangles::plotGrid(int theColor) {
  MacroRegisterFunctionWithName("MeshTriangles::plotGrid");
  this->theGrid.thePlots.setSize(0);
  this->theGrid.thePlots.setExpectedSize(this->theGrid.thePlots.size + this->theTriangles.size * 3);
  PlotObject currentLinePlot;
  List<Vector<double> >& pointsVector = currentLinePlot.thePointsDouble;
  currentLinePlot.thePointsDouble.setSize(4);
  currentLinePlot.colorRGB = theColor;
  for (int i = 0; i < this->theTriangles.size; i ++) {
    pointsVector[0] = this->theTriangles[i][0];
    pointsVector[1] = this->theTriangles[i][1];
    pointsVector[2] = this->theTriangles[i][2];
    pointsVector[3] = this->theTriangles[i][0];
    this->theGrid.thePlots.addOnTop(currentLinePlot);
  }
}

double MeshTriangles::GetValueAtPoint(const Vector<double>& thePoint) {
  int theIndex = this->theEvaluatedPoints.getIndex(thePoint);
  if (theIndex != - 1) {
    return this->theEvaluatedPoints.theValues[theIndex];
  }
  this->knownValues[this->knownValues.size - 2] = thePoint[0];
  this->knownValues[this->knownValues.size - 1] = thePoint[1];
  double functionValue = 0;
  if (this->theFun.evaluatesToDoubleUnderSubstitutions(this->knownEs, this->knownValues, &functionValue)) {
    this->numGoodEvaluations ++;
  } else {
    this->numBadEvaluations ++;
    functionValue = std::nan("");
  }
  this->theEvaluatedPoints.setKeyValue(thePoint, functionValue);
  return functionValue;
}

void MeshTriangles::EvaluateFunAtTriangleVertices(int triangleIndex) {
  MacroRegisterFunctionWithName("MeshTriangles::EvaluateFunAtTriangleVertices");
  for (int j = 0; j < this->theTriangles[triangleIndex].size; j ++) {
    this->GetValueAtPoint(this->theTriangles[triangleIndex][j]);
  }
}

double MeshTriangles::GetTriangleMaxSideLength(int triangleIndex) {
  MacroRegisterFunctionWithName("MeshTriangles::GetTriangleMaxSideLength");
  if (this->theTriangles[triangleIndex].size != 3) {
    global.fatal << "Error: triangle needs three vertices, instead it has vertices: "
    << this->theTriangles[triangleIndex] << global.fatal;
  }
  Vector<double>& firstV = this->theTriangles[triangleIndex][0];
  Vector<double>& secondV = this->theTriangles[triangleIndex][1];
  Vector<double>& thirdV = this->theTriangles[triangleIndex][2];
  List<Vector<double> > theSides;
  theSides.addOnTop(firstV - secondV);
  theSides.addOnTop(secondV - thirdV);
  theSides.addOnTop(thirdV - firstV);
  double result = 0;
  for (int i = 0; i < theSides.size; i ++) {
    double normSquared = theSides[i][0] * theSides[i][0] + theSides[i][1] * theSides[i][1];
    result = MathRoutines::maximum(result, FloatingPoint::sqrtFloating(normSquared));
  }
  return result;
}

void MeshTriangles::AddPointFromVerticesValues(
  Vectors<double>& outputAppend,
  const Vector<double>& left,
  const Vector<double>& right,
  double leftVal,
  double rightVal
) {
  //MacroRegisterFunctionWithName("MeshTriangles::AddPointFromVerticesValues");
  double Delta = leftVal - rightVal;
  if (Delta == 0.0) {
    outputAppend.addOnTop(left);
    return;
  }
  double contributionLeft = rightVal / Delta;
  if (contributionLeft < 0) {
    contributionLeft *= - 1;
  }
  double contributionRight = leftVal / Delta;
  if (contributionRight < 0) {
    contributionRight *= - 1;
  }
  outputAppend.addOnTop(left * contributionLeft + right * contributionRight);
}

int MeshTriangles::CleanUpTrianglesReturnUpdatedCurrentIndex(int currentIndex) {
  MacroRegisterFunctionWithName("MeshTriangles::CleanUpTrianglesReturnUpdatedCurrentIndex");
  if (this->flagTriangleLimitReached || this->theTriangles.size< this->maxNumTriangles * 2) {
    return currentIndex;
  }
  // clean up carried out only when we have twice
  // exceeded the triangle limit
  // this way clean up will be carried only after
  // we have generated at least maxNumTriangles since the last cleanup
  // (which leaves no more than maxNumTriangles alive).
  if (this->theTriangles.size != this->trianglesUsed.size) {
    global.fatal << "this->theTriangles.size is not equal to this->trianglesUsed.size: the values are: "
    << this->theTriangles.size << " and " << this->trianglesUsed.size << ". " << global.fatal;
  }
  int lowestFree = 0;
  for (int i = 0; i < this->theTriangles.size; i ++) {
    if (this->trianglesUsed[i]) {
      this->theTriangles[lowestFree] = this->theTriangles[i];
      this->trianglesUsed[lowestFree] = true;
      if (i == currentIndex) {
        currentIndex = lowestFree;
      }
      lowestFree ++;
    }
  }
  this->theTriangles.setSize(lowestFree);
  this->trianglesUsed.setSize(lowestFree);
  if (lowestFree > this->maxNumTriangles) {
    this->flagTriangleLimitReached = true;
  }
  return currentIndex;
}

void MeshTriangles::subdivide(int triangleIndex) {
  MacroRegisterFunctionWithName("MeshTriangles::subdivide");
  List<Vector<double> > currentTriangle = this->theTriangles[triangleIndex];
  if (currentTriangle.size != 3) {
    global.fatal << "Triangle in mesh with less than 3 sides! " << global.fatal;
  }
  List<Vector<double> > insideTriange;
  insideTriange.setSize(3);
  insideTriange[0] = (currentTriangle[1] + currentTriangle[2]) * 0.5;
  insideTriange[1] = (currentTriangle[2] + currentTriangle[0]) * 0.5;
  insideTriange[2] = (currentTriangle[0] + currentTriangle[1]) * 0.5;
  this->theTriangles.addOnTop(insideTriange);
  this->trianglesUsed.addOnTop(true);
  List<Vector<double> > newTriangle;
  newTriangle = insideTriange;
  newTriangle[0] = currentTriangle[0];
  this->theTriangles.addOnTop(newTriangle);
  this->trianglesUsed.addOnTop(true);
  newTriangle = insideTriange;
  newTriangle[1] = currentTriangle[1];
  this->theTriangles.addOnTop(newTriangle);
  this->trianglesUsed.addOnTop(true);
  newTriangle = insideTriange;
  newTriangle[2] = currentTriangle[2];
  this->theTriangles.addOnTop(newTriangle);
  this->trianglesUsed.addOnTop(true);
}

void MeshTriangles::ComputeImplicitPlotPart2() {
  MacroRegisterFunctionWithName("MeshTriangles::ComputeImplicitPlotPart2");
  this->theTriangles.setExpectedSize(this->maxNumTriangles * 2);
  this->trianglesUsed.setExpectedSize(this->maxNumTriangles * 2);
  this->trianglesUsed.initializeFillInObject(this->theTriangles.size, true);
  this->theEvaluatedPoints.setExpectedSize(this->maxNumTriangles * 4);
  this->flagTriangleLimitReached = false;
  for (int i = 0; i < this->theTriangles.size; i ++) {
    this->EvaluateFunAtTriangleVertices(i);
  }
  if (this->numGoodEvaluations < 5) {
    this->flagFunctionEvaluationFailed = true;
    return;
  }
  double minSide = MathRoutines::minimum(this->Height, this->Width) * this->minTriangleSideAsPercentOfWidthPlusHeight;
  PlotObject currentPlot;
  currentPlot.colorRGB = static_cast<int>(HtmlRoutines::RedGreenBlue(255, 0, 0));
  Vectors<double>& theSegment = currentPlot.thePointsDouble;
  List<Vector<double> > currentTriangle;
  for (int i = 0; i < this->theTriangles.size; i ++) {
    currentTriangle = this->theTriangles[i]; //making a copy in case this->theTriangles changes underneath.
    if (currentTriangle.size != 3) {
      global.fatal << "Error: triangle needs three vertices, instead it has vertices: " << currentTriangle << global.fatal;
    }
    bool isGood = true;
    for (int j = 0; j < currentTriangle.size; j ++) {
      if (this->GetValueAtPoint(currentTriangle[j]) == std::nan("")) {
        isGood = false;
        break;
      }
    }
    if (!isGood) {
      this->trianglesUsed[i] = false;
      continue;
    }
    double val0 = this->GetValueAtPoint(currentTriangle[0]);
    double val1 = this->GetValueAtPoint(currentTriangle[1]);
    double val2 = this->GetValueAtPoint(currentTriangle[2]);
    double prod01 = val0 * val1;
    double prod12 = val1 * val2;
    double prod20 = val2 * val0;
    if (prod01 > 0 && prod12 > 0 && prod20 > 0) {
      this->trianglesUsed[i] = false;
      continue;
    }
    i = this->CleanUpTrianglesReturnUpdatedCurrentIndex(i);
    if (this->GetTriangleMaxSideLength(i) > minSide && !this->flagTriangleLimitReached) {
      this->subdivide(i);
      continue;
    }
    theSegment.setSize(0);
    if (prod01 <= 0) {
      this->AddPointFromVerticesValues(theSegment, currentTriangle[0], currentTriangle[1], val0, val1);
    }
    if (prod12 <= 0) {
      this->AddPointFromVerticesValues(theSegment, currentTriangle[1], currentTriangle[2], val1, val2);
    }
    if (prod20 <= 0 && theSegment.size < 2) {
      this->AddPointFromVerticesValues(theSegment, currentTriangle[2], currentTriangle[0], val2, val0);
    }
    if (theSegment.size != 2) {
      continue;
    }
    this->theCurve.thePlots.addOnTop(currentPlot);
  }
}

void MeshTriangles::ComputeImplicitPlot() {
  MacroRegisterFunctionWithName("MeshTriangles::ComputeImplicitPlot");
  if (this->XstartingGridCount == 0 || this->YstartingGridCount == 0) {
    return;
  }
  this->Width = this->upperRightCorner[0] - this->lowerLeftCorner[0];
  this->Height = this->upperRightCorner[1] - this->lowerLeftCorner[1];
  double DeltaX = (this->Width) / this->XstartingGridCount;
  double DeltaY = (this->Height) / this->YstartingGridCount;
  Vector<double> currentPoint;
  currentPoint.initializeFillInObject(2, 0);
  List<Vector<double> > currentTriangle;
  currentTriangle.initializeFillInObject(3, currentPoint);
  for (int i = 0; i < this->XstartingGridCount; i ++) {
    for (int j = 0; j < this->YstartingGridCount; j ++) {
      currentTriangle[0][0] = this->lowerLeftCorner[0] + DeltaX * i;
      currentTriangle[0][1] = this->lowerLeftCorner[1] + DeltaY * j;
      currentTriangle[1] = currentTriangle[0];
      currentTriangle[1][0] += DeltaX;
      currentTriangle[2] = currentTriangle[1];
      currentTriangle[2][1] += DeltaY;
      this->theTriangles.addOnTop(currentTriangle);
      currentTriangle[1] = currentTriangle[0];
      currentTriangle[1][1] += DeltaY;
      this->theTriangles.addOnTop(currentTriangle);
    }
  }
  if (this->flagShowGrid) {
    this->plotGrid(static_cast<int>(HtmlRoutines::RedGreenBlue(240, 240, 0)));
    this->thePlot.thePlots.addListOnTop(this->theGrid.thePlots);
  }
  this->ComputeImplicitPlotPart2();
  if (this->flagShowGrid) {
    this->plotGrid(static_cast<int>(HtmlRoutines::RedGreenBlue(100, 100, 100)));
    this->thePlot.thePlots.addListOnTop(this->theGrid.thePlots);
  }
  // this->theCurve.colorRGB=HtmlRoutines::RedGreenBlue(255,0,0);
  this->thePlot.thePlots.addListOnTop(this->theCurve.thePlots);
}

bool Calculator::getMatrixDoubles(const Expression& input, Matrix<double>& output, int DesiredNumcols) {
  return this->functionGetMatrix<double>(
    input,
    output,
    nullptr,
    DesiredNumcols,
    CalculatorFunctions::functionEvaluateToDouble
  );
}

bool Calculator::getVectorDoubles(
  const Expression& input,
  Vector<double>& output,
  int DesiredDimensionNonMandatory
) {
  return this->getVector(
    input,
    output,
    nullptr,
    DesiredDimensionNonMandatory,
    CalculatorFunctions::functionEvaluateToDouble
  );
}

bool Calculator::getVectorDoublesFromFunctionArguments(
  const Expression& input, Vector<double>& output, int DesiredDimensionNonMandatory
) {
  return this->getVectorFromFunctionArguments(
    input,
    output,
    nullptr,
    DesiredDimensionNonMandatory,
    CalculatorFunctions::functionEvaluateToDouble
  );
}

bool CalculatorFunctions::innerGetPointsImplicitly(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerGetPointsImplicitly");
  MeshTriangles theMesh;
  if (!theMesh.ComputePoints(theCommands, input, false)) {
    return false;
  }
  HashedList<Vector<double>, MathRoutines::hashVectorDoubles> thePoints;
  for (int i = 0; i < theMesh.theCurve.thePlots.size; i ++) {
    thePoints.addOnTopNoRepetition(theMesh.theCurve.thePlots[i].thePointsDouble[0]);
    thePoints.addOnTopNoRepetition(theMesh.theCurve.thePlots[i].thePointsDouble[1]);
  }
  Matrix<double> theMatrix;
  theMatrix.assignVectorsToRows(thePoints);
  return output.assignMatrix(theMatrix, theCommands);
}

bool CalculatorFunctions::innerPlotImplicitFunction(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  return CalculatorFunctions::innerPlotImplicitFunctionFull(theCommands, input, output, false);
}

bool CalculatorFunctions::innerPlotImplicitShowGridFunction(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  return CalculatorFunctions::innerPlotImplicitFunctionFull(theCommands, input, output, true);
}

bool MeshTriangles::ComputePoints(
  Calculator& theCommands, const Expression& input, bool showGrid
) {
  MacroRegisterFunctionWithName("MeshTriangles::ComputePoints");
  if (input.size() < 5) {
    return false;
  }
  this->thePlot.dimension = 2;
  this->flagShowGrid = showGrid;
  this->owner = &theCommands;
  this->theFun = input[1];
  this->knownEs = theCommands.knownDoubleConstants;
  this->knownValues = theCommands.knownDoubleConstantValues;
  this->knownValues.addOnTop(0);
  this->knownValues.addOnTop(0);
  HashedList<Expression> theFreeVars;
  if (!this->theFun.getFreeVariables(theFreeVars, true)) {
    return theCommands << "Failed to extract free variables from: " << this->theFun.toString();
  }
  theFreeVars.quickSortAscending();
  if (theFreeVars.size > 2) {
    return theCommands << "I got that your curve depends on " << theFreeVars.size << " expressions, namely: "
    << theFreeVars.toStringCommaDelimited()
    << " and that is too many (2 max). ";
  }
  Expression tempE;
  if (theFreeVars.size == 0) {
    tempE.makeAtom("x", theCommands);
    theFreeVars.addOnTop(tempE);
  }
  if (theFreeVars.size == 1) {
    tempE.makeAtom("y", theCommands);
    if (theFreeVars[0] == tempE) {
      tempE.makeAtom("x", theCommands);
    }
    theFreeVars.addOnTop(tempE);
  }
  this->knownEs.addOnTopNoRepetitionMustBeNew(theFreeVars[0]);
  this->knownEs.addOnTopNoRepetitionMustBeNew(theFreeVars[1]);
  if (!theCommands.getVectorDoubles(input[2], this->lowerLeftCorner)) {
    return theCommands << "Failed to extract lower left corner from: " << input[2].toString();
  }
  if (!theCommands.getVectorDoubles(input[3], this->upperRightCorner)) {
    return theCommands << "Failed to extract upper right corner from: " << input[3].toString();
  }
  List<int> theGridCount;
  if (!theCommands.getVectorInt(input[4], theGridCount)) {
    return theCommands << "Failed to extract pair of small integers from: " << input[4].toString();
  }
  if (theGridCount.size != 2) {
    return theCommands << "Failed to extract pair of small integers from: " << input[4].toString();
  }
  this->XstartingGridCount = theGridCount[0];
  this->YstartingGridCount = theGridCount[1];
  if (input.size() >= 6) {
    if (!input[5].isSmallInteger(&this->maxNumTriangles)) {
      return theCommands << "Failed to extract small integer from: " << input[5].toString();
    }
    if (this->maxNumTriangles > 20000) {
      this->maxNumTriangles = 20000;
      theCommands << "Max number of triangles decreased from your input: " << input[5].toString()
      << " to 20000. If you'd like to lift the restriction, modify code around: file: " << __FILE__
      << " line: " << __LINE__ << ". ";
    }
  }
  this->ComputeImplicitPlot();
  theCommands << "Evaluated function in: "
  << this->numGoodEvaluations << " points and failed to evaluate it at: "
  << this->numBadEvaluations << " points. ";
  return true;
}

bool CalculatorFunctions::innerPlotImplicitFunctionFull(
  Calculator& theCommands, const Expression& input, Expression& output, bool showGrid
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPlotImplicitFunctionFull");
  MeshTriangles theMesh;
  if (!theMesh.ComputePoints(theCommands, input, showGrid)) {
    return false;
  }
  return output.assignValue(theMesh.thePlot, theCommands);
}

bool CalculatorConversions::functionMatrixDouble(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::functionMatrixDouble");
  Matrix<double> theMat;
  if (!theCommands.functionGetMatrix(
    input,
    theMat,
    nullptr,
    0,
    CalculatorFunctions::functionEvaluateToDouble
  )) {
    return theCommands << "<br>Failed to get matrix of doubles. ";
  }
  return output.assignMatrix(theMat, theCommands);
}

bool CalculatorFunctions::innerIntegratePullConstant(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIntegratePullConstant");
  Expression theFunctionE, theVariableE, theSetE;
  if (!input.isIndefiniteIntegralFdx(&theVariableE, &theFunctionE, &theSetE)) {
    return false;
  }
  Expression theFunCoeff, theFunNoCoeff;
  theFunctionE.getCoefficientMultiplicandForm(theFunCoeff, theFunNoCoeff);
  if (theFunCoeff.isEqualToOne()) {
    return false;
  }
  Expression theNewIntegralE;
  theNewIntegralE.makeIntegral(theCommands, theSetE, theFunNoCoeff, theVariableE);
  output = theFunCoeff * theNewIntegralE;
  return true;
}

bool CalculatorFunctions::innerIntegrateSqrtOneMinusXsquared(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIntegrateSqrtOneMinusXsquared");
  Expression theFunctionE, theVariableE, theSetE;
  if (!input.isIndefiniteIntegralFdx(&theVariableE, &theFunctionE, &theSetE)) {
    return false;
  }
  Expression theFunCoeff, theFunNoCoeff;
  theFunctionE.getCoefficientMultiplicandForm(theFunCoeff, theFunNoCoeff);
  if (!theFunNoCoeff.startsWith(theCommands.opThePower(), 3)) {
    return false;
  }
  if (!theFunNoCoeff[2].isEqualToHalf()) {
    return false;
  }
  Expression a, b, c;
  if (!CalculatorFunctions::extractQuadraticCoeffsWRTvariable(theFunNoCoeff[1], theVariableE, a, b, c)) {
    return false;
  }
  if (!b.isEqualToZero()) {
    return false;
  }
  if (!a.isNegativeConstant()) {
    return false;
  }
  if (!c.isPositiveNumber()) {
    return false;
  }
  Expression theSQRTedCoeff, theRadSquared, theRad;
  theSQRTedCoeff.makeSqrt(theCommands, a * (- 1));
  theFunCoeff *= theSQRTedCoeff;
  theRadSquared = c / a * (- 1);
  theRad.makeSqrt(theCommands, theRadSquared);
  Expression rescaledArgument, arcCosPart, algSQRTPart, algPart;
  rescaledArgument = theVariableE / theRad;
  arcCosPart.makeOX(theCommands, theCommands.opArcCos(), rescaledArgument);
  arcCosPart *= theRadSquared / - 2;
  algSQRTPart = theRadSquared - theVariableE * theVariableE;
  algPart.makeSqrt(theCommands, algSQRTPart);
  algPart *= theVariableE / 2;
  output = theFunCoeff * (arcCosPart + algPart);
  return true;
}

bool CalculatorFunctions::innerIntegrateXpowerNePowerAx(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIntegrateXpowerNePowerAx");
  Expression theFunctionE, theVariableE, theSetE;
  if (!input.isIndefiniteIntegralFdx(&theVariableE, &theFunctionE, &theSetE)) {
    return false;
  }
  if (!theFunctionE.startsWith(theCommands.opTimes(), 3)) {
    return false;
  }
  Expression exponentPartE = theFunctionE[1]; //<- note: the seemingly odd order is intentional!
  Expression polyPartE = theFunctionE[2]; //<- note: the seemingly odd order is intentional!
  Expression powerOfXE, powerOfEE;
  Expression aE, bE; //exponent is of form aX+b
  powerOfXE.assignValue(1, theCommands);
  bool isGood = false;
  for (int i = 0; i < 2; i ++) {
    MathRoutines::swap(exponentPartE, polyPartE);
    if (!exponentPartE.startsWith(theCommands.opThePower(), 3)) {
      continue;
    }
    if (!exponentPartE[1].isOperationGiven(theCommands.opE())) {
      continue;
    }
    powerOfEE = exponentPartE[2];
    if (!CalculatorFunctions::extractLinearCoeffsWRTvariable(powerOfEE, theVariableE, aE, bE)) {
      continue;
    }
    if (!aE.isConstantNumber() || !bE.isConstantNumber()) {
      continue;
    }
    if (polyPartE != theVariableE) {
      if (!polyPartE.startsWith(theCommands.opThePower(), 3)) {
        continue;
      }
      if (polyPartE[1] != theVariableE) {
        continue;
      }
      int theInt = - 1;
      if (!polyPartE[2].isSmallInteger(&theInt)) {
        continue;
      }
      if (theInt <= 0) {
        continue;
      }
      powerOfXE = polyPartE[2];
    }
    isGood = true;
    break;
  }
  if (!isGood) {
    return false;
  }
  Expression remainingIntegrand, integralPart;
  remainingIntegrand.MakeXOX(
    theCommands, theCommands.opThePower(), theVariableE, powerOfXE - theCommands.expressionOne()
  );
  remainingIntegrand *= exponentPartE;
  integralPart.makeIntegral(theCommands, theSetE,remainingIntegrand, theVariableE);
  output = (polyPartE * exponentPartE - powerOfXE * integralPart) / aE;
  return true;
}

bool CalculatorFunctions::innerIntegrateSqrtXsquaredMinusOne(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIntegrateSqrtXsquaredMinusOne");
  Expression theFunctionE, theVariableE, theSetE;
  if (!input.isIndefiniteIntegralFdx(&theVariableE, &theFunctionE, &theSetE)) {
    return false;
  }
  Expression theFunCoeff, theFunNoCoeff;
  theFunctionE.getCoefficientMultiplicandForm(theFunCoeff, theFunNoCoeff);
  if (!theFunNoCoeff.startsWith(theCommands.opThePower(), 3)) {
    return false;
  }
  if (!theFunNoCoeff[2].isEqualToHalf()) {
    return false;
  }
  Expression a, b, c;
  if (!CalculatorFunctions::extractQuadraticCoeffsWRTvariable(theFunNoCoeff[1], theVariableE, a, b, c)) {
    return false;
  }
  if (!b.isEqualToZero()) {
    return false;
  }
  if (!a.isPositiveNumber()) {
    return false;
  }
  if (!c.isNegativeConstant()) {
    return false;
  }
  Expression extraCF, theVarChangeCF, theNewVarE;
  extraCF.makeSqrt(theCommands, c * (- 1));
  theFunCoeff *= extraCF;
  theVarChangeCF.makeSqrt(theCommands, (a / c) * (- 1));
  theNewVarE = theVariableE * theVarChangeCF;
  theFunCoeff /= theVarChangeCF;
  Expression algSQRTPart, algPart, lnPart;
  algSQRTPart = theNewVarE * theNewVarE - theCommands.expressionOne();
  algPart.makeSqrt(theCommands, algSQRTPart);
  lnPart.makeOX(theCommands, theCommands.opLog(), theNewVarE - algPart);
  output = theFunCoeff * (algPart * theNewVarE + lnPart) / 2;
  return true;
}

bool CalculatorFunctions::innerIntegrateDefiniteIntegral(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerIntegrateDefiniteIntegral");
  Expression theFunctionE, theVariableE, theSetE;
  if (!input.isDefiniteIntegralOverIntervalFdx(&theVariableE, &theFunctionE, &theSetE)) {
    return false;
  }
  if (!theSetE.startsWith(theCommands.opLimitBoundary(), 3)) {
    return false;
  }
  if (
    theSetE[1].containsAsSubExpressionNoBuiltInTypes(theCommands.opInfinity()) ||
    theSetE[2].containsAsSubExpressionNoBuiltInTypes(theCommands.opInfinity())
  ) {
    return false;
  }
  Expression theIndefiniteIntegral, indefiniteExpression;
  indefiniteExpression.makeAtom(theCommands.opIndefiniteIndicator(), theCommands);
  theIndefiniteIntegral.makeIntegral(theCommands, indefiniteExpression, theFunctionE, theVariableE);
  Expression solvedIntegral;
  if (!theCommands.evaluateExpression(theCommands, theIndefiniteIntegral, solvedIntegral)) {
    return false;
  }
  if (solvedIntegral.containsAsSubExpressionNoBuiltInTypes(theCommands.opIntegral())) {
    return false;
  }
  if (solvedIntegral.containsAsSubExpressionNoBuiltInTypes(theCommands.opDivide())) {
    return false;
  }
  if (solvedIntegral.containsAsSubExpressionNoBuiltInTypes(theCommands.opLog())) {
    return false;
  }
  HashedList<Expression> theVar;
  List<double> theValue;
  theVar.addOnTop(theVariableE);
  theValue.addOnTop(0);
  for (int i = 1; i <= 2; i ++) {
    if (theSetE[i].evaluatesToDouble(&theValue[0])) {
      double theResult = 0;
      if (!solvedIntegral.evaluatesToDoubleUnderSubstitutions(theVar, theValue, &theResult)) {
        return false;
      }
    }
  }
  Expression theSubTop(theCommands), theSubBottom(theCommands);
  theSubTop.addChildAtomOnTop(theCommands.opDefine());
  theSubTop.addChildOnTop(theVariableE);
  theSubBottom = theSubTop;
  theSubBottom.addChildOnTop(theSetE[1]);
  theSubTop.addChildOnTop(theSetE[2]);
  Expression theTopCommands(theCommands), theBottomCommands(theCommands);
  theTopCommands.addChildAtomOnTop(theCommands.opEndStatement());
  theBottomCommands.addChildAtomOnTop(theCommands.opEndStatement());

  theTopCommands.addChildOnTop(theSubTop);
  theBottomCommands.addChildOnTop(theSubBottom);

  theTopCommands.addChildOnTop(solvedIntegral);
  theBottomCommands.addChildOnTop(solvedIntegral);
  Expression theTop, theBottom;
  theTop.MakeXOX(theCommands, theCommands.opUnderscore(), theTopCommands, theCommands.expressionTwo());
  theBottom.MakeXOX(theCommands, theCommands.opUnderscore(), theBottomCommands, theCommands.expressionTwo());
  output = theTop - theBottom;
  return true;
}

bool CalculatorFunctions::innerApplyToSubexpressionsRecurseThroughCalculusFunctions(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("innerApplyToSubexpressionsRecurseThroughCalculusFunctions");
  if (input.size() != 3) {
    return false;
  }
  if (!input.startsWithGivenOperation("ApplyToSubexpressionsRecurseThroughCalculusFunctions")) {
    return false;
  }
  const Expression& theArg = input[2];
  if (
    theArg.startsWith(theCommands.opPlus()) ||
    theArg.startsWith(theCommands.opMinus()) ||
    theArg.startsWith(theCommands.opTimes()) ||
    theArg.startsWith(theCommands.opThePower()) ||
    theArg.startsWith(theCommands.opDivide()) ||
    theArg.startsWith(theCommands.opSequence()) ||
    theArg.startsWith(theCommands.opSqrt()) ||
    theArg.startsWith(theCommands.opCos()) ||
    theArg.startsWith(theCommands.opSin())
  ) {
    output.reset(theCommands);
    output.addChildOnTop(input[1]);
    Expression theRecursivelyModifiedE(theCommands), nextE(theCommands);
    theRecursivelyModifiedE.addChildOnTop(theArg[0]);
    nextE.addChildAtomOnTop("ApplyToSubexpressionsRecurseThroughCalculusFunctions");
    nextE.addChildOnTop(input[1]);
    for (int i = 1; i < theArg.size(); i ++) {
      nextE.children.setSize(2);
      nextE.addChildOnTop(theArg[i]);
      theRecursivelyModifiedE.addChildOnTop(nextE);
    }
    return output.addChildOnTop(theRecursivelyModifiedE);
  }
  output.reset(theCommands);
  output.addChildOnTop(input[1]);
  output.addChildOnTop(theArg);
  return true;
}

bool CalculatorFunctions::innerNumerator(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerNumerator");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Rational theRat;
  if (argument.isRational(&theRat)) {
    return output.assignValue(Rational(theRat.getNumerator()), theCommands);
  }
  if (argument.startsWith(theCommands.opDivide())) {
    if (argument.size() > 1) {
      output = argument[1];
      return true;
    }
  }
  output = argument;
  return true;
}

bool CalculatorFunctions::innerDenominator(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDenominator");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Rational theRat, theDen;
  if (argument.isRational(&theRat)) {
    theDen = theRat.getDenominator();
    return output.assignValue(theDen, theCommands);
  }
  if (argument.startsWith(theCommands.opDivide())) {
    if (argument.size() > 2) {
      output = argument[2];
      return true;
    }
  }
  return output.assignValue(1, theCommands);
}

bool CalculatorFunctions::innerHandleUnderscorePowerLimits(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerHandleUnderscorePowerLimits");
  if (
    !input.startsWith(theCommands.opThePower(), 3) &&
    !input.startsWith(theCommands.opUnderscore(), 3)
  ) {
    return false;
  }
  if (!input[1].startsWith(theCommands.opLimitBoundary())) {
    return false;
  }
  output = input[1];
  for (int i = output.size(); i < 3; i ++) {
    output.addChildAtomOnTop(theCommands.opIndefiniteIndicator());
  }
  if (input.startsWith(theCommands.opThePower())) {
    return output.setChild(2, input[2]);
  } else {
    return output.setChild(1, input[2]);
  }
}

bool CalculatorFunctions::innerSumAsOperatorToSumInternalNotation(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSumAsOperatorToSumInternalNotation");
  if (input.size() <= 1) {
    return false;
  }
  if (!input[0].startsWith(theCommands.opSum())) {
    return false;
  }
  output = input[0];
  if (input.size() == 2) {
    return output.addChildOnTop(input[1]);
  }
  List<Expression> theRemaining;
  for (int i = 1; i < input.size(); i ++) {
    theRemaining.addOnTop(input[i]);
  }
  Expression argumentE;
  argumentE.makeSequence(theCommands, &theRemaining);
  return output.addChildOnTop(argumentE);
}

bool CalculatorFunctions::innerSumTimesExpressionToSumOf(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSumTimesExpressionToSumOf");
  if (
    !input.startsWith(theCommands.opTimes(), 3) &&
    !input.startsWith(theCommands.opDivide(), 3)
  ) {
    return false;
  }
  if (
    input[1].startsWith(theCommands.opSum(), 2) &&
    !input.startsWith(theCommands.opDivide())
  ) {
    if (!input[1][1].startsWith(theCommands.opLimitBoundary())) {
      return false;
    }
    output = input[1];
    return output.addChildOnTop(input[2]);
  }
  if (input[1].startsWith(theCommands.opSum(), 3)) {
    if (!input[1][1].startsWith(theCommands.opLimitBoundary())) {
      return false;
    }
    Expression theSummed(theCommands);
    theSummed.addChildOnTop(input[0]);
    theSummed.addChildOnTop(input[1][2]);
    theSummed.addChildOnTop(input[2]);
    output = input[1];
    return output.setChild(2,theSummed);
  }
  return false;
}

bool CalculatorFunctions::innerSumSequence(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSumSequence");
  if (input.size() < 1) {
    return false;
  }
  if (input.size() == 1) {
    return output.assignValue(0, theCommands);
  }
  if (input[1].startsWith(theCommands.opLimitBoundary())) {
    return false;
  }
  const Expression* sequenceToSum = &input;
  if (input[1].isSequenceNElements()) {
    sequenceToSum = &input[1];
  }
  List<Expression> theTerms;
  for (int i = 1; i < sequenceToSum->size(); i ++) {
    theTerms.addOnTop((*sequenceToSum)[i]);
  }
  return output.makeSum(theCommands, theTerms);
}

bool CalculatorFunctions::innerMultiplySequence(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerMultiplySequence");
  if (input.size() < 1) {
    return false;
  }
  if (input.size() == 1) {
    return output.assignValue(1, theCommands);
  }
  List<Expression> theTerms;
  for (int i = 1; i < input.size(); i ++) {
    theTerms.addOnTop(input[i]);
  }
  return output.makeProduct(theCommands, theTerms);
}

bool CalculatorFunctions::innerEnsureExpressionDependsOnlyOnStandard(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerEnsureExpressionDependsOnlyOnStandard");
  if (input.size() < 3) {
    return false;
  }
  const Expression& theExpression = input[1];
  HashedList<Expression> allowedFreeVars, presentFreeVars;
  allowedFreeVars.setExpectedSize(input.size() - 2);
  presentFreeVars.setExpectedSize(input.size() - 2);
  for (int i = 2; i < input.size(); i ++) {
    allowedFreeVars.addOnTopNoRepetition(input[i]);
  }
  std::stringstream out;
  theExpression.getFreeVariables(presentFreeVars, true);
  if (!allowedFreeVars.contains(presentFreeVars)) {
    out << "<hr>";
    out << "Your expression:<br>\\(" << input[1].toString() << "\\)"
    << "<br><b style ='color:red'>contains the unexpected variable(s):</b><br><b>";
    bool found = false;
    for (int i = 0; i < presentFreeVars.size; i ++) {
      if (!allowedFreeVars.contains(presentFreeVars[i])) {
        if (found) {
          out << ", ";
        }
        found = true;
        out << presentFreeVars[i].toString();
      }
    }
    out << "</b>.";
    out << "<br>The expected variables are: " << allowedFreeVars.toStringCommaDelimited() << ". ";
    out << "<br>Beware of typos such as:<br>[wrong:] <span style ='color:red'>lnx, sqrt2</span>  "
    << "<br>[correct:] <span style ='color:green'>ln(x)</span> or <span style ='color:green'>ln x</span>, "
    << "<span style ='color:green'>sqrt(2)</span> or <span style ='color:green'>sqrt 2</span>.<hr>";
    return output.assignValue(out.str(), theCommands);
  }
  return output.assignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerRemoveDuplicates(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerRemoveDuplicates");
  int operationRemoveDuplicated = theCommands.operations.getIndexNoFail("RemoveDuplicates");
  if (
    !input.isListStartingWithAtom(operationRemoveDuplicated) &&
    !input.isSequenceNElements()
  ) {
    return false;
  }
  HashedList<Expression> result;
  for (int i = 1; i < input.size(); i ++) {
    result.addOnTopNoRepetition(input[i]);
  }
  return output.makeSequence(theCommands, &result);
}

bool CalculatorFunctions::innerSort(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSort");
  if (!input.isListStartingWithAtom(
    theCommands.operations.getIndexNoFail("Sort")
  )) {
    return false;
  }
  List<Expression> sortedExpressions;
  const Expression* toBeSorted = &input;
  if (input.size() == 2) {
    if (input[1].isSequenceNElements()) {
      toBeSorted = &(input[1]);
    }
  }
  sortedExpressions.reserve(toBeSorted->size() - 1);
  for (int i = 1; i < toBeSorted->size(); i ++) {
    sortedExpressions.addOnTop((*toBeSorted)[i]);
  }
  sortedExpressions.quickSortAscending();
  return output.makeSequence(theCommands, &sortedExpressions);
}

bool CalculatorFunctions::innerSortDescending(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSortDescending");
  if (
    !input.isListStartingWithAtom(theCommands.operations.getIndexNoFail("SortDescending")) &&
    !input.isSequenceNElements()
  ) {
    return false;
  }
  List<Expression> sortedExpressions;
  const Expression* toBeSorted = &input;
  if (input.size() == 2) {
    if (input[1].isSequenceNElements()) {
      toBeSorted = &(input[1]);
    }
  }
  sortedExpressions.reserve(toBeSorted->size() - 1);
  for (int i = 1; i < toBeSorted->size(); i ++) {
    sortedExpressions.addOnTop((*toBeSorted)[i]);
  }
  sortedExpressions.quickSortDescending();
  return output.makeSequence(theCommands, &sortedExpressions);
}

bool CalculatorFunctions::innerLength(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerLength");
  if (!input.isListStartingWithAtom(
    theCommands.operations.getIndexNoFail("Length")
  )) {
    return false;
  }
  if (input.size() == 2) {
    if (input[1].isSequenceNElements()) {
      return output.assignValue(input[1].size() - 1, theCommands);
    }
  }
  return output.assignValue(input.size() - 1, theCommands);
}

bool CalculatorFunctions::innerEnsureExpressionDependsOnlyOnMandatoryVariables(
  Calculator& theCommands, const Expression& input, Expression& output, bool excludeNamedConstants
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerEnsureExpressionDependsOnlyOnMandatoryVariables");
  if (input.size() < 3) {
    return false;
  }
  const Expression& theExpression = input[1];
  HashedList<Expression> mandatoryFreeVars, allowedFreeVars, presentFreeVars;
  if (input[2].isSequenceNElements()) {
    mandatoryFreeVars.setExpectedSize(input[2].size() - 1);
    for (int i = 1; i < input[2].size(); i ++) {
      mandatoryFreeVars.addOnTop(input[2][i]);
    }
  } else {
    mandatoryFreeVars.addOnTop(input[2]);
  }
  allowedFreeVars.addOnTop(mandatoryFreeVars);
  if (input.size() > 3) {
    if (input[3].isSequenceNElements()) {
      for (int i = 1; i < input[3].size(); i ++) {
        allowedFreeVars.addOnTop(input[3][i]);
      }
    } else {
      allowedFreeVars.addOnTop(input[3]);
    }
  }
  presentFreeVars.setExpectedSize(input.size() - 2);
  theExpression.getFreeVariables(presentFreeVars, excludeNamedConstants);
  std::stringstream out;
  if (!presentFreeVars.contains(mandatoryFreeVars)) {
    out << "<hr>";
    out << "Your expression:<br>\\(" << input[1].toString() << "\\)"
    << "<br><b style ='color:red'>is required to contain the variables:</b><br><b>";
    bool found = false;
    for (int i = 0; i < mandatoryFreeVars.size; i ++) {
      if (!presentFreeVars.contains(mandatoryFreeVars[i])) {
        if (found) {
          out << ", ";
        }
        found = true;
        out << "\\(" << mandatoryFreeVars[i].toString() << "\\)";
      }
    }
    out << "</b>.";
    out << "<br>The mandatory variable(s) are: " << mandatoryFreeVars.toStringCommaDelimited() << ". ";
  }
  if (!allowedFreeVars.contains(presentFreeVars)) {
    out << "<hr>";
    out << "Your expression:<br>\\(" << input[1].toString() << "\\)"
    << "<br><b style ='color:red'>contains the unexpected variable(s):</b><br><b>";
    bool found = false;
    for (int i = 0; i < presentFreeVars.size; i ++) {
      if (!allowedFreeVars.contains(presentFreeVars[i])) {
        if (found) {
          out << ", ";
        }
        found = true;
        out << presentFreeVars[i].toString();
      }
    }
    out << "</b>.";
    out << "<br>The expected variables are: " << allowedFreeVars.toStringCommaDelimited() << ". ";
  }
  if (out.str() != "") {
    out << "<br>Beware of typos such as:<br>[wrong:] <span style = 'color:red'>lnx, sqrt2</span>  "
    << "<br>[correct:] <span style = 'color:green'>ln(x)</span> or <span style ='color:green'>ln x</span>, "
    << "<span style ='color:green'>sqrt(2)</span> or <span style ='color:green'>sqrt 2</span>.<hr>";
  }
  return output.assignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerPlotGrid(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPlotGrid");
  (void) input;
  PlotObject thePlot;
  thePlot.thePlotType = "axesGrid";
  thePlot.dimension = 2;
  return output.assignValue(thePlot, theCommands);
}

bool CalculatorFunctions::innerPlotRemoveCoordinateAxes(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPlotRemoveCoordinateAxes");
  (void) input;
  Plot thePlotFinal;
  thePlotFinal.dimension = 2;
  thePlotFinal.flagIncludeCoordinateSystem = false;
  return output.assignValue(thePlotFinal, theCommands);
}

bool CalculatorFunctions::innerPlotLabel(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPlotLabel");
  if (input.size() != 3) {
    return false;
  }
  Vector<double> labelPosition;
  if (!theCommands.getVectorDoubles(input[1], labelPosition, - 1)) {
    return false;
  }
  std::string theLabel;
  if (!input[2].isOfType<std::string>(&theLabel)) {
    theLabel = input[2].toString();
  }
  PlotObject thePlot;
  thePlot.dimension = labelPosition.size;
  thePlot.thePlotString = theLabel;
  thePlot.thePointsDouble.addOnTop(labelPosition);
  thePlot.thePlotType = "label";
  thePlot.colorJS = "black";
  return output.assignValue(thePlot, theCommands);
}

bool CalculatorFunctions::innerPlotRectangle(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPlotRectangle");
  if (input.size() != 3) {
    return false;
  }
  Vectors<double> theRectangle;
  theRectangle.setSize(2);
  if (
    !theCommands.getVectorDoubles(input[1], theRectangle[0], 2) ||
    !theCommands.getVectorDoubles(input[2], theRectangle[1], 2)
  ) {
    return false;
  }
  PlotObject thePlot;
  thePlot.dimension = 2;
  thePlot.thePlotType = "pathFilled";
  Vector<double> currentCorner = theRectangle[0];
  Vector<double>& dimensions = theRectangle[1];

  thePlot.thePointsDouble.addOnTop(currentCorner);
  currentCorner[0] += dimensions[0];
  thePlot.thePointsDouble.addOnTop(currentCorner);
  currentCorner[1] += dimensions[1];
  thePlot.thePointsDouble.addOnTop(currentCorner);
  currentCorner[0] -= dimensions[0];
  thePlot.thePointsDouble.addOnTop(currentCorner);
  currentCorner[1] -= dimensions[1];
  thePlot.thePointsDouble.addOnTop(currentCorner);
  thePlot.colorFillJS = "cyan";
  thePlot.colorJS = "blue";
  thePlot.thePointsDouble.addOnTop(currentCorner);
  thePlot.theRectangles.addOnTop(theRectangle);
  thePlot.colorRGB = static_cast<int>(HtmlRoutines::RedGreenBlue(0, 0, 255));
  thePlot.colorFillRGB = static_cast<int>(HtmlRoutines::RedGreenBlue(0, 255, 255));
  return output.assignValue(thePlot, theCommands);
}

bool CalculatorFunctions::innerSolveUnivariatePolynomialWithRadicalsWRT(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSolveUnivariatePolynomialWithRadicalsWRT");
  if (input.size() != 3) {
    return theCommands << "SolveFor takes as input three arguments. ";
  }
  if (input[1].hasBoundVariables()) {
    return false;
  }
  if (input[2].hasBoundVariables()) {
    return false;
  }
  Expression thePowers;
  Expression modifiedInput = input;
  if (!modifiedInput[2].startsWith(theCommands.opDefine())) {
    if (!CalculatorFunctions::innerCoefficientsPowersOf(theCommands, modifiedInput, thePowers)) {
      return theCommands << "Failed to extract the coefficients of " << modifiedInput[1].toString()
      << " in " << modifiedInput[2].toString();
    }
  } else {
    Expression convertedEqualityE, convertedSimplifiedEqualityE;
    if (!CalculatorFunctions::functionEqualityToArithmeticExpression(theCommands, modifiedInput[2], convertedEqualityE)) {
      return theCommands << "Failed to interpret the equality " << modifiedInput[2].toString();
    }
    if (!Calculator::evaluateExpression(theCommands, convertedEqualityE, convertedSimplifiedEqualityE)) {
      return theCommands << "Failed to simplify: " << convertedEqualityE.toString();
    }
    modifiedInput.setChild(2, convertedSimplifiedEqualityE);
    if (!CalculatorFunctions::innerCoefficientsPowersOf(theCommands, modifiedInput, thePowers)) {
      return theCommands << "Failed to extract the coefficients of " << modifiedInput[1].toString()
      << " in " << modifiedInput[2].toString() << " which was obtained from the equality "
      << input[2].toString();
    }
  }
  if (!thePowers.isSequenceNElements()) {
    return theCommands << "This is not supposed to happen: expression "
    << thePowers.toString() << " should be a list. This may be a programming bug. ";
  }
  if (thePowers.size() == 2) {
    return theCommands << "Cannot solve: " << modifiedInput[2].toString()
    << ". The expression does not depend on " << modifiedInput[1].toString()
    << ". The coefficients of "
    << modifiedInput[1].toString() << " are: " << thePowers.toString();
  }
  if (thePowers.size() == 3) {
    output = thePowers[1];
    output *= - 1;
    output /= thePowers[2];
    return true;
  }
  if (thePowers.children.size == 4) {
    const Expression& a = thePowers[3];
    const Expression& b = thePowers[2];
    const Expression& c = thePowers[1];
    output.makeSequence(theCommands);
    Expression currentRoot;
    Expression theDiscriminant;
    theDiscriminant = b * b - a * c * 4;
    Expression sqrtDiscriminant;
    sqrtDiscriminant.makeSqrt(theCommands, theDiscriminant, 2);
    currentRoot = (b * (- 1) - sqrtDiscriminant) / (a * 2);
    output.addChildOnTop(currentRoot);
    currentRoot = (b * (- 1) + sqrtDiscriminant) / (a * 2);
    output.addChildOnTop(currentRoot);
    return true;
  }
  return false;
}

bool CalculatorFunctions::innerOperatorBounds(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPowerIntToAny");
  if (
    !input.startsWith(theCommands.opUnderscore(), 3) &&
    !input.startsWith(theCommands.opThePower(), 3)
  ) {
    return false;
  }
  const Expression& baseE = input[1];
  Expression theLimitsE;
  int theIntegralOp = baseE.theData;
  if (
    theIntegralOp == theCommands.opIntegral() || theIntegralOp == theCommands.opSum()
  ) {
    output.reset(theCommands);
    output.addChildAtomOnTop(theIntegralOp);
    theLimitsE.reset(theCommands);
    theLimitsE.addChildAtomOnTop(theCommands.opLimitBoundary());
    theLimitsE.addChildOnTop(input[2]);
    theLimitsE.addChildAtomOnTop(theCommands.opIndefiniteIndicator());
    output.addChildOnTop(theLimitsE);
    return true;
  }
  if (
    !baseE.startsWith(theCommands.opIntegral(), 2) &&
    !baseE.startsWith(theCommands.opSum(), 2)
  ) {
    return false;
  }
  theLimitsE.reset(theCommands);
  theLimitsE.addChildAtomOnTop(theCommands.opLimitBoundary());
  for (int i = 1; i < 3; i ++) {
    if (i < baseE[1].size()) {
      theLimitsE.addChildOnTop(baseE[1][i]);
    } else {
      theLimitsE.addChildAtomOnTop(theCommands.opIndefiniteIndicator());
    }
  }
  if (input[1].isOperationGiven(theCommands.opUnderscore())) {
    theLimitsE.setChild(1, input[2]);
  } else {
    theLimitsE.setChild(2, input[2]);
  }
  output = input[1];
  return output.setChild(1, theLimitsE);
}

bool CalculatorFunctions::innerPowerExponentToLog(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPowerExponentToLog");
  if (!input.startsWith(theCommands.opThePower(), 3)) {
    return false;
  }
  const Expression& baseE = input[1];
  const Expression& powerE = input[2];
  if (baseE.isOperationGiven(theCommands.opE())) {
    if (powerE.startsWith(theCommands.opLog(), 2)) {
      output = powerE[1];
      return true;
    }
  }
  return false;
}

bool CalculatorFunctions::innerDistributeExponent(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerDistributeExponent");
  if (!input.startsWith(theCommands.opThePower(), 3)) {
    return false;
  }
  const Expression& base = input[1];
  const Expression& exponentE = input[2];
  if (exponentE.isOperationGiven(theCommands.opCirc())) {
    return false;
  }
  if (!input[1].startsWith(theCommands.opTimes(), 3)) {
    return false;
  }
  if (!base[1].isConstantNumber()) {
    return false;
  }
  bool isGood =base[1].isPositiveNumber() || base[2].isPositiveNumber();
  if (!isGood) {
    if (exponentE.isInteger()) {
      isGood = true;
    } else {
      Rational exponentRat;
      if (exponentE.isRational(&exponentRat)) {
        if (!exponentRat.getDenominator().IsEven()) {
          isGood = true;
        }
      }
    }
  }
  if (!isGood) {
    return false;
  }
  Expression leftE, rightE;
  leftE.MakeXOX(theCommands, theCommands.opThePower(), input[1][1], input[2]);
  rightE.MakeXOX(theCommands, theCommands.opThePower(), input[1][2], input[2]);
  return output.MakeXOX(theCommands, theCommands.opTimes(), leftE, rightE);
}

bool CalculatorFunctions::innerSqrt(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerSqrt");
  if (input.size() != 3) {
    return false;
  }
  Rational ratPower;
  if (input[1].isRational(&ratPower)) {
    if (ratPower != 0) {
      Expression powerE, powerEreduced, theExponentE;
      ratPower.invert();
      theExponentE.assignValue(ratPower, theCommands);
      powerE.MakeXOX(theCommands,theCommands.opThePower(), input[2], theExponentE);
      if (CalculatorFunctionsBinaryOps::innerPowerRationalByRationalReducePrimeFactors(
        theCommands, powerE, powerEreduced
      )) {
        if (powerEreduced != powerE && powerEreduced != input) {
          output = powerEreduced;
          return true;
        }
      }
    }
  }
  if (input[2].isEqualToOne()) {
    return output.assignValue(1, theCommands);
  }
  int thePower = 0;
  if (!input[1].isSmallInteger(&thePower)) {
    return false;
  }
  if (!input[2].isRational() ) {
    theCommands.checkInputNotSameAsOutput(input, output);
    Expression theExponent;
    Rational thePowerRat(1, thePower);
    theExponent.assignValue(thePowerRat, theCommands);
    return output.MakeXOX(theCommands, theCommands.opThePower(), input[2], theExponent);
  }
  if (thePower > 0 && input[2].isEqualToZero()) {
    return output.assignValue(0, theCommands);
  }
  if (thePower == 0 && input[2].isEqualToZero()) {
    return output.assignValue(1, theCommands);
  }
  Rational rationalValue;
  if (!input[2].isRational(&rationalValue)) {
    return false;
  }
  if (thePower < 0) {
    if (rationalValue.isEqualToZero()) {
      return output.makeError("Division by zero in expression: " + input.toString(), theCommands);
    }
    thePower *= - 1;
    rationalValue.invert();
  }
  if (thePower != 2) {
    return false;
  }
  AlgebraicNumber theNumber;
  if (!theNumber.assignRationalQuadraticRadical(
    rationalValue,
    theCommands.theObjectContainer.theAlgebraicClosure,
    &theCommands.comments
  )) {
    return false;
  }
  return output.assignValue(theNumber, theCommands);
}

bool CalculatorFunctions::innerFloor(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerFloor");
  if (input.size() != 2) {
    return false;
  }
  Rational theRat;
  if (input[1].isOfType<Rational>(&theRat)) {
    theRat.assignFloor();
    return output.assignValue(theRat, theCommands);
  }
  double theDouble = 0;
  if (input[1].evaluatesToDouble(&theDouble)) {
    return output.assignValue(static_cast<int>(std::floor(theDouble)), theCommands);
  }
  return false;
}

bool CalculatorFunctions::innerLogarithmBaseNCeiling(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerLogarithmBaseNCeiling");
  if (input.size() != 3) {
    return theCommands << "Logarithm ceiling function expects exactly two arguments. ";
  }
  int smallInt = 0;
  if (!input[1].isSmallInteger(&smallInt)) {
    return theCommands << "First argument expected to be a small integer. ";
  }
  if (smallInt <= 1) {
    return theCommands << "First argument must be larger than one. ";
  }
  LargeIntegerUnsigned argument;
  if (!input[2].isIntegerNonNegative(&argument)) {
    return theCommands << "Failed to extract positive intger from second argument. ";
  }
  int result = static_cast<int>(
    argument.logarithmBaseNCeiling(static_cast<unsigned>(smallInt))
  );
  return output.assignValue(result, theCommands);
}

bool CalculatorFunctions::innerRound(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerRound");
  if (input.size() != 2) {
    return false;
  }
  Rational theRat;
  if (input[1].isOfType<Rational>(&theRat)) {
    Rational result = theRat;
    result.assignFloor();
    if (theRat - result >= Rational(1, 2)) {
      result ++;
    }
    return output.assignValue(result, theCommands);
  }
  double theDouble = 0;
  if (input[1].evaluatesToDouble(&theDouble)) {
    return output.assignValue(
      static_cast<int>(std::round(theDouble)), theCommands
    );
  }
  return false;
}

bool CalculatorFunctions::innerPlotPath(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPlotPath");
  if (input.size() < 3) {
    return false;
  }
  const Expression& theMatE = input[1];
  Matrix<double> theMat;
  if (!theCommands.getMatrixDoubles(theMatE, theMat)) {
    return theCommands << "Failed to extract matrix from: " << theMatE.toString();
  }
  if (theMat.numberOfColumns != 2 && theMat.numberOfColumns != 3) {
    return theCommands << "Only dimensions 2 and 3 are supported at the moment. ";
  }
  PlotObject theSegment;
  if (input.size() >= 3) {
    theSegment.colorJS = "black";
    theSegment.colorRGB = static_cast<int>(HtmlRoutines::RedGreenBlue(0, 0, 0));
    const Expression& colorE = input[2];
    if (!colorE.isOfType<std::string>(&theSegment.colorJS)) {
      theSegment.colorJS = colorE.toString();
    }
    if (!DrawingVariables::GetColorIntFromColorString(
      theSegment.colorJS, theSegment.colorRGB
    )) {
      theCommands << "Unrecognized color: " << theSegment.colorJS;
    }
  }
  if (input.size() >= 4) {
    const Expression& lineWidthE = input[3];
    if (!lineWidthE.evaluatesToDouble(&theSegment.lineWidth)) {
      theCommands << "Failed to extract line width from: " << lineWidthE.toString();
    }
    std::stringstream lineWidthStream;
    lineWidthStream.precision(4);
    lineWidthStream << theSegment.lineWidth;
    theSegment.lineWidthJS = lineWidthStream.str();
  }
  theSegment.thePlotType = "segmentPath";
  theSegment.dimension = theMat.numberOfColumns;
  theMat.getVectorsFromRows(theSegment.thePointsDouble);
  if (input.size() >= 4) {
    if (!input[3].evaluatesToDouble(&theSegment.lineWidth)) {
      theSegment.lineWidth = 1;
    }
  }
  Plot thePlot;
  thePlot += theSegment;
  return output.assignValue(thePlot, theCommands);
}

bool CalculatorFunctions::innerPlotMarkSegment(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPlotMarkSegment");
  if (input.size() < 3) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  Vector<double> leftV, rightV;
  if (!theCommands.getVectorDoubles(leftE, leftV) || !theCommands.getVectorDoubles(rightE, rightV)) {
    return false;
  }
  if (leftV.size != rightV.size) {
    return false;
  }
  if (leftV.size != 2) {
    return false;
  }
  int numSegments = 1;
  if (input.size() >= 4) {
    if (!input[3].isSmallInteger(&numSegments)) {
      return theCommands << "Could not extract small integer from " << input[3].toString();
    }
  }
  if (numSegments < 1 || numSegments > 100) {
    return theCommands << "Bad number of segments: " << numSegments;
  }
  Expression theVector = (rightE - leftE);
  Expression midPt = (rightE + leftE) / 2;
  Expression theVectorX, theVectorY;
  theVectorX.MakeXOX(theCommands, theCommands.opUnderscore(), theVector, theCommands.expressionOne());
  theVectorY.MakeXOX(theCommands, theCommands.opUnderscore(), theVector, theCommands.expressionTwo());
  Expression theOrthoV;
  theOrthoV.MakeXOX(theCommands, theCommands.opSequence(), theVectorY * (- 1), theVectorX);
  Expression leftPt = midPt - theOrthoV / 25;
  Expression rightPt = midPt + theOrthoV / 25;
  output.reset(theCommands);
  output.addChildAtomOnTop("PlotSegment");
  output.addChildOnTop(leftPt);
  output.addChildOnTop(rightPt);
  for (int i = 4; i < input.size(); i ++) {
    output.addChildOnTop(input[i]);
  }
  return true;
}

bool CalculatorFunctions::innerPlotSegment(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPlotSegment");
  if (input.size() < 3) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  Vector<double> leftV, rightV;
  if (!theCommands.getVectorDoubles(leftE, leftV) || !theCommands.getVectorDoubles(rightE, rightV)) {
    return false;
  }
  if (leftV.size != rightV.size) {
    return false;
  }
  if (leftV.size != 2 && leftV.size != 3) {
    return false;
  }
  PlotObject theSegment;
  if (input.size() >= 4) {
    theSegment.colorJS = "black";
    theSegment.colorRGB = static_cast<int>(HtmlRoutines::RedGreenBlue(0, 0, 0));
    const Expression& colorE = input[3];
    if (!colorE.isOfType<std::string>(&theSegment.colorJS)) {
      theSegment.colorJS = colorE.toString();
    }
    if (!DrawingVariables::GetColorIntFromColorString(theSegment.colorJS, theSegment.colorRGB)) {
      theCommands << "Unrecognized color: " << theSegment.colorJS;
    }
  }
  if (input.size() >= 5) {
    const Expression& lineWidthE = input[4];
    if (!lineWidthE.evaluatesToDouble(&theSegment.lineWidth)) {
      theCommands << "Failed to extract line width from: "
      << lineWidthE.toString();
    }
    std::stringstream lineWidthStream;
    lineWidthStream.precision(4);
    lineWidthStream << theSegment.lineWidth;
    theSegment.lineWidthJS = lineWidthStream.str();
  }
  theSegment.thePlotType = "segment";
  if (leftV.size == 3) {
    theSegment.dimension = 3;
  } else {
    theSegment.dimension = 2;
  }
  theSegment.thePointsDouble.addOnTop(leftV);
  theSegment.thePointsDouble.addOnTop(rightV);
  if (input.size() >= 5) {
    if (!input[4].evaluatesToDouble(&theSegment.lineWidth)) {
      theSegment.lineWidth = 1;
    }
  }
  Plot thePlot;
  thePlot += theSegment;
  return output.assignValue(thePlot, theCommands);
}

bool CalculatorFunctions::innerThaw(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerThaw");
  if (input.size() < 2) {
    return false;
  }
  const Expression& argument = input[1];
  if (!argument.startsWith(theCommands.opFreeze())) {
    output = argument;
    return true;
  }
  if (argument.size() > 2) {
    output = argument;
    return output.setChildAtomValue(0, theCommands.opSequence());
  }
  if (argument.size() == 2) {
    output = argument[1];
    return true;
  }
  return false;
}

bool CalculatorFunctions::innerLCM(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerLCM");
  if (input.size() < 3) {
    return false;
  }
  Vector<LargeInteger> theInts;
  if (!theCommands.getVectorLargeIntegerFromFunctionArguments(input, theInts)) {
    return false;
  }
  if (theInts.size < 1) {
    //<-this shouldn't happen if getVectorLargeIntegerFromFunctionArguments works as intended.
    return false;
  }
  LargeIntegerUnsigned theResult = theInts[0].value;
  if (theResult == 0) {
    return false;
  }
  for (int i = 1; i < theInts.size; i ++) {
    if (theInts[i].value == 0) {
      return false;
    }
    theResult = LargeIntegerUnsigned::lcm(theResult, theInts[i].value);
  }
  return output.assignValue(theResult, theCommands);
}

bool CalculatorFunctions::innerGCD(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerGCD");
  if (input.size() < 3) {
    return false;
  }
  Vector<LargeInteger> theInts;
  if (!theCommands.getVectorLargeIntegerFromFunctionArguments(input, theInts)) {
    return false;
  }
  if (theInts.size < 1) {
    //<-this shouldn't happen if getVectorLargeIntegerFromFunctionArguments works as intended.
    return false;
  }
  LargeIntegerUnsigned theResult = theInts[0].value;
  if (theResult == 0) {
    return false;
  }
  for (int i = 1; i < theInts.size; i ++) {
    if (theInts[i].value == 0) {
      return false;
    }
    theResult = LargeIntegerUnsigned::gcd(theResult, theInts[i].value);
  }
  return output.assignValue(theResult, theCommands);
}

bool CalculatorFunctions::innerLogBaseNaturalToLn(Calculator& theCommands, const Expression& input, Expression& output) {
  if (!input.startsWith(theCommands.opLogBase(), 3)) {
    return false;
  }
  if (!input[1].isOperationGiven(theCommands.opE())) {
    return false;
  }
  return output.makeOX(theCommands, theCommands.opLog(), input[2]);
}

bool CalculatorFunctions::innerLogBaseSimpleCases(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerLogBaseSimpleCases");
  if (!input.startsWith(theCommands.opLogBase(), 3)) {
    return false;
  }
  Rational theBase, theArg;
  if (
    !input[1].isOfType<Rational>(&theBase) ||
    !input[2].isOfType<Rational>(&theArg)
  ) {
    return false;
  }
  if (theBase < 0 || theArg < 0) {
    return false;
  }
  if (theBase == 1) {
    return false;
  }
  if (theBase == 0 || theArg == 0) {
    std::stringstream errorStream;
    errorStream << "Attempt to take logarithm base " << theBase
    << " of " << theArg << " is not allowed. ";
    return output.makeError(errorStream.str(), theCommands);
  }
  if (theArg == 1) {
    return output.assignValue(0, theCommands);
  }
  Expression newBaseE, newArgE;
  newBaseE.assignValue(theBase, theCommands);
  newArgE.assignValue(theArg, theCommands);
  if (theBase < 1) {
    theBase.invert();
    newBaseE.assignValue(theBase, theCommands);
    output.MakeXOX(theCommands, theCommands.opLogBase(), newBaseE, newArgE);
    output *= - 1;
    return true;
  }
  if (theArg < 1) {
    theArg.invert();
    newArgE.assignValue(theArg, theCommands);
    output.MakeXOX(theCommands, theCommands.opLogBase(), newBaseE, newArgE);
    output *= - 1;
    return true;
  }
  LargeInteger baseInt, argNum;
  if (!theBase.isInteger(&baseInt)) {
    return false;
  }
  LargeInteger simplerBase;
  int simplerPower = - 1;
  bool isPower = false;
  if (baseInt.tryIsPower(isPower, simplerBase, simplerPower)) {
    if (isPower) {
      newBaseE.assignValue(Rational(simplerBase), theCommands);
      output.MakeXOX(theCommands, theCommands.opLogBase(), newBaseE, newArgE);
      output /= simplerPower;
      return true;
    }
  }
  argNum = theArg.getNumerator();
  LargeInteger argDen = theArg.getDenominator();
  double doubleBase = baseInt.GetDoubleValue();
  double doubleArgNum = argNum.GetDoubleValue();
  if (FloatingPoint::logFloating(doubleArgNum) / FloatingPoint::logFloating(doubleBase) > 1000) {
    return false;
  }
  int intPart = 0;
  bool changed = false;
  while (argNum % baseInt == 0) {
    intPart ++;
    argNum /= baseInt;
    changed = true;
  }
  while (argDen % baseInt == 0) {
    intPart --;
    argDen /= baseInt;
    changed = true;
  }
  if (!changed) {
    return false;
  }
  theArg = argNum;
  theArg /= argDen;
  newBaseE.assignValue(theBase, theCommands);
  newArgE.assignValue(theArg, theCommands);
  output.MakeXOX(theCommands, theCommands.opLogBase(), newBaseE, newArgE);
  if (intPart == 0) {
    return true;
  }
  output += intPart;
  return true;
}

std::string InputBox::getSliderName() const {
  List<unsigned char> hex;
  Crypto::computeSha256(this->name, hex);
  return this->name + "_" + Crypto::convertListUnsignedCharsToHex(hex);
}

std::string InputBox::getUserInputBox() const {
  MacroRegisterFunctionWithName("InputBox::getUserInputBox");
  std::stringstream out;
  double theReader = 0;
  out.precision(4);
  out << std::fixed;
  if (this->value.evaluatesToDouble(&theReader)) {
    out << "\\FormInput" << "[" << theReader
    << "]" << "{" << this->name << "}" ;
  } else {
    out << "\\FormInput" << "[" << this->value.toString()
    << "]" << "{" << this->name << "}" ;
  }
  return out.str();
}

bool CalculatorFunctions::innerMakeJavascriptExpressioN(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerMakeJavascriptExpressioN");
  if (input.size() != 2) {
    return false;
  }
  return CalculatorFunctions::functionMakeJavascriptExpression(theCommands, input[1], output);
}

bool CalculatorFunctions::functionMakeJavascriptExpression(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::functionMakeJavascriptExpression");
  RecursionDepthCounter theCounter(&theCommands.RecursionDeptH);
  if (theCommands.recursionDepthExceededHandleRoughly()) {
    return false;
  }
  std::string atomString;
  if (input == theCommands.expressionMinusInfinity()) {
    return output.assignValue<std::string>("\"minusInfinity\"", theCommands);
  }
  if (input == theCommands.expressionInfinity()) {
    return output.assignValue<std::string>("\"infinity\"", theCommands);
  }
  if (input.isOperation(&atomString)) {
    if (input.isOperationGiven(theCommands.opE())) {
      return output.assignValue<std::string>(" 2.718281828 ", theCommands);
    }
    if (input.isOperationGiven(theCommands.opPi())) {
      return output.assignValue<std::string>(" 3.141592654 ", theCommands);
    }
    if (input.theData >= theCommands.NumPredefinedAtoms) {
      return output.assignValue(HtmlRoutines::getJavascriptVariable(atomString), theCommands);
    }
    if (atomString == "+" || atomString == "*" || atomString == "/" || atomString == "-") {
      return output.assignValue(atomString, theCommands);
    }
    return output.assignValue(atomString, theCommands);
  }
  std::stringstream out;
  InputBox theBox;
  if (input.isOfType(&theBox)) {
    out << "parseFloat(document.getElementsByName('" << theBox.getSliderName() << "')[0].value)";
    return output.assignValue(out.str(), theCommands);
  }
  out.precision(7);
  bool hasDoubleValue = false;
  double theDoubleValue = - 1;
  if (input.isOfType<Rational>()) {
    hasDoubleValue = true;
    theDoubleValue = input.getValue<Rational>().GetDoubleValue();
  }
  if (input.isOfType<AlgebraicNumber>()) {
    hasDoubleValue = input.getValue<AlgebraicNumber>().evaluatesToDouble(&theDoubleValue);
  }
  if (input.isOfType<double>()) {
    hasDoubleValue = true;
    theDoubleValue = input.getValue<double>();
  }
  if (hasDoubleValue) {
    std::string theDoubleString = FloatingPoint::doubleToString(theDoubleValue);
    if (theDoubleString.size() > 0) {
      if (theDoubleString[0] == '-') {
        out << "(" << theDoubleString << ")";
      } else {
        out << " " << theDoubleString;
      }
    }
    return output.assignValue(out.str(), theCommands);
  }
  Expression opE, leftE, rightE;
  if (input.startsWith(theCommands.opSequence()) || input.startsWith(theCommands.opIntervalOpen())) {
    out << "[";
    for (int i = 1; i < input.size(); i ++) {
      if (!CalculatorFunctions::functionMakeJavascriptExpression(theCommands, input[i], opE)) {
        return output.assignValue("(Failed to convert " + input[i].toString() + ")", theCommands);
      }
      out << opE.toString();
      if (i != input.size() - 1) {
        out << ", ";
      }
    }
    out << "]";
    return output.assignValue(out.str(), theCommands);
  }
  std::string opString, leftString, rightString;
  std::stringstream logStream;
  if (input.size() == 3 || input.size() == 2) {
    Expression* currentE = &opE;
    std::string* currentString = &opString;
    for (int i = 0; i < input.size(); i ++) {
      if (!CalculatorFunctions::functionMakeJavascriptExpression(theCommands, input[i], *currentE)) {
        return output.assignValue("(Failed to convert " + input[i].toString() + ")", theCommands);
      }
      if (!currentE->isOfType(currentString)) {
        return output.assignValue("(Failed to convert " + input[i].toString() + ")", theCommands);
      }
      logStream << "Converted: " << input[i].toString() << " to: "
      << *currentString << ". ";
      if (i == 0) {
        currentE = &leftE;
        currentString = &leftString;
      }
      if (i == 1) {
        currentE = &rightE;
        currentString = &rightString;
      }
    }
    if (input.size() == 3) {
      if (opString == "+" || opString == "-" || opString == "/" || opString == "*") {
        out << "(" << leftString <<  " " << opString << " " << rightString << ")";
        return output.assignValue(out.str(), theCommands);
      }
      if (opString == "\\sqrt") {
        out << "Math.pow(" << rightString << ", 1/" << leftString << ")";
        return output.assignValue(out.str(), theCommands);
      }
      if (opString == "^") {
        out << "Math.pow(" << leftString << ", " << rightString << ")";
        return output.assignValue(out.str(), theCommands);
      }
      if (opString == "LogBase") {
        out << "Math.log(" << rightString << ") / Math.log(" << leftString << ")";
        return output.assignValue(out.str(), theCommands);
      }
    }
    if (input.size() == 2) {
      std::string theFunName ="";
      if (
        opString == "\\sin" || opString == "\\cos" ||
        opString == "\\log" || opString == "\\tan" ||
        opString == "\\cot"
      ) {
        theFunName = opString.substr(1);
      }
      if (opString == "\\arccos") {
        theFunName = "acos";
      }
      if (opString == "\\arcsin") {
        theFunName = "asin";
      }
      if (opString == "\\arctan") {
        theFunName = "atan";
      }
      if (theFunName != "") {
        out << "(Math." << theFunName << "( " << leftString << "))";
        return output.assignValue(out.str(), theCommands);
      }
    }
    if (input.size() == 2) {
      if (opString == "|") {
        out << "(Math.abs( " << leftString << "))";
        return output.assignValue(out.str(), theCommands);
      }
    }
  }
  out << "(Failed to make expression from " << input.toString() << ". "
  << logStream.str() << ")";
  return output.assignValue(out.str(), theCommands);
}

bool CalculatorFunctions::innerPlotSetProjectionScreenBasis(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPlotSetProjectionScreenBasis");
  if (input.size() != 3) {
    return false;
  }
  Vector<double> v1, v2;
  if (
    !theCommands.getVectorDoubles(input[1], v1, 3) ||
    !theCommands.getVectorDoubles(input[2], v2, 3)
  ) {
    return theCommands << "Failed to extract 3d-vectors from "
    << input[1].toString() << ", " << input[2].toString() << ".";
  }
  Plot resultPlot;
  resultPlot.dimension = 3;
  PlotObject thePlot;
  thePlot.thePlotType = "setProjectionScreen";
  thePlot.thePointsDouble.addOnTop(v1);
  thePlot.thePointsDouble.addOnTop(v2);
  resultPlot += thePlot;
  return output.assignValue(resultPlot, theCommands);
}

bool CalculatorFunctions::innerPlotCoordinateSystem(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPlotCoordinateSystem");
  if (input.size() != 3) {
    return false;
  }
  Vector<double> corner1, corner2;
  if (
    !theCommands.getVectorDoubles(input[1], corner1, 3) ||
    !theCommands.getVectorDoubles(input[2], corner2, 3)
  ) {
    return theCommands << "Failed to extract 3d-vectors from "
    << input[1].toString() << ", "
    << input[2].toString() << ".";
  }
  Plot resultPlot;
  resultPlot.dimension = 3;
  PlotObject thePlot;
  thePlot.colorJS = "black";
  thePlot.thePlotType = "segment";
  thePlot.thePointsDouble.setSize(2);
  for (int i = 0; i < 3; i ++) {
    thePlot.thePointsDouble[0].makeZero(3);
    thePlot.thePointsDouble[1].makeZero(3);
    thePlot.thePointsDouble[0][i] = corner1[i];
    thePlot.thePointsDouble[1][i] = corner2[i];
    resultPlot += thePlot;
  }
  PlotObject plotLabels;
  plotLabels.thePlotType = "label";
  plotLabels.thePointsDouble.setSize(1);
  plotLabels.colorJS = "blue";
  for (char i = 0; i < 3; i ++) {
    plotLabels.thePointsDouble[0].makeZero(3);
    plotLabels.thePointsDouble[0][i] = corner2[i];
    std::stringstream out;
    out << static_cast<char>('x' + i);
    plotLabels.thePlotString = out.str();
    resultPlot += plotLabels;
  }
  return output.assignValue(resultPlot, theCommands);
}

bool CalculatorFunctions::innerPlotSurface(Calculator& theCommands, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPlotSurface");
  PlotObject thePlot;
  bool found = false;
  for (int i = 0; i < input.size(); i ++) {
    if (input[i].isSequenceNElements(3)) {
      thePlot.manifoldImmersion = input[i];
      found = true;
      break;
    }
  }
  if (!found) {
    return theCommands << "Could not find a triple of "
    << "functions expressions to use for "
    << "the surface. ";
  }
  thePlot.manifoldImmersion.getFreeVariables(thePlot.variablesInPlay, true);
  if (thePlot.variablesInPlay.size > 2) {
    return theCommands << "Got a surface with "
    << thePlot.variablesInPlay.size
    << " variables, namely: "
    << thePlot.variablesInPlay.toStringCommaDelimited()
    << ". I've been taught to plot 2d surfaces only. "
    << "Please reduce the number of variables to 2. ";
  }
  Expression uE,vE;
  uE.makeAtom("u", theCommands);
  vE.makeAtom("v", theCommands);
  if (thePlot.variablesInPlay.size == 1) {
    if (thePlot.variablesInPlay.contains(vE)) {
      thePlot.variablesInPlay.addOnTop(uE);
    } else {
      thePlot.variablesInPlay.addOnTop(vE);
    }
  }
  if (thePlot.variablesInPlay.size == 0) {
    thePlot.variablesInPlay.addOnTop(uE);
    thePlot.variablesInPlay.addOnTop(vE);
  }
  thePlot.variablesInPlay.quickSortAscending();
  thePlot.coordinateFunctionsE.setSize(thePlot.manifoldImmersion.size() - 1);
  thePlot.coordinateFunctionsJS.setSize(thePlot.coordinateFunctionsE.size);
  thePlot.theVarRangesJS.setSize(2);
  thePlot.variablesInPlayJS.setSize(2);
  for (int i = 0; i < 2; i ++) {
    thePlot.theVarRangesJS[i].setSize(2);
    thePlot.variablesInPlayJS[i] = thePlot.variablesInPlay[i].toString();
  }
  Expression jsConverter;
  for (int i = 1; i < thePlot.manifoldImmersion.size(); i ++) {
    thePlot.coordinateFunctionsE[i - 1] = thePlot.manifoldImmersion[i];
    bool isGood = CalculatorFunctions::functionMakeJavascriptExpression(
      theCommands, thePlot.coordinateFunctionsE[i - 1], jsConverter
    );
    if (isGood) {
      isGood = jsConverter.isOfType<std::string>(&thePlot.coordinateFunctionsJS[i - 1]);
    }
    if (!isGood) {
      return theCommands << "Failed to convert "
      << thePlot.coordinateFunctionsE[i - 1].toString()
      << " to a javascript expression. ";
    }
  }
  for (int i = 1; i < input.size(); i ++) {
    if (input[i].startsWith(theCommands.opIn(), 3)) {
      int theIndex = thePlot.variablesInPlay.getIndex(input[i][1]);
      if (theIndex < 0 || theIndex > 2) {
        // theIndex > 2 should never happen
        continue;
      }
      if (input[i][2].size() != 3) {
        continue;
      }
      for (int j = 0; j < 2; j ++) {
        bool isGood = CalculatorFunctions::functionMakeJavascriptExpression(
          theCommands, input[i][2][j + 1], jsConverter
        );
        if (isGood) {
          isGood = jsConverter.isOfType<std::string>(&thePlot.theVarRangesJS[theIndex][j]);
        }
        if (!isGood) {
          return theCommands << "Failed to convert "
          << input[i][2][j + 1].toString() << " to a javascript expression. ";
        }
      }
    }
  }
  MapList<std::string, Expression, MathRoutines::hashString> theKeys;
  if (CalculatorConversions::innerLoadKeysFromStatementLisT(
    theCommands, input, theKeys, &theCommands.comments, true
  )) {
    if (theKeys.contains("color1")) {
      thePlot.colorUV = theKeys.getValueCreate("color1").toString();
    }
    if (theKeys.contains("color2")) {
      thePlot.colorVU = theKeys.getValueCreate("color2").toString();
    }
    MapList<std::string, std::string, MathRoutines::hashString> keysToConvert;
    keysToConvert.getValueCreate("numSegments1");
    keysToConvert.getValueCreate("numSegments2");
    keysToConvert.getValueCreate("lineWidth");
    for (int i = 0; i < keysToConvert.size(); i ++) {
      if (!theKeys.contains(keysToConvert.theKeys[i])) {
        continue;
      }
      Expression expressionToConvert = theKeys.getValueCreate(keysToConvert.theKeys[i]);
      bool isGood = CalculatorFunctions::functionMakeJavascriptExpression(
        theCommands, expressionToConvert, jsConverter
      );
      if (isGood) {
        isGood = jsConverter.isOfType<std::string>(&keysToConvert.theValues[i]);
      }
      if (!isGood) {
        return theCommands << "Failed to convert "
        << expressionToConvert.toString()
        << " to a javascript expression. ";
      }
    }
    thePlot.numSegmenTsJS.setSize(2);
    if (keysToConvert.getValueCreate("numSegments1") != "") {
      thePlot.numSegmenTsJS[0] = keysToConvert.getValueCreate("numSegments1");
    }
    if (keysToConvert.getValueCreate("numSegments2") != "") {
      thePlot.numSegmenTsJS[1] = keysToConvert.getValueCreate("numSegments2");
    }
    if (keysToConvert.getValueCreate("lineWidth") != "") {
      thePlot.lineWidthJS = keysToConvert.getValueCreate("lineWidth");
    }
  }
  if (
    thePlot.theVarRangesJS[0][0] == "" ||
    thePlot.theVarRangesJS[0][1] == "" ||
    thePlot.theVarRangesJS[1][0] == "" ||
    thePlot.theVarRangesJS[1][1] == ""
  ) {
    return theCommands << "Could not extract variable ranges, got the var ranges: "
    << thePlot.theVarRangesJS;
  }
  thePlot.thePlotType = "surface";
  thePlot.dimension = thePlot.coordinateFunctionsE.size;
  Plot result;
  result += thePlot;
  input.hasInputBoxVariables(&result.boxesThatUpdateMe);
  return output.assignValue(result, theCommands);
}

class QRFactorizationComputation{
};

bool CalculatorFunctions::innerPolynomialDivisionRemainder(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerPolynomialDivisionRemainder");
  ExpressionContext theContext(theCommands);
  Vector<Polynomial<AlgebraicNumber> > polynomials;
  if (!theCommands.getListPolynomialVariableLabelsLexicographic(
    input, polynomials, theContext
  )) {
    return output.makeError("Failed to extract list of polynomials. ", theCommands);
  }
  GroebnerBasisComputation<AlgebraicNumber> computation;
  computation.thePolynomialOrder.monomialOrder = MonomialP::orderDefault();
  computation.flagStoreQuotients = true;
  for (int i = 1; i < polynomials.size; i ++) {
    if (polynomials[i].isEqualToZero()) {
      return output.makeError("Division by zero.", theCommands);
    }
    computation.addBasisElementNoReduction(polynomials[i]);
  }
  Polynomial<AlgebraicNumber> outputRemainder;
  computation.remainderDivisionByBasis(polynomials[0], outputRemainder, - 1);
  Expression thePolyE;
  thePolyE.assignValueWithContext(outputRemainder, theContext, theCommands);
  output.reset(theCommands);
  output.addChildAtomOnTop("MakeExpression");
  output.addChildOnTop(thePolyE);
  return true;
}

bool CalculatorFunctions::innerPolynomialDivisionVerboseGrLex(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  return CalculatorFunctions::innerPolynomialDivisionVerbose(
    theCommands, input, output, &MonomialP::orderDegreeThenLeftLargerWins()
  );
}

bool CalculatorFunctions::innerPolynomialDivisionVerboseGradedReverseLexicographic(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  List<MonomialP>::Comparator order(
    MonomialP::greaterThan_totalDegree_rightSmallerWins
  );
  return CalculatorFunctions::innerPolynomialDivisionVerbose(
    theCommands, input, output, &order
  );
}

bool CalculatorFunctions::innerPolynomialDivisionVerboseLexicographic(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPolynomialDivisionVerboseLexicographic");
  List<MonomialP>::Comparator order(
    MonomialP::greaterThan_leftLargerWins
  );
  return CalculatorFunctions::innerPolynomialDivisionVerbose(
    theCommands,
    input,
    output,
    &order
  );
}

bool CalculatorFunctions::innerPolynomialDivisionVerboseLexicographicOpposite(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  List<MonomialP>::Comparator order(
    MonomialP::greaterThan_rightLargerWins
  );
  return CalculatorFunctions::innerPolynomialDivisionVerbose(
    theCommands, input, output, &order
  );
}

bool CalculatorFunctions::innerPolynomialDivisionVerbose(
  Calculator& theCommands,
  const Expression& input,
  Expression& output,
  List<MonomialP>::Comparator* monomialOrder
) {
  MacroRegisterFunctionWithName("Calculator::innerPolynomialDivisionVerbose");
  ExpressionContext theContext(theCommands);
  Vector<Polynomial<AlgebraicNumber> > polynomialsRational;
  if (!theCommands.getListPolynomialVariableLabelsLexicographic(
    input, polynomialsRational, theContext
  )) {
    return output.makeError(
      "Failed to extract list of polynomials. ",
      theCommands
    );
  }
  GroebnerBasisComputation<AlgebraicNumber> computation;
  computation.flagDoLogDivision = true;
  computation.flagStoreQuotients = true;
  computation.thePolynomialOrder.monomialOrder = *monomialOrder;
  for (int i = 1; i < polynomialsRational.size; i ++) {
    if (polynomialsRational[i].isEqualToZero()) {
      return output.makeError("Division by zero.", theCommands);
    }
    computation.addBasisElementNoReduction(polynomialsRational[i]);
  }
  if (monomialOrder != nullptr) {
    computation.thePolynomialOrder.monomialOrder = *monomialOrder;
  }
  computation.remainderDivisionByBasis(polynomialsRational[0], computation.remainderDivision, - 1);
  theContext.getFormat(computation.theFormat);
  computation.theFormat.flagUseLatex = true;
  computation.theFormat.flagUseFrac = true;
  std::stringstream latexOutput;
  latexOutput << "<br>In latex: <br>"
  << "\\documentclass{article}\\usepackage{longtable}"
  << "\\usepackage{xcolor}\\usepackage{multicol}"
  << "\\begin{document} "
  << computation.getDivisionStringLaTeX()
  << "\\end{document}";
  std::stringstream out;
  out << computation.getDivisionStringHtml();
  out << latexOutput.str();
  return output.assignValue(out.str(), theCommands);
}

template <class Coefficient>
std::string GroebnerBasisComputation<Coefficient>::getSpacedMonomialsWithHighlightLaTeX(
  const Polynomial<Coefficient>& thePoly,
  List<List<int> >* slidesToHighlightMon,
  List<int>* slidesToFcAnswer,
  List<int>* slidesToUncover,
  List<int>* slidesAdditionalHighlight,
  int slidesToUncoverAllMons,
  bool useColumnSeparator
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::getSpacedMonomialsWithHighlightLaTeX");
  (void) slidesToUncoverAllMons;
  std::stringstream out;
  bool found = false;
  int countMons = 0;
  if (thePoly.isEqualToZero()) {
    if (useColumnSeparator) {
      for (int i = 0; i < this->allMonomials.size * 2 - 1; i ++) {
        out << "&";
      }
    }
    std::stringstream highlightHeadStream;
    std::stringstream highlightTailStream;
    MonomialP tempM;
    tempM.makeOne();
    int monIndex = this->allMonomials.getIndex(tempM);
    if (slidesAdditionalHighlight != nullptr && monIndex != - 1) {
      if ((*slidesAdditionalHighlight)[monIndex] > 0) {
        highlightHeadStream << "{ \\only<"
        << (*slidesAdditionalHighlight)[monIndex]
        << "->{\\color{orange}}";
        highlightTailStream << "}";
      }
    }
    if (slidesToFcAnswer != nullptr && monIndex != - 1) {
      if ((*slidesToFcAnswer)[monIndex] > 1) {
        highlightHeadStream << "\\fcAnswer{" << (*slidesToFcAnswer)[monIndex] << "}{";
        highlightTailStream << "}";
      }
    }
    out << "$ " << highlightHeadStream.str() << "0"
    << highlightTailStream.str() << "$";
    return out.str();
  }
  for (int i = 0; i < this->allMonomials.size; i ++) {
    int theIndex = thePoly.theMonomials.getIndex(this->allMonomials[i]);
    if (theIndex == - 1) {
      if (useColumnSeparator) {
        if (i != this->allMonomials.size - 1) {
          out << "&&";
        }
      }
      continue;
    }
    std::string highlightHead;
    std::string highlightTail;
    int fcAnswerSlide = - 1;
    if (slidesToFcAnswer != nullptr) {
      if (i < (*slidesToFcAnswer).size) {
        if ((*slidesToFcAnswer)[i] > 1) {
          fcAnswerSlide = (*slidesToFcAnswer)[i];
          if (slidesToHighlightMon != nullptr) {
            (*slidesToHighlightMon)[i].addOnTop(fcAnswerSlide);
          }
        }
      }
    }
    if (slidesToUncover != nullptr) {
      if ((*slidesToUncover)[i] > 1) {
        std::stringstream highlightStream;
        highlightStream << "\\uncover<" << (*slidesToUncover)[i] << "->{";
        highlightHead += highlightStream.str();
        highlightTail = "}" + highlightTail;
      }
    }
    if (slidesToHighlightMon != nullptr) {
      if ((*slidesToHighlightMon)[i].size > 0) {
        highlightHead += "\\alertNoH{" + (*slidesToHighlightMon)[i].toStringCommaDelimited() + "}{";
        highlightTail = "}" + highlightTail;
      }
    }
    if (slidesAdditionalHighlight != nullptr) {
      if ((*slidesAdditionalHighlight)[i] > 0) {
        std::stringstream highlightStream;
        highlightStream << "{\\only<" << (*slidesAdditionalHighlight)[i] << "->{\\color{orange}}";
        highlightHead += highlightStream.str();
        highlightTail = "}" + highlightTail;
      }
    }
    countMons ++;
    std::string monWithSign =
    Polynomial<Coefficient>::getBlendCoefficientAndMonomial(
      thePoly[theIndex], thePoly.coefficients[theIndex], true, &this->theFormat
    );
    std::string sign = monWithSign.substr(0, 1);
    std::string monNoSign = monWithSign.substr(1);
    if (sign == "-" || found) {
      if (fcAnswerSlide != - 1) {
        out << "\\uncover<" << fcAnswerSlide << "->{";
      }
      out << highlightHead;
      out << "$" << sign << "$";
      out << highlightTail;
      if (fcAnswerSlide != - 1) {
        out << "}";
      }
    }
    found = true;
    if (useColumnSeparator) {
      out << "&";
    } else {
      out << " ";
    }
    if (fcAnswerSlide != - 1) {
      out << "\\fcAnswer{" << fcAnswerSlide << "}{";
    }
    out << highlightHead << "$" << monNoSign << "$" << highlightTail;
    if (fcAnswerSlide != - 1) {
      out << "}";
    }
    if (!useColumnSeparator) {
      out << " ";
    }
    if (useColumnSeparator) {
      if (i != this->allMonomials.size - 1) {
        out << "&";
      }
    }
  }
  if (countMons != thePoly.size()) {
    out << " Programming ERROR!";
  }
  return out.str();
}

template <class Coefficient>
void GroebnerBasisComputation<Coefficient>::computeHighLightsFromRemainder(
  int remainderIndex, int& currentSlideNumber
) {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::computeHighLightsFromRemainder");
  if (remainderIndex == 0) {
    for (int i = 0; i < this->allMonomials.size; i ++) {
      this->highlightMonsRemainders[remainderIndex][i].addOnTop(currentSlideNumber);
    }
    currentSlideNumber ++;
    for (int j = 0; j < this->theBasis.size; j ++) {
      for (int i = 0; i < this->allMonomials.size; i ++) {
        this->highlightMonsDivisors[j][i].addOnTop(currentSlideNumber);
      }
      currentSlideNumber ++;
    }
  }
  for (int i = 0; i < this->intermediateHighlightedMons.getElement()[remainderIndex].size; i ++) {
    int theMonIndex = this->allMonomials.getIndex(
      this->intermediateHighlightedMons.getElement()[remainderIndex][i]
    );
    this->additionalHighlightRemainders[remainderIndex][theMonIndex] = currentSlideNumber;
    this->additionalHighlightFinalRemainder[theMonIndex] = currentSlideNumber;
    this->uncoverMonsFinalRemainder[theMonIndex] = currentSlideNumber;
    currentSlideNumber ++;
  }
  MonomialP constMon;
  constMon.makeOne();
  int zeroMonIndex = this->allMonomials.getIndex(constMon);
  if (this->intermediateRemainders.getElement()[remainderIndex].isEqualToZero()) {
    this->additionalHighlightRemainders[remainderIndex][zeroMonIndex] = currentSlideNumber;
    this->additionalHighlightFinalRemainder[zeroMonIndex] = currentSlideNumber;
    currentSlideNumber ++;
  }
  if (remainderIndex == this->intermediateRemainders.getElement().size - 1) {
    for (int i = 0; i < this->theBasis.size; i ++) {
      this->uncoverAllMonsQuotients[i] = currentSlideNumber;
      for (int j = 0; j < this->allMonomials.size; j ++) {
        this->highlightMonsQuotients[i][j].addOnTop(currentSlideNumber);
      }
    }
    currentSlideNumber ++;
    this->highlightAllMonsFinalRemainder = currentSlideNumber;
  }
  if (remainderIndex >= this->intermediateSelectedDivisors.getElement().size) {
    return;
  }
  Polynomial<Coefficient>& currentRemainder = this->intermediateRemainders.getElement()[remainderIndex];
  int indexCurrentDivisor = this->intermediateSelectedDivisors.getElement()[remainderIndex];
  Polynomial<Coefficient>& currentDivisor = this->theBasis[indexCurrentDivisor].element;
  MonomialP divisorLeadingMonomial;
  int indexCurrentDivisorLeadingMoN = currentDivisor.getIndexLeadingMonomial(
    &divisorLeadingMonomial, nullptr, &this->thePolynomialOrder.monomialOrder
  );
  int indexCurrentDivisorLeadingMonInAllMons = this->allMonomials.getIndex(
    divisorLeadingMonomial
  );
  MonomialP maxMonCurrentRemainder;
  Coefficient leadingCFCurrentRemainder;
  currentRemainder.getIndexLeadingMonomial(
    &maxMonCurrentRemainder, &leadingCFCurrentRemainder, &this->thePolynomialOrder.monomialOrder
  );
  int indexCurrentRemainderLeadingMonInAllMons = this->allMonomials.getIndex(maxMonCurrentRemainder);
  this->highlightMonsDivisors[indexCurrentDivisor][indexCurrentDivisorLeadingMonInAllMons].addOnTop(currentSlideNumber);
  this->highlightMonsRemainders[remainderIndex][indexCurrentRemainderLeadingMonInAllMons].addOnTop(currentSlideNumber);

  if (remainderIndex == 0) {
    this->longDivisionLog << "$\\vphantom"
    << "{\\frac{x^1}{x^1}}$";
  }
  this->longDivisionLog << "\\only<" << currentSlideNumber << ","
  << currentSlideNumber + 1 << "| handout:0>{Divide "
  << "\\alertNoH{"
  << currentSlideNumber << ","
  << currentSlideNumber + 1 << "}{"
  << "$" << currentRemainder.getBlendCoefficientAndMonomial(
    maxMonCurrentRemainder, leadingCFCurrentRemainder, false, &this->theFormat
  )
  << "$ "
  << "}"
  << " by "
  << "\\alertNoH{"
  << currentSlideNumber << ","
  << currentSlideNumber + 1 << "}{"
  << "$" << currentRemainder.getBlendCoefficientAndMonomial(
    currentDivisor.theMonomials[indexCurrentDivisorLeadingMoN],
    currentDivisor.coefficients[indexCurrentDivisorLeadingMoN],
    false,
    &this->theFormat
  )
  << "$"
  << "}."
  << "}";
  currentSlideNumber ++;
  this->highlightMonsDivisors[indexCurrentDivisor][indexCurrentDivisorLeadingMonInAllMons].
  addOnTop(currentSlideNumber);
  this->highlightMonsRemainders[remainderIndex][indexCurrentRemainderLeadingMonInAllMons].addOnTop(currentSlideNumber);
  int indexCurrentQuotientMonInAllMons =
  this->allMonomials.getIndex(this->intermediateHighestMonDivHighestMon.getElement()[remainderIndex]);
  Polynomial<Coefficient>& currentQuotient = this->theQuotients[indexCurrentDivisor];
  int indexCurrentQuotientMoN = currentQuotient.theMonomials.getIndex(
    this->intermediateHighestMonDivHighestMon.getElement()[remainderIndex]
  );
  this->fcAnswerMonsQuotients[indexCurrentDivisor][indexCurrentQuotientMonInAllMons] = currentSlideNumber;
  currentSlideNumber ++;
  this->highlightMonsQuotients[indexCurrentDivisor][indexCurrentQuotientMonInAllMons].addOnTop(currentSlideNumber);
  for (int i = 0; i < currentDivisor.size(); i ++) {
    this->highlightMonsDivisors[indexCurrentDivisor][
      this->allMonomials.getIndex(currentDivisor[i])
    ].addOnTop(currentSlideNumber);
  }
  this->uncoverAllMonsSubtracands[remainderIndex] = currentSlideNumber;
  this->longDivisionLog << "\\only<"
  << currentSlideNumber << ", "
  << currentSlideNumber + 1
  << "| handout:0>{Multiply "
  << "\\alertNoH{"
  << currentSlideNumber << ", "
  << currentSlideNumber + 1
  << "}{$"
  << currentQuotient.getBlendCoefficientAndMonomial(
    currentQuotient.theMonomials[indexCurrentQuotientMoN],
    currentQuotient.coefficients[indexCurrentQuotientMoN],
    false,
    &this->theFormat
  )
  << "$}"
  << " by divisor. "
  << "}";
  currentSlideNumber ++;
  this->highlightMonsQuotients[indexCurrentDivisor][indexCurrentQuotientMonInAllMons].addOnTop(currentSlideNumber);
  for (int i = 0; i < currentDivisor.size(); i ++) {
    this->highlightMonsDivisors[indexCurrentDivisor][
      this->allMonomials.getIndex(currentDivisor[i])
    ].addOnTop(currentSlideNumber);
  }
  if (this->fcAnswerMonsSubtracands[remainderIndex].size != this->allMonomials.size) {
    this->fcAnswerMonsSubtracands[remainderIndex].initializeFillInObject(this->allMonomials.size, - 1);
  }
  for (int i = 0; i < this->intermediateSubtractands.getElement()[remainderIndex].size(); i ++) {
    this->fcAnswerMonsSubtracands[remainderIndex][
      this->allMonomials.getIndex(this->intermediateSubtractands.getElement()[remainderIndex][i])
    ] = currentSlideNumber;
  }
  currentSlideNumber ++;
  for (int i = 0; i < this->intermediateRemainders.getElement()[remainderIndex].size(); i ++) {
    this->highlightMonsRemainders[remainderIndex][
      this->allMonomials.getIndex(this->intermediateRemainders.getElement()[remainderIndex][i])
    ].addOnTop(currentSlideNumber);
  }
  for (int i = 0; i < this->intermediateSubtractands.getElement()[remainderIndex].size(); i ++) {
    this->highlightMonsSubtracands[remainderIndex][
      this->allMonomials.getIndex(this->intermediateSubtractands.getElement()[remainderIndex][i])
    ].addOnTop(currentSlideNumber);
  }
  this->uncoverAllMonsRemainders[remainderIndex + 1] = currentSlideNumber;
  this->longDivisionLog << "\\only<" << currentSlideNumber << ", "
  << currentSlideNumber + 1 << "| handout:0>{Subtract last two polynomials.}";
  currentSlideNumber ++;
  for (int i = 0; i < this->intermediateRemainders.getElement()[remainderIndex].size(); i ++) {
    this->highlightMonsRemainders[remainderIndex][
      this->allMonomials.getIndex(this->intermediateRemainders.getElement()[remainderIndex][i])
    ].addOnTop(currentSlideNumber);
  }
  for (int i = 0; i < this->intermediateSubtractands.getElement()[remainderIndex].size(); i ++) {
    this->highlightMonsSubtracands[remainderIndex][
      this->allMonomials.getIndex(this->intermediateSubtractands.getElement()[remainderIndex][i])
    ].addOnTop(currentSlideNumber);
  }
  if (remainderIndex + 1 >= this->intermediateRemainders.getElement().size) {
    global.fatal << "Something is wrong: not enough intermediate remainders. " << global.fatal;
  }
  for (int i = 0; i < this->intermediateRemainders.getElement()[remainderIndex + 1].size(); i ++) {
    this->fcAnswerMonsRemainders[remainderIndex + 1][
      this->allMonomials.getIndex(this->intermediateRemainders.getElement()[remainderIndex + 1][i])
    ] = currentSlideNumber;
  }
  if (this->intermediateRemainders.getElement()[remainderIndex + 1].isEqualToZero()) {
    this->fcAnswerMonsRemainders[remainderIndex + 1][zeroMonIndex] = currentSlideNumber;
  }
  currentSlideNumber ++;
}

template <class Coefficient>
std::string GroebnerBasisComputation<Coefficient>::getDivisionLaTeXSlide() {
  MacroRegisterFunctionWithName("GroebnerBasisComputation::getDivisionLaTeXSlide");
  std::stringstream out;
  List<Polynomial<Coefficient> >& theRemainders = this->intermediateRemainders.getElement();
  List<Polynomial<Coefficient> >& theSubtracands = this->intermediateSubtractands.getElement();
  this->theFormat.monomialOrder = this->thePolynomialOrder.monomialOrder;
  bool oneDivisor = (this->theBasis.size == 1);
  this->allMonomials.clear();
  this->allMonomials.addOnTopNoRepetition(this->startingPoly.getElement().theMonomials);
  for (int i = 0; i < theRemainders.size; i ++) {
    this->allMonomials.addOnTopNoRepetition(theRemainders[i].theMonomials);
  }
  for (int i = 0; i < theSubtracands.size; i ++) {
    this->allMonomials.addOnTopNoRepetition(theSubtracands[i].theMonomials);
  }
  for (int i = 0; i < this->theBasis.size; i ++) {
    Polynomial<Coefficient>& current = this->theBasis[i].element;
    this->allMonomials.addOnTopNoRepetition(current.theMonomials);
  }
  for (int i = 0; i < this->theQuotients.size; i ++) {
    this->allMonomials.addOnTopNoRepetition(this->theQuotients[i].theMonomials);
  }
  if (this->remainderDivision.isEqualToZero()) {
    MonomialP constMon;
    constMon.makeOne();
    this->allMonomials.addOnTopNoRepetition(constMon);
  }
  this->allMonomials.quickSortDescending(&this->thePolynomialOrder.monomialOrder);
  List<List<int> > dummyListList;
  List<int> dummyList;
  dummyListList.setSize(this->allMonomials.size);
  dummyList.initializeFillInObject(this->allMonomials.size, - 1);
  this->firstNonZeroIndicesPerIntermediateSubtracand.initializeFillInObject(theSubtracands.size, 0);
  this->highlightMonsRemainders.initializeFillInObject(theRemainders.size,   dummyListList);
  this->highlightMonsSubtracands.initializeFillInObject(theSubtracands.size, dummyListList);
  this->highlightMonsQuotients.initializeFillInObject(this->theBasis.size,   dummyListList);
  this->highlightMonsDivisors.initializeFillInObject (this->theBasis.size,   dummyListList);
  this->fcAnswerMonsRemainders.initializeFillInObject(theRemainders.size,    dummyList);
  this->fcAnswerMonsSubtracands.initializeFillInObject(theSubtracands.size,  dummyList);
  this->fcAnswerMonsQuotients.initializeFillInObject(this->theBasis.size,    dummyList);
  this->fcAnswerMonsDivisors.initializeFillInObject (this->theBasis.size,    dummyList);
  this->uncoverAllMonsRemainders.initializeFillInObject (theRemainders.size, 1);
  this->uncoverAllMonsSubtracands.initializeFillInObject(theSubtracands.size, 1);
  this->uncoverAllMonsQuotients.initializeFillInObject  (this->theBasis.size, 1);
  this->uncoverAllMonsDivisors.initializeFillInObject   (this->theBasis.size, 1);
  this->uncoverMonsFinalRemainder.initializeFillInObject(this->allMonomials.size, - 1);
  this->additionalHighlightFinalRemainder.initializeFillInObject(this->allMonomials.size,- 1);
  this->additionalHighlightRemainders.initializeFillInObject(this->allMonomials.size, dummyList);
  this->highlightAllMonsFinalRemainder = - 1;
  int currentSlideNumer = this->firstIndexLatexSlide + 1;
  for (int i = 0; i < theRemainders.size; i ++) {
    this->computeHighLightsFromRemainder(i, currentSlideNumer);
  }
  for (int i = 0; i < theSubtracands.size; i ++) {
    this->firstNonZeroIndicesPerIntermediateSubtracand[i] = theSubtracands[i].getIndexLeadingMonomial(
      nullptr,
      nullptr,
      &this->thePolynomialOrder.monomialOrder
    );
  }
  this->theFormat.flagUseLatex = true;
  out << "\\renewcommand{\\arraystretch}{1.2}";
  out << "\\begin{longtable}{@{}c";
  for (int i = 0; i < this->allMonomials.size * 2; i ++) {
    out << "@{}c";
  }
  out << "}";
  if (!oneDivisor) {
    out << "{\\color{orange}\\textbf{Remainder:} }&"
    << this->getSpacedMonomialsWithHighlightLaTeX(
      this->remainderDivision,
      nullptr,
      nullptr,
      &this->uncoverMonsFinalRemainder,
      &this->additionalHighlightFinalRemainder,
      - 1,
      true
    )
    << "\\\\\\hline";
  }
  if (!oneDivisor) {
    out << "\\textbf{Divisor(s)} &" << "\\multicolumn{"
    << this->allMonomials.size * 2 << "}{c}{"
    << "\\alertNoH{" << this->uncoverAllMonsQuotients[0] << "}{"
    << "\\textbf{Quotient(s)}"
    << "}"
    << "}"
    << "\\\\";
  }
  for (int i = 0; i < this->theBasis.size; i ++) {
    if (!oneDivisor) {
      out << this->getSpacedMonomialsWithHighlightLaTeX(
        this->theBasis[i].element,
        &this->highlightMonsDivisors[i],
        &this->fcAnswerMonsDivisors[i],
        nullptr,
        nullptr,
        this->uncoverAllMonsDivisors[i],
        false
      );
    } else {
      out << "\\uncover<" << this->uncoverAllMonsQuotients[0] << "->{"
      << "\\alertNoH{" << this->uncoverAllMonsQuotients[0] << "}{"
      << "\\textbf{Quotient: }"
      << "}"
      << "}";
    }
    out << "&";
    out << "\\multicolumn{" << this->allMonomials.size * 2 << "}{c}{";
    out << this->getSpacedMonomialsWithHighlightLaTeX(
      this->theQuotients[i],
      &this->highlightMonsQuotients[i],
      &this->fcAnswerMonsQuotients[i],
      nullptr,
      nullptr,
      this->uncoverAllMonsQuotients[i],
      false
    );
    out << "}";
    out << "\\\\";
    if (!oneDivisor) {
      out << "\\hline";
    }
  }
  out << " \\cline{2-" << this->allMonomials.size * 2 + 1 << "}"
  << " \\cline{2-" << this->allMonomials.size * 2 + 1 << "}";
  for (int i = 0; i < theRemainders.size; i ++) {
    if (i == 0) {
      if (oneDivisor) {
        out << "\\multicolumn{1}{c|}{"
        << this->getSpacedMonomialsWithHighlightLaTeX(
          this->theBasis[0].element,
          &this->highlightMonsDivisors[0],
          &this->fcAnswerMonsDivisors[i],
          nullptr,
          nullptr,
          0,
          false
        )
        << "}";
      }
    } else {
      out << "\\uncover<" << this->uncoverAllMonsRemainders[i] << "->{";
    }
    if (i == theRemainders.size - 1 && i != 0 && oneDivisor) {
      out << "\\uncover<" << this->highlightAllMonsFinalRemainder << "->{"
      << "\\textbf{\\color{orange}Remainder: }"
      << "}";
    }
    out << "&";
    out << this->getSpacedMonomialsWithHighlightLaTeX(
      theRemainders[i],
      &this->highlightMonsRemainders[i],
      &this->fcAnswerMonsRemainders[i],
      nullptr,
      &this->additionalHighlightRemainders[i],
      this->uncoverAllMonsRemainders[i],
      true
    )
    << "\\\\";
    if (i < theSubtracands.size) {
      out << "\\uncover<" << this->uncoverAllMonsSubtracands[i] << "->{";
      out << "\\uncover<" << this->uncoverAllMonsSubtracands[i] + 2
      << "->{\\alertNoH{" << this->uncoverAllMonsSubtracands[i] + 2
      << ", " << this->uncoverAllMonsSubtracands[i] + 3
      << "}{"
      << "$\\overline{\\phantom{A}}$"
      << "}}";
      out << "&";
      out << this->getSpacedMonomialsWithHighlightLaTeX(
        theSubtracands[i],
        &this->highlightMonsSubtracands[i],
        &this->fcAnswerMonsSubtracands[i],
        nullptr,
        nullptr,
        this->uncoverAllMonsSubtracands[i],
        true
      );
      out << "\\\\\\cline{"
      << this->firstNonZeroIndicesPerIntermediateSubtracand[i] * 2 + 2
      << "-" << this->allMonomials.size * 2 + 1 << "}";
      out << "}";
    }
    if (i != 0) {
      out << "}";
    }
  }
  out << "\\end{longtable}";
  out << "\r\n";
  out << this->longDivisionLog.str();
  return out.str();
}

bool CalculatorFunctions::innerPolynomialDivisionSlidesGrLex(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerPolynomialDivisionSlidesGrLex");
  ExpressionContext theContext(theCommands);
  Vector<Polynomial<AlgebraicNumber> > polynomialsRational;
  if (!theCommands.getListPolynomialVariableLabelsLexicographic(
    input, polynomialsRational, theContext
  )) {
    return output.makeError("Failed to extract list of polynomials. ", theCommands);
  }
  if (polynomialsRational.size < 3) {
    return theCommands
    << "Function takes at least 3 inputs: "
    << "index of first slide, dividend, divisor(s).";
  }
  GroebnerBasisComputation<AlgebraicNumber> computation;
  computation.flagDoLogDivision = true;
  computation.flagStoreQuotients = true;
  computation.thePolynomialOrder.monomialOrder.setComparison(
    MonomialP::greaterThan_totalDegree_rightSmallerWins
  );
  for (int i = 2; i < polynomialsRational.size; i ++) {
    if (polynomialsRational[i].isEqualToZero()) {
      return output.makeError("Division by zero.", theCommands);
    }
    computation.addBasisElementNoReduction(polynomialsRational[i]);
  }
  if (!polynomialsRational[0].isSmallInteger(&computation.firstIndexLatexSlide)) {
    return theCommands << "Failed to extract integer from first argument";
  }
  computation.remainderDivisionByBasis(polynomialsRational[1], computation.remainderDivision, - 1);
  theContext.getFormat(computation.theFormat);
  computation.theFormat.flagUseLatex = true;
  computation.theFormat.flagUseFrac = true;
  std::stringstream latexOutput;
  latexOutput
  << "In latex: \r\n \\documentclass{beamer}\n"
  << "\\usepackage{longtable}\\usepackage{xcolor}\\usepackage{multicol}\n"
  << "\\newcommand{\\alertNoH}[2]{\\alert<handout:0|#1>{#2}}\n"
  << "\\newcommand{\\fcAnswer}[2]{"
  << "\\uncover<handout:0|\\the\\numexpr#1- 1\\relax>{"
  << "\\alertNoH{\\the\\numexpr#1- 1\\relax}{\\textbf{?}}}"
  << "{\\uncover<#1->{\\alertNoH{#1}{\\!\\!\\!#2}}}}\r\n"
  << "\\begin{document} "
  << "\\begin{frame}"
  << computation.getDivisionLaTeXSlide()
  << "\\end{frame}"
  << "\\end{document}\r\n";
  return output.assignValue(
    HtmlRoutines::convertStringToHtmlString(latexOutput.str(), true), theCommands
  );
}

bool CalculatorFunctions::innerFactorPolynomialModPrime(
  Calculator& theCommands, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerFactorPolynomialModPrime");
  if (input.size() != 2 && input.size() != 3) {
    return theCommands << "Expected two arguments "
    << "(polynomial and prime) or one argument (modular polynomial).";
  }
  Expression converted;
  if (input.size() == 3) {
    if (!CalculatorConversions::innerPolynomialModuloInteger(
      theCommands, input, converted
    )) {
      return theCommands
      << "Failed to convert arguments to modular polynomial.";
    }
  } else {
    converted = input[1];
  }
  WithContext<Polynomial<ElementZmodP> > polynomial;
  if (!converted.isOfTypeWithContext(&polynomial)) {
    return theCommands << "Failed to extract modular polynomial. ";
  }
  if (polynomial.content.isEqualToZero()) {
    return theCommands << "Factoring zero not allowed. ";
  }
  LargeInteger thePrime = polynomial.content.coefficients[0].theModulus;
  if (!input[2].isInteger(&thePrime)) {
    return false;
  }
  if (thePrime < 0) {
    return theCommands << "The modulus: " << thePrime << " is not positive. ";
  }
  std::stringstream commentsOnFailure;
  if (!thePrime.value.IsPossiblyPrime(2, true, &commentsOnFailure)) {
    theCommands << "The modulus: " << thePrime
    << " appears not to be prime. " << commentsOnFailure.str();
  }
  std::stringstream out;
  PolynomialFactorization<ElementZmodP, PolynomialFactorizationCantorZassenhaus> result;
  polynomial.context.getFormat(result.format);
  result.format.flagSuppressModP = true;
  out << "Converted polynomial: \\("
  << polynomial.content.toString(&result.format) << "\\)<br>";
  if (!result.factor(polynomial.content, &out)) {
    return output.assignValue(out.str(), theCommands);
  }
  return output.assignValue(out.str(), theCommands);
}
