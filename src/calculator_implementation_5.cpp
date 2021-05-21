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
  int xStartingGridCount;
  int yStartingGridCount;
  int numBadEvaluations;
  int numGoodEvaluations;
  int maxNumTriangles;
  double height;
  double width;
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
  int cleanUpTrianglesReturnUpdatedCurrentIndex(int currentIndex);
  void computeImplicitPlot();
  void computeImplicitPlotPart2();
  void plotGrid(int theColor);
  bool evaluatesToDouble(double& whichDouble);
  double getValueAtPoint(const Vector<double>& thePoint);
  void evaluateFunAtTriangleVertices(int triangleIndex);
  double getTriangleMaxSideLength(int triangleIndex);
  void subdivide(int triangleIndex);
  void addPointFromVerticesValues(
    Vectors<double>& outputAppend,
    const Vector<double>& left,
    const Vector<double>& right,
    double leftVal,
    double rightVal
  );
  bool computePoints(Calculator& calculator, const Expression& input, bool showGrid);
  MeshTriangles();
};

MeshTriangles::MeshTriangles() {
  this->xStartingGridCount = 0;
  this->yStartingGridCount = 0;
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
  this->theGrid.clearPlotObjects();
  this->theGrid.setExpectedPlotObjects(this->theTriangles.size * 3);
  PlotObject currentLinePlot;
  List<Vector<double> >& pointsVector = currentLinePlot.pointsDouble;
  currentLinePlot.pointsDouble.setSize(4);
  currentLinePlot.colorRGB = theColor;
  for (int i = 0; i < this->theTriangles.size; i ++) {
    pointsVector[0] = this->theTriangles[i][0];
    pointsVector[1] = this->theTriangles[i][1];
    pointsVector[2] = this->theTriangles[i][2];
    pointsVector[3] = this->theTriangles[i][0];
    this->theGrid.addPlotOnTop(currentLinePlot);
  }
}

double MeshTriangles::getValueAtPoint(const Vector<double>& thePoint) {
  int index = this->theEvaluatedPoints.getIndex(thePoint);
  if (index != - 1) {
    return this->theEvaluatedPoints.values[index];
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

void MeshTriangles::evaluateFunAtTriangleVertices(int triangleIndex) {
  MacroRegisterFunctionWithName("MeshTriangles::EvaluateFunAtTriangleVertices");
  for (int j = 0; j < this->theTriangles[triangleIndex].size; j ++) {
    this->getValueAtPoint(this->theTriangles[triangleIndex][j]);
  }
}

double MeshTriangles::getTriangleMaxSideLength(int triangleIndex) {
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

void MeshTriangles::addPointFromVerticesValues(
  Vectors<double>& outputAppend,
  const Vector<double>& left,
  const Vector<double>& right,
  double leftVal,
  double rightVal
) {
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

int MeshTriangles::cleanUpTrianglesReturnUpdatedCurrentIndex(int currentIndex) {
  MacroRegisterFunctionWithName("MeshTriangles::cleanUpTrianglesReturnUpdatedCurrentIndex");
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

void MeshTriangles::computeImplicitPlotPart2() {
  MacroRegisterFunctionWithName("MeshTriangles::ComputeImplicitPlotPart2");
  this->theTriangles.setExpectedSize(this->maxNumTriangles * 2);
  this->trianglesUsed.setExpectedSize(this->maxNumTriangles * 2);
  this->trianglesUsed.initializeFillInObject(this->theTriangles.size, true);
  this->theEvaluatedPoints.setExpectedSize(this->maxNumTriangles * 4);
  this->flagTriangleLimitReached = false;
  for (int i = 0; i < this->theTriangles.size; i ++) {
    this->evaluateFunAtTriangleVertices(i);
  }
  if (this->numGoodEvaluations < 5) {
    this->flagFunctionEvaluationFailed = true;
    return;
  }
  double minSide = MathRoutines::minimum(this->height, this->width) * this->minTriangleSideAsPercentOfWidthPlusHeight;
  PlotObject currentPlot;
  currentPlot.colorRGB = static_cast<int>(HtmlRoutines::redGreenBlue(255, 0, 0));
  Vectors<double>& theSegment = currentPlot.pointsDouble;
  List<Vector<double> > currentTriangle;
  for (int i = 0; i < this->theTriangles.size; i ++) {
    currentTriangle = this->theTriangles[i]; //making a copy in case this->theTriangles changes underneath.
    if (currentTriangle.size != 3) {
      global.fatal << "Error: triangle needs three vertices, instead it has vertices: " << currentTriangle << global.fatal;
    }
    bool isGood = true;
    for (int j = 0; j < currentTriangle.size; j ++) {
      if (this->getValueAtPoint(currentTriangle[j]) == std::nan("")) {
        isGood = false;
        break;
      }
    }
    if (!isGood) {
      this->trianglesUsed[i] = false;
      continue;
    }
    double val0 = this->getValueAtPoint(currentTriangle[0]);
    double val1 = this->getValueAtPoint(currentTriangle[1]);
    double val2 = this->getValueAtPoint(currentTriangle[2]);
    double prod01 = val0 * val1;
    double prod12 = val1 * val2;
    double prod20 = val2 * val0;
    if (prod01 > 0 && prod12 > 0 && prod20 > 0) {
      this->trianglesUsed[i] = false;
      continue;
    }
    i = this->cleanUpTrianglesReturnUpdatedCurrentIndex(i);
    if (this->getTriangleMaxSideLength(i) > minSide && !this->flagTriangleLimitReached) {
      this->subdivide(i);
      continue;
    }
    theSegment.setSize(0);
    if (prod01 <= 0) {
      this->addPointFromVerticesValues(theSegment, currentTriangle[0], currentTriangle[1], val0, val1);
    }
    if (prod12 <= 0) {
      this->addPointFromVerticesValues(theSegment, currentTriangle[1], currentTriangle[2], val1, val2);
    }
    if (prod20 <= 0 && theSegment.size < 2) {
      this->addPointFromVerticesValues(theSegment, currentTriangle[2], currentTriangle[0], val2, val0);
    }
    if (theSegment.size != 2) {
      continue;
    }
    this->theCurve.addPlotOnTop(currentPlot);
  }
}

void MeshTriangles::computeImplicitPlot() {
  MacroRegisterFunctionWithName("MeshTriangles::ComputeImplicitPlot");
  if (this->xStartingGridCount == 0 || this->yStartingGridCount == 0) {
    return;
  }
  this->width = this->upperRightCorner[0] - this->lowerLeftCorner[0];
  this->height = this->upperRightCorner[1] - this->lowerLeftCorner[1];
  double deltaX = (this->width) / this->xStartingGridCount;
  double deltaY = (this->height) / this->yStartingGridCount;
  Vector<double> currentPoint;
  currentPoint.initializeFillInObject(2, 0);
  List<Vector<double> > currentTriangle;
  currentTriangle.initializeFillInObject(3, currentPoint);
  for (int i = 0; i < this->xStartingGridCount; i ++) {
    for (int j = 0; j < this->yStartingGridCount; j ++) {
      currentTriangle[0][0] = this->lowerLeftCorner[0] + deltaX * i;
      currentTriangle[0][1] = this->lowerLeftCorner[1] + deltaY * j;
      currentTriangle[1] = currentTriangle[0];
      currentTriangle[1][0] += deltaX;
      currentTriangle[2] = currentTriangle[1];
      currentTriangle[2][1] += deltaY;
      this->theTriangles.addOnTop(currentTriangle);
      currentTriangle[1] = currentTriangle[0];
      currentTriangle[1][1] += deltaY;
      this->theTriangles.addOnTop(currentTriangle);
    }
  }
  if (this->flagShowGrid) {
    this->plotGrid(static_cast<int>(HtmlRoutines::redGreenBlue(240, 240, 0)));
    this->thePlot.addPlotsOnTop(this->theGrid);
  }
  this->computeImplicitPlotPart2();
  if (this->flagShowGrid) {
    this->plotGrid(static_cast<int>(HtmlRoutines::redGreenBlue(100, 100, 100)));
    this->thePlot.addPlotsOnTop(this->theGrid);
  }
  // this->theCurve.colorRGB=HtmlRoutines::redGreenBlue(255,0,0);
  this->thePlot.addPlotsOnTop(this->theCurve);
}

bool Calculator::getMatrixDoubles(
  const Expression& input, Matrix<double>& output, int desiredNumberOfColumns
) {
  return this->functionGetMatrix<double>(
    input,
    output,
    nullptr,
    desiredNumberOfColumns,
    CalculatorFunctions::functionEvaluateToDouble
  );
}

bool Calculator::getVectorDoubles(
  const Expression& input,
  Vector<double>& output,
  int desiredDimensionNonMandatory
) {
  return this->getVector(
    input,
    output,
    nullptr,
    desiredDimensionNonMandatory,
    CalculatorFunctions::functionEvaluateToDouble
  );
}

bool Calculator::getVectorDoublesFromFunctionArguments(
  const Expression& input, Vector<double>& output, int desiredDimensionNonMandatory
) {
  return this->getVectorFromFunctionArguments(
    input,
    output,
    nullptr,
    desiredDimensionNonMandatory,
    CalculatorFunctions::functionEvaluateToDouble
  );
}

bool CalculatorFunctions::innerGetPointsImplicitly(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerGetPointsImplicitly");
  MeshTriangles theMesh;
  if (!theMesh.computePoints(calculator, input, false)) {
    return false;
  }
  HashedList<Vector<double>, MathRoutines::hashVectorDoubles> thePoints;
  List<PlotObject>& plots = theMesh.theCurve.getPlots();
  for (int i = 0; i < plots.size; i ++) {
    thePoints.addOnTopNoRepetition(plots[i].pointsDouble[0]);
    thePoints.addOnTopNoRepetition(plots[i].pointsDouble[1]);
  }
  Matrix<double> matrix;
  matrix.assignVectorsToRows(thePoints);
  return output.assignMatrix(matrix, calculator);
}

bool CalculatorFunctionsPlot::plotImplicitFunction(
  Calculator& calculator, const Expression& input, Expression& output
) {
  return CalculatorFunctionsPlot::plotImplicitFunctionFull(calculator, input, output, false);
}

bool CalculatorFunctionsPlot::plotImplicitShowGridFunction(
  Calculator& calculator, const Expression& input, Expression& output
) {
  return CalculatorFunctionsPlot::plotImplicitFunctionFull(calculator, input, output, true);
}

bool MeshTriangles::computePoints(
  Calculator& calculator, const Expression& input, bool showGrid
) {
  MacroRegisterFunctionWithName("MeshTriangles::ComputePoints");
  if (input.size() < 5) {
    return false;
  }
  this->thePlot.dimension = 2;
  this->flagShowGrid = showGrid;
  this->owner = &calculator;
  this->theFun = input[1];
  this->knownEs = calculator.knownDoubleConstants;
  this->knownValues = calculator.knownDoubleConstantValues;
  this->knownValues.addOnTop(0);
  this->knownValues.addOnTop(0);
  HashedList<Expression> theFreeVars;
  if (!this->theFun.getFreeVariables(theFreeVars, true)) {
    return calculator << "Failed to extract free variables from: " << this->theFun.toString();
  }
  theFreeVars.quickSortAscending();
  if (theFreeVars.size > 2) {
    return calculator << "I got that your curve depends on " << theFreeVars.size << " expressions, namely: "
    << theFreeVars.toStringCommaDelimited()
    << " and that is too many (2 max). ";
  }
  Expression tempE;
  if (theFreeVars.size == 0) {
    tempE.makeAtom("x", calculator);
    theFreeVars.addOnTop(tempE);
  }
  if (theFreeVars.size == 1) {
    tempE.makeAtom("y", calculator);
    if (theFreeVars[0] == tempE) {
      tempE.makeAtom("x", calculator);
    }
    theFreeVars.addOnTop(tempE);
  }
  this->knownEs.addOnTopNoRepetitionMustBeNew(theFreeVars[0]);
  this->knownEs.addOnTopNoRepetitionMustBeNew(theFreeVars[1]);
  if (!calculator.getVectorDoubles(input[2], this->lowerLeftCorner)) {
    return calculator << "Failed to extract lower left corner from: " << input[2].toString();
  }
  if (!calculator.getVectorDoubles(input[3], this->upperRightCorner)) {
    return calculator << "Failed to extract upper right corner from: " << input[3].toString();
  }
  List<int> theGridCount;
  if (!calculator.getVectorInt(input[4], theGridCount)) {
    return calculator << "Failed to extract pair of small integers from: " << input[4].toString();
  }
  if (theGridCount.size != 2) {
    return calculator << "Failed to extract pair of small integers from: " << input[4].toString();
  }
  this->xStartingGridCount = theGridCount[0];
  this->yStartingGridCount = theGridCount[1];
  if (input.size() >= 6) {
    if (!input[5].isSmallInteger(&this->maxNumTriangles)) {
      return calculator << "Failed to extract small integer from: " << input[5].toString();
    }
    if (this->maxNumTriangles > 20000) {
      this->maxNumTriangles = 20000;
      calculator << "Max number of triangles decreased from your input: " << input[5].toString()
      << " to 20000. If you'd like to lift the restriction, modify code around: file: " << __FILE__
      << " line: " << __LINE__ << ". ";
    }
  }
  this->computeImplicitPlot();
  calculator << "Evaluated function in: "
  << this->numGoodEvaluations << " points and failed to evaluate it at: "
  << this->numBadEvaluations << " points. ";
  return true;
}

bool CalculatorFunctionsPlot::plotImplicitFunctionFull(
  Calculator& calculator, const Expression& input, Expression& output, bool showGrid
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::plotImplicitFunctionFull");
  MeshTriangles theMesh;
  if (!theMesh.computePoints(calculator, input, showGrid)) {
    return false;
  }
  return output.assignValue(theMesh.thePlot, calculator);
}

bool CalculatorConversions::functionMatrixDouble(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorConversions::functionMatrixDouble");
  Matrix<double> matrix;
  if (!calculator.functionGetMatrix(
    input,
    matrix,
    nullptr,
    0,
    CalculatorFunctions::functionEvaluateToDouble
  )) {
    return calculator << "<br>Failed to get matrix of doubles. ";
  }
  return output.assignMatrix(matrix, calculator);
}

bool CalculatorFunctionsIntegration::integratePullConstant(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsIntegration::integratePullConstant");
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
  theNewIntegralE.makeIntegral(calculator, theSetE, theFunNoCoeff, theVariableE);
  output = theFunCoeff * theNewIntegralE;
  return true;
}

bool CalculatorFunctionsIntegration::integrateSqrtOneMinusXsquared(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsIntegration::integrateSqrtOneMinusXsquared");
  Expression theFunctionE, theVariableE, theSetE;
  if (!input.isIndefiniteIntegralFdx(&theVariableE, &theFunctionE, &theSetE)) {
    return false;
  }
  Expression theFunCoeff, theFunNoCoeff;
  theFunctionE.getCoefficientMultiplicandForm(theFunCoeff, theFunNoCoeff);
  if (!theFunNoCoeff.startsWith(calculator.opPower(), 3)) {
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
  theSQRTedCoeff.makeSqrt(calculator, a * (- 1));
  theFunCoeff *= theSQRTedCoeff;
  theRadSquared = c / a * (- 1);
  theRad.makeSqrt(calculator, theRadSquared);
  Expression rescaledArgument, arcCosPart, algSQRTPart, algPart;
  rescaledArgument = theVariableE / theRad;
  arcCosPart.makeOX(calculator, calculator.opArcCos(), rescaledArgument);
  arcCosPart *= theRadSquared / - 2;
  algSQRTPart = theRadSquared - theVariableE * theVariableE;
  algPart.makeSqrt(calculator, algSQRTPart);
  algPart *= theVariableE / 2;
  output = theFunCoeff * (arcCosPart + algPart);
  return true;
}

bool CalculatorFunctionsIntegration::integrateXpowerNePowerAx(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsIntegration::integrateXpowerNePowerAx");
  Expression theFunctionE, theVariableE, theSetE;
  if (!input.isIndefiniteIntegralFdx(&theVariableE, &theFunctionE, &theSetE)) {
    return false;
  }
  if (!theFunctionE.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  Expression exponentPartE = theFunctionE[1]; //<- note: the seemingly odd order is intentional!
  Expression polyPartE = theFunctionE[2]; //<- note: the seemingly odd order is intentional!
  Expression powerOfXE, powerOfEE;
  Expression aE, bE; //exponent is of form aX+b
  powerOfXE.assignValue(1, calculator);
  bool isGood = false;
  for (int i = 0; i < 2; i ++) {
    MathRoutines::swap(exponentPartE, polyPartE);
    if (!exponentPartE.startsWith(calculator.opPower(), 3)) {
      continue;
    }
    if (!exponentPartE[1].isOperationGiven(calculator.opE())) {
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
      if (!polyPartE.startsWith(calculator.opPower(), 3)) {
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
  remainingIntegrand.makeXOX(
    calculator, calculator.opPower(), theVariableE, powerOfXE - calculator.expressionOne()
  );
  remainingIntegrand *= exponentPartE;
  integralPart.makeIntegral(calculator, theSetE,remainingIntegrand, theVariableE);
  output = (polyPartE * exponentPartE - powerOfXE * integralPart) / aE;
  return true;
}

bool CalculatorFunctionsIntegration::integrateSqrtXsquaredMinusOne(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsIntegration::integrateSqrtXsquaredMinusOne");
  Expression theFunctionE, theVariableE, theSetE;
  if (!input.isIndefiniteIntegralFdx(&theVariableE, &theFunctionE, &theSetE)) {
    return false;
  }
  Expression theFunCoeff, theFunNoCoeff;
  theFunctionE.getCoefficientMultiplicandForm(theFunCoeff, theFunNoCoeff);
  if (!theFunNoCoeff.startsWith(calculator.opPower(), 3)) {
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
  extraCF.makeSqrt(calculator, c * (- 1));
  theFunCoeff *= extraCF;
  theVarChangeCF.makeSqrt(calculator, (a / c) * (- 1));
  theNewVarE = theVariableE * theVarChangeCF;
  theFunCoeff /= theVarChangeCF;
  Expression algSQRTPart, algPart, lnPart;
  algSQRTPart = theNewVarE * theNewVarE - calculator.expressionOne();
  algPart.makeSqrt(calculator, algSQRTPart);
  lnPart.makeOX(calculator, calculator.opLog(), theNewVarE - algPart);
  output = theFunCoeff * (algPart * theNewVarE + lnPart) / 2;
  return true;
}

bool CalculatorFunctionsIntegration::integrateDefiniteIntegral(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsIntegration::integrateDefiniteIntegral");
  Expression theFunctionE, theVariableE, theSetE;
  if (!input.isDefiniteIntegralOverIntervalFdx(&theVariableE, &theFunctionE, &theSetE)) {
    return false;
  }
  if (!theSetE.startsWith(calculator.opLimitBoundary(), 3)) {
    return false;
  }
  if (
    theSetE[1].containsAsSubExpressionNoBuiltInTypes(calculator.opInfinity()) ||
    theSetE[2].containsAsSubExpressionNoBuiltInTypes(calculator.opInfinity())
  ) {
    return false;
  }
  Expression theIndefiniteIntegral, indefiniteExpression;
  indefiniteExpression.makeAtom(calculator.opIndefiniteIndicator(), calculator);
  theIndefiniteIntegral.makeIntegral(calculator, indefiniteExpression, theFunctionE, theVariableE);
  Expression solvedIntegral;
  if (!calculator.evaluateExpression(calculator, theIndefiniteIntegral, solvedIntegral)) {
    return false;
  }
  if (solvedIntegral.containsAsSubExpressionNoBuiltInTypes(calculator.opIntegral())) {
    return false;
  }
  if (solvedIntegral.containsAsSubExpressionNoBuiltInTypes(calculator.opDivide())) {
    return false;
  }
  if (solvedIntegral.containsAsSubExpressionNoBuiltInTypes(calculator.opLog())) {
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
  Expression substitutionTop(calculator), substitutionBottom(calculator);
  substitutionTop.addChildAtomOnTop(calculator.opDefine());
  substitutionTop.addChildOnTop(theVariableE);
  substitutionBottom = substitutionTop;
  substitutionBottom.addChildOnTop(theSetE[1]);
  substitutionTop.addChildOnTop(theSetE[2]);
  Expression theTopCommands(calculator), theBottomCommands(calculator);
  theTopCommands.addChildAtomOnTop(calculator.opCommandSequence());
  theBottomCommands.addChildAtomOnTop(calculator.opCommandSequence());

  theTopCommands.addChildOnTop(substitutionTop);
  theBottomCommands.addChildOnTop(substitutionBottom);

  theTopCommands.addChildOnTop(solvedIntegral);
  theBottomCommands.addChildOnTop(solvedIntegral);
  Expression theTop, theBottom;
  theTop.makeXOX(calculator, calculator.opUnderscore(), theTopCommands, calculator.expressionTwo());
  theBottom.makeXOX(calculator, calculator.opUnderscore(), theBottomCommands, calculator.expressionTwo());
  output = theTop - theBottom;
  return true;
}

bool CalculatorFunctions::innerApplyToSubexpressionsRecurseThroughCalculusFunctions(
  Calculator& calculator, const Expression& input, Expression& output
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
    theArg.startsWith(calculator.opPlus()) ||
    theArg.startsWith(calculator.opMinus()) ||
    theArg.startsWith(calculator.opTimes()) ||
    theArg.startsWith(calculator.opPower()) ||
    theArg.startsWith(calculator.opDivide()) ||
    theArg.startsWith(calculator.opSequence()) ||
    theArg.startsWith(calculator.opSqrt()) ||
    theArg.startsWith(calculator.opCos()) ||
    theArg.startsWith(calculator.opSin())
  ) {
    output.reset(calculator);
    output.addChildOnTop(input[1]);
    Expression recursivelyModifiedExpression(calculator);
    recursivelyModifiedExpression.addChildOnTop(theArg[0]);
    for (int i = 1; i < theArg.size(); i ++) {
      Expression nextE(calculator);
      nextE.addChildAtomOnTop("ApplyToSubexpressionsRecurseThroughCalculusFunctions");
      nextE.addChildOnTop(input[1]);
      nextE.addChildOnTop(theArg[i]);
      recursivelyModifiedExpression.addChildOnTop(nextE);
    }
    return output.addChildOnTop(recursivelyModifiedExpression);
  }
  output.reset(calculator);
  output.addChildOnTop(input[1]);
  output.addChildOnTop(theArg);
  return true;
}

bool CalculatorFunctions::innerNumerator(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerNumerator");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Rational rational;
  if (argument.isRational(&rational)) {
    return output.assignValue(Rational(rational.getNumerator()), calculator);
  }
  if (argument.startsWith(calculator.opDivide())) {
    if (argument.size() > 1) {
      output = argument[1];
      return true;
    }
  }
  output = argument;
  return true;
}

bool CalculatorFunctions::innerDenominator(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerDenominator");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Rational rational, theDen;
  if (argument.isRational(&rational)) {
    theDen = rational.getDenominator();
    return output.assignValue(theDen, calculator);
  }
  if (argument.startsWith(calculator.opDivide())) {
    if (argument.size() > 2) {
      output = argument[2];
      return true;
    }
  }
  return output.assignValue(1, calculator);
}

bool CalculatorFunctions::innerHandleUnderscorePowerLimits(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerHandleUnderscorePowerLimits");
  if (
    !input.startsWith(calculator.opPower(), 3) &&
    !input.startsWith(calculator.opUnderscore(), 3)
  ) {
    return false;
  }
  if (!input[1].startsWith(calculator.opLimitBoundary())) {
    return false;
  }
  output = input[1];
  for (int i = output.size(); i < 3; i ++) {
    output.addChildAtomOnTop(calculator.opIndefiniteIndicator());
  }
  if (input.startsWith(calculator.opPower())) {
    return output.setChild(2, input[2]);
  } else {
    return output.setChild(1, input[2]);
  }
}

bool CalculatorFunctions::innerSumAsOperatorToSumInternalNotation(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSumAsOperatorToSumInternalNotation");
  if (input.size() <= 1) {
    return false;
  }
  if (!input[0].startsWith(calculator.opSum())) {
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
  argumentE.makeSequence(calculator, &theRemaining);
  return output.addChildOnTop(argumentE);
}

bool CalculatorFunctions::sumTimesExpressionToSumOf(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSumTimesExpressionToSumOf");
  if (
    !input.startsWith(calculator.opTimes(), 3) &&
    !input.startsWith(calculator.opDivide(), 3)
  ) {
    return false;
  }
  if (
    input[1].startsWith(calculator.opSum(), 2) &&
    !input.startsWith(calculator.opDivide())
  ) {
    if (!input[1][1].startsWith(calculator.opLimitBoundary())) {
      return false;
    }
    output = input[1];
    return output.addChildOnTop(input[2]);
  }
  if (input[1].startsWith(calculator.opSum(), 3)) {
    if (!input[1][1].startsWith(calculator.opLimitBoundary())) {
      return false;
    }
    Expression theSummed(calculator);
    theSummed.addChildOnTop(input[0]);
    theSummed.addChildOnTop(input[1][2]);
    theSummed.addChildOnTop(input[2]);
    output = input[1];
    return output.setChild(2,theSummed);
  }
  return false;
}

bool CalculatorFunctions::innerSumSequence(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSumSequence");
  if (input.size() < 1) {
    return false;
  }
  if (input.size() == 1) {
    return output.assignValue(0, calculator);
  }
  if (input[1].startsWith(calculator.opLimitBoundary())) {
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
  return output.makeSum(calculator, theTerms);
}

bool CalculatorFunctions::innerMultiplySequence(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerMultiplySequence");
  if (input.size() < 1) {
    return false;
  }
  if (input.size() == 2 && input[1].isSequenceNElements()) {
    return CalculatorFunctions::innerMultiplySequence(calculator, input[1], output);
  }
  List<Expression> terms;
  for (int i = 1; i < input.size(); i ++) {
    terms.addOnTop(input[i]);
  }
  return output.makeProduct(calculator, terms);
}

bool CalculatorFunctions::innerEnsureExpressionDependsOnlyOnStandard(
  Calculator& calculator, const Expression& input, Expression& output
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
    return output.assignValue(out.str(), calculator);
  }
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctions::innerRemoveDuplicates(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerRemoveDuplicates");
  int operationRemoveDuplicated = calculator.operations.getIndexNoFail("RemoveDuplicates");
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
  return output.makeSequence(calculator, &result);
}

bool CalculatorFunctions::innerSort(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSort");
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
  return output.makeSequence(calculator, &sortedExpressions);
}

bool CalculatorFunctions::innerSortDescending(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSortDescending");
  if (
    !input.isListStartingWithAtom(calculator.operations.getIndexNoFail("SortDescending")) &&
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
  return output.makeSequence(calculator, &sortedExpressions);
}

bool CalculatorFunctionsListsAndSets::length(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsListsAndSets::length");
  if (!input.isListStartingWithAtom(
    calculator.operations.getIndexNoFail("Length")
  )) {
    return false;
  }
  if (input.size() == 2) {
    if (input[1].isSequenceNElements()) {
      return output.assignValue(input[1].size() - 1, calculator);
    }
  }
  return output.assignValue(input.size() - 1, calculator);
}

bool CalculatorFunctions::innerEnsureExpressionDependsOnlyOnMandatoryVariables(
  Calculator& calculator, const Expression& input, Expression& output, bool excludeNamedConstants
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
  allowedFreeVars.addListOnTop(mandatoryFreeVars);
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
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctionsPlot::plotGrid(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::plotGrid");
  (void) input;
  PlotObject thePlot;
  thePlot.plotType = "axesGrid";
  thePlot.dimension = 2;
  return output.assignValue(thePlot, calculator);
}

bool CalculatorFunctionsPlot::plotRemoveCoordinateAxes(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::plotRemoveCoordinateAxes");
  (void) input;
  Plot thePlotFinal;
  thePlotFinal.dimension = 2;
  thePlotFinal.flagIncludeCoordinateSystem = false;
  return output.assignValue(thePlotFinal, calculator);
}

bool CalculatorFunctionsPlot::plotLabel(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::plotLabel");
  if (input.size() != 3) {
    return false;
  }
  Vector<double> labelPosition;
  if (!calculator.getVectorDoubles(input[1], labelPosition, - 1)) {
    return false;
  }
  std::string label;
  if (!input[2].isOfType<std::string>(&label)) {
    label = input[2].toString();
  }
  PlotObject plot;
  plot.dimension = labelPosition.size;
  plot.plotString = label;
  plot.pointsDouble.addOnTop(labelPosition);
  plot.plotType = "label";
  plot.colorJS = "black";
  return output.assignValue(plot, calculator);
}

bool CalculatorFunctionsPlot::plotRectangle(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::plotRectangle");
  if (input.size() != 3) {
    return false;
  }
  Vectors<double> theRectangle;
  theRectangle.setSize(2);
  if (
    !calculator.getVectorDoubles(input[1], theRectangle[0], 2) ||
    !calculator.getVectorDoubles(input[2], theRectangle[1], 2)
  ) {
    return false;
  }
  PlotObject thePlot;
  thePlot.dimension = 2;
  thePlot.plotType = "pathFilled";
  Vector<double> currentCorner = theRectangle[0];
  Vector<double>& dimensions = theRectangle[1];

  thePlot.pointsDouble.addOnTop(currentCorner);
  currentCorner[0] += dimensions[0];
  thePlot.pointsDouble.addOnTop(currentCorner);
  currentCorner[1] += dimensions[1];
  thePlot.pointsDouble.addOnTop(currentCorner);
  currentCorner[0] -= dimensions[0];
  thePlot.pointsDouble.addOnTop(currentCorner);
  currentCorner[1] -= dimensions[1];
  thePlot.pointsDouble.addOnTop(currentCorner);
  thePlot.colorFillJS = "cyan";
  thePlot.colorJS = "blue";
  thePlot.pointsDouble.addOnTop(currentCorner);
  thePlot.theRectangles.addOnTop(theRectangle);
  thePlot.colorRGB = static_cast<int>(HtmlRoutines::redGreenBlue(0, 0, 255));
  thePlot.colorFillRGB = static_cast<int>(HtmlRoutines::redGreenBlue(0, 255, 255));
  return output.assignValue(thePlot, calculator);
}

bool CalculatorFunctions::innerSolveUnivariatePolynomialWithRadicalsWRT(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerSolveUnivariatePolynomialWithRadicalsWRT");
  if (input.size() != 3) {
    return calculator << "SolveFor takes as input three arguments. ";
  }
  if (input[1].hasBoundVariables()) {
    return false;
  }
  if (input[2].hasBoundVariables()) {
    return false;
  }
  Expression powers;
  Expression modifiedInput = input;
  if (!modifiedInput[2].startsWith(calculator.opDefine())) {
    if (!CalculatorFunctions::innerCoefficientsPowersOf(calculator, modifiedInput, powers)) {
      return calculator << "Failed to extract the coefficients of " << modifiedInput[1].toString()
      << " in " << modifiedInput[2].toString();
    }
  } else {
    Expression convertedEqualityE, convertedSimplifiedEqualityE;
    if (!CalculatorFunctions::functionEqualityToArithmeticExpression(calculator, modifiedInput[2], convertedEqualityE)) {
      return calculator << "Failed to interpret the equality " << modifiedInput[2].toString();
    }
    if (!Calculator::evaluateExpression(calculator, convertedEqualityE, convertedSimplifiedEqualityE)) {
      return calculator << "Failed to simplify: " << convertedEqualityE.toString();
    }
    modifiedInput.setChild(2, convertedSimplifiedEqualityE);
    if (!CalculatorFunctions::innerCoefficientsPowersOf(calculator, modifiedInput, powers)) {
      return calculator << "Failed to extract the coefficients of " << modifiedInput[1].toString()
      << " in " << modifiedInput[2].toString() << " which was obtained from the equality "
      << input[2].toString();
    }
  }
  if (!powers.isSequenceNElements()) {
    return calculator << "This is not supposed to happen: expression "
    << powers.toString() << " should be a list. This may be a programming bug. ";
  }
  if (powers.size() == 2) {
    return calculator << "Cannot solve: " << modifiedInput[2].toString()
    << ". The expression does not depend on " << modifiedInput[1].toString()
    << ". The coefficients of "
    << modifiedInput[1].toString() << " are: " << powers.toString();
  }
  if (powers.size() == 3) {
    output = powers[1];
    output *= - 1;
    output /= powers[2];
    return true;
  }
  if (powers.size() == 4) {
    const Expression& a = powers[3];
    const Expression& b = powers[2];
    const Expression& c = powers[1];
    output.makeSequence(calculator);
    Expression currentRoot;
    Expression theDiscriminant;
    theDiscriminant = b * b - a * c * 4;
    Expression sqrtDiscriminant;
    sqrtDiscriminant.makeSqrt(calculator, theDiscriminant, 2);
    currentRoot = (b * (- 1) - sqrtDiscriminant) / (a * 2);
    output.addChildOnTop(currentRoot);
    currentRoot = (b * (- 1) + sqrtDiscriminant) / (a * 2);
    output.addChildOnTop(currentRoot);
    return true;
  }
  Polynomial<Rational> polynomial;
  for (int i = powers.size() - 1; i >= 1; i --) {
    MonomialPolynomial oneVariable(0, 1);
    polynomial *= oneVariable;
    Rational coefficient;
    if (!powers[i].isRational(&coefficient)) {
      return false;
    }
    polynomial += coefficient;
  }
  List<AlgebraicNumber> solutions;
  if (PolynomialFactorizationKronecker::solvePolynomial(
    polynomial,
    solutions,
    calculator.objectContainer.algebraicClosure,
    &calculator.comments
  )) {
    output.makeSequence(calculator);
    for (int i = 0; i < solutions.size; i ++) {
      output.addChildValueOnTop(solutions[i]);
    }
    return true;
  }
  return false;
}

bool CalculatorFunctions::innerOperatorBounds(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPowerIntToAny");
  if (
    !input.startsWith(calculator.opUnderscore(), 3) &&
    !input.startsWith(calculator.opPower(), 3)
  ) {
    return false;
  }
  const Expression& baseE = input[1];
  Expression theLimitsE;
  int theIntegralOp = baseE.data;
  if (
    theIntegralOp == calculator.opIntegral() || theIntegralOp == calculator.opSum()
  ) {
    output.reset(calculator);
    output.addChildAtomOnTop(theIntegralOp);
    theLimitsE.reset(calculator);
    theLimitsE.addChildAtomOnTop(calculator.opLimitBoundary());
    theLimitsE.addChildOnTop(input[2]);
    theLimitsE.addChildAtomOnTop(calculator.opIndefiniteIndicator());
    output.addChildOnTop(theLimitsE);
    return true;
  }
  if (
    !baseE.startsWith(calculator.opIntegral(), 2) &&
    !baseE.startsWith(calculator.opSum(), 2)
  ) {
    return false;
  }
  theLimitsE.reset(calculator);
  theLimitsE.addChildAtomOnTop(calculator.opLimitBoundary());
  for (int i = 1; i < 3; i ++) {
    if (i < baseE[1].size()) {
      theLimitsE.addChildOnTop(baseE[1][i]);
    } else {
      theLimitsE.addChildAtomOnTop(calculator.opIndefiniteIndicator());
    }
  }
  if (input[1].isOperationGiven(calculator.opUnderscore())) {
    theLimitsE.setChild(1, input[2]);
  } else {
    theLimitsE.setChild(2, input[2]);
  }
  output = input[1];
  return output.setChild(1, theLimitsE);
}

bool CalculatorFunctions::innerPowerExponentToLog(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerPowerExponentToLog");
  if (!input.startsWith(calculator.opPower(), 3)) {
    return false;
  }
  const Expression& baseE = input[1];
  const Expression& powerE = input[2];
  if (baseE.isOperationGiven(calculator.opE())) {
    if (powerE.startsWith(calculator.opLog(), 2)) {
      output = powerE[1];
      return true;
    }
  }
  return false;
}

bool CalculatorFunctions::innerDistributeExponent(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("Calculator::innerDistributeExponent");
  if (!input.startsWith(calculator.opPower(), 3)) {
    return false;
  }
  const Expression& base = input[1];
  const Expression& exponentE = input[2];
  if (exponentE.isOperationGiven(calculator.opCirc())) {
    return false;
  }
  if (!input[1].startsWith(calculator.opTimes(), 3)) {
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
        if (!exponentRat.getDenominator().isEven()) {
          isGood = true;
        }
      }
    }
  }
  if (!isGood) {
    return false;
  }
  Expression leftE, rightE;
  leftE.makeXOX(calculator, calculator.opPower(), input[1][1], input[2]);
  rightE.makeXOX(calculator, calculator.opPower(), input[1][2], input[2]);
  return output.makeXOX(calculator, calculator.opTimes(), leftE, rightE);
}

bool CalculatorFunctions::innerSqrt(
  Calculator& calculator, const Expression& input, Expression& output
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
      theExponentE.assignValue(ratPower, calculator);
      powerE.makeXOX(calculator,calculator.opPower(), input[2], theExponentE);
      if (CalculatorFunctionsBinaryOps::innerPowerRationalByRationalReducePrimeFactors(
        calculator, powerE, powerEreduced
      )) {
        if (powerEreduced != powerE && powerEreduced != input) {
          output = powerEreduced;
          return true;
        }
      }
    }
  }
  if (input[2].isEqualToOne()) {
    return output.assignValue(1, calculator);
  }
  int power = 0;
  if (!input[1].isSmallInteger(&power)) {
    return false;
  }
  if (!input[2].isRational() ) {
    calculator.checkInputNotSameAsOutput(input, output);
    Expression theExponent;
    Rational powerRational(1, power);
    theExponent.assignValue(powerRational, calculator);
    return output.makeXOX(calculator, calculator.opPower(), input[2], theExponent);
  }
  if (power > 0 && input[2].isEqualToZero()) {
    return output.assignValue(0, calculator);
  }
  if (power == 0 && input[2].isEqualToZero()) {
    return output.assignValue(1, calculator);
  }
  Rational rationalValue;
  if (!input[2].isRational(&rationalValue)) {
    return false;
  }
  if (power < 0) {
    if (rationalValue.isEqualToZero()) {
      return output.makeError("Division by zero in expression: " + input.toString(), calculator);
    }
    power *= - 1;
    rationalValue.invert();
  }
  if (power != 2) {
    return false;
  }
  AlgebraicNumber theNumber;
  if (!theNumber.assignRationalQuadraticRadical(
    rationalValue,
    calculator.objectContainer.algebraicClosure,
    &calculator.comments
  )) {
    return false;
  }
  return output.assignValue(theNumber, calculator);
}

bool CalculatorFunctionsBasic::floor(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBasic::floor");
  if (input.size() != 2) {
    return false;
  }
  Rational rational;
  if (input[1].isOfType<Rational>(&rational)) {
    rational.assignFloor();
    return output.assignValue(rational, calculator);
  }
  double theDouble = 0;
  if (input[1].evaluatesToDouble(&theDouble)) {
    return output.assignValue(static_cast<int>(std::floor(theDouble)), calculator);
  }
  return false;
}

bool CalculatorFunctionsBasic::logarithmBaseNCeiling(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBasic::logarithmBaseNCeiling");
  if (input.size() != 3) {
    return calculator << "Logarithm ceiling function expects exactly two arguments. ";
  }
  int smallInt = 0;
  if (!input[1].isSmallInteger(&smallInt)) {
    return calculator << "First argument expected to be a small integer. ";
  }
  if (smallInt <= 1) {
    return calculator << "First argument must be larger than one. ";
  }
  LargeIntegerUnsigned argument;
  if (!input[2].isIntegerNonNegative(&argument)) {
    return calculator << "Failed to extract positive intger from second argument. ";
  }
  int result = static_cast<int>(
    argument.logarithmBaseNCeiling(static_cast<unsigned>(smallInt))
  );
  return output.assignValue(result, calculator);
}

bool CalculatorFunctionsBasic::round(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsBasic::round");
  if (input.size() != 2) {
    return false;
  }
  Rational rational;
  if (input[1].isOfType<Rational>(&rational)) {
    Rational result = rational;
    result.assignFloor();
    if (rational - result >= Rational(1, 2)) {
      result ++;
    }
    return output.assignValue(result, calculator);
  }
  double theDouble = 0;
  if (input[1].evaluatesToDouble(&theDouble)) {
    return output.assignValue(
      static_cast<int>(std::round(theDouble)), calculator
    );
  }
  return false;
}

bool CalculatorFunctionsPlot::plotPath(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::plotPath");
  if (input.size() < 3) {
    return false;
  }
  const Expression& matrixE = input[1];
  Matrix<double> matrix;
  if (!calculator.getMatrixDoubles(matrixE, matrix)) {
    return calculator << "Failed to extract matrix from: " << matrixE.toString();
  }
  if (matrix.numberOfColumns != 2 && matrix.numberOfColumns != 3) {
    return calculator << "Only dimensions 2 and 3 are supported at the moment. ";
  }
  PlotObject segment;
  if (input.size() >= 3) {
    segment.colorJS = "black";
    segment.colorRGB = static_cast<int>(HtmlRoutines::redGreenBlue(0, 0, 0));
    const Expression& colorE = input[2];
    if (!colorE.isOfType<std::string>(&segment.colorJS)) {
      segment.colorJS = colorE.toString();
    }
    if (!DrawingVariables::getColorIntFromColorString(
      segment.colorJS, segment.colorRGB
    )) {
      calculator << "Unrecognized color: " << segment.colorJS;
    }
  }
  if (input.size() >= 4) {
    const Expression& lineWidthE = input[3];
    if (!lineWidthE.evaluatesToDouble(&segment.lineWidth)) {
      calculator << "Failed to extract line width from: " << lineWidthE.toString();
    }
    std::stringstream lineWidthStream;
    lineWidthStream.precision(4);
    lineWidthStream << segment.lineWidth;
    segment.lineWidthJS = lineWidthStream.str();
  }
  segment.plotType = PlotObject::Labels::path;
  segment.dimension = matrix.numberOfColumns;
  matrix.getVectorsFromRows(segment.pointsDouble);
  if (input.size() >= 4) {
    if (!input[3].evaluatesToDouble(&segment.lineWidth)) {
      segment.lineWidth = 1;
    }
  }
  Plot thePlot;
  thePlot += segment;
  return output.assignValue(thePlot, calculator);
}

bool CalculatorFunctionsPlot::plotMarkSegment(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsPlot::plotMarkSegment");
  if (input.size() < 3) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  Vector<double> leftV, rightV;
  if (!calculator.getVectorDoubles(leftE, leftV) || !calculator.getVectorDoubles(rightE, rightV)) {
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
      return calculator << "Could not extract small integer from " << input[3].toString();
    }
  }
  if (numSegments < 1 || numSegments > 100) {
    return calculator << "Bad number of segments: " << numSegments;
  }
  Expression theVector = (rightE - leftE);
  Expression midPt = (rightE + leftE) / 2;
  Expression theVectorX, theVectorY;
  theVectorX.makeXOX(calculator, calculator.opUnderscore(), theVector, calculator.expressionOne());
  theVectorY.makeXOX(calculator, calculator.opUnderscore(), theVector, calculator.expressionTwo());
  Expression theOrthoV;
  theOrthoV.makeXOX(calculator, calculator.opSequence(), theVectorY * (- 1), theVectorX);
  Expression leftPt = midPt - theOrthoV / 25;
  Expression rightPt = midPt + theOrthoV / 25;
  output.reset(calculator);
  output.addChildAtomOnTop("PlotSegment");
  output.addChildOnTop(leftPt);
  output.addChildOnTop(rightPt);
  for (int i = 4; i < input.size(); i ++) {
    output.addChildOnTop(input[i]);
  }
  return true;
}

bool CalculatorFunctionsPlot::plotSegment(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsPlot::plotSegment");
  if (input.size() < 3) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  Vector<double> leftV, rightV;
  if (!calculator.getVectorDoubles(leftE, leftV) || !calculator.getVectorDoubles(rightE, rightV)) {
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
    theSegment.colorRGB = static_cast<int>(HtmlRoutines::redGreenBlue(0, 0, 0));
    const Expression& colorE = input[3];
    if (!colorE.isOfType<std::string>(&theSegment.colorJS)) {
      theSegment.colorJS = colorE.toString();
    }
    if (!DrawingVariables::getColorIntFromColorString(theSegment.colorJS, theSegment.colorRGB)) {
      calculator << "Unrecognized color: " << theSegment.colorJS;
    }
  }
  if (input.size() >= 5) {
    const Expression& lineWidthE = input[4];
    if (!lineWidthE.evaluatesToDouble(&theSegment.lineWidth)) {
      calculator << "Failed to extract line width from: "
      << lineWidthE.toString();
    }
    std::stringstream lineWidthStream;
    lineWidthStream.precision(4);
    lineWidthStream << theSegment.lineWidth;
    theSegment.lineWidthJS = lineWidthStream.str();
  }
  theSegment.plotType = "segment";
  if (leftV.size == 3) {
    theSegment.dimension = 3;
  } else {
    theSegment.dimension = 2;
  }
  theSegment.pointsDouble.addOnTop(leftV);
  theSegment.pointsDouble.addOnTop(rightV);
  if (input.size() >= 5) {
    if (!input[4].evaluatesToDouble(&theSegment.lineWidth)) {
      theSegment.lineWidth = 1;
    }
  }
  Plot thePlot;
  thePlot += theSegment;
  return output.assignValue(thePlot, calculator);
}

bool CalculatorFunctions::innerThaw(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::innerThaw");
  if (input.size() < 2) {
    return false;
  }
  const Expression& argument = input[1];
  if (!argument.startsWith(calculator.opFreeze())) {
    output = argument;
    return true;
  }
  if (argument.size() > 2) {
    output = argument;
    return output.setChildAtomValue(0, calculator.opSequence());
  }
  if (argument.size() == 2) {
    output = argument[1];
    return true;
  }
  return false;
}

bool CalculatorFunctions::leastCommonMultipleInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::leastCommonMultipleInteger");
  if (input.size() < 3) {
    return false;
  }
  Vector<LargeInteger> theInts;
  if (!calculator.getVectorLargeIntegerFromFunctionArguments(input, theInts)) {
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
    theResult = LargeIntegerUnsigned::leastCommonMultiple(theResult, theInts[i].value);
  }
  return output.assignValue(theResult, calculator);
}

bool CalculatorFunctions::greatestCommonDivisorInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::greatestCommonDivisorInteger");
  if (input.size() < 3) {
    return false;
  }
  Vector<LargeInteger> theInts;
  if (!calculator.getVectorLargeIntegerFromFunctionArguments(input, theInts)) {
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
    theResult = LargeIntegerUnsigned::greatestCommonDivisor(theResult, theInts[i].value);
  }
  return output.assignValue(theResult, calculator);
}

bool CalculatorFunctionsBasic::logarithmBaseNaturalToLn(Calculator& calculator, const Expression& input, Expression& output) {
  if (!input.startsWith(calculator.opLogBase(), 3)) {
    return false;
  }
  if (!input[1].isOperationGiven(calculator.opE())) {
    return false;
  }
  return output.makeOX(calculator, calculator.opLog(), input[2]);
}

bool CalculatorFunctionsBasic::logarithmBaseSimpleCases(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::logarithmBaseSimpleCases");
  if (!input.startsWith(calculator.opLogBase(), 3)) {
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
    return output.makeError(errorStream.str(), calculator);
  }
  if (theArg == 1) {
    return output.assignValue(0, calculator);
  }
  Expression newBaseE, newArgE;
  newBaseE.assignValue(theBase, calculator);
  newArgE.assignValue(theArg, calculator);
  if (theBase < 1) {
    theBase.invert();
    newBaseE.assignValue(theBase, calculator);
    output.makeXOX(calculator, calculator.opLogBase(), newBaseE, newArgE);
    output *= - 1;
    return true;
  }
  if (theArg < 1) {
    theArg.invert();
    newArgE.assignValue(theArg, calculator);
    output.makeXOX(calculator, calculator.opLogBase(), newBaseE, newArgE);
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
      newBaseE.assignValue(Rational(simplerBase), calculator);
      output.makeXOX(calculator, calculator.opLogBase(), newBaseE, newArgE);
      output /= simplerPower;
      return true;
    }
  }
  argNum = theArg.getNumerator();
  LargeInteger argDen = theArg.getDenominator();
  double doubleBase = baseInt.getDoubleValue();
  double doubleArgNum = argNum.getDoubleValue();
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
  newBaseE.assignValue(theBase, calculator);
  newArgE.assignValue(theArg, calculator);
  output.makeXOX(calculator, calculator.opLogBase(), newBaseE, newArgE);
  if (intPart == 0) {
    return true;
  }
  output += intPart;
  return true;
}

std::string InputBox::getSliderName() const {
  return StringRoutines::convertStringToJavascriptVariable(
    this->name +
    Crypto::convertStringToHex(Crypto::computeSha256(this->name), 0, false)
  );
}

std::string InputBox::getUserInputBox() const {
  MacroRegisterFunctionWithName("InputBox::getUserInputBox");
  std::stringstream out;
  double reader = 0;
  out.precision(4);
  out << std::fixed;
  if (this->value.evaluatesToDouble(&reader)) {
    out << "\\formInput"
    << "{" << reader << ", " << this->getSliderName() << "}" ;
  } else {
    out << "\\formInput" << "{" << this->value.toString()
    << ", " << this->getSliderName() << "}" ;
  }
  return out.str();
}

bool CalculatorFunctions::makeJavascriptExpression(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctions::makeJavascriptExpression");
  if (input.size() != 2) {
    return false;
  }
  return CalculatorFunctions::functionMakeJavascriptExpression(calculator, input[1], output);
}

bool CalculatorFunctions::functionMakeJavascriptExpression(
  Calculator& calculator,
  const Expression& input,
  Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctions::functionMakeJavascriptExpression");
  RecursionDepthCounter theCounter(&calculator.recursionDepth);
  if (calculator.recursionDepthExceededHandleRoughly()) {
    return false;
  }
  std::string atomString;
  if (input == calculator.expressionMinusInfinity()) {
    return output.assignValue<std::string>("\"minusInfinity\"", calculator);
  }
  if (input == calculator.expressionInfinity()) {
    return output.assignValue<std::string>("\"infinity\"", calculator);
  }
  if (input.isOperation(&atomString)) {
    if (input.isOperationGiven(calculator.opE())) {
      return output.assignValue<std::string>(" 2.718281828 ", calculator);
    }
    if (input.isOperationGiven(calculator.opPi())) {
      return output.assignValue<std::string>(" 3.141592654 ", calculator);
    }
    if (input.data >= calculator.numberOfPredefinedAtoms) {
      return output.assignValue(HtmlRoutines::getJavascriptVariable(atomString), calculator);
    }
    if (atomString == "+" || atomString == "*" || atomString == "/" || atomString == "-") {
      return output.assignValue(atomString, calculator);
    }
    return output.assignValue(atomString, calculator);
  }
  std::stringstream out;
  InputBox box;
  if (input.isOfType(&box)) {
    std::string name = box.getSliderName();
    out << name << " ";
    return output.assignValue(out.str(), calculator);
  }
  out.precision(7);
  bool hasDoubleValue = false;
  double theDoubleValue = - 1;
  if (input.isOfType<Rational>()) {
    hasDoubleValue = true;
    theDoubleValue = input.getValue<Rational>().getDoubleValue();
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
    return output.assignValue(out.str(), calculator);
  }
  Expression operation, leftE, rightE;
  if (input.startsWith(calculator.opSequence()) || input.startsWith(calculator.opIntervalOpen())) {
    out << "[";
    for (int i = 1; i < input.size(); i ++) {
      if (!CalculatorFunctions::functionMakeJavascriptExpression(calculator, input[i], operation)) {
        return output.assignValue("(Failed to convert " + input[i].toString() + ")", calculator);
      }
      out << operation.toString();
      if (i != input.size() - 1) {
        out << ", ";
      }
    }
    out << "]";
    return output.assignValue(out.str(), calculator);
  }
  std::string opString, leftString, rightString;
  std::stringstream logStream;
  if (input.size() == 3 || input.size() == 2) {
    Expression* currentE = &operation;
    std::string* currentString = &opString;
    for (int i = 0; i < input.size(); i ++) {
      if (!CalculatorFunctions::functionMakeJavascriptExpression(calculator, input[i], *currentE)) {
        return output.assignValue("(Failed to convert " + input[i].toString() + ")", calculator);
      }
      if (!currentE->isOfType(currentString)) {
        return output.assignValue("(Failed to convert " + input[i].toString() + ")", calculator);
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
        return output.assignValue(out.str(), calculator);
      }
      if (opString == "\\sqrt") {
        out << "Math.pow(" << rightString << ", 1/" << leftString << ")";
        return output.assignValue(out.str(), calculator);
      }
      if (opString == "^") {
        out << "Math.pow(" << leftString << ", " << rightString << ")";
        return output.assignValue(out.str(), calculator);
      }
      if (opString == "LogBase") {
        out << "Math.log(" << rightString << ") / Math.log(" << leftString << ")";
        return output.assignValue(out.str(), calculator);
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
        return output.assignValue(out.str(), calculator);
      }
    }
    if (input.size() == 2) {
      if (opString == "|") {
        out << "(Math.abs( " << leftString << "))";
        return output.assignValue(out.str(), calculator);
      }
    }
  }
  out << "(Failed to make expression from " << input.toString() << ". "
  << logStream.str() << ")";
  return output.assignValue(out.str(), calculator);
}

bool CalculatorFunctionsPlot::plotSetProjectionScreenBasis(
  Calculator& calculator, const Expression& input, Expression& output
) {
  MacroRegisterFunctionWithName("CalculatorFunctionsPlot::plotSetProjectionScreenBasis");
  if (input.size() != 3) {
    return false;
  }
  Vector<double> v1, v2;
  if (
    !calculator.getVectorDoubles(input[1], v1, 3) ||
    !calculator.getVectorDoubles(input[2], v2, 3)
  ) {
    return calculator << "Failed to extract 3d-vectors from "
    << input[1].toString() << ", " << input[2].toString() << ".";
  }
  Plot resultPlot;
  resultPlot.dimension = 3;
  PlotObject thePlot;
  thePlot.plotType = "setProjectionScreen";
  thePlot.pointsDouble.addOnTop(v1);
  thePlot.pointsDouble.addOnTop(v2);
  resultPlot += thePlot;
  return output.assignValue(resultPlot, calculator);
}

bool CalculatorFunctionsPlot::plotCoordinateSystem(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsPlot::plotCoordinateSystem");
  if (input.size() != 3) {
    return false;
  }
  Vector<double> corner1, corner2;
  if (
    !calculator.getVectorDoubles(input[1], corner1, 3) ||
    !calculator.getVectorDoubles(input[2], corner2, 3)
  ) {
    return calculator << "Failed to extract 3d-vectors from "
    << input[1].toString() << ", "
    << input[2].toString() << ".";
  }
  Plot resultPlot;
  resultPlot.dimension = 3;
  PlotObject thePlot;
  thePlot.colorJS = "black";
  thePlot.plotType = "segment";
  thePlot.pointsDouble.setSize(2);
  for (int i = 0; i < 3; i ++) {
    thePlot.pointsDouble[0].makeZero(3);
    thePlot.pointsDouble[1].makeZero(3);
    thePlot.pointsDouble[0][i] = corner1[i];
    thePlot.pointsDouble[1][i] = corner2[i];
    resultPlot += thePlot;
  }
  PlotObject plotLabels;
  plotLabels.plotType = "label";
  plotLabels.pointsDouble.setSize(1);
  plotLabels.colorJS = "blue";
  for (char i = 0; i < 3; i ++) {
    plotLabels.pointsDouble[0].makeZero(3);
    plotLabels.pointsDouble[0][i] = corner2[i];
    std::stringstream out;
    out << static_cast<char>('x' + i);
    plotLabels.plotString = out.str();
    resultPlot += plotLabels;
  }
  return output.assignValue(resultPlot, calculator);
}

bool CalculatorFunctionsPlot::plotSurface(Calculator& calculator, const Expression& input, Expression& output) {
  MacroRegisterFunctionWithName("CalculatorFunctionsPlot::plotSurface");
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
    return calculator << "Could not find a triple of "
    << "functions expressions to use for "
    << "the surface. ";
  }
  thePlot.manifoldImmersion.getFreeVariables(thePlot.variablesInPlay, true);
  if (thePlot.variablesInPlay.size > 2) {
    return calculator << "Got a surface with "
    << thePlot.variablesInPlay.size
    << " variables, namely: "
    << thePlot.variablesInPlay.toStringCommaDelimited()
    << ". I've been taught to plot 2d surfaces only. "
    << "Please reduce the number of variables to 2. ";
  }
  Expression uE, vE;
  uE.makeAtom("u", calculator);
  vE.makeAtom("v", calculator);
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
  thePlot.variableRangesJS.setSize(2);
  thePlot.variablesInPlayJS.setSize(2);
  for (int i = 0; i < 2; i ++) {
    thePlot.variableRangesJS[i].setSize(2);
    thePlot.variablesInPlayJS[i] = thePlot.variablesInPlay[i].toString();
  }
  Expression jsConverter;
  for (int i = 1; i < thePlot.manifoldImmersion.size(); i ++) {
    thePlot.coordinateFunctionsE[i - 1] = thePlot.manifoldImmersion[i];
    bool isGood = CalculatorFunctions::functionMakeJavascriptExpression(
      calculator, thePlot.coordinateFunctionsE[i - 1], jsConverter
    );
    if (isGood) {
      isGood = jsConverter.isOfType<std::string>(&thePlot.coordinateFunctionsJS[i - 1]);
    }
    if (!isGood) {
      return calculator << "Failed to convert "
      << thePlot.coordinateFunctionsE[i - 1].toString()
      << " to a javascript expression. ";
    }
  }
  for (int i = 1; i < input.size(); i ++) {
    if (input[i].startsWith(calculator.opIn(), 3)) {
      int index = thePlot.variablesInPlay.getIndex(input[i][1]);
      if (index < 0 || index > 2) {
        // index > 2 should never happen
        continue;
      }
      if (input[i][2].size() != 3) {
        continue;
      }
      for (int j = 0; j < 2; j ++) {
        bool isGood = CalculatorFunctions::functionMakeJavascriptExpression(
          calculator, input[i][2][j + 1], jsConverter
        );
        if (isGood) {
          isGood = jsConverter.isOfType<std::string>(&thePlot.variableRangesJS[index][j]);
        }
        if (!isGood) {
          return calculator << "Failed to convert "
          << input[i][2][j + 1].toString() << " to a javascript expression. ";
        }
      }
    }
  }
  MapList<std::string, Expression, MathRoutines::hashString> theKeys;
  if (CalculatorConversions::innerLoadKeysFromStatementList(
    calculator, input, theKeys, &calculator.comments, true
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
      if (!theKeys.contains(keysToConvert.keys[i])) {
        continue;
      }
      Expression expressionToConvert = theKeys.getValueCreate(keysToConvert.keys[i]);
      bool isGood = CalculatorFunctions::functionMakeJavascriptExpression(
        calculator, expressionToConvert, jsConverter
      );
      if (isGood) {
        isGood = jsConverter.isOfType<std::string>(&keysToConvert.values[i]);
      }
      if (!isGood) {
        return calculator << "Failed to convert "
        << expressionToConvert.toString()
        << " to a javascript expression. ";
      }
    }
    thePlot.numberOfSegmentsJS.setSize(2);
    if (keysToConvert.getValueCreate("numSegments1") != "") {
      thePlot.numberOfSegmentsJS[0] = keysToConvert.getValueCreate("numSegments1");
    }
    if (keysToConvert.getValueCreate("numSegments2") != "") {
      thePlot.numberOfSegmentsJS[1] = keysToConvert.getValueCreate("numSegments2");
    }
    if (keysToConvert.getValueCreate("lineWidth") != "") {
      thePlot.lineWidthJS = keysToConvert.getValueCreate("lineWidth");
    }
  }
  if (
    thePlot.variableRangesJS[0][0] == "" ||
    thePlot.variableRangesJS[0][1] == "" ||
    thePlot.variableRangesJS[1][0] == "" ||
    thePlot.variableRangesJS[1][1] == ""
  ) {
    return calculator << "Could not extract variable ranges, got the var ranges: "
    << thePlot.variableRangesJS;
  }
  thePlot.plotType = "surface";
  thePlot.dimension = thePlot.coordinateFunctionsE.size;
  input.hasInputBoxVariables(&thePlot.parametersInPlay, &thePlot.parametersInPlayJS);
  Plot result;
  result += thePlot;
  return output.assignValue(result, calculator);
}

class QRFactorizationComputation{
};
