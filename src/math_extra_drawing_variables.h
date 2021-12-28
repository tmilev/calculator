// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeaderDrawingVars_Already_included
#define vpfHeaderDrawingVars_Already_included

#include "math_general.h"

class DrawOperations {
private:
  void changeBasisPReserveAngles(double newX, double newY);
public:
  List<JSData> theOperations;
  List<List<double> > projectionsEiVectors;
  Vectors<double> basisProjectionPlane;
  static const int graphicsUnitDefault = 100;
  int selectedCircleMinus2noneMinus1Center; //-2= none, - 1= center of coordinate system, nonnegative integers = selectedindex
  Vectors<double> basisToDrawCirclesAt;
  Matrix<double> bilinearForm;
  MapList<std::string, List<int>, MathRoutines::hashString> frames;
  double clickToleranceX;
  double clickToleranceY;
  double centerX;
  double centerY;
  double graphicsUnit;
  int frameLengthInMilliseconds;
  bool flagRotatingPreservingAngles;
  bool flagAnimatingMovingCoordSystem;
  bool flagIsPausedWhileAnimating;
  std::string DebugString;
  int indexStartingModifiableTextCommands;
  void (*specialOperationsOnBasisChange)(DrawOperations& drawOperations);
  static void projectionMultiplicityMergeOnBasisChange(DrawOperations& operations);
  void makeMeAStandardBasis(int dimension);
  void operator+=(const DrawOperations& other);
  void initDimensions(Matrix<double>& bilinearForm, Vectors<double>& draggableBasis, Vectors<double>& startingPlane) {
    this->bilinearForm = bilinearForm;
    this->basisToDrawCirclesAt = draggableBasis;
    this->basisProjectionPlane = startingPlane;
    this->centerX = 300;
    this->centerY = 300;
    this->graphicsUnit = DrawOperations::graphicsUnitDefault;
    this->computeProjectionsEiVectors();
  }
  void initDimensions(Matrix<Rational>& bilinearForm, Vectors<double>& draggableBasis, Vectors<double>& startingPlane) {
    Matrix<double> matrix;
    matrix.initialize(bilinearForm.numberOfRows, bilinearForm.numberOfColumns);
    for (int i = 0; i < bilinearForm.numberOfRows; i ++) {
      for (int j = 0; j < bilinearForm.numberOfColumns; j ++) {
        matrix.elements[i][j] = bilinearForm.elements[i][j].getDoubleValue();
      }
    }
    this->initDimensions(matrix, draggableBasis, startingPlane);
  }
  void initDimensions(int dimension);
  int getDimensionFirstDimensionDependentOperation();
  int getDimensionFromBilinearForm();
  void getCoordsDrawingComputeAll(Vector<double>& input, double& X1, double& Y1) {
    X1 = this->bilinearForm.scalarProduct(input, this->basisProjectionPlane[0]);
    Y1 = this->bilinearForm.scalarProduct(input, this->basisProjectionPlane[1]);
    X1 = X1 * this->graphicsUnit + this->centerX;
    Y1 = Y1 * this->graphicsUnit + this->centerY;
  }
  void getCoordsForDrawingProjectionsComputed(Vector<double>& input, double& X1, double& Y1) {
    X1 = 0;
    Y1 = 0;
    for (int j = 0; j < input.size; j ++) {
      X1 += this->projectionsEiVectors[j][0] * input[j];
      Y1 += this->projectionsEiVectors[j][1] * input[j];
    }
    X1 = X1 * this->graphicsUnit + this->centerX;
    Y1 = Y1 * this->graphicsUnit + this->centerY;
  }
  void getCoordsForDrawingProjectionsComputed(Vector<double>& input1, Vector<double>& input2, double& X1, double& Y1, double& X2, double& Y2) {
    X1 = 0;
    X2 = 0;
    Y1 = 0;
    Y2 = 0;
    for (int j = 0; j < input1.size; j ++) {
      X1 += this->projectionsEiVectors[j][0] * input1[j];
      Y1 += this->projectionsEiVectors[j][1] * input1[j];
      X2 += this->projectionsEiVectors[j][0] * input2[j];
      Y2 += this->projectionsEiVectors[j][1] * input2[j];
    }
    X1 = X1 * this->graphicsUnit + this->centerX;
    X2 = X2 * this->graphicsUnit + this->centerX;
    Y1 = Y1 * this->graphicsUnit + this->centerY;
    Y2 = Y2 * this->graphicsUnit + this->centerY;
  }
  void ensureProperInitialization();
  bool areWithinClickTolerance(double x1, double y1, double x2, double y2) {
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
  bool mouseMoveRedraw(int x, int y) {
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
        this->changeBasisPReserveAngles(static_cast<double>(x), static_cast<double>(y));
        return true;
      }
    }
    return false;
  //  this->draw();
  }
  void click(double x, double y);
  void drawHighlightGroup(Vectors<double>& highlightGroup, List<std::string>& labels, const std::string& color, int radius);
  void drawLineBuffer(
    double x1, double y1, double x2, double y2, uint32_t thePenStyle, int colorIndex, double lineWidth
  );
  void drawTextBuffer(double x1, double y1, const std::string& inputText, int ColorIndex, int fontSize, int textStyle);
  void drawLineBetweenTwoVectorsBufferRational(
    const Vector<Rational>& vector1, const Vector<Rational>& vector2, const std::string& color, double lineWidth = 1
  );
  void drawFilledShape(
    const List<Vector<double> >& corners,
    uint32_t penStyle,
    int colorIndex,
    int fillColorIndex,
    double lineWidth
  );
  void drawLineBetweenTwoVectorsBufferDouble(
    const Vector<double>& vector1, const Vector<double>& vector2, const std::string& color, double lineWidth = 1
  );
  void drawPath(
    const Vectors<Rational>& vectors, const std::string& color, double lineWidth, const std::string& frameId, int frameIndex
  );
  void drawTextAtVectorBufferRational(const Vector<Rational>& input, const std::string& inputText, const std::string& color, int fontSize);
  void drawTextAtVectorBufferDouble(const Vector<double>& input, const std::string& inputText, int ColorIndex, int fontSize, int textStyle);
  void drawCircleAtVectorBufferRational(
    const Vector<Rational>& input, const std::string& color, double radius, const std::string& frameId = "", int frameIndex = - 1
  );
  void drawCircleAtVectorBufferDouble(const Vector<double>& input, const std::string& color, double radius);
  double getAngleFromXandY(double x, double y);
  void scaleToUnitLength(Vector<double>& root) {
    double length = this->bilinearForm.scalarProduct(root, root);
    length = FloatingPoint::sqrtFloating(length);
    root /= length;
  }
  void rotateOutOfPlane(
    std::stringstream& Logger,
    Vector<double>& input,
    Vector<double>& output,
    Vector<double>& orthoBasis1,
    Vector<double>& orthoBasis2,
    double oldTanSquared,
    double newTanSquared
  );
  void modifyToOrthonormalNoShiftSecond(Vector<double>& root1, Vector<double>& root2);
  void computeProjectionsEiVectors();
  DrawOperations() {
    this->initDimensions(2);
    this->flagAnimatingMovingCoordSystem = false;
    this->specialOperationsOnBasisChange = nullptr;
    this->indexStartingModifiableTextCommands = 0;
  }
  void initialize();
  static std::string typeSegment;
  static std::string typePath;
  static std::string typeSegment2DFixed;
  static std::string typeHighlightGroup;
  static std::string typeText;
  static std::string typeText2DFixed;
  static std::string typeTextAtVector;
  static std::string typeCircleAtVector;
  static std::string typeFilledShape;

  static std::string penStyleNormal;

  static std::string fieldPoints;
  static std::string fieldPenStyle;
  static std::string fieldLocation;
  static std::string fieldRadius;
  static std::string fieldColor;
  static std::string fieldOperation;
  static std::string fieldLineWidth;
  static std::string fieldFrameId;
  static std::string fieldFrameIndex;
  static std::string fieldText;
  static std::string fieldLabels;
};

class DrawingVariables {
public:
  typedef void (*DrawLineFunction)(double X1, double Y1, double X2, double Y2, unsigned long penStyle, int ColorIndex);
  typedef void (*DrawTextFunction)(double X1, double Y1, const char* theText, int length, int ColorIndex, int fontSize);
  typedef void (*DrawCircleFunction)(double X1, double Y1, double radius, unsigned long thePenStyle, int ColorIndex);
  typedef void (*DrawClearScreenFunction)();

private:
  DrawLineFunction drawLineFunction;
  DrawTextFunction drawTextFunction;
  DrawCircleFunction drawCircleFunction;
  DrawClearScreenFunction drawClearScreenFunction;
public:
  enum PenStyles {
    PenStyleInvisible,
    PenStyleDashed,
    PenStyleDotted,
    PenStyleNormal,
    PenStyleZeroChamber,
    PenStylePermanentlyZeroChamber,
    PenStyleLinkToOriginZeroChamber,
    PenStyleLinkToOrigin,
    PenStyleLinkToOriginPermanentlyZeroChamber
  };
  enum TextStyles {
    TextStyleNormal, TextStyleInvisible, TextStyleChamber, TextStyleZeroChamber, TextStylePermanentlyZeroChamber
  };
  int ColorDashes;
  bool flagLaTeXDraw;
  bool flag2DprojectionDraw;
  bool flagDisplayingCreationNumbersInsteadOfDisplayNumbers;
  bool flagDrawChamberIndices;
  bool flagDrawingInvisibles;
  bool flagDrawingLinkToOrigin;
  bool flagFillUserDefinedProjection;
  bool flagIncludeExtraHtmlDescriptions;
  bool flagPlotShowJavascriptOnly;
  bool flagAllowMovingCoordinateSystemFromArbitraryClick;
  bool flagUseGraphicsOld;
  Vectors<Rational> FillUserDefinedProjection;
  int Selected;
  int textX;
  int textY;
  int fontSizeNormal;
  int fontSizeSubscript;
  int ColorTextDefault;
  int ColorTextZeroChamber;
  int ColorTextPermanentlyZeroChamber;
  int ColorChamberIndicator;
  int ColorWeylChamberWalls;
  int defaultHtmlWidth;
  int defaultHtmlHeight;
  void initDrawingVariables();
  DrawingVariables() {
    this->initDrawingVariables();
  }
  int getColorFromChamberIndex(int index);
  static void projectOnToHyperPlaneGraphics(Vector<Rational>& input, Vector<Rational>& output);
  static std::string getColorHtmlFromColorIndex(int colorIndex);
  std::string getColorPsTricksFromColorIndex(int colorIndex);
  static bool getColorIntFromColorString(const std::string& input, int& output);
  DrawOperations operations;
  int getActualPenStyleFromFlagsAnd(int inputPenStyle);
  int getActualTextStyleFromFlagsAnd(int inputTextStyle);

  std::string getHTMLDiv(
    int dimension,
    bool useSpanTag
  );
  void drawString(DrawElementInputOutput& drawData, const std::string& input, int fontSize, int textStyle);
  void drawCoordSystemBuffer(DrawingVariables& variables, int dimension);
  void drawLineDirectly(
    double x1, double y1, double x2, double y2, uint32_t thePenStyle, int colorIndex, double lineWidth
  );
  void drawLineBuffer(
    double x1,
    double y1,
    double x2,
    double y2,
    uint32_t thePenStyle,
    int colorIndex,
    double lineWidth
  );
  void drawTextDirectly(double x1, double y1, const std::string& inputText, int color, std::fstream* latexOutFile);
  void drawTextBuffer(double X1, double Y1, const std::string& inputText, int color);
  //if the LatexOutFile is zero then the procedure defaults to the screen
  void drawLineBufferOld(double x1, double y1, double x2, double y2, uint32_t thePenStyle, int colorIndex, std::fstream* latexOutFile);
  void drawLineBetweenTwoVectorsBufferRational(
    const Vector<Rational>& r1, const Vector<Rational>& r2, const std::string& color, double lineWidth = 1
  ) {
    this->operations.drawLineBetweenTwoVectorsBufferRational(r1, r2, color, lineWidth);
  }
  void drawCircleAtVector(
    const Vector<Rational>& point,
    const std::string& color,
    double radius,
    const std::string& frameId = "",
    int frameIndex = - 1
  ) {
    this->operations.drawCircleAtVectorBufferRational(point, color, radius, frameId, frameIndex);
  }
  void drawPath(
    const Vectors<Rational>& vectors,
    const std::string& color,
    double lineWidth,
    const std::string& frameId = "",
    int frameIndex = - 1
  ) {
    this->operations.drawPath(vectors, color, lineWidth, frameId, frameIndex);
  }
  void drawLineBetweenTwoVectorsBufferDouble(
    const Vector<double>& r1, const Vector<double>& r2, const std::string& color, double lineWidth = 1
  ) {
    this->operations.drawLineBetweenTwoVectorsBufferDouble(r1, r2, color, lineWidth);
  }
  void drawTextAtVectorBufferRational(const Vector<Rational>& point, const std::string& inputText, const std::string& color);
  void drawTextAtVectorBufferDouble(const Vector<double>& point, const std::string& inputText, int textColor, int textStyle);
  void drawCircleAtVectorBufferRational(
    const Vector<Rational>& point, const std::string& color, double radius, const std::string& frameId = "", int frameIndex = - 1
  );
  void drawCircleAtVectorBufferDouble(const Vector<double>& point, const std::string& color, double radius);
  void operator=(const DrawingVariables& other) {
    this->drawLineFunction = other.drawLineFunction;
    this->drawTextFunction = other.drawTextFunction;
  }
};
#endif
