#include "json.h"
#include "math_general.h"

bool JSData::Test::All() {
  MacroRegisterFunctionWithName("JSData::Test::All");
  JSData::Test::TestRecode();
  JSData::Test::TestBadInput();
  return true;
}


bool JSData::Test::TestRecode() {
  MacroRegisterFunctionWithName("JSData::Test::TestRecode");
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

bool JSData::Test::TestBadInput() {
  MacroRegisterFunctionWithName("JSData::Test::TestBadInput");
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
