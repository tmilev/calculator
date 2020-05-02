// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"
#include "crypto.h"
#include "webserver.h"
#include "database.h"
#include "calculator_html_interpretation.h"

class Test {
public:
  class Suites {
  public:
    static const std::string all;
    static const std::string database;
    static const std::string problems;
    static const std::string build;
    static const std::string crypto;
    static const std::string topicLists;
    static const std::string topiclists;
    static const std::string calculator;
    static const std::string polynomial;
  };
  HashedList<std::string, MathRoutines::hashString> inputs;
  bool flagTestAll;
  void initialize(List<std::string>& inputArguments);
  void run();
  bool ShouldTest(const std::string& testSuite);
};

int mainTest(List<std::string>& inputArguments) {
  Test tester;
  inputArguments.sliceInPlace(2, inputArguments.size - 2);
  tester.initialize(inputArguments);
  tester.run();
  return 0;
}

const std::string Test::Suites::all = "all";
const std::string Test::Suites::database = "database";
const std::string Test::Suites::problems = "problems";
const std::string Test::Suites::crypto = "crypto";
const std::string Test::Suites::topicLists = "topicLists";
const std::string Test::Suites::topiclists = "topiclists";
const std::string Test::Suites::calculator = "calculator";
const std::string Test::Suites::polynomial = "polynomial";
const std::string Test::Suites::build = "build";

void Test::run() {
  MacroRegisterFunctionWithName("Test::run");
  global << "Testing ..." << Logger::endL;
  global.millisecondsMaxComputation = 100000000;
  if (this->ShouldTest(Test::Suites::database)) {
    Database::Test::all();
  }
  if (this->flagTestAll) {
    JSData::Test::all();
    Expression::Test::all();
    LargeIntegerUnsigned::Test::all();
    Rational::Test::all();
    ElementZmodP::Test::all();
    RationalFunction::Test::all();
  }
  if (this->ShouldTest(Test::Suites::crypto)) {
    ASNObject::initializeNonThreadSafe();
    Crypto::Random::initializeRandomBytes();
    PrivateKeyRSA::Test::all();
    Crypto::Test::all();
    X509Certificate::Test::all();
    SSLRecord::Test::all();
  }
  if (this->ShouldTest(Test::Suites::polynomial)) {
    MonomialP::Test::all();
    Polynomial<Rational>::Test::all();
  }
  if (this->ShouldTest(Test::Suites::calculator)) {
    Calculator::Test::all();
  }
  if (this->ShouldTest(Test::Suites::problems)) {
    CalculatorHTML::Test::all();
  }
  if (
    this->ShouldTest(Test::Suites::topicLists) ||
    this->ShouldTest(Test::Suites::topiclists)
  ) {
    TopicElementParser::Test::all();
  }
  if (this->ShouldTest(Test::Suites::build)) {
    GlobalVariables::Test::all();
  }
  global << Logger::green << "All tests passed. " << Logger::endL;
}

bool Test::ShouldTest(const std::string& testSuite) {
  if (this->flagTestAll) {
    return true;
  }
  return this->inputs.contains(testSuite);
}

void Test::initialize(List<std::string>& inputArguments) {
  this->inputs = inputArguments;
  global << "Input arguments: " << inputArguments.toStringCommaDelimited() << Logger::endL;
  if (
    this->inputs.size == 0 ||
    this->inputs.contains(Test::Suites::all)
  ) {
    this->flagTestAll = true;
  } else {
    this->flagTestAll = false;
  }
}
