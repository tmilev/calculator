// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "general_strings.h"
#include "general_logging_global_variables.h"
#include "crypto.h"

bool StringRoutines::Test::all() {
  StringRoutines::Conversions::Test::all();
  return true;
}

bool StringRoutines::Conversions::Test::all() {
  StringRoutines::Conversions::Test::convertStringToJSONStringEscapeOnly();
  StringRoutines::Conversions::Test::convertUtf8StringToUnicodeCodePoints();
  StringRoutines::Conversions::Test::utf8StringToJSONStringEscaped();
  return true;
}

bool StringRoutines::Conversions::Test::convertStringToJSONStringEscapeOnly() {
  List<List<std::string> > conversionPairs;
  conversionPairs.addOnTop(List<std::string>({ "\\\"", "\\\\\\\""}));
  for (int i = 0; i < conversionPairs.size; i ++) {
    List<std::string> pair = conversionPairs[i];
    std::string result = StringRoutines::Conversions::escapeQuotesBackslashesNewLines(pair[0]);
    if (result != pair[1]) {
      global.fatal << "Escaping string " << pair[0] << " produced: "
      << result << ", expected: " << pair[1] << global.fatal;
    }
  }
  return true;
}

bool StringRoutines::Conversions::Test::oneConversionUtf8Success(
  const std::string& givenInput,
  uint32_t codePoint1,
  uint32_t codePoint2,
  uint32_t codePoint3
) {
  List<uint32_t> expected, computed;
  expected.addOnTop(codePoint1);
  if (codePoint2 != 0xffffffff) {
    expected.addOnTop(codePoint2);
  }
  if (codePoint3 != 0xffffffff) {
    expected.addOnTop(codePoint3);
  }
  std::stringstream commentsOnFailure;
  bool result = StringRoutines::Conversions::utf8StringToUnicodeCodePoints(
    givenInput, computed, &commentsOnFailure
  );
  if (!result) {
    global.fatal << "Failed to convert " << givenInput
    << " with hex: " << Crypto::convertStringToHex(givenInput, 0, false)
    << " to unicode code points. "
    << "Got code points: " << computed.toStringCommaDelimited() << ". Comments: "
    << commentsOnFailure.str()
    << global.fatal;
  }
  if (expected != computed) {
    global.fatal << "Bad conversion of " << givenInput
    << " to unicode code points. Computed: "
    << computed.toStringCommaDelimited() << ", expected: "
    << expected.toStringCommaDelimited()
    << global.fatal;
  }
  return true;
}

bool StringRoutines::Conversions::Test::convertUtf8StringToUnicodeCodePoints() {
  StringRoutines::Conversions::Test::oneConversionUtf8Success("$", 36);
  StringRoutines::Conversions::Test::oneConversionUtf8Success("\u00A2", 162);
  StringRoutines::Conversions::Test::oneConversionUtf8Success("\u0939", 2361);
  StringRoutines::Conversions::Test::oneConversionUtf8Success("\u20AC", 8364);
  StringRoutines::Conversions::Test::oneConversionUtf8Success("\uD55C", 54620);
  StringRoutines::Conversions::Test::oneConversionUtf8Success("\u03C0", 960);
  return true;
}

bool StringRoutines::Conversions::Test::oneUtf8ToJSONSuccess(
  const std::string& givenInput, const std::string& expected
) {
  std::string result = StringRoutines::Conversions::stringToJSONStringEscaped(givenInput);
  if (result != expected) {
    global.fatal << "Converted input: " << givenInput << " to: "
    << result << " but the expected conversion was: "
    << expected << "." << global.fatal;
  }
  return true;
}

bool StringRoutines::Conversions::Test::utf8StringToJSONStringEscaped() {
  StringRoutines::Conversions::Test::oneUtf8ToJSONSuccess("\u00a2$\u00a2@\u03c0", "\\u00a2$\\u00a2@\\u03c0");
  return true;
}
