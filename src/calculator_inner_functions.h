
#ifndef header_calculator_inner_functions_ALREADY_INCLUDED
#define header_calculator_inner_functions_ALREADY_INCLUDED

#include "calculator_interface.h"

class CalculatorFunctionsCrypto {
public:
  static bool aes_cbc_256_encrypt(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool aes_cbc_256_decrypt(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool loadKnownCertificates(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool testLoadPEMCertificates(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool testLoadPEMPrivateKey(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool testTLSMessageSequence(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool testTLSDecodeSSLRecord(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool jwtVerifyAgainstKnownKeys(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool jwtVerifyAgainstRSA256(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool RSAEncrypt(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool RSASign(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool showKnownObjectIds(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool sha224OfString(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool generateRandomPrime(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool testRSASign(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool sha256OfString(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool sha512(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool ripemd160OfString(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool sha3_256OfString(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool keccak256OfString(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool sha256OfStringVerbose(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool sha1OfString(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool functionHashString(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    const std::string& hashId,
    bool verbose
  );
  static bool testASN1Decode(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool x509CertificateServer(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool x509CertificateDecode(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool appendDoubleSha256Check(
    Calculator& calculator, const Expression& input, Expression& output
  );
};

class CalculatorFunctionsEncoding {
public:
  static bool convertBase64ToString(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool convertStringToHex(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool convertCharToBase64(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool testJSON(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool convertBase64ToHex(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool convertIntegerToHex(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool convertHexToInteger(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool convertHexToString(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool convertBase58ToHex(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool convertHexToBase58(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool convertIntegerUnsignedToBase58(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool base64ToCharToBase64Test(
    Calculator& calculator, const Expression& input, Expression& output
  );
};

class CalculatorFunctionsPlot {
public:
  static bool plotCoordinateSystem(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool plotSetProjectionScreenBasis(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool plotSurface(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool plotImplicitFunctionFull(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    bool showGrid
  );
  static bool plotImplicitFunction(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool plotImplicitShowGridFunction(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool plotSegment(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool plotMarkSegment(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool plotPath(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool plotParametricCurve(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool plotDirectionField(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool plotVectorField(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool plotDirectionOrVectorField(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    bool vectorsAreNormalized
  );
  static bool plotPolarRfunctionThetaExtended(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool plotPolarRfunctionTheta(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool isPlot(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool plot2D(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool plot2DOverIntervals(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool plotFill(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool plotRectangle(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool plotRemoveCoordinateAxes(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool plotGrid(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool plotLabel(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool plotLatex(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool plotSetId(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool plotViewRectangle(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool plotViewWindow(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool plotPoint(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool plot2DWithBars(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool makeJavascriptExpression(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool drawExpressionGraphWithOptions(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    bool useFullTree
  );
  static bool drawExpressionGraph(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorFunctionsPlot::drawExpressionGraphWithOptions(
      calculator, input, output, false
    );
  }
  static bool drawExpressionGraphFull(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorFunctionsPlot::drawExpressionGraphWithOptions(
      calculator, input, output, true
    );
  }
};

class JavascriptExtractor {
private:
  bool extractJavascriptRecursive(
    const Expression& input,
    std::string& output,
    std::stringstream* commentsOnFailure
  );
  bool extractFromAtom(const Expression& input, std::string& output);
  bool extractFromSequence(
    const Expression& input,
    std::string& output,
    std::stringstream* commentsOnFailure
  );
  bool extractFromBinaryOrUnary(
    const Expression& input,
    std::string& output,
    std::stringstream* commentsOnFailure
  );
public:
  Calculator* owner;
  Expression startingExpression;
  std::string result;
  int recursionDepth;
  // Parameters on the graph.
  std::string parametersOnTheGraphLetter;
  MapList<std::string, double, HashFunctions::hashFunction>
  parametersOnTheGraph;
  // Parameters in sliders.
  std::string parameterLetter;
  HashedList<std::string> parameterNames;
  HashedList<std::string> parameterNamesJS;
  JavascriptExtractor(Calculator& inputOwner);
  bool extractJavascript(
    const Expression& input, std::stringstream* commentsOnFailure
  );
  bool extract(
    const Expression& input,
    std::string& output,
    std::stringstream* commentsOnFailure
  );
  void writeParameterNames(PlotObject& output);
};

class CalculatorFunctionsTrigonometry {
public:
  static bool sin(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool cos(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool sec(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool csc(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool tan(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool cotangent(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool arcsinAlgebraic(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool arccosAlgebraic(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool arcsin(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool arccos(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool arctan(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool exploitCosineEvenness(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool exploitSineOddness(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool trigonometricSumToTrigonometricProduct(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool sineOfAngleSumToTrigonometry(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool cosineOfAngleSumToTrigonometry(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool convertSineToExponent(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool convertCosineToExponent(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool eulerFormulaAsLaw(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool fourierFractionForm(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool maximalCosineForm(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool maximalSineForm(
    Calculator& calculator, const Expression& input, Expression& output
  );
};

class CalculatorFunctionsIntegration {
public:
  static bool integrateRationalFunctionSplitToBuidingBlocks(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool integrateRationalFunctionBuildingBlockIa(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool integrateRationalFunctionBuidingBlockIb(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool integrateRationalFunctionBuidingBlockIIaandIIIa(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool integrateRationalFunctionBuidingBlockIIb(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool integrateRationalFunctionBuidingBlockIIIb(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool integrateDefiniteIntegral(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool integratePowerByUncoveringParenthesisFirst(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool integrateSum(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool integratePullImaginaryUnit(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool integrateXnDiffX(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool integratePullConstant(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool integrateSqrtOneMinusXsquared(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool integrateSqrtXsquaredMinusOne(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool integrateXpowerNePowerAx(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool integrateEpowerAxDiffX(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool integrateSinPowerNCosPowerM(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool integrateTanPowerNSecPowerM(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool integralOperator(
    Calculator& calculator, const Expression& input, Expression& output
  );
};

class CalculatorFunctionsDifferentiation {
public:
  static bool differentiateTrigAndInverseTrig(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool differentiateChainRule(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool differentiateAplusB(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool differentiateAtimesB(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool differentiateAdivideBCommutative(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool differentiateAdivideBNONCommutative(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool differentiateConstant(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool differentiateX(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool differentiateConstPower(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool differentiateAPowerB(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool diffdivDiffxToDifferentiation(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool ddivDxToDiffDivDiffx(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool interpretAsDifferential(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool atomToDifferential(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool differentiateSqrt(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool differentiateWithRespectToXTimesAny(
    Calculator& calculator, const Expression& input, Expression& output
  );
};

class CalculatorFunctionsAlgebraic {
public:
  static bool convertAlgebraicNumberToMatrix(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool getAlgebraicNumberFromMinimalPolynomial(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool printAlgebraicClosureStatus(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool isAlgebraicRadical(
    Calculator& calculator, const Expression& input, Expression& output
  );
};

class CalculatorFunctionsBasic {
public:
  static bool factorial(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool logarithmBaseNaturalToLn(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool logarithmBaseSimpleCases(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool logarithmBase(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool logarithm(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool absoluteValue(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool floor(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool round(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool logarithmBaseNCeiling(
    Calculator& calculator, const Expression& input, Expression& output
  );
};

class CalculatorFunctionsListsAndSets {
public:
  static bool intersection(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool listUnion(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool unionNoRepetition(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool length(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool intersectUnion(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool unionEmptySet(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool lastElement(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool removeLastElement(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool belongsTo(
    Calculator& calculator, const Expression& input, Expression& output
  );
};

class CalculatorFunctionsIntervals {
public:
  static bool unionUnionIntervals(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool unionIntervals(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool intersectIntervals(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool normalizeIntervals(
    Calculator& calculator, const Expression& input, Expression& output
  );
};

class CalculatorFunctionsFreecalc {
public:
  static bool buildFreecalc(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool buildFreecalcSingleSlides(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool buildFreecalcSlidesOnTopic(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool crawlTexFile(
    Calculator& calculator, const Expression& input, Expression& output
  );
};

class CalculatorFunctionsLinearAlgebra {
public:
  static bool minimalPolynomialMatrix(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool characteristicPolynomialMatrix(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool functionToMatrix(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool diagonalizeMatrix(
    Calculator& calculator, const Expression& input, Expression& output
  );
};

class CalculatorFunctionsComplexDynamics {
public:
  static bool plotEscapeMap(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    bool mandelbrotMode
  );
  static bool plotJuliaSet(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool plotMandelbrotSet(
    Calculator& calculator, const Expression& input, Expression& output
  );
};

class CalculatorFunctions {
public:
  static bool leftIntervalGreaterThanRight(
    const Expression& left, const Expression& right
  );
  static bool printRuleStack(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool crash(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool crashByListOutOfBounds(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool crashByVectorOutOfBounds(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool testProblemInterpretation(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool testTopicListProblems(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool precomputeSemisimpleLieAlgebraStructure(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool turnRulesOnOff(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    bool turnOff
  );
  static bool turnOffRules(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool turnOnRules(
    Calculator& calculator, const Expression& input, Expression& output
  );
  // Returns false. The purpose of this function is to make the
  // turnOnApproximations implementation easier.
  static bool approximationsDummy(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool turnOnApproximations(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool turnOffApproximations(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool equalityToArithmeticExpression(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool functionEqualityToArithmeticExpression(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool randomInteger(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool selectAtRandom(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool setRandomSeed(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool convertElementZmodPToInteger(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool urlStringToNormalString(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool stringToURL(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool urlKeyValuePairsToNormalRecursive(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool stringToAtom(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool expressionToString(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool expressionToUTF8String(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool quoteToString(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool sliceString(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool stringDifference(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool nistEllipticCurveGenerator(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool nistEllipticCurveOrder(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool crossProduct(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool distributeExponent(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool distributeSqrt(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool fourierTransformElementWeylAlgebra(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool sqrt(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool factorInteger(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool numberOfIntegerDivisors(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool functionFactorInteger(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool factorOutNumberContent(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool isProductLinearOrConstantTermsIn(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool isProductTermsUpToPower(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool isLinearOrConstantIn(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool coefficientOf(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool constantTermRelative(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool compositeArithmeticOperationEvaluatedOnArgument(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool matchesPattern(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool thaw(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool getFreeVariablesIncludeNamedConstants(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool getFreeVariablesExcludeNamedConstants(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool compareFunctionsNumerically(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool compareExpressionsNumerically(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool compareExpressionsNumericallyAtPoints(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool ensureExpressionDependsOnlyOnMandatoryVariables(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    bool excludeNamedConstants
  );
  static bool
  ensureExpressionDependsOnlyOnMandatoryVariablesExcludeNamedConstants(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorFunctions::ensureExpressionDependsOnlyOnMandatoryVariables(
      calculator, input, output, true
    );
  }
  static bool
  ensureExpressionDependsOnlyOnMandatoryVariablesIncludeNamedConstants(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorFunctions::ensureExpressionDependsOnlyOnMandatoryVariables(
      calculator, input, output, false
    );
  }
  static bool ensureExpressionDependsOnlyOnStandard(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool multiplySequence(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool sumSequence(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool sumTimesExpressionToSumOf(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool sumAsOperatorToSumInternalNotation(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool handleUnderscorePowerLimits(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool polynomialize(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool functionPolynomialize(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool applyToSubexpressionsRecurseThroughCalculusFunctions(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool subList(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool applyToList(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool numerator(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool denominator(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool dereferenceInterval(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool dereferenceSequenceOrMatrix(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool dereferenceSequenceStatements(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool compositeElementWeylAlgebraActOnPolynomial(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool convertPolynomialModularToPolynomialRational(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool compositeConstTimesAnyActOn(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool compositeAPowerBEvaluatedAtC(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool constantFunction(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool rationalFunctionSubstitution(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool getPointsImplicitly(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool compositeDifferentiateLog(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool operatorBounds(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool powerAnyToZero(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool powerExponentToLog(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool newtonsMethod(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool compareIntervalsNumerically(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool intersectEmptySet(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool getFirstSummandContaining(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool getSummand(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool collectMultiplicands(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool collectOpands(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool collectSummands(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool outerMergeConstantRadicals(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool outerCommuteConstants(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool outerDivideReplaceAdivBpowerItimesBpowerJ(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool atimesBpowerJplusEtcDivBpowerI(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool combineFractionsCommutative(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool sortTerms(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool addTerms(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool isPrimeMillerRabin(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool isPossiblyPrime(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool leastCommonMultipleInteger(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool greatestCommonDivisorInteger(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool isNilpotent(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool invertMatrixRationalFractionsVerbose(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool invertMatrix(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool differentialEquationsEulersMethod(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool scaleToLeadingUnit(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool removeDuplicates(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool sort(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool sortDescending(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool getUserDefinedSubExpressions(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool divideByNumber(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool divisionCancellations(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool associateDivisionDivision(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool associateAdivBdivCPowerD(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool outerEqualEqual(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool equalEqualEqual(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool maximum(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool minimum(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool lessThanOrEqualTo(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool greaterThanOrEqualTo(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool greaterThan(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool lessThan(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool outerCommuteAtimesBifUnivariate(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool outerCommuteAtimesBtimesCifUnivariate(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool differentialStandardHandler(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool differentialOfPolynomial(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool isDifferentialOneFormOneVariable(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool extractQuadraticCoefficientsWithRespectToVariable(
    const Expression& quadratic,
    const Expression& variable,
    Expression& outputCoeffVarSquared,
    Expression& outputCoeffLinTerm,
    Expression& outputConstTerm
  );
  static bool extractLinearCoefficientsWithRespectToVariable(
    const Expression& linearExpression,
    const Expression& variable,
    Expression& outputCoeffLinTerm,
    Expression& outputConstTerm
  );
  static bool powerImaginaryUnit(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool matrixTrace(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool reverseBytes(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool ifStandard(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool ifFrozen(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool orFunction(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool orIdentical(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool andFunction(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool solveUnivariatePolynomialWithRadicalsWithRespectTo(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool cardanoFormula(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool contains(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool expressionLeafs(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool lispify(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool lispifyFull(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool childExpression(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool degreesToRadians(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool evaluateToDouble(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool functionEvaluateToDouble(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool embedSemisimpleAlgebraInSemisimpleAlgebra(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool allPartitions(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool allVectorPartitions(
    Calculator& calculator, const Expression& input, Expression& output
  );
  template <class Coefficient>
  static bool functionDeterminant(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    int maxiumDimension,
    bool convertByComputation
  );
  static bool determinant(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool highestWeightTransposeAntiAutomorphismBilinearForm(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool coefficientsPowersOf(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool intervalOpenFromSequence(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool intervalClosedFromSequence(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool intervalLeftClosedFromSequence(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool intervalRightClosedFromSequence(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool isNonEmptySequence(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool isSquare(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool isSquareFree(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool isSquareFreePolynomial(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool isPower(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool isEven(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool isReal(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool isConstant(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool gaussianEliminationMatrix(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool splitToPartialFractionsOverAlgebraicRealsAlgorithm(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool splitToPartialFractionsOverAlgebraicReals(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool functionSplitToPartialFractionsOverAlgebraicReals(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool testTopCommand(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool testIndicator(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool findProductDistanceModN(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool solveProductSumEquationOverSetModN(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool solveSerreLikeSystem(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    bool useUpperLimit,
    bool startWithAlgebraicClosure
  );
  static bool solveSerreLikeSystemNoUpperLimit(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorFunctions::solveSerreLikeSystem(
      calculator, input, output, false, false
    );
  }
  static bool solveSerreLikeSystemUpperLimit(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorFunctions::solveSerreLikeSystem(
      calculator, input, output, true, false
    );
  }
  static bool solveSerreLikeSystemAlgebraic(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorFunctions::solveSerreLikeSystem(
      calculator, input, output, false, true
    );
  }
  static bool solveSerreLikeSystemAlgebraicUpperLimit(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorFunctions::solveSerreLikeSystem(
      calculator, input, output, true, true
    );
  }
  static bool fetchWebPagePOST(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool fetchWebPageGET(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool fetchKnownPublicKeys(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool elementEllipticCurveNormalForm(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool valueOfModularExpression(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool formatCPPSourceCode(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool formatCPPSourceCodeInternal(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    bool logDebugData
  );
  static bool formatCPPSourceCodeDebug(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool formatCPPDirectory(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool printZnEnumeration(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool notFunction(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool zModP(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool attemptExtendingEtoHEFwithHinCartan(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool conesIntersect(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool perturbSplittingNormal(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool printAllVectorPartitions(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool polynomialWithEWA(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorFunctions::elementWeylAlgebraOrPolynomial(
      calculator, input, output, true
    );
  }
  static bool elementWeylAlgebra(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorFunctions::elementWeylAlgebraOrPolynomial(
      calculator, input, output, false
    );
  }
  static bool elementWeylAlgebraOrPolynomial(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    bool assignPoly
  );
  static bool operationBinary(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    int operation
  );
  static bool interpolatePolynomial(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool times(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return
    CalculatorFunctions::operationBinary(
      calculator, input, output, calculator.opTimes()
    );
  }
  static bool reverseOrder(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool reverseOrderRecursively(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool transpose(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool generateMultiplicativelyClosedSet(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool determinantPolynomial(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool invertMatrixVerbose(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool suffixNotationForPostScript(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool isInteger(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool isRational(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool freudenthalFormula(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool freudenthalFull(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool jacobiSymbol(
    Calculator& calculator, const Expression& input, Expression& output
  );
  class Test {
  public:
    static bool checkSorting(const List<Expression>& mustBeSorted);
  };
};

#endif
