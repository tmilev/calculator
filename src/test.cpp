// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#include "calculator.h"
#include "crypto.h"
#include "webserver.h"
#include "database.h"
#include "calculator_html_interpretation.h"
#include "signals_infrastructure.h"

class Test {
public:
  class Suites {
  public:
    static const std::string all;
    static const std::string API;
    static const std::string database;
    static const std::string problems;
    static const std::string build;
    static const std::string crypto;
    static const std::string topicLists;
    static const std::string topiclists;
    static const std::string calculator;
    static const std::string polynomial;
    static const std::string basic;
  };
  HashedList<std::string, MathRoutines::hashString> inputs;
  bool flagTestAll;
  void initialize(List<std::string>& inputArguments);
  void run();
  bool shouldTest(const std::string& testSuite);
};

int mainTest(List<std::string>& inputArguments) {
  SignalsInfrastructure::signals().initializeSignals();
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
const std::string Test::Suites::basic = "basic";
const std::string Test::Suites::API = "api";

void Test::run() {
  MacroRegisterFunctionWithName("Test::run");
  global << "Testing: " << this->inputs.toStringCommaDelimited() << " ..." << Logger::endL;
  global.millisecondsMaxComputation = 100000000;
  if (this->shouldTest(Test::Suites::database)) {
    Database::Test::all();
    global << Logger::green << "Database tests completed." << Logger::endL;
  }
  if (this->shouldTest(Test::Suites::basic)) {
    AlgebraicNumber::Test::all();
    StringRoutines::Test::all();
    JSData::Test::all();
    Expression::Test::all();
    LargeIntegerUnsigned::Test::all();
    Rational::Test::all();
    ElementZmodP::Test::all();
    RationalFraction<Rational>::Test::all();
    Vectors<Rational>::Test::all();
    Selection::Test::all();
    // Also tested in calculator test suite.
    Calculator::Examples::Test::all();
    ChevalleyGenerator::Test::all();
    PartialFractions::Test::all();
    global << Logger::green << "Basic tests completed." << Logger::endL;
  }
  if (this->shouldTest(Test::Suites::crypto)) {
    ASNObject::initializeNonThreadSafe();
    Crypto::Random::initializeRandomBytes();
    PrivateKeyRSA::Test::all();
    Crypto::Test::all();
    X509Certificate::Test::all();
    SSLRecord::Test::all();
  }
  if (this->shouldTest(Test::Suites::API)) {
    WebAPIResponse::Test::all();
  }
  if (this->shouldTest(Test::Suites::polynomial)) {
    MonomialPolynomial::Test::all();
    Polynomial<Rational>::Test::all();
    PolynomialUnivariateModular::Test::all();
    PolynomialModuloPolynomialModuloInteger::Test::all();
    PolynomialFactorizationFiniteFields::Test::all();
  }
  if (
    this->shouldTest(Test::Suites::topicLists) ||
    this->shouldTest(Test::Suites::topiclists)
  ) {
    TopicElementParser::Test::all();
  }
  if (this->shouldTest(Test::Suites::calculator)) {
    Calculator::Examples::Test::all();
    Calculator::Test::all();
  }
  if (this->shouldTest(Test::Suites::problems)) {
    CalculatorHTML::Test::all();
  }
  if (this->shouldTest(Test::Suites::build)) {
    GlobalVariables::Test::all();
  }
  global << Logger::green << "All tests passed. " << Logger::endL;
}

bool Test::shouldTest(const std::string& testSuite) {
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
