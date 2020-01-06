// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"
#include "crypto.h"
#include "webserver.h"
#include "database.h"
#include "calculator_html_interpretation.h"

static ProjectInformationInstance projectInfoTestRoutinesCPP(__FILE__, "Testing routines. ");

class Test {
public:
  class Suites{
  public:
    static const std::string all;
    static const std::string database;
    static const std::string problems;
  };
  HashedList<std::string, MathRoutines::HashString> inputs;
  bool flagTestAll;
  void initialize(List<std::string>& inputArguments);
  void Run();
  bool ShouldTest(const std::string& testSuite);
};

int mainTest(List<std::string>& inputArguments) {
  Test tester;
  tester.initialize(inputArguments);
  tester.Run();
  return 0;
}

const std::string Test::Suites::all = "all";
const std::string Test::Suites::database = "database";
const std::string Test::Suites::problems = "problems";

void Test::Run() {
  global << "Testing ..." << logger::endL;
  global.millisecondsMaxComputation = 100000000;
  if (this->ShouldTest(Test::Suites::database)) {
    Database::Test::All();
  }
  if (this->ShouldTest(Test::Suites::problems)) {
    CalculatorHTML::Test::All();
  }
  if (this->flagTestAll) {
    JSData::Test::All();
    ASNObject::initializeNonThreadSafe();
    PrivateKeyRSA::Test::All();
    Crypto::Test::All();
    X509Certificate::Test::All();
    Expression::Test::All();
    LargeIntegerUnsigned::Test::All();
    Calculator::Test::All();
    GlobalVariables::Test::All();
  }
  global << logger::green << "All tests passed. " << logger::endL;

}

bool Test::ShouldTest(const std::string &testSuite) {
  if (this->flagTestAll) {
    return true;
  }
  return this->inputs.Contains(testSuite);
}

void Test::initialize(List<std::string>& inputArguments) {
  this->inputs = inputArguments;
  if (
    this->inputs.size == 0 ||
    this->inputs.Contains(Test::Suites::all)
  ) {
    this->flagTestAll = true;
  } else {
    this->flagTestAll = false;
  }
}
