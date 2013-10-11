//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader1General0_General.h"
#include "vpfHeader2Math0_General.h"
#include "vpfImplementationHeader2Math051_PolynomialComputations_Basic.h"
ProjectInformationInstance ProjectInfoVpf9cpp(__FILE__, "Main implementation file, part 1. ");

//the below gives upper limit to the amount of pointers that are allowed to be allocated by the program. Can be changed dynamically.
//used to guard the web server from abuse.
#ifdef CGIversionLimitRAMuse
int ParallelComputing::cgiLimitRAMuseNumPointersInList=2000000000;
#endif

ControllerStartsRunning ParallelComputing::controllerSignalPauseUseForNonGraciousExitOnly;
bool ParallelComputing::flagUngracefulExitInitiated=false;

int ParallelComputing::GlobalPointerCounter=0;
int ParallelComputing::PointerCounterPeakRamUse=0;

void (*CGI::functionCGIServerIgnoreUserAbort) (void)=0;

//CombinatorialChamberContainer GlobalCollectorChambers;
//FacetPointers GlobalCollectorFacets;

Vector<Rational> PartFraction::theVectorToBePartitioned;
bool PartFraction::MakingConsistencyCheck=false;
Rational PartFractions::CheckSum;
Rational PartFraction::CheckSum;
Rational PartFraction::CheckSum2;
int DrawingVariables::NumHtmlGraphics=0;

int CGI::GlobalFormulaIdentifier=0;

template < > bool Matrix<Rational>::flagComputingDebugInfo=true;
template < > bool Polynomial<Rational>::flagAnErrorHasOccuredTimeToPanic=true;

template < > bool  CompleX<double>::flagEqualityIsApproximate=true;
template < > double CompleX<double>::EqualityPrecision=0.00000001;

template <class ElementLeft, class ElementRight, class coefficient>
class TensorProductMonomial;

std::fstream PartFraction::TheBigDump;
std::fstream PartFractions::ComputedContributionsList;

//template < > int ListPointers<PartFraction>::MemoryAllocationIncrement=100;
//ListPointers<PartFraction> PartFraction::GlobalCollectorPartFraction;
bool PartFraction::UseGlobalCollector=true;
bool PartFraction::flagAnErrorHasOccurredTimeToPanic=false;
bool PartFractions::flagSplitTestModeNoNumerators=false;
bool PartFractions::flagAnErrorHasOccurredTimeToPanic=false;
bool PartFractions::flagUsingCheckSum=true;
int PartFractions::NumMonomialsInNumeratorsRelevantFractions=0;
int PartFractions::NumProcessedForVPFMonomialsTotal=0;
int PartFractions::flagMaxNumStringOutputLines=500;
//int PartFraction::lastApplicationOfSVformulaNumNewGenerators=0;
//int PartFraction::lastApplicationOfSVformulaNumNewMonomials=0;
bool Rational::flagAnErrorHasOccurredTimeToPanic=false;
//bool Rational::flagMinorRoutinesOnDontUseFullPrecision=false;
bool PartFractions::flagMakingProgressReport=true;
bool WeylGroup::flagAnErrorHasOcurredTimeToPanic=false;
//FacetPointers TheBigFacetOutput;
//DrawingVariables TDV(200, 400);
int rootSubalgebras::ProblemCounter=0;

unsigned long long Rational::TotalLargeAdditions=0;
unsigned long long Rational::TotalLargeGCDcalls=0;
unsigned long long Rational::TotalLargeMultiplications=0;
unsigned long long Rational::TotalSmallAdditions=0;
unsigned long long Rational::TotalSmallGCDcalls=0;
unsigned long long Rational::TotalSmallMultiplications=0;

GlobalVariables::GlobalVariables()
{ this->FeedDataToIndicatorWindowDefault=0;
  this->MaxComputationTimeSecondsNonPositiveMeansNoLimit=1000000;
  this->callSystem=0;
  this->flagGaussianEliminationProgressReport=false;
  this->getElapsedTimePrivate=0;
  std::cout << "Global variables created!";
}

void ProgressReport::Report(const std::string& theReport)
{ if (this->pointerGV==0)
    return;
  this->pointerGV->theIndicatorVariables.ProgressReportStringS[currentLevel]=theReport;
  this->pointerGV->MakeReport();
}

void ProgressReport::initFromGV(GlobalVariables* theGlobalVariables)
{ this->pointerGV=theGlobalVariables;
  if (this->pointerGV==0)
    return;
  currentLevel=this->pointerGV->theIndicatorVariables.ProgressReportStringS.size;
  this->pointerGV->theIndicatorVariables.ProgressReportStringS.SetSize(this->pointerGV->theIndicatorVariables.ProgressReportStringS.size+1);
  *this->pointerGV->theIndicatorVariables.ProgressReportStringS.LastObject()="";
}

ProgressReport::~ProgressReport()
{ if (this->pointerGV==0)
    return;
  pointerGV->theIndicatorVariables.ProgressReportStringS.size--;
}

ProjectInformationInstance::ProjectInformationInstance(const char* fileName, const std::string& fileDescription)
{ ProjectInformation::GetMainProjectInfo().AddProjectInfo(fileName, fileDescription);
}

RegisterFunctionCall::RegisterFunctionCall(const char* fileName, int line, const std::string& functionName)
{ List<stackInfo>& theStack=ProjectInformation::GetMainProjectInfo().CustomStackTrace;
  static MutexWrapper inCaseOfMultithreading;
  inCaseOfMultithreading.LockMe();
  theStack.SetSize(theStack.size+1);
  stackInfo& stackTop=*theStack.LastObject();
  stackTop.fileName=fileName;
  stackTop.line=line;
  stackTop.functionName=functionName;
  inCaseOfMultithreading.UnlockMe();
}

RegisterFunctionCall::~RegisterFunctionCall()
{ List<stackInfo>& theStack=ProjectInformation::GetMainProjectInfo().CustomStackTrace;
  theStack.size--;
}

std::string ProjectInformation::GetStackTraceReport()
{ std::stringstream out;
  for (int i=this->CustomStackTrace.size-1; i>=0; i--)
  { out << "<tr><td>" << CGI::GetHtmlLinkFromProjectFileName(this->CustomStackTrace[i].fileName) << "</td><td>" << this->CustomStackTrace[i].line << "</td>";
    if (this->CustomStackTrace[i].functionName!="")
      out << "<td>" << this->CustomStackTrace[i].functionName << "</td>";
    out << "</tr>";
  }
  return out.str();
}

std::string CGI::GetStackTraceEtcErrorMessage(const std::string& file, int line)
{ std::stringstream out;
  out << "<br>A partial stack trace follows (function calls not explicitly logged not included). <br>The first two stack trace lines may belong to the same function.";
  out << "<table><tr><td>file</td><td>line</td><td>function name (if known)</td></tr><tr><td> " << CGI::GetHtmlLinkFromProjectFileName(file) << "</td><td> " << line << "</td></tr>";
  out << ProjectInformation::GetMainProjectInfo().GetStackTraceReport();
  out << "</table>";
  return out.str();
}

int DrawingVariables::GetColorFromChamberIndex(int index, std::fstream* LaTexOutput)
{ static const int NumColorsBase=3;
  int tempI=index%(NumColorsBase*NumColorsBase*NumColorsBase);
  if (tempI<0)
    tempI+=(NumColorsBase*NumColorsBase*NumColorsBase);
  int r=(255* (tempI%NumColorsBase))/NumColorsBase;
  int g=(255* (tempI%(NumColorsBase*NumColorsBase)))/(NumColorsBase*NumColorsBase);
  int b=(255* (tempI%(NumColorsBase*NumColorsBase*NumColorsBase)))/(NumColorsBase*NumColorsBase*NumColorsBase);
  return CGI::RedGreenBlue(r, g, b);
}

void DrawingVariables::initDrawingVariables(int cX1, int cY1)
{ this->DefaultHtmlHeight=750;
  this->DefaultHtmlWidth=750;
  this->theDrawLineFunction=0;
  this->theDrawTextFunction=0;
  this->theDrawCircleFunction=0;
  this->theDrawClearScreenFunction=0;
  this->fontSizeNormal=10;
  this->fontSizeSubscript=6;
  this->flagLaTeXDraw= false;
  this->flagDisplayingCreationNumbersInsteadOfDisplayNumbers=false;
  this->flagDrawChamberIndices=true;
  this->flagDrawingInvisibles=false;
  this->flagDrawingLinkToOrigin=true;
  this->flagFillUserDefinedProjection=false;
  this->ColorDashes=CGI::RedGreenBlue(200, 200, 200);
  this->flag2DprojectionDraw=true;
  this->ColorChamberIndicator=CGI::RedGreenBlue(220, 220, 0);
  this->ColorWeylChamberWalls=CGI::RedGreenBlue(220, 220, 0);
  this->ColorTextPermanentlyZeroChamber = CGI::RedGreenBlue(250, 220, 220);
  this->ColorTextZeroChamber = CGI::RedGreenBlue(200, 100, 100);
  this->ColorTextDefault= CGI::RedGreenBlue(0, 0, 0);
  this->Selected=-2;
  this->textX=0;
  this->textY=15;
  this->theBuffer.init();
}

std::stringstream  CGI::outputStream;
int CGI::numLinesAll;
int CGI::shiftX=0;
int CGI::numDashedLines=0;
int CGI::numRegularLines=0;
int CGI::numDottedLines=0;
int CGI::shiftY=-200;
int CGI::scale=100;

void CGI::clearDollarSigns(std::string& theString, std::string& output)
{ std::stringstream out;
  for(unsigned int i=0; i<theString.size(); i++)
    if(theString[i]!='$')
      out << theString[i];
  output=out.str();
}

std::string CGI::clearSlashes(const std::string& theString)
{ std::stringstream out;
  for(unsigned int i=0; i<theString.size(); i++)
    if(theString[i]!='\\')
      out << theString[i];
  return out.str();
}

void CGI::subEqualitiesWithSimeq(std::string& theString, std::string& output)
{ std::stringstream out;
  for(unsigned int i=0; i<theString.size(); i++)
    if(theString[i]!='=')
      out << theString[i];
    else
      out << "\\simeq ";
  output=out.str();
}

void CGI::PrepareOutputLineJavaScriptSpecific(const std::string& lineTypeName, int numberLines)
{ std::cout << "\n\tvar num" << lineTypeName << "Lines=" << numberLines << "; ";
  std::cout << "\n\tvar " << lineTypeName << "1= new Array(" << numberLines << "); " << "  \tvar " << lineTypeName << "2= new Array("
  << numberLines << "); " << "  \tvar clr" << lineTypeName << "= new Array("  << numberLines << "); ";
}

void CGI::outputLineJavaScriptSpecific(const std::string& lineTypeName, int theDimension, std::string& stringColor, int& lineCounter)
{ std::string tempS;
  std::cout  << "\n\t" << lineTypeName << "1["  << lineCounter << "]= new Array(" << theDimension << "); " << "\t" << lineTypeName << "2["
  << lineCounter << "]= new Array(" << theDimension << "); " << "\tclr" << lineTypeName << "[" << lineCounter << "]= new Array(" << 3 << "); \n";
  for (int j=0; j< theDimension; j++)
  { CGI::outputStream >> tempS;
    std::cout << "\t" << lineTypeName << "1[" << lineCounter << "][" << j << "]=" << tempS << "; ";
    CGI::outputStream >> tempS;
    std::cout << "\t" << lineTypeName << "2[" << lineCounter << "][" << j << "]=" << tempS << "; ";
  }
  std::cout << "\tclr" << lineTypeName << "[" << lineCounter << "]=" << stringColor << "; ";
  lineCounter++;
}

void CGI::ElementToStringTooltip(const std::string& input, const std::string& inputTooltip, std::string& output, bool useHtml)
{ std::stringstream out;
  if (useHtml)
    out << "<span title=\"" << inputTooltip << "\">" << input << "</span>";
  output=out.str();
}

void CGI::FormatCPPSourceCode(const std::string& FileName)
{ std::fstream fileIn, fileOut;
  CGI::OpenFileCreateIfNotPresent(fileIn, FileName, false, false, false);
  assert(fileIn.is_open());
  fileIn.clear(std::ios::goodbit);
  fileIn.seekg(0, std::ios_base::end);
  int theSize= fileIn.tellg();
  fileIn.seekg(0);
  char* buffer= new char[theSize*2+1];
  fileIn.read(buffer, theSize*2);
  std::string nameFileOut= FileName;
  nameFileOut.append(".new");
  ::CGI::OpenFileCreateIfNotPresent(fileOut, nameFileOut, false, true, false);
  for (int i=0; i<theSize; i++)
  { char lookAhead= (i< theSize-1)? buffer[i+1] : ' ';
    switch(buffer[i])
    { case'\t':  fileOut << "  "; break;
      case ',':
        fileOut << buffer[i];
        if (lookAhead!=' ' && lookAhead!='\n' && lookAhead!='\'')
          fileOut << " ";
        break;
      case ';':
        fileOut << buffer[i];
        if (lookAhead!=' ' && lookAhead!='\n' && lookAhead!='\'')
          fileOut << " ";
        break;
      default:  fileOut << buffer[i]; break;
    }
  }
  delete [] buffer;
}

bool CGI::OpenFileCreateIfNotPresent(std::fstream& theFile, const std::string& theFileName, bool OpenInAppendMode, bool truncate, bool openAsBinary)
{ if (OpenInAppendMode)
  { if (openAsBinary)
      theFile.open(theFileName.c_str(), std::fstream::in|std::fstream::out|std::fstream::app| std::fstream::binary);
    else
      theFile.open(theFileName.c_str(), std::fstream::in|std::fstream::out|std::fstream::app);
  } else
  { if (openAsBinary)
      theFile.open(theFileName.c_str(), std::fstream::in|std::fstream::out| std::fstream::binary);
    else
    { if (truncate)
        theFile.open(theFileName.c_str(), std::fstream::in|std::fstream::out| std::fstream::trunc);
      else
        theFile.open(theFileName.c_str(), std::fstream::in|std::fstream::out);
    }
  }
  if(theFile.is_open())
  { theFile.clear();
    theFile.seekp(0, std::ios_base::end);
    int tempI=theFile.tellp();
    if (tempI>0)
    { if (!OpenInAppendMode)
      { theFile.seekp(0);
        theFile.seekg(0);
      }
      return true;
    }
  }
  theFile.close();
  theFile.open(theFileName.c_str(), std::fstream::out | std::fstream::in | std::fstream::trunc);
  theFile.clear();
  return theFile.is_open();
}

bool CGI::FileExists(const std::string& theFileName)
{ std::fstream theFile;
  theFile.open(theFileName.c_str(), std::fstream::in);
  if(theFile.is_open())
    return true;
  else
    return false;
}

void DrawingVariables::GetCoordsForDrawing(DrawingVariables& TDV, Vector<Rational>& r, double& x, double& y)
{ x=TDV.theBuffer.centerX[0];
  y=TDV.theBuffer.centerY[0];
  for (int k=0; k<r.size; k++)
  { x+=(r.TheObjects[k].DoubleValue())*(TDV.theBuffer.ProjectionsEiVectors[k][0]);
    y-=(r.TheObjects[k].DoubleValue())*(TDV.theBuffer.ProjectionsEiVectors[k][1]);
  }
}

void DrawingVariables::drawCoordSystemBuffer(DrawingVariables& TDV, int theDimension, std::fstream* LatexOutFile)
{ Vector<Rational> tempRoot;
  Vector<Rational> zeroRoot;
  zeroRoot.MakeZero(theDimension);
  for (int i=0; i<theDimension; i++)
  { tempRoot.MakeEi(theDimension, i);
    std::string tempS;
    tempS=tempRoot.ToString();
    TDV.drawLineBetweenTwoVectorsBuffer(zeroRoot, tempRoot, TDV.PenStyleNormal, CGI::RedGreenBlue(210, 210, 210));
    TDV.drawTextAtVectorBuffer(tempRoot, tempS, CGI::RedGreenBlue(100, 200, 100), TDV.TextStyleNormal, LatexOutFile);
    TDV.drawCircleAtVectorBuffer(tempRoot, 2, TDV.PenStyleNormal, CGI::RedGreenBlue(100, 200, 100) );
  }
  TDV.theBuffer.BasisToDrawCirclesAt.MakeEiBasis(theDimension, 1, 0);
}

void DrawingVariables::drawLineBufferOld(double X1, double Y1, double X2, double Y2, unsigned long thePenStyle, int ColorIndex, std::fstream* LatexOutFile)
{ this->theBuffer.drawLineBuffer(X1, Y1, X2, Y2, thePenStyle, ColorIndex);
  if (LatexOutFile!=0)
    LaTeXProcedures::drawline(X1, Y1, X2, Y2, thePenStyle, ColorIndex, *LatexOutFile, *this);
}

void DrawingVariables::drawTextAtVectorBuffer(const Vector<Rational>& point, const std::string& inputText, int textColor, int theTextStyle, std::fstream* LatexOutFile)
{ this->theBuffer.drawTextAtVectorBuffer(point, inputText, textColor, this->fontSizeNormal, theTextStyle);
}

void DrawingVariables::drawCircleAtVectorBuffer
(const Vector<Rational>& point, double radius, unsigned long thePenStyle, int theColor)
{ this->theBuffer.drawCircleAtVectorBuffer(point, radius, thePenStyle, theColor);
}

void DrawingVariables::drawTextDirectly(double X1, double Y1, const std::string& inputText, int color, std::fstream* LatexOutFile)
{ if (this->theDrawTextFunction!=0)
    this->theDrawTextFunction(X1-7, Y1-7, inputText.c_str(), inputText.length(), color, this->fontSizeNormal);
  if (LatexOutFile!=0)
    LaTeXProcedures::drawTextDirectly(X1, Y1, inputText, color, *LatexOutFile);
}

void DrawingVariables::ProjectOnToHyperPlaneGraphics(Vector<Rational>& input, Vector<Rational>& output, Vectors<Rational>& directions)
{ output=input;
  Vector<Rational> normal; Vector<Rational> basepoint;
  Rational tempRat2, tempRat, tempRat3;
  normal.MakeZero(input.size);
  for (int i=0; i<input.size; i++)
  { if (input.TheObjects[i].IsPositiveOrZero())
      normal.TheObjects[i]+=1;
    else
      normal.TheObjects[i]+=-1;
  }
  basepoint.MakeZero(input.size);
  basepoint.TheObjects[0].AssignInteger(1);
  if (input.TheObjects[0].IsNegative())
    basepoint.Minus();
//////////////////////////////////////////////////
  output.ScalarEuclidean(normal, tempRat2);
  basepoint.ScalarEuclidean(normal, tempRat);
  if (!tempRat2.IsEqualToZero())
  { tempRat3=tempRat;
    tempRat3.DivideBy(tempRat2);
    output*=tempRat3;
  }
  else
    output.MakeZero(input.size);
}

bool WeylGroup::IsStronglyPerpendicularTo(const Vector<Rational>& input, const Vector<Rational>& other)
{ return !this->RootSystem.Contains (input+other) && ! this->RootSystem.Contains(input-other);
}

bool WeylGroup::HasStronglyPerpendicularDecompositionWRT
(Vector<Rational>& input, int UpperBoundNumBetas, Vectors<Rational>& theSet, Vectors<Rational>& output,
 List<Rational>& outputCoeffs, bool IntegralCoefficientsOnly)
{ if (UpperBoundNumBetas>0 && output.size>UpperBoundNumBetas)
    return false;
  if (input.IsEqualToZero())
    return true;
  if (theSet.size==0)
    return false;
  if (output.size==0)
  { if (theSet.Contains(input))
    { output.SetSize(1);
      *output.LastObject()=input;
      outputCoeffs.SetSize(1);
      outputCoeffs.LastObject()->MakeOne();
      return true;
    }
    output.ReservE(theSet.size);
    outputCoeffs.ReservE(theSet.size);
  }
  Vectors<Rational> theNewSet;
  theNewSet.ReservE(theSet.size);
  Vector<Rational> tempRoot;
  Rational tempRat;
  for (int indexFirstNonZeroRoot=0; indexFirstNonZeroRoot<theSet.size; indexFirstNonZeroRoot++)
  { Vector<Rational>& currentRoot = theSet.TheObjects[indexFirstNonZeroRoot];
    tempRat= this->RootScalarCartanRoot(input, currentRoot)/this->RootScalarCartanRoot(currentRoot, currentRoot);
    if (tempRat.IsPositive())
      if (!IntegralCoefficientsOnly || tempRat.DenShort==1)
       { theNewSet.size=0;
         for (int i=indexFirstNonZeroRoot; i<theSet.size; i++)
           if (this->IsStronglyPerpendicularTo(currentRoot, theSet.TheObjects[i]))
             theNewSet.AddOnTop(theSet.TheObjects[i]);
         outputCoeffs.AddOnTop(tempRat);
         output.AddOnTop(currentRoot);
         tempRoot = input-currentRoot*tempRat;
         if (this->HasStronglyPerpendicularDecompositionWRT(tempRoot, UpperBoundNumBetas, theNewSet, output, outputCoeffs, IntegralCoefficientsOnly))
           return true;
         output.size--;
         outputCoeffs.size--;
       }
  }
  return false;
}

int MathRoutines::lcm(int a, int b)
{ if (a<0)
    a=-a;
  if (b<0)
    b=-b;
  return ((a*b)/Rational::gcdSigned(a, b));
}

int MathRoutines::TwoToTheNth(int n)
{ int result=1;
  for(int i=0; i<n; i++)
    result*=2;
  return result;
}

int MathRoutines::KToTheNth(int k, int n)
{ int result=1;
  for(int i=0; i<n; i++)
    result*=k;
  return result;
}

void MathRoutines::KToTheNth(int k, int n, LargeInt& output)
{ output=1;
  for(int i=0; i<n; i++)
    output*=k;
}

int MathRoutines::BinomialCoefficientMultivariate(int N, List<int>& theChoices)
{ int ChoiceIndex=0;
  int denominator=1;
  int result=0;
  for( int i=N; i>0; i--)
  { result*=i;
    result/=denominator;
    denominator++;
    if (denominator>theChoices.TheObjects[ChoiceIndex])
    { denominator=1;
      ChoiceIndex++;
    }
  }
  return result;
}

void Selection::init(int maxNumElements)
{ this->selected.SetSize(maxNumElements);
  this->elements.SetSize(maxNumElements);
  this->MaxSize=maxNumElements;
  for (int i=0; i<this->MaxSize; i++)
    this->selected[i]=false;
  this->CardinalitySelection=0;
}

void Selection::AddSelectionAppendNewIndex(int index)
{ if (index>=this->MaxSize || index<0)
    return;
  if (this->selected[index])
    return;
  this->selected[index]=true;
  this->elements[this->CardinalitySelection]=index;
  this->CardinalitySelection++;
}

Selection::Selection()
{ this->MaxSize=-1;
//  this->elementsInverseSelection=0;
  this->selected=0;
  this->elements=0;
  this->CardinalitySelection=0;
}

Selection::Selection(int m)
{  //this->elementsInverseSelection=0;
  this->selected=0;
  this->elements=0;
  this->CardinalitySelection=0;
  this->init(m);
}

void Selection::RemoveLastSelection()
{ if (this->CardinalitySelection==0)
    return;
  this->selected[this->elements[this->CardinalitySelection-1]]=false;
  this->CardinalitySelection--;
}

int Selection::SelectionToIndex()
{ int result=0;
  for (int i=0; i<MaxSize; i++)
  { result*=2;
    if (this->selected[i])
      result+=1;
  }
  return result;
}

void Selection::ShrinkMaxSize()
{ MaxSize--;
  if (selected[MaxSize])
    CardinalitySelection--;
}

void Selection::ExpandMaxSize()
{ this->elements[this->CardinalitySelection]=this->MaxSize;
  this->selected[this->MaxSize]=true;
  this->MaxSize++;
  this->CardinalitySelection++;
}

void Selection::WriteToFile(std::fstream& output)
{ output << "Sel_max_size: " << this->MaxSize << " cardinality: " << this->CardinalitySelection << " ";
  for (int i=0; i<this->CardinalitySelection; i++)
    output << this->elements[i] << " ";
}

void Selection::ReadFromFile(std::fstream& input)
{ std::string tempS; int tempI, card;
  input >> tempS >> tempI;
  this->init(tempI);
  input >> tempS >> card;
  assert(tempS=="cardinality:");
  for (int i=0; i<card; i++)
  { input >> tempI;
    this->AddSelectionAppendNewIndex(tempI);
  }
}

std::string Selection::ToString()const
{ Vector<Rational> tempRoot;
  tempRoot=*this;
  return tempRoot.ToString();
}

void Selection::incrementSelection()
{ for (int i=this->MaxSize-1; i>=0; i--)
  { this->selected[i]=!this->selected[i];
    if (this->selected[i])
    { this->ComputeIndicesFromSelection();
      return;
    }
  }
  this->ComputeIndicesFromSelection();
}

void Selection::initSelectionFixedCardinality
(int card)
{ this->initNoMemoryAllocation();
  for (int i=0; i<card; i++)
  { this->selected[i]=true;
    this->elements[i]=i;
  }
  this->CardinalitySelection=card;
}

void Selection::incrementSelectionFixedCardinality
(int card)//, int& IndexLastZeroWithOneBefore, int& NumOnesAfterLastZeroWithOneBefore)
{ //example of the order of generation of all combinations when card=2 and MaxSize=5. The second column indicates the
  //state of the array at the point in code marked with *** below
  //11000     (->10000) IndexLastZeroWithOneBefore: 2 NumOnesAfterLastZeroWithOneBefore: 0
  //10100     (->10000) IndexLastZeroWithOneBefore: 3 NumOnesAfterLastZeroWithOneBefore: 0
  //10010     (->10000) IndexLastZeroWithOneBefore: 4 NumOnesAfterLastZeroWithOneBefore: 0
  //10001     (->00000) IndexLastZeroWithOneBefore: 1 NumOnesAfterLastZeroWithOneBefore: 1
  //01100     (->01000) IndexLastZeroWithOneBefore: 3 NumOnesAfterLastZeroWithOneBefore: 0
  //01010     (->01000) IndexLastZeroWithOneBefore: 4 NumOnesAfterLastZeroWithOneBefore: 0
  //01001     (->00000) IndexLastZeroWithOneBefore: 2 NumOnesAfterLastZeroWithOneBefore: 1
  //00110     (->00100) IndexLastZeroWithOneBefore: 4 NumOnesAfterLastZeroWithOneBefore: 0
  //00101     (->00000) IndexLastZeroWithOneBefore: 3 NumOnesAfterLastZeroWithOneBefore: 1
  //00011
  if (card>this->MaxSize)
    return;
  if (this->CardinalitySelection!=card)
  { this->initSelectionFixedCardinality(card);
    return;
  }
  if(card==this->MaxSize || card==0)
    return;
  int IndexLastZeroWithOneBefore=-1;
  int NumOnesAfterLastZeroWithOneBefore=0;
  for(int i=this->MaxSize-1; i>=0; i--)
    if (this->selected[i])
    { if (IndexLastZeroWithOneBefore==-1)
        NumOnesAfterLastZeroWithOneBefore++;
      else
        break;
    } else
      IndexLastZeroWithOneBefore=i;
  if (IndexLastZeroWithOneBefore==0)
  { this->initSelectionFixedCardinality(card);
    return;
  }
  for(int i=0; i<NumOnesAfterLastZeroWithOneBefore+1; i++)
    this->selected[this->elements[this->CardinalitySelection-i-1]]=false;
  //***At this point in time the second column is recorded
  for(int i=0; i<NumOnesAfterLastZeroWithOneBefore+1; i++)
  { this->selected[i+ IndexLastZeroWithOneBefore]=true;
    this->elements[this->CardinalitySelection+i-NumOnesAfterLastZeroWithOneBefore-1] = i+ IndexLastZeroWithOneBefore;
  }
}

void Selection::ComputeIndicesFromSelection()
{ this->CardinalitySelection=0;
  for (int i=0; i<this->MaxSize; i++)
  { if (this->selected[i])
    { this->elements[CardinalitySelection]=i;
      this->CardinalitySelection++;
    }
//    else
//    {  elementsInverseSelection[i-CardinalitySelection]=i;
//    }
  }
}

void Selection::initNoMemoryAllocation()
{ for (int i = 0; i<this->CardinalitySelection; i++)
    this->selected[this->elements[i]]=false;
  this->CardinalitySelection = 0;
}

void Selection::MakeSubSelection(Selection &theSelection, Selection &theSubSelection)
{ this->init(theSelection.MaxSize);
  for(int i=0; i<theSubSelection.CardinalitySelection; i++)
    this->AddSelectionAppendNewIndex(theSelection.elements[theSubSelection.elements[i]]);
}

void Selection::operator=(const Selection& right)
{ if (this==&right)
    return;
  if (this->MaxSize!=right.MaxSize)
    this->init(right.MaxSize);
  else
    this->initNoMemoryAllocation();
  for (int i=0; i<right.CardinalitySelection; i++)
  { this->elements[i]=right.elements[i];
    this->selected[this->elements[i]]=true;
  }
  this->CardinalitySelection=right.CardinalitySelection;
}

unsigned int Selection::HashFunction() const
{ int tempMin=MathRoutines::Minimum(SomeRandomPrimesSize, this->MaxSize);
  unsigned int result=0;
  for (int i=0; i<tempMin; i++)
    if (this->selected[i])
      result+=i*SomeRandomPrimes[i];
  return result;
}

void Rational::operator=(const Polynomial<Rational>& other)
{ if (!other.IsAConstant(this))
    crash << "This is a programming error: attempting to assign non-constant polynomial to a Rational number is not allowed. "
    << crash;
}

bool Rational::IsEqualTo(const Rational& b) const
{ if (this->Extended==0 && b.Extended==0)
    return (this->NumShort*b.DenShort==b.NumShort*this->DenShort);
  Rational tempRat;
  tempRat.Assign(*this);
  tempRat.Subtract(b);
  return tempRat.IsEqualToZero();
}

bool Rational::IsGreaterThanOrEqualTo(const Rational& right)const
{ if (this->Extended==0 && right.Extended==0)
    return (this->NumShort*right.DenShort>=right.NumShort*this->DenShort);
  Rational tempRat;
  tempRat.Assign(*this);
  tempRat.Subtract(right);
  return tempRat.IsPositiveOrZero();
}

std::string Rational::ToString(FormatExpressions* notUsed)const
{ std::stringstream out;
  if (this->Extended==0)
  { out << this->NumShort;
    if (this->DenShort!=1)
      out << "/" << this->DenShort;
  } else
  { std::string tempS;
    this->Extended->num.ToString(tempS);
    out << tempS;
    this->Extended->den.ToString(tempS);
    if (tempS!="1")
      out << "/" << tempS;
  }
  return out.str();
}

//Format expression monomial orders (for the ToString() function follow.
template<>
typename List<MonomialP>::OrderLeftGreaterThanRight FormatExpressions::GetMonOrder<MonomialP>()
{ return this->thePolyMonOrder;
}

template<>
typename List<MonomialVector>::OrderLeftGreaterThanRight
FormatExpressions::GetMonOrder<MonomialVector>()
{ return 0;
}

template<>
typename List<MonomialChar<RationalFunctionOld> >::OrderLeftGreaterThanRight
FormatExpressions::GetMonOrder<MonomialChar<RationalFunctionOld> >()
{ return 0;
}

template<>
typename List<MonomialWeylAlgebra>::OrderLeftGreaterThanRight
FormatExpressions::GetMonOrder<MonomialWeylAlgebra>()
{ return 0;
}

template<>
typename List<MonomialUniversalEnveloping<RationalFunctionOld> >::OrderLeftGreaterThanRight
FormatExpressions::GetMonOrder<MonomialUniversalEnveloping<RationalFunctionOld> >()
{ return 0;
}

template<>
typename List<MonomialGeneralizedVerma<RationalFunctionOld> >::OrderLeftGreaterThanRight
FormatExpressions::GetMonOrder<MonomialGeneralizedVerma<RationalFunctionOld> >()
{ return 0;
}

template<>
typename List<ChevalleyGenerator >::OrderLeftGreaterThanRight
FormatExpressions::GetMonOrder<ChevalleyGenerator>()
{ return 0;
}

template<>
typename List<MonomialMatrix>::OrderLeftGreaterThanRight
FormatExpressions::GetMonOrder<MonomialMatrix>()
{ return 0;
}

template<>
typename List<MonomialChar<Rational> >::OrderLeftGreaterThanRight
FormatExpressions::GetMonOrder<MonomialChar<Rational> >()
{ return 0;
}

template<>
typename List<MonomialUniversalEnveloping<Rational> >::OrderLeftGreaterThanRight
FormatExpressions::GetMonOrder<MonomialUniversalEnveloping<Rational> >()
{ return 0;
}

template<>
typename List<MonomialTensorGeneralizedVermas<RationalFunctionOld> >::OrderLeftGreaterThanRight
FormatExpressions::GetMonOrder<MonomialTensorGeneralizedVermas<RationalFunctionOld> >()
{ return 0;
}

template<>
typename List<quasiDiffMon>::OrderLeftGreaterThanRight
FormatExpressions::GetMonOrder<quasiDiffMon>()
{ return 0;
}

template<>
typename List<MonomialUniversalEnveloping<Polynomial<Rational> > >::OrderLeftGreaterThanRight
FormatExpressions::GetMonOrder<MonomialUniversalEnveloping<Polynomial<Rational> > >()
{ return 0;
}

FormatExpressions::FormatExpressions()
{ this->AmbientWeylLetter='X';
  this->ExtraLinesCounterLatex=0;
  this->chevalleyGgeneratorLetter="g";
  this->chevalleyHgeneratorLetter="h";
  this->polyDefaultLetter="x";
  this->WeylAlgebraDefaultLetter="\\partial";
  this->FDrepLetter="V";
  this->simpleRootLetter="\\eta";
  this->MaxLineLength=100;
  this->flagPassCustomCoeffMonSeparatorToCoeffs=false;
  this->flagUseCalculatorFormatForUEOrdered=true;
  this->flagUseHTML=true;
  this->flagUseLatex=false;
  this->flagUsePNG=false;
  this->MatrixColumnVerticalLineIndex=-1;
  this->flagQuasiDiffOpCombineWeylPart=true;
  this->flagMakingExpressionTableWithLatex=false;
  this->MaxLinesPerPage=40;
  this->NumAmpersandsPerNewLineForLaTeX=0;
  this->MaxRecursionDepthPerExpression=500;
  this->thePolyMonOrder=0;
  this->flagExpressionIsFinal=false;
  this->flagExpressionNewLineAllowed=true;
  this->flagIncludeLieAlgebraNonTechnicalNames=false;
  this->flagIncludeLieAlgebraTypes=true;
  this->flagUseReflectionNotation=false;
  this->flagUseHtmlAndStoreToHD=false;
}

std::string FormatExpressions::GetPolyLetter(int index)const
{ if (index<this->polyAlphabeT.size)
    return this->polyAlphabeT[index];
  std::stringstream out;
  out << this->polyDefaultLetter << "_{" << index+1 << "}";
  std::string tempS=out.str();
  return tempS;
}

void Rational::WriteToFile(std::fstream& output)
{ output << this->ToString();
}

inline void Rational::RaiseToPower(int x)
{ Rational tempRat;
  tempRat.MakeOne();
  if (x<0)
  { x=-x;
    this->Invert();
  }
  LargeInt tempNum=this->GetNumerator();
  LargeIntUnsigned oneLI;
  oneLI.MakeOne();
  MathRoutines::RaiseToPower(tempNum.value, x, oneLI);
  LargeIntUnsigned tempDen= this->GetDenominator();
  MathRoutines::RaiseToPower(tempDen, x, oneLI);
  int theSign= (this->IsPositive() || x%2==0) ? 1 :-1;
  this->AllocateExtended();
  this->Extended->num.sign=(signed char)theSign;
  this->Extended->den=tempDen;
  this->Extended->num.value=tempNum.value;
  this->ShrinkExtendedPartIfPossible();
}

inline void Rational::Invert()
{ if (this->Extended==0)
  { int tempI= this->DenShort;
    assert(tempI>0);
    if (this->NumShort<0)
    { this->DenShort=-this->NumShort;
      this->NumShort=-tempI;
    }
    else
    { this->DenShort= this->NumShort;
      this->NumShort= tempI;
    }
    return;
  }
  MathRoutines::swap(this->Extended->den, this->Extended->num.value);
}

void Rational::ReadFromFile(std::istream& input)
{ std::string tempS;
  input >> tempS;
  this->AssignString(tempS);
}

inline void Rational::MultiplyByInt(int x)
{ Rational tempRat;
  tempRat.AssignInteger(x);
  this->MultiplyBy(tempRat);
}

inline void Rational::MultiplyBy(const Rational& r)
{ if (r.Extended==0 && this->Extended==0)
    if (this->TryToMultiplyQuickly(r.NumShort, r.DenShort))
      return;
  this->InitExtendedFromShortIfNeeded();
  if (r.Extended!=0)
  { this->Extended->num*=(r.Extended->num);
    this->Extended->den*=(r.Extended->den);
  }
  else
  { this->Extended->num.MultiplyByInt(r.NumShort);
    this->Extended->den.MultiplyByUInt((unsigned int)r.DenShort);
  }
  this->Simplify();
}

Rational operator/(int left, const Rational& right)
{ Rational tempRat=left;
  tempRat/=right;
  return tempRat;
}

Rational operator-(const Rational& argument)
{ Rational tempRat;
  tempRat.Assign(argument);
  tempRat.Minus();
  return tempRat;
}

inline void Rational::MultiplyByLargeInt(LargeInt& x)
{ this->InitExtendedFromShortIfNeeded();
  this->Extended->num*=(x);
  this->Simplify();
}

void Rational::MultiplyByLargeIntUnsigned(LargeIntUnsigned& x)
{ this->InitExtendedFromShortIfNeeded();
  this->Extended->num.value.MultiplyBy(x);
  this->Simplify();
}

inline void Rational::DivideBy(const Rational& r)
{ if (this==&r)
  { this->MakeOne();
    return;
  }
  Rational tempRat=r;
  tempRat.Invert();
  this->operator*=(tempRat);
}

Rational Rational::operator/(const Rational& right) const
{ Rational tempRat;
  tempRat.Assign(*this);
  tempRat.DivideBy(right);
  return tempRat;
}

Rational Rational::operator*(const Rational& right) const
{ Rational tempRat;
  tempRat.Assign(*this);
  tempRat.MultiplyBy(right);
  return tempRat;
}

Rational Rational::operator+(const Rational& right) const
{ Rational tempRat;
  tempRat.Assign(*this);
  tempRat+=(right);
  return tempRat;
}

Rational Rational::operator-(const Rational& right) const
{ Rational tempRat;
  tempRat.Assign(*this);
  tempRat-=(right);
  return tempRat;
}

void Rational::Assign(const Rational& r)
{ this->NumShort = r.NumShort;
  this->DenShort = r.DenShort;
  if (r.Extended==0)
  { if (this->Extended==0)
      return;
    this->FreeExtended();
    return;
  }
  this->InitExtendedFromShortIfNeeded();
  this->Extended->num=(r.Extended->num);
  this->Extended->den=(r.Extended->den);
}

void Rational::AssignFracValue()
{ if (this->Extended==0)
  { if (this->NumShort==0)
      return;
    if (this->DenShort==1)
    { this->NumShort=0;
      this->DenShort=1;
      return;
    }
    this->NumShort=this->NumShort% this->DenShort;
    if (this->NumShort<0)
      this->NumShort+=this->DenShort;
    return;
  }
  if (this->IsEqualToZero())
    return;
  if (this->Extended->den.IsEqualToOne())
  { this->MakeZero();
    return;
  }
  LargeIntUnsigned newNum, tempI;
  this->Extended->num.value.DivPositive(this->Extended->den, tempI, newNum);
  this->Extended->num.value=newNum;
  if (this->Extended->num.IsNegative())
    this->Extended->num.AddLargeIntUnsigned(this->Extended->den);
  assert(this->Extended->num.IsPositiveOrZero());
  this->Simplify();
}

void Rational::AddInteger(int x)
{ Rational tempRat;
  tempRat.AssignNumeratorAndDenominator(x, 1);
  this->operator+=(tempRat);
}

bool Rational::IsGreaterThan(const Rational& r) const
{ Rational tempRat;
  tempRat.Assign(*this);
  tempRat.Subtract(r);
  return tempRat.IsPositive();
}

void Rational::Subtract(const Rational& r)
{ Rational temp;
  temp.Assign(r);
  temp.Minus();
  this->operator+=(temp);
}

bool Rational::GetSquareRootIfRational(Rational& output)const
{ if (*this<0)
    return false;
  LargeInt theNum=this->GetNumerator();
  LargeIntUnsigned theDen= this->GetDenominator();
  List<unsigned int> primeFactorsNum, primeFactorsDen;
  List<int> multsNum, multsDen;
  if (!theNum.value.Factor(primeFactorsNum, multsNum))
    return false;
  if (!theDen.Factor(primeFactorsDen, multsDen))
    return false;
  output=1;
  Rational tempRat;
  for (int i=0; i<primeFactorsNum.size; i++)
  { if (multsNum[i]%2!=0)
      return false;
    tempRat=primeFactorsNum[i];
    tempRat.RaiseToPower(multsNum[i]/2);
    output*=tempRat;
  }
  for (int i=0; i<primeFactorsDen.size; i++)
  { if (multsDen[i]%2!=0)
      return false;
    tempRat=primeFactorsDen[i];
    tempRat.RaiseToPower(multsDen[i]/2);
    output/=tempRat;
  }
  return true;
}

void Rational::AssignInteger(int x)
{ this->FreeExtended();
  this->DenShort=1;
  this->NumShort=x;
}

bool Rational::ShrinkExtendedPartIfPossible()
{ if (this->Extended==0)
    return true;
  if (this->Extended->num.value.theDigits.size>1 || this->Extended->den.theDigits.size>1 || this->Extended->num.value.theDigits[0]>=(unsigned int) LargeIntUnsigned::SquareRootOfCarryOverBound ||
      this->Extended->den.theDigits[0]>= (unsigned int) LargeIntUnsigned::SquareRootOfCarryOverBound)
    return false;
  this->NumShort= this->Extended->num.GetIntValueTruncated();
  this->DenShort= this->Extended->den.GetUnsignedIntValueTruncated();
  this->FreeExtended();
  return true;
}

Rational Rational::Factorial(int n, GlobalVariables* theGlobalVariables)
{ if (n<0)
  { assert(false);
    return 0;
  }
  LargeIntUnsigned result;
  result.AssignFactorial((unsigned int) n, theGlobalVariables);
  Rational answer;
  answer=result;
  return answer;
}

Rational Rational::TwoToTheNth(int n)
{ Rational result=1;
  if (n>=0)
    for (int i=0; i<n; i++)
      result.MultiplyByInt(2);
  else
    for (int i=0; i>n; i--)
      result.DivideByInteger(2);
  return result;
}

Rational Rational::NChooseK(const Rational &n, int k)
{ Rational result;
  result.MakeOne();
  for (int i=0; i<k; i++)
  { //std::cout << "<br>" << result.ToString() << " * " << (n-i).ToString() << "/" << i+1;
    result*=n-i;
    result.DivideByInteger(i+1);
    //std::cout << "=" << result.ToString();
  }
  return result;
}

Rational Rational::NtoTheKth(int n, int k)
{ Rational result=n;
  result.RaiseToPower(k);
  return result;
}

bool Rational::IsInteger(LargeInt* whichInteger)const
{ bool result=false;
  if (this->Extended==0)
  { result=(this->DenShort==1);
    if (whichInteger!=0)
      *whichInteger=this->NumShort;
  }
  else
    if (this->Extended->den.IsEqualToOne())
    { result=true;
      if (whichInteger!=0)
        *whichInteger=this->Extended->num;
    }
  return result;
}

double Rational::DoubleValue()const
{ if (this->Extended==0)
    return (double)this->NumShort/(double)this->DenShort;
  else
    return this->Extended->num.GetDoubleValue()/this->Extended->den.GetDoubleValue();
}

void Rational::Simplify()
{ if (this->Extended==0)
  { if (this->NumShort==0)
      this->DenShort=1;
    else
    { if (this->DenShort==1)
        return;
      int tempGCD;
      if (this->NumShort>0)
        tempGCD= this->gcd(this->NumShort, this->DenShort);
      else
        tempGCD= this->gcd(-this->NumShort, this->DenShort);
      this->NumShort/=tempGCD;
      this->DenShort/=tempGCD;
    }
    return;
  }
  if (this->Extended->num.IsEqualToZero())
  { this->MakeZero();
    return;
  }
  if (!this->Extended->den.IsEqualToOne())
  { LargeIntUnsigned tempI;
    LargeIntUnsigned::gcd(this->Extended->den, this->Extended->num.value, tempI);
    /*if (Rational::flagAnErrorHasOccurredTimeToPanic)
    { std::string tempS1, tempS2, tempS3;
      tempI.ToString(tempS1);
      this->ToString(tempS2);
    }*/
    LargeIntUnsigned tempI2;
    this->Extended->den.DivPositive(tempI, this->Extended->den, tempI2);
    this->Extended->num.value.DivPositive(tempI, this->Extended->num.value, tempI2);
  }
  this->ShrinkExtendedPartIfPossible();
}

inline void LargeIntUnsigned::AddShiftedUIntSmallerThanCarryOverBound(unsigned int x, int shift)
{ assert(x<LargeIntUnsigned::CarryOverBound);
  while (x>0)
  { if (shift>=this->theDigits.size)
    { int oldsize=this->theDigits.size;
      this->theDigits.SetSize(shift+1);
      for (int i=oldsize; i<this->theDigits.size; i++)
        this->theDigits[i]=0;
    }
    this->theDigits[shift]+=x;
    if (this->theDigits[shift]>=LargeIntUnsigned::CarryOverBound)
    { this->theDigits[shift]-=LargeIntUnsigned::CarryOverBound;
      x=1;
      shift++;
    } else
      x=0;
  }
//  this->FitSize();
}

inline void LargeIntUnsigned::AssignShiftedUInt(unsigned int x, int shift)
{ if (x==0)
  { this->MakeZero();
    return;
  }
  this->theDigits.SetSize(shift+1);
  for (int i=0; i<shift; i++)
    this->theDigits[i]=0;
  unsigned int tempX= x%LargeIntUnsigned::CarryOverBound;
  this->theDigits[shift]=tempX;
  x= x/LargeIntUnsigned::CarryOverBound;
  while (x!=0)
  { tempX= x%LargeIntUnsigned::CarryOverBound;
    this->theDigits.AddOnTop(tempX);
    x= x/LargeIntUnsigned::CarryOverBound;
  }
}

inline void LargeIntUnsigned::AddNoFitSize(const LargeIntUnsigned& x)
{ MacroIncrementCounter(Rational::TotalLargeAdditions);
  int oldsize= this->theDigits.size;
  this->theDigits.SetSize(MathRoutines::Maximum(this->theDigits.size, x.theDigits.size)+1);
  for (int i=oldsize; i<this->theDigits.size; i++)
    this->theDigits[i]=0;
  unsigned int CarryOver=0;
  for(int i=0; i<x.theDigits.size; i++)
  { this->theDigits[i]+=x.theDigits[i]+CarryOver;
    if (this->theDigits[i]>=LargeIntUnsigned::CarryOverBound)
    { this->theDigits[i]-=LargeIntUnsigned::CarryOverBound;
      CarryOver=1;
    }
    else
      CarryOver=0;
  }
  if (CarryOver!=0)
    for(int i=x.theDigits.size; i<this->theDigits.size; i++)
    { this->theDigits[i]+=1;
      if (this->theDigits[i]>=LargeIntUnsigned::CarryOverBound)
        this->theDigits[i]-=LargeIntUnsigned::CarryOverBound;
      else
        break;
    }
}

void LargeIntUnsigned::operator+=(const LargeIntUnsigned& x)
{ this->AddNoFitSize(x);
  this->FitSize();
}

LargeIntUnsigned LargeIntUnsigned::operator-(const LargeIntUnsigned& other) const
{ LargeIntUnsigned result;
  result=*this;
  result.SubtractSmallerPositive(other);
  return result;
}

void LargeIntUnsigned::SubtractSmallerPositive(const LargeIntUnsigned& x)
{ unsigned int CarryOver=0;
  if (!this->IsGEQ(x))
    crash << "This is a programming error: attempting to subtract a larger LargeIntUnsigned from a smaller one. " << crash;
  for (int i=0; i<x.theDigits.size; i++)
    if (this->theDigits[i]<x.theDigits[i]+CarryOver)
    { this->theDigits[i]+=LargeIntUnsigned::CarryOverBound;
      this->theDigits[i]-=(x.theDigits[i]+CarryOver);
      CarryOver=1;
    }
    else
    { this->theDigits[i]-=(x.theDigits[i]+CarryOver);
      CarryOver=0;
    }
  if (CarryOver!=0)
  { for (int i=x.theDigits.size; i<this->theDigits.size; i++)
      if (this->theDigits[i]>0)
      { this->theDigits[i]--;
        break;
      }
      else
        this->theDigits[i]=LargeIntUnsigned::CarryOverBound-1;
  }
  this->FitSize();
//  assert(this->CheckForConsistensy());
}

void LargeIntUnsigned::MultiplyBy(const LargeIntUnsigned& x, LargeIntUnsigned& output)const
{ if (this==&output || &x==&output)
  { LargeIntUnsigned thisCopy=*this;
    LargeIntUnsigned xCopy=x;
    return thisCopy.MultiplyBy(xCopy, output);
  }
  MacroIncrementCounter(Rational::TotalLargeMultiplications);
  output.theDigits.SetSize(x.theDigits.size+this->theDigits.size);
  for(int i=0; i<output.theDigits.size; i++)
    output.theDigits[i]=0;
  for (int i=0; i<this->theDigits.size; i++)
    for(int j=0; j<x.theDigits.size; j++)
    { unsigned long long tempLong= this->theDigits[i];
      unsigned long long tempLong2= x.theDigits[j];
      tempLong= tempLong*tempLong2;
      unsigned long long lowPart= tempLong%LargeIntUnsigned::CarryOverBound;
      unsigned long long highPart= tempLong/LargeIntUnsigned::CarryOverBound;
      output.AddShiftedUIntSmallerThanCarryOverBound((unsigned int) lowPart, i+j);
      output.AddShiftedUIntSmallerThanCarryOverBound((unsigned int) highPart, i+j+1);
    }
  output.FitSize();
//  assert(this->CheckForConsistensy());
}

void LargeIntUnsigned::FitSize()
{ int newSize=this->theDigits.size;
  for (int i=this->theDigits.size-1; i>=1; i--)
    if (this->theDigits[i]==0)
      newSize--;
    else
      break;
  this->theDigits.SetSize(newSize);
//  assert(this->CheckForConsistensy());
}

void LargeIntUnsigned::MultiplyByUInt(unsigned int x)
{ LargeIntUnsigned tempLI;
  tempLI.AssignShiftedUInt(x, 0);
  this->MultiplyBy(tempLI);
}

void LargeIntUnsigned::MultiplyBy(const LargeIntUnsigned& x)
{ LargeIntUnsigned tempInt;
  this->MultiplyBy(x, tempInt);
  *this=tempInt;
//  assert(this->CheckForConsistensy());
}

void LargeInt::MultiplyByInt(int x)
{//  if (this->value.size==0) return;
  LargeInt tempI;
  tempI.AssignInt(x);
  *this*=(tempI);
}

void LargeIntUnsigned::ToString(std::string& output)const
{ if (this->IsEqualToZero())
  { output="0";
    return;
  }
  if (this->theDigits.size>1)
  { this->ElementToStringLargeElementDecimal(output);
    return;
  }
  std::stringstream out;
  out << this->theDigits[0];
  output=out.str();
}

bool LargeInt::operator==(const LargeInt& x)const
{ if (x.sign!=this->sign)
  { if (x.IsEqualToZero() && this->IsEqualToZero())
      return true;
    else
      return false;
  }
  return this->value==x.value;
}

bool LargeInt::CheckForConsistensy()
{ if (this->sign!=-1 && this->sign!=1)
    return false;
  for (int i=0; i<this->value.theDigits.size; i++)
    if (this->value.theDigits[i]>=LargeIntUnsigned::CarryOverBound)
      return false;
  return true;
}

double LargeInt::GetDoubleValue()
{ return this->sign* this->value.GetDoubleValue();
}

void LargeInt::ToString(std::string& output)const
{ std::stringstream out;
  if (this->IsEqualToZero())
  { output.assign("0");
    return;
  }
  if (this->sign==-1)
    out << "-";
  std::string tempS;
  this->value.ToString(tempS);
  out << tempS;
  output=out.str();
}

void LargeInt::AssignInt(int x)
{ if (x==0)
  { this->MakeZero();
    return;
  }
  this->sign=1;
  if (x<0)
  { this->sign=-1;
    x=-x;
  }
  this->value.AssignShiftedUInt( (unsigned int)x, 0);
//  assert(this->CheckForConsistensy());
}

bool LargeInt::GetDivisors(List<int>& output, bool includeNegative)
{ if (this->value.theDigits.size>1)
    return false;
  int val= this->value.theDigits[0];
  if (val>50000)
    return false;
  output.SetSize(0);
  for (int i=1; i<= val; i++)
    if (val% i==0)
    { output.AddOnTop(i);
      if (includeNegative)
        output.AddOnTop(-i);
    }
  return true;
}

void LargeInt::AddLargeIntUnsigned(const LargeIntUnsigned& x)
{ if (this->sign==1)
  { this->value+=x;
    return;
  }
  if (this->value.IsGEQ(x))
    this->value.SubtractSmallerPositive(x);
  else
  { LargeIntUnsigned tempI=x;
    tempI.SubtractSmallerPositive(this->value);
    this->value=tempI;
    this->sign=1;
  }
}

void LargeInt::operator+=(const LargeInt& x)
{ if (this->sign==x.sign)
    this->value+=x.value;
  else
  { if (this->value.IsGEQ(x.value))
      this->value.SubtractSmallerPositive(x.value);
    else
    { LargeIntUnsigned tempI=this->value;
      this->value=x.value;
      this->value.SubtractSmallerPositive(tempI);
      this->sign= x.sign;
    }
  }
//  assert(this->CheckForConsistensy());
}

int LargeIntUnsigned::GetUnsignedIntValueTruncated()
{ return (int) this->theDigits[0];
}

double LargeIntUnsigned::GetDoubleValue()
{ double result=0;
  for (int i=this->theDigits.size-1; i>=0; i--)
    result=result*LargeIntUnsigned::CarryOverBound+this->theDigits[i];
  return result;
}

void LargeIntUnsigned::gcd(const LargeIntUnsigned& a, const LargeIntUnsigned& b, LargeIntUnsigned& output)
{ MacroIncrementCounter(Rational::TotalLargeGCDcalls);
  LargeIntUnsigned p, q, r, temp;
  p=a;
  q=b;
  while(!q.IsEqualToZero())
  { p.DivPositive(q, temp, r);
    p=q;
    q=r;
  }
  output=p;
//  assert(output.CheckForConsistensy());
}

void LargeInt::MakeZero()
{ this->value.MakeZero();
  this->sign=1;
}

void LargeInt::operator=(const Rational& other)
{ if (!other.IsInteger(this))
    crash << "This is a programming error: converting implicitly rational number " << other.ToString()
    << " to integer is not possible as the Rational number is not integral. " << crash;
//  assert(this->CheckForConsistensy());
}

void LargeInt::operator=(const LargeInt& x)
{ this->sign=x.sign;
  this->value=(x.value);
//  assert(this->CheckForConsistensy());
}

LargeInt LargeInt::operator/(int x)const
{ LargeInt result;
  LargeIntUnsigned remainder;
  LargeIntUnsigned tempX;
  int absX=x; signed char signX=1;
  if (x<0)
  { signX=-1;
    absX=-absX;
  }
  tempX.AssignShiftedUInt(absX, 0);
  this->value.DivPositive(tempX, result.value, remainder);
//  assert(result.CheckForConsistensy());
  result.sign=this->sign* signX;
  return result;
}

LargeInt LargeInt::operator/(LargeInt& x)const
{ LargeInt result;
  LargeInt remainder;
  this->value.DivPositive(x.value, result.value, remainder.value);
  result.sign= this->sign* x.sign;
  assert(result.CheckForConsistensy());
  return result;
}

int LargeInt::operator%(int x)
{ assert(x>0);
  LargeIntUnsigned result;
  LargeIntUnsigned remainder;
  LargeIntUnsigned tempX;
  if (x<0)
    x=-x;
  tempX.AssignShiftedUInt(x, 0);
  this->value.DivPositive(tempX, result, remainder);
  if (remainder.theDigits.size ==0)
    return 0;
  else
    return remainder.theDigits[0];
}

LargeIntUnsigned LargeIntUnsigned::operator/(unsigned int x)const
{ LargeIntUnsigned result;
  LargeIntUnsigned remainder;
  LargeIntUnsigned tempX;
  tempX.AssignShiftedUInt(x, 0);
  this->DivPositive(tempX, result, remainder);
//  assert(result.CheckForConsistensy());
  return result;
}

LargeIntUnsigned LargeIntUnsigned::operator/(const LargeIntUnsigned& x)const
{ LargeIntUnsigned result;
  LargeIntUnsigned remainder;
  this->DivPositive(x, result, remainder);
  return result;
}

int LargeIntUnsigned::operator%(unsigned int x)
{ LargeIntUnsigned result;
  LargeIntUnsigned remainder;
  LargeIntUnsigned tempX;
  tempX.AssignShiftedUInt(x, 0);
  this->DivPositive(tempX, result, remainder);
  return remainder.theDigits[0];
}

void LargeIntUnsigned::MakeOne()
{ this->theDigits.SetSize(1);
  this->theDigits[0]=1;
}

void LargeIntUnsigned::MakeZero()
{ this->theDigits.SetSize(1);
  this->theDigits[0]=0;
}

bool LargeIntUnsigned::IsGEQ(const LargeIntUnsigned& x)const
{ if (this->theDigits.size>x.theDigits.size)
    return true;
  if (this->theDigits.size<x.theDigits.size)
    return false;
  for (int i=this->theDigits.size-1; i>=0; i--)
  { if (x.theDigits[i]>this->theDigits[i])
      return false;
    if (x.theDigits[i]<this->theDigits[i])
      return true;
  }
  return true;
}

bool PartFraction::reduceOnceTotalOrderMethod
(MonomialCollection<PartFraction, Polynomial<LargeInt> >& output, GlobalVariables& theGlobalVariables, Vector<Rational>* Indicator, PartFractions& owner)
{ for (int i=0; i<this->IndicesNonZeroMults.size; i++)
    for (int j=0; j<this->IndicesNonZeroMults.size; j++)
    { //assert (this->IndicesNonZeroMults[i]<this->IndicesNonZeroMults[j]);
      int AminusBindex = owner.TableAllowedAminusB.elements
      [this->IndicesNonZeroMults.TheObjects[i]][this->IndicesNonZeroMults.TheObjects[j]];
      int Aminus2Bindex = owner.TableAllowedAminus2B.elements[this->IndicesNonZeroMults.TheObjects[i]][this->IndicesNonZeroMults.TheObjects[j]];
      if (AminusBindex!=-1 &&  AminusBindex>this->IndicesNonZeroMults.TheObjects[j])
      { this->decomposeAMinusNB
        (this->IndicesNonZeroMults[i], this->IndicesNonZeroMults[j], 1, AminusBindex, output, theGlobalVariables, Indicator, owner);
        return true;
      }
      if (Aminus2Bindex!=-1 &&  Aminus2Bindex>this->IndicesNonZeroMults[j])
      { this->decomposeAMinusNB
        (this->IndicesNonZeroMults[i], this->IndicesNonZeroMults[j], 2, Aminus2Bindex, output, theGlobalVariables, Indicator, owner);
        return true;
      }
    }
//  Accum.Add(*this);
  return false;
}

bool PartFraction::reduceOnceGeneralMethodNoOSBasis
(PartFractions& owner, MonomialCollection<PartFraction, Polynomial<LargeInt> >& output, GlobalVariables& theGlobalVariables,
 Vector<Rational>* Indicator, Vectors<Rational>& bufferVectors, Matrix<Rational>& bufferMat)
{ if (this->IndicesNonZeroMults.size==owner.AmbientDimension)
    return false;
  Vectors<Rational>& tempRoots=bufferVectors;
  Matrix<Rational>& tempMat= bufferMat;
  tempRoots.size=0;
  int IndexInLinRelationOfLastGainingMultiplicityIndex=-1;
  Vector<Rational> tempRoot;
  for (int i=0; i<this->IndicesNonZeroMults.size; i++)
  { tempRoot.SetSize(owner.AmbientDimension);
    int currentIndex= this->IndicesNonZeroMults[i];
    if (currentIndex== this->LastDistinguishedIndex)
      IndexInLinRelationOfLastGainingMultiplicityIndex=i;
    tempRoot= owner.startingVectors[currentIndex]*this->TheObjects[currentIndex].GetLargestElongation();
    tempRoots.AddOnTop(tempRoot);
    bool ShouldDecompose;
    ShouldDecompose = tempRoots.GetLinearDependence(tempMat);
    if (ShouldDecompose && this->LastDistinguishedIndex!=-1)
    { if (IndexInLinRelationOfLastGainingMultiplicityIndex==-1)
        ShouldDecompose=false;
      else
        ShouldDecompose = !tempMat.elements[IndexInLinRelationOfLastGainingMultiplicityIndex][0].IsEqualToZero();
    }
    if (ShouldDecompose)
    { this->DecomposeFromLinRelation(tempMat, output, Indicator, theGlobalVariables, owner.flagUsingOrlikSolomonBasis, owner.startingVectors);
      return true;
    }
  }
  this->LastDistinguishedIndex=-1;
  return false;
  //tempRoots.r
}

bool PartFraction::ReduceOnceGeneralMethod
(PartFractions& owner, MonomialCollection<PartFraction, Polynomial<LargeInt> >& output, GlobalVariables& theGlobalVariables,
 Vector<Rational>* Indicator, Vectors<Rational>& bufferVectors, Matrix<Rational>& bufferMat)
{ if (this->IndicesNonZeroMults.size==owner.AmbientDimension)
    return false;
  Matrix<Rational>& tempMat = bufferMat;
  bufferVectors.size=0;
  this->LastDistinguishedIndex=this->getSmallestNonZeroIndexGreaterThanOrEqualTo(owner, this->LastDistinguishedIndex);
  int IndexInLinRelationOfLastGainingMultiplicityIndex=-1;
  Vector<Rational> tempRoot;
  for (int i=0; i<this->IndicesNonZeroMults.size; i++)
  { tempRoot.SetSize(owner.AmbientDimension);
    int currentIndex= this->IndicesNonZeroMults[i];
    if (currentIndex== this->LastDistinguishedIndex)
      IndexInLinRelationOfLastGainingMultiplicityIndex=i;
    tempRoot= owner.startingVectors[currentIndex];
    tempRoot*=(this->TheObjects[currentIndex].GetLargestElongation());
    bufferVectors.AddOnTop(tempRoot);
    bool ShouldDecompose;
    ShouldDecompose = bufferVectors.GetLinearDependence(tempMat);
  //  if (PartFraction::flagAnErrorHasOccurredTimeToPanic)
  //  { tempMat.ComputeDebugString();
  //  }
  //  tempMat.ComputeDebugString();
    if (ShouldDecompose && (this->LastDistinguishedIndex!=-1 || this->LastDistinguishedIndex==owner.startingVectors.size))
    { if (IndexInLinRelationOfLastGainingMultiplicityIndex==-1)
        ShouldDecompose=false;
      else
        ShouldDecompose = !tempMat.elements[IndexInLinRelationOfLastGainingMultiplicityIndex][0].IsEqualToZero();
    }
    if (ShouldDecompose)
    { if (this->flagAnErrorHasOccurredTimeToPanic)
      {
      }
      this->DecomposeFromLinRelation(tempMat, output, Indicator, theGlobalVariables, owner.flagUsingOrlikSolomonBasis, owner.startingVectors);
      if (this->flagAnErrorHasOccurredTimeToPanic)
      {
      }
      //this->ComputeDebugString();
      return true;
    }
  }
  return false;
  //tempRoots.r
}

int PartFraction::SizeWithoutDebugString()const
{ int Accum =0;
  Accum+=this->::ListLight<oneFracWithMultiplicitiesAndElongations>::SizeWithoutObjects();
  Accum+=this->size*sizeof(oneFracWithMultiplicitiesAndElongations);
  Accum+= this->IndicesNonZeroMults.SizeWithoutObjects();
  return Accum;
}

void PartFraction::AssignDenominatorOnly(const PartFraction& p)
{ this->CopyFromLight(p);
  this->ComputeIndicesNonZeroMults();
}

void PartFraction::Assign(const PartFraction& p)
{ this->CopyFromLight(p);
  this->IndicesNonZeroMults=(p.IndicesNonZeroMults);
  this->IsIrrelevant= p.IsIrrelevant;
  this->RelevanceIsComputed= p.RelevanceIsComputed;
  this->LastDistinguishedIndex=p.LastDistinguishedIndex;
}

void PartFraction::AssignNoIndicesNonZeroMults(PartFraction& p)
{ this->CopyFromLight(p);
}

void PartFraction::WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables)const
{ output << "Fraction_start: " << this->size << "\n";
  for (int j=0; j<this->size; j++)
  { output << this->TheObjects[j].Multiplicities.size << " ";
    for (int i=0; i<this->TheObjects[j].Multiplicities.size; i++)
      output << this->TheObjects[j].Multiplicities.TheObjects[i] << " " << this->TheObjects[j].Elongations.TheObjects[i] << " ";
  }
}

void PartFraction::ReadFromFile(PartFractions& owner, std::fstream& input, GlobalVariables* theGlobalVariables, int theDimension)
{ std::string tempS;
  int tempI;
  input >> tempS >> tempI;
  assert(tempI==owner.startingVectors.size);
  this->init(tempI);
  for (int j=0; j<this->size; j++)
  { input >> tempI;
    this->TheObjects[j].Multiplicities.SetSize(tempI);
    this->TheObjects[j].Elongations.SetSize(tempI);
    for (int i=0; i<this->TheObjects[j].Multiplicities.size; i++)
      input >> this->TheObjects[j].Multiplicities.TheObjects[i] >> this->TheObjects[j].Elongations.TheObjects[i];
  }
  this->ComputeIndicesNonZeroMults();
}

void PartFraction::ComputeOneCheckSuM(PartFractions& owner, Rational& output, int theDimension, GlobalVariables& theGlobalVariables)const
{ if (this->flagAnErrorHasOccurredTimeToPanic)
  { //this->Coefficient.ComputeDebugString();
  }
  Vector<Rational> CheckSumRoot=oneFracWithMultiplicitiesAndElongations::GetCheckSumRoot(owner.AmbientDimension);
  std::string tempS;
  if (this->flagAnErrorHasOccurredTimeToPanic && Rational::flagAnErrorHasOccurredTimeToPanic)
  { tempS= output.ToString();
  }
  //output.ToString(tempS);
  Rational tempRat;
  for (int i=0; i<this->IndicesNonZeroMults.size; i++)
  { this->TheObjects[this->IndicesNonZeroMults[i]].ComputeOneCheckSum(tempRat, owner.startingVectors[this->IndicesNonZeroMults[i]], theDimension);
    //tempRat.ToString(tempS);
    output.MultiplyBy(tempRat);
    //output.ToString(tempS);
  }
}

std::string PartFraction::ToString
(PartFractions& owner, bool LatexFormat, bool includeVPsummand, bool includeNumerator, FormatExpressions& PolyFormatLocal, GlobalVariables& theGlobalVariables, int& NumLinesUsed)
{ std::stringstream out;
  std::string tempS, stringPoly;
  NumLinesUsed=0;
//  int OldCutOff=0;
//  int theDimension= owner.startingVectors[0].size;
  for (int i =0; i<this->size; i++)
  { this->TheObjects[i].ToString(i, LatexFormat);
    out << tempS;
  }
  tempS= out.str();
  if (LatexFormat)
  { out << stringPoly;
    if (stringPoly.size()>(unsigned) PolyFormatLocal.MaxLineLength)
    { out << ("\\\\\n&&");
      NumLinesUsed++;
    }
    out << tempS;
  }
  else
  { out << stringPoly;
    out << tempS;
  }
  return out.str();
}
#ifdef WIN32
#pragma warning(default:4018)//grrrrr
#endif

bool PartFraction::rootIsInFractionCone(PartFractions& owner, Vector<Rational>* theRoot, GlobalVariables& theGlobalVariables)const
{ if (theRoot==0)
    return true;
  if (this->RelevanceIsComputed)
    return !this->IsIrrelevant;
  if (PartFraction::flagAnErrorHasOccurredTimeToPanic)
  {
  }
  Matrix<Rational>  tempMat, MatOneCol;
  Selection NonPivotPoints;
  Cone tempCone;
  Vectors<Rational> tempRoots;
  for (int i=0; i<this->IndicesNonZeroMults.size; i++)
  { int tempI= this->IndicesNonZeroMults.TheObjects[i];
    tempRoots.AddOnTop(owner.startingVectors[tempI]);
  }
  tempCone.CreateFromVertices(tempRoots, &theGlobalVariables);
  return !this->IsIrrelevant;
}

void PartFraction::PrepareFraction
(int indexA, int indexB, int AminusNBindex, bool indexAisNullified, PartFraction& output, Polynomial<LargeInt>& AminusNbetaPoly, Polynomial<LargeInt>& outputCommonCoeff)
{ output.AssignNoIndicesNonZeroMults(*this);
  int powerDropA = this->TheObjects[indexA].Multiplicities[0];
  int powerDropB = this->TheObjects[indexB].Multiplicities[0];
  if (indexAisNullified)
    powerDropB=0;
  else
    powerDropA=0;
  outputCommonCoeff.MakeOne(AminusNbetaPoly.GetMinNumVars());
  for (int i=0; i<powerDropB; i++)
    outputCommonCoeff*=(AminusNbetaPoly);
  output.DecreasePowerOneFrac(indexA, powerDropA);
  output.DecreasePowerOneFrac(indexB, powerDropB);
  output.DecreasePowerOneFrac(AminusNBindex, -powerDropA-powerDropB);
}

int PartFraction::GetNumProportionalVectorsClassicalRootSystems(PartFractions& owner)
{ int result=0;
  for (int i=0; i<owner.IndicesRedundantShortRoots.CardinalitySelection; i++)
  { int tempI = owner.IndicesRedundantShortRoots.elements[i];
    int tempI2= owner.getIndexDoubleOfARoot(owner.startingVectors[tempI]);
    if (tempI2!=-1)
      if (this->TheObjects[tempI2].Multiplicities.size>0)
        result++;
  }
  return result;
}

int PartFraction::getSmallestNonZeroIndexGreaterThanOrEqualTo(PartFractions& owner, int minIndex)
{ int result=owner.startingVectors.size;
  for (int i=0; i<this->IndicesNonZeroMults.size; i++)
  { int tempI=this->IndicesNonZeroMults.TheObjects[i];
    if (this->TheObjects[tempI].Multiplicities.size>0)
      if (tempI>=minIndex && tempI<result)
        result=tempI;
  }
  return result;
}

bool PartFraction::CheckForOrlikSolomonAdmissibility(List<int>& theSelectedIndices)
{ return true;
  /*if (!this->flagUsingPrecomputedOrlikSolomonBases)
    return true;
  else
    return true; */
}

int PartFraction::ComputeGainingMultiplicityIndexInLinearRelation
(bool flagUsingOrlikSolomon,  Matrix<Rational> & theLinearRelation)
{ int DesireToSelectAsGainingMultiplicity=-1;
  int result=-1;
  for( int i=0; i<theLinearRelation.NumRows; i++)
  { if(! theLinearRelation.elements[i][0].IsEqualToZero())
    { int currentIndex= this->IndicesNonZeroMults.TheObjects[i];
      int candidateDesire;
      if (!flagUsingOrlikSolomon)
        candidateDesire=this->TheObjects[currentIndex].GetTotalMultiplicity();
      else
        candidateDesire= currentIndex;
      if (candidateDesire<0)
        candidateDesire=-candidateDesire;
      if (result==-1 || DesireToSelectAsGainingMultiplicity<candidateDesire)
      { result=i;
        DesireToSelectAsGainingMultiplicity = candidateDesire;
      }
    }
  }
  return result;
}

bool PartFraction::DecomposeFromLinRelation
(Matrix<Rational> & theLinearRelation, MonomialCollection<PartFraction, Polynomial<LargeInt> >& output,
 Vector<Rational>* Indicator, GlobalVariables& theGlobalVariables, bool flagUsingOSbasis, List<Vector<Rational> >& startingVectors)
{//  theLinearRelation.ComputeDebugString();
  //theLinearRelation.ComputeDebugString();
  int GainingMultiplicityIndexInLinRelation=-1;
  int GainingMultiplicityIndex=-1;
  int ElongationGainingMultiplicityIndex=-1;
  List<int> ParticipatingIndices;
  List<int> theGreatestElongations;
  List<int> theCoefficients;
  Rational oldCheckSum;
  ParticipatingIndices.size=0;
  theCoefficients.size=0;
  theGreatestElongations.size=0;
  GainingMultiplicityIndexInLinRelation=this->ComputeGainingMultiplicityIndexInLinearRelation(flagUsingOSbasis, theLinearRelation);
  GainingMultiplicityIndex= this->IndicesNonZeroMults.TheObjects[GainingMultiplicityIndexInLinRelation];
  int tempI=this->TheObjects[GainingMultiplicityIndex].GetLargestElongation();
  theLinearRelation.elements[GainingMultiplicityIndexInLinRelation][0].MultiplyByInt(tempI);
  //theLinearRelation.ComputeDebugString();
  theLinearRelation.ScaleToIntegralForMinRationalHeightNoSignChange();
  if (this->flagAnErrorHasOccurredTimeToPanic)
  {
  }
  ElongationGainingMultiplicityIndex =theLinearRelation.elements[GainingMultiplicityIndexInLinRelation][0].NumShort;
  if (ElongationGainingMultiplicityIndex<0)
    ElongationGainingMultiplicityIndex=-ElongationGainingMultiplicityIndex;
  else
    theLinearRelation*=-1;
  //theLinearRelation.ComputeDebugString();
  for (int i=0; i<theLinearRelation.NumRows; i++)
    if (i!=GainingMultiplicityIndexInLinRelation && !theLinearRelation.elements[i][0].IsEqualToZero())
    { int tempI=this->IndicesNonZeroMults.TheObjects[i];
      ParticipatingIndices.AddOnTop(tempI);
      theGreatestElongations.AddOnTop(this->TheObjects[tempI].GetLargestElongation());
      theCoefficients.AddOnTop(theLinearRelation.elements[i][0].NumShort);
    }
  if (!flagUsingOSbasis)
    this->LastDistinguishedIndex=GainingMultiplicityIndex;
  if (PartFraction::flagAnErrorHasOccurredTimeToPanic)
  {}
 //   this->ComputeDebugString(Accum, theGlobalVariables);
  //if (this->MakingConsistencyCheck)
  //{ this->ComputeOneCheckSum(this->CheckSum2);
  //}
  //if (!this->CheckForOrlikSolomonAdmissibility(ParticipatingIndices))
  //  return false;
  this->ApplyGeneralizedSzenesVergneFormulA
  (ParticipatingIndices, theGreatestElongations, theCoefficients, GainingMultiplicityIndex, ElongationGainingMultiplicityIndex, output, theGlobalVariables, Indicator, startingVectors);

  //if (this->MakingConsistencyCheck)
  //{ assert(this->CheckSum2.IsEqualTo(this->CheckSum));
  //}
  /*if (PartFraction::flagAnErrorHasOccurredTimeToPanic)
  { Rational tempRat2, tempRat;
    std::string tempS1, tempS2;
    Accum.ComputeOneCheckSum(tempRat2);
    this->ComputeOneCheckSum(tempRat);
    tempRat.ToString(tempS1);
    this->CheckSum2.ToString(tempS2);
    tempRat2.Subtract(tempRat);
    assert(oldCheckSum.IsEqualTo(tempRat2));
  }*/
  //Accum.ComputeDebugString();
  return true;
}

bool PartFraction::ReduceMeOnce
(const Polynomial<LargeInt>& myCoeff, Polynomial<LargeInt>& outputCoeff, GlobalVariables& theGlobalVariables, Vector<Rational>* Indicator, Vectors<Rational>& startingVectors)
{ bool hasImprovement=true;
  bool improvedAtLeastOnce=false;
  outputCoeff=myCoeff;
  Polynomial<LargeInt> denominator, quotient, remainderDivision;
  while (hasImprovement)
  { hasImprovement=false;
    for (int i=0; i<this->IndicesNonZeroMults.size; i++)
      for (int j=0; j<this->TheObjects[IndicesNonZeroMults[i]].Multiplicities.size; j++)
      { this->TheObjects[IndicesNonZeroMults[i]].GetPolyDenominator(denominator, j, startingVectors[IndicesNonZeroMults[i]]);
        outputCoeff.DivideBy(denominator, quotient, remainderDivision);
        if (remainderDivision.IsEqualToZero())
        { this->DecreasePowerOneFrac(IndicesNonZeroMults.TheObjects[i], 1);
          outputCoeff=quotient;
          hasImprovement=true;
          improvedAtLeastOnce=true;
        }
      }
  }
  return improvedAtLeastOnce;
}

void PartFraction::GetNElongationPolyWithMonomialContribution
(List<Vector<Rational> >& startingVectors, List<int>& theSelectedIndices, List<int>& theCoefficients, List<int>& theGreatestElongations, int theIndex, Polynomial<LargeInt>& output, int theDimension)
{ MonomialP tempM;
  tempM.MakeOne(theDimension);
  for (int i=0; i<theIndex; i++)
  { int tempI= theSelectedIndices[i];
    for (int j=0; j<theDimension; j++)
      tempM[j]+=startingVectors[tempI][j]*theCoefficients[i]*theGreatestElongations[i];
  }
  this->GetNElongationPoly(startingVectors, theSelectedIndices[theIndex], theGreatestElongations[theIndex], theCoefficients[theIndex], output, theDimension);
  output.MultiplyBy(tempM, 1);
}

void PartFraction::ApplyGeneralizedSzenesVergneFormulA
(List<int>& theSelectedIndices, List<int>& theGreatestElongations, List<int>& theCoefficients, int GainingMultiplicityIndex, int ElongationGainingMultiplicityIndex,
 MonomialCollection<PartFraction, Polynomial<LargeInt> >& output, GlobalVariables& theGlobalVariables, Vector<Rational>* Indicator, List<Vector<Rational> >& startingVectors)
{ PartFraction tempFrac; tempFrac.RelevanceIsComputed=false;
  Polynomial<LargeInt> tempP;
  Polynomial<LargeInt> ComputationalBufferCoefficient;
  Rational StartCheckSum;//=0;
  output.MakeZero();
  int theDim=startingVectors[0].size;
  SelectionWithDifferentMaxMultiplicities TheBigBadIndexingSet;
  TheBigBadIndexingSet.initIncomplete(theSelectedIndices.size);
  int TotalMultiplicity;
  TotalMultiplicity=0;
  for (int i=0; i<theSelectedIndices.size; i++)
  { int tempI= this->TheObjects[theSelectedIndices.TheObjects[i]].GetMultiplicityLargestElongation()-1;
    TheBigBadIndexingSet.MaxMultiplicities.TheObjects[i]=tempI;
    TotalMultiplicity+=tempI;
  }
  Polynomial<LargeInt> currentCoeff;
  for (int i=0; i<theSelectedIndices.size; i++)
  { TheBigBadIndexingSet.clearNoMaxMultiplicitiesChange();
    int oldMaxMultiplicity= TheBigBadIndexingSet.MaxMultiplicities.TheObjects[i];
    TheBigBadIndexingSet.MaxMultiplicities.TheObjects[i]=0;
    int NumSubsets=TheBigBadIndexingSet.getTotalNumSubsets();
    for (int j=0; j<NumSubsets; j++)
    { tempFrac.Assign(*this);
      tempFrac.RelevanceIsComputed=false;
      int tempN= TheBigBadIndexingSet.TotalMultiplicity()+oldMaxMultiplicity;
      if (this->flagAnErrorHasOccurredTimeToPanic)
        TheBigBadIndexingSet.ComputeDebugString();
      for (int k=0; k<theSelectedIndices.size; k++)
      { int multiplicityChange;
        if (k!=i)
          multiplicityChange= TheBigBadIndexingSet.Multiplicities.TheObjects[k];
        else
          multiplicityChange= oldMaxMultiplicity+1;
        tempFrac.TheObjects[theSelectedIndices.TheObjects[k]].AddMultiplicity(-multiplicityChange , theGreatestElongations.TheObjects[k]);
        this->GetNElongationPolyWithMonomialContribution(startingVectors, theSelectedIndices, theCoefficients, theGreatestElongations, k, tempP, theDim);
        tempP.RaiseToPower(multiplicityChange, (LargeInt) 1);
        ComputationalBufferCoefficient*=(tempP);
        LargeInt tempInt;
        int tempI;
        if (k==i)
          tempI = oldMaxMultiplicity;
        else
          tempI=multiplicityChange;
        MathRoutines::NChooseK(tempN, tempI, tempInt);
        ComputationalBufferCoefficient*=(tempInt);
        tempN-=tempI;
      }
      tempFrac[GainingMultiplicityIndex].AddMultiplicity(TheBigBadIndexingSet.TotalMultiplicity()+oldMaxMultiplicity+1, ElongationGainingMultiplicityIndex);
      tempFrac.ComputeIndicesNonZeroMults();
      output.AddMonomial(tempFrac, ComputationalBufferCoefficient);
      TheBigBadIndexingSet.IncrementSubset();
    }
    TheBigBadIndexingSet.MaxMultiplicities.TheObjects[i]= oldMaxMultiplicity;
  }
  if (this->flagAnErrorHasOccurredTimeToPanic)
  { //Rational tempRat;
    //output.ComputeOneCheckSum(tempRat, theGlobalVariables);
    //tempRat.Subtract(theDiff);
    //assert(tempRat.IsEqualTo(StartCheckSum));
  }
}

void PartFraction::ApplySzenesVergneFormulA
(List<Vector<Rational> >& startingVectors,  List<int>& theSelectedIndices, List<int>& theElongations, int GainingMultiplicityIndex,
 int ElongationGainingMultiplicityIndex, MonomialCollection<PartFraction, Polynomial<LargeInt> >& output, GlobalVariables& theGlobalVariables, Vector<Rational>* Indicator)
{ PartFraction tempFrac; tempFrac.RelevanceIsComputed=false;
  Polynomial<LargeInt> tempP, CoefficientBuffer;
  MonomialP tempM;
  //this->lastApplicationOfSVformulaNumNewGenerators=0;
  //this->lastApplicationOfSVformulaNumNewMonomials=0;
  output.MakeZero();
  Rational StartCheckSum, theDiff;
  if (this->flagAnErrorHasOccurredTimeToPanic)
  {// Accum.ComputeOneCheckSum(StartCheckSum, theGlobalVariables);
    //this->ComputeOneCheckSum(Accum, theDiff, Accum.AmbientDimension, theGlobalVariables);
  }
  int theDim=startingVectors[0].size;
  CoefficientBuffer.MakeOne(theDim);
  for(int i=0; i<theSelectedIndices.size; i++)
  { tempFrac.Assign(*this);
    tempFrac.RelevanceIsComputed=false;
    tempFrac[GainingMultiplicityIndex].AddMultiplicity(1, ElongationGainingMultiplicityIndex);
    oneFracWithMultiplicitiesAndElongations& currentFrac=tempFrac[theSelectedIndices[i]];
    int LargestElongation= currentFrac.GetLargestElongation();
    currentFrac.AddMultiplicity(-1, LargestElongation);
    tempM.MakeOne(theDim);
    for (int j=0; j<i; j++)
    { int tempElongation=(int) (*this)[theSelectedIndices[j]].GetLargestElongation();
      for (int k=0; k<theDim; k++)
        tempM[k]+=startingVectors[theSelectedIndices[j]][k]*theElongations[j]*tempElongation;
    }
    ParallelComputing::SafePointDontCallMeFromDestructors();
    CoefficientBuffer.MultiplyBy(tempM, 1);
    this->GetNElongationPoly(startingVectors, theSelectedIndices[i], LargestElongation, theElongations[i], tempP, theDim);
    CoefficientBuffer*=(tempP);
    tempFrac.ComputeIndicesNonZeroMults();
    output.AddMonomial(tempFrac, CoefficientBuffer);
  }
  if (this->flagAnErrorHasOccurredTimeToPanic)
  {// Rational tempRat;
    //Accum.ComputeOneCheckSum(tempRat, theGlobalVariables);
    //tempRat.Subtract(theDiff);
    //assert(tempRat.IsEqualTo(StartCheckSum));
  }
  //this->Coefficient.AssignPolynomial(ComputationalBufferCoefficient);
}

//void PartFraction::GetNElongationPoly

void PartFraction::decomposeAMinusNB
(int indexA, int indexB, int n, int indexAminusNB, MonomialCollection<PartFraction, Polynomial<LargeInt> >& output,
 GlobalVariables& theGlobalVariables, Vector<Rational>* Indicator, PartFractions& owner)
{ PartFraction tempFrac;
  tempFrac.RelevanceIsComputed=false;
  Polynomial<LargeInt> AminusNbetaPoly, commonPoly;
  this->GetAlphaMinusNBetaPoly(owner, indexA, indexB, n, AminusNbetaPoly);
  int powerA= (*this)[indexA].Multiplicities[0];
  int powerB= (*this)[indexB].Multiplicities[0];
  output.MakeZero();
  this->PrepareFraction(indexA, indexB, indexAminusNB, true, tempFrac, AminusNbetaPoly, commonPoly);
  for (int i=powerB; i>=1; i--)
  { LargeInt tempInt=MathRoutines::NChooseK(powerA+powerB-i-1, powerA-1);
    commonPoly*=tempInt;
    output.AddMonomial(tempFrac, commonPoly);
    commonPoly/=tempInt;
    if (i>1)
    { tempFrac[indexAminusNB].AddMultiplicity(1, 1);
      tempFrac[indexB].AddMultiplicity(-1, 1);
      commonPoly*=AminusNbetaPoly;
    }
  }
  this->PrepareFraction(indexA, indexB, indexAminusNB, false, tempFrac, AminusNbetaPoly, commonPoly);
  for (int i=powerA; i>=1; i--)
  { LargeInt tempInt=MathRoutines::NChooseK(powerA+powerB-i-1, powerB-1);
    commonPoly*=tempInt;
    output.AddMonomial(tempFrac, commonPoly);
    commonPoly/=tempInt;
    if (i>1)
    { tempFrac[indexAminusNB].AddMultiplicity(1, 1); ;
      tempFrac[indexA].AddMultiplicity(-1, 1); ;
//      tempFrac.ComputeDebugString();
    }
  }
//  Accum.ComputeDebugString();
}

bool PartFraction::DecreasePowerOneFrac(int index, int increment)
{ this->TheObjects[index].AddMultiplicity(-increment, 1);
  this->ComputeIndicesNonZeroMults();
  return true;
}

void PartFraction::ComputeIndicesNonZeroMults()
{ this->IndicesNonZeroMults.size=0;
  for (int i=0; i<this->size; i++)
    if((*this)[i].Multiplicities.size>0)
      this->IndicesNonZeroMults.AddOnTop(i);
}

void PartFraction::GetAlphaMinusNBetaPoly(PartFractions& owner, int indexA, int indexB, int n, Polynomial<LargeInt>& output)
{ output.MakeZero();
  MonomialP tempM;
  tempM.MakeOne(owner.AmbientDimension);
  for (int i=0; i<n; i++)
  { for (int j=0; j<owner.AmbientDimension; j++)
      tempM[j]= owner.startingVectors[indexA][j]- owner.startingVectors[indexB][j]*(i+1);
    output.AddMonomial(tempM, -1);
  }
}

void PartFraction::GetNElongationPoly(List<Vector<Rational> >& startingVectors, int index, int baseElongation, int LengthOfGeometricSeries, Polynomial<LargeInt>& output, int theDimension)
{ output.MakeZero();
  MonomialP tempM;
  tempM.MakeOne(theDimension);
  if (LengthOfGeometricSeries>0)
    for (int i=0; i<LengthOfGeometricSeries; i++)
    { for (int j=0; j<theDimension; j++)
        tempM[j]=startingVectors[index][j]*baseElongation*i;
      output.AddMonomial(tempM, 1);
    }
  else
    for (int i=-1; i>=LengthOfGeometricSeries; i--)
    { for (int j=0; j<theDimension; j++)
        tempM[j]=startingVectors[index][j]*baseElongation*i;
      output.AddMonomial(tempM, -1);
    }
  //output.ComputeDebugString();
}

void PartFraction::MakePolynomialFromOneNormal(Vector<Rational>& normal, const MonomialP& shiftRational, int theMult, Polynomial<Rational>& output)
{ int theDimension= normal.size;
  output.MakeOne(theDimension);
  if (theMult==1)
    return;
  Rational tempRat, tempRat2;
  Polynomial<Rational> tempP;
  Vector<Rational> shiftRationalVector;
  shiftRationalVector.MakeZero(normal.size);
  for (int i=0; i<normal.size; i++)
    shiftRationalVector[i]=shiftRational(i);
  tempRat=normal.ScalarEuclidean(shiftRationalVector);
  for (int j=0; j<theMult-1; j++)
  { tempP.MakeLinPolyFromRootNoConstantTerm(normal);
    if (PartFraction::flagAnErrorHasOccurredTimeToPanic)
    { //tempP.ComputeDebugString();
    }
    tempRat2.AssignNumeratorAndDenominator(-1, j+1);
    tempRat2.MultiplyBy(tempRat);
    tempRat2+=1;
    Rational tempRat3;
    tempRat3.AssignNumeratorAndDenominator(1, j+1);
    tempP*=(tempRat3);
    if (PartFraction::flagAnErrorHasOccurredTimeToPanic)
    { //tempP.ComputeDebugString();
    }
    tempP.AddConstant(tempRat2);
    if (PartFraction::flagAnErrorHasOccurredTimeToPanic)
    {// tempP.ComputeDebugString();
    }
    output*=tempP;
    if (PartFraction::flagAnErrorHasOccurredTimeToPanic)
    { //output.ComputeDebugString();
    }
  }
}

void PartFraction::ComputeNormals(PartFractions& owner, Vectors<Rational>& output, int theDimension, Matrix<Rational>& buffer)
{ Vectors<Rational> dens;
  Vector<Rational> tempRoot;
  tempRoot.SetSize(theDimension);
  dens.size=0;
  output.size=0;
  for (int i=0; i<theDimension; i++)
  { tempRoot=owner.startingVectors[this->IndicesNonZeroMults[i]];
    dens.AddOnTop(tempRoot);
  }
  Rational tempRat, tempRat2;
  for (int i=0; i<theDimension; i++)
  { dens.ComputeNormalExcludingIndex(tempRoot, i, buffer);
    tempRat=tempRoot.ScalarEuclidean(dens[i]);
    assert(!tempRat.IsEqualToZero());
    tempRoot/=tempRat;
    output.AddOnTop(tempRoot);
//    tempRoot.ComputeDebugString();
  }
}

PartFraction::PartFraction()
{//  this->init(PartFraction::RootsToIndices.size);
  this->PowerSeriesCoefficientIsComputed=false;
  this->AlreadyAccountedForInGUIDisplay=false;
  this->FileStoragePosition=-1;
  this->LastDistinguishedIndex=-1;
  this->RelevanceIsComputed=false;
}

void PartFraction::init(int numRoots)
{ this->IndicesNonZeroMults.ReservE(numRoots);
  this->IndicesNonZeroMults.size=0;
  this->SetSize(numRoots);
  for (int i=0; i<this->size; i++)
  { this->TheObjects[i].Elongations.SetSize(0);
    this->TheObjects[i].Multiplicities.SetSize(0);
  }
}

PartFraction::~PartFraction()
{ /*if (this->indexInGlobalCollectorPartFraction==102)
  { Stop();
  }*/
/*PartFraction::GlobalCollectorPartFraction.
    TheObjects[this->indexInGlobalCollectorPartFraction]=0; */
}

unsigned int PartFraction::HashFunction() const
{ unsigned int result=0;
  for (int i=0; i<this->size; i++)
    result+=SomeRandomPrimes[i]*this->TheObjects[i].HashFunction();
  return result;
}

bool PartFraction::operator==(const PartFraction& right)const
{ if (this->size!= right.size)
    return false;
  for (int i=0; i<this->size; i++)
    if (! (this->TheObjects[i]==right.TheObjects[i]))
      return false;
/*  for(int i=0; i<Vector<Rational>::AmbientDimension; i++)
  { if (this->RootShift[i]!=right.RootShift[i])
      return false;
  }*/
  return true;
}

void PartFraction::operator =(const PartFraction &right)
{ this->Assign(right);
}

int PartFractions::SizeWithoutDebugString()
{ int Accum=0;
  Accum+=  this->theMonomials.SizeWithoutObjects();
  for (int i=0; i<this->size(); i++)
    Accum+=(*this)[i].SizeWithoutDebugString();
  Accum+=  sizeof(this->HighestIndex)+sizeof(this->IndexLowestNonProcessed);
  return Accum;
}

bool PartFractions::AssureIndicatorRegularity(GlobalVariables& theGlobalVariables, Vector<Rational>& theIndicator)
{ Vectors<Rational> tempRoots;
  tempRoots=this->startingVectors;
  if (theIndicator.IsEqualToZero())
  { tempRoots.average(theIndicator, this->AmbientDimension);
    theIndicator*=tempRoots.size;
  }
  return tempRoots.PerturbVectorToRegular(theIndicator, theGlobalVariables);
}

void PartFractions::PrepareCheckSums(GlobalVariables& theGlobalVariables)
{//  ::oneFracWithMultiplicitiesAndElongations::CheckSumRoot.InitFromIntegers(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
//  ::oneFracWithMultiplicitiesAndElongations::CheckSumRoot.DivByInteger(4);
//  ::oneFracWithMultiplicitiesAndElongations::CheckSumRoot.MultiplyByInteger(3);
  if (!this->flagUsingCheckSum)
    return;
  this->ComputeOneCheckSum(this->StartCheckSum, theGlobalVariables);
}

void PartFractions::initFromOtherPartFractions(PartFractions& input, GlobalVariables& theGlobalVariables)
{ this->MakeZero();
  this->startingVectors=input.startingVectors;
  this->IndexLowestNonProcessed=0;
  this->IndexCurrentlyProcessed=0;
  this->AmbientDimension= input.AmbientDimension;
}

void PartFractions::CompareCheckSums(GlobalVariables& theGlobalVariables)
{ if (!this->flagUsingCheckSum)
    return;
  ProgressReport theReport1(&theGlobalVariables);
  ProgressReport theReport2(&theGlobalVariables);
  if (!this->flagDiscardingFractions)
  { this->ComputeOneCheckSum(this->EndCheckSum, theGlobalVariables);
  //PartFraction::MakingConsistencyCheck=true;
  /*if (PartFraction::MakingConsistencyCheck)
  {  this->ComputeDebugString();
    tempRat2.ToString(tempS2);
    tempRat.ToString(tempS1);
  }*/
    if (!this->StartCheckSum.IsEqualTo(this->EndCheckSum) || this->flagAnErrorHasOccurredTimeToPanic)
    { std::stringstream out1, out2;
      out1 << "Starting checksum: " << this->StartCheckSum.ToString();
      out2 << "  Ending checksum: " << this->EndCheckSum.ToString();
      theReport1.Report(out1.str());
      theReport2.Report(out2.str());
    }
    assert(this->StartCheckSum.IsEqualTo(this->EndCheckSum));
    if (!this->StartCheckSum.IsEqualTo(this->EndCheckSum))
      crash << "<b>This is a programmign error. The checksum of the partial fractions failed. " << crash;
    else
    { //std::cout<< "Checksum successful";
      //std::cout.flush();
    }
  }
}

void PartFractions::PrepareIndicatorVariables()
{ this->NumberIrrelevantFractions=0;
  this->NumberRelevantReducedFractions=0;
  this->NumGeneratorsInTheNumerators=0;
  this->NumGeneratorsIrrelevantFractions=0;
  this->NumGeneratorsRelevenatFractions=0;
  this->NumMonomialsInNumeratorsIrrelevantFractions=0;
  this->NumMonomialsInNumeratorsRelevantFractions=0;
  this->NumMonomialsInTheNumerators=1;
  this->NumTotalReduced=0;
  this->NumRelevantNonReducedFractions=0;
  this->NumProcessedForVPFMonomialsTotal=0;
  this->NumProcessedForVPFfractions=0;
  this->NumRunsReduceMonomialByMonomial=0;
}

bool PartFractions::splitPartial(GlobalVariables& theGlobalVariables, Vector<Rational>* Indicator)
{ std::string OldDebugString;
  Matrix<Rational> bufferMat;
  Vectors<Rational> bufferRoots;
  std::stringstream out; std::string tempS;
  PartFraction currentFrac;
  MonomialCollection<PartFraction, Polynomial<LargeInt> > buffer;
  PartFractions reducedForGood;
  Polynomial<LargeInt> currentCoeff;
  reducedForGood.MakeZero();
  if (this->flagUsingCheckSum)
  { this->ComputeOneCheckSum(this->CheckSum, theGlobalVariables);
  }
  while(this->size()>0)
  { this->PopMonomial(0, currentFrac, currentCoeff);
    bool tempBool;
    if (this->flagUsingOrlikSolomonBasis)
      tempBool=currentFrac.ReduceOnceGeneralMethod(*this, buffer, theGlobalVariables, Indicator, bufferRoots, bufferMat);
    else
      tempBool=currentFrac.reduceOnceGeneralMethodNoOSBasis(*this, buffer, theGlobalVariables, Indicator, bufferRoots, bufferMat);
    if (tempBool)
    { buffer*=currentCoeff;
      *this+=buffer;
    } else
      reducedForGood.AddMonomial(currentFrac, currentCoeff);
    this->MakeProgressReportSplittingMainPart(theGlobalVariables);
  }
  if (this->flagUsingCheckSum)
  { Rational tempRat;
    reducedForGood.ComputeOneCheckSum(tempRat, theGlobalVariables);
    if (tempRat!=this->CheckSum)
      crash << "This is a programming error. The checksums of the partial fraction decomposition do not match. " << crash;
  }
  *this=reducedForGood;
  return true;
}

bool PartFractions::splitClassicalRootSystem(bool ShouldElongate, GlobalVariables& theGlobalVariables, Vector<Rational>* Indicator)
{ /*this->IndexLowestNonProcessed=0;
  this->PrepareIndicatorVariables();
  //PartFraction::flagAnErrorHasOccurredTimeToPanic= true;
  //PartFractions::flagAnErrorHasOccurredTimeToPanic= true;
  Rational::flagAnErrorHasOccurredTimeToPanic=true;
  this->PrepareCheckSums(theGlobalVariables);
  std::string tempS;
  this->CheckSum.ToString(tempS);
  // if IndicatorRoot is zero then the caller has forgotten
  // to set the flagUsingIndicatorRoot to false
  PartFraction tempF;
  while (this->IndexLowestNonProcessed<this->size)
  { //this->ComputeDebugString();
//    if (!this->ShouldIgnore(theGlobalVariables, Indicator))
    { tempF.Assign(this->TheObjects[this->IndexLowestNonProcessed]);
      //this->ComputeDebugString();
      //tempF.ComputeDebugString();
      if (! (tempF.reduceOnceTotalOrderMethod(*this, theGlobalVariables, Indicator)))
      { if (this->TheObjects[this->IndexLowestNonProcessed].IndicesNonZeroMults.size-this->TheObjects[this->IndexLowestNonProcessed].GetNumProportionalVectorsClassicalRootSystems(*this)>this->AmbientDimension)
        { this->TheObjects[this->IndexLowestNonProcessed].ComputeDebugString(*this, theGlobalVariables);
          this->NumRelevantNonReducedFractions++;
        }
        this->IndexLowestNonProcessed++;
      }
      else
        this->PopIndexHashChooseSwapByLowestNonProcessedAndAccount( this->IndexLowestNonProcessed, theGlobalVariables, Indicator);
      this->MakeProgressReportSplittingMainPart(theGlobalVariables);
    }
//    this->ComputeDebugString();
//    x= this->SizeWithoutDebugString();
  }
  //this->ComputeDebugString();
//  this->CompareCheckSums();
  if (ShouldElongate)
  {  this->RemoveRedundantShortRootsClassicalRootSystem(theGlobalVariables, Indicator);
//  this->ComputeDebugString();
  }
  this->CompareCheckSums(theGlobalVariables);
  this->IndexLowestNonProcessed= this->size;
  this->MakeProgressReportSplittingMainPart(theGlobalVariables);*/
  return this->CheckForMinimalityDecompositionWithRespectToRoot(Indicator, theGlobalVariables);
}

bool PartFractions::CheckForMinimalityDecompositionWithRespectToRoot(Vector<Rational>* theRoot, GlobalVariables& theGlobalVariables)
{ for (int i=0; i<this->size(); i++)
    if ((*this)[i].IndicesNonZeroMults.size>this->AmbientDimension)
      if ((*this)[i].rootIsInFractionCone(*this, theRoot, theGlobalVariables))
        return false;
  return true;
}

bool PartFraction::initFromRoots(PartFractions& owner, Vectors<Rational>& input)
{ if (input.size==0)
    return false;
  for(int i=0; i<input.size; i++)
    if (input.TheObjects[i].IsEqualToZero())
      return false;
  owner.startingVectors=input;
  this->init(owner.startingVectors.size);
  for(int i=0; i<owner.startingVectors.size; i++)
    this->TheObjects[i].init();
  for (int i=0; i<input.size; i++)
  { int index = owner.getIndex(input[i]);
    this->TheObjects[index].AddMultiplicity(1, 1);
  }
  this->ComputeIndicesNonZeroMults();
  for (int i=0; i<input.size; i++)
    for (int j=0; j<input[i].size; j++)
      if (!input[i][j].IsSmallInteger())
        return false;
  return true;
}

int PartFractions::ReadFromFileComputedContributions(std::fstream& input, GlobalVariables& theGlobalVariables)
{ std::string tempS;
  input.seekg(0);
  input >> tempS;
  int lastNonZero=-1;
  int fileStoragePosition;
  for (int i=0; i<this->size(); i++)
  { int x;
    input >> x >> fileStoragePosition;
    if (fileStoragePosition>lastNonZero)
      lastNonZero=fileStoragePosition;
    assert(x==i);
  }
  this->LimitSplittingSteps=0;
  return lastNonZero;
}

void PartFractions::WriteToFileComputedContributions(std::fstream& output, GlobalVariables&  theGlobalVariables)
{ output.seekp(0);
  output << "Partial_fraction_index/file_storage_position\n";
  for (int i=0; i<this->size(); i++)
    output << i << " " << (*this)[i].FileStoragePosition << "\n";
}

PartFractions::PartFractions()
{ this->HighestIndex=-1;
  this->IndexLowestNonProcessed=-2;
  this->flagSplitTestModeNoNumerators=false;
  this->flagDiscardingFractions=false;
  this->flagUsingCheckSum=true;
  this->flagUsingOrlikSolomonBasis=false;
  this->flagInitialized=false;
  this->SplitStepsCounter=0;
  this->LimitSplittingSteps=0;
}

void PartFraction::ReduceMonomialByMonomial(PartFractions& owner, int myIndex, GlobalVariables& theGlobalVariables, Vector<Rational>* Indicator)
{ /*PartFraction tempFrac;
  //tempFrac.Assign(*this);
  Rational StartCheckSum, theDiff;
  if (this->flagAnErrorHasOccurredTimeToPanic)
  { owner.ComputeOneCheckSum(StartCheckSum, theGlobalVariables);
    this->ComputeOneCheckSuM(owner, theDiff, owner.AmbientDimension, theGlobalVariables);
    this->ComputeDebugString(owner, theGlobalVariables);
    owner.NumRunsReduceMonomialByMonomial++;
  }
  Matrix<Rational> & tempMat= theGlobalVariables.matReduceMonomialByMonomial.GetElement();
  Matrix<Rational> & startAsIdMat = theGlobalVariables.matIdMatrix.GetElement();
  Matrix<Rational> & matColumn = theGlobalVariables.matOneColumn.GetElement();
  Matrix<Rational> & matLinComb = theGlobalVariables.matReduceMonomialByMonomial2.GetElement();
  Selection tempSel;
  MonomialP tempMon;
  Vector<Rational> tempRoot;
  tempMat.init(owner.AmbientDimension, (int) this->IndicesNonZeroMults.size);
  for (int i=0; i<this->IndicesNonZeroMults.size; i++)
    for (int j=0; j<owner.AmbientDimension; j++)
      tempMat.elements[j][i]=(owner.startingVectors[this->IndicesNonZeroMults[i]][j]*this->TheObjects[this->IndicesNonZeroMults[i]].GetLargestElongation());
  if (this->flagAnErrorHasOccurredTimeToPanic)
  {
  }
  startAsIdMat.MakeIdMatrix(owner.AmbientDimension);
  Matrix<Rational> ::GaussianEliminationByRows(tempMat, startAsIdMat, tempSel, false);
  if (this->flagAnErrorHasOccurredTimeToPanic)
  {
  }
  SelectionWithDifferentMaxMultiplicities thePowers;
  List<int> thePowersSigned;
  thePowersSigned.SetSize(this->IndicesNonZeroMults.size);
  thePowers.initIncomplete(this->IndicesNonZeroMults.size);
  for (int k=0; k<this->Coefficient.size; k++)
  { this->Coefficient.TheObjects[k].MonomialExponentToColumnMatrix(matColumn);
    if (this->flagAnErrorHasOccurredTimeToPanic)
    {
    }
    matColumn.MultiplyOnTheLeft(startAsIdMat);
    if (this->flagAnErrorHasOccurredTimeToPanic)
    {
    }
    tempFrac.AssignDenominatorOnly(*this);
    tempFrac.Coefficient.MakeZero(this->Coefficient.NumVars);
    tempFrac.Coefficient.AddMonomial(this->Coefficient[k], this->Coefficient.theCoeffs[k]);
    if (tempMat.RowEchelonFormToLinearSystemSolution(tempSel, matColumn, matLinComb))
    { tempMon=this->Coefficient[k];
      if (this->flagAnErrorHasOccurredTimeToPanic)
      {
      }
      for (int i=0; i<matLinComb.NumRows; i++)
      { thePowers.MaxMultiplicities.TheObjects[i]=0;
        if (matLinComb.elements[i][0].IsGreaterThanOrEqualTo(1) || matLinComb.elements[i][0].IsNegative())
        { int tempI=matLinComb.elements[i][0].floorIfSmall();
          thePowersSigned.TheObjects[i]=tempI;
          if (tempI<0)
            thePowers.MaxMultiplicities.TheObjects[i]=this->TheObjects[this->IndicesNonZeroMults.TheObjects[i]].GetMultiplicityLargestElongation();
          else
            thePowers.MaxMultiplicities.TheObjects[i]=MathRoutines::Minimum(tempI, this->TheObjects[this->IndicesNonZeroMults.TheObjects[i]].GetMultiplicityLargestElongation());
          tempRoot=owner.startingVectors[this->IndicesNonZeroMults[i]];
          tempRoot*=(thePowersSigned[i]*this->TheObjects[this->IndicesNonZeroMults[i]].GetLargestElongation());
          tempMon-=tempRoot;
          if (this->flagAnErrorHasOccurredTimeToPanic)
            tempMon.ComputeDebugString();
        } else
        { thePowers.Multiplicities.TheObjects[i]=0;
          thePowersSigned.TheObjects[i]=0;
        }
      }
      thePowers.ComputeElements();
      int numSummands=thePowers.getTotalNumSubsets();
      if (numSummands==1)
        owner.AddAlreadyReduced(tempFrac, theGlobalVariables, Indicator);
      else
      { PartFractions tempFracs;
        Rational tempDiff;
        if (this->flagAnErrorHasOccurredTimeToPanic)
        { tempFracs.initFromOtherPartFractions(owner, theGlobalVariables);
          tempFrac.AssignDenominatorOnly(*this);
          tempFrac.Coefficient.MakeZero(owner.AmbientDimension);
          tempFrac.Coefficient.AddMonomial(this->Coefficient.TheObjects[k], 1);
          tempFrac.ComputeOneCheckSum(owner, tempDiff, owner.AmbientDimension, theGlobalVariables);
        }
        for (int l=0; l<numSummands; l++)
        { tempFrac.AssignDenominatorOnly(*this);
          if (this->flagAnErrorHasOccurredTimeToPanic)
            thePowers.ComputeDebugString();
          tempFrac.ReduceMonomialByMonomialModifyOneMonomial
          (owner, theGlobalVariables, thePowers, thePowersSigned, tempMon, this->Coefficient.theCoeffs[k]);
          if (this->flagAnErrorHasOccurredTimeToPanic)
            tempFrac.ComputeDebugString(owner, theGlobalVariables);
          tempFrac.ReduceMonomialByMonomial(owner, -1, theGlobalVariables, Indicator);
          if (this->flagAnErrorHasOccurredTimeToPanic)
            tempFrac.ReduceMonomialByMonomial(tempFracs, -1, theGlobalVariables, Indicator);
          thePowers.IncrementSubset();
        }
        if (this->flagAnErrorHasOccurredTimeToPanic)
        { Rational tempFracsCheckSum;
          tempFracs.ComputeOneCheckSum(tempFracsCheckSum, theGlobalVariables);
          tempFracs.ComputeDebugString(theGlobalVariables);
          assert(tempFracsCheckSum.IsEqualTo(tempDiff));
        }
      }
    }
    else
      owner.AddAlreadyReduced(tempFrac, theGlobalVariables, Indicator);
  }
  if (this->flagAnErrorHasOccurredTimeToPanic)
  { Rational tempRat;
    owner.ComputeOneCheckSum(tempRat, theGlobalVariables);
    tempRat.Subtract(theDiff);
    assert(tempRat.IsEqualTo(StartCheckSum));
  }*/
}

void PartFraction::ReduceMonomialByMonomialModifyOneMonomial
(PartFractions& Accum, GlobalVariables& theGlobalVariables, SelectionWithDifferentMaxMultiplicities& thePowers, List<int>& thePowersSigned, MonomialP& input, LargeInt& inputCoeff)
{ /*Polynomial<LargeInt>& theNumerator=theGlobalVariables.PolyLargeIntPartFracBuffer5.GetElement();
  Polynomial<LargeInt>& tempP=theGlobalVariables.PolyLargeIntPartFracBuffer6.GetElement();
  theNumerator.MakeZero(Accum.AmbientDimension);
  theNumerator.AddMonomial(input, inputCoeff);
  assert(thePowersSigned.size== thePowers.Multiplicities.size);
  if (this->flagAnErrorHasOccurredTimeToPanic)
  {// theNumerator.ComputeDebugString();
  }
  for (int j=0; j<thePowers.Multiplicities.size; j++)
  { int currentIndexInFraction = this->IndicesNonZeroMults[j];
    int currentElongation= this->TheObjects[currentIndexInFraction].GetLargestElongation();
    int MultChange= thePowers.Multiplicities[j];
    int MaxMultchange= this->TheObjects[currentIndexInFraction].GetMultiplicityLargestElongation();
    Vector<Rational> tempRoot;
    tempRoot= Accum.startingVectors[currentIndexInFraction];
    tempRoot*=(currentElongation);
    this->GetPolyReduceMonomialByMonomial
    (Accum, theGlobalVariables, tempRoot, thePowersSigned[j], MultChange, MaxMultchange, tempP);
    if (this->flagAnErrorHasOccurredTimeToPanic)
    {// tempP.ComputeDebugString();
    }
    theNumerator*=tempP;
    if (this->flagAnErrorHasOccurredTimeToPanic)
    {// theNumerator.ComputeDebugString();
    }
    this->TheObjects[currentIndexInFraction].AddMultiplicity(-MultChange, currentElongation);
  }
  this->Coefficient=theNumerator;
  this->ComputeIndicesNonZeroMults();
  if (this->flagAnErrorHasOccurredTimeToPanic)
    this->ComputeDebugString(Accum, theGlobalVariables);*/
}

void PartFraction::GetPolyReduceMonomialByMonomial
(PartFractions& owner, GlobalVariables& theGlobalVariables, Vector<Rational>& theExponent, int StartMonomialPower, int DenPowerReduction, int startDenominatorPower, Polynomial<LargeInt>& output)
{ if (StartMonomialPower==0)
  { output.MakeOne(owner.AmbientDimension);
    return;
  }
  MonomialP tempMon;
  tempMon.MakeOne(owner.AmbientDimension);
  output.MakeZero();
  LargeInt theCoeff=1;
  if (StartMonomialPower>0)
  { if (DenPowerReduction!=startDenominatorPower)
    { theCoeff= MathRoutines::NChooseK(StartMonomialPower, DenPowerReduction);
      theCoeff*=MathRoutines::parity(DenPowerReduction);
      output.AddMonomial(tempMon, theCoeff);
    } else
    { Vector<Rational> tempRoot;
      assert(StartMonomialPower>=startDenominatorPower);
      for (int k=0; k<=StartMonomialPower-startDenominatorPower; k++)
      { tempMon= theExponent;
        tempMon.RaiseToPower(k);
        theCoeff= MathRoutines::parity(startDenominatorPower)*MathRoutines::NChooseK(StartMonomialPower-1-k, startDenominatorPower-1);
        output.AddMonomial(tempMon, theCoeff);
      }
    }
  }
  if (StartMonomialPower<0)
  { if (DenPowerReduction!=startDenominatorPower)
    { theCoeff= MathRoutines::NChooseK(-StartMonomialPower-1+DenPowerReduction, DenPowerReduction);
      output.AddMonomial(tempMon, theCoeff);
    } else
    { Vector<Rational> tempRoot;
      for (int k=1; k<=-StartMonomialPower; k++)
      { tempMon= theExponent;
        tempMon.RaiseToPower(-k);
        theCoeff= MathRoutines::NChooseK(startDenominatorPower-StartMonomialPower-1-k, startDenominatorPower-1);
        output.AddMonomial(tempMon, theCoeff);
      }
    }
  }
}

int PartFractions::ToString(std::string& output, bool LatexFormat, bool includeVPsummand, bool includeNumerator, GlobalVariables& theGlobalVariables, FormatExpressions& theFormat)
{ Matrix<LargeInt> tempMat;
  return this->ElementToStringBasisChange(tempMat, false, output, LatexFormat, includeVPsummand, includeNumerator, theGlobalVariables, theFormat);
}

int PartFractions::ElementToStringOutputToFile(std::fstream& output, bool LatexFormat, bool includeVPsummand, bool includeNumerator, GlobalVariables& theGlobalVariables)
{ Matrix<LargeInt> tempMat;
  return this->ElementToStringBasisChangeOutputToFile(tempMat, false, output, LatexFormat, includeVPsummand, includeNumerator, theGlobalVariables);
}

int PartFractions::ElementToStringBasisChange
(Matrix<LargeInt>& VarChange, bool UsingVarChange, std::string& output, bool LatexFormat, bool includeVPsummand, bool includeNumerator, GlobalVariables& theGlobalVariables, FormatExpressions& PolyFormatLocal)
{ std::stringstream out;
  std::string tempS;
  int TotalLines=0;
  PolyFormatLocal.ExtraLinesCounterLatex=0;
  if (LatexFormat)
    out << "\\begin{eqnarray*}\n";
  int LastCutOff=0;
  for (int i=0; i<this->size(); i++)
  { if (this->theCoeffs[i].size()>0 )
    { //TotalLines+=this->TheObjects[i].ElementToStringBasisChange(*this, VarChange, UsingVarChange, tempS, LatexFormat, includeVPsummand, includeNumerator, PolyFormatLocal, theGlobalVariables);
      if (LatexFormat)
        out << "&&";
      if (tempS[0]!='-')
        out << "+";
      out << tempS;
      if (LatexFormat)
      { out << "\\\\ \n";
        TotalLines++;
      }
      else
        out << "\n";
      if (LatexFormat && (TotalLines-LastCutOff)> 40)
      { out << "\\end{eqnarray*}\\begin{eqnarray*}\n";
        LastCutOff=TotalLines;
      }
    }
    if (TotalLines>this->flagMaxNumStringOutputLines)
    { out << "\n Number of lines exceeded " << this->flagMaxNumStringOutputLines << "; The rest of the output was suppressed.";
      break;
    }
  }
  if (!LatexFormat)
  { output= out.str();
    if (output.size()>0)
      if (output[0]=='+')  output.erase(0, 1);
  }
  else
  { out << "\\end{eqnarray*}";
    output= out.str();
  }
  return TotalLines;
}

int PartFractions::ElementToStringBasisChangeOutputToFile
(Matrix<LargeInt>& VarChange, bool UsingVarChange, std::fstream& output, bool LatexFormat, bool includeVPsummand, bool includeNumerator, GlobalVariables& theGlobalVariables)
{ std::string tempS;
  int TotalLines=0;
  FormatExpressions PolyFormatLocal;
  PolyFormatLocal.ExtraLinesCounterLatex=0;
  if (LatexFormat)
    output << "\\begin{eqnarray*}\n";
  int LastCutOff=0;
  for (int i=0; i<this->size(); i++)
  { if (this->theCoeffs[i].size()>0 )
    { //TotalLines+=this->TheObjects[i].ElementToStringBasisChange(*this, VarChange, UsingVarChange, tempS, LatexFormat, includeVPsummand, includeNumerator, PolyFormatLocal, theGlobalVariables);
      if (LatexFormat)
        output << "&&";
      if (tempS[0]!='-')
        output << "+";
      output << tempS;
      if (LatexFormat)
      { output << "\\\\ \n";
        TotalLines++;
      }
      else
        output << "\n";
      if (LatexFormat && (TotalLines-LastCutOff)> 20)
      { output << "\\end{eqnarray*}\\begin{eqnarray*}\n";
        LastCutOff=TotalLines;
      }
    }
  }
  if (LatexFormat)
    output << "\\end{eqnarray*}";
  return TotalLines;
}

#ifdef WIN32
#pragma warning(disable:4018)//grrrrr
#endif
int PartFraction::ControlLineSizeFracs(std::string& output, FormatExpressions& PolyFormatLocal)
{ int numCutOffs= output.size()% PolyFormatLocal.MaxLineLength;
  int LastCutOffIndex=0;
  int NumLinesAdded=0;
  for (int i=0; i<numCutOffs; i++)
    for ( int j=LastCutOffIndex+PolyFormatLocal.MaxLineLength; j<((int) output.size())-1; j++)
      if (output[j]=='\\' && output[j+1]=='f')
      { output.insert(j, "\\\\\n&&");
        NumLinesAdded++;
        LastCutOffIndex=j+5;
        break;
      }
  return NumLinesAdded;
}

int PartFraction::ControlLineSizeStringPolys(std::string& output, FormatExpressions& PolyFormatLocal)
{ int numCutOffs= output.size()% PolyFormatLocal.MaxLineLength;
  int LastCutOffIndex=0;
  int NumLinesAdded=0;
  for(int i=0; i<numCutOffs; i++)
    for(int j=LastCutOffIndex+PolyFormatLocal.MaxLineLength; j<(int)(output.size())-1; j++)
      if ((output[j]=='+'||output[j]=='-')&&output[j-1]!='{')
      { output.insert(j, "\\\\\n&&");
        NumLinesAdded++;
        LastCutOffIndex=j+5;
        break;
      }
  return NumLinesAdded;
}

#ifdef WIN32
#pragma warning(default:4018)//grrrrr
#endif
void PartFractions::MakeProgressReportSplittingMainPart(GlobalVariables& theGlobalVariables)
{ if (theGlobalVariables.GetFeedDataToIndicatorWindowDefault()==0)
    return;
  std::stringstream out1, out2, out3;
  out1 << this->NumberRelevantReducedFractions << " relevant reduced + " << this->NumberIrrelevantFractions << " disjoint = " << this->NumTotalReduced;
  if (this->NumRelevantNonReducedFractions!=0)
    out1 << " + " << this->NumRelevantNonReducedFractions << " relevant unreduced ";
  out1 << " out of "<< this->size() << " total fractions";
  ProgressReport theReport1(&theGlobalVariables);
  ProgressReport theReport2(&theGlobalVariables);
  ProgressReport theReport3(&theGlobalVariables);
  theReport1.Report(out1.str());
  out2 << this->NumMonomialsInNumeratorsRelevantFractions << " relevant reduced + " << this->NumMonomialsInNumeratorsIrrelevantFractions << " disjoint = "
  << this->NumMonomialsInNumeratorsRelevantFractions + this->NumMonomialsInNumeratorsIrrelevantFractions << " out of "
  << this->NumMonomialsInTheNumerators << " total monomials in the numerators";
  theReport2.Report(out2.str());
  if (this->NumGeneratorsInTheNumerators!=0)
  { out3 << this->NumGeneratorsRelevenatFractions << " relevant reduced + " << this->NumGeneratorsIrrelevantFractions << " disjoint = "
    << this->NumGeneratorsIrrelevantFractions +this->NumGeneratorsRelevenatFractions << " out of " << this->NumGeneratorsInTheNumerators << " total generators in the numerators";
    theReport3.Report(out3.str());
  } else
    theReport3.Report("");
}

void PartFractions::MakeProgressVPFcomputation(GlobalVariables& theGlobalVariables)
{ this->NumProcessedForVPFfractions++;
  if (theGlobalVariables.GetFeedDataToIndicatorWindowDefault()==0)
    return;
  std::stringstream out2, out3;
  ProgressReport theReport(&theGlobalVariables);
  out2  << "Processed " << this->NumProcessedForVPFfractions << " out of "
  << this->NumberRelevantReducedFractions << " relevant fractions";
//  out3  << "Processed " <<" out of " <<this->NumMonomialsInNumeratorsRelevantFractions
//        << " relevant fractions";
  theReport.Report(out2.str());
}

void PartFractions::ComputeOneCheckSum(Rational& output, GlobalVariables& theGlobalVariables)
{ output.MakeZero();
  Vector<Rational> CheckSumRoot=oneFracWithMultiplicitiesAndElongations::GetCheckSumRoot(this->AmbientDimension);
  ProgressReport theReport(&theGlobalVariables);
  ProgressReport theReport2(&theGlobalVariables);
  for(int i=0; i<this->size(); i++)
  { Rational currentCheckSum, tempRat;
    (*this)[i].ComputeOneCheckSuM(*this, currentCheckSum, this->AmbientDimension, theGlobalVariables);
    (*this)[i].EvaluateIntPoly(this->theCoeffs[i], CheckSumRoot, tempRat);
    currentCheckSum*=tempRat;
    output+=(tempRat);
    if (this->flagMakingProgressReport)
    { std::stringstream out;
      out << "Checksum " << i+1 << " out of " << this->size();
      theReport.Report(out.str());
    }
  }
  if (this->flagMakingProgressReport)
  { std::stringstream out;
    out << "Checksum: " << output.ToString();
    theReport2.Report(out.str());
  }
}

void PartFractions::initCommon()
{ this->MakeZero();
  this->startingVectors.Clear();
  this->flagInitialized=false;
  this->SplitStepsCounter=1;
}

bool PartFractions::initFromRoots(Vectors<Rational>& input, GlobalVariables& theGlobalVariables)
{ this->initCommon();
  if (input.size<1)
    return false;
  PartFraction f;
  this->AmbientDimension= input.TheObjects[0].size;
  bool tempBool=f.initFromRoots(*this, input);
  Polynomial<LargeInt> tempOne;
  tempOne.MakeOne(this->AmbientDimension);
  this->AddMonomial(f, tempOne);
  return tempBool;
}

void PartFractions::initAndSplit(Vectors<Rational>& input, GlobalVariables& theGlobalVariables)
{ this->initFromRoots(input, theGlobalVariables);
  this->split(theGlobalVariables, 0);
}

void PartFractions::Run(Vectors<Rational>& input, GlobalVariables& theGlobalVariables)
{ if (!this->flagInitialized)
    this->initFromRoots(input, theGlobalVariables);
  this->split(theGlobalVariables, 0);
}

void PartFractions::RemoveRedundantShortRoots(GlobalVariables& theGlobalVariables, Vector<Rational>* Indicator)
{ PartFraction tempFrac;
  Rational startCheckSum, tempCheckSum, tempCheckSum2, tempCheckSum3;
  ProgressReport theReport(&theGlobalVariables);
  if (PartFraction::MakingConsistencyCheck)
    this->ComputeOneCheckSum(startCheckSum, theGlobalVariables);
  MonomialCollection<PartFraction, Polynomial<LargeInt> > buffer, output;
  for (int i=0; i<this->size(); i++)
    if(this->RemoveRedundantShortRootsIndex(buffer, i, theGlobalVariables, Indicator))
    { i--;
      if (this->flagMakingProgressReport)
      { std::stringstream out;
        out << "Elongating denominator " << i+1 << " out of " << this->size();
        theReport.Report(out.str());
      }
    }
}

void PartFractions::RemoveRedundantShortRootsClassicalRootSystem(GlobalVariables& theGlobalVariables, Vector<Rational>* Indicator)
{ /*PartFraction tempFrac;
  Polynomial<LargeInt> buffer;
  for (int i=0; i<this->size; i++)
  { tempFrac.Assign(this->TheObjects[i]);
    if(tempFrac.RemoveRedundantShortRootsClassicalRootSystem(*this, Indicator, buffer, this->AmbientDimension, theGlobalVariables))
    { this->TheObjects[i].Coefficient.MakeZero(this->AmbientDimension);
      this->AddAlreadyReduced(tempFrac, theGlobalVariables, Indicator);
    }
    if (this->flagMakingProgressReport)
    { std::stringstream out;
      out << "Elongating denominator " << i+1 << " out of " << this->size;
      theGlobalVariables.theIndicatorVariables.ProgressReportStrings[3]=out.str();
      theGlobalVariables.FeedIndicatorWindow(theGlobalVariables.theIndicatorVariables);
    }
  }
  for (int i=0; i<this->size; i++)
    if (this->TheObjects[i].Coefficient.IsEqualToZero())
    { this->PopIndexHashChooseSwapByLowestNonProcessedAndAccount(i, theGlobalVariables, Indicator);
      i--;
    }*/
}

void FileSetPutPointerToEnd(std::fstream& theFile, bool StoreToFile)
{ //theFile.close();
  //theFile.open(path);
  assert(theFile.is_open()||!StoreToFile);
  std::filebuf* pbuf = theFile.rdbuf();
  int tempSize = pbuf->pubseekoff(0, std::fstream::end);
  theFile.seekp(tempSize);
}

bool PartFractions::VerifyFileComputedContributions(GlobalVariables&  theGlobalVariables)
{ int tempI= this->ReadFromFileComputedContributions(PartFractions::ComputedContributionsList, theGlobalVariables);
  std::filebuf* pbuf = PartFraction::TheBigDump.rdbuf();
  int tempSize = pbuf->pubseekoff(0, std::fstream::end);
  PartFraction::TheBigDump.seekp(tempSize);
  return(tempSize>=tempI);
}

void PartFractions::ComputeDebugString(GlobalVariables& theGlobalVariables)
{ FormatExpressions tempFormat;
  this->ToString(this->DebugString, tempFormat.flagUseLatex, false, true, theGlobalVariables, tempFormat);
}

void PartFractions::ToString(std::string& output, GlobalVariables& theGlobalVariables, FormatExpressions& theFormat)
{ this->ToString(output, theFormat.flagUseLatex, false, true, theGlobalVariables, theFormat);
}

void PartFractions::ComputeDebugStringNoNumerator(GlobalVariables& theGlobalVariables)
{ FormatExpressions theFormat;
  this->ToString(this->DebugString, theFormat.flagUseLatex, false, false, theGlobalVariables, theFormat);
}

void PartFractions::ComputeDebugStringWithVPfunction(GlobalVariables& theGlobalVariables)
{ FormatExpressions theFormat;
  this->ToString(this->DebugString, theFormat.flagUseLatex, true, true, theGlobalVariables, theFormat);
}

void PartFractions::WriteToFile(std::fstream& output, GlobalVariables* theGlobalVariables)
{ std::string tempS;
  output << "Dimension: ";
  output << this->AmbientDimension << "\n";
  output << "Indices_of_roots:\n";
  FormatExpressions PolyFormatLocal;
  for (int i=0; i<this->startingVectors.size; i++)
    output << "| " << i << "    " << this->startingVectors[i].ToString() << "\n";
  output << "Alphabet_used:\n";
  for (int i=0; i<this->AmbientDimension; i++)
    output << PolyFormatLocal.GetPolyLetter(i) << " ";
  output << "\n" << "Number_of_fractions: " << this->size() << "\n";
  for (int i=0; i<this->size(); i++)
    (*this)[i].WriteToFile(output, theGlobalVariables);
}

void PartFractions::ComputeSupport(List<Vectors<Rational> >& output, std::stringstream& outputString)
{ output.size=0;
  output.SetExpectedSize(this->size());
  for (int i=0; i<this->size(); i++)
  { Vectors<Rational> tempRoots;
//    tempRoots.ComputeDebugString();
    for (int j=0; j<(*this)[i].IndicesNonZeroMults.size; j++)
    { Vector<Rational> tempRoot, tempRoot3;
      tempRoot=(this->startingVectors[(*this)[i].IndicesNonZeroMults[j]]);
      Vector<Rational> tempRoot2;
      tempRoot/=2;
      tempRoot2=tempRoot;
      tempRoot3=tempRoot2;
      if (!(tempRoot3==tempRoot))
        tempRoot*=2;
      tempRoots.AddOnTopNoRepetition(tempRoot);
    }
    output.AddOnTopNoRepetition(tempRoots);
  }
}

void PartFractions::ComputeDebugStringBasisChange(Matrix<LargeInt>& VarChange, GlobalVariables& theGlobalVariables)
{ FormatExpressions tempFormat;
  this->ElementToStringBasisChange(VarChange, true, this->DebugString, tempFormat.flagUseLatex, false, true, theGlobalVariables, tempFormat);
}

bool PartFractions::IsHigherThanWRTWeight(const Vector<Rational>& left, const Vector<Rational>& r, const Vector<Rational>& theWeights)
{ assert(left.size == r.size);
  Rational accum=0;
  for (int i=0; i<left.size; i++)
    accum+=(left[i]-r[i])*theWeights[i];
  return (accum>0);
}
//NOTE NOTE NOTE: To be fixed: you gotta use the precedign function to sort the theVPbasis!
void PartFractions::ComputeKostantFunctionFromWeylGroup
(char WeylGroupLetter, int WeylGroupNumber, QuasiPolynomial& output, Vector<Rational>* ChamberIndicator,
 bool UseOldData, bool StoreToFile, GlobalVariables&  theGlobalVariables)
{ this->initCommon();
  Vectors<Rational> theVPbasis;
  Vector<Rational> tempWeight; tempWeight.SetSize(WeylGroupNumber);
  Vectors<Rational> tempRoots;
  WeylGroup tempW;
  tempW.MakeArbitrarySimple(WeylGroupLetter, WeylGroupNumber);
  tempW.ComputeRho(true);
  theVPbasis=tempW.RootsOfBorel;
  if (WeylGroupLetter=='B')
    for (int i=0; i<theVPbasis.size; i++)
    { Rational tempRat;
      Vector<Rational> tempRoot;
      tempRoot=theVPbasis[i];
      tempW.RootScalarCartanRoot(tempRoot, tempRoot, tempRat);
      if (tempRat.IsEqualToOne())
        theVPbasis.AddOnTop(tempW.RootsOfBorel[i]*2);
    }
  if (WeylGroupLetter=='D')
  { Vector<Rational> tempRoot;
    tempRoot.MakeZero(this->AmbientDimension);
    tempRoot.TheObjects[this->AmbientDimension-1]=1;
    tempRoot.TheObjects[this->AmbientDimension-2]=-1;
    theVPbasis.AddOnTop(tempRoot);
    tempRoot.TheObjects[this->AmbientDimension-1]=1;
    tempRoot.TheObjects[this->AmbientDimension-2]=1;
    theVPbasis.AddOnTop(tempRoot);
    for(int i=this->AmbientDimension-3; i>=0; i--)
    { tempRoot.TheObjects[i]=2;
      crash << "This is a programming error: this line of code needs to be fixed but I don't have time right now."
      << " This code shouldn't be used before the line is fixed! " << crash;
//      theVPbasis.AddObjectOnBottom(tempRoot);
    }
    tempWeight.TheObjects[this->AmbientDimension-2]=7;
    tempWeight.TheObjects[this->AmbientDimension-1]=8;
  }
  theVPbasis.QuickSortAscending();
  //fix this!
  assert(false);
//  this->initFromRoots(theVPbasis, 0, theGlobalVariables);
  //this->flagSplitTestModeNoNumerators=true;
//  this->split(theGlobalVariables, ChamberIndicator);
  assert(this->CheckForMinimalityDecompositionWithRespectToRoot(ChamberIndicator, theGlobalVariables));
  //return;
  Vector<Rational> tempRoot;
  if (ChamberIndicator!=0)
    tempRoot=*ChamberIndicator;
  else
    tempRoot.MakeZero(this->AmbientDimension);
  if(!this->GetVectorPartitionFunction(output, tempRoot, theGlobalVariables))
  { this->ComputeDebugStringNoNumerator(theGlobalVariables);
    assert(false);
  }
  //output.ComputeDebugString();
}

void oneFracWithMultiplicitiesAndElongations::operator =(oneFracWithMultiplicitiesAndElongations& right)
{ this->Multiplicities.CopyFromLight(right.Multiplicities);
  this->Elongations.CopyFromLight(right.Elongations);
}

unsigned int oneFracWithMultiplicitiesAndElongations::HashFunction() const
{ return (unsigned) this->GetTotalMultiplicity();
}

void oneFracWithMultiplicitiesAndElongations::GetPolyDenominator(Polynomial<LargeInt>& output, int MultiplicityIndex, Vector<Rational>& theExponent)
{ assert(MultiplicityIndex<this->Multiplicities.size);
  MonomialP tempM;
  output.MakeOne(theExponent.size);
  tempM.MakeOne(theExponent.size);
  for (int i=0; i<theExponent.size; i++)
    tempM[i]=theExponent[i]*this->Elongations[MultiplicityIndex];
  output.AddMonomial(tempM, -1);
}

int oneFracWithMultiplicitiesAndElongations::GetLargestElongation()
{ int result=this->Elongations.TheObjects[0];
  for (int i=1; i<this->Elongations.size; i++)
  { assert(this->Elongations.TheObjects[i]!=result);
    if (this->Elongations.TheObjects[i]>result)
      result= this->Elongations.TheObjects[i];
  }
  return result;
}

int oneFracWithMultiplicitiesAndElongations::GetLCMElongations()
{ int result =1;
  for (int i=0; i<this->Elongations.size; i++)
  { assert(this->Elongations.TheObjects[i]!=0);
    result=MathRoutines::lcm(this->Elongations.TheObjects[i], result);
  }
  return result;
}

int oneFracWithMultiplicitiesAndElongations::GetTotalMultiplicity() const
{ int result=0;
  for (int i=0; i<this->Elongations.size; i++)
    result+= this->Multiplicities.TheObjects[i];
  return result;
}

int oneFracWithMultiplicitiesAndElongations::IndexLargestElongation()
{ int result=0;
  for (int i=1; i<this->Elongations.size; i++)
    if (this->Elongations.TheObjects[i]>this->Elongations.TheObjects[result])
      result=i;
  return result;
}

void oneFracWithMultiplicitiesAndElongations::init()
{ this->Elongations.SetSize(0);
  this->Multiplicities.SetSize(0);
}

void oneFracWithMultiplicitiesAndElongations::ComputeOneCheckSum(Rational& output, const Vector<Rational>& theExp, int theDimension)
{ output=1;
  std::string tempS;
  Vector<Rational> CheckSumRoot=oneFracWithMultiplicitiesAndElongations::GetCheckSumRoot(theDimension);
  for (int i=0; i<this->Elongations.size; i++)
  { Rational tempRat, tempRat2, tempRat3;
    tempRat=1;
    tempRat2=1;
    for (int j=0; j<theDimension; j++)
    { if (PartFraction::flagAnErrorHasOccurredTimeToPanic)
      { tempS=theExp.ToString();
      }
      tempRat3=CheckSumRoot[j];
      if (!tempRat3.IsEqualToZero())
        tempRat3.RaiseToPower((theExp[j]*this->Elongations[i]).NumShort);
      tempRat2*=tempRat3;
      if (PartFraction::flagAnErrorHasOccurredTimeToPanic)
      { tempS=tempRat2.ToString();
      }
    }
    if (PartFraction::flagAnErrorHasOccurredTimeToPanic)
    { tempS=tempRat.ToString();
    }
    tempRat-=tempRat2;
    tempRat.RaiseToPower(this->Multiplicities[i]);
    if (PartFraction::flagAnErrorHasOccurredTimeToPanic)
      tempS=tempRat.ToString();
    output.MultiplyBy(tempRat);
    if (PartFraction::flagAnErrorHasOccurredTimeToPanic)
      tempS=output.ToString();
  }
  if (output.IsEqualToZero())
  { std::string tempS;
  }
  output.Invert();
  if (PartFraction::flagAnErrorHasOccurredTimeToPanic)
    tempS=output.ToString();
}

int oneFracWithMultiplicitiesAndElongations::GetMultiplicityLargestElongation()
{ int result=0;
  int LargestElongationFound=0;
  for (int i=0; i<this->Elongations.size; i++)
    if (LargestElongationFound<this->Elongations.TheObjects[i])
    { LargestElongationFound= this->Elongations.TheObjects[i];
      result= this->Multiplicities.TheObjects[i];
    }
  return result;
}

void oneFracWithMultiplicitiesAndElongations::AddMultiplicity(int MultiplicityIncrement, int Elongation)
{ if (MultiplicityIncrement==0) return;
  int ElongationIndex=this->Elongations.IndexInList(Elongation);
  if (ElongationIndex==-1)
  { this->Elongations.AddObjectOnTopLight(Elongation);
    int tempI=0;
    this->Multiplicities.AddObjectOnTopLight(tempI);
    ElongationIndex= this->Multiplicities.size-1;
  }
  this->Multiplicities.TheObjects[ElongationIndex]+=MultiplicityIncrement;
  assert(this->Multiplicities.TheObjects[ElongationIndex]>=0);
  if (this->Multiplicities.TheObjects[ElongationIndex]==0)
  { this->Multiplicities.PopIndexSwapWithLastLight(ElongationIndex);
    this->Elongations.PopIndexSwapWithLastLight(ElongationIndex);
  }
}

void oneFracWithMultiplicitiesAndElongations::OneFracToStringBasisChange
(PartFractions& owner, int indexElongation, Matrix<LargeInt>& VarChange, bool UsingVarChange, std::string& output, bool LatexFormat, int indexInFraction, int theDimension,
 FormatExpressions& PolyFormatLocal)
{ std::stringstream out;
  std::string tempS;
  Vector<Rational> tempRoot2, tempRoot;
  tempRoot.SetSize(theDimension);
  tempRoot2.SetSize(theDimension);
  int NumCoords;
  if (UsingVarChange)
  { NumCoords= VarChange.NumRows;
    tempRoot2=owner.startingVectors[indexInFraction];
    for (int i=0; i<NumCoords; i++)
    { tempRoot[i]=0;
      for (int j=0; j<theDimension; j++)
        tempRoot[i]+=tempRoot2[j]*VarChange.elements[i][j];
    }
  }
  else
  { NumCoords=theDimension;
    tempRoot = owner.startingVectors[indexInFraction];
  }
  tempRoot*=(this->Elongations[indexElongation]);
  if (!LatexFormat)
    out << "1/(1-";
  else
    out << "\\frac{1}{(1-";
  for(int i=0; i<NumCoords; i++)
    if (tempRoot[i]!=0)
    { out << PolyFormatLocal.GetPolyLetter(i);
      if (tempRoot[i]!=1)
        out << "^{" << tempRoot[i].ToString() << "}";
    }
  out << ")";
  if (this->Multiplicities[indexElongation]>1)
    out << "^" << this->Multiplicities[indexElongation];
  if (LatexFormat)
    out << "}";
  output= out.str();
}

std::string oneFracWithMultiplicitiesAndElongations::ToString
(int index, bool LatexFormat)
{ if (this->Multiplicities.size==0)
    return "";
  std::stringstream out;
  std::string tempS;
  for (int k=0; k<this->Multiplicities.size; k++)
  { //this->OneFracToStringBasisChange(owner, k, VarChange, UsingVarChange, tempS, LatexFormat, index, theDimension, PolyFormatLocal);
    out << tempS;
  }
  out << " ";
  return out.str();
}

bool oneFracWithMultiplicitiesAndElongations::operator ==(oneFracWithMultiplicitiesAndElongations& right)
{ if (this->Elongations.size!=right.Elongations.size)
    return false;
  for (int i=0; i<this->Elongations.size; i++)
  { bool Found=false;
    for (int j=0; j<right.Elongations.size; j++)
      if (this->Elongations[i]==right.Elongations[j])
      { if(this->Multiplicities[i]!=right.Multiplicities[j])
          return false;
        else
        { Found=true;
          break;
        }
      }
    if (!Found)
      return false;
  }
  return true;
}

void PartFractions::initFromRoots(Vectors<Rational>& theAlgorithmBasis, Vector<Rational>* theWeights)
{ if (theAlgorithmBasis.size==0)
    return;
  int theDimension= theAlgorithmBasis[0].size;
  if (theWeights!=0)
    this->weights=*theWeights;
  this->startingVectors.Clear();
  for (int i=0; i<theAlgorithmBasis.size; i++)
    this->AddRootAndSort(theAlgorithmBasis[i]);
  if (theWeights!=0)
    this->startingVectors.QuickSortAscending();
  this->NumNonRedundantShortRoots= this->size();
  this->ComputeTable(theDimension);
}

void PartFractions::ComputeTable(int theDimension)
{ Vector<Rational> tempR, tempR2, tempR3;
  tempR.SetSize(theDimension);  tempR2.SetSize(theDimension); tempR3.SetSize(theDimension);
  this->IndicesRedundantShortRoots.init(this->size());
  this->IndicesDoublesOfRedundantShortRoots.SetSize(this->size());
  this->TableAllowedAminus2B.init(this->size(), this->size());
  this->TableAllowedAminusB.init(this->size(), this->size());
  for (int i=0; i<this->size(); i++)
  { for (int j=0; j<this->size(); j++)
    { for (int k=0; k<theDimension; k++)
      { tempR[k] = this->startingVectors[i][k]-this->startingVectors[j][k];
        tempR2[k] = this->startingVectors[i][k]-this->startingVectors[j][k]*2;
      }
      this->TableAllowedAminusB.elements[i][j]=this->getIndex(tempR);
      this->TableAllowedAminus2B.elements[i][j]=this->getIndex(tempR2);
    }
    tempR3=this->startingVectors[i];
    tempR3*=2;
    this->IndicesDoublesOfRedundantShortRoots[i] = this->getIndex(tempR3);
    if (IndicesDoublesOfRedundantShortRoots[i]!=-1)
      this->IndicesRedundantShortRoots.AddSelectionAppendNewIndex(i);
  }
}

int PartFractions::AddRootAndSort(Vector<Rational>& theRoot)
{ List<Vector<Rational> > tempList;
  tempList=(this->startingVectors);
  int index=0;
  for (index=0; index<tempList.size; index++)
    if (this->IsHigherThanWRTWeight(theRoot, tempList.TheObjects[index], this->weights))
      break;
  tempList.ShiftUpExpandOnTop(index);
  tempList.TheObjects[index]= theRoot;
  this->startingVectors.Clear();
  for (int i=0; i<tempList.size; i++)
    this->startingVectors.AddOnTop(tempList[i]);
  return index;
}

int PartFractions::getIndex(const Vector<Rational>& TheRoot)
{ return this->startingVectors. GetIndex(TheRoot);
}

int PartFractions::getIndexDoubleOfARoot(const Vector<Rational>& TheRoot)
{ return this->getIndex(TheRoot*2);
}

void SelectionWithMultiplicities::initWithMultiplicities(int NumElements)
{ this->Multiplicities.SetSize(NumElements);
  for (int i=0; i<this->Multiplicities.size; i++)
    this->Multiplicities.TheObjects[i]=0;
  this->elements.ReservE(NumElements);
  this->elements.size=0;
}

void SelectionWithMultiplicities::ToString(std::string& output)
{ std::stringstream out;
  for (int i=0; i<this->elements.size; i++)
    out << "Index: " << this->elements.TheObjects[i] << "\nMultiplicity: "
    << this->Multiplicities.TheObjects[this->elements.TheObjects[i]];
  output= out.str();
}

void SelectionWithMaxMultiplicity::initMaxMultiplicity(int NumElements, int MaxMult)
{ this->::SelectionWithMultiplicities::initWithMultiplicities(NumElements);
  this->MaxMultiplicity=MaxMult;
}

int ::SelectionWithMaxMultiplicity::CardinalitySelectionWithMultiplicities()
{ int result=0;
  for (int i=0; i<this->Multiplicities.size; i++)
    result+=this->Multiplicities.TheObjects[i];
  return result;
}

bool SelectionWithMaxMultiplicity::HasMultiplicitiesZeroAndOneOnly()
{ for(int i=0; i<this->elements.size; i++)
    if (this->Multiplicities.TheObjects[elements.TheObjects[i]]>1)
      return false;
  return true;
}

void SelectionWithMaxMultiplicity::IncrementSubsetFixedCardinality(int Cardinality)
{ if (Cardinality<1 || Cardinality>this->MaxMultiplicity*this->Multiplicities.size)
    return;
  if (this->CardinalitySelectionWithMultiplicities()!=Cardinality)
    this->Multiplicities.initFillInObject(this->Multiplicities.size, 0);
  if (this->CardinalitySelectionWithMultiplicities()==0)
  { for (int i=this->Multiplicities.size-1; Cardinality>0; i--)
    { if (Cardinality>=this->MaxMultiplicity)
        this->Multiplicities.TheObjects[i]=this->MaxMultiplicity;
      else
        this->Multiplicities.TheObjects[i]=Cardinality;
      Cardinality-=this->Multiplicities.TheObjects[i];
    }
    this->ComputeElements();
    return;
  }
  int firstNonZeroMult;
  int currentCardinality=Cardinality;
  for(firstNonZeroMult=this->Multiplicities.size-1; firstNonZeroMult>=0; firstNonZeroMult--)
    if (this->Multiplicities.TheObjects[firstNonZeroMult]>0)
      break;
  if (firstNonZeroMult==0)
    return;
  currentCardinality-=this->Multiplicities.TheObjects[firstNonZeroMult];
  this->Multiplicities.TheObjects[firstNonZeroMult]=0;
  for(int i=firstNonZeroMult-1; i>=0; i--)
    if (this->Multiplicities.TheObjects[i]<this->MaxMultiplicity)
    { this->Multiplicities.TheObjects[i]++;
      currentCardinality++;
      break;
    } else
    { this->Multiplicities.TheObjects[i]=0;
      currentCardinality-=this->MaxMultiplicity;
    }
  for (int i=this->Multiplicities.size-1; currentCardinality<Cardinality; i--)
  { assert(this->Multiplicities.TheObjects[i]==0);
    if (Cardinality-currentCardinality>=this->MaxMultiplicity)
      this->Multiplicities.TheObjects[i]=this->MaxMultiplicity;
    else
      this->Multiplicities.TheObjects[i]=Cardinality-currentCardinality;
    currentCardinality+=this->Multiplicities.TheObjects[i];
  }
  this->ComputeElements();
}

int ::SelectionWithMaxMultiplicity::NumCombinationsOfCardinality(int cardinality)
{ //this function needs a complete rewrite;
  return ::MathRoutines::NChooseK(this->Multiplicities.size+cardinality-1, cardinality);
}

LargeInt SelectionWithMaxMultiplicity::GetNumTotalCombinations()const
{ //if (this->MaxMultiplicity==0)
  //  return 1;
  LargeInt result;
  MathRoutines::KToTheNth(this->MaxMultiplicity+1, this->Multiplicities.size, result);
  return result;
}

bool SelectionWithMaxMultiplicity::IncrementReturnFalseIfBackToBeginning()
{ this->IncrementSubset();
  return this->elements.size!=0;
}

void SelectionWithMaxMultiplicity::IncrementSubset()
{ for (int i=this->Multiplicities.size-1; i>=0; i--)
    if (this->Multiplicities.TheObjects[i]<this->MaxMultiplicity)
    { if (this->Multiplicities.TheObjects[i]==0)
        this->elements.AddOnTop(i);
      this->Multiplicities.TheObjects[i]++;
      return;
    }
    else
    { this->Multiplicities.TheObjects[i]=0;
      this->elements.RemoveFirstOccurenceSwapWithLast(i);
    }
}

void SelectionWithMultiplicities::ComputeElements()
{ this->elements.size=0;
  for (int i=0; i<this->Multiplicities.size; i++)
    if (this->Multiplicities.TheObjects[i]>0)
      this->elements.AddOnTop(i);
}

int SelectionWithMultiplicities::CardinalitySelectionWithoutMultiplicities()
{ return this->elements.size;
}

int SelectionWithDifferentMaxMultiplicities::getTotalNumSubsets()
{ int result=1;
  for (int i=0; i<this->MaxMultiplicities.size; i++)
  { result*=(this->MaxMultiplicities[i]+1);
    if (result<0)
    { crash << "This is a programming error: I was asked to enumerate all subsets of a multi-set, however the number of subsets is larger than  "
      << " the maximum value allowed for int on the system (on a 32 bit machine that is around  2 billion). This can be fixed, however I do not have time at the moment. If you "
      << " encounter this error, write me an email and I will take the time to fix this issue. "
      << crash;
      assert(false);
    }
  }
  return result;
}

void SelectionWithDifferentMaxMultiplicities::initFromInts(int* theMaxMults, int NumberMaxMults)
{ this->Multiplicities.initFillInObject(NumberMaxMults, 0);
  this->MaxMultiplicities.SetSize(NumberMaxMults);
  for (int i=0; i<this->MaxMultiplicities.size; i++)
    this->MaxMultiplicities.TheObjects[i]=theMaxMults[i];
  this->elements.initFillInObject(NumberMaxMults, 0);
}

void SelectionWithDifferentMaxMultiplicities::initFromInts(const List<int>& theMaxMults)
{ this->Multiplicities.initFillInObject(theMaxMults.size, 0);
  this->elements.initFillInObject(theMaxMults.size, 0);
  this->MaxMultiplicities=theMaxMults;
}

int ::SelectionWithDifferentMaxMultiplicities::TotalMultiplicity()
{ int result=0;
  for (int i=0; i<this->Multiplicities.size; i++)
    result+=this->Multiplicities.TheObjects[i];
  return result;
}

int ::SelectionWithDifferentMaxMultiplicities::MaxTotalMultiplicity()
{ int result=0;
  for (int i=0; i<this->Multiplicities.size; i++)
    result+=this->MaxMultiplicities.TheObjects[i];
  return result;
}

void ::SelectionWithDifferentMaxMultiplicities::clearNoMaxMultiplicitiesChange()
{ for (int i=0; i<this->Multiplicities.size; i++)
    this->Multiplicities.TheObjects[i]=0;
}

void SelectionWithDifferentMaxMultiplicities::IncrementSubset()
{  for (int i=this->Multiplicities.size-1; i>=0; i--)
    if (this->Multiplicities.TheObjects[i]<this->MaxMultiplicities.TheObjects[i])
    { if (this->Multiplicities.TheObjects[i]==0)
        this->elements.AddOnTop(i);
      this->Multiplicities.TheObjects[i]++;
      return;
    }
    else
    { this->Multiplicities.TheObjects[i]=0;
      this->elements.RemoveFirstOccurenceSwapWithLast(i);
    }
}

void DynkinType::GetTypesWithMults(List<DynkinSimpleType>& output)const
{ output.SetSize(0);
  output.ReservE(this->GetNumSimpleComponents());
  List<DynkinSimpleType> componentsSorted=this->theMonomials;
  componentsSorted.QuickSortAscending();
  for (int i=0; i<componentsSorted.size; i++)
  { int theIndex= this->theMonomials.GetIndex(componentsSorted[i]);
    for (int j=0; j<this->GetMult(theIndex); j++)
      output.AddOnTop(componentsSorted[i]);
  }
}

void DynkinType::GetOuterAutosGeneratorsOneType(List<Matrix<Rational> >& output, const DynkinSimpleType& theType, int multiplicity)const
{ MacroRegisterFunctionWithName("DynkinType::GetOuterAutosGeneratorsOneType");
  output.SetSize(0);
  Matrix<Rational> tempMat, finalMat;
  if (theType.theLetter=='D' || (theType.theLetter=='A'&& theType.theRank>1) ||
      (theType.theLetter=='E' && theType.theRank==6))
  { tempMat.MakeIdMatrix(theType.theRank*(multiplicity-1));
    int numGens=1;
    if (theType.theLetter=='D' && theType.theRank==4)
      numGens=2;
    for (int i=1; i<numGens+1; i++)
    { theType.GetAutomorphismActingOnVectorROWSwhichStandOnTheRight(finalMat, i);
      finalMat.Transpose();
      finalMat.DirectSumWith(tempMat);
      output.AddOnTop(finalMat);
    }
  }
  if (multiplicity<2)
    return;
  for (int i=0; i<multiplicity-1; i++)
  { tempMat.init(theType.theRank*2, theType.theRank*2);
    tempMat.NullifyAll();
    for (int j=0; j<theType.theRank; j++)
    { tempMat(j, theType.theRank+j)=1;
      tempMat(theType.theRank+j,j)=1;
    }
    finalMat.MakeIdMatrix(i*theType.theRank);
    finalMat.DirectSumWith(tempMat);
    tempMat.MakeIdMatrix((multiplicity-2-i)*theType.theRank);
    finalMat.DirectSumWith(tempMat);
    output.AddOnTop(finalMat);
  }
}

void DynkinType::GetOuterAutosGenerators(List<Matrix<Rational> >& output)const
{ MacroRegisterFunctionWithName("DynkinType::GetOuterAutosGenerators");
  List<Matrix<Rational> > intermediateGenerators;
  Matrix<Rational> matrixFinal, matrixToGo;
  int currentMult;
  output.SetSize(0);
  int numRowsSoFar=0;
  for (int i=0; i<this->size(); i++)
  { if (!this->theCoeffs[i].IsSmallInteger(&currentMult))
      assert(false);
    this->GetOuterAutosGeneratorsOneType(intermediateGenerators,(*this)[i], currentMult);
    matrixToGo.MakeIdMatrix(this->GetRank()-numRowsSoFar-currentMult*(*this)[i].theRank);
    for (int j=0; j<intermediateGenerators.size; j++)
    { matrixFinal.MakeIdMatrix(numRowsSoFar);
      matrixFinal.DirectSumWith(intermediateGenerators[j]);
      matrixFinal.DirectSumWith(matrixToGo);
      output.AddOnTop(matrixFinal);
    }
    numRowsSoFar+=currentMult*(*this)[i].theRank;
  }
}

void DynkinType::GetLettersTypesMults
(List<char>* outputLetters, List<int>* outputRanks, List<int>* outputMults,
 List<Rational>* outputFirstCoRootLengthsSquared)const
{ if (outputLetters!=0)
    outputLetters->SetSize(0);
  if (outputRanks!=0)
    outputRanks->SetSize(0);
  if (outputMults!=0)
    outputMults->SetSize(0);
  if (outputFirstCoRootLengthsSquared!=0)
    outputFirstCoRootLengthsSquared->SetSize(0);
  List<DynkinSimpleType> componentsSorted=this->theMonomials;
  componentsSorted.QuickSortAscending();
  for (int i=0; i<componentsSorted.size; i++)
  { int theIndex= this->theMonomials.GetIndex(componentsSorted[i]);
    if (outputLetters!=0)
      outputLetters->AddOnTop((*this)[theIndex].theLetter);
    if (outputRanks!=0)
      outputRanks->AddOnTop((*this)[theIndex].theRank);
    if (outputFirstCoRootLengthsSquared!=0)
      outputFirstCoRootLengthsSquared->AddOnTop((*this)[theIndex].lengthFirstCoRootSquared);
    if (outputMults!=0)
      outputMults->AddOnTop(this->GetMult(theIndex));
  }
}

std::string DynkinType::GetLieAlgebraName(FormatExpressions* theFormat)const
{ return this->ToString(theFormat);
}

std::string DynkinType::GetWeylGroupName(FormatExpressions* theFormat)const
{ return this->ToString(theFormat);
}

void DynkinType::MakeSimpleType(char type, int rank, const Rational* inputFirstCoRootSqLength)
{ DynkinSimpleType theMon;
  theMon.theRank=rank;
  theMon.theLetter=type;
  if (inputFirstCoRootSqLength==0)
    theMon.lengthFirstCoRootSquared= 2;
  else
    theMon.lengthFirstCoRootSquared=*inputFirstCoRootSqLength;
  if (theMon.lengthFirstCoRootSquared<=0)
  { std::cout << "This is a programming error: co-root length must be positive, instead I got " << theMon.lengthFirstCoRootSquared << ". "
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  this->MakeZero();
  this->AddMonomial(theMon, 1);
}

bool DynkinType::HasExceptionalComponent()const
{ for (int i=0; i<this->size(); i++)
    if ((*this)[i].theLetter=='E'|| (*this)[i].theLetter=='F'|| (*this)[i].theLetter=='G')
      return true;
  return false;
}

bool DynkinType::IsSimple(char* outputtype, int* outputRank, Rational* outputLength)const
{ if (this->size()!=1)
    return false;
  if (this->theCoeffs[0]!=1)
    return false;
  const DynkinSimpleType& theMon=(*this)[0];
  if (outputtype!=0)
    *outputtype=theMon.theLetter;
  if (outputRank!=0)
    *outputRank=theMon.theRank;
  if (outputLength!=0)
    *outputLength=theMon.lengthFirstCoRootSquared;
  return true;
}

int DynkinType::GetNumSimpleComponents()const
{ Rational result=0;
  for (int i=0; i<this->size(); i++)
    result+=this->theCoeffs[i];
  int output;
  if (!result.IsSmallInteger(&output))
  { std::cout << "This is a programming error: Dynkin type has a number of simple components which is "
    << " not a small integer. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return output;
}

Rational DynkinType::GetRankRational()const
{ Rational result=0;
  for (int i=0; i<this->size(); i++)
    result+=this->theCoeffs[i]*(*this)[i].theRank;
  return result;
}

int DynkinType::GetRank()const
{ Rational tempRat = this->GetRankRational();
  int result;
  if (!tempRat.IsSmallInteger(&result))
  { std::cout << "This is a programming error: attempting to get a small integer rank from a Dynkin type whose rank is not a small integer, but is instead " << tempRat
    << ". " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  return result;
}

void DynkinType::GetEpsilonMatrix(Matrix<Rational>& output)const
{ output.init(0,0);
  Matrix<Rational> curCartan;
  List<DynkinSimpleType> sortedMons;
  sortedMons=this->theMonomials;
  sortedMons.QuickSortAscending();
  for (int j=0; j<sortedMons.size; j++)
  { int theIndex=this->theMonomials.GetIndex(sortedMons[j]);
    int theMult=this->GetMult(theIndex);
    for (int k=0; k<theMult; k++)
    { DynkinSimpleType::GetEpsilonMatrix
      ((*this)[theIndex].theLetter, (*this)[theIndex].theRank, curCartan);
      output.DirectSumWith(curCartan);
    }
  }
}

void DynkinType::GetCartanSymmetric(Matrix<Rational>& output)const
{ output.init(0,0);
  Matrix<Rational> curCartan;
  List<DynkinSimpleType> sortedMons;
  sortedMons=this->theMonomials;
  sortedMons.QuickSortAscending();
  for (int j=0; j<sortedMons.size; j++)
  { int theIndex=this->theMonomials.GetIndex(sortedMons[j]);
    int mult=this->GetMult(theIndex);
    for (int k=0; k<mult; k++)
    { (*this)[theIndex].GetCartanSymmetric(curCartan);
      output.DirectSumWith(curCartan);
    }
  }
}

int DynkinType::GetCoxeterEdgeWeight(int v, int w)
{ if(v == w)
    return 0;
  Matrix<Rational> M;
  this->GetCartanSymmetric(M);
  if(M(v,w) == 0)
    return 2;
  Rational c2 = M(v,w)*M(v,w)/M(v,v)/M(w,w);
  c2 = 3/c2;
  if(c2 == 12)
    return 3;
  else
    if(c2 == 6)
      return 4;
    else
      if(c2 == 4)
        return 6;
  std::cout << "if you would like an edge weight of a non-crystallographic Coxeter graph, replace the code near " << __FILE__ << ":" << __LINE__ << " with a real arccos function" << std::endl;
  assert(false);
}

Rational DynkinType::GetSizeWeylGroupByFormula()const
{ Rational result=1;
  Rational tempRat;
  for (int i=0; i<this->size(); i++)
  { tempRat=WeylGroup::GetSizeWeylGroupByFormula((*this)[i].theLetter, (*this)[i].theRank);
    tempRat.RaiseToPower(this->GetMult(i));
    result*=tempRat;
  }
  return result;
}

Rational DynkinSimpleType::GetLongRootLengthSquared()const
{ switch (this->theLetter)
  { case 'A':
    case 'B':
    case 'D':
    case 'E':
    case 'F':
      return this->lengthFirstCoRootSquared;
    case 'G':
      return this->lengthFirstCoRootSquared*3;
    case 'C':
      return this->lengthFirstCoRootSquared*2;
    default:
      break;
  }
  std::cout << "This is a programming error: calling DynkinSimpleType::GetLongRootLengthSquared on a non-initialized simple type. "
  << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  assert(false);
  return -1;
}

std::string DynkinSimpleType::ToString(FormatExpressions* theFormat)const
{ std::stringstream out;
  bool includeTechnicalNames= theFormat==0 ? true : theFormat->flagIncludeLieAlgebraTypes;
  bool includeNonTechnicalNames=theFormat==0 ? false : theFormat->flagIncludeLieAlgebraNonTechnicalNames;
  if (!includeNonTechnicalNames && !includeTechnicalNames)
    includeTechnicalNames=true;
  bool hasAmbient=false;
  if (theFormat!=0)
    hasAmbient=(theFormat->AmbientWeylLetter!='X');
  if (includeTechnicalNames)
  { if (!hasAmbient)
      out << theLetter << "^{" << (this->lengthFirstCoRootSquared/2).ToString() << "}";
    else
    { DynkinSimpleType tempType;
      tempType.theLetter=theFormat->AmbientWeylLetter;
      tempType.lengthFirstCoRootSquared=theFormat->AmbientWeylLengthFirstCoRoot;
//      out << "[" << this->theLetter << "^{" << this->lengthFirstCoRootSquared << "}_" << this->theRank << "]";
      Rational theDynkinIndex=this->lengthFirstCoRootSquared*tempType.GetRatioLongRootToFirst()/tempType.GetLongRootLengthSquared();
      if (tempType.theLetter=='C')
        if (this->theLetter=='A' || this->theLetter=='B' || this->theLetter=='D' || this->theLetter=='E' || this->theLetter=='F')
          theDynkinIndex*=2;
      if (tempType.theLetter=='G')
        if (this->theLetter=='A' || this->theLetter=='B' || this->theLetter=='D' || this->theLetter=='E' || this->theLetter=='F')
          theDynkinIndex*=3;

      //Rational theRatio;
      //if (!theRatioSquared.GetSquareRootIfRational(theRatio))
      //{ std::cout << "This is a programming error: wrong ambient dynkin type. The ratio of long roots is: "
      //  << theRatio.ToString() << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      //  assert(false);
      //}
      out << theLetter;
      if (theDynkinIndex!=1)
        out << "^{" << theDynkinIndex.ToString() << "}";
    }
    if (this->theRank>=10)
      out << "_{" << this->theRank << "}";
    else
      out << "_" << this->theRank;
  }
  if (includeNonTechnicalNames)
    if (this->theLetter!='E' && this->theLetter!='F' && this->theLetter!='G')
      switch (this->theLetter)
      { case 'A':  out << "(sl(" <<   this->theRank+1  << "))"; break;
        case 'B':  out << "(so(" << 2*this->theRank+1 << "))"; break;
        case 'C':  out << "(sp(" << 2*this->theRank   << "))"; break;
        case 'D':  out << "(so(" << 2*this->theRank   << "))"; break;
        default : break;
      }
  return out.str();
}

int DynkinSimpleType::GetRootSystemSize()const
{ switch(this->theLetter)
  { case 'A':
      return this->theRank*(this->theRank+1);
    case 'B':
    case 'C':
      return this->theRank*this->theRank*2;
    case 'D':
      return this->theRank*(this->theRank-1) *2;
    case 'E':
      switch(this->theRank)
      { case 6: return 72;
        case 7: return 126;
        case 8: return 240;
        default: return -1;
      }
    case 'G':
      return 12;
    case 'F':
      return 48;
    default: return -1;
  }
}

Rational DynkinSimpleType::GetDefaultCoRootLengthSquared(int rootIndex)const
{ Rational result=this->GetDefaultRootLengthSquared(rootIndex);
  result.Invert();
  result*=4;
  return result;
}

Rational DynkinSimpleType::GetRatioRootSquaredToFirstSquared
(int rootIndex)const
{ Rational result;
  switch (this->theLetter)
  { case 'A':
    case 'D':
    case 'E':
      return 1;
    case 'B':
      if (rootIndex==this->theRank-1)
      { result.AssignNumeratorAndDenominator(1,2);
        return result;
      }
      return 1;
    case 'C':
      if (rootIndex==this->theRank-1)
        return 2;
      return 1;
    case 'F':
      if (rootIndex<2)
        return 1;
      result.AssignNumeratorAndDenominator(1,2);
      return result;
    case 'G':
      if (rootIndex==1)
        return 3;
      return 1;
    default:
      return -1;
  }
}

Rational DynkinSimpleType::GetDefaultRootLengthSquared(int rootIndex)const
{ if (rootIndex>=this->theRank)
  { std::cout << "This is a programming error: attempting to get the squared length of simple root number " << rootIndex+1
    << ", however the root system if of rank " << this->theRank << ". " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  switch (this->theLetter)
  { case 'A':
    case 'D':
    case 'E':
      return 2;
    case 'B':
      if (rootIndex==this->theRank-1)
        return 1;
      return 2;
    case 'C':
      if (rootIndex==this->theRank-1)
        return 4;
      return 2;
    case 'F':
      if (rootIndex<2)
        return 2;
      return 1;
    case 'G':
      if (rootIndex==1)
        return 6;
      return 2;
    default:
      std::cout << "This is a programming error: "
      << "calling DynkinSimpleType::GetDefaultRootLengthSquared on the non-initialized "
      << " Dynkin type " << this->ToString()
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
      return -1;
  }
}

void DynkinSimpleType::GetEpsilonMatrix(char WeylLetter, int WeylRank, Matrix<Rational>& output)
{ if (WeylLetter=='A')
  { output.init(WeylRank+1, WeylRank);
    output.NullifyAll();
    for (int i=0; i<WeylRank; i++)
    { output(i,i)=1;
      output(i+1,i)=-1;
    }
    output(WeylRank,WeylRank-1)=-1;
  }
  if (WeylLetter=='B')
  { output.init(WeylRank, WeylRank);
    output.NullifyAll();
    for (int i=0; i<WeylRank-1; i++)
    { output(i,i)=1;
      output(i+1,i)=-1;
    }
    output(WeylRank-1, WeylRank-1)=1;
  }
  if (WeylLetter=='C')
  { output.init(WeylRank, WeylRank);
    output.NullifyAll();
    for (int i=0; i<WeylRank-1; i++)
    { output(i,i)=1;
      output(i+1,i)=-1;
    }
    output(WeylRank-1,WeylRank-1)=2;
  }
  if (WeylLetter=='D')
  { output.init(WeylRank, WeylRank);
    output.NullifyAll();
    for (int i=0; i<WeylRank-1; i++)
    { output(i,i)=1;
      output(i+1,i)=-1;
    }
    output(WeylRank-1,WeylRank-1)=1;
    output(WeylRank-2,WeylRank-1)=1;
  }
  Rational RHalf(1,2);
  Rational RMHalf(-1,2);
  if (WeylLetter=='E')
  { //Epsilon convention taken with slight modification from
    //Humpreys, Introduction to Lie algebras and representation theory, page 65
    //first comes first root, then the sticky part, then string with the rest of the roots.
    output.init(8, WeylRank);
    output.NullifyAll();
    //first simple root: -1/2e_1-1/2e_8+1/2e_2+1/2e_3+1/2e_4+1/2e_5+1/2e_6+1/2e_7
    output(0,0)=RMHalf;
    output(1,0)=RHalf;
    output(2,0)=RHalf;
    output(3,0)=RHalf;
    output(4,0)=RHalf;
    output(5,0)=RHalf;
    output(6,0)=RHalf;
    output(7,0)=RMHalf;
    //2nd simple root: -e_1-e_2 (that is the sticky piece of the Dynkin diagram)
    output(0,1)=-1;
    output(1,1)=-1;
    //3rd simple root: e_1-e_2
    output(0,2)=1;
    output(1,2)=-1;
    //4th simple root: e_2-e_3
    output(1,3)=1;
    output(2,3)=-1;
    //5th simple root: e_3-e_4
    output(2,4)=1;
    output(3,4)=-1;
    //6th simple root: e_4-e_5
    output(3,5)=1;
    output(4,5)=-1;
    if (WeylRank>6)
    {//7th simple root: e_5-e_6
      output(4,6)=1;
      output(5,6)=-1;
    }
    if(WeylRank>7)
    {//8th simple root: e_6-e_7
      output(5,7)=1;
      output(6,7)=-1;
    }
  }
  if (WeylLetter=='F')
  { //as of May 11 2013 the convention has been changed to coincide with that of
    //Wikipedia
    output.init(4, 4);
    output.NullifyAll();

    //image of first simple root = e_1-e_2 (long one):
    output(0,0)=1;
    output(1,0)=-1;
    //image of second simple root = e_2-e_3 (long one)
    output(1,1)=1;
    output(2,1)=-1;
    //image of third simple root = e_3 (short one):
    output(2,2)=1;
    //image of fourth simple root (short one)
    output(0,3)=RMHalf;
    output(1,3)=RMHalf;
    output(2,3)=RMHalf;
    output(3,3)=RMHalf;
    //eps_2:
    //eps_4:
  }
  if (WeylLetter=='G')
  { //taken from Humpreys, Introduction to Lie algebras and representation theory, page 65
    // the long root has the higher index
    output.init(3, 2);
    output.NullifyAll();
    //image of the first simple root(short one):
    output(0,0)=1;
    output(1,0)=-1;
    //image of second simple root:
    output(0,1)=-2;
    output(1,1)=1;
    output(2,1)=1;
  }
}

void DynkinSimpleType::GetAn(int n, Matrix<Rational>& output)const
{ if (n<=0 || n>30000)
  { std::cout << "This is a programming error: attempting to create type A_n with n=" << n << " is illegal. If this was a bad user input, it should "
    << " be handled at an earlier stage. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  output.init(n, n);
  output.NullifyAll();
  for (int i=0; i<n-1; i++)
  { output(i,i)=2;
    output(i+1,i)=-1;
    output(i,i+1)=-1;
  }
  output(n-1,n-1)=2;
}

void DynkinSimpleType::GetBn(int n, Matrix<Rational>& output)const
{ this->GetAn(n, output);
  output(n-1,n-1)=1;
}

void DynkinSimpleType::GetCn(int n, Matrix<Rational>& output)const
{ this->GetAn(n, output);
  if(n<2)
    return;
  output(n-1,n-1)= 4;
  output(n-2,n-1)=-2;
  output(n-1,n-2)=-2;
}

void DynkinSimpleType::GetDn(int n, Matrix<Rational>& output)const
{ this->GetAn(n, output);
  if (n<3)
    return;
  output(n-1,n-2)=0;
  output(n-2,n-1)=0;
  output(n-3,n-1)=-1;
  output(n-1,n-3)=-1;
}

void DynkinSimpleType::GetEn(int n, Matrix<Rational>& output)const
{ this->GetAn(n, output);
  if (n<4)
    return;
  output(0,1)=0;
  output(1,0)=0;
  output(1,2)=0;
  output(2,1)=0;
  output(0,2)=-1;
  output(1,3)=-1;
  output(2,0)=-1;
  output(3,1)=-1;
}

void DynkinSimpleType::GetF4(Matrix<Rational>& output)const
{ output.init(4, 4);
  Rational mh(-1,2);
  output(0,0)=2 ;  output(0,1)=-1; output(0,2)=0 ; output(0,3)=0 ;
  output(1,0)=-1;  output(1,1)=2 ; output(1,2)=-1; output(1,3)=0 ;
  output(2,0)=0 ;  output(2,1)=-1; output(2,2)=1 ; output(2,3)=mh;
  output(3,0)=0 ;  output(3,1)=0 ; output(3,2)=mh; output(3,3)=1 ;
}

void DynkinSimpleType::GetG2(Matrix<Rational>& output)const
{ output.init(2, 2);
  output(0,0)=2;
  output(1,1)=6;
  output(1,0)=-3;
  output(0,1)=-3;
}

void DynkinSimpleType::GetCartanSymmetric(Matrix<Rational>& output)const
{ switch(this->theLetter)
  { case 'A': this->GetAn(this->theRank, output); break;
    case 'B': this->GetBn(this->theRank, output); break;
    case 'C': this->GetCn(this->theRank, output); break;
    case 'D': this->GetDn(this->theRank, output); break;
    case 'E': this->GetEn(this->theRank, output); break;
    case 'F': this->GetF4(output);                break;
    case 'G': this->GetG2(output);                break;
    default:
      std::cout << "This is a programming error: requesting DynkinSimpleType::GetCartanSymmetric from a non-initialized Dynkin simple type. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
      break;
  }
  output*=this->lengthFirstCoRootSquared/2;
}

Rational DynkinSimpleType::GetRatioLongRootToFirst
(char inputWeylLetter, int inputRank)
{ Rational result;
  switch (inputWeylLetter)
  { case 'A': return 1;
    case 'B': return 1;
    case 'C': return 2;
    case 'D': return 1;
    case 'E': return 1;
    case 'F': return 1;
    case 'G': return 3;
    default:
      return -1;
  }
}

void DynkinSimpleType::operator++(int)
{ if (this->theRank==1)
  { this->theRank++;
    return;
  }
  if (this->theLetter=='A')
  { if (this->theRank>=4)
      this->theLetter='D';
    else
      this->theLetter='B';
    return;
  }
  if (this->theLetter=='D')
  { this->theLetter='B';
    return;
  }
  if (this->theLetter=='B')
  { if (this->theRank>=3)
      this->theLetter='C';
    else
      this->theLetter='G';
    return;
  }
  if (this->theLetter=='C')
  { if (this->theRank==4)
    { this->theLetter='F';
      return;
    }
    if (this->theRank==6 || this->theRank==7 || this->theRank==8)
    { this->theLetter='E';
      return;
    }
    this->theLetter='A';
    this->theRank++;
    return;
  }
  if (this->theLetter=='G'|| this->theLetter=='F' || this->theLetter=='E')
  { this->theRank++;
    this->theLetter='A';
    return;
  }
  std::cout << "This is a programming error. This is a portion of code that should never be reached. Something has gone very wrong. "
  << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
  assert(false);
}

bool DynkinSimpleType::operator<(int otherRank)const
{ return this->theRank<otherRank;
}

void WeylGroup::SimpleReflectionDualSpace(int index, Vector<Rational>& DualSpaceElement)
{ Rational coefficient, tempRat;
  coefficient.Assign(DualSpaceElement.TheObjects[index]);
  coefficient.DivideBy(this->CartanSymmetric.elements[index][index]);
  for (int i=0; i<this->CartanSymmetric.NumCols; i++)
  { tempRat.Assign(coefficient);
    tempRat.MultiplyBy(this->CartanSymmetric.elements[index][i]*(-2));
    DualSpaceElement.TheObjects[i]+=(tempRat);
  }
}

int WeylGroup::GetRootReflection(int rootIndex)
{ Rational x = this->RootSystem[rootIndex].ScalarProduct(this->rho,this->CartanSymmetric)/this->RootSystem[rootIndex].ScalarProduct(this->RootSystem[rootIndex],this->CartanSymmetric) *-2;
  Vector<Rational> rhoImage = this->rho + this->RootSystem[rootIndex]*x;
  return this->rhoOrbit.GetIndexIMustContainTheObject(rhoImage);
}

void WeylGroup::SimpleReflectionRoot(int index, Vector<Rational>& theRoot, bool RhoAction, bool UseMinusRho)
{// if (this->CartanSymmetric.elements[index][index].IsEqualToZero())
  //  return;
  Rational alphaShift, tempRat;
  alphaShift.MakeZero();
  for (int i=0; i<this->CartanSymmetric.NumCols; i++)
  { tempRat.Assign(theRoot.TheObjects[i]);
    tempRat.MultiplyBy(this->CartanSymmetric.elements[index][i]*(-2));
    alphaShift+=(tempRat);
  }
  if (this->flagAnErrorHasOcurredTimeToPanic)
  { std::string tempS;
    tempS=alphaShift.ToString();
  }
  alphaShift.DivideBy(this->CartanSymmetric.elements[index][index]);
  if (RhoAction)
  { if(UseMinusRho)
      alphaShift.AddInteger(1);
    else
      alphaShift.AddInteger(-1);
  }
  theRoot.TheObjects[index]+=(alphaShift);
}

void WeylGroup::SimpleReflectionRootAlg(int index, PolynomialSubstitution<Rational>& theRoot, bool RhoAction)
{ int lengthA=this->CartanSymmetric.elements[index][index].NumShort;
  Polynomial<Rational>  AscalarB, tempP;
  AscalarB.MakeZero();
  for (int i=0; i<this->CartanSymmetric.NumCols; i++)
  { tempP.MakeZero();
    tempP=theRoot[i];
    tempP*=(CartanSymmetric.elements[index][i]);
    AscalarB+=(tempP);
  }
  AscalarB*=(-2);
  AscalarB/=(lengthA);
  theRoot[index]+=(AscalarB);
  if (RhoAction)
    theRoot[index]+=-1;
}

void WeylGroup::ActOnAffineHyperplaneByGroupElement(int index, affineHyperplane& output, bool RhoAction, bool UseMinusRho)
{ int tempI= this->theElements[index].reflections.size;
  for (int i=0; i<tempI; i++)
  { this->SimpleReflectionRoot(this->theElements[index].reflections[i], output.affinePoint, RhoAction, UseMinusRho);
//    output.affinePoint.ComputeDebugString();
    this->SimpleReflectionDualSpace(this->theElements[index].reflections[tempI-i-1], output.normal);
  }
}

void WeylGroup::GenerateAdditivelyClosedSubset(Vectors<Rational>& input, Vectors<Rational>& output)
{ output=(input);
  Vector<Rational> tempRoot;
  for (int i=0; i<output.size; i++)
    for (int j=i+1; j<output.size; j++)
    { tempRoot=output[i]+output[j];
      if (this->IsARoot(tempRoot))
        output.AddOnTopNoRepetition(tempRoot);
    }
}

int WeylGroup::Multiply(int g, int h) const
{ return this->theElements.GetIndex(this->theElements[g] * this->theElements[h]);
}

int WeylGroup::Invert(int g) const
{ return this->theElements.GetIndex(this->theElements[g].Inverse());
}

void WeylGroup::GetGeneratorList(int g, List<int>& out) const
{ out = this->theElements[g].reflections;
}

void ElementWeylGroup::operator*=(const ElementWeylGroup& other)
{ if (this->owner!=other.owner)
  { std::cout << "This is a programming error: attempting to multiply elements  "
    << " belonging to different Weyl groups. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  List<int> oldMe=this->reflections;
  *this=other;
  this->reflections.AddListOnTop(oldMe);
  this->MakeCanonical();
}

Vector<Rational> ElementWeylGroup::operator*(const Vector<Rational>& v) const
{ Vector<Rational> out = v;
  this->owner->ActOn(*this,out);
  return out;
}


ElementWeylGroup ElementWeylGroup::Inverse() const
{ ElementWeylGroup out = *this;
  out.reflections.ReverseOrderElements();
  out.MakeCanonical();
  return out;
}

bool WeylGroup::operator==(const WeylGroup& other)const
{ return
  this->CartanSymmetric==other.CartanSymmetric &&
  this->theDynkinType==other.theDynkinType;
}

void WeylGroup::ActOnRootByGroupElement(int index, Vector<Rational>& theRoot, bool RhoAction, bool UseMinusRho)
{ for (int i=0; i<this->theElements[index].reflections.size; i++)
    this->SimpleReflectionRoot(this->theElements[index].reflections[i], theRoot, RhoAction, UseMinusRho);
}

void WeylGroup::GenerateRootSystemFromKillingFormMatrix()
{ Vector<Rational> tempRoot;
  Vectors<Rational> startRoots;
  HashedList<Vector<Rational> > tempHashedRootS;
  int theDimension=this->CartanSymmetric.NumCols;
  for (int i=0; i<theDimension; i++)
  { tempRoot.MakeEi(theDimension, i);
    startRoots.AddOnTop(tempRoot);
  }

  int estimatedNumRoots=0;
  switch (this->GetDim())
  { case 8: estimatedNumRoots=240; break;
    case 7: estimatedNumRoots=126; break;
    case 6: estimatedNumRoots=72; break;
    case 4: estimatedNumRoots=48; break;
    default: estimatedNumRoots= this->GetDim()*this->GetDim()*4; break;
  }
  this->GenerateOrbit(startRoots, false, tempHashedRootS, false, estimatedNumRoots);
  this->RootSystem.Clear();
  this->RootsOfBorel.size=0;
  this->RootsOfBorel.ReservE(tempHashedRootS.size/2);
  this->RootSystem.SetExpectedSize(tempHashedRootS.size);
  for (int i=0; i<tempHashedRootS.size; i++)
    if (tempHashedRootS.TheObjects[i].IsPositiveOrZero())
      this->RootsOfBorel.AddOnTop(tempHashedRootS.TheObjects[i]);
  this->RootsOfBorel.QuickSortAscending();
  for (int i=this->RootsOfBorel.size-1; i>=0; i--)
    this->RootSystem.AddOnTop(-this->RootsOfBorel.TheObjects[i]);
  for (int i=0; i<this->RootsOfBorel.size; i++)
    this->RootSystem.AddOnTop(this->RootsOfBorel.TheObjects[i]);
}

void WeylGroup::ActOnRootAlgByGroupElement(int index, PolynomialSubstitution<Rational>& theRoot, bool RhoAction)
{ for (int i=0; i<this->theElements[index].reflections.size; i++)
    this->SimpleReflectionRootAlg(this->theElements[index].reflections[i], theRoot, RhoAction);
}

void WeylGroup::ComputeWeylGroupAndRootsOfBorel(Vectors<Rational>& output)
{ this->ComputeAllElements();
  output.size=0;
  output.ReservE(this->RootSystem.size/2);
  for (int i=0; i<this->RootSystem.size; i++)
    if (this->RootSystem[i].IsPositiveOrZero())
      output.AddOnTop(this->RootSystem[i]);
}

void WeylGroup::ComputeRootsOfBorel(Vectors<Rational>& output)
{ output.size=0;
  this->RootSystem.Clear();
  this->GenerateRootSystemFromKillingFormMatrix();
  output=(this->RootsOfBorel);
}

std::string WeylGroup::ToString(FormatExpressions* theFormat)
{ std::stringstream out;
  out << "<br>Size: " << this->theElements.size << "\n";
//  out <<"Number of Vectors<Rational>: "<<this->RootSystem.size<<"\n
  out << "<br>Half-sum positive roots:" << this->rho.ToString() << "\n";
  if (this->conjugacyClasses.size>0)
  { out << "<br>" << this->conjugacyClasses.size << " conjugacy classes total.\n";
    for (int i=0; i<this->conjugacyClasses.size; i++)
    { out << "<br>Conjugacy class " << i+1 << " (" << this->conjugacyClasses[i].size << " elements total): ";
      if (this->conjugacyClasses[i].size>10)
      { out << " ... has too many elements, displaying the first element only: ";
        out << this->theElements[this->conjugacyClasses[i][0]].ToString(theFormat);
        continue;
      }
      for (int j=0; j<this->conjugacyClasses[i].size; j++)
      { out << this->theElements[this->conjugacyClasses[i][j]].ToString(theFormat);
        if (j!=this->conjugacyClasses[i].size-1)
          out << ", ";
      }
    }
  }
  out << "<br>Root system(" << this->RootSystem.size << " elements):\n" << this->RootSystem.ToString() << "\n";
  out << "<br>Elements of the group(" << this->theElements.size << " total):\n ";
  if (this->theElements.size<=100)
    for (int i=0; i<this->theElements.size; i++)
      out << i << ". " << this->theElements[i].ToString() << "\n";
  else
    out << "... too many, not displaying. ";
  out << "<br>Symmetric cartan: " << this->CartanSymmetric.ToString();
  return out.str();
}

bool WeylGroup::IsAddmisibleDynkinType(char candidateLetter, int n)
{ if (candidateLetter=='A' && n>0)
    return true;
  if (candidateLetter=='B' && n>1)
    return true;
  if (candidateLetter=='C' && n>1)
    return true;
  if (candidateLetter=='D' && n>3)
    return true;
  if (candidateLetter=='E' && n>5 && n<9)
    return true;
  if (candidateLetter=='F' && n==4)
    return true;
  if (candidateLetter=='G' && n==2)
    return true;
  return false;
}

void WeylGroup::TransformToAdmissibleDynkinType(char inputLetter, int& outputRank)
{ if (inputLetter=='G')
    outputRank=2;
  if (inputLetter=='F')
    outputRank=4;
  if (inputLetter=='E')
  { if (outputRank>8)
      outputRank=8;
    if(outputRank<6)
      outputRank=6;
  }
  if (inputLetter=='C' || inputLetter=='B')
    if (outputRank<2)
      outputRank=2;
  if (inputLetter=='D')
    if (outputRank<4)
      outputRank=4;
}

void WeylGroup::MakeArbitrarySimple(char WeylGroupLetter, int n, const Rational* firstCoRootLengthSquared)
{ this->init();
  this->theDynkinType.MakeSimpleType(WeylGroupLetter, n, firstCoRootLengthSquared);
  this->theDynkinType.GetCartanSymmetric(this->CartanSymmetric);
}

void WeylGroup::ComputeExternalAutos()
{ if (!this->flagOuterAutosComputed)
    this->theDynkinType.GetOuterAutosGenerators(this->OuterAutomorphisms);
  this->flagOuterAutosComputed=true;
}

void WeylGroup::MakeFromDynkinType(const DynkinType& inputType)
{ this->init();
  this->theDynkinType=inputType;
  this->theDynkinType.GetCartanSymmetric(this->CartanSymmetric);
}

void WeylGroup::GetEpsilonCoordsWRTsubalgebra(Vectors<Rational>& generators, List<Vector<Rational> >& input, Vectors<Rational>& output)
{ Matrix<Rational> basisChange;
  Matrix<Rational> tempMat;
  DynkinDiagramRootSubalgebra tempDyn;
  Vectors<Rational> simpleBasis;
  Vectors<Rational> coordsInNewBasis;
  simpleBasis=(generators);
  tempDyn.ComputeDiagramTypeModifyInput(simpleBasis, *this);
  bool tempBool = true;
  if (generators.size==0)
    tempBool = false;
  if (!tempBool)
  { output.SetSize(input.size);
    for(int i=0; i<input.size; i++)
      output[i].MakeZero(0);
    return;
  }
  basisChange.Resize(0, 0, true);
  for (int i=0; i<tempDyn.SimpleComponentTypes.size; i++)
  { DynkinSimpleType::GetEpsilonMatrix
    (tempDyn.SimpleComponentTypes[i].theLetter, tempDyn.SimpleComponentTypes[i].theRank, tempMat);
    basisChange.DirectSumWith(tempMat, (Rational) 0);
    //basisChange.ComputeDebugString();
  }
  simpleBasis.AssignListList(tempDyn.SimpleBasesConnectedComponents);
//  std::cout << "<br>simple basis: " << simpleBasis.ToString();
//  std::cout << "<br>to be converted: " << input.ToString();
  coordsInNewBasis.SetSize(input.size);
  for (int i=0; i<input.size; i++)
    input[i].GetCoordsInBasiS(simpleBasis, coordsInNewBasis[i]);
  //basisChange.ComputeDebugString();
  //coordsInNewBasis.ComputeDebugString();
  basisChange.ActOnVectorsColumn(coordsInNewBasis, output);
//  output.ComputeDebugString();
}

void WeylGroup::GetEpsilonCoords(const List<Vector<Rational> >& input, Vectors<Rational>& output)
{ if (&input==&output)
  { List<Vector<Rational> > newInput=input;
    this->GetEpsilonCoords(newInput, output);
    return;
  }
  output.SetSize(input.size);
  for (int i=0; i<input.size; i++)
    this->GetEpsilonCoords(input[i], output[i]);
}

void WeylGroup::GetEpsilonCoords(const Vector<Rational>& input, Vector<Rational>& output)
{ if (this->MatrixSendsSimpleVectorsToEpsilonVectors.IsZeroPointer())
    this->theDynkinType.GetEpsilonMatrix
    (this->MatrixSendsSimpleVectorsToEpsilonVectors.GetElement());
  output=input;
  this->MatrixSendsSimpleVectorsToEpsilonVectors.GetElement().ActOnVectorColumn(output);
}

bool WeylGroup::ContainsARootNonStronglyPerpendicularTo(Vectors<Rational>& theVectors, Vector<Rational>& input)
{ for (int i=0; i<this->theElements.size; i++)
    if (this->IsARoot(theVectors[i]+input))
      return true;
  return false;
}

int WeylGroup::NumRootsConnectedTo(Vectors<Rational>& theVectors, Vector<Rational>& input)
{ int result=0;
  for (int i=0; i<this->theElements.size; i++)
    if (!Vector<Rational>::ScalarProduct(theVectors[i], input, this->CartanSymmetric).IsEqualToZero())
      result++;
  return result;
}

void WeylGroup::ComputeAllElements(int UpperLimitNumElements)
{ this->ComputeRho(true);
//  this->ComputeDebugString();
  Vectors<Rational> tempRoots;
  tempRoots.AddOnTop(this->rho);
  this->theElements.Clear();
  this->GenerateOrbit<Rational>(tempRoots, false, this->rhoOrbit, true, -1, &this->theElements, UpperLimitNumElements);
  this->size = this->theElements.size; // compatible with group types that don't have a theElements
}

void WeylGroup::ComputeRho(bool Recompute)
{ if (this->RootSystem.size==0 || Recompute)
    this->GenerateRootSystemFromKillingFormMatrix();
  //this->ComputeDebugString();
  this->rho.MakeZero(this->CartanSymmetric.NumRows);
  for (int i=0; i<this->RootSystem.size; i++)
    if (this->RootSystem[i].IsPositiveOrZero() )
      this->rho+=(RootSystem[i]);
  for (int i=0; i<this->CartanSymmetric.NumCols; i++)
    this->rho[i].DivideByInteger(2);
//  this->lengthLongestRootSquared=this->GetLongestRootLengthSquared();
  this->flagFundamentalToSimpleMatricesAreComputed=false;
}

void ReflectionSubgroupWeylGroup::operator=(const ReflectionSubgroupWeylGroup& other)
{ this->::HashedList<ElementWeylGroup>::operator=(other);
  this->simpleGenerators=(other.simpleGenerators);
  this->ExternalAutomorphisms=(other.ExternalAutomorphisms);
  this->AmbientWeyl=(other.AmbientWeyl);
  this->RootSubsystem=other.RootSubsystem;
  this->RootsOfBorel=other.RootsOfBorel;
}

void ReflectionSubgroupWeylGroup::ComputeRootSubsystem()
{ MacroRegisterFunctionWithName("ReflectionSubgroupWeylGroup::ComputeRootSubsystem");
  this->RootSubsystem.Clear();
  this->RootSubsystem.AddOnTop(this->simpleGenerators);
  this->RootSubsystem.SetExpectedSize(100);
  Vector<Rational> currentRoot;
  for (int i=0; i<this->RootSubsystem.size; i++)
    for (int j=0; j<this->simpleGenerators.size; j++)
    { currentRoot=(this->RootSubsystem[i]);
      this->AmbientWeyl.ReflectBetaWRTAlpha
      (this->simpleGenerators[j], currentRoot, false, currentRoot);
      this->RootSubsystem.AddOnTopNoRepetition(currentRoot);
    }
  Vectors<Rational> tempRoots;
  tempRoots=(this->RootSubsystem);
  tempRoots.QuickSortAscending();
  this->RootSubsystem=(tempRoots);
  if (this->RootSubsystem.size%2!=0)
  { std::cout << "This is a programming error. I am getting that the number of weights of a root system is odd. The generating set of simple weights is "
    << this->simpleGenerators.ToString() << ", and the generated weight subsystem is " << tempRoots.ToString()
    << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
    assert(false);
  }
  int numPosRoots=this->RootSubsystem.size/2;
  this->RootsOfBorel.SetSize(numPosRoots);
  for (int i=0; i<numPosRoots; i++)
    this->RootsOfBorel[i]=this->RootSubsystem[i+numPosRoots];
}

unsigned int ElementWeylGroup::HashFunction() const
{ int top = MathRoutines::Minimum(this->reflections.size, ::SomeRandomPrimesSize);
  unsigned int result = this->owner==0 ? 0 : this->owner->HashFunction();
  for (int i=0; i<top; i++)
    result+=this->reflections[i]*::SomeRandomPrimes[i];
  return result;
}

void KLpolys::WriteKLCoeffsToFile(std::fstream& output, List<int>& KLcoeff, int TopIndex)
{ output.clear();
  output << "Top_index: " << TopIndex << "\n";
  std::string tempS;
  this->KLcoeffsToString(KLcoeff,  tempS);
  output << tempS;
}

int KLpolys::ReadKLCoeffsFromFile(std::fstream& input, List<int>& output)
{ std::string tempS;
  int TopIndex;
  input >> tempS >> TopIndex;
  output.SetSize(this->size);
  for (int i=0; i<this->size; i++)
    input >> tempS >> output[i];
  return TopIndex;
}

void KLpolys::KLcoeffsToString(List<int>& theKLCoeffs, std::string& output)
{ std::stringstream out;
  for (int i=0; i<theKLCoeffs.size; i++)
    out << i << ".  " << theKLCoeffs[i] << "\n";
  output=out.str();
}

void KLpolys::initTheMults()
{ this->TheMultiplicities.SetSize(this->size);
  this->Explored.SetSize(this->size);
  for (int i=0; i<this->size; i++)
  { this->TheMultiplicities[i]=0;
    this->Explored[i]=false;
  }
  this->NextToExplore=0;
  this->LowestNonExplored=0;
}

void KLpolys::Check()
{ for (int i=0; i<this->size; i++)
  { this->Compute(i);
//    bool found=false;
    for (int j=0; j<this->size; j++)
    { if (this->TheMultiplicities[i]!=0 && this->TheMultiplicities[i]!=1 && this->TheMultiplicities[i]!=-1)
      { this->ComputeDebugString();
      }
    }
  }
}

void KLpolys::Compute(int x)
{ this->initTheMults();
  this->TheMultiplicities[x]=1;
  while (this->NextToExplore!=-1)
  { for (int i=0; i<this->BruhatOrder[this->NextToExplore].size; i++)
    { int a=this->BruhatOrder[this->NextToExplore][i];
      this->TheMultiplicities[a]-=this->TheMultiplicities[this->NextToExplore];
    }
    this->Explored[this->NextToExplore]=true;
    this->FindNextToExplore();
  //  this->ComputeDebugString();
  }
}

void KLpolys::FindNextToExplore()
{ bool foundNonExplored=false;
  for (int i=this->LowestNonExplored; i<this->size; i++)
    if (!this->Explored[i])
    { if (!foundNonExplored)
      { this->LowestNonExplored=i;
        foundNonExplored=true;
      }
      if (this->IsMaxNonEplored(i))
      { this->NextToExplore=i;
        return;
      }
    }
  this->NextToExplore=-1;
}

bool KLpolys::IsMaxNonEplored(int index)
{ for (int i=this->LowestNonExplored; i<this->size; i++)
    if (!this->Explored[i]&& i!=index)
    { Vector<Rational> tempRoot;
      tempRoot=(*this)[i];
      tempRoot-=(*this)[index];
      if (tempRoot.IsPositiveOrZero())
        return false;
    }
  return true;
}

std::string KLpolys::ToString(FormatExpressions* theFormat)
{ MacroRegisterFunctionWithName("KLpolys::ToString");
  std::stringstream out;
  bool useHtml=false;
  if (theFormat!=0)
    useHtml=theFormat->flagUseHTML;
  if (!useHtml)
  { out << "Next to explore: " << this->NextToExplore << "<br>\n Orbit of rho:<br>\n";
    for (int i=0; i<this->size; i++)
    { out << this->TheObjects[i].ToString() << "   :  " << this->TheMultiplicities[i];
      if (this->Explored[i])
        out << " Explored<br>\n";
      else
        out << " not Explored<br>\n";
    }
    out << "Bruhat order:<br>\n";
    for (int i=0; i<this->size; i++)
    { out << i << ".   ";
      for(int j=0; j<this->BruhatOrder[i].size; j++)
        out << this->BruhatOrder[i][j] << ", ";
      out << "<br>\n";
    }
  }
  out << "R Polynomials:<br>" << this->RPolysToString(theFormat);
  if (this->theKLcoeffs.size==this->TheWeylGroup->theElements.size)
  { out << "Kazhdan-Lusztig Polynomials:<br>" << this->KLPolysToString(theFormat);
    out << "Kazhdan-Lusztig coefficients; the (w_1,w_2)  coefficient is defined as the multiplicity of " << CGI::GetHtmlMathSpanPure("L_{w_2 \\cdot \\lambda}")
    << " in " <<  CGI::GetHtmlMathSpanPure(" M_{w_1\\cdot \\lambda }  ") << " where \\cdot stands for the \\rho-modified action"
    << " of the Weyl group, \\lambda is a dominant integral weight, M_{\\lambda} stands for Verma module "
    << "of highest weight \\lambda, L_\\lambda stands for irreducible highest weight of highest weight \\lambda: "
    << "<br><table border=\"1\"><tr><td>Weyl elt.</td>";
    for (int i=0; i<this->TheWeylGroup->theElements.size; i++)
      out << "<td>" << this->TheWeylGroup->theElements[i].ToString() << "</td>";
    out << "</tr>";
    for (int i=0; i<this->TheWeylGroup->theElements.size; i++)
      if (this->theKLPolys[i].size>0)
      { out << "<tr>" << "<td>" << this->TheWeylGroup->theElements[i].ToString()  << "</td>";
        for (int j=0; j<this->theKLcoeffs[i].size; j++)
          out << "<td>" << theKLcoeffs[i][j].ToString() << "</td>";
        out << "</tr>";
      }
    out << "</table>";
  } else
    out << "KL polys not computed";
  return out.str();
}

void KLpolys::ComputeDebugString()
{ this->DebugString=this->ToString();
}

void KLpolys::GeneratePartialBruhatOrder()
{ MacroRegisterFunctionWithName("KLpolys::GeneratePartialBruhatOrder");
  int theDimension= this->TheWeylGroup->CartanSymmetric.NumRows;
  Vector<Rational> ZeroRoot;
  ZeroRoot.MakeZero(theDimension);
  this->BruhatOrder.SetSize(this->size);
  this->InverseBruhatOrder.SetSize(this->size);
  this->SimpleReflectionsActionList.SetSize(this->size);
  this->ComputeDebugString();
  for (int i=0; i<this->size; i++)
  { this->SimpleReflectionsActionList[i].ReservE(theDimension);
    for (int j=0; j<theDimension; j++)
    { Vector<Rational> tempRoot, tempRoot2;
      tempRoot=(*this)[i];
      tempRoot2=(*this)[i];
      this->TheWeylGroup->SimpleReflectionRoot(j, tempRoot, false, false);
      int x= this->GetIndex(tempRoot);
      if (x==-1)
      { std::cout << "This is a programming error: something wrong has happened. A weight that is supposed to "
        << " be in a certain Weyl group orbit isn't there. There is an error in the code, crashing accordingly. "
        << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
        assert(false);
      }
      this->SimpleReflectionsActionList[i].AddOnTop(x);
      tempRoot2-=(tempRoot);
      if (tempRoot2.IsPositiveOrZero() && !tempRoot2.IsEqualToZero() )
      { this->BruhatOrder[i].AddOnTop(x);
        this->InverseBruhatOrder[x].AddOnTop(i);
      }
    }
  }
  this->ComputeDebugString();
}

int KLpolys::FindMinimalBruhatNonExplored(List<bool>& theExplored)
{ int lowestIndex=-1;
  for (int i=0; i<this->size; i++)
    if (!theExplored[i])
    { if (lowestIndex==-1)
        lowestIndex=i;
      else
        if (this->IndexGreaterThanIndex(lowestIndex, i))
          lowestIndex=i;
    }
  return lowestIndex;
}

int KLpolys::FindMaximalBruhatNonExplored(List<bool>& theExplored)
{ int highestIndex=-1;
  for (int i=0; i<this->size; i++)
    if (!theExplored[i])
    { if (highestIndex==-1)
        highestIndex=i;
      else
        if (this->IndexGreaterThanIndex(i, highestIndex))
          highestIndex=i;
    }
  return highestIndex;
}

void KLpolys::MergeBruhatLists(int fromList, int toList)
{ for (int i=0; i<this->BruhatOrder[fromList].size; i++)
  { bool found=false;
    for (int j=0; j<this->BruhatOrder[toList].size; j++)
      if (this->BruhatOrder[toList][j]==this->BruhatOrder[fromList][i])
      { found = true;
        break;
      }
    if (!found)
      this->BruhatOrder[toList].AddOnTop(this->BruhatOrder[fromList][i]);
  }
}

int KLpolys::ChamberIndicatorToIndex(Vector<Rational>& ChamberIndicator)
{ int theDimension= this->TheWeylGroup->CartanSymmetric.NumRows;
  Vector<Rational> tempRoot; tempRoot.SetSize(theDimension);
  Vector<Rational> ChamberIndicatorPlusRho;
  ChamberIndicatorPlusRho=(ChamberIndicator);
  ChamberIndicatorPlusRho+=(this->TheWeylGroup->rho);
  for (int i=0; i<this->size; i++)
  { Rational tempRat1, tempRat2;
    bool tempBool1, tempBool2;
    bool haveSameSigns=true;
    for (int j=0; j<this->TheWeylGroup->RootSystem.size; j++)
    { this->TheWeylGroup->RootScalarCartanRoot(ChamberIndicatorPlusRho, this->TheWeylGroup->RootSystem[j], tempRat1);
      tempRoot=(*this)[i];
      tempRoot+=(this->TheWeylGroup->rho);
      this->TheWeylGroup->RootScalarCartanRoot(tempRoot, this->TheWeylGroup->RootSystem[j], tempRat2);
      tempBool1=tempRat1.IsPositive();
      tempBool2=tempRat2.IsPositive();
      assert(!tempRat1.IsEqualToZero());
      assert(!tempRat2.IsEqualToZero());
      if (tempBool1!=tempBool2)
      { haveSameSigns=false;
        break;
      }
    }
    if (haveSameSigns)
      return i;
  }
  return -1;
}

void KLpolys::ComputeKLcoefficients()
{ MacroRegisterFunctionWithName("KLpolys::ComputeKLcoefficients");
  this->theKLcoeffs.SetSize(this->theKLPolys.size);
  for (int i=0; i<this->theKLPolys.size; i++ )
  { this->theKLcoeffs[i].SetSize(this->theKLPolys[i].size);
    for (int j=0; j<this->theKLcoeffs[i].size; j++)
    { Polynomial<Rational>& currentPoly=this->theKLPolys[i][j];
      this->theKLcoeffs[i][j]=0;
      if (this->IndexGEQIndex(j,i))
        for (int k=0; k<currentPoly.size(); k++)
          this->theKLcoeffs[i][j]+=currentPoly.theCoeffs[k];
    }
  }
}

void KLpolys::initFromWeyl(WeylGroup* theWeylGroup)
{ this->TheWeylGroup= theWeylGroup;
  Vectors<Rational> tempRoots;
  this->TheWeylGroup->ComputeRho(true);
  tempRoots.AddOnTop(this->TheWeylGroup->rho);
  this->TheWeylGroup->GenerateOrbit(tempRoots, false, *this, false);
  this->initTheMults();
}

bool KLpolys::ComputeKLPolys(WeylGroup* theWeylGroup)
{ MacroRegisterFunctionWithName("KLpolys::ComputeKLPolys");
  theWeylGroup->ComputeAllElements(-1);
  this->initFromWeyl(theWeylGroup);
  this->GeneratePartialBruhatOrder();
  FormatExpressions PolyFormatLocal;
  PolyFormatLocal.polyDefaultLetter="q";
  this->ComputeRPolys();
  this->theKLPolys.SetSize(this->size);
  this->theKLcoeffs.SetSize(this->size);
  this->Explored.initFillInObject(this->size, false);
  for (int i=0; i<this->theKLPolys.size; i++)
  { this->theKLPolys[i].SetSize(this->size);
    this->theKLcoeffs[i].SetSize(this->size);
  }
  for (int i=0; i<this->size; i++)
  { this->Explored.initFillInObject(this->size, false);
    int highestNonExplored= this->FindMaximalBruhatNonExplored(this->Explored);
    while(highestNonExplored!=-1)
    { this->ComputeKLxy(highestNonExplored, i);
      this->Explored[highestNonExplored]=true;
      highestNonExplored= this->FindMaximalBruhatNonExplored(this->Explored);
    }
  }
  this->ComputeKLcoefficients();
  return true;
}

void KLpolys::ComputeRPolys()
{ MacroRegisterFunctionWithName("KLpolys::ComputeRPolys");
  int theDimension= this->TheWeylGroup->GetDim();
  this->theRPolys.SetSize(this->size);
  for (int i=0; i<this->size; i++)
  { this->Explored[i]=false;
    this->theRPolys[i].SetSize(this->size);
  }
  this->LowestNonExplored=this->FindMinimalBruhatNonExplored(this->Explored);
  List<bool> ExploredFromTop;
  ExploredFromTop.SetSize(this->size);
  while(this->LowestNonExplored!=-1)
  { for (int i=0; i<this->size; i++)
      ExploredFromTop[i]=false;
    int a= this->FindMaximalBruhatNonExplored(ExploredFromTop);
    while (a!=-1)
    { bool tempBool = false;
      for (int j=0; j<theDimension; j++)
        if (this->ComputeRxy(a, this->LowestNonExplored, j))
        { tempBool =true;
          break;
        }
      if (!tempBool)
      { std::cout
        << "This is a programming error: an algorithmic check failed while computing R-polynomials. " << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
        assert(false);
      }
      ExploredFromTop[a]=true;
      a= this->FindMaximalBruhatNonExplored(ExploredFromTop);
    }
    this->Explored[this->LowestNonExplored]=true;
    this->LowestNonExplored= this->FindMinimalBruhatNonExplored(this->Explored);
  }
  //this->ComputeDebugString();
}

bool KLpolys::IndexGEQIndex(int a, int b)
{ /*ElementWeylGroup& eltA= (*this->TheWeylGroup)[a];
  ElementWeylGroup& eltB= (*this->TheWeylGroup)[b];
  int currentPointerInB=0;
  for (int currentPointerInA=0; currentPointerInA<eltA.size && currentPointerInB<eltB.size; currentPointerInA++)
    if (eltA[currentPointerInA]==eltB[currentPointerInB])
      currentPointerInB++;
  return currentPointerInB==eltB.size;*/
  Vector<Rational> tempV;
  tempV=(*this)[a];
  tempV-=(*this)[b];
  return tempV.IsNegativeOrZero();
}

bool KLpolys::IndexGreaterThanIndex(int a, int b)
{ if (a==b)
    return false;
  return this->IndexGEQIndex(a, b);
}

int KLpolys::ComputeProductfromSimpleReflectionsActionList(int x, int y)
{ int start = y;
  for (int i=0; i<this->TheWeylGroup->theElements[x].reflections.size; i++)
    start=this->SimpleReflectionsActionList[start][this->TheWeylGroup->theElements[x].reflections[i]];
  return start;
}

void KLpolys::ComputeKLxy(int x, int y)
{ Polynomial<Rational> Accum, tempP1, tempP2;
  if (x==y)
  { this->theKLPolys[x][y].MakeOne(1);
    return;
  }
  if (!this->IndexGEQIndex(y, x))
  { this->theKLPolys[x][y].MakeZero();
    return;
  }
//  std::cout << " <br>Computing KL " << x << ", " << y << "; ";
  Accum.MakeZero();
  MonomialP tempM;
  for (int i=0; i<this->size; i++)
    if (this->IndexGreaterThanIndex(i, x) && this->IndexGEQIndex(y, i))
    { tempP1.MakeZero();
      for (int j=0; j<this->theRPolys[x][i].size(); j++)
      { tempM=this->theRPolys[x][i][j];
        tempM.Invert();
        tempP1.AddMonomial(tempM, this->theRPolys[x][i].theCoeffs[j]);
      }
      int tempI;
      if ((this->TheWeylGroup->theElements[x].reflections.size+this->TheWeylGroup->theElements[i].reflections.size)%2==0)
        tempI=1;
      else
        tempI=-1;
      Rational powerQ= -this->TheWeylGroup->theElements[x].reflections.size+2*this->TheWeylGroup->theElements[i].reflections.size -
      this->TheWeylGroup->theElements[y].reflections.size;
      powerQ/=2;
      tempP2.MakeMonomiaL(0, powerQ, tempI, 1);
      tempP1*=tempP2;
      tempP1*=(this->theKLPolys[i][y]);
      if (!this->Explored[i])
      { std::cout << "This is a programming error: an internal check during the Kazhdan-Lusztig polynomial computation fails. More precisely, while computing "
        << "KL poly of indices " << x << ", " << y << " I am using KL poly with indices " << i << ", " << y << " which hasn't been computed yet. "
        << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
        assert(false);
      }
      Accum+=tempP1;
    }
  this->theKLPolys[x][y].MakeZero();
  Rational lengthDiff= this->TheWeylGroup->theElements[y].reflections.size-this->TheWeylGroup->theElements[x].reflections.size;
  lengthDiff/=2;
//  std::cout << "Accum: " << Accum.ToString();
  for (int i=0; i<Accum.size(); i++)
    if(Accum[i].HasPositiveOrZeroExponents())
    { tempM=Accum[i];
      tempM[0].Minus();
      tempM[0]+=lengthDiff;
      this->theKLPolys[x][y].AddMonomial(tempM, Accum.theCoeffs[i]);
    }

//  this->theKLPolys.TheObjects[x].TheObjects[w].ComputeDebugString();
}

bool KLpolys::ComputeRxy(int x, int y, int SimpleReflectionIndex)
{ MacroRegisterFunctionWithName("KLpolys::ComputeRxy");
  if (x==y)
  { this->theRPolys[x][y].MakeOne(1);
    return true;
  }
  if (this->IndexGreaterThanIndex(x, y))
  { this->theRPolys[x][y].MakeZero();
    return true;
  }
  int sx= this->SimpleReflectionsActionList[x][SimpleReflectionIndex];
  int sy= this->SimpleReflectionsActionList[y][SimpleReflectionIndex];
  bool boolX, boolY;
  boolX=this->IndexGreaterThanIndex(x, sx);
  boolY=this->IndexGreaterThanIndex(y, sy);
  if (boolX && boolY)
  { if (!this->Explored[sy])
    { std::cout << "This is a programming error: the computaion of R-polynomials is attempting to use a non-computed R-polynomial. "
      << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
    this->theRPolys[x][y]=this->theRPolys[sx][sy];
    return true;
  }
  if (!boolX && boolY)
  { Polynomial<Rational> qMinus1;
    qMinus1.MakeMonomiaL(0, 1, 1, 1);
    this->theRPolys[x][y]=qMinus1;
    this->theRPolys[x][y]*=(this->theRPolys[sx][sy]);
    qMinus1-=1;
    qMinus1*=(this->theRPolys[sx][y]);
    this->theRPolys[x][y]+=qMinus1;
    return true;
  }
  return false;
}

std::string KLpolys::KLPolysToString(FormatExpressions* theFormat)
{ std::stringstream out;
  out << "<table border=\"1\">";
  out << "<tr><td>Weyl elt.</td>";
  for (int i=0; i<this->TheWeylGroup->theElements.size; i++)
    out << "<td>" << this->TheWeylGroup->theElements[i].ToString() << "</td>";
  out << "</tr>";
  for (int i=0; i<this->theKLPolys.size; i++)
    if (this->theKLPolys[i].size>0)
    { out << "<tr><td>" << this->TheWeylGroup->theElements[i].ToString() << "</td>";
      for (int j=0; j<this->theKLPolys[i].size; j++)
        out << "<td>" << this->theKLPolys[i][j].ToString(theFormat) << "</td>";
      out << "</tr>";
    }
  out << "</table>";
  return out.str();
}

std::string KLpolys::RPolysToString(FormatExpressions* theFormat)
{ std::stringstream out;
  out << "<table border=\"1\"><tr><td>Weyl elt.</td>";
  for (int i=0; i<this->TheWeylGroup->theElements.size; i++)
    out << "<td>" << this->TheWeylGroup->theElements[i].ToString() << "</td>";
  out << "</tr>";
  for (int i=0; i<this->theRPolys.size; i++)
  { out << "<tr><td>" << this->TheWeylGroup->theElements[i].ToString() << "</td>";
    for (int j=0; j<this->theRPolys[i].size; j++)
      out << "<td>" << this->theRPolys[i][j].ToString(theFormat) << "</td>\n";
    out << "</tr>";
  }
  out << "</table>";
  return out.str();
}

LargeInt PartFraction::EvaluateIntPolyAtOne(Polynomial<LargeInt>& input)
{ LargeInt result;
  result.MakeZero();
  for(int i=0; i<input.size(); i++)
    result+=input.theCoeffs[i];
  return result;
}

void PartFraction::EvaluateIntPoly(const Polynomial<LargeInt>& input, const Vector<Rational>& values, Rational& output)
{ std::string tempS1, tempS2;
  output.MakeZero();
  //if(this->flagAnErrorHasOccurredTimeToPanic)
  //{ output.ToString(tempS1);
  //}
  Polynomial<Rational> tempInput;
  tempInput=input; //<-implicit type conversion here!
  output=tempInput.Evaluate(values);
}

void LaTeXProcedures::beginPSTricks(std::fstream& output)
{ output << "\\begin{pspicture}(8, 8)";
}

void LaTeXProcedures::endPSTricks(std::fstream& output)
{ output << "\\end{pspicture}\\medskip \\\\ \\noindent ";
}

void LaTeXProcedures::beginDocument(std::fstream& output)
{ output << "\\documentclass{article}\n \\usepackage{pstricks} \\begin{document}";
}

void LaTeXProcedures::endLatexDocument(std::fstream& output)
{ output << "\\end{document}";
}

void LaTeXProcedures::GetStringFromColorIndex(int ColorIndex, std::string &output, DrawingVariables& drawInput)
{ switch(ColorIndex)
  { case 0: output.assign("black"); break;
    case 1: output.assign("blue"); break;
    case 2: output.assign("purple"); break;
    case 3: output.assign("green"); break;
    case 4: output.assign("cyan"); break;
    case 5: output.assign("red"); break;
    case 6: output.assign("purple"); break;
    case 7: output.assign("cyan"); break;
    case 56540: output.assign("yellow"); break;
    default:
      output.assign("black");
      break;
  }
  if(ColorIndex==drawInput.GetColorFromChamberIndex(1,0))
    output.assign("blue");
  if(ColorIndex==drawInput.GetColorFromChamberIndex(2,0))
    output.assign("purple");
  if(ColorIndex==drawInput.GetColorFromChamberIndex(3,0))
    output.assign("green");
  if(ColorIndex==drawInput.GetColorFromChamberIndex(4,0))
    output.assign("cyan");
  if(ColorIndex==drawInput.GetColorFromChamberIndex(5,0))
    output.assign("red");
  if(ColorIndex==drawInput.GetColorFromChamberIndex(6,0))
    output.assign("purple");
  if(ColorIndex==drawInput.GetColorFromChamberIndex(7,0))
    output.assign("cyan");
}

void LaTeXProcedures::drawTextDirectly(double X1, double Y1, const std::string& theText, int ColorIndex, std::fstream& output)
{ output.precision(4);
  X1-=theText.length()* LaTeXProcedures::TextPrintCenteringAdjustmentX;
  Y1+=LaTeXProcedures::TextPrintCenteringAdjustmentY;
  X1/=LaTeXProcedures::ScaleFactor; Y1/=LaTeXProcedures::ScaleFactor;
  output << "\\put(" << X1-LaTeXProcedures::FigureCenterCoordSystemX << ", " << LaTeXProcedures::FigureCenterCoordSystemY-Y1 << "){\\tiny{" << theText << "}}";
}

void LaTeXProcedures::drawline(double X1, double Y1, double X2, double Y2, unsigned long thePenStyle, int ColorIndex, std::fstream& output, DrawingVariables& drawInput)
{ if ((int)thePenStyle== DrawingVariables::PenStyleInvisible)
    return;
  output.precision(4);
  X1/=LaTeXProcedures::ScaleFactor; X2/=LaTeXProcedures::ScaleFactor;
  Y1/=LaTeXProcedures::ScaleFactor; Y2/=LaTeXProcedures::ScaleFactor;
  std::string tempS;
  if (thePenStyle==(unsigned)DrawingVariables::PenStyleDashed)
    tempS="lightgray";
  else
    LaTeXProcedures::GetStringFromColorIndex(ColorIndex, tempS, drawInput);
  output << "\\psline[linewidth=0.3pt, linecolor=" << tempS << "](" << X1-LaTeXProcedures::FigureCenterCoordSystemX << ", " << LaTeXProcedures::FigureCenterCoordSystemY-Y1 << ")" << "(" << X2-LaTeXProcedures::FigureCenterCoordSystemX << ", " << LaTeXProcedures::FigureCenterCoordSystemY-Y2 << ")\n";
}

void rootSubalgebra::ComputeDynkinDiagramKandCentralizer()
{ this->SimpleBasisK=(this->genK);
  this->theDynkinDiagram.ComputeDiagramTypeModifyInput(this->SimpleBasisK, this->GetAmbientWeyl());
  this->SimpleBasisCentralizerRoots.size=0;
  for (int i=0; i<this->GetAmbientWeyl().RootsOfBorel.size; i++)
    if (this->rootIsInCentralizer(this->GetAmbientWeyl().RootsOfBorel[i]))
      this->SimpleBasisCentralizerRoots.AddOnTop(this->GetAmbientWeyl().RootsOfBorel[i]);
  this->theCentralizerDiagram.ComputeDiagramTypeModifyInput(this->SimpleBasisCentralizerRoots, this->GetAmbientWeyl());
}

void rootSubalgebra::ComputeAllButAmbientWeyl()
{ this->PosRootsKConnectedComponents.size=0;
  this->theKComponentRanks.size=0;
  this->theKEnumerations.size=0;
  this->SimpleBasisK=(this->genK);
  this->GetAmbientWeyl().TransformToSimpleBasisGenerators(this->SimpleBasisK, this->GetAmbientWeyl().RootSystem);
  this->ComputeKModules();
  this->ComputeCentralizerFromKModulesAndSortKModules();
  this->NilradicalKmods.init(this->kModules.size);
  this->theDynkinDiagram.ComputeDiagramTypeModifyInput(this->SimpleBasisK, this->GetAmbientWeyl());
  this->theCentralizerDiagram.ComputeDiagramTypeModifyInput(this->SimpleBasisCentralizerRoots, this->GetAmbientWeyl());
}

void rootSubalgebra::ComputeAll()
{ this->ComputeAllButAmbientWeyl();
  //this->ComputeDebugString();
}

void rootSubalgebra::ComputeCentralizerFromKModulesAndSortKModules()
{ this->CentralizerKmods.init(this->kModules.size);
  this->CentralizerRoots.size=0;
  this->CentralizerRoots.ReservE(this->kModules.size);
  this->SimpleBasisCentralizerRoots.size=0;
  this->SimpleBasisCentralizerRoots.ReservE(this->kModules.size);
  int counter=0;
  for (int i=0; i<this->kModules.size; i++)
    if (this->kModules[i].size==1)
    { this->kModules.SwapTwoIndices(counter, i);
      this->HighestWeightsGmodK.SwapTwoIndices(counter, i);
      this->LowestWeightsGmodK.SwapTwoIndices(counter, i);
      this->CentralizerKmods.AddSelectionAppendNewIndex(counter);
      this->CentralizerRoots.AddOnTop(this->kModules[counter][0]);
      this->SimpleBasisCentralizerRoots.AddOnTop(this->kModules[counter][0]);
      counter++;
    }
  this->GetAmbientWeyl().TransformToSimpleBasisGenerators(this->SimpleBasisCentralizerRoots, this->GetAmbientWeyl().RootSystem);
}

void rootSubalgebra::init(SemisimpleLieAlgebra& inputOwner)
{ this->owneR=&inputOwner;
}

void WeylGroup::TransformToSimpleBasisGenerators(Vectors<Rational>& theGens, const HashedList<Vector<Rational> >& inputRootSystem)
{ for (int i=0; i<theGens.size; i++)
    if (!theGens[i].IsPositiveOrZero())
      theGens[i].Minus();
  bool reductionOccured=true;
  Vector<Rational> tempRoot;
  while (reductionOccured)
  { reductionOccured= false;
    for (int i=0; i<theGens.size; i++)
      for (int j=i+1; j<theGens.size; j++)
      { tempRoot=(theGens[i]);
        tempRoot-=(theGens[j]);
        if (tempRoot.IsEqualToZero())
        { theGens.RemoveIndexSwapWithLast(j);
          reductionOccured=true;
        }
        if (inputRootSystem.Contains(tempRoot))
        { if (!tempRoot.IsPositiveOrZero())
          { tempRoot.Minus();
            theGens[j]=(tempRoot);
          }
          else
            theGens[i]=(tempRoot);
          reductionOccured=true;
        }
      }
  }
}

template <class coefficient>
void Vector<coefficient>::PerturbNoZeroScalarProductWithMe(const List<Vector<coefficient> >& inputVectors)
{ MacroRegisterFunctionWithName("Vector::PerturbNoZeroScalarProductWithMe");
  coefficient theScalarProdInverted;
  for (int i=0; i<inputVectors.size; i++)
    if (this->ScalarEuclidean(inputVectors[i])==0)
    { coefficient theScale=1;
      for (int j=0; j<i; j++)
        if (inputVectors[i].ScalarEuclidean(inputVectors[j])!=0)
        { theScalarProdInverted=(this->ScalarEuclidean(inputVectors[j])/inputVectors[i].ScalarEuclidean(inputVectors[j]))/2;
          if (theScalarProdInverted<0)
            theScalarProdInverted*=-1;
          if (theScale==0)
            theScale=theScalarProdInverted;
          else if (theScalarProdInverted!=0)
            theScale=MathRoutines::Minimum(theScale, theScalarProdInverted);
        }
      *this+=inputVectors[i]*theScale;
    }
  for (int i=0; i<inputVectors.size; i++)
    if (this->ScalarEuclidean(inputVectors[i])==0)
    { std::cout << "This is a programming error: the vector produced by PerturbNoZeroScalarProductWithMe, namely, "
      << this->ToString() << " is orthogonal to input vector " << inputVectors[i].ToString() << ". The full list of vectors is "
      << inputVectors.ToString() << CGI::GetStackTraceEtcErrorMessage(__FILE__, __LINE__);
      assert(false);
    }
}

void WeylGroup::TransformToSimpleBasisGeneratorsArbitraryCoords(Vectors<Rational>& theGens, const HashedList<Vector<Rational> >& inputRootSystem)
{ if (theGens.size==0)
    return;
  MacroRegisterFunctionWithName("WeylGroup::TransformToSimpleBasisGeneratorsArbitraryCoords");
//  std::cout << "<hr>Transforming to simple " << theGens.ToString() << " with root system " << inputRootSystem.ToString();
  Vector<Rational> theH;
  theH.MakeZero(theGens[0].size);
  theH.PerturbNoZeroScalarProductWithMe(inputRootSystem);
  for (int i=0; i<theGens.size; i++)
    if (theGens[i].ScalarEuclidean(theH)<0)
      theGens[i].Minus();
  bool reductionOccured=true;
  Vector<Rational> tempRoot;
  while (reductionOccured)
  { reductionOccured= false;
    for (int i=0; i<theGens.size; i++)
      for (int j=i+1; j<theGens.size; j++)
      { tempRoot=(theGens[i]);
        tempRoot-=(theGens[j]);
        if (tempRoot.IsEqualToZero())
        { theGens.RemoveIndexSwapWithLast(j);
          reductionOccured=true;
        }
        if (inputRootSystem.Contains(tempRoot))
        { if (tempRoot.ScalarEuclidean(theH)<0)
          { tempRoot.Minus();
            theGens[j]=(tempRoot);
          } else
            theGens[i]=(tempRoot);
          reductionOccured=true;
        }
      }
  }
//  std::cout << ". Final basis: " << theGens.ToString();
}

void WeylGroup::TransformToSimpleBasisGeneratorsWRTh(Vectors<Rational>& theGens, const Vector<Rational>& theH)
{ for (int i=0; i<theGens.size; i++)
    if (!this->IsPositiveOrPerpWRTh(theGens[i], theH))
      theGens[i].Minus();
  bool reductionOccured=true;
  Vector<Rational> tempRoot;
  while (reductionOccured)
  { reductionOccured= false;
    for (int i=0; i<theGens.size; i++)
      for (int j=i+1; j<theGens.size; j++)
      {// if (this->flagAnErrorHasOccuredTimeToPanic)
        //  theGens.ComputeDebugString();
        tempRoot=theGens[i];
        tempRoot-=theGens[j];
        //if (this->flagAnErrorHasOccuredTimeToPanic)
          //tempRoot.ComputeDebugString();
        if (tempRoot.IsEqualToZero())
        { theGens.RemoveIndexSwapWithLast(j);
          reductionOccured=true;
        }
        if (this->RootSystem.GetIndex(tempRoot)!=-1)
        { if (!this->IsPositiveOrPerpWRTh(tempRoot, theH))
          { tempRoot.Minus();
            theGens[j]=(tempRoot);
          }
          else
            theGens[i]=(tempRoot);
          reductionOccured=true;
        }
      }
  }
}

void rootSubalgebra::ComputeExtremeWeightInTheSameKMod(const Vector<Rational>& input, Vector<Rational>& outputW, bool lookingForHighest)
{ outputW=(input);
  for(bool FoundHigher=true; FoundHigher; )
  { FoundHigher=false;
    for (int i=0; i<this->SimpleBasisK.size; i++)
    { Vector<Rational> tempRoot;
      tempRoot=(outputW);
      if (lookingForHighest)
        tempRoot+=(this->SimpleBasisK[i]);
      else
        tempRoot-=(this->SimpleBasisK[i]);
      if (this->GetAmbientWeyl().RootSystem.GetIndex(tempRoot)!=-1)
      { outputW=(tempRoot);
        FoundHigher=true;
      }
      if (tempRoot.IsEqualToZero())
      { outputW.MakeZero(this->GetOwnerSSalg().GetRank());
        return;
      }
    }
  }
}

void rootSubalgebra::ComputeHighestWeightInTheSameKMod(const Vector<Rational>& input, Vector<Rational>& outputHW)
{ this->ComputeExtremeWeightInTheSameKMod(input, outputHW, true);
}

void rootSubalgebra::ComputeLowestWeightInTheSameKMod(const Vector<Rational>& input, Vector<Rational>& outputLW)
{ this->ComputeExtremeWeightInTheSameKMod(input, outputLW, false);
}

bool rootSubalgebra::RootsDefineASubalgebra(Vectors<Rational>& theRoots)
{ Vector<Rational> tempRoot;
  for (int i=0; i<theRoots.size; i++)
  { if (!this->IsARoot(theRoots[i]))
      return false;
    for (int j=i+1; j<theRoots.size; j++)
    { tempRoot=(theRoots[i]);
      tempRoot+=(theRoots[j]);
      if (this->IsARoot(tempRoot))
        if (!theRoots.Contains(tempRoot))
          return false;
    }
  }
  return true;
}

bool rootSubalgebra::IsBKhighest(Vector<Rational>& input)
{ for (int i=0; i<this->SimpleBasisK.size; i++)
    if (this->IsARoot(input + this->SimpleBasisK[i]))
      return false;
  return true;
}

bool rootSubalgebra::rootIsInCentralizer(Vector<Rational>& input)
{ Vector<Rational> tempRoot;
  for(int i=0; i<this->SimpleBasisK.size; i++)
  { tempRoot=input+this->SimpleBasisK[i];
    if (this->IsARoot(tempRoot) || tempRoot.IsEqualToZero())
      return false;
    tempRoot=input-this->SimpleBasisK[i];
    if (this->IsARoot(tempRoot) || tempRoot.IsEqualToZero())
      return false;
  }
  return true;
}

void rootSubalgebra::WriteMultTableAndOppositeKmodsToFile(std::fstream &output, List<List<List<int> > >& inMultTable, List<int>& inOpposites)
{ output << "pairing_table_size: " << inMultTable.size << "\n";
  for (int i=0; i<inMultTable.size; i++)
    for (int j=0; j<inMultTable.size; j++)
    { output << inMultTable[i][j].size << " ";
      for(int k=0; k<inMultTable[i][j].size; k++)
        output << inMultTable[i][j][k] << " ";
    }
  output << "\nopposites: ";
  for (int i=0; i<inMultTable.size; i++)
    output << inOpposites[i] << " ";
}

void rootSubalgebra::ReadMultTableAndOppositeKmodsFromFile(std::fstream& input, List<List<List<int> > >& outMultTable, List<int>& outOpposites)
{ std::string tempS;
  int tempI, theSize;
  input >> tempS >> theSize;
  outMultTable.SetSize(theSize);
  outOpposites.SetSize(theSize);
  for (int i=0; i<theSize; i++)
  { outMultTable[i].SetSize(theSize);
    for (int j=0; j<theSize; j++)
    { input >> tempI;
      outMultTable[i][j].SetSize(tempI);
      for(int k=0; k<outMultTable[i][j].size; k++)
        input >> outMultTable[i][j][k];
    }
  }
  input >> tempS;
  for (int i=0; i<outMultTable.size; i++)
    input >> outOpposites[i];
  assert(tempS=="opposites:");
}

bool rootSubalgebra::rootIsInNilradicalParabolicCentralizer(Selection& positiveSimpleRootsSel, Vector<Rational>& input)
{ Vector<Rational> tempRoot;
  bool found =true;
  Vector<Rational> currentRoot=input;
  bool foundPositive=false;
  while(found)
  { found=false;
    for (int k=0; k<this->SimpleBasisCentralizerRoots.size; k++)
    { tempRoot=currentRoot - this->SimpleBasisCentralizerRoots.TheObjects[k];
      if (this->IsARoot(tempRoot)|| tempRoot.IsEqualToZero())
      { currentRoot=(tempRoot);
        found=true;
        if(positiveSimpleRootsSel.selected[k])
          foundPositive=true;
        if (currentRoot.IsEqualToZero())
          return foundPositive;
      }
    }
  }
  return false;
}

void rootSubalgebra::GeneratePossibleNilradicalsRecursive(Controller& PauseMutex, GlobalVariables& theGlobalVariables, multTableKmods& multTable, List<Selection>& impliedSelections, List<int>& oppositeKmods, rootSubalgebras& owner, int indexInOwner)
{ int& RecursionDepth=owner.RecursionDepthNilradicalsGeneration;
  std::string tempSsel, tempSopposite;
  if (this->flagAnErrorHasOccuredTimeToPanic)
  { multTable.ComputeDebugString(*this);
    std::stringstream out; out << "\n\t";
    for (int i=0; i<oppositeKmods.size; i++)
      out << i << " / " << oppositeKmods[i] << "\t";
    tempSopposite=out.str();
    multTable.DebugString.append(tempSopposite);
  }
  List<int>& counters=owner.CountersNilradicalsGeneration;
  while (RecursionDepth>-1)
  { while(counters[RecursionDepth]<this->kModules.size)
    { if (!impliedSelections[RecursionDepth].selected[counters[RecursionDepth]])
        if (this->IndexIsCompatibleWithPrevious(counters[RecursionDepth], RecursionDepth, multTable, impliedSelections, oppositeKmods, owner, theGlobalVariables))
        { RecursionDepth++;
          counters[RecursionDepth]=counters[RecursionDepth-1];
        }
    /*  if (this->flagAnErrorHasOccuredTimeToPanic)
      { impliedSelections.ElementToStringGeneric(tempSsel, RecursionDepth+1);
        impliedSelections.TheObjects[RecursionDepth].ComputeDebugString();
      }*/
      counters[RecursionDepth]++;
    }
    this->PossibleNilradicalComputation(theGlobalVariables, impliedSelections[RecursionDepth], owner, indexInOwner);
//    if (this->flagAnErrorHasOccuredTimeToPanic)
//      tempSels.AddOnTop(impliedSelections.TheObjects[RecursionDepth]);
    RecursionDepth--;
    if (RecursionDepth>-1)
      counters[RecursionDepth]++;
    PauseMutex.SafePointDontCallMeFromDestructors();
  }
}

bool rootSubalgebra::ListHasNonSelectedIndexLowerThanGiven(int index, List<int>& tempList, Selection& tempSel)
{ for (int j=0; j<tempList.size; j++)
    if (tempList[j]<index)
      if (!tempSel.selected[tempList[j]])
        return false;
  return true;
}

bool rootSubalgebra::IndexIsCompatibleWithPrevious(int startIndex, int RecursionDepth,  multTableKmods& multTable, List<Selection>& impliedSelections, List<int>& oppositeKmods, rootSubalgebras& owner, GlobalVariables& theGlobalVariables)
{ Selection& targetSel= impliedSelections[RecursionDepth+1];
  Selection& originalSel=impliedSelections[RecursionDepth];
  targetSel=(originalSel);
  targetSel.AddSelectionAppendNewIndex(startIndex);
  for (int k=targetSel.CardinalitySelection-1; k<targetSel.CardinalitySelection; k++)
  { int tempI=targetSel.elements[k];
    for (int i=0; i<targetSel.CardinalitySelection; i++ )
    { if (targetSel.selected[oppositeKmods[targetSel.elements[i]]])
        return false;
      List<int>& tempList=multTable[tempI][targetSel.elements[i]];
      for (int j=0; j<tempList.size; j++)
      { if (tempList[j]<startIndex && !originalSel.selected[tempList[j]])
          return false;
        else
          targetSel.AddSelectionAppendNewIndex(tempList[j]);
      }
    }
  }
  if (!owner.ApproveKmoduleSelectionWRTActionsNormalizerCentralizerNilradical(targetSel, theGlobalVariables))
    return false;
  return true;
}

void rootSubalgebra::PossibleNilradicalComputation(GlobalVariables& theGlobalVariables, Selection& selKmods, rootSubalgebras& owner, int indexInOwner)
{ this->NumNilradicalsAllowed++;
  if (owner.flagCountingNilradicalsOnlyNoComputation)
  { owner.numNilradicalsBySA[indexInOwner]++;
    this->MakeProgressReportPossibleNilradicalComputation(&theGlobalVariables, owner, indexInOwner);
    return;
  }
  //this->ComputeDebugString();
  if (this->flagFirstRoundCounting)
    this->NumTotalSubalgebras=this->NumNilradicalsAllowed;
  if (!this->flagFirstRoundCounting)
  { this->NilradicalKmods=selKmods;
    if(!this->ConeConditionHolds(theGlobalVariables, owner, indexInOwner, owner.flagComputingLprohibitingWeights))
    { this->NumConeConditionFailures++;
      owner.NumConeConditionFailures++;
      if (owner.flagStoringNilradicals)
      { List<List<int> >& currentSAList = owner.storedNilradicals[indexInOwner];
        List<int> newNilradical;
        newNilradical.SetSize(selKmods.CardinalitySelection);
        for (int i=0; i<selKmods.CardinalitySelection; i++)
          newNilradical[i]=selKmods.elements[i];
        currentSAList.AddOnTop(newNilradical);
      }
    } else
    {

      //the below commented out code should be incapsulated. It computes whether a given nilradical is a nilradical of a parabolic subalgebra.
      //this task is pushed on the end of the to-do list.
      /* owner.NumConeConditionHoldsBySSpart.TheObjects[indexInOwner]++;
      if (owner.ReportStringNonNilradicalParabolic=="")
      { this->ComputeRootsOfK();
        Vectors<Rational> tempNilradical; Vectors<Rational> tempOthers; Vectors<Rational> tempK;
        for (int i=0; i<this->kModules.size; i++)
          if (this->NilradicalKmods.selected[i])
            tempNilradical.AddListOnTop(this->kModules.TheObjects[i]);
          else
            tempOthers.AddListOnTop(this->kModules.TheObjects[i]);
        for (int i=0; i<this->PositiveRootsK.size; i++)
        { tempOthers.AddOnTop(this->PositiveRootsK.TheObjects[i]);
          tempOthers.AddOnTop(-this->PositiveRootsK.TheObjects[i]);
          tempK.AddOnTop(this->PositiveRootsK.TheObjects[i]);
        }
        if (Vectors<Rational>::ConesIntersect(theGlobalVariables, tempNilradical, tempOthers, owner.AmbientWeyl.CartanSymmetric.NumRows))
        { Vectors<Rational> tempRoots; std::stringstream out; std::string tempS;
          this->AmbientWeyl.GetEpsilonCoords(tempNilradical, tempRoots, theGlobalVariables);
          tempRoots.ElementToStringEpsilonForm(tempS, true, false, false);
          out << tempS;
          this->AmbientWeyl.GetEpsilonCoords(tempK, tempRoots, theGlobalVariables);
          tempRoots.ElementToStringEpsilonForm(tempS, true, false, false);
          out << "\n\n" << tempS;
          owner.ReportStringNonNilradicalParabolic=out.str();
        }
      }*/
    }
  }
  this->MakeProgressReportPossibleNilradicalComputation(&theGlobalVariables, owner, indexInOwner);
}

void rootSubalgebra::MakeProgressReportGenAutos(int progress, int outOf, int found, GlobalVariables& theGlobalVariables)
{ ProgressReport theReport(&theGlobalVariables);
  std::stringstream out4, out5;
  out5 << progress+1 << " out of " << outOf << " checked; ";
  out5 << found << " found pos. generators";
  theReport.Report(out5.str());
}

void rootSubalgebra::MakeProgressReportPossibleNilradicalComputation(GlobalVariables* theGlobalVariables, rootSubalgebras& owner, int indexInOwner)
{ MacroRegisterFunctionWithName("rootSubalgebra::MakeProgressReportPossibleNilradicalComputation");
  if (theGlobalVariables==0)
    return;
  ProgressReport report1(theGlobalVariables), report2(theGlobalVariables),
  report3(theGlobalVariables), report4(theGlobalVariables), report5(theGlobalVariables);
  if (this->flagMakingProgressReport)
  { std::stringstream out1, out2, out3, out4, out5;
    if (this->flagFirstRoundCounting)
    { out1 << "Counting ss part " << this->theDynkinDiagram.ToStringRelativeToAmbientType(this->owneR->theWeyl.theDynkinType[0]);
      out2 << "# nilradicals for fixed ss part: " << this->NumTotalSubalgebras;
      owner.NumSubalgebrasCounted++;
      out3 << owner.NumSubalgebrasCounted << " total subalgebras counted";
    }
    else
    { out1 << "Computing ss part " << this->theDynkinDiagram.ToStringRelativeToAmbientType(this->owneR->theWeyl.theDynkinType[0]);
      out2 << this->NumNilradicalsAllowed << " Nilradicals processed out of " << this->NumTotalSubalgebras;
      owner.NumSubalgebrasProcessed++;
      out3 << "Total # subalgebras processed: " << owner.NumSubalgebrasProcessed;
      out4 << "Num cone condition failures: " << owner.NumConeConditionFailures;
      out5 << "Num failures to find l-prohibiting relations: " << owner.theBadRelations.size;
      report4.Report(out4.str());
      report5.Report(out5.str());
    }
    report1.Report(out1.str());
    report2.Report(out2.str());
    report3.Report(out3.str());
  }
}

void rootSubalgebra::GenerateKmodMultTable(List<List<List<int> > >& output, List<int>& oppositeKmods, GlobalVariables* theGlobalVariables)
{ output.SetSize(this->kModules.size);
  oppositeKmods.SetSize(this->kModules.size);
  int numTotal= this->kModules.size* this->kModules.size;
  std::stringstream out;
  out << "Computing pairing table for the module decomposition of the root subalgebra of type "
  << this->theDynkinDiagram.ToStringRelativeToAmbientType(this->owneR->theWeyl.theDynkinType[0])
  << "\n<br>\nwith centralizer " << this->theCentralizerDiagram.ToStringRelativeToAmbientType(this->owneR->theWeyl.theDynkinType[0]);
  ProgressReport theReport(theGlobalVariables);
  theReport.Report(out.str());
  ProgressReport theReport2(theGlobalVariables);
  for (int i=0; i<this->kModules.size; i++)
  { output[i].SetSize(this->kModules.size);
    for (int j=0; j<this->kModules.size; j++)
    { this->KmodTimesKmod(i, j, oppositeKmods, output[i][j]);
      if (theGlobalVariables!=0)
      { std::stringstream out5;
        out5 << "Computing pairing table: " << i*this->kModules.size+j+1 << " out of " << numTotal;
        theReport2.Report(out5.str());
      }
    }
  }
}

bool rootSubalgebra::IsARoot(const Vector<Rational>& input)
{ if (input.size!=this->GetOwnerSSalg().GetRank())
    return false;
  return this->GetAmbientWeyl().RootSystem.Contains(input);
}

bool rootSubalgebra::IsARootOrZero(Vector<Rational>& input)
{ return input.IsEqualToZero() || this->IsARoot(input);
}

void rootSubalgebra::KmodTimesKmod(int index1, int index2, List<int>& oppositeKmods, List<int>& output)
{ Vector<Rational> tempRoot;
  output.size=0;
  for (int i=0; i<this->kModules[index1].size; i++)
    for (int j=0; j<this->kModules[index2].size; j++)
    { tempRoot=(this->kModules[index1][i]);
      tempRoot+=(this->kModules[index2][j]);
      if (tempRoot.IsEqualToZero())
      { oppositeKmods[index1]=index2;
        oppositeKmods[index2]=index1;
      }
      else
        if (this->IsARoot(tempRoot))
          for (int k=0; k<this->kModules.size; k++)
            if (this->kModules[k].GetIndex(tempRoot)!=-1)
            { output.AddOnTopNoRepetition(k);
              break;
            }
    }
}

void rootSubalgebra::ComputeKModules()
{ this->HighestRootsK.size =0;
  this->LowestWeightsGmodK.size=0;
  this->HighestWeightsGmodK.size=0;
  this->kModules.size=0;
  //this->ComputeDebugString();
  this->ComputeRootsOfK();
  Vector<Rational> tempLW, tempHW;
  HashedList<Vector<Rational> >& AllRoots= this->GetAmbientWeyl().RootSystem;
  this->kModules.ReservE(AllRoots.size);
  this->HighestRootsK.ReservE(this->GetAmbientWeyl().CartanSymmetric.NumRows);
  this->LowestWeightsGmodK.ReservE(AllRoots.size);
  this->HighestWeightsGmodK.ReservE(AllRoots.size);
  for (int i=0; i<AllRoots.size; i++)
    if (this->AllRootsK.GetIndex(AllRoots[i])==-1)
    { this->ComputeLowestWeightInTheSameKMod(AllRoots[i], tempLW);
      this->ComputeHighestWeightInTheSameKMod(AllRoots[i], tempHW);
      int x=this->LowestWeightsGmodK.GetIndex(tempLW);
      if (x==-1)
      { this->LowestWeightsGmodK.AddOnTop(tempLW);
        this->HighestWeightsGmodK.AddOnTop(tempHW);
        x=this->LowestWeightsGmodK.size -1;
        this->kModules.SetSize(this->LowestWeightsGmodK.size);
        this->kModules[x].size=0;
      }
      this->kModules[x].AddOnTop(AllRoots[i]);
      if (AllRoots[i]==(tempHW))
        this->kModules[x].SwapTwoIndices(0, this->kModules[x].size-1);
    } else
    { if (AllRoots[i].IsPositiveOrZero())
      { Vector<Rational> tempHW;
        this->ComputeHighestWeightInTheSameKMod(AllRoots[i], tempHW);
        int x=this->HighestRootsK.GetIndex(tempHW);
        if (x==-1)
        { this->HighestRootsK.AddOnTop(tempHW);
          x=this->HighestRootsK.size -1;
          this->PosRootsKConnectedComponents.SetSize(this->HighestRootsK.size);
        }
        this->PosRootsKConnectedComponents[x].AddOnTop(AllRoots[i]);
      }
    }
}

int rootSubalgebra::NumRootsInNilradical()
{ int result=0;
  for (int i=0; i<this->NilradicalKmods.CardinalitySelection; i++)
    result+=this->kModules[this->NilradicalKmods.elements[i]].size;
  return result;
}

int rootSubalgebra::GetIndexKmoduleContainingRoot(Vector<Rational>& input)
{ for (int i=0; i<this->kModules.size; i++)
    if (this->kModules[i].Contains(input))
      return i;
  return -1;
}

bool rootSubalgebra::ConeConditionHolds
(GlobalVariables& theGlobalVariables, rootSubalgebras& owner, int indexInOwner, Vectors<Rational>& NilradicalRoots, Vectors<Rational>& Ksingular, bool doExtractRelations)
{ if (Vectors<Rational>::ConesIntersect(NilradicalRoots, Ksingular, 0, 0, &theGlobalVariables))
  { if (doExtractRelations)
      this->ExtractRelations
      (theGlobalVariables.matConeCondition1.GetElement(), theGlobalVariables.matConeCondition3.GetElement(), NilradicalRoots, owner, indexInOwner, theGlobalVariables, Ksingular);
    return false;
  }
  return true;
}

template<class coefficient>
bool Vectors<coefficient>::PerturbVectorToRegular(Vector<coefficient>& output, GlobalVariables& theGlobalVariables)
{ Vector<coefficient> normal;
  bool result=false;
  while (!this->IsRegular(output, normal, output.size))
  { result=true;
    normal/=(10);
    output+=(normal);
  }
  if (result)
    output.ScaleByPositiveRationalToIntegralMinHeight();
  return result;
}
