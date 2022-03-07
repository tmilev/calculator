 // The current file is licensed under the license terms found in the main header
// file "calculator.h".
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
  Expression functionExpression;
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
  List<List<Vector<double> > > triangles;
  MapList<Vector<double>, double, MathRoutines::hashVectorDoubles>
  evaluatedPoints;
  HashedList<Expression> knownEs;
  List<double> knownValues;
  Vector<double> lowerLeftCorner;
  Vector<double> upperRightCorner;
  Plot grid;
  Plot curve;
  Plot plot;
  bool flagShowGrid;
  bool flagFunctionEvaluationFailed;
  bool flagTriangleLimitReached;
  int cleanUpTrianglesReturnUpdatedCurrentIndex(int currentIndex);
  void computeImplicitPlot();
  void computeImplicitPlotPart2();
  void plotGrid(int color);
  bool evaluatesToDouble(double& whichDouble);
  double getValueAtPoint(const Vector<double>& point);
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
  bool computePoints(
    Calculator& calculator, const Expression& input, bool showGrid
  );
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

void MeshTriangles::plotGrid(int color) {
  STACK_TRACE("MeshTriangles::plotGrid");
  this->grid.clearPlotObjects();
  this->grid.setExpectedPlotObjects(this->triangles.size * 3);
  PlotObject currentLinePlot;
  List<Vector<double> >& pointsVector = currentLinePlot.pointsDouble;
  currentLinePlot.pointsDouble.setSize(4);
  currentLinePlot.colorRGB = color;
  for (int i = 0; i < this->triangles.size; i ++) {
    pointsVector[0] = this->triangles[i][0];
    pointsVector[1] = this->triangles[i][1];
    pointsVector[2] = this->triangles[i][2];
    pointsVector[3] = this->triangles[i][0];
    this->grid.addPlotOnTop(currentLinePlot);
  }
}

double MeshTriangles::getValueAtPoint(const Vector<double>& point) {
  int index = this->evaluatedPoints.getIndex(point);
  if (index != - 1) {
    return this->evaluatedPoints.values[index];
  }
  this->knownValues[this->knownValues.size - 2] = point[0];
  this->knownValues[this->knownValues.size - 1] = point[1];
  double functionValue = 0;
  if (
    this->functionExpression.evaluatesToDoubleUnderSubstitutions(
      this->knownEs, this->knownValues, &functionValue
    )
  ) {
    this->numGoodEvaluations ++;
  } else {
    this->numBadEvaluations ++;
    functionValue = std::nan("");
  }
  this->evaluatedPoints.setKeyValue(point, functionValue);
  return functionValue;
}

void MeshTriangles::evaluateFunAtTriangleVertices(int triangleIndex) {
  STACK_TRACE("MeshTriangles::EvaluateFunAtTriangleVertices");
  for (int j = 0; j < this->triangles[triangleIndex].size; j ++) {
    this->getValueAtPoint(this->triangles[triangleIndex][j]);
  }
}

double MeshTriangles::getTriangleMaxSideLength(int triangleIndex) {
  STACK_TRACE("MeshTriangles::GetTriangleMaxSideLength");
  if (this->triangles[triangleIndex].size != 3) {
    global.fatal
    << "Error: triangle needs three vertices, instead it has vertices: "
    << this->triangles[triangleIndex]
    << global.fatal;
  }
  Vector<double>& firstV = this->triangles[triangleIndex][0];
  Vector<double>& secondV = this->triangles[triangleIndex][1];
  Vector<double>& thirdV = this->triangles[triangleIndex][2];
  List<Vector<double> > sides;
  sides.addOnTop(firstV - secondV);
  sides.addOnTop(secondV - thirdV);
  sides.addOnTop(thirdV - firstV);
  double result = 0;
  for (int i = 0; i < sides.size; i ++) {
    double normSquared = sides[i][0] * sides[i][0] + sides[i][1] * sides[i][1];
    result =
    MathRoutines::maximum(
      result, FloatingPoint::sqrtFloating(normSquared)
    );
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

int MeshTriangles::cleanUpTrianglesReturnUpdatedCurrentIndex(int currentIndex)
{
  STACK_TRACE("MeshTriangles::cleanUpTrianglesReturnUpdatedCurrentIndex");
  if (
    this->flagTriangleLimitReached ||
    this->triangles.size < this->maxNumTriangles * 2
  ) {
    return currentIndex;
  }
  // clean up carried out only when we have twice
  // exceeded the triangle limit
  // this way clean up will be carried only after
  // we have generated at least maxNumTriangles since the last cleanup
  // (which leaves no more than maxNumTriangles alive).
  if (this->triangles.size != this->trianglesUsed.size) {
    global.fatal
    <<
    "this->triangles.size is not equal to this->trianglesUsed.size: the values are: "
    << this->triangles.size
    << " and "
    << this->trianglesUsed.size
    << ". "
    << global.fatal;
  }
  int lowestFree = 0;
  for (int i = 0; i < this->triangles.size; i ++) {
    if (this->trianglesUsed[i]) {
      this->triangles[lowestFree] = this->triangles[i];
      this->trianglesUsed[lowestFree] = true;
      if (i == currentIndex) {
        currentIndex = lowestFree;
      }
      lowestFree ++;
    }
  }
  this->triangles.setSize(lowestFree);
  this->trianglesUsed.setSize(lowestFree);
  if (lowestFree > this->maxNumTriangles) {
    this->flagTriangleLimitReached = true;
  }
  return currentIndex;
}

void MeshTriangles::subdivide(int triangleIndex) {
  STACK_TRACE("MeshTriangles::subdivide");
  List<Vector<double> > currentTriangle = this->triangles[triangleIndex];
  if (currentTriangle.size != 3) {
    global.fatal << "Triangle in mesh with less than 3 sides! " << global.fatal
    ;
  }
  List<Vector<double> > insideTriange;
  insideTriange.setSize(3);
  insideTriange[0] = (currentTriangle[1] + currentTriangle[2]) * 0.5;
  insideTriange[1] = (currentTriangle[2] + currentTriangle[0]) * 0.5;
  insideTriange[2] = (currentTriangle[0] + currentTriangle[1]) * 0.5;
  this->triangles.addOnTop(insideTriange);
  this->trianglesUsed.addOnTop(true);
  List<Vector<double> > newTriangle;
  newTriangle = insideTriange;
  newTriangle[0] = currentTriangle[0];
  this->triangles.addOnTop(newTriangle);
  this->trianglesUsed.addOnTop(true);
  newTriangle = insideTriange;
  newTriangle[1] = currentTriangle[1];
  this->triangles.addOnTop(newTriangle);
  this->trianglesUsed.addOnTop(true);
  newTriangle = insideTriange;
  newTriangle[2] = currentTriangle[2];
  this->triangles.addOnTop(newTriangle);
  this->trianglesUsed.addOnTop(true);
}

void MeshTriangles::computeImplicitPlotPart2() {
  STACK_TRACE("MeshTriangles::ComputeImplicitPlotPart2");
  this->triangles.setExpectedSize(this->maxNumTriangles * 2);
  this->trianglesUsed.setExpectedSize(this->maxNumTriangles * 2);
  this->trianglesUsed.initializeFillInObject(this->triangles.size, true);
  this->evaluatedPoints.setExpectedSize(this->maxNumTriangles * 4);
  this->flagTriangleLimitReached = false;
  for (int i = 0; i < this->triangles.size; i ++) {
    this->evaluateFunAtTriangleVertices(i);
  }
  if (this->numGoodEvaluations < 5) {
    this->flagFunctionEvaluationFailed = true;
    return;
  }
  double minSide =
  MathRoutines::minimum(this->height, this->width) *
  this->minTriangleSideAsPercentOfWidthPlusHeight;
  PlotObject currentPlot;
  currentPlot.colorRGB = static_cast<int>(
    HtmlRoutines::redGreenBlue(255, 0, 0)
  );
  Vectors<double>& segment = currentPlot.pointsDouble;
  List<Vector<double> > currentTriangle;
  for (int i = 0; i < this->triangles.size; i ++) {
    currentTriangle = this->triangles[i];
    // making a copy in case this->triangles changes underneath.
    if (currentTriangle.size != 3) {
      global.fatal
      << "Error: triangle needs three vertices, instead it has vertices: "
      << currentTriangle
      << global.fatal;
    }
    bool isGood = true;
    for (int j = 0; j < currentTriangle.size; j ++) {
      if (
        this->getValueAtPoint(currentTriangle[j]) == std::nan("")
      ) {
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
    if (
      this->getTriangleMaxSideLength(i) > minSide &&
      !this->flagTriangleLimitReached
    ) {
      this->subdivide(i);
      continue;
    }
    segment.setSize(0);
    if (prod01 <= 0) {
      this->addPointFromVerticesValues(
        segment, currentTriangle[0], currentTriangle[1], val0, val1
      );
    }
    if (prod12 <= 0) {
      this->addPointFromVerticesValues(
        segment, currentTriangle[1], currentTriangle[2], val1, val2
      );
    }
    if (prod20 <= 0 && segment.size < 2) {
      this->addPointFromVerticesValues(
        segment, currentTriangle[2], currentTriangle[0], val2, val0
      );
    }
    if (segment.size != 2) {
      continue;
    }
    this->curve.addPlotOnTop(currentPlot);
  }
}

void MeshTriangles::computeImplicitPlot() {
  STACK_TRACE("MeshTriangles::ComputeImplicitPlot");
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
      this->triangles.addOnTop(currentTriangle);
      currentTriangle[1] = currentTriangle[0];
      currentTriangle[1][1] += deltaY;
      this->triangles.addOnTop(currentTriangle);
    }
  }
  if (this->flagShowGrid) {
    this->plotGrid(
      static_cast<int>(HtmlRoutines::redGreenBlue(240, 240, 0))
    );
    this->plot.addPlotsOnTop(this->grid);
  }
  this->computeImplicitPlotPart2();
  if (this->flagShowGrid) {
    this->plotGrid(
      static_cast<int>(HtmlRoutines::redGreenBlue(100, 100, 100))
    );
    this->plot.addPlotsOnTop(this->grid);
  }
  this->plot.addPlotsOnTop(this->curve);
}

bool Calculator::getMatrixDoubles(
  const Expression& input,
  Matrix<double>& output,
  int desiredNumberOfColumns
) {
  return
  CalculatorConversions::functionGetMatrix<double>(
    *this, input, output, true, nullptr, desiredNumberOfColumns
  );
}

bool Calculator::getVectorDoubles(
  const Expression& input,
  Vector<double>& output,
  int desiredDimensionNonMandatory
) {
  return
  this->getVector(
    input,
    output,
    nullptr,
    desiredDimensionNonMandatory // ,
    // CalculatorFunctions::functionEvaluateToDouble
  );
}

bool Calculator::getVectorDoublesFromFunctionArguments(
  const Expression& input,
  Vector<double>& output,
  int desiredDimensionNonMandatory
) {
  return
  this->getVectorFromFunctionArguments(
    input,
    output,
    nullptr,
    desiredDimensionNonMandatory // ,
    // CalculatorFunctions::functionEvaluateToDouble
  );
}

bool CalculatorFunctions::getPointsImplicitly(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::getPointsImplicitly");
  MeshTriangles mesh;
  if (!mesh.computePoints(calculator, input, false)) {
    return false;
  }
  HashedList<Vector<double>, MathRoutines::hashVectorDoubles> points;
  List<PlotObject>& plots = mesh.curve.getPlots();
  for (int i = 0; i < plots.size; i ++) {
    points.addOnTopNoRepetition(plots[i].pointsDouble[0]);
    points.addOnTopNoRepetition(plots[i].pointsDouble[1]);
  }
  Matrix<double> matrix;
  matrix.assignVectorsToRows(points);
  return output.makeMatrix(calculator, matrix);
}

bool CalculatorFunctionsPlot::plotImplicitFunction(
  Calculator& calculator, const Expression& input, Expression& output
) {
  return
  CalculatorFunctionsPlot::plotImplicitFunctionFull(
    calculator, input, output, false
  );
}

bool CalculatorFunctionsPlot::plotImplicitShowGridFunction(
  Calculator& calculator, const Expression& input, Expression& output
) {
  return
  CalculatorFunctionsPlot::plotImplicitFunctionFull(
    calculator, input, output, true
  );
}

bool MeshTriangles::computePoints(
  Calculator& calculator, const Expression& input, bool showGrid
) {
  STACK_TRACE("MeshTriangles::ComputePoints");
  if (input.size() < 5) {
    return false;
  }
  this->plot.dimension = 2;
  this->flagShowGrid = showGrid;
  this->owner = &calculator;
  this->functionExpression = input[1];
  this->knownEs = calculator.knownDoubleConstants;
  this->knownValues = calculator.knownDoubleConstantValues;
  this->knownValues.addOnTop(0);
  this->knownValues.addOnTop(0);
  HashedList<Expression> freeVariables;
  if (!this->functionExpression.getFreeVariables(freeVariables, true)) {
    return
    calculator
    << "Failed to extract free variables from: "
    << this->functionExpression.toString();
  }
  freeVariables.quickSortAscending();
  if (freeVariables.size > 2) {
    return
    calculator
    << "I got that your curve depends on "
    << freeVariables.size
    << " expressions, namely: "
    << freeVariables.toStringCommaDelimited()
    << " and that is too many (2 max). ";
  }
  Expression tempE;
  if (freeVariables.size == 0) {
    tempE.makeAtom(calculator, "x");
    freeVariables.addOnTop(tempE);
  }
  if (freeVariables.size == 1) {
    tempE.makeAtom(calculator, "y");
    if (freeVariables[0] == tempE) {
      tempE.makeAtom(calculator, "x");
    }
    freeVariables.addOnTop(tempE);
  }
  this->knownEs.addOnTopNoRepetitionMustBeNew(freeVariables[0]);
  this->knownEs.addOnTopNoRepetitionMustBeNew(freeVariables[1]);
  if (!calculator.getVectorDoubles(input[2], this->lowerLeftCorner)) {
    return
    calculator
    << "Failed to extract lower left corner from: "
    << input[2].toString();
  }
  if (
    !calculator.getVectorDoubles(input[3], this->upperRightCorner)
  ) {
    return
    calculator
    << "Failed to extract upper right corner from: "
    << input[3].toString();
  }
  List<int> gridCount;
  if (!calculator.getVectorInt(input[4], gridCount)) {
    return
    calculator
    << "Failed to extract pair of small integers from: "
    << input[4].toString();
  }
  if (gridCount.size != 2) {
    return
    calculator
    << "Failed to extract pair of small integers from: "
    << input[4].toString();
  }
  this->xStartingGridCount = gridCount[0];
  this->yStartingGridCount = gridCount[1];
  if (input.size() >= 6) {
    if (!input[5].isSmallInteger(&this->maxNumTriangles)) {
      return
      calculator
      << "Failed to extract small integer from: "
      << input[5].toString();
    }
    if (this->maxNumTriangles > 20000) {
      this->maxNumTriangles = 20000;
      calculator
      << "Max number of triangles decreased from your input: "
      << input[5].toString()
      <<
      " to 20000. If you'd like to lift the restriction, modify code around: file: "
      << __FILE__
      << " line: "
      << __LINE__
      << ". ";
    }
  }
  this->computeImplicitPlot();
  calculator
  << "Evaluated function in: "
  << this->numGoodEvaluations
  << " points and failed to evaluate it at: "
  << this->numBadEvaluations
  << " points. ";
  return true;
}

bool CalculatorFunctionsPlot::plotImplicitFunctionFull(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  bool showGrid
) {
  STACK_TRACE("CalculatorFunctions::plotImplicitFunctionFull");
  MeshTriangles mesh;
  if (!mesh.computePoints(calculator, input, showGrid)) {
    return false;
  }
  return output.assignValue(calculator, mesh.plot);
}

bool CalculatorConversions::functionMatrixDouble(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorConversions::functionMatrixDouble");
  Matrix<double> matrix;
  if (
    !CalculatorConversions::functionGetMatrix(
      calculator, input, matrix, true, nullptr, 0
    )
  ) {
    return calculator << "<br>Failed to get matrix of doubles. ";
  }
  return output.makeMatrix(calculator, matrix);
}

bool CalculatorFunctionsIntegration::integratePullConstant(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsIntegration::integratePullConstant");
  Expression functionExpression, variableExpression, setExpression;
  if (
    !input.isIndefiniteIntegralFdx(
      &variableExpression, &functionExpression, &setExpression
    )
  ) {
    return false;
  }
  Expression functionCoefficient, functionNoCoefficient;
  functionExpression.getCoefficientMultiplicandForm(
    functionCoefficient, functionNoCoefficient
  );
  if (functionCoefficient.isEqualToOne()) {
    return false;
  }
  Expression newIntegralExpression;
  newIntegralExpression.makeIntegral(
    calculator, setExpression, functionNoCoefficient, variableExpression
  );
  output = functionCoefficient * newIntegralExpression;
  return true;
}

bool CalculatorFunctionsIntegration::integrateSqrtOneMinusXsquared(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsIntegration::integrateSqrtOneMinusXsquared");
  Expression functionExpression, variableExpression, setExpression;
  if (
    !input.isIndefiniteIntegralFdx(
      &variableExpression, &functionExpression, &setExpression
    )
  ) {
    return false;
  }
  Expression functionCoefficient;
  Expression functionNoCoefficient;
  functionExpression.getCoefficientMultiplicandForm(
    functionCoefficient, functionNoCoefficient
  );
  if (!functionNoCoefficient.startsWith(calculator.opPower(), 3)) {
    return false;
  }
  if (!functionNoCoefficient[2].isEqualToHalf()) {
    return false;
  }
  Expression a, b, c;
  if (
    !CalculatorFunctions::extractQuadraticCoefficientsWithRespectToVariable(
      functionNoCoefficient[1], variableExpression, a, b, c
    )
  ) {
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
  Expression squareRootedCoefficient, radicalSquared, radical;
  squareRootedCoefficient.makeSqrt(calculator, a *(- 1));
  functionCoefficient *= squareRootedCoefficient;
  radicalSquared = c / a *(- 1);
  radical.makeSqrt(calculator, radicalSquared);
  Expression rescaledArgument, arcCosPart, algSQRTPart, algPart;
  rescaledArgument = variableExpression / radical;
  arcCosPart.makeOX(calculator, calculator.opArcCos(), rescaledArgument);
  arcCosPart *= radicalSquared / - 2;
  algSQRTPart = radicalSquared - variableExpression * variableExpression;
  algPart.makeSqrt(calculator, algSQRTPart);
  algPart *= variableExpression / 2;
  output = functionCoefficient *(arcCosPart + algPart);
  return true;
}

bool CalculatorFunctionsIntegration::integrateXpowerNePowerAx(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsIntegration::integrateXpowerNePowerAx");
  Expression functionExpression, variableExpression, setExpression;
  if (
    !input.isIndefiniteIntegralFdx(
      &variableExpression, &functionExpression, &setExpression
    )
  ) {
    return false;
  }
  if (!functionExpression.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  Expression exponentPartE = functionExpression[1];
  // <- note: the seemingly odd order is intentional!
  Expression polyPartE = functionExpression[2];
  // <- note: the seemingly odd order is intentional!
  Expression powerOfXE, powerOfEE;
  Expression aE, bE;
  // exponent is of form aX+b
  powerOfXE.assignValue(calculator, 1);
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
    if (
      !CalculatorFunctions::extractLinearCoefficientsWithRespectToVariable(
        powerOfEE, variableExpression, aE, bE
      )
    ) {
      continue;
    }
    if (!aE.isConstantNumber() || !bE.isConstantNumber()) {
      continue;
    }
    if (polyPartE != variableExpression) {
      if (!polyPartE.startsWith(calculator.opPower(), 3)) {
        continue;
      }
      if (polyPartE[1] != variableExpression) {
        continue;
      }
      int integerValue = - 1;
      if (!polyPartE[2].isSmallInteger(&integerValue)) {
        continue;
      }
      if (integerValue <= 0) {
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
    calculator,
    calculator.opPower(),
    variableExpression,
    powerOfXE - calculator.expressionOne()
  );
  remainingIntegrand *= exponentPartE;
  integralPart.makeIntegral(
    calculator, setExpression, remainingIntegrand, variableExpression
  );
  output = (polyPartE * exponentPartE - powerOfXE * integralPart) / aE;
  return true;
}

bool CalculatorFunctionsIntegration::integrateSqrtXsquaredMinusOne(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsIntegration::integrateSqrtXsquaredMinusOne");
  Expression functionExpression, variableExpression, setExpression;
  if (
    !input.isIndefiniteIntegralFdx(
      &variableExpression, &functionExpression, &setExpression
    )
  ) {
    return false;
  }
  Expression functionCoefficient, functionNoCoefficient;
  functionExpression.getCoefficientMultiplicandForm(
    functionCoefficient, functionNoCoefficient
  );
  if (!functionNoCoefficient.startsWith(calculator.opPower(), 3)) {
    return false;
  }
  if (!functionNoCoefficient[2].isEqualToHalf()) {
    return false;
  }
  Expression a, b, c;
  if (
    !CalculatorFunctions::extractQuadraticCoefficientsWithRespectToVariable(
      functionNoCoefficient[1], variableExpression, a, b, c
    )
  ) {
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
  Expression extraCoefficient;
  Expression variableChangeCoeffiicent;
  Expression newVariableExpression;
  extraCoefficient.makeSqrt(calculator, c *(- 1));
  functionCoefficient *= extraCoefficient;
  variableChangeCoeffiicent.makeSqrt(calculator, (a / c) *(- 1));
  newVariableExpression = variableExpression * variableChangeCoeffiicent;
  functionCoefficient /= variableChangeCoeffiicent;
  Expression algSQRTPart, algPart, lnPart;
  algSQRTPart =
  newVariableExpression * newVariableExpression - calculator.expressionOne();
  algPart.makeSqrt(calculator, algSQRTPart);
  lnPart.makeOX(
    calculator, calculator.opLog(), newVariableExpression - algPart
  );
  output = functionCoefficient *(algPart * newVariableExpression + lnPart) / 2;
  return true;
}

bool CalculatorFunctionsIntegration::integrateDefiniteIntegral(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsIntegration::integrateDefiniteIntegral");
  Expression functionExpression, variableExpression, setExpression;
  if (
    !input.isDefiniteIntegralOverIntervalFdx(
      &variableExpression, &functionExpression, &setExpression
    )
  ) {
    return false;
  }
  if (!setExpression.startsWith(calculator.opLimitBoundary(), 3)) {
    return false;
  }
  if (
    setExpression[1].containsAsSubExpressionNoBuiltInTypes(
      calculator.opInfinity()
    ) ||
    setExpression[2].containsAsSubExpressionNoBuiltInTypes(
      calculator.opInfinity()
    )
  ) {
    return false;
  }
  Expression indefiniteIntegral, indefiniteExpression;
  indefiniteExpression.makeAtom(
    calculator, calculator.opIndefiniteIndicator()
  );
  indefiniteIntegral.makeIntegral(
    calculator, indefiniteExpression, functionExpression, variableExpression
  );
  Expression solvedIntegral;
  if (
    !calculator.evaluateExpression(
      calculator, indefiniteIntegral, solvedIntegral
    )
  ) {
    return false;
  }
  if (
    solvedIntegral.containsAsSubExpressionNoBuiltInTypes(
      calculator.opIntegral()
    )
  ) {
    return false;
  }
  if (
    solvedIntegral.containsAsSubExpressionNoBuiltInTypes(
      calculator.opDivide()
    )
  ) {
    return false;
  }
  if (
    solvedIntegral.containsAsSubExpressionNoBuiltInTypes(
      calculator.opLog()
    )
  ) {
    return false;
  }
  HashedList<Expression> variables;
  List<double> variableValues;
  variables.addOnTop(variableExpression);
  variableValues.addOnTop(0);
  for (int i = 1; i <= 2; i ++) {
    if (setExpression[i].evaluatesToDouble(&variableValues[0])) {
      double result = 0;
      if (
        !solvedIntegral.evaluatesToDoubleUnderSubstitutions(
          variables, variableValues, &result
        )
      ) {
        return false;
      }
    }
  }
  Expression substitutionTop(calculator), substitutionBottom(calculator);
  substitutionTop.addChildAtomOnTop(calculator.opDefine());
  substitutionTop.addChildOnTop(variableExpression);
  substitutionBottom = substitutionTop;
  substitutionBottom.addChildOnTop(setExpression[1]);
  substitutionTop.addChildOnTop(setExpression[2]);
  Expression topCommands(calculator), bottomCommands(calculator);
  topCommands.addChildAtomOnTop(calculator.opCommandSequence());
  bottomCommands.addChildAtomOnTop(calculator.opCommandSequence());
  topCommands.addChildOnTop(substitutionTop);
  bottomCommands.addChildOnTop(substitutionBottom);
  topCommands.addChildOnTop(solvedIntegral);
  bottomCommands.addChildOnTop(solvedIntegral);
  Expression topExpression, bottomExpression;
  topExpression.makeXOX(
    calculator,
    calculator.opUnderscore(),
    topCommands,
    calculator.expressionTwo()
  );
  bottomExpression.makeXOX(
    calculator,
    calculator.opUnderscore(),
    bottomCommands,
    calculator.expressionTwo()
  );
  output = topExpression - bottomExpression;
  return true;
}

bool CalculatorFunctions::applyToSubexpressionsRecurseThroughCalculusFunctions(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE(
    "CalculatorFunctions::applyToSubexpressionsRecurseThroughCalculusFunctions"
  );
  if (input.size() != 3) {
    return false;
  }
  if (
    !input.startsWithGivenOperation(
      "ApplyToSubexpressionsRecurseThroughCalculusFunctions"
    )
  ) {
    return false;
  }
  const Expression& argumentExpression = input[2];
  if (
    argumentExpression.startsWith(calculator.opPlus()) ||
    argumentExpression.startsWith(calculator.opMinus()) ||
    argumentExpression.startsWith(calculator.opTimes()) ||
    argumentExpression.startsWith(calculator.opPower()) ||
    argumentExpression.startsWith(calculator.opDivide()) ||
    argumentExpression.startsWith(calculator.opSequence()) ||
    argumentExpression.startsWith(calculator.opSqrt()) ||
    argumentExpression.startsWith(calculator.opCos()) ||
    argumentExpression.startsWith(calculator.opSin())
  ) {
    output.reset(calculator);
    output.addChildOnTop(input[1]);
    Expression recursivelyModifiedExpression(calculator);
    recursivelyModifiedExpression.addChildOnTop(argumentExpression[0]);
    for (int i = 1; i < argumentExpression.size(); i ++) {
      Expression nextE(calculator);
      nextE.addChildAtomOnTop(
        "ApplyToSubexpressionsRecurseThroughCalculusFunctions"
      );
      nextE.addChildOnTop(input[1]);
      nextE.addChildOnTop(argumentExpression[i]);
      recursivelyModifiedExpression.addChildOnTop(nextE);
    }
    return output.addChildOnTop(recursivelyModifiedExpression);
  }
  output.reset(calculator);
  output.addChildOnTop(input[1]);
  output.addChildOnTop(argumentExpression);
  return true;
}

bool CalculatorFunctions::numerator(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::numerator");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Rational rational;
  if (argument.isRational(&rational)) {
    return
    output.assignValue(calculator, Rational(rational.getNumerator()));
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

bool CalculatorFunctions::denominator(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::denominator");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  Rational rational, denominator;
  if (argument.isRational(&rational)) {
    denominator = rational.getDenominator();
    return output.assignValue(calculator, denominator);
  }
  if (argument.startsWith(calculator.opDivide())) {
    if (argument.size() > 2) {
      output = argument[2];
      return true;
    }
  }
  return output.assignValue(calculator, 1);
}

bool CalculatorFunctions::handleUnderscorePowerLimits(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::handleUnderscorePowerLimits");
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

bool CalculatorFunctions::sumAsOperatorToSumInternalNotation(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::sumAsOperatorToSumInternalNotation");
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
  List<Expression> remaining;
  for (int i = 1; i < input.size(); i ++) {
    remaining.addOnTop(input[i]);
  }
  Expression argumentE;
  argumentE.makeSequence(calculator, &remaining);
  return output.addChildOnTop(argumentE);
}

bool CalculatorFunctions::sumTimesExpressionToSumOf(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::sumTimesExpressionToSumOf");
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
    Expression accumulated(calculator);
    accumulated.addChildOnTop(input[0]);
    accumulated.addChildOnTop(input[1][2]);
    accumulated.addChildOnTop(input[2]);
    output = input[1];
    return output.setChild(2, accumulated);
  }
  return false;
}

bool CalculatorFunctions::sumSequence(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::sumSequence");
  if (input.size() < 1) {
    return false;
  }
  if (input.size() == 1) {
    return output.assignValue(calculator, 0);
  }
  if (input[1].startsWith(calculator.opLimitBoundary())) {
    return false;
  }
  const Expression* sequenceToSum = &input;
  if (input[1].isSequenceNElements()) {
    sequenceToSum = &input[1];
  }
  List<Expression> terms;
  for (int i = 1; i < sequenceToSum->size(); i ++) {
    terms.addOnTop((*sequenceToSum)[i]);
  }
  return output.makeSum(calculator, terms);
}

bool CalculatorFunctions::multiplySequence(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::multiplySequence");
  if (input.size() < 1) {
    return false;
  }
  if (input.size() == 2 && input[1].isSequenceNElements()) {
    return
    CalculatorFunctions::multiplySequence(calculator, input[1], output);
  }
  List<Expression> terms;
  for (int i = 1; i < input.size(); i ++) {
    terms.addOnTop(input[i]);
  }
  return output.makeProduct(calculator, terms);
}

bool CalculatorFunctions::ensureExpressionDependsOnlyOnStandard(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::ensureExpressionDependsOnlyOnStandard");
  if (input.size() < 3) {
    return false;
  }
  const Expression& expression = input[1];
  HashedList<Expression> allowedFreeVars, presentFreeVars;
  allowedFreeVars.setExpectedSize(input.size() - 2);
  presentFreeVars.setExpectedSize(input.size() - 2);
  for (int i = 2; i < input.size(); i ++) {
    allowedFreeVars.addOnTopNoRepetition(input[i]);
  }
  std::stringstream out;
  expression.getFreeVariables(presentFreeVars, true);
  if (!allowedFreeVars.contains(presentFreeVars)) {
    out << "<hr>";
    out
    << "Your expression:<br>\\("
    << input[1].toString()
    << "\\)"
    <<
    "<br><b style ='color:red'>contains the unexpected variable(s):</b><br><b>"
    ;
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
    out
    << "<br>The expected variables are: "
    << allowedFreeVars.toStringCommaDelimited()
    << ". ";
    out
    <<
    "<br>Beware of typos such as:<br>[wrong:] <span style ='color:red'>lnx, sqrt2</span>  "
    <<
    "<br>[correct:] <span style ='color:green'>ln(x)</span> or <span style ='color:green'>ln x</span>, "
    <<
    "<span style ='color:green'>sqrt(2)</span> or <span style ='color:green'>sqrt 2</span>.<hr>"
    ;
    return output.assignValue(calculator, out.str());
  }
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctions::removeDuplicates(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::removeDuplicates");
  int operationRemoveDuplicated =
  calculator.operations.getIndexNoFail("RemoveDuplicates");
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

bool CalculatorFunctions::sort(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::sort");
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

bool CalculatorFunctions::sortDescending(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::sortDescending");
  if (
    !input.isListStartingWithAtom(
      calculator.operations.getIndexNoFail("SortDescending")
    ) &&
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

bool CalculatorFunctionsListsAndSets::length(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsListsAndSets::length");
  if (
    !input.isListStartingWithAtom(
      calculator.operations.getIndexNoFail("Length")
    )
  ) {
    return false;
  }
  if (input.size() == 2) {
    if (input[1].isSequenceNElements()) {
      return output.assignValue(calculator, input[1].size() - 1);
    }
  }
  return output.assignValue(calculator, input.size() - 1);
}

bool CalculatorFunctions::ensureExpressionDependsOnlyOnMandatoryVariables(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  bool excludeNamedConstants
) {
  STACK_TRACE(
    "CalculatorFunctions::ensureExpressionDependsOnlyOnMandatoryVariables"
  );
  if (input.size() < 3) {
    return false;
  }
  const Expression& expression = input[1];
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
  expression.getFreeVariables(presentFreeVars, excludeNamedConstants);
  std::stringstream out;
  if (!presentFreeVars.contains(mandatoryFreeVars)) {
    out << "<hr>";
    out
    << "Your expression:<br>\\("
    << input[1].toString()
    << "\\)"
    <<
    "<br><b style ='color:red'>is required to contain the variables:</b><br><b>"
    ;
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
    out
    << "<br>The mandatory variable(s) are: "
    << mandatoryFreeVars.toStringCommaDelimited()
    << ". ";
  }
  if (!allowedFreeVars.contains(presentFreeVars)) {
    out << "<hr>";
    out
    << "Your expression:<br>\\("
    << input[1].toString()
    << "\\)"
    <<
    "<br><b style ='color:red'>contains the unexpected variable(s):</b><br><b>"
    ;
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
    out
    << "<br>The expected variables are: "
    << allowedFreeVars.toStringCommaDelimited()
    << ". ";
  }
  if (out.str() != "") {
    out
    <<
    "<br>Beware of typos such as:<br>[wrong:] <span style = 'color:red'>lnx, sqrt2</span>  "
    <<
    "<br>[correct:] <span style = 'color:green'>ln(x)</span> or <span style ='color:green'>ln x</span>, "
    <<
    "<span style ='color:green'>sqrt(2)</span> or <span style ='color:green'>sqrt 2</span>.<hr>"
    ;
  }
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctionsPlot::plotGrid(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::plotGrid");
  (void) input;
  PlotObject plot;
  plot.plotType = "axesGrid";
  plot.dimension = 2;
  return output.assignValue(calculator, plot);
}

bool CalculatorFunctionsPlot::plotRemoveCoordinateAxes(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::plotRemoveCoordinateAxes");
  (void) input;
  Plot plotFinal;
  plotFinal.dimension = 2;
  plotFinal.flagIncludeCoordinateSystem = false;
  return output.assignValue(calculator, plotFinal);
}

bool CalculatorFunctionsPlot::plotLabel(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::plotLabel");
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
  plot.makeLabel(labelPosition, label);
  return output.assignValue(calculator, plot);
}

bool CalculatorFunctionsPlot::plotRectangle(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::plotRectangle");
  if (input.size() != 3) {
    return false;
  }
  Vectors<double> rectangle;
  rectangle.setSize(2);
  if (
    !calculator.getVectorDoubles(input[1], rectangle[0], 2) ||
    !calculator.getVectorDoubles(input[2], rectangle[1], 2)
  ) {
    return false;
  }
  PlotObject plot;
  plot.dimension = 2;
  plot.plotType = PlotObject::PlotTypes::pathFilled;
  Vector<double> currentCorner = rectangle[0];
  Vector<double>& dimensions = rectangle[1];
  plot.pointsDouble.addOnTop(currentCorner);
  currentCorner[0] += dimensions[0];
  plot.pointsDouble.addOnTop(currentCorner);
  currentCorner[1] += dimensions[1];
  plot.pointsDouble.addOnTop(currentCorner);
  currentCorner[0] -= dimensions[0];
  plot.pointsDouble.addOnTop(currentCorner);
  currentCorner[1] -= dimensions[1];
  plot.pointsDouble.addOnTop(currentCorner);
  plot.colorFillJS = "cyan";
  plot.colorJS = "blue";
  plot.pointsDouble.addOnTop(currentCorner);
  plot.rectangles.addOnTop(rectangle);
  plot.colorRGB = static_cast<int>(
    HtmlRoutines::redGreenBlue(0, 0, 255)
  );
  plot.colorFillRGB = static_cast<int>(
    HtmlRoutines::redGreenBlue(0, 255, 255)
  );
  return output.assignValue(calculator, plot);
}

bool CalculatorFunctions::operatorBounds(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::operatorBounds");
  if (
    !input.startsWith(calculator.opUnderscore(), 3) &&
    !input.startsWith(calculator.opPower(), 3)
  ) {
    return false;
  }
  const Expression& baseE = input[1];
  Expression limitsExpression;
  int integralOperator = baseE.data;
  if (
    integralOperator == calculator.opIntegral() ||
    integralOperator == calculator.opSum()
  ) {
    output.reset(calculator);
    output.addChildAtomOnTop(integralOperator);
    limitsExpression.reset(calculator);
    limitsExpression.addChildAtomOnTop(calculator.opLimitBoundary());
    limitsExpression.addChildOnTop(input[2]);
    limitsExpression.addChildAtomOnTop(calculator.opIndefiniteIndicator());
    output.addChildOnTop(limitsExpression);
    return true;
  }
  if (
    !baseE.startsWith(calculator.opIntegral(), 2) &&
    !baseE.startsWith(calculator.opSum(), 2)
  ) {
    return false;
  }
  limitsExpression.reset(calculator);
  limitsExpression.addChildAtomOnTop(calculator.opLimitBoundary());
  for (int i = 1; i < 3; i ++) {
    if (i < baseE[1].size()) {
      limitsExpression.addChildOnTop(baseE[1][i]);
    } else {
      limitsExpression.addChildAtomOnTop(
        calculator.opIndefiniteIndicator()
      );
    }
  }
  if (input[1].isOperationGiven(calculator.opUnderscore())) {
    limitsExpression.setChild(1, input[2]);
  } else {
    limitsExpression.setChild(2, input[2]);
  }
  output = input[1];
  return output.setChild(1, limitsExpression);
}

bool CalculatorFunctions::powerExponentToLog(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::powerExponentToLog");
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

bool CalculatorFunctions::distributeExponent(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("Calculator::distributeExponent");
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
  bool isGood = base[1].isPositiveNumber() || base[2].isPositiveNumber();
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
  leftE.makeXOX(
    calculator, calculator.opPower(), input[1][1], input[2]
  );
  rightE.makeXOX(
    calculator, calculator.opPower(), input[1][2], input[2]
  );
  return output.makeXOX(calculator, calculator.opTimes(), leftE, rightE);
}

bool CalculatorFunctions::sqrt(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("Calculator::sqrt");
  if (input.size() != 3) {
    return false;
  }
  Rational ratPower;
  if (input[1].isRational(&ratPower)) {
    if (ratPower != 0) {
      Expression powerE, powerEreduced, exponentExpression;
      ratPower.invert();
      exponentExpression.assignValue(calculator, ratPower);
      powerE.makeXOX(
        calculator,
        calculator.opPower(),
        input[2],
        exponentExpression
      );
      if (
        CalculatorFunctionsBinaryOps::powerRationalByRationalReducePrimeFactors
        (calculator, powerE, powerEreduced)
      ) {
        if (powerEreduced != powerE && powerEreduced != input) {
          output = powerEreduced;
          return true;
        }
      }
    }
  }
  if (input[2].isEqualToOne()) {
    return output.assignValue(calculator, 1);
  }
  int power = 0;
  if (!input[1].isSmallInteger(&power)) {
    return false;
  }
  if (!input[2].isRational()) {
    calculator.checkInputNotSameAsOutput(input, output);
    Expression exponent;
    Rational powerRational(1, power);
    exponent.assignValue(calculator, powerRational);
    return
    output.makeXOX(
      calculator, calculator.opPower(), input[2], exponent
    );
  }
  if (power > 0 && input[2].isEqualToZero()) {
    return output.assignValue(calculator, 0);
  }
  if (power == 0 && input[2].isEqualToZero()) {
    return output.assignValue(calculator, 1);
  }
  Rational rationalValue;
  if (!input[2].isRational(&rationalValue)) {
    return false;
  }
  if (power < 0) {
    if (rationalValue.isEqualToZero()) {
      return
      output.assignError(
        calculator, "Division by zero in expression: " + input.toString()
      );
    }
    power *= - 1;
    rationalValue.invert();
  }
  if (power != 2) {
    return false;
  }
  AlgebraicNumber algebraicNumber;
  if (
    !algebraicNumber.assignRationalQuadraticRadical(
      rationalValue,
      calculator.objectContainer.algebraicClosure,
      &calculator.comments
    )
  ) {
    return false;
  }
  return output.assignValue(calculator, algebraicNumber);
}

bool CalculatorFunctionsBasic::floor(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBasic::floor");
  if (input.size() != 2) {
    return false;
  }
  Rational rational;
  if (input[1].isOfType<Rational>(&rational)) {
    rational.assignFloor();
    return output.assignValue(calculator, rational);
  }
  double floatingPointNumber = 0;
  if (input[1].evaluatesToDouble(&floatingPointNumber)) {
    return
    output.assignValue(
      calculator, static_cast<int>(std::floor(floatingPointNumber))
    );
  }
  return false;
}

bool CalculatorFunctionsBasic::logarithmBaseNCeiling(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBasic::logarithmBaseNCeiling");
  if (input.size() != 3) {
    return
    calculator
    << "Logarithm ceiling function expects exactly two arguments. ";
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
    return
    calculator
    << "Failed to extract positive intger from second argument. ";
  }
  int result = static_cast<int>(
    argument.logarithmBaseNCeiling(static_cast<unsigned>(smallInt))
  );
  return output.assignValue(calculator, result);
}

bool CalculatorFunctionsBasic::round(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBasic::round");
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
    return output.assignValue(calculator, result);
  }
  double doubleNumber = 0;
  if (input[1].evaluatesToDouble(&doubleNumber)) {
    return
    output.assignValue(
      calculator, static_cast<int>(std::round(doubleNumber))
    );
  }
  return false;
}

bool CalculatorFunctionsPlot::plotPath(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::plotPath");
  if (input.size() < 3) {
    return false;
  }
  const Expression& matrixE = input[1];
  Matrix<double> matrix;
  if (!calculator.getMatrixDoubles(matrixE, matrix)) {
    return
    calculator
    << "Failed to extract matrix from: "
    << matrixE.toString();
  }
  if (matrix.numberOfColumns != 2 && matrix.numberOfColumns != 3) {
    return
    calculator
    << "Only dimensions 2 and 3 are supported at the moment. ";
  }
  PlotObject segment;
  if (input.size() >= 3) {
    segment.colorJS = "black";
    segment.colorRGB = static_cast<int>(
      HtmlRoutines::redGreenBlue(0, 0, 0)
    );
    const Expression& colorE = input[2];
    if (!colorE.isOfType<std::string>(&segment.colorJS)) {
      segment.colorJS = colorE.toString();
    }
    if (
      !DrawingVariables::getColorIntFromColorString(
        segment.colorJS, segment.colorRGB
      )
    ) {
      calculator << "Unrecognized color: " << segment.colorJS;
    }
  }
  if (input.size() >= 4) {
    const Expression& lineWidthE = input[3];
    if (!lineWidthE.evaluatesToDouble(&segment.lineWidth)) {
      calculator
      << "Failed to extract line width from: "
      << lineWidthE.toString();
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
  Plot plot;
  plot += segment;
  return output.assignValue(calculator, plot);
}

bool CalculatorFunctionsPlot::plotMarkSegment(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsPlot::plotMarkSegment");
  if (input.size() < 3) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  Vector<double> leftV, rightV;
  if (
    !calculator.getVectorDoubles(leftE, leftV) ||
    !calculator.getVectorDoubles(rightE, rightV)
  ) {
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
      return
      calculator
      << "Could not extract small integer from "
      << input[3].toString();
    }
  }
  if (numSegments < 1 || numSegments > 100) {
    return calculator << "Bad number of segments: " << numSegments;
  }
  Expression vectorExpression = (rightE - leftE);
  Expression midPt = (rightE + leftE) / 2;
  Expression vectorX, vectorY;
  vectorX.makeXOX(
    calculator,
    calculator.opUnderscore(),
    vectorExpression,
    calculator.expressionOne()
  );
  vectorY.makeXOX(
    calculator,
    calculator.opUnderscore(),
    vectorExpression,
    calculator.expressionTwo()
  );
  Expression orthogonalVector;
  orthogonalVector.makeXOX(
    calculator, calculator.opSequence(), vectorY *(- 1), vectorX
  );
  Expression leftPt = midPt - orthogonalVector / 25;
  Expression rightPt = midPt + orthogonalVector / 25;
  output.reset(calculator);
  output.addChildAtomOnTop("PlotSegment");
  output.addChildOnTop(leftPt);
  output.addChildOnTop(rightPt);
  for (int i = 4; i < input.size(); i ++) {
    output.addChildOnTop(input[i]);
  }
  return true;
}

bool CalculatorFunctionsPlot::plotSegment(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsPlot::plotSegment");
  if (input.size() < 3) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  Vector<double> leftV, rightV;
  if (
    !calculator.getVectorDoubles(leftE, leftV) ||
    !calculator.getVectorDoubles(rightE, rightV)
  ) {
    return false;
  }
  if (leftV.size != rightV.size) {
    return false;
  }
  if (leftV.size != 2 && leftV.size != 3) {
    return false;
  }
  PlotObject segment;
  if (input.size() >= 4) {
    segment.colorJS = "black";
    segment.colorRGB = static_cast<int>(
      HtmlRoutines::redGreenBlue(0, 0, 0)
    );
    const Expression& colorE = input[3];
    if (!colorE.isOfType<std::string>(&segment.colorJS)) {
      segment.colorJS = colorE.toString();
    }
    if (
      !DrawingVariables::getColorIntFromColorString(
        segment.colorJS, segment.colorRGB
      )
    ) {
      calculator << "Unrecognized color: " << segment.colorJS;
    }
  }
  if (input.size() >= 5) {
    const Expression& lineWidthE = input[4];
    if (!lineWidthE.evaluatesToDouble(&segment.lineWidth)) {
      calculator
      << "Failed to extract line width from: "
      << lineWidthE.toString();
    }
    std::stringstream lineWidthStream;
    lineWidthStream.precision(4);
    lineWidthStream << segment.lineWidth;
    segment.lineWidthJS = lineWidthStream.str();
  }
  segment.plotType = "segment";
  if (leftV.size == 3) {
    segment.dimension = 3;
  } else {
    segment.dimension = 2;
  }
  segment.pointsDouble.addOnTop(leftV);
  segment.pointsDouble.addOnTop(rightV);
  if (input.size() >= 5) {
    if (!input[4].evaluatesToDouble(&segment.lineWidth)) {
      segment.lineWidth = 1;
    }
  }
  Plot plot;
  plot += segment;
  return output.assignValue(calculator, plot);
}

bool CalculatorFunctions::thaw(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::thaw");
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
  STACK_TRACE("CalculatorFunctions::leastCommonMultipleInteger");
  if (input.size() < 3) {
    return false;
  }
  Vector<LargeInteger> integers;
  if (
    !calculator.getVectorLargeIntegerFromFunctionArguments(input, integers)
  ) {
    return false;
  }
  if (integers.size < 1) {
    // <-this shouldn't happen if getVectorLargeIntegerFromFunctionArguments
    // works as intended.
    return false;
  }
  LargeIntegerUnsigned result = integers[0].value;
  if (result == 0) {
    return false;
  }
  for (int i = 1; i < integers.size; i ++) {
    if (integers[i].value == 0) {
      return false;
    }
    result =
    LargeIntegerUnsigned::leastCommonMultiple(result, integers[i].value);
  }
  return output.assignValue(calculator, result);
}

bool CalculatorFunctions::greatestCommonDivisorInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::greatestCommonDivisorInteger");
  if (input.size() < 3) {
    return false;
  }
  Vector<LargeInteger> integers;
  if (
    !calculator.getVectorLargeIntegerFromFunctionArguments(input, integers)
  ) {
    return false;
  }
  if (integers.size < 1) {
    // <-this shouldn't happen if getVectorLargeIntegerFromFunctionArguments
    // works as intended.
    return false;
  }
  LargeIntegerUnsigned result = integers[0].value;
  if (result == 0) {
    return false;
  }
  for (int i = 1; i < integers.size; i ++) {
    if (integers[i].value == 0) {
      return false;
    }
    result =
    LargeIntegerUnsigned::greatestCommonDivisor(
      result, integers[i].value
    );
  }
  return output.assignValue(calculator, result);
}

bool CalculatorFunctionsBasic::logarithmBaseNaturalToLn(
  Calculator& calculator, const Expression& input, Expression& output
) {
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
  STACK_TRACE("CalculatorFunctions::logarithmBaseSimpleCases");
  if (!input.startsWith(calculator.opLogBase(), 3)) {
    return false;
  }
  Rational base, argument;
  if (
    !input[1].isOfType<Rational>(&base) ||
    !input[2].isOfType<Rational>(&argument)
  ) {
    return false;
  }
  if (base < 0 || argument < 0) {
    return false;
  }
  if (base == 1) {
    return false;
  }
  if (base == 0 || argument == 0) {
    std::stringstream errorStream;
    errorStream
    << "Attempt to take logarithm base "
    << base
    << " of "
    << argument
    << " is not allowed. ";
    return output.assignError(calculator, errorStream.str());
  }
  if (argument == 1) {
    return output.assignValue(calculator, 0);
  }
  Expression newBaseE, newArgE;
  newBaseE.assignValue(calculator, base);
  newArgE.assignValue(calculator, argument);
  if (base < 1) {
    base.invert();
    newBaseE.assignValue(calculator, base);
    output.makeXOX(
      calculator, calculator.opLogBase(), newBaseE, newArgE
    );
    output *= - 1;
    return true;
  }
  if (argument < 1) {
    argument.invert();
    newArgE.assignValue(calculator, argument);
    output.makeXOX(
      calculator, calculator.opLogBase(), newBaseE, newArgE
    );
    output *= - 1;
    return true;
  }
  LargeInteger baseInt, argNum;
  if (!base.isInteger(&baseInt)) {
    return false;
  }
  LargeInteger simplerBase;
  int simplerPower = - 1;
  bool isPower = false;
  if (baseInt.tryIsPower(isPower, simplerBase, simplerPower)) {
    if (isPower) {
      newBaseE.assignValue(calculator, Rational(simplerBase));
      output.makeXOX(
        calculator, calculator.opLogBase(), newBaseE, newArgE
      );
      output /= simplerPower;
      return true;
    }
  }
  argNum = argument.getNumerator();
  LargeInteger argDen = argument.getDenominator();
  double doubleBase = baseInt.getDoubleValue();
  double doubleArgNum = argNum.getDoubleValue();
  if (
    FloatingPoint::logFloating(doubleArgNum) /
    FloatingPoint::logFloating(doubleBase) >
    1000
  ) {
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
  argument = argNum;
  argument /= argDen;
  newBaseE.assignValue(calculator, base);
  newArgE.assignValue(calculator, argument);
  output.makeXOX(calculator, calculator.opLogBase(), newBaseE, newArgE);
  if (intPart == 0) {
    return true;
  }
  output += intPart;
  return true;
}

std::string InputBox::getSliderName() const {
  return
  StringRoutines::convertStringToJavascriptVariable(
    this->name +
    Crypto::convertStringToHex(
      Crypto::computeSha256(this->name), 0, false
    )
  );
}

std::string InputBox::getUserInputBox() const {
  STACK_TRACE("InputBox::getUserInputBox");
  std::stringstream out;
  double reader = 0;
  out.precision(4);
  out << std::fixed;
  if (this->value.evaluatesToDouble(&reader)) {
    out
    << "\\formInput"
    << "{"
    << reader
    << ", "
    << this->getSliderName()
    << "}";
  } else {
    out
    << "\\formInput"
    << "{"
    << this->value.toString()
    << ", "
    << this->getSliderName()
    << "}";
  }
  return out.str();
}

bool CalculatorFunctions::makeJavascriptExpression(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::makeJavascriptExpression");
  if (input.size() != 2) {
    return false;
  }
  return
  CalculatorFunctions::functionMakeJavascriptExpression(
    calculator, input[1], output
  );
}

bool CalculatorFunctions::functionMakeJavascriptExpression(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::functionMakeJavascriptExpression");
  RecursionDepthCounter counter(&calculator.recursionDepth);
  if (calculator.recursionDepthExceededHandleRoughly()) {
    return false;
  }
  std::string atomString;
  if (input == calculator.expressionMinusInfinity()) {
    return output.assignValue<std::string>(calculator, "\"minusInfinity\"");
  }
  if (input == calculator.expressionInfinity()) {
    return output.assignValue<std::string>(calculator, "\"infinity\"");
  }
  if (input.isOperation(&atomString)) {
    if (input.isOperationGiven(calculator.opE())) {
      return output.assignValue<std::string>(calculator, " 2.718281828 ");
    }
    if (input.isOperationGiven(calculator.opPi())) {
      return output.assignValue<std::string>(calculator, " 3.141592654 ");
    }
    if (input.data >= calculator.numberOfPredefinedAtoms) {
      return
      output.assignValue(
        calculator, HtmlRoutines::getJavascriptVariable(atomString)
      );
    }
    if (
      atomString == "+" ||
      atomString == "*" ||
      atomString == "/" ||
      atomString == "-"
    ) {
      return output.assignValue(calculator, atomString);
    }
    return output.assignValue(calculator, atomString);
  }
  std::stringstream out;
  InputBox box;
  if (input.isOfType(&box)) {
    std::string name = box.getSliderName();
    out << name << " ";
    return output.assignValue(calculator, out.str());
  }
  out.precision(7);
  bool hasDoubleValue = false;
  double doubleValue = - 1;
  if (input.isOfType<Rational>()) {
    hasDoubleValue = true;
    doubleValue = input.getValue<Rational>().getDoubleValue();
  }
  if (input.isOfType<AlgebraicNumber>()) {
    hasDoubleValue =
    input.getValue<AlgebraicNumber>().evaluatesToDouble(&doubleValue);
  }
  if (input.isOfType<double>()) {
    hasDoubleValue = true;
    doubleValue = input.getValue<double>();
  }
  if (hasDoubleValue) {
    std::string doubleString = FloatingPoint::doubleToString(doubleValue);
    if (doubleString.size() > 0) {
      if (doubleString[0] == '-') {
        out << "(" << doubleString << ")";
      } else {
        out << " " << doubleString;
      }
    }
    return output.assignValue(calculator, out.str());
  }
  Expression operation, leftE, rightE;
  if (
    input.startsWith(calculator.opSequence()) ||
    input.startsWith(calculator.opIntervalOpen())
  ) {
    out << "[";
    for (int i = 1; i < input.size(); i ++) {
      if (
        !CalculatorFunctions::functionMakeJavascriptExpression(
          calculator, input[i], operation
        )
      ) {
        return
        output.assignValue(
          calculator, "(Failed to convert " + input[i].toString() + ")"
        );
      }
      out << operation.toString();
      if (i != input.size() - 1) {
        out << ", ";
      }
    }
    out << "]";
    return output.assignValue(calculator, out.str());
  }
  std::string opString, leftString, rightString;
  std::stringstream logStream;
  if (input.size() == 3 || input.size() == 2) {
    Expression* currentE = &operation;
    std::string* currentString = &opString;
    for (int i = 0; i < input.size(); i ++) {
      if (
        !CalculatorFunctions::functionMakeJavascriptExpression(
          calculator, input[i], *currentE
        )
      ) {
        return
        output.assignValue(
          calculator, "(Failed to convert " + input[i].toString() + ")"
        );
      }
      if (!currentE->isOfType(currentString)) {
        return
        output.assignValue(
          calculator, "(Failed to convert " + input[i].toString() + ")"
        );
      }
      logStream
      << "Converted: "
      << input[i].toString()
      << " to: "
      << *currentString
      << ". ";
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
      if (
        opString == "+" ||
        opString == "-" ||
        opString == "/" ||
        opString == "*"
      ) {
        out
        << "("
        << leftString
        << " "
        << opString
        << " "
        << rightString
        << ")";
        return output.assignValue(calculator, out.str());
      }
      if (opString == "\\sqrt") {
        out << "Math.pow(" << rightString << ", 1/" << leftString << ")";
        return output.assignValue(calculator, out.str());
      }
      if (opString == "^") {
        out << "Math.pow(" << leftString << ", " << rightString << ")";
        return output.assignValue(calculator, out.str());
      }
      if (opString == "LogBase") {
        out
        << "Math.log("
        << rightString
        << ") / Math.log("
        << leftString
        << ")";
        return output.assignValue(calculator, out.str());
      }
    }
    if (input.size() == 2) {
      std::string functionName = "";
      if (
        opString == "\\sin" ||
        opString == "\\cos" ||
        opString == "\\log" ||
        opString == "\\tan" ||
        opString == "\\cot"
      ) {
        functionName = opString.substr(1);
      }
      if (opString == "\\arccos") {
        functionName = "acos";
      }
      if (opString == "\\arcsin") {
        functionName = "asin";
      }
      if (opString == "\\arctan") {
        functionName = "atan";
      }
      if (functionName != "") {
        out << "(Math." << functionName << "( " << leftString << "))";
        return output.assignValue(calculator, out.str());
      }
    }
    if (input.size() == 2) {
      if (opString == "|") {
        out << "(Math.abs( " << leftString << "))";
        return output.assignValue(calculator, out.str());
      }
    }
  }
  out
  << "(Failed to make expression from "
  << input.toString()
  << ". "
  << logStream.str()
  << ")";
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctionsPlot::plotSetProjectionScreenBasis(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsPlot::plotSetProjectionScreenBasis");
  if (input.size() != 3) {
    return false;
  }
  Vector<double> v1, v2;
  if (
    !calculator.getVectorDoubles(input[1], v1, 3) ||
    !calculator.getVectorDoubles(input[2], v2, 3)
  ) {
    return
    calculator
    << "Failed to extract 3d-vectors from "
    << input[1].toString()
    << ", "
    << input[2].toString()
    << ".";
  }
  Plot resultPlot;
  resultPlot.dimension = 3;
  PlotObject plot;
  plot.plotType = "setProjectionScreen";
  plot.pointsDouble.addOnTop(v1);
  plot.pointsDouble.addOnTop(v2);
  resultPlot += plot;
  return output.assignValue(calculator, resultPlot);
}

bool CalculatorFunctionsPlot::plotCoordinateSystem(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsPlot::plotCoordinateSystem");
  if (input.size() != 3) {
    return false;
  }
  Vector<double> corner1, corner2;
  if (
    !calculator.getVectorDoubles(input[1], corner1, 3) ||
    !calculator.getVectorDoubles(input[2], corner2, 3)
  ) {
    return
    calculator
    << "Failed to extract 3d-vectors from "
    << input[1].toString()
    << ", "
    << input[2].toString()
    << ".";
  }
  Plot resultPlot;
  resultPlot.dimension = 3;
  PlotObject plot;
  plot.colorJS = "black";
  plot.plotType = "segment";
  plot.pointsDouble.setSize(2);
  for (int i = 0; i < 3; i ++) {
    plot.pointsDouble[0].makeZero(3);
    plot.pointsDouble[1].makeZero(3);
    plot.pointsDouble[0][i] = corner1[i];
    plot.pointsDouble[1][i] = corner2[i];
    resultPlot += plot;
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
  return output.assignValue(calculator, resultPlot);
}

bool CalculatorFunctionsPlot::plotSurface(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsPlot::plotSurface");
  PlotObject plot;
  bool found = false;
  for (int i = 0; i < input.size(); i ++) {
    if (input[i].isSequenceNElements(3)) {
      plot.manifoldImmersion = input[i];
      found = true;
      break;
    }
  }
  if (!found) {
    return
    calculator
    << "Could not find a triple of "
    << "functions expressions to use for "
    << "the surface. ";
  }
  plot.manifoldImmersion.getFreeVariables(plot.variablesInPlay, true);
  if (plot.variablesInPlay.size > 2) {
    return
    calculator
    << "Got a surface with "
    << plot.variablesInPlay.size
    << " variables, namely: "
    << plot.variablesInPlay.toStringCommaDelimited()
    << ". I've been taught to plot 2d surfaces only. "
    << "Please reduce the number of variables to 2. ";
  }
  Expression uE, vE;
  uE.makeAtom(calculator, "u");
  vE.makeAtom(calculator, "v");
  if (plot.variablesInPlay.size == 1) {
    if (plot.variablesInPlay.contains(vE)) {
      plot.variablesInPlay.addOnTop(uE);
    } else {
      plot.variablesInPlay.addOnTop(vE);
    }
  }
  if (plot.variablesInPlay.size == 0) {
    plot.variablesInPlay.addOnTop(uE);
    plot.variablesInPlay.addOnTop(vE);
  }
  plot.variablesInPlay.quickSortAscending();
  plot.coordinateFunctionsE.setSize(plot.manifoldImmersion.size() - 1);
  plot.coordinateFunctionsJS.setSize(plot.coordinateFunctionsE.size);
  plot.variableRangesJS.setSize(2);
  plot.variablesInPlayJS.setSize(2);
  for (int i = 0; i < 2; i ++) {
    plot.variableRangesJS[i].setSize(2);
    plot.variablesInPlayJS[i] = plot.variablesInPlay[i].toString();
  }
  Expression jsConverter;
  for (int i = 1; i < plot.manifoldImmersion.size(); i ++) {
    plot.coordinateFunctionsE[i - 1] = plot.manifoldImmersion[i];
    bool isGood =
    CalculatorFunctions::functionMakeJavascriptExpression(
      calculator, plot.coordinateFunctionsE[i - 1], jsConverter
    );
    if (isGood) {
      isGood =
      jsConverter.isOfType<std::string>(
        &plot.coordinateFunctionsJS[i - 1]
      );
    }
    if (!isGood) {
      return
      calculator
      << "Failed to convert "
      << plot.coordinateFunctionsE[i - 1].toString()
      << " to a javascript expression. ";
    }
  }
  for (int i = 1; i < input.size(); i ++) {
    if (input[i].startsWith(calculator.opIn(), 3)) {
      int index = plot.variablesInPlay.getIndex(input[i][1]);
      if (index < 0 || index > 2) {
        // index > 2 should never happen
        continue;
      }
      if (input[i][2].size() != 3) {
        continue;
      }
      for (int j = 0; j < 2; j ++) {
        bool isGood =
        CalculatorFunctions::functionMakeJavascriptExpression(
          calculator, input[i][2][j + 1], jsConverter
        );
        if (isGood) {
          isGood =
          jsConverter.isOfType<std::string>(
            &plot.variableRangesJS[index][j]
          );
        }
        if (!isGood) {
          return
          calculator
          << "Failed to convert "
          << input[i][2][j + 1].toString()
          << " to a javascript expression. ";
        }
      }
    }
  }
  MapList<
    std::string,
    Expression,
    HashFunctions::hashFunction<std::string>
  > keys;
  if (
    CalculatorConversions::loadKeysFromStatementList(
      calculator, input, keys, &calculator.comments, true
    )
  ) {
    if (keys.contains("color1")) {
      plot.colorUV = keys.getValueCreateEmpty("color1").toString();
    }
    if (keys.contains("color2")) {
      plot.colorVU = keys.getValueCreateEmpty("color2").toString();
    }
    MapList<
      std::string,
      std::string,
      HashFunctions::hashFunction<std::string>
    > keysToConvert;
    keysToConvert.getValueCreateEmpty("numSegments1");
    keysToConvert.getValueCreateEmpty("numSegments2");
    keysToConvert.getValueCreateEmpty("lineWidth");
    for (int i = 0; i < keysToConvert.size(); i ++) {
      if (!keys.contains(keysToConvert.keys[i])) {
        continue;
      }
      Expression expressionToConvert =
      keys.getValueCreateEmpty(keysToConvert.keys[i]);
      bool isGood =
      CalculatorFunctions::functionMakeJavascriptExpression(
        calculator, expressionToConvert, jsConverter
      );
      if (isGood) {
        isGood =
        jsConverter.isOfType<std::string>(&keysToConvert.values[i]);
      }
      if (!isGood) {
        return
        calculator
        << "Failed to convert "
        << expressionToConvert.toString()
        << " to a javascript expression. ";
      }
    }
    plot.numberOfSegmentsJS.setSize(2);
    if (keysToConvert.getValueCreateEmpty("numSegments1") != "") {
      plot.numberOfSegmentsJS[0] =
      keysToConvert.getValueCreateEmpty("numSegments1");
    }
    if (keysToConvert.getValueCreateEmpty("numSegments2") != "") {
      plot.numberOfSegmentsJS[1] =
      keysToConvert.getValueCreateEmpty("numSegments2");
    }
    if (keysToConvert.getValueCreateEmpty("lineWidth") != "") {
      plot.lineWidthJS = keysToConvert.getValueCreateEmpty("lineWidth");
    }
  }
  if (
    plot.variableRangesJS[0][0] == "" ||
    plot.variableRangesJS[0][1] == "" ||
    plot.variableRangesJS[1][0] == "" ||
    plot.variableRangesJS[1][1] == ""
  ) {
    return
    calculator
    << "Could not extract variable ranges, got the var ranges: "
    << plot.variableRangesJS;
  }
  plot.plotType = "surface";
  plot.dimension = plot.coordinateFunctionsE.size;
  input.hasInputBoxVariables(
    &plot.parametersInPlay, &plot.parametersInPlayJS
  );
  Plot result;
  result += plot;
  return output.assignValue(calculator, result);
}

class QRFactorizationComputation {};
