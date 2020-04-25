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
  List<List<double> > ProjectionsEiVectors;
  Vectors<double> BasisProjectionPlane;
  static const int GraphicsUnitDefault = 100;
  int SelectedCircleMinus2noneMinus1Center; //-2= none, - 1= center of coordinate system, nonnegative integers = selectedindex
  Vectors<double> BasisToDrawCirclesAt;
  Matrix<double> theBilinearForm;
  MapList<std::string, List<int>, MathRoutines::HashString> frames;
  double ClickToleranceX;
  double ClickToleranceY;
  double centerX;
  double centerY;
  double GraphicsUnit;
  int frameLengthInMilliseconds;
  bool flagRotatingPreservingAngles;
  bool flagAnimatingMovingCoordSystem;
  bool flagIsPausedWhileAnimating;
  std::string DebugString;
  int indexStartingModifiableTextCommands;
  void (*specialOperationsOnBasisChange)(DrawOperations& theOps);
  static void projectionMultiplicityMergeOnBasisChange(DrawOperations& theOps);
  void MakeMeAStandardBasis(int theDim);
  void operator+=(const DrawOperations& other);
  void initDimensions(Matrix<double>& bilinearForm, Vectors<double>& draggableBasis, Vectors<double>& startingPlane) {
    this->theBilinearForm = bilinearForm;
    this->BasisToDrawCirclesAt = draggableBasis;
    this->BasisProjectionPlane = startingPlane;
    this->centerX = 300;
    this->centerY = 300;
    this->GraphicsUnit = DrawOperations::GraphicsUnitDefault;
    this->ComputeProjectionsEiVectors();
  }
  void initDimensions(Matrix<Rational>& bilinearForm, Vectors<double>& draggableBasis, Vectors<double>& startingPlane) {
    Matrix<double> tempMat;
    tempMat.initialize(bilinearForm.numberOfRows, bilinearForm.numberOfColumns);
    for (int i = 0; i < bilinearForm.numberOfRows; i ++) {
      for (int j = 0; j < bilinearForm.numberOfColumns; j ++) {
        tempMat.elements[i][j] = bilinearForm.elements[i][j].GetDoubleValue();
      }
    }
    this->initDimensions(tempMat, draggableBasis, startingPlane);
  }
  void initDimensions(int theDim);
  int GetDimFirstDimensionDependentOperation();
  int GetDimFromBilinearForm();
  void GetCoordsDrawingComputeAll(Vector<double>& input, double& X1, double& Y1) {
    X1 = this->theBilinearForm.scalarProduct(input, this->BasisProjectionPlane[0]);
    Y1 = this->theBilinearForm.scalarProduct(input, this->BasisProjectionPlane[1]);
    X1 = X1 * this->GraphicsUnit + this->centerX;
    Y1 = Y1 * this->GraphicsUnit + this->centerY;
  }
  void GetCoordsForDrawingProjectionsComputed(Vector<double>& input, double& X1, double& Y1) {
    X1 = 0;
    Y1 = 0;
    for (int j = 0; j < input.size; j ++) {
      X1 += this->ProjectionsEiVectors[j][0] * input[j];
      Y1 += this->ProjectionsEiVectors[j][1] * input[j];
    }
    X1 = X1 * this->GraphicsUnit + this->centerX;
    Y1 = Y1 * this->GraphicsUnit + this->centerY;
  }
  void GetCoordsForDrawingProjectionsComputed(Vector<double>& input1, Vector<double>& input2, double& X1, double& Y1, double& X2, double& Y2) {
    X1 = 0;
    X2 = 0;
    Y1 = 0;
    Y2 = 0;
    for (int j = 0; j < input1.size; j ++) {
      X1 += this->ProjectionsEiVectors[j][0] * input1[j];
      Y1 += this->ProjectionsEiVectors[j][1] * input1[j];
      X2 += this->ProjectionsEiVectors[j][0] * input2[j];
      Y2 += this->ProjectionsEiVectors[j][1] * input2[j];
    }
    X1 = X1 * this->GraphicsUnit + this->centerX;
    X2 = X2 * this->GraphicsUnit + this->centerX;
    Y1 = Y1 * this->GraphicsUnit + this->centerY;
    Y2 = Y2 * this->GraphicsUnit + this->centerY;
  }
  void EnsureProperInitialization();
  bool AreWithinClickTolerance(double x1, double y1, double x2, double y2) {
    x1 -= x2;
    y1 -= y2;
    if (x1 < 0) {
      x1 = - x1;
    }
    if (y1 < 0) {
      y1 = - y1;
    }
    return x1 <= this->ClickToleranceX && y1 <= this->ClickToleranceY;
  }
  bool mouseMoveRedraw(int X, int Y) {
    if (this->SelectedCircleMinus2noneMinus1Center == - 2) {
      return false;
    }
    if (this->SelectedCircleMinus2noneMinus1Center == - 1) {
      this->centerX = X;
      this->centerY = Y;
      return true;
    }
    if (this->SelectedCircleMinus2noneMinus1Center >= 0) {
      if (this->flagRotatingPreservingAngles) {
        this->changeBasisPReserveAngles(static_cast<double>(X) , static_cast<double>(Y));
        return true;
      }
    }
    return false;
  //  this->draw();
  }
  void click(double x, double y);
  void drawHighlightGroup(Vectors<double>& highlightGroup, List<std::string>& labels, const std::string& color, int radius);
  void drawLineBuffer(
    double X1, double Y1, double X2, double Y2, uint32_t thePenStyle, int ColorIndex, double lineWidth
  );
  void drawTextBuffer(double X1, double Y1, const std::string& inputText, int ColorIndex, int theFontSize, int theTextStyle);
  void drawLineBetweenTwoVectorsBufferRational(
    const Vector<Rational>& vector1, const Vector<Rational>& vector2, const std::string& color, double lineWidth = 1
  );
  void drawFilledShape(
    const List<Vector<double> >& theCorners, uint32_t thePenStyle, int ColorIndex, int fillColorIndex, double lineWidth
  );
  void drawLineBetweenTwoVectorsBufferDouble(
    const Vector<double>& vector1, const Vector<double>& vector2, const std::string& color, double lineWidth = 1
  );
  void drawPath(
    const Vectors<Rational>& theVectors, const std::string& color, double lineWidth, const std::string& frameId, int frameIndex
  );
  void drawTextAtVectorBufferRational(const Vector<Rational>& input, const std::string& inputText, const std::string& color, int fontSize);
  void drawTextAtVectorBufferDouble(const Vector<double>& input, const std::string& inputText, int ColorIndex, int theFontSize, int theTextStyle);
  void drawCircleAtVectorBufferRational(
    const Vector<Rational>& input, const std::string& color, double radius, const std::string& frameId = "", int frameIndex = - 1
  );
  void drawCircleAtVectorBufferDouble(const Vector<double>& input, const std::string& color, double radius);
  double getAngleFromXandY(double x, double y);
  void ScaleToUnitLength(Vector<double>& theRoot) {
    double theLength = this->theBilinearForm.scalarProduct(theRoot, theRoot);
    theLength = FloatingPoint::Sqrt(theLength);
    theRoot /= theLength;
  }
  void RotateOutOfPlane(
    std::stringstream& Logger,
    Vector<double>& input,
    Vector<double>& output,
    Vector<double>& orthoBasis1,
    Vector<double>& orthoBasis2,
    double oldTanSquared,
    double newTanSquared
  );
  void ModifyToOrthonormalNoShiftSecond(Vector<double>& root1, Vector<double>& root2);
  void ComputeXYsFromProjectionsEisAndGraphicsUnit();
  void ComputeProjectionsEiVectors();
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
private:
  drawLineFunction theDrawLineFunction;
  drawTextFunction theDrawTextFunction;
  drawCircleFunction theDrawCircleFunction;
  drawClearScreenFunction theDrawClearScreenFunction;
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
  //MutexRecursiveWrapper LockedWhileDrawing;
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
  int DefaultHtmlWidth;
  int DefaultHtmlHeight;
  void initDrawingVariables();
  DrawingVariables() {
    this->initDrawingVariables();
  }
  void SetDrawLineFunction(drawLineFunction theFunction) {
    this->theDrawLineFunction = theFunction;
  }
  void SetDrawTextFunction(drawTextFunction theFunction) {
    this->theDrawTextFunction = theFunction;
  }
  void SetDrawCircleFunction(drawCircleFunction theFunction) {
    this->theDrawCircleFunction = theFunction;
  }
  void SetDrawClearFunction(drawClearScreenFunction theFunction) {
    this->theDrawClearScreenFunction = theFunction;
  }
  int GetColorFromChamberIndex(int index);
  static void GetCoordsForDrawing(DrawingVariables& TDV, Vector<Rational>& r, double& x, double& y);
  static void ProjectOnToHyperPlaneGraphics(Vector<Rational>& input, Vector<Rational>& output);
  static std::string GetColorHtmlFromColorIndex(int colorIndex);
  std::string GetColorPsTricksFromColorIndex(int colorIndex);
  static bool GetColorIntFromColorString(const std::string& input, int& output);
  DrawOperations theBuffer;
  int GetActualPenStyleFromFlagsAnd(int inputPenStyle);
  int GetActualTextStyleFromFlagsAnd(int inputTextStyle);

  std::string GetHtmlDiv(int theDimension);
  void drawString(DrawElementInputOutput& theDrawData, const std::string& input, int theFontSize, int theTextStyle);
  void drawCoordSystemDirectlly(DrawingVariables& TDV, int theDimension, std::fstream* LatexOutFile);
  void drawCoordSystemBuffer(DrawingVariables& TDV, int theDimension);
  void drawLineDirectly(
    double X1, double Y1, double X2, double Y2, uint32_t thePenStyle, int ColorIndex, double lineWidth
  );
  void drawLineBuffer(
    double X1, double Y1, double X2, double Y2, uint32_t thePenStyle, int ColorIndex, double lineWidth
  );
  void drawTextDirectly(double X1, double Y1, const std::string& inputText, int color, std::fstream* LatexOutFile);
  void drawTextBuffer(double X1, double Y1, const std::string& inputText, int color);
  //if the LatexOutFile is zero then the procedure defaults to the screen
  void drawLineBufferOld(double X1, double Y1, double X2, double Y2, uint32_t thePenStyle, int ColorIndex, std::fstream* LatexOutFile);
  void drawLineBetweenTwoVectorsBufferRational(
    const Vector<Rational>& r1, const Vector<Rational>& r2, const std::string& color, double lineWidth = 1
  ) {
    this->theBuffer.drawLineBetweenTwoVectorsBufferRational(r1, r2, color, lineWidth);
  }
  void drawCircleAtVector(
    const Vector<Rational>& point, const std::string& color, double radius, const std::string& frameId = "", int frameIndex = - 1
  ) {
    this->theBuffer.drawCircleAtVectorBufferRational(point, color, radius, frameId, frameIndex);
  }
  void drawPath(
    const Vectors<Rational>& theVectors,
    const std::string& color,
    double lineWidth,
    const std::string& frameId = "",
    int frameIndex = - 1
  ) {
    this->theBuffer.drawPath(theVectors, color, lineWidth, frameId, frameIndex);
  }
  void drawLineBetweenTwoVectorsBufferDouble(
    const Vector<double>& r1, const Vector<double>& r2, const std::string& color, double lineWidth = 1
  ) {
    this->theBuffer.drawLineBetweenTwoVectorsBufferDouble(r1, r2, color, lineWidth);
  }
  void drawTextAtVectorBufferRational(const Vector<Rational>& point, const std::string& inputText, const std::string& color);
  void drawTextAtVectorBufferDouble(const Vector<double>& point, const std::string& inputText, int textColor, int theTextStyle);
  void drawCircleAtVectorBufferRational(
    const Vector<Rational>& point, const std::string& color, double radius, const std::string& frameId = "", int frameIndex = - 1
  );
  void drawCircleAtVectorBufferDouble(const Vector<double>& point, const std::string& color, double radius);
  void operator=(const DrawingVariables& other) {
    this->theDrawLineFunction = other.theDrawLineFunction;
    this->theDrawTextFunction = other.theDrawTextFunction;
  }
};
#endif
