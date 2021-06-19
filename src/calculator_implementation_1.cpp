// The current file is licensed under the license terms found in the main header file "calculator.h".
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
#include "math_extra_finite_groups_implementation.h" // undefined reference to `WeylGroupRepresentation<Rational>::reset()
#include "math_rational_function_implementation.h"
#include "string_constants.h"

#include <cmath>
#include <cfloat>

std::string PlotObject::Labels::points              = "points";
std::string PlotObject::Labels::point               = "point";
std::string PlotObject::Labels::path                = "path";
std::string PlotObject::Labels::functionLabel       = "function";
std::string PlotObject::Labels::coordinateFunctions = "coordinateFunctions";
std::string PlotObject::Labels::left                = "left";
std::string PlotObject::Labels::right               = "right";
std::string PlotObject::Labels::numberOfSegments    = "numberOfSegments";
std::string PlotObject::Labels::color               = "color";
std::string PlotObject::Labels::colorFront          = "colorFront";
std::string PlotObject::Labels::colorBack           = "colorBack";
std::string PlotObject::Labels::colorContour        = "colorContour";
std::string PlotObject::Labels::colorFill           = "colorFill";
std::string PlotObject::Labels::lineWidth           = "lineWidth";
std::string PlotObject::Labels::manifoldImmersion   = "manifoldImmersion";
std::string PlotObject::Labels::variableRange       = "variableRange";
std::string PlotObject::Labels::defaultLength       = "defaultLength";
std::string PlotObject::Labels::plotType            = "plotType";
std::string PlotObject::Labels::body                = "body";
std::string PlotObject::Labels::text                = "text";
std::string PlotObject::Labels::arguments           = "arguments";
std::string PlotObject::Labels::parameters          = "parameters";
std::string PlotObject::Labels::viewWindow          = "viewWindow";

std::string Plot::Labels::canvasName                = "canvasName";
std::string Plot::Labels::controlsName              = "controlsName";
std::string Plot::Labels::messagesName              = "messagesName";
std::string Plot::Labels::graphicsType              = "graphicsType";
std::string Plot::Labels::graphicsThreeDimensional  = "graphicsThreeDimensional";
std::string Plot::Labels::graphicsTwoDimensional    = "graphicsTwoDimensional";
std::string Plot::Labels::plotObjects               = "plotObjects";

bool Calculator::getListPolynomialVariableLabelsLexicographic(
  const Expression& input,
  Vector<Polynomial<AlgebraicNumber> >& output,
  ExpressionContext& outputContext
) {
  MacroRegisterFunctionWithName("Calculator::getListPolynomialVariableLabelsLexicographic");
  ExpressionContext contextStart(*this);
  if (!this->getVectorFromFunctionArguments(
    input,
    output,
    &contextStart,
    0,
    CalculatorConversions::functionPolynomial<AlgebraicNumber>
  )) {
    return false;
  }
  if (output.size < 2) {
    return false;
  }
  int numVars = contextStart.numberOfVariables();
  HashedList<Expression> theVars;
  theVars.setExpectedSize(numVars);
  for (int i = 0; i < numVars; i ++) {
    theVars.addOnTop(contextStart.getVariable(i));
  }
  theVars.quickSortAscending();
  PolynomialSubstitution<AlgebraicNumber> substitution;
  substitution.setSize(numVars);
  const AlgebraicNumber& one = this->objectContainer.algebraicClosure.one();
  for (int i = 0; i < substitution.size; i ++) {
    int currentIndex = theVars.getIndex(contextStart.getVariable(i));
    substitution[i].makeMonomial(
      currentIndex,
      1,
      one
    );
  }
  for (int i = 0; i < output.size; i ++) {
    Polynomial<AlgebraicNumber>& currentP = output[i];
    currentP.substitution(substitution, one);
  }
  return outputContext.setVariables(theVars);
}

bool DynkinSimpleType:: hasPrecomputedSubalgebras() const {
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

// WARNING
// Do not use the following web addresses to identify the server,
// EXCEPT in special circumstances described below.
// Instead, to get the web address of the
// calculator server,
// please use global.hostNoPort.
// That address is extracted from the
// incoming message headers
// and is how our server is seen from the outside world.
// HOWEVER,
// the incoming message headers may be forged.
// In most cases, this is not a problem
// as the forgery will
// affect negatively only the forgerer.
// HOWEVER,
// we CANNOT rely on the headers when sending
// activation emails, as an attacker could claim my email,
// forge his headers, and send me activation email which would
// wrongly link to the host given in his forged headers
// - that would be an evil site prompting me for password.
// So, only in this very special circumstance,
// we can only rely on a hard-coded web address.

std::string GlobalVariables::hopefullyPermanentWebAdress =
"https://calculator-algebra.org";

std::string GlobalVariables::hopefullyPermanentWebAdressOfServerExecutable =
"http://calculator-algebra.org/cgi-bin/calculator";
std::string GlobalVariables::hopefullyPermanent_HTTPS_WebAdressOfServerExecutable =
"https://calculator-algebra.org/cgi-bin/calculator";
std::string GlobalVariables::hopefullyPermanentWebAdressOfServerOutputFolder =
"http://calculator-algebra.org/";
std::string GlobalVariables::hopefullyPermanent_HTTPS_WebAdressJavascriptFolder =
"https://calculator-algebra.org/";

std::string Calculator::toStringSemismipleLieAlgebraLinksFromHardDrive(
  const std::string& prefixFolder,
  const DynkinType& dynkinType,
  FormatExpressions* format
) {
  (void) format;
  std::stringstream out;
  SemisimpleLieAlgebra folderComputer;
  folderComputer.weylGroup.dynkinType = dynkinType;
  out << "<tr><td><a href='"
  << folderComputer.fileNames.displayFolderName(prefixFolder)
  << folderComputer.fileNames.fileNameNoPathStructureConstants()
  << "'>"
  << dynkinType[0].letter << dynkinType[0].rank << " structure constants</a></td>\n ";
  if (dynkinType[0].hasPrecomputedSubalgebras()) {
    out << "<td><a href='"
    << folderComputer.fileNames.displayFolderName(prefixFolder)
    << folderComputer.fileNames.fileNameNoPathSemisimpleSubalgebras()
    << "'>"
    << dynkinType[0].letter << dynkinType[0].rank << " semisimple subalgebras</a>";
    out << "</td>\n ";
  } else {
    out << "<td>Not available</td>\n";
  }
  out << "<td><a href='"
  << folderComputer.fileNames.displayFolderName(prefixFolder)
  << folderComputer.fileNames.fileNameRelativePathSlTwoSubalgebras() << "'>"
  << dynkinType[0].letter << dynkinType[0].rank << " sl(2) triples</a></td>\n";
  out << "<td><a href='"
  << folderComputer.fileNames.displayFolderName(prefixFolder)
  << folderComputer.fileNames.fileNameNoPathRootSubalgebras()
  << "'>" << dynkinType[0].letter
  << dynkinType[0].rank << " root subalgebras</a></td>\n";
  return out.str();
}


void Plot::operator+=(const Plot& other) {
  MacroRegisterFunctionWithName("Plot::operator+=");
  if (other.priorityCanvasName > this->priorityCanvasName) {
    this->setCanvasName(other.getCanvasName());
  } else if (this->getCanvasName() == "" && this->priorityCanvasName == other.priorityCanvasName) {
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
    this->highBoundY = other.highBoundY;
    this->lowBoundY = other.lowBoundY;
    this->lowerBoundAxes = other.lowerBoundAxes;
    this->upperBoundAxes = other.upperBoundAxes;
  }
  if (other.priorityViewRectangle == this->priorityViewRectangle) {
    this->highBoundY = MathRoutines::maximum(this->highBoundY, other.highBoundY);
    this->lowBoundY = MathRoutines::minimum(this->lowBoundY,  other.lowBoundY);
    this->upperBoundAxes = MathRoutines::maximum(this->upperBoundAxes, other.upperBoundAxes);
    this->lowerBoundAxes = MathRoutines::minimum(this->lowerBoundAxes, other.lowerBoundAxes);
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
  this->priorityWindow = MathRoutines::maximum(this->priorityWindow, other.priorityWindow);
  this->priorityViewRectangle = MathRoutines::maximum(this->priorityViewRectangle, other.priorityViewRectangle);
}

bool Plot::operator==(const Plot& other) const {
  return
  this->priorityWindow == other.priorityWindow &&
  this->priorityCanvasName == other.priorityCanvasName &&
  this->priorityViewRectangle == other.priorityViewRectangle &&
  this->desiredHtmlHeightInPixels == other.desiredHtmlHeightInPixels &&
  this->desiredHtmlWidthInPixels == other.desiredHtmlWidthInPixels &&
  ((this->highBoundY        - other.highBoundY       ) == 0.0) &&
  ((this->lowBoundY         - other.lowBoundY        ) == 0.0) &&
  ((this->lowerBoundAxes - other.lowerBoundAxes) == 0.0) &&
  ((this->upperBoundAxes - other.upperBoundAxes) == 0.0) &&
  this->plotObjects == other.plotObjects &&
  this->parameterNames == other.parameterNames &&
  this->parameterNamesJS == other.parameterNamesJS &&
  this->getCanvasName() == other.getCanvasName() &&
  this->dimension == other.dimension &&
  this->flagIncludeCoordinateSystem == other.flagIncludeCoordinateSystem;
}

void Plot::operator+=(const PlotObject& other) {
  if (other.dimension != - 1 && this->dimension != - 1 && this->dimension != other.dimension) {
    global.fatal << "Attempting to add plot of dimension: "
    << this->dimension << " to a plot of dimension: "
    << other.dimension << ". " << global.fatal;
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
  return
  ((this->xLow      - other.xLow     ) == 0.0)                               &&
  ((this->xHigh     - other.xHigh    ) == 0.0)                               &&
  ((this->yLow      - other.yLow     ) == 0.0)                               &&
  ((this->yHigh     - other.yHigh    ) == 0.0)                               &&
  ((this->paramLow  - other.paramLow ) == 0.0)                               &&
  ((this->paramHigh - other.paramHigh) == 0.0)                               &&
  ((this->lineWidth - other.lineWidth) == 0.0)                               &&
  this->plotString                     == other.plotString                   &&
  this->fillStyle                      == other.fillStyle                    &&
  this->plotStringWithHtml             == other.plotStringWithHtml           &&
  this->colorRGB                       == other.colorRGB                     &&
  this->colorFillRGB                   == other.colorFillRGB                 &&
  this->dimension                      == other.dimension                    &&
  this->colorUV                        == other.colorUV                      &&
  this->colorVU                        == other.colorVU                      &&
  this->colorJS                        == other.colorJS                      &&
  this->lineWidthJS                    == other.lineWidthJS                  &&
  this->numberOfSegmentsJS             == other.numberOfSegmentsJS           &&
  this->points                         == other.points                       &&
  this->pointsDouble                   == other.pointsDouble                 &&
  this->pointsJS                       == other.pointsJS                     &&
  this->theRectangles                  == other.theRectangles                &&
  this->plotType                       == other.plotType                     &&
  this->manifoldImmersion              == other.manifoldImmersion            &&
  this->coordinateFunctionsE           == other.coordinateFunctionsE         &&
  this->coordinateFunctionsJS          == other.coordinateFunctionsJS        &&
  this->variablesInPlay                == other.variablesInPlay              &&
  this->variableRangesJS               == other.variableRangesJS             &&
  this->leftPoint                      == other.leftPoint                    &&
  this->rightPoint                     == other.rightPoint                   &&
  this->paramLowE                      == other.paramLowE                    &&
  this->paramHighE                     == other.paramHighE                   &&
  this->numSegmentsE                   == other.numSegmentsE                 &&
  this->variablesInPlayJS              == other.variablesInPlayJS            &&
  this->leftBoundaryIsMinusInfinity    == other.leftBoundaryIsMinusInfinity  &&
  this->rightBoundaryIsMinusInfinity   == other.rightBoundaryIsMinusInfinity &&
  this->leftPtJS                       == other.leftPtJS                     &&
  this->rightPtJS                      == other.rightPtJS                    &&
  this->colorFillJS                    == other.colorFillJS                  &&
  this->paramLowJS                     == other.paramLowJS                   &&
  this->paramHighJS                    == other.paramHighJS                  &&
  this->defaultLengthJS                == other.defaultLengthJS              &&
  this->parametersInPlayJS             == other.parametersInPlayJS
  ;
}

PlotObject::PlotObject() {
  this->xLow         = 0;
  this->xHigh        = 0;
  this->paramLow     = 0;
  this->paramHigh    = 0;
  this->yLow         = 0;
  this->yHigh        = 0;
  this->colorRGB     = 0;
  this->lineWidth    = 1;
  this->colorFillRGB = 0;
  this->dimension    = - 1;
  this->rightBoundaryIsMinusInfinity = false;
  this->leftBoundaryIsMinusInfinity = false;
}

void PlotObject::computeYBounds() {
  MacroRegisterFunctionWithName("PlotObject::computeYBounds");
  for (int i = 0; i < this->pointsDouble.size; i ++) {
    this->yHigh = MathRoutines::maximum(this->yHigh, this->pointsDouble[i][1]);
    this->yLow = MathRoutines::minimum(this->yLow, this->pointsDouble[i][1]);
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
  out << "\\psplot[linecolor =\\fcColorGraph, plotpoints =1000]{"
  << FloatingPoint::doubleToString(inputLowerBound) << "}{"
  << FloatingPoint::doubleToString(inputUpperBound) << "}{";
  out << functionStringPostfixNotation << "}";
  return out.str();
}

Plot::Plot() {
  this->lowerBoundAxes = - 0.5;
  this->upperBoundAxes = 1;
  this->lowBoundY = - 0.5;
  this->highBoundY = 0.5;
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
  MacroRegisterFunctionWithName("Plot::computeAxesAndBoundingBox");
  this->lowerBoundAxes = - 0.5;
  this->upperBoundAxes = 1.1;
  this->lowBoundY = - 0.5;
  this->highBoundY = 1.1;
  for (int k = 0; k < this->plotObjects.size; k ++) {
    this->plotObjects[k].computeYBounds();
    this->lowerBoundAxes = MathRoutines::minimum(this->plotObjects[k].xLow, lowerBoundAxes);
    this->upperBoundAxes = MathRoutines::maximum(this->plotObjects[k].xHigh, upperBoundAxes);
    this->lowBoundY = MathRoutines::minimum(this->plotObjects[k].yLow, this->lowBoundY);
    this->highBoundY = MathRoutines::maximum(this->plotObjects[k].yHigh, this->highBoundY);
    for (int j = 0; j < this->plotObjects[k].pointsDouble.size; j ++) {
      Vector<double>& currentPoint = this->plotObjects[k].pointsDouble[j];
      if (!this->isOKVector(currentPoint)) {
        continue;
      }
      this->lowerBoundAxes = MathRoutines::minimum(this->lowerBoundAxes, currentPoint[0]);
      this->upperBoundAxes = MathRoutines::maximum(this->upperBoundAxes, currentPoint[0]);
      this->lowBoundY = MathRoutines::minimum (currentPoint[1], this->lowBoundY);
      this->highBoundY = MathRoutines::maximum (currentPoint[1], this->highBoundY);
    }
  }
}

void Plot::computeAxesAndBoundingBox3d() {
  MacroRegisterFunctionWithName("Plot::computeAxesAndBoundingBox3d");
  this->lowerBoundAxes = - 0.5;
  this->upperBoundAxes = 1.1;
  this->lowBoundY = - 0.5;
  this->highBoundY = 1.1;
  for (int k = 0; k < this->plotObjects.size; k ++) {
    this->plotObjects[k].computeYBounds();
    this->lowerBoundAxes = MathRoutines::minimum(this->plotObjects[k].xLow, lowerBoundAxes);
    this->upperBoundAxes = MathRoutines::maximum(this->plotObjects[k].xHigh, upperBoundAxes);
    this->lowBoundY = MathRoutines::minimum(this->plotObjects[k].yLow, this->lowBoundY);
    this->highBoundY = MathRoutines::maximum(this->plotObjects[k].yHigh, this->highBoundY);
    for (int j = 0; j < this->plotObjects[k].pointsDouble.size; j ++) {
      Vector<double>& currentPoint = this->plotObjects[k].pointsDouble[j];
      if (!this->isOKVector(currentPoint)) {
        continue;
      }
      this->lowerBoundAxes = MathRoutines::minimum(this->lowerBoundAxes, currentPoint[0]);
      this->upperBoundAxes = MathRoutines::maximum(this->upperBoundAxes, currentPoint[0]);
      this->lowBoundY = MathRoutines::minimum(currentPoint[1], this->lowBoundY);
      this->highBoundY = MathRoutines::maximum(currentPoint[1], this->highBoundY);
    }
  }
}

List<PlotObject>& Plot::getPlots() {
  return this->plotObjects;
}

void Plot::addPlotOnTop(PlotObject& input) {
  *this += input;
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
    if (isnan(input[i])) {
      return false;
    }
  }
  return true;
}

std::string Plot::getPlotHtml3d(Calculator& owner) {
  MacroRegisterFunctionWithName("Plot::getPlotHtml3d");
  owner.flagHasGraphics = true;
  this->computeCanvasNameIfNecessary(owner.objectContainer.canvasPlotCounter);
  JSData result;
  std::stringstream out;
  if (!owner.flagPlotShowJavascriptOnly) {
    std::string canvasId = this->getCanvasName();
    std::string controlsId = canvasId + "Controls";
    std::string messagesId = canvasId + "Messages";
    out << "<canvas width='" << this->desiredHtmlWidthInPixels
    << "' height='" << this->desiredHtmlHeightInPixels << "' "
    << "style='border:solid 1px' name='"
    << canvasId
    << "'"
    << ">"
    << "Your browser does not support the HTML5 canvas tag.</canvas><br>"
    << "<span name='" << controlsId << "'></span>"
    << "<span name='" << messagesId << "'></span>";
    result[Plot::Labels::canvasName] = this->getCanvasName();
    result[Plot::Labels::controlsName] = controlsId;
    result[Plot::Labels::messagesName] = messagesId;
  }
  JSData plotUpdaters;
  for (int i = 0; i < this->parameterNames.size; i ++) {
    InputBox& currentBox = owner.objectContainer.
    userInputTextBoxesWithValues.getValueCreateEmpty(this->parameterNames[i]);
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

  std::string script = HtmlRoutines::scriptFromJSON("graphics3d", result);
  out << script;
  owner.objectContainer.graphicsScripts.setKeyValue(this->getCanvasName(), script);
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
  out <<  "Objects: " << this->plotObjects.size << "<br>";
  for (int i = 0; i < this->plotObjects.size; i ++) {
    if (this->plotObjects[i].plotType == "surface") {
      PlotObject& theSurface = this->plotObjects[i];
      out << theSurface.toStringDebug();
    }
  }
  return out.str();
}

std::string PlotObject::toStringDebug() {
  MacroRegisterFunctionWithName("PlotSurfaceIn3d::toStringDebug");
  std::stringstream out;
  out << "colorUV: " << this->colorUV << "<br>";
  out << "colorVU: " << this->colorVU << "<br>";
  out << "Coord f-ns: " << this->coordinateFunctionsE.toStringCommaDelimited() << "<br>";
  out << "Vars: " << this->variablesInPlay << "<br>";
  out << "Var ranges: " << this->variableRangesJS << "<br>";
  return out.str();
}

JSData PlotObject::toJSONCurveImmersionIn3d() {
  MacroRegisterFunctionWithName("PlotSurfaceIn3d::toJSONCurveImmersionIn3d");
  JSData result;
  this->writeVariables(result);
  List<std::string> coordinates;
  coordinates.setSize(this->coordinateFunctionsJS.size);
  for (int i = 0; i < this->coordinateFunctionsJS.size; i ++) {
    coordinates[i] = "return " + this->coordinateFunctionsJS[i] + ";";
  }
  result[PlotObject::Labels::coordinateFunctions] = coordinates;
  std::stringstream curveInstStream;
  result[PlotObject::Labels::variableRange][0] = this->paramLowJS;
  result[PlotObject::Labels::variableRange][1] = this->paramHighJS;
  if (this->numberOfSegmentsJS.size > 0) {
    result[PlotObject::Labels::numberOfSegments] = this->numberOfSegmentsJS[0];
  } else {
    result[PlotObject::Labels::numberOfSegments] = 100;
  }
  this->writeColorLineWidth(result);
  return result;
}

JSData PlotObject::toJSONSurfaceImmersion() {
  MacroRegisterFunctionWithName("PlotObject::toJSONSurfaceImmersion");
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
  MacroRegisterFunctionWithName("Plot::getPlotHtml");
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

void PlotObject::writeVariables(JSData& output) {
  MacroRegisterFunctionWithName("PlotObject::writeVariables");
  JSData arguments = JSData::makeEmptyArray();
  for (int i = 0; i < this->variablesInPlayJS.size; i ++) {
    arguments[i] = HtmlRoutines::getJavascriptVariable(this->variablesInPlayJS[i]);
  }
  output[PlotObject::Labels::arguments] = arguments;
  JSData parameters = JSData::makeEmptyArray();
  for (int i = 0; i < this->parametersInPlayJS.size; i ++) {
    parameters[i] = HtmlRoutines::getJavascriptVariable(this->parametersInPlayJS[i]);
  }
  output[PlotObject::Labels::parameters] = parameters;
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
  MacroRegisterFunctionWithName("PlotSurfaceIn3d::toJSON2dDrawFunction");
  JSData result;
  result[PlotObject::Labels::functionLabel] = this->coordinateFunction(0);
  result[PlotObject::Labels::left] = this->leftPtJS;
  result[PlotObject::Labels::right] = this->rightPtJS;
  this->writeColorWidthSegments(result);
  return result;
}

JSData PlotObject::toJSONParametricCurveInTwoDimensions() {
  MacroRegisterFunctionWithName("PlotSurfaceIn3d::toJSONParametricCurveInTwoDimensions");
  JSData result;
  JSData coordinateFunctions = JSData::makeEmptyArray();
  for (int i = 0; i < this->coordinateFunctionsJS.size; i ++) {
    coordinateFunctions[i] = this->coordinateFunction(i);
  }
  if (this->coordinateFunctionsJS.size != 2) {
    return result;
  }
  result[PlotObject::Labels::coordinateFunctions] = coordinateFunctions;
  result[PlotObject::Labels::left] = this->paramLowJS;
  result[PlotObject::Labels::right] = this->paramHighJS;
  this->writeColorWidthSegments(result);
  return result;
}

JSData PlotObject::manifoldImmersionFunctionsJS() {
  return this->functionFromString("return " + this->manifoldImmersionJS + ";");
}

JSData PlotObject::toJSONDirectionFieldInTwoDimensions() {
  MacroRegisterFunctionWithName("PlotSurfaceIn3d::toJSONDirectionFieldInTwoDimensions");
  JSData result;

  result[PlotObject::Labels::manifoldImmersion] = this->manifoldImmersionFunctionsJS();
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
  result[PlotObject::Labels::points] = this->pointsDouble;
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
  result[PlotObject::Labels::points] = this->pointsDouble;
  this->writeColorLineWidth(result);
  return result;
}

JSData PlotObject::toJSON() {
  JSData result;
  std::string correctedPlotType = this->plotType;
  if (correctedPlotType == "") {
    correctedPlotType = "path";
  }
  if (correctedPlotType == "setProjectionScreen" ) {
    result = this->toJSONSetProjectionScreen();
  } else if (correctedPlotType == "segment") {
    result = this->toJSONSegment();
  } else if (correctedPlotType == "surface") {
    result = this->toJSONSurfaceImmersion();
  } else if (correctedPlotType == "parametricCurve") {
    result = this->toJSONCurveImmersionIn3d();
  } else if (correctedPlotType == "plotFunction") {
    result = this->toJSON2dDrawFunction();
  } else if (correctedPlotType == "parametricCurve") {
    result = this->toJSONParametricCurveInTwoDimensions();
  } else if (correctedPlotType == "plotDirectionField") {
    result = this->toJSONDirectionFieldInTwoDimensions() ;
  } else if (correctedPlotType == PlotObject::Labels::points) {
    result = this->toJSONPoints();
  } else if (correctedPlotType == "label") {
    result = this->toJSONDrawText();
  } else if (correctedPlotType == "plotFillStart") {
    result = this->toJSONPlotFillStart();
  } else if (correctedPlotType == "plotFillFinish") {
    // The plot type carries all information.
  } else if (correctedPlotType == "axesGrid") {
    // The plot type carries all information.
  } else if (correctedPlotType == "pathFilled") {
    result = this->toJSONDrawPathFilled();
  } else {
    result = this->toJSONDrawPath();
  }
  result[PlotObject::Labels::plotType] = correctedPlotType;
  return result;
}

JSData PlotObject::toJSONPoints() {
  MacroRegisterFunctionWithName("PlotObject::toJSONPoints");
  JSData result;
  JSData points = JSData::makeEmptyArray();
  for (int i = 0; i < this->pointsJS.numberOfRows; i ++) {
    JSData onePoint = JSData::makeEmptyArray();
    for (int j = 0; j < this->pointsJS.numberOfColumns; j ++) {
      onePoint[j] = this->pointsJS(i, j);
    }
    points[i] = onePoint;
  }
  result[PlotObject::Labels::points] = points;
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
  lowLeft[0] = this->lowerBoundAxes * 1.10;
  lowLeft[1] = this->lowBoundY * 1.10;
  topRight[0] = this->upperBoundAxes * 1.10;
  topRight[1] = this->highBoundY * 1.10;
  JSData window = JSData::makeEmptyArray();
  window[0] = lowLeft;
  window[1] = topRight;
  result[PlotObject::Labels::viewWindow] = window;
  return result;
}

std::string Plot::getPlotHtml2d(Calculator& owner) {
  MacroRegisterFunctionWithName("Plot::getPlotHtml2d");
  owner.flagHasGraphics = true;
  if (this->flagDivAlreadyDisplayed) {
    return "[plot alredy displayed]";
  }
  this->flagDivAlreadyDisplayed = true;
  this->computeCanvasNameIfNecessary(owner.objectContainer.canvasPlotCounter);
  std::stringstream out;
  if (this->priorityViewRectangle <= 0) {
    this->computeAxesAndBoundingBox();
  }
  JSData result;
  std::string controls = this->getCanvasName() + "Controls";
  std::string messages = this->getCanvasName() + "Messages";
  if (!this->flagPlotShowJavascriptOnly) {
    out << "<canvas width='" << this->desiredHtmlWidthInPixels
    << "' height='" << this->desiredHtmlHeightInPixels << "' "
    << "style='border:solid 1px' "
    << "name='" << this->getCanvasName() << "'"
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
    InputBox& currentBox = owner.objectContainer.userInputTextBoxesWithValues.getValueCreateEmpty(this->parameterNames[i]);
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
  out << HtmlRoutines::scriptFromJSON("graphics", result);
  return out.str();
}

std::string Plot::getPlotStringAddLatexCommands(bool useHtml) {
  MacroRegisterFunctionWithName("Plot::getPlotStringAddLatexCommands");
  std::stringstream resultStream;
  this->computeAxesAndBoundingBox();
  std::string lineSeparator = useHtml ? "<br>\n" : "\n";
  resultStream << "\\documentclass{article}\\usepackage{pstricks}\\usepackage{auto-pst-pdf}"
  << "\\usepackage{pst-math}\\usepackage{pst-plot}";
  resultStream << lineSeparator << "\\newcommand{\\fcLabels}[2]{\\rput[t](#1, -0.1){$x$}\\rput[r](-0.1, #2){$y$}}" << lineSeparator;
  resultStream << "\\addtolength{\\hoffset}{-3.5cm}\\addtolength{\\textwidth}{6.8cm}"
  << "\\addtolength{\\voffset}{-3.2cm}\\addtolength{\\textheight}{6.3cm}"
  << lineSeparator;
  resultStream << "\\newcommand{\\fcColorGraph}{red}" << lineSeparator << "\\begin{document} \\pagestyle{empty}" << lineSeparator
  << "\\newcommand{\\fcColorAreaUnderGraph}{cyan}" << lineSeparator << "\\newcommand{\\fcColorNegativeAreaUnderGraph}{orange}"
  << "\\newcommand{\\fcAxesStandard}[4]{ \\psframe*[linecolor =white](! #1 #2)(! #3 0.1 add #4 01 add) "
  << "\\psaxes[ticks = none, labels = none]{<->}(0,0)(#1, #2)(#3, #4)\\fcLabels{#3}{#4}}"
  << lineSeparator << " \\psset{xunit =1cm, yunit =1cm}";
  resultStream << lineSeparator;
  resultStream << "\\begin{pspicture}(" << FloatingPoint::doubleToString(lowerBoundAxes - 0.4) << ", "
  << FloatingPoint::doubleToString(lowBoundY - 0.4) << ")("
  << FloatingPoint::doubleToString(upperBoundAxes + 0.4)
  << "," << FloatingPoint::doubleToString(highBoundY + 0.5) << ")\n\n";
  resultStream << lineSeparator << "\\tiny\n" << lineSeparator;
  resultStream << " \\fcAxesStandard{" << FloatingPoint::doubleToString(lowerBoundAxes - 0.15)
  << "}{" << FloatingPoint::doubleToString(lowBoundY - 0.15) << "}{"
  << FloatingPoint::doubleToString(upperBoundAxes + 0.15) << "}{"
  << FloatingPoint::doubleToString(highBoundY + 0.15) << "}" << lineSeparator;
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
  MapList<std::string, Expression, MathRoutines::hashString>* substitutions,
  Calculator& owner
) {
  MacroRegisterFunctionWithName("Expression::assignStringParsed");
  Expression commands, result;
  List<SyntacticElement> outputSyntacticSoup, outputSyntacticStack;
  std::string outputSyntacticErrors;
  if (!owner.parser.parseAndExtractExpressions(
    input, commands, outputSyntacticSoup, outputSyntacticStack, &outputSyntacticErrors
  )) {
    this->assignValue(outputSyntacticErrors, owner);
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
      substituted.makeAtom(substitutions->keys[i], owner);
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
  if (this->isBuiltInType() || this->startsWith(this->owner->opBind())) {
    return false;
  }
  return true;
}

bool Expression::isSuitableForRecursion() const {
  if (this->owner == nullptr) {
    return false;
  }
  if (this->isAtom() || this->isBuiltInType() || this->startsWith(this->owner->opBind())) {
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
      this->setChild(i, substitutions.getValueCreateEmpty((*this)[i]));
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
