#include "crypto_calculator.h"
#include "general_file_operations_encodings.h"
#include "general_lists.h"
#include "general_strings.h"
#include "math_extra_drawing_variables.h"
#include "math_extra_finite_groups_implementation.h"
#include "math_extra_modules_semisimple_lie_algebras.h"
#include "math_extra_symmetric_groups_and_generalizations.h"
#include "math_general_implementation.h"
#include "math_general_polynomial_computations_basic_implementation.h" // IWYU pragma: keep: breaks the build.
#include "math_lattices.h"
#include "math_linear_combination.h"
#include "math_mathml.h"
#include "math_rational_function.h"
#include "math_vector_partition_functions.h"
#include "string_constants.h"
#include <dirent.h>
#include <unistd.h>

// The below gives upper limit to the amount of pointers
// that are allowed to be allocated by the program. Can be changed dynamically.
// Used to guard the web server from abuse.
#ifdef AllocationLimitsSafeguard

long long GlobalStatistics::limitTotalPointersInList = 2000000000;
#endif

MutexRecursiveWrapper ProgressReport::reportMutex;
bool GlobalStatistics::flagUngracefulExitInitiated = false;
long long GlobalStatistics::globalPointerCounter = 0;
long long GlobalStatistics::pointerCounterPeakRamUse = 0;
unsigned int GlobalStatistics::numberOfHashResizes = 0;
unsigned int GlobalStatistics::numberOfListResizesTotal = 0;
unsigned int GlobalStatistics::numberOfListsCreated = 0;
int HtmlRoutines::globalGeneralPurposeID = 0;
template < >
bool Complex<double>::flagEqualityIsApproximate =
true;
template < >
double Complex<double>::equalityPrecision =
0.00000001;
template <class ElementLeft, class ElementRight, class Coefficient>
class TensorProductMonomial;

List<bool> FileOperations::safeFileCharacters;
unsigned long long int Rational::totalLargeAdditions = 0;
unsigned long long int Rational::totalLargeGreatestCommonDivisors = 0;
unsigned long long int Rational::totalLargeMultiplications = 0;
unsigned long long int Rational::totalSmallAdditions = 0;
unsigned long long int Rational::totalSmallGreatestCommonDivisors = 0;
unsigned long long int Rational::totalSmallMultiplications = 0;
std::stringstream HtmlRoutines::outputStream;

template < >
List<OnePartialFractionDenominator>::Comparator* FormatExpressions::
getMonomialOrder<OnePartialFractionDenominator>() {
  return nullptr;
}

template < >
List<Vector<Rational> >::Comparator* FormatExpressions::getMonomialOrder<
  Vector<Rational>
>() {
  return nullptr;
}

// Format expression monomial orders for the toString() function follow.
template < >
List<MonomialWrapper<std::string, HashFunctions::hashFunction<std::string> > >
::Comparator* FormatExpressions::getMonomialOrder<
  MonomialWrapper<std::string, HashFunctions::hashFunction<std::string> >
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

bool GlobalVariables::Response::reportDesired() {
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
  // 100 seconds
  this->millisecondsMaxComputation = 100000;
  this->millisecondOffset = 0;
  this->millisecondsComputationStart = - 1;
  // 7 seconds
  this->millisecondsReplyAfterComputation = 7000;
  this->pointerExternalCommandReturnOutput = nullptr;
  this->pointerExternalCommandNoOutput = nullptr;
  this->pointerExternalCommandStream = nullptr;
  this->pointerCallChDir = nullptr;
  this->flagServerForkedIntoWorker = false;
  this->flagComputationComplete = false;
  this->flagComputationFinishedAllOutputSentClosing = false;
  this->runMode = GlobalVariables::RunMode::consoleRegular;
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
  this->databaseType = DatabaseType::internal;
  this->flagLocalhostConnectionMonitor = true;
  this->flagUseMathTags = false;
  this->timePointer = GlobalVariables::timeInternal;
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
  STACK_TRACE("ProgressReport::initialize");
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
  MutexlockGuard guard(this->reportMutex);
  (void) guard;
  this->flagInitialized = true;
  ListReferences<std::string>& currentThreadReports =
  global.progressReportStrings[this->threadIndex];
  this->currentLevel = currentThreadReports.size;
  if (this->currentLevel < 0) {
    std::stringstream out;
    out
    << "Unexpected negative size of progress strings: "
    << currentThreadReports.size
    << " read as: "
    << this->currentLevel
    << "All progress report strings: "
    << global.progressReportStrings
    << " Current thread reports deallocated: "
    << currentThreadReports.flagDeallocated
    << ". Thread index: "
    << this->threadIndex;
    fatalCrash(out.str());
  }
  currentThreadReports.addOnTop(std::string(""));
  this->ticks = 0;
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
  ListReferences<std::string>& current =
  global.progressReportStrings[this->threadIndex];
  if (current.size <= 0) {
    std::stringstream errorStream;
    errorStream << "Corrupt progress report. ";
    fatalCrash(errorStream.str());
  }
  current.size --;
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

void HtmlRoutines::clearDollarSigns(std::string& input, std::string& output) {
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
  " style='background:none; border:0; "
  "text-decoration:underline; color:blue; cursor:pointer' ";
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
  STACK_TRACE(
    "StringRoutines::"
    "Conversions::"
    "escapeQuotesBackslashesNewLines"
  );
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
  if (input >= 2097152 // = 2^21-1
  ) {
    // Invalid code point.
    // A unicode code point must be smaller than 2^21.
    // It must fit in the largest possible encoding:
    // b_11110???,b_10??????,b_10??????,b_10??????  (3+3*6=21 payload bits).
    // We convert the code point to the 4-byte bigendian sequence that
    // represents the integer. 
    List<unsigned char> fourBytes;
    Crypto::convertUnsignedInt32ToUnsignedCharBigendian(input, fourBytes);
    out << fourBytes[0] << fourBytes[1] << fourBytes[2] << fourBytes[3];
    return out.str();
  }
  if (input < 128) {
    unsigned char output = input % 256;
    out << output;
    return out.str();
  }
  if (input < 2048 // = 2^11, encoding fits in b_110?????, b_10??????
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
    input < 65536 // = 2^16, encoding fits in b_1110????, b_10??????,
    // b_10??????
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
      if (
        left == StringRoutines::notAllowedInStrings ||
        right == StringRoutines::notAllowedInStrings
      ) {
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
    char hex0 = MathRoutines::convertHumanReadableHexToCharValue(input[i + 2]);
    char hex1 = MathRoutines::convertHumanReadableHexToCharValue(input[i + 3]);
    char hex2 = MathRoutines::convertHumanReadableHexToCharValue(input[i + 4]);
    char hex3 = MathRoutines::convertHumanReadableHexToCharValue(input[i + 5]);
    if (
      hex0 == StringRoutines::notAllowedInStrings ||
      hex1 == StringRoutines::notAllowedInStrings ||
      hex2 == StringRoutines::notAllowedInStrings ||
      hex3 == StringRoutines::notAllowedInStrings
    ) {
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

std::string StringRoutines::join(
  const List<std::string>& input, const std::string& separator
) {
  std::stringstream out;
  for (int i = 0; i < input.size; i ++) {
    out << input[i];
    if (i != input.size - 1) {
      out << separator;
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
  const std::string& inputString, bool doReplaceNewLineByBr, int maxStringSize
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
  const std::string& inputString, bool doReplaceNewLineCarriageReturnByBr
) {
  std::string result;
  HtmlRoutines::convertStringToHtmlStringReturnTrueIfModified(
    inputString, result, doReplaceNewLineCarriageReturnByBr
  );
  return result;
}

std::string HtmlRoutines::convertStringToHtmlStringWithBr(
  const std::string& inputString
) {
  std::string result;
  HtmlRoutines::convertStringToHtmlStringReturnTrueIfModified(
    inputString, result, true, false
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
  for (int i = static_cast<signed>(fileName.size() - 1); i >= 0; i --) {
    if (fileName[static_cast<unsigned>(i)] == '.') {
      if (outputFileNameNoExtension != nullptr) {
        *outputFileNameNoExtension =
        fileName.substr(0, static_cast<unsigned>(i));
      }
      return fileName.substr(static_cast<unsigned>(i), std::string::npos);
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
  std::string filePath =
  FileOperations::getPathFromFileNameWithPath(fileName);
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
    if (fileName[i] == '/' || fileName[i] == '\\' || fileName[i] == '.') {
      return false;
    }
  }
  return true;
}

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
          static_cast<unsigned int>(static_cast<unsigned char>(characters[i]))
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
  for (unsigned i = startNameWithoutFolderInfo; i < fileName.size(); i ++) {
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

bool FileOperations::writeFileVirtual(
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
  writeFileVirualWithPermissions_accessUltraSensitiveFoldersIfNeeded(
    fileNameVirtual,
    fileContent,
    accessSensitiveFolders,
    false,
    commentsOnError
  );
}

bool FileOperations::
writeFileVirualWithPermissions_accessUltraSensitiveFoldersIfNeeded(
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
  STACK_TRACE("FileOperations::writeFileReturnHTMLLink");
  std::stringstream commentsOnError;
  bool success =
  FileOperations::writeFileVirtual(
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
  FileOperations::loadFiletoStringVirtual_accessUltraSensitiveFoldersIfNeeded(
    fileName, output, accessSensitiveFolders, false, commentsOnFailure
  );
}

bool FileOperations::
loadFiletoStringVirtual_accessUltraSensitiveFoldersIfNeeded(
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
      << " exists but I failed to open it in "
      << "text mode (perhaps not a valid ASCII/UTF8 file). ";
    }
    return false;
  }
  std::stringstream contentStream;
  contentStream << fileStream.rdbuf();
  output = contentStream.str();
  return true;
}

MapList<std::string, std::string, HashFunctions::hashFunction<std::string> >&
FileOperations::folderVirtualLinksNonSensitive() {
  static MapList<
    std::string, std::string, HashFunctions::hashFunction<std::string>
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

MapList<std::string, std::string, HashFunctions::hashFunction<std::string> >&
FileOperations::folderVirtualLinksSensitive() {
  static MapList<
    std::string, std::string, HashFunctions::hashFunction<std::string>
  > result;
  return result;
}

MapList<std::string, std::string, HashFunctions::hashFunction<std::string> >&
FileOperations::folderVirtualLinksULTRASensitive() {
  static MapList<
    std::string, std::string, HashFunctions::hashFunction<std::string>
  > result;
  return result;
}

List<List<std::string> >& FileOperations::folderVirtualLinksDefault() {
  static List<List<std::string> > result;
  return result;
}

void FileOperations::initializeFoldersULTRASensitive() {
  STACK_TRACE("FileOperations::initializeFoldersULTRASensitive");
  MapList<std::string, std::string, HashFunctions::hashFunction<std::string> >&
  folderSubstitutionsULTRASensitive =
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
  folderSubstitutionsULTRASensitive.setKeyValue("database/", "database/");
  folderSubstitutionsULTRASensitive.setKeyValue("/database/", "/database/");
  folderSubstitutionsULTRASensitive.setKeyValue(
    "/database_backups/", "/database_backups/"
  );
  folderSubstitutionsULTRASensitive.setKeyValue(
    "database_backups/", "database_backups/"
  );
}

void FileOperations::initializeFoldersSensitive() {
  STACK_TRACE("FileOperations::initializeFoldersSensitive");
  MapList<std::string, std::string, HashFunctions::hashFunction<std::string> >&
  folderSubstitutionsSensitive =
  FileOperations::folderVirtualLinksSensitive();
  folderSubstitutionsSensitive.clear();
  folderSubstitutionsSensitive.setKeyValue("LogFiles/", "LogFiles/");
  folderSubstitutionsSensitive.setKeyValue("/LogFiles/", "LogFiles/");
  folderSubstitutionsSensitive.setKeyValue("configuration/", "configuration/");
  folderSubstitutionsSensitive.setKeyValue("test/", "test/");
  folderSubstitutionsSensitive.setKeyValue(
    "/configuration/", "configuration/"
  );
  folderSubstitutionsSensitive.setKeyValue(
    Configuration::freecalc, "../freecalc/"
  );
  folderSubstitutionsSensitive.setKeyValue(
    "/" + Configuration::freecalc, "../freecalc/"
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
        }),
      List<std::string>({Configuration::examples, "examples/"}),
      List<std::string>(
        {Configuration::HTMLAceSrcMin, "html-common/ace/src-min/"}
      ),
      List<std::string>({Configuration::HTMLCommonFonts, "html-common/fonts"}),
      List<std::string>({Configuration::HTMLCommonFont, "html-common/fonts"}),
      List<std::string>({Configuration::HTMLCommon, "html-common/"}),
      List<std::string>({Configuration::calculatorHTML, "calculator_html/"}),
      List<std::string>({Configuration::publicHTML, "public_html/"}),
      List<std::string>(
        {Configuration::certificatesPublic, "certificates-public/"}
      ),
      List<std::string>({Configuration::sourceCode, "src/"}),
      List<std::string>({Configuration::testFolder, "test/"}),
      List<std::string>({Configuration::outputFolder, "output/"}),
      List<std::string>({Configuration::HTMLGeneral, "public_html/"}),
      List<std::string>({Configuration::problemsFolder, "../problems/"}),
      List<std::string>(
        {Configuration::courseTemplates, "../coursetemplates/"}
      ),
      List<std::string>(
        {Configuration::coursesAvailable, "../coursesavailable/"}
      ),
      List<std::string>({Configuration::topicLists, "../topiclists/"}),
      List<std::string>(
        {Configuration::laTeXMaterials, "../LaTeX-materials/"}
      ),
      List<std::string>({Configuration::slidesVideo, "../slides-video/"}),
      List<std::string>({Configuration::freecalc, "../freecalc/"}),
      List<std::string>({"favicon.ico", "calculator_html/favicon.ico"}),
    });
  FileOperations::checkFolderLinks();
  return result;
}

void FileOperations::initializeFoldersNonSensitive() {
  STACK_TRACE("FileOperations::initializeFoldersNonSensitive");
  // Warning: order of substitutions is important.
  // Only the first rule that applies is applied, once.
  // No further rules are applied after that.
  // Location keys that start with "/" are coming from webserver references.
  // Location keys that do not start with "/" are for internal use.
  MapList<std::string, std::string, HashFunctions::hashFunction<std::string> >&
  folderSubstitutionsNonSensitive =
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
      << "; global.configuration: "
      << global.configuration.toString()
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

std::string FileOperations::addPaths(
  const std::string& left, const std::string right
) {
  std::string leftWithoutEndSlash = left;
  std::string rightWithoutStartSlash = right;
  if (StringRoutines::stringEndsWith(left, "/")) {
    leftWithoutEndSlash = left.substr(0, left.size() - 1);
  }
  if (StringRoutines::stringBeginsWith(right, "/")) {
    rightWithoutStartSlash = right.substr(1);
  }
  return leftWithoutEndSlash + "/" + rightWithoutStartSlash;
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
  STACK_TRACE(
    "FileOperations::openFileVirtualCustomizedWriteOnlyCreateIfNeeded"
  );
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
  bool openInAppendMode,
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
    file, computedFileName, openInAppendMode, truncate, openAsBinary
  );
}

bool FileOperations::openFileVirtual(
  std::fstream& file,
  const std::string& fileName,
  bool openInAppendMode,
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
    file, computedFileName, openInAppendMode, truncate, openAsBinary
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
        file.open(fileName.c_str(), std::fstream::in | std::fstream::out);
      }
    }
  }
  return file.is_open();
}

bool FileOperations::openFileUnsecureReadOnly(
  std::ifstream& file, const std::string& fileName, bool openAsBinary
) {
  if (openAsBinary) {
    file.open(fileName.c_str(), std::fstream::in | std::fstream::binary);
  } else {
    file.open(fileName.c_str(), std::fstream::in);
  }
  return file.is_open();
}

std::string FileOperations::getVirtualNameWithHash(
  const std::string& inputFileName
) {
  STACK_TRACE("FileOperations::getVirtualNameWithHash");
  std::string result = inputFileName;
  std::string fileNameEnd;
  for (
    int i = 0; i <
    FileOperations::folderVirtualLinksToWhichWeAppendTimeAndBuildHash().size;
    i ++
  ) {
    const std::string& currentStart =
    FileOperations::folderVirtualLinksToWhichWeAppendTimeAndBuildHash()[i];
    if (StringRoutines::stringBeginsWith(result, currentStart, &fileNameEnd)) {
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
    FileOperations::folderStartsToWhichWeAppendInstructorUsernameSlash().
    size; i ++
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
    FileOperations::folderStartsToWhichWeAppendInstructorUsernameSlash().
    size; i ++
  ) {
    if (
      StringRoutines::stringBeginsWith(
        inputFileName,
        FileOperations::folderStartsToWhichWeAppendInstructorUsernameSlash()[
          i
        ],
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

bool FileOperations::deleteFileVirtual(
  const std::string& fileNameVirtual,
  bool accessUltraSensitiveFoldersIfNeeded,
  std::stringstream* commentsOnError
) {
  std::string fileName;
  if (
    !FileOperations::getPhysicalFileNameFromVirtual(
      fileNameVirtual,
      fileName,
      accessUltraSensitiveFoldersIfNeeded,
      accessUltraSensitiveFoldersIfNeeded,
      commentsOnError
    )
  ) {
    return false;
  }
  int result = remove(fileName.c_str());
  if (result != 0) {
    if (commentsOnError != nullptr) {
      *commentsOnError << "Failed to erase file: " << fileName;
    }
    return false;
  }
  return true;
}

bool FileOperations::getPhysicalFileNameFromVirtual(
  const std::string& inputFileName,
  std::string& output,
  bool accessSensitiveFolders,
  bool accessULTRASensitiveFolders,
  std::stringstream* commentsOnFailure
) {
  STACK_TRACE("FileOperations::getPhysicalFileNameFromVirtual");
  // Using loggers forbidden here: function is used by the loggers themselves.
  if (
    !FileOperations::isOKFileNameVirtual(
      inputFileName, accessSensitiveFolders
    )
  ) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure << "File name: " << inputFileName << " not allowed. ";
    }
    return false;
  }
  std::string inputCopy = inputFileName;
  std::string folderEnd, folderEnd2;
  for (
    int i = 0; i <
    FileOperations::folderVirtualLinksToWhichWeAppendTimeAndBuildHash().size;
    i ++
  ) {
    if (
      StringRoutines::stringBeginsWith(
        inputCopy,
        FileOperations::folderVirtualLinksToWhichWeAppendTimeAndBuildHash()[
          i
        ],
        &folderEnd
      )
    ) {
      if (
        StringRoutines::stringBeginsWith(
          folderEnd, global.buildHeadHashWithServerTime, &folderEnd2
        )
      ) {
        inputCopy =
        FileOperations::folderVirtualLinksToWhichWeAppendTimeAndBuildHash()[
          i
        ] +
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
      int i = 0; i < FileOperations::folderVirtualLinksSensitive().size(); i
      ++
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
      int i = 0; i < FileOperations::folderVirtualLinksULTRASensitive().size(
      ); i ++
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
  return
  FileOperations::openFileCreateIfNotPresentUnsecure(
    file, computedFileName, openInAppendMode, truncate, openAsBinary
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
        file.open(fileName.c_str(), std::fstream::in | std::fstream::out);
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
  return StringRoutines::notAllowedInStrings;
}

unsigned int MathRoutines::hashListStrings(const List<std::string>& input) {
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
  std::stringstream leftOut;
  std::stringstream rightOut;
  std::stringstream commentsOnFailure;
  if (!this->computeDifference(&commentsOnFailure)) {
    commentsOnFailure
    << "<b style='color:red'>Failed to compute string difference.</b><br>";
    return
    this->differenceHTMLPartTwo(
      commentsOnFailure.str(),
      labelLeft,
      labelRight,
      StringRoutines::shortenInsertDots(this->left, 10000),
      StringRoutines::shortenInsertDots(this->right, 10000)
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
    static_cast<int>(numberOfRows), static_cast<int>(numberOfColumns)
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
    if (matrix(indexLeft - 1, indexRight) > matrix(indexLeft, indexRight - 1)) {
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
    this->commonStringsLeftStartsReverseOrder, this->left, this->leftResult
  );
  this->extractResult(
    this->commonStringsRightStartsReverseOrder, this->right, this->rightResult
  );
  while (this->leftResult.size < this->rightResult.size) {
    this->leftResult.addOnTop("");
  }
  while (this->rightResult.size < this->leftResult.size) {
    this->rightResult.addOnTop("");
  }
}

void StringRoutines::Differ::extractResult(
  const List<int>& starts, const std::string& input, List<std::string>& output
) {
  STACK_TRACE("StringRoutines::Differ::extractResult");
  output.setSize(0);
  unsigned int previousEnd = 0;
  for (int i = this->commonStringSizesReverseOrder.size - 1; i >= 0; i --) {
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
      static_cast<unsigned>(stringStart), static_cast<unsigned>(stringSize)
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
  leftSize = this->left.size();
  // Warning: putting this in the line above may cause ambiguous conversions,
  // depending on your compiler.
  LargeInteger rightSize;
  rightSize = this->right.size();
  if (leftSize * rightSize > this->maximumMatrixSize) {
    if (commentsOnFailure != nullptr) {
      *commentsOnFailure
      << "The product of the sizes of the two strings equals "
      << leftSize * rightSize
      << " which exceeds the maximum allowed "
      << this->maximumMatrixSize
      << ". ";
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

std::string StringRoutines::shortenInsertDots(
  const std::string& inputString, int maximumCharacters
) {
  if (inputString.size() <= static_cast<unsigned>(maximumCharacters)) {
    return inputString;
  }
  std::stringstream out;
  int numberOfCharactersBeginEnd = maximumCharacters / 2 - 2;
  int numberOfCharactersOmitted = static_cast<signed>(inputString.size()) -
  numberOfCharactersBeginEnd * 2;
  out
  << inputString.substr(0, static_cast<unsigned>(numberOfCharactersBeginEnd))
  << "..omit_"
  << numberOfCharactersOmitted
  << ".."
  << inputString.substr(
    static_cast<unsigned>(
      static_cast<signed>(inputString.size()) - numberOfCharactersBeginEnd
    )
  );
  return out.str();
}

void StringRoutines::splitExcludeDelimiter(
  const std::string& inputString, char delimiter, List<std::string>& output
) {
  List<char> buffer;
  buffer.addOnTop(delimiter);
  StringRoutines::splitExcludeDelimiters(inputString, buffer, output);
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
  if (static_cast<signed>(inputOutput.size()) <= desiredLength50AtLeast) {
    return;
  }
  if (desiredLength50AtLeast < 40) {
    return;
  }
  std::stringstream inputAbbreviatedStream;
  inputAbbreviatedStream
  << inputOutput.substr(0, static_cast<unsigned>(desiredLength50AtLeast - 30))
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
  inputOutput.replace(tagLocation, subStringToReplace.size(), replaceWith);
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
  outputFirst = inputString.substr(0, static_cast<unsigned>(firstStringSize));
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
    << "We do not have factorials of negative "
    << "integers here. You wanted factorial of: "
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
    if (outputStringBeginning != nullptr) {
      *outputStringBeginning = input;
    }
    return true;
  }
  int indexInString = static_cast<signed>(input.size()) - 1;
  for (int i = static_cast<signed>(desiredEnd.size()) - 1; i >= 0; i --) {
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
    *outputStringBeginning = input.substr(0, input.size() - desiredEnd.size());
  }
  return true;
}

bool StringRoutines::stringBeginsWith(
  const std::string& input,
  const std::string& desiredBeginning,
  std::string* outputStringEnd
) {
  std::string actualBeginning;
  std::string stringEnd;
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

int MathRoutines::binomialCoefficientMultivariate(int n, List<int>& choices) {
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
  this->elements.initializeFillInObject(inputNumberOfElements, - 1);
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
  Vector<Rational> vector;
  vector = *this;
  return vector.toString();
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

void Selection::initSelectionFixedCardinality(int cardinality) {
  this->initialize(this->numberOfElements);
  this->elements.setSize(cardinality);
  for (int i = 0; i < cardinality; i ++) {
    this->selected[i] = true;
    this->elements[i] = i;
  }
  this->cardinalitySelection = cardinality;
}

bool Selection::checkInitialization() const {
  if (this->numberOfElements < 0) {
    global.fatal
    << "Number of elements not allowed to be negative. "
    << global.fatal;
  }
  return true;
}

bool Selection::incrementSelectionFixedCardinalityReturnFalseIfPastLast(
  int cardinality
) {
  this->checkInitialization();
  if (cardinality < 0) {
    global.fatal << "Negative cardinality not allowed." << global.fatal;
  }
  // Example of the order of generation of all combinations
  // when cardinality = 2 and maximumSize = 5. The second column indicates the
  // state of the array at the point in code marked with (*) below.
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
    return false;
  }
  if (this->cardinalitySelection != cardinality) {
    this->initSelectionFixedCardinality(cardinality);
    return true;
  }
  if (cardinality == this->numberOfElements || cardinality == 0) {
    // We've reached the last combination.
    return false;
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
    // We've reached the last combination.
    // Go back to the first combination and return false.
    this->initSelectionFixedCardinality(cardinality);
    return false;
  }
  for (int i = 0; i < numberOfOnesAfterLastZeroWithOneBefore + 1; i ++) {
    this->selected[this->elements[this->cardinalitySelection - i - 1]] = false;
  }
  // (*) See the comments above for more explanations.
  for (int i = 0; i < numberOfOnesAfterLastZeroWithOneBefore + 1; i ++) {
    this->selected[i + indexLastZeroWithOneBefore] = true;
    this->elements[
      this->cardinalitySelection +
      i - numberOfOnesAfterLastZeroWithOneBefore - 1
    ] =
    i + indexLastZeroWithOneBefore;
  }
  this->elements.setSize(this->cardinalitySelection);
  return true;
}

void Selection::computeIndicesFromSelection() {
  this->cardinalitySelection = 0;
  this->elements.clear();
  for (int i = 0; i < this->numberOfElements; i ++) {
    if (this->selected[i]) {
      this->elements.addOnTop(i);
      this->cardinalitySelection ++;
    }
  }
}

void Selection::makeSubSelection(Selection& selection, Selection& subSelection)
{
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

void FormatExpressions::makePolynomialAlphabetLetters(
  const std::string& inputDefaultLetter, int letterCount
) {
  this->polynomialAlphabet.clear();
  for (int i = 0; i < letterCount; i ++) {
    std::stringstream out;
    out << inputDefaultLetter << "_";
    int index = i + 1;
    if (index < 10) {
      out << index;
    } else {
      out << "{" << index << "}";
    }
    this->polynomialAlphabet.addOnTop(out.str());
  }
}

void FormatExpressions::makeAlphabetXYZUW() {
  if (this->polynomialAlphabet.size > 0) {
    return;
  }
  List<std::string> letters = List<std::string>({"x", "y", "z", "u", "w"});
  for (const std::string& letter : letters) {
    this->polynomialAlphabet.addOnTop(VariableLetter(letter, letter));
  }
}

FormatExpressions* FormatExpressions::defaultFormat() {
  static FormatExpressions result;
  if (result.polynomialAlphabet.size == 0) {
    result.polynomialAlphabet.addOnTop(VariableLetter("x", "x"));
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
  this->flagIsInNumerator = false;
  this->maximumMatrixDisplayedRows = 20;
  this->maximumMatrixLineLength = 20;
  this->monomialOrder.leftGreaterThanRight =
  MonomialPolynomial::orderDefault().leftGreaterThanRight;
}

std::string FormatExpressions::polynomialLatexLetter(int index) const {
  if (index < this->polynomialAlphabet.size) {
    return this->polynomialAlphabet[index].latexLetter;
  }
  std::stringstream out;
  out << this->polynomialDefaultLetter << "_{" << index + 1 << "}";
  return out.str();
}

std::string FormatExpressions::polynomialMathMLLetter(int index) const {
  if (index < this->polynomialAlphabet.size) {
    return "<mi>" + this->polynomialAlphabet[index].mathMLLetter + "</mi>";
  }
  std::stringstream out;
  out
  << "<msub><mi>"
  << this->polynomialDefaultLetter
  << "</mi><mn>"
  << index + 1
  << "</mn></msub>";
  return out.str();
}

bool OnePartialFractionDenominator::reduceOnceTotalOrderMethod(
  LinearCombination<OnePartialFractionDenominator, Polynomial<LargeInteger> >&
  output
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
  LinearCombination<OnePartialFractionDenominator, Polynomial<LargeInteger> >&
  output,
  const Polynomial<LargeInteger>& numerator
) {
  STACK_TRACE("OnePartialFractionDenominator::reduceOnce");
  this->checkInitialization();
  if (this->denominatorsNoScale.size() <= this->owner->ambientDimension) {
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
  if (this->owner->flagShowDetails) {
    this->owner->details.addStateBeforeFormula(
      *this, numerator, linearDependence, allRootsSorted
    );
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
  STACK_TRACE("OnePartialFractionDenominator::computeOneCheckSum");
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
  STACK_TRACE("OnePartialFractionDenominator::toLatex");
  std::stringstream out;
  out << "\\frac{";
  int numberOfLines = 1;
  std::string numeratorString =
  numerator.toStringWithPossibleLineBreak(format, &numberOfLines);
  if (numberOfLines > 1) {
    out << "~~~~" << numeratorString << "~~~~";
  } else {
    out << numeratorString;
  }
  out << "}{" << this->toStringDenominatorOnly(format) << "}";
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
  STACK_TRACE("OnePartialFractionDenominator::toStringDenominatorOnly");
  if (this->owner == nullptr) {
    return "(uninitialized)";
  }
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
    roots.addOnTop(this->denominatorsNoScale.values[i].getNormalizedVector());
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
    int i = 0; i < owner.indicesRedundantShortRoots.cardinalitySelection; i
    ++
  ) {
    int index = owner.indicesRedundantShortRoots.elements[i];
    int rootIndex =
    owner.getIndexDoubleOfARoot(owner.normalizedVectors[index]);
    if (rootIndex != - 1) {
      if (this->denominatorsNoScale.values[rootIndex].multiplicities.size > 0) {
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
    "OnePartialFractionDenominator::"
    "computeGainingMultiplicityIndexInLinearRelation"
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
  LinearCombination<OnePartialFractionDenominator, Polynomial<LargeInteger> >&
  output
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
  LinearCombination<OnePartialFractionDenominator, Polynomial<LargeInteger> >&
  output
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
  LinearCombination<OnePartialFractionDenominator, Polynomial<LargeInteger> >&
  output,
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
  this->addMultiplicity(this->denominatorsNoScale.keys[index], - increment, 1);
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

unsigned int OnePartialFractionDenominator::hashFunction(
  const OnePartialFractionDenominator& input
) {
  return input.hashFunction();
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

std::string PartialFractions::toLatexOneSum(
  const LinearCombination<
    OnePartialFractionDenominator, Polynomial<LargeInteger>
  >& fractions,
  const std::string& separator,
  const PartialFractions::HighlightInformation* highlightInformation,
  FormatExpressions* format
) const {
  STACK_TRACE("PartialFractions::toLatexOneSum");
  std::stringstream out;
  for (int i = 0; i < fractions.size(); i ++) {
    if (i != 0) {
      out << separator;
      out << "+";
    }
    out << "\\displaystyle ";
    bool showDetails = false;
    if (this->flagShowDetails && highlightInformation != nullptr) {
      if (fractions.monomials[i] == highlightInformation->toBeReducedNext) {
        showDetails = true;
      }
    }
    if (showDetails) {
      out << "\\underbrace{";
    }
    out << fractions.monomials[i].toLatex(fractions.coefficients[i], format);
    if (showDetails) {
      out << "}_{" << highlightInformation->toStringLinearCombination() << "}";
    }
  }
  return out.str();
}

std::string PartialFractions::toLatexFullSum(
  const std::string& lineSeparator, FormatExpressions* format
) const {
  STACK_TRACE("PartialFractions::toLatexFullSum");
  std::stringstream out;
  LinearCombination<OnePartialFractionDenominator, Polynomial<LargeInteger> >
  total;
  this->getSum(total);
  out << this->toLatexOneSum(total, lineSeparator, nullptr, format);
  return out.str();
}

std::string PartialFractions::toLatexDifferentialOperatorForm(
  const std::string& lineSeparator, FormatExpressions* format
) const {
  STACK_TRACE("PartialFractions::toLatexDifferentialOperatorForm");
  std::stringstream out;
  for (int i = 0; i < this->reduced.size(); i ++) {
    const OnePartialFractionDenominator& fraction =
    this->reduced.monomials[i];
    std::string summand =
    fraction.toLatexDifferentialOperator(
      this->reduced.coefficients[i], format
    );
    if (i > 0 && !StringRoutines::stringBeginsWith(summand, "-")) {
      out << "+";
    }
    out << summand;
    if (i != this->reduced.size() - 1) {
      out << lineSeparator << "\\displaystyle ";
    }
  }
  return out.str();
}

void PartialFractions::Details::takeSnapShotBeforeElongation() {
  STACK_TRACE("PartialFractions::Details::takeSnapShotBeforeElongation");
  this->snapShotBeforeElongation.makeZero();
  if (!this->owner->nonReduced.isEqualToZero()) {
    global.fatal
    << "Do not call before all fractions are semi-reduced."
    << global.fatal;
  }
  this->snapShotBeforeElongation += this->owner->reduced;
  this->snapShotBeforeElongation +=
  this->owner->reducedWithElongationRedundancies;
}

std::string PartialFractions::HighlightInformation::toStringLinearCombination()
const {
  std::stringstream linearCombinationStream;
  LinearCombination<Vector<Rational>, Rational> combination;
  for (int i = 0; i < this->highlightLinearCombination.size; i ++) {
    combination.addMonomial(
      this->highlightVectors[i], this->highlightLinearCombination[i]
    );
  }
  linearCombinationStream << combination.toString();
  return linearCombinationStream.str();
}

PartialFractions::Snapshot::Snapshot() {
  this->flagTooManySummands = false;
  this->owner = nullptr;
}

std::string PartialFractions::Snapshot::toLatex(
  const std::string& lineSeparator, FormatExpressions* format
) const {
  if (this->flagTooManySummands) {
    return "\\text{...too~many~summands...}";
  }
  if (this->owner == nullptr) {
    return "(corrupted)";
  }
  return
  this->owner->toLatexOneSum(
    this->content, lineSeparator, &this->highlightInformation, format
  );
}

void PartialFractions::Details::addStateBeforeFormula(
  const OnePartialFractionDenominator& toBeReduced,
  const Polynomial<LargeInteger>& numerator,
  const Vector<Rational>& linearDependence,
  const Vectors<Rational>& sortedVectors
) {
  STACK_TRACE("PartialFractions::Details::addStateBeforeFormula");
  if (!this->owner->flagShowDetails) {
    return;
  }
  if (this->snapshots.size > this->maximumIntermediates) {
    return;
  }
  if (this->snapshots.size == this->maximumIntermediates) {
    PartialFractions::Snapshot empty;
    empty.flagTooManySummands = true;
    this->snapshots.addOnTop(empty);
    return;
  }
  PartialFractions::Snapshot snapShot;
  snapShot.owner = this->owner;
  snapShot.content.addMonomial(toBeReduced, numerator);
  this->owner->accumulateSum(snapShot.content);
  snapShot.highlightInformation.toBeReducedNext = toBeReduced;
  snapShot.highlightInformation.highlightVectors = sortedVectors;
  snapShot.highlightInformation.highlightLinearCombination = linearDependence;
  this->snapshots.addOnTop(snapShot);
}

std::string PartialFractions::toLatexPartialFractionDecomposition(
  FormatExpressions* formatDecomposition, FormatExpressions* formatDenominator
) const {
  STACK_TRACE("PartialFractions::toLatexPartialFractionDecomposition");
  const std::string lineSeparator = "\\\\&\n";
  std::string initialExpression;
  List<std::string> intermediates;
  std::string splitExpressionBeforeElongation;
  std::string splitExpression =
  this->toLatexFullSum(lineSeparator, formatDecomposition);
  std::string differentialForm =
  this->toLatexDifferentialOperatorForm(lineSeparator, formatDenominator);
  if (this->details.snapshots.size > 0) {
    // When snapshots are present, the first one is the initial expression.
    initialExpression =
    this->details.snapshots[0].toLatex(lineSeparator, formatDecomposition);
  } else {
    initialExpression =
    this->initialPartialFraction.toLatex(1, formatDecomposition);
  }
  for (int i = 1; i < this->details.snapshots.size; i ++) {
    intermediates.addOnTop(
      this->details.snapshots[i].toLatex(lineSeparator, formatDecomposition)
    );
  }
  if (this->details.flagNeedsElongation) {
    splitExpressionBeforeElongation =
    this->toLatexOneSum(
      this->details.snapShotBeforeElongation,
      lineSeparator,
      nullptr,
      formatDecomposition
    );
  }
  std::stringstream out;
  out << "\\begin{align*}";
  out << "&~~~\n" << initialExpression;
  for (const std::string& intermediate : intermediates) {
    out << "\\\\=&~~~\n" << intermediate;
  }
  if (splitExpressionBeforeElongation != "") {
    out << "\\\\=&~~~\n" << splitExpressionBeforeElongation;
  }
  out << "\\\\=&~~~\n" << splitExpression;
  out << "\\\\=&\n" << differentialForm;
  out << "\\end{align*}";
  return out.str();
}

std::string PartialFractions::toStringLabel() const {
  std::stringstream out;
  if (this->label != "") {
    out << this->label << ":";
  }
  out << this->originalVectors.toStringCommaDelimited();
  return out.str();
}

std::string PartialFractions::toLatexQuasipolynomialTable() const {
  STACK_TRACE("PartialFractions::toLatexQuasipolynomialTable");
  std::stringstream out;
  FormatExpressions format;
  format.flagUseFrac = true;
  format.flagUseLatex = true;
  format.maximumLineLength = 150;
  out << "\\begin{longtable}{|c|c|c|}";
  out << "\\caption{\\footnotesize V.p.f. of ";
  out << this->toStringLabel() << "}\\\\";
  out
  << "\\hline N & Polynomial/Lattice & Shift(s)\\\\ \\hline\n"
  << "\\endfirsthead"
  << "\\multicolumn{3}{c}{{"
  << "\\bfseries \\tablename\\ \\thetable{} -- continued from previous page"
  << "}} \\\\\n"
  << "\\hline  N &  Polynomial/Lattice & Shift(s) \\\\ \\hline\n"
  << "\\endhead"
  << "\\hline \\multicolumn{3}{|c|}{{Continued on next page}} \\\\ \\hline"
  << "\\endfoot"
  << "\\endlastfoot";
  for (int i = 0; i < this->chambers.refinedCones.size(); i ++) {
    Cone& cone = this->chambers.refinedCones.values[i];
    const QuasiPolynomial& quasiPolynomial = cone.payload.getPolynomial();
    out
    << this->toLatexOneQuasipolynomialInTable(
      cone.displayId(), quasiPolynomial, &format
    );
    if (i != this->chambers.refinedCones.size() - 1) {
      out << "\\hline \n";
    }
  }
  out << "\\end{longtable}\n";
  return out.str();
}

std::string PartialFractions::toStringLatticeShiftsOneQuasipolynomial(
  const List<Vector<Rational> >& shifts, bool isInZn
) const {
  if (isInZn) {
    return "-";
  }
  std::stringstream out;
  if (shifts.size == 1) {
    out << "\\(" << shifts[0].toString() << "\\)";
    return out.str();
  }
  out << "\\(";
  out << "\\begin{array}{l}\n";
  out << shifts.toStringWithSeparator("\\\\\n");
  out << "\\end{array}\n";
  out << "\\)";
  return out.str();
}

std::string PartialFractions::toLatexOneQuasipolynomialInTable(
  const std::string& displayId,
  const QuasiPolynomial& input,
  FormatExpressions* format
) const {
  STACK_TRACE("PartialFractions::toLatexOneQuasipolynomialInTable");
  std::stringstream out;
  bool isZn = input.ambientLatticeReduced.basisRationalForm.isIdentity();
  if (!isZn) {
    out << "\\multirow{" << input.latticeShifts.size + 1 << "}";
    out << "{*}{";
    out << displayId;
    out << "}";
  } else {
    out << displayId;
  }
  if (!isZn) {
    out
    << "&\\(\\Lambda="
    << input.ambientLatticeReduced.toString()
    << "\\)&\\\\\\hline";
  }
  MapList<Polynomial<Rational>, List<Vector<Rational> > > combinedShifts;
  input.combineLatticeShifts(combinedShifts);
  Polynomial<Rational> commonPart = combinedShifts.keys[0];
  for (const Polynomial<Rational>& other : combinedShifts.keys) {
    Polynomial<Rational> difference = commonPart - other;
    for (const MonomialPolynomial& monomial : difference.monomials) {
      commonPart.subtractMonomial(
        monomial, commonPart.getCoefficientOf(monomial)
      );
    }
  }
  for (int i = 0; i < combinedShifts.size(); i ++) {
    const Polynomial<Rational>& polynomial = combinedShifts.keys[i];
    const List<Vector<Rational> >& shifts = combinedShifts.values[i];
    out << "&";
    out << "\\(";
    if (combinedShifts.size() == 1 || commonPart.isEqualToZero()) {
      out << polynomial.toStringWithPossibleLineBreak(format);
    } else {
      Polynomial<Rational> difference = polynomial - commonPart;
      std::string sign = "+";
      if (difference.getLeadingCoefficient(nullptr).isNegative()) {
        sign = "";
      }
      out
      << "\\begin{array}{l}"
      << commonPart.toStringWithPossibleLineBreak(format)
      << "\\\\\n"
      << "\\color{red}\n"
      << sign
      << difference.toStringWithPossibleLineBreak(format)
      << "\\color{black}\n"
      << "\\end{array}\n";
    }
    out << "\\)";
    out << "&";
    out << this->toStringLatticeShiftsOneQuasipolynomial(shifts, isZn);
    out << "\\\\\n";
    if (isZn || i == input.latticeShifts.size - 1) {
      out << "\\hline";
    } else {
      out << "\\cline{2-3}";
    }
  }
  return out.str();
}

std::string PartialFractions::toLatexCopyButton() const {
  STACK_TRACE("PartialFractions::toLatexCopyButton");
  std::stringstream out;
  out << "\\documentclass{article}\n";
  out << "\\usepackage{pstricks}\n";
  out << "\\usepackage{auto-pst-pdf}\n";
  out << "\\usepackage{amssymb}\n";
  out << "\\usepackage{amsmath}\n";
  out << "\\usepackage{longtable}\n";
  out << "\\usepackage{multirow}\n";
  out << "\\usepackage{lscape}\n";
  out << "\\usepackage{xcolor}\n";
  out << "\\begin{document}\n\n\n\n";
  out << this->toLatexSelfContainedDocumentBody();
  out << "\\end{document}";
  return HtmlRoutines::toHtmlLatexLiteralWithCopy(out.str());
}

std::string PartialFractions::toLatexRawPartialFractionDecomposition() const {
  STACK_TRACE("PartialFractions::toLatexRawPartialFractionDecomposition");
  std::stringstream out;
  FormatExpressions format;
  format.maximumLineLength = 120;
  format.flagUseFrac = true;
  format.flagUseLatex = true;
  format.flagIsInNumerator = true;
  FormatExpressions formatForDifferentialOperator;
  formatForDifferentialOperator.flagIsInNumerator = false;
  formatForDifferentialOperator.maximumLineLength = 50;
  formatForDifferentialOperator.flagUseLatex = true;
  formatForDifferentialOperator.flagSuppressOneIn1overXtimesY = true;
  formatForDifferentialOperator.flagUseFrac = true;
  out << "\\allowdisplaybreaks";
  out
  << this->toLatexPartialFractionDecomposition(
    &format, &formatForDifferentialOperator
  );
  return out.str();
}

std::string PartialFractions::toLatexSelfContainedDocumentBody() const {
  STACK_TRACE("PartialFractions::toLatexSelfContainedDocumentBody");
  std::stringstream out;
  out << "\\begin{table}[h!]\n";
  out << "\\begin{center}\n";
  bool includeVertices = this->ambientDimension <= 2;
  out << this->chambers.toLatexGraphicsOnlyPsTricks(includeVertices, false);
  out
  << "\\caption{$"
  << this->chambers.refinedCones.size()
  << "$ combinatorial chambers of "
  << this->toStringLabel()
  << "}\n";
  out << "\\end{center}\n";
  out << "\\end{table}\n";
  out << this->toLatexQuasipolynomialTable();
  out << "\n\n";
  out << this->chambers.toLatexWithoutGraphics(this->toStringLabel());
  out << "\n\n";
  out << this->toLatexRawPartialFractionDecomposition();
  return out.str();
}

std::string PartialFractions::toHTML() const {
  STACK_TRACE("PartialFractions::toHTML");
  std::stringstream out;
  out << "Original vectors: " << this->originalVectors.toString();
  out << "<br>";
  out << "Copy self-contained latex document: ";
  out << this->toLatexCopyButton();
  out << "<br>";
  FormatExpressions format;
  format.flagUseFrac = true;
  format.flagSuppressOneIn1overXtimesY = true;
  out << "\\(";
  out << this->toLatexPartialFractionDecomposition(&format, &format);
  out << "\\)";
  out << this->chambers.toHTMLGraphicsOnly(false, false);
  FormatExpressions formatQuasipolynomial;
  formatQuasipolynomial.flagUseFrac = true;
  for (int i = 0; i < this->chambers.refinedCones.size(); i ++) {
    Cone& cone = this->chambers.refinedCones.values[i];
    const QuasiPolynomial& quasiPolynomial = cone.payload.getPolynomial();
    out << "<hr>";
    out << cone.toHTML(this->chambers, true);
    out << "<br>Vector partition function.<br>";
    out << quasiPolynomial.toHTML(&formatQuasipolynomial);
  }
  out << this->toStringCheckSum();
  return out.str();
}

std::string PartialFractions::toStringCheckSum() const {
  STACK_TRACE("PartialFractions::toStringCheckSum");
  std::stringstream out;
  out << "<hr>Checksum. ";
  out << "Obtained by substituting \\(";
  Vector<Polynomial<Rational> > input;
  for (int i = 0; i < this->ambientDimension; i ++) {
    Polynomial<Rational> monomial;
    monomial.makeMonomial(i, 1, 1);
    input.addOnTop(monomial);
  }
  out << input.toString();
  out
  << "\\mapsto "
  << OnePartialFractionDenominatorComponent::getCheckSumRoot(
    this->ambientDimension
  ).toString();
  out
  << "\\) in the starting generating function "
  << "and in the final partial fraction decomposition.";
  out
  << "<br>Checksum at the start: "
  << this->checkSumStart
  << "."
  << "<br>Current checksum: "
  << this->checkSumLast
  << ".";
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
  }
  return out.str();
}

bool PartialFractions::assureIndicatorRegularity(Vector<Rational>& indicator) {
  STACK_TRACE("PartialFractions::assureIndicatorRegularity");
  if (this->ambientDimension <= 1) {
    return true;
  }
  for (const Cone& cone : this->chambers.refinedCones.values) {
    if (cone.isInInterior(indicator)) {
      return true;
    }
    if (cone.isInCone(indicator)) {
      // The point is in the cone but not in the interior, so it is not
      // regular.
      break;
    }
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
  this->statistics.totalFractionsWithAccountedVectorPartitionFunction = 0;
}

bool PartialFractions::splitPartial() {
  STACK_TRACE("PartialFractions::splitPartial");
  OnePartialFractionDenominator currentFraction;
  LinearCombination<OnePartialFractionDenominator, Polynomial<LargeInteger> >
  currentReduction;
  Polynomial<LargeInteger> currentCoefficient;
  while (this->nonReduced.size() > 0) {
    this->nonReduced.popMonomial(0, currentFraction, currentCoefficient);
    bool needsReduction =
    currentFraction.reduceOnce(currentReduction, currentCoefficient);
    if (needsReduction) {
      currentReduction *= currentCoefficient;
      this->nonReduced += currentReduction;
    } else {
      this->reducedWithElongationRedundancies.addMonomial(
        currentFraction, currentCoefficient
      );
    }
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
  const Vector<Rational>& normalizedVector, int multiplicity, int elongation
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
      *this->owner, this->owner->normalizedVectors.getIndex(normalizedVector)
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
  const PartialFractions& owner
) {
  STACK_TRACE(
    "OnePartialFractionDenominator::initializeFromPartialFractions"
  );
  *this = owner.initialPartialFraction;
  return true;
}

PartialFractions::PartialFractions() {
  this->flagSplitTestModeNoNumerators = false;
  this->flagDiscardingFractions = false;
  this->flagUsingCheckSum = true;
  this->flagInitialized = false;
  this->flagMakingProgressReport = true;
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
  int numberOfCutOffs = static_cast<signed>(output.size()) %
  polynomialFormatLocal.maximumLineLength;
  int lastCutOffIndex = 0;
  int numberOfLinesAdded = 0;
  for (int i = 0; i < numberOfCutOffs; i ++) {
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
      if ((output[k] == '+' || output[k] == '-') && output[k - 1] != '{') {
        output.insert(k, "\\\\\n&&");
        numberOfLinesAdded ++;
        lastCutOffIndex = j + 5;
        break;
      }
    }
  }
  return numberOfLinesAdded;
}

void PartialFractions::makeProgressVPFcomputation() {
  this->statistics.totalFractionsWithAccountedVectorPartitionFunction ++;
  if (!global.response.monitoringAllowed()) {
    return;
  }
  std::stringstream out2;
  ProgressReport report;
  out2
  << "Processed "
  << this->statistics.totalFractionsWithAccountedVectorPartitionFunction
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
  this->details.owner = this;
}

void PartialFractions::initializeDimension() {
  if (this->originalVectors.size == 0) {
    this->ambientDimension = - 1;
    return;
  }
  this->ambientDimension = this->originalVectors[0].size;
}

void PartialFractions::initializeInput(const List<Vector<Rational> >& input) {
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
    component.initialize(*this, this->normalizedVectors.getIndex(exponent));
    this->initialPartialFraction.addMultiplicity(exponent, 1, coefficients[i]);
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
  if (rankComputer.getRankLinearSpan() != this->ambientDimension) {
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

void PartialFractions::removeRedundantShortRoots(Vector<Rational>* indicator) {
  STACK_TRACE("PartialFractions::removeRedundantShortRoots");
  Rational startCheckSum;
  Polynomial<LargeInteger> coefficient;
  OnePartialFractionDenominator denominator;
  OnePartialFractionDenominator transformedDenominator;
  Polynomial<LargeInteger> transformedCoefficient;
  bool found = false;
  while (this->reducedWithElongationRedundancies.size() > 0) {
    this->reducedWithElongationRedundancies.popMonomial(
      0, denominator, coefficient
    );
    bool needsMoreReduction =
    this->reduceOnceRedundantShortRoots(
      denominator, transformedDenominator, transformedCoefficient, indicator
    );
    if (needsMoreReduction) {
      found = true;
      transformedCoefficient *= coefficient;
      this->reducedWithElongationRedundancies.addMonomial(
        transformedDenominator, transformedCoefficient
      );
    } else {
      this->reduced.addMonomial(denominator, coefficient);
    }
  }
  if (found) {
    this->details.flagNeedsElongation = true;
  }
  this->compareCheckSums();
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
  Rational accumulator = 0;
  for (int i = 0; i < left.size; i ++) {
    accumulator += (left[i] - r[i]) * weights[i];
  }
  return accumulator > 0;
}

unsigned int OnePartialFractionDenominatorComponent::hashFunction(
  const OnePartialFractionDenominatorComponent& input
) {
  return input.hashFunction();
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
      global.fatal << "Elongation repeats. " << global.fatal;
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
    result = MathRoutines::leastCommonMultiple(this->elongations[i], result);
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
getDenominatorExponentsWithoutMultiplicities(List<Vector<Rational> >& output)
const {
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
  STACK_TRACE("OnePartialFractionDenominatorComponent::computeOneCheckSum");
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

int OnePartialFractionDenominatorComponent::getMultiplicityLargestElongation(
) {
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
  if (multiplicity > 1 && multiplicity < 10) {
    out << "^" << multiplicity;
  } else if (multiplicity >= 10) {
    out << "^{" << multiplicity << "}";
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
  Vector<Rational> buffer1;
  Vector<Rational> buffer2;
  Vector<Rational> buffer3;
  buffer1.setSize(dimension);
  buffer2.setSize(dimension);
  buffer3.setSize(dimension);
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
        buffer1[k] =
        this->originalVectors[i][k] - this->originalVectors[j][k];
        buffer2[k] =
        this->originalVectors[i][k] - this->originalVectors[j][k] * 2;
      }
      this->tableAllowedAminusB.elements[i][j] = this->getIndex(buffer1);
      this->tableAllowedAminus2B.elements[i][j] = this->getIndex(buffer2);
    }
    buffer3 = this->originalVectors[i];
    buffer3 *= 2;
    this->indicesDoublesOfRedundantShortRoots[i] = this->getIndex(buffer3);
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

void SelectionWithMultiplicities::initWithMultiplicities(
  int numberOfElements
) {
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

void SelectionWithMaximumMultiplicity::initializeMaximumMultiplicity(
  int numberOfElements, int maximulMultiplicity
) {
  this->::SelectionWithMultiplicities::initWithMultiplicities(
    numberOfElements
  );
  this->maximumMultiplicity = maximulMultiplicity;
}

int::SelectionWithMaximumMultiplicity::cardinalitySelectionWithMultiplicities()
{
  int result = 0;
  for (int i = 0; i < this->multiplicities.size; i ++) {
    result += this->multiplicities[i];
  }
  return result;
}

bool SelectionWithMaximumMultiplicity::hasMultiplicitiesZeroAndOneOnly() {
  for (int i = 0; i < this->elements.size; i ++) {
    if (this->multiplicities[elements[i]] > 1) {
      return false;
    }
  }
  return true;
}

bool SelectionWithMaximumMultiplicity::incrementSubsetFixedCardinality(
  int cardinality
) {
  if (
    cardinality < 1 ||
    cardinality > this->maximumMultiplicity * this->multiplicities.size
  ) {
    return false;
  }
  if (this->cardinalitySelectionWithMultiplicities() != cardinality) {
    this->multiplicities.initializeFillInObject(this->multiplicities.size, 0);
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
    return true;
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
    return false;
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
    int i = this->multiplicities.size - 1; currentCardinality < cardinality;
    i --
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
  return true;
}

LargeInteger SelectionWithMaximumMultiplicity::
numberOfCombinationsOfCardinality(int cardinality) {
  return
  MathRoutines::nChooseK(
    this->multiplicities.size + cardinality - 1, cardinality
  );
}

LargeInteger SelectionWithMaximumMultiplicity::getTotalCombinationCount() const {
  // if (this->maximumMultiplicity == 0)
  //  return 1;
  LargeInteger result;
  MathRoutines::kToTheNth(
    this->maximumMultiplicity + 1, this->multiplicities.size, result
  );
  return result;
}

bool SelectionWithMaximumMultiplicity::incrementReturnFalseIfPastLast() {
  this->incrementSubset();
  return this->elements.size != 0;
}

void SelectionWithMaximumMultiplicity::incrementSubset() {
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
      << "all subsets of a multi-set, however "
      << "the number of subsets is larger than  "
      << "the maximum value allowed for int on the system "
      << "(on a 32 bit machine that is around 2 billion). "
      << "This can be fixed, however I do "
      << "not have time at the moment. If you "
      << "encounter this error, write me an email "
      << "and I will take the time to fix this issue. "
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

bool SelectionWithDifferentMaxMultiplicities::incrementReturnFalseIfPastLast(
) {
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
  MatrixTensor<Rational> directSummand;
  MatrixTensor<Rational> finalMatrix;
  if (
    dynkinType.letter == 'D' || (
      dynkinType.letter == 'A' && dynkinType.rank > 1
    ) || (dynkinType.letter == 'E' && dynkinType.rank == 6)
  ) {
    directSummand.makeIdentity(dynkinType.rank *(multiplicity - 1));
    int numberOfGenerators = 1;
    if (dynkinType.letter == 'D' && dynkinType.rank == 4) {
      numberOfGenerators = 2;
    }
    for (int i = 1; i < numberOfGenerators + 1; i ++) {
      dynkinType.getAutomorphismActingOnVectorColumn(finalMatrix, i);
      finalMatrix.directSumWith(directSummand);
      output.addOnTop(finalMatrix);
    }
  }
  if (multiplicity < 2) {
    return;
  }
  for (int i = 0; i < multiplicity - 1; i ++) {
    directSummand.makeZero();
    for (int j = 0; j < dynkinType.rank; j ++) {
      directSummand.addMonomial(MonomialMatrix(j, dynkinType.rank + j), 1);
      directSummand.addMonomial(MonomialMatrix(dynkinType.rank + j, j), 1);
    }
    finalMatrix.makeIdentity(i * dynkinType.rank);
    finalMatrix.directSumWith(directSummand);
    directSummand.makeIdentity((multiplicity - 2 - i) * dynkinType.rank);
    finalMatrix.directSumWith(directSummand);
    output.addOnTop(finalMatrix);
  }
}

void DynkinType::getOuterAutosGeneratorsActOnVectorColumn(
  List<MatrixTensor<Rational> >& output
) {
  STACK_TRACE("DynkinType::getOuterAutosGeneratorsActOnVectorColumn");
  this->sortDynkinTypes();
  List<MatrixTensor<Rational> > intermediateGenerators;
  MatrixTensor<Rational> matrixFinal;
  MatrixTensor<Rational> matrixToGo;
  int currentMultiplicity = 0;
  output.setSize(0);
  int numberOfRowsSoFar = 0;
  for (int i = 0; i < this->size(); i ++) {
    if (!this->coefficients[i].isSmallInteger(&currentMultiplicity)) {
      global.fatal
      << "This is not supposed to happen in function "
      << "DynkinType::getOuterAutosGeneratorsActOnVectorColumn."
      << global.fatal;
    }
    this->getOuterAutosGeneratorsOneTypeActOnVectorColumn(
      intermediateGenerators, (*this)[i], currentMultiplicity
    );
    matrixToGo.makeIdentity(
      this->getRank() -
      numberOfRowsSoFar -
      currentMultiplicity *(*this)[i].rank
    );
    for (int j = 0; j < intermediateGenerators.size; j ++) {
      matrixFinal.makeIdentity(numberOfRowsSoFar);
      matrixFinal.directSumWith(intermediateGenerators[j]);
      matrixFinal.directSumWith(matrixToGo);
      output.addOnTop(matrixFinal);
    }
    numberOfRowsSoFar += currentMultiplicity *(*this)[i].rank;
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

bool DynkinType::canBeExtendedParabolicallyOrIsEqualTo(const DynkinType& other)
const {
  STACK_TRACE("DynkinType::canBeExtendedParabolicallyOrIsEqualTo");
  if (this->isEqualToZero()) {
    return true;
  }
  if (other.isEqualToZero()) {
    return false;
  }
  const DynkinSimpleType& currentSimpleType = (*this)[0];
  DynkinType remainderThis;
  DynkinType remainderOther;
  for (int i = 0; i < other.size(); i ++) {
    if (!currentSimpleType.canBeExtendedParabolicallyOrIsEqualTo(other[i])) {
      continue;
    }
    remainderThis = *this;
    remainderThis.subtractMonomial(currentSimpleType, 1);
    remainderOther = other;
    remainderOther.subtractMonomial(other[i], 1);
    if (remainderThis.canBeExtendedParabolicallyOrIsEqualTo(remainderOther)) {
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
    DynkinType typeMinusMinimal = (*this);
    typeMinusMinimal.subtractMonomial(minimalComponent, 1);
    List<DynkinSimpleType> simpleTypes;
    List<List<int> > lastComponentRootInjections;
    minimalComponent.grow(simpleTypes, &lastComponentRootInjections);
    currentRootInjection.setSize(this->getRank() + 1);
    for (int i = 0; i < simpleTypes.size; i ++) {
      bool isGood = true;
      for (int j = 0; j < typeMinusMinimal.size(); j ++) {
        if (simpleTypes[i] > typeMinusMinimal[j]) {
          isGood = false;
          break;
        }
      }
      if (!isGood) {
        continue;
      }
      output.addOnTop(typeMinusMinimal);
      output.lastObject()->addMonomial(simpleTypes[i], 1);
      if (outputPermutationRoots != nullptr) {
        int baseTypeRank = typeMinusMinimal.getRank();
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
  char currentType = 0;
  int currentRank = 0;
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

int DynkinType::getMultiplicity(int simpleTypeIdentifier) const {
  int result = 0;
  if (!this->coefficients[simpleTypeIdentifier].isSmallInteger(&result)) {
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
    << "Dynkin type has a number of simple "
    << "components which is not a small integer. "
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
    << "Dynkin type has a number of simple "
    << "components which is not a small integer. "
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
  Matrix<Rational> currentCartan;
  List<DynkinSimpleType> sortedMonomials;
  this->getSortedDynkinTypes(sortedMonomials);
  for (int j = 0; j < sortedMonomials.size; j ++) {
    int index = this->monomials.getIndex(sortedMonomials[j]);
    int multiplicity = this->getMultiplicity(index);
    for (int k = 0; k < multiplicity; k ++) {
      DynkinSimpleType::getEpsilonMatrix((*this)[index].letter, (*this)[
          index
        ].rank,
        currentCartan
      );
      output.directSumWith(currentCartan);
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
  List<DynkinSimpleType> sortedMonomials;
  this->getSortedDynkinTypes(sortedMonomials);
  for (int j = 0; j < sortedMonomials.size; j ++) {
    int index = this->monomials.getIndex(sortedMonomials[j]);
    int multiplicity = this->getMultiplicity(index);
    for (int k = 0; k < multiplicity; k ++) {
      (*this)[index].getCartanSymmetric(currentCartan);
      output.directSumWith(currentCartan);
    }
  }
}

void DynkinType::getCoCartanSymmetric(Matrix<Rational>& output) const {
  STACK_TRACE("DynkinType::getCoCartanSymmetric");
  Matrix<Rational> currentCartan;
  this->getCartanSymmetric(currentCartan);
  WeylGroupData::getCoCartanSymmetric(currentCartan, output);
}

void DynkinType::getCartanSymmetricDefaultLengthKeepComponentOrder(
  Matrix<Rational>& output
) const {
  STACK_TRACE(
    "DynkinType::"
    "getCartanSymmetricDefaultLengthKeepComponentOrder"
  );
  output.initialize(0, 0);
  Matrix<Rational> currentCartan;
  List<DynkinSimpleType> sortedMonomials;
  this->getSortedDynkinTypes(sortedMonomials);
  DynkinSimpleType currentType;
  for (int j = 0; j < sortedMonomials.size; j ++) {
    int index = this->monomials.getIndex(sortedMonomials[j]);
    int multiplicity = this->getMultiplicity(index);
    currentType.makeArbitrary(
      sortedMonomials[j].letter, sortedMonomials[j].rank, 1
    );
    currentType.cartanSymmetricInverseScale = 1;
    // = currentType.getDefaultCoRootLengthSquared(0);
    for (int k = 0; k < multiplicity; k ++) {
      currentType.getCartanSymmetric(currentCartan);
      output.directSumWith(currentCartan);
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
  << "If you would like an edge weight of a "
  << "non-crystallographic Coxeter graph, replace the code near "
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
  LargeInteger largeInteger;
  for (int i = 0; i < this->size(); i ++) {
    largeInteger =
    WeylGroupData::sizeByFormulaOrNegative1((*this)[i].letter, (*this)[i].rank
    );
    largeInteger.raiseToPower(this->getMultiplicity(i));
    result *= largeInteger;
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
  << "DynkinSimpleType::getEpsilonRealizationLongRootLengthSquared "
  << "on a non-initialized simple type. "
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

std::string DynkinSimpleType::toMathML(
  FormatExpressions* format, MathMLExpressionProperties* outputProperties
) const {
  (void) outputProperties;
  bool hasAmbient = false;
  if (format != nullptr) {
    hasAmbient = (format->ambientWeylLetter != 'X');
  }
  bool supressDynkinIndexOne = format ==
  nullptr ? false : format->flagSupressDynkinIndexOne;
  Rational dynkinIndex = this->cartanSymmetricInverseScale;
  if (hasAmbient) {
    DynkinSimpleType ambientType;
    ambientType.letter = format->ambientWeylLetter;
    ambientType.cartanSymmetricInverseScale =
    format->ambientCartanSymmetricInverseScale;
    dynkinIndex = ambientType.getLongRootLengthSquared() /
    this->getLongRootLengthSquared();
  }
  bool subscriptOnly = supressDynkinIndexOne && dynkinIndex.isEqualToOne();
  std::stringstream out;
  if (subscriptOnly) {
    out << "<msub>";
  } else {
    out << "<msubsup>";
  }
  out << "<mi>" << this->letter << "</mi>";
  out << "<mn>" << this->rank << "</mn>";
  if (subscriptOnly) {
    out << "</msub>";
  } else {
    out << "<mn>" << dynkinIndex << "</mn>";
    out << "</msubsup>";
  }
  return out.str();
}

std::string DynkinSimpleType::toMathMLFinal(FormatExpressions* format) const {
  return MathML::toMathMLFinal(this->toMathML(format), this->toString(format));
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
      out << this->letter;
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
      out << this->letter;
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
  const Rational& inputLengthFirstCoRootSquared
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
  this->cartanSymmetricInverseScale = inputLengthFirstCoRootSquared;
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
    << "DynkinSimpleType::getDefaultRootLengthSquared "
    << "on the non-initialized Dynkin type "
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
      output(4, 6) = 1;
      output(5, 6) = - 1;
    }
    if (weylRank > 7) {
      // 8th simple root: e_6-e_7
      output(5, 7) = 1;
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
  List<DynkinSimpleType>& output, List<List<int> >* outputPermutationRoots
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
  if ((this->letter == 'B' || this->letter == 'C') && other.letter == 'D') {
    return true;
  }
  if (this->letter == 'D' && (other.letter == 'B' || other.letter == 'C')) {
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

void ElementSubgroupWeylGroupAutomorphisms::multiplyOnTheRightBySimpleRootInner
(int simpleRootIndex) {
  SimpleReflectionOrOuterAutomorphism generator;
  generator.index = simpleRootIndex;
  this->generatorsLastAppliedFirst.addOnTop(generator);
}

void ElementSubgroupWeylGroupAutomorphisms::
multiplyOnTheRightByOuterAutomorphism(int indexOuterAutomorphism) {
  SimpleReflectionOrOuterAutomorphism generator;
  generator.index = indexOuterAutomorphism;
  generator.flagIsOuter = true;
  this->generatorsLastAppliedFirst.addOnTop(generator);
}

unsigned int ElementSubgroupWeylGroupAutomorphisms::hashFunction(
  const ElementSubgroupWeylGroupAutomorphisms& input
) {
  return input.hashFunction();
}

unsigned int ElementSubgroupWeylGroupAutomorphisms::hashFunction() const {
  return this->generatorsLastAppliedFirst.hashFunction();
}

std::string ElementSubgroupWeylGroupAutomorphisms::toString(
  FormatExpressions* format
) const {
  STACK_TRACE(
    "ElementSubgroupWeylGroupAutomorphisms"
    "GeneratedByRootReflectionsAndAutomorphisms::toString"
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
  const ElementWeylGroup& conjugateWith,
  const ElementWeylGroup& conjugateOn,
  ElementWeylGroup& out
) {
  out = conjugateOn ^ conjugateWith;
}

void ElementWeylGroup::operator*=(const ElementWeylGroup& other) {
  if (this->owner != other.owner) {
    global.fatal
    << "Attempt to "
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

void ElementWeylGroup::makeRootReflection(
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
  Polynomial<Rational> leftCharPoly;
  Polynomial<Rational> rightCharPoly;
  this->getCharacteristicPolynomialStandardRepresentation(leftCharPoly);
  right.getCharacteristicPolynomialStandardRepresentation(rightCharPoly);
  if (leftCharPoly != rightCharPoly) {
    return true;
  }
  VectorSparse<Rational> leftCycleStructure;
  VectorSparse<Rational> rightCycleStructure;
  this->getCycleStructure(leftCycleStructure);
  right.getCycleStructure(rightCycleStructure);
  if (leftCycleStructure != rightCycleStructure) {
    return true;
  }
  return false;
}

std::string ElementWeylGroup::toStringInvariants(FormatExpressions* format)
const {
  STACK_TRACE("ElementWeylGroup::toStringInvariants");
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
  VectorSparse<Rational>& outputIndexIsCycleSizeCoordinateIsCycleMultiplicity
) const {
  STACK_TRACE("ElementWeylGroup::getCycleStructure");
  this->checkInitialization();
  outputIndexIsCycleSizeCoordinateIsCycleMultiplicity.makeZero();
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
      outputIndexIsCycleSizeCoordinateIsCycleMultiplicity.addMonomial(
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
  Matrix<Rational> standardRepresentationMatrix;
  this->owner->getMatrixStandardRepresentation(
    *this, standardRepresentationMatrix
  );
  output.assignCharacteristicPolynomial(standardRepresentationMatrix);
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
    scalarProduct.multiplyBy(this->cartanSymmetric.elements[index][i] *(- 2));
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
    scalar.multiplyBy(this->cartanSymmetric.elements[index][i] *(- 2));
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
  Polynomial<Rational> aScalarB;
  Polynomial<Rational> polynomialCoefficient;
  aScalarB.makeZero();
  for (int i = 0; i < this->cartanSymmetric.numberOfColumns; i ++) {
    polynomialCoefficient.makeZero();
    polynomialCoefficient = root[i];
    polynomialCoefficient *= cartanSymmetric.elements[index][i];
    aScalarB += (polynomialCoefficient);
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
  this->fundamentalToSimpleCoordinates.initialize(0, 0);
  this->simpleToFundamentalCoordinates.initialize(0, 0);
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
  int numberOfGenerators =
  this->group.elements[index].generatorsLastAppliedFirst.size;
  for (int i = numberOfGenerators - 1; i >= 0; i --) {
    this->reflectSimple(
      this->group.elements[index].generatorsLastAppliedFirst[i].index,
      output.affinePoint,
      rhoAction,
      useMinusRho
    );
    this->simpleReflectionDualSpace(
      this->group.elements[index].generatorsLastAppliedFirst[
        numberOfGenerators - i - 1
      ].index,
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
  STACK_TRACE("WeylGroupData::getTrivialRepresentation");
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
  STACK_TRACE("WeylGroupData::getSignRepresentation");
  this->group.checkInitializationConjugacyClasses();
  output.initialize(this->group);
  output.basis.makeEiBasis(1);
  ElementWeylGroup currentElement;
  for (int i = 0; i < this->getDimension(); i ++) {
    currentElement.makeSimpleReflection(i, *this);
    output.generators[i].makeIdentityMatrix(1);
    output.generators[i] *= currentElement.sign();
  }
  output.getCharacter();
}

void WeylGroupData::getStandardRepresentation(
  GroupRepresentationCarriesAllMatrices<
    FiniteGroup<ElementWeylGroup>, Rational
  >& output
) {
  STACK_TRACE("WeylGroupData::getStandardRepresentation");
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
    this->cartanSymmetric.numberOfRows, this->cartanSymmetric.numberOfRows
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
    Rational currentMaximumMovement;
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
        currentMaximumMovement = scalarProduct2 / scalarProduct1;
        currentMaximumMovement.AssignAbsoluteValue();
        if ((currentMaximumMovement < maxMovement) ||
          maxMovement.isEqualToZero()
        ) {
          maxMovement = currentMaximumMovement;
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
    output =
    Rational::factorial(dimension) * Rational::twoToTheNth(dimension);
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
  out.setSize(this->group.elements[g].generatorsLastAppliedFirst.size);
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
  const ElementWeylGroup& currentElement = this->group.elements[index];
  for (
    int i = currentElement.generatorsLastAppliedFirst.size - 1; i >= 0; i --
  ) {
    this->reflectSimple(
      currentElement.generatorsLastAppliedFirst[i].index,
      root,
      rhoAction,
      useMinusRho
    );
  }
}

void WeylGroupData::getCoCartanSymmetric(
  const Matrix<Rational>& input, Matrix<Rational>& output
) {
  STACK_TRACE("WeylGroupData::getCoCartanSymmetric");
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
    int i = this->group.elements[index].generatorsLastAppliedFirst.size - 1;
    i >= 0; i --
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
  STACK_TRACE("WeylGroupData::toStringCppCharTable");
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
    for (int j = 0; j < this->group.characterTable[i].data.size; j ++) {
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
  STACK_TRACE("WeylGroupData::toStringCppConjugacyClasses");
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
    for (size_t j = (Rational(i)).toString().size(); j < 3; j ++) {
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
    for (size_t j = (Rational(i)).toString().size(); j < 3; j ++) {
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
  STACK_TRACE("WeylGroupData::toStringRootsAndRootReflections");
  (void) format;
  // portable way to avoid non-used parameter warning.
  std::stringstream out;
  std::stringstream outLatex;
  out << "<br>The root system has " << this->rootSystem.size << " elements.\n";
  out
  << "<table><tr><td>Simple basis coordinates</td><td>Epsilon coordinates</td>"
  << "<td>Reflection w.r.t. root</td></tr>";
  Vectors<Rational> rootSystemEpsilonCoordinates;
  this->getEpsilonCoordinates(this->rootSystem, rootSystemEpsilonCoordinates);
  ElementWeylGroup currentRootReflection;
  for (int i = 0; i < this->rootSystem.size; i ++) {
    const Vector<Rational>& current = this->rootSystem[i];
    currentRootReflection.makeRootReflection(current, *this);
    out
    << "<tr><td>"
    << current.toString()
    << "</td><td>"
    << rootSystemEpsilonCoordinates[i].toStringLetterFormat("e")
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
  this->getCoCartanSymmetric(this->cartanSymmetric, this->coCartanSymmetric);
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
  if (&inputType == &this->dynkinType) {
    DynkinType typeCopy = inputType;
    this->makeFromDynkinType(typeCopy);
    return;
  }
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
  DynkinDiagramRootSubalgebra dynkinType;
  Vectors<Rational> simpleBasis;
  Vectors<Rational> coordinatesInNewBasis;
  simpleBasis = generators;
  dynkinType.computeDiagramTypeModifyInput(simpleBasis, *this);
  bool hasGenerators = true;
  if (generators.size == 0) {
    hasGenerators = false;
  }
  if (!hasGenerators) {
    output.setSize(input.size);
    for (int i = 0; i < input.size; i ++) {
      output[i].makeZero(0);
    }
    return;
  }
  basisChange.resize(0, 0, true);
  for (int i = 0; i < dynkinType.simpleComponentTypes.size; i ++) {
    DynkinSimpleType::getEpsilonMatrix(
      dynkinType.simpleComponentTypes[i].letter,
      dynkinType.simpleComponentTypes[i].rank,
      epsilonMatrix
    );
    basisChange.directSumWith(epsilonMatrix, Rational(0));
  }
  simpleBasis.assignListList(dynkinType.simpleBasesConnectedComponents);
  coordinatesInNewBasis.setSize(input.size);
  for (int i = 0; i < input.size; i ++) {
    input[i].coordinatesInBasisNoFailure(
      simpleBasis, coordinatesInNewBasis[i]
    );
  }
  basisChange.actOnVectorsColumn(coordinatesInNewBasis, output);
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
  FiniteGroup<ElementWeylGroup>& g
) {
  WeylGroupData* weylGroupData = static_cast<WeylGroupData*>(g.specificDataPointer);
  weylGroupData->checkConsistency();
  return weylGroupData->dynkinType.getWeylGroupSizeByFormula();
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
    this->rootScalarCartanRoot(this->rootSystem[i], this->rootSystem[0]);
    result += scalar * scalar;
  }
  result /=
  this->rootScalarCartanRoot(this->rootSystem[0], this->rootSystem[0]);
  return result;
}

void WeylGroupData::getLongestWeylElement(
  ElementWeylGroup& outputWeylElement
) {
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
  ElementWeylGroup elementSimpleReflection;
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
          elementSimpleReflection.makeSimpleReflection(i, *this);
          *outputWeylElement = elementSimpleReflection *(*outputWeylElement);
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
  STACK_TRACE(
    "WeylGroupAutomorphisms::"
    "isElementWeylGroupOrOuterAutomorphisms"
  );
  this->computeOuterAutomorphisms();
  Vector<Rational> rhoImage;
  matrix.actOnVectorColumn(this->weylGroup->rho, rhoImage);
  ElementWeylGroup elementCandidate;
  this->weylGroup->raiseToDominantWeight(
    rhoImage, nullptr, nullptr, &elementCandidate
  );
  Matrix<Rational> candidateMatrix;
  MatrixTensor<Rational> candidateMatrixTensorForm;
  MatrixTensor<Rational> candidateMatrixWithOuterAutomorphisms;
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
  Matrix<Rational> matrixCoxeterElement;
  Matrix<Rational> matrix;
  this->getMatrixStandardRepresentation(element, matrixCoxeterElement);
  matrix = matrixCoxeterElement;
  int coxeterNumber =
  this->rootSystem.lastObject()->sumCoordinates().numeratorShort + 1;
  for (int i = 0; i < coxeterNumber - 1; i ++) {
    matrix.multiplyOnTheLeft(matrixCoxeterElement);
  }
  Complex<double> eigenValue;
  eigenValue.realPart =
  FloatingPoint::cosFloating(2 * MathRoutines::pi() / coxeterNumber);
  eigenValue.imaginaryPart =
  FloatingPoint::sinFloating(2 * MathRoutines::pi() / coxeterNumber);
  Matrix<Complex<double> > eigenMatrix;
  eigenMatrix.initialize(
    matrixCoxeterElement.numberOfRows, matrixCoxeterElement.numberOfColumns
  );
  for (int i = 0; i < eigenMatrix.numberOfRows; i ++) {
    for (int j = 0; j < eigenMatrix.numberOfColumns; j ++) {
      eigenMatrix.elements[i][j] =
      matrixCoxeterElement.elements[i][j].getDoubleValue();
      if (i == j) {
        eigenMatrix.elements[i][i] -= eigenValue;
      }
    }
  }
  List<Vector<Complex<double> > > eigenSpaceList;
  eigenMatrix.getZeroEigenSpace(eigenSpaceList);
  Vectors<Complex<double> > eigenSpace;
  outputBasis1.setSize(dimension);
  outputBasis2.setSize(dimension);
  eigenSpace.operator=(eigenSpaceList);
  DrawingVariables drawingOperations;
  drawingOperations.initializeDimensions(dimension);
  drawingOperations.graphicsUnit = DrawingVariables::graphicsUnitDefault;
  eigenSpace.operator=(eigenSpaceList);
  for (int i = 0; i < dimension; i ++) {
    for (int j = 0; j < dimension; j ++) {
      drawingOperations.bilinearForm.elements[i][j] =
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
    drawingOperations.modifyToOrthonormalNoShiftSecond(
      outputBasis2, outputBasis1
    );
  }
}

void WeylGroupData::drawRootSystem(
  DrawingVariables& outputDrawingVariables,
  bool wipeCanvas,
  bool drawWeylChamber,
  Vector<Rational>* bluePoint,
  bool labelDynkinDiagramVertices,
  Vectors<Rational>* predefinedProjectionPlane
) {
  STACK_TRACE("WeylGroupData::drawRootSystem");
  DrawingVariables& output = outputDrawingVariables;
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
  output.initializeDimensions(dimension);
  output.graphicsUnit = DrawingVariables::graphicsUnitDefault;
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
    double length1 =
    this->rootScalarCartanRoot(root, output.basisProjectionPlane[0]);
    double length2 =
    this->rootScalarCartanRoot(root, output.basisProjectionPlane[1]);
    lengths[i] =
    FloatingPoint::sqrtFloating(length1 * length1 + length2 * length2);
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
  Rational minimalLength =
  this->rootScalarCartanRoot(differenceRoot, differenceRoot);
  for (int i = 2; i < rootSystemSorted.size; i ++) {
    differenceRoot = rootSystemSorted[0] - rootSystemSorted[i];
    if (
      minimalLength >
      this->rootScalarCartanRoot(differenceRoot, differenceRoot)
    ) {
      minimalLength =
      this->rootScalarCartanRoot(differenceRoot, differenceRoot);
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
    weylChamber.drawMeProjective(outputDrawingVariables);
  }
  output.centerX = 300;
  output.centerY = 300;
  Rational scalarProduct;
  for (int i = 0; i < rootSystemSorted.size; i ++) {
    output.drawLineBetweenTwoVectorsBufferRational(
      zeroRoot, rootSystemSorted[i], "green", 1
    );
    output.drawCircleAtVectorBufferRational(rootSystemSorted[i], "#ff00ff", 2);
    for (int j = i + 1; j < rootSystemSorted.size; j ++) {
      differenceRoot = rootSystemSorted[i] - rootSystemSorted[j];
      scalarProduct =
      this->rootScalarCartanRoot(differenceRoot, differenceRoot);
      if (minimalLength == scalarProduct) {
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
      output.drawTextAtVector(
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
  std::stringstream currentStream;
  currentStream << this->dynkinType.getWeylGroupName();
  if (this->getDimension() == 2 && predefinedProjectionPlane != nullptr) {
    twoPlane[1][0] = 1;
    twoPlane[1][1] = 0;
    twoPlane[0][0] = 0;
    twoPlane[0][1] = 1;
    outputDrawingVariables.modifyToOrthonormalNoShiftSecond(
      twoPlane[0], twoPlane[1]
    );
  }
}

std::string WeylGroupData::generateWeightSupportMethod1(
  Vector<Rational>& highestWeightSimpleCoordinates,
  Vectors<Rational>& outputWeightsSimpleCoordinates,
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
  Vector<Rational> highestWeightTrue = highestWeightSimpleCoordinates;
  this->raiseToDominantWeight(highestWeightTrue);
  std::stringstream out;
  if (highestWeightTrue != highestWeightSimpleCoordinates) {
    out
    << "<br>The input weight is not highest... "
    << "using the highest weight in the same orbit instead. "
    << "Your input in simple coordinates was: "
    << highestWeightSimpleCoordinates.toString()
    << ".<br> ";
  }
  out
  << "The highest weight in simple coordinates is: "
  << highestWeightTrue.toString()
  << ".<br>";
  std::string currentString;
  bool isTrimmed =
  !this->getAllDominantWeightsHWFDIM(
    highestWeightSimpleCoordinates,
    dominantWeights,
    upperBoundInt,
    &currentString
  );
  out << currentString << "<br>";
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
    dominantWeightsNonHashed, false, finalWeights, false, 0, nullptr, 10000
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
  outputWeightsSimpleCoordinates = finalWeights;
  return out.str();
}

bool WeylGroupData::isEigenSpaceGeneratorCoxeterElement(
  Vector<Rational>& input
) {
  ElementWeylGroup element;
  this->getCoxeterElement(element);
  Matrix<Rational> matrixCoxeterElement;
  this->getMatrixStandardRepresentation(element, matrixCoxeterElement);
  Vector<Rational> root = input;
  for (int i = 0; i < this->getDimension(); i ++) {
    matrixCoxeterElement.actOnVectorColumn(root);
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

bool WeylGroupData::isElementWeylGroup(const MatrixTensor<Rational>& matrix) {
  STACK_TRACE("WeylGroupData::isElementWeylGroup");
  Vector<Rational> rhoImage;
  matrix.actOnVectorColumn(this->rho, rhoImage);
  ElementWeylGroup elementCandidate;
  this->raiseToDominantWeight(rhoImage, nullptr, nullptr, &elementCandidate);
  Matrix<Rational> candidateMatrix;
  Matrix<Rational> inputMatrix;
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
  bool graphWidthIsOdd = ((graphWidth % 2) != 0);
  if (!graphWidthIsOdd) {
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
        << this->representativesQuotientAmbientOrder[layers[i][j]].toString();
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
  int graphWidth = 1;
  int oldLayerElementLength = - 1;
  for (int i = 0; i < this->allElements.size; i ++) {
    const ElementSubgroupWeylGroupAutomorphisms& currentElement =
    this->allElements[i];
    if (
      currentElement.generatorsLastAppliedFirst.size != oldLayerElementLength
    ) {
      layers.setSize(layers.size + 1);
      oldLayerElementLength = currentElement.generatorsLastAppliedFirst.size;
    }
    layers.lastObject()->addOnTop(i);
    graphWidth = MathRoutines::maximum(graphWidth, layers.lastObject()->size);
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
        const ElementSubgroupWeylGroupAutomorphisms& currentElement =
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
  return this->toStringFromLayersAndArrows(arrows, layers, graphWidth, false);
}

void SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms::
toString(std::string& output, bool displayElements) {
  STACK_TRACE(
    "SubgroupWeylGroupAutomorphismsGeneratedByRootReflectionsAndAutomorphisms"
    "::toString"
  );
  this->checkInitialization();
  std::stringstream out;
  std::stringstream head;
  std::stringstream head2;
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
  DynkinDiagramRootSubalgebra dynkinDiagramRootSubalgebra;
  dynkinDiagramRootSubalgebra.ambientRootSystem =
  this->ambientWeyl->rootSystem;
  dynkinDiagramRootSubalgebra.ambientBilinearForm =
  this->ambientWeyl->cartanSymmetric;
  dynkinDiagramRootSubalgebra.computeDiagramInputIsSimpleBasis(
    this->simpleRootsInner
  );
  out
  << "Dynkin diagram & subalgebra of "
  << "root subsystem generated by the given root: "
  << dynkinDiagramRootSubalgebra.toString();
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
      automorphismMatrix.assignVectorsToRows(this->externalAutomorphisms[i]);
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
      const ElementSubgroupWeylGroupAutomorphisms& currentElement =
      this->allElements[i];
      body << currentElement.toString(nullptr) << "\\\\";
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
  List<Vectors<Rational> > rootsCollections;
  return
  this->computeSubGroupFromGeneratingReflections(
    &selectedRoots, &rootsCollections, upperLimitNumberOfElements, true
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
  layers.reserve(this->representativesQuotientAmbientOrder.size);
  int graphWidth = 1;
  int oldLayerElementLength = - 1;
  for (int i = 0; i < this->representativesQuotientAmbientOrder.size; i ++) {
    if (
      this->representativesQuotientAmbientOrder[i].generatorsLastAppliedFirst.
      size !=
      oldLayerElementLength
    ) {
      layers.setSize(layers.size + 1);
      oldLayerElementLength =
      this->representativesQuotientAmbientOrder[i].generatorsLastAppliedFirst.
      size;
    }
    layers.lastObject()->addOnTop(i);
    graphWidth = MathRoutines::maximum(graphWidth, layers.lastObject()->size);
  }
  for (int i = 0; i < this->representativesQuotientAmbientOrder.size; i ++) {
    root = this->ambientWeyl->rho;
    this->ambientWeyl->actOnRootByGroupElement(
      this->ambientWeyl->group.elements.getIndex(
        this->representativesQuotientAmbientOrder[i]
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
        int k = 0; k < this->representativesQuotientAmbientOrder.size; k ++
      ) {
        if (
          this->ambientWeyl->leftIsHigherInBruhatOrderThanRight(
            this->representativesQuotientAmbientOrder[k],
            this->representativesQuotientAmbientOrder[layers[i][j]]
          )
        ) {
          if (
            this->representativesQuotientAmbientOrder[layers[i][j]].
            generatorsLastAppliedFirst.size ==
            this->representativesQuotientAmbientOrder[k].
            generatorsLastAppliedFirst.size -
            1
          ) {
            arrows[i][j].addOnTop(k);
          }
        }
      }
    }
  }
  return this->toStringFromLayersAndArrows(arrows, layers, graphWidth, true);
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
  this->representativesQuotientAmbientOrder.size = 0;
  this->representativesQuotientAmbientOrder.reserve(
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
      this->representativesQuotientAmbientOrder.addOnTop(
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
  int numberOfPositiveRoots = this->rootSubsystem.size / 2;
  this->rootsOfBorel.setSize(numberOfPositiveRoots);
  for (int i = 0; i < numberOfPositiveRoots; i ++) {
    this->rootsOfBorel[i] = this->rootSubsystem[i + numberOfPositiveRoots];
  }
}

void KazhdanLusztigPolynomials::writeKLCoeffsToFile(
  std::fstream& output, List<int>& KLCoefficient, int topIndex
) {
  output.clear();
  output << "Top_index: " << topIndex << "\n";
  std::string currentString;
  this->kazhdanLuzstigCoefficientsToString(KLCoefficient, currentString);
  output << currentString;
}

int KazhdanLusztigPolynomials::readKLCoeffsFromFile(
  std::fstream& input, List<int>& output
) {
  std::string currentString;
  int topIndex;
  input >> currentString >> topIndex;
  output.setSize(this->size);
  for (int i = 0; i < this->size; i ++) {
    input >> currentString >> output[i];
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
    for (int i = 0; i < this->bruhatOrder[this->nextToExplore].size; i ++) {
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
    out
    << "Kazhdan-Lusztig Polynomials:<br>"
    << this->KLPolysToString(format);
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
        for (int j = 0; j < this->kazhdanLuzstigCoefficients[i].size; j ++) {
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
  Vector<Rational> root;
  Vector<Rational> root2;
  for (int i = 0; i < this->size; i ++) {
    this->simpleReflectionsActionList[i].reserve(dimension);
    for (int j = 0; j < dimension; j ++) {
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
      if (this->bruhatOrder[toList][j] == this->bruhatOrder[fromList][i]) {
        found = true;
        break;
      }
    }
    if (!found) {
      this->bruhatOrder[toList].addOnTop(this->bruhatOrder[fromList][i]);
    }
  }
}

int KazhdanLusztigPolynomials::chamberIndicatorToIndex(
  Vector<Rational>& chamberIndicator
) {
  int dimension = this->weylGroup->cartanSymmetric.numberOfRows;
  Vector<Rational> root;
  root.setSize(dimension);
  Vector<Rational> chamberIndicatorPlusRho;
  chamberIndicatorPlusRho = (chamberIndicator);
  chamberIndicatorPlusRho += this->weylGroup->rho;
  for (int i = 0; i < this->size; i ++) {
    Rational scalarProduct1;
    Rational scalarProduct2;
    bool firstIsPositive = false;
    bool secondIsPositive = false;
    bool haveSameSigns = true;
    for (int j = 0; j < this->weylGroup->rootSystem.size; j ++) {
      this->weylGroup->rootScalarCartanRoot(
        chamberIndicatorPlusRho, this->weylGroup->rootSystem[j], scalarProduct1
      );
      root = (*this)[i];
      root += (this->weylGroup->rho);
      this->weylGroup->rootScalarCartanRoot(
        root, this->weylGroup->rootSystem[j], scalarProduct2
      );
      firstIsPositive = scalarProduct1.isPositive();
      secondIsPositive = scalarProduct2.isPositive();
      if (scalarProduct1.isEqualToZero() || scalarProduct2.isEqualToZero()) {
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
    for (int j = 0; j < this->kazhdanLuzstigCoefficients[i].size; j ++) {
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
  FormatExpressions polynomialFormatLocal;
  polynomialFormatLocal.polynomialDefaultLetter = "q";
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
  List<bool> exploredFromTop;
  exploredFromTop.setSize(this->size);
  while (this->lowestNonExplored != - 1) {
    for (int i = 0; i < this->size; i ++) {
      exploredFromTop[i] = false;
    }
    int a = this->findMaximalBruhatNonExplored(exploredFromTop);
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
      exploredFromTop[a] = true;
      a = this->findMaximalBruhatNonExplored(exploredFromTop);
    }
    this->explored[this->lowestNonExplored] = true;
    this->lowestNonExplored =
    this->findMinimalBruhatNonExplored(this->explored);
  }
}

bool KazhdanLusztigPolynomials::indexGEQIndex(int a, int b) {
  Vector<Rational> difference;
  difference = (*this)[a];
  difference -= (*this)[b];
  return difference.isNegativeOrZero();
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
  Polynomial<Rational> accumulator;
  Polynomial<Rational> buffer1;
  Polynomial<Rational> buffer2;
  if (x == y) {
    this->kazhdanLuzstigPolynomials[x][y].makeOne();
    return;
  }
  if (!this->indexGEQIndex(y, x)) {
    this->kazhdanLuzstigPolynomials[x][y].makeZero();
    return;
  }
  accumulator.makeZero();
  MonomialPolynomial monomial;
  for (int i = 0; i < this->size; i ++) {
    if (this->indexGreaterThanIndex(i, x) && this->indexGEQIndex(y, i)) {
      buffer1.makeZero();
      for (int j = 0; j < this->rPolynomials[x][i].size(); j ++) {
        monomial = this->rPolynomials[x][i][j];
        monomial.invert();
        buffer1.addMonomial(
          monomial, this->rPolynomials[x][i].coefficients[j]
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
      buffer2.makeMonomial(0, powerQ, sign);
      buffer1 *= buffer2;
      buffer1 *= this->kazhdanLuzstigPolynomials[i][y];
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
      accumulator += buffer1;
    }
  }
  this->kazhdanLuzstigPolynomials[x][y].makeZero();
  Rational lengthDifference =
  this->weylGroup->group.elements[y].generatorsLastAppliedFirst.size -
  this->weylGroup->group.elements[x].generatorsLastAppliedFirst.size;
  lengthDifference /= 2;
  for (int i = 0; i < accumulator.size(); i ++) {
    if (accumulator[i].hasPositiveOrZeroExponents()) {
      monomial = accumulator[i];
      monomial.setVariable(0, monomial[0] * - 1);
      monomial.multiplyByVariable(0, lengthDifference);
      this->kazhdanLuzstigPolynomials[x][y].addMonomial(
        monomial, accumulator.coefficients[i]
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
  bool boolX = this->indexGreaterThanIndex(x, sx);
  bool boolY = this->indexGreaterThanIndex(y, sy);
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
      for (int j = 0; j < this->kazhdanLuzstigPolynomials[i].size; j ++) {
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
  x1 -= text.length() * LaTeXProcedures::textPrintCenteringAdjustmentX;
  y1 += LaTeXProcedures::textPrintCenteringAdjustmentY;
  x1 /= LaTeXProcedures::scaleFactor;
  y1 /= LaTeXProcedures::scaleFactor;
  output
  << "\\put("
  << x1 - LaTeXProcedures::figureCenterCoordinateSystemX
  << ", "
  << LaTeXProcedures::figureCenterCoordinateSystemY - y1
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
  if (static_cast<int>(penStyle) == DrawOptions::PenStyle::invisible) {
    return;
  }
  output.precision(4);
  x1 /= LaTeXProcedures::scaleFactor;
  x2 /= LaTeXProcedures::scaleFactor;
  y1 /= LaTeXProcedures::scaleFactor;
  y2 /= LaTeXProcedures::scaleFactor;
  std::string currentString;
  if (penStyle == static_cast<unsigned>(DrawOptions::PenStyle::dashed)) {
    currentString = "lightgray";
  } else {
    LaTeXProcedures::getStringFromColorIndex(
      colorIndex, currentString, drawInput
    );
  }
  output
  << "\\psline[linewidth = 0.3pt, linecolor ="
  << currentString
  << "]("
  << x1 - LaTeXProcedures::figureCenterCoordinateSystemX
  << ", "
  << LaTeXProcedures::figureCenterCoordinateSystemY - y1
  << ")"
  << "("
  << x2 - LaTeXProcedures::figureCenterCoordinateSystemX
  << ", "
  << LaTeXProcedures::figureCenterCoordinateSystemY - y2
  << ")\n";
}

void WeylGroupData::transformToSimpleBasisGenerators(
  Vectors<Rational>& generators,
  const HashedList<Vector<Rational> >& inputRootSystem
) {
  STACK_TRACE("WeylGroupData::transformToSimpleBasisGenerators");
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
    "WeylGroupData::"
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
  STACK_TRACE(
    "WeylGroupData::transformToSimpleBasisGeneratorsWithRespectToH"
  );
  for (int i = 0; i < generators.size; i ++) {
    if (generators[i].isEqualToZero()) {
      generators.removeIndexSwapWithLast(i);
      i --;
    }
  }
  for (int i = 0; i < generators.size; i ++) {
    if (!this->isPositiveOrPerpWithRespectToH(generators[i], hVector)) {
      generators[i].negate();
    }
  }
  bool reductionOccured = true;
  Vector<Rational> root;
  int maximumRuns = this->rootSystem.size * this->rootSystem.size;
  int totalRuns = 0;
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
    totalRuns ++;
    if (totalRuns > maximumRuns) {
      global.fatal
      << "This function should complete much faster, "
      << "there's something wrong with the inputs. The generators are: "
      << generators.toString()
      << "; the positive root is: "
      << hVector.toString()
      << "."
      << global.fatal;
    }
  }
}

void WeylGroupData::computeExtremeRootInTheSameKMod(
  const Vectors<Rational>& inputSimpleBasisK,
  const Vector<Rational>& inputRoot,
  Vector<Rational>& output,
  bool lookingForHighest
) {
  STACK_TRACE("WeylGroupData::computeExtremeRootInTheSameKMod");
  output = inputRoot;
  Vector<Rational> root;
  bool foundHigher = true;
  do {
    foundHigher = false;
    for (int i = 0; i < inputSimpleBasisK.size; i ++) {
      root = output;
      if (lookingForHighest) {
        root += inputSimpleBasisK[i];
      } else {
        root -= inputSimpleBasisK[i];
      }
      if (this->rootSystem.getIndex(root) != - 1) {
        output = root;
        foundHigher = true;
      }
      if (root.isEqualToZero()) {
        output *= - 1;
        foundHigher = true;
      }
    }
  } while (foundHigher);
}

bool OnePartialFractionDenominator::getDifferentialOperatorForm(
  List<ElementWeylAlgebra<Rational> >& output
) const {
  STACK_TRACE("OnePartialFractionDenominator::getDifferentialOperatorForm");
  Vectors<Rational> normals;
  this->getNormalsToConeWallsIfDecomposed(normals);
  output.clear();
  ElementWeylAlgebra<Rational> next;
  ElementWeylAlgebra<Rational> summand;
  for (int i = 0; i < normals.size; i ++) {
    next.makeZero();
    for (int j = 0; j < normals[i].size; j ++) {
      summand.makexidj(j, j);
      summand *= normals[i][j];
      next += summand;
    }
    output.addOnTop(next);
  }
  return true;
}

void OnePartialFractionDenominator::computeDifferentialOperatorConstant(
  Rational& output
) const {
  output = 1;
  for (int i = 0; i < this->denominatorsNoScale.size(); i ++) {
    OnePartialFractionDenominatorComponent& component =
    this->denominatorsNoScale.values[i];
    output *= Rational::factorial(component.getTotalMultiplicity() - 1);
  }
  output.invert();
}

std::string OnePartialFractionDenominator::toLatexDifferentialOperator(
  Polynomial<LargeInteger>& coefficient, FormatExpressions* format
) const {
  STACK_TRACE("OnePartialFractionDenominator::toLatexDifferentialOperator");
  if (this->denominatorsNoScale.size() != this->owner->ambientDimension) {
    return "(non-decomposed)";
  }
  List<ElementWeylAlgebra<Rational> > differentialOperators;
  if (!this->getDifferentialOperatorForm(differentialOperators)) {
    return "(non-decomposed)";
  }
  Rational extraConstant;
  this->computeDifferentialOperatorConstant(extraConstant);
  std::stringstream out;
  int lineCountInCoefficient = 0;
  if (coefficient.size() > 1) {
    out
    << "\\left("
    << coefficient.toStringWithPossibleLineBreak(
      format, &lineCountInCoefficient
    )
    << "\\right)";
  } else {
    out
    << coefficient.toStringWithPossibleLineBreak(
      format, &lineCountInCoefficient
    );
  }
  std::stringstream outOperator;
  outOperator << "\\cdot ";
  if (!extraConstant.isEqualToOne()) {
    if (extraConstant.isNegative()) {
      outOperator << "\\left(";
    }
    outOperator << extraConstant.toString(format);
    if (extraConstant.isNegative()) {
      outOperator << "\\right)";
    }
  }
  Vectors<Rational> exponents;
  this->getNormalizedSortedDenominatorExponents(exponents);
  for (int i = 0; i < differentialOperators.size; i ++) {
    if (!this->denominatorsNoScale.contains(exponents[i])) {
      global.fatal
      << "Exponent vector "
      << exponents[i]
      << " not found in "
      << this->denominatorsNoScale.keys
      << global.fatal;
    }
    const OnePartialFractionDenominatorComponent& component =
    this->denominatorsNoScale.getValueNoFail(exponents[i]);
    if (component.multiplicities.size != 1) {
      return "(non-elongated)";
    }
    int power = component.getTotalMultiplicity() - 1;
    if (power == 0) {
      continue;
    }
    outOperator
    << "\\left("
    << differentialOperators[i].toStringWithPossibleLineBreak(format)
    << "\\right)";
    if (power > 1 && power < 10) {
      outOperator << "^" << power;
    }
    if (power >= 10) {
      outOperator << "^{" << power << "}";
    }
  }
  std::stringstream outFraction;
  OnePartialFractionDenominator withMultiplicityOne;
  withMultiplicityOne.owner = this->owner;
  for (int i = 0; i < this->denominatorsNoScale.size(); i ++) {
    OnePartialFractionDenominatorComponent& current =
    this->denominatorsNoScale.values[i];
    withMultiplicityOne.addMultiplicity(
      this->denominatorsNoScale.keys[i], 1, current.getLargestElongation()
    );
  }
  outFraction << "\\cdot" << withMultiplicityOne.toLatex(1);
  if (lineCountInCoefficient > 1) {
    out
    << "\\begin{array}{l}"
    << outOperator.str()
    << "\\\\\n"
    << "~~"
    << outFraction.str()
    << "\\end{array}";
  } else {
    out << outOperator.str() << outFraction.str();
  }
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
  this->checkInitialization();
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
  if (this->denominatorsNoScale.size() != this->owner->ambientDimension) {
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

void OnePartialFractionDenominator::getNormalsToConeWallsIfDecomposed(
  Vectors<Rational>& output
) const {
  STACK_TRACE(
    "OnePartialFractionDenominator::getNormalsToConeWallsIfDecomposed"
  );
  Vectors<Rational> coneGenerators;
  this->getDenominatorExponentsWithoutMultiplicities(coneGenerators);
  Matrix<Rational> normalsMatrixForm;
  normalsMatrixForm.assignVectorsToRows(coneGenerators);
  // Example 1. The matrix before inversion: (2).
  // Example 2. The matrix before inversion: the 2x2 identity.
  normalsMatrixForm.invert();
  // Example 1. The matrix after inversion: (1/2).
  // Example 2. The matrix after inversion: the 2x2 identity.
  output.assignMatrixColumns(normalsMatrixForm);
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
  this->getNormalsToConeWallsIfDecomposed(normals);
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
  for (Cone& cone : this->chambers.refinedCones.values) {
    this->computeOneQuasipolynomial(cone);
  }
}

void PartialFractions::computeOneQuasipolynomial(Cone& cone) {
  STACK_TRACE("PartialFractions::computeOneQuasipolynomial");
  Vector<Rational> internalPoint = cone.internalPoint();
  this->computeOneVectorPartitionFunction(
    cone.payload.getPolynomialNonConstant(), internalPoint
  );
  cone.payload.getPolynomialNonConstant().checkConsistencyWithoutDebugMessage(
  );
  for (int i = 0; i < cone.vertices.size; i ++) {
    cone.precomputeVectorPartitionFunction(
      cone.vertices[i], this->originalVectors
    );
  }
  cone.precomputeVectorPartitionFunction(internalPoint, this->originalVectors);
  Rational n = 10;
  cone.precomputeVectorPartitionFunction(
    internalPoint* n, this->originalVectors
  );
  n.raiseToPower(100);
  cone.precomputeVectorPartitionFunction(
    internalPoint* n, this->originalVectors
  );
}

void PartialFractions::evaluateVectorPartitionFunctionNonChecked(
  const Vector<Rational>& input, Rational& output, std::stringstream* comments
) {
  STACK_TRACE("PartialFractions::evaluateVectorPartitionFunctionNonChecked");
  if (this->chambers.refinedCones.size() == 0) {
    global.fatal
    << "Please compute the partial fraction decomposition first."
    << global.fatal;
  }
  int coneIndex = 0;
  for (; coneIndex < this->chambers.refinedCones.size(); coneIndex ++) {
    const Cone& cone = this->chambers.refinedCones.values[coneIndex];
    if (cone.isInCone(input)) {
      break;
    }
  }
  if (coneIndex >= this->chambers.refinedCones.size()) {
    // No cone contains the input.
    output = 0;
    return;
  }
  const Cone& cone = this->chambers.refinedCones.values[coneIndex];
  if (comments != nullptr) {
    *comments
    << "Vector: "
    << input.toString()
    << " is in chamber "
    << cone.displayId()
    << "<br>"
    << this->chambers.refinedCones[coneIndex].toHTML(this->chambers, false)
    << "<br>";
  }
  const QuasiPolynomial& quasiPolynomial = cone.payload.getPolynomial();
  output = quasiPolynomial.evaluate(input, comments);
}

void PartialFractions::evaluateVectorPartitionFunctionChecked(
  const Vector<Rational>& input, Rational& output, std::stringstream* comments
) {
  STACK_TRACE("PartialFractions::evaluateVectorPartitionFunctionChecked");
  if (this->chambers.refinedCones.size() == 0) {
    global.fatal
    << "Please compute the partial fraction decomposition first."
    << global.fatal;
  }
  List<int> coneIndices;
  for (int i = 0; i < this->chambers.refinedCones.size(); i ++) {
    const Cone& cone = this->chambers.refinedCones.values[i];
    if (cone.isInCone(input)) {
      coneIndices.addOnTop(i);
    }
  }
  if (coneIndices.size >= this->chambers.refinedCones.size()) {
    // No cone contains the input.
    output = 0;
    return;
  }
  Rational candidate;
  for (int i = 0; i < coneIndices.size; i ++) {
    const Cone& cone = this->chambers.refinedCones.values[coneIndices[i]];
    if (comments != nullptr) {
      *comments
      << "Vector: "
      << input.toString()
      << " is in chamber "
      << cone.displayId()
      << "<br>"
      << cone.toHTML(this->chambers, false)
      << "<br>";
    }
    candidate = cone.payload.getPolynomial().evaluate(input, comments);
    if (i == 0) {
      output = candidate;
    }
    if (output != candidate) {
      const Cone& previous =
      this->chambers.refinedCones.values[coneIndices[i - 1]];
      global.fatal
      << "Different values: "
      << candidate
      << " and "
      << output
      << ".<hr>"
      << cone.toString()
      << "<hr>"
      << previous.toString()
      << comments->str()
      << global.fatal;
    }
  }
}

bool PartialFractions::computeOneVectorPartitionFunction(
  QuasiPolynomial& output, Vector<Rational>& newIndicator
) {
  STACK_TRACE("PartialFractions::computeOneVectorPartitionFunction");
  ProgressReport report;
  if (this->assureIndicatorRegularity(newIndicator)) {
    report.report("Indicator modified to regular");
  }
  if (!this->checkForMinimalityDecompositionWithRespectToRoot(&newIndicator)) {
    return false;
  }
  this->statistics.totalFractionsWithAccountedVectorPartitionFunction = 0;
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
  Lattice ambient = output.ambientLatticeReduced;
  output.compress();
  output.sortLatticeShifts();
  return true;
}

bool Wall::operator==(const Wall& other) const {
  return this->normal == other.normal;
}

bool Wall::operator!=(const Wall& other) const {
  return this->normal != other.normal;
}

bool Wall::operator>(const Wall& other) const {
  return this->normal > other.normal;
}

bool Wall::isInClosedHalfSpace(const Vector<Rational>& point) const {
  return !this->normal.scalarEuclidean(point).isNegative();
}

bool Wall::isInOpenHalfSpace(const Vector<Rational>& point) const {
  return this->normal.scalarEuclidean(point).isPositive();
}

bool Wall::hasAtLeastOnePointInOpenHalfSpace(
  const List<Vector<Rational> >& points
) const {
  for (Vector<Rational>& point : points) {
    if (this->isInOpenHalfSpace(point)) {
      return true;
    }
  }
  return false;
}

std::string Wall::toString() const {
  return "Wall: " + this->normal.toString();
}

bool Wall::checkConsistency() const {
  for (int i = 0; i < this->neighbors.size; i ++) {
    for (int j = i + 1; j < this->neighbors.size; j ++) {
      if (this->neighbors[i] == this->neighbors[j]) {
        global.fatal << "Neighbor appears more than once. " << global.fatal;
      }
    }
  }
  return true;
}

Cone::Payload::Payload() {
  this->lowestSlicingIndex = - 1;
  this->displayNumber = 0;
  this->visited = false;
}

void Cone::Payload::setPolynomial(QuasiPolynomial& input) {
  this->previousPolynomial = this->polynomial;
  this->polynomial = input;
}

const QuasiPolynomial& Cone::Payload::getPolynomial() const {
  return this->polynomial;
}

QuasiPolynomial& Cone::Payload::getPolynomialNonConstant() {
  return this->polynomial;
}

void Cone::Payload::incrementHashOfContainingSimplices(char input) {
  this->hashOfContainingSimplices.addOnTop(input);
  if (this->hashOfContainingSimplices.size > 31) {
    Crypto::computeSha3_256(
      this->hashOfContainingSimplices, this->hashOfContainingSimplices
    );
    this->hashOfContainingSimplices.setSize(16);
  }
}

Cone::Cone() {
  this->flagIsTheZeroCone = true;
  this->id = - 1;
}

Cone::Cone(int inputId) {
  this->flagIsTheZeroCone = true;
  this->id = inputId;
}

bool Cone::checkConsistencyFull(const ConeCollection& owner) const {
  STACK_TRACE("Cone::checkConsistencyFull");
  if (!owner.containsId(this->id)) {
    global.fatal
    << "Cone of id: "
    << this->id
    << " not contained in its owner. "
    << global.fatal;
  }
  HashedList<int> allNeighbors;
  for (const Wall& wall : this->walls) {
    wall.checkConsistency();
    for (int neighbor : wall.neighbors) {
      if (allNeighbors.contains(neighbor)) {
        global.fatal
        << "Cone id: "
        << this->id
        << " has neighbor id: "
        << neighbor
        << " present more than once.<br>"
        << owner.toStringNeighborGraph()
        << "<br>"
        << owner.toHTMLGraphicsOnly(false, false)
        << owner.toHTMLHistory()
        << global.fatal;
      }
      allNeighbors.addOnTop(neighbor);
      if (!owner.containsId(neighbor)) {
        global.fatal
        << "Cone of id: "
        << this->id
        << " has non-existent neighbors. "
        << "Non-existent neighbor: "
        << neighbor
        << " adjacent through wall: "
        << wall
        << ". All walls of the chamber: "
        << this->walls
        << ".<br>All cone ids: "
        << owner.toStringConeIds()
        << "<br>"
        << owner.toHTMLGraphicsOnly(false, true)
        << "<br>"
        << owner.toHTMLHistory()
        << owner.toHTMLWithoutGraphics()
        << global.fatal;
      }
    }
  }
  return true;
}

bool Cone::operator>(const Cone& other) const {
  return this->walls > other.walls;
}

bool Cone::operator==(const Cone& other) const {
  return
  this->flagIsTheZeroCone == other.flagIsTheZeroCone &&
  this->walls == other.walls;
}

unsigned int Cone::hashFunction() const {
  return this->vertices.hashFunction();
}

unsigned int Cone::hashFunction(const Cone& input) {
  return input.hashFunction();
}

List<List<int> > Cone::getAllNeighbors() const {
  List<List<int> > result;
  for (const Wall& wall : this->walls) {
    result.addOnTop(wall.neighbors);
  }
  return result;
}

Vectors<Rational> Cone::getAllNormals() const {
  Vectors<Rational> result;
  for (const Wall& wall : this->walls) {
    result.addOnTop(wall.normal);
  }
  return result;
}

void Cone::intersectHyperplane(
  Vector<Rational>& normal, Cone& outputConeLowerDimension
) {
  if (normal.isEqualToZero()) {
    global.fatal << "zero normal not allowed. " << global.fatal;
  }
  int dimension = normal.size;
  Matrix<Rational> kernelComputer;
  Matrix<Rational> embedding;
  Matrix<Rational> projection;
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
  simpleRoots.coordinatesInBasisNoFailure(basis, convertedCoordinates);
  projection.assignVectorsToRows(convertedCoordinates);
  projection.transpose();
  projection.resize(dimension - 1, dimension, false);
  Vectors<Rational> newNormals = this->getAllNormals();
  projection.actOnVectorsColumn(newNormals, Rational::zero());
  List<Wall> inputWalls;
  for (int i = 0; i < newNormals.size; i ++) {
    Wall incoming;
    incoming.normal = newNormals[i];
    incoming.neighbors = this->walls[i].neighbors;
  }
  bool success = outputConeLowerDimension.createFromWalls(inputWalls, false);
  if (!success) {
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
    if (inputLPoly[i].::MonomialPolynomial::isOneLetterFirstDegree(&index)) {
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

Wall& Cone::getWallWithNormalNoFail(const Vector<Rational>& desiredNormal) {
  for (Wall& wall : this->walls) {
    if (wall.normal == desiredNormal) {
      return wall;
    }
  }
  global.fatal
  << "Failed to find normal: "
  << desiredNormal
  << ". "
  << global.fatal;
  return this->walls[0];
}

bool Cone::solveLQuasiPolyEqualsZeroIAmProjective(
  QuasiPolynomial& inputLQP, List<Cone>& outputConesOverEachLatticeShift
) {
  outputConesOverEachLatticeShift.setSize(inputLQP.latticeShifts.size);
  bool result = true;
  for (int i = 0; i < inputLQP.latticeShifts.size; i ++) {
    result = result &&
    this->solveLPolynomialEqualsZeroIAmProjective(
      inputLQP.valueOnEachLatticeShift[i], outputConesOverEachLatticeShift[i]
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

std::string HtmlRoutines::toHtmlLatexLiteralWithCopy(
  const std::string& inputLatex
) {
  std::stringstream out;
  out
  << "<div class='"
  << HtmlRoutines::latexWithCopyButton
  << "'>"
  << inputLatex
  << "</div>";
  return out.str();
}

std::string HtmlRoutines::toHtmlTableRowsFromJSON(const JSData& input) {
  STACK_TRACE("HtmlRoutines::toHtmlTableRowsFromJSON");
  if (input.elementType == JSData::Type::tokenObject) {
    std::stringstream out;
    out << "<table class='tableDatabaseItem'>";
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
  if (input.elementType == JSData::Type::tokenArray) {
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
  List<std::string>& labels, List<List<std::string> >& content, bool nestTables
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
        << StringRoutines::stringTrimToLengthForDisplay(content[i][j], 1000)
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
  bool doReplaceNewLineByBr,
  bool newLineNeedsCarriageReturn
) {
  std::stringstream out;
  bool modified = false;
  for (unsigned int i = 0; i < input.size(); i ++) {
    bool currentlyModified = true;
    bool isReturnNewLine = false;
    if (i + 1 < input.size()) {
      isReturnNewLine = input[i] == '\r' && input[i + 1] == '\n';
    }
    bool isNewLine = input[i] == '\n';
    if (doReplaceNewLineByBr && isNewLine && !newLineNeedsCarriageReturn) {
      out << "<br>\n";
    } else if (doReplaceNewLineByBr && isReturnNewLine) {
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
  this->statistics.numberOfElongations ++;
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
    int totalSummands = leastCommonMultipleElongations / elongationValue;
    Vector<Rational> exponent = normalized * elongationValue;
    toBeReduced.getNElongationPolynomial(
      totalSummands, exponent, multiplicand
    );
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

PartialFractions::Details::Details() {
  this->owner = nullptr;
  this->maximumIntermediates = 4;
  this->flagNeedsElongation = false;
}

PartialFractions::Statistics::Statistics() {
  this->numberOfElongations = 0;
}

bool SlTwoInSlN::computeInvariantsOfDegree(
  List<int>& decompositionDimensions,
  int degree,
  List<Polynomial<Rational> >& output,
  std::string& outputError
) {
  this->initFromModuleDecomposition(decompositionDimensions, false, false);
  SelectionWithMaximumMultiplicity selection;
  selection.initializeMaximumMultiplicity(this->dimension, degree);
  outputError = "";
  LargeInteger numberOfCycles =
  selection.numberOfCombinationsOfCardinality(degree);
  if (numberOfCycles > 1000000) {
    outputError = " Computation too large. ";
    return false;
  }
  Polynomial<Rational> basisMonomialsZeroWeight;
  Polynomial<Rational> basisMonomialsAll;
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
  Polynomial<Rational> buffer;
  for (int l = 0; l < 2; l ++) {
    for (int k = 0; k < basisMonomialsZeroWeight.size(); k ++) {
      if (l == 0) {
        this->eElement.actOnMonomialAsDifferentialOperator(
          basisMonomialsZeroWeight[k], buffer
        );
      } else {
        this->fElement.actOnMonomialAsDifferentialOperator(
          basisMonomialsZeroWeight[k], buffer
        );
      }
      for (int j = 0; j < basisMonomialsAll.size(); j ++) {
        int indexInResult = buffer.monomials.getIndex(basisMonomialsAll[j]);
        int currentRow = l * basisMonomialsAll.size() + j;
        if (indexInResult == - 1) {
          matrix.elements[currentRow][k] = 0;
        } else {
          matrix.elements[currentRow][k] = buffer.coefficients[indexInResult];
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

void DrawingVariables::makeMeAStandardBasis(int dimension) {
  if (dimension < 1) {
    return;
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
    this->basisProjectionPlane[0][0] = 0.6;
    this->basisProjectionPlane[0][1] = 0.4;
    this->basisProjectionPlane[0][2] = 0;
    this->basisProjectionPlane[1][0] = - 0.4;
    this->basisProjectionPlane[1][1] = 0.6;
    this->basisProjectionPlane[1][2] = 1;
    this->modifyToOrthonormalNoShiftSecond(
      this->basisProjectionPlane[0], this->basisProjectionPlane[1]
    );
  } else if (dimension == 2) {
    // <-if not needed but good for documentation purposes.
    this->basisProjectionPlane[1] *= - 1;
  }
  if (this->bilinearForm.numberOfRows != dimension) {
    this->bilinearForm.makeIdentityMatrix(dimension, 1, 0);
  }
  this->computeProjectionsEiVectors();
}

std::string ConeCollection::drawMeToHtmlLastCoordAffine(
  DrawingVariables& drawingVariables, FormatExpressions& format
) {
  bool isBad = false;
  isBad = this->drawMeLastCoordinateAffine(true, drawingVariables, format);
  std::stringstream out;
  out << drawingVariables.getHTMLDiv(this->getDimension() - 1, true);
  if (isBad) {
    out << "<hr>" << "found cones which I can't draw<hr>";
  }
  out << this->toHTML();
  return out.str();
}

std::string ConeCollection::drawMeToHtmlProjective(
  DrawingVariables& drawingVariables,
  bool includeVertices,
  bool includeLattice,
  bool generateControls
) const {
  STACK_TRACE("ConeCollection::drawMeToHtmlProjective");
  bool isGood = true;
  isGood =
  this->drawMeProjective(drawingVariables, includeVertices, includeLattice);
  std::stringstream out;
  if (this->refinedCones.size() > this->maximumCones) {
    out
    << "I have "
    << this->refinedCones.size()
    << " refined cones, more than the maximum of "
    << this->maximumCones
    << " allowed for drawing. I am omitting most of the graphics. ";
  }
  out << drawingVariables.getHTMLDiv(this->getDimension(), generateControls);
  if (!isGood) {
    out << "<hr>" << "Found cones which I cannot draw.<hr>";
    out << this->toString();
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
    drawingVariables.initializeDimensions(this->getDimension() - 1);
  }
  drawingVariables.drawCoordinateSystemBuffer(this->getDimension() - 1);
  for (int i = 0; i < this->refinedCones.size(); i ++) {
    if (
      !this->refinedCones[i].drawMeLastCoordinateAffine(
        initializeDrawingVariables, drawingVariables, format
      )
    ) {
      result = false;
    }
    std::stringstream currentStream;
    currentStream << i + 1;
    Vector<Rational> point = this->nonRefinedCones[i].internalPoint();
    point.makeAffineUsingLastCoordinate();
    drawingVariables.drawTextAtVector(point, currentStream.str(), "black");
  }
  return result;
}

bool ConeCollection::drawMeProjectiveInitialize(
  DrawingVariables& drawingVariables
) const {
  STACK_TRACE("ConeCollection::drawMeProjectiveInitialize");
  Matrix<Rational> matrix;
  drawingVariables.initialize();
  drawingVariables.initializeDimensions(this->getDimension());
  drawingVariables.makeMeAStandardBasis(this->getDimension());
  Vectors<Rational> roots;
  Vector<Rational> point;
  if (this->getDimension() == 2) {
    return true;
  }
  if (!this->convexHull.internalPoint(point)) {
    global.comments
    << "Failed to get internal point of convex hull: "
    << this->convexHull.toString();
    return false;
  }
  matrix.assignVectorRow(point);
  matrix.getZeroEigenSpace(roots);
  for (int i = 0; i < 2; i ++) {
    for (int j = 0; j < this->getDimension(); j ++) {
      drawingVariables.basisProjectionPlane[i][j] =
      roots[i][j].getDoubleValue();
    }
  }
  drawingVariables.modifyToOrthonormalNoShiftSecond(
    drawingVariables.basisProjectionPlane[0],
    drawingVariables.basisProjectionPlane[1]
  );
  drawingVariables.computeProjectionsEiVectors();
  return true;
}

bool ConeCollection::drawProjectiveChambers(
  DrawingVariables& drawingVariables, bool includeVertices, bool includeLattice
) const {
  bool result = true;
  List<const List<Cone>*> coneCollectionsToPlot = List<const List<Cone>*>({
      &this->refinedCones.values,
      &this->nonRefinedCones.values,
      &this->conesWithIrregularWalls.values
    });
  if (this->refinedCones.size() > this->maximumCones) {
    // Too many cones to draw;
    return true;
  }
  if (includeLattice) {
    for (const List<Cone> * collection : coneCollectionsToPlot) {
      for (Cone& cone : *collection) {
        cone.drawLattice(drawingVariables);
      }
    }
  }
  bool omitLowerIdWalls = this->conesWithIrregularWalls.size() == 0;
  if (includeVertices) {
    for (const List<Cone> * collection : coneCollectionsToPlot) {
      for (Cone& cone : *collection) {
        result = result &&
        cone.drawMeProjectiveVertices(drawingVariables, omitLowerIdWalls);
      }
    }
  }
  for (const List<Cone> * collection : coneCollectionsToPlot) {
    for (Cone& cone : *collection) {
      result = result &&
      cone.drawMeProjectiveSlice(drawingVariables, omitLowerIdWalls);
    }
  }
  return result;
}

bool ConeCollection::drawMeProjective(
  DrawingVariables& drawingVariables, bool includeVertices, bool includeLattice
) const {
  STACK_TRACE("ConeCollection::drawMeProjective");
  bool result = true;
  if (this->getDimension() <= 1) {
    return false;
  }
  this->drawMeProjectiveInitialize(drawingVariables);
  drawingVariables.drawCoordinateSystemBuffer(this->getDimension());
  result = result &&
  this->drawProjectiveChambers(
    drawingVariables, includeVertices, includeLattice
  );
  for (Vector<Rational> vertex : this->slicingDirections) {
    vertex /= vertex.sumCoordinates();
    drawingVariables.drawCircleAtVectorBufferRational(vertex, "blue", 3);
  }
  drawingVariables.computeBoundingBox();
  return result;
}

bool Cone::isEntireSpace() {
  return this->walls.size == 0 && this->flagIsTheZeroCone;
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
  Matrix<Rational> bufferMatrix;
  Vector<Rational> candidate;
  Rational scalarProduct;
  for (int i = 0; i < x; i ++) {
    wallSelection.incrementSelectionFixedCardinalityReturnFalseIfPastLast(
      dimension - 1
    );
    if (
      basis.computeNormalFromSelection(
        candidate, wallSelection, bufferMatrix, dimension
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
  while (!Cone::isRegularToBasis(basis, output, normal, output.size)) {
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
  for (int k = 0; k < this->walls.size; k ++) {
    for (int i = 0; i < verticesScaled.size; i ++) {
      if (
        !drawVertex[i] ||
        !this->walls[k].normal.scalarEuclidean(this->vertices[i]).isEqualToZero
        ()
      ) {
        continue;
      }
      for (int j = i + 1; j < verticesScaled.size; j ++) {
        if (
          !drawVertex[j] ||
          !this->walls[k].normal.scalarEuclidean(this->vertices[j]).
          isEqualToZero()
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
  if (this->walls.size < 1) {
    return "The cone is the entire space";
  }
  if (this->vertices.size < 1) {
    return "The cone is empty";
  }
  std::stringstream out;
  drawingVariables.makeMeAStandardBasis(this->getDimension() - 1);
  bool foundBadVertex =
  this->drawMeLastCoordinateAffine(false, drawingVariables, format);
  drawingVariables.drawCoordinateSystemBuffer(this->getDimension() - 1);
  if (foundBadVertex) {
    out << "<br>The cone does not lie in the upper half-space. ";
  } else {
    out << drawingVariables.getHTMLDiv(this->getDimension() - 1, true);
  }
  out << "<br>" << this->toString(&format);
  return out.str();
}

bool Cone::drawMeProjective(DrawingVariables& drawingVariables) const {
  STACK_TRACE("Cone::drawMeProjective");
  this->drawMeProjectiveVertices(drawingVariables, false);
  return this->drawMeProjectiveSlice(drawingVariables, false);
}

void Cone::computeRescaledVerticesForDrawing(Vectors<Rational>& output) const {
  output = this->vertices;
  for (int i = 0; i < output.size; i ++) {
    Rational sumAbsoluteValuesCoordinates = 0;
    for (int j = 0; j < this->getDimension(); j ++) {
      sumAbsoluteValuesCoordinates += (output[i][j].isPositive()) ?
      output[i][j] :
      - output[i][j];
    }
    if (sumAbsoluteValuesCoordinates.isEqualToZero()) {
      global.fatal << "Zero vector not allowed. " << global.fatal;
    }
    output[i] /= sumAbsoluteValuesCoordinates;
  }
}

void Cone::drawLattice(DrawingVariables& drawingVariables) const {
  STACK_TRACE("Cone::drawLattice");
  const QuasiPolynomial& polynomial = this->payload.getPolynomial();
  if (polynomial.isEqualToZero()) {
    return;
  }
  Vectors<Rational> allNormals = this->getAllNormals();
  polynomial.ambientLatticeReduced.draw(drawingVariables, &allNormals);
}

bool Cone::isAlreadyDrawn(const Vector<Rational>& vertex) const {
  for (const Wall& wall : this->walls) {
    if (!wall.normal.scalarEuclidean(vertex).isEqualToZero()) {
      continue;
    }
    if (wall.neighbors.size != 1) {
      continue;
    }
    if (this->id > wall.neighbors[0]) {
      return true;
    }
  }
  return false;
}

bool Cone::drawMeProjectiveVertices(
  DrawingVariables& drawingVariables, bool omitAlreadyDrawn
) const {
  STACK_TRACE("Cone::drawMeProjectiveVertices");
  Vector<Rational> zeroRoot;
  zeroRoot.makeZero(this->getDimension());
  Vectors<Rational> verticesScaled;
  this->computeRescaledVerticesForDrawing(verticesScaled);
  Vector<Rational> root;
  for (const Vector<Rational>& vertex : verticesScaled) {
    if (omitAlreadyDrawn) {
      if (this->isAlreadyDrawn(vertex)) {
        continue;
      }
    }
    drawingVariables.drawLineBetweenTwoVectorsBufferRational(
      zeroRoot,
      vertex,
      "lightblue",
      1,
      DrawOptions::PenStyle::dashed,
      "vertices"
    );
  }
  return true;
}

bool Cone::drawMeProjectiveSlice(
  DrawingVariables& drawingVariables, bool omitWallsBelongingToLowerId
) const {
  Vectors<Rational> verticesScaled;
  this->computeRescaledVerticesForDrawing(verticesScaled);
  std::string color = "black";
  int width = 1;
  if (this->payload.visited) {
    color = "brown";
    width = 2;
  }
  for (const Wall& wall : this->walls) {
    if (omitWallsBelongingToLowerId && wall.neighbors.size == 1) {
      if (wall.neighbors[0] < this->id) {
        continue;
      }
    }
    for (int i = 0; i < this->vertices.size; i ++) {
      if (!wall.normal.scalarEuclidean(this->vertices[i]).isEqualToZero()) {
        continue;
      }
      for (int j = i + 1; j < this->vertices.size; j ++) {
        if (!wall.normal.scalarEuclidean(this->vertices[j]).isEqualToZero()) {
          continue;
        }
        if (!this->isHonest1DEdgeAffine(i, j)) {
          continue;
        }
        drawingVariables.drawLineBetweenTwoVectorsBufferRational(
          verticesScaled[i], verticesScaled[j], color, width
        );
      }
    }
  }
  std::stringstream out;
  out << this->displayId();
  Vector<Rational> point = this->internalPointNormal();
  point /= point.sumCoordinates();
  drawingVariables.drawTextAtVector(point, out.str(), "black");
  return true;
}

std::string Cone::drawMeToHtmlProjective(
  DrawingVariables& drawingVariables, FormatExpressions& format
) {
  if (this->flagIsTheZeroCone) {
    return "The cone is the zero cone (i.e. contains only the origin).";
  }
  if (this->walls.size <= 0) {
    return "The cone is the entire space.";
  }
  std::stringstream out;
  if (this->vertices.size < 1) {
    out
    << "There has been a programming error. The cone is empty.<br>"
    << this->toString(&format);
    return out.str();
  }
  drawingVariables.makeMeAStandardBasis(this->getDimension());
  this->drawMeProjective(drawingVariables);
  drawingVariables.drawCoordinateSystemBuffer(this->getDimension());
  out << drawingVariables.getHTMLDiv(this->getDimension(), true);
  out << "<br>" << this->toString(&format);
  return out.str();
}

template <class Base>
std::iostream& operator<<(std::iostream& output, const Complex<Base>& input) {
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
  Vectors<Rational>& inputDirections, Vectors<Rational>& inputAffinePoints
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
    projectivizedDirections[i + inputAffinePoints.size] =
    inputAffinePoints[i];
    projectivizedDirections[i + inputAffinePoints.size].setSize(
      affineDimension + 1
    );
    *projectivizedDirections[i + inputAffinePoints.size].lastObject() = 1;
  }
  this->initializeFromDirectionsAndRefine(projectivizedDirections);
}

int ConeCollection::getDimension() const {
  if (this->refinedCones.size() > 0) {
    return this->refinedCones.values[0].getDimension();
  }
  if (this->nonRefinedCones.size() > 0) {
    return this->nonRefinedCones.values[0].getDimension();
  }
  if (this->conesWithIrregularWalls.size() > 0) {
    return this->conesWithIrregularWalls.values[0].getDimension();
  }
  return - 1;
}

int ConeCollection::getLowestIndexRefinedChamberContaining(
  const Vector<Rational>& root
) const {
  for (int i = 0; i < this->refinedCones.size(); i ++) {
    if (this->refinedCones.values[i].isInCone(root)) {
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
  int affineDimension = affinePoint.size;
  for (int i = 0; i < this->refinedCones.size(); i ++) {
    const Cone& refined = this->refinedCones.values[i];
    List<Wall> newNormals;
    Wall wall;
    for (int j = 0; j < refined.walls.size; j ++) {
      wall.normal =
      refined.walls[j].normal.getProjectivizedNormal(affinePoint);
      wall.neighbors = refined.walls[j].neighbors;
    }
    wall.normal.makeEi(affineDimension + 1, affineDimension);
    incoming.createFromWalls(newNormals, false);
    output.convexHull.makeConvexHullOfMeAnd(incoming);
    output.addRefinedCone(incoming);
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
  input.coordinatesInBasisNoFailure(basis, root);
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
  SelectionWithMaximumMultiplicity boundingBox;
  boundingBox.initializeMaximumMultiplicity(
    dimensionAffine, upperBoundPointsInEachDim* 2
  );
  // format of the boundingBox:
  // if bounding box shows a vector (x_1, ...) then
  // it corresponds to a vector with coodinates (x_1-upperBoundPointsInEachDim,
  // x_2-upperBoundPointsInEachDim, ...)
  int numberOfCycles = boundingBox.numberOfSelectionsTotal();
  if (numberOfCycles <= 0 || numberOfCycles > 1000000) {
    // We test up to 1 million lattice points.
    // This is very restrictive: in 8 dimensions, selecting
    // upperBoundPointsInEachDim=2,
    // we get a total of (2*2+ 1)^8=390625 points to test, which is a pretty
    // darn small box
    return false;
  }
  outputPoints.reserve(numberOfCycles);
  outputPoints.size = 0;
  Vector<Rational> candidatePoint;
  Vectors<Rational> latticeBasis;
  latticeBasis.assignMatrixRows(lattice.basisRationalForm);
  for (int i = 0; i < numberOfCycles; i ++, boundingBox.incrementSubset()) {
    candidatePoint = actualShift;
    if (shiftAllPointsBy != nullptr) {
      candidatePoint += *shiftAllPointsBy;
    }
    for (int j = 0; j < boundingBox.multiplicities.size; j ++) {
      candidatePoint +=
      latticeBasis[j] *(
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
  for (int i = 0; i < this->projectivizedComplex.refinedCones.size(); i ++) {
    int index =
    other.projectivizedComplex.getLowestIndexRefinedChamberContaining(
      this->projectivizedComplex.refinedCones.values[i].internalPoint()
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
  out << HtmlRoutines::getMathNoDisplay(partialFractions.toString(&format));
  partialFractions.split(nullptr);
  out << HtmlRoutines::getMathNoDisplay(partialFractions.toString(&format));
  // partialFractions.chambers.initializeFromDirectionsAndRefine(roots);
  global.fatal << "Not implemented. " << global.fatal;
  // partialFractions.chambersOld.AmbientDimension = roots[0].size;
  // partialFractions.chambersOld.directions = roots;
  // partialFractions.chambersOld.SliceEuclideanSpace(false);
  // partialFractions.chambers.
  // AssignCombinatorialChamberComplex(partialFractions.chambersOld);
  this->quasiPolynomials.setSize(
    partialFractions.chambers.refinedCones.size()
  );
  Vector<Rational> indicator;
  for (int i = 0; i < partialFractions.chambers.refinedCones.size(); i ++) {
    indicator = partialFractions.chambers.refinedCones[i].internalPoint();
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
  coneContainingOutputPoint.internalPoint(output);
  Vectors<Rational> basisRoots;
  basisRoots.assignMatrixRows(this->basisRationalForm);
  if (!output.coordinatesInBasis(basisRoots, coordinatesInBasis, nullptr)) {
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
  for (int i = 0; i < this->walls.size; i ++) {
    if (
      vertex1.scalarEuclidean(this->walls[i].normal).isEqualToZero() &&
      vertex2.scalarEuclidean(this->walls[i].normal).isEqualToZero()
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
  for (int i = 0; i < this->walls.size; i ++) {
    root = this->walls[i].normal;
    root.removeLastObject();
    *this->walls[i].normal.lastObject() -=
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
  for (int i = 0; i < this->nonRefinedCones.size(); i ++) {
    for (int j = 0; j < this->nonRefinedCones.values[i].walls.size; j ++) {
      root = this->nonRefinedCones.values[i].walls[j].normal;
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
    this->nonRefinedCones.setKeyValue(currentCone.id, currentCone);
    this->convexHull.makeConvexHullOfMeAnd(currentCone);
    this->splittingNormals.addOnTopNoRepetition(newWalls);
  }
  other.getAllWallsConesNoOrientationNoRepetitionNoSplittingNormals(newWalls);
  this->splittingNormals.addOnTopNoRepetition(newWalls);
  this->refineByNormals();
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
  for (int i = 0; i < myCopy.refinedCones.size(); i ++) {
    currentCone = myCopy.refinedCones[i];
    currentCone.translateMeMyLastCoordinateAffinization(translationVector);
    this->refinedCones.setKeyValue(currentCone.id, currentCone);
    this->convexHull.makeConvexHullOfMeAnd(currentCone);
  }
  Vector<Rational> normalNoAffinePart;
  Vector<Rational> newNormal;
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
    this->quasiPolynomials[i].substitute(
      - translateToBeAddedToArgument, currentContribution
    );
    this->quasiPolynomials[i] = currentContribution;
  }
}

std::string PiecewiseQuasipolynomial::toString(FormatExpressions* format) {
  std::stringstream out;
  for (int i = 0; i < this->projectivizedComplex.refinedCones.size(); i ++) {
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
  List<std::string> colors;
  if (numberOfLatticePointsPerDimension < 0) {
    numberOfLatticePointsPerDimension = 0;
  }
  const std::string zeroColor = "gray";
  for (int i = 0; i < this->projectivizedComplex.refinedCones.size(); i ++) {
    std::string chamberWallColor = "black";
    bool isZeroChamber = this->quasiPolynomials[i].isEqualToZero();
    if (isZeroChamber) {
      chamberWallColor = zeroColor;
    }
    this->projectivizedComplex.refinedCones[i].drawMeLastCoordinateAffine(
      false, drawingVariables, format, chamberWallColor
    );
    std::stringstream currentStream;
    currentStream << i + 1;
    Vector<Rational> root =
    this->projectivizedComplex.refinedCones[i].internalPoint();
    root.makeAffineUsingLastCoordinate();
    for (int j = 0; j < this->quasiPolynomials[i].latticeShifts.size; j ++) {
      this->projectivizedComplex.refinedCones[i].getLatticePointsInCone(
        this->quasiPolynomials[i].ambientLatticeReduced,
        this->quasiPolynomials[i].latticeShifts[j],
        numberOfLatticePointsPerDimension,
        true,
        latticePoints,
        distinguishedPoint
      );
      colors.initializeFillInObject(latticePoints.size, chamberWallColor);
      if (restrictingChamber != nullptr) {
        for (int k = 0; k < latticePoints.size; k ++) {
          root = latticePoints[k];
          root.makeAffineUsingLastCoordinate();
          if (!restrictingChamber->isInCone(root)) {
            colors[k] = zeroColor;
          }
        }
      }
      latticePointsFinal.addListOnTop(latticePoints);
      latticePointColors.addListOnTop(colors);
    }
  }
  for (int i = 0; i < latticePointsFinal.size; i ++) {
    drawingVariables.drawCircleAtVectorBufferRational(
      latticePointsFinal[i], latticePointColors[i], 2
    );
    drawingVariables.drawTextAtVector(
      latticePointsFinal[i],
      this->evaluateInputProjectivized(latticePointsFinal[i], nullptr).toString
      (),
      latticePointColors[i]
    );
  }
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
  Vector<Rational> projectivizedInput = input;
  projectivizedInput.setSize(input.size + 1);
  *projectivizedInput.lastObject() = 1;
  return this->evaluateInputProjectivized(projectivizedInput, comments);
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
  // Comment it out as soon as the code has been tested sufficiently.
  for (int i = 0; i < this->projectivizedComplex.refinedCones.size(); i ++) {
    if (!this->projectivizedComplex.refinedCones[i].isInCone(input)) {
      continue;
    }
    Rational altResult =
    this->quasiPolynomials[i].evaluate(affineInput, comments);
    if (result != altResult) {
      global.fatal << "Different values of quasipolynomials. " << global.fatal;
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
    this->projectivizedComplex.refinedCones.size()
  );
  for (int i = 0; i < this->projectivizedComplex.refinedCones.size(); i ++) {
    int oldIndex =
    oldComplex.getLowestIndexRefinedChamberContaining(
      this->projectivizedComplex.refinedCones[i].internalPoint()
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

void PartialFractions::getSum(
  LinearCombination<OnePartialFractionDenominator, Polynomial<LargeInteger> >&
  output
) const {
  output.makeZero();
  this->accumulateSum(output);
}

void PartialFractions::accumulateSum(
  LinearCombination<OnePartialFractionDenominator, Polynomial<LargeInteger> >&
  output
) const {
  if (
    &output == &this->nonReduced ||
    &output == &this->reducedWithElongationRedundancies ||
    &output == &this->reduced
  ) {
    global.fatal
    << "Not allowed to accumulate in an internal collection. "
    << global.fatal;
  }
  output += this->nonReduced;
  output += this->reducedWithElongationRedundancies;
  output += this->reduced;
}

bool PartialFractions::split(Vector<Rational>* indicator) {
  STACK_TRACE("PartialFractions::split");
  if (!this->flagInitialized) {
    this->prepareIndicatorVariables();
    this->prepareCheckSums();
    this->flagInitialized = true;
  }
  if (this->splitPartial()) {
    this->details.takeSnapShotBeforeElongation();
    this->removeRedundantShortRoots(indicator);
  }
  this->compareCheckSums();
  return false;
}

void Cone::changeBasis(Matrix<Rational>& linearMap) {
  List<Wall> newWalls;
  for (const Wall& wall : this->walls) {
    Wall transformedWall;
    transformedWall.normal = wall.normal;
    transformedWall.neighbors = wall.neighbors;
    linearMap.actOnVectorColumn(transformedWall.normal);
  }
  this->walls.clear();
  this->createFromWalls(newWalls, false);
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
  std::string currentString;
  this->toString(currentString);
  this->log << "\nWeyl chamber: " << this->WeylChamber.toString() << "\n";
  this->log << currentString;
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
  this->toString(currentString);
  this->log << currentString;
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
  this->numberOfNonParameters = 0;
  this->numberOfProcessedNonParameters = 0;
  this->lPtoMaximizeLargerDim.size = 0;
  this->lPtoMaximizeSmallerDim.size = 0;
  this->startingRepresentative = startingShift;
  this->finalRepresentatives.size = 0;
  this->complexStartingPerRepresentative.size = 0;
  this->complexRefinedPerRepresentative.size = 0;
  this->conesLargerDimension.addOnTop(allCones);
  this->lPtoMaximizeLargerDim.addOnTop(inequalities);
  this->isInfinity.initializeFillInObject(1, false);
}

void Lattice::applyLinearMap(Matrix<Rational>& linearMap, Lattice& output) {
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
    drawingVariables.initialize();
    out
    << "<br>"
    << this->conesLargerDimension[i].projectivizedCone.
    drawMeToHtmlLastCoordAffine(drawingVariables, *format);
    out
    << "<br>over "
    << this->conesLargerDimension[i].shift.toString()
    << " + "
    << this->conesLargerDimension[i].lattice.toString();
    polynomialPart.makeLinearWithConstantTerm(this->lPtoMaximizeLargerDim[i]);
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
      drawingVariables.initialize();
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
    std::stringstream currentStream;
    currentStream
    << "intersecing lattice "
    << i + 1
    << " out of "
    << this->conesLargerDimension.size;
    report.report(currentStream.str());
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
    std::stringstream currentStream;
    currentStream
    << "Computing representative "
    << i + 1
    << " out of "
    << this->conesLargerDimension.size;
    currentStream
    << "\nSo far "
    << this->finalRepresentatives.size
    << " found.";
    report2.report(currentStream.str());
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
        this->complexStartingPerRepresentative[i].refinedCones.setKeyValue(
          this->conesLargerDimension[j].projectivizedCone.id,
          this->conesLargerDimension[j].projectivizedCone
        );
        this->startingLPtoMaximize[i].addOnTop(
          this->lPtoMaximizeLargerDim[j]
        );
      }
    }
  }
}

void ConeLatticeAndShiftMaxComputation::findExtremaParametricStep4() {
  this->complexRefinedPerRepresentative.setSize(
    this->finalRepresentatives.size
  );
  this->maximaCandidates.setSize(this->finalRepresentatives.size);
  ProgressReport report;
  for (int i = 0; i < this->finalRepresentatives.size; i ++) {
    ConeCollection& currentComplex = this->complexRefinedPerRepresentative[i];
    currentComplex.initFromConeWalls(
      this->complexStartingPerRepresentative[i].refinedCones.values, true
    );
    std::stringstream currentStream;
    currentStream
    << "Processing representative "
    << i + 1
    << " out of "
    << this->finalRepresentatives.size;
    report.report(currentStream.str());
    currentComplex.refineByNormals();
    this->maximaCandidates[i].setSize(currentComplex.refinedCones.size());
    for (int j = 0; j < currentComplex.refinedCones.size(); j ++) {
      for (
        int k = 0; k < this->complexStartingPerRepresentative[k].refinedCones
        .size(); k ++
      ) {
        if (
          this->complexStartingPerRepresentative[i].refinedCones[k].isInCone(
            currentComplex.refinedCones[j].internalPoint()
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
      this->complexRefinedPerRepresentative[i].refinedCones.size()
    );
    this->finalRepresentatives[i].setSize(
      this->complexRefinedPerRepresentative[i].refinedCones.size()
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
  ProgressReport report1;
  ProgressReport report2;
  ProgressReport report3;
  for (
    ; this->numberOfProcessedNonParameters < this->numberOfNonParameters;
    this->numberOfProcessedNonParameters ++
  ) {
    while (this->conesLargerDimension.size > 0) {
      ConeLatticeAndShift& currentCLS =
      *this->conesLargerDimension.lastObject();
      if (
        this->lPtoMaximizeLargerDim.lastObject()->size !=
        currentCLS.getDimensionAffine() + 1
      ) {
        global.fatal
        << "In ConeLatticeAndShiftMaxComputation::findExtremaParametricStep1: "
        << "dimensions don't match. "
        << global.fatal;
      }
      if (!this->lPtoMaximizeLargerDim.lastObject()->isEqualToZero()) {
        currentCLS.findExtremaInDirectionOverLatticeOneNonParametric(
          *this->lPtoMaximizeLargerDim.lastObject(),
          this->lPtoMaximizeSmallerDim,
          this->conesSmallerDimension
        );
      }
      this->conesLargerDimension.size --;
      this->lPtoMaximizeLargerDim.size --;
      std::stringstream currentStream1;
      std::stringstream currentStream2;
      std::stringstream currentStream3;
      currentStream1
      << "Processing "
      << this->numberOfProcessedNonParameters + 1
      << " out of "
      << this->numberOfNonParameters;
      currentStream2 << "Remaining cones: " << this->conesLargerDimension.size;
      currentStream3
      << "Cones smaller dim total: "
      << this->conesSmallerDimension.size;
      report1.report(currentStream1.str());
      report2.report(currentStream2.str());
      report3.report(currentStream3.str());
    }
    this->lPtoMaximizeLargerDim = this->lPtoMaximizeSmallerDim;
    this->conesLargerDimension = this->conesSmallerDimension;
    this->conesSmallerDimension.size = 0;
    this->lPtoMaximizeSmallerDim.size = 0;
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
  ConeLatticeAndShift currentConeLatticeShift;
  Vectors<Rational> newNormals = this->projectivizedCone.getAllNormals();
  Rational firstCoordNewNormal;
  for (int i = 0; i < newNormals.size; i ++) {
    firstCoordNewNormal = newNormals[i][0];
    newNormals[i].shiftToTheLeftOnePosition();
    newNormals[i] += preferredNormal * firstCoordNewNormal;
  }
  currentConeLatticeShift.projectivizedCone.createFromNormals(
    newNormals, this->projectivizedCone.getAllNeighbors()
  );
  currentConeLatticeShift.shift = this->shift;
  currentConeLatticeShift.shift.shiftToTheLeftOnePosition();
  this->lattice.applyLinearMap(
    projectionLatticeLevel, currentConeLatticeShift.lattice
  );
  Vector<Rational> root;
  root = lpToMaximizeAffine.getshiftToTheLeftOnePositionition();
  root += preferredNormal * lpToMaximizeAffine[0];
  if (!currentConeLatticeShift.projectivizedCone.flagIsTheZeroCone) {
    outputAppend.addOnTop(currentConeLatticeShift);
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
  if (!this->projectivizedCone.vertices.linearSpanContainsVector(direction)) {
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
    std::stringstream currentStream;
    currentStream
    << "<hr><hr><hr><hr>The bad cone:"
    << this->projectivizedCone.toString(&format);
    report.report(currentStream.str());
  }
  ConeCollection complexBeforeProjection;
  complexBeforeProjection.initialize();
  complexBeforeProjection.refinedCones.setKeyValue(
    this->projectivizedCone.id, this->projectivizedCone
  );
  complexBeforeProjection.convexHull.makeConvexHullOfMeAnd(
    this->projectivizedCone
  );
  if (direction.scalarEuclidean(lpToMaximizeAffine).isNegative()) {
    direction.negate();
  }
  complexBeforeProjection.slicingDirections.addOnTop(direction);
  complexBeforeProjection.slicingDirections.addOnTop(- direction);
  complexBeforeProjection.refineByDirections(&report);
  Wall wall;
  Vector<Rational> extraEquation;
  Vector<Rational> exitNormalAffine;
  Vector<Rational> enteringNormalAffine;
  Vector<Rational> exitNormalLatticeLevel;
  Vector<Rational> exitNormalShiftedAffineProjected;
  Vector<Rational> directionSmallerDimension;
  Vector<Rational> directionSmallerDimensionOnLattice;
  Vector<Rational> shiftReduced = this->shift;
  this->lattice.reduceVector(shiftReduced);
  Vectors<Rational> exitRepresentatives;
  Vectors<Rational> exitWallsShifted;
  Lattice exitRougherLattice;
  ConeLatticeAndShift coneLatticeShift;
  directionSmallerDimension.makeEi(dimensionProjectivized - 1, 0);
  List<Wall> newWalls;
  for (int i = 0; i < complexBeforeProjection.refinedCones.size(); i ++) {
    const Cone& currentCone = complexBeforeProjection.refinedCones[i];
    int numberOfNonPerpendicularWalls = 0;
    newWalls.size = 0;
    bool foundEnteringNormal = false;
    bool foundExitNormal = false;
    for (int j = 0; j < currentCone.walls.size; j ++) {
      Vector<Rational>& currentNormal = currentCone.walls[j].normal;
      if (currentNormal[0].isEqualToZero()) {
        wall.normal = currentNormal.getshiftToTheLeftOnePositionition();
        newWalls.addOnTop(wall);
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
            directionSmallerDimension,
            directionSmallerDimensionOnLattice,
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
      projectionLatticeLevel, coneLatticeShift.lattice
    );
    for (int j = 0; j < exitRepresentatives.size; j ++) {
      exitRepresentatives[j] += shiftReduced;
      Lattice::
      getClosestPointInDirectionOfNormalToAffineWallMovingIntegralStepsInDirection
      (
        exitRepresentatives[j],
        exitNormalAffine,
        directionSmallerDimensionOnLattice,
        exitRepresentatives[j]
      );
    }
    global.comments << "<hr><hr><hr>" << currentCone.toString(&format);
    global.comments
    << "<br>Entering normal: "
    << ((foundEnteringNormal) ? enteringNormalAffine.toString() : "not found");
    global.comments
    << "<br>Exit normal: "
    << ((foundExitNormal) ? exitNormalAffine.toString() : "not found");
    global.comments
    << "<br>The shifted lattice representatives: "
    << exitRepresentatives.toString()
    << "<br>exitNormalsShiftedAffineProjected";
    if (newWalls.size <= 0) {
      global.fatal << "New normals missing. " << global.fatal;
    }
    for (int j = 0; j < exitRepresentatives.size; j ++) {
      coneLatticeShift.projectivizedCone.walls = newWalls;
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
        Wall extraWall;
        extraWall.normal = extraEquation;
        coneLatticeShift.projectivizedCone.walls.addOnTop(extraWall);
      }
      wall.normal = lpToMaximizeAffine.getshiftToTheLeftOnePositionition();
      wall.normal -= exitNormalShiftedAffineProjected * lpToMaximizeAffine[0] /
      exitNormalAffine[0];
      outputAppendLPToMaximizeAffine.addOnTop(wall.normal);
      if (coneLatticeShift.projectivizedCone.walls.size <= 0) {
        global.fatal << "Projectivized cone has no normals. " << global.fatal;
      }
      Vectors<Rational> roots =
      coneLatticeShift.projectivizedCone.getAllNormals();
      coneLatticeShift.projectivizedCone.createFromNormals(
        roots, coneLatticeShift.projectivizedCone.getAllNeighbors()
      );
      if (!coneLatticeShift.projectivizedCone.flagIsTheZeroCone) {
        projectionLatticeLevel.actOnVectorColumn(
          exitRepresentatives[j], coneLatticeShift.shift
        );
        outputAppend.addOnTop(coneLatticeShift);
        if (coneLatticeShift.getDimensionProjectivized() == 0) {
          report.report(roots.toString());
          while (true) {}
        }
        if (
          coneLatticeShift.getDimensionProjectivized() !=
          dimensionProjectivized - 1
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
  MathRoutines::nChooseK(toBeSplit.walls.size, dimensionMinusTwo);
  Selection selection;
  Selection nonPivotPoints;
  selection.initialize(toBeSplit.walls.size);
  Matrix<Rational> toBeEliminated;
  toBeEliminated.initialize(dimensionMinusTwo + 1, dimension);
  Vector<Rational> root;
  for (int i = 0; i < numberOfCycles; i ++) {
    selection.incrementSelectionFixedCardinalityReturnFalseIfPastLast(
      dimensionMinusTwo
    );
    for (int j = 0; j < dimensionMinusTwo; j ++) {
      Vector<Rational>& currentNormal =
      toBeSplit.walls[selection.elements[j]].normal;
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

bool ConeCollection::splitVerticesByNormal(
  const Cone& toBeSliced,
  const Vector<Rational>& normal,
  Vectors<Rational>& outputPlusVertices,
  Vectors<Rational>& outputMinusVertices,
  HashedList<Vector<Rational> >& outputZeroVertices
) {
  Rational scalarProduct;
  for (int i = 0; i < toBeSliced.vertices.size; i ++) {
    normal.scalarEuclidean(toBeSliced.vertices[i], scalarProduct);
    if (scalarProduct.isPositive()) {
      outputPlusVertices.addOnTop(toBeSliced.vertices[i]);
    }
    if (scalarProduct.isEqualToZero()) {
      outputZeroVertices.addOnTopNoRepetition(toBeSliced.vertices[i]);
    }
    if (scalarProduct.isNegative()) {
      outputMinusVertices.addOnTop(toBeSliced.vertices[i]);
    }
  }
  if (outputPlusVertices.size == 0 || outputMinusVertices.size == 0) {
    return false;
  }
  return true;
}

bool ConeCollection::splitChamber(
  const Cone& toBeSliced, const Vector<Rational>& normal
) {
  STACK_TRACE("ConeCollection::splitChamber");
  Cone newPlusCone(this->conesCreated);
  this->conesCreated ++;
  Cone newMinusCone(this->conesCreated);
  this->conesCreated ++;
  bool needToRecomputeVertices = (
    toBeSliced.getAllNormals().getRankLinearSpan() < this->getDimension()
  );
  newPlusCone.flagIsTheZeroCone = false;
  newMinusCone.flagIsTheZeroCone = false;
  HashedList<Vector<Rational> > zeroVertices;
  if (
    !this->splitVerticesByNormal(
      toBeSliced,
      normal,
      newPlusCone.vertices,
      newMinusCone.vertices,
      zeroVertices
    )
  ) {
    return false;
  }
  newPlusCone.payload = toBeSliced.payload;
  newMinusCone.payload = toBeSliced.payload;
  this->getNewVerticesAppend(toBeSliced, normal, zeroVertices);
  for (int i = 0; i < toBeSliced.walls.size; i ++) {
    Wall wall;
    wall.normal = toBeSliced.walls[i].normal;
    if (newPlusCone.vertices.hasAnElementPerpendicularTo(wall.normal)) {
      newPlusCone.walls.addOnTop(wall);
    }
    if (newMinusCone.vertices.hasAnElementPerpendicularTo(wall.normal)) {
      newMinusCone.walls.addOnTop(wall);
    }
  }
  Wall wall;
  wall.normal = normal;
  wall.neighbors.addOnTop(newMinusCone.id);
  newPlusCone.walls.addOnTop(wall);
  wall.normal.negate();
  wall.neighbors[0] = newPlusCone.id;
  newMinusCone.walls.addOnTop(wall);
  newPlusCone.vertices.addListOnTop(zeroVertices);
  newMinusCone.vertices.addListOnTop(zeroVertices);
  if (needToRecomputeVertices) {
    newPlusCone.createFromWalls(newPlusCone.walls, false);
    newMinusCone.createFromWalls(newMinusCone.walls, false);
  }
  this->attachNeighbbors(
    toBeSliced, List<Cone*>({&newMinusCone, &newPlusCone})
  );
  this->addNonRefinedCone(newPlusCone);
  this->addNonRefinedCone(newMinusCone);
  this->processNeighborsOfNewlyAddedCone(newPlusCone);
  this->processNeighborsOfNewlyAddedCone(newMinusCone);
  return true;
}

std::string Cone::displayId() const {
  std::stringstream out;
  if (this->payload.displayNumber > 0) {
    out << this->payload.displayNumber;
    return out.str();
  }
  out << "_" << this->id;
  return out.str();
}

bool Cone::hasNormal(const Vector<Rational>& normalOfWall) const {
  for (const Wall& wall : this->walls) {
    if (normalOfWall == wall.normal) {
      return true;
    }
  }
  return false;
}

bool Cone::hasCommonPossiblyIrregularWall(
  Cone& neighborCandidate, Vector<Rational>& normalOfWall
) {
  STACK_TRACE("Cone::hasCommonPossiblyIrregularWall");
  if (
    !this->hasNormal(normalOfWall) ||
    !neighborCandidate.hasNormal(- normalOfWall)
  ) {
    global.fatal << "Cones must both have the given normal. " << global.fatal;
  }
  if (this->twoPlanesSeparateUs(neighborCandidate, normalOfWall)) {
    return false;
  }
  if (neighborCandidate.twoPlanesSeparateUs(*this, - normalOfWall)) {
    return false;
  }
  return true;
}

bool Cone::twoPlanesSeparateUs(
  Cone& neighborCandidate, const Vector<Rational>& normalOfWall
) {
  for (const Vector<Rational>& vertex : this->vertices) {
    for (const Wall& wall : this->walls) {
      if (wall.normal == normalOfWall) {
        // This is the original wall that separates the two cones.
        continue;
      }
      if (!wall.normal.scalarEuclidean(vertex).isEqualToZero()) {
        continue;
      }
      if (this->isSeparatingPlane(neighborCandidate, wall)) {
        return true;
      }
    }
  }
  return false;
}

bool Cone::isSeparatingPlane(Cone& neighborCandidate, const Wall& wall) {
  for (const Vector<Rational>& otherVertex : neighborCandidate.vertices) {
    if (wall.isInOpenHalfSpace(otherVertex)) {
      return false;
    }
  }
  return true;
}

void ConeCollection::removeIdFromNeighbors(const Cone& cone) {
  STACK_TRACE("ConeCollection::removeIdFromNeighbors");
  for (const Wall& wall : cone.walls) {
    for (int id : wall.neighbors) {
      Cone* neighbor = this->getConeByIdNonConst(id);
      if (neighbor == nullptr) {
        continue;
      }
      neighbor->removeNeighbor(cone.id);
    }
  }
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

bool ConeCollection::refineOneByNormals(
  Cone& toBeRefined, List<Cone>& output
) {
  STACK_TRACE("ConeCollection::refineByNormals");
  output.clear();
  int& counter = toBeRefined.payload.lowestSlicingIndex;
  for (; counter < this->splittingNormals.size; counter ++) {
    if (this->splitChamber(toBeRefined, this->splittingNormals[counter])) {
      return true;
    }
  }
  return false;
}

void ConeCollection::refineAllConesWithWallsWithMultipleNeighbors(
  ProgressReport* report
) {
  STACK_TRACE("ConeCollection::refineAllConesWithWallsWithMultipleNeighbors");
  List<Cone> split;
  while (this->conesWithIrregularWalls.size() > 0) {
    this->reportStats(report);
    int index = this->conesWithIrregularWalls.size() - 1;
    Cone cone = this->conesWithIrregularWalls.values[index];
    this->conesWithIrregularWalls.removeIndex(index);
    this->splitConeByMultipleNeighbors(cone);
    this->addHistoryPoint();
  }
}

void ConeCollection::splitConeByMultipleNeighbors(Cone& input) {
  STACK_TRACE("ConeCollection::splitConeByMultipleNeighbors");
  for (Wall& wall : input.walls) {
    if (wall.neighbors.size > 1) {
      this->splitConeByMultipleNeighbors(input, wall);
      return;
    }
  }
  this->addNonRefinedCone(input);
}

void ConeCollection::splitConeByMultipleNeighbors(Cone& input, Wall& wall) {
  STACK_TRACE("ConeCollection::splitConeByMultipleNeighbors");
  Wall wallWithReducedNeighborCount = wall;
  int neighborIndex = wallWithReducedNeighborCount.neighbors.popLastObject();
  Cone& neighbor = *this->getConeByIdNonConst(neighborIndex);
  HashedList<Vector<Rational> > verticesBelongingToWall;
  input.getAllVerticesPerpendicularTo(wall.normal, verticesBelongingToWall);
  Vectors<Rational> neighborVertices;
  neighbor.getAllVerticesPerpendicularTo(wall.normal, neighborVertices);
  for (Wall& slicer : neighbor.walls) {
    if (slicer.normal == - wall.normal) {
      continue;
    }
    if (!neighborVertices.hasAnElementPerpendicularTo(slicer.normal)) {
      // The candidate slicing wall has no common vertex with the current wall.
      continue;
    }
    if (!slicer.hasAtLeastOnePointInOpenHalfSpace(input.vertices)) {
      // Both the original wall normal and the slicer normal separate the two
      // chambers.
      // The two chambers are not honest neighbors, as the
      // region they touch along is of not of the maximum possible dimension
      // n-1
      // (empty intersection is possible too).
      wall.neighbors = wallWithReducedNeighborCount.neighbors;
      neighbor.removeNeighbor(input.id);
      this->addNonRefinedCone(input);
    }
    if (this->splitChamber(input, slicer.normal)) {
      return;
    }
  }
  this->addNonRefinedCone(input);
}

bool ConeCollection::isSpannedByDirections(const Vector<Rational>& planeNormal)
{
  STACK_TRACE("ConeCollection::isSpannedByDirections");
  Vectors<Rational> directionsPerpendicularToNormal;
  for (Vector<Rational>& direction : this->slicingDirections) {
    if (planeNormal.scalarEuclidean(direction).isEqualToZero()) {
      directionsPerpendicularToNormal.addOnTop(direction);
    }
  }
  int rankSpan = directionsPerpendicularToNormal.getRankLinearSpan();
  return rankSpan == this->getDimension() - 1;
}

bool ConeCollection::allExitWallsAreVisited(
  Cone& toBeRefined,
  const Vector<Rational>& direction,
  List<Wall>& outputExitWalls
) {
  STACK_TRACE("ConeCollection::allExitWallsAreVisited");
  outputExitWalls.clear();
  for (const Wall& wall : toBeRefined.walls) {
    if (!direction.scalarEuclidean(wall.normal).isPositive()) {
      continue;
    }
    for (int id : wall.neighbors) {
      Cone& neighbor = this->getConeByIdNonConstNoFail(id);
      if (!neighbor.payload.visited) {
        return false;
      }
    }
    outputExitWalls.addOnTop(wall);
  }
  if (outputExitWalls.size <= 0) {
    global.comments << "Unexpected: number of exit walls is zero. ";
  }
  return true;
}

bool ConeCollection::refineOneByOneDirection(
  Cone& toBeRefined, int directionIndex
) {
  STACK_TRACE("ConeCollection::refineOneByOneDirection");
  const Vector<Rational>& direction = this->slicingDirections[directionIndex];
  List<Wall> exitWalls;
  if (!this->allExitWallsAreVisited(toBeRefined, direction, exitWalls)) {
    this->addNonRefinedCone(toBeRefined);
    return false;
  }
  if (exitWalls.size == 1) {
    // The chamber is already refined;
    this->addRefinedCone(toBeRefined);
    return true;
  }
  if (this->flagUseSpannedSlices) {
    return
    this->refineOneByOneDirectionSpannedSlices(
      toBeRefined, exitWalls, directionIndex
    );
  } else {
    return
    this->refineOneByOneDirectionArbitrarySlices(
      toBeRefined, exitWalls, direction
    );
  }
}

bool ConeCollection::refineOneByOneDirectionArbitrarySlices(
  Cone& toBeRefined, List<Wall>& exitWalls, const Vector<Rational>& direction
) {
  STACK_TRACE("ConeCollection::refineOneByOneDirectionArbitrarySlices");
  MapList<int, Cone> candidates;
  int firstIndex = this->conesCreated;
  for (int i = 0; i < exitWalls.size; i ++) {
    Cone current(this->conesCreated);
    current.payload = toBeRefined.payload;
    this->conesCreated ++;
    Wall exitWallWithoutNeighbors;
    exitWallWithoutNeighbors.normal = exitWalls[i].normal;
    current.walls.addOnTop(exitWallWithoutNeighbors);
    for (int j = 0; j < exitWalls.size; j ++) {
      if (j == i) {
        continue;
      }
      Wall newWall;
      toBeRefined.produceNormalSeparatingWallsInDirection(
        direction, exitWalls[i].normal, exitWalls[j].normal, newWall.normal
      );
      newWall.neighbors.addOnTop(firstIndex + j);
      current.walls.addOnTop(newWall);
    }
    for (Wall& wall : toBeRefined.walls) {
      Wall withoutNeighbors;
      withoutNeighbors.normal = wall.normal;
      current.walls.addOnTop(withoutNeighbors);
    }
    candidates.setKeyValue(current.id, current);
  }
  for (Cone& candidate : candidates.values) {
    candidate.createFromWalls(candidate.walls, true);
  }
  HashedList<Vector<Rational> > startingVertices;
  startingVertices.addListOnTop(toBeRefined.vertices);
  this->attachNeighbbors(toBeRefined, candidates);
  for (Cone& cone : candidates.values) {
    this->addNonRefinedCone(cone);
  }
  for (Cone& cone : candidates.values) {
    this->processNeighborsOfNewlyAddedCone(cone);
  }
  this->addHistoryPoint();
  this->checkConsistencyFullWithoutDebugMessage();
  return true;
}

void ConeCollection::addHistoryPoint() {
  if (!this->flagRecordSlicingHistory) {
    return;
  }
  static const int maximumHistorySize = 100;
  if (this->historyHTML.size >= maximumHistorySize) {
    if (this->historyHTML.size == maximumHistorySize) {
      std::stringstream comment;
      comment
      << "Not recording further history: maximum of "
      << maximumHistorySize
      << " steps recorded.";
      this->historyHTML.addOnTop(comment.str());
    }
    return;
  }
  std::stringstream out;
  out
  << "<hr>"
  << this->toStringNeighborGraph()
  << "<br>"
  << this->toHTMLGraphicsOnly(false, false);
  this->historyHTML.addOnTop(out.str());
}

void ConeCollection::attachNeighbbors(
  const Cone& toBeReplaced, MapList<int, Cone>& allCandidates
) {
  STACK_TRACE("ConeCollection::attachNeighbbors");
  List<Cone*> allCones;
  for (Cone& cone : allCandidates.values) {
    allCones.addOnTop(&cone);
  }
  this->attachNeighbbors(toBeReplaced, allCones);
}

void ConeCollection::attachNeighbbors(
  const Cone& toBeReplaced, const List<Cone*>& newCones
) {
  STACK_TRACE("ConeCollection::attachNeighbbors");
  for (const Wall& wall : toBeReplaced.walls) {
    this->replaceConeAdjacentToWall(toBeReplaced, wall, newCones);
  }
}

void ConeCollection::replaceConeAdjacentToWall(
  const Cone& toBeReplaced, const Wall& wall, const List<Cone*>& newCones
) {
  STACK_TRACE("ConeCollection::replaceConeAdjacentToWall");
  for (int idNeighbor : wall.neighbors) {
    Cone* neighbor = this->getConeByIdNonConst(idNeighbor);
    if (neighbor == nullptr) {
      global.fatal
      << "Failed to find cone of id: "
      << idNeighbor
      << ". New cones: "
      << newCones
      << global.fatal;
      return;
    }
    this->replaceConeInNeighbor(
      *neighbor, - wall.normal, toBeReplaced.id, newCones
    );
  }
}

void ConeCollection::replaceConeInNeighbor(
  Cone& neighbor,
  const Vector<Rational>& wallNormal,
  int idReplacedCone,
  const List<Cone*>& newCones
) {
  STACK_TRACE("ConeCollection::replaceConeInNeighbor");
  Wall& neighborWall = neighbor.getWallWithNormalNoFail(wallNormal);
  for (int i = neighborWall.neighbors.size - 1; i >= 0; i --) {
    if (neighborWall.neighbors[i] != idReplacedCone) {
      continue;
    }
    neighborWall.neighbors.removeIndexShiftDown(i);
    this->addAllMutualNeighborsIfAdjacent(neighbor, neighborWall, newCones);
    return;
  }
}

void ConeCollection::addAllMutualNeighborsIfAdjacent(
  Cone& original, Wall& wallInOriginal, const List<Cone*>& incoming
) {
  for (Cone * cone : incoming) {
    this->addMutualNeighborsIfAdjacent(original, wallInOriginal, *cone);
  }
}

void ConeCollection::addMutualNeighborsIfAdjacent(
  Cone& original, Wall& wallInOriginal, Cone& incoming
) {
  Vector<Rational> oppositeNormal = - wallInOriginal.normal;
  if (!incoming.hasNormal(oppositeNormal)) {
    // The chambers aren't neighbors along this wall.
    return;
  }
  if (
    !original.hasCommonPossiblyIrregularWall(incoming, wallInOriginal.normal)
  ) {
    // The chambers are on the opposite sides of a plane that acts as a wall
    // for both chambers. However, the set of common points is less than n-1
    // dimensional
    // (all other dimensions allowed, including n-2 dimensional and empty
    // (dimension -\infty).
    return;
  }
  Wall& wallInIncoming = incoming.getWallWithNormalNoFail(oppositeNormal);
  wallInOriginal.neighbors.addOnTopNoRepetition(incoming.id);
  wallInIncoming.neighbors.addOnTopNoRepetition(original.id);
}

void ConeCollection::initializeFromDirections(
  const List<Vector<Rational> >& inputVectors
) {
  STACK_TRACE("ConeCollection::initializeFromDirections");
  this->initialize();
  Cone startingCone(0);
  this->conesCreated ++;
  Vectors<Rational> input = inputVectors;
  startingCone.createFromVertices(input);
  this->nonRefinedCones.setKeyValue(startingCone.id, startingCone);
  this->convexHull.makeConvexHullOfMeAnd(startingCone);
  this->slicingDirections.addListOnTop(inputVectors);
}

void ConeCollection::initializeFromDirectionsAndRefine(
  Vectors<Rational>& inputVectors
) {
  STACK_TRACE("ConeCollection::initializeFromDirectionsAndRefine");
  this->initializeFromDirections(inputVectors);
  ProgressReport report;
  this->refineByDirectionsAndSort(report);
}

void ConeCollection::sort() {
  List<Cone> sortedValues = this->refinedCones.values;
  sortedValues.quickSortAscending();
  this->refinedCones.clear();
  for (int i = 0; i < sortedValues.size; i ++) {
    sortedValues[i].payload.displayNumber = i + 1;
    this->refinedCones.setKeyValue(sortedValues[i].id, sortedValues[i]);
  }
}

void ConeCollection::refineByNormals() {
  STACK_TRACE("ConeCollection::refineByNormals");
  ProgressReport report;
  List<Cone> incoming;
  while (this->nonRefinedCones.size() > 0) {
    int indexLast = this->nonRefinedCones.size() - 1;
    Cone toBeReduced = this->nonRefinedCones.values[indexLast];
    this->nonRefinedCones.removeKey(this->nonRefinedCones.keys[indexLast]);
    bool isAlreadyRefined = !this->refineOneByNormals(toBeReduced, incoming);
    if (isAlreadyRefined) {
      this->refinedCones.setKeyValue(toBeReduced.id, toBeReduced);
    } else {
      for (const Cone& cone : incoming) {
        this->nonRefinedCones.setKeyValue(cone.id, cone);
      }
    }
    std::stringstream out;
    out
    << "Refined "
    << this->refinedCones.size()
    << " cones; remaining non-refined: "
    << this->nonRefinedCones.size();
    report.report(out.str());
  }
}

void ConeCollection::refineByDirectionsAndSort(ProgressReport& report) {
  this->refineByDirections(&report);
  this->sort();
}

void ConeCollection::refineByDirections(ProgressReport* report) {
  STACK_TRACE("ConeCollection::refineByDirections");
  for (int i = 0; i < this->slicingDirections.size; i ++) {
    this->markAllConesNonRefined(i);
    this->refineByOneDirection(i, report);
  }
  if (this->flagAmalgamateChambers) {
    // We want only spanned slices,
    // By our strategy for splitting cones,
    // each pair of neighbors are
    // separated by a plane that is
    // spanned by n-1=this->getDimension()-1
    // of the original vectors.
    // However, the wall (the subset of the plane that
    // actually separates the neighbors)
    // may not be a subset of a wall of
    // of a cone formed by taking all non-negative linear
    // combinations of this->getDimension()
    // of the original vectors. Example:
    // ConeDecomposition((1,0,0),(0,1,0),(0,0,1),(1,1,1));
    // will generate 4 combinatorial chambers, rather than 3.
    // 2 of the 4 chambers will be separated by a plane that
    // is cutting off one side of the 4 original cones.
    // Howerver, the section of the plane that separates the
    // two neighbors lies outside of both of the 4 original cones that
    // are cut off by that plane.
    this->mergeChambers();
  }
}

void ConeCollection::mergeChambers() {
  STACK_TRACE("ConeCollection::mergeChambers");
  Selection selection;
  selection.initialize(this->slicingDirections.size);
  int dimension = this->getDimension();
  MapList<List<unsigned char>, HashedList<int> > container1;
  MapList<List<unsigned char>, HashedList<int> > container2;
  MapList<List<unsigned char>, HashedList<int> >* currentConeFamilies =
  &container1;
  MapList<List<unsigned char>, HashedList<int> >* nextConeFamilies =
  &container2;
  currentConeFamilies->setKeyValue(List<unsigned char>(), HashedList<int>());
  for (Cone& cone : this->refinedCones.values) {
    cone.payload.hashOfContainingSimplices.clear();
    currentConeFamilies->getValueCreateNoInitialization(
      cone.payload.hashOfContainingSimplices
    ).addOnTop(cone.id);
  }
  LargeInteger total =
  selection.getNumberOfCombinationsFixedCardinality(dimension);
  LargeInteger count;
  ProgressReport report(100, "simplexCones");
  while (
    selection.incrementSelectionFixedCardinalityReturnFalseIfPastLast(
      dimension
    )
  ) {
    count ++;
    if (report.tickAndWantReport()) {
      std::stringstream out;
      out << "Processed " << count << " selections out of " << total;
      report.report(out.str());
    }
    Cone cone;
    this->slicingDirections.subSelection(selection, cone.vertices);
    if (!cone.createFromVertices(cone.vertices)) {
      continue;
    }
    this->accountOneDirectionSimplex(
      cone, *currentConeFamilies, *nextConeFamilies
    );
    MathRoutines::swap(currentConeFamilies, nextConeFamilies);
    nextConeFamilies->clear();
    if (currentConeFamilies->size() == 0) {
      break;
    }
  }
  for (HashedList<int>& family : currentConeFamilies->values) {
    this->mergeOneChamberFamily(family);
  }
}

void ConeCollection::mergeChambersAlongWall(
  Wall& oldWall,
  MapList<Vector<Rational>, Wall>& outputNewWalls,
  HashedList<Vector<Rational> >& outputMergedWalls
) {
  Vector<Rational> normal = oldWall.normal;
  Vector<Rational> minusNormal = - normal;
  if (
    outputNewWalls.contains(minusNormal) ||
    outputMergedWalls.contains(normal)
  ) {
    outputMergedWalls.addOnTopNoRepetition(normal);
    outputMergedWalls.addOnTop(minusNormal);
    outputNewWalls.removeKey(normal);
    outputNewWalls.removeKey(minusNormal);
    return;
  }
  if (!outputNewWalls.contains(normal)) {
    outputNewWalls.setKeyValue(normal, oldWall);
    return;
  }
  for (int neighbor : oldWall.neighbors) {
    outputNewWalls.getValueCreateNoInitialization(normal).neighbors.
    addOnTopNoRepetition(neighbor);
  }
}

void ConeCollection::mergeOneChamberFamily(HashedList<int>& family) {
  MapList<Vector<Rational>, Wall> newWalls;
  HashedList<Vector<Rational> > mergedWalls;
  for (int id : family) {
    Cone& cone = *this->getConeByIdNonConst(id);
    for (Wall& oldWall : cone.walls) {
      this->mergeChambersAlongWall(oldWall, newWalls, mergedWalls);
    }
  }
  this->conesCreated ++;
  Cone result(this->conesCreated);
  if (!result.createFromWalls(newWalls.values, true)) {
    global.fatal << "Unexpected failure to create cone. " << global.fatal;
  }
  if (result.walls.size != newWalls.size()) {
    // Most articles I've read imply that the combinatorial chambers must be
    // convex.
    // However, I don't recall reading a proof of that.
    // Is this actually possible?
    global.comments << "<br>Found non-convex combinatorial chamber!";
    return;
  }
  this->addRefinedCone(result);
  for (int id : family) {
    Cone& cone = *this->getConeByIdNonConst(id);
    this->attachNeighbbors(cone, List<Cone*>({&result}));
    this->refinedCones.removeKey(id);
  }
  this->addHistoryPoint();
}

void ConeCollection::accountOneDirectionSimplex(
  Cone& directionSimplex,
  MapList<List<unsigned char>, HashedList<int> >& currentConeFamilies,
  MapList<List<unsigned char>, HashedList<int> >& nextConeFamilies
) {
  for (HashedList<int>& ids : currentConeFamilies.values) {
    for (int id : ids) {
      Cone& cone = this->refinedCones.getValueNoFailNonConst(id);
      if (directionSimplex.isInCone(cone.internalPointCached())) {
        cone.payload.incrementHashOfContainingSimplices(1);
      } else {
        cone.payload.incrementHashOfContainingSimplices(0);
      }
      HashedList<int>& newFamily =
      nextConeFamilies.getValueCreate(
        cone.payload.hashOfContainingSimplices, HashedList<int>()
      );
      newFamily.addOnTop(id);
    }
  }
  for (int i = nextConeFamilies.size() - 1; i >= 0; i --) {
    HashedList<int>& value = nextConeFamilies.values[i];
    if (value.size == 1) {
      nextConeFamilies.removeIndex(i);
    }
  }
}

void ConeCollection::markAllConesNonRefined(int directionIndex) {
  STACK_TRACE("ConeCollection::markAllConesNonRefined");
  for (int i = this->refinedCones.size() - 1; i >= 0; i --) {
    this->addNonRefinedCone(this->refinedCones.values[i]);
    this->refinedCones.removeIndex(i);
  }
  for (Cone& cone : this->nonRefinedCones.values) {
    cone.payload.visited = false;
    cone.payload.lowestSlicingIndex = directionIndex - 1;
  }
}

bool ConeCollection::isAddmissibleConeSplit(
  const Vector<Rational>& sliceNormal,
  const Vector<Rational>& wall1,
  const Vector<Rational>& wall2
) {
  if (sliceNormal.size == 2) {
    return true;
  }
  Vectors<Rational> vertices;
  bool hasVertexInWedge = false;
  bool hasVertexInOppositeWedge = false;
  for (const Vector<Rational>& vertex : this->slicingDirections) {
    bool vertexIn = sliceNormal.scalarEuclidean(vertex).isEqualToZero();
    if (vertexIn) {
      vertices.addOnTop(vertex);
    }
    if (
      vertexIn &&
      wall1.scalarEuclidean(vertex).isPositive() &&
      wall2.scalarEuclidean(vertex).isPositive()
    ) {
      hasVertexInWedge = true;
    }
    if (
      vertexIn &&
      wall1.scalarEuclidean(vertex).isNonPositive() &&
      wall2.scalarEuclidean(vertex).isNonPositive()
    ) {
      hasVertexInOppositeWedge = true;
    }
  }
  int rank = vertices.getRankLinearSpan();
  if (rank != sliceNormal.size - 1) {
    // The wall doesn't pass through vertices that span it.
    return false;
  }
  return hasVertexInWedge && hasVertexInOppositeWedge;
}

bool ConeCollection::computeSplittingNormal(
  Cone& toBeSliced,
  List<Wall>& exitWalls,
  int directionIndex,
  Vector<Rational>& normal
) {
  Vector<Rational>& direction = this->slicingDirections[directionIndex];
  for (int i = 0; i < exitWalls.size; i ++) {
    const Vector<Rational>& exitNormal1 = exitWalls[i].normal;
    for (int j = i + 1; j < exitWalls.size; j ++) {
      const Vector<Rational>& exitNormal2 = exitWalls[j].normal;
      if (
        !toBeSliced.produceNormalSeparatingWallsInDirection(
          direction, exitNormal1, exitNormal2, normal
        )
      ) {
        continue;
      }
      if (this->isAddmissibleConeSplit(normal, exitNormal1, exitNormal2)) {
        return true;
      }
    }
  }
  return false;
}

bool ConeCollection::refineOneByOneDirectionSpannedSlices(
  Cone& toBeSliced, List<Wall>& exitWalls, int directionIndex
) {
  STACK_TRACE("ConeCollection::refineOneByOneDirectionSpannedSlices");
  const Vector<Rational>& direction = this->slicingDirections[directionIndex];
  if (!this->allExitWallsAreVisited(toBeSliced, direction, exitWalls)) {
    this->addNonRefinedCone(toBeSliced);
    return false;
  }
  if (exitWalls.size == 1) {
    // The chamber is already refined;
    this->addRefinedCone(toBeSliced);
    return false;
  }
  Vector<Rational> normal;
  if (
    !this->computeSplittingNormal(
      toBeSliced, exitWalls, directionIndex, normal
    )
  ) {
    this->addRefinedCone(toBeSliced);
    return false;
  }
  if (!this->splitChamber(toBeSliced, normal)) {
    global.fatal << "Splitting is not supposed to fail here. " << global.fatal;
    this->addCone(this->nonRefinedCones, toBeSliced);
    return false;
  }
  this->addHistoryPoint();
  return true;
}

ConeCollection::Statistics::Statistics() {
  this->totalSplitFailuresDueToVisitOrder = 0;
}

std::string ConeCollection::toStringRefineStats() {
  std::stringstream out;
  out
  << "Processing direction "
  << this->statistics.currentDirectionIndex + 1
  << " out of "
  << this->slicingDirections.size;
  out
  << "<br>"
  << "Refined cones: "
  << this->refinedCones.size()
  << ". Non-refined cones: "
  << this->nonRefinedCones.size()
  << ". Irregular cones: "
  << this->conesWithIrregularWalls.size();
  out
  << "<br>Out-of-order split misses: "
  << this->statistics.totalSplitFailuresDueToVisitOrder;
  return out.str();
}

void ConeCollection::reportStats(ProgressReport* report) {
  if (report == nullptr) {
    return;
  }
  if (!report->tickAndWantReport()) {
    return;
  }
  report->report(this->toStringRefineStats());
}

void ConeCollection::refineByOneDirection(
  int directionIndex, ProgressReport* report
) {
  STACK_TRACE("ConeCollection::refineByOneDirection");
  this->statistics.currentDirectionIndex = directionIndex;
  List<Cone> subdivided;
  while (this->nonRefinedCones.size() > 0) {
    for (int i = this->nonRefinedCones.size() - 1; i >= 0; i --) {
      Cone toBeRefined = this->nonRefinedCones.values[i];
      this->nonRefinedCones.removeIndex(i);
      if (!this->refineOneByOneDirection(toBeRefined, directionIndex)) {
        this->statistics.totalSplitFailuresDueToVisitOrder ++;
        this->reportStats(report);
        continue;
      }
      this->reportStats(report);
      this->refineAllConesWithWallsWithMultipleNeighbors(report);
    }
  }
  this->checkConsistencyFullWithoutDebugMessage();
}

bool ConeCollection::containsId(int id) const {
  return this->getConeById(id) != nullptr;
}

Cone& ConeCollection::getConeByIdNonConstNoFail(int id) {
  Cone* result = this->getConeByIdNonConst(id);
  if (result == nullptr) {
    global.fatal
    << "Id "
    << id
    << " not found when it should be. "
    << global.fatal;
  }
  return *result;
}

Cone* ConeCollection::getConeByIdNonConst(int id) {
  if (this->refinedCones.contains(id)) {
    return &this->refinedCones.getValueCreateNoInitialization(id);
  }
  if (this->nonRefinedCones.contains(id)) {
    return &this->nonRefinedCones.getValueCreateNoInitialization(id);
  }
  if (this->conesWithIrregularWalls.contains(id)) {
    return &this->conesWithIrregularWalls.getValueCreateNoInitialization(id);
  }
  return nullptr;
}

const Cone* ConeCollection::getConeById(int id) const {
  if (this->refinedCones.contains(id)) {
    return &this->refinedCones.getValueNoFail(id);
  }
  if (this->nonRefinedCones.contains(id)) {
    return &this->nonRefinedCones.getValueNoFail(id);
  }
  if (this->conesWithIrregularWalls.contains(id)) {
    return &this->conesWithIrregularWalls.getValueNoFail(id);
  }
  return nullptr;
}

void ConeCollection::processNeighborsOfNewlyAddedCone(const Cone& cone) {
  STACK_TRACE("ConeCollection::processNeighborsOfNewlyAddedCone");
  bool hasMultiNeighborWall = false;
  for (const Wall& wall : cone.walls) {
    hasMultiNeighborWall = hasMultiNeighborWall || wall.neighbors.size > 1;
    for (int otherId : wall.neighbors) {
      const Cone* neighbor = this->getConeById(otherId);
      if (!neighbor->hasMultipleNeighborWall()) {
        continue;
      }
      if (this->conesWithIrregularWalls.contains(otherId)) {
        continue;
      }
      this->addCone(this->conesWithIrregularWalls, *neighbor);
      this->nonRefinedCones.removeKey(otherId);
      this->refinedCones.removeKey(otherId);
    }
  }
}

void Cone::computeVerticesOneDimension() {
  STACK_TRACE("Cone::computeVerticesOneDimension");
  bool foundNegative = false;
  bool foundPositive = false;
  for (int i = 0; i < this->walls.size; i ++) {
    if (this->walls[i].normal.isPositiveOrZero()) {
      foundPositive = true;
    }
    if (this->walls[i].normal.isNegativeOrZero()) {
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
}

void Cone::computeVerticesFromNormals() {
  STACK_TRACE("Cone::computeVerticesFromNormals");
  this->vertices.clear();
  for (int i = 0; i < this->walls.size; i ++) {
    Cone::scaleNormalizeByPositive(this->walls[i].normal);
  }
  int dimension = this->walls[0].normal.size;
  if (dimension == 1) {
    this->computeVerticesOneDimension();
    return;
  }
  Selection selection;
  Selection nonPivotPoints;
  selection.initialize(this->walls.size);
  LargeInteger numberOfCycles =
  selection.getNumberOfCombinationsFixedCardinality(dimension - 1);
  Matrix<Rational> matrix;
  Vector<Rational> root;
  matrix.initialize(dimension - 1, dimension);
  for (int i = 0; i < numberOfCycles; i ++) {
    selection.incrementSelectionFixedCardinalityReturnFalseIfPastLast(
      dimension - 1
    );
    for (int j = 0; j < selection.cardinalitySelection; j ++) {
      for (int k = 0; k < dimension; k ++) {
        matrix.elements[j][k] = this->walls[selection.elements[j]].normal[k];
      }
    }
    matrix.gaussianEliminationByRows(nullptr, &nonPivotPoints);
    if (nonPivotPoints.cardinalitySelection == 1) {
      matrix.nonPivotPointsToEigenVectorHomogeneous(nonPivotPoints, root);
      bool belongsToCone = this->isInCone(root);
      if (!belongsToCone) {
        root.negate();
        belongsToCone = this->isInCone(root);
      }
      if (belongsToCone) {
        Cone::scaleNormalizeByPositive(root);
        this->vertices.addOnTopNoRepetition(root);
      }
    }
  }
}

bool Cone::eliminateFakeNormalsUsingVertices(int numberOfAddedFakeWalls) {
  if (this->vertices.size == 0) {
    this->flagIsTheZeroCone = true;
    this->walls.setSize(0);
    return false;
  }
  Vectors<Rational> verticesOnWall;
  if (numberOfAddedFakeWalls != 0) {
    // We modify the normals so that they lie in the subspace spanned by the
    // vertices.
    Matrix<Rational> matrix;
    Matrix<Rational> matrixOfNormals;
    Matrix<Rational> gramMatrixInverted;
    matrix.assignVectorsToRows(this->vertices);
    Vectors<Rational> normalsToSubspace;
    matrix.getZeroEigenSpaceModifyMe(normalsToSubspace);
    if (normalsToSubspace.size > 0) {
      matrixOfNormals.assignVectorsToRows(normalsToSubspace);
      gramMatrixInverted = matrixOfNormals;
      gramMatrixInverted.transpose();
      gramMatrixInverted.multiplyOnTheLeft(matrixOfNormals);
      gramMatrixInverted.invert();
      Vector<Rational> root;
      for (int i = 0; i < this->walls.size; i ++) {
        matrixOfNormals.actOnVectorColumn(this->walls[i].normal, root);
        gramMatrixInverted.actOnVectorColumn(root);
        for (int j = 0; j < root.size; j ++) {
          this->walls[i].normal -= normalsToSubspace[j] * root[j];
        }
        Cone::scaleNormalizeByPositive(this->walls[i].normal);
        if (this->walls[i].normal.isEqualToZero()) {
          this->walls.removeIndexSwapWithLast(i);
          i --;
        }
      }
      // All normals should now lie in the subspace spanned by the vertices
      // add the walls needed to go down to the subspace.
      this->walls.reserve(this->walls.size + 2 * normalsToSubspace.size);
      for (int i = 0; i < normalsToSubspace.size; i ++) {
        Cone::scaleNormalizeByPositive(normalsToSubspace[i]);
        Wall wall;
        wall.normal = normalsToSubspace[i];
        this->walls.addOnTop(wall);
        wall.normal = - normalsToSubspace[i];
        this->walls.addOnTop(wall);
      }
    }
  }
  Matrix<Rational> matrixX;
  Selection currentSelectionX;
  int desiredRank =
  this->vertices.getRankLinearSpan(&matrixX, &currentSelectionX);
  if (desiredRank > 1) {
    for (int i = 0; i < this->walls.size; i ++) {
      Vector<Rational>& currentNormal = this->walls[i].normal;
      verticesOnWall.size = 0;
      bool wallIsGood = false;
      for (int j = 0; j < this->vertices.size; j ++) {
        if (currentNormal.scalarEuclidean(this->vertices[j]).isEqualToZero()) {
          verticesOnWall.addOnTop(this->vertices[j]);
          int rank =
          verticesOnWall.getRankLinearSpan(&matrixX, &currentSelectionX);
          if (rank < verticesOnWall.size) {
            verticesOnWall.removeLastObject();
          } else {
            if (rank == desiredRank - 1) {
              wallIsGood = true;
              break;
            }
          }
        }
      }
      if (!wallIsGood) {
        this->walls.removeIndexSwapWithLast(i);
        i --;
      }
    }
  }
  // Eliminate identical normals.
  this->walls.quickSortAscending();
  int currentUniqueElementIndex = 0;
  for (int i = 1; i < this->walls.size; i ++) {
    if (this->walls[currentUniqueElementIndex] != this->walls[i]) {
      currentUniqueElementIndex ++;
      this->walls.swapTwoIndices(currentUniqueElementIndex, i);
    }
  }
  if (this->walls.size > 0) {
    this->walls.setSize(currentUniqueElementIndex + 1);
  }
  for (int i = 0; i < this->vertices.size; i ++) {
    if (!this->isInCone(this->vertices[i])) {
      global.fatal << "Negative scalar products not allowed. " << global.fatal;
    }
  }
  for (int i = 0; i < this->walls.size; i ++) {
    if (
      !this->vertices.hasAnElementWithPositiveScalarProduct(
        this->walls[i].normal
      )
    ) {
      return false;
    }
  }
  return numberOfAddedFakeWalls == 0;
}

bool Cone::haveCommonVertex(
  const Vector<Rational>& normal1, const Vector<Rational>& normal2
) {
  for (const Vector<Rational>& vertex : this->vertices) {
    if (
      normal1.scalarEuclidean(vertex).isEqualToZero() &&
      normal2.scalarEuclidean(vertex).isEqualToZero()
    ) {
      return true;
    }
  }
  return false;
}

bool Cone::produceNormalSeparatingWallsInDirection(
  const Vector<Rational>& slicingDirection,
  const Vector<Rational>& base,
  const Vector<Rational>& otherWall,
  Vector<Rational>& output
) const {
  // We are looking for a normal n of the form
  // n = t1 * normal1 + t2 * normal2
  // such that
  // <n, slicingDirection> =
  // <t1 * normal1 + t2 * normal2, slicingDirection> = 0
  // t1<normal1, slicingDirection> = -t2<normal2, slicingDirection>
  // This is solved by
  // t1 = -<normal2, slicingDirection>
  // t2 = <normal1, slicingDirection>
  // Suppose
  // normal1   = (1,0)
  // normal2   = (-5,1)
  // direction = (4,1).
  Rational product1 = base.scalarEuclidean(slicingDirection);
  // In the example, product1 = 4.
  Rational product2 = otherWall.scalarEuclidean(slicingDirection);
  // In the example product2 = -19.
  if (!product1.isPositive() || !product2.isPositive()) {
    return false;
  }
  output = otherWall * product1;
  output += base *(- product2);
  // Output:
  //  (-5,1)*4 + (1,0)*(-(-19))
  // =(-20,4) + (19,0)
  // =(-1,4)
  Cone::scaleNormalizeByPositive(output);
  // output = (-1,4)
  return true;
}

void Cone::scaleNormalizeByPositive(Vector<Rational>& toScale) {
  Lattice::scaleNormalizeByPositive(toScale);
}

Vector<Rational> Cone::internalPointNormal() const {
  Vector<Rational> output;
  output.makeZero(this->getDimension());
  if (this->vertices.size == 0) {
    return output;
  }
  for (Vector<Rational> vertex : this->vertices) {
    vertex /= vertex.sumCoordinates();
    output += vertex;
  }
  output /= this->vertices.size;
  return output;
}

Vector<Rational>& Cone::internalPointCached() {
  if (this->payload.internalPoint.size == 0) {
    this->internalPoint(this->payload.internalPoint);
  }
  return this->payload.internalPoint;
}

Vector<Rational> Cone::internalPoint() const {
  Vector<Rational> result;
  this->internalPoint(result);
  return result;
}

bool Cone::internalPoint(Vector<Rational>& output) const {
  if (this->vertices.size <= 0) {
    return false;
  }
  this->vertices.sum(output, this->vertices[0].size);
  return true;
}

bool Cone::createFromVertices(const Vectors<Rational>& inputVertices) {
  this->payload.lowestSlicingIndex = 0;
  this->flagIsTheZeroCone = false;
  if (inputVertices.size <= 0) {
    this->walls.clear();
    this->vertices.size = 0;
    this->flagIsTheZeroCone = true;
    return false;
  }
  this->walls.clear();
  Matrix<Rational> matrix;
  Selection wallGeneratorSelection;
  int rankVerticesSpan =
  inputVertices.getRankLinearSpan(&matrix, &wallGeneratorSelection);
  int dimension = inputVertices.getDimension();
  Vectors<Rational> extraVertices;
  extraVertices.setSize(0);
  if (rankVerticesSpan < dimension) {
    matrix.assignVectorsToRows(inputVertices);
    matrix.getZeroEigenSpace(extraVertices);
    for (int i = 0; i < extraVertices.size; i ++) {
      Wall wall;
      wall.normal = extraVertices[i];
      this->walls.addOnTop(wall);
      wall.normal.negate();
      this->walls.addOnTop(wall);
    }
  }
  LargeInteger numberOfCandidates =
  MathRoutines::nChooseK(inputVertices.size, rankVerticesSpan - 1);
  Selection selection;
  selection.initialize(inputVertices.size);
  Vector<Rational> normalCandidate;
  for (int i = 0; i < numberOfCandidates; i ++) {
    selection.incrementSelectionFixedCardinalityReturnFalseIfPastLast(
      rankVerticesSpan - 1
    );
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
        Wall wall;
        wall.normal = normalCandidate;
        this->walls.addOnTopNoRepetition(wall);
      }
    }
    extraVertices.size = dimension - rankVerticesSpan;
  }
  return this->createFromWalls(this->walls, false);
}

bool Cone::createFromNormals(
  const Vectors<Rational>& inputNormals, const List<List<int> >& inputNeighbors
) {
  return this->createFromNormals(inputNormals, inputNeighbors, false);
}

bool Cone::createFromNormals(
  const Vectors<Rational>& inputNormals,
  const List<List<int> >& inputNeighbors,
  bool hasEnoughProjectiveVertices
) {
  List<Wall> inputWalls;
  for (int i = 0; i < inputNormals.size; i ++) {
    Wall incoming;
    incoming.normal = inputNormals[i];
    incoming.neighbors = inputNeighbors[i];
    inputWalls.addOnTop(incoming);
  }
  return this->createFromWalls(inputWalls, hasEnoughProjectiveVertices);
}

int Cone::getDimension() const {
  if (this->walls.size == 0) {
    return 0;
  }
  return this->walls[0].normal.size;
}

bool Cone::createFromWalls(
  const List<Wall>& inputWalls, bool hasEnoughProjectiveVertices
) {
  STACK_TRACE("Cone::createFromWalls");
  this->flagIsTheZeroCone = false;
  this->payload.lowestSlicingIndex = 0;
  this->walls = inputWalls;
  for (int i = 0; i < this->walls.size; i ++) {
    if (this->walls[i].normal.isEqualToZero()) {
      this->walls.removeIndexSwapWithLast(i);
      i --;
    }
  }
  int fakeWallCount = 0;
  if (!hasEnoughProjectiveVertices) {
    fakeWallCount = this->addFakeWalls();
  }
  this->computeVerticesFromNormals();
  if (!hasEnoughProjectiveVertices) {
    this->removeFakeWalls(fakeWallCount);
  }
  return this->eliminateFakeNormalsUsingVertices(fakeWallCount);
}

int Cone::addFakeWalls() {
  int dimension = this->getDimension();
  int result = 0;
  Matrix<Rational> matrix;
  Selection selection;
  for (
    int i = 0; i < dimension &&
    this->getAllNormals().getRankLinearSpan(&matrix, &selection) < dimension;
    i ++
  ) {
    Wall fakeWall;
    fakeWall.normal.makeEi(dimension, i);
    if (
      !this->getAllNormals().linearSpanContainsVector(
        fakeWall.normal, matrix, selection
      )
    ) {
      result ++;
      this->walls.addOnTop(fakeWall);
    }
  }
  return result;
}

bool Cone::hasMultipleNeighborWall() const {
  for (const Wall& wall : this->walls) {
    if (wall.neighbors.size > 1) {
      return true;
    }
  }
  return false;
}

void Cone::removeNeighbor(int neighborId) {
  for (Wall& wall : this->walls) {
    for (int i = wall.neighbors.size - 1; i >= 0; i --) {
      if (wall.neighbors[i] == neighborId) {
        wall.neighbors.removeIndexShiftDown(i);
      }
    }
  }
}

void Cone::removeFakeWalls(int fakeWallCount) {
  if (fakeWallCount <= 0) {
    return;
  }
  this->walls.setSize(this->walls.size - fakeWallCount);
  Vector<Rational> root;
  int originalSize = this->vertices.size;
  for (int i = 0; i < originalSize; i ++) {
    root = - this->vertices[i];
    if (this->isInCone(root)) {
      this->vertices.addOnTopNoRepetition(root);
    }
  }
}

ConeCollection::ConeCollection() {
  this->flagChambersHaveTooFewVertices = false;
  this->flagIsRefined = false;
  this->conesCreated = 0;
  this->flagRecordSlicingHistory = true;
  this->flagUseSpannedSlices = false;
  this->flagAmalgamateChambers = false;
}

bool ConeCollection::checkConsistencyFullWithoutDebugMessage() const {
  STACK_TRACE("ConeCollection::checkConsistencyFullWithoutDebugMessage");
  this->checkConsistencyOneCollection(this->refinedCones);
  this->checkConsistencyOneCollection(this->nonRefinedCones);
  this->checkConsistencyOneCollection(this->conesWithIrregularWalls);
  return true;
}

bool ConeCollection::checkConsistencyFull() const {
  STACK_TRACE("ConeCollection::checkConsistencyFull");
  // Deliberately issue a noisy comment to alert against
  // accidentally calling this function.
  // If you really want to perform this expensive check (should be done
  // at most a few times per algorithm run), then run
  // checkConsistencyFullWithoutDebugMessage instead.
  global.comments << "<br>Checking full consistency.";
  return this->checkConsistencyFullWithoutDebugMessage();
}

bool ConeCollection::checkConsistencyOneCollection(
  const MapList<int, Cone>& collection
) const {
  STACK_TRACE("ConeCollection::checkConsistencyOneCollection");
  for (const Cone& cone : collection.values) {
    cone.checkConsistencyFull(*this);
  }
  return true;
}

void ConeCollection::initialize() {
  this->conesCreated = 0;
  this->splittingNormals.clear();
  this->slicingDirections.size = 0;
  this->refinedCones.clear();
  this->nonRefinedCones.clear();
  this->convexHull.walls.clear();
  this->convexHull.vertices.size = 0;
  this->convexHull.flagIsTheZeroCone = true;
}

void ConeCollection::initFromConeWalls(
  List<Cone>& normalsOfCones, bool hasEnoughProjectiveVertices
) {
  STACK_TRACE("ConeCollection::initFromConeWalls");
  Cone startingCone;
  this->refinedCones.clear();
  this->nonRefinedCones.clear();
  ProgressReport report;
  report.report(normalsOfCones.toString());
  for (int i = 0; i < normalsOfCones.size; i ++) {
    if (
      startingCone.createFromWalls(
        normalsOfCones[i].walls, hasEnoughProjectiveVertices
      )
    ) {
      this->addNonRefinedCone(startingCone);
    }
    std::stringstream out;
    out << "Initializing cone " << i + 1 << " out of " << normalsOfCones.size;
    report.report(out.str());
  }
  Vector<Rational> root;
  this->splittingNormals.clear();
  for (int i = 0; i < this->nonRefinedCones.size(); i ++) {
    for (int j = 0; j < this->nonRefinedCones.values[i].walls.size; j ++) {
      root = this->nonRefinedCones[i].walls[j].normal;
      root.scaleNormalizeFirstNonZero();
      this->splittingNormals.addOnTopNoRepetition(root);
      std::stringstream out;
      out
      << "Extracting walls from cone "
      << i + 1
      << " out of "
      << this->nonRefinedCones.size()
      << " total distinct chambers.";
      out
      << "\nProcessed "
      << j + 1
      << " out of "
      << this->nonRefinedCones.values[i].walls.size
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
  if (this->nonRefinedCones.size() > 0) {
    global.fatal
    << "Non-refined cones not allowed by this function."
    << global.fatal;
  }
  if (this->conesWithIrregularWalls.size() > 0) {
    global.fatal
    << "Cones with irregular walls not allowed by this function."
    << global.fatal;
  }
  return true;
}

std::string Cone::toLatexVectorWithErrorCheck(
  const Vector<Rational>& input, bool includeErrorChecks
) const {
  std::stringstream out;
  out << input.toString();
  if (
    includeErrorChecks && this->payload.precomputedNonChecked.contains(input)
  ) {
    out
    << ": "
    << this->payload.precomputedNonChecked.getValueNoFail(input).toString();
  }
  if (includeErrorChecks && this->payload.precomputedChecked.contains(input)) {
    out
    << ": "
    << this->payload.precomputedChecked.getValueNoFail(input).toString()
    << "\\checkmark";
  }
  return out.str();
}

std::string Cone::toLatexVertices(bool includeErrorChecks) const {
  std::stringstream out;
  if (this->vertices.size == 0) {
    return " ";
  }
  if (this->vertices.size == 1) {
    return
    this->toLatexVectorWithErrorCheck(this->vertices[0], includeErrorChecks);
  }
  out << "\\begin{array}{l}";
  for (int i = 0; i < this->vertices.size; i ++) {
    out
    << this->toLatexVectorWithErrorCheck(
      this->vertices[i], includeErrorChecks
    );
    if (i != this->vertices.size - 1) {
      out << "\\\\";
    }
  }
  out << "\\end{array}";
  return out.str();
}

std::string Cone::toLatex(const ConeCollection& owner, bool includeErrorChecks)
const {
  STACK_TRACE("Cone::toLatex");
  std::stringstream out;
  bool lastVarIsConstant = false;
  out << this->displayId();
  out << "&";
  if (this->flagIsTheZeroCone) {
    out << "The cone is the zero cone.";
    return out.str();
  }
  if (this->walls.size == 0) {
    out << "The cone is the entire space";
    return out.str();
  }
  out << "\n";
  out << "\\(";
  FormatExpressions format;
  out << this->getAllNormals().toLatexInequalities(lastVarIsConstant, format);
  out << "\\)";
  out << "&";
  out << "\\(" << this->toLatexVertices(includeErrorChecks) << "\\)";
  out << "&";
  if (this->vertices.size > 0) {
    out
    << this->toLatexVectorWithErrorCheck(
      this->internalPoint(), includeErrorChecks
    );
  }
  out << "& ";
  out << this->toStringNeighbors(owner);
  return out.str();
}

std::string Cone::toHTML(const ConeCollection& owner, bool includeErrorChecks)
const {
  STACK_TRACE("Cone::toHTML");
  std::stringstream out;
  bool lastVarIsConstant = false;
  out << "Id: " << this->displayId();
  if (this->flagIsTheZeroCone) {
    out << "The cone is the zero cone.";
  } else if (this->walls.size == 0) {
    out << "The cone is the entire space";
  }
  out << "<br>";
  out << "\\(";
  FormatExpressions format;
  out << this->getAllNormals().toLatexInequalities(lastVarIsConstant, format);
  out << "\\)";
  out << "<br>Projectivized vertices: " << this->vertices.toString();
  if (this->vertices.size > 0) {
    out << "<br>Internal point: " << this->internalPoint().toString();
  }
  out << "<br>Neighbors: ";
  out << this->toStringNeighborsAlongWall(owner);
  if (includeErrorChecks) {
    out << this->toStringPrecomputedVectorPartitionFunctionValues();
  }
  return out.str();
}

std::string Cone::toStringPrecomputedVectorPartitionFunctionValues() const {
  std::stringstream out;
  out << "<br>Precomputed values, checked by enumeration: ";
  for (const Vector<Rational>& input : this->payload.precomputedChecked.keys) {
    out
    << "<br>"
    << input
    << ": "
    << this->payload.precomputedChecked.getValueNoFail(input);
  }
  out << "<br>Precomputed values, too large to check: ";
  for (
    const Vector<Rational>& input : this->payload.precomputedNonChecked.keys
  ) {
    out
    << "<br>"
    << input
    << ": "
    << this->payload.precomputedNonChecked.getValueNoFail(input);
  }
  return out.str();
}

std::string Cone::toStringNeighborsAlongWall(const ConeCollection& owner) const {
  std::stringstream out;
  for (int i = 0; i < this->walls.size; i ++) {
    List<std::string> neighborIds;
    for (int j : this->walls[i].neighbors) {
      const Cone* neighbor = owner.getConeById(j);
      if (neighbor == nullptr) {
        std::stringstream neigborReport;
        neigborReport << "[missing id: " << j << "]";
        neighborIds.addOnTop(neigborReport.str());
        continue;
      }
      neighborIds.addOnTop(neighbor->displayId());
    }
    out
    << "<br>Along wall "
    << i + 1
    << ": "
    << StringRoutines::join(neighborIds, ",");
    if (neighborIds.size == 0) {
      out << "N/A";
    }
  }
  return out.str();
}

void Cone::getAllNeighbors(HashedList<int>& output) const {
  output.clear();
  for (const Wall& wall : this->walls) {
    for (int neighbor : wall.neighbors) {
      output.addOnTopNoRepetition(neighbor);
    }
  }
}

std::string Cone::toStringNeighbors(const ConeCollection& owner) const {
  HashedList<int> neighbors;
  this->getAllNeighbors(neighbors);
  List<std::string> neighborDisplayIds;
  for (int id : neighbors) {
    const Cone* neighbor = owner.getConeById(id);
    if (neighbor == nullptr) {
      neighborDisplayIds.addOnTop("(corrupt)");
      continue;
    }
    neighborDisplayIds.addListOnTop(neighbor->displayId());
  }
  std::stringstream out;
  out << neighbors;
  return out.str();
}

std::string Cone::toString(FormatExpressions* format) const {
  std::stringstream out;
  bool prepareMathReport = format == nullptr ? false : format->flagUseLatex;
  bool useHtml = format == nullptr ? false : format->flagUseHTML;
  bool useLatex = format == nullptr ? false : format->flagUseLatex;
  bool lastVarIsConstant = false;
  if (this->flagIsTheZeroCone) {
    out << "The cone is the zero cone.\n";
  } else if (this->walls.size == 0) {
    out << "The cone is the entire space.\n";
  }
  out << "id: " << this->id << ".\n";
  if (!prepareMathReport) {
    out
    << "Index next wall to refine by: "
    << this->payload.lowestSlicingIndex
    << "\n";
    if (useHtml) {
      out << "<br>";
    }
    out
    << "\nIndex next direction to slice in: "
    << this->payload.lowestSlicingIndex
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
  FormatExpressions formatContainer;
  if (format == nullptr) {
    format = &formatContainer;
  }
  out << this->getAllNormals().toLatexInequalities(lastVarIsConstant, *format);
  if (useLatex) {
    out << "\\]";
  }
  out << "\nProjectivized Vertices: " << this->vertices.toString();
  if (useHtml) {
    out << "<br>";
  }
  if (this->vertices.size > 0) {
    out << "\nInternal point: " << this->internalPoint().toString();
  }
  return out.str();
}

int Cone::upperBoundForEnumeration(int dimension) {
  switch (dimension) {
  case 1:
    return 100;
  case 2:
    return 500;
    break;
  case 3:
    return 100;
  case 4:
    return 30;
  default:
    return 10;
  }
}

void Cone::precomputeVectorPartitionFunction(
  const Vector<Rational>& inputMustBeInCone,
  const List<Vector<Rational> >& originalVectors
) {
  STACK_TRACE("Cone::precomputeVectorPartitionFunction");
  Rational result =
  this->payload.getPolynomial().evaluate(inputMustBeInCone, nullptr);
  if (!result.isInteger()) {
    global.fatal
    << "Non-integer vector partition function value. "
    << global.fatal;
  }
  int upperBound = this->upperBoundForEnumeration(inputMustBeInCone.size);
  if (result > upperBound) {
    this->payload.precomputedNonChecked.setKeyValue(inputMustBeInCone, result);
    return;
  }
  this->payload.precomputedChecked.setKeyValue(inputMustBeInCone, result);
  VectorPartition partition;
  partition.initialize(originalVectors, inputMustBeInCone);
  Rational resultByEnumeration = partition.numberOfPartitionsByEnumeration();
  if (result != resultByEnumeration) {
    global.fatal
    << "Vector partition function computation is incorrect. Input: "
    << inputMustBeInCone.toString()
    << ". Cone id: "
    << this->id
    << ". Quasipolynomial: \\("
    << this->payload.getPolynomial().toString()
    << "\\)"
    << global.fatal;
  }
}

std::string ConeCollection::toStringNeighborGraph(
  const MapList<int, Cone>& map
) const {
  std::stringstream out;
  for (const Cone& cone : map.values) {
    out
    << "<br>id "
    << cone.displayId()
    << ": "
    << cone.toStringNeighbors(*this);
  }
  return out.str();
}

std::string ConeCollection::toStringNeighborGraph() const {
  std::stringstream out;
  out << "Refined:";
  out << this->toStringNeighborGraph(this->refinedCones);
  out << "<br>Non-refined:";
  out << this->toStringNeighborGraph(this->nonRefinedCones);
  out << "<br>Irregular:";
  out << this->toStringNeighborGraph(this->conesWithIrregularWalls);
  return out.str();
}

std::string ConeCollection::toStringConeIds() const {
  std::stringstream out;
  out << "Refined cones: " << this->refinedCones.keys << ". ";
  out << "Non-refined cones: " << this->nonRefinedCones.keys << ". ";
  out
  << "Non-refined irregular-wall cones: "
  << this->conesWithIrregularWalls.keys
  << ". ";
  return out.str();
}

std::string ConeCollection::toStringConeCount() const {
  std::stringstream out;
  out << "Refined chambers: " << this->refinedCones.size() << ".";
  out
  << "\nNon-refined regular chambers: "
  << this->nonRefinedCones.size()
  << ".";
  out
  << "\nNon-refined irregular chambers: "
  << this->refinedCones.size()
  << ".\n";
  return out.str();
}

std::string ConeCollection::toString() const {
  std::stringstream out;
  FormatExpressions format;
  out << this->toStringConeCount();
  Vectors<Rational> roots;
  roots = this->splittingNormals;
  out << "\nNormals of walls to refine by: ";
  out << roots.toString(&format) << ".\n";
  if (this->slicingDirections.size > 0) {
    out
    << "Directions to slice along: "
    << this->slicingDirections.toString()
    << ".\n";
  }
  out << "Cones with irregular neighbors follow.\n";
  for (int i = 0; i < this->conesWithIrregularWalls.size(); i ++) {
    Cone& cone = this->conesWithIrregularWalls.values[i];
    out << "\nChamber " << i + 1 << ":\n";
    out << cone.toString(&format) << "\n";
  }
  out << "Non-refined cones follow.\n";
  for (int i = 0; i < this->nonRefinedCones.size(); i ++) {
    Cone& cone = this->nonRefinedCones.values[i];
    out << "\nChamber " << i + 1 << ":\n";
    out << cone.toString(&format) << "\n";
  }
  out << "Refined cones follow.\n";
  for (int i = 0; i < this->refinedCones.size(); i ++) {
    Cone& cone = this->refinedCones.values[i];
    out << "\nChamber " << i + 1 << ":\n";
    out << cone.toString(&format) << "\n";
  }
  return out.str();
}

std::string ConeCollection::toHTMLHistory() const {
  List<std::string> historyInReverse;
  historyInReverse = this->historyHTML;
  historyInReverse.reverseElements();
  return StringRoutines::join(historyInReverse);
}

std::string ConeCollection::toLatexWithoutGraphics(const std::string& label)
const {
  std::stringstream out;
  out << "\\begin{longtable}{|ccccc|}";
  out << "\\caption{\\footnotesize V.p.f. of ";
  out << label << "}\\\\";
  out
  << "\\hline N & "
  << "Defining inequalities & "
  << "Vertices& Int. Pt.& "
  << "Neighbors \\\\ \\hline\n"
  << "\\endfirsthead"
  << "\\multicolumn{5}{c}{{"
  << "\\bfseries \\tablename\\ \\thetable{} -- continued from previous page"
  << "}} \\\\\n"
  << "\\hline N & "
  << "Defining inequalities & "
  << "Vertices&Int. Pt. & "
  << "Neighbors\\\\ \\hline\n"
  << "\\endhead"
  << "\\hline \\multicolumn{5}{|c|}{{Continued on next page}} \\\\ \\hline"
  << "\\endfoot"
  << "\\endlastfoot";
  for (int i = 0; i < this->refinedCones.size(); i ++) {
    Cone& cone = this->refinedCones.values[i];
    out << cone.toLatex(*this, true);
    out << "\\\\\\hline\n";
  }
  out << "\\end{longtable}";
  return out.str();
}

std::string ConeCollection::toLatexGraphicsOnlyPsTricks(
  bool includeVertices, bool includeLattice
) const {
  STACK_TRACE("ConeCollection::toLatexGraphicsOnlyPsTricks");
  std::stringstream out;
  DrawingVariables drawingVariables;
  if (
    !this->drawMeProjective(drawingVariables, includeVertices, includeLattice)
  ) {
    return "";
  }
  out << "{\\tiny\n";
  out << drawingVariables.toLatexPsTricks();
  out << "}\n";
  return out.str();
}

std::string ConeCollection::toHTMLGraphicsOnly(
  bool includePanels, bool includeLattice
) const {
  DrawingVariables drawingVariables;
  FormatExpressions format;
  return
  this->drawMeToHtmlProjective(
    drawingVariables, true, includeLattice, includePanels
  );
}

std::string ConeCollection::toHTML() const {
  STACK_TRACE("ConeCollection::toHTML");
  std::stringstream out;
  out << this->toHTMLGraphicsOnly(true, false);
  out << this->toHTMLWithoutGraphics();
  return out.str();
}

std::string ConeCollection::toHTMLOneCollection(
  const MapList<int, Cone>& cones, int& totalChambers
) const {
  std::stringstream out;
  for (int i = 0; i < cones.size(); i ++) {
    totalChambers ++;
    if (totalChambers > this->maximumCones) {
      out << "<br>Printout too large: omitting the rest of the cones.";
      break;
    }
    Cone& cone = cones.values[i];
    out << "<br>";
    out << cone.toHTML(*this, false) << "\n";
  }
  return out.str();
}

std::string ConeCollection::toHTMLWithoutGraphics() const {
  std::stringstream out;
  FormatExpressions format;
  Vectors<Rational> roots;
  roots = this->splittingNormals;
  out << "<br>Normals of walls to refine by: ";
  out << roots.toString(&format);
  if (this->slicingDirections.size > 0) {
    out
    << " Directions to slice along: "
    << this->slicingDirections.toString();
  }
  int totalChambers = 0;
  out << "Total refined chambers: " << this->refinedCones.size();
  out << this->toHTMLOneCollection(this->refinedCones, totalChambers);
  out << "<hr>Total non-refined chambers: " << this->nonRefinedCones.size();
  out << this->toHTMLOneCollection(this->nonRefinedCones, totalChambers);
  out
  << "<hr>Total non-refined chambers with irregular walls: "
  << this->conesWithIrregularWalls.size();
  out
  << this->toHTMLOneCollection(this->conesWithIrregularWalls, totalChambers);
  return out.str();
}

int ConeCollection::largestConeId() {
  int result = - 1;
  for (int i : this->refinedCones.keys) {
    result = MathRoutines::maximum(i, result);
  }
  for (int i : this->nonRefinedCones.keys) {
    result = MathRoutines::maximum(i, result);
  }
  for (int i : this->conesWithIrregularWalls.keys) {
    result = MathRoutines::maximum(i, result);
  }
  return result;
}

bool ConeCollection::findMaxLFOverConeProjective(
  const Cone& input,
  List<Polynomial<Rational> >& inputLinearPolynomials,
  List<int>& outputMaximumOverEeachSubChamber
) {
  Vectors<Rational> hyperPlanesCorrespondingToLF;
  if (input.walls.size < 1 || inputLinearPolynomials.size < 1) {
    return false;
  }
  int dimension = input.walls[0].normal.size;
  hyperPlanesCorrespondingToLF.setSize(inputLinearPolynomials.size);
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
    Vector<Rational>& newWall = hyperPlanesCorrespondingToLF[i];
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
    input, hyperPlanesCorrespondingToLF, outputMaximumOverEeachSubChamber
  );
}

void ConeCollection::addCone(MapList<int, Cone>& map, const Cone& cone) {
  if (cone.id == - 1) {
    global.fatal
    << "Adding a non-initialized cone not allowed.\nCone:\n"
    << cone.toString()
    << global.fatal;
  }
  if (map.contains(cone.id)) {
    global.fatal << "Not allowed to add a cone twice. " << global.fatal;
  }
  map.setKeyValue(cone.id, cone);
}

void ConeCollection::addRefinedCone(Cone& cone) {
  STACK_TRACE("ConeCollection::addRefinedCone");
  cone.payload.visited = true;
  this->addCone(this->refinedCones, cone);
}

void ConeCollection::addNonRefinedCone(const Cone& cone) {
  STACK_TRACE("ConeCollection::addNonRefinedCone");
  if (cone.hasMultipleNeighborWall()) {
    this->addCone(this->conesWithIrregularWalls, cone);
  } else {
    this->addCone(this->nonRefinedCones, cone);
  }
}

bool ConeCollection::findMaxLFOverConeProjective(
  const Cone& input,
  Vectors<Rational>& inputLFsLastCoordConst,
  List<int>& outputMaximumOverEeachSubChamber
) {
  this->initialize();
  this->addNonRefinedCone(input);
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
  this->refineByNormals();
  outputMaximumOverEeachSubChamber.setSize(this->refinedCones.size());
  Rational maximumScalarProduct = 0;
  for (int i = 0; i < this->refinedCones.size(); i ++) {
    this->refinedCones.values[i].internalPoint(root);
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

bool VariableLetter::operator==(const VariableLetter& other) const {
  return
  this->latexLetter == other.latexLetter &&
  this->mathMLLetter == other.mathMLLetter;
}

void VariableLetter::operator=(const std::string& other) {
  this->latexLetter = other;
  this->mathMLLetter = other;
}
