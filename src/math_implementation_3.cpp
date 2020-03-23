// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "general_lists.h"
#include "math_general_implementation.h"
#include "math_general_polynomial_computations_basic_implementation.h"
#include "math_general_polynomial_computations_advanced_implementation.h"
#include "math_extra_finite_groups_implementation.h"
#include "math_extra_symmetric_groups_and_generalizations.h"
#include "math_extra_drawing_variables.h"
#include "string_constants.h"
#include <dirent.h>

// The below gives upper limit to the amount of pointers
// that are allowed to be allocated by the program. Can be changed dynamically.
// Used to guard the web server from abuse.
#ifdef AllocationLimitsSafeguard
long long ParallelComputing::cgiLimitRAMuseNumPointersInList = 2000000000;
#endif

ControllerStartsRunning ParallelComputing::controllerSignalPauseUseForNonGraciousExitOnly;
bool ParallelComputing::flagUngracefulExitInitiated = false;

long long ParallelComputing::GlobalPointerCounter = 0;
long long ParallelComputing::PointerCounterPeakRamUse = 0;
unsigned int ParallelComputing::NumHashResizes = 0;
unsigned int ParallelComputing::NumListResizesTotal = 0;
unsigned int ParallelComputing::NumListsCreated = 0;

//CombinatorialChamberContainer GlobalCollectorChambers;
//FacetPointers GlobalCollectorFacets;

Vector<Rational> PartFraction::theVectorToBePartitioned;
bool PartFraction::MakingConsistencyCheck = false;
Rational PartFractions::CheckSum;
Rational PartFraction::CheckSum;
Rational PartFraction::CheckSum2;

int HtmlRoutines::GlobalMathSpanID = 0;
int HtmlRoutines::GlobalCanvasID = 0;
int HtmlRoutines::GlobalGeneralPurposeID = 0;

template < > bool Matrix<Rational>::flagComputingDebugInfo = true;
template < > bool Polynomial<Rational>::flagAnErrorHasOccuredTimeToPanic = true;

template < > bool CompleX<double>::flagEqualityIsApproximate = true;
template < > double CompleX<double>::EqualityPrecision = 0.00000001;

template <class ElementLeft, class ElementRight, class coefficient>
class TensorProductMonomial;

std::fstream PartFraction::TheBigDump;
std::fstream PartFractions::ComputedContributionsList;

//template < > int ListPointers<PartFraction>::MemoryAllocationIncrement =100;
//ListPointers<PartFraction> PartFraction::GlobalCollectorPartFraction;
bool PartFraction::UseGlobalCollector = true;
bool PartFraction::flagAnErrorHasOccurredTimeToPanic = false;
bool PartFractions::flagSplitTestModeNoNumerators = false;
bool PartFractions::flagAnErrorHasOccurredTimeToPanic = false;
bool PartFractions::flagUsingCheckSum = true;
int PartFractions::NumMonomialsInNumeratorsRelevantFractions = 0;
int PartFractions::NumProcessedForVPFMonomialsTotal = 0;
int PartFractions::flagMaxNumStringOutputLines = 500;
//int PartFraction::lastApplicationOfSVformulaNumNewGenerators = 0;
//int PartFraction::lastApplicationOfSVformulaNumNewMonomials = 0;
bool PartFractions::flagMakingProgressReport = true;
//FacetPointers TheBigFacetOutput;
//DrawingVariables TDV(200, 400);

unsigned long long int Rational::TotalLargeAdditions = 0;
unsigned long long int Rational::TotalLargeGCDcalls = 0;
unsigned long long int Rational::TotalLargeMultiplications = 0;
unsigned long long int Rational::TotalSmallAdditions = 0;
unsigned long long int Rational::TotalSmallGCDcalls = 0;
unsigned long long int Rational::TotalSmallMultiplications = 0;

int GlobalVariables::CallSystemNoOutput(const std::string& systemCommand, bool logErrors) {
  if (this->pointerCallSystemNoOutput == nullptr) {
    return - 1;
  }
  int exitCode = this->pointerCallSystemNoOutput(systemCommand);
  if (exitCode != 0 && logErrors) {
    global << logger::red << "System command: " << systemCommand << " exited with " << exitCode << ". " << logger::endL;
  }
  return exitCode;
}

std::string GlobalVariables::CallSystemWithOutput(const std::string& systemCommand) {
  if (this->pointerCallSystemWithOutput == nullptr) {
    return "Error";
  }
  return this->pointerCallSystemWithOutput(systemCommand);
}

void GlobalVariables::ChDir(const std::string& systemCommand) {
  if (this->pointerCallChDir != nullptr) {
    this->pointerCallChDir(FileOperations::ConvertStringToEscapedStringFileNameSafe(systemCommand));
  }
}

bool GlobalVariables::Response::TimedOut() {
  return this->flagTimedOut;
}

bool GlobalVariables::Response::MonitoringAllowed() {
  return !this->flagBanProcessMonitorinG && this->flagReportAllowed;
}

bool GlobalVariables::Response::ReportDesired(int type) {
  (void) type;
  return this->MonitoringAllowed() && this->flagTimedOut && this->flagReportDesired;
}

void GlobalVariables::Response::AllowReport() {
  this->flagReportAllowed = true;
}

void GlobalVariables::Response::DisallowReport() {
  this->flagReportAllowed = false;
}

GlobalVariables::Response::Response() {
  this->flagBanProcessMonitorinG = false;
  this->flagReportDesired = true;
  this->flagTimedOut = false;
  this->flagReportAllowed = false;
 }

GlobalVariables::GlobalVariables() {
  this->flagDeallocated = false;
  this->flagNotAllocated = false;
  this->flagCertificatesAreOfficiallySigned = false;
  this->flagIsChildProcess = false;
  this->flagRestartNeeded = false;
  this->flagStopNeeded = false;
  this->millisecondsMaxComputation = 100000; //100 seconds
  this->millisecondOffset = 0;
  this->millisecondsComputationStart = - 1;
  this->millisecondsReplyAfterComputation = 5000; //5 seconds
  this->pointerCallSystemWithOutput = nullptr;
  this->pointerCallSystemNoOutput = nullptr;
  this->pointerCallChDir = nullptr;
  this->flagServerForkedIntoWorker = false;
  this->flagComputationCompletE = false;
  this->flagComputationFinishedAllOutputSentClosing = false;
  this->flagRunningConsoleTest = false;
  this->flagRunningConsoleRegular = false;
  this->flagRunningBuiltInWebServer = false;
  this->flagRunningConsoleHelp = false;
  this->flagLoggedIn = false;
  this->flagLogInAttempted = false;
  this->flagServerDetailedLog = false;
  this->flagUsingSSLinCurrentConnection = false;
  this->flagSSLIsAvailable = false;

  this->millisecondsNoPingBeforeChildIsPresumedDead = 10000;
  this->MutexReturnBytes.mutexName = "WriteBytesLock";
  this->flagCachingInternalFilesOn = true;
  this->flagRunServerOnEmptyCommandLine = false;
  this->flagRequestComingLocally = false;
  this->flagDatabaseCompiled = false;
  this->flagServerAutoMonitor = true;
  this->flagDisableDatabaseLogEveryoneAsAdmin = false;
}

bool ProgressReport::TickAndWantReport() {
  if (!this->flagInitialized) {
    return false;
  }
  // Increments and internal counter and returns
  // desire [true/false] to display a progress report.
  //
  // Note: this function will not generate a
  // report on first run when
  // this->ticksPerReport is larger than 1.
  this->ticks ++;
  if (this->ticks >= this->ticksPerReport) {
    this->ticks = 0;
  }
  return this->ticks == 0;
}

void ProgressReport::Report(const std::string& theReport) {
  if (!this->flagInitialized) {
    return;
  }
  if (global.fatal.flagCrashInitiateD) {
    this->threadIndex = - 1;
    return;
  }
  if (this->threadIndex == - 1) {
    return;
  }
  if (global.progressReportStrings[this->threadIndex].size > this->currentLevel) {
    global.progressReportStrings[this->threadIndex][this->currentLevel] = theReport;
    global.MakeReport();
  }
}

void ProgressReport::init() {
  this->flagInitialized = false;
  if (!global.theResponse.MonitoringAllowed()) {
    return;
  }
  if (global.fatal.flagCrashInitiateD) {
    this->threadIndex = - 1;
    return;
  }
  this->threadIndex = ThreadData::getCurrentThreadId();
  if (this->threadIndex == - 1) {
    return;
  }
  this->flagInitialized = true;
  this->currentLevel = global.progressReportStrings[this->threadIndex].size;
  global.progressReportStrings[this->threadIndex].AddOnTop(std::string(""));
  this->ticks = 0;
  this->ticksPerReport = 1;
  this->reportType = GlobalVariables::Response::ReportType::general;
}

ProgressReport::~ProgressReport() {
  if (!this->flagInitialized) {
    return;
  }
  if (global.fatal.flagCrashInitiateD) {
    this->threadIndex = - 1;
    return;
  }
  if (this->threadIndex == - 1) {
    return;
  }
  if (global.fatal.flagCrashInitiateD) {
    return;
  }
  global.progressReportStrings[this->threadIndex].size --;
}

RegisterFunctionCall::RegisterFunctionCall(const char* fileName, int line, const std::string& functionName) {
  this->threadIndex = ThreadData::getCurrentThreadId();
  if (this->threadIndex == - 1) {
    return;
  }
  ListReferences<stackInfo>& theStack = global.CustomStackTrace[this->threadIndex];
  theStack.SetSize(theStack.size + 1);
  stackInfo& stackTop = theStack.LastObject();
  stackTop.fileName = fileName;
  stackTop.line = line;
  stackTop.functionName = functionName;
  if (theStack.size > 200000) {
    global.fatal << "Stack too deep: 200000 layers. " << global.fatal;
  }
}

RegisterFunctionCall::~RegisterFunctionCall() {
  if (this->threadIndex == - 1) {
    return;
  }
  global.CustomStackTrace[this->threadIndex].size --;
}

int DrawingVariables::GetColorFromChamberIndex(int index) {
  static const int NumColorsBase = 3;
  int tempI = index % (NumColorsBase * NumColorsBase * NumColorsBase);
  if (tempI < 0) {
    tempI += (NumColorsBase * NumColorsBase * NumColorsBase);
  }
  int r = (255 * (tempI % NumColorsBase)) / NumColorsBase;
  int g = (255 * (tempI % (NumColorsBase * NumColorsBase))) / (NumColorsBase * NumColorsBase);
  int b = (255 * (tempI % (NumColorsBase * NumColorsBase * NumColorsBase))) / (NumColorsBase * NumColorsBase * NumColorsBase);
  return static_cast<int>(HtmlRoutines::RedGreenBlue(
    static_cast<unsigned int>(r),
    static_cast<unsigned int>(g),
    static_cast<unsigned int>(b)
  ));
}

void DrawingVariables::initDrawingVariables() {
  this->DefaultHtmlHeight = 400;
  this->DefaultHtmlWidth = 400;
  this->theDrawLineFunction = nullptr;
  this->theDrawTextFunction = nullptr;
  this->theDrawCircleFunction = nullptr;
  this->theDrawClearScreenFunction = nullptr;
  this->fontSizeNormal = 10;
  this->fontSizeSubscript = 6;
  this->flagLaTeXDraw = false;
  this->flagDisplayingCreationNumbersInsteadOfDisplayNumbers = false;
  this->flagDrawChamberIndices = true;
  this->flagDrawingInvisibles = false;
  this->flagDrawingLinkToOrigin = true;
  this->flagFillUserDefinedProjection = false;
  this->ColorDashes = static_cast<int>(HtmlRoutines::RedGreenBlue(200, 200, 200));
  this->flag2DprojectionDraw = true;
  this->flagIncludeExtraHtmlDescriptions = true;
  this->flagAllowMovingCoordinateSystemFromArbitraryClick = true;
  this->ColorChamberIndicator = static_cast<int>(HtmlRoutines::RedGreenBlue(220, 220, 0));
  this->ColorWeylChamberWalls = static_cast<int>(HtmlRoutines::RedGreenBlue(220, 220, 0));
  this->ColorTextPermanentlyZeroChamber = static_cast<int>(HtmlRoutines::RedGreenBlue(250, 220, 220));
  this->ColorTextZeroChamber = static_cast<int>(HtmlRoutines::RedGreenBlue(200, 100, 100));
  this->ColorTextDefault = static_cast<int>(HtmlRoutines::RedGreenBlue(0, 0, 0));
  this->Selected = - 2;
  this->textX = 0;
  this->textY = 15;
  this->flagPlotShowJavascriptOnly = false;
  this->flagUseGraphicsOld = false;
  this->theBuffer.init();
}

std::stringstream  HtmlRoutines::outputStream;
int HtmlRoutines::numLinesAll;
int HtmlRoutines::shiftX = 0;
int HtmlRoutines::numDashedLines = 0;
int HtmlRoutines::numRegularLines = 0;
int HtmlRoutines::numDottedLines = 0;
int HtmlRoutines::shiftY = - 200;
int HtmlRoutines::scale = 100;

std::string HtmlRoutines::CleanUpForLaTeXLabelUse(const std::string& inputString) {
  std::stringstream out;
  for (unsigned i = 0; i < inputString.size(); i ++) {
    if (
      inputString[i] != '/' &&
      inputString[i] != '^' &&
      inputString[i] != '_' &&
      inputString[i] != '{' &&
      inputString[i] != '}'
    ) {
      out << inputString[i];
    }
  }
  return out.str();
}

void HtmlRoutines::clearDollarSigns(std::string& theString, std::string& output) {
  std::stringstream out;
  for (unsigned int i = 0; i < theString.size(); i ++) {
    if (theString[i] != '$') {
      out << theString[i];
    }
  }
  output = out.str();
}

std::string HtmlRoutines::DoubleBackslashes(const std::string& input) {
  std::stringstream out;
  for (unsigned i = 0; i < input.size(); i ++) {
    out << input[i];
    if (input[i] == '\\') {
      out << "\\";
    }
  }
  return out.str();
}

std::string HtmlRoutines::clearNewLines(const std::string& input) {
  std::stringstream out;
  for (unsigned i = 0; i < input.size(); i ++) {
    if (input[i] == '\n' || input[i] == '\r') {
      out << " ";
    } else {
      out << input[i];
    }
  }
  return out.str();
}

std::string HtmlRoutines::backslashQuotes(const std::string& input) {
  std::stringstream out;
  for (unsigned i = 0; i < input.size(); i ++) {
    if (input[i] == '"') {
      out << "\\";
    }
    out << input[i];
  }
  return out.str();
}

std::string HtmlRoutines::clearSlashes(const std::string& theString) {
  std::stringstream out;
  for (unsigned int i = 0; i < theString.size(); i ++) {
    if (theString[i] != '\\') {
      out << theString[i];
    }
  }
  return out.str();
}

void HtmlRoutines::subEqualitiesWithSimeq(
  std::string& theString, std::string& output
) {
  std::stringstream out;
  for (unsigned int i = 0; i < theString.size(); i ++) {
    if (theString[i] != '=') {
      out << theString[i];
    } else {
      out << "\\simeq ";
    }
  }
  output = out.str();
}

void HtmlRoutines::ElementToStringTooltip(
  const std::string& input,
  const std::string& inputTooltip,
  std::string& output,
  bool useHtml
) {
  std::stringstream out;
  if (useHtml) {
    out << "<span title =\"" << inputTooltip << "\">" << input << "</span>";
  }
  output = out.str();
}

std::string HtmlRoutines::GetStyleButtonLikeHtml() {
  return " style =\"background:none; border:0; text-decoration:underline; color:blue; cursor:pointer\" ";
}

std::string HtmlRoutines::ConvertStringEscapeQuotesAndBackslashes(const std::string& input) {
  MacroRegisterFunctionWithName("HtmlRoutines::ConvertStringToBackslashEscapedString");
  std::stringstream out;
  for (unsigned i = 0; i < input.size(); i ++) {
    if (input[i] == '"') {
      out << "\\\"";
    } else if (input[i] == '\\') {
      out << "\\\\";
    } else {
      out << input[i];
    }
  }
  return out.str();
}

std::string StringRoutines::ConvertStringToJSONString(const std::string &input) {
  MacroRegisterFunctionWithName("StringRoutines::ConvertStringToJSONString");
  std::stringstream out;
  for (unsigned i = 0; i < input.size(); i ++) {
    if (input[i] == '"') {
      out << "\\\"";
    } else if (input[i] == '\\') {
      out << "\\\\";
    } else if (input[i] == '\n') {
      out << "\\n";
    } else if (StringRoutines::isASCIICharacterVisible(input[i])) {
      out << input[i];
    } else {
      out << "\\u00" << StringRoutines::ConvertByteToHex(static_cast<unsigned char>(input[i]));
    }
  }
  return out.str();
}

std::string StringRoutines::ConvertStringToJavascriptVariable(const std::string& input) {
  MacroRegisterFunctionWithName("StringRoutines::ConvertStringForJavascript");
  std::stringstream out;
  for (unsigned i = 0; i < input.size(); i ++) {
    if (MathRoutines::isADigit(input[i]) || MathRoutines::isALatinLetter(input[i])) {
      out << input[i];
    } else {
      out << "_" << StringRoutines::ConvertByteToHex(static_cast<unsigned char>(input[i]));
    }
  }
  return out.str();

}

std::string StringRoutines::ConvertStringToJavascriptString(const std::string& input) {
  MacroRegisterFunctionWithName("StringRoutines::ConvertStringForJavascript");
  std::stringstream out;
  for (unsigned i = 0; i < input.size(); i ++) {
    if (input[i] == '"') {
      out << "\\\"";
    } else if (input[i] == '\\') {
      out << "\\\\";
    } else if (input[i] == '\n') {
      out << "\\n";
    } else if (StringRoutines::isASCIICharacterVisible(input[i])) {
      out << input[i];
    } else {
      out << "\\x" << StringRoutines::ConvertByteToHex(static_cast<unsigned char>(input[i]));
    }
  }
  return out.str();
}

std::string HtmlRoutines::ConvertStringEscapeNewLinesQuotesBackslashes(const std::string& input) {
  MacroRegisterFunctionWithName("HtmlRoutines::ConvertStringEscapeNewLinesQuotesBackslashes");
  std::stringstream out;
  for (unsigned i = 0; i < input.size(); i ++) {
    if (input[i] == '"') {
      out << "\\\"";
    } else if (input[i] == '\\') {
      out << "\\\\";
    } else if (input[i] == '\n') {
      out << "\\n";
    } else {
      out << input[i];
    }
  }
  return out.str();
}

std::string HtmlRoutines::ConvertStringToHtmlStringRestrictSize(
  const std::string& theString, bool doReplaceNewLineByBr, int maxStringSize
) {
  std::string result = HtmlRoutines::ConvertStringToHtmlString(theString, doReplaceNewLineByBr);
  if (maxStringSize > 0) {
    if (static_cast<signed>(result.size()) > maxStringSize) {
      std::stringstream resultStream;
      resultStream << result.substr(0, static_cast<unsigned>(maxStringSize))
      << "...[only first " << maxStringSize << " chars shown]. ";
      return resultStream.str();
    }
  }
  return result;
}

std::string HtmlRoutines::ConvertStringToHtmlString(const std::string& theString, bool doReplaceNewLineByBr) {
  std::string result;
  HtmlRoutines::ConvertStringToHtmlStringReturnTrueIfModified(theString, result, doReplaceNewLineByBr);
  return result;
}

uint32_t HtmlRoutines::RedGreenBlue(unsigned int r, unsigned int g, unsigned int b) {
  r = r % 256;
  g = g % 256;
  b = b % 256;
  return r * 65536 + g * 256 + b;
}

bool FileOperations::IsFolderUnsecure(const std::string& theFolderName) {
  MacroRegisterFunctionWithName("FileOperations::IsFolderUnsecure");
  DIR *pDir;
  pDir = opendir(theFolderName.c_str());
  if (pDir != nullptr) {
    closedir(pDir);
    return true;
  }
  return false;
}

std::string FileOperations::GetFileExtensionWithDot(
  const std::string& theFileName, std::string* outputFileNameNoExtension
) {
  MacroRegisterFunctionWithName("FileOperations::GetFileExtensionWithDot");
  if (theFileName == "" || theFileName.size() <= 0) {
    return "";
  }
  if (&theFileName == outputFileNameNoExtension) {
    std::string theCopy = theFileName;
    return FileOperations::GetFileExtensionWithDot(theCopy, outputFileNameNoExtension);
  }
  for (int i = static_cast<signed>(theFileName.size() - 1); i >= 0; i --) {
    if (theFileName[static_cast<unsigned>(i)] == '.') {
      if (outputFileNameNoExtension != nullptr) {
        *outputFileNameNoExtension = theFileName.substr(0, static_cast<unsigned>(i));
      }
      return theFileName.substr(static_cast<unsigned>(i), std::string::npos);
    }
  }
  return "";
}

std::string FileOperations::ConvertStringToLatexFileName(const std::string& input) {
  MacroRegisterFunctionWithName("FileOperations::ConvertStringToLatexFileName");
  std::stringstream out;
  for (unsigned i = 0; i < input.size(); i ++) {
    if (MathRoutines::isADigit(input[i]) || MathRoutines::isALatinLetter(input[i])) {
      out << input[i];
    } else if (input[i] == ' ' || input[i] == ':') {
      out << "_";
    } else {
      out << "_" << static_cast<int>(input[i]);
    }
  }
  std::string result = out.str();
  StringRoutines::StringTrimToLengthWithHash(result, 220);
  return result;
}

std::string FileOperations::ConvertStringToEscapedStringFileNameSafe(const std::string& input) {
  MacroRegisterFunctionWithName("FileOperations::ConvertStringToEscapedStringFileNameSafe");
  std::stringstream out;
  for (unsigned i = 0; i < input.size(); i ++) {
    if (input[i] == ' ') {
      out << " ";
    } else if (input[i] == '"') {
      out << "\\\"";
    } else if (input[i] == '\\') {
      out << "\\\\";
    } else {
      out << input[i];
    }
  }
  return out.str();
}

bool FileOperations::IsOKfileNameVirtual(
  const std::string& theFileName,
  bool accessSensitiveFolders,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("FileOperations::IsOKfileNameVirtual");
  (void) accessSensitiveFolders;
  std::string theFileNameNoPath = FileOperations::GetFileNameFromFileNameWithPath(theFileName);
  std::string theFilePath = FileOperations::GetPathFromFileNameWithPath(theFileName);
  if (theFilePath.size() > 10000000) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Invalid file name: too long. ";
    }
    return false;
  }
  if (theFilePath.size() > 0) {
    if (theFilePath[0] == '.') {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Invalid file name: "
        << theFileName << ": starts with dot but not with ./. ";
      }
      return false;
    }
  }
  for (unsigned i = 0; i < theFilePath.size(); i ++) {
    if (theFilePath[i] == '.') {
      if (i + 1 < theFilePath.size()) {
        if (theFilePath[i + 1] == '.') {
          if (commentsOnFailure != nullptr) {
            *commentsOnFailure << "Invalid file name: "
            << theFileName << ": has two consecutive dots. ";
          }
          return false;
        }
      }
    }
  }
  if (theFileNameNoPath.size() > 0) {
    if (theFileNameNoPath[0] == '.') {
      if (commentsOnFailure != nullptr) {
       *commentsOnFailure << "Invalid file name: " << theFileName << ": starts with dot. ";
      }
      return false;
    }
  }
  return true;
}

bool FileOperations::IsFileNameWithoutDotsAndSlashes(const std::string& theFileName) {
  MacroRegisterFunctionWithName("FileOperations::IsFileNameWithoutDotsAndSlashes");
  for (unsigned i = 0; i < theFileName.size(); i ++) {
    if (theFileName[i] == '/' || theFileName[i] == '\\' || theFileName[i] == '.') {
      return false;
    }
  }
  return true;
}

List<bool> FileOperations::safeFileCharacters;
List<bool>& FileOperations::GetSafeFileChars() {
  if (FileOperations::safeFileCharacters.size == 0) {
    FileOperations::safeFileCharacters.initializeFillInObject(256, false);
    std::string theChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    theChars += "0123456789";
    theChars += "@";
    theChars += "+-/=._%";
    for (unsigned i = 0; i < theChars.size(); i ++) {
      FileOperations::safeFileCharacters[
        static_cast<int>(static_cast<unsigned int>(static_cast<unsigned char>(theChars[i])))
      ] = true;
    }
  }
  return FileOperations::safeFileCharacters;
}

std::string FileOperations::CleanUpForFileNameUse(const std::string& inputString) {
  std::stringstream out;
  for (unsigned i = 0; i < inputString.size(); i ++) {
    if (inputString[i] == '/') {
      out << "_slash_";
    } else if (inputString[i] == '.') {
      out << "_dot_";
    } else {
      out << inputString[i];
    }
  }
  return out.str();
}

bool FileOperations::IsFileNameSafeForSystemCommands(
  const std::string& theFileName, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("FileOperations::IsFileNameSafeForSystemCommands");
  const unsigned maxAllowedFileNameSize = 1000;
  if (theFileName.size() > maxAllowedFileNameSize) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "File name has length: " << theFileName.size()
      << "; max allowed file name size is: " << maxAllowedFileNameSize;
    }
    return false;
  }
  for (unsigned i = 0; i < theFileName.size(); i ++) {
    if (!FileOperations::GetSafeFileChars()[theFileName[i]]) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Character: " << theFileName[i] << " not allowed in file name. ";
      }
      return false;
    }
  }
  return true;
}

std::string FileOperations::GetFileNameFromFileNameWithPath(const std::string& fileName) {
  unsigned startNameWithoutFolderInfo = 0;
  for (unsigned i = 0; i < fileName.size(); i ++) {
    if (fileName[i] == '/' || fileName[i] == '\\') {
      startNameWithoutFolderInfo = i + 1;
    }
  }
  std::stringstream nameWithoutFolderInfo;
  for (unsigned i = startNameWithoutFolderInfo; i < fileName.size(); i ++) {
    nameWithoutFolderInfo << fileName[i];
  }
  return nameWithoutFolderInfo.str();
}

std::string FileOperations::GetPathFromFileNameWithPath(const std::string& fileName) {
  unsigned startNameWithoutFolderInfo = 0;
  for (unsigned i = 0; i < fileName.size(); i ++) {
    if (fileName[i] == '/' || fileName[i] == '\\') {
      startNameWithoutFolderInfo = i + 1;
    }
  }
  std::stringstream folderName;
  for (unsigned i = 0; i < startNameWithoutFolderInfo; i ++) {
    folderName << fileName[i];
  }
  return folderName.str();
}

bool FileOperations::GetFolderFileNamesVirtual(
  const std::string& theFolderName,
  List<std::string>& outputFileNamesNoPath,
  List<std::string>* outputFileTypesWithDot,
  bool accessSensitiveFolders,
  bool accessULTRASensitiveFolders,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("FileOperations::GetFolderFileNamesVirtual");
  std::string computedFolderName;
  if (!FileOperations::GetPhysicalFileNameFromVirtual(
    theFolderName,
    computedFolderName,
    accessSensitiveFolders,
    accessULTRASensitiveFolders,
    commentsOnFailure
  )) {
    return false;
  }
  return FileOperations::GetFolderFileNamesUnsecure(
    computedFolderName, outputFileNamesNoPath, outputFileTypesWithDot
  );
}

bool FileOperations::GetFolderFileNamesUnsecure(
  const std::string& theFolderName,
  List<std::string>& outputFileNamesNoPath,
  List<std::string>* outputFileTypesWithDot
) {
  MacroRegisterFunctionWithName("FileOperations::GetFolderFileNamesUnsecure");
  DIR *theDirectory = opendir(theFolderName.c_str());
  if (theDirectory == nullptr) {
    return false;
  }
  outputFileNamesNoPath.Reserve(1000);
  if (outputFileTypesWithDot != nullptr) {
    outputFileTypesWithDot->Reserve(1000);
  }
  std::string fileNameNoPath, fullName, theExtension;
  for (
    dirent *fileOrFolder = readdir(theDirectory);
    fileOrFolder != nullptr;
    fileOrFolder = readdir(theDirectory)
  ) {
    outputFileNamesNoPath.AddOnTop(fileOrFolder->d_name);
    if (outputFileTypesWithDot != nullptr) {
      fileNameNoPath = fileOrFolder->d_name;
      fullName = theFolderName + fileNameNoPath;
      if (FileOperations::IsFolderUnsecure(fullName)) {
        outputFileTypesWithDot->AddOnTop(".d");
      } else {
        theExtension = FileOperations::GetFileExtensionWithDot(fileNameNoPath);
        if (theExtension == ".d") {
          theExtension = "";
        }
        outputFileTypesWithDot->AddOnTop(theExtension);
      }
    }
  }
  closedir (theDirectory);
  return true;
}

bool FileOperations::LoadFileToStringVirtualCustomizedReadOnly(
  const std::string& theFileName,
  std::string& output,
  std::stringstream* commentsOnFailure
) {
  if (theFileName == "") {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Empty file name not allowed. ";
    }
    return false;
  }
  std::string computedFileName;
  if (!FileOperations::GetPhysicalFileNameFromVirtualCustomizedReadOnly(
    theFileName, computedFileName, commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Failed to extract physical file name from the virtual file name: "
      << theFileName;
    }
    return false;
  }
  return FileOperations::LoadFileToStringUnsecure(
    computedFileName, output, commentsOnFailure
  );
}

bool FileOperations::WriteFileVirual(
  const std::string& fileNameVirtual,
  const std::string& fileContent,
  std::stringstream* commentsOnError
) {
  return FileOperations::WriteFileVirualWithPermissions(
    fileNameVirtual, fileContent, false, commentsOnError
  );
}

bool FileOperations::WriteFileVirualWithPermissions(
  const std::string& fileNameVirtual,
  const std::string& fileContent,
  bool accessSensitiveFolders,
  std::stringstream* commentsOnError
) {
  return FileOperations::WriteFileVirualWithPermissions_AccessUltraSensitiveFoldersIfNeeded(
    fileNameVirtual, fileContent, accessSensitiveFolders, false, commentsOnError
  );
}

bool FileOperations::WriteFileVirualWithPermissions_AccessUltraSensitiveFoldersIfNeeded(
  const std::string& fileNameVirtual,
  const std::string& fileContent,
  bool accessSensitiveFolders,
  bool accessUltraSensistiveFolders,
  std::stringstream* commentsOnError
) {
  std::fstream theFile;
  if (!FileOperations::OpenFileCreateIfNotPresentVirtualCreateFoldersIfNeeded_UltraSensitiveOptions(
    theFile,
    fileNameVirtual,
    false,
    true,
    false,
    accessSensitiveFolders,
    accessUltraSensistiveFolders
  )) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "Filed to open file. ";
    }
    return false;
  }
  theFile << fileContent;
  return true;
}

std::string FileOperations::WriteFileReturnHTMLLink(
  const std::string& fileContent, const std::string& fileNameVirtual, const std::string& linkText
) {
  MacroRegisterFunctionWithName("Calculator::WriteFileReturnHTMLLink");
  std::stringstream commentsOnError;
  bool success = FileOperations::WriteFileVirual(fileNameVirtual, fileContent, &commentsOnError);
  if (!success) {
    global.fatal << "Failed to write file. " << commentsOnError.str() << global.fatal;
  }
  std::stringstream out;
  out << "<a href=\"" << fileNameVirtual << "\" target = \"_blank\">" << linkText << "</a>";
  return out.str();
}

bool FileOperations::LoadFileToStringVirtual(
  const std::string& theFileName,
  std::string& output,
  bool accessSensitiveFolders,
  std::stringstream* commentsOnFailure
) {
  return FileOperations::LoadFileToStringVirtual_AccessUltraSensitiveFoldersIfNeeded(
    theFileName, output, accessSensitiveFolders, false, commentsOnFailure
  );
}

bool FileOperations::LoadFileToStringVirtual_AccessUltraSensitiveFoldersIfNeeded(
  const std::string& theFileName,
  std::string& output,
  bool accessSensitiveFolders,
  bool accessULTRASensitiveFolders,
  std::stringstream* commentsOnFailure
) {
  std::string computedFileName;
  if (!FileOperations::GetPhysicalFileNameFromVirtual(
    theFileName, computedFileName, accessSensitiveFolders, accessULTRASensitiveFolders, commentsOnFailure
  )) {
    return false;
  }
  return FileOperations::LoadFileToStringUnsecure(computedFileName, output, commentsOnFailure);
}

bool FileOperations::LoadFileToStringUnsecure(
  const std::string& fileNameUnsecure, std::string& output, std::stringstream* commentsOnFailure
) {
  if (!FileOperations::FileExistsUnsecure(fileNameUnsecure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "The requested file "
      << HtmlRoutines::ConvertStringToHtmlString(fileNameUnsecure, false)
      << " does not appear to exist. ";
    }
    return false;
  }
  std::ifstream theFile;
  if (!FileOperations::OpenFileUnsecureReadOnly(theFile, fileNameUnsecure, false)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "The requested file "
      << HtmlRoutines::ConvertStringToHtmlString(fileNameUnsecure, false)
      << " exists but I failed to open it in text mode (perhaps not a valid ASCII/UTF8 file). ";
    }
    return false;
  }
  std::stringstream contentStream;
  contentStream << theFile.rdbuf();
  output = contentStream.str();
  return true;
}

#include "general_list_references.h"

MapList<std::string, std::string, MathRoutines::HashString>&
FileOperations::FolderVirtualLinksNonSensitive() {
  static MapList<std::string, std::string, MathRoutines::HashString> result;
  return result;
}

HashedList<std::string, MathRoutines::HashString>&
FileOperations::FolderStartsToWhichWeAppendInstructorUsernameSlash() {
  static HashedList<std::string, MathRoutines::HashString> result;
  return result;
}

HashedList<std::string, MathRoutines::HashString>&
FileOperations::FolderVirtualLinksToWhichWeAppendTimeAndBuildHash() {
  static HashedList<std::string, MathRoutines::HashString> result;
  return result;
}

MapList<std::string, std::string, MathRoutines::HashString>&
FileOperations::FolderVirtualLinksSensitive() {
  static MapList<std::string, std::string, MathRoutines::HashString> result;
  return result;
}

MapList<std::string, std::string, MathRoutines::HashString>&
FileOperations::FolderVirtualLinksULTRASensitive() {
  static MapList<std::string, std::string, MathRoutines::HashString> result;
  return result;
}

List<List<std::string> >& FileOperations::FolderVirtualLinksDefault() {
  static List<List<std::string> > result;
  return result;
}

void FileOperations::InitializeFoldersULTRASensitive() {
  MacroRegisterFunctionWithName("WebServer::InitializeMainFoldersULTRASensitive");
  MapList<std::string, std::string, MathRoutines::HashString>&
  folderSubstitutionsULTRASensitive = FileOperations::FolderVirtualLinksULTRASensitive(); //<- allocates data structure
  folderSubstitutionsULTRASensitive.SetKeyValue("certificates/", "certificates/");
  folderSubstitutionsULTRASensitive.SetKeyValue("/results/", "results/");
  folderSubstitutionsULTRASensitive.SetKeyValue("results/", "results/");
  folderSubstitutionsULTRASensitive.SetKeyValue("crashes/", "results/crashes/");
}

void FileOperations::InitializeFoldersSensitive() {
  MacroRegisterFunctionWithName("WebServer::InitializeMainFoldersSensitive");
  MapList<std::string, std::string, MathRoutines::HashString>&
  folderSubstitutionsSensitive = FileOperations::FolderVirtualLinksSensitive();
  folderSubstitutionsSensitive.Clear();

  folderSubstitutionsSensitive.SetKeyValue("LogFiles/", "LogFiles/");
  folderSubstitutionsSensitive.SetKeyValue("/LogFiles/", "LogFiles/");
  folderSubstitutionsSensitive.SetKeyValue("configuration/", "configuration/");
  folderSubstitutionsSensitive.SetKeyValue("/configuration/", "configuration/");
  folderSubstitutionsSensitive.SetKeyValue("freecalc/", "../freecalc/");
  folderSubstitutionsSensitive.SetKeyValue("/freecalc/", "../freecalc/");
  folderSubstitutionsSensitive.SetKeyValue("database_fallback/database.json", "database_fallback/database.json");
  folderSubstitutionsSensitive.SetKeyValue("/database_fallback/database.json", "database_fallback/database.json");
}

List<List<std::string> >& FileOperations::InitializeFolderVirtualLinksDefaults() {
  List<List<std::string> >& result = FileOperations::FolderVirtualLinksDefault();
  // The default values given here are overridden by file configuration.json.
  // Substitution order matters, earlier substitutions are processed first.
  // Once a substitution is found, no more substitutions are carried out.
  result = List<List<std::string> >({
    List<std::string>({Configuration::HTMLMathQuillFonts        , "html-common/mathquill/fonts/"               }),
    List<std::string>({Configuration::HTMLAceSrcMin             , "html-common/ace/src-min/"                   }),
    List<std::string>({Configuration::HTMLJQueryMinified        , "html-common/jquery/jquery.min.js"           }),
    List<std::string>({Configuration::HTMLJQueryUI              , "html-common/jquery_ui/"                     }),
    List<std::string>({Configuration::HTMLMathQuill             , "html-common/mathquill/"                     }),
    List<std::string>({Configuration::HTMLCommonFonts           , "html-common/fonts"                          }),
    List<std::string>({Configuration::HTMLCommonFont            , "html-common/fonts"                          }),
    List<std::string>({Configuration::HTMLCommon                , "html-common/"                               }),
    List<std::string>({Configuration::calculatorHTML            , "calculator-html/"                           }),
    List<std::string>({Configuration::publicHTML                , "public_html/"                               }),
    List<std::string>({Configuration::certificatesPublic        , "certificates-public/"                       }),
    List<std::string>({Configuration::sourceCode                , "src/"                                       }),
    List<std::string>({Configuration::testFolder                , "test/"                                      }),
    List<std::string>({Configuration::outputFolder              , "output/"                                    }),
    List<std::string>({Configuration::HTMLGeneral               , "public_html/"                               }),
    List<std::string>({Configuration::mathJaxLatestConfiguration, "calculator-html/mathjax-calculator-setup.js"}),
    List<std::string>({Configuration::mathJaxLatest             , "public_html/MathJax-2.7-latest/"            }),
    List<std::string>({Configuration::problemsFolder            , "../problems/"                               }),
    List<std::string>({Configuration::courseTemplates           , "../coursetemplates/"                        }),
    List<std::string>({Configuration::coursesAvailable          , "../coursesavailable/"                       }),
    List<std::string>({Configuration::topicLists                , "../topiclists/"                             }),
    List<std::string>({Configuration::laTeXMaterials            , "../LaTeX-materials/"                        }),
    List<std::string>({Configuration::slidesVideo               , "../slides-video/"                           }),
    List<std::string>({Configuration::freecalc                  , "../freecalc/"                               }),
    List<std::string>({"favicon.ico"                            , "calculator-html/favicon.ico"                }),
  });
  FileOperations::CheckFolderLinks();
  return result;
}

void FileOperations::InitializeFoldersNonSensitive() {
  MacroRegisterFunctionWithName("WebServer::InitializeMainFoldersNonSensitive");
  // Warning: order of substitutions is important.
  // Only the first rule that applies is applied, once.
  // No further rules are applied after that.
  // Location keys that start with "/" are coming from webserver references.
  // Location keys that do not start with "/" are for internal use.
  MapList<std::string, std::string, MathRoutines::HashString>&
  folderSubstitutionsNonSensitive = FileOperations::FolderVirtualLinksNonSensitive();

  // InitializeFolderVirtualLinksDefaults() is called in
  // GlobalVariables::ConfigurationProcess and its contents
  // are used together with file configuration.json,
  // to compute the folder locations below.

  std::string HTMLCommonFolder = global.configuration[Configuration::HTMLCommon].theString;
  List<List<std::string> >& links = FileOperations::FolderVirtualLinksDefault();
  for (int i = 0; i < links.size; i ++) {
    std::string& key = links[i][0];
    std::string value = global.configuration.GetValue(key).theString;
    if (value == "") {
      global.fatal << logger::red << "Unexpected empty folder mapping: key: "
      << key << ", value: " << value << global.fatal;
    }
    folderSubstitutionsNonSensitive.SetKeyValue(key, value);
    folderSubstitutionsNonSensitive.SetKeyValue("/" + key, value);
  }
}

bool FileOperations::CheckFolderLinks() {
  MacroRegisterFunctionWithName("FileOperations::CheckFolderLinks");
  List<List<std::string> > links = FileOperations::FolderVirtualLinksDefault();
  for (int i = 0; i < links.size; i ++) {
    if (links[i].size != 2) {
      global.fatal << "Folder links incorrectly initialized. " << global.fatal;
    }
    std::string& key = links[i][0];
    if (key.size() <= 1) {
      global.fatal << "Folder link key: " << key << " is too short. " << global.fatal;
    }
    if (key[0] == '/' || key[0] == '.' || key[0] == ' ') {
      global.fatal << "Folder link key " << key << " starts with a banned character. " << global.fatal;
    }
  }
  return true;
}

bool FileOperations::FileExistsVirtualCustomizedReadOnly(
  const std::string& theFileName, std::stringstream* commentsOnFailure
) {
  std::string computedFileName;
  if (!FileOperations::GetPhysicalFileNameFromVirtualCustomizedReadOnly(
    theFileName, computedFileName, commentsOnFailure
  )) {
    return false;
  }
  return FileOperations::FileExistsUnsecure(computedFileName);
}

bool FileOperations::FileExistsVirtual(
  const std::string& theFileName,
  bool accessSensitiveFolders,
  bool accessULTRASensitiveFolders,
  std::stringstream* commentsOnFailure
) {
  std::string computedFileName;
  if (!FileOperations::GetPhysicalFileNameFromVirtual(
    theFileName,
    computedFileName,
    accessSensitiveFolders,
    accessULTRASensitiveFolders,
    commentsOnFailure
  )) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to extract file name from virtual one. ";
    }
    return false;
  }
  return FileOperations::FileExistsUnsecure(computedFileName);
}

bool FileOperations::FileExistsUnsecure(const std::string& theFileName) {
  std::fstream theFile;
  theFile.open(theFileName.c_str(), std::fstream::in);
  if (theFile.is_open()) {
    return true;
  } else {
    return false;
  }
}

bool FileOperations::OpenFileVirtualReadOnly(
  std::ifstream& theFile,
  const std::string& theFileName,
  bool openAsBinary,
  bool accessSensitiveFolders
) {
  std::string computedFileName;
  if (!FileOperations::GetPhysicalFileNameFromVirtual(
    theFileName, computedFileName, accessSensitiveFolders, false, nullptr
  )) {
    return false;
  }
  return FileOperations::OpenFileUnsecureReadOnly(theFile, computedFileName, openAsBinary);
}

bool FileOperations::OpenFileVirtualCustomizedWriteOnly(
  std::fstream& theFile,
  const std::string& theFileName,
  bool OpenInAppendMode,
  bool truncate,
  bool openAsBinary,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("FileOperations::OpenFileVirtualCustomizedWriteOnly");
  std::string computedFileName;
  if (!FileOperations::GetPhysicalFileNameFromVirtualCustomizedWriteOnly(
    theFileName, computedFileName, commentsOnFailure
  )) {
    return false;
  }
  return FileOperations::OpenFileUnsecure(
    theFile, computedFileName, OpenInAppendMode, truncate, openAsBinary
  );
}

bool FileOperations::OpenFileVirtualCustomizedWriteOnlyCreateIfNeeded(
  std::fstream& theFile,
  const std::string& theFileName,
  bool OpenInAppendMode,
  bool truncate,
  bool openAsBinary,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("FileOperations::OpenFileVirtualCustomizedWriteOnly");
  std::string computedFileName;
  if (!FileOperations::GetPhysicalFileNameFromVirtualCustomizedWriteOnly(
    theFileName, computedFileName, commentsOnFailure
  )) {
    return false;
  }
  return FileOperations::OpenFileCreateIfNotPresentUnsecure(
    theFile, computedFileName, OpenInAppendMode, truncate, openAsBinary
  );
}

bool FileOperations::OpenFileVirtualCustomizedReadOnly(
  std::fstream& theFile,
  const std::string& theFileName,
  bool OpenInAppendMode,
  bool truncate,
  bool openAsBinary,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("FileOperations::OpenFileVirtualCustomizedReadOnly");
  std::string computedFileName;
  if (!FileOperations::GetPhysicalFileNameFromVirtualCustomizedReadOnly(
    theFileName, computedFileName, commentsOnFailure
  )) {
    return false;
  }
  return FileOperations::OpenFileUnsecure(
    theFile, computedFileName, OpenInAppendMode, truncate, openAsBinary
  );
}

bool FileOperations::OpenFileVirtual(
  std::fstream& theFile,
  const std::string& theFileName,
  bool OpenInAppendMode,
  bool truncate,
  bool openAsBinary,
  bool accessSensitiveFolders
) {
  std::string computedFileName;
  if (!FileOperations::GetPhysicalFileNameFromVirtual(
    theFileName, computedFileName, accessSensitiveFolders, false, nullptr
  )) {
    return false;
  }
  return FileOperations::OpenFileUnsecure(
    theFile, computedFileName, OpenInAppendMode, truncate, openAsBinary
  );
}

#include <unistd.h>

std::string FileOperations::GetWouldBeFolderAfterHypotheticalChdirNonThreadSafe(const std::string& wouldBePath) {
  // TODO: Investigate whether this code is safe.
  std::string currentFolder = FileOperations::GetCurrentFolder();
  global.ChDir(wouldBePath);
  std::string result = FileOperations::GetCurrentFolder();
  global.ChDir(currentFolder);
  return result;
}

std::string FileOperations::GetCurrentFolder() {
  char cwd[100000];
  if (getcwd(cwd, sizeof(cwd)) != nullptr) {
    return std::string(cwd);
  } else {
    global.fatal << "Error: getcwd returned NULL. This shouldn't happen. " << global.fatal;
  }
  return "";
}

bool FileOperations::OpenFileUnsecure(
  std::fstream& theFile, const std::string& theFileName, bool OpenInAppendMode, bool truncate, bool openAsBinary
) {
  if (OpenInAppendMode) {
    if (openAsBinary) {
      theFile.open(theFileName.c_str(), std::fstream::in | std::fstream::out | std::fstream::app | std::fstream::binary);
    } else {
      theFile.open(theFileName.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
    }
  } else {
    if (openAsBinary) {
      theFile.open(theFileName.c_str(), std::fstream::in | std::fstream::out | std::fstream::binary);
    } else {
      if (truncate) {
        theFile.open(theFileName.c_str(), std::fstream::in | std::fstream::out | std::fstream::trunc);
      } else {
        theFile.open(theFileName.c_str(), std::fstream::in | std::fstream::out);
      }
    }
  }
  return theFile.is_open();
}

bool FileOperations::OpenFileUnsecureReadOnly(std::ifstream& theFile, const std::string& theFileName, bool openAsBinary) {
  if (openAsBinary) {
    theFile.open(theFileName.c_str(), std::fstream::in | std::fstream::binary);
  } else {
    theFile.open(theFileName.c_str(), std::fstream::in);
  }
  return theFile.is_open();
}

std::string FileOperations::GetVirtualNameWithHash(const std::string& inputFileName) {
  MacroRegisterFunctionWithName("FileOperations::GetVirtualNameWithHash");
  std::string result = inputFileName;
  std::string fileNameEnd;
  for (int i = 0; i < FileOperations::FolderVirtualLinksToWhichWeAppendTimeAndBuildHash().size; i ++) {
    const std::string& currentStart = FileOperations::FolderVirtualLinksToWhichWeAppendTimeAndBuildHash()[i];
    if (StringRoutines::StringBeginsWith(result, currentStart, &fileNameEnd)) {
      result = currentStart + global.buildHeadHashWithServerTime + fileNameEnd;
      break;
    }
  }
  return result;
}

bool FileOperations::GetPhysicalFileNameFromVirtualCustomizedWriteOnly(
  const std::string& inputFileName, std::string& output, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("FileOperations::GetPhysicalFileNameFromVirtualCustomizedWriteOnly");
  std::string fileEnd = "";
  std::string inputStart = "";
  for (int i = 0; i < FileOperations::FolderStartsToWhichWeAppendInstructorUsernameSlash().size; i ++)
    if (StringRoutines::StringBeginsWith(
      inputFileName, FileOperations::FolderStartsToWhichWeAppendInstructorUsernameSlash()[i], &fileEnd
    )) {
      inputStart = FileOperations::FolderStartsToWhichWeAppendInstructorUsernameSlash()[i];
      break;
    }
  if (inputStart == "") {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "File name: "
      << inputFileName << " does not correspond to a customizable file. ";
    }
    return false;
  }
  std::string customized =
  HtmlRoutines::ConvertStringToURLString(global.userDefault.instructorComputed, false);
  if (customized == "") {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Customizing files not available for non-logged-in users. ";
    }
    return false;
  }
  std::string inputCopy = inputStart + customized + "/" + fileEnd;
  std::string outputCandidate;
  bool result = FileOperations::GetPhysicalFileNameFromVirtual(
    inputCopy, outputCandidate, false, false, commentsOnFailure
  );
  if (!FileOperations::FileExistsVirtual(outputCandidate, false, false)) {
    std::string fileContent;
    std::string inputDefault = inputStart + "default/" + fileEnd;
    if (FileOperations::LoadFileToStringVirtual(inputDefault, fileContent, false, commentsOnFailure)) {
      std::fstream theFile;
      if (FileOperations::OpenFileCreateIfNotPresentVirtualCreateFoldersIfNeeded(
        theFile, inputCopy, false, true, false, false
      )) {
        theFile << fileContent;
        result = true;
      }
    }
  }
  if (result) {
    output = outputCandidate;
  }
  return result;
}

bool FileOperations::GetPhysicalFileNameFromVirtualCustomizedReadOnly(
  const std::string& inputFileName, std::string& output, std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("FileOperations::GetPhysicalFileNameFromVirtualCustomizedReadOnly");
  std::string fileEnd = "";
  std::string inputStart = "";
  for (int i = 0; i < FileOperations::FolderStartsToWhichWeAppendInstructorUsernameSlash().size; i ++) {
    if (StringRoutines::StringBeginsWith(
      inputFileName,
      FileOperations::FolderStartsToWhichWeAppendInstructorUsernameSlash()[i],
      &fileEnd
    )) {
      inputStart = FileOperations::FolderStartsToWhichWeAppendInstructorUsernameSlash()[i];
      break;
    }
  }
  if (inputStart == "") {
    return FileOperations::GetPhysicalFileNameFromVirtual(inputFileName, output, false, false, commentsOnFailure);
  }
  std::string customized =
  HtmlRoutines::ConvertStringToURLString(global.userDefault.instructorComputed, false) + "/";
  if (customized == "") {
    customized = "default/";
  }
  std::string inputCopy = inputStart + customized + fileEnd;
  if (!FileExistsVirtual(inputCopy, false, false)) {
    customized = "default/";
  }
  inputCopy = inputStart + customized + fileEnd;
  return FileOperations::GetPhysicalFileNameFromVirtual(inputCopy, output, false, false, commentsOnFailure);
}

bool FileOperations::GetPhysicalFileNameFromVirtual(
  const std::string& inputFileNamE,
  std::string& output,
  bool accessSensitiveFolders,
  bool accessULTRASensitiveFolders,
  std::stringstream* commentsOnFailure
) {
  MacroRegisterFunctionWithName("FileOperations::GetPhysicalFileNameFromVirtual");
  // Using loggers forbidden here: function is used by the loggers themselves.
  if (!FileOperations::IsOKfileNameVirtual(inputFileNamE, accessSensitiveFolders)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "File name: " << inputFileNamE << " not allowed. ";
    }
    return false;
  }
  std::string inputCopy = inputFileNamE;
  std::string folderEnd, folderEnd2;
  for (int i = 0; i < FileOperations::FolderVirtualLinksToWhichWeAppendTimeAndBuildHash().size; i ++) {
    if (StringRoutines::StringBeginsWith(
      inputCopy,
      FileOperations::FolderVirtualLinksToWhichWeAppendTimeAndBuildHash()[i], &folderEnd
    )) {
      if (StringRoutines::StringBeginsWith(
        folderEnd, global.buildHeadHashWithServerTime, &folderEnd2
      )) {
        inputCopy = FileOperations::FolderVirtualLinksToWhichWeAppendTimeAndBuildHash()[i] + folderEnd2;
      }
    }
  }
  for (int i = 0; i < FileOperations::FolderVirtualLinksNonSensitive().size(); i ++) {
    if (StringRoutines::StringBeginsWith(
      inputCopy, FileOperations::FolderVirtualLinksNonSensitive().theKeys[i], &folderEnd
    )) {
      output = global.PhysicalPathProjectBase +
      FileOperations::FolderVirtualLinksNonSensitive().theValues[i] + folderEnd;
      return true;
    }
  }
  if (accessSensitiveFolders) {
    for (int i = 0; i < FileOperations::FolderVirtualLinksSensitive().size(); i ++) {
      if (StringRoutines::StringBeginsWith(
        inputCopy, FileOperations::FolderVirtualLinksSensitive().theKeys[i], &folderEnd
      )) {
        output = global.PhysicalPathProjectBase +
        FileOperations::FolderVirtualLinksSensitive().theValues[i] + folderEnd;
        return true;
      }
    }
  }
  if (accessULTRASensitiveFolders) {
    for (int i = 0; i < FileOperations::FolderVirtualLinksULTRASensitive().size(); i ++) {
      if (StringRoutines::StringBeginsWith(
        inputCopy, FileOperations::FolderVirtualLinksULTRASensitive().theKeys[i], &folderEnd
      )) {
        output = global.PhysicalPathProjectBase +
        FileOperations::FolderVirtualLinksULTRASensitive().theValues[i] + folderEnd;
        return true;
      }
    }
  }
  output = global.PhysicalPathServerBase + "public_html";
  if (inputCopy.size() > 0) {
    if (inputCopy[0] != '/') {
      output += "/";
    }
  }
  output += inputCopy;
  return true;
}

bool FileOperations::OpenFileCreateIfNotPresentVirtualCreateFoldersIfNeeded(
  std::fstream& theFile,
  const std::string& theFileName,
  bool OpenInAppendMode,
  bool truncate,
  bool openAsBinary,
  bool accessSensitiveFolders
) {
  return FileOperations::OpenFileCreateIfNotPresentVirtualCreateFoldersIfNeeded_UltraSensitiveOptions(
    theFile,
    theFileName,
    OpenInAppendMode,
    truncate,
    openAsBinary,
    accessSensitiveFolders
  );
}

bool FileOperations::OpenFileCreateIfNotPresentVirtualCreateFoldersIfNeeded_UltraSensitiveOptions(
  std::fstream& theFile,
  const std::string& theFileName,
  bool OpenInAppendMode,
  bool truncate,
  bool openAsBinary,
  bool accessSensitiveFolders,
  bool accessUltraSensitiveFolders
) {
  std::string computedFileName;
  // USING loggers FORBIDDEN here! Loggers call this function themselves in their constructors.
  if (!FileOperations::GetPhysicalFileNameFromVirtual(
    theFileName,
    computedFileName,
    accessSensitiveFolders,
    accessUltraSensitiveFolders,
    nullptr
  )) {
    return false;
  }
  std::string folderName = FileOperations::GetPathFromFileNameWithPath(computedFileName);
  std::stringstream mkDirCommand;
  mkDirCommand << "mkdir -p " << folderName;
  global.CallSystemWithOutput(mkDirCommand.str());
  return FileOperations::OpenFileCreateIfNotPresentUnsecure(
    theFile, computedFileName, OpenInAppendMode, truncate, openAsBinary
  );
}

bool FileOperations::OpenFileCreateIfNotPresentVirtual(std::fstream& theFile,
  const std::string& theFileName,
  bool OpenInAppendMode,
  bool truncate,
  bool openAsBinary,
  bool accessSensitiveFolders,
  bool accessUltraSensitiveFolders
) {
  std::string computedFileName;
  // USING loggers FORBIDDEN here! Loggers call this function themselves in their constructors.
  if (!FileOperations::GetPhysicalFileNameFromVirtual(
    theFileName, computedFileName, accessSensitiveFolders, accessUltraSensitiveFolders, nullptr
  )) {
    return false;
  }
  return FileOperations::OpenFileCreateIfNotPresentUnsecure(
    theFile, computedFileName, OpenInAppendMode, truncate, openAsBinary
  );
}

bool FileOperations::OpenFileCreateIfNotPresentUnsecure(
  std::fstream& theFile, const std::string& theFileName, bool OpenInAppendMode, bool truncate, bool openAsBinary
) {
  //USING loggers FORBIDDEN here! Loggers call this function themselves in their constructors.
  if (OpenInAppendMode) {
    if (openAsBinary) {
      theFile.open(theFileName.c_str(), std::fstream::in | std::fstream::out | std::fstream::app | std::fstream::binary);
    } else {
      theFile.open(theFileName.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
    }
  } else {
    if (openAsBinary) {
      theFile.open(theFileName.c_str(), std::fstream::in | std::fstream::out | std::fstream::binary);
    } else {
      if (truncate) {
        theFile.open(theFileName.c_str(), std::fstream::in | std::fstream::out | std::fstream::trunc);
      } else {
        theFile.open(theFileName.c_str(), std::fstream::in | std::fstream::out);
      }
    }
  }
  if (theFile.is_open()) {
    theFile.clear();
    theFile.seekp(0, std::ios_base::end);
    long tempI = theFile.tellp();
    if (tempI > 0) {
      if (!OpenInAppendMode) {
        theFile.seekp(0);
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

StateMaintainerCurrentFolder::StateMaintainerCurrentFolder() {
  this->currentFolderPhysicalAbsolute = FileOperations::GetCurrentFolder();
}

StateMaintainerCurrentFolder::~StateMaintainerCurrentFolder() {
  global.ChDir(this->currentFolderPhysicalAbsolute);
}

void DrawingVariables::GetCoordsForDrawing(DrawingVariables& TDV, Vector<Rational>& r, double& x, double& y) {
  x = TDV.theBuffer.centerX;
  y = TDV.theBuffer.centerY;
  for (int k = 0; k < r.size; k ++) {
    x += (r[k].GetDoubleValue()) * (TDV.theBuffer.ProjectionsEiVectors[k][0]);
    y -= (r[k].GetDoubleValue()) * (TDV.theBuffer.ProjectionsEiVectors[k][1]);
  }
}

void DrawingVariables::drawCoordSystemBuffer(DrawingVariables& TDV, int theDimension) {
  MacroRegisterFunctionWithName("DrawingVariables::drawCoordSystemBuffer");
  Vector<Rational> tempRoot;
  Vector<Rational> zeroRoot;
  zeroRoot.MakeZero(theDimension);
  std::string colorText ="#64c064";
  for (int i = 0; i < theDimension; i ++) {
    tempRoot.MakeEi(theDimension, i);
    std::string tempS;
    tempS = tempRoot.ToString();
    TDV.drawLineBetweenTwoVectorsBufferRational(zeroRoot, tempRoot, "gray", 1);
    TDV.drawTextAtVectorBufferRational(tempRoot, tempS, "#94c894");
    TDV.drawCircleAtVectorBufferRational(tempRoot, colorText, 2);
  }
  TDV.theBuffer.BasisToDrawCirclesAt.MakeEiBasis(theDimension);
}

void DrawingVariables::drawLineBufferOld(
  double X1, double Y1, double X2, double Y2, uint32_t thePenStyle, int ColorIndex, std::fstream* LatexOutFile
) {
  this->theBuffer.drawLineBuffer(X1, Y1, X2, Y2, thePenStyle, ColorIndex, 1);
  if (LatexOutFile != nullptr) {
    LaTeXProcedures::drawline(X1, Y1, X2, Y2, thePenStyle, ColorIndex, *LatexOutFile, *this);
  }
}

void DrawingVariables::drawTextAtVectorBufferRational(
  const Vector<Rational>& point, const std::string& inputText, const std::string& color
) {
  this->theBuffer.drawTextAtVectorBufferRational(point, inputText, color, 10);
}

void DrawingVariables::drawTextAtVectorBufferDouble(
  const Vector<double>& point, const std::string& inputText, int textColor, int theTextStyle
) {
  this->theBuffer.drawTextAtVectorBufferDouble(point, inputText, textColor, this->fontSizeNormal, theTextStyle);
}

void DrawingVariables::drawCircleAtVectorBufferRational(
  const Vector<Rational>& point, const std::string& color, double radius, const std::string& frameId, int frameIndex
) {
  this->theBuffer.drawCircleAtVectorBufferRational(point, color, radius, frameId, frameIndex);
}

void DrawingVariables::drawCircleAtVectorBufferDouble(
  const Vector<double>& point, const std::string& color, double radius
) {
  this->theBuffer.drawCircleAtVectorBufferDouble(point, color, radius);
}

void DrawingVariables::drawTextDirectly(
  double X1, double Y1, const std::string& inputText, int color, std::fstream* LatexOutFile
) {
  if (this->theDrawTextFunction != nullptr) {
    this->theDrawTextFunction(
      X1 - 7, Y1 - 7, inputText.c_str(), static_cast<int>(inputText.length()), color, this->fontSizeNormal
    );
  }
  if (LatexOutFile != nullptr) {
    LaTeXProcedures::drawTextDirectly(X1, Y1, inputText, color, *LatexOutFile);
  }
}

void DrawingVariables::ProjectOnToHyperPlaneGraphics(Vector<Rational>& input, Vector<Rational>& output) {
  output = input;
  Vector<Rational> normal; Vector<Rational> basepoint;
  Rational tempRat2, tempRat, tempRat3;
  normal.MakeZero(input.size);
  for (int i = 0; i < input.size; i ++) {
    if (input[i].IsPositiveOrZero()) {
      normal[i] += 1;
    } else {
      normal[i] += - 1;
    }
  }
  basepoint.MakeZero(input.size);
  basepoint[0].AssignInteger(1);
  if (input[0].IsNegative()) {
    basepoint.Minus();
  }
  //////////////////////////////////////////////////
  output.ScalarEuclidean(normal, tempRat2);
  basepoint.ScalarEuclidean(normal, tempRat);
  if (!tempRat2.IsEqualToZero()) {
    tempRat3 = tempRat;
    tempRat3.DivideBy(tempRat2);
    output *= tempRat3;
  } else {
    output.MakeZero(input.size);
  }
}

bool WeylGroupData::IsStronglyPerpendicularTo(const Vector<Rational>& input, const Vector<Rational>& other) {
  return !this->RootSystem.Contains(input + other) && !this->RootSystem.Contains(input - other);
}

bool WeylGroupData::HasStronglyPerpendicularDecompositionWRT(
  Vector<Rational>& input,
  int UpperBoundNumBetas,
  Vectors<Rational>& theSet,
  Vectors<Rational>& output,
  List<Rational>& outputCoeffs,
  bool IntegralCoefficientsOnly
) {
  if (UpperBoundNumBetas > 0 && output.size > UpperBoundNumBetas) {
    return false;
  }
  if (input.IsEqualToZero()) {
    return true;
  }
  if (theSet.size == 0) {
    return false;
  }
  if (output.size == 0) {
    if (theSet.Contains(input)) {
      output.SetSize(1);
      *output.LastObject() = input;
      outputCoeffs.SetSize(1);
      outputCoeffs.LastObject()->MakeOne();
      return true;
    }
    output.Reserve(theSet.size);
    outputCoeffs.Reserve(theSet.size);
  }
  Vectors<Rational> theNewSet;
  theNewSet.Reserve(theSet.size);
  Vector<Rational> tempRoot;
  Rational tempRat;
  for (int indexFirstNonZeroRoot = 0; indexFirstNonZeroRoot < theSet.size; indexFirstNonZeroRoot ++) {
    Vector<Rational>& currentRoot = theSet[indexFirstNonZeroRoot];
    tempRat = this->RootScalarCartanRoot(input, currentRoot)/this->RootScalarCartanRoot(currentRoot, currentRoot);
    if (tempRat.IsPositive()) {
      if (!IntegralCoefficientsOnly || tempRat.DenShort == 1) {
         theNewSet.size = 0;
         for (int i = indexFirstNonZeroRoot; i < theSet.size; i ++) {
           if (this->IsStronglyPerpendicularTo(currentRoot, theSet[i])) {
             theNewSet.AddOnTop(theSet[i]);
           }
         }
         outputCoeffs.AddOnTop(tempRat);
         output.AddOnTop(currentRoot);
         tempRoot = input - currentRoot * tempRat;
         if (this->HasStronglyPerpendicularDecompositionWRT(
          tempRoot, UpperBoundNumBetas, theNewSet, output, outputCoeffs, IntegralCoefficientsOnly
         )) {
           return true;
         }
         output.size --;
         outputCoeffs.size --;
       }
    }
  }
  return false;
}

char MathRoutines::ConvertHumanReadableHexToCharValue(char input) {
  if ('0' <= input  && input <= '9') {
    return input - '0';
  }
  if ('A' <= input && input <= 'F') {
    return 10 + input - 'A';
  }
  if ('a' <= input && input <= 'f') {
    return 10 + input - 'a';
  }
  return - 1;
}

unsigned int MathRoutines::HashListUnsignedChars(const List<unsigned char> &input) {
  unsigned int result = 0;
  int numCycles = MathRoutines::Minimum(input.size, SomeRandomPrimesSize);
  for (int i = 0; i < numCycles; i ++) {
    result += SomeRandomPrimes[i] * static_cast<unsigned>(input[i]);
  }
  return result;
}

unsigned int MathRoutines::HashListStrings(const List<std::string>& input) {
  unsigned int result = 0;
  int numCycles = MathRoutines::Minimum(input.size, SomeRandomPrimesSize);
  for (int i = 0; i < numCycles; i ++) {
    result += SomeRandomPrimes[i] * MathRoutines::HashString(input[i]);
  }
  return result;
}

unsigned int MathRoutines::HashListDoubles(const List<double>& input) {
  unsigned int result = 0;
  int numCycles = MathRoutines::Minimum(input.size, SomeRandomPrimesSize);
  for (int i = 0; i < numCycles; i ++) {
    result += SomeRandomPrimes[i] * MathRoutines::HashDouble(input[i]);
  }
  return result;
}

unsigned int MathRoutines::HashListInts(const List<int>& input) {
  unsigned int result = 0;
  int numCycles = MathRoutines::Minimum(input.size, SomeRandomPrimesSize);
  for (int i = 0; i < numCycles; i ++) {
    result += SomeRandomPrimes[i] * MathRoutines::IntUnsignIdentity(input[i]);
  }
  return result;
}

unsigned int MathRoutines::HashString(const std::string& x) {
  size_t numCycles = x.size();
  unsigned int result = 0;
  for (unsigned i = 0, counter = 0; i < numCycles; i ++, counter ++) {
    if (counter >= SomeRandomPrimesSize) {
      counter = 0;
    }
    result += static_cast<unsigned>(x[i]) * SomeRandomPrimes[counter];
  }
  return result;
}

unsigned int MathRoutines::HashVectorDoubles(const Vector<double>& input) {
  return MathRoutines::HashListDoubles(input);
}

std::string StringRoutines::Differ::DifferenceHTMLStatic(
  const std::string& inputLeft,
  const std::string& inputRight,
  const std::string& labelLeft,
  const std::string& labelRight
) {
  StringRoutines::Differ theDiffer;
  theDiffer.left = inputLeft;
  theDiffer.right = inputRight;
  return theDiffer.DifferenceHTML(
    labelLeft, labelRight
  );
}

std::string StringRoutines::Differ::DifferenceHTML(
  const std::string& labelLeft,
  const std::string& labelRight
) {
  MacroRegisterFunctionWithName("StringRoutines::Differ::DifferenceHTML");
  std::stringstream leftOut, rightOut, commentsOnFailure;
  if (!this->ComputeDifference(&commentsOnFailure)) {
    commentsOnFailure << "<b style='color:red'>Failed to compute string difference.</b><br>";
    this->DifferenceHTMLPartTwo(commentsOnFailure.str(), labelLeft, labelRight, this->left, this->right);
  }
  for (int i = 0; i < this->leftResult.size; i ++) {
    std::string leftS = this->leftResult[i];
    std::string rightS = this->rightResult[i];
    if (leftS == rightS && leftS.size() != 0) {
      if (leftS.size() > 0) {
        leftOut << "<span class = 'spanStringEqual'>" << leftS << "</span>";
      }
      if (rightS.size() > 0) {
        rightOut << "<span class = 'spanStringEqual'>" << rightS << "</span>";
      }
    } else {
      if (leftS.size() > 0) {
        leftOut << "<span class = 'spanStringDifferent'>" << leftS << "</span>";
      }
      if (rightS.size() > 0) {
        rightOut << "<span class = 'spanStringDifferent'>" << rightS << "</span>";
      }
    }
  }
  return this->DifferenceHTMLPartTwo("", labelLeft, labelRight, leftOut.str(), rightOut.str());
}

std::string StringRoutines::Differ::DifferenceHTMLPartTwo(
  const std::string& preamble,
  const std::string& labelLeft,
  const std::string& labelRight,
  const std::string& outputLeft,
  const std::string& outputRight
) {
  std::stringstream out;
  out << preamble;
  out << "<span class = 'abstractSyntaxOneAnnotation'><b>" << labelLeft << "</b><br>";
  out << outputLeft;
  out << "<hr><b>" << labelRight << "</b><br>";
  out << outputRight;
  out << "</span>";
  return out.str();
}

StringRoutines::Differ::Differ() {
  this->MaxMatrixSize = 200000000;
  this->currentCommonStringLength = 0;
}

std::string StringRoutines::Differ::ToString() {
  FormatExpressions theFormat;
  theFormat.flagUseHTML = true;
  std::stringstream out;
  out << "Matrix:<br>" << this->matrixLongestCommonSubsequence.GetElement() << "<br>"
  << "Common string sizes:<br>"
  << this->commonStringSizesReverseOrder
  << "<br>"
  << "String starts, reverse order:<br>"
  << "left: <br>"
  << this->commonStringsLeftStartsReverseOrder
  << "<br>right: <br>"
  << this->commonStringsRightStartsReverseOrder;
  return out.str();
}

void StringRoutines::Differ::ComputeBestStartingIndices(int& outputIndexLeft, int& outputIndexRight) {
  MacroRegisterFunctionWithName("StringRoutines::Differ::ComputeBestStartingIndex");
  Matrix<int> theMatrix = this->matrixLongestCommonSubsequence.GetElement();
  outputIndexLeft = theMatrix.NumRows - 1;
  outputIndexRight = theMatrix.NumCols - 1;
  int bestSoFar = 0;
  for (int i = 1; i < theMatrix.NumRows; i ++) {
    for (int j = 1; j < theMatrix.NumCols; j ++) {
      if (this->left[static_cast<unsigned>(i - 1)] != this->right[static_cast<unsigned>(j - 1)]) {
        continue;
      }
      int current = theMatrix(i, j);
      bool isBetter = false;
      if (current > bestSoFar) {
        isBetter = true;
      }
      if (current == bestSoFar && i + j < outputIndexLeft + outputIndexRight) {
        isBetter = true;
      }
      if (isBetter) {
        bestSoFar = current;
        outputIndexLeft = i;
        outputIndexRight = j;
      }
    }
  }
}

void StringRoutines::Differ::ComputeLongestSubsequenceMatrix() {
  MacroRegisterFunctionWithName("StringRoutines::Differ::ComputeLongestSubsequenceMatrix");
  unsigned numberOfRows = static_cast<unsigned>(left.size() ) + 1;
  unsigned numberOfColumns = static_cast<unsigned>(right.size()) + 1;
  Matrix<int>& theMatrix = this->matrixLongestCommonSubsequence.GetElement();
  theMatrix.init(
    static_cast<int>(numberOfRows), static_cast<int>(numberOfColumns)
  );
  for (int j = 0; j < theMatrix.NumCols; j ++) {
    theMatrix(0, j) = 0;
  }
  for (int i = 0; i < theMatrix.NumRows; i ++) {
    theMatrix(i, 0) = 0;
  }
  for (unsigned i = 1; i < numberOfRows; i ++) {
    for (unsigned j = 1; j < numberOfColumns; j ++) {
      if (this->left[i - 1] == this->right[j - 1]) {
        theMatrix(i, j) = theMatrix(i - 1, j - 1) + 1;
      } else {
        theMatrix(i, j) = MathRoutines::Maximum(theMatrix(i - 1, j), theMatrix(i, j - 1));
      }
    }
  }
}

void StringRoutines::Differ::PushCommonString(int indexLeft, int indexRight) {
  if (this->currentCommonStringLength == 0) {
    return;
  }
  this->commonStringSizesReverseOrder.AddOnTop(this->currentCommonStringLength);
  this->commonStringsLeftStartsReverseOrder.AddOnTop(indexLeft - 1);
  this->commonStringsRightStartsReverseOrder.AddOnTop(indexRight - 1);
  this->currentCommonStringLength = 0;
}

void StringRoutines::Differ::ExtractCommonStrings(
  int indexLeft, int indexRight, int previousLeft, int previousRight
) {
  MacroRegisterFunctionWithName("stringDiffer::ExtractCommonStrings");
  if (indexLeft == 0 || indexRight == 0) {
    this->PushCommonString(previousLeft, previousRight);
    return;
  }
  unsigned leftUnsigned  = static_cast<unsigned>(indexLeft - 1);
  unsigned rightUnsigned = static_cast<unsigned>(indexRight - 1);
  if (this->left[leftUnsigned] == this->right[rightUnsigned]) {
    this->currentCommonStringLength ++ ;
    this->ExtractCommonStrings(indexLeft - 1, indexRight - 1, indexLeft, indexRight);
    return;
  }
  this->PushCommonString(previousLeft, previousRight);
  Matrix<int>& theMatrix = this->matrixLongestCommonSubsequence.GetElement();
  if (
    theMatrix(indexLeft - 1, indexRight) > theMatrix(indexLeft, indexRight - 1)
  ) {
    this->ExtractCommonStrings(indexLeft - 1, indexRight, indexLeft, indexRight);

  } else {
    this->ExtractCommonStrings(indexLeft, indexRight - 1, indexLeft, indexRight);
  }
}

void StringRoutines::Differ::ExtractDifferences() {
  this->ExtractResult(this->commonStringsLeftStartsReverseOrder, this->left, this->leftResult);
  this->ExtractResult(this->commonStringsRightStartsReverseOrder, this->right, this->rightResult);
  while (this->leftResult.size < this->rightResult.size) {
    this->leftResult.AddOnTop("");
  }
  while (this->rightResult.size < this->leftResult.size) {
    this->rightResult.AddOnTop("");
  }
}

void StringRoutines::Differ::ExtractResult(
  const List<int>& starts, const std::string& input, List<std::string>& output
) {
  MacroRegisterFunctionWithName("StringRoutines::Differ::ExtractResult");
  output.SetSize(0);
  unsigned int previousEnd = 0;
  for (int i = this->commonStringSizesReverseOrder.size - 1; i >= 0; i --) {
    int stringSize = this->commonStringSizesReverseOrder[i];
    int stringStart = starts[i];
    std::string nonCommon = input.substr(
      static_cast<unsigned>(previousEnd),
      static_cast<unsigned>(stringStart) - previousEnd
    );
    output.AddOnTop(nonCommon);
    std::string common = input.substr(
      static_cast<unsigned>(stringStart),
      static_cast<unsigned>(stringSize)
    );
    previousEnd = static_cast<unsigned>(stringStart + stringSize);
    output.AddOnTop(common);
  }
  if (previousEnd < input.size()) {
    output.AddOnTop(input.substr(previousEnd));
  }
}

// We compute string difference using the
// longest common subsequence problem.
// https://en.wikipedia.org/wiki/Longest_common_subsequence_problem
bool StringRoutines::Differ::ComputeDifference(std::stringstream* commentsOnFailure) {
  MacroRegisterFunctionWithName("StringRoutines::Differ::ComputeDifference");
  LargeInteger leftSize = left.size();
  LargeInteger rightSize = right.size();
  if (leftSize * rightSize > this->MaxMatrixSize) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "The product of the sizes of the two strings equals "
      << leftSize * rightSize
      << " which exceeds the maximum allowed "
      << this->MaxMatrixSize;
    }
    return false;
  }
  this->ComputeLongestSubsequenceMatrix();
  this->currentCommonStringLength = 0;
  int startLeft = 0, startRight = 0;
  this->ComputeBestStartingIndices(startLeft, startRight);
  this->ExtractCommonStrings(
    startLeft,
    startRight,
    0,
    0
  );
  this->ExtractDifferences();
  return true;
}

bool StringRoutines::isASCIICharacterVisible(char input) {
  return input >= 32 && input <= 126;
}

std::string StringRoutines::StringShortenInsertDots(const std::string& inputString, int maxNumChars) {
  if (inputString.size() <= static_cast<unsigned>(maxNumChars)) {
    return inputString;
  }
  std::stringstream out;
  int numCharsBeginEnd = maxNumChars / 2 - 2;
  int numCharsOmitted = static_cast<signed>(inputString.size()) - numCharsBeginEnd * 2;
  out << inputString.substr(0, static_cast<unsigned>(numCharsBeginEnd)) << "... ("
  << numCharsOmitted << " characters omitted)..."
  << inputString.substr(static_cast<unsigned>( static_cast<signed>(inputString.size()) - numCharsBeginEnd));
  return out.str();
}

void StringRoutines::StringSplitExcludeDelimiter(
  const std::string& inputString, char delimiter, List<std::string>& output
) {
  List<char> tempList;
  tempList.AddOnTop(delimiter);
  StringRoutines::StringSplitExcludeDelimiters(inputString, tempList, output);
}

std::string StringRoutines::StringTrimWhiteSpace(const std::string& inputString) {
  std::string result;
  StringRoutines::StringTrimWhiteSpace(inputString, result);
  return result;
}

void StringRoutines::StringTrimToLengthWithHash(std::string& inputOutput, int desiredLength50AtLeast) {
  if (static_cast<signed>(inputOutput.size()) <= desiredLength50AtLeast) {
    return;
  }
  if (desiredLength50AtLeast < 40) {
    return;
  }
  std::stringstream inputAbbreviatedStream;
  inputAbbreviatedStream << inputOutput.substr(0, static_cast<unsigned>(desiredLength50AtLeast - 30))
  << "_abbrev_hash_" << MathRoutines::HashString(inputOutput);
  inputOutput = inputAbbreviatedStream.str();
}

std::string StringRoutines::StringTrimToLengthForDisplay(const std::string& input, int desiredLength20AtLeast) {
  if (desiredLength20AtLeast < 20) {
    desiredLength20AtLeast = 20;
  }
  if (static_cast<signed>(input.size()) <= desiredLength20AtLeast) {
    return input;
  }
  std::stringstream abbreviationStream;
  abbreviationStream << "...(" << input.size() << " characters) ...";
  int numCharsAtEnds = (static_cast<signed>(input.size()) - desiredLength20AtLeast) / 2;
  std::stringstream out;
  for (int i = 0; i < numCharsAtEnds; i ++) {
    out << input[static_cast<unsigned>(i)];
  }
  out << abbreviationStream.str();
  for (int i = 0; i < numCharsAtEnds; i ++) {
    out << input[
      static_cast<unsigned>(static_cast<signed>(input.size()) - numCharsAtEnds + i)
    ];
  }
  return out.str();
}

void StringRoutines::StringTrimWhiteSpace(const std::string& inputString, std::string& output) {
  //this function needs to be rewritten to do one substr call (no time now).
  output = "";
  output.reserve(inputString.size());
  unsigned i = 0;
  for (i = 0; i < inputString.size(); i ++) {
    if (
      inputString[i] != ' ' &&
      inputString[i] != '\r' &&
      inputString[i] != '\t' &&
      inputString[i] != '\n'
    ) {
      break;
    }
  }
  output = inputString.substr(i, std::string::npos);
  if (output.size() == 0) {
    return;
  }
  signed j = 0;
  for (j = static_cast<signed>(output.size()) - 1; j >= 0; j --) {
    unsigned k = static_cast<unsigned>(j);
    if (
      output[k] != ' ' &&
      output[k] != '\r' &&
      output[k] != '\t' &&
      output[k] != '\n' &&
      output[k] != '\0'
    ) {
      break;
    }
  }
  j ++;
  output = output.substr(0, static_cast<unsigned>(j));
}

void StringRoutines::StringSplitDefaultDelimiters(const std::string& inputString, List<std::string>& output) {
  MacroRegisterFunctionWithName("StringRoutines::StringSplitDefaultDelimiters");
  List<char> delimiters;
  delimiters.AddOnTop(' ');
  delimiters.AddOnTop('\r');
  delimiters.AddOnTop('\n');
  delimiters.AddOnTop('\t');
  delimiters.AddOnTop(',');
  delimiters.AddOnTop(';');
  delimiters.AddOnTop(static_cast<char>(160)); //<-&nbsp
  StringRoutines::StringSplitExcludeDelimiters(inputString, delimiters, output);
}

void StringRoutines::StringSplitExcludeDelimiters(
  const std::string& inputString, const List<char>& delimiters, List<std::string>& output
) {
  MacroRegisterFunctionWithName("MathRoutines::StringSplit");
  output.SetSize(0);
  std::string reader;
  for (unsigned i = 0; i < inputString.size(); i ++) {
    if (delimiters.Contains(inputString[i])) {
      if (reader != "") {
        output.AddOnTop(reader);
        reader = "";
      }
    } else {
      reader.push_back(inputString[i]);
    }
  }
  if (reader != "") {
    output.AddOnTop(reader);
  }
}

std::string StringRoutines::ReplaceAll(
  const std::string& input,
  const std::string& subStringToReplace,
  const std::string& replaceWith
) {
  std::stringstream out;
  if (subStringToReplace.size() == 0) {
    global.fatal << "Not allowed to replace the empty string. " << global.fatal;
  }
  for (unsigned i = 0; i < input.size(); i ++) {
    bool found = true;
    for (unsigned j = 0; j < subStringToReplace.size(); j ++) {
      unsigned indexInString = i + j;
      if (indexInString >= input.size()) {
        found = false;
        break;
      }
      if (input[indexInString] != subStringToReplace[j]) {
        found = false;
        break;
      }
    }
    if (!found) {
      out << input[i];
    } else {
      i += subStringToReplace.size();
      i --;
      out << replaceWith;
    }
  }
  return out.str();
}

void StringRoutines::SplitStringInTwo(
  const std::string& inputString, int firstStringSize, std::string& outputFirst, std::string& outputSecond
) {
  if (&outputFirst == &inputString || &outputSecond == &inputString) {
    std::string inputCopy = inputString;
    StringRoutines::SplitStringInTwo(inputCopy, firstStringSize, outputFirst, outputSecond);
    return;
  }
  if (firstStringSize < 0) {
    firstStringSize = 0;
  }
  int inputSize = static_cast<signed>(inputString.size());
  if (firstStringSize > inputSize) {
    firstStringSize = inputSize;
  }
  outputFirst = "";
  outputFirst = inputString.substr(0, static_cast<unsigned>(firstStringSize));
  outputSecond = "";
  int secondStringSize =  inputSize - firstStringSize;
  if (secondStringSize > 0) {
    outputSecond = inputString.substr(static_cast<unsigned>(firstStringSize), static_cast<unsigned>(secondStringSize));
  }
}

void MathRoutines::NChooseK(int n, int k, LargeInteger& result) {
  result = 1;
  for (int i = 0; i < k; i ++) {
    result *= n - i;
    result /= i + 1;
  }
}

double MathRoutines::ReducePrecision(double x) {
  if (x < 0.00001 && x > - 0.00001) {
    return 0;
  }
  return x;
}

int MathRoutines::parity(int n) {
  if (n % 2 == 0) {
    return 1;
  } else {
    return - 1;
  }
}

int MathRoutines::Factorial(int n) {
  if (n < 0) {
    global.fatal << "We do not have factorials of negative integers here. You wanted factorial of: " << n
    << ". If you are interested in the Gamma function, "
    << "you may want to use a different data structure rather than int. "
    << global.fatal;
  }
  int fac = 1;
  for (int i = 1; i <= n; i ++) {
    fac *= i;
  }
  return fac;
}

int MathRoutines::NChooseK(int n, int k) {
  int result = 1;
  for (int i = 0; i < k; i ++) {
    result *= n - i;
    if (result < 0) {
      return - 1;
    }
    result /= i + 1;
    if (result < 0) {
      return - 1;
    }
  }
  return result;
}

bool StringRoutines::StringEndsWith(
  const std::string& theString, const std::string& desiredEnd, std::string* outputStringBeginning
) {
  MacroRegisterFunctionWithName("StringRoutines::StringEndsWith");
  if (desiredEnd.size() == 0) {
    if (outputStringBeginning == nullptr) {
      *outputStringBeginning = theString;
    }
    return true;
  }
  int indexInTheString = static_cast<signed>(theString.size()) - 1;
  for (int i = static_cast<signed>(desiredEnd.size()) - 1; i >= 0; i --) {
    if (indexInTheString < 0) {
      return false;
    }
    if (desiredEnd[static_cast<unsigned>(i)] != theString[static_cast<unsigned>(indexInTheString)]) {
      return false;
    }
    indexInTheString --;
  }
  if (outputStringBeginning != nullptr) {
    *outputStringBeginning = theString.substr(0, theString.size() - desiredEnd.size());
  }
  return true;
}

bool StringRoutines::StringBeginsWith(
  const std::string& theString, const std::string& desiredBeginning, std::string* outputStringEnd
) {
  std::string actualBeginning, stringEnd;
  StringRoutines::SplitStringInTwo(theString, static_cast<int>(desiredBeginning.size()), actualBeginning, stringEnd);
  bool result = (actualBeginning == desiredBeginning);
  //outputstring is only modified if result is true
  if (result && outputStringEnd != nullptr) {
    *outputStringEnd = stringEnd;
  }
  return result;
}

bool MathRoutines::isALatinLetter(char input) {
  if (input >= 'a' && input <= 'z') {
    return true;
  }
  if (input >= 'A' && input <= 'Z') {
    return true;
  }
  return false;
}

bool MathRoutines::hasDecimalDigitsOnly(const std::string& input) {
  for (unsigned i = 0; i < input.size(); i ++) {
    if (!MathRoutines::isADigit(input[i])) {
      return false;
    }
  }
  return true;
}

bool MathRoutines::isADigit(char theChar, int* whichDigit) {
  int theDigit = theChar - '0';
  bool result = (theDigit < 10 && theDigit >= 0);
  if (result && whichDigit != nullptr) {
    *whichDigit = theDigit;
  }
  return result;
}

bool MathRoutines::IsAHexDigit(char digitCandidate) {
  if (MathRoutines::isADigit(digitCandidate)) {
    return true;
  }
  if (digitCandidate >= 'A' && digitCandidate <= 'F') {
    return true;
  }
  if (digitCandidate >= 'a' && digitCandidate <= 'f') {
    return true;
  }
  return false;
}

bool MathRoutines::isADigit(const std::string& input, int* whichDigit) {
  if (input.size() != 1) {
    return false;
  }
  return MathRoutines::isADigit(input[0], whichDigit);
}

int MathRoutines::lcm(int a, int b) {
  if (a < 0) {
    a = - a;
  }
  if (b < 0) {
    b = - b;
  }
  return ((a * b) / Rational::gcdSigned(a, b));
}

int MathRoutines::TwoToTheNth(int n) {
  int result = 1;
  for (int i = 0; i < n; i ++) {
    result *= 2;
  }
  return result;
}

int MathRoutines::KToTheNth(int k, int n) {
  int result = 1;
  for (int i = 0; i < n; i ++) {
    result *= k;
  }
  return result;
}

void MathRoutines::KToTheNth(int k, int n, LargeInteger& output) {
  output = 1;
  for (int i = 0; i < n; i ++) {
    output *= k;
  }
}

Vector<double> MathRoutines::GetVectorDouble(Vector<Rational>& input) {
  Vector<double> result;
  result.SetSize(input.size);
  for (int i = 0; i < input.size; i ++) {
    result[i] = input[i].GetDoubleValue();
  }
  return result;
}

int MathRoutines::BinomialCoefficientMultivariate(int N, List<int>& theChoices) {
  int ChoiceIndex = 0;
  int denominator = 1;
  int result = 0;
  for ( int i = N; i > 0; i --) {
    result *= i;
    result /= denominator;
    denominator ++;
    if (denominator > theChoices[ChoiceIndex]) {
      denominator = 1;
      ChoiceIndex ++;
    }
  }
  return result;
}

void Selection::init(int maxNumElements) {
  this->selected.SetSize(maxNumElements);
  this->elements.SetSize(maxNumElements);
  this->MaxSize = maxNumElements;
  for (int i = 0; i < this->MaxSize; i ++) {
    this->selected[i] = false;
  }
  this->CardinalitySelection = 0;
}

void Selection::AddSelectionAppendNewIndex(int index) {
  if (index >= this->MaxSize || index < 0) {
    return;
  }
  if (this->selected[index]) {
    return;
  }
  this->selected[index] = true;
  this->elements[this->CardinalitySelection] = index;
  this->CardinalitySelection ++;
}

Selection::Selection() {
  this->MaxSize = - 1;
  this->CardinalitySelection = 0;
}

Selection::Selection(int m) {
  this->CardinalitySelection = 0;
  this->init(m);
}

void Selection::RemoveLastSelection() {
  if (this->CardinalitySelection == 0) {
    return;
  }
  this->selected[this->elements[this->CardinalitySelection - 1]] = false;
  this->CardinalitySelection--;
}

int Selection::SelectionToIndex() {
  int result = 0;
  for (int i = 0; i < MaxSize; i ++) {
    result *= 2;
    if (this->selected[i]) {
      result += 1;
    }
  }
  return result;
}

void Selection::ShrinkMaxSize() {
  MaxSize --;
  if (selected[MaxSize]) {
    CardinalitySelection --;
  }
}

void Selection::ExpandMaxSize() {
  this->elements[this->CardinalitySelection] = this->MaxSize;
  this->selected[this->MaxSize] = true;
  this->MaxSize ++;
  this->CardinalitySelection ++;
}

void Selection::WriteToFile(std::fstream& output) {
  output << "Sel_max_size: " << this->MaxSize << " cardinality: " << this->CardinalitySelection << " ";
  for (int i = 0; i < this->CardinalitySelection; i ++) {
    output << this->elements[i] << " ";
  }
}

void Selection::ReadFromFile(std::fstream& input) {
  std::string tempS; int tempI, card;
  input >> tempS >> tempI;
  this->init(tempI);
  input >> tempS >> card;
  if (tempS != "cardinality:") {
    global.fatal << "Failed to read selection from file. " << global.fatal;
  }
  for (int i = 0; i < card; i ++) {
    input >> tempI;
    this->AddSelectionAppendNewIndex(tempI);
  }
}

std::string Selection::ToString() const {
  Vector<Rational> tempRoot;
  tempRoot = *this;
  return tempRoot.ToString();
}

void Selection::incrementSelection() {
  for (int i = this->MaxSize - 1; i >= 0; i --) {
    this->selected[i] = !this->selected[i];
    if (this->selected[i]) {
      this->ComputeIndicesFromSelection();
      return;
    }
  }
  this->ComputeIndicesFromSelection();
}

void Selection::initSelectionFixedCardinality(int card) {
  this->initNoMemoryAllocation();
  for (int i = 0; i < card; i ++) {
    this->selected[i] = true;
    this->elements[i] = i;
  }
  this->CardinalitySelection = card;
}

void Selection::incrementSelectionFixedCardinality(int card) {
  //example of the order of generation of all combinations when card = 2 and MaxSize = 5. The second column indicates the
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
  if (card > this->MaxSize) {
    return;
  }
  if (this->CardinalitySelection != card) {
    this->initSelectionFixedCardinality(card);
    return;
  }
  if (card == this->MaxSize || card == 0) {
    return;
  }
  int IndexLastZeroWithOneBefore = - 1;
  int NumOnesAfterLastZeroWithOneBefore = 0;
  for (int i = this->MaxSize - 1; i >= 0; i --) {
    if (this->selected[i]) {
      if (IndexLastZeroWithOneBefore == - 1) {
        NumOnesAfterLastZeroWithOneBefore ++;
      } else {
        break;
      }
    } else {
      IndexLastZeroWithOneBefore = i;
    }
  }
  if (IndexLastZeroWithOneBefore == 0) {
    this->initSelectionFixedCardinality(card);
    return;
  }
  for (int i = 0; i < NumOnesAfterLastZeroWithOneBefore + 1; i ++) {
    this->selected[this->elements[this->CardinalitySelection - i - 1]] = false;
  }
  //***At this point in time the second column is recorded
  for (int i = 0; i < NumOnesAfterLastZeroWithOneBefore + 1; i ++) {
    this->selected[i + IndexLastZeroWithOneBefore] = true;
    this->elements[this->CardinalitySelection + i - NumOnesAfterLastZeroWithOneBefore - 1] = i + IndexLastZeroWithOneBefore;
  }
}

void Selection::ComputeIndicesFromSelection() {
  this->CardinalitySelection = 0;
  for (int i = 0; i < this->MaxSize; i ++) {
    if (this->selected[i]) {
      this->elements[CardinalitySelection] = i;
      this->CardinalitySelection ++;
    }
  }
}

void Selection::initNoMemoryAllocation() {
  for (int i = 0; i < this->CardinalitySelection; i ++) {
    this->selected[this->elements[i]] = false;
  }
  this->CardinalitySelection = 0;
}

void Selection::MakeSubSelection(Selection &theSelection, Selection &theSubSelection) {
  this->init(theSelection.MaxSize);
  for (int i = 0; i < theSubSelection.CardinalitySelection; i ++) {
    this->AddSelectionAppendNewIndex(theSelection.elements[theSubSelection.elements[i]]);
  }
}

void Selection::operator=(const Selection& right) {
  if (this == &right) {
    return;
  }
  if (this->MaxSize != right.MaxSize) {
    this->init(right.MaxSize);
  } else {
    this->initNoMemoryAllocation();
  }
  for (int i = 0; i < right.CardinalitySelection; i ++) {
    this->elements[i] = right.elements[i];
    this->selected[this->elements[i]] = true;
  }
  this->CardinalitySelection = right.CardinalitySelection;
}

unsigned int Selection::HashFunction() const {
  int tempMin = MathRoutines::Minimum(SomeRandomPrimesSize, this->MaxSize);
  unsigned int result = 0;
  for (int i = 0; i < tempMin; i ++) {
    if (this->selected[i]) {
      result += static_cast<unsigned>(i) * SomeRandomPrimes[i];
    }
  }
  return result;
}

//Format expression monomial orders (for the ToString() function follow.

template<>
List<MonomialWrapper<std::string, MathRoutines::HashString> >::Comparator*
FormatExpressions::GetMonOrder<MonomialWrapper<std::string, MathRoutines::HashString> >() {
  return nullptr;
}

template<>
List<MonomialP>::Comparator* FormatExpressions::GetMonOrder<MonomialP>() {
  return &this->monomialOrder;
}

template<>
List<Polynomial<AlgebraicNumber> >::Comparator* FormatExpressions::GetMonOrder<Polynomial<AlgebraicNumber> >() {
  return nullptr;
}

template<>
List<MonomialVector>::Comparator*
FormatExpressions::GetMonOrder<MonomialVector>() {
  return nullptr;
}

template<>
List<MonomialWeylAlgebra>::Comparator*
FormatExpressions::GetMonOrder<MonomialWeylAlgebra>() {
  return nullptr;
}

template<>
List<MonomialUniversalEnveloping<RationalFunctionOld> >::Comparator*
FormatExpressions::GetMonOrder<MonomialUniversalEnveloping<RationalFunctionOld> >() {
  return nullptr;
}

template<>
List<MonomialGeneralizedVerma<RationalFunctionOld> >::Comparator*
FormatExpressions::GetMonOrder<MonomialGeneralizedVerma<RationalFunctionOld> >() {
  return nullptr;
}

template<>
List<ChevalleyGenerator >::Comparator*
FormatExpressions::GetMonOrder<ChevalleyGenerator>() {
  return nullptr;
}

template<>
List<MonomialMatrix>::Comparator*
FormatExpressions::GetMonOrder<MonomialMatrix>() {
  return nullptr;
}

template<>
List<MonomialUniversalEnveloping<Rational> >::Comparator*
FormatExpressions::GetMonOrder<MonomialUniversalEnveloping<Rational> >() {
  return nullptr;
}

template<>
List<MonomialTensorGeneralizedVermas<RationalFunctionOld> >::Comparator*
FormatExpressions::GetMonOrder<MonomialTensorGeneralizedVermas<RationalFunctionOld> >() {
  return nullptr;
}

template<>
List<quasiDiffMon>::Comparator*
FormatExpressions::GetMonOrder<quasiDiffMon>() {
  return nullptr;
}

template<>
List<MonomialUniversalEnveloping<Polynomial<Rational> > >::Comparator*
FormatExpressions::GetMonOrder<MonomialUniversalEnveloping<Polynomial<Rational> > >() {
  return nullptr;
}

FormatExpressions::FormatExpressions() {
  this->AmbientWeylLetter = 'X';
  this->ExtraLinesCounterLatex = 0;
  this->chevalleyGgeneratorLetter = "g";
  this->chevalleyHgeneratorLetter = "h";
  this->polyDefaultLetter = "x";
  this->WeylAlgebraDefaultLetter = "\\partial";
  this->FDrepLetter = "V";
  this->simpleRootLetter = "\\eta";
  this->MaxLineLength = 100;
  this->flagPassCustomCoeffMonSeparatorToCoeffs = false;
  this->flagUseCalculatorFormatForUEOrdered = true;
  this->flagUseHTML = true;
  this->flagUseLatex = false;
  this->flagUsePNG = false;
  this->flagUsePmatrix = true;
  this->MatrixColumnVerticalLineIndex = - 1;
  this->flagQuasiDiffOpCombineWeylPart = true;
  this->flagMakingExpressionTableWithLatex = false;
  this->MaxLinesPerPage = 40;
  this->NumAmpersandsPerNewLineForLaTeX = 0;
  this->MaxRecursionDepthPerExpression = 500;
  this->monomialOrder = nullptr;
  this->flagExpressionIsFinal = true;
  this->flagExpressionNewLineAllowed = false;
  this->flagIncludeLieAlgebraTypes = true;
  this->flagUseReflectionNotation = false;
  this->flagUseHtmlAndStoreToHD = false;
  this->flagIncludeMutableInformation = true;
  this->flagUseMathSpanPureVsMouseHover = false;
  this->flagDynkinTypeDontUsePlusAndExponent = false;
  this->flagSupressDynkinIndexOne = false;
  this->flagFormatWeightAsVectorSpaceIndex = true;
  this->flagUseFrac = false;
  this->flagSuppressOneIn1overXtimesY = true;
  this->flagFormatMatrixAsLinearSystem = false;
  this->flagIncludeExtraHtmlDescriptionsInPlots = true;
  this->flagDontCollalpseProductsByUnits = false;
  this->flagSuppressLongMatrices = true;
  this->flagLatexDetailsInHtml = false;
  this->flagUseQuotes = true;
  this->MaxMatrixDisplayedRows = 20;
  this->MaxMatrixLineLength = 20;
}

std::string FormatExpressions::GetPolyLetter(int index) const {
  if (index < this->polyAlphabeT.size) {
    return this->polyAlphabeT[index];
  }
  std::stringstream out;
  out << this->polyDefaultLetter << "_{" << index + 1 << "}";
  std::string tempS = out.str();
  return tempS;
}

bool PartFraction::reduceOnceTotalOrderMethod(
  LinearCombination<PartFraction, Polynomial<LargeInteger> >& output, PartFractions& owner
) {
  for (int i = 0; i < this->IndicesNonZeroMults.size; i ++) {
    for (int j = 0; j < this->IndicesNonZeroMults.size; j ++) {
      int AminusBindex = owner.TableAllowedAminusB.elements[this->IndicesNonZeroMults[i]][this->IndicesNonZeroMults[j]];
      int Aminus2Bindex = owner.TableAllowedAminus2B.elements[this->IndicesNonZeroMults[i]][this->IndicesNonZeroMults[j]];
      if (AminusBindex != - 1 &&  AminusBindex > this->IndicesNonZeroMults[j]) {
        this->decomposeAMinusNB(
          this->IndicesNonZeroMults[i], this->IndicesNonZeroMults[j], 1, AminusBindex, output, owner
        );
        return true;
      }
      if (Aminus2Bindex != - 1 &&  Aminus2Bindex > this->IndicesNonZeroMults[j]) {
        this->decomposeAMinusNB(
          this->IndicesNonZeroMults[i], this->IndicesNonZeroMults[j], 2, Aminus2Bindex, output, owner
        );
        return true;
      }
    }
  }
  return false;
}

bool PartFraction::reduceOnceGeneralMethodNoOSBasis(
  PartFractions& owner,
  LinearCombination<PartFraction, Polynomial<LargeInteger> >& output,
  Vectors<Rational>& bufferVectors,
  Matrix<Rational>& bufferMat
) {
  if (this->IndicesNonZeroMults.size == owner.AmbientDimension) {
    return false;
  }
  Vectors<Rational>& tempRoots = bufferVectors;
  Matrix<Rational>& tempMat = bufferMat;
  tempRoots.size = 0;
  int IndexInLinRelationOfLastGainingMultiplicityIndex = - 1;
  Vector<Rational> tempRoot;
  for (int i = 0; i < this->IndicesNonZeroMults.size; i ++) {
    tempRoot.SetSize(owner.AmbientDimension);
    int currentIndex = this->IndicesNonZeroMults[i];
    if (currentIndex == this->LastDistinguishedIndex) {
      IndexInLinRelationOfLastGainingMultiplicityIndex = i;
    }
    tempRoot = owner.startingVectors[currentIndex] * this->TheObjects[currentIndex].GetLargestElongation();
    tempRoots.AddOnTop(tempRoot);
    bool ShouldDecompose;
    ShouldDecompose = tempRoots.GetLinearDependence(tempMat);
    if (ShouldDecompose && this->LastDistinguishedIndex != - 1) {
      if (IndexInLinRelationOfLastGainingMultiplicityIndex == - 1) {
        ShouldDecompose = false;
      } else {
        ShouldDecompose = !tempMat.elements[IndexInLinRelationOfLastGainingMultiplicityIndex][0].IsEqualToZero();
      }
    }
    if (ShouldDecompose) {
      this->DecomposeFromLinRelation(tempMat, output, owner.flagUsingOrlikSolomonBasis, owner.startingVectors);
      return true;
    }
  }
  this->LastDistinguishedIndex = - 1;
  return false;
}

bool PartFraction::ReduceOnceGeneralMethod(
  PartFractions& owner,
  LinearCombination<PartFraction, Polynomial<LargeInteger> >& output,
  Vectors<Rational>& bufferVectors,
  Matrix<Rational>& bufferMat
) {
  if (this->IndicesNonZeroMults.size == owner.AmbientDimension) {
    return false;
  }
  Matrix<Rational>& tempMat = bufferMat;
  bufferVectors.size = 0;
  this->LastDistinguishedIndex = this->getSmallestNonZeroIndexGreaterThanOrEqualTo(owner, this->LastDistinguishedIndex);
  int IndexInLinRelationOfLastGainingMultiplicityIndex = - 1;
  Vector<Rational> tempRoot;
  for (int i = 0; i < this->IndicesNonZeroMults.size; i ++) {
    tempRoot.SetSize(owner.AmbientDimension);
    int currentIndex = this->IndicesNonZeroMults[i];
    if (currentIndex == this->LastDistinguishedIndex) {
      IndexInLinRelationOfLastGainingMultiplicityIndex = i;
    }
    tempRoot = owner.startingVectors[currentIndex];
    tempRoot *= (this->TheObjects[currentIndex].GetLargestElongation());
    bufferVectors.AddOnTop(tempRoot);
    bool ShouldDecompose;
    ShouldDecompose = bufferVectors.GetLinearDependence(tempMat);
    if (ShouldDecompose && (
      this->LastDistinguishedIndex != - 1 ||
      this->LastDistinguishedIndex == owner.startingVectors.size
    )) {
      if (IndexInLinRelationOfLastGainingMultiplicityIndex == - 1) {
        ShouldDecompose = false;
      } else {
        ShouldDecompose = !tempMat.elements[IndexInLinRelationOfLastGainingMultiplicityIndex][0].IsEqualToZero();
      }
    }
    if (ShouldDecompose) {
      if (this->flagAnErrorHasOccurredTimeToPanic) {
      }
      this->DecomposeFromLinRelation(tempMat, output, owner.flagUsingOrlikSolomonBasis, owner.startingVectors);
      if (this->flagAnErrorHasOccurredTimeToPanic) {
      }
      return true;
    }
  }
  return false;
}

int PartFraction::SizeWithoutDebugString() const {
  int Accum = 0;
  Accum += this->::ListLight<oneFracWithMultiplicitiesAndElongations>::SizeWithoutObjects();
  Accum += this->size * static_cast<int>(sizeof(oneFracWithMultiplicitiesAndElongations));
  Accum += this->IndicesNonZeroMults.SizeWithoutObjects();
  return Accum;
}

void PartFraction::AssignDenominatorOnly(const PartFraction& p) {
  this->CopyFromLight(p);
  this->ComputeIndicesNonZeroMults();
}

void PartFraction::Assign(const PartFraction& p) {
  this->CopyFromLight(p);
  this->IndicesNonZeroMults = (p.IndicesNonZeroMults);
  this->IsIrrelevant = p.IsIrrelevant;
  this->RelevanceIsComputed = p.RelevanceIsComputed;
  this->LastDistinguishedIndex = p.LastDistinguishedIndex;
}

void PartFraction::AssignNoIndicesNonZeroMults(PartFraction& p) {
  this->CopyFromLight(p);
}

void PartFraction::WriteToFile(std::fstream& output) const {
  output << "Fraction_start: " << this->size << "\n";
  for (int j = 0; j < this->size; j ++) {
    output << (*this)[j].Multiplicities.size << " ";
    for (int i = 0; i < (*this)[j].Multiplicities.size; i ++) {
      output << (*this)[j].Multiplicities[i] << " " << (*this)[j].Elongations[i] << " ";
    }
  }
}

void PartFraction::ReadFromFile(PartFractions& owner, std::fstream& input) {
  std::string tempS;
  int tempI;
  input >> tempS >> tempI;
  if (tempI != owner.startingVectors.size) {
    global.fatal << "Failed to read partial fraction from file. " << global.fatal;
  }
  this->init(tempI);
  for (int j = 0; j < this->size; j ++) {
    input >> tempI;
    (*this)[j].Multiplicities.SetSize(tempI);
    (*this)[j].Elongations.SetSize(tempI);
    for (int i = 0; i < (*this)[j].Multiplicities.size; i ++) {
      input >> (*this)[j].Multiplicities[i] >> (*this)[j].Elongations[i];
    }
  }
  this->ComputeIndicesNonZeroMults();
}

void PartFraction::ComputeOneCheckSuM(PartFractions& owner, Rational& output, int theDimension) const {
  if (this->flagAnErrorHasOccurredTimeToPanic) {
  }
  Vector<Rational> CheckSumRoot = oneFracWithMultiplicitiesAndElongations::GetCheckSumRoot(owner.AmbientDimension);
  Rational tempRat;
  for (int i = 0; i < this->IndicesNonZeroMults.size; i ++) {
    this->TheObjects[this->IndicesNonZeroMults[i]].ComputeOneCheckSum(
      tempRat, owner.startingVectors[this->IndicesNonZeroMults[i]], theDimension
    );
    output.MultiplyBy(tempRat);
  }
}

std::string PartFraction::ToString(
  bool LatexFormat, FormatExpressions& PolyFormatLocal, int& NumLinesUsed
) {
  std::stringstream out;
  std::string tempS, stringPoly;
  NumLinesUsed = 0;
  for (int i = 0; i < this->size; i ++) {
    this->TheObjects[i].ToString();
    out << tempS;
  }
  tempS = out.str();
  if (LatexFormat) {
    out << stringPoly;
    if (stringPoly.size() > static_cast<unsigned>(PolyFormatLocal.MaxLineLength)) {
      out << ("\\\\\n&&");
      NumLinesUsed++;
    }
    out << tempS;
  } else {
    out << stringPoly;
    out << tempS;
  }
  return out.str();
}

bool PartFraction::rootIsInFractionCone(PartFractions& owner, Vector<Rational>* theRoot) const {
  if (theRoot == nullptr) {
    return true;
  }
  if (this->RelevanceIsComputed) {
    return !this->IsIrrelevant;
  }
  if (PartFraction::flagAnErrorHasOccurredTimeToPanic) {
  }
  Cone tempCone;
  Vectors<Rational> tempRoots;
  for (int i = 0; i < this->IndicesNonZeroMults.size; i ++) {
    int tempI = this->IndicesNonZeroMults[i];
    tempRoots.AddOnTop(owner.startingVectors[tempI]);
  }
  tempCone.CreateFromVertices(tempRoots);
  return !this->IsIrrelevant;
}

void PartFraction::PrepareFraction(
  int indexA,
  int indexB,
  int AminusNBindex,
  bool indexAisNullified,
  PartFraction& output,
  Polynomial<LargeInteger>& AminusNbetaPoly,
  Polynomial<LargeInteger>& outputCommonCoeff
) {
  output.AssignNoIndicesNonZeroMults(*this);
  int powerDropA = this->TheObjects[indexA].Multiplicities[0];
  int powerDropB = this->TheObjects[indexB].Multiplicities[0];
  if (indexAisNullified) {
    powerDropB = 0;
  } else {
    powerDropA = 0;
  }
  outputCommonCoeff.MakeOne(AminusNbetaPoly.GetMinNumVars());
  for (int i = 0; i < powerDropB; i ++) {
    outputCommonCoeff *= AminusNbetaPoly;
  }
  output.DecreasePowerOneFrac(indexA, powerDropA);
  output.DecreasePowerOneFrac(indexB, powerDropB);
  output.DecreasePowerOneFrac(AminusNBindex, - powerDropA - powerDropB);
}

int PartFraction::GetNumProportionalVectorsClassicalRootSystems(PartFractions& owner) {
  int result = 0;
  for (int i = 0; i < owner.IndicesRedundantShortRoots.CardinalitySelection; i ++) {
    int tempI = owner.IndicesRedundantShortRoots.elements[i];
    int tempI2 = owner.getIndexDoubleOfARoot(owner.startingVectors[tempI]);
    if (tempI2 != - 1) {
      if (this->TheObjects[tempI2].Multiplicities.size > 0) {
        result ++;
      }
    }
  }
  return result;
}

int PartFraction::getSmallestNonZeroIndexGreaterThanOrEqualTo(PartFractions& owner, int minIndex) {
  int result = owner.startingVectors.size;
  for (int i = 0; i < this->IndicesNonZeroMults.size; i ++) {
    int tempI = this->IndicesNonZeroMults[i];
    if ((*this)[tempI].Multiplicities.size > 0) {
      if (tempI >= minIndex && tempI < result) {
        result = tempI;
      }
    }
  }
  return result;
}

bool PartFraction::CheckForOrlikSolomonAdmissibility(List<int>& theSelectedIndices) {
  global.fatal << "Orlik solomon admissability not implemented yet " << global.fatal;
  (void) theSelectedIndices;
  return true;
}

int PartFraction::ComputeGainingMultiplicityIndexInLinearRelation(
  bool flagUsingOrlikSolomon,  Matrix<Rational> & theLinearRelation
) {
  int DesireToSelectAsGainingMultiplicity = - 1;
  int result = - 1;
  for (int i = 0; i < theLinearRelation.NumRows; i ++) {
    if (!theLinearRelation.elements[i][0].IsEqualToZero()) {
      int currentIndex = this->IndicesNonZeroMults[i];
      int candidateDesire;
      if (!flagUsingOrlikSolomon) {
        candidateDesire = (*this)[currentIndex].GetTotalMultiplicity();
      } else {
        candidateDesire = currentIndex;
      }
      if (candidateDesire < 0) {
        candidateDesire = - candidateDesire;
      }
      if (result == - 1 || DesireToSelectAsGainingMultiplicity < candidateDesire) {
        result = i;
        DesireToSelectAsGainingMultiplicity = candidateDesire;
      }
    }
  }
  return result;
}

bool PartFraction::DecomposeFromLinRelation(
  Matrix<Rational>& theLinearRelation,
  LinearCombination<PartFraction, Polynomial<LargeInteger> >& output,
  bool flagUsingOSbasis,
  List<Vector<Rational> >& startingVectors
) {
 //  theLinearRelation.ComputeDebugString();
  //theLinearRelation.ComputeDebugString();
  int GainingMultiplicityIndexInLinRelation = - 1;
  int GainingMultiplicityIndex = - 1;
  int ElongationGainingMultiplicityIndex = - 1;
  List<int> ParticipatingIndices;
  List<int> theGreatestElongations;
  List<int> theCoefficients;
  ParticipatingIndices.size = 0;
  theCoefficients.size = 0;
  theGreatestElongations.size = 0;
  GainingMultiplicityIndexInLinRelation = this->ComputeGainingMultiplicityIndexInLinearRelation(flagUsingOSbasis, theLinearRelation);
  GainingMultiplicityIndex = this->IndicesNonZeroMults[GainingMultiplicityIndexInLinRelation];
  int tempI = this->TheObjects[GainingMultiplicityIndex].GetLargestElongation();
  theLinearRelation.elements[GainingMultiplicityIndexInLinRelation][0].MultiplyByInt(tempI);
  //theLinearRelation.ComputeDebugString();
  theLinearRelation.ScaleToIntegralForMinRationalHeightNoSignChange();
  if (this->flagAnErrorHasOccurredTimeToPanic) {
  }
  ElongationGainingMultiplicityIndex = theLinearRelation.elements[GainingMultiplicityIndexInLinRelation][0].NumShort;
  if (ElongationGainingMultiplicityIndex < 0) {
    ElongationGainingMultiplicityIndex = - ElongationGainingMultiplicityIndex;
  } else {
    theLinearRelation *= - 1;
  }
  //theLinearRelation.ComputeDebugString();
  for (int i = 0; i < theLinearRelation.NumRows; i ++) {
    if (i != GainingMultiplicityIndexInLinRelation && !theLinearRelation.elements[i][0].IsEqualToZero()) {
      int tempI = this->IndicesNonZeroMults[i];
      ParticipatingIndices.AddOnTop(tempI);
      theGreatestElongations.AddOnTop((*this)[tempI].GetLargestElongation());
      theCoefficients.AddOnTop(theLinearRelation.elements[i][0].NumShort);
    }
  }
  if (!flagUsingOSbasis) {
    this->LastDistinguishedIndex = GainingMultiplicityIndex;
  }
  if (PartFraction::flagAnErrorHasOccurredTimeToPanic) {
  }
  this->ApplyGeneralizedSzenesVergneFormulA(
    ParticipatingIndices,
    theGreatestElongations,
    theCoefficients,
    GainingMultiplicityIndex,
    ElongationGainingMultiplicityIndex,
    output,
    startingVectors
  );

  //if (this->MakingConsistencyCheck)
  //{ if (!this->CheckSum2.IsEqualTo(this->CheckSum))global.fatal << global.fatal;
  //}
  /*if (PartFraction::flagAnErrorHasOccurredTimeToPanic) {
    Rational tempRat2, tempRat;
    std::string tempS1, tempS2;
    Accum.ComputeOneCheckSum(tempRat2);
    this->ComputeOneCheckSum(tempRat);
    tempRat.ToString(tempS1);
    this->CheckSum2.ToString(tempS2);
    tempRat2.Subtract(tempRat);
    if (!oldCheckSum.IsEqualTo(tempRat2))global.fatal << global.fatal;
  }*/
  //Accum.ComputeDebugString();
  return true;
}

bool PartFraction::ReduceMeOnce(
  const Polynomial<LargeInteger>& myCoeff,
  Polynomial<LargeInteger>& outputCoeff,
  Vectors<Rational>& startingVectors
) {
  bool hasImprovement = true;
  bool improvedAtLeastOnce = false;
  outputCoeff = myCoeff;
  Polynomial<LargeInteger> denominator, quotient, remainderDivision;
  while (hasImprovement) {
    hasImprovement = false;
    for (int i = 0; i < this->IndicesNonZeroMults.size; i ++) {
      for (int j = 0; j < this->TheObjects[IndicesNonZeroMults[i]].Multiplicities.size; j ++) {
        this->TheObjects[IndicesNonZeroMults[i]].GetPolyDenominator(denominator, j, startingVectors[IndicesNonZeroMults[i]]);
        outputCoeff.DivideBy(denominator, quotient, remainderDivision, &MonomialP::orderDefault());
        if (remainderDivision.IsEqualToZero()) {
          this->DecreasePowerOneFrac(IndicesNonZeroMults[i], 1);
          outputCoeff = quotient;
          hasImprovement = true;
          improvedAtLeastOnce = true;
        }
      }
    }
  }
  return improvedAtLeastOnce;
}

void PartFraction::GetNElongationPolyWithMonomialContribution(
  List<Vector<Rational> >& startingVectors,
  List<int>& theSelectedIndices,
  List<int>& theCoefficients,
  List<int>& theGreatestElongations,
  int theIndex,
  Polynomial<LargeInteger>& output,
  int theDimension
) {
  MonomialP tempM;
  tempM.MakeOne(theDimension);
  for (int i = 0; i < theIndex; i ++) {
    int tempI = theSelectedIndices[i];
    for (int j = 0; j < theDimension; j ++) {
      tempM[j] += startingVectors[tempI][j] * theCoefficients[i] * theGreatestElongations[i];
    }
  }
  this->GetNElongationPoly(
    startingVectors,
    theSelectedIndices[theIndex],
    theGreatestElongations[theIndex],
    theCoefficients[theIndex],
    output,
    theDimension
  );
  output.MultiplyBy(tempM, 1);
}

void PartFraction::ApplyGeneralizedSzenesVergneFormulA(
  List<int>& theSelectedIndices,
  List<int>& theGreatestElongations,
  List<int>& theCoefficients,
  int GainingMultiplicityIndex,
  int ElongationGainingMultiplicityIndex,
  LinearCombination<PartFraction, Polynomial<LargeInteger> >& output,
  List<Vector<Rational> >& startingVectors
) {
  PartFraction tempFrac;
  tempFrac.RelevanceIsComputed = false;
  Polynomial<LargeInteger> tempP;
  Polynomial<LargeInteger> ComputationalBufferCoefficient;
  output.MakeZero();
  int theDim = startingVectors[0].size;
  SelectionWithDifferentMaxMultiplicities TheBigBadIndexingSet;
  TheBigBadIndexingSet.initPart1(theSelectedIndices.size);
  int TotalMultiplicity;
  TotalMultiplicity = 0;
  for (int i = 0; i < theSelectedIndices.size; i ++) {
    int tempI = (*this)[theSelectedIndices[i]].GetMultiplicityLargestElongation() - 1;
    TheBigBadIndexingSet.MaxMultiplicities[i] = tempI;
    TotalMultiplicity += tempI;
  }
  for (int i = 0; i < theSelectedIndices.size; i ++) {
    TheBigBadIndexingSet.clearNoMaxMultiplicitiesChange();
    int oldMaxMultiplicity = TheBigBadIndexingSet.MaxMultiplicities[i];
    TheBigBadIndexingSet.MaxMultiplicities[i] = 0;
    int NumSubsets = TheBigBadIndexingSet.TotalNumSubsetsMustBeSmalInt();
    for (int j = 0; j < NumSubsets; j ++) {
      tempFrac.Assign(*this);
      tempFrac.RelevanceIsComputed = false;
      int tempN = TheBigBadIndexingSet.TotalMultiplicity() + oldMaxMultiplicity;
      for (int k = 0; k < theSelectedIndices.size; k ++) {
        int multiplicityChange;
        if (k != i) {
          multiplicityChange = TheBigBadIndexingSet.Multiplicities[k];
        } else {
          multiplicityChange = oldMaxMultiplicity + 1;
        }
        tempFrac[theSelectedIndices[k]].AddMultiplicity(- multiplicityChange, theGreatestElongations[k]);
        this->GetNElongationPolyWithMonomialContribution(
          startingVectors, theSelectedIndices, theCoefficients, theGreatestElongations, k, tempP, theDim
        );
        tempP.RaiseToPower(multiplicityChange);
        ComputationalBufferCoefficient *= (tempP);
        LargeInteger tempInt;
        int tempI;
        if (k == i) {
          tempI = oldMaxMultiplicity;
        } else {
          tempI = multiplicityChange;
        }
        MathRoutines::NChooseK(tempN, tempI, tempInt);
        ComputationalBufferCoefficient *= tempInt;
        tempN -= tempI;
      }
      tempFrac[GainingMultiplicityIndex].AddMultiplicity(
        TheBigBadIndexingSet.TotalMultiplicity() + oldMaxMultiplicity + 1,
        ElongationGainingMultiplicityIndex
      );
      tempFrac.ComputeIndicesNonZeroMults();
      output.AddMonomial(tempFrac, ComputationalBufferCoefficient);
      TheBigBadIndexingSet.IncrementReturnFalseIfPastLast();
    }
    TheBigBadIndexingSet.MaxMultiplicities[i] = oldMaxMultiplicity;
  }
}

void PartFraction::ApplySzenesVergneFormulA(
  List<Vector<Rational> >& startingVectors,
  List<int>& theSelectedIndices,
  List<int>& theElongations,
  int GainingMultiplicityIndex,
  int ElongationGainingMultiplicityIndex,
  LinearCombination<PartFraction, Polynomial<LargeInteger> >& output
) {
  PartFraction tempFrac;
  tempFrac.RelevanceIsComputed = false;
  Polynomial<LargeInteger> tempP, CoefficientBuffer;
  MonomialP tempM;
  output.MakeZero();
  int theDim = startingVectors[0].size;
  CoefficientBuffer.MakeOne(theDim);
  for (int i = 0; i < theSelectedIndices.size; i ++) {
    tempFrac.Assign(*this);
    tempFrac.RelevanceIsComputed = false;
    tempFrac[GainingMultiplicityIndex].AddMultiplicity(1, ElongationGainingMultiplicityIndex);
    oneFracWithMultiplicitiesAndElongations& currentFrac = tempFrac[theSelectedIndices[i]];
    int LargestElongation = currentFrac.GetLargestElongation();
    currentFrac.AddMultiplicity(- 1, LargestElongation);
    tempM.MakeOne(theDim);
    for (int j = 0; j < i; j ++) {
      int tempElongation = (*this)[theSelectedIndices[j]].GetLargestElongation();
      for (int k = 0; k < theDim; k ++) {
        tempM[k] += startingVectors[theSelectedIndices[j]][k] * theElongations[j] * tempElongation;
      }
    }
    ParallelComputing::SafePointDontCallMeFromDestructors();
    CoefficientBuffer.MultiplyBy(tempM, 1);
    this->GetNElongationPoly(startingVectors, theSelectedIndices[i], LargestElongation, theElongations[i], tempP, theDim);
    CoefficientBuffer *= tempP;
    tempFrac.ComputeIndicesNonZeroMults();
    output.AddMonomial(tempFrac, CoefficientBuffer);
  }
}

void PartFraction::decomposeAMinusNB(
  int indexA,
  int indexB,
  int n,
  int indexAminusNB,
  LinearCombination<PartFraction, Polynomial<LargeInteger> >& output,
  PartFractions& owner
) {
  PartFraction tempFrac;
  tempFrac.RelevanceIsComputed = false;
  Polynomial<LargeInteger> AminusNbetaPoly, commonPoly;
  this->GetAlphaMinusNBetaPoly(owner, indexA, indexB, n, AminusNbetaPoly);
  int powerA = (*this)[indexA].Multiplicities[0];
  int powerB = (*this)[indexB].Multiplicities[0];
  output.MakeZero();
  this->PrepareFraction(indexA, indexB, indexAminusNB, true, tempFrac, AminusNbetaPoly, commonPoly);
  for (int i = powerB; i >= 1; i --) {
    LargeInteger tempInt = MathRoutines::NChooseK(powerA + powerB - i - 1, powerA - 1);
    commonPoly *= tempInt;
    output.AddMonomial(tempFrac, commonPoly);
    commonPoly /= tempInt;
    if (i > 1) {
      tempFrac[indexAminusNB].AddMultiplicity(1, 1);
      tempFrac[indexB].AddMultiplicity(- 1, 1);
      commonPoly *= AminusNbetaPoly;
    }
  }
  this->PrepareFraction(indexA, indexB, indexAminusNB, false, tempFrac, AminusNbetaPoly, commonPoly);
  for (int i = powerA; i >= 1; i --) {
    LargeInteger tempInt = MathRoutines::NChooseK(powerA + powerB - i - 1, powerB - 1);
    commonPoly *= tempInt;
    output.AddMonomial(tempFrac, commonPoly);
    commonPoly /= tempInt;
    if (i > 1) {
      tempFrac[indexAminusNB].AddMultiplicity(1, 1);
      tempFrac[indexA].AddMultiplicity(- 1, 1);
    }
  }
}

bool PartFraction::DecreasePowerOneFrac(int index, int increment) {
  this->TheObjects[index].AddMultiplicity(- increment, 1);
  this->ComputeIndicesNonZeroMults();
  return true;
}

void PartFraction::ComputeIndicesNonZeroMults() {
  this->IndicesNonZeroMults.size = 0;
  for (int i = 0; i < this->size; i ++) {
    if ((*this)[i].Multiplicities.size > 0) {
      this->IndicesNonZeroMults.AddOnTop(i);
    }
  }
}

void PartFraction::GetAlphaMinusNBetaPoly(PartFractions& owner, int indexA, int indexB, int n, Polynomial<LargeInteger>& output) {
  output.MakeZero();
  MonomialP tempM;
  tempM.MakeOne(owner.AmbientDimension);
  for (int i = 0; i < n; i ++) {
    for (int j = 0; j < owner.AmbientDimension; j ++) {
      tempM[j] = owner.startingVectors[indexA][j] - owner.startingVectors[indexB][j] * (i + 1);
    }
    output.AddMonomial(tempM, - 1);
  }
}

void PartFraction::GetNElongationPoly(
  List<Vector<Rational> >& startingVectors,
  int index,
  int baseElongation,
  int LengthOfGeometricSeries,
  Polynomial<LargeInteger>& output,
  int theDimension
) {
  output.MakeZero();
  MonomialP tempM;
  tempM.MakeOne(theDimension);
  if (LengthOfGeometricSeries > 0) {
    for (int i = 0; i < LengthOfGeometricSeries; i ++) {
      for (int j = 0; j < theDimension; j ++) {
        tempM[j] = startingVectors[index][j] * baseElongation * i;
      }
      output.AddMonomial(tempM, 1);
    }
  } else {
    for (int i = - 1; i >= LengthOfGeometricSeries; i --) {
      for (int j = 0; j < theDimension; j ++) {
        tempM[j] = startingVectors[index][j] * baseElongation * i;
      }
      output.AddMonomial(tempM, - 1);
    }
  }
}

void PartFraction::MakePolynomialFromOneNormal(
  Vector<Rational>& normal, const MonomialP& shiftRational, int theMult, Polynomial<Rational>& output
) {
  int theDimension = normal.size;
  output.MakeOne(theDimension);
  if (theMult == 1) {
    return;
  }
  Rational tempRat, tempRat2;
  Polynomial<Rational> tempP;
  Vector<Rational> shiftRationalVector;
  shiftRationalVector.MakeZero(normal.size);
  for (int i = 0; i < normal.size; i ++) {
    shiftRationalVector[i] = shiftRational(i);
  }
  tempRat = normal.ScalarEuclidean(shiftRationalVector);
  for (int j = 0; j < theMult - 1; j ++) {
    tempP.MakeLinPolyFromRootNoConstantTerm(normal);
    tempRat2.AssignNumeratorAndDenominator(- 1, j + 1);
    tempRat2.MultiplyBy(tempRat);
    tempRat2 += 1;
    Rational tempRat3;
    tempRat3.AssignNumeratorAndDenominator(1, j + 1);
    tempP *= (tempRat3);
    tempP.AddConstant(tempRat2);
    output *= tempP;
  }
}

void PartFraction::ComputeNormals(PartFractions& owner, Vectors<Rational>& output, int theDimension, Matrix<Rational>& buffer) {
  Vectors<Rational> dens;
  Vector<Rational> tempRoot;
  tempRoot.SetSize(theDimension);
  dens.size = 0;
  output.size = 0;
  for (int i = 0; i < theDimension; i ++) {
    tempRoot = owner.startingVectors[this->IndicesNonZeroMults[i]];
    dens.AddOnTop(tempRoot);
  }
  Rational tempRat;
  for (int i = 0; i < theDimension; i ++) {
    dens.ComputeNormalExcludingIndex(tempRoot, i, buffer);
    tempRat = tempRoot.ScalarEuclidean(dens[i]);
    if (tempRat.IsEqualToZero()) {
      global.fatal << "Scalar product is equal to zero. " << global.fatal;
    }
    tempRoot /= tempRat;
    output.AddOnTop(tempRoot);
  }
}

PartFraction::PartFraction() {
  this->PowerSeriesCoefficientIsComputed = false;
  this->AlreadyAccountedForInGUIDisplay = false;
  this->FileStoragePosition = - 1;
  this->LastDistinguishedIndex = - 1;
  this->RelevanceIsComputed = false;
}

void PartFraction::init(int numRoots) {
  this->IndicesNonZeroMults.Reserve(numRoots);
  this->IndicesNonZeroMults.size = 0;
  this->SetSize(numRoots);
  for (int i = 0; i < this->size; i ++) {
    this->TheObjects[i].Elongations.SetSize(0);
    this->TheObjects[i].Multiplicities.SetSize(0);
  }
}

PartFraction::~PartFraction() {
}

unsigned int PartFraction::HashFunction() const {
  unsigned int result = 0;
  for (int i = 0; i < this->size; i ++) {
    result += SomeRandomPrimes[i] * this->TheObjects[i].HashFunction();
  }
  return result;
}

bool PartFraction::operator==(const PartFraction& right) const {
  if (this->size != right.size) {
    return false;
  }
  for (int i = 0; i < this->size; i ++) {
    if (!(this->TheObjects[i] == right[i])) {
      return false;
    }
  }
  return true;
}

void PartFraction::operator=(const PartFraction& right) {
  this->Assign(right);
}

int PartFractions::SizeWithoutDebugString() {
  int Accum = 0;
  Accum += this->theMonomials.SizeWithoutObjects();
  for (int i = 0; i < this->size(); i ++) {
    Accum += (*this)[i].SizeWithoutDebugString();
  }
  Accum += sizeof(this->HighestIndex) + sizeof(this->IndexLowestNonProcessed);
  return Accum;
}

bool PartFractions::AssureIndicatorRegularity(Vector<Rational>& theIndicator) {
  Vectors<Rational> tempRoots;
  tempRoots = this->startingVectors;
  if (theIndicator.IsEqualToZero()) {
    tempRoots.average(theIndicator, this->AmbientDimension);
    theIndicator *= tempRoots.size;
  }
  return tempRoots.PerturbVectorToRegular(theIndicator);
}

void PartFractions::PrepareCheckSums() {
  if (!this->flagUsingCheckSum) {
    return;
  }
  this->ComputeOneCheckSum(this->StartCheckSum);
}

void PartFractions::initFromOtherPartFractions(PartFractions& input) {
  this->MakeZero();
  this->startingVectors = input.startingVectors;
  this->IndexLowestNonProcessed = 0;
  this->IndexCurrentlyProcessed = 0;
  this->AmbientDimension = input.AmbientDimension;
}

void PartFractions::CompareCheckSums() {
  if (!this->flagUsingCheckSum) {
    return;
  }
  ProgressReport theReport1;
  ProgressReport theReport2;
  if (!this->flagDiscardingFractions) {
    this->ComputeOneCheckSum(this->EndCheckSum);
    if (!this->StartCheckSum.IsEqualTo(this->EndCheckSum) || this->flagAnErrorHasOccurredTimeToPanic) {
      std::stringstream out1, out2;
      out1 << "Starting checksum: " << this->StartCheckSum.ToString();
      out2 << "  Ending checksum: " << this->EndCheckSum.ToString();
      theReport1.Report(out1.str());
      theReport2.Report(out2.str());
    }
    if (!this->StartCheckSum.IsEqualTo(this->EndCheckSum)) {
      global.fatal << "<b>This is a programmign error. The checksum of the partial fractions failed. " << global.fatal;
    } else {
    }
  }
}

void PartFractions::PrepareIndicatorVariables() {
  this->NumberIrrelevantFractions = 0;
  this->NumberRelevantReducedFractions = 0;
  this->NumGeneratorsInTheNumerators = 0;
  this->NumGeneratorsIrrelevantFractions = 0;
  this->NumGeneratorsRelevenatFractions = 0;
  this->NumMonomialsInNumeratorsIrrelevantFractions = 0;
  this->NumMonomialsInNumeratorsRelevantFractions = 0;
  this->NumMonomialsInTheNumerators = 1;
  this->NumTotalReduced = 0;
  this->NumRelevantNonReducedFractions = 0;
  this->NumProcessedForVPFMonomialsTotal = 0;
  this->NumProcessedForVPFfractions = 0;
  this->NumRunsReduceMonomialByMonomial = 0;
}

bool PartFractions::splitPartial() {
  std::string OldDebugString;
  Matrix<Rational> bufferMat;
  Vectors<Rational> bufferRoots;
  std::stringstream out;
  std::string tempS;
  PartFraction currentFrac;
  LinearCombination<PartFraction, Polynomial<LargeInteger> > buffer;
  PartFractions reducedForGood;
  Polynomial<LargeInteger> currentCoeff;
  reducedForGood.MakeZero();
  if (this->flagUsingCheckSum) {
    this->ComputeOneCheckSum(this->CheckSum);
  }
  while (this->size() > 0) {
    this->PopMonomial(0, currentFrac, currentCoeff);
    bool tempBool;
    if (this->flagUsingOrlikSolomonBasis) {
      tempBool = currentFrac.ReduceOnceGeneralMethod(*this, buffer, bufferRoots, bufferMat);
    } else {
      tempBool = currentFrac.reduceOnceGeneralMethodNoOSBasis(*this, buffer, bufferRoots, bufferMat);
    }
    if (tempBool) {
      buffer *= currentCoeff;
      *this += buffer;
    } else {
      reducedForGood.AddMonomial(currentFrac, currentCoeff);
    }
    this->MakeProgressReportSplittingMainPart();
  }
  if (this->flagUsingCheckSum) {
    Rational tempRat;
    reducedForGood.ComputeOneCheckSum(tempRat);
    if (tempRat != this->CheckSum) {
      global.fatal << "This is a programming error. "
      << "The checksums of the partial fraction decomposition do not match. " << global.fatal;
    }
  }
  *this = reducedForGood;
  return true;
}

bool PartFractions::splitClassicalRootSystem(bool ShouldElongate, Vector<Rational>* Indicator) {
  (void) ShouldElongate;
  (void) Indicator;
  global.fatal << "Split classical root system not implemented" << global.fatal;
    /*this->IndexLowestNonProcessed = 0;
  this->PrepareIndicatorVariables();
  //PartFraction::flagAnErrorHasOccurredTimeToPanic = true;
  //PartFractions::flagAnErrorHasOccurredTimeToPanic = true;
  Rational::flagAnErrorHasOccurredTimeToPanic = true;
  this->PrepareCheckSums();
  std::string tempS;
  this->CheckSum.ToString(tempS);
  // if IndicatorRoot is zero then the caller has forgotten
  // to set the flagUsingIndicatorRoot to false
  PartFraction tempF;
  while (this->IndexLowestNonProcessed< this->size) {
    //this->ComputeDebugString();
//    if (!this->ShouldIgnore(Indicator)) {
      tempF.Assign(this->TheObjects[this->IndexLowestNonProcessed]);
      //this->ComputeDebugString();
      //tempF.ComputeDebugString();
      if (! (tempF.reduceOnceTotalOrderMethod(*this, Indicator))) {
        if (this->TheObjects[this->IndexLowestNonProcessed].IndicesNonZeroMults.size-this->TheObjects[this->IndexLowestNonProcessed].GetNumProportionalVectorsClassicalRootSystems(*this)>this->AmbientDimension) {
          this->TheObjects[this->IndexLowestNonProcessed].ComputeDebugString(*this);
          this->NumRelevantNonReducedFractions++;
        }
        this->IndexLowestNonProcessed++;
      }
      else
        this->PopIndexHashChooseSwapByLowestNonProcessedAndAccount( this->IndexLowestNonProcessed, Indicator);
      this->MakeProgressReportSplittingMainPart();
    }
//    this->ComputeDebugString();
//    x = this->SizeWithoutDebugString();
  }
  //this->ComputeDebugString();
//  this->CompareCheckSums();
  if (ShouldElongate) {
     this->RemoveRedundantShortRootsClassicalRootSystem(Indicator);
//  this->ComputeDebugString();
  }
  this->CompareCheckSums();
  this->IndexLowestNonProcessed = this->size;
  this->MakeProgressReportSplittingMainPart();*/
  return this->CheckForMinimalityDecompositionWithRespectToRoot(Indicator);
}

bool PartFractions::CheckForMinimalityDecompositionWithRespectToRoot(Vector<Rational>* theRoot) {
  for (int i = 0; i < this->size(); i ++) {
    if ((*this)[i].IndicesNonZeroMults.size > this->AmbientDimension) {
      if ((*this)[i].rootIsInFractionCone(*this, theRoot)) {
        return false;
      }
    }
  }
  return true;
}

bool PartFraction::initFromRoots(PartFractions& owner, Vectors<Rational>& input) {
  if (input.size == 0) {
    return false;
  }
  for (int i = 0; i < input.size; i ++) {
    if (input[i].IsEqualToZero()) {
      return false;
    }
  }
  owner.startingVectors = input;
  this->init(owner.startingVectors.size);
  for (int i = 0; i < owner.startingVectors.size; i ++) {
    this->TheObjects[i].init();
  }
  for (int i = 0; i < input.size; i ++) {
    int index = owner.getIndex(input[i]);
    this->TheObjects[index].AddMultiplicity(1, 1);
  }
  this->ComputeIndicesNonZeroMults();
  for (int i = 0; i < input.size; i ++) {
    for (int j = 0; j < input[i].size; j ++) {
      if (!input[i][j].IsSmallInteger()) {
        return false;
      }
    }
  }
  return true;
}

int PartFractions::ReadFromFileComputedContributions(std::fstream& input) {
  std::string tempS;
  input.seekg(0);
  input >> tempS;
  int lastNonZero = - 1;
  int fileStoragePosition;
  for (int i = 0; i < this->size(); i ++) {
    int x;
    input >> x >> fileStoragePosition;
    if (fileStoragePosition > lastNonZero) {
      lastNonZero = fileStoragePosition;
    }
    if (x != i) {
      global.fatal << "Failed to read. " << global.fatal;
    }
  }
  this->LimitSplittingSteps = 0;
  return lastNonZero;
}

void PartFractions::WriteToFileComputedContributions(std::fstream& output) {
  output.seekp(0);
  output << "Partial_fraction_index/file_storage_position\n";
  for (int i = 0; i < this->size(); i ++) {
    output << i << " " << (*this)[i].FileStoragePosition << "\n";
  }
}

PartFractions::PartFractions() {
  this->HighestIndex = - 1;
  this->IndexLowestNonProcessed = - 2;
  this->flagSplitTestModeNoNumerators = false;
  this->flagDiscardingFractions = false;
  this->flagUsingCheckSum = true;
  this->flagUsingOrlikSolomonBasis = false;
  this->flagInitialized = false;
  this->SplitStepsCounter = 0;
  this->LimitSplittingSteps = 0;
}

void PartFraction::ReduceMonomialByMonomial(PartFractions& owner, int myIndex, Vector<Rational>* Indicator) {
  (void) owner;
  (void) myIndex;
  (void) Indicator;
  /*PartFraction tempFrac;
  //tempFrac.Assign(*this);
  Rational StartCheckSum, theDiff;
  if (this->flagAnErrorHasOccurredTimeToPanic) {
    owner.ComputeOneCheckSum(StartCheckSum);
    this->ComputeOneCheckSuM(owner, theDiff, owner.AmbientDimension);
    this->ComputeDebugString(owner);
    owner.NumRunsReduceMonomialByMonomial ++;
  }
  Matrix<Rational> & tempMat = global.matReduceMonomialByMonomial.GetElement();
  Matrix<Rational> & startAsIdMat = global.matIdMatrix.GetElement();
  Matrix<Rational> & matColumn = global.matOneColumn.GetElement();
  Matrix<Rational> & matLinComb = global.matReduceMonomialByMonomial2.GetElement();
  Selection tempSel;
  MonomialP tempMon;
  Vector<Rational> tempRoot;
  tempMat.init(owner.AmbientDimension, (int) this->IndicesNonZeroMults.size);
  for (int i = 0; i < this->IndicesNonZeroMults.size; i ++)
    for (int j = 0; j<owner.AmbientDimension; j ++)
      tempMat.elements[j][i] =(owner.startingVectors[this->IndicesNonZeroMults[i]][j]*this->TheObjects[this->IndicesNonZeroMults[i]].GetLargestElongation());
  if (this->flagAnErrorHasOccurredTimeToPanic) {

  }
  startAsIdMat.MakeIdMatrix(owner.AmbientDimension);
  Matrix<Rational> ::GaussianEliminationByRows(tempMat, startAsIdMat, tempSel, false);
  if (this->flagAnErrorHasOccurredTimeToPanic) {

  }
  SelectionWithDifferentMaxMultiplicities thePowers;
  List<int> thePowersSigned;
  thePowersSigned.SetSize(this->IndicesNonZeroMults.size);
  thePowers.initIncomplete(this->IndicesNonZeroMults.size);
  for (int k = 0; k< this->Coefficient.size; k++) {
    this->Coefficient[k].MonomialExponentToColumnMatrix(matColumn);
    if (this->flagAnErrorHasOccurredTimeToPanic) {

    }
    matColumn.MultiplyOnTheLeft(startAsIdMat);
    if (this->flagAnErrorHasOccurredTimeToPanic) {

    }
    tempFrac.AssignDenominatorOnly(*this);
    tempFrac.Coefficient.MakeZero(this->Coefficient.NumVars);
    tempFrac.Coefficient.AddMonomial(this->Coefficient[k], this->Coefficient.theCoeffs[k]);
    if (tempMat.RowEchelonFormToLinearSystemSolution(tempSel, matColumn, matLinComb)) {
      tempMon = this->Coefficient[k];
      if (this->flagAnErrorHasOccurredTimeToPanic) {

      }
      for (int i = 0; i <matLinComb.NumRows; i ++) {
        thePowers.MaxMultiplicities[i] = 0;
        if (matLinComb.elements[i][0].IsGreaterThanOrEqualTo(1) || matLinComb.elements[i][0].IsNegative()) {
          int tempI = matLinComb.elements[i][0].floorIfSmall();
          thePowersSigned[i] = tempI;
          if (tempI<0)
            thePowers.MaxMultiplicities[i] = this->TheObjects[this->IndicesNonZeroMults[i]].GetMultiplicityLargestElongation();
          else
            thePowers.MaxMultiplicities[i] =MathRoutines::Minimum(tempI, this->TheObjects[this->IndicesNonZeroMults[i]].GetMultiplicityLargestElongation());
          tempRoot = owner.startingVectors[this->IndicesNonZeroMults[i]];
          tempRoot*=(thePowersSigned[i]*this->TheObjects[this->IndicesNonZeroMults[i]].GetLargestElongation());
          tempMon-= tempRoot;
          if (this->flagAnErrorHasOccurredTimeToPanic)
            tempMon.ComputeDebugString();
        } else {
          thePowers.Multiplicities[i] = 0;
          thePowersSigned[i] = 0;
        }
      }
      thePowers.ComputeElements();
      int numSummands = thePowers.TotalNumSubsetsMustBeSmalInt();
      if (numSummands ==1)
        owner.AddAlreadyReduced(tempFrac, Indicator);
      else {
        PartFractions tempFracs;
        Rational tempDiff;
        if (this->flagAnErrorHasOccurredTimeToPanic) {
          tempFracs.initFromOtherPartFractions(owner);
          tempFrac.AssignDenominatorOnly(*this);
          tempFrac.Coefficient.MakeZero(owner.AmbientDimension);
          tempFrac.Coefficient.AddMonomial(this->Coefficient[k], 1);
          tempFrac.ComputeOneCheckSum(owner, tempDiff, owner.AmbientDimension);
        }
        for (int l = 0; l < numSummands; l ++) {
          tempFrac.AssignDenominatorOnly(*this);
          if (this->flagAnErrorHasOccurredTimeToPanic)
            thePowers.ComputeDebugString();
          tempFrac.ReduceMonomialByMonomialModifyOneMonomial
          (owner, thePowers, thePowersSigned, tempMon, this->Coefficient.theCoeffs[k]);
          if (this->flagAnErrorHasOccurredTimeToPanic)
            tempFrac.ComputeDebugString(owner);
          tempFrac.ReduceMonomialByMonomial(owner, - 1, Indicator);
          if (this->flagAnErrorHasOccurredTimeToPanic)
            tempFrac.ReduceMonomialByMonomial(tempFracs, - 1, Indicator);
          thePowers.IncrementSubset();
        }
        if (this->flagAnErrorHasOccurredTimeToPanic) {
          Rational tempFracsCheckSum;
          tempFracs.ComputeOneCheckSum(tempFracsCheckSum);
          tempFracs.ComputeDebugString();
          if (!tempFracsCheckSum.IsEqualTo(tempDiff))
            global.fatal << global.fatal;
        }
      }
    }
    else
      owner.AddAlreadyReduced(tempFrac, Indicator);
  }
  if (this->flagAnErrorHasOccurredTimeToPanic) {
    Rational tempRat;
    owner.ComputeOneCheckSum(tempRat);
    tempRat.Subtract(theDiff);
    if (!tempRat.IsEqualTo(StartCheckSum))
      global.fatal << global.fatal;
  }*/
}

void PartFraction::ReduceMonomialByMonomialModifyOneMonomial(
  PartFractions& Accum,
  SelectionWithDifferentMaxMultiplicities& thePowers,
  List<int>& thePowersSigned,
  MonomialP& input,
  LargeInteger& inputCoeff
) {
  (void) Accum; (void) thePowers; (void) thePowersSigned; (void) input; (void) inputCoeff;
  /*Polynomial<LargeInt>& theNumerator = global.PolyLargeIntPartFracBuffer5.GetElement();
  Polynomial<LargeInt>& tempP = global.PolyLargeIntPartFracBuffer6.GetElement();
  theNumerator.MakeZero(Accum.AmbientDimension);
  theNumerator.AddMonomial(input, inputCoeff);
  if (thePowersSigned.size != thePowers.Multiplicities.size)
    global.fatal << global.fatal;
  if (this->flagAnErrorHasOccurredTimeToPanic) {
   // theNumerator.ComputeDebugString();
  }
  for (int j = 0; j < thePowers.Multiplicities.size; j ++) {
    int currentIndexInFraction = this->IndicesNonZeroMults[j];
    int currentElongation = this->TheObjects[currentIndexInFraction].GetLargestElongation();
    int MultChange = thePowers.Multiplicities[j];
    int MaxMultchange = this->TheObjects[currentIndexInFraction].GetMultiplicityLargestElongation();
    Vector<Rational> tempRoot;
    tempRoot = Accum.startingVectors[currentIndexInFraction];
    tempRoot*=(currentElongation);
    this->GetPolyReduceMonomialByMonomial
    (Accum, tempRoot, thePowersSigned[j], MultChange, MaxMultchange, tempP);
    if (this->flagAnErrorHasOccurredTimeToPanic) {
     // tempP.ComputeDebugString();
    }
    theNumerator*= tempP;
    if (this->flagAnErrorHasOccurredTimeToPanic) {
     // theNumerator.ComputeDebugString();
    }
    this->TheObjects[currentIndexInFraction].AddMultiplicity(-MultChange, currentElongation);
  }
  this->Coefficient = theNumerator;
  this->ComputeIndicesNonZeroMults();
  if (this->flagAnErrorHasOccurredTimeToPanic)
    this->ComputeDebugString(Accum);*/
}

void PartFraction::GetPolyReduceMonomialByMonomial(
  PartFractions& owner,
  Vector<Rational>& theExponent,
  int StartMonomialPower,
  int DenPowerReduction,
  int startDenominatorPower,
  Polynomial<LargeInteger>& output
) {
  if (StartMonomialPower == 0) {
    output.MakeOne(owner.AmbientDimension);
    return;
  }
  MonomialP tempMon;
  tempMon.MakeOne(owner.AmbientDimension);
  output.MakeZero();
  LargeInteger theCoeff = 1;
  if (StartMonomialPower > 0) {
    if (DenPowerReduction != startDenominatorPower) {
      theCoeff = MathRoutines::NChooseK(StartMonomialPower, DenPowerReduction);
      theCoeff *= MathRoutines::parity(DenPowerReduction);
      output.AddMonomial(tempMon, theCoeff);
    } else {
      if (StartMonomialPower < startDenominatorPower) {
        global.fatal << "Start monomial power: " << StartMonomialPower << " smaller than start denominator power: "
        << startDenominatorPower << ". " << global.fatal;
      }
      for (int k = 0; k <= StartMonomialPower - startDenominatorPower; k++) {
        tempMon = theExponent;
        tempMon.RaiseToPower(k);
        theCoeff = MathRoutines::parity(startDenominatorPower) * MathRoutines::NChooseK(
          StartMonomialPower - 1 - k, startDenominatorPower - 1
        );
        output.AddMonomial(tempMon, theCoeff);
      }
    }
  }
  if (StartMonomialPower < 0) {
    if (DenPowerReduction != startDenominatorPower) {
      theCoeff = MathRoutines::NChooseK(- StartMonomialPower - 1 + DenPowerReduction, DenPowerReduction);
      output.AddMonomial(tempMon, theCoeff);
    } else {
      for (int k = 1; k <= - StartMonomialPower; k ++) {
        tempMon = theExponent;
        tempMon.RaiseToPower(- k);
        theCoeff = MathRoutines::NChooseK(startDenominatorPower - StartMonomialPower - 1 - k, startDenominatorPower - 1);
        output.AddMonomial(tempMon, theCoeff);
      }
    }
  }
}

int PartFractions::ToString(std::string& output, bool LatexFormat, FormatExpressions& theFormat) {
  return this->ElementToStringBasisChange(output, LatexFormat, theFormat);
}

int PartFractions::ElementToStringOutputToFile(std::fstream& output, bool LatexFormat) {
  return this->ElementToStringBasisChangeOutputToFile(output, LatexFormat);
}

int PartFractions::ElementToStringBasisChange(
  std::string& output, bool LatexFormat, FormatExpressions& PolyFormatLocal
) {
  std::stringstream out;
  std::string tempS;
  int TotalLines = 0;
  PolyFormatLocal.ExtraLinesCounterLatex = 0;
  if (LatexFormat) {
    out << "\\begin{eqnarray*}\n";
  }
  int LastCutOff = 0;
  for (int i = 0; i < this->size(); i ++) {
    if (this->coefficients[i].size() > 0 ) {
      if (LatexFormat) {
        out << "&&";
      }
      if (tempS[0] != '-') {
        out << "+";
      }
      out << tempS;
      if (LatexFormat) {
        out << "\\\\ \n";
        TotalLines ++;
      } else {
        out << "\n";
      }
      if (LatexFormat && (TotalLines - LastCutOff) > 40) {
        out << "\\end{eqnarray*}\\begin{eqnarray*}\n";
        LastCutOff = TotalLines;
      }
    }
    if (TotalLines>this->flagMaxNumStringOutputLines) {
      out << "\n Number of lines exceeded " << this->flagMaxNumStringOutputLines
      << "; The rest of the output was suppressed.";
      break;
    }
  }
  if (!LatexFormat) {
    output = out.str();
    if (output.size() > 0) {
      if (output[0] == '+') {
        output.erase(0, 1);
      }
    }
  } else {
    out << "\\end{eqnarray*}";
    output = out.str();
  }
  return TotalLines;
}

int PartFractions::ElementToStringBasisChangeOutputToFile(std::fstream& output, bool LatexFormat) {
  std::string tempS;
  int TotalLines = 0;
  FormatExpressions PolyFormatLocal;
  PolyFormatLocal.ExtraLinesCounterLatex = 0;
  if (LatexFormat) {
    output << "\\begin{eqnarray*}\n";
  }
  int LastCutOff = 0;
  for (int i = 0; i < this->size(); i ++) {
    if (this->coefficients[i].size() > 0 ) {
      if (LatexFormat) {
        output << "&&";
      }
      if (tempS[0] != '-') {
        output << "+";
      }
      output << tempS;
      if (LatexFormat) {
        output << "\\\\ \n";
        TotalLines ++;
      } else {
        output << "\n";
      }
      if (LatexFormat && (TotalLines - LastCutOff) > 20) {
        output << "\\end{eqnarray*}\\begin{eqnarray*}\n";
        LastCutOff = TotalLines;
      }
    }
  }
  if (LatexFormat) {
    output << "\\end{eqnarray*}";
  }
  return TotalLines;
}

int PartFraction::ControlLineSizeFracs(std::string& output, FormatExpressions& PolyFormatLocal) {
  int numCutOffs = static_cast<signed>(output.size()) % PolyFormatLocal.MaxLineLength;
  int LastCutOffIndex = 0;
  int NumLinesAdded = 0;
  for (int i = 0; i < numCutOffs; i ++) {
    for (int j = LastCutOffIndex + PolyFormatLocal.MaxLineLength; j < static_cast<signed>(output.size()) - 1; j ++) {
      unsigned k = static_cast<unsigned>(j);
      if (output[k] == '\\' && output[k + 1] == 'f') {
        output.insert(k, "\\\\\n&&");
        NumLinesAdded ++;
        LastCutOffIndex = j + 5;
        break;
      }
    }
  }
  return NumLinesAdded;
}

int PartFraction::ControlLineSizeStringPolys(std::string& output, FormatExpressions& PolyFormatLocal) {
  int numCutOffs = static_cast<int>(static_cast<int>(output.size()) % PolyFormatLocal.MaxLineLength);
  int LastCutOffIndex = 0;
  int NumLinesAdded = 0;
  for (int i = 0; i < numCutOffs; i ++) {
    for (int j = LastCutOffIndex + PolyFormatLocal.MaxLineLength; j < static_cast<int>(output.size()) - 1; j ++) {
      unsigned k = static_cast<unsigned>(j);
      if ((output[k] == '+' || output[k] == '-') && output[k - 1] != '{') {
        output.insert(k, "\\\\\n&&");
        NumLinesAdded ++;
        LastCutOffIndex = j + 5;
        break;
      }
    }
  }
  return NumLinesAdded;
}

void PartFractions::MakeProgressReportSplittingMainPart() {
  if (!global.theResponse.MonitoringAllowed()) {
    return;
  }
  std::stringstream out1, out2, out3;
  out1 << this->NumberRelevantReducedFractions << " relevant reduced + " << this->NumberIrrelevantFractions
  << " disjoint = " << this->NumTotalReduced;
  if (this->NumRelevantNonReducedFractions != 0) {
    out1 << " + " << this->NumRelevantNonReducedFractions << " relevant unreduced ";
  }
  out1 << " out of " << this->size() << " total fractions";
  ProgressReport theReport1;
  ProgressReport theReport2;
  ProgressReport theReport3;
  theReport1.Report(out1.str());
  out2 << this->NumMonomialsInNumeratorsRelevantFractions << " relevant reduced + "
  << this->NumMonomialsInNumeratorsIrrelevantFractions << " disjoint = "
  << this->NumMonomialsInNumeratorsRelevantFractions + this->NumMonomialsInNumeratorsIrrelevantFractions << " out of "
  << this->NumMonomialsInTheNumerators << " total monomials in the numerators";
  theReport2.Report(out2.str());
  if (this->NumGeneratorsInTheNumerators != 0) {
    out3 << this->NumGeneratorsRelevenatFractions << " relevant reduced + "
    << this->NumGeneratorsIrrelevantFractions << " disjoint = "
    << this->NumGeneratorsIrrelevantFractions + this->NumGeneratorsRelevenatFractions
    << " out of " << this->NumGeneratorsInTheNumerators << " total generators in the numerators";
    theReport3.Report(out3.str());
  } else {
    theReport3.Report("");
  }
}

void PartFractions::MakeProgressVPFcomputation() {
  this->NumProcessedForVPFfractions ++;
  if (!global.theResponse.MonitoringAllowed()) {
    return;
  }
  std::stringstream out2;
  ProgressReport theReport;
  out2 << "Processed " << this->NumProcessedForVPFfractions << " out of "
  << this->NumberRelevantReducedFractions << " relevant fractions";
  theReport.Report(out2.str());
}

void PartFractions::ComputeOneCheckSum(Rational& output) {
  output.MakeZero();
  Vector<Rational> CheckSumRoot = oneFracWithMultiplicitiesAndElongations::GetCheckSumRoot(this->AmbientDimension);
  ProgressReport theReport;
  ProgressReport theReport2;
  for (int i = 0; i < this->size(); i ++) {
    Rational currentCheckSum, tempRat;
    (*this)[i].ComputeOneCheckSuM(*this, currentCheckSum, this->AmbientDimension);
    (*this)[i].EvaluateIntPoly(this->coefficients[i], CheckSumRoot, tempRat);
    currentCheckSum *= tempRat;
    output += (tempRat);
    if (this->flagMakingProgressReport) {
      std::stringstream out;
      out << "Checksum " << i + 1 << " out of " << this->size();
      theReport.Report(out.str());
    }
  }
  if (this->flagMakingProgressReport) {
    std::stringstream out;
    out << "Checksum: " << output.ToString();
    theReport2.Report(out.str());
  }
}

void PartFractions::initCommon() {
  this->MakeZero();
  this->startingVectors.Clear();
  this->flagInitialized = false;
  this->SplitStepsCounter = 1;
}

bool PartFractions::initFromRoots(Vectors<Rational>& input) {
  this->initCommon();
  if (input.size < 1) {
    return false;
  }
  PartFraction f;
  this->AmbientDimension = input[0].size;
  bool tempBool = f.initFromRoots(*this, input);
  Polynomial<LargeInteger> tempOne;
  tempOne.MakeOne(this->AmbientDimension);
  this->AddMonomial(f, tempOne);
  return tempBool;
}

void PartFractions::initAndSplit(Vectors<Rational>& input) {
  this->initFromRoots(input);
  this->split(nullptr);
}

void PartFractions::Run(Vectors<Rational>& input) {
  if (!this->flagInitialized) {
    this->initFromRoots(input);
  }
  this->split(nullptr);
}

void PartFractions::RemoveRedundantShortRoots(Vector<Rational>* Indicator) {
  Rational startCheckSum;
  ProgressReport theReport;
  if (PartFraction::MakingConsistencyCheck) {
    this->ComputeOneCheckSum(startCheckSum);
  }
  for (int i = 0; i < this->size(); i ++) {
    if (this->RemoveRedundantShortRootsIndex(i, Indicator)) {
      i --;
      if (this->flagMakingProgressReport) {
        std::stringstream out;
        out << "Elongating denominator " << i + 1 << " out of " << this->size();
        theReport.Report(out.str());
      }
    }
  }
}

void PartFractions::RemoveRedundantShortRootsClassicalRootSystem(Vector<Rational>* Indicator) {
  (void) Indicator;
  /*PartFraction tempFrac;
  Polynomial<LargeInt> buffer;
  for (int i = 0; i < this->size; i ++) {
    tempFrac.Assign(this->TheObjects[i]);
    if (tempFrac.RemoveRedundantShortRootsClassicalRootSystem(*this, Indicator, buffer, this->AmbientDimension)) {
      this->TheObjects[i].Coefficient.MakeZero(this->AmbientDimension);
      this->AddAlreadyReduced(tempFrac, Indicator);
    }
    if (this->flagMakingProgressReport) {
      std::stringstream out;
      out << "Elongating denominator " << i + 1 << " out of " << this->size;
      global.theIndicatorVariables.ProgressReportStrings[3] = out.str();
      global.FeedIndicatorWindow(global.theIndicatorVariables);
    }
  }
  for (int i = 0; i < this->size; i ++)
    if (this->TheObjects[i].Coefficient.IsEqualToZero()) {
      this->PopIndexHashChooseSwapByLowestNonProcessedAndAccount(i, Indicator);
      i --;
    }*/
}

void FileSetPutPointerToEnd(std::fstream& theFile, bool StoreToFile) {
  if (!theFile.is_open() && StoreToFile) {
    global.fatal << "Put pointer seek on closed file not allowed. " << global.fatal;
  }
  std::filebuf* pbuf = theFile.rdbuf();
  long tempSize = pbuf->pubseekoff(0, std::fstream::end);
  theFile.seekp(tempSize);
}

bool PartFractions::VerifyFileComputedContributions() {
  int tempI = this->ReadFromFileComputedContributions(PartFractions::ComputedContributionsList);
  std::filebuf* pbuf = PartFraction::TheBigDump.rdbuf();
  long tempSize = pbuf->pubseekoff(0, std::fstream::end);
  PartFraction::TheBigDump.seekp(tempSize);
  return(tempSize >= tempI);
}

void PartFractions::ComputeDebugString() {
  FormatExpressions tempFormat;
  this->ToString(this->DebugString, tempFormat.flagUseLatex, tempFormat);
}

void PartFractions::ToString(std::string& output, FormatExpressions& theFormat) {
  this->ToString(output, theFormat.flagUseLatex, theFormat);
}

void PartFractions::ComputeDebugStringNoNumerator() {
  FormatExpressions theFormat;
  this->ToString(this->DebugString, theFormat.flagUseLatex, theFormat);
}

void PartFractions::ComputeDebugStringWithVPfunction() {
  FormatExpressions theFormat;
  this->ToString(this->DebugString, theFormat.flagUseLatex, theFormat);
}

void PartFractions::WriteToFile(std::fstream& output) {
  output << "Dimension: ";
  output << this->AmbientDimension << "\n";
  output << "Indices_of_roots:\n";
  FormatExpressions PolyFormatLocal;
  for (int i = 0; i < this->startingVectors.size; i ++) {
    output << "| " << i << "    " << this->startingVectors[i].ToString() << "\n";
  }
  output << "Alphabet_used:\n";
  for (int i = 0; i < this->AmbientDimension; i ++) {
    output << PolyFormatLocal.GetPolyLetter(i) << " ";
  }
  output << "\n" << "Number_of_fractions: " << this->size() << "\n";
  for (int i = 0; i < this->size(); i ++) {
    (*this)[i].WriteToFile(output);
  }
}

void PartFractions::ComputeSupport(List<Vectors<Rational> >& output) {
  output.size = 0;
  output.SetExpectedSize(this->size());
  for (int i = 0; i < this->size(); i ++) {
    Vectors<Rational> tempRoots;
    for (int j = 0; j < (*this)[i].IndicesNonZeroMults.size; j ++) {
      Vector<Rational> tempRoot, tempRoot3;
      tempRoot = (this->startingVectors[(*this)[i].IndicesNonZeroMults[j]]);
      Vector<Rational> tempRoot2;
      tempRoot /= 2;
      tempRoot2 = tempRoot;
      tempRoot3 = tempRoot2;
      if (!(tempRoot3 == tempRoot)) {
        tempRoot *= 2;
      }
      tempRoots.AddOnTopNoRepetition(tempRoot);
    }
    output.AddOnTopNoRepetition(tempRoots);
  }
}

void PartFractions::ComputeDebugStringBasisChange() {
  FormatExpressions tempFormat;
  this->ElementToStringBasisChange(this->DebugString, tempFormat.flagUseLatex, tempFormat);
}

bool PartFractions::IsHigherThanWRTWeight(
  const Vector<Rational>& left, const Vector<Rational>& r, const Vector<Rational>& theWeights
) {
  if (left.size != r.size) {
    global.fatal << "Left and right vectors need equal dimensions. " << global.fatal;
  }
  Rational accum = 0;
  for (int i = 0; i < left.size; i ++) {
    accum += (left[i] - r[i]) * theWeights[i];
  }
  return (accum > 0);
}

//NOTE NOTE NOTE: To be fixed: you gotta use the preceding function to sort the theVPbasis!
void PartFractions::ComputeKostantFunctionFromWeylGroup(
  char WeylGroupLetter, int WeylGroupNumber, QuasiPolynomial& output, Vector<Rational>* ChamberIndicator
) {
  this->initCommon();
  Vectors<Rational> theVPbasis;
  Vector<Rational> tempWeight; tempWeight.SetSize(WeylGroupNumber);
  WeylGroupData tempW;
  tempW.MakeArbitrarySimple(WeylGroupLetter, WeylGroupNumber);
  tempW.ComputeRho(true);
  theVPbasis = tempW.RootsOfBorel;
  if (WeylGroupLetter == 'B') {
    for (int i = 0; i < theVPbasis.size; i ++) {
      Rational tempRat;
      Vector<Rational> tempRoot;
      tempRoot = theVPbasis[i];
      tempW.RootScalarCartanRoot(tempRoot, tempRoot, tempRat);
      if (tempRat.IsEqualToOne()) {
        theVPbasis.AddOnTop(tempW.RootsOfBorel[i] * 2);
      }
    }
  }
  if (WeylGroupLetter == 'D') {
    Vector<Rational> tempRoot;
    tempRoot.MakeZero(this->AmbientDimension);
    tempRoot[this->AmbientDimension - 1] = 1;
    tempRoot[this->AmbientDimension - 2] = - 1;
    theVPbasis.AddOnTop(tempRoot);
    tempRoot[this->AmbientDimension - 1] = 1;
    tempRoot[this->AmbientDimension - 2] = 1;
    theVPbasis.AddOnTop(tempRoot);
    for (int i = this->AmbientDimension - 3; i >= 0; i --) {
      tempRoot[i] = 2;
      global.fatal << "This is a programming error: this line of code "
      << "needs to be fixed but I don't have time right now. "
      << "This code shouldn't be used before the line is fixed! " << global.fatal;
    }
    tempWeight[this->AmbientDimension - 2] = 7;
    tempWeight[this->AmbientDimension - 1] = 8;
  }
  theVPbasis.QuickSortAscending();
  //fix this!
  global.fatal << " Not implemented yet. " << global.fatal;
  //  this->initFromRoots(theVPbasis, 0);
  //this->flagSplitTestModeNoNumerators = true;
  //  this->split(ChamberIndicator);
  if (!this->CheckForMinimalityDecompositionWithRespectToRoot(ChamberIndicator)) {
    global.fatal << "Minimality decomposition missing. " << global.fatal;
  }
  //return;
  Vector<Rational> tempRoot;
  if (ChamberIndicator != nullptr) {
    tempRoot = *ChamberIndicator;
  } else {
    tempRoot.MakeZero(this->AmbientDimension);
  }
  if (!this->GetVectorPartitionFunction(output, tempRoot)) {
    this->ComputeDebugStringNoNumerator();
    global.fatal << "Failed to get vector partition function. " << global.fatal;
  }
}

void oneFracWithMultiplicitiesAndElongations::operator=(oneFracWithMultiplicitiesAndElongations& right) {
  this->Multiplicities.CopyFromLight(right.Multiplicities);
  this->Elongations.CopyFromLight(right.Elongations);
}

unsigned int oneFracWithMultiplicitiesAndElongations::HashFunction() const {
  return static_cast<unsigned>(this->GetTotalMultiplicity());
}

void oneFracWithMultiplicitiesAndElongations::GetPolyDenominator(
  Polynomial<LargeInteger>& output, int MultiplicityIndex, Vector<Rational>& theExponent
) {
  if (MultiplicityIndex >= this->Multiplicities.size) {
    global.fatal << "Bad multiplicity. " << global.fatal;
  }
  MonomialP tempM;
  output.MakeOne(theExponent.size);
  tempM.MakeOne(theExponent.size);
  for (int i = 0; i < theExponent.size; i ++) {
    tempM[i] = theExponent[i] * this->Elongations[MultiplicityIndex];
  }
  output.AddMonomial(tempM, - 1);
}

int oneFracWithMultiplicitiesAndElongations::GetLargestElongation() {
  int result = this->Elongations[0];
  for (int i = 1; i < this->Elongations.size; i ++) {
    if (this->Elongations[i] == result) {
      global.fatal << "Elongation does not math the result. " << global.fatal;
    }
    if (this->Elongations[i] > result) {
      result = this->Elongations[i];
    }
  }
  return result;
}

int oneFracWithMultiplicitiesAndElongations::GetLCMElongations() {
  int result = 1;
  for (int i = 0; i < this->Elongations.size; i ++) {
    if (this->Elongations[i] == 0) {
      global.fatal << "Elongation not allowed to be zero. " << global.fatal;
    }
    result = MathRoutines::lcm(this->Elongations[i], result);
  }
  return result;
}

int oneFracWithMultiplicitiesAndElongations::GetTotalMultiplicity() const {
  int result = 0;
  for (int i = 0; i < this->Elongations.size; i ++) {
    result += this->Multiplicities[i];
  }
  return result;
}

int oneFracWithMultiplicitiesAndElongations::IndexLargestElongation() {
  int result = 0;
  for (int i = 1; i < this->Elongations.size; i ++) {
    if (this->Elongations[i] > this->Elongations[result]) {
      result = i;
    }
  }
  return result;
}

void oneFracWithMultiplicitiesAndElongations::init() {
  this->Elongations.SetSize(0);
  this->Multiplicities.SetSize(0);
}

void oneFracWithMultiplicitiesAndElongations::ComputeOneCheckSum(
  Rational& output, const Vector<Rational>& theExp, int theDimension
) {
  output = 1;
  std::string tempS;
  Vector<Rational> CheckSumRoot = oneFracWithMultiplicitiesAndElongations::GetCheckSumRoot(theDimension);
  for (int i = 0; i < this->Elongations.size; i ++) {
    Rational tempRat, tempRat2, tempRat3;
    tempRat = 1;
    tempRat2 = 1;
    for (int j = 0; j < theDimension; j ++) {
      if (PartFraction::flagAnErrorHasOccurredTimeToPanic) {
        tempS = theExp.ToString();
      }
      tempRat3 = CheckSumRoot[j];
      if (!tempRat3.IsEqualToZero()) {
        tempRat3.RaiseToPower((theExp[j] * this->Elongations[i]).NumShort);
      }
      tempRat2 *= tempRat3;
      if (PartFraction::flagAnErrorHasOccurredTimeToPanic) {
        tempS = tempRat2.ToString();
      }
    }
    if (PartFraction::flagAnErrorHasOccurredTimeToPanic) {
      tempS = tempRat.ToString();
    }
    tempRat -= tempRat2;
    tempRat.RaiseToPower(this->Multiplicities[i]);
    if (PartFraction::flagAnErrorHasOccurredTimeToPanic) {
      tempS = tempRat.ToString();
    }
    output.MultiplyBy(tempRat);
    if (PartFraction::flagAnErrorHasOccurredTimeToPanic) {
      tempS = output.ToString();
    }
  }
  output.Invert();
  if (PartFraction::flagAnErrorHasOccurredTimeToPanic) {
    tempS = output.ToString();
  }
}

int oneFracWithMultiplicitiesAndElongations::GetMultiplicityLargestElongation() {
  int result = 0;
  int LargestElongationFound = 0;
  for (int i = 0; i < this->Elongations.size; i ++) {
    if (LargestElongationFound< this->Elongations[i]) {
      LargestElongationFound = this->Elongations[i];
      result = this->Multiplicities[i];
    }
  }
  return result;
}

void oneFracWithMultiplicitiesAndElongations::AddMultiplicity(int MultiplicityIncrement, int Elongation) {
  if (MultiplicityIncrement == 0) {
    return;
  }
  int ElongationIndex = this->Elongations.IndexInList(Elongation);
  if (ElongationIndex == - 1) {
    this->Elongations.AddObjectOnTopLight(Elongation);
    int tempI = 0;
    this->Multiplicities.AddObjectOnTopLight(tempI);
    ElongationIndex = this->Multiplicities.size - 1;
  }
  this->Multiplicities[ElongationIndex] += MultiplicityIncrement;
  if (!(this->Multiplicities[ElongationIndex] >= 0)) {
    global.fatal << "Multiplicity is supposed to be positive. " << global.fatal;
  }
  if (this->Multiplicities[ElongationIndex] == 0) {
    this->Multiplicities.PopIndexSwapWithLastLight(ElongationIndex);
    this->Elongations.PopIndexSwapWithLastLight(ElongationIndex);
  }
}

void oneFracWithMultiplicitiesAndElongations::OneFracToStringBasisChange(
  PartFractions& owner,
  int indexElongation,
  Matrix<LargeInteger>& VarChange,
  bool UsingVarChange,
  std::string& output,
  bool LatexFormat,
  int indexInFraction,
  int theDimension,
  FormatExpressions& PolyFormatLocal
) {
  std::stringstream out;
  Vector<Rational> tempRoot2, tempRoot;
  tempRoot.SetSize(theDimension);
  tempRoot2.SetSize(theDimension);
  int NumCoords;
  if (UsingVarChange) {
    NumCoords = VarChange.NumRows;
    tempRoot2 = owner.startingVectors[indexInFraction];
    for (int i = 0; i < NumCoords; i ++) {
      tempRoot[i] = 0;
      for (int j = 0; j < theDimension; j ++) {
        tempRoot[i] += tempRoot2[j] * VarChange.elements[i][j];
      }
    }
  } else {
    NumCoords = theDimension;
    tempRoot = owner.startingVectors[indexInFraction];
  }
  tempRoot *= this->Elongations[indexElongation];
  if (!LatexFormat) {
    out << "1/(1-";
  } else {
    out << "\\frac{1}{(1-";
  }
  for (int i = 0; i < NumCoords; i ++) {
    if (tempRoot[i] != 0) {
      out << PolyFormatLocal.GetPolyLetter(i);
      if (tempRoot[i] != 1) {
        out << "^{" << tempRoot[i].ToString() << "}";
      }
    }
  }
  out << ")";
  if (this->Multiplicities[indexElongation] > 1) {
    out << "^" << this->Multiplicities[indexElongation];
  }
  if (LatexFormat) {
    out << "}";
  }
  output = out.str();
}

std::string oneFracWithMultiplicitiesAndElongations::ToString() {
  if (this->Multiplicities.size == 0) {
    return "";
  }
  std::stringstream out;
  std::string tempS;
  for (int k = 0; k < this->Multiplicities.size; k ++) {
    //this->OneFracToStringBasisChange(owner, k, VarChange, UsingVarChange, tempS, LatexFormat, index, theDimension, PolyFormatLocal);
    out << tempS;
  }
  out << " ";
  return out.str();
}

bool oneFracWithMultiplicitiesAndElongations::operator==(oneFracWithMultiplicitiesAndElongations& right) {
  if (this->Elongations.size != right.Elongations.size) {
    return false;
  }
  for (int i = 0; i < this->Elongations.size; i ++) {
    bool Found = false;
    for (int j = 0; j < right.Elongations.size; j ++) {
      if (this->Elongations[i] == right.Elongations[j]) {
        if (this->Multiplicities[i] != right.Multiplicities[j]) {
          return false;
        } else {
          Found = true;
          break;
        }
      }
    }
    if (!Found) {
      return false;
    }
  }
  return true;
}

void PartFractions::initFromRoots(Vectors<Rational>& theAlgorithmBasis, Vector<Rational>* theWeights) {
  if (theAlgorithmBasis.size == 0) {
    return;
  }
  int theDimension = theAlgorithmBasis[0].size;
  if (theWeights != nullptr) {
    this->weights = *theWeights;
  }
  this->startingVectors.Clear();
  for (int i = 0; i < theAlgorithmBasis.size; i ++) {
    this->AddRootAndSort(theAlgorithmBasis[i]);
  }
  if (theWeights != nullptr) {
    this->startingVectors.QuickSortAscending();
  }
  this->NumNonRedundantShortRoots = this->size();
  this->ComputeTable(theDimension);
}

void PartFractions::ComputeTable(int theDimension) {
  Vector<Rational> tempR, tempR2, tempR3;
  tempR.SetSize(theDimension);
  tempR2.SetSize(theDimension);
  tempR3.SetSize(theDimension);
  this->IndicesRedundantShortRoots.init(this->size());
  this->IndicesDoublesOfRedundantShortRoots.SetSize(this->size());
  this->TableAllowedAminus2B.init(this->size(), this->size());
  this->TableAllowedAminusB.init(this->size(), this->size());
  for (int i = 0; i < this->size(); i ++) {
    for (int j = 0; j < this->size(); j ++) {
      for (int k = 0; k < theDimension; k ++) {
        tempR[k] = this->startingVectors[i][k] - this->startingVectors[j][k];
        tempR2[k] = this->startingVectors[i][k] - this->startingVectors[j][k] * 2;
      }
      this->TableAllowedAminusB.elements[i][j] = this->getIndex(tempR);
      this->TableAllowedAminus2B.elements[i][j] = this->getIndex(tempR2);
    }
    tempR3 = this->startingVectors[i];
    tempR3 *= 2;
    this->IndicesDoublesOfRedundantShortRoots[i] = this->getIndex(tempR3);
    if (IndicesDoublesOfRedundantShortRoots[i] != - 1) {
      this->IndicesRedundantShortRoots.AddSelectionAppendNewIndex(i);
    }
  }
}

int PartFractions::AddRootAndSort(Vector<Rational>& theRoot) {
  List<Vector<Rational> > tempList;
  tempList = this->startingVectors;
  int index = 0;
  for (index = 0; index < tempList.size; index ++) {
    if (this->IsHigherThanWRTWeight(theRoot, tempList[index], this->weights)) {
      break;
    }
  }
  tempList.ShiftUpExpandOnTop(index);
  tempList[index] = theRoot;
  this->startingVectors.Clear();
  for (int i = 0; i < tempList.size; i ++) {
    this->startingVectors.AddOnTop(tempList[i]);
  }
  return index;
}

int PartFractions::getIndex(const Vector<Rational>& TheRoot) {
  return this->startingVectors.GetIndex(TheRoot);
}

int PartFractions::getIndexDoubleOfARoot(const Vector<Rational>& TheRoot) {
  return this->getIndex(TheRoot * 2);
}

void SelectionWithDifferentMaxMultiplicities::initPart1(int NumElements) {
  this->Multiplicities.initializeFillInObject(NumElements, 0);
  this->MaxMultiplicities.initializeFillInObject(NumElements,0);
  this->elements.Reserve(NumElements);
  this->elements.size = 0;
}

void SelectionWithMultiplicities::initWithMultiplicities(int NumElements) {
  this->Multiplicities.SetSize(NumElements);
  for (int i = 0; i < this->Multiplicities.size; i ++) {
    this->Multiplicities[i] = 0;
  }
  this->elements.Reserve(NumElements);
  this->elements.size = 0;
}

void SelectionWithMultiplicities::ToString(std::string& output) {
  std::stringstream out;
  for (int i = 0; i < this->elements.size; i ++) {
    out << "Index: " << this->elements[i] << "\nMultiplicity: "
    << this->Multiplicities[this->elements[i]];
  }
  output = out.str();
}

void SelectionWithMaxMultiplicity::initMaxMultiplicity(int NumElements, int MaxMult) {
  this->::SelectionWithMultiplicities::initWithMultiplicities(NumElements);
  this->MaxMultiplicity =MaxMult;
}

int ::SelectionWithMaxMultiplicity::CardinalitySelectionWithMultiplicities() {
  int result = 0;
  for (int i = 0; i < this->Multiplicities.size; i ++) {
    result += this->Multiplicities[i];
  }
  return result;
}

bool SelectionWithMaxMultiplicity::HasMultiplicitiesZeroAndOneOnly() {
  for (int i = 0; i < this->elements.size; i ++) {
    if (this->Multiplicities[elements[i]] > 1) {
      return false;
    }
  }
  return true;
}

void SelectionWithMaxMultiplicity::IncrementSubsetFixedCardinality(int Cardinality) {
  if (Cardinality < 1 || Cardinality > this->MaxMultiplicity * this->Multiplicities.size) {
    return;
  }
  if (this->CardinalitySelectionWithMultiplicities() != Cardinality) {
    this->Multiplicities.initializeFillInObject(this->Multiplicities.size, 0);
  }
  if (this->CardinalitySelectionWithMultiplicities() == 0) {
    for (int i = this->Multiplicities.size - 1; Cardinality > 0; i --) {
      if (Cardinality >= this->MaxMultiplicity) {
        this->Multiplicities[i] = this->MaxMultiplicity;
      } else {
        this->Multiplicities[i] = Cardinality;
      }
      Cardinality -= this->Multiplicities[i];
    }
    this->ComputeElements();
    return;
  }
  int firstNonZeroMult;
  int currentCardinality = Cardinality;
  for (firstNonZeroMult = this->Multiplicities.size - 1; firstNonZeroMult >= 0; firstNonZeroMult --) {
    if (this->Multiplicities[firstNonZeroMult] > 0) {
      break;
    }
  }
  if (firstNonZeroMult == 0) {
    return;
  }
  currentCardinality -= this->Multiplicities[firstNonZeroMult];
  this->Multiplicities[firstNonZeroMult] = 0;
  for (int i = firstNonZeroMult - 1; i >= 0; i --) {
    if (this->Multiplicities[i] < this->MaxMultiplicity) {
      this->Multiplicities[i] ++;
      currentCardinality ++;
      break;
    } else {
      this->Multiplicities[i] = 0;
      currentCardinality -= this->MaxMultiplicity;
    }
  }
  for (int i = this->Multiplicities.size - 1; currentCardinality < Cardinality; i --) {
    if (this->Multiplicities[i] != 0) {
      global.fatal << "Non-zero multiplicities not implemented here. " << global.fatal;
    }
    if (Cardinality - currentCardinality >= this->MaxMultiplicity) {
      this->Multiplicities[i] = this->MaxMultiplicity;
    } else {
      this->Multiplicities[i] = Cardinality - currentCardinality;
    }
    currentCardinality += this->Multiplicities[i];
  }
  this->ComputeElements();
}

int ::SelectionWithMaxMultiplicity::NumCombinationsOfCardinality(int cardinality) {
  //this function needs a complete rewrite;
  return ::MathRoutines::NChooseK(this->Multiplicities.size + cardinality - 1, cardinality);
}

LargeInteger SelectionWithMaxMultiplicity::GetNumTotalCombinations() const {
  //if (this->MaxMultiplicity == 0)
  //  return 1;
  LargeInteger result;
  MathRoutines::KToTheNth(this->MaxMultiplicity + 1, this->Multiplicities.size, result);
  return result;
}

bool SelectionWithMaxMultiplicity::IncrementReturnFalseIfPastLast() {
  this->IncrementSubset();
  return this->elements.size != 0;
}

void SelectionWithMaxMultiplicity::IncrementSubset() {
  for (int i = this->Multiplicities.size - 1; i >= 0; i --) {
    if (this->Multiplicities[i] < this->MaxMultiplicity) {
      if (this->Multiplicities[i] == 0) {
        this->elements.AddOnTop(i);
      }
      this->Multiplicities[i] ++;
      return;
    } else {
      this->Multiplicities[i] = 0;
      this->elements.RemoveFirstOccurenceSwapWithLast(i);
    }
  }
}

void SelectionWithMultiplicities::ComputeElements() {
  this->elements.size = 0;
  for (int i = 0; i < this->Multiplicities.size; i ++) {
    if (this->Multiplicities[i] > 0) {
      this->elements.AddOnTop(i);
    }
  }
}

int SelectionWithMultiplicities::CardinalitySelectionWithoutMultiplicities() {
  return this->elements.size;
}

int SelectionWithDifferentMaxMultiplicities::TotalNumSubsetsMustBeSmalInt() {
  int result = 1;
  for (int i = 0; i < this->MaxMultiplicities.size; i ++) {
    result *= (this->MaxMultiplicities[i] + 1);
    if (result < 0) {
      global.fatal << "This is a programming error: I was asked to enumerate "
      << "all subsets of a multi-set, however the number of subsets is larger than  "
      << " the maximum value allowed for int on the system "
      << "(on a 32 bit machine that is around  2 billion). "
      << "This can be fixed, however I do not have time at the moment. If you "
      << " encounter this error, write me an email and I will take the time to fix this issue. "
      << global.fatal;
    }
  }
  return result;
}

LargeInteger SelectionWithDifferentMaxMultiplicities::TotalNumSubsets() {
  LargeInteger result = 1;
  for (int i = 0; i < this->MaxMultiplicities.size; i ++) {
    result *= (this->MaxMultiplicities[i] + 1);
  }
  return result;
}

void SelectionWithDifferentMaxMultiplicities::initFromInts(int* theMaxMults, int NumberMaxMults) {
  this->Multiplicities.initializeFillInObject(NumberMaxMults, 0);
  this->MaxMultiplicities.SetSize(NumberMaxMults);
  for (int i = 0; i < this->MaxMultiplicities.size; i ++) {
    this->MaxMultiplicities[i] = theMaxMults[i];
  }
  this->elements.initializeFillInObject(NumberMaxMults, 0);
}

void SelectionWithDifferentMaxMultiplicities::initFromInts(const List<int>& theMaxMults) {
  this->Multiplicities.initializeFillInObject(theMaxMults.size, 0);
  this->elements.initializeFillInObject(theMaxMults.size, 0);
  this->MaxMultiplicities = theMaxMults;
}

int ::SelectionWithDifferentMaxMultiplicities::TotalMultiplicity() {
  int result = 0;
  for (int i = 0; i < this->Multiplicities.size; i ++) {
    result += this->Multiplicities[i];
  }
  return result;
}

int ::SelectionWithDifferentMaxMultiplicities::MaxTotalMultiplicity() {
  int result = 0;
  for (int i = 0; i < this->Multiplicities.size; i ++) {
    result += this->MaxMultiplicities[i];
  }
  return result;
}

void ::SelectionWithDifferentMaxMultiplicities::clearNoMaxMultiplicitiesChange() {
  for (int i = 0; i < this->Multiplicities.size; i ++) {
    this->Multiplicities[i] = 0;
  }
}

bool SelectionWithDifferentMaxMultiplicities::IncrementReturnFalseIfPastLast() {
  MacroRegisterFunctionWithName("SelectionWithDifferentMaxMultiplicities::IncrementReturnFalseIfPastLast");
  for (int i = this->Multiplicities.size - 1; i >= 0; i --) {
    if (this->Multiplicities[i] < this->MaxMultiplicities[i]) {
      if (this->Multiplicities[i] == 0) {
        this->elements.AddOnTop(i);
      }
      this->Multiplicities[i] ++;
      return true;
    } else {
      this->Multiplicities[i] = 0;
      this->elements.RemoveFirstOccurenceSwapWithLast(i);
    }
  }
  return false;
}

void DynkinType::GetTypesWithMults(List<DynkinSimpleType>& output) const {
  output.SetSize(0);
  output.Reserve(this->GetNumSimpleComponents());
  List<DynkinSimpleType> componentsSorted;
  this->GetSortedDynkinTypes(componentsSorted);
  for (int i = 0; i < componentsSorted.size; i ++) {
    int theIndex = this->theMonomials.GetIndex(componentsSorted[i]);
    for (int j = 0; j < this->GetMult(theIndex); j ++) {
      output.AddOnTop(componentsSorted[i]);
    }
  }
}

void DynkinType::GetOuterAutosGeneratorsOneTypeActOnVectorColumn(
  List<MatrixTensor<Rational> >& output, const DynkinSimpleType& theType, int multiplicity
) {
  MacroRegisterFunctionWithName("DynkinType::GetOuterAutosGeneratorsOneTypeActOnVectorColumn");
  output.SetSize(0);
  MatrixTensor<Rational> directSummand, finalMat;
  if (
    theType.theLetter == 'D' ||
    (theType.theLetter == 'A' && theType.theRank > 1) ||
    (theType.theLetter == 'E' && theType.theRank == 6)
  ) {
    directSummand.MakeId(theType.theRank * (multiplicity - 1));
    int numGens = 1;
    if (theType.theLetter == 'D' && theType.theRank == 4) {
      numGens = 2;
    }
    for (int i = 1; i < numGens + 1; i ++) {
      theType.GetAutomorphismActingOnVectorColumn(finalMat, i);
      finalMat.DirectSumWith(directSummand);
      output.AddOnTop(finalMat);
    }
  }
  if (multiplicity < 2) {
    return;
  }
  for (int i = 0; i < multiplicity - 1; i ++) {
    directSummand.MakeZero();
    for (int j = 0; j < theType.theRank; j ++) {
      directSummand.AddMonomial(MonomialMatrix(j, theType.theRank + j), 1);
      directSummand.AddMonomial(MonomialMatrix(theType.theRank + j, j), 1);
    }
    finalMat.MakeId(i * theType.theRank);
    finalMat.DirectSumWith(directSummand);
    directSummand.MakeId((multiplicity - 2 - i) * theType.theRank);
    finalMat.DirectSumWith(directSummand);
    output.AddOnTop(finalMat);
  }
}

void DynkinType::GetOuterAutosGeneratorsActOnVectorColumn(List<MatrixTensor<Rational> >& output) {
  MacroRegisterFunctionWithName("DynkinType::GetOuterAutosGenerators");
  this->SortTheDynkinTypes();
  List<MatrixTensor<Rational> > intermediateGenerators;
  MatrixTensor<Rational> matrixFinal, matrixToGo;
  int currentMult = 0;
  output.SetSize(0);
  int numRowsSoFar = 0;
  for (int i = 0; i < this->size(); i ++) {
    if (!this->coefficients[i].IsSmallInteger(&currentMult)) {
      global.fatal << "This is not supposed to happen in function "
      << "DynkinType::GetOuterAutosGeneratorsActOnVectorColumn." << global.fatal;
    }
    this->GetOuterAutosGeneratorsOneTypeActOnVectorColumn(intermediateGenerators,(*this)[i], currentMult);
    matrixToGo.MakeId(this->GetRank() - numRowsSoFar - currentMult * (*this)[i].theRank);
    for (int j = 0; j < intermediateGenerators.size; j ++) {
      matrixFinal.MakeId(numRowsSoFar);
      matrixFinal.DirectSumWith(intermediateGenerators[j]);
      matrixFinal.DirectSumWith(matrixToGo);
      output.AddOnTop(matrixFinal);
    }
    numRowsSoFar += currentMult * (*this)[i].theRank;
  }
  if (output.size == 0) {
    output.SetSize(1);
    output[0].MakeId(this->GetRank());
  }
}

void DynkinType::GetLettersTypesMults(
  List<char>* outputLetters,
  List<int>* outputRanks,
  List<int>* outputMults,
  List<Rational>* outputFirstCoRootLengthsSquared
) const {
  if (outputLetters != nullptr) {
    outputLetters->SetSize(0);
  }
  if (outputRanks != nullptr) {
    outputRanks->SetSize(0);
  }
  if (outputMults != nullptr) {
    outputMults->SetSize(0);
  }
  if (outputFirstCoRootLengthsSquared != nullptr) {
    outputFirstCoRootLengthsSquared->SetSize(0);
  }
  List<DynkinSimpleType> componentsSorted;
  this->GetSortedDynkinTypes(componentsSorted);
  for (int i = 0; i < componentsSorted.size; i ++) {
    int theIndex = this->theMonomials.GetIndex(componentsSorted[i]);
    if (outputLetters != nullptr) {
      outputLetters->AddOnTop((*this)[theIndex].theLetter);
    }
    if (outputRanks != nullptr) {
      outputRanks->AddOnTop((*this)[theIndex].theRank);
    }
    if (outputFirstCoRootLengthsSquared != nullptr) {
      outputFirstCoRootLengthsSquared->AddOnTop((*this)[theIndex].CartanSymmetricInverseScale);
    }
    if (outputMults != nullptr) {
      outputMults->AddOnTop(this->GetMult(theIndex));
    }
  }
}


std::string DynkinType::GetWeylGroupName(FormatExpressions* theFormat) const {
  return this->ToString(theFormat);
}

bool DynkinType::CanBeExtendedParabolicallyOrIsEqualTo(const DynkinType& other) const {
  MacroRegisterFunctionWithName("DynkinType::CanBeExtendedParabolicallyOrIsEqualTo");
  if (this->IsEqualToZero()) {
    return true;
  }
  if (other.IsEqualToZero()) {
    return false;
  }
  const DynkinSimpleType& currentSimpleType = (*this)[0];
  DynkinType remainderThis, remainderOther;
  for (int i = 0; i < other.size(); i ++) {
    if (!currentSimpleType.CanBeExtendedParabolicallyOrIsEqualTo(other[i])) {
      continue;
    }
    remainderThis = *this;
    remainderThis -= currentSimpleType;
    remainderOther = other;
    remainderOther -= other[i];
    if (remainderThis.CanBeExtendedParabolicallyOrIsEqualTo(remainderOther)) {
      return true;
    }
  }
  return false;
}

bool DynkinType::CanBeExtendedParabolicallyTo(const DynkinType& other) const {
  MacroRegisterFunctionWithName("DynkinType::CanBeExtendedParabolicallyTo");
  if (*this == other) {
    return false;
  }
  return this->CanBeExtendedParabolicallyOrIsEqualTo(other);
}

bool DynkinType::Grow(
  const List<Rational>& allowedInverseScales,
  int AmbientWeylDim,
  List<DynkinType>& output, List<List<int> >* outputPermutationRoots
) const {
  MacroRegisterFunctionWithName("DynkinType::Grow");
  output.SetSize(0);
  if (outputPermutationRoots != nullptr) {
    outputPermutationRoots->SetSize(0);
  }
  if (this->GetRank() >= AmbientWeylDim) {
    return true;
  }
  if (this->IsEqualToZero()) {
    output.SetSize(allowedInverseScales.size);
    if (outputPermutationRoots != nullptr) {
      outputPermutationRoots->SetSize(allowedInverseScales.size);
    }
    for (int i = 0; i < allowedInverseScales.size; i ++) {
      output[i].MakeSimpleType('A', 1, &allowedInverseScales[i]);
      if (outputPermutationRoots != nullptr) {
        (*outputPermutationRoots)[i].SetSize(1);
        (*outputPermutationRoots)[i][0] = 0;
      }
    }
    return true;
  }
  //  Rational minCoRootLengthSquared = - 1;
  //growth is allowed from the minimal component only
  DynkinSimpleType minComponent;
  Rational coeffMinCompo;
  List<int> currentRootInjection;
  this->GetMinMonomial(minComponent, coeffMinCompo);
  if (coeffMinCompo == 1) {
    DynkinType typeMinusMin = (*this);
    typeMinusMin.SubtractMonomial(minComponent, 1);
    List<DynkinSimpleType> theSimpleTypes;
    List<List<int> > lastComponentRootInjections;
    minComponent.Grow(theSimpleTypes, &lastComponentRootInjections);
    currentRootInjection.SetSize(this->GetRank() + 1);
    for (int i = 0; i < theSimpleTypes.size; i ++) {
      bool isGood = true;
      for (int j = 0; j < typeMinusMin.size(); j ++) {
        if (theSimpleTypes[i] > typeMinusMin[j]) {
          isGood = false;
          break;
        }
      }
      if (!isGood) {
        continue;
      }
      output.AddOnTop(typeMinusMin);
      output.LastObject()->AddMonomial(theSimpleTypes[i], 1);
      if (outputPermutationRoots != nullptr) {
        int baseTypeRank = typeMinusMin.GetRank();
        for (int j = 0; j < baseTypeRank; j ++) {
          currentRootInjection[j] = j;
        }
        for (int j = 0; j < lastComponentRootInjections[i].size; j ++) {
          currentRootInjection[j + baseTypeRank] = lastComponentRootInjections[i][j] + baseTypeRank;
        }
        outputPermutationRoots->AddOnTop(currentRootInjection);
      }
    }
  }
  DynkinSimpleType currentA1;
  for (int i = 0; i < allowedInverseScales.size; i ++) {
    currentA1.MakeArbitrary('A', 1, allowedInverseScales[i]);
    if (minComponent < currentA1) {
      continue;
    }
    output.AddOnTop(*this);
    *output.LastObject() += currentA1;
    currentRootInjection.SetSize(output.LastObject()->GetRank());
    if (outputPermutationRoots != nullptr) {
      for (int j = 0; j < currentRootInjection.size; j ++) {
        currentRootInjection[j] = j;
      }
      outputPermutationRoots->AddOnTop(currentRootInjection);
    }
  }
  return true;
}

int DynkinType::GetIndexPreimageFromRootInjection(int inputIndex, const List<int>& inputRootInjection) {
  MacroRegisterFunctionWithName("DynkinType::GetIndexPreimageFromRootInjection");
  for (int i = 0; i < inputRootInjection.size; i ++) {
    if (inputRootInjection[i] == inputIndex) {
      return i;
    }
  }
  global.fatal << "This is a programming error: asking to find the preimage of root index "
  << inputIndex << " w.r.t. root injection "
  << inputRootInjection << " - the root index has no preimage. This function is not allowed to fail. ";
  global.fatal << global.fatal;
  return - 1;
}

void DynkinType::MakeSimpleType(char type, int rank, const Rational* inputFirstCoRootSqLength) {
  DynkinSimpleType theMon;
  Rational cartanSymmetricInvScale = (inputFirstCoRootSqLength == nullptr ? 1 : *inputFirstCoRootSqLength);
  theMon.MakeArbitrary(type, rank, cartanSymmetricInvScale);
  this->MakeZero();
  this->AddMonomial(theMon, 1);
}

bool DynkinType::HasExceptionalComponent() const {
  for (int i = 0; i < this->size(); i ++) {
    if ((*this)[i].theLetter == 'E' || (*this)[i].theLetter == 'F' || (*this)[i].theLetter == 'G') {
      return true;
    }
  }
  return false;
}

bool DynkinType::ContainsType(char theTypeLetter) const {
  for (int i = 0; i < this->size(); i ++) {
    if ((*this)[i].theLetter == theTypeLetter) {
      return true;
    }
  }
  return false;
}

bool DynkinType::IsSimple(char* outputtype, int* outputRank, Rational* outputLength) const {
  if (this->size() != 1) {
    return false;
  }
  if (this->coefficients[0] != 1) {
    return false;
  }
  const DynkinSimpleType& theMon = (*this)[0];
  if (outputtype != nullptr) {
    *outputtype = theMon.theLetter;
  }
  if (outputRank != nullptr) {
    *outputRank = theMon.theRank;
  }
  if (outputLength != nullptr) {
    *outputLength = theMon.CartanSymmetricInverseScale;
  }
  return true;
}

int DynkinType::GetNumSimpleComponentsOfGivenRank(int desiredRank) const {
  Rational result = 0;
  for (int i = 0; i < this->size(); i ++) {
    if ((*this)[i].theRank == desiredRank) {
      result += this->coefficients[i];
    }
  }
  int output = 0;
  if (!result.IsSmallInteger(&output)) {
    global.fatal << "This is a programming error: "
    << "Dynkin type has a number of simple components which is not a small integer. " << global.fatal;
  }
  return output;
}

int DynkinType::GetNumSimpleComponents() const {
  Rational result = 0;
  for (int i = 0; i < this->size(); i ++) {
    result += this->coefficients[i];
  }
  int output = 0;
  if (!result.IsSmallInteger(&output)) {
    global.fatal << "This is a programming error: "
    << "Dynkin type has a number of simple components which is not a small integer. " << global.fatal;
  }
  return output;
}

Rational DynkinType::GetRankRational() const {
  Rational result = 0;
  for (int i = 0; i < this->size(); i ++) {
    result += this->coefficients[i] * (*this)[i].theRank;
  }
  return result;
}

int DynkinType::GetRank() const {
  Rational tempRat = this->GetRankRational();
  int result = 0;
  if (!tempRat.IsSmallInteger(&result)) {
    global.fatal << "This is a programming error: attempting to get a small integer "
    << "rank from a Dynkin type whose rank is not a small integer, but is instead "
    << tempRat.ToString() << ". " << global.fatal;
  }
  return result;
}

void DynkinType::GetEpsilonMatrix(Matrix<Rational>& output) const {
  output.init(0,0);
  Matrix<Rational> curCartan;
  List<DynkinSimpleType> sortedMons;
  this->GetSortedDynkinTypes(sortedMons);
  for (int j = 0; j < sortedMons.size; j ++) {
    int theIndex = this->theMonomials.GetIndex(sortedMons[j]);
    int theMult = this->GetMult(theIndex);
    for (int k = 0; k < theMult; k ++) {
      DynkinSimpleType::GetEpsilonMatrix((*this)[theIndex].theLetter, (*this)[theIndex].theRank, curCartan);
      output.DirectSumWith(curCartan);
    }
  }
}

void DynkinType::SortTheDynkinTypes() {
  List<DynkinSimpleType> sortedTypes;
  this->GetSortedDynkinTypes(sortedTypes);
  DynkinType finalMe;
  finalMe.MakeZero();
  for (int i = 0; i < sortedTypes.size; i ++) {
    finalMe.AddMonomial(sortedTypes[i], this->GetMonomialCoefficient(sortedTypes[i]));
  }
  *this = finalMe;
}

void DynkinType::GetSortedDynkinTypes(List<DynkinSimpleType>& output) const {
  output = this->theMonomials;
  output.QuickSortDescending();
}

void DynkinType::GetCartanSymmetric(Matrix<Rational>& output) const {
  MacroRegisterFunctionWithName("DynkinType::GetCartanSymmetric");
  output.init(0, 0);
  Matrix<Rational> curCartan;
  List<DynkinSimpleType> sortedMons;
  this->GetSortedDynkinTypes(sortedMons);
  for (int j = 0; j < sortedMons.size; j ++) {
    int theIndex = this->theMonomials.GetIndex(sortedMons[j]);
    int mult = this->GetMult(theIndex);
    for (int k = 0; k < mult; k ++) {
      (*this)[theIndex].GetCartanSymmetric(curCartan);
      output.DirectSumWith(curCartan);
    }
  }
}

void DynkinType::GetCoCartanSymmetric(Matrix<Rational>& output) const {
  MacroRegisterFunctionWithName("DynkinType::GetCartanSymmetric");
  Matrix<Rational> curCartan;
  this->GetCartanSymmetric(curCartan);
  WeylGroupData::GetCoCartanSymmetric(curCartan, output);
}

void DynkinType::GetCartanSymmetricDefaultLengthKeepComponentOrder(Matrix<Rational>& output) const {
  MacroRegisterFunctionWithName("DynkinType::GetCartanSymmetricDefaultLengthKeepComponentOrder");
  output.init(0, 0);
  Matrix<Rational> curCartan;
  List<DynkinSimpleType> sortedMons;
  this->GetSortedDynkinTypes(sortedMons);
  DynkinSimpleType currentType;
  for (int j = 0; j < sortedMons.size; j ++) {
    int theIndex = this->theMonomials.GetIndex(sortedMons[j]);
    int mult = this->GetMult(theIndex);
    currentType.MakeArbitrary(sortedMons[j].theLetter, sortedMons[j].theRank, 1);
    currentType.CartanSymmetricInverseScale = 1;//= currentType.GetDefaultCoRootLengthSquared(0);
    for (int k = 0; k < mult; k ++) {
      currentType.GetCartanSymmetric(curCartan);
      output.DirectSumWith(curCartan);
    }
  }
}

int DynkinType::GetCoxeterEdgeWeight(int v, int w) {
  if (v == w) {
    return 0;
  }
  Matrix<Rational> M;
  this->GetCartanSymmetric(M);
  if (M(v, w) == 0) {
    return 2;
  }
  Rational c2 = M(v, w) * M(v, w) / M(v, v) / M(w,w);
  c2 = 3 / c2;
  if (c2 == 12) {
    return 3;
  } else {
    if (c2 == 6) {
      return 4;
    } else {
      if (c2 == 4) {
        return 6;
      }
    }
  }
  global.fatal << "If you would like an edge weight of a non-crystallographic Coxeter graph, replace the code near "
  << __FILE__ << ":" << __LINE__ << " with an arccos function. " << global.fatal;
  return - 1;
}

LargeInteger DynkinType::GetWeylGroupSizeByFormula() const {
  MacroRegisterFunctionWithName("DynkinType::GetWeylGroupSizeByFormula");
  this->CheckFlagDeallocated();
  LargeInteger result = 1;
  LargeInteger tempLI;
  for (int i = 0; i < this->size(); i ++) {
    tempLI = WeylGroupData::SizeByFormulaOrNeg1((*this)[i].theLetter, (*this)[i].theRank);
    tempLI.RaiseToPower(this->GetMult(i));
    result *= tempLI;
  }
  if (result <= 0) {
    global.fatal << "Something has gone very wrong: Weyl group size reported to be " << result.ToString()
    << " which appears to not be a positive integer! " << global.fatal;
  }
  return result;
}

Rational DynkinSimpleType::GetLongRootLengthSquared() const {
  return this->GetDefaultLongRootLengthSquared() / this->CartanSymmetricInverseScale;
}

Rational DynkinSimpleType::GetDefaultLongRootLengthSquared() const {
  //Dynkin's convention says that the default long root length is 2.
  return 2;
}

Rational DynkinSimpleType::GetEpsilonRealizationLongRootLengthSquared() const {
  //Dynkin's convention says that the default long root length is 2.
  //However, the accepted epsilon coordinate realizations of the root systems
  //of G_2 and C_n do not have long root length of 2.
  switch (this->theLetter) {
    case 'A':
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
  global.fatal << "This is a programming error: calling "
  << "DynkinSimpleType::GetLongRootLengthSquared on a non-initialized simple type. " << global.fatal;
  return - 1;
}

std::string DynkinSimpleType::ToStringNonTechnicalName(FormatExpressions* theFormat) const {
  (void) theFormat;
  std::stringstream out;
  switch (this->theLetter) {
    case 'A':
      out << "sl(" << this->theRank + 1 << ")";
      break;
    case 'B':
      out << "so(" << 2 * this->theRank + 1 << ")";
      break;
    case 'C':
      out << "sp(" << 2 * this->theRank << ")";
      break;
    case 'D':
      out << "so(" << 2 * this->theRank << ")";
      break;
    default :
      break;
  }
  return out.str();
}

std::string DynkinSimpleType::ToString(FormatExpressions* theFormat) const {
  std::stringstream out;
  bool includeTechnicalNames = theFormat == nullptr ? true : theFormat->flagIncludeLieAlgebraTypes;
  bool usePlusesAndExponents = theFormat == nullptr ? true: !theFormat->flagDynkinTypeDontUsePlusAndExponent;
  bool supressDynkinIndexOne = theFormat == nullptr ? false : theFormat->flagSupressDynkinIndexOne;
  bool hasAmbient = false;
  if (theFormat != nullptr) {
    hasAmbient = (theFormat->AmbientWeylLetter != 'X');
  }
  if (includeTechnicalNames) {
    if (!hasAmbient) {
      out << theLetter;
      if (!supressDynkinIndexOne || this->CartanSymmetricInverseScale != 1) {
        if (usePlusesAndExponents) {
          out << "^{";
        }
        out << this->CartanSymmetricInverseScale.ToString();
        if (usePlusesAndExponents) {
          out << "}";
        }
      }
    } else {
      DynkinSimpleType ambientType;
      ambientType.theLetter = theFormat->AmbientWeylLetter;
      ambientType.CartanSymmetricInverseScale = theFormat->AmbientCartanSymmetricInverseScale;
      Rational theDynkinIndex = ambientType.GetLongRootLengthSquared() / this->GetLongRootLengthSquared();
//      (this->CartanSymmetricInverseScale/this->GetDefaultLongRootLengthSquared())/
//      (ambientType.CartanSymmetricInverseScale/ambientType.GetDefaultLongRootLengthSquared());
      out << theLetter;
      if (!supressDynkinIndexOne || theDynkinIndex != 1) {
        if (usePlusesAndExponents) {
          out << "^{" ;
        }
        out << theDynkinIndex.ToString();
        if (usePlusesAndExponents) {
          out << "}";
        }
      }
    }
    if (this->theRank >= 10) {
      out << "_";
      if (usePlusesAndExponents) {
        out << "{";
      }
      out << this->theRank;
      if (usePlusesAndExponents) {
        out << "}";
      }
    } else {
      out << "_" << this->theRank;
    }
    //out << "[" << this->theLetter << "^{" << this->CartanSymmetricInverseScale << "}_" << this->theRank << "]";
  }
  return out.str();
}

void DynkinSimpleType::MakeArbitrary(
  char inputLetter,
  int inputRank,
  const Rational& inputLengthFirstCorRootSquared
) {
  if ((
      inputLetter != 'A' && inputLetter != 'B' && inputLetter != 'C' && inputLetter != 'D' &&
      inputLetter != 'E' && inputLetter != 'F' && inputLetter != 'G'
    ) || inputRank <= 0
  ) {
    global.fatal << "This is a programming error. "
    << "Requested to create a simple Dynkin type of type "
    << inputLetter << " and rank "
    << inputRank << ". This is not allowed: I only accept types "
    << "A, B, C, D, E, F and G and non-negative ranks. "
    << global.fatal;
  }
  if (inputLetter == 'G') {
    inputRank = 2;
  }
  if (inputLetter == 'F') {
    inputRank = 4;
  }
  if (inputLetter == 'E' && inputRank > 8) {
    inputRank = 8;
  }
  if (inputRank == 1) {
    inputLetter = 'A';
  }
  this->theRank = inputRank;
  this->theLetter = inputLetter;
  this->CartanSymmetricInverseScale = inputLengthFirstCorRootSquared;
}

int DynkinSimpleType::GetRootSystemSize() const {
  switch (this->theLetter) {
    case 'A':
      return this->theRank * (this->theRank + 1);
    case 'B':
    case 'C':
      return this->theRank * this->theRank * 2;
    case 'D':
      return this->theRank * (this->theRank - 1) * 2;
    case 'E':
      switch(this->theRank) {
        case 6: return 72;
        case 7: return 126;
        case 8: return 240;
        default: return - 1;
      }
    case 'G':
      return 12;
    case 'F':
      return 48;
    default:
      return - 1;
  }
}

Rational DynkinSimpleType::GetDefaultCoRootLengthSquared(int rootIndex) const {
  Rational result = this->GetDefaultRootLengthSquared(rootIndex);
  result.Invert();
  result *= 4;
  return result;
}

Rational DynkinSimpleType::GetRatioRootSquaredToFirstSquared(int rootIndex) const {
  Rational result;
  switch (this->theLetter) {
    case 'A':
    case 'D':
    case 'E':
      return 1;
    case 'B':
      if (rootIndex == this->theRank - 1) {
        result.AssignNumeratorAndDenominator(1, 2);
        return result;
      }
      return 1;
    case 'C':
      if (rootIndex == this->theRank - 1) {
        return 2;
      }
      return 1;
    case 'F':
      if (rootIndex < 2) {
        return 1;
      }
      result.AssignNumeratorAndDenominator(1, 2);
      return result;
    case 'G':
      if (rootIndex == 1) {
        return 3;
      }
      return 1;
    default:
      return - 1;
  }
}

Rational DynkinSimpleType::GetDefaultRootLengthSquared(int rootIndex) const {
  if (rootIndex >= this->theRank) {
    global.fatal << "This is a programming error: "
    << "attempting to get the squared length of simple root number " << rootIndex + 1
    << ", however the root system if of rank " << this->theRank << ". " << global.fatal;
  }
  switch (this->theLetter) {
    case 'A':
    case 'D':
    case 'E':
      return 2;
    case 'B':
      if (rootIndex == this->theRank - 1) {
        return 1;
      }
      return 2;
    case 'F':
      if (rootIndex < 2) {
        return 2;
      }
      return 1;
    case 'C':
      if (rootIndex == this->theRank - 1) {
        return 2;
      }
      return 1;
    case 'G':
      if (rootIndex == 1) {
        return 2;
      }
      return Rational(2, 3);
    default:
      global.fatal << "This is a programming error: calling "
      << "DynkinSimpleType::GetDefaultRootLengthSquared on the non-initialized Dynkin type "
      << this->ToString() << global.fatal;
      return - 1;
  }
}

void DynkinSimpleType::GetEpsilonMatrix(char WeylLetter, int WeylRank, Matrix<Rational>& output) {
  if (WeylLetter == 'A') {
    output.init(WeylRank + 1, WeylRank);
    output.MakeZero();
    for (int i = 0; i < WeylRank; i ++) {
      output(i, i) = 1;
      output(i + 1, i) = - 1;
    }
    output(WeylRank, WeylRank - 1) = - 1;
  }
  if (WeylLetter == 'B') {
    output.init(WeylRank, WeylRank);
    output.MakeZero();
    for (int i = 0; i < WeylRank - 1; i ++) {
      output(i, i) = 1;
      output(i + 1, i) = - 1;
    }
    output(WeylRank - 1, WeylRank - 1) = 1;
  }
  if (WeylLetter == 'C') {
    output.init(WeylRank, WeylRank);
    output.MakeZero();
    for (int i = 0; i < WeylRank - 1; i ++) {
      output(i, i) = 1;
      output(i + 1, i) = - 1;
    }
    output(WeylRank - 1, WeylRank - 1) = 2;
  }
  if (WeylLetter == 'D') {
    output.init(WeylRank, WeylRank);
    output.MakeZero();
    for (int i = 0; i < WeylRank - 1; i ++) {
      output(i, i) = 1;
      output(i + 1, i) = - 1;
    }
    output(WeylRank - 1, WeylRank - 1) = 1;
    output(WeylRank - 2, WeylRank - 1) = 1;
  }
  Rational RHalf(1, 2);
  Rational RMHalf(- 1, 2);
  if (WeylLetter == 'E') {
    //Epsilon convention taken with slight modification from
    //Humpreys, Introduction to Lie algebras and representation theory, page 65
    //first comes first root, then the sticky part, then string with the rest of the roots.
    output.init(8, WeylRank);
    output.MakeZero();
    //first simple root: - 1/2e_1- 1/2e_8+ 1/2e_2+ 1/2e_3+ 1/2e_4+ 1/2e_5+ 1/2e_6+ 1/2e_7
    output(0, 0) = RMHalf;
    output(1, 0) = RHalf;
    output(2, 0) = RHalf;
    output(3, 0) = RHalf;
    output(4, 0) = RHalf;
    output(5, 0) = RHalf;
    output(6, 0) = RHalf;
    output(7, 0) = RMHalf;
    //2nd simple root: -e_1-e_2 (that is the sticky piece of the Dynkin diagram)
    output(0, 1) = - 1;
    output(1, 1) = - 1;
    //3rd simple root: e_1-e_2
    output(0, 2) = 1;
    output(1, 2) = - 1;
    //4th simple root: e_2-e_3
    output(1, 3) = 1;
    output(2, 3) = - 1;
    //5th simple root: e_3-e_4
    output(2, 4) = 1;
    output(3, 4) = - 1;
    //6th simple root: e_4-e_5
    output(3, 5) = 1;
    output(4, 5) = - 1;
    if (WeylRank > 6) {
     //7th simple root: e_5-e_6
      output(4, 6) = 1;
      output(5, 6) = - 1;
    }
    if (WeylRank > 7) {
     //8th simple root: e_6-e_7
      output(5, 7) = 1;
      output(6, 7) = - 1;
    }
  }
  if (WeylLetter == 'F') {
    //as of May 11 2013 the convention has been changed to coincide with that of
    //Wikipedia
    output.init(4, 4);
    output.MakeZero();

    //image of first simple root = e_1 - e_2 (long one):
    output(0, 0) = 1;
    output(1, 0) = - 1;
    //image of second simple root = e_2 - e_3 (long one)
    output(1, 1) = 1;
    output(2, 1) = - 1;
    //image of third simple root = e_3 (short one):
    output(2, 2) = 1;
    //image of fourth simple root (short one)
    output(0, 3) = RMHalf;
    output(1, 3) = RMHalf;
    output(2, 3) = RMHalf;
    output(3, 3) = RMHalf;
    //eps_2:
    //eps_4:
  }
  if (WeylLetter == 'G') {
    //taken from Humpreys, Introduction to Lie algebras and representation theory, page 65
    // the long root has the higher index
    output.init(3, 2);
    output.MakeZero();
    //image of the first simple root(short one):
    output(0, 0) = 1;
    output(1, 0) = - 1;
    //image of second simple root:
    output(0, 1) = - 2;
    output(1, 1) = 1;
    output(2, 1) = 1;
  }
}

void DynkinSimpleType::GetAn(int n, Matrix<Rational>& output) const {
  if (n <= 0 || n > 30000) {
    global.fatal << "This is a programming error: attempting to create type A_n with n ="
    << n << " is illegal. If this was a bad user input, it should "
    << " be handled at an earlier stage. " << global.fatal;
  }
  output.init(n, n);
  output.MakeZero();
  for (int i = 0; i < n - 1; i ++) {
    output(i, i) = 2;
    output(i + 1, i) = - 1;
    output(i, i + 1) = - 1;
  }
  output(n - 1, n - 1) = 2;
}

void DynkinSimpleType::GetBn(int n, Matrix<Rational>& output) const {
  this->GetAn(n, output);
  output(n - 1, n - 1) = 1;
}

Rational DynkinSimpleType::GetDynkinIndexParabolicallyInducingSubalgebra(char inputType) {
  switch (inputType) {
    case 'A':
    case 'B':
    case 'D':
    case 'E':
    case 'F':
      return 1;
    case 'G':
      return 3;
    case 'C':
      return 2;
    default:
      global.fatal << "DynkinSimpleType::GetDynkinIndexParabolicallyInducingSubalgebra called with input "
      << inputType << ", this is not allowed. " << global.fatal;
      return - 1;
  }
}

bool DynkinSimpleType::CanBeExtendedParabolicallyTo(const DynkinSimpleType& other) const {
  MacroRegisterFunctionWithName("DynkinSimpleType::CanBeExtendedParabolicallyTo");
  if (other.theRank <= this->theRank) {
    return false;
  }
  if (
    this->CartanSymmetricInverseScale / this->GetDynkinIndexParabolicallyInducingSubalgebra(other.theLetter) !=
    other.CartanSymmetricInverseScale
  ) {
    return false;
  }
  if (other.theLetter == 'F') {
    if (this->theLetter == 'A' && this->theRank < 3) {
      return true;
    }
    if (this->theLetter == 'B' && this->theRank == 3) {
      return true;
    }
    return false;
  }
  if (other.theLetter == 'E') {
    if (this->theRank < 5) {
      return this->theLetter == 'A';
    }
    if (this->theRank == 5) {
      return this->theLetter == 'D';
    }
    return this->theLetter == 'E';
  }
  return this->theLetter == 'A';
}

void DynkinSimpleType::Grow(List<DynkinSimpleType>& output, List<List<int> >* outputPermutationRoots) const {
  MacroRegisterFunctionWithName("DynkinSimpleType::Grow");
  //almost all simple types are grown from type A. Exceptions only for types F4 (grown from B_3),
  //E6 (grown from D_5), E7 (grown from E6) and E8 (grown from E7).
  output.SetSize(0);
  List<int> currentImagesSimpleRootsCurrent;
  if (outputPermutationRoots != nullptr) {
    outputPermutationRoots->SetSize(0);
  }
  currentImagesSimpleRootsCurrent.SetSize(this->theRank + 1);
  DynkinSimpleType newType;
  if (this->theLetter == 'B' && this->theRank == 3) {
    newType.MakeArbitrary('F', 4, this->CartanSymmetricInverseScale);
    output.AddOnTop(newType);
    if (outputPermutationRoots != nullptr) {
      for (int i = 0; i < currentImagesSimpleRootsCurrent.size; i ++) {
        currentImagesSimpleRootsCurrent[i] = i;
      }
      outputPermutationRoots->AddOnTop(currentImagesSimpleRootsCurrent);
    }
  }
  if (this->theLetter == 'D' && this->theRank == 5) {
    newType.MakeArbitrary('E', 6, this->CartanSymmetricInverseScale);
    output.AddOnTop(newType);
    if (outputPermutationRoots != nullptr) {
      currentImagesSimpleRootsCurrent[0] = 0;
      currentImagesSimpleRootsCurrent[1] = 2;
      currentImagesSimpleRootsCurrent[2] = 3;
      currentImagesSimpleRootsCurrent[3] = 4;
      currentImagesSimpleRootsCurrent[4] = 1;
      currentImagesSimpleRootsCurrent[5] = 5;
      outputPermutationRoots->AddOnTop(currentImagesSimpleRootsCurrent);
    }
  }
  if (this->theLetter == 'E' && this->theRank < 8) {
    newType.MakeArbitrary('E', this->theRank + 1, this->CartanSymmetricInverseScale);
    output.AddOnTop(newType);
    if (outputPermutationRoots != nullptr) {
      for (int i = 0; i < currentImagesSimpleRootsCurrent.size; i ++) {
        currentImagesSimpleRootsCurrent[i] = i;
      }
      outputPermutationRoots->AddOnTop(currentImagesSimpleRootsCurrent);
    }
  }
  if (this->theLetter != 'A') {
    return;
  }
  newType.MakeArbitrary(this->theLetter, this->theRank + 1, this->CartanSymmetricInverseScale);
  output.AddOnTop(newType);
  if (outputPermutationRoots != nullptr) {
    for (int i = 0; i < currentImagesSimpleRootsCurrent.size; i ++) {
      currentImagesSimpleRootsCurrent[i] = i;
    }
    outputPermutationRoots->AddOnTop(currentImagesSimpleRootsCurrent);
  }
  newType.MakeArbitrary('B', this->theRank + 1, this->CartanSymmetricInverseScale);
  output.AddOnTop(newType);
  if (outputPermutationRoots != nullptr) {
    outputPermutationRoots->AddOnTop(currentImagesSimpleRootsCurrent);
  }
  if (this->theRank > 1) {
    newType.MakeArbitrary('C', this->theRank + 1, this->CartanSymmetricInverseScale / 2);
    output.AddOnTop(newType);
    if (outputPermutationRoots != nullptr) {
      outputPermutationRoots->AddOnTop(currentImagesSimpleRootsCurrent);
    }
  }
  if (this->theRank > 2) {
    newType.MakeArbitrary('D', this->theRank + 1, this->CartanSymmetricInverseScale);
    output.AddOnTop(newType);
    if (outputPermutationRoots != nullptr) {
      outputPermutationRoots->AddOnTop(currentImagesSimpleRootsCurrent);
    }
  }
  if (this->theRank == 1) {
    newType.MakeArbitrary('G', 2, this->CartanSymmetricInverseScale / 3);
    output.AddOnTop(newType);
    if (outputPermutationRoots != nullptr) {
      outputPermutationRoots->AddOnTop(currentImagesSimpleRootsCurrent);
    }
  }
}

bool DynkinSimpleType::operator>(const DynkinSimpleType& other) const {
  if (this->theRank > other.theRank) {
    return true;
  }
  if (this->theRank < other.theRank) {
    return false;
  }
  if (this->CartanSymmetricInverseScale > other.CartanSymmetricInverseScale) {
    return true;
  }
  if (this->CartanSymmetricInverseScale < other.CartanSymmetricInverseScale) {
    return false;
  }
  if ((this->theLetter == 'B' || this->theLetter == 'C') && other.theLetter == 'D') {
    return true;
  }
  if (this->theLetter == 'D' && (other.theLetter == 'B' ||other.theLetter == 'C')) {
    return false;
  }
  return this->theLetter > other.theLetter;
  //  if (this->theLetter>other.theLetter)
  //    return true;
  //  if (this->theLetter<other.theLetter)
  //    return false;
  //  return this->CartanSymmetricScale>other.CartanSymmetricScale;
}

void DynkinSimpleType::GetCn(int n, Matrix<Rational>& output) const {
  this->GetAn(n, output);
  if (n < 2) {
    return;
  }
  output(n - 1, n - 1) = 4;
  output(n - 2, n - 1) = - 2;
  output(n - 1, n - 2) = - 2;
  output /= 2;
}

void DynkinSimpleType::GetDn(int n, Matrix<Rational>& output) const {
  this->GetAn(n, output);
  if (n < 3) {
    return;
  }
  output(n - 1, n - 2) = 0;
  output(n - 2, n - 1) = 0;
  output(n - 3, n - 1) = - 1;
  output(n - 1, n - 3) = - 1;
}

void DynkinSimpleType::GetEn(int n, Matrix<Rational>& output) const {
  this->GetAn(n, output);
  if (n < 4) {
    return;
  }
  output(0, 1) = 0;
  output(1, 0) = 0;
  output(1, 2) = 0;
  output(2, 1) = 0;
  output(0, 2) = - 1;
  output(1, 3) = - 1;
  output(2, 0) = - 1;
  output(3, 1) = - 1;
}

void DynkinSimpleType::GetF4(Matrix<Rational>& output) const {
  output.init(4, 4);
  Rational mh(- 1, 2);
  output(0, 0) = 2 ;   output(0, 1) = - 1; output(0, 2) = 0 ;  output(0, 3) = 0 ;
  output(1, 0) = - 1;  output(1, 1) = 2 ;  output(1, 2) = - 1; output(1, 3) = 0 ;
  output(2, 0) = 0 ;   output(2, 1) = - 1; output(2, 2) = 1 ;  output(2, 3) = mh;
  output(3, 0) = 0 ;   output(3, 1) = 0 ;  output(3, 2) = mh;  output(3, 3) = 1 ;
}

void DynkinSimpleType::GetG2(Matrix<Rational>& output) const {
  output.init(2, 2);
  output(0, 0) = 2;
  output(1, 1) = 6;
  output(1, 0) = - 3;
  output(0, 1) = - 3;
  output /= 3;
}

void DynkinSimpleType::GetCoCartanSymmetric(Matrix<Rational>& output) const {
  Matrix<Rational> currentCartan;
  this->GetCartanSymmetric(currentCartan);
  WeylGroupData::GetCoCartanSymmetric(currentCartan, output);
}

Rational DynkinSimpleType::GetPrincipalSlTwoCSInverseScale() const {
  MacroRegisterFunctionWithName("DynkinSimpleType::GetPrincipalSlTwoCSInverseScale");
  //Reference: Panyushev, On the Dynkin index of a principal sl(2)-subalgebra, Advances in Mathematics, 2008.
  Rational nonScaled = 0;
  switch(this->theLetter) {
    case 'A':
      nonScaled = (this->theRank + 2) * (this->theRank + 1) * this->theRank / 6;
    break;
    case 'B':
      nonScaled = this->theRank * (this->theRank + 1) * (2 * this->theRank + 1) / 3;
      break;
    case 'C':
      nonScaled = (this->theRank * 2 + 1) * (this->theRank * 2) * (this->theRank * 2 - 1) / 6;
      break;
    case 'D':
      nonScaled = (this->theRank - 1) * this->theRank * (2 * this->theRank - 1) / 3;
    break;
    case 'E':
      if (this->theRank == 6) {
        nonScaled = 156;
      }
      if (this->theRank == 7) {
        nonScaled = 399;
      }
      if (this->theRank == 8) {
        nonScaled = 1240;
      }
    break;
    case 'F':
      nonScaled = 156;
      break;
    case 'G':
      nonScaled = 28;
      break;
    default:
      global.fatal << "This is a programming error: requesting DynkinSimpleType::GetCartanSymmetric "
      << "from a non-initialized Dynkin simple type. " << global.fatal;
      break;
  }
  return nonScaled * this->CartanSymmetricInverseScale;
}

void DynkinSimpleType::GetCartanSymmetric(Matrix<Rational>& output) const {
  switch(this->theLetter) {
    case 'A': this->GetAn(this->theRank, output); break;
    case 'B': this->GetBn(this->theRank, output); break;
    case 'C': this->GetCn(this->theRank, output); break;
    case 'D': this->GetDn(this->theRank, output); break;
    case 'E': this->GetEn(this->theRank, output); break;
    case 'F': this->GetF4(output);                break;
    case 'G': this->GetG2(output);                break;
    default:
      global.fatal << "This is a programming error: requesting "
      << "DynkinSimpleType::GetCartanSymmetric from a non-initialized Dynkin simple type. " << global.fatal;
      break;
  }
  output /= this->CartanSymmetricInverseScale;
}

Rational DynkinSimpleType::GetRatioLongRootToFirst(char inputWeylLetter, int inputRank) {
  (void) inputRank; //avoid unused parameter warning, portable.
  switch (inputWeylLetter) {
    case 'A': return 1;
    case 'B': return 1;
    case 'C': return 2;
    case 'D': return 1;
    case 'E': return 1;
    case 'F': return 1;
    case 'G': return 3;
    default:
      return - 1;
  }
}

void DynkinSimpleType::operator++(int) {
  if (this->theRank == 1) {
    this->theRank ++;
    return;
  }
  if (this->theLetter == 'A') {
    if (this->theRank >= 4) {
      this->theLetter = 'D';
    } else {
      this->theLetter = 'B';
    }
    return;
  }
  if (this->theLetter == 'D') {
    this->theLetter = 'B';
    return;
  }
  if (this->theLetter == 'B') {
    if (this->theRank >= 3) {
      this->theLetter = 'C';
    } else {
      this->theLetter = 'G';
    }
    return;
  }
  if (this->theLetter == 'C') {
    if (this->theRank == 4) {
      this->theLetter = 'F';
      return;
    }
    if (this->theRank == 6 || this->theRank == 7 || this->theRank == 8) {
      this->theLetter = 'E';
      return;
    }
    this->theLetter = 'A';
    this->theRank ++;
    return;
  }
  if (this->theLetter == 'G' || this->theLetter == 'F' || this->theLetter == 'E') {
    this->theRank ++;
    this->theLetter = 'A';
    return;
  }
  global.fatal << "This is a programming error. "
  << "This is a portion of code that should never be reached. "
  << "Something has gone very wrong. " << global.fatal;
}

bool DynkinSimpleType::operator<(int otherRank) const {
  return this->theRank < otherRank;
}

ElementWeylGroupAutomorphisms::ElementWeylGroupAutomorphisms() {
  this->owner = nullptr;
  this->flagDeallocated = false;
}

ElementWeylGroupAutomorphisms::~ElementWeylGroupAutomorphisms() {
  this->owner = nullptr;
  this->flagDeallocated = true;
}

bool ElementWeylGroupAutomorphisms::CheckInitialization() const {
  if (this->owner == nullptr) {
    global.fatal << "Element of Weyl group automorphisms has zero owner. " << global.fatal;
  }
  return true;
}

void ElementWeylGroupAutomorphisms::MakeID(WeylGroupAutomorphisms& inputAutomorphisms) {
  this->owner = &inputAutomorphisms;
  this->generatorsLastAppliedFirst.SetSize(0);
}

void ElementWeylGroupAutomorphisms::MultiplyOnTheRightByOuterAuto(int outerAutoIndex) {
  simpleReflectionOrOuterAutomorphism generator;
  generator.index = outerAutoIndex;
  generator.flagIsOuter = true;
  this->generatorsLastAppliedFirst.AddOnTop(generator);
}

void ElementWeylGroupAutomorphisms::MakeSimpleReflection(int simpleRootIndex, WeylGroupAutomorphisms& inputWeyl) {
  this->owner = &inputWeyl;
  this->generatorsLastAppliedFirst.SetSize(1);
  this->generatorsLastAppliedFirst[0].MakeSimpleReflection(simpleRootIndex);
}

void ElementWeylGroupAutomorphisms::MakeOuterAuto(int outerAutoIndex, WeylGroupAutomorphisms& inputWeyl) {
  this->owner = &inputWeyl;
  this->generatorsLastAppliedFirst.SetSize(1);
  this->generatorsLastAppliedFirst[0].MakeOuterAuto(outerAutoIndex);
}


unsigned int ElementWeylGroupAutomorphisms::HashFunction(const ElementWeylGroupAutomorphisms& input) {
  return input.HashFunction();
}

unsigned int ElementWeylGroupAutomorphisms::HashFunction() const {
  return this->generatorsLastAppliedFirst.HashFunction();
}

std::string ElementWeylGroupAutomorphisms::ToString(FormatExpressions* theFormat) const {
  MacroRegisterFunctionWithName("ElementWeylGroupAutomorphisms::ToString");
  (void) theFormat;
  if (this->generatorsLastAppliedFirst.size == 0) {
    return "id";
  }
  std::stringstream out;
  for (int i = 0; i < this->generatorsLastAppliedFirst.size; i ++) {
    out << this->generatorsLastAppliedFirst[i].ToString();
  }
  return out.str();
}

void ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::MultiplyOnTheRightBySimpleRootInner(int simpleRootIndex) {
  simpleReflectionOrOuterAutomorphism generator;
  generator.index = simpleRootIndex;
  this->generatorsLastAppliedFirst.AddOnTop(generator);
}

void ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::MultiplyOnTheRightByOuterAutomorphism(int indexOuterAutomorphism) {
  simpleReflectionOrOuterAutomorphism generator;
  generator.index = indexOuterAutomorphism;
  generator.flagIsOuter = true;
  this->generatorsLastAppliedFirst.AddOnTop(generator);
}

unsigned int ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::HashFunction(
  const ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms& input
) {
  return input.HashFunction();
}

unsigned int ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::HashFunction() const {
  return this->generatorsLastAppliedFirst.HashFunction();
}

std::string ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::ToString(FormatExpressions* theFormat) const {
  MacroRegisterFunctionWithName("ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::ToString");
  (void) theFormat;
  if (this->generatorsLastAppliedFirst.size == 0) {
    return "id";
  }
  std::stringstream out;
  for (int i = 0; i < this->generatorsLastAppliedFirst.size; i ++) {
    out << this->generatorsLastAppliedFirst[i].ToString();
  }
  return out.str();
}

bool ElementWeylGroup::CheckConsistency() const {
  if (this->flagDeallocated) {
    global.fatal << "Programming error: use after free of class ElementWeylGroup" << global.fatal;
  }
  return true;
}

bool ElementWeylGroup::CheckInitialization() const {
  this->CheckConsistency();
  if (this->owner == nullptr) {
    global.fatal << "This is a programming error: non-initialized element Weyl group. " << global.fatal;
    return false;
  }
  this->owner->CheckConsistency();
  return true;
}

int ElementWeylGroup::Sign() const {
  return this->generatorsLastAppliedFirst.size % 2 == 0 ? 1 : - 1;
}

ElementWeylGroup ElementWeylGroup::operator^(
  const ElementWeylGroup& right
) const {
  if (this->owner != right.owner) {
    global.fatal << "Not allowed to conjugate elements of different Weyl groups. "
    << "If you did this intentionally, change the present file. "
    << global.fatal;
  }
  ElementWeylGroup out;
  out.owner = this->owner;
  out.generatorsLastAppliedFirst = right.generatorsLastAppliedFirst;
  out.generatorsLastAppliedFirst.AddListOnTop(this->generatorsLastAppliedFirst);
  for (int i = right.generatorsLastAppliedFirst.size - 1; i >= 0; i --) {
    out.generatorsLastAppliedFirst.AddOnTop(right.generatorsLastAppliedFirst[i]);
  }
  out.MakeCanonical();
  return out;
}

void ElementWeylGroup::ConjugationAction(
  const ElementWeylGroup& ConjugateWith,
  const ElementWeylGroup& ConjugateOn,
  ElementWeylGroup& out
) {
  out = ConjugateOn ^ ConjugateWith;
}

void ElementWeylGroup::operator*=(const ElementWeylGroup& other) {
  if (this->owner != other.owner) {
    global.fatal << "This is a programming error: attempting to "
    << "multiply elements belonging to different Weyl groups. " << global.fatal;
  }
  if (&other == this) {
    ElementWeylGroup otherCopy = other;
    (*this) *= otherCopy;
    return;
  }
  this->generatorsLastAppliedFirst.AddListOnTop(other.generatorsLastAppliedFirst);
  this->MakeCanonical();
}

Vector<Rational> ElementWeylGroup::operator*(const Vector<Rational>& v) const {
  Vector<Rational> out = v;
  this->owner->ActOn(*this,out);
  return out;
}
void ElementWeylGroup::MakeID(WeylGroupData& inputWeyl) {
  this->owner = &inputWeyl;
  this->generatorsLastAppliedFirst.SetSize(0);
}

void ElementWeylGroup::MakeID(const FiniteGroup<ElementWeylGroup> &inputGroup) {
  this->owner = inputGroup.generators[0].owner;
  this->generatorsLastAppliedFirst.SetSize(0);
}

void ElementWeylGroup::MakeID(const ElementWeylGroup& initializeFrom) {
  this->MakeID(*initializeFrom.owner);
}

bool ElementWeylGroup::IsID() {
  this->MakeCanonical();
  return this->generatorsLastAppliedFirst.size == 0;
}

void ElementWeylGroup::MakeRootReflection(
  const Vector<Rational>& mustBeRoot, WeylGroupData& inputWeyl
) {
  *this = inputWeyl.GetRootReflection(inputWeyl.RootSystem.GetIndexIMustContainTheObject(mustBeRoot));
}

void ElementWeylGroup::MakeSimpleReflection(int simpleRootIndex, WeylGroupData& inputWeyl) {
  this->owner = &inputWeyl;
  this->generatorsLastAppliedFirst.SetSize(1);
  this->generatorsLastAppliedFirst[0].MakeSimpleReflection(simpleRootIndex);
}

void ElementWeylGroup::MakeFromRhoImage(const Vector<Rational>& inputRhoImage, WeylGroupData& inputWeyl) {
  this->owner = &inputWeyl;
  int theRank = this->owner->GetDim();
  this->generatorsLastAppliedFirst.SetSize(0);
  Vector<Rational> theVector = inputRhoImage;
  simpleReflection theGen;
  while (theVector != this->owner->rho) {
    for (int i = 0; i < theRank; i ++) {
      if (this->owner->GetScalarProdSimpleRoot(theVector, i) < 0) {
        this->owner->SimpleReflection(i, theVector);
        theGen.MakeSimpleReflection(i);
        this->generatorsLastAppliedFirst.AddOnTop(theGen);
        break;
      }
    }
  }
}

void ElementWeylGroup::MakeCanonical() {
  MacroRegisterFunctionWithName("ElementWeylGroup::MakeCanonical");
  this->CheckInitialization();
  if (this->owner->rho.size == 0) {
    this->owner->ComputeRho(false);
  }
  Vector<Rational> theVector;
  this->owner->ActOn(*this, this->owner->rho, theVector);
  this->MakeFromRhoImage(theVector, *this->owner);
}

bool ElementWeylGroup::HasDifferentConjugacyInvariantsFrom(
  const ElementWeylGroup& right
) const {
  MacroRegisterFunctionWithName("ElementWeylGroup::HasDifferentConjugacyInvariantsFrom");
  if ((this->generatorsLastAppliedFirst.size % 2) != (right.generatorsLastAppliedFirst.size % 2)) {
    return true;
  }
  if (*this == right) {
    //just in case
    return false;
  }
  this->CheckInitialization();
  Polynomial<Rational> leftCharPoly, rightCharPoly;
  this->GetCharacteristicPolyStandardRepresentation(leftCharPoly);
  right.GetCharacteristicPolyStandardRepresentation(rightCharPoly);
  if (leftCharPoly != rightCharPoly) {
    return true;
  }
  VectorSparse<Rational> leftCycleStructure, rightCycleStructure;
  this->GetCycleStructure(leftCycleStructure);
  right.GetCycleStructure(rightCycleStructure);
  if (leftCycleStructure != rightCycleStructure) {
    return true;
  }
  return false;
}

std::string ElementWeylGroup::ToStringInvariants(FormatExpressions* theFormat) const {
  MacroRegisterFunctionWithName("ElementWeylGroup::GetCycleStructure");
  (void) theFormat;
  VectorSparse<Rational> theCycleStructure;
  this->GetCycleStructure(theCycleStructure);
  FormatExpressions cycleLetterFormat;
  cycleLetterFormat.polyDefaultLetter = "c";
  std::stringstream out;
  out << "Cycle structure: " << theCycleStructure.ToString(&cycleLetterFormat) << ". ";
  return out.str();
}

void ElementWeylGroup::GetCycleStructure(
  VectorSparse<Rational>& outputIndexIsCycleSizeCoordinateIsCycleMult
) const {
  MacroRegisterFunctionWithName("ElementWeylGroup::GetCycleStructure");
  this->CheckInitialization();
  outputIndexIsCycleSizeCoordinateIsCycleMult.MakeZero();
  List<bool> Explored;
  HashedList<Vector<Rational> >& theRootSystem = this->owner->RootSystem;
  Explored.initializeFillInObject(theRootSystem.size, false);
  Vector<Rational> currentRoot;
  for (int i = 0; i < Explored.size; i ++) {
    if (!Explored[i]) {
      int currentCycleSize = 1;
      currentRoot = theRootSystem[i];
      for (
        this->owner->ActOn(*this, currentRoot, currentRoot);
        currentRoot != theRootSystem[i];
        this->owner->ActOn(*this, currentRoot, currentRoot)
      ) {
        currentCycleSize ++;
        Explored[theRootSystem.GetIndex(currentRoot)] = true;
      }
      outputIndexIsCycleSizeCoordinateIsCycleMult.AddMonomial(MonomialVector(currentCycleSize - 1), 1);
    }
  }
}

void ElementWeylGroup::GetCharacteristicPolyStandardRepresentation(Polynomial<Rational>& output) const {
  MacroRegisterFunctionWithName("ElementWeylGroup::GetCharacteristicPolyStandardRepresentation");
  this->CheckInitialization();
  Matrix<Rational> standardRepMat;
  this->owner->GetMatrixStandardRep(*this, standardRepMat);
  output.AssignCharPoly(standardRepMat);
}

ElementWeylGroup ElementWeylGroup::Inverse() const {
  ElementWeylGroup out = *this;
  out.generatorsLastAppliedFirst.ReverseOrderElements();
  out.MakeCanonical();
  return out;
}

std::string ElementWeylGroup::ToString(FormatExpressions* theFormat) const {
  MacroRegisterFunctionWithName("ElementWeylGroup::ToString");
  (void) theFormat;//avoid unused parameter warning, portable
  if (this->generatorsLastAppliedFirst.size == 0) {
    return "id";
  }
  std::stringstream out;
  for (int i = 0; i < this->generatorsLastAppliedFirst.size; i ++) {
    out << this->generatorsLastAppliedFirst[i].ToString();
  }
  return out.str();
}

unsigned int ElementWeylGroup::HashFunction() const {
  int top = MathRoutines::Minimum(this->generatorsLastAppliedFirst.size, ::SomeRandomPrimesSize);
  unsigned int result = 0;
  for (int i = 0; i < top; i ++) {
    result += this->generatorsLastAppliedFirst[i].HashFunction() * ::SomeRandomPrimes[i];
  }
  return result;
}

bool ElementWeylGroup::operator>(const ElementWeylGroup& other) const {
  if (this->owner != other.owner) {
    global.fatal << "Comparing elements of different Weyl groups. " << global.fatal;
  }
  return this->generatorsLastAppliedFirst > other.generatorsLastAppliedFirst;
}

void ElementWeylGroup::MultiplyOnTheRightBySimpleReflection(int reflectionIndex) {
  simpleReflection theGen;
  theGen.MakeSimpleReflection(reflectionIndex);
  this->generatorsLastAppliedFirst.AddOnTop(theGen);
}

void WeylGroupData::SimpleReflectionDualSpace(int index, Vector<Rational>& DualSpaceElement) {
  Rational coefficient, tempRat;
  coefficient.Assign(DualSpaceElement[index]);
  coefficient.DivideBy(this->CartanSymmetric.elements[index][index]);
  for (int i = 0; i < this->CartanSymmetric.NumCols; i ++) {
    tempRat.Assign(coefficient);
    tempRat.MultiplyBy(this->CartanSymmetric.elements[index][i] * (- 2));
    DualSpaceElement[i] += (tempRat);
  }
}

ElementWeylGroup WeylGroupData::GetRootReflection(int rootIndex) {
  if (this->rho.size == 0) {
    this->ComputeRho(true);
  }
  Vector<Rational> rhoImage;
  this->ReflectBetaWRTAlpha(this->RootSystem[rootIndex], this->rho, false, rhoImage);
  ElementWeylGroup result;
  result.MakeFromRhoImage(rhoImage, *this);
  return result;
}

void WeylGroupData::SimpleReflectionRoot(int index, Vector<Rational>& theRoot, bool RhoAction, bool UseMinusRho) {
  Rational alphaShift, tempRat;
  alphaShift.MakeZero();
  for (int i = 0; i < this->CartanSymmetric.NumCols; i ++) {
    tempRat = theRoot[i];
    tempRat.MultiplyBy(this->CartanSymmetric.elements[index][i] * (- 2));
    alphaShift += tempRat;
  }
  alphaShift.DivideBy(this->CartanSymmetric.elements[index][index]);
  if (RhoAction) {
    if (UseMinusRho) {
      alphaShift.AddInteger(1);
    } else {
      alphaShift.AddInteger(- 1);
    }
  }
  theRoot[index] += (alphaShift);
}

void WeylGroupData::SimpleReflectionRootAlg(int index, PolynomialSubstitution<Rational>& theRoot, bool RhoAction) {
  int lengthA = this->CartanSymmetric.elements[index][index].NumShort;
  Polynomial<Rational> AscalarB, tempP;
  AscalarB.MakeZero();
  for (int i = 0; i < this->CartanSymmetric.NumCols; i ++) {
    tempP.MakeZero();
    tempP = theRoot[i];
    tempP *= CartanSymmetric.elements[index][i];
    AscalarB += (tempP);
  }
  AscalarB *= - 2;
  AscalarB /= lengthA;
  theRoot[index] += AscalarB;
  if (RhoAction) {
    theRoot[index] += - 1;
  }
}

Matrix<Rational> WeylGroupData::GetMatrixStandardRep(int elementIndex) const {
  Matrix<Rational> result;
  this->GetMatrixStandardRep(this->theGroup.theElements[elementIndex], result);
  return result;
}

void WeylGroupData::reset() {
  this->FundamentalToSimpleCoords.init(0, 0);
  this->SimpleToFundamentalCoords.init(0, 0);
  this->MatrixSendsSimpleVectorsToEpsilonVectors.FreeMemory();
  this->flagFundamentalToSimpleMatricesAreComputed = false;
  this->flagIrrepsAreComputed = false;
  this->flagCharTableIsComputed = false;

  this->theDynkinType.MakeZero();
  this->CartanSymmetric.init(0, 0);
  this->CoCartanSymmetric.init(0, 0);
  this->rho.SetSize(0);
  this->rhoOrbit.Clear();
  this->RootSystem.Clear();
  this->RootsOfBorel.SetSize(0);
  this->ccCarterLabels.SetSize(0);
  this->irrepsCarterLabels.SetSize(0);

  this->theGroup.init();
  this->theGroup.specificDataPointer = this;
  this->theGroup.GetWordByFormula = this->GetWordByFormulaImplementation;
  this->theGroup.GetSizeByFormula = this->GetSizeByFormulaImplementation;
  this->theGroup.ComputeCCSizesAndRepresentativesByFormula = nullptr;
  this->theGroup.AreConjugateByFormula = nullptr;
  this->theGroup.ComputeIrreducibleRepresentationsWithFormulas = nullptr;
}

void WeylGroupData::ActOnAffineHyperplaneByGroupElement(
  int index, affineHyperplane<Rational>& output, bool RhoAction, bool UseMinusRho
) {
  int numGens = this->theGroup.theElements[index].generatorsLastAppliedFirst.size;
  for (int i = numGens - 1; i >= 0; i --) {
    this->SimpleReflectionRoot(
      this->theGroup.theElements[index].generatorsLastAppliedFirst[i].index,
      output.affinePoint,
      RhoAction,
      UseMinusRho
    );
    this->SimpleReflectionDualSpace(
      this->theGroup.theElements[index].generatorsLastAppliedFirst[numGens - i - 1].index,
      output.normal
    );
  }
}

bool WeylGroupData::GetWordByFormulaImplementation(
  FiniteGroup<ElementWeylGroup>& G, const ElementWeylGroup& g, List<int>& out
) {
  (void) G;// avoid unused parameter warning, portable.
  out.SetSize(g.generatorsLastAppliedFirst.size);
  for (int i = 0; i < g.generatorsLastAppliedFirst.size; i ++) {
    out[i] = g.generatorsLastAppliedFirst[i].index;
  }
  return true;
}

void WeylGroupData::GetSignCharacter(Vector<Rational>& out) {
  if (!this->theGroup.flagCCRepresentativesComputed) {
    this->theGroup.ComputeCCSizesAndRepresentatives();
  }
  out.SetSize(this->theGroup.ConjugacyClassCount());
  for (int i = 0; i < this->theGroup.ConjugacyClassCount(); i ++) {
    out[i] = this->theGroup.conjugacyClasseS[i].representative.Sign();
  }
}

/*
// this is only used in one place.
void SubgroupDataWeylGroup::GetSignCharacter(Vector<Rational>& out) {
  MacroRegisterFunctionWithName("SubgroupRootReflections::GetSignCharacter");
  this->theSubgroupData->theSubgroup->GetSignCharacter(out);
//  if (!this->theSubgroup->flagCCRepresentativesComputed)
//    this->theSubgroup->ComputeCCSizesAndRepresentatives(0);
//  out.SetSize(this->theSubgroup->ConjugacyClassCount());
//  for (int i = 0; i < this->theSubgroup->ConjugacyClassCount(); i ++)
//    out[i] = this->theSubgroup->conjugacyClasseS[i].representative.Sign();
}
*/

void WeylGroupData::GetTrivialRepresentation(
  GroupRepresentationCarriesAllMatrices<FiniteGroup<ElementWeylGroup>, Rational>& output
) {
  MacroRegisterFunctionWithName("WeylGroup::GetTrivialRepresentation");
  this->theGroup.CheckInitializationConjugacyClasses();
  output.init(this->theGroup);
  output.basis.MakeEiBasis(1);
  for (int i = 0; i < this->GetDim(); i ++) {
    output.generatorS[i].MakeIdMatrix(1);
  }
  output.GetCharacter();
}

void WeylGroupData::GetSignRepresentation(
  GroupRepresentationCarriesAllMatrices<FiniteGroup<ElementWeylGroup>, Rational>& output
) {
  MacroRegisterFunctionWithName("WeylGroup::GetSignRepresentation");
  this->theGroup.CheckInitializationConjugacyClasses();
  output.init(this->theGroup);
  output.basis.MakeEiBasis(1);
  ElementWeylGroup currentElt;
  for (int i = 0; i < this->GetDim(); i ++) {
    currentElt.MakeSimpleReflection(i, *this);
    output.generatorS[i].MakeIdMatrix(1);
    output.generatorS[i] *= currentElt.Sign();
  }
  output.GetCharacter();
}

void WeylGroupData::GetStandardRepresentation(
  GroupRepresentationCarriesAllMatrices<FiniteGroup<ElementWeylGroup>, Rational>& output
) {
  MacroRegisterFunctionWithName("WeylGroup::GetStandardRepresentation");
  this->theGroup.CheckInitializationConjugacyClasses();
  output.init(this->theGroup);
  output.basis.MakeEiBasis(this->GetDim());
  for (int i = 0; i < this->GetDim(); i ++) {
    this->GetSimpleReflectionMatrix(i, output.generatorS[i]);
  }
  output.GetCharacter();
}

void WeylGroupData::GetStandardRepresentationMatrix(int g, Matrix<Rational>& output) const {
  // the rank is the number of rows of the Cartan form
  output.init(this->CartanSymmetric.NumRows,this->CartanSymmetric.NumRows);
  for (int i = 0; i < this->CartanSymmetric.NumRows; i ++) {
    Vector<Rational> v;
    v.MakeEi(this->CartanSymmetric.NumRows, i);
    this->ActOn(g, v);
    for (int j = 0; j < this->CartanSymmetric.NumRows; j ++) {
      output.elements[j][i] = v[j];
    }
  }
}

void WeylGroupData::GenerateAdditivelyClosedSubset(Vectors<Rational>& input, Vectors<Rational>& output) {
  output = (input);
  Vector<Rational> tempRoot;
  for (int i = 0; i < output.size; i ++) {
    for (int j = i + 1; j < output.size; j ++) {
      tempRoot = output[i] + output[j];
      if (this->IsARoot(tempRoot)) {
        output.AddOnTopNoRepetition(tempRoot);
      }
    }
  }
}

void WeylGroupData::PerturbWeightToRegularWRTrootSystem(const Vector<Rational>& inputH, Vector<Rational>& output) {
  output = (inputH);
  int indexFirstNonRegular;
  while (!this->IsRegular(output, &indexFirstNonRegular)) {
    const Vector<Rational>& theBadRoot = this->RootSystem[indexFirstNonRegular];
    Rational maxMovement = 0;
    Rational tempRat1, tempRat2, tempMaxMovement;
    for (int i = 0; i < this->RootsOfBorel.size; i ++) {
      this->RootScalarCartanRoot(theBadRoot, this->RootsOfBorel[i], tempRat1);
      this->RootScalarCartanRoot(output, this->RootsOfBorel[i], tempRat2);
      if ((!tempRat1.IsEqualToZero()) && (!tempRat2.IsEqualToZero())) {
        tempMaxMovement = tempRat2 / tempRat1;
        tempMaxMovement.AssignAbsoluteValue();
        if ((tempMaxMovement < maxMovement) || maxMovement.IsEqualToZero()) {
          maxMovement = tempMaxMovement;
        }
      }
    }
    int tempInt = 2;
    if (this->RootScalarCartanRoot(theBadRoot, inputH).IsNegative()) {
      tempInt = - 2;
    }
    output += theBadRoot*maxMovement / tempInt;
  }
}

bool WeylGroupData::IsRegular(Vector<Rational>& input, int* indexFirstPerpendicularRoot) {
  if (indexFirstPerpendicularRoot != nullptr) {
    *indexFirstPerpendicularRoot = - 1;
  }
  for (int i = 0; i < this->RootSystem.size; i ++) {
    if (this->RootScalarCartanRoot(input, this->RootSystem[i]).IsEqualToZero()) {
      if (indexFirstPerpendicularRoot != nullptr) {
        *indexFirstPerpendicularRoot = i;
      }
      return false;
    }
  }
  return true;
}

LargeInteger WeylGroupData::SizeByFormulaOrNeg1(char weylLetter, int theDim) {
  //Humphreys, Introduction to Lie algebras and representation theory(1980), page 66, Table 1
  if (
    weylLetter != 'A' &&
    weylLetter != 'B' &&
    weylLetter != 'C' &&
    weylLetter != 'D' &&
    weylLetter != 'E' &&
    weylLetter != 'F' &&
    weylLetter != 'G'
  ) {
    global.fatal << "WeylGroupData::SizeByFormulaOrNeg1 called with impossible Weyl type: " << weylLetter << global.fatal;
  }
  LargeInteger theOutput = 1;
  if (weylLetter == 'A') {
    theOutput = Rational::Factorial(theDim + 1);
  }
  if (weylLetter == 'B' || weylLetter == 'C') {
    theOutput = Rational::Factorial(theDim)*Rational::TwoToTheNth(theDim);
  }
  if (weylLetter == 'D') {
    theOutput = Rational::Factorial(theDim)*Rational::TwoToTheNth(theDim - 1);
  }
  if (weylLetter == 'E') {
    if (theDim == 6) {
      theOutput = 51840;
    }
    if (theDim == 7) {
      theOutput = 1024;
      theOutput *= 81 * 35;
    }
    if (theDim == 8) {
      theOutput = 1024 * 16;
      theOutput *= 81 * 3;
      theOutput *= 25 * 7;
    }
  }
  if (weylLetter == 'F') {
    theOutput = 128 * 9;
  }
  if (weylLetter == 'G') {
    theOutput = 12;
  }
  return theOutput;
}

void WeylGroupData::GetWord(int g, List<int>& out) const {
  out.SetSize(this->theGroup.theElements[g].generatorsLastAppliedFirst.size);
  for (int i = 0; i < this->theGroup.theElements[g].generatorsLastAppliedFirst.size; i ++) {
    out[i] = this->theGroup.theElements[g].generatorsLastAppliedFirst[i].index;
  }
}

bool WeylGroupData::operator==(const WeylGroupData& other) const {
  return this->CartanSymmetric == other.CartanSymmetric && this->theDynkinType == other.theDynkinType;
}

void WeylGroupData::ActOnRootByGroupElement(int index, Vector<Rational>& theRoot, bool RhoAction, bool UseMinusRho) {
  const ElementWeylGroup& currentElt = this->theGroup.theElements[index];
  for (int i = currentElt.generatorsLastAppliedFirst.size - 1; i >= 0; i --) {
    this->SimpleReflectionRoot(currentElt.generatorsLastAppliedFirst[i].index, theRoot, RhoAction, UseMinusRho);
  }
}

void WeylGroupData::GetCoCartanSymmetric(const Matrix<Rational>& input, Matrix<Rational>& output) {
  MacroRegisterFunctionWithName("DynkinType::GetCoCartanSymmetric");
  if (&input == &output) {
    Matrix<Rational> inputCopy = input;
    WeylGroupData::GetCoCartanSymmetric(inputCopy, output);
    return;
  }
  output.init(input.NumRows, input.NumCols);
  for (int i = 0; i < input.NumRows; i ++) {
    for (int j = 0; j < input.NumCols; j ++) {
      output(i, j) = input(i, j) * 4 / (input(i, i) * input(j, j));
    }
  }
}

void WeylGroupData::GenerateRootSystem() {
  Vectors<Rational> startRoots;
  HashedList<Vector<Rational> > theRootsFinder;
  startRoots.MakeEiBasis(this->GetDim());
  int estimatedNumRoots = this->theDynkinType.GetRootSystemSize();
  this->GenerateOrbit(startRoots, false, theRootsFinder, false, estimatedNumRoots);
  this->RootSystem.Clear();
  this->RootSystem.SetExpectedSize(theRootsFinder.size);
  this->RootsOfBorel.SetSize(0);
  this->RootsOfBorel.Reserve(theRootsFinder.size / 2);
  for (int i = 0; i < theRootsFinder.size; i ++) {
    if (theRootsFinder[i].IsPositiveOrZero()) {
      this->RootsOfBorel.AddOnTop(theRootsFinder[i]);
    }
  }
  this->RootsOfBorel.QuickSortAscending();
  for (int i = this->RootsOfBorel.size - 1; i >= 0; i --) {
    this->RootSystem.AddOnTop(- this->RootsOfBorel[i]);
  }
  for (int i = 0; i < this->RootsOfBorel.size; i ++) {
    this->RootSystem.AddOnTop(this->RootsOfBorel[i]);
  }
}

void WeylGroupData::ActOnRootAlgByGroupElement(int index, PolynomialSubstitution<Rational>& theRoot, bool RhoAction) {
  for (int i = this->theGroup.theElements[index].generatorsLastAppliedFirst.size - 1; i >= 0; i --) {
    this->SimpleReflectionRootAlg(this->theGroup.theElements[index].generatorsLastAppliedFirst[i].index, theRoot, RhoAction);
  }
}

void WeylGroupData::ComputeWeylGroupAndRootsOfBorel(Vectors<Rational>& output) {
  this->theGroup.ComputeAllElements(false);
  output.size = 0;
  output.Reserve(this->RootSystem.size / 2);
  for (int i = 0; i < this->RootSystem.size; i ++) {
    if (this->RootSystem[i].IsPositiveOrZero()) {
      output.AddOnTop(this->RootSystem[i]);
    }
  }
}

bool WeylGroupData::LeftIsHigherInBruhatOrderThanRight(ElementWeylGroup& left, ElementWeylGroup& right) {
  Vector<Rational> leftImage;
  leftImage = this->rho;
  Vector<Rational> rightImage;
  rightImage = this->rho;
  this->ActOn(left, leftImage, leftImage);
  this->ActOn(right, rightImage, rightImage);
  return (rightImage - leftImage).IsPositiveOrZero() && !(rightImage - leftImage).IsEqualToZero();
}

void WeylGroupData::ComputeRootsOfBorel(Vectors<Rational>& output) {
  output.size = 0;
  this->RootSystem.Clear();
  this->GenerateRootSystem();
  output = this->RootsOfBorel;
}

std::string WeylGroupData::ToStringCppCharTable(FormatExpressions* theFormat) {
  MacroRegisterFunctionWithName("WeylGroup::ToStringCppConjugacyClasses");
  (void) theFormat;//portable way to avoid non-used parameter warning.
  if (!this->flagCharTableIsComputed) {
    return "<br>Conjugacy classes not computed";
  }
  std::stringstream out;
  out << "<hr>Here is the c++ input code for the char table.";
  out << "<br>";
  FormatExpressions theFormatNoDynkinTypePlusesExponents;
  theFormatNoDynkinTypePlusesExponents.flagDynkinTypeDontUsePlusAndExponent = true;
  out << "bool LoadCharTable" << this->theDynkinType.ToString(&theFormatNoDynkinTypePlusesExponents) << "(WeylGroup& output)\n<br>{ ";
  out << " output.characterTable.SetExpectedSize(" << this->theGroup.GetSize().ToString() << "); output.characterTable.SetSize(0);";
  out << "\n<br>&nbsp;&nbsp;ClassFunction&lt;FiniteGroup&lt;ElementWeylGroup&lt;WeylGroup&gt; &gt;, Rational&gt; currentCF;";
  out << "\n<br>&nbsp;&nbsp;currentCF.G = &output;";
  for (int i = 0; i < this->theGroup.characterTable.size; i ++) {
    out << "\n<br>&nbsp;&nbsp;currentCF.data.AssignString(\"";
    out << "(";
    //Print vector ensuring every number is at least 3 characters wide. (3 should suffice for E8... or does it?)
    for (int j = 0; j < this->theGroup.characterTable[i].data.size; j ++) {
      std::string theNumber = this->theGroup.characterTable[i].data[j].ToString();
      out << theNumber;
      for (size_t k = theNumber.size(); k < 3; k ++) {
        out << "&nbsp;";
      }
      if (j != this->theGroup.characterTable[i].data.size - 1) {
        out << ", ";
      }
    }
    out << ")";
    out << "\"); output.characterTable.AddOnTop(currentCF);";
  }
  out << "\n<br>&nbsp;&nbsp;output.irrepsCarterLabels.SetSize(0);";
  for (int i = 0; i < this->irrepsCarterLabels.size; i ++) {
    out << "\n<br>&nbsp;&nbsp;output.irrepsCarterLabels.AddOnTop(\"" << this->irrepsCarterLabels[i] << "\");";
  }
  out << "\n<br>&nbsp;&nbsp;return true;";
  out << "\n<br>}";
  return out.str();
}

std::string WeylGroupData::ToStringCppConjugacyClasses(FormatExpressions* theFormat) {
  MacroRegisterFunctionWithName("WeylGroup::ToStringCppConjugacyClasses");
  (void) theFormat;//portable way to avoid non-used parameter warning.
  if (!this->theGroup.flagCCRepresentativesComputed) {
    return "";
  }
  std::stringstream out;
  out << "<hr>Here is the c++ input code for the conjugacy class table.";
  out << "<br>";
  FormatExpressions theFormatNoDynkinTypePlusesExponents;
  theFormatNoDynkinTypePlusesExponents.flagDynkinTypeDontUsePlusAndExponent = true;
  out << "bool LoadConjugacyClasses" << this->theDynkinType.ToString(&theFormatNoDynkinTypePlusesExponents)
  << "(WeylGroup& output)\n<br>{ ";
  out << "output.ComputeRho(true);";
  out << "\n<br>&nbsp;&nbsp;WeylGroup::ConjugacyClass emptyClass;";
  out << "\n<br>&nbsp;&nbsp;emptyClass.flagRepresentativeComputed = true;";
  out << "\n<br>&nbsp;&nbsp;output.conjugacyClasseS.initializeFillInObject("
  << this->theGroup.conjugacyClasseS.size << ", emptyClass);";
  for (int i = 0; i < this->theGroup.ConjugacyClassCount(); i ++) {
    out << "\n<br>&nbsp;&nbsp;output.conjugacyClasseS[" << i;
    for (size_t j = (Rational(i)).ToString().size(); j < 3; j ++) { //<-if the index i is smaller than 100, make sure it takes
      out << "&nbsp;"; // making sure index has width exactly 3 spaces
    }
    out << "].representative.MakeFromReadableReflections(output, false, \"";
    for (int j = 0; j < this->theGroup.conjugacyClasseS[i].representative.generatorsLastAppliedFirst.size; j ++) {
      out << this->theGroup.conjugacyClasseS[i].representative.generatorsLastAppliedFirst[j].index + 1;
      if (j != this->theGroup.conjugacyClasseS[i].representative.generatorsLastAppliedFirst.size - 1) {
        out << ",";
      }
    }
    out << "\");";
  }
  for (int i = 0; i < this->theGroup.ConjugacyClassCount(); i ++) {
    out << "\n<br>&nbsp;&nbsp;output.conjugacyClasseS[" << i;
    for (size_t j = (Rational(i)).ToString().size(); j < 3; j ++) { //<-if the index i is smaller than 100, make sure it takes
      out << "&nbsp;"; // making sure index has width exactly 3 spaces
    }
    out  << "].size =" << this->theGroup.conjugacyClasseS[i].size.ToString() << ";";
  }
  out << "\n<br>&nbsp;&nbsp;output.ccCarterLabels.SetSize(0);";
  for (int i = 0; i < this->ccCarterLabels.size; i ++) {
    out << "\n<br>&nbsp;&nbsp;output.ccCarterLabels.AddOnTop(\"" << this->ccCarterLabels[i] << "\");";
  }
  out << "\n<br>&nbsp;&nbsp;output.LoadConjugacyClassesHelper();";
  out << "\n<br>&nbsp;&nbsp;return true;";
  out << "\n<br>}";
  return out.str();
}

std::string WeylGroupData::ToStringRootsAndRootReflections(FormatExpressions* theFormat) {
  MacroRegisterFunctionWithName("WeylGroup::ToStringRootsAndRootReflections");
  (void) theFormat;//portable way to avoid non-used parameter warning.
  std::stringstream out, outLatex;
  out << "<br>The root system has " << this->RootSystem.size << " elements.\n";
  out << "<table><tr><td>Simple basis coordinates</td><td>Epsilon coordinates</td>"
  << "<td>Reflection w.r.t. root</td></tr>";
  Vectors<Rational> rootSystemEpsCoords;
  this->GetEpsilonCoords(this->RootSystem, rootSystemEpsCoords);
  ElementWeylGroup currentRootReflection;
  for (int i = 0; i < this->RootSystem.size; i ++) {
    const Vector<Rational>& current = this->RootSystem[i];
    currentRootReflection.MakeRootReflection(current, *this);
    out << "<tr><td>" << current.ToString() << "</td><td>" << rootSystemEpsCoords[i].ToStringLetterFormat("e") << "</td>"
    << "<td>" << HtmlRoutines::GetMathMouseHover(currentRootReflection.ToString()) << "</td>" << "</tr>";
  }
  out << "</table>";
  out << "Comma delimited list of roots: ";
  for (int i = 0; i < this->RootSystem.size; i ++) {
    out << this->RootSystem[i].ToString();
    if (i != this->RootSystem.size - 1) {
      out << ", ";
    }
  }
  out << outLatex.str();
  return out.str();
}

std::string WeylGroupData::ToString(FormatExpressions* theFormat) {
  MacroRegisterFunctionWithName("WeylGroup::ToString");
  std::stringstream out;
  out << "<br>Size: " << this->theGroup.theElements.size << "\n";
  out << "Number of Vectors: " << this->RootSystem.size << "\n";
  out << "<br>Half-sum positive roots:" << this->rho.ToString() << "\n";
  out << this->ToStringRootsAndRootReflections();
  out << "<br>Symmetric cartan: " << this->CartanSymmetric.ToString();
  if (this->flagCharTableIsComputed) {
    out << "<br>Character table: ";
    out << this->theGroup.PrettyPrintCharacterTable();
  }
  out << this->theGroup.ToStringConjugacyClasses(theFormat);
  out << this->theGroup.ToStringElements(theFormat);
  out << this->ToStringCppConjugacyClasses(theFormat);
  out << this->ToStringCppCharTable(theFormat);
  return out.str();
}

bool WeylGroupData::IsAddmisibleDynkinType(char candidateLetter, int n) {
  if (candidateLetter == 'A' && n > 0) {
    return true;
  }
  if (candidateLetter == 'B' && n > 1) {
    return true;
  }
  if (candidateLetter == 'C' && n > 1) {
    return true;
  }
  if (candidateLetter == 'D' && n > 3) {
    return true;
  }
  if (candidateLetter == 'E' && n > 5 && n < 9) {
    return true;
  }
  if (candidateLetter == 'F' && n == 4) {
    return true;
  }
  if (candidateLetter == 'G' && n == 2) {
    return true;
  }
  return false;
}

void WeylGroupData::ComputeEpsilonMatrix() {
  if (this->MatrixSendsSimpleVectorsToEpsilonVectors.IsZeroPointer()) {
    this->theDynkinType.GetEpsilonMatrix(this->MatrixSendsSimpleVectorsToEpsilonVectors.GetElement());
  }
}

void WeylGroupData::TransformToAdmissibleDynkinType(char inputLetter, int& outputRank) {
  if (inputLetter == 'G') {
    outputRank = 2;
  }
  if (inputLetter == 'F') {
    outputRank = 4;
  }
  if (inputLetter == 'E') {
    if (outputRank > 8) {
      outputRank = 8;
    }
    if (outputRank < 6) {
      outputRank = 6;
    }
  }
  if (inputLetter == 'C' || inputLetter == 'B') {
    if (outputRank < 2) {
      outputRank = 2;
    }
  }
  if (inputLetter == 'D') {
    if (outputRank < 4) {
      outputRank = 4;
    }
  }
}

void WeylGroupData::ComputeCoCartanSymmetricFromCartanSymmetric() {
  this->GetCoCartanSymmetric(this->CartanSymmetric, this->CoCartanSymmetric);
}

void WeylGroupData::MakeMeFromMyCartanSymmetric() {
  this->reset();
  this->GenerateRootSystem();
  DynkinDiagramRootSubalgebra theDynkinTypeComputer;
  Vectors<Rational> simpleBasis;
  simpleBasis.MakeEiBasis(this->CartanSymmetric.NumRows);
  theDynkinTypeComputer.ComputeDiagramTypeModifyInputRelative(simpleBasis, this->RootSystem, this->CartanSymmetric);
  theDynkinTypeComputer.GetDynkinType(this->theDynkinType);
  this->MakeFinalSteps();
}

void WeylGroupData::MakeFinalSteps() {
  this->ComputeCoCartanSymmetricFromCartanSymmetric();
}

void WeylGroupData::InitGenerators() {
  this->theGroup.generators.SetSize(this->GetDim());
  if (this->GetDim() == 0) {
    this->theGroup.generators.SetSize(1);
    this->theGroup.generators[0].MakeID(*this);
  }
  for (int i = 0; i < this->GetDim(); i ++) {
    this->theGroup.generators[i].MakeSimpleReflection(i, *this);
  }
}

void WeylGroupData::MakeFromDynkinType(const DynkinType& inputType) {
  this->reset();
  this->theDynkinType = inputType;
  this->theDynkinType.GetCartanSymmetric(this->CartanSymmetric);
  this->theDynkinType.GetCoCartanSymmetric(this->CoCartanSymmetric);
  this->MakeFinalSteps();

  // eventually, there will be formulas for all classical types
  List<char> letters;
  List<int> ranks;
  this->theDynkinType.GetLettersTypesMults(&letters, &ranks, nullptr);
  if (letters.size == 1) {
    if (ranks.size == 1) {
      if (letters[0] == 'A') {
        this->theGroup.ComputeIrreducibleRepresentationsWithFormulas =
        this->ComputeIrreducibleRepresentationsWithFormulasImplementation;
      }
    }
  }
  this->InitGenerators();
  this->theGroup.CheckInitialization();
}

void WeylGroupData::MakeFromDynkinTypeDefaultLengthKeepComponentOrder(const DynkinType& inputType) {
  this->reset();
  this->theDynkinType = inputType;
  this->theDynkinType.GetCartanSymmetricDefaultLengthKeepComponentOrder(this->CartanSymmetric);
  this->MakeFinalSteps();
}

void WeylGroupData::MakeArbitrarySimple(char WeylGroupLetter, int n, const Rational* firstCoRootLengthSquared) {
  DynkinType inputType;
  inputType.MakeSimpleType(WeylGroupLetter, n, firstCoRootLengthSquared);
  this->MakeFromDynkinType(inputType);
}

WeylGroupAutomorphisms::WeylGroupAutomorphisms() {
  this->flagAllOuterAutosComputed = false;
  this->flagDeallocated = false;
  this->flagOuterAutosGeneratorsComputed = false;
  this->theWeyl = nullptr;
}

WeylGroupAutomorphisms::~WeylGroupAutomorphisms() {
  this->flagDeallocated = true;
  this->theWeyl = nullptr;
}

void WeylGroupAutomorphisms::ComputeOuterAutos() {
  MacroRegisterFunctionWithName("WeylGroupAutomorphisms::ComputeOuterAutos");
  this->checkInitialization();
  if (this->flagAllOuterAutosComputed) {
    return;
  }
  this->ComputeOuterAutoGenerators();
  this->theOuterAutos.GenerateElements(0);
  this->flagAllOuterAutosComputed = true;
}

void WeylGroupAutomorphisms::ComputeOuterAutoGenerators() {
  MacroRegisterFunctionWithName("WeylGroupAutomorphisms::ComputeOuterAutoGenerators");
  if (this->flagOuterAutosGeneratorsComputed) {
    return;
  }
  this->checkInitialization();
  List<MatrixTensor<Rational> >& theGens = this->theOuterAutos.theGenerators;
  this->theWeyl->theDynkinType.GetOuterAutosGeneratorsActOnVectorColumn(theGens);
  for (int i = 0; i < theGens.size; i ++) {
    if (
      theGens[i].GetMinNumColsNumRows() != this->theWeyl->GetDim() ||
      theGens[i].GetMinNumCols() != this->theWeyl->GetDim() ||
      theGens[i].GetMinNumRows() != this->theWeyl->GetDim()
    ) {
      global.fatal << "Bad outer automorphisms, type " << this->theWeyl->theDynkinType.ToString() << "." << global.fatal;
    }
  }
  this->flagOuterAutosGeneratorsComputed = true;
}

void WeylGroupData::GetEpsilonCoordsWRTsubalgebra(
  Vectors<Rational>& generators, List<Vector<Rational> >& input, Vectors<Rational>& output
) {
  Matrix<Rational> basisChange;
  Matrix<Rational> tempMat;
  DynkinDiagramRootSubalgebra tempDyn;
  Vectors<Rational> simpleBasis;
  Vectors<Rational> coordsInNewBasis;
  simpleBasis = generators;
  tempDyn.ComputeDiagramTypeModifyInput(simpleBasis, *this);
  bool tempBool = true;
  if (generators.size == 0) {
    tempBool = false;
  }
  if (!tempBool) {
    output.SetSize(input.size);
    for (int i = 0; i < input.size; i ++) {
      output[i].MakeZero(0);
    }
    return;
  }
  basisChange.Resize(0, 0, true);
  for (int i = 0; i < tempDyn.SimpleComponentTypes.size; i ++) {
    DynkinSimpleType::GetEpsilonMatrix(
      tempDyn.SimpleComponentTypes[i].theLetter, tempDyn.SimpleComponentTypes[i].theRank, tempMat
    );
    basisChange.DirectSumWith(tempMat, Rational(0));
  }
  simpleBasis.AssignListList(tempDyn.SimpleBasesConnectedComponents);
  coordsInNewBasis.SetSize(input.size);
  for (int i = 0; i < input.size; i ++) {
    input[i].GetCoordsInBasiS(simpleBasis, coordsInNewBasis[i]);
  }
  basisChange.ActOnVectorsColumn(coordsInNewBasis, output);
}

void WeylGroupData::GetEpsilonCoords(const List<Vector<Rational> >& input, Vectors<Rational>& output) {
  if (&input == &output) {
    List<Vector<Rational> > newInput = input;
    this->GetEpsilonCoords(newInput, output);
    return;
  }
  output.SetSize(input.size);
  for (int i = 0; i < input.size; i ++) {
    this->GetEpsilonCoords(input[i], output[i]);
  }
}

LargeInteger WeylGroupData::GetSizeByFormulaImplementation(FiniteGroup<ElementWeylGroup>& G) {
  WeylGroupData* W = static_cast<WeylGroupData*>(G.specificDataPointer);
  W->CheckConsistency();
  return W->theDynkinType.GetWeylGroupSizeByFormula();
}

void WeylGroupData::GetWeylChamber(Cone& output) {
  Matrix<Rational> tempMat;
  tempMat = this->CartanSymmetric;
  tempMat.Invert();
  Vectors<Rational> tempRoots;
  tempRoots.AssignMatrixRows(tempMat);
  output.CreateFromVertices(tempRoots);
}

void WeylGroupData::GetFundamentalWeightsInSimpleCoordinates(Vectors<Rational>& output) {
  Matrix<Rational> tempMat;
  tempMat = this->CartanSymmetric;
  Rational tempRat;
  for (int i = 0; i < this->GetDim(); i ++) {
    tempRat = 2 / this->CartanSymmetric.elements[i][i];
    tempMat.RowTimesScalar(i, tempRat);
  }
  tempMat.Transpose();
  tempMat.Invert();
  output.AssignMatrixRows(tempMat);
}

void WeylGroupData::GetIntegralLatticeInSimpleCoordinates(Lattice& output) {
  output.basisRationalForm= this->CartanSymmetric;
  Vector<Rational> tempRoot;
  for (int i = 0; i < this->GetDim(); i ++) {
    tempRoot.MakeEi(this->GetDim(), i);
    output.basisRationalForm.RowTimesScalar(i, 2 / this->RootScalarCartanRoot(tempRoot, tempRoot));
  }
  output.basisRationalForm.Transpose();
  output.basisRationalForm.Invert();
  output.MakeFromMat(output.basisRationalForm);
  output.Reduce();
}

Rational WeylGroupData::GetKillingDivTraceRatio() {
  Rational result = 0;
  Rational tempRat;
  for (int i = 0; i < this->RootSystem.size; i ++) {
    tempRat = this->RootScalarCartanRoot(this->RootSystem[i], this->RootSystem[0]);
    result += tempRat * tempRat;
  }
  result /= this->RootScalarCartanRoot(this->RootSystem[0], this->RootSystem[0]);
  return result;
}

void WeylGroupData::GetLongestWeylElt(ElementWeylGroup& outputWeylElt) {
  this->ComputeRho(false);
  Vector<Rational> lowest = this->rho;
  Vectors<Rational> tempRoots;
  tempRoots.MakeEiBasis(this->GetDim());
  this->GetLowestElementInOrbit(lowest, &outputWeylElt, tempRoots, false, false);
}

void WeylGroupData::GetExtremeElementInOrbit(
  Vector<Rational>& inputOutput,
  ElementWeylGroup* outputWeylElt,
  Vectors<Rational>& bufferEiBAsis,
  bool findLowest,
  bool RhoAction,
  bool UseMinusRho,
  int* sign,
  bool* stabilizerFound
) {
  if (outputWeylElt != nullptr) {
    outputWeylElt->MakeID(*this);
  }
  if (sign != nullptr) {
    *sign = 1;
  }
  if (stabilizerFound != nullptr) {
    *stabilizerFound = false;
  }
  Rational theScalarProd;
  ElementWeylGroup eltSimplReflection;
  //  static int numTimesReflectionWasApplied = 0;
  for (bool found = true; found;) {
    found = false;
    for (int i = 0; i < this->GetDim(); i ++) {
      bool shouldApplyReflection = false;
      theScalarProd = this->RootScalarCartanRoot(inputOutput, bufferEiBAsis[i]);
      if (findLowest) {
        shouldApplyReflection = theScalarProd.IsPositive();
      } else {
        shouldApplyReflection = theScalarProd.IsNegative();
      }
      if (stabilizerFound != nullptr) {
        if (theScalarProd.IsEqualToZero()) {
          *stabilizerFound = true;
        }
      }
      if (shouldApplyReflection) {
        found = true;
        if (!RhoAction) {
          this->SimpleReflection<Rational>(i, inputOutput);
        } else if (!UseMinusRho) {
          this->SimpleReflectionRhoModified(i, inputOutput);
        } else {
          this->SimpleReflectionMinusRhoModified(i, inputOutput);
        }
        if (outputWeylElt != nullptr) {
          eltSimplReflection.MakeSimpleReflection(i, *this);
          *outputWeylElt = eltSimplReflection*(*outputWeylElt);
        }
        if (sign != nullptr) {
          *sign *= - 1;
        }
      }
    }
  }
}

LargeInteger WeylGroupAutomorphisms::GetOrbitSize(Vector<Rational>& theWeight) {
  MacroRegisterFunctionWithName("WeylGroupAutomorphisms::GetOrbitSize");
  this->checkInitialization();
  HashedList<Vector<Rational> > highestWeights;
  for (int i = 0; i < this->theOuterAutos.theElements.size; i ++) {
    Vector<Rational> candidate;
    this->theOuterAutos.theElements[i].ActOnVectorColumn(theWeight, candidate);
    this->theWeyl->RaiseToDominantWeight(candidate);
    highestWeights.AddOnTopNoRepetition(candidate);
  }
  return this->theWeyl->GetOrbitSize(theWeight) * highestWeights.size;
}

bool WeylGroupAutomorphisms::IsElementWeylGroupOrOuterAuto(const MatrixTensor<Rational>& input) {
  MacroRegisterFunctionWithName("WeylGroup::IsElementGroupOrOuterAuto");
  this->ComputeOuterAutos();
  Vector<Rational> theRhoImage;
  input.ActOnVectorColumn(this->theWeyl->rho, theRhoImage);
  ElementWeylGroup theElementCandidate;
  this->theWeyl->RaiseToDominantWeight(theRhoImage, nullptr, nullptr, &theElementCandidate);
  Matrix<Rational> theCandidateMat;
  MatrixTensor<Rational> theCandidateMatTensorForm, theCandidateMatWithOuterAuto;
  this->theWeyl->GetMatrixStandardRep(theElementCandidate, theCandidateMat);
  theCandidateMatTensorForm = theCandidateMat;
  for (int i = 0; i < this->theOuterAutos.theElements.size; i ++) {
    theCandidateMatWithOuterAuto = this->theOuterAutos.theElements[i];
    theCandidateMatWithOuterAuto *= theCandidateMatTensorForm;
    if (theCandidateMatWithOuterAuto == input) {
      return true;
    }
  }
  return false;
}

void WeylGroupData::GetMatrixReflection(Vector<Rational>& reflectionRoot, Matrix<Rational>& output) {
  Vectors<Rational> basis;
  int theDim = this->GetDim();
  basis.MakeEiBasis(theDim);
  for (int i = 0; i < theDim; i ++) {
    this->ReflectBetaWRTAlpha(reflectionRoot, basis[i], false, basis[i]);
  }
  output.AssignVectorsToRows(basis);
  output.Transpose();
}

void WeylGroupData::GetCoxeterPlane(Vector<double>& outputBasis1, Vector<double>& outputBasis2) {
  MacroRegisterFunctionWithName("WeylGroup::GetCoxeterPlane");
  this->ComputeRho(true);
  Vector<Rational> ZeroRoot;
  int theDimension = this->GetDim();
  if (theDimension < 2) {
    return;
  }
  ZeroRoot.MakeZero(theDimension);
  ElementWeylGroup tempElt;
  this->GetCoxeterElement(tempElt);
  Matrix<Rational> matCoxeterElt, tempMat;
  this->GetMatrixStandardRep(tempElt, matCoxeterElt);
  tempMat = matCoxeterElt;
  int coxeterNumber = this->RootSystem.LastObject()->SumCoords().NumShort + 1;
  for (int i = 0; i < coxeterNumber - 1; i ++) {
    tempMat.MultiplyOnTheLeft(matCoxeterElt);
  }
  CompleX<double> theEigenValue;
  theEigenValue.Re = FloatingPoint::Cos(2 * MathRoutines::Pi() / coxeterNumber);
  theEigenValue.Im = FloatingPoint::Sin(2 * MathRoutines::Pi() / coxeterNumber);
  Matrix<CompleX<double> > eigenMat;
  eigenMat.init(matCoxeterElt.NumRows, matCoxeterElt.NumCols);
  for (int i = 0; i < eigenMat.NumRows; i ++) {
    for (int j = 0; j < eigenMat.NumCols; j ++) {
      eigenMat.elements[i][j] = matCoxeterElt.elements[i][j].GetDoubleValue();
      if (i == j) {
        eigenMat.elements[i][i] -= theEigenValue;
      }
    }
  }
  List<Vector<CompleX<double> > > theEigenSpaceList;
  eigenMat.GetZeroEigenSpace(theEigenSpaceList);
  Vectors<CompleX<double> > theEigenSpace;
  outputBasis1.SetSize(theDimension);
  outputBasis2.SetSize(theDimension);
  theEigenSpace.operator=(theEigenSpaceList);
  DrawOperations tempDO;
  tempDO.initDimensions(theDimension);
  tempDO.GraphicsUnit = DrawOperations::GraphicsUnitDefault;
  theEigenSpace.operator=(theEigenSpaceList);
  for (int i = 0; i < theDimension; i ++) {
    for (int j = 0; j < theDimension; j ++) {
      tempDO.theBilinearForm.elements[i][j] = this->CartanSymmetric.elements[i][j].GetDoubleValue();
    }
  }
  if (theEigenSpace.size > 0) {
    if (coxeterNumber > 2) {
      for (int j = 0; j < theDimension; j ++) {
        outputBasis1[j] = theEigenSpace[0][j].Re;
        outputBasis2[j] = theEigenSpace[0][j].Im;
      }
    } else if (coxeterNumber == 1 && theEigenSpace.size > 1) {
      for (int j = 0; j < theDimension; j ++) {
        outputBasis1[j] = theEigenSpace[0][j].Re;
        outputBasis2[j] = theEigenSpace[1][j].Re;
      }
    }
    tempDO.ModifyToOrthonormalNoShiftSecond(outputBasis2, outputBasis1);
  }
}

void WeylGroupData::DrawRootSystem(
  DrawingVariables& outputDV,
  bool wipeCanvas,
  bool drawWeylChamber,
  Vector<Rational>* bluePoint,
  bool LabelDynkinDiagramVertices,
  Vectors<Rational>* predefinedProjectionPlane
) {
  MacroRegisterFunctionWithName("WeylGroup::DrawRootSystem");
  DrawOperations& output = outputDV.theBuffer;
  if (wipeCanvas) {
    output.init();
  }
  int theDimension = this->GetDim();
  if (theDimension == 1) {
    Vector<Rational> tempRoot, tempZero;
    tempZero.MakeZero(2);
    tempRoot.MakeEi(2, 0);
    std::string currentColor = "#ff00ff";
    for (int i = 0; i < 2; i ++) {
      output.drawLineBetweenTwoVectorsBufferRational(tempZero, tempRoot, "green", 1);
      output.drawCircleAtVectorBufferRational(tempRoot, currentColor, 2);
      tempRoot.Minus();
    }
    return;
  }
  this->ComputeRho(true);
  Vector<Rational> ZeroRoot;
  ZeroRoot.MakeZero(theDimension);
  output.initDimensions(theDimension);
  output.GraphicsUnit = DrawOperations::GraphicsUnitDefault;
  for (int i = 0; i < theDimension; i ++) {
    for (int j = 0; j < theDimension; j ++) {
      output.theBilinearForm.elements[i][j] = this->CartanSymmetric.elements[i][j].GetDoubleValue();
    }
  }
  Vector<double> tempRoot;
  Vectors<double>& theTwoPlane = output.BasisProjectionPlane;
  if (predefinedProjectionPlane == nullptr) {
    this->GetCoxeterPlane(theTwoPlane[0], theTwoPlane[1]);
  } else {
    predefinedProjectionPlane->GetVectorsDouble(theTwoPlane);
  }
  if (theTwoPlane.size != 2) {
    global.fatal << "Object theTwoPlane is supposed to be two-dimensional but it is instead of dimension: "
    << theTwoPlane.size << ". " << global.fatal;
  }
  Vectors<Rational> RootSystemSorted;
  RootSystemSorted = this->RootSystem;
  List<double> lengths;
  lengths.SetSize(RootSystemSorted.size);
  for (int i = 0; i < this->RootSystem.size; i ++) {
    tempRoot.SetSize(theDimension);
    for (int j = 0; j < theDimension; j ++) {
      tempRoot[j] = this->RootSystem[i][j].GetDoubleValue();
    }
    double Length1 = this->RootScalarCartanRoot(tempRoot, output.BasisProjectionPlane[0]);
    double Length2 = this->RootScalarCartanRoot(tempRoot, output.BasisProjectionPlane[1]);
    lengths[i] = FloatingPoint::Sqrt(Length1 * Length1 + Length2 * Length2);
  }
  for (int i = 0; i < RootSystemSorted.size; i ++) {
    for (int j = i; j < RootSystemSorted.size; j ++) {
      if (lengths[i] < lengths[j]) {
        MathRoutines::swap(lengths[i], lengths[j]);
        MathRoutines::swap(RootSystemSorted[i], RootSystemSorted[j]);
      }
    }
  }
  Vector<Rational> differenceRoot;
  differenceRoot = RootSystemSorted[0] - RootSystemSorted[1];
  Rational minLength = this->RootScalarCartanRoot(differenceRoot, differenceRoot);
  for (int i = 2; i < RootSystemSorted.size; i ++) {
    differenceRoot = RootSystemSorted[0] - RootSystemSorted[i];
    if (minLength > this->RootScalarCartanRoot(differenceRoot, differenceRoot)) {
      minLength = this->RootScalarCartanRoot(differenceRoot, differenceRoot);
    }
  }
  Rational tempRat;
  if (bluePoint != nullptr) {
    output.drawCircleAtVectorBufferRational(*bluePoint, "blue", 5);
    output.drawCircleAtVectorBufferRational(*bluePoint, "blue", 4);
    output.drawCircleAtVectorBufferRational(*bluePoint, "blue", 3);
  }
  if (drawWeylChamber) {
    Cone theWeylChamber;
    this->GetWeylChamber(theWeylChamber);
    FormatExpressions tempFormat;
    theWeylChamber.DrawMeProjective(nullptr, false, outputDV, tempFormat);
  }
  output.centerX = 300;
  output.centerY = 300;
  for (int i = 0; i < RootSystemSorted.size; i ++) {
    output.drawLineBetweenTwoVectorsBufferRational(ZeroRoot, RootSystemSorted[i], "green", 1);
    output.drawCircleAtVectorBufferRational(RootSystemSorted[i], "#ff00ff", 2);
    for (int j = i + 1; j < RootSystemSorted.size; j ++) {
      differenceRoot = RootSystemSorted[i] - RootSystemSorted[j];
      tempRat = this->RootScalarCartanRoot(differenceRoot, differenceRoot);
      if (minLength == tempRat) {
        output.drawLineBetweenTwoVectorsBufferRational(RootSystemSorted[i], RootSystemSorted[j], "blue", 1);
      }
    }
  }
  Vector<Rational> tempRootRat;
  Vectors<Rational> epsNotationSimpleBasis;
  epsNotationSimpleBasis.MakeEiBasis(theDimension);
  this->GetEpsilonCoords(epsNotationSimpleBasis, epsNotationSimpleBasis);
  for (int i = 0; i < theDimension; i ++) {
    tempRootRat.MakeEi(theDimension, i);
    output.drawCircleAtVectorBufferRational(tempRootRat, "red", 1);
    output.drawCircleAtVectorBufferRational(tempRootRat, "red", 3);
    output.drawCircleAtVectorBufferRational(tempRootRat, "red", 4);
    if (LabelDynkinDiagramVertices) {
      Vector<Rational>& current = epsNotationSimpleBasis[i];
      output.drawTextAtVectorBufferRational(tempRootRat, current.ToStringLetterFormat("e"), "black", 10);
    }
  }
  Vectors<double> highlightGroup;
  List<std::string> highlightLabels;
  highlightGroup.SetSize(2);
  highlightLabels.SetSize(2);
  for (int i = 0; i < this->RootsOfBorel.size; i ++) {
    highlightGroup[0] = this->RootsOfBorel[i].GetVectorDouble();
    highlightGroup[1] = - this->RootsOfBorel[i].GetVectorDouble();
    highlightLabels[0] = this->RootsOfBorel[i].ToString();
    highlightLabels[1] = (- this->RootsOfBorel[i]).ToString();
    output.drawHighlightGroup(highlightGroup, highlightLabels, "gray", 5);
  }
  std::stringstream tempStream;
  tempStream << this->theDynkinType.GetWeylGroupName();
  if (this->GetDim() == 2 && predefinedProjectionPlane != nullptr) {
    theTwoPlane[1][0] = 1;
    theTwoPlane[1][1] = 0;
    theTwoPlane[0][0] = 0;
    theTwoPlane[0][1] = 1;
    outputDV.theBuffer.ModifyToOrthonormalNoShiftSecond(theTwoPlane[0], theTwoPlane[1]);
  }
  output.drawTextBuffer(
    0,
    0,
    tempStream.str(),
    10,
    static_cast<int>(HtmlRoutines::RedGreenBlue(0, 0, 0)),
    DrawingVariables::TextStyleNormal
  );
}

std::string WeylGroupData::GenerateWeightSupportMethoD1(
  Vector<Rational>& highestWeightSimpleCoords, Vectors<Rational>& outputWeightsSimpleCoords, int upperBoundWeights
) {
  HashedList<Vector<Rational> > theDominantWeights;
  (void) upperBoundWeights;//portable way to avoid non-used parameter warning.
  double upperBoundDouble = 100000 / (Rational(this->theGroup.GetSize())).GetDoubleValue();
  int upperBoundInt = MathRoutines::Maximum(static_cast<int>(upperBoundDouble), 10000);
  //int upperBoundInt = 10000;
  Vector<Rational> highestWeightTrue = highestWeightSimpleCoords;
  this->RaiseToDominantWeight(highestWeightTrue);
  std::stringstream out;
  if (highestWeightTrue != highestWeightSimpleCoords) {
    out << "<br>The input weight is not highest... using the highest weight in the same orbit instead. "
    << "Your input in simple coordinates was: " << highestWeightSimpleCoords.ToString() << ".<br> ";
  }
  out << "The highest weight in simple coordinates is: " << highestWeightTrue.ToString() << ".<br>";
  std::string tempS;
  bool isTrimmed = !this->GetAlLDominantWeightsHWFDIM(highestWeightSimpleCoords, theDominantWeights, upperBoundInt, &tempS);
  out << tempS << "<br>";
  if (isTrimmed) {
    out << "Trimmed the # of dominant weights - upper bound is " << upperBoundInt << ". <br>";
  } else {
    out << "Number of (non-strictly) dominant weights: " << theDominantWeights.size << "<br>";
  }
  HashedList<Vector<Rational> > finalWeights;
  int estimatedNumWeights = static_cast<int>((Rational(this->theGroup.GetSize())).GetDoubleValue() * theDominantWeights.size);
  estimatedNumWeights = MathRoutines::Minimum(10000, estimatedNumWeights);
  finalWeights.Reserve(estimatedNumWeights);
  finalWeights.SetHashSizE(estimatedNumWeights);
  Vectors<Rational> dominantWeightsNonHashed;
  dominantWeightsNonHashed = (theDominantWeights);
  this->GenerateOrbit(dominantWeightsNonHashed, false, finalWeights, false, 0, nullptr, 10000);
  if (finalWeights.size >= 10000) {
    out << "Did not generate all weights of the module due to RAM limits. ";
    if (!isTrimmed) {
      out << "However, all dominant weights were computed and are drawn. ";
    }
    out << "<br>";
  }
  if (!isTrimmed && finalWeights.size < 10000) {
    out << "All weights were computed and are drawn. <br>";
  }
  outputWeightsSimpleCoords = finalWeights;
  return out.str();
}

bool WeylGroupData::IsEigenSpaceGeneratorCoxeterElement(Vector<Rational>& input) {
  ElementWeylGroup tempElt;
  this->GetCoxeterElement(tempElt);
  Matrix<Rational> matCoxeterElt;
  this->GetMatrixStandardRep(tempElt, matCoxeterElt);
  Vector<Rational> tempRoot = input;
  for (int i = 0; i < this->GetDim(); i ++) {
    matCoxeterElt.ActOnVectorColumn(tempRoot);
  }
  return tempRoot == input;
}

Rational WeylGroupData::GetLongestRootLengthSquared() const {
  Rational result;
  result = this->CartanSymmetric(0, 0);
  for (int i = 1; i < this->CartanSymmetric.NumRows; i ++) {
    result = MathRoutines::Maximum(result, this->CartanSymmetric(i, i));
  }
  return result;
}

bool WeylGroupData::IsElementWeylGroup(const MatrixTensor<Rational>& input) {
  MacroRegisterFunctionWithName("WeylGroup::IsElementWeylGroup");
  Vector<Rational> theRhoImage;
  input.ActOnVectorColumn(this->rho, theRhoImage);
  ElementWeylGroup theElementCandidate;
  this->RaiseToDominantWeight(theRhoImage, nullptr, nullptr, &theElementCandidate);
  Matrix<Rational> theCandidateMat, inputMat;
  input.GetMatrix(inputMat, this->GetDim());
  this->GetMatrixStandardRep(theElementCandidate, theCandidateMat);
  return theCandidateMat == inputMat;
}

bool WeylGroupData::ContainsARootNonStronglyPerpendicularTo(Vectors<Rational>& theVectors, Vector<Rational>& input) {
  for (int i = 0; i < this->theGroup.theElements.size; i ++) {
    if (this->IsARoot(theVectors[i] + input)) {
      return true;
    }
  }
  return false;
}

void WeylGroupData::GetMatrixStandardRep(const ElementWeylGroup& input, Matrix<Rational>& outputMatrix) const {
  Vector<Rational> tempRoot;
  int theDim = this->CartanSymmetric.NumRows;
  outputMatrix.init(theDim, theDim);
  for (int i = 0; i < theDim; i ++) {
    tempRoot.MakeEi(theDim, i);
    this->ActOn(input, tempRoot, tempRoot);
    for (int j = 0; j < theDim; j ++) {
      outputMatrix(j, i) = tempRoot[j];
    }
  }
}

int WeylGroupData::NumRootsConnectedTo(Vectors<Rational>& theVectors, Vector<Rational>& input) {
  int result = 0;
  for (int i = 0; i < this->theGroup.theElements.size; i ++) {
    if (!Vector<Rational>::ScalarProduct(theVectors[i], input, this->CartanSymmetric).IsEqualToZero()) {
      result ++;
    }
  }
  return result;
}

void WeylGroupData::ComputeRho(bool Recompute) {
  if (this->RootSystem.size == 0 || Recompute) {
    this->GenerateRootSystem();
  }
  this->rho.MakeZero(this->CartanSymmetric.NumRows);
  for (int i = 0; i < this->RootSystem.size; i ++) {
    if (this->RootSystem[i].IsPositiveOrZero()) {
      this->rho += RootSystem[i];
    }
  }
  for (int i = 0; i < this->CartanSymmetric.NumCols; i ++) {
    this->rho[i].DivideByInteger(2);
  }
  this->flagFundamentalToSimpleMatricesAreComputed = false;
}

std::string SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::ElementToStringFromLayersAndArrows(
  List<List<List<int> > >& arrows, List<List<int> >& Layers, int GraphWidth, bool useAmbientIndices
) {
  MacroRegisterFunctionWithName("SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::ElementToStringFromLayersAndArrows");
  this->CheckInitialization();
  std::stringstream out;
  List<int> DisplayIndicesSimpleGenerators;
  if (!useAmbientIndices) {
    DisplayIndicesSimpleGenerators.SetSize(this->simpleRootsInner.size);
    for (int i = 0; i < this->simpleRootsInner.size; i ++) {
      DisplayIndicesSimpleGenerators[i] = this->AmbientWeyl->RootsOfBorel.GetIndex(this->simpleRootsInner[i]) + 1;
    }
  }
  out << "\\xymatrix{";
  bool GraphWidthIsOdd = ((GraphWidth % 2) != 0);
  if (!GraphWidthIsOdd) {
    GraphWidth ++;
  }
  for (int i = 0; i < Layers.size; i ++) {
    int currentRowOffset = (GraphWidth - Layers[i].size) / 2;
    int nextRowOffset = - 1;
    if (i < Layers.size - 1) {
      nextRowOffset = (GraphWidth - Layers[i + 1].size) / 2;
    }
    for (int j = 0; j < currentRowOffset; j ++) {
      out << "&";
    }
    for (int j = 0; j < Layers[i].size; j ++) {
      if (!useAmbientIndices) {
        out << this->allElements[Layers[i][j]].ToString(nullptr);
      } else {
        out << this->RepresentativesQuotientAmbientOrder[Layers[i][j]].ToString();
      }
      int currentOffset = j + currentRowOffset;
      if (Layers[i].size % 2 == 0) {
        if (currentOffset >= GraphWidth / 2) {
          currentOffset ++;
        }
      }
      for (int k = 0; k < arrows[i][j].size; k ++) {
        out << " \\ar[d";
        int indexInLayer = Layers[i + 1].GetIndex(arrows[i][j][k]);
        if (indexInLayer == - 1) {
          global.fatal << "Negative index in layer ... " << global.fatal;
        }
        int nextOffset = indexInLayer+nextRowOffset;
        if (Layers[i + 1].size % 2 == 0) {
          if (nextOffset >= GraphWidth / 2) {
            nextOffset ++;
          }
        }
        int actualOffset = - currentOffset +nextOffset;
        for (int l = 0; l < actualOffset; l ++) {
          out << "r";
        }
        for (int l = 0; l > actualOffset; l --) {
          out << "l";
        }
        out << "]";
      }
      out << " & ";
      if (Layers[i].size % 2 == 0 && j == Layers[i].size / 2 - 1) {
        out << " & ";
      }
    }
    out << " \\\\\n";
  }
  out << "}";
  return out.str();
}

std::string SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::ElementToStringBruhatGraph() {
  MacroRegisterFunctionWithName("SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::ElementToStringBruhatGraph");
  this->CheckInitialization();
  if (this->allElements.size < 1) {
    return "Error, non-initialized group";
  }
  if (this->allElements.size == 1) {
    return "id";
  }
  List<List<List<int> > > arrows;
  List<List<int> > Layers;
  Vector<Rational> tempRoot;
  Layers.Reserve(this->allElements.size);
  int GraphWidth = 1;
  int oldLayerElementLength = - 1;
  for (int i = 0; i < this->allElements.size; i ++) {
    const ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms& currentElement = this->allElements[i];
    if (currentElement.generatorsLastAppliedFirst.size != oldLayerElementLength) {
      Layers.SetSize(Layers.size + 1);
      oldLayerElementLength = currentElement.generatorsLastAppliedFirst.size;
    }
    Layers.LastObject()->AddOnTop(i);
    GraphWidth = MathRoutines::Maximum(GraphWidth, Layers.LastObject()->size);
  }
  HashedList<Vector<Rational> > orbit;
  orbit.Reserve(this->allElements.size);
  for (int i = 0; i < this->allElements.size; i ++) {
    this->ActByNonSimpleElement(i, this->AmbientWeyl->rho, tempRoot);
    orbit.AddOnTop(tempRoot);
  }
  arrows.SetSize(Layers.size);
  for (int i = 0; i < Layers.size; i ++) {
    arrows[i].SetSize(Layers[i].size);
    for (int j = 0; j < Layers[i].size; j ++) {
      int indexOther = Layers[i][j];
      for (int k = 0; k < this->simpleRootsInner.size; k ++) {
        this->AmbientWeyl->ReflectBetaWRTAlpha(this->simpleRootsInner[k], orbit[indexOther], false, tempRoot);
        int index = orbit.GetIndex(tempRoot);
        if (index == - 1) {
          global.fatal << "Negative index not allowed. " << global.fatal;
        }
        const ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms& currentElement = this->allElements[index];
        if (currentElement.generatorsLastAppliedFirst.size > this->allElements[indexOther].generatorsLastAppliedFirst.size) {
          arrows[i][j].AddOnTop(index);
        }
      }
    }
  }
  return this->ElementToStringFromLayersAndArrows(arrows, Layers, GraphWidth, false);
}

void SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::ToString(std::string& output, bool displayElements) {
  MacroRegisterFunctionWithName("SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::ToString");
  this->CheckInitialization();
  std::stringstream out, head, head2;
  List<int> DisplayIndicesSimpleGenerators;
  DisplayIndicesSimpleGenerators.SetSize(this->simpleRootsInner.size);
  FormatExpressions latexFormat;
  latexFormat.flagUseHTML = false;
  latexFormat.flagUseLatex = true;
  bool isGood = true;
  for (int i = 0; i < this->simpleRootsInner.size; i ++) {
    DisplayIndicesSimpleGenerators[i] = this->AmbientWeyl->RootsOfBorel.GetIndex(this->simpleRootsInner[i]) + 1;
    if (DisplayIndicesSimpleGenerators[i] == 0) {
      isGood = false;
      break;
    }
  }
  if (!isGood) {
    for (int i = 0; i < this->simpleRootsInner.size; i ++) {
      DisplayIndicesSimpleGenerators[i] = i + 1;
    }
  }
  DynkinDiagramRootSubalgebra tempDyn;
  tempDyn.AmbientRootSystem = this->AmbientWeyl->RootSystem;
  tempDyn.AmbientBilinearForm = this->AmbientWeyl->CartanSymmetric;
  tempDyn.ComputeDiagramInputIsSimple(this->simpleRootsInner);
  out << "Dynkin diagram & subalgebra of root subsystem generated by the given root: "
  << tempDyn.ToString();
  out << "<br>Simple roots:\n<br>\n ";
  head << "\\begin{array}{rcl}";
  for (int i = 0; i < this->simpleRootsInner.size; i ++) {
    head << "\n\\eta_{" << DisplayIndicesSimpleGenerators[i] << "}&= &" << this->simpleRootsInner[i].ToString() << "\\\\";
  }
  head << "\\end{array}";
  out << HtmlRoutines::GetMathMouseHover(head.str());
  if (this->ExternalAutomorphisms.size > 0) {
    out << "<br>Outer automorphisms: \n";
    Matrix<Rational> tempMat;
    head2 << "\\begin{array}{rcl}";
    for (int i = 0; i < this->ExternalAutomorphisms.size; i ++) {
      tempMat.AssignVectorsToRows(this->ExternalAutomorphisms[i]);
      tempMat.Transpose();
      head2 << "a_{" << i + 1 << "}&= &" << tempMat.ToString(&latexFormat) << "\\\\";
    }
    head2 << "\\end{array}";
    out << HtmlRoutines::GetMathMouseHover(head2.str());
  }
  out << "<br>Half sum of the positive roots: " << this->GetRho().ToString();
  out << "<br>Roots of Borel (" << this->RootsOfBorel.size << " total): ";
  for (int i = 0; i < this->RootsOfBorel.size; i ++) {
    out << this->RootsOfBorel[i].ToString();
  }
  if (displayElements) {
    std::stringstream body;
    out << "<br>The elements of the automorphisms of the Weyl group of the subgroup written with minimal # of generators:<br>";
    body << "\\begin{array}{l}";
    for (int i = 0; i < this->allElements.size; i ++) {
      const ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms& currentElt = this->allElements[i];
      body << currentElt.ToString(nullptr) << "\\\\";
    }
    body << "\\end{array}";
    out << HtmlRoutines::GetMathMouseHover(body.str());
  }
  output = out.str();
}

bool SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::MakeParabolicFromSelectionSimpleRoots(
  WeylGroupData& inputWeyl, const Selection& ZeroesMeanSimpleRootSpaceIsInParabolic, int UpperLimitNumElements
) {
  MacroRegisterFunctionWithName("SubgroupWeylGroupOLD::MakeParabolicFromSelectionSimpleRoots");
  this->AmbientWeyl = &inputWeyl;
  this->CheckInitialization();
  Vectors<Rational> selectedRoots;
  selectedRoots.Reserve(ZeroesMeanSimpleRootSpaceIsInParabolic.MaxSize - ZeroesMeanSimpleRootSpaceIsInParabolic.CardinalitySelection);
  if (this->AmbientWeyl->GetDim() != ZeroesMeanSimpleRootSpaceIsInParabolic.MaxSize) {
    global.fatal << "This is a programming error: parabolic selection selects out of "
    << ZeroesMeanSimpleRootSpaceIsInParabolic.MaxSize
    << " elements while the Weyl group is of rank " << this->AmbientWeyl->GetDim() << ". " << global.fatal;
  }
  for (int i = 0; i < ZeroesMeanSimpleRootSpaceIsInParabolic.MaxSize; i ++) {
    if (!ZeroesMeanSimpleRootSpaceIsInParabolic.selected[i]) {
      selectedRoots.SetSize(selectedRoots.size + 1);
      selectedRoots.LastObject()->MakeEi(inputWeyl.GetDim(), i);
    }
  }
  List<Vectors<Rational> > tempRootsCol;
  return this->ComputeSubGroupFromGeneratingReflections(&selectedRoots, &tempRootsCol, UpperLimitNumElements, true);
}

std::string SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::ElementToStringCosetGraph() {
  MacroRegisterFunctionWithName("SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::ElementToStringCosetGraph");
  this->CheckInitialization();
  if (this->allElements.size < 1) {
    return "Error, non-initialized group";
  }
  if (this->allElements.size == 1) {
    return "id";
  }
  List<List<List<int> > > arrows;
  List<List<int> > Layers;
  Vector<Rational> tempRoot;
  Layers.Reserve(this->RepresentativesQuotientAmbientOrder.size);
  int GraphWidth = 1;
  int oldLayerElementLength = - 1;
  for (int i = 0; i < this->RepresentativesQuotientAmbientOrder.size; i ++) {
    if (this->RepresentativesQuotientAmbientOrder[i].generatorsLastAppliedFirst.size != oldLayerElementLength) {
      Layers.SetSize(Layers.size + 1);
      oldLayerElementLength = this->RepresentativesQuotientAmbientOrder[i].generatorsLastAppliedFirst.size;
    }
    Layers.LastObject()->AddOnTop(i);
    GraphWidth =MathRoutines::Maximum(GraphWidth, Layers.LastObject()->size);
  }
  for (int i = 0; i < this->RepresentativesQuotientAmbientOrder.size; i ++) {
    tempRoot = this->AmbientWeyl->rho;
    this->AmbientWeyl->ActOnRootByGroupElement(
      this->AmbientWeyl->theGroup.theElements.GetIndex(this->RepresentativesQuotientAmbientOrder[i]), tempRoot, false, false
    );
  }
  arrows.SetSize(Layers.size);
  for (int i = 0; i < Layers.size; i ++) {
    arrows[i].SetSize(Layers[i].size);
    for (int j = 0; j < Layers[i].size; j ++) {
      for (int k = 0; k < this->RepresentativesQuotientAmbientOrder.size; k ++) {
        if (this->AmbientWeyl->LeftIsHigherInBruhatOrderThanRight(
          this->RepresentativesQuotientAmbientOrder[k], this->RepresentativesQuotientAmbientOrder[Layers[i][j]]
        )) {
          if (
            this->RepresentativesQuotientAmbientOrder[Layers[i][j]].generatorsLastAppliedFirst.size ==
            this->RepresentativesQuotientAmbientOrder[k].generatorsLastAppliedFirst.size - 1
          ) {
            arrows[i][j].AddOnTop(k);
          }
        }
      }
    }
  }
  return this->ElementToStringFromLayersAndArrows(arrows, Layers, GraphWidth, true);
}

void SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::FindQuotientRepresentatives(int UpperLimit) {
  MacroRegisterFunctionWithName("SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::FindQuotientRepresentatives");
  this->CheckInitialization();
  this->AmbientWeyl->theGroup.ComputeAllElements(UpperLimit);
  Vector<Rational> image1;
  this->RepresentativesQuotientAmbientOrder.size = 0;
  this->RepresentativesQuotientAmbientOrder.Reserve(this->AmbientWeyl->theGroup.theElements.size);
  for (int i = 0; i < this->AmbientWeyl->theGroup.theElements.size; i ++) {
    image1 = this->AmbientWeyl->rho;
    this->AmbientWeyl->ActOnRootByGroupElement(i, image1, false, false);
    bool isGood = true;
    for (int j = 0; j < this->simpleRootsInner.size; j ++) {
      if (this->AmbientWeyl->RootScalarCartanRoot(image1, this->simpleRootsInner[j]).IsNegative()) {
        isGood = false;
        break;
      }
    }
    if (isGood) {
      this->RepresentativesQuotientAmbientOrder.AddOnTop(this->AmbientWeyl->theGroup.theElements[i]);
    }
  }
}

bool SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::DrawContour(
  const Vector<Rational>& highestWeightSimpleCoord,
  DrawingVariables& theDV,
  const std::string& color,
  int UpperBoundVertices
) {
  MacroRegisterFunctionWithName("SubgroupWeylGroupOLD::DrawContour");
  HashedList<Vector<Rational> > theOrbit;
  theOrbit.AddOnTop(highestWeightSimpleCoord);
  WeylGroupData& theWeyl = *this->AmbientWeyl;
  Vector<Rational> tempRoot;
  for (int i = 0; i < theOrbit.size; i ++) {
    for (int j = 0; j < this->simpleRootsInner.size; j ++) {
      tempRoot = theOrbit[i];
      theWeyl.ReflectBetaWRTAlpha(this->simpleRootsInner[j], tempRoot, false, tempRoot);
      if (theOrbit.AddOnTopNoRepetition(tempRoot)) {
        theDV.drawLineBetweenTwoVectorsBufferRational(theOrbit[i], tempRoot, color, 1);
      }
      if (theOrbit.size > UpperBoundVertices) {
        return false;
      }
    }
  }
  return true;
}

SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms() {
  this->AmbientWeyl = nullptr;
  this->flagDeallocated = false;
}

SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::~SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms() {
  this->AmbientWeyl = nullptr;
  this->flagDeallocated = true;
}

bool SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::CheckInitialization() {
  //if (this == 0)
  //  global.fatal << "Subgroup of Weyl Group has 0 this pointer. " << global.fatal;
  if (this->AmbientWeyl == nullptr) {
    global.fatal << "Use of non-initialized subgroup of Weyl Group. " << global.fatal;
  }
  if (this->flagDeallocated) {
    global.fatal << "Use after free of subgroup of a Weyl group. " << global.fatal;
  }
  if (this->AmbientWeyl->flagDeallocated) {
    global.fatal << "Use after free of owner Weyl groups in a subgroup. " << global.fatal;
  }
  return true;
}

void SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::ComputeRootSubsystem() {
  MacroRegisterFunctionWithName("SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::ComputeRootSubsystem");
  this->RootSubsystem.Clear();
  this->RootSubsystem.AddOnTop(this->simpleRootsInner);
  this->RootSubsystem.SetExpectedSize(100);
  Vector<Rational> currentRoot;
  for (int i = 0; i < this->RootSubsystem.size; i ++) {
    for (int j = 0; j < this->simpleRootsInner.size; j ++) {
      currentRoot = this->RootSubsystem[i];
      this->AmbientWeyl->ReflectBetaWRTAlpha(this->simpleRootsInner[j], currentRoot, false, currentRoot);
      this->RootSubsystem.AddOnTopNoRepetition(currentRoot);
    }
  }
  Vectors<Rational> tempRoots;
  tempRoots = this->RootSubsystem;
  tempRoots.QuickSortAscending();
  this->RootSubsystem = tempRoots;
  if (this->RootSubsystem.size % 2 != 0) {
    global.fatal << "This is a programming error. "
    << "I am getting that the number of weights of a root system is odd. "
    << "The generating set of simple weights is "
    << this->simpleRootsInner.ToString() << ", and the "
    << "generated weight subsystem is " << tempRoots.ToString() << global.fatal;
  }
  int numPosRoots = this->RootSubsystem.size / 2;
  this->RootsOfBorel.SetSize(numPosRoots);
  for (int i = 0; i < numPosRoots; i ++) {
    this->RootsOfBorel[i] = this->RootSubsystem[i + numPosRoots];
  }
}

void KLpolys::WriteKLCoeffsToFile(std::fstream& output, List<int>& KLcoeff, int TopIndex) {
  output.clear();
  output << "Top_index: " << TopIndex << "\n";
  std::string tempS;
  this->KLcoeffsToString(KLcoeff,  tempS);
  output << tempS;
}

int KLpolys::ReadKLCoeffsFromFile(std::fstream& input, List<int>& output) {
  std::string tempS;
  int TopIndex;
  input >> tempS >> TopIndex;
  output.SetSize(this->size);
  for (int i = 0; i < this->size; i ++) {
    input >> tempS >> output[i];
  }
  return TopIndex;
}

void KLpolys::KLcoeffsToString(List<int>& theKLCoeffs, std::string& output) {
  std::stringstream out;
  for (int i = 0; i < theKLCoeffs.size; i ++) {
    out << i << ".  " << theKLCoeffs[i] << "\n";
  }
  output = out.str();
}

void KLpolys::initTheMults() {
  this->TheMultiplicities.SetSize(this->size);
  this->Explored.SetSize(this->size);
  for (int i = 0; i < this->size; i ++) {
    this->TheMultiplicities[i] = 0;
    this->Explored[i] = false;
  }
  this->NextToExplore = 0;
  this->LowestNonExplored = 0;
}

void KLpolys::Check() {
  for (int i = 0; i < this->size; i ++) {
    this->Compute(i);
    for (int j = 0; j < this->size; j ++) {
      if (this->TheMultiplicities[i] != 0 && this->TheMultiplicities[i] != 1 && this->TheMultiplicities[i] != - 1) {
        this->ComputeDebugString();
      }
    }
  }
}

void KLpolys::Compute(int x) {
  this->initTheMults();
  this->TheMultiplicities[x] = 1;
  while (this->NextToExplore != - 1) {
    for (int i = 0; i < this->BruhatOrder[this->NextToExplore].size; i ++) {
      int a = this->BruhatOrder[this->NextToExplore][i];
      this->TheMultiplicities[a] -= this->TheMultiplicities[this->NextToExplore];
    }
    this->Explored[this->NextToExplore] = true;
    this->FindNextToExplore();
  }
}

void KLpolys::FindNextToExplore() {
  bool foundNonExplored = false;
  for (int i = this->LowestNonExplored; i < this->size; i ++) {
    if (!this->Explored[i]) {
      if (!foundNonExplored) {
        this->LowestNonExplored = i;
        foundNonExplored = true;
      }
      if (this->IsMaxNonEplored(i)) {
        this->NextToExplore = i;
        return;
      }
    }
  }
  this->NextToExplore = - 1;
}

bool KLpolys::IsMaxNonEplored(int index) {
  for (int i = this->LowestNonExplored; i < this->size; i ++) {
    if (!this->Explored[i] && i != index) {
      Vector<Rational> tempRoot;
      tempRoot = (*this)[i];
      tempRoot -= (*this)[index];
      if (tempRoot.IsPositiveOrZero()) {
        return false;
      }
    }
  }
  return true;
}

std::string KLpolys::ToString(FormatExpressions* theFormat) {
  MacroRegisterFunctionWithName("KLpolys::ToString");
  std::stringstream out;
  bool useHtml = false;
  if (theFormat != nullptr) {
    useHtml = theFormat->flagUseHTML;
  }
  if (!useHtml) {
    out << "Next to explore: " << this->NextToExplore << "<br>\n Orbit of rho:<br>\n";
    for (int i = 0; i < this->size; i ++) {
      out << this->TheObjects[i].ToString() << "   :  " << this->TheMultiplicities[i];
      if (this->Explored[i]) {
        out << " Explored<br>\n";
      } else {
        out << " not Explored<br>\n";
      }
    }
    out << "Bruhat order:<br>\n";
    for (int i = 0; i < this->size; i ++) {
      out << i << ".   ";
      for (int j = 0; j < this->BruhatOrder[i].size; j ++) {
        out << this->BruhatOrder[i][j] << ", ";
      }
      out << "<br>\n";
    }
  }
  out << "R Polynomials:<br>" << this->RPolysToString(theFormat);
  if (this->theKLcoeffs.size == this->TheWeylGroup->theGroup.theElements.size) {
    out << "Kazhdan-Lusztig Polynomials:<br>" << this->KLPolysToString(theFormat);
    out << "Kazhdan-Lusztig coefficients; the (w_1,w_2)  coefficient is defined as the multiplicity of "
    << HtmlRoutines::GetMathSpanPure("L_{w_2 \\cdot \\lambda}")
    << " in " <<  HtmlRoutines::GetMathSpanPure(" M_{w_1\\cdot \\lambda }  ")
    << " where \\cdot stands for the \\rho-modified action"
    << " of the Weyl group, \\lambda is a dominant integral weight, M_{\\lambda} stands for Verma module "
    << "of highest weight \\lambda, L_\\lambda stands for irreducible highest "
    << "weight of highest weight \\lambda: <br><table border =\"1\"><tr><td>Weyl elt.</td>";
    for (int i = 0; i < this->TheWeylGroup->theGroup.theElements.size; i ++) {
      out << "<td>" << this->TheWeylGroup->theGroup.theElements[i].ToString() << "</td>";
    }
    out << "</tr>";
    for (int i = 0; i < this->TheWeylGroup->theGroup.theElements.size; i ++) {
      if (this->theKLPolys[i].size > 0) {
        out << "<tr>" << "<td>" << this->TheWeylGroup->theGroup.theElements[i].ToString()  << "</td>";
        for (int j = 0; j < this->theKLcoeffs[i].size; j ++) {
          out << "<td>" << theKLcoeffs[i][j].ToString() << "</td>";
        }
        out << "</tr>";
      }
    }
    out << "</table>";
  } else {
    out << "KL polys not computed";
  }
  return out.str();
}

void KLpolys::ComputeDebugString() {
  this->DebugString = this->ToString();
}

void KLpolys::GeneratePartialBruhatOrder() {
  MacroRegisterFunctionWithName("KLpolys::GeneratePartialBruhatOrder");
  int theDimension = this->TheWeylGroup->CartanSymmetric.NumRows;
  Vector<Rational> ZeroRoot;
  ZeroRoot.MakeZero(theDimension);
  this->BruhatOrder.SetSize(this->size);
  this->InverseBruhatOrder.SetSize(this->size);
  this->SimpleReflectionsActionList.SetSize(this->size);
  this->ComputeDebugString();
  for (int i = 0; i < this->size; i ++) {
    this->SimpleReflectionsActionList[i].Reserve(theDimension);
    for (int j = 0; j < theDimension; j ++) {
      Vector<Rational> tempRoot, tempRoot2;
      tempRoot = (*this)[i];
      tempRoot2 = (*this)[i];
      this->TheWeylGroup->SimpleReflectionRoot(j, tempRoot, false, false);
      int x = this->GetIndex(tempRoot);
      if (x == - 1) {
        global.fatal << "This is a programming error: something wrong has happened. "
        << "A weight that is supposed to "
        << " be in a certain Weyl group orbit isn't there. "
        << "There is an error in the code, crashing accordingly. " << global.fatal;
      }
      this->SimpleReflectionsActionList[i].AddOnTop(x);
      tempRoot2 -= tempRoot;
      if (tempRoot2.IsPositiveOrZero() && !tempRoot2.IsEqualToZero()) {
        this->BruhatOrder[i].AddOnTop(x);
        this->InverseBruhatOrder[x].AddOnTop(i);
      }
    }
  }
  this->ComputeDebugString();
}

int KLpolys::FindMinimalBruhatNonExplored(List<bool>& theExplored) {
  int lowestIndex = - 1;
  for (int i = 0; i < this->size; i ++) {
    if (!theExplored[i]) {
      if (lowestIndex == - 1) {
        lowestIndex = i;
      } else {
        if (this->IndexGreaterThanIndex(lowestIndex, i)) {
          lowestIndex = i;
        }
      }
    }
  }
  return lowestIndex;
}

int KLpolys::FindMaximalBruhatNonExplored(List<bool>& theExplored) {
  int highestIndex = - 1;
  for (int i = 0; i < this->size; i ++) {
    if (!theExplored[i]) {
      if (highestIndex == - 1) {
        highestIndex = i;
      } else {
        if (this->IndexGreaterThanIndex(i, highestIndex)) {
          highestIndex = i;
        }
      }
    }
  }
  return highestIndex;
}

void KLpolys::MergeBruhatLists(int fromList, int toList) {
  for (int i = 0; i < this->BruhatOrder[fromList].size; i ++) {
    bool found = false;
    for (int j = 0; j < this->BruhatOrder[toList].size; j ++) {
      if (this->BruhatOrder[toList][j] == this->BruhatOrder[fromList][i]) {
        found = true;
        break;
      }
    }
    if (!found) {
      this->BruhatOrder[toList].AddOnTop(this->BruhatOrder[fromList][i]);
    }
  }
}

int KLpolys::ChamberIndicatorToIndex(Vector<Rational>& ChamberIndicator) {
  int theDimension = this->TheWeylGroup->CartanSymmetric.NumRows;
  Vector<Rational> tempRoot;
  tempRoot.SetSize(theDimension);
  Vector<Rational> ChamberIndicatorPlusRho;
  ChamberIndicatorPlusRho = (ChamberIndicator);
  ChamberIndicatorPlusRho += this->TheWeylGroup->rho;
  for (int i = 0; i < this->size; i ++) {
    Rational tempRat1, tempRat2;
    bool tempBool1, tempBool2;
    bool haveSameSigns = true;
    for (int j = 0; j < this->TheWeylGroup->RootSystem.size; j ++) {
      this->TheWeylGroup->RootScalarCartanRoot(ChamberIndicatorPlusRho, this->TheWeylGroup->RootSystem[j], tempRat1);
      tempRoot = (*this)[i];
      tempRoot += (this->TheWeylGroup->rho);
      this->TheWeylGroup->RootScalarCartanRoot(tempRoot, this->TheWeylGroup->RootSystem[j], tempRat2);
      tempBool1 = tempRat1.IsPositive();
      tempBool2 = tempRat2.IsPositive();
      if (tempRat1.IsEqualToZero() || tempRat2.IsEqualToZero()) {
        global.fatal << "Coefficients are zero at a bad place. " << global.fatal;
      }
      if (tempBool1 != tempBool2) {
        haveSameSigns = false;
        break;
      }
    }
    if (haveSameSigns) {
      return i;
    }
  }
  return - 1;
}

void KLpolys::ComputeKLcoefficients() {
  MacroRegisterFunctionWithName("KLpolys::ComputeKLcoefficients");
  this->theKLcoeffs.SetSize(this->theKLPolys.size);
  for (int i = 0; i < this->theKLPolys.size; i ++) {
    this->theKLcoeffs[i].SetSize(this->theKLPolys[i].size);
    for (int j = 0; j < this->theKLcoeffs[i].size; j ++) {
      Polynomial<Rational>& currentPoly = this->theKLPolys[i][j];
      this->theKLcoeffs[i][j] = 0;
      if (this->IndexGEQIndex(j, i)) {
        for (int k = 0; k < currentPoly.size(); k ++) {
          this->theKLcoeffs[i][j] += currentPoly.coefficients[k];
        }
      }
    }
  }
}

void KLpolys::initFromWeyl(WeylGroupData* theWeylGroup) {
  this->TheWeylGroup = theWeylGroup;
  Vectors<Rational> tempRoots;
  this->TheWeylGroup->ComputeRho(true);
  tempRoots.AddOnTop(this->TheWeylGroup->rho);
  this->TheWeylGroup->GenerateOrbit(tempRoots, false, *this, false);
  this->initTheMults();
}

bool KLpolys::ComputeKLPolys(WeylGroupData* theWeylGroup) {
  MacroRegisterFunctionWithName("KLpolys::ComputeKLPolys");
  theWeylGroup->theGroup.ComputeAllElements(- 1);
  this->initFromWeyl(theWeylGroup);
  this->GeneratePartialBruhatOrder();
  FormatExpressions PolyFormatLocal;
  PolyFormatLocal.polyDefaultLetter = "q";
  this->ComputeRPolys();
  this->theKLPolys.SetSize(this->size);
  this->theKLcoeffs.SetSize(this->size);
  this->Explored.initializeFillInObject(this->size, false);
  for (int i = 0; i < this->theKLPolys.size; i ++) {
    this->theKLPolys[i].SetSize(this->size);
    this->theKLcoeffs[i].SetSize(this->size);
  }
  for (int i = 0; i < this->size; i ++) {
    this->Explored.initializeFillInObject(this->size, false);
    int highestNonExplored = this->FindMaximalBruhatNonExplored(this->Explored);
    while (highestNonExplored != - 1) {
      this->ComputeKLxy(highestNonExplored, i);
      this->Explored[highestNonExplored] = true;
      highestNonExplored = this->FindMaximalBruhatNonExplored(this->Explored);
    }
  }
  this->ComputeKLcoefficients();
  return true;
}

void KLpolys::ComputeRPolys() {
  MacroRegisterFunctionWithName("KLpolys::ComputeRPolys");
  int theDimension = this->TheWeylGroup->GetDim();
  this->theRPolys.SetSize(this->size);
  for (int i = 0; i < this->size; i ++) {
    this->Explored[i] = false;
    this->theRPolys[i].SetSize(this->size);
  }
  this->LowestNonExplored = this->FindMinimalBruhatNonExplored(this->Explored);
  List<bool> ExploredFromTop;
  ExploredFromTop.SetSize(this->size);
  while (this->LowestNonExplored != - 1) {
    for (int i = 0; i < this->size; i ++) {
      ExploredFromTop[i] = false;
    }
    int a = this->FindMaximalBruhatNonExplored(ExploredFromTop);
    while (a != - 1) {
      bool tempBool = false;
      for (int j = 0; j < theDimension; j ++) {
        if (this->ComputeRxy(a, this->LowestNonExplored, j)) {
          tempBool = true;
          break;
        }
      }
      if (!tempBool) {
        global.fatal << "This is a programming error: an algorithmic check failed while computing R-polynomials. " << global.fatal;
      }
      ExploredFromTop[a] = true;
      a = this->FindMaximalBruhatNonExplored(ExploredFromTop);
    }
    this->Explored[this->LowestNonExplored] = true;
    this->LowestNonExplored = this->FindMinimalBruhatNonExplored(this->Explored);
  }
  //this->ComputeDebugString();
}

bool KLpolys::IndexGEQIndex(int a, int b) {
  Vector<Rational> tempV;
  tempV = (*this)[a];
  tempV -= (*this)[b];
  return tempV.IsNegativeOrZero();
}

bool KLpolys::IndexGreaterThanIndex(int a, int b) {
  if (a == b) {
    return false;
  }
  return this->IndexGEQIndex(a, b);
}

int KLpolys::ComputeProductfromSimpleReflectionsActionList(int x, int y) {
  int start = y;
  const ElementWeylGroup& currentElement = this->TheWeylGroup->theGroup.theElements[x];
  for (int i = currentElement.generatorsLastAppliedFirst.size - 1; i >= 0; i --) {
    start = this->SimpleReflectionsActionList[start][
      this->TheWeylGroup->theGroup.theElements[x].generatorsLastAppliedFirst[i].index
    ];
  }
  return start;
}

void KLpolys::ComputeKLxy(int x, int y) {
  Polynomial<Rational> Accum, tempP1, tempP2;
  if (x == y) {
    this->theKLPolys[x][y].MakeOne();
    return;
  }
  if (!this->IndexGEQIndex(y, x)) {
    this->theKLPolys[x][y].MakeZero();
    return;
  }
  Accum.MakeZero();
  MonomialP tempM;
  for (int i = 0; i < this->size; i ++) {
    if (this->IndexGreaterThanIndex(i, x) && this->IndexGEQIndex(y, i)) {
      tempP1.MakeZero();
      for (int j = 0; j < this->theRPolys[x][i].size(); j ++) {
        tempM = this->theRPolys[x][i][j];
        tempM.Invert();
        tempP1.AddMonomial(tempM, this->theRPolys[x][i].coefficients[j]);
      }
      int tempI;
      if ((
          this->TheWeylGroup->theGroup.theElements[x].generatorsLastAppliedFirst.size +
          this->TheWeylGroup->theGroup.theElements[i].generatorsLastAppliedFirst.size
        ) % 2 == 0
      ) {
        tempI = 1;
      } else {
        tempI = - 1;
      }
      Rational powerQ = - this->TheWeylGroup->theGroup.theElements[x].generatorsLastAppliedFirst.size +
      2 * this->TheWeylGroup->theGroup.theElements[i].generatorsLastAppliedFirst.size -
      this->TheWeylGroup->theGroup.theElements[y].generatorsLastAppliedFirst.size;
      powerQ /= 2;
      tempP2.MakeMonomiaL(0, powerQ, tempI, 1);
      tempP1 *= tempP2;
      tempP1 *= this->theKLPolys[i][y];
      if (!this->Explored[i]) {
        global.fatal << "This is a programming error: an internal check during the "
        << "Kazhdan-Lusztig polynomial computation fails. More precisely, while computing "
        << "KL poly of indices " << x << ", " << y
        << " I am using KL poly with indices " << i << ", " << y << " which hasn't been computed yet. "
        << global.fatal;
      }
      Accum += tempP1;
    }
  }
  this->theKLPolys[x][y].MakeZero();
  Rational lengthDiff = this->TheWeylGroup->theGroup.theElements[y].generatorsLastAppliedFirst.size -
  this->TheWeylGroup->theGroup.theElements[x].generatorsLastAppliedFirst.size;
  lengthDiff /= 2;
  for (int i = 0; i < Accum.size(); i ++) {
    if (Accum[i].HasPositiveOrZeroExponents()) {
      tempM = Accum[i];
      tempM[0].Minus();
      tempM[0] += lengthDiff;
      this->theKLPolys[x][y].AddMonomial(tempM, Accum.coefficients[i]);
    }
  }
}

bool KLpolys::ComputeRxy(int x, int y, int SimpleReflectionIndex) {
  MacroRegisterFunctionWithName("KLpolys::ComputeRxy");
  if (x == y) {
    this->theRPolys[x][y].MakeOne();
    return true;
  }
  if (this->IndexGreaterThanIndex(x, y)) {
    this->theRPolys[x][y].MakeZero();
    return true;
  }
  int sx = this->SimpleReflectionsActionList[x][SimpleReflectionIndex];
  int sy = this->SimpleReflectionsActionList[y][SimpleReflectionIndex];
  bool boolX, boolY;
  boolX = this->IndexGreaterThanIndex(x, sx);
  boolY = this->IndexGreaterThanIndex(y, sy);
  if (boolX && boolY) {
    if (!this->Explored[sy]) {
      global.fatal << "This is a programming error: the computaion of R-polynomials "
      << "is attempting to use a non-computed R-polynomial. " << global.fatal;
    }
    this->theRPolys[x][y] = this->theRPolys[sx][sy];
    return true;
  }
  if (!boolX && boolY) {
    Polynomial<Rational> qMinus1;
    qMinus1.MakeMonomiaL(0, 1, 1, 1);
    this->theRPolys[x][y] = qMinus1;
    this->theRPolys[x][y] *= (this->theRPolys[sx][sy]);
    qMinus1 -= 1;
    qMinus1 *= this->theRPolys[sx][y];
    this->theRPolys[x][y] += qMinus1;
    return true;
  }
  return false;
}

std::string KLpolys::KLPolysToString(FormatExpressions* theFormat) {
  std::stringstream out;
  out << "<table border =\"1\">";
  out << "<tr><td>Weyl elt.</td>";
  for (int i = 0; i < this->TheWeylGroup->theGroup.theElements.size; i ++) {
    out << "<td>" << this->TheWeylGroup->theGroup.theElements[i].ToString() << "</td>";
  }
  out << "</tr>";
  for (int i = 0; i < this->theKLPolys.size; i ++) {
    if (this->theKLPolys[i].size > 0) {
      out << "<tr><td>" << this->TheWeylGroup->theGroup.theElements[i].ToString() << "</td>";
      for (int j = 0; j < this->theKLPolys[i].size; j ++) {
        out << "<td>" << this->theKLPolys[i][j].ToString(theFormat) << "</td>";
      }
      out << "</tr>";
    }
  }
  out << "</table>";
  return out.str();
}

std::string KLpolys::RPolysToString(FormatExpressions* theFormat) {
  std::stringstream out;
  out << "<table border =\"1\"><tr><td>Weyl elt.</td>";
  for (int i = 0; i < this->TheWeylGroup->theGroup.theElements.size; i ++) {
    out << "<td>" << this->TheWeylGroup->theGroup.theElements[i].ToString() << "</td>";
  }
  out << "</tr>";
  for (int i = 0; i < this->theRPolys.size; i ++) {
    out << "<tr><td>" << this->TheWeylGroup->theGroup.theElements[i].ToString() << "</td>";
    for (int j = 0; j < this->theRPolys[i].size; j ++) {
      out << "<td>" << this->theRPolys[i][j].ToString(theFormat) << "</td>\n";
    }
    out << "</tr>";
  }
  out << "</table>";
  return out.str();
}

LargeInteger PartFraction::EvaluateIntPolyAtOne(Polynomial<LargeInteger>& input) {
  LargeInteger result;
  result.MakeZero();
  for (int i = 0; i < input.size(); i ++) {
    result += input.coefficients[i];
  }
  return result;
}

void PartFraction::EvaluateIntPoly(const Polynomial<LargeInteger>& input, const Vector<Rational>& values, Rational& output) {
  output.MakeZero();
  Polynomial<Rational> tempInput;
  tempInput = input; //<-implicit type conversion here!
  output = tempInput.Evaluate(values);
}

void LaTeXProcedures::beginPSTricks(std::fstream& output) {
  output << "\\begin{pspicture}(8, 8)";
}

void LaTeXProcedures::endPSTricks(std::fstream& output) {
  output << "\\end{pspicture}\\medskip \\\\ \\noindent ";
}

void LaTeXProcedures::beginDocument(std::fstream& output) {
  output << "\\documentclass{article}\n \\usepackage{pstricks} \\begin{document}";
}

void LaTeXProcedures::endLatexDocument(std::fstream& output) {
  output << "\\end{document}";
}

void LaTeXProcedures::GetStringFromColorIndex(int ColorIndex, std::string &output, DrawingVariables& drawInput) {
  switch(ColorIndex) {
    case 0:
      output.assign("black");
      break;
    case 1:
      output.assign("blue");
      break;
    case 2:
      output.assign("purple");
      break;
    case 3:
      output.assign("green");
      break;
    case 4:
      output.assign("cyan");
      break;
    case 5:
      output.assign("red");
      break;
    case 6:
      output.assign("purple");
      break;
    case 7:
      output.assign("cyan");
      break;
    case 56540:
      output.assign("yellow");
      break;
    default:
      output.assign("black");
      break;
  }
  if (ColorIndex == drawInput.GetColorFromChamberIndex(1)) {
    output.assign("blue");
  }
  if (ColorIndex == drawInput.GetColorFromChamberIndex(2)) {
    output.assign("purple");
  }
  if (ColorIndex == drawInput.GetColorFromChamberIndex(3)) {
    output.assign("green");
  }
  if (ColorIndex == drawInput.GetColorFromChamberIndex(4)) {
    output.assign("cyan");
  }
  if (ColorIndex == drawInput.GetColorFromChamberIndex(5)) {
    output.assign("red");
  }
  if (ColorIndex == drawInput.GetColorFromChamberIndex(6)) {
    output.assign("purple");
  }
  if (ColorIndex == drawInput.GetColorFromChamberIndex(7)) {
    output.assign("cyan");
  }
}

void LaTeXProcedures::drawTextDirectly(double X1, double Y1, const std::string& theText, int ColorIndex, std::fstream& output) {
  (void) ColorIndex;//avoid unused parameter warning, portable.
  output.precision(4);
  X1 -= theText.length() * LaTeXProcedures::TextPrintCenteringAdjustmentX;
  Y1 += LaTeXProcedures::TextPrintCenteringAdjustmentY;
  X1 /= LaTeXProcedures::ScaleFactor;
  Y1 /= LaTeXProcedures::ScaleFactor;
  output << "\\put(" << X1 - LaTeXProcedures::FigureCenterCoordSystemX << ", "
  << LaTeXProcedures::FigureCenterCoordSystemY - Y1 << "){\\tiny{" << theText << "}}";
}

void LaTeXProcedures::drawline(
  double X1,
  double Y1,
  double X2,
  double Y2,
  uint32_t thePenStyle,
  int ColorIndex,
  std::fstream& output,
  DrawingVariables& drawInput
) {
  if (static_cast<int>(thePenStyle) == DrawingVariables::PenStyleInvisible) {
    return;
  }
  output.precision(4);
  X1 /= LaTeXProcedures::ScaleFactor;
  X2 /= LaTeXProcedures::ScaleFactor;
  Y1 /= LaTeXProcedures::ScaleFactor;
  Y2 /= LaTeXProcedures::ScaleFactor;
  std::string tempS;
  if (thePenStyle == static_cast<unsigned>(DrawingVariables::PenStyleDashed)) {
    tempS = "lightgray";
  } else {
    LaTeXProcedures::GetStringFromColorIndex(ColorIndex, tempS, drawInput);
  }
  output << "\\psline[linewidth = 0.3pt, linecolor =" << tempS << "]("
  << X1 - LaTeXProcedures::FigureCenterCoordSystemX << ", "
  << LaTeXProcedures::FigureCenterCoordSystemY - Y1 << ")" << "("
  << X2 - LaTeXProcedures::FigureCenterCoordSystemX << ", "
  << LaTeXProcedures::FigureCenterCoordSystemY - Y2 << ")\n";
}

void WeylGroupData::TransformToSimpleBasisGenerators(
  Vectors<Rational>& theGens, const HashedList<Vector<Rational> >& inputRootSystem
) {
  MacroRegisterFunctionWithName("WeylGroup::TransformToSimpleBasisGenerators");
  for (int i = 0; i < theGens.size; i ++) {
    if (!theGens[i].IsPositiveOrZero()) {
      theGens[i].Minus();
    }
  }
  bool reductionOccured = true;
  Vector<Rational> tempRoot;
  while (reductionOccured) {
    reductionOccured = false;
    for (int i = 0; i < theGens.size; i ++) {
      for (int j = i + 1; j < theGens.size; j ++) {
        tempRoot = theGens[i];
        tempRoot -= theGens[j];
        if (tempRoot.IsEqualToZero()) {
          theGens.RemoveIndexSwapWithLast(j);
          reductionOccured = true;
        }
        if (inputRootSystem.Contains(tempRoot)) {
          if (!tempRoot.IsPositiveOrZero()) {
            tempRoot.Minus();
            theGens[j] = tempRoot;
          } else {
            theGens[i] = tempRoot;
          }
          reductionOccured = true;
        }
      }
    }
  }
}

template <class coefficient>
void Vector<coefficient>::PerturbNoZeroScalarProductWithMe(const List<Vector<coefficient> >& inputVectors) {
  MacroRegisterFunctionWithName("Vector::PerturbNoZeroScalarProductWithMe");
  coefficient theScalarProdInverted;
  for (int i = 0; i < inputVectors.size; i ++) {
    if (this->ScalarEuclidean(inputVectors[i]) == 0) {
      coefficient theScale = 1;
      for (int j = 0; j < i; j ++) {
        if (inputVectors[i].ScalarEuclidean(inputVectors[j]) != 0) {
          theScalarProdInverted = (this->ScalarEuclidean(inputVectors[j]) / inputVectors[i].ScalarEuclidean(inputVectors[j])) / 2;
          if (theScalarProdInverted < 0) {
            theScalarProdInverted *= - 1;
          }
          if (theScale == 0) {
            theScale = theScalarProdInverted;
          } else if (theScalarProdInverted != 0) {
            theScale = MathRoutines::Minimum(theScale, theScalarProdInverted);
          }
        }
      }
      *this += inputVectors[i] * theScale;
    }
  }
  for (int i = 0; i < inputVectors.size; i ++) {
    if (this->ScalarEuclidean(inputVectors[i]) == 0) {
      global.fatal << "This is a programming error: the vector produced by PerturbNoZeroScalarProductWithMe, namely, "
      << this->ToString() << " is orthogonal to input vector "
      << inputVectors[i].ToString() << ". The full list of vectors is "
      << inputVectors.ToString() << global.fatal;
    }
  }
}

void WeylGroupData::TransformToSimpleBasisGeneratorsArbitraryCoords(Vectors<Rational>& theGens, const HashedList<Vector<Rational> >& inputRootSystem) {
  if (theGens.size == 0) {
    return;
  }
  MacroRegisterFunctionWithName("WeylGroup::TransformToSimpleBasisGeneratorsArbitraryCoords");
  Vector<Rational> theH;
  theH.MakeZero(theGens[0].size);
  theH.PerturbNoZeroScalarProductWithMe(inputRootSystem);
  for (int i = 0; i < theGens.size; i ++) {
    if (theGens[i].ScalarEuclidean(theH) < 0) {
      theGens[i].Minus();
    }
  }
  bool reductionOccured = true;
  Vector<Rational> tempRoot;
  while (reductionOccured) {
    reductionOccured = false;
    for (int i = 0; i < theGens.size; i ++) {
      for (int j = i + 1; j < theGens.size; j ++) {
        tempRoot = theGens[i];
        tempRoot -= theGens[j];
        if (tempRoot.IsEqualToZero()) {
          theGens.RemoveIndexSwapWithLast(j);
          reductionOccured = true;
        }
        if (inputRootSystem.Contains(tempRoot)) {
          if (tempRoot.ScalarEuclidean(theH) < 0) {
            tempRoot.Minus();
            theGens[j] = tempRoot;
          } else {
            theGens[i] = tempRoot;
          }
          reductionOccured = true;
        }
      }
    }
  }
}

void WeylGroupData::TransformToSimpleBasisGeneratorsWRTh(Vectors<Rational>& theGens, const Vector<Rational>& theH) {
  for (int i = 0; i < theGens.size; i ++) {
    if (!this->IsPositiveOrPerpWRTh(theGens[i], theH)) {
      theGens[i].Minus();
    }
  }
  bool reductionOccured = true;
  Vector<Rational> tempRoot;
  while (reductionOccured) {
    reductionOccured = false;
    for (int i = 0; i < theGens.size; i ++) {
      for (int j = i + 1; j < theGens.size; j ++) {
        tempRoot = theGens[i];
        tempRoot -= theGens[j];
        if (tempRoot.IsEqualToZero()) {
          theGens.RemoveIndexSwapWithLast(j);
          reductionOccured = true;
        }
        if (this->RootSystem.GetIndex(tempRoot) != - 1) {
          if (!this->IsPositiveOrPerpWRTh(tempRoot, theH)) {
            tempRoot.Minus();
            theGens[j] = tempRoot;
          } else {
            theGens[i] = tempRoot;
          }
          reductionOccured = true;
        }
      }
    }
  }
}

void WeylGroupData::ComputeExtremeRootInTheSameKMod(
  const Vectors<Rational>& inputSimpleBasisK, const Vector<Rational>& inputRoot, Vector<Rational>& output, bool lookingForHighest
) {
  MacroRegisterFunctionWithName("WeylGroup::ComputeExtremeRootInTheSameKMod");
  output = inputRoot;
  Vector<Rational> tempRoot;
  bool FoundHigher = true;
  do {
    FoundHigher = false;
    for (int i = 0; i < inputSimpleBasisK.size; i ++) {
      tempRoot = output;
      if (lookingForHighest) {
        tempRoot += inputSimpleBasisK[i];
      } else {
        tempRoot -= inputSimpleBasisK[i];
      }
      if (this->RootSystem.GetIndex(tempRoot) != - 1) {
        output = tempRoot;
        FoundHigher = true;
      }
      if (tempRoot.IsEqualToZero()) {
        output *= - 1;
        FoundHigher = true;
      }
    }
  } while (FoundHigher);
}

template<class coefficient>
bool Vectors<coefficient>::PerturbVectorToRegular(Vector<coefficient>& output) {
  Vector<coefficient> normal;
  bool result = false;
  while (!this->IsRegular(output, normal, output.size)) {
    result = true;
    normal /= 10;
    output += normal;
  }
  if (result) {
    output.ScaleByPositiveRationalToIntegralMinHeight();
  }
  return result;
}

//returning false means that the lattice given as rougher is not actually rougher than the current lattice
//or that there are too many representatives
bool Lattice::GetAllRepresentatives(const Lattice& rougherLattice, Vectors<Rational>& output) const {
  output.size = 0;
  if (this->basis.NumRows != rougherLattice.basis.NumRows) {
    return false;
  }
  List<int> thePeriods;
  Vectors<Rational> thePeriodVectors;
  thePeriods.SetSize(this->basis.NumRows);
  thePeriodVectors.SetSize(this->basis.NumRows);
  Vector<Rational> tempRoot, tempRoot2;
  int col = 0;
  int theDim = this->GetDim();
  Rational currentPeriod;
  LargeInteger currentPeriodInt;
  for (int i = 0; i < this->basis.NumRows; i ++) {
    while (this->basisRationalForm.elements[i][col].IsEqualToZero()) {
      col ++;
    }
    currentPeriod = rougherLattice.basisRationalForm.elements[i][col] / this->basisRationalForm.elements[i][col];
    currentPeriodInt = currentPeriod.GetNumerator();
    if (currentPeriodInt.value.theDigits.size > 1) {
      return false;
    } else {
      thePeriods[i] = currentPeriodInt.value.theDigits[0];
    }
    this->basisRationalForm.GetVectorFromRow(i, thePeriodVectors[i]);
    rougherLattice.basisRationalForm.GetVectorFromRow(i, tempRoot2);
    tempRoot = thePeriodVectors[i];
    tempRoot *= thePeriods[i];
  }
  for (int i = 0; i < thePeriods.size; i ++) {
    thePeriods[i] --;
  }
  SelectionWithDifferentMaxMultiplicities theCoeffSelection;
  theCoeffSelection.initFromInts(thePeriods);
  int NumCycles = theCoeffSelection.TotalNumSubsetsMustBeSmalInt();
  output.SetSize(NumCycles);
  for (int i = 0; i < NumCycles; i ++, theCoeffSelection.IncrementReturnFalseIfPastLast()) {
    output[i].MakeZero(theDim);
    for (int j = 0; j < theCoeffSelection.Multiplicities.size; j ++) {
      output[i] += thePeriodVectors[j] * theCoeffSelection.Multiplicities[j];
    }
  }
  return true;
}

bool Lattice::GetClosestPointInDirectionOfTheNormalToAffineWallMovingIntegralStepsInDirection(
  Vector<Rational>& startingPoint,
  Vector<Rational>& theAffineHyperplane,
  Vector<Rational>& theDirection,
  Vector<Rational>& outputPoint
) {
  Vector<Rational> theNormal = theAffineHyperplane;
  theNormal.SetSize(theAffineHyperplane.size - 1);
  Rational theShift = - (*theAffineHyperplane.LastObject());
  if (theNormal.ScalarEuclidean(startingPoint) == theShift) {
    outputPoint = startingPoint;
    return true;
  }
  if (theDirection.ScalarEuclidean(theNormal).IsEqualToZero()) {
    return false;
  }
  Rational theMovement = (theShift - startingPoint.ScalarEuclidean(theNormal)) / theDirection.ScalarEuclidean(theNormal);
  global.Comments << "<br>the movement: " << theMovement.ToString() << ", (" << theShift.ToString()
  << " - " << startingPoint.ScalarEuclidean(theNormal).ToString() << ")/ "
  << theDirection.ScalarEuclidean(theNormal).ToString() << ", ";
  if (!theMovement.IsInteger()) {
    global.Comments << "the movement is not integral; ";
    theMovement.AssignFloor();
    if (theDirection.ScalarEuclidean(theNormal).IsPositive()) {
      theMovement += 1;
    }
  }
  global.Comments << "the normal: " << theNormal.ToString() << ", the direction: "
  << theDirection.ToString() << ", the shift: " << theShift.ToString()
  << ", the movement: " << theMovement.ToString() << ", startingPoint: " << startingPoint.ToString();
  outputPoint = startingPoint;
  outputPoint += theDirection * theMovement;
  global.Comments << ", finalPoint: " << outputPoint.ToString();
  return true;
}

bool Lattice::GetAllRepresentativesProjectingDownTo(
  const Lattice& rougherLattice, Vectors<Rational>& startingShifts, Vectors<Rational>& output
) const {
  Vectors<Rational> tempRepresentatives;
  if (!this->GetAllRepresentatives(rougherLattice, tempRepresentatives)) {
    return false;
  }
  output.SetExpectedSize(startingShifts.size * tempRepresentatives.size);
  output.size = 0;
  for (int i = 0; i < startingShifts.size; i ++) {
    for (int j = 0; j < tempRepresentatives.size; j ++) {
      output.AddOnTop(startingShifts[i] + tempRepresentatives[j]);
    }
  }
  return true;
}

void QuasiPolynomial::MakeRougherLattice(const Lattice& latticeToRoughenBy) {
  if (this->AmbientLatticeReduced == latticeToRoughenBy) {
    return;
  }
  Lattice OldLattice;
  OldLattice = this->AmbientLatticeReduced;
  this->AmbientLatticeReduced.IntersectWith(latticeToRoughenBy);
  Vectors<Rational> representativesQuotientLattice;
  OldLattice.GetAllRepresentatives(this->AmbientLatticeReduced, representativesQuotientLattice);
  Vectors<Rational> OldLatticeShifts = this->LatticeShifts;
  List<Polynomial<Rational> > oldValues;
  oldValues = this->valueOnEachLatticeShift;
  this->LatticeShifts.SetSize(OldLatticeShifts.size * representativesQuotientLattice.size);
  this->valueOnEachLatticeShift.SetSize(this->LatticeShifts.size);
  for (int i = 0; i < OldLatticeShifts.size; i ++) {
    for (int j = 0; j < representativesQuotientLattice.size; j ++) {
      this->LatticeShifts[i * representativesQuotientLattice.size + j] = OldLatticeShifts[i] + representativesQuotientLattice[j];
      this->valueOnEachLatticeShift[i * representativesQuotientLattice.size + j] = oldValues[i];
    }
  }
}

void Lattice::GetDualLattice(Lattice& output) const {
  if (this->GetRank() != this->GetDim()) {
    return;
  }
  Matrix<Rational> tempMat;
  tempMat = this->basisRationalForm;
  tempMat.Invert();
  tempMat.Transpose();
  tempMat.GetMatrixIntWithDen(output.basis, output.Den);
  output.Reduce();
}

bool Lattice::FindOnePreimageInLatticeOf(
  const Matrix<Rational>& theLinearMap, const Vectors<Rational>& input, Vectors<Rational>& output
) {
  Vectors<Rational> thisBasis;
  thisBasis.AssignMatrixRows(this->basisRationalForm);
  theLinearMap.ActOnVectorsColumn(thisBasis, Rational(0));
  bool result = input.GetIntegralCoordsInBasisIfTheyExist(thisBasis, output, 1, - 1, 0);
  Matrix<Rational> tempMat;
  tempMat = this->basisRationalForm;
  tempMat.Transpose();
  tempMat.ActOnVectorsColumn(output);
  return result;
}

void Lattice::IntersectWithPreimageOfLattice(const Matrix<Rational> & theLinearMap, const Lattice& other) {
  Vectors<Rational> startingBasis, imageStartingBasis, basisImageIntersection, ImageBasisInImageStartingBasisCoords;
  Vectors<Rational> resultNonKernelPart, result, tempRoots;
  startingBasis.AssignMatrixRows(this->basisRationalForm);
  theLinearMap.ActOnVectorsColumn(startingBasis, imageStartingBasis);
  Lattice ImageLattice;
  ImageLattice.MakeFromRoots(imageStartingBasis);
  ImageLattice.IntersectWith(other);
  basisImageIntersection.AssignMatrixRows(ImageLattice.basisRationalForm);
  Vectors<Rational> tempBasisImageIntersection, tempImageStartingBasis, tempImageBasisInImageStartingBasisCoords;
  basisImageIntersection = tempBasisImageIntersection;
  imageStartingBasis = tempImageStartingBasis;
  bool tempBool = tempBasisImageIntersection.GetIntegralCoordsInBasisIfTheyExist(
    tempImageStartingBasis, tempImageBasisInImageStartingBasisCoords, Rational(1), Rational(- 1), Rational(0)
  );
  ImageBasisInImageStartingBasisCoords =(tempImageBasisInImageStartingBasisCoords);
  if (!tempBool) {
    global.fatal << "Coordinates not integral when they should be. " << global.fatal;
  }
  resultNonKernelPart.SetSize(ImageBasisInImageStartingBasisCoords.size);
  for (int i = 0; i < resultNonKernelPart.size; i ++) {
    Vector<Rational>& currentRoot = resultNonKernelPart[i];
    currentRoot.MakeZero(this->GetDim());
    for (int j = 0; j < startingBasis.size; j ++) {
      currentRoot += startingBasis[j] * ImageBasisInImageStartingBasisCoords[i][j];
    }
  }
  Lattice KernelPart;
  KernelPart = *this;
  tempRoots.AssignMatrixRows(theLinearMap);
  KernelPart.IntersectWithLinearSubspaceGivenByNormals(tempRoots);
  result.AssignMatrixRows(KernelPart.basisRationalForm);
  result.AddListOnTop(resultNonKernelPart);
  this->MakeFromRoots(result);
}

void Lattice::IntersectWith(const Lattice& other) {
  Vectors<Rational> commonBasis, otherBasis, startBasis;
  startBasis.AssignMatrixRows(this->basisRationalForm);
  otherBasis.AssignMatrixRows(other.basisRationalForm);
  startBasis.IntersectTwoLinSpaces(startBasis, otherBasis, commonBasis);
  Lattice thisLatticeIntersected, otherLatticeIntersected;
  thisLatticeIntersected = *this;
  otherLatticeIntersected = other;
  thisLatticeIntersected.IntersectWithLinearSubspaceSpannedBy(commonBasis);
  otherLatticeIntersected.IntersectWithLinearSubspaceSpannedBy(commonBasis);
  Vectors<Rational> thisCommonBasis, otherCommonBasis, thisCommonCoords, otherCommonCoords;
  thisCommonBasis.AssignMatrixRows(thisLatticeIntersected.basisRationalForm);
  otherCommonBasis.AssignMatrixRows(otherLatticeIntersected.basisRationalForm);
  thisCommonBasis.GetCoordsInBasis(commonBasis, thisCommonCoords);
  otherCommonBasis.GetCoordsInBasis(commonBasis, otherCommonCoords);
  Lattice thisCommonCoordsLattice, otherCommonCoordsLattice;
  thisCommonCoordsLattice.MakeFromRoots(thisCommonCoords);
  otherCommonCoordsLattice.MakeFromRoots(otherCommonCoords);
  thisCommonCoordsLattice.IntersectWithBothOfMaxRank(otherCommonCoordsLattice);
  Vectors<Rational> resultBasis;
  resultBasis.SetSize(thisCommonCoordsLattice.basisRationalForm.NumRows);
  for (int i = 0; i < resultBasis.size; i ++) {
    Vector<Rational>& currentRoot = resultBasis[i];
    currentRoot.MakeZero(this->GetDim());
    for (int j = 0; j < thisCommonCoordsLattice.basisRationalForm.NumCols; j ++) {
      currentRoot += commonBasis[j] * thisCommonCoordsLattice.basisRationalForm.elements[i][j];
    }
  }
  this->MakeFromRoots(resultBasis);
}

void Lattice::IntersectWithBothOfMaxRank(const Lattice& other) {
  Lattice dualLatticeThis, dualLatticeOther;
  if (this->basis.NumRows != this->GetDim() || this->GetDim() != other.GetDim() || other.basis.NumRows != this->GetDim()) {
    global.fatal << "Bad dimensions. " << global.fatal;
  }
  this->GetDualLattice(dualLatticeThis);
  other.GetDualLattice(dualLatticeOther);
  dualLatticeThis.RefineByOtherLattice(dualLatticeOther);
  dualLatticeThis.GetDualLattice(*this);
}

void QuasiPolynomial::operator+=(const QuasiPolynomial& other) {
  this->MakeRougherLattice(other.AmbientLatticeReduced);
  QuasiPolynomial tempQP = other;
  tempQP.MakeRougherLattice(this->AmbientLatticeReduced);
  for (int i = 0; i < tempQP.LatticeShifts.size; i ++) {
    this->AddLatticeShift(tempQP.valueOnEachLatticeShift[i], tempQP.LatticeShifts[i]);
  }
}

std::string QuasiPolynomial::ToString(bool useHtml, bool useLatex, FormatExpressions* thePolyFormat) {
  std::stringstream out;
  if (this->LatticeShifts.size == 0) {
    return "0";
  }
  if (useLatex && !useHtml) {
    out << "\\begin{tabular}{c}";
  }
  if (useLatex && useHtml) {
    out << "\\begin{array}{rcl}&&";
  }
  for (int i = 0; i < this->LatticeShifts.size; i ++) {
    if (useLatex) {
      if (!useHtml) {
        out << "$\\begin{array}{rcl}&&";
      } else {
        out << "<span class =\"math\">";
      }
    }
    out << this->valueOnEachLatticeShift[i].ToString(thePolyFormat);
    if (useLatex) {
      if (!useHtml) {
        out << "\\end{array}$";
      } else {
        out << "</span>";
      }
    }
    if (!useLatex) {
      out << " over ";
    } else {
      if (useHtml) {
        out << " \\mathrm{~over~}";
      } else {
        out << " over ";
      }
    }
    if (!this->LatticeShifts[i].IsEqualToZero()) {
      out << this->LatticeShifts[i].ToString() << " + ";
    }
    if (useLatex) {
      if (!useHtml) {
        out << "$\\Lambda$, \\\\\\hline\n ";
      } else {
        out << " \\Lambda";
      }
    } else {
      out << "L ";
    }
    if (this->LatticeShifts.size > 1) {
      if (useHtml & ! useLatex) {
        out << "<br>";
      }
      if (useLatex && !useHtml) {
        out << "\\\\";
      }
      if (useLatex && useHtml) {
        out << "\\\\&&";
      }
      out << "\n\n";
    }
  }
  if (!this->AmbientLatticeReduced.basisRationalForm.IsIdMatrix()) {
    if (!useLatex) {
      out << ", where L=< ";
    }
    if (useLatex && useHtml) {
      out << ", \\mathrm{~where~} \\Lambda =<";
    }
    if (useLatex && !useHtml) {
      out << " where $\\Lambda =\\left\\langle\\begin{array}{c}";
    }
    Vectors<Rational> tempRoots;
    tempRoots.AssignMatrixRows(this->AmbientLatticeReduced.basisRationalForm);
    for (int i = 0; i < tempRoots.size; i ++) {
      out << tempRoots[i].ToString();
      if (i != tempRoots.size - 1) {
        out << ", ";
        if (useLatex) {
          out << "\\\\";
        }
      }
    }
    if (!useLatex) {
      out << " >";
    }
    if (useLatex && useHtml) {
      out << " >";
    }
    if (useLatex && !useHtml) {
      out << "\\end{array}\\right\\rangle$";
    }
  } else {
    if (useLatex) {
      if (!useHtml) {
        out << ", where $\\Lambda =\\mathbb{Z}^{" << this->GetNumVars() << "}$";
      } else {
        out << ", \\mathrm{~where~} \\Lambda =\\mathbb{Z}^{" << this->GetNumVars() << "}";
      }
    } else {
      out << "Z^" <<  this->GetNumVars();
    }
  }
  if (useLatex && !useHtml) {
    out << "\\end{tabular}";
  }
  if (useLatex && useHtml) {
    out << "\\end{array}";
  }
  return out.str();
}

void QuasiPolynomial::MakeFromPolyShiftAndLattice(
  const Polynomial<Rational>& inputPoly, const MonomialP& theShift, const Lattice& theLattice
) {
  this->AmbientLatticeReduced = theLattice;
  this->LatticeShifts.SetSize(1);
  Vector<Rational>& firstShift = this->LatticeShifts[0];
  firstShift.SetSize(theLattice.GetDim());
  for (int i = 0; i < theLattice.GetDim(); i ++) {
    firstShift[i] = theShift(i);
  }
  this->AmbientLatticeReduced.ReduceVector(this->LatticeShifts[0]);
  this->valueOnEachLatticeShift.SetSize(1);
  this->valueOnEachLatticeShift[0] = inputPoly;
}

bool Lattice::ReduceVector(Vector<Rational>& theVector) const {
  Vector<Rational> output;
  if (theVector.size != this->GetDim()) {
    global.fatal << "Vector dimension not as expected. " << global.fatal;
  }
  Vectors<Rational> basisRoots;
  basisRoots.AssignMatrixRows(this->basisRationalForm);
  if (!theVector.GetCoordsInBasiS(basisRoots, output)) {
    global.fatal << "Failed to get coordinates in basis. " << global.fatal;
    return false;
  }
  for (int i = 0; i < output.size; i ++) {
    output[i].AssignFracValue();
  }
  theVector.MakeZero(theVector.size);
  for (int i = 0; i < basisRoots.size; i ++) {
    theVector += basisRoots[i] * output[i];
  }
  return true;
}

void Lattice::MakeZn(int theDim) {
  this->basisRationalForm.MakeIdMatrix(theDim);
  this->basis.MakeIdMatrix(theDim, LargeInteger(1), LargeInteger(0));
  this->Den.MakeOne();
}

void QuasiPolynomial::MakeZeroOverLattice(Lattice& theLattice) {
  this->AmbientLatticeReduced = theLattice;
  this->LatticeShifts.size = 0;
  this->valueOnEachLatticeShift.size = 0;
}

void QuasiPolynomial::MakeZeroLatTiceZn(int theDim) {
  if (theDim <= 0) {
    global.fatal << "Negative dimension not allowed. " << global.fatal;
  }
  this->AmbientLatticeReduced.MakeZn(theDim);
  this->LatticeShifts.size = 0;
  this->valueOnEachLatticeShift.size = 0;
}

void PartFraction::GetRootsFromDenominator(PartFractions& owner, Vectors<Rational>& output) const {
  output.SetSize(this->IndicesNonZeroMults.size);
  for (int i = 0; i < this->IndicesNonZeroMults.size; i ++) {
    output[i] = owner.startingVectors[this->IndicesNonZeroMults[i]];
    oneFracWithMultiplicitiesAndElongations& current = this->TheObjects[this->IndicesNonZeroMults[i]];
    if (current.Elongations.size != 1) {
      global.fatal << "Elongations expected to have a single element. " << global.fatal;
    }
    output[i] *= current.Elongations[0];
  }
}

void PartFraction::ComputePolyCorrespondingToOneMonomial(
  QuasiPolynomial& outputQP, const MonomialP& theMon, Vectors<Rational>& normals, Lattice& theLattice
) const {
  Polynomial<Rational> tempP, outputPolyPart;
  outputPolyPart.MakeOne(theMon.GetMinNumVars());
  for (int i = 0; i < theMon.GetMinNumVars(); i ++) {
    this->MakePolynomialFromOneNormal(normals[i], theMon, this->TheObjects[this->IndicesNonZeroMults[i]].Multiplicities[0], tempP);
    outputPolyPart *= tempP;
  }
  outputQP.MakeFromPolyShiftAndLattice(outputPolyPart, theMon, theLattice);
}


void PartFraction::GetVectorPartitionFunction(
  PartFractions& owner, Polynomial<LargeInteger>& theCoeff, QuasiPolynomial& output
) const {
  QuasiPolynomial shiftedPoly;
  Vectors<Rational> theNormals, theLatticeGenerators;
  if (PartFraction::MakingConsistencyCheck) {
    PartFraction::CheckSum.MakeZero();
  }
  this->GetRootsFromDenominator(owner, theLatticeGenerators);
  Lattice theLattice;
  theLattice.MakeFromRoots(theLatticeGenerators);
  Matrix<Rational>  theNormalsMatForm;
  theNormalsMatForm.AssignVectorsToRows(theLatticeGenerators);
  theNormalsMatForm.Invert();
  theNormals.AssignMatrixColumns(theNormalsMatForm);
  output.MakeZeroLatTiceZn(owner.AmbientDimension);
  for (int i = 0; i < theCoeff.size(); i ++) {
    this->ComputePolyCorrespondingToOneMonomial(shiftedPoly, theCoeff[i], theNormals, theLattice);
    shiftedPoly *= theCoeff.coefficients[i];
    output += shiftedPoly;
  }
}

bool PartFractions::GetVectorPartitionFunction(QuasiPolynomial& output, Vector<Rational>& newIndicator) {
  ProgressReport theReport;
  if (this->AssureIndicatorRegularity(newIndicator)) {
    theReport.Report("Indicator modified to regular");
  }
  this->ResetRelevanceIsComputed();
  if (!this->CheckForMinimalityDecompositionWithRespectToRoot(&newIndicator)) {
    return false;
  }
  this->NumProcessedForVPFfractions = 0;
  if (PartFraction::MakingConsistencyCheck) {
    PartFractions::CheckSum.MakeZero();
  }
  output.MakeZeroLatTiceZn(this->AmbientDimension);
  ///////////////////////////////////////////////
  //this->flagAnErrorHasOccurredTimeToPanic = true;
  //PartFraction::flagAnErrorHasOccurredTimeToPanic = true;
  //this->ComputeDebugString();
  ///////////////////////////////////////////////
  QuasiPolynomial tempQP;
  for (int i = 0; i < this->size(); i ++) {
    if ((*this)[i].rootIsInFractionCone(*this, &newIndicator)) {
      const PartFraction& currentPF = (*this)[i];
      currentPF.GetVectorPartitionFunction(*this, this->coefficients[i], tempQP);
      output += tempQP;
      this->MakeProgressVPFcomputation();
    }
  }
  return true;
}

std::string PartFractions::DoTheFullComputationReturnLatexFileString(
  Vectors<Rational>& toBePartitioned, FormatExpressions& theFormat, std::string* outputHtml
) {
  std::string whatWentWrong;
  global.fatal << "Not implemented yet. " << global.fatal;
//  this->theChambersOld.theDirections = toBePartitioned;
  this->AmbientDimension = toBePartitioned.GetDim();
//  this->theChambersOld.AmbientDimension = toBePartitioned.GetDim();
//  this->theChambersOld.thePauseController.InitComputation();
  //this->theChambers.ReadFromDefaultFile();
  std::stringstream out;
  std::stringstream outHtml;
  global.fatal << global.fatal ;
//  this->theChambersOld.SliceTheEuclideanSpace(false);
//  this->theChambersOld.QuickSortAscending();
//  this->theChambersOld.LabelChamberIndicesProperly();
//  this->theChambers.AssignCombinatorialChamberComplex(this->theChambersOld);
//  this->theChambersOld.drawOutput(global.theDrawingVariables, tempRoot, 0);
//  this->theChambersOld.thePauseController.ExitComputation();
  DrawingVariables theDVs;
  this->theChambers.DrawMeProjective(nullptr, true, theDVs, theFormat);
  outHtml << theDVs.GetHtmlDiv(this->AmbientDimension);
  Vector<Rational> tempRoot;
  tempRoot.MakeZero(this->AmbientDimension);
  global.fatal << "not implemented yet" << global.fatal;
//  this->initFromRoots(theChambersOld.theDirections);
  out << "\\documentclass{article}\\usepackage{amsmath, amsfonts, amssymb} \n\\begin{document}\n";
  out << "The vector partition funciton is the number of ways you can represent a vector $(x_1,\\dots, x_n)$ as a non-negative integral linear combination of "
  << " a given set of vectors.  You requested the vector partition function with respect to the set of vectors: ";
  global.fatal << global.fatal;
//  out << this->theChambersOld.theDirections.ElementToStringGeneric();
  out << "\n\n The corresponding generating function is: " << this->ToString(theFormat) << "= (after splitting acording to algorithm)";
  this->split(nullptr);
  out << this->ToString(theFormat);
  global.fatal << global.fatal;
//  out << "Therefore the vector partition function is given by " << this->theChambersOld.GetNumNonZeroPointers()
//        << " quasipolynomials depending on which set of linear inequalities is satisfied (each such set we call ``Chamber'').";
//  outHtml << "There are " << this->theChambersOld.size << " systems of linear inequalities "
//  << " such that on each such system of inequalities the vector partition function is quasi-polynomial. "
//  << " A full list of the systems of inequalities (\"chambers\") and the corresponding quasi-polynomials follows.<hr> ";
  QuasiPolynomial tempQP;
  std::string tempS;
  Vector<Rational> tempIndicator;
  global.fatal << global.fatal;
  /*for (int i = 0; i < this->theChambersOld.size; i ++)
    if (this->theChambersOld[i] != 0) {
      Cone& currentChamber = this->theChambers[i];
      tempIndicator = currentChamber.GetInternalPoint();
      this->GetVectorPartitionFunction(tempQP, tempIndicator);
      out << "\n\n" << currentChamber.ToString(true, false, true, false, theFormat);
      out << "\n\nQuasipolynomial: " << tempQP.ToString(false, true, theFormat);
      outHtml << "<hr>Chamber: " << currentChamber.ToString(false, true, true, false, theFormat);
      bool useHtml = false;
      if (tempQP.valueOnEachLatticeShift.size >0)
        if (tempQP.valueOnEachLatticeShift[0].size<30)
          useHtml = true;
      outHtml << "<br>Quasi-polynomial: " <<
      HtmlRoutines::GetHtmlMathDivFromLatexFormulA(tempQP.ToString(useHtml, true, theFormat));
    }
    */
  out << "\\end{document}";
  if (outputHtml != nullptr) {
    *outputHtml = outHtml.str();
  }
  return out.str();
}

void QuasiPolynomial::AddLatticeShift(
  const Polynomial<Rational>& input, const Vector<Rational>& inputShift
) {
  Vector<Rational> theShift = inputShift;
  this->AmbientLatticeReduced.ReduceVector(theShift);
  int index = this->LatticeShifts.GetIndex(theShift);
  if (index == - 1) {
    index = this->LatticeShifts.size;
    this->LatticeShifts.AddOnTop(theShift);
    this->valueOnEachLatticeShift.ExpandOnTop(1);
    this->valueOnEachLatticeShift.LastObject()->MakeZero();
  }
  this->valueOnEachLatticeShift[index] += input;
  if (this->valueOnEachLatticeShift[index].IsEqualToZero()) {
    this->LatticeShifts.RemoveIndexSwapWithLast(index);
    this->valueOnEachLatticeShift.RemoveIndexSwapWithLast(index);
  }
}

void QuasiPolynomial::Substitution(
  const Matrix<Rational>& mapFromNewSpaceToOldSpace, const Lattice& ambientLatticeNewSpace, QuasiPolynomial& output
) {
  MacroRegisterFunctionWithName("QuasiPolynomial::Substitution");
  //Format of the substitution.
  //If we want to carry out a substitution in P(y_1, ..., y_m),
  // of the form
  //y_1=a_11 x_1+...+a_1nx_n
  //...
  //y_m=a_m1 x_1+...+a_mnx_n
  // then the mapFromNewSpaceToOldSpace shouldbe the matrix
  //a_11 ... a_1n
  //...
  //a_m1 ... a_mn
  if (
    this == &output || mapFromNewSpaceToOldSpace.NumRows != this->GetNumVars() ||
    ambientLatticeNewSpace.GetDim() != mapFromNewSpaceToOldSpace.NumCols
  ) {
    global.fatal << "Bad lattice dimensions. " << global.fatal;
  }
  output.AmbientLatticeReduced = ambientLatticeNewSpace;
  output.AmbientLatticeReduced.IntersectWithPreimageOfLattice(mapFromNewSpaceToOldSpace, this->AmbientLatticeReduced);
  Vectors<Rational> allRepresentatives, imagesAllRepresentatives;
  bool tempBool = ambientLatticeNewSpace.GetAllRepresentatives(output.AmbientLatticeReduced, allRepresentatives);
  if (!tempBool) {
    global.fatal << "Failed to get all representatives. " << global.fatal;
  }
  mapFromNewSpaceToOldSpace.ActOnVectorsColumn(allRepresentatives, imagesAllRepresentatives);
  PolynomialSubstitution<Rational> theSub;
  theSub.SetSize(this->GetNumVars());
  Vector<Rational> tempRoot;
  for (int i = 0; i < theSub.size; i ++) {
    Polynomial<Rational>& currentPoly = theSub[i];
    mapFromNewSpaceToOldSpace.GetVectorFromRow(i, tempRoot);
    currentPoly.MakeLinPolyFromRootNoConstantTerm(tempRoot);
  }
  Polynomial<Rational> tempP;
  for (int i = 0; i < this->valueOnEachLatticeShift.size; i ++) {
    tempP = this->valueOnEachLatticeShift[i];
    bool tempB = tempP.Substitution(theSub);
    if (!tempB) {
      global.fatal << "This is a programming error: substitution "
      << theSub.ToString() << " into polynomial " << tempP.ToString()
      << " failed but the current function does not handle this properly. " << global.fatal;
    }
    for (int j = 0; j < allRepresentatives.size; j ++) {
      if (imagesAllRepresentatives[j] == this->LatticeShifts[i]) {
        output.AddLatticeShift(tempP, allRepresentatives[j]);
      }
    }
  }
}

void QuasiPolynomial::Substitution(
  const Vector<Rational>& inputTranslationSubtractedFromArgument, QuasiPolynomial& output
) {
  MacroRegisterFunctionWithName("QuasiPolynomial::Substitution");
  //format of the translation. If the starting quasipolynomial was P(y_1, ..., y_n),
  //and the translation has coordinates (t_1, ..., t_n),
  //then the resulting quasipolynomial will be P(x_1-t_1, ..., x_n-t_n)
  PolynomialSubstitution<Rational> theSub;
  theSub.MakeIdSubstitution(this->GetNumVars(), Rational(1));
  for (int i = 0; i < theSub.size; i ++) {
    theSub[i].AddConstant(- inputTranslationSubtractedFromArgument[i]);
  }
  Polynomial<Rational> tempP;
  output.MakeZeroLatTiceZn(this->GetNumVars());
  output.AmbientLatticeReduced = this->AmbientLatticeReduced;
  for (int i = 0; i < this->valueOnEachLatticeShift.size; i ++) {
    tempP = this->valueOnEachLatticeShift[i];
    bool tempB = tempP.Substitution(theSub);
    if (!tempB) {
      global.fatal << "This is a programming error: substitution "
      << theSub.ToString() << " into polynomial " << tempP.ToString()
      << " failed but the current function does not handle this properly. " << global.fatal;
    }
    output.AddLatticeShift(tempP, this->LatticeShifts[i] + inputTranslationSubtractedFromArgument);
  }
}

void QuasiPolynomial::Substitution(
  const Matrix<Rational>& mapFromNewSpaceToOldSpace,
  const Vector<Rational>& inputTranslationSubtractedFromArgument,
  const Lattice& ambientLatticeNewSpace,
  QuasiPolynomial& output
) {
  QuasiPolynomial tempQP;
  this->Substitution(inputTranslationSubtractedFromArgument, tempQP);
  tempQP.Substitution(mapFromNewSpaceToOldSpace, ambientLatticeNewSpace, output);
}

bool QuasiPolynomial::SubstitutionLessVariables(const PolynomialSubstitution<Rational>& theSub, QuasiPolynomial& output) const {
  Matrix<Rational> theLatticeSub;
  if (!this->AmbientLatticeReduced.GetHomogeneousSubMatFromSubIgnoreConstantTerms(theSub, theLatticeSub)) {
    return false;
  }
  Matrix<Rational> theSubLatticeShift;
  output.AmbientLatticeReduced = this->AmbientLatticeReduced;
  if (!output.AmbientLatticeReduced.SubstitutionHomogeneous(theLatticeSub)) {
    return false;
  }
  theSubLatticeShift.init(theLatticeSub.NumRows, 1);
  for (int i = 0; i < theSubLatticeShift.NumRows; i ++) {
    theSub[i].GetConstantTerm(theSubLatticeShift.elements[i][0], Rational(0));
  }
  Matrix<Rational> theShiftImage, shiftMatForm;
  output.LatticeShifts.size = 0;
  output.valueOnEachLatticeShift.size = 0;
  output.valueOnEachLatticeShift.Reserve(this->LatticeShifts.size);
  output.LatticeShifts.Reserve(this->LatticeShifts.size);
  Vector<Rational> tempRoot;
  Polynomial<Rational> tempP;
  for (int i = 0; i < this->LatticeShifts.size; i ++) {
    shiftMatForm.AssignVectorColumn(this->LatticeShifts[i]);
    shiftMatForm -= theSubLatticeShift;
    if (theLatticeSub.Solve_Ax_Equals_b_ModifyInputReturnFirstSolutionIfExists(theLatticeSub, shiftMatForm, theShiftImage)) {
      tempRoot.AssignMatDetectRowOrColumn(theShiftImage);
      tempP = this->valueOnEachLatticeShift[i];
      bool tempB = tempP.Substitution(theSub);
      if (!tempB) {
        global.fatal << "This is a programming error: substitution "
        << theSub.ToString() << " into polynomial " << tempP.ToString()
        << " failed but the current function does not handle this properly. " << global.fatal;
      }
      output.AddLatticeShift(tempP, tempRoot);
    }
  }
  return true;
}

bool Lattice::SubstitutionHomogeneous(const PolynomialSubstitution<Rational>& theSub) {
  Matrix<Rational> matSub;
  if (!this->GetHomogeneousSubMatFromSubIgnoreConstantTerms(theSub, matSub)) {
    return false;
  }
  return this->SubstitutionHomogeneous(matSub);
}

bool Lattice::GetHomogeneousSubMatFromSubIgnoreConstantTerms(
  const PolynomialSubstitution<Rational>& theSub, Matrix<Rational>& output
) {
  if (theSub.size < 1) {
    return false;
  }
  int theTargetDim = 0;
  for (int i = 0; i < theSub.size; i ++) {
    theTargetDim = MathRoutines::Maximum(theTargetDim, theSub[i].GetMinNumVars());
  }
  output.init(theSub.size, theTargetDim);
  for (int i = 0; i < theSub.size; i ++) {
    Polynomial<Rational>& currentPoly = theSub[i];
    if (!currentPoly.IsLinear()) {
      return false;
    }
    for (int j = 0; j < theTargetDim; j ++) {
      currentPoly.GetCoeffInFrontOfLinearTermVariableIndex(j, output.elements[i][j]);
    }
  }
  return true;
}

void Lattice::IntersectWithLinearSubspaceGivenByNormal(const Vector<Rational>& theNormal) {
  Vectors<Rational> startingBasis, resultBasis;
  startingBasis.AssignMatrixRows(this->basisRationalForm);
  Vector<Rational> theScalarProducts;
  theScalarProducts.SetSize(startingBasis.size);
  for (int i = 0; i < this->basisRationalForm.NumRows; i ++) {
    theScalarProducts[i] = theNormal.ScalarEuclidean(startingBasis[i]);
  }
  if (theScalarProducts.IsEqualToZero()) {
    return;
  }
  int pivotColumnIndex = theScalarProducts.getIndexLastNonZeroCoordinate();
  Rational pivotCoeff = theScalarProducts[pivotColumnIndex];
  theScalarProducts /= - pivotCoeff;
  Vectors<Rational> eigenSpacePlusOrthogonalComponent;
  eigenSpacePlusOrthogonalComponent.SetSize(theScalarProducts.size);
  for (int i = 0; i < theScalarProducts.size; i ++) {
    if (i != pivotColumnIndex) {
      Vector<Rational>& currentRoot = eigenSpacePlusOrthogonalComponent[i];
      currentRoot.MakeZero(theScalarProducts.size);
      currentRoot[i] = 1;
      currentRoot[pivotColumnIndex] = theScalarProducts[i];
    }
  }
  theScalarProducts.ScaleByPositiveRationalToIntegralMinHeight();
  eigenSpacePlusOrthogonalComponent[pivotColumnIndex] = theScalarProducts;
  Lattice eigenLattice, theZnLattice;
  eigenLattice.MakeFromRoots(eigenSpacePlusOrthogonalComponent);
  theZnLattice.MakeZn(theScalarProducts.size);
  theZnLattice.IntersectWithBothOfMaxRank(eigenLattice);
  resultBasis.Reserve(theScalarProducts.size - 1);
  Vector<Rational> tempRoot, resultRoot; Rational orthogonalComponent;
  for (int i = 0; i < theZnLattice.basisRationalForm.NumRows; i ++) {
    theZnLattice.basisRationalForm.GetVectorFromRow(i, tempRoot);
    orthogonalComponent = tempRoot.ScalarEuclidean(theScalarProducts) / theScalarProducts.ScalarEuclidean(theScalarProducts);
    tempRoot -= theScalarProducts * orthogonalComponent;
    if (!orthogonalComponent.IsInteger()) {
      global.fatal << "Orthogonal component is supposed to be an integer. " << global.fatal;
    }
    if (!tempRoot.IsEqualToZero()) {
      resultRoot.MakeZero(this->GetDim());
      for (int j = 0; j < startingBasis.size; j ++) {
        resultRoot += startingBasis[j] * tempRoot[j];
      }
      resultBasis.AddOnTop(resultRoot);
    }
  }
  this->MakeFromRoots(resultBasis);
}

void Lattice::IntersectWithLinearSubspaceSpannedBy(const Vectors<Rational>& theSubspaceBasis) {
  Vectors<Rational> theNormals;
  Matrix<Rational> theMat;
  theSubspaceBasis.GetMatrixRootsToRows(theMat);
  theMat.GetZeroEigenSpace(theNormals);
  this->IntersectWithLinearSubspaceGivenByNormals(theNormals);
}

void Lattice::IntersectWithLinearSubspaceGivenByNormals(const Vectors<Rational>& theSubspaceNormals) {
  for (int i = 0; i < theSubspaceNormals.size; i ++) {
    this->IntersectWithLinearSubspaceGivenByNormal(theSubspaceNormals[i]);
  }
}

bool Lattice::SubstitutionHomogeneous(const Matrix<Rational> & theSub, Lattice& resultIsSubsetOf) {
  (void) theSub;
  (void) resultIsSubsetOf;
  global.fatal << "Not implemented yet. " << global.fatal;
 /*Vectors<Rational> preimageBasis;
  preimageBasis.AssignMatrixRows(this->basisRationalForm);
  Matrix<Rational>  theSubModifiable, currentBasisVector, oneSolution;
  for (int i = 0; i <preimageBasis.size; i ++) {
    theSubModifiable = theSub;
    currentBasisVector.AssignVectorColumn(preimageBasis[i]);
    if (theSubModifiable.Solve_Ax_Equals_b_ModifyInputReturnFirstSolutionIfExists(theSubModifiable, currentBasisVector, oneSolution)) {
      theSubModifiable = theSub;
      theSubModifiable.fin
    }
  }*/
return false;
}

bool Lattice::SubstitutionHomogeneous(const Matrix<Rational>& theSub) {
  int targetDim = theSub.NumCols;
  if (theSub.NumRows != this->GetDim()) {
    return false;
  }
  int startingDim = this->GetDim();
  Matrix<Rational> theMat, oldBasisTransformed, matRelationBetweenStartingVariables;
  theMat = theSub;
  oldBasisTransformed = this->basisRationalForm;
  oldBasisTransformed.Transpose();
  Selection nonPivotPoints;
  theMat.GaussianEliminationByRows(&oldBasisTransformed, &nonPivotPoints);
  if (nonPivotPoints.CardinalitySelection != 0) {
    return false;
  }
  int numNonZeroRows = nonPivotPoints.MaxSize;
  int numZeroRows = theMat.NumRows-numNonZeroRows;
  matRelationBetweenStartingVariables.init(numZeroRows, startingDim);
  for (int i = 0; i < numZeroRows; i ++) {
    for (int j = 0; j < startingDim; j ++) {
      matRelationBetweenStartingVariables.elements[i][j] = oldBasisTransformed.elements[i + numNonZeroRows][j];
    }
  }
  Vectors<Rational> theEigenSpace;
  matRelationBetweenStartingVariables.GetZeroEigenSpaceModifyMe(theEigenSpace);
  for (int i = 0; i < theEigenSpace.size; i ++) {
    theEigenSpace[i].ScaleByPositiveRationalToIntegralMinHeight();
  }
  oldBasisTransformed.ActOnVectorsColumn(theEigenSpace);
  this->basisRationalForm.init(targetDim, targetDim);
  for (int i = 0; i < targetDim; i ++) {
    for (int j = 0; j < targetDim; j ++) {
      this->basisRationalForm.elements[i][j] = theEigenSpace[i][j];
    }
  }
  this->basisRationalForm.GetMatrixIntWithDen(this->basis, this->Den);
  this->Reduce();
  return true;
}

void QuasiPolynomial::operator*=(const Rational& theConst) {
  if (theConst.IsEqualToZero()) {
    this->valueOnEachLatticeShift.size = 0;
    this->LatticeShifts.size = 0;
    return;
  }
  for (int i = 0; i < this->valueOnEachLatticeShift.size; i ++) {
    this->valueOnEachLatticeShift[i] *= theConst;
  }
}

void Cone::IntersectAHyperplane(Vector<Rational>& theNormal, Cone& outputConeLowerDim) {
  if (theNormal.IsEqualToZero()) {
    global.fatal << "Zero normal not allowed. " << global.fatal;
  }
  int theDimension = theNormal.size;
  Matrix<Rational> tempMat, theEmbedding, theProjection;
  tempMat.AssignVectorRow(theNormal);
  Vectors<Rational> theBasis;
  tempMat.GetZeroEigenSpace(theBasis);
  if (theBasis.size != theNormal.size - 1) {
    global.fatal << "Plane intersection: normals don't match. " << global.fatal;
  }
  theEmbedding.AssignVectorsToRows(theBasis);
  theEmbedding.Transpose();
  theBasis.AddOnTop(theNormal);
  Vectors<Rational> tempRoots, tempRoots2, tempRoots3;
  tempRoots.MakeEiBasis(theDimension);
  tempRoots.GetCoordsInBasis(theBasis, tempRoots2);
  theProjection.AssignVectorsToRows(tempRoots2);
  theProjection.Transpose();
  theProjection.Resize(theDimension - 1, theDimension, false);
  Vectors<Rational> newNormals = this->Normals;
  theProjection.ActOnVectorsColumn(newNormals);
  bool tempBool = outputConeLowerDim.CreateFromNormals(newNormals);
  if (tempBool) {
    global.fatal << "Create from normals failed. " << global.fatal;
  }
}

bool Cone::GetRootFromLPolyConstantTermGoesToLastVariable(Polynomial<Rational>& inputLPoly, Vector<Rational>& output) {
  if (!inputLPoly.IsLinear()) {
    return false;
  }
  output.MakeZero(inputLPoly.GetMinNumVars() + 1);
  for (int i = 0; i < inputLPoly.size(); i ++) {
    int theIndex;
    if (inputLPoly[i].::MonomialP::IsOneLetterFirstDegree(&theIndex)) {
      output[theIndex] = inputLPoly.coefficients[i];
    } else {
      *output.LastObject() = inputLPoly.coefficients[i];
    }
  }
  return true;
}

bool Cone::SolveLPolyEqualsZeroIAmProjective(Polynomial<Rational>& inputLPoly, Cone& outputCone) {
  Vector<Rational> theNormal;
  if (!this->GetRootFromLPolyConstantTermGoesToLastVariable(inputLPoly, theNormal)) {
    return false;
  }
  this->IntersectAHyperplane(theNormal, outputCone);
  return true;
}

bool Cone::SolveLQuasiPolyEqualsZeroIAmProjective(QuasiPolynomial& inputLQP, List<Cone>& outputConesOverEachLatticeShift) {
  outputConesOverEachLatticeShift.SetSize(inputLQP.LatticeShifts.size);
  bool result = true;
  for (int i = 0; i < inputLQP.LatticeShifts.size; i ++) {
    result = result && this->SolveLPolyEqualsZeroIAmProjective(
      inputLQP.valueOnEachLatticeShift[i], outputConesOverEachLatticeShift[i]
    );
  }
  return result;
}

std::string HtmlRoutines::ToHtmlTableRowsFromStringContainingJSON(const std::string& theJSON) {
  MacroRegisterFunctionWithName("HtmlRoutines::ToHtmlTableFromStringContainingJSON");
  JSData parser;
  if (!parser.readstring(theJSON)) {
    return StringRoutines::StringTrimToLengthForDisplay(theJSON, 1000);
  }
  return HtmlRoutines::ToHtmlTableRowsFromJSON(parser);
}

std::string HtmlRoutines::ToHtmlTableRowsFromJSON(const JSData& input) {
  MacroRegisterFunctionWithName("HtmlRoutines::ToHtmlTableFromJSON");
  if (input.theType == JSData::token::tokenObject) {
    std::stringstream out;
    out << "<table class = \"tableDatabaseItem\">";
    for (int i = 0; i < input.objects.size(); i ++) {
      out << "<tr><td>" << input.objects.theKeys[i] << "</td>" << "<td>"
      << HtmlRoutines::ToHtmlTableRowsFromJSON(input.objects.theValues[i]) << "</td>" << "</tr>";
    }
    out << "</table>";
    return out.str();
  }
  if (input.theType == JSData::token::tokenArray) {
    std::stringstream out;
    out << "<table class = \"tableDatabaseItem\">";
    for (int i = 0; i < input.objects.size(); i ++) {
      out << "<tr>" << "<td>"
      << HtmlRoutines::ToHtmlTableRowsFromJSON(input.objects.theValues[i]) << "</td>" << "</tr>";
    }
    out << "</table>";
    return out.str();
  }
  return input.ToString(nullptr);
}

std::string HtmlRoutines::ToHtmlTable(List<std::string>& labels, List<List<std::string> >& content, bool nestTables) {
  MacroRegisterFunctionWithName("HtmlRoutines::ToHtmlTable");
  std::stringstream out;
  out << "<table class =\"tableDatabase\">";
  out << "<tr>";
  for (int i = 0; i < labels.size; i ++) {
    out << "<th>" << StringRoutines::StringTrimToLengthForDisplay(labels[i], 1000) << "</th>";
  }
  out << "</tr>";
  for (int i = 0; i < content.size; i ++) {
    out << "<tr>";
    if (labels.size != content[i].size) {
      global.fatal << "Error: while composing table, got " << labels.size << " labels but row index " << i
      << " has " << content[i].size << " elements. " << global.fatal;
    }
    for (int j = 0; j < content[i].size; j ++) {
      if (! nestTables) {
        out << "<td>" << StringRoutines::StringTrimToLengthForDisplay(content[i][j], 1000) << "</td>";
      } else {
        out << "<td>" << HtmlRoutines::ToHtmlTableRowsFromStringContainingJSON(content[i][j]) << "</td>";
      }
    }
    out << "</tr>";
  }
  out << "</table>";
  return out.str();
}

bool HtmlRoutines::ConvertStringToHtmlStringReturnTrueIfModified(
  const std::string& input, std::string& output, bool doReplaceNewLineByBr
) {
  std::stringstream out;
  bool modified = false;
  for (unsigned int i = 0; i < input.size(); i ++) {
    bool currentlyModified = true;
    bool isReturnNewLine = false;
    if (i + 1 < input.size()) {
      isReturnNewLine = input[i] == '\r' && input[i + 1] == '\n';
    }
    if (doReplaceNewLineByBr && isReturnNewLine) {
      out << "<br>\n";
      i ++;
    } else if (input[i] == '<') {
      out << "&lt;";
    } else if (input[i] == '>') {
      out << "&gt;";
    } else if (input[i] == '&') {
      out << "&amp;";
    } else {
      out << input[i];
      currentlyModified = false;
    }
    if (currentlyModified) {
      modified = true;
    }
  }
  output = out.str();
  return modified;
}

bool HtmlRoutines::IsRepresentedByItselfInURLs(char input) {
  if (MathRoutines::isADigit(input)) {
    return true;
  }
  if (MathRoutines::isALatinLetter(input)) {
    return true;
  }
  return input == '.' || input == '-' || input == '_';
}

std::string HtmlRoutines::ConvertStringToURLStringExceptDashesAndSlashes(const std::string& input) {
  std::stringstream out;
  for (unsigned int i = 0; i < input.size(); i ++) {
    if (
      HtmlRoutines::IsRepresentedByItselfInURLs(input[i]) ||
      input[i] == '-' ||
      input[i] == '/'
    ) {
      out << input[i];
    } else {
      out << "%";
      int x = input[i];
      out << std::hex << ((x / 16) % 16) << (x % 16) << std::dec;
    }
  }
  return out.str();
}

std::string HtmlRoutines::ConvertStringToURLString(const std::string& input, bool usePlusesForSpacebars) {
  std::stringstream out;
  for (unsigned int i = 0; i < input.size(); i ++) {
    if (HtmlRoutines::IsRepresentedByItselfInURLs(input[i])) {
      out << input[i];
    } else if (input[i] == ' ' && usePlusesForSpacebars) {
      out << '+';
    } else {
      out << "%";
      int x = input[i];
      out << std::hex << ((x / 16) % 16) << (x % 16) << std::dec;
    }
  }
  return out.str();
}

Vector<Rational> oneFracWithMultiplicitiesAndElongations::GetCheckSumRoot(int NumVars) {
  Vector<Rational> output;
  output.SetSize(NumVars);
  for (int i = 0; i < NumVars; i ++) {
    output[i] = i + 2;
  }
  return output;
}

bool PartFractions::RemoveRedundantShortRootsIndex(int theIndex, Vector<Rational>* Indicator) {
  if (!(*this)[theIndex].rootIsInFractionCone(*this, Indicator)) {
    return false;
  }
  bool found = false;
  for (int k = 0; k < (*this)[theIndex].IndicesNonZeroMults.size; k ++) {
    int currentIndex = (*this)[theIndex].IndicesNonZeroMults[k];
    const oneFracWithMultiplicitiesAndElongations& currentFrac = (*this)[theIndex][currentIndex];
    if (currentFrac.Elongations.size > 1) {
      found = true;
      break;
    }
  }
  if (!found) {
    return false;
  }
  PartFraction thePF;
  Rational localStartCheckSum, localEndCheckSum;
  std::string tempS;
  Polynomial<LargeInteger> tempIP, currentCoeff;
  this->PopMonomial(theIndex, thePF, currentCoeff);
  for (int k = 0; k < thePF.IndicesNonZeroMults.size; k ++) {
    int currentIndex = thePF.IndicesNonZeroMults[k];
    oneFracWithMultiplicitiesAndElongations& currentFrac = thePF[currentIndex];
    int LCMElongations = currentFrac.GetLCMElongations();
    tempS = this->startingVectors[currentIndex].ToString();
    while (currentFrac.Elongations.size > 1) {
      for (int i = 0; i < currentFrac.Elongations.size; i ++) {
        int ElongationValue = currentFrac.Elongations[i];
        if (ElongationValue != LCMElongations) {
          int numSummands = LCMElongations / ElongationValue;
          thePF.GetNElongationPoly(
            this->startingVectors, currentIndex, ElongationValue, numSummands, tempIP, this->AmbientDimension
          );
          tempIP.RaiseToPower(currentFrac.Multiplicities[i]);
          currentCoeff *= tempIP;
          currentFrac.AddMultiplicity(currentFrac.Multiplicities[i], LCMElongations);
          currentFrac.AddMultiplicity(- currentFrac.Multiplicities[i], ElongationValue);
          if (!localEndCheckSum.IsEqualTo(localStartCheckSum)) {
            global.fatal << "Local end checksum must be zero. " << global.fatal;
          }
        }
      }
    }
  }
  this->AddMonomial(thePF, currentCoeff);
  return true;
}

void Lattice::GetRougherLatticeFromAffineHyperplaneDirectionAndLattice(
  const Vector<Rational>& theDirection,
  Vector<Rational>& outputDirectionMultipleOnLattice,
  Vector<Rational>& theShift,
  Vector<Rational>& theAffineHyperplane,
  Vectors<Rational>& outputRepresentatives,
  Vectors<Rational>& movementInDirectionPerRepresentative,
  Lattice& outputRougherLattice
) {
  Vector<Rational> theNormal = theAffineHyperplane;
  theNormal.SetSize(theNormal.size - 1);
  if (theDirection.ScalarEuclidean(theNormal).IsEqualToZero()) {
    return;
  }
  Rational theConstOnTheOtherSide = - *theAffineHyperplane.LastObject();
  Vectors<Rational> theBasis;
  theBasis.AssignMatrixRows(this->basisRationalForm);
  Lattice theHyperplaneLatticeNoShift, theDirectionLattice;//, normalProjectionLattice, theTrueProjectionLattice;
  Vectors<Rational> tempRoots; //Vector<Rational> tempRoot;
  tempRoots.AddOnTop(theDirection);
  theDirectionLattice = *this;
  theDirectionLattice.IntersectWithLinearSubspaceSpannedBy(tempRoots);
  theDirectionLattice.basisRationalForm.GetVectorFromRow(0, outputDirectionMultipleOnLattice);
  theHyperplaneLatticeNoShift = *this;
  theHyperplaneLatticeNoShift.IntersectWithLinearSubspaceGivenByNormal(theNormal);
  tempRoots.AssignMatrixRows(theHyperplaneLatticeNoShift.basisRationalForm);
  tempRoots.AddOnTop(outputDirectionMultipleOnLattice);
  outputRougherLattice.MakeFromRoots(tempRoots);
  this->GetAllRepresentatives(outputRougherLattice, outputRepresentatives);
  for (int i = 0; i < outputRepresentatives.size; i ++) {
    outputRepresentatives[i] += theShift;
    outputRougherLattice.ReduceVector(outputRepresentatives[i]);
  }
  Rational theShiftedConst, unitMovement, tempRat;
  unitMovement = theNormal.ScalarEuclidean(outputDirectionMultipleOnLattice);
  movementInDirectionPerRepresentative.SetSize(outputRepresentatives.size);
  for (int i = 0; i < outputRepresentatives.size; i ++) {
    tempRat = (theNormal.ScalarEuclidean(outputRepresentatives[i]) - theConstOnTheOtherSide) / unitMovement;
    tempRat.AssignFracValue();
    theShiftedConst = theConstOnTheOtherSide + tempRat;
    Vector<Rational>& currentMovement =movementInDirectionPerRepresentative[i];
    currentMovement = theAffineHyperplane;
    *currentMovement.LastObject() = theShiftedConst;
  }
}

bool slTwoInSlN::ComputeInvariantsOfDegree(
  List<int>& decompositionDimensions, int theDegree, List<Polynomial<Rational> >& output, std::string& outputError
) {
  this->initFromModuleDecomposition(decompositionDimensions, false, false);
  SelectionWithMaxMultiplicity theSel;
  theSel.initMaxMultiplicity(this->theDimension, theDegree);
  outputError = "";
  int numCycles = theSel.NumCombinationsOfCardinality(theDegree);
  if (numCycles <= 0) {
    outputError = " Computation too large. ";
    return false;
  }
  Polynomial<Rational> basisMonsZeroWeight, basisMonsAll;
  basisMonsZeroWeight.MakeZero();
  basisMonsZeroWeight.SetExpectedSize(numCycles);
  basisMonsAll.MakeZero();
  basisMonsAll.SetExpectedSize(numCycles);
  MonomialP theMon;
  theMon.MakeOne(this->theDimension);
  Vector<Rational> theWeight;
  Vector<Rational> theCartanAction;
  theCartanAction.SetSize(this->theDimension);
  theWeight.SetSize(this->theDimension);
  for (int j = 0; j < this->theDimension; j ++) {
    theCartanAction[j] = this->theH.elements[j][j];
  }
  theSel.IncrementSubsetFixedCardinality(theDegree);
  Rational theMonCoeff = 1;
  for (int i = 0; i < numCycles; i ++, theSel.IncrementSubsetFixedCardinality(theDegree)) {
    for (int j = 0; j < this->theDimension; j ++) {
      theMon[j] = theSel.Multiplicities[j];
      theWeight[j] = theMon[j];
    }
    basisMonsAll.AddMonomial(theMon, theMonCoeff);
    if (theWeight.ScalarEuclidean(theCartanAction).IsEqualToZero()) {
      basisMonsZeroWeight.AddMonomial(theMon, theMonCoeff);
    }
  }
  Matrix<Rational> tempMat;
  tempMat.init(basisMonsAll.size() * 2, basisMonsZeroWeight.size());
  Polynomial<Rational>  tempP;
  for (int l = 0; l < 2; l ++) {
    for (int k = 0; k < basisMonsZeroWeight.size(); k ++) {
      if (l == 0) {
        this->theE.ActOnMonomialAsDifferentialOperator(basisMonsZeroWeight[k], tempP);
      } else {
        this->theF.ActOnMonomialAsDifferentialOperator(basisMonsZeroWeight[k], tempP);
      }
      for (int j = 0; j < basisMonsAll.size(); j ++) {
        int indexInResult = tempP.theMonomials.GetIndex(basisMonsAll[j]);
        int currentRow = l * basisMonsAll.size() + j;
        if (indexInResult == - 1) {
          tempMat.elements[currentRow][k] = 0;
        } else {
          tempMat.elements[currentRow][k] = tempP.coefficients[indexInResult];
        }
      }
    }
  }
  Vectors<Rational> tempRoots;
  tempMat.GetZeroEigenSpaceModifyMe(tempRoots);
  output.SetSize(tempRoots.size);
  for (int i = 0; i < output.size; i ++) {
    Polynomial<Rational>& current = output[i];
    current.MakeZero();
    for (int j = 0; j < basisMonsZeroWeight.size(); j ++) {
      if (!tempRoots[i][j].IsEqualToZero()) {
        theMon = basisMonsZeroWeight[j];
        current.AddMonomial(theMon, tempRoots[i][j]);
      }
    }
  }
  return true;
}

std::string DrawingVariables::GetColorPsTricksFromColorIndex(int colorIndex) {
  std::stringstream out;
  int r = (colorIndex / 65536) % 256;
  int g = (colorIndex / 256) % 256;
  int b = colorIndex % 256;
  out << "\\\\newrgbcolor{currentColor}{" << (static_cast<double>(r) / 255) << ", "
  << ((static_cast<double>(g)) / 255) << ", " << ((static_cast<double>(b)) / 255) << "}";
  return out.str();
}

bool DrawingVariables::GetColorIntFromColorString(const std::string& input, int& output) {
  if (input == "blue") {
    output = static_cast<int>(HtmlRoutines::RedGreenBlue(0, 0, 255));
    return true;
  }
  if (input == "green") {
    output = static_cast<int>(HtmlRoutines::RedGreenBlue(0, 255, 0));
    return true;
  }
  if (input == "red") {
    output = static_cast<int>(HtmlRoutines::RedGreenBlue(255, 0, 0));
    return true;
  }
  if (input == "cyan") {
    output = static_cast<int>(HtmlRoutines::RedGreenBlue(0, 255, 255));
    return true;
  }
  if (input == "orange") {
    output = static_cast<int>(HtmlRoutines::RedGreenBlue(255, 127, 0));
    return true;
  }
  return false;
}

std::string DrawingVariables::GetColorHtmlFromColorIndex(int colorIndex) {
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

std::string ConeLatticeAndShift::ToString(FormatExpressions& theFormat) {
  std::stringstream out;
  out << this->theProjectivizedCone.ToString(&theFormat);
  out << "<br>Shift +lattice: " << this->theShift.ToString() << " + " << this->theLattice.ToString();
  return out.str();
}

void DrawOperations::MakeMeAStandardBasis(int theDim) {
  if (theDim < 1) {
    return;
  }
  if (theDim > 3) {
    this->ProjectionsEiVectors.SetSizeMakeMatrix(theDim, 2);
    for (int i = 0; i < theDim; i ++) {
      this->ProjectionsEiVectors[i][0] = FloatingPoint::Sin(static_cast<double>(i) / static_cast<double>(theDim * MathRoutines::Pi()));
      this->ProjectionsEiVectors[i][1] = FloatingPoint::Cos(static_cast<double>(i) / static_cast<double>(theDim * MathRoutines::Pi()));
    }
  } else if (theDim == 3) {
    this->ProjectionsEiVectors.SetSizeMakeMatrix(3, 2);
    this->ProjectionsEiVectors[0][0] = 1;
    this->ProjectionsEiVectors[0][1] = - 0.2;
    this->ProjectionsEiVectors[1][0] = 0.1;
    this->ProjectionsEiVectors[1][1] = 1;
    this->ProjectionsEiVectors[2][0] = 0.01;
    this->ProjectionsEiVectors[2][1] = 0.01;
  } else {
    this->ProjectionsEiVectors.SetSizeMakeMatrix(2, 2);
    this->ProjectionsEiVectors[0][0] = 1;
    this->ProjectionsEiVectors[0][1] = 0;
    this->ProjectionsEiVectors[1][0] = 0;
    this->ProjectionsEiVectors[1][1] = - 1;
  }
  if (this->BasisProjectionPlane.size < 1)
    this->BasisProjectionPlane.SetSize(1);
  this->BasisProjectionPlane.MakeEiBasis(theDim);
  this->BasisProjectionPlane.SetSize(2);
  if (theDim != 3) {
    for (int i = 0; i < this->BasisProjectionPlane[1].size; i ++) {
      this->BasisProjectionPlane[1][i] = 2 * i + 1;
    }
    for (int i = 0; i < this->BasisProjectionPlane[0].size; i ++) {
      this->BasisProjectionPlane[0][i] = 3 * i + 2;
    }
  } else if (theDim == 3) {//<-if not needed but good for documentation purposes
    this->BasisProjectionPlane[0][0] = 0.6;
    this->BasisProjectionPlane[0][1] = 0.4;
    this->BasisProjectionPlane[0][2] = 0;
    this->BasisProjectionPlane[1][0] = - 0.4;
    this->BasisProjectionPlane[1][1] = 0.6;
    this->BasisProjectionPlane[1][2] = 1;
  }

  if (this->theBilinearForm.NumRows != theDim) {
    this->theBilinearForm.MakeIdMatrix(theDim, 1, 0);
  }
}

std::string ConeComplex::DrawMeToHtmlLastCoordAffine(DrawingVariables& theDrawingVariables, FormatExpressions& theFormat) {
  bool isBad = false;
  isBad = this->DrawMeLastCoordAffine(true, theDrawingVariables, theFormat);
  std::stringstream out;
  out << theDrawingVariables.GetHtmlDiv(this->GetDim() - 1);
  if (isBad) {
    out << "<hr>" << "found cones which I can't draw<hr>";
  }
  out << this->ToString(true);
  return out.str();
}

std::string ConeComplex::DrawMeToHtmlProjective(DrawingVariables& theDrawingVariables, FormatExpressions& theFormat) {
  bool isGood = true;
  isGood = this->DrawMeProjective(nullptr, true, theDrawingVariables, theFormat);
  std::stringstream out;
  out << theDrawingVariables.GetHtmlDiv(this->GetDim());
  if (!isGood) {
    out << "<hr>" << "found cones which I can't draw<hr>";
  }
  out << this->ToString(true);
  return out.str();
}

bool ConeComplex::DrawMeLastCoordAffine(
  bool InitDrawVars, DrawingVariables& theDrawingVariables, FormatExpressions& theFormat
) {
  bool result = true;
  if (InitDrawVars) {
    theDrawingVariables.theBuffer.initDimensions(this->GetDim() - 1);
  }

  theDrawingVariables.drawCoordSystemBuffer(theDrawingVariables, this->GetDim() - 1);
  for (int i = 0; i < this->size; i ++) {
    result = this->TheObjects[i].DrawMeLastCoordAffine(InitDrawVars, theDrawingVariables, theFormat) && result;
    std::stringstream tempStream;
    tempStream << i + 1;
    Vector<Rational> tempRoot = this->TheObjects[i].GetInternalPoint();
    tempRoot.MakeAffineUsingLastCoordinate();
    theDrawingVariables.drawTextAtVectorBufferRational(tempRoot, tempStream.str(), "black");
  }
  return result;
}

bool ConeComplex::DrawMeProjective(
  Vector<Rational>* coordCenterTranslation, bool InitDrawVars, DrawingVariables& theDrawingVariables, FormatExpressions& theFormat
) {
  bool result = true;
  Vector<Rational> tempRoot;
  Vectors<Rational> tempRoots;
  Matrix<Rational> tempMat;
  if (this->GetDim() <= 1) {
    return false;
  }
  if (InitDrawVars) {
    theDrawingVariables.theBuffer.init();
    theDrawingVariables.theBuffer.initDimensions(this->GetDim());
    theDrawingVariables.theBuffer.MakeMeAStandardBasis(this->GetDim());
    theDrawingVariables.drawCoordSystemBuffer(theDrawingVariables, this->GetDim());
    if (this->GetDim() > 2) {
      this->ConvexHull.GetInternalPoint(tempRoot);
      tempMat.AssignVectorRow(tempRoot);
      tempMat.GetZeroEigenSpace(tempRoots);
      for (int i = 0; i < 2; i ++) {
        for (int j = 0; j < this->GetDim(); j ++) {
          theDrawingVariables.theBuffer.BasisProjectionPlane[i][j] = tempRoots[i][j].GetDoubleValue();
        }
      }
    }
  }
  for (int i = 0; i < this->size; i ++) {
    result = ((*this)[i].DrawMeProjective(coordCenterTranslation, false, theDrawingVariables, theFormat) && result);
  }
  return result;
}

bool Cone::DrawMeLastCoordAffine(
  bool InitDrawVars, DrawingVariables& theDrawingVariables, FormatExpressions& theFormat, const std::string& ChamberWallColor
) const {
  (void) theFormat; //avoid unused parameter warning, portable.
  Vector<Rational> ZeroRoot;
  ZeroRoot.MakeZero(this->GetDim() - 1);
  Vectors<Rational> VerticesScaled;
  VerticesScaled = this->Vertices;
  Rational tempRat;
  List<bool> DrawVertex;
  DrawVertex.initializeFillInObject(this->Vertices.size, true);
  bool foundBadVertex = false;
  for (int i = 0; i < this->Vertices.size; i ++) {
    tempRat = *VerticesScaled[i].LastObject();
    VerticesScaled[i].SetSize(this->GetDim() - 1);
    if (tempRat.IsPositive()) {
      VerticesScaled[i] /= tempRat;
    }
    if (tempRat.IsEqualToZero()) {
      VerticesScaled[i] *= 10000;
    }
    if (tempRat.IsNegative()) {
      DrawVertex[i] = false;
      foundBadVertex = true;
    }
  }
  if (InitDrawVars) {
    theDrawingVariables.drawCoordSystemBuffer(theDrawingVariables, this->GetDim() - 1);
  }
  for (int k = 0; k < this->Normals.size; k ++) {
    for (int i = 0; i < VerticesScaled.size; i ++) {
      if (DrawVertex[i] && this->Normals[k].ScalarEuclidean(this->Vertices[i]).IsEqualToZero()) {
        for (int j = i + 1; j < VerticesScaled.size; j ++) {
          if (DrawVertex[j] && this->Normals[k].ScalarEuclidean(this->Vertices[j]).IsEqualToZero()) {
            if (this->IsAnHonest1DEdgeAffine(i, j)) {
              /*bool iVertexLiesAtInfinity = this->Vertices[i].LastObject()->IsEqualToZero();
              bool jVertexLiesAtInfinity = this->Vertices[j].LastObject()->IsEqualToZero();
              if (iVertexLiesAtInfinity || jVertexLiesAtInfinity) {
                iScaledVertex =VerticesScaled[i];
                jScaledVertex =VerticesScaled[j];
                if (iVertexLiesAtInfinity)
                  iScaledVertex*=10000;
                if (jVertexLiesAtInfinity)
                  jScaledVertex*=10000;
                theDrawingVariables.drawLineBetweenTwoVectorsBuffer
                (iScaledVertex, jScaledVertex, theDrawingVariables.PenStyleNormal, HtmlRoutines::RedGreenBlue(200,200,200));
              }*/
              theDrawingVariables.drawLineBetweenTwoVectorsBufferRational(
                VerticesScaled[i], VerticesScaled[j], ChamberWallColor, 1
              );
            }
          }
        }
      }
    }
  }
  return foundBadVertex;
}

std::string Cone::DrawMeToHtmlLastCoordAffine(DrawingVariables& theDrawingVariables, FormatExpressions& theFormat) {
  if (this->flagIsTheZeroCone) {
    return "The cone is empty. ";
  }
  if (this->Normals.size < 1) {
    return "The cone is the entire space";
  }
  if (this->Vertices.size < 1) {
    return "The cone is empty";
  }
  std::stringstream out;
  theDrawingVariables.theBuffer.MakeMeAStandardBasis(this->GetDim() - 1);
  bool foundBadVertex = this->DrawMeLastCoordAffine(false, theDrawingVariables, theFormat);
  theDrawingVariables.drawCoordSystemBuffer(theDrawingVariables, this->GetDim() - 1);
  if (foundBadVertex) {
    out << "<br>The cone does not lie in the upper half-space. ";
  } else {
    out << theDrawingVariables.GetHtmlDiv(this->GetDim() - 1);
  }
  out << "<br>" << this->ToString(&theFormat);
  return out.str();
}

bool Cone::DrawMeProjective(
  Vector<Rational>* coordCenterTranslation,
  bool initTheDrawVars,
  DrawingVariables& theDrawingVariables,
  FormatExpressions& theFormat
) const {
  (void) theFormat;  //avoid unused parameter warning, portable.
  Vector<Rational> ZeroRoot, coordCenter;
  ZeroRoot.MakeZero(this->GetDim());
  if (coordCenterTranslation == nullptr) {
    coordCenter = ZeroRoot;
  } else {
    coordCenter = *coordCenterTranslation;
  }
  Vectors<Rational> VerticesScaled = this->Vertices;
  for (int i = 0; i < VerticesScaled.size; i ++) {
    Rational sumAbsValuesCoords = 0;
    for (int j = 0; j < this->GetDim(); j ++) {
      sumAbsValuesCoords += (VerticesScaled[i][j].IsPositive()) ? VerticesScaled[i][j] : - VerticesScaled[i][j];
    }
    if (sumAbsValuesCoords.IsEqualToZero()) {
      global.fatal << "Zero vector not allowed. " << global.fatal;
    }
    VerticesScaled[i] /= sumAbsValuesCoords;
  }
  Vector<Rational> tempRoot;
  if (initTheDrawVars) {
    theDrawingVariables.theBuffer.MakeMeAStandardBasis(this->GetDim());
    for (int i = 0; i < this->GetDim(); i ++) {
      tempRoot.MakeEi(this->GetDim(), i);
      theDrawingVariables.drawLineBetweenTwoVectorsBufferRational(
        ZeroRoot + coordCenter, tempRoot + coordCenter, "gray", 1
      );
    }
  }
  for (int i = 0; i < this->Vertices.size; i ++) {
    theDrawingVariables.drawLineBetweenTwoVectorsBufferRational(
      ZeroRoot + coordCenter, VerticesScaled[i] * 10000 + coordCenter, "gray", 1
    );
  }
  for (int k = 0; k < this->Normals.size; k ++) {
    for (int i = 0; i < this->Vertices.size; i ++) {
      if (this->Normals[k].ScalarEuclidean(this->Vertices[i]).IsEqualToZero()) {
        for (int j = i + 1; j < this->Vertices.size; j ++) {
          if (this->Normals[k].ScalarEuclidean(this->Vertices[j]).IsEqualToZero()) {
            if (this->IsAnHonest1DEdgeAffine(i, j)) {
              theDrawingVariables.drawLineBetweenTwoVectorsBufferRational(
                VerticesScaled[i] + coordCenter, VerticesScaled[j] + coordCenter, "black", 1
              );
            }
          }
        }
      }
    }
  }
  return true;
}

std::string Cone::DrawMeToHtmlProjective(DrawingVariables& theDrawingVariables, FormatExpressions& theFormat) {
  if (this->flagIsTheZeroCone) {
    return "The cone is the zero cone (i.e. contains only the origin).";
  }
  if (this->Normals.size <= 0) {
    return "The cone is the entire space.";
  }
  std::stringstream out;
  if (this->Vertices.size < 1) {
    out << "There has been a programming error. The cone is empty.<br>"
    << this->ToString(&theFormat);
    return out.str();
  }
  theDrawingVariables.theBuffer.MakeMeAStandardBasis(this->GetDim());
  this->DrawMeProjective(nullptr, true, theDrawingVariables, theFormat);
  theDrawingVariables.drawCoordSystemBuffer(theDrawingVariables, this->GetDim());
  out << theDrawingVariables.GetHtmlDiv(this->GetDim());
  out << "<br>" << this->ToString(&theFormat);
  return out.str();
}

std::string HtmlRoutines::GetHtmlButton(const std::string& buttonID, const std::string& theScript, const std::string& buttonText) {
  std::stringstream out;
  out << "\n<button id =\"" << buttonID << "\" " << HtmlRoutines::GetStyleButtonLikeHtml()
  << " onclick=\"" << theScript << "\">" << buttonText << "</button>";
  return out.str();
}

std::string HtmlRoutines::GetHtmlSpanHidableStartsHiddeN(
  const std::string& input, const std::string& labelExpandButton, const std::string& desiredIdStart
) {
  std::stringstream out;
  HtmlRoutines::GlobalGeneralPurposeID ++;
  std::stringstream buttonLabel;
  std::stringstream spanLabel;
  spanLabel << "hidableSpan" << desiredIdStart << HtmlRoutines::GlobalGeneralPurposeID;
  buttonLabel << "buttonHS" << HtmlRoutines::GlobalGeneralPurposeID;
  std::string switchMenu = "window.calculator.miscellaneousFrontend.switchMenu('" + spanLabel.str() + "');";
  out << HtmlRoutines::GetHtmlButton(buttonLabel.str(), switchMenu, labelExpandButton);
  out << "<span";
  out << " id =\"" << spanLabel.str() << "\" class=\"hiddenClass\">";
  out << input << "</span>";
  return out.str();
}

int DrawOperations::GetDimFromBilinearForm() {
  return this->theBilinearForm.NumRows;
}

void DrawOperations::initDimensions(int theDim) {
  if (theDim < 2) {
    theDim = 2;
  }
  this->theBilinearForm.MakeIdMatrix(theDim, 1, 0);
  this->ProjectionsEiVectors.SetSizeMakeMatrix(theDim, 2);
  this->BasisProjectionPlane.MakeEiBasis(theDim);
  this->BasisProjectionPlane.size = 2;
/*  for (int i = 0; i < tempBasis[1].size; i ++)
    tempBasis[1][i] =2*i + 1;
  for (int i = 0; i < tempBasis[0].size; i ++)
    tempBasis[0][i] =3*i +2;*/
  this->ModifyToOrthonormalNoShiftSecond(this->BasisProjectionPlane[1], this->BasisProjectionPlane[0]);
  this->BasisToDrawCirclesAt.MakeEiBasis(theDim);
  this->SelectedCircleMinus2noneMinus1Center = - 2;
  this->centerX = 300;
  this->centerY = 300;
  this->GraphicsUnit = DrawOperations::GraphicsUnitDefault;
  this->frameLengthInMilliseconds = 500;
}

int DrawOperations::GetDimFirstDimensionDependentOperation() {
  for (int i = 0; i < this->theOperations.size; i ++) {
    if (this->theOperations[i][fieldOperation].theString == DrawOperations::typeSegment) {
      if (this->theOperations[i][DrawOperations::fieldPoints].theList.size > 0) {
        return this->theOperations[i][DrawOperations::fieldPoints][0].theList.size;
      }
    }
  }
  return - 1;
}

void DrawOperations::EnsureProperInitialization() {
  int theDim = this->GetDimFirstDimensionDependentOperation();
  bool isGood = (this->ProjectionsEiVectors.size == theDim && this->theBilinearForm.NumRows == theDim);
  if (isGood) {
    isGood = this->BasisProjectionPlane.size == 2;
  }
  if (isGood) {
    isGood = this->BasisProjectionPlane[0].size == theDim;
  }
  if (!isGood) {
    this->initDimensions(theDim);
  }
}

template<class Base>
std::iostream& operator<< (std::iostream& output, const CompleX<Base>& input) {
  if (input.IsEqualToZero()) {
    output << "0";
    return output;
  }
  if (input.Re != 0) {
    output << input.Re;
    if (input.Im != 0) {
      output << " + ";
    }
  }
  if (input.Im != 0) {
    output << "i";
  }
  if (input.Im < 0) {
    output << "(";
  }
  if (input.Im != 1) {
    output << input.Im;
  }
  if (input.Im < 0) {
    output << ")";
  }
  return output;
}

void DrawOperations::init() {
  this->theOperations.Reserve(1000);
  this->centerX = 300;
  this->centerY = 300;
  this->GraphicsUnit = DrawOperations::GraphicsUnitDefault;
  this->ClickToleranceX = 5;
  this->ClickToleranceY = 5;
  this->SelectedCircleMinus2noneMinus1Center = - 2;
  this->flagRotatingPreservingAngles = true;
  this->flagAnimatingMovingCoordSystem = false;
  this->flagIsPausedWhileAnimating = false;
}

double DrawOperations::getAngleFromXandY(double x, double y) {
  double result;
  if (x != 0.0) {
    result = FloatingPoint::Arctan(y / x);
  } else {
    if (y > 0) {
      result = MathRoutines::Pi() / 2;
    } else {
      result = MathRoutines::Pi() / (- 2);
    }
  }
  return result;
}

void DrawOperations::click(double x , double y) {
  this->EnsureProperInitialization();
  this->SelectedCircleMinus2noneMinus1Center = - 2;
  if (this->AreWithinClickTolerance(x, y, this->centerX, this->centerY)) {
    this->SelectedCircleMinus2noneMinus1Center = - 1;
  }
  int theDim = this->theBilinearForm.NumRows;
  for (int i = 0; i < theDim; i ++) {
    double Xbasis, Ybasis;
    this->GetCoordsDrawingComputeAll(this->BasisToDrawCirclesAt[i], Xbasis, Ybasis);
    if (this->AreWithinClickTolerance(x, y, Xbasis, Ybasis)) {
      this->SelectedCircleMinus2noneMinus1Center = i;
      return;
    }
  }
}

void DrawOperations::RotateOutOfPlane(
  std::stringstream& logger,
  Vector<double>& input,
  Vector<double>& output,
  Vector<double>& orthoBasis1,
  Vector<double>& orthoBasis2,
  double oldTanSquared,
  double newTanSquared
) {
  Vector<double> projection = orthoBasis1;
  Vector<double> vComponent = input;
  double scal1 = this->theBilinearForm.ScalarProduct(orthoBasis1, input);
  double scal2 = this->theBilinearForm.ScalarProduct(orthoBasis2, input);
  projection *= scal1;
  projection += orthoBasis2 * scal2;
  vComponent -= projection;
  logger << "\ngetScalarProd =" << this->theBilinearForm.ScalarProduct(projection, vComponent);
  if (oldTanSquared < 0 || newTanSquared < 0) {
    return;
  }
  double oldAngle = FloatingPoint::Arctan(FloatingPoint::Sqrt(oldTanSquared));
  double newAngle = FloatingPoint::Arctan(FloatingPoint::Sqrt(newTanSquared));
  double angleChange = - oldAngle + newAngle;
  projection = orthoBasis1;
  projection *= FloatingPoint::Cos(angleChange) * scal1 - FloatingPoint::Sin(angleChange) * scal2;
  projection += orthoBasis2 * (FloatingPoint::Sin(angleChange) * scal1 + FloatingPoint::Sin(angleChange) * scal2);
  output = vComponent;
  output += projection;
}

void DrawOperations::ModifyToOrthonormalNoShiftSecond(Vector<double>& root1, Vector<double>& root2) {
  //if  (this->getScalarProduct(root2, root2) == 0)
  //  root2.MakeEi(this->theWeyl.CartanSymmetric.NumRows,1);
  double theScalar = this->theBilinearForm.ScalarProduct(root1, root2) / this->theBilinearForm.ScalarProduct(root2, root2);
  root1 -= root2 * theScalar;
  this->ScaleToUnitLength(root1);
  this->ScaleToUnitLength(root2);
}

void DrawOperations::ComputeProjectionsEiVectors() {
  int theDimension = this->theBilinearForm.NumRows;
  this->ProjectionsEiVectors.SetSizeMakeMatrix(theDimension, 2);
  Vector<double> tempRoot;
  for (int i = 0; i < theDimension; i ++) {
    tempRoot.MakeEi(theDimension, i);
    this->ProjectionsEiVectors[i][0] = this->theBilinearForm.ScalarProduct(tempRoot, this->BasisProjectionPlane[0]);
    this->ProjectionsEiVectors[i][1] = this->theBilinearForm.ScalarProduct(tempRoot, this->BasisProjectionPlane[1]);
  }
}

void DrawOperations::changeBasisPReserveAngles(double newX, double newY) {
  double bufferCenterX = this->centerX;
  double bufferCenterY = this->centerY;
  double bufferGraphicsUnit = this->GraphicsUnit;
  newX = (newX - bufferCenterX) / bufferGraphicsUnit;
  newY = (newY - bufferCenterY) / bufferGraphicsUnit;
  if (newX == 0.0 && newY == 0.0) {
    return;
  }
  std::stringstream out;
  Vector<double>& selectedRoot = this->BasisToDrawCirclesAt[this->SelectedCircleMinus2noneMinus1Center];
  double selectedRootLength = this->theBilinearForm.ScalarProduct(selectedRoot, selectedRoot);
  double oldX, oldY;
  this->GetCoordsDrawingComputeAll(selectedRoot, oldX, oldY);
  oldX = (oldX - bufferCenterX) / bufferGraphicsUnit;
  oldY = (oldY - bufferCenterY) / bufferGraphicsUnit;

  double oldAngle = getAngleFromXandY(oldX, oldY);
  double newAngle = getAngleFromXandY(newX, newY);
  double AngleChange = - newAngle + oldAngle;
  double epsilon = 0.000000000001;
  while (AngleChange > MathRoutines::Pi() / 2 + epsilon) {
    AngleChange -= MathRoutines::Pi();
  }
  while (AngleChange <= MathRoutines::Pi() / (- 2) - epsilon) {
    AngleChange += MathRoutines::Pi();
  }
  out << "\nold angle: " << oldAngle;
  out << "\nnew angle:  " << newAngle;
  Vector<double> NewVectorE1, NewVectorE2;
  Vectors<double>& currentBasisPlane = this->BasisProjectionPlane;
  NewVectorE1 = currentBasisPlane[0] * FloatingPoint::Cos(AngleChange);
  NewVectorE1 += currentBasisPlane[1] * FloatingPoint::Sin(AngleChange);
  NewVectorE2 = currentBasisPlane[1] * FloatingPoint::Cos(AngleChange);
  NewVectorE2 += currentBasisPlane[0] * (- FloatingPoint::Sin(AngleChange));
  currentBasisPlane[0] = NewVectorE1;
  currentBasisPlane[1] = NewVectorE2;
  double RootTimesE1 = this->theBilinearForm.ScalarProduct(selectedRoot, currentBasisPlane[0]);
  double RootTimesE2 = this->theBilinearForm.ScalarProduct(selectedRoot, currentBasisPlane[1]);
  Vector<double> vOrthogonal = selectedRoot;
  Vector<double> vProjection = currentBasisPlane[0] * RootTimesE1;
  vProjection += currentBasisPlane[1] * RootTimesE2;
  vOrthogonal -= vProjection;
  double oldRatioProjectionOverHeightSquared = (oldX * oldX + oldY * oldY) / (selectedRootLength - oldX * oldX - oldY * oldY);
  double newRatioProjectionOverHeightSquared = (newX * newX + newY * newY) / (selectedRootLength - newX * newX - newY * newY);
  out << "\noldRatio: " << oldRatioProjectionOverHeightSquared;
  out << "\nnewRatio: " << newRatioProjectionOverHeightSquared;
  if (
    this->theBilinearForm.ScalarProduct(vOrthogonal, vOrthogonal) > epsilon ||
    this->theBilinearForm.ScalarProduct(vOrthogonal, vOrthogonal) < - epsilon
  ) {
    this->ScaleToUnitLength(vProjection);
    this->ScaleToUnitLength(vOrthogonal);
    out << "\nscaled vOrthogonal =" << vOrthogonal << "->"
    << this->theBilinearForm.ScalarProduct(vOrthogonal, vOrthogonal);
    out << "\nscaled vProjection =" << vProjection << "->"
    << this->theBilinearForm.ScalarProduct(vProjection, vProjection);
    out << "\ntheScalarProd: " << this->theBilinearForm.ScalarProduct(vOrthogonal, vProjection);
    this->RotateOutOfPlane(
      out,
      currentBasisPlane[0],
      currentBasisPlane[0],
      vProjection,
      vOrthogonal,
      oldRatioProjectionOverHeightSquared,
      newRatioProjectionOverHeightSquared
    );
    this->RotateOutOfPlane(
      out,
      currentBasisPlane[1],
      currentBasisPlane[1],
      vProjection,
      vOrthogonal,
      oldRatioProjectionOverHeightSquared,
      newRatioProjectionOverHeightSquared
    );
  }
  this->ModifyToOrthonormalNoShiftSecond(currentBasisPlane[0], currentBasisPlane[1]);
  out << "\ne1=" << currentBasisPlane[0];
  out << "\ne2=" << currentBasisPlane[1];
  out << "\ne1*e2=" << this->theBilinearForm.ScalarProduct(currentBasisPlane[0], currentBasisPlane[1]);
  if (this->specialOperationsOnBasisChange != nullptr) {
    this->specialOperationsOnBasisChange(*this);
  }
  this->ComputeProjectionsEiVectors();
  this->DebugString = out.str();
}

class ImpreciseDouble {
  private:
  double precision;
  double theValue;
  public:
  std::string ToString(FormatExpressions* theFormat = nullptr) const {
    (void) theFormat; //avoid unused parameter warning, portable.
    std::stringstream out;
    out << this->theValue;
    return out.str();
  }
  void operator=(const ImpreciseDouble& other) {
    this->theValue = other.theValue;
    this->precision = other.precision;
  }
  void operator=(double other) {
    this->theValue = other;
  }
  ImpreciseDouble(const ImpreciseDouble& other) {
    this->operator=(other);
  }
  ImpreciseDouble() {
    this->theValue = 0;
    this->precision = 0.1;
  }
  ImpreciseDouble(double other) {
    this->operator=(other);
  }
  void operator+=(const ImpreciseDouble& other) {
    if (!other.IsEqualToZero()) {
      this->theValue += other.theValue;
    }
  }
  void operator-=(const ImpreciseDouble& other) {
    if (!other.IsEqualToZero()) {
      this->theValue -= other.theValue;
    }
  }
  void operator=(const Rational& other) {
    this->theValue = other.GetDoubleValue();
  }
  bool IsEqualToZero() const {
    if (this->theValue < 0) {
      return (- theValue) < this->precision;
    }
    return this->theValue < this->precision;
  }
  bool operator<=(const ImpreciseDouble& other) {
    return ! (other<*this);
  }
  bool IsPositive() const {
    return this->theValue > this->precision;
  }
  bool IsNegative() const {
    return *this < this->GetZero();
  }
  bool operator<(const ImpreciseDouble& other) const {
    ImpreciseDouble temp = other;
    temp -= *this;
    return temp.IsPositive();
  }
  void AssignFloor() {
    this->theValue = FloatingPoint::Floor(this->theValue);
  }
  void operator/=(const ImpreciseDouble& other) {
    ImpreciseDouble copyMe;
    copyMe = *this;
    *this = copyMe / other;
  }
  ImpreciseDouble operator/(const ImpreciseDouble& other) const {
    ImpreciseDouble result;
    result = *this;
    if (other.IsEqualToZero()) {
      //the below is written like this to avoid this->theValue/0;
      //If the user attempts to divide by zero, I want a regular division by zero exception to be generated
      result.theValue = this->theValue / (other.theValue - other.theValue);
      return result;
    }
    result.theValue /= other.theValue;
    return result;
  }
  void operator*=(const ImpreciseDouble& other) {
    if (!other.IsEqualToZero()) {
      this->theValue *= other.theValue;
    } else {
      this->theValue = 0;
    }
  }
  bool operator==(const ImpreciseDouble& other) const {
    double diff = this->theValue - other.theValue;
    if (diff < 0) {
      diff = - diff;
    }
    return diff < this->precision;
  }
  static ImpreciseDouble GetMinusOne() {
    return - 1;
  }
  static ImpreciseDouble GetOne() {
    return 1;
  }
  static ImpreciseDouble GetZero() {
    return 0;
  }
};

void DrawOperations::projectionMultiplicityMergeOnBasisChange(DrawOperations& theOps) {
  Matrix<ImpreciseDouble> theMat;
  int theDim = theOps.theBilinearForm.NumRows;
  theMat.init(theDim, 2);
//we assume that the ComputeProjectionsEiVectors has been called
//  theOps.ComputeProjectionsEiVectors();
  for (int i = 0; i < theOps.ProjectionsEiVectors.size; i ++) {
    for (int j = 0; j < 2; j ++) {
      theMat.elements[i][j] = theOps.ProjectionsEiVectors[i][j];
    }
  }
  ProgressReport theReport;
  std::stringstream out;
  out << "before elimination:\n" << theMat.ToString();
  theMat.GaussianEliminationEuclideanDomain(nullptr, ImpreciseDouble::GetMinusOne(), ImpreciseDouble::GetOne());
  out << "after elimination:\n" << theMat.ToString();
  theReport.Report(out.str());
}

void DrawOperations::operator+=(const DrawOperations& other) {
  if (this->theBilinearForm.NumRows != other.theBilinearForm.NumRows) {
    return;
  }
  this->theOperations.AddListOnTop(other.theOperations);
  //this->BasisProjectionPlane.AddListOnTop(other.BasisProjectionPlane);
  //this->centerX.AddListOnTop(other.centerX);
  //this->centerY.AddListOnTop(other.centerY);
  //this->GraphicsUnit.AddListOnTop(other.GraphicsUnit);
}

void RationalFunctionOld::operator/=(const Polynomial<Rational>& other) {
  RationalFunctionOld tempRF;
  tempRF = other;
  tempRF.Invert();
  *this *= tempRF;
  if (!this->checkConsistency()) {
    global.fatal << "Bad rational function." << global.fatal;
  }
}

void RationalFunctionOld::ReduceRFToPoly() {
  if (this->expressionType != this->typeRationalFunction) {
    return;
  }
  if (this->Denominator.GetElement().IsConstant()) {
    this->Numerator.GetElement() /= this->Denominator.GetElement().coefficients[0];
    this->Denominator.FreeMemory();
    this->expressionType = this->typePoly;
  }
  if (this->Numerator.GetElement().IsEqualToZero()) {
    this->MakeZero();
  }
}

bool RationalFunctionOld::Substitution(const PolynomialSubstitution<Rational>& theSub) {
  MacroRegisterFunctionWithName("RationalFunctionOld::Substitution");
  if (theSub.size < 1) {
    return false;
  }
//  FormatExpressions tempFormat;
//  int commentMEWhenDone;
  switch(this->expressionType) {
    case RationalFunctionOld::typeRational:
//      if (!this->checkConsistency())global.fatal << global.fatal;
      return true;
    case RationalFunctionOld::typePoly:
//      global.Comments << "<hr>subbing in<br>" << this->ToString(tempFormat) << " using " << theSub.ToString()
//      << " to get ";
      if (!this->Numerator.GetElement().Substitution(theSub)) {
        return false;
      }
//      global.Comments << "<br>finally:<br>" << this->Numerator.GetElement().ToString();
      this->Simplify();
//      global.Comments << ", which, simplified, yields<br> " << this->ToString(tempFormat);
//      if (!this->checkConsistency())global.fatal << global.fatal;
      return true;
    case RationalFunctionOld::typeRationalFunction:
      if (!this->Numerator.GetElement().Substitution(theSub)) {
        return false;
      }
      if (!this->Denominator.GetElement().Substitution(theSub)) {
        return false;
      }
      if (this->Denominator.GetElement().IsEqualToZero()) {
        return false;
      }
      this->Simplify();
      return true;
    default:
      global.fatal << "Default case not allowed. " << global.fatal;
      break;
  }
  return false;
}

void Selection::operator=(const Vector<Rational>& other) {
  this->init(other.size);
  for (int i = 0; i <other.size; i ++) {
    if (!other[i].IsEqualToZero()) {
      this->selected[i] = true;
    }
  }
  this->ComputeIndicesFromSelection();
}

void ConeComplex::InitFromAffineDirectionsAndRefine(Vectors<Rational>& inputDirections, Vectors<Rational>& inputAffinePoints) {
  if (inputDirections.size != inputAffinePoints.size || inputDirections.size <= 0) {
    global.fatal << "Input directions size does not match affine point size. " << global.fatal;
  }
  Vectors<Rational> projectivizedDirections;
  projectivizedDirections.SetSize(inputDirections.size * 2);
  int theAffineDim = inputDirections[0].size;
  for (int i = 0; i < inputDirections.size; i ++) {
    projectivizedDirections[i] = inputDirections[i];
    projectivizedDirections[i].SetSize(theAffineDim + 1);
    *projectivizedDirections[i].LastObject()= 0;
  }
  for (int i = 0; i < inputAffinePoints.size; i ++) {
    projectivizedDirections[i + inputAffinePoints.size] = inputAffinePoints[i];
    projectivizedDirections[i + inputAffinePoints.size].SetSize(theAffineDim + 1);
    *projectivizedDirections[i + inputAffinePoints.size].LastObject() = 1;
  }
  this->InitFromDirectionsAndRefine(projectivizedDirections);
}

void ConeComplex::MakeAffineAndTransformToProjectiveDimPlusOne(Vector<Rational>& affinePoint, ConeComplex& output) {
  if (&output == this) {
    global.fatal << "Output coincides with input, not allowed. " << global.fatal;
  }
  output.init();
  Cone tempCone;
  Vectors<Rational> newNormals;
  int theAffineDim = affinePoint.size;
  for (int i = 0; i < this->size; i ++) {
    newNormals.SetSize(this->TheObjects[i].Normals.size + 1);
    for (int j = 0; j < this->TheObjects[i].Normals.size; j ++) {
      newNormals[j] = this->TheObjects[i].Normals[j].GetProjectivizedNormal(affinePoint);
    }
    newNormals.LastObject()->MakeEi(theAffineDim + 1, theAffineDim);
    tempCone.CreateFromNormals(newNormals);
    output.AddNonRefinedChamberOnTopNoRepetition(tempCone);
  }
}

template<class coefficient>
Vector<coefficient> Vector<coefficient>::GetProjectivizedNormal(Vector<coefficient>& affinePoint) {
  Vector<coefficient> result = *this;
  result.SetSize(this->size + 1);
  *result.LastObject()= - affinePoint.ScalarEuclidean(*this);
  return result;
}

void Lattice::GetRootOnLatticeSmallestPositiveProportionalTo(Vector<Rational>& input, Vector<Rational>& output) {
  if (&input == &output) {
    global.fatal << "Input not allowed to coincide with output. " << global.fatal;
  }
  Vectors<Rational> theBasis;
  Vector<Rational> tempRoot;
  theBasis.AssignMatrixRows(this->basisRationalForm);
  input.GetCoordsInBasiS(theBasis, tempRoot);
  tempRoot.ScaleByPositiveRationalToIntegralMinHeight();
  Matrix<Rational> tempMat;
  tempMat = this->basisRationalForm;
  tempMat.Transpose();
  tempMat.ActOnVectorColumn(tempRoot, output);
/*
  Vectors<Rational> tempRoots;
  tempRoots.AddOnTop(input);
  Lattice tempLattice =*this;
  tempLattice.IntersectWithLinearSubspaceSpannedBy(tempRoots);
  if (!tempLattice.basisRationalForm.NumRows ==1)global.fatal << global.fatal;
  tempLattice.basisRationalForm.RowToRoot(0, output);
  Rational tempRat;
  bool tempBool =  output.IsProportionalTo(input, tempRat);
  if (!tempBool)global.fatal << global.fatal;
  if (tempRat.IsNegative())
    output.Minus();*/
}

bool Cone::GetLatticePointsInCone(
  Lattice& theLattice,
  Vector<Rational>& theShift,
  int upperBoundPointsInEachDim,
  bool lastCoordinateIsOne,
  Vectors<Rational>& outputPoints,
  Vector<Rational>* shiftAllPointsBy
) const {
  if (upperBoundPointsInEachDim <= 0) {
    upperBoundPointsInEachDim = 5;
  }
  Vector<Rational> theActualShift = theShift;
  theLattice.ReduceVector(theActualShift);
  int theDimAffine = this->GetDim();
  if (lastCoordinateIsOne) {
    theDimAffine --;
  }
  SelectionWithMaxMultiplicity boundingBox;
  boundingBox.initMaxMultiplicity(theDimAffine, upperBoundPointsInEachDim*2);
  //format of the boundingBox:
  // if bounding box shows a vector (x_1, ...) then
  // it corresponds to a vector with coodinates (x_1-upperBoundPointsInEachDim, x_2-upperBoundPointsInEachDim, ...)
  int numCycles = boundingBox.NumSelectionsTotal();
  if (numCycles <= 0 || numCycles > 1000000) {
    //we test up to 1 million lattice points.
    //This is very restrictive: in 8 dimensions, selecting upperBoundPointsInEachDim=2,
    //we get a total of (2*2+ 1)^8=390625 points to test, which is a pretty darn small box
    return false;
  }
  outputPoints.Reserve(numCycles);
  outputPoints.size = 0;
  Vector<Rational> candidatePoint;
  Vectors<Rational> LatticeBasis;
  LatticeBasis.AssignMatrixRows(theLattice.basisRationalForm);
  for (int i = 0; i < numCycles; i ++, boundingBox.IncrementSubset()) {
    candidatePoint = theActualShift;
    if (shiftAllPointsBy != nullptr) {
      candidatePoint += *shiftAllPointsBy;
    }
    for (int j = 0; j < boundingBox.Multiplicities.size; j ++) {
      candidatePoint += LatticeBasis[j] * (boundingBox.Multiplicities[j] - upperBoundPointsInEachDim);
    }
    if (lastCoordinateIsOne) {
      candidatePoint.SetSize(candidatePoint.size + 1);
      *candidatePoint.LastObject() = 1;
    }
    if (this->IsInCone(candidatePoint)) {
      outputPoints.AddOnTop(candidatePoint);
    }
  }
  return true;
}

void PiecewiseQuasipolynomial::operator*=(const Rational& other) {
  if (other.IsEqualToZero()) {
    this->MakeZero(this->NumVariables);
    return;
  }
  for (int i = 0; i < this->theQPs.size; i ++) {
    this->theQPs[i] *= other;
  }
}

void PiecewiseQuasipolynomial::operator+=(const PiecewiseQuasipolynomial& other) {
  this->MakeCommonRefinement(other);
  for (int i = 0; i < this->theProjectivizedComplex.size; i ++) {
    int theIndex = other.theProjectivizedComplex.GetLowestIndexchamberContaining(
      this->theProjectivizedComplex[i].GetInternalPoint()
    );
    if (theIndex != - 1) {
      this->theQPs[i] += other.theQPs[theIndex];
    }
  }
}

bool PiecewiseQuasipolynomial::MakeVPF(Vectors<Rational>& theRoots, std::string& outputstring) {
  if (theRoots.size <= 0) {
    outputstring = "Error.";
    return false;
  }
  this->NumVariables = theRoots.GetDim();
  PartFractions theFracs;
  FormatExpressions theFormat;
  std::stringstream out;
  std::string whatWentWrong;

  theFracs.initFromRoots(theRoots);
  out << HtmlRoutines::GetMathMouseHover(theFracs.ToString(theFormat));
  theFracs.split(nullptr);
  out << HtmlRoutines::GetMathMouseHover(theFracs.ToString(theFormat));
  //theFracs.theChambers.InitFromDirectionsAndRefine(theRoots);
  global.fatal << "Not implemented. " << global.fatal ;
//  theFracs.theChambersOld.AmbientDimension = theRoots[0].size;
//  theFracs.theChambersOld.theDirections = theRoots;
//  theFracs.theChambersOld.SliceTheEuclideanSpace(false);
//  theFracs.theChambers.AssignCombinatorialChamberComplex(theFracs.theChambersOld);
  this->theQPs.SetSize(theFracs.theChambers.size);
  Vector<Rational> indicator;
  for (int i = 0; i < theFracs.theChambers.size; i ++) {
    indicator = theFracs.theChambers[i].GetInternalPoint();
    theFracs.GetVectorPartitionFunction(this->theQPs[i], indicator);
    //QuasiPolynomial& currentQP = this->theQPs[i];
  }
  Lattice baseLattice;
  baseLattice.MakeFromRoots(theRoots);
  Cone baseCone;
  baseCone.CreateFromVertices(theRoots);
  Vector<Rational> shiftRoot;
  baseLattice.GetInternalPointInConeForSomeFundamentalDomain(shiftRoot, baseCone);
  shiftRoot.Minus();
  theFracs.theChambers.MakeAffineAndTransformToProjectiveDimPlusOne(shiftRoot, this->theProjectivizedComplex);
  outputstring = out.str();
  return true;
}

bool Lattice::GetInternalPointInConeForSomeFundamentalDomain(Vector<Rational>& output, Cone& coneContainingOutputPoint) {
  Vector<Rational> coordsInBasis;
  coneContainingOutputPoint.GetInternalPoint(output);
  Vectors<Rational> basisRoots;
  basisRoots.AssignMatrixRows(this->basisRationalForm);
  if (!output.GetCoordsInBasiS(basisRoots, coordsInBasis)) {
    return false;
  }
  Rational maxCoord = coordsInBasis[0];
  if (maxCoord < 0) {
    maxCoord = - maxCoord;
  }
  for (int i = 0; i < coordsInBasis.size; i ++) {
    Rational tempRat = (coordsInBasis[i] < 0) ? - coordsInBasis[i] : coordsInBasis[i];
    if (tempRat > maxCoord) {
      maxCoord = tempRat;
    }
  }
  maxCoord += 1;
  output /= maxCoord;
  return true;
}

void Cone::TranslateMeMyLastCoordinateAffinization(Vector<Rational>& theTranslationVector) {
  if (theTranslationVector.size != this->GetDim() - 1) {
    global.fatal << "Translation vector size does not equal dimension minus one. " << global.fatal;
  }
  Vector<Rational> tempRoot;
  for (int i = 0; i < this->Normals.size; i ++) {
    tempRoot = this->Normals[i];
    tempRoot.size --;
    (*this->Normals[i].LastObject()) -= tempRoot.ScalarEuclidean(theTranslationVector);
  }
  tempRoot = theTranslationVector;
  tempRoot.SetSize(theTranslationVector.size + 1);
  *tempRoot.LastObject() = 0;
  for (int i = 0; i < this->Vertices.size; i ++) {
    if (!this->Vertices[i].LastObject()->IsEqualToZero()) {
      Rational tempRat = *this->Vertices[i].LastObject();
      this->Vertices[i] /= tempRat;
      this->Vertices[i] += tempRoot;
      this->Vertices[i].ScaleByPositiveRationalToIntegralMinHeight();
    }
  }
}

void ConeComplex::GetAllWallsConesNoOrientationNoRepetitionNoSplittingNormals(Vectors<Rational>& output) const {
  HashedList<Vector<Rational> > outputHashed;
  Vector<Rational> tempRoot;
  for (int i = 0; i < this->size; i ++) {
    for (int j = 0; j < this->TheObjects[i].Normals.size; j ++) {
      tempRoot = this->TheObjects[i].Normals[j];
      tempRoot.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
      outputHashed.AddOnTopNoRepetition(tempRoot);
    }
  }
  output = outputHashed;
}

void ConeComplex::RefineMakeCommonRefinement(const ConeComplex& other) {
  Vectors<Rational> newWalls;
  Cone tempCone = this->ConvexHull;
  if (tempCone.MakeConvexHullOfMeAnd(other.ConvexHull)) {
    this->GetAllWallsConesNoOrientationNoRepetitionNoSplittingNormals(newWalls);
    this->init();
    this->ConvexHull = tempCone;
    this->AddNonRefinedChamberOnTopNoRepetition(tempCone);
    this->splittingNormals.AddOnTopNoRepetition(newWalls);
  }
  other.GetAllWallsConesNoOrientationNoRepetitionNoSplittingNormals(newWalls);
  this->splittingNormals.AddOnTopNoRepetition(newWalls);
  this->indexLowestNonRefinedChamber = 0;
  this->Refine();
}

void ConeComplex::TranslateMeMyLastCoordinateAffinization(Vector<Rational>& theTranslationVector) {
  ConeComplex myCopy;
  myCopy = *this;
  this->init();
  Cone tempCone;
  for (int i = 0; i < myCopy.size; i ++) {
    tempCone = myCopy[i];
    tempCone.TranslateMeMyLastCoordinateAffinization(theTranslationVector);
    this->AddNonRefinedChamberOnTopNoRepetition(tempCone);
  }
  Vector<Rational> normalNoAffinePart, newNormal;
  for (int j = 0; j < myCopy.splittingNormals.size; j ++) {
    normalNoAffinePart = myCopy.splittingNormals[j];
    newNormal = normalNoAffinePart;
    normalNoAffinePart.size --;
    (*newNormal.LastObject()) -= normalNoAffinePart.ScalarEuclidean(theTranslationVector);
    this->splittingNormals.AddOnTop(newNormal);
  }
}

void PiecewiseQuasipolynomial::TranslateArgument(Vector<Rational>& translateToBeAddedToArgument) {
  Vector<Rational> chamberShift = - translateToBeAddedToArgument;
  this->theProjectivizedComplex.TranslateMeMyLastCoordinateAffinization(chamberShift);
  QuasiPolynomial tempQP;
  for (int i = 0; i < this->theQPs.size; i ++) {
    this->theQPs[i].Substitution(- translateToBeAddedToArgument, tempQP);
    this->theQPs[i] = tempQP;
  }
}

std::string PiecewiseQuasipolynomial::ToString(bool useLatex, bool useHtml) {
  std::stringstream out;
  FormatExpressions theFormat;
  for (int i = 0; i < this->theProjectivizedComplex.size; i ++) {
    const Cone& currentCone = this->theProjectivizedComplex[i];
    QuasiPolynomial& currentQP = this->theQPs[i];
    out << "Chamber number " << i + 1;
    if (useHtml) {
      out << "<br>";
    }
    out << currentCone.ToString(&theFormat);
    if (useHtml) {
      out << "<br>";
    }
    out << "quasipolynomial: ";
    if (useLatex& useHtml) {
      out << HtmlRoutines::GetMathSpanBeginArrayL(currentQP.ToString(useHtml, useLatex));
    } else {
      out << currentQP.ToString(useHtml, useLatex);
    }
    if (useHtml) {
      out << "<hr>";
    }
  }
  return out.str();
}

void PiecewiseQuasipolynomial::DrawMe(
  DrawingVariables& theDrawingVars,
  int numLatticePointsPerDim,
  Cone* RestrictingChamber,
  Vector<Rational>* distinguishedPoint
) {
  FormatExpressions theFormat;
  Vectors<Rational> latticePoints;
  HashedList<Vector<Rational> > theLatticePointsFinal;
  List<std::string> theLatticePointColors;
  List<std::string> tempList;
  if (numLatticePointsPerDim < 0) {
    numLatticePointsPerDim = 0;
  }
  const std::string ZeroColor = "gray";
  for (int i = 0; i < this->theProjectivizedComplex.size; i ++) {
    std::string chamberWallColor = "black";
    bool isZeroChamber = this->theQPs[i].IsEqualToZero();
    if (isZeroChamber) {
      chamberWallColor = ZeroColor;
    }
    this->theProjectivizedComplex[i].DrawMeLastCoordAffine(false, theDrawingVars, theFormat, chamberWallColor);
    std::stringstream tempStream;
    tempStream << i + 1;
    Vector<Rational> tempRoot = this->theProjectivizedComplex[i].GetInternalPoint();
    tempRoot.MakeAffineUsingLastCoordinate();
    for (int j = 0; j < this->theQPs[i].LatticeShifts.size; j ++) {
      this->theProjectivizedComplex[i].GetLatticePointsInCone(
        this->theQPs[i].AmbientLatticeReduced,
        this->theQPs[i].LatticeShifts[j],
        numLatticePointsPerDim,
        true,
        latticePoints,
        distinguishedPoint
      );
      tempList.initializeFillInObject(latticePoints.size, chamberWallColor);
      if (RestrictingChamber != nullptr) {
        for (int k = 0; k < latticePoints.size; k ++) {
          tempRoot = latticePoints[k];
          tempRoot.MakeAffineUsingLastCoordinate();
          if (!RestrictingChamber->IsInCone(tempRoot)) {
            tempList[k] = ZeroColor;
          }
        }
      }
      theLatticePointsFinal.AddOnTop(latticePoints);
      theLatticePointColors.AddListOnTop(tempList);
    }
  }
  for (int i = 0; i < theLatticePointsFinal.size; i ++) {
    theDrawingVars.drawCircleAtVectorBufferRational(theLatticePointsFinal[i], theLatticePointColors[i], 2);
    theDrawingVars.drawTextAtVectorBufferRational(
      theLatticePointsFinal[i],
      this->EvaluateInputProjectivized(theLatticePointsFinal[i]).ToString(),
      theLatticePointColors[i]
    );
  }
}

Rational QuasiPolynomial::Evaluate(const Vector<Rational>& input) {
  Vector<Rational> testLatticeBelonging;
  for (int i = 0; i < this->LatticeShifts.size; i ++) {
    testLatticeBelonging = this->LatticeShifts[i] - input;
    if (this->AmbientLatticeReduced.IsInLattice(testLatticeBelonging)) {
      return this->valueOnEachLatticeShift[i].Evaluate(input);
    }
  }
  return 0;
}

Rational PiecewiseQuasipolynomial::Evaluate(const Vector<Rational>& input) {
  if (input.size != this->theProjectivizedComplex.GetDim() - 1) {
    global.fatal << "Input size does not equal the projectivized complex dimension minus one. " << global.fatal;
  }
  Vector<Rational> ProjectivizedInput = input;
  ProjectivizedInput.SetSize(input.size + 1);
  *ProjectivizedInput.LastObject() = 1;
  return this->EvaluateInputProjectivized(ProjectivizedInput);
}

Rational PiecewiseQuasipolynomial::EvaluateInputProjectivized(const Vector<Rational>& input) {
  Rational result;
  if (input.size != this->theProjectivizedComplex.GetDim()) {
    global.fatal << "Input size not equal to projectivized complex dimension. " << global.fatal;
  }
  Vector<Rational> AffineInput = input;
  AffineInput.SetSize(input.size - 1);
  int theIndex = this->theProjectivizedComplex.GetLowestIndexchamberContaining(input);
  if (theIndex == - 1) {
    return 0;
  }
  result = this->theQPs[theIndex].Evaluate(AffineInput);
  //the following for cycle is for self-check purposes only. Comment it out as soon as
  // the code has been tested sufficiently
  bool firstFail = true;
  for (int i = 0; i < this->theProjectivizedComplex.size; i ++) {
    if (this->theProjectivizedComplex[i].IsInCone(input)) {
      Rational altResult = this->theQPs[i].Evaluate(AffineInput);
      if (result != altResult) {
        if ((false)) {
          if (!firstFail) {
            break;
          }
          FormatExpressions tempFormat;
          global.Comments << "<hr>Error!!! Failed on chamber " << theIndex + 1 << " and " << i + 1;
          global.Comments << "<br>Evaluating at point " << AffineInput.ToString() << "<br>";
          global.Comments << "<br>Chamber " << theIndex + 1 << ": " << this->theProjectivizedComplex[theIndex].ToString(&tempFormat);
          global.Comments << "<br>QP: " << this->theQPs[theIndex].ToString(true, false);
          global.Comments << "<br>value: " << result.ToString();
          global.Comments << "<br><br>Chamber " << i + 1 << ": " << this->theProjectivizedComplex[i].ToString(&tempFormat);
          global.Comments << "<br>QP: " << this->theQPs[i].ToString(true, false);
          global.Comments << "<br>value: " << altResult.ToString();
          if (firstFail) {
            DrawingVariables tempDV;
            global.Comments << "<br><b>Point of failure: " << AffineInput.ToString() << "</b>";
            //this->DrawMe(tempDV);
            this->theProjectivizedComplex.DrawMeLastCoordAffine(true, tempDV, tempFormat);
            tempDV.theBuffer.drawCircleAtVectorBufferRational(AffineInput, "black", 5);
            tempDV.theBuffer.drawCircleAtVectorBufferRational(AffineInput, "black", 10);
            tempDV.theBuffer.drawCircleAtVectorBufferRational(AffineInput, "red", 4);
            global.Comments << tempDV.GetHtmlDiv(this->theProjectivizedComplex.GetDim() - 1);
          }
          firstFail = false;
        }
      }
    }
  }
  return result;
}

void PiecewiseQuasipolynomial::MakeCommonRefinement(const ConeComplex& other) {
  List<QuasiPolynomial> oldQPList = this->theQPs;
  ConeComplex oldComplex = this->theProjectivizedComplex;
  this->theProjectivizedComplex.RefineMakeCommonRefinement(other);
  this->theQPs.SetSize(this->theProjectivizedComplex.size);
  for (int i = 0; i < this->theProjectivizedComplex.size; i ++) {
    int theOldIndex = oldComplex.GetLowestIndexchamberContaining(this->theProjectivizedComplex[i].GetInternalPoint());
    if (theOldIndex != - 1) {
      this->theQPs[i] = oldQPList[theOldIndex];
    } else {
      //the below needs to be fixed!!!!!
      this->theQPs[i].MakeZeroLatTiceZn(this->GetNumVars());
    }
  }
}

void Lattice::GetDefaultFundamentalDomainInternalPoint(Vector<Rational>& output) {
  output.MakeZero(this->GetDim());
  Vector<Rational> tempRoot;
  for (int i = 0; i < this->basisRationalForm.NumRows; i ++) {
    this->basisRationalForm.GetVectorFromRow(i, tempRoot);
    output += tempRoot;
  }
  output /= 2;
}

bool PartFractions::split(Vector<Rational>* Indicator) {
  //PartFraction::flagAnErrorHasOccurredTimeToPanic = true;
  //this->flagAnErrorHasOccurredTimeToPanic = true;
  if (!this->flagInitialized) {
    this->IndexLowestNonProcessed = 0;
    this->PrepareIndicatorVariables();
    this->PrepareCheckSums();
    this->flagInitialized = true;
  }
  if (this->splitPartial()) {
    this->RemoveRedundantShortRoots(Indicator);
    this->CompareCheckSums();
    this->IndexLowestNonProcessed = this->size();
    this->MakeProgressReportSplittingMainPart();
  }
  return false;
}

void Cone::ChangeBasis(Matrix<Rational>& theLinearMap) {
  theLinearMap.ActOnVectorsColumn(this->Normals);
  this->CreateFromNormals(this->Normals);
}

void Cone::TransformToWeylProjective(ConeComplex& owner) {
  (void) owner;
  global.fatal << "Not implemented yet. " << global.fatal;
/*
  for (int i = 0; i < this->Externalwalls.size; i ++)
    this->Externalwalls[i].TransformToWeylProjective();
  WallData newWall;
  this->Externalwalls.Reserve(owner.WeylChamber.size +this->Externalwalls.size);
  Vectors<Rational> newExternalWalls;
  owner.GetWeylChamberWallsForCharacterComputation(newExternalWalls);
  for (int i = 0; i < newExternalWalls.size; i ++) {
    newWall.normal = newExternalWalls[i];
    this->Externalwalls.AddOnTop(newWall);
  }
  this->AllVertices.size = 0;
  this->ComputeVerticesFromNormals(owner);*/
}

void ConeComplex::TransformToWeylProjective() {
 /* this->AmbientWeyl.GetElement().ComputeAllElements();
  this->log << this->AmbientWeyl.GetElement().ToString();
  std::string tempS;
  this->ToString(tempS);
  this->log << "\nWeyl chamber: " << this->WeylChamber.ToString() << "\n";
  this->log << tempS;
  this->NewHyperplanesToSliceWith.size = 0;
  this->theHyperplanes.size = 0;
  Vector<Rational> wallToSliceWith;
//  Vectors<Rational> oldDirections;
  for (int k = 0; k< this->AmbientWeyl.GetElement().size; k++)
    for (int i = 0; i < this->size; i ++)
      if (this->TheObjects[i] != 0)
        for (int j = 0; j < this->TheObjects[i]->Externalwalls.size; j ++) {
          this->GetAffineWallImage(k, this->TheObjects[i]->Externalwalls[j], wallToSliceWith);
//          if (k == 0)
//            oldDirections.AddOnBottomNoRepetition(wallToSliceWith);
          wallToSliceWith.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
          if (k > 0)
            this->NewHyperplanesToSliceWith.AddOnTopNoRepetition(wallToSliceWith);
          this->theHyperplanes.AddOnTopNoRepetition(wallToSliceWith);
        }
  this->log << "\n Affine walls to slice with:";
  for (int i = 0; i < this->NewHyperplanesToSliceWith.size; i ++)
    this->log << "\n" << this->NewHyperplanesToSliceWith[i].ToString();
  this->log << "\n";
  this->AmbientDimension = this->AmbientDimension*2+ 1;
  for (int i = 0; i < this->size; i ++)
    if (this->TheObjects[i] != 0) {
      this->TheObjects[i]->TransformToWeylProjective(*this);
//      theVertices.AddRootSnoRepetition(this->TheObjects[i]->AllVertices);
    }
  //this->startingCones.initFromDirections()
  Vector<Rational> tempRoot;
  for (int i = 0; i < this->TheGlobalConeNormals.size; i ++) {
    tempRoot.MakeZero(this->AmbientDimension);
    int startingDim= this->TheGlobalConeNormals[i].size;
    for (int j = 0; j<startingDim; j ++) {
      tempRoot[j] = this->TheGlobalConeNormals[i][j];
      tempRoot[j+startingDim] = -tempRoot[j];
    }
    this->TheGlobalConeNormals[i] = tempRoot;
  }
  Vectors<Rational> tempRoots;
  this->GetWeylChamberWallsForCharacterComputation(tempRoots);
  this->TheGlobalConeNormals.AddListOnTop(tempRoots);
  this->log << "the global cone normals: " << this->TheGlobalConeNormals.ToString();
  this->ToString(tempS);
  this->log << tempS;
  global.theIndicatorVariables.StatusString1NeedsRefresh = true;
  global.theIndicatorVariables.StatusString1= this->log.str();
  global.MakeReport();*/
}

void ConeLatticeAndShiftMaxComputation::init(
  Vector<Rational>& theNEq, Cone& startingCone, Lattice& startingLattice, Vector<Rational>& startingShift
) {
  ConeLatticeAndShift theCLS;
  theCLS.theProjectivizedCone = startingCone;
  theCLS.theLattice = startingLattice;
  theCLS.theShift = startingShift;
  this->numNonParaM = 0;
  this->numProcessedNonParam = 0;
  this->LPtoMaximizeLargerDim.size = 0;
  this->LPtoMaximizeSmallerDim.size = 0;
  this->theStartingRepresentative = startingShift;
  this->theFinalRepresentatives.size = 0;
  this->complexStartingPerRepresentative.size = 0;
  this->complexRefinedPerRepresentative.size = 0;
  this->theConesLargerDim.AddOnTop(theCLS);
  this->LPtoMaximizeLargerDim.AddOnTop(theNEq);
  this->IsInfinity.initializeFillInObject(1, false);
}

void Cone::SliceInDirection(Vector<Rational>& theDirection, ConeComplex& output) {
  output.init();
  output.AddNonRefinedChamberOnTopNoRepetition(*this);
  output.slicingDirections.AddOnTop(theDirection);
  output.Refine();
}

void Lattice::ApplyLinearMap(Matrix<Rational> & theMap, Lattice& output) {
  Vectors<Rational> tempRoots;
  tempRoots.AssignMatrixRows(this->basisRationalForm);
  theMap.ActOnVectorsColumn(tempRoots);
  output.MakeFromRoots(tempRoots);
}

std::string ConeLatticeAndShiftMaxComputation::ToString(FormatExpressions* theFormat) {
  std::stringstream out;
  out << "<hr>Resulting lattice: " << this->theFinalRougherLattice.ToString() << "<hr><hr>";
/*  if (this->complexStartingPerRepresentative.size >0) {
    out << "<hr> Non-refined complex per representative:<br>\n ";
    for (int i = 0; i < this->complexStartingPerRepresentative.size; i ++) {
      out << "Lattice +shift ="  << this->theFinalRepresentatives[i].ToString() << " + " << this->theFinalRougherLattice.ToString();
      out << "<br>\n" << this->complexStartingPerRepresentative[i].ToString(false, true);
      out << "the function we need to max: " << this->LPtoMaximizeSmallerDim[i].ToString();
    }
  }*/
  out << "<hr><hr>Cones not processed(number of cones " << this->theConesLargerDim.size << "):\n<hr>\n";
  DrawingVariables theDrawingVariables;
  Polynomial<Rational>  tempP;
  for (int i = 0; i < this->theConesLargerDim.size; i ++) {
    out << "";// << this->theConesLargerDim[i].ToString(theFormat);
    //out << "<br>" << this->LPtoMaximizeLargerDim[i].ToString();
    theDrawingVariables.theBuffer.init();
    out << "<br>" << this->theConesLargerDim[i].theProjectivizedCone.DrawMeToHtmlLastCoordAffine(theDrawingVariables, *theFormat);
    out << "<br>over " << this->theConesLargerDim[i].theShift.ToString() << " + " << this->theConesLargerDim[i].theLattice.ToString();
    tempP.MakeLinPolyFromRootLastCoordConst(this->LPtoMaximizeLargerDim[i]);
    out << "<br>the function we have maxed, as a function of the remaining variables, is: " << tempP.ToString(theFormat) << "<hr><hr>";
  }
  if (this->theConesSmallerDim.size > 0) {
    out << "<br>Cones processed: <br>";
    for (int i = 0; i < this->theConesSmallerDim.size; i ++) {
      out << this->theConesSmallerDim[i].ToString(*theFormat);
      //out << "<br>" << this->LPtoMaximizeSmallerDim[i].ToString();
      theDrawingVariables.theBuffer.init();
      out << this->theConesSmallerDim[i].theProjectivizedCone.DrawMeToHtmlLastCoordAffine(theDrawingVariables, *theFormat);
    }
  }
  return out.str();
}

void ConeLatticeAndShiftMaxComputation::FindExtremaParametricStep3() {
  this->theFinalRougherLattice = this->theConesLargerDim[0].theLattice;
  ProgressReport theReport;
  ProgressReport theReport2;
  for (int i = 1; i < this->theConesLargerDim.size; i ++) {
    this->theFinalRougherLattice.IntersectWith(this->theConesLargerDim[i].theLattice);
    std::stringstream tempStream;
    tempStream << "intersecing lattice " << i + 1 << " out of " << this->theConesLargerDim.size;
    theReport.Report(tempStream.str());
  }
  this->theFinalRepresentatives.size = 0;
  Vectors<Rational> tempRoots, tempRoots2;
  tempRoots2.SetSize(1);
  for (int i = 0; i < this->theConesLargerDim.size; i ++) {
    tempRoots2[0] = this->theConesLargerDim[i].theShift;
    this->theConesLargerDim[i].theLattice.GetAllRepresentativesProjectingDownTo(
      this->theFinalRougherLattice, tempRoots2, tempRoots
    );
    this->theFinalRepresentatives.AddOnTopNoRepetition(tempRoots);
    std::stringstream tempStream;
    tempStream << "Computing representative " << i + 1 << " out of " << this->theConesLargerDim.size;
    tempStream << "\nSo far " << this->theFinalRepresentatives.size << " found.";
    theReport2.Report(tempStream.str());
  }
  this->complexStartingPerRepresentative.SetSize(this->theFinalRepresentatives.size);
  this->startingLPtoMaximize.SetSize(this->theFinalRepresentatives.size);
  this->finalMaxima.SetSize(this->theFinalRepresentatives.size);
  Vector<Rational> tempRoot;
  for (int i = 0; i < this->theFinalRepresentatives.size; i ++) {
    for (int j = 0; j < this->theConesLargerDim.size; j ++) {
      tempRoot = this->theFinalRepresentatives[i];
      this->theConesLargerDim[j].theLattice.ReduceVector(tempRoot);
      if (tempRoot == this->theConesLargerDim[j].theShift) {
        this->complexStartingPerRepresentative[i].AddOnTop(this->theConesLargerDim[j].theProjectivizedCone);
        this->startingLPtoMaximize[i].AddOnTop(this->LPtoMaximizeLargerDim[j]);
      }
    }
  }
}

/*void ConeLatticeAndShiftMaxComputation::FindExtremaParametricStep2TrimChamberForMultOne
    (Controller& thePauseController) {
  Cone trimmedCone;
  Vectors<Rational> tempRoots;
  Vector<Rational> multFreeWall;
  int startingNumCones = this->theConesLargerDim.size;
  int numKilledCones = 0;
  for (int i = 0; i < this->theConesLargerDim.size; i ++) {
    trimmedCone.Normals = this->theConesLargerDim[i].theProjectivizedCone.Normals;
    multFreeWall = this->LPtoMaximizeLargerDim[i];
    multFreeWall.Minus();
    *multFreeWall.LastObject() +=1;
    trimmedCone.Normals.AddOnTop(multFreeWall);
    trimmedCone.CreateFromNormals(trimmedCone.Normals);
    if (!trimmedCone.flagIsTheZeroCone)
      this->theConesLargerDim[i].theProjectivizedCone = trimmedCone;
    else {
      this->theConesLargerDim.RemoveIndexSwapWithLast(i);
      this->LPtoMaximizeLargerDim.RemoveIndexSwapWithLast(i);
      i --;
      numKilledCones++;
    }
    std::stringstream tempStream;
    tempStream << "Processed " << i +numKilledCones << " out of " << startingNumCones;
    tempStream << "\nKilled " << numKilledCones << " cones so far";
    global.theIndicatorVariables.ProgressReportStrings[2] = tempStream.str();
    global.MakeReport();
  }
}
*/

void ConeLatticeAndShiftMaxComputation::FindExtremaParametricStep4() {
  this->complexRefinedPerRepresentative.SetSize(this->theFinalRepresentatives.size);
  this->theMaximaCandidates.SetSize(this->theFinalRepresentatives.size);
  ProgressReport theReport;
  for (int i = 0; i < this->theFinalRepresentatives.size; i ++) {
    ConeComplex& currentComplex = this->complexRefinedPerRepresentative[i];
    currentComplex.initFromCones(this->complexStartingPerRepresentative[i], true);
    std::stringstream tempStream;
    tempStream << "Processing representative " << i + 1 << " out of " << this->theFinalRepresentatives.size;
    theReport.Report(tempStream.str());
    currentComplex.Refine();
    this->theMaximaCandidates[i].SetSize(currentComplex.size);
    for (int j = 0; j < currentComplex.size; j ++) {
      for (int k = 0; k < this->complexStartingPerRepresentative[k].size; k ++) {
        if (this->complexStartingPerRepresentative[i][k].IsInCone(currentComplex[j].GetInternalPoint())) {
          this->theMaximaCandidates[i][j].AddOnTopNoRepetition(this->startingLPtoMaximize[i][k]);
        }
      }
    }
  }
}

void ConeLatticeAndShiftMaxComputation::FindExtremaParametricStep5() {
  this->finalMaximaChambers.SetSize(this->theFinalRepresentatives.size);
  this->finalMaximaChambersIndicesMaxFunctions.SetSize(this->theFinalRepresentatives.size);
  for (int i = 0; i < 1; i ++ ) {
    this->finalMaximaChambers[i].SetSize(this->complexRefinedPerRepresentative[i].size);
    this->theFinalRepresentatives[i].SetSize(this->complexRefinedPerRepresentative[i].size);
    for (int j = 0; j < 1; j ++) {
      const Cone& currentCone = this->complexRefinedPerRepresentative[i][j];
      this->finalMaximaChambers[i][j].init();
      this->finalMaximaChambers[i][j].findMaxLFOverConeProjective(
        currentCone,
        this->theMaximaCandidates[i][j],
        this->finalMaximaChambersIndicesMaxFunctions[i][j]
      );
    }
  }
}

void ConeLatticeAndShiftMaxComputation::FindExtremaParametricStep1(
  PauseThread& thePauseController
) {
  FormatExpressions tempFormat;
  ProgressReport theReport1;
  ProgressReport theReport2;
  ProgressReport theReport3;
  for (; this->numProcessedNonParam< this->numNonParaM; this->numProcessedNonParam ++) {
    while (this->theConesLargerDim.size > 0) {
      ConeLatticeAndShift& currentCLS = *this->theConesLargerDim.LastObject();
      if (this->LPtoMaximizeLargerDim.LastObject()->size != currentCLS.GetDimAffine() + 1) {
        global.fatal << "In ConeLatticeAndShiftMaxComputation::FindExtremaParametricStep1: "
        << "dimensions don't match. " << global.fatal;
      }
      if (!this->LPtoMaximizeLargerDim.LastObject()->IsEqualToZero()) {
        currentCLS.FindExtremaInDirectionOverLatticeOneNonParam(
          *this->LPtoMaximizeLargerDim.LastObject(), this->LPtoMaximizeSmallerDim, this->theConesSmallerDim
        );
      }
      this->theConesLargerDim.size --;
      this->LPtoMaximizeLargerDim.size --;
      thePauseController.SafePointDontCallMeFromDestructors();
      std::stringstream tempStream1, tempStream2, tempStream3;
      tempStream1 << "Processing " << this->numProcessedNonParam + 1 << " out of " << this->numNonParaM;
      tempStream2 << "Remaining cones: " << this->theConesLargerDim.size;
      tempStream3 << "Cones smaller dim total: " << this->theConesSmallerDim.size;
      theReport1.Report(tempStream1.str());
      theReport2.Report(tempStream2.str());
      theReport3.Report(tempStream3.str());
    }
    this->LPtoMaximizeLargerDim = this->LPtoMaximizeSmallerDim;
    this->theConesLargerDim = this->theConesSmallerDim;
    this->theConesSmallerDim.size = 0;
    this->LPtoMaximizeSmallerDim.size = 0;
  }
}

void ConeLatticeAndShift::FindExtremaInDirectionOverLatticeOneNonParamDegenerateCase(
  Vector<Rational>& theLPToMaximizeAffine,
  Vectors<Rational>& outputAppendLPToMaximizeAffine,
  List<ConeLatticeAndShift>& outputAppend,
  Matrix<Rational>& theProjectionLatticeLevel
) {
  Matrix<Rational> matVertices;
  matVertices.AssignVectorsToRows(this->theProjectivizedCone.Vertices);
  Vectors<Rational> theNormals;
  matVertices.GetZeroEigenSpaceModifyMe(theNormals);
  Vector<Rational> preferredNormal;
  for (int i = 0; i < theNormals.size; i ++) {
    if (!theNormals[i][0].IsEqualToZero()) {
      preferredNormal = theNormals[i];
      break;
    }
  }
  Rational firstCoord = preferredNormal[0];
  preferredNormal.ShiftToTheLeftOnePos();
  preferredNormal /= - firstCoord;
  ConeLatticeAndShift tempCLS;
  Vectors<Rational> newNormals = this->theProjectivizedCone.Normals;
  Rational firstCoordNewNormal;
  for (int i = 0; i < newNormals.size; i ++) {
    firstCoordNewNormal = newNormals[i][0];
    newNormals[i].ShiftToTheLeftOnePos();
    newNormals[i] += preferredNormal * firstCoordNewNormal;
  }
  //bool tempBool =
  tempCLS.theProjectivizedCone.CreateFromNormals(newNormals);
  tempCLS.theShift = this->theShift;
  tempCLS.theShift.ShiftToTheLeftOnePos();
  this->theLattice.ApplyLinearMap(theProjectionLatticeLevel, tempCLS.theLattice);
  Vector<Rational> tempRoot;
  tempRoot = theLPToMaximizeAffine.GetShiftToTheLeftOnePosition();
  //Vector<Rational> tempRoot2;
  //tempRoot2= preferredNormal;
  tempRoot += preferredNormal * theLPToMaximizeAffine[0];
  if (!tempCLS.theProjectivizedCone.flagIsTheZeroCone) {
    outputAppend.AddOnTop(tempCLS);
    outputAppendLPToMaximizeAffine.AddOnTop(tempRoot);
  }
}

void ConeLatticeAndShift::FindExtremaInDirectionOverLatticeOneNonParam(
  Vector<Rational>& theLPToMaximizeAffine,
  Vectors<Rational>& outputAppendLPToMaximizeAffine,
  List<ConeLatticeAndShift>& outputAppend
) {
  Vector<Rational> direction;
  FormatExpressions theFormat;
  int theDimProjectivized = this->GetDimProjectivized();
  Matrix<Rational> theProjectionLatticeLevel;
  theProjectionLatticeLevel.init(theDimProjectivized - 2, theDimProjectivized - 1);
  theProjectionLatticeLevel.MakeZero();
  for (int i = 0; i < theProjectionLatticeLevel.NumRows; i ++) {
    theProjectionLatticeLevel.elements[i][i + 1] = 1;
  }
  direction.MakeEi(theDimProjectivized, 0);
  if (!this->theProjectivizedCone.Vertices.LinSpanContainsVector(direction)) {
    this->FindExtremaInDirectionOverLatticeOneNonParamDegenerateCase(
      theLPToMaximizeAffine, outputAppendLPToMaximizeAffine, outputAppend, theProjectionLatticeLevel
    );
    return;
  }
  ProgressReport theReport;
  if (outputAppend.size >= 10) {
    std::stringstream tempStream;
    tempStream << "<hr><hr><hr><hr>The bad cone:" << this->theProjectivizedCone.ToString(&theFormat);
    theReport.Report(tempStream.str());
  }
  ConeComplex complexBeforeProjection;
  complexBeforeProjection.init();
  complexBeforeProjection.AddNonRefinedChamberOnTopNoRepetition(this->theProjectivizedCone);
  if (direction.ScalarEuclidean(theLPToMaximizeAffine).IsNegative()) {
    direction.Minus();
  }
  complexBeforeProjection.slicingDirections.AddOnTop(direction);
  complexBeforeProjection.slicingDirections.AddOnTop(-direction);
  complexBeforeProjection.Refine();
  Vector<Rational> tempRoot, extraEquation, exitNormalAffine, enteringNormalAffine;
  Vector<Rational> exitNormalLatticeLevel, exitNormalShiftedAffineProjected;
  Vector<Rational> directionSmallerDim, directionSmallerDimOnLattice;
  Vector<Rational> theShiftReduced = this->theShift;
  this->theLattice.ReduceVector(theShiftReduced);
  Vectors<Rational> exitRepresentatives, exitWallsShifted;
  Lattice exitRougherLattice;
  ConeLatticeAndShift tempCLS;
  directionSmallerDim.MakeEi(theDimProjectivized - 1, 0);
  Vectors<Rational> theNewNormals;
  for (int i = 0; i < complexBeforeProjection.size; i ++) {
    const Cone& currentCone = complexBeforeProjection[i];
    int numNonPerpWalls = 0;
    theNewNormals.size = 0;
    bool foundEnteringNormal = false;
    bool foundExitNormal = false;
    for (int j = 0; j < currentCone.Normals.size; j ++) {
      Vector<Rational>& currentNormal = currentCone.Normals[j];
      if (currentNormal[0].IsEqualToZero()) {
        tempRoot = currentNormal.GetShiftToTheLeftOnePosition();
        theNewNormals.AddOnTop(tempRoot);
      } else {
        numNonPerpWalls ++;
        if (numNonPerpWalls >= 3) {
          global.fatal << "Number of non-perpendicular walls is larger than 3. " << global.fatal;
        }
        if (!currentNormal.ScalarEuclidean(direction).IsPositive() && !foundExitNormal) {
          theLattice.GetRougherLatticeFromAffineHyperplaneDirectionAndLattice(
            directionSmallerDim,
            directionSmallerDimOnLattice,
            theShift,
            currentNormal,
            exitRepresentatives,
            exitWallsShifted,
            exitRougherLattice
          );
          exitNormalAffine = currentNormal;
          exitNormalLatticeLevel = exitNormalAffine;
          exitNormalLatticeLevel.SetSize(theDimProjectivized - 1);
          foundExitNormal = true;
        } else {
          enteringNormalAffine = currentNormal;
          foundEnteringNormal = true;
        }
      }
    }
    exitRougherLattice.ApplyLinearMap(theProjectionLatticeLevel, tempCLS.theLattice);
    for (int j = 0; j < exitRepresentatives.size; j ++) {
      exitRepresentatives[j] += theShiftReduced;
      Lattice::GetClosestPointInDirectionOfTheNormalToAffineWallMovingIntegralStepsInDirection(
        exitRepresentatives[j], exitNormalAffine, directionSmallerDimOnLattice, exitRepresentatives[j]
      );
    }
    global.Comments << "<hr><hr><hr>" << currentCone.ToString(&theFormat);
    global.Comments << "<br>Entering normal: " << ((foundEnteringNormal) ? enteringNormalAffine.ToString() : "not found");
    global.Comments << "<br>Exit normal: " << ((foundExitNormal) ? exitNormalAffine.ToString() : "not found");
    global.Comments << "<br>The shifted lattice representatives: " << exitRepresentatives.ToString()
    << "<br>exitNormalsShiftedAffineProjected";
    if (theNewNormals.size <= 0) {
      global.fatal << "New normals missing. " << global.fatal;
    }
    for (int j = 0; j < exitRepresentatives.size; j ++) {
      tempCLS.theProjectivizedCone.Normals = theNewNormals;
      exitNormalShiftedAffineProjected = exitNormalAffine.GetShiftToTheLeftOnePosition();
      *exitNormalShiftedAffineProjected.LastObject() = - exitNormalLatticeLevel.ScalarEuclidean(exitRepresentatives[j]);
      global.Comments << exitNormalShiftedAffineProjected.ToString() << ", ";
      if (foundEnteringNormal) {
        extraEquation = enteringNormalAffine.GetShiftToTheLeftOnePosition();
        extraEquation -= (exitNormalShiftedAffineProjected * enteringNormalAffine[0]) / exitNormalAffine[0];
        global.Comments << "extra equation: " << extraEquation.ToString() << ", ";
        tempCLS.theProjectivizedCone.Normals.AddOnTop(extraEquation);
      }
      tempRoot = theLPToMaximizeAffine.GetShiftToTheLeftOnePosition();
      tempRoot -= exitNormalShiftedAffineProjected * theLPToMaximizeAffine[0] / exitNormalAffine[0];
      outputAppendLPToMaximizeAffine.AddOnTop(tempRoot);
      if (tempCLS.theProjectivizedCone.Normals.size <= 0) {
        global.fatal << "Projectivized cone has no normals. " << global.fatal;
      }
      Vectors<Rational> tempTempRoots = tempCLS.theProjectivizedCone.Normals;
      tempCLS.theProjectivizedCone.CreateFromNormals(tempTempRoots);
      /*if (!tempBool) {
        std::stringstream tempStream;
        tempStream << "The bad starting cone (cone number " << i + 1 << "):" << this->ToString(theFormat) << "<hr><hr><hr><hr>The bad cone:" << tempCLS.ToString(theFormat);
        tempStream << "<br>\n\n" << this->theProjectivizedCone.Normals.ToString(false, false, false);
        tempStream << "\n\n<br>\n\n" << complexBeforeProjection.ToString(false, true);
        if (!foundEnteringNormal)
          tempStream << "<hr>not found entering normal!!!!!!<hr>";
        if (!foundExitNormal)
          tempStream << "<hr>not found exit normal!!!!!!<hr>";
        Cone tempCone;
        tempCone.CreateFromNormals(tempTempRoots);
        tempStream << "\n\n\n\n<br><br><hr>The bad normals: " << tempTempRoots.ToString();
        tempStream << "\n\n\n\n<br><br><hr>The bad normals after creation: " << tempCLS.theProjectivizedCone.Normals.ToString();
        global.theIndicatorVariables.StatusString1= tempStream.str();
        global.theIndicatorVariables.StatusString1NeedsRefresh = true;
        global.MakeReport();
        for (int i = 0; i <10000000; i ++)
          if (i%3== 0)
            i = i +2;
        while (true){}
      }*/
      //if (!tempBool)global.fatal << global.fatal;
      //global.Comments << tempCLS.theProjectivizedCone.ToString(false, true, true, true, theFormat);
      if (!tempCLS.theProjectivizedCone.flagIsTheZeroCone) {
        theProjectionLatticeLevel.ActOnVectorColumn(exitRepresentatives[j], tempCLS.theShift);
        outputAppend.AddOnTop(tempCLS);
        if (tempCLS.GetDimProjectivized() == 0) {
          theReport.Report(tempTempRoots.ToString());
          while (true) {
          }
        }
        if (tempCLS.GetDimProjectivized() != theDimProjectivized - 1) {
          global.fatal << "Projectivized dimension not correct. " << global.fatal;
        }
      }
    }
  }
}

void ConeComplex::GetNewVerticesAppend(
  Cone& myDyingCone, const Vector<Rational>& killerNormal, HashedList<Vector<Rational> >& outputVertices
) {
  int theDimMinusTwo = killerNormal.size - 2;
  int theDim = killerNormal.size;
  int numCycles = MathRoutines::NChooseK(myDyingCone.Normals.size, theDimMinusTwo);
  Selection theSel;
  Selection nonPivotPoints;
  theSel.init(myDyingCone.Normals.size);
  Matrix<Rational> theLinearAlgebra;
  theLinearAlgebra.init(theDimMinusTwo + 1, theDim);
  Vector<Rational> tempRoot;
  for (int i = 0; i < numCycles; i ++) {
    theSel.incrementSelectionFixedCardinality(theDimMinusTwo);//, IndexLastZeroWithOneBefore, NumOnesAfterLastZeroWithOneBefore);
    for (int j = 0; j < theDimMinusTwo; j ++) {
      Vector<Rational>& currentNormal = myDyingCone.Normals[theSel.elements[j]];
      for (int k = 0; k < theDim; k ++) {
        theLinearAlgebra.elements[j][k] = currentNormal[k];
      }
    }
    for (int k = 0; k < theDim; k ++) {
      theLinearAlgebra.elements[theDimMinusTwo][k] = killerNormal[k];
    }
    theLinearAlgebra.GaussianEliminationByRows(nullptr, &nonPivotPoints);
    if (nonPivotPoints.CardinalitySelection == 1) {
      theLinearAlgebra.NonPivotPointsToEigenVector(nonPivotPoints, tempRoot, Rational(1), Rational(0));
      tempRoot.ScaleByPositiveRationalToIntegralMinHeight();
      if (myDyingCone.IsInCone(tempRoot)) {
        outputVertices.AddOnTopNoRepetition(tempRoot);
      } else {
        tempRoot.Minus();
        if (myDyingCone.IsInCone(tempRoot)) {
          outputVertices.AddOnTopNoRepetition(tempRoot);
        }
      }
    }
  }
}

bool ConeComplex::SplitChamber(
  int indexChamberBeingRefined, bool weAreChopping, const Vector<Rational>& killerNormal
) {
  Cone& myDyingCone = this->TheObjects[indexChamberBeingRefined];
  Cone newPlusCone, newMinusCone;
  Matrix<Rational> bufferMat;
  Selection bufferSel;
  bool needToRecomputeVertices = (myDyingCone.Normals.GetRankOfSpanOfElements(&bufferMat, &bufferSel) < this->GetDim());
//  newPlusCone.flagHasSufficientlyManyVertices = true;
//  newMinusCone.flagHasSufficientlyManyVertices = true;
  newPlusCone.LowestIndexNotCheckedForSlicingInDirection = myDyingCone.LowestIndexNotCheckedForSlicingInDirection;
  newMinusCone.LowestIndexNotCheckedForSlicingInDirection = myDyingCone.LowestIndexNotCheckedForSlicingInDirection;
  newPlusCone.LowestIndexNotCheckedForChopping = myDyingCone.LowestIndexNotCheckedForChopping;
  newMinusCone.LowestIndexNotCheckedForChopping = myDyingCone.LowestIndexNotCheckedForChopping;
  newPlusCone.flagIsTheZeroCone = false;
  newMinusCone.flagIsTheZeroCone = false;
  if (weAreChopping) {
    newPlusCone.LowestIndexNotCheckedForChopping ++;
    newMinusCone.LowestIndexNotCheckedForChopping ++;
  }
  newPlusCone.Vertices.size = 0; newPlusCone.Normals.size = 0;
  newMinusCone.Vertices.size = 0; newMinusCone.Normals.size = 0;
  HashedList<Vector<Rational> > ZeroVertices;
  Rational tempRat;
  for (int i = 0; i < myDyingCone.Vertices.size; i ++) {
    killerNormal.ScalarEuclidean(myDyingCone.Vertices[i], tempRat);
    if (tempRat.IsPositive()) {
      newPlusCone.Vertices.AddOnTop(myDyingCone.Vertices[i]);
    }
    if (tempRat.IsEqualToZero()) {
      ZeroVertices.AddOnTopNoRepetition(myDyingCone.Vertices[i]);
    }
    if (tempRat.IsNegative()) {
      newMinusCone.Vertices.AddOnTop(myDyingCone.Vertices[i]);
    }
  }
  if (newPlusCone.Vertices.size == 0 || newMinusCone.Vertices.size == 0) {
    return false;
  }
  this->GetNewVerticesAppend(myDyingCone, killerNormal, ZeroVertices);
  for (int i = 0; i < myDyingCone.Normals.size; i ++) {
    if (newPlusCone.Vertices.HasAnElementPerpendicularTo(myDyingCone.Normals[i])) {
      newPlusCone.Normals.AddOnTop(myDyingCone.Normals[i]);
    }
    if (newMinusCone.Vertices.HasAnElementPerpendicularTo(myDyingCone.Normals[i])) {
      newMinusCone.Normals.AddOnTop(myDyingCone.Normals[i]);
    }
  }
  newPlusCone.Normals.AddOnTop(killerNormal);
  newMinusCone.Normals.AddOnTop(- killerNormal);
  newPlusCone.Vertices.AddListOnTop(ZeroVertices);
  newMinusCone.Vertices.AddListOnTop(ZeroVertices);
  this->PopChamberSwapWithLast(indexChamberBeingRefined);
  if (needToRecomputeVertices) {
    newPlusCone.CreateFromNormals(newPlusCone.Normals);
    newMinusCone.CreateFromNormals(newMinusCone.Normals);
  }
  this->AddNonRefinedChamberOnTopNoRepetition(newPlusCone);
  this->AddNonRefinedChamberOnTopNoRepetition(newMinusCone);
  return true;
}

void ConeComplex::PopChamberSwapWithLast(int index) {
  this->RemoveIndexSwapWithLast(index);
}

bool Cone::MakeConvexHullOfMeAnd(const Cone& other) {
  if (this->IsInCone(other.Vertices)) {
    return false;
  }
  if (other.flagIsTheZeroCone) {
    return false;
  }
  if (this->flagIsTheZeroCone) {
    this->operator=(other);
    return true;
  }
  Vectors<Rational> newVertices;
  newVertices.AddListOnTop(other.Vertices);
  newVertices.AddListOnTop(this->Vertices);
  this->CreateFromVertices(newVertices);
  return true;
}

bool ConeComplex::AddNonRefinedChamberOnTopNoRepetition(const Cone& newCone) {
  Cone theConeSorted;
  theConeSorted = newCone;
  theConeSorted.Normals.QuickSortAscending();
  this->ConvexHull.MakeConvexHullOfMeAnd(theConeSorted);
  return this->AddOnTopNoRepetition(theConeSorted);
}

void ConeComplex::RefineOneStep() {
  if (this->indexLowestNonRefinedChamber >= this->size) {
    return;
  }
  Cone& currentCone = this->TheObjects[this->indexLowestNonRefinedChamber];
  for (;
    currentCone.LowestIndexNotCheckedForChopping < this->splittingNormals.size;
    currentCone.LowestIndexNotCheckedForChopping ++
  ) {
    if (this->SplitChamber(
      this->indexLowestNonRefinedChamber, true, this->splittingNormals[currentCone.LowestIndexNotCheckedForChopping]
    )) {
      return;
    }
  }
  Vector<Rational> currentNewWall;
  for (;
    currentCone.LowestIndexNotCheckedForSlicingInDirection < this->slicingDirections.size;
    currentCone.LowestIndexNotCheckedForSlicingInDirection ++
  ) {
    for (int i = 0; i < currentCone.Normals.size; i ++) {
      if (this->slicingDirections[currentCone.LowestIndexNotCheckedForSlicingInDirection].ScalarEuclidean(
          currentCone.Normals[i]
        ).IsPositive()
      ) {
        for (int j = i + 1; j < currentCone.Normals.size; j ++) {
          if (this->slicingDirections[currentCone.LowestIndexNotCheckedForSlicingInDirection].ScalarEuclidean(
              currentCone.Normals[j]
            ).IsPositive()
          ) {
            if (currentCone.ProduceNormalFromTwoNormalsAndSlicingDirection(
              this->slicingDirections[currentCone.LowestIndexNotCheckedForSlicingInDirection],
              currentCone.Normals[i],
              currentCone.Normals[j],
              currentNewWall
            )) {
              if (this->SplitChamber(this->indexLowestNonRefinedChamber, false, currentNewWall)) {
                return;
              }
            }
          }
        }
      }
    }
  }
  this->indexLowestNonRefinedChamber ++;
}

void ConeComplex::InitFromDirectionsAndRefine(Vectors<Rational>& inputVectors) {
  this->init();
  Cone startingCone;
  startingCone.CreateFromVertices(inputVectors);
  this->AddNonRefinedChamberOnTopNoRepetition(startingCone);
  this->slicingDirections.AddListOnTop(inputVectors);
  this->Refine();
}

void ConeComplex::Sort() {
  List<Cone> tempList;
  tempList = *this;
  tempList.QuickSortAscending();
  this->Clear();
  for (int i = 0; i < tempList.size; i ++) {
    this->AddOnTop(tempList[i]);
  }
}

void ConeComplex::RefineAndSort() {
  this->Refine();
  this->Sort();
}

void ConeComplex::Refine() {
  ProgressReport theReport;
  while (this->indexLowestNonRefinedChamber < this->size) {
    this->RefineOneStep();
    std::stringstream out;
    out << "Refined " << this->indexLowestNonRefinedChamber << " out of " << this->size;
    theReport.Report(out.str());
  }
}

void Cone::ComputeVerticesFromNormalsNoFakeVertices() {
  this->Vertices.size = 0;
  Selection theSel, nonPivotPoints;
  for (int i = 0; i < this->Normals.size; i ++) {
    this->Normals[i].ScaleByPositiveRationalToIntegralMinHeight();
  }
  int theDim = this->Normals[0].size;
  theSel.init(this->Normals.size);
  int numCycles = theSel.GetNumCombinationsFixedCardinality(theDim - 1);
  if (theDim == 1) {
    numCycles = 0;
    bool foundNegative = false;
    bool foundPositive = false;
    for (int i = 0; i < this->Normals.size; i ++) {
      if (this->Normals[i].IsPositiveOrZero()) {
        foundPositive = true;
      }
      if (this->Normals[i].IsNegativeOrZero()) {
        foundNegative = true;
      }
    }
    if (foundNegative xor foundPositive) {
      this->Vertices.SetSizeMakeMatrix(1, 1);
      if (foundNegative) {
        this->Vertices[0][0] = - 1;
      } else {
        this->Vertices[0][0] = 1;
      }
    }
    return;
  }
  Matrix<Rational> theMat;
  Vector<Rational> tempRoot;
  theMat.init(theDim - 1, theDim);
  for (int i = 0; i < numCycles; i ++) {
    theSel.incrementSelectionFixedCardinality(theDim - 1);
    for (int j = 0; j < theSel.CardinalitySelection; j ++) {
      for (int k = 0; k < theDim; k ++) {
        theMat.elements[j][k] = this->Normals[theSel.elements[j]][k];
      }
    }
    theMat.GaussianEliminationByRows(nullptr, &nonPivotPoints);
    if (nonPivotPoints.CardinalitySelection == 1) {
      theMat.NonPivotPointsToEigenVector(nonPivotPoints, tempRoot);
      bool tempBool = this->IsInCone(tempRoot);
      if (!tempBool) {
        tempRoot.Minus();
        tempBool = this->IsInCone(tempRoot);
      }
      if (tempBool) {
        tempRoot.ScaleByPositiveRationalToIntegralMinHeight();
        this->Vertices.AddOnTopNoRepetition(tempRoot);
      }
    }
  }
}

bool Cone::EliminateFakeNormalsUsingVertices(int numAddedFakeWalls) {
  if (this->Vertices.size == 0) {
    this->flagIsTheZeroCone = true;
    this->Normals.SetSize(0);
    return false;
  }
  Vectors<Rational> verticesOnWall;
  if (numAddedFakeWalls != 0) {
    //we modify the normals so that they lie in the subspace spanned by the vertices
    Matrix<Rational> tempMat, matNormals, gramMatrixInverted;
    tempMat.AssignVectorsToRows(this->Vertices);
    Vectors<Rational> NormalsToSubspace;
    tempMat.GetZeroEigenSpaceModifyMe(NormalsToSubspace);
    if (NormalsToSubspace.size > 0) {
      matNormals.AssignVectorsToRows(NormalsToSubspace);
//      global.Comments << "<br>Normals to the subspace spanned by the vertices: " << NormalsToSubspace.ToString();
      gramMatrixInverted = matNormals;
      gramMatrixInverted.Transpose();
      gramMatrixInverted.MultiplyOnTheLeft(matNormals);
      gramMatrixInverted.Invert();
      Vector<Rational> tempRoot;
      for (int i = 0; i < this->Normals.size; i ++) {
        matNormals.ActOnVectorColumn(this->Normals[i], tempRoot);
        gramMatrixInverted.ActOnVectorColumn(tempRoot);
        for (int j = 0; j < tempRoot.size; j ++) {
          this->Normals[i] -= NormalsToSubspace[j] * tempRoot[j];
        }
        this->Normals[i].ScaleByPositiveRationalToIntegralMinHeight();
        if (this->Normals[i].IsEqualToZero()) {
          this->Normals.RemoveIndexSwapWithLast(i);
          i --;
        }
      }
      //all normals should now lie in the subspace spanned by the vertices
      //add the walls needed to go down to the subspace
      this->Normals.Reserve(this->Normals.size + 2 * NormalsToSubspace.size);
      for (int i = 0; i < NormalsToSubspace.size; i ++) {
        NormalsToSubspace[i].ScaleByPositiveRationalToIntegralMinHeight();
        this->Normals.AddOnTop(NormalsToSubspace[i]);
        this->Normals.AddOnTop(- NormalsToSubspace[i]);
      }
    }
  }
  Matrix<Rational> tempMatX;
  Selection tempSelX;
  int DesiredRank = this->Vertices.GetRankOfSpanOfElements(&tempMatX, &tempSelX);
  if (DesiredRank > 1) {
    for (int i = 0; i < this->Normals.size; i ++) {
      Vector<Rational>& currentNormal = this->Normals[i];
      verticesOnWall.size = 0;
      bool wallIsGood = false;
      for (int j = 0; j < this->Vertices.size; j ++) {
        if (currentNormal.ScalarEuclidean(this->Vertices[j]).IsEqualToZero()) {
          verticesOnWall.AddOnTop(this->Vertices[j]);
          int theRank = verticesOnWall.GetRankOfSpanOfElements(&tempMatX, &tempSelX);
          if (theRank < verticesOnWall.size) {
            verticesOnWall.RemoveLastObject();
          } else {
            if (theRank == DesiredRank - 1) {
              wallIsGood = true;
              break;
            }
          }
        }
      }
      if (!wallIsGood) {
        this->Normals.RemoveIndexSwapWithLast(i);
        i --;
      }
    }
  }
  //eliminate identical normals
  this->Normals.QuickSortAscending();
  int currentUniqueElementIndex = 0;
  for (int i = 1; i < this->Normals.size; i ++) {
    if (this->Normals[currentUniqueElementIndex] != this->Normals[i]) {
      currentUniqueElementIndex ++;
      this->Normals.SwapTwoIndices(currentUniqueElementIndex, i);
    }
  }
  if (this->Normals.size > 0) {
    this->Normals.SetSize(currentUniqueElementIndex + 1);
  }
  for (int i = 0; i < this->Vertices.size; i ++) {
    if (this->Normals.HasAnElementWithNegativeScalarProduct(this->Vertices[i])) {
      global.fatal << "Negative scalar products not allowed. " << global.fatal;
    }
  }
  for (int i = 0; i < this->Normals.size; i ++) {
    if (!this->Vertices.HasAnElementWithPositiveScalarProduct(this->Normals[i])) {
      return false;
    }
  }
  return numAddedFakeWalls == 0;
}

bool Cone::ProduceNormalFromTwoNormalsAndSlicingDirection(
  Vector<Rational>& SlicingDirection, Vector<Rational>& normal1, Vector<Rational>& normal2, Vector<Rational>& output
) {
  // we are looking for a normal n of the form n = t1*normal1+t2*normal2
  // such that  <t1*normal1+t2*normal2, slicingDirection>= 0
  Rational normal1ScalarDirection = normal1.ScalarEuclidean(SlicingDirection);
  if (normal1ScalarDirection.IsEqualToZero()) {
    output = normal1;
    return false;
  }
  //from now on we assume t2=1;
  Rational t1 = - normal2.ScalarEuclidean(SlicingDirection) / normal1ScalarDirection;
  output = normal2;
  output += normal1 * t1;
  output.ScaleByPositiveRationalToIntegralMinHeight();
  return true;
}

bool Cone::CreateFromVertices(const Vectors<Rational>& inputVertices) {
  this->LowestIndexNotCheckedForChopping = 0;
  this->LowestIndexNotCheckedForSlicingInDirection = 0;
  this->flagIsTheZeroCone = false;
  if (inputVertices.size <= 0) {
    this->Normals.size = 0;
    this->Vertices.size = 0;
    this->flagIsTheZeroCone = true;
    return false;
  }
  this->Normals.size = 0;
  Matrix<Rational> tempMat;
  Selection tempSel;
  int rankVerticesSpan = inputVertices.GetRankOfSpanOfElements(&tempMat, &tempSel);
  int theDim = inputVertices.GetDim();
  Vectors<Rational> extraVertices;
  extraVertices.SetSize(0);
  if (rankVerticesSpan < theDim) {
    Matrix<Rational> tempMat;
    tempMat.AssignVectorsToRows(inputVertices);
    tempMat.GetZeroEigenSpace(extraVertices);
    for (int i = 0; i < extraVertices.size; i ++) {
      this->Normals.AddOnTop(extraVertices[i]);
      this->Normals.AddOnTop(-extraVertices[i]);
    }
  }
  int NumCandidates = MathRoutines::NChooseK(inputVertices.size, rankVerticesSpan - 1);
  Selection theSelection;
  theSelection.init(inputVertices.size);
  Vector<Rational> normalCandidate;
  for (int i = 0; i < NumCandidates; i ++) {
    theSelection.incrementSelectionFixedCardinality(rankVerticesSpan - 1);
    for (int j = 0; j < theSelection.CardinalitySelection; j ++) {
      extraVertices.AddOnTop(inputVertices[theSelection.elements[j]]);
    }
    if (extraVertices.ComputeNormal(normalCandidate, theDim)) {
      bool hasPositive = false;
      bool hasNegative = false;
      for (int j = 0; j < inputVertices.size; j ++) {
        Rational tempRat = normalCandidate.ScalarEuclidean(inputVertices[j]);
        if (tempRat.IsNegative()) {
          hasNegative = true;
        }
        if (tempRat.IsPositive()) {
          hasPositive = true;
        }
        if (hasNegative && hasPositive) {
          break;
        }
      }
      normalCandidate.ScaleByPositiveRationalToIntegralMinHeight();
      if ((hasNegative && !hasPositive)) {
        normalCandidate.Minus();
      }
      if (!(hasNegative && hasPositive)) {
        this->Normals.AddOnTopNoRepetition(normalCandidate);
      }
    }
    extraVertices.size = theDim - rankVerticesSpan;
  }
  return this->CreateFromNormals(this->Normals);
}

bool Cone::CreateFromNormalS(
  Vectors<Rational>& inputNormals, bool UseWithExtremeMathCautionAssumeConeHasSufficientlyManyProjectiveVertices
) {
  this->flagIsTheZeroCone = false;
  this->LowestIndexNotCheckedForChopping = 0;
  this->LowestIndexNotCheckedForSlicingInDirection = 0;
  int theDim = 1;
  if (inputNormals.size > 0) {
    theDim = inputNormals[0].size;
  }
  this->Normals = inputNormals;
  for (int i = 0; i < this->Normals.size; i ++) {
    if (this->Normals[i].IsEqualToZero()) {
      this->Normals.RemoveIndexSwapWithLast(i);
      i --;
    }
  }
  int numAddedFakeWalls = 0;
  Matrix<Rational> tempMat;
  Selection tempSel;
  if (!UseWithExtremeMathCautionAssumeConeHasSufficientlyManyProjectiveVertices) {
    for (int i = 0; i < theDim && this->Normals.GetRankOfSpanOfElements(&tempMat, &tempSel) < theDim; i ++) {
      Vector<Rational> tempRoot;
      tempRoot.MakeEi(theDim, i);
      if (!this->Normals.LinSpanContainsVector(tempRoot, tempMat, tempSel)) {
        numAddedFakeWalls ++;
        this->Normals.AddOnTop(tempRoot);
      }
    }
  }
  this->ComputeVerticesFromNormalsNoFakeVertices();
  if (numAddedFakeWalls > 0) {
    this->Normals.SetSize(this->Normals.size - numAddedFakeWalls);
    Vector<Rational> tempRoot;
    int originalSize = this->Vertices.size;
    for (int i = 0; i < originalSize; i ++) {
      tempRoot = - this->Vertices[i];
      if (this->IsInCone(tempRoot)) {
        this->Vertices.AddOnTopNoRepetition(tempRoot);
      }
    }
  }
  return this->EliminateFakeNormalsUsingVertices(numAddedFakeWalls);
}

void ConeComplex::initFromCones(List<Cone>& NormalsOfCones, bool AssumeConesHaveSufficientlyManyProjectiveVertices) {
  List<Vectors<Rational> > tempRoots;
  tempRoots.SetSize(NormalsOfCones.size);
  for (int i = 0; i < NormalsOfCones.size; i ++) {
    tempRoots[i] = NormalsOfCones[i].Normals;
  }
  this->initFromCones(tempRoots, AssumeConesHaveSufficientlyManyProjectiveVertices);
}

void ConeComplex::initFromCones(
  List<Vectors<Rational> >& NormalsOfCones,
  bool UseWithExtremeMathCautionAssumeConeHasSufficientlyManyProjectiveVertices
) {
  Cone tempCone;
  this->Clear();
  ProgressReport theReport;
  theReport.Report(NormalsOfCones.ToString());
  for (int i = 0; i < NormalsOfCones.size; i ++) {
    if (tempCone.CreateFromNormalS(
      NormalsOfCones[i], UseWithExtremeMathCautionAssumeConeHasSufficientlyManyProjectiveVertices
    )) {
      this->AddNonRefinedChamberOnTopNoRepetition(tempCone);
    }
    std::stringstream out;
    out << "Initializing cone " << i + 1 << " out of " << NormalsOfCones.size;
    theReport.Report(out.str());
  }
  Vector<Rational> tempRoot;
  this->splittingNormals.Clear();
  for (int i = 0; i < this->size; i ++) {
    for (int j = 0; j < this->TheObjects[i].Normals.size; j ++) {
      tempRoot = this->TheObjects[i].Normals[j];
      tempRoot.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
      this->splittingNormals.AddOnTopNoRepetition(tempRoot);
      std::stringstream out;
      out << "Extracting walls from cone " << i + 1 << " out of " << this->size
      << " total distinct chambers.";
      out << "\nProcessed " << j + 1 << " out of " << this->TheObjects[i].Normals.size
      << " walls of the current chamber.";
      out << "\nTotal # of distinct walls found: " << this->splittingNormals.size;
      theReport.Report(out.str());
    }
  }
}

std::string Cone::ToString(FormatExpressions* theFormat) const {
  std::stringstream out;
  bool PrepareMathReport = theFormat == nullptr ? false: theFormat->flagUseLatex;
  bool useHtml = theFormat == nullptr ? false: theFormat->flagUseHTML;
  bool useLatex = theFormat == nullptr ? false: theFormat->flagUseLatex;
  bool lastVarIsConstant = false;
  if (this->flagIsTheZeroCone) {
    out << "The cone is the zero cone.";
  } else if (this->Normals.size == 0) {
    out << "The cone is the entire space";
  }
  if (!PrepareMathReport) {
    out << "Index next wall to refine by: " << this->LowestIndexNotCheckedForChopping << "\n";
    if (useHtml) {
      out << "<br>";
    }
    out << "\nIndex next direction to slice in: " << this->LowestIndexNotCheckedForSlicingInDirection << "\n";
  }
  if (useHtml) {
    out << "<br>";
  }
  out << "Normals:\n";
  if (useHtml) {
    out << "<br>";
  }
  if (useLatex) {
    out << "\\[";
  }
  FormatExpressions tempF;
  if (theFormat == nullptr) {
    theFormat = &tempF;
  }
  out << this->Normals.ElementsToInequalitiesString(useLatex, useHtml, lastVarIsConstant, *theFormat);
  if (useLatex) {
    out << "\\]";
  }
  out << "\nProjectivized Vertices: " << this->Vertices.ToString();
  if (useHtml) {
    out << "<br>";
  }
  if (this->Vertices.size > 0) {
    out << "\nInternal point: " << this->GetInternalPoint().ToString();
  }
  return out.str();
}

std::string ConeComplex::ToString(bool useHtml) {
  std::stringstream out;
  FormatExpressions theFormat;
  out << "Number of chambers: " << this->size;
  if (useHtml) {
    out << "<br>";
  }
  out << " Next non-refined chamber: " << this->indexLowestNonRefinedChamber + 1;
  if (useHtml) {
    out << "<br>";
  }
  out << "Normals of walls to refine by: ";
  Vectors<Rational> tempRoots;
  tempRoots = this->splittingNormals;
  out << tempRoots.ToString(&theFormat);
  if (this->slicingDirections.size > 0) {
    if (useHtml) {
      out << "<br>\n";
    }
    out << " Directions to slice along: " << this->slicingDirections.ToString();
  }
  for (int i = 0; i < this->size; i ++) {
    if (useHtml) {
      out << "<hr>";
    }
    out << "\n\n\nChamber " << i + 1 << ":\n";
    if (useHtml) {
      out << "<br>";
    }
    out << this->TheObjects[i].ToString(&theFormat) << "\n\n\n";
  }
  return out.str();
}

int RationalFunctionOld::GetMinNumVars() const {
  switch (this->expressionType) {
    case RationalFunctionOld::typeRational:
      return 0;
    case RationalFunctionOld::typePoly:
      return this->Numerator.GetElementConst().GetMinNumVars();
    case RationalFunctionOld::typeRationalFunction:
      return MathRoutines::Maximum(
        this->Numerator.GetElementConst().GetMinNumVars(), this->Denominator.GetElementConst().GetMinNumVars()
      );
    default: //this should never happen! maybe global.fatal << global.fatal here...
      return - 1;
  }
}

bool RationalFunctionOld::GetRelations(
  const List<Polynomial<Rational> >& inputElements,
  List<Polynomial<Rational> >& outputGeneratorLabels,
  List<Polynomial<Rational> >& outputRelations,
  std::stringstream& comments
) {
  MacroRegisterFunctionWithName("RationalFunctionOld::GetRelationsGetRelations");
  outputGeneratorLabels.SetSize(inputElements.size);
  outputRelations.SetSize(0);
  if (inputElements.size == 0) {
    return true;
  }
  List<Polynomial<Rational> > theGroebnerBasis;
  theGroebnerBasis = inputElements;
  int numStartingGenerators = inputElements.size;
  int numStartingVariables = 0;
  for (int i = 0; i < inputElements.size; i ++) {
    numStartingVariables = MathRoutines::Maximum(numStartingVariables, inputElements[0].GetMinNumVars());
  }
  Polynomial<Rational> currentGenerator;
  for (int i = 0; i < numStartingGenerators; i ++) {
    Polynomial<Rational>& currentPoly = theGroebnerBasis[i];
    currentPoly.SetNumVariablesSubDeletedVarsByOne(numStartingVariables + numStartingGenerators);
    currentGenerator.MakeDegreeOne(numStartingVariables + numStartingGenerators, i + numStartingVariables, 1);
    outputGeneratorLabels[i] = currentGenerator;
    currentPoly -= currentGenerator;
  }
  GroebnerBasisComputation<Rational> theComputation;
  theComputation.thePolynomialOrder.theMonOrder = MonomialP::Left_isGEQ_leftToRight_firstGEQ;
  if (!theComputation.TransformToReducedGroebnerBasis(theGroebnerBasis)) {
    comments << "Failed to find Groebner basis";
    return false;
  }
  outputRelations.Reserve(theGroebnerBasis.size);
  outputRelations.SetSize(0);
  for (int i = 0; i < theGroebnerBasis.size; i ++) {
    Polynomial<Rational>& currentPoly = theGroebnerBasis[i];
    bool bad = false;
    for (int j = 0; j < numStartingVariables; j ++) {
      if (currentPoly.GetMaxPowerOfVariableIndex(j) > 0) {
        bad = true;
        break;
      }
    }
    if (!bad) {
      outputRelations.AddOnTop(currentPoly);
    }
  }
  return true;
}

bool ConeComplex::findMaxLFOverConeProjective(
  const Cone& input, List<Polynomial<Rational> >& inputLinPolys, List<int>& outputMaximumOverEeachSubChamber
) {
  Vectors<Rational> HyperPlanesCorrespondingToLF;
  if (input.Normals.size < 1 || inputLinPolys.size < 1) {
    return false;
  }
  int theDim = input.Normals[0].size;
  HyperPlanesCorrespondingToLF.SetSize(inputLinPolys.size);
  for (int i = 0; i < inputLinPolys.size; i ++) {
    Polynomial<Rational>& currentPoly = inputLinPolys[i];
    if (currentPoly.TotalDegree() != 1 ) {
      global.Comments << "You messed up the total degree which must be one, instead it is "
      << currentPoly.TotalDegree() << ". The dimension of the cone is " << theDim;
      return false;
    }
    Vector<Rational>& newWall = HyperPlanesCorrespondingToLF[i];
    newWall.MakeZero(theDim);
    for (int j = 0; j < currentPoly.size(); j ++) {
      for (int k = 0; k < theDim; k ++) {
        if (currentPoly[j](k) == 1) {
          newWall[k] = currentPoly.coefficients[j];
          break;
        }
      }
    }
  }
  return this->findMaxLFOverConeProjective(
    input, HyperPlanesCorrespondingToLF, outputMaximumOverEeachSubChamber
  );
}

bool ConeComplex::findMaxLFOverConeProjective(
  const Cone& input,
  Vectors<Rational>& inputLFsLastCoordConst,
  List<int>& outputMaximumOverEeachSubChamber
) {
  this->init();
  this->AddNonRefinedChamberOnTopNoRepetition(input);
  Vector<Rational> tempRoot;
  for (int i = 0; i < inputLFsLastCoordConst.size; i ++) {
    for (int j = i + 1; j < inputLFsLastCoordConst.size; j ++) {
      tempRoot = inputLFsLastCoordConst[i] - inputLFsLastCoordConst[j];
      tempRoot.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
      if (!tempRoot.IsEqualToZero()) {
        this->splittingNormals.AddOnTopNoRepetition(tempRoot);
      }
    }
  }
  global.Comments << this->ToString(true);
  this->Refine();
  outputMaximumOverEeachSubChamber.SetSize(this->size);
  Rational theMax = 0;
  for (int i = 0; i < this->size; i ++) {
    this->TheObjects[i].GetInternalPoint(tempRoot);
    bool isInitialized = false;
    for (int j = 0; j < inputLFsLastCoordConst.size; j ++) {
      if (!isInitialized || tempRoot.ScalarEuclidean(inputLFsLastCoordConst[j]) > theMax) {
        theMax = tempRoot.ScalarEuclidean(inputLFsLastCoordConst[j]);
        outputMaximumOverEeachSubChamber[i] = j;
        isInitialized = true;
      }
    }
  }
  return true;
}

void Lattice::Reduce() {
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  this->basis.GaussianEliminationEuclideanDomain();
  int numRowsToTrim = 0;
  for (int i = this->basis.NumRows - 1; i >= 0; i --) {
    bool foundNonZeroRow = false;
    for (int j = 0; j < this->basis.NumCols; j ++) {
      if (!this->basis.elements[i][j].IsEqualToZero()) {
        foundNonZeroRow = true;
        break;
      }
    }
    if (foundNonZeroRow) {
      break;
    }
    numRowsToTrim ++;
  }
  this->basis.Resize(this->basis.NumRows-numRowsToTrim, this->basis.NumCols, true);
  this->basisRationalForm.AssignMatrixIntWithDen(this->basis, this->Den);
}

void Lattice::TestGaussianEliminationEuclideanDomainRationals(Matrix<Rational>& output) {
  output.AssignMatrixIntWithDen(this->basis, this->Den);
  std::stringstream out;
  global.Comments << "Test output: " << output.ToString();
  out << "Test output: " << output.ToString();
  output.GaussianEliminationEuclideanDomain();
  global.Comments << "<br>After gaussian elimination:" << output.ToString();
  out << "<br>After gaussian elimination:" << output.ToString();
}

void Lattice::RefineByOtherLattice(const Lattice& other) {
  if (other.basis.NumCols == 0) {
    return;
  }
  if (other.basis == this->basis && this->Den == other.Den) {
    return;
  }
  if (other.GetDim() != this->GetDim()) {
    global.fatal << "Dimension mismatch. " << global.fatal;
  }
  int theDim = this->GetDim();
  LargeIntegerUnsigned oldDen = this->Den;
  LargeIntegerUnsigned::lcm(other.Den, oldDen, this->Den);
  LargeIntegerUnsigned scaleThis, scaleOther;
  scaleThis = this->Den / oldDen;
  scaleOther = this->Den / other.Den;
  int oldNumRows = this->basis.NumRows;
  LargeInteger tempI;
  tempI = scaleThis;
  this->basis *= tempI;
  this->basis.Resize(this->basis.NumRows+other.basis.NumRows, theDim, true);
  for (int i = oldNumRows; i < this->basis.NumRows; i ++) {
    for (int j = 0; j < this->basis.NumCols; j ++) {
      this->basis.elements[i][j] = other.basis.elements[i - oldNumRows][j] * scaleOther;
    }
  }
  this->Reduce();
}

void Lattice::MakeFromMat(const Matrix<Rational>& input) {
  this->basisRationalForm = input;
  this->basisRationalForm.GetMatrixIntWithDen(this->basis, this->Den);
  this->Reduce();
}

void Lattice::MakeFromRoots(const Vectors<Rational>& input) {
  Matrix<Rational> tempMat;
  tempMat.AssignVectorsToRows(input);
  tempMat.GetMatrixIntWithDen(this->basis, this->Den);
  this->Reduce();
}

std::string Lattice::ToString() const {
  std::stringstream out;
  out << "L=<";
  Vectors<Rational> tempRoots;
  tempRoots.AssignMatrixRows(this->basisRationalForm);
  for (int i = 0; i < this->basisRationalForm.NumRows; i ++) {
    out << tempRoots[i].ToString();
    if (i != this->basisRationalForm.NumRows - 1) {
      out << ",";
    }
  }
  out << ">";
  return out.str();
}

