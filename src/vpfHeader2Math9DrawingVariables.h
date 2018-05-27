//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#ifndef vpfHeaderDrawingVars_Already_included
#define vpfHeaderDrawingVars_Already_included

#include "vpfHeader2Math0_General.h"

static ProjectInformationInstance ProjectInfoVpfHeaderDrawingVars(__FILE__, "Header, drawing routines. ");

class DrawTextOperation
{
public:
  double X1;
  double Y1;
  std::string theText;
  int ColorIndex;
  int fontSize;
  int TextStyle;
  void init(double x1, double y1, const std::string& inputText, int color, int theFontSize, int theTextStyle)
  { this->X1 = x1;
    this->Y1 = y1;
    this->theText = inputText;
    this->ColorIndex = color;
    this->fontSize = theFontSize;
    this->TextStyle = theTextStyle;
  }
};

class DrawLineOperation
{
public:
  double X1;
  double Y1;
  double X2;
  double Y2;
  int thePenStyle;
  int ColorIndex;
  double lineWidth;
  inline void init(double x1, double y1, double x2, double y2, uint32_t PenStyle, int colorIndex, double inputLineWidth)
  { this->X1 = x1;
    this->Y1 = y1;
    this->X2 = x2;
    this->Y2 = y2;
    this->thePenStyle = PenStyle;
    this->ColorIndex = colorIndex;
    this->lineWidth = inputLineWidth;
  }
};

class DrawFilledShapeOperation
{
public:
  List<Vector<double> > theCorners;
  int thePenStyle;
  int ColorIndex;
  int ColorFillIndex;
  double lineWidth;
  DrawFilledShapeOperation();
};


class DrawLineBetweenTwoRootsOperation
{
public:
  Vector<double> v1;
  Vector<double> v2;
  int thePenStyle;
  int ColorIndex;
  double lineWidth;
  double precomputedX1, precomputedY1, precomputedX2, precomputedY2;
  void init(const Vector<Rational>& input1, const Vector<Rational>& input2, uint32_t PenStyle, int colorIndex, double inputLineWidth);
  void init(const Vector<double>& input1, const Vector<double>& input2, uint32_t PenStyle, int colorIndex, double inputLineWidth);
};

class DrawTextAtVectorOperation
{
public:
  Vector<double> theVector;
  std::string theText;
  int ColorIndex;
  int fontSize;
  int TextStyle;
  double precomputedX, precomputedY;
  void init(const Vector<Rational>& input, const std::string& inputText, int colorIndex, int theFontSize, int theTextStyle)
  { this->theVector.SetSize(input.size);
    for (int i = 0; i < input.size; i ++)
      this->theVector[i] = input[i].GetDoubleValue();
    this->ColorIndex = colorIndex;
    this->theText = inputText;
    this->fontSize = theFontSize;
    this->TextStyle = theTextStyle;
  }
  void init(const Vector<double>& input, const std::string& inputText, int colorIndex, int theFontSize, int theTextStyle)
  { this->theVector = input;
    this->ColorIndex = colorIndex;
    this->theText = inputText;
    this->fontSize = theFontSize;
    this->TextStyle = theTextStyle;
  }
  void operator=(const DrawTextAtVectorOperation& other)
  { this->theVector = other.theVector;
    this->ColorIndex = other.ColorIndex;
    this->theText = other.theText;
    this->fontSize = other.fontSize;
    this->TextStyle = other.TextStyle;
    this->precomputedX = other.precomputedX;
    this->precomputedY = other.precomputedY;
  }
};

class DrawCircleAtVectorOperation
{
public:
  Vector<double> theVector;
  int theMultiplicity;
  double radius;
  int ColorIndex;
  int thePenStyle;
  double precomputedX, precomputedY;
  void initFromVectorRational(const Vector<Rational>& input, double inputRadius, uint32_t inputPenStyle, int colorIndex)
  { this->theVector.SetSize(input.size);
    for (int i = 0; i < input.size; i ++)
      this->theVector[i] = input[i].GetDoubleValue();
    this->initFromVectorDouble(this->theVector, inputRadius, inputPenStyle, colorIndex);
  }
  void initFromVectorDouble(const Vector<double>& input, double inputRadius, uint32_t inputPenStyle, int colorIndex)
  { this->theVector = input;
    this->ColorIndex = colorIndex;
    this->thePenStyle = inputPenStyle;
    this->radius = inputRadius;
    this->theMultiplicity = 1;
  }
  void operator=(const DrawCircleAtVectorOperation& other)
  { this->theVector = other.theVector;
    this->ColorIndex = other.ColorIndex;
    this->thePenStyle = other.thePenStyle;
    this->radius = other.radius;
    this->precomputedX = other.precomputedX;
    this->precomputedY = other.precomputedY;
    this->theMultiplicity = other.theMultiplicity;
  }
};

class DrawOperations
{
private:
  void changeBasisPReserveAngles(double newX, double newY);
public:
  List<int> IndexNthDrawOperation;
  List<int> TypeNthDrawOperation;
  List<DrawFilledShapeOperation> theShapes;
  List<DrawTextOperation> theDrawTextOperations;
  List<DrawLineOperation> theDrawLineOperations;
  List<DrawLineBetweenTwoRootsOperation> theDrawLineBetweenTwoRootsOperations;
  List<DrawTextAtVectorOperation> theDrawTextAtVectorOperations;
  List<DrawCircleAtVectorOperation> theDrawCircleAtVectorOperations;
  List<Vectors<double> > toBeHighlightedWhenLabeledVectorHovered;
  Vectors<double> labeledVectors;
  List<std::string> labelsOfLabeledVectors;
  List<List<double> > ProjectionsEiVectors;
  List<Vectors<double> > BasisProjectionPlane;
  static const int GraphicsUnitDefault = 150;
  int SelectedCircleMinus2noneMinus1Center; //-2= none, -1=center of coordinate system, nonnegative integers= selectedindex
  Vectors<double> BasisToDrawCirclesAt;
  Matrix<double> theBilinearForm;
  double ClickToleranceX;
  double ClickToleranceY;
  List<double> centerX;
  List<double> centerY;
  List<double> GraphicsUnit;
  bool flagRotatingPreservingAngles;
  bool flagAnimatingMovingCoordSystem;
  bool flagIsPausedWhileAnimating;
  int SelectedPlane;
  std::string DebugString;
  int indexStartingModifiableTextCommands;
  void (*specialOperationsOnBasisChange)(DrawOperations& theOps);
  static void projectionMultiplicityMergeOnBasisChange(DrawOperations& theOps);
  void MakeMeAStandardBasis(int theDim);
  void operator+=(const DrawOperations& other);
  void initDimensions(Matrix<double>& bilinearForm, Vectors<double>& draggableBasis, Vectors<double>& startingPlane, int NumAnimationFrames)
  { this->theBilinearForm = bilinearForm;
    this->BasisToDrawCirclesAt = draggableBasis;
    this->BasisProjectionPlane.initFillInObject(NumAnimationFrames, startingPlane);
    this->centerX.initFillInObject(NumAnimationFrames, 300);
    this->centerY.initFillInObject(NumAnimationFrames, 300);
    this->GraphicsUnit.initFillInObject(NumAnimationFrames, DrawOperations::GraphicsUnitDefault);
    this->ComputeProjectionsEiVectors();
  }
  void initDimensions(Matrix<Rational>& bilinearForm, Vectors<double>& draggableBasis, Vectors<double>& startingPlane, int NumAnimationFrames)
  { Matrix<double> tempMat;
    tempMat.init(bilinearForm.NumRows, bilinearForm.NumCols);
    for (int i = 0; i < bilinearForm.NumRows; i ++)
      for (int j = 0; j < bilinearForm.NumCols; j ++)
        tempMat.elements[i][j] = bilinearForm.elements[i][j].GetDoubleValue();
    this->initDimensions(tempMat, draggableBasis, startingPlane, NumAnimationFrames);
  }
  void initDimensions(int theDim, int numAnimationFrames);
  int GetDimFirstDimensionDependentOperation();
  int GetDimFromBilinearForm();
  inline void GetCoordsDrawingComputeAll(Vector<double>& input, double& X1, double& Y1)
  { X1 = this->theBilinearForm.ScalarProduct(input, this->BasisProjectionPlane[this->SelectedPlane][0]);
    Y1 = this->theBilinearForm.ScalarProduct(input, this->BasisProjectionPlane[this->SelectedPlane][1]);
    X1 = X1 * this->GraphicsUnit[this->SelectedPlane] + this->centerX[this->SelectedPlane];
    Y1 = Y1 * this->GraphicsUnit[this->SelectedPlane] + this->centerY[this->SelectedPlane] ;
  }
  inline void GetCoordsForDrawingProjectionsComputed(Vector<double>& input, double& X1, double& Y1)
  { X1 = 0;
    Y1 = 0;
    for (int j = 0; j < input.size; j ++)
    { X1 += this->ProjectionsEiVectors[j][0] * input[j];
      Y1 += this->ProjectionsEiVectors[j][1] * input[j];
    }
    X1 = X1 * this->GraphicsUnit[this->SelectedPlane] + this->centerX[this->SelectedPlane];
    Y1 = Y1 * this->GraphicsUnit[this->SelectedPlane] + this->centerY[this->SelectedPlane];
  }
  inline void GetCoordsForDrawingProjectionsComputed(Vector<double>& input1, Vector<double>& input2, double& X1, double& Y1, double& X2, double& Y2)
  { X1 = 0;
    X2 = 0;
    Y1 = 0;
    Y2 = 0;
    for (int j = 0; j < input1.size; j ++)
    { X1 += this->ProjectionsEiVectors[j][0] * input1[j];
      Y1 += this->ProjectionsEiVectors[j][1] * input1[j];
      X2 += this->ProjectionsEiVectors[j][0] * input2[j];
      Y2 += this->ProjectionsEiVectors[j][1] * input2[j];
    }
    X1 = X1 * this->GraphicsUnit[this->SelectedPlane] + this->centerX[this->SelectedPlane];
    X2 = X2 * this->GraphicsUnit[this->SelectedPlane] + this->centerX[this->SelectedPlane];
    Y1 = Y1 * this->GraphicsUnit[this->SelectedPlane] + this->centerY[this->SelectedPlane];
    Y2 = Y2 * this->GraphicsUnit[this->SelectedPlane] + this->centerY[this->SelectedPlane];
  }
  void EnsureProperInitialization();
  bool AreWithinClickTolerance(double x1, double y1, double x2, double y2)
  { x1 -= x2;
    y1 -= y2;
    if (x1 < 0)
      x1 = - x1;
    if (y1<0)
      y1 = - y1;
    return x1 <= this->ClickToleranceX && y1 <= this->ClickToleranceY;
  }
  bool mouseMoveRedraw(int X, int Y)
  { if (this->SelectedCircleMinus2noneMinus1Center == - 2)
      return false;
    if (this->SelectedCircleMinus2noneMinus1Center == - 1)
    { this->centerX[this->SelectedPlane] = X;
      this->centerY[this->SelectedPlane] = Y;
      return true;
    }
    if (this->SelectedCircleMinus2noneMinus1Center>=0)
    { if (this->flagRotatingPreservingAngles)
      { this->changeBasisPReserveAngles((double) X , (double) Y);
        return true;
      }
    }
    return false;
  //  this->draw();
  }
  void click(double x, double y);
  void drawLineBuffer
  (double X1, double Y1, double X2, double Y2, uint32_t thePenStyle, int ColorIndex,
   double lineWidth);
  void drawTextBuffer(double X1, double Y1, const std::string& inputText, int ColorIndex, int theFontSize, int theTextStyle);
  void drawLineBetweenTwoVectorsBufferRational
  (const Vector<Rational>& vector1, const Vector<Rational>& vector2, uint32_t thePenStyle, int ColorIndex,
   double lineWidth);
  void drawParallelogram
  (const Vector<double>& lowerLeftCorner, const Vector<double>& vector1,
   const Vector<double>& vector2, uint32_t thePenStyle, int ColorIndex, int fillColorIndex,
   double lineWidth);
  void drawFilledShape
  (const List<Vector<double> >& theCorners, uint32_t thePenStyle, int ColorIndex, int fillColorIndex,
   double lineWidth);
  void drawLineBetweenTwoVectorsBufferDouble
  (const Vector<double>& vector1, const Vector<double>& vector2, uint32_t thePenStyle, int ColorIndex,
   double lineWidth);
  void drawTextAtVectorBufferRational(const Vector<Rational>& input, const std::string& inputText, int ColorIndex, int theFontSize, int theTextStyle);
  void drawTextAtVectorBufferDouble(const Vector<double>& input, const std::string& inputText, int ColorIndex, int theFontSize, int theTextStyle);
  void drawCircleAtVectorBufferRational(const Vector<Rational>& input, double radius, uint32_t thePenStyle, int theColor);
  void drawCircleAtVectorBufferDouble(const Vector<double>& input, double radius, uint32_t thePenStyle, int theColor);
  double getAngleFromXandY(double x, double y);
  void ScaleToUnitLength(Vector<double>& theRoot)
  { double theLength = this->theBilinearForm.ScalarProduct(theRoot, theRoot);
    theLength = FloatingPoint::sqrt(theLength);
    theRoot /= theLength;
  }
  void RotateOutOfPlane
  (std::stringstream& logger, Vector<double>& input, Vector<double>& output, Vector<double>& orthoBasis1, Vector<double>& orthoBasis2,
   double oldTanSquared, double newTanSquared);
  void ModifyToOrthonormalNoShiftSecond(Vector<double>& root1, Vector<double>& root2);
  void ComputeXYsFromProjectionsEisAndGraphicsUnit();
  void ComputeProjectionsEiVectors();
  DrawOperations()
  { this->SelectedPlane = 0;
    this->initDimensions(2, 2);
    this->flagAnimatingMovingCoordSystem = false;
    this->specialOperationsOnBasisChange = 0;
    this->indexStartingModifiableTextCommands = 0;
  }
  void init();
  enum DrawOperationType{ typeDrawLine, typeDrawText, typeDrawLineBetweenTwoVectors, typeDrawTextAtVector, typeDrawCircleAtVector, typeFilledShape};
};

class VirtualDrawOp
{
public:
  int theVirtualOp;
  int indexPhysicalFrame;
  int indexPhysicalDrawOp;
  int selectedPlaneInPhysicalDrawOp;
  std::string ToString();
  void operator=(const VirtualDrawOp& other)
  { this->theVirtualOp = other.theVirtualOp;
    this->indexPhysicalFrame = other.indexPhysicalFrame;
    this->indexPhysicalDrawOp = other.indexPhysicalDrawOp;
    this->selectedPlaneInPhysicalDrawOp = other.selectedPlaneInPhysicalDrawOp;
  }
};

class AnimationBuffer
{
public:
//the following items are a part of a dirty hack i need to do for an
//under-stress presentation. Please do not use them !
//they need a rewrite!
  std::string GetHtmlFromDrawOperationsCreateDivWithUniqueName(int theDimension);
  List<DrawingVariables> theFrames;
  DrawOperations theBuffer;
////////////////////////////////////////end of dirty hack
  DrawOperations stillFrame;
  List<DrawOperations> thePhysicalDrawOps;
  List<VirtualDrawOp> theVirtualOpS;
  bool flagAnimating;
  bool flagIsPausedWhileAnimating;
  int indexVirtualOp;
  std::string ToString();
  bool IncrementOpReturnNeedsRedraw();
  int GetIndexCurrentPhysicalFrame();
  void DrawNoInit(DrawingVariables& theDrawingVariables);
  AnimationBuffer();
  DrawOperations& GetCurrentDrawOps();
  DrawOperations& GetLastDrawOps();
  int GetIndexCurrentDrawOps();
  int GetNumPhysicalFramesNoStillFrame();
  void MakeZero();
  void operator+=(const AnimationBuffer& other);
  void operator+=(const DrawOperations& other);
  void operator=(const AnimationBuffer& other);
  void AddPause(int numFrames);
  void AddCloneLastFrameAppendOperations(const DrawOperations& other);
  void AddFrameShift(int numFrames);
  enum {typeDrawOps, typePause, typeClearScreen, typeCloneLastFrameAddOps};
};

class DrawingVariables
{
private:
  drawLineFunction theDrawLineFunction;
  drawTextFunction theDrawTextFunction;
  drawCircleFunction theDrawCircleFunction;
  drawClearScreenFunction theDrawClearScreenFunction;
public:
  enum PenStyles
  { PenStyleInvisible, PenStyleDashed, PenStyleDotted, PenStyleNormal, PenStyleZeroChamber, PenStylePermanentlyZeroChamber, PenStyleLinkToOriginZeroChamber, PenStyleLinkToOrigin, PenStyleLinkToOriginPermanentlyZeroChamber };
  enum TextStyles
  { TextStyleNormal, TextStyleInvisible, TextStyleChamber, TextStyleZeroChamber, TextStylePermanentlyZeroChamber };
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
  Vectors<Rational> FillUserDefinedProjection;
  int Selected;
  static int NumHtmlGraphics;
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
  DrawingVariables()
  { this->initDrawingVariables();
  }
  void SetDrawLineFunction(drawLineFunction theFunction)
  { this->theDrawLineFunction = theFunction;
  }
  void SetDrawTextFunction(drawTextFunction theFunction)
  { this->theDrawTextFunction = theFunction;
  }
  void SetDrawCircleFunction(drawCircleFunction theFunction)
  { this->theDrawCircleFunction = theFunction;
  }
  void SetDrawClearFunction(drawClearScreenFunction theFunction)
  { this->theDrawClearScreenFunction = theFunction;
  }
  int GetColorFromChamberIndex(int index);
  static void GetCoordsForDrawing(DrawingVariables& TDV, Vector<Rational>& r, double& x, double& y);
  static void ProjectOnToHyperPlaneGraphics(Vector<Rational>& input, Vector<Rational>& output);
  static std::string GetColorHtmlFromColorIndex(int colorIndex);
  std::string GetColorPsTricksFromColorIndex(int colorIndex);
  static bool GetColorIntFromColorString(const std::string& input, int& output);
  DrawOperations theBuffer;
  inline int GetActualPenStyleFromFlagsAnd(int inputPenStyle);
  inline int GetActualTextStyleFromFlagsAnd(int inputTextStyle);
  std::string GetHtmlFromDrawOperationsCreateDivWithUniqueName(int theDimension);
  void drawString(DrawElementInputOutput& theDrawData, const std::string& input, int theFontSize, int theTextStyle);
  void drawCoordSystemDirectlly(DrawingVariables& TDV, int theDimension, std::fstream* LatexOutFile);
  void drawCoordSystemBuffer(DrawingVariables& TDV, int theDimension);
  void drawLineDirectly
  (double X1, double Y1, double X2, double Y2, uint32_t thePenStyle, int ColorIndex,
   double lineWidth);
  void drawLineBuffer
  (double X1, double Y1, double X2, double Y2, uint32_t thePenStyle, int ColorIndex,
   double lineWidth);
  void drawTextDirectly(double X1, double Y1, const std::string& inputText, int color, std::fstream* LatexOutFile);
  void drawTextBuffer(double X1, double Y1, const std::string& inputText, int color);
  void drawBufferNoIniT(DrawOperations& theOps);
  void drawBufferNoIniT()
  { this->drawBufferNoIniT(this->theBuffer);
  }
  //if the LatexOutFile is zero then the procedure defaults to the screen
  void drawLineBufferOld(double X1, double Y1, double X2, double Y2, uint32_t thePenStyle, int ColorIndex, std::fstream* LatexOutFile);
  void drawLineBetweenTwoVectorsBufferRational
  (const Vector<Rational>& r1, const Vector<Rational>& r2, int PenStyle, int PenColor, double lineWidth=1)
  { this->theBuffer.drawLineBetweenTwoVectorsBufferRational(r1, r2, PenStyle, PenColor, lineWidth);
  }
  void drawLineBetweenTwoVectorsBufferDouble
  (const Vector<double>& r1, const Vector<double>& r2, uint32_t PenStyle, int PenColor, double lineWidth=1)
  { this->theBuffer.drawLineBetweenTwoVectorsBufferDouble(r1, r2, PenStyle, PenColor, lineWidth);
  }
  void drawTextAtVectorBufferRational(const Vector<Rational>& point, const std::string& inputText, int textColor, int theTextStyle);
  void drawTextAtVectorBufferDouble(const Vector<double>& point, const std::string& inputText, int textColor, int theTextStyle);
  void drawCircleAtVectorBufferRational(const Vector<Rational>& point, double radius, uint32_t thePenStyle, int theColor);
  void drawCircleAtVectorBufferDouble(const Vector<double>& point, double radius, uint32_t thePenStyle, int theColor);
  void operator=(const DrawingVariables& other)
  { this->theDrawLineFunction = other.theDrawLineFunction;
    this->theDrawTextFunction = other.theDrawTextFunction;
  }
};
#endif
