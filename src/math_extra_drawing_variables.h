
#ifndef header_math_extra_drawing_variables_ALREADY_INCLUDED
#define header_math_extra_drawing_variables_ALREADY_INCLUDED

#include "math_general.h"

class DrawingVariables;
class DrawGeneric;

class DrawOperation {
public:
  MemorySaving<DrawGeneric> drawGeneric;
  void toJSON(JSData& output, const DrawingVariables& owner) const;
  JSData toJSON(const DrawingVariables& owner) const;
  void toLatexPsTricks(
    std::stringstream& out, const DrawingVariables& owner
  ) const;
  DrawOperation();
  DrawOperation(const DrawGeneric& other);
  void operator=(const DrawGeneric& other);
  std::string toString() const;
};

// Draws a generic object by bouncing back a JSData object.
class DrawGeneric {
public:
  JSData content;
  void toJSON(JSData& output, const DrawingVariables& owner) const;
  void toLatexPsTricks(
    std::stringstream& out, const DrawingVariables& owner
  ) const;
  std::string toString() const;
};

// Represents graphics in the calculator.
// There are two classes that do that: Plot and DrawingVariables.
// DrawingVariables supports arbitrary dimension > 2, Plot supports only
// dimensions 2 and 3. It will be nice to unify these two classes into a
// single class; until then, please use transition functions.
class DrawingVariables {
private:
  void changeBasisPReserveAngles(double newX, double newY);
public:
  List<DrawOperation> operations;
  List<List<double> > projectionsEiVectors;
  Vectors<double> basisProjectionPlane;
  static const int graphicsUnitDefault = 100;
  int selectedCircleMinus2noneMinus1Center;
  // -2= none, - 1= center of coordinate system, nonnegative integers =
  // selectedindex
  Vectors<double> basisToDrawCirclesAt;
  Matrix<double> bilinearForm;
  MapList<
    std::string,
    List<int>,
    HashFunctions::hashFunction<std::string>
  > frames;
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
  enum PenStyles {
    PenStyleInvisible,
    PenStyleDashed,
    PenStyleDotted,
    PenStyleNormal,
    PenStyleZeroChamber,
    PenStylePermanentlyZeroChamber,
    PenStyleLinkToOriginZeroChamber,
    PenStyleLinkToOrigin,
    PenStyleLinkToOriginPermanentlyZeroChamber  };
  enum TextStyles {
    TextStyleNormal,
    TextStyleInvisible,
    TextStyleChamber,
    TextStyleZeroChamber,
    TextStylePermanentlyZeroChamber  };
  int colorDashes;
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
  Vectors<Rational> fillUserDefinedProjection;
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
  static void projectOnToHyperPlaneGraphics(
    Vector<Rational>& input, Vector<Rational>& output
  );
  static std::string getColorHtmlFromColorIndex(int colorIndex);
  std::string getColorPsTricksFromColorIndex(int colorIndex);
  static bool getColorIntFromColorString(
    const std::string& input, int& output
  );
  void scaleToUnitLength(Vector<double>& root) {
    double length = this->bilinearForm.scalarProduct(root, root);
    length = FloatingPoint::sqrtFloating(length);
    root /= length;
  }
  int getActualPenStyleFromFlagsAnd(int inputPenStyle);
  int getActualTextStyleFromFlagsAnd(int inputTextStyle);
  std::string getHTMLDiv(
    int dimension, bool useSpanTag, bool generateInfoPanels
  );
  void drawString(
    DrawElementInputOutput& drawData,
    const std::string& input,
    int fontSize,
    int textStyle
  );
  void drawCoordSystemBuffer(DrawingVariables& variables, int dimension);
  void drawLineDirectly(
    double x1,
    double y1,
    double x2,
    double y2,
    uint32_t penStyle,
    int colorIndex,
    double lineWidth
  );
  void drawLineBuffer(
    double x1,
    double y1,
    double x2,
    double y2,
    uint32_t penStyle,
    int colorIndex,
    double lineWidth
  );
  void drawTextBuffer(
    double X1, double Y1, const std::string& inputText, int color
  );
  void drawPath(
    const Vectors<Rational>& vectors,
    const std::string& color,
    double lineWidth,
    const std::string& frameId = "",
    int frameIndex = - 1
  );
  void drawLineBetweenTwoVectorsBufferDouble(
    const Vector<double>& r1,
    const Vector<double>& r2,
    const std::string& color,
    double lineWidth = 1
  );
  void drawTextAtVectorBufferRational(
    const Vector<Rational>& point,
    const std::string& inputText,
    const std::string& color
  );
  void drawTextAtVectorBufferRational(
    const Vector<Rational>& input,
    const std::string& inputText,
    const std::string& color,
    int fontSize
  );
  void drawTextAtVectorBufferDouble(
    const Vector<double>& point,
    const std::string& inputText,
    const std::string& color,
    int textStyle
  );
  void drawCircleAtVectorBufferRational(
    const Vector<Rational>& point,
    const std::string& color,
    double radius,
    const std::string& frameId = "",
    int frameIndex = - 1
  );
  void drawCircleAtVectorBufferDouble(
    const Vector<double>& point,
    const std::string& color,
    double radius
  );
  static void projectionMultiplicityMergeOnBasisChange(
    DrawingVariables& operations
  );
  void makeMeAStandardBasis(int dimension);
  void operator+=(const DrawingVariables& other);
  void initDimensions(
    Matrix<double>& bilinearForm,
    Vectors<double>& draggableBasis,
    Vectors<double>& startingPlane
  );
  void initDimensions(
    Matrix<Rational>& bilinearForm,
    Vectors<double>& draggableBasis,
    Vectors<double>& startingPlane
  );
  void initDimensions(int dimension);
  int getDimensionFromBilinearForm();
  void getCoordinatesDrawingComputeAll(
    Vector<double>& input, double& x1, double& y1
  );
  void getCoordinatesForDrawingProjectionsComputed(
    Vector<double>& input, double& x1, double& y1
  );
  void getCoordinatesForDrawingProjectionsComputed(
    Vector<double>& input1,
    Vector<double>& input2,
    double& x1,
    double& y1,
    double& x2,
    double& y2
  );
  bool areWithinClickTolerance(double x1, double y1, double x2, double y2);
  bool mouseMoveRedraw(int x, int y);
  void click(double x, double y);
  void drawHighlightGroup(
    Vectors<double>& highlightGroup,
    List<std::string>& labels,
    const std::string& color,
    int radius
  );
  void drawTextBuffer(
    double x1,
    double y1,
    const std::string& inputText,
    int colorIndex,
    int fontSize,
    int textStyle
  );
  void drawLineBetweenTwoVectorsBufferRational(
    const Vector<Rational>& vector1,
    const Vector<Rational>& vector2,
    const std::string& color,
    double lineWidth = 1
  );
  void drawFilledShape(
    const List<Vector<double> >& corners,
    uint32_t penStyle,
    int colorIndex,
    int fillColorIndex,
    double lineWidth
  );
  void drawTextAtVectorBufferDouble(
    const Vector<double>& input,
    const std::string& inputText,
    const std::string& color,
    int fontSize,
    int textStyle
  );
  double getAngleFromXandY(double x, double y);
  void rotateOutOfPlane(
    std::stringstream& Logger,
    Vector<double>& input,
    Vector<double>& output,
    Vector<double>& orthoBasis1,
    Vector<double>& orthoBasis2,
    double oldTanSquared,
    double newTanSquared
  );
  void modifyToOrthonormalNoShiftSecond(
    Vector<double>& root1, Vector<double>& root2
  );
  void computeProjectionsEiVectors();
  std::string toLatexPsTricks() const;
  void initialize();
  void toJSON(JSData& output);
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

#endif // header_math_extra_drawing_variables_ALREADY_INCLUDED
