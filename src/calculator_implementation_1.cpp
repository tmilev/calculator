// The current file is licensed under the license terms found in the main
// header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator_interface.h"
#include "calculator_weyl_group_characters.h"
#include "calculator_inner_typed_functions.h"
#include "math_general_implementation.h"
#include "math_extra_semisimple_lie_algebras.h"
#include "math_extra_universal_enveloping_implementation.h"
#include "math_general_polynomial_computations_basic_implementation.h"
#include "math_general_polynomial_computations_advanced_implementation.h"
#include "math_extra_semisimple_lie_algebras_implementation.h"

// undefined reference to `WeylGroupRepresentation<Rational>::reset()
#include "math_extra_finite_groups_implementation.h"
#include "math_rational_function_implementation.h"
#include "string_constants.h"
#include <cmath>
#include <cfloat>

std::string PlotObject::Labels::point = "point";
std::string PlotObject::Labels::path = "path";
std::string PlotObject::Labels::functionLabel = "function";
std::string PlotObject::Labels::coordinateFunctions = "coordinateFunctions";
std::string PlotObject::Labels::left = "left";
std::string PlotObject::Labels::right = "right";
std::string PlotObject::Labels::numberOfSegments = "numberOfSegments";
std::string PlotObject::Labels::color = "color";
std::string PlotObject::Labels::colorFront = "colorFront";
std::string PlotObject::Labels::colorBack = "colorBack";
std::string PlotObject::Labels::colorContour = "colorContour";
std::string PlotObject::Labels::colorFill = "colorFill";
std::string PlotObject::Labels::lineWidth = "lineWidth";
std::string PlotObject::Labels::manifoldImmersion = "manifoldImmersion";
std::string PlotObject::Labels::variableRange = "variableRange";
std::string PlotObject::Labels::defaultLength = "defaultLength";
std::string PlotObject::Labels::plotType = "plotType";
std::string PlotObject::Labels::body = "body";
std::string PlotObject::Labels::text = "text";
std::string PlotObject::Labels::mandelbrotMode = "mandelbrotMode";
std::string PlotObject::Labels::arguments = "arguments";
std::string PlotObject::Labels::parameters = "parameters";
std::string PlotObject::Labels::parameterLetter = "parameterLetter";
std::string PlotObject::Labels::parametersOnTheGraph = "parametersOnTheGraph";
std::string PlotObject::Labels::parametersOnTheGraphLetter =
"parametersOnTheGraphLetter";
std::string PlotObject::Labels::viewWindow = "viewWindow";
std::string PlotObject::PlotTypes::escapeMap = "escapeMap";
std::string PlotObject::PlotTypes::parametricCurve = "parametricCurve";
std::string PlotObject::PlotTypes::plotFunction = "plotFunction";
std::string PlotObject::PlotTypes::points = "points";
std::string PlotObject::PlotTypes::segment = "segment";
std::string PlotObject::PlotTypes::plotFillStart = "plotFillStart";
std::string PlotObject::PlotTypes::plotFillFinish = "plotFillFinish";
std::string PlotObject::PlotTypes::pathFilled = "pathFilled";
std::string PlotObject::PlotTypes::label = "label";
std::string PlotObject::PlotTypes::axesGrid = "axesGrid";
std::string Plot::Labels::canvasName = "canvasName";
std::string Plot::Labels::controlsName = "controlsName";
std::string Plot::Labels::messagesName = "messagesName";
std::string Plot::Labels::graphicsType = "graphicsType";
std::string Plot::Labels::graphicsThreeDimensional =
"graphicsThreeDimensional";
std::string Plot::Labels::graphicsTwoDimensional = "graphicsTwoDimensional";
std::string Plot::Labels::plotObjects = "plotObjects";
std::string GlobalVariables::webAdress = "";
bool DynkinSimpleType::hasPrecomputedSubalgebras() const {
  if (this->letter == 'F') {
    return true;
  }
  if (this->letter == 'G') {
    return true;
  }
  if (this->letter == 'A' && this->rank <= 6) {
    return true;
  }
  if (this->letter == 'B' && this->rank <= 4) {
    return true;
  }
  if (this->letter == 'D' && this->rank <= 4) {
    return true;
  }
  if (this->letter == 'C' && this->rank <= 5) {
    return true;
  }
  if (this->letter == 'E' && this->rank == 6) {
    return true;
  }
  return false;
}

std::string Calculator::toStringSemismipleLieAlgebraLinksFromHardDrive(
  const std::string& prefixFolder,
  const DynkinType& dynkinType,
  FormatExpressions* format
) {
  (void) format;
  std::stringstream out;
  SemisimpleLieAlgebra folderComputer;
  folderComputer.weylGroup.dynkinType = dynkinType;
  out
  << "<tr><td><a href='"
  << folderComputer.fileNames.displayFolderName(prefixFolder)
  << folderComputer.fileNames.fileNameNoPathStructureConstants()
  << "'>"
  << dynkinType[0].letter
  << dynkinType[0].rank
  << " structure constants</a></td>\n ";
  if (dynkinType[0].hasPrecomputedSubalgebras()) {
    out
    << "<td><a href='"
    << folderComputer.fileNames.displayFolderName(prefixFolder)
    << folderComputer.fileNames.fileNameNoPathSemisimpleSubalgebras()
    << "'>"
    << dynkinType[0].letter
    << dynkinType[0].rank
    << " semisimple subalgebras</a>";
    out << "</td>\n ";
  } else {
    out << "<td>Not available</td>\n";
  }
  out
  << "<td><a href='"
  << folderComputer.fileNames.displayFolderName(prefixFolder)
  << folderComputer.fileNames.fileNameRelativePathSlTwoSubalgebras()
  << "'>"
  << dynkinType[0].letter
  << dynkinType[0].rank
  << " sl(2) triples</a></td>\n";
  out
  << "<td><a href='"
  << folderComputer.fileNames.displayFolderName(prefixFolder)
  << folderComputer.fileNames.fileNameNoPathRootSubalgebras()
  << "'>"
  << dynkinType[0].letter
  << dynkinType[0].rank
  << " root subalgebras</a></td>\n";
  return out.str();
}

void Plot::operator+=(const Plot& other) {
  STACK_TRACE("Plot::operator+=");
  if (other.priorityCanvasName > this->priorityCanvasName) {
    this->setCanvasName(other.getCanvasName());
  } else if (
    this->getCanvasName() == "" &&
    this->priorityCanvasName == other.priorityCanvasName
  ) {
    this->setCanvasName(other.getCanvasName());
  }
  if (other.priorityCanvasName > this->priorityCanvasName) {
    this->priorityCanvasName = other.priorityCanvasName;
  }
  if (this->priorityCanvasName == 0) {
    this->setCanvasName("");
  }
  if (this->dimension == - 1) {
    this->dimension = other.dimension;
  }
  if (other.priorityViewRectangle > this->priorityViewRectangle) {
    this->highY = other.highY;
    this->lowY = other.lowY;
    this->lowX = other.lowX;
    this->highX = other.highX;
  }
  if (other.priorityViewRectangle == this->priorityViewRectangle) {
    this->highY = MathRoutines::maximum(this->highY, other.highY);
    this->lowY = MathRoutines::minimum(this->lowY, other.lowY);
    this->highX = MathRoutines::maximum(this->highX, other.highX);
    this->lowX = MathRoutines::minimum(this->lowX, other.lowX);
  }
  this->plotObjects.addListOnTop(other.plotObjects);
  if (other.priorityWindow > this->priorityWindow) {
    this->desiredHtmlHeightInPixels = other.desiredHtmlHeightInPixels;
    this->desiredHtmlWidthInPixels = other.desiredHtmlWidthInPixels;
  } else if (this->priorityWindow == other.priorityWindow) {
    if (this->desiredHtmlHeightInPixels < other.desiredHtmlHeightInPixels) {
      this->desiredHtmlHeightInPixels = other.desiredHtmlHeightInPixels;
    }
    if (this->desiredHtmlWidthInPixels < other.desiredHtmlWidthInPixels) {
      this->desiredHtmlWidthInPixels = other.desiredHtmlWidthInPixels;
    }
  }
  if (!other.flagIncludeCoordinateSystem) {
    this->flagIncludeCoordinateSystem = false;
  }
  this->parameterNamesJS.addOnTopNoRepetition(other.parameterNamesJS);
  this->priorityWindow =
  MathRoutines::maximum(this->priorityWindow, other.priorityWindow);
  this->priorityViewRectangle =
  MathRoutines::maximum(
    this->priorityViewRectangle, other.priorityViewRectangle
  );
}

bool Plot::operator==(const Plot& other) const {
  return
  this->priorityWindow == other.priorityWindow &&
  this->priorityCanvasName == other.priorityCanvasName &&
  this->priorityViewRectangle == other.priorityViewRectangle &&
  this->desiredHtmlHeightInPixels == other.desiredHtmlHeightInPixels &&
  this->desiredHtmlWidthInPixels == other.desiredHtmlWidthInPixels && ((
      this->highY - other.highY
    ) ==
    0.0
  ) && ((this->lowY - other.lowY) == 0.0) && ((this->lowX - other.lowX) == 0.0
  ) && ((this->highX - other.highX) == 0.0) &&
  this->plotObjects == other.plotObjects &&
  this->parameterNames == other.parameterNames &&
  this->parameterNamesJS == other.parameterNamesJS &&
  this->getCanvasName() == other.getCanvasName() &&
  this->dimension == other.dimension &&
  this->flagIncludeCoordinateSystem == other.flagIncludeCoordinateSystem;
}

void Plot::operator+=(const PlotObject& other) {
  if (
    other.dimension != - 1 &&
    this->dimension != - 1 &&
    this->dimension != other.dimension
  ) {
    global.fatal
    << "Attempting to add plot of dimension: "
    << this->dimension
    << " to a plot of dimension: "
    << other.dimension
    << ". "
    << global.fatal;
  }
  if (this->dimension == - 1) {
    this->dimension = other.dimension;
  }
  this->plotObjects.addOnTop(other);
  this->parameterNames.addOnTopNoRepetition(other.parametersInPlay);
  this->parameterNamesJS.addOnTopNoRepetition(other.parametersInPlayJS);
  this->setCanvasName("");
}

bool PlotObject::operator==(const PlotObject& other) const {
  return ((this->xLow - other.xLow) == 0.0) && ((this->xHigh - other.xHigh) ==
    0.0
  ) && ((this->yLow - other.yLow) == 0.0) && ((this->yHigh - other.yHigh) ==
    0.0
  ) && ((this->paramLow - other.paramLow) == 0.0) && ((
      this->paramHigh - other.paramHigh
    ) ==
    0.0
  ) && ((this->lineWidth - other.lineWidth) == 0.0) &&
  this->plotString == other.plotString &&
  this->fillStyle == other.fillStyle &&
  this->plotStringWithHtml == other.plotStringWithHtml &&
  this->colorRGB == other.colorRGB &&
  this->colorFillRGB == other.colorFillRGB &&
  this->dimension == other.dimension &&
  this->colorUV == other.colorUV &&
  this->colorVU == other.colorVU &&
  this->colorJS == other.colorJS &&
  this->lineWidthJS == other.lineWidthJS &&
  this->numberOfSegmentsJS == other.numberOfSegmentsJS &&
  this->points == other.points &&
  this->pointsDouble == other.pointsDouble &&
  this->pointsJS == other.pointsJS &&
  this->rectangles == other.rectangles &&
  this->plotType == other.plotType &&
  this->manifoldImmersion == other.manifoldImmersion &&
  this->coordinateFunctionsE == other.coordinateFunctionsE &&
  this->coordinateFunctionsJS == other.coordinateFunctionsJS &&
  this->variablesInPlay == other.variablesInPlay &&
  this->variableRangesJS == other.variableRangesJS &&
  this->leftPoint == other.leftPoint &&
  this->rightPoint == other.rightPoint &&
  this->paramLowE == other.paramLowE &&
  this->paramHighE == other.paramHighE &&
  this->numSegmentsE == other.numSegmentsE &&
  this->variablesInPlayJS == other.variablesInPlayJS &&
  this->leftBoundaryIsMinusInfinity == other.leftBoundaryIsMinusInfinity &&
  this->rightBoundaryIsMinusInfinity == other.rightBoundaryIsMinusInfinity &&
  this->leftPtJS == other.leftPtJS &&
  this->rightPtJS == other.rightPtJS &&
  this->colorFillJS == other.colorFillJS &&
  this->paramLowJS == other.paramLowJS &&
  this->paramHighJS == other.paramHighJS &&
  this->defaultLengthJS == other.defaultLengthJS &&
  this->parametersInPlayJS == other.parametersInPlayJS;
}

PlotObject::PlotObject() {
  this->xLow = 0;
  this->xHigh = 0;
  this->paramLow = 0;
  this->paramHigh = 0;
  this->yLow = 0;
  this->yHigh = 0;
  this->colorRGB = 0;
  this->lineWidth = 1;
  this->colorFillRGB = 0;
  this->dimension = - 1;
  this->rightBoundaryIsMinusInfinity = false;
  this->leftBoundaryIsMinusInfinity = false;
}

void PlotObject::computeYBounds() {
  STACK_TRACE("PlotObject::computeYBounds");
  for (int i = 0; i < this->pointsDouble.size; i ++) {
    this->yHigh =
    MathRoutines::maximum(this->yHigh, this->pointsDouble[i][1]);
    this->yLow =
    MathRoutines::minimum(this->yLow, this->pointsDouble[i][1]);
  }
}

std::string PlotObject::getPlotStringFromFunctionStringAndRanges(
  bool useHtml,
  const std::string& functionStringPostfixNotation,
  const std::string& functionStringCalculatorFormat,
  double inputLowerBound,
  double inputUpperBound
) {
  std::stringstream out;
  if (useHtml) {
    out << "<br>";
  }
  out << "\n\n%Function formula: " << functionStringCalculatorFormat << "\n\n";
  if (useHtml) {
    out << "<br>";
  }
  out
  << "\\psplot[linecolor =\\fcColorGraph, plotpoints =1000]{"
  << FloatingPoint::doubleToString(inputLowerBound)
  << "}{"
  << FloatingPoint::doubleToString(inputUpperBound)
  << "}{";
  out << functionStringPostfixNotation << "}";
  return out.str();
}

Plot::Plot() {
  this->lowX = - 0.5;
  this->highX = 1;
  this->lowY = - 0.5;
  this->highY = 0.5;
  this->flagIncludeExtraHtmlDescriptions = true;
  this->desiredHtmlHeightInPixels = 350;
  this->desiredHtmlWidthInPixels = 350;
  this->defaultLineColor = 0;
  this->flagPlotShowJavascriptOnly = false;
  this->priorityWindow = 0;
  this->priorityViewRectangle = 0;
  this->priorityCanvasName = 0;
  this->dimension = - 1;
  this->flagDivAlreadyDisplayed = false;
  this->flagIncludeCoordinateSystem = true;
}

void Plot::computeAxesAndBoundingBox() {
  STACK_TRACE("Plot::computeAxesAndBoundingBox");
  this->lowX = - 0.5;
  this->highX = 1.1;
  this->lowY = - 0.5;
  this->highY = 1.1;
  for (int k = 0; k < this->plotObjects.size; k ++) {
    this->plotObjects[k].computeYBounds();
    this->lowX = MathRoutines::minimum(this->plotObjects[k].xLow, lowX);
    this->highX =
    MathRoutines::maximum(this->plotObjects[k].xHigh, highX);
    this->lowY =
    MathRoutines::minimum(this->plotObjects[k].yLow, this->lowY);
    this->highY =
    MathRoutines::maximum(this->plotObjects[k].yHigh, this->highY);
    for (
      int j = 0; j < this->plotObjects[k].pointsDouble.size; j ++
    ) {
      Vector<double>& currentPoint = this->plotObjects[k].pointsDouble[j];
      if (!this->isOKVector(currentPoint)) {
        continue;
      }
      this->lowX = MathRoutines::minimum(this->lowX, currentPoint[0]);
      this->highX = MathRoutines::maximum(this->highX, currentPoint[0]);
      this->lowY = MathRoutines::minimum(currentPoint[1], this->lowY);
      this->highY = MathRoutines::maximum(currentPoint[1], this->highY);
    }
  }
}

void Plot::computeAxesAndBoundingBox3d() {
  STACK_TRACE("Plot::computeAxesAndBoundingBox3d");
  this->lowX = - 0.5;
  this->highX = 1.1;
  this->lowY = - 0.5;
  this->highY = 1.1;
  for (int k = 0; k < this->plotObjects.size; k ++) {
    this->plotObjects[k].computeYBounds();
    this->lowX = MathRoutines::minimum(this->plotObjects[k].xLow, lowX);
    this->highX =
    MathRoutines::maximum(this->plotObjects[k].xHigh, highX);
    this->lowY =
    MathRoutines::minimum(this->plotObjects[k].yLow, this->lowY);
    this->highY =
    MathRoutines::maximum(this->plotObjects[k].yHigh, this->highY);
    for (
      int j = 0; j < this->plotObjects[k].pointsDouble.size; j ++
    ) {
      Vector<double>& currentPoint = this->plotObjects[k].pointsDouble[j];
      if (!this->isOKVector(currentPoint)) {
        continue;
      }
      this->lowX = MathRoutines::minimum(this->lowX, currentPoint[0]);
      this->highX = MathRoutines::maximum(this->highX, currentPoint[0]);
      this->lowY = MathRoutines::minimum(currentPoint[1], this->lowY);
      this->highY = MathRoutines::maximum(currentPoint[1], this->highY);
    }
  }
}

List<PlotObject>& Plot::getPlots() {
  return this->plotObjects;
}

void Plot::addPlotOnTop(PlotObject& input) {
  *this += input;
}

void PlotObject::makeSegment(
  const Vector<Rational>& left, const Vector<Rational>& right
) {
  this->pointsDouble.addOnTop(left.getVectorDouble());
  this->pointsDouble.addOnTop(right.getVectorDouble());
  this->plotType = PlotObject::PlotTypes::segment;
}

void PlotObject::makeLabel(
  const Vector<Rational>& position, const std::string& label
) {
  this->makeLabel(position.getVectorDouble(), label);
}

void PlotObject::makeRectangle(
  double xLowLeft,
  double yLowLeft,
  double width,
  double height,
  const std::string& color
) {
  this->plotType = PlotObject::PlotTypes::pathFilled;
  Vector<double> current;
  current.addOnTop(xLowLeft);
  current.addOnTop(yLowLeft);
  this->pointsDouble.addOnTop(current);
  current[0] += width;
  this->pointsDouble.addOnTop(current);
  current[1] += height;
  this->pointsDouble.addOnTop(current);
  current[0] -= width;
  this->pointsDouble.addOnTop(current);
  current[1] -= height;
  this->pointsDouble.addOnTop(current);
  this->colorFillJS = color;
}

void PlotObject::makeLabel(
  const Vector<double>& position, const std::string& label
) {
  this->dimension = position.size;
  this->plotString = label;
  this->pointsDouble.addOnTop(position);
  this->colorJS = "black";
  this->plotType = PlotObject::PlotTypes::label;
}

void PlotObject::makePlotFillStart() {
  this->plotType = PlotObject::PlotTypes::plotFillStart;
}

void PlotObject::makePlotFillFinish() {
  this->plotType = PlotObject::PlotTypes::plotFillFinish;
}

void PlotObject::makeCircle(
  const Vector<Rational>& center,
  const Rational& radius,
  const std::string& color
) {
  if (center.size < 2) {
    global.fatal
    << "The circle dimension must be at least two. "
    << global.fatal;
  }
  this->variableRangesJS.setSize(1);
  this->variableRangesJS[0].setSize(2);
  this->variableRangesJS[0][0] = "0";
  this->variableRangesJS[0][1] = "3.14159";
  this->paramLowJS = "0";
  this->paramHighJS = "2*3.14159";
  this->numberOfSegmentsJS.setSize(1);
  this->numberOfSegmentsJS[0] = "30";
  this->coordinateFunctionsJS.setSize(2);
  this->variablesInPlayJS.addOnTop("t");
  std::string radiusString = StringRoutines::toStringElement(radius);
  this->coordinateFunctionsJS[0] =
  center[0].toString() + "+" + radiusString + "*Math.cos(t)";
  this->coordinateFunctionsJS[1] =
  center[1].toString() + "+" + radiusString + "*Math.sin(t)";
  this->colorJS = color;
  this->plotType = PlotObject::PlotTypes::parametricCurve;
}

void PlotObject::makeEscapeMap(
  const Expression& functionX,
  const std::string& javascriptX,
  const std::string& variableX,
  const Expression& functionY,
  const std::string& javascriptY,
  const std::string& variableY,
  const std::string& inputParameterLetter,
  const List<std::string>& parametersInPlayJS,
  const MapList<std::string, double, HashFunctions::hashFunction>&
  inputParametersOnTheGraph,
  const std::string& inputParametersOnTheGraphLetter,
  bool mandelbrotMode
) {
  this->coordinateFunctionsJS.addOnTop(javascriptX);
  this->coordinateFunctionsJS.addOnTop(javascriptY);
  this->coordinateFunctionsE.addOnTop(functionX);
  this->coordinateFunctionsE.addOnTop(functionY);
  this->variablesInPlayJS.addOnTop(variableX);
  this->variablesInPlayJS.addOnTop(variableY);
  this->plotType = PlotObject::PlotTypes::escapeMap;
  this->parametersInPlayJS = parametersInPlayJS;
  this->parametersOnTheGraph = inputParametersOnTheGraph;
  this->parameterLetter = inputParameterLetter;
  this->parametersOnTheGraphLetter = inputParametersOnTheGraphLetter;
  this->extraFlags.setKeyValue(
    PlotObject::Labels::mandelbrotMode, mandelbrotMode
  );
}

void Plot::drawCoordinateAxes() {
  this->flagIncludeCoordinateSystem = true;
}

void Plot::drawGrid() {
  PlotObject plot;
  plot.plotType = PlotObject::PlotTypes::axesGrid;
  plot.dimension = 2;
  this->addPlotOnTop(plot);
  this->dimension = 2;
}

void Plot::drawPlotFillStart() {
  PlotObject plot;
  plot.plotType = PlotObject::PlotTypes::plotFillStart;
  this->addPlotOnTop(plot);
  this->dimension = 2;
}

void Plot::drawPlotFillFinish() {
  PlotObject plot;
  plot.plotType = PlotObject::PlotTypes::plotFillFinish;
  this->addPlotOnTop(plot);
  this->dimension = 2;
}

void Plot::drawRectangle(
  double xLowLeft,
  double yLowLeft,
  double width,
  double height,
  const std::string& color
) {
  PlotObject plot;
  plot.makeRectangle(xLowLeft, yLowLeft, width, height, color);
  this->addPlotOnTop(plot);
}

void Plot::setViewWindow(
  double inputLowX, double inputLowY, double inputHighX, double inputHighY
) {
  this->dimension = 2;
  this->lowX = inputLowX;
  this->lowY = inputLowY;
  this->highX = inputHighX;
  this->highY = inputHighY;
  this->priorityViewRectangle = 1;
}

void Plot::drawEscapeMap(
  Expression& functionX,
  const std::string& javascriptX,
  const std::string& variableX,
  Expression& functionY,
  const std::string& javascriptY,
  const std::string& variableY,
  const std::string& parameterLetter,
  List<std::string>& parametersInPlayJS,
  const MapList<std::string, double, HashFunctions::hashFunction>&
  parametersOnTheGraph,
  const std::string& parametersOnTheGraphLetter,
  bool mandelbrotMode
) {
  PlotObject plot;
  plot.makeEscapeMap(
    functionX,
    javascriptX,
    variableX,
    functionY,
    javascriptY,
    variableY,
    parameterLetter,
    parametersInPlayJS,
    parametersOnTheGraph,
    parametersOnTheGraphLetter,
    mandelbrotMode
  );
  this->addPlotOnTop(plot);
}

void Plot::drawCircle(
  const Vector<Rational>& center,
  const Rational& radius,
  const std::string& color,
  bool filled
) {
  PlotObject circle;
  circle.makeCircle(center, radius, color);
  if (filled) {
    PlotObject plotFillStart;
    plotFillStart.makePlotFillStart();
    this->addPlotOnTop(plotFillStart);
  }
  this->addPlotOnTop(circle);
  if (filled) {
    PlotObject plotFillFinish;
    plotFillFinish.makePlotFillFinish();
    this->addPlotOnTop(plotFillFinish);
  }
}

void Plot::drawSegment(
  const Vector<Rational>& left, const Vector<Rational>& right
) {
  PlotObject segment;
  segment.makeSegment(left, right);
  this->addPlotOnTop(segment);
}

void Plot::drawLabel(
  const Vector<Rational>& position, const std::string& label
) {
  PlotObject labelPlot;
  labelPlot.makeLabel(position, label);
  this->addPlotOnTop(labelPlot);
}

void Plot::addPlotsOnTop(Plot& input) {
  this->plotObjects.addListOnTop(input.plotObjects);
  this->parameterNames.addOnTopNoRepetition(input.parameterNames);
  this->parameterNamesJS.addOnTopNoRepetition(input.parameterNamesJS);
}

void Plot::clearPlotObjects() {
  this->plotObjects.clear();
  this->parameterNames.clear();
  this->parameterNamesJS.clear();
}

void Plot::setExpectedPlotObjects(int expectedSize) {
  this->plotObjects.setExpectedSize(expectedSize);
}

bool Plot::isOKVector(const Vector<double>& input) {
  for (int i = 0; i < input.size; i ++) {
    if (FloatingPoint::isNaN(input[i])) {
      return false;
    }
  }
  return true;
}

std::string Plot::getPlotHtml3d(Calculator& owner) {
  STACK_TRACE("Plot::getPlotHtml3d");
  owner.flagHasGraphics = true;
  this->computeCanvasNameIfNecessary(
    owner.objectContainer.canvasPlotCounter
  );
  JSData result;
  std::stringstream out;
  if (!owner.flagPlotShowJavascriptOnly) {
    std::string canvasId = this->getCanvasName();
    std::string controlsId = canvasId + "Controls";
    std::string messagesId = canvasId + "Messages";
    out
    << "<canvas width='"
    << this->desiredHtmlWidthInPixels
    << "' height='"
    << this->desiredHtmlHeightInPixels
    << "' "
    << "style='border:solid 1px' name='"
    << canvasId
    << "'"
    << ">"
    << "Your browser does not support the HTML5 canvas tag.</canvas><br>"
    << "<span name='"
    << controlsId
    << "'></span>"
    << "<span name='"
    << messagesId
    << "'></span>";
    result[Plot::Labels::canvasName] = this->getCanvasName();
    result[Plot::Labels::controlsName] = controlsId;
    result[Plot::Labels::messagesName] = messagesId;
  }
  JSData plotUpdaters;
  for (int i = 0; i < this->parameterNames.size; i ++) {
    InputBox& currentBox =
    owner.objectContainer.userInputTextBoxesWithValues.getValueCreateEmpty(
      this->parameterNames[i]
    );
    plotUpdaters[currentBox.getSliderName()] = this->getCanvasName();
  }
  result["plotUpdaters"] = plotUpdaters;
  JSData objects3D = JSData::makeEmptyArray();
  for (int i = 0; i < this->plotObjects.size; i ++) {
    PlotObject& current = this->plotObjects[i];
    objects3D.listObjects.addOnTop(current.toJSON());
  }
  result[Plot::Labels::plotObjects] = objects3D;
  result["showPerformance"] = owner.flagPlotNoControls;
  result["setBoundingBoxAsDefaultViewWindow"] = true;
  result[Plot::Labels::graphicsType] = "threeDimensional";
  this->writeParameters(result, owner);
  std::string script = HtmlRoutines::scriptFromJSON("graphics3d", result);
  out << script;
  owner.objectContainer.graphicsScripts.setKeyValue(
    this->getCanvasName(), script
  );
  return out.str();
}

void Plot::setCanvasName(const std::string& input) {
  this->canvasNamE = StringRoutines::convertStringToJavascriptVariable(input);
}

std::string Plot::getCanvasName() const {
  return this->canvasNamE;
}

std::string Plot::toStringDebug() {
  std::stringstream out;
  out << "Objects: " << this->plotObjects.size << "<br>";
  for (int i = 0; i < this->plotObjects.size; i ++) {
    if (this->plotObjects[i].plotType == "surface") {
      PlotObject& surface = this->plotObjects[i];
      out << surface.toStringDebug();
    }
  }
  return out.str();
}

std::string PlotObject::toStringDebug() {
  STACK_TRACE("PlotObject::toStringDebug");
  std::stringstream out;
  out << "colorUV: " << this->colorUV << "<br>";
  out << "colorVU: " << this->colorVU << "<br>";
  out
  << "Coord f-ns: "
  << this->coordinateFunctionsE.toStringCommaDelimited()
  << "<br>";
  out << "Vars: " << this->variablesInPlay << "<br>";
  out << "Var ranges: " << this->variableRangesJS << "<br>";
  return out.str();
}

void PlotObject::writeCoordinateFunctions(JSData& output) {
  this->writeVariables(output);
  List<std::string> coordinates;
  coordinates.setSize(this->coordinateFunctionsJS.size);
  for (int i = 0; i < this->coordinateFunctionsJS.size; i ++) {
    coordinates[i] = "return " + this->coordinateFunctionsJS[i] + ";";
  }
  output[PlotObject::Labels::coordinateFunctions] = coordinates;
}

JSData PlotObject::toJSONParametricCurve() {
  STACK_TRACE("PlotObject::toJSONParametricCurve");
  JSData result;
  this->writeCoordinateFunctions(result);
  result[PlotObject::Labels::variableRange][0] = this->paramLowJS;
  result[PlotObject::Labels::variableRange][1] = this->paramHighJS;
  if (this->numberOfSegmentsJS.size > 0) {
    result[PlotObject::Labels::numberOfSegments] = this->numberOfSegmentsJS[0];
  } else {
    result[PlotObject::Labels::numberOfSegments] = 100;
  }
  JSData parametersOnTheGraphJSON;
  parametersOnTheGraphJSON.makeEmptyArray();
  for (int i = 0; i < this->parametersOnTheGraph.size(); i ++) {
    JSData variableValue;
    std::string parameterName = this->parametersOnTheGraph.keys[i];
    variableValue[parameterName] = this->parametersOnTheGraph.values[i];
    parametersOnTheGraphJSON.listObjects.addOnTop(variableValue);
  }
  result[PlotObject::Labels::parametersOnTheGraph] = parametersOnTheGraphJSON;
  this->writeColorLineWidth(result);
  return result;
}

JSData PlotObject::toJSONEscapeMap() {
  STACK_TRACE("PlotObject::toJSONEscapeMap");
  JSData result;
  this->writeCoordinateFunctions(result);
  result[PlotObject::Labels::mandelbrotMode] =
  this->extraFlags.getValueNoFail(PlotObject::Labels::mandelbrotMode);
  return result;
}

JSData PlotObject::toJSONSurfaceImmersion() {
  STACK_TRACE("PlotObject::toJSONSurfaceImmersion");
  JSData result;
  this->writeVariables(result);
  if (this->coordinateFunctionsJS.size == 3) {
    result[PlotObject::Labels::coordinateFunctions] =
    "return [" + this->coordinateFunctionsJS.toStringCommaDelimited() + "];";
  } else {
    result["error"] = "wrong number of coordinates";
  }
  result[PlotObject::Labels::variableRange] = this->variableRangesJS;
  if (this->numberOfSegmentsJS.size <= 1) {
    result[PlotObject::Labels::numberOfSegments][0] = 22;
    result[PlotObject::Labels::numberOfSegments][1] = 4;
  } else {
    result[PlotObject::Labels::numberOfSegments] = this->numberOfSegmentsJS;
  }
  result[PlotObject::Labels::colorContour] = "black";
  if (this->colorUV != "") {
    result[PlotObject::Labels::colorBack] = this->colorUV;
  } else {
    result[PlotObject::Labels::colorBack] = "red";
  }
  if (this->colorVU != "") {
    result[PlotObject::Labels::colorFront] = this->colorVU;
  } else {
    result[PlotObject::Labels::colorFront] = "pink";
  }
  this->writeLineWidth(result);
  return result;
}

std::string Plot::getPlotHtml(Calculator& owner) {
  STACK_TRACE("Plot::getPlotHtml");
  if (this->dimension == 3) {
    return this->getPlotHtml3d(owner);
  } else if (this->dimension == 2) {
    return this->getPlotHtml2d(owner);
  } else {
    std::stringstream out;
    out << "Error:dimension =" << this->dimension;
    return out.str();
  }
}

void PlotObject::writeParameters(JSData& output) {
  JSData parameters = JSData::makeEmptyArray();
  for (int i = 0; i < this->parametersInPlayJS.size; i ++) {
    parameters[i] =
    HtmlRoutines::getJavascriptVariable(this->parametersInPlayJS[i]);
  }
  output[PlotObject::Labels::parameters] = parameters;
  output[PlotObject::Labels::parameterLetter] = this->parameterLetter;
  if (
    this->parametersOnTheGraphLetter != "" &&
    this->parametersOnTheGraph.size() > 0
  ) {
    output[PlotObject::Labels::parametersOnTheGraphLetter] =
    this->parametersOnTheGraphLetter;
    JSData parametersOnTheGraph;
    parametersOnTheGraph.makeEmptyArray();
    for (int i = 0; i < this->parametersOnTheGraph.size(); i ++) {
      parametersOnTheGraph[i] = this->parametersOnTheGraph.values[i];
    }
    output[PlotObject::Labels::parametersOnTheGraph] = parametersOnTheGraph;
  }
}

void PlotObject::writeVariables(JSData& output) {
  STACK_TRACE("PlotObject::writeVariables");
  JSData arguments = JSData::makeEmptyArray();
  for (int i = 0; i < this->variablesInPlayJS.size; i ++) {
    arguments[i] =
    HtmlRoutines::getJavascriptVariable(this->variablesInPlayJS[i]);
  }
  output[PlotObject::Labels::arguments] = arguments;
  this->writeParameters(output);
}

JSData PlotObject::functionFromString(const std::string& input) {
  JSData result;
  this->writeVariables(result);
  result[PlotObject::Labels::body] = input;
  return result;
}

JSData PlotObject::coordinateFunction(int index) {
  std::string body;
  if (index >= this->coordinateFunctionsJS.size) {
    body = "{console.log('Error: bad coordinate index.');}";
  } else {
    body = "return " + this->coordinateFunctionsJS[index] + ";";
  }
  return this->functionFromString(body);
}

void PlotObject::writeColorWidthSegments(JSData& output) {
  if (this->numberOfSegmentsJS.size > 0) {
    output[PlotObject::Labels::numberOfSegments] = this->numberOfSegmentsJS[0];
  } else {
    output[PlotObject::Labels::numberOfSegments] = 200;
  }
  this->writeColorLineWidth(output);
}

void PlotObject::writeColor(JSData& output) {
  output[PlotObject::Labels::color] = this->colorJS;
}

void PlotObject::writeColorLineWidth(JSData& output) {
  this->writeLineWidth(output);
  this->writeColor(output);
}

void PlotObject::writeLineWidth(JSData& output) {
  if (this->lineWidthJS != "") {
    output[PlotObject::Labels::lineWidth] = this->lineWidthJS;
  } else {
    output[PlotObject::Labels::lineWidth] = this->lineWidth;
  }
}

JSData PlotObject::toJSON2dDrawFunction() {
  STACK_TRACE("PlotObject::toJSON2dDrawFunction");
  JSData result;
  result[PlotObject::Labels::functionLabel] = this->coordinateFunction(0);
  result[PlotObject::Labels::left] = this->leftPtJS;
  result[PlotObject::Labels::right] = this->rightPtJS;
  this->writeColorWidthSegments(result);
  return result;
}

JSData PlotObject::manifoldImmersionFunctionsJS() {
  return
  this->functionFromString("return " + this->manifoldImmersionJS + ";");
}

JSData PlotObject::toJSONDirectionFieldInTwoDimensions() {
  STACK_TRACE("PlotObject::toJSONDirectionFieldInTwoDimensions");
  JSData result;
  result[PlotObject::Labels::manifoldImmersion] =
  this->manifoldImmersionFunctionsJS();
  JSData variableRange = JSData::makeEmptyArray();
  for (int i = 0; i < 2; i ++) {
    JSData currentRange = JSData::makeEmptyArray();
    for (int j = 0; j < this->variableRangesJS.size; j ++) {
      if (i < this->variableRangesJS[j].size) {
        currentRange[j] = this->variableRangesJS[j][i];
      } else {
        currentRange[j] = "bad_variable_range";
      }
    }
    variableRange[i] = currentRange;
  }
  result[PlotObject::Labels::variableRange] = variableRange;
  JSData segmentRange = JSData::makeEmptyArray();
  for (int i = 0; i < this->numberOfSegmentsJS.size; i ++) {
    segmentRange[i] = this->numberOfSegmentsJS[i];
  }
  result[PlotObject::Labels::numberOfSegments] = segmentRange;
  result[PlotObject::Labels::defaultLength] = this->defaultLengthJS;
  this->writeColorLineWidth(result);
  return result;
}

JSData PlotObject::toJSONDrawText() {
  JSData result;
  result[PlotObject::Labels::text] = this->plotString;
  this->writeColor(result);
  if (this->pointsDouble.size > 0) {
    result[PlotObject::Labels::point] = this->pointsDouble[0];
  }
  return result;
}

JSData PlotObject::toJSONDrawPath() {
  JSData result;
  result[PlotObject::PlotTypes::points] = this->pointsDouble;
  this->writeColorLineWidth(result);
  return result;
}

void PlotObject::writeColorFilled(JSData& output) {
  output[PlotObject::Labels::colorFill] = this->colorFillJS;
}

JSData PlotObject::toJSONDrawPathFilled() {
  JSData result = this->toJSONDrawPath();
  this->writeColorFilled(result);
  return result;
}

JSData PlotObject::toJSONPlotFillStart() {
  JSData result;
  this->writeColorFilled(result);
  return result;
}

JSData PlotObject::toJSONSetProjectionScreen() {
  JSData result;
  result["projectionScreen"] = this->pointsDouble;
  return result;
}

JSData PlotObject::toJSONSegment() {
  JSData result;
  result[PlotObject::PlotTypes::points] = this->pointsDouble;
  this->writeColorLineWidth(result);
  return result;
}

JSData PlotObject::toJSON() {
  JSData result;
  std::string correctedPlotType = this->plotType;
  if (correctedPlotType == "") {
    correctedPlotType = "path";
  }
  if (correctedPlotType == "setProjectionScreen") {
    result = this->toJSONSetProjectionScreen();
  } else if (correctedPlotType == "segment") {
    result = this->toJSONSegment();
  } else if (correctedPlotType == "surface") {
    result = this->toJSONSurfaceImmersion();
  } else if (correctedPlotType == PlotObject::PlotTypes::parametricCurve) {
    result = this->toJSONParametricCurve();
  } else if (correctedPlotType == PlotObject::PlotTypes::plotFunction) {
    result = this->toJSON2dDrawFunction();
  } else if (correctedPlotType == "plotDirectionField") {
    result = this->toJSONDirectionFieldInTwoDimensions();
  } else if (correctedPlotType == PlotObject::PlotTypes::escapeMap) {
    result = this->toJSONEscapeMap();
  } else if (correctedPlotType == PlotObject::PlotTypes::points) {
    result = this->toJSONPoints();
  } else if (correctedPlotType == "label") {
    result = this->toJSONDrawText();
  } else if (correctedPlotType == PlotObject::PlotTypes::plotFillStart) {
    result = this->toJSONPlotFillStart();
  } else if (correctedPlotType == "plotFillFinish") {    // The plot type carries
    // all
    // information.
  } else if (correctedPlotType == "axesGrid") {    // The plot type carries all
    // information.
  } else if (correctedPlotType == PlotObject::PlotTypes::pathFilled) {
    result = this->toJSONDrawPathFilled();
  } else {
    result = this->toJSONDrawPath();
  }
  result[PlotObject::Labels::plotType] = correctedPlotType;
  return result;
}

JSData PlotObject::toJSONPoints() {
  STACK_TRACE("PlotObject::toJSONPoints");
  JSData result;
  JSData points = JSData::makeEmptyArray();
  for (int i = 0; i < this->pointsJS.numberOfRows; i ++) {
    JSData onePoint = JSData::makeEmptyArray();
    for (int j = 0; j < this->pointsJS.numberOfColumns; j ++) {
      onePoint[j] = this->pointsJS(i, j);
    }
    points[i] = onePoint;
  }
  result[PlotObject::PlotTypes::points] = points;
  result[PlotObject::Labels::color] = this->colorJS;
  this->writeVariables(result);
  return result;
}

void Plot::computeCanvasNameIfNecessary(int& canvasCounter) {
  if (this->getCanvasName() != "") {
    return;
  }
  canvasCounter ++;
  std::stringstream canvasNameStream;
  canvasNameStream << "theCanvas" << canvasCounter;
  this->setCanvasName(canvasNameStream.str());
}

JSData Plot::getCoordinateSystem() {
  JSData result;
  result[PlotObject::Labels::plotType] = "coordinateAxes";
  return result;
}

JSData Plot::getComputeViewWindow() {
  JSData result;
  result[PlotObject::Labels::plotType] = "computeViewWindow";
  return result;
}

JSData Plot::getSetViewWindow() {
  JSData result;
  result[PlotObject::Labels::plotType] = "setViewWindow";
  JSData lowLeft = JSData::makeEmptyArray();
  JSData topRight = JSData::makeEmptyArray();
  lowLeft[0] = this->lowX * 1.10;
  lowLeft[1] = this->lowY * 1.10;
  topRight[0] = this->highX * 1.10;
  topRight[1] = this->highY * 1.10;
  JSData window = JSData::makeEmptyArray();
  window[0] = lowLeft;
  window[1] = topRight;
  result[PlotObject::Labels::viewWindow] = window;
  return result;
}

std::string Plot::getPlotHtml2d(Calculator& owner) {
  STACK_TRACE("Plot::getPlotHtml2d");
  owner.flagHasGraphics = true;
  if (this->flagDivAlreadyDisplayed) {
    return "[plot alredy displayed]";
  }
  this->flagDivAlreadyDisplayed = true;
  this->computeCanvasNameIfNecessary(
    owner.objectContainer.canvasPlotCounter
  );
  std::stringstream out;
  if (this->priorityViewRectangle <= 0) {
    this->computeAxesAndBoundingBox();
  }
  JSData result;
  std::string controls = this->getCanvasName() + "Controls";
  std::string messages = this->getCanvasName() + "Messages";
  if (!this->flagPlotShowJavascriptOnly) {
    out
    << "<canvas width='"
    << this->desiredHtmlWidthInPixels
    << "' height='"
    << this->desiredHtmlHeightInPixels
    << "' "
    << "style='border:solid 1px' "
    << "name='"
    << this->getCanvasName()
    << "'"
    << ">"
    << "Your browser does not support the HTML5 canvas tag.</canvas><br>";
    out << "<span name='" << controls << "'></span>";
    if (!owner.flagPlotNoControls) {
      out << "<br>";
    }
    out << "<span name='" << messages << "'></span>";
    result["noControls"] = owner.flagPlotNoControls;
  }
  result[Plot::Labels::canvasName] = this->getCanvasName();
  result[Plot::Labels::graphicsType] = "twoDimensional";
  result[Plot::Labels::controlsName] = controls;
  result[Plot::Labels::messagesName] = messages;
  result["plotUpdaters"].elementType = JSData::token::tokenArray;
  for (int i = 0; i < this->parameterNames.size; i ++) {
    InputBox& currentBox =
    owner.objectContainer.userInputTextBoxesWithValues.getValueCreateEmpty(
      this->parameterNames[i]
    );
    result["plotUpdaters"][i] = currentBox.getSliderName();
  }
  JSData plotObjects = JSData::makeEmptyArray();
  if (this->flagIncludeCoordinateSystem) {
    plotObjects.listObjects.addOnTop(this->getCoordinateSystem());
  }
  for (int i = 0; i < this->plotObjects.size; i ++) {
    PlotObject& currentPlot = this->plotObjects[i];
    plotObjects.listObjects.addOnTop(currentPlot.toJSON());
  }
  if (this->priorityViewRectangle > 0) {
    plotObjects[plotObjects.listObjects.size] = this->getSetViewWindow();
  } else {
    plotObjects[plotObjects.listObjects.size] = this->getComputeViewWindow();
  }
  result[Plot::Labels::plotObjects] = plotObjects;
  this->writeParameters(result, owner);
  out << HtmlRoutines::scriptFromJSON("graphics", result);
  return out.str();
}

void Plot::writeParameters(JSData& output, Calculator& owner) {
  List<std::string> inputBoxes;
  for (
    int i = 0; i < owner.objectContainer.userInputTextBoxesWithValues.size(); i
    ++
  ) {
    inputBoxes.addOnTop(
      owner.objectContainer.userInputTextBoxesWithValues.values[i].
      getSliderName()
    );
  }
  output[PlotObject::Labels::parameters] = inputBoxes;
}

std::string Plot::getPlotStringAddLatexCommands(bool useHtml) {
  STACK_TRACE("Plot::getPlotStringAddLatexCommands");
  std::stringstream resultStream;
  this->computeAxesAndBoundingBox();
  std::string lineSeparator = useHtml ? "<br>\n" : "\n";
  resultStream
  << "\\documentclass{article}\\usepackage{pstricks}\\usepackage{auto-pst-pdf}"
  << "\\usepackage{pst-math}\\usepackage{pst-plot}";
  resultStream
  << lineSeparator
  << "\\newcommand{\\fcLabels}[2]{\\rput[t](#1, -0.1){$x$}"
  << "\\rput[r](-0.1, #2){$y$}}"
  << lineSeparator;
  resultStream
  << "\\addtolength{\\hoffset}{-3.5cm}\\addtolength{\\textwidth}{6.8cm}"
  << "\\addtolength{\\voffset}{-3.2cm}\\addtolength{\\textheight}{6.3cm}"
  << lineSeparator;
  resultStream
  << "\\newcommand{\\fcColorGraph}{red}"
  << lineSeparator
  << "\\begin{document} \\pagestyle{empty}"
  << lineSeparator
  << "\\newcommand{\\fcColorAreaUnderGraph}{cyan}"
  << lineSeparator
  << "\\newcommand{\\fcColorNegativeAreaUnderGraph}{orange}"
  << "\\newcommand{\\fcAxesStandard}[4]{ "
  << "\\psframe*[linecolor =white](! #1 #2)(! #3 0.1 add #4 01 add) "
  << "\\psaxes[ticks = none, labels = none]"
  << "{<->}(0,0)(#1, #2)(#3, #4)\\fcLabels{#3}{#4}}"
  << lineSeparator
  << " \\psset{xunit =1cm, yunit =1cm}";
  resultStream << lineSeparator;
  resultStream
  << "\\begin{pspicture}("
  << FloatingPoint::doubleToString(lowX - 0.4)
  << ", "
  << FloatingPoint::doubleToString(lowY - 0.4)
  << ")("
  << FloatingPoint::doubleToString(highX + 0.4)
  << ","
  << FloatingPoint::doubleToString(highY + 0.5)
  << ")\n\n";
  resultStream << lineSeparator << "\\tiny\n" << lineSeparator;
  resultStream
  << " \\fcAxesStandard{"
  << FloatingPoint::doubleToString(lowX - 0.15)
  << "}{"
  << FloatingPoint::doubleToString(lowY - 0.15)
  << "}{"
  << FloatingPoint::doubleToString(highX + 0.15)
  << "}{"
  << FloatingPoint::doubleToString(highY + 0.15)
  << "}"
  << lineSeparator;
  for (int i = 0; i < this->plotObjects.size; i ++) {
    if (useHtml) {
      resultStream << this->plotObjects[i].plotStringWithHtml << lineSeparator;
    } else {
      resultStream << this->plotObjects[i].plotString << lineSeparator;
    }
  }
  resultStream << "\\end{pspicture}\n\n" << lineSeparator << "\\end{document}";
  return resultStream.str();
}

bool Expression::assignStringParsed(
  const std::string& input,
  MapList<
    std::string,
    Expression,
    HashFunctions::hashFunction<std::string>
  >* substitutions,
  Calculator& owner
) {
  STACK_TRACE("Expression::assignStringParsed");
  Expression commands, result;
  List<SyntacticElement> outputSyntacticSoup, outputSyntacticStack;
  std::string outputSyntacticErrors;
  if (
    !owner.parser.parseAndExtractExpressions(
      input,
      commands,
      outputSyntacticSoup,
      outputSyntacticStack,
      &outputSyntacticErrors
    )
  ) {
    this->assignValue(owner, outputSyntacticErrors);
    return false;
  }
  if (commands.startsWith(owner.opCommandSequence(), 2)) {
    result = commands[1];
  } else {
    result = commands;
  }
  if (substitutions != nullptr) {
    MapList<Expression, Expression> substitutionMap;
    for (int i = 0; i < substitutions->size(); i ++) {
      Expression substituted;
      substituted.makeAtom(owner, substitutions->keys[i]);
      substitutionMap.setKeyValue(substituted, substitutions->values[i]);
    }
    result.substituteRecursively(substitutionMap);
  }
  *this = result;
  return true;
}

bool Expression::isSuitableForSubstitution() const {
  if (this->owner == nullptr) {
    return false;
  }
  if (
    this->isBuiltInType() || this->startsWith(this->owner->opBind())
  ) {
    return false;
  }
  return true;
}

bool Expression::isSuitableForRecursion() const {
  if (this->owner == nullptr) {
    return false;
  }
  if (
    this->isAtom() ||
    this->isBuiltInType() ||
    this->startsWith(this->owner->opBind())
  ) {
    return false;
  }
  return true;
}

void Expression::substituteRecursively(
  MapList<Expression, Expression>& substitutions
) {
  if (substitutions.contains(*this)) {
    (*this) = substitutions.getValueCreateEmpty(*this);
    return;
  }
  this->substituteRecursivelyInChildren(substitutions);
}

void Expression::substituteRecursivelyInChildren(
  MapList<Expression, Expression>& substitutions
) {
  if (!this->isSuitableForSubstitution()) {
    return;
  }
  Expression tempE;
  for (int i = 0; i < this->size(); i ++) {
    if (substitutions.contains((*this)[i])) {
      this->setChild(
        i, substitutions.getValueCreateEmpty((*this)[i])
      );
    } else {
      tempE = (*this)[i];
      tempE.substituteRecursivelyInChildren(substitutions);
      if (!(tempE == (*this)[i])) {
        this->setChild(i, tempE);
      }
    }
  }
}

void Expression::substituteRecursively(
  const Expression& toBeSubstituted, const Expression& substituteWith
) {
  if ((*this) == toBeSubstituted) {
    (*this) = substituteWith;
    return;
  }
  this->substituteRecursivelyInChildren(toBeSubstituted, substituteWith);
}

void Expression::substituteRecursivelyInChildren(
  const Expression& toBeSubstituted, const Expression& substituteWith
) {
  if (!this->isSuitableForSubstitution()) {
    return;
  }
  Expression tempE;
  for (int i = 0; i < this->size(); i ++) {
    if (toBeSubstituted == (*this)[i]) {
      this->setChild(i, substituteWith);
    } else {
      tempE = (*this)[i];
      tempE.substituteRecursivelyInChildren(toBeSubstituted, substituteWith);
      if (!(tempE == (*this)[i])) {
        this->setChild(i, tempE);
      }
    }
  }
}
