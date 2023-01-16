
#ifndef header_math_extra_drawing_variables_ALREADY_INCLUDED
#define header_math_extra_drawing_variables_ALREADY_INCLUDED

#include "math_general.h"

class DrawingVariables;

// Draws a generic object by bouncing back a JSData object.
class DrawGeneric {
public:
  JSData content;
  void toJSON(JSData& output, const DrawingVariables& owner) const;
  void toLatexPsTricks(
    std::stringstream& out, const DrawingVariables& owner
  ) const;
  std::string toString() const;
  void accountBoundingBox(DrawingVariables& owner) const;
};

class DrawOptions {
public:
  enum PenStyle {
    normal,
    invisible,
    dashed,
    PenStyleDotted,
    PenStyleZeroChamber,
    PenStylePermanentlyZeroChamber,
    PenStyleLinkToOriginZeroChamber,
    PenStyleLinkToOrigin,
    PenStyleLinkToOriginPermanentlyZeroChamber,
  };
  static std::string penStyleNormal;
  static std::string penStyleDashed;
  std::string color;
  int lineWidth;
  DrawOptions::PenStyle penStyle;
  std::string layer;
  std::string toStringPsTricks() const;
  static std::string toPsTricksPenStyle(DrawOptions::PenStyle input);
  static std::string toJSONPenStyle(DrawOptions::PenStyle input);
  void set(const std::string& inputColor, int inputLineWidth);
  void writeJSON(JSData& output) const;
  DrawOptions();
};

// Draws a segment between two vectors in n dimensions.
class DrawSegment {
public:
  Vector<double> vector1;
  Vector<double> vector2;
  DrawOptions drawOptions;
  void toJSON(JSData& output, const DrawingVariables& owner) const;
  void toLatexPsTricks(
    std::stringstream& out, const DrawingVariables& owner
  ) const;
  void accountBoundingBox(DrawingVariables& owner) const;
};

class DrawText {
public:
  Vector<double> location;
  std::string text;
  DrawOptions drawOptions;
  void toJSON(JSData& output, const DrawingVariables& owner) const;
  void toLatexPsTricks(
    std::stringstream& out, const DrawingVariables& owner
  ) const;
  void accountBoundingBox(DrawingVariables& owner) const;
};

class DrawCircle {
public:
  Vector<double> location;
  double radius;
  DrawOptions drawOptions;
  std::string frameId;
  int frameIndex;
  void toJSON(JSData& output, const DrawingVariables& owner) const;
  void toLatexPsTricks(
    std::stringstream& out, const DrawingVariables& owner
  ) const;
  void accountBoundingBox(DrawingVariables& owner) const;
};

class DrawOperation {
  template <class Implementation>
  MemorySaving<Implementation>& getImplementation();
  template <class Implementation>
  bool toJSONImplementation(JSData& output, const DrawingVariables& owner)
  const {
    DrawOperation copy = *this;
    MemorySaving<Implementation>& implementation =
    copy.getImplementation<Implementation>();
    if (implementation.isZeroPointer()) {
      return false;
    }
    implementation.getElementConst().toJSON(output, owner);
    return true;
  }
  template <class Implementation>
  bool toLatexPsTricksImplementation(
    std::stringstream& out, const DrawingVariables& owner
  ) const {
    DrawOperation copy = *this;
    MemorySaving<Implementation>& implementation =
    copy.getImplementation<Implementation>();
    if (implementation.isZeroPointer()) {
      return false;
    }
    implementation.getElementConst().toLatexPsTricks(out, owner);
    return true;
  }
  template <class Implementation>
  bool accountBoundingBoxImplementation(DrawingVariables& owner) {
    MemorySaving<Implementation>& implementation =
    this->getImplementation<Implementation>();
    if (implementation.isZeroPointer()) {
      return false;
    }
    implementation.getElementConst().accountBoundingBox(owner);
    return true;
  }
public:
  MemorySaving<DrawGeneric> drawGeneric;
  MemorySaving<DrawSegment> drawSegment;
  MemorySaving<DrawText> drawText;
  MemorySaving<DrawCircle> drawCircle;
  bool toJSON(JSData& output, const DrawingVariables& owner) const;
  JSData toJSON(const DrawingVariables& owner) const;
  bool toLatexPsTricks(
    std::stringstream& out, const DrawingVariables& owner
  ) const;
  bool accountBoundingBox(DrawingVariables& owner);
  DrawOperation();
  template <class Implementation>
  DrawOperation(const Implementation& other) {
    this->getImplementation<Implementation>().getElement() = other;
  }
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
  List<Vector<double> > boundingBoxLatex;
  static const int graphicsUnitDefault = 100;
  int selectedCircleMinus2noneMinus1Center;
  // - 2 = none,
  // - 1 = center of coordinate system,
  // nonnegative integers = selectedindex
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
  enum TextStyles {
    TextStyleNormal,
    TextStyleInvisible,
    TextStyleChamber,
    TextStyleZeroChamber,
    TextStylePermanentlyZeroChamber,
  };
  int fontSizeNormal;
  int defaultHtmlWidth;
  int defaultHtmlHeight;
  void initDrawingVariables();
  DrawingVariables();
  int getColorFromChamberIndex(int index);
  static void projectOnToHyperPlaneGraphics(
    Vector<Rational>& input, Vector<Rational>& output
  );
  static std::string getColorHtmlFromColorIndex(int colorIndex);
  std::string getColorPsTricksFromColorIndex(int colorIndex);
  static bool getColorIntFromColorString(
    const std::string& input, int& output
  );
  void scaleToUnitLength(Vector<double>& root);
  std::string getHTMLDiv(
    int dimension, bool useSpanTag, bool generateInfoPanels
  );
  void drawString(
    DrawElementInputOutput& drawData,
    const std::string& input,
    int fontSize,
    int textStyle
  );
  void drawCoordinateSystemBuffer(int dimension);
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
    double x1, double y1, const std::string& inputText, int color
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
    double lineWidth = 1,
    DrawOptions::PenStyle penstyle = DrawOptions::PenStyle::normal,
    const std::string& layer = ""
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
    const std::string& color
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
    double radius,
    const std::string& frameId = "",
    int frameIndex = - 1
  );
  static void projectionMultiplicityMergeOnBasisChange(
    DrawingVariables& operations
  );
  void computeBoundingBox();
  void makeMeAStandardBasis(int dimension);
  void operator+=(const DrawingVariables& other);
  void initializeDimensions(
    Matrix<double>& bilinearForm,
    Vectors<double>& draggableBasis,
    Vectors<double>& startingPlane
  );
  void initializeDimensions(
    Matrix<Rational>& bilinearForm,
    Vectors<double>& draggableBasis,
    Vectors<double>& startingPlane
  );
  void initializeDimensions(int dimension);
  int getDimensionFromBilinearForm();
  void getCoordinatesDrawingComputeAll(
    Vector<double>& input, double& x1, double& y1
  ) const;
  void getCoordinatesForDrawingProjectionsComputed(
    const Vector<double>& input, double& x1, double& y1
  ) const;
  void getCoordinatesForDrawingProjectionsComputed(
    const Vector<double>& input1,
    const Vector<double>& input2,
    double& x1,
    double& y1,
    double& x2,
    double& y2
  ) const;
  Vector<double> getCoordinatesPixels(const Vector<double>& input) const;
  Vector<double> getCoordinatesLatex(const Vector<double>& input) const;
  void accountBoundingBox(const Vector<double>& input);
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
    double lineWidth = 1,
    DrawOptions::PenStyle penstyle = DrawOptions::PenStyle::normal,
    const std::string& layer = ""
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
    int fontSize
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
  double latexUnit() const;
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
  static std::string fieldPoints;
  static std::string fieldPenStyle;
  static std::string fieldLayer;
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

class ImpreciseDouble {
private:
  double precision;
  double value;
public:
  std::string toString(FormatExpressions* format = nullptr) const {
    (void) format;
    std::stringstream out;
    out << this->value;
    return out.str();
  }
  void operator=(const ImpreciseDouble& other) {
    this->value = other.value;
    this->precision = other.precision;
  }
  void operator=(double other) {
    this->value = other;
  }
  ImpreciseDouble(const ImpreciseDouble& other) {
    this->operator=(other);
  }
  ImpreciseDouble() {
    this->value = 0;
    this->precision = 0.1;
  }
  ImpreciseDouble(double other) {
    this->precision = 0.1;
    this->operator=(other);
  }
  void operator+=(const ImpreciseDouble& other) {
    if (!other.isEqualToZero()) {
      this->value += other.value;
    }
  }
  void operator-=(const ImpreciseDouble& other) {
    if (!other.isEqualToZero()) {
      this->value -= other.value;
    }
  }
  void operator=(const Rational& other) {
    this->value = other.getDoubleValue();
  }
  bool isEqualToZero() const {
    if (this->value < 0) {
      return (- value) < this->precision;
    }
    return this->value < this->precision;
  }
  bool operator<=(const ImpreciseDouble& other) {
    return !(other < *this);
  }
  bool isPositive() const {
    return this->value > this->precision;
  }
  bool isNegative() const {
    return *this < this->zero();
  }
  bool operator<(const ImpreciseDouble& other) const {
    ImpreciseDouble temp = other;
    temp -= *this;
    return temp.isPositive();
  }
  void assignFloor() {
    this->value = FloatingPoint::floorFloating(this->value);
  }
  void operator/=(const ImpreciseDouble& other) {
    ImpreciseDouble copyMe;
    copyMe = *this;
    *this = copyMe / other;
  }
  ImpreciseDouble operator/(const ImpreciseDouble& other) const {
    ImpreciseDouble result;
    result = *this;
    if (other.isEqualToZero()) {
      // The following is written like this to
      // avoid this->value / 0;
      // If the user attempts to divide by zero,
      // I want a regular division by zero exception to be generated.
      result.value = this->value / (other.value - other.value);
      return result;
    }
    result.value /= other.value;
    return result;
  }
  void operator*=(const ImpreciseDouble& other) {
    if (!other.isEqualToZero()) {
      this->value *= other.value;
    } else {
      this->value = 0;
    }
  }
  bool operator==(const ImpreciseDouble& other) const {
    double difference = this->value - other.value;
    if (difference < 0) {
      difference = - difference;
    }
    return difference < this->precision;
  }
  static ImpreciseDouble minusOne() {
    return - 1;
  }
  static ImpreciseDouble getOne() {
    return 1;
  }
  static ImpreciseDouble zero() {
    return 0;
  }
};

#endif // header_math_extra_drawing_variables_ALREADY_INCLUDED
