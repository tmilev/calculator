#include "calculator_html_interpretation.h"
#include "calculator_inner_functions.h"
#include "crypto_calculator.h"
#include "database.h"
#include "general_file_operations_encodings.h"
#include "main.h"
#include "signals_infrastructure.h"
#include "string_constants.h"
#include "test.h"
#include "transport_layer_security.h"
#include "web_api.h"
#include <iostream>

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
const std::string Test::Suites::json = "json";
const std::string Test::Suites::wasm = "wasm";
const std::string Test::Suites::basic = "basic";
const std::string Test::Suites::API = "api";

int MainFunctions::mainTest(List<std::string>& inputArguments) {
  SignalsInfrastructure::signals().initializeSignals();
  Test tester;
  inputArguments.sliceInPlace(2, inputArguments.size - 2);
  tester.initialize(inputArguments);
  tester.run();
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
    QueryUpdate::Test::all();
    Database::Test::all();
    global << Logger::green << "Database tests completed." << Logger::endL;
  }
  if (this->shouldTest(Test::Suites::json)) {
    JSData::Test::all();
    global << Logger::green << "Json tests completed." << Logger::endL;
  }
  if (this->shouldTest(Test::Suites::basic)) {
    AlgebraicNumber::Test::all();
    StringRoutines::Test::all();
    Expression::Test::all();
    LargeIntegerUnsigned::Test::all();
    Rational::Test::all();
    ElementZmodP::Test::all();
    RationalFraction<Rational>::Test::all();
    Vectors<Rational>::Test::all();
    VectorTest::all();
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
      std::stringstream comments;
      bool success =
      FileOperations::deleteFileVirtual(
        WebAPI::Calculator::testFileNameVirtual, false, &comments
      );
      if (!success) {
        global.fatal
        << "Failed to erase WebAPI::Calculator::testFileNameVirtual. "
        << comments.str()
        << global.fatal;
      }
      global << Logger::blue << " test file deleted. " << Logger::endL;
    }
    Calculator::Examples::Test::all();
    Calculator::Test::all(Test::flagUpdateABTests);
  }
  if (this->shouldTest(Test::Suites::problems)) {
    CalculatorHTML::Test::all();
  }
  if (this->shouldTest(Test::Suites::build)) {
    GlobalVariables::Test::all();
  }
  if (this->shouldTest(Test::Suites::wasm)) {
    GlobalVariables::Test::webAssemblyBuild();
  }
  if (this->shouldTest(Test::Suites::database)) {
    // This special test can only run once per
    // test executable, and it must run last.
    Database::Test::noShutdownSignal();
  }
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
    this->inputs.removeFirstOccurenceSwapWithLast(Test::update);
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
