#ifndef header_test_ALREADY_INCLUDED
#define header_test_ALREADY_INCLUDED

#include "general_lists.h"
#include "math_extra_polynomial_factorization.h"
#include "transport_layer_security.h"

namespace Testing {

class Test {
public:
  class Suites {
  public:
    static const std::string all;
    static const std::string API;
    static const std::string database;
    static const std::string problems;
    static const std::string build;
    static const std::string wasm;
    static const std::string crypto;
    static const std::string topicLists;
    static const std::string topiclists;
    static const std::string topics;
    static const std::string freecalc;
    static const std::string calculator;
    static const std::string json;
    static const std::string polynomial;
    static const std::string basic;
    static const std::string expressions;
    static const std::string scientific;
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

class VectorTest {
public:
  static bool all();
  static bool order();
  static Vector<Rational> fromString(const std::string& input);
};

class CalculatorParserTest {
public:
  static bool all();
  static bool whitespace();
  static bool largeExample1(Calculator& initializedTester);
};

class ListReferencesTest {
public:
  static bool all();
  // The purpose of this test is to check
  // that adding elements to a list of references does not
  // accidentally incur quadratic complexity.
  static bool largeListReferences1(int millionMultiple);
  static bool quicksortReversedList(int numberOfElements);
};

class HashedListTest {
public:
  static bool all();
  static bool largeHashedList1();
  static bool largeHashedList2();
  static bool largeHashedListReferences1(int millionMultiple);
  static int incrementIntPseudorandomly(int counter);
};

enum ExpressionTestType {
  Sum,
  SumProduct,
  Arithmetic,
  NestedList,
};

class ExpressionTest {
public:
  static bool all();
  static bool builtInAtomValues(Calculator& owner);
  static bool largeNestedExpressionCreationSpeed1(Calculator& owner);
  static bool toStringTestRecode(Calculator& owner);
  static bool toStringTestRecodeOnce(
    const std::string& inputHardCodedMustParse, Calculator& owner
  );
  static bool isUserDefinedAtomOnce(
    Calculator& owner, const std::string& input, bool isUserDefinedAtom
  );
  static bool isUserDefinedAtom(Calculator& owner);
  static void makeLargeAdditionSumNested(
    Calculator& owner, Expression& output, int iterations
  );
  static void makeLargeAdditionSumProductNested(
    Calculator& owner, Expression& output, int iterations
  );
  static void makeLargeExpressionArithmeticNested(
    Calculator& owner, Expression& output, int iterations
  );
  static void makeLargeNestedList(
    Calculator& owner, Expression& output, int iterations
  );
  static void makeLarge(
    Calculator& owner,
    Expression& output,
    ExpressionTestType expressionType,
    int iterations
  );
  static bool largeStringConversion(
    Calculator& owner,
    bool mathML,
    ExpressionTestType expressionType,
    int iterations
  );
  static bool smallStringConversion(
    Calculator& owner,
    bool mathML,
    ExpressionTestType expressionType,
    int iterations,
    const std::string& expected
  );
  static std::string expressionTypeToString(ExpressionTestType expressionType);
};

class JSDataTest {
public:
  static bool all();
  static bool keyAccessUsingOperator();
  static bool recode();
  static bool recodeOnce(const List<std::string>& pair, bool relaxedInput);
  static bool recodeRelaxed();
  static bool decodeEscapedUnicode();
  static bool endcodeNonstandardWhitespace();
  static bool badInput();
  static bool loadLarger();
};

class MatrixTest {
public:
  static bool all();
  static bool matrixIntegerWithDenominator();
  static bool oneMatrixIntegerWithDenominator(
    const std::string& input,
    const std::string& expectedMatrix,
    int expectedScale
  );
  // Makes a matrix from a string such as
  // ((2,3), (3,4), (4,5))
  // Converted to matrix using the calculator MakeMatrix function.
  static void matrixFromString(
    const std::string& inputString, Matrix<Rational>& output
  );
};

class CalculatorTest {
public:
  static bool all(bool updateABTestFile);
  static bool cacheWorks();
  static bool loopDetection();
  static bool loopDetectionCycle();
  static bool loopDetectionEverExpanding();
  static bool numberOfTestFunctions(Calculator& ownerInitialized);
  static bool parseDecimal(Calculator& ownerInitialized);
  static bool parseQuotes(Calculator& ownerInitialized);
  static bool parseAllExamples(Calculator& ownerInitialized);
  static bool builtInFunctionsABTest(
    Calculator& ownerInitialized, bool updateABTestFile
  );
  static bool checkBuiltInInitializations(Calculator& ownerInitialized);
};

class CalculatorExamplesTest {
public:
  static bool compose();
  static bool all();
};

class AlgebraicNumberTest {
public:
  // Constructs an algebraic number from string.
  // Owned by an instance of the Calculator.
  static bool fromString(
    const std::string& input, Calculator& inputOwner, AlgebraicNumber& output
  );
  // Same as fromString but will crash if the input
  // cannot be parsed.
  static AlgebraicNumber fromStringWithoutFailure(
    const std::string& input, Calculator& inputOwner
  );
  static bool all();
  static bool constantValues();
  static bool evaluatesToComplex();
  static bool hashFunction();
};

class WebAPIResponseTest {
public:
  static bool scoredQuiz(DatabaseType databaseType);
  static bool all();
  static bool solveJSON();
  static bool compareExpressions();
  static bool addUsersFromData();
  static bool forgotLogin();
  static bool changePasswordEmailOnly();
  static bool changePassword();
  static bool signUp();
  static bool deleteAccount();
  static void extactActivationTokenFromEmail(
    const std::string& email, std::string& outputToken
  );
};

class SSLRecordTest {
public:
  static std::string sampleClientHelloHex;
  static bool all();
  static bool serialization();
  static bool serializationClientHello(
    TransportLayerSecurityServer& testServer
  );
};

class PolynomialFactorizationFiniteFieldsTest {
public:
  class TestCase {
  public:
    std::string toBeFactored;
    std::string desiredFactorization;
    Polynomial<Rational>::Test parser;
    bool run();
  };

  static bool all();
  static bool test(
    const std::string& toFactor, const std::string& desiredResult
  );
  static bool gelfondBound(
    const std::string& inputPolynomial, const std::string& desiredBound
  );
};

class PolynomialUnivariateModularTest {
public:
  static bool all();
  static bool greatestCommonDivisor();
  static bool division();
  static bool derivative();
  static bool testOneGreatestCommonDivisor(
    int modulusData,
    const std::string& left,
    const std::string& right,
    const std::string& expected
  );
  static bool testOneDivision(
    int modulusData,
    const std::string& dividend,
    const std::string& divisor,
    const std::string& expectedQuotient,
    const std::string& expectedRemainder
  );
  static bool testOneDerivative(
    int modulusData,
    const std::string& toBeDifferentiated,
    const std::string& expected
  );
  static PolynomialUnivariateModular fromStringAndModulus(
    const std::string& input, IntegerModulusSmall* modulus
  );
  static Polynomial<ElementZmodP> fromStringAndModulus(
    const std::string& input, int modulus
  );
  static std::string toStringPolynomialElementZModP(
    const Polynomial<ElementZmodP>& other
  );
};

class PolynomialUnivariateModularAsModulusTest {
public:
  static bool all();
  static bool oneTest(
    int modulus,
    const std::string& modulusPolynomial,
    const std::string& expectedImagesOfX
  );
};

class PolynomialModuloPolynomialModuloIntegerTest {
public:
  static bool all();
  static bool product();
  static bool testOneProduct(
    int modulus,
    const std::string& left,
    const std::string& right,
    const std::string& modulusPolynomial,
    const std::string& expected
  );
};

class PolynomialConversionsTest {
public:
  static bool all();
  static bool univariateModularToDense();
  static bool oneUnivariateModularToDense(
    int modulus, const std::string& input, const std::string& expected
  );
};

class PolynomialFactorizationCantorZassenhausTest {
public:
  static bool all();
  static bool constructStartingPolynomial();
  static bool testOneStartingPolynomial(
    int modulus, int constant, int currentDegree, const std::string& expected
  );
  static bool testOnce(
    int modulus, const std::string& input, const std::string& expected
  );
};

class LargeIntegerUnsignedTest {
public:
  static bool all();
  static bool serializationToHex(const LargeIntegerUnsigned& input);
  static bool serializationToHex();
  static bool comparisons();
  static bool isPossiblyPrime();
  static bool guaranteedPrime();
  static bool isPossiblyPrimeFast(
    const List<LargeIntegerUnsigned>& input,
    bool mustBeTrue,
    int millerRabinTries,
    int64_t maximumRunningTimeMilliseconds
  );
  static bool isPossiblyPrimeMillerRabinOnly(
    const List<LargeIntegerUnsigned>& input,
    bool mustBeTrue,
    int millerRabinTries
  );
  static bool factor();
  static bool factorSmall(
    const LargeIntegerUnsigned& input,
    const std::string& expectedFactors,
    const std::string& expectedMultiplicities,
    int maximumDivisorToTry,
    int numberMillerRabinRuns,
    int64_t maximumRunningTime
  );
};

class RationalTest {
public:
  static bool all();
  static bool testScale();
};

class ElementZmodPTest {
public:
  static bool all();
  static bool basicOperations();
  static bool scale();
};

class RationalFractionTest {
public:
  static bool all();
  static bool scaleNormalizeIndex();
  static RationalFraction<Rational> fromString(const std::string& input);
  static bool fromStringTest();
};

class SelectionTest {
public:
  static bool all();
  static bool testNElements(int n);
};
} // namespace: Testing.
#endif
