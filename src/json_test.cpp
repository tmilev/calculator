#include "json.h"
#include "math_general.h"

bool JSData::Test::all() {
  MacroRegisterFunctionWithName("JSData::Test::all");
  JSData::Test::testRecode();
  JSData::Test::testBadInput();
  return true;
}


bool JSData::Test::testRecode() {
  MacroRegisterFunctionWithName("JSData::Test::testRecode");
  // first element is string input, second element
  // is expected recoded output.
  List<List<std::string> > toRecode;
  toRecode.addOnTop(List<std::string>({
    "{}", "{}"
  }));
  toRecode.addOnTop(List<std::string>({
    "null", "null"
  }));
  toRecode.addOnTop(List<std::string>({
    "5", "5"
  }));
  toRecode.addOnTop(List<std::string>({
    "- 5", "-5"
  }));
  toRecode.addOnTop(List<std::string>({
    "[1, \"\"]", "[1,\"\"]"
  }));
  toRecode.addOnTop(List<std::string>({
    "{\"a\":\"b\", \"c\": [\"d\",5]}", "{\"a\":\"b\",\"c\":[\"d\",5]}"
  }));
  toRecode.addOnTop(List<std::string>({
    "{\"a\":\"\\n\"}", "{\"a\":\"\\n\"}"
  }));
  toRecode.addOnTop(List<std::string>({
    "{\"a\":\"\n\"}", "{\"a\":\"\\n\"}"
  }));
  toRecode.addOnTop(List<std::string>({
    "\"\u03C0\"", "\"\u03C0\""
  }));
  std::stringstream commentsOnFailure;
  for (int i = 0; i < toRecode.size; i ++) {
    JSData parser;
    std::string input = toRecode[i][0];
    if (!parser.readstring(input, &commentsOnFailure)) {
      global.fatal << "Failed to decode "<< toRecode[i][0] << global.fatal;
    }
    std::string expectedOutput = toRecode[i][1];
    std::string recoded = parser.toString(nullptr);
    if (recoded != expectedOutput) {
      global.fatal << "Input " << input << " decoded to " << recoded
      << ". However, I expected: " << expectedOutput << ". " << global.fatal;
    }

  }
  return true;
}

bool JSData::Test::testBadInput() {
  MacroRegisterFunctionWithName("JSData::Test::testBadInput");
  // first element is string input, second element
  // is expected recoded output.
  List<std::string> broken = List<std::string>({
      "{", "ab", "]", ""
  });

  std::stringstream commentsOnFailure;
  for (int i = 0; i < broken.size; i ++) {
    JSData parser;
    std::string input = broken[i];
    if (parser.readstring(input, &commentsOnFailure)) {
      global.fatal << "Successfully decoded invalid json: " << input << "." << global.fatal;
    }
  }
  return true;
}
