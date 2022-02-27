// The current file is licensed under the license terms found in the main header
// file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "general_strings.h"
#include "general_logging_global_variables.h"
#include "crypto.h"
#include "macros.h"

bool StringRoutines::Test::all() {
  StringRoutines::Conversions::Test::all();
  return true;
}

bool StringRoutines::Conversions::Test::all() {
  StringRoutines::Conversions::Test::convertStringToJSONStringEscapeOnly();
  StringRoutines::Conversions::Test::convertUtf8StringToUnicodeCodePoints();
  StringRoutines::Conversions::Test::utf8StringToJSONStringEscaped();
  StringRoutines::Conversions::Test::unescapeJavascriptLike();
  StringRoutines::Conversions::Test::codePointToUtf8();
  return true;
}

bool StringRoutines::Conversions::Test::convertStringToJSONStringEscapeOnly() {
  MacroRegisterFunctionWithName(
    "StringRoutines::Conversions::Test::convertStringToJSONStringEscapeOnly"
  );
  List<List<std::string> > conversionPairs;
  conversionPairs.addOnTop(List<std::string>({"\\\"", "\\\\\\\""}));
  for (int i = 0; i < conversionPairs.size; i ++) {
    List<std::string> pair = conversionPairs[i];
    std::string result =
    StringRoutines::Conversions::escapeQuotesBackslashesNewLines(pair[0]);
    if (result != pair[1]) {
      global.fatal
      << "Escaping string "
      << pair[0]
      << " produced: "
      << result
      << ", expected: "
      << pair[1]
      << global.fatal;
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
  MacroRegisterFunctionWithName(
    "StringRoutines::Conversions::Test::oneConversionUtf8Success"
  );
  List<uint32_t> expected, computed;
  expected.addOnTop(codePoint1);
  if (codePoint2 != 0xffffffff) {
    expected.addOnTop(codePoint2);
  }
  if (codePoint3 != 0xffffffff) {
    expected.addOnTop(codePoint3);
  }
  std::stringstream commentsOnFailure;
  bool result =
  StringRoutines::Conversions::utf8StringToUnicodeCodePoints(
    givenInput, computed, &commentsOnFailure
  );
  if (!result) {
    global.fatal
    << "Failed to convert "
    << givenInput
    << " with hex: "
    << Crypto::convertStringToHex(givenInput, 0, false)
    << " to unicode code points. "
    << "Got code points: "
    << computed.toStringCommaDelimited()
    << ". Comments: "
    << commentsOnFailure.str()
    << global.fatal;
  }
  if (expected != computed) {
    global.fatal
    << "Bad conversion of "
    << givenInput
    << " to unicode code points. Computed: "
    << computed.toStringCommaDelimited()
    << ", expected: "
    << expected.toStringCommaDelimited()
    << global.fatal;
  }
  return true;
}

bool StringRoutines::Conversions::Test::convertUtf8StringToUnicodeCodePoints()
{
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
  std::string result =
  StringRoutines::Conversions::stringToJSONStringEscaped(givenInput);
  if (result != expected) {
    global.fatal
    << "Converted input: "
    << givenInput
    << " to: "
    << result
    << " but the expected conversion was: "
    << expected
    << "."
    << global.fatal;
  }
  return true;
}

bool StringRoutines::Conversions::Test::oneCodePointToUtf8(
  uint32_t codePoint, const std::string& expectedHex
) {
  MacroRegisterFunctionWithName(
    "StringRoutines::Conversions::Test::oneCodePointToUtf8"
  );
  std::string converted =
  StringRoutines::Conversions::codePointToUtf8(codePoint);
  std::string convertedHex = Crypto::convertStringToHex(converted, 0, false);
  if (convertedHex != expectedHex) {
    global.fatal
    << "Code point: "
    << codePoint
    << " converted to hex: "
    << convertedHex
    << ", expected: "
    << expectedHex
    << global.fatal;
  }
  return true;
}

bool StringRoutines::Conversions::Test::codePointToUtf8() {
  MacroRegisterFunctionWithName(
    "StringRoutines::Conversions::Test::codePointToUtf8"
  );
  StringRoutines::Conversions::Test::oneCodePointToUtf8(67, "43");
  StringRoutines::Conversions::Test::oneCodePointToUtf8(960, "cf80");
  // \pi.
  StringRoutines::Conversions::Test::oneCodePointToUtf8(35000, "e8a2b8");
  //
  StringRoutines::Conversions::Test::oneCodePointToUtf8(68000, "f090a6a0");
  // Meroitic Cursive Letter A
  return true;
}

bool StringRoutines::Conversions::Test::unescapeJavascriptLike() {
  std::string input = "\u00a2$\u00a2@\u03c0";
  std::string unescaped =
  StringRoutines::Conversions::unescapeJavascriptLike(input);
  std::string hexUnescaped = Crypto::convertStringToHex(unescaped, 0, false);
  std::string expected = "c2a224c2a240cf80";
  if (hexUnescaped != expected) {
    global.fatal
    << "Input: "
    << input
    << " was javascript-like unescaped to hex: "
    << hexUnescaped
    << " but I expected: "
    << expected
    << "."
    << global.fatal;
  }
  return true;
}

bool StringRoutines::Conversions::Test::utf8StringToJSONStringEscaped() {
  StringRoutines::Conversions::Test::oneUtf8ToJSONSuccess(
    "\u00a2$\u00a2@\u03c0", "\\u00a2$\\u00a2@\\u03c0"
  );
  return true;
}

