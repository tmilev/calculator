//The current file is licensed under the license terms found in the main header file "vpf.h".
//For additional information refer to the file "vpf.h".
#include "vpfHeader1General0_General.h"
#include "vpfHeader2Math0_General.h"
#include "vpfImplementationHeader2Math051_PolynomialComputations_Basic.h"
#include "vpfImplementationHeader2Math3_FiniteGroups.h"
ProjectInformationInstance ProjectInfoVpf9cpp(__FILE__, "Math routines implementation. ");

//the below gives upper limit to the amount of pointers that are allowed to be allocated by the program. Can be changed dynamically.
//used to guard the web server from abuse.
#ifdef CGIversionLimitRAMuse
long long ParallelComputing::cgiLimitRAMuseNumPointersInList=2000000000;
#endif

ControllerStartsRunning ParallelComputing::controllerSignalPauseUseForNonGraciousExitOnly;
bool ParallelComputing::flagUngracefulExitInitiated=false;

long long ParallelComputing::GlobalPointerCounter=0;
long long ParallelComputing::PointerCounterPeakRamUse=0;

//CombinatorialChamberContainer GlobalCollectorChambers;
//FacetPointers GlobalCollectorFacets;

Vector<Rational> PartFraction::theVectorToBePartitioned;
bool PartFraction::MakingConsistencyCheck=false;
Rational PartFractions::CheckSum;
Rational PartFraction::CheckSum;
Rational PartFraction::CheckSum2;
int DrawingVariables::NumHtmlGraphics=0;

int CGI::GlobalMathSpanID=0;
int CGI::GlobalCanvasID=0;
int CGI::GlobalGeneralPurposeID=0;

template < > bool Matrix<Rational>::flagComputingDebugInfo=true;
template < > bool Polynomial<Rational>::flagAnErrorHasOccuredTimeToPanic=true;

template < > bool CompleX<double>::flagEqualityIsApproximate=true;
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
//FacetPointers TheBigFacetOutput;
//DrawingVariables TDV(200, 400);

unsigned long long int Rational::TotalLargeAdditions=0;
unsigned long long int Rational::TotalLargeGCDcalls=0;
unsigned long long int Rational::TotalLargeMultiplications=0;
unsigned long long int Rational::TotalSmallAdditions=0;
unsigned long long int Rational::TotalSmallGCDcalls=0;
unsigned long long int Rational::TotalSmallMultiplications=0;

GlobalVariables::GlobalVariables()
{ this->StandardStringOutputFunction=0;
  this->MaxComputationTimeSecondsNonPositiveMeansNoLimit=1000000;
  this->callSystem=0;
  this->ReturnIndicator=0;
  this->sleepFunction=0;
  this->flagGaussianEliminationProgressReport=false;
  this->getElapsedTimePrivate=0;
//  stOutput << "Global variables created!";
}

void ProgressReport::Report(const std::string& theReport)
{ if (this->pointerGV==0)
    return;
  this->pointerGV->ProgressReportStringS[currentLevel]=theReport;
  this->pointerGV->MakeReport();
}

void ProgressReport::initFromGV(GlobalVariables* theGlobalVariables)
{ this->pointerGV=theGlobalVariables;
  if (this->pointerGV==0)
    return;
  currentLevel=this->pointerGV->ProgressReportStringS.size;
  this->pointerGV->ProgressReportStringS.SetSize(this->pointerGV->ProgressReportStringS.size+1);
  *this->pointerGV->ProgressReportStringS.LastObject()="";
}

ProgressReport::~ProgressReport()
{ if (this->pointerGV==0)
    return;
  pointerGV->ProgressReportStringS.size--;
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

std::string CGI::CleanUpForFileNameUse(const std::string& inputString)
{ std::stringstream out;
  for (int i=0; i<(signed) inputString.size(); i++)
    if (inputString[i]=='/')
      out << "_div_";
    else
      out << inputString[i];
  return out.str();
}

std::string CGI::CleanUpForLaTeXLabelUse(const std::string& inputString)
{ std::stringstream out;
  for (int i=0; i<(signed) inputString.size(); i++)
    if (inputString[i]!='/' && inputString[i]!='^' && inputString[i]!='_' && inputString[i]!='{'
        && inputString[i]!='}')
      out << inputString[i];
  return out.str();
}

void CGI::clearDollarSigns(std::string& theString, std::string& output)
{ std::stringstream out;
  for(unsigned int i=0; i<theString.size(); i++)
    if(theString[i]!='$')
      out << theString[i];
  output=out.str();
}

std::string CGI::DoubleBackslashes(const std::string& input)
{ std::stringstream out;
  for (int i=0; i<(signed) input.size(); i++)
  { out << input[i];
    if (input[i]=='\\')
      out << "\\";
  }
  return out.str();
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
{ stOutput << "\n\tvar num" << lineTypeName << "Lines=" << numberLines << "; ";
  stOutput << "\n\tvar " << lineTypeName << "1= new Array(" << numberLines << "); " << "  \tvar " << lineTypeName << "2= new Array("
  << numberLines << "); " << "  \tvar clr" << lineTypeName << "= new Array("  << numberLines << "); ";
}

void CGI::outputLineJavaScriptSpecific(const std::string& lineTypeName, int theDimension, std::string& stringColor, int& lineCounter)
{ std::string tempS;
  stOutput  << "\n\t" << lineTypeName << "1["  << lineCounter << "]= new Array(" << theDimension << "); " << "\t" << lineTypeName << "2["
  << lineCounter << "]= new Array(" << theDimension << "); " << "\tclr" << lineTypeName << "[" << lineCounter << "]= new Array(" << 3 << "); \n";
  for (int j=0; j< theDimension; j++)
  { CGI::outputStream >> tempS;
    stOutput << "\t" << lineTypeName << "1[" << lineCounter << "][" << j << "]=" << tempS << "; ";
    CGI::outputStream >> tempS;
    stOutput << "\t" << lineTypeName << "2[" << lineCounter << "][" << j << "]=" << tempS << "; ";
  }
  stOutput << "\tclr" << lineTypeName << "[" << lineCounter << "]=" << stringColor << "; ";
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
  FileOperations::OpenFileCreateIfNotPresent(fileIn, FileName, false, false, false);
  if(!fileIn.is_open())
    crash << crash;
  fileIn.clear(std::ios::goodbit);
  fileIn.seekg(0, std::ios_base::end);
  int theSize= fileIn.tellg();
  fileIn.seekg(0);
  char* buffer= new char[theSize*2+1];
  fileIn.read(buffer, theSize*2);
  std::string nameFileOut= FileName;
  nameFileOut.append(".new");
  ::FileOperations::OpenFileCreateIfNotPresent(fileOut, nameFileOut, false, true, false);
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

#include <dirent.h>
bool FileOperations::IsFolder(const std::string& theFolderName)
{ MacroRegisterFunctionWithName("FileOperations::IsFolder");
  DIR *pDir;
  pDir = opendir(theFolderName.c_str());
  if (pDir != NULL)
  { closedir(pDir);
    return true;
  }
  return false;
}

std::string FileOperations::GetFileExtensionWithDot(const std::string& theFileName)
{ if (theFileName=="")
    return "";
  for (unsigned i=theFileName.size()-1; i>=0; i--)
    if (theFileName[i]=='.')
      return theFileName.substr(i, std::string::npos);
  return "";
}

bool FileOperations::GetFolderFileNames
(const std::string& theFolderName, List<std::string>& outputFileNamesNoPath, List<std::string>* outputFileTypes)
{ MacroRegisterFunctionWithName("FileOperations::GetFolderFileNames");
  DIR *theDirectory = opendir(theFolderName.c_str());
  if (theDirectory==NULL)
    return false;
  outputFileNamesNoPath.ReservE(1000);
  if (outputFileTypes!=0)
    outputFileTypes->ReservE(1000);
  std::string fullName;
  for (dirent *fileOrFolder=readdir(theDirectory); fileOrFolder!=0; fileOrFolder= readdir (theDirectory))
  { outputFileNamesNoPath.AddOnTop(fileOrFolder->d_name);
    if (outputFileTypes!=0)
    { fullName=theFolderName + fileOrFolder->d_name;
      if (FileOperations::IsFolder(fullName))
        outputFileTypes->AddOnTop(".d");
      else
        outputFileTypes->AddOnTop("");
    }
  }
  closedir (theDirectory);
  return true;
}

bool FileOperations::FileExists(const std::string& theFileName)
{ std::fstream theFile;
  theFile.open(theFileName.c_str(), std::fstream::in);
  if(theFile.is_open())
    return true;
  else
    return false;
}

bool FileOperations::OpenFile(std::fstream& theFile, const std::string& theFileName, bool OpenInAppendMode, bool truncate, bool openAsBinary)
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
  return theFile.is_open();
}

bool FileOperations::OpenFileCreateIfNotPresent(std::fstream& theFile, const std::string& theFileName, bool OpenInAppendMode, bool truncate, bool openAsBinary)
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

bool XML::ReadFromFile(std::fstream& inputFile)
{ MacroRegisterFunctionWithName("XML::ReadFromFile");
  inputFile.seekg(0, std::fstream::end);
  unsigned int theFileSize = inputFile.tellg();
  char* memblock = new char [theFileSize];
  inputFile.seekg (0, std::fstream::beg);
  inputFile.read (memblock, theFileSize);
  this->theString=memblock;
  delete[] memblock;
  return true;
}

XML::XML()
{ this->positionInString=-1;
}

bool XML::GetStringEnclosedIn(const std::string& theTagName, std::string& outputString)
{ MacroRegisterFunctionWithName("XML::GetStringEnclosedIn");
  std::string charReader="";
  std::string theOpenTagWithSymbols=this->GetOpenTagNoInputCheck(theTagName);
  std::string theCloseTagWithSymbols=this->GetCloseTagNoInputCheck(theTagName);
  int lengthOpenTag=theOpenTagWithSymbols.size();
  int lengthCloseTag=theCloseTagWithSymbols.size();
  int positionInOpenTag=0;
  int positionInCloseTag=0;
  int numTags=0;
  //stOutput << "open tag with symbols: " << theOpenTagWithSymbols << ", close tag: " << theCloseTagWithSymbols;
//  stOutput << "lengths are :" << lengthOpenTag << " and "<< lengthCloseTag;
  std::stringstream out;
  if (this->positionInString<0)
    this->positionInString=0;
  bool tagWasClosed=false;
  int numCharRead=0;
  for (; this->positionInString<(signed)this->theString.size(); this->positionInString++)
  { numCharRead++;
    charReader.push_back(this->theString[this->positionInString]);
    bool tagStarted=false;
  if (this->theString[this->positionInString]==theOpenTagWithSymbols[positionInOpenTag])
    { positionInOpenTag++;
      if (positionInOpenTag>=lengthOpenTag)
      { charReader="";
        numTags++;
//        stOutput << "<br>numTags: " << numTags;
        positionInOpenTag=0;
      }
      //stOutput << "<br>found " << charReader << " from " << theOpenTagWithSymbols;
      tagStarted=true;
    } else
      positionInOpenTag=0;
    if (this->theString[this->positionInString]==theCloseTagWithSymbols[positionInCloseTag])
    { positionInCloseTag++;
      if (positionInCloseTag>=lengthCloseTag)
      { positionInCloseTag=0;
        charReader= "";
        numTags--;
//        stOutput << "<br>numTags: " << numTags;
        if (numTags<0)
          return false;
        if (numTags==0)
        { this->positionInString++;
          tagWasClosed=true;
          break;
        }
      }
      //stOutput << "<br>found " << charReader << " from " << theCloseTagWithSymbols;
      tagStarted=true;
    } else
      positionInCloseTag=0;
    if (tagStarted)
      continue;
    if (numTags>0)
      out << charReader;
    charReader="";
  }
  if (!tagWasClosed)
  { //stOutput << "tag wasn't closed. Read so far: " << out.str();
    //stOutput << "num chars read: " << numCharRead;
    return false;
  }
  outputString=out.str();
  return true;
}

void DrawingVariables::GetCoordsForDrawing(DrawingVariables& TDV, Vector<Rational>& r, double& x, double& y)
{ x=TDV.theBuffer.centerX[0];
  y=TDV.theBuffer.centerY[0];
  for (int k=0; k<r.size; k++)
  { x+=(r[k].GetDoubleValue())*(TDV.theBuffer.ProjectionsEiVectors[k][0]);
    y-=(r[k].GetDoubleValue())*(TDV.theBuffer.ProjectionsEiVectors[k][1]);
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
  if(tempS!="cardinality:")
    crash << crash;
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

std::string Rational::ToString(FormatExpressions* theFormat)const
{ if (theFormat!=0)
    if (theFormat->flagUseFrac)
      return this->ToStringFrac();
  std::stringstream out;
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

std::string Rational::ToStringForFileOperations(FormatExpressions* notUsed)const
{ std::stringstream out;
  if (this->Extended==0)
  { if (this->NumShort<0)
      out << "-";
    int numShortAbsoluteValue=this->NumShort<0 ?- this->NumShort: this->NumShort;
    if (this->DenShort==1)
      out << numShortAbsoluteValue;
    else
      out << numShortAbsoluteValue << "_div_" << this->DenShort;
    return out.str();
  }
  std::string tempS;
  LargeInt numAbsVal=this->Extended->num;
  if (numAbsVal<0)
  { out << "-";
    numAbsVal.sign=1;
  }
  if (this->Extended->den.IsEqualToOne())
    out << numAbsVal.ToString();
  else
    out << numAbsVal.ToString() << "_div_" << this->Extended->den.ToString();
  return out.str();
}

std::string Rational::ToStringFrac()const
{ std::stringstream out;
  if (this->Extended==0)
  { if (this->NumShort<0)
      out << "-";
    int numShortAbsoluteValue=this->NumShort<0 ?- this->NumShort: this->NumShort;
    if (this->DenShort==1)
      out << numShortAbsoluteValue;
    else
      out << "\\frac{" << numShortAbsoluteValue << "}{" << this->DenShort << "}";
    return out.str();
  }
  std::string tempS;
  LargeInt numAbsVal=this->Extended->num;
  if (numAbsVal<0)
  { out << "-";
    numAbsVal.sign=1;
  }
  if (this->Extended->den.IsEqualToOne())
    out << numAbsVal.ToString();
  else
    out << "\\frac{" << numAbsVal.ToString() << "}{" << this->Extended->den.ToString() << "}";
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
  this->flagIncludeMutableInformation=true;
  this->flagUseMathSpanPureVsMouseHover=false;
  this->flagDynkinTypeDontUsePlusAndExponent=false;
  this->flagSupressDynkinIndexOne=false;
  this->flagFormatWeightAsVectorSpaceIndex=true;
  this->flagUseFrac=false;
  this->flagFormatMatrixAsLinearSystem=false;
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
    if(tempI<=0)
      crash << crash;
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
  if(!this->Extended->num.IsPositiveOrZero())
    crash << crash;
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
  { crash << "This is a programming error: taking factorial of the negative number " << n << ". " << crash;
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
  { //stOutput << "<br>" << result.ToString() << " * " << (n-i).ToString() << "/" << i+1;
    result*=n-i;
    result.DivideByInteger(i+1);
    //stOutput << "=" << result.ToString();
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

double Rational::GetDoubleValue()const
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
{ if(!(x<LargeIntUnsigned::CarryOverBound))
    crash << crash;
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
//  if(!this->CheckForConsistensy())crash << crash;
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
//  if(!this->CheckForConsistensy())crash << crash;
}

void LargeIntUnsigned::FitSize()
{ int newSize=this->theDigits.size;
  for (int i=this->theDigits.size-1; i>=1; i--)
    if (this->theDigits[i]==0)
      newSize--;
    else
      break;
  this->theDigits.SetSize(newSize);
//  if(!this->CheckForConsistensy())crash << crash;
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
//  if(!this->CheckForConsistensy())crash << crash;
}

bool LargeIntUnsigned::IsSmallEnoughToFitInInt(int* whichInt)
{ LargeIntUnsigned twoToThe31=2;
  MathRoutines::RaiseToPower(twoToThe31, 31, (LargeIntUnsigned) 1);
  if (*this>=twoToThe31)
    return false;
  if (whichInt==0)
    return true;
  *whichInt=0;
  for (int i=this->theDigits.size-1; i>=0; i--)
  { *whichInt*=LargeIntUnsigned::CarryOverBound;
    *whichInt+=this->theDigits[i];
  }
  return true;
}

bool LargeInt::IsSmallEnoughToFitInInt(int* whichInt)
{ if (!this->value.IsSmallEnoughToFitInInt(whichInt))
    return false;
  if (whichInt!=0)
    *whichInt*=this->sign;
  return true;
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

double LargeInt::GetDoubleValue()const
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
//  if(!this->CheckForConsistensy())crash << crash;
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
//  if(!this->CheckForConsistensy())crash << crash;
}

int LargeIntUnsigned::GetUnsignedIntValueTruncated()
{ return (int) this->theDigits[0];
}

double LargeIntUnsigned::GetDoubleValue()const
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
//  if(!output.CheckForConsistensy())crash << crash;
}

void LargeInt::MakeZero()
{ this->value.MakeZero();
  this->sign=1;
}

void LargeInt::operator=(const Rational& other)
{ if (!other.IsInteger(this))
    crash << "This is a programming error: converting implicitly rational number " << other.ToString()
    << " to integer is not possible as the Rational number is not integral. " << crash;
//  if(!this->CheckForConsistensy())crash << crash;
}

void LargeInt::operator=(const LargeInt& x)
{ this->sign=x.sign;
  this->value=(x.value);
//  if(!this->CheckForConsistensy())crash << crash;
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
//  if(!result.CheckForConsistensy()) crash << crash;
  result.sign=this->sign* signX;
  return result;
}

LargeInt LargeInt::operator/(LargeInt& x)const
{ LargeInt result;
  LargeInt remainder;
  this->value.DivPositive(x.value, result.value, remainder.value);
  result.sign= this->sign* x.sign;
  if(!result.CheckForConsistensy())
    crash << crash;
  return result;
}

int LargeInt::operator%(int x)
{ if(x<=0)
    crash << crash;
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
  { if(this->sign == -1)
      return x-remainder.theDigits[0];
    else
      return remainder.theDigits[0];
  }
}

LargeIntUnsigned LargeIntUnsigned::operator/(unsigned int x)const
{ LargeIntUnsigned result;
  LargeIntUnsigned remainder;
  LargeIntUnsigned tempX;
  tempX.AssignShiftedUInt(x, 0);
  this->DivPositive(tempX, result, remainder);
//  if(!result.CheckForConsistensy()) crash << crash;
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
    { //if (this->IndicesNonZeroMults[i]>=this->IndicesNonZeroMults[j])crash << crash;
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
  if(tempI!=owner.startingVectors.size)
    crash << crash;
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
  //{ if(!this->CheckSum2.IsEqualTo(this->CheckSum))crash << crash;
  //}
  /*if (PartFraction::flagAnErrorHasOccurredTimeToPanic)
  { Rational tempRat2, tempRat;
    std::string tempS1, tempS2;
    Accum.ComputeOneCheckSum(tempRat2);
    this->ComputeOneCheckSum(tempRat);
    tempRat.ToString(tempS1);
    this->CheckSum2.ToString(tempS2);
    tempRat2.Subtract(tempRat);
    if(!oldCheckSum.IsEqualTo(tempRat2))crash << crash;
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
    //if(!tempRat.IsEqualTo(StartCheckSum))crash << crash;
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
    //if(!tempRat.IsEqualTo(StartCheckSum)) crash << crash;
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
    if(tempRat.IsEqualToZero())
      crash << crash;
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
    if (!this->StartCheckSum.IsEqualTo(this->EndCheckSum))
      crash << "<b>This is a programmign error. The checksum of the partial fractions failed. " << crash;
    else
    { //stOutput<< "Checksum successful";
      //stOutput.flush();
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
    if(x!=i)
      crash << crash;
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
          if(!tempFracsCheckSum.IsEqualTo(tempDiff))
            crash << crash;
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
    if(!tempRat.IsEqualTo(StartCheckSum))
      crash << crash;
  }*/
}

void PartFraction::ReduceMonomialByMonomialModifyOneMonomial
(PartFractions& Accum, GlobalVariables& theGlobalVariables, SelectionWithDifferentMaxMultiplicities& thePowers, List<int>& thePowersSigned, MonomialP& input, LargeInt& inputCoeff)
{ /*Polynomial<LargeInt>& theNumerator=theGlobalVariables.PolyLargeIntPartFracBuffer5.GetElement();
  Polynomial<LargeInt>& tempP=theGlobalVariables.PolyLargeIntPartFracBuffer6.GetElement();
  theNumerator.MakeZero(Accum.AmbientDimension);
  theNumerator.AddMonomial(input, inputCoeff);
  if(thePowersSigned.size!= thePowers.Multiplicities.size)
    crash << crash;
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
      if(StartMonomialPower<startDenominatorPower)
        crash << crash;
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
{ if (theGlobalVariables.StandardStringOutputFunction==0)
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
  if (theGlobalVariables.StandardStringOutputFunction==0)
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
  if(!theFile.is_open() && StoreToFile)
    crash << crash;
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
{ if(left.size != r.size)
    crash << crash;
  Rational accum=0;
  for (int i=0; i<left.size; i++)
    accum+=(left[i]-r[i])*theWeights[i];
  return (accum>0);
}

//NOTE NOTE NOTE: To be fixed: you gotta use the preceding function to sort the theVPbasis!
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
  crash << crash;
//  this->initFromRoots(theVPbasis, 0, theGlobalVariables);
  //this->flagSplitTestModeNoNumerators=true;
//  this->split(theGlobalVariables, ChamberIndicator);
  if(!this->CheckForMinimalityDecompositionWithRespectToRoot(ChamberIndicator, theGlobalVariables))
    crash << crash;
  //return;
  Vector<Rational> tempRoot;
  if (ChamberIndicator!=0)
    tempRoot=*ChamberIndicator;
  else
    tempRoot.MakeZero(this->AmbientDimension);
  if(!this->GetVectorPartitionFunction(output, tempRoot, theGlobalVariables))
  { this->ComputeDebugStringNoNumerator(theGlobalVariables);
    crash << crash;
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
{ if(MultiplicityIndex>=this->Multiplicities.size)
    crash << crash;
  MonomialP tempM;
  output.MakeOne(theExponent.size);
  tempM.MakeOne(theExponent.size);
  for (int i=0; i<theExponent.size; i++)
    tempM[i]=theExponent[i]*this->Elongations[MultiplicityIndex];
  output.AddMonomial(tempM, -1);
}

int oneFracWithMultiplicitiesAndElongations::GetLargestElongation()
{ int result=this->Elongations[0];
  for (int i=1; i<this->Elongations.size; i++)
  { if(this->Elongations[i]==result)
      crash << crash;
    if (this->Elongations[i]>result)
      result= this->Elongations[i];
  }
  return result;
}

int oneFracWithMultiplicitiesAndElongations::GetLCMElongations()
{ int result =1;
  for (int i=0; i<this->Elongations.size; i++)
  { if(this->Elongations[i]==0)
      crash << crash;
    result=MathRoutines::lcm(this->Elongations[i], result);
  }
  return result;
}

int oneFracWithMultiplicitiesAndElongations::GetTotalMultiplicity() const
{ int result=0;
  for (int i=0; i<this->Elongations.size; i++)
    result+= this->Multiplicities[i];
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
  this->Multiplicities[ElongationIndex]+=MultiplicityIncrement;
  if(!this->Multiplicities[ElongationIndex]>=0)
    crash << crash;
  if (this->Multiplicities[ElongationIndex]==0)
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
  { if(this->Multiplicities[i]!=0)
      crash << crash;
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

bool SelectionWithMaxMultiplicity::IncrementReturnFalseIfPastLast()
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
      crash << "This is a programming error: I was asked to enumerate all subsets of a multi-set, however the number of subsets is larger than  "
      << " the maximum value allowed for int on the system (on a 32 bit machine that is around  2 billion). This can be fixed, however I do not have time at the moment. If you "
      << " encounter this error, write me an email and I will take the time to fix this issue. "
      << crash;
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
    this->Multiplicities[i]=0;
}

void SelectionWithDifferentMaxMultiplicities::IncrementSubset()
{ for (int i=this->Multiplicities.size-1; i>=0; i--)
    if (this->Multiplicities[i]<this->MaxMultiplicities[i])
    { if (this->Multiplicities[i]==0)
        this->elements.AddOnTop(i);
      this->Multiplicities[i]++;
      return;
    }else
    { this->Multiplicities[i]=0;
      this->elements.RemoveFirstOccurenceSwapWithLast(i);
    }
}

void DynkinType::GetTypesWithMults(List<DynkinSimpleType>& output)const
{ output.SetSize(0);
  output.ReservE(this->GetNumSimpleComponents());
  List<DynkinSimpleType> componentsSorted;
  this->GetSortedDynkinTypes(componentsSorted);
  for (int i=0; i<componentsSorted.size; i++)
  { int theIndex= this->theMonomials.GetIndex(componentsSorted[i]);
    for (int j=0; j<this->GetMult(theIndex); j++)
      output.AddOnTop(componentsSorted[i]);
  }
}

void DynkinType::GetOuterAutosGeneratorsOneTypeActOnVectorColumn(List<MatrixTensor<Rational> >& output, const DynkinSimpleType& theType, int multiplicity)
{ MacroRegisterFunctionWithName("DynkinType::GetOuterAutosGeneratorsOneTypeActOnVectorColumn");
  output.SetSize(0);
  MatrixTensor<Rational> directSummand, finalMat;
  if (theType.theLetter=='D' || (theType.theLetter=='A'&& theType.theRank>1) || (theType.theLetter=='E' && theType.theRank==6))
  { directSummand.MakeId(theType.theRank*(multiplicity-1));
    int numGens=1;
    if (theType.theLetter=='D' && theType.theRank==4)
      numGens=2;
    for (int i=1; i<numGens+1; i++)
    { theType.GetAutomorphismActingOnVectorColumn(finalMat, i);
      finalMat.DirectSumWith(directSummand);
      output.AddOnTop(finalMat);
    }
  }
  if (multiplicity<2)
    return;
  for (int i=0; i<multiplicity-1; i++)
  { directSummand.MakeZero();
    for (int j=0; j<theType.theRank; j++)
    { directSummand.AddMonomial(MonomialMatrix(j, theType.theRank+j),1);
      directSummand.AddMonomial(MonomialMatrix(theType.theRank+j,j),1);
    }
    finalMat.MakeId(i*theType.theRank);
    finalMat.DirectSumWith(directSummand);
    directSummand.MakeId((multiplicity-2-i)*theType.theRank);
    finalMat.DirectSumWith(directSummand);
    output.AddOnTop(finalMat);
  }
}

void DynkinType::GetOuterAutosGeneratorsActOnVectorColumn(List<MatrixTensor<Rational> >& output)
{ MacroRegisterFunctionWithName("DynkinType::GetOuterAutosGenerators");
  this->SortTheDynkinTypes();
  List<MatrixTensor<Rational> > intermediateGenerators;
  MatrixTensor<Rational> matrixFinal, matrixToGo;
  int currentMult;
  output.SetSize(0);
  int numRowsSoFar=0;
  for (int i=0; i<this->size(); i++)
  { if (!this->theCoeffs[i].IsSmallInteger(&currentMult))
      crash << crash;
    this->GetOuterAutosGeneratorsOneTypeActOnVectorColumn(intermediateGenerators,(*this)[i], currentMult);
    matrixToGo.MakeId(this->GetRank()-numRowsSoFar-currentMult*(*this)[i].theRank);
    for (int j=0; j<intermediateGenerators.size; j++)
    { matrixFinal.MakeId(numRowsSoFar);
      matrixFinal.DirectSumWith(intermediateGenerators[j]);
      matrixFinal.DirectSumWith(matrixToGo);
      output.AddOnTop(matrixFinal);
    }
    numRowsSoFar+=currentMult*(*this)[i].theRank;
  }
  if (output.size==0)
  { output.SetSize(1);
    output[0].MakeId(this->GetRank());
  }
}

void DynkinType::GetLettersTypesMults
(List<char>* outputLetters, List<int>* outputRanks, List<int>* outputMults, List<Rational>* outputFirstCoRootLengthsSquared)const
{ if (outputLetters!=0)
    outputLetters->SetSize(0);
  if (outputRanks!=0)
    outputRanks->SetSize(0);
  if (outputMults!=0)
    outputMults->SetSize(0);
  if (outputFirstCoRootLengthsSquared!=0)
    outputFirstCoRootLengthsSquared->SetSize(0);
  List<DynkinSimpleType> componentsSorted;
  this->GetSortedDynkinTypes(componentsSorted);
  for (int i=0; i<componentsSorted.size; i++)
  { int theIndex= this->theMonomials.GetIndex(componentsSorted[i]);
    if (outputLetters!=0)
      outputLetters->AddOnTop((*this)[theIndex].theLetter);
    if (outputRanks!=0)
      outputRanks->AddOnTop((*this)[theIndex].theRank);
    if (outputFirstCoRootLengthsSquared!=0)
      outputFirstCoRootLengthsSquared->AddOnTop((*this)[theIndex].CartanSymmetricInverseScale);
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

bool DynkinType::CanBeExtendedParabolicallyOrIsEqualTo(const DynkinType& other)const
{ if ((*this)==other)
    return true;
  return this->CanBeExtendedParabolicallyTo(other);
}

bool DynkinType::CanBeExtendedParabolicallyTo(const DynkinType& other)const
{ MacroRegisterFunctionWithName("DynkinType::CanBeExtendedParabolicallyTo");
  //stOutput << "<br>computing whether  " << this->ToString() << " can be extended parabolically to "
  //<< other.ToString();
  if (other.IsEqualToZero())
  { //stOutput << "... it can't";
    return false;
  }
  if (this->IsEqualToZero())
  { //stOutput << "... it can";
    return true;
  }
  DynkinSimpleType targetType, currentType;
  targetType=other[0];
  DynkinType remainderThis, remainderOther;
  //stOutput << "<hr>Testing whether " << this->ToString() << " fits in " << other.ToString();
  for (int i=0; i< this->size(); i++)
  { currentType=(*this)[i];
    if (currentType.CanBeExtendedParabolicallyTo(targetType) || currentType==targetType)
    { remainderThis=*this;
      remainderThis.SubtractMonomial(currentType, 1);
      remainderOther= other;
      remainderOther.SubtractMonomial(targetType, 1);
      if (currentType==targetType)
      { if (remainderThis.CanBeExtendedParabolicallyTo(remainderOther))
        { //stOutput << " it can";
          return true;
        }
      } else
        if (remainderThis.CanBeExtendedParabolicallyOrIsEqualTo(remainderOther))
        { //stOutput << " it can";
          return true;
        }
    }
  }
  //stOutput << "...it cant";
  return false;
}

bool DynkinType::Grow
(const List<Rational>& allowedInverseScales, int AmbientWeylDim, List<DynkinType>& output, List<List<int> >* outputPermutationRoots)const
{ MacroRegisterFunctionWithName("DynkinType::Grow");
  //if (this->ToString()=="2A^{1}_1[A^{1}_1]")
    //stOutput << "<br>Here be I!!!!" << this->ToString();
  output.SetSize(0);
  if (outputPermutationRoots!=0)
    outputPermutationRoots->SetSize(0);
  if (this->GetRank()>=AmbientWeylDim)
    return true;
  if (this->IsEqualToZero())
  { output.SetSize(allowedInverseScales.size);
    if (outputPermutationRoots!=0)
      outputPermutationRoots->SetSize(allowedInverseScales.size);
    for (int i=0; i<allowedInverseScales.size; i++)
    { output[i].MakeSimpleType('A', 1, &allowedInverseScales[i]);
      if (outputPermutationRoots!=0)
      { (*outputPermutationRoots)[i].SetSize(1);
        (*outputPermutationRoots)[i][0]=0;
      }
    }
    return true;
  }
  //  Rational minCoRootLengthSquared=-1;
  //growth is allowed from the minimal component only
  int indexMinComponentByLengthAndSimpleType=0;
  for (int i=1; i<this->size(); i++)
    if ((*this)[indexMinComponentByLengthAndSimpleType]>(*this)[i])
      indexMinComponentByLengthAndSimpleType=i;
  DynkinType typeMinusMin=(*this);
  typeMinusMin.SubtractMonomial((*this)[indexMinComponentByLengthAndSimpleType], 1);
  List<DynkinSimpleType> theSimpleTypes;
  List<List<int> > lastComponentRootInjections;
  (*this)[indexMinComponentByLengthAndSimpleType].Grow(theSimpleTypes, &lastComponentRootInjections);
  List<int> currentRootInjection;
  currentRootInjection.SetSize(this->GetRank()+1);
  for (int i=0; i<theSimpleTypes.size; i++)
  { bool isGood=true;
    for (int j=0; j<typeMinusMin.size(); j++)
      if (theSimpleTypes[i]>typeMinusMin[j])
      { isGood=false;
        break;
      }
    if (!isGood)
      continue;
    output.AddOnTop(typeMinusMin);
    output.LastObject()->AddMonomial(theSimpleTypes[i],1);
    if (outputPermutationRoots!=0)
    { int baseTypeRank=typeMinusMin.GetRank();
      for (int j=0; j<baseTypeRank; j++)
        currentRootInjection[j]=j;
      for (int j=0; j<lastComponentRootInjections[i].size; j++)
        currentRootInjection[j+baseTypeRank]=lastComponentRootInjections[i][j]+baseTypeRank;
      outputPermutationRoots->AddOnTop(currentRootInjection);
    }
  }
  for (int i=0; i<allowedInverseScales.size; i++)
    if (allowedInverseScales[i]<=(*this)[indexMinComponentByLengthAndSimpleType].CartanSymmetricInverseScale)
    { output.SetSize(output.size+1);
      output.LastObject()->MakeSimpleType('A', 1, &allowedInverseScales[i]);
      *output.LastObject()+=*this;
      if (outputPermutationRoots!=0)
      { for (int j=0; j<currentRootInjection.size; j++)
          currentRootInjection[j]=j;
        outputPermutationRoots->AddOnTop(currentRootInjection);
      }
    }
  //if (this->ToString()=="2A^{1}_1[A^{1}_1]")
    //stOutput << "<br>output be: " << output.ToString();
  return true;
}

int DynkinType::GetIndexPreimageFromRootInjection(int inputIndex, const List<int>& inputRootInjection)
{ MacroRegisterFunctionWithName("DynkinType::GetIndexPreimageFromRootInjection");
  for (int i=0; i<inputRootInjection.size; i++)
    if (inputRootInjection[i]==inputIndex)
      return i;
  crash.theCrashReport << "This is a programming error: asking to find the preimage of root index " << inputIndex << " w.r.t. root injection "
  << inputRootInjection << " - the root index has no preimage. This function is not allowed to fail. ";
  crash << crash;
  return -1;
}

void DynkinType::MakeSimpleType(char type, int rank, const Rational* inputFirstCoRootSqLength)
{ DynkinSimpleType theMon;
  Rational cartanSymmetricInvScale= (inputFirstCoRootSqLength==0 ? 1 : *inputFirstCoRootSqLength);
  theMon.MakeArbitrary(type, rank, cartanSymmetricInvScale);
  this->MakeZero();
  this->AddMonomial(theMon, 1);
}

bool DynkinType::HasExceptionalComponent()const
{ for (int i=0; i<this->size(); i++)
    if ((*this)[i].theLetter=='E'|| (*this)[i].theLetter=='F'|| (*this)[i].theLetter=='G')
      return true;
  return false;
}

bool DynkinType::ContainsType(char theTypeLetter)const
{ for (int i=0; i<this->size(); i++)
    if ((*this)[i].theLetter==theTypeLetter)
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
    *outputLength=theMon.CartanSymmetricInverseScale;
  return true;
}

int DynkinType::GetNumSimpleComponents()const
{ Rational result=0;
  for (int i=0; i<this->size(); i++)
    result+=this->theCoeffs[i];
  int output;
  if (!result.IsSmallInteger(&output))
    crash << "This is a programming error: Dynkin type has a number of simple components which is not a small integer. " << crash;
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
    crash << "This is a programming error: attempting to get a small integer rank from a Dynkin type whose rank is not a small integer, but is instead "
    << tempRat.ToString() << ". " << crash;
  return result;
}

void DynkinType::GetEpsilonMatrix(Matrix<Rational>& output)const
{ output.init(0,0);
  Matrix<Rational> curCartan;
  List<DynkinSimpleType> sortedMons;
  this->GetSortedDynkinTypes(sortedMons);
  for (int j=0; j<sortedMons.size; j++)
  { int theIndex=this->theMonomials.GetIndex(sortedMons[j]);
    int theMult=this->GetMult(theIndex);
    for (int k=0; k<theMult; k++)
    { DynkinSimpleType::GetEpsilonMatrix((*this)[theIndex].theLetter, (*this)[theIndex].theRank, curCartan);
      output.DirectSumWith(curCartan);
    }
  }
}

void DynkinType::SortTheDynkinTypes()
{ List<DynkinSimpleType> sortedTypes;
  this->GetSortedDynkinTypes(sortedTypes);
  DynkinType finalMe;
  finalMe.MakeZero();
  for (int i=0; i<sortedTypes.size; i++)
    finalMe.AddMonomial(sortedTypes[i], this->GetMonomialCoefficient(sortedTypes[i]));
  *this=finalMe;
}

void DynkinType::GetSortedDynkinTypes(List<DynkinSimpleType>& output)const
{ output=this->theMonomials;
  output.QuickSortDescending();
}

void DynkinType::GetCartanSymmetric(Matrix<Rational>& output)const
{ MacroRegisterFunctionWithName("DynkinType::GetCartanSymmetric");
  output.init(0,0);
  Matrix<Rational> curCartan;
  List<DynkinSimpleType> sortedMons;
  this->GetSortedDynkinTypes(sortedMons);
  for (int j=0; j<sortedMons.size; j++)
  { int theIndex=this->theMonomials.GetIndex(sortedMons[j]);
    int mult=this->GetMult(theIndex);
    for (int k=0; k<mult; k++)
    { (*this)[theIndex].GetCartanSymmetric(curCartan);
      output.DirectSumWith(curCartan);
    }
  }
}

void DynkinType::GetCoCartanSymmetric(Matrix<Rational>& output)const
{ MacroRegisterFunctionWithName("DynkinType::GetCartanSymmetric");
  Matrix<Rational> curCartan;
  this->GetCartanSymmetric(curCartan);
  WeylGroup::GetCoCartanSymmetric(curCartan, output);
}

void DynkinType::GetCartanSymmetricDefaultLengthKeepComponentOrder(Matrix<Rational>& output)const
{ MacroRegisterFunctionWithName("DynkinType::GetCartanSymmetricDefaultLengthKeepComponentOrder");
  output.init(0,0);
  Matrix<Rational> curCartan;
  List<DynkinSimpleType> sortedMons;
  this->GetSortedDynkinTypes(sortedMons);
  DynkinSimpleType currentType;
  for (int j=0; j<sortedMons.size; j++)
  { int theIndex=this->theMonomials.GetIndex(sortedMons[j]);
    int mult=this->GetMult(theIndex);
    currentType.MakeArbitrary(sortedMons[j].theLetter, sortedMons[j].theRank, 1);
    currentType.CartanSymmetricInverseScale=1;//=currentType.GetDefaultCoRootLengthSquared(0);
    for (int k=0; k<mult; k++)
    { currentType.GetCartanSymmetric(curCartan);
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
  crash << "if you would like an edge weight of a non-crystallographic Coxeter graph, replace the code near "
  << __FILE__ << ":" << __LINE__ << " with an arccos function. " << crash;
  return -1;
}

LargeInt DynkinType::GetWeylGroupSizeByFormula()const
{ LargeInt result=1;
  LargeInt tempLI;
  for (int i=0; i<this->size(); i++)
  { tempLI=WeylGroup::GetGroupSizeByFormula((*this)[i].theLetter, (*this)[i].theRank);
    tempLI.RaiseToPower(this->GetMult(i));
    result*=tempLI;
  }
  return result;
}

Rational DynkinSimpleType::GetLongRootLengthSquared()const
{ return this->GetDefaultLongRootLengthSquared()/this->CartanSymmetricInverseScale;
}

Rational DynkinSimpleType::GetDefaultLongRootLengthSquared()const
{ switch (this->theLetter)
  { case 'A':
    case 'B':
    case 'D':
    case 'E':
    case 'F':
      return 2;
    case 'G':
      return 6;
    case 'C':
      return 4;
    default:
      break;
  }
  crash << "This is a programming error: calling DynkinSimpleType::GetLongRootLengthSquared on a non-initialized simple type. " << crash;
  return -1;
}

std::string DynkinSimpleType::ToString(FormatExpressions* theFormat)const
{ std::stringstream out;
  bool includeTechnicalNames= theFormat==0 ? true : theFormat->flagIncludeLieAlgebraTypes;
  bool includeNonTechnicalNames=theFormat==0 ? false : theFormat->flagIncludeLieAlgebraNonTechnicalNames;
  bool usePlusesAndExponents= theFormat==0? true: !theFormat->flagDynkinTypeDontUsePlusAndExponent;
  bool supressDynkinIndexOne= theFormat==0 ? false : theFormat->flagSupressDynkinIndexOne;
  if (!includeNonTechnicalNames && !includeTechnicalNames)
    includeTechnicalNames=true;
  bool hasAmbient=false;
  if (theFormat!=0)
    hasAmbient=(theFormat->AmbientWeylLetter!='X');
  if (includeTechnicalNames)
  { if (!hasAmbient)
    { out << theLetter;
      if (!supressDynkinIndexOne || this->CartanSymmetricInverseScale!=1)
      { if (usePlusesAndExponents)
          out << "^{";
        out << this->CartanSymmetricInverseScale.ToString();
        if (usePlusesAndExponents)
          out << "}";
      }
    } else
    { DynkinSimpleType ambientType;
      ambientType.theLetter=theFormat->AmbientWeylLetter;
      ambientType.CartanSymmetricInverseScale=theFormat->AmbientCartanSymmetricInverseScale;
      Rational theDynkinIndex= ambientType.GetLongRootLengthSquared()/this->GetLongRootLengthSquared();
//      (this->CartanSymmetricInverseScale/this->GetDefaultLongRootLengthSquared())/
//      (ambientType.CartanSymmetricInverseScale/ambientType.GetDefaultLongRootLengthSquared());
      out << theLetter;
      if (!supressDynkinIndexOne || theDynkinIndex!=1)
      { if (usePlusesAndExponents)
          out << "^{" ;
        out << theDynkinIndex.ToString();
        if (usePlusesAndExponents)
          out << "}";
      }
    }
    if (this->theRank>=10)
    { out << "_";
      if (usePlusesAndExponents)
        out << "{";
      out << this->theRank;
      if (usePlusesAndExponents)
        out << "}";
    }
    else
      out << "_" << this->theRank;
    //out << "[" << this->theLetter << "^{" << this->CartanSymmetricInverseScale << "}_" << this->theRank << "]";
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

Rational DynkinSimpleType::GetRatioRootSquaredToFirstSquared(int rootIndex)const
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
    crash << "This is a programming error: attempting to get the squared length of simple root number " << rootIndex+1
    << ", however the root system if of rank " << this->theRank << ". " << crash;
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
      crash << "This is a programming error: calling DynkinSimpleType::GetDefaultRootLengthSquared on the non-initialized Dynkin type "
      << this->ToString() << crash;
      return -1;
  }
}

void DynkinSimpleType::GetEpsilonMatrix(char WeylLetter, int WeylRank, Matrix<Rational>& output)
{ if (WeylLetter=='A')
  { output.init(WeylRank+1, WeylRank);
    output.MakeZero();
    for (int i=0; i<WeylRank; i++)
    { output(i,i)=1;
      output(i+1,i)=-1;
    }
    output(WeylRank,WeylRank-1)=-1;
  }
  if (WeylLetter=='B')
  { output.init(WeylRank, WeylRank);
    output.MakeZero();
    for (int i=0; i<WeylRank-1; i++)
    { output(i,i)=1;
      output(i+1,i)=-1;
    }
    output(WeylRank-1, WeylRank-1)=1;
  }
  if (WeylLetter=='C')
  { output.init(WeylRank, WeylRank);
    output.MakeZero();
    for (int i=0; i<WeylRank-1; i++)
    { output(i,i)=1;
      output(i+1,i)=-1;
    }
    output(WeylRank-1,WeylRank-1)=2;
  }
  if (WeylLetter=='D')
  { output.init(WeylRank, WeylRank);
    output.MakeZero();
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
    output.MakeZero();
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
    output.MakeZero();

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
    output.MakeZero();
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
    crash << "This is a programming error: attempting to create type A_n with n=" << n << " is illegal. If this was a bad user input, it should "
    << " be handled at an earlier stage. " << crash;
  output.init(n, n);
  output.MakeZero();
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

bool DynkinSimpleType::CanBeExtendedParabolicallyTo(const DynkinSimpleType& other)const
{ MacroRegisterFunctionWithName("DynkinSimpleType::CanBeExtendedParabolicallyTo");
  //stOutput << "<br>checking whether " << this->ToString() << " can be extended to " << other.ToString();
  if (this->CartanSymmetricInverseScale!=other.CartanSymmetricInverseScale)
    return false;
  if (other.theRank<=this->theRank)
    return false;
  if (other.theLetter=='F')
  { if (this->theLetter=='A' && this->theRank<3)
      return true;
    if (this->theLetter=='B' && this->theRank==3)
      return true;
    return false;
  }
  if (other.theLetter=='E')
  { if (this->theRank<5)
      return this->theLetter=='A';
    if (this->theRank==5)
      return this->theLetter=='D';
    return this->theLetter=='E';
  }
  return this->theLetter=='A';
}

void DynkinSimpleType::Grow(List<DynkinSimpleType>& output, List<List<int> >* outputPermutationRoots)const
{ MacroRegisterFunctionWithName("DynkinSimpleType::Grow");
  //almost all simple types are grown from type A. Exceptions only for types F4 (grown from C_3),
  //E6 (grown from D_5), E7 (grown from E6) and E8 (grown from E7).
  output.SetSize(0);
  List<int> currentImagesSimpleRootsCurrent;
  if (outputPermutationRoots!=0)
    outputPermutationRoots->SetSize(0);
  currentImagesSimpleRootsCurrent.SetSize(this->theRank+1);
  DynkinSimpleType newType;
  if (this->theLetter=='B' && this->theRank==3)
  { newType.MakeArbitrary('F', 4, this->CartanSymmetricInverseScale);
    output.AddOnTop(newType);
    if (outputPermutationRoots!=0)
    { for (int i=0; i<currentImagesSimpleRootsCurrent.size; i++)
        currentImagesSimpleRootsCurrent[i]=i;
      outputPermutationRoots->AddOnTop(currentImagesSimpleRootsCurrent);
    }
  }
  if (this->theLetter=='D' && this->theRank==5)
  { newType.MakeArbitrary('E', 6, this->CartanSymmetricInverseScale);
    output.AddOnTop(newType);
    if (outputPermutationRoots!=0)
    { currentImagesSimpleRootsCurrent[0]=0;
      currentImagesSimpleRootsCurrent[1]=2;
      currentImagesSimpleRootsCurrent[2]=3;
      currentImagesSimpleRootsCurrent[3]=4;
      currentImagesSimpleRootsCurrent[4]=1;
      currentImagesSimpleRootsCurrent[5]=5;
      outputPermutationRoots->AddOnTop(currentImagesSimpleRootsCurrent);
    }
  }
  if (this->theLetter=='E' && this->theRank<8)
  { newType.MakeArbitrary('E', this->theRank+1, this->CartanSymmetricInverseScale);
    output.AddOnTop(newType);
    if (outputPermutationRoots!=0)
    { for (int i=0; i<currentImagesSimpleRootsCurrent.size; i++)
        currentImagesSimpleRootsCurrent[i]=i;
      outputPermutationRoots->AddOnTop(currentImagesSimpleRootsCurrent);
    }
  }
  if (this->theLetter!='A')
    return;
  newType.MakeArbitrary(this->theLetter, this->theRank+1, this->CartanSymmetricInverseScale);
  output.AddOnTop(newType);
  if (outputPermutationRoots!=0)
  { for (int i=0; i<currentImagesSimpleRootsCurrent.size; i++)
      currentImagesSimpleRootsCurrent[i]=i;
    outputPermutationRoots->AddOnTop(currentImagesSimpleRootsCurrent);
  }
  newType.MakeArbitrary('B', this->theRank+1, this->CartanSymmetricInverseScale);
  output.AddOnTop(newType);
  if (outputPermutationRoots!=0)
    outputPermutationRoots->AddOnTop(currentImagesSimpleRootsCurrent);
  if (this->theRank>1)
  { newType.MakeArbitrary('C', this->theRank+1, this->CartanSymmetricInverseScale);
    output.AddOnTop(newType);
    if (outputPermutationRoots!=0)
      outputPermutationRoots->AddOnTop(currentImagesSimpleRootsCurrent);
  }
  if (this->theRank>2)
  { newType.MakeArbitrary('D', this->theRank+1, this->CartanSymmetricInverseScale);
    output.AddOnTop(newType);
    if (outputPermutationRoots!=0)
      outputPermutationRoots->AddOnTop(currentImagesSimpleRootsCurrent);
  }
  if (this->theRank==1)
  { newType.MakeArbitrary('G', 2, this->CartanSymmetricInverseScale);
    output.AddOnTop(newType);
    if (outputPermutationRoots!=0)
      outputPermutationRoots->AddOnTop(currentImagesSimpleRootsCurrent);
  }
}

bool DynkinSimpleType::operator>(const DynkinSimpleType& other)const
{ if (this->CartanSymmetricInverseScale>other.CartanSymmetricInverseScale)
    return true;
  if (this->CartanSymmetricInverseScale<other.CartanSymmetricInverseScale)
    return false;
  if (this->theRank>other.theRank)
    return true;
  if (this->theRank<other.theRank)
    return false;
  if ((this->theLetter=='B' ||this->theLetter=='C') && other.theLetter=='D')
    return true;
  if (this->theLetter=='D' && (other.theLetter=='B' ||other.theLetter=='C'))
    return false;
  return this->theLetter>other.theLetter;
//  if (this->theLetter>other.theLetter)
//    return true;
//  if (this->theLetter<other.theLetter)
//    return false;
//  return this->CartanSymmetricScale>other.CartanSymmetricScale;
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

void DynkinSimpleType::GetCoCartanSymmetric(Matrix<Rational>& output)const
{ Matrix<Rational> currentCartan;
  this->GetCartanSymmetric(currentCartan);
  WeylGroup::GetCoCartanSymmetric(currentCartan, output);
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
      crash << "This is a programming error: requesting DynkinSimpleType::GetCartanSymmetric from a non-initialized Dynkin simple type. " << crash;
      break;
  }
  output/=this->CartanSymmetricInverseScale;
}

Rational DynkinSimpleType::GetRatioLongRootToFirst(char inputWeylLetter, int inputRank)
{ switch (inputWeylLetter)
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
  crash << "This is a programming error. This is a portion of code that should never be reached. Something has gone very wrong. " << crash;
}

bool DynkinSimpleType::operator<(int otherRank)const
{ return this->theRank<otherRank;
}

void WeylGroup::SimpleReflectionDualSpace(int index, Vector<Rational>& DualSpaceElement)
{ Rational coefficient, tempRat;
  coefficient.Assign(DualSpaceElement[index]);
  coefficient.DivideBy(this->CartanSymmetric.elements[index][index]);
  for (int i=0; i<this->CartanSymmetric.NumCols; i++)
  { tempRat.Assign(coefficient);
    tempRat.MultiplyBy(this->CartanSymmetric.elements[index][i]*(-2));
    DualSpaceElement[i]+=(tempRat);
  }
}

ElementWeylGroup<WeylGroup> WeylGroup::GetRootReflection(int rootIndex)
{ if (!this->rho.size==0)
    this->ComputeRho(true);
  Vector<Rational> rhoImage;
  this->ReflectBetaWRTAlpha(this->RootSystem[rootIndex], this->rho, false, rhoImage);
  ElementWeylGroup<WeylGroup> result;
  result.MakeFromRhoImage(rhoImage, *this);
  return result;
}

void WeylGroup::SimpleReflectionRoot(int index, Vector<Rational>& theRoot, bool RhoAction, bool UseMinusRho)
{// if (this->CartanSymmetric.elements[index][index].IsEqualToZero())
  //  return;
  Rational alphaShift, tempRat;
  alphaShift.MakeZero();
  for (int i=0; i<this->CartanSymmetric.NumCols; i++)
  { tempRat=(theRoot[i]);
    tempRat.MultiplyBy(this->CartanSymmetric.elements[index][i]*(-2));
    alphaShift+=(tempRat);
  }
  alphaShift.DivideBy(this->CartanSymmetric.elements[index][index]);
  if (RhoAction)
  { if(UseMinusRho)
      alphaShift.AddInteger(1);
    else
      alphaShift.AddInteger(-1);
  }
  theRoot[index]+=(alphaShift);
}

void WeylGroup::SimpleReflectionRootAlg(int index, PolynomialSubstitution<Rational>& theRoot, bool RhoAction)
{ int lengthA=this->CartanSymmetric.elements[index][index].NumShort;
  Polynomial<Rational> AscalarB, tempP;
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

Matrix<Rational> WeylGroup::GetMatrixStandardRep(int elementIndex)const
{ Matrix<Rational> result;
  this->GetMatrixStandardRep(this->theElements[elementIndex], result);
  return result;
}

void WeylGroup::init()
{ this->flagFundamentalToSimpleMatricesAreComputed=false;
  this->flagAllOuterAutosComputed=false;
  this->flagOuterAutosGeneratorsComputed=false;
  this->flagIrrepsAreComputed=false;
  this->flagCharTableIsComputed=false;
  this->MatrixSendsSimpleVectorsToEpsilonVectors.FreeMemory();
  this->::FiniteGroup<ElementWeylGroup<WeylGroup> >::init();
}

void WeylGroup::ActOnAffineHyperplaneByGroupElement(int index, affineHyperplane<Rational>& output, bool RhoAction, bool UseMinusRho)
{ int numGens= this->theElements[index].generatorsLastAppliedFirst.size;
  for (int i=numGens-1; i>=0; i--)
  { this->SimpleReflectionRoot(this->theElements[index].generatorsLastAppliedFirst[i], output.affinePoint, RhoAction, UseMinusRho);
//    output.affinePoint.ComputeDebugString();
    this->SimpleReflectionDualSpace(this->theElements[index].generatorsLastAppliedFirst[numGens-i-1], output.normal);
  }
}

void WeylGroup::GetSignCharacter(Vector<Rational>& out)
{ if(!this->flagCCRepresentativesComputed)
    this->ComputeCCSizesAndRepresentatives(0);
  out.SetSize(this->ConjugacyClassCount());
  for(int i=0; i<this->ConjugacyClassCount(); i++)
    out[i]= this->conjugacyClasseS[i].representative.Sign();
}

void SubgroupWeylGroup::GetSignCharacter(Vector<Rational>& out)
{ MacroRegisterFunctionWithName("SubgroupRootReflections::GetSignCharacter");
  if(!this->flagCCRepresentativesComputed)
    this->ComputeCCSizesAndRepresentatives(0);
  out.SetSize(ConjugacyClassCount());
  for(int i=0; i<this->ConjugacyClassCount(); i++)
    out[i] = this->conjugacyClasseS[i].representative.Sign();
//  stOutput << "<br>Sign character is: " << out.ToString();
}

void WeylGroup::GetTrivialRepresentation(WeylGroupRepresentation<Rational>& output)
{ MacroRegisterFunctionWithName("WeylGroup::GetTrivialRepresentation");
  this->CheckInitializationConjugacyClasses();
  output.init(*this);
  output.basis.MakeEiBasis(1);
  for(int i=0; i<this->GetDim(); i++)
    output.generatorS[i].MakeIdMatrix(1);
  output.GetCharacter();
}

void WeylGroup::GetSignRepresentation(WeylGroupRepresentation<Rational>& output)
{ MacroRegisterFunctionWithName("WeylGroup::GetSignRepresentation");
  this->CheckInitializationConjugacyClasses();
  output.init(*this);
  output.basis.MakeEiBasis(1);
  ElementWeylGroup<WeylGroup> currentElt;
  for(int i=0; i<this->GetDim(); i++)
  { currentElt.MakeSimpleReflection(i, *this);
    output.generatorS[i].MakeIdMatrix(1);
    output.generatorS[i]*=currentElt.Sign();
  }
  output.GetCharacter();
}

void WeylGroup::GetStandardRepresentation(WeylGroupRepresentation<Rational>& output)
{ MacroRegisterFunctionWithName("WeylGroup::GetStandardRepresentation");
  this->CheckInitializationConjugacyClasses();
  output.init(*this);
  output.basis.MakeEiBasis(this->GetDim());
  for(int i=0; i<this->GetDim(); i++)
    this->GetSimpleReflectionMatrix(i, output.generatorS[i]);
//  stOutput << output.ToString();
  output.GetCharacter();
//  stOutput << "The char: " << output.theCharacteR.ToString();
}

void WeylGroup::GetStandardRepresentationMatrix(int g, Matrix<Rational>& output) const
{ // the rank is the number of rows of the Cartan form
  output.init(this->CartanSymmetric.NumRows,this->CartanSymmetric.NumRows);
  for(int i=0; i<this->CartanSymmetric.NumRows; i++)
  { Vector<Rational> v;
    v.MakeEi(this->CartanSymmetric.NumRows,i);
    this->ActOn(g,v);
    for(int j=0; j<this->CartanSymmetric.NumRows; j++)
      output.elements[j][i] = v[j];
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

void WeylGroup::PerturbWeightToRegularWRTrootSystem(const Vector<Rational>& inputH, Vector<Rational>& output)
{ output=(inputH);
  int indexFirstNonRegular;
  while(!this->IsRegular(output, &indexFirstNonRegular))
  { const Vector<Rational>& theBadRoot= this->RootSystem[indexFirstNonRegular];
    Rational maxMovement=0;
    Rational tempRat1, tempRat2, tempMaxMovement;
    for (int i=0; i<this->RootsOfBorel.size; i++)
    { this->RootScalarCartanRoot(theBadRoot, this->RootsOfBorel[i], tempRat1);
      this->RootScalarCartanRoot(output, this->RootsOfBorel[i], tempRat2);
      if ((!tempRat1.IsEqualToZero()) && (!tempRat2.IsEqualToZero()))
      { tempMaxMovement = tempRat2/tempRat1;
        tempMaxMovement.AssignAbsoluteValue();
        if ((tempMaxMovement< maxMovement) || maxMovement.IsEqualToZero())
          maxMovement = tempMaxMovement;
      }
    }
    int tempInt=2;
    if (this->RootScalarCartanRoot(theBadRoot, inputH).IsNegative())
      tempInt=-2;
    output+=theBadRoot*maxMovement/tempInt;
  }
}

bool WeylGroup::IsRegular(Vector<Rational>& input, int* indexFirstPerpendicularRoot)
{ if (indexFirstPerpendicularRoot!=0)
    *indexFirstPerpendicularRoot=-1;
  for (int i=0; i<this->RootSystem.size; i++)
    if (this->RootScalarCartanRoot(input, this->RootSystem[i]).IsEqualToZero())
    { if (indexFirstPerpendicularRoot!=0)
        *indexFirstPerpendicularRoot=i;
      return false;
    }
  return true;
}

LargeInt WeylGroup::GetGroupSizeByFormula(char weylLetter, int theDim)
{ //Humphreys, Introduction to Lie algebras and representation theory(1980), page 66, Table 1
  LargeInt theOutput=1;
  if (weylLetter=='A')
    theOutput= Rational::Factorial(theDim+1);
  if (weylLetter=='B' || weylLetter=='C')
    theOutput= Rational::Factorial(theDim)*Rational::TwoToTheNth(theDim);
  if (weylLetter=='D')
    theOutput= Rational::Factorial(theDim)*Rational::TwoToTheNth(theDim-1);
  if (weylLetter=='E')
  { if (theDim==6)
      theOutput= 51840;
    if (theDim==7)
    { theOutput=1024;
      theOutput*=81*35;
    }
    if (theDim==8)
    { theOutput=1024*16;
      theOutput*=81*3;
      theOutput*=25*7;
    }
  }
  if (weylLetter=='F')
    theOutput=128*9;
  if (weylLetter=='G')
    theOutput=12;
  return theOutput;
}

int WeylGroup::Invert(int g) const
{ return this->theElements.GetIndex(this->theElements[g].Inverse());
}

void WeylGroup::GetGeneratorList(int g, List<int>& out) const
{ out = this->theElements[g].generatorsLastAppliedFirst;
}

bool WeylGroup::operator==(const WeylGroup& other)const
{ return this->CartanSymmetric==other.CartanSymmetric && this->theDynkinType==other.theDynkinType;
}

void WeylGroup::ActOnRootByGroupElement(int index, Vector<Rational>& theRoot, bool RhoAction, bool UseMinusRho)
{ const ElementWeylGroup<WeylGroup>& currentElt=this->theElements[index];
  for (int i=currentElt.generatorsLastAppliedFirst.size-1; i>=0; i--)
    this->SimpleReflectionRoot(currentElt.generatorsLastAppliedFirst[i], theRoot, RhoAction, UseMinusRho);
}

void WeylGroup::GetCoCartanSymmetric(const Matrix<Rational>& input, Matrix<Rational>& output)
{ MacroRegisterFunctionWithName("DynkinType::GetCoCartanSymmetric");
  if (&input==&output)
  { Matrix<Rational> inputCopy=input;
    WeylGroup::GetCoCartanSymmetric(inputCopy, output);
    return;
  }
  output.init(input.NumRows, input.NumCols);
  for (int i=0; i<input.NumRows; i++)
    for (int j=0; j<input.NumCols; j++)
      output(i,j)= input(i,j)*4/(input(i,i)*input(j,j));
}

void WeylGroup::GenerateRootSystem()
{ Vectors<Rational> startRoots;
  HashedList<Vector<Rational> > theRootsFinder;
  startRoots.MakeEiBasis(this->GetDim());
  int estimatedNumRoots=this->theDynkinType.GetRootSystemSize();
//  stOutput << "<hr><hr>Generating root system, startroots: " << startRoots.ToString();
  this->GenerateOrbit(startRoots, false, theRootsFinder, false, estimatedNumRoots);
//  stOutput << " final roots: " << theRootsFinder.ToString() << "<hr>";
  this->RootSystem.Clear();
  this->RootSystem.SetExpectedSize(theRootsFinder.size);
  this->RootsOfBorel.SetSize(0);
  this->RootsOfBorel.ReservE(theRootsFinder.size/2);
  for (int i=0; i<theRootsFinder.size; i++)
    if (theRootsFinder[i].IsPositiveOrZero())
      this->RootsOfBorel.AddOnTop(theRootsFinder[i]);
  this->RootsOfBorel.QuickSortAscending();
  for (int i=this->RootsOfBorel.size-1; i>=0; i--)
    this->RootSystem.AddOnTop(-this->RootsOfBorel[i]);
  for (int i=0; i<this->RootsOfBorel.size; i++)
    this->RootSystem.AddOnTop(this->RootsOfBorel[i]);
}

void WeylGroup::ActOnRootAlgByGroupElement(int index, PolynomialSubstitution<Rational>& theRoot, bool RhoAction)
{ for (int i=this->theElements[index].generatorsLastAppliedFirst.size-1; i>=0; i--)
    this->SimpleReflectionRootAlg(this->theElements[index].generatorsLastAppliedFirst[i], theRoot, RhoAction);
}

void WeylGroup::ComputeWeylGroupAndRootsOfBorel(Vectors<Rational>& output)
{ this->ComputeAllElements();
  output.size=0;
  output.ReservE(this->RootSystem.size/2);
  for (int i=0; i<this->RootSystem.size; i++)
    if (this->RootSystem[i].IsPositiveOrZero())
      output.AddOnTop(this->RootSystem[i]);
}

bool WeylGroup::LeftIsHigherInBruhatOrderThanRight(ElementWeylGroup<WeylGroup>& left, ElementWeylGroup<WeylGroup>& right)
{ Vector<Rational> leftImage; leftImage=this->rho;
  Vector<Rational> rightImage; rightImage=this->rho;
  this->ActOn(left, leftImage);
  this->ActOn(right, rightImage);
  return (rightImage-leftImage).IsPositiveOrZero() && !(rightImage-leftImage).IsEqualToZero();
}

void WeylGroup::ComputeRootsOfBorel(Vectors<Rational>& output)
{ output.size=0;
  this->RootSystem.Clear();
  this->GenerateRootSystem();
  output=(this->RootsOfBorel);
}

std::string WeylGroup::ToStringCppCharTable(FormatExpressions* theFormat)
{ MacroRegisterFunctionWithName("WeylGroup::ToStringCppConjugacyClasses");
  if (!this->flagCharTableIsComputed)
    return "<br>Conjugacy classes not computed";
  std::stringstream out;
  out << "<hr>Here is the c++ input code for the char table.";
  out << "<br>";
  FormatExpressions theFormatNoDynkinTypePlusesExponents;
  theFormatNoDynkinTypePlusesExponents.flagDynkinTypeDontUsePlusAndExponent=true;
  out << "bool LoadCharTable" << this->theDynkinType.ToString(&theFormatNoDynkinTypePlusesExponents) << "(WeylGroup& output)\n<br>{ ";
  out << " output.characterTable.SetExpectedSize(" << this->size().ToString() << "); output.characterTable.SetSize(0);";
  out << "\n<br>&nbsp;&nbsp;ClassFunction&lt;FiniteGroup&lt;ElementWeylGroup&lt;WeylGroup&gt; &gt;, Rational&gt; currentCF;";
  out << "\n<br>&nbsp;&nbsp;currentCF.G=&output;";
  for (int i=0; i<this->characterTable.size; i++)
  { out << "\n<br>&nbsp;&nbsp;currentCF.data.AssignString(\"";
    out << "(";
    //Print vector ensuring every number is at least 3 characters wide. (3 should suffice for E8... or does it?)
    for (int j=0; j<this->characterTable[i].data.size; j++)
    { std::string theNumber= this->characterTable[i].data[j].ToString();
      out << theNumber;
      for (int k=theNumber.size(); k<3; k++)
        out << "&nbsp;";
      if (j!=this->characterTable[i].data.size-1)
        out << ", ";
    }
    out << ")";
    out << "\"); output.characterTable.AddOnTop(currentCF);";
  }
  out << "\n<br>&nbsp;&nbsp;output.irrepsCarterLabels.SetSize(0);";
  for (int i=0; i<this->irrepsCarterLabels.size; i++)
    out << "\n<br>&nbsp;&nbsp;output.irrepsCarterLabels.AddOnTop(\"" << this->irrepsCarterLabels[i] << "\");";
  out << "\n<br>&nbsp;&nbsp;return true;";
  out << "\n<br>}";
  return out.str();
}

std::string WeylGroup::ToStringCppConjugacyClasses(FormatExpressions* theFormat)
{ MacroRegisterFunctionWithName("WeylGroup::ToStringCppConjugacyClasses");
  if (!this->flagCCRepresentativesComputed)
    return "";
  std::stringstream out;
  out << "<hr>Here is the c++ input code for the conjugacy class table.";
  out << "<br>";
  FormatExpressions theFormatNoDynkinTypePlusesExponents;
  theFormatNoDynkinTypePlusesExponents.flagDynkinTypeDontUsePlusAndExponent=true;
  out << "bool LoadConjugacyClasses" << this->theDynkinType.ToString(&theFormatNoDynkinTypePlusesExponents) << "(WeylGroup& output)\n<br>{ ";
  out << "output.ComputeRho(true);";
  out << "\n<br>&nbsp;&nbsp;WeylGroup::ConjugacyClass emptyClass;";
  out << "\n<br>&nbsp;&nbsp;emptyClass.flagRepresentativeComputed=true;";
  out << "\n<br>&nbsp;&nbsp;output.conjugacyClasseS.initFillInObject(" << this->conjugacyClasseS.size << ", emptyClass);";
  for (int i=0; i<this->ConjugacyClassCount(); i++)
  { out << "\n<br>&nbsp;&nbsp;output.conjugacyClasseS[" << i;
    for (int j=((Rational)i).ToString().size(); j<3; j++) //<-if the index i is smaller than 100, make sure it takes
      out << "&nbsp;"; // making sure index has width exactly 3 spaces
    out << "].representative.MakeFromReadableReflections(output, false, \"";
    for (int j=0; j<this->conjugacyClasseS[i].representative.generatorsLastAppliedFirst.size; j++)
    { out << this->conjugacyClasseS[i].representative.generatorsLastAppliedFirst[j]+1;
      if (j!=this->conjugacyClasseS[i].representative.generatorsLastAppliedFirst.size-1)
        out << ",";
    }
    out << "\");";
  }
  for (int i=0; i<this->ConjugacyClassCount(); i++)
  { out << "\n<br>&nbsp;&nbsp;output.conjugacyClasseS[" << i;
    for (int j=((Rational)i).ToString().size(); j<3; j++) //<-if the index i is smaller than 100, make sure it takes
      out << "&nbsp;"; // making sure index has width exactly 3 spaces
    out  << "].size=" << this->conjugacyClasseS[i].size.ToString() << ";";
  }
  out << "\n<br>&nbsp;&nbsp;output.ccCarterLabels.SetSize(0);";
  for (int i=0; i<this->ccCarterLabels.size; i++)
    out << "\n<br>&nbsp;&nbsp;output.ccCarterLabels.AddOnTop(\"" << this->ccCarterLabels[i] << "\");";
  out << "\n<br>&nbsp;&nbsp;output.LoadConjugacyClassesHelper();";
  out << "\n<br>&nbsp;&nbsp;return true;";
  out << "\n<br>}";
  return out.str();
}

std::string WeylGroup::ToStringRootsAndRootReflections(FormatExpressions* theFormat)
{ MacroRegisterFunctionWithName("WeylGroup::ToStringRootsAndRootReflections");
  std::stringstream out, outLatex;
  out << "<br>The root system has " << this->RootSystem.size << " elements.\n";
  out << "<table><tr><td>Simple basis coordinates</td><td>Epsilon coordinates</td>"
  << "<td>Reflection w.r.t. root</td></tr>";
  Vectors<Rational> rootSystemEpsCoords;
  this->GetEpsilonCoords(this->RootSystem, rootSystemEpsCoords);
  ElementWeylGroup<WeylGroup> currentRootReflection;
  for (int i=0; i<this->RootSystem.size; i++)
  { const Vector<Rational>& current=this->RootSystem[i];
    currentRootReflection.MakeRootReflection(current, *this);
    out << "<tr><td>" << current.ToString() << "</td><td>" << rootSystemEpsCoords[i].ToStringLetterFormat("e") << "</td>"
    << "<td>" << CGI::GetMathMouseHover(currentRootReflection.ToString()) << "</td>" << "</tr>";
  }
  out << "</table>";
  out << "Comma delimited list of roots: ";
  for (int i=0; i<this->RootSystem.size; i++)
  { out << this->RootSystem[i].ToString();
    if (i!=this->RootSystem.size-1)
      out << ", ";
  }
  out << outLatex.str();
  return out.str();
}

std::string WeylGroup::ToString(FormatExpressions* theFormat)
{ MacroRegisterFunctionWithName("WeylGroup::ToString");
  std::stringstream out;
  out << "<br>Size: " << this->theElements.size << "\n";
//  out <<"Number of Vectors<Rational>: "<<this->RootSystem.size<<"\n
  out << "<br>Half-sum positive roots:" << this->rho.ToString() << "\n";
  out << this->ToStringRootsAndRootReflections();
  out << "<br>Symmetric cartan: " << this->CartanSymmetric.ToString();
  if (this->flagCharTableIsComputed)
  { out << "<br>Character table: ";
    Matrix<Rational> charTableMatForm;
    charTableMatForm.init(this->irreps.size, this->ConjugacyClassCount());
    for (int i=0; i<this->irreps.size; i++)
      charTableMatForm.AssignVectorToRowKeepOtherRowsIntactNoInit(i, this->irreps[i].theCharacteR.data);
    out << charTableMatForm.ToString();
  }
  out << this->ToStringConjugacyClasses(theFormat);
  out << this->ToStringElements(theFormat);
  out << this->ToStringCppConjugacyClasses(theFormat);
  out << this->ToStringCppCharTable(theFormat);
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

void WeylGroup::ComputeCoCartanSymmetricFromCartanSymmetric()
{ this->GetCoCartanSymmetric(this->CartanSymmetric, this->CoCartanSymmetric);
}

void WeylGroup::MakeMeFromMyCartanSymmetric()
{ this->init();
  this->GenerateRootSystem();
  DynkinDiagramRootSubalgebra theDynkinTypeComputer;
  Vectors<Rational> simpleBasis;
  simpleBasis.MakeEiBasis(this->CartanSymmetric.NumRows);
  theDynkinTypeComputer.ComputeDiagramTypeModifyInputRelative(simpleBasis, this->RootSystem, this->CartanSymmetric);
  theDynkinTypeComputer.GetDynkinType(this->theDynkinType);
  this->MakeFinalSteps();
}

void WeylGroup::MakeFinalSteps()
{ this->ComputeCoCartanSymmetricFromCartanSymmetric();
}

void WeylGroup::InitGenerators()
{ this->generators.SetSize(this->GetDim());
  if (this->GetDim()==0)
  { this->generators.SetSize(1);
    this->generators[0].MakeID(*this);
  }
  for (int i=0; i<this->GetDim(); i++)
    this->generators[i].MakeSimpleReflection(i, *this);
}

void WeylGroup::MakeFromDynkinType(const DynkinType& inputType)
{ this->init();
  this->theDynkinType=inputType;
  this->theDynkinType.GetCartanSymmetric(this->CartanSymmetric);
  this->MakeFinalSteps();
}

void WeylGroup::MakeFromDynkinTypeDefaultLengthKeepComponentOrder(const DynkinType& inputType)
{ this->init();
  this->theDynkinType=inputType;
  this->theDynkinType.GetCartanSymmetricDefaultLengthKeepComponentOrder(this->CartanSymmetric);
  this->MakeFinalSteps();
}

void WeylGroup::MakeArbitrarySimple(char WeylGroupLetter, int n, const Rational* firstCoRootLengthSquared)
{ DynkinType inputType;
  inputType.MakeSimpleType(WeylGroupLetter, n, firstCoRootLengthSquared);
  this->MakeFromDynkinType(inputType);
}

void WeylGroup::ComputeOuterAutos()
{ if (this->flagAllOuterAutosComputed)
    return;
  MacroRegisterFunctionWithName("WeylGroup::ComputeOuterAutoGenerators");
  this->ComputeOuterAutoGenerators();
  this->theOuterAutos.GetElement().GenerateElements(0);
  this->flagAllOuterAutosComputed=true;
}

void WeylGroup::ComputeOuterAutoGenerators()
{ if (this->flagOuterAutosGeneratorsComputed)
    return;
  List<MatrixTensor<Rational> >& theGens=this->theOuterAutos.GetElement().theGenerators;
  this->theDynkinType.GetOuterAutosGeneratorsActOnVectorColumn(theGens);
  for (int i=0; i<theGens.size; i++)
    if (theGens[i].GetMinNumColsNumRows()!=this->GetDim() || theGens[i].GetMinNumCols()!=this->GetDim() ||
        theGens[i].GetMinNumRows()!=this->GetDim() )
    { crash << "Bad outer automorphisms, type " << this->theDynkinType.ToString() << "." << crash;
    }
  this->flagOuterAutosGeneratorsComputed=true;
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
  { DynkinSimpleType::GetEpsilonMatrix(tempDyn.SimpleComponentTypes[i].theLetter, tempDyn.SimpleComponentTypes[i].theRank, tempMat);
    basisChange.DirectSumWith(tempMat, (Rational) 0);
    //basisChange.ComputeDebugString();
  }
  simpleBasis.AssignListList(tempDyn.SimpleBasesConnectedComponents);
//  stOutput << "<br>simple basis: " << simpleBasis.ToString();
//  stOutput << "<br>to be converted: " << input.ToString();
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

void WeylGroup::GetWeylChamber(Cone& output, GlobalVariables& theGlobalVariables)
{ Matrix<Rational> tempMat;
  tempMat=this->CartanSymmetric;
  tempMat.Invert();
  Vectors<Rational> tempRoots;
  tempRoots.AssignMatrixRows(tempMat);
  output.CreateFromVertices(tempRoots, &theGlobalVariables);
//  output.CreateFromNormals(tempRoots, theGlobalVariables);
}

void WeylGroup::GetFundamentalWeightsInSimpleCoordinates(Vectors<Rational>& output)
{ Matrix<Rational> tempMat;
  tempMat=this->CartanSymmetric;
  Rational tempRat;
  for (int i=0; i<this->GetDim(); i++)
  { tempRat=2/this->CartanSymmetric.elements[i][i];
    tempMat.RowTimesScalar(i, tempRat);
  }
  tempMat.Transpose();
  tempMat.Invert();
  output.AssignMatrixRows(tempMat);
}

void WeylGroup::GetIntegralLatticeInSimpleCoordinates(Lattice& output)
{ output.basisRationalForm=this->CartanSymmetric;
  Vector<Rational> tempRoot;
  for (int i=0; i<this->GetDim(); i++)
  { tempRoot.MakeEi(this->GetDim(), i);
    output.basisRationalForm.RowTimesScalar(i, 2/this->RootScalarCartanRoot(tempRoot, tempRoot));
  }
  output.basisRationalForm.Transpose();
  output.basisRationalForm.Invert();
//  stOutput << output.basisRationalForm.ToString(true, false);
  output.MakeFromMat(output.basisRationalForm);
  output.Reduce();
}

Rational WeylGroup::GetKillingDivTraceRatio()
{ Rational result=0;
  Rational tempRat;
  for (int i=0; i<this->RootSystem.size; i++)
  { tempRat=this->RootScalarCartanRoot(this->RootSystem[i], this->RootSystem[0]);
    result+=tempRat*tempRat;
  }
  result/=this->RootScalarCartanRoot(this->RootSystem[0], this->RootSystem[0]);
  return result;
}

void WeylGroup::GetLongestWeylElt(ElementWeylGroup<WeylGroup>& outputWeylElt)
{ this->ComputeRho(false);
  Vector<Rational> lowest=this->rho;
//  stOutput << "rho: " << this->rho.ToString() << "<hr>";
  Vectors<Rational> tempRoots;
  tempRoots.MakeEiBasis(this->GetDim());
  this->GetLowestElementInOrbit(lowest, &outputWeylElt, tempRoots, false, false);
  //std::stringstream out;
  //out << outputWeylElt;
//  out << "\n<br>";
  //Matrix<Rational>  tempMat;
  //this->GetMatrixOfElement(outputWeylElt, tempMat);
  //out << tempMat.ToString(true, false);
  //stOutput << out.str();
  //stOutput << outputWeylElt;
 //stOutput << this->GetMatrixOfElement(
}

void WeylGroup::GetExtremeElementInOrbit
(Vector<Rational>& inputOutput, ElementWeylGroup<WeylGroup>* outputWeylElt, Vectors<Rational>& bufferEiBAsis,
 bool findLowest, bool RhoAction, bool UseMinusRho, int* sign, bool* stabilizerFound)
{ if (outputWeylElt!=0)
    outputWeylElt->MakeID(*this);
  if (sign!=0)
    *sign=1;
  if (stabilizerFound!=0)
    *stabilizerFound=false;
  Rational theScalarProd;
  ElementWeylGroup<WeylGroup> eltSimplReflection;
  //  static int numTimesReflectionWasApplied=0;
  for (bool found = true; found; )
  { found=false;
    for (int i=0; i<this->GetDim(); i++)
    { bool shouldApplyReflection=false;
      theScalarProd=this->RootScalarCartanRoot(inputOutput, bufferEiBAsis[i]);
      if (findLowest)
        shouldApplyReflection=theScalarProd.IsPositive();
      else
        shouldApplyReflection= theScalarProd.IsNegative();
      if (stabilizerFound!=0)
        if (theScalarProd.IsEqualToZero())
          *stabilizerFound=true;
      if (shouldApplyReflection)
      { found=true;
        if (!RhoAction)
          this->SimpleReflection<Rational>(i, inputOutput);
        else if (!UseMinusRho)
          this->SimpleReflectionRhoModified(i, inputOutput);
        else
          this->SimpleReflectionMinusRhoModified(i, inputOutput);
        if (outputWeylElt!=0)
        { eltSimplReflection.MakeSimpleReflection(i, *this);
          *outputWeylElt=eltSimplReflection*(*outputWeylElt);
        }
        if (sign!=0)
          *sign*=-1;
//        numTimesReflectionWasApplied++;
      }
    }
  }
//  stOutput << "<hr># simple reflections applied total: " << numTimesReflectionWasApplied;
}

WeylGroup::WeylGroup()
{ this->init();
}

bool WeylGroup::IsElementWeylGroupOrOuterAuto(const MatrixTensor<Rational>& input)
{ MacroRegisterFunctionWithName("WeylGroup::IsElementGroup<WeylGroup>OrOuterAuto");
  this->ComputeOuterAutos();
//  stOutput << this->theOuterAutos.GetElement().ToString();
  Vector<Rational> theRhoImage;
  input.ActOnVectorColumn(this->rho, theRhoImage);
  ElementWeylGroup<WeylGroup> theElementCandidate;
  this->RaiseToDominantWeight(theRhoImage, 0, 0, &theElementCandidate);
  Matrix<Rational> theCandidateMat;
  MatrixTensor<Rational> theCandidateMatTensorForm, theCandidateMatWithOuterAuto;
  this->GetMatrixStandardRep(theElementCandidate, theCandidateMat);
//  stOutput << "<br>input: " << input.ToStringMatForm();
//  stOutput << "<br>checking whether input is outer auto acting on: " << theCandidateMat.ToString();
  theCandidateMatTensorForm=theCandidateMat;
  for (int i=0; i<this->theOuterAutos.GetElement().theElements.size; i++)
  { theCandidateMatWithOuterAuto=this->theOuterAutos.GetElement().theElements[i];
    theCandidateMatWithOuterAuto*=theCandidateMatTensorForm;
//    stOutput << "Candidate mat with outer auto: " << theCandidateMatWithOuterAuto.ToStringMatForm();
    if (theCandidateMatWithOuterAuto==input)
      return true;
  }
  return false;
}

void WeylGroup::GetMatrixReflection(Vector<Rational>& reflectionRoot, Matrix<Rational>& output)
{ Vectors<Rational> basis;
  int theDim=this->GetDim();
  basis.MakeEiBasis(theDim);
//  output.init(theDim, theDim);
  for (int i=0; i<theDim; i++)
    this->ReflectBetaWRTAlpha(reflectionRoot, basis[i], false, basis[i]);
  output.AssignVectorsToRows(basis);
  output.Transpose();
}

void WeylGroup::GetCoxeterPlane(Vector<double>& outputBasis1, Vector<double>& outputBasis2, GlobalVariables& theGlobalVariables)
{ MacroRegisterFunctionWithName("WeylGroup::GetCoxeterPlane");
  this->ComputeRho(true);
  Vector<Rational> ZeroRoot;
  int theDimension=this->GetDim();
  if (theDimension<2)
    return;
  ZeroRoot.MakeZero(theDimension);
  ElementWeylGroup<WeylGroup> tempElt;
  this->GetCoxeterElement(tempElt);
  Matrix<Rational>  matCoxeterElt, tempMat;
  this->GetMatrixStandardRep(tempElt, matCoxeterElt);
//  stOutput << matCoxeterElt.ToString(true, false);
  tempMat=matCoxeterElt;
  int coxeterNumber=this->RootSystem.LastObject()->SumCoords().NumShort+1;
  for (int i=0; i<coxeterNumber-1; i++)
    tempMat.MultiplyOnTheLeft(matCoxeterElt);
//  stOutput << "<br>coxeter transformation to the power of " << coxeterNumber << " equals: " << tempMat.ToString(true, false);
  CompleX<double> theEigenValue;
  theEigenValue.Re= FloatingPoint:: cos(2*MathRoutines::Pi()/coxeterNumber);
  theEigenValue.Im= FloatingPoint:: sin(2*MathRoutines::Pi()/coxeterNumber);
  Matrix<CompleX<double> > eigenMat, idMat;
  eigenMat.init(matCoxeterElt.NumRows, matCoxeterElt.NumCols);
  for (int i =0; i<eigenMat.NumRows; i++)
    for (int j=0; j<eigenMat.NumCols; j++)
    { eigenMat.elements[i][j]=matCoxeterElt.elements[i][j].GetDoubleValue();
      if (i==j)
        eigenMat.elements[i][i]-=theEigenValue;
    }
  List<Vector<CompleX<double> > > theEigenSpaceList;
  eigenMat.GetZeroEigenSpace(theEigenSpaceList);
  Vectors<CompleX<double> > theEigenSpace;
  outputBasis1.SetSize(theDimension);
  outputBasis2.SetSize(theDimension);
  theEigenSpace.operator=(theEigenSpaceList);
  DrawOperations tempDO;
  tempDO.initDimensions(theDimension, 1);
  tempDO.GraphicsUnit[0]=DrawOperations::GraphicsUnitDefault;
  theEigenSpace.operator=(theEigenSpaceList);
  for (int i=0; i<theDimension; i++)
    for (int j=0; j<theDimension; j++)
      tempDO.theBilinearForm.elements[i][j]=this->CartanSymmetric.elements[i][j].GetDoubleValue();

  if (theEigenSpace.size>0)
  { if (coxeterNumber>2)
      for (int j=0; j<theDimension; j++)
      { outputBasis1[j]=theEigenSpace[0][j].Re;
        outputBasis2[j]=theEigenSpace[0][j].Im;
      }
    else if (coxeterNumber==1 && theEigenSpace.size>1 )
      for (int j=0; j<theDimension; j++)
      { outputBasis1[j]=theEigenSpace[0][j].Re;
        outputBasis2[j]=theEigenSpace[1][j].Re;
      }
    tempDO.ModifyToOrthonormalNoShiftSecond(outputBasis2, outputBasis1);
  }
}

void WeylGroup::DrawRootSystem
(DrawingVariables& outputDV, bool wipeCanvas, GlobalVariables& theGlobalVariables, bool drawWeylChamber, Vector<Rational>* bluePoint,
 bool LabelDynkinDiagramVertices, Vectors<Rational>* predefinedProjectionPlane)
{ MacroRegisterFunctionWithName("WeylGroup::DrawRootSystem");
  DrawOperations& output=outputDV.theBuffer;
  if (wipeCanvas)
    output.init();
  int theDimension=this->GetDim();
  if(theDimension==1)
  { int color=CGI::RedGreenBlue(0, 255, 0);
    Vector<Rational> tempRoot, tempZero;
    tempZero.MakeZero(2);
    tempRoot.MakeEi(2, 0);
    for (int i=0; i<2; i++)
    { output.drawLineBetweenTwoVectorsBuffer(tempZero, tempRoot, DrawingVariables::PenStyleNormal, color);
      output.drawCircleAtVectorBuffer(tempRoot, 2, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(255,0,255));
      tempRoot.Minus();
    }
    return;
  }
  this->ComputeRho(true);
  Vector<Rational> ZeroRoot;
  ZeroRoot.MakeZero(theDimension);
  output.initDimensions(theDimension, 1);
  output.GraphicsUnit[0]=DrawOperations::GraphicsUnitDefault;
  for (int i=0; i<theDimension; i++)
    for (int j=0; j<theDimension; j++)
      output.theBilinearForm.elements[i][j]=this->CartanSymmetric.elements[i][j].GetDoubleValue();
  Vector<double> tempRoot;
  output.SelectedPlane=0;
  Vectors<double>& theTwoPlane= output.BasisProjectionPlane[0];
  if (predefinedProjectionPlane==0)
    this->GetCoxeterPlane(theTwoPlane[0], theTwoPlane[1], theGlobalVariables);
  else
    predefinedProjectionPlane->GetVectorsDouble(theTwoPlane);
  if(theTwoPlane.size!=2)
    crash << crash;
//  stOutput << "<hr><hr>the eigenspace: " << theEigenSpace.ToString(false, true, false);
//  std::stringstream tempStream;
//  tempStream << "<hr>the eigen mat:";
//  tempStream << eigenMat;
//  stOutput << tempStream.str();
  Vectors<Rational> RootSystemSorted;
  RootSystemSorted=(this->RootSystem);
  List<double> lengths;
  lengths.SetSize(RootSystemSorted.size);
  for (int i=0; i<this->RootSystem.size; i++)
  { tempRoot.SetSize(theDimension);
    for (int j=0; j<theDimension; j++)
      tempRoot[j]=this->RootSystem[i][j].GetDoubleValue();
    double Length1 = this->RootScalarCartanRoot(tempRoot, output.BasisProjectionPlane[0][0]);
    double Length2 = this->RootScalarCartanRoot(tempRoot, output.BasisProjectionPlane[0][1]);
    lengths[i]=FloatingPoint::sqrt(Length1*Length1+Length2*Length2);
  }
  for (int i=0; i<RootSystemSorted.size; i++)
    for (int j=i; j<RootSystemSorted.size; j++)
      if (lengths[i]<lengths[j])
      { MathRoutines::swap(lengths[i], lengths[j]);
        MathRoutines::swap(RootSystemSorted[i], RootSystemSorted[j]);
      }
  Vector<Rational> differenceRoot;
  differenceRoot=RootSystemSorted[0]-RootSystemSorted[1];
  Rational minLength= this->RootScalarCartanRoot(differenceRoot, differenceRoot);
  for (int i=2; i<RootSystemSorted.size; i++)
  { differenceRoot=RootSystemSorted[0]-RootSystemSorted[i];
    if (minLength> this->RootScalarCartanRoot(differenceRoot, differenceRoot))
      minLength=this->RootScalarCartanRoot(differenceRoot, differenceRoot);
  }
//  stOutput << "<hr>the min length is: " << minLength.ToString();
  Rational tempRat;
  if (bluePoint!=0)
  { output.drawCircleAtVectorBuffer(*bluePoint, 5, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(0,0,255));
    output.drawCircleAtVectorBuffer(*bluePoint, 4, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(0,0,255));
    output.drawCircleAtVectorBuffer(*bluePoint, 3, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(0,0,255));
  }
  if (drawWeylChamber)
  { Cone theWeylChamber;
    this->GetWeylChamber(theWeylChamber, theGlobalVariables);
    FormatExpressions tempFormat;
    theWeylChamber.DrawMeProjective(0, false, outputDV, tempFormat);
  }
  theGlobalVariables.theDrawingVariables.DefaultHtmlHeight=600;
  theGlobalVariables.theDrawingVariables.DefaultHtmlWidth=600;
  output.centerX[0]=300;
  output.centerY[0]=300;
  for (int i=0; i<RootSystemSorted.size; i++)
  { int color=CGI::RedGreenBlue(0, 255, 0);
    output.drawLineBetweenTwoVectorsBuffer(ZeroRoot, RootSystemSorted[i], DrawingVariables::PenStyleNormal, color);
    output.drawCircleAtVectorBuffer(RootSystemSorted[i], 2, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(255,0,255));
    for (int j=i+1; j<RootSystemSorted.size; j++)
    { differenceRoot=RootSystemSorted[i]-RootSystemSorted[j];
      tempRat=this->RootScalarCartanRoot(differenceRoot, differenceRoot);
      if (minLength== tempRat)
        output.drawLineBetweenTwoVectorsBuffer(RootSystemSorted[i], RootSystemSorted[j], DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(0, 0, 255));
    }
  }
  Vector<Rational> tempRootRat;
  Vectors<Rational> epsNotationSimpleBasis;
  epsNotationSimpleBasis.MakeEiBasis(theDimension);
  this->GetEpsilonCoords(epsNotationSimpleBasis, epsNotationSimpleBasis);
  for (int i=0; i<theDimension; i++)
  { tempRootRat.MakeEi(theDimension, i);
    output.drawCircleAtVectorBuffer(tempRootRat, 1, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(255,0,0));
    output.drawCircleAtVectorBuffer(tempRootRat, 3, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(255,0,0));
    output.drawCircleAtVectorBuffer(tempRootRat, 4, DrawingVariables::PenStyleNormal, CGI::RedGreenBlue(255,0,0));
    if (LabelDynkinDiagramVertices)
    { Vector<Rational>& current=epsNotationSimpleBasis[i];
      output.drawTextAtVectorBuffer(tempRootRat, current.ToStringLetterFormat("e"),0, 10, DrawingVariables::TextStyleNormal);
    }
  }
  for (int i=0; i<this->RootSystem.size; i++)
  { output.labeledVectors.AddOnTop(this->RootSystem[i].GetVectorDouble());
    output.labelsOfLabeledVectors.AddOnTop(this->RootSystem[i].ToString());
    output.toBeHighlightedWhenLabeledVectorHovered.SetSize(output.toBeHighlightedWhenLabeledVectorHovered.size+1);
    output.toBeHighlightedWhenLabeledVectorHovered.LastObject()->SetSize(0);
    output.toBeHighlightedWhenLabeledVectorHovered.LastObject()->AddOnTop(this->RootSystem[i].GetVectorDouble());
    output.toBeHighlightedWhenLabeledVectorHovered.LastObject()->AddOnTop((-this->RootSystem[i]).GetVectorDouble());
  }
  std::stringstream tempStream;
  tempStream << this->theDynkinType.GetWeylGroupName();
  if (this->GetDim()==2 && predefinedProjectionPlane!=0)
  { theTwoPlane[1][0]=1;
    theTwoPlane[1][1]=0;
    theTwoPlane[0][0]=0;
    theTwoPlane[0][1]=1;
    outputDV.theBuffer.ModifyToOrthonormalNoShiftSecond(theTwoPlane[0], theTwoPlane[1]);
  }
  output.drawTextBuffer(0, 0, tempStream.str(), 10, CGI::RedGreenBlue(0,0,0), DrawingVariables::TextStyleNormal);
}

std::string WeylGroup::GenerateWeightSupportMethoD1
(Vector<Rational>& highestWeightSimpleCoords, Vectors<Rational>& outputWeightsSimpleCoords, int upperBoundWeights, GlobalVariables& theGlobalVariables)
{ HashedList<Vector<Rational> > theDominantWeights;
  double upperBoundDouble=100000/((Rational)this->GetGroupSizeByFormula()).GetDoubleValue();
  int upperBoundInt = MathRoutines::Maximum((int) upperBoundDouble, 10000);
  //int upperBoundInt = 10000;
  Vector<Rational> highestWeightTrue=highestWeightSimpleCoords;
  this->RaiseToDominantWeight(highestWeightTrue);
  std::stringstream out;
  if (highestWeightTrue!=highestWeightSimpleCoords)
    out << "<br>Cheater! The input weight is not highest... using the highest weight in the same orbit instead. "
    << "Your input in simple coordinates was: "
    << highestWeightSimpleCoords.ToString() << ".<br> ";
  out << "The highest weight in simple coordinates is: " << highestWeightTrue.ToString() << ".<br>";
  std::string tempS;
  bool isTrimmed = !this->GetAlLDominantWeightsHWFDIM(highestWeightSimpleCoords, theDominantWeights, upperBoundInt, &tempS, &theGlobalVariables);
  out << tempS << "<br>";
  if (isTrimmed)
    out << "Trimmed the # of dominant weights - upper bound is " << upperBoundInt << ". <br>";
  else
    out << "Number of (non-strictly) dominant weights: " << theDominantWeights.size << "<br>";
  Vectors<Rational> tempRoots;
  HashedList<Vector<Rational> > finalWeights;
  int estimatedNumWeights=(int)
  ( ((Rational)this->GetGroupSizeByFormula()).GetDoubleValue()*theDominantWeights.size);
  estimatedNumWeights= MathRoutines::Minimum(10000, estimatedNumWeights);
  finalWeights.ReservE(estimatedNumWeights);
  finalWeights.SetHashSizE(estimatedNumWeights);
  Vectors<Rational> dominantWeightsNonHashed;
  dominantWeightsNonHashed=(theDominantWeights);
  this->GenerateOrbit(dominantWeightsNonHashed, false, finalWeights, false, 0, 0, 10000);
  if (finalWeights.size>=10000)
  { out << "Did not generate all weights of the module due to RAM limits. ";
    if (!isTrimmed)
      out << "However, all dominant weights were computed and are drawn.";
    out << "<br>";
  }
  if (!isTrimmed && finalWeights.size<10000)
    out << "All weights were computed and are drawn. <br>";
  outputWeightsSimpleCoords=(finalWeights);
  return out.str();
}

bool WeylGroup::IsEigenSpaceGeneratorCoxeterElement(Vector<Rational>& input)
{ ElementWeylGroup<WeylGroup> tempElt;
  this->GetCoxeterElement(tempElt);
  Matrix<Rational> matCoxeterElt;
  this->GetMatrixStandardRep(tempElt, matCoxeterElt);
  Vector<Rational> tempRoot=input;
  for (int i=0; i<this->GetDim(); i++)
    matCoxeterElt.ActOnVectorColumn(tempRoot);
  return tempRoot==input;
}

Rational WeylGroup::GetLongestRootLengthSquared()
{ Rational result;
  result=this->CartanSymmetric(0,0);
  for (int i=1; i<this->CartanSymmetric.NumRows; i++)
    result=MathRoutines::Maximum(result, this->CartanSymmetric(i,i));
  return result;
}

bool WeylGroup::IsElementWeylGroup(const MatrixTensor<Rational>& input)
{ MacroRegisterFunctionWithName("WeylGroup::IsElementWeylGroup");
  Vector<Rational> theRhoImage;
  input.ActOnVectorColumn(this->rho, theRhoImage);
  ElementWeylGroup<WeylGroup> theElementCandidate;
  this->RaiseToDominantWeight(theRhoImage, 0, 0, &theElementCandidate);
  Matrix<Rational> theCandidateMat, inputMat;
  input.GetMatrix(inputMat, this->GetDim());
  this->GetMatrixStandardRep(theElementCandidate, theCandidateMat);
  return theCandidateMat==inputMat;
}

bool WeylGroup::ContainsARootNonStronglyPerpendicularTo(Vectors<Rational>& theVectors, Vector<Rational>& input)
{ for (int i=0; i<this->theElements.size; i++)
    if (this->IsARoot(theVectors[i]+input))
      return true;
  return false;
}

void WeylGroup::GetMatrixStandardRep(const ElementWeylGroup<WeylGroup>& input, Matrix<Rational>& outputMatrix)const
{ Vector<Rational> tempRoot;
  int theDim=this->CartanSymmetric.NumRows;
  outputMatrix.init(theDim, theDim);
  for (int i=0; i<theDim; i++)
  { tempRoot.MakeEi(theDim, i);
    this->ActOn(input, tempRoot);
    for (int j=0; j<theDim; j++)
      outputMatrix(j,i)=tempRoot[j];
  }
}

int WeylGroup::NumRootsConnectedTo(Vectors<Rational>& theVectors, Vector<Rational>& input)
{ int result=0;
  for (int i=0; i<this->theElements.size; i++)
    if (!Vector<Rational>::ScalarProduct(theVectors[i], input, this->CartanSymmetric).IsEqualToZero())
      result++;
  return result;
}

void WeylGroup::ComputeRho(bool Recompute)
{ if (this->RootSystem.size==0 || Recompute)
    this->GenerateRootSystem();
  //this->ComputeDebugString();
  this->rho.MakeZero(this->CartanSymmetric.NumRows);
  for (int i=0; i<this->RootSystem.size; i++)
    if (this->RootSystem[i].IsPositiveOrZero())
      this->rho+=(RootSystem[i]);
  for (int i=0; i<this->CartanSymmetric.NumCols; i++)
    this->rho[i].DivideByInteger(2);
//  this->lengthLongestRootSquared=this->GetLongestRootLengthSquared();
  this->flagFundamentalToSimpleMatricesAreComputed=false;
}

void SubgroupWeylGroupOLD::operator=(const SubgroupWeylGroupOLD& other)
{ this->::HashedList<ElementWeylGroup<WeylGroup>>::operator=(other);
  this->simpleGenerators=(other.simpleGenerators);
  this->ExternalAutomorphisms=(other.ExternalAutomorphisms);
  this->AmbientWeyl=(other.AmbientWeyl);
  this->RootSubsystem=other.RootSubsystem;
  this->RootsOfBorel=other.RootsOfBorel;
}

std::string SubgroupWeylGroupOLD::ElementToStringFromLayersAndArrows(List<List<List<int> > >& arrows, List<List<int> >& Layers, int GraphWidth, bool useAmbientIndices)
{ std::stringstream out;
//  stOutput << this->simpleGenerators.ToString();
  List<int> DisplayIndicesSimpleGenerators;
  if (!useAmbientIndices)
  { DisplayIndicesSimpleGenerators.SetSize(this->simpleGenerators.size);
    for (int i=0; i<this->simpleGenerators.size; i++)
      DisplayIndicesSimpleGenerators[i]=this->AmbientWeyl.RootsOfBorel.GetIndex(this->simpleGenerators[i])+1;
  }
  out << "\\xymatrix{";
  bool GraphWidthIsOdd=((GraphWidth%2)!=0);
  if (!GraphWidthIsOdd)
    GraphWidth++;
  for (int i=0; i<Layers.size; i++)
  { int currentRowOffset=(GraphWidth-Layers[i].size)/2;
    int nextRowOffset=-1;
    if (i<Layers.size-1)
      nextRowOffset=(GraphWidth-Layers[i+1].size)/2;
    for (int j=0; j<currentRowOffset; j++)
      out << "&";
    for (int j=0; j<Layers[i].size; j++)
    { if (!useAmbientIndices)
        out << this->TheObjects[Layers[i][j]].ToString(0, &DisplayIndicesSimpleGenerators);
      else
        out << this->RepresentativesQuotientAmbientOrder[Layers[i][j]].ToString();
      int currentOffset=j+currentRowOffset;
      if (Layers[i].size%2==0)
        if (currentOffset>=GraphWidth/2)
          currentOffset++;
      for (int k=0; k<arrows[i][j].size; k++)
      { out << " \\ar[d";
        int indexInLayer=Layers[i+1].GetIndex(arrows[i][j][k]);
        if(indexInLayer==-1)
          crash << crash;
        int nextOffset=indexInLayer+nextRowOffset;
        if (Layers[i+1].size%2==0)
          if (nextOffset>=GraphWidth/2)
            nextOffset++;
        int actualOffset=-currentOffset+nextOffset;
        for (int l=0; l<actualOffset; l++)
          out << "r";
        for (int l=0; l>actualOffset; l--)
          out << "l";
        out << "]";
      }
      out << " & ";
      if (Layers[i].size%2==0 && j==Layers[i].size/2-1)
        out << " & ";
    }
    out << " \\\\\n";
  }
  out << "}";
  return out.str();

}

std::string SubgroupWeylGroupOLD::ElementToStringBruhatGraph()
{ if (this->size<1)
    return "Error, non-initialized group";
  if (this->size==1)
    return "id";
  List<List<List<int> > > arrows;
  List<List<int> > Layers;
  Vector<Rational> tempRoot;
  Layers.ReservE(this->size);
  int GraphWidth=1;
  int oldLayerElementLength=-1;
  for (int i=0; i< this->size; i++)
  { if ((*this)[i].generatorsLastAppliedFirst.size!=oldLayerElementLength)
    { Layers.SetSize(Layers.size+1);
      oldLayerElementLength=(*this)[i].generatorsLastAppliedFirst.size;
    }
    Layers.LastObject()->AddOnTop(i);
    GraphWidth=MathRoutines::Maximum(GraphWidth, Layers.LastObject()->size);
  }
  HashedList<Vector<Rational> > orbit;
  orbit.ReservE(this->size);
  for (int i=0; i<this->size; i++)
  { this->ActByElement(i, this->AmbientWeyl.rho, tempRoot);
    orbit.AddOnTop(tempRoot);
  }
  arrows.SetSize(Layers.size);
  for (int i=0; i< Layers.size; i++)
  { arrows[i].SetSize(Layers[i].size);
    for (int j=0; j<Layers[i].size; j++)
      for (int k=0; k<this->simpleGenerators.size; k++)
      { this->AmbientWeyl.ReflectBetaWRTAlpha(this->simpleGenerators[k], orbit[Layers[i][j]], false, tempRoot);
        int index=orbit.GetIndex(tempRoot);
        if(index==-1)
          crash << crash;
        if ((*this)[index].generatorsLastAppliedFirst.size>(*this)[Layers[i][j]].generatorsLastAppliedFirst.size)
          arrows[i][j].AddOnTop(index);
      }
  }
  return this->ElementToStringFromLayersAndArrows(arrows, Layers, GraphWidth, false);
}

void SubgroupWeylGroupOLD::ToString(std::string& output, bool displayElements)
{ MacroRegisterFunctionWithName("SubgroupWeylGroupOLD::ToString");
  std::stringstream out, head, head2;
  List<int> DisplayIndicesSimpleGenerators;
  DisplayIndicesSimpleGenerators.SetSize(this->simpleGenerators.size);
  FormatExpressions latexFormat;
  latexFormat.flagUseHTML=false;
  latexFormat.flagUseLatex=true;
  bool isGood=true;
  for (int i=0; i<this->simpleGenerators.size; i++)
  { DisplayIndicesSimpleGenerators[i]=this->AmbientWeyl.RootsOfBorel.GetIndex(this->simpleGenerators[i])+1;
    if (DisplayIndicesSimpleGenerators[i]==0)
    { isGood=false;
      break;
    }
  }
  if (!isGood)
    for (int i=0; i<this->simpleGenerators.size; i++)
      DisplayIndicesSimpleGenerators[i]=i+1;
  DynkinDiagramRootSubalgebra tempDyn;
  tempDyn.ComputeDiagramInputIsSimple(this->simpleGenerators, this->AmbientWeyl.CartanSymmetric);
  out << "Dynkin diagram & subalgebra of root subsystem generated by the given root: "
  << tempDyn.ToStringRelativeToAmbientType(this->AmbientWeyl.theDynkinType[0]);
  out << "<br>Simple roots:\n<br>\n ";
  head << "\\begin{array}{rcl}";
  for (int i=0; i<this->simpleGenerators.size; i++)
    head << "\n\\eta_{" << DisplayIndicesSimpleGenerators[i] << "}&:=&" << this->simpleGenerators[i].ToString() << "\\\\";
  head << "\\end{array}";
  out << CGI::GetMathMouseHover(head.str());
  if (this->ExternalAutomorphisms.size>0)
  { out << "<br>Outer automorphisms: \n";
    Matrix<Rational> tempMat;
    head2 << "\\begin{array}{rcl}";
    for (int i=0; i<this->ExternalAutomorphisms.size; i++)
    { tempMat.AssignVectorsToRows(this->ExternalAutomorphisms[i]);
      tempMat.Transpose();
      head2 << "a_{" << i+1 << "}&:=&" << tempMat.ToString(&latexFormat) << "\\\\";
    }
    head2 << "\\end{array}";
    out << CGI::GetMathMouseHover(head2.str());
  }
  out << "<br>Half sum of the positive roots: " << this->GetRho().ToString();
  out << "<br>Roots of Borel (" << this->RootsOfBorel.size << " total): ";
  for (int i=0; i<this->RootsOfBorel.size; i++)
  { out << this->RootsOfBorel[i].ToString();
  }
  if (displayElements)
  { std::stringstream body;
    out << "<br>The elements of the weyl group of the subgroup written with minimal # of generators:<br>";
    body << "\\begin{array}{l}";
    for (int i=0; i<this->size; i++)
    { const ElementWeylGroup<WeylGroup>& currentElt=(*this)[i];
      body << currentElt.ToString(this->simpleGenerators.size, 0, &DisplayIndicesSimpleGenerators) << "\\\\";
    }
    body << "\\end{array}";
    out << CGI::GetMathMouseHover(body.str());
  }
  output=out.str();
}

bool SubgroupWeylGroupOLD::MakeParabolicFromSelectionSimpleRoots
(WeylGroup& inputWeyl, const Selection& ZeroesMeanSimpleRootSpaceIsInParabolic, GlobalVariables& theGlobalVariables, int UpperLimitNumElements)
{ MacroRegisterFunctionWithName("SubgroupWeylGroupOLD::MakeParabolicFromSelectionSimpleRoots");
  Vectors<Rational> selectedRoots;
  selectedRoots.ReservE(ZeroesMeanSimpleRootSpaceIsInParabolic.MaxSize- ZeroesMeanSimpleRootSpaceIsInParabolic.CardinalitySelection);
  this->AmbientWeyl=inputWeyl;
  if (this->AmbientWeyl.GetDim()!=ZeroesMeanSimpleRootSpaceIsInParabolic.MaxSize)
    crash << "This is a programming error: parabolic selection selects out of " << ZeroesMeanSimpleRootSpaceIsInParabolic.MaxSize
    << " elements while the weyl group is of rank " << this->AmbientWeyl.GetDim() << ". " << crash;
  for (int i=0; i<ZeroesMeanSimpleRootSpaceIsInParabolic.MaxSize; i++)
    if (!ZeroesMeanSimpleRootSpaceIsInParabolic.selected[i])
    { selectedRoots.SetSize(selectedRoots.size+1);
      selectedRoots.LastObject()->MakeEi(inputWeyl.GetDim(), i);
    }
  List<Vectors<Rational> > tempRootsCol;
  return this->ComputeSubGroupFromGeneratingReflections(&selectedRoots, &tempRootsCol, &theGlobalVariables, UpperLimitNumElements, true);
}

std::string SubgroupWeylGroupOLD::ElementToStringCosetGraph()
{ if (this->size<1)
    return "Error, non-initialized group";
  if (this->size==1)
    return "id";
  List<List<List<int> > > arrows;
  List<List<int> > Layers;
  Vector<Rational> tempRoot;
  Layers.ReservE(this->RepresentativesQuotientAmbientOrder.size);
  int GraphWidth=1;
  int oldLayerElementLength=-1;
  for (int i=0; i< this->RepresentativesQuotientAmbientOrder.size; i++)
  { if (this->RepresentativesQuotientAmbientOrder[i].generatorsLastAppliedFirst.size!=oldLayerElementLength)
    { Layers.SetSize(Layers.size+1);
      oldLayerElementLength=this->RepresentativesQuotientAmbientOrder[i].generatorsLastAppliedFirst.size;
    }
    Layers.LastObject()->AddOnTop(i);
    GraphWidth=MathRoutines::Maximum(GraphWidth, Layers.LastObject()->size);
  }
//  HashedList<Vector<Rational>> orbit;
//  orbit.Reserve(this->RepresentativesQuotientAmbientOrder.size);
  for (int i=0; i<this->RepresentativesQuotientAmbientOrder.size; i++)
  { tempRoot=this->AmbientWeyl.rho;
    this->AmbientWeyl.ActOnRootByGroupElement
    (this->AmbientWeyl.theElements.GetIndex(this->RepresentativesQuotientAmbientOrder[i]), tempRoot, false, false);
//    orbit.AddOnTop(tempRoot);
  }
  arrows.SetSize(Layers.size);
  for (int i=0; i< Layers.size; i++)
  { arrows[i].SetSize(Layers[i].size);
    for (int j=0; j<Layers[i].size; j++)
      for (int k=0; k<this->RepresentativesQuotientAmbientOrder.size; k++)
        if (this->AmbientWeyl.LeftIsHigherInBruhatOrderThanRight
        ( this->RepresentativesQuotientAmbientOrder[k], this->RepresentativesQuotientAmbientOrder[Layers[i][j]]))
          if (this->RepresentativesQuotientAmbientOrder[Layers[i][j]].generatorsLastAppliedFirst.size==this->RepresentativesQuotientAmbientOrder[k].generatorsLastAppliedFirst.size-1)
            arrows[i][j].AddOnTop(k);
  }
  return this->ElementToStringFromLayersAndArrows(arrows, Layers, GraphWidth, true);
}

void SubgroupWeylGroupOLD::FindQuotientRepresentatives(int UpperLimit)
{ this->AmbientWeyl.ComputeAllElements(UpperLimit);
  Vector<Rational> image1;
  this->RepresentativesQuotientAmbientOrder.size=0;
  this->RepresentativesQuotientAmbientOrder.ReservE(this->AmbientWeyl.theElements.size);
  for (int i=0; i<this->AmbientWeyl.theElements.size; i++)
  { image1=this->AmbientWeyl.rho;
    this->AmbientWeyl.ActOnRootByGroupElement(i, image1, false, false);
    bool isGood=true;
    for (int j=0; j<this->simpleGenerators.size; j++)
      if (this->AmbientWeyl.RootScalarCartanRoot(image1, this->simpleGenerators[j]).IsNegative())
      { isGood=false;
        break;
      }
    if (isGood)
      this->RepresentativesQuotientAmbientOrder.AddOnTop(this->AmbientWeyl.theElements[i]);
  }
}

bool SubgroupWeylGroupOLD::DrawContour
(const Vector<Rational>& highestWeightSimpleCoord, DrawingVariables& theDV, GlobalVariables& theGlobalVariables, int theColor, int UpperBoundVertices)
{ HashedList<Vector<Rational> > theOrbit;
  theOrbit.AddOnTop(highestWeightSimpleCoord);
  WeylGroup& theWeyl=this->AmbientWeyl;
  Vector<Rational> tempRoot;
  for (int i=0; i<theOrbit.size; i++)
    for (int j=0; j<this->simpleGenerators.size; j++)
    { tempRoot=theOrbit[i];
      theWeyl.ReflectBetaWRTAlpha(this->simpleGenerators[j], tempRoot, false, tempRoot);
      if (theOrbit.AddOnTopNoRepetition(tempRoot))
        theDV.drawLineBetweenTwoVectorsBuffer(theOrbit[i], tempRoot, DrawingVariables::PenStyleNormal, theColor);
      if (theOrbit.size>UpperBoundVertices)
        return false;
    }
  return true;
}

void SubgroupWeylGroupOLD::ComputeRootSubsystem()
{ MacroRegisterFunctionWithName("SubgroupWeylGroupOLD::ComputeRootSubsystem");
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
    crash << "This is a programming error. I am getting that the number of weights of a root system is odd. The generating set of simple weights is "
    << this->simpleGenerators.ToString() << ", and the generated weight subsystem is " << tempRoots.ToString() << crash;
  int numPosRoots=this->RootSubsystem.size/2;
  this->RootsOfBorel.SetSize(numPosRoots);
  for (int i=0; i<numPosRoots; i++)
    this->RootsOfBorel[i]=this->RootSubsystem[i+numPosRoots];
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
    out << "Kazhdan-Lusztig coefficients; the (w_1,w_2)  coefficient is defined as the multiplicity of " << CGI::GetMathSpanPure("L_{w_2 \\cdot \\lambda}")
    << " in " <<  CGI::GetMathSpanPure(" M_{w_1\\cdot \\lambda }  ") << " where \\cdot stands for the \\rho-modified action"
    << " of the Weyl group, \\lambda is a dominant integral weight, M_{\\lambda} stands for Verma module "
    << "of highest weight \\lambda, L_\\lambda stands for irreducible highest weight of highest weight \\lambda: <br><table border=\"1\"><tr><td>Weyl elt.</td>";
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
        crash << "This is a programming error: something wrong has happened. A weight that is supposed to "
        << " be in a certain Weyl group orbit isn't there. There is an error in the code, crashing accordingly. " << crash;
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
      if(tempRat1.IsEqualToZero() || tempRat2.IsEqualToZero())
        crash << crash;
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
        crash << "This is a programming error: an algorithmic check failed while computing R-polynomials. " << crash;
      ExploredFromTop[a]=true;
      a= this->FindMaximalBruhatNonExplored(ExploredFromTop);
    }
    this->Explored[this->LowestNonExplored]=true;
    this->LowestNonExplored= this->FindMinimalBruhatNonExplored(this->Explored);
  }
  //this->ComputeDebugString();
}

bool KLpolys::IndexGEQIndex(int a, int b)
{ /*ElementWeylGroup<WeylGroup>& eltA= (*this->TheWeylGroup)[a];
  ElementWeylGroup<WeylGroup>& eltB= (*this->TheWeylGroup)[b];
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
  const ElementWeylGroup<WeylGroup>& currentElement=this->TheWeylGroup->theElements[x];
  for (int i=currentElement.generatorsLastAppliedFirst.size-1; i>=0; i--)
    start=this->SimpleReflectionsActionList[start][this->TheWeylGroup->theElements[x].generatorsLastAppliedFirst[i]];
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
//  stOutput << " <br>Computing KL " << x << ", " << y << "; ";
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
      if ((this->TheWeylGroup->theElements[x].generatorsLastAppliedFirst.size+this->TheWeylGroup->theElements[i].generatorsLastAppliedFirst.size)%2==0)
        tempI=1;
      else
        tempI=-1;
      Rational powerQ= -this->TheWeylGroup->theElements[x].generatorsLastAppliedFirst.size+2*this->TheWeylGroup->theElements[i].generatorsLastAppliedFirst.size -
      this->TheWeylGroup->theElements[y].generatorsLastAppliedFirst.size;
      powerQ/=2;
      tempP2.MakeMonomiaL(0, powerQ, tempI, 1);
      tempP1*=tempP2;
      tempP1*=(this->theKLPolys[i][y]);
      if (!this->Explored[i])
        crash << "This is a programming error: an internal check during the Kazhdan-Lusztig polynomial computation fails. More precisely, while computing "
        << "KL poly of indices " << x << ", " << y << " I am using KL poly with indices " << i << ", " << y << " which hasn't been computed yet. "
        << crash;
      Accum+=tempP1;
    }
  this->theKLPolys[x][y].MakeZero();
  Rational lengthDiff= this->TheWeylGroup->theElements[y].generatorsLastAppliedFirst.size-this->TheWeylGroup->theElements[x].generatorsLastAppliedFirst.size;
  lengthDiff/=2;
//  stOutput << "Accum: " << Accum.ToString();
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
      crash << "This is a programming error: the computaion of R-polynomials is attempting to use a non-computed R-polynomial. " << crash;
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

void WeylGroup::TransformToSimpleBasisGenerators(Vectors<Rational>& theGens, const HashedList<Vector<Rational> >& inputRootSystem)
{ MacroRegisterFunctionWithName("WeylGroup::TransformToSimpleBasisGenerators");
  //stOutput << "<br>Transforming to simple basis: " << theGens.ToString();
  for (int i=0; i<theGens.size; i++)
    if (!theGens[i].IsPositiveOrZero())
      theGens[i].Minus();
  bool reductionOccured=true;
  Vector<Rational> tempRoot;
  while (reductionOccured)
  { reductionOccured= false;
    for (int i=0; i<theGens.size; i++)
      for (int j=i+1; j<theGens.size; j++)
      { tempRoot=theGens[i];
        tempRoot-=theGens[j];
        if (tempRoot.IsEqualToZero())
        { theGens.RemoveIndexSwapWithLast(j);
          reductionOccured=true;
        }
        if (inputRootSystem.Contains(tempRoot))
        { if (!tempRoot.IsPositiveOrZero())
          { tempRoot.Minus();
            theGens[j]=tempRoot;
          } else
            theGens[i]=tempRoot;
          reductionOccured=true;
        }
      }
  }
//  stOutput << "<br><br>Resulting simple basis: " << theGens.ToString();
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
      crash << "This is a programming error: the vector produced by PerturbNoZeroScalarProductWithMe, namely, "
      << this->ToString() << " is orthogonal to input vector " << inputVectors[i].ToString() << ". The full list of vectors is "
      << inputVectors.ToString() << crash;
}

void WeylGroup::TransformToSimpleBasisGeneratorsArbitraryCoords(Vectors<Rational>& theGens, const HashedList<Vector<Rational> >& inputRootSystem)
{ if (theGens.size==0)
    return;
  MacroRegisterFunctionWithName("WeylGroup::TransformToSimpleBasisGeneratorsArbitraryCoords");
//  stOutput << "<hr>Transforming to simple " << theGens.ToString() << " with root system " << inputRootSystem.ToString();
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
//  stOutput << ". Final basis: " << theGens.ToString();
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

void WeylGroup::ComputeExtremeRootInTheSameKMod(const Vectors<Rational>& inputSimpleBasisK, const Vector<Rational>& inputRoot, Vector<Rational>& output, bool lookingForHighest)
{ MacroRegisterFunctionWithName("WeylGroup::ComputeExtremeRootInTheSameKMod");
  output=inputRoot;
  Vector<Rational> tempRoot;
  bool FoundHigher=true;
  do
  { FoundHigher=false;
    for (int i=0; i<inputSimpleBasisK.size; i++)
    { tempRoot=output;
      if (lookingForHighest)
        tempRoot+=(inputSimpleBasisK[i]);
      else
        tempRoot-=(inputSimpleBasisK[i]);
      if (this->RootSystem.GetIndex(tempRoot)!=-1)
      { output=tempRoot;
        FoundHigher=true;
      }
      if (tempRoot.IsEqualToZero())
      { output*=-1;
        FoundHigher=true;
      }
    }
  } while (FoundHigher);
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
