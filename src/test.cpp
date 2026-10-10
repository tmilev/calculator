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

int MainFunctions::mainTest(List<std::string>& inputArguments) {
  SignalsInfrastructure::signals().initializeSignals();
  Testing::Test tester;
  inputArguments.sliceInPlace(2, inputArguments.size - 2);
  tester.initialize(inputArguments);
  tester.run();
  return 0;
}

namespace Testing {

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
const std::string Test::Suites::topics = "topics";
const std::string Test::Suites::calculator = "calculator";
const std::string Test::Suites::polynomial = "polynomial";
const std::string Test::Suites::build = "build";
const std::string Test::Suites::json = "json";
const std::string Test::Suites::wasm = "wasm";
const std::string Test::Suites::basic = "basic";
const std::string Test::Suites::expressions = "expressions";
const std::string Test::Suites::API = "api";
const std::string Test::Suites::scientific = "scientific";

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
    QueryUpdateTest::all();
    DatabaseTest::all();
    global << Logger::green << "Database tests completed." << Logger::endL;
  }
  if (this->shouldTest(Test::Suites::json)) {
    JSDataTest::all();
    global << Logger::green << "Json tests completed." << Logger::endL;
  }
  if (this->shouldTest(Test::Suites::expressions)) {
    ExpressionTest::all();
  }
  if (this->shouldTest(Test::Suites::basic)) {
    ListReferencesTest::all();
    HashedListTest::all();
    AlgebraicNumberTest::all();
    StringRoutinesTest::all();
    LargeIntegerUnsignedTest::all();
    RationalTest::all();
    ElementZmodPTest::all();
    RationalFractionTest::all();
    VectorsTest::all();
    VectorTest::all();
    SelectionTest::all();
    // Also tested in calculator test suite.
    CalculatorExamplesTest::all();
    ChevalleyGeneratorTest::all();
    PartialFractionsTest::all();
    MatrixTest::all();
    global << Logger::green << "Basic tests completed." << Logger::endL;
  }
  if (this->shouldTest(Test::Suites::crypto)) {
    ASNObject::initializeNonThreadSafe();
    Crypto::Random::initializeRandomBytes();
    PrivateKeyRSATest::all();
    CryptoTest::all();
    X509CertificateTest::all();
    SSLRecordTest::all();
    global << Logger::green << "Crypto tests completed." << Logger::endL;
  }
  if (this->shouldTest(Test::Suites::API)) {
    WebAPIResponseTest::all();
    global << Logger::green << "API tests completed." << Logger::endL;
  }
  if (this->shouldTest(Test::Suites::polynomial)) {
    PolynomialUnivariateModularAsModulusTest::all();
    PolynomialModuloPolynomialModuloIntegerTest::all();
    PolynomialFactorizationCantorZassenhausTest::all();
    PolynomialConversionsTest::all();
    MonomialPolynomialTest::all();
    PolynomialTest::all();
    PolynomialUnivariateModularTest::all();
    PolynomialFactorizationFiniteFieldsTest::all();
    global << Logger::green << "Polynomial tests completed." << Logger::endL;
  }
  if (this->shouldTest(Test::Suites::scientific)) {
    global
    << Logger::blue
    << "Scientific function test start ..."
    << Logger::endL;
    ElementUniversalEnvelopingTest::all();
    SemisimpleSubalgebrasTest::all();
    global
    << Logger::green
    << "Scientific function tests passed."
    << Logger::endL;
  }
  if (
    this->shouldTest(Test::Suites::topicLists) ||
    this->shouldTest(Test::Suites::topiclists) ||
    this->shouldTest(Test::Suites::topics)
  ) {
    TopicElementParserTest::all();
    global << Logger::green << "Topic tests completed." << Logger::endL;
  }
  if (this->shouldTest(Test::Suites::freecalc)) {
    CalculatorFunctionsFreecalcTest::all();
    global << Logger::green << "Freecalc tests completed." << Logger::endL;
  }
  if (this->shouldTest(Test::Suites::courses)) {
    CourseTest::all();
  }
  if (this->shouldTest(Test::Suites::calculator)) {
    CalculatorParserTest::all();
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
      global << Logger::blue << "Test file deleted. " << Logger::endL;
    }
    CalculatorExamplesTest::all();
    CalculatorTest::all(Test::flagUpdateABTests);
  }
  if (this->shouldTest(Test::Suites::problems)) {
    CalculatorHTMLTest::all();
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
    DatabaseTest::noShutdownSignal();
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
  if (this->inputs.size == 0 || this->inputs.contains(Test::Suites::all)) {
    this->flagTestAll = true;
  } else {
    this->flagTestAll = false;
  }
}
}
