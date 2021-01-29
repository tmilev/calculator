// The current file is licensed under the license terms found in the main header file "calculator.h".
// For additional information refer to the file "calculator.h".
#ifndef vpfHeader6_2InnerFns
#define vpfHeader6_2InnerFns
#include "calculator_interface.h"

class CalculatorFunctionsCrypto {
public:
  static bool aes_cbc_256_encrypt(Calculator& calculator, const Expression& input, Expression& output);
  static bool aes_cbc_256_decrypt(Calculator& calculator, const Expression& input, Expression& output);
  static bool loadKnownCertificates(Calculator& calculator, const Expression& input, Expression& output);
  static bool testLoadPEMCertificates(Calculator& calculator, const Expression& input, Expression& output);
  static bool testLoadPEMPrivateKey(Calculator& calculator, const Expression& input, Expression& output);
  static bool testTLSMessageSequence(Calculator& calculator, const Expression& input, Expression& output);
  static bool testTLSDecodeSSLRecord(Calculator& calculator, const Expression& input, Expression& output);

  static bool jwtVerifyAgainstKnownKeys(Calculator& calculator, const Expression& input, Expression& output);
  static bool jwtVerifyAgainstRSA256(Calculator& calculator, const Expression& input, Expression& output);
  static bool RSAEncrypt(Calculator& calculator, const Expression& input, Expression& output);
  static bool RSASign(Calculator& calculator, const Expression& input, Expression& output);
  static bool showKnownObjectIds(Calculator& calculator, const Expression& input, Expression& output);
  static bool sha224OfString(Calculator& calculator, const Expression& input, Expression& output);
  static bool generateRandomPrime(Calculator& calculator, const Expression& input, Expression& output);
  static bool testRSASign(Calculator& calculator, const Expression& input, Expression& output);
  static bool sha256OfString(Calculator& calculator, const Expression& input, Expression& output);
  static bool sha512(Calculator& calculator, const Expression& input, Expression& output);
  static bool ripemd160OfString(Calculator& calculator, const Expression& input, Expression& output);
  static bool sha3_256OfString(Calculator& calculator, const Expression& input, Expression& output);
  static bool keccak256OfString(Calculator& calculator, const Expression& input, Expression& output);
  static bool sha256OfStringVerbose(Calculator& calculator, const Expression& input, Expression& output);
  static bool sha1OfString(Calculator& calculator, const Expression& input, Expression& output);
  static bool functionHashString(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    const std::string& hashId,
    bool verbose
  );
  static bool testASN1Decode(Calculator& calculator, const Expression& input, Expression& output);
  static bool x509CertificateServer(Calculator& calculator, const Expression& input, Expression& output);
  static bool x509CertificateDecode(Calculator& calculator, const Expression& input, Expression& output);
  static bool appendDoubleSha256Check(Calculator& calculator, const Expression& input, Expression& output);
};

class CalculatorFunctionsEncoding {
public:
  static bool convertBase64ToString(Calculator& calculator, const Expression& input, Expression& output);
  static bool convertStringToHex(Calculator& calculator, const Expression& input, Expression& output);
  static bool convertCharToBase64(Calculator& calculator, const Expression& input, Expression& output);

  static bool testJSON(Calculator& calculator, const Expression& input, Expression& output);
  static bool convertBase64ToHex(Calculator& calculator, const Expression& input, Expression& output );
  static bool convertIntegerToHex(Calculator& calculator, const Expression& input, Expression& output);
  static bool convertHexToInteger(Calculator& calculator, const Expression& input, Expression& output);
  static bool convertHexToString(Calculator& calculator, const Expression& input, Expression& output);
  static bool convertBase58ToHex(Calculator& calculator, const Expression& input, Expression& output);
  static bool convertHexToBase58(Calculator& calculator, const Expression& input, Expression& output);
  static bool convertIntegerUnsignedToBase58(Calculator& calculator, const Expression& input, Expression& output);
  static bool base64ToCharToBase64Test(Calculator& calculator, const Expression& input, Expression& output);

};

class CalculatorFunctionsPlot {
public:
  static bool plotCoordinateSystem(Calculator& calculator, const Expression& input, Expression& output);
  static bool plotSetProjectionScreenBasis(Calculator& calculator, const Expression& input, Expression& output);

  static bool plotSurface(Calculator& calculator, const Expression& input, Expression& output);
  static bool plotImplicitFunctionFull(Calculator& calculator, const Expression& input, Expression& output, bool showGrid);
  static bool plotImplicitFunction(Calculator& calculator, const Expression& input, Expression& output);
  static bool plotImplicitShowGridFunction(Calculator& calculator, const Expression& input, Expression& output);
  static bool plotSegment(Calculator& calculator, const Expression& input, Expression& output);
  static bool plotMarkSegment(Calculator& calculator, const Expression& input, Expression& output);
  static bool plotPath(Calculator& calculator, const Expression& input, Expression& output);
  static bool plotParametricCurve(Calculator& calculator, const Expression& input, Expression& output);
  static bool plotDirectionField(Calculator& calculator, const Expression& input, Expression& output);
  static bool plotVectorField(Calculator& calculator, const Expression& input, Expression& output);
  static bool plotDirectionOrVectorField(
    Calculator& calculator, const Expression& input, Expression& output, bool vectorsAreNormalized
  );
  static bool plotPolarRfunctionThetaExtended(Calculator& calculator, const Expression& input, Expression& output);
  static bool plotPolarRfunctionTheta(Calculator& calculator, const Expression& input, Expression& output);
  static bool isPlot(Calculator& calculator, const Expression& input, Expression& output);
  static bool plot2D(Calculator& calculator, const Expression& input, Expression& output);
  static bool plot2DOverIntervals(Calculator& calculator, const Expression& input, Expression& output);
  static bool plotFill(Calculator& calculator, const Expression& input, Expression& output);
  static bool plotRectangle(Calculator& calculator, const Expression& input, Expression& output);
  static bool plotRemoveCoordinateAxes(Calculator& calculator, const Expression& input, Expression& output);
  static bool plotGrid(Calculator& calculator, const Expression& input, Expression& output);
  static bool plotLabel(Calculator& calculator, const Expression& input, Expression& output);
  static bool plotSetId(Calculator& calculator, const Expression& input, Expression& output);
  static bool plotViewRectangle(Calculator& calculator, const Expression& input, Expression& output);
  static bool plotViewWindow(Calculator& calculator, const Expression& input, Expression& output);
  static bool plotPoint(Calculator& calculator, const Expression& input, Expression& output);
  static bool plot2DWithBars(Calculator& calculator, const Expression& input, Expression& output);

  static bool drawExpressionGraphWithOptions(
    Calculator& calculator, const Expression& input, Expression& output, bool useFullTree
  );
  static bool drawExpressionGraph(Calculator& calculator, const Expression& input, Expression& output) {
    return CalculatorFunctionsPlot::drawExpressionGraphWithOptions(calculator, input, output, false);
  }
  static bool drawExpressionGraphFull(Calculator& calculator, const Expression& input, Expression& output) {
    return CalculatorFunctionsPlot::drawExpressionGraphWithOptions(calculator, input, output, true);
  }

};

class CalculatorFunctionsTrigonometry {
public:
  static bool sin(Calculator& calculator, const Expression& input, Expression& output);
  static bool cos(Calculator& calculator, const Expression& input, Expression& output);
  static bool sec(Calculator& calculator, const Expression& input, Expression& output);
  static bool csc(Calculator& calculator, const Expression& input, Expression& output);
  static bool tan(Calculator& calculator, const Expression& input, Expression& output);
  static bool cotangent(Calculator& calculator, const Expression& input, Expression& output);
  static bool arcsinAlgebraic(Calculator& calculator, const Expression& input, Expression& output);
  static bool arccosAlgebraic(Calculator& calculator, const Expression& input, Expression& output);
  static bool arcsin(Calculator& calculator, const Expression& input, Expression& output);
  static bool arccos(Calculator& calculator, const Expression& input, Expression& output);
  static bool arctan(Calculator& calculator, const Expression& input, Expression& output);
  static bool exploitCosineEvenness(Calculator& calculator, const Expression& input, Expression& output);
  static bool exploitSineOddness(Calculator& calculator, const Expression& input, Expression& output);

  static bool trigonometricSumToTrigonometricProduct(Calculator& calculator, const Expression& input, Expression& output);
  static bool sineOfAngleSumToTrigonometry(Calculator& calculator, const Expression& input, Expression& output);
  static bool cosineOfAngleSumToTrigonometry(Calculator& calculator, const Expression& input, Expression& output);
  static bool convertSineToExponent(Calculator& calculator, const Expression& input, Expression& output);
  static bool convertCosineToExponent(Calculator& calculator, const Expression& input, Expression& output);
  static bool eulerFormulaAsLaw(Calculator& calculator, const Expression& input, Expression& output);

};

class CalculatorFunctionsIntegration {
public:
  static bool integrateRationalFunctionSplitToBuidingBlocks(Calculator& calculator, const Expression& input, Expression& output);
  static bool integrateRationalFunctionBuidingBlockIa(Calculator& calculator, const Expression& input, Expression& output);
  static bool integrateRationalFunctionBuidingBlockIb(Calculator& calculator, const Expression& input, Expression& output);
  static bool integrateRationalFunctionBuidingBlockIIaandIIIa(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool integrateRationalFunctionBuidingBlockIIb(Calculator& calculator, const Expression& input, Expression& output);
  static bool integrateRationalFunctionBuidingBlockIIIb(Calculator& calculator, const Expression& input, Expression& output);
  static bool integrateDefiniteIntegral(Calculator& calculator, const Expression& input, Expression& output);
  static bool integratePowerByUncoveringParenthesisFirst(Calculator& calculator, const Expression& input, Expression& output);
  static bool integrateSum(Calculator& calculator, const Expression& input, Expression& output);
  static bool integratePullImaginaryUnit(Calculator& calculator, const Expression& input, Expression& output);
  static bool integrateXnDiffX(Calculator& calculator, const Expression& input, Expression& output);
  static bool integratePullConstant(Calculator& calculator, const Expression& input, Expression& output);
  static bool integrateSqrtOneMinusXsquared(Calculator& calculator, const Expression& input, Expression& output);
  static bool integrateSqrtXsquaredMinusOne(Calculator& calculator, const Expression& input, Expression& output);
  static bool integrateXpowerNePowerAx(Calculator& calculator, const Expression& input, Expression& output);

  static bool integrateEpowerAxDiffX(Calculator& calculator, const Expression& input, Expression& output);
  static bool integrateSinPowerNCosPowerM(Calculator& calculator, const Expression& input, Expression& output);
  static bool integrateTanPowerNSecPowerM(Calculator& calculator, const Expression& input, Expression& output);
  static bool integralOperator(Calculator& calculator, const Expression& input, Expression& output);
};

class CalculatorFunctionsDifferentiation {
public:
  static bool differentiateTrigAndInverseTrig(Calculator& calculator, const Expression& input, Expression& output);
  static bool differentiateChainRule(Calculator& calculator, const Expression& input, Expression& output);
  static bool differentiateAplusB(Calculator& calculator, const Expression& input, Expression& output);
  static bool differentiateAtimesB(Calculator& calculator, const Expression& input, Expression& output);
  static bool differentiateAdivideBCommutative(Calculator& calculator, const Expression& input, Expression& output);
  static bool differentiateAdivideBNONCommutative(Calculator& calculator, const Expression& input, Expression& output);
  static bool differentiateConstant(Calculator& calculator, const Expression& input, Expression& output);
  static bool differentiateX(Calculator& calculator, const Expression& input, Expression& output);
  static bool differentiateConstPower(Calculator& calculator, const Expression& input, Expression& output);
  static bool differentiateAPowerB(Calculator& calculator, const Expression& input, Expression& output);
  static bool diffdivDiffxToDifferentiation(Calculator& calculator, const Expression& input, Expression& output);
  static bool ddivDxToDiffDivDiffx(Calculator& calculator, const Expression& input, Expression& output);

  static bool interpretAsDifferential(Calculator& calculator, const Expression& input, Expression& output);
  static bool differentiateSqrt(Calculator& calculator, const Expression& input, Expression& output);
  static bool differentiateWithRespectToXTimesAny(Calculator& calculator, const Expression& input, Expression& output);
};

class CalculatorFunctionsAlgebraic {
public:
  static bool convertAlgebraicNumberToMatrix(Calculator& calculator, const Expression& input, Expression& output);
  static bool getAlgebraicNumberFromMinPoly(Calculator& calculator, const Expression& input, Expression& output);
  static bool printAlgebraicClosureStatus(Calculator& calculator, const Expression& input, Expression& output);
  static bool isAlgebraicRadical(Calculator& calculator, const Expression& input, Expression& output);
};

class CalculatorFunctionsBasic {
public:
  static bool factorial(Calculator& calculator, const Expression& input, Expression& output);
  static bool logarithmBaseNaturalToLn(Calculator& calculator, const Expression& input, Expression& output);

  static bool logarithmBaseSimpleCases(Calculator& calculator, const Expression& input, Expression& output);
  static bool logarithmBase(Calculator& calculator, const Expression& input, Expression& output);
  static bool logarithm(Calculator& calculator, const Expression& input, Expression& output);
  static bool absoluteValue(Calculator& calculator, const Expression& input, Expression& output);

  static bool floor(Calculator& calculator, const Expression& input, Expression& output);
  static bool round(Calculator& calculator, const Expression& input, Expression& output);
  static bool logarithmBaseNCeiling(Calculator& calculator, const Expression& input, Expression& output);
};

class CalculatorFunctionsListsAndSets {
public:
  static bool intersection(Calculator& calculator, const Expression& input, Expression& output);
  static bool listUnion(Calculator& calculator, const Expression& input, Expression& output);
  static bool unionNoRepetition(Calculator& calculator, const Expression& input, Expression& output);
  static bool length(Calculator& calculator, const Expression& input, Expression& output);
  static bool intersectUnion(Calculator& calculator, const Expression& input, Expression& output);
  static bool unionEmptySet(Calculator& calculator, const Expression& input, Expression& output);
  static bool lastElement(Calculator& calculator, const Expression& input, Expression& output);
  static bool removeLastElement(Calculator& calculator, const Expression& input, Expression& output);
  static bool belongsTo(Calculator& calculator, const Expression& input, Expression& output);
};

class CalculatorFunctionsIntervals {
public:
  static bool unionUnionIntervals(Calculator& calculator, const Expression& input, Expression& output);
  static bool unionIntervals(Calculator& calculator, const Expression& input, Expression& output);
  static bool intersectIntervals(Calculator& calculator, const Expression& input, Expression& output);
  static bool normalizeIntervals(Calculator& calculator, const Expression& input, Expression& output);
};

class CalculatorFunctionsFreecalc {
public:
  static bool buildFreecalc(Calculator& calculator, const Expression& input, Expression& output);
  static bool buildFreecalcSingleSlides(Calculator& calculator, const Expression& input, Expression& output);
  static bool buildFreecalcSlidesOnTopic(Calculator& calculator, const Expression& input, Expression& output);
  static bool crawlTexFile(Calculator& calculator, const Expression& input, Expression& output);
};

class CalculatorFunctionsLinearAlgebra {
public:
  static bool minimalPolynomialMatrix(Calculator& calculator, const Expression& input, Expression& output);
  static bool characteristicPolynomialMatrix(Calculator& calculator, const Expression& input, Expression& output);
  static bool functionToMatrix(Calculator& calculator, const Expression& input, Expression& output);
};

class CalculatorFunctions {
public:
  static bool leftIntervalGreaterThanRight(const Expression& left, const Expression& right);
  static bool printRuleStack(Calculator& calculator, const Expression& input, Expression& output);
  static bool crash(Calculator& calculator, const Expression& input, Expression& output);
  static bool crashByListOutOfBounds(Calculator& calculator, const Expression& input, Expression& output);
  static bool crashByVectorOutOfBounds(Calculator& calculator, const Expression& input, Expression& output);
  static bool testProblemInterpretation(Calculator& calculator, const Expression& input, Expression& output);
  static bool testTopicListProblems(Calculator& calculator, const Expression& input, Expression& output);
  static bool precomputeSemisimpleLieAlgebraStructure(Calculator& calculator, const Expression& input, Expression& output);

  static bool turnRulesOnOff(Calculator& calculator, const Expression& input, Expression& output, bool turnOff);

  static bool turnOffRules(Calculator& calculator, const Expression& input, Expression& output);
  static bool turnOnRules(Calculator& calculator, const Expression& input, Expression& output);

  static bool turnOnApproximations(Calculator& calculator, const Expression& input, Expression& output);
  static bool turnOffApproximations(Calculator& calculator, const Expression& input, Expression& output);

  static bool equalityToArithmeticExpression(Calculator& calculator, const Expression& input, Expression& output);
  static bool functionEqualityToArithmeticExpression(Calculator& calculator, const Expression& input, Expression& output);

  static bool randomInteger(Calculator& calculator, const Expression& input, Expression& output);
  static bool selectAtRandom(Calculator& calculator, const Expression& input, Expression& output);
  static bool setRandomSeed(Calculator& calculator, const Expression& input, Expression& output);

  static bool convertElementZmodPToInteger(Calculator& calculator, const Expression& input, Expression& output);
  static bool urlStringToNormalString(Calculator& calculator, const Expression& input, Expression& output);
  static bool stringToURL(Calculator& calculator, const Expression& input, Expression& output);
  static bool urlKeyValuePairsToNormalRecursive(Calculator& calculator, const Expression& input, Expression& output);

  static bool stringToAtom(Calculator& calculator, const Expression& input, Expression& output);
  static bool expressionToString(Calculator& calculator, const Expression& input, Expression& output);
  static bool expressiontoUTF8String(Calculator& calculator, const Expression& input, Expression& output);
  static bool quoteToString(Calculator& calculator, const Expression& input, Expression& output);

  static bool sliceString(Calculator& calculator, const Expression& input, Expression& output);
  static bool stringDifference(Calculator& calculator, const Expression& input, Expression& output);
  static bool nistEllipticCurveGenerator(Calculator& calculator, const Expression& input, Expression& output);
  static bool nistEllipticCurveOrder(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerCrossProduct(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerDistributeExponent(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDistributeSqrt(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerFourierTransformEWA(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSqrt(Calculator& calculator, const Expression& input, Expression& output);

  static bool factorInteger(Calculator& calculator, const Expression& input, Expression& output);
  static bool functionFactorInteger(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerFactorOutNumberContent(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerIsProductLinearOrConstantTermsIn(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIsProductTermsUpToPower(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIsLinearOrConstantIn(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerCoefficientOf(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerConstTermRelative(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerCompositeArithmeticOperationEvaluatedOnArgument(
    Calculator& calculator, const Expression& input, Expression& output
  );

  static bool innerMatchesPattern(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerThaw(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerGetFreeVariablesIncludeNamedConstants(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerGetFreeVariablesExcludeNamedConstants(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerCompareFunctionsNumerically(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerCompareExpressionsNumerically(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerCompareExpressionsNumericallyAtPoints(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerEnsureExpressionDependsOnlyOnMandatoryVariables(
    Calculator& calculator, const Expression& input, Expression& output, bool excludeNamedConstants
  );
  static bool innerEnsureExpressionDependsOnlyOnMandatoryVariablesExcludeNamedConstants(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return CalculatorFunctions::innerEnsureExpressionDependsOnlyOnMandatoryVariables(calculator, input, output, true);
  }
  static bool innerEnsureExpressionDependsOnlyOnMandatoryVariablesIncludeNamedConstants(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return CalculatorFunctions::innerEnsureExpressionDependsOnlyOnMandatoryVariables(calculator, input, output, false);
  }
  static bool innerEnsureExpressionDependsOnlyOnStandard(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerMultiplySequence(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSumSequence(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSumTimesExpressionToSumOf(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSumAsOperatorToSumInternalNotation(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerHandleUnderscorePowerLimits(Calculator& calculator, const Expression& input, Expression& output);
  static bool outerPolynomialize(Calculator& calculator, const Expression& input, Expression& output);
  static bool functionPolynomialize(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerApplyToSubexpressionsRecurseThroughCalculusFunctions(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool innerSubList(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerApplyToList(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerNumerator(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDenominator(Calculator& calculator, const Expression& input, Expression& output);


  static bool innerDereferenceInterval(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDereferenceSequenceOrMatrix(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDereferenceSequenceStatements(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerCompositeEWAactOnPoly(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerCompositeConstTimesAnyActOn(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerCompositeApowerBevaluatedAtC(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerConstantFunction(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerRationalFunctionSubstitution(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerGetPointsImplicitly(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerCompositeDifferentiateLog(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerOperatorBounds(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPowerAnyToZero(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPowerExponentToLog(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerNewtonsMethod(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerCompareIntervalsNumerically(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIntersectEmptySet(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerGetFirstSummandContaining(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerGetSummand(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerCollectMultiplicands(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerCollectOpands(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerCollectSummands(Calculator& calculator, const Expression& input, Expression& output);
  static bool outerMergeConstantRadicals(Calculator& calculator, const Expression& input, Expression& output);
  static bool outerCommuteConstants(Calculator& calculator, const Expression& input, Expression& output);
  static bool outerDivideReplaceAdivBpowerItimesBpowerJ(Calculator& calculator, const Expression& input, Expression& output);
  static bool outerAtimesBpowerJplusEtcDivBpowerI(Calculator& calculator, const Expression& input, Expression& output);

  static bool combineFractionsCommutative(Calculator& calculator, const Expression& input, Expression& output);
  static bool sortTerms(Calculator& calculator, const Expression& input, Expression& output);
  static bool addTerms(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerIsPrimeMillerRabin(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIsPossiblyPrime(Calculator& calculator, const Expression& input, Expression& output);
  static bool leastCommonMultipleInteger(Calculator& calculator, const Expression& input, Expression& output);
  static bool greatestCommonDivisorInteger(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerIsNilpotent(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerInvertMatrixRFsVerbose(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerInvertMatrix(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDFQsEulersMethod(Calculator& calculator, const Expression& input, Expression& output);


  static bool innerScaleToLeadingUnit(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerRemoveDuplicates(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSort(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSortDescending(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerGetUserDefinedSubExpressions(Calculator& calculator, const Expression& input, Expression& output);

  static bool divideByNumber(Calculator& calculator, const Expression& input, Expression& output);
  static bool outerDivCancellations(Calculator& calculator, const Expression& input, Expression& output);
  static bool outerAssociateDivisionDivision(Calculator& calculator, const Expression& input, Expression& output);
  static bool outerAssociateAdivBdivCpowerD(Calculator& calculator, const Expression& input, Expression& output);
  static bool outerEqualEqual(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerEqualEqualEqual(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMax(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerMin(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerLessThanOrEqualTo(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerGreaterThanOrEqualTo(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerGreaterThan(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerLessThan(Calculator& calculator, const Expression& input, Expression& output);

  static bool outerCommuteAtimesBifUnivariate(Calculator& calculator, const Expression& input, Expression& output);
  static bool outerCommuteAtimesBtimesCifUnivariate(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDifferentialStandardHandler(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerDifferentialOfPolynomial(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerIsDifferentialOneFormOneVariable(Calculator& calculator, const Expression& input, Expression& output);
  static bool extractQuadraticCoeffsWRTvariable(
    const Expression& theQuadratic,
    const Expression& theVariable,
    Expression& outputCoeffVarSquared,
    Expression& outputCoeffLinTerm,
    Expression& outputConstTerm
  );
  static bool extractLinearCoeffsWRTvariable(
    const Expression& theLinearExpression,
    const Expression& theVariable,
    Expression& outputCoeffLinTerm,
    Expression& outputConstTerm
  );

  static bool innerPowerImaginaryUnit(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerTrace(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerReverseBytes(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerIfStandard(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIfFrozen(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerOr(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerOrIdentical(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerAnd(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerSolveUnivariatePolynomialWithRadicalsWRT(
    Calculator& calculator, const Expression& input, Expression& output
  );

  static bool innerMakeJavascriptExpressioN(Calculator& calculator, const Expression& input, Expression& output);
  static bool functionMakeJavascriptExpression(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerContains(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerExpressionLeafs(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerLispify(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerLispifyFull(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerChildExpression(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerDegreesToRadians(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerEvaluateToDoublE(Calculator& calculator, const Expression& input, Expression& output);
  static bool functionEvaluateToDouble(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerEmbedSemisimpleAlgebraInSemisimpleAlgebra(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerAllPartitions(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerAllVectorPartitions(Calculator& calculator, const Expression& input, Expression& output);

  template<class Coefficient>
  static bool functionDeterminant(
    Calculator& calculator,
    const Expression& input,
    Expression& output,
    Expression::FunctionAddress conversionFunction,
    int maxiumDimension
  );

  static bool innerDeterminant(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerHighestWeightTransposeAntiAutomorphismBilinearForm(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerCoefficientsPowersOf(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIntervalOpenFromSequence(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIntervalClosedFromSequence(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIntervalLeftClosedFromSequence(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIntervalRightClosedFromSequence(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIsNonEmptySequence(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerIsSquare(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIsSquareFree(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIsSquareFreePolynomial(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerIsPower(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIsEven(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerIsReal(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIsConstant(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerGaussianEliminationMatrix(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSplitToPartialFractionsOverAlgebraicRealsAlgorithm(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool innerSplitToPartialFractionsOverAlgebraicRealS(Calculator& calculator, const Expression& input, Expression& output);
  static bool functionSplitToPartialFractionsOverAlgebraicReals(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerTestTopCommand(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerTestIndicator(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerFindProductDistanceModN(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSolveProductSumEquationOverSetModN(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerSolveSerreLikeSystem(
    Calculator& calculator, const Expression& input, Expression& output, bool useUpperLimit, bool startWithAlgebraicClosure
  );
  static bool innerSolveSerreLikeSystemNoUpperLimit(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return CalculatorFunctions::innerSolveSerreLikeSystem(calculator, input, output, false, false);
  }
  static bool innerSolveSerreLikeSystemUpperLimit(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return CalculatorFunctions::innerSolveSerreLikeSystem(calculator, input, output, true, false);
  }
  static bool innerSolveSerreLikeSystemAlgebraic(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return CalculatorFunctions::innerSolveSerreLikeSystem(calculator, input, output, false, true);
  }
  static bool innerSolveSerreLikeSystemAlgebraicUpperLimit(
    Calculator& calculator, const Expression& input, Expression& output
  ) {
    return CalculatorFunctions::innerSolveSerreLikeSystem(calculator, input, output, true, true);
  }
  static bool innerFetchWebPagePOST(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerFetchWebPageGET(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerFetchKnownPublicKeys(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSendEmailWithMailGun(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerElementEllipticCurveNormalForm(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool innerValueOfModularExpression(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool innerFormatCPPSourceCode(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool innerformatCPPDirectory(
    Calculator& calculator, const Expression& input, Expression& output
  );

  static bool innerPrintZnEnumeration(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerNot(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerZmodP(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerAttemptExtendingEtoHEFwithHinCartan(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool innerAdCommonEigenSpaces(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerConesIntersect(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPerturbSplittingNormal(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerPrintAllVectorPartitions(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerPolynomialWithEWA(Calculator& calculator, const Expression& input, Expression& output) {
    return CalculatorFunctions::innerEWAorPoly(calculator, input, output, true);
  }
  static bool innerElementWeylAlgebra(Calculator& calculator, const Expression& input, Expression& output) {
    return CalculatorFunctions::innerEWAorPoly(calculator, input, output, false);
  }
  static bool innerEWAorPoly(Calculator& calculator, const Expression& input, Expression& output, bool assignPoly);
  static bool innerOperationBinary(Calculator& calculator, const Expression& input, Expression& output, int theOp);
  static bool innerInterpolatePoly(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerTimes(Calculator& calculator, const Expression& input, Expression& output) {
    return CalculatorFunctions::innerOperationBinary(calculator, input, output, calculator.opTimes());
  }
  static bool innerReverseOrder(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerReverseOrderRecursively(Calculator& calculator, const Expression& input, Expression& output);
  static bool transpose(Calculator& calculator, const Expression& input, Expression& output);

  static bool innerGenerateMultiplicativelyClosedSet(
    Calculator& calculator, const Expression& input, Expression& output
  );
  static bool innerDeterminantPolynomial(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerInvertMatrixVerbose(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerSuffixNotationForPostScript(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIsInteger(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerIsRational(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerFreudenthalFormula(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerFreudenthalFull(Calculator& calculator, const Expression& input, Expression& output);
  static bool innerJacobiSymbol(Calculator& calculator, const Expression& input, Expression& output);
  class Test{
  public:
    static bool checkSorting(const List<Expression>& mustBeSorted);
  };
};
#endif

