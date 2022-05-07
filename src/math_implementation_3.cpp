// The current file is licensed under the license terms found in the main
// header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "general_lists.h"
#include "math_general_implementation.h"
#include "math_general_polynomial_computations_basic_implementation.h"
#include "math_general_polynomial_computations_advanced_implementation.h"
#include "math_extra_finite_groups_implementation.h"
#include "math_extra_symmetric_groups_and_generalizations.h"
#include "math_extra_drawing_variables.h"
#include "string_constants.h"
#include "math_rational_function.h"
#include <dirent.h>
#include <unistd.h>
#include "math_extra_modules_semisimple_lie_algebras.h"
#include "crypto.h"

// The below gives upper limit to the amount of pointers
// that are allowed to be allocated by the program. Can be changed dynamically.
// Used to guard the web server from abuse.
#ifdef AllocationLimitsSafeguard

long long GlobalStatistics::cgiLimitRAMuseNumPointersInList = 2000000000;
#endif

bool GlobalStatistics::flagUngracefulExitInitiated = false;
long long GlobalStatistics::globalPointerCounter = 0;
long long GlobalStatistics::pointerCounterPeakRamUse = 0;
unsigned int GlobalStatistics::numberOfHashResizes = 0;
unsigned int GlobalStatistics::numberOfListResizesTotal = 0;
unsigned int GlobalStatistics::numberOfListsCreated = 0;
// CombinatorialChamberContainer GlobalCollectorChambers;
// FacetPointers GlobalCollectorFacets;
int HtmlRoutines::globalMathSpanID = 0;
int HtmlRoutines::globalGeneralPurposeID = 0;
template < >
bool Complex<double>::flagEqualityIsApproximate =
true;
template < >
double Complex<double>::equalityPrecision =
0.00000001;
template <class ElementLeft, class ElementRight, class Coefficient>
class TensorProductMonomial;

// template < > int ListPointers<PartFraction>::MemoryAllocationIncrement =100;
// ListPointers<PartFraction> PartFraction::GlobalCollectorPartFraction;
// int PartFraction::lastApplicationOfSVformulaNumNewGenerators = 0;
// int PartFraction::lastApplicationOfSVformulaNumNewMonomials = 0;
// FacetPointers TheBigFacetOutput;
// DrawingVariables TDV(200, 400);
unsigned long long int Rational::totalLargeAdditions = 0;
unsigned long long int Rational::totalLargeGreatestCommonDivisors = 0;
unsigned long long int Rational::totalLargeMultiplications = 0;
unsigned long long int Rational::totalSmallAdditions = 0;
unsigned long long int Rational::totalSmallGreatestCommonDivisors = 0;
unsigned long long int Rational::totalSmallMultiplications = 0;
template < >
List<OnePartialFractionDenominator>::Comparator* FormatExpressions::
getMonomialOrder<OnePartialFractionDenominator>() {
  return nullptr;
}

int GlobalVariables::externalCommandNoOutput(
  const std::string& systemCommand, bool logErrors
) {
  if (this->pointerExternalCommandNoOutput == nullptr) {
    global
    << Logger::red
    << "Unable to call system command without output. "
    << Logger::endL;
    return - 1;
  }
  int exitCode = this->pointerExternalCommandNoOutput(systemCommand);
  if (exitCode != 0 && logErrors) {
    global
    << Logger::red
    << "System command: "
    << systemCommand
    << " exited with "
    << exitCode
    << ". "
    << Logger::endL;
  }
  return exitCode;
}

int GlobalVariables::externalCommandStream(const std::string& systemCommand) {
  if (this->pointerExternalCommandStream == nullptr) {
    global
    << Logger::red
    << "Unable to call system command with stream output. "
    << Logger::endL;
    return - 1;
  }
  // Usually this is the non-member function int externalCommandStream():
  int exitCode = this->pointerExternalCommandStream(systemCommand);
  if (exitCode != 0) {
    global
    << Logger::red
    << "System command: "
    << systemCommand
    << " exited with "
    << exitCode
    << ". "
    << Logger::endL;
  }
  return exitCode;
}

std::string GlobalVariables::externalCommandReturnOutput(
  const std::string& systemCommand
) {
  if (this->pointerExternalCommandReturnOutput == nullptr) {
    return "Error";
  }
  return this->pointerExternalCommandReturnOutput(systemCommand);
}

void GlobalVariables::changeDirectory(const std::string& systemCommand) {
  // When not nullptr, this most likely points to: callChDirWrapper.
  if (this->pointerCallChDir != nullptr) {
    this->pointerCallChDir(
      FileOperations::convertStringToEscapedStringFileNameSafe(systemCommand)
    );
  }
}

bool GlobalVariables::Response::isTimedOut() {
  return this->flagTimedOut;
}

bool GlobalVariables::Response::monitoringAllowed() {
  return !this->flagBanProcessMonitoring && this->flagReportAllowed;
}

bool GlobalVariables::Response::reportDesired(int type) {
  (void) type;
  return
  this->monitoringAllowed() &&
  this->flagTimedOut &&
  this->flagReportDesired;
}

void GlobalVariables::Response::allowReport() {
  this->flagReportAllowed = true;
}

void GlobalVariables::Response::disallowReport() {
  this->flagReportAllowed = false;
}

GlobalVariables::Response::Response() {
  this->flagBanProcessMonitoring = false;
  this->flagReportDesired = true;
  this->flagTimedOut = false;
  this->flagReportAllowed = false;
}

GlobalVariables::GlobalVariables() {
  this->flagDeallocated = false;
  this->flagNotAllocated = false;
  this->flagCertificatesAreOfficiallySigned = false;
  this->flagIsChildProcess = false;
  this->millisecondsMaxComputation = 100000;
  // 100 seconds
  this->millisecondOffset = 0;
  this->millisecondsComputationStart = - 1;
  this->millisecondsReplyAfterComputation = 7000;
  // 7 seconds
  this->pointerExternalCommandReturnOutput = nullptr;
  this->pointerExternalCommandNoOutput = nullptr;
  this->pointerExternalCommandStream = nullptr;
  this->pointerCallChDir = nullptr;
  this->flagServerforkedIntoWorker = false;
  this->flagComputationComplete = false;
  this->flagComputationFinishedAllOutputSentClosing = false;
  this->flagRunningConsoleTest = false;
  this->flagRunningConsoleRegular = false;
  this->flagRunningBuiltInWebServer = false;
  this->flagRunningWebAssembly = false;
  this->flagRunningConsoleHelp = false;
  this->flagLoggedIn = false;
  this->flagLogInAttempted = false;
  this->flagServerDetailedLog = false;
  this->flagUsingSSLinCurrentConnection = false;
  this->flagSSLAvailable = false;
  this->millisecondsNoPingBeforeChildIsPresumedDead = 10000;
  this->mutexReturnBytes.mutexName = "WriteByteslock";
  this->flagCachingInternalFilesOn = true;
  this->flagRunServerOnEmptyCommandLine = false;
  this->flagRequestComingLocally = false;
  this->flagDatabaseCompiled = false;
  this->flagLocalhostConnectionMonitor = true;
  this->flagDaemonMonitor = false;
  this->flagDisableDatabaseLogEveryoneAsAdmin = false;
  this->flagUseMathTags = false;
  this->flagRunningFormatCode = false;
}

bool ProgressReport::tickAndWantReport() {
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

void ProgressReport::report(const std::string& stringToReport) {
  if (!this->flagInitialized) {
    return;
  }
  if (global.fatal.flagCrashInitiated) {
    this->threadIndex = - 1;
    return;
  }
  if (this->threadIndex == - 1) {
    return;
  }
  if (
    global.progressReportStrings[this->threadIndex].size > this->currentLevel
  ) {
    global.progressReportStrings[this->threadIndex][this->currentLevel] =
    stringToReport;
    global.makeReport();
  }
}

void ProgressReport::initialize() {
  this->flagInitialized = false;
  if (!global.response.monitoringAllowed()) {
    return;
  }
  if (global.fatal.flagCrashInitiated) {
    this->threadIndex = - 1;
    return;
  }
  this->threadIndex = ThreadData::getCurrentThreadId();
  if (this->threadIndex == - 1) {
    return;
  }
  this->flagInitialized = true;
  this->currentLevel = global.progressReportStrings[this->threadIndex].size;
  global.progressReportStrings[this->threadIndex].addOnTop(
    std::string("")
  );
  this->ticks = 0;
  this->ticksPerReport = 1;
  this->reportType = GlobalVariables::Response::ReportType::general;
}

ProgressReport::~ProgressReport() {
  if (!this->flagInitialized) {
    return;
  }
  if (global.fatal.flagCrashInitiated) {
    this->threadIndex = - 1;
    return;
  }
  if (this->threadIndex == - 1) {
    return;
  }
  if (global.fatal.flagCrashInitiated) {
    return;
  }
  global.progressReportStrings[this->threadIndex].size --;
}

RegisterFunctionCall::RegisterFunctionCall(
  const char* fileName, int line, const std::string& functionName
) {
  this->threadIndex = ThreadData::getCurrentThreadId();
  if (this->threadIndex == - 1) {
    return;
  }
  ListReferences<StackInfo>& stack =
  global.customStackTrace[this->threadIndex];
  stack.setSize(stack.size + 1);
  StackInfo& stackTop = stack.lastObject();
  stackTop.fileName = fileName;
  stackTop.line = line;
  stackTop.functionName = functionName;
  if (stack.size > 200000) {
    global.fatal << "Stack too deep: 200000 layers. " << global.fatal;
  }
}

RegisterFunctionCall::~RegisterFunctionCall() {
  if (this->threadIndex == - 1) {
    return;
  }
  global.customStackTrace[this->threadIndex].size --;
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

void DrawingVariables::initDrawingVariables() {
  this->defaultHtmlHeight = 400;
  this->defaultHtmlWidth = 400;
  this->drawLineFunction = nullptr;
  this->drawTextFunction = nullptr;
  this->drawCircleFunction = nullptr;
  this->drawClearScreenFunction = nullptr;
  this->fontSizeNormal = 10;
  this->fontSizeSubscript = 6;
  this->flagLaTeXDraw = false;
  this->flagDisplayingCreationNumbersInsteadOfDisplayNumbers = false;
  this->flagDrawChamberIndices = true;
  this->flagDrawingInvisibles = false;
  this->flagDrawingLinkToOrigin = true;
  this->flagFillUserDefinedProjection = false;
  this->colorDashes = static_cast<int>(
    HtmlRoutines::redGreenBlue(200, 200, 200)
  );
  this->flag2DprojectionDraw = true;
  this->flagIncludeExtraHtmlDescriptions = true;
  this->flagAllowMovingCoordinateSystemFromArbitraryClick = true;
  this->ColorChamberIndicator = static_cast<int>(
    HtmlRoutines::redGreenBlue(220, 220, 0)
  );
  this->ColorWeylChamberWalls = static_cast<int>(
    HtmlRoutines::redGreenBlue(220, 220, 0)
  );
  this->ColorTextPermanentlyZeroChamber = static_cast<int>(
    HtmlRoutines::redGreenBlue(250, 220, 220)
  );
  this->ColorTextZeroChamber = static_cast<int>(
    HtmlRoutines::redGreenBlue(200, 100, 100)
  );
  this->ColorTextDefault = static_cast<int>(
    HtmlRoutines::redGreenBlue(0, 0, 0)
  );
  this->Selected = - 2;
  this->textX = 0;
  this->textY = 15;
  this->flagPlotShowJavascriptOnly = false;
  this->flagUseGraphicsOld = false;
  this->operations.initialize();
}

std::stringstream HtmlRoutines::outputStream;
int HtmlRoutines::numLinesAll;
int HtmlRoutines::shiftX = 0;
int HtmlRoutines::numDashedLines = 0;
int HtmlRoutines::numRegularLines = 0;
int HtmlRoutines::numDottedLines = 0;
int HtmlRoutines::shiftY = - 200;
int HtmlRoutines::scale = 100;
std::string HtmlRoutines::cleanUpForLaTeXLabelUse(
  const std::string& inputString
) {
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

void HtmlRoutines::clearDollarSigns(
  std::string& input, std::string& output
) {
  std::stringstream out;
  for (unsigned int i = 0; i < input.size(); i ++) {
    if (input[i] != '$') {
      out << input[i];
    }
  }
  output = out.str();
}

std::string HtmlRoutines::doubleBackslashes(const std::string& input) {
  std::stringstream out;
  for (unsigned i = 0; i < input.size(); i ++) {
    out << input[i];
    if (input[i] == '\\') {
      out << "\\";
    }
  }
  return out.str();
}

std::string HtmlRoutines::clearNewLines(const std::string& inputString) {
  std::stringstream out;
  for (unsigned i = 0; i < inputString.size(); i ++) {
    if (inputString[i] == '\n' || inputString[i] == '\r') {
      out << " ";
    } else {
      out << inputString[i];
    }
  }
  return out.str();
}

std::string HtmlRoutines::backslashQuotes(const std::string& inputString) {
  std::stringstream out;
  for (unsigned i = 0; i < inputString.size(); i ++) {
    if (inputString[i] == '"') {
      out << "\\";
    }
    out << inputString[i];
  }
  return out.str();
}

std::string HtmlRoutines::clearSlashes(const std::string& inputString) {
  std::stringstream out;
  for (unsigned int i = 0; i < inputString.size(); i ++) {
    if (inputString[i] != '\\') {
      out << inputString[i];
    }
  }
  return out.str();
}

void HtmlRoutines::subEqualitiesWithSimeq(
  std::string& input, std::string& output
) {
  std::stringstream out;
  for (unsigned int i = 0; i < input.size(); i ++) {
    if (input[i] != '=') {
      out << input[i];
    } else {
      out << "\\simeq ";
    }
  }
  output = out.str();
}

void HtmlRoutines::elementToStringTooltip(
  const std::string& input,
  const std::string& inputTooltip,
  std::string& output,
  bool useHtml
) {
  std::stringstream out;
  if (useHtml) {
    out << "<span title ='" << inputTooltip << "'>" << input << "</span>";
  }
  output = out.str();
}

std::string HtmlRoutines::getStyleButtonLikeHtml() {
  return
  " style='background:none; border:0; text-decoration:underline; color:blue; cursor:pointer' "
  ;
}

std::string HtmlRoutines::convertStringEscapeQuotesAndBackslashes(
  const std::string& input
) {
  STACK_TRACE("HtmlRoutines::convertStringEscapeQuotesAndBackslashes");
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

bool StringRoutines::Conversions::isValidUtf8(const std::string& input) {
  List<uint32_t> codePoints;
  return utf8StringToUnicodeCodePoints(input, codePoints, nullptr);
}

bool StringRoutines::Conversions::utf8StringToUnicodeCodePoints(
  const std::string& input,
  List<uint32_t>& output,
  std::stringstream* commentsOnFailure
) {
  output.setSize(0);
  output.setExpectedSize(input.size());
  bool result = true;
  for (unsigned i = 0; i < input.size(); i ++) {
    bool isValid = true;
    output.addOnTop(
      StringRoutines::Conversions::codePointFromUtf8(
        input, i, isValid, commentsOnFailure
      )
    );
    if (!isValid) {
      result = false;
      // Do not generate comments on further errors.
      commentsOnFailure = nullptr;
    }
  }
  return result;
}

uint32_t StringRoutines::Conversions::codePointFromUtf8(
  const std::string& input,
  unsigned& indexIncrementedToLastConsumedByte,
  bool& isValid,
  std::stringstream* commentsOnFailure
) {
  unsigned char next = input[indexIncrementedToLastConsumedByte];
  unsigned char leadingBit = next >> 7;
  isValid = true;
  if (leadingBit == 0) {
    return static_cast<uint32_t>(next);
  }
  unsigned bytesToRead = 0;
  if ((next >> 5) == 6) {
    // 6 is the bit sequence 110.
    // A two-byte utf8 sequence.
    bytesToRead = 2;
  } else if ((next >> 4) == 14) {
    // 14 is the bit sequence 1110.
    bytesToRead = 3;
  } else if ((next >> 3) == 30) {
    // 30 is the bit sequence 11110.
    bytesToRead = 4;
  } else {
    // Invalid utf encoding: does not start with one of:
    // 0x0, 0x110, 0x1110, 0x11110.
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Byte "
      << StringRoutines::convertByteToHex(next)
      << " with index "
      << indexIncrementedToLastConsumedByte
      << " does not start with a valid utf8 sequence.";
    }
    isValid = false;
    return static_cast<uint32_t>(next);
  }
  // The static cast below ensures that its argument won't be
  // silently converted to a
  // 32-bit integer.
  unsigned char nextContribution = static_cast<unsigned char>(
    next << (bytesToRead + 1)
  ) >> (bytesToRead + 1);
  uint32_t codePoint = nextContribution;
  for (unsigned j = 1; j < bytesToRead; j ++) {
    if (indexIncrementedToLastConsumedByte + j >= input.size()) {
      // The bytes promised by the utf8 encoding are missing.
      if (
        // Generate an error message for the first error only.
        commentsOnFailure != nullptr
      ) {
        *commentsOnFailure
        << "Missing bytes at byte: "
        << StringRoutines::convertByteToHex(next)
        << " of index "
        << indexIncrementedToLastConsumedByte
        << ".";
      }
      isValid = false;
      return static_cast<uint32_t>(next);
    }
    // Strip the first two bits.
    nextContribution = static_cast<unsigned char>(
      input[indexIncrementedToLastConsumedByte + j] << 2
    ) >>
    2;
    codePoint <<= 6;
    codePoint += nextContribution;
  }
  indexIncrementedToLastConsumedByte += bytesToRead - 1;
  return codePoint;
}

std::string StringRoutines::Conversions::codePointToBackslashEscapedString(
  uint32_t input
) {
  std::stringstream out;
  if (input <= 127) {
    unsigned char currentCharacter = static_cast<unsigned char>(input);
    if (currentCharacter == '\\') {
      return "\\\\";
    } else if (currentCharacter == '\n') {
      return "\\n";
    } else if (currentCharacter == '\r') {
      return "\\r";
    } else if (currentCharacter == '"') {
      return "\\\"";
    } else {
      out << currentCharacter;
      return out.str();
    }
  }
  unsigned char byte0 = static_cast<unsigned char>(input);
  unsigned char byte1 = static_cast<unsigned char>(input >> 8);
  if (input <= 0xFFFF) {
    out
    << "\\u"
    << StringRoutines::convertByteToHex(byte1)
    << StringRoutines::convertByteToHex(byte0);
    return out.str();
  }
  unsigned char byte2 = static_cast<unsigned char>(input >> 16);
  unsigned char byte3 = static_cast<unsigned char>(input >> 24);
  out
  << "\\u"
  << StringRoutines::convertByteToHex(byte3)
  << StringRoutines::convertByteToHex(byte2)
  << "\\u"
  << StringRoutines::convertByteToHex(byte1)
  << StringRoutines::convertByteToHex(byte0);
  return out.str();
}

std::string StringRoutines::Conversions::stringToJSONStringEscaped(
  const std::string& inputUtf8
) {
  List<uint32_t> codePoints;
  StringRoutines::Conversions::utf8StringToUnicodeCodePoints(
    inputUtf8, codePoints, nullptr
  );
  std::stringstream out;
  for (int i = 0; i < codePoints.size; i ++) {
    out
    << StringRoutines::Conversions::codePointToBackslashEscapedString(
      codePoints[i]
    );
  }
  return out.str();
}

std::string StringRoutines::Conversions::escapeQuotesBackslashesNewLines(
  const std::string& input
) {
  STACK_TRACE("StringRoutines::escapeQuotesBackslashesNewLines");
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
      out << input[i];
    }
  }
  return out.str();
}

std::string StringRoutines::convertStringToJavascriptVariable(
  const std::string& input
) {
  STACK_TRACE("StringRoutines::ConvertStringForJavascript");
  std::stringstream out;
  for (unsigned i = 0; i < input.size(); i ++) {
    if (i == 0 && MathRoutines::isDigit(input[i])) {
      out << "x";
    }
    if (
      MathRoutines::isDigit(input[i]) ||
      MathRoutines::isLatinLetter(input[i])
    ) {
      out << input[i];
    } else {
      out
      << "x"
      << StringRoutines::convertByteToHex(
        static_cast<unsigned char>(input[i])
      );
    }
  }
  return out.str();
}

std::string StringRoutines::Conversions::codePointToUtf8(uint32_t input) {
  std::stringstream out;
  if (input >=
2097152 // = 2^21-1
  ) {
    // Invalid code point.
    // A unicode code point must be smaller than 2^21.
    // It must fit in the largest possible encoding:
    // b_11110???,b_10??????,b_10??????,b_10??????  (3+3*6=21 payload bits).
    // We convert the code point to the 4-byte bigendian sequence that
    // represents the integer.
    List<unsigned char> fourBytes;
    Crypto::convertUint32ToUcharBigendiaN(input, fourBytes);
    out << fourBytes[0] << fourBytes[1] << fourBytes[2] << fourBytes[3];
    return out.str();
  }
  if (input < 128) {
    unsigned char output = input % 256;
    out << output;
    return out.str();
  }
  if (input <
2048 // = 2^11, encoding fits in b_110?????, b_10??????
  ) {
    unsigned char high = input >> 6;
    high += 128 + 64;
    // b_11000000
    unsigned char low = input % 64;
    low += 128;
    out << high << low;
    return out.str();
  }
  if (
    input <
    65536 // = 2^16, encoding fits in b_1110????, b_10??????, b_10??????
  ) {
    unsigned char high = input >> 12;
    high += 128 + 64 + 32;
    // b_11100000
    unsigned char middle = (input >> 6) % 64;
    middle += 128;
    unsigned char low = input % 64;
    low += 128;
    out << high << middle << low;
    return out.str();
  }
  unsigned char highest = input >> 18;
  highest += 128 + 64 + 32 + 16;
  // 224 = b_11110000
  unsigned char higher = (input >> 12) % 64;
  higher += 128;
  unsigned char lower = (input >> 6) % 64;
  lower += 128;
  unsigned char lowest = input % 64;
  lowest += 128;
  out << highest << higher << lower << lowest;
  return out.str();
}

std::string StringRoutines::Conversions::unescapeJavascriptLike(
  const std::string& input
) {
  STACK_TRACE("StringRoutines::Conversions::unescapeJavascriptLike");
  std::stringstream out;
  for (unsigned i = 0; i < input.size(); i ++) {
    if (i + 1 >= input.size() || input[i] != '\\') {
      out << input[i];
      continue;
    }
    char next = input[i + 1];
    if (next == 'n') {
      // Sequence "\n".
      out << "\n";
      i ++;
      continue;
    }
    if (next == '\\') {
      // Sequence "\\".
      out << "\\";
      i ++;
      continue;
    }
    if (next == '"') {
      // Sequence "\"".
      out << "\"";
      i ++;
      continue;
    }
    if (next == 'x' && i + 3 >= input.size()) {
      // Sequence "\x??".
      char left =
      MathRoutines::convertHumanReadableHexToCharValue(input[i + 2]);
      char right =
      MathRoutines::convertHumanReadableHexToCharValue(input[i + 3]);
      if (left == - 1 || right == - 1) {
        // Sequence not a byte encoding: "\xPQ".
        out << "\\";
        continue;
      }
      // The sequence is a byte encoding: "\xAb". Mixed case allowed.
      char byte = static_cast<char>(left * 16 + right);
      out << byte;
      i += 3;
      continue;
    }
    if (next != 'u' || i + 5 >= input.size()) {
      out << "\\";
      continue;
    }
    // Sequence "\u????".
    char hex0 =
    MathRoutines::convertHumanReadableHexToCharValue(input[i + 2]);
    char hex1 =
    MathRoutines::convertHumanReadableHexToCharValue(input[i + 3]);
    char hex2 =
    MathRoutines::convertHumanReadableHexToCharValue(input[i + 4]);
    char hex3 =
    MathRoutines::convertHumanReadableHexToCharValue(input[i + 5]);
    if (hex0 == - 1 || hex1 == - 1 || hex2 == - 1 || hex3 == - 1) {
      // Sequence not a two-byte encoding: "\u010P".
      out << "\\";
      continue;
    }
    uint32_t codePoint = static_cast<unsigned>(hex0) * 4096 +
    static_cast<unsigned>(hex1) *
    256 +
    static_cast<unsigned>(hex2) *
    16 +
    static_cast<unsigned>(hex3);
    out << StringRoutines::Conversions::codePointToUtf8(codePoint);
    i += 5;
  }
  return out.str();
}

std::string StringRoutines::Conversions::escapeJavascriptLike(
  const std::string& input
) {
  STACK_TRACE("StringRoutines::Conversions::escapeJavascriptLike");
  std::stringstream out;
  for (unsigned i = 0; i < input.size(); i ++) {
    unsigned char current = input[i];
    if (current == '"') {
      out << "\\\"";
    } else if (current == '\\') {
      out << "\\\\";
    } else if (current == '\n') {
      out << "\\n";
    } else if (current == '\r') {
      out << "\\r";
    } else if (StringRoutines::isASCIICharacterVisible(current)) {
      out << current;
    } else {
      out << "\\x" << StringRoutines::convertByteToHex(current) << "";
    }
  }
  return out.str();
}

std::string HtmlRoutines::convertStringEscapeNewLinesQuotesBackslashes(
  const std::string& input
) {
  STACK_TRACE("HtmlRoutines::convertStringEscapeNewLinesQuotesBackslashes");
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

std::string HtmlRoutines::convertStringToHtmlStringRestrictSize(
  const std::string& inputString,
  bool doReplaceNewLineByBr,
  int maxStringSize
) {
  std::string result =
  HtmlRoutines::convertStringToHtmlString(inputString, doReplaceNewLineByBr);
  if (maxStringSize > 0) {
    if (static_cast<signed>(result.size()) > maxStringSize) {
      std::stringstream resultStream;
      resultStream
      << result.substr(0, static_cast<unsigned>(maxStringSize))
      << "...[only first "
      << maxStringSize
      << " chars shown]. ";
      return resultStream.str();
    }
  }
  return result;
}

std::string HtmlRoutines::convertStringToHtmlString(
  const std::string& inputString, bool doReplaceNewLineByBr
) {
  std::string result;
  HtmlRoutines::convertStringToHtmlStringReturnTrueIfModified(
    inputString, result, doReplaceNewLineByBr
  );
  return result;
}

uint32_t HtmlRoutines::redGreenBlue(
  unsigned int r, unsigned int g, unsigned int b
) {
  r = r % 256;
  g = g % 256;
  b = b % 256;
  return r * 65536 + g * 256 + b;
}

bool FileOperations::isFolderUnsecure(const std::string& folderName) {
  STACK_TRACE("FileOperations::isFolderUnsecure");
  DIR* pDir;
  pDir = opendir(folderName.c_str());
  if (pDir != nullptr) {
    closedir(pDir);
    return true;
  }
  return false;
}

std::string FileOperations::getFileExtensionWithDot(
  const std::string& fileName, std::string* outputFileNameNoExtension
) {
  STACK_TRACE("FileOperations::getFileExtensionWithDot");
  if (fileName == "" || fileName.size() <= 0) {
    return "";
  }
  if (&fileName == outputFileNameNoExtension) {
    std::string copy = fileName;
    return
    FileOperations::getFileExtensionWithDot(copy, outputFileNameNoExtension);
  }
  for (
    int i = static_cast<signed>(fileName.size() - 1); i >= 0; i --
  ) {
    if (fileName[static_cast<unsigned>(i)] == '.') {
      if (outputFileNameNoExtension != nullptr) {
        *outputFileNameNoExtension =
        fileName.substr(0, static_cast<unsigned>(i));
      }
      return
      fileName.substr(static_cast<unsigned>(i), std::string::npos);
    }
  }
  return "";
}

std::string FileOperations::convertStringToLatexFileName(
  const std::string& input
) {
  STACK_TRACE("FileOperations::convertStringToLatexFileName");
  std::stringstream out;
  for (unsigned i = 0; i < input.size(); i ++) {
    if (
      MathRoutines::isDigit(input[i]) ||
      MathRoutines::isLatinLetter(input[i])
    ) {
      out << input[i];
    } else if (input[i] == ' ' || input[i] == ':') {
      out << "_";
    } else {
      out << "_" << static_cast<int>(input[i]);
    }
  }
  std::string result = out.str();
  StringRoutines::stringTrimToLengthWithHash(result, 220);
  return result;
}

std::string FileOperations::convertStringToEscapedStringFileNameSafe(
  const std::string& input
) {
  STACK_TRACE("FileOperations::convertStringToEscapedStringFileNameSafe");
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

bool FileOperations::isOKFileNameVirtual(
  const std::string& fileName,
  bool accessSensitiveFolders,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("FileOperations::isOKFileNameVirtual");
  (void) accessSensitiveFolders;
  std::string fileNameNoPath =
  FileOperations::getFileNameFromFileNameWithPath(fileName);
  std::string filePath = FileOperations::getPathFromFileNameWithPath(fileName);
  if (filePath.size() > 10000000) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Invalid file name: too long. ";
    }
    return false;
  }
  if (filePath.size() > 0) {
    if (filePath[0] == '.') {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "Invalid file name: "
        << fileName
        << ": starts with dot but not with ./. ";
      }
      return false;
    }
  }
  for (unsigned i = 0; i < filePath.size(); i ++) {
    if (filePath[i] == '.') {
      if (i + 1 < filePath.size()) {
        if (filePath[i + 1] == '.') {
          if (commentsOnFailure != nullptr) {
            *commentsOnFailure
            << "Invalid file name: "
            << fileName
            << ": has two consecutive dots. ";
          }
          return false;
        }
      }
    }
  }
  if (fileNameNoPath.size() > 0) {
    if (fileNameNoPath[0] == '.') {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "Invalid file name: "
        << fileName
        << ": starts with dot. ";
      }
      return false;
    }
  }
  return true;
}

bool FileOperations::isFileNameWithoutDotsAndSlashes(
  const std::string& fileName
) {
  STACK_TRACE("FileOperations::isFileNameWithoutDotsAndSlashes");
  for (unsigned i = 0; i < fileName.size(); i ++) {
    if (
      fileName[i] == '/' || fileName[i] == '\\' || fileName[i] == '.'
    ) {
      return false;
    }
  }
  return true;
}

List<bool> FileOperations::safeFileCharacters;
List<bool>& FileOperations::getSafeFileChars() {
  if (FileOperations::safeFileCharacters.size == 0) {
    FileOperations::safeFileCharacters.initializeFillInObject(256, false);
    std::string characters =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    characters += "0123456789";
    characters += "@";
    characters += "+-/=._%";
    for (unsigned i = 0; i < characters.size(); i ++) {
      FileOperations::safeFileCharacters[
        static_cast<int>(
          static_cast<unsigned int>(
            static_cast<unsigned char>(characters[i])
          )
        )
      ] =
      true;
    }
  }
  return FileOperations::safeFileCharacters;
}

std::string FileOperations::cleanUpForFileNameUse(
  const std::string& inputString
) {
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

bool FileOperations::isFileNameSafeForSystemCommands(
  const std::string& fileName, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("FileOperations::isFileNameSafeForSystemCommands");
  const unsigned maxAllowedFileNameSize = 1000;
  if (fileName.size() > maxAllowedFileNameSize) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "File name has length: "
      << fileName.size()
      << "; max allowed file name size is: "
      << maxAllowedFileNameSize;
    }
    return false;
  }
  for (unsigned i = 0; i < fileName.size(); i ++) {
    if (!FileOperations::getSafeFileChars()[fileName[i]]) {
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure
        << "Character: "
        << fileName[i]
        << " not allowed in file name. ";
      }
      return false;
    }
  }
  return true;
}

std::string FileOperations::getFileNameFromFileNameWithPath(
  const std::string& fileName
) {
  unsigned startNameWithoutFolderInfo = 0;
  for (unsigned i = 0; i < fileName.size(); i ++) {
    if (fileName[i] == '/' || fileName[i] == '\\') {
      startNameWithoutFolderInfo = i + 1;
    }
  }
  std::stringstream nameWithoutFolderInfo;
  for (
    unsigned i = startNameWithoutFolderInfo; i < fileName.size(); i ++
  ) {
    nameWithoutFolderInfo << fileName[i];
  }
  return nameWithoutFolderInfo.str();
}

std::string FileOperations::getPathFromFileNameWithPath(
  const std::string& fileName
) {
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

bool FileOperations::getFolderFileNamesVirtual(
  const std::string& folderName,
  List<std::string>& outputFileNamesNoPath,
  List<std::string>* outputFileTypesWithDot,
  bool accessSensitiveFolders,
  bool accessULTRASensitiveFolders,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("FileOperations::getFolderFileNamesVirtual");
  std::string computedFolderName;
  if (
    !FileOperations::getPhysicalFileNameFromVirtual(
      folderName,
      computedFolderName,
      accessSensitiveFolders,
      accessULTRASensitiveFolders,
      commentsOnFailure
    )
  ) {
    return false;
  }
  return
  FileOperations::getFolderFileNamesUnsecure(
    computedFolderName, outputFileNamesNoPath, outputFileTypesWithDot
  );
}

bool FileOperations::getFolderFileNamesUnsecure(
  const std::string& folderName,
  List<std::string>& outputFileNamesNoPath,
  List<std::string>* outputFileTypesWithDot
) {
  STACK_TRACE("FileOperations::getFolderFileNamesUnsecure");
  DIR* directory = opendir(folderName.c_str());
  if (directory == nullptr) {
    return false;
  }
  outputFileNamesNoPath.reserve(1000);
  if (outputFileTypesWithDot != nullptr) {
    outputFileTypesWithDot->reserve(1000);
  }
  std::string fileNameNoPath, fullName, extension;
  for (
    dirent* fileOrFolder = readdir(directory); fileOrFolder != nullptr;
    fileOrFolder = readdir(directory)
  ) {
    outputFileNamesNoPath.addOnTop(fileOrFolder->d_name);
    if (outputFileTypesWithDot != nullptr) {
      fileNameNoPath = fileOrFolder->d_name;
      fullName = folderName + fileNameNoPath;
      if (FileOperations::isFolderUnsecure(fullName)) {
        outputFileTypesWithDot->addOnTop(".d");
      } else {
        extension = FileOperations::getFileExtensionWithDot(fileNameNoPath);
        if (extension == ".d") {
          extension = "";
        }
        outputFileTypesWithDot->addOnTop(extension);
      }
    }
  }
  closedir(directory);
  return true;
}

bool FileOperations::loadFiletoStringVirtualCustomizedReadOnly(
  const std::string& fileName,
  std::string& output,
  std::stringstream* commentsOnFailure
) {
  if (fileName == "") {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Empty file name not allowed. ";
    }
    return false;
  }
  std::string computedFileName;
  if (
    !FileOperations::getPhysicalFileNameFromVirtualCustomizedReadOnly(
      fileName, computedFileName, commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Failed to extract physical file name from the virtual file name: "
      << fileName;
    }
    return false;
  }
  return
  FileOperations::loadFileToStringUnsecure(
    computedFileName, output, commentsOnFailure
  );
}

bool FileOperations::writeFileVirual(
  const std::string& fileNameVirtual,
  const std::string& fileContent,
  std::stringstream* commentsOnError
) {
  return
  FileOperations::writeFileVirualWithPermissions(
    fileNameVirtual, fileContent, false, commentsOnError
  );
}

bool FileOperations::writeFileVirualWithPermissions(
  const std::string& fileNameVirtual,
  const std::string& fileContent,
  bool accessSensitiveFolders,
  std::stringstream* commentsOnError
) {
  return
  FileOperations::
  writeFileVirualWithPermissions_AccessUltraSensitiveFoldersIfNeeded(
    fileNameVirtual,
    fileContent,
    accessSensitiveFolders,
    false,
    commentsOnError
  );
}

bool FileOperations::
writeFileVirualWithPermissions_AccessUltraSensitiveFoldersIfNeeded(
  const std::string& fileNameVirtual,
  const std::string& fileContent,
  bool accessSensitiveFolders,
  bool accessUltraSensistiveFolders,
  std::stringstream* commentsOnError
) {
  std::fstream file;
  if (
    !FileOperations::
    openFileCreateIfNotPresentVirtualCreateFoldersIfNeeded_UltraSensitiveOptions
    (
      file,
      fileNameVirtual,
      false,
      true,
      false,
      accessSensitiveFolders,
      accessUltraSensistiveFolders
    )
  ) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "Filed to open file. ";
    }
    return false;
  }
  file << fileContent;
  return true;
}

std::string FileOperations::writeFileReturnHTMLLink(
  const std::string& fileContent,
  const std::string& fileNameVirtual,
  const std::string& linkText
) {
  STACK_TRACE("Calculator::writeFileReturnHTMLLink");
  std::stringstream commentsOnError;
  bool success =
  FileOperations::writeFileVirual(
    fileNameVirtual, fileContent, &commentsOnError
  );
  if (!success) {
    global.fatal
    << "Failed to write file. "
    << commentsOnError.str()
    << global.fatal;
  }
  std::stringstream out;
  out
  << "<a href=\""
  << fileNameVirtual
  << "\" target = \"_blank\">"
  << linkText
  << "</a>";
  return out.str();
}

bool FileOperations::loadFileToStringVirtual(
  const std::string& fileName,
  std::string& output,
  bool accessSensitiveFolders,
  std::stringstream* commentsOnFailure
) {
  return
  FileOperations::loadFiletoStringVirtual_AccessUltraSensitiveFoldersIfNeeded(
    fileName, output, accessSensitiveFolders, false, commentsOnFailure
  );
}

bool FileOperations::
loadFiletoStringVirtual_AccessUltraSensitiveFoldersIfNeeded(
  const std::string& fileName,
  std::string& output,
  bool accessSensitiveFolders,
  bool accessULTRASensitiveFolders,
  std::stringstream* commentsOnFailure
) {
  std::string computedFileName;
  if (
    !FileOperations::getPhysicalFileNameFromVirtual(
      fileName,
      computedFileName,
      accessSensitiveFolders,
      accessULTRASensitiveFolders,
      commentsOnFailure
    )
  ) {
    return false;
  }
  return
  FileOperations::loadFileToStringUnsecure(
    computedFileName, output, commentsOnFailure
  );
}

bool FileOperations::loadFileToStringUnsecure(
  const std::string& fileNameUnsecure,
  std::string& output,
  std::stringstream* commentsOnFailure
) {
  if (!FileOperations::fileExistsUnsecure(fileNameUnsecure)) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "The requested file "
      << HtmlRoutines::convertStringToHtmlString(fileNameUnsecure, false)
      << " does not appear to exist. ";
    }
    return false;
  }
  std::ifstream fileStream;
  if (
    !FileOperations::openFileUnsecureReadOnly(
      fileStream, fileNameUnsecure, false
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "The requested file "
      << HtmlRoutines::convertStringToHtmlString(fileNameUnsecure, false)
      <<
      " exists but I failed to open it in text mode (perhaps not a valid ASCII/UTF8 file). "
      ;
    }
    return false;
  }
  std::stringstream contentStream;
  contentStream << fileStream.rdbuf();
  output = contentStream.str();
  return true;
}

#include "general_list_references.h"

MapList<
  std::string,
  std::string,
  HashFunctions::hashFunction<std::string>
>& FileOperations::folderVirtualLinksNonSensitive() {
  static MapList<
    std::string,
    std::string,
    HashFunctions::hashFunction<std::string>
  > result;
  return result;
}

HashedList<std::string>& FileOperations::
folderStartsToWhichWeAppendInstructorUsernameSlash() {
  static HashedList<std::string> result;
  return result;
}

HashedList<std::string>& FileOperations::
folderVirtualLinksToWhichWeAppendTimeAndBuildHash() {
  static HashedList<std::string> result;
  return result;
}

MapList<
  std::string,
  std::string,
  HashFunctions::hashFunction<std::string>
>& FileOperations::folderVirtualLinksSensitive() {
  static MapList<
    std::string,
    std::string,
    HashFunctions::hashFunction<std::string>
  > result;
  return result;
}

MapList<
  std::string,
  std::string,
  HashFunctions::hashFunction<std::string>
>& FileOperations::folderVirtualLinksULTRASensitive() {
  static MapList<
    std::string,
    std::string,
    HashFunctions::hashFunction<std::string>
  > result;
  return result;
}

List<List<std::string> >& FileOperations::folderVirtualLinksDefault() {
  static List<List<std::string> > result;
  return result;
}

void FileOperations::initializeFoldersULTRASensitive() {
  STACK_TRACE("WebServer::InitializeMainFoldersULTRASensitive");
  MapList<
    std::string,
    std::string,
    HashFunctions::hashFunction<std::string>
  >& folderSubstitutionsULTRASensitive =
  FileOperations::folderVirtualLinksULTRASensitive();
  // <- allocates data structure
  folderSubstitutionsULTRASensitive.setKeyValue(
    "certificates/", "certificates/"
  );
  folderSubstitutionsULTRASensitive.setKeyValue("/results/", "results/");
  folderSubstitutionsULTRASensitive.setKeyValue("results/", "results/");
  folderSubstitutionsULTRASensitive.setKeyValue(
    "crashes/", "results/crashes/"
  );
  folderSubstitutionsULTRASensitive.setKeyValue("calculator", "calculator");
}

void FileOperations::initializeFoldersSensitive() {
  STACK_TRACE("WebServer::InitializeMainFoldersSensitive");
  MapList<
    std::string,
    std::string,
    HashFunctions::hashFunction<std::string>
  >& folderSubstitutionsSensitive =
  FileOperations::folderVirtualLinksSensitive();
  folderSubstitutionsSensitive.clear();
  folderSubstitutionsSensitive.setKeyValue("LogFiles/", "LogFiles/");
  folderSubstitutionsSensitive.setKeyValue("/LogFiles/", "LogFiles/");
  folderSubstitutionsSensitive.setKeyValue("configuration/", "configuration/");
  folderSubstitutionsSensitive.setKeyValue(
    "/configuration/", "configuration/"
  );
  folderSubstitutionsSensitive.setKeyValue(
    Configuration::freecalc, "../freecalc/"
  );
  folderSubstitutionsSensitive.setKeyValue(
    "/" + Configuration::freecalc, "../freecalc/"
  );
  folderSubstitutionsSensitive.setKeyValue(
    "database_fallback/database.json", "database_fallback/database.json"
  );
  folderSubstitutionsSensitive.setKeyValue(
    "/database_fallback/database.json", "database_fallback/database.json"
  );
}

List<List<std::string> >& FileOperations::initializeFolderVirtualLinksDefaults(
) {
  List<List<std::string> >& result =
  FileOperations::folderVirtualLinksDefault();
  // The default values given here are overridden by file configuration.json.
  // substitution order matters, earlier substitutions are processed first.
  // Once a substitution is found, no more substitutions are carried out.
  result = List<List<std::string> >({
      List<std::string>({
          Configuration::calculatorWasm,
          "calculator_html/web_assembly/calculator.wasm"
        }
      ),
      List<std::string>({Configuration::examples, "examples/"}),
      List<std::string>({
          Configuration::HTMLAceSrcMin, "html-common/ace/src-min/"
        }
      ),
      List<std::string>({Configuration::HTMLCommonFonts, "html-common/fonts"}
      ),
      List<std::string>({Configuration::HTMLCommonFont, "html-common/fonts"}
      ),
      List<std::string>({Configuration::HTMLCommon, "html-common/"}),
      List<std::string>({Configuration::calculatorHTML, "calculator_html/"}
      ),
      List<std::string>({Configuration::publicHTML, "public_html/"}),
      List<std::string>({
          Configuration::certificatesPublic, "certificates-public/"
        }
      ),
      List<std::string>({Configuration::sourceCode, "src/"}),
      List<std::string>({Configuration::testFolder, "test/"}),
      List<std::string>({Configuration::outputFolder, "output/"}),
      List<std::string>({Configuration::HTMLGeneral, "public_html/"}),
      List<std::string>({Configuration::problemsFolder, "../problems/"}),
      List<std::string>({
          Configuration::courseTemplates, "../coursetemplates/"
        }
      ),
      List<std::string>({
          Configuration::coursesAvailable, "../coursesavailable/"
        }
      ),
      List<std::string>({Configuration::topicLists, "../topiclists/"}),
      List<std::string>({
          Configuration::laTeXMaterials, "../LaTeX-materials/"
        }
      ),
      List<std::string>({Configuration::slidesVideo, "../slides-video/"}),
      List<std::string>({Configuration::freecalc, "../freecalc/"}),
      List<std::string>({"favicon.ico", "calculator_html/favicon.ico"}),

    }
  );
  FileOperations::checkFolderLinks();
  return result;
}

void FileOperations::initializeFoldersNonSensitive() {
  STACK_TRACE("WebServer::InitializeMainFoldersNonSensitive");
  // Warning: order of substitutions is important.
  // Only the first rule that applies is applied, once.
  // No further rules are applied after that.
  // Location keys that start with "/" are coming from webserver references.
  // Location keys that do not start with "/" are for internal use.
  MapList<
    std::string,
    std::string,
    HashFunctions::hashFunction<std::string>
  >& folderSubstitutionsNonSensitive =
  FileOperations::folderVirtualLinksNonSensitive();
  // initializeFolderVirtualLinksDefaults() is called in
  // GlobalVariables::configurationProcess and its contents
  // are used together with file configuration.json,
  // to compute the folder locations below.
  std::string HTMLCommonFolder =
  global.configuration[Configuration::HTMLCommon].stringValue;
  List<List<std::string> >& links =
  FileOperations::folderVirtualLinksDefault();
  for (int i = 0; i < links.size; i ++) {
    std::string& key = links[i][0];
    std::string value = global.configuration.getValue(key).stringValue;
    if (value == "") {
      global.fatal
      << Logger::red
      << "Unexpected empty folder mapping: key: "
      << key
      << ", value: "
      << value
      << global.fatal;
    }
    folderSubstitutionsNonSensitive.setKeyValue(key, value);
    folderSubstitutionsNonSensitive.setKeyValue("/" + key, value);
  }
}

bool FileOperations::checkFolderLinks() {
  STACK_TRACE("FileOperations::checkFolderLinks");
  List<List<std::string> > links = FileOperations::folderVirtualLinksDefault();
  for (int i = 0; i < links.size; i ++) {
    if (links[i].size != 2) {
      global.fatal << "Folder links incorrectly initialized. " << global.fatal;
    }
    std::string & key = links[i][0];
    if (key.size() <= 1) {
      global.fatal
      << "Folder link key: "
      << key
      << " is too short. "
      << global.fatal;
    }
    if (key[0] == '/' || key[0] == '.' || key[0] == ' ') {
      global.fatal
      << "Folder link key "
      << key
      << " starts with a banned character. "
      << global.fatal;
    }
  }
  return true;
}

bool FileOperations::fileExistsVirtualCustomizedReadOnly(
  const std::string& fileName, std::stringstream* commentsOnFailure
) {
  std::string computedFileName;
  if (
    !FileOperations::getPhysicalFileNameFromVirtualCustomizedReadOnly(
      fileName, computedFileName, commentsOnFailure
    )
  ) {
    return false;
  }
  return FileOperations::fileExistsUnsecure(computedFileName);
}

bool FileOperations::fileExistsVirtual(
  const std::string& fileName,
  bool accessSensitiveFolders,
  bool accessULTRASensitiveFolders,
  std::stringstream* commentsOnFailure
) {
  std::string computedFileName;
  if (
    !FileOperations::getPhysicalFileNameFromVirtual(
      fileName,
      computedFileName,
      accessSensitiveFolders,
      accessULTRASensitiveFolders,
      commentsOnFailure
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Failed to extract file name from virtual one. ";
    }
    return false;
  }
  return FileOperations::fileExistsUnsecure(computedFileName);
}

bool FileOperations::fileExistsUnsecure(const std::string& fileName) {
  std::fstream fileStream;
  fileStream.open(fileName.c_str(), std::fstream::in);
  if (fileStream.is_open()) {
    return true;
  } else {
    return false;
  }
}

bool FileOperations::openFileVirtualReadOnly(
  std::ifstream& file,
  const std::string& fileName,
  bool openAsBinary,
  bool accessSensitiveFolders
) {
  std::string computedFileName;
  if (
    !FileOperations::getPhysicalFileNameFromVirtual(
      fileName, computedFileName, accessSensitiveFolders, false, nullptr
    )
  ) {
    return false;
  }
  return
  FileOperations::openFileUnsecureReadOnly(
    file, computedFileName, openAsBinary
  );
}

bool FileOperations::openFileVirtualCustomizedWriteOnly(
  std::fstream& file,
  const std::string& fileName,
  bool OpenInAppendMode,
  bool truncate,
  bool openAsBinary,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("FileOperations::openFileVirtualCustomizedWriteOnly");
  std::string computedFileName;
  if (
    !FileOperations::getPhysicalFileNameFromVirtualCustomizedWriteOnly(
      fileName, computedFileName, commentsOnFailure
    )
  ) {
    return false;
  }
  return
  FileOperations::openFileUnsecure(
    file, computedFileName, OpenInAppendMode, truncate, openAsBinary
  );
}

bool FileOperations::openFileVirtualCustomizedWriteOnlyCreateIfNeeded(
  std::fstream& file,
  const std::string& fileName,
  bool OpenInAppendMode,
  bool truncate,
  bool openAsBinary,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("FileOperations::openFileVirtualCustomizedWriteOnly");
  std::string computedFileName;
  if (
    !FileOperations::getPhysicalFileNameFromVirtualCustomizedWriteOnly(
      fileName, computedFileName, commentsOnFailure
    )
  ) {
    return false;
  }
  return
  FileOperations::openFileCreateIfNotPresentUnsecure(
    file, computedFileName, OpenInAppendMode, truncate, openAsBinary
  );
}

bool FileOperations::openFileVirtualCustomizedReadOnly(
  std::fstream& file,
  const std::string& fileName,
  bool OpenInAppendMode,
  bool truncate,
  bool openAsBinary,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("FileOperations::openFileVirtualCustomizedReadOnly");
  std::string computedFileName;
  if (
    !FileOperations::getPhysicalFileNameFromVirtualCustomizedReadOnly(
      fileName, computedFileName, commentsOnFailure
    )
  ) {
    return false;
  }
  return
  FileOperations::openFileUnsecure(
    file, computedFileName, OpenInAppendMode, truncate, openAsBinary
  );
}

bool FileOperations::openFileVirtual(
  std::fstream& file,
  const std::string& fileName,
  bool OpenInAppendMode,
  bool truncate,
  bool openAsBinary,
  bool accessSensitiveFolders
) {
  std::string computedFileName;
  if (
    !FileOperations::getPhysicalFileNameFromVirtual(
      fileName, computedFileName, accessSensitiveFolders, false, nullptr
    )
  ) {
    return false;
  }
  return
  FileOperations::openFileUnsecure(
    file, computedFileName, OpenInAppendMode, truncate, openAsBinary
  );
}

std::string FileOperations::getWouldBeFolderAfterHypotheticalChdirNonThreadSafe
(const std::string& wouldBePath) {
  StateMaintainerCurrentFolder maintainFolder;
  // TODO: Investigate whether this code is safe.
  global.changeDirectory(wouldBePath);
  std::string result = FileOperations::getCurrentFolder();
  return result;
}

std::string FileOperations::getCurrentFolder() {
  char cwd[100000];
  if (getcwd(cwd, sizeof(cwd)) != nullptr) {
    return std::string(cwd);
  } else {
    global.fatal
    << "Error: getcwd returned NULL. This shouldn't happen. "
    << global.fatal;
  }
  return "";
}

bool FileOperations::openFileUnsecure(
  std::fstream& file,
  const std::string& fileName,
  bool openInAppendMode,
  bool truncate,
  bool openAsBinary
) {
  if (openInAppendMode) {
    if (openAsBinary) {
      file.open(
        fileName.c_str(),
        std::fstream::in | std::fstream::out | std::fstream::app |
        std::fstream::binary
      );
    } else {
      file.open(
        fileName.c_str(),
        std::fstream::in | std::fstream::out | std::fstream::app
      );
    }
  } else {
    if (openAsBinary) {
      file.open(
        fileName.c_str(),
        std::fstream::in | std::fstream::out | std::fstream::binary
      );
    } else {
      if (truncate) {
        file.open(
          fileName.c_str(),
          std::fstream::in | std::fstream::out | std::fstream::trunc
        );
      } else {
        file.open(
          fileName.c_str(), std::fstream::in | std::fstream::out
        );
      }
    }
  }
  return file.is_open();
}

bool FileOperations::openFileUnsecureReadOnly(
  std::ifstream& file, const std::string& fileName, bool openAsBinary
) {
  if (openAsBinary) {
    file.open(
      fileName.c_str(), std::fstream::in | std::fstream::binary
    );
  } else {
    file.open(fileName.c_str(), std::fstream::in);
  }
  return file.is_open();
}

std::string FileOperations::GetVirtualNameWithHash(
  const std::string& inputFileName
) {
  STACK_TRACE("FileOperations::GetVirtualNameWithHash");
  std::string result = inputFileName;
  std::string fileNameEnd;
  for (
    int i = 0; i <
    FileOperations::folderVirtualLinksToWhichWeAppendTimeAndBuildHash().size; i
    ++
  ) {
    const std::string& currentStart =
    FileOperations::folderVirtualLinksToWhichWeAppendTimeAndBuildHash()[i];
    if (
      StringRoutines::stringBeginsWith(result, currentStart, &fileNameEnd)
    ) {
      result = currentStart + global.buildHeadHashWithServerTime + fileNameEnd;
      break;
    }
  }
  return result;
}

bool FileOperations::getPhysicalFileNameFromVirtualCustomizedWriteOnly(
  const std::string& inputFileName,
  std::string& output,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE(
    "FileOperations::getPhysicalFileNameFromVirtualCustomizedWriteOnly"
  );
  std::string fileEnd = "";
  std::string inputStart = "";
  for (
    int i = 0; i <
    FileOperations::folderStartsToWhichWeAppendInstructorUsernameSlash().size;
    i ++
  ) if (
    StringRoutines::stringBeginsWith(
      inputFileName,
      FileOperations::folderStartsToWhichWeAppendInstructorUsernameSlash()[i],
      &fileEnd
    )
  ) {
    inputStart =
    FileOperations::folderStartsToWhichWeAppendInstructorUsernameSlash()[i];
    break;
  }
  if (inputStart == "") {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "File name: "
      << inputFileName
      << " does not correspond to a customizable file. ";
    }
    return false;
  }
  std::string customized =
  HtmlRoutines::convertStringToURLString(
    global.userDefault.instructorComputed, false
  );
  if (customized == "") {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "Customizing files not available for non-logged-in users. ";
    }
    return false;
  }
  std::string inputCopy = inputStart + customized + "/" + fileEnd;
  std::string outputCandidate;
  bool result =
  FileOperations::getPhysicalFileNameFromVirtual(
    inputCopy, outputCandidate, false, false, commentsOnFailure
  );
  if (!FileOperations::fileExistsVirtual(outputCandidate, false, false)) {
    std::string fileContent;
    std::string inputDefault = inputStart + "default/" + fileEnd;
    if (
      FileOperations::loadFileToStringVirtual(
        inputDefault, fileContent, false, commentsOnFailure
      )
    ) {
      std::fstream fileStream;
      if (
        FileOperations::openFileCreateIfNotPresentVirtualCreateFoldersIfNeeded(
          fileStream, inputCopy, false, true, false, false
        )
      ) {
        fileStream << fileContent;
        result = true;
      }
    }
  }
  if (result) {
    output = outputCandidate;
  }
  return result;
}

bool FileOperations::getPhysicalFileNameFromVirtualCustomizedReadOnly(
  const std::string& inputFileName,
  std::string& output,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE(
    "FileOperations::getPhysicalFileNameFromVirtualCustomizedReadOnly"
  );
  std::string fileEnd = "";
  std::string inputStart = "";
  for (
    int i = 0; i <
    FileOperations::folderStartsToWhichWeAppendInstructorUsernameSlash().size;
    i ++
  ) {
    if (
      StringRoutines::stringBeginsWith(
        inputFileName,
        FileOperations::folderStartsToWhichWeAppendInstructorUsernameSlash()[i]
        ,
        &fileEnd
      )
    ) {
      inputStart =
      FileOperations::folderStartsToWhichWeAppendInstructorUsernameSlash()[i];
      break;
    }
  }
  if (inputStart == "") {
    return
    FileOperations::getPhysicalFileNameFromVirtual(
      inputFileName, output, false, false, commentsOnFailure
    );
  }
  std::string customized =
  HtmlRoutines::convertStringToURLString(
    global.userDefault.instructorComputed, false
  ) +
  "/";
  if (customized == "") {
    customized = "default/";
  }
  std::string inputCopy = inputStart + customized + fileEnd;
  if (!fileExistsVirtual(inputCopy, false, false)) {
    customized = "default/";
  }
  inputCopy = inputStart + customized + fileEnd;
  return
  FileOperations::getPhysicalFileNameFromVirtual(
    inputCopy, output, false, false, commentsOnFailure
  );
}

bool FileOperations::getPhysicalFileNameFromVirtual(
  const std::string& inputFileNamE,
  std::string& output,
  bool accessSensitiveFolders,
  bool accessULTRASensitiveFolders,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("FileOperations::getPhysicalFileNameFromVirtual");
  // Using loggers forbidden here: function is used by the loggers themselves.
  if (
    !FileOperations::isOKFileNameVirtual(
      inputFileNamE, accessSensitiveFolders
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "File name: " << inputFileNamE << " not allowed. ";
    }
    return false;
  }
  std::string inputCopy = inputFileNamE;
  std::string folderEnd, folderEnd2;
  for (
    int i = 0; i <
    FileOperations::folderVirtualLinksToWhichWeAppendTimeAndBuildHash().size; i
    ++
  ) {
    if (
      StringRoutines::stringBeginsWith(
        inputCopy,
        FileOperations::folderVirtualLinksToWhichWeAppendTimeAndBuildHash()[i],
        &folderEnd
      )
    ) {
      if (
        StringRoutines::stringBeginsWith(
          folderEnd, global.buildHeadHashWithServerTime, &folderEnd2
        )
      ) {
        inputCopy =
        FileOperations::folderVirtualLinksToWhichWeAppendTimeAndBuildHash()[i]
        +
        folderEnd2;
      }
    }
  }
  for (
    int i = 0; i < FileOperations::folderVirtualLinksNonSensitive().size(); i
    ++
  ) {
    if (
      StringRoutines::stringBeginsWith(
        inputCopy,
        FileOperations::folderVirtualLinksNonSensitive().keys[i],
        &folderEnd
      )
    ) {
      output =
      global.physicalPathProjectBase +
      FileOperations::folderVirtualLinksNonSensitive().values[i] +
      folderEnd;
      return true;
    }
  }
  if (accessSensitiveFolders) {
    for (
      int i = 0; i < FileOperations::folderVirtualLinksSensitive().size(); i ++
    ) {
      if (
        StringRoutines::stringBeginsWith(
          inputCopy,
          FileOperations::folderVirtualLinksSensitive().keys[i],
          &folderEnd
        )
      ) {
        output =
        global.physicalPathProjectBase +
        FileOperations::folderVirtualLinksSensitive().values[i] +
        folderEnd;
        return true;
      }
    }
  }
  if (accessULTRASensitiveFolders) {
    for (
      int i = 0; i < FileOperations::folderVirtualLinksULTRASensitive().size();
      i ++
    ) {
      if (
        StringRoutines::stringBeginsWith(
          inputCopy,
          FileOperations::folderVirtualLinksULTRASensitive().keys[i],
          &folderEnd
        )
      ) {
        output =
        global.physicalPathProjectBase +
        FileOperations::folderVirtualLinksULTRASensitive().values[i] +
        folderEnd;
        return true;
      }
    }
  }
  output = global.physicalPathServerBase + "public_html";
  if (inputCopy.size() > 0) {
    if (inputCopy[0] != '/') {
      output += "/";
    }
  }
  output += inputCopy;
  return true;
}

bool FileOperations::openFileCreateIfNotPresentVirtualCreateFoldersIfNeeded(
  std::fstream& file,
  const std::string& fileName,
  bool openInAppendMode,
  bool truncate,
  bool openAsBinary,
  bool accessSensitiveFolders
) {
  return
  FileOperations::
  openFileCreateIfNotPresentVirtualCreateFoldersIfNeeded_UltraSensitiveOptions(
    file,
    fileName,
    openInAppendMode,
    truncate,
    openAsBinary,
    accessSensitiveFolders
  );
}

bool FileOperations::
openFileCreateIfNotPresentVirtualCreateFoldersIfNeeded_UltraSensitiveOptions(
  std::fstream& file,
  const std::string& fileName,
  bool openInAppendMode,
  bool truncate,
  bool openAsBinary,
  bool accessSensitiveFolders,
  bool accessUltraSensitiveFolders
) {
  std::string computedFileName;
  // USING loggers FORBIDDEN here! Loggers call this function themselves in
  // their constructors.
  if (
    !FileOperations::getPhysicalFileNameFromVirtual(
      fileName,
      computedFileName,
      accessSensitiveFolders,
      accessUltraSensitiveFolders,
      nullptr
    )
  ) {
    return false;
  }
  std::string folderName =
  FileOperations::getPathFromFileNameWithPath(computedFileName);
  std::stringstream mkDirCommand;
  mkDirCommand << "mkdir -p " << folderName;
  global.externalCommandReturnOutput(mkDirCommand.str());
  return
  FileOperations::openFileCreateIfNotPresentUnsecure(
    file, computedFileName, openInAppendMode, truncate, openAsBinary
  );
}

bool FileOperations::openFileCreateIfNotPresentVirtual(
  std::fstream& file,
  const std::string& fileName,
  bool OpenInAppendMode,
  bool truncate,
  bool openAsBinary,
  bool accessSensitiveFolders,
  bool accessUltraSensitiveFolders
) {
  std::string computedFileName;
  // USING loggers FORBIDDEN here! Loggers call this function themselves in
  // their constructors.
  if (
    !FileOperations::getPhysicalFileNameFromVirtual(
      fileName,
      computedFileName,
      accessSensitiveFolders,
      accessUltraSensitiveFolders,
      nullptr
    )
  ) {
    return false;
  }
  return
  FileOperations::openFileCreateIfNotPresentUnsecure(
    file, computedFileName, OpenInAppendMode, truncate, openAsBinary
  );
}

bool FileOperations::openFileCreateIfNotPresentUnsecure(
  std::fstream& file,
  const std::string& fileName,
  bool openInAppendMode,
  bool truncate,
  bool openAsBinary
) {
  // USING loggers FORBIDDEN here! Loggers call this function themselves in
  // their constructors.
  if (openInAppendMode) {
    if (openAsBinary) {
      file.open(
        fileName.c_str(),
        std::fstream::in | std::fstream::out | std::fstream::app |
        std::fstream::binary
      );
    } else {
      file.open(
        fileName.c_str(),
        std::fstream::in | std::fstream::out | std::fstream::app
      );
    }
  } else {
    if (openAsBinary) {
      file.open(
        fileName.c_str(),
        std::fstream::in | std::fstream::out | std::fstream::binary
      );
    } else {
      if (truncate) {
        file.open(
          fileName.c_str(),
          std::fstream::in | std::fstream::out | std::fstream::trunc
        );
      } else {
        file.open(
          fileName.c_str(), std::fstream::in | std::fstream::out
        );
      }
    }
  }
  if (file.is_open()) {
    file.clear();
    file.seekp(0, std::ios_base::end);
    long filePosition = file.tellp();
    if (filePosition > 0) {
      if (!openInAppendMode) {
        file.seekp(0);
        file.seekg(0);
      }
      return true;
    }
  }
  file.close();
  file.open(
    fileName.c_str(),
    std::fstream::out | std::fstream::in | std::fstream::trunc
  );
  file.clear();
  return file.is_open();
}

StateMaintainerCurrentFolder::StateMaintainerCurrentFolder() {
  this->currentFolderPhysicalAbsolute = FileOperations::getCurrentFolder();
}

StateMaintainerCurrentFolder::~StateMaintainerCurrentFolder() {
  global.changeDirectory(this->currentFolderPhysicalAbsolute);
}

void DrawingVariables::drawCoordSystemBuffer(
  DrawingVariables& variables, int dimension
) {
  STACK_TRACE("DrawingVariables::drawCoordSystemBuffer");
  Vector<Rational> root;
  Vector<Rational> zeroRoot;
  zeroRoot.makeZero(dimension);
  std::string colorText = "#64c064";
  for (int i = 0; i < dimension; i ++) {
    root.makeEi(dimension, i);
    std::string tempS;
    tempS = root.toString();
    variables.drawLineBetweenTwoVectorsBufferRational(
      zeroRoot, root, "gray", 1
    );
    variables.drawTextAtVectorBufferRational(root, tempS, "#94c894");
    variables.drawCircleAtVectorBufferRational(root, colorText, 4);
  }
  variables.operations.basisToDrawCirclesAt.makeEiBasis(dimension);
}

void DrawingVariables::drawLineBufferOld(
  double x1,
  double y1,
  double x2,
  double y2,
  uint32_t penStyle,
  int colorIndex,
  std::fstream* latexOutFile
) {
  this->operations.drawLineBuffer(
    x1, y1, x2, y2, penStyle, colorIndex, 1
  );
  if (latexOutFile != nullptr) {
    LaTeXProcedures::drawline(
      x1, y1, x2, y2, penStyle, colorIndex, *latexOutFile, *this
    );
  }
}

void DrawingVariables::drawTextAtVectorBufferRational(
  const Vector<Rational>& point,
  const std::string& inputText,
  const std::string& color
) {
  this->operations.drawTextAtVectorBufferRational(
    point, inputText, color, 10
  );
}

void DrawingVariables::drawTextAtVectorBufferDouble(
  const Vector<double>& point,
  const std::string& inputText,
  int textColor,
  int textStyle
) {
  this->operations.drawTextAtVectorBufferDouble(
    point, inputText, textColor, this->fontSizeNormal, textStyle
  );
}

void DrawingVariables::drawCircleAtVectorBufferRational(
  const Vector<Rational>& point,
  const std::string& color,
  double radius,
  const std::string& frameId,
  int frameIndex
) {
  this->operations.drawCircleAtVectorBufferRational(
    point, color, radius, frameId, frameIndex
  );
}

void DrawingVariables::drawCircleAtVectorBufferDouble(
  const Vector<double>& point, const std::string& color, double radius
) {
  this->operations.drawCircleAtVectorBufferDouble(point, color, radius);
}

void DrawingVariables::drawTextDirectly(
  double x1,
  double y1,
  const std::string& inputText,
  int color,
  std::fstream* latexOutFile
) {
  if (this->drawTextFunction != nullptr) {
    this->drawTextFunction(
      x1 - 7,
      y1 - 7,
      inputText.c_str(),
      static_cast<int>(inputText.length()),
      color,
      this->fontSizeNormal
    );
  }
  if (latexOutFile != nullptr) {
    LaTeXProcedures::drawTextDirectly(
      x1, y1, inputText, color, *latexOutFile
    );
  }
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

bool WeylGroupData::isStronglyPerpendicularTo(
  const Vector<Rational>& input, const Vector<Rational>& other
) {
  return
  !this->rootSystem.contains(input + other) &&
  !this->rootSystem.contains(input - other);
}

bool WeylGroupData::hasStronglyPerpendicularDecompositionWRT(
  Vector<Rational>& input,
  int upperBoundNumBetas,
  Vectors<Rational>& set,
  Vectors<Rational>& output,
  List<Rational>& outputCoeffs,
  bool integralCoefficientsOnly
) {
  if (upperBoundNumBetas > 0 && output.size > upperBoundNumBetas) {
    return false;
  }
  if (input.isEqualToZero()) {
    return true;
  }
  if (set.size == 0) {
    return false;
  }
  if (output.size == 0) {
    if (set.contains(input)) {
      output.setSize(1);
      *output.lastObject() = input;
      outputCoeffs.setSize(1);
      outputCoeffs.lastObject()->makeOne();
      return true;
    }
    output.reserve(set.size);
    outputCoeffs.reserve(set.size);
  }
  Vectors<Rational> newSet;
  newSet.reserve(set.size);
  Vector<Rational> root;
  Rational scalarProduct;
  for (
    int indexFirstNonZeroRoot = 0; indexFirstNonZeroRoot < set.size;
    indexFirstNonZeroRoot ++
  ) {
    Vector<Rational>& currentRoot = set[indexFirstNonZeroRoot];
    scalarProduct = this->rootScalarCartanRoot(input, currentRoot) /
    this->rootScalarCartanRoot(currentRoot, currentRoot);
    if (scalarProduct.isPositive()) {
      if (!integralCoefficientsOnly || scalarProduct.denominatorShort == 1) {
        newSet.size = 0;
        for (int i = indexFirstNonZeroRoot; i < set.size; i ++) {
          if (this->isStronglyPerpendicularTo(currentRoot, set[i])) {
            newSet.addOnTop(set[i]);
          }
        }
        outputCoeffs.addOnTop(scalarProduct);
        output.addOnTop(currentRoot);
        root = input - currentRoot * scalarProduct;
        if (
          this->hasStronglyPerpendicularDecompositionWRT(
            root,
            upperBoundNumBetas,
            newSet,
            output,
            outputCoeffs,
            integralCoefficientsOnly
          )
        ) {
          return true;
        }
        output.size --;
        outputCoeffs.size --;
      }
    }
  }
  return false;
}

char MathRoutines::convertHumanReadableHexToCharValue(char input) {
  if ('0' <= input && input <= '9') {
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

unsigned int MathRoutines::hashListUnsignedChars(
  const List<unsigned char>& input
) {
  unsigned int result = 0;
  int j = 0;
  for (int i = 0; i < input.size; i ++) {
    result +=
    HashConstants::getConstantIncrementCounter(j) * static_cast<unsigned>(
      input[i]
    );
  }
  return result;
}

unsigned int MathRoutines::hashListStrings(
  const List<std::string>& input
) {
  unsigned int result = 0;
  int j = 0;
  for (int i = 0; i < input.size; i ++) {
    result +=
    HashConstants::getConstantIncrementCounter(j) *
    HashFunctions::hashFunction<std::string>(input[i]);
  }
  return result;
}

unsigned int MathRoutines::hashListDoubles(const List<double>& input) {
  unsigned int result = 0;
  int j = 0;
  for (int i = 0; i < input.size; i ++) {
    result +=
    HashConstants::getConstantIncrementCounter(j) *
    MathRoutines::hashDouble(input[i]);
  }
  return result;
}

unsigned int MathRoutines::hashListInts(const List<int>& input) {
  unsigned int result = 0;
  int j = 0;
  for (int i = 0; i < input.size; i ++) {
    result +=
    HashConstants::getConstantIncrementCounter(j) *
    HashFunctions::hashFunction(input[i]);
  }
  return result;
}

unsigned int MathRoutines::hashVectorDoubles(const Vector<double>& input) {
  return MathRoutines::hashListDoubles(input);
}

std::string StringRoutines::Differ::differenceHTMLStatic(
  const std::string& inputLeft,
  const std::string& inputRight,
  const std::string& labelLeft,
  const std::string& labelRight
) {
  StringRoutines::Differ differ;
  differ.left = inputLeft;
  differ.right = inputRight;
  return differ.differenceHTML(labelLeft, labelRight);
}

std::string StringRoutines::Differ::differenceHTML(
  const std::string& labelLeft, const std::string& labelRight
) {
  STACK_TRACE("StringRoutines::Differ::differenceHTML");
  std::stringstream leftOut, rightOut, commentsOnFailure;
  if (!this->computeDifference(&commentsOnFailure)) {
    commentsOnFailure
    << "<b style='color:red'>Failed to compute string difference.</b><br>";
    this->differenceHTMLPartTwo(
      commentsOnFailure.str(),
      labelLeft,
      labelRight,
      this->left,
      this->right
    );
  }
  for (int i = 0; i < this->leftResult.size; i ++) {
    std::string leftS = this->leftResult[i];
    std::string rightS = this->rightResult[i];
    if (leftS == rightS && leftS.size() != 0) {
      if (leftS.size() > 0) {
        leftOut << "<span class='spanStringEqual'>" << leftS << "</span>";
      }
      if (rightS.size() > 0) {
        rightOut << "<span class='spanStringEqual'>" << rightS << "</span>";
      }
    } else {
      if (leftS.size() > 0) {
        leftOut << "<span class='spanStringDifferent'>" << leftS << "</span>";
      }
      if (rightS.size() > 0) {
        rightOut
        << "<span class='spanStringDifferent'>"
        << rightS
        << "</span>";
      }
    }
  }
  return
  this->differenceHTMLPartTwo(
    "", labelLeft, labelRight, leftOut.str(), rightOut.str()
  );
}

std::string StringRoutines::Differ::differenceHTMLPartTwo(
  const std::string& preamble,
  const std::string& labelLeft,
  const std::string& labelRight,
  const std::string& outputLeft,
  const std::string& outputRight
) {
  std::stringstream out;
  out << preamble;
  out
  << "<span class='abstractSyntaxOneAnnotation'><b>"
  << labelLeft
  << "</b><br>";
  out << outputLeft;
  out << "<hr><b>" << labelRight << "</b><br>";
  out << outputRight;
  out << "</span>";
  return out.str();
}

StringRoutines::Differ::Differ() {
  this->maximumMatrixSize = 200000000;
  this->currentCommonStringLength = 0;
}

std::string StringRoutines::Differ::toString() {
  FormatExpressions format;
  format.flagUseHTML = true;
  std::stringstream out;
  out
  << "Matrix:<br>"
  << this->matrixLongestCommonSubsequence.getElement()
  << "<br>"
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

void StringRoutines::Differ::computeBestStartingIndices(
  int& outputIndexLeft, int& outputIndexRight
) {
  STACK_TRACE("StringRoutines::Differ::computeBestStartingIndices");
  Matrix<int> matrix = this->matrixLongestCommonSubsequence.getElement();
  outputIndexLeft = matrix.numberOfRows - 1;
  outputIndexRight = matrix.numberOfColumns - 1;
  int bestSoFar = 0;
  for (int i = 1; i < matrix.numberOfRows; i ++) {
    for (int j = 1; j < matrix.numberOfColumns; j ++) {
      if (
        this->left[static_cast<unsigned>(i - 1)] !=
        this->right[static_cast<unsigned>(j - 1)]
      ) {
        continue;
      }
      int current = matrix(i, j);
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

void StringRoutines::Differ::computeLongestSubsequenceMatrix() {
  STACK_TRACE("StringRoutines::Differ::computeLongestSubsequenceMatrix");
  unsigned numberOfRows = static_cast<unsigned>(left.size()) + 1;
  unsigned numberOfColumns = static_cast<unsigned>(right.size()) + 1;
  Matrix<int>& matrix = this->matrixLongestCommonSubsequence.getElement();
  matrix.initialize(
    static_cast<int>(numberOfRows),
    static_cast<int>(numberOfColumns)
  );
  for (int j = 0; j < matrix.numberOfColumns; j ++) {
    matrix(0, j) = 0;
  }
  for (int i = 0; i < matrix.numberOfRows; i ++) {
    matrix(i, 0) = 0;
  }
  for (unsigned i = 1; i < numberOfRows; i ++) {
    for (unsigned j = 1; j < numberOfColumns; j ++) {
      if (this->left[i - 1] == this->right[j - 1]) {
        matrix(i, j) = matrix(i - 1, j - 1) + 1;
      } else {
        matrix(i, j) =
        MathRoutines::maximum(matrix(i - 1, j), matrix(i, j - 1));
      }
    }
  }
}

void StringRoutines::Differ::pushCommonString(int indexLeft, int indexRight) {
  if (this->currentCommonStringLength == 0) {
    return;
  }
  this->commonStringSizesReverseOrder.addOnTop(
    this->currentCommonStringLength
  );
  this->commonStringsLeftStartsReverseOrder.addOnTop(indexLeft - 1);
  this->commonStringsRightStartsReverseOrder.addOnTop(indexRight - 1);
  this->currentCommonStringLength = 0;
}

void StringRoutines::Differ::extractCommonStrings(
  int indexLeft, int indexRight
) {
  STACK_TRACE("StringRoutines::Differ::extractCommonStrings");
  if (
    this->left.size() == 0 ||
    this->right.size() == 0 ||
    indexLeft < 0 ||
    indexRight < 0
  ) {
    return;
  }
  int previousLeft = - 1;
  int previousRight = - 1;
  while (indexLeft >= 0 && indexRight >= 0) {
    if (indexLeft == 0 || indexRight == 0) {
      this->pushCommonString(previousLeft, previousRight);
      return;
    }
    unsigned leftUnsigned = static_cast<unsigned>(indexLeft - 1);
    unsigned rightUnsigned = static_cast<unsigned>(indexRight - 1);
    if (this->left[leftUnsigned] == this->right[rightUnsigned]) {
      this->currentCommonStringLength ++;
      previousLeft = indexLeft;
      previousRight = indexRight;
      indexLeft --;
      indexRight --;
      continue;
    }
    this->pushCommonString(previousLeft, previousRight);
    Matrix<int>& matrix = this->matrixLongestCommonSubsequence.getElement();
    if (
      matrix(indexLeft - 1, indexRight) > matrix(indexLeft, indexRight - 1)
    ) {
      indexLeft --;
    } else {
      indexRight --;
    }
    previousLeft = indexLeft;
    previousRight = indexRight;
  }
}

void StringRoutines::Differ::extractDifferences() {
  this->extractResult(
    this->commonStringsLeftStartsReverseOrder,
    this->left,
    this->leftResult
  );
  this->extractResult(
    this->commonStringsRightStartsReverseOrder,
    this->right,
    this->rightResult
  );
  while (this->leftResult.size < this->rightResult.size) {
    this->leftResult.addOnTop("");
  }
  while (this->rightResult.size < this->leftResult.size) {
    this->rightResult.addOnTop("");
  }
}

void StringRoutines::Differ::extractResult(
  const List<int>& starts,
  const std::string& input,
  List<std::string>& output
) {
  STACK_TRACE("StringRoutines::Differ::extractResult");
  output.setSize(0);
  unsigned int previousEnd = 0;
  for (
    int i = this->commonStringSizesReverseOrder.size - 1; i >= 0; i --
  ) {
    int stringSize = this->commonStringSizesReverseOrder[i];
    int stringStart = starts[i];
    std::string nonCommon =
    input.substr(
      static_cast<unsigned>(previousEnd),
      static_cast<unsigned>(stringStart) - previousEnd
    );
    output.addOnTop(nonCommon);
    std::string common =
    input.substr(
      static_cast<unsigned>(stringStart),
      static_cast<unsigned>(stringSize)
    );
    previousEnd = static_cast<unsigned>(stringStart + stringSize);
    output.addOnTop(common);
  }
  if (previousEnd < input.size()) {
    output.addOnTop(input.substr(previousEnd));
  }
}

// We compute string difference using the
// longest common subsequence problem.
// https://en.wikipedia.org/wiki/Longest_common_subsequence_problem
bool StringRoutines::Differ::computeDifference(
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("StringRoutines::Differ::computeDifference");
  LargeInteger leftSize;
  // Warning: putting this in the line above may cause ambiguous conversions,
  // depending on your compiler.
  leftSize = left.size();
  // Warning: putting this in the line above may cause ambiguous conversions,
  // depending on your compiler.
  LargeInteger rightSize;
  rightSize = right.size();
  if (leftSize * rightSize > this->maximumMatrixSize) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "The product of the sizes of the two strings equals "
      << leftSize * rightSize
      << " which exceeds the maximum allowed "
      << this->maximumMatrixSize;
    }
    return false;
  }
  this->computeLongestSubsequenceMatrix();
  this->currentCommonStringLength = 0;
  int startLeft = 0;
  int startRight = 0;
  this->computeBestStartingIndices(startLeft, startRight);
  this->extractCommonStrings(startLeft, startRight);
  this->extractDifferences();
  return true;
}

bool StringRoutines::isASCIICharacterVisible(char input) {
  return input >= 32 && input <= 126;
}

std::string StringRoutines::stringShortenInsertDots(
  const std::string& inputString, int maxNumChars
) {
  if (inputString.size() <= static_cast<unsigned>(maxNumChars)) {
    return inputString;
  }
  std::stringstream out;
  int numCharsBeginEnd = maxNumChars / 2 - 2;
  int numCharsOmitted = static_cast<signed>(inputString.size()) -
  numCharsBeginEnd * 2;
  out
  << inputString.substr(0, static_cast<unsigned>(numCharsBeginEnd))
  << "... ("
  << numCharsOmitted
  << " characters omitted)..."
  << inputString.substr(
    static_cast<unsigned>(
      static_cast<signed>(inputString.size()) - numCharsBeginEnd
    )
  );
  return out.str();
}

void StringRoutines::splitExcludeDelimiter(
  const std::string& inputString,
  char delimiter,
  List<std::string>& output
) {
  List<char> tempList;
  tempList.addOnTop(delimiter);
  StringRoutines::splitExcludeDelimiters(inputString, tempList, output);
}

std::string StringRoutines::stringTrimWhiteSpace(
  const std::string& inputString
) {
  std::string result;
  StringRoutines::stringTrimWhiteSpace(inputString, result);
  return result;
}

void StringRoutines::stringTrimToLengthWithHash(
  std::string& inputOutput, int desiredLength50AtLeast
) {
  if (
    static_cast<signed>(inputOutput.size()) <= desiredLength50AtLeast
  ) {
    return;
  }
  if (desiredLength50AtLeast < 40) {
    return;
  }
  std::stringstream inputAbbreviatedStream;
  inputAbbreviatedStream
  << inputOutput.substr(
    0, static_cast<unsigned>(desiredLength50AtLeast - 30)
  )
  << "_abbrev_hash_"
  << HashFunctions::hashFunction<std::string>(inputOutput);
  inputOutput = inputAbbreviatedStream.str();
}

std::string StringRoutines::stringTrimToLengthForDisplay(
  const std::string& input, int desiredLength20AtLeast
) {
  if (desiredLength20AtLeast < 20) {
    desiredLength20AtLeast = 20;
  }
  if (static_cast<signed>(input.size()) <= desiredLength20AtLeast) {
    return input;
  }
  std::stringstream abbreviationStream;
  abbreviationStream << "...(" << input.size() << " characters) ...";
  int numberOfCharactersAtEnds = (
    static_cast<signed>(input.size()) - desiredLength20AtLeast
  ) /
  2;
  std::stringstream out;
  for (int i = 0; i < numberOfCharactersAtEnds; i ++) {
    out << input[static_cast<unsigned>(i)];
  }
  out << abbreviationStream.str();
  for (int i = 0; i < numberOfCharactersAtEnds; i ++) {
    out
    << input[
      static_cast<unsigned>(
        static_cast<signed>(input.size()) - numberOfCharactersAtEnds + i
      )
    ];
  }
  return out.str();
}

void StringRoutines::stringTrimWhiteSpace(
  const std::string& inputString, std::string& output
) {
  // this function needs to be rewritten to do one substr call (no time now).
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

void StringRoutines::stringSplitDefaultDelimiters(
  const std::string& inputString, List<std::string>& output
) {
  STACK_TRACE("StringRoutines::stringSplitDefaultDelimiters");
  List<char> delimiters;
  delimiters.addOnTop(' ');
  delimiters.addOnTop('\r');
  delimiters.addOnTop('\n');
  delimiters.addOnTop('\t');
  delimiters.addOnTop(',');
  delimiters.addOnTop(';');
  delimiters.addOnTop(static_cast<char>(160));
  // <-&nbsp
  StringRoutines::splitExcludeDelimiters(inputString, delimiters, output);
}

void StringRoutines::splitExcludeDelimiters(
  const std::string& inputString,
  const List<char>& delimiters,
  List<std::string>& output
) {
  STACK_TRACE("StringRoutines::splitExcludeDelimiters");
  output.setSize(0);
  std::string reader;
  for (unsigned i = 0; i < inputString.size(); i ++) {
    if (delimiters.contains(inputString[i])) {
      if (reader != "") {
        output.addOnTop(reader);
        reader = "";
      }
    } else {
      reader.push_back(inputString[i]);
    }
  }
  if (reader != "") {
    output.addOnTop(reader);
  }
}

std::string StringRoutines::replaceAll(
  const std::string& input,
  const std::string& subStringToReplace,
  const std::string& replaceWith
) {
  std::stringstream out;
  if (subStringToReplace.size() == 0) {
    global.fatal
    << "Not allowed to replace the empty string. "
    << global.fatal;
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

bool StringRoutines::stringContains(
  const std::string& container, const std::string& content
) {
  return container.find(content) != std::string::npos;
}

bool StringRoutines::replaceOnce(
  std::string& inputOutput,
  const std::string& subStringToReplace,
  const std::string& replaceWith
) {
  auto tagLocation = inputOutput.find(subStringToReplace);
  if (tagLocation == std::string::npos) {
    return false;
  }
  inputOutput.replace(
    tagLocation, subStringToReplace.size(), replaceWith
  );
  return true;
}

void StringRoutines::splitStringInTwo(
  const std::string& inputString,
  int firstStringSize,
  std::string& outputFirst,
  std::string& outputSecond
) {
  if (&outputFirst == &inputString || &outputSecond == &inputString) {
    std::string inputCopy = inputString;
    StringRoutines::splitStringInTwo(
      inputCopy, firstStringSize, outputFirst, outputSecond
    );
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
  outputFirst =
  inputString.substr(0, static_cast<unsigned>(firstStringSize));
  outputSecond = "";
  int secondStringSize = inputSize - firstStringSize;
  if (secondStringSize > 0) {
    outputSecond =
    inputString.substr(
      static_cast<unsigned>(firstStringSize),
      static_cast<unsigned>(secondStringSize)
    );
  }
}

LargeInteger MathRoutines::nChooseK(int n, int k) {
  LargeInteger result;
  MathRoutines::nChooseK(n, k, result);
  return result;
}

void MathRoutines::nChooseK(int n, int k, LargeInteger& result) {
  result = 1;
  for (int i = 0; i < k; i ++) {
    result *= n - i;
    result /= i + 1;
  }
}

double MathRoutines::reducePrecision(double x) {
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

int MathRoutines::factorial(int n) {
  if (n < 0) {
    global.fatal
    <<
    "We do not have factorials of negative integers here. You wanted factorial of: "
    << n
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

bool StringRoutines::stringEndsWith(
  const std::string& input,
  const std::string& desiredEnd,
  std::string* outputStringBeginning
) {
  STACK_TRACE("StringRoutines::stringEndsWith");
  if (desiredEnd.size() == 0) {
    if (outputStringBeginning == nullptr) {
      *outputStringBeginning = input;
    }
    return true;
  }
  int indexInString = static_cast<signed>(input.size()) - 1;
  for (
    int i = static_cast<signed>(desiredEnd.size()) - 1; i >= 0; i --
  ) {
    if (indexInString < 0) {
      return false;
    }
    if (
      desiredEnd[static_cast<unsigned>(i)] !=
      input[static_cast<unsigned>(indexInString)]
    ) {
      return false;
    }
    indexInString --;
  }
  if (outputStringBeginning != nullptr) {
    *outputStringBeginning =
    input.substr(0, input.size() - desiredEnd.size());
  }
  return true;
}

bool StringRoutines::stringBeginsWith(
  const std::string& input,
  const std::string& desiredBeginning,
  std::string* outputStringEnd
) {
  std::string actualBeginning, stringEnd;
  StringRoutines::splitStringInTwo(
    input,
    static_cast<int>(desiredBeginning.size()),
    actualBeginning,
    stringEnd
  );
  bool result = (actualBeginning == desiredBeginning);
  // outputstring is only modified if result is true
  if (result && outputStringEnd != nullptr) {
    *outputStringEnd = stringEnd;
  }
  return result;
}

bool MathRoutines::isLatinLetter(char input) {
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
    if (!MathRoutines::isDigit(input[i])) {
      return false;
    }
  }
  return true;
}

bool MathRoutines::isDigit(char input, int* whichDigit) {
  int digit = input - '0';
  bool result = (digit < 10 && digit >= 0);
  if (result && whichDigit != nullptr) {
    *whichDigit = digit;
  }
  return result;
}

bool MathRoutines::isHexDigit(char digitCandidate) {
  if (MathRoutines::isDigit(digitCandidate)) {
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

bool MathRoutines::isDigit(const std::string& input, int* whichDigit) {
  if (input.size() != 1) {
    return false;
  }
  return MathRoutines::isDigit(input[0], whichDigit);
}

int MathRoutines::leastCommonMultiple(int a, int b) {
  if (a < 0) {
    a = - a;
  }
  if (b < 0) {
    b = - b;
  }
  return ((a* b) / Rational::greatestCommonDivisorSigned(a, b));
}

int MathRoutines::twoToTheNth(int n) {
  int result = 1;
  for (int i = 0; i < n; i ++) {
    result *= 2;
  }
  return result;
}

int MathRoutines::kToTheNth(int k, int n) {
  int result = 1;
  for (int i = 0; i < n; i ++) {
    result *= k;
  }
  return result;
}

void MathRoutines::kToTheNth(int k, int n, LargeInteger& output) {
  output = 1;
  for (int i = 0; i < n; i ++) {
    output *= k;
  }
}

bool MathRoutines::isPrimeSimple(int input) {
  if (input <= 1) {
    return false;
  }
  for (int i = 2; i * i <= input; i += 2) {
    if (input % i == 0) {
      return false;
    }
  }
  return true;
}

Vector<double> MathRoutines::getVectorDouble(Vector<Rational>& input) {
  Vector<double> result;
  result.setSize(input.size);
  for (int i = 0; i < input.size; i ++) {
    result[i] = input[i].getDoubleValue();
  }
  return result;
}

int MathRoutines::binomialCoefficientMultivariate(
  int n, List<int>& choices
) {
  int choiceIndex = 0;
  int denominator = 1;
  int result = 0;
  for (int i = n; i > 0; i --) {
    result *= i;
    result /= denominator;
    denominator ++;
    if (denominator > choices[choiceIndex]) {
      denominator = 1;
      choiceIndex ++;
    }
  }
  return result;
}

void Selection::initialize(int inputNumberOfElements) {
  this->selected.initializeFillInObject(inputNumberOfElements, false);
  this->elements.initializeFillInObject(inputNumberOfElements, 0);
  this->numberOfElements = inputNumberOfElements;
  this->cardinalitySelection = 0;
}

void Selection::addSelectionAppendNewIndex(int index) {
  if (index >= this->numberOfElements || index < 0) {
    return;
  }
  if (this->selected[index]) {
    return;
  }
  this->selected[index] = true;
  this->elements[this->cardinalitySelection] = index;
  this->cardinalitySelection ++;
}

Selection::Selection() {
  this->numberOfElements = - 1;
  this->cardinalitySelection = 0;
}

Selection::Selection(int m) {
  this->cardinalitySelection = 0;
  this->initialize(m);
}

void Selection::removeLastSelection() {
  if (this->cardinalitySelection == 0) {
    return;
  }
  this->selected[this->elements[this->cardinalitySelection - 1]] = false;
  this->cardinalitySelection --;
}

int Selection::selectionToIndex() {
  int result = 0;
  for (int i = 0; i < this->numberOfElements; i ++) {
    result *= 2;
    if (this->selected[i]) {
      result += 1;
    }
  }
  return result;
}

void Selection::shrinkMaxSize() {
  this->numberOfElements --;
  if (this->selected[this->numberOfElements]) {
    this->cardinalitySelection --;
  }
}

void Selection::expandMaxSize() {
  this->elements[this->cardinalitySelection] = this->numberOfElements;
  this->selected[this->numberOfElements] = true;
  this->numberOfElements ++;
  this->cardinalitySelection ++;
}

std::string Selection::toString() const {
  Vector<Rational> root;
  root = *this;
  return root.toString();
}

void Selection::incrementSelection() {
  for (int i = this->numberOfElements - 1; i >= 0; i --) {
    this->selected[i] = !this->selected[i];
    if (this->selected[i]) {
      this->computeIndicesFromSelection();
      return;
    }
  }
  this->computeIndicesFromSelection();
}

void Selection::initSelectionFixedCardinality(int card) {
  this->initialize(this->numberOfElements);
  for (int i = 0; i < card; i ++) {
    this->selected[i] = true;
    this->elements[i] = i;
  }
  this->cardinalitySelection = card;
}

void Selection::incrementSelectionFixedCardinality(int cardinality) {
  // Example of the order of generation of all combinations
  // when cardinality = 2 and maximumSize = 5. The second column indicates the
  // state of the array at the point in code marked with *** below.
  // 11000 (10000) indexLastZeroWithOneBefore: 2
  // numberOfOnesAfterLastZeroWithOneBefore: 0
  // 10100 (10000) indexLastZeroWithOneBefore: 3
  // numberOfOnesAfterLastZeroWithOneBefore: 0
  // 10010 (10000) indexLastZeroWithOneBefore: 4
  // numberOfOnesAfterLastZeroWithOneBefore: 0
  // 10001 (00000) indexLastZeroWithOneBefore: 1
  // numberOfOnesAfterLastZeroWithOneBefore: 1
  // 01100 (01000) indexLastZeroWithOneBefore: 3
  // numberOfOnesAfterLastZeroWithOneBefore: 0
  // 01010 (01000) indexLastZeroWithOneBefore: 4
  // numberOfOnesAfterLastZeroWithOneBefore: 0
  // 01001 (00000) indexLastZeroWithOneBefore: 2
  // numberOfOnesAfterLastZeroWithOneBefore: 1
  // 00110 (00100) indexLastZeroWithOneBefore: 4
  // numberOfOnesAfterLastZeroWithOneBefore: 0
  // 00101 (00000) indexLastZeroWithOneBefore: 3
  // numberOfOnesAfterLastZeroWithOneBefore: 1
  // 00011
  if (cardinality > this->numberOfElements) {
    return;
  }
  if (this->cardinalitySelection != cardinality) {
    this->initSelectionFixedCardinality(cardinality);
    return;
  }
  if (cardinality == this->numberOfElements || cardinality == 0) {
    return;
  }
  int indexLastZeroWithOneBefore = - 1;
  int numberOfOnesAfterLastZeroWithOneBefore = 0;
  for (int i = this->numberOfElements - 1; i >= 0; i --) {
    if (this->selected[i]) {
      if (indexLastZeroWithOneBefore == - 1) {
        numberOfOnesAfterLastZeroWithOneBefore ++;
      } else {
        break;
      }
    } else {
      indexLastZeroWithOneBefore = i;
    }
  }
  if (indexLastZeroWithOneBefore == 0) {
    this->initSelectionFixedCardinality(cardinality);
    return;
  }
  for (int i = 0; i < numberOfOnesAfterLastZeroWithOneBefore + 1; i ++) {
    this->selected[this->elements[this->cardinalitySelection - i - 1]] = false;
  }
  // ***At this point in time the second column is recorded.
  for (int i = 0; i < numberOfOnesAfterLastZeroWithOneBefore + 1; i ++) {
    this->selected[i + indexLastZeroWithOneBefore] = true;
    this->elements[
      this->cardinalitySelection +
      i - numberOfOnesAfterLastZeroWithOneBefore - 1
    ] =
    i + indexLastZeroWithOneBefore;
  }
}

void Selection::computeIndicesFromSelection() {
  this->cardinalitySelection = 0;
  for (int i = 0; i < this->numberOfElements; i ++) {
    if (this->selected[i]) {
      this->elements[cardinalitySelection] = i;
      this->cardinalitySelection ++;
    }
  }
}

void Selection::makeSubSelection(
  Selection& selection, Selection& subSelection
) {
  this->initialize(selection.numberOfElements);
  for (int i = 0; i < subSelection.cardinalitySelection; i ++) {
    this->addSelectionAppendNewIndex(
      selection.elements[subSelection.elements[i]]
    );
  }
}

void Selection::operator=(const Selection& right) {
  this->elements = right.elements;
  this->cardinalitySelection = right.cardinalitySelection;
  this->numberOfElements = right.numberOfElements;
  this->selected = right.selected;
}

unsigned int Selection::hashFunction() const {
  unsigned int result = 0;
  int j = 0;
  for (int i = 0; i < this->selected.size; i ++, j ++) {
    if (this->selected[i]) {
      result += static_cast<unsigned>(i) *
      HashConstants::getConstantIncrementCounter(j);
    }
  }
  return result;
}

template < >
unsigned int HashFunctions::hashFunction(const Selection& input) {
  return input.hashFunction();
}

// Format expression monomial orders for the toString() function follow.
template < >
List<
  MonomialWrapper<
    std::string, HashFunctions::hashFunction<std::string>
  >
>::Comparator* FormatExpressions::getMonomialOrder<
  MonomialWrapper<
    std::string, HashFunctions::hashFunction<std::string>
  >
>() {
  return nullptr;
}

template < >
List<MonomialPolynomial>::Comparator* FormatExpressions::getMonomialOrder<
  MonomialPolynomial
>() {
  return &this->monomialOrder;
}

template < >
List<Polynomial<AlgebraicNumber> >::Comparator* FormatExpressions::
getMonomialOrder<Polynomial<AlgebraicNumber> >() {
  return nullptr;
}

template < >
List<MonomialVector>::Comparator* FormatExpressions::getMonomialOrder<
  MonomialVector
>() {
  return nullptr;
}

template < >
List<MonomialWeylAlgebra>::Comparator* FormatExpressions::getMonomialOrder<
  MonomialWeylAlgebra
>() {
  return nullptr;
}

template < >
List<MonomialUniversalEnveloping<RationalFraction<Rational> > >::Comparator*
FormatExpressions::getMonomialOrder<
  MonomialUniversalEnveloping<RationalFraction<Rational> >
>() {
  return nullptr;
}

template < >
List<MonomialGeneralizedVerma<RationalFraction<Rational> > >::Comparator*
FormatExpressions::getMonomialOrder<
  MonomialGeneralizedVerma<RationalFraction<Rational> >
>() {
  return nullptr;
}

template < >
List<ChevalleyGenerator>::Comparator* FormatExpressions::getMonomialOrder<
  ChevalleyGenerator
>() {
  return nullptr;
}

template < >
List<MonomialMatrix>::Comparator* FormatExpressions::getMonomialOrder<
  MonomialMatrix
>() {
  return nullptr;
}

template < >
List<MonomialUniversalEnveloping<Rational> >::Comparator* FormatExpressions::
getMonomialOrder<MonomialUniversalEnveloping<Rational> >() {
  return nullptr;
}

template < >
List<MonomialTensorGeneralizedVermas<RationalFraction<Rational> > >::Comparator
* FormatExpressions::getMonomialOrder<
  MonomialTensorGeneralizedVermas<RationalFraction<Rational> >
>() {
  return nullptr;
}

template < >
List<QuasiDifferentialMononomial>::Comparator* FormatExpressions::
getMonomialOrder<QuasiDifferentialMononomial>() {
  return nullptr;
}

template < >
List<MonomialUniversalEnveloping<Polynomial<Rational> > >::Comparator*
FormatExpressions::getMonomialOrder<
  MonomialUniversalEnveloping<Polynomial<Rational> >
>() {
  return nullptr;
}

void FormatExpressions::makeAlphabetXYZUW() {
  if (this->polynomialAlphabet.size > 0) {
    return;
  }
  this->polynomialAlphabet.addOnTop("x");
  this->polynomialAlphabet.addOnTop("y");
  this->polynomialAlphabet.addOnTop("z");
  this->polynomialAlphabet.addOnTop("u");
  this->polynomialAlphabet.addOnTop("w");
}

FormatExpressions* FormatExpressions::defaultFormat() {
  static FormatExpressions result;
  if (result.polynomialAlphabet.size == 0) {
    result.polynomialAlphabet.addOnTop("x");
  }
  return &result;
}

FormatExpressions::FormatExpressions() {
  this->ambientWeylLetter = 'X';
  this->extraLinesCounterLatex = 0;
  this->chevalleyGGeneratorLetter = "g";
  this->chevalleyHGeneratorLetter = "h";
  this->polynomialDefaultLetter = "x";
  this->weylAlgebraDefaultLetter = "\\partial";
  this->finiteDimensionalRepresentationLetter = "V";
  this->simpleRootLetter = "\\eta";
  this->maximumLineLength = 100;
  this->flagPassCustomCoefficientMonomialSeparatorToCoefficients = false;
  this->flagUseCalculatorFormatForUEOrdered = true;
  this->flagUseHTML = false;
  this->flagUseLatex = false;
  this->flagUsePNG = false;
  this->flagUsePmatrix = true;
  this->matrixColumnVerticalLineIndex = - 1;
  this->flagQuasiDiffOpCombineWeylPart = true;
  this->flagMakingExpressionTableWithLatex = false;
  this->numberOfAmpersandsPerNewLineForLaTeX = 0;
  this->monomialOrder = nullptr;
  this->flagExpressionIsFinal = true;
  this->flagExpressionNewLineAllowed = false;
  this->flagIncludeLieAlgebraTypes = true;
  this->flagUseReflectionNotation = false;
  this->flagIncludeMutableInformation = true;
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
  this->flagSuppressModP = false;
  this->maximumMatrixDisplayedRows = 20;
  this->maximumMatrixLineLength = 20;
  this->monomialOrder.leftGreaterThanRight =
  MonomialPolynomial::orderDefault().leftGreaterThanRight;
}

std::string FormatExpressions::getPolynomialLetter(int index) const {
  if (index < this->polynomialAlphabet.size) {
    return this->polynomialAlphabet[index];
  }
  std::stringstream out;
  out << this->polynomialDefaultLetter << "_{" << index + 1 << "}";
  std::string tempS = out.str();
  return tempS;
}

bool OnePartialFractionDenominator::reduceOnceTotalOrderMethod(
  LinearCombination<
    OnePartialFractionDenominator, Polynomial<LargeInteger>
  >& output
) {
  for (int i = 0; i < this->denominatorsNoScale.size(); i ++) {
    for (int j = 0; j < this->denominatorsNoScale.size(); j ++) {
      int aIndex = this->denominatorsNoScale.values[i].normalizedVectorIndex;
      int bIndex = this->denominatorsNoScale.values[j].normalizedVectorIndex;
      int aMinusBIndex =
      this->owner->tableAllowedAminusB.elements[aIndex][bIndex];
      int aMinus2BIndex =
      this->owner->tableAllowedAminus2B.elements[aIndex][bIndex];
      if (aMinusBIndex != - 1 && aMinusBIndex > bIndex) {
        this->decomposeAMinusNB(
          aIndex, bIndex, 1, aMinusBIndex, output, *this->owner
        );
        return true;
      }
      if (aMinus2BIndex != - 1 && aMinus2BIndex > bIndex) {
        this->decomposeAMinusNB(
          aIndex, bIndex, 2, aMinus2BIndex, output, *this->owner
        );
        return true;
      }
    }
  }
  return false;
}

bool OnePartialFractionDenominator::checkInitialization() const {
  if (this->owner == nullptr) {
    global.fatal
    << "Uninitialized partial fraction denominator. "
    << global.fatal;
  }
  return true;
}

bool OnePartialFractionDenominator::reduceOnce(
  LinearCombination<
    OnePartialFractionDenominator, Polynomial<LargeInteger>
  >& output
) {
  STACK_TRACE("OnePartialFractionDenominator::reduceOnce");
  this->checkInitialization();
  if (
    this->denominatorsNoScale.size() <= this->owner->ambientDimension
  ) {
    return false;
  }
  Vectors<Rational> allRootsSorted;
  this->getNormalizedSortedDenominatorExponents(allRootsSorted);
  Vector<Rational> linearDependence;
  if (!allRootsSorted.getLinearDependenceLexicographic(linearDependence)) {
    // This must not happen: we have more vectors than the dimension
    // of the ambient vector space, so we must have a linear relation.
    // We have made a fatal programming error.
    global.fatal
    << "Unexpected failure to find linear dependence."
    << global.fatal;
  }
  this->decomposeFromNormalizedLinearRelation(
    linearDependence, allRootsSorted, output
  );
  return true;
}

bool OnePartialFractionDenominator::operator>(
  const OnePartialFractionDenominator& other
) const {
  if (this->denominatorsNoScale.keys > other.denominatorsNoScale.keys) {
    return true;
  }
  if (other.denominatorsNoScale.keys > this->denominatorsNoScale.keys) {
    return false;
  }
  return this->denominatorsNoScale.values > other.denominatorsNoScale.values;
}

void OnePartialFractionDenominator::computeOneCheckSum(
  Vector<Rational>& variableValues, Rational& output
) const {
  STACK_TRACE("OnePartialFraction::computeOneCheckSum");
  Rational multiplicand;
  output = 1;
  for (int i = 0; i < this->denominatorsNoScale.size(); i ++) {
    this->denominatorsNoScale.values[i].computeOneCheckSum(
      multiplicand, variableValues
    );
    output.multiplyBy(multiplicand);
  }
  output.invert();
}

std::string OnePartialFractionDenominator::toLatex(
  const Polynomial<LargeInteger>& numerator, FormatExpressions* format
) const {
  std::stringstream out;
  out
  << "\\frac{"
  << numerator.toString(format)
  << "}{"
  << this->toStringDenominatorOnly(format)
  << "}";
  return out.str();
}

std::string OnePartialFractionDenominator::toString(FormatExpressions* format)
const {
  if (this->denominatorsNoScale.size() == 0) {
    return "1";
  }
  MemorySaving<FormatExpressions> backupFormat;
  if (format == nullptr) {
    format = &backupFormat.getElement();
    format->makeAlphabetXYZUW();
  }
  std::stringstream out;
  out << "/(";
  out << this->toStringDenominatorOnly(format);
  out << ")";
  return out.str();
}

std::string OnePartialFractionDenominator::toStringDenominatorOnly(
  FormatExpressions* format
) const {
  std::stringstream out;
  MapList<Vector<Rational>, OnePartialFractionDenominatorComponent>
  summandsSorted;
  this->getDenominatorsSorted(summandsSorted);
  for (int i = 0; i < summandsSorted.size(); i ++) {
    const OnePartialFractionDenominatorComponent& currentDenominator =
    this->denominatorsNoScale.values[i];
    std::string current = currentDenominator.toString(format);
    if (current != "1") {
      out << current;
    }
  }
  return out.str();
}

bool OnePartialFractionDenominator::rootIsInFractionCone(
  Vector<Rational>* root
) const {
  if (root == nullptr) {
    return true;
  }
  Cone cone;
  Vectors<Rational> roots;
  for (int i = 0; i < this->denominatorsNoScale.size(); i ++) {
    roots.addOnTop(
      this->denominatorsNoScale.values[i].getNormalizedVector()
    );
  }
  cone.createFromVertices(roots);
  return cone.isInCone(*root);
}

void OnePartialFractionDenominator::prepareFraction(
  int indexA,
  int indexB,
  int aMinusNBIndex,
  bool indexAIsNullified,
  OnePartialFractionDenominator& output,
  Polynomial<LargeInteger>& aMinusNBetaPolynomial,
  Polynomial<LargeInteger>& outputCommonCoefficient
) {
  output = *this;
  int powerDropA = this->denominatorsNoScale.values[indexA].multiplicities[0];
  int powerDropB = this->denominatorsNoScale.values[indexB].multiplicities[0];
  if (indexAIsNullified) {
    powerDropB = 0;
  } else {
    powerDropA = 0;
  }
  outputCommonCoefficient.makeOne();
  for (int i = 0; i < powerDropB; i ++) {
    outputCommonCoefficient *= aMinusNBetaPolynomial;
  }
  output.decreasePowerOneFraction(indexA, powerDropA);
  output.decreasePowerOneFraction(indexB, powerDropB);
  output.decreasePowerOneFraction(aMinusNBIndex, - powerDropA - powerDropB);
}

int OnePartialFractionDenominator::
getNumberProportionalVectorsClassicalRootSystems(PartialFractions& owner) {
  int result = 0;
  for (
    int i = 0; i < owner.indicesRedundantShortRoots.cardinalitySelection; i ++
  ) {
    int index = owner.indicesRedundantShortRoots.elements[i];
    int rootIndex =
    owner.getIndexDoubleOfARoot(owner.normalizedVectors[index]);
    if (rootIndex != - 1) {
      if (
        this->denominatorsNoScale.values[rootIndex].multiplicities.size > 0
      ) {
        result ++;
      }
    }
  }
  return result;
}

int OnePartialFractionDenominator::
computeGainingMultiplicityIndexInLinearRelation(
  Vector<Rational>& linearRelation
) {
  STACK_TRACE(
    "OnePartialFractionDenominator::computeGainingMultiplicityIndexInLinearRelation"
  );
  for (int i = 0; i < linearRelation.size; i ++) {
    if (linearRelation[i].isEqualToZero()) {
      continue;
    }
    return i;
  }
  return - 1;
}

void OnePartialFractionDenominator::getLinearRelationFromNormalized(
  int gainingMultiplicityIndex,
  List<Vector<Rational> >& normalizedVectors,
  Vector<Rational>& linearRelationBetweenNormalizedVectors,
  Vector<Rational>& output
) {
  STACK_TRACE(
    "OnePartialFractionDenominator::getLinearRelationFromNormalized"
  );
  output = linearRelationBetweenNormalizedVectors;
  for (int i = 0; i < output.size; i ++) {
    if (output[i].isEqualToZero()) {
      continue;
    }
    if (i == gainingMultiplicityIndex) {
      continue;
    }
    output[i] /=
    this->denominatorsNoScale.getValueNoFail(normalizedVectors[i]).
    getLargestElongation();
  }
}

bool OnePartialFractionDenominator::decomposeFromNormalizedLinearRelation(
  Vector<Rational>& linearRelationBetweenNormalizedVectors,
  Vectors<Rational>& normalizedVectors,
  LinearCombination<
    OnePartialFractionDenominator, Polynomial<LargeInteger>
  >& output
) {
  STACK_TRACE(
    "OnePartialFractionDenominator::decomposeFromNormalizedLinearRelation"
  );
  MapList<Vector<Rational>, OnePartialFractionDenominatorComponent> reduced;
  this->getDenominatorsSorted(reduced);
  int gainingMultiplicityIndexInLinearRelation =
  this->computeGainingMultiplicityIndexInLinearRelation(
    linearRelationBetweenNormalizedVectors
  );
  Vector<Rational> gainingExponent =
  normalizedVectors[gainingMultiplicityIndexInLinearRelation];
  for (int i = 0; i < linearRelationBetweenNormalizedVectors.size; i ++) {
    const OnePartialFractionDenominatorComponent& beingReduced =
    this->denominatorsNoScale.getValueNoFail(normalizedVectors[i]);
    reduced.setKeyValue(normalizedVectors[i], beingReduced);
  }
  Vector<Rational> linearRelation;
  this->getLinearRelationFromNormalized(
    gainingMultiplicityIndexInLinearRelation,
    normalizedVectors,
    linearRelationBetweenNormalizedVectors,
    linearRelation
  );
  Rational::scaleNormalizeIndex(
    linearRelation, gainingMultiplicityIndexInLinearRelation
  );
  int elongationGainingMultiplicity = 0;
  if (
    !linearRelation[gainingMultiplicityIndexInLinearRelation].isSmallInteger(
      &elongationGainingMultiplicity
    )
  ) {
    global.fatal << "Elongation is too large. " << global.fatal;
  }
  this->applyGeneralizedSzenesVergneFormula(
    reduced,
    linearRelation,
    gainingExponent,
    elongationGainingMultiplicity,
    output
  );
  return true;
}

void OnePartialFractionDenominator::applyGeneralizedSzenesVergneFormula(
  MapList<Vector<Rational>, OnePartialFractionDenominatorComponent>&
  toBeReduced,
  const Vector<Rational>& linearRelation,
  const Vector<Rational>& gainingNormalizedExponent,
  int elongationGainingMultiplicityIndex,
  LinearCombination<
    OnePartialFractionDenominator, Polynomial<LargeInteger>
  >& output
) const {
  STACK_TRACE(
    "OnePartialFractionDenominator::applyGeneralizedSzenesVergneFormula"
  );
  Polynomial<LargeInteger> geometricSeriesNumerator;
  Polynomial<LargeInteger> coefficientBuffer;
  MonomialPolynomial monomial;
  output.makeZero();
  OnePartialFractionDenominator currentFraction;
  int gainingIndex = toBeReduced.keys.getIndex(gainingNormalizedExponent);
  MonomialPolynomial cummulativeExponent;
  cummulativeExponent.makeOne();
  for (int i = 0; i < toBeReduced.size(); i ++) {
    if (gainingIndex == i) {
      continue;
    }
    currentFraction = *this;
    currentFraction.addMultiplicity(
      gainingNormalizedExponent, 1, elongationGainingMultiplicityIndex
    );
    const OnePartialFractionDenominatorComponent& currentDenominator =
    toBeReduced.values[i];
    int removedElongation = currentDenominator.getLargestElongation();
    currentFraction.addMultiplicity(
      toBeReduced.keys[i], - 1, removedElongation
    );
    Vector<Rational> nextExponent;
    currentDenominator.getLongestVector(nextExponent);
    coefficientBuffer.makeZero();
    coefficientBuffer.addMonomial(cummulativeExponent, 1);
    int currentCoefficient = 0;
    if (!linearRelation[i].isSmallInteger(&currentCoefficient)) {
      global.fatal
      << "Linear combination with non-small integers."
      << global.fatal;
    }
    currentCoefficient *= - 1;
    this->getNElongationPolynomial(
      currentCoefficient, nextExponent, geometricSeriesNumerator
    );
    coefficientBuffer *= geometricSeriesNumerator;
    output.addMonomial(currentFraction, coefficientBuffer);
    cummulativeExponent.multiplyBy(
      MonomialPolynomial(nextExponent* currentCoefficient)
    );
  }
}

void OnePartialFractionDenominator::decomposeAMinusNB(
  int indexA,
  int indexB,
  int n,
  int indexAMinusNB,
  LinearCombination<
    OnePartialFractionDenominator, Polynomial<LargeInteger>
  >& output,
  PartialFractions& owner
) {
  OnePartialFractionDenominator currentFraction;
  Polynomial<LargeInteger> aMinusNBetaPolynomial, commonPolynomial;
  this->getAlphaMinusNBetaPoly(
    owner, indexA, indexB, n, aMinusNBetaPolynomial
  );
  int powerA = this->denominatorsNoScale.values[indexA].multiplicities[0];
  int powerB = this->denominatorsNoScale.values[indexB].multiplicities[0];
  output.makeZero();
  this->prepareFraction(
    indexA,
    indexB,
    indexAMinusNB,
    true,
    currentFraction,
    aMinusNBetaPolynomial,
    commonPolynomial
  );
  for (int i = powerB; i >= 1; i --) {
    LargeInteger binomialCoefficient =
    MathRoutines::nChooseK(powerA + powerB - i - 1, powerA - 1);
    commonPolynomial *= binomialCoefficient;
    output.addMonomial(currentFraction, commonPolynomial);
    commonPolynomial /= binomialCoefficient;
    if (i > 1) {
      currentFraction.denominatorsNoScale.values[indexAMinusNB].addMultiplicity
      (1, 1);
      currentFraction.denominatorsNoScale.values[indexB].addMultiplicity(
        - 1, 1
      );
      commonPolynomial *= aMinusNBetaPolynomial;
    }
  }
  this->prepareFraction(
    indexA,
    indexB,
    indexAMinusNB,
    false,
    currentFraction,
    aMinusNBetaPolynomial,
    commonPolynomial
  );
  for (int i = powerA; i >= 1; i --) {
    LargeInteger coefficient =
    MathRoutines::nChooseK(powerA + powerB - i - 1, powerB - 1);
    commonPolynomial *= coefficient;
    output.addMonomial(currentFraction, commonPolynomial);
    commonPolynomial /= coefficient;
    if (i > 1) {
      currentFraction.addMultiplicity(
        currentFraction.denominatorsNoScale.keys[indexAMinusNB], 1, 1
      );
      currentFraction.addMultiplicity(
        currentFraction.denominatorsNoScale.keys[indexA], - 1, 1
      );
    }
  }
}

bool OnePartialFractionDenominator::decreasePowerOneFraction(
  int index, int increment
) {
  this->addMultiplicity(
    this->denominatorsNoScale.keys[index], - increment, 1
  );
  return true;
}

void OnePartialFractionDenominator::getAlphaMinusNBetaPoly(
  PartialFractions& owner,
  int indexA,
  int indexB,
  int n,
  Polynomial<LargeInteger>& output
) {
  output.makeZero();
  MonomialPolynomial monomial;
  monomial.makeOne();
  for (int i = 0; i < n; i ++) {
    for (int j = 0; j < owner.ambientDimension; j ++) {
      monomial.setVariable(
        j,
        owner.normalizedVectors[indexA][j] -
        owner.normalizedVectors[indexB][j] *(i + 1)
      );
    }
    output.addMonomial(monomial, - 1);
  }
}

void OnePartialFractionDenominator::getNElongationPolynomial(
  int lengthOfGeometricSeries,
  const Vector<Rational>& exponent,
  Polynomial<LargeInteger>& output
) {
  STACK_TRACE("OnePartialFractionDenominator::getNElongationPolynomial");
  int start = 0;
  int increment = 1;
  int count = lengthOfGeometricSeries;
  if (lengthOfGeometricSeries < 0) {
    start = - 1;
    increment = - 1;
    count = - lengthOfGeometricSeries;
  }
  int coefficient = start;
  output.makeZero();
  MonomialPolynomial monomial;
  for (int i = 0; i < count; i ++) {
    monomial.makeFromPowers(exponent* coefficient);
    output.addMonomial(monomial, increment);
    coefficient += increment;
  }
}

void OnePartialFractionDenominator::makePolynomialFromOneNormal(
  Vector<Rational>& normal,
  const MonomialPolynomial& shiftRational,
  int multiplicities,
  Polynomial<Rational>& output
) {
  STACK_TRACE("OnePartialFractionDenominator::makePolynomialFromOneNormal");
  output.makeOne();
  if (multiplicities == 1) {
    return;
  }
  Vector<Rational> shiftRationalVector;
  shiftRationalVector.makeZero(normal.size);
  for (int i = 0; i < normal.size; i ++) {
    shiftRationalVector[i] = shiftRational(i);
  }
  Rational scalarProduct;
  scalarProduct = normal.scalarEuclidean(shiftRationalVector);
  Rational constantTerm;
  Polynomial<Rational> nextMultiplicand;
  // Example 1: 1/(1-x^2)^2.
  // multiplicities should be equal to 2.
  for (int j = 0; j < multiplicities - 1; j ++) {
    nextMultiplicand.makeLinearNoConstant(normal);
    constantTerm.assignNumeratorAndDenominator(- 1, j + 1);
    constantTerm.multiplyBy(scalarProduct);
    constantTerm += 1;
    Rational scalar;
    scalar.assignNumeratorAndDenominator(1, j + 1);
    nextMultiplicand *= scalar;
    nextMultiplicand.addConstant(constantTerm);
    // Example 1: nextMultiplicand should equal:
    // 1/2x
    output *= nextMultiplicand;
  }
}

void OnePartialFractionDenominator::computeNormals(
  PartialFractions& owner,
  Vectors<Rational>& output,
  int dimension,
  Matrix<Rational>& buffer
) {
  Vectors<Rational> denominators;
  Vector<Rational> root;
  root.setSize(dimension);
  denominators.size = 0;
  output.size = 0;
  for (int i = 0; i < dimension; i ++) {
    root = owner.normalizedVectors[i];
    denominators.addOnTop(root);
  }
  Rational scalarProduct;
  for (int i = 0; i < dimension; i ++) {
    denominators.computeNormalExcludingIndex(root, i, buffer);
    scalarProduct = root.scalarEuclidean(denominators[i]);
    if (scalarProduct.isEqualToZero()) {
      global.fatal << "Scalar product is equal to zero. " << global.fatal;
    }
    root /= scalarProduct;
    output.addOnTop(root);
  }
}

OnePartialFractionDenominator::OnePartialFractionDenominator(
  const OnePartialFractionDenominator& other
) {
  *this = other;
}

OnePartialFractionDenominator::OnePartialFractionDenominator() {
  this->owner = nullptr;
}

OnePartialFractionDenominator::~OnePartialFractionDenominator() {}

unsigned int OnePartialFractionDenominator::hashFunction() const {
  return this->denominatorsNoScale.hashFunction();
}

bool OnePartialFractionDenominator::operator==(
  const OnePartialFractionDenominator& right
) const {
  return this->denominatorsNoScale == right.denominatorsNoScale;
}

void OnePartialFractionDenominator::operator=(
  const OnePartialFractionDenominator& right
) {
  this->owner = right.owner;
  this->denominatorsNoScale = right.denominatorsNoScale;
}

std::string PartialFractions::toLatex(FormatExpressions* format) const {
  if (
    this->nonReduced.size() > 0 ||
    this->reducedWithElongationRedundancies.size() > 0
  ) {
    return this->toString(format);
  }
  if (this->reduced.size() == 0) {
    return "0";
  }
  std::stringstream out;
  out << "\\begin{array}{ll}&";
  for (int i = 0; i < this->reduced.size(); i ++) {
    if (i != 0) {
      out << "\\\\\n+&\n";
    }
    out
    << this->reduced.monomials[i].toLatex(
      this->reduced.coefficients[i], format
    );
  }
  out << "\\end{array}";
  return out.str();
}

std::string PartialFractions::toHTML(FormatExpressions* format) const {
  STACK_TRACE("PartialFractions::toHTML");
  std::stringstream out;
  out << "Original vectors: " << this->originalVectors.toString() << "<br>";
  out << "\\(" << this->toLatex(format) << "\\)";
  FormatExpressions formatQuasipolynomial;
  formatQuasipolynomial.flagUseFrac = true;
  for (int i = 0; i < this->chambers.refinedCones.size; i ++) {
    QuasiPolynomial& quasiPolynomial = this->allQuasiPolynomials[i];
    const Cone& cone = this->chambers.refinedCones[i];
    out << "<hr>Chamber " << i + 1 << ".<br>";
    out << cone.toHTML();
    out << "<br>Vector partition function.<br>";
    out << quasiPolynomial.toHTML(&formatQuasipolynomial);
  }
  out
  << "<br>Checksum start: "
  << this->checkSumStart
  << ", last: "
  << this->checkSumLast;
  return out.str();
}

std::string PartialFractions::toString(FormatExpressions* format) const {
  if (
    this->reduced.isEqualToZero() &&
    this->nonReduced.isEqualToZero() &&
    this->reducedWithElongationRedundancies.isEqualToZero()
  ) {
    return "0";
  }
  std::stringstream out;
  bool found = false;
  if (!this->reduced.isEqualToZero()) {
    out << this->reduced.toString(format);
    found = true;
  }
  if (!this->reducedWithElongationRedundancies.isEqualToZero()) {
    if (found) {
      out << "+";
    }
    out << "[[" << this->nonReduced.toString(format) << "]]";
    found = true;
  }
  if (!this->nonReduced.isEqualToZero()) {
    if (found) {
      out << "+";
    }
    out << "[[((" << this->nonReduced.toString(format) << "))]]";
    found = true;
  }
  return out.str();
}

bool PartialFractions::assureIndicatorRegularity(
  Vector<Rational>& indicator
) {
  STACK_TRACE("PartialFractions::assureIndicatorRegularity");
  if (this->ambientDimension <= 1) {
    return true;
  }
  Vectors<Rational> roots;
  roots = this->normalizedVectors;
  if (indicator.isEqualToZero()) {
    roots.average(indicator, this->ambientDimension);
    indicator *= roots.size;
  }
  return Cone::regularizeToBasis(roots, indicator);
}

void PartialFractions::prepareCheckSums() {
  STACK_TRACE("PartialFractions::prepareCheckSums");
  if (!this->flagUsingCheckSum) {
    return;
  }
  this->computeCheckSum(this->checkSumStart);
}

void PartialFractions::compareCheckSums() {
  STACK_TRACE("PartialFractions::compareCheckSums");
  if (!this->flagUsingCheckSum || this->flagDiscardingFractions) {
    return;
  }
  this->computeCheckSum(this->checkSumLast);
  if (!this->checkSumStart.isEqualTo(checkSumLast)) {
    global.fatal
    << "Checksums are different: starting: "
    << this->checkSumStart.toString()
    << "; current: "
    << this->checkSumLast.toString()
    << ". Current fractions: "
    << this->toString(nullptr)
    << global.fatal;
  }
}

void PartialFractions::prepareIndicatorVariables() {
  this->numberOfIrrelevantFractions = 0;
  this->numberOfRelevantReducedFractions = 0;
  this->numberOfGeneratorsInNumerators = 0;
  this->numberOfGeneratorsIrrelevantFractions = 0;
  this->totalGeneratorsRelevantFractions = 0;
  this->numberOfMonomialsInNumeratorsIrrelevantFractions = 0;
  this->numberOfMonomialsInNumeratorsRelevantFractions = 0;
  this->numberOfMonomialsInNumerators = 1;
  this->totalReduced = 0;
  this->numberOfRelevantNonReducedFractions = 0;
  this->numberOfProcessedForVPFMonomialsTotal = 0;
  this->totalFractionsWithAccountedVectorPartitionFunction = 0;
  this->numberOfRunsReduceMonomialByMonomial = 0;
}

bool PartialFractions::splitPartial() {
  STACK_TRACE("PartialFractions::splitPartial");
  OnePartialFractionDenominator currentFraction;
  LinearCombination<
    OnePartialFractionDenominator, Polynomial<LargeInteger>
  > currentReduction;
  Polynomial<LargeInteger> currentCoefficient;
  while (this->nonReduced.size() > 0) {
    this->nonReduced.popMonomial(0, currentFraction, currentCoefficient);
    bool needsReduction = currentFraction.reduceOnce(currentReduction);
    if (needsReduction) {
      currentReduction *= currentCoefficient;
      this->nonReduced += currentReduction;
    } else {
      this->reducedWithElongationRedundancies.addMonomial(
        currentFraction, currentCoefficient
      );
    }
    this->makeProgressReportSplittingMainPart();
  }
  this->compareCheckSums();
  return true;
}

bool PartialFractions::checkForMinimalityDecompositionWithRespectToRoot(
  Vector<Rational>* root
) {
  STACK_TRACE(
    "PartialFractions::checkForMinimalityDecompositionWithRespectToRoot"
  );
  for (int i = 0; i < this->nonReduced.size(); i ++) {
    if (
      this->nonReduced[i].denominatorsNoScale.size() <= this->ambientDimension
    ) {
      continue;
    }
    if (this->nonReduced[i].rootIsInFractionCone(root)) {
      return false;
    }
  }
  return true;
}

void OnePartialFractionDenominator::addMultiplicity(
  const Vector<Rational>& normalizedVector,
  int multiplicity,
  int elongation
) {
  STACK_TRACE("OnePartialFractionDenominator::addMultiplicity");
  this->checkInitialization();
  if (!this->owner->normalizedVectors.contains(normalizedVector)) {
    global.fatal << "Unrecognized normalized vector." << global.fatal;
  }
  OnePartialFractionDenominatorComponent* current = nullptr;
  if (!this->denominatorsNoScale.contains(normalizedVector)) {
    OnePartialFractionDenominatorComponent incoming;
    incoming.initialize(
      *this->owner,
      this->owner->normalizedVectors.getIndex(normalizedVector)
    );
    this->denominatorsNoScale.setKeyValue(normalizedVector, incoming);
  }
  current =
  &this->denominatorsNoScale.getValueCreateNoInitialization(normalizedVector);
  current->addMultiplicity(multiplicity, elongation);
  if (current->elongations.size == 0) {
    this->denominatorsNoScale.removeKey(normalizedVector);
  }
}

bool OnePartialFractionDenominator::initializeFromPartialFractions(
  PartialFractions& owner
) {
  STACK_TRACE("OnePartialFraction::initializeFromVectors");
  *this = owner.initialPartialFraction;
  return true;
}

PartialFractions::PartialFractions() {
  this->highestIndex = - 1;
  this->flagSplitTestModeNoNumerators = false;
  this->flagDiscardingFractions = false;
  this->flagUsingCheckSum = true;
  this->flagInitialized = false;
  this->flagMakingProgressReport = true;
  this->splitStepsCounter = 0;
  this->limitSplittingSteps = 0;
  this->numberOfMonomialsInNumeratorsRelevantFractions = 0;
  this->numberOfProcessedForVPFMonomialsTotal = 0;
}

void OnePartialFractionDenominator::getPolyReduceMonomialByMonomial(
  PartialFractions& owner,
  Vector<Rational>& exponent,
  int startMonomialPower,
  int denominatorPowerReduction,
  int startDenominatorPower,
  Polynomial<LargeInteger>& output
) {
  (void) owner;
  if (startMonomialPower == 0) {
    output.makeOne();
    return;
  }
  MonomialPolynomial monomial;
  monomial.makeOne();
  output.makeZero();
  LargeInteger coefficient = 1;
  if (startMonomialPower > 0) {
    if (denominatorPowerReduction != startDenominatorPower) {
      coefficient =
      MathRoutines::nChooseK(startMonomialPower, denominatorPowerReduction);
      coefficient *= MathRoutines::parity(denominatorPowerReduction);
      output.addMonomial(monomial, coefficient);
    } else {
      if (startMonomialPower < startDenominatorPower) {
        global.fatal
        << "Start monomial power: "
        << startMonomialPower
        << " smaller than start denominator power: "
        << startDenominatorPower
        << ". "
        << global.fatal;
      }
      for (int k = 0; k <= startMonomialPower - startDenominatorPower; k ++) {
        monomial = exponent;
        monomial.raiseToPower(k);
        coefficient =
        MathRoutines::parity(startDenominatorPower) *
        MathRoutines::nChooseK(
          startMonomialPower - 1 - k, startDenominatorPower - 1
        );
        output.addMonomial(monomial, coefficient);
      }
    }
  }
  if (startMonomialPower < 0) {
    if (denominatorPowerReduction != startDenominatorPower) {
      coefficient =
      MathRoutines::nChooseK(
        - startMonomialPower - 1 + denominatorPowerReduction,
        denominatorPowerReduction
      );
      output.addMonomial(monomial, coefficient);
    } else {
      for (int k = 1; k <= - startMonomialPower; k ++) {
        monomial = exponent;
        monomial.raiseToPower(- k);
        coefficient =
        MathRoutines::nChooseK(
          startDenominatorPower - startMonomialPower - 1 - k,
          startDenominatorPower - 1
        );
        output.addMonomial(monomial, coefficient);
      }
    }
  }
}

int OnePartialFractionDenominator::controlLineSizeFracs(
  std::string& output, FormatExpressions& polynomialFormatLocal
) {
  int numCutOffs = static_cast<signed>(output.size()) % polynomialFormatLocal.
  maximumLineLength;
  int lastCutOffIndex = 0;
  int numberOfLinesAdded = 0;
  for (int i = 0; i < numCutOffs; i ++) {
    for (
      int j = lastCutOffIndex + polynomialFormatLocal.maximumLineLength; j <
      static_cast<signed>(output.size()) -
      1; j ++
    ) {
      unsigned k = static_cast<unsigned>(j);
      if (output[k] == '\\' && output[k + 1] == 'f') {
        output.insert(k, "\\\\\n&&");
        numberOfLinesAdded ++;
        lastCutOffIndex = j + 5;
        break;
      }
    }
  }
  return numberOfLinesAdded;
}

int OnePartialFractionDenominator::controlLineSizeStringPolys(
  std::string& output, FormatExpressions& polynomialFormatLocal
) {
  int numberOfCutOffs = static_cast<int>(
    static_cast<int>(output.size()) % polynomialFormatLocal.maximumLineLength
  );
  int lastCutOffIndex = 0;
  int numberOfLinesAdded = 0;
  for (int i = 0; i < numberOfCutOffs; i ++) {
    for (
      int j = lastCutOffIndex + polynomialFormatLocal.maximumLineLength; j <
      static_cast<int>(output.size()) -
      1; j ++
    ) {
      unsigned k = static_cast<unsigned>(j);
      if ((output[k] == '+' || output[k] == '-') && output[k - 1] != '{'
      ) {
        output.insert(k, "\\\\\n&&");
        numberOfLinesAdded ++;
        lastCutOffIndex = j + 5;
        break;
      }
    }
  }
  return numberOfLinesAdded;
}

void PartialFractions::makeProgressReportSplittingMainPart() {
  if (!global.response.monitoringAllowed()) {
    return;
  }
  std::stringstream out1, out2, out3;
  out1
  << this->numberOfRelevantReducedFractions
  << " relevant reduced + "
  << this->numberOfIrrelevantFractions
  << " disjoint = "
  << this->totalReduced;
  if (this->numberOfRelevantNonReducedFractions != 0) {
    out1
    << " + "
    << this->numberOfRelevantNonReducedFractions
    << " relevant unreduced ";
  }
  out1
  << " out of "
  << this->nonReduced.size()
  << " non-reduced fractions fractions";
  ProgressReport report1;
  ProgressReport report2;
  ProgressReport report3;
  report1.report(out1.str());
  out2
  << this->numberOfMonomialsInNumeratorsRelevantFractions
  << " relevant reduced + "
  << this->numberOfMonomialsInNumeratorsIrrelevantFractions
  << " disjoint = "
  << this->numberOfMonomialsInNumeratorsRelevantFractions +
  this->numberOfMonomialsInNumeratorsIrrelevantFractions
  << " out of "
  << this->numberOfMonomialsInNumerators
  << " total monomials in the numerators";
  report2.report(out2.str());
  if (this->numberOfGeneratorsInNumerators != 0) {
    out3
    << this->totalGeneratorsRelevantFractions
    << " relevant reduced + "
    << this->numberOfGeneratorsIrrelevantFractions
    << " disjoint = "
    << this->numberOfGeneratorsIrrelevantFractions +
    this->totalGeneratorsRelevantFractions
    << " out of "
    << this->numberOfGeneratorsInNumerators
    << " total generators in the numerators";
    report3.report(out3.str());
  } else {
    report3.report("");
  }
}

void PartialFractions::makeProgressVPFcomputation() {
  this->totalFractionsWithAccountedVectorPartitionFunction ++;
  if (!global.response.monitoringAllowed()) {
    return;
  }
  std::stringstream out2;
  ProgressReport report;
  out2
  << "Processed "
  << this->totalFractionsWithAccountedVectorPartitionFunction
  << " out of "
  << this->numberOfRelevantReducedFractions
  << " relevant fractions";
  report.report(out2.str());
}

Rational PartialFractions::computeCheckSumFromLinearCombination(
  const LinearCombination<
    OnePartialFractionDenominator, Polynomial<LargeInteger>
  >& input
) {
  Rational result = 0;
  this->accumulateCheckSum(input, result);
  return result;
}

void PartialFractions::accumulateCheckSum(
  const LinearCombination<
    OnePartialFractionDenominator, Polynomial<LargeInteger>
  >& input,
  Rational& output
) {
  Vector<Rational> checkSumRoot =
  OnePartialFractionDenominatorComponent::getCheckSumRoot(
    this->ambientDimension
  );
  Rational currentCheckSum;
  Rational rationalValue;
  for (int i = 0; i < input.size(); i ++) {
    input[i].computeOneCheckSum(checkSumRoot, currentCheckSum);
    input[i].evaluateIntegerPolynomial(
      input.coefficients[i], checkSumRoot, rationalValue
    );
    currentCheckSum *= rationalValue;
    output += currentCheckSum;
  }
}

void PartialFractions::computeCheckSum(Rational& output) {
  STACK_TRACE("PartialFractions::computeCheckSum");
  output.makeZero();
  this->accumulateCheckSum(this->reduced, output);
  this->accumulateCheckSum(this->reducedWithElongationRedundancies, output);
  this->accumulateCheckSum(this->nonReduced, output);
}

void PartialFractions::initializeCommon() {
  this->reduced.makeZero();
  this->reducedWithElongationRedundancies.makeZero();
  this->nonReduced.makeZero();
  this->originalVectors.clear();
  this->normalizedVectors.clear();
  this->flagInitialized = false;
  this->splitStepsCounter = 1;
}

void PartialFractions::initializeDimension() {
  if (this->originalVectors.size == 0) {
    this->ambientDimension = - 1;
    return;
  }
  this->ambientDimension = this->originalVectors[0].size;
}

void PartialFractions::initializeInput(
  const List<Vector<Rational> >& input
) {
  STACK_TRACE("PartialFractions::initializeInput");
  this->originalVectors = input;
  List<int> coefficients;
  List<Vector<Rational> > normalizedWithMultiplicity;
  for (int i = 0; i < this->originalVectors.size; i ++) {
    Vector<Rational> normalized = this->originalVectors[i];
    Rational scaleInverted = Rational::scaleNoSignChange(normalized);
    scaleInverted.invert();
    this->normalizedVectors.addOnTopNoRepetition(normalized);
    normalizedWithMultiplicity.addOnTop(normalized);
    int scaleInteger = 0;
    if (!scaleInverted.isSmallInteger(&scaleInteger)) {
      global.fatal
      << "Normalized vector is too short: scale: "
      << scaleInverted.toString()
      << ". "
      << global.fatal;
    }
    coefficients.addOnTop(scaleInteger);
  }
  this->normalizedVectors.quickSortAscending();
  this->initialPartialFraction.owner = this;
  normalizedWithMultiplicity.quickSortAscending(nullptr, &coefficients);
  for (int i = 0; i < normalizedWithMultiplicity.size; i ++) {
    OnePartialFractionDenominatorComponent component;
    const Vector<Rational>& exponent = normalizedWithMultiplicity[i];
    component.initialize(
      *this, this->normalizedVectors.getIndex(exponent)
    );
    this->initialPartialFraction.addMultiplicity(
      exponent, 1, coefficients[i]
    );
  }
}

bool PartialFractions::inputIsValid(std::stringstream* commentsOnFailure) const {
  if (this->normalizedVectors.size < 1) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Zero vectors not allowed.";
    }
    return false;
  }
  for (int i = 0; i < this->normalizedVectors.size; i ++) {
    if (!this->normalizedVectors[i].isEqualToZero()) {
      continue;
    }
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "Zero vector not allowed. ";
    }
    return false;
  }
  for (int i = 0; i < this->normalizedVectors.size; i ++) {
    for (int j = 0; j < this->normalizedVectors[i].size; j ++) {
      if (this->normalizedVectors[i][j].isSmallInteger()) {
        continue;
      }
      if (commentsOnFailure != nullptr) {
        *commentsOnFailure << "Input has non-integer coefficient. ";
      }
      return false;
    }
  }
  Vectors<Rational> rankComputer;
  rankComputer.addListOnTop(this->normalizedVectors);
  if (rankComputer.getRankElementSpan() != this->ambientDimension) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "The input vectors are required to span the entire vector space. ";
    }
    return false;
  }
  return true;
}

bool PartialFractions::initializeFromVectors(
  Vectors<Rational>& input, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("PartialFractions::initializeFromVectors");
  this->initializeCommon();
  this->initializeInput(input);
  this->initializeDimension();
  if (!this->inputIsValid(commentsOnFailure)) {
    return false;
  }
  OnePartialFractionDenominator initialFraction;
  this->ambientDimension = input[0].size;
  bool tempBool = initialFraction.initializeFromPartialFractions(*this);
  Polynomial<LargeInteger> one;
  one.makeOne();
  this->nonReduced.addMonomial(initialFraction, one);
  return tempBool;
}

void PartialFractions::initializeAndSplit(
  Vectors<Rational>& input, std::stringstream* commentsOnFailure
) {
  STACK_TRACE("PartialFractions::initializeAndSplit");
  this->initializeFromVectors(input, commentsOnFailure);
  this->split(nullptr);
}

void PartialFractions::run(Vectors<Rational>& input) {
  if (!this->flagInitialized) {
    this->initializeFromVectors(input, nullptr);
  }
  this->split(nullptr);
}

void PartialFractions::removeRedundantShortRoots(
  Vector<Rational>* indicator
) {
  STACK_TRACE("PartialFractions::removeRedundantShortRoots");
  Rational startCheckSum;
  Polynomial<LargeInteger> coefficient;
  OnePartialFractionDenominator denominator;
  OnePartialFractionDenominator transformedDenominator;
  Polynomial<LargeInteger> transformedCoefficient;
  while (this->reducedWithElongationRedundancies.size() > 0) {
    this->reducedWithElongationRedundancies.popMonomial(
      0, denominator, coefficient
    );
    bool needsMoreReduction =
    this->reduceOnceRedundantShortRoots(
      denominator, transformedDenominator, transformedCoefficient, indicator
    );
    if (needsMoreReduction) {
      transformedCoefficient *= coefficient;
      this->reducedWithElongationRedundancies.addMonomial(
        transformedDenominator, transformedCoefficient
      );
    } else {
      this->reduced.addMonomial(denominator, coefficient);
    }
  }
  this->compareCheckSums();
}

void PartialFractions::removeRedundantShortRootsClassicalRootSystem(
  Vector<Rational>* indicator
) {
  (void) indicator;
  /*PartFraction tempFrac;
  Polynomial<LargeInt> buffer;
  for (int i = 0; i < this->size; i ++) {
    tempFrac.assign(this->objects[i]);
    if (tempFrac.removeRedundantShortRootsClassicalRootSystem(*this, Indicator, buffer, this->AmbientDimension)) {
      this->objects[i].Coefficient.makeZero(this->AmbientDimension);
      this->AddAlreadyReduced(tempFrac, Indicator);
    }
    if (this->flagMakingProgressReport) {
      std::stringstream out;
      out << "Elongating denominator " << i + 1 << " out of " << this->size;
      global.indicatorVariables.ProgressReportStrings[3] = out.str();
      global.FeedIndicatorWindow(global.indicatorVariables);
    }
  }
  for (int i = 0; i < this->size; i ++)
    if (this->objects[i].Coefficient.isEqualToZero()) {
      this->popIndexHashChooseSwapByLowestNonProcessedAndAccount(i, Indicator);
      i --;
    }*/
}

bool PartialFractions::isHigherThanWithRespectToWeight(
  const Vector<Rational>& left,
  const Vector<Rational>& r,
  const Vector<Rational>& weights
) {
  if (left.size != r.size) {
    global.fatal
    << "Left and right vectors need equal dimensions. "
    << global.fatal;
  }
  Rational accum = 0;
  for (int i = 0; i < left.size; i ++) {
    accum += (left[i] - r[i]) * weights[i];
  }
  return accum > 0;
}

// TODO(tmilev): be fixed: you gotta use the preceding function to sort the
// vpBasis!
void PartialFractions::computeKostantFunctionFromWeylGroup(
  char weylGroupLetter,
  int weylGroupNumber,
  QuasiPolynomial& output,
  Vector<Rational>* chamberIndicator
) {
  this->initializeCommon();
  Vectors<Rational> vectorPartitionBasis;
  Vector<Rational> tempWeight;
  tempWeight.setSize(weylGroupNumber);
  WeylGroupData weylGroup;
  weylGroup.makeArbitrarySimple(weylGroupLetter, weylGroupNumber);
  weylGroup.computeRho(true);
  vectorPartitionBasis = weylGroup.rootsOfBorel;
  if (weylGroupLetter == 'B') {
    for (int i = 0; i < vectorPartitionBasis.size; i ++) {
      Rational scalarProduct;
      Vector<Rational> root;
      root = vectorPartitionBasis[i];
      weylGroup.rootScalarCartanRoot(root, root, scalarProduct);
      if (scalarProduct.isEqualToOne()) {
        vectorPartitionBasis.addOnTop(weylGroup.rootsOfBorel[i] * 2);
      }
    }
  }
  if (weylGroupLetter == 'D') {
    Vector<Rational> root;
    root.makeZero(this->ambientDimension);
    root[this->ambientDimension - 1] = 1;
    root[this->ambientDimension - 2] = - 1;
    vectorPartitionBasis.addOnTop(root);
    root[this->ambientDimension - 1] = 1;
    root[this->ambientDimension - 2] = 1;
    vectorPartitionBasis.addOnTop(root);
    for (int i = this->ambientDimension - 3; i >= 0; i --) {
      root[i] = 2;
      global.fatal
      << "This line of code "
      << "needs to be fixed but I don't have time right now. "
      << "This code shouldn't be used before the line is fixed! "
      << global.fatal;
    }
    tempWeight[this->ambientDimension - 2] = 7;
    tempWeight[this->ambientDimension - 1] = 8;
  }
  vectorPartitionBasis.quickSortAscending();
  // fix this!
  global.fatal << " Not implemented yet. " << global.fatal;
  //  this->initFromRoots(vpBasis, 0);
  // this->flagSplitTestModeNoNumerators = true;
  //  this->split(ChamberIndicator);
  if (
    !this->checkForMinimalityDecompositionWithRespectToRoot(chamberIndicator)
  ) {
    global.fatal << "Minimality decomposition missing. " << global.fatal;
  }
  // return;
  Vector<Rational> root;
  if (chamberIndicator != nullptr) {
    root = *chamberIndicator;
  } else {
    root.makeZero(this->ambientDimension);
  }
  if (!this->computeOneVectorPartitionFunction(output, root)) {
    global.fatal
    << "Failed to get vector partition function. "
    << global.fatal;
  }
}

void OnePartialFractionDenominatorComponent::operator=(
  const OnePartialFractionDenominatorComponent& right
) {
  this->owner = right.owner;
  this->normalizedVectorIndex = right.normalizedVectorIndex;
  this->multiplicities = right.multiplicities;
  this->elongations = right.elongations;
}

unsigned int OnePartialFractionDenominatorComponent::hashFunction() const {
  return static_cast<unsigned>(this->getTotalMultiplicity());
}

void OnePartialFractionDenominatorComponent::getPolynomialDenominator(
  Polynomial<LargeInteger>& output,
  int multiplicityIndex,
  Vector<Rational>& exponent
) {
  if (multiplicityIndex >= this->multiplicities.size) {
    global.fatal << "Bad multiplicity. " << global.fatal;
  }
  MonomialPolynomial monomial;
  output.makeOne();
  monomial.makeOne();
  for (int i = 0; i < exponent.size; i ++) {
    monomial.setVariable(
      i, exponent[i] * this->elongations[multiplicityIndex]
    );
  }
  output.addMonomial(monomial, - 1);
}

int OnePartialFractionDenominatorComponent::getLargestElongation() const {
  STACK_TRACE("OnePartialFractionDenominatorComponent::getLargestElongation");
  if (this->elongations.size == 0) {
    global.fatal
    << "Elongations are not supposed to be empty. "
    << global.fatal;
  }
  int result = this->elongations[0];
  for (int i = 1; i < this->elongations.size; i ++) {
    if (this->elongations[i] == result) {
      global.fatal << "Elongation does not math the result. " << global.fatal;
    }
    if (this->elongations[i] > result) {
      result = this->elongations[i];
    }
  }
  return result;
}

int OnePartialFractionDenominatorComponent::getLeastCommonMultipleElongations()
const {
  int result = 1;
  for (int i = 0; i < this->elongations.size; i ++) {
    if (this->elongations[i] == 0) {
      global.fatal << "Elongation not allowed to be zero. " << global.fatal;
    }
    result =
    MathRoutines::leastCommonMultiple(this->elongations[i], result);
  }
  return result;
}

void OnePartialFractionDenominatorComponent::getDenominatorExponents(
  List<Vector<Rational> >& output
) const {
  output.clear();
  Vector<Rational> normalizedVector = this->getNormalizedVector();
  for (int i = 0; i < this->multiplicities.size; i ++) {
    for (int j = 0; j < this->multiplicities[i]; j ++) {
      output.addOnTop(normalizedVector * this->elongations[i]);
    }
  }
}

void OnePartialFractionDenominatorComponent::
getDenominatorExponentsWithoutMultiplicities(
  List<Vector<Rational> >& output
) const {
  output.clear();
  Vector<Rational> normalizedVector = this->getNormalizedVector();
  for (int i = 0; i < this->multiplicities.size; i ++) {
    output.addOnTop(normalizedVector * this->elongations[i]);
  }
}

int OnePartialFractionDenominatorComponent::getTotalMultiplicity() const {
  int result = 0;
  for (int i = 0; i < this->elongations.size; i ++) {
    result += this->multiplicities[i];
  }
  return result;
}

int OnePartialFractionDenominatorComponent::indexLargestElongation() const {
  int result = 0;
  for (int i = 1; i < this->elongations.size; i ++) {
    if (this->elongations[i] > this->elongations[result]) {
      result = i;
    }
  }
  return result;
}

void OnePartialFractionDenominatorComponent::initialize(
  PartialFractions& inputOwner, int inputNormalizedVectorIndex
) {
  this->elongations.setSize(0);
  this->multiplicities.setSize(0);
  this->owner = &inputOwner;
  this->normalizedVectorIndex = inputNormalizedVectorIndex;
}

bool OnePartialFractionDenominatorComponent::checkInitialization() const {
  if (this->owner == nullptr) {
    global.fatal << "Denominator without owner." << global.fatal;
  }
  return true;
}

const Vector<Rational>& OnePartialFractionDenominatorComponent::
getNormalizedVector() const {
  this->checkInitialization();
  return this->owner->normalizedVectors[this->normalizedVectorIndex];
}

void OnePartialFractionDenominatorComponent::getLongestVector(
  Vector<Rational>& output
) const {
  return this->getElongatedVector(this->indexLargestElongation(), output);
}

void OnePartialFractionDenominatorComponent::getElongatedVector(
  int index, Vector<Rational>& output
) const {
  output = this->getNormalizedVector() * this->elongations[index];
}

void OnePartialFractionDenominatorComponent::getMonomial(
  int elongationIndex, MonomialPolynomial& output
) {
  Vector<Rational> powers;
  this->getElongatedVector(elongationIndex, powers);
  output.makeFromPowers(powers);
}

void OnePartialFractionDenominatorComponent::computeOneCheckSum(
  Rational& output, const Vector<Rational>& variableValues
) {
  STACK_TRACE("OnePartialFractionDenominator::computeOneCheckSum");
  output = 1;
  MonomialPolynomial monomial;
  Rational scalar;
  Rational minuend;
  Rational exponentiated;
  for (int i = 0; i < this->elongations.size; i ++) {
    this->getMonomial(i, monomial);
    monomial.evaluate(variableValues, minuend);
    scalar = 1;
    scalar -= minuend;
    scalar.raiseToPower(this->multiplicities[i]);
    output.multiplyBy(scalar);
  }
}

int OnePartialFractionDenominatorComponent::getMultiplicityLargestElongation()
{
  int result = 0;
  int largestElongationFound = 0;
  for (int i = 0; i < this->elongations.size; i ++) {
    if (largestElongationFound < this->elongations[i]) {
      largestElongationFound = this->elongations[i];
      result = this->multiplicities[i];
    }
  }
  return result;
}

void OnePartialFractionDenominatorComponent::addMultiplicity(
  int multiplicityIncrement, int elongation
) {
  if (multiplicityIncrement == 0) {
    return;
  }
  int elongationIndex = this->elongations.getIndex(elongation);
  if (elongationIndex == - 1) {
    this->elongations.addOnTop(elongation);
    this->multiplicities.addOnTop(0);
    this->elongations.quickSortAscending(nullptr, &this->multiplicities);
    elongationIndex = this->elongations.getIndex(elongation);
  }
  if (this->multiplicities[elongationIndex] + multiplicityIncrement < 0) {
    global.fatal
    << "Multiplicity is supposed to be non-negative. Instead it is: "
    << this->toStringInternal()
    << ". "
    << "Multiplicity increment: "
    << multiplicityIncrement
    << ", elongation: "
    << elongation
    << ". "
    << global.fatal;
  }
  this->multiplicities[elongationIndex] += multiplicityIncrement;
  if (this->multiplicities[elongationIndex] == 0) {
    this->multiplicities.popIndexShiftDown(elongationIndex);
    this->elongations.popIndexShiftDown(elongationIndex);
  }
}

OnePartialFractionDenominatorComponent::OnePartialFractionDenominatorComponent(
) {
  this->owner = nullptr;
  this->normalizedVectorIndex = - 1;
}

bool OnePartialFractionDenominatorComponent::operator>(
  const OnePartialFractionDenominatorComponent& other
) const {
  if (this->normalizedVectorIndex > other.normalizedVectorIndex) {
    return true;
  }
  if (this->normalizedVectorIndex < other.normalizedVectorIndex) {
    return false;
  }
  if (this->multiplicities > other.multiplicities) {
    return true;
  }
  if (other.multiplicities > this->multiplicities) {
    return false;
  }
  if (this->elongations > other.elongations) {
    return true;
  }
  if (other.elongations > this->elongations) {
    return false;
  }
  return false;
}

std::string OnePartialFractionDenominatorComponent::toStringOneDenominator(
  int elongation, int multiplicity, FormatExpressions* format
) const {
  if (this->owner == nullptr) {
    return "(uninitialized OnePartialFractionDenominator)";
  }
  std::stringstream out;
  Vector<Rational> exponent = this->getNormalizedVector();
  exponent *= elongation;
  MemorySaving<FormatExpressions> formatContainer;
  if (format == nullptr) {
    format = &formatContainer.getElement();
  }
  out << "(1-";
  MonomialPolynomial monomial;
  monomial.makeFromPowers(exponent);
  out << monomial.toString(format);
  out << ")";
  if (multiplicity > 1) {
    out << "^{" << multiplicity << "}";
  }
  if (format->flagUseLatex) {
    out << "}";
  }
  return out.str();
}

std::string OnePartialFractionDenominatorComponent::toStringInternal() const {
  std::stringstream out;
  out
  << "elongations: "
  << this->elongations.toStringCommaDelimited()
  << ", "
  << "multiplicities: "
  << this->multiplicities.toStringCommaDelimited();
  return out.str();
}

std::string OnePartialFractionDenominatorComponent::toString(
  FormatExpressions* format
) const {
  if (this->owner == nullptr) {
    return "(uninitialized)";
  }
  if (this->multiplicities.size == 0) {
    return "1";
  }
  MemorySaving<FormatExpressions> formatContainer;
  if (format == nullptr) {
    format = &formatContainer.getElement();
  }
  std::stringstream out;
  for (int k = 0; k < this->multiplicities.size; k ++) {
    out
    << this->toStringOneDenominator(
      this->elongations[k], this->multiplicities[k], format
    );
  }
  out << " ";
  return out.str();
}

bool OnePartialFractionDenominatorComponent::operator==(
  const OnePartialFractionDenominatorComponent& right
) const {
  return
  this->elongations == right.elongations &&
  this->normalizedVectorIndex == right.normalizedVectorIndex &&
  this->multiplicities == right.multiplicities;
}

void PartialFractions::computeTable(int dimension) {
  Vector<Rational> tempR;
  Vector<Rational> tempR2;
  Vector<Rational> tempR3;
  tempR.setSize(dimension);
  tempR2.setSize(dimension);
  tempR3.setSize(dimension);
  this->indicesRedundantShortRoots.initialize(this->reduced.size());
  this->indicesDoublesOfRedundantShortRoots.setSize(this->reduced.size());
  this->tableAllowedAminus2B.initialize(
    this->reduced.size(), this->reduced.size()
  );
  this->tableAllowedAminusB.initialize(
    this->reduced.size(), this->reduced.size()
  );
  for (int i = 0; i < this->reduced.size(); i ++) {
    for (int j = 0; j < this->reduced.size(); j ++) {
      for (int k = 0; k < dimension; k ++) {
        tempR[k] = this->originalVectors[i][k] - this->originalVectors[j][k];
        tempR2[k] =
        this->originalVectors[i][k] - this->originalVectors[j][k] * 2;
      }
      this->tableAllowedAminusB.elements[i][j] = this->getIndex(tempR);
      this->tableAllowedAminus2B.elements[i][j] = this->getIndex(tempR2);
    }
    tempR3 = this->originalVectors[i];
    tempR3 *= 2;
    this->indicesDoublesOfRedundantShortRoots[i] = this->getIndex(tempR3);
    if (indicesDoublesOfRedundantShortRoots[i] != - 1) {
      this->indicesRedundantShortRoots.addSelectionAppendNewIndex(i);
    }
  }
}

int PartialFractions::getIndex(const Vector<Rational>& root) {
  return this->normalizedVectors.getIndex(root);
}

int PartialFractions::getIndexDoubleOfARoot(const Vector<Rational>& root) {
  return this->getIndex(root* 2);
}

void SelectionWithDifferentMaxMultiplicities::initializePart1(
  int numberOfElements
) {
  this->multiplicities.initializeFillInObject(numberOfElements, 0);
  this->capacities.initializeFillInObject(numberOfElements, 0);
  this->elements.reserve(numberOfElements);
  this->elements.size = 0;
}

void SelectionWithMultiplicities::initWithMultiplicities(int numberOfElements)
{
  this->multiplicities.setSize(numberOfElements);
  for (int i = 0; i < this->multiplicities.size; i ++) {
    this->multiplicities[i] = 0;
  }
  this->elements.reserve(numberOfElements);
  this->elements.size = 0;
}

void SelectionWithMultiplicities::toString(std::string& output) {
  std::stringstream out;
  for (int i = 0; i < this->elements.size; i ++) {
    out
    << "Index: "
    << this->elements[i]
    << "\nMultiplicity: "
    << this->multiplicities[this->elements[i]];
  }
  output = out.str();
}

void SelectionWithMaxMultiplicity::initMaxMultiplicity(
  int numberOfElements, int maximulMultiplicity
) {
  this->::SelectionWithMultiplicities::initWithMultiplicities(
    numberOfElements
  );
  this->maximumMultiplicity = maximulMultiplicity;
}

int::SelectionWithMaxMultiplicity::cardinalitySelectionWithMultiplicities() {
  int result = 0;
  for (int i = 0; i < this->multiplicities.size; i ++) {
    result += this->multiplicities[i];
  }
  return result;
}

bool SelectionWithMaxMultiplicity::hasMultiplicitiesZeroAndOneOnly() {
  for (int i = 0; i < this->elements.size; i ++) {
    if (this->multiplicities[elements[i]] > 1) {
      return false;
    }
  }
  return true;
}

void SelectionWithMaxMultiplicity::incrementSubsetFixedCardinality(
  int cardinality
) {
  if (
    cardinality < 1 ||
    cardinality > this->maximumMultiplicity * this->multiplicities.size
  ) {
    return;
  }
  if (this->cardinalitySelectionWithMultiplicities() != cardinality) {
    this->multiplicities.initializeFillInObject(
      this->multiplicities.size, 0
    );
  }
  if (this->cardinalitySelectionWithMultiplicities() == 0) {
    for (int i = this->multiplicities.size - 1; cardinality > 0; i --) {
      if (cardinality >= this->maximumMultiplicity) {
        this->multiplicities[i] = this->maximumMultiplicity;
      } else {
        this->multiplicities[i] = cardinality;
      }
      cardinality -= this->multiplicities[i];
    }
    this->computeElements();
    return;
  }
  int firstNonZeroMult;
  int currentCardinality = cardinality;
  for (
    firstNonZeroMult = this->multiplicities.size - 1; firstNonZeroMult >= 0;
    firstNonZeroMult --
  ) {
    if (this->multiplicities[firstNonZeroMult] > 0) {
      break;
    }
  }
  if (firstNonZeroMult == 0) {
    return;
  }
  currentCardinality -= this->multiplicities[firstNonZeroMult];
  this->multiplicities[firstNonZeroMult] = 0;
  for (int i = firstNonZeroMult - 1; i >= 0; i --) {
    if (this->multiplicities[i] < this->maximumMultiplicity) {
      this->multiplicities[i] ++;
      currentCardinality ++;
      break;
    } else {
      this->multiplicities[i] = 0;
      currentCardinality -= this->maximumMultiplicity;
    }
  }
  for (
    int i = this->multiplicities.size - 1; currentCardinality < cardinality; i
    --
  ) {
    if (this->multiplicities[i] != 0) {
      global.fatal
      << "Non-zero multiplicities not implemented here. "
      << global.fatal;
    }
    if (cardinality - currentCardinality >= this->maximumMultiplicity) {
      this->multiplicities[i] = this->maximumMultiplicity;
    } else {
      this->multiplicities[i] = cardinality - currentCardinality;
    }
    currentCardinality += this->multiplicities[i];
  }
  this->computeElements();
}

LargeInteger SelectionWithMaxMultiplicity::numberOfCombinationsOfCardinality(
  int cardinality
) {
  return
  MathRoutines::nChooseK(
    this->multiplicities.size + cardinality - 1, cardinality
  );
}

LargeInteger SelectionWithMaxMultiplicity::getTotalCombinationCount() const {
  // if (this->maximumMultiplicity == 0)
  //  return 1;
  LargeInteger result;
  MathRoutines::kToTheNth(
    this->maximumMultiplicity + 1, this->multiplicities.size, result
  );
  return result;
}

bool SelectionWithMaxMultiplicity::incrementReturnFalseIfPastLast() {
  this->incrementSubset();
  return this->elements.size != 0;
}

void SelectionWithMaxMultiplicity::incrementSubset() {
  for (int i = this->multiplicities.size - 1; i >= 0; i --) {
    if (this->multiplicities[i] < this->maximumMultiplicity) {
      if (this->multiplicities[i] == 0) {
        this->elements.addOnTop(i);
      }
      this->multiplicities[i] ++;
      return;
    } else {
      this->multiplicities[i] = 0;
      this->elements.removeFirstOccurenceSwapWithLast(i);
    }
  }
}

void SelectionWithMultiplicities::computeElements() {
  this->elements.size = 0;
  for (int i = 0; i < this->multiplicities.size; i ++) {
    if (this->multiplicities[i] > 0) {
      this->elements.addOnTop(i);
    }
  }
}

int SelectionWithMultiplicities::cardinalitySelectionWithoutMultiplicities() {
  return this->elements.size;
}

int SelectionWithDifferentMaxMultiplicities::totalNumberSubsetsSmallInt() {
  int result = 1;
  for (int i = 0; i < this->capacities.size; i ++) {
    result *= (this->capacities[i] + 1);
    if (result < 0) {
      global.fatal
      << "I was asked to enumerate "
      <<
      "all subsets of a multi-set, however the number of subsets is larger than  "
      << "the maximum value allowed for int on the system "
      << "(on a 32 bit machine that is around 2 billion). "
      << "This can be fixed, however I do not have time at the moment. If you "
      <<
      "encounter this error, write me an email and I will take the time to fix this issue. "
      << global.fatal;
    }
  }
  return result;
}

LargeInteger SelectionWithDifferentMaxMultiplicities::totalNumberOfSubsets() {
  LargeInteger result = 1;
  for (int i = 0; i < this->capacities.size; i ++) {
    result *= (this->capacities[i] + 1);
  }
  return result;
}

void SelectionWithDifferentMaxMultiplicities::initializeFromIntegers(
  int* maximalMultiplicities, int numberOfMaximalMultiplicities
) {
  this->multiplicities.initializeFillInObject(
    numberOfMaximalMultiplicities, 0
  );
  this->capacities.setSize(numberOfMaximalMultiplicities);
  for (int i = 0; i < this->capacities.size; i ++) {
    this->capacities[i] = maximalMultiplicities[i];
  }
  this->elements.initializeFillInObject(numberOfMaximalMultiplicities, 0);
}

void SelectionWithDifferentMaxMultiplicities::initializeFromIntegers(
  const List<int>& maximalMultiplicities
) {
  this->multiplicities.initializeFillInObject(maximalMultiplicities.size, 0);
  this->elements.initializeFillInObject(maximalMultiplicities.size, 0);
  this->capacities = maximalMultiplicities;
}

int::SelectionWithDifferentMaxMultiplicities::totalMultiplicity() {
  int result = 0;
  for (int i = 0; i < this->multiplicities.size; i ++) {
    result += this->multiplicities[i];
  }
  return result;
}

int::SelectionWithDifferentMaxMultiplicities::maximumTotalMultiplicity() {
  int result = 0;
  for (int i = 0; i < this->multiplicities.size; i ++) {
    result += this->capacities[i];
  }
  return result;
}

void::SelectionWithDifferentMaxMultiplicities::clearNoMaxMultiplicitiesChange()
{
  for (int i = 0; i < this->multiplicities.size; i ++) {
    this->multiplicities[i] = 0;
  }
}

bool SelectionWithDifferentMaxMultiplicities::incrementReturnFalseIfPastLast()
{
  STACK_TRACE(
    "SelectionWithDifferentMaxMultiplicities::incrementReturnFalseIfPastLast"
  );
  for (int i = this->multiplicities.size - 1; i >= 0; i --) {
    if (this->multiplicities[i] < this->capacities[i]) {
      if (this->multiplicities[i] == 0) {
        this->elements.addOnTop(i);
      }
      this->multiplicities[i] ++;
      return true;
    } else {
      this->multiplicities[i] = 0;
      this->elements.removeFirstOccurenceSwapWithLast(i);
    }
  }
  return false;
}

void DynkinType::getTypesWithMults(List<DynkinSimpleType>& output) const {
  output.setSize(0);
  output.reserve(this->getNumberOfSimpleComponents());
  List<DynkinSimpleType> componentsSorted;
  this->getSortedDynkinTypes(componentsSorted);
  for (int i = 0; i < componentsSorted.size; i ++) {
    int index = this->monomials.getIndex(componentsSorted[i]);
    for (int j = 0; j < this->getMultiplicity(index); j ++) {
      output.addOnTop(componentsSorted[i]);
    }
  }
}

void DynkinType::getOuterAutosGeneratorsOneTypeActOnVectorColumn(
  List<MatrixTensor<Rational> >& output,
  const DynkinSimpleType& dynkinType,
  int multiplicity
) {
  STACK_TRACE("DynkinType::getOuterAutosGeneratorsOneTypeActOnVectorColumn");
  output.setSize(0);
  MatrixTensor<Rational> directSummand, finalMat;
  if (
    dynkinType.letter == 'D' || (
      dynkinType.letter == 'A' && dynkinType.rank > 1
    ) || (dynkinType.letter == 'E' && dynkinType.rank == 6)
  ) {
    directSummand.makeIdentity(dynkinType.rank *(multiplicity - 1));
    int numGens = 1;
    if (dynkinType.letter == 'D' && dynkinType.rank == 4) {
      numGens = 2;
    }
    for (int i = 1; i < numGens + 1; i ++) {
      dynkinType.getAutomorphismActingOnVectorColumn(finalMat, i);
      finalMat.directSumWith(directSummand);
      output.addOnTop(finalMat);
    }
  }
  if (multiplicity < 2) {
    return;
  }
  for (int i = 0; i < multiplicity - 1; i ++) {
    directSummand.makeZero();
    for (int j = 0; j < dynkinType.rank; j ++) {
      directSummand.addMonomial(
        MonomialMatrix(j, dynkinType.rank + j), 1
      );
      directSummand.addMonomial(
        MonomialMatrix(dynkinType.rank + j, j), 1
      );
    }
    finalMat.makeIdentity(i * dynkinType.rank);
    finalMat.directSumWith(directSummand);
    directSummand.makeIdentity((multiplicity - 2 - i) * dynkinType.rank);
    finalMat.directSumWith(directSummand);
    output.addOnTop(finalMat);
  }
}

void DynkinType::getOuterAutosGeneratorsActOnVectorColumn(
  List<MatrixTensor<Rational> >& output
) {
  STACK_TRACE("DynkinType::GetOuterAutosGenerators");
  this->sortDynkinTypes();
  List<MatrixTensor<Rational> > intermediateGenerators;
  MatrixTensor<Rational> matrixFinal, matrixToGo;
  int currentMult = 0;
  output.setSize(0);
  int numRowsSoFar = 0;
  for (int i = 0; i < this->size(); i ++) {
    if (!this->coefficients[i].isSmallInteger(&currentMult)) {
      global.fatal
      << "This is not supposed to happen in function "
      << "DynkinType::getOuterAutosGeneratorsActOnVectorColumn."
      << global.fatal;
    }
    this->getOuterAutosGeneratorsOneTypeActOnVectorColumn(
      intermediateGenerators, (*this)[i], currentMult
    );
    matrixToGo.makeIdentity(
      this->getRank() - numRowsSoFar - currentMult *(*this)[i].rank
    );
    for (int j = 0; j < intermediateGenerators.size; j ++) {
      matrixFinal.makeIdentity(numRowsSoFar);
      matrixFinal.directSumWith(intermediateGenerators[j]);
      matrixFinal.directSumWith(matrixToGo);
      output.addOnTop(matrixFinal);
    }
    numRowsSoFar += currentMult *(*this)[i].rank;
  }
  if (output.size == 0) {
    output.setSize(1);
    output[0].makeIdentity(this->getRank());
  }
}

void DynkinType::getLettersTypesMultiplicities(
  List<char>* outputLetters,
  List<int>* outputRanks,
  List<int>* outputMults,
  List<Rational>* outputFirstCoRootLengthsSquared
) const {
  if (outputLetters != nullptr) {
    outputLetters->setSize(0);
  }
  if (outputRanks != nullptr) {
    outputRanks->setSize(0);
  }
  if (outputMults != nullptr) {
    outputMults->setSize(0);
  }
  if (outputFirstCoRootLengthsSquared != nullptr) {
    outputFirstCoRootLengthsSquared->setSize(0);
  }
  List<DynkinSimpleType> componentsSorted;
  this->getSortedDynkinTypes(componentsSorted);
  for (int i = 0; i < componentsSorted.size; i ++) {
    int index = this->monomials.getIndex(componentsSorted[i]);
    if (outputLetters != nullptr) {
      outputLetters->addOnTop((*this)[index].letter);
    }
    if (outputRanks != nullptr) {
      outputRanks->addOnTop((*this)[index].rank);
    }
    if (outputFirstCoRootLengthsSquared != nullptr) {
      outputFirstCoRootLengthsSquared->addOnTop((*this)[index].
        cartanSymmetricInverseScale
      );
    }
    if (outputMults != nullptr) {
      outputMults->addOnTop(this->getMultiplicity(index));
    }
  }
}

std::string DynkinType::getWeylGroupName(FormatExpressions* format) const {
  return this->toString(format);
}

bool DynkinType::canBeExtendedParabolicallyOrIsEqualTo(
  const DynkinType& other
) const {
  STACK_TRACE("DynkinType::canBeExtendedParabolicallyOrIsEqualTo");
  if (this->isEqualToZero()) {
    return true;
  }
  if (other.isEqualToZero()) {
    return false;
  }
  const DynkinSimpleType& currentSimpleType = (*this)[0];
  DynkinType remainderThis, remainderOther;
  for (int i = 0; i < other.size(); i ++) {
    if (
      !currentSimpleType.canBeExtendedParabolicallyOrIsEqualTo(other[i])
    ) {
      continue;
    }
    remainderThis = *this;
    remainderThis.subtractMonomial(currentSimpleType, 1);
    remainderOther = other;
    remainderOther.subtractMonomial(other[i], 1);
    if (
      remainderThis.canBeExtendedParabolicallyOrIsEqualTo(remainderOther)
    ) {
      return true;
    }
  }
  return false;
}

bool DynkinType::canBeExtendedParabolicallyTo(const DynkinType& other) const {
  STACK_TRACE("DynkinType::canBeExtendedParabolicallyTo");
  if (*this == other) {
    return false;
  }
  return this->canBeExtendedParabolicallyOrIsEqualTo(other);
}

bool DynkinType::grow(
  const List<Rational>& allowedInverseScales,
  int ambientWeylDimension,
  List<DynkinType>& output,
  List<List<int> >* outputPermutationRoots
) const {
  STACK_TRACE("DynkinType::grow");
  output.setSize(0);
  if (outputPermutationRoots != nullptr) {
    outputPermutationRoots->setSize(0);
  }
  if (this->getRank() >= ambientWeylDimension) {
    return true;
  }
  if (this->isEqualToZero()) {
    output.setSize(allowedInverseScales.size);
    if (outputPermutationRoots != nullptr) {
      outputPermutationRoots->setSize(allowedInverseScales.size);
    }
    for (int i = 0; i < allowedInverseScales.size; i ++) {
      output[i].makeSimpleType('A', 1, &allowedInverseScales[i]);
      if (outputPermutationRoots != nullptr) {
        (*outputPermutationRoots)[i].setSize(1);
        (*outputPermutationRoots)[i][0] = 0;
      }
    }
    return true;
  }
  // Rational minCoRootLengthSquared = - 1;
  // growth is allowed from the minimal component only
  DynkinSimpleType minimalComponent;
  Rational coefficientMinimalComponent;
  List<int> currentRootInjection;
  this->getMinimalMonomial(minimalComponent, coefficientMinimalComponent);
  if (coefficientMinimalComponent == 1) {
    DynkinType typeMinusMin = (*this);
    typeMinusMin.subtractMonomial(minimalComponent, 1);
    List<DynkinSimpleType> simpleTypes;
    List<List<int> > lastComponentRootInjections;
    minimalComponent.grow(simpleTypes, &lastComponentRootInjections);
    currentRootInjection.setSize(this->getRank() + 1);
    for (int i = 0; i < simpleTypes.size; i ++) {
      bool isGood = true;
      for (int j = 0; j < typeMinusMin.size(); j ++) {
        if (simpleTypes[i] > typeMinusMin[j]) {
          isGood = false;
          break;
        }
      }
      if (!isGood) {
        continue;
      }
      output.addOnTop(typeMinusMin);
      output.lastObject()->addMonomial(simpleTypes[i], 1);
      if (outputPermutationRoots != nullptr) {
        int baseTypeRank = typeMinusMin.getRank();
        for (int j = 0; j < baseTypeRank; j ++) {
          currentRootInjection[j] = j;
        }
        for (int j = 0; j < lastComponentRootInjections[i].size; j ++) {
          currentRootInjection[j + baseTypeRank] =
          lastComponentRootInjections[i][j] + baseTypeRank;
        }
        outputPermutationRoots->addOnTop(currentRootInjection);
      }
    }
  }
  DynkinSimpleType currentA1;
  for (int i = 0; i < allowedInverseScales.size; i ++) {
    currentA1.makeArbitrary('A', 1, allowedInverseScales[i]);
    if (minimalComponent < currentA1) {
      continue;
    }
    output.addOnTop(*this);
    output.lastObject()->addMonomial(currentA1, Rational::one());
    currentRootInjection.setSize(output.lastObject()->getRank());
    if (outputPermutationRoots != nullptr) {
      for (int j = 0; j < currentRootInjection.size; j ++) {
        currentRootInjection[j] = j;
      }
      outputPermutationRoots->addOnTop(currentRootInjection);
    }
  }
  return true;
}

int DynkinType::getIndexPreimageFromRootInjection(
  int inputIndex, const List<int>& inputRootInjection
) {
  STACK_TRACE("DynkinType::getIndexPreimageFromRootInjection");
  for (int i = 0; i < inputRootInjection.size; i ++) {
    if (inputRootInjection[i] == inputIndex) {
      return i;
    }
  }
  global.fatal
  << "Asking to find the preimage of root index "
  << inputIndex
  << " w.r.t. root injection "
  << inputRootInjection
  << " - the root index has no preimage. "
  << "This function is not allowed to fail. "
  << global.fatal;
  return - 1;
}

void DynkinType::makeSimpleType(
  char type, int rank, const Rational* inputFirstCoRootSquareLength
) {
  DynkinSimpleType dynkinType;
  Rational cartanSymmetricInvScale = (
    inputFirstCoRootSquareLength == nullptr ? 1 : *inputFirstCoRootSquareLength
  );
  dynkinType.makeArbitrary(type, rank, cartanSymmetricInvScale);
  this->makeZero();
  this->addMonomial(dynkinType, 1);
}

void DynkinType::operator=(
  const LinearCombination<DynkinSimpleType, Rational>& other
) {
  this->::LinearCombination<DynkinSimpleType, Rational>::operator=(other);
}

DynkinType DynkinType::operator-(
  const LinearCombination<DynkinSimpleType, Rational>& other
) {
  DynkinType result = *this;
  result -= other;
  return result;
}

bool DynkinType::hasExceptionalComponent() const {
  for (int i = 0; i < this->size(); i ++) {
    if ((*this)[i].letter == 'E' || (*this)[i].letter == 'F' || (*this)[i].
      letter ==
      'G'
    ) {
      return true;
    }
  }
  return false;
}

bool DynkinType::containsType(char typeLetter) const {
  for (int i = 0; i < this->size(); i ++) {
    if ((*this)[i].letter == typeLetter) {
      return true;
    }
  }
  return false;
}

bool DynkinType::isSimpleOfType(char desiredType, int desiredRank) const {
  char currentType;
  int currentRank;
  if (!this->isSimple(&currentType, &currentRank)) {
    return false;
  }
  return currentType == desiredType && currentRank == desiredRank;
}

bool DynkinType::isSimpleOfType(
  char desiredType, int* outputRank, Rational* outputLength
) const {
  char actualType = 0;
  if (!this->isSimple(&actualType, outputRank, outputLength)) {
    return false;
  }
  return actualType == desiredType;
}

bool DynkinType::isSimple(
  char* outputtype, int* outputRank, Rational* outputLength
) const {
  if (this->size() != 1) {
    return false;
  }
  if (this->coefficients[0] != 1) {
    return false;
  }
  const DynkinSimpleType& dynkinType = (*this)[0];
  if (outputtype != nullptr) {
    *outputtype = dynkinType.letter;
  }
  if (outputRank != nullptr) {
    *outputRank = dynkinType.rank;
  }
  if (outputLength != nullptr) {
    *outputLength = dynkinType.cartanSymmetricInverseScale;
  }
  return true;
}

int DynkinType::getMultiplicity(int SimpleTypeIdentifier) const {
  int result = 0;
  if (
    !this->coefficients[SimpleTypeIdentifier].isSmallInteger(&result)
  ) {
    global.fatal
    << "Dynkin type has multiplicity that is not a small integer. "
    << global.fatal;
  }
  return result;
}

int DynkinType::getNumberOfSimpleComponentsOfGivenRank(int desiredRank) const {
  Rational result = 0;
  for (int i = 0; i < this->size(); i ++) {
    if ((*this)[i].rank == desiredRank) {
      result += this->coefficients[i];
    }
  }
  int output = 0;
  if (!result.isSmallInteger(&output)) {
    global.fatal
    <<
    "Dynkin type has a number of simple components which is not a small integer. "
    << global.fatal;
  }
  return output;
}

int DynkinType::getNumberOfSimpleComponents() const {
  Rational result = 0;
  for (int i = 0; i < this->size(); i ++) {
    result += this->coefficients[i];
  }
  int output = 0;
  if (!result.isSmallInteger(&output)) {
    global.fatal
    <<
    "Dynkin type has a number of simple components which is not a small integer. "
    << global.fatal;
  }
  return output;
}

Rational DynkinType::getRankRational() const {
  Rational result = 0;
  for (int i = 0; i < this->size(); i ++) {
    result += this->coefficients[i] *(*this)[i].rank;
  }
  return result;
}

int DynkinType::getLieAlgebraDimension() const {
  Rational result = 0;
  for (int i = 0; i < this->size(); i ++) {
    result += this->coefficients[i] *(*this)[i].getLieAlgebraDimension();
  }
  int intResult = 0;
  if (!result.isSmallInteger(&intResult)) {
    global.fatal
    << "Multiplicity of simple type is not a small integer. "
    << global.fatal;
  }
  return intResult;
}

int DynkinType::getRootSystemSize() const {
  Rational result = 0;
  for (int i = 0; i < this->size(); i ++) {
    result += this->coefficients[i] *(*this)[i].getRootSystemSize();
  }
  int intResult = 0;
  if (!result.isSmallInteger(&intResult)) {
    global.fatal
    << "Multiplicity of simple type is not a small integer. "
    << global.fatal;
  }
  return intResult;
}

int DynkinType::getRank() const {
  Rational rationalRank = this->getRankRational();
  int result = 0;
  if (!rationalRank.isSmallInteger(&result)) {
    global.fatal
    << "Attempt to get a small integer "
    << "rank from a Dynkin type whose rank "
    << "is not a small integer, but is instead "
    << rationalRank.toString()
    << ". "
    << global.fatal;
  }
  return result;
}

void DynkinType::getEpsilonMatrix(Matrix<Rational>& output) const {
  output.initialize(0, 0);
  Matrix<Rational> curCartan;
  List<DynkinSimpleType> sortedMons;
  this->getSortedDynkinTypes(sortedMons);
  for (int j = 0; j < sortedMons.size; j ++) {
    int index = this->monomials.getIndex(sortedMons[j]);
    int multiplicity = this->getMultiplicity(index);
    for (int k = 0; k < multiplicity; k ++) {
      DynkinSimpleType::getEpsilonMatrix((*this)[index].letter, (*this)[index].
        rank, curCartan
      );
      output.directSumWith(curCartan);
    }
  }
}

void DynkinType::sortDynkinTypes() {
  List<DynkinSimpleType> sortedTypes;
  this->getSortedDynkinTypes(sortedTypes);
  DynkinType finalMe;
  finalMe.makeZero();
  for (int i = 0; i < sortedTypes.size; i ++) {
    finalMe.addMonomial(
      sortedTypes[i], this->getCoefficientOf(sortedTypes[i])
    );
  }
  *this = finalMe;
}

void DynkinType::getSortedDynkinTypes(List<DynkinSimpleType>& output) const {
  output = this->monomials;
  output.quickSortDescending();
}

void DynkinType::getCartanSymmetric(Matrix<Rational>& output) const {
  STACK_TRACE("DynkinType::getCartanSymmetric");
  output.initialize(0, 0);
  Matrix<Rational> currentCartan;
  List<DynkinSimpleType> sortedMons;
  this->getSortedDynkinTypes(sortedMons);
  for (int j = 0; j < sortedMons.size; j ++) {
    int index = this->monomials.getIndex(sortedMons[j]);
    int multiplicity = this->getMultiplicity(index);
    for (int k = 0; k < multiplicity; k ++) {
      (*this)[index].getCartanSymmetric(currentCartan);
      output.directSumWith(currentCartan);
    }
  }
}

void DynkinType::getCoCartanSymmetric(Matrix<Rational>& output) const {
  STACK_TRACE("DynkinType::getCartanSymmetric");
  Matrix<Rational> curCartan;
  this->getCartanSymmetric(curCartan);
  WeylGroupData::getCoCartanSymmetric(curCartan, output);
}

void DynkinType::getCartanSymmetricDefaultLengthKeepComponentOrder(
  Matrix<Rational>& output
) const {
  STACK_TRACE("DynkinType::getCartanSymmetricDefaultLengthKeepComponentOrder");
  output.initialize(0, 0);
  Matrix<Rational> curCartan;
  List<DynkinSimpleType> sortedMons;
  this->getSortedDynkinTypes(sortedMons);
  DynkinSimpleType currentType;
  for (int j = 0; j < sortedMons.size; j ++) {
    int index = this->monomials.getIndex(sortedMons[j]);
    int mult = this->getMultiplicity(index);
    currentType.makeArbitrary(
      sortedMons[j].letter, sortedMons[j].rank, 1
    );
    currentType.cartanSymmetricInverseScale = 1;
    // = currentType.getDefaultCoRootLengthSquared(0);
    for (int k = 0; k < mult; k ++) {
      currentType.getCartanSymmetric(curCartan);
      output.directSumWith(curCartan);
    }
  }
}

int DynkinType::getCoxeterEdgeWeight(int v, int w) {
  if (v == w) {
    return 0;
  }
  Matrix<Rational> M;
  this->getCartanSymmetric(M);
  if (M(v, w) == 0) {
    return 2;
  }
  Rational c2 = M(v, w) * M(v, w) / M(v, v) / M(w, w);
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
  global.fatal
  <<
  "If you would like an edge weight of a non-crystallographic Coxeter graph, replace the code near "
  << __FILE__
  << ":"
  << __LINE__
  << " with an arccos function. "
  << global.fatal;
  return - 1;
}

LargeInteger DynkinType::getWeylGroupSizeByFormula() const {
  STACK_TRACE("DynkinType::getWeylGroupSizeByFormula");
  this->checkFlagDeallocated();
  LargeInteger result = 1;
  LargeInteger tempLI;
  for (int i = 0; i < this->size(); i ++) {
    tempLI =
    WeylGroupData::sizeByFormulaOrNegative1((*this)[i].letter, (*this)[i].rank
    );
    tempLI.raiseToPower(this->getMultiplicity(i));
    result *= tempLI;
  }
  if (result <= 0) {
    global.fatal
    << "Something has gone very wrong: Weyl group size reported to be "
    << result.toString()
    << " which appears to not be a positive integer! "
    << global.fatal;
  }
  return result;
}

Rational DynkinSimpleType::getLongRootLengthSquared() const {
  return
  this->getDefaultLongRootLengthSquared() /
  this->cartanSymmetricInverseScale;
}

Rational DynkinSimpleType::getDefaultLongRootLengthSquared() const {
  // Dynkin's convention says that the default long root length is 2.
  return 2;
}

Rational DynkinSimpleType::getEpsilonRealizationLongRootLengthSquared() const {
  // Dynkin's convention says that the default long root length is 2.
  // However, the accepted epsilon coordinate realizations of the root systems
  // of G_2 and C_n do not have long root length of 2.
  switch (this->letter) {
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
  global.fatal
  << "Calling "
  <<
  "DynkinSimpleType::getLongRootLengthSquared on a non-initialized simple type. "
  << global.fatal;
  return - 1;
}

std::string DynkinSimpleType::toStringNonTechnicalName(
  FormatExpressions* format
) const {
  (void) format;
  std::stringstream out;
  switch (this->letter) {
  case 'A':
    out << "sl(" << this->rank + 1 << ")";
    break;
  case 'B':
    out << "so(" << 2 * this->rank + 1 << ")";
    break;
  case 'C':
    out << "sp(" << 2 * this->rank << ")";
    break;
  case 'D':
    out << "so(" << 2 * this->rank << ")";
    break;
  default:
    break;
  }
  return out.str();
}

std::string DynkinSimpleType::toString(FormatExpressions* format) const {
  std::stringstream out;
  bool includeTechnicalNames = format ==
  nullptr ? true : format->flagIncludeLieAlgebraTypes;
  bool usePlusesAndExponents = format ==
  nullptr ? true : !format->flagDynkinTypeDontUsePlusAndExponent;
  bool supressDynkinIndexOne = format ==
  nullptr ? false : format->flagSupressDynkinIndexOne;
  bool hasAmbient = false;
  if (format != nullptr) {
    hasAmbient = (format->ambientWeylLetter != 'X');
  }
  if (includeTechnicalNames) {
    if (!hasAmbient) {
      out << letter;
      if (!supressDynkinIndexOne || this->cartanSymmetricInverseScale != 1) {
        if (usePlusesAndExponents) {
          out << "^{";
        }
        out << this->cartanSymmetricInverseScale.toString();
        if (usePlusesAndExponents) {
          out << "}";
        }
      }
    } else {
      DynkinSimpleType ambientType;
      ambientType.letter = format->ambientWeylLetter;
      ambientType.cartanSymmetricInverseScale =
      format->ambientCartanSymmetricInverseScale;
      Rational dynkinIndex = ambientType.getLongRootLengthSquared() /
      this->getLongRootLengthSquared();
      //      (this->CartanSymmetricInverseScale/this->getDefaultLongRootLengthSquared())/
      //      (ambientType.CartanSymmetricInverseScale/ambientType.getDefaultLongRootLengthSquared());
      out << letter;
      if (!supressDynkinIndexOne || dynkinIndex != 1) {
        if (usePlusesAndExponents) {
          out << "^{";
        }
        out << dynkinIndex.toString();
        if (usePlusesAndExponents) {
          out << "}";
        }
      }
    }
    if (this->rank >= 10) {
      out << "_";
      if (usePlusesAndExponents) {
        out << "{";
      }
      out << this->rank;
      if (usePlusesAndExponents) {
        out << "}";
      }
    } else {
      out << "_" << this->rank;
    }
  }
  return out.str();
}

void DynkinSimpleType::makeArbitrary(
  char inputLetter,
  int inputRank,
  const Rational& inputLengthFirstCorRootSquared
) {
  if ((
      inputLetter != 'A' &&
      inputLetter != 'B' &&
      inputLetter != 'C' &&
      inputLetter != 'D' &&
      inputLetter != 'E' &&
      inputLetter != 'F' &&
      inputLetter != 'G'
    ) ||
    inputRank <= 0
  ) {
    global.fatal
    << "Requested to create a simple Dynkin type of type "
    << inputLetter
    << " and rank "
    << inputRank
    << ". This is not allowed: I only accept types "
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
  this->rank = inputRank;
  this->letter = inputLetter;
  this->cartanSymmetricInverseScale = inputLengthFirstCorRootSquared;
}

int DynkinSimpleType::getRootSystemSize() const {
  switch (this->letter) {
  case 'A':
    return this->rank *(this->rank + 1);
  case 'B':
  case 'C':
    return this->rank * this->rank * 2;
  case 'D':
    return this->rank *(this->rank - 1) * 2;
  case 'E':
    switch (this->rank) {
    case 6:
      return 72;
    case 7:
      return 126;
    case 8:
      return 240;
    default:
      return - 1;
    }
  case 'G':
    return 12;
  case 'F':
    return 48;
  default:
    return - 1;
  }
}

Rational DynkinSimpleType::getDefaultCoRootLengthSquared(int rootIndex) const {
  Rational result = this->getDefaultRootLengthSquared(rootIndex);
  result.invert();
  result *= 4;
  return result;
}

Rational DynkinSimpleType::getRatioRootSquaredToFirstSquared(int rootIndex)
const {
  Rational result;
  switch (this->letter) {
  case 'A':
  case 'D':
  case 'E':
    return 1;
  case 'B':
    if (rootIndex == this->rank - 1) {
      result.assignNumeratorAndDenominator(1, 2);
      return result;
    }
    return 1;
  case 'C':
    if (rootIndex == this->rank - 1) {
      return 2;
    }
    return 1;
  case 'F':
    if (rootIndex < 2) {
      return 1;
    }
    result.assignNumeratorAndDenominator(1, 2);
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

Rational DynkinSimpleType::getDefaultRootLengthSquared(int rootIndex) const {
  if (rootIndex >= this->rank) {
    global.fatal
    << "Attempt to get the squared length of simple root number "
    << rootIndex + 1
    << ", however the root system if of rank "
    << this->rank
    << ". "
    << global.fatal;
  }
  switch (this->letter) {
  case 'A':
  case 'D':
  case 'E':
    return 2;
  case 'B':
    if (rootIndex == this->rank - 1) {
      return 1;
    }
    return 2;
  case 'F':
    if (rootIndex < 2) {
      return 2;
    }
    return 1;
  case 'C':
    if (rootIndex == this->rank - 1) {
      return 2;
    }
    return 1;
  case 'G':
    if (rootIndex == 1) {
      return 2;
    }
    return Rational(2, 3);
  default:
    global.fatal
    << "Calling "
    <<
    "DynkinSimpleType::getDefaultRootLengthSquared on the non-initialized Dynkin type "
    << this->toString()
    << global.fatal;
    return - 1;
  }
}

void DynkinSimpleType::getEpsilonMatrix(
  char weylLetter, int weylRank, Matrix<Rational>& output
) {
  if (weylLetter == 'A') {
    output.initialize(weylRank + 1, weylRank);
    output.makeZero();
    for (int i = 0; i < weylRank; i ++) {
      output(i, i) = 1;
      output(i + 1, i) = - 1;
    }
    output(weylRank, weylRank - 1) = - 1;
  }
  if (weylLetter == 'B') {
    output.initialize(weylRank, weylRank);
    output.makeZero();
    for (int i = 0; i < weylRank - 1; i ++) {
      output(i, i) = 1;
      output(i + 1, i) = - 1;
    }
    output(weylRank - 1, weylRank - 1) = 1;
  }
  if (weylLetter == 'C') {
    output.initialize(weylRank, weylRank);
    output.makeZero();
    for (int i = 0; i < weylRank - 1; i ++) {
      output(i, i) = 1;
      output(i + 1, i) = - 1;
    }
    output(weylRank - 1, weylRank - 1) = 2;
  }
  if (weylLetter == 'D') {
    output.initialize(weylRank, weylRank);
    output.makeZero();
    for (int i = 0; i < weylRank - 1; i ++) {
      output(i, i) = 1;
      output(i + 1, i) = - 1;
    }
    output(weylRank - 1, weylRank - 1) = 1;
    output(weylRank - 2, weylRank - 1) = 1;
  }
  Rational half(1, 2);
  Rational negativeHalf(- 1, 2);
  if (weylLetter == 'E') {
    // Epsilon convention taken with slight modification from
    // Humpreys, Introduction to Lie algebras and representation theory, page
    // 65
    // first comes first root, then the sticky part, then string with the rest
    // of the roots.
    output.initialize(8, weylRank);
    output.makeZero();
    // first simple root: - 1/2e_1- 1/2e_8+ 1/2e_2+ 1/2e_3+ 1/2e_4+ 1/2e_5+
    // 1/2e_6+ 1/2e_7
    output(0, 0) = negativeHalf;
    output(1, 0) = half;
    output(2, 0) = half;
    output(3, 0) = half;
    output(4, 0) = half;
    output(5, 0) = half;
    output(6, 0) = half;
    output(7, 0) = negativeHalf;
    // 2nd simple root: -e_1-e_2 (that is the sticky piece of the Dynkin
    // diagram)
    output(0, 1) = - 1;
    output(1, 1) = - 1;
    // 3rd simple root: e_1-e_2
    output(0, 2) = 1;
    output(1, 2) = - 1;
    // 4th simple root: e_2-e_3
    output(1, 3) = 1;
    output(2, 3) = - 1;
    // 5th simple root: e_3-e_4
    output(2, 4) = 1;
    output(3, 4) = - 1;
    // 6th simple root: e_4-e_5
    output(3, 5) = 1;
    output(4, 5) = - 1;
    if (weylRank > 6) {
      // 7th simple root: e_5-e_6
      output(4, 6) =
      1;
      output(5, 6) = - 1;
    }
    if (weylRank > 7) {
      // 8th simple root: e_6-e_7
      output(5, 7) =
      1;
      output(6, 7) = - 1;
    }
  }
  if (weylLetter == 'F') {
    // as of May 11 2013 the convention has been changed to coincide with that
    // of
    // Wikipedia
    output.initialize(4, 4);
    output.makeZero();
    // image of first simple root = e_1 - e_2 (long one):
    output(0, 0) = 1;
    output(1, 0) = - 1;
    // image of second simple root = e_2 - e_3 (long one)
    output(1, 1) = 1;
    output(2, 1) = - 1;
    // image of third simple root = e_3 (short one):
    output(2, 2) = 1;
    // image of fourth simple root (short one)
    output(0, 3) = negativeHalf;
    output(1, 3) = negativeHalf;
    output(2, 3) = negativeHalf;
    output(3, 3) = negativeHalf;
    // eps_2:
    // eps_4:
  }
  if (weylLetter == 'G') {
    // taken from Humpreys, Introduction to Lie algebras and representation
    // theory, page 65
    // the long root has the higher index
    output.initialize(3, 2);
    output.makeZero();
    // image of the first simple root(short one):
    output(0, 0) = 1;
    output(1, 0) = - 1;
    // image of second simple root:
    output(0, 1) = - 2;
    output(1, 1) = 1;
    output(2, 1) = 1;
  }
}

void DynkinSimpleType::getAn(int n, Matrix<Rational>& output) const {
  if (n <= 0 || n > 30000) {
    global.fatal
    << "Attempt to create type A_n with n ="
    << n
    << " is illegal. If this was a bad user input, it should "
    << " be handled at an earlier stage. "
    << global.fatal;
  }
  output.initialize(n, n);
  output.makeZero();
  for (int i = 0; i < n - 1; i ++) {
    output(i, i) = 2;
    output(i + 1, i) = - 1;
    output(i, i + 1) = - 1;
  }
  output(n - 1, n - 1) = 2;
}

void DynkinSimpleType::getBn(int n, Matrix<Rational>& output) const {
  this->getAn(n, output);
  output(n - 1, n - 1) = 1;
}

Rational DynkinSimpleType::getDynkinIndexParabolicallyInducingSubalgebra(
  char inputType
) {
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
    global.fatal
    << "DynkinSimpleType::getDynkinIndexParabolicallyInducingSubalgebra "
    << "called with input "
    << inputType
    << ", this is not allowed. "
    << global.fatal;
    return - 1;
  }
}

bool DynkinSimpleType::canBeExtendedParabolicallyTo(
  const DynkinSimpleType& other
) const {
  STACK_TRACE("DynkinSimpleType::canBeExtendedParabolicallyTo");
  if (other.rank <= this->rank) {
    return false;
  }
  if (
    this->cartanSymmetricInverseScale /
    this->getDynkinIndexParabolicallyInducingSubalgebra(other.letter) !=
    other.cartanSymmetricInverseScale
  ) {
    return false;
  }
  if (other.letter == 'F') {
    if (this->letter == 'A' && this->rank < 3) {
      return true;
    }
    if (this->letter == 'B' && this->rank == 3) {
      return true;
    }
    return false;
  }
  if (other.letter == 'E') {
    if (this->rank < 5) {
      return this->letter == 'A';
    }
    if (this->rank == 5) {
      return this->letter == 'D';
    }
    return this->letter == 'E';
  }
  return this->letter == 'A';
}

void DynkinSimpleType::grow(
  List<DynkinSimpleType>& output,
  List<List<int> >* outputPermutationRoots
) const {
  STACK_TRACE("DynkinSimpleType::grow");
  // Almost all simple types are grown from type A. Exceptions only for types
  // F4
  // (grown from B_3),
  // E6 (grown from D_5), E7 (grown from E6) and E8 (grown from E7).
  output.setSize(0);
  List<int> currentImagesSimpleRootsCurrent;
  if (outputPermutationRoots != nullptr) {
    outputPermutationRoots->setSize(0);
  }
  currentImagesSimpleRootsCurrent.setSize(this->rank + 1);
  DynkinSimpleType newType;
  if (this->letter == 'B' && this->rank == 3) {
    newType.makeArbitrary('F', 4, this->cartanSymmetricInverseScale);
    output.addOnTop(newType);
    if (outputPermutationRoots != nullptr) {
      for (int i = 0; i < currentImagesSimpleRootsCurrent.size; i ++) {
        currentImagesSimpleRootsCurrent[i] = i;
      }
      outputPermutationRoots->addOnTop(currentImagesSimpleRootsCurrent);
    }
  }
  if (this->letter == 'D' && this->rank == 5) {
    newType.makeArbitrary('E', 6, this->cartanSymmetricInverseScale);
    output.addOnTop(newType);
    if (outputPermutationRoots != nullptr) {
      currentImagesSimpleRootsCurrent[0] = 0;
      currentImagesSimpleRootsCurrent[1] = 2;
      currentImagesSimpleRootsCurrent[2] = 3;
      currentImagesSimpleRootsCurrent[3] = 4;
      currentImagesSimpleRootsCurrent[4] = 1;
      currentImagesSimpleRootsCurrent[5] = 5;
      outputPermutationRoots->addOnTop(currentImagesSimpleRootsCurrent);
    }
  }
  if (this->letter == 'E' && this->rank < 8) {
    newType.makeArbitrary(
      'E', this->rank + 1, this->cartanSymmetricInverseScale
    );
    output.addOnTop(newType);
    if (outputPermutationRoots != nullptr) {
      for (int i = 0; i < currentImagesSimpleRootsCurrent.size; i ++) {
        currentImagesSimpleRootsCurrent[i] = i;
      }
      outputPermutationRoots->addOnTop(currentImagesSimpleRootsCurrent);
    }
  }
  if (this->letter != 'A') {
    return;
  }
  newType.makeArbitrary(
    this->letter, this->rank + 1, this->cartanSymmetricInverseScale
  );
  output.addOnTop(newType);
  if (outputPermutationRoots != nullptr) {
    for (int i = 0; i < currentImagesSimpleRootsCurrent.size; i ++) {
      currentImagesSimpleRootsCurrent[i] = i;
    }
    outputPermutationRoots->addOnTop(currentImagesSimpleRootsCurrent);
  }
  newType.makeArbitrary(
    'B', this->rank + 1, this->cartanSymmetricInverseScale
  );
  output.addOnTop(newType);
  if (outputPermutationRoots != nullptr) {
    outputPermutationRoots->addOnTop(currentImagesSimpleRootsCurrent);
  }
  if (this->rank > 1) {
    newType.makeArbitrary(
      'C', this->rank + 1, this->cartanSymmetricInverseScale / 2
    );
    output.addOnTop(newType);
    if (outputPermutationRoots != nullptr) {
      outputPermutationRoots->addOnTop(currentImagesSimpleRootsCurrent);
    }
  }
  if (this->rank > 2) {
    newType.makeArbitrary(
      'D', this->rank + 1, this->cartanSymmetricInverseScale
    );
    output.addOnTop(newType);
    if (outputPermutationRoots != nullptr) {
      outputPermutationRoots->addOnTop(currentImagesSimpleRootsCurrent);
    }
  }
  if (this->rank == 1) {
    newType.makeArbitrary('G', 2, this->cartanSymmetricInverseScale / 3);
    output.addOnTop(newType);
    if (outputPermutationRoots != nullptr) {
      outputPermutationRoots->addOnTop(currentImagesSimpleRootsCurrent);
    }
  }
}

bool DynkinSimpleType::operator>(const DynkinSimpleType& other) const {
  if (this->rank > other.rank) {
    return true;
  }
  if (this->rank < other.rank) {
    return false;
  }
  if (this->cartanSymmetricInverseScale > other.cartanSymmetricInverseScale) {
    return true;
  }
  if (this->cartanSymmetricInverseScale < other.cartanSymmetricInverseScale) {
    return false;
  }
  if ((this->letter == 'B' || this->letter == 'C') && other.letter == 'D'
  ) {
    return true;
  }
  if (
    this->letter == 'D' && (other.letter == 'B' || other.letter == 'C')
  ) {
    return false;
  }
  return this->letter > other.letter;
}

void DynkinSimpleType::getCn(int n, Matrix<Rational>& output) const {
  this->getAn(n, output);
  if (n < 2) {
    return;
  }
  output(n - 1, n - 1) = 4;
  output(n - 2, n - 1) = - 2;
  output(n - 1, n - 2) = - 2;
  output /= 2;
}

void DynkinSimpleType::getDn(int n, Matrix<Rational>& output) const {
  this->getAn(n, output);
  if (n < 3) {
    return;
  }
  output(n - 1, n - 2) = 0;
  output(n - 2, n - 1) = 0;
  output(n - 3, n - 1) = - 1;
  output(n - 1, n - 3) = - 1;
}

void DynkinSimpleType::getEn(int n, Matrix<Rational>& output) const {
  this->getAn(n, output);
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

void DynkinSimpleType::getF4(Matrix<Rational>& output) const {
  output.initialize(4, 4);
  Rational mh(- 1, 2);
  output(0, 0) = 2;
  output(0, 1) = - 1;
  output(0, 2) = 0;
  output(0, 3) = 0;
  output(1, 0) = - 1;
  output(1, 1) = 2;
  output(1, 2) = - 1;
  output(1, 3) = 0;
  output(2, 0) = 0;
  output(2, 1) = - 1;
  output(2, 2) = 1;
  output(2, 3) = mh;
  output(3, 0) = 0;
  output(3, 1) = 0;
  output(3, 2) = mh;
  output(3, 3) = 1;
}

void DynkinSimpleType::getG2(Matrix<Rational>& output) const {
  output.initialize(2, 2);
  output(0, 0) = 2;
  output(1, 1) = 6;
  output(1, 0) = - 3;
  output(0, 1) = - 3;
  output /= 3;
}

void DynkinSimpleType::getCoCartanSymmetric(Matrix<Rational>& output) const {
  Matrix<Rational> currentCartan;
  this->getCartanSymmetric(currentCartan);
  WeylGroupData::getCoCartanSymmetric(currentCartan, output);
}

Rational DynkinSimpleType::getPrincipalSlTwoCartanSymmetricInverseScale() const {
  STACK_TRACE(
    "DynkinSimpleType::getPrincipalSlTwoCartanSymmetricInverseScale"
  );
  // Reference: Panyushev, On the Dynkin index of a principal sl(2)-subalgebra,
  // Advances in Mathematics, 2008.
  Rational nonScaled = 0;
  switch (this->letter) {
  case 'A':
    nonScaled = (this->rank + 2) *(this->rank + 1) * this->rank / 6;
    break;
  case 'B':
    nonScaled = this->rank *(this->rank + 1) *(2 * this->rank + 1) / 3;
    break;
  case 'C':
    nonScaled = (this->rank * 2 + 1) *(this->rank * 2) *(this->rank * 2 - 1) /
    6;
    break;
  case 'D':
    nonScaled = (this->rank - 1) * this->rank *(2 * this->rank - 1) / 3;
    break;
  case 'E':
    if (this->rank == 6) {
      nonScaled = 156;
    }
    if (this->rank == 7) {
      nonScaled = 399;
    }
    if (this->rank == 8) {
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
    global.fatal
    << "Request DynkinSimpleType::getCartanSymmetric "
    << "from a non-initialized Dynkin simple type. "
    << global.fatal;
    break;
  }
  return nonScaled * this->cartanSymmetricInverseScale;
}

void DynkinSimpleType::getCartanSymmetric(Matrix<Rational>& output) const {
  switch (this->letter) {
  case 'A':
    this->getAn(this->rank, output);
    break;
  case 'B':
    this->getBn(this->rank, output);
    break;
  case 'C':
    this->getCn(this->rank, output);
    break;
  case 'D':
    this->getDn(this->rank, output);
    break;
  case 'E':
    this->getEn(this->rank, output);
    break;
  case 'F':
    this->getF4(output);
    break;
  case 'G':
    this->getG2(output);
    break;
  default:
    global.fatal
    << "Request "
    << "DynkinSimpleType::getCartanSymmetric "
    << "from a non-initialized Dynkin simple type. "
    << global.fatal;
    break;
  }
  output /= this->cartanSymmetricInverseScale;
}

Rational DynkinSimpleType::getRatioLongRootToFirst(
  char inputWeylLetter, int inputRank
) {
  (void) inputRank;
  switch (inputWeylLetter) {
  case 'A':
    return 1;
  case 'B':
    return 1;
  case 'C':
    return 2;
  case 'D':
    return 1;
  case 'E':
    return 1;
  case 'F':
    return 1;
  case 'G':
    return 3;
  default:
    return - 1;
  }
}

void DynkinSimpleType::operator++(int) {
  if (this->rank == 1) {
    this->rank ++;
    return;
  }
  if (this->letter == 'A') {
    if (this->rank >= 4) {
      this->letter = 'D';
    } else {
      this->letter = 'B';
    }
    return;
  }
  if (this->letter == 'D') {
    this->letter = 'B';
    return;
  }
  if (this->letter == 'B') {
    if (this->rank >= 3) {
      this->letter = 'C';
    } else {
      this->letter = 'G';
    }
    return;
  }
  if (this->letter == 'C') {
    if (this->rank == 4) {
      this->letter = 'F';
      return;
    }
    if (this->rank == 6 || this->rank == 7 || this->rank == 8) {
      this->letter = 'E';
      return;
    }
    this->letter = 'A';
    this->rank ++;
    return;
  }
  if (this->letter == 'G' || this->letter == 'F' || this->letter == 'E') {
    this->rank ++;
    this->letter = 'A';
    return;
  }
  global.fatal
  << "This is a portion of code that should never be reached. "
  << "Something has gone very wrong. "
  << global.fatal;
}

bool DynkinSimpleType::operator<(int otherRank) const {
  return this->rank < otherRank;
}

ElementWeylGroupAutomorphisms::ElementWeylGroupAutomorphisms() {
  this->owner = nullptr;
  this->flagDeallocated = false;
}

ElementWeylGroupAutomorphisms::~ElementWeylGroupAutomorphisms() {
  this->owner = nullptr;
  this->flagDeallocated = true;
}

bool ElementWeylGroupAutomorphisms::checkInitialization() const {
  if (this->owner == nullptr) {
    global.fatal
    << "Element of Weyl group automorphisms has zero owner. "
    << global.fatal;
  }
  return true;
}

void ElementWeylGroupAutomorphisms::makeIdentity(
  WeylGroupAutomorphisms& inputAutomorphisms
) {
  this->owner = &inputAutomorphisms;
  this->generatorsLastAppliedFirst.setSize(0);
}

void ElementWeylGroupAutomorphisms::multiplyOnTheRightByOuterAuto(
  int outerAutoIndex
) {
  SimpleReflectionOrOuterAutomorphism generator;
  generator.index = outerAutoIndex;
  generator.flagIsOuter = true;
  this->generatorsLastAppliedFirst.addOnTop(generator);
}

void ElementWeylGroupAutomorphisms::makeSimpleReflection(
  int simpleRootIndex, WeylGroupAutomorphisms& inputWeyl
) {
  this->owner = &inputWeyl;
  this->generatorsLastAppliedFirst.setSize(1);
  this->generatorsLastAppliedFirst[0].makeSimpleReflection(simpleRootIndex);
}

void ElementWeylGroupAutomorphisms::makeOuterAuto(
  int outerAutoIndex, WeylGroupAutomorphisms& inputWeyl
) {
  this->owner = &inputWeyl;
  this->generatorsLastAppliedFirst.setSize(1);
  this->generatorsLastAppliedFirst[0].makeOuterAuto(outerAutoIndex);
}

unsigned int ElementWeylGroupAutomorphisms::hashFunction(
  const ElementWeylGroupAutomorphisms& input
) {
  return input.hashFunction();
}

unsigned int ElementWeylGroupAutomorphisms::hashFunction() const {
  return this->generatorsLastAppliedFirst.hashFunction();
}

std::string ElementWeylGroupAutomorphisms::toString(FormatExpressions* format)
const {
  STACK_TRACE("ElementWeylGroupAutomorphisms::toString");
  (void) format;
  if (this->generatorsLastAppliedFirst.size == 0) {
    return "id";
  }
  std::stringstream out;
  for (int i = 0; i < this->generatorsLastAppliedFirst.size; i ++) {
    out << this->generatorsLastAppliedFirst[i].toString();
  }
  return out.str();
}

void
ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms
::multiplyOnTheRightBySimpleRootInner(int simpleRootIndex) {
  SimpleReflectionOrOuterAutomorphism generator;
  generator.index = simpleRootIndex;
  this->generatorsLastAppliedFirst.addOnTop(generator);
}

void
ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms
::multiplyOnTheRightByOuterAutomorphism(int indexOuterAutomorphism) {
  SimpleReflectionOrOuterAutomorphism generator;
  generator.index = indexOuterAutomorphism;
  generator.flagIsOuter = true;
  this->generatorsLastAppliedFirst.addOnTop(generator);
}

unsigned int
ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms
::hashFunction(
  const
  ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms
  & input
) {
  return input.hashFunction();
}

unsigned int
ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms
::hashFunction() const {
  return this->generatorsLastAppliedFirst.hashFunction();
}

std::string
ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms
::toString(FormatExpressions* format) const {
  STACK_TRACE(
    "ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::toString"
  );
  (void) format;
  if (this->generatorsLastAppliedFirst.size == 0) {
    return "id";
  }
  std::stringstream out;
  for (int i = 0; i < this->generatorsLastAppliedFirst.size; i ++) {
    out << this->generatorsLastAppliedFirst[i].toString();
  }
  return out.str();
}

bool ElementWeylGroup::checkConsistency() const {
  if (this->flagDeallocated) {
    global.fatal << "Use after free of class ElementWeylGroup" << global.fatal;
  }
  return true;
}

bool ElementWeylGroup::checkInitialization() const {
  this->checkConsistency();
  if (this->owner == nullptr) {
    global.fatal << "Non-initialized element Weyl group. " << global.fatal;
    return false;
  }
  this->owner->checkConsistency();
  return true;
}

int ElementWeylGroup::sign() const {
  return this->generatorsLastAppliedFirst.size % 2 == 0 ? 1 : - 1;
}

ElementWeylGroup ElementWeylGroup::operator^(const ElementWeylGroup& right)
const {
  if (this->owner != right.owner) {
    global.fatal
    << "Not allowed to conjugate elements of different Weyl groups. "
    << "If you did this intentionally, change the present file. "
    << global.fatal;
  }
  ElementWeylGroup out;
  out.owner = this->owner;
  out.generatorsLastAppliedFirst = right.generatorsLastAppliedFirst;
  out.generatorsLastAppliedFirst.addListOnTop(
    this->generatorsLastAppliedFirst
  );
  for (int i = right.generatorsLastAppliedFirst.size - 1; i >= 0; i --) {
    out.generatorsLastAppliedFirst.addOnTop(
      right.generatorsLastAppliedFirst[i]
    );
  }
  out.makeCanonical();
  return out;
}

void ElementWeylGroup::conjugationAction(
  const ElementWeylGroup& ConjugateWith,
  const ElementWeylGroup& ConjugateOn,
  ElementWeylGroup& out
) {
  out = ConjugateOn ^ ConjugateWith;
}

void ElementWeylGroup::operator*=(const ElementWeylGroup& other) {
  if (this->owner != other.owner) {
    global.fatal
    << "Attemptto "
    << "multiply elements belonging to different Weyl groups. "
    << global.fatal;
  }
  if (&other == this) {
    ElementWeylGroup otherCopy = other;
    (*this) *= otherCopy;
    return;
  }
  this->generatorsLastAppliedFirst.addListOnTop(
    other.generatorsLastAppliedFirst
  );
  this->makeCanonical();
}

Vector<Rational> ElementWeylGroup::operator*(const Vector<Rational>& v) const {
  Vector<Rational> out = v;
  this->owner->actOn(*this, out);
  return out;
}

void ElementWeylGroup::makeIdentity(WeylGroupData& inputWeyl) {
  this->owner = &inputWeyl;
  this->generatorsLastAppliedFirst.setSize(0);
}

void ElementWeylGroup::makeIdentity(
  const FiniteGroup<ElementWeylGroup>& inputGroup
) {
  this->owner = inputGroup.generators[0].owner;
  this->generatorsLastAppliedFirst.setSize(0);
}

void ElementWeylGroup::makeIdentity(const ElementWeylGroup& initializeFrom) {
  this->makeIdentity(*initializeFrom.owner);
}

bool ElementWeylGroup::isIdentity() {
  this->makeCanonical();
  return this->generatorsLastAppliedFirst.size == 0;
}

void ElementWeylGroup::MakeRootReflection(
  const Vector<Rational>& mustBeRoot, WeylGroupData& inputWeyl
) {
  *this =
  inputWeyl.getRootReflection(
    inputWeyl.rootSystem.getIndexNoFail(mustBeRoot)
  );
}

void ElementWeylGroup::makeSimpleReflection(
  int simpleRootIndex, WeylGroupData& inputWeyl
) {
  this->owner = &inputWeyl;
  this->generatorsLastAppliedFirst.setSize(1);
  this->generatorsLastAppliedFirst[0].makeSimpleReflection(simpleRootIndex);
}

void ElementWeylGroup::makeFromRhoImage(
  const Vector<Rational>& inputRhoImage, WeylGroupData& inputWeyl
) {
  this->owner = &inputWeyl;
  int rank = this->owner->getDimension();
  this->generatorsLastAppliedFirst.setSize(0);
  Vector<Rational> rhoImage = inputRhoImage;
  SimpleReflection generator;
  while (rhoImage != this->owner->rho) {
    for (int i = 0; i < rank; i ++) {
      if (this->owner->getScalarProductSimpleRoot(rhoImage, i) < 0) {
        this->owner->reflectSimple(i, rhoImage);
        generator.makeSimpleReflection(i);
        this->generatorsLastAppliedFirst.addOnTop(generator);
        break;
      }
    }
  }
}

void ElementWeylGroup::makeCanonical() {
  STACK_TRACE("ElementWeylGroup::makeCanonical");
  this->checkInitialization();
  if (this->owner->rho.size == 0) {
    this->owner->computeRho(false);
  }
  Vector<Rational> workingVector;
  this->owner->actOn(*this, this->owner->rho, workingVector);
  this->makeFromRhoImage(workingVector, *this->owner);
}

bool ElementWeylGroup::hasDifferentConjugacyInvariantsFrom(
  const ElementWeylGroup& right
) const {
  STACK_TRACE("ElementWeylGroup::hasDifferentConjugacyInvariantsFrom");
  if ((this->generatorsLastAppliedFirst.size % 2) != (
      right.generatorsLastAppliedFirst.size % 2
    )
  ) {
    return true;
  }
  if (*this == right) {
    // just in case
    return false;
  }
  this->checkInitialization();
  Polynomial<Rational> leftCharPoly, rightCharPoly;
  this->getCharacteristicPolynomialStandardRepresentation(leftCharPoly);
  right.getCharacteristicPolynomialStandardRepresentation(rightCharPoly);
  if (leftCharPoly != rightCharPoly) {
    return true;
  }
  VectorSparse<Rational> leftCycleStructure, rightCycleStructure;
  this->getCycleStructure(leftCycleStructure);
  right.getCycleStructure(rightCycleStructure);
  if (leftCycleStructure != rightCycleStructure) {
    return true;
  }
  return false;
}

std::string ElementWeylGroup::toStringInvariants(FormatExpressions* format)
const {
  STACK_TRACE("ElementWeylGroup::getCycleStructure");
  (void) format;
  VectorSparse<Rational> cycleStructure;
  this->getCycleStructure(cycleStructure);
  FormatExpressions cycleLetterFormat;
  cycleLetterFormat.polynomialDefaultLetter = "c";
  std::stringstream out;
  out
  << "Cycle structure: "
  << cycleStructure.toString(&cycleLetterFormat)
  << ". ";
  return out.str();
}

void ElementWeylGroup::getCycleStructure(
  VectorSparse<Rational>& outputIndexIsCycleSizeCoordinateIsCycleMult
) const {
  STACK_TRACE("ElementWeylGroup::getCycleStructure");
  this->checkInitialization();
  outputIndexIsCycleSizeCoordinateIsCycleMult.makeZero();
  List<bool> explored;
  HashedList<Vector<Rational> >& rootSystem = this->owner->rootSystem;
  explored.initializeFillInObject(rootSystem.size, false);
  Vector<Rational> currentRoot;
  for (int i = 0; i < explored.size; i ++) {
    if (!explored[i]) {
      int currentCycleSize = 1;
      currentRoot = rootSystem[i];
      for (
        this->owner->actOn(*this, currentRoot, currentRoot); currentRoot !=
        rootSystem[i]; this->owner->actOn(*this, currentRoot, currentRoot)
      ) {
        currentCycleSize ++;
        explored[rootSystem.getIndex(currentRoot)] = true;
      }
      outputIndexIsCycleSizeCoordinateIsCycleMult.addMonomial(
        MonomialVector(currentCycleSize - 1), 1
      );
    }
  }
}

void ElementWeylGroup::getCharacteristicPolynomialStandardRepresentation(
  Polynomial<Rational>& output
) const {
  STACK_TRACE(
    "ElementWeylGroup::getCharacteristicPolynomialStandardRepresentation"
  );
  this->checkInitialization();
  Matrix<Rational> standardRepMat;
  this->owner->getMatrixStandardRepresentation(*this, standardRepMat);
  output.assignCharacteristicPolynomial(standardRepMat);
}

ElementWeylGroup ElementWeylGroup::inverse() const {
  ElementWeylGroup out = *this;
  out.generatorsLastAppliedFirst.reverseElements();
  out.makeCanonical();
  return out;
}

std::string ElementWeylGroup::toString(FormatExpressions* format) const {
  STACK_TRACE("ElementWeylGroup::toString");
  (void) format;
  if (this->generatorsLastAppliedFirst.size == 0) {
    return "id";
  }
  std::stringstream out;
  for (int i = 0; i < this->generatorsLastAppliedFirst.size; i ++) {
    out << this->generatorsLastAppliedFirst[i].toString();
  }
  return out.str();
}

unsigned int ElementWeylGroup::hashFunction() const {
  unsigned int result = 0;
  int j = 0;
  for (int i = 0; i < this->generatorsLastAppliedFirst.size; i ++) {
    result +=
    this->generatorsLastAppliedFirst[i].hashFunction() *
    HashConstants::getConstantIncrementCounter(j);
  }
  return result;
}

bool ElementWeylGroup::operator>(const ElementWeylGroup& other) const {
  if (this->owner != other.owner) {
    global.fatal
    << "Comparing elements of different Weyl groups. "
    << global.fatal;
  }
  return this->generatorsLastAppliedFirst > other.generatorsLastAppliedFirst;
}

void ElementWeylGroup::multiplyOnTheRightBySimpleReflection(
  int reflectionIndex
) {
  SimpleReflection simpleReflection;
  simpleReflection.makeSimpleReflection(reflectionIndex);
  this->generatorsLastAppliedFirst.addOnTop(simpleReflection);
}

void WeylGroupData::simpleReflectionDualSpace(
  int index, Vector<Rational>& dualSpaceElement
) {
  Rational coefficient;
  Rational scalarProduct;
  coefficient.assign(dualSpaceElement[index]);
  coefficient.divideBy(this->cartanSymmetric.elements[index][index]);
  for (int i = 0; i < this->cartanSymmetric.numberOfColumns; i ++) {
    scalarProduct.assign(coefficient);
    scalarProduct.multiplyBy(
      this->cartanSymmetric.elements[index][i] *(- 2)
    );
    dualSpaceElement[i] += scalarProduct;
  }
}

ElementWeylGroup WeylGroupData::getRootReflection(int rootIndex) {
  if (this->rho.size == 0) {
    this->computeRho(true);
  }
  Vector<Rational> rhoImage;
  this->reflectBetaWithRespectToAlpha(
    this->rootSystem[rootIndex], this->rho, false, rhoImage
  );
  ElementWeylGroup result;
  result.makeFromRhoImage(rhoImage, *this);
  return result;
}

void WeylGroupData::reflectSimple(
  int index, Vector<Rational>& root, bool rhoAction, bool useMinusRho
) {
  Rational alphaShift;
  Rational scalar;
  alphaShift.makeZero();
  for (int i = 0; i < this->cartanSymmetric.numberOfColumns; i ++) {
    scalar = root[i];
    scalar.multiplyBy(
      this->cartanSymmetric.elements[index][i] *(- 2)
    );
    alphaShift += scalar;
  }
  alphaShift.divideBy(this->cartanSymmetric.elements[index][index]);
  if (rhoAction) {
    if (useMinusRho) {
      alphaShift.addInteger(1);
    } else {
      alphaShift.addInteger(- 1);
    }
  }
  root[index] += alphaShift;
}

void WeylGroupData::simpleReflectionRootPolynomial(
  int index, PolynomialSubstitution<Rational>& root, bool rhoAction
) {
  int lengthA = this->cartanSymmetric.elements[index][index].numeratorShort;
  Polynomial<Rational> aScalarB, tempP;
  aScalarB.makeZero();
  for (int i = 0; i < this->cartanSymmetric.numberOfColumns; i ++) {
    tempP.makeZero();
    tempP = root[i];
    tempP *= cartanSymmetric.elements[index][i];
    aScalarB += (tempP);
  }
  aScalarB *= - 2;
  aScalarB /= lengthA;
  root[index] += aScalarB;
  if (rhoAction) {
    root[index] += Rational(- 1);
  }
}

Matrix<Rational> WeylGroupData::getMatrixStandardRepresentation(
  int elementIndex
) const {
  Matrix<Rational> result;
  this->getMatrixStandardRepresentation(
    this->group.elements[elementIndex], result
  );
  return result;
}

void WeylGroupData::reset() {
  this->fundamentalToSimpleCoords.initialize(0, 0);
  this->simpleToFundamentalCoords.initialize(0, 0);
  this->matrixSendsSimpleVectorsToEpsilonVectors.freeMemory();
  this->flagFundamentalToSimpleMatricesAreComputed = false;
  this->flagIrrepsAreComputed = false;
  this->flagCharTableIsComputed = false;
  this->dynkinType.makeZero();
  this->cartanSymmetric.initialize(0, 0);
  this->coCartanSymmetric.initialize(0, 0);
  this->rho.setSize(0);
  this->rhoOrbit.clear();
  this->rootSystem.clear();
  this->rootsOfBorel.setSize(0);
  this->ccCarterLabels.setSize(0);
  this->irrepsCarterLabels.setSize(0);
  this->group.initialize();
  this->group.specificDataPointer = this;
  this->group.getWordByFormula = this->getWordByFormulaImplementation;
  this->group.getSizeByFormula = this->getSizeByFormulaImplementation;
  this->group.computeConjugacyClassSizesAndRepresentativesByFormula = nullptr;
  this->group.areConjugateByFormula = nullptr;
  this->group.ComputeIrreducibleRepresentationsWithFormulas = nullptr;
}

void WeylGroupData::actOnAffineHyperplaneByGroupElement(
  int index,
  AffineHyperplane<Rational>& output,
  bool rhoAction,
  bool useMinusRho
) {
  int numGens = this->group.elements[index].generatorsLastAppliedFirst.size;
  for (int i = numGens - 1; i >= 0; i --) {
    this->reflectSimple(
      this->group.elements[index].generatorsLastAppliedFirst[i].index,
      output.affinePoint,
      rhoAction,
      useMinusRho
    );
    this->simpleReflectionDualSpace(
      this->group.elements[index].generatorsLastAppliedFirst[numGens - i - 1].
      index,
      output.normal
    );
  }
}

bool WeylGroupData::getWordByFormulaImplementation(
  FiniteGroup<ElementWeylGroup>& group,
  const ElementWeylGroup& g,
  List<int>& out
) {
  (void) group;
  // avoid unused parameter warning, portable.
  out.setSize(g.generatorsLastAppliedFirst.size);
  for (int i = 0; i < g.generatorsLastAppliedFirst.size; i ++) {
    out[i] = g.generatorsLastAppliedFirst[i].index;
  }
  return true;
}

void WeylGroupData::getSignCharacter(Vector<Rational>& out) {
  if (!this->group.flagCCRepresentativesComputed) {
    this->group.computeConjugacyClassSizesAndRepresentatives();
  }
  out.setSize(this->group.conjugacyClassCount());
  for (int i = 0; i < this->group.conjugacyClassCount(); i ++) {
    out[i] = this->group.conjugacyClasses[i].representative.sign();
  }
}

/*
// this is only used in one place.
void SubgroupDataWeylGroup::getSignCharacter(Vector<Rational>& out) {
  STACK_TRACE("SubgroupRootReflections::getSignCharacter");
  this->subgroupData->subgroup->getSignCharacter(out);
//  if (!this->subgroup->flagCCRepresentativesComputed)
//    this->subgroup->computeConjugacyClassSizesAndRepresentatives(0);
//  out.setSize(this->subgroup->conjugacyClassCount());
//  for (int i = 0; i < this->subgroup->conjugacyClassCount(); i ++)
//    out[i] = this->subgroup->conjugacyClasses[i].representative.Sign();
}
*/
void WeylGroupData::getTrivialRepresentation(
  GroupRepresentationCarriesAllMatrices<
    FiniteGroup<ElementWeylGroup>, Rational
  >& output
) {
  STACK_TRACE("WeylGroup::getTrivialRepresentation");
  this->group.checkInitializationConjugacyClasses();
  output.initialize(this->group);
  output.basis.makeEiBasis(1);
  for (int i = 0; i < this->getDimension(); i ++) {
    output.generators[i].makeIdentityMatrix(
      1, Rational::one(), Rational::zero()
    );
  }
  output.getCharacter();
}

void WeylGroupData::getSignRepresentation(
  GroupRepresentationCarriesAllMatrices<
    FiniteGroup<ElementWeylGroup>, Rational
  >& output
) {
  STACK_TRACE("WeylGroup::getSignRepresentation");
  this->group.checkInitializationConjugacyClasses();
  output.initialize(this->group);
  output.basis.makeEiBasis(1);
  ElementWeylGroup currentElt;
  for (int i = 0; i < this->getDimension(); i ++) {
    currentElt.makeSimpleReflection(i, *this);
    output.generators[i].makeIdentityMatrix(1);
    output.generators[i] *= currentElt.sign();
  }
  output.getCharacter();
}

void WeylGroupData::getStandardRepresentation(
  GroupRepresentationCarriesAllMatrices<
    FiniteGroup<ElementWeylGroup>, Rational
  >& output
) {
  STACK_TRACE("WeylGroup::getStandardRepresentation");
  this->group.checkInitializationConjugacyClasses();
  output.initialize(this->group);
  output.basis.makeEiBasis(this->getDimension());
  for (int i = 0; i < this->getDimension(); i ++) {
    this->getSimpleReflectionMatrix(i, output.generators[i]);
  }
  output.getCharacter();
}

void WeylGroupData::getStandardRepresentationMatrix(
  int g, Matrix<Rational>& output
) const {
  // the rank is the number of rows of the Cartan form
  output.initialize(
    this->cartanSymmetric.numberOfRows,
    this->cartanSymmetric.numberOfRows
  );
  for (int i = 0; i < this->cartanSymmetric.numberOfRows; i ++) {
    Vector<Rational> v;
    v.makeEi(this->cartanSymmetric.numberOfRows, i);
    this->actOn(g, v);
    for (int j = 0; j < this->cartanSymmetric.numberOfRows; j ++) {
      output.elements[j][i] = v[j];
    }
  }
}

void WeylGroupData::generateAdditivelyClosedSubset(
  Vectors<Rational>& input, Vectors<Rational>& output
) {
  output = (input);
  Vector<Rational> root;
  for (int i = 0; i < output.size; i ++) {
    for (int j = i + 1; j < output.size; j ++) {
      root = output[i] + output[j];
      if (this->isARoot(root)) {
        output.addOnTopNoRepetition(root);
      }
    }
  }
}

void WeylGroupData::perturbWeightToRegularWithRespectToRootSystem(
  const Vector<Rational>& inputH, Vector<Rational>& output
) {
  output = inputH;
  int indexFirstNonRegular;
  while (!this->isRegular(output, &indexFirstNonRegular)) {
    const Vector<Rational>& badRoot = this->rootSystem[indexFirstNonRegular];
    Rational maxMovement = 0;
    Rational scalarProduct1;
    Rational scalarProduct2;
    Rational tempMaxMovement;
    for (int i = 0; i < this->rootsOfBorel.size; i ++) {
      this->rootScalarCartanRoot(
        badRoot, this->rootsOfBorel[i], scalarProduct1
      );
      this->rootScalarCartanRoot(
        output, this->rootsOfBorel[i], scalarProduct2
      );
      if ((!scalarProduct1.isEqualToZero()) && (
          !scalarProduct2.isEqualToZero()
        )
      ) {
        tempMaxMovement = scalarProduct2 / scalarProduct1;
        tempMaxMovement.AssignAbsoluteValue();
        if ((tempMaxMovement < maxMovement) || maxMovement.isEqualToZero()
        ) {
          maxMovement = tempMaxMovement;
        }
      }
    }
    int adjustment = 2;
    if (this->rootScalarCartanRoot(badRoot, inputH).isNegative()) {
      adjustment = - 2;
    }
    output += badRoot * maxMovement / adjustment;
  }
}

bool WeylGroupData::isRegular(
  Vector<Rational>& input, int* indexFirstPerpendicularRoot
) {
  if (indexFirstPerpendicularRoot != nullptr) {
    *indexFirstPerpendicularRoot = - 1;
  }
  for (int i = 0; i < this->rootSystem.size; i ++) {
    if (
      this->rootScalarCartanRoot(input, this->rootSystem[i]).isEqualToZero()
    ) {
      if (indexFirstPerpendicularRoot != nullptr) {
        *indexFirstPerpendicularRoot = i;
      }
      return false;
    }
  }
  return true;
}

LargeInteger WeylGroupData::sizeByFormulaOrNegative1(
  char weylLetter, int dimension
) {
  // Humphreys, Introduction to Lie algebras and representation theory(1980),
  // page 66, Table 1
  if (
    weylLetter != 'A' &&
    weylLetter != 'B' &&
    weylLetter != 'C' &&
    weylLetter != 'D' &&
    weylLetter != 'E' &&
    weylLetter != 'F' &&
    weylLetter != 'G'
  ) {
    global.fatal
    << "WeylGroupData::sizeByFormulaOrNegative1 "
    << "called with impossible Weyl type: "
    << weylLetter
    << global.fatal;
  }
  LargeInteger output = 1;
  if (weylLetter == 'A') {
    output = Rational::factorial(dimension + 1);
  }
  if (weylLetter == 'B' || weylLetter == 'C') {
    output = Rational::factorial(dimension) * Rational::twoToTheNth(dimension);
  }
  if (weylLetter == 'D') {
    output =
    Rational::factorial(dimension) * Rational::twoToTheNth(dimension - 1);
  }
  if (weylLetter == 'E') {
    if (dimension == 6) {
      output = 51840;
    }
    if (dimension == 7) {
      output = 1024;
      output *= 81 * 35;
    }
    if (dimension == 8) {
      output = 1024 * 16;
      output *= 81 * 3;
      output *= 25 * 7;
    }
  }
  if (weylLetter == 'F') {
    output = 128 * 9;
  }
  if (weylLetter == 'G') {
    output = 12;
  }
  return output;
}

void WeylGroupData::getWord(int g, List<int>& out) const {
  out.setSize(
    this->group.elements[g].generatorsLastAppliedFirst.size
  );
  for (
    int i = 0; i < this->group.elements[g].generatorsLastAppliedFirst.size; i
    ++
  ) {
    out[i] = this->group.elements[g].generatorsLastAppliedFirst[i].index;
  }
}

bool WeylGroupData::operator==(const WeylGroupData& other) const {
  return
  this->cartanSymmetric == other.cartanSymmetric &&
  this->dynkinType == other.dynkinType;
}

void WeylGroupData::actOnRootByGroupElement(
  int index, Vector<Rational>& root, bool rhoAction, bool useMinusRho
) {
  const ElementWeylGroup& currentElt = this->group.elements[index];
  for (
    int i = currentElt.generatorsLastAppliedFirst.size - 1; i >= 0; i --
  ) {
    this->reflectSimple(
      currentElt.generatorsLastAppliedFirst[i].index,
      root,
      rhoAction,
      useMinusRho
    );
  }
}

void WeylGroupData::getCoCartanSymmetric(
  const Matrix<Rational>& input, Matrix<Rational>& output
) {
  STACK_TRACE("DynkinType::getCoCartanSymmetric");
  if (&input == &output) {
    Matrix<Rational> inputCopy = input;
    WeylGroupData::getCoCartanSymmetric(inputCopy, output);
    return;
  }
  output.initialize(input.numberOfRows, input.numberOfColumns);
  for (int i = 0; i < input.numberOfRows; i ++) {
    for (int j = 0; j < input.numberOfColumns; j ++) {
      output(i, j) = input(i, j) * 4 / (input(i, i) * input(j, j));
    }
  }
}

void WeylGroupData::generateRootSystem() {
  Vectors<Rational> startRoots;
  HashedList<Vector<Rational> > rootsFinder;
  startRoots.makeEiBasis(this->getDimension());
  int estimatedNumberOfRoots = this->dynkinType.getRootSystemSize();
  this->generateOrbit(
    startRoots, false, rootsFinder, false, estimatedNumberOfRoots
  );
  this->rootSystem.clear();
  this->rootSystem.setExpectedSize(rootsFinder.size);
  this->rootsOfBorel.setSize(0);
  this->rootsOfBorel.reserve(rootsFinder.size / 2);
  for (int i = 0; i < rootsFinder.size; i ++) {
    if (rootsFinder[i].isPositiveOrZero()) {
      this->rootsOfBorel.addOnTop(rootsFinder[i]);
    }
  }
  this->rootsOfBorel.quickSortAscending();
  for (int i = this->rootsOfBorel.size - 1; i >= 0; i --) {
    this->rootSystem.addOnTop(- this->rootsOfBorel[i]);
  }
  for (int i = 0; i < this->rootsOfBorel.size; i ++) {
    this->rootSystem.addOnTop(this->rootsOfBorel[i]);
  }
}

void WeylGroupData::actOnRootAlgByGroupElement(
  int index, PolynomialSubstitution<Rational>& root, bool rhoAction
) {
  for (
    int i = this->group.elements[index].generatorsLastAppliedFirst.size - 1; i
    >=
    0; i --
  ) {
    this->simpleReflectionRootPolynomial(
      this->group.elements[index].generatorsLastAppliedFirst[i].index,
      root,
      rhoAction
    );
  }
}

void WeylGroupData::computeWeylGroupAndrootsOfBorel(
  Vectors<Rational>& output
) {
  this->group.computeAllElements(false);
  output.size = 0;
  output.reserve(this->rootSystem.size / 2);
  for (int i = 0; i < this->rootSystem.size; i ++) {
    if (this->rootSystem[i].isPositiveOrZero()) {
      output.addOnTop(this->rootSystem[i]);
    }
  }
}

bool WeylGroupData::leftIsHigherInBruhatOrderThanRight(
  ElementWeylGroup& left, ElementWeylGroup& right
) {
  Vector<Rational> leftImage;
  leftImage = this->rho;
  Vector<Rational> rightImage;
  rightImage = this->rho;
  this->actOn(left, leftImage, leftImage);
  this->actOn(right, rightImage, rightImage);
  return (rightImage - leftImage).isPositiveOrZero() &&
  !(rightImage - leftImage).isEqualToZero();
}

void WeylGroupData::computeRootsOfBorel(Vectors<Rational>& output) {
  output.size = 0;
  this->rootSystem.clear();
  this->generateRootSystem();
  output = this->rootsOfBorel;
}

std::string WeylGroupData::toStringCppCharTable(FormatExpressions* format) {
  STACK_TRACE("WeylGroup::toStringCppConjugacyClasses");
  (void) format;
  // portable way to avoid non-used parameter warning.
  if (!this->flagCharTableIsComputed) {
    return "<br>Conjugacy classes not computed";
  }
  std::stringstream out;
  out << "<hr>Here is the c++ input code for the char table.";
  out << "<br>";
  FormatExpressions formatNoDynkinTypePlusesExponents;
  formatNoDynkinTypePlusesExponents.flagDynkinTypeDontUsePlusAndExponent =
  true;
  out
  << "bool loadCharacterTable"
  << this->dynkinType.toString(&formatNoDynkinTypePlusesExponents)
  << "(WeylGroup& output)\n<br>{ ";
  out
  << " output.characterTable.setExpectedSize("
  << this->group.getSize().toString()
  << "); output.characterTable.setSize(0);";
  out
  << "\n<br>&nbsp;&nbsp;ClassFunction&lt;"
  << "FiniteGroup&lt;"
  << "ElementWeylGroup&lt;"
  << "WeylGroup&gt; &gt;, Rational&gt; currentCF;";
  out << "\n<br>&nbsp;&nbsp;currentCF.G = &output;";
  for (int i = 0; i < this->group.characterTable.size; i ++) {
    out << "\n<br>&nbsp;&nbsp;currentCF.data.assignString(\"";
    out << "(";
    // Print vector ensuring every number is at least 3 characters wide. (3
    // should suffice for E8... or does it?)
    for (
      int j = 0; j < this->group.characterTable[i].data.size; j ++
    ) {
      std::string numberString =
      this->group.characterTable[i].data[j].toString();
      out << numberString;
      for (size_t k = numberString.size(); k < 3; k ++) {
        out << "&nbsp;";
      }
      if (j != this->group.characterTable[i].data.size - 1) {
        out << ", ";
      }
    }
    out << ")";
    out << "\"); output.characterTable.addOnTop(currentCF);";
  }
  out << "\n<br>&nbsp;&nbsp;output.irrepsCarterLabels.setSize(0);";
  for (int i = 0; i < this->irrepsCarterLabels.size; i ++) {
    out
    << "\n<br>&nbsp;&nbsp;output.irrepsCarterLabels.addOnTop(\""
    << this->irrepsCarterLabels[i]
    << "\");";
  }
  out << "\n<br>&nbsp;&nbsp;return true;";
  out << "\n<br>}";
  return out.str();
}

std::string WeylGroupData::toStringCppConjugacyClasses(
  FormatExpressions* format
) {
  STACK_TRACE("WeylGroup::toStringCppConjugacyClasses");
  (void) format;
  // portable way to avoid non-used parameter warning.
  if (!this->group.flagCCRepresentativesComputed) {
    return "";
  }
  std::stringstream out;
  out << "<hr>Here is the c++ input code for the conjugacy class table.";
  out << "<br>";
  FormatExpressions formatNoDynkinTypePlusesExponents;
  formatNoDynkinTypePlusesExponents.flagDynkinTypeDontUsePlusAndExponent =
  true;
  out
  << "bool loadConjugacyClasses"
  << this->dynkinType.toString(&formatNoDynkinTypePlusesExponents)
  << "(WeylGroup& output)\n<br>{ ";
  out << "output.computeRho(true);";
  out << "\n<br>&nbsp;&nbsp;WeylGroup::ConjugacyClass emptyClass;";
  out << "\n<br>&nbsp;&nbsp;emptyClass.flagRepresentativeComputed = true;";
  out
  << "\n<br>&nbsp;&nbsp;output.conjugacyClasses.initializeFillInObject("
  << this->group.conjugacyClasses.size
  << ", emptyClass);";
  for (int i = 0; i < this->group.conjugacyClassCount(); i ++) {
    out << "\n<br>&nbsp;&nbsp;output.conjugacyClasses[" << i;
    for (
      size_t j = (Rational(i)).toString().size(); j < 3; j ++
    ) {
      // <-if the index i is smaller than 100, make sure it takes
      out << "&nbsp;";
      // making sure index has width exactly 3 spaces
    }
    out << "].representative.makeFromReadableReflections(output, false, \"";
    for (
      int j = 0; j < this->group.conjugacyClasses[i].representative.
      generatorsLastAppliedFirst.size; j ++
    ) {
      out
      << this->group.conjugacyClasses[i].representative.
      generatorsLastAppliedFirst[j].index +
      1;
      if (
        j !=
        this->group.conjugacyClasses[i].representative.
        generatorsLastAppliedFirst.size -
        1
      ) {
        out << ",";
      }
    }
    out << "\");";
  }
  for (int i = 0; i < this->group.conjugacyClassCount(); i ++) {
    out << "\n<br>&nbsp;&nbsp;output.conjugacyClasses[" << i;
    for (
      size_t j = (Rational(i)).toString().size(); j < 3; j ++
    ) {
      // <-if the index i is smaller than 100, make sure it takes
      out << "&nbsp;";
      // making sure index has width exactly 3 spaces
    }
    out
    << "].size ="
    << this->group.conjugacyClasses[i].size.toString()
    << ";";
  }
  out << "\n<br>&nbsp;&nbsp;output.ccCarterLabels.setSize(0);";
  for (int i = 0; i < this->ccCarterLabels.size; i ++) {
    out
    << "\n<br>&nbsp;&nbsp;output.ccCarterLabels.addOnTop(\""
    << this->ccCarterLabels[i]
    << "\");";
  }
  out << "\n<br>&nbsp;&nbsp;output.loadConjugacyClassesHelper();";
  out << "\n<br>&nbsp;&nbsp;return true;";
  out << "\n<br>}";
  return out.str();
}

std::string WeylGroupData::toStringRootsAndRootReflections(
  FormatExpressions* format
) {
  STACK_TRACE("WeylGroup::toStringRootsAndRootReflections");
  (void) format;
  // portable way to avoid non-used parameter warning.
  std::stringstream out, outLatex;
  out << "<br>The root system has " << this->rootSystem.size << " elements.\n";
  out
  << "<table><tr><td>Simple basis coordinates</td><td>Epsilon coordinates</td>"
  << "<td>Reflection w.r.t. root</td></tr>";
  Vectors<Rational> rootSystemEpsCoords;
  this->getEpsilonCoordinates(this->rootSystem, rootSystemEpsCoords);
  ElementWeylGroup currentRootReflection;
  for (int i = 0; i < this->rootSystem.size; i ++) {
    const Vector<Rational>& current = this->rootSystem[i];
    currentRootReflection.MakeRootReflection(current, *this);
    out
    << "<tr><td>"
    << current.toString()
    << "</td><td>"
    << rootSystemEpsCoords[i].toStringLetterFormat("e")
    << "</td>"
    << "<td>"
    << HtmlRoutines::getMathNoDisplay(currentRootReflection.toString())
    << "</td>"
    << "</tr>";
  }
  out << "</table>";
  out << "Comma delimited list of roots: ";
  for (int i = 0; i < this->rootSystem.size; i ++) {
    out << this->rootSystem[i].toString();
    if (i != this->rootSystem.size - 1) {
      out << ", ";
    }
  }
  out << outLatex.str();
  return out.str();
}

std::string WeylGroupData::toString(FormatExpressions* format) {
  STACK_TRACE("WeylGroupData::toString");
  std::stringstream out;
  out << "<br>Size: " << this->group.elements.size << "\n";
  out << "Number of Vectors: " << this->rootSystem.size << "\n";
  out << "<br>Half-sum positive roots:" << this->rho.toString() << "\n";
  out << this->toStringRootsAndRootReflections();
  out << "<br>Symmetric cartan: " << this->cartanSymmetric.toString();
  if (this->flagCharTableIsComputed) {
    out << "<br>Character table: ";
    out << this->group.prettyPrintCharacterTable();
  }
  out << this->group.toStringConjugacyClasses(format);
  out << this->group.toStringElements(format);
  out << this->toStringCppConjugacyClasses(format);
  out << this->toStringCppCharTable(format);
  return out.str();
}

bool WeylGroupData::isAddmisibleDynkinType(char candidateLetter, int n) {
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

void WeylGroupData::computeEpsilonMatrix() {
  if (this->matrixSendsSimpleVectorsToEpsilonVectors.isZeroPointer()) {
    this->dynkinType.getEpsilonMatrix(
      this->matrixSendsSimpleVectorsToEpsilonVectors.getElement()
    );
  }
}

void WeylGroupData::transformToAdmissibleDynkinType(
  char inputLetter, int& outputRank
) {
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

void WeylGroupData::computeCoCartanSymmetricFromCartanSymmetric() {
  this->getCoCartanSymmetric(
    this->cartanSymmetric, this->coCartanSymmetric
  );
}

void WeylGroupData::makeMeFromMyCartanSymmetric() {
  this->reset();
  this->generateRootSystem();
  DynkinDiagramRootSubalgebra dynkinTypeComputer;
  Vectors<Rational> simpleBasis;
  simpleBasis.makeEiBasis(this->cartanSymmetric.numberOfRows);
  dynkinTypeComputer.computeDiagramTypeModifyInputRelative(
    simpleBasis, this->rootSystem, this->cartanSymmetric
  );
  dynkinTypeComputer.getDynkinType(this->dynkinType);
  this->makeFinalSteps();
}

void WeylGroupData::makeFinalSteps() {
  this->computeCoCartanSymmetricFromCartanSymmetric();
}

void WeylGroupData::initializeGenerators() {
  this->group.generators.setSize(this->getDimension());
  if (this->getDimension() == 0) {
    this->group.generators.setSize(1);
    this->group.generators[0].makeIdentity(*this);
  }
  for (int i = 0; i < this->getDimension(); i ++) {
    this->group.generators[i].makeSimpleReflection(i, *this);
  }
}

void WeylGroupData::makeFromDynkinType(const DynkinType& inputType) {
  this->reset();
  this->dynkinType = inputType;
  this->dynkinType.getCartanSymmetric(this->cartanSymmetric);
  this->dynkinType.getCoCartanSymmetric(this->coCartanSymmetric);
  this->makeFinalSteps();
  // eventually, there will be formulas for all classical types
  List<char> letters;
  List<int> ranks;
  this->dynkinType.getLettersTypesMultiplicities(&letters, &ranks, nullptr);
  if (letters.size == 1) {
    if (ranks.size == 1) {
      if (letters[0] == 'A') {
        this->group.ComputeIrreducibleRepresentationsWithFormulas =
        this->computeIrreducibleRepresentationsWithFormulasImplementation;
      }
    }
  }
  this->initializeGenerators();
  this->group.checkInitialization();
}

void WeylGroupData::makeFromDynkinTypeDefaultLengthKeepComponentOrder(
  const DynkinType& inputType
) {
  this->reset();
  this->dynkinType = inputType;
  this->dynkinType.getCartanSymmetricDefaultLengthKeepComponentOrder(
    this->cartanSymmetric
  );
  this->makeFinalSteps();
}

void WeylGroupData::makeArbitrarySimple(
  char weylGroupLetter, int n, const Rational* firstCoRootLengthSquared
) {
  DynkinType inputType;
  inputType.makeSimpleType(weylGroupLetter, n, firstCoRootLengthSquared);
  this->makeFromDynkinType(inputType);
}

WeylGroupAutomorphisms::WeylGroupAutomorphisms() {
  this->flagAllOuterAutosComputed = false;
  this->flagDeallocated = false;
  this->flagOuterAutosGeneratorsComputed = false;
  this->weylGroup = nullptr;
}

WeylGroupAutomorphisms::~WeylGroupAutomorphisms() {
  this->flagDeallocated = true;
  this->weylGroup = nullptr;
}

void WeylGroupAutomorphisms::computeOuterAutomorphisms() {
  STACK_TRACE("WeylGroupAutomorphisms::computeOuterAutomorphisms");
  this->checkInitialization();
  if (this->flagAllOuterAutosComputed) {
    return;
  }
  this->computeOuterAutoGenerators();
  this->outerAutomorphisms.generateElements(0);
  this->flagAllOuterAutosComputed = true;
}

void WeylGroupAutomorphisms::computeOuterAutoGenerators() {
  STACK_TRACE("WeylGroupAutomorphisms::computeOuterAutoGenerators");
  if (this->flagOuterAutosGeneratorsComputed) {
    return;
  }
  this->checkInitialization();
  List<MatrixTensor<Rational> >& generators =
  this->outerAutomorphisms.generators;
  this->weylGroup->dynkinType.getOuterAutosGeneratorsActOnVectorColumn(
    generators
  );
  for (int i = 0; i < generators.size; i ++) {
    if (
      generators[i].getMinimumNumberOfColumnsNumberOfRows() !=
      this->weylGroup->getDimension() ||
      generators[i].getMinimalNumberOfColumns() !=
      this->weylGroup->getDimension() ||
      generators[i].getMinimalNumberOfRows() != this->weylGroup->getDimension()
    ) {
      global.fatal
      << "Bad outer automorphisms, type "
      << this->weylGroup->dynkinType.toString()
      << "."
      << global.fatal;
    }
  }
  this->flagOuterAutosGeneratorsComputed = true;
}

void WeylGroupData::getEpsilonCoordinatesWRTsubalgebra(
  Vectors<Rational>& generators,
  List<Vector<Rational> >& input,
  Vectors<Rational>& output
) {
  Matrix<Rational> basisChange;
  Matrix<Rational> epsilonMatrix;
  DynkinDiagramRootSubalgebra tempDyn;
  Vectors<Rational> simpleBasis;
  Vectors<Rational> coordsInNewBasis;
  simpleBasis = generators;
  tempDyn.computeDiagramTypeModifyInput(simpleBasis, *this);
  bool tempBool = true;
  if (generators.size == 0) {
    tempBool = false;
  }
  if (!tempBool) {
    output.setSize(input.size);
    for (int i = 0; i < input.size; i ++) {
      output[i].makeZero(0);
    }
    return;
  }
  basisChange.resize(0, 0, true);
  for (int i = 0; i < tempDyn.simpleComponentTypes.size; i ++) {
    DynkinSimpleType::getEpsilonMatrix(
      tempDyn.simpleComponentTypes[i].letter,
      tempDyn.simpleComponentTypes[i].rank,
      epsilonMatrix
    );
    basisChange.directSumWith(epsilonMatrix, Rational(0));
  }
  simpleBasis.assignListList(tempDyn.simpleBasesConnectedComponents);
  coordsInNewBasis.setSize(input.size);
  for (int i = 0; i < input.size; i ++) {
    input[i].getCoordinatesInBasis(simpleBasis, coordsInNewBasis[i]);
  }
  basisChange.actOnVectorsColumn(coordsInNewBasis, output);
}

void WeylGroupData::getEpsilonCoordinates(
  const List<Vector<Rational> >& input, Vectors<Rational>& output
) {
  if (&input == &output) {
    List<Vector<Rational> > newInput = input;
    this->getEpsilonCoordinates(newInput, output);
    return;
  }
  output.setSize(input.size);
  for (int i = 0; i < input.size; i ++) {
    this->getEpsilonCoordinates(input[i], output[i]);
  }
}

LargeInteger WeylGroupData::getSizeByFormulaImplementation(
  FiniteGroup<ElementWeylGroup>& G
) {
  WeylGroupData* W = static_cast<WeylGroupData*>(G.specificDataPointer);
  W->checkConsistency();
  return W->dynkinType.getWeylGroupSizeByFormula();
}

void WeylGroupData::getWeylChamber(Cone& output) {
  Matrix<Rational> invertedCartan;
  invertedCartan = this->cartanSymmetric;
  invertedCartan.invert();
  Vectors<Rational> roots;
  roots.assignMatrixRows(invertedCartan);
  output.createFromVertices(roots);
}

void WeylGroupData::getFundamentalWeightsInSimpleCoordinates(
  Vectors<Rational>& output
) {
  Matrix<Rational> result;
  result = this->cartanSymmetric;
  Rational scalar;
  for (int i = 0; i < this->getDimension(); i ++) {
    scalar = 2 / this->cartanSymmetric.elements[i][i];
    result.rowTimesScalar(i, scalar);
  }
  result.transpose();
  result.invert();
  output.assignMatrixRows(result);
}

void WeylGroupData::getIntegralLatticeInSimpleCoordinates(Lattice& output) {
  output.basisRationalForm = this->cartanSymmetric;
  Vector<Rational> root;
  for (int i = 0; i < this->getDimension(); i ++) {
    root.makeEi(this->getDimension(), i);
    output.basisRationalForm.rowTimesScalar(
      i, 2 / this->rootScalarCartanRoot(root, root)
    );
  }
  output.basisRationalForm.transpose();
  output.basisRationalForm.invert();
  output.makeFromMatrix(output.basisRationalForm);
  output.reduce();
}

Rational WeylGroupData::getKillingDividedByTraceRatio() {
  Rational result = 0;
  Rational scalar;
  for (int i = 0; i < this->rootSystem.size; i ++) {
    scalar =
    this->rootScalarCartanRoot(
      this->rootSystem[i], this->rootSystem[0]
    );
    result += scalar * scalar;
  }
  result /=
  this->rootScalarCartanRoot(
    this->rootSystem[0], this->rootSystem[0]
  );
  return result;
}

void WeylGroupData::getLongestWeylElement(ElementWeylGroup& outputWeylElement)
{
  this->computeRho(false);
  Vector<Rational> lowest = this->rho;
  Vectors<Rational> roots;
  roots.makeEiBasis(this->getDimension());
  this->getLowestElementInOrbit(
    lowest, &outputWeylElement, roots, false, false
  );
}

void WeylGroupData::getExtremeElementInOrbit(
  Vector<Rational>& inputOutput,
  ElementWeylGroup* outputWeylElement,
  Vectors<Rational>& bufferEiBAsis,
  bool findLowest,
  bool rhoAction,
  bool useMinusRho,
  int* sign,
  bool* stabilizerFound
) {
  if (outputWeylElement != nullptr) {
    outputWeylElement->makeIdentity(*this);
  }
  if (sign != nullptr) {
    *sign = 1;
  }
  if (stabilizerFound != nullptr) {
    *stabilizerFound = false;
  }
  Rational scalarProduct;
  ElementWeylGroup eltSimplReflection;
  //  static int numTimesReflectionWasApplied = 0;
  for (bool found = true; found;) {
    found = false;
    for (int i = 0; i < this->getDimension(); i ++) {
      bool shouldApplyReflection = false;
      scalarProduct =
      this->rootScalarCartanRoot(inputOutput, bufferEiBAsis[i]);
      if (findLowest) {
        shouldApplyReflection = scalarProduct.isPositive();
      } else {
        shouldApplyReflection = scalarProduct.isNegative();
      }
      if (stabilizerFound != nullptr) {
        if (scalarProduct.isEqualToZero()) {
          *stabilizerFound = true;
        }
      }
      if (shouldApplyReflection) {
        found = true;
        if (!rhoAction) {
          this->reflectSimple<Rational>(i, inputOutput);
        } else if (!useMinusRho) {
          this->reflectRhoSimple(i, inputOutput);
        } else {
          this->reflectMinusRhoSimple(i, inputOutput);
        }
        if (outputWeylElement != nullptr) {
          eltSimplReflection.makeSimpleReflection(i, *this);
          *outputWeylElement = eltSimplReflection *(*outputWeylElement);
        }
        if (sign != nullptr) {
          *sign *= - 1;
        }
      }
    }
  }
}

LargeInteger WeylGroupAutomorphisms::getOrbitSize(Vector<Rational>& weight) {
  STACK_TRACE("WeylGroupAutomorphisms::getOrbitSize");
  this->checkInitialization();
  HashedList<Vector<Rational> > highestWeights;
  for (int i = 0; i < this->outerAutomorphisms.elements.size; i ++) {
    Vector<Rational> candidate;
    this->outerAutomorphisms.elements[i].actOnVectorColumn(weight, candidate);
    this->weylGroup->raiseToDominantWeight(candidate);
    highestWeights.addOnTopNoRepetition(candidate);
  }
  return this->weylGroup->getOrbitSize(weight) * highestWeights.size;
}

bool WeylGroupAutomorphisms::isElementWeylGroupOrOuterAutomorphisms(
  const MatrixTensor<Rational>& matrix
) {
  STACK_TRACE("WeylGroup::IsElementGroupOrOuterAuto");
  this->computeOuterAutomorphisms();
  Vector<Rational> rhoImage;
  matrix.actOnVectorColumn(this->weylGroup->rho, rhoImage);
  ElementWeylGroup elementCandidate;
  this->weylGroup->raiseToDominantWeight(
    rhoImage, nullptr, nullptr, &elementCandidate
  );
  Matrix<Rational> candidateMatrix;
  MatrixTensor<Rational>
  candidateMatrixTensorForm,
  candidateMatrixWithOuterAutomorphisms;
  this->weylGroup->getMatrixStandardRepresentation(
    elementCandidate, candidateMatrix
  );
  candidateMatrixTensorForm = candidateMatrix;
  for (int i = 0; i < this->outerAutomorphisms.elements.size; i ++) {
    candidateMatrixWithOuterAutomorphisms =
    this->outerAutomorphisms.elements[i];
    candidateMatrixWithOuterAutomorphisms *= candidateMatrixTensorForm;
    if (candidateMatrixWithOuterAutomorphisms == matrix) {
      return true;
    }
  }
  return false;
}

void WeylGroupData::getMatrixReflection(
  Vector<Rational>& reflectionRoot, Matrix<Rational>& output
) {
  Vectors<Rational> basis;
  int dimension = this->getDimension();
  basis.makeEiBasis(dimension);
  for (int i = 0; i < dimension; i ++) {
    this->reflectBetaWithRespectToAlpha(
      reflectionRoot, basis[i], false, basis[i]
    );
  }
  output.assignVectorsToRows(basis);
  output.transpose();
}

void WeylGroupData::getCoxeterPlane(
  Vector<double>& outputBasis1, Vector<double>& outputBasis2
) {
  STACK_TRACE("WeylGroup::getCoxeterPlane");
  this->computeRho(true);
  int dimension = this->getDimension();
  if (dimension < 2) {
    return;
  }
  ElementWeylGroup element;
  this->getCoxeterElement(element);
  Matrix<Rational> matCoxeterElt, matrix;
  this->getMatrixStandardRepresentation(element, matCoxeterElt);
  matrix = matCoxeterElt;
  int coxeterNumber =
  this->rootSystem.lastObject()->sumCoordinates().numeratorShort + 1;
  for (int i = 0; i < coxeterNumber - 1; i ++) {
    matrix.multiplyOnTheLeft(matCoxeterElt);
  }
  Complex<double> eigenValue;
  eigenValue.realPart =
  FloatingPoint::cosFloating(2 * MathRoutines::pi() / coxeterNumber);
  eigenValue.imaginaryPart =
  FloatingPoint::sinFloating(2 * MathRoutines::pi() / coxeterNumber);
  Matrix<Complex<double> > eigenMat;
  eigenMat.initialize(
    matCoxeterElt.numberOfRows, matCoxeterElt.numberOfColumns
  );
  for (int i = 0; i < eigenMat.numberOfRows; i ++) {
    for (int j = 0; j < eigenMat.numberOfColumns; j ++) {
      eigenMat.elements[i][j] = matCoxeterElt.elements[i][j].getDoubleValue();
      if (i == j) {
        eigenMat.elements[i][i] -= eigenValue;
      }
    }
  }
  List<Vector<Complex<double> > > eigenSpaceList;
  eigenMat.getZeroEigenSpace(eigenSpaceList);
  Vectors<Complex<double> > eigenSpace;
  outputBasis1.setSize(dimension);
  outputBasis2.setSize(dimension);
  eigenSpace.operator=(eigenSpaceList);
  DrawOperations tempDO;
  tempDO.initDimensions(dimension);
  tempDO.graphicsUnit = DrawOperations::graphicsUnitDefault;
  eigenSpace.operator=(eigenSpaceList);
  for (int i = 0; i < dimension; i ++) {
    for (int j = 0; j < dimension; j ++) {
      tempDO.bilinearForm.elements[i][j] =
      this->cartanSymmetric.elements[i][j].getDoubleValue();
    }
  }
  if (eigenSpace.size > 0) {
    if (coxeterNumber > 2) {
      for (int j = 0; j < dimension; j ++) {
        outputBasis1[j] = eigenSpace[0][j].realPart;
        outputBasis2[j] = eigenSpace[0][j].imaginaryPart;
      }
    } else if (coxeterNumber == 1 && eigenSpace.size > 1) {
      for (int j = 0; j < dimension; j ++) {
        outputBasis1[j] = eigenSpace[0][j].realPart;
        outputBasis2[j] = eigenSpace[1][j].realPart;
      }
    }
    tempDO.modifyToOrthonormalNoShiftSecond(outputBasis2, outputBasis1);
  }
}

void WeylGroupData::drawRootSystem(
  DrawingVariables& outputDV,
  bool wipeCanvas,
  bool drawWeylChamber,
  Vector<Rational>* bluePoint,
  bool labelDynkinDiagramVertices,
  Vectors<Rational>* predefinedProjectionPlane
) {
  STACK_TRACE("WeylGroup::drawRootSystem");
  DrawOperations& output = outputDV.operations;
  if (wipeCanvas) {
    output.initialize();
  }
  int dimension = this->getDimension();
  if (dimension == 1) {
    Vector<Rational> root;
    Vector<Rational> zeroVector;
    zeroVector.makeZero(2);
    root.makeEi(2, 0);
    std::string currentColor = "#ff00ff";
    for (int i = 0; i < 2; i ++) {
      output.drawLineBetweenTwoVectorsBufferRational(
        zeroVector, root, "green", 1
      );
      output.drawCircleAtVectorBufferRational(root, currentColor, 2);
      root.negate();
    }
    return;
  }
  this->computeRho(true);
  Vector<Rational> zeroRoot;
  zeroRoot.makeZero(dimension);
  output.initDimensions(dimension);
  output.graphicsUnit = DrawOperations::graphicsUnitDefault;
  for (int i = 0; i < dimension; i ++) {
    for (int j = 0; j < dimension; j ++) {
      output.bilinearForm.elements[i][j] =
      this->cartanSymmetric.elements[i][j].getDoubleValue();
    }
  }
  Vector<double> root;
  Vectors<double>& twoPlane = output.basisProjectionPlane;
  if (predefinedProjectionPlane == nullptr) {
    this->getCoxeterPlane(twoPlane[0], twoPlane[1]);
  } else {
    predefinedProjectionPlane->getVectorsDouble(twoPlane);
  }
  if (twoPlane.size != 2) {
    global.fatal
    << "Object twoPlane is supposed to "
    << "be two-dimensional but it is instead of dimension: "
    << twoPlane.size
    << ". "
    << global.fatal;
  }
  Vectors<Rational> rootSystemSorted;
  rootSystemSorted = this->rootSystem;
  List<double> lengths;
  lengths.setSize(rootSystemSorted.size);
  for (int i = 0; i < this->rootSystem.size; i ++) {
    root.setSize(dimension);
    for (int j = 0; j < dimension; j ++) {
      root[j] = this->rootSystem[i][j].getDoubleValue();
    }
    double Length1 =
    this->rootScalarCartanRoot(root, output.basisProjectionPlane[0]);
    double Length2 =
    this->rootScalarCartanRoot(root, output.basisProjectionPlane[1]);
    lengths[i] =
    FloatingPoint::sqrtFloating(Length1 * Length1 + Length2 * Length2);
  }
  for (int i = 0; i < rootSystemSorted.size; i ++) {
    for (int j = i; j < rootSystemSorted.size; j ++) {
      if (lengths[i] < lengths[j]) {
        MathRoutines::swap(lengths[i], lengths[j]);
        MathRoutines::swap(rootSystemSorted[i], rootSystemSorted[j]);
      }
    }
  }
  Vector<Rational> differenceRoot;
  differenceRoot = rootSystemSorted[0] - rootSystemSorted[1];
  Rational minLength =
  this->rootScalarCartanRoot(differenceRoot, differenceRoot);
  for (int i = 2; i < rootSystemSorted.size; i ++) {
    differenceRoot = rootSystemSorted[0] - rootSystemSorted[i];
    if (
      minLength > this->rootScalarCartanRoot(differenceRoot, differenceRoot)
    ) {
      minLength = this->rootScalarCartanRoot(differenceRoot, differenceRoot);
    }
  }
  if (bluePoint != nullptr) {
    output.drawCircleAtVectorBufferRational(*bluePoint, "blue", 5);
    output.drawCircleAtVectorBufferRational(*bluePoint, "blue", 4);
    output.drawCircleAtVectorBufferRational(*bluePoint, "blue", 3);
  }
  if (drawWeylChamber) {
    Cone weylChamber;
    this->getWeylChamber(weylChamber);
    FormatExpressions tempFormat;
    weylChamber.drawMeProjective(nullptr, false, outputDV, tempFormat);
  }
  output.centerX = 300;
  output.centerY = 300;
  Rational scalarProduct;
  for (int i = 0; i < rootSystemSorted.size; i ++) {
    output.drawLineBetweenTwoVectorsBufferRational(
      zeroRoot, rootSystemSorted[i], "green", 1
    );
    output.drawCircleAtVectorBufferRational(
      rootSystemSorted[i], "#ff00ff", 2
    );
    for (int j = i + 1; j < rootSystemSorted.size; j ++) {
      differenceRoot = rootSystemSorted[i] - rootSystemSorted[j];
      scalarProduct =
      this->rootScalarCartanRoot(differenceRoot, differenceRoot);
      if (minLength == scalarProduct) {
        output.drawLineBetweenTwoVectorsBufferRational(
          rootSystemSorted[i], rootSystemSorted[j], "blue", 1
        );
      }
    }
  }
  Vector<Rational> rationalRoot;
  Vectors<Rational> epsNotationSimpleBasis;
  epsNotationSimpleBasis.makeEiBasis(dimension);
  this->getEpsilonCoordinates(epsNotationSimpleBasis, epsNotationSimpleBasis);
  for (int i = 0; i < dimension; i ++) {
    rationalRoot.makeEi(dimension, i);
    output.drawCircleAtVectorBufferRational(rationalRoot, "red", 1);
    output.drawCircleAtVectorBufferRational(rationalRoot, "red", 3);
    output.drawCircleAtVectorBufferRational(rationalRoot, "red", 4);
    if (labelDynkinDiagramVertices) {
      Vector<Rational>& current = epsNotationSimpleBasis[i];
      output.drawTextAtVectorBufferRational(
        rationalRoot, current.toStringLetterFormat("e"), "black", 10
      );
    }
  }
  Vectors<double> highlightGroup;
  List<std::string> highlightLabels;
  highlightGroup.setSize(2);
  highlightLabels.setSize(2);
  for (int i = 0; i < this->rootsOfBorel.size; i ++) {
    highlightGroup[0] = this->rootsOfBorel[i].getVectorDouble();
    highlightGroup[1] = - this->rootsOfBorel[i].getVectorDouble();
    highlightLabels[0] = this->rootsOfBorel[i].toString();
    highlightLabels[1] = (- this->rootsOfBorel[i]).toString();
    output.drawHighlightGroup(highlightGroup, highlightLabels, "gray", 5);
  }
  std::stringstream tempStream;
  tempStream << this->dynkinType.getWeylGroupName();
  if (this->getDimension() == 2 && predefinedProjectionPlane != nullptr) {
    twoPlane[1][0] = 1;
    twoPlane[1][1] = 0;
    twoPlane[0][0] = 0;
    twoPlane[0][1] = 1;
    outputDV.operations.modifyToOrthonormalNoShiftSecond(
      twoPlane[0], twoPlane[1]
    );
  }
  output.drawTextBuffer(
    0,
    0,
    tempStream.str(),
    10,
    static_cast<int>(HtmlRoutines::redGreenBlue(0, 0, 0)),
    DrawingVariables::TextStyleNormal
  );
}

std::string WeylGroupData::generateWeightSupportMethod1(
  Vector<Rational>& highestWeightSimpleCoords,
  Vectors<Rational>& outputWeightsSimpleCoords,
  int upperBoundWeights
) {
  HashedList<Vector<Rational> > dominantWeights;
  (void) upperBoundWeights;
  // portable way to avoid non-used parameter warning.
  double upperBoundDouble = 100000 / (Rational(this->group.getSize())).
  getDoubleValue();
  int upperBoundInt =
  MathRoutines::maximum(static_cast<int>(upperBoundDouble), 10000);
  // int upperBoundInt = 10000;
  Vector<Rational> highestWeightTrue = highestWeightSimpleCoords;
  this->raiseToDominantWeight(highestWeightTrue);
  std::stringstream out;
  if (highestWeightTrue != highestWeightSimpleCoords) {
    out
    << "<br>The input weight is not highest... "
    << "using the highest weight in the same orbit instead. "
    << "Your input in simple coordinates was: "
    << highestWeightSimpleCoords.toString()
    << ".<br> ";
  }
  out
  << "The highest weight in simple coordinates is: "
  << highestWeightTrue.toString()
  << ".<br>";
  std::string tempS;
  bool isTrimmed =
  !this->getAllDominantWeightsHWFDIM(
    highestWeightSimpleCoords, dominantWeights, upperBoundInt, &tempS
  );
  out << tempS << "<br>";
  if (isTrimmed) {
    out
    << "Trimmed the # of dominant weights - upper bound is "
    << upperBoundInt
    << ". <br>";
  } else {
    out
    << "Number of (non-strictly) dominant weights: "
    << dominantWeights.size
    << "<br>";
  }
  HashedList<Vector<Rational> > finalWeights;
  int estimatedNumberOfWeights = static_cast<int>((
      Rational(this->group.getSize())
    ).getDoubleValue() *
    dominantWeights.size
  );
  estimatedNumberOfWeights =
  MathRoutines::minimum(10000, estimatedNumberOfWeights);
  finalWeights.reserve(estimatedNumberOfWeights);
  finalWeights.setHashSize(estimatedNumberOfWeights);
  Vectors<Rational> dominantWeightsNonHashed;
  dominantWeightsNonHashed = (dominantWeights);
  this->generateOrbit(
    dominantWeightsNonHashed,
    false,
    finalWeights,
    false,
    0,
    nullptr,
    10000
  );
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

bool WeylGroupData::isEigenSpaceGeneratorCoxeterElement(
  Vector<Rational>& input
) {
  ElementWeylGroup element;
  this->getCoxeterElement(element);
  Matrix<Rational> matCoxeterElt;
  this->getMatrixStandardRepresentation(element, matCoxeterElt);
  Vector<Rational> root = input;
  for (int i = 0; i < this->getDimension(); i ++) {
    matCoxeterElt.actOnVectorColumn(root);
  }
  return root == input;
}

Rational WeylGroupData::getLongestRootLengthSquared() const {
  Rational result;
  result = this->cartanSymmetric(0, 0);
  for (int i = 1; i < this->cartanSymmetric.numberOfRows; i ++) {
    result = MathRoutines::maximum(result, this->cartanSymmetric(i, i));
  }
  return result;
}

bool WeylGroupData::isElementWeylGroup(
  const MatrixTensor<Rational>& matrix
) {
  STACK_TRACE("WeylGroup::isElementWeylGroup");
  Vector<Rational> rhoImage;
  matrix.actOnVectorColumn(this->rho, rhoImage);
  ElementWeylGroup elementCandidate;
  this->raiseToDominantWeight(
    rhoImage, nullptr, nullptr, &elementCandidate
  );
  Matrix<Rational> candidateMatrix, inputMatrix;
  matrix.getMatrix(inputMatrix, this->getDimension());
  this->getMatrixStandardRepresentation(elementCandidate, candidateMatrix);
  return candidateMatrix == inputMatrix;
}

bool WeylGroupData::containsRootNonStronglyPerpendicularTo(
  Vectors<Rational>& vectors, Vector<Rational>& input
) {
  for (int i = 0; i < this->group.elements.size; i ++) {
    if (this->isARoot(vectors[i] + input)) {
      return true;
    }
  }
  return false;
}

void WeylGroupData::getMatrixStandardRepresentation(
  const ElementWeylGroup& input, Matrix<Rational>& outputMatrix
) const {
  Vector<Rational> root;
  int dimension = this->cartanSymmetric.numberOfRows;
  outputMatrix.initialize(dimension, dimension);
  for (int i = 0; i < dimension; i ++) {
    root.makeEi(dimension, i);
    this->actOn(input, root, root);
    for (int j = 0; j < dimension; j ++) {
      outputMatrix(j, i) = root[j];
    }
  }
}

int WeylGroupData::numberOfRootsConnectedTo(
  Vectors<Rational>& vectors, Vector<Rational>& input
) {
  int result = 0;
  for (int i = 0; i < this->group.elements.size; i ++) {
    if (
      !Vector<Rational>::scalarProduct(
        vectors[i], input, this->cartanSymmetric
      ).isEqualToZero()
    ) {
      result ++;
    }
  }
  return result;
}

void WeylGroupData::computeRho(bool recompute) {
  if (this->rootSystem.size == 0 || recompute) {
    this->generateRootSystem();
  }
  this->rho.makeZero(this->cartanSymmetric.numberOfRows);
  for (int i = 0; i < this->rootSystem.size; i ++) {
    if (this->rootSystem[i].isPositiveOrZero()) {
      this->rho += rootSystem[i];
    }
  }
  for (int i = 0; i < this->cartanSymmetric.numberOfColumns; i ++) {
    this->rho[i].divideByInteger(2);
  }
  this->flagFundamentalToSimpleMatricesAreComputed = false;
}

std::string
SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::
toStringFromLayersAndArrows(
  List<List<List<int> > >& arrows,
  List<List<int> >& layers,
  int graphWidth,
  bool useAmbientIndices
) {
  STACK_TRACE(
    "SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms"
    "::toStringFromLayersAndArrows"
  );
  this->checkInitialization();
  std::stringstream out;
  List<int> displayIndicesSimpleGenerators;
  if (!useAmbientIndices) {
    displayIndicesSimpleGenerators.setSize(this->simpleRootsInner.size);
    for (int i = 0; i < this->simpleRootsInner.size; i ++) {
      displayIndicesSimpleGenerators[i] =
      this->ambientWeyl->rootsOfBorel.getIndex(this->simpleRootsInner[i]) + 1;
    }
  }
  out << "\\xymatrix{";
  bool GraphWidthIsOdd = ((graphWidth % 2) != 0);
  if (!GraphWidthIsOdd) {
    graphWidth ++;
  }
  for (int i = 0; i < layers.size; i ++) {
    int currentRowOffset = (graphWidth - layers[i].size) / 2;
    int nextRowOffset = - 1;
    if (i < layers.size - 1) {
      nextRowOffset = (graphWidth - layers[i + 1].size) / 2;
    }
    for (int j = 0; j < currentRowOffset; j ++) {
      out << "&";
    }
    for (int j = 0; j < layers[i].size; j ++) {
      if (!useAmbientIndices) {
        out << this->allElements[layers[i][j]].toString(nullptr);
      } else {
        out
        << this->RepresentativesQuotientAmbientOrder[layers[i][j]].toString();
      }
      int currentOffset = j + currentRowOffset;
      if (layers[i].size % 2 == 0) {
        if (currentOffset >= graphWidth / 2) {
          currentOffset ++;
        }
      }
      for (int k = 0; k < arrows[i][j].size; k ++) {
        out << " \\ar[d";
        int indexInLayer = layers[i + 1].getIndex(arrows[i][j][k]);
        if (indexInLayer == - 1) {
          global.fatal << "Negative index in layer ... " << global.fatal;
        }
        int nextOffset = indexInLayer + nextRowOffset;
        if (layers[i + 1].size % 2 == 0) {
          if (nextOffset >= graphWidth / 2) {
            nextOffset ++;
          }
        }
        int actualOffset = - currentOffset + nextOffset;
        for (int l = 0; l < actualOffset; l ++) {
          out << "r";
        }
        for (int l = 0; l > actualOffset; l --) {
          out << "l";
        }
        out << "]";
      }
      out << " & ";
      if (layers[i].size % 2 == 0 && j == layers[i].size / 2 - 1) {
        out << " & ";
      }
    }
    out << " \\\\\n";
  }
  out << "}";
  return out.str();
}

std::string
SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::
toStringBruhatGraph() {
  STACK_TRACE(
    "SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms"
    "::toStringBruhatGraph"
  );
  this->checkInitialization();
  if (this->allElements.size < 1) {
    return "Error, non-initialized group";
  }
  if (this->allElements.size == 1) {
    return "id";
  }
  List<List<List<int> > > arrows;
  List<List<int> > layers;
  Vector<Rational> root;
  layers.reserve(this->allElements.size);
  int GraphWidth = 1;
  int oldLayerElementLength = - 1;
  for (int i = 0; i < this->allElements.size; i ++) {
    const
    ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms
    & currentElement =
    this->allElements[i];
    if (
      currentElement.generatorsLastAppliedFirst.size != oldLayerElementLength
    ) {
      layers.setSize(layers.size + 1);
      oldLayerElementLength = currentElement.generatorsLastAppliedFirst.size;
    }
    layers.lastObject()->addOnTop(i);
    GraphWidth =
    MathRoutines::maximum(GraphWidth, layers.lastObject()->size);
  }
  HashedList<Vector<Rational> > orbit;
  orbit.reserve(this->allElements.size);
  for (int i = 0; i < this->allElements.size; i ++) {
    this->actByNonSimpleElement(i, this->ambientWeyl->rho, root);
    orbit.addOnTop(root);
  }
  arrows.setSize(layers.size);
  for (int i = 0; i < layers.size; i ++) {
    arrows[i].setSize(layers[i].size);
    for (int j = 0; j < layers[i].size; j ++) {
      int indexOther = layers[i][j];
      for (int k = 0; k < this->simpleRootsInner.size; k ++) {
        this->ambientWeyl->reflectBetaWithRespectToAlpha(
          this->simpleRootsInner[k], orbit[indexOther], false, root
        );
        int index = orbit.getIndex(root);
        if (index == - 1) {
          global.fatal << "Negative index not allowed. " << global.fatal;
        }
        const
        ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms
        & currentElement =
        this->allElements[index];
        if (
          currentElement.generatorsLastAppliedFirst.size >
          this->allElements[indexOther].generatorsLastAppliedFirst.size
        ) {
          arrows[i][j].addOnTop(index);
        }
      }
    }
  }
  return
  this->toStringFromLayersAndArrows(arrows, layers, GraphWidth, false);
}

void SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::
toString(std::string& output, bool displayElements) {
  STACK_TRACE(
    "SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms"
    "::toString"
  );
  this->checkInitialization();
  std::stringstream out, head, head2;
  List<int> displayIndicesSimpleGenerators;
  displayIndicesSimpleGenerators.setSize(this->simpleRootsInner.size);
  FormatExpressions latexFormat;
  latexFormat.flagUseHTML = false;
  latexFormat.flagUseLatex = true;
  bool isGood = true;
  for (int i = 0; i < this->simpleRootsInner.size; i ++) {
    displayIndicesSimpleGenerators[i] =
    this->ambientWeyl->rootsOfBorel.getIndex(this->simpleRootsInner[i]) + 1;
    if (displayIndicesSimpleGenerators[i] == 0) {
      isGood = false;
      break;
    }
  }
  if (!isGood) {
    for (int i = 0; i < this->simpleRootsInner.size; i ++) {
      displayIndicesSimpleGenerators[i] = i + 1;
    }
  }
  DynkinDiagramRootSubalgebra tempDyn;
  tempDyn.ambientRootSystem = this->ambientWeyl->rootSystem;
  tempDyn.ambientBilinearForm = this->ambientWeyl->cartanSymmetric;
  tempDyn.computeDiagramInputIsSimple(this->simpleRootsInner);
  out
  << "Dynkin diagram & subalgebra of "
  << "root subsystem generated by the given root: "
  << tempDyn.toString();
  out << "<br>Simple roots:\n<br>\n ";
  head << "\\begin{array}{rcl}";
  for (int i = 0; i < this->simpleRootsInner.size; i ++) {
    head
    << "\n\\eta_{"
    << displayIndicesSimpleGenerators[i]
    << "}&= &"
    << this->simpleRootsInner[i].toString()
    << "\\\\";
  }
  head << "\\end{array}";
  out << HtmlRoutines::getMathNoDisplay(head.str());
  if (this->externalAutomorphisms.size > 0) {
    out << "<br>Outer automorphisms: \n";
    Matrix<Rational> automorphismMatrix;
    head2 << "\\begin{array}{rcl}";
    for (int i = 0; i < this->externalAutomorphisms.size; i ++) {
      automorphismMatrix.assignVectorsToRows(
        this->externalAutomorphisms[i]
      );
      automorphismMatrix.transpose();
      head2
      << "a_{"
      << i + 1
      << "}&= &"
      << automorphismMatrix.toString(&latexFormat)
      << "\\\\";
    }
    head2 << "\\end{array}";
    out << HtmlRoutines::getMathNoDisplay(head2.str());
  }
  out << "<br>Half sum of the positive roots: " << this->getRho().toString();
  out << "<br>Roots of Borel (" << this->rootsOfBorel.size << " total): ";
  for (int i = 0; i < this->rootsOfBorel.size; i ++) {
    out << this->rootsOfBorel[i].toString();
  }
  if (displayElements) {
    std::stringstream body;
    out
    << "<br>The elements of the automorphisms of the "
    << "Weyl group of the subgroup "
    << "written with minimal # of generators:<br>";
    body << "\\begin{array}{l}";
    for (int i = 0; i < this->allElements.size; i ++) {
      const
      ElementSubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms
      & currentElt =
      this->allElements[i];
      body << currentElt.toString(nullptr) << "\\\\";
    }
    body << "\\end{array}";
    out << HtmlRoutines::getMathNoDisplay(body.str());
  }
  output = out.str();
}

bool SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::
makeParabolicFromSelectionSimpleRoots(
  WeylGroupData& inputWeyl,
  const Selection& zeroesMeanSimpleRootSpaceIsInParabolic,
  int upperLimitNumberOfElements
) {
  STACK_TRACE(
    "SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms"
    "::makeParabolicFromSelectionSimpleRoots"
  );
  this->ambientWeyl = &inputWeyl;
  this->checkInitialization();
  Vectors<Rational> selectedRoots;
  selectedRoots.reserve(
    zeroesMeanSimpleRootSpaceIsInParabolic.numberOfElements -
    zeroesMeanSimpleRootSpaceIsInParabolic.cardinalitySelection
  );
  if (
    this->ambientWeyl->getDimension() !=
    zeroesMeanSimpleRootSpaceIsInParabolic.numberOfElements
  ) {
    global.fatal
    << "Parabolic selection selects out of "
    << zeroesMeanSimpleRootSpaceIsInParabolic.numberOfElements
    << " elements while the Weyl group is of rank "
    << this->ambientWeyl->getDimension()
    << ". "
    << global.fatal;
  }
  for (
    int i = 0; i < zeroesMeanSimpleRootSpaceIsInParabolic.numberOfElements; i
    ++
  ) {
    if (!zeroesMeanSimpleRootSpaceIsInParabolic.selected[i]) {
      selectedRoots.setSize(selectedRoots.size + 1);
      selectedRoots.lastObject()->makeEi(inputWeyl.getDimension(), i);
    }
  }
  List<Vectors<Rational> > rootsCol;
  return
  this->computeSubGroupFromGeneratingReflections(
    &selectedRoots, &rootsCol, upperLimitNumberOfElements, true
  );
}

std::string
SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::
toStringCosetGraph() {
  STACK_TRACE(
    "SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms"
    "::toStringCosetGraph"
  );
  this->checkInitialization();
  if (this->allElements.size < 1) {
    return "Error, non-initialized group";
  }
  if (this->allElements.size == 1) {
    return "id";
  }
  List<List<List<int> > > arrows;
  List<List<int> > layers;
  Vector<Rational> root;
  layers.reserve(this->RepresentativesQuotientAmbientOrder.size);
  int GraphWidth = 1;
  int oldLayerElementLength = - 1;
  for (
    int i = 0; i < this->RepresentativesQuotientAmbientOrder.size; i ++
  ) {
    if (
      this->RepresentativesQuotientAmbientOrder[i].generatorsLastAppliedFirst.
      size !=
      oldLayerElementLength
    ) {
      layers.setSize(layers.size + 1);
      oldLayerElementLength =
      this->RepresentativesQuotientAmbientOrder[i].generatorsLastAppliedFirst.
      size;
    }
    layers.lastObject()->addOnTop(i);
    GraphWidth =
    MathRoutines::maximum(GraphWidth, layers.lastObject()->size);
  }
  for (
    int i = 0; i < this->RepresentativesQuotientAmbientOrder.size; i ++
  ) {
    root = this->ambientWeyl->rho;
    this->ambientWeyl->actOnRootByGroupElement(
      this->ambientWeyl->group.elements.getIndex(
        this->RepresentativesQuotientAmbientOrder[i]
      ),
      root,
      false,
      false
    );
  }
  arrows.setSize(layers.size);
  for (int i = 0; i < layers.size; i ++) {
    arrows[i].setSize(layers[i].size);
    for (int j = 0; j < layers[i].size; j ++) {
      for (
        int k = 0; k < this->RepresentativesQuotientAmbientOrder.size; k ++
      ) {
        if (
          this->ambientWeyl->leftIsHigherInBruhatOrderThanRight(
            this->RepresentativesQuotientAmbientOrder[k],
            this->RepresentativesQuotientAmbientOrder[layers[i][j]]
          )
        ) {
          if (
            this->RepresentativesQuotientAmbientOrder[layers[i][j]].
            generatorsLastAppliedFirst.size ==
            this->RepresentativesQuotientAmbientOrder[k].
            generatorsLastAppliedFirst.size -
            1
          ) {
            arrows[i][j].addOnTop(k);
          }
        }
      }
    }
  }
  return this->toStringFromLayersAndArrows(arrows, layers, GraphWidth, true);
}

void SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::
findQuotientRepresentatives(int upperLimit) {
  STACK_TRACE(
    "SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms"
    "::findQuotientRepresentatives"
  );
  this->checkInitialization();
  this->ambientWeyl->group.computeAllElements(upperLimit);
  Vector<Rational> image1;
  this->RepresentativesQuotientAmbientOrder.size = 0;
  this->RepresentativesQuotientAmbientOrder.reserve(
    this->ambientWeyl->group.elements.size
  );
  for (int i = 0; i < this->ambientWeyl->group.elements.size; i ++) {
    image1 = this->ambientWeyl->rho;
    this->ambientWeyl->actOnRootByGroupElement(i, image1, false, false);
    bool isGood = true;
    for (int j = 0; j < this->simpleRootsInner.size; j ++) {
      if (
        this->ambientWeyl->rootScalarCartanRoot(
          image1, this->simpleRootsInner[j]
        ).isNegative()
      ) {
        isGood = false;
        break;
      }
    }
    if (isGood) {
      this->RepresentativesQuotientAmbientOrder.addOnTop(
        this->ambientWeyl->group.elements[i]
      );
    }
  }
}

bool SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::
drawContour(
  const Vector<Rational>& highestWeightSimpleCoord,
  DrawingVariables& drawingVariables,
  const std::string& color,
  int upperBoundVertices
) {
  STACK_TRACE(
    "SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms"
    "::drawContour"
  );
  HashedList<Vector<Rational> > orbit;
  orbit.addOnTop(highestWeightSimpleCoord);
  WeylGroupData& ambientWeylGroup = *this->ambientWeyl;
  Vector<Rational> root;
  for (int i = 0; i < orbit.size; i ++) {
    for (int j = 0; j < this->simpleRootsInner.size; j ++) {
      root = orbit[i];
      ambientWeylGroup.reflectBetaWithRespectToAlpha(
        this->simpleRootsInner[j], root, false, root
      );
      if (orbit.addOnTopNoRepetition(root)) {
        drawingVariables.drawLineBetweenTwoVectorsBufferRational(
          orbit[i], root, color, 1
        );
      }
      if (orbit.size > upperBoundVertices) {
        return false;
      }
    }
  }
  return true;
}

SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::
SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms() {
  this->ambientWeyl = nullptr;
  this->flagDeallocated = false;
}

SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::~
SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms() {
  this->ambientWeyl = nullptr;
  this->flagDeallocated = true;
}

bool SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::
checkInitialization() {
  // if (this == 0)
  // global.fatal << "Subgroup of Weyl Group has 0 this pointer. " <<
  // global.fatal;
  if (this->ambientWeyl == nullptr) {
    global.fatal
    << "Use of non-initialized subgroup of Weyl Group. "
    << global.fatal;
  }
  if (this->flagDeallocated) {
    global.fatal
    << "Use after free of subgroup of a Weyl group. "
    << global.fatal;
  }
  if (this->ambientWeyl->flagDeallocated) {
    global.fatal
    << "Use after free of owner Weyl groups in a subgroup. "
    << global.fatal;
  }
  return true;
}

void SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::
computeRootSubsystem() {
  STACK_TRACE(
    "SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms"
    "::computeRootSubsystem"
  );
  this->rootSubsystem.clear();
  this->rootSubsystem.addListOnTop(this->simpleRootsInner);
  this->rootSubsystem.setExpectedSize(100);
  Vector<Rational> currentRoot;
  for (int i = 0; i < this->rootSubsystem.size; i ++) {
    for (int j = 0; j < this->simpleRootsInner.size; j ++) {
      currentRoot = this->rootSubsystem[i];
      this->ambientWeyl->reflectBetaWithRespectToAlpha(
        this->simpleRootsInner[j], currentRoot, false, currentRoot
      );
      this->rootSubsystem.addOnTopNoRepetition(currentRoot);
    }
  }
  Vectors<Rational> roots;
  roots = this->rootSubsystem;
  roots.quickSortAscending();
  this->rootSubsystem = roots;
  if (this->rootSubsystem.size % 2 != 0) {
    global.fatal
    << "I am getting that the number of weights of a root system is odd. "
    << "The generating set of simple weights is "
    << this->simpleRootsInner.toString()
    << ", and the "
    << "generated weight subsystem is "
    << roots.toString()
    << global.fatal;
  }
  int numPosRoots = this->rootSubsystem.size / 2;
  this->rootsOfBorel.setSize(numPosRoots);
  for (int i = 0; i < numPosRoots; i ++) {
    this->rootsOfBorel[i] = this->rootSubsystem[i + numPosRoots];
  }
}

void KazhdanLusztigPolynomials::writeKLCoeffsToFile(
  std::fstream& output, List<int>& KLcoeff, int topIndex
) {
  output.clear();
  output << "Top_index: " << topIndex << "\n";
  std::string tempS;
  this->kazhdanLuzstigCoefficientsToString(KLcoeff, tempS);
  output << tempS;
}

int KazhdanLusztigPolynomials::readKLCoeffsFromFile(
  std::fstream& input, List<int>& output
) {
  std::string tempS;
  int topIndex;
  input >> tempS >> topIndex;
  output.setSize(this->size);
  for (int i = 0; i < this->size; i ++) {
    input >> tempS >> output[i];
  }
  return topIndex;
}

void KazhdanLusztigPolynomials::kazhdanLuzstigCoefficientsToString(
  List<int>& kazhdanLuzstigCoefficients, std::string& output
) {
  std::stringstream out;
  for (int i = 0; i < kazhdanLuzstigCoefficients.size; i ++) {
    out << i << ".  " << kazhdanLuzstigCoefficients[i] << "\n";
  }
  output = out.str();
}

void KazhdanLusztigPolynomials::initTheMults() {
  this->multiplicities.setSize(this->size);
  this->explored.setSize(this->size);
  for (int i = 0; i < this->size; i ++) {
    this->multiplicities[i] = 0;
    this->explored[i] = false;
  }
  this->nextToExplore = 0;
  this->lowestNonExplored = 0;
}

void KazhdanLusztigPolynomials::check() {
  for (int i = 0; i < this->size; i ++) {
    this->compute(i);
  }
}

void KazhdanLusztigPolynomials::compute(int x) {
  this->initTheMults();
  this->multiplicities[x] = 1;
  while (this->nextToExplore != - 1) {
    for (
      int i = 0; i < this->bruhatOrder[this->nextToExplore].size; i ++
    ) {
      int a = this->bruhatOrder[this->nextToExplore][i];
      this->multiplicities[a] -= this->multiplicities[this->nextToExplore];
    }
    this->explored[this->nextToExplore] = true;
    this->findNextToExplore();
  }
}

void KazhdanLusztigPolynomials::findNextToExplore() {
  bool foundNonExplored = false;
  for (int i = this->lowestNonExplored; i < this->size; i ++) {
    if (!this->explored[i]) {
      if (!foundNonExplored) {
        this->lowestNonExplored = i;
        foundNonExplored = true;
      }
      if (this->isMaximalNonExplored(i)) {
        this->nextToExplore = i;
        return;
      }
    }
  }
  this->nextToExplore = - 1;
}

bool KazhdanLusztigPolynomials::isMaximalNonExplored(int index) {
  for (int i = this->lowestNonExplored; i < this->size; i ++) {
    if (!this->explored[i] && i != index) {
      Vector<Rational> difference;
      difference = (*this)[i];
      difference -= (*this)[index];
      if (difference.isPositiveOrZero()) {
        return false;
      }
    }
  }
  return true;
}

std::string KazhdanLusztigPolynomials::toString(FormatExpressions* format) {
  STACK_TRACE("KazhdanLusztigPolynomials::toString");
  std::stringstream out;
  bool useHtml = false;
  if (format != nullptr) {
    useHtml = format->flagUseHTML;
  }
  if (!useHtml) {
    out
    << "Next to explore: "
    << this->nextToExplore
    << "<br>\n Orbit of rho:<br>\n";
    for (int i = 0; i < this->size; i ++) {
      out
      << this->objects[i].toString()
      << "   :  "
      << this->multiplicities[i];
      if (this->explored[i]) {
        out << " Explored<br>\n";
      } else {
        out << " not Explored<br>\n";
      }
    }
    out << "Bruhat order:<br>\n";
    for (int i = 0; i < this->size; i ++) {
      out << i << ".   ";
      for (int j = 0; j < this->bruhatOrder[i].size; j ++) {
        out << this->bruhatOrder[i][j] << ", ";
      }
      out << "<br>\n";
    }
  }
  out << "R Polynomials:<br>" << this->rPolysToString(format);
  if (
    this->kazhdanLuzstigCoefficients.size ==
    this->weylGroup->group.elements.size
  ) {
    out << "Kazhdan-Lusztig Polynomials:<br>" << this->KLPolysToString(format);
    out
    << "Kazhdan-Lusztig coefficients; the (w_1,w_2) "
    << "coefficient is defined as the multiplicity of "
    << HtmlRoutines::getMathNoDisplay("L_{w_2 \\cdot \\lambda}")
    << " in "
    << HtmlRoutines::getMathNoDisplay(" M_{w_1\\cdot \\lambda }  ")
    << " where \\cdot stands for the \\rho-modified action"
    << " of the Weyl group, \\lambda is a "
    << "dominant integral weight, "
    << "M_{\\lambda} stands for Verma module "
    << "of highest weight \\lambda, L_\\lambda stands for irreducible highest "
    << "weight of highest weight \\lambda: "
    << "<br><table border =\"1\"><tr><td>Weyl elt.</td>";
    for (int i = 0; i < this->weylGroup->group.elements.size; i ++) {
      out
      << "<td>"
      << this->weylGroup->group.elements[i].toString()
      << "</td>";
    }
    out << "</tr>";
    for (int i = 0; i < this->weylGroup->group.elements.size; i ++) {
      if (this->kazhdanLuzstigPolynomials[i].size > 0) {
        out
        << "<tr>"
        << "<td>"
        << this->weylGroup->group.elements[i].toString()
        << "</td>";
        for (
          int j = 0; j < this->kazhdanLuzstigCoefficients[i].size; j ++
        ) {
          out
          << "<td>"
          << kazhdanLuzstigCoefficients[i][j].toString()
          << "</td>";
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

void KazhdanLusztigPolynomials::generatePartialBruhatOrder() {
  STACK_TRACE("KazhdanLusztigPolynomials::generatePartialBruhatOrder");
  int dimension = this->weylGroup->cartanSymmetric.numberOfRows;
  Vector<Rational> zeroRoot;
  zeroRoot.makeZero(dimension);
  this->bruhatOrder.setSize(this->size);
  this->inverseBruhatOrder.setSize(this->size);
  this->simpleReflectionsActionList.setSize(this->size);
  for (int i = 0; i < this->size; i ++) {
    this->simpleReflectionsActionList[i].reserve(dimension);
    for (int j = 0; j < dimension; j ++) {
      Vector<Rational> root, root2;
      root = (*this)[i];
      root2 = (*this)[i];
      this->weylGroup->reflectSimple(j, root, false, false);
      int x = this->getIndex(root);
      if (x == - 1) {
        global.fatal
        << "Something wrong has happened. "
        << "A weight that is supposed to "
        << "be in a certain Weyl group orbit isn't there. "
        << "There is an error in the code, crashing accordingly. "
        << global.fatal;
      }
      this->simpleReflectionsActionList[i].addOnTop(x);
      root2 -= root;
      if (root2.isPositiveOrZero() && !root2.isEqualToZero()) {
        this->bruhatOrder[i].addOnTop(x);
        this->inverseBruhatOrder[x].addOnTop(i);
      }
    }
  }
}

int KazhdanLusztigPolynomials::findMinimalBruhatNonExplored(
  List<bool>& explored
) {
  int lowestIndex = - 1;
  for (int i = 0; i < this->size; i ++) {
    if (!explored[i]) {
      if (lowestIndex == - 1) {
        lowestIndex = i;
      } else {
        if (this->indexGreaterThanIndex(lowestIndex, i)) {
          lowestIndex = i;
        }
      }
    }
  }
  return lowestIndex;
}

int KazhdanLusztigPolynomials::findMaximalBruhatNonExplored(
  List<bool>& explored
) {
  int highestIndex = - 1;
  for (int i = 0; i < this->size; i ++) {
    if (!explored[i]) {
      if (highestIndex == - 1) {
        highestIndex = i;
      } else {
        if (this->indexGreaterThanIndex(i, highestIndex)) {
          highestIndex = i;
        }
      }
    }
  }
  return highestIndex;
}

void KazhdanLusztigPolynomials::mergeBruhatLists(int fromList, int toList) {
  for (int i = 0; i < this->bruhatOrder[fromList].size; i ++) {
    bool found = false;
    for (int j = 0; j < this->bruhatOrder[toList].size; j ++) {
      if (
        this->bruhatOrder[toList][j] == this->bruhatOrder[fromList][i]
      ) {
        found = true;
        break;
      }
    }
    if (!found) {
      this->bruhatOrder[toList].addOnTop(
        this->bruhatOrder[fromList][i]
      );
    }
  }
}

int KazhdanLusztigPolynomials::chamberIndicatorToIndex(
  Vector<Rational>& ChamberIndicator
) {
  int dimension = this->weylGroup->cartanSymmetric.numberOfRows;
  Vector<Rational> root;
  root.setSize(dimension);
  Vector<Rational> ChamberIndicatorPlusRho;
  ChamberIndicatorPlusRho = (ChamberIndicator);
  ChamberIndicatorPlusRho += this->weylGroup->rho;
  for (int i = 0; i < this->size; i ++) {
    Rational scalarProduct1;
    Rational scalarProduct2;
    bool firstIsPositive = false;
    bool secondIsPositive = false;
    bool haveSameSigns = true;
    for (int j = 0; j < this->weylGroup->rootSystem.size; j ++) {
      this->weylGroup->rootScalarCartanRoot(
        ChamberIndicatorPlusRho,
        this->weylGroup->rootSystem[j],
        scalarProduct1
      );
      root = (*this)[i];
      root += (this->weylGroup->rho);
      this->weylGroup->rootScalarCartanRoot(
        root, this->weylGroup->rootSystem[j], scalarProduct2
      );
      firstIsPositive = scalarProduct1.isPositive();
      secondIsPositive = scalarProduct2.isPositive();
      if (
        scalarProduct1.isEqualToZero() || scalarProduct2.isEqualToZero()
      ) {
        global.fatal
        << "Coefficients are zero at a bad place. "
        << global.fatal;
      }
      if (firstIsPositive != secondIsPositive) {
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

void KazhdanLusztigPolynomials::computeKLCoefficients() {
  STACK_TRACE("KazhdanLusztigPolynomials::computeKLCoefficients");
  this->kazhdanLuzstigCoefficients.setSize(
    this->kazhdanLuzstigPolynomials.size
  );
  for (int i = 0; i < this->kazhdanLuzstigPolynomials.size; i ++) {
    this->kazhdanLuzstigCoefficients[i].setSize(
      this->kazhdanLuzstigPolynomials[i].size
    );
    for (
      int j = 0; j < this->kazhdanLuzstigCoefficients[i].size; j ++
    ) {
      Polynomial<Rational>& currentPoly =
      this->kazhdanLuzstigPolynomials[i][j];
      this->kazhdanLuzstigCoefficients[i][j] = 0;
      if (this->indexGEQIndex(j, i)) {
        for (int k = 0; k < currentPoly.size(); k ++) {
          this->kazhdanLuzstigCoefficients[i][j] +=
          currentPoly.coefficients[k];
        }
      }
    }
  }
}

void KazhdanLusztigPolynomials::initFromWeyl(WeylGroupData* weylGroup) {
  this->weylGroup = weylGroup;
  Vectors<Rational> roots;
  this->weylGroup->computeRho(true);
  roots.addOnTop(this->weylGroup->rho);
  this->weylGroup->generateOrbit(roots, false, *this, false);
  this->initTheMults();
}

bool KazhdanLusztigPolynomials::computeKLPolys(WeylGroupData* weylGroup) {
  STACK_TRACE("KazhdanLusztigPolynomials::computeKLPolys");
  weylGroup->group.computeAllElements(- 1);
  this->initFromWeyl(weylGroup);
  this->generatePartialBruhatOrder();
  FormatExpressions PolyFormatLocal;
  PolyFormatLocal.polynomialDefaultLetter = "q";
  this->computeRPolys();
  this->kazhdanLuzstigPolynomials.setSize(this->size);
  this->kazhdanLuzstigCoefficients.setSize(this->size);
  this->explored.initializeFillInObject(this->size, false);
  for (int i = 0; i < this->kazhdanLuzstigPolynomials.size; i ++) {
    this->kazhdanLuzstigPolynomials[i].setSize(this->size);
    this->kazhdanLuzstigCoefficients[i].setSize(this->size);
  }
  for (int i = 0; i < this->size; i ++) {
    this->explored.initializeFillInObject(this->size, false);
    int highestNonExplored =
    this->findMaximalBruhatNonExplored(this->explored);
    while (highestNonExplored != - 1) {
      this->computeKLxy(highestNonExplored, i);
      this->explored[highestNonExplored] = true;
      highestNonExplored = this->findMaximalBruhatNonExplored(this->explored);
    }
  }
  this->computeKLCoefficients();
  return true;
}

void KazhdanLusztigPolynomials::computeRPolys() {
  STACK_TRACE("KazhdanLusztigPolynomials::computeRPolys");
  int dimension = this->weylGroup->getDimension();
  this->rPolynomials.setSize(this->size);
  for (int i = 0; i < this->size; i ++) {
    this->explored[i] = false;
    this->rPolynomials[i].setSize(this->size);
  }
  this->lowestNonExplored =
  this->findMinimalBruhatNonExplored(this->explored);
  List<bool> ExploredFromTop;
  ExploredFromTop.setSize(this->size);
  while (this->lowestNonExplored != - 1) {
    for (int i = 0; i < this->size; i ++) {
      ExploredFromTop[i] = false;
    }
    int a = this->findMaximalBruhatNonExplored(ExploredFromTop);
    while (a != - 1) {
      bool tempBool = false;
      for (int j = 0; j < dimension; j ++) {
        if (this->computeRxy(a, this->lowestNonExplored, j)) {
          tempBool = true;
          break;
        }
      }
      if (!tempBool) {
        global.fatal
        << "An algorithmic check failed while computing R-polynomials. "
        << global.fatal;
      }
      ExploredFromTop[a] = true;
      a = this->findMaximalBruhatNonExplored(ExploredFromTop);
    }
    this->explored[this->lowestNonExplored] = true;
    this->lowestNonExplored =
    this->findMinimalBruhatNonExplored(this->explored);
  }
  // this->ComputeDebugString();
}

bool KazhdanLusztigPolynomials::indexGEQIndex(int a, int b) {
  Vector<Rational> tempV;
  tempV = (*this)[a];
  tempV -= (*this)[b];
  return tempV.isNegativeOrZero();
}

bool KazhdanLusztigPolynomials::indexGreaterThanIndex(int a, int b) {
  if (a == b) {
    return false;
  }
  return this->indexGEQIndex(a, b);
}

int KazhdanLusztigPolynomials::computeProductfromSimpleReflectionsActionList(
  int x, int y
) {
  int start = y;
  const ElementWeylGroup& currentElement = this->weylGroup->group.elements[x];
  for (
    int i = currentElement.generatorsLastAppliedFirst.size - 1; i >= 0; i --
  ) {
    start =
    this->simpleReflectionsActionList[start][
      this->weylGroup->group.elements[x].generatorsLastAppliedFirst[i].index
    ];
  }
  return start;
}

void KazhdanLusztigPolynomials::computeKLxy(int x, int y) {
  Polynomial<Rational> Accum, tempP1, tempP2;
  if (x == y) {
    this->kazhdanLuzstigPolynomials[x][y].makeOne();
    return;
  }
  if (!this->indexGEQIndex(y, x)) {
    this->kazhdanLuzstigPolynomials[x][y].makeZero();
    return;
  }
  Accum.makeZero();
  MonomialPolynomial tempM;
  for (int i = 0; i < this->size; i ++) {
    if (
      this->indexGreaterThanIndex(i, x) && this->indexGEQIndex(y, i)
    ) {
      tempP1.makeZero();
      for (int j = 0; j < this->rPolynomials[x][i].size(); j ++) {
        tempM = this->rPolynomials[x][i][j];
        tempM.invert();
        tempP1.addMonomial(
          tempM, this->rPolynomials[x][i].coefficients[j]
        );
      }
      int sign = 0;
      if ((
          this->weylGroup->group.elements[x].generatorsLastAppliedFirst.size +
          this->weylGroup->group.elements[i].generatorsLastAppliedFirst.size
        ) %
        2 ==
        0
      ) {
        sign = 1;
      } else {
        sign = - 1;
      }
      Rational powerQ =
      - this->weylGroup->group.elements[x].generatorsLastAppliedFirst.size +
      2 *
      this->weylGroup->group.elements[i].generatorsLastAppliedFirst.size -
      this->weylGroup->group.elements[y].generatorsLastAppliedFirst.size;
      powerQ /= 2;
      tempP2.makeMonomial(0, powerQ, sign);
      tempP1 *= tempP2;
      tempP1 *= this->kazhdanLuzstigPolynomials[i][y];
      if (!this->explored[i]) {
        global.fatal
        << "An internal check during the "
        << "Kazhdan-Lusztig polynomial computation fails. "
        << "More precisely, while computing "
        << "KL poly of indices "
        << x
        << ", "
        << y
        << " I am using KL poly with indices "
        << i
        << ", "
        << y
        << " which hasn't been computed yet. "
        << global.fatal;
      }
      Accum += tempP1;
    }
  }
  this->kazhdanLuzstigPolynomials[x][y].makeZero();
  Rational lengthDiff =
  this->weylGroup->group.elements[y].generatorsLastAppliedFirst.size -
  this->weylGroup->group.elements[x].generatorsLastAppliedFirst.size;
  lengthDiff /= 2;
  for (int i = 0; i < Accum.size(); i ++) {
    if (Accum[i].hasPositiveOrZeroExponents()) {
      tempM = Accum[i];
      tempM.setVariable(0, tempM[0] * - 1);
      tempM.multiplyByVariable(0, lengthDiff);
      this->kazhdanLuzstigPolynomials[x][y].addMonomial(
        tempM, Accum.coefficients[i]
      );
    }
  }
}

bool KazhdanLusztigPolynomials::computeRxy(
  int x, int y, int simpleReflectionIndex
) {
  STACK_TRACE("KazhdanLusztigPolynomials::computeRxy");
  if (x == y) {
    this->rPolynomials[x][y].makeOne();
    return true;
  }
  if (this->indexGreaterThanIndex(x, y)) {
    this->rPolynomials[x][y].makeZero();
    return true;
  }
  int sx = this->simpleReflectionsActionList[x][simpleReflectionIndex];
  int sy = this->simpleReflectionsActionList[y][simpleReflectionIndex];
  bool boolX, boolY;
  boolX = this->indexGreaterThanIndex(x, sx);
  boolY = this->indexGreaterThanIndex(y, sy);
  if (boolX && boolY) {
    if (!this->explored[sy]) {
      global.fatal
      << "The computaion of R-polynomials "
      << "is attempting to use a non-computed R-polynomial. "
      << global.fatal;
    }
    this->rPolynomials[x][y] = this->rPolynomials[sx][sy];
    return true;
  }
  if (!boolX && boolY) {
    Polynomial<Rational> qMinus1;
    qMinus1.makeMonomial(0, 1, 1);
    this->rPolynomials[x][y] = qMinus1;
    this->rPolynomials[x][y] *= (this->rPolynomials[sx][sy]);
    qMinus1 -= 1;
    qMinus1 *= this->rPolynomials[sx][y];
    this->rPolynomials[x][y] += qMinus1;
    return true;
  }
  return false;
}

std::string KazhdanLusztigPolynomials::KLPolysToString(
  FormatExpressions* format
) {
  std::stringstream out;
  out << "<table border =\"1\">";
  out << "<tr><td>Weyl elt.</td>";
  for (int i = 0; i < this->weylGroup->group.elements.size; i ++) {
    out << "<td>" << this->weylGroup->group.elements[i].toString() << "</td>";
  }
  out << "</tr>";
  for (int i = 0; i < this->kazhdanLuzstigPolynomials.size; i ++) {
    if (this->kazhdanLuzstigPolynomials[i].size > 0) {
      out
      << "<tr><td>"
      << this->weylGroup->group.elements[i].toString()
      << "</td>";
      for (
        int j = 0; j < this->kazhdanLuzstigPolynomials[i].size; j ++
      ) {
        out
        << "<td>"
        << this->kazhdanLuzstigPolynomials[i][j].toString(format)
        << "</td>";
      }
      out << "</tr>";
    }
  }
  out << "</table>";
  return out.str();
}

std::string KazhdanLusztigPolynomials::rPolysToString(
  FormatExpressions* format
) {
  std::stringstream out;
  out << "<table border =\"1\"><tr><td>Weyl elt.</td>";
  for (int i = 0; i < this->weylGroup->group.elements.size; i ++) {
    out << "<td>" << this->weylGroup->group.elements[i].toString() << "</td>";
  }
  out << "</tr>";
  for (int i = 0; i < this->rPolynomials.size; i ++) {
    out
    << "<tr><td>"
    << this->weylGroup->group.elements[i].toString()
    << "</td>";
    for (int j = 0; j < this->rPolynomials[i].size; j ++) {
      out << "<td>" << this->rPolynomials[i][j].toString(format) << "</td>\n";
    }
    out << "</tr>";
  }
  out << "</table>";
  return out.str();
}

void OnePartialFractionDenominator::evaluateIntegerPolynomial(
  const Polynomial<LargeInteger>& input,
  const Vector<Rational>& values,
  Rational& output
) {
  output.makeZero();
  Polynomial<Rational> rationalPolynomial;
  rationalPolynomial = input;
  // <-implicit type conversion here!
  output = rationalPolynomial.evaluate(values);
}

void LaTeXProcedures::beginPSTricks(std::fstream& output) {
  output << "\\begin{pspicture}(8, 8)";
}

void LaTeXProcedures::endPSTricks(std::fstream& output) {
  output << "\\end{pspicture}\\medskip \\\\ \\noindent ";
}

void LaTeXProcedures::beginDocument(std::fstream& output) {
  output
  << "\\documentclass{article}\n \\usepackage{pstricks} \\begin{document}";
}

void LaTeXProcedures::endLatexDocument(std::fstream& output) {
  output << "\\end{document}";
}

void LaTeXProcedures::getStringFromColorIndex(
  int colorIndex, std::string& output, DrawingVariables& drawInput
) {
  switch (colorIndex) {
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
  if (colorIndex == drawInput.getColorFromChamberIndex(1)) {
    output.assign("blue");
  }
  if (colorIndex == drawInput.getColorFromChamberIndex(2)) {
    output.assign("purple");
  }
  if (colorIndex == drawInput.getColorFromChamberIndex(3)) {
    output.assign("green");
  }
  if (colorIndex == drawInput.getColorFromChamberIndex(4)) {
    output.assign("cyan");
  }
  if (colorIndex == drawInput.getColorFromChamberIndex(5)) {
    output.assign("red");
  }
  if (colorIndex == drawInput.getColorFromChamberIndex(6)) {
    output.assign("purple");
  }
  if (colorIndex == drawInput.getColorFromChamberIndex(7)) {
    output.assign("cyan");
  }
}

void LaTeXProcedures::drawTextDirectly(
  double x1,
  double y1,
  const std::string& text,
  int colorIndex,
  std::fstream& output
) {
  (void) colorIndex;
  output.precision(4);
  x1 -= text.length() * LaTeXProcedures::TextPrintCenteringAdjustmentX;
  y1 += LaTeXProcedures::TextPrintCenteringAdjustmentY;
  x1 /= LaTeXProcedures::ScaleFactor;
  y1 /= LaTeXProcedures::ScaleFactor;
  output
  << "\\put("
  << x1 - LaTeXProcedures::FigureCenterCoordSystemX
  << ", "
  << LaTeXProcedures::FigureCenterCoordSystemY - y1
  << "){\\tiny{"
  << text
  << "}}";
}

void LaTeXProcedures::drawline(
  double x1,
  double y1,
  double x2,
  double y2,
  uint32_t penStyle,
  int colorIndex,
  std::fstream& output,
  DrawingVariables& drawInput
) {
  if (static_cast<int>(penStyle) == DrawingVariables::PenStyleInvisible) {
    return;
  }
  output.precision(4);
  x1 /= LaTeXProcedures::ScaleFactor;
  x2 /= LaTeXProcedures::ScaleFactor;
  y1 /= LaTeXProcedures::ScaleFactor;
  y2 /= LaTeXProcedures::ScaleFactor;
  std::string tempS;
  if (
    penStyle == static_cast<unsigned>(DrawingVariables::PenStyleDashed)
  ) {
    tempS = "lightgray";
  } else {
    LaTeXProcedures::getStringFromColorIndex(colorIndex, tempS, drawInput);
  }
  output
  << "\\psline[linewidth = 0.3pt, linecolor ="
  << tempS
  << "]("
  << x1 - LaTeXProcedures::FigureCenterCoordSystemX
  << ", "
  << LaTeXProcedures::FigureCenterCoordSystemY - y1
  << ")"
  << "("
  << x2 - LaTeXProcedures::FigureCenterCoordSystemX
  << ", "
  << LaTeXProcedures::FigureCenterCoordSystemY - y2
  << ")\n";
}

void WeylGroupData::transformToSimpleBasisGenerators(
  Vectors<Rational>& generators,
  const HashedList<Vector<Rational> >& inputRootSystem
) {
  STACK_TRACE("WeylGroup::transformToSimpleBasisGenerators");
  for (int i = 0; i < generators.size; i ++) {
    if (!generators[i].isPositiveOrZero()) {
      generators[i].negate();
    }
  }
  bool reductionOccured = true;
  Vector<Rational> root;
  while (reductionOccured) {
    reductionOccured = false;
    for (int i = 0; i < generators.size; i ++) {
      for (int j = i + 1; j < generators.size; j ++) {
        root = generators[i];
        root -= generators[j];
        if (root.isEqualToZero()) {
          generators.removeIndexSwapWithLast(j);
          reductionOccured = true;
        }
        if (inputRootSystem.contains(root)) {
          if (!root.isPositiveOrZero()) {
            root.negate();
            generators[j] = root;
          } else {
            generators[i] = root;
          }
          reductionOccured = true;
        }
      }
    }
  }
}

template <class Coefficient>
void Vector<Coefficient>::perturbNoZeroScalarProductWithMe(
  const List<Vector<Coefficient> >& inputVectors
) {
  STACK_TRACE("Vector::perturbNoZeroScalarProductWithMe");
  Coefficient scalarProductInverted;
  for (int i = 0; i < inputVectors.size; i ++) {
    if (this->scalarEuclidean(inputVectors[i]) == 0) {
      Coefficient scale = 1;
      for (int j = 0; j < i; j ++) {
        if (inputVectors[i].scalarEuclidean(inputVectors[j]) != 0) {
          scalarProductInverted = (
            this->scalarEuclidean(inputVectors[j]) /
            inputVectors[i].scalarEuclidean(inputVectors[j])
          ) /
          2;
          if (scalarProductInverted < 0) {
            scalarProductInverted *= - 1;
          }
          if (scale == 0) {
            scale = scalarProductInverted;
          } else if (scalarProductInverted != 0) {
            scale = MathRoutines::minimum(scale, scalarProductInverted);
          }
        }
      }
      *this += inputVectors[i] * scale;
    }
  }
  for (int i = 0; i < inputVectors.size; i ++) {
    if (this->scalarEuclidean(inputVectors[i]) == 0) {
      global.fatal
      << "The vector produced by perturbNoZeroScalarProductWithMe, namely, "
      << this->toString()
      << " is orthogonal to input vector "
      << inputVectors[i].toString()
      << ". The full list of vectors is "
      << inputVectors.toString()
      << global.fatal;
    }
  }
}

void WeylGroupData::transformToSimpleBasisGeneratorsArbitraryCoordinates(
  Vectors<Rational>& generators,
  const HashedList<Vector<Rational> >& inputRootSystem
) {
  if (generators.size == 0) {
    return;
  }
  STACK_TRACE(
    "WeylGroup::"
    "transformToSimpleBasisGeneratorsArbitraryCoordinates"
  );
  Vector<Rational> hElement;
  hElement.makeZero(generators[0].size);
  hElement.perturbNoZeroScalarProductWithMe(inputRootSystem);
  for (int i = 0; i < generators.size; i ++) {
    if (generators[i].scalarEuclidean(hElement) < 0) {
      generators[i].negate();
    }
  }
  bool reductionOccured = true;
  Vector<Rational> root;
  while (reductionOccured) {
    reductionOccured = false;
    for (int i = 0; i < generators.size; i ++) {
      for (int j = i + 1; j < generators.size; j ++) {
        root = generators[i];
        root -= generators[j];
        if (root.isEqualToZero()) {
          generators.removeIndexSwapWithLast(j);
          reductionOccured = true;
        }
        if (inputRootSystem.contains(root)) {
          if (root.scalarEuclidean(hElement) < 0) {
            root.negate();
            generators[j] = root;
          } else {
            generators[i] = root;
          }
          reductionOccured = true;
        }
      }
    }
  }
}

void WeylGroupData::transformToSimpleBasisGeneratorsWithRespectToH(
  Vectors<Rational>& generators, const Vector<Rational>& hVector
) {
  for (int i = 0; i < generators.size; i ++) {
    if (!this->isPositiveOrPerpWithRespectToH(generators[i], hVector)) {
      generators[i].negate();
    }
  }
  bool reductionOccured = true;
  Vector<Rational> root;
  while (reductionOccured) {
    reductionOccured = false;
    for (int i = 0; i < generators.size; i ++) {
      for (int j = i + 1; j < generators.size; j ++) {
        root = generators[i];
        root -= generators[j];
        if (root.isEqualToZero()) {
          generators.removeIndexSwapWithLast(j);
          reductionOccured = true;
        }
        if (this->rootSystem.getIndex(root) != - 1) {
          if (!this->isPositiveOrPerpWithRespectToH(root, hVector)) {
            root.negate();
            generators[j] = root;
          } else {
            generators[i] = root;
          }
          reductionOccured = true;
        }
      }
    }
  }
}

void WeylGroupData::computeExtremeRootInTheSameKMod(
  const Vectors<Rational>& inputSimpleBasisK,
  const Vector<Rational>& inputRoot,
  Vector<Rational>& output,
  bool lookingForHighest
) {
  STACK_TRACE("WeylGroup::computeExtremeRootInTheSameKMod");
  output = inputRoot;
  Vector<Rational> root;
  bool FoundHigher = true;
  do {
    FoundHigher = false;
    for (int i = 0; i < inputSimpleBasisK.size; i ++) {
      root = output;
      if (lookingForHighest) {
        root += inputSimpleBasisK[i];
      } else {
        root -= inputSimpleBasisK[i];
      }
      if (this->rootSystem.getIndex(root) != - 1) {
        output = root;
        FoundHigher = true;
      }
      if (root.isEqualToZero()) {
        output *= - 1;
        FoundHigher = true;
      }
    }
  } while (FoundHigher);
}

// Returning false means that the lattice given as rougher is not actually
// rougher than the current lattice
// or that there are too many representatives
bool Lattice::getAllRepresentatives(
  const Lattice& rougherLattice, Vectors<Rational>& output
) const {
  output.size = 0;
  if (this->basis.numberOfRows != rougherLattice.basis.numberOfRows) {
    return false;
  }
  List<int> periods;
  Vectors<Rational> periodVectors;
  periods.setSize(this->basis.numberOfRows);
  periodVectors.setSize(this->basis.numberOfRows);
  Vector<Rational> root;
  Vector<Rational> root2;
  int column = 0;
  int dimension = this->getDimension();
  Rational currentPeriod;
  LargeInteger currentPeriodInt;
  for (int i = 0; i < this->basis.numberOfRows; i ++) {
    while (
      this->basisRationalForm.elements[i][column].isEqualToZero()
    ) {
      column ++;
    }
    currentPeriod = rougherLattice.basisRationalForm.elements[i][column] /
    this->basisRationalForm.elements[i][column];
    currentPeriodInt = currentPeriod.getNumerator();
    if (currentPeriodInt.value.digits.size > 1) {
      return false;
    } else {
      periods[i] = currentPeriodInt.value.digits[0];
    }
    this->basisRationalForm.getVectorFromRow(i, periodVectors[i]);
    rougherLattice.basisRationalForm.getVectorFromRow(i, root2);
    root = periodVectors[i];
    root *= periods[i];
  }
  for (int i = 0; i < periods.size; i ++) {
    periods[i] --;
  }
  SelectionWithDifferentMaxMultiplicities coefficientSelection;
  coefficientSelection.initializeFromIntegers(periods);
  int NumCycles = coefficientSelection.totalNumberSubsetsSmallInt();
  output.setSize(NumCycles);
  for (
    int i = 0; i < NumCycles;
    i ++,
    coefficientSelection.incrementReturnFalseIfPastLast()
  ) {
    output[i].makeZero(dimension);
    for (int j = 0; j < coefficientSelection.multiplicities.size; j ++) {
      output[i] += periodVectors[j] * coefficientSelection.multiplicities[j];
    }
  }
  return true;
}

bool Lattice::
getClosestPointInDirectionOfTheNormalToAffineWallMovingIntegralStepsInDirection
(
  Vector<Rational>& startingPoint,
  Vector<Rational>& affineHyperplane,
  Vector<Rational>& direction,
  Vector<Rational>& outputPoint
) {
  Vector<Rational> normal = affineHyperplane;
  normal.setSize(affineHyperplane.size - 1);
  Rational shift = -(*affineHyperplane.lastObject());
  if (normal.scalarEuclidean(startingPoint) == shift) {
    outputPoint = startingPoint;
    return true;
  }
  if (direction.scalarEuclidean(normal).isEqualToZero()) {
    return false;
  }
  Rational movement = (shift - startingPoint.scalarEuclidean(normal)) /
  direction.scalarEuclidean(normal);
  global.comments
  << "<br>the movement: "
  << movement.toString()
  << ", ("
  << shift.toString()
  << " - "
  << startingPoint.scalarEuclidean(normal).toString()
  << ")/ "
  << direction.scalarEuclidean(normal).toString()
  << ", ";
  if (!movement.isInteger()) {
    global.comments << "the movement is not integral; ";
    movement.assignFloor();
    if (direction.scalarEuclidean(normal).isPositive()) {
      movement += 1;
    }
  }
  global.comments
  << "the normal: "
  << normal.toString()
  << ", the direction: "
  << direction.toString()
  << ", the shift: "
  << shift.toString()
  << ", the movement: "
  << movement.toString()
  << ", startingPoint: "
  << startingPoint.toString();
  outputPoint = startingPoint;
  outputPoint += direction * movement;
  global.comments << ", finalPoint: " << outputPoint.toString();
  return true;
}

bool Lattice::getAllRepresentativesProjectingDownTo(
  const Lattice& rougherLattice,
  Vectors<Rational>& startingShifts,
  Vectors<Rational>& output
) const {
  Vectors<Rational> tempRepresentatives;
  if (!this->getAllRepresentatives(rougherLattice, tempRepresentatives)) {
    return false;
  }
  output.setExpectedSize(startingShifts.size * tempRepresentatives.size);
  output.size = 0;
  for (int i = 0; i < startingShifts.size; i ++) {
    for (int j = 0; j < tempRepresentatives.size; j ++) {
      output.addOnTop(startingShifts[i] + tempRepresentatives[j]);
    }
  }
  return true;
}

void QuasiPolynomial::makeRougherLattice(const Lattice& latticeToRoughenBy) {
  if (this->ambientLatticeReduced == latticeToRoughenBy) {
    return;
  }
  Lattice OldLattice;
  OldLattice = this->ambientLatticeReduced;
  this->ambientLatticeReduced.intersectWith(latticeToRoughenBy);
  Vectors<Rational> representativesQuotientLattice;
  OldLattice.getAllRepresentatives(
    this->ambientLatticeReduced, representativesQuotientLattice
  );
  Vectors<Rational> OldLatticeShifts = this->latticeShifts;
  List<Polynomial<Rational> > oldValues;
  oldValues = this->valueOnEachLatticeShift;
  this->latticeShifts.setSize(
    OldLatticeShifts.size * representativesQuotientLattice.size
  );
  this->valueOnEachLatticeShift.setSize(this->latticeShifts.size);
  for (int i = 0; i < OldLatticeShifts.size; i ++) {
    for (int j = 0; j < representativesQuotientLattice.size; j ++) {
      this->latticeShifts[i * representativesQuotientLattice.size + j] =
      OldLatticeShifts[i] + representativesQuotientLattice[j];
      this->valueOnEachLatticeShift[
        i * representativesQuotientLattice.size + j
      ] =
      oldValues[i];
    }
  }
}

void Lattice::getDualLattice(Lattice& output) const {
  if (this->getRank() != this->getDimension()) {
    return;
  }
  Matrix<Rational> dualMatrix;
  dualMatrix = this->basisRationalForm;
  dualMatrix.invert();
  dualMatrix.transpose();
  dualMatrix.getMatrixIntegerWithDenominator(
    output.basis, output.denominator
  );
  output.reduce();
}

bool Lattice::findOnePreimageInLatticeOf(
  const Matrix<Rational>& linearMap,
  const Vectors<Rational>& input,
  Vectors<Rational>& output
) {
  Vectors<Rational> thisBasis;
  thisBasis.assignMatrixRows(this->basisRationalForm);
  linearMap.actOnVectorsColumn(thisBasis, Rational(0));
  bool result =
  input.getIntegralCoordsInBasisIfTheyExist(thisBasis, output, 1, - 1, 0);
  Matrix<Rational> matrix;
  matrix = this->basisRationalForm;
  matrix.transpose();
  matrix.actOnVectorsColumn(output);
  return result;
}

void Lattice::intersectWithPreimageOfLattice(
  const Matrix<Rational>& linearMap, const Lattice& other
) {
  Vectors<Rational>
  startingBasis,
  imageStartingBasis,
  basisImageIntersection,
  imageBasisInImageStartingBasisCoords;
  Vectors<Rational> resultNonKernelPart, result, roots;
  startingBasis.assignMatrixRows(this->basisRationalForm);
  linearMap.actOnVectorsColumn(startingBasis, imageStartingBasis);
  Lattice imageLattice;
  imageLattice.makeFromRoots(imageStartingBasis);
  imageLattice.intersectWith(other);
  basisImageIntersection.assignMatrixRows(imageLattice.basisRationalForm);
  Vectors<Rational>
  tempBasisImageIntersection,
  tempImageStartingBasis,
  tempImageBasisInImageStartingBasisCoords;
  basisImageIntersection = tempBasisImageIntersection;
  imageStartingBasis = tempImageStartingBasis;
  bool tempBool =
  tempBasisImageIntersection.getIntegralCoordsInBasisIfTheyExist(
    tempImageStartingBasis,
    tempImageBasisInImageStartingBasisCoords,
    Rational(1),
    Rational(- 1),
    Rational(0)
  );
  imageBasisInImageStartingBasisCoords = (
    tempImageBasisInImageStartingBasisCoords
  );
  if (!tempBool) {
    global.fatal
    << "Coordinates not integral when they should be. "
    << global.fatal;
  }
  resultNonKernelPart.setSize(imageBasisInImageStartingBasisCoords.size);
  for (int i = 0; i < resultNonKernelPart.size; i ++) {
    Vector<Rational>& currentRoot = resultNonKernelPart[i];
    currentRoot.makeZero(this->getDimension());
    for (int j = 0; j < startingBasis.size; j ++) {
      currentRoot +=
      startingBasis[j] * imageBasisInImageStartingBasisCoords[i][j];
    }
  }
  Lattice kernelPart;
  kernelPart = *this;
  roots.assignMatrixRows(linearMap);
  kernelPart.intersectWithLinearSubspaceGivenByNormals(roots);
  result.assignMatrixRows(kernelPart.basisRationalForm);
  result.addListOnTop(resultNonKernelPart);
  this->makeFromRoots(result);
}

void Lattice::intersectWith(const Lattice& other) {
  Vectors<Rational> commonBasis, otherBasis, startBasis;
  startBasis.assignMatrixRows(this->basisRationalForm);
  otherBasis.assignMatrixRows(other.basisRationalForm);
  startBasis.intersectTwoLinearSpaces(startBasis, otherBasis, commonBasis);
  Lattice thisLatticeIntersected, otherLatticeIntersected;
  thisLatticeIntersected = *this;
  otherLatticeIntersected = other;
  thisLatticeIntersected.intersectWithLinearSubspaceSpannedBy(commonBasis);
  otherLatticeIntersected.intersectWithLinearSubspaceSpannedBy(commonBasis);
  Vectors<Rational>
  thisCommonBasis,
  otherCommonBasis,
  thisCommonCoords,
  otherCommonCoords;
  thisCommonBasis.assignMatrixRows(thisLatticeIntersected.basisRationalForm);
  otherCommonBasis.assignMatrixRows(
    otherLatticeIntersected.basisRationalForm
  );
  thisCommonBasis.getCoordinatesInBasis(commonBasis, thisCommonCoords);
  otherCommonBasis.getCoordinatesInBasis(commonBasis, otherCommonCoords);
  Lattice thisCommonCoordsLattice, otherCommonCoordsLattice;
  thisCommonCoordsLattice.makeFromRoots(thisCommonCoords);
  otherCommonCoordsLattice.makeFromRoots(otherCommonCoords);
  thisCommonCoordsLattice.intersectWithBothOfMaximalRank(
    otherCommonCoordsLattice
  );
  Vectors<Rational> resultBasis;
  resultBasis.setSize(
    thisCommonCoordsLattice.basisRationalForm.numberOfRows
  );
  for (int i = 0; i < resultBasis.size; i ++) {
    Vector<Rational>& currentRoot = resultBasis[i];
    currentRoot.makeZero(this->getDimension());
    for (
      int j = 0; j < thisCommonCoordsLattice.basisRationalForm.numberOfColumns;
      j ++
    ) {
      currentRoot +=
      commonBasis[j] *
      thisCommonCoordsLattice.basisRationalForm.elements[i][j];
    }
  }
  this->makeFromRoots(resultBasis);
}

void Lattice::intersectWithBothOfMaximalRank(const Lattice& other) {
  Lattice dualLatticeThis, dualLatticeOther;
  if (
    this->basis.numberOfRows != this->getDimension() ||
    this->getDimension() != other.getDimension() ||
    other.basis.numberOfRows != this->getDimension()
  ) {
    global.fatal << "Bad dimensions. " << global.fatal;
  }
  this->getDualLattice(dualLatticeThis);
  other.getDualLattice(dualLatticeOther);
  dualLatticeThis.refineByOtherLattice(dualLatticeOther);
  dualLatticeThis.getDualLattice(*this);
}

void QuasiPolynomial::operator+=(const QuasiPolynomial& other) {
  this->makeRougherLattice(other.ambientLatticeReduced);
  QuasiPolynomial tempQP = other;
  tempQP.makeRougherLattice(this->ambientLatticeReduced);
  for (int i = 0; i < tempQP.latticeShifts.size; i ++) {
    this->addLatticeShift(
      tempQP.valueOnEachLatticeShift[i], tempQP.latticeShifts[i]
    );
  }
}

std::string QuasiPolynomial::toString(FormatExpressions* format) {
  std::stringstream out;
  if (this->latticeShifts.size == 0) {
    return "0";
  }
  out << "\\begin{array}{l}\n";
  for (int i = 0; i < this->latticeShifts.size; i ++) {
    out << this->valueOnEachLatticeShift[i].toString(format);
    out << "\\\\\n \\text{~over~}";
    if (!this->latticeShifts[i].isEqualToZero()) {
      out << this->latticeShifts[i].toString() << " + ";
    }
    out << "\\Lambda, \\\\\\hline\n ";
    if (i != this->latticeShifts.size - 1) {
      out << "\\\\";
    }
  }
  if (!this->ambientLatticeReduced.basisRationalForm.isIdentity()) {
    out << ", \\mathrm{~where~} \\Lambda = \\langle ";
    Vectors<Rational> roots;
    roots.assignMatrixRows(this->ambientLatticeReduced.basisRationalForm);
    for (int i = 0; i < roots.size; i ++) {
      out << roots[i].toString();
      if (i != roots.size - 1) {
        out << ", ";
      }
    }
    out << "\\rangle";
  } else {
    out
    << "\\Lambda =\\mathbb{Z}^{"
    << this->minimalNumberOfVariables()
    << "}";
  }
  out << "\\end{array}";
  return out.str();
}

std::string QuasiPolynomial::toHTML(FormatExpressions* format) {
  std::stringstream out;
  if (this->latticeShifts.size == 0) {
    return "0";
  }
  for (int i = 0; i < this->latticeShifts.size; i ++) {
    out << "\\(" << this->valueOnEachLatticeShift[i].toString(format) << "\\)";
    out << " over ";
    if (!this->latticeShifts[i].isEqualToZero()) {
      out << this->latticeShifts[i].toString() << " + ";
    }
    out << "&Lambda;";
    if (i != this->latticeShifts.size - 1) {
      out << "<hr>";
    }
  }
  if (!this->ambientLatticeReduced.basisRationalForm.isIdentity()) {
    out << "where " << "\\(" << "\\Lambda = \\langle";
    Vectors<Rational> roots;
    roots.assignMatrixRows(this->ambientLatticeReduced.basisRationalForm);
    for (int i = 0; i < roots.size; i ++) {
      out << roots[i].toString();
      if (i != roots.size - 1) {
        out << ", ";
      }
    }
    out << "\\rangle";
  } else {
    out
    << "\\Lambda =\\mathbb{Z}^{"
    << this->minimalNumberOfVariables()
    << "}";
  }
  out << "\\)";
  return out.str();
}

void QuasiPolynomial::makeFromPolynomialShiftAndLattice(
  const Polynomial<Rational>& inputPoly,
  const MonomialPolynomial& shift,
  const Lattice& lattice
) {
  STACK_TRACE("QuasiPolynomial::makeFromPolynomialShiftAndLattice");
  this->ambientLatticeReduced = lattice;
  this->latticeShifts.setSize(1);
  Vector<Rational>& firstShift = this->latticeShifts[0];
  firstShift.setSize(lattice.getDimension());
  for (int i = 0; i < lattice.getDimension(); i ++) {
    firstShift[i] = shift(i);
  }
  this->ambientLatticeReduced.reduceVector(this->latticeShifts[0]);
  this->valueOnEachLatticeShift.setSize(1);
  this->valueOnEachLatticeShift[0] = inputPoly;
}

bool Lattice::reduceVector(Vector<Rational>& vector) const {
  STACK_TRACE("Lattice::reduceVector");
  Vector<Rational> output;
  if (vector.size != this->getDimension()) {
    global.fatal << "Vector dimension not as expected. " << global.fatal;
  }
  Vectors<Rational> basisRoots;
  basisRoots.assignMatrixRows(this->basisRationalForm);
  if (!vector.getCoordinatesInBasis(basisRoots, output)) {
    global.fatal
    << "Failed to get coordinates of: "
    << vector.toString()
    << " in basis: "
    << basisRoots.toString()
    << global.fatal;
    return false;
  }
  for (int i = 0; i < output.size; i ++) {
    output[i].assignFractionalValue();
  }
  vector.makeZero(vector.size);
  for (int i = 0; i < basisRoots.size; i ++) {
    vector += basisRoots[i] * output[i];
  }
  return true;
}

void Lattice::makeZn(int dimension) {
  this->basisRationalForm.makeIdentityMatrix(
    dimension, Rational::one(), Rational::zero()
  );
  this->basis.makeIdentityMatrix(
    dimension, LargeInteger(1), LargeInteger(0)
  );
  this->denominator.makeOne();
}

void QuasiPolynomial::makeZeroOverLattice(Lattice& lattice) {
  this->ambientLatticeReduced = lattice;
  this->latticeShifts.size = 0;
  this->valueOnEachLatticeShift.size = 0;
}

void QuasiPolynomial::makeZeroLatticeZn(int dimension) {
  if (dimension <= 0) {
    global.fatal << "Negative dimension not allowed. " << global.fatal;
  }
  this->ambientLatticeReduced.makeZn(dimension);
  this->latticeShifts.size = 0;
  this->valueOnEachLatticeShift.size = 0;
}

void OnePartialFractionDenominator::getDenominatorsSorted(
  MapList<Vector<Rational>, OnePartialFractionDenominatorComponent>& output
) const {
  Vectors<Rational> normalizedExponents;
  this->getNormalizedSortedDenominatorExponents(normalizedExponents);
  output.clear();
  for (int i = 0; i < normalizedExponents.size; i ++) {
    output.setKeyValue(
      normalizedExponents[i],
      this->denominatorsNoScale.getValueNoFail(normalizedExponents[i])
    );
  }
}

void OnePartialFractionDenominator::getNormalizedSortedDenominatorExponents(
  Vectors<Rational>& output
) const {
  output.clear();
  for (int i = 0; i < this->owner->normalizedVectors.size; i ++) {
    const Vector<Rational>& current = this->owner->normalizedVectors[i];
    if (!this->denominatorsNoScale.contains(current)) {
      continue;
    }
    output.addOnTop(current);
  }
}

void OnePartialFractionDenominator::getDenominatorExponents(
  Vectors<Rational>& output
) const {
  STACK_TRACE("OnePartialFractionDenominator::getDenominatorExponents");
  output.clear();
  for (int i = 0; i < this->owner->normalizedVectors.size; i ++) {
    const Vector<Rational>& current = this->owner->normalizedVectors[i];
    if (!this->denominatorsNoScale.contains(current)) {
      continue;
    }
    const OnePartialFractionDenominatorComponent& component =
    this->denominatorsNoScale.getValueNoFail(current);
    List<Vector<Rational> > currentExponents;
    component.getDenominatorExponents(currentExponents);
    output.addListOnTop(currentExponents);
  }
}

void OnePartialFractionDenominator::
getDenominatorExponentsWithoutMultiplicities(Vectors<Rational>& output) const {
  STACK_TRACE(
    "OnePartialFractionDenominator::"
    "getDenominatorExponentsWithoutMultiplicities"
  );
  output.clear();
  for (int i = 0; i < this->owner->normalizedVectors.size; i ++) {
    const Vector<Rational>& current = this->owner->normalizedVectors[i];
    if (!this->denominatorsNoScale.contains(current)) {
      continue;
    }
    const OnePartialFractionDenominatorComponent& component =
    this->denominatorsNoScale.getValueNoFail(current);
    List<Vector<Rational> > currentExponents;
    component.getDenominatorExponentsWithoutMultiplicities(currentExponents);
    output.addListOnTop(currentExponents);
  }
}

void OnePartialFractionDenominator::computePolynomialCorrespondingToOneMonomial
(
  QuasiPolynomial& outputQuasipolynomial,
  const MonomialPolynomial& monomial,
  Vectors<Rational>& coneGenerators,
  Vectors<Rational>& normals,
  Lattice& lattice
) const {
  STACK_TRACE(
    "OnePartialFractionDenominator::"
    "computePolynomialCorrespondingToOneMonomial"
  );
  Polynomial<Rational> multiplicand;
  Polynomial<Rational> outputPolynomialPart;
  outputPolynomialPart.makeOne();
  if (
    this->denominatorsNoScale.size() != this->owner->ambientDimension
  ) {
    global.fatal
    << "Attempt to extract quasipolynomial "
    << "from non-reduced fraction (too many vectors). "
    << global.fatal;
  }
  // Example 1. 1/(1-x^2)^2
  // Content of normals: 1/2
  //
  // Example 2. 1/((1-x)^2(1-y))
  // Content of normals: (1,0) and (0,1).
  for (int i = 0; i < normals.size; i ++) {
    Vector<Rational> vector = coneGenerators[i];
    Rational::scaleNoSignChange(vector);
    const OnePartialFractionDenominatorComponent& current =
    this->denominatorsNoScale.getValueNoFail(vector);
    if (current.multiplicities.size != 1) {
      global.fatal
      << "Attempt to extract quasipolynomial "
      << "from non-reduced fraction (too many multiplicities). "
      << global.fatal;
    }
    this->makePolynomialFromOneNormal(
      normals[i], monomial, current.multiplicities[0], multiplicand
    );
    outputPolynomialPart *= multiplicand;
  }
  outputQuasipolynomial.makeFromPolynomialShiftAndLattice(
    outputPolynomialPart, monomial, lattice
  );
}

void OnePartialFractionDenominator::getVectorPartitionFunction(
  PartialFractions& owner,
  Polynomial<LargeInteger>& coefficient,
  QuasiPolynomial& output
) const {
  STACK_TRACE("OnePartialFractionDenominator::getVectorPartitionFunction");
  QuasiPolynomial shiftedQuasiPolynomial;
  Vectors<Rational> normals;
  Vectors<Rational> latticeGenerators;
  this->getDenominatorExponentsWithoutMultiplicities(latticeGenerators);
  if (latticeGenerators.size != this->owner->ambientDimension) {
    global.fatal
    << "Unexpected: the number of denominators "
    << latticeGenerators.size
    << " without multiplicity should equal "
    << "the dimension: "
    << this->owner->ambientDimension
    << global.fatal;
  }
  // Example 1. We have the partial fraction
  // 1/(1-x^2)^2 = (1+x^2+x^4+...)(1+x+x^2+...)
  // Example 2. We have the partial fraction:
  // 1/((1-x)^2(1-y)
  Lattice lattice;
  lattice.makeFromRoots(latticeGenerators);
  Matrix<Rational> normalsMatrixForm;
  normalsMatrixForm.assignVectorsToRows(latticeGenerators);
  // Example 1. The matrix before inversion: (2).
  // Example 2. The matrix before inversion: the 2x2 identity.
  normalsMatrixForm.invert();
  // Example 1. The matrix after inversion: (1/2).
  // Example 2. The matrix after inversion: the 2x2 identity.
  normals.assignMatrixColumns(normalsMatrixForm);
  output.makeZeroLatticeZn(owner.ambientDimension);
  for (int i = 0; i < coefficient.size(); i ++) {
    this->computePolynomialCorrespondingToOneMonomial(
      shiftedQuasiPolynomial,
      coefficient[i],
      latticeGenerators,
      normals,
      lattice
    );
    shiftedQuasiPolynomial *= coefficient.coefficients[i];
    output += shiftedQuasiPolynomial;
  }
}

void PartialFractions::computeAllVectorPartitionFunctions() {
  STACK_TRACE("PartialFractions::computeAllVectorPartitionFunctions");
  Vectors<Rational> vectors;
  vectors.addListOnTop(this->normalizedVectors);
  this->chambers.initializeFromDirectionsAndRefine(vectors);
  this->computeQuasipolynomials();
}

void PartialFractions::computeQuasipolynomials() {
  STACK_TRACE("PartialFractions::computeQuasipolynomials");
  this->allQuasiPolynomials.clear();
  QuasiPolynomial current;
  for (Cone& cone : this->chambers.refinedCones) {
    Vector<Rational> internalPoint = cone.getInternalPoint();
    this->computeOneVectorPartitionFunction(current, internalPoint);
    this->allQuasiPolynomials.addOnTop(current);
  }
}

void PartialFractions::evaluateVectorPartitionFunction(
  const Vector<Rational>& input,
  Rational& output,
  std::stringstream* comments
) {
  STACK_TRACE("PartialFractions::evaluateVectorPartitionFunction");
  if (this->chambers.refinedCones.size == 0) {
    global.fatal
    << "Please compute the partial fraction decomposition first."
    << global.fatal;
  }
  int coneIndex = 0;
  for (; coneIndex < this->chambers.refinedCones.size; coneIndex ++) {
    const Cone& cone = this->chambers.refinedCones[coneIndex];
    if (cone.isInCone(input)) {
      break;
    }
  }
  if (coneIndex >= this->chambers.refinedCones.size) {
    // No cone contains the input.
    output = 0;
    return;
  }
  if (comments != nullptr) {
    *comments
    << "Vector: "
    << input.toString()
    << " is in chamber "
    << coneIndex + 1
    << this->chambers.refinedCones[coneIndex].toHTML()
    << "<br>";
  }
  QuasiPolynomial& quasiPolynomial = this->allQuasiPolynomials[coneIndex];
  output = quasiPolynomial.evaluate(input, comments);
}

bool PartialFractions::computeOneVectorPartitionFunction(
  QuasiPolynomial& output, Vector<Rational>& newIndicator
) {
  STACK_TRACE("PartialFractions::computeOneVectorPartitionFunction");
  ProgressReport report;
  if (this->assureIndicatorRegularity(newIndicator)) {
    report.report("Indicator modified to regular");
  }
  if (
    !this->checkForMinimalityDecompositionWithRespectToRoot(&newIndicator)
  ) {
    return false;
  }
  this->totalFractionsWithAccountedVectorPartitionFunction = 0;
  output.makeZeroLatticeZn(this->ambientDimension);
  QuasiPolynomial summand;
  for (int i = 0; i < this->reduced.size(); i ++) {
    if (!this->reduced[i].rootIsInFractionCone(&newIndicator)) {
      continue;
    }
    const OnePartialFractionDenominator& currentFraction = this->reduced[i];
    currentFraction.getVectorPartitionFunction(
      *this, this->reduced.coefficients[i], summand
    );
    output += summand;
    this->makeProgressVPFcomputation();
  }
  return true;
}

std::string PartialFractions::doFullComputationReturnLatexFileString(
  Vectors<Rational>& toBePartitioned,
  FormatExpressions& format,
  std::string* outputHtml
) {
  std::string whatWentWrong;
  global.fatal << "Not implemented yet. " << global.fatal;
  //  this->chambersOld.directions = toBePartitioned;
  this->ambientDimension = toBePartitioned.getDimension();
  //  this->chambersOld.AmbientDimension = toBePartitioned.getDimension();
  // this->chambers.ReadFromDefaultFile();
  std::stringstream out;
  std::stringstream outHtml;
  global.fatal << global.fatal;
  //  this->chambersOld.SliceEuclideanSpace(false);
  //  this->chambersOld.quickSortAscending();
  //  this->chambersOld.LabelChamberIndicesProperly();
  //  this->chambers.AssignCombinatorialChamberComplex(this->chambersOld);
  //  this->chambersOld.drawOutput(global.drawingVariables, root, 0);
  DrawingVariables drawingVariables;
  this->chambers.drawMeProjective(nullptr, true, drawingVariables, format);
  outHtml << drawingVariables.getHTMLDiv(this->ambientDimension, false);
  Vector<Rational> root;
  root.makeZero(this->ambientDimension);
  global.fatal << "not implemented yet" << global.fatal;
  //  this->initFromRoots(chambersOld.directions);
  out
  << "\\documentclass{article}"
  << "\\usepackage{amsmath, amsfonts, amssymb}\n"
  << "\\begin{document}\n";
  out
  << "The vector partition funciton is the number of "
  << "ways you can represent a vector "
  << "$(x_1,\\dots, x_n)$ as a non-negative "
  << "integral linear combination of "
  << " a given set of vectors. "
  << "You requested the vector partition function "
  << "with respect to the set of vectors: ";
  global.fatal << global.fatal;
  //  out << this->chambersOld.directions.ElementToStringGeneric();
  out
  << "\n\n The corresponding generating function is: "
  << this->toString(&format)
  << "= (after splitting acording to algorithm)";
  this->split(nullptr);
  out << this->toString(&format);
  global.fatal << global.fatal;
  // out << "Therefore the vector partition function is given by " <<
  // this->chambersOld.GetNumNonZeroPointers()
  // << " quasipolynomials depending on which set of linear inequalities is
  // satisfied (each such set we call ``Chamber'').";
  // outHtml << "There are " << this->chambersOld.size << " systems of linear
  // inequalities "
  // << " such that on each such system of inequalities the vector partition
  // function is quasi-polynomial. "
  // << " A full list of the systems of inequalities (\"chambers\") and the
  // corresponding quasi-polynomials follows.<hr> ";
  QuasiPolynomial tempQP;
  std::string tempS;
  Vector<Rational> tempIndicator;
  global.fatal << global.fatal;
  /*for (int i = 0; i < this->chambersOld.size; i ++)
    if (this->chambersOld[i] != 0) {
      Cone& currentChamber = this->chambers[i];
      tempIndicator = currentChamber.getInternalPoint();
      this->getVectorPartitionFunction(tempQP, tempIndicator);
      out << "\n\n" << currentChamber.toString(true, false, true, false, format);
      out << "\n\nQuasipolynomial: " << tempQP.toString(false, true, format);
      outHtml << "<hr>Chamber: " << currentChamber.toString(false, true, true, false, format);
      bool useHtml = false;
      if (tempQP.valueOnEachLatticeShift.size >0)
        if (tempQP.valueOnEachLatticeShift[0].size<30)
          useHtml = true;
      outHtml << "<br>Quasi-polynomial: " <<
      HtmlRoutines::GetHtmlMathDivFromLatexFormulA(tempQP.toString(useHtml, true, format));
    }
    */
  out << "\\end{document}";
  if (outputHtml != nullptr) {
    *outputHtml = outHtml.str();
  }
  return out.str();
}

void QuasiPolynomial::addLatticeShift(
  const Polynomial<Rational>& input,
  const Vector<Rational>& inputShift
) {
  Vector<Rational> shift = inputShift;
  this->ambientLatticeReduced.reduceVector(shift);
  int index = this->latticeShifts.getIndex(shift);
  if (index == - 1) {
    index = this->latticeShifts.size;
    this->latticeShifts.addOnTop(shift);
    this->valueOnEachLatticeShift.expandOnTop(1);
    this->valueOnEachLatticeShift.lastObject()->makeZero();
  }
  this->valueOnEachLatticeShift[index] += input;
  if (this->valueOnEachLatticeShift[index].isEqualToZero()) {
    this->latticeShifts.removeIndexSwapWithLast(index);
    this->valueOnEachLatticeShift.removeIndexSwapWithLast(index);
  }
}

void QuasiPolynomial::substitution(
  const Matrix<Rational>& mapFromNewSpaceToOldSpace,
  const Lattice& ambientLatticeNewSpace,
  QuasiPolynomial& output
) {
  STACK_TRACE("QuasiPolynomial::substitution");
  // Format of the substitution.
  // If we want to carry out a substitution in P(y_1, ..., y_m),
  // of the form
  // y_1 = a_11 x_1+...+a_1n x_n
  // ...
  // y_m = a_m1 x_1+...+a_mn x_n
  // then the mapFromNewSpaceToOldSpace should be the matrix
  // a_11 ... a_1n
  // ...
  // a_m1 ... a_mn
  if (
    this == &output ||
    mapFromNewSpaceToOldSpace.numberOfRows != this->minimalNumberOfVariables()
    ||
    ambientLatticeNewSpace.getDimension() !=
    mapFromNewSpaceToOldSpace.numberOfColumns
  ) {
    global.fatal << "Bad lattice dimensions. " << global.fatal;
  }
  output.ambientLatticeReduced = ambientLatticeNewSpace;
  output.ambientLatticeReduced.intersectWithPreimageOfLattice(
    mapFromNewSpaceToOldSpace, this->ambientLatticeReduced
  );
  Vectors<Rational> allRepresentatives, imagesAllRepresentatives;
  bool tempBool =
  ambientLatticeNewSpace.getAllRepresentatives(
    output.ambientLatticeReduced, allRepresentatives
  );
  if (!tempBool) {
    global.fatal << "Failed to get all representatives. " << global.fatal;
  }
  mapFromNewSpaceToOldSpace.actOnVectorsColumn(
    allRepresentatives, imagesAllRepresentatives
  );
  PolynomialSubstitution<Rational> substitution;
  substitution.setSize(this->minimalNumberOfVariables());
  Vector<Rational> root;
  for (int i = 0; i < substitution.size; i ++) {
    Polynomial<Rational>& currentPoly = substitution[i];
    mapFromNewSpaceToOldSpace.getVectorFromRow(i, root);
    currentPoly.makeLinearNoConstant(root);
  }
  Polynomial<Rational> tempP;
  for (int i = 0; i < this->valueOnEachLatticeShift.size; i ++) {
    tempP = this->valueOnEachLatticeShift[i];
    bool tempB = tempP.substitution(substitution, Rational::one());
    if (!tempB) {
      global.fatal
      << "Substitution "
      << substitution.toString()
      << " into polynomial "
      << tempP.toString()
      << " failed but the current function does not handle this properly. "
      << global.fatal;
    }
    for (int j = 0; j < allRepresentatives.size; j ++) {
      if (imagesAllRepresentatives[j] == this->latticeShifts[i]) {
        output.addLatticeShift(tempP, allRepresentatives[j]);
      }
    }
  }
}

void QuasiPolynomial::substitution(
  const Vector<Rational>& inputTranslationSubtractedFromArgument,
  QuasiPolynomial& output
) {
  STACK_TRACE("QuasiPolynomial::substitution");
  // format of the translation. If the starting quasipolynomial was P(y_1, ...,
  // y_n),
  // and the translation has coordinates (t_1, ..., t_n),
  // then the resulting quasipolynomial will be P(x_1-t_1, ..., x_n-t_n)
  PolynomialSubstitution<Rational> substitution;
  substitution.makeIdentitySubstitution(
    this->minimalNumberOfVariables(), Rational(1)
  );
  for (int i = 0; i < substitution.size; i ++) {
    substitution[i].addConstant(- inputTranslationSubtractedFromArgument[i]);
  }
  Polynomial<Rational> tempP;
  output.makeZeroLatticeZn(this->minimalNumberOfVariables());
  output.ambientLatticeReduced = this->ambientLatticeReduced;
  for (int i = 0; i < this->valueOnEachLatticeShift.size; i ++) {
    tempP = this->valueOnEachLatticeShift[i];
    bool tempB = tempP.substitution(substitution, Rational::one());
    if (!tempB) {
      global.fatal
      << "Substitution "
      << substitution.toString()
      << " into polynomial "
      << tempP.toString()
      << " failed but the current function does not handle this properly. "
      << global.fatal;
    }
    output.addLatticeShift(
      tempP,
      this->latticeShifts[i] + inputTranslationSubtractedFromArgument
    );
  }
}

void QuasiPolynomial::substitution(
  const Matrix<Rational>& mapFromNewSpaceToOldSpace,
  const Vector<Rational>& inputTranslationSubtractedFromArgument,
  const Lattice& ambientLatticeNewSpace,
  QuasiPolynomial& output
) {
  QuasiPolynomial tempQP;
  this->substitution(inputTranslationSubtractedFromArgument, tempQP);
  tempQP.substitution(
    mapFromNewSpaceToOldSpace, ambientLatticeNewSpace, output
  );
}

bool QuasiPolynomial::substitutionFewerVariables(
  const PolynomialSubstitution<Rational>& substitution,
  QuasiPolynomial& output
) const {
  Matrix<Rational> latticeSubstitution;
  if (
    !this->ambientLatticeReduced.
    getHomogeneousSubstitutionMatrixFromSubstitutionIgnoreConstantTerms(
      substitution, latticeSubstitution
    )
  ) {
    return false;
  }
  Matrix<Rational> subLatticeShift;
  output.ambientLatticeReduced = this->ambientLatticeReduced;
  if (
    !output.ambientLatticeReduced.substitutionHomogeneous(latticeSubstitution)
  ) {
    return false;
  }
  subLatticeShift.initialize(latticeSubstitution.numberOfRows, 1);
  for (int i = 0; i < subLatticeShift.numberOfRows; i ++) {
    substitution[i].getConstantTerm(
      subLatticeShift.elements[i][0], Rational(0)
    );
  }
  Matrix<Rational> shiftImage, shiftMatForm;
  output.latticeShifts.size = 0;
  output.valueOnEachLatticeShift.size = 0;
  output.valueOnEachLatticeShift.reserve(this->latticeShifts.size);
  output.latticeShifts.reserve(this->latticeShifts.size);
  Vector<Rational> root;
  Polynomial<Rational> tempP;
  for (int i = 0; i < this->latticeShifts.size; i ++) {
    shiftMatForm.assignVectorColumn(this->latticeShifts[i]);
    shiftMatForm -= subLatticeShift;
    if (
      latticeSubstitution.
      solve_Ax_Equals_b_ModifyInputReturnFirstSolutionIfExists(
        latticeSubstitution, shiftMatForm, shiftImage
      )
    ) {
      root.assignMatrixDetectRowOrColumn(shiftImage);
      tempP = this->valueOnEachLatticeShift[i];
      bool tempB = tempP.substitution(substitution, Rational::one());
      if (!tempB) {
        global.fatal
        << "Substitution "
        << substitution.toString()
        << " into polynomial "
        << tempP.toString()
        << " failed but the current function does not handle this properly. "
        << global.fatal;
      }
      output.addLatticeShift(tempP, root);
    }
  }
  return true;
}

bool Lattice::substitutionHomogeneous(
  const PolynomialSubstitution<Rational>& substitution
) {
  Matrix<Rational> matrixSubstitution;
  if (
    !this->getHomogeneousSubstitutionMatrixFromSubstitutionIgnoreConstantTerms(
      substitution, matrixSubstitution
    )
  ) {
    return false;
  }
  return this->substitutionHomogeneous(matrixSubstitution);
}

bool Lattice::
getHomogeneousSubstitutionMatrixFromSubstitutionIgnoreConstantTerms(
  const PolynomialSubstitution<Rational>& substitution,
  Matrix<Rational>& output
) {
  if (substitution.size < 1) {
    return false;
  }
  int targetDimension = 0;
  for (int i = 0; i < substitution.size; i ++) {
    targetDimension =
    MathRoutines::maximum(
      targetDimension, substitution[i].minimalNumberOfVariables()
    );
  }
  output.initialize(substitution.size, targetDimension);
  for (int i = 0; i < substitution.size; i ++) {
    Polynomial<Rational>& currentPoly = substitution[i];
    if (!currentPoly.isLinear()) {
      return false;
    }
    for (int j = 0; j < targetDimension; j ++) {
      currentPoly.getCoefficientInFrontOfLinearTermVariableIndex(
        j, output.elements[i][j]
      );
    }
  }
  return true;
}

void Lattice::intersectWithLinearSubspaceGivenByNormal(
  const Vector<Rational>& normal
) {
  Vectors<Rational> startingBasis, resultBasis;
  startingBasis.assignMatrixRows(this->basisRationalForm);
  Vector<Rational> scalarProducts;
  scalarProducts.setSize(startingBasis.size);
  for (int i = 0; i < this->basisRationalForm.numberOfRows; i ++) {
    scalarProducts[i] = normal.scalarEuclidean(startingBasis[i]);
  }
  if (scalarProducts.isEqualToZero()) {
    return;
  }
  int pivotColumnIndex = scalarProducts.getIndexLastNonZeroCoordinate();
  Rational pivotCoeff = scalarProducts[pivotColumnIndex];
  scalarProducts /= - pivotCoeff;
  Vectors<Rational> eigenSpacePlusOrthogonalComponent;
  eigenSpacePlusOrthogonalComponent.setSize(scalarProducts.size);
  for (int i = 0; i < scalarProducts.size; i ++) {
    if (i != pivotColumnIndex) {
      Vector<Rational>& currentRoot = eigenSpacePlusOrthogonalComponent[i];
      currentRoot.makeZero(scalarProducts.size);
      currentRoot[i] = 1;
      currentRoot[pivotColumnIndex] = scalarProducts[i];
    }
  }
  scalarProducts.scaleNormalizeFirstNonZero();
  eigenSpacePlusOrthogonalComponent[pivotColumnIndex] = scalarProducts;
  Lattice eigenLattice, znLattice;
  eigenLattice.makeFromRoots(eigenSpacePlusOrthogonalComponent);
  znLattice.makeZn(scalarProducts.size);
  znLattice.intersectWithBothOfMaximalRank(eigenLattice);
  resultBasis.reserve(scalarProducts.size - 1);
  Vector<Rational> root;
  Vector<Rational> resultRoot;
  Rational orthogonalComponent;
  for (int i = 0; i < znLattice.basisRationalForm.numberOfRows; i ++) {
    znLattice.basisRationalForm.getVectorFromRow(i, root);
    orthogonalComponent = root.scalarEuclidean(scalarProducts) /
    scalarProducts.scalarEuclidean(scalarProducts);
    root -= scalarProducts * orthogonalComponent;
    if (!orthogonalComponent.isInteger()) {
      global.fatal
      << "Orthogonal component is supposed to be an integer. "
      << global.fatal;
    }
    if (!root.isEqualToZero()) {
      resultRoot.makeZero(this->getDimension());
      for (int j = 0; j < startingBasis.size; j ++) {
        resultRoot += startingBasis[j] * root[j];
      }
      resultBasis.addOnTop(resultRoot);
    }
  }
  this->makeFromRoots(resultBasis);
}

void Lattice::intersectWithLinearSubspaceSpannedBy(
  const Vectors<Rational>& subspaceBasis
) {
  Vectors<Rational> normals;
  Matrix<Rational> matrix;
  subspaceBasis.getMatrixRootsToRows(matrix);
  matrix.getZeroEigenSpace(normals);
  this->intersectWithLinearSubspaceGivenByNormals(normals);
}

void Lattice::intersectWithLinearSubspaceGivenByNormals(
  const Vectors<Rational>& subspaceNormals
) {
  for (int i = 0; i < subspaceNormals.size; i ++) {
    this->intersectWithLinearSubspaceGivenByNormal(subspaceNormals[i]);
  }
}

bool Lattice::substitutionHomogeneous(
  const Matrix<Rational>& substitution, Lattice& resultIsSubsetOf
) {
  (void) substitution;
  (void) resultIsSubsetOf;
  global.fatal << "Not implemented yet. " << global.fatal;
  /*Vectors<Rational> preimageBasis;
  preimageBasis.assignMatrixRows(this->basisRationalForm);
  Matrix<Rational>  subModifiable, currentBasisVector, oneSolution;
  for (int i = 0; i <preimageBasis.size; i ++) {
    subModifiable = sub;
    currentBasisVector.assignVectorColumn(preimageBasis[i]);
    if (subModifiable.solve_Ax_Equals_b_ModifyInputReturnFirstSolutionIfExists(
      subModifiable, currentBasisVector, oneSolution)) {
      subModifiable = sub;
      subModifiable.fin
    }
  }*/
  return false;
}

bool Lattice::substitutionHomogeneous(const Matrix<Rational>& substitution) {
  int targetDimension = substitution.numberOfColumns;
  if (substitution.numberOfRows != this->getDimension()) {
    return false;
  }
  int startingDim = this->getDimension();
  Matrix<Rational>
  matrix,
  oldBasisTransformed,
  matRelationBetweenStartingVariables;
  matrix = substitution;
  oldBasisTransformed = this->basisRationalForm;
  oldBasisTransformed.transpose();
  Selection nonPivotPoints;
  matrix.gaussianEliminationByRows(&oldBasisTransformed, &nonPivotPoints);
  if (nonPivotPoints.cardinalitySelection != 0) {
    return false;
  }
  int numNonZeroRows = nonPivotPoints.numberOfElements;
  int numZeroRows = matrix.numberOfRows - numNonZeroRows;
  matRelationBetweenStartingVariables.initialize(numZeroRows, startingDim);
  for (int i = 0; i < numZeroRows; i ++) {
    for (int j = 0; j < startingDim; j ++) {
      matRelationBetweenStartingVariables.elements[i][j] =
      oldBasisTransformed.elements[i + numNonZeroRows][j];
    }
  }
  Vectors<Rational> eigenSpace;
  matRelationBetweenStartingVariables.getZeroEigenSpaceModifyMe(eigenSpace);
  for (int i = 0; i < eigenSpace.size; i ++) {
    Cone::scaleNormalizeByPositive(eigenSpace[i]);
  }
  oldBasisTransformed.actOnVectorsColumn(eigenSpace);
  this->basisRationalForm.initialize(targetDimension, targetDimension);
  for (int i = 0; i < targetDimension; i ++) {
    for (int j = 0; j < targetDimension; j ++) {
      this->basisRationalForm.elements[i][j] = eigenSpace[i][j];
    }
  }
  this->basisRationalForm.getMatrixIntegerWithDenominator(
    this->basis, this->denominator
  );
  this->reduce();
  return true;
}

void QuasiPolynomial::operator*=(const Rational& scalar) {
  if (scalar.isEqualToZero()) {
    this->valueOnEachLatticeShift.size = 0;
    this->latticeShifts.size = 0;
    return;
  }
  for (int i = 0; i < this->valueOnEachLatticeShift.size; i ++) {
    this->valueOnEachLatticeShift[i] *= scalar;
  }
}

Cone::Cone(const Cone& other) {
  this->operator=(other);
}

Cone::Cone() {
  this->lowestIndexNotCheckedForSlicingInDirection = 0;
  this->lowestIndexNotCheckedForChopping = 0;
  this->flagIsTheZeroCone = true;
  // this->flagHasSufficientlyManyVertices = true;
}

bool Cone::operator>(const Cone& other) const {
  return this->normals > other.normals;
}

bool Cone::operator==(const Cone& other) const {
  return
  this->flagIsTheZeroCone == other.flagIsTheZeroCone &&
  this->normals == other.normals;
}

void Cone::operator=(const Cone& other) {
  this->flagIsTheZeroCone = other.flagIsTheZeroCone;
  this->vertices = other.vertices;
  this->normals = other.normals;
  this->lowestIndexNotCheckedForSlicingInDirection =
  other.lowestIndexNotCheckedForSlicingInDirection;
  this->lowestIndexNotCheckedForChopping =
  other.lowestIndexNotCheckedForChopping;
}

unsigned int Cone::hashFunction() const {
  return this->vertices.hashFunction();
}

unsigned int Cone::hashFunction(const Cone& input) {
  return input.hashFunction();
}

void Cone::intersectHyperplane(
  Vector<Rational>& normal, Cone& outputConeLowerDimension
) {
  if (normal.isEqualToZero()) {
    global.fatal << "zero normal not allowed. " << global.fatal;
  }
  int dimension = normal.size;
  Matrix<Rational> kernelComputer, embedding, projection;
  kernelComputer.assignVectorRow(normal);
  Vectors<Rational> basis;
  kernelComputer.getZeroEigenSpace(basis);
  if (basis.size != normal.size - 1) {
    global.fatal
    << "Plane intersection: normals don't match. "
    << global.fatal;
  }
  embedding.assignVectorsToRows(basis);
  embedding.transpose();
  basis.addOnTop(normal);
  Vectors<Rational> simpleRoots;
  Vectors<Rational> convertedCoordinates;
  simpleRoots.makeEiBasis(dimension);
  simpleRoots.getCoordinatesInBasis(basis, convertedCoordinates);
  projection.assignVectorsToRows(convertedCoordinates);
  projection.transpose();
  projection.resize(dimension - 1, dimension, false);
  Vectors<Rational> newNormals = this->normals;
  projection.actOnVectorsColumn(newNormals, Rational::zero());
  bool tempBool = outputConeLowerDimension.createFromNormals(newNormals);
  if (tempBool) {
    global.fatal << "Create from normals failed. " << global.fatal;
  }
}

bool Cone::getRootFromLPolynomialConstantTermGoesToLastVariable(
  Polynomial<Rational>& inputLPoly, Vector<Rational>& output
) {
  if (!inputLPoly.isLinear()) {
    return false;
  }
  output.makeZero(inputLPoly.minimalNumberOfVariables() + 1);
  for (int i = 0; i < inputLPoly.size(); i ++) {
    int index;
    if (
      inputLPoly[i].::MonomialPolynomial::isOneLetterFirstDegree(&index)
    ) {
      output[index] = inputLPoly.coefficients[i];
    } else {
      *output.lastObject() = inputLPoly.coefficients[i];
    }
  }
  return true;
}

bool Cone::solveLPolynomialEqualsZeroIAmProjective(
  Polynomial<Rational>& inputLPoly, Cone& outputCone
) {
  Vector<Rational> normal;
  if (
    !this->getRootFromLPolynomialConstantTermGoesToLastVariable(
      inputLPoly, normal
    )
  ) {
    return false;
  }
  this->intersectHyperplane(normal, outputCone);
  return true;
}

bool Cone::solveLQuasiPolyEqualsZeroIAmProjective(
  QuasiPolynomial& inputLQP, List<Cone>& outputConesOverEachLatticeShift
) {
  outputConesOverEachLatticeShift.setSize(inputLQP.latticeShifts.size);
  bool result = true;
  for (int i = 0; i < inputLQP.latticeShifts.size; i ++) {
    result = result &&
    this->solveLPolynomialEqualsZeroIAmProjective(
      inputLQP.valueOnEachLatticeShift[i],
      outputConesOverEachLatticeShift[i]
    );
  }
  return result;
}

std::string HtmlRoutines::toHtmlTableRowsFromStringContainingJSON(
  const std::string& inputJSON
) {
  STACK_TRACE("HtmlRoutines::toHtmlTableRowsFromStringContainingJSON");
  JSData parser;
  if (!parser.parse(inputJSON)) {
    return StringRoutines::stringTrimToLengthForDisplay(inputJSON, 1000);
  }
  return HtmlRoutines::toHtmlTableRowsFromJSON(parser);
}

std::string HtmlRoutines::toHtmlTableRowsFromJSON(const JSData& input) {
  STACK_TRACE("HtmlRoutines::ToHtmlTableFromJSON");
  if (input.elementType == JSData::token::tokenObject) {
    std::stringstream out;
    out << "<table class = \"tableDatabaseItem\">";
    for (int i = 0; i < input.objects.size(); i ++) {
      out
      << "<tr><td>"
      << input.objects.keys[i]
      << "</td>"
      << "<td>"
      << HtmlRoutines::toHtmlTableRowsFromJSON(input.objects.values[i])
      << "</td>"
      << "</tr>";
    }
    out << "</table>";
    return out.str();
  }
  if (input.elementType == JSData::token::tokenArray) {
    std::stringstream out;
    out << "<table class='tableDatabaseItem'>";
    for (int i = 0; i < input.objects.size(); i ++) {
      out
      << "<tr>"
      << "<td>"
      << HtmlRoutines::toHtmlTableRowsFromJSON(input.objects.values[i])
      << "</td>"
      << "</tr>";
    }
    out << "</table>";
    return out.str();
  }
  return input.toString(nullptr);
}

std::string HtmlRoutines::toHtmlTable(
  List<std::string>& labels,
  List<List<std::string> >& content,
  bool nestTables
) {
  STACK_TRACE("HtmlRoutines::toHtmlTable");
  std::stringstream out;
  out << "<table class='tableDatabase'>";
  out << "<tr>";
  for (int i = 0; i < labels.size; i ++) {
    out
    << "<th>"
    << StringRoutines::stringTrimToLengthForDisplay(labels[i], 1000)
    << "</th>";
  }
  out << "</tr>";
  for (int i = 0; i < content.size; i ++) {
    out << "<tr>";
    if (labels.size != content[i].size) {
      global.fatal
      << "Error: while composing table, got "
      << labels.size
      << " labels but row index "
      << i
      << " has "
      << content[i].size
      << " elements. "
      << global.fatal;
    }
    for (int j = 0; j < content[i].size; j ++) {
      if (!nestTables) {
        out
        << "<td>"
        << StringRoutines::stringTrimToLengthForDisplay(
          content[i][j], 1000
        )
        << "</td>";
      } else {
        out
        << "<td>"
        << HtmlRoutines::toHtmlTableRowsFromStringContainingJSON(
          content[i][j]
        )
        << "</td>";
      }
    }
    out << "</tr>";
  }
  out << "</table>";
  return out.str();
}

bool HtmlRoutines::convertStringToHtmlStringReturnTrueIfModified(
  const std::string& input,
  std::string& output,
  bool doReplaceNewLineByBr
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

bool HtmlRoutines::isRepresentedByItselfInURLs(char input) {
  if (MathRoutines::isDigit(input)) {
    return true;
  }
  if (MathRoutines::isLatinLetter(input)) {
    return true;
  }
  return input == '.' || input == '-' || input == '_';
}

std::string HtmlRoutines::convertStringToURLStringExceptDashesAndSlashes(
  const std::string& input
) {
  std::stringstream out;
  for (unsigned int i = 0; i < input.size(); i ++) {
    if (
      HtmlRoutines::isRepresentedByItselfInURLs(input[i]) ||
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

std::string HtmlRoutines::convertStringToURLString(
  const std::string& input, bool usePlusesForSpacebars
) {
  std::stringstream out;
  for (unsigned int i = 0; i < input.size(); i ++) {
    if (HtmlRoutines::isRepresentedByItselfInURLs(input[i])) {
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

Vector<Rational> OnePartialFractionDenominatorComponent::getCheckSumRoot(
  int numberOfVariables
) {
  Vector<Rational> output;
  output.setSize(numberOfVariables);
  for (int i = 0; i < numberOfVariables; i ++) {
    output[i] = i + 2;
  }
  return output;
}

bool PartialFractions::reduceOnceRedundantShortRoots(
  const OnePartialFractionDenominator& toBeReduced,
  OnePartialFractionDenominator& outputFraction,
  Polynomial<LargeInteger>& outputCoefficient,
  Vector<Rational>* indicator
) {
  STACK_TRACE("PartialFractions::reduceOnceRedundantShortRoots");
  outputCoefficient.makeZero();
  if (!toBeReduced.rootIsInFractionCone(indicator)) {
    return false;
  }
  bool found = false;
  Vector<Rational> normalized;
  for (int k = 0; k < toBeReduced.denominatorsNoScale.size(); k ++) {
    const OnePartialFractionDenominatorComponent& currentFraction =
    toBeReduced.denominatorsNoScale.values[k];
    if (currentFraction.elongations.size > 1) {
      found = true;
      normalized = toBeReduced.denominatorsNoScale.keys[k];
      break;
    }
  }
  if (!found) {
    return false;
  }
  const OnePartialFractionDenominatorComponent& currentFraction =
  toBeReduced.denominatorsNoScale.getValueNoFail(normalized);
  Rational localStartCheckSum;
  Rational localEndCheckSum;
  Polynomial<LargeInteger> multiplicand;
  int leastCommonMultipleElongations =
  currentFraction.getLeastCommonMultipleElongations();
  outputCoefficient.makeOne();
  outputFraction = toBeReduced;
  for (int i = 0; i < currentFraction.elongations.size; i ++) {
    int elongationValue = currentFraction.elongations[i];
    if (elongationValue == leastCommonMultipleElongations) {
      continue;
    }
    int numSummands = leastCommonMultipleElongations / elongationValue;
    Vector<Rational> exponent = normalized * elongationValue;
    toBeReduced.getNElongationPolynomial(numSummands, exponent, multiplicand);
    int multiplicityChange = currentFraction.multiplicities[i];
    multiplicand.raiseToPower(multiplicityChange, 1);
    outputCoefficient *= multiplicand;
    outputFraction.addMultiplicity(
      normalized, multiplicityChange, leastCommonMultipleElongations
    );
    outputFraction.addMultiplicity(
      normalized, - multiplicityChange, elongationValue
    );
  }
  return true;
}

void Lattice::getRougherLatticeFromAffineHyperplaneDirectionAndLattice(
  const Vector<Rational>& direction,
  Vector<Rational>& outputDirectionMultipleOnLattice,
  Vector<Rational>& shift,
  Vector<Rational>& affineHyperplane,
  Vectors<Rational>& outputRepresentatives,
  Vectors<Rational>& movementInDirectionPerRepresentative,
  Lattice& outputRougherLattice
) {
  Vector<Rational> normal = affineHyperplane;
  normal.setSize(normal.size - 1);
  if (direction.scalarEuclidean(normal).isEqualToZero()) {
    return;
  }
  Rational constOnRightHandSide = - *affineHyperplane.lastObject();
  Vectors<Rational> basis;
  basis.assignMatrixRows(this->basisRationalForm);
  Lattice hyperplaneLatticeNoShift, directionLattice;
  // , normalProjectionLattice, trueProjectionLattice;
  Vectors<Rational> roots;
  // Vector<Rational> root;
  roots.addOnTop(direction);
  directionLattice = *this;
  directionLattice.intersectWithLinearSubspaceSpannedBy(roots);
  directionLattice.basisRationalForm.getVectorFromRow(
    0, outputDirectionMultipleOnLattice
  );
  hyperplaneLatticeNoShift = *this;
  hyperplaneLatticeNoShift.intersectWithLinearSubspaceGivenByNormal(normal);
  roots.assignMatrixRows(hyperplaneLatticeNoShift.basisRationalForm);
  roots.addOnTop(outputDirectionMultipleOnLattice);
  outputRougherLattice.makeFromRoots(roots);
  this->getAllRepresentatives(outputRougherLattice, outputRepresentatives);
  for (int i = 0; i < outputRepresentatives.size; i ++) {
    outputRepresentatives[i] += shift;
    outputRougherLattice.reduceVector(outputRepresentatives[i]);
  }
  Rational shiftedConstant;
  Rational unitMovement;
  Rational scalarProduct;
  unitMovement = normal.scalarEuclidean(outputDirectionMultipleOnLattice);
  movementInDirectionPerRepresentative.setSize(outputRepresentatives.size);
  for (int i = 0; i < outputRepresentatives.size; i ++) {
    scalarProduct = (
      normal.scalarEuclidean(outputRepresentatives[i]) - constOnRightHandSide
    ) /
    unitMovement;
    scalarProduct.assignFractionalValue();
    shiftedConstant = constOnRightHandSide + scalarProduct;
    Vector<Rational>& currentMovement =
    movementInDirectionPerRepresentative[i];
    currentMovement = affineHyperplane;
    *currentMovement.lastObject() = shiftedConstant;
  }
}

bool SlTwoInSlN::computeInvariantsOfDegree(
  List<int>& decompositionDimensions,
  int degree,
  List<Polynomial<Rational> >& output,
  std::string& outputError
) {
  this->initFromModuleDecomposition(decompositionDimensions, false, false);
  SelectionWithMaxMultiplicity selection;
  selection.initMaxMultiplicity(this->dimension, degree);
  outputError = "";
  LargeInteger numberOfCycles =
  selection.numberOfCombinationsOfCardinality(degree);
  if (numberOfCycles > 1000000) {
    outputError = " Computation too large. ";
    return false;
  }
  Polynomial<Rational> basisMonomialsZeroWeight, basisMonomialsAll;
  basisMonomialsZeroWeight.makeZero();
  basisMonomialsAll.makeZero();
  MonomialPolynomial monomial;
  monomial.makeOne();
  Vector<Rational> weight;
  Vector<Rational> cartanAction;
  cartanAction.setSize(this->dimension);
  weight.setSize(this->dimension);
  for (int j = 0; j < this->dimension; j ++) {
    cartanAction[j] = this->hElement.elements[j][j];
  }
  selection.incrementSubsetFixedCardinality(degree);
  Rational monomialCoefficient = 1;
  for (
    int i = 0; i < numberOfCycles;
    i ++,
    selection.incrementSubsetFixedCardinality(degree)
  ) {
    for (int j = 0; j < this->dimension; j ++) {
      monomial.setVariable(j, selection.multiplicities[j]);
      weight[j] = monomial[j];
    }
    basisMonomialsAll.addMonomial(monomial, monomialCoefficient);
    if (weight.scalarEuclidean(cartanAction).isEqualToZero()) {
      basisMonomialsZeroWeight.addMonomial(monomial, monomialCoefficient);
    }
  }
  Matrix<Rational> matrix;
  matrix.initialize(
    basisMonomialsAll.size() * 2, basisMonomialsZeroWeight.size()
  );
  Polynomial<Rational> tempP;
  for (int l = 0; l < 2; l ++) {
    for (int k = 0; k < basisMonomialsZeroWeight.size(); k ++) {
      if (l == 0) {
        this->eElement.actOnMonomialAsDifferentialOperator(
          basisMonomialsZeroWeight[k], tempP
        );
      } else {
        this->fElement.actOnMonomialAsDifferentialOperator(
          basisMonomialsZeroWeight[k], tempP
        );
      }
      for (int j = 0; j < basisMonomialsAll.size(); j ++) {
        int indexInResult = tempP.monomials.getIndex(basisMonomialsAll[j]);
        int currentRow = l * basisMonomialsAll.size() + j;
        if (indexInResult == - 1) {
          matrix.elements[currentRow][k] = 0;
        } else {
          matrix.elements[currentRow][k] = tempP.coefficients[indexInResult];
        }
      }
    }
  }
  Vectors<Rational> roots;
  matrix.getZeroEigenSpaceModifyMe(roots);
  output.setSize(roots.size);
  for (int i = 0; i < output.size; i ++) {
    Polynomial<Rational>& current = output[i];
    current.makeZero();
    for (int j = 0; j < basisMonomialsZeroWeight.size(); j ++) {
      if (!roots[i][j].isEqualToZero()) {
        monomial = basisMonomialsZeroWeight[j];
        current.addMonomial(monomial, roots[i][j]);
      }
    }
  }
  return true;
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

std::string ConeLatticeAndShift::toString(FormatExpressions& format) {
  std::stringstream out;
  out << this->projectivizedCone.toString(&format);
  out
  << "<br>Shift +lattice: "
  << this->shift.toString()
  << " + "
  << this->lattice.toString();
  return out.str();
}

void DrawOperations::makeMeAStandardBasis(int dimension) {
  if (dimension < 1) {
    return;
  }
  if (dimension > 3) {
    this->projectionsEiVectors.setSizeMakeMatrix(dimension, 2);
    for (int i = 0; i < dimension; i ++) {
      this->projectionsEiVectors[i][0] =
      FloatingPoint::sinFloating(
        static_cast<double>(i) / static_cast<double>(
          dimension * MathRoutines::pi()
        )
      );
      this->projectionsEiVectors[i][1] =
      FloatingPoint::cosFloating(
        static_cast<double>(i) / static_cast<double>(
          dimension * MathRoutines::pi()
        )
      );
    }
  } else if (dimension == 3) {
    this->projectionsEiVectors.setSizeMakeMatrix(3, 2);
    this->projectionsEiVectors[0][0] = 1;
    this->projectionsEiVectors[0][1] = - 0.2;
    this->projectionsEiVectors[1][0] = 0.1;
    this->projectionsEiVectors[1][1] = 1;
    this->projectionsEiVectors[2][0] = 0.01;
    this->projectionsEiVectors[2][1] = 0.01;
  } else {
    // dimension = 2;
    this->projectionsEiVectors.setSizeMakeMatrix(2, 2);
    this->projectionsEiVectors[0][0] = 1;
    this->projectionsEiVectors[0][1] = 0;
    this->projectionsEiVectors[1][0] = 0;
    this->projectionsEiVectors[1][1] = 1;
  }
  if (this->basisProjectionPlane.size < 1) {
    this->basisProjectionPlane.setSize(1);
  }
  this->basisProjectionPlane.makeEiBasis(dimension);
  this->basisProjectionPlane.setSize(2);
  if (dimension > 3) {
    for (int i = 0; i < this->basisProjectionPlane[1].size; i ++) {
      this->basisProjectionPlane[1][i] = 2 * i + 1;
    }
    for (int i = 0; i < this->basisProjectionPlane[0].size; i ++) {
      this->basisProjectionPlane[0][i] = 3 * i + 2;
    }
    modifyToOrthonormalNoShiftSecond(
      this->basisProjectionPlane[0], this->basisProjectionPlane[1]
    );
  } else if (dimension == 3) {
    // <-if not needed but good for documentation purposes
    this->basisProjectionPlane[0][0] =
    0.6;
    this->basisProjectionPlane[0][1] = 0.4;
    this->basisProjectionPlane[0][2] = 0;
    this->basisProjectionPlane[1][0] = - 0.4;
    this->basisProjectionPlane[1][1] = 0.6;
    this->basisProjectionPlane[1][2] = 1;
    modifyToOrthonormalNoShiftSecond(
      this->basisProjectionPlane[0], this->basisProjectionPlane[1]
    );
  } else if (dimension == 2) {
    this->basisProjectionPlane[1] *= - 1;
  }
  if (this->bilinearForm.numberOfRows != dimension) {
    this->bilinearForm.makeIdentityMatrix(dimension, 1, 0);
  }
}

std::string ConeCollection::drawMeToHtmlLastCoordAffine(
  DrawingVariables& drawingVariables, FormatExpressions& format
) {
  bool isBad = false;
  isBad = this->drawMeLastCoordinateAffine(true, drawingVariables, format);
  std::stringstream out;
  out << drawingVariables.getHTMLDiv(this->getDimension() - 1, false);
  if (isBad) {
    out << "<hr>" << "found cones which I can't draw<hr>";
  }
  out << this->toHTML();
  return out.str();
}

std::string ConeCollection::drawMeToHtmlProjective(
  DrawingVariables& drawingVariables, FormatExpressions& format
) const {
  STACK_TRACE("ConeCollection::drawMeToHtmlProjective");
  bool isGood = true;
  isGood = this->drawMeProjective(nullptr, true, drawingVariables, format);
  std::stringstream out;
  out << drawingVariables.getHTMLDiv(this->getDimension(), false);
  if (!isGood) {
    out << "<hr>" << "found cones which I can't draw<hr>";
  }
  return out.str();
}

bool ConeCollection::drawMeLastCoordinateAffine(
  bool initializeDrawingVariables,
  DrawingVariables& drawingVariables,
  FormatExpressions& format
) {
  STACK_TRACE("ConeCollection::drawMeLastCoordinateAffine");
  this->checkIsRefinedOrCrash();
  bool result = true;
  if (initializeDrawingVariables) {
    drawingVariables.operations.initDimensions(this->getDimension() - 1);
  }
  drawingVariables.drawCoordSystemBuffer(
    drawingVariables, this->getDimension() - 1
  );
  for (int i = 0; i < this->refinedCones.size; i ++) {
    if (
      !this->refinedCones[i].drawMeLastCoordinateAffine(
        initializeDrawingVariables, drawingVariables, format
      )
    ) {
      result = false;
    }
    std::stringstream tempStream;
    tempStream << i + 1;
    Vector<Rational> root = this->nonRefinedCones[i].getInternalPoint();
    root.makeAffineUsingLastCoordinate();
    drawingVariables.drawTextAtVectorBufferRational(
      root, tempStream.str(), "black"
    );
  }
  return result;
}

bool ConeCollection::drawMeProjectiveInitialize(
  DrawingVariables& drawingVariables
) const {
  STACK_TRACE("ConeCollection::drawMeProjectiveInitialize");
  Matrix<Rational> matrix;
  drawingVariables.operations.initialize();
  drawingVariables.operations.initDimensions(this->getDimension());
  drawingVariables.operations.makeMeAStandardBasis(this->getDimension());
  drawingVariables.drawCoordSystemBuffer(
    drawingVariables, this->getDimension()
  );
  Vectors<Rational> roots;
  Vector<Rational> root;
  if (this->getDimension() == 2) {
    return true;
  }
  if (!this->convexHull.getInternalPoint(root)) {
    global.comments
    << "Failed to get internal point of convex hull: "
    << this->convexHull.toString();
    return false;
  }
  matrix.assignVectorRow(root);
  matrix.getZeroEigenSpace(roots);
  for (int i = 0; i < 2; i ++) {
    for (int j = 0; j < this->getDimension(); j ++) {
      drawingVariables.operations.basisProjectionPlane[i][j] =
      roots[i][j].getDoubleValue();
    }
  }
  return true;
}

bool ConeCollection::drawMeProjective(
  Vector<Rational>* coordCenterTranslation,
  bool initDrawVars,
  DrawingVariables& drawingVariables,
  FormatExpressions& format
) const {
  STACK_TRACE("ConeCollection::drawMeProjective");
  // There's no non-technical reason to refuse drawing non-refined cones
  // perhaps this needs to be relaxed.
  this->checkIsRefinedOrCrash();
  bool result = true;
  if (this->getDimension() <= 1) {
    return false;
  }
  if (initDrawVars) {
    this->drawMeProjectiveInitialize(drawingVariables);
  }
  for (int i = 0; i < this->refinedCones.size; i ++) {
    if (
      !this->refinedCones[i].drawMeProjective(
        coordCenterTranslation, false, drawingVariables, format
      )
    ) {
      result = false;
    }
  }
  return result;
}

bool Cone::isEntireSpace() {
  return this->normals.size == 0 && this->flagIsTheZeroCone;
}

bool Cone::isHonest1DEdgeAffine(int vertexIndex1, int vertexIndex2) const {
  Vector<Rational>& vertex1 = this->vertices[vertexIndex1];
  Vector<Rational>& vertex2 = this->vertices[vertexIndex2];
  return this->isHonest1DEdgeAffine(vertex1, vertex2);
}

bool Cone::isRegularToBasis(
  const Vectors<Rational>& basis,
  Vector<Rational>& input,
  Vector<Rational>& outputFailingNormal,
  int dimension
) {
  Selection wallSelection;
  wallSelection.initialize(basis.size);
  LargeInteger x = MathRoutines::nChooseK(basis.size, dimension - 1);
  Matrix<Rational> bufferMat;
  Vector<Rational> candidate;
  Rational scalarProduct;
  for (int i = 0; i < x; i ++) {
    wallSelection.incrementSelectionFixedCardinality(dimension - 1);
    if (
      basis.computeNormalFromSelection(
        candidate, wallSelection, bufferMat, dimension
      )
    ) {
      candidate.scalarEuclidean(input, scalarProduct);
      if (scalarProduct.isEqualToZero()) {
        outputFailingNormal = candidate;
        return false;
      }
    }
  }
  return true;
}

bool Cone::regularizeToBasis(
  const Vectors<Rational>& basis, Vector<Rational>& output
) {
  Vector<Rational> normal;
  bool result = false;
  while (
    !Cone::isRegularToBasis(basis, output, normal, output.size)
  ) {
    result = true;
    normal /= 10;
    output += normal;
  }
  if (result) {
    Cone::scaleNormalizeByPositive(output);
  }
  return result;
}

bool Cone::drawMeLastCoordinateAffine(
  bool initializeDrawingVariables,
  DrawingVariables& drawingVariables,
  FormatExpressions& format,
  const std::string& chamberWallColor
) const {
  (void) format;
  (void) initializeDrawingVariables;
  Vector<Rational> zeroRoot;
  zeroRoot.makeZero(this->getDimension() - 1);
  Vectors<Rational> verticesScaled;
  verticesScaled = this->vertices;
  Rational scalar;
  List<bool> drawVertex;
  drawVertex.initializeFillInObject(this->vertices.size, true);
  bool foundBadVertex = false;
  for (int i = 0; i < this->vertices.size; i ++) {
    scalar = *verticesScaled[i].lastObject();
    verticesScaled[i].setSize(this->getDimension() - 1);
    if (scalar.isPositive()) {
      verticesScaled[i] /= scalar;
    }
    if (scalar.isEqualToZero()) {
      verticesScaled[i] *= 10000;
    }
    if (scalar.isNegative()) {
      drawVertex[i] = false;
      foundBadVertex = true;
    }
  }
  for (int k = 0; k < this->normals.size; k ++) {
    for (int i = 0; i < verticesScaled.size; i ++) {
      if (
        !drawVertex[i] ||
        !this->normals[k].scalarEuclidean(this->vertices[i]).isEqualToZero()
      ) {
        continue;
      }
      for (int j = i + 1; j < verticesScaled.size; j ++) {
        if (
          !drawVertex[j] ||
          !this->normals[k].scalarEuclidean(this->vertices[j]).isEqualToZero()
        ) {
          continue;
        }
        if (!this->isHonest1DEdgeAffine(i, j)) {
          continue;
        }
        drawingVariables.drawLineBetweenTwoVectorsBufferRational(
          verticesScaled[i], verticesScaled[j], chamberWallColor, 1
        );
      }
    }
  }
  return foundBadVertex;
}

std::string Cone::drawMeToHtmlLastCoordAffine(
  DrawingVariables& drawingVariables, FormatExpressions& format
) {
  if (this->flagIsTheZeroCone) {
    return "The cone is empty. ";
  }
  if (this->normals.size < 1) {
    return "The cone is the entire space";
  }
  if (this->vertices.size < 1) {
    return "The cone is empty";
  }
  std::stringstream out;
  drawingVariables.operations.makeMeAStandardBasis(
    this->getDimension() - 1
  );
  bool foundBadVertex =
  this->drawMeLastCoordinateAffine(false, drawingVariables, format);
  drawingVariables.drawCoordSystemBuffer(
    drawingVariables, this->getDimension() - 1
  );
  if (foundBadVertex) {
    out << "<br>The cone does not lie in the upper half-space. ";
  } else {
    out << drawingVariables.getHTMLDiv(this->getDimension() - 1, false);
  }
  out << "<br>" << this->toString(&format);
  return out.str();
}

bool Cone::drawMeProjective(
  Vector<Rational>* coordCenterTranslation,
  bool initTheDrawVars,
  DrawingVariables& drawingVariables,
  FormatExpressions& format
) const {
  STACK_TRACE("Cone::drawMeProjective");
  (void) format;
  Vector<Rational> zeroRoot;
  Vector<Rational> coordinateCenter;
  zeroRoot.makeZero(this->getDimension());
  if (coordCenterTranslation == nullptr) {
    coordinateCenter = zeroRoot;
  } else {
    coordinateCenter = *coordCenterTranslation;
  }
  Vectors<Rational> verticesScaled = this->vertices;
  for (int i = 0; i < verticesScaled.size; i ++) {
    Rational sumAbsoluteValuesCoordinates = 0;
    for (int j = 0; j < this->getDimension(); j ++) {
      sumAbsoluteValuesCoordinates += (
        verticesScaled[i][j].isPositive()
      ) ?
      verticesScaled[i][j] :
      - verticesScaled[i][j];
    }
    if (sumAbsoluteValuesCoordinates.isEqualToZero()) {
      global.fatal << "Zero vector not allowed. " << global.fatal;
    }
    verticesScaled[i] /= sumAbsoluteValuesCoordinates;
  }
  Vector<Rational> root;
  if (initTheDrawVars) {
    drawingVariables.operations.makeMeAStandardBasis(this->getDimension());
    for (int i = 0; i < this->getDimension(); i ++) {
      root.makeEi(this->getDimension(), i);
      drawingVariables.drawLineBetweenTwoVectorsBufferRational(
        zeroRoot + coordinateCenter, root + coordinateCenter, "gray", 1
      );
    }
  }
  for (int i = 0; i < this->vertices.size; i ++) {
    drawingVariables.drawLineBetweenTwoVectorsBufferRational(
      zeroRoot + coordinateCenter,
      verticesScaled[i] * 5 + coordinateCenter,
      "lightblue",
      1
    );
  }
  for (int k = 0; k < this->normals.size; k ++) {
    for (int i = 0; i < this->vertices.size; i ++) {
      if (
        !this->normals[k].scalarEuclidean(this->vertices[i]).isEqualToZero()
      ) {
        continue;
      }
      for (int j = i + 1; j < this->vertices.size; j ++) {
        if (
          !this->normals[k].scalarEuclidean(this->vertices[j]).isEqualToZero()
        ) {
          continue;
        }
        if (!this->isHonest1DEdgeAffine(i, j)) {
          continue;
        }
        drawingVariables.drawLineBetweenTwoVectorsBufferRational(
          verticesScaled[i] + coordinateCenter,
          verticesScaled[j] + coordinateCenter,
          "black",
          1
        );
      }
    }
  }
  return true;
}

std::string Cone::drawMeToHtmlProjective(
  DrawingVariables& drawingVariables, FormatExpressions& format
) {
  if (this->flagIsTheZeroCone) {
    return "The cone is the zero cone (i.e. contains only the origin).";
  }
  if (this->normals.size <= 0) {
    return "The cone is the entire space.";
  }
  std::stringstream out;
  if (this->vertices.size < 1) {
    out
    << "There has been a programming error. The cone is empty.<br>"
    << this->toString(&format);
    return out.str();
  }
  drawingVariables.operations.makeMeAStandardBasis(this->getDimension());
  this->drawMeProjective(nullptr, true, drawingVariables, format);
  drawingVariables.drawCoordSystemBuffer(
    drawingVariables, this->getDimension()
  );
  out << drawingVariables.getHTMLDiv(this->getDimension(), false);
  out << "<br>" << this->toString(&format);
  return out.str();
}

int DrawOperations::getDimensionFromBilinearForm() {
  return this->bilinearForm.numberOfRows;
}

void DrawOperations::initDimensions(int dimension) {
  if (dimension < 2) {
    dimension = 2;
  }
  this->bilinearForm.makeIdentityMatrix(dimension, 1, 0);
  this->projectionsEiVectors.setSizeMakeMatrix(dimension, 2);
  this->basisProjectionPlane.makeEiBasis(dimension);
  this->basisProjectionPlane.size = 2;
  /*  for (int i = 0; i < tempBasis[1].size; i ++)
    tempBasis[1][i] =2*i + 1;
  for (int i = 0; i < tempBasis[0].size; i ++)
    tempBasis[0][i] =3*i +2;*/
  this->modifyToOrthonormalNoShiftSecond(
    this->basisProjectionPlane[1], this->basisProjectionPlane[0]
  );
  this->basisToDrawCirclesAt.makeEiBasis(dimension);
  this->selectedCircleMinus2noneMinus1Center = - 2;
  this->centerX = 300;
  this->centerY = 300;
  this->graphicsUnit = DrawOperations::graphicsUnitDefault;
  this->frameLengthInMilliseconds = 500;
}

int DrawOperations::getDimensionFirstDimensionDependentOperation() {
  for (int i = 0; i < this->operations.size; i ++) {
    if (
      this->operations[i][fieldOperation].stringValue ==
      DrawOperations::typeSegment
    ) {
      if (
        this->operations[i][DrawOperations::fieldPoints].listObjects.size > 0
      ) {
        return
        this->operations[i][DrawOperations::fieldPoints][0].listObjects.size;
      }
    }
  }
  return - 1;
}

void DrawOperations::ensureProperInitialization() {
  int dimension = this->getDimensionFirstDimensionDependentOperation();
  bool isGood = (
    this->projectionsEiVectors.size == dimension &&
    this->bilinearForm.numberOfRows == dimension
  );
  if (isGood) {
    isGood = this->basisProjectionPlane.size == 2;
  }
  if (isGood) {
    isGood = this->basisProjectionPlane[0].size == dimension;
  }
  if (!isGood) {
    this->initDimensions(dimension);
  }
}

template <class Base>
std::iostream& operator<<(
  std::iostream& output, const Complex<Base>& input
) {
  if (input.isEqualToZero()) {
    output << "0";
    return output;
  }
  if (input.realPart != 0) {
    output << input.realPart;
    if (input.imaginaryPart != 0) {
      output << " + ";
    }
  }
  if (input.imaginaryPart != 0) {
    output << "i";
  }
  if (input.imaginaryPart < 0) {
    output << "(";
  }
  if (input.imaginaryPart != 1) {
    output << input.imaginaryPart;
  }
  if (input.imaginaryPart < 0) {
    output << ")";
  }
  return output;
}

void DrawOperations::initialize() {
  this->operations.reserve(1000);
  this->centerX = 300;
  this->centerY = 300;
  this->graphicsUnit = DrawOperations::graphicsUnitDefault;
  this->clickToleranceX = 5;
  this->clickToleranceY = 5;
  this->selectedCircleMinus2noneMinus1Center = - 2;
  this->flagRotatingPreservingAngles = true;
  this->flagAnimatingMovingCoordSystem = false;
  this->flagIsPausedWhileAnimating = false;
}

double DrawOperations::getAngleFromXandY(double x, double y) {
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

void DrawOperations::click(double x, double y) {
  this->ensureProperInitialization();
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
    this->getCoordsDrawingComputeAll(
      this->basisToDrawCirclesAt[i], Xbasis, Ybasis
    );
    if (this->areWithinClickTolerance(x, y, Xbasis, Ybasis)) {
      this->selectedCircleMinus2noneMinus1Center = i;
      return;
    }
  }
}

void DrawOperations::rotateOutOfPlane(
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

void DrawOperations::modifyToOrthonormalNoShiftSecond(
  Vector<double>& root1, Vector<double>& root2
) {
  double scalar = this->bilinearForm.scalarProduct(root1, root2) /
  this->bilinearForm.scalarProduct(root2, root2);
  root1 -= root2 * scalar;
  this->scaleToUnitLength(root1);
  this->scaleToUnitLength(root2);
}

void DrawOperations::computeProjectionsEiVectors() {
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

void DrawOperations::changeBasisPReserveAngles(double newX, double newY) {
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
  this->getCoordsDrawingComputeAll(selectedRoot, oldX, oldY);
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
  if (this->specialOperationsOnBasisChange != nullptr) {
    this->specialOperationsOnBasisChange(*this);
  }
  this->computeProjectionsEiVectors();
  this->DebugString = out.str();
}

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
      result.value =
      this->value / (other.value - other.value);
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

void DrawOperations::projectionMultiplicityMergeOnBasisChange(
  DrawOperations& operations
) {
  Matrix<ImpreciseDouble> matrix;
  int dimension = operations.bilinearForm.numberOfRows;
  matrix.initialize(dimension, 2);
  // We assume that the computeProjectionsEiVectors has been called.
  for (int i = 0; i < operations.projectionsEiVectors.size; i ++) {
    for (int j = 0; j < 2; j ++) {
      matrix.elements[i][j] = operations.projectionsEiVectors[i][j];
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

void DrawOperations::operator+=(const DrawOperations& other) {
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

void Selection::operator=(const Vector<Rational>& other) {
  this->initialize(other.size);
  for (int i = 0; i < other.size; i ++) {
    if (!other[i].isEqualToZero()) {
      this->selected[i] = true;
    }
  }
  this->computeIndicesFromSelection();
}

void ConeCollection::initializeFromAffineDirectionsAndRefine(
  Vectors<Rational>& inputDirections,
  Vectors<Rational>& inputAffinePoints
) {
  if (
    inputDirections.size != inputAffinePoints.size || inputDirections.size <= 0
  ) {
    global.fatal
    << "Input directions size does not match affine point size. "
    << global.fatal;
  }
  Vectors<Rational> projectivizedDirections;
  projectivizedDirections.setSize(inputDirections.size * 2);
  int affineDimension = inputDirections[0].size;
  for (int i = 0; i < inputDirections.size; i ++) {
    projectivizedDirections[i] = inputDirections[i];
    projectivizedDirections[i].setSize(affineDimension + 1);
    *projectivizedDirections[i].lastObject() = 0;
  }
  for (int i = 0; i < inputAffinePoints.size; i ++) {
    projectivizedDirections[i + inputAffinePoints.size] = inputAffinePoints[i];
    projectivizedDirections[i + inputAffinePoints.size].setSize(
      affineDimension + 1
    );
    *projectivizedDirections[i + inputAffinePoints.size].lastObject() = 1;
  }
  this->initializeFromDirectionsAndRefine(projectivizedDirections);
}

int ConeCollection::getDimension() const {
  if (this->refinedCones.size > 0) {
    return this->refinedCones[0].getDimension();
  }
  if (this->nonRefinedCones.size > 0) {
    return this->nonRefinedCones[0].getDimension();
  }
  return - 1;
}

int ConeCollection::getLowestIndexRefinedChamberContaining(
  const Vector<Rational>& root
) const {
  for (int i = 0; i < this->refinedCones.size; i ++) {
    if (this->refinedCones[i].isInCone(root)) {
      return i;
    }
  }
  return - 1;
}

void ConeCollection::makeAffineAndTransformToProjectiveDimensionPlusOne(
  Vector<Rational>& affinePoint, ConeCollection& output
) {
  if (&output == this) {
    global.fatal
    << "Output coincides with input, not allowed. "
    << global.fatal;
  }
  this->checkIsRefinedOrCrash();
  output.initialize();
  Cone incoming;
  Vectors<Rational> newNormals;
  int affineDimension = affinePoint.size;
  for (int i = 0; i < this->refinedCones.size; i ++) {
    const Cone& refined = this->refinedCones[i];
    newNormals.setSize(refined.normals.size + 1);
    for (int j = 0; j < refined.normals.size; j ++) {
      newNormals[j] = refined.normals[j].getProjectivizedNormal(affinePoint);
    }
    newNormals.lastObject()->makeEi(affineDimension + 1, affineDimension);
    incoming.createFromNormals(newNormals);
    output.convexHull.makeConvexHullOfMeAnd(incoming);
    output.refinedCones.addOnTop(incoming);
  }
}

template <class Coefficient>
Vector<Coefficient> Vector<Coefficient>::getProjectivizedNormal(
  Vector<Coefficient>& affinePoint
) {
  Vector<Coefficient> result = *this;
  result.setSize(this->size + 1);
  *result.lastObject() = - affinePoint.scalarEuclidean(*this);
  return result;
}

void Lattice::getRootOnLatticeSmallestPositiveProportionalTo(
  Vector<Rational>& input, Vector<Rational>& output
) {
  if (&input == &output) {
    global.fatal
    << "Input not allowed to coincide with output. "
    << global.fatal;
  }
  Vectors<Rational> basis;
  Vector<Rational> root;
  basis.assignMatrixRows(this->basisRationalForm);
  input.getCoordinatesInBasis(basis, root);
  Cone::scaleNormalizeByPositive(root);
  Matrix<Rational> matrix;
  matrix = this->basisRationalForm;
  matrix.transpose();
  matrix.actOnVectorColumn(root, output);
}

bool Cone::getLatticePointsInCone(
  Lattice& lattice,
  Vector<Rational>& shift,
  int upperBoundPointsInEachDim,
  bool lastCoordinateIsOne,
  Vectors<Rational>& outputPoints,
  Vector<Rational>* shiftAllPointsBy
) const {
  STACK_TRACE("Cone::getLatticePointsInCone");
  if (upperBoundPointsInEachDim <= 0) {
    upperBoundPointsInEachDim = 5;
  }
  Vector<Rational> actualShift = shift;
  lattice.reduceVector(actualShift);
  int dimensionAffine = this->getDimension();
  if (lastCoordinateIsOne) {
    dimensionAffine --;
  }
  SelectionWithMaxMultiplicity boundingBox;
  boundingBox.initMaxMultiplicity(
    dimensionAffine, upperBoundPointsInEachDim* 2
  );
  // format of the boundingBox:
  // if bounding box shows a vector (x_1, ...) then
  // it corresponds to a vector with coodinates (x_1-upperBoundPointsInEachDim,
  // x_2-upperBoundPointsInEachDim, ...)
  int numCycles = boundingBox.numberOfSelectionsTotal();
  if (numCycles <= 0 || numCycles > 1000000) {
    // We test up to 1 million lattice points.
    // This is very restrictive: in 8 dimensions, selecting
    // upperBoundPointsInEachDim=2,
    // we get a total of (2*2+ 1)^8=390625 points to test, which is a pretty
    // darn small box
    return false;
  }
  outputPoints.reserve(numCycles);
  outputPoints.size = 0;
  Vector<Rational> candidatePoint;
  Vectors<Rational> LatticeBasis;
  LatticeBasis.assignMatrixRows(lattice.basisRationalForm);
  for (int i = 0; i < numCycles; i ++, boundingBox.incrementSubset()) {
    candidatePoint = actualShift;
    if (shiftAllPointsBy != nullptr) {
      candidatePoint += *shiftAllPointsBy;
    }
    for (int j = 0; j < boundingBox.multiplicities.size; j ++) {
      candidatePoint +=
      LatticeBasis[j] *(
        boundingBox.multiplicities[j] - upperBoundPointsInEachDim
      );
    }
    if (lastCoordinateIsOne) {
      candidatePoint.setSize(candidatePoint.size + 1);
      *candidatePoint.lastObject() = 1;
    }
    if (this->isInCone(candidatePoint)) {
      outputPoints.addOnTop(candidatePoint);
    }
  }
  return true;
}

void PiecewiseQuasipolynomial::operator*=(const Rational& other) {
  if (other.isEqualToZero()) {
    this->makeZero(this->numberOfVariables);
    return;
  }
  for (int i = 0; i < this->quasiPolynomials.size; i ++) {
    this->quasiPolynomials[i] *= other;
  }
}

void PiecewiseQuasipolynomial::operator+=(
  const PiecewiseQuasipolynomial& other
) {
  this->makeCommonRefinement(other);
  for (
    int i = 0; i < this->projectivizedComplex.refinedCones.size; i ++
  ) {
    int index =
    other.projectivizedComplex.getLowestIndexRefinedChamberContaining(
      this->projectivizedComplex.refinedCones[i].getInternalPoint()
    );
    if (index != - 1) {
      this->quasiPolynomials[i] += other.quasiPolynomials[index];
    }
  }
}

bool PiecewiseQuasipolynomial::makeVPF(
  Vectors<Rational>& roots, std::string& outputstring
) {
  if (roots.size <= 0) {
    outputstring = "Error.";
    return false;
  }
  this->numberOfVariables = roots.getDimension();
  PartialFractions partialFractions;
  FormatExpressions format;
  std::stringstream out;
  std::string whatWentWrong;
  partialFractions.initializeFromVectors(roots, nullptr);
  out
  << HtmlRoutines::getMathNoDisplay(partialFractions.toString(&format));
  partialFractions.split(nullptr);
  out
  << HtmlRoutines::getMathNoDisplay(partialFractions.toString(&format));
  // partialFractions.chambers.initializeFromDirectionsAndRefine(roots);
  global.fatal << "Not implemented. " << global.fatal;
  // partialFractions.chambersOld.AmbientDimension = roots[0].size;
  // partialFractions.chambersOld.directions = roots;
  // partialFractions.chambersOld.SliceEuclideanSpace(false);
  // partialFractions.chambers.AssignCombinatorialChamberComplex(partialFractions.chambersOld);
  this->quasiPolynomials.setSize(
    partialFractions.chambers.refinedCones.size
  );
  Vector<Rational> indicator;
  for (
    int i = 0; i < partialFractions.chambers.refinedCones.size; i ++
  ) {
    indicator = partialFractions.chambers.refinedCones[i].getInternalPoint();
    partialFractions.computeOneVectorPartitionFunction(
      this->quasiPolynomials[i], indicator
    );
    // QuasiPolynomial& currentQP = this->qps[i];
  }
  Lattice baseLattice;
  baseLattice.makeFromRoots(roots);
  Cone baseCone;
  baseCone.createFromVertices(roots);
  Vector<Rational> shiftRoot;
  baseLattice.getInternalPointInConeForSomeFundamentalDomain(
    shiftRoot, baseCone
  );
  shiftRoot.negate();
  partialFractions.chambers.makeAffineAndTransformToProjectiveDimensionPlusOne(
    shiftRoot, this->projectivizedComplex
  );
  outputstring = out.str();
  return true;
}

bool Lattice::getInternalPointInConeForSomeFundamentalDomain(
  Vector<Rational>& output, Cone& coneContainingOutputPoint
) {
  Vector<Rational> coordinatesInBasis;
  coneContainingOutputPoint.getInternalPoint(output);
  Vectors<Rational> basisRoots;
  basisRoots.assignMatrixRows(this->basisRationalForm);
  if (!output.getCoordinatesInBasis(basisRoots, coordinatesInBasis)) {
    return false;
  }
  Rational maximalCoordinate = coordinatesInBasis[0];
  if (maximalCoordinate < 0) {
    maximalCoordinate = - maximalCoordinate;
  }
  for (int i = 0; i < coordinatesInBasis.size; i ++) {
    Rational candidate = (coordinatesInBasis[i] < 0) ?
    - coordinatesInBasis[i] :
    coordinatesInBasis[i];
    if (candidate > maximalCoordinate) {
      maximalCoordinate = candidate;
    }
  }
  maximalCoordinate += 1;
  output /= maximalCoordinate;
  return true;
}

bool Cone::isHonest1DEdgeAffine(
  const Vector<Rational>& vertex1, const Vector<Rational>& vertex2
) const {
  int numberOfCommonWalls = 0;
  for (int i = 0; i < this->normals.size; i ++) {
    if (
      vertex1.scalarEuclidean(this->normals[i]).isEqualToZero() &&
      vertex2.scalarEuclidean(this->normals[i]).isEqualToZero()
    ) {
      numberOfCommonWalls ++;
      if (numberOfCommonWalls == this->getDimension() - 2) {
        return true;
      }
    }
  }
  return false;
}

void Cone::translateMeMyLastCoordinateAffinization(
  Vector<Rational>& translationVector
) {
  if (translationVector.size != this->getDimension() - 1) {
    global.fatal
    << "Translation vector size does not equal dimension minus one. "
    << global.fatal;
  }
  Vector<Rational> root;
  for (int i = 0; i < this->normals.size; i ++) {
    root = this->normals[i];
    root.size --;
    (*this->normals[i].lastObject()) -=
    root.scalarEuclidean(translationVector);
  }
  root = translationVector;
  root.setSize(translationVector.size + 1);
  *root.lastObject() = 0;
  for (int i = 0; i < this->vertices.size; i ++) {
    if (!this->vertices[i].lastObject()->isEqualToZero()) {
      this->vertices[i] /= *this->vertices[i].lastObject();
      this->vertices[i] += root;
      Cone::scaleNormalizeByPositive(this->vertices[i]);
    }
  }
}

void ConeCollection::
getAllWallsConesNoOrientationNoRepetitionNoSplittingNormals(
  Vectors<Rational>& output
) const {
  HashedList<Vector<Rational> > outputHashed;
  Vector<Rational> root;
  for (int i = 0; i < this->nonRefinedCones.size; i ++) {
    for (int j = 0; j < this->nonRefinedCones[i].normals.size; j ++) {
      root = this->nonRefinedCones[i].normals[j];
      root.scaleNormalizeFirstNonZero();
      outputHashed.addOnTopNoRepetition(root);
    }
  }
  output = outputHashed;
}

void ConeCollection::refineMakeCommonRefinement(const ConeCollection& other) {
  Vectors<Rational> newWalls;
  Cone currentCone = this->convexHull;
  if (currentCone.makeConvexHullOfMeAnd(other.convexHull)) {
    this->getAllWallsConesNoOrientationNoRepetitionNoSplittingNormals(
      newWalls
    );
    this->initialize();
    this->convexHull = currentCone;
    this->nonRefinedCones.addOnTop(currentCone);
    this->convexHull.makeConvexHullOfMeAnd(currentCone);
    this->splittingNormals.addOnTopNoRepetition(newWalls);
  }
  other.getAllWallsConesNoOrientationNoRepetitionNoSplittingNormals(newWalls);
  this->splittingNormals.addOnTopNoRepetition(newWalls);
  this->refine();
}

void ConeCollection::translateMeMyLastCoordinateAffinization(
  Vector<Rational>& translationVector
) {
  STACK_TRACE("ConeCollection::translateMeMyLastCoordinateAffinization");
  this->checkIsRefinedOrCrash();
  ConeCollection myCopy;
  myCopy = *this;
  this->initialize();
  Cone currentCone;
  for (int i = 0; i < myCopy.refinedCones.size; i ++) {
    currentCone = myCopy.refinedCones[i];
    currentCone.translateMeMyLastCoordinateAffinization(translationVector);
    this->refinedCones.addOnTop(currentCone);
    this->convexHull.makeConvexHullOfMeAnd(currentCone);
  }
  Vector<Rational> normalNoAffinePart, newNormal;
  for (int j = 0; j < myCopy.splittingNormals.size; j ++) {
    normalNoAffinePart = myCopy.splittingNormals[j];
    newNormal = normalNoAffinePart;
    normalNoAffinePart.size --;
    (*newNormal.lastObject()) -=
    normalNoAffinePart.scalarEuclidean(translationVector);
    this->splittingNormals.addOnTop(newNormal);
  }
}

void PiecewiseQuasipolynomial::translateArgument(
  Vector<Rational>& translateToBeAddedToArgument
) {
  Vector<Rational> chamberShift = - translateToBeAddedToArgument;
  this->projectivizedComplex.translateMeMyLastCoordinateAffinization(
    chamberShift
  );
  QuasiPolynomial currentContribution;
  for (int i = 0; i < this->quasiPolynomials.size; i ++) {
    this->quasiPolynomials[i].substitution(
      - translateToBeAddedToArgument, currentContribution
    );
    this->quasiPolynomials[i] = currentContribution;
  }
}

std::string PiecewiseQuasipolynomial::toString(FormatExpressions* format) {
  std::stringstream out;
  for (
    int i = 0; i < this->projectivizedComplex.refinedCones.size; i ++
  ) {
    const Cone& currentCone = this->projectivizedComplex.refinedCones[i];
    QuasiPolynomial& currentQuasiPolynomial = this->quasiPolynomials[i];
    out << "Chamber number " << i + 1;
    out << currentCone.toString(format);
    out << "quasipolynomial: ";
    out << currentQuasiPolynomial.toString();
  }
  return out.str();
}

void PiecewiseQuasipolynomial::drawMe(
  DrawingVariables& drawingVariables,
  int numberOfLatticePointsPerDimension,
  Cone* restrictingChamber,
  Vector<Rational>* distinguishedPoint
) {
  FormatExpressions format;
  Vectors<Rational> latticePoints;
  HashedList<Vector<Rational> > latticePointsFinal;
  List<std::string> latticePointColors;
  List<std::string> tempList;
  if (numberOfLatticePointsPerDimension < 0) {
    numberOfLatticePointsPerDimension = 0;
  }
  const std::string zeroColor = "gray";
  for (
    int i = 0; i < this->projectivizedComplex.refinedCones.size; i ++
  ) {
    std::string chamberWallColor = "black";
    bool isZeroChamber = this->quasiPolynomials[i].isEqualToZero();
    if (isZeroChamber) {
      chamberWallColor = zeroColor;
    }
    this->projectivizedComplex.refinedCones[i].drawMeLastCoordinateAffine(
      false, drawingVariables, format, chamberWallColor
    );
    std::stringstream tempStream;
    tempStream << i + 1;
    Vector<Rational> root =
    this->projectivizedComplex.refinedCones[i].getInternalPoint();
    root.makeAffineUsingLastCoordinate();
    for (
      int j = 0; j < this->quasiPolynomials[i].latticeShifts.size; j ++
    ) {
      this->projectivizedComplex.refinedCones[i].getLatticePointsInCone(
        this->quasiPolynomials[i].ambientLatticeReduced,
        this->quasiPolynomials[i].latticeShifts[j],
        numberOfLatticePointsPerDimension,
        true,
        latticePoints,
        distinguishedPoint
      );
      tempList.initializeFillInObject(latticePoints.size, chamberWallColor);
      if (restrictingChamber != nullptr) {
        for (int k = 0; k < latticePoints.size; k ++) {
          root = latticePoints[k];
          root.makeAffineUsingLastCoordinate();
          if (!restrictingChamber->isInCone(root)) {
            tempList[k] = zeroColor;
          }
        }
      }
      latticePointsFinal.addListOnTop(latticePoints);
      latticePointColors.addListOnTop(tempList);
    }
  }
  for (int i = 0; i < latticePointsFinal.size; i ++) {
    drawingVariables.drawCircleAtVectorBufferRational(
      latticePointsFinal[i], latticePointColors[i], 2
    );
    drawingVariables.drawTextAtVectorBufferRational(
      latticePointsFinal[i],
      this->evaluateInputProjectivized(latticePointsFinal[i], nullptr).toString
      (),
      latticePointColors[i]
    );
  }
}

Rational QuasiPolynomial::evaluate(
  const Vector<Rational>& input, std::stringstream* comments
) {
  Vector<Rational> testLatticeBelonging;
  for (int i = 0; i < this->latticeShifts.size; i ++) {
    testLatticeBelonging = this->latticeShifts[i] - input;
    if (this->ambientLatticeReduced.isInLattice(testLatticeBelonging)) {
      if (comments != nullptr) {
        *comments
        << "Lattice shift: "
        << this->latticeShifts[i]
        << " contains: "
        << input.toString()
        << ", ambient lattice: "
        << this->ambientLatticeReduced.toString()
        << "<br>Quasipolynomial on shift: "
        << "\\("
        << this->valueOnEachLatticeShift[i].toString()
        << "\\)";
      }
      return this->valueOnEachLatticeShift[i].evaluate(input);
    }
  }
  return 0;
}

Rational PiecewiseQuasipolynomial::evaluate(
  const Vector<Rational>& input, std::stringstream* comments
) {
  if (input.size != this->projectivizedComplex.getDimension() - 1) {
    global.fatal
    << "Input size does not equal the "
    << "projectivized complex dimension minus one. "
    << global.fatal;
  }
  Vector<Rational> ProjectivizedInput = input;
  ProjectivizedInput.setSize(input.size + 1);
  *ProjectivizedInput.lastObject() = 1;
  return this->evaluateInputProjectivized(ProjectivizedInput, comments);
}

Rational PiecewiseQuasipolynomial::evaluateInputProjectivized(
  const Vector<Rational>& input, std::stringstream* comments
) {
  Rational result;
  if (input.size != this->projectivizedComplex.getDimension()) {
    global.fatal
    << "Input size not equal to projectivized complex dimension. "
    << global.fatal;
  }
  Vector<Rational> affineInput = input;
  affineInput.setSize(input.size - 1);
  int index =
  this->projectivizedComplex.getLowestIndexRefinedChamberContaining(input);
  if (index == - 1) {
    return 0;
  }
  result = this->quasiPolynomials[index].evaluate(affineInput, comments);
  // The following for loop is for self-check purposes only.
  // Comment it out as soon as
  // the code has been tested sufficiently.
  bool firstFail = true;
  for (
    int i = 0; i < this->projectivizedComplex.refinedCones.size; i ++
  ) {
    if (
      !this->projectivizedComplex.refinedCones[i].isInCone(input)
    ) {
      continue;
    }
    Rational altResult =
    this->quasiPolynomials[i].evaluate(affineInput, comments);
    if (result != altResult) {
      if ((false)) {
        if (!firstFail) {
          break;
        }
        FormatExpressions tempFormat;
        global.comments
        << "<hr>Error!!! Failed on chamber "
        << index + 1
        << " and "
        << i + 1;
        global.comments
        << "<br>Evaluating at point "
        << affineInput.toString()
        << "<br>";
        global.comments
        << "<br>Chamber "
        << index + 1
        << ": "
        << this->projectivizedComplex.refinedCones[index].toString(
          &tempFormat
        );
        global.comments
        << "<br>QP: "
        << this->quasiPolynomials[index].toString();
        global.comments << "<br>value: " << result.toString();
        global.comments
        << "<br><br>Chamber "
        << i + 1
        << ": "
        << this->projectivizedComplex.refinedCones[i].toString(&tempFormat);
        global.comments << "<br>QP: " << this->quasiPolynomials[i].toString();
        global.comments << "<br>value: " << altResult.toString();
        if (firstFail) {
          DrawingVariables tempDV;
          global.comments
          << "<br><b>Point of failure: "
          << affineInput.toString()
          << "</b>";
          // this->drawMe(tempDV);
          this->projectivizedComplex.drawMeLastCoordinateAffine(
            true, tempDV, tempFormat
          );
          tempDV.operations.drawCircleAtVectorBufferRational(
            affineInput, "black", 5
          );
          tempDV.operations.drawCircleAtVectorBufferRational(
            affineInput, "black", 10
          );
          tempDV.operations.drawCircleAtVectorBufferRational(
            affineInput, "red", 4
          );
          global.comments
          << tempDV.getHTMLDiv(
            this->projectivizedComplex.getDimension() - 1, false
          );
        }
        firstFail = false;
      }
    }
  }
  return result;
}

void PiecewiseQuasipolynomial::makeCommonRefinement(
  const ConeCollection& other
) {
  List<QuasiPolynomial> oldQuasiPolynomials = this->quasiPolynomials;
  ConeCollection oldComplex = this->projectivizedComplex;
  this->projectivizedComplex.refineMakeCommonRefinement(other);
  this->quasiPolynomials.setSize(
    this->projectivizedComplex.refinedCones.size
  );
  for (
    int i = 0; i < this->projectivizedComplex.refinedCones.size; i ++
  ) {
    int oldIndex =
    oldComplex.getLowestIndexRefinedChamberContaining(
      this->projectivizedComplex.refinedCones[i].getInternalPoint()
    );
    if (oldIndex != - 1) {
      this->quasiPolynomials[i] = oldQuasiPolynomials[oldIndex];
    } else {
      // the below needs to be fixed!!!!!
      this->quasiPolynomials[i].makeZeroLatticeZn(
        this->minimalNumberOfVariables()
      );
    }
  }
}

void Lattice::getDefaultFundamentalDomainInternalPoint(
  Vector<Rational>& output
) {
  output.makeZero(this->getDimension());
  Vector<Rational> root;
  for (int i = 0; i < this->basisRationalForm.numberOfRows; i ++) {
    this->basisRationalForm.getVectorFromRow(i, root);
    output += root;
  }
  output /= 2;
}

bool PartialFractions::split(Vector<Rational>* indicator) {
  STACK_TRACE("PartialFractions::split");
  if (!this->flagInitialized) {
    this->prepareIndicatorVariables();
    this->prepareCheckSums();
    this->flagInitialized = true;
  }
  if (this->splitPartial()) {
    this->removeRedundantShortRoots(indicator);
    this->makeProgressReportSplittingMainPart();
  }
  this->compareCheckSums();
  return false;
}

void Cone::changeBasis(Matrix<Rational>& linearMap) {
  linearMap.actOnVectorsColumn(this->normals);
  this->createFromNormals(this->normals);
}

void Cone::transformToWeylProjective(ConeCollection& owner) {
  (void) owner;
  global.fatal << "Not implemented yet. " << global.fatal;
  /*
  for (int i = 0; i < this->externalWalls.size; i ++)
    this->externalWalls[i].transformToWeylProjective();
  WallData newWall;
  this->externalWalls.reserve(owner.WeylChamber.size +this->externalWalls.size);
  Vectors<Rational> newExternalWalls;
  owner.GetWeylChamberWallsForCharacterComputation(newExternalWalls);
  for (int i = 0; i < newExternalWalls.size; i ++) {
    newWall.normal = newExternalWalls[i];
    this->externalWalls.addOnTop(newWall);
  }
  this->AllVertices.size = 0;
  this->ComputeVerticesFromNormals(owner);*/
}

void ConeCollection::transformToWeylProjective() {
  /* this->ambientWeyl.getElement().computeAllElements();
  this->log << this->ambientWeyl.getElement().toString();
  std::string tempS;
  this->toString(tempS);
  this->log << "\nWeyl chamber: " << this->WeylChamber.toString() << "\n";
  this->log << tempS;
  this->NewHyperplanesToSliceWith.size = 0;
  this->hyperplanes.size = 0;
  Vector<Rational> wallToSliceWith;
//  Vectors<Rational> oldDirections;
  for (int k = 0; k< this->ambientWeyl.getElement().size; k++)
    for (int i = 0; i < this->size; i ++)
      if (this->objects[i] != 0)
        for (int j = 0; j < this->objects[i]->externalWalls.size; j ++) {
          this->GetAffineWallImage(k, this->objects[i]->externalWalls[j], wallToSliceWith);
//          if (k == 0)
//            oldDirections.AddOnBottomNoRepetition(wallToSliceWith);
          wallToSliceWith.ScaleToIntegralMinHeightFirstNonZeroCoordinatePositive();
          if (k > 0)
            this->NewHyperplanesToSliceWith.addOnTopNoRepetition(wallToSliceWith);
          this->hyperplanes.addOnTopNoRepetition(wallToSliceWith);
        }
  this->log << "\n Affine walls to slice with:";
  for (int i = 0; i < this->NewHyperplanesToSliceWith.size; i ++)
    this->log << "\n" << this->NewHyperplanesToSliceWith[i].toString();
  this->log << "\n";
  this->AmbientDimension = this->AmbientDimension*2+ 1;
  for (int i = 0; i < this->size; i ++)
    if (this->objects[i] != 0) {
      this->objects[i]->transformToWeylProjective(*this);
//      vertices.AddRootSnoRepetition(this->objects[i]->AllVertices);
    }
  //this->startingCones.initFromDirections()
  Vector<Rational> root;
  for (int i = 0; i < this->TheGlobalConeNormals.size; i ++) {
    root.makeZero(this->AmbientDimension);
    int startingDim= this->TheGlobalConeNormals[i].size;
    for (int j = 0; j<startingDim; j ++) {
      root[j] = this->TheGlobalConeNormals[i][j];
      root[j+startingDim] = -root[j];
    }
    this->TheGlobalConeNormals[i] = root;
  }
  Vectors<Rational> roots;
  this->GetWeylChamberWallsForCharacterComputation(roots);
  this->TheGlobalConeNormals.addListOnTop(roots);
  this->log << "the global cone normals: " << this->TheGlobalConeNormals.toString();
  this->toString(tempS);
  this->log << tempS;
  global.indicatorVariables.StatusString1NeedsRefresh = true;
  global.indicatorVariables.StatusString1= this->log.str();
  global.makeReport();*/
}

void ConeLatticeAndShiftMaxComputation::initialize(
  Vector<Rational>& inequalities,
  Cone& startingCone,
  Lattice& startingLattice,
  Vector<Rational>& startingShift
) {
  ConeLatticeAndShift allCones;
  allCones.projectivizedCone = startingCone;
  allCones.lattice = startingLattice;
  allCones.shift = startingShift;
  this->numNonParaM = 0;
  this->numProcessedNonParam = 0;
  this->LPtoMaximizeLargerDim.size = 0;
  this->LPtoMaximizeSmallerDim.size = 0;
  this->startingRepresentative = startingShift;
  this->finalRepresentatives.size = 0;
  this->complexStartingPerRepresentative.size = 0;
  this->complexRefinedPerRepresentative.size = 0;
  this->conesLargerDimension.addOnTop(allCones);
  this->LPtoMaximizeLargerDim.addOnTop(inequalities);
  this->isInfinity.initializeFillInObject(1, false);
}

void Lattice::applyLinearMap(
  Matrix<Rational>& linearMap, Lattice& output
) {
  Vectors<Rational> roots;
  roots.assignMatrixRows(this->basisRationalForm);
  linearMap.actOnVectorsColumn(roots);
  output.makeFromRoots(roots);
}

std::string ConeLatticeAndShiftMaxComputation::toString(
  FormatExpressions* format
) {
  std::stringstream out;
  out
  << "<hr>Resulting lattice: "
  << this->finalRougherLattice.toString()
  << "<hr><hr>";
  /*  if (this->complexStartingPerRepresentative.size >0) {
    out << "<hr> Non-refined complex per representative:<br>\n ";
    for (int i = 0; i < this->complexStartingPerRepresentative.size; i ++) {
      out << "Lattice +shift ="  << this->finalRepresentatives[i].toString() << " + " << this->finalRougherLattice.toString();
      out << "<br>\n" << this->complexStartingPerRepresentative[i].toString(false, true);
      out << "the function we need to max: " << this->LPtoMaximizeSmallerDim[i].toString();
    }
  }*/
  out
  << "<hr><hr>Cones not processed(number of cones "
  << this->conesLargerDimension.size
  << "):\n<hr>\n";
  DrawingVariables drawingVariables;
  Polynomial<Rational> polynomialPart;
  for (int i = 0; i < this->conesLargerDimension.size; i ++) {
    out << "";
    // << this->conesLargerDim[i].toString(format);
    // out << "<br>" << this->LPtoMaximizeLargerDim[i].toString();
    drawingVariables.operations.initialize();
    out
    << "<br>"
    << this->conesLargerDimension[i].projectivizedCone.
    drawMeToHtmlLastCoordAffine(drawingVariables, *format);
    out
    << "<br>over "
    << this->conesLargerDimension[i].shift.toString()
    << " + "
    << this->conesLargerDimension[i].lattice.toString();
    polynomialPart.makeLinearWithConstantTerm(
      this->LPtoMaximizeLargerDim[i]
    );
    out
    << "<br>the function we have maxed, as "
    << "a function of the remaining variables, is: "
    << polynomialPart.toString(format)
    << "<hr><hr>";
  }
  if (this->conesSmallerDimension.size > 0) {
    out << "<br>Cones processed: <br>";
    for (int i = 0; i < this->conesSmallerDimension.size; i ++) {
      out << this->conesSmallerDimension[i].toString(*format);
      // out << "<br>" << this->LPtoMaximizeSmallerDim[i].toString();
      drawingVariables.operations.initialize();
      out
      << this->conesSmallerDimension[i].projectivizedCone.
      drawMeToHtmlLastCoordAffine(drawingVariables, *format);
    }
  }
  return out.str();
}

void ConeLatticeAndShiftMaxComputation::findExtremaParametricStep3() {
  this->finalRougherLattice = this->conesLargerDimension[0].lattice;
  ProgressReport report;
  ProgressReport report2;
  for (int i = 1; i < this->conesLargerDimension.size; i ++) {
    this->finalRougherLattice.intersectWith(
      this->conesLargerDimension[i].lattice
    );
    std::stringstream tempStream;
    tempStream
    << "intersecing lattice "
    << i + 1
    << " out of "
    << this->conesLargerDimension.size;
    report.report(tempStream.str());
  }
  this->finalRepresentatives.size = 0;
  Vectors<Rational> roots;
  Vectors<Rational> roots2;
  roots2.setSize(1);
  for (int i = 0; i < this->conesLargerDimension.size; i ++) {
    roots2[0] = this->conesLargerDimension[i].shift;
    this->conesLargerDimension[i].lattice.getAllRepresentativesProjectingDownTo
    (this->finalRougherLattice, roots2, roots);
    this->finalRepresentatives.addOnTopNoRepetition(roots);
    std::stringstream tempStream;
    tempStream
    << "Computing representative "
    << i + 1
    << " out of "
    << this->conesLargerDimension.size;
    tempStream << "\nSo far " << this->finalRepresentatives.size << " found.";
    report2.report(tempStream.str());
  }
  this->complexStartingPerRepresentative.setSize(
    this->finalRepresentatives.size
  );
  this->startingLPtoMaximize.setSize(this->finalRepresentatives.size);
  this->finalMaxima.setSize(this->finalRepresentatives.size);
  Vector<Rational> root;
  for (int i = 0; i < this->finalRepresentatives.size; i ++) {
    for (int j = 0; j < this->conesLargerDimension.size; j ++) {
      root = this->finalRepresentatives[i];
      this->conesLargerDimension[j].lattice.reduceVector(root);
      if (root == this->conesLargerDimension[j].shift) {
        this->complexStartingPerRepresentative[i].refinedCones.addOnTop(
          this->conesLargerDimension[j].projectivizedCone
        );
        this->startingLPtoMaximize[i].addOnTop(
          this->LPtoMaximizeLargerDim[j]
        );
      }
    }
  }
}

/*void ConeLatticeAndShiftMaxComputation::findExtremaParametricStep2TrimChamberForMultOne
    () {
  Cone trimmedCone;
  Vectors<Rational> roots;
  Vector<Rational> multFreeWall;
  int startingNumCones = this->conesLargerDim.size;
  int numKilledCones = 0;
  for (int i = 0; i < this->conesLargerDim.size; i ++) {
    trimmedCone.normals = this->conesLargerDim[i].projectivizedCone.normals;
    multFreeWall = this->LPtoMaximizeLargerDim[i];
    multFreeWall.negate();
    *multFreeWall.lastObject() +=1;
    trimmedCone.normals.addOnTop(multFreeWall);
    trimmedCone.createFromNormals(trimmedCone.normals);
    if (!trimmedCone.flagIsTheZeroCone)
      this->conesLargerDim[i].projectivizedCone = trimmedCone;
    else {
      this->conesLargerDim.removeIndexSwapWithLast(i);
      this->LPtoMaximizeLargerDim.removeIndexSwapWithLast(i);
      i --;
      numKilledCones++;
    }
    std::stringstream tempStream;
    tempStream << "Processed " << i +numKilledCones << " out of " << startingNumCones;
    tempStream << "\nKilled " << numKilledCones << " cones so far";
    global.indicatorVariables.ProgressReportStrings[2] = tempStream.str();
    global.makeReport();
  }
}
*/
void ConeLatticeAndShiftMaxComputation::findExtremaParametricStep4() {
  this->complexRefinedPerRepresentative.setSize(
    this->finalRepresentatives.size
  );
  this->maximaCandidates.setSize(this->finalRepresentatives.size);
  ProgressReport report;
  for (int i = 0; i < this->finalRepresentatives.size; i ++) {
    ConeCollection& currentComplex = this->complexRefinedPerRepresentative[i];
    currentComplex.initFromConeWalls(
      this->complexStartingPerRepresentative[i].refinedCones, true
    );
    std::stringstream tempStream;
    tempStream
    << "Processing representative "
    << i + 1
    << " out of "
    << this->finalRepresentatives.size;
    report.report(tempStream.str());
    currentComplex.refine();
    this->maximaCandidates[i].setSize(currentComplex.refinedCones.size);
    for (int j = 0; j < currentComplex.refinedCones.size; j ++) {
      for (
        int k = 0; k < this->complexStartingPerRepresentative[k].refinedCones.
        size; k ++
      ) {
        if (
          this->complexStartingPerRepresentative[i].refinedCones[k].isInCone(
            currentComplex.refinedCones[j].getInternalPoint()
          )
        ) {
          this->maximaCandidates[i][j].addOnTopNoRepetition(
            this->startingLPtoMaximize[i][k]
          );
        }
      }
    }
  }
}

void ConeLatticeAndShiftMaxComputation::findExtremaParametricStep5() {
  this->finalMaximaChambers.setSize(this->finalRepresentatives.size);
  this->finalMaximaChambersIndicesMaxFunctions.setSize(
    this->finalRepresentatives.size
  );
  for (int i = 0; i < 1; i ++) {
    this->finalMaximaChambers[i].setSize(
      this->complexRefinedPerRepresentative[i].refinedCones.size
    );
    this->finalRepresentatives[i].setSize(
      this->complexRefinedPerRepresentative[i].refinedCones.size
    );
    for (int j = 0; j < 1; j ++) {
      const Cone& currentCone =
      this->complexRefinedPerRepresentative[i].refinedCones[j];
      this->finalMaximaChambers[i][j].initialize();
      this->finalMaximaChambers[i][j].findMaxLFOverConeProjective(
        currentCone,
        this->maximaCandidates[i][j],
        this->finalMaximaChambersIndicesMaxFunctions[i][j]
      );
    }
  }
}

void ConeLatticeAndShiftMaxComputation::findExtremaParametricStep1() {
  FormatExpressions tempFormat;
  ProgressReport report1;
  ProgressReport report2;
  ProgressReport report3;
  for (
    ; this->numProcessedNonParam < this->numNonParaM; this->
    numProcessedNonParam ++
  ) {
    while (this->conesLargerDimension.size > 0) {
      ConeLatticeAndShift& currentCLS =
      *this->conesLargerDimension.lastObject();
      if (
        this->LPtoMaximizeLargerDim.lastObject()->size !=
        currentCLS.getDimensionAffine() + 1
      ) {
        global.fatal
        << "In ConeLatticeAndShiftMaxComputation::findExtremaParametricStep1: "
        << "dimensions don't match. "
        << global.fatal;
      }
      if (!this->LPtoMaximizeLargerDim.lastObject()->isEqualToZero()) {
        currentCLS.findExtremaInDirectionOverLatticeOneNonParametric(
          *this->LPtoMaximizeLargerDim.lastObject(),
          this->LPtoMaximizeSmallerDim,
          this->conesSmallerDimension
        );
      }
      this->conesLargerDimension.size --;
      this->LPtoMaximizeLargerDim.size --;
      std::stringstream tempStream1, tempStream2, tempStream3;
      tempStream1
      << "Processing "
      << this->numProcessedNonParam + 1
      << " out of "
      << this->numNonParaM;
      tempStream2 << "Remaining cones: " << this->conesLargerDimension.size;
      tempStream3
      << "Cones smaller dim total: "
      << this->conesSmallerDimension.size;
      report1.report(tempStream1.str());
      report2.report(tempStream2.str());
      report3.report(tempStream3.str());
    }
    this->LPtoMaximizeLargerDim = this->LPtoMaximizeSmallerDim;
    this->conesLargerDimension = this->conesSmallerDimension;
    this->conesSmallerDimension.size = 0;
    this->LPtoMaximizeSmallerDim.size = 0;
  }
}

void ConeLatticeAndShift::
findExtremaInDirectionOverLatticeOneNonParamDegenerateCase(
  Vector<Rational>& lpToMaximizeAffine,
  Vectors<Rational>& outputAppendLPToMaximizeAffine,
  List<ConeLatticeAndShift>& outputAppend,
  Matrix<Rational>& projectionLatticeLevel
) {
  Matrix<Rational> matVertices;
  matVertices.assignVectorsToRows(this->projectivizedCone.vertices);
  Vectors<Rational> normals;
  matVertices.getZeroEigenSpaceModifyMe(normals);
  Vector<Rational> preferredNormal;
  for (int i = 0; i < normals.size; i ++) {
    if (!normals[i][0].isEqualToZero()) {
      preferredNormal = normals[i];
      break;
    }
  }
  Rational firstCoord = preferredNormal[0];
  preferredNormal.shiftToTheLeftOnePosition();
  preferredNormal /= - firstCoord;
  ConeLatticeAndShift tempCLS;
  Vectors<Rational> newNormals = this->projectivizedCone.normals;
  Rational firstCoordNewNormal;
  for (int i = 0; i < newNormals.size; i ++) {
    firstCoordNewNormal = newNormals[i][0];
    newNormals[i].shiftToTheLeftOnePosition();
    newNormals[i] += preferredNormal * firstCoordNewNormal;
  }
  tempCLS.projectivizedCone.createFromNormals(newNormals);
  tempCLS.shift = this->shift;
  tempCLS.shift.shiftToTheLeftOnePosition();
  this->lattice.applyLinearMap(projectionLatticeLevel, tempCLS.lattice);
  Vector<Rational> root;
  root = lpToMaximizeAffine.getshiftToTheLeftOnePositionition();
  root += preferredNormal * lpToMaximizeAffine[0];
  if (!tempCLS.projectivizedCone.flagIsTheZeroCone) {
    outputAppend.addOnTop(tempCLS);
    outputAppendLPToMaximizeAffine.addOnTop(root);
  }
}

void ConeLatticeAndShift::operator=(const ConeLatticeAndShift& other) {
  this->projectivizedCone = other.projectivizedCone;
  this->lattice = other.lattice;
  this->shift = other.shift;
}

int ConeLatticeAndShift::getDimensionProjectivized() {
  return this->projectivizedCone.getDimension();
}

int ConeLatticeAndShift::getDimensionAffine() {
  return this->projectivizedCone.getDimension() - 1;
}

void ConeLatticeAndShift::findExtremaInDirectionOverLatticeOneNonParametric(
  Vector<Rational>& lpToMaximizeAffine,
  Vectors<Rational>& outputAppendLPToMaximizeAffine,
  List<ConeLatticeAndShift>& outputAppend
) {
  Vector<Rational> direction;
  FormatExpressions format;
  int dimensionProjectivized = this->getDimensionProjectivized();
  Matrix<Rational> projectionLatticeLevel;
  projectionLatticeLevel.initialize(
    dimensionProjectivized - 2, dimensionProjectivized - 1
  );
  projectionLatticeLevel.makeZero();
  for (int i = 0; i < projectionLatticeLevel.numberOfRows; i ++) {
    projectionLatticeLevel.elements[i][i + 1] = 1;
  }
  direction.makeEi(dimensionProjectivized, 0);
  if (
    !this->projectivizedCone.vertices.linearSpanContainsVector(direction)
  ) {
    this->findExtremaInDirectionOverLatticeOneNonParamDegenerateCase(
      lpToMaximizeAffine,
      outputAppendLPToMaximizeAffine,
      outputAppend,
      projectionLatticeLevel
    );
    return;
  }
  ProgressReport report;
  if (outputAppend.size >= 10) {
    std::stringstream tempStream;
    tempStream
    << "<hr><hr><hr><hr>The bad cone:"
    << this->projectivizedCone.toString(&format);
    report.report(tempStream.str());
  }
  ConeCollection complexBeforeProjection;
  complexBeforeProjection.initialize();
  complexBeforeProjection.refinedCones.addOnTop(this->projectivizedCone);
  complexBeforeProjection.convexHull.makeConvexHullOfMeAnd(
    this->projectivizedCone
  );
  if (direction.scalarEuclidean(lpToMaximizeAffine).isNegative()) {
    direction.negate();
  }
  complexBeforeProjection.slicingDirections.addOnTop(direction);
  complexBeforeProjection.slicingDirections.addOnTop(- direction);
  complexBeforeProjection.refine();
  Vector<Rational>
  root,
  extraEquation,
  exitNormalAffine,
  enteringNormalAffine;
  Vector<Rational> exitNormalLatticeLevel, exitNormalShiftedAffineProjected;
  Vector<Rational> directionSmallerDim, directionSmallerDimOnLattice;
  Vector<Rational> shiftReduced = this->shift;
  this->lattice.reduceVector(shiftReduced);
  Vectors<Rational> exitRepresentatives, exitWallsShifted;
  Lattice exitRougherLattice;
  ConeLatticeAndShift tempCLS;
  directionSmallerDim.makeEi(dimensionProjectivized - 1, 0);
  Vectors<Rational> newNormals;
  for (int i = 0; i < complexBeforeProjection.refinedCones.size; i ++) {
    const Cone& currentCone = complexBeforeProjection.refinedCones[i];
    int numberOfNonPerpendicularWalls = 0;
    newNormals.size = 0;
    bool foundEnteringNormal = false;
    bool foundExitNormal = false;
    for (int j = 0; j < currentCone.normals.size; j ++) {
      Vector<Rational>& currentNormal = currentCone.normals[j];
      if (currentNormal[0].isEqualToZero()) {
        root = currentNormal.getshiftToTheLeftOnePositionition();
        newNormals.addOnTop(root);
      } else {
        numberOfNonPerpendicularWalls ++;
        if (numberOfNonPerpendicularWalls >= 3) {
          global.fatal
          << "Number of non-perpendicular walls is larger than 3. "
          << global.fatal;
        }
        if (
          !currentNormal.scalarEuclidean(direction).isPositive() &&
          !foundExitNormal
        ) {
          lattice.getRougherLatticeFromAffineHyperplaneDirectionAndLattice(
            directionSmallerDim,
            directionSmallerDimOnLattice,
            shift,
            currentNormal,
            exitRepresentatives,
            exitWallsShifted,
            exitRougherLattice
          );
          exitNormalAffine = currentNormal;
          exitNormalLatticeLevel = exitNormalAffine;
          exitNormalLatticeLevel.setSize(dimensionProjectivized - 1);
          foundExitNormal = true;
        } else {
          enteringNormalAffine = currentNormal;
          foundEnteringNormal = true;
        }
      }
    }
    exitRougherLattice.applyLinearMap(
      projectionLatticeLevel, tempCLS.lattice
    );
    for (int j = 0; j < exitRepresentatives.size; j ++) {
      exitRepresentatives[j] += shiftReduced;
      Lattice::
      getClosestPointInDirectionOfTheNormalToAffineWallMovingIntegralStepsInDirection
      (
        exitRepresentatives[j],
        exitNormalAffine,
        directionSmallerDimOnLattice,
        exitRepresentatives[j]
      );
    }
    global.comments << "<hr><hr><hr>" << currentCone.toString(&format);
    global.comments
    << "<br>Entering normal: "
    << ((foundEnteringNormal) ? enteringNormalAffine.toString() : "not found"
    );
    global.comments
    << "<br>Exit normal: "
    << ((foundExitNormal) ? exitNormalAffine.toString() : "not found");
    global.comments
    << "<br>The shifted lattice representatives: "
    << exitRepresentatives.toString()
    << "<br>exitNormalsShiftedAffineProjected";
    if (newNormals.size <= 0) {
      global.fatal << "New normals missing. " << global.fatal;
    }
    for (int j = 0; j < exitRepresentatives.size; j ++) {
      tempCLS.projectivizedCone.normals = newNormals;
      exitNormalShiftedAffineProjected =
      exitNormalAffine.getshiftToTheLeftOnePositionition();
      *exitNormalShiftedAffineProjected.lastObject() =
      - exitNormalLatticeLevel.scalarEuclidean(exitRepresentatives[j]);
      global.comments << exitNormalShiftedAffineProjected.toString() << ", ";
      if (foundEnteringNormal) {
        extraEquation =
        enteringNormalAffine.getshiftToTheLeftOnePositionition();
        extraEquation -= (
          exitNormalShiftedAffineProjected * enteringNormalAffine[0]
        ) /
        exitNormalAffine[0];
        global.comments
        << "extra equation: "
        << extraEquation.toString()
        << ", ";
        tempCLS.projectivizedCone.normals.addOnTop(extraEquation);
      }
      root = lpToMaximizeAffine.getshiftToTheLeftOnePositionition();
      root -= exitNormalShiftedAffineProjected * lpToMaximizeAffine[0] /
      exitNormalAffine[0];
      outputAppendLPToMaximizeAffine.addOnTop(root);
      if (tempCLS.projectivizedCone.normals.size <= 0) {
        global.fatal << "Projectivized cone has no normals. " << global.fatal;
      }
      Vectors<Rational> roots = tempCLS.projectivizedCone.normals;
      tempCLS.projectivizedCone.createFromNormals(roots);
      if (!tempCLS.projectivizedCone.flagIsTheZeroCone) {
        projectionLatticeLevel.actOnVectorColumn(
          exitRepresentatives[j], tempCLS.shift
        );
        outputAppend.addOnTop(tempCLS);
        if (tempCLS.getDimensionProjectivized() == 0) {
          report.report(roots.toString());
          while (true) {}
        }
        if (
          tempCLS.getDimensionProjectivized() != dimensionProjectivized - 1
        ) {
          global.fatal
          << "Projectivized dimension not correct. "
          << global.fatal;
        }
      }
    }
  }
}

void ConeCollection::getNewVerticesAppend(
  const Cone& toBeSplit,
  const Vector<Rational>& normalSlicingPlane,
  HashedList<Vector<Rational> >& outputVertices
) {
  int dimensionMinusTwo = normalSlicingPlane.size - 2;
  int dimension = normalSlicingPlane.size;
  LargeInteger numberOfCycles =
  MathRoutines::nChooseK(toBeSplit.normals.size, dimensionMinusTwo);
  Selection selection;
  Selection nonPivotPoints;
  selection.initialize(toBeSplit.normals.size);
  Matrix<Rational> toBeEliminated;
  toBeEliminated.initialize(dimensionMinusTwo + 1, dimension);
  Vector<Rational> root;
  for (int i = 0; i < numberOfCycles; i ++) {
    selection.incrementSelectionFixedCardinality(dimensionMinusTwo);
    // , indexLastZeroWithOneBefore, NumOnesAfterLastZeroWithOneBefore);
    for (int j = 0; j < dimensionMinusTwo; j ++) {
      Vector<Rational>& currentNormal =
      toBeSplit.normals[selection.elements[j]];
      for (int k = 0; k < dimension; k ++) {
        toBeEliminated.elements[j][k] = currentNormal[k];
      }
    }
    for (int k = 0; k < dimension; k ++) {
      toBeEliminated.elements[dimensionMinusTwo][k] = normalSlicingPlane[k];
    }
    toBeEliminated.gaussianEliminationByRows(nullptr, &nonPivotPoints);
    if (nonPivotPoints.cardinalitySelection == 1) {
      toBeEliminated.nonPivotPointsToEigenVectorHomogeneous(
        nonPivotPoints, root
      );
      Cone::scaleNormalizeByPositive(root);
      if (toBeSplit.isInCone(root)) {
        outputVertices.addOnTopNoRepetition(root);
      } else {
        root.negate();
        if (toBeSplit.isInCone(root)) {
          outputVertices.addOnTopNoRepetition(root);
        }
      }
    }
  }
}

bool ConeCollection::splitChamber(
  const Cone& toBeSliced,
  bool weAreChopping,
  const Vector<Rational>& killerNormal,
  List<Cone>& output
) {
  STACK_TRACE("ConeCollection::splitChamber");
  Cone newPlusCone;
  Cone newMinusCone;
  bool needToRecomputeVertices = (
    toBeSliced.normals.getRankElementSpan() < this->getDimension()
  );
  newPlusCone.lowestIndexNotCheckedForSlicingInDirection =
  toBeSliced.lowestIndexNotCheckedForSlicingInDirection;
  newMinusCone.lowestIndexNotCheckedForSlicingInDirection =
  toBeSliced.lowestIndexNotCheckedForSlicingInDirection;
  newPlusCone.lowestIndexNotCheckedForChopping =
  toBeSliced.lowestIndexNotCheckedForChopping;
  newMinusCone.lowestIndexNotCheckedForChopping =
  toBeSliced.lowestIndexNotCheckedForChopping;
  newPlusCone.flagIsTheZeroCone = false;
  newMinusCone.flagIsTheZeroCone = false;
  if (weAreChopping) {
    newPlusCone.lowestIndexNotCheckedForChopping ++;
    newMinusCone.lowestIndexNotCheckedForChopping ++;
  }
  newPlusCone.vertices.size = 0;
  newPlusCone.normals.size = 0;
  newMinusCone.vertices.size = 0;
  newMinusCone.normals.size = 0;
  HashedList<Vector<Rational> > zeroVertices;
  Rational scalarProduct;
  for (int i = 0; i < toBeSliced.vertices.size; i ++) {
    killerNormal.scalarEuclidean(toBeSliced.vertices[i], scalarProduct);
    if (scalarProduct.isPositive()) {
      newPlusCone.vertices.addOnTop(toBeSliced.vertices[i]);
    }
    if (scalarProduct.isEqualToZero()) {
      zeroVertices.addOnTopNoRepetition(toBeSliced.vertices[i]);
    }
    if (scalarProduct.isNegative()) {
      newMinusCone.vertices.addOnTop(toBeSliced.vertices[i]);
    }
  }
  if (newPlusCone.vertices.size == 0 || newMinusCone.vertices.size == 0) {
    return false;
  }
  this->getNewVerticesAppend(toBeSliced, killerNormal, zeroVertices);
  for (int i = 0; i < toBeSliced.normals.size; i ++) {
    if (
      newPlusCone.vertices.hasAnElementPerpendicularTo(
        toBeSliced.normals[i]
      )
    ) {
      newPlusCone.normals.addOnTop(toBeSliced.normals[i]);
    }
    if (
      newMinusCone.vertices.hasAnElementPerpendicularTo(
        toBeSliced.normals[i]
      )
    ) {
      newMinusCone.normals.addOnTop(toBeSliced.normals[i]);
    }
  }
  newPlusCone.normals.addOnTop(killerNormal);
  newMinusCone.normals.addOnTop(- killerNormal);
  newPlusCone.vertices.addListOnTop(zeroVertices);
  newMinusCone.vertices.addListOnTop(zeroVertices);
  if (needToRecomputeVertices) {
    newPlusCone.createFromNormals(newPlusCone.normals);
    newMinusCone.createFromNormals(newMinusCone.normals);
  }
  output.addOnTop(newPlusCone);
  output.addOnTop(newMinusCone);
  return true;
}

bool Cone::makeConvexHullOfMeAnd(const Cone& other) {
  if (this->isInCone(other.vertices)) {
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
  newVertices.addListOnTop(other.vertices);
  newVertices.addListOnTop(this->vertices);
  this->createFromVertices(newVertices);
  return true;
}

void ConeCollection::addNonRefinedChamberOnTopNoRepetition(
  const Cone& newCone, List<Cone>& output
) {
  Cone coneSorted;
  coneSorted = newCone;
  coneSorted.normals.quickSortAscending();
  this->convexHull.makeConvexHullOfMeAnd(coneSorted);
  output.addOnTop(newCone);
}

bool ConeCollection::refineOneStep(Cone& toBeRefined, List<Cone>& output) {
  STACK_TRACE("ConeCollection::refineOneStep");
  if (this->refineByNormals(toBeRefined, output)) {
    return true;
  }
  if (this->refineByDirections(toBeRefined, output)) {
    return true;
  }
  return false;
}

bool ConeCollection::refineByNormals(
  Cone& toBeRefined, List<Cone>& output
) {
  STACK_TRACE("ConeCollection::refineByNormals");
  output.clear();
  for (
    ; toBeRefined.lowestIndexNotCheckedForChopping < this->splittingNormals.
    size; toBeRefined.lowestIndexNotCheckedForChopping ++
  ) {
    if (
      this->splitChamber(
        toBeRefined,
        true,
        this->splittingNormals[toBeRefined.lowestIndexNotCheckedForChopping],
        output
      )
    ) {
      return true;
    }
  }
  return false;
}

bool ConeCollection::refineByDirections(
  Cone& toBeRefined, List<Cone>& output
) {
  STACK_TRACE("ConeCollection::refineByDirections");
  output.clear();
  Vector<Rational> currentNewWall;
  for (
    ; toBeRefined.lowestIndexNotCheckedForSlicingInDirection < this->
    slicingDirections.size; toBeRefined.
    lowestIndexNotCheckedForSlicingInDirection ++
  ) {
    const Vector<Rational>& firstSlicingDirection =
    this->slicingDirections[
      toBeRefined.lowestIndexNotCheckedForSlicingInDirection
    ];
    for (int i = 0; i < toBeRefined.normals.size; i ++) {
      if (
        firstSlicingDirection.scalarEuclidean(toBeRefined.normals[i]).
        isPositive()
      ) {
        for (int j = i + 1; j < toBeRefined.normals.size; j ++) {
          if (
            firstSlicingDirection.scalarEuclidean(toBeRefined.normals[j]).
            isPositive()
          ) {
            if (
              toBeRefined.produceNormalFromTwoNormalsAndSlicingDirection(
                firstSlicingDirection,
                toBeRefined.normals[i],
                toBeRefined.normals[j],
                currentNewWall
              )
            ) {
              if (
                this->splitChamber(
                  toBeRefined, false, currentNewWall, output
                )
              ) {
                return true;
              }
            }
          }
        }
      }
    }
  }
  return false;
}

void ConeCollection::initializeFromDirectionsAndRefine(
  Vectors<Rational>& inputVectors
) {
  STACK_TRACE("ConeCollection::initializeFromDirectionsAndRefine");
  this->initialize();
  Cone startingCone;
  startingCone.createFromVertices(inputVectors);
  this->nonRefinedCones.addOnTop(startingCone);
  this->convexHull.makeConvexHullOfMeAnd(startingCone);
  this->slicingDirections.addListOnTop(inputVectors);
  this->refine();
}

void ConeCollection::sort() {
  this->refinedCones.quickSortAscending();
}

void ConeCollection::refineAndSort() {
  this->refine();
  this->sort();
}

void ConeCollection::refine() {
  STACK_TRACE("ConeCollection::refine");
  ProgressReport report;
  List<Cone> incoming;
  while (this->nonRefinedCones.size > 0) {
    Cone toBeReduced = this->nonRefinedCones.popLastObject();
    bool isAlreadyRefined = !this->refineOneStep(toBeReduced, incoming);
    if (isAlreadyRefined) {
      this->refinedCones.addOnTop(toBeReduced);
    } else {
      this->nonRefinedCones.addListOnTop(incoming);
    }
    std::stringstream out;
    out
    << "Refined "
    << this->refinedCones.size
    << " cones; remaining non-refined: "
    << this->nonRefinedCones.size;
    report.report(out.str());
  }
}

void Cone::computeVerticesFromNormalsNoFakeVertices() {
  this->vertices.size = 0;
  Selection selection;
  Selection nonPivotPoints;
  for (int i = 0; i < this->normals.size; i ++) {
    Cone::scaleNormalizeByPositive(this->normals[i]);
  }
  int dimension = this->normals[0].size;
  selection.initialize(this->normals.size);
  LargeInteger numCycles =
  selection.getNumberOfCombinationsFixedCardinality(dimension - 1);
  if (dimension == 1) {
    numCycles = 0;
    bool foundNegative = false;
    bool foundPositive = false;
    for (int i = 0; i < this->normals.size; i ++) {
      if (this->normals[i].isPositiveOrZero()) {
        foundPositive = true;
      }
      if (this->normals[i].isNegativeOrZero()) {
        foundNegative = true;
      }
    }
    if (foundNegative xor foundPositive) {
      this->vertices.setSizeMakeMatrix(1, 1);
      if (foundNegative) {
        this->vertices[0][0] = - 1;
      } else {
        this->vertices[0][0] = 1;
      }
    }
    return;
  }
  Matrix<Rational> matrix;
  Vector<Rational> root;
  matrix.initialize(dimension - 1, dimension);
  for (int i = 0; i < numCycles; i ++) {
    selection.incrementSelectionFixedCardinality(dimension - 1);
    for (int j = 0; j < selection.cardinalitySelection; j ++) {
      for (int k = 0; k < dimension; k ++) {
        matrix.elements[j][k] = this->normals[selection.elements[j]][k];
      }
    }
    matrix.gaussianEliminationByRows(nullptr, &nonPivotPoints);
    if (nonPivotPoints.cardinalitySelection == 1) {
      matrix.nonPivotPointsToEigenVectorHomogeneous(nonPivotPoints, root);
      bool tempBool = this->isInCone(root);
      if (!tempBool) {
        root.negate();
        tempBool = this->isInCone(root);
      }
      if (tempBool) {
        Cone::scaleNormalizeByPositive(root);
        this->vertices.addOnTopNoRepetition(root);
      }
    }
  }
}

bool Cone::eliminateFakeNormalsUsingVertices(int numAddedFakeWalls) {
  if (this->vertices.size == 0) {
    this->flagIsTheZeroCone = true;
    this->normals.setSize(0);
    return false;
  }
  Vectors<Rational> verticesOnWall;
  if (numAddedFakeWalls != 0) {
    // we modify the normals so that they lie in the subspace spanned by the
    // vertices
    Matrix<Rational> matrix,
    matNormals,
    gramMatrixInverted;
    matrix.assignVectorsToRows(this->vertices);
    Vectors<Rational> NormalsToSubspace;
    matrix.getZeroEigenSpaceModifyMe(NormalsToSubspace);
    if (NormalsToSubspace.size > 0) {
      matNormals.assignVectorsToRows(NormalsToSubspace);
      // global.Comments << "<br>normals to the subspace spanned by the
      // vertices: " << NormalsToSubspace.toString();
      gramMatrixInverted = matNormals;
      gramMatrixInverted.transpose();
      gramMatrixInverted.multiplyOnTheLeft(matNormals);
      gramMatrixInverted.invert();
      Vector<Rational> root;
      for (int i = 0; i < this->normals.size; i ++) {
        matNormals.actOnVectorColumn(this->normals[i], root);
        gramMatrixInverted.actOnVectorColumn(root);
        for (int j = 0; j < root.size; j ++) {
          this->normals[i] -= NormalsToSubspace[j] * root[j];
        }
        Cone::scaleNormalizeByPositive(this->normals[i]);
        if (this->normals[i].isEqualToZero()) {
          this->normals.removeIndexSwapWithLast(i);
          i --;
        }
      }
      // All normals should now lie in the subspace spanned by the vertices
      // add the walls needed to go down to the subspace.
      this->normals.reserve(
        this->normals.size + 2 * NormalsToSubspace.size
      );
      for (int i = 0; i < NormalsToSubspace.size; i ++) {
        Cone::scaleNormalizeByPositive(NormalsToSubspace[i]);
        this->normals.addOnTop(NormalsToSubspace[i]);
        this->normals.addOnTop(- NormalsToSubspace[i]);
      }
    }
  }
  Matrix<Rational> matrixX;
  Selection tempSelX;
  int DesiredRank = this->vertices.getRankElementSpan(&matrixX, &tempSelX);
  if (DesiredRank > 1) {
    for (int i = 0; i < this->normals.size; i ++) {
      Vector<Rational>& currentNormal = this->normals[i];
      verticesOnWall.size = 0;
      bool wallIsGood = false;
      for (int j = 0; j < this->vertices.size; j ++) {
        if (
          currentNormal.scalarEuclidean(this->vertices[j]).isEqualToZero()
        ) {
          verticesOnWall.addOnTop(this->vertices[j]);
          int rank = verticesOnWall.getRankElementSpan(&matrixX, &tempSelX);
          if (rank < verticesOnWall.size) {
            verticesOnWall.removeLastObject();
          } else {
            if (rank == DesiredRank - 1) {
              wallIsGood = true;
              break;
            }
          }
        }
      }
      if (!wallIsGood) {
        this->normals.removeIndexSwapWithLast(i);
        i --;
      }
    }
  }
  // Eliminate identical normals.
  this->normals.quickSortAscending();
  int currentUniqueElementIndex = 0;
  for (int i = 1; i < this->normals.size; i ++) {
    if (this->normals[currentUniqueElementIndex] != this->normals[i]) {
      currentUniqueElementIndex ++;
      this->normals.swapTwoIndices(currentUniqueElementIndex, i);
    }
  }
  if (this->normals.size > 0) {
    this->normals.setSize(currentUniqueElementIndex + 1);
  }
  for (int i = 0; i < this->vertices.size; i ++) {
    if (
      this->normals.hasAnElementWithNegativeScalarProduct(
        this->vertices[i]
      )
    ) {
      global.fatal << "Negative scalar products not allowed. " << global.fatal;
    }
  }
  for (int i = 0; i < this->normals.size; i ++) {
    if (
      !this->vertices.hasAnElementWithPositiveScalarProduct(
        this->normals[i]
      )
    ) {
      return false;
    }
  }
  return numAddedFakeWalls == 0;
}

bool Cone::produceNormalFromTwoNormalsAndSlicingDirection(
  const Vector<Rational>& slicingDirection,
  Vector<Rational>& normal1,
  Vector<Rational>& normal2,
  Vector<Rational>& output
) {
  // We are looking for a normal n of the form n = t1 * normal1 + t2 * normal2
  // such that <t1 * normal1 + t2 * normal2, slicingDirection> = 0
  Rational normal1ScalarDirection =
  normal1.scalarEuclidean(slicingDirection);
  if (normal1ScalarDirection.isEqualToZero()) {
    output = normal1;
    return false;
  }
  // from now on we assume t2=1;
  Rational t1 = - normal2.scalarEuclidean(slicingDirection) /
  normal1ScalarDirection;
  output = normal2;
  output += normal1 * t1;
  Cone::scaleNormalizeByPositive(output);
  return true;
}

void Cone::scaleNormalizeByPositive(Vector<Rational>& toScale) {
  int firstIndex = toScale.getIndexFirstNonZeroCoordinate();
  if (firstIndex < 0) {
    return;
  }
  Rational scale = Rational::scaleNormalizeIndex(toScale, firstIndex);
  if (scale < 0) {
    toScale *= - 1;
  }
}

Vector<Rational> Cone::getInternalPoint() const {
  Vector<Rational> result;
  this->getInternalPoint(result);
  return result;
}

bool Cone::getInternalPoint(Vector<Rational>& output) const {
  if (this->vertices.size <= 0) {
    return false;
  }
  this->vertices.sum(output, this->vertices[0].size);
  return true;
}

bool Cone::createFromVertices(const Vectors<Rational>& inputVertices) {
  this->lowestIndexNotCheckedForChopping = 0;
  this->lowestIndexNotCheckedForSlicingInDirection = 0;
  this->flagIsTheZeroCone = false;
  if (inputVertices.size <= 0) {
    this->normals.size = 0;
    this->vertices.size = 0;
    this->flagIsTheZeroCone = true;
    return false;
  }
  this->normals.size = 0;
  Matrix<Rational> matrix;
  Selection wallGeneratorSelection;
  int rankVerticesSpan =
  inputVertices.getRankElementSpan(&matrix, &wallGeneratorSelection);
  int dimension = inputVertices.getDimension();
  Vectors<Rational> extraVertices;
  extraVertices.setSize(0);
  if (rankVerticesSpan < dimension) {
    matrix.assignVectorsToRows(inputVertices);
    matrix.getZeroEigenSpace(extraVertices);
    for (int i = 0; i < extraVertices.size; i ++) {
      this->normals.addOnTop(extraVertices[i]);
      this->normals.addOnTop(- extraVertices[i]);
    }
  }
  LargeInteger numberOfCandidates =
  MathRoutines::nChooseK(inputVertices.size, rankVerticesSpan - 1);
  Selection selection;
  selection.initialize(inputVertices.size);
  Vector<Rational> normalCandidate;
  for (int i = 0; i < numberOfCandidates; i ++) {
    selection.incrementSelectionFixedCardinality(rankVerticesSpan - 1);
    for (int j = 0; j < selection.cardinalitySelection; j ++) {
      extraVertices.addOnTop(inputVertices[selection.elements[j]]);
    }
    if (extraVertices.computeNormal(normalCandidate, dimension)) {
      bool hasPositive = false;
      bool hasNegative = false;
      for (int j = 0; j < inputVertices.size; j ++) {
        Rational scalarProduct =
        normalCandidate.scalarEuclidean(inputVertices[j]);
        if (scalarProduct.isNegative()) {
          hasNegative = true;
        }
        if (scalarProduct.isPositive()) {
          hasPositive = true;
        }
        if (hasNegative && hasPositive) {
          break;
        }
      }
      Cone::scaleNormalizeByPositive(normalCandidate);
      if ((hasNegative && !hasPositive)) {
        normalCandidate.negate();
      }
      if (!(hasNegative && hasPositive)) {
        this->normals.addOnTopNoRepetition(normalCandidate);
      }
    }
    extraVertices.size = dimension - rankVerticesSpan;
  }
  return this->createFromNormals(this->normals);
}

bool Cone::createFromNormals(Vectors<Rational>& inputNormals) {
  return this->createFromNormals(inputNormals, false);
}

int Cone::getDimension() const {
  if (this->normals.size == 0) {
    return 0;
  }
  return this->normals[0].size;
}

bool Cone::createFromNormals(
  Vectors<Rational>& inputNormals, bool hasEnoughProjectiveVertices
) {
  STACK_TRACE("Cone::createFromNormals");
  this->flagIsTheZeroCone = false;
  this->lowestIndexNotCheckedForChopping = 0;
  this->lowestIndexNotCheckedForSlicingInDirection = 0;
  int dimension = 1;
  if (inputNormals.size > 0) {
    dimension = inputNormals[0].size;
  }
  this->normals = inputNormals;
  for (int i = 0; i < this->normals.size; i ++) {
    if (this->normals[i].isEqualToZero()) {
      this->normals.removeIndexSwapWithLast(i);
      i --;
    }
  }
  int numAddedFakeWalls = 0;
  Matrix<Rational> matrix;
  Selection selection;
  if (!hasEnoughProjectiveVertices) {
    for (
      int i = 0; i < dimension &&
      this->normals.getRankElementSpan(&matrix, &selection) < dimension; i ++
    ) {
      Vector<Rational> root;
      root.makeEi(dimension, i);
      if (
        !this->normals.linearSpanContainsVector(root, matrix, selection)
      ) {
        numAddedFakeWalls ++;
        this->normals.addOnTop(root);
      }
    }
  }
  this->computeVerticesFromNormalsNoFakeVertices();
  if (numAddedFakeWalls > 0) {
    this->normals.setSize(this->normals.size - numAddedFakeWalls);
    Vector<Rational> root;
    int originalSize = this->vertices.size;
    for (int i = 0; i < originalSize; i ++) {
      root = - this->vertices[i];
      if (this->isInCone(root)) {
        this->vertices.addOnTopNoRepetition(root);
      }
    }
  }
  return this->eliminateFakeNormalsUsingVertices(numAddedFakeWalls);
}

void ConeCollection::operator=(const ConeCollection& other) {
  this->refinedCones = other.refinedCones;
  this->nonRefinedCones = other.nonRefinedCones;
  this->splittingNormals = other.splittingNormals;
  this->slicingDirections = other.slicingDirections;
  this->flagIsRefined = other.flagIsRefined;
  this->flagChambersHaveTooFewVertices = other.flagChambersHaveTooFewVertices;
}

ConeCollection::ConeCollection() {
  this->flagChambersHaveTooFewVertices = false;
  this->flagIsRefined = false;
}

ConeCollection::ConeCollection(const ConeCollection& other) {
  this->operator=(other);
}

void ConeCollection::initialize() {
  this->splittingNormals.clear();
  this->slicingDirections.size = 0;
  this->refinedCones.clear();
  this->nonRefinedCones.clear();
  this->convexHull.normals.size = 0;
  this->convexHull.vertices.size = 0;
  this->convexHull.flagIsTheZeroCone = true;
}

void ConeCollection::initFromConeWalls(
  List<Cone>& normalsOfCones, bool hasEnoughProjectiveVertices
) {
  List<Vectors<Rational> > roots;
  roots.setSize(normalsOfCones.size);
  for (int i = 0; i < normalsOfCones.size; i ++) {
    roots[i] = normalsOfCones[i].normals;
  }
  this->initFromConeWalls(roots, hasEnoughProjectiveVertices);
}

void ConeCollection::initFromConeWalls(
  List<Vectors<Rational> >& normalsOfCones,
  bool hasEnoughProjectiveVertices
) {
  STACK_TRACE("ConeCollection::initFromConeWalls");
  Cone startingCone;
  this->refinedCones.clear();
  this->nonRefinedCones.clear();
  ProgressReport report;
  report.report(normalsOfCones.toString());
  for (int i = 0; i < normalsOfCones.size; i ++) {
    if (
      startingCone.createFromNormals(
        normalsOfCones[i], hasEnoughProjectiveVertices
      )
    ) {
      this->nonRefinedCones.addOnTop(startingCone);
    }
    std::stringstream out;
    out << "Initializing cone " << i + 1 << " out of " << normalsOfCones.size;
    report.report(out.str());
  }
  Vector<Rational> root;
  this->splittingNormals.clear();
  for (int i = 0; i < this->nonRefinedCones.size; i ++) {
    for (int j = 0; j < this->nonRefinedCones[i].normals.size; j ++) {
      root = this->nonRefinedCones[i].normals[j];
      root.scaleNormalizeFirstNonZero();
      this->splittingNormals.addOnTopNoRepetition(root);
      std::stringstream out;
      out
      << "Extracting walls from cone "
      << i + 1
      << " out of "
      << this->nonRefinedCones.size
      << " total distinct chambers.";
      out
      << "\nProcessed "
      << j + 1
      << " out of "
      << this->nonRefinedCones[i].normals.size
      << " walls of the current chamber.";
      out
      << "\nTotal # of distinct walls found: "
      << this->splittingNormals.size;
      report.report(out.str());
    }
  }
  this->convexHull.makeConvexHullOfMeAnd(startingCone);
}

bool ConeCollection::checkIsRefinedOrCrash() const {
  STACK_TRACE("ConeCollection::checkIsRefinedOrCrash");
  if (this->nonRefinedCones.size > 0) {
    global.fatal
    << "Non-refined cones not allowed by this function."
    << global.fatal;
  }
  return true;
}

std::string Cone::toHTML() const {
  STACK_TRACE("Cone::toHTML");
  std::stringstream out;
  bool lastVarIsConstant = false;
  if (this->flagIsTheZeroCone) {
    out << "The cone is the zero cone.";
  } else if (this->normals.size == 0) {
    out << "The cone is the entire space";
  }
  out
  << "<br>Index next wall to refine by: "
  << this->lowestIndexNotCheckedForChopping;
  out << "<br>";
  out
  << "\nIndex next direction to slice in: "
  << this->lowestIndexNotCheckedForSlicingInDirection
  << "\n";
  out << "<br>";
  out << "normals:\n";
  out << "<br>";
  out << "\\(";
  FormatExpressions format;
  out << this->normals.toLatexInequalities(lastVarIsConstant, format);
  out << "\\)";
  out << "<br>Projectivized vertices: " << this->vertices.toString();
  if (this->vertices.size > 0) {
    out << "<br>Internal point: " << this->getInternalPoint().toString();
  }
  return out.str();
}

std::string Cone::toString(FormatExpressions* format) const {
  std::stringstream out;
  bool PrepareMathReport = format == nullptr ? false : format->flagUseLatex;
  bool useHtml = format == nullptr ? false : format->flagUseHTML;
  bool useLatex = format == nullptr ? false : format->flagUseLatex;
  bool lastVarIsConstant = false;
  if (this->flagIsTheZeroCone) {
    out << "The cone is the zero cone.";
  } else if (this->normals.size == 0) {
    out << "The cone is the entire space";
  }
  if (!PrepareMathReport) {
    out
    << "Index next wall to refine by: "
    << this->lowestIndexNotCheckedForChopping
    << "\n";
    if (useHtml) {
      out << "<br>";
    }
    out
    << "\nIndex next direction to slice in: "
    << this->lowestIndexNotCheckedForSlicingInDirection
    << "\n";
  }
  if (useHtml) {
    out << "<br>";
  }
  out << "normals:\n";
  if (useHtml) {
    out << "<br>";
  }
  if (useLatex) {
    out << "\\[";
  }
  FormatExpressions tempF;
  if (format == nullptr) {
    format = &tempF;
  }
  out << this->normals.toLatexInequalities(lastVarIsConstant, *format);
  if (useLatex) {
    out << "\\]";
  }
  out << "\nProjectivized Vertices: " << this->vertices.toString();
  if (useHtml) {
    out << "<br>";
  }
  if (this->vertices.size > 0) {
    out << "\nInternal point: " << this->getInternalPoint().toString();
  }
  return out.str();
}

std::string ConeCollection::toString() {
  std::stringstream out;
  FormatExpressions format;
  out << "Non-refined chambers: " << this->nonRefinedCones.size;
  out << "\nRefined chamber: " << this->refinedCones.size;
  Vectors<Rational> roots;
  roots = this->splittingNormals;
  out << "\nNormals of walls to refine by: ";
  out << roots.toString(&format);
  if (this->slicingDirections.size > 0) {
    out
    << " Directions to slice along: "
    << this->slicingDirections.toString();
  }
  out << "Non-refined cones follow.\n";
  for (int i = 0; i < this->nonRefinedCones.size; i ++) {
    out << "\nChamber " << i + 1 << ":\n";
    out << this->nonRefinedCones[i].toString(&format) << "\n";
  }
  out << "Refined cones follow.\n";
  for (int i = 0; i < this->refinedCones.size; i ++) {
    out << "\nChamber " << i + 1 << ":\n";
    out << this->refinedCones[i].toString(&format) << "\n";
  }
  return out.str();
}

std::string ConeCollection::toHTML() const {
  STACK_TRACE("ConeCollection::toHTML");
  std::stringstream out;
  DrawingVariables drawingVariables;
  FormatExpressions format;
  out << this->drawMeToHtmlProjective(drawingVariables, format);
  out << "Refined chamber count: " << this->refinedCones.size;
  Vectors<Rational> roots;
  roots = this->splittingNormals;
  out << "<br>Normals of walls to refine by: ";
  out << roots.toString(&format);
  if (this->slicingDirections.size > 0) {
    out
    << " Directions to slice along: "
    << this->slicingDirections.toString();
  }
  for (int i = 0; i < this->refinedCones.size; i ++) {
    out << "<br>Chamber " << i + 1 << ":<br>";
    out << this->refinedCones[i].toHTML() << "\n\n\n";
  }
  out << "<hr>Non-refined chamber count: " << this->nonRefinedCones.size;
  for (int i = 0; i < this->nonRefinedCones.size; i ++) {
    out << "<br>Chamber " << i + 1 << ":<br>";
    out << this->nonRefinedCones[i].toHTML() << "<br>";
  }
  return out.str();
}

bool ConeCollection::findMaxLFOverConeProjective(
  const Cone& input,
  List<Polynomial<Rational> >& inputLinearPolynomials,
  List<int>& outputMaximumOverEeachSubChamber
) {
  Vectors<Rational> HyperPlanesCorrespondingToLF;
  if (input.normals.size < 1 || inputLinearPolynomials.size < 1) {
    return false;
  }
  int dimension = input.normals[0].size;
  HyperPlanesCorrespondingToLF.setSize(inputLinearPolynomials.size);
  for (int i = 0; i < inputLinearPolynomials.size; i ++) {
    Polynomial<Rational>& currentPoly = inputLinearPolynomials[i];
    if (currentPoly.totalDegree() != 1) {
      global.comments
      << "The total degree must be one, instead it is "
      << currentPoly.totalDegree()
      << ". The dimension of the cone is "
      << dimension;
      return false;
    }
    Vector<Rational>& newWall = HyperPlanesCorrespondingToLF[i];
    newWall.makeZero(dimension);
    for (int j = 0; j < currentPoly.size(); j ++) {
      for (int k = 0; k < dimension; k ++) {
        if (currentPoly[j](k) == 1) {
          newWall[k] = currentPoly.coefficients[j];
          break;
        }
      }
    }
  }
  return
  this->findMaxLFOverConeProjective(
    input, HyperPlanesCorrespondingToLF, outputMaximumOverEeachSubChamber
  );
}

bool ConeCollection::findMaxLFOverConeProjective(
  const Cone& input,
  Vectors<Rational>& inputLFsLastCoordConst,
  List<int>& outputMaximumOverEeachSubChamber
) {
  this->initialize();
  this->nonRefinedCones.addOnTop(input);
  this->convexHull.makeConvexHullOfMeAnd(input);
  Vector<Rational> root;
  for (int i = 0; i < inputLFsLastCoordConst.size; i ++) {
    for (int j = i + 1; j < inputLFsLastCoordConst.size; j ++) {
      root = inputLFsLastCoordConst[i] - inputLFsLastCoordConst[j];
      root.scaleNormalizeFirstNonZero();
      if (!root.isEqualToZero()) {
        this->splittingNormals.addOnTopNoRepetition(root);
      }
    }
  }
  global.comments << this->toHTML();
  this->refine();
  outputMaximumOverEeachSubChamber.setSize(this->refinedCones.size);
  Rational maximumScalarProduct = 0;
  for (int i = 0; i < this->refinedCones.size; i ++) {
    this->refinedCones[i].getInternalPoint(root);
    bool isInitialized = false;
    for (int j = 0; j < inputLFsLastCoordConst.size; j ++) {
      if (
        !isInitialized ||
        root.scalarEuclidean(inputLFsLastCoordConst[j]) > maximumScalarProduct
      ) {
        maximumScalarProduct =
        root.scalarEuclidean(inputLFsLastCoordConst[j]);
        outputMaximumOverEeachSubChamber[i] = j;
        isInitialized = true;
      }
    }
  }
  return true;
}

void Lattice::reduce() {
  STACK_TRACE("Lattice::reduce");
  this->basis.gaussianEliminationEuclideanDomain();
  int rowsToTrim = 0;
  for (int i = this->basis.numberOfRows - 1; i >= 0; i --) {
    bool foundNonZeroRow = false;
    for (int j = 0; j < this->basis.numberOfColumns; j ++) {
      if (!this->basis.elements[i][j].isEqualToZero()) {
        foundNonZeroRow = true;
        break;
      }
    }
    if (foundNonZeroRow) {
      break;
    }
    rowsToTrim ++;
  }
  this->basis.resize(
    this->basis.numberOfRows - rowsToTrim,
    this->basis.numberOfColumns,
    true
  );
  this->basisRationalForm.assignMatrixIntegerWithDenominator(
    this->basis, this->denominator
  );
}

void Lattice::testGaussianEliminationEuclideanDomainRationals(
  Matrix<Rational>& output
) {
  output.assignMatrixIntegerWithDenominator(
    this->basis, this->denominator
  );
  std::stringstream out;
  global.comments << "Test output: " << output.toString();
  out << "Test output: " << output.toString();
  output.gaussianEliminationEuclideanDomain();
  global.comments << "<br>After gaussian elimination:" << output.toString();
  out << "<br>After gaussian elimination:" << output.toString();
}

void Lattice::refineByOtherLattice(const Lattice& other) {
  if (other.basis.numberOfColumns == 0) {
    return;
  }
  if (
    other.basis == this->basis && this->denominator == other.denominator
  ) {
    return;
  }
  if (other.getDimension() != this->getDimension()) {
    global.fatal << "Dimension mismatch. " << global.fatal;
  }
  int dimension = this->getDimension();
  LargeIntegerUnsigned oldDen = this->denominator;
  LargeIntegerUnsigned::leastCommonMultiple(
    other.denominator, oldDen, this->denominator
  );
  LargeIntegerUnsigned scaleThis, scaleOther;
  scaleThis = this->denominator / oldDen;
  scaleOther = this->denominator / other.denominator;
  int oldNumRows = this->basis.numberOfRows;
  LargeInteger scale;
  scale = scaleThis;
  this->basis *= scale;
  this->basis.resize(
    this->basis.numberOfRows + other.basis.numberOfRows,
    dimension,
    true
  );
  for (int i = oldNumRows; i < this->basis.numberOfRows; i ++) {
    for (int j = 0; j < this->basis.numberOfColumns; j ++) {
      this->basis.elements[i][j] =
      other.basis.elements[i - oldNumRows][j] * scaleOther;
    }
  }
  this->reduce();
}

void Lattice::makeFromMatrix(const Matrix<Rational>& input) {
  this->basisRationalForm = input;
  this->basisRationalForm.getMatrixIntegerWithDenominator(
    this->basis, this->denominator
  );
  this->reduce();
}

void Lattice::makeFromRoots(const Vectors<Rational>& input) {
  Matrix<Rational> rescaled;
  rescaled.assignVectorsToRows(input);
  rescaled.getMatrixIntegerWithDenominator(this->basis, this->denominator);
  this->reduce();
}

std::string Lattice::toString() const {
  std::stringstream out;
  out << "L=<";
  Vectors<Rational> roots;
  roots.assignMatrixRows(this->basisRationalForm);
  for (int i = 0; i < this->basisRationalForm.numberOfRows; i ++) {
    out << roots[i].toString();
    if (i != this->basisRationalForm.numberOfRows - 1) {
      out << ",";
    }
  }
  out << ">";
  return out.str();
}
