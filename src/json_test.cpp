#include "json.h"
#include "math_general.h"


static ProjectInformationInstance ProjectInfoJSONTestCPP(__FILE__, "Unit tests, JSON.");

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
  toRecode.AddOnTop(List<std::string>({
    "{}", "{}"
  }));
  toRecode.AddOnTop(List<std::string>({
    "null", "null"
  }));
  toRecode.AddOnTop(List<std::string>({
    "5", "5"
  }));
  toRecode.AddOnTop(List<std::string>({
    "- 5", "-5"
  }));
  toRecode.AddOnTop(List<std::string>({
    "[1, \"\"]", "[1,\"\"]"
  }));
  toRecode.AddOnTop(List<std::string>({
    "{\"a\":\"b\", \"c\": [\"d\",5]}", "{\"a\":\"b\",\"c\":[\"d\",5]}"
  }));
  toRecode.AddOnTop(List<std::string>({
    "{\"a\":\"\\n\"}", "{\"a\":\"\\n\"}"
  }));
  toRecode.AddOnTop(List<std::string>({
    "{\"a\":\"\n\"}", "{\"a\":\"\\n\"}"
  }));
  std::stringstream commentsOnFailure;
  for (int i = 0; i < toRecode.size; i ++) {
    JSData parser;
    std::string input = toRecode[i][0];
    if (!parser.readstring(input, &commentsOnFailure)) {
      crash << "Failed to decode "<< toRecode[i][0] << crash;
    }
    std::string expectedOutput = toRecode[i][1];
    std::string recoded = parser.ToString(false);
    if (recoded != expectedOutput) {
      crash << "Input " << input << " decoded to " << recoded << ". However, I expected: " << expectedOutput << ". " << crash;
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
      crash << "Successfully decoded invalid json: " << input << "." << crash;
    }
  }
  return true;
}