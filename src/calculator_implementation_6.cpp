#include "calculator.h"
#include "calculator_inner_functions.h"
#include "calculator_inner_typed_functions.h"
#include "calculator_html_functions.h"
#include "web_api.h"
#include "math_general_implementation.h"
#include "calculator_html_interpretation.h"
#include "math_extra_drawing_variables.h"
#include "math_general_polynomial_computations_advanced_implementation.h"
#include "crypto.h"
#include "abstract_syntax_notation_one_decoder.h"
#include "math_extra_elliptic_curves_implementation.h"
#include "transport_layer_security.h"
#include "string_constants.h"
#include <iomanip>
#include "math_extra_semisimple_lie_algebras_implementation.h"
#include "math_rational_function_implementation.h"

std::string CalculatorHTML::toStringLinkFromProblem(
  const std::string& fileName, bool practiceMode, int randomSeed
) {
  JSData request;
  request[WebAPI::problem::fileName] = fileName;
  request[WebAPI::frontend::problemFileName] = fileName;
  std::stringstream randomSeedStream;
  randomSeedStream << randomSeed;
  request[WebAPI::problem::randomSeed] = randomSeedStream.str();
  request[WebAPI::problem::fileName] = fileName;
  request[WebAPI::frontend::currentPage] = WebAPI::frontend::problemPage;
  if (practiceMode) {
    request[WebAPI::frontend::exerciseType] = WebAPI::frontend::exercise;
  } else {
    request[WebAPI::frontend::exerciseType] = WebAPI::frontend::scoredQuiz;
  }
  std::stringstream out;
  out
  << "<a href='"
  << global.displayApplicationNoCache
  << "#"
  << request.toString()
  << "'>"
  << fileName
  << "</a>";
  return out.str();
}

bool CalculatorFunctions::testTopicListProblems(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::testTopicListProblems");
  if (!global.userDefaultHasAdminRights()) {
    return
    calculator
    << "Topic list tests available to logged-in admins only. ";
  }
  (void) input;
  TopicElementParser::Test tester;
  tester.defaultTopicListsOK();
  return output.assignValue(calculator, tester.comments.str());
}

bool CalculatorFunctions::testProblemInterpretation(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::testProblemInterpretation");
  if (!global.userDefaultHasAdminRights()) {
    return
    calculator
    << "Automated tests available to logged-in admins only. ";
  }
  if (input.size() != 4) {
    return
    calculator
    << "I expected two arguments: "
    << "1) index of first problem to test, where "
    << "0 = start at beginning, 1 = start at second problem, etc.; "
    << "2) number of problems to test (0 or less = test all); "
    << "3) starting random seed, set to 0 if you don't know what this is. ";
  }
  if (global.response.monitoringAllowed()) {
    global.response.initiate("Triggered by testProblemInterpretation.");
  }
  int desiredNumberOfTests = 0;
  int firstFileIndex = 0;
  int randomSeed = 0;
  input[1].isSmallInteger(&firstFileIndex);
  input[2].isSmallInteger(&desiredNumberOfTests);
  input[3].isSmallInteger(&randomSeed);
  std::stringstream comments;
  CalculatorHTML::Test::builtInMultiple(
    firstFileIndex, desiredNumberOfTests, randomSeed, 3, &comments
  );
  return output.assignValue(calculator, comments.str());
}

bool CalculatorFunctions::intervalClosedFromSequence(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::intervalClosedFromSequence");
  if (input.size() != 2) {
    return false;
  }
  if (!input[1].isSequenceNElements(2)) {
    return false;
  }
  output = input[1];
  return output.setChildAtomValue(0, calculator.opIntervalClosed());
}

bool CalculatorFunctions::intervalOpenFromSequence(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::intervalOpenFromSequence");
  if (input.size() != 2) {
    return false;
  }
  if (!input[1].isSequenceNElements(2)) {
    return false;
  }
  output = input[1];
  return output.setChildAtomValue(0, calculator.opIntervalOpen());
}

bool CalculatorFunctions::intervalRightClosedFromSequence(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::intervalRightClosedFromSequence");
  if (input.size() != 2) {
    return false;
  }
  if (!input[1].isSequenceNElements()) {
    return false;
  }
  output = input[1];
  return output.setChildAtomValue(0, calculator.opIntervalRightClosed());
}

bool CalculatorFunctions::intervalLeftClosedFromSequence(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::intervalLeftClosedFromSequence");
  if (input.size() != 2) {
    return false;
  }
  if (!input[1].isSequenceNElements()) {
    return false;
  }
  output = input[1];
  return output.setChildAtomValue(0, calculator.opIntervalLeftClosed());
}

bool CalculatorFunctions::getFirstSummandContaining(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::getFirstSummandContaining");
  if (!input.startsWithGivenOperation("GetFirstSummandContaining", 3)) {
    return false;
  }
  List<Expression> summands;
  calculator.collectOpands(input[1], calculator.opPlus(), summands);
  for (int i = 0; i < summands.size; i ++) {
    if (summands[i].containsAsSubExpressionNoBuiltInTypes(input[2])) {
      output = summands[i];
      return true;
    }
  }
  return false;
}

bool CalculatorFunctions::getSummand(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::getSummand");
  if (!input.startsWithGivenOperation("GetSummand", 3)) {
    return false;
  }
  const Expression& expression = input[1];
  if (expression.startsWith(calculator.opPlus(), 3)) {
    int summandIndex = - 1;
    if (!input[2].isSmallInteger(&summandIndex)) {
      return false;
    }
    if (summandIndex < 0) {
      return false;
    }
    List<Expression> summands;
    List<Expression> sums;
    calculator.collectOpands(expression, calculator.opPlus(), summands);
    for (int i = 0; i < summands.size; i ++) {
      if (
        summands[i].containsAsSubExpressionNoBuiltInTypes(
          calculator.opSum()
        )
      ) {
        sums.addOnTop(summands[i]);
        summands.removeIndexShiftDown(i);
        i --;
      }
    }
    if (sums.size > 1) {
      return false;
    }
    if (summandIndex < summands.size) {
      output = summands[summandIndex];
      return true;
    }
    if (sums.size == 0) {
      return false;
    }
    output.reset(calculator);
    output.addChildAtomOnTop("GetSummand");
    output.addChildOnTop(sums[0]);
    Expression shiftE;
    shiftE.assignValue(calculator, summands.size);
    return output.addChildOnTop(input[2] - shiftE);
  }
  List<Expression> multiplicands;
  expression.getMultiplicandsRecursive(multiplicands);
  Expression sumExpression = *multiplicands.lastObject();
  multiplicands.removeLastObject();
  Expression coeffExpression;
  if (multiplicands.size > 0) {
    coeffExpression.makeProduct(calculator, multiplicands);
  } else {
    coeffExpression.assignValue(calculator, 1);
  }
  if (!sumExpression.startsWith(calculator.opSum(), 3)) {
    return false;
  }
  const Expression& limitExpressions = sumExpression[1];
  if (!limitExpressions.startsWith(calculator.opLimitBoundary(), 3)) {
    return false;
  }
  const Expression& bottomBoundary = limitExpressions[1];
  if (!bottomBoundary.startsWith(calculator.opDefine(), 3)) {
    return false;
  }
  Expression substitution = bottomBoundary;
  Expression // oneE,
  valueToSubWith;
  valueToSubWith = bottomBoundary[2] + input[2];
  substitution.setChild(2, valueToSubWith);
  Expression commandSequence(calculator);
  commandSequence.addChildAtomOnTop(calculator.opCommandSequence());
  commandSequence.addChildOnTop(substitution);
  commandSequence.addChildOnTop(coeffExpression * sumExpression[2]);
  return
  output.makeXOX(
    calculator,
    calculator.opUnderscore(),
    commandSequence,
    calculator.expressionTwo()
  );
}

bool CalculatorFunctionsPlot::plotVectorField(
  Calculator& calculator, const Expression& input, Expression& output
) {
  return
  CalculatorFunctionsPlot::plotDirectionOrVectorField(
    calculator, input, output, false
  );
}

bool CalculatorFunctionsPlot::plotDirectionField(
  Calculator& calculator, const Expression& input, Expression& output
) {
  return
  CalculatorFunctionsPlot::plotDirectionOrVectorField(
    calculator, input, output, true
  );
}

bool CalculatorFunctionsPlot::plotDirectionOrVectorField(
  Calculator& calculator,
  const Expression& input,
  Expression& output,
  bool vectorsAreNormalized
) {
  STACK_TRACE("CalculatorFunctionsPlot::plotDirectionOrVectorField");
  (void) vectorsAreNormalized;
  if (input.size() < 5) {
    return
    output.assignError(
      calculator,
      "Vector fields take at least four arguments: the vector field, "
      "the low left corner, the upper right, and the "
      "number of segments in each direction. "
    );
  }
  if (input.hasBoundVariables()) {
    return false;
  }
  Plot plot;
  plot.dimension = 2;
  PlotObject plotObject;
  plotObject.leftPoint = input[2];
  plotObject.rightPoint = input[3];
  if (input.size() >= 7) {
    if (!input[6].isOfType<std::string>(&plotObject.colorJS)) {
      plotObject.colorJS = input[6].toString();
    }
  } else {
    plotObject.colorJS = "blue";
  }
  plotObject.colorRGB = static_cast<int>(
    HtmlRoutines::redGreenBlue(0, 0, 255)
  );
  DrawingVariables::getColorIntFromColorString(
    plotObject.colorJS, plotObject.colorRGB
  );
  plotObject.lineWidth = 1;
  if (input.size() >= 8) {
    input[7].evaluatesToDouble(&plotObject.lineWidth);
  }
  Vector<double> lowLeft, upRight;
  if (!calculator.getVectorDoubles(input[2], lowLeft, 2)) {
    return
    calculator
    << "Failed to low left corner from: "
    << input[2].toString();
  }
  if (!calculator.getVectorDoubles(input[3], upRight, 2)) {
    return
    calculator
    << "Failed to up right corner from: "
    << input[3].toString();
  }
  plotObject.yHigh = upRight[1];
  plotObject.yLow = lowLeft[1];
  plotObject.xHigh = upRight[0];
  plotObject.xLow = lowLeft[0];
  List<std::string> lowLeftStrings, upRightStrings;
  lowLeft.toListStringsBasicType(lowLeftStrings);
  upRight.toListStringsBasicType(upRightStrings);
  plotObject.variableRangesJS.setSize(2);
  plotObject.variableRangesJS[0].setSize(2);
  plotObject.variableRangesJS[1].setSize(2);
  plotObject.variableRangesJS[0][0] = lowLeftStrings[0];
  plotObject.variableRangesJS[0][1] = upRightStrings[0];
  plotObject.variableRangesJS[1][0] = lowLeftStrings[1];
  plotObject.variableRangesJS[1][1] = upRightStrings[1];
  plotObject.manifoldImmersion = input[1];
  Expression jsConverterE;
  JavascriptExtractor extractor(calculator);
  if (input.size() >= 6) {
    if (
      !extractor.extract(
        input[5], plotObject.defaultLengthJS, &calculator.comments
      )
    ) {
      return
      calculator
      << "<br>Failed to extract javascript from "
      << input[5].toString()
      << ". ";
    }
  }
  if (
    !extractor.extract(
      plotObject.manifoldImmersion,
      plotObject.manifoldImmersionJS,
      &calculator.comments
    )
  ) {
    return
    calculator
    << "<br>Failed to extract javascript from "
    << input[1].toString()
    << ". ";
  }
  plotObject.manifoldImmersion.getFreeVariables(
    plotObject.variablesInPlay, true
  );
  Expression xE, yE;
  xE.makeAtom(calculator, "x");
  yE.makeAtom(calculator, "y");
  if (plotObject.variablesInPlay.size == 0) {
    plotObject.variablesInPlay.addOnTop(xE);
  }
  if (plotObject.variablesInPlay.size == 1) {
    if (plotObject.variablesInPlay.contains(xE)) {
      plotObject.variablesInPlay.addOnTop(yE);
    } else {
      plotObject.variablesInPlay.addOnTop(xE);
    }
  }
  plotObject.variablesInPlay.quickSortAscending();
  plotObject.variablesInPlayJS.setSize(plotObject.variablesInPlay.size);
  for (int i = 0; i < plotObject.variablesInPlay.size; i ++) {
    plotObject.variablesInPlayJS[i] = plotObject.variablesInPlay[i].toString();
  }
  plotObject.plotType = "plotDirectionField";
  if (
    !input[4].isSequenceNElements(2) &&
    !input[4].startsWith(calculator.opIntervalOpen(), 3)
  ) {
    return
    calculator
    << "<hr>Could not extract a list of elements for the "
    << "number of segments from: "
    << input[4].toString();
  }
  plotObject.numberOfSegmentsJS.setSize(2);
  for (int i = 0; i < 2; i ++) {
    if (
      !extractor.extract(
        input[4][i + 1],
        plotObject.numberOfSegmentsJS[i],
        &calculator.comments
      )
    ) {
      return
      calculator
      << "Failed to convert "
      << input[4][i + 1].toString()
      << " to javascript. ";
    }
  }
  extractor.writeParameterNames(plotObject);
  plot.addPlotOnTop(plotObject);
  return output.assignValue(calculator, plot);
}

bool CalculatorFunctionsCrypto::jwtVerifyAgainstRSA256(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsCrypto::jwtVerifyAgainstRSA256");
  if (input.size() != 4) {
    return
    calculator
    << "The JWT verify command expects 3 arguments: "
    << "string with the token in the usual format (\"a.b.c\"), "
    << "the modulus of the key and the exponent of the key. ";
  }
  std::string tokenString;
  if (!input[1].isOfType(&tokenString)) {
    return
    calculator
    << "The first argument of "
    << input.toString()
    << " is not a string. ";
  }
  std::stringstream out;
  std::string modulusBase64, exponentBase64;
  if (
    !input[2].isOfType(&modulusBase64) || !input[3].isOfType(&exponentBase64)
  ) {
    return
    calculator
    << "Failed to convert the arguments "
    << input[2].toString()
    << " and "
    << input[3].toString()
    << " to base64 strings";
  }
  JSONWebToken webToken;
  LargeIntegerUnsigned modulus, exponent;
  if (!webToken.assignString(tokenString, &out)) {
    return output.assignValue(calculator, out.str());
  }
  out
  << "Sucesfully extracted JWT token. <br>"
  << webToken.toString()
  << "<br>";
  if (
    !Crypto::convertBase64ToLargeUnsigned(modulusBase64, modulus, &out) ||
    !Crypto::convertBase64ToLargeUnsigned(exponentBase64, exponent, &out)
  ) {
    return output.assignValue(calculator, out.str());
  }
  out << "<br>Successfully extracted modulus and exponent";
  webToken.verifyRSA256(modulus, exponent, &out, &out);
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctionsCrypto::jwtVerifyAgainstKnownKeys(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsCrypto::jwtVerifyAgainstKnownKeys");
  if (!global.userDefaultHasAdminRights()) {
    return
    calculator
    << "This function is only available to logged-in admins. ";
  }
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].isOfType(&inputString)) {
    return false;
  }
  std::stringstream out;
  Crypto::verifyJWTagainstKnownKeys(inputString, &out, &out);
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctionsEncoding::convertHexToString(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsEncoding::convertHexToString");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].isOfType(&inputString)) {
    inputString = input[1].toString();
  }
  std::string result;
  std::stringstream commentsOnFailure;
  if (
    !Crypto::convertHexToString(inputString, result, &commentsOnFailure)
  ) {
    return
    calculator
    << "Failed to interpret your input as string. "
    << commentsOnFailure.str();
  }
  return output.assignValue(calculator, result);
}

bool CalculatorFunctionsEncoding::convertIntegerToHex(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsEncoding::convertIntegerToHex");
  if (input.size() != 2) {
    return false;
  }
  LargeInteger largeInteger;
  if (!input[1].isInteger(&largeInteger)) {
    return false;
  }
  if (largeInteger < 0) {
    return calculator << "I only convert positive integers to hex strings. ";
  }
  std::string result;
  if (
    !Crypto::convertLargeUnsignedToHexSignificantDigitsFirst(
      largeInteger.value, 0, result
    )
  ) {
    return
    calculator
    << "Failed to convert "
    << largeInteger
    << " to a hex string. ";
  }
  return output.assignValue(calculator, result);
}

bool CalculatorFunctionsEncoding::convertHexToInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsEncoding::convertHexToInteger");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].isOfType(&inputString)) {
    return false;
  }
  LargeIntegerUnsigned result;
  int notUsedNumberOfLeadingZeroes = 0;
  if (
    !Crypto::convertHexToInteger(
      inputString, result, notUsedNumberOfLeadingZeroes
    )
  ) {
    return
    calculator
    << "Failed to interpret "
    << inputString
    << " as a hex string ";
  }
  Rational resultRat = result;
  return output.assignValue(calculator, resultRat);
}

bool CalculatorFunctionsEncoding::testJSON(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsEncoding::testJSON");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].isOfType(&inputString)) {
    inputString = input[1].toString();
  }
  JSData data;
  std::stringstream comments, out;
  if (!data.parse(inputString, true, &comments)) {
    return output.assignValue(calculator, comments.str());
  }
  out
  << "Input:<br>"
  << inputString
  << "<hr>Output: <br>"
  << data.toString(nullptr);
  if (comments.str() != "") {
    out << "<hr>Extra comments: " << comments.str();
  }
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctionsEncoding::convertBase64ToHex(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsEncoding::convertBase64ToHex");
  if (input.size() != 2) {
    return false;
  }
  std::string inputString;
  if (!input[1].isOfType(&inputString)) {
    inputString = input[1].toString();
  }
  std::string result, bitStream;
  if (
    !Crypto::convertBase64ToString(
      inputString,
      bitStream,
      &calculator.comments,
      &calculator.comments
    )
  ) {
    return false;
  }
  Crypto::convertStringToHex(bitStream, result, 0, false);
  return output.assignValue(calculator, result);
}

bool CalculatorFunctionsCrypto::generateRandomPrime(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsCrypto::generateRandomPrime");
  if (input.size() != 2) {
    return false;
  }
  int numberOfBytes = 0;
  if (!input[1].isSmallInteger(&numberOfBytes)) {
    return false;
  }
  int maxNumberOfBytes = 128;
  int minNumberOfBytes = 1;
  if (numberOfBytes > maxNumberOfBytes || numberOfBytes < minNumberOfBytes) {
    return
    calculator
    << "Max number of bytes: "
    << maxNumberOfBytes
    << ", min number of bytes: "
    << minNumberOfBytes
    << ", you requested: "
    << numberOfBytes
    << ". ";
  }
  LargeIntegerUnsigned result;
  Crypto::Random::getRandomLargePrime(result, numberOfBytes);
  return output.assignValue(calculator, result);
}

bool CalculatorFunctionsCrypto::testRSASign(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsCrypto::testRSASign");
  if (input.size() != 4) {
    return
    calculator
    << "RSA signature takes as input 3 arguments: message and two primes. ";
  }
  std::stringstream out, errorStream;
  std::string messageString;
  if (!input[1].isOfType(&messageString)) {
    return false;
  }
  PrivateKeyRSA key;
  if (
    !input[2].isIntegerNonNegative(&key.primeOne) ||
    !input[3].isIntegerNonNegative(&key.primeTwo)
  ) {
    return
    calculator
    << "Failed to extract positive integers "
    << "from the second and third argument. ";
  }
  if (
    !key.computeFromTwoPrimes(
      key.primeOne, key.primeTwo, true, &errorStream
    )
  ) {
    return
    calculator
    << "Inputs do not appear to be prime. "
    << errorStream.str();
  }
  List<unsigned char> message, paddedMessage, signature;
  message = messageString;
  key.hashAndPadPKCS1(
    message,
    SignatureAlgorithmSpecification::HashAlgorithm::sha256,
    paddedMessage
  );
  out << "Temporary private key:<br>" << key.toString() << "<br>";
  key.signBytesPadPKCS1(
    message,
    SignatureAlgorithmSpecification::HashAlgorithm::sha256,
    signature
  );
  out << "Message:<br>" << Crypto::convertListUnsignedCharsToHex(message);
  out
  << "<br>Padded message digest:<br>"
  << Crypto::convertListUnsignedCharsToHex(paddedMessage);
  out
  << "<br>Signature:<br>"
  << Crypto::convertListUnsignedCharsToHex(signature);
  ElementZmodP element, one;
  element.modulus = key.publicKey.modulus;
  one.makeOne(element.modulus);
  Crypto::convertListUnsignedCharsToLargeUnsignedIntegerBigEndian(
    signature, element.value
  );
  out << "<br>Signature integer:<br>" << element.value.toString();
  MathRoutines::raiseToPower(element, key.publicKey.exponent, one);
  out
  << "<br>Signature power e mod n [e = "
  << key.publicKey.exponent
  << ", n = "
  << key.publicKey.modulus
  << "]"
  << ":<br>"
  << element.value.toString();
  std::string hexadecimalString;
  Crypto::convertLargeUnsignedToHexSignificantDigitsFirst(
    element.value, 0, hexadecimalString
  );
  out << "<br>Converted to hex:<br>" << hexadecimalString;
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctionsCrypto::RSAEncrypt(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsCrypto::RSAEncrypt");
  if (input.size() != 4) {
    return false;
  }
  LargeInteger exponent;
  LargeInteger modulus;
  LargeInteger message;
  LargeInteger result;
  if (
    !input[1].isInteger(&modulus) ||
    !input[2].isInteger(&exponent) ||
    !input[3].isInteger(&message)
  ) {
    return
    calculator
    << "Failed to extract three (large) integers from the arguments of "
    << input.toString();
  }
  if (modulus < 0) {
    modulus *= - 1;
  }
  if (modulus == 0 || exponent == 0) {
    return calculator << "The modulus and exponent must be non-zero.";
  }
  if (modulus == 1) {
    return calculator << "Modulus 1 not allowed";
  }
  result = Crypto::rsaEncrypt(modulus.value, exponent, message);
  return output.assignValue(calculator, Rational(result));
}

bool CalculatorFunctions::isSquare(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::isSquare");
  if (input.size() != 2) {
    return false;
  }
  LargeInteger value;
  if (!input[1].isInteger(&value)) {
    return false;
  }
  if (value < 0) {
    return output.assignValue(calculator, 0);
  }
  if (value == 0) {
    return output.assignValue(calculator, 1);
  }
  List<int> multiplicities;
  List<LargeInteger> factors;
  if (!value.value.factor(factors, multiplicities, 0, 4, nullptr)) {
    return
    calculator
    << "Failed to factor: "
    << value.toString()
    << " (may be too large?).";
  }
  int result = 1;
  for (int i = 0; i < multiplicities.size; i ++) {
    if ((multiplicities[i] % 2) != 0) {
      result = 0;
      break;
    }
  }
  return output.assignValue(calculator, result);
}

bool CalculatorFunctions::isSquareFree(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::isSquareFree");
  if (input.size() != 2) {
    return false;
  }
  LargeInteger integerValue;
  if (!input[1].isInteger(&integerValue)) {
    return false;
  }
  List<int> multiplicities;
  List<LargeInteger> factors;
  if (
    !integerValue.value.factor(
      factors, multiplicities, 0, 3, &calculator.comments
    )
  ) {
    return
    calculator
    << "Failed to factor: "
    << integerValue.toString()
    << " (may be too large?).";
  }
  int result = 1;
  for (int i = 0; i < multiplicities.size; i ++) {
    if (multiplicities[i] > 1) {
      result = 0;
      break;
    }
  }
  return output.assignValue(calculator, result);
}

bool CalculatorFunctions::isSquareFreePolynomial(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::isSquareFreePolynomial");
  if (input.size() != 2) {
    return false;
  }
  WithContext<Polynomial<Rational> > polynomial;
  if (
    !CalculatorConversions::functionPolynomial(
      calculator, input[1], polynomial, - 1, - 1, false
    )
  ) {
    return false;
  }
  std::stringstream out;
  bool squareFree =
  polynomial.content.isSquareFree(Rational::one(), &out);
  return output.assignValue(calculator, Rational(int(squareFree)));
}

bool CalculatorFunctions::isPower(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::isPower");
  if (input.size() != 2) {
    return false;
  }
  LargeInteger toBeFactored;
  if (!input[1].isInteger(&toBeFactored)) {
    return false;
  }
  if (toBeFactored.isEqualToZero()) {
    return false;
  }
  List<int> multiplicities;
  List<LargeInteger> factors;
  if (
    !toBeFactored.value.factor(
      factors, multiplicities, 0, 3, &calculator.comments
    )
  ) {
    return
    calculator
    << "Failed to factor: "
    << toBeFactored.toString()
    << " (may be too large?).";
  }
  int result = 1;
  if (multiplicities.size > 0) {
    result = (multiplicities[0] > 1);
  }
  for (int i = 1; i < multiplicities.size; i ++) {
    if (multiplicities[i] != multiplicities[0]) {
      result = 0;
      break;
    }
  }
  return output.assignValue(calculator, result);
}

bool CalculatorFunctions::numberOfIntegerDivisors(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::numberOfIntegerDivisors");
  if (input.size() != 2) {
    return false;
  }
  LargeInteger argument;
  if (!input[1].isInteger(&argument)) {
    return false;
  }
  if (argument.isEqualToZero()) {
    return calculator << "Factorizaion of 0 is not defined in the calculator.";
  }
  List<LargeInteger> allPrimeFactors;
  List<int> allMultiplicities;
  argument.value.factor(
    allPrimeFactors, allMultiplicities, 10000, 2, nullptr
  );
  LargeInteger result = 1;
  for (int i = 0; i < allMultiplicities.size; i ++) {
    result *= allMultiplicities[i] + 1;
  }
  result *= 2;
  return output.assignValue(calculator, Rational(result));
}

bool CalculatorFunctions::factorInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::factorInteger");
  if (input.size() != 2) {
    return false;
  }
  return
  CalculatorFunctions::functionFactorInteger(calculator, input[1], output);
}

bool CalculatorFunctions::functionFactorInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::functionFactorInteger");
  LargeInteger integer;
  int upperBound = 30000;
  int opFactorInteger = calculator.operations.getIndexNoFail("FactorInteger");
  if (input.startsWith(opFactorInteger) && input.size() == 3) {
    if (!input[1].isInteger(&integer)) {
      return false;
    }
    if (!input[2].isIntegerFittingInInt(&upperBound)) {
      return
      calculator
      << "Failed to extract integer from "
      << upperBound
      << ".";
    }
  } else if (!input.isInteger(&integer)) {
    return false;
  }
  if (integer.isEqualToZero()) {
    return false;
  }
  List<LargeInteger> primeFactors;
  List<int> multiplicities;
  bool complete =
  integer.value.factor(
    primeFactors, multiplicities, upperBound, 3, &calculator.comments
  );
  if (!complete && primeFactors.size <= 1) {
    return false;
  }
  List<Expression> result;
  int numberOfPrimeFactors = primeFactors.size;
  if (!complete) {
    numberOfPrimeFactors --;
  }
  for (int i = 0; i < numberOfPrimeFactors; i ++) {
    Expression currentE;
    currentE.assignValue(calculator, Rational(primeFactors[i]));
    for (int j = 0; j < multiplicities[i]; j ++) {
      result.addOnTop(currentE);
    }
  }
  if (integer < 0 && result.size > 0) {
    result[0] *= - 1;
  }
  if (complete) {
    return output.makeSequence(calculator, &result);
  } else {
    Expression factorsSoFar, factorNext, numberLast;
    factorsSoFar.makeSequence(calculator, &result);
    factorNext.reset(calculator);
    factorNext.addChildAtomOnTop(opFactorInteger);
    numberLast.assignValue(
      calculator, Rational(primeFactors[primeFactors.size - 1])
    );
    factorNext.addChildOnTop(numberLast);
    return
    output.makeXOX(
      calculator, calculator.opUnion(), factorsSoFar, factorNext
    );
  }
}

bool CalculatorFunctions::factorOutNumberContent(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::factorOutNumberContent");
  if (input.size() != 2) {
    return calculator << "FactorOutNumberContent expects single argument. ";
  }
  LinearCombination<Expression, Rational> linearCombination;
  if (
    !calculator.functionCollectSummandsCombine(
      calculator, input[1], linearCombination
    )
  ) {
    return
    calculator
    << "Failed to extract summands from: "
    << input[1].toString();
  }
  if (linearCombination.isEqualToZero()) {
    return output.assignValue(calculator, 0);
  }
  Rational scale = linearCombination.scaleNormalizeLeadingMonomial(nullptr);
  if (scale == 0) {
    return false;
  }
  scale.invert();
  if (scale == 1) {
    output = input[1];
    return true;
  }
  Expression left, right;
  left.assignValue(calculator, scale);
  right.makeSum(calculator, linearCombination);
  output = left * right;
  return true;
}

bool CalculatorFunctions::subList(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::subList");
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].isSequenceNElements()) {
    return false;
  }
  HashedList<Expression> boundVars;
  if (!input[2].getBoundVariables(boundVars)) {
    return
    calculator
    << "Could not get bound variables from: "
    << input[2].toString();
  }
  if (input[2].isEqualToOne()) {
    output = input[1];
    return true;
  }
  if (boundVars.size == 0) {
    return output.makeSequence(calculator, nullptr);
  }
  Expression substituted, toBeSubbed, subbedSimplified;
  toBeSubbed.reset(calculator);
  toBeSubbed.addChildAtomOnTop(calculator.opBind());
  toBeSubbed.addChildOnTop(boundVars[0]);
  List<Expression> currentList;
  for (int i = 1; i < input[1].size(); i ++) {
    substituted = input[2];
    substituted.substituteRecursively(toBeSubbed, input[1][i]);
    if (
      !calculator.evaluateExpression(
        calculator, substituted, subbedSimplified
      )
    ) {
      return calculator << "Failed to evaluate " << substituted.toString();
    }
    if (subbedSimplified.isEqualToOne()) {
      currentList.addOnTop(input[1][i]);
    }
  }
  return output.makeSequence(calculator, &currentList);
}

bool CalculatorFunctions::applyToList(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::applyToList");
  if (input.size() != 3) {
    return false;
  }
  const Expression& functionExpression = input[1];
  if (!input[2].isSequenceNElements()) {
    return false;
  }
  List<Expression> result;
  result.setSize(input[2].size() - 1);
  for (int i = 1; i < input[2].size(); i ++) {
    result[i - 1].reset(calculator);
    result[i - 1].addChildOnTop(functionExpression);
    result[i - 1].addChildOnTop(input[2][i]);
  }
  return output.makeSequence(calculator, &result);
}

bool CalculatorFunctions::matchesPattern(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::matchesPattern");
  if (input.size() != 3) {
    return false;
  }
  MapList<Expression, Expression> matchedExpressions;
  if (
    !calculator.expressionMatchesPattern(
      input[2], input[1], matchedExpressions, nullptr
    )
  ) {
    return output.assignValue(calculator, 0);
  }
  Expression commandList;
  commandList.reset(calculator);
  commandList.addChildAtomOnTop(calculator.opCommandSequence());
  for (int i = 0; i < matchedExpressions.size(); i ++) {
    Expression currentCommand;
    if (
      !matchedExpressions.keys[i].startsWith(calculator.opBind(), 2)
    ) {
      std::stringstream errorStream;
      errorStream
      << "Bound variable "
      << matchedExpressions.keys[i].toString()
      << " does not start with the bind atom. ";
      return output.assignError(calculator, errorStream.str());
    }
    currentCommand.makeXOX(
      calculator,
      calculator.opDefine(),
      matchedExpressions.keys[i][1],
      matchedExpressions.values[i]
    );
    commandList.addChildOnTop(currentCommand);
  }
  output.reset(calculator);
  output.addChildAtomOnTop(calculator.opCommandEnclosure());
  return output.addChildOnTop(commandList);
}

bool CalculatorFunctions::degreesToRadians(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::degreesToRadians");
  if (!input.startsWith(calculator.opPower(), 3)) {
    return false;
  }
  if (!input[2].isOperationGiven("\\circ")) {
    return false;
  }
  Expression piE;
  piE.makeAtom(calculator, calculator.opPi());
  output = input[1] * piE;
  output /= 180;
  return true;
}

bool CalculatorFunctions::lessThanOrEqualTo(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::lessThanOrEqualTo");
  if (input.size() != 3) {
    return false;
  }
  Expression result(calculator);
  result.addChildAtomOnTop("\\geq");
  result.addChildOnTop(input[2]);
  result.addChildOnTop(input[1]);
  output = result;
  return true;
}

bool CalculatorFunctions::greaterThanOrEqualTo(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::greaterThanOrEqualTo");
  if (input.size() != 3) {
    return false;
  }
  const Expression& left = input[1];
  const Expression& right = input[2];
  Rational leftRat, rightRat;
  if (left.isRational(&leftRat) && right.isRational(&rightRat)) {
    if (leftRat >= rightRat) {
      return output.assignValue(calculator, 1);
    }
    return output.assignValue(calculator, 0);
  }
  double leftD, rightD;
  if (
    left.evaluatesToDouble(&leftD) && right.evaluatesToDouble(&rightD)
  ) {
    if (leftD >= rightD) {
      return output.assignValue(calculator, 1);
    }
    return output.assignValue(calculator, 0);
  }
  return false;
}

bool CalculatorFunctions::greaterThan(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::greaterThan");
  if (!input.isListNElements(3)) {
    return false;
  }
  const Expression& left = input[1];
  const Expression& right = input[2];
  Rational leftRat, rightRat;
  if (left.isRational(&leftRat) && right.isRational(&rightRat)) {
    if (leftRat > rightRat) {
      return output.assignValue(calculator, 1);
    }
    return output.assignValue(calculator, 0);
  }
  double leftD, rightD;
  if (
    left.evaluatesToDouble(&leftD) && right.evaluatesToDouble(&rightD)
  ) {
    if (leftD > rightD) {
      return output.assignValue(calculator, 1);
    }
    return output.assignValue(calculator, 0);
  }
  return false;
}

bool CalculatorFunctions::lessThan(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::lessThan");
  if (!input.isListNElements(3)) {
    return false;
  }
  Expression swappedE(calculator);
  swappedE.addChildOnTop(input[0]);
  swappedE.addChildOnTop(input[2]);
  swappedE.addChildOnTop(input[1]);
  return CalculatorFunctions::greaterThan(calculator, swappedE, output);
}

bool CalculatorFunctions::collectOpands(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::collectOpands");
  if (input.size() != 3) {
    return false;
  }
  if (!input[1].isAtom()) {
    return false;
  }
  List<Expression> opandList;
  calculator.appendOpandsReturnTrueIfOrderNonCanonical(
    input[2], opandList, input[1].data
  );
  return output.makeSequence(calculator, &opandList);
}

bool CalculatorFunctions::collectMultiplicands(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::outerCollectSummands");
  if (input.size() != 2) {
    return false;
  }
  List<Expression> multiplicands;
  calculator.appendOpandsReturnTrueIfOrderNonCanonical(
    input[1], multiplicands, calculator.opTimes()
  );
  return output.makeSequence(calculator, &multiplicands);
}

bool CalculatorFunctions::collectSummands(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::collectSummands");
  if (input.size() != 2) {
    return false;
  }
  List<Expression> summands;
  calculator.appendSummandsReturnTrueIfOrderNonCanonical(
    input[1], summands
  );
  return output.makeSequence(calculator, &summands);
}

bool CalculatorFunctions::leftIntervalGreaterThanRight(
  const Expression& left, const Expression& right
) {
  if (left.owner == nullptr || right.owner == nullptr) {
    return left > right;
  }
  if (left.size() != 3 || right.size() != 3) {
    return left > right;
  }
  if (!left.isIntervalRealLine()) {
    return left > right;
  }
  if (!right.isIntervalRealLine()) {
    return left > right;
  }
  double left1, right1, left2, right2;
  bool left1IsDouble = left[1].evaluatesToDouble(&left1);
  bool right1IsDouble = right[1].evaluatesToDouble(&right1);
  bool left2IsDouble = left[2].evaluatesToDouble(&left2);
  bool righ2IsDouble = right[2].evaluatesToDouble(&right2);
  const Expression& inftyE = right.owner->expressionInfinity();
  const Expression& mInftyE = right.owner->expressionMinusInfinity();
  if (left1IsDouble && right1IsDouble) {
    if (left1 > right1) {
      return true;
    }
    if (right1 > left1) {
      return false;
    }
    if (left2IsDouble && righ2IsDouble) {
      if (left2 > right2) {
        return true;
      }
      if (right2 > left2) {
        return false;
      }
    }
    if (left[2] == inftyE && righ2IsDouble) {
      return true;
    }
    if (left2IsDouble && right[2] == inftyE) {
      return false;
    }
  }
  if (left[1] == mInftyE && right1IsDouble) {
    return false;
  }
  if (left1IsDouble && right[1] == mInftyE) {
    return true;
  }
  if (left[1] == mInftyE || right[1] == mInftyE) {
    if (left2IsDouble && righ2IsDouble) {
      if (left2 > right2) {
        return true;
      }
      if (right2 > left2) {
        return false;
      }
    }
    if (left[2] == inftyE && righ2IsDouble) {
      return true;
    }
    if (left2IsDouble && right[2] == inftyE) {
      return false;
    }
  }
  return left > right;
}

bool CalculatorFunctionsIntervals::intersectIntervals(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsIntervals::intersectIntervals");
  if (!input.startsWith(calculator.opIntersection(), 3)) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  if (!leftE.isIntervalRealLine()) {
    return false;
  }
  if (!rightE.isIntervalRealLine()) {
    return false;
  }
  double left1 = 0, left2 = 0, right1 = 0, right2 = 0;
  if (
    !leftE[1].evaluatesToDouble(&left1) || !leftE[2].evaluatesToDouble(&left2)
  ) {
    return false;
  }
  if (
    !rightE[1].evaluatesToDouble(&right1) ||
    !rightE[2].evaluatesToDouble(&right2)
  ) {
    return false;
  }
  if (left1 > left2 || right1 > right2) {
    return false;
  }
  bool leftIsClosed = true;
  bool rightIsClosed = true;
  Expression leftFinal, rightFinal;
  double leftResult = 0, rightResult = 0;
  if (left1 < right1) {
    leftFinal = rightE[1];
    leftResult = right1;
    if (
      rightE.startsWith(calculator.opSequence()) ||
      rightE.startsWith(calculator.opIntervalRightClosed())
    ) {
      leftIsClosed = false;
    }
  }
  if (left1 - right1 == 0.0) {
    leftFinal = leftE[1];
    leftResult = left1;
    if (
      leftE.startsWith(calculator.opSequence()) ||
      leftE.startsWith(calculator.opIntervalRightClosed()) ||
      rightE.startsWith(calculator.opSequence()) ||
      rightE.startsWith(calculator.opIntervalRightClosed())
    ) {
      leftIsClosed = false;
    }
  }
  if (left1 > right1) {
    leftFinal = leftE[1];
    leftResult = left1;
    if (
      leftE.startsWith(calculator.opSequence()) ||
      leftE.startsWith(calculator.opIntervalRightClosed())
    ) {
      leftIsClosed = false;
    }
  }
  // /////////////////////////
  if (left2 > right2) {
    rightFinal = rightE[2];
    rightResult = right2;
    if (
      rightE.startsWith(calculator.opSequence()) ||
      rightE.startsWith(calculator.opIntervalLeftClosed())
    ) {
      rightIsClosed = false;
    }
  }
  if (left2 - right2 == 0.0) {
    rightFinal = rightE[2];
    rightResult = right2;
    if (
      leftE.startsWith(calculator.opSequence()) ||
      leftE.startsWith(calculator.opIntervalLeftClosed()) ||
      rightE.startsWith(calculator.opSequence()) ||
      rightE.startsWith(calculator.opIntervalLeftClosed())
    ) {
      rightIsClosed = false;
    }
  }
  if (left2 < right2) {
    rightFinal = leftE[2];
    rightResult = left2;
    if (
      leftE.startsWith(calculator.opSequence()) ||
      leftE.startsWith(calculator.opIntervalLeftClosed())
    ) {
      rightIsClosed = false;
    }
  }
  if (leftResult > rightResult) {
    return output.makeAtom(calculator, "\\emptyset");
  }
  if (leftIsClosed && rightIsClosed) {
    return
    output.makeXOX(
      calculator, calculator.opIntervalClosed(), leftFinal, rightFinal
    );
  }
  if (!leftIsClosed && rightIsClosed) {
    return
    output.makeXOX(
      calculator,
      calculator.opIntervalRightClosed(),
      leftFinal,
      rightFinal
    );
  }
  if (leftIsClosed && !rightIsClosed) {
    return
    output.makeXOX(
      calculator,
      calculator.opIntervalLeftClosed(),
      leftFinal,
      rightFinal
    );
  }
  if (!leftIsClosed && !rightIsClosed) {
    return
    output.makeXOX(
      calculator, calculator.opSequence(), leftFinal, rightFinal
    );
  }
  return false;
}

bool CalculatorFunctionsIntervals::unionUnionIntervals(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsIntervals::unionUnionIntervals");
  if (input.size() != 3) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightComposite = input[2];
  if (!rightComposite.startsWith(calculator.opUnion(), 3)) {
    return false;
  }
  const Expression& rightE = input[2][1];
  Expression middleUnion, middleUnionReduced;
  middleUnion.makeXOX(calculator, calculator.opUnion(), leftE, rightE);
  if (
    !CalculatorFunctionsIntervals::unionIntervals(
      calculator, middleUnion, middleUnionReduced
    )
  ) {
    return false;
  }
  return
  output.makeXOX(
    calculator,
    calculator.opUnion(),
    middleUnionReduced,
    rightComposite[2]
  );
}

bool CalculatorFunctionsIntervals::unionIntervals(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsIntervals::unionIntervals");
  if (input.size() != 3) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  if (!leftE.isIntervalRealLine()) {
    return false;
  }
  if (!rightE.isIntervalRealLine()) {
    return false;
  }
  double left1 = 0, left2 = 0, right1 = 0, right2 = 0;
  if (
    !leftE[1].evaluatesToDouble(&left1) || !leftE[2].evaluatesToDouble(&left2)
  ) {
    return false;
  }
  if (
    !rightE[1].evaluatesToDouble(&right1) ||
    !rightE[2].evaluatesToDouble(&right2)
  ) {
    return false;
  }
  if (left1 > left2 || right1 > right2) {
    return false;
  }
  bool makeUnion = false;
  if (right1 <= left2 && left2 <= right2) {
    makeUnion = true;
  }
  if ((right1 - left2 == 0.0) &&
    leftE.startsWith(calculator.opIntervalOpen(), 3) &&
    rightE.startsWith(calculator.opIntervalOpen(), 3)
  ) {
    makeUnion = false;
  }
  if (!makeUnion) {
    return false;
  }
  bool leftIsClosed = false;
  bool rightIsClosed = false;
  Expression leftFinal, rightFinal;
  if (left1 < right1) {
    leftFinal = leftE[1];
    if (
      leftE.startsWith(calculator.opIntervalClosed()) ||
      leftE.startsWith(calculator.opIntervalLeftClosed())
    ) {
      leftIsClosed = true;
    }
  }
  if (left1 - right1 == 0.0) {
    leftFinal = leftE[1];
    if (
      leftE.startsWith(calculator.opIntervalClosed()) ||
      leftE.startsWith(calculator.opIntervalLeftClosed()) ||
      rightE.startsWith(calculator.opIntervalClosed()) ||
      rightE.startsWith(calculator.opIntervalLeftClosed())
    ) {
      leftIsClosed = true;
    }
  }
  if (left1 > right1) {
    leftFinal = rightE[1];
    if (
      rightE.startsWith(calculator.opIntervalClosed()) ||
      rightE.startsWith(calculator.opIntervalLeftClosed())
    ) {
      leftIsClosed = true;
    }
  }
  // /////////////////////
  if (left2 > right2) {
    rightFinal = leftE[2];
    if (
      leftE.startsWith(calculator.opIntervalClosed()) ||
      leftE.startsWith(calculator.opIntervalRightClosed())
    ) {
      rightIsClosed = true;
    }
  }
  if (left2 - right2 == 0.0) {
    rightFinal = rightE[2];
    if (
      leftE.startsWith(calculator.opIntervalClosed()) ||
      leftE.startsWith(calculator.opIntervalRightClosed()) ||
      rightE.startsWith(calculator.opIntervalClosed()) ||
      rightE.startsWith(calculator.opIntervalRightClosed())
    ) {
      rightIsClosed = true;
    }
  }
  if (left2 < right2) {
    rightFinal = rightE[2];
    if (
      rightE.startsWith(calculator.opIntervalClosed()) ||
      rightE.startsWith(calculator.opIntervalRightClosed())
    ) {
      rightIsClosed = true;
    }
  }
  if (leftIsClosed && rightIsClosed) {
    return
    output.makeXOX(
      calculator, calculator.opIntervalClosed(), leftFinal, rightFinal
    );
  }
  if (!leftIsClosed && rightIsClosed) {
    return
    output.makeXOX(
      calculator,
      calculator.opIntervalRightClosed(),
      leftFinal,
      rightFinal
    );
  }
  if (leftIsClosed && !rightIsClosed) {
    return
    output.makeXOX(
      calculator,
      calculator.opIntervalLeftClosed(),
      leftFinal,
      rightFinal
    );
  }
  if (!leftIsClosed && !rightIsClosed) {
    return
    output.makeXOX(
      calculator, calculator.opIntervalOpen(), leftFinal, rightFinal
    );
  }
  return false;
}

bool CalculatorFunctionsIntervals::normalizeIntervals(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsIntervals::outerNormalizeIntervals");
  if (input.size() != 2) {
    return false;
  }
  List<Expression> outputList;
  if (
    !calculator.collectOpands(
      input[1], calculator.opUnion(), outputList
    )
  ) {
    return false;
  }
  List<Expression>::Comparator order(
    CalculatorFunctions::leftIntervalGreaterThanRight
  );
  outputList.quickSortAscending(&order);
  return
  output.makeXOXOdotsOX(calculator, calculator.opUnion(), outputList);
}

bool CalculatorFunctionsListsAndSets::unionEmptySet(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsListsAndSets::unionEmptySet");
  if (!input.startsWith(calculator.opUnion(), 3)) {
    return false;
  }
  if (input[1].isOperationGiven("\\emptyset")) {
    output = input[2];
    return true;
  }
  if (input[2].isOperationGiven("\\emptyset")) {
    output = input[1];
    return true;
  }
  return false;
}

bool CalculatorFunctionsListsAndSets::intersectUnion(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsListsAndSets::intersectUnion");
  return
  CalculatorBasics::distribute(
    calculator,
    input,
    output,
    calculator.opUnion(),
    calculator.opIntersection(),
    false
  );
}

bool CalculatorFunctions::compareIntervalsNumerically(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::compareIntervalsNumerically");
  if (input.size() != 4) {
    return
    calculator
    << "Comparing intervals numerically takes "
    << "3 arguments: two unions and precision. ";
  }
  double precision = 0;
  if (!input[3].evaluatesToDouble(&precision)) {
    return calculator << "Could not extract precision from the last argument.";
  }
  List<Expression> leftList, rightList;
  if (
    !calculator.collectOpands(
      input[1], calculator.opUnion(), leftList
    )
  ) {
    return false;
  }
  if (
    !calculator.collectOpands(
      input[2], calculator.opUnion(), rightList
    )
  ) {
    return false;
  }
  if (leftList.size != rightList.size) {
    return output.assignValue(calculator, 0);
  }
  List<Expression>::Comparator order(
    CalculatorFunctions::leftIntervalGreaterThanRight
  );
  leftList.quickSortAscending(&order);
  rightList.quickSortAscending(&order);
  for (int i = 0; i < leftList.size; i ++) {
    if (leftList[i] == rightList[i]) {
      continue;
    }
    if ((!leftList[i].isIntervalRealLine()) || (
        !rightList[i].isIntervalRealLine()
      )
    ) {
      return output.assignValue(calculator, 0);
    }
    if (leftList[i][0] != rightList[i][0]) {
      return output.assignValue(calculator, 0);
    }
    double left1 = 0, left2 = 0, right1 = 0, right2 = 0;
    if (
      !leftList[i][1].evaluatesToDouble(&left1) ||
      !rightList[i][1].evaluatesToDouble(&right1)
    ) {
      if (leftList[i][1] != rightList[i][1]) {
        return output.assignValue(calculator, 0);
      }
    }
    if (
      !rightList[i][2].evaluatesToDouble(&right2) ||
      !leftList[i][2].evaluatesToDouble(&left2)
    ) {
      if (leftList[i][2] != rightList[i][2]) {
        return output.assignValue(calculator, 0);
      }
    }
    if (
      FloatingPoint::absFloating(left1 - right1) > precision ||
      FloatingPoint::absFloating(left2 - right2) > precision
    ) {
      return output.assignValue(calculator, 0);
    }
  }
  return output.assignValue(calculator, 1);
}

bool CalculatorFunctions::intersectEmptySet(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::intersectEmptySet");
  if (!input.startsWith(calculator.opIntersection(), 3)) {
    return false;
  }
  if (input[1].isOperationGiven("\\emptyset")) {
    output = input[1];
    return true;
  }
  if (input[2].isOperationGiven("\\emptyset")) {
    output = input[2];
    return true;
  }
  return false;
}

bool CalculatorFunctions::isLinearOrConstantIn(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::isLinearOrConstantIn");
  if (input.size() < 3) {
    return false;
  }
  List<List<Expression> > summands;
  if (!calculator.getSumProductsExpressions(input[2], summands)) {
    return calculator << "Failed to extract sum from " << input[2].toString();
  }
  for (int i = 0; i < summands.size; i ++) {
    bool found = false;
    for (int j = 0; j < summands[i].size; j ++) {
      if (summands[i][j] == input[1]) {
        if (found) {
          return output.assignValue(calculator, 0);
        }
        found = true;
      } else if (!summands[i][j].evaluatesToDouble()) {
        return output.assignValue(calculator, 0);
      }
    }
  }
  return output.assignValue(calculator, 1);
}

bool CalculatorFunctions::isProductLinearOrConstantTermsIn(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::isProductLinearOrConstantTermsIn");
  if (input.size() < 3) {
    return false;
  }
  List<Expression> multiplicands;
  if (
    !calculator.collectOpands(
      input[2], calculator.opTimes(), multiplicands
    )
  ) {
    return
    calculator
    << "Could not extract multiplicands from: "
    << input[2].toString();
  }
  for (int k = 0; k < multiplicands.size; k ++) {
    List<List<Expression> > summands;
    if (
      !calculator.getSumProductsExpressions(multiplicands[k], summands)
    ) {
      return
      calculator
      << "Failed to extract sum from "
      << multiplicands[k].toString();
    }
    for (int i = 0; i < summands.size; i ++) {
      bool found = false;
      for (int j = 0; j < summands[i].size; j ++) {
        if (summands[i][j] == input[1]) {
          if (found) {
            return output.assignValue(calculator, 0);
          }
          found = true;
        } else if (!summands[i][j].evaluatesToDouble()) {
          return output.assignValue(calculator, 0);
        }
      }
    }
  }
  return output.assignValue(calculator, 1);
}

bool CalculatorFunctions::orIdentical(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::orIdentical");
  (void) calculator;
  if (!input.startsWithGivenOperation("or", 3)) {
    return false;
  }
  if (input[1] == input[2]) {
    output = input[1];
    return true;
  }
  return false;
}

bool CalculatorFunctions::distributeSqrt(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::distributeSqrt");
  if (!input.startsWith(calculator.opSqrt(), 3)) {
    return false;
  }
  const Expression& base = input[2];
  const Expression& oneOverExponentE = input[1];
  if (!base.startsWith(calculator.opTimes(), 3)) {
    return false;
  }
  if (!base[1].isConstantNumber()) {
    return false;
  }
  bool isGood = base[1].isPositiveNumber() || base[2].isPositiveNumber();
  if (!isGood) {
    if (oneOverExponentE.isInteger()) {
      isGood = true;
    } else {
      Rational exponentRat;
      if (oneOverExponentE.isRational(&exponentRat)) {
        if (!exponentRat.getDenominator().isEven()) {
          isGood = true;
        }
      }
    }
  }
  if (!isGood) {
    return false;
  }
  Expression leftE, rightE;
  leftE.makeXOX(
    calculator, calculator.opSqrt(), oneOverExponentE, base[1]
  );
  rightE.makeXOX(
    calculator, calculator.opSqrt(), oneOverExponentE, base[2]
  );
  return output.makeXOX(calculator, calculator.opTimes(), leftE, rightE);
}

bool CalculatorFunctionsAlgebraic::isAlgebraicRadical(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsAlgebraic::isAlgebraicRadical");
  if (input.size() != 2) {
    return false;
  }
  int result = static_cast<int>(input[1].isAlgebraicRadical());
  return output.assignValue(calculator, result);
}

bool CalculatorFunctions::isReal(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::isReal");
  if (input.size() != 2) {
    return false;
  }
  if (!input[1].evaluatesToDouble()) {
    return output.assignValue(calculator, 0);
  }
  return output.assignValue(calculator, 1);
}

bool CalculatorFunctions::expressionToUTF8String(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::expressiontoUTF8String");
  if (input.size() > 2) {
    Expression argumentSequence = input;
    argumentSequence.setChildAtomValue(0, calculator.opSequence());
    return output.assignValue(calculator, argumentSequence.toUTF8String());
  }
  return output.assignValue(calculator, input[1].toUTF8String());
}

bool CalculatorFunctions::isProductTermsUpToPower(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::isProductTermsUpToPower");
  if (input.size() < 3) {
    return false;
  }
  Expression baseExpression;
  baseExpression = input[1];
  LargeInteger desiredMaxPower = 1;
  if (baseExpression.startsWith(calculator.opPower(), 3)) {
    if (baseExpression[2].isInteger(&desiredMaxPower)) {
      if (desiredMaxPower > 0) {
        baseExpression = input[1][1];
      } else {
        desiredMaxPower = 1;
      }
    } else {
      desiredMaxPower = 1;
    }
  }
  List<Expression> multiplicands;
  if (
    !calculator.collectOpands(
      input[2], calculator.opTimes(), multiplicands
    )
  ) {
    return
    calculator
    << "Could not extract multiplicands from: "
    << input[2].toString();
  }
  for (int k = 0; k < multiplicands.size; k ++) {
    List<List<Expression> > summands;
    if (
      !calculator.getSumProductsExpressions(multiplicands[k], summands)
    ) {
      return
      calculator
      << "Failed to extract sum from "
      << multiplicands[k].toString();
    }
    for (int i = 0; i < summands.size; i ++) {
      LargeInteger foundPower = 0;
      for (int j = 0; j < summands[i].size; j ++) {
        if (summands[i][j] == baseExpression) {
          foundPower ++;
          continue;
        }
        if (summands[i][j].startsWith(calculator.opPower(), 3)) {
          if (summands[i][j][1] == baseExpression) {
            LargeInteger localPower;
            if (summands[i][j][2].isInteger(&localPower)) {
              foundPower += localPower;
              continue;
            }
          }
        }
        if (!summands[i][j].evaluatesToDouble()) {
          return output.assignValue(calculator, 0);
        }
      }
      if (foundPower > desiredMaxPower) {
        return output.assignValue(calculator, 0);
      }
    }
  }
  return output.assignValue(calculator, 1);
}

bool CalculatorFunctions::scaleToLeadingUnit(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::scaleToLeadingUnit");
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  LinearCombination<Expression, Rational> collection;
  calculator.functionCollectSummandsCombine(calculator, argument, collection);
  collection /= collection.getLeadingCoefficient(nullptr);
  return output.makeSum(calculator, collection);
}

bool CalculatorFunctionsBinaryOps::powerRationalByRationalOutputAlgebraic(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsBinaryOps::powerRatByRatReducePrimeFactors");
  if (!input.startsWith(calculator.opPower(), 3)) {
    return false;
  }
  Rational exponent;
  if (!input[1].isOfType<Rational>()) {
    return false;
  }
  if (!input[2].isOfType<Rational>(&exponent)) {
    return false;
  }
  if (exponent.getDenominator() != 2) {
    return false;
  }
  Expression radical, reduced;
  radical.makeXOX(
    calculator,
    calculator.opSqrt(),
    calculator.expressionTwo(),
    input[1]
  );
  if (!CalculatorFunctions::sqrt(calculator, radical, reduced)) {
    return false;
  }
  if (!reduced.isOfType<AlgebraicNumber>()) {
    return false;
  }
  Expression integerPower;
  integerPower.assignValue(
    calculator, Rational(exponent.getNumerator())
  );
  return
  output.makeXOX(
    calculator, calculator.opPower(), reduced, integerPower
  );
}

bool CalculatorFunctions::newtonsMethod(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::newtonsMethod");
  if (input.size() != 4) {
    return false;
  }
  Expression functionExpression;
  if (
    !CalculatorFunctions::functionEqualityToArithmeticExpression(
      calculator, input[1], functionExpression
    )
  ) {
    functionExpression = input[1];
  }
  HashedList<Expression> variables;
  if (!functionExpression.getFreeVariables(variables, true)) {
    return
    calculator
    << "Failed to get free variables from: "
    << functionExpression.toString();
  }
  if (variables.size != 1) {
    return
    calculator
    << "While trying to extract a function from: "
    << functionExpression.toString()
    << ", got "
    << variables.size
    << " variables. Newton's method requires an expression that depends "
    << "on exactly one variable. The variables I got were: "
    << variables.toStringCommaDelimited();
  }
  int numIterations = - 1;
  if (!input[3].isSmallInteger(&numIterations)) {
    std::stringstream errorStream;
    errorStream
    << "While doing Newton's method, "
    << "could not extract a **small** integer from the third argument "
    << input[3].toString()
    << " of "
    << input.toString()
    << ". Please enter a number as the third argument of Newton's method. ";
    return output.assignError(calculator, errorStream.str());
  }
  if (numIterations < 1 || numIterations > 50) {
    std::stringstream errorStream;
    errorStream
    << "While doing Newton's method with the command: "
    << input.toString()
    << ", the third argument requests "
    << numIterations
    << " iterations. However, "
    << "the number of iterations is required "
    << "to be a number between 1 and 50. ";
    return output.assignError(calculator, errorStream.str());
  }
  MapList<
    std::string,
    Expression,
    HashFunctions::hashFunction<std::string>
  > substitution;
  substitution.setKeyValue("x", variables[0]);
  substitution.setKeyValue("f", functionExpression);
  substitution.setKeyValue("a", calculator.getNewAtom());
  substitution.setKeyValue("iteratedMap", calculator.getNewAtom());
  substitution.setKeyValue("NewtonMap", calculator.getNewAtom());
  substitution.setKeyValue("y", calculator.getNewAtom());
  substitution.setKeyValue("startingPoint", input[2]);
  substitution.setKeyValue("numIterations", input[3]);
  return
  output.assignStringParsed(
    "(NewtonMap{}{{a}} = "
    "DoubleValue( "
    "(iteratedMap =x- f/ Differentiate{}(x, f); "
    "x ={{a}}; "
    "iteratedMap "
    ")_3); "
    "y_{0} = startingPoint;"
    "y_{{a}} = NewtonMap{}(y_{a- 1});"
    "y_{numIterations})_4",
    &substitution,
    calculator
  );
}

bool CalculatorFunctions::valueOfModularExpression(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::valueOfModularExpression");
  if (input.size() != 2) {
    return false;
  }
  ElementZmodP candidateZmodP;
  if (input[1].isOfType(&candidateZmodP)) {
    return output.assignValue(calculator, candidateZmodP.value);
  }
  Polynomial<ElementZmodP> candidatePolynomial;
  if (input[1].isOfType(&candidatePolynomial)) {
    Polynomial<Rational> rationalConversion;
    for (int i = 0; i < candidatePolynomial.size(); i ++) {
      rationalConversion.addMonomial(
        candidatePolynomial.monomials[i],
        candidatePolynomial.coefficients[i].value
      );
    }
    return
    output.assignValueWithContext(
      calculator, rationalConversion, input[1].getContext()
    );
  }
  PolynomialModuloPolynomial<ElementZmodP> quotientElement;
  if (input[1].isOfType(&quotientElement)) {
    return
    output.assignValueWithContext(
      calculator, quotientElement.value, input[1].getContext()
    );
  }
  return false;
}

bool CalculatorFunctions::elementEllipticCurveNormalForm(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::elementEllipticCurveNormalForm");
  if (input.size() != 4) {
    return
    calculator
    << "Elliptic curve expects 3 arguments "
    << "(curve, generator letter, baseX and baseY) ";
  }
  const Expression& xDefE = input[2];
  const Expression& yDefE = input[3];
  //  HashedList<Expression> xEcandidates, yEcandidates;
  //  if (!xDefE.getFreeVariables(xEcandidates, false))
  // return calculator << "Failed to get free variables from " <<
  // xDefE.toString();
  //  if (!yDefE.getFreeVariables(yEcandidates, false))
  // return calculator << "Failed to get free variables from " <<
  // yDefE.toString();
  //  if (xEcandidates.size != 1 || yEcandidates.size != 1)
  // return calculator << "Couldn't get single free variable from " <<
  // xEcandidates.toStringCommaDelimited()
  //    << " and/or " << yEcandidates.toStringCommaDelimited();
  //  if (CalculatorFunctions::equalityToArithmeticExpression())
  if (!xDefE.startsWith(calculator.opDefine(), 3)) {
    return
    calculator
    << "Failed to extract variable form "
    << xDefE.toString();
  }
  if (!yDefE.startsWith(calculator.opDefine(), 3)) {
    return
    calculator
    << "Failed to extract variable form "
    << yDefE.toString();
  }
  ElementEllipticCurve<ElementZmodP> eltZmodP;
  ElementEllipticCurve<Rational> eltRational;
  eltZmodP.flagInfinity = false;
  eltRational.flagInfinity = false;
  bool isRational = true;
  bool isElementZmodP = true;
  if (!xDefE[2].isOfType(&eltRational.xCoordinate)) {
    isRational = false;
  }
  if (!yDefE[2].isOfType(&eltRational.yCoordinate)) {
    isRational = false;
  }
  if (!xDefE[2].isOfType(&eltZmodP.xCoordinate)) {
    isElementZmodP = false;
  }
  if (!yDefE[2].isOfType(&eltZmodP.yCoordinate)) {
    isElementZmodP = false;
  }
  if (!isRational && !isElementZmodP) {
    return
    calculator
    << "Could not extract rational or element of z mod p from "
    << xDefE[2].toString()
    << ", "
    << yDefE[2].toString();
  }
  if (isElementZmodP) {
    if (eltZmodP.xCoordinate.modulus != eltZmodP.yCoordinate.modulus) {
      return calculator << "The two base coordinates have different moduli. ";
    }
  }
  Expression curveE;
  if (
    !CalculatorFunctions::functionEqualityToArithmeticExpression(
      calculator, input[1], curveE
    )
  ) {
    return
    calculator
    << "Could not get arithmetic expression from: "
    << input[1].toString()
    << ". I was expecting a cubic equality.";
  }
  WithContext<Polynomial<Rational> > polynomialWithContext;
  if (
    !CalculatorConversions::functionPolynomial<Rational>(
      calculator, curveE, polynomialWithContext, - 1, - 1, false
    )
  ) {
    return calculator << "Could not get polynomial from " << curveE.toString();
  }
  ExpressionContext curveContext = polynomialWithContext.context;
  Polynomial<Rational> polynomial = polynomialWithContext.content;
  if (curveContext.numberOfVariables() != 2) {
    return
    calculator
    << "Expected 2 context variables in "
    << curveE.toString()
    << ", got context: "
    << curveContext.toString();
  }
  MonomialPolynomial leadingMonomial;
  List<MonomialPolynomial>::Comparator monomialOrder(
    MonomialPolynomial::greaterThan_totalDegree_rightSmallerWins
  );
  polynomial.getIndexLeadingMonomial(
    &leadingMonomial, nullptr, &monomialOrder
  );
  int indexX = 0;
  int indexY = 1;
  if (leadingMonomial[indexX] != 3) {
    MathRoutines::swap(indexX, indexY);
  }
  Expression xE = xDefE[1];
  Expression yE = yDefE[1];
  if (curveContext.getVariable(indexX) != xE) {
    MathRoutines::swap(xE, yE);
  }
  if (curveContext.getVariable(indexY) != yE) {
    return
    calculator
    << "Curve variable "
    << curveContext.getVariable(1).toString()
    << " not equal to "
    << yE.toString();
  }
  calculator
  << "Created elliptic curve "
  << polynomial.toString()
  << " = 0. The variables are assumed to be: x = "
  << xE.toString()
  << ", y = "
  << yE.toString();
  if (polynomial.size() > 4) {
    return
    calculator
    << "Elliptic curve allowed to have max 4 terms, yours has: "
    << polynomial.size();
  }
  MonomialPolynomial xCubed, xLinear, ySquared;
  xCubed.makeEi(indexX, 3);
  xLinear.makeEi(indexX, 1);
  ySquared.makeEi(indexY, 2);
  Rational coefficientY = polynomial.getCoefficientOf(ySquared);
  Rational coefficientXcubed = - polynomial.getCoefficientOf(xCubed);
  if (coefficientY == 0) {
    return calculator << "Did not find square term in your curve.";
  }
  if (coefficientXcubed == 0) {
    return calculator << "Did not find cube term in your curve.";
  }
  Rational coefficientXLinear = - polynomial.getCoefficientOf(xLinear);
  Rational constCoefficient = - polynomial.getConstantTerm();
  EllipticCurveWeierstrassNormalForm& curveConstants =
  isRational ? eltRational.owner : eltZmodP.owner;
  if (
    !coefficientXLinear.isInteger(&curveConstants.linearCoefficient) ||
    !constCoefficient.isInteger(&curveConstants.constantTerm)
  ) {
    return
    calculator
    << "At the moment only integer elliptic curve "
    "coefficients are supported. Your coefficients were: "
    << coefficientXLinear
    << ", "
    << constCoefficient
    << ". ";
  }
  int numberOfNonZeroGoodCoefficients = 2;
  if (coefficientXLinear != 0) {
    numberOfNonZeroGoodCoefficients ++;
  }
  if (constCoefficient != 0) {
    numberOfNonZeroGoodCoefficients ++;
  }
  if (numberOfNonZeroGoodCoefficients != polynomial.size()) {
    return
    calculator
    << "It appears your curve: "
    << curveE.toString()
    << " is not of the form y^2 = x^3 + ax + b. ";
  }
  ExpressionContext context(calculator);
  context.addVariable(xE);
  context.addVariable(yE);
  if (isRational) {
    return output.assignValueWithContext(calculator, eltRational, context);
  }
  if (isElementZmodP) {
    return output.assignValueWithContext(calculator, eltZmodP, context);
  }
  return false;
}

bool CalculatorFunctions::precomputeSemisimpleLieAlgebraStructure(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::precomputeSemisimpleLieAlgebraStructure");
  if (!global.response.monitoringAllowed()) {
    global.response.initiate(
      "Triggered by precomputeSemisimpleLieAlgebraStructure."
    );
  }
  if (!global.userDefaultHasAdminRights()) {
    return
    calculator
    << "Function CalculatorFunctions::precomputeSemisimpleLieAlgebraStructure "
    << "requires administrator access. "
    << "To get one, install the calculator on your machine.";
  }
  int startingIndex = 0;
  if (!input[1].isSmallInteger(&startingIndex)) {
    return
    calculator
    << "Argument of precomputeSemisimpleLieAlgebraStructure "
    << "not a small integer.";
  }
  List<DynkinType> allTypes;
  DynkinType::getPrecomputedDynkinTypes(allTypes);
  ProgressReport report;
  std::stringstream out;
  out
  << "Generated structure constants, "
  << "root subalgebras and sl(2) subalgebras for the following. ";
  if (startingIndex <= 0) {
    startingIndex = 0;
  }
  for (int i = startingIndex; i < allTypes.size; i ++) {
    std::stringstream reportStream;
    reportStream
    << "Computing structure of subalgebra "
    << allTypes[i].toString()
    << " ("
    << i + 1
    << " out of "
    << allTypes.size
    << ").";
    report.report(reportStream.str());
    SemisimpleLieAlgebra algebra;
    algebra.weylGroup.makeFromDynkinType(allTypes[i]);
    algebra.computeChevalleyConstants();
    SlTwoSubalgebras slTwoSubalgebras(algebra);
    slTwoSubalgebras.rootSubalgebras.flagPrintParabolicPseudoParabolicInfo =
    true;
    algebra.findSl2Subalgebras(
      algebra,
      slTwoSubalgebras,
      true,
      &calculator.objectContainer.algebraicClosure
    );
    slTwoSubalgebras.writeHTML();
    Plot plot;
    algebra.weylGroup.dynkinType.plot(plot);
    std::string plotDynkinType = plot.getPlotHtml2d(calculator);
    algebra.writeHTML(true, false, plotDynkinType);
    if (allTypes[i].hasPrecomputedSubalgebras()) {
      SemisimpleSubalgebras subalgebras;
      MapReferences<DynkinType, SemisimpleLieAlgebra> subalgebrasContainer;
      ListReferences<SlTwoSubalgebras> sl2Conainer;
      if (
        !subalgebras.computeStructureWriteFiles(
          algebra,
          calculator.objectContainer.algebraicClosure,
          subalgebrasContainer,
          sl2Conainer,
          CalculatorConversions::stringFromSemisimpleSubalgebras,
          nullptr,
          false,
          true,
          false,
          true,
          true,
          false,
          true,
          plotDynkinType
        )
      ) {
        out << "Failed to compute " << allTypes[i].toString();
      }
    }
    out << allTypes[i].toString();
    if (i != allTypes.size - 1) {
      out << ", ";
    }
  }
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctionsCrypto::showKnownObjectIds(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsCrypto::showKnownObjectIds");
  (void) input;
  return
  output.assignValue(
    calculator, ASNObject::toStringAllRecognizedObjectIds()
  );
}

bool CalculatorFunctionsCrypto::testASN1Decode(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::testASN1Decode");
  if (input.size() != 2) {
    return false;
  }
  std::string data;
  if (!input[1].isOfType(&data)) {
    return false;
  }
  AbstractSyntaxNotationOneSubsetDecoder decoder;
  List<unsigned char> dataList;
  dataList = data;
  std::stringstream commentsOnError;
  std::stringstream out;
  ASNElement result;
  if (!decoder.decode(dataList, 0, result, &commentsOnError)) {
    out << "Failed to decode.<br>" << commentsOnError.str();
  } else {
    List<unsigned char> recoded;
    result.writeBytesUpdatePromisedLength(recoded);
    std::string originalHex = Crypto::convertStringToHex(data, 0, false);
    std::string recodedHex = Crypto::convertListUnsignedCharsToHex(recoded);
    out
    << StringRoutines::Differ::differenceHTMLStatic(
      originalHex, recodedHex, "original", "recoded"
    );
    out << decoder.toStringAnnotateBinary();
  }
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctions::isInteger(
  Calculator& calculator, const Expression& input, Expression& output
) {
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  if (argument.hasBoundVariables()) {
    return false;
  }
  if (argument.isInteger()) {
    output.assignValue(calculator, 1);
  } else {
    output.assignValue(calculator, 0);
  }
  return true;
}

bool CalculatorFunctions::determinantPolynomial(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::determinantPolynomial");
  if (input.size() != 2) {
    return false;
  }
  Matrix<Polynomial<Rational> > matrixPolynomial;
  ExpressionContext context(calculator);
  if (
    !CalculatorConversions::functionGetMatrix(
      calculator, input[1], matrixPolynomial, true, &context, - 1
    )
  ) {
    return
    calculator
    << "<hr>Failed to convert the input to "
    << "matrix of polynomials. ";
  }
  if (matrixPolynomial.numberOfRows != matrixPolynomial.numberOfColumns) {
    return
    output.assignError(
      calculator, "<hr>Failed to compute determinant: matrix is non-square. "
    );
  }
  if (matrixPolynomial.numberOfRows > 8) {
    return
    calculator
    << "<hr>Failed to compute determinant: "
    << "matrix is larger than 8 x 8, and your matrix had "
    << matrixPolynomial.numberOfRows
    << " rows. Note that you can compute "
    << "determinant using the \\det function which "
    << "does Gaussian elimination "
    << "and will work for large rational matrices. "
    << "This function is meant to be used with honest "
    << "polynomial entries. ";
  }
  Polynomial<Rational> outputPoly;
  outputPoly.makeDeterminantFromSquareMatrix(matrixPolynomial);
  return output.assignValueWithContext(calculator, outputPoly, context);
}

bool CalculatorFunctions::generateMultiplicativelyClosedSet(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::generateMultiplicativelyClosedSet");
  if (input.size() <= 2) {
    return
    output.assignError(
      calculator,
      "I need at least two arguments - "
      "upper bound and at least one element to multiply. "
    );
  }
  int upperLimit;
  if (!input[1].isSmallInteger(&upperLimit)) {
    return
    output.assignError(
      calculator,
      "First argument must be a small integer, "
      "serving as upper bound for the set. "
    );
  }
  if (upperLimit <= 0) {
    upperLimit = 10000;
    calculator
    << "The upper computation limit I got was 0 or less; "
    << "I replaced it with the default value "
    << upperLimit
    << ".";
  }
  HashedList<Expression> set;
  set.setExpectedSize(input.size() - 2);
  for (int i = 2; i < input.size(); i ++) {
    set.addOnTop(input[i]);
  }
  int numGenerators = set.size;
  Expression product, evaluatedProduct;
  ProgressReport report;
  for (int i = 0; i < set.size; i ++) {
    for (int j = 0; j < numGenerators; j ++) {
      product.makeProduct(calculator, set[j], set[i]);
      std::stringstream reportStream;
      reportStream
      << "found "
      << set.size
      << "elements so far, exploring element "
      << i + 1;
      reportStream << "<br>Evaluating: " << product.toString();
      report.report(reportStream.str());
      calculator.evaluateExpression(calculator, product, evaluatedProduct);
      set.addOnTopNoRepetition(evaluatedProduct);
      if (set.size > upperLimit) {
        std::stringstream out;
        out
        << "<hr>While generating multiplicatively closed set, "
        << "I went above the upper limit of "
        << upperLimit
        << " elements.";
        evaluatedProduct.assignError(calculator, out.str());
        set.addOnTop(evaluatedProduct);
        i = set.size;
        break;
      }
    }
  }
  calculator << "<hr>Generated a list of " << set.size << " elements";
  output.reset(calculator, set.size + 1);
  output.addChildAtomOnTop(calculator.opSequence());
  for (int i = 0; i < set.size; i ++) {
    output.addChildOnTop(set[i]);
  }
  return true;
}

template < >
bool Matrix<Rational>::jordanNormalForm(
  Matrix<AlgebraicNumber>& outputLeft,
  Matrix<AlgebraicNumber>& outputDiagonalized,
  Matrix<AlgebraicNumber>& outputRight,
  AlgebraicClosureRationals& ownerField,
  std::stringstream* comments
) {
  Matrix<RationalFraction<Rational> > characteristicMatrix;
  characteristicMatrix = *this;
  Polynomial<Rational> minusLambda;
  minusLambda.makeMonomial(0, 1, - 1);
  int dimension = this->numberOfColumns;
  for (int i = 0; i < dimension; i ++) {
    characteristicMatrix(i, i) += minusLambda;
  }
  RationalFraction<Rational> characteristicPolynomialComputer;
  characteristicMatrix.computeDeterminantOverwriteMatrix(
    characteristicPolynomialComputer
  );
  Polynomial<Rational> characteristicPolynomial;
  if (
    !characteristicPolynomialComputer.isPolynomial(&characteristicPolynomial)
  ) {
    // We have made a programming error.
    global.fatal
    << "The characteristic polynomial "
    << "should not be an honest rational fraction. "
    << global.fatal;
  }
  PolynomialSolverWithQuadraticRadicalsUnivariate solver(ownerField);
  if (
    !solver.solvePolynomialWithRadicals(characteristicPolynomial, comments)
  ) {
    return false;
  }
  Matrix<AlgebraicNumber> diagonalizer;
  diagonalizer.makeZeroMatrix(dimension);
  List<Vector<AlgebraicNumber> > allEigenVectors;
  for (int i = 0; i < solver.solutions.size; i ++) {
    List<Vector<AlgebraicNumber> > currentEigenVectors;
    diagonalizer = *this;
    diagonalizer.getEigenspaceModifyMe(
      solver.solutions[i], currentEigenVectors
    );
    allEigenVectors.addListOnTop(currentEigenVectors);
  }
  if (allEigenVectors.size != dimension) {
    if (comments != nullptr) {
      *comments
      << "The Jordan normal form of the matrix is not diagonal; "
      << "diagonalization is not implemented yet.";
    }
    return false;
  }
  outputLeft.makeZeroMatrix(dimension);
  outputDiagonalized.makeZeroMatrix(dimension);
  outputRight.makeZeroMatrix(dimension);
  for (int i = 0; i < dimension; i ++) {
    for (int j = 0; j < dimension; j ++) {
      outputLeft(i, j) = allEigenVectors[j][i];
    }
  }
  outputRight = outputLeft;
  if (!outputRight.invert()) {
    global.fatal
    << "The eigenvector matrix must be invertible."
    << global.fatal;
  }
  outputDiagonalized = *this;
  outputDiagonalized = outputRight;
  Matrix<AlgebraicNumber> converted;
  converted = *this;
  outputDiagonalized *= converted;
  outputDiagonalized *= outputLeft;
  return true;
}

bool CalculatorFunctionsLinearAlgebra::diagonalizeMatrix(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsLinearAlgebra::diagonalizeMatrix");
  if (input.size() != 2) {
    return false;
  }
  Matrix<Rational> matrix;
  if (
    !CalculatorConversions::functionGetMatrix(
      calculator, input[1], matrix, false
    )
  ) {
    return false;
  }
  if (!matrix.isSquare()) {
    return
    calculator
    << "Diagonalization (Jordan normal form) "
    << "is allowed only for square matrices. ";
  }
  Matrix<AlgebraicNumber> eigenMatrix, jordanNormalForm, eigenMatrixInverted;
  std::stringstream comments;
  if (
    !matrix.jordanNormalForm(
      eigenMatrix,
      jordanNormalForm,
      eigenMatrixInverted,
      calculator.objectContainer.algebraicClosure,
      &comments
    )
  ) {
    return
    calculator
    << "Failed to compute the Jordan normal form."
    << comments.str();
  }
  List<Expression> result;
  Expression
  eigenMatrixExpression,
  jordanNormalFormExpression,
  eigenMatrixInvertedExpression;
  eigenMatrixExpression.makeMatrix(calculator, eigenMatrix);
  jordanNormalFormExpression.makeMatrix(calculator, jordanNormalForm);
  eigenMatrixInvertedExpression.makeMatrix(calculator, eigenMatrixInverted);
  result.addOnTop(eigenMatrixExpression);
  result.addOnTop(jordanNormalFormExpression);
  result.addOnTop(eigenMatrixInvertedExpression);
  return output.makeSequence(calculator, &result);
}

bool CalculatorFunctionsLinearAlgebra::functionToMatrix(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctionsLinearAlgebra::functionToMatrix");
  if (!input.isListNElements(4)) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& middleE = input[2];
  const Expression& rightE = input[3];
  int numberOfRows, numberOfColumns;
  if (
    !middleE.isIntegerFittingInInt(&numberOfRows) ||
    !rightE.isIntegerFittingInInt(&numberOfColumns)
  ) {
    return false;
  }
  if (numberOfRows <= 0 || numberOfColumns <= 0) {
    return false;
  }
  LargeInteger numberOfRowsTimesColumns = numberOfRows;
  numberOfRowsTimesColumns *= numberOfColumns;
  if (numberOfRowsTimesColumns > 10000) {
    calculator
    << "Max number of matrix entries is 10000. You requested "
    << numberOfRows
    << " rows and "
    << numberOfColumns
    << " columns, total: "
    << numberOfRowsTimesColumns.toString()
    << " entries<br>";
    return false;
  }
  Matrix<Expression> resultMat;
  resultMat.initialize(numberOfRows, numberOfColumns);
  Expression leftIE, rightIE;
  for (int i = 0; i < numberOfRows; i ++) {
    for (int j = 0; j < numberOfColumns; j ++) {
      leftIE.assignValue(calculator, i + 1);
      rightIE.assignValue(calculator, j + 1);
      resultMat.elements[i][j].reset(calculator, 3);
      resultMat.elements[i][j].addChildOnTop(leftE);
      resultMat.elements[i][j].addChildOnTop(leftIE);
      resultMat.elements[i][j].addChildOnTop(rightIE);
    }
  }
  return output.assignMatrixExpressions(resultMat, calculator, true, true);
}

bool CalculatorFunctions::suffixNotationForPostScript(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::suffixNotationForPostScript");
  RecursionDepthCounter counter(&calculator.recursionDepth);
  if (*counter.counter == calculator.maximumRecursionDepth - 2) {
    return output.assignValue(calculator, std::string("..."));
  }
  std::string currentString;
  if (input.isOperation(&currentString)) {
    if (input.toString() == "e") {
      return output.assignValue(calculator, std::string(" 2.718281828 "));
    }
    if (input.toString() == "\\pi") {
      return output.assignValue(calculator, std::string(" 3.141592654 "));
    }
    if (input.data >= calculator.numberOfPredefinedAtoms) {
      return output.assignValue(calculator, currentString);
    }
    if (currentString == "|") {
      return output.assignValue(calculator, std::string("abs "));
    }
    if (currentString == "+") {
      return output.assignValue(calculator, std::string("add "));
    }
    if (currentString == "*") {
      return output.assignValue(calculator, std::string("mul "));
    }
    if (currentString == "-") {
      return output.assignValue(calculator, std::string("sub "));
    }
    if (currentString == "/") {
      return output.assignValue(calculator, std::string("div "));
    }
    if (currentString == "^") {
      return output.assignValue(calculator, std::string("exp "));
    }
    if (currentString == "\\log") {
      return output.assignValue(calculator, std::string("ln "));
    }
    if (currentString == "\\sin") {
      return
      output.assignValue(calculator, std::string(" 57.29578 mul sin "));
    }
    if (currentString == "\\cos") {
      return
      output.assignValue(calculator, std::string(" 57.29578 mul cos "));
    }
    if (currentString == "\\tan") {
      return
      output.assignValue(calculator, std::string(" 57.29578 mul tan "));
    }
    if (currentString == "\\arctan") {
      return output.assignValue(calculator, std::string("ATAN "));
    }
    if (currentString == "\\arcsin") {
      return output.assignValue(calculator, std::string("ASIN "));
    }
    if (currentString == "\\arccos") {
      return output.assignValue(calculator, std::string("ACOS "));
    }
    if (currentString == "\\sqrt") {
      return output.assignValue(calculator, std::string("sqrt "));
    }
    return
    output.assignError(
      calculator, "Cannot convert " + currentString + " to suffix notation."
    );
  }
  std::stringstream out;
  out.precision(7);
  bool hasDoubleValue = false;
  double doubleValue = - 1;
  Rational rational;
  if (input.isOfType<Rational>(&rational)) {
    if (
      rational.getDenominator().isIntegerFittingInInt(nullptr) &&
      rational.getNumerator().isIntegerFittingInInt(nullptr)
    ) {
      out
      << " "
      << rational.getNumerator().toString()
      << " "
      << rational.getDenominator()
      << " div ";
      return output.assignValue(calculator, out.str());
    }
    hasDoubleValue = true;
    doubleValue = input.getValue<Rational>().getDoubleValue();
  }
  if (input.isOfType<AlgebraicNumber>()) {
    hasDoubleValue =
    input.getValue<AlgebraicNumber>().evaluatesToDouble(&doubleValue);
  }
  if (input.isOfType<double>()) {
    hasDoubleValue = true;
    doubleValue = input.getValue<double>();
  }
  if (hasDoubleValue) {
    out << " " << FloatingPoint::doubleToString(doubleValue);
    return output.assignValue(calculator, out.str());
  }
  Expression currentE;
  bool useUsualOrder = !input[0].isOperationGiven(calculator.opDivide()) &&
  !input[0].isOperationGiven(calculator.opPower());
  if (useUsualOrder) {
    for (int i = input.size() - 1; i >= 1; i --) {
      if (
        !CalculatorFunctions::suffixNotationForPostScript(
          calculator, input[i], currentE
        )
      ) {
        return
        output.assignError(
          calculator, "Failed to convert " + input[i].toString()
        );
      }
      if (!currentE.isOfType(&currentString)) {
        return
        output.assignError(
          calculator, "Failed to convert " + input[i].toString()
        );
      }
      out << currentString << " ";
    }
  } else {
    for (int i = 1; i < input.size(); i ++) {
      if (
        !CalculatorFunctions::suffixNotationForPostScript(
          calculator, input[i], currentE
        )
      ) {
        return
        output.assignError(
          calculator, "Failed to convert " + input[i].toString()
        );
      }
      if (!currentE.isOfType(&currentString)) {
        return
        output.assignError(
          calculator, "Failed to convert " + input[i].toString()
        );
      }
      out << currentString << " ";
    }
  }
  if (
    !CalculatorFunctions::suffixNotationForPostScript(
      calculator, input[0], currentE
    )
  ) {
    return
    output.assignError(
      calculator, "Failed to convert " + input[0].toString()
    );
  }
  if (!currentE.isOfType(&currentString)) {
    return
    output.assignError(
      calculator, "Failed to convert " + input[0].toString()
    );
  }
  out << currentString << " ";
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctions::isRational(
  Calculator& calculator, const Expression& input, Expression& output
) {
  if (input.size() != 2) {
    return false;
  }
  const Expression& argument = input[1];
  if (argument.hasBoundVariables()) {
    return false;
  }
  if (argument.isRational()) {
    output.assignValue(calculator, 1);
  } else {
    output.assignValue(calculator, 0);
  }
  return true;
}

bool CalculatorFunctions::freudenthalFull(
  Calculator& calculator, const Expression& input, Expression& output
) {
  Vector<Rational> hwFundamental, hwSimple;
  Selection tempSel;
  WithContext<SemisimpleLieAlgebra*> semisimpleLieAlgebra;
  if (
    !calculator.getTypeHighestWeightParabolic<Rational>(
      calculator,
      input,
      output,
      hwFundamental,
      tempSel,
      semisimpleLieAlgebra
    )
  ) {
    return
    output.assignError(
      calculator, "Failed to extract highest weight and algebra"
    );
  }
  if (output.isError()) {
    return true;
  }
  if (tempSel.cardinalitySelection > 0) {
    return
    output.assignError(calculator, "Failed to extract highest weight. ");
  }
  CharacterSemisimpleLieAlgebraModule<Rational> startingChar, resultChar;
  hwSimple =
  semisimpleLieAlgebra.content->weylGroup.getSimpleCoordinatesFromFundamental(
    hwFundamental, Rational::zero()
  );
  startingChar.makeFromWeight(hwSimple, semisimpleLieAlgebra.content);
  std::string reportString;
  if (
    !startingChar.freudenthalEvaluateMeFullCharacter(
      resultChar, 10000, &reportString
    )
  ) {
    return output.assignError(calculator, reportString);
  }
  std::stringstream out;
  out << resultChar.toString();
  return output.assignValue(calculator, out.str());
}

bool CalculatorFunctions::freudenthalFormula(
  Calculator& calculator, const Expression& input, Expression& output
) {
  Vector<Rational> hwFundamental, hwSimple;
  Selection tempSel;
  WithContext<SemisimpleLieAlgebra*> semisimpleLieAlgebra;
  if (
    !calculator.getTypeHighestWeightParabolic<Rational>(
      calculator,
      input,
      output,
      hwFundamental,
      tempSel,
      semisimpleLieAlgebra
    )
  ) {
    return
    output.assignError(
      calculator, "Failed to extract highest weight and algebra"
    );
  }
  if (output.isError()) {
    return true;
  }
  if (tempSel.cardinalitySelection > 0) {
    return
    output.assignError(calculator, "Failed to extract highest weight. ");
  }
  CharacterSemisimpleLieAlgebraModule<Rational> startingChar, resultChar;
  hwSimple =
  semisimpleLieAlgebra.content->weylGroup.getSimpleCoordinatesFromFundamental(
    hwFundamental, Rational::zero()
  );
  startingChar.makeFromWeight(hwSimple, semisimpleLieAlgebra.content);
  std::string reportString;
  if (
    !startingChar.freudenthalEvalMeDominantWeightsOnly(
      resultChar, 10000, &reportString
    )
  ) {
    return output.assignError(calculator, reportString);
  }
  return output.assignValue(calculator, resultChar);
}

std::string StringRoutines::Conversions::stringToCalculatorDisplay(
  const std::string& input
) {
  if (StringRoutines::Conversions::isValidUtf8(input)) {
    return input;
  }
  return
  "\"" +
  StringRoutines::Conversions::escapeJavascriptLike(input) +
  "\"";
}

std::string StringRoutines::convertByteToHex(unsigned char byte) {
  std::stringstream out;
  out << std::setfill('0') << std::setw(2) << std::hex << static_cast<unsigned>
  (byte);
  return out.str();
}

std::string StringRoutines::convertStringToHexIfNonReadable(
  const std::string& input, int lineWidthZeroForNone, bool useHTML
) {
  bool foundBad = false;
  for (unsigned i = 0; i < input.size(); i ++) {
    // All characters smaller than \t = 9
    // and all characters larger than 127
    // are considered non-standard.
    if (
      static_cast<unsigned>(input[i]) < 9 || static_cast<unsigned>(input[i]) >
      127
    ) {
      foundBad = true;
      break;
    }
  }
  if (!foundBad) {
    return input;
  }
  return Crypto::convertStringToHex(input, lineWidthZeroForNone, useHTML);
}

bool StringRoutines::isLatinLetterSequence(const std::string& input) {
  for (unsigned i = 0; i < input.size(); i ++) {
    if (!MathRoutines::isLatinLetter(input[i])) {
      return false;
    }
  }
  return true;
}

bool StringRoutines::isLatinLetterOrDigitSequence(const std::string& input) {
  for (unsigned i = 0; i < input.size(); i ++) {
    if (
      !MathRoutines::isLatinLetter(input[i]) &&
      !MathRoutines::isDigit(input[i])
    ) {
      return false;
    }
  }
  return true;
}

bool CalculatorFunctions::jacobiSymbol(
  Calculator& calculator, const Expression& input, Expression& output
) {
  STACK_TRACE("CalculatorFunctions::jacobiSymbol");
  global.fatal << "Function not implemented yet." << global.fatal;
  (void) calculator;
  (void) output;
  if (input.size() != 3) {
    return false;
  }
  const Expression& leftE = input[1];
  const Expression& rightE = input[2];
  int leftInt, rightInt;
  if (
    !leftE.isSmallInteger(&leftInt) || !rightE.isSmallInteger(&rightInt)
  ) {
    return false;
  }
  return true;
}
