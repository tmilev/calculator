#include "general_file_operations_encodings.h"
#include "general_logging_global_variables.h"
#include "json.h"

bool JSData::Test::all() {
  STACK_TRACE("JSData::Test::all");
  JSData::Test::recode();
  JSData::Test::recodeRelaxed();
  JSData::Test::badInput();
  JSData::Test::decodeEscapedUnicode();
  JSData::Test::loadLarger();
  return true;
}

bool JSData::Test::recode() {
  STACK_TRACE("JSData::Test::recode");
  // first element is string input, second element
  // is expected recoded output.
  List<List<std::string> > toRecode;
  toRecode.addOnTop(List<std::string>({"{}", "{}"}));
  toRecode.addOnTop(List<std::string>({"null", "null"}));
  toRecode.addOnTop(List<std::string>({"5", "5"}));
  toRecode.addOnTop(List<std::string>({"- 5", "-5"}));
  toRecode.addOnTop(List<std::string>({"[1, \"\"]", "[1,\"\"]"}));
  toRecode.addOnTop(
    List<std::string>({
        "{\"a\":\"b\", \"c\": [\"d\",5]}",
        "{\"a\":\"b\",\"c\":[\"d\",5]}"
      }
    )
  );
  toRecode.addOnTop(
    List<std::string>({"{\"a\":\"\\n\"}", "{\"a\":\"\\n\"}"})
  );
  toRecode.addOnTop(
    List<std::string>({"{\"a\":\"\n\"}", "{\"a\":\"\\n\"}"})
  );
  toRecode.addOnTop(
    List<std::string>({"\"\u03C0\"", "\"\\u03c0\""})
  );
  toRecode.addOnTop(
    List<std::string>({"{\"\n\r\":\"\n\r\"}", "{\"\\n\\r\":\"\\n\\r\"}"})
  );
  toRecode.addOnTop(
    List<std::string>({
        "{\"resultHtml\":\"&lt;!-- --&gt;\\na\"}",
        "{\"resultHtml\":\"&lt;!-- --&gt;\\na\"}"
      }
    )
  );
  for (int i = 0; i < toRecode.size; i ++) {
    JSData::Test::recodeOnce(toRecode[i], false);
  }
  return true;
}

bool JSData::Test::recodeOnce(
  const List<std::string>& pair, bool relaxedInput
) {
  STACK_TRACE("JSData::Test::recodeOnce");
  JSData parser;
  std::string input = pair[0];
  std::stringstream commentsOnFailure;
  if (!parser.parse(input, relaxedInput, &commentsOnFailure, false)) {
    global.fatal
    << "Failed to decode:\n"
    << pair[0]
    << "\nComments: "
    << commentsOnFailure.str()
    << global.fatal;
  }
  std::string expectedOutput = pair[1];
  std::string recoded = parser.toString(nullptr);
  if (recoded != expectedOutput) {
    global.fatal
    << "Input "
    << input
    << " decoded-recoded to:\n"
    << recoded
    << "\nHowever, I expected:\n"
    << expectedOutput
    << ". "
    << global.fatal;
  }
  std::string recodedSecondTime = parser.toString(nullptr);
  if (recoded != recodedSecondTime) {
    global.fatal
    << "Input "
    << input
    << " recoded to "
    << recoded
    << ", but then, the second encoding gave: "
    << recodedSecondTime
    << ". "
    << global.fatal;
  }
  if (recodedSecondTime.find('\n') != std::string::npos) {
    global.fatal
    << "Input "
    << input
    << " recoded to "
    << recoded
    << ", but that had new lines in it."
    << global.fatal;
  }
  return true;
}

bool JSData::Test::recodeRelaxed() {
  STACK_TRACE("JSData::Test::recodeRelaxed");
  // first element is string input, second element
  // is expected recoded output.
  List<List<std::string> > toRecode;
  toRecode.addOnTop(List<std::string>({"{a: 1}", "{\"a\":1}"}));
  toRecode.addOnTop(
    List<std::string>({
        "[true, false, null, \"\", {\"a\":\"b\"}, "
        "4.0, 4.1,-4.0,-4.01, 1234, -123, - 123]",
        "[true,false,null,\"\",{\"a\":\"b\"},4,4.1,-4,-4.01,1234,-123,-123]"
      }
    )
  );
  toRecode.addOnTop(
    List<std::string>({"{a: 1, b:\"c\"}", "{\"a\":1,\"b\":\"c\"}"})
  );
  for (int i = 0; i < toRecode.size; i ++) {
    JSData::Test::recodeOnce(toRecode[i], true);
  }
  return true;
}

bool JSData::Test::decodeEscapedUnicode() {
  JSData parser;
  std::string input = "\"\\u03C0\"";
  std::string expectedOutput = "\u03C0";
  std::stringstream commentsOnFailure;
  if (!parser.parse(input, false, &commentsOnFailure)) {
    global.fatal
    << "In decodeEscapedUnicode: failed to decode "
    << input
    << ". The comments: "
    << commentsOnFailure.str()
    << global.fatal;
  }
  if (parser.stringValue != expectedOutput) {
    global.fatal
    << "In decodeEscapedUnicode: input "
    << input
    << " decoded to "
    << parser.stringValue
    << ". However, I expected: "
    << expectedOutput
    << ". "
    << global.fatal;
  }
  return true;
}

bool JSData::Test::badInput() {
  STACK_TRACE("JSData::Test::badInput");
  // first element is string input, second element
  // is expected recoded output.
  List<std::string> broken = List<std::string>({"{", "ab", "]", ""});
  std::stringstream commentsOnFailure;
  for (int i = 0; i < broken.size; i ++) {
    JSData parser;
    std::string input = broken[i];
    if (parser.parse(input, false, &commentsOnFailure)) {
      global.fatal
      << "Successfully decoded invalid json: "
      << input
      << "."
      << global.fatal;
    }
  }
  return true;
}

bool JSData::Test::loadLarger() {
  STACK_TRACE("JSData::Test::loadLarger");
  std::string input;
  FileOperations::loadFileToStringVirtual(
    "test/larger.json", input, false, nullptr
  );
  int64_t start = global.getElapsedMilliseconds();
  JSData result;
  bool mustParse = result.parse(input, false, nullptr, false);
  if (!mustParse) {
    global.fatal << "Failed to parse test/larger.json" << global.fatal;
  }
  int64_t loadDuration = global.getElapsedMilliseconds() - start;
  global
  << "Time to parse "
  << input.size()
  << " bytes to json: "
  << loadDuration
  << "ms."
  << Logger::endL;
  start = global.getElapsedMilliseconds();
  std::string json = result.toString();
  if (json != input) {
    global.fatal
    << "Recoded string does not coincide with input string. "
    << global.fatal;
  }
  int64_t stringDuration = global.getElapsedMilliseconds() - start;
  global
  << "Time to encode json of size "
  << json.size()
  << " bytes to json: "
  << stringDuration
  << "ms."
  << Logger::endL;
  return true;
}
