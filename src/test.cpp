#include "calculator.h"
#include "crypto.h"
#include "webserver.h"
#include "database.h"
#include "calculator_html_interpretation.h"
#include "signals_infrastructure.h"
#include "source_code_formatter.h"
#include "math_general_implementation.h"
#include "calculator_inner_functions.h"
#include "string_constants.h"

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
    static const std::string freecalc;
    static const std::string calculator;
    static const std::string polynomial;
    static const std::string basic;
    static const std::string courses;
  };

  static std::string update;
  HashedList<std::string> inputs;
  bool flagTestAll;
  // If this is set, AB tests will not be compared to the known results,
  // but instead the known results file will be updated.
  // Recall that an AB tests ("golden test") is running a bunch of input
  // and comparing the output to previously recorded outputs.
  // Use this flag to ignore previously recorded behavior and instead reset
  // the expected outputs.
  // Use with appropriate caution to avoid
  // accidentally record undesired bad behavior.
  static bool flagUpdateABTests;
  void initialize(List<std::string>& inputArguments);
  void run();
  bool shouldTest(const std::string& testSuite);
  Test();
};

std::string Test::update = "update";
bool Test::flagUpdateABTests = false;
const std::string Test::Suites::all = "all";
const std::string Test::Suites::database = "database";
const std::string Test::Suites::problems = "problems";
const std::string Test::Suites::courses = "courses";
const std::string Test::Suites::crypto = "crypto";
const std::string Test::Suites::freecalc = "freecalc";
const std::string Test::Suites::topicLists = "topicLists";
const std::string Test::Suites::topiclists = "topiclists";
const std::string Test::Suites::calculator = "calculator";
const std::string Test::Suites::polynomial = "polynomial";
const std::string Test::Suites::build = "build";
const std::string Test::Suites::basic = "basic";
const std::string Test::Suites::API = "api";

int mainTest(List<std::string>& inputArguments) {
  SignalsInfrastructure::signals().initializeSignals();
  Test tester;
  inputArguments.sliceInPlace(2, inputArguments.size - 2);
  tester.initialize(inputArguments);
  tester.run();
  return 0;
}

int mainFormat() {
  SignalsInfrastructure::signals().initializeSignals();
  CodeFormatter::formatCalculatorInPlace();
  return 0;
}

void Test::run() {
  STACK_TRACE("Test::run");
  global
  << "Testing "
  << this->inputs.size
  << " test cases: "
  << this->inputs.toStringCommaDelimited()
  << " ..."
  << Logger::endL;
  global.millisecondsMaxComputation = 100000000;
  if (this->shouldTest(Test::Suites::database)) {
    QuerySet::Test::all();
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
    Matrix<Rational>::Test::all();
    global << Logger::green << "Basic tests completed." << Logger::endL;
  }
  if (this->shouldTest(Test::Suites::crypto)) {
    ASNObject::initializeNonThreadSafe();
    Crypto::Random::initializeRandomBytes();
    PrivateKeyRSA::Test::all();
    Crypto::Test::all();
    X509Certificate::Test::all();
    SSLRecord::Test::all();
    global << Logger::green << "Crypto tests completed." << Logger::endL;
  }
  if (this->shouldTest(Test::Suites::API)) {
    WebAPIResponse::Test::all();
    global << Logger::green << "API tests completed." << Logger::endL;
  }
  if (this->shouldTest(Test::Suites::polynomial)) {
    PolynomialUnivariateModularAsModulus::Test::all();
    PolynomialModuloPolynomialModuloInteger::Test::all();
    PolynomialFactorizationCantorZassenhaus<
      PolynomialModuloPolynomialModuloInteger,
      PolynomialUnivariateModular,
      PolynomialUnivariateModularAsModulus
    >::Test::all();
    PolynomialConversions::Test::all();
    MonomialPolynomial::Test::all();
    Polynomial<Rational>::Test::all();
    PolynomialUnivariateModular::Test::all();
    PolynomialFactorizationFiniteFields::Test::all();
    global << Logger::green << "Polynomial tests completed." << Logger::endL;
  }
  if (
    this->shouldTest(Test::Suites::topicLists) ||
    this->shouldTest(Test::Suites::topiclists)
  ) {
    TopicElementParser::Test::all();
    global << Logger::green << "Topic tests completed." << Logger::endL;
  }
  if (this->shouldTest(Test::Suites::freecalc)) {
    CalculatorFunctionsFreecalc::Test::all();
    global << Logger::green << "Freecalc tests completed." << Logger::endL;
  }
  if (this->shouldTest(Test::Suites::courses)) {
    Course::Test::all();
  }
  if (this->shouldTest(Test::Suites::calculator)) {
    if (Test::flagUpdateABTests) {
      global
      << Logger::red
      << "About to erase the test file name."
      << Logger::endL;
      global << "Enter y or Y to confirm: " << Logger::endL;
      std::string userInput;
      std::cin >> userInput;
      if (userInput == "y" || userInput == "Y") {
        FileOperations::deleteFileVirtual(
          WebAPI::calculator::testFileNameVirtual, nullptr
        );
        global << Logger::blue << " test file deleted. " << Logger::endL;
      } else {
        global
        << Logger::green
        << " test file deletion aborted. "
        << Logger::endL;
      }
    }
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

Test::Test() {
  this->flagTestAll = false;
  this->flagUpdateABTests = false;
}

void Test::initialize(List<std::string>& inputArguments) {
  this->inputs = inputArguments;
  if (this->inputs.contains(Test::update)) {
    this->flagUpdateABTests = true;
  } else {
    this->flagUpdateABTests = false;
  }
  global
  << "Input arguments: "
  << inputArguments.toStringCommaDelimited()
  << Logger::endL;
  if (
    this->inputs.size == 0 || this->inputs.contains(Test::Suites::all)
  ) {
    this->flagTestAll = true;
  } else {
    this->flagTestAll = false;
  }
}
