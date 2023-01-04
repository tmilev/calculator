#include "math_extra_drawing_variables.h"
#include "progress_report.h"
#include "math_general_implementation.h"

std::string DrawingVariables::typeCircleAtVector = "circleAtVector";
std::string DrawingVariables::typePath = "path";
std::string DrawingVariables::typeSegment = "segment";
std::string DrawingVariables::typeSegment2DFixed = "segment2DFixed";
std::string DrawingVariables::typeTextAtVector = "text";
std::string DrawingVariables::typeText2DFixed = "text2DFixed";
std::string DrawingVariables::typeFilledShape = "filledShape";
std::string DrawingVariables::typeHighlightGroup = "highlightGroup";
std::string DrawingVariables::fieldColor = "color";
std::string DrawingVariables::fieldOperation = "operation";
std::string DrawingVariables::fieldPoints = "points";
std::string DrawingVariables::fieldLocation = "location";
std::string DrawingVariables::fieldRadius = "radius";
std::string DrawingVariables::fieldPenStyle = "penStyle";
std::string DrawingVariables::fieldText = "text";
std::string DrawingVariables::fieldLabels = "labels";
std::string DrawingVariables::fieldLineWidth = "lineWidth";
std::string DrawingVariables::fieldFrameId = "frameId";
std::string DrawingVariables::fieldFrameIndex = "frameIndex";

DrawingVariables::DrawingVariables() {
  this->initDrawingVariables();
}

void DrawingVariables::initDrawingVariables() {
  this->defaultHtmlHeight = 400;
  this->defaultHtmlWidth = 400;
  this->fontSizeNormal = 10;
  this->initialize();
  this->initDimensions(2);
  this->flagAnimatingMovingCoordSystem = false;
}

void DrawingVariables::drawCoordinateSystemBuffer(
  DrawingVariables& variables, int dimension
) {
  STACK_TRACE("DrawingVariables::drawCoordinateSystemBuffer");
  Vector<Rational> unitVector;
  Vector<Rational> zeroRoot;
  zeroRoot.makeZero(dimension);
  for (int i = 0; i < dimension; i ++) {
    unitVector.makeEi(dimension, i);
    std::string label;
    label = unitVector.toString();
    variables.drawLineBetweenTwoVectorsBufferRational(
      zeroRoot, unitVector, "black", 1
    );
    variables.drawTextAtVectorBufferRational(unitVector, label, "black");
    variables.drawCircleAtVectorBufferRational(unitVector, "black", 4);
  }
  variables.basisToDrawCirclesAt.makeEiBasis(dimension);
}

void DrawingVariables::drawTextAtVectorBufferDouble(
  const Vector<double>& point,
  const std::string& inputText,
  const std::string& color
) {
  this->drawTextAtVectorBufferDouble(
    point, inputText, color, this->fontSizeNormal
  );
}

void DrawingVariables::projectOnToHyperPlaneGraphics(
  Vector<Rational>& input, Vector<Rational>& output
) {
  output = input;
  Vector<Rational> normal;
  Vector<Rational> basepoint;
  normal.makeZero(input.size);
  for (int i = 0; i < input.size; i ++) {
    if (input[i].isPositiveOrZero()) {
      normal[i] += 1;
    } else {
      normal[i] += - 1;
    }
  }
  basepoint.makeZero(input.size);
  basepoint[0].assignInteger(1);
  if (input[0].isNegative()) {
    basepoint.negate();
  }
  Rational scalarProduct;
  output.scalarEuclidean(normal, scalarProduct);
  Rational scalarProductBasePoint;
  basepoint.scalarEuclidean(normal, scalarProductBasePoint);
  Rational scalar;
  if (!scalarProduct.isEqualToZero()) {
    scalar = scalarProductBasePoint;
    scalar.divideBy(scalarProduct);
    output *= scalar;
  } else {
    output.makeZero(input.size);
  }
}

int DrawingVariables::getColorFromChamberIndex(int index) {
  static const int numberColorsBase = 3;
  int colorIndex = index % (
    numberColorsBase * numberColorsBase * numberColorsBase
  );
  if (colorIndex < 0) {
    colorIndex += (numberColorsBase * numberColorsBase * numberColorsBase);
  }
  int r = (255 *(colorIndex % numberColorsBase)) / numberColorsBase;
  int g = (255 *(colorIndex % (numberColorsBase* numberColorsBase))) / (
    numberColorsBase* numberColorsBase
  );
  int b = (
    255 *(
      colorIndex % (numberColorsBase * numberColorsBase * numberColorsBase)
    )
  ) / (numberColorsBase * numberColorsBase * numberColorsBase);
  return
  static_cast<int>(
    HtmlRoutines::redGreenBlue(
      static_cast<unsigned int>(r),
      static_cast<unsigned int>(g),
      static_cast<unsigned int>(b)
    )
  );
}

std::string DrawingVariables::getColorPsTricksFromColorIndex(int colorIndex) {
  std::stringstream out;
  int r = (colorIndex / 65536) % 256;
  int g = (colorIndex / 256) % 256;
  int b = colorIndex % 256;
  out
  << "\\\\newrgbcolor{currentColor}{"
  << (static_cast<double>(r) / 255)
  << ", "
  << ((static_cast<double>(g)) / 255)
  << ", "
  << ((static_cast<double>(b)) / 255)
  << "}";
  return out.str();
}

void DrawingVariables::scaleToUnitLength(Vector<double>& root) {
  double length = this->bilinearForm.scalarProduct(root, root);
  length = FloatingPoint::sqrtFloating(length);
  root /= length;
}

bool DrawingVariables::getColorIntFromColorString(
  const std::string& input, int& output
) {
  if (input == "blue") {
    output = static_cast<int>(HtmlRoutines::redGreenBlue(0, 0, 255));
    return true;
  }
  if (input == "green") {
    output = static_cast<int>(HtmlRoutines::redGreenBlue(0, 255, 0));
    return true;
  }
  if (input == "red") {
    output = static_cast<int>(HtmlRoutines::redGreenBlue(255, 0, 0));
    return true;
  }
  if (input == "cyan") {
    output = static_cast<int>(HtmlRoutines::redGreenBlue(0, 255, 255));
    return true;
  }
  if (input == "orange") {
    output = static_cast<int>(HtmlRoutines::redGreenBlue(255, 127, 0));
    return true;
  }
  return false;
}

std::string DrawingVariables::getColorHtmlFromColorIndex(int colorIndex) {
  std::stringstream out;
  int r = (colorIndex / 65536) % 256;
  int g = (colorIndex / 256) % 256;
  int b = colorIndex % 256;
  out << "#";
  if (r < 16) {
    out << 0;
  }
  out << std::hex << r;
  if (g < 16) {
    out << 0;
  }
  out << std::hex << g;
  if (b < 16) {
    out << 0;
  }
  out << std::hex << b;
  return out.str();
}

void DrawingVariables::drawLineBetweenTwoVectorsBufferDouble(
  const Vector<double>& vector1,
  const Vector<double>& vector2,
  const std::string& color,
  double lineWidth
) {
  DrawSegment drawSegment;
  drawSegment.vector1 = vector1;
  drawSegment.vector2 = vector2;
  drawSegment.drawOptions.set(color, lineWidth);
  this->operations.addOnTop(drawSegment);
}

void DrawingVariables::drawFilledShape(
  const List<Vector<double> >& corners,
  uint32_t penStyle,
  int colorIndex,
  int fillColorIndex,
  double lineWidth
) {
  (void) penStyle;
  (void) colorIndex;
  (void) fillColorIndex;
  DrawGeneric drawObject;
  JSData& operation = drawObject.content;
  operation[DrawingVariables::fieldOperation] =
  DrawingVariables::typeFilledShape;
  operation[DrawingVariables::fieldPoints] = JSData::token::tokenArray;
  for (int i = 0; i < corners.size; i ++) {
    operation[DrawingVariables::fieldPoints][i] = corners[i];
  }
  if (lineWidth != 1.0) {
    operation[DrawingVariables::fieldLineWidth] = lineWidth;
  }
  this->operations.addOnTop(drawObject);
}

void DrawingVariables::drawTextAtVectorBufferRational(
  const Vector<Rational>& input,
  const std::string& inputText,
  const std::string& color
) {
  this->drawTextAtVectorBufferRational(
    input, inputText, color, this->fontSizeNormal
  );
}

void DrawingVariables::drawTextAtVectorBufferRational(
  const Vector<Rational>& input,
  const std::string& inputText,
  const std::string& color,
  int fontSize
) {
  this->drawTextAtVectorBufferDouble(
    input.getVectorDouble(), inputText, color, fontSize
  );
}

void DrawingVariables::drawTextAtVectorBufferDouble(
  const Vector<double>& input,
  const std::string& inputText,
  const std::string& color,
  int fontSize
) {
  (void) fontSize;
  DrawText result;
  result.location = input;
  result.text = inputText;
  result.drawOptions.color = color;
  this->operations.addOnTop(result);
}

void DrawingVariables::drawLineDirectly(
  double x1,
  double y1,
  double x2,
  double y2,
  uint32_t penStyle,
  int colorIndex,
  double lineWidth
) {
  this->drawLineBuffer(x1, y1, x2, y2, penStyle, colorIndex, lineWidth);
}

void DrawingVariables::drawHighlightGroup(
  Vectors<double>& highlightGroup,
  List<std::string>& labels,
  const std::string& color,
  int radius
) {
  DrawGeneric drawObject;
  JSData& operation = drawObject.content;
  operation[DrawingVariables::fieldOperation] =
  DrawingVariables::typeHighlightGroup;
  operation[DrawingVariables::fieldPoints] = highlightGroup;
  operation[DrawingVariables::fieldLabels] = labels;
  operation[DrawingVariables::fieldColor] = color;
  operation[DrawingVariables::fieldRadius] = radius;
  this->operations.addOnTop(drawObject);
}

void DrawingVariables::drawLineBuffer(
  double x1,
  double y1,
  double x2,
  double y2,
  uint32_t penStyle,
  int colorIndex,
  double lineWidth
) {
  (void) penStyle;
  (void) colorIndex;
  (void) lineWidth;
  DrawGeneric drawObject;
  JSData& operation = drawObject.content;
  operation[DrawingVariables::fieldOperation] =
  DrawingVariables::typeSegment2DFixed;
  operation[DrawingVariables::fieldPoints].elementType =
  JSData::token::tokenArray;
  operation[DrawingVariables::fieldPoints][0][0] = x1;
  operation[DrawingVariables::fieldPoints][0][1] = y1;
  operation[DrawingVariables::fieldPoints][1][0] = x2;
  operation[DrawingVariables::fieldPoints][1][1] = y2;
  this->operations.addOnTop(drawObject);
}

void DrawingVariables::drawTextBuffer(
  double x1,
  double y1,
  const std::string& inputText,
  int colorIndex,
  int fontSize,
  int textStyle
) {
  (void) fontSize;
  (void) colorIndex;
  (void) textStyle;
  DrawGeneric drawObject;
  JSData& operation = drawObject.content;
  operation[DrawingVariables::fieldOperation] =
  DrawingVariables::typeSegment2DFixed;
  operation[DrawingVariables::fieldLocation].elementType =
  JSData::token::tokenArray;
  operation[DrawingVariables::fieldLocation][0] = x1;
  operation[DrawingVariables::fieldLocation][1] = y1;
  operation[DrawingVariables::fieldText] = inputText;
  this->operations.addOnTop(drawObject);
}

double DrawingVariables::latexUnit() const {
  double width = this->boundingBoxLatex[1][0] - this->boundingBoxLatex[0][0];
  double height = this->boundingBoxLatex[1][1] - this->boundingBoxLatex[0][1];
  return MathRoutines::minimum(4 / width, 4 / height);
}

std::string DrawingVariables::toLatexPsTricks() const {
  std::stringstream out;
  double unit = this->latexUnit();
  out
  << "\\psset{xunit="
  << FloatingPoint::doubleToString(unit)
  << "cm, yunit="
  << FloatingPoint::doubleToString(unit)
  << "cm}\n";
  out
  << "\\begin{pspicture}("
  << FloatingPoint::doubleToString(this->boundingBoxLatex[0][0])
  << ","
  << FloatingPoint::doubleToString(this->boundingBoxLatex[0][1])
  << ")("
  << FloatingPoint::doubleToString(this->boundingBoxLatex[1][0])
  << ","
  << FloatingPoint::doubleToString(this->boundingBoxLatex[1][1])
  << ")\n";
  for (int i = 0; i < this->operations.size; i ++) {
    this->operations[i].toLatexPsTricks(out, *this);
    out << "\n";
  }
  out << "\\end{pspicture}";
  return out.str();
}

void DrawingVariables::drawTextBuffer(
  double x1, double y1, const std::string& inputText, int color
) {
  this->drawTextBuffer(
    x1,
    y1,
    inputText,
    color,
    this->fontSizeNormal,
    this->TextStyleNormal
  );
}

void DrawingVariables::drawString(
  DrawElementInputOutput& drawData,
  const std::string& input,
  int fontSize,
  int textStyle
) {
  drawData.outputHeight = 0;
  drawData.outputWidth = 0;
  if (input == "") {
    return;
  }
  for (unsigned int i = 0; i < input.size(); i ++) {
    std::string character;
    character = input.at(i);
    this->drawTextBuffer(
      drawData.outputWidth + drawData.topLeftCornerX,
      drawData.outputHeight + drawData.topLeftCornerY,
      character,
      0,
      fontSize,
      textStyle
    );
    drawData.outputWidth += static_cast<int>(
      static_cast<double>(fontSize) / 1.15
    );
  }
}

int DrawingVariables::getDimensionFromBilinearForm() {
  return this->bilinearForm.numberOfRows;
}

void DrawingVariables::getCoordinatesDrawingComputeAll(
  Vector<double>& input, double& x1, double& y1
) const {
  x1 =
  this->bilinearForm.scalarProduct(input, this->basisProjectionPlane[0]);
  y1 =
  this->bilinearForm.scalarProduct(input, this->basisProjectionPlane[1]);
  x1 = x1 * this->graphicsUnit + this->centerX;
  y1 = y1 * this->graphicsUnit + this->centerY;
}

void DrawingVariables::getCoordinatesForDrawingProjectionsComputed(
  const Vector<double>& input, double& x1, double& y1
) const {
  x1 = 0;
  y1 = 0;
  for (int j = 0; j < input.size; j ++) {
    x1 += this->projectionsEiVectors[j][0] * input[j];
    y1 += this->projectionsEiVectors[j][1] * input[j];
  }
  x1 = x1 * this->graphicsUnit + this->centerX;
  y1 = y1 * this->graphicsUnit + this->centerY;
}

Vector<double> DrawingVariables::getCoordinatesPixels(
  const Vector<double>& input
) const {
  double x = 0;
  double y = 0;
  DrawingVariables::getCoordinatesForDrawingProjectionsComputed(input, x, y);
  Vector<double> result;
  result.addOnTop(x);
  result.addOnTop(y);
  return result;
}

Vector<double> DrawingVariables::getCoordinatesLatex(
  const Vector<double>& input
) const {
  Vector<double> result;
  result.makeZero(2);
  for (int j = 0; j < input.size; j ++) {
    result[0] += this->projectionsEiVectors[j][0] * input[j];
    result[1] += this->projectionsEiVectors[j][1] * input[j];
  }
  return result;
}

void DrawingVariables::getCoordinatesForDrawingProjectionsComputed(
  const Vector<double>& input1,
  const Vector<double>& input2,
  double& x1,
  double& y1,
  double& x2,
  double& y2
) const {
  x1 = 0;
  x2 = 0;
  y1 = 0;
  y2 = 0;
  for (int j = 0; j < input1.size; j ++) {
    x1 += this->projectionsEiVectors[j][0] * input1[j];
    y1 += this->projectionsEiVectors[j][1] * input1[j];
    x2 += this->projectionsEiVectors[j][0] * input2[j];
    y2 += this->projectionsEiVectors[j][1] * input2[j];
  }
  x1 = x1 * this->graphicsUnit + this->centerX;
  x2 = x2 * this->graphicsUnit + this->centerX;
  y1 = y1 * this->graphicsUnit + this->centerY;
  y2 = y2 * this->graphicsUnit + this->centerY;
}

void DrawingVariables::initDimensions(
  Matrix<double>& bilinearForm,
  Vectors<double>& draggableBasis,
  Vectors<double>& startingPlane
) {
  this->bilinearForm = bilinearForm;
  this->basisToDrawCirclesAt = draggableBasis;
  this->basisProjectionPlane = startingPlane;
  this->centerX = 300;
  this->centerY = 300;
  this->graphicsUnit = DrawingVariables::graphicsUnitDefault;
  this->computeProjectionsEiVectors();
}

void DrawingVariables::accountBoundingBox(const Vector<double>& input) {
  Vector<double> coordinatesLatex = this->getCoordinatesLatex(input);
  if (this->boundingBoxLatex.size == 0) {
    Vector<double> v;
    v.makeZero(2);
    this->boundingBoxLatex.addOnTop(v);
    this->boundingBoxLatex.addOnTop(v);
  }
  this->boundingBoxLatex[0][0] =
  MathRoutines::minimum(
    this->boundingBoxLatex[0][0], coordinatesLatex[0]
  );
  this->boundingBoxLatex[0][1] =
  MathRoutines::minimum(
    this->boundingBoxLatex[0][1], coordinatesLatex[1]
  );
  this->boundingBoxLatex[1][0] =
  MathRoutines::maximum(
    this->boundingBoxLatex[1][0], coordinatesLatex[0]
  );
  this->boundingBoxLatex[1][1] =
  MathRoutines::maximum(
    this->boundingBoxLatex[1][1], coordinatesLatex[1]
  );
}

void DrawingVariables::initDimensions(
  Matrix<Rational>& bilinearForm,
  Vectors<double>& draggableBasis,
  Vectors<double>& startingPlane
) {
  Matrix<double> matrix;
  matrix.initialize(
    bilinearForm.numberOfRows, bilinearForm.numberOfColumns
  );
  for (int i = 0; i < bilinearForm.numberOfRows; i ++) {
    for (int j = 0; j < bilinearForm.numberOfColumns; j ++) {
      matrix.elements[i][j] = bilinearForm.elements[i][j].getDoubleValue();
    }
  }
  this->initDimensions(matrix, draggableBasis, startingPlane);
}

void DrawingVariables::computeBoundingBox() {
  for (DrawOperation& operation : this->operations) {
    operation.accountBoundingBox(*this);
  }
}

void DrawingVariables::initDimensions(int dimension) {
  if (dimension < 2) {
    dimension = 2;
  }
  this->bilinearForm.makeIdentityMatrix(dimension, 1, 0);
  this->projectionsEiVectors.setSizeMakeMatrix(dimension, 2);
  this->basisProjectionPlane.makeEiBasis(dimension);
  this->basisProjectionPlane.size = 2;
  this->modifyToOrthonormalNoShiftSecond(
    this->basisProjectionPlane[1], this->basisProjectionPlane[0]
  );
  this->basisToDrawCirclesAt.makeEiBasis(dimension);
  this->selectedCircleMinus2noneMinus1Center = - 2;
  this->centerX = 300;
  this->centerY = 300;
  this->graphicsUnit = DrawingVariables::graphicsUnitDefault;
  this->frameLengthInMilliseconds = 500;
}

bool DrawingVariables::areWithinClickTolerance(
  double x1, double y1, double x2, double y2
) {
  x1 -= x2;
  y1 -= y2;
  if (x1 < 0) {
    x1 = - x1;
  }
  if (y1 < 0) {
    y1 = - y1;
  }
  return x1 <= this->clickToleranceX && y1 <= this->clickToleranceY;
}

bool DrawingVariables::mouseMoveRedraw(int x, int y) {
  if (this->selectedCircleMinus2noneMinus1Center == - 2) {
    return false;
  }
  if (this->selectedCircleMinus2noneMinus1Center == - 1) {
    this->centerX = x;
    this->centerY = y;
    return true;
  }
  if (this->selectedCircleMinus2noneMinus1Center >= 0) {
    if (this->flagRotatingPreservingAngles) {
      this->changeBasisPReserveAngles(
        static_cast<double>(x), static_cast<double>(y)
      );
      return true;
    }
  }
  return false;
  //  this->draw();
}

void DrawingVariables::initialize() {
  this->operations.reserve(1000);
  this->centerX = 300;
  this->centerY = 300;
  this->graphicsUnit = DrawingVariables::graphicsUnitDefault;
  this->clickToleranceX = 5;
  this->clickToleranceY = 5;
  this->selectedCircleMinus2noneMinus1Center = - 2;
  this->flagRotatingPreservingAngles = true;
  this->flagAnimatingMovingCoordSystem = false;
}

double DrawingVariables::getAngleFromXandY(double x, double y) {
  double result;
  if (x != 0.0) {
    result = FloatingPoint::arctan(y / x);
  } else {
    if (y > 0) {
      result = MathRoutines::pi() / 2;
    } else {
      result = MathRoutines::pi() / (- 2);
    }
  }
  return result;
}

void DrawingVariables::click(double x, double y) {
  this->selectedCircleMinus2noneMinus1Center = - 2;
  if (
    this->areWithinClickTolerance(x, y, this->centerX, this->centerY)
  ) {
    this->selectedCircleMinus2noneMinus1Center = - 1;
  }
  int dimension = this->bilinearForm.numberOfRows;
  for (int i = 0; i < dimension; i ++) {
    double Xbasis = 0;
    double Ybasis = 0;
    this->getCoordinatesDrawingComputeAll(
      this->basisToDrawCirclesAt[i], Xbasis, Ybasis
    );
    if (this->areWithinClickTolerance(x, y, Xbasis, Ybasis)) {
      this->selectedCircleMinus2noneMinus1Center = i;
      return;
    }
  }
}

void DrawingVariables::rotateOutOfPlane(
  std::stringstream& Logger,
  Vector<double>& input,
  Vector<double>& output,
  Vector<double>& orthoBasis1,
  Vector<double>& orthoBasis2,
  double oldTanSquared,
  double newTanSquared
) {
  Vector<double> projection = orthoBasis1;
  Vector<double> vComponent = input;
  double scalarProduct1 = this->bilinearForm.scalarProduct(orthoBasis1, input);
  double scalarProduct2 = this->bilinearForm.scalarProduct(orthoBasis2, input);
  projection *= scalarProduct1;
  projection += orthoBasis2 * scalarProduct2;
  vComponent -= projection;
  Logger
  << "\ngetScalarProd ="
  << this->bilinearForm.scalarProduct(projection, vComponent);
  if (oldTanSquared < 0 || newTanSquared < 0) {
    return;
  }
  double oldAngle =
  FloatingPoint::arctan(FloatingPoint::sqrtFloating(oldTanSquared));
  double newAngle =
  FloatingPoint::arctan(FloatingPoint::sqrtFloating(newTanSquared));
  double angleChange = - oldAngle + newAngle;
  projection = orthoBasis1;
  projection *=
  FloatingPoint::cosFloating(angleChange) *
  scalarProduct1 - FloatingPoint::sinFloating(angleChange) * scalarProduct2;
  projection +=
  orthoBasis2 *(
    FloatingPoint::sinFloating(angleChange) * scalarProduct1 +
    FloatingPoint::sinFloating(angleChange) * scalarProduct2
  );
  output = vComponent;
  output += projection;
}

void DrawingVariables::modifyToOrthonormalNoShiftSecond(
  Vector<double>& root1, Vector<double>& root2
) {
  double scalar = this->bilinearForm.scalarProduct(root1, root2) /
  this->bilinearForm.scalarProduct(root2, root2);
  root1 -= root2 * scalar;
  this->scaleToUnitLength(root1);
  this->scaleToUnitLength(root2);
}

void DrawingVariables::computeProjectionsEiVectors() {
  int dimension = this->bilinearForm.numberOfRows;
  this->projectionsEiVectors.setSizeMakeMatrix(dimension, 2);
  Vector<double> root;
  for (int i = 0; i < dimension; i ++) {
    root.makeEi(dimension, i);
    this->projectionsEiVectors[i][0] =
    this->bilinearForm.scalarProduct(root, this->basisProjectionPlane[0]);
    this->projectionsEiVectors[i][1] =
    this->bilinearForm.scalarProduct(root, this->basisProjectionPlane[1]);
  }
}

void DrawingVariables::changeBasisPReserveAngles(double newX, double newY) {
  double bufferCenterX = this->centerX;
  double bufferCenterY = this->centerY;
  double bufferGraphicsUnit = this->graphicsUnit;
  newX = (newX - bufferCenterX) / bufferGraphicsUnit;
  newY = (newY - bufferCenterY) / bufferGraphicsUnit;
  if (newX == 0.0 && newY == 0.0) {
    return;
  }
  std::stringstream out;
  Vector<double>& selectedRoot =
  this->basisToDrawCirclesAt[this->selectedCircleMinus2noneMinus1Center];
  double selectedRootLength =
  this->bilinearForm.scalarProduct(selectedRoot, selectedRoot);
  double oldX, oldY;
  this->getCoordinatesDrawingComputeAll(selectedRoot, oldX, oldY);
  oldX = (oldX - bufferCenterX) / bufferGraphicsUnit;
  oldY = (oldY - bufferCenterY) / bufferGraphicsUnit;
  double oldAngle = getAngleFromXandY(oldX, oldY);
  double newAngle = getAngleFromXandY(newX, newY);
  double angleChange = - newAngle + oldAngle;
  double epsilon = 0.000000000001;
  while (angleChange > MathRoutines::pi() / 2 + epsilon) {
    angleChange -= MathRoutines::pi();
  }
  while (angleChange <= MathRoutines::pi() / (- 2) - epsilon) {
    angleChange += MathRoutines::pi();
  }
  out << "\nold angle: " << oldAngle;
  out << "\nnew angle:  " << newAngle;
  Vector<double> newVectorE1;
  Vector<double> newVectorE2;
  Vectors<double>& currentBasisPlane = this->basisProjectionPlane;
  newVectorE1 = currentBasisPlane[0] * FloatingPoint::cosFloating(angleChange);
  newVectorE1 +=
  currentBasisPlane[1] * FloatingPoint::sinFloating(angleChange);
  newVectorE2 = currentBasisPlane[1] * FloatingPoint::cosFloating(angleChange);
  newVectorE2 +=
  currentBasisPlane[0] *(- FloatingPoint::sinFloating(angleChange));
  currentBasisPlane[0] = newVectorE1;
  currentBasisPlane[1] = newVectorE2;
  double RootTimesE1 =
  this->bilinearForm.scalarProduct(selectedRoot, currentBasisPlane[0]);
  double RootTimesE2 =
  this->bilinearForm.scalarProduct(selectedRoot, currentBasisPlane[1]);
  Vector<double> vOrthogonal = selectedRoot;
  Vector<double> vProjection = currentBasisPlane[0] * RootTimesE1;
  vProjection += currentBasisPlane[1] * RootTimesE2;
  vOrthogonal -= vProjection;
  double oldRatioProjectionOverHeightSquared = (oldX * oldX + oldY * oldY) / (
    selectedRootLength - oldX * oldX - oldY * oldY
  );
  double newRatioProjectionOverHeightSquared = (newX * newX + newY * newY) / (
    selectedRootLength - newX * newX - newY * newY
  );
  out << "\noldRatio: " << oldRatioProjectionOverHeightSquared;
  out << "\nnewRatio: " << newRatioProjectionOverHeightSquared;
  if (
    this->bilinearForm.scalarProduct(vOrthogonal, vOrthogonal) > epsilon ||
    this->bilinearForm.scalarProduct(vOrthogonal, vOrthogonal) < - epsilon
  ) {
    this->scaleToUnitLength(vProjection);
    this->scaleToUnitLength(vOrthogonal);
    out
    << "\nscaled vOrthogonal ="
    << vOrthogonal
    << "->"
    << this->bilinearForm.scalarProduct(vOrthogonal, vOrthogonal);
    out
    << "\nscaled vProjection ="
    << vProjection
    << "->"
    << this->bilinearForm.scalarProduct(vProjection, vProjection);
    out
    << "\ntheScalarProd: "
    << this->bilinearForm.scalarProduct(vOrthogonal, vProjection);
    this->rotateOutOfPlane(
      out,
      currentBasisPlane[0],
      currentBasisPlane[0],
      vProjection,
      vOrthogonal,
      oldRatioProjectionOverHeightSquared,
      newRatioProjectionOverHeightSquared
    );
    this->rotateOutOfPlane(
      out,
      currentBasisPlane[1],
      currentBasisPlane[1],
      vProjection,
      vOrthogonal,
      oldRatioProjectionOverHeightSquared,
      newRatioProjectionOverHeightSquared
    );
  }
  this->modifyToOrthonormalNoShiftSecond(
    currentBasisPlane[0], currentBasisPlane[1]
  );
  out << "\ne1=" << currentBasisPlane[0];
  out << "\ne2=" << currentBasisPlane[1];
  out
  << "\ne1*e2="
  << this->bilinearForm.scalarProduct(
    currentBasisPlane[0], currentBasisPlane[1]
  );
  this->computeProjectionsEiVectors();
}

void DrawingVariables::projectionMultiplicityMergeOnBasisChange(
  DrawingVariables& operations
) {
  Matrix<ImpreciseDouble> matrix;
  int dimension = operations.bilinearForm.numberOfRows;
  matrix.initialize(dimension, 2);
  // We assume that the computeProjectionsEiVectors has been called.
  for (int i = 0; i < operations.projectionsEiVectors.size; i ++) {
    for (int j = 0; j < 2; j ++) {
      matrix(i, j) = operations.projectionsEiVectors[i][j];
    }
  }
  ProgressReport report;
  std::stringstream out;
  out << "before elimination:\n" << matrix.toString();
  matrix.gaussianEliminationEuclideanDomain(
    nullptr, ImpreciseDouble::minusOne(), ImpreciseDouble::getOne()
  );
  out << "after elimination:\n" << matrix.toString();
  report.report(out.str());
}

void DrawingVariables::operator+=(const DrawingVariables& other) {
  if (
    this->bilinearForm.numberOfRows != other.bilinearForm.numberOfRows
  ) {
    return;
  }
  this->operations.addListOnTop(other.operations);
  // this->BasisProjectionPlane.addListOnTop(other.BasisProjectionPlane);
  // this->centerX.addListOnTop(other.centerX);
  // this->centerY.addListOnTop(other.centerY);
  // this->graphicsUnit.addListOnTop(other.graphicsUnit);
}

void DrawingVariables::drawCircleAtVectorBufferRational(
  const Vector<Rational>& input,
  const std::string& color,
  double radius,
  const std::string& frameId,
  int frameIndex
) {
  this->drawCircleAtVectorBufferDouble(
    input.getVectorDouble(), color, radius, frameId, frameIndex
  );
}

void DrawingVariables::drawCircleAtVectorBufferDouble(
  const Vector<double>& input,
  const std::string& color,
  double radius,
  const std::string& frameId,
  int frameIndex
) {
  DrawCircle drawCircle;
  drawCircle.location = input;
  drawCircle.drawOptions.color = color;
  drawCircle.radius = radius;
  drawCircle.frameId = frameId;
  drawCircle.frameIndex = frameIndex;
  this->operations.addOnTop(drawCircle);
}

void DrawingVariables::drawLineBetweenTwoVectorsBufferRational(
  const Vector<Rational>& vector1,
  const Vector<Rational>& vector2,
  const std::string& color,
  double lineWidth
) {
  this->drawLineBetweenTwoVectorsBufferDouble(
    vector1.getVectorDouble(),
    vector2.getVectorDouble(),
    color,
    lineWidth
  );
}

void DrawingVariables::drawPath(
  const Vectors<Rational>& vectors,
  const std::string& color,
  double lineWidth,
  const std::string& frameId,
  int frameIndex
) {
  DrawGeneric drawObject;
  JSData& operation = drawObject.content;
  operation[DrawingVariables::fieldOperation] = DrawingVariables::typePath;
  Vectors<double> vectorsDouble;
  vectors.getVectorsDouble(vectorsDouble);
  operation[DrawingVariables::fieldPoints] = vectorsDouble;
  operation[DrawingVariables::fieldFrameId] = frameId;
  operation[DrawingVariables::fieldFrameIndex] = frameIndex;
  if (color != "") {
    operation[DrawingVariables::fieldColor] = color;
  }
  if (lineWidth != 1.0) {
    operation[DrawingVariables::fieldLineWidth] = lineWidth;
  }
  this->operations.addOnTop(drawObject);
}

void DrawSegment::toJSON(JSData& output, const DrawingVariables& owner) const {
  (void) owner;
  output[DrawingVariables::fieldOperation] = DrawingVariables::typeSegment;
  output[DrawingVariables::fieldPoints] = JSData::token::tokenArray;
  output[DrawingVariables::fieldPoints][0] = this->vector1;
  output[DrawingVariables::fieldPoints][1] = this->vector2;
  this->drawOptions.writeJSON(output);
}

void DrawSegment::toLatexPsTricks(
  std::stringstream& out, const DrawingVariables& owner
) const {
  Vector<double> projection1 = owner.getCoordinatesLatex(this->vector1);
  Vector<double> projection2 = owner.getCoordinatesLatex(this->vector2);
  out << "\\psline";
  if (this->drawOptions.color != "") {
    out << "[linecolor=" << this->drawOptions.color << "]";
  }
  out
  << "("
  << FloatingPoint::doubleToString(projection1[0])
  << ","
  << FloatingPoint::doubleToString(projection1[1])
  << ")"
  << "("
  << FloatingPoint::doubleToString(projection2[0])
  << ","
  << FloatingPoint::doubleToString(projection2[1])
  << ")";
}

void DrawSegment::accountBoundingBox(DrawingVariables& owner) const {
  owner.accountBoundingBox(this->vector1);
  owner.accountBoundingBox(this->vector2);
}

void DrawOptions::writeJSON(JSData& output) const {
  if (this->color != "") {
    output[DrawingVariables::fieldColor] = this->color;
  }
  if (this->lineWidth != 1.0) {
    output[DrawingVariables::fieldLineWidth] = this->lineWidth;
  }
}

void DrawOptions::set(const std::string& inputColor, int inputLineWidth) {
  this->color = inputColor;
  this->lineWidth = inputLineWidth;
}

std::string DrawGeneric::toString() const {
  return this->content.toString();
}

void DrawGeneric::accountBoundingBox(DrawingVariables& owner) const {
  (void) owner;
}

void DrawGeneric::toJSON(JSData& output, const DrawingVariables& owner) const {
  STACK_TRACE("DrawGeneric::toJSON");
  (void) owner;
  output = this->content;
}

void DrawGeneric::toLatexPsTricks(
  std::stringstream& out, const DrawingVariables& owner
) const {
  JSData json;
  this->toJSON(json, owner);
  out << "%unimplemented " << json.toString();
}

void DrawCircle::accountBoundingBox(DrawingVariables& owner) const {
  owner.accountBoundingBox(this->location);
}

void DrawCircle::toJSON(JSData& output, const DrawingVariables& owner) const {
  STACK_TRACE("DrawCircle::toJSON");
  (void) owner;
  output[DrawingVariables::fieldOperation] =
  DrawingVariables::typeCircleAtVector;
  output[DrawingVariables::fieldLocation] = this->location;
  output[DrawingVariables::fieldRadius] = this->radius;
  this->drawOptions.writeJSON(output);
  if (this->frameId != "") {
    output[DrawingVariables::fieldFrameId] = this->frameId;
    output[DrawingVariables::fieldFrameIndex] = this->frameIndex;
  }
}

void DrawCircle::toLatexPsTricks(
  std::stringstream& out, const DrawingVariables& owner
) const {
  Vector<double> coordinates = owner.getCoordinatesLatex(this->location);
  out << "\\pscircle*";
  if (this->drawOptions.color != "") {
    out << "[linecolor=" << this->drawOptions.color << "]";
  }
  // As of writing, the circle radius measures
  // a fixed radius that does not change with zoom level.
  double inCentimeters = this->radius / owner.latexUnit();
  double adjustedRadius = inCentimeters / 20;
  out
  << "("
  << FloatingPoint::doubleToString(coordinates[0])
  << ","
  << FloatingPoint::doubleToString(coordinates[1])
  << ")"
  << "{"
  << FloatingPoint::doubleToString(adjustedRadius)
  << "}";
}

DrawOperation::DrawOperation() {}

bool DrawOperation::toJSON(JSData& output, const DrawingVariables& owner) const {
  return
  this->toJSONImplementation<DrawGeneric>(output, owner) ||
  this->toJSONImplementation<DrawSegment>(output, owner) ||
  this->toJSONImplementation<DrawText>(output, owner) ||
  this->toJSONImplementation<DrawCircle>(output, owner);
}

bool DrawOperation::toLatexPsTricks(
  std::stringstream& out, const DrawingVariables& owner
) const {
  return
  this->toLatexPsTricksImplementation<DrawGeneric>(out, owner) ||
  this->toLatexPsTricksImplementation<DrawSegment>(out, owner) ||
  this->toLatexPsTricksImplementation<DrawText>(out, owner) ||
  this->toLatexPsTricksImplementation<DrawCircle>(out, owner);
}

bool DrawOperation::accountBoundingBox(DrawingVariables& owner) {
  return
  this->accountBoundingBoxImplementation<DrawGeneric>(owner) ||
  this->accountBoundingBoxImplementation<DrawSegment>(owner) ||
  this->accountBoundingBoxImplementation<DrawText>(owner) ||
  this->accountBoundingBoxImplementation<DrawCircle>(owner);
}

template < >
MemorySaving<DrawGeneric>& DrawOperation::getImplementation() {
  return this->drawGeneric;
}

template < >
MemorySaving<DrawSegment>& DrawOperation::getImplementation() {
  return this->drawSegment;
}

template < >
MemorySaving<DrawText>& DrawOperation::getImplementation() {
  return this->drawText;
}

template < >
MemorySaving<DrawCircle>& DrawOperation::getImplementation() {
  return this->drawCircle;
}

std::string DrawOperation::toString() const {
  if (!this->drawGeneric.isZeroPointer()) {
    return this->drawGeneric.getElementConst().toString();
  }
  return "null";
}

void DrawText::toJSON(JSData& output, const DrawingVariables& owner) const {
  (void) owner;
  output[DrawingVariables::fieldOperation] =
  DrawingVariables::typeTextAtVector;
  output[DrawingVariables::fieldLocation] = this->location;
  output[DrawingVariables::fieldText] = this->text;
  output[DrawingVariables::fieldColor] = this->drawOptions.color;
}

void DrawText::toLatexPsTricks(
  std::stringstream& out, const DrawingVariables& owner
) const {
  Vector<double> coordinates = owner.getCoordinatesLatex(this->location);
  out << "\\rput";
  out << "[";
  if (
    2 *
    coordinates[0] < owner.boundingBoxLatex[1][0] -
    owner.boundingBoxLatex[0][0]
  ) {
    out << "r";
  } else {
    out << "l";
  }
  if (
    2 *
    coordinates[1] < owner.boundingBoxLatex[1][1] -
    owner.boundingBoxLatex[0][1]
  ) {
    out << "t";
  } else {
    out << "b";
  }
  out << "]";
  out
  << "("
  << FloatingPoint::doubleToString(coordinates[0])
  << ","
  << FloatingPoint::doubleToString(coordinates[1])
  << ")"
  << "{"
  << this->text
  << "}";
}

void DrawText::accountBoundingBox(DrawingVariables& owner) const {
  owner.accountBoundingBox(this->location);
}
